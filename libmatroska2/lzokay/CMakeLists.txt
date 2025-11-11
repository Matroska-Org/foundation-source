# lzokay library API ported to C only containing the decompressor

add_library("lzokay" STATIC)
set(LZOKAY_GROUP_PUBLIC_HEADERS
    lzokay.h
)
set(LZOKAY_GROUP_SOURCES lzokay.c)

target_sources("lzokay" PRIVATE ${LZOKAY_GROUP_SOURCES} ${LZOKAY_GROUP_PUBLIC_HEADERS})
target_include_directories("lzokay" PUBLIC ".")
set_target_properties("lzokay" PROPERTIES 
    PUBLIC_HEADER "${LZOKAY_GROUP_PUBLIC_HEADERS}"
)
