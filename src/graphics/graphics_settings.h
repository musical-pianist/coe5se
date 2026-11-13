#ifdef _WIN32
#include <shellapi.h>
#include <wchar.h>
#define FPS_GPU_WAIT_POINTER (int *)(detail_base+RVA_FPS_GPU_WAIT)
#define FPS_GPU_COMMIT() ((void)0)
#define FPS_FRAME_CHOICES_HERE(id) 1
#else
#include <wchar.h>
#include <crt_externs.h>
#include <sys/stat.h>
#define FPS_GPU_WAIT_POINTER fps_gpu_wait_bind()
#define FPS_GPU_COMMIT() fps_gpu_wait_commit()
#define FPS_FRAME_CHOICES_HERE(id) ((id)==408)
#endif

static int *fps_gpu_wait;
static int *fps_frame_interval;
static int fps_pending_frame_interval=-1;
static int fps_graphics_settings_active;
static int fps_benchmark_mode;
static wchar_t fps_graphics_ini[MAX_PATH];
static void (*fps_next_graphics_section_end)(void);
static int (*fps_next_preferences_wait)(int,int);
static int (*fps_next_frame_rate_choice)(int,int,const char *,...);
static void (*fps_next_map_draw)(void);
#ifdef __APPLE__
#include "graphics_settings_mac.h"
#endif
#include "frame_pacing.h"

typedef struct {int interval;const char *label,*tip;} FpsFrameOption;
static const FpsFrameOption fps_frame_options[]={
    {7,"144 Frames per second","Approximate target: about 143 FPS before rendering and other limits. Applies immediately and is remembered. Explicit --maxfps launch options take priority on restart."},
    {6,"165 Frames per second","Approximate target: about 167 FPS before rendering and other limits. Applies immediately and is remembered. Explicit --maxfps launch options take priority on restart."},
    {4,"240 Frames per second","Approximate target: about 250 FPS before rendering and other limits. Applies immediately and is remembered. Explicit --maxfps launch options take priority on restart."},
    {3,"360 Frames per second","Approximate target: about 333 FPS before rendering, scheduling and other limits. Actual FPS can be lower. Applies immediately and is remembered. Explicit --maxfps launch options take priority on restart."}
};
static int fps_frame_argument(int argc,wchar_t **argv) {
    for(int i=1;i<argc;i++)if(!wcsncmp(argv[i],L"--maxfps=",9))return 1;
    return 0;
}
static int fps_benchmark_argument(int argc,wchar_t **argv) {
    for(int i=1;i<argc;i++)if(!wcscmp(argv[i],L"--benchmark"))return 1;
    return 0;
}
static int fps_frame_preference(const wchar_t *value) {
    if(!value[0] || (value[0]==L'0' && value[1]))return -1;
    int result=0;
    for(size_t i=0;value[i];i++) {
        if(value[i]<L'0' || value[i]>L'9')return -1;
        result=result*10+value[i]-L'0';
        if(result>200)return -1;
    }
    return result>0?result:-1;
}
static void fps_log_frame_limit(int interval,const char *source,int saved) {
    char line[240];
    snprintf(line,sizeof(line),"fps_mod frame_limit interval_ms=%d unlimited=%d source=%s saved=%d timing=%s quality_unchanged=1",interval,interval==0,source,saved,fps_pacing_enabled?"high_resolution":"native");
    mod_log(line);
}

