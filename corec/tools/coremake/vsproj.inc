IFNDEF COREMAKE_UNICODE
  TARGUNICODETYPE = 2
ELSE
  TARGUNICODETYPE = 1
ENDIF

IF DEFINED(COREMAKE_MSVCRT) || "%(USE_MFC)"=="dll"
  USE_DLL_RUNTIME=1
ENDIF

IF %(TYPE)==DLL
  DEFINE += _USRDLL
ENDIF

IF "%(USE_DLL_RUNTIME)"!="" && "%(USE_MFC)"=="dll"
  DEFINE += _AFXDLL
ENDIF

IF %(USE_MFC)!="" && %(USE_MFC)!="dll"
  LIBS_RELEASE += uafxcw.lib
  LIBS_DEBUG   += uafxcwd.lib
ENDIF

IF %(USE_ATL) && %(USE_ATL)!="dll"
  DEFINE += _ATL_STATIC_REGISTRY
ENDIF

IFDEF TARGET_PALMOS
  IF %(PALMSDK5R4)==""
    PALMSDK5R4 = C:\Program Files\ACCESS\Garnet OS Development Suite\sdk-5r4
  ENDIF
  TARGINC += ;&quot;%(~PALMSDK5R4)\include&quot;
  TARGINC += ;&quot;%(~PALMSDK5R4)\include\Core&quot;
  TARGINC += ;&quot;%(~PALMSDK5R4)\include\Core\Hardware&quot;
  TARGINC += ;&quot;%(~PALMSDK5R4)\include\Core\System&quot;
  TARGINC += ;&quot;%(~PALMSDK5R4)\include\Core\UI&quot;
  TARGINC += ;&quot;%(~PALMSDK5R4)\include\Dynamic&quot;
  TARGINC += ;&quot;%(~PALMSDK5R4)\include\Libraries&quot;
ENDIF

DEFINE += STRICT
IF "%(VSVERSION)"=="8.00" || "%(VSVERSION)"=="9.00"
  IFNDEF MSVC_SEE_DEPRECATE
    DEFINE += _CRT_SECURE_NO_DEPRECATE
    DEFINE += _CRT_NONSTDC_NO_DEPRECATE
    DEFINE += _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
  ENDIF
ENDIF

HAS_IDL =
FOR EACH SOURCE
  IF (%(>NAME)==idl)
    ../HAS_IDL = 1
  ENDIF
ENDFOR

IF %(DOXYGEN)
  COMPILE %(DOXYGEN) "%(PATH)%(`PROJECT_NAME).doxyfile"
ENDIF

IF "%(PRECOMPILED_HEADER)"
//  IF "%(PRECOMPILED_HEADER_FORCED)"=="no"
//    PCH_FILE = %(<PRECOMPILED_HEADER).%(>PRECOMPILED_HEADER)
//  ELSE
    PCH_FILE = %(PRECOMPILED_HEADER)
//  ENDIF
  PCH_SOURCE=%(PRECOMPILED_HEADER)
ENDIF

IF "%(OUTPUT_DIR)"!=""
  OUTPUTDIR = \%(~§OUTPUT_DIR)
ENDIF

// trick because it's not possible to do the basic assignment with &
CMD_NEWLINE= &%(CMD_NEWLINE)

PRINTNL <?xml version="1.0" encoding="Windows-1252"?>
PRINTNL <VisualStudioProject
PRINTNL 	ProjectType="%(VS_PROJECT_TYPE)"
PRINTNL 	Version="%(COMPILERVERSION)"
PRINTNL 	Name="%(NAME)"
PRINTNL 	ProjectGUID="%(VSPROJ_GUID)"
PRINTNL 	RootNamespace="%(NAME)"
IF "%(VSNESTED)"
  PRINTNL 	VCNestedProjectGUID="%(GUIDBIS)"
  PRINTNL 	VCNestedProjectFileName="%(VSNESTED)"
ENDIF
IF %(PLATFORM_KEYWORD)
  PRINTNL 	Keyword="%(PLATFORM_KEYWORD)"
ELIF %(MANAGED_CLR)!=""
  PRINTNL 	Keyword="ManagedCProj"
ELIF %(USE_MFC)!="")
  IF %(COM_COMPONENT)!=""
    PRINTNL 	Keyword="MFCActiveXProj"
  ELSE
    PRINTNL 	Keyword="MFCProj"
  ENDIF
ENDIF
PRINTNL 	>
PRINTNL 	<Platforms>
PRINTNL 		<Platform
PRINTNL 			Name="%(TARGNAME)"
PRINTNL 		/>
PRINTNL 	</Platforms>
PRINTNL 	<ToolFiles>
IF DEFINED(CARBIDE_VS)
  PRINTNL 		<DefaultToolFile
  PRINTNL 			FileName="Carbide.vs.rules"
  PRINTNL 		/>
ENDIF
PRINTNL 	</ToolFiles>
PRINTNL 	<Configurations>
IF "%(PROJ_IS_ICL)"
  PRINTNL 		<Configuration
  PRINTNL 			Name="%(TARG_DEBUG)|%(TARGNAME)">
  PRINTNL 			<Tool
  PRINTNL 				Name="GeneralTool"
ELSE
  PRINTNL 		<Configuration
  PRINTNL 			Name="%(TARG_DEBUG)|%(TARGNAME)"
ENDIF
PRINTNL 			OutputDirectory="%(¯~!OUTPUTPATH)debug%(OUTPUTDIR)"
IF DEFINED(CARBIDE_VS)
  PRINTNL 			IntermediateDirectory="Deb_WINSCW"
ELSE
  PRINTNL 			IntermediateDirectory="%(¯~!BUILDPATH)debug\%(~§PATH)\%(NAME)"
ENDIF
PRINTNL 			ConfigurationType="%(TARGCONFTYPE)"
// does not work with MFC apps
//IF "%(USE_DLL_RUNTIME)"==""
//  PRINTNL 			ATLMinimizesCRunTimeLibraryUsage="true"
//ENDIF
IF %(MANAGED_CLR)!=""
  PRINTNL 			ManagedExtensions="1"
ENDIF
IF "%(USE_MFC)"
  IF "%(USE_MFC)"=="dll"
    PRINTNL 			UseOfMFC="2"
  ELSE
    PRINTNL 			UseOfMFC="1"
  ENDIF
ENDIF
IF "%(USE_ATL)"
  IF %(USE_ATL)=="dll"
    PRINTNL 			UseOfATL="2"
  ELSE
    PRINTNL 			UseOfATL="1"
  ENDIF
ENDIF
PRINTNL 			CharacterSet="%(TARGUNICODETYPE)"
IF "%(PROJ_IS_ICL)"
  PRINTNL 				/>
ELSE
  PRINTNL 			>
ENDIF
PRINTNL 			<Tool
PRINTNL 				Name="%(PREBUILTOOL)"
IF "%(PRECOMMAND_DEB)"
  PRINTNL 				CommandLine="%(PRECOMMAND_DEB)"
ENDIF
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(CUSTOM_TOOL)"
PRINTNL 			/>
IF DEFINED(CARBIDE_VS)
  PRINTNL 			<Tool
  PRINTNL 				Name="Carbide.vs.build.rule"
  PRINTNL 				sdkname="%(SDKNAME)"
  PRINTNL 				target="WINSCW"
  PRINTNL 			/>
ENDIF
PRINTNL 			<Tool
PRINTNL 				Name="VCXMLDataGeneratorTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCWebServiceProxyGeneratorTool"
PRINTNL 			/>
ADDOPTIONS = %(TARGLFLAGS)
FOR EACH SOURCE_OBJ
  ../ADDOPTIONS += %(NAME)
ENDFOR
PRINTNL 			<Tool
IF "%(PROJ_IS_ICL)"
  PRINTNL 				Name="MidlCmplrTool"
  PRINTNL 				PreprocessorDefinitions="_DEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_DEBUG:%=;%)"
  PRINTNL 				AdditionalIncludeDirectories="%(@&INCLUDE:%=;%)%(@&INCLUDE_DEBUG:%=;%)%(@&SYSINCLUDE:%=;%);%(&TARGINC)%(@&CONFIG_INCLUDE:%=;%)"
  PRINTNL 				MkTypLibCompatible="false"
  PRINTNL 				TypeLibraryName="$(TargetDir)$(ProjectName).tlb"
  PRINTNL 				OutputDirectory="%(¯~!BUILDPATH)%(~§PATH)"
  PRINTNL 				HeaderFileName="$(InputName)_i.h"
  PRINTNL 			/>
  PRINTNL 			<Tool
  PRINTNL 				Name="CppCmplrTool"
