// ---- Texture-array transfer verification ----
static _Thread_local void (*ta_read_subimage)(unsigned,int,int,int,int,int,int,int,unsigned,unsigned,int,void *);
static _Thread_local void (*ta_gen_fbo)(int,unsigned *),(*ta_bind_fbo)(unsigned,unsigned);
static _Thread_local void (*ta_attach_layer)(unsigned,unsigned,unsigned,int,int);
static _Thread_local unsigned (*ta_fbo_status)(unsigned);
static _Thread_local void (*ta_read_pixels)(int,int,int,int,unsigned,unsigned,void *);
static _Thread_local unsigned ta_check_fbo,ta_read_target,ta_read_binding;
static _Thread_local unsigned ta_check_pack;
static _Thread_local void (*ta_gen_buffers)(int,unsigned *),(*ta_bind_buffer)(unsigned,unsigned);
static _Thread_local void (*ta_buffer_data)(unsigned,ptrdiff_t,const void *,unsigned);
static _Thread_local void *(*ta_map_range)(unsigned,ptrdiff_t,ptrdiff_t,unsigned);
static _Thread_local unsigned char (*ta_unmap_buffer)(unsigned);

// Prefer a direct layer read. Older compatibility contexts can read one attached slice.
static void ta_init_layer_reader(int major,int minor,const char *ext) {
    if(major>4 || (major==4 && minor>=5) || ta_extension(ext,"GL_ARB_get_texture_sub_image"))
        *(void **)&ta_read_subimage=ta_proc("glGetTextureSubImage");
    if(ta_read_subimage){
        *(void **)&ta_gen_buffers=ta_proc("glGenBuffers");
        *(void **)&ta_bind_buffer=ta_proc("glBindBuffer");
        *(void **)&ta_buffer_data=ta_proc("glBufferData");
        *(void **)&ta_map_range=ta_proc("glMapBufferRange");
        *(void **)&ta_unmap_buffer=ta_proc("glUnmapBuffer");
        return;
    }
    int core=major>=3 || ta_extension(ext,"GL_ARB_framebuffer_object");
    if(!core && !ta_extension(ext,"GL_EXT_framebuffer_object"))return;
    *(void **)&ta_gen_fbo=ta_proc(core?"glGenFramebuffers":"glGenFramebuffersEXT");
    *(void **)&ta_bind_fbo=ta_proc(core?"glBindFramebuffer":"glBindFramebufferEXT");
    *(void **)&ta_attach_layer=ta_proc(core?"glFramebufferTextureLayer":"glFramebufferTextureLayerEXT");
    *(void **)&ta_fbo_status=ta_proc(core?"glCheckFramebufferStatus":"glCheckFramebufferStatusEXT");
    *(void **)&ta_read_pixels=ta_proc("glReadPixels");
    int separate=core || ta_extension(ext,"GL_EXT_framebuffer_blit");
    ta_read_target=separate?0x8ca8:0x8d40;
    ta_read_binding=separate?0x8caa:0x8ca6;
}

// Queue all mip reads into one private pack buffer and compare after one GPU wait.
static int ta_copied_levels_match(unsigned texture,int layer,int width,int height,int last) {
    if(!ta_read_subimage || !ta_gen_buffers || !ta_bind_buffer || !ta_buffer_data || !ta_map_range || !ta_unmap_buffer)return 0;
    int previous=0;
    ta_get_int(0x88ed,&previous);
    if(previous)return 0;
    enum { pack=0x88eb };
    if(!ta_check_pack)ta_gen_buffers(1,&ta_check_pack);
    if(!ta_check_pack)return 0;
    ta_bind_buffer(pack,ta_check_pack);
    int binding=0;
    ta_get_int(0x88ed,&binding);
    if((unsigned)binding!=ta_check_pack)return 0;
    size_t offsets[10]={0};
    if(last<0 || last>=9){ta_bind_buffer(pack,0);return 0;}
    for(int i=0;i<=last;i++){
        int w=width>>i,h=height>>i;
        offsets[i+1]=offsets[i]+(size_t)(w?w:1)*(h?h:1)*4;
    }
    size_t bytes=offsets[last+1];
    unsigned char *data;
    int ok=0;
    ta_buffer_data(pack,(ptrdiff_t)(2*bytes),NULL,0x88e1);
    data=ta_map_range(pack,0,(ptrdiff_t)(2*bytes),0x0002|0x0008);
    if(!data)goto done;
    const uint32_t marker=0x17c52a91u;
    for(int i=0;i<=last;i++){
        memcpy(data+offsets[i],&marker,4);
        memcpy(data+bytes+offsets[i],&marker,4);
    }
    if(!ta_unmap_buffer(pack))goto done;
    for(int i=0;i<=last;i++){
        int w=width>>i,h=height>>i;
        ta_get_image(0x0de1,i,0x1908,0x1401,(void *)(uintptr_t)offsets[i]);
        ta_read_subimage(texture,i,0,0,layer,w?w:1,h?h:1,1,0x1908,0x1401,
                         (int)(offsets[i+1]-offsets[i]),(void *)(uintptr_t)(bytes+offsets[i]));
    }
    data=ta_map_range(pack,0,(ptrdiff_t)(2*bytes),0x0001);
    if(!data)goto done;
    ok=1;
    for(int i=0;i<=last && ok;i++)
        if(!memcmp(data+offsets[i],&marker,4) || !memcmp(data+bytes+offsets[i],&marker,4))ok=0;
    if(ok)ok=!memcmp(data,data+bytes,bytes);
    if(!ta_unmap_buffer(pack))ok=0;
done:
    ta_bind_buffer(pack,0);
    return ok;
}

