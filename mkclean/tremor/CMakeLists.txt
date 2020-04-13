# tremor library API

add_library("tremor")
set(TREMOR_GROUP_PUBLIC_HEADERS
    ivorbiscodec.h
    codec_internal.h
    misc.h
)
set(TREMOR_GROUP_SOURCES 
    block.c
    bitwise.c
    codebook.c
    floor0.c
    floor1.c
    info.c
    mapping0.c
    mdct.c
    registry.c
    res012.c
    sharedbook.c
    window.c
)

target_sources("tremor" PRIVATE ${TREMOR_GROUP_SOURCES} ${TREMOR_GROUP_PUBLIC_HEADERS})
target_include_directories("tremor" PUBLIC ".")
set_target_properties("tremor" PROPERTIES 
    PUBLIC_HEADER "${TREMOR_GROUP_PUBLIC_HEADERS}"
)
