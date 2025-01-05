project("matroska2" VERSION 0.22.3 LANGUAGES C)

option(CONFIG_ZLIB "Enable zlib (de)compression" ON)
option(CONFIG_BZLIB "Enable bzlib decompression in libmatroska2" ON)
option(CONFIG_LZO1X "Enable lzo decompression in libmatroska2" ON)
option(CONFIG_CODEC_HELPER "Enable Vorbis frame durations in libmatroska2" ON)

if (CONFIG_ZLIB)
  include(FindZLIB)

  if(NOT ZLIB_FOUND)
    include(FetchContent)

    set(ZLIB_BUILD_EXAMPLES OFF)

    FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG 51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf # 1.3.1 tag
        PATCH_COMMAND git apply "${CMAKE_CURRENT_SOURCE_DIR}/zlib/0001-CMake-don-t-build-dynamic-zlib.patch"
    )

    FetchContent_MakeAvailable(zlib)
    target_compile_definitions("zlibstatic" PRIVATE NO_GZIP)
  endif()
endif()

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
  if(ZLIB_FOUND)
    target_include_directories("matroska2" PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries("matroska2" PRIVATE ${ZLIB_LIBRARIES})
  else()
    target_link_libraries("matroska2" PRIVATE $<BUILD_INTERFACE:zlibstatic>)
  endif()
endif(CONFIG_ZLIB)

if (CONFIG_LZO1X)
  add_subdirectory("minilzo")
  target_link_libraries("matroska2" PRIVATE $<BUILD_INTERFACE:minilzo>)
endif(CONFIG_LZO1X)

if (CONFIG_BZLIB)
  find_package (BZip2)
  if(BZIP2_FOUND)
    target_include_directories("matroska2" PRIVATE ${BZIP2_INCLUDE_DIRS})
    target_link_libraries("matroska2" PRIVATE ${BZIP2_LIBRARIES})
  else()
    add_subdirectory("bzip2")
    target_link_libraries("matroska2" PRIVATE $<BUILD_INTERFACE:bzlib>)
  endif()
endif(CONFIG_BZLIB)

if (CONFIG_CODEC_HELPER)
  add_subdirectory("tremor")
  target_link_libraries("matroska2" PRIVATE $<BUILD_INTERFACE:tremor>)
endif(CONFIG_CODEC_HELPER)

set_target_properties("matroska2" PROPERTIES
  PUBLIC_HEADER "${matroska2_PUBLIC_HEADERS}"
  C_STANDARD 11
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
