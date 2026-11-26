#pragma once

// @platform-split terrain-batching: Windows terrain rectangle calls are mapped.
#define TERRAIN_QUADS 512u
#define TERRAIN_UPLOADS_PER_PASS 64
typedef struct { float x,y,w,h,u,v,du,dv; } TerrainQuad;
typedef struct { float xy[2],uv[4]; } TerrainVertex;
typedef void (*TerrainNativeRect)(float,float,float,float,float,float,float,float);
static _Thread_local TerrainVertex terrain_vertices[TERRAIN_QUADS*4];
static _Thread_local TerrainQuad terrain_quads[TERRAIN_QUADS];
static _Thread_local unsigned terrain_sources[TERRAIN_QUADS];
static _Thread_local unsigned short terrain_indices[TERRAIN_QUADS*6];
static _Thread_local unsigned terrain_count,terrain_page;
static _Thread_local int terrain_client_ready;
static _Thread_local long long terrain_upload_start;
static _Thread_local long long terrain_batches,terrain_tiles;
static _Thread_local void (*terrain_draw_elements)(unsigned,int,unsigned,const void *);

static void terrain_native_rect(TerrainQuad q) {
    ((TerrainNativeRect)(detail_base+RVA_FPS_TERRAIN_RECT_DRAW))(q.x,q.y,q.w,q.h,q.u,q.v,q.du,q.dv);
}
// Replay captured fixed-function tiles with their original sampling state, then return it to the caller.
static void terrain_replay(unsigned count) {
    TaReplaySampling saved=ta_begin_native_replay();
    int restore=0;
    arrays_get_int(0x8069,&restore);
    for(unsigned i=0;i<count;i++) {
        real_gl_bind(0x0de1,terrain_sources[i]);
        terrain_native_rect(terrain_quads[i]);
    }
    real_gl_bind(0x0de1,(unsigned)restore);
    ta_end_native_replay(saved);
}

static int terrain_submission_available(void) {
    if(!ta_client_stack_room() || (ta_has_restart && arrays_is_enabled(0x8f9d)) ||
       (ta_has_nv_restart && arrays_is_enabled(0x8558)))return 0;
    int value=0;
    ta_get_int(0x8b8d,&value);if(value)return 0;
    ta_get_int(0x84e0,&value);if(value!=0x84c0)return 0;
    ta_get_int(0x8ca6,&value);if(value)return 0;
    ta_get_int(0x8caa,&value);if(value)return 0;
    if(ta_has_sampler){ta_get_int(0x8919,&value);if(value)return 0;}
    if(arrays_has_vbo) {
        ta_get_int(0x8894,&value);if(value)return 0;
        ta_get_int(0x8895,&value);if(value)return 0;
    }
    if(arrays_has_vao){ta_get_int(0x85b5,&value);if(value)return 0;}
    if(arrays_client_active){ta_get_int(0x84e1,&value);if(value!=0x84c0)return 0;}
    return !arrays_is_enabled(0x8074) && !arrays_is_enabled(0x8076) && !arrays_is_enabled(0x8078);
}

static void terrain_flush(void) {
    unsigned count=terrain_count;
    if(!count)return;
    terrain_count=0;
    if(!ta_allowed_state() || !terrain_submission_available() || !ta_legacy_sampling_allowed()) {terrain_replay(count);return;}
    unsigned unit=ta_multi_page?0x84c1u:0x84c0u;
    int saved_array=0,saved_sampler=0;
    ta_active_texture(unit);
    ta_get_int(0x8c1d,&saved_array);
    if(ta_multi_page && ta_has_sampler) {
        ta_get_int(0x8919,&saved_sampler);
        ta_bind_sampler(unit-0x84c0u,0);
    }
    ta_bind(0x8c1a,terrain_page);
    ta_active_texture(0x84c0);
    ta_use_program(ta_program);
    arrays_push_client(0x00000002u);
    arrays_enable_client(0x8074);arrays_enable_client(0x8078);
    arrays_vertex_pointer(2,0x1406,sizeof(TerrainVertex),terrain_vertices[0].xy);
    arrays_tex_pointer(4,0x1406,sizeof(TerrainVertex),terrain_vertices[0].uv);
    terrain_draw_elements(4,(int)(count*6),0x1403,terrain_indices);
    arrays_pop_client();
    ta_use_program(0);
    ta_active_texture(unit);
    ta_bind(0x8c1a,(unsigned)saved_array);
    if(ta_multi_page && ta_has_sampler)ta_bind_sampler(unit-0x84c0u,(unsigned)saved_sampler);
    ta_active_texture(0x84c0);
    arrays_color_known=0;arrays_uv_known=0;
    terrain_batches++;terrain_tiles+=count;
}

