#pragma once
#include "render_diagnostics.h"
#include <stdatomic.h>
#define TA_CACHE_SIZE 4096
#define TA_TERRAIN_CACHE_SIZE 8192
#define TA_MAX_PAGES 8
#define TA_TEXTURE_RECTANGLE 0x84f5u
#define TA_BUDGET (128u*1024u*1024u)
typedef struct {
    unsigned source_texture,array_texture;
    int layer,width,height,mip_count,page;
} FpsTextureLayer;
typedef struct {
    unsigned texture,min_filter,mag_filter;
    int width,height,last_level,layers,used;
    unsigned char occupied[1024];
} FpsTextureArrayPage;
typedef struct {
    unsigned source,last_pass,capacity_epoch,last_retry_pass;
    int decision, scene_checked, map_checked;
    FpsTextureLayer value;
    FpsFallbackReason reason;
} FpsTextureArrayEntry;
typedef struct {
    FpsTextureArrayPage pages[TA_MAX_PAGES];
    FpsTextureArrayEntry *entries;
    unsigned size;
    unsigned pass,capacity_epoch;
    int page_count, layer_limit;
    int pressure;
    long long bytes, attempts;
} FpsTextureArrayCache;
static int texture_array_enabled;
static atomic_int ta_external_plugin;
static _Thread_local int ta_multi_page=1;
static int ta_track_state,ta_scene_program;
#ifdef __APPLE__
static _Thread_local int ta_glsl120,ta_has_blit;
#endif
static _Thread_local int ta_texture_on,ta_modulate,ta_scene_replace,ta_env_mode;
static _Thread_local unsigned ta_forbidden;
static _Thread_local int ta_saved_arrays[TA_MAX_PAGES],ta_saved_samplers[TA_MAX_PAGES];
static _Thread_local unsigned ta_bound_pages[TA_MAX_PAGES];
static _Thread_local long long ta_state_checks,ta_draw_sessions;
static _Thread_local HGLRC ta_context;
static _Thread_local int ta_context_ok,ta_context_attempted,ta_state_checked,ta_state_ok,ta_drawing;
static _Thread_local int ta_fixed_units;
static _Thread_local unsigned char (WINAPI *ta_indexed_enabled)(unsigned,unsigned);
static _Thread_local int ta_has_aniso,ta_has_swizzle,ta_has_sampler,ta_has_rectangle;
static _Thread_local int ta_has_vertex_program,ta_has_fragment_program;
// Remember which contexts have exactly one initialized array cache for native texture edits.
typedef struct {
    HGLRC context;
    atomic_int shared;
#ifdef _WIN32
    HANDLE owner;
#else
    atomic_int exited;
#endif
} TaEditContext;
static TaEditContext ta_edit_contexts[32];
static _Thread_local TaEditContext *ta_edit_context;

#ifdef _WIN32
static _Thread_local int ta_has_restart,ta_has_nv_restart;
#endif
static _Thread_local unsigned ta_program;
static _Thread_local int ta_saved_program,ta_saved_binding,ta_saved_unit;
static _Thread_local FpsTextureArrayEntry ta_entries[TA_CACHE_SIZE];
static _Thread_local FpsTextureArrayCache ta_sprite_cache,ta_terrain_cache;
static _Thread_local long long ta_hits,ta_uploads,ta_rejected;
static _Thread_local char ta_status[1024];
static _Thread_local FpsFallbackReason ta_reject_reason;
static HMODULE ta_gl;
static HGLRC (WINAPI *ta_current_context)(void);
static PROC (WINAPI *ta_wgl_proc)(LPCSTR);
static void (*ta_get_int)(unsigned,int *);
static void (*ta_get_float)(unsigned,float *);
static const unsigned char *(*ta_get_string)(unsigned);
static unsigned char (*ta_is_enabled)(unsigned);
static void (*ta_get_parameter)(unsigned,unsigned,int *);
static void (*ta_get_parameter_float)(unsigned,unsigned,float *);
static void (*ta_get_level)(unsigned,int,unsigned,int *);
static void (*ta_get_image)(unsigned,int,unsigned,unsigned,void *);
static void (*ta_get_env)(unsigned,unsigned,int *);
static void (*ta_get_env_float)(unsigned,unsigned,float *);
static void (*ta_set_env_float)(unsigned,unsigned,float);
static void (*ta_bind)(unsigned,unsigned);
static void (*ta_generate)(int,unsigned *);
static void (*ta_delete)(int,const unsigned *);
static void (*ta_parameter)(unsigned,unsigned,int);
static void (*ta_push_client)(unsigned);
static void (*ta_pop_client)(void);
static void (*ta_pixel_store)(unsigned,int);
static _Thread_local void (*ta_active_texture)(unsigned);
static _Thread_local void (*ta_image3d)(unsigned,int,int,int,int,int,int,unsigned,unsigned,const void *);
static _Thread_local void (*ta_subimage3d)(unsigned,int,int,int,int,int,int,int,unsigned,unsigned,const void *);
static _Thread_local unsigned (*ta_create_shader)(unsigned);
static _Thread_local void (*ta_shader_source)(unsigned,int,const char *const *,const int *);
static _Thread_local void (*ta_compile_shader)(unsigned);
static _Thread_local void (*ta_get_shader)(unsigned,unsigned,int *);
static _Thread_local void (*ta_shader_log)(unsigned,int,int *,char *);
static _Thread_local void (*ta_delete_shader)(unsigned);
static _Thread_local unsigned (*ta_create_program)(void);
static _Thread_local void (*ta_attach_shader)(unsigned,unsigned);
static _Thread_local void (*ta_link_program)(unsigned);
static _Thread_local void (*ta_get_program)(unsigned,unsigned,int *);
static _Thread_local void (*ta_program_log)(unsigned,int,int *,char *);
static _Thread_local void (*ta_delete_program)(unsigned);
static _Thread_local void (*ta_use_program)(unsigned);
static _Thread_local int (*ta_uniform_location)(unsigned,const char *);
static _Thread_local void (*ta_uniform1i)(int,int);
static _Thread_local void (*ta_bind_sampler)(unsigned,unsigned);

