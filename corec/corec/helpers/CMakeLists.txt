# File API
set(corec_file_BASE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/file/bufstream.c
  ${CMAKE_CURRENT_SOURCE_DIR}/file/memstream.c
  ${CMAKE_CURRENT_SOURCE_DIR}/file/streams.c
  ${CMAKE_CURRENT_SOURCE_DIR}/file/tools.c
)
set(corec_file_STDIO_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/file/stream_stdio.c
)
set(corec_file_WIN32_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/file/file_win32.c
)
set(corec_file_UNIX_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/file/file_libc.c
)
set(corec_file_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/file/file.h
  ${CMAKE_CURRENT_SOURCE_DIR}/file/streams.h
  PARENT_SCOPE
)
add_library("corec_file" ${corec_file_BASE_SOURCES} ${corec_file_PUBLIC_HEADERS})
if (CONFIG_STDIO)
  target_sources("corec_file" PRIVATE ${corec_file_STDIO_SOURCES})
endif(CONFIG_STDIO)
if (WIN32)
  target_sources("corec_file" PRIVATE ${corec_file_WIN32_SOURCES})

  # for SHFileOperation to move files in the recycle bin
  target_link_libraries("corec_file" PRIVATE shell32)
elseif(UNIX)
  check_include_file(sys/mount.h   HAVE_SYS_MOUNT_H)
  check_include_file(sys/vfs.h     HAVE_SYS_VFS_H)
  check_include_file(sys/statvfs.h HAVE_SYS_STATVFS_H)
  if (HAVE_SYS_MOUNT_H)
    target_compile_definitions("corec_file" PRIVATE HAVE_SYS_MOUNT_H)
  endif()
  if (HAVE_SYS_VFS_H)
    target_compile_definitions("corec_file" PRIVATE HAVE_SYS_VFS_H)
  endif()
  if (HAVE_SYS_STATVFS_H)
    target_compile_definitions("corec_file" PRIVATE HAVE_SYS_STATVFS_H)
  endif()
  target_sources("corec_file" PRIVATE ${corec_file_UNIX_SOURCES})
endif(WIN32)
target_include_directories("corec_file" PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
target_include_directories("corec_file" PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)

# Parser API
set(corec_parser_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/buffer.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/hotkey.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/nodelookup.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser2.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/strtab.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/strtypes.c
)
set(corec_parser_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/hotkey.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/nodelookup.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/strtab.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/strtypes.h
  PARENT_SCOPE
)
add_library("corec_parser" ${corec_parser_SOURCES} ${corec_parser_PUBLIC_HEADERS})
target_include_directories("corec_parser" PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
target_include_directories("corec_parser" PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)
if (CYGWIN)
# if CMake wasn't this piece of shit this would simply work
  target_link_libraries("corec_parser" PRIVATE iconv)
endif (CYGWIN)
target_include_directories("corec_parser" PUBLIC ".")

# Date API
set(corec_date_WIN32_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/date/date_win32.c
)
set(corec_date_UNIX_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/date/date_libc.c
)
set(corec_date_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/date/date.h
  PARENT_SCOPE
)
add_library("corec_date" ${corec_date_PUBLIC_HEADERS} ${corec_base_PUBLIC_HEADERS})
if (WIN32)
  target_sources("corec_date" PRIVATE ${corec_date_WIN32_SOURCES})
elseif(UNIX)
    target_sources("corec_date" PRIVATE ${corec_date_UNIX_SOURCES})
endif(WIN32)
target_include_directories("corec_date" PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
target_include_directories("corec_date" PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)

# Character Conversion API
set(corec_charconvert_WIN32_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/charconvert/charconvert_win32.c
)
set(corec_charconvert_OSX_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/charconvert/charconvert_osx.c
)
set(corec_charconvert_LINUX_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/charconvert/charconvert_linux.c
)
set(corec_charconvert_OTHER_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/charconvert/charconvert_utf8.c
)
set(corec_charconvert_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/charconvert/charconvert.h
  PARENT_SCOPE
)
add_library("corec_charconvert" ${corec_charconvert_PUBLIC_HEADERS} ${corec_base_PUBLIC_HEADERS})
if (WIN32)
  target_sources("corec_charconvert" PRIVATE ${corec_charconvert_WIN32_SOURCES})
elseif(APPLE)
  target_link_libraries("corec_charconvert" PRIVATE "-framework CoreFoundation")
  target_sources("corec_charconvert" PRIVATE ${corec_charconvert_OSX_SOURCES})
elseif(UNIX)
  target_sources("corec_charconvert" PRIVATE ${corec_charconvert_LINUX_SOURCES})
else(UNIX)
  target_sources("corec_charconvert" PRIVATE ${corec_charconvert_OTHER_SOURCES})
  if (CYGWIN)
    target_link_libraries("corec_charconvert" PRIVATE iconv)
  endif (CYGWIN)
endif(WIN32)
target_include_directories("corec_charconvert" PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
target_include_directories("corec_charconvert" PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)
