// ---- Opaque terrain texture reserve ----
enum { MAP_RESERVE_SIZE=1024, MAP_RESERVE_MASK=31 };
typedef struct {
    unsigned char *pixels;
    unsigned texture;
    int x,y;
} MapReservedTile;
static MapReservedTile map_reserve[MAP_RESERVE_SIZE];
static unsigned map_reserve_scan, map_reserve_count, map_reserve_pending;
static int map_reserve_close, map_reserve_main, map_crop_x, map_crop_y;
typedef struct { int left,right,low,high; } MapTileBox;
static MapTileBox map_tile_box, map_previous_box;
static unsigned map_trim_left, map_trim_cursor;
static int map_atlas_height;
static long long map_capture_ticks;
static unsigned map_captured_this_pass;
static HGLRC map_reserve_context;
static char map_reserve_tag[64];
static volatile LONG map_reserve_reset;
static int map_reserve_draining, map_reserve_failed, map_load_observed;
// Keep only names for old contexts. Allow one full reserve to migrate without starving its successor.
enum { MAP_NAME_LIMIT=2*MAP_RESERVE_SIZE };
typedef struct { HGLRC context; unsigned texture; unsigned long long generation; } MapRetiredTile;
static MapRetiredTile map_retired[MAP_NAME_LIMIT];
static unsigned map_retired_count;
static int map_retired_scan;
static unsigned long long map_generation;

// Invalidate immediately; delete names only on their own rendering context.
static void map_load_world(const CallInfo *call) {
    (void)call;
    InterlockedExchange(&map_reserve_reset,1);
}

// Map coordinates identify a candidate; adoption always compares the entire tile.
static MapReservedTile *map_reserve_slot(int x,int y) {
    unsigned index=((unsigned)(y/64)&MAP_RESERVE_MASK)*32+((unsigned)(x/64)&MAP_RESERVE_MASK);
    return &map_reserve[index];
}

// A recycled context handle and texture number cannot establish ownership.
static int map_name_matches(unsigned texture,const char *expected) {
    char tag[sizeof map_reserve_tag]={0};
    int length=0;
    if(map_read_label && glIsTexture(texture)){
        map_read_label(GL_TEXTURE,texture,sizeof tag,&length,tag);
        if(length>0 && length<(int)sizeof tag && !strcmp(tag,expected))return 1;
    }
    return 0;
}

// Stop adoption if a live name has changed owners.
static int map_owns_reserved_name(unsigned texture) {
    if(map_name_matches(texture,map_reserve_tag))return 1;
    map_reserve_failed=1;
    return 0;
}

// A zero texture denotes a pending CPU snapshot.
static void map_release_slot(MapReservedTile *slot,int transfer) {
    if(!slot->texture)--map_reserve_pending;
    else if(!transfer && map_owns_reserved_name(slot->texture))glDeleteTextures(1,&slot->texture);
    free(slot->pixels);
    *slot=(MapReservedTile){0};
    --map_reserve_count;
}

// Tag each reserve generation separately, including recycled context handles.
static void map_make_tag(char *tag,unsigned long long generation) {
    snprintf(tag,sizeof map_reserve_tag,"CoE5SE terrain %lu %llu",(unsigned long)GetCurrentProcessId(),generation);
}

// Under the map-pass lock, release CPU snapshots and retire GL names only on the owning context.
static void map_sync_reserve_context(HGLRC current) {
    if(current!=map_reserve_context){
        for(unsigned i=0;map_reserve_count && i<MAP_RESERVE_SIZE;i++){
            MapReservedTile *slot=&map_reserve[i];
            if(!slot->pixels)continue;
            if(slot->texture)
                map_retired[map_retired_count++]=(MapRetiredTile){map_reserve_context,slot->texture,map_generation};
            map_release_slot(slot,1);
        }
        map_reserve_context=current;
        map_make_tag(map_reserve_tag,++map_generation);
        map_retired_scan=1;map_reserve_failed=!map_load_observed;
        map_reserve_draining=map_atlas_height=map_trim_left=0;
        map_previous_box=(MapTileBox){0};
    }
    if(!map_retired_scan || !current || current!=ta_context || !map_read_label)return;
    unsigned processed=0;
    for(unsigned i=0;i<map_retired_count && processed<32;){
        MapRetiredTile tile=map_retired[i];
        if(tile.context!=current){i++;continue;}
        char tag[sizeof map_reserve_tag];
        map_make_tag(tag,tile.generation);
        if(map_name_matches(tile.texture,tag))glDeleteTextures(1,&tile.texture);
        map_retired[i]=map_retired[--map_retired_count];
        processed++;
    }
    map_retired_scan=processed==32;
}

