1/ To build these sources you first need to build coremake:

* for UNIX + gcc:
gcc corec/tools/coremake/coremake.c -o coremake

* for Windows in a Visual Studio shell:
cl corec/tools/coremake/coremake.c -o coremake.exe


2/ Generate the makefiles

* for gcc on Linux
./coremake gcc_linux
or
./coremake gcc_linux_x64

* for gcc on Mingw64
./coremake gcc_win64

* for Visual Studio on Windows
./coremake vs9_x64


3a/ build the spec tools

make -C spectool

the result executables will be in <root>/release/<gcc_win64> with <gcc_win64> being the name of the
coremake target you used on step 2.

3b/ build mkvalidator

make -C mkvalidator

the result executables will be in <root>/release/<gcc_win64> with <gcc_win64> being the name of the
coremake target you used on step 2.


3c/ build mkclean

make -C mkclean

the result executables will be in <root>/release/<gcc_win64> with <gcc_win64> being the name of the
coremake target you used on step 2.

