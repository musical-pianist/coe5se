// ---- Texture array batching's default by graphics card ----------------------------------------------
// @platform-split graphics: Windows reads GL_VENDOR at the first menu frame; Mac reads no card and keeps the .c5se default.

#define BATCHING_SETTING     "smoother_map_rendering"
#define GRAPHICS_VENDOR_FILE "graphics_vendor.txt"

static char graphicsVendor[128];

static int batchingDefaultFor(const char *vendor) {
    return !(vendor && (strstr(vendor, "NVIDIA") || strstr(vendor, "Nvidia") || strstr(vendor, "nvidia")));
}

static int settingDefaultHere(const char *mod, const char *name, int declared) {
    if (_stricmp(mod, COE5SE_MOD) || _stricmp(name, BATCHING_SETTING) || !graphicsVendor[0]) return declared;
    return declared && batchingDefaultFor(graphicsVendor);
}

static void applyBatchingDefault(void) {
    ModSetting *s = findDeclaredSetting(COE5SE_MOD, BATCHING_SETTING);
    char pick[16];
    if (!s || s->rejected || !graphicsVendor[0] || batchingDefaultFor(graphicsVendor)) return;
    s->defaultValue = 0;
    if (savedPickText(COE5SE_MOD, BATCHING_SETTING, pick, sizeof pick)) return;
    s->value = s->launchValue = s->promptedValue = 0;
    refreshSettingReads();
}

static void applySavedGraphicsVendor(void) {
    char path[MAX_PATH];
    if (!userFileToRead(COE5SE_USER_DIR, GRAPHICS_VENDOR_FILE, path, sizeof path)) return;
    FILE *f = fopen(path, "rb");
    if (!f) return;
    size_t n = fread(graphicsVendor, 1, sizeof graphicsVendor - 1, f);
    fclose(f);
    graphicsVendor[n] = '\0';
    graphicsVendor[strcspn(graphicsVendor, "\r\n")] = '\0';
    applyBatchingDefault();
    writeLog("graphics_vendor_saved", "vendor=\"%.60s\" batching_default=%d", graphicsVendor, batchingDefaultFor(graphicsVendor));
}

static void noteGraphicsVendor(void) {
#ifdef _WIN32
    HMODULE gl = GetModuleHandleA("opengl32.dll");
    const char *(WINAPI *getString)(unsigned) =
        gl ? (const char *(WINAPI *)(unsigned))(void (*)(void))GetProcAddress(gl, "glGetString") : NULL;
    const char *vendor = getString ? getString(0x1f00) : NULL;
    if (!vendor || !vendor[0]) {
        writeLogWarning("graphics_vendor_unavailable", "reason=no_gl_vendor");
        return;
    }
    char path[MAX_PATH];
    int changed = strncmp(vendor, graphicsVendor, sizeof graphicsVendor - 1) != 0, saved = 0;
    snprintf(graphicsVendor, sizeof graphicsVendor, "%s", vendor);
    if (changed && userFileToWrite(COE5SE_USER_DIR, GRAPHICS_VENDOR_FILE, path, sizeof path)) {
        FILE *f = fopen(path, "wb");
        saved = f && fputs(graphicsVendor, f) >= 0;
        if (f) fclose(f);
    }
    applyBatchingDefault();
    writeLog("graphics_vendor", "vendor=\"%.60s\" batching_default=%d changed=%d saved=%d", graphicsVendor,
             batchingDefaultFor(graphicsVendor), changed, saved);
#endif
}

static void noteGraphicsVendorAtMenu(void) {
    if (featureOn(FEATURE_MAP_RENDERING)) noteGraphicsVendor();
}