ELSE
  PRINTNL 				Name="%(IDL_TOOL)"
  PRINTNL 				PreprocessorDefinitions="_DEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_DEBUG:%=;%)"
  PRINTNL 				AdditionalIncludeDirectories="%(@&INCLUDE:%=;%)%(@&INCLUDE_DEBUG:%=;%)%(@&SYSINCLUDE:%=;%);%(TARGINC)%(@&CONFIG_INCLUDE:%=;%)"
  PRINTNL 				MkTypLibCompatible="false"
  PRINTNL 				TypeLibraryName="$(TargetDir)$(ProjectName).tlb"
  PRINTNL 				OutputDirectory="%(¯~!BUILDPATH)%(~§PATH)"
  PRINTNL 				HeaderFileName="$(InputName)_i.h"
  PRINTNL 			/>
  PRINTNL 			<Tool
  PRINTNL 				Name="%(CPP_TOOL)"
ENDIF
IF "%(TARGCFLAGS)"!=""
  PRINTNL 				AdditionalOptions="%(TARGCFLAGS)"
ENDIF
PRINTNL 				Optimization="0"
IF %(HAS_IDL)
  INCLUDE_DEBUG   += %(OUTPUTPATH)debug\%(OUTPUT_DIR)
  INCLUDE_RELEASE += %(OUTPUTPATH)release\%(OUTPUT_DIR)
ENDIF
IF "%(FULL_INLINE)"
  PRINTNL 				InlineFunctionExpansion="2"
ELIF "%(LIMIT_INLINE)"
  PRINTNL 				InlineFunctionExpansion="1"
ENDIF
IF ("%(HAS_IDL)") || ("%(HEADER_QT4)"!="") || ("%(UI_FORM_QT4)"!="")
  INCLUDE += %(¯|BUILDPATH)%(PATH)
ENDIF
IF "%(HEADER_QT4)"!=""
  INCLUDE_DEBUG += %(¯|BUILDPATH)debug\%(PATH)GeneratedFiles
  INCLUDE_RELEASE += %(¯|BUILDPATH)release\%(PATH)GeneratedFiles
ENDIF
PRINTNL 				AdditionalIncludeDirectories="%(@&INCLUDE:%=;%)%(@&INCLUDE_DEBUG:%=;%)%(@&SYSINCLUDE:%=;%);%(&TARGINC)%(@&CONFIG_INCLUDE:%=;%)"
PRINTNL 				PreprocessorDefinitions="_DEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_DEBUG:%=;%)"
IF DEFINED(CARBIDE_VS)
PRINTNL 				IgnoreStandardIncludePath="true"
ENDIF
IF %(MANAGED_CLR)==""
  PRINTNL 				BasicRuntimeChecks="3"
ENDIF
IF "%(USE_DLL_RUNTIME)"!="" || %(MANAGED_CLR)!=""
  PRINTNL 				RuntimeLibrary="3"
ELSE
  PRINTNL 				RuntimeLibrary="1"
ENDIF
IF defined(NO_RUNTIME_CHECK)
  PRINTNL 				BufferSecurityCheck="false"
ELSE
  PRINTNL 				BufferSecurityCheck="true"
ENDIF
IF "%(PRECOMPILED_HEADER)"
  PRINTNL 				UsePrecompiledHeader="2"
  PRINTNL 				PrecompiledHeaderThrough="%(PCH_FILE)"
  IF "%(PRECOMPILED_HEADER_FORCED)"!="no"
    PRINTNL 				ForcedIncludeFiles="%(PCH_FILE)"
  ENDIF
ENDIF
PRINTNL 				ProgramDataBaseFileName="$(OutDir)\$(ProjectName).pdb"
PRINTNL 				WarningLevel="3"
IF "%(TARGMACHINE)"!=""
  PRINTNL 				TargetMachine="%(TARGMACHINE)"
ENDIF
IF "%(TARGCOMPILEARCH)"!=""
  PRINTNL 				CompileForArchitecture="%(TARGCOMPILEARCH)"
ENDIF
IF %(TAR64)
  PRINTNL 				Detect64BitPortabilityProblems="true"
ENDIF
PRINTNL 				DebugInformationFormat="3"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCManagedResourceCompilerTool"
PRINTNL 				AdditionalIncludeDirectories="$(TargetDir)"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(RES_TOOL)"
PRINTNL 				PreprocessorDefinitions="_DEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_DEBUG:%=;%)"
PRINTNL 				AdditionalIncludeDirectories="$(TargetDir)%(@&INCLUDERC:%=;%)%(@&INCLUDE:%=;%)%(@&CONFIG_INCLUDE:%=;%)"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(PRELINKTOOL)"
PRINTNL 			/>
PRINTNL 			<Tool
IF (%(TARGCONFTYPE)==4)
  PRINTNL 				Name="%(LIB_TOOL)"
ELSE
  PRINTNL 				Name="%(LINKER_TOOL)"
ENDIF
IF (%(COM_COMPONENT)!="" || %(MANAGED_CLR)!="") && !defined(TARGET_WINCE)
  PRINTNL 				RegisterOutput="true"
ENDIF
IF ("%(LIBS)"||"%(LIBS_DEBUG)") && !(%(TYPE)==LIB)
  PRINTNL 				AdditionalDependencies="%(LIBS) %(LIBS_DEBUG)"
ENDIF
IF "%(ADDOPTIONS)"!=""
  PRINTNL 				AdditionalOptions="%(ADDOPTIONS)"
ENDIF
PRINTNL 				OutputFile="$(OutDir)\%(OUTPUT)"
IF "%(USEBUILT)"!=""
  PRINTNL 				AdditionalLibraryDirectories="%(@LIBINCLUDE:%=;%)%(@LIBINCLUDE_DEBUG:%=;%);%(~!OUTPUTPATH)debug\"
ELSE
  PRINTNL 				AdditionalLibraryDirectories="%(@LIBINCLUDE:%=;%)%(@LIBINCLUDE_DEBUG:%=;%)"
ENDIF
IF "%(USE_DLL_RUNTIME)"!="" && "%(MANIFEST_DEBUG_DEP)"
  PRINTNL 				AdditionalManifestDependencies="%(MANIFEST_DEBUG_DEP)"
ENDIF
IF %(TARGDEPLOY)
  PRINTNL 				DelayLoadDLLs="$(NOINHERIT)"
ENDIF
IF %(DEF)
  PRINTNL 				ModuleDefinitionFile="%(~!DEF)"
ENDIF
PRINTNL 				GenerateDebugInformation="true"
IF %(MANAGED_CLR)!=""
  PRINTNL 				AssemblyDebug="1"
ENDIF
PRINTNL 				GenerateMapFile="true"
PRINTNL 				MapFileName="$(IntDir)\$(ProjectName).map"
IF %(TARGSUBSYSTEM)
  PRINTNL 				SubSystem="%(TARGSUBSYSTEM)"
ENDIF
IF defined(TARGET_WINCE) && (%(TYPE)==EXE)
  PRINTNL 				StackReserveSize="65536"
ENDIF
IF "%(USE_MFC)" && "%(COM_COMPONENT)"=="" && defined(TARGET_WINCE)
  PRINTNL 				EntryPointSymbol="WinMainCRTStartup"
ENDIF
IF "%(TARGMACHINE)"
  PRINTNL 				TargetMachine="%(TARGMACHINE)"
ENDIF
IF %(MANAGED_CLR)!=""
  PRINTNL 				CLRUnmanagedCodeCheck="true"
ENDIF
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCALinkTool"
PRINTNL 			/>
IF (%(TYPE)!=LIB) && %(SKIP_MANIFEST)==""
  PRINTNL 			<Tool
  PRINTNL 				Name="VCManifestTool"
  IF "%(PROJECT_NAME)"
    ASSEMBLYID=%(PROJECT_NAME), processorArchitecture=%(TARGARCH)
    IF "%(PROJECT_VERSION)" && "%(PROJECT_BUILD)"
      ASSEMBLYID+=,version=%(PROJECT_VERSION).%(PROJECT_BUILD)
    ELSE
      IF "%(PROJECT_VERSION)"
        ASSEMBLYID+=,version=%(PROJECT_VERSION).0
      ENDIF
    ENDIF
    PRINTNL 				AssemblyIdentity="%(ASSEMBLYID)"
  ENDIF
  PRINTNL 			/>
ENDIF
PRINTNL 			<Tool
PRINTNL 				Name="VCXDCMakeTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(BSC_TOOL)"
PRINTNL 			/>
IF ("%(TARGSIGN)"!="") && (%(TYPE)!=LIB)
  PRINTNL 			<Tool
  PRINTNL 				Name="VCCodeSignTool"
  PRINTNL 				AuthenticodeSignature="true"
  PRINTNL 				Certificate="%(TARGSIGN)"
  PRINTNL 			/>
ENDIF
PRINTNL 			<Tool
PRINTNL 				Name="VCFxCopTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCAppVerifierTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCWebDeploymentTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(POSTBUILTOOL)"
POSTCOMMAND =
FOR EACH SOURCE
  IF (%(>NAME)==r)
    ../POSTCOMMAND += cmd /c move /y &quot;$(TargetPath)&quot; &quot;$(TargetPath).tmp&quot;%(CMD_NEWLINE)
    ../POSTCOMMAND += &quot;%(CONFIG_QUICKTIME_SDK)\Tools\RezWack.exe&quot; -f -d &quot;$(TargetPath).tmp&quot; -r &quot;$(IntDir)\%(<NAME).qtr&quot; -o &quot;$(TargetPath)&quot;%(CMD_NEWLINE)
    ../POSTCOMMAND += del /f &quot;$(TargetPath).tmp&quot;%(CMD_NEWLINE)
  ENDIF
