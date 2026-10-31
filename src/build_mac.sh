#!/bin/bash
# Builds this mod's Mac dylibs into the mod folder above with zig 0.16.0 (set ZIG if it isn't on PATH).
set -e
cd "$(dirname "$0")"
ZIG=${ZIG:-zig}
"$ZIG" cc -target aarch64-macos.11.0 -dynamiclib -O2 -Wall -Wextra -fstack-protector -g0 -s -install_name coe5se.dylib -o ../coe5se.dylib frozen_stub/mac_stub.c -isystem frozen_stub/mac_sdk frozen_stub/mac_sdk/CoreFoundation.tbd
"$ZIG" cc -target aarch64-macos.11.0 -dynamiclib -O2 -Wall -Wextra -fstack-protector -g0 -s -install_name coe5se_loader.dylib -o ../coe5se_loader.dylib coe5se_loader.c -isystem mac_sdk mac_sdk/CoreFoundation.tbd
"$ZIG" cc -target aarch64-macos.11.0 -dynamiclib -O2 -Wall -Wextra -fstack-protector -g0 -s -install_name coe5se.c5plugin.dylib -o ../coe5se.c5plugin.dylib coe5se_plugin.c fanpatch/fix_feeblemind.c fanpatch/fix_item_reset.c fanpatch/fix_token_sprites.c lua/lapi.c lua/lauxlib.c lua/lbaselib.c lua/lcode.c lua/lcorolib.c lua/lctype.c lua/ldebug.c lua/ldo.c lua/ldump.c lua/lfunc.c lua/lgc.c lua/llex.c lua/lmathlib.c lua/lmem.c lua/lobject.c lua/lopcodes.c lua/lparser.c lua/lstate.c lua/lstring.c lua/lstrlib.c lua/ltable.c lua/ltablib.c lua/ltm.c lua/lundump.c lua/lutf8lib.c lua/lvm.c lua/lzio.c -I. -Ilua -isystem mac_sdk mac_sdk/CoreFoundation.tbd
