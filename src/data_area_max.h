#pragma once
#include "mod_storage.h"

#define MAXDATA_WORD "maxdata"

typedef struct {
    char mod[MOD_NAME_MAX];
    int megabytes;
} DataAreaMax;

static DataAreaMax *dataAreaMaxes;
static int dataAreaMaxCount, dataAreaMaxCapacity;

static struct {
    int mod;
    int lines;
    int megabytes;
} maxdataReading = {-1, 0, 0};

static int storageAreaMegabytes(const char *mod) {
    for (int i = 0; i < dataAreaMaxCount; i++)
        if (!_stricmp(dataAreaMaxes[i].mod, mod)) return dataAreaMaxes[i].megabytes;
    return STORAGE_AREA_DEFAULT_MB;
}

static long long storageAreaMax(const char *mod) {
    return (long long)storageAreaMegabytes(mod) * 1024 * 1024;
}

static void setStorageAreaMegabytes(const char *mod, int megabytes) {
    int i = 0, was = storageAreaMegabytes(mod), now = megabytes ? megabytes : STORAGE_AREA_DEFAULT_MB;
    while (i < dataAreaMaxCount && _stricmp(dataAreaMaxes[i].mod, mod)) i++;
    if (now == STORAGE_AREA_DEFAULT_MB) {
        if (i < dataAreaMaxCount) dataAreaMaxes[i] = dataAreaMaxes[--dataAreaMaxCount];
    } else if (i < dataAreaMaxCount) {
        dataAreaMaxes[i].megabytes = now;
    } else if (growList((void **)&dataAreaMaxes, &dataAreaMaxCapacity, dataAreaMaxCount + 1, sizeof *dataAreaMaxes)) {
        snprintf(dataAreaMaxes[dataAreaMaxCount].mod, sizeof dataAreaMaxes[0].mod, "%s", mod);
        dataAreaMaxes[dataAreaMaxCount++].megabytes = now;
    } else {
        writeLog("data_area_max_dropped", "mod=%s mb=%d reason=out_of_memory", mod, now);
        return;
    }
    if (now != was) writeLog("data_area_max", "mod=%s mb=%d", mod, now);
}

static void maxdataReadingAt(int mod, int start) {
    if (start == 1) {
        maxdataReading.mod = mod;
        maxdataReading.lines = maxdataReading.megabytes = 0;
        return;
    }
    if (start < 0) {
        maxdataReading.mod = -1;
        return;
    }
    setStorageAreaMegabytes(modName(mod), maxdataReading.mod == mod ? maxdataReading.megabytes : 0);
    maxdataReading.mod = -1;
}

static const char *maxdataProblem(const char *args, int *megabytes) {
    const char *p = args;
    skipSpaces(&p);
    int negative = *p == '-';
    if (negative) p++;
    if (*p < '0' || *p > '9') return "not_a_number";
    long value = 0;
    for (; *p >= '0' && *p <= '9'; p++)
        if (value <= STORAGE_AREA_CEILING_MB) value = value * 10 + (*p - '0');
    if (!isLineEnd(p)) return "not_a_number";
    if (negative || value < 1 || value > STORAGE_AREA_CEILING_MB) return "out_of_range";
    *megabytes = (int)value;
    return NULL;
}

static void maxdataLine(const char *args, int mod, int level) {
    if (level != 2) return;
    const char *name = spellModName(mod);
    int line = G32(RVA_MOD_LINE), megabytes = 0;
    if (!readingSidecarOf(mod) && !readingPrefixedLine()) {
        refuseC5mLine(MAXDATA_WORD, name, "maxdata_in_c5m", "line=%d", line);
        return;
    }
    if (maxdataReading.mod != mod) return;
    if (++maxdataReading.lines > 1) {
        refuseC5mLine(MAXDATA_WORD, name, "duplicate_line", "line=%d", line);
        return;
    }
    const char *problem = maxdataProblem(args, &megabytes);
    if (problem) {
        refuseC5mLine(MAXDATA_WORD, name, problem, "line=%d text=%.40s min=1 max=%d", line, args, STORAGE_AREA_CEILING_MB);
        return;
    }
    maxdataReading.megabytes = megabytes;
}

static void maxdataWordInit(void) {
    int ok = apiAddModCommand("maxdata", maxdataLine);
    writeLog("maxdata_word_installed", "ok=%d", ok);
}
