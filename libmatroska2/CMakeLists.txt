project("matroska2" VERSION 0.22.3)

option(CONFIG_BZLIB "Enable bzlib (de)compression in libmatroska2" ON)
option(CONFIG_LZO1X "Enable lzo (de)compression in libmatroska2" ON)
option(CONFIG_NOCODEC_HELPER "Enable Vorbis frame durations in libmatroska2" ON)

set(matroska2_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/matroskamain.c
  ${CMAKE_CURRENT_SOURCE_DIR}/matroskablock.c
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska_sem.c
)
set(matroska2_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska2/matroska.h
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska2/matroska_classes.h
  ${CMAKE_CURRENT_SOURCE_DIR}/matroska2/matroska_sem.h
  ${CMAKE_CURRENT_BINARY_DIR}/matroska2/matroska_config.h
)

add_library("matroska2")
target_sources("matroska2" PRIVATE ${matroska2_SOURCES} ${matroska2_PUBLIC_HEADERS})

target_include_directories("matroska2"
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
# keep until the sources are split
target_include_directories("matroska2" PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

if (CONFIG_ZLIB)
  target_link_libraries("matroska2" PRIVATE $<BUILD_INTERFACE:zlib>)
endif(CONFIG_ZLIB)
if (CONFIG_LZO1X)
  add_subdirectory("minilzo")
  target_link_libraries("matroska2" PRIVATE $<BUILD_INTERFACE:minilzo>)
endif(CONFIG_LZO1X)
if (CONFIG_BZLIB)
  add_subdirectory("bzip2")
  target_link_libraries("matroska2" PRIVATE $<BUILD_INTERFACE:bzlib>)
endif(CONFIG_BZLIB)
if (NOT CONFIG_NOCODEC_HELPER)
  add_subdirectory("tremor")
  target_link_libraries("matroska2" PRIVATE $<BUILD_INTERFACE:tremor>)
endif(NOT CONFIG_NOCODEC_HELPER)

set_target_properties("matroska2" PROPERTIES 
  PUBLIC_HEADER "${matroska2_PUBLIC_HEADERS}"
)
configure_file(matroska_config.h.in matroska2/matroska_config.h)

# the library needs to be static as it shares constants variables
target_link_libraries("matroska2" PUBLIC "ebml2")
if (BUILD_SHARED_LIBS)
  target_compile_definitions("matroska2" PUBLIC matroska2_IMPORTS)
endif (BUILD_SHARED_LIBS)

include(GNUInstallDirs)

install(TARGETS "matroska2" 
  EXPORT Matroska2
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/matroska2
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# CMake file to import libmatroska2
install(EXPORT Matroska2 DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/matroska2)

add_executable("mkvtree" test/mkvtree.c)
target_link_libraries("mkvtree" PRIVATE "matroska2" "ebml2" "corec")

if (0)
add_library("matroska2_haali" MatroskaParser/MatroskaParser.c)
target_include_directories("matroska2_haali" PUBLIC "." "MatroskaParser")
set_target_properties("matroska2_haali" PROPERTIES PUBLIC_HEADER "MatroskaParser/MatroskaParser.h")
target_link_libraries("matroska2_haali" PRIVATE "matroska2")
endif()
