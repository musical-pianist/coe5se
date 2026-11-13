// @platform-split item_slot_replay: Windows hooks RVA_SAVE_BATTLE_REPLAY and RVA_LOAD_BATTLE_REPLAY at their entries; Mac inlines both, so the writer's fclose call is redirected and the replay is read from tmp_shom.vcr
#pragma once

#define REPLAY_ITEMS_TAG      SAVE_TAG('C', '5', 'R', 'I')
#define REPLAY_ITEMS_VERSION  1
#define REPLAY_ITEMS_TRAILER  8
#define REPLAY_HEADER_BYTES   22
#define REPLAY_FILE_MAX       (64 << 20)
#define REPLAY_VIEW_FILE      "tmp_shom.vcr"

typedef struct { int unit, position, item; } ReplayItem;
static ReplayItem *replayItems;
static int replayItemCount;
static int replayItemsRead;
static int *replayGivenUnits;
static int replayGivenCount, replayGivenCapacity;
#ifndef __APPLE__
static const char *replayLoadPath;
#endif

static int replayWritesUnit(int unit) {
    size_t at = (size_t)unit * UNIT_SIZE;
    return G32(RVA_UNIT_X + at) == G32(RVA_BATTLE_X) && G32(RVA_UNIT_X + 4 + at) == G32(RVA_BATTLE_Y) && (short)G16(RVA_UNIT_HP + at) > 0;
}

static int buildReplayItems(SaveBuffer *out) {
    if (!anyExtraItems()) return 0;
    int entries = 0, last = G32(RVA_UNIT_COUNT);
    for (int u = 0; u <= last && u < UNIT_TABLE_MAX; u++)
        if (unitExtraItemCount(u) && replayWritesUnit(u)) entries += unitExtraItemCount(u);
    if (!entries) return 0;
    SaveBuffer payload = {0};
    saveBufferInt(&payload, REPLAY_ITEMS_VERSION);
    saveBufferInt(&payload, entries);
    for (int u = 0; u <= last && u < UNIT_TABLE_MAX; u++) {
        if (!unitExtraItemCount(u) || !replayWritesUnit(u)) continue;
        for (int p = UNIT_ITEM_SLOTS; p < ITEM_POSITION_MAX; p++) {
            int item = unitItemAt(u, p);
            if (item < 0) continue;
            saveBufferInt(&payload, u);
            saveBufferInt(&payload, p);
            saveBufferString(&payload, (const char *)(tableAddress(TABLE_ITEM) + (size_t)item * ITEM_SIZE));
        }
    }
    saveBufferAdd(out, payload.bytes, payload.size);
    saveBufferInt(out, (int)payload.size);
    saveBufferInt(out, REPLAY_ITEMS_TAG);
    int failed = payload.failed || out->failed;
    free(payload.bytes);
    if (failed) {
        writeLogError("item_slots_replay_written", "ok=0 reason=out_of_memory_or_too_big entries=%d", entries);
        return 0;
    }
    writeLog("item_slots_replay_written", "entries=%d bytes=%zu", entries, out->size);
    return entries;
}

static int writeReplayItems(FILE *file) {
    SaveBuffer bytes = {0};
    int entries = buildReplayItems(&bytes);
    if (entries && fwrite(bytes.bytes, 1, bytes.size, file) != bytes.size) {
        writeLogError("item_slots_replay_written", "ok=0 reason=write_failed entries=%d", entries);
        entries = 0;
    }
    free(bytes.bytes);
    return entries;
}

static void forgetReplayItems(void) {
    free(replayItems);
    replayItems = NULL;
    replayItemCount = 0;
}

