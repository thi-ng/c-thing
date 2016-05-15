#!/bin/sh

FILES="src/circle.c src/mpool.c src/triangle.c src/vec.c"
TESTS=
CFLAGS="-std=c11 -Os -Isrc"
EMFLAGS="-s ASM_JS=1 -s INVOKE_RUN=0 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s MODULARIZE=1"
OUT=geom.js

usage()
{
    cat <<EOF
Usage:
  -c     : enable Closure compiler step
  -d     : remove duplicate functions
  -D SYM : add define
  -h     : show this help
  -k     : enable runtime checks
  -m     : enable MPool tracing
  -s     : enable SSE (SIMD.js)
  -t     : include tests
EOF
    exit 1
}

while getopts cdhkmstD: opt; do
    case $opt in
        s) CFLAGS="$CFLAGS -fslp-vectorize -msse"
           ;;
        c) EMFLAGS="$EMFLAGS --closure 1"
           ;;
        d) EMFLAGS="$EMFLAGS -s ELIMINATE_DUPLICATE_FUNCTIONS=1"
           # EMFLAGS="$EMFLAGS -s ELIMINATE_DUPLICATE_FUNCTIONS_PASSES=5"
           EMFLAGS="$EMFLAGS -s ELIMINATE_DUPLICATE_FUNCTIONS_DUMP_EQUIVALENT_FUNCTIONS=1"
           ;;
        k) CFLAGS="$CFLAGS -DCT_FEATURE_CHECKS"
           ;;
        m) CFLAGS="$CFLAGS -DCT_FEATURE_TRACE_MPOOL"
           ;;
        D) CFLAGS="$CFLAGS -D$OPTARG"
           ;;
        t) CFLAGS="$CFLAGS -DCT_FEATURE_CHECKS"
           TESTS="test/test-mpool.c test/test-vec.c test/test-circle.c test/main.c"
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
