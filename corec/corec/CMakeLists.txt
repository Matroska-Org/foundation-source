project("corec" VERSION 1.0.0)

include(CheckIncludeFile)

option(CONFIG_STDIO "Build Core-C file API with stdio support" ON)
option(CONFIG_MULTITHREAD "Enable multithread support in Core-C" OFF)
option(CONFIG_DEBUGCHECKS "Add extra checks in Core-C objects/helpers in debug builds" ON)
option(CONFIG_DEBUG_LEAKS "Debug leaks in Core-C objects in debug builds" OFF)
option(CONFIG_SAFE_C "Notify unsafe C legacy calls" OFF)

set(CONFIG_UNICODE "best" CACHE STRING "Select the type of tchar_t (best, wchar, utf8)")
set_property(CACHE CONFIG_UNICODE PROPERTY STRINGS "best;utf8;wchar")
if(${CONFIG_UNICODE} STREQUAL "utf8")
  set(CONFIG_UNICODE_MODE CONFIG_UNICODE_UTF8)
elseif(${CONFIG_UNICODE} STREQUAL "wchar")
  set(CONFIG_UNICODE_MODE CONFIG_UNICODE_WCHAR)
else(${CONFIG_UNICODE} STREQUAL "best")
  set(CONFIG_UNICODE_MODE CONFIG_UNICODE_BEST)
endif()

check_include_file(stdint.h    HAVE_STDINT_H)

# Base core API
add_library("corec_bare" INTERFACE)
set(corec_base_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/corec.h
  ${CMAKE_CURRENT_SOURCE_DIR}/err.h
  ${CMAKE_CURRENT_SOURCE_DIR}/helper.h
  ${CMAKE_CURRENT_SOURCE_DIR}/memalloc.h
  ${CMAKE_CURRENT_SOURCE_DIR}/portab.h
  ${CMAKE_CURRENT_SOURCE_DIR}/confhelper.h
  ${CMAKE_CURRENT_SOURCE_DIR}/banned.h
  ${CMAKE_CURRENT_BINARY_DIR}/corec_config.h
)
target_sources("corec_bare" INTERFACE ${corec_base_PUBLIC_HEADERS})
target_include_directories("corec_bare" INTERFACE "..")
configure_file(corec_config.h.in corec_config.h)
target_include_directories("corec_bare" INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

# Node API
set(corec_node_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/node/node.c
  ${CMAKE_CURRENT_SOURCE_DIR}/node/nodetree.c
  # documentation installed with the sources
  ${CMAKE_CURRENT_SOURCE_DIR}/node/corec.html
)
set(corec_node_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/node/node.h
  ${CMAKE_CURRENT_SOURCE_DIR}/node/nodebase.h
  ${CMAKE_CURRENT_SOURCE_DIR}/node/nodetree.h
  ${CMAKE_CURRENT_SOURCE_DIR}/node/nodetools.h
)
add_library("corec_node" INTERFACE)
target_sources("corec_node" INTERFACE ${corec_node_SOURCES} ${corec_node_PUBLIC_HEADERS})
target_include_directories("corec_node" INTERFACE ".")
target_link_libraries("corec_node" INTERFACE "corec_array" "corec_str" "corec_file")
if(CONFIG_MULTITHREAD)
  target_link_libraries("corec_node" INTERFACE "corec_multithread")
endif(CONFIG_MULTITHREAD)

# Array API
set(corec_array_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/array/array.c
)
set(corec_array_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/array/array.h
)
add_library("corec_array" INTERFACE)
target_sources("corec_array" INTERFACE ${corec_array_SOURCES} ${corec_array_PUBLIC_HEADERS})
target_link_libraries("corec_array" INTERFACE "corec_bare")

# String API
add_library("corec_str" INTERFACE)
set(corec_str_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/str/str.c
)
set(corec_str_WIN32_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/str/str_win32.c
)
set(corec_str_OSX_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/str/str_osx.c
)
set(corec_str_LINUX_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/str/str_linux.c
)
set(corec_str_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/str/str.h
)
target_sources("corec_str" INTERFACE  ${corec_str_SOURCES} ${corec_str_PUBLIC_HEADERS})
if (WIN32)
  target_sources("corec_str" INTERFACE ${corec_str_WIN32_SOURCES})
elseif(APPLE)
    target_sources("corec_str" INTERFACE ${corec_str_OSX_SOURCES})
elseif(UNIX)
    target_sources("corec_str" INTERFACE ${corec_str_LINUX_SOURCES})
endif(WIN32)

set(corec_multithread_WIN32_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/multithread/multithread_win32.c
)
set(corec_multithread_UNIX_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/multithread/multithread_pthread.c
)
set(corec_multithread_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/multithread/multithread.h
)
add_library("corec_multithread" INTERFACE)
if (WIN32)
  target_sources("corec_multithread" INTERFACE ${corec_multithread_WIN32_SOURCES})
elseif(UNIX)
  target_sources("corec_multithread" INTERFACE ${corec_multithread_UNIX_SOURCES})
  target_link_libraries("corec_multithread" INTERFACE pthread)
endif(WIN32)

add_subdirectory("helpers")

add_library("corec")
target_include_directories("corec"
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/helpers>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
# keep until the sources are split
target_include_directories("corec" PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

set(corec_INTERNAL_LIBS
  $<BUILD_INTERFACE:corec_node> 
  $<BUILD_INTERFACE:corec_parser>
  $<BUILD_INTERFACE:corec_system>
  $<BUILD_INTERFACE:corec_md5>
)

if (BUILD_SHARED_LIBS)
  target_link_libraries("corec" PRIVATE ${corec_INTERNAL_LIBS})
  target_compile_definitions("corec" PUBLIC NODE_IMPORTS ARRAY_IMPORTS CHARCONVERT_IMPORTS DATE_IMPORTS FILE_IMPORTS MD5_IMPORTS CCSYSYEM_IMPORTS MULTITHREAD_IMPORTS STR_IMPORTS)
else(BUILD_SHARED_LIBS)
  target_link_libraries("corec" PUBLIC ${corec_INTERNAL_LIBS})
endif(BUILD_SHARED_LIBS)

set(corec_PUBLIC_HEADERS
  ${corec_node_PUBLIC_HEADERS} ${corec_parser_PUBLIC_HEADERS} ${corec_system_PUBLIC_HEADERS} ${core_md5_PUBLIC_HEADERS}
)

set_target_properties("corec" PROPERTIES
  PUBLIC_HEADER "${corec_PUBLIC_HEADERS}"
)

include(GNUInstallDirs)

install(TARGETS "corec"
  EXPORT Core-C
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# CMake file to import corec
install(EXPORT Core-C DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/corec)
