#pragma once

#define PERSONAL_LIST_PREFIX      "CoE5SE mods:"
#define PERSONAL_LIST_SEND_MS     2000
#define PERSONAL_LIST_SIZE        (NET_LOBBY_MESSAGE_MAX - (int)sizeof PERSONAL_LIST_PREFIX - 17)
#define PERSONAL_LIST_LABEL       "Personal mods: "
#define PERSONAL_LIST_SHOWN_MAX   54
#define PARTICIPANTS_NAME_COLUMN  25.0f
#define CONTROLLER_ANOTHER_PLAYER 0
#define CONTROLLER_ME             5

static char personalModLists[PLAYER_SLOTS_MOVED][PERSONAL_LIST_SIZE];
static char ownPersonalModList[PERSONAL_LIST_SIZE];
static int participantRowsDrawn;
static int personalListSentAt;
static u64 personalListSentSlots;
static int personalListSentOnce;

static int personalListClockNow(void) { return GAME_FN(int (*)(void), RVA_TICKS_MS)(); }
static int (*personalListClock)(void) = personalListClockNow;

static int participantSlotShown(short controller) {
    return controller != -1 && controller != 2 && controller != 3 && controller != 6;
}

static void buildPersonalModList(char *out, size_t size) {
    size_t used = 0;
    int left = 0;
    out[0] = '\0';
    for (int m = 0; m <= G32(RVA_MOD_LAST_INDEX); m++) {
        if (!modEnabled(m) || !modIsPersonal(modName(m))) continue;
        char name[MOD_NAME_MAX];
        modDisplayName(modName(m), name, sizeof name);
        for (char *c = name; *c; c++)
            if ((unsigned char)*c < 0x20 || *c == 0x7f) *c = '?';
        size_t length = strlen(name);
        if (left || used + length + 2 + sizeof ", +999" > size) {
            left++;
            continue;
        }
        used += (size_t)snprintf(out + used, size - used, "%s%s", used ? ", " : "", name);
    }
    if (left) snprintf(out + used, size - used, "%s+%d", used ? ", " : "", left);
}

static u64 ownParticipantSlots(void) {
    u64 slots = 0;
    for (int i = 0; i < playerSlotCount(); i++)
        if (*(short *)playerSlotAddress(RVA_SLOT_CONTROLLER, i, 2) == CONTROLLER_ME) slots |= (u64)1 << i;
    return slots;
}

static void readPersonalModList(const char *body) {
    char *end = NULL;
    u64 slots = strtoull(body, &end, 16);
    if (end != body + 16 || *end != ':' || strlen(end + 1) >= PERSONAL_LIST_SIZE) {
        writeLogWarning("personal_mod_list_dropped", "reason=bad_text");
        return;
    }
    char list[PERSONAL_LIST_SIZE];
    size_t n = 0;
    for (const char *c = end + 1; *c; c++) list[n++] = (unsigned char)*c < 0x20 || *c == 0x7f ? '?' : *c;
    list[n] = '\0';
    for (int i = 0; i < PLAYER_SLOTS_MOVED; i++) {
        if (!(slots >> i & 1) || !strcmp(personalModLists[i], list)) continue;
        memcpy(personalModLists[i], list, n + 1);
        writeLog("personal_mod_list_read", "slot=%d list=%s", i, list);
    }
}

static const char *participantRowList(int row) {
    for (int i = 0, seen = 0; i < playerSlotCount(); i++) {
        short controller = *(short *)playerSlotAddress(RVA_SLOT_CONTROLLER, i, 2);
        if (!participantSlotShown(controller) || seen++ != row) continue;
        return controller == CONTROLLER_ME ? ownPersonalModList : controller == CONTROLLER_ANOTHER_PLAYER ? personalModLists[i] : NULL;
    }
    return NULL;
}

static void sendPersonalModListIfDue(void) {
    if (!runsNetworkChecks() || !isNetworkClient()) return;
    u64 slots = ownParticipantSlots();
    int now = personalListClock();
    if (!slots || (personalListSentOnce && slots == personalListSentSlots && now - personalListSentAt < PERSONAL_LIST_SEND_MS)) return;
    buildPersonalModList(ownPersonalModList, sizeof ownPersonalModList);
    char message[NET_LOBBY_MESSAGE_MAX];
    snprintf(message, sizeof message, PERSONAL_LIST_PREFIX "%016llx:%s", (unsigned long long)slots, ownPersonalModList);
    sendNetworkCheckText(message);
    if (!personalListSentOnce || slots != personalListSentSlots) writeLog("personal_mod_list_sent", "slots=%llx list=%s", (unsigned long long)slots, ownPersonalModList);
    personalListSentOnce = 1;
    personalListSentAt = now;
    personalListSentSlots = slots;
}

