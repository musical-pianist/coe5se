// Check transfer failures, native error ownership and reserve cleanup in Windows OpenGL.
#include "../graphics.c"
#include <GL/glext.h>
#include <assert.h>

// Simulate an ignored driver copy or upload without changing the caller's error flag.
static void WINAPI skip_copy(unsigned a,unsigned b,int c,int d,int e,int f,unsigned g,unsigned h,
                             int i,int j,int k,int l,int m,int n,int o) {
    (void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;
    (void)i;(void)j;(void)k;(void)l;(void)m;(void)n;(void)o;
}
static void skip_upload(unsigned a,int b,int c,int d,int e,int f,int g,int h,unsigned i,unsigned j,const void *k) {
    (void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;(void)i;(void)j;(void)k;
}

// The same source must recover on the next copy and preserve an external GL error.
static void check_copy(unsigned source) {
    FpsTextureLayer layer={0};
    MapCopyImage driver_copy=map_copy_image;
    void (*driver_upload)(unsigned,int,int,int,int,int,int,int,unsigned,unsigned,const void *)=ta_subimage3d;
    map_copy_image=skip_copy;
    ta_subimage3d=skip_upload;
    glEnable(0xffff);
    map_copy_ticks=0;
    assert(!ta_prepare(&ta_terrain_cache,source,&layer,1));
    assert(ta_reject_reason==FPS_TEXTURE_TRANSFER);
    assert(ta_terrain_cache.pages[0].used==0 && glGetError()==GL_INVALID_ENUM);
    map_copy_image=driver_copy;
    long long attempts=ta_terrain_cache.attempts;
    assert(!ta_prepare(&ta_terrain_cache,source,&layer,1) && ta_terrain_cache.attempts==attempts);
    ta_terrain_cache.pass+=64;
    map_copy_ticks=0;
    assert(ta_prepare(&ta_terrain_cache,source,&layer,1));
    ta_release_layer(&ta_terrain_cache,&layer);
    memset(ta_terrain_cache.entries,0,ta_terrain_cache.size*sizeof *ta_terrain_cache.entries);
    glBindTexture(GL_TEXTURE_2D,source);
    unsigned char image[64*64*4];
    memset(image,0,sizeof image);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
    map_copy_image=skip_copy;
    ta_subimage3d=driver_upload;
    assert(ta_clone(&ta_terrain_cache,source,&layer));
    ta_release_layer(&ta_terrain_cache,&layer);
    map_copy_image=driver_copy;
    assert(glGetError()==GL_NO_ERROR);
}

// Native mods invalidate only their source. A cold miss on a full stack must be retryable.
static void check_edits_and_stack(unsigned source) {
    FpsTextureLayer layer,other_layer;
    GLuint other;
    glGenTextures(1,&other);glBindTexture(GL_TEXTURE_2D,other);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,0x812f);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,0x812f);
    unsigned char pixels[64*64*4];
    memset(pixels,182,sizeof pixels);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    array_texture_remove(source);
    int depth=0,max_depth=0;
    glGetIntegerv(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH,&max_depth);
    for(int i=0;i<max_depth;i++)glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    assert(!ta_prepare(&ta_terrain_cache,source,&layer,1) && ta_reject_reason==FPS_CLIENT_STACK_FULL);
    glGetIntegerv(GL_CLIENT_ATTRIB_STACK_DEPTH,&depth);
    assert(!terrain_submission_available() && depth==max_depth && glGetError()==GL_NO_ERROR);
    for(int i=0;i<max_depth;i++)glPopClientAttrib();
    map_copy_ticks=0;
    assert(ta_prepare(&ta_terrain_cache,source,&layer,1));
    map_copy_ticks=0;
    assert(ta_prepare(&ta_terrain_cache,other,&other_layer,1));
    ta_scene_replace=1;
    assert(ta_prepare(&ta_sprite_cache,other,&other_layer,1));
    ta_scene_replace=0;
    assert(!coe5seBeforeTextureUpdate(0));
    arrays_native_open=1;
    assert(!coe5seBeforeTextureUpdate(other));
    arrays_native_open=0;
    assert(ta_prepare(&ta_terrain_cache,other,&other_layer,0));
    assert(coe5seBeforeTextureUpdate(other));
    long long attempts=ta_terrain_cache.attempts;
    assert(ta_prepare(&ta_terrain_cache,source,&layer,0) && ta_terrain_cache.attempts==attempts);
    assert(!ta_prepare(&ta_terrain_cache,other,&other_layer,0));
    assert(!ta_prepare(&ta_sprite_cache,other,&other_layer,0));
    memset(pixels,97,sizeof pixels);
    glBindTexture(GL_TEXTURE_2D,other);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,64,64,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    // Leave the old pixels in the freed array layer on the unchecked path.
    void (*upload)(unsigned,int,int,int,int,int,int,int,unsigned,unsigned,const void *)=ta_subimage3d;
    map_texture_pass=0;ta_subimage3d=skip_upload;
    assert(ta_prepare(&ta_terrain_cache,other,&other_layer,1));
    ta_subimage3d=upload;map_texture_pass=MAP_TERRAIN;map_copy_ticks=0;
    assert(ta_prepare(&ta_terrain_cache,other,&other_layer,1));
    unsigned char actual[sizeof pixels];
    assert(ta_layer_matches(other_layer.array_texture,other_layer.layer,0,64,64,pixels,actual));
    assert(ta_prepare(&ta_terrain_cache,other,&other_layer,0));
    if(ta_has_restart){
        glEnable(0x8f9d);assert(!terrain_submission_available());glDisable(0x8f9d);
    }
    assert(glGetError()==GL_NO_ERROR);
    array_texture_remove(other);glDeleteTextures(1,&other);
}

