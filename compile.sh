#!/bin/sh

FILES="src/circle.c src/mpool.c src/triangle.c src/vec.c"
TESTS=
CFLAGS=
EMFLAGS=

usage()
{
    cat <<EOF
Usage:
  -c     : enable Closure compiler step
  -d     : remove duplicate functions
  -D SYM : add define
  -h     : show this help
  -m     : enable MPool tracing
  -s     : enable SSE (SIMD.js)
  -t     : include tests
EOF
    exit 1
}

while getopts cdhmstD: opt; do
    case $opt in
        s) CFLAGS="$CFLAGS -fslp-vectorize -msse"
           ;;
        c) EMFLAGS="$CFLAGS --closure 1"
           ;;
        d) EMFLAGS="$EMFLAGS -s ELIMINATE_DUPLICATE_FUNCTIONS=1"
           # EMFLAGS="$EMFLAGS -s ELIMINATE_DUPLICATE_FUNCTIONS_PASSES=5"
           EMFLAGS="$EMFLAGS -s ELIMINATE_DUPLICATE_FUNCTIONS_DUMP_EQUIVALENT_FUNCTIONS=1"
           ;;
        m) CFLAGS="$CFLAGS -DTRACE_MPOOL"
           ;;
        D) CFLAGS="$CFLAGS -D$OPTARG"
           ;;
        t) TESTS="test/test-mpool.c test/test-vec.c test/test-circle.c test/main.c"
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

echo "using cflags: $CFLAGS"
echo "using emflags: $EMFLAGS"

emcc -Os -Isrc \
     $CFLAGS \
     --memory-init-file 0 \
     -s ASM_JS=1 \
     -s INVOKE_RUN=0 \
     -s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
     -s "TOTAL_STACK=1*1024*1024" \
     -s "EXPORT_NAME='geom'" \
     -s MODULARIZE=1 \
     $EMFLAGS \
     -o geom.js \
     $FILES \
     $TESTS

cp geom.js geom-cljs/resources/public/js/

# -s EXPORTED_FUNCTIONS=@exports.json
# -s ELIMINATE_DUPLICATE_FUNCTIONS_DUMP_EQUIVALENT_FUNCTIONS=1