// Reset pass-local state. All owned pixels come from the native crop call.
static void map_reserve_begin(unsigned plane) {
    map_crop_x=map_crop_y=-1;
    map_capture_ticks=0;
    map_captured_this_pass=0;
    map_reserve_main=plane==0;
    map_reserve_close=map_reserve_main && *(const float *)(detail_base+RVA_MAP_ZOOM)==48.f;
    map_tile_box=(MapTileBox){INT_MAX,-1,INT_MAX,-1};
    if(InterlockedExchange(&map_reserve_reset,0) || !map_reserve_main){
        map_reserve_draining=map_reserve_count!=0;
        map_atlas_height=0;
    }
}

// Record the visible tile rectangle in atlas coordinates for overlap after a pan.
static void map_draw_tile(int plane,int x,int y,float screen_x,float screen_y) {
    if(map_texture_pass==MAP_TERRAIN && map_reserve_close && !plane && (unsigned)x<160 && (unsigned)y<160 && map_atlas_height>=64){
        int col=x*64,row=map_atlas_height-64*(y+1);
        if(col<map_tile_box.left)map_tile_box.left=col;
        if(col>map_tile_box.right)map_tile_box.right=col;
        if(row<map_tile_box.low)map_tile_box.low=row;
        if(row>map_tile_box.high)map_tile_box.high=row;
    }
    ((void (*)(int,int,int,float,float))(detail_base+RVA_MAP_BG_TILE_DRAW))(plane,x,y,screen_x,screen_y);
}

// Snapshot adjacent strips within this crop's valid atlas lifetime.
static void map_capture_neighbors(const unsigned char *src,int w,int h,int x,int y) {
    if(!map_reserve_close || !map_label || !map_read_label || map_reserve_draining || map_reserve_failed || map_reserve_count==MAP_RESERVE_SIZE ||
       map_captured_this_pass>=256 || !map_tick_frequency || map_capture_ticks>=map_tick_frequency/1000 ||
       w>160*64 || h>160*64 || (x|y)%64)return;
    long long start=map_ticks();
    static const int offsets[][2]={{-4,0},{4,0},{0,-4},{0,4},{-8,0},{8,0},{0,-8},{0,8},
                                     {-4,-4},{4,-4},{-4,4},{4,4},{-12,0},{12,0},{0,-12},{0,12}};
    for(unsigned i=0;i<sizeof offsets/sizeof *offsets && map_captured_this_pass<256 &&
        map_reserve_count+map_retired_count<MAP_NAME_LIMIT;i++){
        int nx=x+64*offsets[i][0],ny=y+64*offsets[i][1];
        if(nx<0 || nx>w-64 || ny<0 || ny>h-64)continue;
        MapReservedTile *slot=map_reserve_slot(nx,ny);
        if(slot->pixels)continue;
        const unsigned char *first=src+4*((size_t)ny*w+nx);
        if(first[3]!=255)continue;
        unsigned char tile[MAP_TILE_BYTES];
        tcCopyRows(src,w,nx,ny,tile);
        unsigned c=3;
        while(c<MAP_TILE_BYTES && tile[c]==255)c+=4;
        if(c<MAP_TILE_BYTES)continue;
        unsigned char *pixels=malloc(sizeof tile);
        if(!pixels){map_reserve_failed=1;break;}
        memcpy(pixels,tile,sizeof tile);
        *slot=(MapReservedTile){.pixels=pixels,.x=nx,.y=ny};
        ++map_reserve_count;
        ++map_reserve_pending;
        ++map_captured_this_pass;
    }
    map_capture_ticks+=map_ticks()-start;
}

// Build a first-use texture with the game's opaque terrain sampling parameters.
static int map_prepare_slot(MapReservedTile *slot) {
    unsigned name=0;
    glGenTextures(1,&name);
    if(!name)return 0;
    glBindTexture(GL_TEXTURE_2D,name);
    map_label(GL_TEXTURE,name,-1,map_reserve_tag);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,0x812f);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,0x812f);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    int result=arrays_real_gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,64,64,GL_RGBA,GL_UNSIGNED_BYTE,slot->pixels);
    if(result || !map_level_widths(64,64,1) || !map_owns_reserved_name(name)){
        glDeleteTextures(1,&name);return 0;
    }
    slot->texture=name;
    --map_reserve_pending;
    return 1;
}

