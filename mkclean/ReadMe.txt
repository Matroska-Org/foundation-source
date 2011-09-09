mkclean is a command line tool to clean and optimize Matroska files that have
already been muxed.

It currently reorders elements in an optimum way for local and web use.
It also removes elements not found in the Matroska specs and the extra Meta 
Seek list of Clusters that some program add to their file.

** LICENSE **
The program is licensed with the BSD license. So you can modify it in any way
you want. However we ask that you generously give back your enhancements
to the community if you think it can help. Contact: slhomme@matroska.org

mkclean depends on libebml2 (BSD) and libmatroska2 (BSD) that themselves
rely on Core-C (BSD) which adds an object API on top of the C language.

** BUILDING **
To build mkclean you have to build coremake first:
You can either run ./configure on Linux/OS X or
- Just compile <root>/corec/tools/coremake/coremake.c with your C compiler.
- run coremake from the root of the source :
  * "coremake.exe vs9_win32" to generate the project files for Visual Studio 2008
  * "coremake gcc_linux" to generate the project files for GNU make building with gcc
  * "coremake gcc_linux_x64" for GNU make building with gcc on Linux 64 bits
  * "coremake xcode_uni" to generate the project files for XCode for a universal OS X binary
  * look in the coremake folder for all the platforms supported
  
The Makefile/mkclean.sln will end up in the mkclean folder. You just have to
build from there and it will create mkclean(.exe) under <root>/release/

To delete all the project files run "coremake(.exe) clean" from the root of the sources.