ENDFOR
IF "%(TRANSLATION_QT4)"!=""
  POSTCOMMAND += &quot;$(QTDIR)/bin/lupdate&quot; %(NAME).tmp.pro%(CMD_NEWLINE)
  POSTCOMMAND += &quot;$(QTDIR)/bin/lrelease&quot; %(NAME).tmp.pro%(CMD_NEWLINE)
  FOR EACH TRANSLATION_QT4
    ../POSTCOMMAND += copy /y &quot;%(~?NAME)%(<NAME).qm&quot; &quot;$(OutDir)&quot;%(CMD_NEWLINE)
  ENDFOR
ENDIF
IF (%(TYPE)!=LIB) && %(CONFIG_INSTALL_DIR)
    POSTCOMMAND += copy /y &quot;$(TargetPath)&quot; &quot;%(CONFIG_INSTALL_DIR)&quot;%(CMD_NEWLINE)
ENDIF
IF "%(EXPORT_SVN)"
  IF (%(TYPE)==LIB)
    POSTCOMMAND += if not exist &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)static\debug\&quot; mkdir &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)static\debug\&quot;%(CMD_NEWLINE)copy /y &quot;$(TargetPath)&quot; &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)static\debug\&quot;%(CMD_NEWLINE)copy /y &quot;$(TargetDir)$(TargetName).pdb&quot; &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)static\debug\&quot;
  ELIF (%(TYPE)==DLL)
    POSTCOMMAND += if not exist &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)dynamic\debug\&quot; mkdir &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)dynamic\debug\&quot;%(CMD_NEWLINE)copy /y &quot;$(TargetPath)&quot; &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)dynamic\debug\&quot;%(CMD_NEWLINE)copy /y &quot;$(TargetDir)$(TargetName).pdb&quot; &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)dynamic\debug\&quot;%(CMD_NEWLINE)copy /y &quot;$(TargetDir)$(TargetName).lib&quot; &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)dynamic\debug\&quot;
  ENDIF
ENDIF
IF "%(POSTCOMMAND)"
  PRINTNL 				CommandLine="%(POSTCOMMAND)"
ENDIF
PRINTNL 			/>
IF %(TARGDEPLOY)
  PRINTNL 			<DeploymentTool
  PRINTNL 				ForceDirty="-1"
  IF defined(COREMAKE_STATIC)
    PRINTNL 				RemoteDirectory="%CSIDL_PROGRAM_FILES%\%(PROJECT_PATH)"
  ELSE
    PRINTNL 				RemoteDirectory="%CSIDL_PROGRAM_FILES%\%(PLATFORMNAME)"
  ENDIF
  IF %(COM_COMPONENT)!="" || %(MANAGED_CLR)!=""
    PRINTNL 				RegisterOutput="-1"
  ELSE
    PRINTNL 				RegisterOutput="0"
  ENDIF
  PRINTNL 				AdditionalFiles="%(@INSTALL_CAB:%=;%)"
  PRINTNL 			/>
ENDIF
IFDEF TARGET_WINCE
  PRINTNL 			<DebuggerTool
  IF defined(COREMAKE_STATIC)
    PRINTNL 				RemoteExecutable="%CSIDL_PROGRAM_FILES%\%(PROJECT_PATH)\$(TargetFileName)"
  ELSE
    PRINTNL 				RemoteExecutable="%CSIDL_PROGRAM_FILES%\%(PLATFORMNAME)\$(TargetFileName)"
  ENDIF
  PRINTNL 			/>
ENDIF
PRINTNL 		</Configuration>
IF "%(PROJ_IS_ICL)"
  PRINTNL 		<Configuration
  PRINTNL 			Name="%(TARG_RELEASE)|%(TARGNAME)">
  PRINTNL 			<Tool
  PRINTNL 				Name="GeneralTool"
ELSE
  PRINTNL 		<Configuration
  PRINTNL 			Name="%(TARG_RELEASE)|%(TARGNAME)"
ENDIF
PRINTNL 			OutputDirectory="%(¯~!OUTPUTPATH)release%(OUTPUTDIR)"
IF DEFINED(CARBIDE_VS)
  PRINTNL 			IntermediateDirectory="Deb_WINSCW"
ELSE
  PRINTNL 			IntermediateDirectory="%(¯~!BUILDPATH)release\%(~§PATH)\%(NAME)"
ENDIF
IF DEFINED(CARBIDE_VS)
  PRINTNL 			ConfigurationType="0"
ELSE
  PRINTNL 			ConfigurationType="%(TARGCONFTYPE)"
ENDIF
//IF "%(USE_DLL_RUNTIME)"==""
//  PRINTNL 			ATLMinimizesCRunTimeLibraryUsage="true"
//ENDIF
IF "%(USE_MFC)"
  IF "%(USE_MFC)"=="dll"
    PRINTNL 			UseOfMFC="2"
  ELSE
    PRINTNL 			UseOfMFC="1"
  ENDIF
ENDIF
IF "%(USE_ATL)"
  IF %(USE_ATL)=="dll"
    PRINTNL 			UseOfATL="2"
  ELSE
    PRINTNL 			UseOfATL="1"
  ENDIF
ENDIF
PRINTNL 			CharacterSet="%(TARGUNICODETYPE)"
IF %(MANAGED_CLR)!=""
  PRINTNL 			ManagedExtensions="2"
ENDIF
IF "%(OPTIMIZE_GLOBALLY)"
  PRINTNL 			WholeProgramOptimization="1"
ENDIF
IF "%(PROJ_IS_ICL)"
  PRINTNL 				/>
ELSE
  PRINTNL 			>
ENDIF
IF DEFINED(CARBIDE_VS)
  PRINTNL 			<Tool
  PRINTNL 				Name="VCNMakeTool"
  PRINTNL 				BuildCommandLine="RCMake.exe &quot;$(ProjectPath)&quot; %(SDKNAME) GCCE UREL build"
  PRINTNL 				ReBuildCommandLine="RCMake.exe &quot;$(ProjectPath)&quot; %(SDKNAME) GCCE UREL rebuild"
  PRINTNL 				CleanCommandLine="RCMake.exe &quot;$(ProjectPath)&quot; %(SDKNAME) GCCE UREL clean"
  PRINTNL 				Output=""
  PRINTNL 				PreprocessorDefinitions="NDEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_RELEASE:%=;%)"
  PRINTNL 				IncludeSearchPath="%(@&INCLUDE:%=;%)%(@&INCLUDE_RELEASE:%=;%)%(@&SYSINCLUDE:%=;%);%(&TARGINC)%(@&CONFIG_INCLUDE:%=;%)"
  PRINTNL 				ForcedIncludes=""
  PRINTNL 				AssemblySearchPath=""
  PRINTNL 				ForcedUsingAssemblies=""
  PRINTNL 				CompileAsManaged=""
  PRINTNL 			/>
ENDIF
PRINTNL 			<Tool
PRINTNL 				Name="%(PREBUILTOOL)"
IF "%(PRECOMMAND_REL)"
  PRINTNL 				CommandLine="%(PRECOMMAND_REL)"
ENDIF
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(CUSTOM_TOOL)"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCXMLDataGeneratorTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCWebServiceProxyGeneratorTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(IDL_TOOL)"
PRINTNL 				PreprocessorDefinitions="NDEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_RELEASE:%=;%)"
PRINTNL 				AdditionalIncludeDirectories="%(@&INCLUDE:%=;%)%(@&INCLUDE_RELEASE:%=;%)%(@&SYSINCLUDE:%=;%);%(&TARGINC)%(@&CONFIG_INCLUDE:%=;%)"
PRINTNL 				MkTypLibCompatible="false"
PRINTNL 				TypeLibraryName="$(TargetDir)$(ProjectName).tlb"
PRINTNL 				OutputDirectory="%(¯~!BUILDPATH)%(~§PATH)"
PRINTNL 				HeaderFileName="$(InputName)_i.h"
PRINTNL 			/>
IF "%(PROJ_IS_ICL)"
  PRINTNL 			<Tool
  PRINTNL 				Name="CppCmplrTool"
ELSE
  PRINTNL 			<Tool
  PRINTNL 				Name="%(CPP_TOOL)"
  ENDIF
PRINTNL 				Optimization="3"
IF "%(FULL_INLINE)"
  PRINTNL 				InlineFunctionExpansion="2"
ELIF "%(LIMIT_INLINE)"
  PRINTNL 				InlineFunctionExpansion="1"
ENDIF
IF "%(FAVOR_MAX_SPEED)"
  PRINTNL 				EnableIntrinsicFunctions="true"
  PRINTNL 				FavorSizeOrSpeed="1"
  PRINTNL 				OmitFramePointers="true"