static int readReplayItems(const unsigned char *file, size_t size) {
    forgetReplayItems();
    if (size < REPLAY_HEADER_BYTES + REPLAY_ITEMS_TRAILER) return 0;
    SaveCursor end = {file + size - REPLAY_ITEMS_TRAILER, file + size, 0};
    int payloadSize = saveCursorInt(&end), tag = saveCursorInt(&end);
    if (tag != REPLAY_ITEMS_TAG) return 0;
    if (payloadSize < 8 || (size_t)payloadSize > size - REPLAY_HEADER_BYTES - REPLAY_ITEMS_TRAILER) {
        writeLogWarning("item_slots_replay_read", "reason=bad_size bytes=%d file=%zu", payloadSize, size);
        return -1;
    }
    SaveCursor c = {file + size - REPLAY_ITEMS_TRAILER - (size_t)payloadSize, file + size - REPLAY_ITEMS_TRAILER, 0};
    int version = saveCursorInt(&c), entries = saveCursorInt(&c);
    if (version != REPLAY_ITEMS_VERSION || entries < 0 || entries > payloadSize / 9) {
        writeLogWarning("item_slots_replay_read", "reason=%s version=%d entries=%d", version != REPLAY_ITEMS_VERSION ? "other_version" : "bad_entries",
                        version, entries);
        return -1;
    }
    replayItems = entries ? malloc((size_t)entries * sizeof replayItems[0]) : NULL;
    if (entries && !replayItems) {
        writeLogError("item_slots_replay_read", "reason=out_of_memory entries=%d", entries);
        return -1;
    }
    int skipped = 0, missing = 0;
    for (int i = 0; i < entries; i++) {
        int unit = saveCursorInt(&c), position = saveCursorInt(&c);
        char name[SAVE_STRING_MAX + 1];
        saveCursorString(&c, name, sizeof name);
        if (c.failed) break;
        if (unit < 0 || unit >= UNIT_TABLE_MAX || position < UNIT_ITEM_SLOTS || position >= ITEM_POSITION_MAX) {
            skipped++;
            continue;
        }
        int item = itemSlotsItemNamed(name);
        if (item < 0) {
            missing++;
            continue;
        }
        replayItems[replayItemCount++] = (ReplayItem){unit, position, item};
    }
    if (c.failed) {
        writeLogWarning("item_slots_replay_read", "reason=bad_entries read=%d entries=%d", replayItemCount, entries);
        forgetReplayItems();
        return -1;
    }
    if (skipped || missing)
        writeLogWarning("item_slots_replay_read", "reason=bad_entries read=%d skipped=%d missing=%d", replayItemCount, skipped, missing);
    else writeLog("item_slots_replay_read", "read=%d", replayItemCount);
    return replayItemCount;
}

static int replayFilePath(char *out, size_t size) {
#ifndef __APPLE__
    if (replayLoadPath) return buildPath(out, size, __func__, "%s", replayLoadPath);
#endif
    return buildPath(out, size, __func__, "%s/" REPLAY_VIEW_FILE, (const char *)GAME_PTR(RVA_TEMP_DIR));
}

static void readReplayFileItems(void) {
    replayItemsRead = 1;
    char path[2600];
    if (!replayFilePath(path, sizeof path)) return;
    FILE *file = fopen(path, "rb");
    if (!file) {
        writeLogWarning("item_slots_replay_read", "reason=no_file");
        return;
    }
    unsigned char *bytes = NULL;
    long size = fseek(file, 0, SEEK_END) == 0 ? ftell(file) : -1;
    if (size >= 0 && size <= REPLAY_FILE_MAX && fseek(file, 0, SEEK_SET) == 0 && (bytes = malloc((size_t)size + 1)) &&
        fread(bytes, 1, (size_t)size, file) == (size_t)size)
        readReplayItems(bytes, (size_t)size);
    else writeLogWarning("item_slots_replay_read", "reason=unreadable bytes=%ld", size);
    free(bytes);
    fclose(file);
}

static int replayLoadingUnits(void) { return *(const unsigned char *)GAME_PTR(RVA_BATTLE_IS_REPLAY) != 0; }

