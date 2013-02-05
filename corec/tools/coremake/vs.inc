IFDEF TARGET_PALMOS
#define OUTPUT_EXE %(NAME).dll
ELSE
#define OUTPUT_EXE %(NAME).exe
ENDIF
#define OUTPUT_DLL %(NAME).dll
#define OUTPUT_LIB %(NAME).lib
IFNDEF TARGET_WINCE
#define OUTPUT_CON %(NAME).exe
ENDIF

#define COMPILER_MSVC

CONFIG

#ifdef TARGET_WINCE
#define COREMAKE_UNICODE
#endif

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

IFDEF TARGET_SYMBIAN
  TARG_DEBUG = Deb_%(SDKNAME)_WINSCW
  TARG_RELEASE = Rel_%(SDKNAME)_GCCE
ENDIF

IF DEFINED(COMPILER_ICL9) || DEFINED(COMPILER_ICL11)
  PROJECT_EXT=icproj
  PROJECT_GUID={EAF909A5-FA59-4C3D-9431-0FCC20D5BCF9}
ELSE
  PROJECT_EXT=vcproj
  PROJECT_GUID={8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}
ENDIF

FOR EACH WORKSPACE

  FILE %(¯PATH)%(NAME).sln
  IF "%(VSVERSION)"=="8.00"
    PRINTNL Microsoft Visual Studio Solution File, Format Version 9.00
    PRINTNL # Visual %(VSBUILDER)
  ELSE
  IF "%(VSVERSION)"=="9.00"
    PRINTNL Microsoft Visual Studio Solution File, Format Version 10.00
    PRINTNL # Visual %(VSBUILDER)
  ELSE
    PRINTNL Microsoft Visual Studio Solution File, Format Version 8.00
  ENDIF
  ENDIF
  
  HAS_CSHARP=
  FOR EACH USE
    IF %(TYPE)==LIB_CSHARP || %(TYPE)==DLL_CSHARP || %(TYPE)==EXE_CSHARP || %(TYPE)==CON_CSHARP
      ../HAS_CSHARP=1
    ENDIF
  ENDFOR
  
  FOR EACH USE
    IF %(TYPE)==LIB_CSHARP || %(TYPE)==DLL_CSHARP || %(TYPE)==EXE_CSHARP || %(TYPE)==CON_CSHARP
      IFNDEF TARGET_NO_DOT_NET
        PRINTNL Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "%(NAME)", "%(~!PATH)%(NAME).csproj", "%(GUID)"
      ENDIF
    ELSE
      PRINTNL Project("%(PROJECT_GUID)") = "%(NAME)", "%(¯~!PATH)%(NAME).%(PROJECT_EXT)", "%(GUID)"
    ENDIF
    IF "%(USE)"!=""
      PRINTNL 	ProjectSection(ProjectDependencies) = postProject
      FOR EACH USE
        IF (%NOLIB)
          PRINTNL 		%(GUID) = %(GUID)
        ENDIF
      ENDFOR
      PRINTNL 	EndProjectSection
    ENDIF
    PRINTNL EndProject
    IF %(CAB)!="" && %(CABWIZ)!="" && %(../HAS_CSHARP)!=""
      PRINTNL Project("{B900F1C2-3D47-4FEC-85B3-04AAF18C3634}") = "%(PROJECT_NAME)", "%(¯~!PATH)%(NAME)_cab.vddproj", "%(GUID2)"
      PRINTNL 	ProjectSection(ProjectDependencies) = postProject
      PRINTNL 		%(PROJECT_GUID) = %(PROJECT_GUID)
      PRINTNL 	EndProjectSection
      PRINTNL EndProject
    ENDIF
  ENDFOR
