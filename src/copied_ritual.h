#pragma once

static int copyritualCarrySource(const char *line, int level) {
    if (level != 2 || !spellWordIs(line, strcspn(line, " \t\r\n"), "copyritual")) return -1;
    int target = G32(RVA_RITUAL_CURRENT);
    if (target < 0 || target >= ritualSlotCount()) return -1;
    char text[OFFSET_NAME_TEXT_MAX] = "";
    const char *quote = strchr(line, '"');
    if (quote) readSpellQuoted(&quote, text, sizeof text);
    int source = text[0] ? ritualByName(text, 0) : -1;
    return source == target ? -1 : source;
}

static void carryCopiedRitualWords(int source, int mod) {
    int target = G32(RVA_RITUAL_CURRENT);
    int ok = copyNoExplodeStrings(source, target);
    ok &= copyCorpseRules(source, target);
    ok &= copyRandomEquipLines(source, target);
    ok &= copyCasterMaxCast(source, target);
    ok &= copyFactionRefs(FACTION_REF_RITUAL, source, target);
    ok &= copyPlaneRefs(source, target);
    if (!ok) refuseC5mLine("copyritual", spellModName(mod), "out_of_memory", "line=%d", G32(RVA_MOD_LINE));
}
