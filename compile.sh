#!/bin/sh

# This compiles the file particles.c with optimizations,
# declares a number of exported function names,
# post-processes the resulting JS with Closure compiler and
# wraps all within a global 'Particles' module/object
# This module MUST be initialized by calling 'Particles();'
# before first use (in our example this is done from CLJS in ex07.core/main)

emcc -O2 -s ASM_JS=1 -s INVOKE_RUN=0 \
     -s EXPORTED_FUNCTIONS=@exports.json \
     -s "EXPORT_NAME='geom'" \
     -s MODULARIZE=1 \
     -o geom.js \
     mpool.c vec.c test-vec.c

# copy memory initialization file to main webroot dir
#cp resources/public/js/native.js.mem resources/public/
