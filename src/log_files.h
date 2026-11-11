#pragma once
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#ifndef LOG_FILE_MAX
#define LOG_FILE_MAX (100L * 1024 * 1024)
#endif
#define LOG_FILES 5
#define LOG_FILE "coe5mod.log"
#define LOG_FOLDER "coe5se_logs"
#define LOG_STUB_OLD_FILE "coe5mod.old.log"
#ifdef _WIN32
#define LOG_PATH_SEP "\\"
#else
#define LOG_PATH_SEP "/"
#endif
#define LOG_HEADER_END_EVENT "log_header_end"

static __attribute__((unused)) int logFilePath(const char *path, int n, char *out, size_t size) {
    size_t length = strlen(path);
    if (length < 4 || strcmp(path + length - 4, ".log")) return 0;
    int wrote = n == 0 ? snprintf(out, size, "%s", path)
              : n < 0  ? snprintf(out, size, "%.*s.old.log", (int)(length - 4), path)
                       : snprintf(out, size, "%.*s.%d.log", (int)(length - 4), path, n);
    return wrote > 0 && (size_t)wrote < size;
}

static __attribute__((unused)) int shiftLogFiles(const char *path, const char *from) {
    char older[1024], newer[1024];
    if (!logFilePath(path, LOG_FILES - 1, older, sizeof older)) return 0;
    remove(older);
    for (int n = LOG_FILES - 2; n >= 1; n--) {
        if (!logFilePath(path, n, newer, sizeof newer)) return 0;
        rename(newer, older);
        memcpy(older, newer, sizeof older);
    }
    remove(older);
    return rename(from, older) == 0;
}

// @platform-split log_truncate: Windows cuts with _chsize_s, a Mac with ftruncate
static __attribute__((unused)) int truncateLogFile(const char *path, long bytes) {
    FILE *f = fopen(path, "r+b");
    if (!f) return 0;
#ifdef _WIN32
    int ok = _chsize_s(_fileno(f), bytes) == 0;
#else
    int ok = ftruncate(fileno(f), bytes) == 0;
#endif
    return fclose(f) == 0 && ok;
}

static __attribute__((unused)) long launchStartIn(const char *text, long length, unsigned long pid) {
    char tail[32];
    snprintf(tail, sizeof tail, " pid=%lu", pid);
    size_t tailLength = strlen(tail);
    long found = -1;
    for (long start = 0; start < length;) {
        const char *end = memchr(text + start, '\n', (size_t)(length - start));
        long lineEnd = end ? (long)(end - text) : length;
        long trimmed = lineEnd > start && text[lineEnd - 1] == '\r' ? lineEnd - 1 : lineEnd;
        const char *line = text + start;
        size_t lineLength = (size_t)(trimmed - start);
        static const char EVENT[] = " event=loader_attached ";
        if (lineLength > tailLength && !memcmp(line + lineLength - tailLength, tail, tailLength)) {
            for (size_t i = 0; i + sizeof EVENT - 1 <= lineLength; i++)
                if (!memcmp(line + i, EVENT, sizeof EVENT - 1)) {
                    found = start;
                    break;
                }
        }
        start = lineEnd + 1;
    }
    return found;
}

typedef struct {
    int found;
    long earlier;
    int fromOld;
    int error;
} LogRotation;

static __attribute__((unused)) LogRotation rotateLogAtLaunch(const char *path, unsigned long pid) {
    LogRotation r = {0, 0, 0, 0};
    char old[1024];
    FILE *probe;
    if (logFilePath(path, -1, old, sizeof old) && (probe = fopen(old, "rb")) != NULL) {
        fclose(probe);
        r.fromOld = shiftLogFiles(path, old);
    }
    FILE *f = fopen(path, "rb");
    if (!f) return r;
    long size = fseek(f, 0, SEEK_END) == 0 ? ftell(f) : -1;
    char *text = size > 0 ? malloc((size_t)size) : NULL;
    if (!text || fseek(f, 0, SEEK_SET) || fread(text, 1, (size_t)size, f) != (size_t)size) {
        free(text);
        fclose(f);
        return r;
    }
    fclose(f);
    long start = launchStartIn(text, size, pid);
    r.found = start >= 0;
    if (start == 0) {
        free(text);
        return r;
    }
    if (start < 0) start = size;
    char first[1024];
    errno = 0;
    if (!logFilePath(path, 1, first, sizeof first)) {
        free(text);
        return r;
    }
    if (shiftLogFiles(path, path)) {
        r.earlier = start;
        if (start < size) {
            truncateLogFile(first, start);
            FILE *now = fopen(path, "wb");
            if (now) {
                fwrite(text + start, 1, (size_t)(size - start), now);
                fclose(now);
            }
        }
    } else {
        r.error = errno ? errno : -1;
        FILE *earlier = fopen(first, "wb");
        int copied = earlier && fwrite(text, 1, (size_t)start, earlier) == (size_t)start;
        if (earlier && fclose(earlier)) copied = 0;
        FILE *now = copied ? fopen(path, "wb") : NULL;
        if (now) {
            fwrite(text + start, 1, (size_t)(size - start), now);
            fclose(now);
            r.earlier = start;
        } else {
            remove(first);
        }
    }
    free(text);
    return r;
}

static __attribute__((unused)) int logPathFor(const char *gameDir, char *out, size_t size) {
    char folder[1024];
    struct stat st;
    int n = snprintf(folder, sizeof folder, "%s" LOG_PATH_SEP LOG_FOLDER, gameDir);
    int inFolder = n > 0 && (size_t)n < sizeof folder && !stat(folder, &st) && (st.st_mode & S_IFMT) == S_IFDIR;
    int wrote = snprintf(out, size, "%s" LOG_PATH_SEP LOG_FILE, inFolder ? folder : gameDir);
    return wrote > 0 && (size_t)wrote < size;
}

static __attribute__((unused)) int appendLogFileTo(const char *from, const char *to) {
    FILE *in = fopen(from, "rb");
    if (!in) return 0;
    FILE *out = fopen(to, "ab");
    int ok = out != NULL;
    char buffer[16384];
    while (ok) {
        size_t n = fread(buffer, 1, sizeof buffer, in);
        if (n && fwrite(buffer, 1, n, out) != n) ok = 0;
        if (n < sizeof buffer) {
            if (ferror(in)) ok = 0;
            break;
        }
    }
    fclose(in);
    if (out && fclose(out)) ok = 0;
    return ok && remove(from) == 0 ? 1 : -1;
}

typedef struct {
    int moved;
    int removed;
    int failed;
} LogFolderMove;

static __attribute__((unused)) LogFolderMove moveGameFolderLogs(const char *gameLog, const char *log) {
    LogFolderMove m = {0, 0, 0};
    char from[1024], to[1024];
    for (int n = LOG_FILES - 1; n >= -1; n--) {
        if (!logFilePath(gameLog, n, from, sizeof from) || !logFilePath(log, n, to, sizeof to)) continue;
        if (n <= 0) {
            int r = appendLogFileTo(from, to);
            m.moved += r == 1;
            m.failed += r < 0;
            continue;
        }
        FILE *probe = fopen(from, "rb");
        if (!probe) continue;
        fclose(probe);
        FILE *taken = fopen(to, "rb");
        if (taken) {
            fclose(taken);
            if (remove(from) == 0) m.removed++;
            else m.failed++;
        } else if (rename(from, to) == 0) {
            m.moved++;
        } else {
            m.failed++;
        }
    }
    return m;
}
