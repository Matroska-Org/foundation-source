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
add_library("corec_file" INTERFACE)
target_sources("corec_file" INTERFACE ${corec_file_BASE_SOURCES} ${corec_file_PUBLIC_HEADERS})
if (CONFIG_STDIO)
  target_sources("corec_file" INTERFACE ${corec_file_STDIO_SOURCES})
endif(CONFIG_STDIO)
if (WIN32)
  target_sources("corec_file" INTERFACE ${corec_file_WIN32_SOURCES})

  # for SHFileOperation to move files in the recycle bin
  target_link_libraries("corec_file" INTERFACE shell32)
elseif(UNIX)
  target_sources("corec_file" INTERFACE ${corec_file_UNIX_SOURCES})
endif(WIN32)
target_link_libraries("corec_file" INTERFACE "corec_node" "corec_date")

# Parser API
set(corec_parser_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/buffer.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/dataheap.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/hotkey.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/nodelookup.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser2.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/strtab.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/strtypes.c
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/urlpart.c
)
set(corec_parser_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/dataheap.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/hotkey.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/nodelookup.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/strtab.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/strtypes.h
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/urlpart.h
  PARENT_SCOPE
)
add_library("corec_parser" INTERFACE)
target_sources("corec_parser" INTERFACE ${corec_parser_SOURCES} ${corec_parser_PUBLIC_HEADERS})
target_link_libraries("corec_parser" INTERFACE "corec_str" "corec_charconvert" "corec_file")
if (CYGWIN)
# if CMake wasn't this piece of shit this would simply work
  target_link_libraries("corec_parser" INTERFACE iconv)
endif (CYGWIN)
target_include_directories("corec_parser" INTERFACE ".")

# Date API
add_library("corec_date" INTERFACE)
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
if (WIN32)
  target_sources("corec_date" INTERFACE ${corec_date_WIN32_SOURCES})
elseif(UNIX)
    target_sources("corec_date" INTERFACE ${corec_date_UNIX_SOURCES})
endif(WIN32)
target_sources("corec_date" INTERFACE ${corec_date_PUBLIC_HEADERS})
target_link_libraries("corec_date" INTERFACE "corec_bare")

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
add_library("corec_charconvert" INTERFACE)
if (WIN32)
  target_sources("corec_charconvert" INTERFACE ${corec_charconvert_WIN32_SOURCES})
elseif(APPLE)
  target_link_libraries("corec_charconvert" INTERFACE "-framework CoreFoundation")
  target_sources("corec_charconvert" INTERFACE ${corec_charconvert_OSX_SOURCES})
elseif(UNIX)
  target_sources("corec_charconvert" INTERFACE ${corec_charconvert_LINUX_SOURCES})
else(UNIX)
  target_sources("corec_charconvert" INTERFACE ${corec_charconvert_OTHER_SOURCES})
  if (CYGWIN)
    target_link_libraries("corec_charconvert" INTERFACE iconv)
  endif (CYGWIN)
endif(WIN32)
target_link_libraries("corec_charconvert" INTERFACE "corec_bare")
