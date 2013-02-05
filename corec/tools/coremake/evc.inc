#define OUTPUT_EXE %(NAME).exe
#define OUTPUT_DLL %(NAME).dll
#define OUTPUT_LIB %(NAME).lib

#define COMPILER_MSVC

CONFIG

#ifdef TARGET_WINCE
#define COREMAKE_UNICODE
#endif

#include "winrc.inc"

IF (%(TARGCFLAGS_REL)=="")
  TARGCFLAGS_REL = /O2 /Oxs
ENDIF

IFNDEF IX86
  TARGCFLAGS_REL = %(TARGCFLAGS_REL) /M$(CECrtMT)
  TARGCFLAGS_DBG = %(TARGCFLAGS_DBG) /M$(CECrtMTDebug)
ENDIF

IF ("%(SUBSYSTEM)"=="")
  IFDEF CONFIG_WINCE2
    SUBSYSTEM = /subsystem:windowsce,2.00
  ELSE
    SUBSYSTEM = /subsystem:windowsce,3.00
    //SUBSYSTEM = /subsystem:$(CESubsystem)
  ENDIF
ENDIF

FOR EACH WORKSPACE

  FILE %(¯PATH)%(NAME).vcw
  PRINTNLDOS Microsoft eMbedded Visual Tools Workspace File, Format Version 3.00
  PRINTNLDOS # WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!
  PRINTNLDOS 
  FOR EACH USE
    IF (%(TYPE)!=LIB_CSHARP) && (%(TYPE)!=DLL_CSHARP) && (%(TYPE)!=CON_CSHARP) && (%(TYPE)!=EXE_CSHARP)
      PRINTNLDOS ###############################################################################
      PRINTNLDOS 
      PRINTNLDOS Project: "%(NAME)"=%(¯!PATH)%(NAME).vcp - Package Owner=<4>
      PRINTNLDOS 
      PRINTNLDOS Package=<5>
      PRINTNLDOS {{{
      PRINTNLDOS }}}
      PRINTNLDOS 
      PRINTNLDOS Package=<4>
      PRINTNLDOS {{{
      FOR EACH USE
        IF ((%(TYPE)!=LIB) || (%(../TYPE)!=LIB)) && (%(TYPE)!=LIB_CSHARP) && (%(TYPE)!=DLL_CSHARP) && (%(TYPE)!=CON_CSHARP) && (%(TYPE)!=EXE_CSHARP)
          PRINTNLDOS     Begin Project Dependency
          PRINTNLDOS     Project_Dep_Name %(NAME)
          PRINTNLDOS     End Project Dependency
        ENDIF
        IF (%(TYPE)==EXE) || (%(TYPE)==CON) || (%(TYPE)==DLL)
          ../ITEM_DEPS += "%(NAME)"
        ENDIF
      ENDFOR
      FOR EACH DEP
        IF (%(TYPE)!="LIB_CSHARP") && (%(TYPE)!="DLL_CSHARP") && (%(TYPE)!="CON_CSHARP") && (%(TYPE)!="EXE_CSHARP")
          PRINTNLDOS     Begin Project Dependency
          PRINTNLDOS     Project_Dep_Name %(NAME)
          PRINTNLDOS     End Project Dependency
        ENDIF
        IF (%(TYPE)==EXE) || (%(TYPE)==CON) || (%(TYPE)==DLL)
          ../ITEM_DEPS += "%(NAME)"
        ENDIF
      ENDFOR
      PRINTNLDOS }}}
      PRINTNLDOS 
    ENDIF
  ENDFOR
  PRINTNLDOS ###############################################################################
  PRINTNLDOS 
  PRINTNLDOS Global:
  PRINTNLDOS 
  PRINTNLDOS Package=<5>
  PRINTNLDOS {{{
  PRINTNLDOS }}}
  PRINTNLDOS 
  PRINTNLDOS Package=<3>
  PRINTNLDOS {{{
  PRINTNLDOS }}}
  PRINTNLDOS 
  PRINTNLDOS ###############################################################################
  PRINTNLDOS 

  IF %(EVC)!=""
    FILE %(¯PATH)Makefile.evc
    FOR EACH USE
      IF (%(TYPE)==EXE) || (%(TYPE)==CON) || (%(TYPE)==DLL)
        ../TO_BUILD += "%(NAME)"
      ENDIF
    ENDFOR
    PRINTNL all: %(TO_BUILD)
    PRINTNL 
    FOR EACH USE
      PRINTNL "%(NAME)": %(ITEM_DEPS)
      PRINTNL 	@%(EVC) %(../NAME).vcw /make "%(NAME) - Win32 (%(TARGID)) Release" /CEConfig="%(EVC_SDK)" /out "%(../NAME).log.tmp"
      PRINTNL 	@grep -v "Could not find the file" <%(../NAME).log.tmp >>%(../NAME).log
      PRINTNL 	@del /q %(../NAME).log.tmp
      IF %(EXEPROTECT)!=""
        TARGET = %(¯~!OUTPUTPATH)release\%(OUTPUT)
        PRINTNL 	@-%(EXEPROTECT)
        FOR EACH USE
          IF %(TYPE)==DLL
            TARGET = %(¯~!OUTPUTPATH)release\%(OUTPUT)
            PRINTNL 	@-%(../EXEPROTECT)
          ENDIF
        ENDFOR
      ENDIF
      FOR EACH INSTALL, INSTALL_CAB
        PRINTNL 	@copy /Y %(~!NAME) %(¯~!OUTPUTPATH)release
      ENDFOR
      IF %(CAB)!="" && %(CABWIZ)!=""
        PRINTNL 	@%(CABWIZ) "%(¯~!PATH)%(NAME)_wince.inf" /dest "%(¯~|OUTPUTPATH)"
        PRINTNL 	@move "%(¯~!OUTPUTPATH)%(NAME)_wince.CAB" "%(¯~!OUTPUTPATH)%(PROJECT_NAME).%(CABNAME).cab"
      ENDIF
      IF %(NSI)!=""
        COMPILE %(NSI) %(PATH)%(NAME)_win32.nsi
        PRINTNL 	@makensis.exe /V2 %(¯!PATH)%(NAME)_win32.nsi /O%(NAME)_nsi.log
      ENDIF
      PRINTNL 
    ENDFOR
    PRINTNL clean:
    PRINTNL 	@if exist %(¯~!§BUILDPATH) rmdir /s /q %(¯~!§BUILDPATH)
    PRINTNL 	@if exist %(¯~!§OUTPUTPATH) rmdir /s /q %(¯~!§OUTPUTPATH)
  ENDIF