// Read the state seen by the original game rectangle helper.
static int replay_calls;
static GLboolean replay_rectangle;
static GLfloat replay_bias;
static void observe_rect(float x,float y,float w,float h,float u,float v,float du,float dv) {
    (void)x;(void)y;(void)w;(void)h;(void)u;(void)v;(void)du;(void)dv;
    replay_rectangle=glIsEnabled(TA_TEXTURE_RECTANGLE);
    glGetTexEnvfv(GL_TEXTURE_FILTER_CONTROL,GL_TEXTURE_LOD_BIAS,&replay_bias);
    replay_calls++;
}

// A raw state change between two tiles affects only the second tile, even on the replay path.
static void check_terrain_order(unsigned source) {
    if(!ta_has_rectangle)return;
    unsigned char *base=detail_base;
    detail_base=(unsigned char *)((uintptr_t)observe_rect-RVA_FPS_TERRAIN_RECT_DRAW);
    terrain_scope=terrain_client_ready=1;arrays_texture=source;
    glBindTexture(GL_TEXTURE_2D,source);
    terrain_count=1;terrain_sources[0]=source;
    glEnable(TA_TEXTURE_RECTANGLE);glTexEnvf(GL_TEXTURE_FILTER_CONTROL,GL_TEXTURE_LOD_BIAS,1.25f);
    glEnableClientState(GL_VERTEX_ARRAY);
    terrain_flush();
    assert(replay_calls==1 && !replay_rectangle && replay_bias==0.0f);
    glDisableClientState(GL_VERTEX_ARRAY);
    terrain_rect(0,0,1,1,0,0,1,1);
    assert(replay_calls==2 && replay_rectangle && replay_bias==1.25f && !terrain_count);
    glDisable(TA_TEXTURE_RECTANGLE);glTexEnvf(GL_TEXTURE_FILTER_CONTROL,GL_TEXTURE_LOD_BIAS,0);
    terrain_scope=0;detail_base=base;array_texture_begin_frame();
}

// Loading a native plugin extends sampling checks to partially filled batches.
static void check_sampling_boundary(void) {
    assert(!atomic_load(&ta_external_plugin));
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL,GL_TEXTURE_LOD_BIAS,0.5f);
    assert(!ta_capture_sampling_allowed(1) && ta_capture_sampling_allowed(0));
    coe5seGraphicsNativePluginLoading();
    assert(!ta_capture_sampling_allowed(0));
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL,GL_TEXTURE_LOD_BIAS,0);
    assert(ta_capture_sampling_allowed(0));
}

