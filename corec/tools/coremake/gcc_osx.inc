#define TARGET_OSX

#define OUTPUT_EXE     %(NAME)
#define OUTPUT_CON     %(NAME)
#define OUTPUT_DLL     lib%(NAME).dylib
#define OUTPUT_DLL_LIB lib%(NAME).lo
#define OUTPUT_LIB     lib%(NAME).a

#define ASM yasm
IFNDEF STRIP
#define STRIP %(CC_PREFIX)strip
ENDIF
#define STRIPFLAGS -x
// TODO should come from config.h
#ifndef TARGET_IPHONE
#define CCENV MACOSX_DEPLOYMENT_TARGET_ppc=10.3 MACOSX_DEPLOYMENT_TARGET_i386=10.5
#endif

#define GLIBC

IF %(SDK)
  CCFLAGS = %(CCFLAGS) -isysroot %(SDK)
ENDIF
IF %(SYSLIB_ROOT)
  LFLAGS= -Wl,-syslibroot,%(SYSLIB_ROOT)
ELIF %(SDK)
  LFLAGS= -isysroot %(SDK) -Wl,-syslibroot,%(SDK)
ENDIF

CCFLAGS = %(CCFLAGS) -pipe -O3
IF %(SDK_PATH)
  CCFLAGS = -I%(SDK_PATH)/include %(CCFLAGS)
ENDIF
//#define ASMFLAGS -f elf
//#define LFLAGS
//#define LDFLAGS -Wl,-dynamic,-search_paths_first
//#define FFSLDFLAGS -Wl,-bind_at_load

#ifdef PIC
#define SHFLAGS -dynamiclib -Wl,-single_module -Wl,-install_name,\$(OUTPUT)
#else
#define SHFLAGS -dynamiclib -Wl,-read_only_relocs,suppress -Wl,-single_module -Wl,-install_name,\$(OUTPUT)
#endif

#include "gcc.inc"
