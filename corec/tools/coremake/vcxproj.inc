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

DEFINE += STRICT
IF "%(VSVERSION)"=="8.00" || "%(VSVERSION)"=="9.00" || "%(VSVERSION)"=="14.00"
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

PRINTNL <?xml version="1.0" encoding="utf-8"?>
PRINTNL <Project DefaultTargets="Build" ToolsVersion="%(TOOLSVERSION)" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
PRINTNL   <ItemGroup Label="ProjectConfigurations">
IF "%(PROJ_IS_ICL)"
  PRINTNL     <ProjectConfiguration Include="%(TARG_DEBUG)|%(TARGNAME)">
  PRINTNL       <Tool
  PRINTNL         Name="GeneralTool"
PRINTNL       OutputDirectory="%(¯~!OUTPUTPATH)debug%(OUTPUTDIR)"
PRINTNL       IntermediateDirectory="%(¯~!BUILDPATH)debug\%(~§PATH)\%(NAME)"
PRINTNL       ConfigurationType="%(TARGCONFTYPE)"
ELSE
  PRINTNL     <ProjectConfiguration Include="%(TARG_DEBUG)|%(TARGNAME)">
  PRINTNL       <Configuration>%(TARG_DEBUG)</Configuration>
  PRINTNL       <Platform>%(TARGNAME)</Platform>
  PRINTNL     </ProjectConfiguration>
ENDIF
IF "%(PROJ_IS_ICL)"
  PRINTNL     <ProjectConfiguration Include="%(TARG_RELEASE)|%(TARGNAME)">
  PRINTNL       <Tool
  PRINTNL         Name="GeneralTool"
PRINTNL       OutputDirectory="%(¯~!OUTPUTPATH)release%(OUTPUTDIR)"
PRINTNL       IntermediateDirectory="%(¯~!BUILDPATH)release\%(~§PATH)\%(NAME)"
PRINTNL       ConfigurationType="%(TARGCONFTYPE)"
ELSE
  PRINTNL     <ProjectConfiguration Include="%(TARG_RELEASE)|%(TARGNAME)">
  PRINTNL       <Configuration>%(TARG_RELEASE)</Configuration>
  PRINTNL       <Platform>%(TARGNAME)</Platform>
  PRINTNL     </ProjectConfiguration>
ENDIF
PRINTNL   </ItemGroup>
PRINTNL   <PropertyGroup Label="Globals">
PRINTNL     <ProjectGuid>%(GUID)</ProjectGuid>
PRINTNL     <RootNamespace>%(NAME)</RootNamespace>
PRINTNL   </PropertyGroup>
PRINTNL   <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
PRINTNL   <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='%(TARG_RELEASE)|%(TARGNAME)'" Label="Configuration">
IF %(TYPE)==DLL
  PRINTNL     <ConfigurationType>DynamicLibrary</ConfigurationType>
ELIF %(TYPE)==LIB
  PRINTNL     <ConfigurationType>StaticLibrary</ConfigurationType>
ELSE
  PRINTNL     <ConfigurationType>Application</ConfigurationType>
ENDIF
PRINTNL     <PlatformToolset>%(PLATFORMTOOLSET)</PlatformToolset>
PRINTNL     <CharacterSet>Unicode</CharacterSet>
PRINTNL   </PropertyGroup>
PRINTNL   <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='%(TARG_DEBUG)|%(TARGNAME)'" Label="Configuration">
IF %(TYPE)==DLL
  PRINTNL     <ConfigurationType>DynamicLibrary</ConfigurationType>
ELIF %(TYPE)==LIB
  PRINTNL     <ConfigurationType>StaticLibrary</ConfigurationType>
ELSE
  PRINTNL     <ConfigurationType>Application</ConfigurationType>
ENDIF
PRINTNL     <PlatformToolset>%(PLATFORMTOOLSET)</PlatformToolset>
PRINTNL     <CharacterSet>Unicode</CharacterSet>
PRINTNL   </PropertyGroup>
PRINTNL   <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
PRINTNL   <ImportGroup Label="ExtensionSettings">
PRINTNL   </ImportGroup>
PRINTNL   <ImportGroup Condition="'$(Configuration)|$(Platform)'=='%(TARG_RELEASE)|%(TARGNAME)'" Label="PropertySheets">
PRINTNL     <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
PRINTNL   </ImportGroup>
PRINTNL   <ImportGroup Condition="'$(Configuration)|$(Platform)'=='%(TARG_DEBUG)|%(TARGNAME)'" Label="PropertySheets">
PRINTNL     <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
PRINTNL   </ImportGroup>
PRINTNL   <PropertyGroup Label="UserMacros" />
PRINTNL   <PropertyGroup>
PRINTNL     <_ProjectFileVersion>14.0.25431.1</_ProjectFileVersion>
PRINTNL   </PropertyGroup>
PRINTNL   <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='%(TARG_DEBUG)|%(TARGNAME)'">
PRINTNL     <OutDir>%(¯~!OUTPUTPATH)debug\%(~+OUTPUTDIR)</OutDir>
PRINTNL     <IntDir>%(¯~!BUILDPATH)debug\%(~§PATH)\%(NAME)\</IntDir>
IFDEF COMPILER_CLANG
  PRINTNL     <UseMultiToolTask>true</UseMultiToolTask>
ENDIF
PRINTNL   </PropertyGroup>
PRINTNL   <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='%(TARG_RELEASE)|%(TARGNAME)'">
PRINTNL     <OutDir>%(¯~!OUTPUTPATH)release\%(~+OUTPUTDIR)</OutDir>
PRINTNL     <IntDir>%(¯~!BUILDPATH)release\%(~§PATH)\%(NAME)\</IntDir>
IFDEF COMPILER_CLANG
  PRINTNL     <UseMultiToolTask>true</UseMultiToolTask>