static void ta_message(const char *text) {snprintf(ta_status,sizeof(ta_status),"%s",text);}
static int ta_refuse(FpsFallbackReason reason) {ta_reject_reason=reason;fps_note_fallback(reason);return 0;}
static int ta_extension(const char *list,const char *word) {
    const char *p=list;size_t n=strlen(word);
    while(p && (p=strstr(p,word))) {if((p==list || p[-1]==' ') && (!p[n] || p[n]==' '))return 1;p+=n;}
    return 0;
}
static void *ta_proc(const char *name) {
    void *p=(void *)GetProcAddress(ta_gl,name);
    if(!p && ta_wgl_proc)p=(void *)ta_wgl_proc(name);
    if((uintptr_t)p<=3 || (uintptr_t)p==UINTPTR_MAX)return NULL;
    return p;
}
static unsigned ta_shader(unsigned kind,const char *source) {
    unsigned shader=ta_create_shader(kind);if(!shader)return (unsigned)ta_refuse(FPS_ALLOCATION);
    ta_shader_source(shader,1,&source,NULL);ta_compile_shader(shader);
    int ok=0;ta_get_shader(shader,0x8b81,&ok);
    if(!ok) {ta_shader_log(shader,sizeof(ta_status),NULL,ta_status);ta_delete_shader(shader);return (unsigned)ta_refuse(FPS_SHADER_COMPILE);}
    return shader;
}
static int ta_build_program(void) {
#define TA_VERTEX_POSITION "bool projected=gl_MultiTexCoord0.z<0.0;" \
        "if(projected)gl_Position=mat4(gl_MultiTexCoord1,gl_MultiTexCoord2,gl_MultiTexCoord3,gl_MultiTexCoord4)*gl_Vertex;" \
        "else gl_Position=ftransform();sampleUV=gl_MultiTexCoord0.xy;" \
        "sampleLayer=int((projected?-1.0-gl_MultiTexCoord0.z:gl_MultiTexCoord0.z)+0.5);"
#define TA_SCENE_COLOR "sampleColor=gl_MultiTexCoord0.w<0.0?vec4(1.0):clamp(gl_Color,0.0,1.0);"
#define TA_SCENE_PAGE "samplePage=int((gl_MultiTexCoord0.w<0.0?-1.0-gl_MultiTexCoord0.w:gl_MultiTexCoord0.w)+0.5);"
    static const char vertex[]=
        "#version 130\n"
        "out vec2 sampleUV;out vec4 sampleColor;flat out int sampleLayer;\n"
        "invariant gl_Position;void main(){" TA_VERTEX_POSITION TA_SCENE_COLOR "}\n";
    static const char fragment[]=
        "#version 130\n"
        "uniform sampler2DArray sourceImages;in vec2 sampleUV;in vec4 sampleColor;flat in int sampleLayer;\n"
        "void main(){gl_FragColor=texture(sourceImages,vec3(sampleUV,float(sampleLayer)))*sampleColor;}\n";
    const char *vertex_source=vertex,*fragment_source=fragment;
    static const char multi_vertex[]=
        "#version 130\nout vec2 sampleUV;out vec4 sampleColor;flat out int sampleLayer;flat out int samplePage;\n"
        "invariant gl_Position;void main(){" TA_VERTEX_POSITION
        TA_SCENE_PAGE TA_SCENE_COLOR "}\n";
    static const char multi_fragment[]=
        "#version 130\nuniform sampler2DArray page0,page1,page2,page3,page4,page5,page6,page7;\n"
        "in vec2 sampleUV;in vec4 sampleColor;flat in int sampleLayer;flat in int samplePage;\n"
        "void main(){vec3 c=vec3(sampleUV,float(sampleLayer));vec4 t;"
        "if(samplePage==0)t=texture(page0,c);else if(samplePage==1)t=texture(page1,c);"
        "else if(samplePage==2)t=texture(page2,c);else if(samplePage==3)t=texture(page3,c);"
        "else if(samplePage==4)t=texture(page4,c);else if(samplePage==5)t=texture(page5,c);"
        "else if(samplePage==6)t=texture(page6,c);else t=texture(page7,c);gl_FragColor=t*sampleColor;}\n";
    if(ta_multi_page){vertex_source=multi_vertex;fragment_source=multi_fragment;}
    static const char basic_vertex[]=
        "#version 130\nout vec2 sampleUV;out vec4 sampleColor;flat out int sampleLayer;\n"
        "void main(){gl_Position=ftransform();sampleUV=gl_MultiTexCoord0.xy;"
        "sampleLayer=int(gl_MultiTexCoord0.z+0.5);sampleColor=clamp(gl_Color,0.0,1.0);}\n";
    static const char basic_multi_vertex[]=
        "#version 130\nout vec2 sampleUV;out vec4 sampleColor;flat out int sampleLayer;flat out int samplePage;\n"
        "void main(){gl_Position=ftransform();sampleUV=gl_MultiTexCoord0.xy;sampleLayer=int(gl_MultiTexCoord0.z+0.5);"
        "samplePage=int(gl_MultiTexCoord0.w+0.5);sampleColor=clamp(gl_Color,0.0,1.0);}\n";
    if(!ta_scene_program)vertex_source=ta_multi_page?basic_multi_vertex:basic_vertex;
#ifdef __APPLE__
    static const char vertex120[]=
        "#version 120\n#extension GL_EXT_gpu_shader4 : require\n"
        "varying vec2 sampleUV;varying vec4 sampleColor;flat varying int sampleLayer;\n"
        "invariant gl_Position;void main(){" TA_VERTEX_POSITION TA_SCENE_COLOR "}\n";
    static const char fragment120[]=
        "#version 120\n#extension GL_EXT_gpu_shader4 : require\n#extension GL_EXT_texture_array : require\n"
        "uniform sampler2DArray sourceImages;varying vec2 sampleUV;varying vec4 sampleColor;flat varying int sampleLayer;\n"
        "void main(){gl_FragColor=texture2DArray(sourceImages,vec3(sampleUV,float(sampleLayer)))*sampleColor;}\n";
    static const char multi_vertex120[]=
        "#version 120\n#extension GL_EXT_gpu_shader4 : require\n"
        "varying vec2 sampleUV;varying vec4 sampleColor;flat varying int sampleLayer;flat varying int samplePage;\n"
        "invariant gl_Position;void main(){" TA_VERTEX_POSITION
        TA_SCENE_PAGE TA_SCENE_COLOR "}\n";
    static const char multi_fragment120[]=
        "#version 120\n#extension GL_EXT_gpu_shader4 : require\n#extension GL_EXT_texture_array : require\n"
        "uniform sampler2DArray page0,page1,page2,page3,page4,page5,page6,page7;\n"
        "varying vec2 sampleUV;varying vec4 sampleColor;flat varying int sampleLayer;flat varying int samplePage;\n"
        "void main(){vec3 c=vec3(sampleUV,float(sampleLayer));vec4 t;"
        "if(samplePage==0)t=texture2DArray(page0,c);else if(samplePage==1)t=texture2DArray(page1,c);"
        "else if(samplePage==2)t=texture2DArray(page2,c);else if(samplePage==3)t=texture2DArray(page3,c);"
        "else if(samplePage==4)t=texture2DArray(page4,c);else if(samplePage==5)t=texture2DArray(page5,c);"
        "else if(samplePage==6)t=texture2DArray(page6,c);else t=texture2DArray(page7,c);gl_FragColor=t*sampleColor;}\n";
    if(ta_glsl120){
        vertex_source=ta_multi_page?multi_vertex120:vertex120;
        fragment_source=ta_multi_page?multi_fragment120:fragment120;
    }
    static const char basic_vertex120[]=
        "#version 120\n#extension GL_EXT_gpu_shader4 : require\n"
        "varying vec2 sampleUV;varying vec4 sampleColor;flat varying int sampleLayer;\n"
        "void main(){gl_Position=ftransform();sampleUV=gl_MultiTexCoord0.xy;"
        "sampleLayer=int(gl_MultiTexCoord0.z+0.5);sampleColor=clamp(gl_Color,0.0,1.0);}\n";
    static const char basic_multi_vertex120[]=
        "#version 120\n#extension GL_EXT_gpu_shader4 : require\n"
        "varying vec2 sampleUV;varying vec4 sampleColor;flat varying int sampleLayer;flat varying int samplePage;\n"
        "void main(){gl_Position=ftransform();sampleUV=gl_MultiTexCoord0.xy;sampleLayer=int(gl_MultiTexCoord0.z+0.5);"
        "samplePage=int(gl_MultiTexCoord0.w+0.5);sampleColor=clamp(gl_Color,0.0,1.0);}\n";
    if(ta_glsl120 && !ta_scene_program)vertex_source=ta_multi_page?basic_multi_vertex120:basic_vertex120;
#endif
#undef TA_VERTEX_POSITION
#undef TA_SCENE_COLOR
#undef TA_SCENE_PAGE
    unsigned vs=ta_shader(0x8b31,vertex_source);if(!vs)return 0;
    unsigned fs=ta_shader(0x8b30,fragment_source);if(!fs){ta_delete_shader(vs);return 0;}
    unsigned program=ta_create_program();
    if(!program) {ta_delete_shader(vs);ta_delete_shader(fs);return ta_refuse(FPS_ALLOCATION);}
    ta_attach_shader(program,vs);ta_attach_shader(program,fs);ta_link_program(program);
    ta_delete_shader(vs);ta_delete_shader(fs);
    int ok=0;ta_get_program(program,0x8b82,&ok);
    if(!ok) {ta_program_log(program,sizeof(ta_status),NULL,ta_status);ta_delete_program(program);return ta_refuse(FPS_SHADER_LINK);}
    int locations[TA_MAX_PAGES],count=ta_multi_page?TA_MAX_PAGES:1;
    for(int i=0;i<count;i++) {
        char name[8]="page0";name[4]=(char)('0'+i);
        locations[i]=ta_uniform_location(program,ta_multi_page?name:"sourceImages");
        if(locations[i]<0){ta_message("sampler uniform unavailable");ta_delete_program(program);return ta_refuse(FPS_SHADER_LINK);}
    }
    int before=0;ta_get_int(0x8b8d,&before);ta_use_program(program);
    for(int i=0;i<count;i++)ta_uniform1i(locations[i],ta_multi_page?i+1:0);
    ta_use_program((unsigned)before);
    ta_program=program;
#ifdef __APPLE__
    ta_message(ta_glsl120?"ready GLSL120 EXT_texture_array original UV and mip layers":"ready GLSL130 compatibility original UV and mip layers");return 1;
#else
    ta_message("ready GLSL130 compatibility original UV and mip layers");return 1;
#endif
}
#ifdef _WIN32
#include "map_transfer.h"
#include "texture_layer_check.h"
#endif
// Only the owning thread's exit makes its slot reusable, even if the GL handle is recycled.
// @platform-split context-lifetime: Windows waits on a thread handle; macOS marks exit via pthread TLS.
#ifdef _WIN32
static SRWLOCK ta_registry_lock=SRWLOCK_INIT;
static void ta_registry_enter(void){AcquireSRWLockExclusive(&ta_registry_lock);}
static void ta_registry_leave(void){ReleaseSRWLockExclusive(&ta_registry_lock);}
static int ta_owner_exited(TaEditContext *slot) {
    if(WaitForSingleObject(slot->owner,0)!=WAIT_OBJECT_0)return 0;
    CloseHandle(slot->owner);return 1;
}
static int ta_register_owner(TaEditContext *slot) {
    HANDLE process=GetCurrentProcess();
    return DuplicateHandle(process,GetCurrentThread(),process,&slot->owner,SYNCHRONIZE,FALSE,0);
}
#else
static pthread_mutex_t ta_registry_lock=PTHREAD_MUTEX_INITIALIZER;
static pthread_key_t ta_owner_key;
static int ta_owner_key_ready;
static void ta_registry_enter(void){pthread_mutex_lock(&ta_registry_lock);}
static void ta_registry_leave(void){pthread_mutex_unlock(&ta_registry_lock);}
static void ta_thread_exited(void *value){atomic_store(&((TaEditContext *)value)->exited,1);}
static int ta_owner_exited(TaEditContext *slot){return atomic_load(&slot->exited);}
static int ta_register_owner(TaEditContext *slot) {
    if(!ta_owner_key_ready && pthread_key_create(&ta_owner_key,ta_thread_exited))return 0;
    ta_owner_key_ready=1;
    atomic_store(&slot->exited,0);
    return !pthread_setspecific(ta_owner_key,slot);
}
#endif
// Scan past empty slots to find a live owner before assigning a replacement.
static TaEditContext *ta_visit_context(HGLRC context,int claim) {
    TaEditContext *available=NULL,*result=NULL;
    ta_registry_enter();
    for(size_t i=0;i<sizeof ta_edit_contexts/sizeof *ta_edit_contexts;i++){
        TaEditContext *slot=&ta_edit_contexts[i];
        if(slot->context && (claim || slot->context==context) && ta_owner_exited(slot))slot->context=NULL;
        if(slot->context==context){atomic_store(&slot->shared,1);goto done;}
        if(!slot->context && !available)available=slot;
    }
    if(claim && available && ta_register_owner(available)){
        atomic_store(&available->shared,0);
        available->context=context;result=available;
    }
done:
    ta_registry_leave();
    return result;
}
// An upload-only thread can also retire a cache before the rendering thread returns.
static int ta_cache_owned(HGLRC context) {
    if(!context)return 0;
    if(context!=ta_context){ta_visit_context(context,0);return 0;}
    return ta_edit_context && !atomic_load(&ta_edit_context->shared);
}
static int ta_initialize_context(void) {
    HGLRC context=ta_current_context();if(!context)return ta_refuse(FPS_NO_CONTEXT);
    if(ta_context_attempted)return ta_cache_owned(context)?ta_context_ok:ta_refuse(FPS_CONTEXT_CHANGED);
    ta_sprite_cache.entries=ta_entries;ta_sprite_cache.size=TA_CACHE_SIZE;ta_sprite_cache.layer_limit=256;
    ta_context_attempted=1;ta_context=context;
    ta_edit_context=ta_visit_context(context,1);
    if(!ta_edit_context)return ta_refuse(FPS_CONTEXT_CHANGED);
    const char *v=(const char *)ta_get_string(0x1f02),*ext;
    int major=0,minor=0;
#ifdef __APPLE__
    ext=(const char *)ta_get_string(0x1f03);
    if(v && sscanf(v,"%d.%d",&major,&minor)==2 && major==2 && minor>=1 && ta_extension(ext,"GL_EXT_texture_array") && ta_extension(ext,"GL_EXT_gpu_shader4"))ta_glsl120=1;
    if(!v || sscanf(v,"%d.%d",&major,&minor)!=2 || (major<3 && !ta_glsl120)) {ta_message("OpenGL 3 compatibility context or 2.1 with EXT_texture_array required");return ta_refuse(FPS_GL_VERSION);}
#else
    if(!v || sscanf(v,"%d.%d",&major,&minor)!=2 || major<3) {ta_message("OpenGL 3 compatibility context required");return ta_refuse(FPS_GL_VERSION);}
#endif
    if(major>3 || (major==3 && minor>=2)) {int profile=0;ta_get_int(0x9126,&profile);if((profile&1) && !(profile&2)) {ta_message("compatibility profile required");return ta_refuse(FPS_CORE_PROFILE);}}
    ext=(const char *)ta_get_string(0x1f03);
#ifdef __APPLE__
    ta_has_blit=major>=3 || ta_extension(ext,"GL_EXT_framebuffer_blit");
#endif
#ifdef _WIN32
    map_transfer_init(major,minor,ext);
    ta_has_restart=major>3 || (major==3 && minor>=1);
    ta_has_nv_restart=ta_extension(ext,"GL_NV_primitive_restart");
    if(map_textures_enabled)ta_init_layer_reader(major,minor,ext);
#endif
    ta_has_rectangle=major>3 || (major==3 && minor>=1) || ta_extension(ext,"GL_ARB_texture_rectangle") ||
                     ta_extension(ext,"GL_EXT_texture_rectangle") || ta_extension(ext,"GL_NV_texture_rectangle");
    ta_has_vertex_program=ta_extension(ext,"GL_ARB_vertex_program");
    ta_has_fragment_program=ta_extension(ext,"GL_ARB_fragment_program");
    ta_get_int(0x84e2,&ta_fixed_units);
    if(ta_fixed_units<1 || ta_fixed_units>32)return ta_refuse(FPS_TEXTURE_UNIT);
    if(ta_has_rectangle && ta_extension(ext,"GL_EXT_direct_state_access"))
        *(void **)&ta_indexed_enabled=ta_proc("glIsEnabledIndexedEXT");
    ta_has_aniso=ta_extension(ext,"GL_EXT_texture_filter_anisotropic") || ta_extension(ext,"GL_ARB_texture_filter_anisotropic");
    ta_has_swizzle=major>3 || (major==3 && minor>=3) || ta_extension(ext,"GL_ARB_texture_swizzle");
    ta_has_sampler=major>3 || (major==3 && minor>=3) || ta_extension(ext,"GL_ARB_sampler_objects");
#define TA_EXT(var,name) do {*(void **)&var=ta_proc(name);if(!var){ta_message("missing " name);return ta_refuse(FPS_ENTRY_POINT);}}while(0)
    TA_EXT(ta_active_texture,"glActiveTexture");TA_EXT(ta_image3d,"glTexImage3D");TA_EXT(ta_subimage3d,"glTexSubImage3D");
    TA_EXT(ta_create_shader,"glCreateShader");TA_EXT(ta_shader_source,"glShaderSource");TA_EXT(ta_compile_shader,"glCompileShader");
    TA_EXT(ta_get_shader,"glGetShaderiv");TA_EXT(ta_shader_log,"glGetShaderInfoLog");TA_EXT(ta_delete_shader,"glDeleteShader");
    TA_EXT(ta_create_program,"glCreateProgram");TA_EXT(ta_attach_shader,"glAttachShader");TA_EXT(ta_link_program,"glLinkProgram");
    TA_EXT(ta_get_program,"glGetProgramiv");TA_EXT(ta_program_log,"glGetProgramInfoLog");TA_EXT(ta_delete_program,"glDeleteProgram");
    TA_EXT(ta_use_program,"glUseProgram");TA_EXT(ta_uniform_location,"glGetUniformLocation");TA_EXT(ta_uniform1i,"glUniform1i");
    if(ta_multi_page) {
        int units=0;ta_get_int(0x8872,&units);
        if(units<TA_MAX_PAGES+1)ta_multi_page=0;
        else if(ta_has_sampler){*(void **)&ta_bind_sampler=ta_proc("glBindSampler");if(!ta_bind_sampler)ta_multi_page=0;}
    }
#undef TA_EXT
    if(!ta_build_program())return 0;
    ta_context_ok=1;return 1;
}
static void array_texture_invalidate_state(void) {ta_state_checked=0;}
static void array_texture_begin_frame(void) {array_texture_invalidate_state();}
static int ta_cap_bit(unsigned cap) {
    const unsigned caps[]={0x0b50,0x0b57,0x0b60,0x0de0,0x806f,0x8513,0x8458};
    for(int i=0;i<7;i++)if(cap==caps[i])return i;
    if(cap>=0x3000 && cap<0x3006)return 7+(int)(cap-0x3000);
    if(cap>=0x0c60 && cap<=0x0c63)return 13+(int)(cap-0x0c60);
    return -1;
}
static void ta_note_cap(unsigned cap,int on) {
    if(cap==TA_TEXTURE_RECTANGLE || cap==0x8620 || cap==0x8804){array_texture_invalidate_state();return;}
    if(cap==0x0de1)ta_texture_on=on;
    int bit=ta_cap_bit(cap);
    if(bit>=0)ta_forbidden=on?ta_forbidden|(1u<<bit):ta_forbidden&~(1u<<bit);
}
static void ta_read_switches(void) {
    ta_forbidden=0;ta_texture_on=ta_is_enabled(0x0de1)!=0;
    const unsigned caps[]={0x0b50,0x0b57,0x0b60,0x0de0,0x806f,0x8513,0x8458,0x3000,0x3001,0x3002,0x3003,0x3004,0x3005,0x0c60,0x0c61,0x0c62,0x0c63};
    for(size_t i=0;i<sizeof(caps)/sizeof(caps[0]);i++)if(ta_is_enabled(caps[i]))ta_note_cap(caps[i],1);
    ta_get_env(0x2300,0x2200,&ta_env_mode);ta_modulate=ta_env_mode==0x2100;
}
static int ta_switches_allowed(void) {
    if(ta_texture_on && !ta_forbidden && (ta_modulate || (ta_scene_replace && ta_env_mode==0x1e01)))return 1;
    return ta_refuse(FPS_FIXED_FUNCTION);
}
static int ta_copy_buffers_clear(void) {
    int value=0;
    ta_get_int(0x88ed,&value);if(value)return ta_refuse(FPS_PIXEL_BUFFER);
    ta_get_int(0x88ef,&value);if(value)return ta_refuse(FPS_PIXEL_BUFFER);
    return 1;
}
// Array sampling requires the original fixed-function shader and neutral unit LOD bias.
static int ta_legacy_sampling_allowed(void) {
    if((ta_has_vertex_program && ta_is_enabled(0x8620)) ||
       (ta_has_fragment_program && ta_is_enabled(0x8804)))return ta_refuse(FPS_CUSTOM_PROGRAM);
    float bias=0;ta_get_env_float(0x8500,0x8501,&bias);
    return bias==0.0f || ta_refuse(FPS_TEXTURE_SAMPLING);
}
static int ta_allowed_state(void) {
    if(!texture_array_enabled || !ta_current_context || !ta_initialize_context())return 0;
    if(ta_state_checked)return ta_state_ok && (!ta_track_state || ta_switches_allowed());
    ta_state_checks++;
    ta_state_checked=1;ta_state_ok=0;
    int value=0;ta_get_int(0x8b8d,&value);if(value)return ta_refuse(FPS_CUSTOM_PROGRAM);
    ta_get_int(0x84e0,&value);if(value!=0x84c0)return ta_refuse(FPS_TEXTURE_UNIT);
    if(!ta_legacy_sampling_allowed())return 0;
    if(!ta_copy_buffers_clear())return 0;
#ifdef __APPLE__
    ta_get_int(0x8ca6,&value);if(value)return ta_refuse(FPS_FRAMEBUFFER);if(ta_has_blit){ta_get_int(0x8caa,&value);if(value)return ta_refuse(FPS_FRAMEBUFFER);}
#else
    ta_get_int(0x8ca6,&value);if(value)return ta_refuse(FPS_FRAMEBUFFER);ta_get_int(0x8caa,&value);if(value)return ta_refuse(FPS_FRAMEBUFFER);
#endif
    ta_get_int(0x0d10,&value);if(value)return ta_refuse(FPS_PIXEL_TRANSFER);
    if(ta_has_sampler) {ta_get_int(0x8919,&value);if(value)return ta_refuse(FPS_SAMPLER_OBJECT);}
    if(ta_has_rectangle && ta_is_enabled(TA_TEXTURE_RECTANGLE))return ta_refuse(FPS_FIXED_FUNCTION);
    if(ta_track_state)ta_read_switches();
    if(!ta_track_state) {
    if(!ta_is_enabled(0x0de1) || ta_is_enabled(0x0b50) || ta_is_enabled(0x0b57) || ta_is_enabled(0x0b60))return ta_refuse(FPS_FIXED_FUNCTION);
    if(ta_is_enabled(0x0de0) || ta_is_enabled(0x806f) || ta_is_enabled(0x8513) || ta_is_enabled(0x8458))return ta_refuse(FPS_FIXED_FUNCTION);
    for(unsigned cap=0x3000;cap<0x3006;cap++)if(ta_is_enabled(cap))return ta_refuse(FPS_FIXED_FUNCTION);
    for(unsigned cap=0x0c60;cap<=0x0c63;cap++)if(ta_is_enabled(cap))return ta_refuse(FPS_FIXED_FUNCTION);
    ta_get_env(0x2300,0x2200,&value);if(value!=0x2100)return ta_refuse(FPS_FIXED_FUNCTION);
    }
    ta_get_int(0x0c40,&value);if(value!=0x1c00)return ta_refuse(FPS_RENDER_MODE);
    ta_get_int(0x0b33,&value);if(value)return ta_refuse(FPS_RENDER_MODE);
    float matrix[16];ta_get_float(0x0ba8,matrix);
    for(int i=0;i<16;i++)if(matrix[i]!=((i%5)==0?1.0f:0.0f))return ta_refuse(FPS_TEXTURE_MATRIX);
    const unsigned scales[]={0x0d14,0x0d18,0x0d1a,0x0d1c},biases[]={0x0d15,0x0d19,0x0d1b,0x0d1d};
    for(int i=0;i<4;i++){float f;ta_get_float(scales[i],&f);if(f!=1.0f)return ta_refuse(FPS_PIXEL_TRANSFER);ta_get_float(biases[i],&f);if(f!=0.0f)return ta_refuse(FPS_PIXEL_TRANSFER);}
    for(int unit=1;unit<ta_fixed_units;unit++) {
        ta_active_texture(0x84c0u+(unsigned)unit);
        int enabled=ta_is_enabled(0x0de0) || ta_is_enabled(0x0de1) || ta_is_enabled(0x806f) || ta_is_enabled(0x8513) ||
                    (ta_has_rectangle && ta_is_enabled(TA_TEXTURE_RECTANGLE));
        ta_active_texture(0x84c0);if(enabled)return ta_refuse(FPS_EXTRA_TEXTURE_UNIT);
    }
    ta_state_ok=1;return !ta_track_state || ta_switches_allowed();
}
// Recheck raw plugin state once per submitted batch, preserving the active texture unit.
static int ta_rectangle_units_clear(void) {
    if(!ta_has_rectangle)return 1;
    if(ta_indexed_enabled){
        for(int i=0;i<ta_fixed_units;i++)
            if(ta_indexed_enabled(TA_TEXTURE_RECTANGLE,i))return ta_refuse(FPS_FIXED_FUNCTION);
        return 1;
    }
    int active=0;
    ta_get_int(0x84e0,&active);
    if(active!=0x84c0)return ta_refuse(FPS_TEXTURE_UNIT);
    int enabled=ta_is_enabled(TA_TEXTURE_RECTANGLE);
    for(int i=1;i<ta_fixed_units && !enabled;i++){
        ta_active_texture(0x84c0u+i);
        enabled=ta_is_enabled(TA_TEXTURE_RECTANGLE);
    }
    ta_active_texture(0x84c0);
    return enabled?ta_refuse(FPS_FIXED_FUNCTION):1;
}
// Game state changes flush through the imports. External native code can change state between primitives.
static int ta_capture_sampling_allowed(int first) {
    if(!first && !atomic_load_explicit(&ta_external_plugin,memory_order_relaxed))return 1;
    return ta_legacy_sampling_allowed() && ta_rectangle_units_clear();
}
// Sampling state that a native plugin may have changed since the last collected primitive.
typedef struct { unsigned rectangles; int active,vertex_program,fragment_program; float bias; } TaReplaySampling;

