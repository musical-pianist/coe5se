// ---- Turn-start player conditions ----
// @platform-split player-rule-cache: the numeric argument call is mapped on Windows 5.43.
static long long (*nextPlayerNumber)(const char *, int);
static _Thread_local int playerNumberActive;
static _Thread_local char playerNumberKey[48];
static _Thread_local long long playerNumberValue;

// Accept only the game's short decimal player literals.
static int plainPlayerNumber(const char *s) {
    if (!strncmp(s, "+player", 7) && (s[7] == ' ' || s[7] == '\t')) s += 7;
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '-') s++;
    const char *digits = s;
    while (*s >= '0' && *s <= '9') s++;
    if (s == digits || s - digits > 2) return 0;
    while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') s++;
    return !*s;
}

// Cache the parser result for one unchanged line within this pass.
static long long readPlayerNumber(const char *line, int index) {
    if (!playerNumberActive || !line || index || nextPlayerNumber != GAME_FN(long long (*)(const char *,int), RVA_EVENT_INT_ARG))
        return nextPlayerNumber(line, index);
    if (*playerNumberKey && !strcmp(playerNumberKey, line)) return playerNumberValue;
    long long value = nextPlayerNumber(line, index);
    size_t size = strnlen(line, sizeof playerNumberKey);
    playerNumberKey[0] = 0;
    if (size < sizeof playerNumberKey && plainPlayerNumber(line)) {
        memcpy(playerNumberKey, line, size + 1);
        playerNumberValue = value;
    }
    return value;
}

// Install the private reader only when selected at launch.
static void initPlayerNumberCache(int passInstalled) {
    const int *setting = api->setting("cache_player_arguments");
    if (setting && *setting) {
        if (!passInstalled) {
            writeLog("player_rule_cache", "installed=%d reason=mods_run_unavailable", 0);
            return;
        }
        int installed = wrapSite(SITE_TURN_RULE_PLAYER_NUMBER, (void *)readPlayerNumber, (void **)&nextPlayerNumber);
        writeLog("player_rule_cache", "installed=%d", installed);
    }
}