ENDFOR

FOR EACH LIB,DLL,EXE

  FILE %(¯PATH)%(NAME).vcp

  PRJNAME = %(NAME)
  
  IFNDEF COREMAKE_UNICODE
    DEFINE += _MBCS
  ELSE
    DEFINE += UNICODE
    DEFINE += _UNICODE
  ENDIF

  IF "%(USE_MFC)"!=""
    USE_MFC = dll
  ENDIF
  IF "%(USE_MFC)"=="" && "%(USE_ATL)"!=""
    USE_MFC = .
  ENDIF

  USE_DLL_RUNTIME = 
  IF %(USE_MFC)=="dll"
    DEFINE += _AFXDLL
    MFCFLAGS =
    USE_DLL_RUNTIME = 1
  ELSE
    LIBS += coredll.lib
    MFCFLAGS = %(TARGLFLAGS_noMFC)
  ENDIF

  IF %(USE_ATL) && %(USE_ATL)!="dll"
    DEFINE += _ATL_STATIC_REGISTRY
  ENDIF

  IF (%(TYPE)==EXE)
    TARGTYPE = 01
    TARGNAME = Application
    TARGLINK = /base:"0x00010000" /stack:0x20000,0x1000 %(SUBSYSTEM) 
    IF "%(USE_MFC)"
      TARGLINK += /entry:"wWinMainCRTStartup"
    ELSE
      TARGLINK += /entry:"WinMainCRTStartup"
    ENDIF
    DEFINE += _WINDOWS
  ENDIF

  IF (%(TYPE)==DLL)
    TARGTYPE = 02
    TARGNAME = Dynamic-Link Library
    TARGLINK = /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll %(SUBSYSTEM)
    DEFINE += _USRDLL
    DEFINE += _WINDOWS
    DEFINE += %(<^NAME)_EXPORTS
  ENDIF

  IF (%(TYPE)==LIB)
    TARGTYPE = 04
    TARGNAME = Static Library
    DEFINE += _LIB
  ENDIF
  
  IF "%(USELIB)"
    LIBS += %(USELIB:%=%_%(PLATFORMLIB).lib)
  ENDIF

  HAS_IDL =
  FOR EACH SOURCE
    IF (%(>NAME)==idl)
      ../HAS_IDL = 1
    ENDIF
  ENDFOR
  
  IF %(HAS_IDL)
    INCLUDE += %(¯|BUILDPATH)%(~§PATH)
    INCLUDE_DEBUG   += %(¯|OUTPUTPATH)debug\%(OUTPUT_DIR)
    INCLUDE_RELEASE += %(¯|OUTPUTPATH)release\%(OUTPUT_DIR)
  ENDIF

  AFLAGS = %(DEFINE:%=-D%) %(TARGDEFINE:%=-D%) -D_WIN32
  RFLAGS = /d UNDER_CE /d _WIN32_WCE=$(CEVersion) /d "$(CePlatform)" %(DEFINE:%=/d "%") %(TARGDEFINE:%=/d "%")
  DFLAGS = /D UNDER_CE /D _WIN32_WCE=$(CEVersion) /D "$(CePlatform)" %(DEFINE:%=/D "%") %(TARGDEFINE:%=/D "%")
  CFLAGS = /D UNDER_CE /D _WIN32_WCE=$(CEVersion) /D "$(CePlatform)" %(DEFINE:%=/D "%") %(TARGDEFINE:%=/D "%") %(~@INCLUDE:%=/I "%") %(~@SYSINCLUDE:%=/I "%") %(TARGCFLAGS)
  GFLAGS = -D __GLIBC__ -D _WIN32 -D _WIN32_WCE=$(CEVersion) -D __cdecl= -O3 -fsigned-char -fomit-frame-pointer -mno-apcs-frame -march=armv4 -mtune=xscale -fno-leading-underscore %(DEFINE:%=-D %) %(TARGDEFINE:%=-D %) %(@INCLUDE:%=-I"%") -I. -I/usr/include -I/usr/include/w32api -IC:\MinGW\include
  SFLAGS = %(~@SYSINCLUDE:%=/I "%") 

  PRINTNLDOS # Microsoft eMbedded Visual Tools Project File - Name="%(NAME)" - Package Owner=<4>
  PRINTNLDOS # Microsoft eMbedded Visual Tools Generated Build File, Format Version 6.02
  PRINTNLDOS # ** DO NOT EDIT **
  PRINTNLDOS 
  PRINTNLDOS # TARGTYPE "Win32 (%(TARGID)) %(TARGNAME)" %(TARGCODE)%(TARGTYPE)
  PRINTNLDOS 
  PRINTNLDOS CFG=%(NAME) - Win32 (%(TARGID)) Debug
  PRINTNLDOS !MESSAGE This is not a valid makefile. To build this project using NMAKE,
  PRINTNLDOS !MESSAGE use the Export Makefile command and run
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS !MESSAGE NMAKE /f "%(NAME).vcn".
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS !MESSAGE You can specify a configuration when running NMAKE
  PRINTNLDOS !MESSAGE by defining the macro CFG on the command line. For example:
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS !MESSAGE NMAKE /f "%(NAME).vcn" CFG="%(NAME) - Win32 (%(TARGID)) Debug"
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS !MESSAGE Possible choices for configuration are:
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS !MESSAGE "%(NAME) - Win32 (%(TARGID)) Release" (based on "Win32 (%(TARGID)) %(TARGNAME)")
  PRINTNLDOS !MESSAGE "%(NAME) - Win32 (%(TARGID)) Debug" (based on "Win32 (%(TARGID)) %(TARGNAME)")
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS 
  PRINTNLDOS # Begin Project
  PRINTNLDOS # PROP AllowPerConfigDependencies 0
  PRINTNLDOS # PROP Scc_ProjName ""
  PRINTNLDOS # PROP Scc_LocalPath ""
  PRINTNLDOS # PROP ATL_Project 2
  PRINTNLDOS CPP=%(TARGCC)
  PRINTNLDOS MTL=midl.exe
  PRINTNLDOS RSC=rc.exe
  PRINTNLDOS 
  PRINTNLDOS !IF  "$(CFG)" == "%(NAME) - Win32 (%(TARGID)) Release"
  PRINTNLDOS 
  IF "%(USE_MFC)"
    IF %(USE_MFC)=="dll"
      PRINTNLDOS # PROP BASE Use_MFC 2
    ELSE
      PRINTNLDOS # PROP BASE Use_MFC 1
    ENDIF
    LIBS += mfcs42.lib
  ELSE
    PRINTNLDOS # PROP BASE Use_MFC 0
  ENDIF
  PRINTNLDOS # PROP BASE Use_Debug_Libraries 0
  PRINTNLDOS # PROP BASE Output_Dir "%(¯~!OUTPUTPATH)release\%(OUTPUT_DIR)"
  PRINTNLDOS # PROP BASE Intermediate_Dir "%(¯~!BUILDPATH)release\%(~PATH)"
  PRINTNLDOS # PROP BASE CPU_ID %(TARGCPU)
  PRINTNLDOS # PROP BASE Platform_ID "{8A9A2F80-6887-11D3-842E-005004848CBA}"
  PRINTNLDOS # PROP BASE Target_Dir ""
  IF "%(USE_MFC)"
    IF %(USE_MFC)=="dll"
      PRINTNLDOS # PROP Use_MFC 2
    ELSE
      PRINTNLDOS # PROP Use_MFC 1
    ENDIF
  ELSE
    PRINTNLDOS # PROP Use_MFC 0
  ENDIF
  PRINTNLDOS # PROP Use_Debug_Libraries 0
  PRINTNLDOS # PROP Output_Dir "%(¯~!OUTPUTPATH)release\%(OUTPUT_DIR)"
  PRINTNLDOS # PROP Intermediate_Dir "%(¯~!BUILDPATH)release\%(~PATH)"
  PRINTNLDOS # PROP CPU_ID %(TARGCPU)
  PRINTNLDOS # PROP Platform_ID "{8A9A2F80-6887-11D3-842E-005004848CBA}"
