#include "../graphics.c"
static int mismatches,settings_calls;

// Counts refusal diagnostics without running graphics initialization.
static void capture_refusal(const void *owner,const char *line){(void)owner;if(strstr(line,"unsupported_game_hash"))mismatches++;}

// Records unexpected attempts to read settings on a foreign executable.
static const int *capture_setting(const char *name){(void)name;settings_calls++;return NULL;}

// Verifies null/API and foreign-game refusal through the built-in entry point.
int main(void){
    PluginApi api={0};api.version=PLUGIN_API_VERSION;api.logLine=capture_refusal;api.setting=capture_setting;
    if(coe5seGraphicsInit(NULL)!=0)return 1;
    api.version=PLUGIN_API_VERSION-1;if(coe5seGraphicsInit(&api)!=0 || mismatches)return 2;
    api.version=PLUGIN_API_VERSION;
    if(coe5seGraphicsInit(&api)!=1 || mismatches!=1 || settings_calls)return 3;
    puts("PASS: built-in API/null guards and foreign-executable refusal.");return 0;
}