//  PRINTNL 	EndProjectSection
//  PRINTNL EndProject
  PRINTNL Global
  IF "%(VSVERSION)"=="8.00" || "%(VSVERSION)"=="9.00"
    PRINTNL 	GlobalSection(SolutionConfigurationPlatforms) = preSolution
    IF %(TYPE)==LIB_CSHARP || %(TYPE)==DLL_CSHARP || %(TYPE)==EXE_CSHARP || %(TYPE)==CON_CSHARP
      PRINTNL 		%(TARG_DEBUG)|%(TARGNAME) = %(TARG_DEBUG)|Any CPU
    ELSE
      PRINTNL 		%(TARG_DEBUG)|%(TARGNAME) = %(TARG_DEBUG)|%(TARGNAME)
    ENDIF
    IF %(TYPE)==LIB_CSHARP || %(TYPE)==DLL_CSHARP || %(TYPE)==EXE_CSHARP || %(TYPE)==CON_CSHARP
      PRINTNL 		%(TARG_RELEASE)|%(TARGNAME) = %(TARG_RELEASE)|Any CPU
    ELSE
      PRINTNL 		%(TARG_RELEASE)|%(TARGNAME) = %(TARG_RELEASE)|%(TARGNAME)
    ENDIF
  ELSE
    PRINTNL 	GlobalSection(SolutionConfiguration) = preSolution
    PRINTNL 		%(TARG_DEBUG) = %(TARG_DEBUG)
    PRINTNL 		%(TARG_RELEASE) = %(TARG_RELEASE)
  ENDIF
  PRINTNL 	EndGlobalSection
  IF "%(VSVERSION)"=="8.00" || "%(VSVERSION)"=="9.00"
    PRINTNL 	GlobalSection(ProjectConfigurationPlatforms) = postSolution
  ELSE
    PRINTNL 	GlobalSection(ProjectConfiguration) = postSolution
  ENDIF  
  IF "%(VSVERSION)"=="7.10"
    FOR EACH USE
        PRINTNL 		%(GUID).%(TARG_DEBUG).ActiveCfg = %(TARG_DEBUG)|%(TARGNAME)
        PRINTNL 		%(GUID).%(TARG_DEBUG).Build.0 = %(TARG_DEBUG)|%(TARGNAME)
        IF %(TARGDEPLOY)
            PRINTNL 		%(GUID).%(TARG_DEBUG).Deploy.0 = %(TARG_DEBUG)|%(TARGNAME)
        ENDIF
        PRINTNL 		%(GUID).%(TARG_RELEASE).ActiveCfg = %(TARG_RELEASE)|%(TARGNAME)
        PRINTNL 		%(GUID).%(TARG_RELEASE).Build.0 = %(TARG_RELEASE)|%(TARGNAME)
        IF %(TARGDEPLOY)
            PRINTNL 		%(GUID).%(TARG_RELEASE).Deploy.0 = %(TARG_RELEASE)|%(TARGNAME)
        ENDIF
    ENDFOR
  ELSE
    FOR EACH USE
      IF %(TYPE)==LIB_CSHARP || %(TYPE)==DLL_CSHARP || %(TYPE)==EXE_CSHARP || %(TYPE)==CON_CSHARP
        PRINTNL 		%(GUID).%(TARG_DEBUG)|%(TARGNAME).ActiveCfg = %(TARG_DEBUG)|Any CPU
        PRINTNL 		%(GUID).%(TARG_DEBUG)|%(TARGNAME).Build.0 = %(TARG_DEBUG)|Any CPU
        IF %(TARGDEPLOY)
          PRINTNL 		%(GUID).%(TARG_DEBUG)|%(TARGNAME).Deploy.0 = %(TARG_DEBUG)|Any CPU
        ENDIF
      ELSE
        PRINTNL 		%(GUID).%(TARG_DEBUG)|%(TARGNAME).ActiveCfg = %(TARG_DEBUG)|%(TARGNAME)
        PRINTNL 		%(GUID).%(TARG_DEBUG)|%(TARGNAME).Build.0 = %(TARG_DEBUG)|%(TARGNAME)
        IF %(TARGDEPLOY)
          PRINTNL 		%(GUID).%(TARG_DEBUG)|%(TARGNAME).Deploy.0 = %(TARG_DEBUG)|%(TARGNAME)
        ENDIF
      ENDIF
      IF %(TYPE)==LIB_CSHARP || %(TYPE)==DLL_CSHARP || %(TYPE)==EXE_CSHARP || %(TYPE)==CON_CSHARP
        PRINTNL 		%(GUID).%(TARG_RELEASE)|%(TARGNAME).ActiveCfg = %(TARG_RELEASE)|Any CPU
        PRINTNL 		%(GUID).%(TARG_RELEASE)|%(TARGNAME).Build.0 = %(TARG_RELEASE)|Any CPU
        IF %(TARGDEPLOY)
          PRINTNL 		%(GUID).%(TARG_RELEASE)|%(TARGNAME).Deploy.0 = %(TARG_RELEASE)|Any CPU
        ENDIF
      ELSE
        PRINTNL 		%(GUID).%(TARG_RELEASE)|%(TARGNAME).ActiveCfg = %(TARG_RELEASE)|%(TARGNAME)
        PRINTNL 		%(GUID).%(TARG_RELEASE)|%(TARGNAME).Build.0 = %(TARG_RELEASE)|%(TARGNAME)
        IF %(TARGDEPLOY)
          PRINTNL 		%(GUID).%(TARG_RELEASE)|%(TARGNAME).Deploy.0 = %(TARG_RELEASE)|%(TARGNAME)
        ENDIF
      ENDIF
      IF %(CAB)!="" && %(CABWIZ)!=""
        PRINTNL 		%(GUID2).%(TARG_DEBUG)|%(TARGNAME).ActiveCfg = %(TARG_DEBUG)
        PRINTNL 		%(GUID2).%(TARG_DEBUG)|%(TARGNAME).Build.0 = %(TARG_RELEASE)
        IF %(TARGDEPLOY)
          PRINTNL 		%(GUID2).%(TARG_DEBUG)|%(TARGNAME).Deploy.0 = %(TARG_DEBUG)
        ENDIF
        PRINTNL 		%(GUID2).%(TARG_RELEASE)|%(TARGNAME).ActiveCfg = %(TARG_RELEASE)
        PRINTNL 		%(GUID2).%(TARG_RELEASE)|%(TARGNAME).Build.0 = %(TARG_RELEASE)
        IF %(TARGDEPLOY)
          PRINTNL 		%(GUID2).%(TARG_RELEASE)|%(TARGNAME).Deploy.0 = %(TARG_RELEASE)
        ENDIF
      ENDIF
    ENDFOR  
  ENDIF
  PRINTNL 	EndGlobalSection
  IF "%(VSVERSION)"=="8.00" || "%(VSVERSION)"=="9.00"
    PRINTNL 	GlobalSection(SolutionProperties) = preSolution
    PRINTNL 		HideSolutionNode = FALSE
    PRINTNL 	EndGlobalSection
  ELSE
    PRINTNL 	GlobalSection(ExtensibilityGlobals) = postSolution
    PRINTNL 	EndGlobalSection
    PRINTNL 	GlobalSection(ExtensibilityAddIns) = postSolution
    PRINTNL 	EndGlobalSection
  ENDIF
  PRINTNL EndGlobal

  IF %(DEVENV)!=""
    IF %(TARGNONAME)
      ../TARGBUILD = %(TARG_RELEASE)
    ELSE
      ../TARGBUILD = %(TARG_RELEASE)|%(TARGNAME)
    ENDIF
    EXE =
    FILE %(¯PATH)Makefile.vs
    FOR EACH USE
      IF (%(TYPE)==EXE) || (%(TYPE)==CON) || (%(TYPE)==DLL) || (%(TYPE)==CON_CSHARP) || (%(TYPE)==EXE_CSHARP) || (%(TYPE)==DLL_CSHARP)
        IF %(CAB)=="" || %(../HAS_CSHARP)==""
          // CSharp needs the cab generated in VS in the IDE, in all other cases CAB has to be done in command-line due to a bug in VS
          ../EXE += "%(NAME)"
        ENDIF
        IF %(EXEPROTECT)!="" || %(NSI)!=""
          // the ones that require extra processing inside the makefile (could well be added in the project as post build)
          ../EXE += "%(NAME)"
        ENDIF
      ENDIF
    ENDFOR
    //PRINTNL all: %(EXE)
    PRINTNL all:
    //IF %(../HAS_CSHARP)!=""
      PRINTNL 	@%(DEVENV) %(NAME).sln /build "%(TARGBUILD)" /out "%(NAME).log"
    //ENDIF
    PRINTNL 
    FOR EACH USE
//      IF %(CAB)!="" && %(CABWIZ)!="" && %(../HAS_CSHARP)!=""
//        PRINTNL "%(PROJECT_NAME)": %(USE:%="%")
//        PRINTNL 	@%(DEVENV) %(../NAME).sln /build "%(TARGBUILD)" /project "%(PROJECT_NAME)" /out "%(../NAME).log"
//        PRINTNL 
//      ENDIF
      PRINTNL "%(NAME)":
      PRINTNL 	@%(DEVENV) %(../NAME).sln /build "%(TARGBUILD)" /project "%(NAME)" /out "%(../NAME).log"
      IF %(EXEPROTECT)!=""
        TARGET = %(¯~!OUTPUTPATH)release\%(OUTPUT)
// already done as post process        PRINTNL 	@-%(EXEPROTECT)
        FOR EACH USE
          IF %(TYPE)==DLL
            TARGET = %(¯~!OUTPUTPATH)release\%(OUTPUT)
// UPX fails with DLLs in WM6.1            PRINTNL 	@-%(../EXEPROTECT)
          ENDIF
        ENDFOR
      ENDIF
// do it as post build      IF %(CAB)!="" && %(CABWIZ)!="" && %(../HAS_CSHARP)==""
//        PRINTNL 	@%(CABWIZ) "%(¯~!PATH)%(NAME)_wince.inf" /dest "%(¯~|OUTPUTPATH)" %(CABWIZCLI)
//        PRINTNL 	@move "%(¯~!OUTPUTPATH)%(NAME)_wince.CAB" "%(¯~!OUTPUTPATH)%(PROJECT_NAME).%(CABNAME).cab"
//      ENDIF
      IF %(NSI)!=""
        COMPILE %(NSI) %(¯PATH)%(NAME)_win32.nsi
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
  IF "%(TRANSLATION_QT4)"!=""
    FILE %(¯PATH)%(NAME).tmp.pro
    #include "qmake.inc"
  ENDIF
ENDFOR

