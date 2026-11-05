#pragma once
#include <stdlib.h>
#ifdef __APPLE__
#include <crt_externs.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#endif
#include <sys/stat.h>

#define OFFICIAL_MOD_CHECK_OUTPUT_FILE "official_mod_check_output.txt"
#define OFFICIAL_NO_LOBBY_ENV      "COE5SE_OFFICIAL_NO_LOBBY"

static int isOfficialWatchStart(void) {
    const char *value = getenv(OFFICIAL_WATCH_ENV);
    return value && value[0];
}

#ifdef __APPLE__
// @platform-split official_mod_check_gl: Mac's child also inserts the loader, whose constructor (coe5se_loader.c, makeModCheckGlContext) makes a current GL context with no window; Windows' needs none
static char **officialModCheckEnvironment(void) {
    Dl_info self;
    if (!dladdr((void *)isOfficialWatchStart, &self) || !self.dli_fname) return NULL;
    static char inserted[MAX_PATH * 2 + 64];
    char value[MAX_PATH * 2];
    if (!officialInsertLibraryValue(getenv("DYLD_INSERT_LIBRARIES"), self.dli_fname, value, sizeof value)) return NULL;
    snprintf(inserted, sizeof inserted, "DYLD_INSERT_LIBRARIES=%s", value);
    char **env = *_NSGetEnviron();
    int count = 0;
    while (env[count]) count++;
    char **out = calloc((size_t)count + 3, sizeof *out);
    if (!out) return NULL;
    int n = 0;
    for (int i = 0; i < count; i++)
        if (strncmp(env[i], "DYLD_INSERT_LIBRARIES=", 22)) out[n++] = env[i];
    out[n++] = inserted;
    out[n++] = (char *)OFFICIAL_MOD_CHECK_GL_ENV "=1";
    return out;
}
#endif

typedef struct {
    int running;
    unsigned long id;
#ifdef __APPLE__
    pid_t pid;
#else
    HANDLE process;
#endif
    long logStart;
    char output[MAX_PATH];
} OfficialWatchState;

static OfficialWatchState officialChild;
static const OfficialModCheckUi *officialModCheckUi;

// @platform-split official_mod_check_output: Windows reads the game's log.txt in the game folder, the game's only trace on that build; Mac redirects the game's stdout and stderr to a file
static int officialModCheckOutputPath(char *out, size_t size) {
#ifdef __APPLE__
    return coe5seUserFile(OFFICIAL_MOD_CHECK_OUTPUT_FILE, out, size);
#else
    return buildPath(out, size, __func__, "%s" USER_DIR_SEP "log.txt", gameDir);
#endif
}

static int startOfficialChild(char *const *args, char **environment, const char *commandLine, const char *outputFile, int quiet) {
    char exe[MAX_PATH];
    GetModuleFileNameA(NULL, exe, sizeof exe);
    struct stat st;
    officialChild.logStart = !stat(logPath, &st) ? (long)st.st_size : 0;
#ifdef __APPLE__
    (void)commandLine;
    (void)quiet;
    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    if (outputFile) {
        posix_spawn_file_actions_addopen(&actions, 1, outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        posix_spawn_file_actions_adddup2(&actions, 1, 2);
    }
    int error = posix_spawn(&officialChild.pid, exe, &actions, NULL, args, environment ? environment : *_NSGetEnviron());
    posix_spawn_file_actions_destroy(&actions);
    if (error) {
        writeLogError("official_child_started", "ok=0 reason=launch_failed error=%d", error);
        return 0;
    }
    officialChild.id = (unsigned long)officialChild.pid;
#else
    (void)args;
    (void)environment;
    (void)outputFile;
    char dir[MAX_PATH];
    snprintf(dir, sizeof dir, "%s", gameDir);
    static char cmd[8192];
    if (snprintf(cmd, sizeof cmd, "%s", commandLine) >= (int)sizeof cmd) {
        writeLogError("official_child_started", "ok=0 reason=command_line_too_long");
        return 0;
    }
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof si);
    si.cb = sizeof si;
    if (quiet) {
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
    }
    PROCESS_INFORMATION pi;
    if (!CreateProcessA(exe, cmd, NULL, NULL, FALSE, quiet ? CREATE_NO_WINDOW : 0, NULL, dir, &si, &pi)) {
        writeLogError("official_child_started", "ok=0 reason=launch_failed error=%lu", GetLastError());
        return 0;
    }
    CloseHandle(pi.hThread);
    officialChild.process = pi.hProcess;
    officialChild.id = pi.dwProcessId;
#endif
    officialChild.running = 1;
    writeLog("official_child_started", "ok=1 pid=%lu", officialChild.id);
    return 1;
}

