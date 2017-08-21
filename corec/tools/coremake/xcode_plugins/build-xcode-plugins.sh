#!/bin/sh

echo "This script is now preparing the xcode plugin..."

VERSION=1.3.0

curl -O http://www.tortall.net/projects/yasm/releases/yasm-$VERSION.tar.gz
tar -xvzf yasm-$VERSION.tar.gz
pushd yasm-$VERSION
./configure
make
popd

echo "Now compiling the Xcode plugin itself"

#build the Xcode plugin
pushd XcodePlugins
xcodebuild -configuration "Release" build
popd

echo "Installing the plugin and required files. You might be prompted for your password to copy the files."

mkdir -p "$HOME/Library/Application Support/Developer/Shared/Xcode/Plug-ins"
# copy the plugin to the user folder
cp -R XcodePlugins/build/Release/CoreMakeXcode.xcplugin "$HOME/Library/Application Support/Developer/Shared/Xcode/Plug-ins"

sudo mkdir -p "/usr/local/coremake/bin/"
#copy gas-preprocessor to destination directory
sudo cp XcodePlugins/gas-preprocessor.pl "/usr/local/coremake/bin/gas-preprocessor.pl"
#copy yasm to destination directory
sudo cp yasm-$VERSION/yasm "/usr/local/coremake/bin/yasm"


# now make the plugin compatible with the current xcode
# read DVTPlugInCompatibilityUUID from the installed Xcode version
UUID=$(defaults read /Applications/Xcode.app/Contents/Info DVTPlugInCompatibilityUUID)

# try to find this UUID in the plugin Info.plist
UUID_IN_PLUGIN=$(defaults read $HOME/Library/Application\ Support/Developer/Shared/Xcode/Plug-ins/CoreMakeXcode.xcplugin/Contents/Info DVTPlugInCompatibilityUUIDs | grep $UUID)

echo "Xcode UUID: $UUID"

# if the result is not empty, add the UUID
if [ -z "$UUID_IN_PLUGIN" ]; then
    echo "Plugin is not yet compatible with this version of Xcode..."
    defaults write $HOME/Library/Application\ Support/Developer/Shared/Xcode/Plug-ins/CoreMakeXcode.xcplugin/Contents/Info DVTPlugInCompatibilityUUIDs -array-add $UUID
    echo "Plugin is now compatible.\n"
else
    echo "Plugin was already compatible with this version"
      
fi

echo " "
echo " "
echo " "
echo "  ********************************"
echo "  * DONE: PLEASE RESTART XCODE ! *"
echo "  ********************************"
echo " "
echo "  A dialog should appear and ask if you want to load an unknown bundle. Click on the load button and you should be done!"
echo " "
echo " "

