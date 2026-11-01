#pragma once
#include <stdarg.h>
#include <stdio.h>

#ifndef LOG_TAG
#error "Define LOG_TAG before including startup_log.h"
#endif

#ifndef PRINTF_FORMAT
#if defined(__MINGW32__) && !defined(__clang__)
#define PRINTF_FORMAT(fmtIndex, firstArg) __attribute__((format(gnu_printf, fmtIndex, firstArg)))
#else
#define PRINTF_FORMAT(fmtIndex, firstArg) __attribute__((format(printf, fmtIndex, firstArg)))
#endif
#endif

static char logPath[MAX_PATH];

#ifdef STARTUP_LOG_ESCAPE
#include <stdlib.h>
#include "log_escape.h"
#endif

#ifdef STARTUP_LOG_ESCAPE
#define LOG_LEVEL_INFO ""
#define LOG_LEVEL_WARNING "level=warning "
#define LOG_LEVEL_ERROR "level=error "

static PRINTF_FORMAT(3, 0) void writeLogAt(const char *level, const char *event, const char *fmt, va_list given) {
    if (!logPath[0]) return;
    FILE *f = fopen(logPath, "a");
    if (!f) return;
    SYSTEMTIME t;
    GetLocalTime(&t);
    fprintf(f, "%02d:%02d:%02d.%03d plugin=" LOG_TAG " event=%s %s", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, event, level);
    va_list ap, again;
    va_copy(ap, given);
    va_copy(again, given);
    char stackText[1024], *text = stackText, *heap = NULL;
    int length = vsnprintf(stackText, sizeof stackText, fmt, ap);
    if (length >= (int)sizeof stackText && (heap = malloc((size_t)length + 1)) != NULL) {
        text = heap;
        vsnprintf(heap, (size_t)length + 1, fmt, again);
    } else if (length >= (int)sizeof stackText) {
        length = (int)sizeof stackText - 1;
    }
    va_end(again);
    va_end(ap);
    for (int i = 0; i < length; i++) {
        char escaped[4];
        size_t size = logByteSize((unsigned char)text[i]);
        escaped[0] = text[i];
        escapeLogText(escaped, 1, size - 1);
        fwrite(escaped, 1, size, f);
    }
    free(heap);
#ifdef STARTUP_LOG_PID
    fprintf(f, " pid=%lu", (unsigned long)GetCurrentProcessId());
#endif
    fputc('\n', f);
    fclose(f);
}

static PRINTF_FORMAT(2, 3) void writeLog(const char *event, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    writeLogAt(LOG_LEVEL_INFO, event, fmt, ap);
    va_end(ap);
}

static __attribute__((unused)) PRINTF_FORMAT(2, 3) void writeLogWarning(const char *event, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    writeLogAt(LOG_LEVEL_WARNING, event, fmt, ap);
    va_end(ap);
}

static __attribute__((unused)) PRINTF_FORMAT(2, 3) void writeLogError(const char *event, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    writeLogAt(LOG_LEVEL_ERROR, event, fmt, ap);
    va_end(ap);
}
#define STUB_SAFE_WARNING writeLogWarning
#define STUB_SAFE_ERROR writeLogError
#else
#define STUB_SAFE_WARNING writeLog
#define STUB_SAFE_ERROR writeLog

static PRINTF_FORMAT(2, 3) void writeLog(const char *event, const char *fmt, ...) {
    if (!logPath[0]) return;
    FILE *f = fopen(logPath, "a");
    if (!f) return;
    SYSTEMTIME t;
    GetLocalTime(&t);
    fprintf(f, "%02d:%02d:%02d.%03d plugin=" LOG_TAG " event=%s ", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, event);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
#ifdef STARTUP_LOG_PID
    fprintf(f, " pid=%lu", (unsigned long)GetCurrentProcessId());
#endif
    fputc('\n', f);
    fclose(f);
}
#endif

#include "build_path.h"