ENDIF
PRINTNL   </PropertyGroup>
PRINTNL   <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='%(TARG_DEBUG)|%(TARGNAME)'">
PRINTNL     <Midl>
PRINTNL       <PreprocessorDefinitions>_DEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_DEBUG:%=;%)</PreprocessorDefinitions>
PRINTNL       <AdditionalIncludeDirectories>%(@&INCLUDE:%= ;%)%(@&INCLUDE_DEBUG:%= ;%)%(@&SYSINCLUDE:%= ;%) ;%(&TARGINC)%(@&CONFIG_INCLUDE:%= ;%)</AdditionalIncludeDirectories>
PRINTNL       <MkTypLibCompatible>false</MkTypLibCompatible>
PRINTNL       <TypeLibraryName>$(TargetDir)$(ProjectName).tlb</TypeLibraryName>
PRINTNL       <OutputDirectory>%(¯~!BUILDPATH)%(~§PATH)</OutputDirectory>
PRINTNL       <HeaderFileName>%%(Filename)_i.h</HeaderFileName>
PRINTNL     </Midl>
PRINTNL     <ClCompile>
PRINTNL       <ObjectFileName>$(IntDir)%%(RelativeDir)%%(filename).obj</ObjectFileName>
IF "%(TARGCFLAGS)"!=""
  PRINTNL       <AdditionalOptions>%(TARGCFLAGS)</AdditionalOptions>
ENDIF
PRINTNL       <Optimization>Disabled</Optimization>
IF %(HAS_IDL)
  INCLUDE_DEBUG   += %(OUTPUTPATH)debug\%(OUTPUT_DIR)
  INCLUDE_RELEASE += %(OUTPUTPATH)release\%(OUTPUT_DIR)
ENDIF
FOR EACH USE
  IF %(TYPE)==GENERATE
    ../INCLUDE_DEBUG   += %(OUTPUTPATH)debug\%(OUTPUT_DIR)
    ../INCLUDE_RELEASE += %(OUTPUTPATH)release\%(OUTPUT_DIR)
  ENDIF
ENDFOR
IF "%(FULL_INLINE)"
  PRINTNL       <InlineFunctionExpansion>2</InlineFunctionExpansion>
ELIF "%(LIMIT_INLINE)"
  PRINTNL       <InlineFunctionExpansion>1</InlineFunctionExpansion>
ENDIF
IF ("%(HAS_IDL)") || ("%(HEADER_QT4)"!="") || ("%(UI_FORM_QT4)"!="")
  INCLUDE += %(¯|BUILDPATH)%(PATH)
ENDIF
IF "%(HEADER_QT4)"!=""
  INCLUDE_DEBUG += %(¯|BUILDPATH)debug\%(PATH)GeneratedFiles
  INCLUDE_RELEASE += %(¯|BUILDPATH)release\%(PATH)GeneratedFiles
ENDIF
PRINTNL       <AdditionalIncludeDirectories>%(@&INCLUDE:%=;%)%(@&INCLUDE_DEBUG:%=;%)%(@&SYSINCLUDE:%=;%);%(&TARGINC)%(@&CONFIG_INCLUDE:%=;%)</AdditionalIncludeDirectories>
PRINTNL       <PreprocessorDefinitions>_DEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_DEBUG:%=;%)</PreprocessorDefinitions>
IF %(MANAGED_CLR)==""
  PRINTNL       <BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>
ENDIF
IF "%(USE_DLL_RUNTIME)"!="" || %(MANAGED_CLR)!=""
  PRINTNL       <RuntimeLibrary>SingleThreadedDebug</RuntimeLibrary>
ELSE
  PRINTNL       <RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>
ENDIF
IF defined(NO_RUNTIME_CHECK)
  PRINTNL       <BufferSecurityCheck>false</BufferSecurityCheck>
ELSE
  PRINTNL       <BufferSecurityCheck>true</BufferSecurityCheck>
ENDIF
IF "%(PRECOMPILED_HEADER)"
  PRINTNL       <UsePrecompiledHeader>2</UsePrecompiledHeader>
  PRINTNL       <PrecompiledHeaderThrough>%(PCH_FILE)"
  IF "%(PRECOMPILED_HEADER_FORCED)"!="no"
    HEADER_FORCE += %(PCH_FILE)
  ENDIF
ENDIF
IF "%(HEADER_FORCE)"
  PRINTNL       <ForcedIncludeFiles>%(HEADER_FORCE:%=;%)</ForcedIncludeFiles>
ENDIF
PRINTNL       <ProgramDataBaseFileName>$(OutDir)$(ProjectName).pdb</ProgramDataBaseFileName>
IFDEF COMPILER_CLANG
  PRINTNL       <WarningLevel>EnableAllWarnings</WarningLevel>
ELSE
  PRINTNL       <WarningLevel>Level3</WarningLevel>
ENDIF
IF "%(TARGCOMPILEARCH)"!=""
  PRINTNL       <CompileForArchitecture>%(TARGCOMPILEARCH)</CompileForArchitecture>
ENDIF
IF %(TAR64)
  PRINTNL       <Detect64BitPortabilityProblems>true</Detect64BitPortabilityProblems>
ENDIF
IFDEF COMPILER_CLANG
  PRINTNL       <DebugInformationFormat>FullDebug</DebugInformationFormat>
ELSE
  PRINTNL       <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
ENDIF
IFNDEF COMPILER_CLANG
  PRINTNL       <MultiProcessorCompilation>true</MultiProcessorCompilation>
ENDIF
PRINTNL     </ClCompile>
PRINTNL     <ResourceCompile>
PRINTNL       <PreprocessorDefinitions>_DEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_DEBUG:%=;%)</PreprocessorDefinitions>
PRINTNL       <AdditionalIncludeDirectories>%(@&INCLUDE:%=;%)%(@&INCLUDE_DEBUG:%=;%)%(@&SYSINCLUDE:%=;%);%(&TARGINC)%(@&CONFIG_INCLUDE:%=;%)</AdditionalIncludeDirectories>
PRINTNL     </ResourceCompile>
IF %(TYPE)==LIB
  PRINTNL     <Lib>
  PRINTNL     </Lib>
