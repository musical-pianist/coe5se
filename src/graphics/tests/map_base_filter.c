// Compare every supported irregular terrain size against the Windows GLU filter.
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "../map_base_filter.h"

int main(void) {
    HWND win=CreateWindowExW(0,L"STATIC",L"",WS_POPUP,0,0,8,8,NULL,NULL,NULL,NULL);
    HDC dc=GetDC(win);
    PIXELFORMATDESCRIPTOR pfd={0};
    pfd.nSize=sizeof pfd;pfd.nVersion=1;pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL;
    pfd.iPixelType=PFD_TYPE_RGBA;pfd.cColorBits=32;
    int fmt=ChoosePixelFormat(dc,&pfd);
    if(!win || !dc || !fmt || !SetPixelFormat(dc,fmt,&pfd))return 2;
    HGLRC context=wglCreateContext(dc);
    if(!context || !wglMakeCurrent(dc,context))return 2;
    glPixelStorei(GL_PACK_ALIGNMENT,1);glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    static uint8_t src[529*72*4];
    static uint16_t ref[512*64*4],got[512*64*4];
    uint32_t rng=0x71309a12;
    GLuint scratch;
    glGenTextures(1,&scratch);glBindTexture(GL_TEXTURE_2D,scratch);
    if(bfScale(src,129,71,got,128,64) || bfScale(src,530,71,got,512,64) ||
       bfScale(src,130,70,got,128,64) || bfScale(src,130,71,got,256,64) ||
       bfScale(src,130,71,got,128,32))return 4;
    long long channels=0,bad=0;int cases=0;
    for(int h=71;h<=72;h++)for(int w=130;w<=529;w++) {
        int dw=bfTargetWidth(w);
        for(int p=0;p<w*h;p++) {
            rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;
            src[p*4]=(uint8_t)rng;src[p*4+1]=((p/w+p%w)&1)?255:0;
            src[p*4+2]=255;src[p*4+3]=(uint8_t)(p%w);
        }
        if(gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA4,w,h,GL_RGBA,GL_UNSIGNED_BYTE,src))return 3;
        int gw=0,gh=0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&gw);
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&gh);
        if(gw!=dw || gh!=64){printf("size mismatch input=%dx%d native=%dx%d expected=%dx64\n",w,h,gw,gh,dw);return 3;}
        if(gluScaleImage(GL_RGBA,w,h,GL_UNSIGNED_BYTE,src,dw,64,GL_UNSIGNED_SHORT,ref) ||
           !bfScale(src,w,h,got,dw,64) || glGetError()!=GL_NO_ERROR)return 3;
        for(int k=0;k<dw*64*4;k++){
            if(ref[k]!=got[k]){
                if(bad<12)printf("mismatch %dx%d -> %d x=%d y=%d channel=%d reference=%u filter=%u\n",
                                 w,h,dw,(k/4)%dw,k/(4*dw),k%4,ref[k],got[k]);
                ++bad;
            }
        }
        ++cases;channels+=dw*64*4;
    }
    printf("shapes=%d channels=%lld differing=%lld\n",cases,channels,bad);
    glDeleteTextures(1,&scratch);wglMakeCurrent(NULL,NULL);wglDeleteContext(context);ReleaseDC(win,dc);DestroyWindow(win);
    return bad!=0;
}
