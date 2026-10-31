#pragma once

#define EVENT_FINDER_SHADOW_EXPORT "coe5seEventFinderShadow"

#define EVENT_FINDER_UNIT      0
#define EVENT_FINDER_ITEM      1
#define EVENT_FINDER_AFFLICTED 2
#define EVENT_FINDER_COUNT_AT  3
#define EVENT_FINDER_COUNT     4
#define EVENT_FINDER_KINDS     5

typedef struct {
    int kind;
    int player, x, y;
    const char *name;
    int monster;
    unsigned long long stateMask, resistanceMask;
} EventFinderQuery;

typedef int (*EventFinderFn)(const EventFinderQuery *q);

typedef int (*EventFinderShadowFn)(const EventFinderQuery *q, EventFinderFn game, EventFinderFn answer);

typedef struct {
    unsigned long long answered;
    unsigned long long passed;
    unsigned long long builds;
    unsigned long long walks;
} EventFinderCounts;

typedef const EventFinderCounts *(*EventFinderShadowSetFn)(EventFinderShadowFn shadow);