//  PRINTNLDOS # PROP Ignore_Export_Lib 0
  IF %(TYPE)==LIB
    PRINTNLDOS # PROP Sec_AlwaysSign 0
  ENDIF
  IF ("%(WIN_CERTIFICATE)"!="")
    PRINTNLDOS # PROP Sec_CertificateHash "%(WIN_CERTIFICATE)"
  ELSE
    PRINTNLDOS # PROP Sec_CertificateHash "8aeb6e720079eb0494ae9be32cb13a9e3687d913"
  ENDIF
  PRINTNLDOS # PROP Sec_ConfigureDevice 0
  PRINTNLDOS # PROP Target_Dir ""
  PRINTNLDOS # ADD BASE RSC /l 0x409 %(RFLAGS) /d "NDEBUG" /r %(~!INCLUDERC:%=/i "%")
  IF %(PRECOMPILED_HEADER)!=""
    PRECOMP_OPTION = /Yu"%(PRECOMPILED_HEADER)"
    SET_PRECOMPILED_HEADER = 1
  ELSE
    PRECOMP_OPTION = /YX
  ENDIF
  PRINTNLDOS # ADD RSC /l 0x409 %(~@INCLUDE_RELEASE:%=/i "%") %(RFLAGS) /d "NDEBUG" /r %(~!INCLUDERC:%=/i "%") %(CONFIG_INCLUDE:%=/i "%")
  PRINTNLDOS # ADD BASE CPP /nologo %(TARGCFLAGS_REL) /D "NDEBUG" %(CFLAGS) %(~@INCLUDE_RELEASE:%=/I "%") %(CONFIG_INCLUDE:%=/i "%") /FA %(PRECOMP_OPTION) /c
  PRINTNLDOS # ADD CPP /nologo %(TARGCFLAGS_REL) /D "NDEBUG" %(CFLAGS) %(~@INCLUDE_RELEASE:%=/I "%") %(CONFIG_INCLUDE:%=/i "%") /FA %(PRECOMP_OPTION) /c
  IF (%(TYPE)!=LIB && %(TYPE)!=CON)
    PRINTNLDOS # ADD BASE MTL /nologo /D "NDEBUG" %(DFLAGS) /mktyplib203 /o "NUL" /win32
    PRINTNLDOS # ADD MTL /nologo /D "NDEBUG" %(DFLAGS) /mktyplib203 /o "NUL" /win32
  ENDIF
  PRINTNLDOS BSC32=bscmake.exe
  PRINTNLDOS # ADD BASE BSC32 /nologo
  PRINTNLDOS # ADD BSC32 /nologo
  IF (%(TYPE)==LIB)
    PRINTNLDOS LIB32=link.exe -lib
    PRINTNLDOS # ADD BASE LIB32 /nologo
    PRINTNLDOS # ADD LIB32 /nologo
  ELSE
    PRINTNLDOS LINK32=link.exe
    PRINTNLDOS # ADD BASE LINK32 %(LIBS) %(LIBS_RELEASE) /nologo %(TARGLINK) /map /nodefaultlib:"$(CENoDefaultLib)" /out:"%(¯!OUTPUTPATH)release/%(OUTPUT)" %(TARGLFLAGS) %(MFCFLAGS) %(@LIBINCLUDE:%=/libpath:%) %(@LIBINCLUDE_RELEASE:%=/libpath:%)
    PRINTNLDOS # ADD LINK32 %(LIBS) %(LIBS_RELEASE) /nologo %(TARGLINK) /map /nodefaultlib:"$(CENoDefaultLib)" /out:"%(¯!OUTPUTPATH)release/%(OUTPUT)" %(TARGLFLAGS) %(MFCFLAGS) %(@LIBINCLUDE:%=/libpath:%) %(@LIBINCLUDE_RELEASE:%=/libpath:%)
  ENDIF
  PRINTNLDOS 
  PRINTNLDOS !ELSEIF  "$(CFG)" == "%(NAME) - Win32 (%(TARGID)) Debug"
  PRINTNLDOS 
  IF "%(USE_MFC)"
    IF %(USE_MFC)=="dll"
      PRINTNLDOS # PROP BASE Use_MFC 2
    ELSE
      PRINTNLDOS # PROP BASE Use_MFC 1
    ENDIF
  ELSE
    PRINTNLDOS # PROP BASE Use_MFC 0
  ENDIF
  PRINTNLDOS # PROP BASE Use_Debug_Libraries 1
  PRINTNLDOS # PROP BASE Output_Dir "%(¯~!OUTPUTPATH)debug\%(OUTPUT_DIR)"
  PRINTNLDOS # PROP BASE Intermediate_Dir "%(¯~!BUILDPATH)debug\%(~PATH)"
  PRINTNLDOS # PROP BASE CPU_ID %(TARGCPU)
  PRINTNLDOS # PROP BASE Platform_ID "{8A9A2F80-6887-11D3-842E-005004848CBA}"
  PRINTNLDOS # PROP BASE Target_Dir ""
  IF "%(USE_MFC)"
    IF %(USE_MFC)=="dll"
      PRINTNLDOS # PROP Use_MFC 2
    ELSE
      PRINTNLDOS # PROP Use_MFC 1
    ENDIF
  ELSE
    PRINTNLDOS # PROP Use_MFC 0
  ENDIF
  PRINTNLDOS # PROP Use_Debug_Libraries 1
  PRINTNLDOS # PROP Output_Dir "%(¯~!OUTPUTPATH)debug\%(OUTPUT_DIR)"
  PRINTNLDOS # PROP Intermediate_Dir "%(¯~!BUILDPATH)debug\%(~PATH)"
  PRINTNLDOS # PROP CPU_ID %(TARGCPU)
  PRINTNLDOS # PROP Platform_ID "{8A9A2F80-6887-11D3-842E-005004848CBA}"