ENDIF
IF !DEFINED(IX86_64)
  IF DEFINED(COMPILER_ICL11)
    PRINTNL 				EnableEnhancedInstructionSet="1001"
  ELIF "%(FASTER_FLOAT)"
    PRINTNL 				EnableEnhancedInstructionSet="1"
  ENDIF
ENDIF
IF "%(OPTIMIZE_GLOBALLY)"
  PRINTNL 				WholeProgramOptimization="true"
ENDIF
IF "%(HEADER_QT4)"!=""
  DEFINE += QT_NO_DEBUG
  DEFINE += QT_NO_DEBUG_OUTPUT
ENDIF
PRINTNL 				AdditionalIncludeDirectories="%(@&INCLUDE:%=;%)%(@&INCLUDE_RELEASE:%=;%)%(@&SYSINCLUDE:%=;%);%(&TARGINC)%(@&CONFIG_INCLUDE:%=;%)"
PRINTNL 				PreprocessorDefinitions="NDEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_RELEASE:%=;%)"
PRINTNL 				StringPooling="true"
IF !defined(TARGET_WINCE)
  IF "%(USE_DLL_RUNTIME)"!="" || %(MANAGED_CLR)!=""
    PRINTNL 				RuntimeLibrary="2"
  ELSE
    PRINTNL 				RuntimeLibrary="0"
  ENDIF
ENDIF
PRINTNL 				BufferSecurityCheck="false"
IF "%(REDUCE_SIZE)" || defined(CONFIG_COREPROTECT)
  PRINTNL 				EnableFunctionLevelLinking="true"
ENDIF
IF "%(PRECOMPILED_HEADER)"
  PRINTNL 				UsePrecompiledHeader="2"
  PRINTNL 				PrecompiledHeaderThrough="%(PCH_FILE)"
  IF "%(PRECOMPILED_HEADER_FORCED)"!="no"
    PRINTNL 				ForcedIncludeFiles="%(PCH_FILE)"
  ENDIF
ENDIF
PRINTNL 				WarningLevel="3"
IF "%(TARGMACHINE)"!=""
  PRINTNL 				TargetMachine="%(TARGMACHINE)"
ENDIF
IF "%(TARGCOMPILEARCH)"!=""
  PRINTNL 				CompileForArchitecture="%(TARGCOMPILEARCH)"
ENDIF
IF %(TAR64)
  PRINTNL 				Detect64BitPortabilityProblems="true"
ENDIF
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCManagedResourceCompilerTool"
PRINTNL 				AdditionalIncludeDirectories="$(TargetDir)"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(RES_TOOL)"
PRINTNL 				PreprocessorDefinitions="NDEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_RELEASE:%=;%)"
PRINTNL 				AdditionalIncludeDirectories="$(TargetDir)%(@&INCLUDERC:%=;%)%(@&INCLUDE:%=;%)%(@&CONFIG_INCLUDE:%=;%)"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(PRELINKTOOL)"
PRINTNL 			/>
PRINTNL 			<Tool
IF (%(TARGCONFTYPE)==4)
  PRINTNL 				Name="%(LIB_TOOL)"
ELSE
  PRINTNL 				Name="%(LINKER_TOOL)"
ENDIF
IF (%(COM_COMPONENT)!="" || %(MANAGED_CLR)!="") && !defined(TARGET_WINCE)
  PRINTNL 				RegisterOutput="true"
ENDIF
IF ("%(LIBS)"||"%(LIBS_RELEASE)") && !(%(TYPE)==LIB)
  PRINTNL 				AdditionalDependencies="%(LIBS) %(LIBS_RELEASE)"
ENDIF
IF "%(ADDOPTIONS)"!=""
  PRINTNL 				AdditionalOptions="%(ADDOPTIONS)"
ENDIF
PRINTNL 				OutputFile="$(OutDir)\%(OUTPUT)"
IF "%(USEBUILT)"!=""
  PRINTNL 				AdditionalLibraryDirectories="%(@LIBINCLUDE:%=;%)%(@LIBINCLUDE_RELEASE:%=;%);%(~!OUTPUTPATH)release\"
ELSE
  PRINTNL 				AdditionalLibraryDirectories="%(@LIBINCLUDE:%=;%)%(@LIBINCLUDE_RELEASE:%=;%)"
ENDIF
IF "%(USE_DLL_RUNTIME)"!="" && "%(MANIFEST_RELEASE_DEP)"
  PRINTNL 				AdditionalManifestDependencies="%(MANIFEST_RELEASE_DEP)"
ENDIF
IF %(TARGDEPLOY)
  PRINTNL 				DelayLoadDLLs="$(NOINHERIT)"
ENDIF
IF %(DEF)
  PRINTNL 				ModuleDefinitionFile="%(~!DEF)"
ENDIF
IFDEF COREMAKE_DEBUG_SYMBOLS
  PRINTNL 				GenerateDebugInformation="true"
ENDIF
PRINTNL 				GenerateMapFile="true"
PRINTNL 				MapFileName="$(IntDir)\$(ProjectName).map"
IF %(TARGSUBSYSTEM)
  PRINTNL 				SubSystem="%(TARGSUBSYSTEM)"
ENDIF
IF defined(TARGET_WINCE) && (%(TYPE)==EXE)
  PRINTNL 				StackReserveSize="65536"
ENDIF
PRINTNL 				OptimizeReferences="2"
PRINTNL 				EnableCOMDATFolding="2"
IF "%(OPTIMIZE_GLOBALLY)"
  PRINTNL 				LinkTimeCodeGeneration="1"
ENDIF
IF "%(USE_MFC)" && "%(COM_COMPONENT)"=="" && defined(TARGET_WINCE)
  PRINTNL 				EntryPointSymbol="WinMainCRTStartup"
ENDIF
IF "%(TARGMACHINE)"
  PRINTNL 				TargetMachine="%(TARGMACHINE)"
ENDIF
IF %(MANAGED_CLR)!=""
  PRINTNL 				CLRUnmanagedCodeCheck="true"
ENDIF
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCALinkTool"
PRINTNL 			/>
IF (%(TYPE)!=LIB) && %(SKIP_MANIFEST)==""
  PRINTNL 			<Tool
  PRINTNL 				Name="VCManifestTool"
  IF "%(ASSEMBLYID)"
    PRINTNL 				AssemblyIdentity="%(ASSEMBLYID)"
  ENDIF
  PRINTNL 			/>
ENDIF
PRINTNL 			<Tool
PRINTNL 				Name="VCXDCMakeTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(BSC_TOOL)"
PRINTNL 			/>
IF ("%(TARGSIGN)"!="") && (%(TYPE)!=LIB)
  PRINTNL 			<Tool
  PRINTNL 				Name="VCCodeSignTool"
  PRINTNL 				AuthenticodeSignature="true"
  PRINTNL 				Certificate="%(TARGSIGN)"
  PRINTNL 			/>
ENDIF
PRINTNL 			<Tool
PRINTNL 				Name="VCFxCopTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCAppVerifierTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="VCWebDeploymentTool"
PRINTNL 			/>
PRINTNL 			<Tool
PRINTNL 				Name="%(POSTBUILTOOL)"
POSTCOMMAND =
FOR EACH SOURCE
  IF (%(>NAME)==r)
    ../POSTCOMMAND += cmd /c move /y &quot;$(TargetPath)&quot; &quot;$(TargetPath).tmp&quot;%(CMD_NEWLINE)
    ../POSTCOMMAND += &quot;%(CONFIG_QUICKTIME_SDK)\Tools\RezWack.exe&quot; -f -d &quot;$(TargetPath).tmp&quot; -r &quot;$(IntDir)\%(<NAME).qtr&quot; -o &quot;$(TargetPath)&quot;%(CMD_NEWLINE)
    ../POSTCOMMAND += del /f &quot;$(TargetPath).tmp&quot;%(CMD_NEWLINE)
  ENDIF
ENDFOR
IF "%(TRANSLATION_QT4)"!=""
  POSTCOMMAND += &quot;$(QTDIR)/bin/lupdate&quot; %(NAME).tmp.pro%(CMD_NEWLINE)
  POSTCOMMAND += &quot;$(QTDIR)/bin/lrelease&quot; %(NAME).tmp.pro%(CMD_NEWLINE)
  FOR EACH TRANSLATION_QT4
    ../POSTCOMMAND += copy /y &quot;%(~?NAME)%(<NAME).qm&quot; &quot;$(OutDir)&quot;%(CMD_NEWLINE)
  ENDFOR
ENDIF
IF ((%(TYPE)==EXE) || (%(TYPE)==DLL)) && %(EXEPROTECT)!=""
    TARGET = %(¯~!OUTPUTPATH)release\%(OUTPUT)
    POSTCOMMAND += %(&&EXEPROTECT)%(CMD_NEWLINE)
ENDIF
IF (%(TYPE)!=LIB) && %(CONFIG_INSTALL_DIR)
    POSTCOMMAND += copy /y &quot;$(TargetPath)&quot; &quot;%(CONFIG_INSTALL_DIR)&quot;%(CMD_NEWLINE)
