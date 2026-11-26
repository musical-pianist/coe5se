#pragma once
#include <GL/gl.h>

// ---- Textured interface frames ----
// @platform-split gui-frame-batching: only Windows has verified uninterrupted rectangle sequences.
enum { GUI_PANEL_RECTS=9, GUI_BORDER_RECTS=8 };

// The installed calls of one straight-line group, in drawing order.
typedef struct {
    const unsigned char *site[GUI_PANEL_RECTS];
    unsigned char bytes[GUI_PANEL_RECTS][5];
    unsigned installed;
} GuiFrameSites;
static GuiFrameSites gui_panel,gui_border;

// The five imports read by the native rectangle helper.
static const uintptr_t gui_imports[]={RVA_GL_ENABLE_CLIENT_STATE,RVA_GL_DISABLE_CLIENT_STATE,
    RVA_GL_VERTEX_POINTER,RVA_FPS_GL_TEX_COORD_POINTER_IAT,RVA_GL_DRAW_ARRAYS};
static void *gui_expected_imports[sizeof(gui_imports)/sizeof(gui_imports[0])];

// Expect our verified renderer hooks, or the five original exports for standalone GUI drawing.
static int gui_prepare_imports(int shared_renderer) {
    HMODULE gl=GetModuleHandleA(FPS_GL_LIBRARY);
    for(size_t i=0;i<sizeof(gui_imports)/sizeof(gui_imports[0]);i++){
        size_t j=0;
        while(j<ARRAY_IMPORT_COUNT && arrays_imports[j].rva!=gui_imports[i])j++;
        if(j==ARRAY_IMPORT_COUNT)return 0;
        const FpsGlImport *entry=&arrays_imports[j];
        if(shared_renderer){
            gui_expected_imports[i]=entry->hook;
        }else{
            void *native=(void *)GetProcAddress(gl,entry->name),*actual=*gl_import_slot(entry->rva);
            if(!native || actual!=native){
                mod_import_problem(entry->name,entry->rva,actual,native);
                return 0;
            }
            gui_expected_imports[i]=native;
        }
    }
    return 1;
}

// Optional compatibility state and indexed queries belong to the current GL context.
static _Thread_local struct {
    HGLRC context;
    int compatible,assembly,fragment_program,pipeline;
    GLboolean (APIENTRY *client_enabled)(GLenum,GLuint);
} gui_gl;

// Read the capabilities once per current context.
static int gui_context_ready(void) {
    if(!arrays_check_context())return 0;
    if(gui_gl.context!=arrays_context){
        gui_gl.context=arrays_context;
        int major=0,minor=0;
        sscanf((const char *)arrays_get_string(GL_VERSION),"%d.%d",&major,&minor);
        gui_gl.compatible=major>=2;
        const char *ext=(const char *)arrays_get_string(GL_EXTENSIONS);
        gui_gl.assembly=arrays_extension(ext,"GL_ARB_vertex_program");
        gui_gl.fragment_program=arrays_extension(ext,"GL_ARB_fragment_program");
        gui_gl.pipeline=major>4 || (major==4 && minor>=1) || arrays_extension(ext,"GL_ARB_separate_shader_objects");
        PROC indexed=arrays_extension(ext,"GL_EXT_direct_state_access")?arrays_wgl_proc("glIsEnabledIndexedEXT"):NULL;
        gui_gl.client_enabled=((uintptr_t)indexed>3 && (uintptr_t)indexed!=UINTPTR_MAX)?
            (GLboolean (APIENTRY *)(GLenum,GLuint))(void (*)(void))indexed:NULL;
    }
    return gui_gl.compatible;
}

// Remember the installed instruction so a later mod can take over any piece.
static void gui_record_site(const unsigned char *site) {
    GuiFrameSites *sites=site<detail_base+RVA_GUI_BORDER_RECT_1?&gui_panel:&gui_border;
    if(sites->installed==GUI_PANEL_RECTS)return;
    unsigned i=sites->installed++;
    sites->site[i]=site;
    memcpy(sites->bytes[i],site,5);
}

