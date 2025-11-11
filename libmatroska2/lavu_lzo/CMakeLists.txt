# libavutil lzo decompressor library API

add_library("lavu_lzo" STATIC)
set(MINILZO_GROUP_PUBLIC_HEADERS
    lzo.h
)
set(MINILZO_GROUP_SOURCES lzo.c)

target_sources("lavu_lzo" PRIVATE ${MINILZO_GROUP_SOURCES} ${MINILZO_GROUP_PUBLIC_HEADERS})
target_include_directories("lavu_lzo" PUBLIC ".")
set_target_properties("lavu_lzo" PROPERTIES 
    PUBLIC_HEADER "${MINILZO_GROUP_PUBLIC_HEADERS}"
)
