project("matroska2" VERSION 0.22.3)

option(CONFIG_BZLIB "Enable bzlib (de)compression in libmatroska2" ON)
option(CONFIG_LZO1X "Enable lzo (de)compression in libmatroska2" ON)

set(matroska2_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/matroskamain.c
  ${CMAKE_CURRENT_SOURCE_DIR}/matroskablock.c
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska_sem.c
)
set(matroska2_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska2/matroska.h
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska2/matroska_sem.h
  ${CMAKE_CURRENT_BINARY_DIR}/matroska2/matroska_config.h
)
set(matroska2_INTERNAL_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska2/matroska_internal.h
)

add_library("matroska2")
target_sources("matroska2" PRIVATE ${matroska2_SOURCES} ${matroska2_PUBLIC_HEADERS} ${matroska2_INTERNAL_HEADERS})
target_include_directories("matroska2" PUBLIC ".")

if (CONFIG_ZLIB)
  target_link_libraries("matroska2" PRIVATE "zlib")
endif(CONFIG_ZLIB)
if (CONFIG_LZO1X)
  add_subdirectory("minilzo")
  target_link_libraries("matroska2" PRIVATE "minilzo")
endif(CONFIG_LZO1X)
if (CONFIG_BZLIB)
  add_subdirectory("bzip2")
  target_link_libraries("matroska2" PRIVATE "bzlib")
endif(CONFIG_BZLIB)
if (NOT CONFIG_NOCODEC_HELPER)
  target_link_libraries("matroska2" PRIVATE "tremor")
endif(NOT CONFIG_NOCODEC_HELPER)
# set_target_properties("matroska2" PROPERTIES 
#   PUBLIC_HEADER "${matroska2_PUBLIC_HEADERS}"
# )
configure_file(matroska_config.h.in matroska2/matroska_config.h)
target_include_directories("matroska2" PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# the library needs to be static as it shares constants variables
target_link_libraries("matroska2" PUBLIC "ebml2")
if (BUILD_SHARED_LIBS)
  target_compile_definitions("matroska2" PUBLIC matroska2_IMPORTS)
endif (BUILD_SHARED_LIBS)

add_executable("mkvtree" test/mkvtree.c)
target_link_libraries("mkvtree" PRIVATE "matroska2" "ebml2" "corec")

if (0)
add_library("matroska2_haali" MatroskaParser/MatroskaParser.c)
target_include_directories("matroska2_haali" PUBLIC "." "MatroskaParser")
set_target_properties("matroska2_haali" PROPERTIES PUBLIC_HEADER "MatroskaParser/MatroskaParser.h")
target_link_libraries("matroska2_haali" PRIVATE "matroska2")
endif()
