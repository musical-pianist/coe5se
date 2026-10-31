#pragma once

static int checkGameFunction(const char *event, const char *head) {
    if (!gameBuild) {
        writeLog(event, "%schecked=0 reason=unknown_game_build", head);
        return 0;
    }
    if (!gameCodeMatches) {
        writeLog(event, "%smatch=0 reason=game_code_differs", head);
        return 0;
    }
    writeLog(event, "%smatch=1 build=%s", head, gameBuild->gameVersion);
    return 1;
}

static void checkNameBoxCode(void) { checkGameFunction("name_box_check", ""); }

static int checkEventFinderCode(void) {
    int unit = checkGameFunction("event_finders_check", "function=find_unit_at ");
    int item = checkGameFunction("event_finders_check", "function=find_item_holder_at ");
    int afflicted = checkGameFunction("event_finders_check", "function=find_afflicted_unit_at ");
    int countAt = checkGameFunction("event_finders_check", "function=count_units_at ");
    int count = checkGameFunction("event_finders_check", "function=count_units ");
    return unit && item && afflicted && countAt && count;
}
