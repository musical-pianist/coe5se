typedef struct {
    int source_texture,array_texture,program,client_unit;
    int vertex_format[3],color_format[3],enabled[5],tex_enabled[32],tex_format[32][3];
    void *pointers[5],*tex_pointers[32];
    float color[4],uv[4];
    int server_unit,array_bindings[9],sampler_bindings[9];
} OracleDrawState;
static int oracle_draw_state(OracleDrawState *out) {
    typedef void (*GetPointer)(unsigned,void **);
    GetPointer get_pointer=(GetPointer)GetProcAddress(GetModuleHandleA("opengl32.dll"),"glGetPointerv");
    if(!get_pointer || !arrays_context_ok)return 0;
    memset(out,0,sizeof(*out));
    arrays_get_int(0x8069,&out->source_texture);arrays_get_int(0x8c1d,&out->array_texture);arrays_get_int(0x8b8d,&out->program);
    out->client_unit=0x84c0;if(arrays_client_active)arrays_get_int(0x84e1,&out->client_unit);
    const unsigned caps[]={0x8074,0x8075,0x8076,0x8077,0x8079};
    const unsigned pointers[]={0x808e,0x808f,0x8090,0x8091,0x8093};
    for(int i=0;i<5;i++){out->enabled[i]=arrays_is_enabled(caps[i]);get_pointer(pointers[i],&out->pointers[i]);}
    for(int i=0;i<3;i++){arrays_get_int(0x807au+(unsigned)i,&out->vertex_format[i]);arrays_get_int(0x8081u+(unsigned)i,&out->color_format[i]);}
    for(int unit=0;unit<arrays_tex_units;unit++) {
        if(arrays_client_active)arrays_client_active(0x84c0u+(unsigned)unit);
        out->tex_enabled[unit]=arrays_is_enabled(0x8078);get_pointer(0x8092,&out->tex_pointers[unit]);
        for(int i=0;i<3;i++)arrays_get_int(0x8088u+(unsigned)i,&out->tex_format[unit][i]);
    }
    if(arrays_client_active)arrays_client_active((unsigned)out->client_unit);
    arrays_get_float(0x0b00,out->color);arrays_get_float(0x0b03,out->uv);
    ta_get_int(0x84e0,&out->server_unit);
    for(int i=0;i<9;i++) {
        ta_active_texture(0x84c0u+(unsigned)i);ta_get_int(0x8c1d,&out->array_bindings[i]);
        if(ta_has_sampler)ta_get_int(0x8919,&out->sampler_bindings[i]);
    }
    ta_active_texture((unsigned)out->server_unit);return 1;
}
// One-frame lab oracle: restore the exact background, render the same prepared
// lists twice, compare RGBA bytes. Refuse depth-tested or unusual viewports.
static void graphics_test_render_pass(void (*original)(void)) {
    static unsigned rendered;
    rendered++;
    if(rendered!=180 && rendered!=480 && rendered!=780) { original();return; }
    HMODULE lib=GetModuleHandleA("opengl32.dll");
#define GL_FN(name,ret,args) typedef ret (*Fn_##name) args; Fn_##name name=(Fn_##name)GetProcAddress(lib,#name); if(!name) goto unavailable
    GL_FN(glGetIntegerv,void,(unsigned,int *));
    GL_FN(glIsEnabled,unsigned char,(unsigned));
    GL_FN(glReadPixels,void,(int,int,int,int,unsigned,unsigned,void *));
    GL_FN(glDrawPixels,void,(int,int,unsigned,unsigned,const void *));
    GL_FN(glPushAttrib,void,(unsigned));GL_FN(glPopAttrib,void,(void));
    GL_FN(glPushClientAttrib,void,(unsigned));GL_FN(glPopClientAttrib,void,(void));
    GL_FN(glMatrixMode,void,(unsigned));GL_FN(glPushMatrix,void,(void));GL_FN(glPopMatrix,void,(void));
    GL_FN(glLoadIdentity,void,(void));GL_FN(glOrtho,void,(double,double,double,double,double,double));
    GL_FN(glRasterPos2i,void,(int,int));GL_FN(glPixelZoom,void,(float,float));
    GL_FN(glPixelStorei,void,(unsigned,int));GL_FN(glGetError,unsigned,(void));
#undef GL_FN
    int viewport[4],matrix,read_buffer,draw_buffer;
    glGetIntegerv(0x0ba2,viewport);glGetIntegerv(0x0ba0,&matrix);
    glGetIntegerv(0x0c02,&read_buffer);glGetIntegerv(0x0c01,&draw_buffer);
    if(viewport[0] || viewport[1] || viewport[2]<=0 || viewport[3]<=0 || viewport[2]>4096 || viewport[3]>4096 || glIsEnabled(0x0b71) || read_buffer!=draw_buffer) goto unavailable;
    size_t bytes=(size_t)viewport[2]*viewport[3]*4;
    unsigned char *mem=HeapAlloc(GetProcessHeap(),0,bytes*3);
    if(!mem) goto unavailable;
    unsigned char *background=mem,*baseline=mem+bytes,*candidate=mem+2*bytes;
    int selected_arrays=arrays_enabled,selected_layered=texture_array_enabled;
    arrays_end_frame();arrays_enabled=0;texture_array_enabled=0;
    while(glGetError()) { }
    glPushClientAttrib(0xffffffffu);glPixelStorei(0x0d05,1);glPixelStorei(0x0cf5,1);
    glReadPixels(0,0,viewport[2],viewport[3],0x1908,0x1401,background);
    original();arrays_end_frame();
    OracleDrawState baseline_state,candidate_state;
    int have_baseline_state=oracle_draw_state(&baseline_state);
    glReadPixels(0,0,viewport[2],viewport[3],0x1908,0x1401,baseline);
    unsigned baseline_error=glGetError();
    glPushAttrib(0x000fffff);
    real_gl_disable(0x0be2);real_gl_disable(0x0bc0);real_gl_disable(0x0b71);
    real_gl_disable(0x0b90);real_gl_disable(0x0c11);real_gl_disable(0x0b50);real_gl_disable(0x0de1);
    glPixelZoom(1,1);glMatrixMode(0x1701);glPushMatrix();glLoadIdentity();
    glOrtho(0,viewport[2],0,viewport[3],-1,1);
    glMatrixMode(0x1700);glPushMatrix();glLoadIdentity();glRasterPos2i(0,0);
    glDrawPixels(viewport[2],viewport[3],0x1908,0x1401,background);
    glPopMatrix();glMatrixMode(0x1701);glPopMatrix();glMatrixMode((unsigned)matrix);glPopAttrib();
    glReadPixels(0,0,viewport[2],viewport[3],0x1908,0x1401,candidate);
    int background_equal=memcmp(background,candidate,bytes)==0;
    arrays_enabled=selected_arrays;texture_array_enabled=selected_layered;
    arrays_begin_frame();array_texture_begin_frame();
    original();arrays_end_frame();
    int have_candidate_state=oracle_draw_state(&candidate_state);
    int state_equal=have_baseline_state && have_candidate_state && !memcmp(&baseline_state,&candidate_state,sizeof(baseline_state));
    glReadPixels(0,0,viewport[2],viewport[3],0x1908,0x1401,candidate);
    unsigned candidate_error=glGetError();glPopClientAttrib();
    arrays_enabled=selected_arrays;texture_array_enabled=selected_layered;
    size_t diff=0;unsigned max_delta=0;
    for(size_t i=0;i<bytes;i++) { unsigned a=baseline[i],b=candidate[i],d=a>b?a-b:b-a;if(d) diff++;if(d>max_delta) max_delta=d; }
    char line[512];snprintf(line,sizeof(line),"fps_pixel_oracle width=%d height=%d background_exact=%d different_channels=%llu max_delta=%u baseline_gl_error=%u candidate_gl_error=%u arrays=%d state_equal=%d client_session_closed=%d quality=%d",
       viewport[2],viewport[3],background_equal,(unsigned long long)diff,max_delta,baseline_error,candidate_error,selected_arrays,state_equal,!arrays_client_session,*(int *)(detail_base+0x1a50e2c));
    plugin_api->logLine((const void *)graphics_test_render_pass,line);HeapFree(GetProcessHeap(),0,mem);return;
unavailable:
    plugin_api->logLine((const void *)graphics_test_render_pass,"fps_pixel_oracle unavailable=context_or_memory");
    original();
}
