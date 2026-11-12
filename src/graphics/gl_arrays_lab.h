// Optional, pinned CoE5 5.43 laboratory experiment. Include after gl_batch_lab.h.
#pragma once
// Preserve submission order and per-vertex position, texture coordinates and RGBA.
// Only decoration GL_QUADS are buffered. Everything else is a flush boundary.
#include <stddef.h>
#include "render_diagnostics.h"
#define ARR_CAPACITY 16384u
typedef struct { float xyz[3], uv[4], rgba[4];unsigned source_texture;float source_uv[4]; } ArraysVertex;
static int arrays_enabled;
static int arrays_lazy_bindings;
static int arrays_client_sessions;
static _Thread_local int arrays_client_session;
static _Thread_local long long arrays_client_setups;
static _Thread_local int arrays_binding_dirty;
static _Thread_local long long arrays_source_binds;
static _Thread_local ArraysVertex arrays_vertices[ARR_CAPACITY];
static _Thread_local unsigned arrays_count;
static _Thread_local int arrays_open, arrays_native_open, arrays_color_known, arrays_uv_known;
static _Thread_local int arrays_color_dirty, arrays_uv_dirty, arrays_has_extended_arrays;
static _Thread_local int arrays_allowed_cached=-1;
static _Thread_local float arrays_color[4], arrays_uv[4];
static _Thread_local unsigned arrays_texture;
static _Thread_local unsigned arrays_output_texture;
static _Thread_local int arrays_atlas_current;
static _Thread_local int arrays_layer_current,arrays_output_layered;
static _Thread_local FpsTextureLayer arrays_layer;
static _Thread_local FpsAtlasRect arrays_rect;
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
static void (*arrays_client_active)(unsigned);
static HGLRC (WINAPI *arrays_current_context)(void);
static PROC (WINAPI *arrays_wgl_proc)(LPCSTR);

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
    arrays_has_vao=major>=3 || arrays_extension(ext,"GL_ARB_vertex_array_object");
    if(major>1 || (major==1 && minor>=3) || arrays_extension(ext,"GL_ARB_multitexture")) {
        arrays_client_active=(void (*)(unsigned))arrays_wgl_proc("glClientActiveTexture");
        if(!arrays_client_active) arrays_client_active=(void (*)(unsigned))arrays_wgl_proc("glClientActiveTextureARB");
        if((uintptr_t)arrays_client_active<=3 || (uintptr_t)arrays_client_active==UINTPTR_MAX) {fps_note_fallback(FPS_ENTRY_POINT);return 0;}
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
    color_valid=0;
}
static void arrays_replay(void) {
    fps_note_fallback(FPS_BATCH_REPLAY);
    unsigned bound=0;int open=0;
    for(unsigned i=0;i<arrays_count;i++) {
        ArraysVertex *v=&arrays_vertices[i];
        if(!open || (!(i%4) && bound!=v->source_texture)) {
            if(open) real_gl_end();real_gl_bind(0x0de1,v->source_texture);real_gl_begin(7);open=1;bound=v->source_texture;
        }
        real_gl_color(v->rgba[0],v->rgba[1],v->rgba[2],v->rgba[3]);
        arrays_tex4(v->source_uv[0],v->source_uv[1],v->source_uv[2],v->source_uv[3]);
        arrays_vertex3(v->xyz[0],v->xyz[1],v->xyz[2]);
    }
    if(open) real_gl_end();real_gl_bind(0x0de1,arrays_texture);arrays_replays++;
}
static void arrays_end_client_session(void) {
    if(arrays_client_session) { arrays_pop_client();arrays_client_session=0; }
}
static void arrays_begin_client_session(void) {
    if(arrays_client_session)return;
    arrays_push_client(0x00000002u); // GL_CLIENT_VERTEX_ARRAY_BIT
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
    arrays_client_session=1;arrays_client_setups++;
}
static void arrays_flush(void) {
    if(!arrays_count) { if(!arrays_open && !arrays_native_open) arrays_restore_current();return; }
    long long begin=now_ticks();
    int complete=(arrays_count%4)==0;
    int shader_ok=complete && (!arrays_output_layered || array_texture_begin_draw(arrays_output_texture));
    if(!shader_ok) arrays_replay();
    else {
        if(arrays_output_texture && !arrays_output_layered) real_gl_bind(0x0de1,arrays_output_texture);
        arrays_begin_client_session();
        arrays_draw(7,0,(int)arrays_count); // unchanged QUADS, no reordering
        if(!arrays_client_sessions)arrays_end_client_session();
        if(arrays_output_texture && !arrays_output_layered) real_gl_bind(0x0de1,arrays_texture);
        arrays_draw_calls++;
    }
    if(arrays_output_layered && shader_ok && !ta_multi_page) array_texture_end_draw();
    arrays_vertex_count+=arrays_count;arrays_count=0;arrays_color_dirty=1;arrays_uv_dirty=1;
    arrays_restore_current();arrays_draw_ticks+=now_ticks()-begin;
}
static void arrays_end_frame(void) { arrays_flush();array_texture_end_draw();arrays_end_client_session(); }
static int arrays_capture_allowed(void) {
    if(arrays_allowed_cached>=0) return arrays_allowed_cached;
    if(!arrays_check_context()) return 0;
    arrays_guard_checks++;
    int binding=0,vao=0,mode=0,list=0,unit=0x84c0;
    if(arrays_has_vbo) arrays_get_int(0x8894,&binding);
    if(arrays_has_vao) arrays_get_int(0x85b5,&vao);
    arrays_get_int(0x0c40,&mode);arrays_get_int(0x0b33,&list);
    if(arrays_client_active) arrays_get_int(0x84e0,&unit);
    // Fixed-function 2D only. No VBO binding changes or VBO/FBO ownership.
    if(binding)fps_note_fallback(FPS_VBO_BOUND);
    if(vao)fps_note_fallback(FPS_VAO_BOUND);
    if(mode!=0x1c00 || list)fps_note_fallback(FPS_RENDER_MODE);
    if(unit!=0x84c0)fps_note_fallback(FPS_TEXTURE_UNIT);
    int fixed_state=arrays_is_enabled(0x0b50) || arrays_is_enabled(0x0b57);
    if(fixed_state)fps_note_fallback(FPS_FIXED_FUNCTION);
    arrays_allowed_cached=!(binding || vao || mode!=0x1c00 || list || unit!=0x84c0 || fixed_state);
    return arrays_allowed_cached;
}
// Required once at the start of either map-decoration pass. The pinned game does not
// import BindBuffer/RenderMode/NewList/ActiveTexture. If another mod uses those
// through an extension pointer it must call arrays_flush before mutating them
// and arrays_invalidate afterwards; unobserved third-party GL mutation is unsupported.
static void arrays_invalidate(void) { arrays_allowed_cached=-1; }
static void arrays_begin_frame(void) {
    arrays_end_frame();arrays_invalidate();
    if(arrays_enabled && arrays_lazy_bindings) (void)arrays_check_context();
    if(ta_track_state && arrays_enabled && arrays_context_ok) {
        arrays_get_float(0x0b00,arrays_color);arrays_get_float(0x0b03,arrays_uv);
        int texture=0;arrays_get_int(0x8069,&texture);arrays_texture=(unsigned)texture;
        arrays_get_int(0x0ba0,&arrays_matrix_mode);
        arrays_color_known=arrays_uv_known=1;
    }
}
static void arrays_begin_probe(unsigned mode) {
#ifndef FPS_SE_MOD
    if(arrays_enabled && decor_scope && mode==7) {
        static unsigned seen[12],seen_count;
        int known=0;for(unsigned i=0;i<seen_count;i++) if(seen[i]==arrays_texture) known=1;
        if(!known && seen_count<12) {
            arrays_flush();seen[seen_count++]=arrays_texture;
            typedef void (*GetParam)(unsigned,unsigned,int *);
            typedef void (*GetLevel)(unsigned,int,unsigned,int *);
            HMODULE module=GetModuleHandleA("opengl32.dll");
            GetParam gp=(GetParam)GetProcAddress(module,"glGetTexParameteriv");
            GetLevel gl=(GetLevel)GetProcAddress(module,"glGetTexLevelParameteriv");
            int min=0,mag=0,s=0,t=0,w=0,h=0;
            gp(0x0de1,0x2801,&min);gp(0x0de1,0x2800,&mag);gp(0x0de1,0x2802,&s);gp(0x0de1,0x2803,&t);
            gl(0x0de1,0,0x1000,&w);gl(0x0de1,0,0x1001,&h);
            char msg[256];snprintf(msg,sizeof(msg),"fps_texture id=%u width=%d height=%d min=%x mag=%x wrap=%x,%x",arrays_texture,w,h,min,mag,s,t);
            plugin_api->logLine((const void *)arrays_begin_probe,msg);
        }
    }
#endif
    if(arrays_enabled && decor_scope && mode==7 && !arrays_native_open && !arrays_open) {
        if(arrays_count || arrays_capture_allowed()) {
            FpsAtlasRect rect;
            FpsTextureLayer layer;
            int layered=array_texture_prepare(arrays_texture,&layer);
            int mapped=!layered && atlas_prepare(arrays_texture,&rect);
            // Unsupported textures/states keep the original immediate-mode
            // path. Buffering one tiny legacy batch per texture can be slower.
            if(texture_array_enabled && !layered && !mapped) {
                arrays_end_frame();batch_flush();arrays_restore_current();
                real_gl_begin(mode);arrays_native_open=1;return;
            }
            unsigned output=layered?layer.array_texture:(mapped?rect.texture:arrays_texture);
            if(arrays_count && ((arrays_output_texture!=output && !(layered && arrays_output_layered && ta_multi_page)) || arrays_output_layered!=layered)) arrays_flush();
            arrays_output_texture=output;arrays_output_layered=layered;arrays_layer_current=layered;
            arrays_atlas_current=mapped;if(mapped) arrays_rect=rect;if(layered) arrays_layer=layer;
            if(!arrays_color_known) { arrays_get_float(0x0b00,arrays_color);arrays_color_known=1; }
            if(!arrays_uv_known) { arrays_get_float(0x0b03,arrays_uv);arrays_uv_known=1; }
            arrays_open=1;return;
        }
    }
    arrays_end_frame();batch_flush();arrays_restore_current();real_gl_begin(mode);arrays_native_open=1;
}
static void arrays_end_probe(void) {
    if(arrays_open) { arrays_open=0;if(arrays_count%4)arrays_flush();return; }
    real_gl_end();arrays_native_open=0;
}
static void arrays_tex2_probe(float s,float t) {
    arrays_uv[0]=s;arrays_uv[1]=t;arrays_uv[2]=0;arrays_uv[3]=1;arrays_uv_known=1;
    if(arrays_open) { arrays_uv_dirty=1;return; }
    if(!arrays_native_open) arrays_flush();arrays_tex2(s,t);
}
static void arrays_vertex3_probe(float x,float y,float z) {
    if(!arrays_open) { if(!arrays_native_open) arrays_flush();arrays_vertex3(x,y,z);return; }
    // Capacity is a multiple of four; never split a quad across draw calls.
    if(arrays_count==ARR_CAPACITY) arrays_flush();
    ArraysVertex *v=&arrays_vertices[arrays_count++];v->xyz[0]=x;v->xyz[1]=y;v->xyz[2]=z;
    memcpy(v->uv,arrays_uv,sizeof(v->uv));memcpy(v->rgba,arrays_color,sizeof(v->rgba));
    v->source_texture=arrays_texture;memcpy(v->source_uv,arrays_uv,sizeof(v->source_uv));
    if(arrays_layer_current){v->uv[2]=(float)arrays_layer.layer;if(ta_multi_page)v->uv[3]=(float)arrays_layer.page;}
    if(arrays_atlas_current) {
        float mapped[4];
        if(atlas_remap_uv(&arrays_rect,arrays_uv,mapped)) memcpy(v->uv,mapped,sizeof(mapped));
    }
}
static void arrays_color_probe(float r,float g,float b,float a) {
    if(arrays_enabled && in_highest && !arrays_native_open) (void)arrays_check_context();
    arrays_color[0]=r;arrays_color[1]=g;arrays_color[2]=b;arrays_color[3]=a;arrays_color_known=1;
    if(arrays_enabled && in_highest && !arrays_native_open && arrays_context_ok) { arrays_color_dirty=1;return; }
    if(!arrays_native_open) arrays_flush();real_gl_color(r,g,b,a);color_valid=0;
}
static void arrays_color3_probe(float r,float g,float b) { arrays_color_probe(r,g,b,1); }
static void arrays_bind_probe(unsigned target,unsigned texture) {
    if((atlas_enabled || texture_array_enabled) && arrays_enabled && in_highest && !arrays_native_open && target==0x0de1) {
        // Array-texture capture uses the logical 2D name and binds sources
        // explicitly only while cloning them. Delay legacy 2D bindings until
        // a flush/native boundary, where the original state is restored.
        if(arrays_lazy_bindings && arrays_context_checked && arrays_context_ok) {
            arrays_texture=texture;arrays_binding_dirty=1;return;
        }
        real_gl_bind(target,texture);arrays_source_binds++;arrays_texture=texture;arrays_binding_dirty=0;return;
    }
    // Rebinding the same already-bound 2D name cannot change pending vertices.
    if(arrays_count && target==0x0de1 && texture==arrays_texture) return;
    arrays_flush();real_gl_bind(target,texture);
    if(target==0x0de1) { arrays_source_binds++;arrays_texture=texture;arrays_binding_dirty=0; }
}