static int officialChildRunning(void) {
    if (!officialChild.running) return 0;
    if (officialModCheckUi) officialModCheckUi->draw();
#ifdef __APPLE__
    int status;
    pid_t ended = waitpid(officialChild.pid, &status, WNOHANG);
    if (ended == officialChild.pid || (ended < 0)) {
        officialChild.running = 0;
        return 0;
    }
    usleep(100000);
    return 1;
#else
    if (WaitForSingleObject(officialChild.process, 100) == WAIT_TIMEOUT) return 1;
    CloseHandle(officialChild.process);
    officialChild.running = 0;
    return 0;
#endif
}

static void killOfficialChild(void) {
#ifdef __APPLE__
    kill(officialChild.pid, SIGKILL);
    waitpid(officialChild.pid, NULL, 0);
#else
    TerminateProcess(officialChild.process, 1);
    WaitForSingleObject(officialChild.process, 5000);
    CloseHandle(officialChild.process);
#endif
    officialChild.running = 0;
}

static long officialNowSeconds(void) {
#ifdef __APPLE__
    return (long)time(NULL);
#else
    return (long)(GetTickCount() / 1000);
#endif
}

static size_t readOfficialTail(const char *path, long start, char *out, size_t size) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    long at = start;
    if (at < 0) {
        fseek(f, 0, SEEK_END);
        long total = ftell(f);
        at = total > (long)size ? total - (long)size : 0;
    }
    fseek(f, at, SEEK_SET);
    size_t len = fread(out, 1, size, f);
    fclose(f);
    return len;
}

static int officialHasModCheckTexts(void *context) {
    (void)context;
    const char *noLobby = getenv(OFFICIAL_NO_LOBBY_ENV);
    if (noLobby && !strcmp(noLobby, "fail")) return 0;
    char exe[MAX_PATH];
    GetModuleFileNameA(NULL, exe, sizeof exe);
    static const char *const TEXTS[] = {OFFICIAL_MOD_CHECK_SWITCH, OFFICIAL_MOD_CHECK_END_TEXT, "*** %s", "Error in mod '%s' at line %d", "Parsing mod: %s (level %d)"};
    return officialFileHasTexts(exe, TEXTS, sizeof TEXTS / sizeof TEXTS[0]);
}

static int officialStartModCheck(void *context) {
    (void)context;
    if (!officialModCheckOutputPath(officialChild.output, sizeof officialChild.output)) return 0;
    remove(officialChild.output);
#ifdef __APPLE__
    static const char *args[64];
    int count = officialModCheckArgs(*_NSGetArgc(), *_NSGetArgv(), args, 63);
    args[count] = NULL;
    for (int i = 0; i < count; i++)
        if (officialArgIsNetwork(args[i])) return 0;
    char **environment = officialModCheckEnvironment();
    if (!environment) return 0;
    int started = startOfficialChild((char *const *)args, environment, NULL, officialChild.output, 1);
    free(environment);
    return started;
#else
    static char cmd[4096], arg[1024];
    char exe[MAX_PATH];
    GetModuleFileNameA(NULL, exe, sizeof exe);
    int used = snprintf(cmd, sizeof cmd, "\"%s\" " OFFICIAL_MOD_CHECK_SWITCH " -d", exe);
    const char *c = GetCommandLineA();
    for (int first = 1;; first = 0) {
        while (*c == ' ' || *c == '\t') c++;
        if (!*c) break;
        size_t n = 0;
        int quoted = 0;
        for (; *c && (quoted || (*c != ' ' && *c != '\t')); c++) {
            if (*c == '"') quoted = !quoted;
            else if (n + 1 < sizeof arg) arg[n++] = *c;
        }
        arg[n] = '\0';
        if (first || !officialModCheckKeepsArg(arg) || officialArgIsNetwork(arg)) continue;
        int more = snprintf(cmd + used, sizeof cmd - (size_t)used, " \"%s\"", arg);
        if (more < 0 || (size_t)more >= sizeof cmd - (size_t)used) return 0;
        used += more;
    }
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    return startOfficialChild(NULL, NULL, cmd, NULL, 1);
#endif
}

static int officialStartLobby(void *context) {
    const char *noLobby = getenv(OFFICIAL_NO_LOBBY_ENV);
    if (noLobby && noLobby[0]) {
        writeLog("official_lobby_stand_in", "reason=test");
        return officialStartModCheck(context);
    }
#ifdef __APPLE__
    int count = *_NSGetArgc();
    char **argv = *_NSGetArgv(), **args = calloc((size_t)count + 2, sizeof *args);
    if (!args) return 0;
    memcpy(args, argv, (size_t)count * sizeof *args);
    args[count] = (char *)LOBBY_CLIENT_SWITCH;
    int ok = startOfficialChild(args, NULL, NULL, NULL, 0);
    free(args);
    return ok;
#else
    static char cmd[4096];
    SetErrorMode(0);
    if (snprintf(cmd, sizeof cmd, "%s " LOBBY_CLIENT_SWITCH, GetCommandLineA()) >= (int)sizeof cmd) return 0;
    return startOfficialChild(NULL, NULL, cmd, NULL, 0);
#endif
}

