// @platform-split graphics: both builds redirect pinned 5.43 calls and OpenGL's import slots; Windows wraps the decoration pass's call, Mac starts at its first sprite and ends after each plane draw, and the frame limiter's GetTickCount and Sleep are Mac's gettimeofday and nanosleep.
#if defined(_WIN32) || defined(__APPLE__)
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jazzy_api.h"
#include "game_addresses.h"
#include "graphics.h"
#include "fps_version.h"

static const unsigned char expected_sha256[32]={
    0x48,0x9d,0x0b,0x82,0xfa,0x79,0x0e,0xc3,0x6e,0x69,0xa2,0x3e,0x23,0x35,0xd0,0xc2,
    0x04,0x8f,0xa5,0x7f,0x40,0x51,0xfe,0x81,0x59,0xc6,0x4f,0x2d,0x33,0x50,0x8e,0x55
};
#define FPS_PATH_SEPARATOR '\\'
#define FPS_GAME_MODULE() GetModuleHandleW(NULL)
#else
#define PLATFORM_NO_DLLMAIN
#include <limits.h>
#include <stddef.h>
#include "platform_mac.h"
#include "jazzy_api.h"
#include "game_addresses.h"
#include "graphics.h"
#include "fps_version.h"

static const unsigned char expected_sha256[32]={
    0x69,0xa5,0xfc,0x67,0xde,0xe8,0x09,0x7f,0x5c,0x29,0xd0,0x9d,0x4d,0x06,0xca,0xb0,
    0xda,0x21,0x36,0x31,0xca,0x5f,0xa0,0x94,0x3e,0x2a,0x3a,0x8f,0xca,0x00,0x91,0xf4
};
#define FPS_PATH_SEPARATOR '/'
#define FPS_GAME_MODULE() GetModuleHandleA(NULL)
#endif
static const PluginApi *plugin_api;
static unsigned char *detail_base;
static _Thread_local int in_highest;
static long long now_ticks(void) {return 0;}
#include "game_guard.h"
#include "patch_calls.h"
#include "gl_platform.h"
#include "gl_batch_lab.h"
#include "gl_texture_array_lab.h"
#include "gl_arrays_lab.h"
#ifdef _WIN32
#include "terrain_batch.h"
#endif
#include "gl_hook_guard.h"
#ifdef COE5SE_GRAPHICS_ORACLE
#include "tests/image_oracle.h"
#endif

typedef void (*ModDecorFn)(int,int,uintptr_t,float,float,float,float,float);
#ifdef _WIN32
static void (*mod_original_highest)(void);
static void (*mod_original_high)(void);
#else
static void (*mod_original_plane)(uintptr_t,uintptr_t,uintptr_t);
static int mod_pass_open,mod_pass_skipped;
#endif
static ModDecorFn mod_original_decor;
static int mod_operational,mod_reported_ready,mod_reported_graphics;
static const int *mod_render_setting;
static unsigned mod_reported_reasons;
static int mod_reported_setting=-1;
static unsigned mod_reported_qualities;
static _Thread_local int mod_scene_kind,mod_scene_drawing;
static int mod_scenes_enabled;

