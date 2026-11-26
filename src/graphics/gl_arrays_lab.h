#pragma once
#include <stddef.h>
#include "render_diagnostics.h"
#ifdef _WIN32
static _Thread_local int terrain_scope;
static void terrain_flush(void);
#else
static inline void terrain_flush(void) {}
#endif
#define ARR_CAPACITY 16384u
typedef struct { float xyz[3], uv[4], rgba[4];unsigned source_texture;float source_uv[4]; } ArraysVertex;
static int arrays_enabled;
static int arrays_lazy_bindings;
static int arrays_client_sessions;
static _Thread_local int arrays_scene_scope;
static _Thread_local int arrays_native_texture;
static _Thread_local unsigned arrays_scene_caps_known,arrays_scene_caps_on;
static _Thread_local int arrays_client_session;
static _Thread_local long long arrays_client_setups;
static _Thread_local int arrays_binding_dirty;
static _Thread_local long long arrays_source_binds;
static _Thread_local ArraysVertex arrays_vertices[ARR_CAPACITY];
static _Thread_local float (*arrays_projections)[16],arrays_projection[16];
static _Thread_local int arrays_projection_capture,arrays_projection_dirty,arrays_projection_attempted;
static _Thread_local float arrays_saved_coordinates[4][4];
static _Thread_local int arrays_projection_session;
static _Thread_local unsigned arrays_count;
static _Thread_local int arrays_open, arrays_native_open, arrays_color_known, arrays_uv_known;
static _Thread_local int arrays_color_dirty, arrays_uv_dirty, arrays_has_extended_arrays;
static _Thread_local int arrays_allowed_cached=-1;
static _Thread_local float arrays_color[4], arrays_uv[4];
static _Thread_local unsigned arrays_texture;
static _Thread_local unsigned arrays_output_texture;
static _Thread_local int arrays_layer_current,arrays_output_layered;
static _Thread_local FpsTextureLayer arrays_layer;
static _Thread_local int arrays_context_checked, arrays_context_ok, arrays_has_vbo, arrays_tex_units=1;
static _Thread_local int arrays_has_vao;
static _Thread_local int arrays_matrix_mode=0x1700;
static _Thread_local long long arrays_guard_checks;
static _Thread_local HGLRC arrays_context;
static _Thread_local long long arrays_draw_ticks, arrays_draw_calls, arrays_replays, arrays_vertex_count;
static void (*arrays_get_int)(unsigned,int *);
static void (*arrays_get_float)(unsigned,float *);
static const unsigned char *(*arrays_get_string)(unsigned);
static unsigned char (*arrays_is_enabled)(unsigned);
static void (*arrays_push_client)(unsigned);
static void (*arrays_pop_client)(void);
static void (*arrays_enable_client)(unsigned);
static void (*arrays_disable_client)(unsigned);
static void (*arrays_vertex_pointer)(int,unsigned,int,const void *);
static void (*arrays_tex_pointer)(int,unsigned,int,const void *);
static void (*arrays_color_pointer)(int,unsigned,int,const void *);
static void (*arrays_draw)(unsigned,int,int);
static void (*arrays_tex2)(float,float);
static void (*arrays_tex4)(float,float,float,float);
static void (*arrays_vertex3)(float,float,float);
static _Thread_local void (*arrays_client_active)(unsigned);
static _Thread_local void (*arrays_multi_tex4)(unsigned,float,float,float,float);
static void (*arrays_raw_matrix_mode)(unsigned);
static void (*arrays_load_matrix)(const float *);
static HGLRC (WINAPI *arrays_current_context)(void);
#ifdef _WIN32
static PROC (WINAPI *arrays_wgl_proc)(LPCSTR);
#else
static HMODULE arrays_gl;
#endif

