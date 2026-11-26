// ---- Windows map mipmap preparation ----
#include "map_base_filter.h"
enum { MAP_TILE_BYTES=64*64*4, MAP_LOWER_BYTES=(32*32+16*16+8*8+4*4+2*2+1)*4, MAP_MIP_SLOTS=512 };

// CPU pixels only. Entries remain valid across save, context and plane changes.
typedef struct {
    int valid;
    unsigned char source[MAP_TILE_BYTES], levels[MAP_LOWER_BYTES];
} MapMipEntry;
static MapMipEntry *map_mips;
static SRWLOCK map_work_lock=SRWLOCK_INIT;
static unsigned short map_base_pixels[512*64*4];
static _Thread_local int map_filters_disabled;

// Match the Windows GLU intermediate precision at every level.
static void map_filter_tile(const unsigned char *source,unsigned char *levels) {
    unsigned short work[32*32*4];
    for(int side=64;side>1;side/=2){
        int next=side/2;
        for(int y=0;y<next;y++)for(int x=0;x<next;x++){
            int a=(2*y*side+2*x)*4,b=a+side*4,out=(y*next+x)*4;
            for(int c=0;c<4;c++){
                unsigned sum=side==64 ? 257u*(source[a+c]+source[a+4+c]+source[b+c]+source[b+4+c])
                                      : (unsigned)(work[a+c]+work[a+4+c]+work[b+c]+work[b+4+c]);
                unsigned value=(sum+2u)/4u;
                work[out+c]=(unsigned short)value;
                levels[out+c]=(unsigned char)((value+128u)/257u);
            }
        }
        levels+=next*next*4;
    }
}

// A hash selects a bucket; full equality decides reuse even for custom textures.
static MapMipEntry *map_tile_levels(const unsigned char *source) {
    if(!map_mips){
        map_mips=calloc(MAP_MIP_SLOTS,sizeof *map_mips);
        if(!map_mips)return NULL;
    }
    unsigned hash=2166136261u;
    for(unsigned i=0;i<16;i++){
        uint32_t word;
        memcpy(&word,source+i*(MAP_TILE_BYTES-4)/15,4);
        hash=(hash^word)*16777619u;
        hash^=hash>>13;
    }
    MapMipEntry *entry=&map_mips[hash%MAP_MIP_SLOTS];
    if(!entry->valid || memcmp(entry->source,source,MAP_TILE_BYTES)!=0){
        memcpy(entry->source,source,MAP_TILE_BYTES);
        map_filter_tile(source,entry->levels);
        entry->valid=1;
    }
    return entry;
}

// Require a tightly packed client image and fixed-function pixel transfer.
static int map_simple_upload(int row,int needed) {
    const unsigned zero[]={GL_LIST_INDEX,0x88ef,0x88ed,GL_UNPACK_ROW_LENGTH,GL_UNPACK_SKIP_ROWS,
                           GL_UNPACK_SKIP_PIXELS,GL_UNPACK_SWAP_BYTES};
    for(unsigned i=0;i<sizeof zero/sizeof *zero;i++){
        int value; glGetIntegerv(zero[i],&value); if(value)return 0;
    }
    int alignment,bound,max,automatic;
    glGetIntegerv(GL_UNPACK_ALIGNMENT,&alignment);
    if((alignment!=1 && alignment!=2 && alignment!=4 && alignment!=8) || row%alignment)return 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D,&bound);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
    if(!bound || max<needed)return 0;
    glGetTexParameteriv(GL_TEXTURE_2D,0x8191,&automatic);
    return !automatic && map_identity_transfer();
}

// A level known to be empty must acquire the width of the requested two-dimensional allocation.
static int map_level_widths(int width,int height,int allocated) {
    for(int level=0;;level++){
        int w=-1;
        glGetTexLevelParameteriv(GL_TEXTURE_2D,level,GL_TEXTURE_WIDTH,&w);
        if(w!=(allocated?width:0))return 0;
        if(width==1 && height==1)return 1;
        if(width>1)width/=2;
        if(height>1)height/=2;
    }
}

// Accelerate only the two input layouts validated against Windows GLU.
static int map_build_mips(unsigned target,int internal,int w,int h,unsigned format,unsigned type,const void *source) {
    if(!map_texture_pass || map_filters_disabled || !source || target!=GL_TEXTURE_2D || format!=GL_RGBA ||
       type!=GL_UNSIGNED_BYTE || !ta_current_context || ta_current_context()!=ta_context)return 0;
    int square=internal==GL_RGBA && w==64 && h==64;
    int dw=internal==GL_RGBA4 && (h==71 || h==72)?bfTargetWidth(w):0;
    if((!square && !dw) || !map_simple_upload(w*4,square?64:dw))return 0;
    if(!map_level_widths(square?64:dw,64,0))return 0;
    int result=0;
    if(square){
        MapMipEntry *entry=map_tile_levels(source);
        if(!entry){map_filters_disabled=1;return 0;}
        glTexImage2D(target,0,internal,64,64,0,format,type,source);
        const unsigned char *pixels=entry->levels;
        for(int side=32,level=1;side;side/=2,level++){
            glTexImage2D(target,level,internal,side,side,0,format,type,pixels);
            pixels+=side*side*4;
        }
    }else{
        if(!bfScale(source,w,h,map_base_pixels,dw,64))return 0;
        result=arrays_real_gluBuild2DMipmaps(target,internal,dw,64,format,GL_UNSIGNED_SHORT,map_base_pixels);
    }
    return !result && map_level_widths(square?64:dw,64,1);
}