// State changes and native DrawArrays always submit pending decorations first.
#define ARR_INVALIDATE() do { arrays_invalidate();atlas_invalidate_state();array_texture_invalidate_state(); } while(0)
#define ARR_AFTER_STATE() do {if(!ta_track_state)ARR_INVALIDATE();}while(0)
static void arrays_matrix_changed(void) {
    if(!ta_track_state){ARR_INVALIDATE();return;}
    if(arrays_matrix_mode==0x1702)array_texture_invalidate_state();
}
#define ARR_WRAP0(name) static void (*arrays_real_##name)(void); static void arrays_##name(void){ arrays_end_frame();arrays_real_##name();ARR_AFTER_STATE(); }
#define ARR_WRAP1(name,T) static void (*arrays_real_##name)(T); static void arrays_##name(T a){ arrays_end_frame();arrays_real_##name(a);ARR_AFTER_STATE(); }
#define ARR_WRAP2(name,T,U) static void (*arrays_real_##name)(T,U); static void arrays_##name(T a,U b){ arrays_end_frame();arrays_real_##name(a,b);ARR_AFTER_STATE(); }
#define ARR_WRAP3(name,T,U,V) static void (*arrays_real_##name)(T,U,V); static void arrays_##name(T a,U b,V c){ arrays_end_frame();arrays_real_##name(a,b,c);ARR_AFTER_STATE(); }
#define ARR_WRAP4(name,T,U,V,W) static void (*arrays_real_##name)(T,U,V,W); static void arrays_##name(T a,U b,V c,W d){ arrays_end_frame();arrays_real_##name(a,b,c,d);ARR_AFTER_STATE(); }
#define ARR_MATRIX0(name) static void (*arrays_real_##name)(void);static void arrays_##name(void){arrays_end_frame();arrays_real_##name();arrays_matrix_changed();}
#define ARR_MATRIX3(name) static void (*arrays_real_##name)(float,float,float);static void arrays_##name(float a,float b,float c){arrays_end_frame();arrays_real_##name(a,b,c);arrays_matrix_changed();}
ARR_MATRIX0(glLoadIdentity) ARR_MATRIX0(glPushMatrix) ARR_MATRIX0(glPopMatrix)
ARR_WRAP0(glFinish) ARR_WRAP0(glFlush)
static void (*arrays_real_glMatrixMode)(unsigned);
static void arrays_glMatrixMode(unsigned mode){
    arrays_end_frame();arrays_real_glMatrixMode(mode);
    if(mode>=0x1700 && mode<=0x1702)arrays_matrix_mode=(int)mode;
    else {arrays_get_int(0x0ba0,&arrays_matrix_mode);ARR_INVALIDATE();}
    ARR_AFTER_STATE();
}
ARR_WRAP1(glClear,unsigned)
ARR_WRAP1(glEnableClientState,unsigned) ARR_WRAP1(glDisableClientState,unsigned) ARR_WRAP1(glLineWidth,float) ARR_WRAP1(glDepthMask,unsigned char)
ARR_WRAP2(glBlendFunc,unsigned,unsigned) ARR_WRAP2(glAlphaFunc,unsigned,float) ARR_WRAP2(glPixelStorei,unsigned,int)
ARR_WRAP2(glFogfv,unsigned,const float *) ARR_WRAP2(glFogf,unsigned,float) ARR_WRAP2(glFogi,unsigned,int)
ARR_WRAP2(glHint,unsigned,unsigned) ARR_WRAP2(glLightModeli,unsigned,int) ARR_WRAP2(glColorMaterial,unsigned,unsigned)
static void (*arrays_real_glDeleteTextures)(int,const unsigned *);
static void arrays_glDeleteTextures(int n,const unsigned *ids) {
    arrays_end_frame();
    for(int i=0;i<n;i++) {atlas_forget(ids[i]);array_texture_remove(ids[i]);}
    arrays_real_glDeleteTextures(n,ids);
    for(int i=0;i<n;i++)if(ids[i]==arrays_texture){arrays_texture=0;arrays_binding_dirty=0;}
}
ARR_MATRIX3(glScalef) ARR_MATRIX3(glTranslatef)
static void (*arrays_real_glTexEnvi)(unsigned,unsigned,int);
static void arrays_glTexEnvi(unsigned target,unsigned name,int value) {
    arrays_end_frame();arrays_real_glTexEnvi(target,name,value);
    if(target==0x2300 && name==0x2200)ta_modulate=value==0x2100;
    ARR_AFTER_STATE();
}
static void (*arrays_real_glTexParameteri)(unsigned,unsigned,int);
static void arrays_glTexParameteri(unsigned a,unsigned b,int c) { arrays_end_frame();atlas_forget(arrays_texture);array_texture_forget(arrays_texture);arrays_real_glTexParameteri(a,b,c);ARR_AFTER_STATE(); }
ARR_WRAP3(glMaterialfv,unsigned,unsigned,const float *) ARR_WRAP3(glLightfv,unsigned,unsigned,const float *)
static void (*arrays_real_glRotatef)(float,float,float,float);
static void arrays_glRotatef(float a,float b,float c,float d){arrays_end_frame();arrays_real_glRotatef(a,b,c,d);arrays_matrix_changed();}
ARR_WRAP4(glViewport,int,int,int,int) ARR_WRAP4(glScissor,int,int,int,int)
ARR_WRAP4(glClearColor,float,float,float,float)
ARR_WRAP4(glVertexPointer,int,unsigned,int,const void *) ARR_WRAP4(glTexCoordPointer,int,unsigned,int,const void *)
ARR_WRAP1(glShadeModel,unsigned)
static void (*arrays_real_glEnable)(unsigned);
static void arrays_glEnable(unsigned cap) {arrays_end_frame();arrays_real_glEnable(cap);ta_note_cap(cap,1);if(cap==0x0b50 || cap==0x0b57)arrays_invalidate();ARR_AFTER_STATE();}
static void (*arrays_real_glDisable)(unsigned);
static void arrays_glDisable(unsigned cap) {arrays_end_frame();arrays_real_glDisable(cap);ta_note_cap(cap,0);if(cap==0x0b50 || cap==0x0b57)arrays_invalidate();ARR_AFTER_STATE();}
static void (*arrays_real_glDrawArrays)(unsigned,int,int);
static void arrays_glDrawArrays(unsigned mode,int first,int count) {arrays_end_frame();arrays_real_glDrawArrays(mode,first,count);arrays_color_known=0;arrays_uv_known=0;}
static void (*arrays_real_glOrtho)(double,double,double,double,double,double);
static void arrays_glOrtho(double a,double b,double c,double d,double e,double f){arrays_end_frame();arrays_real_glOrtho(a,b,c,d,e,f);arrays_matrix_changed();}
static void (*arrays_real_glTexImage2D)(unsigned,int,int,int,int,int,unsigned,unsigned,const void *);
static void arrays_glTexImage2D(unsigned a,int b,int c,int d,int e,int f,unsigned g,unsigned h,const void *i){arrays_end_frame();atlas_forget(arrays_texture);array_texture_forget(arrays_texture);arrays_real_glTexImage2D(a,b,c,d,e,f,g,h,i);ARR_AFTER_STATE();}
static int (WINAPI *arrays_real_gluBuild2DMipmaps)(unsigned,int,int,int,unsigned,unsigned,const void *);
static int WINAPI arrays_gluBuild2DMipmaps(unsigned target,int components,int width,int height,unsigned format,unsigned type,const void *pixels) {
    arrays_end_frame();
    if(target==0x0de1) {
        int binding=0;arrays_get_int(0x8069,&binding);
        atlas_forget((unsigned)binding);array_texture_forget((unsigned)binding);
    }
    int result=arrays_real_gluBuild2DMipmaps(target,components,width,height,format,type,pixels);
    ARR_AFTER_STATE();return result;
}
static unsigned (*arrays_real_glGetError)(void);
static unsigned arrays_glGetError(void){arrays_flush();return arrays_real_glGetError();}
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