static int arrays_extension(const char *list,const char *name) {
    size_t len=strlen(name);const char *p=list;
    while(p && (p=strstr(p,name))) { if((p==list || p[-1]==' ') && (!p[len] || p[len]==' ')) return 1;p+=len; }
    return 0;
}
static int arrays_check_context(void) {
    HGLRC context=arrays_current_context();
    if(arrays_context_checked && context==arrays_context) return arrays_context_ok;
    arrays_context_checked=1;arrays_context=context;arrays_context_ok=0;
    arrays_color_known=0;arrays_uv_known=0;arrays_has_vbo=0;arrays_tex_units=1;arrays_has_extended_arrays=0;arrays_has_vao=0;
    if(!context) {fps_note_fallback(FPS_NO_CONTEXT);return 0;}
    const char *version=(const char *)arrays_get_string(0x1f02);
    int major=0,minor=0;if(!version || sscanf(version,"%d.%d",&major,&minor)!=2) {fps_note_fallback(FPS_GL_VERSION);return 0;}
    if(major>3 || (major==3 && minor>=2)) {
        int profile=0;arrays_get_int(0x9126,&profile);
        if((profile&1) && !(profile&2)){fps_note_fallback(FPS_CORE_PROFILE);return 0;}
    }
    const char *ext=(const char *)arrays_get_string(0x1f03);
    arrays_get_float(0x0b00,arrays_color);arrays_color_known=1;
    arrays_get_float(0x0b03,arrays_uv);arrays_uv_known=1;
    int texture=0;arrays_get_int(0x8069,&texture);arrays_texture=(unsigned)texture;
    arrays_has_extended_arrays=major>1 || (major==1 && minor>=4);
    arrays_has_vbo=major>1 || (major==1 && minor>=5) || arrays_extension(ext,"GL_ARB_vertex_buffer_object");
#ifdef __APPLE__
    arrays_has_vao=major>=3 || arrays_extension(ext,"GL_ARB_vertex_array_object") || arrays_extension(ext,"GL_APPLE_vertex_array_object");
#else
    arrays_has_vao=major>=3 || arrays_extension(ext,"GL_ARB_vertex_array_object");
#endif
    if(major>1 || (major==1 && minor>=3) || arrays_extension(ext,"GL_ARB_multitexture")) {
#ifdef _WIN32
        arrays_client_active=(void (*)(unsigned))(void (*)(void))arrays_wgl_proc("glClientActiveTexture");
        if(!arrays_client_active) arrays_client_active=(void (*)(unsigned))(void (*)(void))arrays_wgl_proc("glClientActiveTextureARB");
#else
        arrays_client_active=(void (*)(unsigned))GetProcAddress(arrays_gl,"glClientActiveTexture");
        if(!arrays_client_active) arrays_client_active=(void (*)(unsigned))GetProcAddress(arrays_gl,"glClientActiveTextureARB");
#endif
        if((uintptr_t)arrays_client_active<=3 || (uintptr_t)arrays_client_active==UINTPTR_MAX) {fps_note_fallback(FPS_ENTRY_POINT);return 0;}
#ifdef _WIN32
        arrays_multi_tex4=(void (*)(unsigned,float,float,float,float))(void (*)(void))arrays_wgl_proc("glMultiTexCoord4f");
#else
        arrays_multi_tex4=(void (*)(unsigned,float,float,float,float))GetProcAddress(arrays_gl,"glMultiTexCoord4f");
#endif
        if((uintptr_t)arrays_multi_tex4<=3 || (uintptr_t)arrays_multi_tex4==UINTPTR_MAX)arrays_multi_tex4=NULL;
        arrays_get_int(0x84e2,&arrays_tex_units);
        if(arrays_tex_units<1 || arrays_tex_units>32) {fps_note_fallback(FPS_TEXTURE_UNIT);return 0;}
    }
    arrays_context_ok=1;return 1;
}
static void arrays_restore_binding(void) {
    if(arrays_binding_dirty) {
        real_gl_bind(0x0de1,arrays_texture);arrays_source_binds++;arrays_binding_dirty=0;
    }
}
static void arrays_restore_current(void) {
    arrays_restore_binding();
    if(arrays_color_known && arrays_color_dirty) real_gl_color(arrays_color[0],arrays_color[1],arrays_color[2],arrays_color[3]);
    if(arrays_uv_known && arrays_uv_dirty) arrays_tex4(arrays_uv[0],arrays_uv[1],arrays_uv[2],arrays_uv[3]);
    arrays_color_dirty=0;arrays_uv_dirty=0;
}
static void arrays_replay(void) {
    fps_note_fallback(FPS_BATCH_REPLAY);
    unsigned bound=0,env=(unsigned)ta_env_mode;int open=0,projected=0,saved_mode=0;
    float saved_projection[16];
    for(unsigned i=0;arrays_output_layered && i<arrays_count;i++)if(arrays_vertices[i].uv[2]<0){projected=1;break;}
    if(projected){arrays_get_int(0x0ba0,&saved_mode);arrays_get_float(0x0ba7,saved_projection);}
    for(unsigned i=0;i<arrays_count;i++) {
        ArraysVertex *v=&arrays_vertices[i];
        unsigned source_env=arrays_output_layered?(v->uv[3]<0?0x1e01u:0x2100u):(unsigned)ta_env_mode;
        if(!open || (!(i%4) && (projected || bound!=v->source_texture || env!=source_env))) {
            if(open) real_gl_end();
            if(projected){arrays_raw_matrix_mode(0x1701);arrays_load_matrix(v->uv[2]<0?arrays_projections[i]:saved_projection);arrays_raw_matrix_mode((unsigned)saved_mode);}
            if(env!=source_env){real_gl_tex_env(0x2300,0x2200,(int)source_env);env=source_env;}
            real_gl_bind(0x0de1,v->source_texture);real_gl_begin(7);open=1;bound=v->source_texture;
        }
        real_gl_color(v->rgba[0],v->rgba[1],v->rgba[2],v->rgba[3]);
        arrays_tex4(v->source_uv[0],v->source_uv[1],v->source_uv[2],v->source_uv[3]);
        arrays_vertex3(v->xyz[0],v->xyz[1],v->xyz[2]);
    }
    if(open) real_gl_end();
    if(projected){arrays_raw_matrix_mode(0x1701);arrays_load_matrix(saved_projection);arrays_raw_matrix_mode((unsigned)saved_mode);}
    if(env!=(unsigned)ta_env_mode)real_gl_tex_env(0x2300,0x2200,ta_env_mode);
    real_gl_bind(0x0de1,arrays_texture);arrays_replays++;
}
static void arrays_end_client_session(void) {
    if(arrays_client_session) {
        arrays_pop_client();arrays_client_session=0;
        if(arrays_projection_session){
            for(unsigned i=0;i<4;i++){float *v=arrays_saved_coordinates[i];arrays_multi_tex4(0x84c1u+i,v[0],v[1],v[2],v[3]);}
            arrays_projection_session=0;
        }
    }
}
static void arrays_begin_client_session(void) {
    if(arrays_client_session)return;
    arrays_push_client(0x00000002u);
    arrays_disable_client(0x8075);arrays_disable_client(0x8077);arrays_disable_client(0x8079);
    if(arrays_has_extended_arrays) { arrays_disable_client(0x8457);arrays_disable_client(0x845e); }
    for(int unit=arrays_tex_units-1;unit>=0;unit--) {
        if(arrays_client_active)arrays_client_active(0x84c0u+(unsigned)unit);
        arrays_disable_client(0x8078);
    }
    arrays_enable_client(0x8074);arrays_enable_client(0x8076);arrays_enable_client(0x8078);
    arrays_vertex_pointer(3,0x1406,sizeof(ArraysVertex),arrays_vertices[0].xyz);
    arrays_color_pointer(4,0x1406,sizeof(ArraysVertex),arrays_vertices[0].rgba);
    arrays_tex_pointer(4,0x1406,sizeof(ArraysVertex),arrays_vertices[0].uv);
    if(arrays_scene_scope && !arrays_native_texture && arrays_projections && arrays_tex_units>=5 && arrays_client_active && arrays_multi_tex4){
        for(unsigned column=0;column<4;column++){
            ta_active_texture(0x84c1u+column);arrays_get_float(0x0b03,arrays_saved_coordinates[column]);
            arrays_client_active(0x84c1u+column);arrays_enable_client(0x8078);
            arrays_tex_pointer(4,0x1406,sizeof(arrays_projections[0]),arrays_projections[0]+column*4);
        }
        arrays_client_active(0x84c0);ta_active_texture(0x84c0);arrays_projection_session=1;
    }
    arrays_client_session=1;arrays_client_setups++;
}
static void arrays_flush(void) {
    terrain_flush();
    if(!arrays_count) { if(!arrays_open && !arrays_native_open) arrays_restore_current();return; }
    long long begin=now_ticks();
    int complete=(arrays_count%4)==0;
    if(!arrays_output_layered)array_texture_end_draw();
    int shader_ok=complete && (arrays_client_session || ta_client_stack_room()) &&
        (!arrays_output_layered || array_texture_begin_draw(&ta_sprite_cache,arrays_output_texture));
    if(!shader_ok) {
        array_texture_end_draw();
        arrays_end_client_session();
        TaReplaySampling saved={0};
        if(arrays_output_layered)saved=ta_begin_native_replay();
        arrays_replay();
        if(arrays_output_layered)ta_end_native_replay(saved);
    }
    else {
        if(arrays_output_texture && !arrays_output_layered) real_gl_bind(0x0de1,arrays_output_texture);
        arrays_begin_client_session();
        arrays_draw(7,0,(int)arrays_count);
        if(!arrays_client_sessions)arrays_end_client_session();
        if(arrays_output_texture && !arrays_output_layered) real_gl_bind(0x0de1,arrays_texture);
        arrays_draw_calls++;
    }
    if(arrays_output_layered && shader_ok && !ta_multi_page) array_texture_end_draw();
    arrays_vertex_count+=arrays_count;arrays_count=0;arrays_color_dirty=1;arrays_uv_dirty=1;
    arrays_restore_current();arrays_draw_ticks+=now_ticks()-begin;
}
static void arrays_end_frame(void) {
    arrays_flush();array_texture_end_draw();arrays_end_client_session();
}
static int arrays_capture_allowed(void) {
    if(arrays_allowed_cached>=0) return arrays_allowed_cached;
    if(!arrays_check_context()) return 0;
    arrays_guard_checks++;
    int binding=0,vao=0,mode=0,list=0,unit=0x84c0;
    if(arrays_has_vbo) arrays_get_int(0x8894,&binding);
    if(arrays_has_vao) arrays_get_int(0x85b5,&vao);
    arrays_get_int(0x0c40,&mode);arrays_get_int(0x0b33,&list);
    if(arrays_client_active) arrays_get_int(0x84e0,&unit);
    if(binding)fps_note_fallback(FPS_VBO_BOUND);
    if(vao)fps_note_fallback(FPS_VAO_BOUND);
    if(mode!=0x1c00 || list)fps_note_fallback(FPS_RENDER_MODE);
    if(unit!=0x84c0)fps_note_fallback(FPS_TEXTURE_UNIT);
    int fixed_state=arrays_is_enabled(0x0b50) || arrays_is_enabled(0x0b57);
    if(fixed_state)fps_note_fallback(FPS_FIXED_FUNCTION);
    arrays_allowed_cached=!(binding || vao || mode!=0x1c00 || list || unit!=0x84c0 || fixed_state);
    if(arrays_native_texture && !ta_allowed_state())arrays_allowed_cached=0;
    return arrays_allowed_cached;
}
static void arrays_invalidate(void) { arrays_allowed_cached=-1; }
static void arrays_begin_frame(void) {
    arrays_end_frame();arrays_invalidate();
    ++ta_sprite_cache.pass;
    if(ta_sprite_cache.pressure && ta_context_ok && ta_cache_owned(ta_current_context()))
        ta_trim_empty_pages(&ta_sprite_cache);
    arrays_scene_caps_known=arrays_scene_caps_on=0;
    arrays_projection_capture=0;arrays_projection_dirty=1;
    if(arrays_enabled && arrays_lazy_bindings) (void)arrays_check_context();
    if(ta_track_state && arrays_enabled && arrays_context_ok) {
        arrays_get_float(0x0b00,arrays_color);arrays_get_float(0x0b03,arrays_uv);
        int texture=0;arrays_get_int(0x8069,&texture);arrays_texture=(unsigned)texture;
        arrays_get_int(0x0ba0,&arrays_matrix_mode);
        arrays_color_known=arrays_uv_known=1;
        if(arrays_scene_scope && !arrays_native_texture && arrays_client_active && arrays_multi_tex4 && arrays_tex_units>=5){
            if(!arrays_projection_attempted){
                arrays_projection_attempted=1;arrays_projections=FPS_ALLOC(ARR_CAPACITY*sizeof(*arrays_projections));
                if(!arrays_projections)fps_note_fallback(FPS_ALLOCATION);
            }
            float model[16];arrays_get_float(0x0ba6,model);arrays_projection_capture=arrays_projections && !arrays_is_enabled(0x0b71);
            for(int i=0;i<16;i++)if(model[i]!=((i%5)==0?1.0f:0.0f))arrays_projection_capture=0;
        }
    }
}
static void arrays_begin_probe(unsigned mode) {
    if(arrays_enabled && decor_scope && mode==7 && !arrays_native_open && !arrays_open) {
        if(arrays_count || arrays_capture_allowed()) {
            FpsTextureLayer layer;
            int layered=arrays_native_texture?0:array_texture_prepare(arrays_texture,&layer);
            if(layered && !ta_capture_sampling_allowed(!arrays_count))layered=0;
            if(texture_array_enabled && !layered && !arrays_native_texture) {
                arrays_end_frame();arrays_restore_current();
                real_gl_begin(mode);arrays_native_open=1;return;
            }
            unsigned output=layered?layer.array_texture:arrays_texture;
            if(arrays_count && ((arrays_output_texture!=output && !(layered && arrays_output_layered && ta_multi_page)) || arrays_output_layered!=layered)) arrays_flush();
            arrays_output_texture=output;arrays_output_layered=layered;arrays_layer_current=layered;
            if(layered) arrays_layer=layer;
            if(!arrays_color_known) { arrays_get_float(0x0b00,arrays_color);arrays_color_known=1; }
            if(!arrays_uv_known) { arrays_get_float(0x0b03,arrays_uv);arrays_uv_known=1; }
            arrays_open=1;return;
        }
    }
    arrays_end_frame();arrays_restore_current();real_gl_begin(mode);arrays_native_open=1;
}
static void arrays_end_probe(void) {
    if(arrays_open) { arrays_open=0;if(arrays_count%4)arrays_flush();return; }
    real_gl_end();arrays_native_open=0;
}
static void arrays_tex2_probe(float s,float t) {
    arrays_uv[0]=s;arrays_uv[1]=t;arrays_uv[2]=0;arrays_uv[3]=1;arrays_uv_known=1;
    if(arrays_open) { arrays_uv_dirty=1;return; }
    if(!arrays_native_open) arrays_flush();
    arrays_tex2(s,t);
}
static void arrays_vertex3_probe(float x,float y,float z) {
    if(!arrays_open) { if(!arrays_native_open) arrays_flush();arrays_vertex3(x,y,z);return; }
    if(arrays_count==ARR_CAPACITY) arrays_flush();
    unsigned index=arrays_count++;ArraysVertex *v=&arrays_vertices[index];v->xyz[0]=x;v->xyz[1]=y;v->xyz[2]=z;
    memcpy(v->uv,arrays_uv,sizeof(v->uv));memcpy(v->rgba,arrays_color,sizeof(v->rgba));
    v->source_texture=arrays_texture;memcpy(v->source_uv,arrays_uv,sizeof(v->source_uv));
    if(arrays_layer_current){
        v->uv[2]=(float)arrays_layer.layer;v->uv[3]=ta_multi_page?(float)arrays_layer.page:0.0f;
        if(ta_scene_replace && ta_env_mode==0x1e01)v->uv[3]=-1.0f-v->uv[3];
    }
    if(arrays_layer_current && arrays_projection_capture){
        if(arrays_projection_dirty){arrays_get_float(0x0ba7,arrays_projection);arrays_projection_dirty=0;}
        memcpy(arrays_projections[index],arrays_projection,sizeof(arrays_projection));v->uv[2]=-1.0f-v->uv[2];
    }
}
static void arrays_color_probe(float r,float g,float b,float a) {
    if(arrays_enabled && in_highest && !arrays_native_open) (void)arrays_check_context();
    arrays_color[0]=r;arrays_color[1]=g;arrays_color[2]=b;arrays_color[3]=a;arrays_color_known=1;
    if(arrays_enabled && in_highest && !arrays_native_open && arrays_context_ok) { arrays_color_dirty=1;return; }
    if(!arrays_native_open) arrays_flush();
    real_gl_color(r,g,b,a);
}
static void arrays_color3_probe(float r,float g,float b) { arrays_color_probe(r,g,b,1); }
static void arrays_bind_probe(unsigned target,unsigned texture) {
#ifdef _WIN32
    if(terrain_scope && target==0x0de1) {
        real_gl_bind(target,texture);arrays_texture=texture;arrays_binding_dirty=0;return;
    }
#endif
    if(texture_array_enabled && arrays_enabled && in_highest && !arrays_native_open && target==0x0de1) {
        if(arrays_lazy_bindings && arrays_context_checked && arrays_context_ok) {
            arrays_texture=texture;arrays_binding_dirty=1;return;
        }
        real_gl_bind(target,texture);arrays_source_binds++;arrays_texture=texture;arrays_binding_dirty=0;return;
    }
    if(arrays_count && target==0x0de1 && texture==arrays_texture) return;
    arrays_flush();real_gl_bind(target,texture);
    if(target==0x0de1) { arrays_source_binds++;arrays_texture=texture;arrays_binding_dirty=0; }
}

