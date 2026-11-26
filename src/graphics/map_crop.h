// ---- Complete, disjoint RGBA tile crops ----
#include <stdint.h>
#include <limits.h>
#include <string.h>

static int tcCanCopy(const void *src,int w,int h,int x,int y,void *dst) {
    if(!src || !dst || w<64 || h<64 || x<0 || y<0 || x>w-64 || y>h-64)return 0;
    uint64_t bytes=(uint64_t)(unsigned)w*(unsigned)h*4;
    uintptr_t a=(uintptr_t)src,b=(uintptr_t)dst;
    if(bytes>INT_MAX || a>UINTPTR_MAX-bytes || b>UINTPTR_MAX-16384)return 0;
    return b+16384<=a || a+bytes<=b;
}

static void tcCopyRows(const unsigned char *src,int stride,int x,int y,unsigned char *dst) {
    const size_t pitch=(size_t)stride*4;
    src+=(size_t)y*pitch+(size_t)x*4;
    for(int i=0;i<64;++i)
        memcpy(dst+(size_t)i*256,src+(size_t)i*pitch,256);
}
