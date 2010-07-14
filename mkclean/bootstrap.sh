#!/bin/sh
%%BEGIN
SCRIPT
%%END

make -s -f coremake.mak coremake

cpu_name=`uname -m | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
os_name=`uname -s | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
coremake_param="gcc_linux"

#echo "CPU" $cpu_name
#echo "OS" $os_name

case $os_name in
cygwin*)
  coremake_param="gcc_linux"
  ;;
mingw* | pw32*)
  coremake_param="gcc_win32"
  ;;
darwin* | rhapsody*)
    case $cpu_name in
        i*86)
            coremake_param="gcc_osx_x86"
            ;;
        powerpc*)
            coremake_param="gcc_osx_ppc"
            ;;
        x86_64)
            coremake_param="gcc_osx_x64"
            ;;
    esac
  ;;
freebsd* | kfreebsd*-gnu | dragonfly*)
  coremake_param="gcc_linux"
  ;;
linux*)
    case $cpu_name in
        i*86)
            coremake_param="gcc_linux"
            ;;
        x86_64)
            coremake_param="gcc_linux_x64"
            ;;
        powerpc*)
            coremake_param="gcc_linux_ppc"
            ;;
        arm*)
            coremake_param="gcc_linux_arm"
            ;;
        mips*)
            coremake_param="gcc_linux_mips"
            ;;
        sparc64)
            coremake_param="gcc_linux_sparc64"
            ;;
        sparc*)
            coremake_param="gcc_linux_sparc32"
            ;;
    esac
  ;;
esac


echo "Running ./coremake" $coremake_param
./coremake $coremake_param

echo "Now you can run make -C %(PROJECT_NAME)"