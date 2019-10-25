### 
# @Author: Sunqi
 # @Date: 2019-09-09 02:03:24
 # @LastEditTime: 2019-10-26 00:55:20
 # @Description: Emcc Compile Shell
 # @FilePath: /aac-wasm-decoder/compile.sh
 ###

#!/bin/sh

export TOTAL_MEMORY=10485760

export EXPORTED_FUNCTIONS="[  	\
	'_aac_adts_decoder_version' \
	,'_aac_adts_decoder_create' \
	,'_aac_adts_decoder_free'   \
	,'_aac_adts_decoder_decode' \
	,'_main' \
]"

export LIBRARY_FUNCTIONS="[\
    'malloc', \
    'free'	  \
]"


echo "Running Emscripten..."

emcc  \
	-O1 \
 	-s WASM=1 \
	-s NO_DYNAMIC_EXECUTION=1 \
	-s NO_FILESYSTEM=0 \
	-s EXTRA_EXPORTED_RUNTIME_METHODS="['cwrap']" \
	-s EXPORTED_FUNCTIONS="${EXPORTED_FUNCTIONS}" \
	-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE="${LIBRARY_FUNCTIONS}" \
	--pre-js 'js/pre.js' \
	--post-js 'js/post.js' \
	decoder.c \
	-s ASSERTIONS=1 \
	-I "faad2/include" \
	"faad2/libfaad/.libs/libfaad.2.dylib" \
	-o "dist/aac.js" \
	-v