ELIF %(TYPE)==EXE || %(TYPE)=CON
  PRINTNL     <Link>
  IF (%(COM_COMPONENT)!="" || %(MANAGED_CLR)!="") && !defined(TARGET_WINCE)
    PRINTNL       <RegisterOutput>true</RegisterOutput>
  ENDIF
  IF "%(LIBS)"||"%(LIBS_DEBUG)"
    PRINTNL       <AdditionalDependencies>%(LIBS:%=;%)%(LIBS_DEBUG:%=;%)</AdditionalDependencies>
  ENDIF
  IF "%(ADDOPTIONS)"!=""
    PRINTNL       <AdditionalOptions>%(ADDOPTIONS)</AdditionalOptions>
  ENDIF
  IF "%(USEBUILT)"!=""
    PRINTNL       <AdditionalLibraryDirectories>%(@LIBINCLUDE:%=;%)%(@LIBINCLUDE_DEBUG:%=;%);%(~!OUTPUTPATH)debug\</AdditionalLibraryDirectories>
  ELSE
    PRINTNL       <AdditionalLibraryDirectories>%(@LIBINCLUDE:%=;%)%(@LIBINCLUDE_DEBUG:%=;%)</AdditionalLibraryDirectories>
  ENDIF
  IF "%(USE_DLL_RUNTIME)"!="" && "%(MANIFEST_DEBUG_DEP)"
    PRINTNL       <AdditionalManifestDependencies>%(MANIFEST_DEBUG_DEP)</AdditionalManifestDependencies>
  ENDIF
  IF %(TARGDEPLOY)
    PRINTNL       <DelayLoadDLLs>$(NOINHERIT)</DelayLoadDLLs>
  ENDIF
  IF %(DEF)
    PRINTNL       <ModuleDefinitionFile>%(~!DEF)</ModuleDefinitionFile>
  ENDIF
    PRINTNL       <GenerateDebugInformation>true</GenerateDebugInformation>
  IF %(MANAGED_CLR)!=""
    PRINTNL       <AssemblyDebug>1</AssemblyDebug>
  ENDIF
  PRINTNL       <GenerateMapFile>true</GenerateMapFile>
  PRINTNL       <MapFileName>$(IntDir)$(ProjectName).map</MapFileName>
  IF %(TARGSUBSYSTEMX)
    PRINTNL       <SubSystem>%(TARGSUBSYSTEMX)</SubSystem>
  ENDIF
  IF defined(TARGET_WINCE) && (%(TYPE)==EXE)
    PRINTNL       <StackReserveSize>65536</StackReserveSize>
  ENDIF
  IF "%(USE_MFC)" && "%(COM_COMPONENT)"=="" && defined(TARGET_WINCE)
    PRINTNL       <EntryPointSymbol>WinMainCRTStartup</EntryPointSymbol>
  ENDIF
  IF "%(TARGMACHINE)"
    PRINTNL       <TargetMachine>%(TARGMACHINE)</TargetMachine>
  ENDIF
  IF %(MANAGED_CLR)!=""
    PRINTNL       <CLRUnmanagedCodeCheck>true</CLRUnmanagedCodeCheck>
  ENDIF
  PRINTNL     </Link>
ENDIF
IF "%(RUN)"
  PRINTNL     <PreBuildEvent>
  PRINTNL       <Command>
  FOR EACH RUN
    PRINTNL cmd /c "cd $(OutDir) &amp;&amp; %(NAME)"
  ENDFOR
  PRINTNL       </Command>
  PRINTNL     </PreBuildEvent>
ENDIF
PRINTNL   </ItemDefinitionGroup>
PRINTNL   <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='%(TARG_RELEASE)|%(TARGNAME)'">
PRINTNL     <Midl>
PRINTNL       <PreprocessorDefinitions>NDEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_RELEASE:%=;%)</PreprocessorDefinitions>
PRINTNL       <AdditionalIncludeDirectories>%(@&INCLUDE:%= ;%)%(@&INCLUDE_RELEASE:%= ;%)%(@&SYSINCLUDE:%= ;%) ;%(&TARGINC)%(@&CONFIG_INCLUDE:%= ;%)</AdditionalIncludeDirectories>
PRINTNL       <MkTypLibCompatible>false</MkTypLibCompatible>
PRINTNL       <TypeLibraryName>$(TargetDir)$(ProjectName).tlb</TypeLibraryName>
PRINTNL       <OutputDirectory>%(¯~!BUILDPATH)%(~§PATH)</OutputDirectory>
PRINTNL       <HeaderFileName>%%(Filename)_i.h</HeaderFileName>
PRINTNL     </Midl>
PRINTNL     <ClCompile>
PRINTNL       <ObjectFileName>$(IntDir)%%(RelativeDir)%%(filename).obj</ObjectFileName>
PRINTNL       <Optimization>Full</Optimization>
IF "%(FULL_INLINE)"
  PRINTNL       <InlineFunctionExpansion>2</InlineFunctionExpansion>
ELIF "%(LIMIT_INLINE)"
  PRINTNL       <InlineFunctionExpansion>1</InlineFunctionExpansion>
ENDIF
IF "%(FAVOR_MAX_SPEED)"
  PRINTNL       <EnableIntrinsicFunctions>true</EnableIntrinsicFunctions>
  PRINTNL       <FavorSizeOrSpeed>1</FavorSizeOrSpeed>
  PRINTNL       <OmitFramePointers>true</OmitFramePointers>
ENDIF
IF !DEFINED(IX86_64)
  IF DEFINED(COMPILER_ICL11)
    PRINTNL       <EnableEnhancedInstructionSet>1001</EnableEnhancedInstructionSet>
  ELIF "%(FASTER_FLOAT)"
    PRINTNL       <EnableEnhancedInstructionSet>1</EnableEnhancedInstructionSet>
  ENDIF
ENDIF
IF "%(OPTIMIZE_GLOBALLY)"
  PRINTNL       <WholeProgramOptimization>true</WholeProgramOptimization>
ENDIF
IF "%(HEADER_QT4)"!=""
  DEFINE += QT_NO_DEBUG
  DEFINE += QT_NO_DEBUG_OUTPUT
ENDIF
PRINTNL       <AdditionalIncludeDirectories>%(@&INCLUDE:%=;%)%(@&INCLUDE_RELEASE:%=;%)%(@&SYSINCLUDE:%=;%);%(&TARGINC)%(@&CONFIG_INCLUDE:%=;%)</AdditionalIncludeDirectories>
PRINTNL       <PreprocessorDefinitions>NDEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_RELEASE:%=;%)</PreprocessorDefinitions>
PRINTNL       <StringPooling>true</StringPooling>
IF !defined(TARGET_WINCE)
  IF "%(USE_DLL_RUNTIME)"!="" || %(MANAGED_CLR)!=""
    PRINTNL       <RuntimeLibrary>SingleThreaded</RuntimeLibrary>
  ELSE
    PRINTNL       <RuntimeLibrary>MultiThreaded</RuntimeLibrary>
  ENDIF
ENDIF
PRINTNL       <BufferSecurityCheck>false</BufferSecurityCheck>
IF "%(REDUCE_SIZE)" || defined(CONFIG_COREPROTECT)
  PRINTNL       <EnableFunctionLevelLinking>true</EnableFunctionLevelLinking>
