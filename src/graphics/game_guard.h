#ifdef _WIN32
#include "../stub_hash.h"
// Hash a loaded module's file using the same pinned-build check as the game.
static int fps_module_hash(HMODULE module,unsigned char digest[32]) {
    wchar_t path[32768];
    unsigned char buffer[65536];
    DWORD n = GetModuleFileNameW(module, path, (DWORD)(sizeof(path)/sizeof(path[0])));
    if (!n || n >= sizeof(path)/sizeof(path[0])) return 0;
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return 0;
    BCRYPT_ALG_HANDLE algorithm = NULL;
    BCRYPT_HASH_HANDLE hash = NULL;
    DWORD object_size = 0, received = 0, read = 0;
    unsigned char *object = NULL;
    int ok = 0;
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0) < 0) goto done;
    if (BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, (PUCHAR)&object_size,
                          sizeof(object_size), &received, 0) < 0 || received != sizeof(object_size)) goto done;
    object = (unsigned char *)HeapAlloc(GetProcessHeap(), 0, object_size);
    if (!object || BCryptCreateHash(algorithm, &hash, object, object_size, NULL, 0, 0) < 0) goto done;
    for (;;) {
        if (!ReadFile(file, buffer, sizeof(buffer), &read, NULL)) goto done;
        if (!read) break;
        if (BCryptHashData(hash, buffer, read, 0) < 0) goto done;
    }
    ok = BCryptFinishHash(hash, digest, 32, 0) >= 0;
done:
    if (hash) BCryptDestroyHash(hash);
    if (object) HeapFree(GetProcessHeap(), 0, object);
    if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
    CloseHandle(file);
    return ok;
}
static int game_hash_matches(void) {
    unsigned char digest[32];
    return fps_module_hash(NULL,digest) && !memcmp(digest,expected_sha256,sizeof digest);
}

// The optimized filters implement the Windows system GLU or its shipped forwarding stub.
static int fps_standard_glu(void *entry) {
    HMODULE owner=NULL;
    if(!entry || !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                     (LPCWSTR)entry,&owner))return 0;
    wchar_t path[32768];
    UINT length=GetSystemDirectoryW(path,32768);
    if(length && length<32750){
        wcscat(path,L"\\glu32.dll");
        if(owner==GetModuleHandleW(path))return 1;
    }
    unsigned char digest[32];
    if(!fps_module_hash(owner,digest))return 0;
    char hex[65];
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    return !strcmp(hex,SHIPPED_GLU32_SHA256);
}
#else
#include <CommonCrypto/CommonDigest.h>
static int game_hash_matches(void) {
    char path[4096];
    uint32_t length=sizeof(path);
    if(_NSGetExecutablePath(path,&length)!=0)return 0;
    FILE *file=fopen(path,"rb");
    if(!file)return 0;
    static unsigned char buffer[65536];
    unsigned char digest[CC_SHA256_DIGEST_LENGTH];
    CC_SHA256_CTX context;
    CC_SHA256_Init(&context);
    int ok=1;
    for(;;) {
        size_t read=fread(buffer,1,sizeof(buffer),file);
        CC_SHA256_Update(&context,buffer,(CC_LONG)read);
        if(read<sizeof(buffer)){ok=!ferror(file);break;}
    }
    fclose(file);
    CC_SHA256_Final(digest,&context);
    return ok && memcmp(digest,expected_sha256,sizeof(digest))==0;
}
#endif