// A complete group must still belong to us before deferring its first rectangle.
static int gui_owns_group(const GuiFrameSites *sites,unsigned count) {
    static const unsigned char native_rect_entry[]={0x48,0x81,0xec,0x98,0,0,0};
    if(sites->installed!=count || memcmp(detail_base+RVA_FPS_TERRAIN_RECT_DRAW,
        native_rect_entry,sizeof(native_rect_entry)))return 0;
    for(size_t i=0;i<sizeof(gui_imports)/sizeof(gui_imports[0]);i++)
        if(*gl_import_slot(gui_imports[i])!=gui_expected_imports[i])return 0;
    for(unsigned i=0;i<count;i++)
        if(memcmp(sites->site[i],sites->bytes[i],5))return 0;
    return 1;
}

// One fixed-function vertex with the source texture's original UV.
typedef struct { float xy[2],uv[2]; } GuiVertex;

// Each four-vertex fan becomes the same two ordered triangles.
static void gui_vertices(GuiVertex *out,TerrainQuad q,float sx,float sy) {
#ifdef __clang__
#pragma clang fp contract(off)
#endif
    float l=q.x*sx,r=(q.x+q.w)*sx,t=q.y*sy,b=(q.y+q.h)*sy;
    float u=q.u+q.du,v=q.v+q.dv;
    out[0]=(GuiVertex){{l,t},{q.u,q.v}};
    out[1]=(GuiVertex){{r,t},{u,q.v}};
    out[2]=(GuiVertex){{r,b},{u,v}};
    out[3]=out[0];out[4]=out[2];
    out[5]=(GuiVertex){{l,b},{q.u,v}};
}

// Only the native fixed-function position and texture inputs can be replaced.
static int gui_can_draw(void) {
    if(!gui_context_ready())return 0;
    int program=0,list=0,mode=0,binding=0,vao=0,unit=0x84c0;
    arrays_get_int(0x8b8d,&program);
    arrays_get_int(GL_RENDER_MODE,&mode);
    arrays_get_int(GL_LIST_INDEX,&list);
    if(arrays_has_vbo)arrays_get_int(0x8894,&binding);
    if(arrays_has_vao)arrays_get_int(0x85b5,&vao);
    int pipeline=0,polygon[2];
    if(gui_gl.pipeline)arrays_get_int(0x825a,&pipeline);
    if(program || binding || vao || list || pipeline || mode!=GL_RENDER)return 0;
    if((gui_gl.assembly && arrays_is_enabled(0x8620)) ||
       (gui_gl.fragment_program && arrays_is_enabled(0x8804)) || arrays_is_enabled(GL_LIGHTING))return 0;
    arrays_get_int(GL_POLYGON_MODE,polygon);
    if(polygon[0]!=GL_FILL || polygon[1]!=GL_FILL || arrays_is_enabled(GL_POLYGON_SMOOTH))return 0;
    const GLenum inputs[]={GL_VERTEX_ARRAY,GL_NORMAL_ARRAY,GL_COLOR_ARRAY,GL_INDEX_ARRAY,
                          GL_EDGE_FLAG_ARRAY,0x8457,0x845e};
    for(size_t i=0;i<sizeof(inputs)/sizeof(inputs[0]);i++)
        if(arrays_is_enabled(inputs[i]))return 0;
    arrays_get_int(0x84e0,&unit);
    if(unit!=0x84c0)return 0;
    if(arrays_client_active)arrays_get_int(0x84e1,&unit);
    if(unit!=0x84c0 || arrays_is_enabled(GL_TEXTURE_COORD_ARRAY))return 0;
    if(gui_gl.client_enabled){
        for(int i=1;i<arrays_tex_units;i++)
            if(gui_gl.client_enabled(GL_TEXTURE_COORD_ARRAY,i))return 0;
    }else{
        for(int i=1;i<arrays_tex_units;i++){
            arrays_client_active(0x84c0+i);
            if(arrays_is_enabled(GL_TEXTURE_COORD_ARRAY)){
                arrays_client_active(0x84c0);return 0;
            }
        }
        if(arrays_tex_units>1)arrays_client_active(0x84c0);
    }
    return 1;
}

