#define COREMAKE_STATIC
//#define COREMAKE_UNICODE

//#define CONFIG_MULTITHREAD
#define CONFIG_DEBUGCHECKS

//-----------
// failsafes

#if !defined(COREMAKE_UNICODE) && (defined(TARGET_WINCE) || defined(TARGET_SYMBIAN))
#define COREMAKE_UNICODE
#endif

#if defined(CONFIG_MULTITHREAD) && (defined(TARGET_SYMBIAN) || defined(TARGET_PALMOS))
#undef CONFIG_MULTITHREAD
#endif

#if defined(COREMAKE_UNICODE) && (defined(TARGET_PALMOS) || defined(TARGET_LINUX))
#undef COREMAKE_UNICODE
#endif

#if !defined(COREMAKE_STATIC) && defined(TARGET_PALMOS) && defined(IX86)
#define COREMAKE_STATIC
#endif