ENDIF
IF "%(EXPORT_SVN)"
  IF (%(TYPE)==LIB)
    POSTCOMMAND += if not exist &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)static\release\&quot; mkdir &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)static\release\&quot;%(CMD_NEWLINE)copy /y &quot;$(TargetPath)&quot; &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)static\release\&quot;
  ELIF (%(TYPE)==DLL)
    POSTCOMMAND += if not exist &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)dynamic\release\&quot; mkdir &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)dynamic\release\&quot;%(CMD_NEWLINE)copy /y &quot;$(TargetPath)&quot; &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)dynamic\release\&quot;%(CMD_NEWLINE)copy /y &quot;$(TargetDir)$(TargetName).lib&quot; &quot;%(~!EXPORT_SVN)export\lib\%(~+SVNDIR)dynamic\release\&quot;
  ENDIF
ENDIF
IF %(CAB)!="" && %(CABWIZ)!="" && %(../HAS_CSHARP)==""
  POSTCOMMAND += %(&&CABWIZ) &quot;%(¯~!PATH)%(NAME)_wince.inf&quot; /dest &quot;%(¯~|OUTPUTPATH)&quot; %(CABWIZCLI)%(CMD_NEWLINE)
  POSTCOMMAND += move &quot;%(¯~!OUTPUTPATH)%(NAME)_wince.CAB&quot; &quot;%(¯~!OUTPUTPATH)%(PROJECT_NAME).%(CABNAME).cab&quot;%(CMD_NEWLINE)
ENDIF
IF "%(POSTCOMMAND)"
  PRINTNL 				CommandLine="%(POSTCOMMAND)"
ENDIF
PRINTNL 			/>
IF %(TARGDEPLOY)
  PRINTNL 			<DeploymentTool
  PRINTNL 				ForceDirty="-1"
  IF defined(COREMAKE_STATIC)
    PRINTNL 				RemoteDirectory="%CSIDL_PROGRAM_FILES%\%(PROJECT_PATH)"
  ELSE
    PRINTNL 				RemoteDirectory="%CSIDL_PROGRAM_FILES%\%(PLATFORMNAME)"
  ENDIF
  IF %(COM_COMPONENT)!="" || %(MANAGED_CLR)!=""
    PRINTNL 				RegisterOutput="-1"
  ELSE
    PRINTNL 				RegisterOutput="0"
  ENDIF
  PRINTNL 				AdditionalFiles="%(@INSTALL_CAB:%=;%)"
  PRINTNL 			/>
ENDIF
IFDEF TARGET_WINCE
  PRINTNL 			<DebuggerTool
  IF defined(COREMAKE_STATIC)
    PRINTNL 				RemoteExecutable="%CSIDL_PROGRAM_FILES%\%(PROJECT_PATH)\$(TargetFileName)"
  ELSE
    PRINTNL 				RemoteExecutable="%CSIDL_PROGRAM_FILES%\%(PLATFORMNAME)\$(TargetFileName)"
  ENDIF
  PRINTNL 			/>
ENDIF
PRINTNL 		</Configuration>
PRINTNL 	</Configurations>
PRINTNL 	<References>
FOR EACH USE
  IF (!%(NOLIB))
    PRINTNL 		<ProjectReference
    PRINTNL 			ReferencedProjectIdentifier="%(GUID)"
//    PRINTNL 			RelativePathToProject="%(¯~!PATH)%(NAME).%(PROJECT_EXT)"
    PRINTNL 		/>
  ENDIF
ENDFOR
FOR EACH DEP
  PRINTNL 		<ProjectReference
  PRINTNL 			ReferencedProjectIdentifier="%(GUID)"
  PRINTNL 			CopyLocal="false"
  PRINTNL 			CopyLocalDependencies="false"
  PRINTNL 			UseInBuild="false"
  PRINTNL 			UseDependenciesInBuild="false"
  PRINTNL 			CopyLocalSatelliteAssemblies="false"
//  PRINTNL 			RelativePathToProject="%(¯~!PATH)%(NAME).%(PROJECT_EXT)"
  PRINTNL 		/>
ENDFOR
IF %(MANAGED_CLR)!=""
  PRINTNL 		<AssemblyReference
  PRINTNL 			RelativePath="System.dll"
  PRINTNL 			AssemblyName="System, Version=2.0.0.0, PublicKeyToken=b77a5c561934e089, processorArchitecture=MSIL"
  PRINTNL 		/>
  PRINTNL 		<AssemblyReference
  PRINTNL 			RelativePath="System.Data.dll"
  PRINTNL 			AssemblyName="System.Data, Version=2.0.0.0, PublicKeyToken=b77a5c561934e089, processorArchitecture=x86"
  PRINTNL 		/>
  PRINTNL 		<AssemblyReference
  PRINTNL 			RelativePath="System.Drawing.dll"
  PRINTNL 			AssemblyName="System.Drawing, Version=2.0.0.0, PublicKeyToken=b03f5f7f11d50a3a, processorArchitecture=MSIL"
  PRINTNL 		/>
  PRINTNL 		<AssemblyReference
  PRINTNL 			RelativePath="System.Windows.Forms.dll"
  PRINTNL 			AssemblyName="System.Windows.Forms, Version=2.0.0.0, PublicKeyToken=b77a5c561934e089, processorArchitecture=MSIL"
  PRINTNL 		/>
  PRINTNL 		<AssemblyReference
  PRINTNL 			RelativePath="System.XML.dll"
  PRINTNL 			AssemblyName="System.Xml, Version=2.0.0.0, PublicKeyToken=b77a5c561934e089, processorArchitecture=MSIL"
  PRINTNL 		/>
ENDIF
PRINTNL 	</References>
PRINTNL 	<Files>
PRINTNL 		<Filter
PRINTNL 			Name="Source Files"
PRINTNL 			Filter="cpp;c;cc;cxx;rc;def;r;odl;idl;hpj;bat;asm;asmx;nas"
PRINTNL 			UniqueIdentifier="{4FC737F1-C7A5-4376-A066-2A32D752A2FF}"
PRINTNL 			>
FOR EACH SOURCE
PRINTNL 			<File
PRINTNL 				RelativePath="%(~!NAME)"
IF (%(_copy_item))
  PRINTNL 				DeploymentContent="true"
ENDIF
PRINTNL 				>
IF DEFINED(CARBIDE_VS)
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 				</FileConfiguration>
ELIF ((%(>NAME)==c || %(>NAME)==cpp) && %(../PCH_SOURCE) && %(NO_PCH)=="")
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CPP_TOOL)"
  PRINTNL 						UsePrecompiledHeader="1"
  IF %(w)
    PRINTNL 						WarningLevel="0"
  ENDIF
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CPP_TOOL)"
  PRINTNL 						UsePrecompiledHeader="1"
  IF %(w)
    PRINTNL 						WarningLevel="0"
  ENDIF
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  ../PCH_SOURCE =
ELIF ((%(>NAME)==c || %(>NAME)==cpp) && (%(w) || (%(NO_PCH)!="" && %(PRECOMPILED_HEADER))))
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CPP_TOOL)"
  IF %(w)
    PRINTNL 						WarningLevel="0"
  ENDIF
  IF %(NO_PCH)!="" && %(PRECOMPILED_HEADER)
    PRINTNL 						UsePrecompiledHeader="0"
  ENDIF
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CPP_TOOL)"
  IF %(w)
    PRINTNL 						WarningLevel="0"
  ENDIF
  IF %(NO_PCH)!="" && %(PRECOMPILED_HEADER)
    PRINTNL 						UsePrecompiledHeader="0"
  ENDIF
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
ELIF (%(>NAME)==asm)
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="Assembling %(~!NAME)..."
  IFDEF IX86
    PRINTNL 						CommandLine="yasm -f win32 -D_WIN32 %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) %(@&INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF IX86_64
    PRINTNL 						CommandLine="yasm -f win64 -D_WIN64 %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) %(@&INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF ARM
    IF %(CPU)==armv6
      PRINTNL 						CommandLine="armasm -cpu arm1136 &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
    ELSE
      PRINTNL 						CommandLine="armasm -cpu xscale &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
    ENDIF
  ENDIF
  PRINTNL 						Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="Assembling %(~!NAME)..."
  IFDEF IX86
    PRINTNL 						CommandLine="yasm -f win32 -D_WIN32 %(DEFINE:%=-D%) %(DEFINE_DEBUG:%=-D%) %(@&INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF IX86_64
    PRINTNL 						CommandLine="yasm -f win64 -D_WIN64 %(DEFINE:%=-D%) %(DEFINE_DEBUG:%=-D%) %(@&INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF ARM
    IF %(CPU)==armv6
      PRINTNL 						CommandLine="armasm -cpu arm1136 &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
    ELSE
      PRINTNL 						CommandLine="armasm -cpu xscale &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
    ENDIF
  ENDIF
  PRINTNL 						Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