ENDIF
IF "%(PRECOMPILED_HEADER)"
  PRINTNL       <UsePrecompiledHeader>2</UsePrecompiledHeader>
  PRINTNL       <PrecompiledHeaderThrough>%(PCH_FILE)</PrecompiledHeaderThrough>
ENDIF
IF "%(HEADER_FORCE)"
  PRINTNL       <ForcedIncludeFiles>%(HEADER_FORCE:%=;%)</ForcedIncludeFiles>
ENDIF
IFDEF COMPILER_CLANG
  PRINTNL       <WarningLevel>EnableAllWarnings</WarningLevel>
ELSE
  PRINTNL       <WarningLevel>Level3</WarningLevel>
ENDIF
IF "%(TARGCOMPILEARCH)"!=""
  PRINTNL       <CompileForArchitecture>%(TARGCOMPILEARCH)</CompileForArchitecture>
ENDIF
IF %(TAR64)
  PRINTNL       <Detect64BitPortabilityProblems>true</Detect64BitPortabilityProblems>
ENDIF
IFDEF COMPILER_CLANG
  PRINTNL       <DebugInformationFormat>None</DebugInformationFormat>
ENDIF
IFNDEF COMPILER_CLANG
  PRINTNL       <MultiProcessorCompilation>true</MultiProcessorCompilation>
ENDIF
PRINTNL     </ClCompile>
PRINTNL     <ResourceCompile>
PRINTNL       <PreprocessorDefinitions>NDEBUG%(&TARGDEF:%=;%)%(&DEFINE:%=;%)%(&DEFINE_RELEASE:%=;%)</PreprocessorDefinitions>
PRINTNL       <AdditionalIncludeDirectories>%(@&INCLUDE:%=;%)%(@&INCLUDE_RELEASE:%=;%)%(@&SYSINCLUDE:%=;%);%(&TARGINC)%(@&CONFIG_INCLUDE:%=;%)</AdditionalIncludeDirectories>
PRINTNL     </ResourceCompile>
IF %(TYPE)==LIB
  PRINTNL     <Lib>
  PRINTNL     </Lib>
ELIF %(TYPE)==EXE || %(TYPE)=CON
  PRINTNL     <Link>
  IF (%(COM_COMPONENT)!="" || %(MANAGED_CLR)!="") && !defined(TARGET_WINCE)
    PRINTNL       <RegisterOutput>true</RegisterOutput>
  ENDIF
  IF "%(LIBS)"||"%(LIBS_RELEASE)"
    PRINTNL       <AdditionalDependencies>%(LIBS:%=;%)%(LIBS_RELEASE:%=;%)</AdditionalDependencies>
  ENDIF
  IF "%(ADDOPTIONS)"!=""
    PRINTNL       <AdditionalOptions>%(ADDOPTIONS)</AdditionalOptions>
  ENDIF
  IF "%(USEBUILT)"!=""
    PRINTNL       <AdditionalLibraryDirectories>%(@LIBINCLUDE:%=;%)%(@LIBINCLUDE_RELEASE:%=;%);%(~!OUTPUTPATH)release\</AdditionalLibraryDirectories>
  ELSE
    PRINTNL       <AdditionalLibraryDirectories>%(@LIBINCLUDE:%=;%)%(@LIBINCLUDE_RELEASE:%=;%)</AdditionalLibraryDirectories>
  ENDIF
  IF "%(USE_DLL_RUNTIME)"!="" && "%(MANIFEST_RELEASE_DEP)"
    PRINTNL       <AdditionalManifestDependencies>%(MANIFEST_RELEASE_DEP)</AdditionalManifestDependencies>
  ENDIF
  IF %(TARGDEPLOY)
    PRINTNL       <DelayLoadDLLs>$(NOINHERIT)</DelayLoadDLLs>
  ENDIF
  IF %(DEF)
    PRINTNL       <ModuleDefinitionFile>%(~!DEF)</ModuleDefinitionFile>
  ENDIF
  IF %(MANAGED_CLR)!=""
    PRINTNL       <AssemblyDebug>1</AssemblyDebug>
  ENDIF
  PRINTNL       <GenerateMapFile>true</GenerateMapFile>
  PRINTNL       <MapFileName>$(IntDir)$(ProjectName).map</MapFileName>
  IF %(TARGSUBSYSTEMX)
    PRINTNL       <SubSystem>%(TARGSUBSYSTEMX)</SubSystem>
  ENDIF
  IF defined(TARGET_WINCE) && (%(TYPE)==EXE)
    PRINTNL       <StackReserveSize>65536</StackReserveSize>
  ENDIF
  PRINTNL       <OptimizeReferences>true</OptimizeReferences>
  PRINTNL       <EnableCOMDATFolding>true</EnableCOMDATFolding>
  IF "%(USE_MFC)" && "%(COM_COMPONENT)"=="" && defined(TARGET_WINCE)
    PRINTNL       <EntryPointSymbol>WinMainCRTStartup</EntryPointSymbol>
  ENDIF
  IF "%(TARGMACHINE)"
    PRINTNL       <TargetMachine>%(TARGMACHINE)</TargetMachine>
  ENDIF
  IF %(MANAGED_CLR)!=""
    PRINTNL       <CLRUnmanagedCodeCheck>true</CLRUnmanagedCodeCheck>
  ENDIF
  PRINTNL     </Link>
ENDIF
IF "%(RUN)"
  PRINTNL     <PreBuildEvent>
  PRINTNL       <Command>
  FOR EACH RUN
    PRINTNL cmd /c "cd $(OutDir) &amp;&amp; %(NAME)"
  ENDFOR
  PRINTNL       </Command>
  PRINTNL     </PreBuildEvent>
ENDIF
PRINTNL   </ItemDefinitionGroup>
PRINTNL   <ItemGroup>
FOR EACH USE, DEP
  PRINTNL     <ProjectReference Include="%(¯~!PATH)%(NAME).vcxproj">
  PRINTNL       <Project>%(GUID)</Project>
  PRINTNL     </ProjectReference>
ENDFOR
PRINTNL   </ItemGroup>
PRINTNL   <ItemGroup>
FOR EACH SOURCE
IF (%(_copy_item))
  PRINTNL     <ClCompile Include="%(~!NAME)" DeploymentContent="true">
ELSE
  PRINTNL     <ClCompile Include="%(~!NAME)">