FOR EACH LIB,DLL,EXE,CON
 
  IF (%(TYPE)==EXE)
    TARGCONFTYPE = 1
    TARGSUBSYSTEM = %(TARGEXESUBSYSTEM)
  ENDIF

  IF (%(TYPE)==CON)
    TARGCONFTYPE = 1
    TARGSUBSYSTEM = 1
    DEFINE += _CONSOLE
  ENDIF

  IF (%(TYPE)==DLL) || ((%(TYPE)==EXE) && defined(TARGET_PALMOS))
    TARGCONFTYPE = 2
    DEFINE += %(<^NAME)_EXPORTS
    TARGSUBSYSTEM = %(TARGEXESUBSYSTEM)
  ENDIF

  IF (%(TYPE)==LIB)
    TARGCONFTYPE = 4
  ENDIF

  FOR EACH USEBUILT
    ../LIBS += %(NAME).lib
  ENDFOR

  IF "%(USELIB)"
    LIBS += %(USELIB:%=%_%(PLATFORMLIB).lib)
  ENDIF
  
  IFDEF TARGET_WINCE_2003
    LIBS += ccrtrtti.lib
  ENDIF

  IF %(WIN_CERTIFICATE) && DEFINED(TARGET_WINCE)
    TARGSIGN = SHA-1 Hash=%(^WIN_CERTIFICATE)|%(^WIN_CERTIFICATE)
  ELSE
    IF DEFINED(TARGET_WINCE)
      TARGSIGN = SHA-1 Hash=8aeb6e720079eb0494ae9be32cb13a9e3687d913|8aeb6e720079eb0494ae9be32cb13a9e3687d913
    ELSE
      TARGSIGN =
    ENDIF
  ENDIF

  #include "wincab.inc"
  IF %(CAB)!="" && %(CABWIZ)!=""
    FILE %(¯PATH)%(NAME)_cab.vddproj
    PRINTNL "DeployProject"
    PRINTNL {
    PRINTNL "VSVersion" = "3:701"
    PRINTNL "ProjectType" = "8:{AB322303-2255-48EF-A496-5904EB18DA55}"
    PRINTNL "IsWebType" = "8:FALSE"
    PRINTNL "ProjectName" = "8:%(PROJECT_NAME)"
    PRINTNL "LanguageId" = "3:1033"
    PRINTNL "CodePage" = "3:1252"
    PRINTNL "SccProjectName" = "8:"
    PRINTNL "SccLocalPath" = "8:"
    PRINTNL "SccAuxPath" = "8:"
    PRINTNL "SccProvider" = "8:"
    PRINTNL     "Hierarchy"
    PRINTNL     {
      PRINTNL         "Entry"
      PRINTNL         {
      PRINTNL         "MsmKey" = "8:_%(MSMUID)"
      PRINTNL         "OwnerKey" = "8:_UNDEFINED"
      PRINTNL         "MsmSig" = "8:_UNDEFINED"
      PRINTNL         }
      HAS_CONTENT=
      FOR EACH SOURCE
        IF (%(_copy_item))
          ../HAS_CONTENT=1
        ENDIF
      ENDFOR
      FOR EACH INSTALL_CAB
        PRINTNL         "Entry"
        PRINTNL         {
        PRINTNL         "MsmKey" = "8:_%(MSMUID)"
        PRINTNL         "OwnerKey" = "8:_UNDEFINED"
        PRINTNL         "MsmSig" = "8:_UNDEFINED"
        PRINTNL         }
      ENDFOR
      FOR EACH USE, DEP
        IF %(TYPE)==EXE || %(TYPE)==DLL || %(TYPE)==EXE_CSHARP || %(TYPE)==DLL_CSHARP
          PRINTNL         "Entry"
          PRINTNL         {
          PRINTNL         "MsmKey" = "8:_%(MSMUID)"
          PRINTNL         "OwnerKey" = "8:_UNDEFINED"
          PRINTNL         "MsmSig" = "8:_UNDEFINED"
          PRINTNL         }
          HAS_CONTENT=
          FOR EACH SOURCE
            IF (%(_copy_item))
              ../HAS_CONTENT=1
            ENDIF
          ENDFOR
          IF %(HAS_CONTENT)
            PRINTNL         "Entry"
            PRINTNL         {
            PRINTNL         "MsmKey" = "8:_%(MSMUID2)"
            PRINTNL         "OwnerKey" = "8:_UNDEFINED"
            PRINTNL         "MsmSig" = "8:_UNDEFINED"
            PRINTNL         }
          ENDIF
        ENDIF
      ENDFOR
      PRINTNL     }
    PRINTNL     "Configurations"
    PRINTNL     {
    PRINTNL         "%(TARG_DEBUG)"
    PRINTNL         {
    PRINTNL         "DisplayName" = "8:%(TARG_DEBUG)"
    PRINTNL         "IsDebugOnly" = "11:TRUE"
    PRINTNL         "IsReleaseOnly" = "11:FALSE"
    PRINTNL         "OutputFilename" = "8:%(~!`OUTPUTPATH)%(PROJECT_NAME).%(CABNAME).cab"
    PRINTNL         "PackageFileSize" = "3:-2147483648"
    PRINTNL         "CabType" = "3:1"
    IF %(WIN_CERTIFICATE)!=""
      PRINTNL         "SignOutput" = "11:TRUE"
    ELSE
      PRINTNL         "SignOutput" = "11:FALSE"
    ENDIF
    PRINTNL         "CertificateHash" = "8:%(^WIN_CERTIFICATE)"
    PRINTNL         "TimeStampServer" = "8:"
    PRINTNL         }
    PRINTNL         "%(TARG_RELEASE)"
    PRINTNL         {
    PRINTNL         "DisplayName" = "8:%(TARG_RELEASE)"
    PRINTNL         "IsDebugOnly" = "11:FALSE"
    PRINTNL         "IsReleaseOnly" = "11:TRUE"
    PRINTNL         "OutputFilename" = "8:%(~!`OUTPUTPATH)%(PROJECT_NAME).%(CABNAME).cab"
    PRINTNL         "PackageFileSize" = "3:-2147483648"
    PRINTNL         "CabType" = "3:1"
    IF %(WIN_CERTIFICATE)!=""
      PRINTNL         "SignOutput" = "11:TRUE"
    ELSE
      PRINTNL         "SignOutput" = "11:FALSE"
    ENDIF
    PRINTNL         "CertificateHash" = "8:%(^WIN_CERTIFICATE)"
    PRINTNL         "TimeStampServer" = "8:"
    PRINTNL         }
    PRINTNL     }
    PRINTNL     "Deployable"
    PRINTNL     {
    IF %(INSTALL_CAB)!=""
      PRINTNL         "File"
      PRINTNL         {
      FOR EACH INSTALL_CAB
        PRINTNL             "{BA458234-9B71-4F46-8107-E91156377184}:_%(MSMUID)"
        PRINTNL             {
        PRINTNL             "SourcePath" = "8:%(~!`NAME)"
        PRINTNL             "TargetName" = "8:%(<NAME).%(>NAME)"
        PRINTNL             "Tag" = "8:"
        PRINTNL             "Folder" = "8:_9AF581B9382D47739E13BE0AF058DB60"
        PRINTNL             "Condition" = "8:"
        PRINTNL             "Vital" = "11:TRUE"
        PRINTNL             "ReadOnly" = "11:FALSE"
        PRINTNL             "Hidden" = "11:FALSE"
        PRINTNL             "System" = "11:FALSE"
        PRINTNL             "Permanent" = "11:FALSE"
        PRINTNL             "SharedLegacy" = "11:FALSE"
        PRINTNL             "Register" = "3:0"
        PRINTNL             "Exclude" = "11:FALSE"
        PRINTNL             "IsDependency" = "11:FALSE"
        PRINTNL             "IsolateTo" = "8:"
        PRINTNL             }
      ENDFOR
      PRINTNL         }
    ENDIF
    PRINTNL         "Folder"
    PRINTNL         {
    PRINTNL             "{F6DFDB62-1DD1-498B-89B7-27BAC542EE5A}:_446D9EB723864B94AAEBFF061F7277B1"
    PRINTNL             {
    PRINTNL             "Name" = "8:#1916"
    PRINTNL             "Condition" = "8:"
    PRINTNL             "Property" = "8:%CE11%"
    PRINTNL                 "Folders"
    PRINTNL                 {
    PRINTNL                 }
    PRINTNL             }
    PRINTNL             "{D5B00AC1-E6C3-4CC5-90FF-8E0F53D9E5E0}:_9AF581B9382D47739E13BE0AF058DB60"
    PRINTNL             {
    PRINTNL             "DefaultLocation" = "8:%InstallDir%"
    PRINTNL             "Name" = "8:#1920"
    PRINTNL             "Condition" = "8:"
    PRINTNL             "Property" = "8:%InstallDir%"
    PRINTNL                 "Folders"
    PRINTNL                 {
    PRINTNL                 }
    PRINTNL             }
    PRINTNL         }
    PRINTNL         "Product"
    PRINTNL         {
    PRINTNL         "Name" = "8:Microsoft Visual Studio"
    PRINTNL         "ProductName" = "8:%(PROJECT_NAME)"
    PRINTNL         "Manufacturer" = "8:%(PROJECT_VENDOR)"
    PRINTNL         "OSVersionMin" = "8:3.0"
    PRINTNL         "OSVersionMax" = "8:100.0"
    PRINTNL         "NoUninstall" = "11:FALSE"
    PRINTNL         "Compress" = "11:FALSE"
// compression not supported under CE 5.0   
//  PRINTNL         "Compress" = "11:TRUE"
    FOR EACH USE
      IF (%(TYPE)==DLL || %(TYPE)==DLL_CSHARP) && %(SETUP)!=""
        PRINTNL         "CESetupDLL" = "8:_%(MSMUID)"
      ENDIF
    ENDFOR
    PRINTNL          }
    PRINTNL         "ProjectOutput"
    PRINTNL         {
    PRINTNL             "{605BE39E-5046-48BD-AEC0-63C3A46626D5}:_%(MSMUID)"
    PRINTNL             {
    PRINTNL             "SourcePath" = "8:%(~!`OUTPUTPATH)release\\%(OUTPUT)"
    PRINTNL             "TargetName" = "8:"
    PRINTNL             "Tag" = "8:"
    PRINTNL             "Folder" = "8:_9AF581B9382D47739E13BE0AF058DB60"
    PRINTNL             "Condition" = "8:"
    PRINTNL             "Vital" = "11:TRUE"
    PRINTNL             "ReadOnly" = "11:FALSE"
    PRINTNL             "Hidden" = "11:FALSE"
    PRINTNL             "System" = "11:FALSE"
    PRINTNL             "Permanent" = "11:FALSE"
    PRINTNL             "SharedLegacy" = "11:FALSE"
    PRINTNL             "Register" = "3:0"
    PRINTNL             "Exclude" = "11:FALSE"
    PRINTNL             "IsDependency" = "11:FALSE"
    PRINTNL             "IsolateTo" = "8:"
    IF %(TARGDEPLOY) && %(COM_COMPONENT)!=""
      PRINTNL             "ProjectOutputGroupRegister" = "3:1"
    ELSE
      PRINTNL             "ProjectOutputGroupRegister" = "3:0"
    ENDIF
    PRINTNL             "OutputConfiguration" = "8:"
    PRINTNL             "OutputGroupCanonicalName" = "8:Built"
    PRINTNL             "OutputProjectGuid" = "8:%(GUID)"
    PRINTNL             "ShowKeyOutput" = "11:TRUE"
    PRINTNL                 "ExcludeFilters"
    PRINTNL                 {
    PRINTNL                 }
    PRINTNL             }
    IF %(HAS_CONTENT)
      PRINTNL             "{605BE39E-5046-48BD-AEC0-63C3A46626D5}:_%(MSMUID2)"
      PRINTNL             {
      PRINTNL             "SourcePath" = "8:"
      PRINTNL             "TargetName" = "8:"
      PRINTNL             "Tag" = "8:"
      PRINTNL             "Folder" = "8:_9AF581B9382D47739E13BE0AF058DB60"
      PRINTNL             "Condition" = "8:"
      PRINTNL             "Vital" = "11:TRUE"
      PRINTNL             "ReadOnly" = "11:FALSE"
      PRINTNL             "Hidden" = "11:FALSE"
      PRINTNL             "System" = "11:FALSE"
      PRINTNL             "Permanent" = "11:FALSE"
      PRINTNL             "SharedLegacy" = "11:FALSE"
      PRINTNL             "Register" = "3:0"
      PRINTNL             "Exclude" = "11:FALSE"
      PRINTNL             "IsDependency" = "11:FALSE"
      PRINTNL             "IsolateTo" = "8:"
      PRINTNL             "ProjectOutputGroupRegister" = "3:0"
      PRINTNL             "OutputConfiguration" = "8:"
      PRINTNL             "OutputGroupCanonicalName" = "8:ContentFiles"
      PRINTNL             "OutputProjectGuid" = "8:%(GUID)"
      PRINTNL             "ShowKeyOutput" = "11:TRUE"
      PRINTNL                 "ExcludeFilters"
      PRINTNL                 {
      PRINTNL                 }
      PRINTNL             }
    ENDIF
    FOR EACH USE, DEP
      IF %(TYPE)==EXE || %(TYPE)==DLL || %(TYPE)==EXE_CSHARP || %(TYPE)==DLL_CSHARP
        PRINTNL             "{605BE39E-5046-48BD-AEC0-63C3A46626D5}:_%(MSMUID)"
        PRINTNL             {
        PRINTNL             "SourcePath" = "8:%(~!`OUTPUTPATH)release\\%(OUTPUT)"
        PRINTNL             "TargetName" = "8:"
        PRINTNL             "Tag" = "8:"
        PRINTNL             "Folder" = "8:_9AF581B9382D47739E13BE0AF058DB60"
        PRINTNL             "Condition" = "8:"
        PRINTNL             "Vital" = "11:TRUE"
        PRINTNL             "ReadOnly" = "11:FALSE"
        PRINTNL             "Hidden" = "11:FALSE"
        PRINTNL             "System" = "11:FALSE"
        PRINTNL             "Permanent" = "11:FALSE"
        PRINTNL             "SharedLegacy" = "11:FALSE"
        PRINTNL             "Register" = "3:0"
        PRINTNL             "Exclude" = "11:FALSE"
        PRINTNL             "IsDependency" = "11:FALSE"
        PRINTNL             "IsolateTo" = "8:"
        IF %(TARGDEPLOY) && %(COM_COMPONENT)!=""
          PRINTNL             "ProjectOutputGroupRegister" = "3:1"
        ELSE
          PRINTNL             "ProjectOutputGroupRegister" = "3:0"
        ENDIF
        PRINTNL             "OutputConfiguration" = "8:"
        PRINTNL             "OutputGroupCanonicalName" = "8:Built"
        PRINTNL             "OutputProjectGuid" = "8:%(GUID)"
        PRINTNL             "ShowKeyOutput" = "11:TRUE"
        PRINTNL                 "ExcludeFilters"
        PRINTNL                 {
        PRINTNL                 }
        PRINTNL             }
        IF %(HAS_CONTENT)
          PRINTNL             "{605BE39E-5046-48BD-AEC0-63C3A46626D5}:_%(MSMUID2)"
          PRINTNL             {
          PRINTNL             "SourcePath" = "8:"
          PRINTNL             "TargetName" = "8:"
          PRINTNL             "Tag" = "8:"
          PRINTNL             "Folder" = "8:_9AF581B9382D47739E13BE0AF058DB60"
          PRINTNL             "Condition" = "8:"
          PRINTNL             "Vital" = "11:TRUE"
          PRINTNL             "ReadOnly" = "11:FALSE"
          PRINTNL             "Hidden" = "11:FALSE"
          PRINTNL             "System" = "11:FALSE"
          PRINTNL             "Permanent" = "11:FALSE"
          PRINTNL             "SharedLegacy" = "11:FALSE"
          PRINTNL             "Register" = "3:0"
          PRINTNL             "Exclude" = "11:FALSE"
          PRINTNL             "IsDependency" = "11:FALSE"
          PRINTNL             "IsolateTo" = "8:"
          PRINTNL             "ProjectOutputGroupRegister" = "3:0"
          PRINTNL             "OutputConfiguration" = "8:"
          PRINTNL             "OutputGroupCanonicalName" = "8:ContentFiles"
          PRINTNL             "OutputProjectGuid" = "8:%(GUID)"
          PRINTNL             "ShowKeyOutput" = "11:TRUE"
          PRINTNL                 "ExcludeFilters"
          PRINTNL                 {
          PRINTNL                 }
          PRINTNL             }
        ENDIF
      ENDIF
    ENDFOR
    PRINTNL         }
    PRINTNL         "Registry"
    PRINTNL         {
    PRINTNL             "HKLM"
    PRINTNL             {
    PRINTNL                 "Keys"
    PRINTNL                 {
    PRINTNL                 }
    PRINTNL             }
    PRINTNL             "HKCU"
    PRINTNL             {
    PRINTNL                 "Keys"
    PRINTNL                 {
    PRINTNL                 }
    PRINTNL             }
    PRINTNL             "HKCR"
    PRINTNL             {
    PRINTNL                 "Keys"
    PRINTNL                 {
    REGEXE=%(OUTPUT)
    FOR EACH REGISTER_CAB
      PRINTNL                     "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(MSMUID)"
      PRINTNL                     {
      PRINTNL                     "Name" = "8:.%(NAME)"
      PRINTNL                     "Condition" = "8:"
      PRINTNL                         "Keys"
      PRINTNL                         {
      PRINTNL                         }
      PRINTNL                         "Values"
      PRINTNL                         {
      PRINTNL                             "{C3684F64-58E2-4092-8E3C-D7483080DE5A}:_%(_MSMUID)"
      PRINTNL                             {
      PRINTNL                             "Name" = "8:Default"
      PRINTNL                             "Condition" = "8:"
      PRINTNL                             "ValueTypes" = "3:1"
      PRINTNL                             "Value" = "8:%(NAME)file"
      PRINTNL                             }
      PRINTNL                         }
      PRINTNL                     }
      PRINTNL                     "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(MSMUID2)"
      PRINTNL                     {
      PRINTNL                     "Name" = "8:%(NAME)file"
      PRINTNL                     "Condition" = "8:"
      PRINTNL                         "Keys"
      PRINTNL                         {
      PRINTNL                             "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(_MSMUID2)"
      PRINTNL                             {
      PRINTNL                             "Name" = "8:shell"
      PRINTNL                             "Condition" = "8:"
      PRINTNL                                 "Keys"
      PRINTNL                                 {
      PRINTNL                                     "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(MSMUID3)"
      PRINTNL                                     {
      PRINTNL                                     "Name" = "8:open"
      PRINTNL                                     "Condition" = "8:"
      PRINTNL                                         "Keys"
      PRINTNL                                         {
      PRINTNL                                             "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(_MSMUID3)"
      PRINTNL                                             {
      PRINTNL                                             "Name" = "8:command"
      PRINTNL                                             "Condition" = "8:"
      PRINTNL                                                 "Keys"
      PRINTNL                                                 {
      PRINTNL                                                 }
      PRINTNL                                                 "Values"
      PRINTNL                                                 {
      PRINTNL                                                     "{C3684F64-58E2-4092-8E3C-D7483080DE5A}:_%(MSMUID4)"
      PRINTNL                                                     {
      PRINTNL                                                     "Name" = "8:Default"
      PRINTNL                                                     "Condition" = "8:"
      PRINTNL                                                     "ValueTypes" = "3:1"
      IF %(NAME)=="trg"
        PRINTNL                                                     "Value" = "8:\"\"%InstallDir%\\\\%(REGEXE)\"\"\" -trig \"\"\"%%1\"\""
      ELSE
        PRINTNL                                                     "Value" = "8:\"\"%InstallDir%\\\\%(REGEXE)\"\"\" \"\"\"%%1\"\""
      ENDIF
      PRINTNL                                                     }
      PRINTNL                                                 }
      PRINTNL                                             }
      PRINTNL                                         }
      PRINTNL                                         "Values"
      PRINTNL                                         {
      PRINTNL                                         }
      PRINTNL                                     }
      PRINTNL                                 }
      PRINTNL                                 "Values"
      PRINTNL                                 {
      PRINTNL                                 }
      PRINTNL                             }
      PRINTNL                             "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(MSMUID5)"
      PRINTNL                             {
      PRINTNL                             "Name" = "8:DefaultIcon"
      PRINTNL                             "Condition" = "8:"
      PRINTNL                                 "Keys"
      PRINTNL                                 {
      PRINTNL                                 }
      PRINTNL                                 "Values"
      PRINTNL                                 {
      PRINTNL                                     "{C3684F64-58E2-4092-8E3C-D7483080DE5A}:_%(_MSMUID5)"
      PRINTNL                                     {
      PRINTNL                                     "Name" = "8:Default"
      PRINTNL                                     "Condition" = "8:"
      PRINTNL                                     "ValueTypes" = "3:1"
      IF %(NAME)=="trg"
        PRINTNL                                     "Value" = "8:%InstallDir%\\\\tf.ico"
      ELSE
        PRINTNL                                     "Value" = "8:%InstallDir%\\\\%(REGEXE),0"
      ENDIF
      PRINTNL                                     }
      PRINTNL                                 }
      PRINTNL                             }
      PRINTNL                         }
      IF %(NAME)=="trg"
      PRINTNL                         "Values"
      PRINTNL                         {
      PRINTNL                             "{C3684F64-58E2-4092-8E3C-D7483080DE5A}:_97140C2D1F6045388269FCF671F878E8"
      PRINTNL                             {
      PRINTNL                             "Name" = "8:EditFlags"
      PRINTNL                             "Condition" = "8:"
      PRINTNL                             "ValueTypes" = "3:4"
      PRINTNL                             "Value" = "8:1190"
      PRINTNL                             }
      PRINTNL                         }
      ENDIF
      PRINTNL                     }
      IF %(NAME)=="trg"
        PRINTNL                     "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(MSMUID6)"
        PRINTNL                     {
        PRINTNL                     "Name" = "8:MIME"
        PRINTNL                     "Condition" = "8:"
        PRINTNL                         "Keys"
        PRINTNL                         {
        PRINTNL                             "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(MSMUID6)"
        PRINTNL                             {
        PRINTNL                             "Name" = "8:Database"
        PRINTNL                             "Condition" = "8:"
        PRINTNL                                 "Keys"
        PRINTNL                                 {
        PRINTNL                                     "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(MSMUID7)"
        PRINTNL                                     {
        PRINTNL                                     "Name" = "8:Content Type"
        PRINTNL                                     "Condition" = "8:"
        PRINTNL                                         "Keys"
        PRINTNL                                         {
        PRINTNL                                             "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(_MSMUID7)"
        PRINTNL                                             {
        PRINTNL                                             "Name" = "8:application"
        PRINTNL                                             "Condition" = "8:"
        PRINTNL                                                 "Keys"
        PRINTNL                                                 {
        PRINTNL                                                     "{B1EBC968-6B66-4532-A885-748B79AF4477}:_%(_MSMUID7)"
        PRINTNL                                                     {
        PRINTNL                                                     "Name" = "8:tftrg"
        PRINTNL                                                     "Condition" = "8:"
        PRINTNL                                                         "Keys"
        PRINTNL                                                         {
        PRINTNL                                                         }
        PRINTNL                                                         "Values"
        PRINTNL                                                         {
        PRINTNL                                                             "{C3684F64-58E2-4092-8E3C-D7483080DE5A}:_%(MSMUID8)"
        PRINTNL                                                             {
        PRINTNL                                                             "Name" = "8:.trg"
        PRINTNL                                                             "Condition" = "8:"
        PRINTNL                                                             "ValueTypes" = "3:1"
        PRINTNL                                                             "Value" = "8:"
        PRINTNL                                                             }
        PRINTNL                                                         }
        PRINTNL                                                     }
        PRINTNL                                                 }
        PRINTNL                                             }
        PRINTNL                                         }
        PRINTNL                                         "Values"
        PRINTNL                                         {
        PRINTNL                                         }
        PRINTNL                                     }
        PRINTNL                                 }
        PRINTNL                                 "Values"
        PRINTNL                                 {
        PRINTNL                                 }
        PRINTNL                             }
        PRINTNL                         }
        PRINTNL                         "Values"
        PRINTNL                         {
        PRINTNL                         }
        PRINTNL                     }
      ENDIF
    ENDFOR
    PRINTNL                 }
    PRINTNL             }
    PRINTNL             "HKU"
    PRINTNL             {
    PRINTNL                 "Keys"
    PRINTNL                 {
    PRINTNL                 }
    PRINTNL             }
    PRINTNL             "HKPU"
    PRINTNL             {
    PRINTNL                 "Keys"
    PRINTNL                 {
    PRINTNL                 }
    PRINTNL             }
    PRINTNL         }
    PRINTNL         "Shortcut"
    PRINTNL         {
    FOR EACH USE, DEP
      IF %(TYPE)==EXE || %(TYPE)==CON || %(TYPE)==EXE_CSHARP || %(TYPE)==CON_CSHARP
        PRINTNL             "{971D45E9-E38C-49AC-B36E-4C5BEA533350}:_CEC24BEDD7894CBDAEA2C929E630B7A4"
        PRINTNL             {
        PRINTNL             "Name" = "8:%(NAME).lnk"
        PRINTNL             "Arguments" = "8:"
        PRINTNL             "Description" = "8:"
        PRINTNL             "ShowCmd" = "3:1"
        PRINTNL             "Target" = "8:_%(MSMUID)"
        PRINTNL             "Folder" = "8:_446D9EB723864B94AAEBFF061F7277B1"
        PRINTNL             "WorkingFolder" = "8:_9AF581B9382D47739E13BE0AF058DB60"
        PRINTNL             }
      ENDIF
    ENDFOR
    PRINTNL         }
    PRINTNL     }
    PRINTNL }
  ENDIF

  VSPROJ_GUID=%(GUID)
  IF DEFINED(COMPILER_ICL11)
    VS_PROJECT_TYPE=Intel C++ Project
    VSNESTED=%(NAME).vcproj
    FILE %(¯PATH)%(NAME).icproj
    PRINTNL <?xml version="1.0" encoding="Windows-1252"?>
    PRINTNL <VisualStudioProject
    PRINTNL 	ProjectType="%(VS_PROJECT_TYPE)"
    PRINTNL 	Version="%(ICC_COMPILERVERSION)"
    PRINTNL 	Name="%(NAME)"
    PRINTNL 	ProjectGUID="%(VSPROJ_GUID)"
    IF "%(VSNESTED)"
      PRINTNL 	VCNestedProjectGUID="%(GUIDBIS)"
      PRINTNL 	VCNestedProjectFileName="%(VSNESTED)">
    ENDIF
    PRINTNL 	<Configurations/>
    PRINTNL 	<Files/>
    PRINTNL </VisualStudioProject>

  ELIF DEFINED(COMPILER_ICL9)
   VS_PROJECT_TYPE=Intel C++ Project
    PREBUILTOOL=PreBuildEventTool
    POSTBUILTOOL=PostBuildEventTool
    PRELINKTOOL=PreLinkEventTool
    CUSTOM_TOOL=CustomTool
    IDL_TOOL=MidlCmplrTool
    CPP_TOOL=CppCmplrTool
    RES_TOOL=ResCmplrTool
    LIB_TOOL=LibrarianTool
    BSC_TOOL=BscMakeTool
    LINKER_TOOL=LinkerTool
    CMD_NEWLINE=#xA;
    VSNESTED=%(NAME).vcproj
    PROJ_IS_ICL=yes
    PRECOMPILED_HEADER=
    FILE %(¯PATH)%(NAME).icproj
    #include "vsproj.inc"
    IFDEF TARGET_WINCE
