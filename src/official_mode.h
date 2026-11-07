#pragma once
#include <string.h>
#ifdef __APPLE__
#include <crt_externs.h>
#endif

#define LOBBY_CLIENT_SWITCH "--lobbyclient"
#define OFFICIAL_WATCH_ENV "COE5SE_OFFICIAL_WATCH"
#define OFFICIAL_MOD_CHECK_GL_ENV "COE5SE_MOD_CHECK_GL"

typedef struct {
    int (*ask)(const char *text);
    void (*tell)(const char *text);
    void (*draw)(void);
} OfficialModCheckUi;

__attribute__((unused)) static int commandLineHasArg(const char *line, const char *word) {
    char arg[64];
    size_t wordLength = strlen(word);
    if (!line || wordLength >= sizeof arg) return 0;
    const char *c = line;
    for (;;) {
        while (*c == ' ' || *c == '\t') c++;
        if (!*c) return 0;
        size_t n = 0;
        int quoted = 0, tooLong = 0;
        for (; *c && (quoted || (*c != ' ' && *c != '\t')); c++) {
            if (*c == '"') {
                quoted = !quoted;
                continue;
            }
            if (n + 1 < sizeof arg) arg[n++] = *c;
            else tooLong = 1;
        }
        arg[n] = '\0';
        if (!tooLong && n == wordLength && !memcmp(arg, word, n)) return 1;
    }
}

__attribute__((unused)) static int argsHaveArg(int count, char *const *args, const char *word) {
    for (int i = 0; args && i < count; i++)
        if (args[i] && !strcmp(args[i], word)) return 1;
    return 0;
}

static int hasLobbyClientSwitch(void) {
#ifdef __APPLE__
    return argsHaveArg(*_NSGetArgc(), *_NSGetArgv(), LOBBY_CLIENT_SWITCH);
#else
    return commandLineHasArg(GetCommandLineA(), LOBBY_CLIENT_SWITCH);
#endif
}

static int isOfficialLaunch(void) { return hasLobbyClientSwitch(); }