ENDIF
IF ((%(>NAME)==c || %(>NAME)==cpp) && %(../PCH_SOURCE) && %(NO_PCH)=="")
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CPP_TOOL)"
  PRINTNL             UsePrecompiledHeader="1"
  IF %(w)
    PRINTNL             WarningLevel="0"
  ENDIF
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CPP_TOOL)"
  PRINTNL             UsePrecompiledHeader="1"
  IF %(w)
    PRINTNL             WarningLevel="0"
  ENDIF
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  ../PCH_SOURCE =
ELIF ((%(>NAME)==c || %(>NAME)==cpp) && (%(w) || (%(NO_PCH)!="" && %(PRECOMPILED_HEADER))))
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CPP_TOOL)"
  IF %(w)
    PRINTNL             WarningLevel="0"
  ENDIF
  IF %(NO_PCH)!="" && %(PRECOMPILED_HEADER)
    PRINTNL             UsePrecompiledHeader="0"
  ENDIF
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CPP_TOOL)"
  IF %(w)
    PRINTNL             WarningLevel="0"
  ENDIF
  IF %(NO_PCH)!="" && %(PRECOMPILED_HEADER)
    PRINTNL             UsePrecompiledHeader="0"
  ENDIF
  PRINTNL           />
  PRINTNL         </FileConfiguration>
ELIF (%(>NAME)==asm)
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="Assembling %(~!NAME)..."
  IFDEF IX86
    PRINTNL             CommandLine="yasm -f win32 -D_WIN32 %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) %(@&INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF IX86_64
    PRINTNL             CommandLine="yasm -f win64 -D_WIN64 %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) %(@&INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF ARM
    IF %(CPU)==armv6
      PRINTNL             CommandLine="armasm -cpu arm1136 &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
    ELSE
      PRINTNL             CommandLine="armasm -cpu xscale &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
    ENDIF
  ENDIF
  PRINTNL             Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="Assembling %(~!NAME)..."
  IFDEF IX86
    PRINTNL             CommandLine="yasm -f win32 -D_WIN32 %(DEFINE:%=-D%) %(DEFINE_DEBUG:%=-D%) %(@&INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF IX86_64
    PRINTNL             CommandLine="yasm -f win64 -D_WIN64 %(DEFINE:%=-D%) %(DEFINE_DEBUG:%=-D%) %(@&INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF ARM
    IF %(CPU)==armv6
      PRINTNL             CommandLine="armasm -cpu arm1136 &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
    ELSE
      PRINTNL             CommandLine="armasm -cpu xscale &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
    ENDIF
  ENDIF
  PRINTNL             Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
ELIF (%(>NAME)==nas)
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="Assembling %(~!NAME)..."
  IFDEF IX86
    PRINTNL             CommandLine="nasmw -f win32 -D_WIN32 -DWIN32 %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) %(@INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF IX86_64
    PRINTNL             CommandLine="nasmw -f win64 -D_WIN64 -DWIN64 %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) %(@INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF ARM
    PRINTNL             CommandLine="armasm -cpu xscale &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
  ENDIF
  PRINTNL             Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="Assembling %(~!NAME)..."
  IFDEF IX86
    PRINTNL             CommandLine="nasmw -f win32 -D_WIN32 -DWIN32 %(DEFINE:%=-D%) %(DEFINE_DEBUG:%=-D%) %(@INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF IX86_64
    PRINTNL             CommandLine="nasmw -f win64 -D_WIN64 -DWIN64 %(DEFINE:%=-D%) %(DEFINE_DEBUG:%=-D%) %(@INCLUDE:%=-I%) -o &quot;$(IntDir)/$(InputName).obj&quot; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  ELIF ARM
    PRINTNL             CommandLine="armasm -cpu xscale &quot;$(InputPath)&quot; &quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
  ENDIF
  PRINTNL             Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
ELIF (%(>NAME)==c) && defined(MIPS) && ("%(TARGGCC)"!="") && %(gcc)
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             CommandLine="%(TARGGCC) -DNDEBUG %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) %(@INCLUDE:%=-I%) %(@CONFIG_INCLUDE:%=-I%) -S &quot;$(InputPath)&quot; -o&quot;$(IntDir)/$(InputName).S&quot;%(CMD_NEWLINE)mips2s &quot;$(IntDir)/$(InputName).S&quot;%(CMD_NEWLINE)mipsasm /QMmips32 /c &quot;$(IntDir)/$(InputName)_gcc.S&quot; /Fo&quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
  PRINTNL             Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL         </FileConfiguration>
ELIF (%(>NAME)==s) && defined(MIPS)
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="Assembling %(~!NAME)..."
  PRINTNL             CommandLine="mipsasm /QMmips32 /c &quot;$(InputPath)&quot; /Fo&quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
  PRINTNL             Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="Assembling %(~!NAME)..."
  PRINTNL             CommandLine="mipsasm /QMmips32 /c &quot;$(InputPath)&quot; /Fo&quot;$(IntDir)/$(InputName).obj&quot;%(CMD_NEWLINE)"
  PRINTNL             Outputs="$(IntDir)/$(InputName).obj"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
ELIF (%(_copy_item))
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="Copying $(InputPath) to $(TargetDir)..."
  PRINTNL             CommandLine="copy /y &quot;$(InputPath)&quot; &quot;$(TargetDir)&quot; &gt;nul"
  PRINTNL             Outputs="$(TargetDir)/$(InputFileName)"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="Copying $(InputPath) to $(TargetDir)..."
  PRINTNL             CommandLine="copy /y &quot;$(InputPath)&quot; &quot;$(TargetDir)&quot; &gt;nul"
  PRINTNL             Outputs="$(TargetDir)/$(InputFileName)"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
ELIF (%(>NAME)==r)
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="Resource forking %(~!NAME)..."
  PRINTNL             CommandLine="&quot;%(CONFIG_QUICKTIME_SDK)\Tools\Rez.exe&quot; -p %(~@INCLUDE:%=-i &quot;%&quot;) -i &quot;%(CONFIG_QUICKTIME_SDK)\RIncludes&quot; -d TARGET_REZ_CARBON_MACHO=0 -o &quot;$(IntDir)\$(InputName).qtr&quot; &lt; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  PRINTNL             Outputs="$(IntDir)/$(InputName).qtr"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="Resource forking %(~!NAME)..."
  PRINTNL             CommandLine="&quot;%(CONFIG_QUICKTIME_SDK)\Tools\Rez.exe&quot; -p %(~@INCLUDE:%=-i &quot;%&quot;) -i &quot;%(CONFIG_QUICKTIME_SDK)\RIncludes&quot; -d TARGET_REZ_CARBON_MACHO=0 -o &quot;$(IntDir)\$(InputName).qtr&quot; &lt; &quot;$(InputPath)&quot;%(CMD_NEWLINE)"
  PRINTNL             Outputs="$(IntDir)/$(InputName).qtr"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