static void officialWaitForStub(void *context) {
    (void)context;
    static char text[256 * 1024];
    long limit = officialNowSeconds() + OFFICIAL_STUB_SECONDS;
    while (officialChildRunning() && officialNowSeconds() < limit) {
        size_t len = readOfficialTail(logPath, officialChild.logStart, text, sizeof text);
        if (officialLogShowsStubPassed(text, len, officialChild.id)) {
            writeLog("official_child_stub_passed", "pid=%lu", officialChild.id);
            return;
        }
    }
    writeLogWarning("official_child_stub_passed", "ok=0 pid=%lu reason=%s", officialChild.id, officialChild.running ? "time_limit" : "child_ended");
}

static int officialWaitModCheck(void *context) {
    (void)context;
    long limit = officialNowSeconds() + OFFICIAL_MOD_CHECK_SECONDS;
    int ended = 1;
    while (ended && officialChildRunning()) ended = officialNowSeconds() < limit;
    if (!ended) killOfficialChild();
    writeLog("official_mod_check_ended", "killed=%d limit_seconds=%d", !ended, OFFICIAL_MOD_CHECK_SECONDS);
    return ended;
}

static void officialWaitLobby(void *context) {
    (void)context;
    while (officialChildRunning()) {
    }
}

static size_t officialReadOutput(void *context, char *out, size_t size) {
    (void)context;
    return readOfficialTail(officialChild.output, -1, out, size);
}

static int officialAsk(void *context, const char *text) {
    (void)context;
    return officialModCheckUi->ask(text);
}

static void officialTell(void *context, const char *text) {
    (void)context;
    officialModCheckUi->tell(text);
}

static void officialStartNormalGame(void *context) {
    (void)context;
#ifdef __APPLE__
    startOfficialChild(*_NSGetArgv(), NULL, NULL, NULL, 0);
#else
    SetErrorMode(0);
    startOfficialChild(NULL, NULL, GetCommandLineA(), NULL, 0);
#endif
}

static int officialWatchPathsFill(char *config, char *backup, char *session, char *done, size_t size, OfficialWatchPaths *paths) {
    if (!buildPath(config, size, __func__, "%s" USER_DIR_SEP CONFIG_FILE, userDir()) || !coe5seUserFile(OFFICIAL_CONFIG_BACKUP_FILE, backup, size) ||
        !coe5seUserFile(OFFICIAL_SESSION_FILE, session, size) || !coe5seUserFile(AUTO_ENABLE_DONE_FILE, done, size))
        return 0;
    paths->config = config;
    paths->backup = backup;
    paths->session = session;
    paths->done = done;
    paths->doneText = AUTO_ENABLE_DONE_TEXT;
    return 1;
}

static const OfficialWatchOps OFFICIAL_WATCH_OPS = {NULL,
                                                    officialHasModCheckTexts,
                                                    officialStartModCheck,
                                                    officialStartLobby,
                                                    officialWaitForStub,
                                                    officialWaitModCheck,
                                                    officialWaitLobby,
                                                    officialReadOutput,
                                                    officialAsk,
                                                    officialTell,
                                                    officialStartNormalGame};

__declspec(dllexport) int coe5seRunModCheck(const OfficialModCheckUi *ui) {
    char config[MAX_PATH], backup[MAX_PATH], session[MAX_PATH], done[MAX_PATH];
    OfficialWatchPaths paths;
    if (!officialWatchPathsFill(config, backup, session, done, MAX_PATH, &paths)) return 0;
    officialModCheckUi = ui;
#ifndef __APPLE__
    UINT errorMode = SetErrorMode(0);
#endif
    int restart = officialModCheckRun(&OFFICIAL_WATCH_OPS, &paths);
#ifndef __APPLE__
    SetErrorMode(errorMode);
#endif
    officialModCheckUi = NULL;
    return restart;
}

static void runOfficialWatch(void) {
    char config[MAX_PATH], backup[MAX_PATH], session[MAX_PATH], done[MAX_PATH];
    OfficialWatchPaths paths;
    officialSetEnvironment(OFFICIAL_WATCH_ENV, NULL);
    const char *noLobby = getenv(OFFICIAL_NO_LOBBY_ENV);
    writeLog("official_watch_started", "no_lobby=%d", noLobby && noLobby[0]);
    if (officialWatchPathsFill(config, backup, session, done, MAX_PATH, &paths)) officialLobbyRun(&OFFICIAL_WATCH_OPS, &paths);
    writeLog("official_watch_ended", "at=exit");
#ifdef __APPLE__
    exit(0);
#else
    ExitProcess(0);
#endif
}