ELIF (%(>NAME)==nas)
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="Assembling %(~!NAME)..."
  IFDEF IX86
    PRINTNL 						CommandLine="nasmw -f win32 -D_WIN32 -DWIN32 %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) %(@INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF IX86_64
    PRINTNL 						CommandLine="nasmw -f win64 -D_WIN64 -DWIN64 %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) %(@INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF ARM
    PRINTNL 						CommandLine="armasm -cpu xscale &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
  ENDIF
  PRINTNL 						Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="Assembling %(~!NAME)..."
  IFDEF IX86
    PRINTNL 						CommandLine="nasmw -f win32 -D_WIN32 -DWIN32 %(DEFINE:%=-D%) %(DEFINE_DEBUG:%=-D%) %(@INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF IX86_64
    PRINTNL 						CommandLine="nasmw -f win64 -D_WIN64 -DWIN64 %(DEFINE:%=-D%) %(DEFINE_DEBUG:%=-D%) %(@INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF ARM
    PRINTNL 						CommandLine="armasm -cpu xscale &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
  ENDIF
  PRINTNL 						Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
ELIF (%(>NAME)==c) && defined(MIPS) && ("%(TARGGCC)"!="") && %(gcc)
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						CommandLine="%(TARGGCC) -DNDEBUG %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) %(@INCLUDE:%=-I%) %(@CONFIG_INCLUDE:%=-I%) -S &quot;$(InputPath)&quot; -o&quot;$(IntDir)/$(InputName).S&quot;%(CMD_NEWLINE)mips2s &quot;$(IntDir)/$(InputName).S&quot;%(CMD_NEWLINE)mipsasm /QMmips32 /c &quot;$(IntDir)/$(InputName)_gcc.S&quot; /Fo&quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
  PRINTNL 						Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 				</FileConfiguration>
ELIF (%(>NAME)==s) && defined(MIPS)
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="Assembling %(~!NAME)..."
  PRINTNL 						CommandLine="mipsasm /QMmips32 /c &quot;$(InputPath)&quot; /Fo&quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
  PRINTNL 						Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="Assembling %(~!NAME)..."
  PRINTNL 						CommandLine="mipsasm /QMmips32 /c &quot;$(InputPath)&quot; /Fo&quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
  PRINTNL 						Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
ELIF (%(_copy_item))
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="Copying $(InputPath) to $(TargetDir)..."
  PRINTNL 						CommandLine="copy /y &quot;$(InputPath)&quot; &quot;$(TargetDir)&quot; &gt;nul"
  PRINTNL 						Outputs="$(TargetDir)/$(InputFileName)"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="Copying $(InputPath) to $(TargetDir)..."
  PRINTNL 						CommandLine="copy /y &quot;$(InputPath)&quot; &quot;$(TargetDir)&quot; &gt;nul"
  PRINTNL 						Outputs="$(TargetDir)/$(InputFileName)"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
ELIF (%(>NAME)==r)
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="Resource forking %(~!NAME)..."
  PRINTNL 						CommandLine="&quot;%(CONFIG_QUICKTIME_SDK)\Tools\Rez.exe&quot; -p %(~@INCLUDE:%=-i &quot;%&quot;) -i &quot;%(CONFIG_QUICKTIME_SDK)\RIncludes&quot; -d TARGET_REZ_CARBON_MACHO=0 -o &quot;$(IntDir)\$(InputName).qtr&quot; &lt; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  PRINTNL 						Outputs="$(IntDir)/$(InputName).qtr"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="Resource forking %(~!NAME)..."
  PRINTNL 						CommandLine="&quot;%(CONFIG_QUICKTIME_SDK)\Tools\Rez.exe&quot; -p %(~@INCLUDE:%=-i &quot;%&quot;) -i &quot;%(CONFIG_QUICKTIME_SDK)\RIncludes&quot; -d TARGET_REZ_CARBON_MACHO=0 -o &quot;$(IntDir)\$(InputName).qtr&quot; &lt; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  PRINTNL 						Outputs="$(IntDir)/$(InputName).qtr"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
ENDIF
PRINTNL 			</File>
ENDFOR
FOR EACH DEF
  PRINTNL 			<File
  PRINTNL 				RelativePath="%(~!NAME)"
  PRINTNL 				>
  PRINTNL 			</File>
ENDFOR
PRINTNL 		</Filter>
IF DEFINED(CARBIDE_VS)
  PRINTNL 		<Filter
  PRINTNL 			Name="Symbian"
  PRINTNL 			>
  IF (%(TYPE)==EXE)
    PRINTNL 			<File
    PRINTNL 				RelativePath="../bld.inf"
    PRINTNL 				>
    PRINTNL 				<FileConfiguration
    PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL 					>
    PRINTNL 				</FileConfiguration>
    PRINTNL 			</File>
  ENDIF
  PRINTNL 			<File
  PRINTNL 				RelativePath="%(NAME).uid.cpp"
  PRINTNL 				>
  PRINTNL 			</File>
  PRINTNL 			<File
  PRINTNL 				RelativePath="%(NAME).mmp"
  PRINTNL 				>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL 					ExcludedFromBuild="true"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="Carbide.vs.build.rule"
  PRINTNL 						AdditionalDependencies=""
  PRINTNL 						type="2"
  PRINTNL 						output=""
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 				</FileConfiguration>
  PRINTNL 			</File>
  PRINTNL 		</Filter>
ENDIF
IF %(HEADER_QT4) || %(HEADER)
  PRINTNL 		<Filter
  PRINTNL 			Name="Header Files"
  PRINTNL 			Filter="h;hpp;hxx;hm;inl;inc;xsd;xpm"
  PRINTNL 			UniqueIdentifier="{93995380-89BD-4b04-88EB-625FBE52EBFB}"
  PRINTNL 			>
  FOR EACH HEADER_QT4
    PRINTNL 			<File
    PRINTNL 				RelativePath="%(~!NAME)"
    PRINTNL 				>
    PRINTNL 				<FileConfiguration
    PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
    PRINTNL 					>
    PRINTNL 					<Tool
    PRINTNL 						Name="%(CUSTOM_TOOL)"
    PRINTNL 						Description="Moc&apos;ing %(~!NAME)..."
//    IF "%(PCH_FILE)"
//      PRINTNL 						CommandLine="$(QTDIR)\bin\moc.exe -f%(PCH_FILE) %(DEFINE:%=-D%) -I&quot;%(~!BUILDPATH)debug\%(PATH)GeneratedFiles\%(@INCLUDE:%=;%)&quot; &quot;%(~!NAME)&quot; -o &quot;%(~!BUILDPATH)debug\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
//    ELSE
      PRINTNL 						CommandLine="$(QTDIR)\bin\moc.exe %(DEFINE:%=-D%) %(DEFINE_DEBUG:%=-D%) -I&quot;%(~!BUILDPATH)debug\%(PATH)GeneratedFiles\%(@INCLUDE:%=;%)&quot; &quot;%(~!NAME)&quot; -o &quot;%(~!BUILDPATH)debug\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
//    ENDIF
    PRINTNL 						AdditionalDependencies="$(QTDIR)\bin\moc.exe;%(~!NAME)"
    PRINTNL 						Outputs="%(~!BUILDPATH)debug\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp"
    PRINTNL 					/>
    PRINTNL 				</FileConfiguration>
    PRINTNL 				<FileConfiguration
    PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL 					>
    PRINTNL 					<Tool
    PRINTNL 						Name="%(CUSTOM_TOOL)"
    PRINTNL 						Description="Moc&apos;ing %(~!NAME)..."
//    IF "%(PCH_FILE)"
//      PRINTNL 						CommandLine="$(QTDIR)\bin\moc.exe -f%(PCH_FILE) %(DEFINE:%=-D%) -I&quot;%(~!BUILDPATH)release\%(PATH)GeneratedFiles\%(@INCLUDE:%=;%)&quot; &quot;%(~!NAME)&quot; -o &quot;%(~!BUILDPATH)release\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
//    ELSE
      PRINTNL 						CommandLine="$(QTDIR)\bin\moc.exe %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) -I&quot;%(~!BUILDPATH)release\%(PATH)GeneratedFiles\%(@INCLUDE:%=;%)&quot; &quot;%(~!NAME)&quot; -o &quot;%(~!BUILDPATH)release\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
//    ENDIF
    PRINTNL 						AdditionalDependencies="$(QTDIR)\bin\moc.exe;%(~!NAME)"
    PRINTNL 						Outputs="%(~!BUILDPATH)release\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp"
    PRINTNL 					/>
    PRINTNL 				</FileConfiguration>
    PRINTNL 			</File>
  ENDFOR
  FOR EACH HEADER
    PRINTNL 			<File
    PRINTNL 				RelativePath="%(~!NAME)"
    IF %(managed)!=""
      PRINTNL 				FileType="4"
    ENDIF
    PRINTNL 				>
    IF DEFINED(CARBIDE_VS)
      PRINTNL 				<FileConfiguration
      PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
      PRINTNL 					>
      PRINTNL 				</FileConfiguration>
    ELIF (%(>NAME)==cpp) || (%(>NAME)==c)
      PRINTNL 				<FileConfiguration
      PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
      PRINTNL 					>
      PRINTNL 					<Tool
      PRINTNL 						Name="VCCustomBuildTool"
      PRINTNL 					/>
      PRINTNL 				</FileConfiguration>
      PRINTNL 				<FileConfiguration
      PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
      PRINTNL 					>
      PRINTNL 					<Tool
      PRINTNL 						Name="VCCustomBuildTool"
      PRINTNL 					/>
      PRINTNL 				</FileConfiguration>
    ENDIF
    IF %(resx)!=""
      PRINTNL 				<File
      PRINTNL 					RelativePath="%(<NAME).resX"
      PRINTNL 					SubType="Designer"
      PRINTNL 					>
      PRINTNL 				</File>
    ENDIF
    PRINTNL 			</File>
  ENDFOR
  FOR EACH HEADER_QT4
    PRINTNL 			<File
    PRINTNL 				RelativePath="%(!BUILDPATH)%(PATH)ui_%(~<!NAME).h"
    PRINTNL 				>
    PRINTNL 			</File>
  ENDFOR
  PRINTNL 		</Filter>
