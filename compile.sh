### 
# @Author: Sunqi
 # @Date: 2019-09-09 02:03:24
 # @LastEditTime: 2019-10-24 15:37:20
 # @Description: Emcc Compile Shell
 # @FilePath: /aac-wasm-decoder/compile.sh
 ###

#!/bin/sh
emcc \
		-o "./dist/aac.js" \
		-O1 \
		-s WASM=1 \
		-s NO_DYNAMIC_EXECUTION=1 \
		-s NO_FILESYSTEM=1 \
		-s EXTRA_EXPORTED_RUNTIME_METHODS="['cwrap']" \
		-s EXPORTED_FUNCTIONS="[ \
				'_free', '_malloc' \
			, '_aac_adts_decoder_version' \
			, '_aac_adts_decoder_create' \
			, '_aac_adts_decoder_free' \
			, '_aac_adts_decoder_decode' \
			, '_main' \
		]" \
		-s ASSERTIONS=1 \
		--pre-js 'js/pre.js' \
		--post-js 'js/post.js' \
		-I "faad2/include" \
		"faad2/libfaad/.libs/libfaad.2.dylib" \
		decoder.c \
		-v





