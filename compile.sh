#!/bin/sh

FLAGS=

usage()
{
    cat <<EOF
Usage:
  -c     : enable Closure compiler step
  -D SYM : add define
  -h     : show this help
  -m     : enable MPool tracing
  -s     : enable SSE (SIMD.js)
  -t     : include tests
EOF
    exit 1
}

while getopts chmstD: opt; do
    case $opt in
        s) FLAGS="$FLAGS -fslp-vectorize -msse"
           ;;
        c) FLAGS="$FLAGS --closure 1"
           ;;
        m) FLAGS="$FLAGS -DTRACE_MPOOL"
           ;;
        D) FLAGS="$FLAGS -D$OPTARG"
           ;;
        t) TESTS="test/test-mpool.c test/test-vec.c test/main.c"
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

echo "using flags: $FLAGS"

emcc -Os -Isrc \
     $FLAGS \
     --memory-init-file 0 \
     -s ASM_JS=1 \
     -s INVOKE_RUN=0 \
     -s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
     -s "TOTAL_STACK=1*1024*1024" \
     -s "EXPORT_NAME='geom'" \
     -s MODULARIZE=1 \
     -o geom.js \
     src/mpool.c all.c \
     $TESTS

cp geom.js geom-cljs/resources/public/js/

# -s EXPORTED_FUNCTIONS=@exports.json
