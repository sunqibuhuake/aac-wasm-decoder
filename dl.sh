### 
# @Author: Sunqi
 # @Date: 2019-10-24 10:19:32
 # @LastEditTime: 2019-10-24 13:40:41
 # @Description: Download faad2 and compile with emcc
 # @FilePath: /aac-wasm-decoder/dl.sh
 ###
 
#!/bin/sh

export FAAD2_VERSION=2.7

curl -L -O http://downloads.sourceforge.net/faac/faad2-${FAAD2_VERSION}.tar.gz
tar zxf faad2-${FAAD2_VERSION}.tar.gz


mv faad2-${FAAD2_VERSION} faad2
cd faad2

. bootstrap
emconfigure ./configure --enable-shared --disable-static --without-xmms --without-drm --without-mpeg4ip

cd libfaad
emmake make

echo "Compiled Successfully"


