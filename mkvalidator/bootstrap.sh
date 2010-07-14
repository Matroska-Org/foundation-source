#!/bin/sh
%%BEGIN
SCRIPT
%%END

make -s -f coremake.mak coremake

cpu_name=`uname -m | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
os_name=`uname -s | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`

echo "CPU" $cpu_name
echo "OS" $os_name

echo "Running ./coremake gcc_linux"
./coremake gcc_linux

echo "Now you can run make -C %(PROJECT_NAME)"