static void personalModRowText(const char *list, char *out, size_t size) {
    if (strlen(list) > PERSONAL_LIST_SHOWN_MAX) snprintf(out, size, PERSONAL_LIST_LABEL "%.*s...", PERSONAL_LIST_SHOWN_MAX - 3, list);
    else snprintf(out, size, PERSONAL_LIST_LABEL "%s", list);
}

static void setParticipantRowOpen(int open) {
#ifdef __APPLE__
    rowBegin(open);
#else
    GAME_FN(void (*)(int), RVA_ROW_BEGIN)(!open);
#endif
}

static void participantsRowStripeHook(int stripe, int width) {
    const char *list = participantRowList(participantRowsDrawn++);
    if (list && list[0]) {
        char text[sizeof PERSONAL_LIST_LABEL + PERSONAL_LIST_SHOWN_MAX];
        personalModRowText(list, text, sizeof text);
        GAME_FN(void (*)(int, int), RVA_ROW_STRIPE)(stripe, width);
        columnX((float)(int)(GF(RVA_UI_TEXT_SCALE) * PARTICIPANTS_NAME_COLUMN));
        GAME_FN(void (*)(int, float, float, float, float, FMT_ARGS), RVA_BUTTON_CORE)(0, -1.0f, -1.0f, -1.0f, 0.6f, "%s", text);
        GAME_FN(unsigned long long (*)(FMT_ARGS), RVA_TOOLTIP)("%s", list);
        setParticipantRowOpen(0);
        setParticipantRowOpen(1);
    }
    GAME_FN(void (*)(int, int), RVA_ROW_STRIPE)(stripe, width);
}

static int participantsWaitHook(int a, int mode) {
    participantRowsDrawn = 0;
    sendPersonalModListIfDue();
    return GAME_FN(int (*)(int, int), RVA_WAIT_FOR_CLICK)(a, mode);
}

static void participantsLobbyTrace(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    if (strncmp(format, "lobby", 5)) {
        int type = va_arg(ap, int);
        va_end(ap);
        GAME_FN(GameTraceFn, RVA_TRACE)(format, type);
        return;
    }
    char *message = (char *)GAME_PTR(RVA_LOBBY_MESSAGE);
    const char *body = networkCheckBody(message, PERSONAL_LIST_PREFIX);
    va_end(ap);
    if (!body) {
        GAME_FN(GameTraceFn, RVA_TRACE)(format, message);
        return;
    }
    readPersonalModList(body);
    message[0] = '\0';
}

#define PERSONAL_LIST_HOOKS 3
#ifndef __APPLE__
static int installPersonalListHooks(void) {
    unsigned char *stubs = allocNear(gameBase + RVA_TEXT_SECTION);
    if (!stubs) {
        writeLog("personal_mod_list_hooks_skipped", "reason=no_near_page");
        return 0;
    }
    makePagesReadExecute(stubs, 0x1000);
    return redirectCall(RVA_PARTICIPANTS_ROW_STRIPE_CALL, RVA_ROW_STRIPE, stubs, (void *)participantsRowStripeHook, "participants_row_stripe", NULL) +
           redirectCall(RVA_PARTICIPANTS_WAIT_CALL, RVA_WAIT_FOR_CLICK, stubs + NETWORK_CHECK_STUB_SIZE, (void *)participantsWaitHook, "participants_wait", NULL) +
           redirectCall(RVA_PARTICIPANTS_LOBBY_TRACE, RVA_TRACE, stubs + 2 * NETWORK_CHECK_STUB_SIZE, (void *)participantsLobbyTrace, "participants_lobby_trace", NULL);
}
#else
static int installPersonalListHooks(void) {
    return pointMissingModsCall(RVA_PARTICIPANTS_ROW_STRIPE_CALL, RVA_ROW_STRIPE, (void *)participantsRowStripeHook, "participants_row_stripe") +
           pointMissingModsCall(RVA_PARTICIPANTS_WAIT_CALL, RVA_WAIT_FOR_CLICK, (void *)participantsWaitHook, "participants_wait") +
           pointMissingModsCall(RVA_PARTICIPANTS_LOBBY_TRACE, RVA_TRACE, (void *)participantsLobbyTrace, "participants_lobby_trace");
}
#endif

static void personalModListsInit(void) {
    writeInstallLog("personal_mod_list_hooks_installed", installPersonalListHooks(), PERSONAL_LIST_HOOKS);
}