ENDIF
IF "%(XRC_SOURCE)"!=""
  PRINTNL 		<Filter
  PRINTNL 			Name="XRC Resources"
  PRINTNL 			Filter="xrc"
  PRINTNL 			UniqueIdentifier="{99349809-55BA-4b9d-BF79-8FAC20286EB3}"
  PRINTNL 			>
  FOR EACH XRC_SOURCE
    PRINTNL 			<File
    PRINTNL 				RelativePath="%(~NAME)"
    PRINTNL 				>
    PRINTNL 				<FileConfiguration
    PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
    PRINTNL 					>
    PRINTNL 					<Tool
    PRINTNL 						Name="%(CUSTOM_TOOL)"
    PRINTNL 						Description="wxRC&apos;ing %(~!NAME)..."
    PRINTNL 						CommandLine="&quot;$(WXWIN)\bin\wxrc.exe&quot; /c /o &quot;%(~!BUILDPATH)%(~PATH)xrc_%(~<!NAME).cpp&quot; &quot;%(NAME)&quot;%(CMD_NEWLINE)"
    PRINTNL 						AdditionalDependencies="$(WXWIN)\bin\wxrc.exe"
    PRINTNL 						Outputs="%(~!BUILDPATH)%(~PATH)xrc_%(~<!NAME).cpp"
    PRINTNL 					/>
    PRINTNL 				</FileConfiguration>
    PRINTNL 				<FileConfiguration
    PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL 					>
    PRINTNL 					<Tool
    PRINTNL 						Name="%(CUSTOM_TOOL)"
    PRINTNL 						Description="wxRC&apos;ing %(~!NAME)..."
    PRINTNL 						CommandLine="&quot;$(WXWIN)\bin\wxrc.exe&quot; /c /o &quot;%(~!BUILDPATH)%(~PATH)xrc_%(~<!NAME).cpp&quot; &quot;%(NAME)&quot;%(CMD_NEWLINE)"
    PRINTNL 						AdditionalDependencies="$(WXWIN)\bin\wxrc.exe"
    PRINTNL 						Outputs="%(~!BUILDPATH)%(~PATH)xrc_%(~<!NAME).cpp"
    PRINTNL 					/>
    PRINTNL 				</FileConfiguration>
    PRINTNL 			</File>
  ENDFOR
  PRINTNL 		</Filter>
ENDIF
IF "%(UI_FORM_QT4)"!=""
  PRINTNL 		<Filter
  PRINTNL 			Name="Form Files"
  PRINTNL 			Filter="ui"
  PRINTNL 			UniqueIdentifier="{99349809-55BA-4b9d-BF79-8FDBB0286EB3}"
  PRINTNL 			>
  FOR EACH UI_FORM_QT4
    PRINTNL 			<File
    PRINTNL 				RelativePath="%(~!NAME)"
    PRINTNL 				>
    PRINTNL 				<FileConfiguration
    PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
    PRINTNL 					>
    PRINTNL 					<Tool
    PRINTNL 						Name="%(CUSTOM_TOOL)"
    PRINTNL 						Description="Uic&apos;ing %(~!NAME)..."
    PRINTNL 						CommandLine="&quot;$(QTDIR)\bin\uic.exe&quot; -o &quot;%(!BUILDPATH)%(PATH)ui_%(~<!NAME).h&quot; &quot;%(~!NAME)&quot;%(CMD_NEWLINE)"
    PRINTNL 						AdditionalDependencies="$(QTDIR)\bin\uic.exe"
    PRINTNL 						Outputs="%(!BUILDPATH)%(PATH)ui_%(~<!NAME).h"
    PRINTNL 					/>
    PRINTNL 				</FileConfiguration>
    PRINTNL 				<FileConfiguration
    PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL 					>
    PRINTNL 					<Tool
    PRINTNL 						Name="%(CUSTOM_TOOL)"
    PRINTNL 						Description="Uic&apos;ing %(~!NAME)..."
    PRINTNL 						CommandLine="&quot;$(QTDIR)\bin\uic.exe&quot; -o &quot;%(!BUILDPATH)%(PATH)ui_%(~<!NAME).h&quot; &quot;%(~!NAME)&quot;%(CMD_NEWLINE)"
    PRINTNL 						AdditionalDependencies="$(QTDIR)\bin\uic.exe"
    PRINTNL 						Outputs="%(!BUILDPATH)%(PATH)ui_%(~<!NAME).h"
    PRINTNL 					/>
    PRINTNL 				</FileConfiguration>
    PRINTNL 			</File>
  ENDFOR
  PRINTNL 		</Filter>
ENDIF
IF "%(HEADER_QT4)"!=""||"%(RESOURCE_QT4)"!=""||"%(XRC_SOURCE)"!=""||"%(HAS_IDL)"!=""
  PRINTNL 		<Filter
  PRINTNL 			Name="Generated Files"
  PRINTNL 			Filter="moc;h;cpp;c"
  PRINTNL 			UniqueIdentifier="{71ED8ED8-ACB9-4CE9-BBE1-E00B30144E11}"
  PRINTNL 			SourceControlFiles="false"
  PRINTNL 			>
  FOR EACH SOURCE
    IF (%(>NAME)==idl) && %(hmarshal)!=""
      PRINTNL 			<File
      PRINTNL 				RelativePath="%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.h"
      PRINTNL 				>
      PRINTNL 			</File>
    ENDIF
    IF (%(>NAME)==idl) && %(cmarshal)!=""
      PRINTNL 			<File
      PRINTNL 				RelativePath="%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.c"
      PRINTNL 				>
      IF "%(PRECOMPILED_HEADER)"
        PRINTNL					<FileConfiguration
        PRINTNL						Name="%(TARG_DEBUG)|%(TARGNAME)"
        PRINTNL						>
        PRINTNL						<Tool
        PRINTNL							Name="%(CPP_TOOL)"
        PRINTNL							UsePrecompiledHeader="0"
        PRINTNL						/>
        PRINTNL					</FileConfiguration>
        PRINTNL					<FileConfiguration
        PRINTNL						Name="%(TARG_RELEASE)|%(TARGNAME)"
        PRINTNL						>
        PRINTNL						<Tool
        PRINTNL							Name="%(CPP_TOOL)"
        PRINTNL							UsePrecompiledHeader="0"
        PRINTNL						/>
        PRINTNL					</FileConfiguration>
      ENDIF
      PRINTNL 			</File>
    ENDIF
  ENDFOR
  FOR EACH XRC_SOURCE
    PRINTNL 			<File
    PRINTNL 				RelativePath="%(¯~!BUILDPATH)%(~PATH)xrc_%(~<!NAME).cpp"
    PRINTNL 				>
    PRINTNL 			</File>
  ENDFOR
  FOR EACH HEADER_QT4
   IF (%(>NAME)!=cpp)
    PRINTNL 			<File
    PRINTNL 				RelativePath="%(¯~!BUILDPATH)debug\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp"
    PRINTNL 				>
    PRINTNL				<FileConfiguration
    PRINTNL					Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL					ExcludedFromBuild="true"
    PRINTNL					>
    PRINTNL					<Tool
    PRINTNL						Name="%(CPP_TOOL)"
  //  PRINTNL						UsePrecompiledHeader="0"
    PRINTNL						/>
    PRINTNL				</FileConfiguration>
    PRINTNL			</File>
    PRINTNL			<File
    PRINTNL				RelativePath="%(¯~!BUILDPATH)release\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp"
    PRINTNL				>
    PRINTNL				<FileConfiguration
    PRINTNL					Name="%(TARG_DEBUG)|%(TARGNAME)"
    PRINTNL					ExcludedFromBuild="true"
    PRINTNL					>
    PRINTNL					<Tool
    PRINTNL						Name="%(CPP_TOOL)"
  //  PRINTNL						UsePrecompiledHeader="0"
    PRINTNL					/>
    PRINTNL				</FileConfiguration>
    PRINTNL 			</File>
   ENDIF
  ENDFOR
  FOR EACH RESOURCE_QT4
    PRINTNL 			<File
    PRINTNL 				RelativePath="%(¯~!BUILDPATH)%(PATH)GeneratedFiles\qrc_%(~<!NAME).cpp"
    PRINTNL 				>
    IF "%(PRECOMPILED_HEADER)"!="" && "%(PRECOMPILED_HEADER_FORCED)"=="no"
      PRINTNL 				<FileConfiguration
      PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
      PRINTNL 					>
      PRINTNL 					<Tool
      PRINTNL 						Name="%(CPP_TOOL)"
      PRINTNL 						UsePrecompiledHeader="0"
      PRINTNL 					/>
      PRINTNL 				</FileConfiguration>
      PRINTNL 				<FileConfiguration
      PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
      PRINTNL 					>
      PRINTNL 					<Tool
      PRINTNL 						Name="%(CPP_TOOL)"
      PRINTNL 						UsePrecompiledHeader="0"
      PRINTNL 					/>
      PRINTNL 				</FileConfiguration>
    ENDIF
    PRINTNL 			</File>
  ENDFOR
  PRINTNL 		</Filter>
