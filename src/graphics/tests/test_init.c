#include "../graphics.c"
static int mismatches,settings_calls,wrap_calls;
static int main_switch,render_switch,frame_switch;

// Counts refusal diagnostics without running graphics initialization.
static void capture_refusal(const void *owner,const char *line){(void)owner;if(strstr(line,"unsupported_game_hash"))mismatches++;}

// Supplies independent switches without permitting game-memory access.
static const int *capture_setting(const char *name){
    settings_calls++;
    if(!strcmp(name,"graphics_changes"))return &main_switch;
    if(!strcmp(name,"smoother_map_rendering"))return &render_switch;
    if(!strcmp(name,"frame_controls"))return &frame_switch;
    return NULL;
}
// Counts wrapper installations that disabled features must never request.
static int capture_wrap(CallSite site,void *hook,void **next){(void)site;(void)hook;(void)next;wrap_calls++;return 0;}

// Verifies null/API and foreign-game refusal through the built-in entry point.
int main(void){
    PluginApi api={0};api.version=PLUGIN_API_VERSION;api.logLine=capture_refusal;api.setting=capture_setting;api.wrap=capture_wrap;
    if(coe5seGraphicsInit(NULL,NULL)!=0)return 1;
    api.version=PLUGIN_API_VERSION-1;if(coe5seGraphicsInit(&api,NULL)!=0 || mismatches)return 2;
    api.version=PLUGIN_API_VERSION;
    for(unsigned mask=0;mask<8;mask++){
        main_switch=!!(mask&4);render_switch=!!(mask&1);frame_switch=!!(mask&2);
        unsigned expected=main_switch?((render_switch?MOD_RENDERING:0)|(frame_switch?MOD_FRAME_CONTROLS:0)):0;
        if(mod_requested_features(&api)!=expected)return 3;
        if(!expected && (coe5seGraphicsInit(&api,NULL)!=1 || mismatches || wrap_calls || fps_graphics_ini[0]))return 4;
    }
    main_switch=render_switch=1;frame_switch=0;settings_calls=0;
    if(coe5seGraphicsInit(&api,NULL)!=1 || mismatches!=1 || settings_calls!=3 || wrap_calls)return 5;
    puts("PASS: API/null guards, all eight master/child combinations, fully disabled startup and foreign-executable refusal.");return 0;
}
