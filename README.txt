libEBML2, libMatroska2, mkvalidator, mkclean and the specifications

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


3a/ build mkvalidator

make -C mkvalidator

the result executables will be in <root>/release/<gcc_win64> with <gcc_win64> being the name of the
coremake target you used on step 2.


3b/ build mkclean

make -C mkclean

the result executables will be in <root>/release/<gcc_win64> with <gcc_win64> being the name of the
coremake target you used on step 2.

## Code of conduct

Please note that this project is released with a Contributor Code of Conduct. By participating in this project you agree to abide by its terms.