// Shader programs, biased sampling and incomplete mip chains must reach the native renderer.
static void check_sampling_fallbacks(void) {
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL,GL_TEXTURE_LOD_BIAS,-0.5f);
    assert(!ta_legacy_sampling_allowed());
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL,GL_TEXTURE_LOD_BIAS,0);
    if(ta_has_fragment_program){
        glEnable(GL_FRAGMENT_PROGRAM_ARB);assert(!ta_legacy_sampling_allowed());glDisable(GL_FRAGMENT_PROGRAM_ARB);
    }
    if(ta_has_vertex_program){
        glEnable(GL_VERTEX_PROGRAM_ARB);assert(!ta_legacy_sampling_allowed());glDisable(GL_VERTEX_PROGRAM_ARB);
    }
    assert(ta_legacy_sampling_allowed());
    unsigned texture;glGenTextures(1,&texture);glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    int w,h,last,rgba=1;unsigned min,mag;
    glTexImage2D(GL_TEXTURE_2D,1,GL_RGB8,32,32,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
    assert(!ta_source_parameters(&w,&h,&last,&min,&mag,1,&rgba) && ta_reject_reason==FPS_TEXTURE_FORMAT);
    glTexImage2D(GL_TEXTURE_2D,1,GL_RGBA8,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    assert(ta_source_parameters(&w,&h,&last,&min,&mag,1,&rgba) && last==1);
    glDeleteTextures(1,&texture);assert(glGetError()==GL_NO_ERROR);
}

// Exercise rectangle targets, deferred army copies and reclaiming empty sprite pages.
static void check_sprite_limits(unsigned source) {
    FpsTextureLayer layer={0};
    if(ta_has_rectangle){
        arrays_real_glEnable=(void *)glEnable;arrays_real_glDisable=(void *)glDisable;
        arrays_glEnable(TA_TEXTURE_RECTANGLE);
        ta_active_texture(0x84c1);
        arrays_glEnable(TA_TEXTURE_RECTANGLE);arrays_glDisable(TA_TEXTURE_RECTANGLE);
        ta_active_texture(0x84c0);
        assert(!ta_allowed_state());
        arrays_glDisable(TA_TEXTURE_RECTANGLE);
        assert(ta_allowed_state());
        unsigned char (WINAPI *indexed)(unsigned,unsigned)=ta_indexed_enabled;
        for(int path=0;path<2;path++){
            if(path)ta_indexed_enabled=NULL;
            for(unsigned unit=0;unit<2;unit++){
                ta_active_texture(0x84c0+unit);glEnable(TA_TEXTURE_RECTANGLE);ta_active_texture(0x84c0);
                assert(!ta_rectangle_units_clear());
                int active=0;glGetIntegerv(0x84e0,&active);assert(active==0x84c0);
                ta_active_texture(0x84c0+unit);glDisable(TA_TEXTURE_RECTANGLE);ta_active_texture(0x84c0);
                assert(ta_rectangle_units_clear());
            }
        }
        ta_indexed_enabled=indexed;
    }
    ta_scene_replace=1;map_texture_pass=0;
    assert(ta_prepare(&ta_sprite_cache,source,&layer,1));
    if(ta_has_rectangle){
        ta_active_texture(0x84c1);glEnable(TA_TEXTURE_RECTANGLE);ta_active_texture(0x84c0);
        assert(!array_texture_begin_draw(&ta_sprite_cache,layer.array_texture));
        ta_active_texture(0x84c1);glDisable(TA_TEXTURE_RECTANGLE);ta_active_texture(0x84c0);
    }
    long long attempts=ta_sprite_cache.attempts;
    for(int i=0;i<20;i++){
        map_texture_pass=MAP_UNITS;map_copy_ticks=map_tick_frequency;
        assert(!ta_prepare(&ta_sprite_cache,source,&layer,1));
        map_texture_pass=0;
        assert(ta_prepare(&ta_sprite_cache,source,&layer,0) && ta_sprite_cache.attempts==attempts);
    }
    map_texture_pass=MAP_UNITS;
    map_copy_ticks=0;
    assert(ta_prepare(&ta_sprite_cache,source,&layer,1));
    map_copy_ticks=map_tick_frequency;
    assert(ta_prepare(&ta_sprite_cache,source,&layer,0));
    array_texture_remove(source);
    map_texture_pass=0;map_copy_ticks=0;

    GLuint tex=0;
    glGenTextures(1,&tex);glBindTexture(GL_TEXTURE_2D,tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,0x812f);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,0x812f);
    unsigned char pixels[64*8*4]={0};
    for(int i=0;i<TA_MAX_PAGES+2;i++){
        int w=4<<(i%5),h=i<5?4:8;
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
        if(!ta_prepare(&ta_sprite_cache,tex,&layer,1)){
            assert(ta_reject_reason==FPS_CACHE_LIMIT);
            arrays_begin_frame();assert(ta_prepare(&ta_sprite_cache,tex,&layer,1));
        }
        assert(layer.width==w && layer.height==h);
        array_texture_remove(tex);
    }
    ta_scene_replace=0;
    glDeleteTextures(1,&tex);assert(glGetError()==GL_NO_ERROR);
}