// Spend at most one slice per pass, with no GL names shared between contexts.
static void map_reserve_end(void) {
    if(!map_reserve_count){map_reserve_draining=0;return;}
    if(!ta_current_context || !ta_context_ok || ta_current_context()!=ta_context ||
       (map_reserve_context && map_reserve_context!=ta_context))return;
    if(!map_reserve_close || map_reserve_failed)map_reserve_draining=1;
    if(map_reserve_draining){
        for(unsigned i=0,freed=0;i<MAP_RESERVE_SIZE && freed<32 && map_reserve_count;i++){
            MapReservedTile *slot=&map_reserve[map_reserve_scan++%MAP_RESERVE_SIZE];
            if(slot->pixels || slot->texture){map_release_slot(slot,0);++freed;}
        }
        if(!map_reserve_count)map_reserve_draining=0;
        return;
    }
    if(map_tile_box.right>=0 && memcmp(&map_previous_box,&map_tile_box,sizeof map_tile_box)!=0){
        map_previous_box=map_tile_box;
        map_trim_left=MAP_RESERVE_SIZE;
    }
    for(unsigned i=0;i<64 && map_trim_left;i++,map_trim_left--){
        MapReservedTile *slot=&map_reserve[map_trim_cursor++%MAP_RESERVE_SIZE];
        if(slot->pixels && (slot->x<map_previous_box.left-12*64 || slot->x>map_previous_box.right+12*64 ||
                            slot->y<map_previous_box.low-12*64 || slot->y>map_previous_box.high+12*64))
            map_release_slot(slot,0);
    }
    if(!map_reserve_pending || !map_tick_frequency || arrays_open || arrays_native_open ||
       !map_simple_upload(256,64))return;
    int bound,alignment;
    glGetIntegerv(GL_TEXTURE_BINDING_2D,&bound);
    glGetIntegerv(GL_UNPACK_ALIGNMENT,&alignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    long long start=map_ticks();
    for(unsigned i=0;i<MAP_RESERVE_SIZE && map_reserve_pending && map_ticks()-start<map_tick_frequency/2000;i++){
        MapReservedTile *slot=&map_reserve[map_reserve_scan++%MAP_RESERVE_SIZE];
        if(slot->pixels && !slot->texture && !map_prepare_slot(slot)){map_reserve_failed=1;break;}
    }
    glBindTexture(GL_TEXTURE_2D,(unsigned)bound);
    glPixelStorei(GL_UNPACK_ALIGNMENT,alignment);
}

// Transfer a matching private name exactly once with the constructor's observable state.
static int map_create_tile(int w,int h,void *pixels,short kind,short extra,int a,int b,int c,int d,int e) {
    if(map_texture_pass==MAP_TERRAIN && map_reserve_main && !map_reserve_draining && !map_reserve_failed &&
       !map_reserve_reset && map_crop_x>=0 && map_crop_y>=0 && w==64 && h==64 && pixels && kind==1 &&
       !extra && a==1 && b==1 && c==1 && d==1 && !e && map_reserve_context &&
       ta_current_context()==map_reserve_context && *(const int *)(detail_base+RVA_MAP_MAX_TILE_TEXTURE)>=64){
        MapReservedTile *tile=map_reserve_slot(map_crop_x,map_crop_y);
        if(tile->texture && tile->x==map_crop_x && tile->y==map_crop_y && !memcmp(tile->pixels,pixels,MAP_TILE_BYTES) &&
           !arrays_open && !arrays_native_open && map_simple_upload(256,64) && map_owns_reserved_name(tile->texture)){
            unsigned name=tile->texture;
            map_label(GL_TEXTURE,name,0,NULL);
            char label[2];
            int length=-1;
            map_read_label(GL_TEXTURE,name,sizeof label,&length,label);
            if(length==0){
                arrays_glPixelStorei(GL_UNPACK_ALIGNMENT,1);
                arrays_bind_probe(GL_TEXTURE_2D,name);
                map_release_slot(tile,1);
                return (int)name;
            }
            map_reserve_failed=1;
        }
    }
    return ((int (*)(int,int,void *,short,short,int,int,int,int,int))(detail_base+RVA_TEXTURE_CREATE))
        (w,h,pixels,kind,extra,a,b,c,d,e);
}