//      FILE %(¯PATH)%(NAME).icproj.%(MACHINE).%(USER).user
//      FILE %(¯PATH)%(NAME).icproj.user
//      IF defined(COREMAKE_STATIC)
//        EXECUTABLE_RELEASE = "%CSIDL_PROGRAM_FILES%\%(PROJECT_PATH)\$(TargetFileName)"
//      ELSE
//        EXECUTABLE_RELEASE = "%CSIDL_PROGRAM_FILES%\%(PLATFORMNAME)\$(TargetFileName)"
//      ENDIF
//      EXECUTABLE_DEBUG = %(RemoteExecutable)
//      #include "vsproj-user.inc"
    ENDIF
    IFDEF TARGET_PALMOS
      FILE %(¯PATH)%(NAME).icproj.user
      EXECUTABLE_RELEASE = %(~PALMSDK5R4)\tools\Palm_OS_54_Simulator\release\PalmSim_54_rel.exe
      EXECUTABLE_DEBUG = %(~PALMSDK5R4)\tools\Palm_OS_54_Simulator\debug\PalmSim_54_dbg.exe
      #include "vsproj-user.inc"
    ENDIF
    
    // clean for the next project
    COMPILERVERSION=8.00
    VSPROJ_GUID=%(GUIDBIS)
    VSNESTED=
    PROJ_IS_ICL=
  ENDIF
  
  VS_PROJECT_TYPE=Visual C++
  PREBUILTOOL=VCPreBuildEventTool
  POSTBUILTOOL=VCPostBuildEventTool
  PRELINKTOOL=VCPreLinkEventTool
  CUSTOM_TOOL=VCCustomBuildTool
  IDL_TOOL=VCMIDLTool
  CPP_TOOL=VCCLCompilerTool
  RES_TOOL=VCResourceCompilerTool
  LIB_TOOL=VCLibrarianTool
  BSC_TOOL=VCBscMakeTool
  LINKER_TOOL=VCLinkerTool
  CMD_NEWLINE=#x0D;&#x0A;
  FILE %(¯PATH)%(NAME).vcproj
  #include "vsproj.inc"
  IFDEF TARGET_WINCE
