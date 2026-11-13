#pragma once

static int squarePanelWrapAsGame(int right, int limit, int line) {
    (void)right, (void)limit, (void)line;
    return 0;
}

static unsigned char *squarePanelElement(int index) { return GAME_PTR(RVA_LAYOUT_TABLE) + (size_t)index * LAYOUT_STRIDE; }

static int squarePanelLineFirst(int index) {
    while (index > 0 && !LAYOUT_ENDS(squarePanelElement(index - 1))) index--;
    return index;
}

static float squarePanelLineRight(int first, int last) {
    float x = 0.0f, start = 0.0f;
    for (int i = first; i <= last; i++) {
        const unsigned char *e = squarePanelElement(i);
        short type = LAYOUT_TYPE(e);
        if (type == LAYOUT_TYPE_OVERLAP) {
            x -= (x - start) * LAYOUT_F(e, LAYOUT_ELEMENT_OVERLAP);
        } else if (type < LAYOUT_TYPE_ROW_BEGIN_LIMIT && !(type == LAYOUT_TYPE_LIST_GAP && !LAYOUT_ENDS(e))) {
            start = x;
            x += LAYOUT_F(e, LAYOUT_ELEMENT_WIDTH) + LAYOUT_ELEMENT_PAD * LAYOUT_F(e, LAYOUT_ELEMENT_COLUMN_X);
        }
    }
    return x;
}

static int squarePanelElementBefore(int first, int index) {
    for (int i = index - 1; i >= first; i--) {
        short type = LAYOUT_TYPE(squarePanelElement(i));
        if (type != LAYOUT_TYPE_OVERLAP && type < LAYOUT_TYPE_ROW_BEGIN_LIMIT) return i;
    }
    return -1;
}

static int squarePanelLinesAdded;

static void breakSquarePanelLine(int first, int before, int index) {
    squarePanelLinesAdded++;
    float ended = squarePanelLineRight(first, before);
    LAYOUT_ENDS(squarePanelElement(before)) = 1;
    for (int i = before + 1; i < index; i++)
        if (LAYOUT_TYPE(squarePanelElement(i)) == LAYOUT_TYPE_OVERLAP) LAYOUT_TYPE(squarePanelElement(i)) = LAYOUT_TYPE_LIST_GAP;
    if (GF(RVA_WIDEST_LINE) < ended) GF(RVA_WIDEST_LINE) = ended;
    GF(RVA_LINE_WIDTH) = squarePanelLineRight(index, index);
}

static void wrapSquarePanelIcon(int index) {
    int first = squarePanelLineFirst(index);
    float right = squarePanelLineRight(first, index), limit = (float)G32(RVA_SCREEN_W);
    if (right <= limit) return;
    int before = squarePanelElementBefore(first, index);
    if (before < 0) return;
    int line = squarePanelLinesAdded + 1;
    int answer = ((int (*)(int, int, int))siteEntry(SITE_SQUARE_PANEL_WRAP, (void *)squarePanelWrapAsGame))((int)(right + 0.999f), (int)limit, line);
    if (answer) breakSquarePanelLine(first, before, index);
}

static int (*nextSquarePanelIcon)(int, int, int, int);

static int drawSquarePanelIcon(int id, int kind, int sprite, int flags) {
    int last = G32(RVA_LAYOUT_LAST);
    int result = nextSquarePanelIcon(id, kind, sprite, flags);
    int index = G32(RVA_LAYOUT_LAST);
    if (index > last && index > 0 && siteInstalled[SITE_SQUARE_PANEL_WRAP]) wrapSquarePanelIcon(index);
    return result;
}

static void (*nextSquarePanelLayout)(int, float, float);

static void layOutSquarePanel(int first, float x, float top) {
    int added = squarePanelLinesAdded;
    squarePanelLinesAdded = 0;
    nextSquarePanelLayout(first, x, top + (float)added * SQUARE_PANEL_LINE_HEIGHT * GF(RVA_UI_TEXT_SCALE));
}

static int installSquarePanelIcons(void) {
    static int done;
    if (done) return done > 0;
    char owner[64];
    moduleFileName((void *)drawSquarePanelIcon, owner, sizeof owner);
    const WrapPlace outermost = {.priority = WRAP_PRIORITY_DEFAULT, .layer = WRAP_LAYER_OUTERMOST};
    int layout = insertWrap(SITE_SQUARE_PANEL_LAYOUT, (void *)layOutSquarePanel, (void **)&nextSquarePanelLayout, outermost, owner, NULL);
    int icons = layout && insertWrap(SITE_SQUARE_PANEL_ICON, (void *)drawSquarePanelIcon, (void **)&nextSquarePanelIcon, outermost, owner, NULL);
    done = layout && icons ? 1 : -1;
    writeLog("square_panel_wrap_installed", "layout=%d icons=%d", layout, icons);
    return done > 0;
}
