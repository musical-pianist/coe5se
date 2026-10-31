#pragma once

#include <stddef.h>

unsigned char *fanpatchGameBase(void);
void fanpatchLog(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
int fanpatchGameCodeMatches(const char *fix);
unsigned char *fanpatchItemTable(void);
int fanpatchItemRecords(void);
int fanpatchNetworkGame(void);
int fanpatchSpellAboveLevel(int unit);
int fanpatchReplacedSprite(unsigned file, unsigned spr);

int fanpatchHookCreate(void *target, void *detour, void **original);
int fanpatchHookEnable(void *target, int enable);
int fanpatchHookRemove(void *target);

int fix_item_reset_install(void);
void fix_item_reset_report(void);
int fix_feeblemind_install(void);
void fix_feeblemind_report(void);
int fix_token_sprites_install(void);
void fix_token_sprites_report(void);
