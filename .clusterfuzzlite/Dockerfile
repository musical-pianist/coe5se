# base-builder:v1, pinned by digest
FROM gcr.io/oss-fuzz-base/base-builder@sha256:1f9cf74bc3d7029b528494542f9d9851b256dceed38d3f420989434dc5ae0760
COPY . $SRC/coe5se
WORKDIR $SRC/coe5se
COPY .clusterfuzzlite/build.sh $SRC/
