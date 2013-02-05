#!/bin/sh

curl -O http://www.tortall.net/projects/yasm/releases/yasm-1.0.1.tar.gz
tar -xvzf yasm-1.0.1.tar.gz
pushd yasm-1.0.1
./configure
make
popd

# make a directory location for yasm to reside
mkdir -p base/usr/local/coremake/bin

#copy yasm to destination directory
cp yasm-1.0.1/yasm base/usr/local/coremake/bin/

#build the Xcode plugin
pushd YasmPlugin
xcodebuild -configuration "Release" build
popd

mkdir -p base/Developer/Library/Xcode/Plug-ins
cp -R YasmPlugin/build/Release/Yasm.xcplugin base/Developer/Library/Xcode/Plug-ins

#make the package
/Developer/usr/bin/packagemaker -v --doc "YasmPluginInstaller.pmdoc" -o YasmPlugin.pkg --target 10.5 -i com.CoreCodec.yasmplugin;