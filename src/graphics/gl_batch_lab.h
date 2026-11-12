// Shared native OpenGL entry points and import replacement.
static _Thread_local int decor_scope;
static void (*real_gl_color3)(float,float,float);
static void (*real_gl_begin)(unsigned);
static void (*real_gl_end)(void);
static void (*real_gl_bind)(unsigned,unsigned);
static void (*real_gl_enable)(unsigned);
static void (*real_gl_disable)(unsigned);
static void (*real_gl_blend)(unsigned,unsigned);
static void (*real_gl_alpha)(unsigned,float);
static void (*real_gl_depth_mask)(unsigned char);
static void (*real_gl_tex_env)(unsigned,unsigned,int);
static void (*real_gl_color)(float,float,float,float);
static int replace_gl_import(uintptr_t rva,void *hook,void *saved) {
    void **slot=(void **)(detail_base+rva);
    memcpy(saved,slot,sizeof(void *));
    DWORD old,unused;
    if(!VirtualProtect(slot,sizeof(void *),PAGE_READWRITE,&old)) return 0;
    InterlockedExchangePointer(slot,hook);VirtualProtect(slot,sizeof(void *),old,&unused);return 1;
}