#define ARR_INVALIDATE() do { arrays_invalidate();array_texture_invalidate_state(); } while(0)
#define ARR_AFTER_STATE() do {if(!ta_track_state)ARR_INVALIDATE();}while(0)
static void arrays_matrix_changed(void) {
    if(arrays_scene_scope && !arrays_native_texture){
        if(arrays_matrix_mode==0x1701)arrays_projection_dirty=1;
        if(arrays_matrix_mode==0x1700){
            float model[16];arrays_get_float(0x0ba6,model);arrays_projection_capture=arrays_projections && arrays_client_active && arrays_multi_tex4 && arrays_tex_units>=5 && !arrays_is_enabled(0x0b71);
            for(int i=0;i<16;i++)if(model[i]!=((i%5)==0?1.0f:0.0f))arrays_projection_capture=0;
        }
    }
    if(!ta_track_state){ARR_INVALIDATE();return;}
    if(arrays_matrix_mode==0x1702)array_texture_invalidate_state();
}
static void arrays_matrix_boundary(void) {
    if(arrays_scene_scope && arrays_projection_capture && arrays_matrix_mode==0x1701 && !arrays_native_open)return;
    if(arrays_scene_scope && arrays_matrix_mode!=0x1702 && !arrays_native_open)arrays_flush();
    else arrays_end_frame();
}
#define ARR_WRAP0(name) static void (*arrays_real_##name)(void); static void arrays_##name(void){ arrays_end_frame();arrays_real_##name();ARR_AFTER_STATE(); }
#define ARR_WRAP1(name,T) static void (*arrays_real_##name)(T); static void arrays_##name(T a){ arrays_end_frame();arrays_real_##name(a);ARR_AFTER_STATE(); }
#define ARR_WRAP2(name,T,U) static void (*arrays_real_##name)(T,U); static void arrays_##name(T a,U b){ arrays_end_frame();arrays_real_##name(a,b);ARR_AFTER_STATE(); }
#define ARR_WRAP3(name,T,U,V) static void (*arrays_real_##name)(T,U,V); static void arrays_##name(T a,U b,V c){ arrays_end_frame();arrays_real_##name(a,b,c);ARR_AFTER_STATE(); }
#define ARR_WRAP4(name,T,U,V,W) static void (*arrays_real_##name)(T,U,V,W); static void arrays_##name(T a,U b,V c,W d){ arrays_end_frame();arrays_real_##name(a,b,c,d);ARR_AFTER_STATE(); }
#define ARR_MATRIX0(name) static void (*arrays_real_##name)(void);static void arrays_##name(void){arrays_matrix_boundary();arrays_real_##name();arrays_matrix_changed();}
#define ARR_MATRIX3(name) static void (*arrays_real_##name)(float,float,float);static void arrays_##name(float a,float b,float c){arrays_matrix_boundary();arrays_real_##name(a,b,c);arrays_matrix_changed();}
ARR_MATRIX0(glLoadIdentity) ARR_MATRIX0(glPopMatrix)
static void (*arrays_real_glPushMatrix)(void);
static void arrays_glPushMatrix(void){arrays_real_glPushMatrix();}
ARR_WRAP0(glFinish) ARR_WRAP0(glFlush)
static void (*arrays_real_glMatrixMode)(unsigned);
static void arrays_glMatrixMode(unsigned mode){
    terrain_flush();
    arrays_real_glMatrixMode(mode);
    if(mode>=0x1700 && mode<=0x1702)arrays_matrix_mode=(int)mode;
    else {arrays_get_int(0x0ba0,&arrays_matrix_mode);ARR_INVALIDATE();}
    ARR_AFTER_STATE();
}
ARR_WRAP1(glClear,unsigned)
ARR_WRAP1(glEnableClientState,unsigned) ARR_WRAP1(glDisableClientState,unsigned) ARR_WRAP1(glLineWidth,float)
ARR_WRAP2(glPixelStorei,unsigned,int)