#ifdef _WIN32
static int fps_write_preference(const wchar_t *key,const wchar_t *value){
    if(!fps_graphics_ini[0])return 0;
    const wchar_t *keys[]={L"FrameIntervalMs",L"PreciseFrameLimit",L"WaitForGpu"};
    wchar_t values[3][32],temporary[MAX_PATH];
    for(unsigned i=0;i<3;i++){
        GetPrivateProfileStringW(L"Graphics",keys[i],L"",values[i],32,fps_graphics_ini);
        if(!wcscmp(key,keys[i])){if(wcslen(value)>=32)return 0;wcscpy(values[i],value);}
    }
    int n=swprintf(temporary,MAX_PATH,L"%ls.%lu.tmp",fps_graphics_ini,(unsigned long)GetCurrentProcessId());
    if(n<0 || n>=MAX_PATH)return 0;
    char text[400];int length=snprintf(text,sizeof text,"[Graphics]\r\nFrameIntervalMs=%ls\r\nPreciseFrameLimit=%ls\r\nWaitForGpu=%ls\r\n",values[0],values[1],values[2]);
    if(length<0 || (size_t)length>=sizeof text)return 0;
    HANDLE file=CreateFileW(temporary,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return 0;
    DWORD written=0;int ok=WriteFile(file,text,(DWORD)length,&written,NULL) && written==(DWORD)length && FlushFileBuffers(file);
    if(!CloseHandle(file))ok=0;
    if(ok)ok=MoveFileExW(temporary,fps_graphics_ini,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
    if(!ok)DeleteFileW(temporary);
    return ok;
}
#else
static int fps_write_preference(const wchar_t *key,const wchar_t *value){return fps_write_preference_file(fps_graphics_ini,key,value);}
#endif
static int fps_store_frame_limit(int interval) {
    if(fps_benchmark_mode || interval<=0)return 0;
    wchar_t value[16];swprintf(value,16,L"%d",interval);
    return fps_write_preference(L"FrameIntervalMs",value);
}
static void fps_apply_frame_preference(void) {
    if(!fps_graphics_settings_active)return;
    if(fps_benchmark_mode || *fps_frame_interval<=0){fps_pending_frame_interval=-1;return;}
    if(fps_pending_frame_interval>0) {
        *fps_frame_interval=fps_pending_frame_interval;
        fps_pending_frame_interval=-1;
        fps_log_frame_limit(*fps_frame_interval,"preference",0);
    }
}
static void fps_frame_preference_map_draw(void) {fps_apply_frame_preference();fps_next_map_draw();}
static void fps_save_frame_limit(int interval) {
    if(fps_benchmark_mode || interval<=0)return;
    fps_pacing_launch_target=0;fps_pacing_reset();
    int saved=fps_store_frame_limit(interval);
    fps_log_frame_limit(interval,"graphics_menu",saved);
}
static int fps_frame_rate_choices(int id,int selected,const char *label) {
    fps_apply_frame_preference();
    int result=fps_next_frame_rate_choice(id,selected,label);
    if(fps_graphics_settings_active && FPS_FRAME_CHOICES_HERE(id)) {
        for(size_t i=0;i<sizeof(fps_frame_options)/sizeof(fps_frame_options[0]);i++) {
            const FpsFrameOption *option=&fps_frame_options[i];
            plugin_api->ui->choice(400+option->interval,*fps_frame_interval==option->interval,option->label);
            plugin_api->ui->tooltip(fps_pacing_enabled?"Precise frame pacing uses this FPS target. Rendering and driver limits can still lower actual FPS. Background rendering is capped at 30 FPS. Graphics quality is unchanged.":option->tip);
        }
    }
    return result;
}

static int fps_gpu_wait_argument(int argc,wchar_t **argv) {
    int value=-1;
    for(int i=1;i<argc;i++) {
        if(!wcscmp(argv[i],L"--glfinish"))value=1;
        if(!wcscmp(argv[i],L"--noglfinish"))value=0;
    }
    return value;
}
static int fps_gpu_wait_preference(const wchar_t *value) {
    if(!wcscmp(value,L"0"))return 0;
    if(!wcscmp(value,L"1"))return 1;
    return -1;
}
static void fps_log_gpu_wait(const char *source,int saved) {
    char line[240];
    snprintf(line,sizeof(line),"fps_mod gpu_wait glfinish=%d source=%s saved=%d quality_unchanged=1 vsync_unchanged=1",*fps_gpu_wait,source,saved);
    mod_log(line);
}
static void fps_save_gpu_wait(void) {
    int saved=fps_write_preference(L"WaitForGpu",*fps_gpu_wait?L"1":L"0");
    fps_log_gpu_wait("graphics_menu",saved);
}
static void fps_log_precise_limit(const char *source,int saved){
    int interval=fps_pending_frame_interval>0?fps_pending_frame_interval:*fps_frame_interval;
    char line[300];snprintf(line,sizeof(line),"fps_mod precise_frame_limit enabled=%d source=%s saved=%d target=%d background_target=30 busy_wait=0 game_clock_unchanged=1 quality_unchanged=1",fps_pacing_enabled,source,saved,fps_pacing_launch_target?fps_pacing_launch_target:fps_pacing_interval_target(interval));mod_log(line);
}
static void fps_save_precise_limit(void){
    (void)fps_pacing_enable(fps_precise_setting);
    int saved=fps_write_preference(L"PreciseFrameLimit",fps_precise_setting?L"1":L"0");
    fps_log_precise_limit("graphics_menu",saved);
}
static void fps_graphics_section_end(void) {
    if(fps_graphics_settings_active) {
        plugin_api->ui->checkbox(1,fps_gpu_wait,"Wait for GPU each frame");
        plugin_api->ui->tooltip("Extra GPU synchronization (glFinish). Off = --noglfinish; may improve performance. Graphics quality is unchanged. This is not VSync. Applies immediately and is remembered; explicit launch options take priority on restart.");
        plugin_api->ui->checkbox(2,&fps_precise_setting,"Precise frame rate limit");
        plugin_api->ui->tooltip("Uses a high-resolution timer for the selected Frames per second target. For a 240 Hz display choose 240. Caps background rendering at 30 FPS. Higher FPS can increase CPU/GPU usage. Graphics quality and hardware clock/voltage settings are unchanged. Applies immediately and is remembered.");
    }
    fps_next_graphics_section_end();
}
static int fps_preferences_wait(int a,int b) {
    int before=fps_graphics_settings_active?*fps_gpu_wait:0;
    int precise_before=fps_precise_setting;
    int result=fps_next_preferences_wait(a,b);
    FPS_GPU_COMMIT();
    if(fps_graphics_settings_active && *fps_gpu_wait!=before)fps_save_gpu_wait();
    if(fps_graphics_settings_active && fps_precise_setting!=precise_before)fps_save_precise_limit();
    if(fps_graphics_settings_active && result>400 && result<500)fps_save_frame_limit(result-400);
    return result;
}
#ifdef _WIN32
static int fps_graphics_user_path(const char *userFolder) {
    wchar_t relative[MAX_PATH],folder[MAX_PATH];
    fps_graphics_ini[0]=0;
    if(!userFolder || !userFolder[0] || !MultiByteToWideChar(CP_ACP,0,userFolder,-1,relative,MAX_PATH))return 0;
    DWORD length=GetFullPathNameW(relative,MAX_PATH,folder,NULL);
    if(!length || length>=MAX_PATH)return 0;
    int n=swprintf(relative,MAX_PATH,L"%ls\\coe5se",folder);
    if(n<0 || n>=MAX_PATH)return 0;
    if(!CreateDirectoryW(relative,NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)return 0;
    DWORD attributes=GetFileAttributesW(relative);
    if(attributes==INVALID_FILE_ATTRIBUTES || !(attributes&FILE_ATTRIBUTE_DIRECTORY))return 0;
    n=swprintf(fps_graphics_ini,MAX_PATH,L"%ls\\CoE5SE-Graphics.ini",relative);
    if(n<0 || n>=MAX_PATH){fps_graphics_ini[0]=0;return 0;}
    return 1;
}
#else
static int fps_graphics_user_path(const char *userFolder){return fps_graphics_user_path_mac(userFolder,fps_graphics_ini);}
#endif
static void fps_initialize_graphics_settings(const char *userFolder) {
    if(!plugin_api->ui || !plugin_api->ui->checkbox || !plugin_api->ui->tooltip || !plugin_api->ui->choice) {
        mod_log("fps_mod graphics_menu unavailable reason=ui_api_missing");return;
    }
    fps_gpu_wait=FPS_GPU_WAIT_POINTER;
    fps_frame_interval=(int *)(detail_base+RVA_FRAME_DELAY_MS);
    if(*fps_gpu_wait!=0 && *fps_gpu_wait!=1) {
        mod_log("fps_mod graphics_menu unavailable reason=unexpected_glfinish_value");return;
    }
    if(!fps_graphics_user_path(userFolder))mod_log("fps_mod graphics_menu preferences_unavailable reason=user_path_unavailable");
    int argc=0;wchar_t **argv=CommandLineToArgvW(GetCommandLineW(),&argc);
    int explicit_value=argv?fps_gpu_wait_argument(argc,argv):-1;
    int explicit_frame_limit=argv?fps_frame_argument(argc,argv):0;
    fps_pacing_launch_target=argv?fps_pacing_argument(argc,argv):0;
    int arguments_available=argv!=NULL;
    if(argv)LocalFree(argv);
    wchar_t value[32]=L"";
    if(fps_graphics_ini[0])GetPrivateProfileStringW(L"Graphics",L"WaitForGpu",L"",value,32,fps_graphics_ini);
    int preference=fps_gpu_wait_preference(value);
    const char *source="game_default";
    if(explicit_value>=0){*fps_gpu_wait=explicit_value;source="launch_option";}
    else if(arguments_available && preference>=0){*fps_gpu_wait=preference;source="preference";}
    else if(value[0])mod_log("fps_mod graphics_menu ignored reason=invalid_saved_glfinish_value");
    FPS_GPU_COMMIT();
    fps_graphics_settings_active=1;fps_log_gpu_wait(source,0);
    value[0]=0;
    if(fps_graphics_ini[0])GetPrivateProfileStringW(L"Graphics",L"FrameIntervalMs",L"",value,32,fps_graphics_ini);
    preference=fps_frame_preference(value);source="game_default";
    if(explicit_frame_limit)source="launch_option";
    else if(arguments_available && preference>0){fps_pending_frame_interval=preference;source="preference_pending";}
    else if(value[0])mod_log("fps_mod graphics_menu ignored reason=invalid_saved_frame_interval");
    fps_log_frame_limit(fps_pending_frame_interval>=0?fps_pending_frame_interval:*fps_frame_interval,source,0);
    value[0]=0;
    if(fps_graphics_ini[0])GetPrivateProfileStringW(L"Graphics",L"PreciseFrameLimit",L"0",value,32,fps_graphics_ini);
    fps_precise_setting=arguments_available && fps_gpu_wait_preference(value)==1;
    (void)fps_pacing_enable(fps_precise_setting);fps_log_precise_limit("preference",0);
    mod_log("fps_mod graphics_menu ready page=Graphics_2 label=Wait_for_GPU_each_frame default_preserved=1");
    mod_log("fps_mod graphics_menu ready page=Graphics_1 frame_targets=144,165,240,360 timing=precise_optional");
}