ENDIF
PRINTNL     </ClCompile>
ENDFOR
FOR EACH DEF
  PRINTNL     <ClCompile Include="%(~!NAME)"/>
ENDFOR
PRINTNL   </ItemGroup>
IF %(HEADER_QT4) || %(HEADER) || %(HEADER_FORCE)
  PRINTNL   <ItemGroup>
  FOR EACH HEADER_QT4
    PRINTNL     <ClInclude Include="%(~!NAME)">
    PRINTNL         <FileConfiguration
    PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
    PRINTNL           >
    PRINTNL           <Tool
    PRINTNL             Name="%(CUSTOM_TOOL)"
    PRINTNL             Description="Moc&apos;ing %(~!NAME)..."
//    IF "%(PCH_FILE)"
//      PRINTNL             CommandLine="$(QTDIR)\bin\moc.exe -f%(PCH_FILE) %(DEFINE:%=-D%) -I&quot;%(~!BUILDPATH)debug\%(PATH)GeneratedFiles\%(@INCLUDE:%=;%)&quot; &quot;%(~!NAME)&quot; -o &quot;%(~!BUILDPATH)debug\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
//    ELSE
      PRINTNL             CommandLine="$(QTDIR)\bin\moc.exe %(DEFINE:%=-D%) %(DEFINE_DEBUG:%=-D%) -I&quot;%(~!BUILDPATH)debug\%(PATH)GeneratedFiles\%(@INCLUDE:%=;%)&quot; &quot;%(~!NAME)&quot; -o &quot;%(~!BUILDPATH)debug\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
//    ENDIF
    PRINTNL             AdditionalDependencies="$(QTDIR)\bin\moc.exe;%(~!NAME)"
    PRINTNL             Outputs="%(~!BUILDPATH)debug\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp"
    PRINTNL           />
    PRINTNL         </FileConfiguration>
    PRINTNL         <FileConfiguration
    PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL           >
    PRINTNL           <Tool
    PRINTNL             Name="%(CUSTOM_TOOL)"
    PRINTNL             Description="Moc&apos;ing %(~!NAME)..."
//    IF "%(PCH_FILE)"
//      PRINTNL             CommandLine="$(QTDIR)\bin\moc.exe -f%(PCH_FILE) %(DEFINE:%=-D%) -I&quot;%(~!BUILDPATH)release\%(PATH)GeneratedFiles\%(@INCLUDE:%=;%)&quot; &quot;%(~!NAME)&quot; -o &quot;%(~!BUILDPATH)release\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
//    ELSE
      PRINTNL             CommandLine="$(QTDIR)\bin\moc.exe %(DEFINE:%=-D%) %(DEFINE_RELEASE:%=-D%) -I&quot;%(~!BUILDPATH)release\%(PATH)GeneratedFiles\%(@INCLUDE:%=;%)&quot; &quot;%(~!NAME)&quot; -o &quot;%(~!BUILDPATH)release\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
//    ENDIF
    PRINTNL             AdditionalDependencies="$(QTDIR)\bin\moc.exe;%(~!NAME)"
    PRINTNL             Outputs="%(~!BUILDPATH)release\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp"
    PRINTNL           />
    PRINTNL         </FileConfiguration>
    PRINTNL     </ClInclude>
  ENDFOR
  FOR EACH HEADER,HEADER_FORCE
    PRINTNL     <ClInclude Include="%(~!NAME)">
    IF (%(>NAME)==cpp) || (%(>NAME)==c)
      PRINTNL         <FileConfiguration
      PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
      PRINTNL           >
      PRINTNL           <Tool
      PRINTNL             Name="VCCustomBuildTool"
      PRINTNL           />
      PRINTNL         </FileConfiguration>
      PRINTNL         <FileConfiguration
      PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
      PRINTNL           >
      PRINTNL           <Tool
      PRINTNL             Name="VCCustomBuildTool"
      PRINTNL           />
      PRINTNL         </FileConfiguration>
    ENDIF
    IF %(resx)!=""
      PRINTNL         <ClInclude
      PRINTNL           RelativePath="%(<NAME).resX"
      PRINTNL           SubType="Designer"
      PRINTNL           >
      PRINTNL     </ClInclude>
    ENDIF
    PRINTNL     </ClInclude>
  ENDFOR
  FOR EACH HEADER_QT4
    PRINTNL       <File
    PRINTNL         RelativePath="%(!BUILDPATH)%(PATH)ui_%(~<!NAME).h"
    PRINTNL         >
    PRINTNL       </File>
  ENDFOR
  PRINTNL   </ItemGroup>
ENDIF
IF "%(XRC_SOURCE)"!=""
  PRINTNL   <ItemGroup>
  FOR EACH XRC_SOURCE
    PRINTNL       <File
    PRINTNL         RelativePath="%(~NAME)"
    PRINTNL         >
    PRINTNL         <FileConfiguration
    PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
    PRINTNL           >
    PRINTNL           <Tool
    PRINTNL             Name="%(CUSTOM_TOOL)"
    PRINTNL             Description="wxRC&apos;ing %(~!NAME)..."
    PRINTNL             CommandLine="&quot;$(WXWIN)\bin\wxrc.exe&quot; /c /o &quot;%(~!BUILDPATH)%(~PATH)xrc_%(~<!NAME).cpp&quot; &quot;%(NAME)&quot;%(CMD_NEWLINE)"
    PRINTNL             AdditionalDependencies="$(WXWIN)\bin\wxrc.exe"
    PRINTNL             Outputs="%(~!BUILDPATH)%(~PATH)xrc_%(~<!NAME).cpp"
    PRINTNL           />
    PRINTNL         </FileConfiguration>
    PRINTNL         <FileConfiguration
    PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL           >
    PRINTNL           <Tool
    PRINTNL             Name="%(CUSTOM_TOOL)"
    PRINTNL             Description="wxRC&apos;ing %(~!NAME)..."
    PRINTNL             CommandLine="&quot;$(WXWIN)\bin\wxrc.exe&quot; /c /o &quot;%(~!BUILDPATH)%(~PATH)xrc_%(~<!NAME).cpp&quot; &quot;%(NAME)&quot;%(CMD_NEWLINE)"
    PRINTNL             AdditionalDependencies="$(WXWIN)\bin\wxrc.exe"
    PRINTNL             Outputs="%(~!BUILDPATH)%(~PATH)xrc_%(~<!NAME).cpp"
    PRINTNL           />
    PRINTNL         </FileConfiguration>
    PRINTNL       </File>
  ENDFOR
  PRINTNL   </ItemGroup>