static void arrays_fragment_boundary(void) {
    if(arrays_scene_scope && !arrays_native_open)arrays_flush();else arrays_end_frame();
}
static void (*arrays_real_glDepthMask)(unsigned char);
static void arrays_glDepthMask(unsigned char value){arrays_fragment_boundary();arrays_real_glDepthMask(value);ARR_AFTER_STATE();}
static void (*arrays_real_glBlendFunc)(unsigned,unsigned);
static void arrays_glBlendFunc(unsigned source,unsigned destination){arrays_fragment_boundary();arrays_real_glBlendFunc(source,destination);ARR_AFTER_STATE();}
static void (*arrays_real_glAlphaFunc)(unsigned,float);
static void arrays_glAlphaFunc(unsigned function,float reference){arrays_fragment_boundary();arrays_real_glAlphaFunc(function,reference);ARR_AFTER_STATE();}
ARR_WRAP2(glFogfv,unsigned,const float *) ARR_WRAP2(glFogf,unsigned,float) ARR_WRAP2(glFogi,unsigned,int)
ARR_WRAP2(glHint,unsigned,unsigned) ARR_WRAP2(glLightModeli,unsigned,int) ARR_WRAP2(glColorMaterial,unsigned,unsigned)
static void (*arrays_real_glDeleteTextures)(int,const unsigned *);
static void arrays_glDeleteTextures(int n,const unsigned *ids) {
    arrays_end_frame();
    if(ta_current_context && ta_cache_owned(ta_current_context()))
        for(int i=0;i<n;i++)array_texture_remove(ids[i]);
    arrays_real_glDeleteTextures(n,ids);
    for(int i=0;i<n;i++)if(ids[i]==arrays_texture){arrays_texture=0;arrays_binding_dirty=0;}
}
ARR_MATRIX3(glScalef) ARR_MATRIX3(glTranslatef)
static void (*arrays_real_glTexEnvi)(unsigned,unsigned,int);
static void arrays_glTexEnvi(unsigned target,unsigned name,int value) {
    if(target==0x2300 && name==0x2200 && (value==0x2100 || value==0x1e01)){
        if(!arrays_scene_scope || !texture_array_enabled || arrays_native_open || (arrays_count && !arrays_output_layered))arrays_fragment_boundary();
    }else arrays_end_frame();
    arrays_real_glTexEnvi(target,name,value);
    if(target==0x2300 && name==0x2200){ta_modulate=value==0x2100;ta_env_mode=value;}
    if(target==0x8500 && name==0x8501)array_texture_invalidate_state();
    ARR_AFTER_STATE();
}
static void (*arrays_real_glTexParameteri)(unsigned,unsigned,int);
static void arrays_forget_bound_texture(unsigned target) {
    if(target!=0x0de1 || !ta_current_context || !ta_cache_owned(ta_current_context()))return;
    int binding=0;
    arrays_get_int(0x8069,&binding);
    array_texture_forget((unsigned)binding);
}
static void arrays_glTexParameteri(unsigned target,unsigned name,int value) {
    arrays_end_frame();
    arrays_forget_bound_texture(target);
    arrays_real_glTexParameteri(target,name,value);
    ARR_AFTER_STATE();
}
ARR_WRAP3(glMaterialfv,unsigned,unsigned,const float *) ARR_WRAP3(glLightfv,unsigned,unsigned,const float *)
static void (*arrays_real_glRotatef)(float,float,float,float);
static void arrays_glRotatef(float a,float b,float c,float d){arrays_matrix_boundary();arrays_real_glRotatef(a,b,c,d);arrays_matrix_changed();}
ARR_WRAP4(glViewport,int,int,int,int) ARR_WRAP4(glScissor,int,int,int,int)
ARR_WRAP4(glClearColor,float,float,float,float)
ARR_WRAP4(glVertexPointer,int,unsigned,int,const void *) ARR_WRAP4(glTexCoordPointer,int,unsigned,int,const void *)
ARR_WRAP1(glShadeModel,unsigned)
static void (*arrays_real_glEnable)(unsigned);
static void arrays_cap_boundary(unsigned cap,int on) {
    const unsigned caps[]={0x0b44,0x0be2,0x0bc0,0x0b71,0x0c11};
    for(unsigned i=0;i<5;i++)if(cap==caps[i]){
        if(arrays_scene_scope && !arrays_native_open){
            unsigned bit=1u<<i;
            if(!(arrays_scene_caps_known&bit)){
                arrays_scene_caps_known|=bit;
                if(arrays_is_enabled(cap))arrays_scene_caps_on|=bit;
            }
            int was_on=(arrays_scene_caps_on&bit)!=0;
            if(on)arrays_scene_caps_on|=bit;else arrays_scene_caps_on&=~bit;
            if(was_on==on)return;
        }
        arrays_fragment_boundary();return;
    }
    arrays_end_frame();
}
static void arrays_glEnable(unsigned cap) {arrays_cap_boundary(cap,1);arrays_real_glEnable(cap);ta_note_cap(cap,1);if(arrays_scene_scope && cap==0x0b71)arrays_projection_capture=0;if(cap==0x0b50 || cap==0x0b57)arrays_invalidate();ARR_AFTER_STATE();}
static void (*arrays_real_glDisable)(unsigned);
static void arrays_glDisable(unsigned cap) {arrays_cap_boundary(cap,0);arrays_real_glDisable(cap);ta_note_cap(cap,0);if(cap==0x0b50 || cap==0x0b57)arrays_invalidate();ARR_AFTER_STATE();}
static void (*arrays_real_glDrawArrays)(unsigned,int,int);
static void arrays_glDrawArrays(unsigned mode,int first,int count) {arrays_end_frame();arrays_real_glDrawArrays(mode,first,count);arrays_color_known=0;arrays_uv_known=0;}
static void (*arrays_real_glOrtho)(double,double,double,double,double,double);
static void arrays_glOrtho(double a,double b,double c,double d,double e,double f){arrays_matrix_boundary();arrays_real_glOrtho(a,b,c,d,e,f);arrays_matrix_changed();}
static void (*arrays_real_glTexImage2D)(unsigned,int,int,int,int,int,unsigned,unsigned,const void *);
static void arrays_glTexImage2D(unsigned target,int level,int internal,int width,int height,int border,unsigned format,unsigned type,const void *pixels) {
    arrays_end_frame();
    arrays_forget_bound_texture(target);
    arrays_real_glTexImage2D(target,level,internal,width,height,border,format,type,pixels);
    ARR_AFTER_STATE();
}
static int (WINAPI *arrays_real_gluBuild2DMipmaps)(unsigned,int,int,int,unsigned,unsigned,const void *);
#ifdef _WIN32
#include "map_mipmaps.h"
#endif
static int WINAPI arrays_gluBuild2DMipmaps(unsigned target,int components,int width,int height,unsigned format,unsigned type,const void *pixels) {
    arrays_end_frame();
    arrays_forget_bound_texture(target);
#ifdef _WIN32
    if(map_build_mips(target,components,width,height,format,type,pixels)){ARR_AFTER_STATE();return 0;}
#endif
    int result=arrays_real_gluBuild2DMipmaps(target,components,width,height,format,type,pixels);
    ARR_AFTER_STATE();return result;
}
static unsigned (*arrays_real_glGetError)(void);
static unsigned arrays_glGetError(void){
    arrays_flush();
    return arrays_real_glGetError();
}
#undef ARR_WRAP0
#undef ARR_WRAP1
#undef ARR_WRAP2
#undef ARR_WRAP3
#undef ARR_WRAP4
#undef ARR_MATRIX0
#undef ARR_MATRIX3

