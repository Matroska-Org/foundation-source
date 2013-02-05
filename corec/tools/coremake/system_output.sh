#!/bin/sh

# output the right configuration (.build file) to use for the current OS

cpu_name=`uname -m | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
os_name=`uname -s | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
cpu_type=`echo $HOSTTYPE`
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
    case $cpu_type in
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
linux* | freebsd* | kfreebsd*-gnu | dragonfly*)
    case $cpu_name in
        i*86)
            coremake_param="gcc_linux"
            ;;
        x86_64 | amd64)
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

echo $coremake_param
exit
