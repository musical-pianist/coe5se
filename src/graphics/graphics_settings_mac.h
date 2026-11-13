#pragma once

static int fps_narrow(const wchar_t *wide,char *out,size_t size) {
    size_t n=wcstombs(out,wide,size);
    return n!=(size_t)-1 && n<size;
}
static int fps_widen(const char *narrow,wchar_t *out,size_t count) {
    size_t n=mbstowcs(out,narrow,count);
    return n!=(size_t)-1 && n<count;
}

static DWORD GetPrivateProfileStringW(const wchar_t *section,const wchar_t *key,const wchar_t *fallback,wchar_t *out,DWORD size,const wchar_t *file) {
    char path[MAX_PATH],wanted_section[64],wanted_key[64],line[512];
    out[0]=0;
    int found=0;
    if(fps_narrow(file,path,sizeof(path)) && fps_narrow(section,wanted_section,sizeof(wanted_section)) && fps_narrow(key,wanted_key,sizeof(wanted_key))) {
        FILE *f=fopen(path,"rb");
        int inside=0;
        while(f && fgets(line,sizeof(line),f)) {
            line[strcspn(line,"\r\n")]=0;
            if(line[0]=='[') {
                char *close=strchr(line,']');
                inside=close && (size_t)(close-line-1)==strlen(wanted_section) && !strncasecmp(line+1,wanted_section,strlen(wanted_section));
            } else if(inside) {
                char *equals=strchr(line,'=');
                if(equals && (size_t)(equals-line)==strlen(wanted_key) && !strncasecmp(line,wanted_key,strlen(wanted_key))) {
                    found=fps_widen(equals+1,out,size);
                    break;
                }
            }
        }
        if(f)fclose(f);
    }
    if(!found) {
        wcsncpy(out,fallback,size-1);
        out[size-1]=0;
    }
    return (DWORD)wcslen(out);
}

static int fps_write_preference_file(const wchar_t *ini,const wchar_t *key,const wchar_t *value) {
    if(!ini[0])return 0;
    const wchar_t *keys[]={L"FrameIntervalMs",L"PreciseFrameLimit",L"WaitForGpu"};
    wchar_t values[3][32];
    for(unsigned i=0;i<3;i++) {
        GetPrivateProfileStringW(L"Graphics",keys[i],L"",values[i],32,ini);
        if(!wcscmp(key,keys[i])) {
            if(wcslen(value)>=32)return 0;
            wcscpy(values[i],value);
        }
    }
    char path[MAX_PATH],temporary[MAX_PATH+32],text[400];
    if(!fps_narrow(ini,path,sizeof(path)))return 0;
    int n=snprintf(temporary,sizeof(temporary),"%.*s.%d.tmp",(int)(strlen(path)<MAX_PATH?strlen(path):MAX_PATH-1),path,(int)getpid());
    if(n<0 || (size_t)n>=sizeof(temporary))return 0;
    n=snprintf(text,sizeof(text),"[Graphics]\nFrameIntervalMs=%ls\nPreciseFrameLimit=%ls\nWaitForGpu=%ls\n",values[0],values[1],values[2]);
    if(n<0 || (size_t)n>=sizeof(text))return 0;
    FILE *f=fopen(temporary,"wb");
    if(!f)return 0;
    int ok=fwrite(text,1,(size_t)n,f)==(size_t)n && fflush(f)==0 && fsync(fileno(f))==0;
    if(fclose(f)!=0)ok=0;
    if(ok)ok=rename(temporary,path)==0;
    if(!ok)unlink(temporary);
    return ok;
}

static int fps_graphics_user_path_mac(const char *userFolder,wchar_t *ini) {
    char folder[MAX_PATH],path[MAX_PATH];
    ini[0]=0;
    if(!userFolder || !userFolder[0])return 0;
    int n=snprintf(folder,sizeof(folder),"%s/coe5se",userFolder);
    if(n<0 || (size_t)n>=sizeof(folder))return 0;
    struct stat info;
    if(mkdir(folder,0777)!=0 && (stat(folder,&info)!=0 || !S_ISDIR(info.st_mode)))return 0;
    n=snprintf(path,sizeof(path),"%s/CoE5SE-Graphics.ini",folder);
    if(n<0 || (size_t)n>=sizeof(path) || !fps_widen(path,ini,MAX_PATH)) {ini[0]=0;return 0;}
    return 1;
}

static const wchar_t *GetCommandLineW(void) {return L"";}
static wchar_t **CommandLineToArgvW(const wchar_t *unused,int *count) {
    (void)unused;
    int argc=*_NSGetArgc();
    char **argv=*_NSGetArgv();
    wchar_t **result=calloc((size_t)argc+1,sizeof(*result));
    if(!result)return NULL;
    for(int i=0;i<argc;i++) {
        size_t n=mbstowcs(NULL,argv[i],0);
        result[i]=n==(size_t)-1?NULL:calloc(n+1,sizeof(wchar_t));
        if(!result[i] || mbstowcs(result[i],argv[i],n+1)==(size_t)-1) {
            for(int j=0;j<=i;j++)free(result[j]);
            free(result);
            return NULL;
        }
    }
    *count=argc;
    return result;
}
static void LocalFree(wchar_t **argv) {
    if(!argv)return;
    for(int i=0;argv[i];i++)free(argv[i]);
    free(argv);
}

static int fps_gpu_wait_shadow;
static unsigned char *fps_gpu_wait_byte;
static int *fps_gpu_wait_bind(void) {
    fps_gpu_wait_byte=detail_base+RVA_FPS_GPU_WAIT;
    fps_gpu_wait_shadow=*fps_gpu_wait_byte;
    return &fps_gpu_wait_shadow;
}
static void fps_gpu_wait_commit(void) {
    if(fps_gpu_wait_byte && (fps_gpu_wait_shadow==0 || fps_gpu_wait_shadow==1))*fps_gpu_wait_byte=(unsigned char)fps_gpu_wait_shadow;
}