// Submit one frame against the already-bound native texture.
static void gui_draw_group(const TerrainQuad *rects,unsigned count) {
    GuiVertex vertices[GUI_PANEL_RECTS*6];
    float sx=(float)*(int *)(detail_base+RVA_WINDOW_W)/(float)*(int *)(detail_base+RVA_SCREEN_W);
    float sy=(float)*(int *)(detail_base+RVA_WINDOW_H)/(float)*(int *)(detail_base+RVA_SCREEN_H);
    for(unsigned i=0;i<count;i++)gui_vertices(vertices+6*i,rects[i],sx,sy);
    float uv[4];
    arrays_get_float(GL_CURRENT_TEXTURE_COORDS,uv);
    real_gl_begin(GL_TRIANGLES);
    for(unsigned i=0;i<count*6;i++){
        arrays_tex2(vertices[i].uv[0],vertices[i].uv[1]);
        arrays_vertex3(vertices[i].xy[0],vertices[i].xy[1],0);
    }
    real_gl_end();
    arrays_tex4(uv[0],uv[1],uv[2],uv[3]);
    arrays_color_known=arrays_uv_known=0;
}

// The pinned code between these calls contains only coordinate arithmetic.
static _Thread_local TerrainQuad gui_rects[GUI_PANEL_RECTS];
static _Thread_local unsigned gui_count,gui_expected;

// Start only outside the map, scene and terrain renderer's deferred passes.
static void gui_start(const GuiFrameSites *sites,unsigned count,TerrainQuad q) {
    if(in_highest || terrain_scope || arrays_native_open || arrays_open || arrays_count ||
       arrays_client_session || ta_drawing || arrays_binding_dirty || arrays_uv_dirty || arrays_color_dirty ||
       !gui_owns_group(sites,count)){
        terrain_native_rect(q);return;
    }
    gui_expected=count;gui_count=1;gui_rects[0]=q;
}

// Both frame routines pass the same eight rectangle arguments.
#define GUI_RECT_ARGS float x,float y,float w,float h,float u,float v,float du,float dv
#define GUI_RECT_VALUE (TerrainQuad){x,y,w,h,u,v,du,dv}

// Start the nine-piece bar group.
static void gui_first_panel(GUI_RECT_ARGS){gui_start(&gui_panel,GUI_PANEL_RECTS,GUI_RECT_VALUE);}
// Start the border group; its final edge follows in the native tail call.
static void gui_first_border(GUI_RECT_ARGS){gui_start(&gui_border,GUI_BORDER_RECTS,GUI_RECT_VALUE);}
// Collect an interior piece, or draw it immediately when the group is inactive.
static void gui_next_rect(GUI_RECT_ARGS){
    if(gui_expected && gui_count<gui_expected)gui_rects[gui_count++]=GUI_RECT_VALUE;
    else terrain_native_rect(GUI_RECT_VALUE);
}
// Finish before the caller can draw anything else or change GL state.
static void gui_last_rect(GUI_RECT_ARGS){
    if(!gui_expected){terrain_native_rect(GUI_RECT_VALUE);return;}
    unsigned count=gui_count,expected=gui_expected;
    gui_count=gui_expected=0;
    TerrainQuad rects[GUI_PANEL_RECTS];
    memcpy(rects,gui_rects,count*sizeof(*rects));
    if(count+1!=expected){
        for(unsigned i=0;i<count;i++)terrain_native_rect(rects[i]);
        terrain_native_rect(GUI_RECT_VALUE);return;
    }
    rects[count++]=GUI_RECT_VALUE;
    if(gui_can_draw()){
        gui_draw_group(rects,count);
        static unsigned reported;
        unsigned bit=1u<<count;
        if(!(reported&bit)){
            reported|=bit;
            mod_log(count==GUI_PANEL_RECTS?"fps_mod gui_frame_ready pieces=9 draws=1":"fps_mod gui_frame_ready pieces=8 draws=1");
        }
    }else{
        for(unsigned i=0;i<count;i++)terrain_native_rect(rects[i]);
    }
}
#undef GUI_RECT_ARGS
#undef GUI_RECT_VALUE
