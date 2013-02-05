#define TARGET_LINUX
#define TARGET_QTOPIA

// GLIBC because of no iconv ?
#define GLIBC

#define OUTPUT_EXE       %(NAME)
#define OUTPUT_CON       %(NAME)
#define OUTPUT_DLL       lib%(NAME).so
#define OUTPUT_DLL_LIB   lib%(NAME).so
#define OUTPUT_LIB       lib%(NAME).a

#include "gcc.inc"
