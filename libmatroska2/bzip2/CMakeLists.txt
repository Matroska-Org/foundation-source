# bzlib library API

add_library("bzlib" STATIC)
set(BZLIB_GROUP_PUBLIC_HEADERS
    bzlib.h
)
set(BZLIB_GROUP_SOURCES 
    bzlib.c
    crctable.c
    decompress.c
    compress.c
    randtable.c
    huffman.c
    blocksort.c
)

target_sources("bzlib" PRIVATE ${BZLIB_GROUP_SOURCES} ${BZLIB_GROUP_PUBLIC_HEADERS} bzlib_private.h)
target_include_directories("bzlib" PUBLIC ".")
set_target_properties("bzlib" PROPERTIES 
    PUBLIC_HEADER "${BZLIB_GROUP_PUBLIC_HEADERS}"
)