typedef struct { uintptr_t rva;void *hook;const char *name;void *save; } FpsGlImport;
static FpsGlImport arrays_imports[]={
        {RVA_FPS_GL_BEGIN_IAT,(void *)arrays_begin_probe,"glBegin",NULL},{RVA_FPS_GL_END_IAT,(void *)arrays_end_probe,"glEnd",NULL},
        {RVA_FPS_GL_TEX_COORD2F_IAT,(void *)arrays_tex2_probe,"glTexCoord2f",NULL},{RVA_FPS_GL_VERTEX3F_IAT,(void *)arrays_vertex3_probe,"glVertex3f",NULL},
        {RVA_GL_COLOR4F,(void *)arrays_color_probe,"glColor4f",NULL},{RVA_FPS_GL_COLOR3F_IAT,(void *)arrays_color3_probe,"glColor3f",NULL},
        {RVA_FPS_GL_BIND_TEXTURE_IAT,(void *)arrays_bind_probe,"glBindTexture",NULL},
#define ARR_HOOK(rva,name) {rva,(void *)arrays_##name,#name,&arrays_real_##name}
        ARR_HOOK(RVA_FPS_GL_ORTHO_IAT,glOrtho),ARR_HOOK(RVA_FPS_GL_GET_ERROR_IAT,glGetError),ARR_HOOK(RVA_FPS_GL_FINISH_IAT,glFinish),
        ARR_HOOK(RVA_FPS_GL_LOAD_IDENTITY_IAT,glLoadIdentity),ARR_HOOK(RVA_FPS_GL_MATRIX_MODE_IAT,glMatrixMode),ARR_HOOK(RVA_FPS_GL_FOGFV_IAT,glFogfv),
        ARR_HOOK(RVA_FPS_GL_FOGF_IAT,glFogf),ARR_HOOK(RVA_FPS_GL_CLEAR_IAT,glClear),ARR_HOOK(RVA_FPS_GL_PIXEL_STOREI_IAT,glPixelStorei),
        ARR_HOOK(RVA_FPS_GL_TEX_IMAGE2_D_IAT,glTexImage2D),ARR_HOOK(RVA_FPS_GL_TEX_PARAMETERI_IAT,glTexParameteri),ARR_HOOK(RVA_FPS_GL_VIEWPORT_IAT,glViewport),
        ARR_HOOK(RVA_FPS_GL_PUSH_MATRIX_IAT,glPushMatrix),ARR_HOOK(RVA_FPS_GL_SCALEF_IAT,glScalef),ARR_HOOK(RVA_FPS_GL_ROTATEF_IAT,glRotatef),
        ARR_HOOK(RVA_FPS_GL_TRANSLATEF_IAT,glTranslatef),ARR_HOOK(RVA_GL_DRAW_ARRAYS,glDrawArrays),ARR_HOOK(RVA_FPS_GL_TEX_COORD_POINTER_IAT,glTexCoordPointer),
        ARR_HOOK(RVA_FPS_GL_DELETE_TEXTURES_IAT,glDeleteTextures),ARR_HOOK(RVA_GL_LINE_WIDTH,glLineWidth),ARR_HOOK(RVA_GL_DISABLE_CLIENT_STATE,glDisableClientState),
        ARR_HOOK(RVA_FPS_GL_SCISSOR_IAT,glScissor),ARR_HOOK(RVA_FPS_GL_FLUSH_IAT,glFlush),ARR_HOOK(RVA_GL_VERTEX_POINTER,glVertexPointer),
        ARR_HOOK(RVA_FPS_GL_POP_MATRIX_IAT,glPopMatrix),ARR_HOOK(RVA_FPS_GL_LIGHT_MODELI_IAT,glLightModeli),ARR_HOOK(RVA_FPS_GL_SHADE_MODEL_IAT,glShadeModel),
        ARR_HOOK(RVA_FPS_GL_MATERIALFV_IAT,glMaterialfv),ARR_HOOK(RVA_FPS_GL_LIGHTFV_IAT,glLightfv),ARR_HOOK(RVA_FPS_GL_DISABLE_IAT,glDisable),
        ARR_HOOK(RVA_FPS_GL_TEX_ENVI_IAT,glTexEnvi),ARR_HOOK(RVA_FPS_GL_COLOR_MATERIAL_IAT,glColorMaterial),ARR_HOOK(RVA_FPS_GL_FOGI_IAT,glFogi),
        ARR_HOOK(RVA_FPS_GL_CLEAR_COLOR_IAT,glClearColor),ARR_HOOK(RVA_FPS_GL_ALPHA_FUNC_IAT,glAlphaFunc),ARR_HOOK(RVA_FPS_GL_HINT_IAT,glHint),
        ARR_HOOK(RVA_FPS_GL_BLEND_FUNC_IAT,glBlendFunc),ARR_HOOK(RVA_FPS_GL_DEPTH_MASK_IAT,glDepthMask),ARR_HOOK(RVA_FPS_GL_ENABLE_IAT,glEnable),
        ARR_HOOK(RVA_GL_ENABLE_CLIENT_STATE,glEnableClientState)
#undef ARR_HOOK
    };

