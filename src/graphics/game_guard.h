#ifdef _WIN32
static int game_hash_matches(void) {
    wchar_t path[32768];
    unsigned char buffer[65536], digest[32];
    DWORD n = GetModuleFileNameW(NULL, path, (DWORD)(sizeof(path)/sizeof(path[0])));
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
    if (BCryptFinishHash(hash, digest, sizeof(digest), 0) < 0) goto done;
    ok = memcmp(digest, expected_sha256, sizeof(digest)) == 0;
done:
    if (hash) BCryptDestroyHash(hash);
    if (object) HeapFree(GetProcessHeap(), 0, object);
    if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
    CloseHandle(file);
    return ok;
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
