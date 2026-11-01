#!/bin/bash -eu
# Builds each fuzz/fuzz_*.c harness with the engine and sanitizer ClusterFuzzLite picked,
# and zips fuzz/corpus/<name>/ as that harness's seed corpus.
for src in fuzz/fuzz_*.c; do
  name=$(basename "$src" .c)
  $CC $CFLAGS -c "$src" -o "$WORK/$name.o"
  $CXX $CXXFLAGS $LIB_FUZZING_ENGINE "$WORK/$name.o" -o "$OUT/$name"
  corpus="fuzz/corpus/${name#fuzz_}"
  if [ -d "$corpus" ]; then
    (cd "$corpus" && zip -q -r "$OUT/${name}_seed_corpus.zip" .)
  fi
done
