#define COREMAKE_STATIC
#define COREMAKE_UNICODE
#define CONFIG_MULTITHREAD

//-----------
// failsafes

#if !defined(ARM) || defined(TARGET_SYMBIAN) || defined(TARGET_IPHONE)
#undef CONFIG_WMMX
#endif

#if !defined(IX86) || defined(TARGET_SYMBIAN)
#undef CONFIG_MMX
#endif

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