// A rejected read leaves the destination untouched, including its first byte.
static int ta_read_source(int level,unsigned char *pixels) {
    pixels[0]=0;
    ta_get_image(0x0de1,level,0x1908,0x1401,pixels);
    if(pixels[0])return 1;
    pixels[0]=255;
    ta_get_image(0x0de1,level,0x1908,0x1401,pixels);
    return pixels[0]==0;
}

// Verify the complete slice without consuming errors belonging to the game or other mods.
static int ta_layer_matches(unsigned texture,int layer,int level,int w,int h,
                            const unsigned char *expected,unsigned char *actual) {
    size_t bytes=(size_t)w*h*4;
    actual[0]=(unsigned char)~expected[0];
    if(ta_read_subimage){
        ta_read_subimage(texture,level,0,0,layer,w,h,1,0x1908,0x1401,(int)bytes,actual);
    }else{
        if(!ta_gen_fbo || !ta_bind_fbo || !ta_attach_layer || !ta_fbo_status || !ta_read_pixels)return 0;
        if(!ta_check_fbo)ta_gen_fbo(1,&ta_check_fbo);
        if(!ta_check_fbo)return 0;
        int previous=0,bound=0;
        ta_get_int(ta_read_binding,&previous);
        ta_bind_fbo(ta_read_target,ta_check_fbo);
        ta_get_int(ta_read_binding,&bound);
        if((unsigned)bound!=ta_check_fbo)return 0;
        ta_attach_layer(ta_read_target,0x8ce0,texture,level,layer);
        if(ta_fbo_status(ta_read_target)==0x8cd5)
            ta_read_pixels(0,0,w,h,0x1908,0x1401,actual);
        ta_attach_layer(ta_read_target,0x8ce0,0,0,0);
        ta_bind_fbo(ta_read_target,(unsigned)previous);
    }
    return actual[0]==expected[0] && !memcmp(expected,actual,bytes);
}

// Leave the layer free if neither the direct copy nor a checked replacement succeeds.
static int ta_checked_map_layer(unsigned source,unsigned dest,int layer,int width,int height,int last,int rgba) {
    int submitted=rgba && map_copy_layer(source,dest,layer,width,height,last);
    if(submitted && ta_copied_levels_match(dest,layer,width,height,last))return 1;
    size_t largest=(size_t)width*height*4;
    unsigned char *pixels=FPS_ALLOC(2*largest);
    if(!pixels)return ta_refuse(FPS_ALLOCATION);
    unsigned char *actual=pixels+largest;
    int verified=1;
    for(int level=0;level<=last && verified;level++){
        int w=width>>level,h=height>>level;
        if(!w)w=1;
        if(!h)h=1;
        ta_bind(0x0de1,source);
        if(!ta_read_source(level,pixels)){verified=0;break;}
        if(submitted && ta_layer_matches(dest,layer,level,w,h,pixels,actual))continue;
        ta_bind(0x8c1a,dest);
        ta_subimage3d(0x8c1a,level,0,0,layer,w,h,1,0x1908,0x1401,pixels);
        verified=ta_layer_matches(dest,layer,level,w,h,pixels,actual);
    }
    FPS_FREE(pixels);
    return verified?1:ta_refuse(FPS_TEXTURE_TRANSFER);
}