//    FILE %(¯PATH)%(NAME).vcproj.%(MACHINE).%(USER).user
//    FILE %(¯PATH)%(NAME).vcproj.user
//    IF defined(COREMAKE_STATIC)
//      EXECUTABLE_RELEASE = "%CSIDL_PROGRAM_FILES%\%(PROJECT_PATH)\$(TargetFileName)"
//    ELSE
//      EXECUTABLE_RELEASE = "%CSIDL_PROGRAM_FILES%\%(PLATFORMNAME)\$(TargetFileName)"
//    ENDIF
//    EXECUTABLE_DEBUG = %(RemoteExecutable)
//    #include "vsproj-user.inc"
  ENDIF
  IFDEF TARGET_PALMOS
    FILE %(¯PATH)%(NAME).vcproj.user
    EXECUTABLE_RELEASE = %(~PALMSDK5R4)\tools\Palm_OS_54_Simulator\release\PalmSim_54_rel.exe
    EXECUTABLE_DEBUG = %(~PALMSDK5R4)\tools\Palm_OS_54_Simulator\debug\PalmSim_54_dbg.exe
    WORKINGDIR_DEBUG = %(~`|OUTPUTPATH)
    #include "vsproj-user.inc"
  ENDIF

ENDFOR

IFNDEF TARGET_NO_DOT_NET
FOR EACH LIB_CSHARP,DLL_CSHARP,EXE_CSHARP,CON_CSHARP
  FILE %(PATH)%(NAME).csproj
  PRINTNL <Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003" ToolsVersion="3.5">
  PRINTNL   <PropertyGroup>
  PRINTNL     <Configuration Condition=" '$(Configuration)' == '' ">%(TARG_DEBUG)</Configuration>
  PRINTNL     <Platform Condition=" '$(Platform)' == '' ">AnyCPU</Platform>
  PRINTNL     <ProductVersion>%(CSPROJVERSION)</ProductVersion>
  PRINTNL     <SchemaVersion>2.0</SchemaVersion>
  PRINTNL     <ProjectGuid>%(GUID)</ProjectGuid>
  IF %(TYPE)==LIB_CSHARP || %(TYPE)==DLL_CSHARP
    PRINTNL     <OutputType>Library</OutputType>
  ELIF %(TYPE)==EXE_CSHARP
    PRINTNL     <OutputType>WinExe</OutputType>
  ENDIF
  PRINTNL     <AppDesignerFolder>Properties</AppDesignerFolder>
  PRINTNL     <RootNamespace>%(NAME)</RootNamespace>
  PRINTNL     <AssemblyName>%(NAME)</AssemblyName>
  IFDEF TARGET_WINCE
    PRINTNL     <ProjectTypeGuids>{4D628B5B-2FBC-4AA6-8C16-197242AEB884};{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}</ProjectTypeGuids>
    IFDEF TARGET_SMARTPHONE
      PRINTNL     <PlatformFamilyName>Smartphone</PlatformFamilyName>
    ELSE
      IFDEF TARGET_NO_WINMO
        PRINTNL     <PlatformFamilyName>WindowsCE</PlatformFamilyName>
      ELSE
        PRINTNL     <PlatformFamilyName>PocketPC</PlatformFamilyName>
      ENDIF
    ENDIF
    PRINTNL     <PlatformID>%(WINCE_CS_UUID)</PlatformID>
    PRINTNL     <OSVersion>%(WINCE_VERSION)</OSVersion>
    PRINTNL     <DeployDirSuffix>%(PLATFORMNAME)</DeployDirSuffix>
    PRINTNL     <FormFactorID>
    PRINTNL     </FormFactorID>
  ENDIF
  IFDEF COREMAKE_NET_FRAMEWORK_35
    PRINTNL     <TargetFrameworkVersion>v3.5</TargetFrameworkVersion>
  ELSE
    PRINTNL     <TargetFrameworkVersion>v2.0</TargetFrameworkVersion>
  ENDIF
  PRINTNL   </PropertyGroup>
  PRINTNL   <PropertyGroup Condition=" '$(Configuration)|$(Platform)' == '%(TARG_DEBUG)|AnyCPU' ">
  PRINTNL     <DebugSymbols>true</DebugSymbols>
  PRINTNL     <DebugType>full</DebugType>
  PRINTNL     <Optimize>false</Optimize>
  PRINTNL     <OutputPath>%(~!OUTPUTPATH)debug%(OUTPUTDIR)</OutputPath>
  PRINTNL     <DefineConstants>TRACE%(TARGDEF:%=;%)%(DEFINE:%=;%)%(DEFINE_DEBUG:%=;%)</DefineConstants>
  IFDEF TARGET_WINCE
    PRINTNL     <NoStdLib>true</NoStdLib>
    PRINTNL     <NoConfig>true</NoConfig>
    PRINTNL     <FileAlignment>512</FileAlignment>
  ENDIF
  PRINTNL     <ErrorReport>prompt</ErrorReport>
  PRINTNL     <WarningLevel>4</WarningLevel>
  IFDEF TARGET_WINCE
    PRINTNL     <GenerateSerializationAssemblies>Off</GenerateSerializationAssemblies>
    PRINTNL     <CheckForOverflowUnderflow>true</CheckForOverflowUnderflow>
  ENDIF
  PRINTNL   </PropertyGroup>
  PRINTNL   <PropertyGroup Condition=" '$(Configuration)|$(Platform)' == '%(TARG_RELEASE)|AnyCPU' ">
  PRINTNL     <DebugSymbols>false</DebugSymbols>
  PRINTNL     <DebugType>pdbonly</DebugType>
  PRINTNL     <Optimize>true</Optimize>
  PRINTNL     <OutputPath>%(~!OUTPUTPATH)release%(OUTPUTDIR)</OutputPath>
  PRINTNL     <DefineConstants>TRACE%(TARGDEF:%=;%)%(DEFINE:%=;%)%(DEFINE_RELEASE:%=;%)</DefineConstants>
  IFDEF TARGET_WINCE
    PRINTNL     <NoStdLib>true</NoStdLib>
    PRINTNL     <NoConfig>true</NoConfig>
    PRINTNL     <FileAlignment>512</FileAlignment>
  ENDIF
  PRINTNL     <ErrorReport>prompt</ErrorReport>
  PRINTNL     <WarningLevel>4</WarningLevel>
  IFDEF TARGET_WINCE
    PRINTNL     <GenerateSerializationAssemblies>Off</GenerateSerializationAssemblies>
    PRINTNL     <CheckForOverflowUnderflow>true</CheckForOverflowUnderflow>
  ENDIF
  PRINTNL   </PropertyGroup>
  PRINTNL   <ItemGroup>
  IFDEF TARGET_WINCE
    PRINTNL     <Reference Include="mscorlib" />
  ENDIF
  IFDEF COREMAKE_NET_FRAMEWORK_35
    PRINTNL     <Reference Include="System.Core">
    PRINTNL       <RequiredTargetFramework>3.5</RequiredTargetFramework>
    PRINTNL     </Reference>
  ENDIF
  FOR EACH REFERENCES
    PRINTNL     <Reference Include="%(NAME)" />
  ENDFOR
  PRINTNL   </ItemGroup>
  PRINTNL   <ItemGroup>
  FOR EACH SOURCE
    PRINTNL     <Compile Include="%(~!NAME)">
    IF %(control)!=""
      PRINTNL       <SubType>UserControl</SubType>
    ELIF %(form)!=""
      PRINTNL       <SubType>Form</SubType>
    ELSE
      PRINTNL       <SubType>Code</SubType>
    ENDIF
    PRINTNL     </Compile>
    IF %(designer)!=""
      PRINTNL     <Compile Include="%(~!PATH)%(<NAME).Designer.cs">
      PRINTNL       <DependentUpon>%(~!NAME)</DependentUpon>
      PRINTNL     </Compile>
    ENDIF
    IF %(resx)!=""
      PRINTNL     <EmbeddedResource Include="%(~!PATH)%(<NAME).resx">
      PRINTNL       <SubType>Designer</SubType>
      PRINTNL       <DependentUpon>%(~!NAME)</DependentUpon>
      PRINTNL     </EmbeddedResource>
    ENDIF
  ENDFOR
  FOR EACH CS_RESOURCES
    PRINTNL     <EmbeddedResource Include="%(NAME).resx">
    PRINTNL       <Generator>ResXFileCodeGenerator</Generator>
    PRINTNL       <LastGenOutput>%(~!PATH)%(NAME).Designer.cs</LastGenOutput>
    PRINTNL       <SubType>Designer</SubType>
    PRINTNL     </EmbeddedResource>
    PRINTNL     <Compile Include="%(NAME).Designer.cs">
    PRINTNL       <AutoGen>True</AutoGen>
    PRINTNL       <DependentUpon>%(~!PATH)%(<NAME).resx</DependentUpon>
    PRINTNL     </Compile>
  ENDFOR
  FOR EACH CS_SETTINGS
    PRINTNL     <None Include="%(NAME).settings">
    PRINTNL       <Generator>SettingsSingleFileGenerator</Generator>
    PRINTNL       <LastGenOutput>%(~!PATH)%(NAME).Designer.cs</LastGenOutput>
    PRINTNL     </None>
    PRINTNL     <Compile Include="%(NAME).Designer.cs">
    PRINTNL       <AutoGen>True</AutoGen>
    PRINTNL       <DependentUpon>%(~!PATH)%(<NAME).settings</DependentUpon>
    PRINTNL       <DesignTimeSharedInput>True</DesignTimeSharedInput>
    PRINTNL     </Compile>
  ENDFOR
  PRINTNL     <Service Include="{94E38DFF-614B-4cbd-B67C-F211BB35CE8B}" />
  PRINTNL   </ItemGroup>
  PRINTNL   <ItemGroup>
  FOR EACH USE, DEP
    IF %(TYPE)==LIB_CSHARP || %(TYPE)==DLL_CSHARP || %(TYPE)==EXE_CSHARP || %(TYPE)==CON_CSHARP
      PRINTNL     <ProjectReference Include="%(~!PATH)%(NAME).csproj">
    ELSE
      PRINTNL     <ProjectReference Include="%(¯~!PATH)%(NAME).%(PROJECT_EXT)">
    ENDIF
    PRINTNL       <Project>%(GUID)</Project>
    PRINTNL       <Name>%(NAME)</Name>
    PRINTNL       <Private>False</Private>
    PRINTNL     </ProjectReference>
  ENDFOR
  PRINTNL   </ItemGroup>
  PRINTNL   <ItemGroup>
  FOR EACH REFERENCES_COM
    PRINTNL     <COMReference Include="%(NAME)">
    IF %(com_guid)!=""
      PRINTNL       <Guid>{%(com_guid)}</Guid>
    ENDIF
    PRINTNL       <VersionMajor>1</VersionMajor>
    PRINTNL       <VersionMinor>0</VersionMinor>
    PRINTNL       <Lcid>0</Lcid>
    PRINTNL       <WrapperTool>tlbimp</WrapperTool>
    PRINTNL       <Isolated>False</Isolated>
    PRINTNL     </COMReference>
  ENDFOR
  PRINTNL   </ItemGroup>
  IFDEF TARGET_WINCE
    PRINTNL   <Import Condition="'$(TargetFrameworkVersion)' == 'v1.0'" Project="$(MSBuildBinPath)\Microsoft.CompactFramework.CSharp.v1.targets" />
    PRINTNL   <Import Condition="'$(TargetFrameworkVersion)' != 'v1.0'" Project="$(MSBuildBinPath)\Microsoft.CompactFramework.CSharp.targets" />
    PRINTNL   <ProjectExtensions>
    PRINTNL     <VisualStudio>
    PRINTNL       <FlavorProperties GUID="{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}">
    PRINTNL         <HostingProcess disable="1" />
    PRINTNL       </FlavorProperties>
    PRINTNL     </VisualStudio>
    PRINTNL   </ProjectExtensions>
  ELSE
    PRINTNL   <Import Project="$(MSBuildBinPath)\Microsoft.CSharp.targets" />
  ENDIF
  PRINTNL   <!-- To modify your build process, add your task inside one of the targets below and uncomment it.
  PRINTNL        Other similar extension points exist, see Microsoft.Common.targets.
  PRINTNL   <Target Name="BeforeBuild">
  PRINTNL   </Target>
  PRINTNL   <Target Name="AfterBuild">
  PRINTNL   </Target>
  PRINTNL   -->
  PRINTNL </Project>
ENDFOR
ENDIF