#pragma once
#include <stdlib.h>

#define OFFICIAL_GAMES_TITLE "CoE5SE: Multiplayer on Illwinter servers"

static __attribute__((unused)) void officialTellDialog(const char *text) {
#ifdef __APPLE__
    CFStringRef message = CFStringCreateWithCString(NULL, text, kCFStringEncodingUTF8);
    if (!message) message = CFStringCreateWithCString(NULL, text, 0x0201);
    CFOptionFlags response = kCFUserNotificationDefaultResponse;
    CFUserNotificationDisplayAlert(0, kCFUserNotificationCautionAlertLevel, NULL, NULL, NULL, CFSTR(OFFICIAL_GAMES_TITLE), message,
                                   CFSTR("Ok"), NULL, NULL, &response);
    if (message) CFRelease(message);
#else
    MessageBoxA(NULL, text, OFFICIAL_GAMES_TITLE, MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
#endif
}

static __attribute__((unused)) void officialSetEnvironment(const char *name, const char *value) {
#ifdef __APPLE__
    if (value) setenv(name, value, 1);
    else unsetenv(name);
#else
    SetEnvironmentVariableA(name, value);
#endif
}
