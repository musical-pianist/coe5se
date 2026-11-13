#pragma once
#include "jazzy_api.h"

int coe5seGraphicsInit(const PluginApi *api, const char *userFolder);

#ifdef __APPLE__
typedef struct {
    int (*pointCall)(unsigned callRva, unsigned targetRva, void *hook, const char *name);
    int (*makeDataWritable)(void *address, size_t size);
} GraphicsMacHost;

void coe5seGraphicsMacHost(const GraphicsMacHost *host);
#endif