static void mod_log(const char *line) {
    if(plugin_api && plugin_api->logLine)plugin_api->logLine((const void *)mod_log,line);
}
#include "graphics_settings.h"
static void mod_log_graphics(void) {
    if(mod_reported_graphics || !ta_current_context || !ta_get_string || !ta_current_context())return;
    const char *vendor=(const char *)ta_get_string(0x1f00),*renderer=(const char *)ta_get_string(0x1f01),*driver=(const char *)ta_get_string(0x1f02);
    if(!vendor || !renderer || !driver)return;
    int major=0,minor=0,profile=0,layers=0;sscanf(driver,"%d.%d",&major,&minor);
    const char *glsl=major>=2?(const char *)ta_get_string(0x8b8c):NULL;
    if(major>3 || (major==3 && minor>=2))ta_get_int(0x9126,&profile);
    if(major>=3)ta_get_int(0x88ff,&layers);
    char line[2300];
    snprintf(line,sizeof(line),"fps_mod graphics version=" FPS_MOD_VERSION " vendor=\"%.200s\" renderer=\"%.300s\" opengl_driver=\"%.600s\" glsl=\"%.300s\" profile=%s max_array_layers=%d cache_limit_mib=128",
        vendor,renderer,driver,glsl?glsl:"unavailable",profile&2?"compatibility":(profile&1?"core":"legacy"),layers);
    mod_log(line);mod_reported_graphics=1;
}
static int (*mod_next_main_menu_wait)(int,int);
static int mod_main_menu_wait(int a,int b) {fps_apply_frame_preference();mod_log_graphics();return mod_next_main_menu_wait(a,b);}
static void mod_log_fallbacks(void) {
    unsigned fresh=fps_fallback_reasons&~mod_reported_reasons;
    if(!fresh)return;
    for(unsigned i=0;i<FPS_REASON_COUNT;i++)if(fresh&(1u<<i)) {
        char line[1400];
        snprintf(line,sizeof(line),"fps_mod original_path reason=%s rejected_textures=%lld cache_mib=%.2f note=unsupported_draws_use_original_renderer",
            fps_fallback_name((FpsFallbackReason)i),ta_rejected,(double)ta_sprite_cache.bytes/(1024.0*1024.0));
        mod_log(line);
        if(i==FPS_SHADER_COMPILE || i==FPS_SHADER_LINK || i==FPS_ENTRY_POINT) {
            snprintf(line,sizeof(line),"fps_mod renderer_detail %.1023s",ta_status);mod_log(line);
        }
    }
    mod_reported_reasons|=fresh;
}
#ifdef _WIN32
#include "map_texture_work.h"
static unsigned terrain_reported;
// Yield to plugins that take over a participating graphics import after startup.
static int map_owns_imports(void) {
    if(*gl_import_slot(FPS_GLU_SLOT)!=(void *)arrays_gluBuild2DMipmaps)return 0;
    for(size_t i=0;i<ARRAY_IMPORT_COUNT;i++)
        if(*gl_import_slot(arrays_imports[i].rva)!=arrays_imports[i].hook)return 0;
    return 1;
}
static void mod_terrain_finish(unsigned view,long long before) {
    terrain_end_scope();
    if((terrain_reported&view) || terrain_tiles==before)return;
    char line[200];
    snprintf(line,sizeof(line),"fps_mod terrain_ready view=%s tiles=%lld cache_mib=%.2f original_mips=1 original_order=1",
        view==1?"map":view==2?"battle":"location",terrain_tiles-before,ta_terrain_cache.bytes/(1024.0*1024.0));
    mod_log(line);terrain_reported|=view;
}
static void mod_terrain_plane(uintptr_t a,unsigned plane) {
    int opened=mod_operational && (!mod_render_setting || *mod_render_setting) && terrain_begin_scope();
    long long before=terrain_tiles;
    int previous=map_texture_pass;
    HGLRC current=map_textures_enabled && ta_current_context?ta_current_context():NULL;
    int locked=map_textures_enabled && TryAcquireSRWLockExclusive(&map_work_lock);
    int prepare=current && locked && opened && map_owns_imports() && ta_initialize_context();
    if(locked)map_sync_reserve_context(current);
    map_texture_pass=prepare?MAP_TERRAIN:0;
    if(prepare){
        map_copy_ticks=0;
        map_reserve_begin(plane);
    }
    ((void (*)(uintptr_t,unsigned))(detail_base+RVA_MAP_DRAW_PLANE))(a,plane);
    map_texture_pass=previous;
    if(opened)mod_terrain_finish(1,before);
    if(prepare)map_reserve_end();
    if(locked)ReleaseSRWLockExclusive(&map_work_lock);
}
static void mod_terrain_battle(void) {
    int opened=mod_scenes_enabled && mod_scene_kind && terrain_begin_scope();
    long long before=terrain_tiles;
    ((void (*)(void))(detail_base+RVA_BATTLE_BACKGROUND))();
    if(opened)mod_terrain_finish(mod_scene_kind==1?2:4,before);
}