enum { ARRAY_IMPORT_COUNT=sizeof(arrays_imports)/sizeof(arrays_imports[0]) };
typedef struct {void *original[ARRAY_IMPORT_COUNT+1];size_t count;} FpsArrayInstall;
typedef int (*FpsReplaceImport)(uintptr_t,void *,void *);

static int arrays_install_imports(FpsArrayInstall *install,FpsReplaceImport replace) {
    for(;install->count<ARRAY_IMPORT_COUNT;install->count++) {
        size_t i=install->count;
        if(!replace(arrays_imports[i].rva,arrays_imports[i].hook,&install->original[i]))return 0;
    }
    if(!replace(FPS_GLU_SLOT,(void *)arrays_gluBuild2DMipmaps,&install->original[install->count]))return 0;
    install->count++;
    return 1;
}

static int arrays_restore_imports(FpsArrayInstall *install,FpsReplaceImport replace) {
    int restored=1;
    while(install->count) {
        size_t i=--install->count;
        uintptr_t rva=i==ARRAY_IMPORT_COUNT?FPS_GLU_SLOT:arrays_imports[i].rva;
        void *hook=i==ARRAY_IMPORT_COUNT?(void *)arrays_gluBuild2DMipmaps:arrays_imports[i].hook;
        void **slot=gl_import_slot(rva);
        void *current=NULL,*ignored=NULL;
        if(!slot){restored=0;continue;}
        memcpy(&current,slot,sizeof(current));
        if(current==hook && !replace(rva,install->original[i],&ignored))restored=0;
    }
    return restored;
}

