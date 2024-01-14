mkvalidator is a command line tool to verify Matroska files for spec conformance.

It currently checks for missing mandatory element, EBML "junk" data (use of 
unknown IDs), bad track or cues (index) entries, and profile violation (like
a v1 file using v2 features).

** LICENSE **
The program is licensed with the BSD license. So you can modify it in any way
you want. However we ask that you generously give back your enhancements
to the community if you think it can help. Contact: slhomme@matroska.org

mkvalidator depends on libebml2 (BSD) and libmatroska2 (BSD) that themselves
rely on Core-C (BSD) which adds an object API on top of the C language.

mkvalidator can be built with minilzo support whic is GPL. In that case the
license of the binary becomes GPL.

** BUILDING **
To build mkvalidator you need CMake and a working C compiler.

You just run "cmake ." to generate the makefiles and then "make".

On Windows a Visual Studio project may be created instead.
