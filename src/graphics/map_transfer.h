// ---- Map texture transfers ----
// @platform-split map-textures: mapped Windows 5.43 world-map passes.
#include <GL/gl.h>
typedef void (WINAPI *MapCopyImage)(unsigned,unsigned,int,int,int,int,unsigned,unsigned,int,int,int,int,int,int,int);
static _Thread_local MapCopyImage map_copy_image;
static _Thread_local void (WINAPI *map_label)(unsigned,unsigned,int,const char *);
static _Thread_local void (WINAPI *map_read_label)(unsigned,unsigned,int,int *,char *);
static _Thread_local int map_imaging, map_partial_imaging;
static int map_textures_enabled;
enum { MAP_TERRAIN = 1, MAP_UNITS = 2 };
static _Thread_local int map_texture_pass;
static _Thread_local int map_bounds_active;
static _Thread_local long long map_copy_ticks, map_tick_frequency;

// Resolve the optional copy function for the single supported rendering context.
static void map_transfer_init(int major,int minor,const char *extensions) {
    if(!map_textures_enabled)return;
    map_imaging=ta_extension(extensions,"GL_ARB_imaging");
    if(!map_imaging){
        const char *optional[]={"GL_EXT_histogram","GL_EXT_convolution","GL_SGI_color_matrix",
                                "GL_SGI_color_table","GL_EXT_color_table","GL_SGI_texture_color_table"};
        for(unsigned i=0;i<sizeof optional/sizeof *optional;i++)
            if(ta_extension(extensions,optional[i]))map_partial_imaging=1;
    }
    if(major>4 || (major==4 && minor>=3) || ta_extension(extensions,"GL_ARB_copy_image"))
        map_copy_image=(MapCopyImage)ta_proc("glCopyImageSubData");
    if(major>4 || (major==4 && minor>=3) || ta_extension(extensions,"GL_KHR_debug")){
        *(void **)&map_label=ta_proc("glObjectLabel");
        *(void **)&map_read_label=ta_proc("glGetObjectLabel");
    }
    LARGE_INTEGER frequency;
    if(QueryPerformanceFrequency(&frequency))map_tick_frequency=frequency.QuadPart;
}

// Only bypass the readback conversion when all active pixel transfers are identity.
static int map_identity_transfer(void) {
    if(map_partial_imaging)return 0;
    int mapped=0;
    glGetIntegerv(GL_MAP_COLOR,&mapped);
    if(mapped)return 0;
    const unsigned scales[]={GL_RED_SCALE,GL_GREEN_SCALE,GL_BLUE_SCALE,GL_ALPHA_SCALE};
    const unsigned biases[]={GL_RED_BIAS,GL_GREEN_BIAS,GL_BLUE_BIAS,GL_ALPHA_BIAS};
    for(unsigned i=0;i<4;i++){
        float scale,bias;
        glGetFloatv(scales[i],&scale); glGetFloatv(biases[i],&bias);
        if(scale!=1.0f || bias!=0.0f)return 0;
    }
    if(!map_imaging)return 1;
    const unsigned caps[]={0x80d0,0x80d1,0x80d2,0x8010,0x8011,0x8012,0x8024,0x802e};
    for(unsigned i=0;i<sizeof caps/sizeof *caps;i++)if(glIsEnabled(caps[i]))return 0;
    float matrix[16]; glGetFloatv(0x80b1,matrix);
    for(int i=0;i<16;i++)if(matrix[i]!=(i%5==0?1.f:0.f))return 0;
    for(unsigned i=0;i<4;i++){
        const unsigned params[]={0x801c+i,0x8020+i,0x80b4+i,0x80b8+i};
        for(unsigned j=0;j<4;j++){
            float value; glGetFloatv(params[j],&value);
            if(value!=(j%2==0?1.f:0.f))return 0;
        }
    }
    return 1;
}

// Submit compatible copies. The caller verifies storage before publishing the layer.
static int map_copy_layer(unsigned source,unsigned destination,int layer,int width,int height,int last) {
    if(!map_texture_pass || !map_copy_image || !map_identity_transfer())return 0;
    for(int i=0;i<=last;i++){
        map_copy_image(source,GL_TEXTURE_2D,i,0,0,0,destination,0x8c1a,i,0,0,layer,width,height,1);
        if(width>1)width/=2;
        if(height>1)height/=2;
    }
    return 1;
}

// Measure only new layer preparation in a map terrain pass.
static long long map_ticks(void) {
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    return ticks.QuadPart;
}
