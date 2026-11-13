#pragma once

#ifdef _WIN32
#define FPS_GL_LIBRARY "opengl32.dll"
#define FPS_GL_CONTEXT_FN "wglGetCurrentContext"
#define FPS_ALLOC(size) HeapAlloc(GetProcessHeap(),0,(size))
#define FPS_FREE(memory) HeapFree(GetProcessHeap(),0,(memory))
#define FPS_GLU_SLOT RVA_RDATA_SECTION
#else
#include <dlfcn.h>
#define FPS_GL_LIBRARY "OpenGL"
#define FPS_GL_CONTEXT_FN "CGLGetCurrentContext"
#define FPS_ALLOC(size) malloc(size)
#define FPS_FREE(memory) free(memory)
#define FPS_GLU_SLOT RVA_GLU_BUILD_2D_MIPMAPS
typedef void *HGLRC;
typedef void (*PROC)(void);

static const GraphicsMacHost *fps_host;

static void **fps_stub_slot(const unsigned char *stub) {
    uint32_t adrp,ldr,br;
    memcpy(&adrp,stub,4);memcpy(&ldr,stub+4,4);memcpy(&br,stub+8,4);
    if((adrp&0x9f00001fu)!=0x90000010u || (ldr&0xffc003ffu)!=0xf9400210u || br!=0xd61f0200u)return NULL;
    int64_t page=(int64_t)((((adrp>>5)&0x7ffffu)<<2)|((adrp>>29)&3u));
    if(page&(1<<20))page-=1<<21;
    return (void **)(((uintptr_t)stub&~(uintptr_t)0xfff)+(uintptr_t)(page<<12)+((ldr>>10)&0xfffu)*8);
}
static const unsigned char *fps_bl_target(const unsigned char *at) {
    uint32_t insn;
    memcpy(&insn,at,4);
    if((insn&0xfc000000u)!=0x94000000u)return NULL;
    return at+(intptr_t)((int32_t)(insn<<6)>>6)*4;
}
static int fps_in_game_image(const void *address,const void *game_base) {
    Dl_info info;
    return address && dladdr(address,&info) && info.dli_fbase==game_base;
}
#endif
