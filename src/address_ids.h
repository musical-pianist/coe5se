#pragma once
#include <stdint.h>

#define ADDRESS_ID_FORMAT 1
#define ADDRESS_SLOT_MAGIC 0x41354543u

#define ADDRESS_KIND_OFFSET 1
#define ADDRESS_KIND_SIZE   2
#define ADDRESS_KIND_FRAME  3

typedef struct AddressSlot {
    uint32_t magic;
    uint16_t format;
    uint16_t kind;
    uint32_t id;
    uint32_t reserved;
    volatile uintptr_t value;
    const char *name;
} AddressSlot;

typedef struct AddressEntry {
    uint32_t id;
    uint16_t kind;
    const char *name;
    unsigned long long value;
    const char *missing;
} AddressEntry;

#ifdef __APPLE__
#define ADDRESS_SLOT_SECTION __attribute__((section("__DATA,__c5addr"), used, aligned(8)))
#else
#define ADDRESS_SLOT_SECTION __attribute__((section(".c5addr"), used, aligned(8)))
#endif

#ifndef COE5SE_RAW_ADDRESSES
#include "jazzy_api.h"

static __attribute__((unused, noinline, cold)) uintptr_t addressSlotResolve(AddressSlot *slot) {
    jazzyConnect();
    uintptr_t value = slot->value;
    if (!value) __builtin_trap();
    return value;
}

#define ADDRESS_SLOT_VALUE(kindValue, idValue, nameText) ({ \
    static ADDRESS_SLOT_SECTION AddressSlot addressSlot_ = \
        {ADDRESS_SLOT_MAGIC, ADDRESS_ID_FORMAT, kindValue, idValue, 0, 0, nameText}; \
    uintptr_t addressValue_ = addressSlot_.value; \
    addressValue_ ? addressValue_ : addressSlotResolve(&addressSlot_); })

#define ADDRESS_ID(id, name) ((unsigned char *)ADDRESS_SLOT_VALUE(ADDRESS_KIND_OFFSET, id, name))
#define SIZE_ID(id, name)    ((int)ADDRESS_SLOT_VALUE(ADDRESS_KIND_SIZE, id, name))
#define FRAME_ID(id, name)   name
#ifdef __APPLE__
#define ADDRESS_ID_WINDOWS(id, name, macReason) macReason
#define ADDRESS_ID_MAC(id, name, winReason)     ADDRESS_ID(id, name)
#define FRAME_ID_WINDOWS(id, name, macReason)   macReason
#define FRAME_ID_MAC(id, name, winReason)       name
#define ADDRESS_ID_NEITHER(winReason, macReason) macReason
#else
#define ADDRESS_ID_WINDOWS(id, name, macReason) ADDRESS_ID(id, name)
#define ADDRESS_ID_MAC(id, name, winReason)     winReason
#define FRAME_ID_WINDOWS(id, name, macReason)   name
#define FRAME_ID_MAC(id, name, winReason)       winReason
#define ADDRESS_ID_NEITHER(winReason, macReason) winReason
#endif

#include "game_address_ids.h"
#if GAME_ADDRESS_IDS_FORMAT != ADDRESS_ID_FORMAT
#error "game_address_ids.h and address_ids.h are from different CoE5SE versions; copy both from the same one"
#endif

#define GAME_PTR(address) ((unsigned char *)(address))
#define G8(address)  (*(unsigned char *)(address))
#define G16(address) (*(short *)(address))
#define G32(address) (*(int *)(address))
#define GF(address)  (*(float *)(address))
#define GAME_FN(type, address) ((type)(void *)(address))
#endif
