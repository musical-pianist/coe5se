#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    const char *reason;
    size_t call;
    size_t target;
    int matches;
} LobbyCallShape;

static __attribute__((unused)) int lobbyShapeWinAt(const unsigned char *code, size_t size, size_t at, size_t *call, size_t *target) {
    size_t i = at;
    if (i + 3 > size || code[i] != 0x83 || code[i + 1] != 0xf8 || code[i + 2] != 0x03) return 0;
    i += 3;
    if (i + 2 <= size && code[i] == 0x75) i += 2;
    else if (i + 6 <= size && code[i] == 0x0f && code[i + 1] == 0x85) i += 6;
    else return 0;
    if (i + 6 + 5 + 6 > size || code[i] != 0x89 || (code[i + 1] & 0xc7) != 0x05) return 0;
    int32_t first, rel, second;
    memcpy(&first, code + i + 2, 4);
    size_t firstFlag = i + 6 + (size_t)(intptr_t)first;
    int firstReg = (code[i + 1] >> 3) & 7;
    i += 6;
    if (code[i] != 0xe8) return 0;
    memcpy(&rel, code + i + 1, 4);
    size_t callAt = i;
    i += 5;
    if (code[i] != 0x89 || (code[i + 1] & 0xc7) != 0x05) return 0;
    memcpy(&second, code + i + 2, 4);
    size_t secondFlag = i + 6 + (size_t)(intptr_t)second;
    if (secondFlag != firstFlag || ((code[i + 1] >> 3) & 7) == firstReg) return 0;
    *call = callAt;
    *target = callAt + 5 + (size_t)(intptr_t)rel;
    return 1;
}

static __attribute__((unused)) int lobbyShapeMacAt(const unsigned char *code, size_t size, size_t at, size_t *call, size_t *target) {
    uint32_t w[7];
    if (at % 4 || at >= size) return 0;
    size_t words = (size - at) / 4 < 7 ? (size - at) / 4 : 7;
    if (words < 6) return 0;
    memcpy(w, code + at, words * 4);
    if ((w[0] & 0xfffffc1fu) != 0x71000c1fu) return 0;
    if ((w[1] & 0xff00001fu) != 0x54000001u) return 0;
    int k = 2, base = -1;
    if ((w[k] & 0x9f000000u) == 0x90000000u) base = (int)(w[k++] & 0x1f);
    if (k + 4 > (int)words) return 0;
    if ((w[k] & 0xffffffe0u) != 0x52800020u) return 0;
    uint32_t value = w[k++] & 0x1f;
    uint32_t set = w[k++], bl = w[k++], clear = w[k];
    if ((set & 0xffc00000u) != 0x39000000u || (set & 0x1f) != value) return 0;
    if (base >= 0 && (int)((set >> 5) & 0x1f) != base) return 0;
    if ((bl & 0xfc000000u) != 0x94000000u) return 0;
    if (clear != ((set & ~0x1fu) | 0x1f)) return 0;
    size_t callAt = at + (size_t)(k - 1) * 4;
    int32_t imm26 = (int32_t)(bl << 6) >> 6;
    *call = callAt;
    *target = callAt + (size_t)((intptr_t)imm26 * 4);
    return 1;
}

static __attribute__((unused)) LobbyCallShape findLobbyCallShape(const unsigned char *code, size_t size, int arm64) {
    LobbyCallShape found = {"not_found", 0, 0, 0};
    for (size_t at = 0; at < size; at += arm64 ? 4 : 1) {
        size_t call, target;
        if (!(arm64 ? lobbyShapeMacAt(code, size, at, &call, &target) : lobbyShapeWinAt(code, size, at, &call, &target))) continue;
        if (!found.matches++) found.call = call, found.target = target;
    }
    if (found.matches == 1) found.reason = NULL;
    else if (found.matches > 1) found.reason = "several_matches";
    return found;
}
