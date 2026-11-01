// libFuzzer harness for src/coe5config.h: the parser for the game's coe5config file,
// which CoE5SE reads to see whether a mod is enabled and edits to enable one.
// Input: 1 byte mod name length (0-63), the mod name, 2 bytes layout file length
// (little endian), the layout file (coe5config_layout.txt), then the coe5config bytes.
// Built by .clusterfuzzlite/build.sh; run locally with:
//   clang -g -fsanitize=fuzzer,address,undefined fuzz/fuzz_coe5config.c -o fuzz_coe5config && ./fuzz_coe5config fuzz/corpus/coe5config
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH 260
#define writeLog(event, ...) ((void)0)
#include "../src/build_path.h"
#include "../src/coe5config.h"

static char configPath[64], layoutPath[64];

static void writeTestFile(const char *path, const uint8_t *data, size_t len) {
    FILE *f = fopen(path, "wb");
    if (!f) abort();
    if (len && fwrite(data, 1, len, f) != len) abort();
    fclose(f);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (!configPath[0]) {
        snprintf(configPath, sizeof configPath, "/tmp/fuzz_coe5config_%d", (int)getpid());
        snprintf(layoutPath, sizeof layoutPath, "/tmp/fuzz_coe5config_layout_%d", (int)getpid());
    }
    if (size < 1) return 0;
    size_t modLen = data[0] & 63;
    if (size < 1 + modLen + 2) return 0;
    char mod[64];
    memcpy(mod, data + 1, modLen);
    mod[modLen] = '\0';
    const uint8_t *rest = data + 1 + modLen;
    size_t restLen = size - 1 - modLen;
    modLen = strlen(mod); // a NUL byte ends the name early, as it does for CoE5SE
    size_t layoutLen = rest[0] | (size_t)rest[1] << 8;
    rest += 2;
    restLen -= 2;
    if (layoutLen > restLen) layoutLen = restLen;
    const uint8_t *layout = rest;
    const uint8_t *config = rest + layoutLen;
    size_t configLen = restLen - layoutLen;
    if (configLen > CONFIG_MAX) return 0;

    // Exact-size copies, so the sanitizer sees any read past the end.
    unsigned char *exact = malloc(configLen ? configLen : 1);
    memcpy(exact, config, configLen);
    configDataHasModName(exact, configLen, mod);

    size_t cap = configLen + modLen + 1, len = configLen;
    unsigned char *grown = malloc(cap);
    memcpy(grown, config, configLen);
    if (!addModToConfig(grown, &len, cap, mod)) {
        if (len != configLen + modLen + 1) abort();
        // The mod just added must read back as enabled.
        if (modLen && !configDataHasModName(grown, len, mod)) abort();
    }
    free(grown);
    free(exact);

    FILE *f = fmemopen((void *)config, configLen ? configLen : 1, "rb");
    if (f) {
        char name[16];
        while (configLen && readConfigModName(f, name, sizeof name)) {}
        fclose(f);
    }

    writeTestFile(configPath, config, configLen);
    writeTestFile(layoutPath, layout, layoutLen);
    const char *why;
    int version = configLen >= 4 ? (int)readLittleEndian32(config) : 0;
    configLayoutListStart(layoutPath, version, &why);
    configEnablesMod(configPath, mod, layoutPath);
    return 0;
}
