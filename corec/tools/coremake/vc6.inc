#define COMPILER_MSVC

CONFIG

#include "winrc.inc"

TARG_DEBUG = Debug
TARG_RELEASE = Release

IFDEF COREMAKE_UNICODE
  TARG_DEBUG = %(TARG_DEBUG) Unicode
  TARG_RELEASE = %(TARG_RELEASE) Unicode
ENDIF
IFDEF COREMAKE_STATIC
  TARG_DEBUG = %(TARG_DEBUG) Static
  TARG_RELEASE = %(TARG_RELEASE) Static
ENDIF

IFDEF COREMAKE_MSVCRT
  TARGRUNTIME = /MD
ELSE
  TARGRUNTIME = /MT
ENDIF

FOR EACH WORKSPACE

  FILE %(¯PATH)%(NAME).dsw
  PRINTNLDOS Microsoft Developer Studio Workspace File, Format Version 6.00
  PRINTNLDOS # WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!
  PRINTNLDOS 
  FOR EACH USE
    IF (%(TYPE)!=LIB_CSHARP) && (%(TYPE)!=DLL_CSHARP) && (%(TYPE)!=CON_CSHARP) && (%(TYPE)!=EXE_CSHARP)
      PRINTNLDOS ###############################################################################
      PRINTNLDOS 
      PRINTNLDOS Project: "%(NAME)"=%(¯!PATH)%(NAME).dsp - Package Owner=<4>
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

  IF %(MSDEV)!=""
    EXE =
    FOR EACH USE
      IF (%(TYPE)==EXE) || (%(TYPE)==CON) || (%(TYPE)==DLL)
        ../EXE += "%(NAME)"
      ENDIF
    ENDFOR
    FILE %(¯PATH)Makefile.vc6
    PRINTNL all: %(EXE)
    PRINTNL 
    FOR EACH USE
      PRINTNL "%(NAME)": %(ITEM_DEPS)
      PRINTNL 	@%(MSDEV) %(../NAME).dsw /make "%(NAME) - Win32 %(TARG_RELEASE)" /out "%(../NAME).log.tmp"
      PRINTNL 	@grep -v "Could not find the file" <%(../NAME).log.tmp >>%(../NAME).log
      PRINTNL 	@del /q %(../NAME).log.tmp
      IF %(NSI)!=""
        IF %(EXEPROTECT)!=""
          TARGET = %(¯~!OUTPUTPATH)release\%(OUTPUT)
          PRINTNL 	@%(EXEPROTECT)
          FOR EACH USE
            IF %(TYPE)==DLL
              TARGET = %(¯~!OUTPUTPATH)release\%(OUTPUT)
              PRINTNL 	@%(../EXEPROTECT)
            ENDIF
          ENDFOR
        ENDIF
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

