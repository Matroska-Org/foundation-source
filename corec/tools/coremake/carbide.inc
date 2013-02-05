CARBIDE_SYSINCLUDE += ${EPOCROOT}epoc32\include 
CARBIDE_SYSINCLUDE += ${EPOCROOT}epoc32\include\variant
CARBIDE_SYSINCLUDE += ${EPOCROOT}epoc32\include\libc

IF DEFINED(SYMBIAN90) && DEFINED(SERIES60)
  CARBIDE_SYSINCLUDE += ${EPOCROOT}epoc32\include\mmf\plugin
ENDIF

FOR EACH LIB,DLL,EXE,CON

  FILE %(¯MAKEPATH).project

  PRINTNL <?xml version="1.0" encoding="UTF-8"?>
  PRINTNL <projectDescription>
  PRINTNL 	<name>%(NAME)</name>
  PRINTNL 	<comment></comment>
  PRINTNL 	<projects>

  IF %(TYPE)==EXE
    RSS += %(¯MAKEPATH)%(PROJECT_PATH).rss
    RSS += %(¯MAKEPATH)%(PROJECT_PATH)_reg.rss
  ENDIF

  IF (%(TYPE)!=LIB)
    FOR EACH USE
      IF (!%(NOLIB))
        PRINTNL 		<project>%(NAME)</project>
      ENDIF
    ENDFOR
  ENDIF

  PRINTNL 	</projects>
  PRINTNL 	<buildSpec>
  IF !DEFINED(CARBIDE_12) && !DEFINED(CARBIDE_20)
    PRINTNL 		<buildCommand>
    PRINTNL 			<name>org.eclipse.cdt.managedbuilder.core.genmakebuilder</name>
    PRINTNL 			<arguments>
    PRINTNL 			</arguments>
    PRINTNL 		</buildCommand>
  ELSE
    PRINTNL 		<buildCommand>
    PRINTNL 			<name>com.nokia.carbide.cdt.builder.carbideCPPBuilder</name>
    PRINTNL 			<arguments>
    PRINTNL 			</arguments>
    PRINTNL 		</buildCommand>
    IF DEFINED(CARBIDE_12)
      PRINTNL 		<buildCommand>
      PRINTNL 			<name>com.nokia.carbide.cdt.rombuilder.carbideRomBuilder</name>
      PRINTNL 			<arguments>
      PRINTNL 			</arguments>
      PRINTNL 		</buildCommand>
    ENDIF
  ENDIF
  PRINTNL 	</buildSpec>
  PRINTNL 	<natures>
  PRINTNL 		<nature>org.eclipse.cdt.core.cnature</nature>
  PRINTNL 		<nature>org.eclipse.cdt.core.ccnature</nature>
  IF DEFINED(CARBIDE_12) || DEFINED(CARBIDE_20)
    PRINTNL 		<nature>com.nokia.carbide.cdt.builder.carbideCPPBuilderNature</nature>
    IF DEFINED(CARBIDE_12)
      PRINTNL 		<nature>com.nokia.carbide.cdt.rombuilder.carbideRomBuildNature</nature>
    ENDIF
  ELSE
    PRINTNL 		<nature>org.eclipse.cdt.managedbuilder.core.managedBuildNature</nature>
    PRINTNL 		<nature>com.symbian.cdt.core.symbiannature</nature>
  ENDIF
  PRINTNL 	</natures>
  PRINTNL 	<linkedResources>
  FOR EACH SOURCE,HEADER
    IF %(>NAME)!=rss
      PRINTNL 		<link>
      PRINTNL 			<name>%(<NAME).%(>NAME)</name>
      PRINTNL 			<type>1</type>
      PRINTNL 			<location>%(|NAME)</location>
      PRINTNL 		</link>
    ENDIF
  ENDFOR
  FOR EACH RSS
    PRINTNL 		<link>
    PRINTNL 			<name>%(<NAME).%(>NAME)</name>
    PRINTNL 			<type>1</type>
    PRINTNL 			<location>%(|NAME)</location>
    PRINTNL 		</link>
  ENDFOR
  PRINTNL 	</linkedResources>
  PRINTNL </projectDescription>

  IF (%(TYPE)==LIB)
    CARBIDE_TYPE = lib
    CARBIDE_PTYPE = lib
    CARBIDE_CTYPE = Lib
    IF %(CARBIDE_TOOLCHAIN)==winscw
      CARBIDE_TTYPE = SLib
      CARBIDE_TTYPE2 = SLib
    ELSE
      CARBIDE_TTYPE = Slib
      CARBIDE_TTYPE2 = Slib
    ENDIF
    CARBIDE_TTYPE3 = slib
    CARBIDE_TYPE_NAME = Static Library
  ENDIF
  IF (%(TYPE)==DLL)
    CARBIDE_TYPE = dll
    CARBIDE_PTYPE = pdll
    CARBIDE_CTYPE = PDll
    CARBIDE_TTYPE = Pdll
    CARBIDE_TTYPE2 = PDll
    CARBIDE_TTYPE3 = pdll
    CARBIDE_TYPE_NAME = Static !!!!
  ENDIF
  IF (%(TYPE)==EXE)
    CARBIDE_TYPE = exe
    CARBIDE_PTYPE = exe
    CARBIDE_CTYPE = Exe
    CARBIDE_TTYPE = Exe
    CARBIDE_TTYPE2 = Exe
    CARBIDE_TTYPE3 = exe
    CARBIDE_TYPE_NAME = Static Executable
  ENDIF

  POSTBUILDCMD = 
  IF %(INSTALL) && %(PROJECT_UID_3rd) && (%(CARBIDE_TOOLCHAIN)==winscw)
    POSTBUILDCMD = postannouncebuildStep="Install dependent files" postbuildStep="..\install.bat"
    FILE %(¯MAKEPATH)install.bat
    PRINTNL @set
    PRINTNL if not exist %EPOCROOT%Epoc32\release\%(TARGPLATFORM)\%(TARGMODE)\z\private\%(#PROJECT_UID_3rd) mkdir %EPOCROOT%Epoc32\release\%(TARGPLATFORM)\%(TARGMODE)\z\private\%(#PROJECT_UID_3rd) 
    FOR EACH INSTALL
      PRINTNL copy /b /y "%(~|NAME)" %EPOCROOT%Epoc32\release\%(TARGPLATFORM)\%(TARGMODE)\z\private\%(#PROJECT_UID_3rd)
    ENDFOR
  ENDIF

  IF !DEFINED(CARBIDE_12) && !DEFINED(CARBIDE_20)
    FILE %(¯MAKEPATH).cdtbuild
    PRINTNL <?xml version="1.0" encoding="UTF-8"?>
    PRINTNL <?fileVersion 3.0.0?>
    PRINTNL 
    PRINTNL <ManagedProjectBuildInfo>
    PRINTNL   <project id="%(NAME).com.symbian.cdt.build.projectType.%(CARBIDE_TYPE)" name="Symbian %(CARBIDE_TYPE_NAME) Project" projectType="com.symbian.cdt.build.projectType.%(CARBIDE_TYPE)">
    IF (%(TYPE)==EXE)
      PRINTNL     <configuration artifactExtension="%(CARBIDE_TYPE)" artifactName="%(PROJECT_PATH)" cleanCommand="del /s/q" description="" %(CARBIDE_ERRORPARSERS) id="%(CARBIDE_BASE).symbian.cdt.build.projectType.%(CARBIDE_PTYPE)_%(CARBIDE_TOOLCHAIN).udeb_%(CARBIDE_SDK).%(NAME)" name="%(CARBIDE_SDK_NAME) Debug [%(CARBIDE_SDK)]" parent="%(CARBIDE_BASE).symbian.cdt.build.projectType.%(CARBIDE_PTYPE)_%(CARBIDE_TOOLCHAIN).udeb_%(CARBIDE_SDK)" %(POSTBUILDCMD)>
    ELSE
      PRINTNL     <configuration artifactExtension="%(CARBIDE_TYPE)" artifactName="%(NAME)" cleanCommand="del /s/q" description="" %(CARBIDE_ERRORPARSERS) id="%(CARBIDE_BASE).symbian.cdt.build.projectType.%(CARBIDE_PTYPE)_%(CARBIDE_TOOLCHAIN).udeb_%(CARBIDE_SDK).%(NAME)" name="%(CARBIDE_SDK_NAME) Debug [%(CARBIDE_SDK)]" parent="%(CARBIDE_BASE).symbian.cdt.build.projectType.%(CARBIDE_PTYPE)_%(CARBIDE_TOOLCHAIN).udeb_%(CARBIDE_SDK)" %(POSTBUILDCMD)>
    ENDIF
    PRINTNL       <toolChain id="com.nokia.sdk.%(CARBIDE_NOKIA_SDK).build.%(CARBIDE_TOOLCHAIN_PREFIX)%(CARBIDE_TOOLCHAIN).%(CARBIDE_TOOLCHAIN)Udeb%(CARBIDE_TTYPE).%(NAME)" name="%(CARBIDE_TOOLCHAIN) UDEB Defaults" superClass="com.nokia.sdk.%(CARBIDE_NOKIA_SDK).build.%(CARBIDE_TOOLCHAIN_PREFIX)%(CARBIDE_TOOLCHAIN).%(CARBIDE_TOOLCHAIN)Udeb%(CARBIDE_TTYPE)">
    PRINTNL         <option id="com.nokia.cdt.build.toolChain.%(CARBIDE_NOKIA_SDK).%(CARBIDE_TOOLCHAIN).GlobalOptions.systemIncludePaths.%(CARBIDE_TOOLCHAIN)Udeb%(CARBIDE_TTYPE).%(NAME)" name="System Include Paths" superClass="com.nokia.cdt.build.toolChain.%(CARBIDE_NOKIA_SDK).%(CARBIDE_TOOLCHAIN).GlobalOptions.systemIncludePaths.%(CARBIDE_TOOLCHAIN)Udeb%(CARBIDE_TTYPE)" valueType="includePath">
    FOR EACH ../CARBIDE_SYSINCLUDE,SYSINCLUDE
      PRINTNL           <listOptionValue builtIn="false" value="%(|@NAME)"/>
    ENDFOR
    PRINTNL         </option>
    PRINTNL         <option id="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.userIncludePaths.%(NAME)" superClass="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.userIncludePaths" valueType="includePath">
//  PRINTNL           <listOptionValue builtIn="false" value="&quot;%(/|@CONFIG_FILE)&quot;"/>
    FOR EACH INCLUDE,SUBINCLUDE
      PRINTNL           <listOptionValue builtIn="false" value="&quot;%(|@NAME)&quot;"/>
    ENDFOR
    PRINTNL         </option>

    IF %(CARBIDE_TOOLCHAIN)==winscw
      IF (%(TYPE)==EXE)
        PRINTNL         <option id="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid2.udeb.%(NAME)" NAME="UID2" superClass="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid2.udeb" value="0x100039ce" valueType="string"/>
        IFDEF SYMBIAN90
          PRINTNL         <option id="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.udeb.%(NAME)" NAME="UID3" superClass="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.udeb" value="%(PROJECT_UID_3rd)" valueType="string"/>
        ELSE
          PRINTNL         <option id="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.udeb.%(NAME)" NAME="UID3" superClass="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.udeb" value="%(PROJECT_UID)" valueType="string"/>
        ENDIF
      ENDIF
      IF (%(TYPE)==DLL)
        PRINTNL         <option id="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid2.udeb.%(NAME)" NAME="UID2" superClass="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid2.udeb" value="0x1000008d" valueType="string"/>
        PRINTNL         <option id="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.udeb.%(NAME)" NAME="UID3" superClass="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.udeb" value="%(PROJECT_UID)" valueType="string"/>
      ENDIF

      IF %(TYPE)==lib
        PRINTNL         <option id="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.winscw.staticLibUdeb.targetType.%(NAME)" superClass="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.winscw.staticLibUdeb.targetType" value="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.winscw.staticLibUdeb.lib" valueType="enumerated"/>
      ENDIF
      IF %(TYPE)==exe
        PRINTNL         <option id="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.winscw.exeUdeb.targetType.%(NAME)" name="TargetType" superClass="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.winscw.exeUdeb.targetType" value="exe" valueType="enumerated"/>
      ENDIF
      IF %(TYPE)==dll
        PRINTNL         <option id="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.winscw.polymorphicDllUdeb.targetType.%(NAME)" superClass="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.winscw.polymorphicDllUdeb.targetType" value="exe" valueType="enumerated"/>
      ENDIF

      PRINTNL         <tool id="com.nokia.sdk.%(CARBIDE_NOKIA_SDK).build.winscw.compiler.udeb%(CARBIDE_CTYPE).%(NAME)" name="WINSCW C/C++ Compiler" superClass="com.nokia.sdk.%(CARBIDE_NOKIA_SDK).build.winscw.compiler.udeb%(CARBIDE_CTYPE)">
      PRINTNL           <option id="com.nokia.build.buildDefinitions.%(CARBIDE_NOKIA_SDK).%(CARBIDE_TOOLCHAIN).compiler.definedSymbols.udeb%(CARBIDE_CTYPE).%(NAME)" superClass="com.nokia.build.buildDefinitions.%(CARBIDE_NOKIA_SDK).%(CARBIDE_TOOLCHAIN).compiler.definedSymbols.udeb%(CARBIDE_CTYPE)" valueType="definedSymbols">
      PRINTNL             <listOptionValue builtIn="false" value="_DEBUG"/>
      IF %(TYPE)==exe
        PRINTNL             <listOptionValue builtIn="false" value="__EXE__"/>
      ENDIF
      FOR EACH ../CARBIDE_DEFINE,DEFINE
        PRINTNL             <listOptionValue builtIn="false" value="%(NAME)"/>
      ENDFOR
      PRINTNL           </option>
      PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.%(CARBIDE_TOOLCHAIN).compiler.warnings.warningsUnusedArgs.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.%(CARBIDE_TOOLCHAIN).compiler.warnings.warningsUnusedArgs" value="false" valueType="boolean"/>
      PRINTNL         </tool>
      PRINTNL         <tool id="com.symbian.cdt.build.tool.winscw.preCompiledHeader.default.%(NAME)" name="WINSCW C/C++ Precompiled Header Gen" superClass="com.symbian.cdt.build.tool.winscw.preCompiledHeader.default"/>
      PRINTNL         <tool id="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.winscw.linker.udeb%(CARBIDE_TTYPE2).%(NAME)" name="WINSCW C/C++ Linker" superClass="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.winscw.linker.udeb%(CARBIDE_TTYPE2)">

      IF (%(TYPE)!=LIB)
        PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.winscw.linker.libraries.staticlibraries.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.winscw.linker.libraries.staticlibraries" valueType="stringList">
        FOR REVERSE EACH USE
          IF !%(NOLIB) && (%(TYPE)!=DLL)
            PRINTNL             <listOptionValue builtIn="false" value="%(NAME).lib"/>
          ENDIF
        ENDFOR
        PRINTNL           </option>
        PRINTNL           <option id="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).buildDefinitions.winscw.linker.libraries.libraries.udeb%(CARBIDE_TTYPE2).%(NAME)" superClass="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).buildDefinitions.winscw.linker.libraries.libraries.udeb%(CARBIDE_TTYPE2)" valueType="stringList">
        FOR REVERSE EACH LIBS
          PRINTNL             <listOptionValue builtIn="false" value="%(<NAME).lib"/>
        ENDFOR
        FOR EACH USE
          IF !%(NOLIB) && (%(TYPE)==DLL)
            PRINTNL             <listOptionValue builtIn="false" value="%(NAME).lib"/>
          ENDIF
        ENDFOR
        FOR EACH USELIB
          PRINTNL             <listOptionValue builtIn="false" value="%(NAME:%=%_%(PLATFORMLIB).lib)"/>
        ENDFOR
        PRINTNL           </option>
      ENDIF

      IF (%(TYPE)==EXE)
        PRINTNL          <option id="com.symbian.cdt.build.buildDefinitions.winscw.linker.intelOptions.heap.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.winscw.linker.intelOptions.heap" value="0x00008000 0x01000000" valueType="string"/>
        PRINTNL          <option id="com.symbian.cdt.build.buildDefinitions.winscw.linker.intelOptions.stack.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.winscw.linker.intelOptions.stack" value="0x00008000" valueType="string"/>
      ENDIF

      PRINTNL         </tool>
      PRINTNL         <tool id="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).tool.winscw.aif.udeb.%(NAME)" name="AIF Compiler" superClass="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).tool.winscw.aif.udeb"/>
      PRINTNL         <tool id="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).tool.winscw.rcomp.udeb.%(NAME)" name="Resource Compiler" superClass="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).tool.winscw.rcomp.udeb"/>
      PRINTNL         <tool id="com.symbian.cdt.build.buildDefinitions.makedef.winscw.default.%(NAME)" name="Def File Creator for WINSCW" superClass="com.symbian.cdt.build.buildDefinitions.makedef.winscw.default"/>
      PRINTNL         <tool id="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).tool.winscw.mbm.udeb.%(NAME)" name="MBM Compiler" superClass="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).tool.winscw.mbm.udeb"/>

      IF %(TYPE)==lib
        PRINTNL         <tool id="com.nokia.sdk.%(CARBIDE_NOKIA_SDK).build.winscw.mif.udeb%(CARBIDE_CTYPE).%(NAME)" name="MIF Compiler" superClass="com.nokia.sdk.%(CARBIDE_NOKIA_SDK).build.winscw.mif.udeb%(CARBIDE_CTYPE)"/>
      ENDIF
      IF %(TYPE)==dll
        PRINTNL         <tool id="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.buildDefinitions.winscw.defFileManager.udeb%(CARBIDE_CTYPE).%(NAME)" name="Def file Manager" superClass="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.buildDefinitions.winscw.defFileManager.udeb%(CARBIDE_CTYPE)"/>
      ENDIF
      IF %(TYPE)==exe
        PRINTNL         <tool id="com.nokia.sdk.%(CARBIDE_NOKIA_SDK).build.winscw.mif.udeb%(CARBIDE_CTYPE).%(NAME)" name="MIF Compiler" superClass="com.nokia.sdk.%(CARBIDE_NOKIA_SDK).build.winscw.mif.udeb%(CARBIDE_CTYPE)"/>
      ENDIF
      
      PRINTNL         <macros/>
      PRINTNL       </toolChain>
      FOR EACH RSS
        PRINTNL       <resourceConfiguration exclude="false" id="%(CARBIDE_BASE).symbian.cdt.build.projectType.%(CARBIDE_TYPE)_%(CARBIDE_TOOLCHAIN).udeb_%(CARBIDE_SDK).%(../NAME).%(NAME)" name="%(<NAME).%(>NAME)" rcbsApplicability="disable" resourcePath="/%(../NAME)/%(<NAME).%(>NAME)" toolsToInvoke="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).tool.%(CARBIDE_TOOLCHAIN).rcomp.udeb.%(../NAME).%(NAME)">
        PRINTNL         <tool id="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).tool.%(CARBIDE_TOOLCHAIN).rcomp.udeb.%(../NAME).%(NAME)" name="Resource Compiler" superClass="com.symbian.cdt.build.%(CARBIDE_CDT_SDK).tool.%(CARBIDE_TOOLCHAIN).rcomp.udeb.%(../NAME)">
        IFDEF SYMBIAN90
          PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.rcomp.general.targetPath.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.rcomp.general.targetPath"       
          IF %(<NAME)==%(PROJECT_PATH)_reg
            IF %(CARBIDE_TOOLCHAIN)==winscw
              PRINTNL value="\private\10003a3f\apps"
            ELSE
              PRINTNL value="\private\10003a3f\import\apps"
            ENDIF
          ELSE
            PRINTNL value="\resource\apps" 
          ENDIF   
          PRINTNL valueType="string"/>
        ENDIF
        PRINTNL         </tool>
        PRINTNL       </resourceConfiguration>
      ENDFOR
    ENDIF

    IF %(CARBIDE_TOOLCHAIN)==gcce

      IF (%(TYPE)==EXE)
        PRINTNL       <option id="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid2.%(CARBIDE_TYPE).%(NAME)" NAME="UID2" superClass="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid2.%(CARBIDE_TYPE)" value="0x100039ce" valueType="string"/>
        IFDEF SYMBIAN90
          PRINTNL       <option id="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.%(CARBIDE_TYPE).%(NAME)" NAME="UID3" superClass="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.%(CARBIDE_TYPE)" value="%(PROJECT_UID_3rd)" valueType="string"/>
        ELSE
          PRINTNL       <option id="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.%(CARBIDE_TYPE).%(NAME)" NAME="UID3" superClass="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.%(CARBIDE_TYPE)" value="%(PROJECT_UID)" valueType="string"/>
        ENDIF
      ENDIF
      IF (%(TYPE)==DLL)
        PRINTNL         <option id="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid2.%(CARBIDE_TYPE).%(NAME)" NAME="UID2" superClass="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid2.%(CARBIDE_TYPE)" value="0x1000008d" valueType="string"/>
        PRINTNL         <option id="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.%(CARBIDE_TYPE).%(NAME)" NAME="UID3" superClass="com.symbian.cdt.build.toolChain.%(CARBIDE_TOOLCHAIN).GlobalOptions.uid3.%(CARBIDE_TYPE)" value="%(PROJECT_UID)" valueType="string"/>
      ENDIF

      PRINTNL         <option id="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.gcce.targetType.%(CARBIDE_TTYPE3).%(NAME)" superClass="com.symbian.cdt.sdk.%(CARBIDE_CDT_SDK).build.gcce.targetType.%(CARBIDE_TTYPE3)" value="%(CARBIDE_TYPE)" valueType="enumerated"/>
      PRINTNL         <tool id="com.nokia.cdt.build.tool.%(CARBIDE_NOKIA_SDK).gcce.compiler.%(CARBIDE_TTYPE3)Udeb.%(NAME)" name="GCCE Compiler" superClass="com.nokia.cdt.build.tool.%(CARBIDE_NOKIA_SDK).gcce.compiler.%(CARBIDE_TTYPE3)Udeb">
      PRINTNL           <option id="com.nokia.cdt.build.buildDefinitions.%(CARBIDE_NOKIA_SDK).gcce.compiler.defineSymbols.gcceUdeb%(CARBIDE_TTYPE).common" superClass="com.nokia.cdt.build.buildDefinitions.%(CARBIDE_NOKIA_SDK).gcce.compiler.defineSymbols.gcceUdeb%(CARBIDE_TTYPE)" valueType="definedSymbols">
      PRINTNL             <listOptionValue builtIn="false" value="_DEBUG"/>
      IF %(TYPE)==exe
        PRINTNL             <listOptionValue builtIn="false" value="__EXE__"/>
      ENDIF
      FOR EACH ../CARBIDE_DEFINE,DEFINE
        PRINTNL             <listOptionValue builtIn="false" value="%(NAME)"/>
      ENDFOR
      PRINTNL           </option>
      PRINTNL         </tool>
      PRINTNL         <tool id="com.symbian.cdt.build.tool.gcce.aif.%(CARBIDE_TTYPE3).%(NAME)" name="AIF Compiler" superClass="com.symbian.cdt.build.tool.gcce.aif.%(CARBIDE_TTYPE3)"/>
      PRINTNL         <tool id="com.symbian.cdt.build.tool.gcce.rcomp.%(CARBIDE_TTYPE3).%(NAME)" name="Resource Compiler" superClass="com.symbian.cdt.build.tool.gcce.rcomp.%(CARBIDE_TTYPE3)"/>
      PRINTNL         <tool id="com.symbian.cdt.build.tool.gcce.mbm.%(CARBIDE_TTYPE3).%(NAME)" name="MBM Compiler" superClass="com.symbian.cdt.build.tool.gcce.mbm.%(CARBIDE_TTYPE3)"/>
      IF DEFINED(SYMBIAN90) && %(SYMBIAN_CERT) && %(SYMBIAN_KEY)
        PRINTNL         <tool id="com.symbian.cdt.build.tool.gcce.createsis.%(CARBIDE_TTYPE3).%(NAME)" name="CreateSis (Installation File Generator)" superClass="com.symbian.cdt.build.tool.gcce.createsis.%(CARBIDE_TTYPE3)">
        IF %(SYMBIAN_PASS)
          PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.createsis.general.passPhrase.1552935510" superClass="com.symbian.cdt.build.buildDefinitions.createsis.general.passPhrase" value="%(SYMBIAN_PASS)" valueType="string"/>
        ENDIF
        PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.createsis.general.certificate.1637248733" superClass="com.symbian.cdt.build.buildDefinitions.createsis.general.certificate" value="%(~|SYMBIAN_CERT)" valueType="string"/>
        PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.createsis.general.key.1339668517" superClass="com.symbian.cdt.build.buildDefinitions.createsis.general.key" value="%(~|SYMBIAN_KEY)" valueType="string"/>
        PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.createsis.general.outputName.2020124418" superClass="com.symbian.cdt.build.buildDefinitions.createsis.general.outputName" value="%(~|OUTPUTPATH)%(PROJECT_PATH).sis" valueType="string"/>
        PRINTNL         </tool>
      ELSE
        PRINTNL         <tool id="com.symbian.cdt.build.tool.gcce.createsis.%(CARBIDE_TTYPE3).%(NAME)" name="CreateSis (Installation File Generator)" superClass="com.symbian.cdt.build.tool.gcce.createsis.%(CARBIDE_TTYPE3)"/>
      ENDIF

      IF (%(TYPE)!=LIB)
        PRINTNL         <tool id="com.symbian.cdt.build.tool.gcce.linker.%(CARBIDE_TTYPE3)Udeb.%(NAME)" name="GCCE Linker" superClass="com.symbian.cdt.build.tool.gcce.linker.%(CARBIDE_TTYPE3)Udeb">
/* buggy Carbide
        PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.gcce.linker.libraries.staticlibraries.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.gcce.linker.libraries.staticlibraries" valueType="stringList">
        FOR REVERSE EACH USE
          IF !%(NOLIB) && (%(TYPE)!=DLL)
            PRINTNL             <listOptionValue builtIn="false" value="%(NAME).lib"/>
          ENDIF
        ENDFOR
        PRINTNL           </option>
*/
        PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.gcce.linker.libraries.libraries.%(CARBIDE_TTYPE3)Udeb.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.gcce.linker.libraries.libraries.%(CARBIDE_TTYPE3)Udeb" valueType="stringList">
        FOR REVERSE EACH LIBS
          PRINTNL             <listOptionValue builtIn="false" value="&quot;%(<NAME).dso&quot;"/>
        ENDFOR
        FOR EACH USE
          IF !%(NOLIB) && (%(TYPE)==DLL)
            PRINTNL             <listOptionValue builtIn="false" value="&quot;%(NAME).dso&quot;"/>
          ENDIF
          IF !%(NOLIB) && (%(TYPE)!=DLL)
            PRINTNL             <listOptionValue builtIn="false" value="&quot;${EPOCROOT}epoc32\release\ARMV5\UDEB\%(NAME).lib&quot;"/>
          ENDIF
        ENDFOR
        FOR EACH USELIB
          PRINTNL             <listOptionValue builtIn="false" value="&quot;${EPOCROOT}epoc32\release\ARMV5\UREL\%(NAME).lib&quot;"/>
        ENDFOR
        PRINTNL           </option>
        PRINTNL         </tool>
        PRINTNL         <tool id="com.symbian.cdt.build.tool.postLinker.%(CARBIDE_TTYPE3)Udeb.%(NAME)" name="Post linker" superClass="com.symbian.cdt.build.tool.postLinker.%(CARBIDE_TTYPE3)Udeb">
        PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.postLinker.heap.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.postLinker.heap" value="0x00008000 0x01000000" valueType="string"/>
        PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.postLinker.stack.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.postLinker.stack" value="0x00008000" valueType="string"/>
        PRINTNL         </tool>
      ELSE
        PRINTNL         <tool id="com.symbian.cdt.build.tool.gcce.archiver.%(CARBIDE_TTYPE3).%(NAME)" name="Archiver" superClass="com.symbian.cdt.build.tool.gcce.archiver.%(CARBIDE_TTYPE3)"/>
      ENDIF

      PRINTNL         <tool id="com.nokia.cdt.build.tool.%(CARBIDE_NOKIA_SDK).gcce.mif.%(CARBIDE_TTYPE3)Udeb.%(NAME)" name="MIF Compiler" superClass="com.nokia.cdt.build.tool.%(CARBIDE_NOKIA_SDK).gcce.mif.%(CARBIDE_TTYPE3)Udeb"/>

      PRINTNL         <macros/>
      PRINTNL       </toolChain>

      FOR EACH SOURCE
        IF %(>NAME)==s
          PRINTNL       <resourceConfiguration exclude="false" id="%(CARBIDE_BASE).symbian.cdt.build.projectType.%(CARBIDE_TYPE)_%(CARBIDE_TOOLCHAIN).udeb_%(CARBIDE_SDK).%(../NAME).%(NAME)" name="%(<NAME).%(>NAME)" rcbsApplicability="override" resourcePath="/%(../NAME)/%(<NAME).%(>NAME)" toolsToInvoke="">
          PRINTNL         <tool announcement="Invoking: GCCE Compiler" command="arm-none-symbianelf-g++ -c &quot;%(|NAME)&quot; -o %(<NAME).o" customBuildStep="true" id="org.eclipse.cdt.managedbuilder.ui.rcbs.%(NAME)" name="Resource Custom Build Step">
          PRINTNL           <inputType id="org.eclipse.cdt.managedbuilder.ui.rcbs.inputtype.%(NAME)" name="Resource Custom Build Step Input Type">
          PRINTNL             <additionalInput kind="additionalinputdependency" paths=""/>
          PRINTNL           </inputType>
          PRINTNL           <outputType id="org.eclipse.cdt.managedbuilder.ui.rcbs.outputtype.%(NAME)" name="Resource Custom Build Step Output Type" outputNames="%(<NAME).o"/>
          PRINTNL         </tool>
          PRINTNL       </resourceConfiguration>
        ENDIF
        IF %(>NAME)==c
          PRINTNL       <resourceConfiguration exclude="false" id="%(CARBIDE_BASE).symbian.cdt.build.projectType.%(CARBIDE_TYPE)_%(CARBIDE_TOOLCHAIN).udeb_%(CARBIDE_SDK).%(../NAME).%(NAME)" name="%(<NAME).%(>NAME)" rcbsApplicability="disable" resourcePath="/%(../NAME)/%(<NAME).%(>NAME)" toolsToInvoke="com.nokia.cdt.build.tool.%(CARBIDE_NOKIA_SDK).%(CARBIDE_TOOLCHAIN).compiler.%(CARBIDE_TTYPE3)Udeb.%(../NAME).%(NAME)">
          PRINTNL         <tool id="com.nokia.cdt.build.tool.%(CARBIDE_NOKIA_SDK).%(CARBIDE_TOOLCHAIN).compiler.%(CARBIDE_TTYPE3)Udeb.%(../NAME).%(NAME)" name="GCCE Compiler" superClass="com.nokia.cdt.build.tool.%(CARBIDE_NOKIA_SDK).%(CARBIDE_TOOLCHAIN).compiler.%(CARBIDE_TTYPE3)Udeb.%(../NAME)">
          PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.%(CARBIDE_TOOLCHAIN).compiler.general.additionalOptions.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.%(CARBIDE_TOOLCHAIN).compiler.general.additionalOptions" value="-x c" valueType="string"/>
          PRINTNL         </tool>
          PRINTNL       </resourceConfiguration>
        ENDIF
      ENDFOR

      FOR EACH RSS
        PRINTNL       <resourceConfiguration exclude="false" id="%(CARBIDE_BASE).symbian.cdt.build.projectType.%(CARBIDE_TYPE)_%(CARBIDE_TOOLCHAIN).udeb_%(CARBIDE_SDK).%(../NAME).%(NAME)" name="%(<NAME).%(>NAME)" rcbsApplicability="disable" resourcePath="/%(../NAME)/%(<NAME).%(>NAME)" toolsToInvoke="com.symbian.cdt.build.tool.%(CARBIDE_TOOLCHAIN).rcomp.%(CARBIDE_TTYPE3).%(../NAME).%(NAME)">
        PRINTNL         <tool id="com.symbian.cdt.build.tool.%(CARBIDE_TOOLCHAIN).rcomp.%(CARBIDE_TTYPE3).%(../NAME).%(NAME)" name="Resource Compiler" superClass="com.symbian.cdt.build.tool.%(CARBIDE_TOOLCHAIN).rcomp.%(CARBIDE_TTYPE3).%(../NAME)">
        IFDEF SYMBIAN90
          PRINTNL           <option id="com.symbian.cdt.build.buildDefinitions.rcomp.general.targetPath.%(NAME)" superClass="com.symbian.cdt.build.buildDefinitions.rcomp.general.targetPath"       
          IF %(<NAME)==%(PROJECT_PATH)_reg
            IF %(CARBIDE_TOOLCHAIN)==winscw
              PRINTNL value="\private\10003a3f\apps"
            ELSE
              PRINTNL value="\private\10003a3f\import\apps"
            ENDIF
          ELSE
            PRINTNL value="\resource\apps" 
          ENDIF   
          PRINTNL valueType="string"/>
        ENDIF
        PRINTNL         </tool>
        PRINTNL       </resourceConfiguration>
      ENDFOR
    ENDIF

    PRINTNL     </configuration>

    PRINTNL     <macros/>
    PRINTNL   </project>
    PRINTNL </ManagedProjectBuildInfo>
  ENDIF

  FILE %(¯MAKEPATH)bld.inf
  PRINTNL PRJ_PLATFORMS 
  PRINTNL %(TARGPLATFORM)
  PRINTNL 
  PRINTNL PRJ_MMPFILES
  IF %(TYPE)==exe
    IF defined(SYMBIAN90) && defined(SERIES60) && %(ICON)!=""
      PRINTNL gnumakefile %(¯~!PATH)%(PROJECT_PATH)_icon.mak
    ENDIF
  ENDIF
  PRINTNL %(NAME).mmp

  IF %(CARBIDE_TOOLCHAIN)==winscw
    CONFIG_STRING = Emulator Debug (WINSCW) [%(CARBIDE_SDK)]
  ELIF %(TARGMODE)=="UDEB"
    CONFIG_STRING = Phone Debug (%(TARGPLATFORM)) [%(CARBIDE_SDK)]
  ELSE
    CONFIG_STRING = Phone Release (%(TARGPLATFORM)) [%(CARBIDE_SDK)]
  ENDIF
  IF DEFINED(CARBIDE_20)
    FILE %(¯MAKEPATH).cproject
    PRINTNL <?xml version="1.0" encoding="UTF-8"?>
    PRINTNL <?fileVersion 4.0.0?>
    PRINTNL 
    PRINTNL <cproject>
    PRINTNL   <storageModule buildFromInf="true" buildingTestComps="true" cleanLevel="0" defaultMMPChangedAction="0" infBuildComponents="" infFileLocation="bld.inf" macrosFile="" makeEngineToUse="make" manageDependencies="true" moduleId="com.nokia.carbide.cdt.builder.carbideCPPBuilder" overrideMakeEngine="false" overrideWorkspaceSettings="false" promptForMMPChangedAction="false" useDebugMode="false" useIncrementalBuilder="false" useKeepGoing="false" useMMPMacros="true"/>
    PRINTNL   <storageModule moduleId="org.eclipse.cdt.core.settings">
    PRINTNL     <cconfiguration id="%(CONFIG_STRING)">
    PRINTNL       <storageModule buildSystemId="com.nokia.carbide.cdt.builder.CarbideConfigurationDataProvider" id="%(CONFIG_STRING)" moduleId="org.eclipse.cdt.core.settings" name="%(CONFIG_STRING)">
    PRINTNL         <externalSettings/>
    PRINTNL         <extensions>
    PRINTNL           <extension id="org.eclipse.cdt.core.PE" point="org.eclipse.cdt.core.BinaryParser"/>
    PRINTNL           <extension id="org.eclipse.cdt.core.ELF" point="org.eclipse.cdt.core.BinaryParser"/>
    PRINTNL           <extension id="com.nokia.cdt.debug.cw.symbian.SymbianE32" point="org.eclipse.cdt.core.BinaryParser"/>
    PRINTNL           <extension id="com.nokia.carbide.cdt.builder.MakeDefErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
    PRINTNL           <extension id="com.nokia.carbide.cdt.builder.SBSv2ErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
    PRINTNL           <extension id="com.nokia.carbide.cdt.builder.GCCErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
    PRINTNL           <extension id="com.nokia.carbide.cdt.builder.MWLDErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
    PRINTNL           <extension id="com.nokia.carbide.cdt.builder.MWCCErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
    PRINTNL           <extension id="com.nokia.carbide.cdt.builder.CarbideMakeErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
    PRINTNL           <extension id="com.nokia.carbide.cdt.builder.RCOMPErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
    PRINTNL           <extension id="com.nokia.carbide.cdt.builder.Elf2E32ErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
    PRINTNL           <extension id="com.nokia.carbide.cdt.builder.MakmakeErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
    PRINTNL         </extensions>
    PRINTNL       </storageModule>
    PRINTNL       <storageModule moduleId="CarbideConfigurationDataProvider">
    IF (%(TYPE)==EXE) && %(CARBIDE_TOOLCHAIN)!=winscw
      IF DEFINED(SYMBIAN90) && %(SYMBIAN_CERT) && %(SYMBIAN_KEY)
        PRINTNL         <SIS_BUILDER_DATA_ID CERTIFICATESTORAGE="%(~|SYMBIAN_CERT)" CREATESTUBSTORAGE="false" ENABLEDSTORAGE="true" KEYSTORAGE="%(~|SYMBIAN_KEY)" OUTPUTFILENAMESTORAGE="%(~|BUILDPATH)..\%(CARBIDE_SUBDIR)\%(~PATH)%(PROJECT_PATH).sis" PARTIALUPGRADESTORAGE="false" PASSWORDSTORAGE="%(SYMBIAN_PASS)" PKGFILESTORAGE="%(PROJECT_PATH).pkg" SIGNEDSISFILENAMESTORAGE="%(~|OUTPUTPATH)%(PROJECT_NAME).sis" SIGNINGTYPESTORAGE="2"/>
      ELSE
        PRINTNL         <SIS_BUILDER_DATA_ID CREATESTUBSTORAGE="false" ENABLEDSTORAGE="true" PARTIALUPGRADESTORAGE="false" PKGFILESTORAGE="%(PROJECT_PATH).pkg" OUTPUTFILENAMESTORAGE="%(~|OUTPUTPATH)%(PROJECT_NAME).sis" SIGNINGTYPESTORAGE="1"/>
      ENDIF
    ELSE
      PRINTNL         <SIS_BUILDER_DATA_ID CREATESTUBSTORAGE="false" ENABLEDSTORAGE="true" PARTIALUPGRADESTORAGE="false" SIGNINGTYPESTORAGE="1"/>
    ENDIF
    PRINTNL         <ENV_VAR_DATA_ID/>
    PRINTNL         <ARGUMENTS_DATA_ID ABLDBUILDARGSSTORAGE="-v" ABLDCLEANARGSSTORAGE="-v" ABLDFREEZEARGSSTORAGE="-v"/>
    PRINTNL         <ROM_BUILDER_DATA_ID ROMBUILDWORKINGDIRECTORYSTORAGE="C:\"/>
    PRINTNL       </storageModule>
    PRINTNL       <storageModule moduleId="org.eclipse.cdt.core.language.mapping"/>
    PRINTNL       <storageModule moduleId="org.eclipse.cdt.core.externalSettings"/>
    PRINTNL     </cconfiguration>
    PRINTNL   </storageModule>
    PRINTNL   <storageModule moduleId="com.nokia.carbide.cdt.rombuilder"/>
    PRINTNL </cproject>

    FILE %(¯MAKEPATH).settings/org.eclipse.cdt.ui.prefs
    PRINTNL eclipse.preferences.version=1
    PRINTNL formatter_profile=org.eclipse.cdt.ui.default.allman_profile
    PRINTNL formatter_settings_version=1

  ELSE
    FILE %(¯MAKEPATH).cdtproject
    PRINTNL <?xml version="1.0" encoding="UTF-8"?>
    PRINTNL <?eclipse-cdt version="2.0"?>
    PRINTNL 
    IF DEFINED(CARBIDE_12)
      PRINTNL <cdtproject id="com.nokia.carbide.cpp.project.core.carbidecppproject">
      PRINTNL   <extension id="com.nokia.carbide.cdt.builder" point="com.nokia.carbide.cdt.builder.carbideCPPBuilder">
      PRINTNL     <attribute key="infFileLocation" value="bld.inf"/>
      PRINTNL     <attribute key="defaultBuildConfig" value="%(CONFIG_STRING)"/>
      PRINTNL     <attribute key="buildFromInf" value="true"/>
      PRINTNL     <attribute key="verboseFlag" value="true"/>
      PRINTNL     <attribute key="debugTargetMMP" value="%(NAME).mmp"/>
      PRINTNL     <attribute key="infBuildComponents" value=""/>
      PRINTNL     <attribute key="cleanCommand" value="clean"/>
      PRINTNL     <attribute key="buildingTestComps" value="true"/>
      PRINTNL   </extension>
      PRINTNL   <extension id="com.nokia.carbide.cpp.project.core.CarbideCPPScannerProvider" point="org.eclipse.cdt.core.ScannerInfoProvider"/>
      PRINTNL   <extension id="com.nokia.cdt.debug.cw.symbian.SymbianE32" point="org.eclipse.cdt.core.BinaryParser"/>
      PRINTNL   <extension id="org.eclipse.cdt.core.PE" point="org.eclipse.cdt.core.BinaryParser"/>
      PRINTNL   <extension id="org.eclipse.cdt.core.ELF" point="org.eclipse.cdt.core.BinaryParser"/>
      PRINTNL   <data>
      PRINTNL     <item id="org.eclipse.cdt.core.pathentry">
      PRINTNL       <pathentry kind="src" path=""/>
//    PRINTNL       <pathentry kind="out" path=""/>
      PRINTNL       <pathentry kind="con" path="com.nokia.carbide.cpp.project.core.CARBIDE_CPP_PATH_INFO"/>
/*
      FOR EACH USE
        IF !%(NOLIB) && (%(TYPE)!=DLL)
          PRINTNL       <pathentry kind="lib" library="epoc32/release/ARMV5/UDEB/%(NAME).lib" path="%EPOCROOT%"/>
        ENDIF
      ENDFOR
      FOR EACH USELIB
        PRINTNL       <pathentry kind="lib" library="%EPOCROOT%epoc32/release/ARMV5/UREL/%(NAME:%=%_%(PLATFORMLIB).lib)" path=""/>
      ENDFOR
*/
      FOR EACH USE
        PRINTNL       <pathentry kind="prj" path="/%(NAME)"/>
      ENDFOR
      PRINTNL     </item>
      PRINTNL   </data>
      PRINTNL </cdtproject>
    ELSE
      PRINTNL <cdtproject id="org.eclipse.cdt.managedbuilder.core.managedMake">
      PRINTNL   <extension id="org.eclipse.cdt.managedbuilder.core.ManagedBuildManager" point="org.eclipse.cdt.core.ScannerInfoProvider"/>
      PRINTNL   <extension id="org.eclipse.cdt.core.PE" point="org.eclipse.cdt.core.BinaryParser"/>
      PRINTNL   <extension id="org.eclipse.cdt.core.ELF" point="org.eclipse.cdt.core.BinaryParser"/>
      PRINTNL   <extension id="com.nokia.cdt.debug.cw.symbian.SymbianE32" point="org.eclipse.cdt.core.BinaryParser"/>
      PRINTNL   <extension id="org.eclipse.cdt.core.ctagsindexer" point="org.eclipse.cdt.core.CIndexer">
      PRINTNL     <attribute key="ctagslocation" value=""/>
      PRINTNL     <attribute key="ctagfiletype" value="ctags_internal"/>
      PRINTNL     <attribute key="ctagsindexincludes" value="false"/>
      PRINTNL     <attribute key="ctagfilelocation" value=""/>
      PRINTNL     <attribute key="ctagslocationtype" value="ctags_path_default"/>
      PRINTNL   </extension>
      PRINTNL   <data>
      PRINTNL     <item id="org.eclipse.cdt.core.pathentry">
      PRINTNL       <pathentry kind="src" path=""/>
      PRINTNL       <pathentry kind="out" path=""/>
      PRINTNL       <pathentry kind="con" path="org.eclipse.cdt.managedbuilder.MANAGED_CONTAINER"/>
      PRINTNL     </item>
      PRINTNL   </data>
      PRINTNL </cdtproject>
    ENDIF

    FILE %(¯MAKEPATH).settings/.carbide_build_settings
    PRINTNL <?xml version="1.0" encoding="UTF-8"?>
    PRINTNL <CarbideBuilderConfigInfo version="1">
    PRINTNL   <configuration name="%(CONFIG_STRING)">
    IF %(CARBIDE_TOOLCHAIN)==winscw
      IF (%(TYPE)==EXE)
        PRINTNL     <sisBuilder additionalOptions="" cert="" contentSearchLocation="" key="" outputFileName="" password="" pkgFile="(none)" signedFileName=""/>
      ENDIF
    ELSE
      IF (%(TYPE)==EXE)
        IF DEFINED(SYMBIAN90) && %(SYMBIAN_CERT) && %(SYMBIAN_KEY)
          PRINTNL     <sisBuilder additionalOptions="" cert="%(~|SYMBIAN_CERT)" contentSearchLocation="" key="%(~|SYMBIAN_KEY)" outputFileName="%(~|BUILDPATH)..\%(CARBIDE_SUBDIR)\%(~PATH)%(PROJECT_PATH).sis" password="%(SYMBIAN_PASS)" pkgFile="%(PROJECT_PATH).pkg" signedFileName="%(~|OUTPUTPATH)%(PROJECT_NAME).sis"/>
        ELSE
          PRINTNL     <sisBuilder additionalOptions="" cert="" contentSearchLocation="" key="" outputFileName="%(~|OUTPUTPATH)%(PROJECT_PATH).sis" password="" pkgFile="%(PROJECT_PATH).pkg" signedFileName=""/>
        ENDIF
      ENDIF
    ENDIF
    PRINTNL     <envVars/>
    PRINTNL   </configuration>
    PRINTNL </CarbideBuilderConfigInfo>
  ENDIF

  IF %(TYPE)==exe && %(CARBIDE_TOOLCHAIN)!=winscw
    IF %(ICON)!=""
      FILE %(¯MAKEPATH)%(PROJECT_PATH).mifdef
      PRINTNL c32|%(|ICON)_s60.svg
    ENDIF

    TARGEPOC = $(EPOCROOT)
    TARGPLATFORM = $(PLATFORM)
    TARGETMODE = $(TARGET)
    FILE %(¯MAKEPATH)%(PROJECT_PATH).pkg
    FILEABS
    #include "symbian_pkg.inc"
  ENDIF

ENDFOR
