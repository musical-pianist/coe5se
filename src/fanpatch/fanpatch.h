#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
typedef unsigned char BYTE;
typedef uintptr_t ULONG_PTR;
typedef long LONG;
#define InterlockedIncrement(p) __atomic_add_fetch((p), 1, __ATOMIC_SEQ_CST)
#endif
#ifndef _MSC_VER
#define _ReturnAddress() __builtin_return_address(0)
#endif

#include "game_addresses.h"
#include "fanpatch_bridge.h"

#define GAME_PTR(type, va)      ((type)(fanpatchGameBase() + (va)))
#define GAME_VERSION_TEXT       "5.41"

#ifdef __APPLE__
#define VA_FIND_ALL_MODS        RVA_RUN_ENABLED_MODS
#else
#define VA_FIND_ALL_MODS        RVA_FIND_ALL_MODS
#define VA_REPLACED_SPR         RVA_REPLACED_SPRITE
#endif
#define VA_RESTORE_TABLES       RVA_RESET_MODS
#define VA_ITEMS                ((ULONG_PTR)(fanpatchItemTable() - fanpatchGameBase()))
#define VA_ITEM_COUNT           RVA_ITEM_COUNT
#define ITEM_MAX                fanpatchItemRecords()
#define ITEM_STRIDE             ITEM_SIZE
#define VA_ADD_AFFL             RVA_ADD_AFFLICTION
#define VA_PICK_SPELL           RVA_RANDOM_MEMORIZED_SPELL
#define VA_REBUILD_SPELLS       RVA_CHOOSE_MEMORIZED
#define VA_UNIT_CARD            RVA_UNIT_CARD
#define VA_UNITS                RVA_UNITS
#define VA_UNIT_MAX             RVA_UNIT_COUNT
#define UNIT_STRIDE             UNIT_SIZE
#define VA_DRAW_SPRITE          RVA_SPRITE_DRAW_SCALED
#define VA_LOAD_SPRFILE         RVA_LOAD_SPRITE_FILE
#define VA_SPRFILE_TABLE        RVA_SPRITE_FILES
#define VA_SPR_WIDTH            RVA_SPRITE_WIDTHS
#define VA_SPR_HEIGHT           RVA_SPRITE_HEIGHTS
#ifdef __APPLE__
#define VA_TOKEN_ICON_RET       (RVA_TOKEN_ITEM_ICON_CALL + 4)
#else
#define VA_TOKEN_ICON_RET       (RVA_TOKEN_ITEM_ICON_CALL + 5)
#endif

#define fp_log                  fanpatchLog
#define fp_config_int(section, key, def) (def)
#define fp_game_code_matches    fanpatchGameCodeMatches