// Resolve the native client-array functions without changing game imports.
static int arrays_load_functions(void) {
    HMODULE gl=GetModuleHandleA(FPS_GL_LIBRARY);if(!gl) return 0;
#ifdef __APPLE__
    arrays_gl=gl;
#endif
#define ARR_PROC(var,name) do{ *(void **)&var=(void *)GetProcAddress(gl,name);if(!var)return 0; }while(0)
    ARR_PROC(arrays_get_int,"glGetIntegerv");ARR_PROC(arrays_get_float,"glGetFloatv");ARR_PROC(arrays_get_string,"glGetString");
    ARR_PROC(arrays_is_enabled,"glIsEnabled");ARR_PROC(arrays_push_client,"glPushClientAttrib");ARR_PROC(arrays_pop_client,"glPopClientAttrib");
    ARR_PROC(arrays_enable_client,"glEnableClientState");ARR_PROC(arrays_disable_client,"glDisableClientState");
    ARR_PROC(arrays_vertex_pointer,"glVertexPointer");ARR_PROC(arrays_tex_pointer,"glTexCoordPointer");ARR_PROC(arrays_color_pointer,"glColorPointer");
    ARR_PROC(arrays_draw,"glDrawArrays");ARR_PROC(arrays_tex2,"glTexCoord2f");ARR_PROC(arrays_tex4,"glTexCoord4f");ARR_PROC(arrays_vertex3,"glVertex3f");
    ARR_PROC(arrays_current_context,FPS_GL_CONTEXT_FN);
    ARR_PROC(arrays_raw_matrix_mode,"glMatrixMode");ARR_PROC(arrays_load_matrix,"glLoadMatrixf");
#ifdef _WIN32
    ARR_PROC(arrays_wgl_proc,"wglGetProcAddress");
#endif
#undef ARR_PROC
    return 1;
}

static int install_gl_arrays(FpsArrayInstall *install) {
    arrays_enabled=1;arrays_lazy_bindings=1;arrays_client_sessions=1;
    HMODULE gl=GetModuleHandleA(FPS_GL_LIBRARY);
    if(!arrays_load_functions())return 0;
    for(size_t i=0;i<sizeof(arrays_imports)/sizeof(arrays_imports[0]);i++) {
        void *native=(void *)GetProcAddress(gl,arrays_imports[i].name);if(!native)return 0;
        if(arrays_imports[i].save) memcpy(arrays_imports[i].save,&native,sizeof(native));
    }
#ifdef _WIN32
    memcpy(&arrays_real_gluBuild2DMipmaps,detail_base+RVA_RDATA_SECTION,sizeof(arrays_real_gluBuild2DMipmaps));
#else
    *(void **)&arrays_real_gluBuild2DMipmaps=(void *)GetProcAddress(gl,"gluBuild2DMipmaps");
#endif
    if(!arrays_real_gluBuild2DMipmaps)return 0;
    return arrays_install_imports(install,replace_gl_import);
}