ENDIF
IF "%(RESOURCE_QT4)"!=""
  PRINTNL 		<Filter
  PRINTNL 			Name="Resource Files"
  PRINTNL 			Filter="ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;qrc;txt"
  PRINTNL 			UniqueIdentifier="{D9D6E242-F8AF-46E4-B9FD-80ECBC20BA3E}"
  PRINTNL 			>
  FOR EACH RESOURCE_QT4
    PRINTNL 			<File
    PRINTNL 				RelativePath="%(~!NAME)"
    PRINTNL 				>
      PRINTNL 				<FileConfiguration
      PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
      PRINTNL 					>
      PRINTNL 					<Tool
      PRINTNL 						Name="%(CUSTOM_TOOL)"
      PRINTNL 						Description="Rcc&apos;ing %(~!NAME)..."
      PRINTNL 						CommandLine="&quot;$(QTDIR)\bin\rcc.exe&quot; -compress 1 -name %(~<!NAME) %(~!NAME) -o &quot;%(¯!BUILDPATH)%(PATH)GeneratedFiles\qrc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
      PRINTNL 						AdditionalDependencies="$(QTDIR)\bin\rcc.exe"
      PRINTNL 						Outputs="%(¯!BUILDPATH)%(PATH)GeneratedFiles\qrc_%(~<!NAME).cpp"
      PRINTNL 					/>
      PRINTNL 				</FileConfiguration>
      PRINTNL 				<FileConfiguration
      PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
      PRINTNL 					>
      PRINTNL 					<Tool
      PRINTNL 						Name="%(CUSTOM_TOOL)"
      PRINTNL 						Description="Rcc&apos;ing %(~!NAME)..."
      PRINTNL 						CommandLine="&quot;$(QTDIR)\bin\rcc.exe&quot; -name %(~<!NAME) %(~!NAME) -o &quot;%(¯!BUILDPATH)%(PATH)GeneratedFiles\qrc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
      PRINTNL 						AdditionalDependencies="$(QTDIR)\bin\rcc.exe"
      PRINTNL 						Outputs="%(¯!BUILDPATH)%(PATH)GeneratedFiles\qrc_%(~<!NAME).cpp"
      PRINTNL 					/>
      PRINTNL 				</FileConfiguration>
    PRINTNL 			</File>
  ENDFOR
  PRINTNL 		</Filter>
ENDIF
IF "%(I18N)"
  PRINTNL 		<Filter
  PRINTNL 			Name="i18n Language Files"
  PRINTNL 			Filter="pot,po"
  PRINTNL 			UniqueIdentifier="{99349809-66BA-4b9d-BF79-8FDBB0286EB3}"
  PRINTNL 			ParseFiles="false"
  PRINTNL 			>
  PRINTNL 			<File
  PRINTNL 				RelativePath="%(~I18N)\%(../NAME).pot"
  PRINTNL 				>
  I18NCOMMAND =if not exist &quot;%(I18N)&quot; mkdir &quot;%(I18N)&quot;%(CMD_NEWLINE)
  I18NCOMMAND +=xgettext -p %(I18N) -o %(NAME).pot --no-location --from-code=UTF-8
  IF %(I18N_KEYWORD)
    I18NCOMMAND += --keyword=&quot;%(I18N_KEYWORD)&quot;
  ELSE
    I18NCOMMAND += -a
  ENDIF
  FOR EACH SOURCE
    IF (%(>NAME)!=rc)
      ../I18NSOURCES += %(~!NAME)
    ENDIF
  ENDFOR
  I18NCOMMAND +=%(I18NSOURCES)%(CMD_NEWLINE)
  OUTDIR_LOCALE = %(~I18N)\%(../NAME)
  FOR EACH I18N_LANG
    ../I18NCOMMAND +=if not exist &quot;%(OUTDIR_LOCALE)&quot; mkdir &quot;%(OUTDIR_LOCALE)&quot;%(CMD_NEWLINE)
    ../I18NCOMMAND +=if not exist &quot;%(OUTDIR_LOCALE)\%(NAME).po&quot; copy &quot;%(~I18N)\%(../NAME).pot&quot; &quot;%(OUTDIR_LOCALE)\%(NAME).po&quot;%(CMD_NEWLINE)
    ../I18NCOMMAND +=msgmerge -U --backup=none %(OUTDIR_LOCALE)\%(NAME).po %(~I18N)\%(../NAME).pot%(CMD_NEWLINE)
  ENDFOR
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
  IF "%(I18N_DEBUG)"==""
    PRINTNL 					ExcludedFromBuild="true"
  ENDIF
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="i18n&apos;ing language %(NAME)..."
  PRINTNL 						CommandLine="%(I18NCOMMAND)"
  PRINTNL 						AdditionalDependencies="%(I18NSOURCES:%=%;)"
  PRINTNL 						Outputs="%(~I18N)\%(../NAME).pot"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 				<FileConfiguration
  PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL 					>
  PRINTNL 					<Tool
  PRINTNL 						Name="%(CUSTOM_TOOL)"
  PRINTNL 						Description="i18n&apos;ing language %(NAME)..."
  PRINTNL 						CommandLine="%(I18NCOMMAND)"
  PRINTNL 						AdditionalDependencies="%(I18NSOURCES:%=%;)"
  PRINTNL 						Outputs="%(~I18N)\%(../NAME).pot"
  PRINTNL 					/>
  PRINTNL 				</FileConfiguration>
  PRINTNL 			</File>
  FOR EACH I18N_LANG
    PRINTNL 			<File
    PRINTNL 				RelativePath="%(OUTDIR_LOCALE)\%(NAME).po"
    PRINTNL 				>
    PRINTNL 				<FileConfiguration
    PRINTNL 					Name="%(TARG_DEBUG)|%(TARGNAME)"
    IF "%(I18N_DEBUG)"==""
      PRINTNL 					ExcludedFromBuild="true"
    ENDIF
    PRINTNL 					>
    PRINTNL 					<Tool
    PRINTNL 						Name="%(CUSTOM_TOOL)"
    PRINTNL 						Description="i18n&apos;ing %(NAME)..."
    PRINTNL 						CommandLine="if not exist &quot;$(TargetDir)%(NAME)\LC_MESSAGES&quot; mkdir &quot;$(TargetDir)%(NAME)\LC_MESSAGES&quot;%(CMD_NEWLINE)msgfmt -o $(TargetDir)%(NAME)\LC_MESSAGES\%(../NAME).mo %(OUTDIR_LOCALE)\%(NAME).po%(CMD_NEWLINE)"
    PRINTNL 						AdditionalDependencies="%(~I18N)\%(../NAME).pot"
    PRINTNL 						Outputs="$(TargetDir)%(NAME)\LC_MESSAGES\%(../NAME).mo"
    PRINTNL 					/>
    PRINTNL 				</FileConfiguration>
    PRINTNL 				<FileConfiguration
    PRINTNL 					Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL 					>
    PRINTNL 					<Tool
    PRINTNL 						Name="%(CUSTOM_TOOL)"
    PRINTNL 						Description="i18n&apos;ing %(NAME)..."
    PRINTNL 						CommandLine="if not exist &quot;$(TargetDir)%(NAME)\LC_MESSAGES&quot; mkdir &quot;$(TargetDir)%(NAME)\LC_MESSAGES&quot;%(CMD_NEWLINE)msgfmt -o $(TargetDir)%(NAME)\LC_MESSAGES\%(../NAME).mo %(OUTDIR_LOCALE)\%(NAME).po%(CMD_NEWLINE)"
    PRINTNL 						AdditionalDependencies="%(~I18N)\%(../NAME).pot"
    PRINTNL 						Outputs="$(TargetDir)%(NAME)\LC_MESSAGES\%(../NAME).mo"
    PRINTNL 					/>
    PRINTNL 				</FileConfiguration>
    PRINTNL 			</File>
  ENDFOR
  PRINTNL 		</Filter>
ENDIF
PRINTNL 	</Files>
PRINTNL 	<Globals>
//		<Global
//			Name="RESOURCE_FILE"
//			Value="ActiveXwin32.rc"
//		/>
PRINTNL 	</Globals>
PRINTNL </VisualStudioProject>
