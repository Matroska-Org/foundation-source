#!/bin/sh

make -s -f coremake.mak coremake

echo "Running ./coremake gcc_linux"
./coremake gcc_linux

echo "Now you can run make -C %(PROJECT_NAME)"