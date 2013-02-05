#!/bin/sh
make -f build.mak
sudo mv -f coremake /usr/local/bin/coremake
sudo mv -f corerc /usr/local/bin/corerc

#sudo gcc -O2 coremake.c -o /usr/local/bin/coremake
#sudo gcc -O2 corerc.c -o /usr/local/bin/corerc