// Rejected live textures must leave space for a supported source.
static void check_rejected_slots(unsigned source) {
    FpsTextureArrayEntry entries[4]={0};
    FpsTextureArrayCache cache={.entries=entries,.size=4,.layer_limit=4};
    GLuint rejected[4];glGenTextures(4,rejected);
    FpsTextureLayer layer={0};ta_scene_replace=1;
    for(int i=0;i<4;i++){
        glBindTexture(GL_TEXTURE_2D,rejected[i]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,3,3,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
        assert(!ta_prepare(&cache,rejected[i],&layer,1));
    }
    for(int i=0;i<4;i++)assert(entries[i].source && entries[i].decision<0);
    assert(!ta_prepare(&cache,source,&layer,0));
    assert(ta_prepare(&cache,source,&layer,1));
    for(int i=0;i<4;i++){
        ta_remove(&cache,rejected[i]);
        assert(ta_prepare(&cache,source,&layer,0));
    }
    ta_remove(&cache,source);ta_trim_empty_pages(&cache);
    assert(!cache.page_count && !cache.bytes);
    glDeleteTextures(4,rejected);ta_scene_replace=0;
}

// Evict across a wrapping cluster, then delete its head without losing either live entry.
static void check_probe_chain(void) {
    FpsTextureArrayEntry entries[4]={0};
    FpsTextureArrayCache cache={.entries=entries,.size=4};
    const unsigned names[]={14,13,9,16,7};
    for(int i=0;i<5;i++){
        FpsTextureArrayEntry *e=ta_source_slot(&cache,names[i]);
        assert(e);
        if(e->source)e=ta_close_gap(&cache,e);
        *e=(FpsTextureArrayEntry){.source=names[i],.decision=i<2?-1:1};
    }
    ta_remove(&cache,16);
    assert(ta_source_slot(&cache,9)->source==9 && ta_source_slot(&cache,7)->source==7);
}

// Inject one failed allocation and verify the next permitted pass retries the original texture.
static void skip_storage(unsigned t,int l,int f,int w,int h,int d,int b,unsigned fmt,unsigned type,const void *data) {
    (void)t;(void)l;(void)f;(void)w;(void)h;(void)d;(void)b;(void)fmt;(void)type;(void)data;
}
static void check_allocation_retry(unsigned source) {
    FpsTextureArrayEntry entries[4]={0};
    FpsTextureArrayCache cache={.entries=entries,.size=4,.layer_limit=1};
    FpsTextureLayer layer={0};
    void (*driver)(unsigned,int,int,int,int,int,int,unsigned,unsigned,const void *)=ta_image3d;
    ta_scene_replace=1;ta_image3d=skip_storage;
    assert(!ta_prepare(&cache,source,&layer,1) && ta_reject_reason==FPS_ALLOCATION);
    ta_image3d=driver;
    cache.pass+=63;
    assert(!ta_prepare(&cache,source,&layer,1) && cache.attempts==1);
    ++cache.pass;
    assert(ta_prepare(&cache,source,&layer,1) && cache.attempts==2);
    glDeleteTextures(1,&layer.array_texture);ta_scene_replace=0;
}

// Return a private name only to its original context. Other contexts may reuse its number.
static void check_context(HDC dc,HGLRC owner) {
    HGLRC other=wglCreateContext(dc);
    assert(other && map_label && map_read_label);
    map_sync_reserve_context(owner);
    GLuint name;
    glGenTextures(1,&name);glBindTexture(GL_TEXTURE_2D,name);
    map_label(GL_TEXTURE,name,-1,map_reserve_tag);
    map_reserve[0]=(MapReservedTile){.texture=name,.pixels=calloc(1,MAP_TILE_BYTES)};
    map_reserve[1]=(MapReservedTile){.pixels=calloc(1,MAP_TILE_BYTES)};
    map_reserve_count=2;map_reserve_pending=1;
    assert(wglMakeCurrent(NULL,NULL));
    map_sync_reserve_context(NULL);
    assert(!map_reserve_count && !map_reserve_pending && map_retired_count==1);
    assert(wglMakeCurrent(dc,other));
    map_sync_reserve_context(other);
    assert(!map_reserve_count && !map_reserve_pending && map_retired_count==1);
    glBindTexture(GL_TEXTURE_2D,name);
    assert(glIsTexture(name));
    assert(wglMakeCurrent(dc,owner));
    map_sync_reserve_context(owner);
    assert(!map_retired_count && !glIsTexture(name));
    assert(wglMakeCurrent(dc,other) && glIsTexture(name));
    assert(wglMakeCurrent(dc,owner) && wglDeleteContext(other));
}

typedef struct { HDC dc; HGLRC replacement; GLuint source; } WorkerContext;

// A game upload on a second thread retires the old cache even before that thread starts drawing.
static DWORD WINAPI check_edit_handoff(void *arg) {
    WorkerContext *worker=arg;
    assert(wglMakeCurrent(worker->dc,worker->replacement));
    glBindTexture(GL_TEXTURE_2D,worker->source);
    unsigned char pixels[64*64*4];memset(pixels,193,sizeof pixels);
    arrays_real_glTexImage2D=(void *)glTexImage2D;
    arrays_glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    assert(!ta_initialize_context() && !coe5seBeforeTextureUpdate(worker->source));
    assert(wglMakeCurrent(NULL,NULL));
    return 0;
}

// A new rendering thread can prepare its own map tiles after the original context is destroyed.
static DWORD WINAPI check_replacement(void *arg) {
    WorkerContext *worker=arg;
    assert(wglMakeCurrent(worker->dc,worker->replacement));
    assert(TryAcquireSRWLockExclusive(&map_work_lock));
    assert(ta_initialize_context() && ta_sprite_cache.entries==ta_entries);
    map_sync_reserve_context(worker->replacement);
    assert(!map_reserve_count && !map_reserve_pending && map_retired_count==1);
    MapReservedTile *slot=&map_reserve[0];
    slot->pixels=calloc(1,MAP_TILE_BYTES);
    map_reserve_count=map_reserve_pending=1;
    assert(slot->pixels && map_prepare_slot(slot) && !map_reserve_pending);
    assert(map_name_matches(slot->texture,map_reserve_tag));
    map_release_slot(slot,0);
    assert(!map_reserve_count && glGetError()==GL_NO_ERROR);
    ReleaseSRWLockExclusive(&map_work_lock);
    assert(wglMakeCurrent(NULL,NULL));
    return 0;
}

// Each retired rendering thread releases its claim for later contexts.
static DWORD WINAPI claim_context(void *arg) {
    WorkerContext *worker=arg;
    assert(wglMakeCurrent(worker->dc,worker->replacement) && ta_initialize_context());
    assert(wglMakeCurrent(NULL,NULL));
    return 0;
}

// Exercise the direct-state and framebuffer layer readers on the same compatibility driver.
int main(void) {
    HWND win=CreateWindowW(L"STATIC",L"",WS_POPUP,0,0,8,8,NULL,NULL,NULL,NULL);
    HDC dc=GetDC(win);
    PIXELFORMATDESCRIPTOR pf={.nSize=sizeof pf,.nVersion=1,
        .dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL,.iPixelType=PFD_TYPE_RGBA,.cColorBits=32};
    assert(win && dc && SetPixelFormat(dc,ChoosePixelFormat(dc,&pf),&pf));
    HGLRC context=wglCreateContext(dc);
    assert(context && wglMakeCurrent(dc,context));
    map_textures_enabled=1;
    assert(mod_initialize_original_gl() && install_texture_array() && arrays_load_functions());
    assert(arrays_check_context() && ta_initialize_context() && map_copy_image);
    check_sampling_boundary();
    ta_terrain_cache.layer_limit=1;
    static FpsTextureArrayEntry entries[16];
    ta_terrain_cache.entries=entries;
    ta_terrain_cache.size=16;
    glEnable(GL_TEXTURE_2D);
    map_texture_pass=MAP_TERRAIN;
    unsigned source;
    glGenTextures(1,&source);glBindTexture(GL_TEXTURE_2D,source);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,0x812f);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,0x812f);
    unsigned char image[64*64*4];
    memset(image,19,sizeof image);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
    FpsTextureLayer seed={0};
    assert(ta_clone(&ta_terrain_cache,source,&seed));
    assert(glGetError()==GL_NO_ERROR);
    assert(map_copy_layer(source,seed.array_texture,99,64,64,0));
    assert(glGetError()==GL_INVALID_VALUE && map_copy_image);
    ta_release_layer(&ta_terrain_cache,&seed);
    memset(image,77,sizeof image);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
    check_copy(source);
    ta_terrain_cache.layer_limit=4;
    check_edits_and_stack(source);
    array_texture_remove(source);
    ta_read_subimage=NULL;
    ta_init_layer_reader(3,0,"");
    glBindTexture(GL_TEXTURE_2D,source);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
    check_copy(source);
    map_texture_pass=0;
    check_sprite_limits(source);
    check_terrain_order(source);
    check_sampling_fallbacks();
    check_allocation_retry(source);
    check_rejected_slots(source);
    check_probe_chain();
    // Failed load observation must survive every context change.
    map_sync_reserve_context(context);assert(map_reserve_failed);
    map_sync_reserve_context(NULL);assert(map_reserve_failed);
    map_load_observed=1;
    check_context(dc,context);
    ta_scene_replace=1;
    assert(ta_prepare(&ta_sprite_cache,source,&seed,1));
    WorkerContext handoff={dc,context,source};
    assert(wglMakeCurrent(NULL,NULL));
    HANDLE moved=CreateThread(NULL,0,check_edit_handoff,&handoff,0,NULL);
    assert(moved && WaitForSingleObject(moved,15000)==WAIT_OBJECT_0);
    CloseHandle(moved);
    assert(wglMakeCurrent(dc,context) && !coe5seBeforeTextureUpdate(source));
    assert(!ta_prepare(&ta_sprite_cache,source,&seed,0));
    glBindTexture(GL_TEXTURE_2D,source);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
    assert(image[0]==193 && glGetError()==GL_NO_ERROR);
    map_reserve_context=context;
    map_reserve[0]=(MapReservedTile){.texture=source,.pixels=calloc(1,MAP_TILE_BYTES)};
    map_reserve_count=1;
    HGLRC replacement=wglCreateContext(dc);
    assert(map_reserve[0].pixels && replacement && wglMakeCurrent(NULL,NULL) && wglDeleteContext(context));
    HMODULE glu=LoadLibraryExW(L"glu32.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
    assert(glu);
    arrays_real_gluBuild2DMipmaps=(void *)GetProcAddress(glu,"gluBuild2DMipmaps");
    assert(arrays_real_gluBuild2DMipmaps);
    WorkerContext worker={dc,replacement,0};
    HANDLE thread=CreateThread(NULL,0,check_replacement,&worker,0,NULL);
    assert(thread && WaitForSingleObject(thread,15000)==WAIT_OBJECT_0);
    CloseHandle(thread);FreeLibrary(glu);
    assert(wglDeleteContext(replacement));
    for(unsigned i=0;i<40;i++){
        WorkerContext next={dc,wglCreateContext(dc),0};assert(next.replacement);
        HANDLE owner=CreateThread(NULL,0,claim_context,&next,0,NULL);
        assert(owner && WaitForSingleObject(owner,15000)==WAIT_OBJECT_0);
        CloseHandle(owner);assert(wglDeleteContext(next.replacement));
    }
    ReleaseDC(win,dc);DestroyWindow(win);
    puts("Verified texture edits, rectangle targets, army budget, cache recovery, layer readers and context handoff.");
}
