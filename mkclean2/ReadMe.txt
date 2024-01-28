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

mkclean can be built with minilzo support whic is GPL. In that case the
license of the binary becomes GPL.

** BUILDING **
To build mkclean you need CMake and a working C compiler.

You just run "cmake ." to generate the makefiles and then "make".

On Windows a Visual Studio project may be created instead.
