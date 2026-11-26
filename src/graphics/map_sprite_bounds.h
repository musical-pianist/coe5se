// ---- External map army sprite bounds ----
#include <stdint.h>
#include <stddef.h>

static int sbDimensionsOK(int width,int height) {
    return width>0 && width<=1024 && height>0 && height<=1024;
}
static int sbVisible(const uint8_t *rgba) {
    return rgba[3] && (rgba[0] || rgba[1] || rgba[2]);
}
static int sbMeasure(const uint8_t *rgba,int w,int h,int *width,int *height) {
    if(!rgba || !width || !height || !sbDimensionsOK(w,h))return 0;
    *height=0;
    *width=0;
    for(int y=0;y<h && !*height;++y){
        const uint8_t *row=rgba+(size_t)y*w*4;
        for(int x=0;x<w;++x){
            if(sbVisible(row+x*4)){
                *height=h-y;
                break;
            }
        }
    }
    if(!*height)return 1;
    /* The external-image path in 5.43 tests the same left-side column twice.
       Right-only pixels, and the middle column of an odd width, affect height only. */
    for(int edge=0;edge<w/2;++edge){
        for(int y=0;y<h;++y){
            const uint8_t *row=rgba+(size_t)y*w*4;
            if(sbVisible(row+edge*4)){
                *width=w-2*edge;
                return 1;
            }
        }
    }
    return 1;
}