// @platform-split map-units: the complete draw call is mapped on Windows.
static void mod_map_units(int a,int b,int flags) {
    void (*draw)(int,int,int)=(void (*)(int,int,int))(detail_base+RVA_MAP_DRAW_UNITS);
    if(!mod_operational || (mod_render_setting && !*mod_render_setting) || in_highest || terrain_scope || mod_scene_drawing ||
       !ta_current_context || !ta_initialize_context()){
        draw(a,b,flags);return;
    }
    int samples=0;arrays_get_int(0x80a8,&samples);
    int previous_bounds=map_bounds_active;
    int locked=map_textures_enabled && map_owns_imports() && TryAcquireSRWLockExclusive(&map_work_lock);
    map_bounds_active=locked;
    if(samples){
        draw(a,b,flags);
        map_bounds_active=previous_bounds;
        if(locked)ReleaseSRWLockExclusive(&map_work_lock);
        return;
    }
    array_texture_begin_frame();arrays_begin_frame();
    in_highest=decor_scope=arrays_scene_scope=ta_scene_replace=1;
    map_texture_pass=map_bounds_active?MAP_UNITS:0;
    if(map_texture_pass)map_copy_ticks=0;
    draw(a,b,flags);
    map_texture_pass=0;
    map_bounds_active=previous_bounds;
    arrays_end_frame();
    in_highest=decor_scope=arrays_scene_scope=ta_scene_replace=0;
    if(locked)ReleaseSRWLockExclusive(&map_work_lock);
}
#endif
#include "scene_batch.h"
static void mod_pointer_module(void *address,char *out,size_t size) {
    HMODULE module=NULL;char path[MAX_PATH];
    if(!address || !GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,(LPCSTR)address,&module)) {
        snprintf(out,size,"unmapped_or_unknown");return;
    }
    DWORD length=GetModuleFileNameA(module,path,MAX_PATH);
    if(!length || length>=MAX_PATH){snprintf(out,size,"module_path_unavailable");return;}
    const char *name=strrchr(path,FPS_PATH_SEPARATOR);name=name?name+1:path;
    snprintf(out,size,"%s+0x%llx",name,(unsigned long long)((uintptr_t)address-(uintptr_t)module));
}
static void mod_import_problem(const char *name,uintptr_t rva,void *actual,void *expected) {
    char owner[340],expected_owner[340],line[1000];
    mod_pointer_module(actual,owner,sizeof(owner));mod_pointer_module(expected,expected_owner,sizeof(expected_owner));
    snprintf(line,sizeof(line),"fps_mod import_check reason=%s function=%s import_rva=0x%llx actual=%s expected=%s imports_unchanged=1",
        expected?"graphics_import_conflict":"required_export_unavailable",name,(unsigned long long)rva,owner,expected_owner);
    mod_log(line);
}
#ifdef _WIN32
static void mod_decor(int file,int sprite,uintptr_t unused,float x,float y,float width,float height,float skew) {
    int previous=decor_scope;
    decor_scope=in_highest;
    mod_original_decor(file,sprite,unused,x,y,width,height,skew);
    decor_scope=previous;
}
static void mod_render_pass(void (*draw)(void),const char *path) {
    if(mod_scene_drawing){draw();return;}
    mod_log_graphics();
    int enabled=mod_operational && (!mod_render_setting || *mod_render_setting);
    if(mod_reported_setting!=enabled) {
        mod_reported_setting=enabled;
        mod_log(enabled?"fps_mod setting enabled=1":"fps_mod setting enabled=0");
    }
    if(!mod_operational || !enabled) {
        draw();return;
    }
    int quality=*(int *)(detail_base+RVA_GRAPHICS_DETAIL);
    unsigned bit=quality>=9 && quality<=13?1u<<(quality-9):0;
    if(bit && !(mod_reported_qualities&bit)){
        char line[200];snprintf(line,sizeof(line),"fps_mod map_renderer quality=%d path=%s batching_requested=1 quality_unchanged=1",quality,path);mod_log(line);mod_reported_qualities|=bit;
    }
    array_texture_begin_frame();arrays_begin_frame();
    int previous_terrain=terrain_scope;
    terrain_scope=0;
    in_highest=1;
#ifdef COE5SE_GRAPHICS_ORACLE
    graphics_test_render_pass(draw);
#else
    draw();
#endif
    arrays_end_frame();in_highest=0;decor_scope=0;terrain_scope=previous_terrain;
    if(!mod_reported_ready && ta_context_ok && ta_uploads) {
        char line[384];mod_reported_ready=1;
        snprintf(line,sizeof(line),"fps_mod renderer_ready version=" FPS_MOD_VERSION " game=5.43 api=%d quality=%d path=%s original_uv=1 original_mips=1 quality_unchanged=1 multi_page=%d state_tracking=%d uploads=%lld cache_mib=%.2f",PLUGIN_API_VERSION,quality,path,ta_multi_page,ta_track_state,ta_uploads,(double)ta_sprite_cache.bytes/(1024.0*1024.0));
        mod_log(line);
    }
    mod_log_fallbacks();
}
static void mod_highest(void){mod_render_pass(mod_original_highest,"highest_ultra");}
static void mod_high(void){mod_render_pass(mod_original_high,"low_medium_high");}
#else
static void mod_pass_begin(const char *path) {
    mod_log_graphics();
    int enabled=mod_operational && (!mod_render_setting || *mod_render_setting);
    if(mod_reported_setting!=enabled) {
        mod_reported_setting=enabled;
        mod_log(enabled?"fps_mod setting enabled=1":"fps_mod setting enabled=0");
    }
    if(!enabled){mod_pass_skipped=1;return;}
    int quality=*(int *)(detail_base+RVA_GRAPHICS_DETAIL);
    unsigned bit=quality>=9 && quality<=13?1u<<(quality-9):0;
    if(bit && !(mod_reported_qualities&bit)){
        char line[200];snprintf(line,sizeof(line),"fps_mod map_renderer quality=%d path=%s batching_requested=1 quality_unchanged=1",quality,path);mod_log(line);mod_reported_qualities|=bit;
    }
    array_texture_begin_frame();arrays_begin_frame();
    in_highest=1;mod_pass_open=1;
}
static void mod_pass_end(void) {
    arrays_end_frame();in_highest=0;decor_scope=0;mod_pass_open=0;
    if(!mod_reported_ready && ta_context_ok && ta_uploads) {
        char line[384];mod_reported_ready=1;
        int quality=*(int *)(detail_base+RVA_GRAPHICS_DETAIL);
        snprintf(line,sizeof(line),"fps_mod renderer_ready version=" FPS_MOD_VERSION " game=5.43 api=%d quality=%d path=%s original_uv=1 original_mips=1 quality_unchanged=1 multi_page=%d state_tracking=%d uploads=%lld cache_mib=%.2f",PLUGIN_API_VERSION,quality,quality>=12?"highest_ultra":"low_medium_high",ta_multi_page,ta_track_state,ta_uploads,(double)ta_sprite_cache.bytes/(1024.0*1024.0));
        mod_log(line);
    }
    mod_log_fallbacks();
}
static void mod_decor_sprite(const char *path,int file,int sprite,uintptr_t unused,float x,float y,float width,float height,float skew) {
    if(mod_scene_drawing){mod_original_decor(file,sprite,unused,x,y,width,height,skew);return;}
    if(mod_operational && !mod_pass_open && !mod_pass_skipped)mod_pass_begin(path);
    decor_scope=in_highest;
    mod_original_decor(file,sprite,unused,x,y,width,height,skew);
    decor_scope=0;
}
static void mod_decor(int file,int sprite,uintptr_t unused,float x,float y,float width,float height,float skew) {
    mod_decor_sprite("highest_ultra",file,sprite,unused,x,y,width,height,skew);
}
static void mod_decor_low(int file,int sprite,uintptr_t unused,float x,float y,float width,float height,float skew) {
    mod_decor_sprite("low_medium_high",file,sprite,unused,x,y,width,height,skew);
}
static void mod_plane_draw(uintptr_t a,uintptr_t b,uintptr_t c) {
    mod_original_plane(a,b,c);
    if(mod_pass_open)mod_pass_end();
    mod_pass_skipped=0;
}
#endif
// Feature groups selected once at launch.
enum { MOD_RENDERING=1, MOD_FRAME_CONTROLS=2, MOD_SCENES=4, MOD_GUI=8, MOD_MAP_TEXTURES=32 };
#ifdef _WIN32
typedef struct {
    unsigned group;
    uintptr_t site,target;
    void (*hook)(void);
    unsigned char original[5];
} ModCall;
typedef struct { unsigned char *page;SIZE_T size;DWORD original; } ModCodePage;
#include "gui_batch.h"
static unsigned mod_features;
static ModCall mod_calls[]={
    {MOD_MAP_TEXTURES,RVA_MAP_ARMY_BOUNDS_CALL,RVA_SPRITE_FIGURE_SIZE,(void (*)(void))map_army_bounds,{0}},
    {MOD_MAP_TEXTURES,RVA_MAP_BG_TILE_DRAW_CALL,RVA_MAP_BG_TILE_DRAW,(void (*)(void))map_draw_tile,{0}},
    {MOD_MAP_TEXTURES,RVA_MAP_TILE_CREATE_CALL,RVA_TEXTURE_CREATE,(void (*)(void))map_create_tile,{0}},
    {MOD_MAP_TEXTURES,RVA_MAP_TILE_CROP_CALL,RVA_MAP_TILE_CROP,(void (*)(void))map_tile_crop,{0}},
    {MOD_GUI,RVA_GUI_PANEL_RECT_1,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_first_panel,{0}},
    {MOD_GUI,RVA_GUI_PANEL_RECT_2,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_PANEL_RECT_3,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_PANEL_RECT_4,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_PANEL_RECT_5,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_PANEL_RECT_6,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_PANEL_RECT_7,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_PANEL_RECT_8,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_PANEL_RECT_9,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_last_rect,{0}},
    {MOD_GUI,RVA_GUI_BORDER_RECT_1,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_first_border,{0}},
    {MOD_GUI,RVA_GUI_BORDER_RECT_2,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_BORDER_RECT_3,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_BORDER_RECT_4,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_BORDER_RECT_5,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_BORDER_RECT_6,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_BORDER_RECT_7,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_next_rect,{0}},
    {MOD_GUI,RVA_GUI_BORDER_RECT_8,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))gui_last_rect,{0}},
    {MOD_RENDERING,RVA_FPS_HIGH_DECOR_CALL,RVA_FPS_HIGH_DECOR_DRAW,mod_highest,{0}},
    {MOD_RENDERING,RVA_MAP_VIEW_DRAW_CALL,RVA_MAP_DRAW_PLANE,(void (*)(void))mod_terrain_plane,{0}},
    {MOD_RENDERING,RVA_MAP_SKY_UNDERLAY_DRAW_CALL,RVA_MAP_DRAW_PLANE,(void (*)(void))mod_terrain_plane,{0}},
    {MOD_RENDERING,RVA_MAP_DRAW_UNITS_CALL,RVA_MAP_DRAW_UNITS,(void (*)(void))mod_map_units,{0}},
    {MOD_RENDERING,RVA_FPS_MAP_TERRAIN_RECT_CALL,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))terrain_rect,{0}},
    {MOD_RENDERING,RVA_DECOR_DRAW_CALL,RVA_DECOR_DRAW,mod_high,{0}},
    {MOD_RENDERING,RVA_FPS_HIGH_DECOR_SPRITE_CALL_A,RVA_DRAW_SPRITE_SHEARED,(void (*)(void))mod_decor,{0}},
    {MOD_RENDERING,RVA_FPS_HIGH_DECOR_SPRITE_CALL_B,RVA_DRAW_SPRITE_SHEARED,(void (*)(void))mod_decor,{0}},
    {MOD_RENDERING,RVA_FPS_LOW_DECOR_SPRITE_CALL,RVA_DRAW_SPRITE_SHEARED,(void (*)(void))mod_decor,{0}},
    {MOD_SCENES,RVA_FPS_SCENE_FIELD_CALL_A,RVA_BATTLE_FIELD_DRAW,(void (*)(void))mod_scene_field,{0}},
    {MOD_SCENES,RVA_FPS_SCENE_FIELD_CALL_B,RVA_BATTLE_FIELD_DRAW,(void (*)(void))mod_scene_field,{0}},
    {MOD_SCENES,RVA_FPS_SCENE_FIELD_CALL_C,RVA_BATTLE_FIELD_DRAW,(void (*)(void))mod_scene_field,{0}},
    {MOD_SCENES,RVA_WAIT_BATTLE_BACKGROUND_CALL_A,RVA_BATTLE_BACKGROUND,mod_terrain_battle,{0}},
    {MOD_SCENES,RVA_WAIT_BATTLE_BACKGROUND_CALL_B,RVA_BATTLE_BACKGROUND,mod_terrain_battle,{0}},
    {MOD_SCENES,RVA_WAIT_BATTLE_BACKGROUND_CALL_C,RVA_BATTLE_BACKGROUND,mod_terrain_battle,{0}},
    {MOD_SCENES,RVA_FPS_SCENE_TERRAIN_RECT_CALL,RVA_FPS_TERRAIN_RECT_DRAW,(void (*)(void))terrain_rect,{0}},
    {MOD_SCENES,RVA_FPS_SCENE_PARTICLES_CALL_A,RVA_PARTICLES_DRAW,(void (*)(void))mod_scene_particles,{0}},
    {MOD_SCENES,RVA_FPS_SCENE_PARTICLES_CALL_B,RVA_PARTICLES_DRAW,(void (*)(void))mod_scene_particles,{0}},
    {MOD_SCENES,RVA_FPS_SCENE_PARTICLES_CALL_C,RVA_PARTICLES_DRAW,(void (*)(void))mod_scene_particles,{0}},
    {MOD_SCENES,RVA_FPS_SCENE_PARTICLES_CALL_D,RVA_PARTICLES_DRAW,(void (*)(void))mod_scene_particles,{0}},
    {MOD_SCENES,RVA_FPS_SCENE_PROJECTILES_CALL,RVA_FPS_SCENE_PROJECTILES_DRAW,mod_scene_projectiles,{0}},
    {MOD_FRAME_CONTROLS,RVA_FPS_GRAPHICS_SECTION_END_CALL,RVA_SECTION_END,fps_graphics_section_end,{0}},
    {MOD_FRAME_CONTROLS,RVA_FPS_PREFERENCES_WAIT_CALL,RVA_WAIT_FOR_CLICK,(void (*)(void))fps_preferences_wait,{0}},
    {MOD_FRAME_CONTROLS,RVA_FPS_FRAME_CHOICES_CALL,RVA_RADIO_OPTION,(void (*)(void))fps_frame_rate_choices,{0}}
};
static int mod_verify_calls(void) {
    for(size_t i=0;i<sizeof(mod_calls)/sizeof(mod_calls[0]);i++) {
        ModCall *c=&mod_calls[i];if(!(c->group&mod_features))continue;
        unsigned char *site=detail_base+c->site;int32_t rel=0;
        memcpy(&rel,site+1,4);
        if(site[0]!=0xe8 || site+5+rel!=detail_base+c->target) {
            char line[256];
            snprintf(line,sizeof(line),"fps_mod %sdisabled reason=game_callsite_changed site_rva=0x%llx expected_target_rva=0x%llx",
                c->group==MOD_GUI?"gui_frame_batching ":c->group==MOD_MAP_TEXTURES?"map_texture_preparation ":"",(unsigned long long)c->site,(unsigned long long)c->target);
            mod_log(line);
            if(c->group!=MOD_GUI && c->group!=MOD_MAP_TEXTURES)return 0;
            mod_features&=~c->group;
            continue;
        }
        memcpy(c->original,site,5);
    }
    return 1;
}
static int mod_restore_page_protections(ModCodePage *pages,size_t count) {
    int ok=1;DWORD ignored;
    while(count) {ModCodePage *p=&pages[--count];if(!VirtualProtect(p->page,p->size,p->original,&ignored))ok=0;}
    return ok;
}
static int mod_rollback_calls(ModCodePage *pages,size_t count) {
    int writable=1;DWORD ignored;
    for(size_t i=0;i<count;i++)if(!VirtualProtect(pages[i].page,pages[i].size,PAGE_EXECUTE_READWRITE,&ignored))writable=0;
    if(writable) {
        for(size_t i=0;i<sizeof(mod_calls)/sizeof(mod_calls[0]);i++) {
            if(!(mod_calls[i].group&mod_features))continue;
            unsigned char *site=detail_base+mod_calls[i].site;memcpy(site,mod_calls[i].original,5);
            FlushInstructionCache(GetCurrentProcess(),site,5);
        }
    }
    int restored=mod_restore_page_protections(pages,count);
    if(!writable || !restored)mod_log("fps_mod disabled reason=rollback_or_page_protection_failed module_retained=1");
    return writable;
}
static int mod_install_calls(void) {
    if(!mod_verify_calls())return 0;
    if(!mod_features)return 1;
    unsigned char *stubs=allocate_near((uintptr_t)(detail_base+mod_calls[0].site));if(!stubs)return 0;
    const size_t count=sizeof(mod_calls)/sizeof(mod_calls[0]);
    for(size_t i=0;i<count;i++) {
        if(!(mod_calls[i].group&mod_features))continue;
        intptr_t delta=(intptr_t)(stubs+i*16)-(intptr_t)(detail_base+mod_calls[i].site+5);
        if(delta<INT32_MIN || delta>INT32_MAX){VirtualFree(stubs,0,MEM_RELEASE);return 0;}
        write_jump(stubs+i*16,mod_calls[i].hook);
    }
    DWORD old,ignored;
    if(!VirtualProtect(stubs,4096,PAGE_EXECUTE_READ,&old)){VirtualFree(stubs,0,MEM_RELEASE);return 0;}
    FlushInstructionCache(GetCurrentProcess(),stubs,count*16);
    SYSTEM_INFO system;GetSystemInfo(&system);
    ModCodePage pages[sizeof(mod_calls)/sizeof(mod_calls[0])];size_t page_count=0,opened=0;
    for(size_t i=0;i<count;i++) {
        if(!(mod_calls[i].group&mod_features))continue;
        unsigned char *page=(unsigned char *)((uintptr_t)(detail_base+mod_calls[i].site)&~((uintptr_t)system.dwPageSize-1));
        size_t j=0;while(j<page_count && pages[j].page!=page)j++;
        if(j==page_count)pages[page_count++]=(ModCodePage){page,system.dwPageSize,0};
    }
    for(;opened<page_count;opened++)if(!VirtualProtect(pages[opened].page,pages[opened].size,PAGE_EXECUTE_READWRITE,&pages[opened].original))break;
    if(opened!=page_count) {
        if(!mod_restore_page_protections(pages,opened))mod_log("fps_mod disabled reason=page_protection_restore_failed");
        VirtualFree(stubs,0,MEM_RELEASE);return 0;
    }
    int ok=1;
    for(size_t i=0;i<count;i++) {
        if(!(mod_calls[i].group&mod_features))continue;
        unsigned char *site=detail_base+mod_calls[i].site;
        if(!write_relative_call(site,stubs+i*16)){ok=0;break;}
        if(mod_calls[i].group==MOD_GUI)gui_record_site(site);
        FlushInstructionCache(GetCurrentProcess(),site,5);
    }
    if(ok && !mod_restore_page_protections(pages,page_count))ok=0;
    if(!ok) {
        if(mod_rollback_calls(pages,page_count))VirtualFree(stubs,0,MEM_RELEASE);
        return 0;
    }
    (void)ignored;return 1;
}
#else
typedef struct {
    unsigned group;
    uintptr_t site,target;
    void (*hook)(void);
    const char *name;
} ModCall;
static unsigned mod_features;
static ModCall mod_calls[]={
    {MOD_RENDERING,RVA_FPS_HIGH_DECOR_SPRITE_CALL_A,RVA_DRAW_SPRITE_SHEARED,(void (*)(void))mod_decor,"decor_sprite_a"},
    {MOD_RENDERING,RVA_FPS_HIGH_DECOR_SPRITE_CALL_B,RVA_DRAW_SPRITE_SHEARED,(void (*)(void))mod_decor,"decor_sprite_b"},
    {MOD_RENDERING,RVA_FPS_LOW_DECOR_SPRITE_CALL,RVA_DRAW_SPRITE_SHEARED,(void (*)(void))mod_decor_low,"decor_sprite_low"},
    {MOD_RENDERING,RVA_MAP_SKY_UNDERLAY_DRAW_CALL,RVA_MAP_DRAW_PLANE,(void (*)(void))mod_plane_draw,"decor_sky_plane_draw"},
    {MOD_RENDERING,RVA_MAP_VIEW_DRAW_CALL,RVA_MAP_DRAW_PLANE,(void (*)(void))mod_plane_draw,"decor_view_plane_draw"},
    {MOD_FRAME_CONTROLS,RVA_FPS_GRAPHICS_SECTION_END_CALL,RVA_SECTION_END,fps_graphics_section_end,"graphics_section_end"},
    {MOD_FRAME_CONTROLS,RVA_FPS_PREFERENCES_WAIT_CALL,RVA_WAIT_FOR_CLICK,(void (*)(void))fps_preferences_wait,"preferences_wait"},
    {MOD_FRAME_CONTROLS,RVA_FPS_FRAME_CHOICES_CALL,RVA_RADIO_OPTION,(void (*)(void))fps_frame_rate_choices,"frame_rate_choices"}
};
static int mod_verify_calls(void) {
    for(size_t i=0;i<sizeof(mod_calls)/sizeof(mod_calls[0]);i++) {
        ModCall *c=&mod_calls[i];if(!(c->group&mod_features))continue;
        if(fps_bl_target(detail_base+c->site)!=detail_base+c->target) {
            char line[256];snprintf(line,sizeof(line),"fps_mod disabled reason=game_callsite_changed site_rva=0x%llx expected_target_rva=0x%llx",(unsigned long long)c->site,(unsigned long long)c->target);mod_log(line);return 0;
        }
    }
    return 1;
}
static int mod_install_calls(void) {
    if(!(mod_features&(MOD_RENDERING|MOD_FRAME_CONTROLS)))return 1;
    if(!mod_verify_calls())return 0;
    for(size_t i=0;i<sizeof(mod_calls)/sizeof(mod_calls[0]);i++) {
        ModCall *c=&mod_calls[i];if(!(c->group&mod_features))continue;
        if(!fps_host->pointCall((unsigned)c->site,(unsigned)c->target,(void *)c->hook,c->name))return 0;
    }
    return 1;
}
#endif