ENDIF
IF "%(UI_FORM_QT4)"!=""
  PRINTNL   <ItemGroup>
  FOR EACH UI_FORM_QT4
    PRINTNL       <File
    PRINTNL         RelativePath="%(~!NAME)"
    PRINTNL         >
    PRINTNL         <FileConfiguration
    PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
    PRINTNL           >
    PRINTNL           <Tool
    PRINTNL             Name="%(CUSTOM_TOOL)"
    PRINTNL             Description="Uic&apos;ing %(~!NAME)..."
    PRINTNL             CommandLine="&quot;$(QTDIR)\bin\uic.exe&quot; -o &quot;%(!BUILDPATH)%(PATH)ui_%(~<!NAME).h&quot; &quot;%(~!NAME)&quot;%(CMD_NEWLINE)"
    PRINTNL             AdditionalDependencies="$(QTDIR)\bin\uic.exe"
    PRINTNL             Outputs="%(!BUILDPATH)%(PATH)ui_%(~<!NAME).h"
    PRINTNL           />
    PRINTNL         </FileConfiguration>
    PRINTNL         <FileConfiguration
    PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL           >
    PRINTNL           <Tool
    PRINTNL             Name="%(CUSTOM_TOOL)"
    PRINTNL             Description="Uic&apos;ing %(~!NAME)..."
    PRINTNL             CommandLine="&quot;$(QTDIR)\bin\uic.exe&quot; -o &quot;%(!BUILDPATH)%(PATH)ui_%(~<!NAME).h&quot; &quot;%(~!NAME)&quot;%(CMD_NEWLINE)"
    PRINTNL             AdditionalDependencies="$(QTDIR)\bin\uic.exe"
    PRINTNL             Outputs="%(!BUILDPATH)%(PATH)ui_%(~<!NAME).h"
    PRINTNL           />
    PRINTNL         </FileConfiguration>
    PRINTNL       </File>
  ENDFOR
  PRINTNL   </ItemGroup>
ENDIF
IF "%(HEADER_QT4)"!=""||"%(RESOURCE_QT4)"!=""||"%(XRC_SOURCE)"!=""||"%(HAS_IDL)"!=""
  PRINTNL   <ItemGroup>
  FOR EACH SOURCE
    IF (%(>NAME)==idl) && %(hmarshal)!=""
      PRINTNL       <File
      PRINTNL         RelativePath="%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.h"
      PRINTNL         >
      PRINTNL       </File>
    ENDIF
    IF (%(>NAME)==idl) && %(cmarshal)!=""
      PRINTNL       <File
      PRINTNL         RelativePath="%(¯~!BUILDPATH)%(~§PATH)\%(<NAME)_i.c"
      PRINTNL         >
      IF "%(PRECOMPILED_HEADER)"
        PRINTNL          <FileConfiguration
        PRINTNL            Name="%(TARG_DEBUG)|%(TARGNAME)"
        PRINTNL            >
        PRINTNL            <Tool
        PRINTNL              Name="%(CPP_TOOL)"
        PRINTNL              UsePrecompiledHeader="0"
        PRINTNL            />
        PRINTNL          </FileConfiguration>
        PRINTNL          <FileConfiguration
        PRINTNL            Name="%(TARG_RELEASE)|%(TARGNAME)"
        PRINTNL            >
        PRINTNL            <Tool
        PRINTNL              Name="%(CPP_TOOL)"
        PRINTNL              UsePrecompiledHeader="0"
        PRINTNL            />
        PRINTNL          </FileConfiguration>
      ENDIF
      PRINTNL       </File>
    ENDIF
  ENDFOR
  FOR EACH XRC_SOURCE
    PRINTNL       <File
    PRINTNL         RelativePath="%(¯~!BUILDPATH)%(~PATH)xrc_%(~<!NAME).cpp"
    PRINTNL         >
    PRINTNL       </File>
  ENDFOR
  FOR EACH HEADER_QT4
   IF (%(>NAME)!=cpp)
    PRINTNL       <File
    PRINTNL         RelativePath="%(¯~!BUILDPATH)debug\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp"
    PRINTNL         >
    PRINTNL        <FileConfiguration
    PRINTNL          Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL          ExcludedFromBuild="true"
    PRINTNL          >
    PRINTNL          <Tool
    PRINTNL            Name="%(CPP_TOOL)"
  //  PRINTNL            UsePrecompiledHeader="0"
    PRINTNL            />
    PRINTNL        </FileConfiguration>
    PRINTNL      </File>
    PRINTNL      <File
    PRINTNL        RelativePath="%(¯~!BUILDPATH)release\%(PATH)GeneratedFiles\moc_%(~<!NAME).cpp"
    PRINTNL        >
    PRINTNL        <FileConfiguration
    PRINTNL          Name="%(TARG_DEBUG)|%(TARGNAME)"
    PRINTNL          ExcludedFromBuild="true"
    PRINTNL          >
    PRINTNL          <Tool
    PRINTNL            Name="%(CPP_TOOL)"
  //  PRINTNL            UsePrecompiledHeader="0"
    PRINTNL          />
    PRINTNL        </FileConfiguration>
    PRINTNL       </File>
   ENDIF
  ENDFOR
  FOR EACH RESOURCE_QT4
    PRINTNL       <File
    PRINTNL         RelativePath="%(¯~!BUILDPATH)%(PATH)GeneratedFiles\qrc_%(~<!NAME).cpp"
    PRINTNL         >
    IF "%(PRECOMPILED_HEADER)"!="" && "%(PRECOMPILED_HEADER_FORCED)"=="no"
      PRINTNL         <FileConfiguration
      PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
      PRINTNL           >
      PRINTNL           <Tool
      PRINTNL             Name="%(CPP_TOOL)"
      PRINTNL             UsePrecompiledHeader="0"
      PRINTNL           />
      PRINTNL         </FileConfiguration>
      PRINTNL         <FileConfiguration
      PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
      PRINTNL           >
      PRINTNL           <Tool
      PRINTNL             Name="%(CPP_TOOL)"
      PRINTNL             UsePrecompiledHeader="0"
      PRINTNL           />
      PRINTNL         </FileConfiguration>
    ENDIF
    PRINTNL       </File>
  ENDFOR
  PRINTNL   </ItemGroup>