FOR EACH LIB,DLL,EXE,CON

  PRJNAME = %(NAME)

  IFNDEF TARGET_PALMOS
    DEFINE += WIN32
  ELSE
    DEFINE += __palmos__
  ENDIF

  IFNDEF COREMAKE_UNICODE
    DEFINE += _MBCS
  ELSE
    DEFINE += UNICODE
    DEFINE += _UNICODE
  ENDIF

  IF "%(USE_MFC)"=="" && "%(USE_ATL)"!=""
    USE_MFC = .
  ENDIF

  IF %(USE_MFC)!="" && %(USE_MFC)!="dll"
    LIBS_RELEASE += uafxcw.lib
    LIBS_DEBUG   += uafxcwd.lib
  ENDIF

  USE_DLL_RUNTIME = 
  IF %(USE_MFC)=="dll"
    DEFINE += _AFXDLL
    MFCFLAGS =
    USE_DLL_RUNTIME = 1
  ELSE
    MFCFLAGS = %(TARGLFLAGS_noMFC)
  ENDIF

  IF %(USE_ATL) && %(USE_ATL)!="dll"
    DEFINE += _ATL_STATIC_REGISTRY
  ENDIF

  IF (%(TYPE)==EXE) && !defined(TARGET_PALMOS)
    TARGTYPE = 0x0101
    TARGNAME = Application
    TARGLINK = /subsystem:windows
    DEFINE += _WINDOWS
  ENDIF

  IF (%(TYPE)==CON)
    TARGTYPE = 0x0103
    TARGNAME = Console Application
    TARGLINK = /subsystem:console
    DEFINE += _CONSOLE
  ENDIF

  IF (%(TYPE)==DLL) || ((%(TYPE)==EXE) && defined(TARGET_PALMOS))
    TARGTYPE = 0x0102
    TARGNAME = Dynamic-Link Library
    TARGLINK = /dll
    DEFINE += _USRDLL
    IFNDEF TARGET_PALMOS
      DEFINE += _WINDOWS
    ENDIF
    DEFINE += %(<^NAME)_EXPORTS
  ENDIF

  IF (%(TYPE)==LIB)
    TARGTYPE = 0x0104
    TARGNAME = Static Library
    DEFINE += _LIB
  ENDIF
 
  IF %(DEF)
    SOURCE += %(DEF)
  ENDIF

  FILE %(¯PATH)%(NAME).dsp

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

  IF %(PRECOMPILED_HEADER)!=""
    PRECOMP_OPTION = /YX"%(PRECOMPILED_HEADER)"
    SET_PRECOMPILED_HEADER = 1
  ELIF %(NO_PRECOMPILED_HEADER)==""
    PRECOMP_OPTION = /YX
  ENDIF

  CFLAGS = %(DEFINE:%=/D "%") %(@INCLUDE:%=/I "%") %(@SYSINCLUDE:%=/I "%")

  IF defined(TARGET_PALMOS)
    IF %(PALMSDK5R4)==""
      PALMSDK5R4 = C:\Program Files\ACCESS\Garnet OS Development Suite\sdk-5r4
    ENDIF
    CFLAGS += /I "%(~PALMSDK5R4)\include"
    CFLAGS += /I "%(~PALMSDK5R4)\include\Core"
    CFLAGS += /I "%(~PALMSDK5R4)\include\Core\Hardware"
    CFLAGS += /I "%(~PALMSDK5R4)\include\Core\System"
    CFLAGS += /I "%(~PALMSDK5R4)\include\Core\UI"
    CFLAGS += /I "%(~PALMSDK5R4)\include\Dynamic"
    CFLAGS += /I "%(~PALMSDK5R4)\include\Libraries"
  ENDIF

  PRINTNLDOS # Microsoft Developer Studio Project File - Name="%(NAME)" - Package Owner=<4>
  PRINTNLDOS # Microsoft Developer Studio Generated Build File, Format Version 6.00
  PRINTNLDOS # ** DO NOT EDIT **
  PRINTNLDOS 
  PRINTNLDOS # TARGTYPE "Win32 (x86) %(TARGNAME)" %(TARGTYPE)
  PRINTNLDOS 
  PRINTNLDOS CFG=%(NAME) - Win32 %(TARG_DEBUG)
  PRINTNLDOS !MESSAGE This is not a valid makefile. To build this project using NMAKE,
  PRINTNLDOS !MESSAGE use the Export Makefile command and run
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS !MESSAGE NMAKE /f "%(NAME).mak".
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS !MESSAGE You can specify a configuration when running NMAKE
  PRINTNLDOS !MESSAGE by defining the macro CFG on the command line. For example:
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS !MESSAGE NMAKE /f "%(NAME).mak" CFG="%(NAME) - Win32 %(TARG_DEBUG)"
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS !MESSAGE Possible choices for configuration are:
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS !MESSAGE "%(NAME) - Win32 %(TARG_RELEASE)" (based on "Win32 (x86) %(TARGNAME)")
  PRINTNLDOS !MESSAGE "%(NAME) - Win32 %(TARG_DEBUG)" (based on "Win32 (x86) %(TARGNAME)")
  PRINTNLDOS !MESSAGE 
  PRINTNLDOS 
  PRINTNLDOS # Begin Project
  PRINTNLDOS # PROP AllowPerConfigDependencies 0
  PRINTNLDOS # PROP Scc_ProjName ""
  PRINTNLDOS # PROP Scc_LocalPath ""
  PRINTNLDOS CPP=cl.exe
  IF (%(TYPE)!=LIB && %(TYPE)!=CON)
  PRINTNLDOS MTL=midl.exe
  ENDIF
  PRINTNLDOS RSC=rc.exe
  PRINTNLDOS 
  PRINTNLDOS !IF  "$(CFG)" == "%(NAME) - Win32 %(TARG_RELEASE)"
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
  PRINTNLDOS # PROP BASE Use_Debug_Libraries 0
  PRINTNLDOS # PROP BASE Output_Dir "%(¯~!OUTPUTPATH)release\%(OUTPUT_DIR)"
  PRINTNLDOS # PROP BASE Intermediate_Dir "%(¯~!BUILDPATH)release\%(~PATH)"
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
  PRINTNLDOS # PROP Ignore_Export_Lib 0
  PRINTNLDOS # PROP Target_Dir ""
  PRINTNLDOS # ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" %(CFLAGS) %(@INCLUDE_RELEASE:%=/I "%") %(CONFIG_INCLUDE:%=/I "%") %(PRECOMP_OPTION) /FD /c
  PRINTNLDOS # ADD CPP /nologo %(TARGRUNTIME) /GX /O2 /D "NDEBUG" %(CFLAGS) %(@INCLUDE_RELEASE:%=/I "%") %(CONFIG_INCLUDE:%=/I "%") %(PRECOMP_OPTION) /FD /c
  IF (%(TYPE)!=LIB && %(TYPE)!=CON)
  PRINTNLDOS # ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
  PRINTNLDOS # ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
  ENDIF
  PRINTNLDOS # ADD BASE RSC /l 0x40e /d "NDEBUG" %(CFLAGS) %(INCLUDERC:%=/i "%") %(INCLUDE_RELEASE:%=/i "%") %(CONFIG_INCLUDE:%=/I "%")
  PRINTNLDOS # ADD RSC /l 0x40e /d "NDEBUG" %(CFLAGS) %(INCLUDERC:%=/i "%") %(INCLUDE_RELEASE:%=/i "%") %(CONFIG_INCLUDE:%=/I "%")
  PRINTNLDOS BSC32=bscmake.exe
  PRINTNLDOS # ADD BASE BSC32 /nologo
  PRINTNLDOS # ADD BSC32 /nologo
  IF (%(TYPE)==LIB)
  PRINTNLDOS LIB32=link.exe -lib
  PRINTNLDOS # ADD BASE LIB32 /nologo
  PRINTNLDOS # ADD LIB32 /nologo
  ELSE
  PRINTNLDOS LINK32=link.exe
  PRINTNLDOS # ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib %(LIBS) %(LIBS_RELEASE) %(MFCFLAGS) /nologo %(TARGLINK) %(@LIBINCLUDE:%=/libpath:%) %(@LIBINCLUDE_RELEASE:%=/libpath:%) /machine:I386
  PRINTNLDOS # ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib %(LIBS) %(LIBS_RELEASE) %(MFCFLAGS) /nologo %(TARGLINK) %(@LIBINCLUDE:%=/libpath:%) %(@LIBINCLUDE_RELEASE:%=/libpath:%) /map /machine:I386 /out:"%(¯~!OUTPUTPATH)release/%(OUTPUT)"
  ENDIF
  IF (%(TYPE)==EXE) && defined(TARGET_PALMOS)
    PRINTNLDOS # Begin Special Build Tool
    PRINTNLDOS SOURCE="$(InputPath)"
    PRINTNLDOS PostBuild_Cmds=copy $(TargetPath) "%(PALMSDK5R4)\tools\Palm_OS_54_Simulator\release\AutoLoad"
    PRINTNLDOS # End Special Build Tool
  ENDIF
  PRINTNLDOS 
  PRINTNLDOS !ELSEIF  "$(CFG)" == "%(NAME) - Win32 %(TARG_DEBUG)"
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
  PRINTNLDOS # PROP Ignore_Export_Lib 0
  PRINTNLDOS # PROP Target_Dir ""
  PRINTNLDOS # ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od  /D "_DEBUG" %(CFLAGS) %(@INCLUDE_DEBUG:%=/I "%") %(CONFIG_INCLUDE:%=/I "%") %(PRECOMP_OPTION) /FD /GZ /c
  PRINTNLDOS # ADD CPP /nologo %(TARGRUNTIME)d /Gm /GX /ZI /Od /D "_DEBUG" %(CFLAGS) %(@INCLUDE_DEBUG:%=/I "%") %(CONFIG_INCLUDE:%=/I "%") %(PRECOMP_OPTION) /FD /GZ /c
  IF (%(TYPE)!=LIB && %(TYPE)!=CON)
  PRINTNLDOS # ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
  PRINTNLDOS # ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
  ENDIF
  PRINTNLDOS # ADD BASE RSC /l 0x40e /d "_DEBUG" %(CFLAGS) %(INCLUDERC:%=/i "%") %(INCLUDE_DEBUG:%=/i "%") %(CONFIG_INCLUDE:%=/I "%")
  PRINTNLDOS # ADD RSC /l 0x40e /d "_DEBUG" %(CFLAGS) %(INCLUDERC:%=/i "%") %(INCLUDE_DEBUG:%=/i "%") %(CONFIG_INCLUDE:%=/I "%")
  PRINTNLDOS BSC32=bscmake.exe
  PRINTNLDOS # ADD BASE BSC32 /nologo
  PRINTNLDOS # ADD BSC32 /nologo
  IF (%(TYPE)==LIB)
  PRINTNLDOS LIB32=link.exe -lib
  PRINTNLDOS # ADD BASE LIB32 /nologo
  PRINTNLDOS # ADD LIB32 /nologo
  ELSE
  PRINTNLDOS LINK32=link.exe
  PRINTNLDOS # ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib %(LIBS) %(LIBS_DEBUG) %(MFCFLAGS) /nologo %(TARGLINK) %(@LIBINCLUDE:%=/libpath:%) %(@LIBINCLUDE_DEBUG:%=/libpath:%) /debug /machine:I386 /pdbtype:sept
  PRINTNLDOS # ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib %(LIBS) %(LIBS_DEBUG) %(MFCFLAGS) /nologo %(TARGLINK) %(@LIBINCLUDE:%=/libpath:%) %(@LIBINCLUDE_DEBUG:%=/libpath:%) /debug /machine:I386 /out:"%(¯~!OUTPUTPATH)debug/%(OUTPUT)" /pdbtype:sept
  ENDIF
  IF (%(TYPE)==EXE) && defined(TARGET_PALMOS)
    PRINTNLDOS # Begin Special Build Tool
    PRINTNLDOS SOURCE="$(InputPath)"
    PRINTNLDOS PostBuild_Cmds=copy $(TargetPath) "%(PALMSDK5R4)\tools\Palm_OS_54_Simulator\debug\AutoLoad"
    PRINTNLDOS # End Special Build Tool
  ENDIF
  PRINTNLDOS 
  PRINTNLDOS !ENDIF 
  PRINTNLDOS 
  PRINTNLDOS # Begin Target
  PRINTNLDOS 
  PRINTNLDOS # Name "%(NAME) - Win32 %(TARG_RELEASE)"
  PRINTNLDOS # Name "%(NAME) - Win32 %(TARG_DEBUG)"
  PRINTNLDOS # Begin Group "Source Files"
  PRINTNLDOS 
  PRINTNLDOS # PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
  FOR EACH SOURCE
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
    IF (%(>NAME)==asm)
      PRINTNLDOS !IF  "$(CFG)" == "%(PRJNAME) - Win32 %(TARG_RELEASE)"
      PRINTNLDOS 
      PRINTNLDOS # PROP Ignore_Default_Tool 1
      PRINTNLDOS # Begin Custom Build
      PRINTNLDOS IntDir=%(¯~!BUILDPATH)release\%(~PATH)
      PRINTNLDOS InputPath=%(~!NAME)
      PRINTNLDOS InputName=%(<NAME)
      PRINTNLDOS 
      PRINTNLDOS "$(IntDir)/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
      PRINTNLDOS 	yasm -f win32 -D_WIN32 %(DEFINE:%=-D%) -o "$(IntDir)/$(InputName).obj" "$(InputPath)"
      PRINTNLDOS 
      PRINTNLDOS # End Custom Build
      PRINTNLDOS 
      PRINTNLDOS !ELSEIF  "$(CFG)" == "%(PRJNAME) - Win32 %(TARG_DEBUG)"
      PRINTNLDOS 
      PRINTNLDOS # PROP Ignore_Default_Tool 1
      PRINTNLDOS # Begin Custom Build
      PRINTNLDOS IntDir=%(¯~!BUILDPATH)debug\%(~PATH)
      PRINTNLDOS InputPath=%(~!NAME)
      PRINTNLDOS InputName=%(<NAME)
      PRINTNLDOS 
      PRINTNLDOS "$(IntDir)/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
      PRINTNLDOS 	yasm -f win32 -D_WIN32 %(DEFINE:%=-D%) -o "$(IntDir)/$(InputName).obj" "$(InputPath)"
      PRINTNLDOS 
      PRINTNLDOS # End Custom Build
      PRINTNLDOS 
      PRINTNLDOS !ENDIF 
      PRINTNLDOS 
    ELSE
      IF (%(>NAME)==idl)
        PRINTNLDOS !IF  "$(CFG)" == "%(PRJNAME) - Win32 %(TARG_RELEASE)"
        PRINTNLDOS # ADD MTL /tlb "%(¯~!OUTPUTPATH)release\%(OUTPUT_DIR)%(PRJNAME).tlb" /h "%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.h" /iid "%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.c"
        PRINTNLDOS 
        PRINTNLDOS !ELSEIF  "$(CFG)" == "%(PRJNAME) - Win32 %(TARG_DEBUG)"
        PRINTNLDOS # ADD MTL /tlb "%(¯~!OUTPUTPATH)debug\%(OUTPUT_DIR)%(PRJNAME).tlb" /h "%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.h" /iid "%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.c"
        PRINTNLDOS 
        PRINTNLDOS !ENDIF 
        IF %(cmarshal)!=""
          PRINTNLDOS # End Source File
          PRINTNLDOS # Begin Source File
          PRINTNLDOS 
          PRINTNLDOS SOURCE="%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.c"
        ENDIF
      ELSE 
        IF (%(_copy_item))
          PRINTNLDOS !IF  "$(CFG)" == "%(PRJNAME) - Win32 %(TARG_RELEASE)"
          PRINTNLDOS 
          PRINTNLDOS # PROP Ignore_Default_Tool 1
          PRINTNLDOS # Begin Custom Build
          PRINTNLDOS TargetDir=%(¯~!OUTPUTPATH)release
          PRINTNLDOS InputPath=%(~!NAME)
          PRINTNLDOS InputName=%(<NAME)
          PRINTNLDOS 
          PRINTNLDOS "$(TargetDir)/$(InputName).%(>NAME)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
          PRINTNLDOS 	copy "$(InputPath)" "$(TargetDir)" >nul
          PRINTNLDOS 
          PRINTNLDOS # End Custom Build
          PRINTNLDOS 
          PRINTNLDOS !ELSEIF  "$(CFG)" == "%(PRJNAME) - Win32 %(TARG_DEBUG)"
          PRINTNLDOS 
          PRINTNLDOS # PROP Ignore_Default_Tool 1
          PRINTNLDOS # Begin Custom Build
          PRINTNLDOS TargetDir=%(¯~!OUTPUTPATH)debug
          PRINTNLDOS InputPath=%(~!NAME)
          PRINTNLDOS InputName=%(<NAME)
          PRINTNLDOS 
          PRINTNLDOS "$(TargetDir)/$(InputName).%(>NAME)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
          PRINTNLDOS 	copy "$(InputPath)" "$(TargetDir)" >nul
          PRINTNLDOS 
          PRINTNLDOS # End Custom Build
          PRINTNLDOS 
          PRINTNLDOS !ENDIF 
          PRINTNLDOS 
        ENDIF
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

ENDFOR
