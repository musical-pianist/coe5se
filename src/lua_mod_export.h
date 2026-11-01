#pragma once

// @platform-split clipboard_write: Windows writes CF_UNICODETEXT with OpenClipboard and SetClipboardData; Mac calls NSPasteboard through the Objective-C runtime
static int copyTextToClipboard(const char *text) {
#ifdef _WIN32
    int wide = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    if (wide <= 0) return 0;
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, (size_t)wide * sizeof(WCHAR));
    if (!memory) return 0;
    WCHAR *out = (WCHAR *)GlobalLock(memory);
    if (!out) {
        GlobalFree(memory);
        return 0;
    }
    MultiByteToWideChar(CP_UTF8, 0, text, -1, out, wide);
    GlobalUnlock(memory);
    if (!OpenClipboard(NULL)) {
        GlobalFree(memory);
        return 0;
    }
    EmptyClipboard();
    int ok = SetClipboardData(CF_UNICODETEXT, memory) != NULL;
    CloseClipboard();
    if (!ok) GlobalFree(memory);
    return ok;
#else
    void *(*getClass)(const char *) = (void *(*)(const char *))dlsym(RTLD_DEFAULT, "objc_getClass");
    void *(*selector)(const char *) = (void *(*)(const char *))dlsym(RTLD_DEFAULT, "sel_registerName");
    void *send = dlsym(RTLD_DEFAULT, "objc_msgSend");
    if (!getClass || !selector || !send) return 0;
    void *boardClass = getClass("NSPasteboard"), *stringClass = getClass("NSString");
    if (!boardClass || !stringClass) return 0;
    void *(*sendNoArgs)(void *, void *) = (void *(*)(void *, void *))send;
    void *(*sendText)(void *, void *, const char *) = (void *(*)(void *, void *, const char *))send;
    signed char (*sendTwo)(void *, void *, void *, void *) = (signed char (*)(void *, void *, void *, void *))send;
    void *board = sendNoArgs(boardClass, selector("generalPasteboard"));
    void *string = sendText(stringClass, selector("stringWithUTF8String:"), text);
    void *type = sendText(stringClass, selector("stringWithUTF8String:"), "public.utf8-plain-text");
    if (!board || !string || !type) return 0;
    sendNoArgs(board, selector("clearContents"));
    return sendTwo(board, selector("setString:forType:"), string, type) != 0;
#endif
}

static int lCopyText(lua_State *L) {
    LuaScript *s = checkLuaScript(L);
    size_t len;
    const char *text = luaL_checklstring(L, 1, &len);
    if (strlen(text) != len) {
        refuseLuaCall(s->mod, "coe5.copyText", "bad_string", "bytes=%zu", len);
        lua_pushboolean(L, 0);
        return 1;
    }
    if (!copyTextToClipboard(text)) {
        refuseLuaCall(s->mod, "coe5.copyText", "clipboard_refused", "bytes=%zu", len);
        lua_pushboolean(L, 0);
        return 1;
    }
    writeLog("lua_text_copied", "mod=%s bytes=%zu", s->mod, len);
    lua_pushboolean(L, 1);
    return 1;
}

#define NEW_MOD_NAME_MAX 40
#define NEW_MOD_BANNER_W 256
#define NEW_MOD_BANNER_H 64

static const char *newModNameProblem(const char *name) {
    size_t n = strlen(name);
    if (!n) return "no_name";
    if (n > NEW_MOD_NAME_MAX) return "name_too_long";
    if (name[0] == ' ' || name[n - 1] == ' ') return "bad_name";
    for (const char *c = name; *c; c++)
        if (!((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9') || *c == '_' || *c == ' '))
            return "bad_name";
    return NULL;
}

static int writePlainBanner(const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    unsigned char header[18] = {0, 0, 2};
    header[12] = NEW_MOD_BANNER_W & 0xff;
    header[13] = NEW_MOD_BANNER_W >> 8;
    header[14] = NEW_MOD_BANNER_H & 0xff;
    header[15] = NEW_MOD_BANNER_H >> 8;
    header[16] = 24;
    int ok = fwrite(header, sizeof header, 1, f) == 1;
    unsigned char pixel[3] = {70, 70, 70};
    for (int i = 0; ok && i < NEW_MOD_BANNER_W * NEW_MOD_BANNER_H; i++) ok = fwrite(pixel, sizeof pixel, 1, f) == 1;
    return fclose(f) == 0 && ok;
}

static int lSaveNewMod(lua_State *L) {
    LuaScript *s = checkLuaScript(L);
    const char *name = luaL_checkstring(L, 1);
    size_t len;
    const char *text = luaL_checklstring(L, 2, &len);
    const char *reason = newModNameProblem(name);
    if (!reason && strlen(text) != len) reason = "bad_string";
    char folder[MAX_PATH], c5m[MAX_PATH], banner[MAX_PATH];
    if (!reason && (!buildPath(folder, sizeof folder, "coe5.saveNewMod", "%s" PATH_SEP "%s", (const char *)GAME_PTR(RVA_MODS_DIR), name) ||
                    !buildPath(c5m, sizeof c5m, "coe5.saveNewMod", "%s" PATH_SEP "%s.c5m", folder, name) ||
                    !buildPath(banner, sizeof banner, "coe5.saveNewMod", "%s" PATH_SEP "banner.tga", folder)))
        reason = "path_too_long";
    if (!reason && GetFileAttributesA(folder) != INVALID_FILE_ATTRIBUTES) reason = "exists";
    if (!reason && !CreateDirectoryA(folder, NULL)) reason = "folder_failed";
    if (reason) {
        refuseLuaCall(s->mod, "coe5.saveNewMod", reason, "name=%s", name);
        lua_pushboolean(L, 0);
        return 1;
    }
    FILE *f = fopen(c5m, "wb");
    int ok = f && fprintf(f, "icon \"banner.tga\"\ndescription \"%s\"\n\n", name) > 0 && fwrite(text, 1, len, f) == len;
    if (f && fclose(f)) ok = 0;
    if (ok) ok = writePlainBanner(banner);
    if (!ok) {
        refuseLuaCall(s->mod, "coe5.saveNewMod", "write_failed", "name=%s path=%s", name, folder);
        lua_pushboolean(L, 0);
        return 1;
    }
    writeLog("lua_mod_saved", "mod=%s name=%s bytes=%zu", s->mod, name, len);
    lua_pushboolean(L, 1);
    return 1;
}
