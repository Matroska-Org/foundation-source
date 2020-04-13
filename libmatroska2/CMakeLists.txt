project("matroska2" VERSION 0.22.3)

option(CONFIG_BZLIB "Enable bzlib (de)compression in libmatroska2" ON)
option(CONFIG_LZO1X "Enable lzo (de)compression in libmatroska2" ON)

set(matroska2_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/matroskamain.c
  ${CMAKE_CURRENT_SOURCE_DIR}/matroskablock.c
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska_sem.c
)
set(matroska2_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska/matroska.h
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska/matroska_sem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska/matroska_internal.h
)

add_library("matroska2_group" INTERFACE)
target_sources("matroska2_group" INTERFACE ${matroska2_SOURCES} ${matroska2_PUBLIC_HEADERS})
target_include_directories("matroska2_group" INTERFACE ".")
target_compile_definitions("matroska2_group" INTERFACE HAS_MATROSKA2)
target_link_libraries("matroska2_group" INTERFACE "ebml2_group")
configure_file(matroska2_project.h.in matroska2_project.h)
target_include_directories("matroska2_group" INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

if (CONFIG_ZLIB)
  target_link_libraries("matroska2_group" INTERFACE "zlib")
endif(CONFIG_ZLIB)
if (CONFIG_LZO1X)
  add_subdirectory("minilzo")
  target_link_libraries("matroska2_group" INTERFACE "minilzo")
endif(CONFIG_LZO1X)
if (CONFIG_BZLIB)
  add_subdirectory("bzip2")
  target_link_libraries("matroska2_group" INTERFACE "bzlib")
endif(CONFIG_BZLIB)
if (CONFIG_CODEC_HELPER)
  target_link_libraries("matroska2_group" INTERFACE "tremor")
endif(CONFIG_CODEC_HELPER)
# set_target_properties("matroska2_group" PROPERTIES 
#   PUBLIC_HEADER "${matroska2_PUBLIC_HEADERS}"
# )

# the library needs to be static as it shares constants variables
add_library("matroska2" STATIC)
target_link_libraries("matroska2" PRIVATE "matroska2_group")
target_include_directories("matroska2" PUBLIC ".")
# TODO dirty until we use installed headers
target_include_directories("matroska2" PUBLIC "../libebml2")
target_compile_definitions("matroska2" PRIVATE MATROSKA_LIBRARY)
configure_file(config.h.in config.h)
target_include_directories("matroska2" PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_executable("mkvtree" test/mkvtree.c)
target_link_libraries("mkvtree" PRIVATE "matroska2_group" "corec_parser")

if (0)
add_library("matroska2_haali" MatroskaParser/MatroskaParser.c)
target_include_directories("matroska2_haali" PUBLIC "." "MatroskaParser")
set_target_properties("matroska2_haali" PROPERTIES PUBLIC_HEADER "MatroskaParser/MatroskaParser.h")
target_link_libraries("matroska2_haali" PRIVATE "matroska2_group")
endif()