static int install_gl_arrays(void) {
    char env[16];arrays_enabled=GetEnvironmentVariableA("FPS_GL_ARRAYS",env,sizeof(env))>0;
    arrays_lazy_bindings=GetEnvironmentVariableA("FPS_LAZY_BINDS",env,sizeof(env))>0;
    arrays_client_sessions=GetEnvironmentVariableA("FPS_ARRAY_SESSIONS",env,sizeof(env))>0;
#ifdef FPS_SE_MOD
    arrays_enabled=1;arrays_lazy_bindings=1;arrays_client_sessions=1;
#endif
    if(!arrays_enabled) return 1;
    // Do not combine the two submission methods. Oracle toggles arrays_enabled separately.
    batch_enabled=0;color_cache_enabled=0;
    HMODULE gl=GetModuleHandleA("opengl32.dll");if(!gl) return 0;
#define ARR_PROC(var,name) do{ *(void **)&var=(void *)GetProcAddress(gl,name);if(!var)return 0; }while(0)
    ARR_PROC(arrays_get_int,"glGetIntegerv");ARR_PROC(arrays_get_float,"glGetFloatv");ARR_PROC(arrays_get_string,"glGetString");
    ARR_PROC(arrays_is_enabled,"glIsEnabled");ARR_PROC(arrays_push_client,"glPushClientAttrib");ARR_PROC(arrays_pop_client,"glPopClientAttrib");
    ARR_PROC(arrays_enable_client,"glEnableClientState");ARR_PROC(arrays_disable_client,"glDisableClientState");
    ARR_PROC(arrays_vertex_pointer,"glVertexPointer");ARR_PROC(arrays_tex_pointer,"glTexCoordPointer");ARR_PROC(arrays_color_pointer,"glColorPointer");
    ARR_PROC(arrays_draw,"glDrawArrays");ARR_PROC(arrays_tex2,"glTexCoord2f");ARR_PROC(arrays_tex4,"glTexCoord4f");ARR_PROC(arrays_vertex3,"glVertex3f");
    ARR_PROC(arrays_current_context,"wglGetCurrentContext");ARR_PROC(arrays_wgl_proc,"wglGetProcAddress");
#undef ARR_PROC
    // Resolve native addresses before replacing slots, never chain the batching wrappers.
    for(size_t i=0;i<sizeof(arrays_imports)/sizeof(arrays_imports[0]);i++) {
        void *native=(void *)GetProcAddress(gl,arrays_imports[i].name);if(!native)return 0;
        if(arrays_imports[i].save) memcpy(arrays_imports[i].save,&native,sizeof(native));
    }
    // Preserve the existing GLU loader/forwarder chain. System GLU performs
    // uploads internally, so the executable's glTexImage2D hook cannot see it.
    memcpy(&arrays_real_gluBuild2DMipmaps,detail_base+RVA_RDATA_SECTION,sizeof(arrays_real_gluBuild2DMipmaps));
    if(!arrays_real_gluBuild2DMipmaps)return 0;
    for(size_t i=0;i<sizeof(arrays_imports)/sizeof(arrays_imports[0]);i++) {
        void *ignored=NULL;if(!replace_gl_import(arrays_imports[i].rva,arrays_imports[i].hook,&ignored))return 0;
    }
    void *ignored=NULL;
    return replace_gl_import(RVA_RDATA_SECTION,(void *)arrays_gluBuild2DMipmaps,&ignored);
}
