#!/bin/sh

FILES="src/circle.c src/cons.c src/consrc.c src/hashfn.c src/mpool.c src/object.c src/quadedge.c src/quadtree.c src/triangle.c src/vec.c"
TESTS=
CFLAGS="-std=c11 -Os -Isrc"
EMFLAGS="-s ASM_JS=1 -s INVOKE_RUN=0 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s MODULARIZE=1"
OUT=geom.js

usage()
{
    cat <<EOF
Usage:
  -a     : separate asm.js output
  -c     : enable Closure compiler step
  -d     : remove duplicate functions
  -D SYM : add define
  -h     : show this help
  -k     : enable runtime checks
  -s     : enable SSE (SIMD.js)
  -t     : include tests
EOF
    exit 1
}

while getopts acdhkstD: opt; do
    case $opt in
        s) CFLAGS="$CFLAGS -DCT_FEATURE_SSE -fslp-vectorize -msse2"
           EMFLAGS="$EMFLAGS -s SIMD=1"
           ;;
        a) EMFLAGS="$EMFLAGS --separate-asm"
           ;;
        c) EMFLAGS="$EMFLAGS --closure 1"
           ;;
        d) EMFLAGS="$EMFLAGS -s ELIMINATE_DUPLICATE_FUNCTIONS=1"
           # EMFLAGS="$EMFLAGS -s ELIMINATE_DUPLICATE_FUNCTIONS_PASSES=5"
           EMFLAGS="$EMFLAGS -s ELIMINATE_DUPLICATE_FUNCTIONS_DUMP_EQUIVALENT_FUNCTIONS=1"
           ;;
        k) CFLAGS="$CFLAGS -DCT_FEATURE_CHECKS"
           ;;
        D) CFLAGS="$CFLAGS -D$OPTARG"
           ;;
        t) CFLAGS="$CFLAGS -DNDEBUG -DCT_FEATURE_LOG"
           TESTS="src/test.c test/circle.c test/cons.c test/consrc.c test/hash.c test/main.c test/mpool.c test/quadtree.c test/qedge.c test/vec.c"
           ;;
        h) usage
           ;;
        \?) echo "invalid option: $opt" >&2
            usage
            exit 1
            ;;
        :) echo "$opt missing argument" >&2
           usage
           exit 1
           ;;
    esac
done

echo "cflags: $CFLAGS"
echo "emflags: $EMFLAGS"

emcc $CFLAGS \
     --memory-init-file 0 \
     --emit-symbol-map \
     $EMFLAGS \
     -s "TOTAL_STACK=1*1024*1024" \
     -s "EXPORT_NAME='geom'" \
     -o $OUT \
     $FILES \
     $TESTS

ls -la $OUT
cp $OUT geom-cljs/resources/public/js/

# -s EXPORTED_FUNCTIONS=@exports.json
# -s ELIMINATE_DUPLICATE_FUNCTIONS_DUMP_EQUIVALENT_FUNCTIONS=1
