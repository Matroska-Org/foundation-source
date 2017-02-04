ifeq ($(OS),Windows_NT)
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CC := x86_64-pc-msys-gcc
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CC := x86_64-pc-msys-gcc
        else
            CC := i686-pc-msys-gcc
        endif
    endif
endif

all: coremake corerc
# make has all the default built-ins to build this for the current system

install: coremake corerc
	mkdir -p /usr/local/bin
	install -c coremake /usr/local/bin/coremake
	install -c corerc /usr/local/bin/corerc
	for b in `ls *.build *.inc`; do install -c -m 644 -D "$$b" "/usr/local/share/coremake/$$b"; done
