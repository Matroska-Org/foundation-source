project("ebml2" VERSION 0.21.3)

option(CONFIG_EBML_WRITING "Build libebml2 with write support" ON)

set(ebml2_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmlmain.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmlbinary.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmlcrc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmldate.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmlelement.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmlmaster.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmlnumber.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmlstring.c
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmlvoid.c
)
set(ebml2_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/ebml2/ebml.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ebml2/ebml_classes.h
  ${CMAKE_CURRENT_BINARY_DIR}/ebml2/ebml_config.h
)
set(ebml2_INTERNAL_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/internal.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmlcrc.h
)
add_library("ebml2")
target_sources("ebml2" PRIVATE 
  ${ebml2_SOURCES} 
  ${ebml2_PUBLIC_HEADERS} 
  ${ebml2_INTERNAL_HEADERS}
)
target_compile_definitions("ebml2" PRIVATE EBML2_UGLY_HACKS_API)
target_compile_definitions("ebml2" PUBLIC HAS_EBML2)
configure_file(ebml_config.h.in ebml2/ebml_config.h)
set_target_properties("ebml2" PROPERTIES 
  PUBLIC_HEADER "${ebml2_PUBLIC_HEADERS}"
)

# the library needs to be static as it shares constants variables
target_link_libraries("ebml2" PUBLIC "corec")

target_include_directories("ebml2"
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
# keep until the sources are split
target_include_directories("ebml2" PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)


# we can't export EBML as a DLL for now as it doesn't seem possible to use dllimport to initialize global (startup) variables
if (BUILD_SHARED_LIBS)
  target_compile_definitions("ebml2" PUBLIC ebml2_IMPORTS)
endif (BUILD_SHARED_LIBS)

include(GNUInstallDirs)

install(TARGETS "ebml2" 
  EXPORT Ebml2
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ebml2
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# CMake file to import libebml2
install(EXPORT Ebml2 DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ebml2)


# TODO fix build
add_executable("ebmltree" test/ebmltree.c)
target_link_libraries("ebmltree" PUBLIC "ebml2")

# TODO finish this
# configure_file(legacy/ebml2_legacy_project.h.in legacy/ebml2_legacy_project.h)
# set(LEGACY_LIBEBML_PUBLIC_HEADERS
#   legacy/ebml/EbmlBinary.h
#   legacy/ebml/EbmlConfig.h
#   legacy/ebml/EbmlContexts.h
#   legacy/ebml/EbmlCrc32.h
#   legacy/ebml/EbmlDate.h
#   legacy/ebml/EbmlDummy.h
#   legacy/ebml/EbmlElement.h
#   legacy/ebml/EbmlFloat.h
#   legacy/ebml/EbmlHead.h
#   legacy/ebml/EbmlId.h
#   legacy/ebml/EbmlMaster.h
#   legacy/ebml/EbmlSInteger.h
#   legacy/ebml/EbmlStream.h
#   legacy/ebml/EbmlString.h
#   legacy/ebml/EbmlSubHead.h
#   legacy/ebml/EbmlTypes.h
#   legacy/ebml/EbmlUInteger.h
#   legacy/ebml/EbmlUnicodeString.h
#   legacy/ebml/EbmlVersion.h
#   legacy/ebml/EbmlVoid.h
#   legacy/ebml/IOCallback.h
#   legacy/ebml/c/libebml_t.h
# )
#add_library("ebml2_legacy" 
#  legacy/legacy.cpp
#  ${ebml2_SOURCES}
#  ${LEGACY_LIBEBML_PUBLIC_HEADERS}
#)
# target_compile_definitions("ebml2_legacy" PUBLIC EBML_STRICT_API)
# target_compile_definitions("ebml2_legacy" PRIVATE EBML_LEGACY_API)
# target_link_libraries("ebml2_legacy" PUBLIC "ebml2")
# target_include_directories("ebml2_legacy" PUBLIC "legacy")

