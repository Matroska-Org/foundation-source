#define TARGET_SYMBIAN
#define ARM
#define COMPILER_GCC
#define MSVC_SEE_DEPRECATE
#define CARBIDE_VS

DEVENV = "%VS80COMNTOOLS%..\IDE\devenv.exe"

VSBUILDER = Studio 2005
VSVERSION = 8.00
COMPILERVERSION = 8.00
USE_DLL_RUNTIME = 1

TARGNAME = Win32
TARGCFLAGS = /QIfist /NDSProjectFile &quot;$(ProjectPath)&quot; /NDSSDKShortName %(SDKNAME)
TARGLFLAGS = /FORCE:MULTIPLE,UNRESOLVED
TARGINC = $(EPOCROOT)\epoc32\include
TARGINC += ;&quot;$(EPOCROOT)\epoc32\include\libc&quot;
IF defined(SYMBIAN90)
  TARGINC += ;&quot;$(EPOCROOT)\epoc32\include\ecom&quot;
ENDIF
IF defined(SERIES60)
  IF defined(SYMBIAN90)
    TARGINC += ;&quot;$(EPOCROOT)\epoc32\include\mmf\plugin&quot;
  ENDIF
ENDIF

TARGMACHINE = 1
TARGEXESUBSYSTEM = 2

PRECOMMAND_DEB = RCMake.exe &quot;$(ProjectPath)&quot; %(SDKNAME) WINSCW UDEB
PRECOMMAND_REL = RCMake.exe &quot;$(ProjectPath)&quot; %(SDKNAME) GCCE UREL

TARGDEF += _UNICODE
TARGDEF += __SYMBIAN32__
TARGDEF += __SUPPORT_CPP_EXCEPTIONS__
DEFINE_RELEASE += __EPOC32__
DEFINE_RELEASE += __MARM__
DEFINE_RELEASE += __EABI__
DEFINE_RELEASE += __MARM_ARMV5__
DEFINE_RELEASE += __GCCE__
DEFINE_DEBUG += __WINSCW__
DEFINE_DEBUG += __WINS__
DEFINE_DEBUG += __CW32__

#include "vs.inc"

EXECUTABLE_DEBUG = C:\Nokia\Tools\Carbide_vs30\EpocWrapper.exe
ARGUMENTS_DEBUG = $(EPOCROOT)\\Epoc32\release\winscw\udeb\eshell.exe

#include "symbian.inc"

FOR EACH EXE,DLL,LIB
  FILE %(¯PATH)%(NAME)-SymbianOS-properties.xml
  PRINTNL <?xml version="1.0" encoding="utf-8"?>
  PRINTNL <NDSProject xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" version="1.0" supplier="Nokia">
  PRINTNL   <activeMmpFile>%(NAME).mmp</activeMmpFile>
  PRINTNL   <updateMmpFile>false</updateMmpFile>
  PRINTNL   <updateUidCppFile>false</updateUidCppFile>
  IF %(TYPE)==EXE
    PRINTNL   <pkgFile>%(NAME).pkg</pkgFile>
    PRINTNL   <pkgUpdate>true</pkgUpdate>
    PRINTNL   <pkgGeneration>true</pkgGeneration>
    PRINTNL   <projectProperty name="uid" label="Application UID" value="%(PROJECT_UID_3RD)" type="input" pattern="UID" role="Symbian9UID" persistedAs="project.uid3" />
  ELIF %(TYPE)==DLL
    PRINTNL   <projectProperty name="engineUid" label="DLL UID" value="%(PROJECT_UID_3RD)" type="input" pattern="UID" role="Symbian9UID" persistedAs="project.uid3" />
  ELIF %(TYPE)==LIB
    PRINTNL   <projectProperty name="libUid" label="Lib UID" value="0xe611bdef" type="input" pattern="UID" role="Symbian9UID" persistedAs="project.uid3" />
  ENDIF
  PRINTNL   <lang>EN</lang>
  PRINTNL   <copyExportsFlag>false</copyExportsFlag>
  PRINTNL </NDSProject>

  FILE %(¯PATH)%(NAME).uid.cpp
  PRINTNL #include <e32cmn.h>
  PRINTNL #pragma data_seg(".SYMBIAN")
  IF %(TYPE)==EXE
    PRINTNL __EMULATOR_IMAGE_HEADER2(0x1000007a,0x100039ce,%(PROJECT_UID_3RD),EPriorityForeground,0x0u,0x00000000u,%(PROJECT_UID_3RD),0,0,0)
  ELIF %(TYPE)==DLL
    PRINTNL __EMULATOR_IMAGE_HEADER2(0x10000079,0x1000008d,%(PROJECT_UID_3RD),EPriorityForeground,0x0u,0x00000000u,%(PROJECT_UID_3RD),0,0,0)
  ELIF %(TYPE)==LIB
    PRINTNL __EMULATOR_IMAGE_HEADER2(0x00000000,0x0,0xe611bdef,EPriorityForeground,0x0u,0x00000000u,0,0,0,0)
  ENDIF
  PRINTNL #pragma data_seg()
    
  FILE %(¯PATH)%(NAME).vcproj.user
  #include "vsproj-user.inc"
ENDFOR