//  PRINTNLDOS # PROP Ignore_Export_Lib 0
  IF %(TYPE)==LIB
    PRINTNLDOS # PROP Sec_AlwaysSign 0
  ENDIF
  IF ("%(WIN_CERTIFICATE)"!="")
    PRINTNLDOS # PROP Sec_CertificateHash "%(WIN_CERTIFICATE)"
  ELSE
    PRINTNLDOS # PROP Sec_CertificateHash "8aeb6e720079eb0494ae9be32cb13a9e3687d913"
  ENDIF
  PRINTNLDOS # PROP Sec_ConfigureDevice 0
  PRINTNLDOS # PROP Target_Dir ""
  PRINTNLDOS # ADD BASE RSC /l 0x409 %(RFLAGS) /d "DEBUG" /r %(~!INCLUDERC:%=/i "%")
  PRINTNLDOS # ADD RSC /l 0x409 %(~@INCLUDE_DEBUG:%=/i "%")  %(RFLAGS) /d "DEBUG" /r %(~!INCLUDERC:%=/i "%") %(CONFIG_INCLUDE:%=/i "%")
  PRINTNLDOS # ADD BASE CPP /nologo %(TARGCFLAGS_DBG) /W3 /Zi /Od /D "DEBUG" %(CFLAGS) %(~@INCLUDE_DEBUG:%=/I "%") %(CONFIG_INCLUDE:%=/i "%") %(PRECOMP_OPTION) /c
  PRINTNLDOS # ADD CPP /nologo %(TARGCFLAGS_DBG) /W3 /Zi /Od /D "DEBUG" %(CFLAGS) %(~@INCLUDE_DEBUG:%=/I "%") %(CONFIG_INCLUDE:%=/i "%") %(PRECOMP_OPTION) /c
  IF (%(TYPE)!=LIB && %(TYPE)!=CON)
    PRINTNLDOS # ADD BASE MTL /nologo /D "_DEBUG" %(DFLAGS) /mktyplib203 /o "NUL" /win32
    PRINTNLDOS # ADD MTL /nologo /D "_DEBUG" %(DFLAGS) /mktyplib203 /o "NUL" /win32
  ENDIF
  PRINTNLDOS BSC32=bscmake.exe
  PRINTNLDOS # ADD BASE BSC32 /nologo
  PRINTNLDOS # ADD BSC32 /nologo
  IF (%(TYPE)==LIB)
    PRINTNLDOS LIB32=link.exe -lib
    PRINTNLDOS # ADD BASE LIB32 /nologo
    PRINTNLDOS # ADD LIB32 /nologo
  ELSE
    PRINTNLDOS LINK32=link.exe
    PRINTNLDOS # ADD BASE LINK32 %(LIBS) %(LIBS_DEBUG) /nologo %(TARGLINK) /debug /nodefaultlib:"$(CENoDefaultLib)" /out:"%(¯!OUTPUTPATH)debug/%(OUTPUT)" %(TARGLFLAGS) %(MFCFLAGS) %(@LIBINCLUDE:%=/libpath:%) %(@LIBINCLUDE_DEBUG:%=/libpath:%)
    PRINTNLDOS # ADD LINK32 %(LIBS) %(LIBS_DEBUG) /nologo %(TARGLINK) /debug /nodefaultlib:"$(CENoDefaultLib)" /out:"%(¯!OUTPUTPATH)debug/%(OUTPUT)" %(TARGLFLAGS) %(MFCFLAGS) %(@LIBINCLUDE:%=/libpath:%) %(@LIBINCLUDE_DEBUG:%=/libpath:%)
  ENDIF
  PRINTNLDOS 
  PRINTNLDOS !ENDIF 
  PRINTNLDOS 
  PRINTNLDOS # Begin Target
  PRINTNLDOS 
  PRINTNLDOS # Name "%(NAME) - Win32 (%(TARGID)) Release"
  PRINTNLDOS # Name "%(NAME) - Win32 (%(TARGID)) Debug"
  PRINTNLDOS # Begin Group "Source Files"
  PRINTNLDOS 
  PRINTNLDOS # PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
  FOR EACH SOURCE, DEF
    PRINTNLDOS # Begin Source File
    PRINTNLDOS 
    PRINTNLDOS SOURCE=%(~!NAME)
    IF (%(../SET_PRECOMPILED_HEADER)==1) && (%(>NAME)==c || %(>NAME)==cpp)
      PRINTNLDOS # ADD CPP /Yc"%(PRECOMPILED_HEADER)"
      ../SET_PRECOMPILED_HEADER =
    ELSE
      IF ((%(>NAME)==c || %(>NAME)==cpp) && %(w))
        PRINTNLDOS # ADD CPP /w /W0
      ENDIF
    ENDIF
    IF (%(>NAME)==idl)
      PRINTNLDOS !IF  "$(CFG)" == "%(PRJNAME) - Win32 (%(TARGID)) Release"
      PRINTNLDOS # ADD MTL /tlb "%(¯~!OUTPUTPATH)release\%(OUTPUT_DIR)%(PRJNAME).tlb" /h "%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.h" /iid "%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.c"
      PRINTNLDOS 
      PRINTNLDOS !ELSEIF  "$(CFG)" == "%(PRJNAME) - Win32 (%(TARGID)) Debug"
      PRINTNLDOS # ADD MTL /tlb "%(¯~!OUTPUTPATH)debug\%(OUTPUT_DIR)%(PRJNAME).tlb" /h "%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.h" /iid "%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.c"
      PRINTNLDOS 
      PRINTNLDOS !ENDIF 
      IF %(cmarshal)!=""
        PRINTNLDOS # End Source File
        PRINTNLDOS # Begin Source File
        PRINTNLDOS 
        PRINTNLDOS SOURCE="%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.c"
      ENDIF
    ENDIF
    IF (%(>NAME)==asm)
      PRINTNLDOS !IF  "$(CFG)" == "%(PRJNAME) - Win32 (%(TARGID)) Release"
      PRINTNLDOS 
      PRINTNLDOS # PROP Ignore_Default_Tool 1
      PRINTNLDOS # Begin Custom Build
      PRINTNLDOS IntDir=%(¯!BUILDPATH)release\%(PATH)
      PRINTNLDOS InputPath=%(!NAME)
      PRINTNLDOS InputName=%(<NAME)
      PRINTNLDOS 
      PRINTNLDOS "$(IntDir)/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
      PRINTNLDOS 	%(TARGASM)
      PRINTNLDOS 
      PRINTNLDOS # End Custom Build
      PRINTNLDOS 
      PRINTNLDOS !ELSEIF  "$(CFG)" == "%(PRJNAME) - Win32 (%(TARGID)) Debug"
      PRINTNLDOS 
      PRINTNLDOS # PROP Ignore_Default_Tool 1
      PRINTNLDOS # Begin Custom Build
      PRINTNLDOS IntDir=%(¯!BUILDPATH)debug\%(PATH)
      PRINTNLDOS InputPath=%(!NAME)
      PRINTNLDOS InputName=%(<NAME)
      PRINTNLDOS 
      PRINTNLDOS "$(IntDir)/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
      PRINTNLDOS 	%(TARGASM)
      PRINTNLDOS 
      PRINTNLDOS # End Custom Build
      PRINTNLDOS 
      PRINTNLDOS !ENDIF 
      PRINTNLDOS 
    ENDIF
    IF DEFINED(MIPS)
     IF (%(>NAME)==s)
      PRINTNLDOS !IF  "$(CFG)" == "%(PRJNAME) - Win32 (%(TARGID)) Release"
      PRINTNLDOS 
      PRINTNLDOS # PROP Ignore_Default_Tool 1
      PRINTNLDOS # Begin Custom Build
      PRINTNLDOS IntDir=%(¯!BUILDPATH)release\%(PATH)
      PRINTNLDOS InputPath=%(!NAME)
      PRINTNLDOS InputName=%(<NAME)
      PRINTNLDOS 
      PRINTNLDOS "$(IntDir)/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
      PRINTNLDOS 	%(TARGS) %(SFLAGS)
      PRINTNLDOS 
      PRINTNLDOS # End Custom Build
      PRINTNLDOS 
      PRINTNLDOS !ELSEIF  "$(CFG)" == "%(PRJNAME) - Win32 (%(TARGID)) Debug"
      PRINTNLDOS 
      PRINTNLDOS # PROP Ignore_Default_Tool 1
      PRINTNLDOS # Begin Custom Build
      PRINTNLDOS IntDir=%(¯!BUILDPATH)debug\%(PATH)
      PRINTNLDOS InputPath=%(!NAME)
      PRINTNLDOS InputName=%(<NAME)
      PRINTNLDOS 
      PRINTNLDOS "$(IntDir)/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
      PRINTNLDOS 	%(TARGS) %(SFLAGS)
      PRINTNLDOS 
      PRINTNLDOS # End Custom Build
      PRINTNLDOS 
      PRINTNLDOS !ENDIF 
      PRINTNLDOS 
     ENDIF
    ENDIF
    PRINTNLDOS # End Source File
  ENDFOR
  PRINTNLDOS # End Group
  PRINTNLDOS # Begin Group "Header Files"
  PRINTNLDOS 
  PRINTNLDOS # PROP Default_Filter "h;hpp;hxx;hm;inl"
  FOR EACH HEADER
    PRINTNLDOS # Begin Source File
    PRINTNLDOS 
    PRINTNLDOS SOURCE=%(~!NAME)
    PRINTNLDOS # End Source File
  ENDFOR
  IF %(HAS_IDL)
    FOR EACH SOURCE
      IF (%(>NAME)==idl) && %(hmarshal)!=""
        PRINTNLDOS # Begin Source File
        PRINTNLDOS 
        PRINTNLDOS SOURCE="%(<NAME)_i.h"
        PRINTNLDOS # End Source File
      ENDIF
    ENDFOR
  ENDIF
  PRINTNLDOS # End Group
  PRINTNLDOS # Begin Group "Resource Files"
  PRINTNLDOS 
  PRINTNLDOS # PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
  PRINTNLDOS # End Group
  PRINTNLDOS # End Target
  PRINTNLDOS # End Project

  #include "wincab.inc"

ENDFOR
