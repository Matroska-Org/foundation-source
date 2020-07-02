//#define COREMAKE_STATIC
//#define COREMAKE_UNICODE
#define CONFIG_EBML_WRITING
#define CONFIG_EBML_UNICODE
#define HAVE_EBML2
#define CONFIG_ZLIB
#define CONFIG_MATROSKA2
//#define USE_PRECOMPILED_HEADERS
//#define CONFIG_DEBUGCHECKS
#define CONFIG_STDIO
#define CONFIG_FILEPOS_64
#define COREMAKE_CONFIG_HELPER
//#define CONFIG_DEBUG_LEAKS

#ifndef COREMAKE_STATIC
#ifdef _USE_DLL
#define STR_IMPORTS
#define ARRAY_IMPORTS
#define DATE_IMPORTS
#define FILE_IMPORTS
#define EBML2_IMPORTS
#define MATROSKA2_IMPORTS
#define MD5_IMPORTS
#define NODE_IMPORTS
#else
#define STR_EXPORTS
#define ARRAY_EXPORTS
#define DATE_EXPORTS
#define FILE_EXPORTS
#define EBML2_EXPORTS
#define MATROSKA2_EXPORTS
#define MD5_EXPORTS
#define NODE_EXPORTS
#endif
#endif

#if defined(_WIN32) || defined(TARGET_SYMBIAN)
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#define DLLHIDDEN
#elif defined(__GNUC__) && ((__GNUC__*100 + __GNUC_MINOR__) >= 340)
#define DLLEXPORT __attribute__ ((visibility("default")))
#define DLLIMPORT __attribute__ ((visibility("default")))
#define DLLHIDDEN __attribute__ ((visibility("hidden")))
#else
#define DLLEXPORT
#define DLLIMPORT
#define DLLHIDDEN
#endif

#if defined(NODE_EXPORTS)
#define NODE_DLL DLLEXPORT
#elif defined(NODE_IMPORTS)
#define NODE_DLL DLLIMPORT
#else
#define NODE_DLL
#endif

#if defined(MATROSKA2_EXPORTS)
#define MATROSKA_DLL DLLEXPORT
#elif defined(MATROSKA2_IMPORTS)
#define MATROSKA_DLL DLLIMPORT
#else
#define MATROSKA_DLL
#endif

#if defined(ARRAY_EXPORTS)
#define ARRAY_DLL DLLEXPORT
#elif defined(ARRAY_IMPORTS)
#define ARRAY_DLL DLLIMPORT
#else
#define ARRAY_DLL
#endif

#if defined(STR_EXPORTS)
#define STR_DLL DLLEXPORT
#elif defined(STR_IMPORTS)
#define STR_DLL DLLIMPORT
#else
#define STR_DLL
#endif

#if defined(DATE_EXPORTS)
#define DATE_DLL DLLEXPORT
#elif defined(DATE_IMPORTS)
#define DATE_DLL DLLIMPORT
#else
#define DATE_DLL
#endif

#if defined(FILE_EXPORTS)
#define FILE_DLL DLLEXPORT
#elif defined(FILE_IMPORTS)
#define FILE_DLL DLLIMPORT
#else
#define FILE_DLL
#endif

#if defined(EBML2_EXPORTS)
#define EBML_DLL DLLEXPORT
#elif defined(EBML2_IMPORTS)
#define EBML_DLL DLLIMPORT
#else
#define EBML_DLL
#endif

#if defined(MD5_EXPORTS)
#define MD5_DLL DLLEXPORT
#elif defined(MD5_IMPORTS)
#define MD5_DLL DLLIMPORT
#else
#define MD5_DLL
#endif