// Collected array primitives used ordinary 2D sampling on unit zero.
static TaReplaySampling ta_begin_native_replay(void) {
    TaReplaySampling saved={0};
    ta_get_int(0x84e0,&saved.active);
    if(ta_has_rectangle){
        for(int i=0;i<ta_fixed_units;i++){
            ta_active_texture(0x84c0u+(unsigned)i);
            if(!ta_is_enabled(TA_TEXTURE_RECTANGLE))continue;
            saved.rectangles|=1u<<i;
            real_gl_disable(TA_TEXTURE_RECTANGLE);
        }
    }
    ta_active_texture(0x84c0);
    saved.vertex_program=ta_has_vertex_program && ta_is_enabled(0x8620);
    saved.fragment_program=ta_has_fragment_program && ta_is_enabled(0x8804);
    ta_get_env_float(0x8500,0x8501,&saved.bias);
    if(saved.vertex_program)real_gl_disable(0x8620);
    if(saved.fragment_program)real_gl_disable(0x8804);
    if(saved.bias!=0.0f)ta_set_env_float(0x8500,0x8501,0);
    return saved;
}
// Return exactly the state observed immediately before replay on this context.
static void ta_end_native_replay(TaReplaySampling saved) {
    if(saved.bias!=0.0f)ta_set_env_float(0x8500,0x8501,saved.bias);
    if(saved.fragment_program)real_gl_enable(0x8804);
    if(saved.vertex_program)real_gl_enable(0x8620);
    for(unsigned i=0;saved.rectangles;i++,saved.rectangles>>=1){
        if(saved.rectangles&1){ta_active_texture(0x84c0u+i);real_gl_enable(TA_TEXTURE_RECTANGLE);}
    }
    ta_active_texture((unsigned)saved.active);
}
static int ta_level_size(int size,int level){int n=size>>level;return n>0?n:1;}
static int ta_source_parameters(int *source_width,int *source_height,int *last,unsigned *min,unsigned *mag,int terrain,int *rgba) {
    int w=0,h=0,base=0,max=0,s=0,t=0,mi=0,ma=0,compare=0;
    ta_get_level(0x0de1,0,0x1000,&w);ta_get_level(0x0de1,0,0x1001,&h);
    if(ta_scene_replace){if(w<=0 || h<=0 || w>256 || h>256 || (w&(w-1)) || (h&(h-1)))return ta_refuse(FPS_TEXTURE_SIZE);}
    else if((w!=64 && w!=128) || h!=w)return ta_refuse(FPS_TEXTURE_SIZE);
    *source_width=w;*source_height=h;*last=0;for(int n=w>h?w:h;n>1;n>>=1)(*last)++;
    ta_get_parameter(0x0de1,0x2801,&mi);ta_get_parameter(0x0de1,0x2800,&ma);
    if((mi!=0x2600 && mi!=0x2601 && (mi<0x2700 || mi>0x2703)) || (ma!=0x2600 && ma!=0x2601))return ta_refuse(FPS_TEXTURE_SAMPLING);
    ta_get_parameter(0x0de1,0x2802,&s);ta_get_parameter(0x0de1,0x2803,&t);if(s!=0x812f || t!=0x812f)return ta_refuse(FPS_TEXTURE_SAMPLING);
    ta_get_parameter(0x0de1,0x813c,&base);ta_get_parameter(0x0de1,0x813d,&max);
    ta_get_parameter(0x0de1,0x884c,&compare);if(base || max<0 || compare)return ta_refuse(FPS_TEXTURE_SAMPLING);
    if(ta_scene_replace || terrain){if(mi==0x2600 || mi==0x2601)*last=0;else if(max<*last)*last=max;}
    else if(max<*last)return ta_refuse(FPS_TEXTURE_SAMPLING);
    float f=0;ta_get_parameter_float(0x0de1,0x813a,&f);if(f!=-1000.0f)return ta_refuse(FPS_TEXTURE_SAMPLING);
    ta_get_parameter_float(0x0de1,0x813b,&f);if(f!=1000.0f)return ta_refuse(FPS_TEXTURE_SAMPLING);
    ta_get_parameter_float(0x0de1,0x8501,&f);if(f!=0.0f)return ta_refuse(FPS_TEXTURE_SAMPLING);
    if(ta_has_aniso){ta_get_parameter_float(0x0de1,0x84fe,&f);if(!(f<=1.0f))return ta_refuse(FPS_TEXTURE_SAMPLING);}
    if(ta_has_swizzle)for(int i=0;i<4;i++){int a=0;ta_get_parameter(0x0de1,0x8e42u+(unsigned)i,&a);if(a!=0x1903+i)return ta_refuse(FPS_TEXTURE_SAMPLING);}
    int base_format=0;
    for(int level=0;level<=*last;level++) {
        int width=0,height=0,border=0,format=0,compressed=0;
        ta_get_level(0x0de1,level,0x1000,&width);ta_get_level(0x0de1,level,0x1001,&height);
        ta_get_level(0x0de1,level,0x1005,&border);ta_get_level(0x0de1,level,0x1003,&format);ta_get_level(0x0de1,level,0x86a1,&compressed);
        if(width!=ta_level_size(w,level) || height!=ta_level_size(h,level) || border)return ta_refuse(FPS_TEXTURE_LEVELS);
        if(!level)base_format=format;
        else if(format!=base_format)return ta_refuse(FPS_TEXTURE_FORMAT);
        int rgb=terrain && (format==0x8051 || format==0x1907 || format==3);
        if(rgb)*rgba=0;
        if(compressed || (!rgb && format!=0x8058 && format!=0x1908 && format!=4))return ta_refuse(FPS_TEXTURE_FORMAT);
        for(unsigned attr=0x805c;attr<=0x805f;attr++) {
            int bits=0;ta_get_level(0x0de1,level,attr,&bits);
            if(bits!=((rgb && attr==0x805f)?0:8))return ta_refuse(FPS_TEXTURE_FORMAT);
        }
    }
    *min=(unsigned)mi;*mag=(unsigned)ma;return 1;
}
static size_t ta_layer_bytes(int width,int height,int last) {
    size_t bytes=0;
    for(int i=0;i<=last;i++)bytes+=(size_t)ta_level_size(width,i)*ta_level_size(height,i)*4;
    return bytes;
}
static FpsTextureArrayPage *ta_find_page(FpsTextureArrayCache *cache,int width,int height,int last,unsigned min,unsigned mag) {
    for(int i=0;i<cache->page_count;i++){FpsTextureArrayPage *p=&cache->pages[i];if(p->width==width && p->height==height && p->last_level==last && p->min_filter==min && p->mag_filter==mag && p->used<p->layers)return p;}
    if(cache->page_count==TA_MAX_PAGES){ta_refuse(FPS_CACHE_LIMIT);return NULL;}
    int layers=0;ta_get_int(0x88ff,&layers);if(layers>cache->layer_limit)layers=cache->layer_limit;
    size_t one=ta_layer_bytes(width,height,last);
    long long remaining=(long long)TA_BUDGET-cache->bytes;if(remaining<=0){ta_refuse(FPS_CACHE_LIMIT);return NULL;}
    if((long long)layers>(remaining/(long long)one))layers=(int)(remaining/(long long)one);
    if(layers<1){ta_refuse(FPS_CACHE_LIMIT);return NULL;}
    unsigned texture=0;ta_generate(1,&texture);if(!texture){ta_refuse(FPS_ALLOCATION);return NULL;}
    ta_bind(0x8c1a,texture);ta_parameter(0x8c1a,0x2801,(int)min);ta_parameter(0x8c1a,0x2800,(int)mag);
    ta_parameter(0x8c1a,0x2802,0x812f);ta_parameter(0x8c1a,0x2803,0x812f);
    ta_parameter(0x8c1a,0x813c,0);ta_parameter(0x8c1a,0x813d,last);
    for(int level=0;level<=last;level++) {
        int w=ta_level_size(width,level),h=ta_level_size(height,level);ta_image3d(0x8c1a,level,0x8058,w,h,layers,0,0x1908,0x1401,NULL);
        int actual=0,actual_height=0;ta_get_level(0x8c1a,level,0x1000,&actual);ta_get_level(0x8c1a,level,0x1001,&actual_height);
        if(actual!=w || actual_height!=h){ta_delete(1,&texture);ta_refuse(FPS_ALLOCATION);return NULL;}
    }
    FpsTextureArrayPage *p=&cache->pages[cache->page_count++];
    *p=(FpsTextureArrayPage){.texture=texture,.min_filter=min,.mag_filter=mag,.width=width,.height=height,.last_level=last,.layers=layers};
    cache->bytes+=(long long)one*layers;return p;
}
static int ta_available_layer(const FpsTextureArrayPage *page) {
    for(int i=0;i<page->layers;i++)if(!page->occupied[i])return i;
    return -1;
}
static void ta_release_layer(FpsTextureArrayCache *cache,FpsTextureLayer *value) {
    for(int i=0;i<cache->page_count;i++) {
        FpsTextureArrayPage *page=&cache->pages[i];
        if(page->texture==value->array_texture && value->layer>=0 && value->layer<page->layers && page->occupied[value->layer]) {
            page->occupied[value->layer]=0;page->used--;cache->capacity_epoch++;break;
        }
    }
    memset(value,0,sizeof(*value));
}
static void ta_packing(void) {
    ta_pixel_store(0x0d05,1);ta_pixel_store(0x0d02,0);ta_pixel_store(0x0d03,0);ta_pixel_store(0x0d04,0);
    ta_pixel_store(0x0d00,0);ta_pixel_store(0x0d01,0);ta_pixel_store(0x806b,0);ta_pixel_store(0x806c,0);
    ta_pixel_store(0x0cf5,1);ta_pixel_store(0x0cf2,0);ta_pixel_store(0x0cf3,0);ta_pixel_store(0x0cf4,0);
    ta_pixel_store(0x0cf0,0);ta_pixel_store(0x0cf1,0);ta_pixel_store(0x806d,0);ta_pixel_store(0x806e,0);
}
// Leave a caller's saved client state in place when the stack is full.
static int ta_client_stack_room(void) {
    int depth=0,limit=0;
    ta_get_int(0x0bb1,&depth);ta_get_int(0x0d3b,&limit);
    return depth<limit;
}
static int ta_clone(FpsTextureArrayCache *cache,unsigned source,FpsTextureLayer *out) {
    int before2d=0,before_array=0,width=0,height=0,last=0,ok=0;unsigned min=0,mag=0;
    ta_get_int(0x8069,&before2d);ta_get_int(0x8c1d,&before_array);ta_bind(0x0de1,source);
    unsigned char *memory=NULL;
    int rgba=1;
    if(!ta_source_parameters(&width,&height,&last,&min,&mag,cache==&ta_terrain_cache,&rgba))goto done;
    if(!ta_client_stack_room()){ta_refuse(FPS_CLIENT_STACK_FULL);goto done;}
    ta_push_client(0x00000001u);ta_packing();FpsTextureArrayPage *p=ta_find_page(cache,width,height,last,min,mag);
    if(!p){ta_pop_client();goto done;}
    int layer=ta_available_layer(p);if(layer<0){ta_pop_client();ta_refuse(FPS_CACHE_LIMIT);goto done;}
#ifdef _WIN32
    if(map_texture_pass){
        int verified=ta_checked_map_layer(source,p->texture,layer,width,height,last,rgba);
        if(!verified){ta_pop_client();goto done;}
        goto copied;
    }
#endif
    memory=FPS_ALLOC((size_t)width*height*4);
    if(!memory){ta_pop_client();ta_refuse(FPS_ALLOCATION);goto done;}
    for(int level=0;level<=last;level++) {
        int w=ta_level_size(width,level),h=ta_level_size(height,level);
        ta_bind(0x0de1,source);
        ta_get_image(0x0de1,level,0x1908,0x1401,memory);
        ta_bind(0x8c1a,p->texture);
        ta_subimage3d(0x8c1a,level,0,0,layer,w,h,1,0x1908,0x1401,memory);
    }
#ifdef _WIN32
copied:
#endif
    ta_pop_client();p->occupied[layer]=1;p->used++;
    *out=(FpsTextureLayer){source,p->texture,layer,width,height,last+1,(int)(p-cache->pages)};
    if(cache==&ta_sprite_cache)ta_uploads++;
    ok=1;
done:
    if(memory)FPS_FREE(memory);
    ta_bind(0x8c1a,(unsigned)before_array);ta_bind(0x0de1,(unsigned)before2d);return ok;
}
// Close a probe-chain gap and return the empty slot, including when the table was full.
static FpsTextureArrayEntry *ta_close_gap(FpsTextureArrayCache *cache,FpsTextureArrayEntry *entry) {
    unsigned mask=cache->size-1,start=(unsigned)(entry-cache->entries),gap=start;
    for(unsigned step=1;step<cache->size;step++){
        unsigned i=(start+step)&mask;
        FpsTextureArrayEntry *next=&cache->entries[i];
        if(!next->source)break;
        unsigned home=(next->source*2654435761u)&mask;
        if(((gap-home)&mask)<((i-home)&mask)){
            cache->entries[gap]=*next;
            gap=i;
        }
    }
    cache->entries[gap]=(FpsTextureArrayEntry){0};
    return &cache->entries[gap];
}
// A full table can admit a new texture by retiring a layerless entry.
static FpsTextureArrayEntry *ta_source_slot(FpsTextureArrayCache *cache,unsigned source) {
    unsigned mask=cache->size-1,index=(source*2654435761u)&mask;
    FpsTextureArrayEntry *spare=NULL;
    for(unsigned i=0;i<cache->size;i++){
        FpsTextureArrayEntry *e=&cache->entries[(index+i)&mask];
        if(!e->source || e->source==source)return e;
        if(!spare && e->decision<=0)spare=e;
    }
    return spare;
}
static int ta_prepare(FpsTextureArrayCache *cache,unsigned source,FpsTextureLayer *out,int allow_upload) {
    if(!source || !out || !ta_allowed_state() || !cache->entries)return 0;
    for(int i=0;i<cache->page_count;i++)if(cache->pages[i].texture==source)return 0;
    FpsTextureArrayEntry *e=ta_source_slot(cache,source);
    if(!e){cache->pressure=1;return ta_refuse(FPS_CACHE_TABLE_FULL);}
    int replace=e->source!=source;
    if(!replace)e->last_pass=cache->pass;
    int verify=0;
#ifdef _WIN32
    verify=map_texture_pass && e->decision>0 && !e->map_checked;
#endif
    int retry=e->decision<0 && ((ta_scene_replace && !e->scene_checked) ||
              ((e->reason==FPS_TEXTURE_TRANSFER || e->reason==FPS_ALLOCATION) && cache->pass-e->last_retry_pass>=64) ||
              (e->reason==FPS_CACHE_LIMIT && e->capacity_epoch!=cache->capacity_epoch));
    if(replace || !e->decision || retry || verify) {
        if(!allow_upload || !ta_copy_buffers_clear())return 0;
#ifdef _WIN32
        int budgeted=map_tick_frequency && ((map_texture_pass==MAP_TERRAIN && cache==&ta_terrain_cache) ||
                                            (map_texture_pass==MAP_UNITS && cache==&ta_sprite_cache));
        if(budgeted && map_copy_ticks>=map_tick_frequency/250)return 0;
        long long started=budgeted?map_ticks():0;
#endif
        if(verify)ta_release_layer(cache,&e->value);
        if(replace){
            if(e->source)e=ta_close_gap(cache,e);
            e->source=source;
        }
        cache->attempts++;
        e->scene_checked=ta_scene_replace;
        e->capacity_epoch=cache->capacity_epoch;
        e->decision=ta_clone(cache,source,&e->value)?1:-1;
#ifdef _WIN32
        e->map_checked=map_texture_pass!=0 && e->decision>0;
        if(budgeted)map_copy_ticks+=map_ticks()-started;
#endif
        if(e->decision<0){
            e->reason=ta_reject_reason;
            if(e->reason==FPS_CLIENT_STACK_FULL){e->decision=0;return 0;}
            e->last_retry_pass=cache->pass;
            ta_rejected++;
        }
    }
    if(cache->pass)e->last_pass=cache->pass;
    if(e->decision<0) {
        if(e->reason==FPS_CACHE_LIMIT)cache->pressure=1;
        fps_note_fallback(e->reason);return 0;
    }
    *out=e->value;ta_hits++;return 1;
}
static int array_texture_prepare(unsigned source,FpsTextureLayer *out) {
    return ta_prepare(&ta_sprite_cache,source,out,1);
}
static void ta_forget(FpsTextureArrayCache *cache,unsigned source) {
    if(!source || !cache->entries)return;
    unsigned mask=cache->size-1,index=(source*2654435761u)&mask;
    for(unsigned i=0;i<cache->size;i++) {
        FpsTextureArrayEntry *e=&cache->entries[(index+i)&mask];
        if(e->source==source){ta_release_layer(cache,&e->value);e->decision=0;return;}
        if(!e->source)return;
    }
}
static void array_texture_forget(unsigned source) {
    ta_forget(&ta_sprite_cache,source);
    ta_forget(&ta_terrain_cache,source);
}
static void ta_remove(FpsTextureArrayCache *cache,unsigned source) {
    if(!source || !cache->entries)return;
    unsigned mask=cache->size-1,start=(source*2654435761u)&mask;
    for(unsigned i=0;i<cache->size;i++) {
        unsigned index=(start+i)&mask;
        if(!cache->entries[index].source)return;
        if(cache->entries[index].source!=source)continue;
        ta_release_layer(cache,&cache->entries[index].value);
        ta_close_gap(cache,&cache->entries[index]);
        return;
    }
}
static void array_texture_remove(unsigned source) {
    ta_remove(&ta_sprite_cache,source);
    ta_remove(&ta_terrain_cache,source);
}
// Reclaim unused page layouts after queued draws finish and repair moved page indices.
static void ta_trim_empty_pages(FpsTextureArrayCache *cache) {
    for(int i=0;i<cache->page_count;) {
        FpsTextureArrayPage *page=&cache->pages[i];
        if(page->used){i++;continue;}
        ta_delete(1,&page->texture);
        cache->bytes-=(long long)ta_layer_bytes(page->width,page->height,page->last_level)*page->layers;
        int last=--cache->page_count;
        if(i!=last) {
            *page=cache->pages[last];
            for(unsigned e=0;e<cache->size;e++)
                if(cache->entries[e].decision>0 && cache->entries[e].value.page==last)cache->entries[e].value.page=i;
        }
        cache->capacity_epoch++;
    }
    cache->pressure=0;
}
#ifdef _WIN32
static void ta_trim_terrain_cache(FpsTextureArrayCache *cache) {
    for(unsigned i=0;i<cache->size;) {
        FpsTextureArrayEntry *entry=&cache->entries[i];
        if(entry->source && cache->pass-entry->last_pass>8)ta_remove(cache,entry->source);
        else i++;
    }
    ta_trim_empty_pages(cache);
}
#endif
static int array_texture_begin_draw(FpsTextureArrayCache *cache,unsigned texture) {
    if(!ta_allowed_state() || !ta_legacy_sampling_allowed() || !ta_rectangle_units_clear())return 0;
    int own=0;for(int i=0;i<cache->page_count;i++)if(cache->pages[i].texture==texture)own=1;if(!own)return 0;
    if(ta_multi_page) {
        if(!ta_drawing) {
            ta_get_int(0x8b8d,&ta_saved_program);ta_get_int(0x84e0,&ta_saved_unit);
            if(ta_saved_program || ta_saved_unit!=0x84c0)return 0;
            for(int i=0;i<TA_MAX_PAGES;i++) {
                ta_active_texture(0x84c1u+(unsigned)i);ta_get_int(0x8c1d,&ta_saved_arrays[i]);
                ta_saved_samplers[i]=0;
                if(ta_has_sampler){ta_get_int(0x8919,&ta_saved_samplers[i]);ta_bind_sampler((unsigned)i+1,0);}
                ta_bound_pages[i]=UINT32_MAX;
            }
            ta_active_texture(0x84c0);ta_use_program(ta_program);ta_drawing=1;ta_draw_sessions++;
        }
        for(int i=0;i<TA_MAX_PAGES;i++) {
            unsigned page=i<cache->page_count?cache->pages[i].texture:0;
            if(ta_bound_pages[i]!=page){ta_active_texture(0x84c1u+(unsigned)i);ta_bind(0x8c1a,page);ta_bound_pages[i]=page;}
        }
        ta_active_texture(0x84c0);return 1;
    }
    if(ta_drawing)return 0;
    ta_get_int(0x8b8d,&ta_saved_program);ta_get_int(0x84e0,&ta_saved_unit);
    if(ta_saved_program || ta_saved_unit!=0x84c0)return 0;
    ta_get_int(0x8c1d,&ta_saved_binding);ta_use_program(ta_program);ta_bind(0x8c1a,texture);ta_drawing=1;return 1;
}
static void array_texture_end_draw(void) {
    if(!ta_drawing)return;
    if(ta_multi_page) {
        for(int i=0;i<TA_MAX_PAGES;i++) {
            ta_active_texture(0x84c1u+(unsigned)i);ta_bind(0x8c1a,(unsigned)ta_saved_arrays[i]);
            if(ta_has_sampler)ta_bind_sampler((unsigned)i+1,(unsigned)ta_saved_samplers[i]);
        }
        ta_use_program((unsigned)ta_saved_program);ta_active_texture((unsigned)ta_saved_unit);ta_drawing=0;return;
    }
    ta_bind(0x8c1a,(unsigned)ta_saved_binding);ta_use_program((unsigned)ta_saved_program);
    ta_active_texture((unsigned)ta_saved_unit);ta_drawing=0;
}
static int install_texture_array(void) {
    texture_array_enabled=1;ta_track_state=1;
    ta_gl=GetModuleHandleA(FPS_GL_LIBRARY);if(!ta_gl)return 0;
#define TA_CORE(var,name) do {*(void **)&var=(void *)GetProcAddress(ta_gl,name);if(!var)return 0;}while(0)
    TA_CORE(ta_current_context,FPS_GL_CONTEXT_FN);
#ifdef _WIN32
    TA_CORE(ta_wgl_proc,"wglGetProcAddress");
#endif
    TA_CORE(ta_get_int,"glGetIntegerv");TA_CORE(ta_get_float,"glGetFloatv");TA_CORE(ta_get_string,"glGetString");TA_CORE(ta_is_enabled,"glIsEnabled");
    TA_CORE(ta_get_parameter,"glGetTexParameteriv");TA_CORE(ta_get_parameter_float,"glGetTexParameterfv");TA_CORE(ta_get_level,"glGetTexLevelParameteriv");
    TA_CORE(ta_get_image,"glGetTexImage");TA_CORE(ta_get_env,"glGetTexEnviv");TA_CORE(ta_bind,"glBindTexture");TA_CORE(ta_generate,"glGenTextures");TA_CORE(ta_delete,"glDeleteTextures");
    TA_CORE(ta_get_env_float,"glGetTexEnvfv");TA_CORE(ta_set_env_float,"glTexEnvf");
    TA_CORE(ta_parameter,"glTexParameteri");TA_CORE(ta_push_client,"glPushClientAttrib");TA_CORE(ta_pop_client,"glPopClientAttrib");TA_CORE(ta_pixel_store,"glPixelStorei");
#undef TA_CORE
    ta_message("awaiting compatibility OpenGL context");return 1;
}