ENDIF
IF "%(RESOURCE_QT4)"!=""
  PRINTNL   <ItemGroup>
  FOR EACH RESOURCE_QT4
    PRINTNL       <File
    PRINTNL         RelativePath="%(~!NAME)"
    PRINTNL         >
      PRINTNL         <FileConfiguration
      PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
      PRINTNL           >
      PRINTNL           <Tool
      PRINTNL             Name="%(CUSTOM_TOOL)"
      PRINTNL             Description="Rcc&apos;ing %(~!NAME)..."
      PRINTNL             CommandLine="&quot;$(QTDIR)\bin\rcc.exe&quot; -compress 1 -name %(~<!NAME) %(~!NAME) -o &quot;%(¯!BUILDPATH)%(PATH)GeneratedFiles\qrc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
      PRINTNL             AdditionalDependencies="$(QTDIR)\bin\rcc.exe"
      PRINTNL             Outputs="%(¯!BUILDPATH)%(PATH)GeneratedFiles\qrc_%(~<!NAME).cpp"
      PRINTNL           />
      PRINTNL         </FileConfiguration>
      PRINTNL         <FileConfiguration
      PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
      PRINTNL           >
      PRINTNL           <Tool
      PRINTNL             Name="%(CUSTOM_TOOL)"
      PRINTNL             Description="Rcc&apos;ing %(~!NAME)..."
      PRINTNL             CommandLine="&quot;$(QTDIR)\bin\rcc.exe&quot; -name %(~<!NAME) %(~!NAME) -o &quot;%(¯!BUILDPATH)%(PATH)GeneratedFiles\qrc_%(~<!NAME).cpp&quot;%(CMD_NEWLINE)"
      PRINTNL             AdditionalDependencies="$(QTDIR)\bin\rcc.exe"
      PRINTNL             Outputs="%(¯!BUILDPATH)%(PATH)GeneratedFiles\qrc_%(~<!NAME).cpp"
      PRINTNL           />
      PRINTNL         </FileConfiguration>
    PRINTNL       </File>
  ENDFOR
  PRINTNL   </ItemGroup>
ENDIF
IF "%(I18N)"
  PRINTNL   <ItemGroup>
  PRINTNL       <File
  PRINTNL         RelativePath="%(~I18N)\%(../NAME).pot"
  PRINTNL         >
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
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
  IF "%(I18N_DEBUG)"==""
    PRINTNL           ExcludedFromBuild="true"
  ENDIF
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="i18n&apos;ing language %(NAME)..."
  PRINTNL             CommandLine="%(I18NCOMMAND)"
  PRINTNL             AdditionalDependencies="%(I18NSOURCES:%=%;)"
  PRINTNL             Outputs="%(~I18N)\%(../NAME).pot"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  PRINTNL         <FileConfiguration
  PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
  PRINTNL           >
  PRINTNL           <Tool
  PRINTNL             Name="%(CUSTOM_TOOL)"
  PRINTNL             Description="i18n&apos;ing language %(NAME)..."
  PRINTNL             CommandLine="%(I18NCOMMAND)"
  PRINTNL             AdditionalDependencies="%(I18NSOURCES:%=%;)"
  PRINTNL             Outputs="%(~I18N)\%(../NAME).pot"
  PRINTNL           />
  PRINTNL         </FileConfiguration>
  PRINTNL       </File>
  FOR EACH I18N_LANG
    PRINTNL       <File
    PRINTNL         RelativePath="%(OUTDIR_LOCALE)\%(NAME).po"
    PRINTNL         >
    PRINTNL         <FileConfiguration
    PRINTNL           Name="%(TARG_DEBUG)|%(TARGNAME)"
    IF "%(I18N_DEBUG)"==""
      PRINTNL           ExcludedFromBuild="true"
    ENDIF
    PRINTNL           >
    PRINTNL           <Tool
    PRINTNL             Name="%(CUSTOM_TOOL)"
    PRINTNL             Description="i18n&apos;ing %(NAME)..."
    PRINTNL             CommandLine="if not exist &quot;$(TargetDir)%(NAME)\LC_MESSAGES&quot; mkdir &quot;$(TargetDir)%(NAME)\LC_MESSAGES&quot;%(CMD_NEWLINE)msgfmt -o $(TargetDir)%(NAME)\LC_MESSAGES\%(../NAME).mo %(OUTDIR_LOCALE)\%(NAME).po%(CMD_NEWLINE)"
    PRINTNL             AdditionalDependencies="%(~I18N)\%(../NAME).pot"
    PRINTNL             Outputs="$(TargetDir)%(NAME)\LC_MESSAGES\%(../NAME).mo"
    PRINTNL           />
    PRINTNL         </FileConfiguration>
    PRINTNL         <FileConfiguration
    PRINTNL           Name="%(TARG_RELEASE)|%(TARGNAME)"
    PRINTNL           >
    PRINTNL           <Tool
    PRINTNL             Name="%(CUSTOM_TOOL)"
    PRINTNL             Description="i18n&apos;ing %(NAME)..."
    PRINTNL             CommandLine="if not exist &quot;$(TargetDir)%(NAME)\LC_MESSAGES&quot; mkdir &quot;$(TargetDir)%(NAME)\LC_MESSAGES&quot;%(CMD_NEWLINE)msgfmt -o $(TargetDir)%(NAME)\LC_MESSAGES\%(../NAME).mo %(OUTDIR_LOCALE)\%(NAME).po%(CMD_NEWLINE)"
    PRINTNL             AdditionalDependencies="%(~I18N)\%(../NAME).pot"
    PRINTNL             Outputs="$(TargetDir)%(NAME)\LC_MESSAGES\%(../NAME).mo"
    PRINTNL           />
    PRINTNL         </FileConfiguration>
    PRINTNL       </File>
  ENDFOR
  PRINTNL   </ItemGroup>
ENDIF
PRINTNL   <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
PRINTNL   <ImportGroup Label="ExtensionTargets">
PRINTNL   </ImportGroup>
PRINTNL </Project>