// Enable per-primitive state checks before another native plugin can issue direct GL calls.
void coe5seGraphicsNativePluginLoading(void) {
    atomic_store_explicit(&ta_external_plugin,1,memory_order_relaxed);
}

// Finish queued draws using the old pixels before a native mod edits a source texture.
__declspec(dllexport) int coe5seBeforeTextureUpdate(unsigned int texture) {
    if(!texture)return 0;
    coe5seGraphicsNativePluginLoading();
#ifdef _WIN32
    if(gui_expected)return 0;
#endif
    if(!texture_array_enabled)return 1;
    if(!ta_context_ok || !ta_edit_context || atomic_load(&ta_edit_context->shared) ||
       !ta_context || !ta_current_context || ta_current_context()!=ta_context ||
       arrays_native_open || arrays_open)return 0;
    arrays_end_frame();
    array_texture_remove(texture);
    return 1;
}

static int mod_initialize_original_gl(void) {
    HMODULE gl=GetModuleHandleA(FPS_GL_LIBRARY);if(!gl)return 0;
#define MOD_GL(var,name) do {*(void **)&var=(void *)GetProcAddress(gl,name);if(!var)return 0;}while(0)
    MOD_GL(real_gl_begin,"glBegin");MOD_GL(real_gl_end,"glEnd");MOD_GL(real_gl_bind,"glBindTexture");
    MOD_GL(real_gl_color,"glColor4f");MOD_GL(real_gl_color3,"glColor3f");MOD_GL(real_gl_enable,"glEnable");
    MOD_GL(real_gl_disable,"glDisable");MOD_GL(real_gl_blend,"glBlendFunc");MOD_GL(real_gl_alpha,"glAlphaFunc");
    MOD_GL(real_gl_depth_mask,"glDepthMask");MOD_GL(real_gl_tex_env,"glTexEnvi");
#undef MOD_GL
    return 1;
}
static void mod_disable(void) {
    mod_operational=mod_scenes_enabled=0;arrays_enabled=0;texture_array_enabled=0;in_highest=0;decor_scope=0;
}
static unsigned mod_requested_features(const PluginApi *api) {
    const int *enabled=api->setting("graphics_changes");
    if(!enabled || !*enabled)return 0;
    const int *render=api->setting("smoother_map_rendering"),*frames=api->setting("frame_controls"),*scenes=api->setting("scene_sprite_batching");
    mod_render_setting=render;
    unsigned requested=(render && *render?MOD_RENDERING:0)|(frames && *frames?MOD_FRAME_CONTROLS:0)|(scenes && *scenes?MOD_SCENES:0);
#ifdef _WIN32
    const int *gui=api->setting("gui_frame_batching");
    if(gui && *gui)requested|=MOD_GUI;
    const int *textures=api->setting("optimize_map_textures");
    if((requested&MOD_RENDERING) && textures && *textures)requested|=MOD_MAP_TEXTURES;
#endif
#ifdef __APPLE__
    if(requested&MOD_SCENES)mod_log("fps_mod scene_batching disabled reason=scene_calls_unmapped_on_mac");
    requested&=~MOD_SCENES;
#endif
    return requested;
}
int coe5seGraphicsInit(const PluginApi *api,const char *userFolder) {
    if(!api || api->version!=PLUGIN_API_VERSION || !api->logLine || !api->setting)return 0;
    plugin_api=api;detail_base=(unsigned char *)FPS_GAME_MODULE();
    char line[192];
    snprintf(line,sizeof(line),"fps_mod startup version=" FPS_MOD_VERSION " required_game=5.43 api=%d diagnostics=once_per_category",api->version);
    mod_log(line);
    mod_features=mod_requested_features(api);
    if((mod_features&MOD_SCENES) && !api->wrap){mod_features&=~MOD_SCENES;mod_log("fps_mod scene_batching disabled reason=wrapper_api_unavailable");}
    if(mod_features&MOD_FRAME_CONTROLS){
        int argc=0;wchar_t **argv=CommandLineToArgvW(GetCommandLineW(),&argc);
        fps_benchmark_mode=argv && fps_benchmark_argument(argc,argv);
        if(!argv || fps_benchmark_mode || !api->ui || !api->ui->checkbox || !api->ui->tooltip || !api->ui->choice){
            mod_features&=~MOD_FRAME_CONTROLS;
            mod_log(fps_benchmark_mode?"fps_mod frame_controls skipped reason=native_benchmark":"fps_mod frame_controls skipped reason=required_api_unavailable");
        }
        if(argv)LocalFree(argv);
    }
    if(!mod_features){mod_log("fps_mod disabled reason=launch_settings hooks_installed=0");return 1;}
#ifdef _WIN32
    mod_original_highest=(void (*)(void))(detail_base+RVA_FPS_HIGH_DECOR_DRAW);
    mod_original_high=(void (*)(void))(detail_base+RVA_DECOR_DRAW);
#else
    mod_original_plane=(void (*)(uintptr_t,uintptr_t,uintptr_t))(detail_base+RVA_MAP_DRAW_PLANE);
    if(!fps_host){mod_disable();mod_log("fps_mod disabled reason=core_routes_unavailable");return 1;}
#endif
    mod_original_decor=(ModDecorFn)(detail_base+RVA_DRAW_SPRITE_SHEARED);
    if(!game_hash_matches()) {mod_disable();mod_log("fps_mod disabled reason=unsupported_game_hash graphics_imports_unchanged=1");return 1;}
#ifdef _WIN32
    if((mod_features&MOD_MAP_TEXTURES) && !fps_standard_glu(fps_expected_glu())){
        mod_features&=~MOD_MAP_TEXTURES;
        mod_log("fps_mod map_texture_preparation disabled reason=unrecognized_glu_provider");
    }
#endif
    if(!mod_verify_calls()){mod_disable();mod_log("fps_mod hook_check status=blocked graphics_imports_unchanged=1");return 1;}
    if((mod_features&(MOD_RENDERING|MOD_SCENES)) &&
       !fps_check_gl_imports(detail_base,GetModuleHandleA(FPS_GL_LIBRARY),fps_expected_glu(),mod_import_problem)){
        mod_features&=~(MOD_MAP_TEXTURES|MOD_RENDERING|MOD_SCENES);
        mod_log("fps_mod map_and_scene_batching disabled reason=graphics_import_check_failed");
    }
    if((mod_features&(MOD_RENDERING|MOD_SCENES|MOD_GUI)) && !mod_initialize_original_gl()){
        mod_features&=~(MOD_MAP_TEXTURES|MOD_RENDERING|MOD_SCENES|MOD_GUI);
        mod_log("fps_mod drawing disabled reason=original_gl_exports_unavailable");
    }
#ifdef _WIN32
    if(mod_features&MOD_GUI){
        int shared_renderer=!!(mod_features&(MOD_RENDERING|MOD_SCENES));
        if(!gui_prepare_imports(shared_renderer) || (!shared_renderer && !arrays_load_functions())){
            mod_features&=~MOD_GUI;
            mod_log("fps_mod gui_frame_batching disabled reason=graphics_functions_unavailable_or_replaced");
        }
    }
    ta_scene_program=!!(mod_features&(MOD_RENDERING|MOD_SCENES));
#endif
    FpsArrayInstall imports={0};
    if((mod_features&(MOD_RENDERING|MOD_SCENES)) && (!install_texture_array() || !install_gl_arrays(&imports))) {
        mod_disable();
        if(!arrays_restore_imports(&imports,replace_gl_import))mod_log("fps_mod disabled reason=gl_hook_rollback_failed module_retained=1");
        mod_log("fps_mod disabled reason=gl_hook_installation_failed module_retained=1");return 1;
    }
    if(mod_features&MOD_FRAME_CONTROLS){
        fps_next_graphics_section_end=(void (*)(void))(detail_base+RVA_SECTION_END);
        fps_next_preferences_wait=(int (*)(int,int))(detail_base+RVA_WAIT_FOR_CLICK);
        fps_next_frame_rate_choice=(int (*)(int,int,const char *,...))(detail_base+RVA_RADIO_OPTION);
    }
    if(!mod_install_calls()) {
        mod_disable();
        if(!arrays_restore_imports(&imports,replace_gl_import))mod_log("fps_mod disabled reason=gl_hook_rollback_failed module_retained=1");
        mod_log("fps_mod disabled reason=call_patch_transaction_failed module_retained=1");return 1;
    }
    if(!mod_features){mod_log("fps_mod disabled reason=no_compatible_features hooks_installed=0");return 1;}
    {
        unsigned calls=0;for(size_t i=0;i<sizeof(mod_calls)/sizeof(mod_calls[0]);i++)if(mod_calls[i].group&mod_features)calls++;
        snprintf(line,sizeof(line),"fps_mod hook_check status=passed imports=%u callsites=%u rendering=%d frame_controls=%d scene_batching=%d gui_frames=%d",
            (unsigned)imports.count,calls,!!(mod_features&MOD_RENDERING),!!(mod_features&MOD_FRAME_CONTROLS),!!(mod_features&MOD_SCENES),!!(mod_features&MOD_GUI));mod_log(line);
    }
    if(mod_features&MOD_FRAME_CONTROLS){
        fps_initialize_graphics_settings(userFolder);
        if(!api->wrap || !api->wrap(SITE_MAP_DRAW,(void *)fps_frame_preference_map_draw,(void **)&fps_next_map_draw))mod_log("fps_mod graphics_menu map_start_callback=unavailable preference_applies_at_menu");
        if(!api->wrap || !api->wrap(SITE_MAIN_MENU_WAIT,(void *)mod_main_menu_wait,(void **)&mod_next_main_menu_wait))mod_log("fps_mod diagnostics main_menu_callback=unavailable graphics_report=first_map");
    }
#ifdef _WIN32
    map_textures_enabled=!!(mod_features&MOD_MAP_TEXTURES);
    if(map_textures_enabled){
        map_bounds_init();
        map_load_observed=api->observe && api->observe(SITE_GAME_LOAD,map_load_world,NULL);
        map_reserve_failed=!map_load_observed;
        snprintf(line,sizeof line,"fps_mod map_texture_preparation enabled=1 terrain_reserve=%d",!map_reserve_failed);
        mod_log(line);
    }
#endif
    mod_operational=!!(mod_features&MOD_RENDERING);
    if(mod_features&MOD_SCENES)mod_install_scene_waits(api);
    snprintf(line,sizeof(line),"fps_mod initialized version=" FPS_MOD_VERSION " game=5.43 api=%d launch_settings_fixed=1 quality_unchanged=1 graphics_report=first_current_context",api->version);
    mod_log(line);
    mod_log_graphics();return 1;
}

#ifdef __APPLE__
void coe5seGraphicsMacHost(const GraphicsMacHost *host) { fps_host=host; }
#endif

#else
#include "graphics.h"

int coe5seGraphicsInit(const PluginApi *api, const char *userFolder) { (void)api; (void)userFolder; return 1; }
void coe5seGraphicsNativePluginLoading(void) {}
#endif