static int terrain_check_client(void) {
    if(!arrays_capture_allowed())return 0;
    int element_buffer=0,unit=0x84c0,sample_buffers=0;
    if(arrays_has_vbo)arrays_get_int(0x8895,&element_buffer);
    if(arrays_client_active)arrays_get_int(0x84e1,&unit);
    arrays_get_int(0x80a8,&sample_buffers);
    if(element_buffer || unit!=0x84c0 || sample_buffers)return 0;
    return !arrays_is_enabled(0x8074) && !arrays_is_enabled(0x8076) && !arrays_is_enabled(0x8078);
}

static void terrain_rect(float x,float y,float w,float h,float u,float v,float du,float dv) {
#ifdef __clang__
#pragma clang fp contract(off)
#endif
    TerrainQuad q={x,y,w,h,u,v,du,dv};
    if(!terrain_scope || in_highest || arrays_native_open) {terrain_native_rect(q);return;}
    FpsTextureLayer layer;
    if(terrain_client_ready<0)terrain_client_ready=terrain_check_client();
    int allow_upload=ta_terrain_cache.attempts-terrain_upload_start<TERRAIN_UPLOADS_PER_PASS;
    if(!terrain_client_ready || !ta_prepare(&ta_terrain_cache,arrays_texture,&layer,allow_upload) ||
       !ta_capture_sampling_allowed(!terrain_count)) {
        terrain_flush();terrain_native_rect(q);return;
    }
    if(terrain_count==TERRAIN_QUADS || (terrain_count && terrain_page!=layer.array_texture))terrain_flush();
    terrain_page=layer.array_texture;
    unsigned index=terrain_count++;
    terrain_sources[index]=arrays_texture;
    terrain_quads[index]=q;

    float sx=(float)*(int *)(detail_base+RVA_WINDOW_W)/(float)*(int *)(detail_base+RVA_SCREEN_W);
    float sy=(float)*(int *)(detail_base+RVA_WINDOW_H)/(float)*(int *)(detail_base+RVA_SCREEN_H);
    float left=x*sx,right=(x+w)*sx,top=y*sy,bottom=(y+h)*sy;
    float s2=u+du,t2=v+dv,l=(float)layer.layer;
    TerrainVertex *vertices=terrain_vertices+index*4;
    vertices[0]=(TerrainVertex){{left,top},{u,v,l,0}};
    vertices[1]=(TerrainVertex){{right,top},{s2,v,l,0}};
    vertices[2]=(TerrainVertex){{right,bottom},{s2,t2,l,0}};
    vertices[3]=(TerrainVertex){{left,bottom},{u,t2,l,0}};
}

static int terrain_begin_scope(void) {
    if(!arrays_enabled || !texture_array_enabled || terrain_scope || in_highest)return 0;
    if(!ta_terrain_cache.entries) {
        *(void **)&terrain_draw_elements=(void *)GetProcAddress(ta_gl,"glDrawElements");
        if(!terrain_draw_elements)return 0;
        ta_terrain_cache.entries=FPS_ALLOC(TA_TERRAIN_CACHE_SIZE*sizeof(*ta_terrain_cache.entries));
        if(!ta_terrain_cache.entries)return 0;
        memset(ta_terrain_cache.entries,0,TA_TERRAIN_CACHE_SIZE*sizeof(*ta_terrain_cache.entries));
        ta_terrain_cache.size=TA_TERRAIN_CACHE_SIZE;ta_terrain_cache.layer_limit=1024;
        for(unsigned i=0;i<TERRAIN_QUADS;i++) {
            unsigned short n=(unsigned short)(i*4);
            unsigned short indices[]={n,n+1,n+2,n,n+2,n+3};
            memcpy(terrain_indices+i*6,indices,sizeof(indices));
        }
    }
    array_texture_begin_frame();arrays_begin_frame();
    ++ta_terrain_cache.pass;
    if(ta_terrain_cache.pressure && !(ta_terrain_cache.pass%16) && ta_context_ok && ta_cache_owned(ta_current_context()))
        ta_trim_terrain_cache(&ta_terrain_cache);
    terrain_client_ready=-1;
    terrain_upload_start=ta_terrain_cache.attempts;
    return terrain_scope=1;
}

static void terrain_end_scope(void) {
    terrain_flush();terrain_scope=0;
}
