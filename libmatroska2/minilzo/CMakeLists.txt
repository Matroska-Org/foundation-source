# minilzo library API

add_library("minilzo" STATIC)
set(MINILZO_GROUP_PUBLIC_HEADERS
    minilzo.h
    lzoconf.h
    lzodefs.h
)
set(MINILZO_GROUP_SOURCES minilzo.c)

target_sources("minilzo" PRIVATE ${MINILZO_GROUP_SOURCES} ${MINILZO_GROUP_PUBLIC_HEADERS})
target_include_directories("minilzo" PUBLIC ".")
set_target_properties("minilzo" PROPERTIES 
    PUBLIC_HEADER "${MINILZO_GROUP_PUBLIC_HEADERS}"
)

add_executable("testmini" testmini.c)
target_link_libraries("testmini" PRIVATE "minilzo")
