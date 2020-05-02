project("ebml2" VERSION 0.21.3)

option(CONFIG_EBML_WRITING "Build libebml2 with write support" ON)

set(ebml2_group_SOURCES
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
set(ebml2_group_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/ebml/ebml.h
  ${CMAKE_CURRENT_BINARY_DIR}/ebml/ebml_config.h
)
set(ebml2_group_INTERNAL_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/ebml/ebml_internal.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ebmlcrc.h
)
add_library("ebml2_group" INTERFACE)
target_sources("ebml2_group" INTERFACE 
  ${ebml2_group_SOURCES} 
  ${ebml2_group_PUBLIC_HEADERS} 
  ${ebml2_group_INTERNAL_HEADERS}
)
target_link_libraries("ebml2_group" INTERFACE "corec_node")
target_compile_definitions("ebml2_group" INTERFACE HAS_EBML2)
target_include_directories("ebml2_group" INTERFACE ".")
configure_file(ebml_config.h.in ebml/ebml_config.h)
target_include_directories("ebml2_group" INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
if(${CMAKE_VERSION}` VERSION_GREATER_EQUAL "3.15.0")
  set_target_properties("ebml2_group" PROPERTIES 
    PUBLIC_HEADER "${ebml2_group_PUBLIC_HEADERS}"
  )
endif()

# the library needs to be static as it shares constants variables
add_library("ebml2" STATIC)
target_link_libraries("ebml2" PUBLIC "ebml2_group")

# TODO fix build
add_executable("ebmltree" test/ebmltree.c)
target_link_libraries("ebmltree" PUBLIC "ebml2" "corec_parser")

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
#  ${ebml2_group_SOURCES}
#  ${LEGACY_LIBEBML_PUBLIC_HEADERS}
#)
# target_compile_definitions("ebml2_legacy" PUBLIC EBML_STRICT_API)
# target_compile_definitions("ebml2_legacy" PRIVATE EBML_LEGACY_API)
# target_link_libraries("ebml2_legacy" PUBLIC "ebml2")
# target_include_directories("ebml2_legacy" PUBLIC "legacy")
# set_target_properties("corec_node" PROPERTIES PUBLIC_HEADER 
#  "${LEGACY_LIBEBML_PUBLIC_HEADERS}"
#)

