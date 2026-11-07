#pragma once

#define PLANE_ZONES_MAX           8
#define PLANE_ZONE_SWAP_MAX       32
#define PLANE_UNIT_NAME_SIZE      52
#define PLANE_UNIT_COUNT_MAX      1000
#define PLANE_ZONE_RADIUS_MAX     999

enum { PIECE_CENTRE, PIECE_RANDOM, PIECE_AT, PIECE_NEAR_PORTAL };

typedef struct {
    char file[MAX_PATH];
    int w, h;
    int placement, count, x, y;
    int line;
} PlanePiece;

typedef struct {
    char monster[PLANE_UNIT_NAME_SIZE];
    int count, chance, line;
} PlaneUnits;

typedef struct {
    PlaneUnits *units;
    int count, room;
} PlaneUnitList;

typedef struct {
    char name[PLANE_NAME_SIZE];
    int piece, radius, line;
    int swapCount;
    short swapFrom[PLANE_ZONE_SWAP_MAX], swapTo[PLANE_ZONE_SWAP_MAX];
    int swapLine[PLANE_ZONE_SWAP_MAX];
    int populateOff;
    PlaneUnitList units;
} PlaneZone;

typedef struct { int piece, x, y, w, h; } PlacedPiece;

typedef struct {
    PlanePiece *pieces;
    int pieceCount, pieceRoom;
    PlaneZone zones[PLANE_ZONES_MAX];
    int zoneCount;
    PlaneUnitList units;
    char source[MAX_PATH];
    int script;
    PlacedPiece *placed;
    int placedCount, placedRoom, wayInX, wayInY;
} PlaneLayout;

static int growLayoutList(void **list, int *room, int count, size_t size) {
    if (count < *room) return 1;
    int grown = *room ? *room * 2 : 8;
    void *p = grown > *room ? realloc(*list, (size_t)grown * size) : NULL;
    if (!p) return 0;
    *list = p, *room = grown;
    return 1;
}

static void freePlaneLayout(PlaneLayout *layout) {
    for (int z = 0; z < layout->zoneCount; z++) {
        free(layout->zones[z].units.units);
        layout->zones[z].units = (PlaneUnitList){0};
    }
    free(layout->units.units);
    free(layout->pieces);
    free(layout->placed);
    layout->units = (PlaneUnitList){0};
    layout->pieces = NULL, layout->placed = NULL;
    layout->pieceCount = layout->pieceRoom = layout->zoneCount = layout->placedCount = layout->placedRoom = 0;
}

static void dropPlaneLayoutLists(PlaneLayout *layout) {
    for (int z = 0; z < layout->zoneCount; z++) layout->zones[z].units = (PlaneUnitList){0};
    layout->units = (PlaneUnitList){0};
    layout->pieces = NULL, layout->placed = NULL;
    layout->pieceCount = layout->pieceRoom = layout->zoneCount = layout->placedCount = layout->placedRoom = 0;
}