static void giveReplayUnitItems(int unit) {
    if (!replayItemsRead) readReplayFileItems();
    int given = 0;
    for (int i = 0; i < replayItemCount; i++)
        if (replayItems[i].unit == unit) given += setUnitItemAt(unit, replayItems[i].position, replayItems[i].item);
    if (!given) return;
    if (replayGivenCount == replayGivenCapacity) {
        int capacity = replayGivenCapacity ? replayGivenCapacity * 2 : 64;
        int *units = realloc(replayGivenUnits, (size_t)capacity * sizeof *units);
        if (!units) {
            writeLogError("item_slots_replay_given", "reason=out_of_memory unit=%d", unit);
            clearUnitExtraItems(unit);
            return;
        }
        replayGivenUnits = units;
        replayGivenCapacity = capacity;
    }
    replayGivenUnits[replayGivenCount++] = unit;
}

static void endReplayItems(void) {
    for (int i = 0; i < replayGivenCount; i++) clearUnitExtraItems(replayGivenUnits[i]);
    if (replayGivenCount) writeLog("item_slots_replay_ended", "units=%d", replayGivenCount);
    replayGivenCount = 0;
    replayItemsRead = 0;
    forgetReplayItems();
}

static int itemSlotReplayState;

#ifndef __APPLE__
// ---- Windows: the writer and the loader are functions ---------------------------------------------------------
typedef void (*BattleReplayFileFn)(const char *path);
static BattleReplayFileFn gameSaveBattleReplay, gameLoadBattleReplay;

static void saveBattleReplayHook(const char *path) {
    gameSaveBattleReplay(path);
    if (!anyExtraItems()) return;
    FILE *file = fopen(path, "ab");
    if (!file) {
        writeLogError("item_slots_replay_written", "ok=0 reason=no_file");
        return;
    }
    writeReplayItems(file);
    fclose(file);
}

static void loadBattleReplayHook(const char *path) {
    replayLoadPath = path;
    replayItemsRead = 0;
    gameLoadBattleReplay(path);
    replayLoadPath = NULL;
}

static int installItemSlotReplaySites(void) {
    struct { unsigned int rva; void *hook; void **game; } hooks[] = {
        {RVA_SAVE_BATTLE_REPLAY, (void *)saveBattleReplayHook, (void **)&gameSaveBattleReplay},
        {RVA_LOAD_BATTLE_REPLAY, (void *)loadBattleReplayHook, (void **)&gameLoadBattleReplay},
    };
    for (size_t i = 0; i < sizeof hooks / sizeof hooks[0]; i++) {
        void *target = gameBase + hooks[i].rva;
        if (!fanpatchHookCreate(target, hooks[i].hook, hooks[i].game) || !fanpatchHookEnable(target, 1)) return 0;
    }
    return 1;
}
#else
// ---- Mac: the writer is inline in RVA_DO_ALL_FIGHTS ------------------------------------------------------------
static int closeBattleReplayWritten(FILE *file) {
    writeReplayItems(file);
    return fclose(file);
}

static int installItemSlotReplaySites(void) {
    unsigned char *at = gameBase + RVA_REPLAY_WRITE_FCLOSE_CALL;
    if (blTarget(at) != gameBase + RVA_FCLOSE) {
        writeLogError("hook_skipped", "site=item_slot_replay_write reason=not_the_expected_call");
        return 0;
    }
    unsigned char *trampoline = newTrampoline((void *)closeBattleReplayWritten);
    if (!trampoline) {
        writeLogError("hook_skipped", "site=item_slot_replay_write reason=no_padding");
        return 0;
    }
    uint32_t word = blTo(at, trampoline);
    return writeGameCode(at, &word, 4);
}
#endif

static int installItemSlotReplay(void) {
    if (itemSlotReplayState) return itemSlotReplayState > 0;
    itemSlotReplayState = installItemSlotReplaySites() ? 1 : -1;
    writeResultLog(itemSlotReplayState > 0, "item_slot_replay_installed", "sites=%d", itemSlotReplayState > 0 ? 2 : 0);
    return itemSlotReplayState > 0;
}
