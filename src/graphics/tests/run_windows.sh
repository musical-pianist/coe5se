#!/bin/bash
# Builds and runs the built-in renderer's Windows regressions with llvm-mingw.
set -e
cd "$(dirname "$0")/.."
CC=${CC:-x86_64-w64-mingw32-clang}
mkdir -p tests/build
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -Wno-unused-function -I. tests/test_texture_cache.c -o tests/build/test_texture_cache.exe
tests/build/test_texture_cache.exe
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -I. -I.. tests/test_graphics_imports.c -lbcrypt -lshell32 -luser32 -o tests/build/test_graphics_imports.exe
tests/build/test_graphics_imports.exe
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -I. -I.. tests/test_init.c -lbcrypt -lshell32 -luser32 -o tests/build/test_init.exe
tests/build/test_init.exe
