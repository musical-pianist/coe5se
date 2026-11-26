// ---- World map texture sources ----
#include "map_sprite_bounds.h"
#include "map_crop.h"
typedef int (*MapSemaphoreFn)(void *);
static MapSemaphoreFn map_try_lock, map_unlock;
#include "map_tile_reserve.h"

// Check the game's external-image cache while its insertion/eviction semaphore is held.
static int map_cached_bounds(const char *path,unsigned w,unsigned h,int *width,int *height) {
    void *lock=*(void **)(detail_base+RVA_IMAGE_CACHE_SEMAPHORE);
    if(!lock || !map_try_lock || map_try_lock(lock)!=0)return 0;
    int last=*(const int *)(detail_base+RVA_IMAGE_CACHE_LAST);
    int measured=0, dx=0, dy=0;
    if(last>=-1 && last<512){
        int (*find)(const char *)=(void *)(detail_base+RVA_IMAGE_CACHE_FIND);
        int i=find(path);
        if(i==-1)measured=-1;
        if(i>=0 && i<=last && ((const unsigned *)(detail_base+RVA_IMAGE_CACHE_WIDTHS))[i]==w &&
           ((const unsigned *)(detail_base+RVA_IMAGE_CACHE_HEIGHTS))[i]==h){
            const uint8_t *pixels=((const uint8_t *const *)(detail_base+RVA_IMAGE_CACHE_PIXELS))[i];
            measured=sbMeasure(pixels,(int)w,(int)h,&dx,&dy);
        }
    }
    if(map_unlock(lock)!=0){map_try_lock=NULL;return 0;}
    if(measured>0){*width=dx; *height=dy;}
    return measured;
}

// Let the game populate cold slots using its own allocation and eviction policy.
static int map_external_bounds(const char *path,unsigned w,unsigned h,int *width,int *height) {
    int result=map_cached_bounds(path,w,h,width,height);
    if(result>=0)return result;
    int (*sample)(const char *,int,int,unsigned *,unsigned *,unsigned *,unsigned *,int)=(void *)(detail_base+RVA_IMAGE_CACHE_SAMPLE);
    unsigned r,g,b,a;
    return sample(path,0,0,&r,&g,&b,&a,0)==1 && map_cached_bounds(path,w,h,width,height)>0;
}

// Use the native path for internal sprites, cache contention and unsupported dimensions.
static void map_army_bounds(unsigned file,unsigned sprite,int *width,int *height) {
    if(map_bounds_active && map_try_lock && file==0x21 && sprite<0x7fbc && width && height && width!=height){
        int (*replacement)(unsigned,unsigned)=(void *)(detail_base+RVA_REPLACED_SPRITE);
        int index=replacement(file,sprite);
        if((unsigned)index<4096){
            const char *path=((const char *const *)(detail_base+RVA_EXTERNAL_IMAGE_PATHS))[index];
            int (*header)(const char *,unsigned *,unsigned *)=(void *)(detail_base+RVA_IMAGE_SIZE_READ);
            unsigned w=0,h=0;
            if(path && *path && header(path,&w,&h)>0 && w<=1024 && h<=1024 && w && h &&
               map_external_bounds(path,w,h,width,height))return;
        }
    }
    ((void (*)(unsigned,unsigned,int *,int *))(detail_base+RVA_SPRITE_FIGURE_SIZE))(file,sprite,width,height);
}

// Resolve the game's nonblocking SDL cache lock once.
static void map_bounds_init(void) {
    HMODULE sdl=GetModuleHandleW(L"SDL2.dll");
    if(!sdl)return;
    map_unlock=(MapSemaphoreFn)(void *)GetProcAddress(sdl,"SDL_SemPost");
    if(map_unlock)map_try_lock=(MapSemaphoreFn)(void *)GetProcAddress(sdl,"SDL_SemTryWait");
}

// Crop complete, disjoint RGBA tiles using the game's original row order.
static void map_tile_crop(const unsigned char *src,int w,int h,int x,int y,uintptr_t a,uintptr_t b,unsigned char *dest) {
    if(map_texture_pass==MAP_TERRAIN){map_crop_x=map_crop_y=-1;}
    if(map_texture_pass==MAP_TERRAIN && tcCanCopy(src,w,h,x,y,dest)){
        tcCopyRows(src,w,x,y,dest);
        map_crop_x=x; map_crop_y=y;
        if(map_reserve_main && h<=160*64)map_atlas_height=h;
        map_capture_neighbors(src,w,h,x,y);
        return;
    }
    ((void (*)(const unsigned char *,int,int,int,int,uintptr_t,uintptr_t,unsigned char *))
        (detail_base+RVA_MAP_TILE_CROP))(src,w,h,x,y,a,b,dest);
}
