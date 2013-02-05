#define TARGET_OSX

ASMFLAGS = -f macho -D_MACHO
PLATFORMLIB = osx_ppc

#define OUTPUT_EXE %(NAME)
#define OUTPUT_DLL %(NAME).so
#define OUTPUT_LIB lib%(NAME).a
#define OUTPUT_CON %(NAME)

#define COMPILER_XCODE
#define COMPILER_GCC

IFDEF USE_MAKEFILE
#include "gcc_osx.inc"
ELSE
  CONFIG
ENDIF

IFDEF TARGET_IPHONE
  ICONSUFFIX = _iphone2.png
ELSE
  ICONSUFFIX = .icns
ENDIF

FOR EACH WORKSPACE

  FOR EACH USE
    IF %(TYPE)==EXE || %(TYPE)==DLL
      IF (%(TYPE)==DLL)
        DEFINE += %(<^NAME)_EXPORTS
      ENDIF
      FILE %(¯../PATH)%(NAME).plist
      #include "osx_plist.inc"
    ENDIF

    IF %(PMDOC)!=""
      COMPILE %(PMDOC).pmdoc/index.xml %(PATH)%(NAME).pmdoc/index.xml
      COMPILE %(PMDOC).pmdoc/01%(PROJECT_NAME).xml %(PATH)%(NAME).pmdoc/01%(PROJECT_NAME).xml
      COMPILE %(PMDOC).pmdoc/01%(PROJECT_NAME)-contents.xml %(PATH)%(NAME).pmdoc/01%(PROJECT_NAME)-contents.xml
    ENDIF
  ENDFOR

  HAS_PRIVATE_FRAMEWORK = 
  
  FILE %(¯PATH)%(NAME).xcodeproj/project.pbxproj
  PRINTNL // !$*UTF8*$!
  PRINTNL {
  PRINTNL 	archiveVersion = 1;
  PRINTNL 	classes = {
  PRINTNL 	};
  IF "%(XCODECOMPATVERSION)"=="2.4"
    PRINTNL 	objectVersion = 42;
  ELSE
    PRINTNL	objectVersion = 45;
  ENDIF
  PRINTNL 	objects = {
  PRINTNL 
  PRINTNL /* Begin PBXBuildFile section */
  FOR EACH USE
    HAS_RESOURCE =
    HAS_REZ =
    FOR EACH SOURCE
      IF (%(>NAME)==r)
        PRINTNL 		%(_XCODEUID) /* %(!;NAME) in Rez */ = {isa = PBXBuildFile; fileRef = %(XCODEUID) /* %(!;NAME) */; };
        ../HAS_REZ = 1
      ELIF (%(>NAME)==nib)
        PRINTNL 		%(_XCODEUID) /* %(!;NAME) in Resources */ = {isa = PBXBuildFile; fileRef = %(XCODEUID) /* %(!;NAME) */; };
        ../HAS_RESOURCE = 1
      ELSE
        PRINTNL 		%(_XCODEUID) /* %(!;NAME) in Sources */ = {isa = PBXBuildFile; fileRef = %(XCODEUID) /* %(!;NAME) */; };
      ENDIF
    ENDFOR
    IF (%(TYPE)==EXE) || (%(TYPE)==DLL)
      PRINTNL 		%(_XCODEGRPUID10) /* %(!;NAME).plist in Resources */ = {isa = PBXBuildFile; fileRef = %(XCODEGRPUID9) /* %(!;NAME).plist */; };
    ENDIF
    FOR EACH OSX_ICON
      PRINTNL 		%(_XCODEUID) /* %(!;NAME) in Resources */ = {isa = PBXBuildFile; fileRef = %(XCODEUID) /* %(!;NAME) */; };
    ENDFOR
    FOR EACH ICON
      PRINTNL 		%(_XCODEUID) /* %(!;NAME)%(ICONSUFFIX) in Resources */ = {isa = PBXBuildFile; fileRef = %(XCODEUID) /* %(!;NAME)%(ICONSUFFIX) */; };
    ENDFOR
    FOR EACH INSTALL
      PRINTNL 		%(_XCODEUID) /* %(!;NAME) in Resources */ = {isa = PBXBuildFile; fileRef = %(XCODEUID) /* %(!;NAME) */; };
      ../HAS_RESOURCE = 1
    ENDFOR
    FOR EACH OSX_STRINGS
      PRINTNL 		%(_XCODEUID) /* InfoPlist.strings in Resources */ = {isa = PBXBuildFile; fileRef = %(XCODEUID) /* InfoPlist.strings */; };
    ENDFOR
//    IF (%(TYPE)==EXE) || (%(TYPE)==CON) || (%(TYPE)==DLL)
      FOR EACH FRAMEWORK
        PRINTNL 		%(XCODEFRWFILE) /* %(NAME).framework in %(../NAME) Frameworks */ = {isa = PBXBuildFile; fileRef = %(_XCODEFRWFILE) /* %(NAME).framework */; };
      ENDFOR
      FOR EACH FRAMEWORK_LIB
        PRINTNL 		%(XCODEFRWFILE) /* %(NAME) in Frameworks */ = {isa = PBXBuildFile; fileRef = %(_XCODEFRWFILE) /* %(NAME) */; };
      ENDFOR
      FOR EACH PRIVATEFRAMEWORK
        PRINTNL 		%(XCODEFRWFILE) /* %(NAME).framework in %(../NAME) Frameworks */ = {isa = PBXBuildFile; fileRef = %(_XCODEFRWFILE) /* %(NAME).framework */; };
        ../../HAS_PRIVATE_FRAMEWORK = 1
      ENDFOR
      FOR EACH USE
        IF (%(TYPE)==LIB)
          PRINTNL 		%(*XCODEFRWFILE) /* lib%(NAME).a in %(../NAME) Frameworks */ = {isa = PBXBuildFile; fileRef = %(XCODEGRPUID3) /* lib%(NAME).a */; };
        ENDIF
        IF (%(TYPE)==DLL)
          IF %(OSX_DLL_TYPE)=="Bundle"
            PRINTNL 		%(*XCODEFRWFILE) /* %(NAME).component in %(../NAME) Frameworks */ = {isa = PBXBuildFile; fileRef = %(XCODEGRPUID3) /* %(NAME).component */; };
          ELSE
            PRINTNL 		%(*XCODEFRWFILE) /* %(NAME).dylib in %(../NAME) Frameworks */ = {isa = PBXBuildFile; fileRef = %(XCODEGRPUID3) /* %(NAME).dylib */; };
          ENDIF
        ENDIF
      ENDFOR
//    ENDIF
  ENDFOR
  PRINTNL /* End PBXBuildFile section */
  PRINTNL 
  PRINTNL /* Begin PBXContainerItemProxy section */
  FOR EACH USE
    IF (%(TYPE)==LIB)
      PRINTNL 		%(_XCODEGRPUID5) /* PBXContainerItemProxy */ = {
      PRINTNL 			isa = PBXContainerItemProxy;
      PRINTNL 			containerPortal = 91B15E841AA80083484172DE /* Project object */;
      PRINTNL 			proxyType = 1;
      PRINTNL 			remoteGlobalIDString = %(_XCODEGRPUID3);
      PRINTNL 			remoteInfo = lib%(NAME).a;
      PRINTNL 		};
    ENDIF
  ENDFOR
  PRINTNL /* End PBXContainerItemProxy section */
  PRINTNL 
  PRINTNL /* Begin PBXFileReference section */
  FOR EACH USE
    IF (%(TYPE)==CON)
      PRINTNL 		%(XCODEGRPUID3) /* %(NAME) */ = {isa = PBXFileReference; explicitFileType = "compiled.mach-o.executable"; includeInIndex = 0; sourceTree = BUILT_PRODUCTS_DIR; };
    ENDIF
    IF (%(TYPE)==LIB)
      PRINTNL 		%(XCODEGRPUID3) /* lib%(NAME).a */ = {isa = PBXFileReference; explicitFileType = archive.ar; includeInIndex = 0; name = lib%(NAME).a; path = lib%(NAME).a; sourceTree = BUILT_PRODUCTS_DIR; };
    ENDIF
    IF (%(TYPE)==EXE)
      IF %(PROJECT_PATH)
        PRINTNL 		%(XCODEGRPUID3) /* %(NAME).app */ = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = %(PROJECT_PATH).app; sourceTree = BUILT_PRODUCTS_DIR; };
      ELSE
        PRINTNL 		%(XCODEGRPUID3) /* %(NAME).app */ = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; sourceTree = BUILT_PRODUCTS_DIR; };
      ENDIF
    ENDIF
    IF (%(TYPE)==DLL)
      IF %(OSX_DLL_TYPE)=="Bundle"
        PRINTNL 		%(XCODEGRPUID3) /* %(NAME).component */ = {isa = PBXFileReference; explicitFileType = wrapper.cfbundle; includeInIndex = 0; path = %(NAME).component; sourceTree = BUILT_PRODUCTS_DIR; };
      ELSE
        PRINTNL 		%(XCODEGRPUID3) /* %(NAME).dylib */ = {isa = PBXFileReference; explicitFileType = compiled.mach-o.dylib; includeInIndex = 0; path = lib%(NAME).dylib; sourceTree = BUILT_PRODUCTS_DIR; };
      ENDIF
    ENDIF
    IF (%(TYPE)==EXE) || (%(TYPE)==DLL)
      PRINTNL 		%(XCODEGRPUID9) /* %(NAME).plist */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = text.plist; path = %(NAME).plist; sourceTree = "<group>"; };    
    ENDIF
    FOR EACH OSX_ICON
      PRINTNL 		%(XCODEUID) /* %(NAME) */ = {isa = PBXFileReference; lastKnownFileType = image.icns; name = %(NAME); path = %(!;NAME); sourceTree = "<group>"; };
    ENDFOR
    FOR EACH ICON
      PRINTNL 		%(XCODEUID) /* %(NAME)%(ICONSUFFIX) */ = {isa = PBXFileReference; lastKnownFileType = image.icns; name = %(NAME)%(ICONSUFFIX); path = %(!;NAME)%(ICONSUFFIX); sourceTree = "<group>"; };
    ENDFOR
    FOR EACH INSTALL
      PRINTNL 		%(XCODEUID) /* %(NAME) */ = {isa = PBXFileReference; lastKnownFileType = image.icns; name = %(NAME); path = %(!;NAME); sourceTree = "<group>"; };
    ENDFOR
    FOR EACH OSX_STRINGS
      PRINTNL 		%(_XCODEUID) /* %(NAME) */ = {isa = PBXFileReference; fileEncoding = 10; lastKnownFileType = text.plist.strings; name = %(NAME); path = %(!;NAME).lproj/InfoPlist.strings; sourceTree = "<group>"; };
    ENDFOR
    FOR EACH HEADER
      PRINTNL 		%(XCODEUID) /* %(!;NAME) */ = {isa = PBXFileReference; fileEncoding = 12; lastKnownFileType = sourcecode.c.h; name = %(!;NAME); path = %(!;NAME); sourceTree = "<group>"; };
    ENDFOR
    FOR EACH SOURCE
      IF (%(>NAME)==cpp)
        PRINTNL 		%(XCODEUID) /* %(!;NAME) */ = {isa = PBXFileReference; fileEncoding = 12; lastKnownFileType = sourcecode.cpp.cpp; name = %(!;NAME); path = %(!;NAME); sourceTree = "<group>"; };
      ELIF (%(>NAME)==c)
        PRINTNL 		%(XCODEUID) /* %(!;NAME) */ = {isa = PBXFileReference; fileEncoding = 12; lastKnownFileType = sourcecode.c.c; name = %(!;NAME); path = %(!;NAME); sourceTree = "<group>"; };
      ELIF (%(>NAME)==r)
        PRINTNL 		%(XCODEUID) /* %(!;NAME) */ = {isa = PBXFileReference; fileEncoding = 30; lastKnownFileType = sourcecode.rez; name = %(!;NAME); path = %(!;NAME); sourceTree = "<group>"; };
      ELIF ((%(>NAME)==m) || (%(>NAME)==rl))
        PRINTNL 		%(XCODEUID) /* %(!;NAME) */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.c.objc; name = %(!;NAME); path = %(!;NAME); sourceTree = "<group>"; };
      ELIF (%(>NAME)==mm)
        PRINTNL 		%(XCODEUID) /* %(!;NAME) */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.c.objcpp; name = %(!;NAME); path = %(!;NAME); sourceTree = "<group>"; };
      ELIF (%(>NAME)==asm)
        PRINTNL 		%(XCODEUID) /* %(!;NAME) */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.yasm; name = %(!;NAME); path = %(!;NAME); sourceTree = "<group>"; };
      ELIF (%(>NAME)==S)
        SOURCEASM = sourcecode.asm.asm
        IFDEF TARGET_IPHONE
          SOURCEASM = sourcecode.asm.arm
          IF DEFINED(CONFIG_NEON) && %(cpu)==neon
            SOURCEASM = sourcecode.asm.gaspp
          ENDIF
        ENDIF
        PRINTNL		%(XCODEUID) /* %(!;NAME) */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = %(SOURCEASM); name = %(!;NAME); path = %(!;NAME); sourceTree = "<group>"; };
      ELIF (%(>NAME)==s)
        PRINTNL 		%(XCODEUID) /* %(!;NAME) */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.asm; name = %(!;NAME); path = %(!;NAME); sourceTree = "<group>"; };
      ELIF (%(>NAME)==nib)
        PRINTNL 		%(XCODEUID) /* %(!;NAME) */ = {isa = PBXFileReference; lastKnownFileType = wrapper.nib; name = %(!;NAME); path = %(!;NAME); sourceTree = "<group>"; };
      ENDIF
    ENDFOR
    FOR EACH FRAMEWORK_LIB
      PRINTNL 		        %(_XCODEFRWFILE) /* %(NAME) */ = {isa = PBXFileReference; explicitFileType = compiled.mach-o.dylib; includeInIndex = 0; name = %(NAME); path = System/Library/Frameworks/%(NAME).framework/Versions/A/%(NAME); sourceTree = SDKROOT; };
    ENDFOR
//    IF (%(TYPE)==EXE) || (%(TYPE)==CON) || (%(TYPE)==DLL)
    FOR EACH FRAMEWORK
      IFDEF TARGET_IPHONE
        PRINTNL 		%(_XCODEFRWFILE) /* %(NAME).framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = %(NAME).framework; path = %(FRAMEWORK_PATH)System/Library/Frameworks/%(NAME).framework; sourceTree = SDKROOT; };
      ELSE
        PRINTNL 		%(_XCODEFRWFILE) /* %(NAME).framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = %(NAME).framework; path = %(FRAMEWORK_PATH)System/Library/Frameworks/%(NAME).framework; sourceTree = "<absolute>"; };
      ENDIF
    ENDFOR
    FOR EACH PRIVATEFRAMEWORK
      IFDEF TARGET_IPHONE
        PRINTNL 		%(_XCODEFRWFILE) /* %(NAME).framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = %(NAME).framework; path = %(FRAMEWORK_PATH)System/Library/PrivateFrameworks/%(NAME).framework; sourceTree = SDKROOT; };
      ELSE
        PRINTNL 		%(_XCODEFRWFILE) /* %(NAME).framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = %(NAME).framework; path = %(FRAMEWORK_PATH)System/Library/PrivateFrameworks/%(NAME).framework; sourceTree = "<absolute>"; };
      ENDIF
    ENDFOR
//    ENDIF
  ENDFOR
  PRINTNL /* End PBXFileReference section */
  PRINTNL 
  PRINTNL /* Begin PBXFrameworksBuildPhase section */
  FOR EACH USE
//    IF ((%(TYPE)==EXE) || (%(TYPE)==CON) || (%(TYPE)==DLL)) && ("%(FRAMEWORK)" || "%(PRIVATEFRAMEWORK)" || "%(USE)")
    IF ("%(FRAMEWORK)" || "%(PRIVATEFRAMEWORK)" || "%(USE)" || "%(FRAMEWORK_LIB)")
      PRINTNL 		%(_XCODEGRPUID7) /* %(NAME) Frameworks */ = {
      PRINTNL 			isa = PBXFrameworksBuildPhase;
      PRINTNL 			buildActionMask = 2147483647;
      PRINTNL 			files = (
      FOR EACH FRAMEWORK
        PRINTNL 				%(XCODEFRWFILE) /* %(NAME).framework in Frameworks */,
      ENDFOR
      FOR EACH PRIVATEFRAMEWORK
        PRINTNL 				%(XCODEFRWFILE) /* %(NAME).framework in Frameworks */,
      ENDFOR
      FOR EACH USE
        IF (%(TYPE)==LIB)
          PRINTNL 				%(*XCODEFRWFILE) /* lib%(NAME).a in Frameworks */,
        ENDIF
        IF (%(TYPE)==DLL)
          PRINTNL 				%(*XCODEFRWFILE) /* %(NAME).dylib in Frameworks */,
        ENDIF
      ENDFOR
      FOR EACH FRAMEWORK_LIB
        PRINTNL 				%(XCODEFRWFILE) /* %(NAME) in Frameworks */,
      ENDFOR
      PRINTNL 			);
      PRINTNL 			runOnlyForDeploymentPostprocessing = 0;
      PRINTNL 		};
    ENDIF
  ENDFOR
  PRINTNL /* End PBXFrameworksBuildPhase section */
  PRINTNL 
  PRINTNL /* Begin PBXGroup section */
  PRINTNL 		001B7795FE84155DC02AAC07 /* %(NAME) */ = {
  PRINTNL 			isa = PBXGroup;
  PRINTNL 			children = (
  FOR EACH USE
    PRINTNL 				%(XCODEGRPUID) /* %(NAME) */,
  ENDFOR
  PRINTNL 				00249F820B5EBE71005A4BD4 /* Products */,
  PRINTNL 			);
  PRINTNL 			name = %(NAME);
  PRINTNL 			sourceTree = "<group>";
  PRINTNL 		};
  PRINTNL 		00249F820B5EBE71005A4BD4 /* Products */ = {
  PRINTNL 			isa = PBXGroup;
  PRINTNL 			children = (
  FOR EACH USE
    IF (%(TYPE)==CON) || (%(TYPE)==EXE)
      PRINTNL 				%(XCODEGRPUID3) /* %(NAME) */,
    ENDIF
    IF (%(TYPE)==LIB)
      PRINTNL 				%(XCODEGRPUID3) /* lib%(NAME).a */,
    ENDIF
    IF (%(TYPE)==DLL)
      IF %(OSX_DLL_TYPE)=="Bundle"
        PRINTNL 				%(XCODEGRPUID3) /* %(NAME).component */,
      ELSE
        PRINTNL 				%(XCODEGRPUID3) /* %(NAME).dylib */,
      ENDIF
    ENDIF
    FOR EACH FRAMEWORK_LIB
      PRINTNL 				%(_XCODEFRWFILE) /* %(NAME) */,
    ENDFOR
  ENDFOR
  PRINTNL 			);
  PRINTNL 			name = Products;
  PRINTNL 			sourceTree = "<group>";
  PRINTNL 		};
  FOR EACH USE
    PRINTNL 		%(XCODEGRPUID) /* %(NAME) */ = {
    PRINTNL 			isa = PBXGroup;
    PRINTNL 			children = (
    IF %(SOURCE)
      PRINTNL 				%(_XCODEGRPUID) /* Sources */,
    ENDIF
    IF %(HEADER)
      PRINTNL 				%(XCODEGRPUID2) /* Headers */,
    ENDIF
    IF %(HAS_REZ) || %(HAS_RESOURCE) || %(OSX_STRINGS) || (%(TYPE)==EXE) || (%(TYPE)==DLL)
      PRINTNL 				%(_XCODEGRPUID9) /* Resources */,
    ENDIF
    IF ((%(TYPE)==EXE) || (%(TYPE)==CON) || (%(TYPE)==DLL)) && ("%(FRAMEWORK)"!="" || "%(PRIVATEFRAMEWORK)"!="")
      PRINTNL 				%(XCODEGRPUID8) /* External Frameworks and Libraries */,
    ENDIF
    PRINTNL 			);
    PRINTNL 			name = %(NAME);
    PRINTNL 			sourceTree = "<group>";
    PRINTNL 		};
    IF %(HEADER)
      PRINTNL 		%(XCODEGRPUID2) /* Headers */ = {
      PRINTNL 			isa = PBXGroup;
      PRINTNL 			children = (
      FOR EACH HEADER
        PRINTNL 				%(XCODEUID) /* %(!;NAME) */,
      ENDFOR
      PRINTNL 			);
      PRINTNL 			name = Headers;
      PRINTNL 			sourceTree = "<group>";
      PRINTNL 		};
    ENDIF
    IF %(SOURCE)
      PRINTNL 		%(_XCODEGRPUID) /* Sources */ = {
      PRINTNL 			isa = PBXGroup;
      PRINTNL 			children = (
      FOR EACH SOURCE
        IF ((%(>NAME)!=r) && (%(>NAME)!=nib))
          PRINTNL 				%(XCODEUID) /* %(!;NAME) */,
        ENDIF
      ENDFOR
      PRINTNL 			);
      PRINTNL 			name = Sources;
      PRINTNL 			sourceTree = "<group>";
      PRINTNL 		};
    ENDIF
//    IF ((%(TYPE)==EXE) || (%(TYPE)==CON) || (%(TYPE)==DLL)) && %(FRAMEWORK)
    IF %(FRAMEWORK) || %(PRIVATEFRAMEWORK)
      PRINTNL 		%(XCODEGRPUID8) /* External Frameworks and Libraries */ = {
      PRINTNL 			isa = PBXGroup;
      PRINTNL 			children = (
      FOR EACH FRAMEWORK
        PRINTNL 				%(_XCODEFRWFILE) /* %(NAME).framework */,
      ENDFOR
      FOR EACH PRIVATEFRAMEWORK
        PRINTNL 				%(_XCODEFRWFILE) /* %(NAME).framework */,
      ENDFOR
      PRINTNL 			);
      PRINTNL 			name = "External Frameworks and Libraries";
      PRINTNL 			sourceTree = "<group>";
      PRINTNL 		};
    ENDIF
    IF %(HAS_REZ) || %(HAS_RESOURCE) || %(OSX_STRINGS) || (%(TYPE)==EXE) || (%(TYPE)==DLL)
      PRINTNL 		%(_XCODEGRPUID9) /* Resources */ = {
      PRINTNL 			isa = PBXGroup;
      PRINTNL 			children = (
      FOR EACH SOURCE
        IF ((%(>NAME)==r) || (%(>NAME)==nib))
          PRINTNL 				%(XCODEUID) /* %(!;NAME) */,
        ENDIF
      ENDFOR
      IF (%(TYPE)==EXE) || (%(TYPE)==DLL)
        PRINTNL 				%(XCODEGRPUID9) /* %(NAME).plist */,
      ENDIF
      FOR EACH OSX_ICON
        PRINTNL 				%(XCODEUID) /* %(!;NAME) */,
      ENDFOR
      FOR EACH ICON
        PRINTNL 				%(XCODEUID) /* %(!;NAME)%(ICONSUFFIX)*/,
      ENDFOR
      FOR EACH INSTALL
        PRINTNL 				%(XCODEUID) /* %(!;NAME) */,
      ENDFOR
      FOR EACH OSX_STRINGS
        PRINTNL 				%(XCODEUID) /* InfoPlist.strings */,
      ENDFOR
      PRINTNL 			);
      PRINTNL 			name = Resources;
      PRINTNL 			sourceTree = "<group>";
      PRINTNL 		};
    ENDIF
  ENDFOR
  PRINTNL /* End PBXGroup section */
  PRINTNL 
  IFNDEF USE_MAKEFILE
  PRINTNL /* Begin PBXNativeTarget section */
  FOR EACH USE
    PRINTNL 		%(_XCODEGRPUID3) /* %(NAME) */ = {
    PRINTNL 			isa = PBXNativeTarget;
    PRINTNL 			buildConfigurationList = %(XCODEGRPUID5) /* Build configuration list for PBXNativeTarget "%(NAME)" */;
    PRINTNL 			buildPhases = (
    IF %(HAS_RESOURCE) || (((%(TYPE)==EXE) || (%(TYPE)==DLL)) && %(OSX_STRINGS))
      PRINTNL 				%(XCODEGRPUID7) /* Resources */,
    ENDIF
    PRINTNL 				%(_XCODEGRPUID2) /* %(NAME) Sources */,
    IF ((%(TYPE)==EXE) || (%(TYPE)==DLL) || (%(TYPE)==CON)) && ("%(FRAMEWORK)"!="" || "%(PRIVATEFRAMEWORK)"!=""|| "%(USE)"!="")
      PRINTNL 				%(_XCODEGRPUID7) /* Frameworks */,
    ENDIF
    IF %(HAS_REZ)
      PRINTNL 				%(XCODEGRPUID10) /* Rez */,
    ENDIF
    IF %(I18N) || %(EXPORT_SVN)
      PRINTNL 				%(_XCODEGRPUID6) /* Post Build */,
    ENDIF
    PRINTNL 			);
    PRINTNL 			buildRules = (
    PRINTNL 			);
    PRINTNL 			dependencies = (
    FOR EACH USE
      PRINTNL 				%(XCODEGRPUID6) /* PBXTargetDependency */,
    ENDFOR
    PRINTNL 			);
    IF (%(TYPE)==LIB)
      PRINTNL 			name = lib%(NAME).a;
      PRINTNL 			productName = lib%(NAME).a;
      PRINTNL 			productReference = %(XCODEGRPUID3) /* lib%(NAME).a */;
      PRINTNL 			productType = "com.apple.product-type.library.static";
    ELSE
      PRINTNL 			name = %(NAME);
//      IF %(PROJECT_PATH)
//        PRINTNL 			productName = %(PROJECT_PATH);
//      ELSE
        PRINTNL 			productName = %(NAME);
//      ENDIF
      IF (%(TYPE)==CON)
        PRINTNL 			productReference = %(XCODEGRPUID3) /* %(NAME) */;
        PRINTNL 			productType = "com.apple.product-type.tool";
      ENDIF
      IF (%(TYPE)==EXE)
        IF %(PROJECT_PATH)
          PRINTNL 			productReference = %(XCODEGRPUID3) /* %(PROJECT_PATH).app */;
        ELSE
          PRINTNL 			productReference = %(XCODEGRPUID3) /* %(NAME).app */;
        ENDIF
        PRINTNL 			productType = "com.apple.product-type.application";
      ENDIF
      IF (%(TYPE)==DLL)
        IF %(OSX_DLL_TYPE)=="Bundle"
          PRINTNL 			productInstallPath = "$(HOME)/Library/Bundles";
          PRINTNL 			productReference = %(XCODEGRPUID3) /* %(NAME).component */;
          PRINTNL 			productType = "com.apple.product-type.bundle";
        ELSE
          PRINTNL 			productReference = %(XCODEGRPUID3) /* lib%(NAME).dylib */;
          PRINTNL 			productType = "com.apple.product-type.library.dynamic";
        ENDIF
      ENDIF
    ENDIF
    PRINTNL 		};
  ENDFOR
  PRINTNL /* End PBXNativeTarget section */
  ELSE
  PRINTNL /* Begin PBXLegacyTarget section */
  FOR EACH USE
    PRINTNL 		%(_XCODEGRPUID3) /* %(NAME) */ = {
    PRINTNL 			isa = PBXLegacyTarget;
    PRINTNL                        buildArgumentsString = "$(ACTION)";
    PRINTNL                         buildConfigurationList = D92591D40C6B42CE00BBDBBC /* Build configuration list for PBXLegacyTarget "%(NAME)" */;
    PRINTNL                         buildPhases = (
    PRINTNL                         );
    PRINTNL                         buildToolPath = /usr/bin/make;
    PRINTNL                         dependencies = (
    PRINTNL                         );
    PRINTNL                         name = "%(NAME)";
    PRINTNL                         passBuildSettingsInEnvironment = 1;
    PRINTNL                         productName = "%(NAME)";
    PRINTNL 		};
  ENDFOR
  PRINTNL /* End PBXLegacyTarget section */
  ENDIF
  PRINTNL 
  PRINTNL /* Begin PBXProject section */
  PRINTNL 		91B15E841AA80083484172DE /* Project object */ = {
  PRINTNL 			isa = PBXProject;
  PRINTNL 			attributes = {
  PRINTNL 				BuildIndependentTargetsInParallel = YES;
  PRINTNL 			};
  PRINTNL 			buildConfigurationList = AA277BB30B5E830800EA36F7 /* Build configuration list for PBXProject "%(NAME)" */;
  PRINTNL 			compatibilityVersion = "Xcode %(XCODECOMPATVERSION)";
  PRINTNL 			hasScannedForEncodings = 0;
  PRINTNL 			mainGroup = 001B7795FE84155DC02AAC07 /* %(NAME) */;
  PRINTNL 			projectDirPath = "";
  PRINTNL 			projectRoot = "";
  PRINTNL 			targets = (
  FOR EACH USE
    PRINTNL 				%(_XCODEGRPUID3) /* %(NAME) */,
  ENDFOR
  PRINTNL 			);
  PRINTNL 		};
  PRINTNL /* End PBXProject section */
  PRINTNL 
  PRINTNL /* Begin PBXResourcesBuildPhase section */
  FOR EACH USE
    IF (%(TYPE)==EXE) || (%(TYPE)==DLL)
      PRINTNL 		%(XCODEGRPUID7) /* Resources */ = {
      PRINTNL 			isa = PBXResourcesBuildPhase;
      PRINTNL 			buildActionMask = 2147483647;
      PRINTNL 			files = (
      FOR EACH OSX_STRINGS
        PRINTNL 				%(_XCODEUID) /* InfoPlist.strings in Resources */,
      ENDFOR
//      PRINTNL 				%(_XCODEGRPUID10) /* %(NAME).plist */,
      FOR EACH SOURCE
        IF (%(>NAME)==nib)
          PRINTNL 				%(_XCODEUID) /* %(!;NAME) in Resources */,
        ENDIF
      ENDFOR
      FOR EACH OSX_ICON
        PRINTNL 				%(_XCODEUID) /* %(!;NAME) in Resources */,
      ENDFOR
      FOR EACH ICON
        PRINTNL 				%(_XCODEUID) /* %(!;NAME)%(ICONSUFFIX) in Resources */,
      ENDFOR
      FOR EACH INSTALL
        PRINTNL 				%(_XCODEUID) /* %(!;NAME) in Resources */,
      ENDFOR
      PRINTNL 			);
      PRINTNL 			runOnlyForDeploymentPostprocessing = 0;
      PRINTNL 		};
    ENDIF
  ENDFOR
  PRINTNL /* End PBXResourcesBuildPhase section */
  PRINTNL 
  PRINTNL /* Begin PBXShellScriptBuildPhase section */
  FOR EACH USE
    IF %(I18N) || %(EXPORT_SVN)
      PRINTNL 		%(_XCODEGRPUID6) /* Post Build */ = {
      PRINTNL 			isa = PBXShellScriptBuildPhase;
      PRINTNL 			buildActionMask = 2147483647;
      PRINTNL 			files = (
      PRINTNL 			);
      PRINTNL 			runOnlyForDeploymentPostprocessing = 0;
      PRINTNL 			shellPath = /bin/sh;
      CMDLINE =
      IF %(I18N)
        CMDLINE += cd %(!;PATH)\nmkdir -p \"%(I18N)\"\n
//        CMDLINE += xgettext -p %(I18N) -a -o %(NAME).pot --no-location --from-code=UTF-8\n
      ENDIF
      IF %(EXPORT_SVN)
        IF (%(TYPE)==LIB)
          CMDLINE += mkdir -p \"%(!;EXPORT_SVN)export/lib/osx/static/$CONFIGURATION\"\ncp -p \"$TARGET_BUILD_DIR/$TARGET_NAME\" \"%(!;EXPORT_SVN)export/lib/osx/static/$CONFIGURATION\"
        ELIF (%(TYPE)==DLL)
          CMDLINE += mkdir -p \"%(!;EXPORT_SVN)export/lib/osx/dynamic/$CONFIGURATION\"\ncp -p \"$TARGET_BUILD_DIR/$TARGET_NAME\" \"%(!;EXPORT_SVN)export/lib/osx/dynamic/$CONFIGURATION\"
        ENDIF
      ENDIF
      PRINTNL 			shellScript = "%(CMDLINE)";
      PRINTNL 		};
    ENDIF
  ENDFOR
  PRINTNL /* End PBXShellScriptBuildPhase section */
  PRINTNL 
  PRINTNL /* Begin PBXRezBuildPhase section */
  FOR EACH USE
    IF %(HAS_RESOURCE) || %(HAS_REZ)
      PRINTNL 		%(XCODEGRPUID10) /* Rez */ = {
      PRINTNL 			isa = PBXRezBuildPhase;
      PRINTNL 			buildActionMask = 2147483647;
      PRINTNL 			files = (
      FOR EACH SOURCE
        IF (%(>NAME)==r)
          PRINTNL 				%(_XCODEUID) /* %(!;NAME) in Rez */,
        ENDIF
      ENDFOR
      PRINTNL 			);
      PRINTNL 			runOnlyForDeploymentPostprocessing = 0;
      PRINTNL 		};
    ENDIF
  ENDFOR
  PRINTNL /* End PBXRezBuildPhase section */
  PRINTNL 
  PRINTNL /* Begin PBXSourcesBuildPhase section */
  FOR EACH USE
    PRINTNL 		%(_XCODEGRPUID2) /* %(NAME) Sources */ = {
    PRINTNL 			isa = PBXSourcesBuildPhase;
    PRINTNL 			buildActionMask = 2147483647;
    PRINTNL 			files = (
    FOR EACH SOURCE
      IF ((%(>NAME)!=r) && (%(>NAME)!=nib))
        PRINTNL 				%(_XCODEUID) /* %(!;NAME) in Sources */,
      ENDIF
    ENDFOR
    PRINTNL 			);
    PRINTNL 			runOnlyForDeploymentPostprocessing = 0;
    PRINTNL 		};
  ENDFOR
  PRINTNL /* End PBXSourcesBuildPhase section */
  PRINTNL 
  PRINTNL /* Begin PBXTargetDependency section */
  FOR EACH USE
    FOR EACH USE
      PRINTNL 		%(XCODEGRPUID6) /* PBXTargetDependency */ = {
      PRINTNL 			isa = PBXTargetDependency;
      PRINTNL 			target = %(_XCODEGRPUID3) /* lib%(NAME).a */;
      PRINTNL 			targetProxy = %(_XCODEGRPUID5) /* PBXContainerItemProxy */;
      PRINTNL 		};
    ENDFOR
  ENDFOR
  PRINTNL /* End PBXTargetDependency section */
  PRINTNL 
  PRINTNL /* Begin PBXVariantGroup section */
  FOR EACH USE
    FOR EACH OSX_STRINGS
      PRINTNL 		%(XCODEUID) /* InfoPlist.strings */ = {
      PRINTNL 			isa = PBXVariantGroup;
      PRINTNL 			children = (
      PRINTNL 				%(_XCODEUID) /* %(NAME) */,
      PRINTNL 			);
      PRINTNL 			name = InfoPlist.strings;
      PRINTNL 			sourceTree = "<group>";
      PRINTNL 		};
    ENDFOR
  ENDFOR
  PRINTNL /* End PBXVariantGroup section */
  PRINTNL 
  PRINTNL /* Begin XCBuildConfiguration section */
  FOR EACH USE
    PRINTNL 		%(_XCODEGRPUID4) /* Release */ = {
    PRINTNL 			isa = XCBuildConfiguration;
    PRINTNL 			buildSettings = {
    PRINTNL 				GCC_C_LANGUAGE_STANDARD = gnu99;
    IF (%(TYPE)!=DLL)
      PRINTNL 				GCC_DYNAMIC_NO_PIC = YES;
    ENDIF
    IF %(FAVOR_MAX_SPEED)
      PRINTNL 				GCC_OPTIMIZATION_LEVEL = 3;
      PRINTNL 				OTHER_CFLAGS = "-fomit-frame-pointer";
      IFNDEF XCODE4
        PRINTNL				OTHER_CFLAGS_ppc = "-maltivec";
        PRINTNL				OTHER_CPLUSPLUSFLAGS_ppc = "-maltivec";
      ENDIF
      PRINTNL 				OTHER_CPLUSPLUSFLAGS = "-fomit-frame-pointer";
      PRINTNL				DEPLOYMENT_POSTPROCESSING = YES;
      PRINTNL				STRIP_INSTALLED_PRODUCT = YES;
      PRINTNL				SEPARATE_STRIP = YES;
      PRINTNL				COPY_PHASE_STRIP = YES;
    ENDIF
    IFDEF TARGET_IPHONE
      PRINTNL				GCC_THUMB_SUPPORT = NO;
      IF %(IOSDEPLOYMENTTARGET)!=""
	PRINTNL				IPHONEOS_DEPLOYMENT_TARGET = %(IOSDEPLOYMENTTARGET);
      ENDIF
      PRINTNL				MACOSX_DEPLOYMENT_TARGET = "";
      IF (%(TYPE)==EXE)
        PRINTNL				"CODE_SIGN_IDENTITY[sdk=iphoneos*]" = "iPhone Developer";
        PRINTNL				IPHONE_OPTIMIZE_OPTIONS = "-skip-PNGs";
      ENDIF
    ENDIF
    IF %(ASMFLAGS)!=""
      PRINTNL 				NASM_OTHER_FLAGS = "%(ASMFLAGS)";
    ENDIF
    PRINTNL 				GCC_PREPROCESSOR_DEFINITIONS = (
    PRINTNL 					NDEBUG,
    FOR EACH DEFINE
      PRINTNL 					"%(NAME)",
    ENDFOR
    FOR EACH DEFINE_RELEASE
      PRINTNL 					"%(NAME)",
    ENDFOR
    PRINTNL 				);
    IF %(USE) || %(LIBS) || %(LIBS_RELEASE) || %(LIBINCLUDE) || %(LIBINCLUDE_RELEASE) || %(ABSLIBINCLUDE) || %(ABSLIBINCLUDE_RELEASE)
      PRINTNL 				OTHER_LDFLAGS = (
      FOR EACH LIBINCLUDE
        PRINTNL 					"-L%(!;NAME)",
      ENDFOR
      FOR EACH LIBINCLUDE_RELEASE
        PRINTNL 					"-L%(!;NAME)",
      ENDFOR
      FOR EACH ABSLIBINCLUDE
        PRINTNL 					"-L%(NAME)",
      ENDFOR
      FOR EACH ABSLIBINCLUDE_RELEASE
        PRINTNL 					"-L%(NAME)",
      ENDFOR
      IF (%(TYPE)!=LIB)
         FOR EACH USEBUILT
           PRINTNL 					"-l%(NAME)",
         ENDFOR
         FOR EACH LIBS
           PRINTNL 					"-l%(NAME)",
         ENDFOR
         FOR EACH LIBS_RELEASE
           PRINTNL 					"-l%(NAME)",
         ENDFOR
      ENDIF
      PRINTNL 				);
    ENDIF
    PRINTNL 				OBJROOT = %(!;§BUILDPATH);
    IF (%(TYPE)==EXE) && (%(PROJECT_PATH)!="")
        PRINTNL 				PRODUCT_NAME = %(PROJECT_PATH);
    ELSE
        PRINTNL 				PRODUCT_NAME = %(NAME);
    ENDIF
    IF %(INCLUDE) || %(INCLUDE_RELEASE) || %(SYSINCLUDE) || %(ABSINCLUDE_RELEASE) || %(ABSINCLUDE) || %(CONFIG_INCLUDE)
      PRINTNL 				USER_HEADER_SEARCH_PATHS = "%(!;@INCLUDE:%=% )%(!;@INCLUDE_RELEASE:%=% )%(!;@SYSINCLUDE:%=% )%(ABSINCLUDE_RELEASE:%=% )%(ABSINCLUDE:%=% )%(!;@CONFIG_INCLUDE:%=% )";
    ENDIF
    PRINTNL 				SYMROOT = %(¯!;§OUTPUTPATH);
    IF %(HAS_REZ)
      PRINTNL 				OTHER_REZFLAGS = "-d ppc_$(ppc) -d i386_$(i386)";
    ENDIF
    IF (%(TYPE)==EXE) || (%(TYPE)==DLL)
      IF %(OSX_DLL_TYPE)=="Bundle"
        PRINTNL 				WRAPPER_EXTENSION = component;
      ENDIF
      PRINTNL 				INFOPLIST_FILE = %(NAME).plist;
    ENDIF
    IF %(PRECOMPILED_HEADER)
      PRINTNL 				GCC_PREFIX_HEADER = "%(!;PATH)%(PRECOMPILED_HEADER)";
      PRINTNL 				GCC_PRECOMPILE_PREFIX_HEADER = YES;
    ENDIF
    PRINTNL 			};
    IFDEF OSX_UNIVERSAL
      PRINTNL 			name = "Release Universal";
    ELSE
      PRINTNL 			name = "Release";
    ENDIF
    PRINTNL 		};
    IFDEF OSX_UNIVERSAL
      PRINTNL 		%(XCODEGRPUID4) /* Debug other */ = {
    ELSE
      PRINTNL 		%(XCODEGRPUID4) /* Debug */ = {
    ENDIF
    PRINTNL 			isa = XCBuildConfiguration;
    PRINTNL 			buildSettings = {
    PRINTNL 				GCC_C_LANGUAGE_STANDARD = gnu99;
    IF (%(TYPE)!=DLL)
      PRINTNL 				GCC_DYNAMIC_NO_PIC = YES;
    ENDIF
    IF %(FAVOR_MAX_SPEED_DEBUG)
      PRINTNL 				GCC_OPTIMIZATION_LEVEL = 3;
      IFDEF XCODE4
        PRINTNL 				OTHER_CFLAGS = "-fomit-frame-pointer";
        PRINTNL 				OTHER_CPLUSPLUSFLAGS = "-fomit-frame-pointer";
      ELSE
        PRINTNL 				OTHER_CFLAGS = "-fomit-frame-pointer -maltivec";
        PRINTNL 				OTHER_CPLUSPLUSFLAGS = "-fomit-frame-pointer -maltivec";
      ENDIF
    ELSE
      PRINTNL 				GCC_OPTIMIZATION_LEVEL = 0;
      //PRINTNL 				GCC_ENABLE_FIX_AND_CONTINUE = YES;
    ENDIF
    IFDEF TARGET_IPHONE
        PRINTNL				GCC_THUMB_SUPPORT = NO;
        IF %(IOSDEPLOYMENTTARGET)!=""
	  PRINTNL			IPHONEOS_DEPLOYMENT_TARGET = %(IOSDEPLOYMENTTARGET);
        ENDIF
        PRINTNL				MACOSX_DEPLOYMENT_TARGET = "";
        IF (%(TYPE)==EXE)
          PRINTNL			"CODE_SIGN_IDENTITY[sdk=iphoneos*]" = "iPhone Developer";
          PRINTNL			IPHONE_OPTIMIZE_OPTIONS = "-skip-PNGs";
        ENDIF
    ENDIF
    IF %(DEFINE) || %(DEFINE_DEBUG)
      PRINTNL 				GCC_PREPROCESSOR_DEFINITIONS = (
      FOR EACH DEFINE
        PRINTNL 					"%(NAME)",
      ENDFOR
      FOR EACH DEFINE_DEBUG
        PRINTNL 					"%(NAME)",
      ENDFOR
      PRINTNL 				);
    ENDIF
    IF %(USE) || %(LIBS) || %(LIBS_DEBUG) || %(LIBINCLUDE) || %(LIBINCLUDE_DEBUG) || %(ABSLIBINCLUDE) || %(ABSLIBINCLUDE_DEBUG)
      PRINTNL 				OTHER_LDFLAGS = (
      FOR EACH LIBINCLUDE
        PRINTNL 					"-L%(!;NAME)",
      ENDFOR
      FOR EACH LIBINCLUDE_DEBUG
        PRINTNL 					"-L%(!;NAME)",
      ENDFOR
      FOR EACH ABSLIBINCLUDE
        PRINTNL 					"-L%(NAME)",
      ENDFOR
      FOR EACH ABSLIBINCLUDE_DEBUG
        PRINTNL 					"-L%(NAME)",
      ENDFOR
      IF (%(TYPE)!=LIB)
         FOR EACH USEBUILT
           PRINTNL 					"-l%(NAME)",
         ENDFOR
         FOR EACH LIBS
           PRINTNL 					"-l%(NAME)",
         ENDFOR
         FOR EACH LIBS_DEBUG
           PRINTNL 					"-l%(NAME)",
         ENDFOR
      ENDIF
      PRINTNL 				);
    ENDIF
    PRINTNL 				OBJROOT = %(!;§BUILDPATH);
    IF (%(TYPE)==EXE) && (%(PROJECT_PATH)!="")
        PRINTNL 				PRODUCT_NAME = %(PROJECT_PATH);
    ELSE
        PRINTNL 				PRODUCT_NAME = %(NAME);
    ENDIF
    PRINTNL 				SYMROOT = %(¯!;§OUTPUTPATH);
    IF %(INCLUDE) || %(INCLUDE_DEBUG) || %(SYSINCLUDE) || %(ABSINCLUDE_DEBUG) || %(ABSINCLUDE) || %(CONFIG_INCLUDE)
      PRINTNL 				USER_HEADER_SEARCH_PATHS = "%(!;@INCLUDE:%=% )%(!;@INCLUDE_DEBUG:%=% )%(!;@SYSINCLUDE:%=% )%(ABSINCLUDE_DEBUG:%=% )%(ABSINCLUDE:%=% )%(!;@CONFIG_INCLUDE:%=% )";
    ENDIF
    IF %(HAS_REZ)
      PRINTNL 				OTHER_REZFLAGS = "-d ppc_$(ppc) -d i386_$(i386)";
    ENDIF
    IF (%(TYPE)==EXE) || (%(TYPE)==DLL)
      IF %(OSX_DLL_TYPE)=="Bundle"
        PRINTNL 				WRAPPER_EXTENSION = component;
      ENDIF
      PRINTNL 				INFOPLIST_FILE = %(NAME).plist;
    ENDIF
    IF %(PRECOMPILED_HEADER)
      PRINTNL 				GCC_PREFIX_HEADER = "%(!;PATH)%(PRECOMPILED_HEADER)";
      PRINTNL 				GCC_PRECOMPILE_PREFIX_HEADER = YES;
    ENDIF
    PRINTNL 			};
    IFDEF OSX_UNIVERSAL
      IFDEF IX86_64
        PRINTNL 			name = "Debug x86_64";
      ELSE
        PRINTNL 			name = "Debug ppc";
      ENDIF
    ELSE
      PRINTNL 			name = Debug;
    ENDIF
    PRINTNL 		};
    IFDEF OSX_UNIVERSAL
      PRINTNL 		%(_XCODEGRPUID8) /* Debug i386 */ = {
      PRINTNL 			isa = XCBuildConfiguration;
      PRINTNL 			buildSettings = {
      PRINTNL 				GCC_C_LANGUAGE_STANDARD = gnu99;
      IF (%(TYPE)!=DLL)
        PRINTNL 				GCC_DYNAMIC_NO_PIC = YES;
      ENDIF
      IF %(FAVOR_MAX_SPEED_DEBUG)
        PRINTNL 				GCC_OPTIMIZATION_LEVEL = 3;
        PRINTNL 				OTHER_CFLAGS = "-fomit-frame-pointer";
        PRINTNL 				OTHER_CPLUSPLUSFLAGS = "-fomit-frame-pointer";
      ELSE
        PRINTNL 				GCC_OPTIMIZATION_LEVEL = 0;
        //PRINTNL 				GCC_ENABLE_FIX_AND_CONTINUE = YES;
      ENDIF
      IF %(ASMFLAGS)!=""
        PRINTNL 				NASM_OTHER_FLAGS = "%(ASMFLAGS)";
      ENDIF
      IF %(DEFINE) || %(DEFINE_DEBUG)
        PRINTNL 				GCC_PREPROCESSOR_DEFINITIONS = (
        FOR EACH DEFINE
          PRINTNL 					"%(NAME)",
        ENDFOR
        FOR EACH DEFINE_DEBUG
          PRINTNL 					"%(NAME)",
        ENDFOR
        PRINTNL 				);
      ENDIF
      IF %(USE) || %(LIBS) || %(LIBS_DEBUG) || %(LIBINCLUDE) || %(LIBINCLUDE_DEBUG) || %(ABSLIBINCLUDE) || %(ABSLIBINCLUDE_DEBUG)
        PRINTNL 				OTHER_LDFLAGS = (
        FOR EACH LIBINCLUDE
          PRINTNL 					"-L%(!;NAME)",
        ENDFOR
        FOR EACH LIBINCLUDE_DEBUG
          PRINTNL 					"-L%(!;NAME)",
        ENDFOR
        FOR EACH ABSLIBINCLUDE
          PRINTNL 					"-L%(NAME)",
        ENDFOR
        FOR EACH ABSLIBINCLUDE_DEBUG
          PRINTNL 					"-L%(NAME)",
        ENDFOR
        IF (%(TYPE)!=LIB)
           FOR EACH USEBUILT
             PRINTNL 					"-l%(NAME)",
           ENDFOR
           FOR EACH LIBS
             PRINTNL 					"-l%(NAME)",
           ENDFOR
           FOR EACH LIBS_DEBUG
             PRINTNL 					"-l%(NAME)",
           ENDFOR
        ENDIF
        PRINTNL 				);
      ENDIF
      PRINTNL 				OBJROOT = %(!;§BUILDPATH);
      IF (%(TYPE)==EXE) && (%(PROJECT_PATH)!="")
        PRINTNL 				PRODUCT_NAME = %(PROJECT_PATH);
      ELSE
        PRINTNL 				PRODUCT_NAME = %(NAME);
      ENDIF
      PRINTNL 				SYMROOT = %(¯!;§OUTPUTPATH);
      IF %(INCLUDE) || %(INCLUDE_DEBUG) || %(SYSINCLUDE) || %(ABSINCLUDE_DEBUG) || %(ABSINCLUDE) || %(CONFIG_INCLUDE)
      PRINTNL 				USER_HEADER_SEARCH_PATHS = "%(!;@INCLUDE:%=% )%(!;@INCLUDE_DEBUG:%=% )%(!;@SYSINCLUDE:%=% )%(ABSINCLUDE_DEBUG:%=% )%(ABSINCLUDE:%=% )%(!;@CONFIG_INCLUDE:%=% )";
      ENDIF
      IF %(HAS_REZ)
        PRINTNL 				OTHER_REZFLAGS = "-d ppc_$(ppc) -d i386_$(i386)";
      ENDIF
      IF (%(TYPE)==EXE) || (%(TYPE)==DLL)
        IF %(OSX_DLL_TYPE)=="Bundle"
          PRINTNL 				WRAPPER_EXTENSION = component;
        ENDIF
        PRINTNL 				INFOPLIST_FILE = %(NAME).plist;
      ENDIF
      IF %(PRECOMPILED_HEADER)
        PRINTNL 				GCC_PREFIX_HEADER = "%(!;PATH)%(PRECOMPILED_HEADER)";
        PRINTNL 				GCC_PRECOMPILE_PREFIX_HEADER = YES;
      ENDIF
      PRINTNL 			};
      PRINTNL 			name = "Debug i386";
      PRINTNL 		};
    ENDIF
  ENDFOR
  IFDEF OSX_UNIVERSAL
    PRINTNL 		00277BB40B5E830800EA36F7 /* Debug other */ = {
  ELSE
    PRINTNL 		00277BB40B5E830800EA36F7 /* Debug */ = {
  ENDIF
  PRINTNL 			isa = XCBuildConfiguration;
  PRINTNL 			buildSettings = {
  IF %(ARCHS)
    PRINTNL 				ARCHS = %(ARCHS);
  ELSE
    IFDEF IX86_64
      PRINTNL 				ARCHS = "$(ARCHS_STANDARD_64_BIT)";
    ELSE
      PRINTNL 				ARCHS = ppc;
    ENDIF
  ENDIF
//  PRINTNL 				BUILD_VARIANTS = debug;
  PRINTNL 				GCC_MODEL_TUNING = G5;
  PRINTNL 				COPY_PHASE_STRIP = NO;
  IFNDEF XCODE4
    PRINTNL 				GCC_ALTIVEC_EXTENSIONS = YES;
  ENDIF
  PRINTNL 				GCC_WARN_ABOUT_RETURN_TYPE = YES;
  PRINTNL 				GCC_WARN_UNUSED_VARIABLE = YES;
  PRINTNL 				GCC_VERSION = "%(GCC_VERSION)";
  PRINTNL 				ONLY_LINK_ESSENTIAL_SYMBOLS = YES;
//  PRINTNL 				ZERO_LINK = YES;
  IFDEF OSX_UNIVERSAL
    PRINTNL 				PREBINDING = NO;
  ENDIF
  PRINTNL 				SYMROOT = %(¯!;§OUTPUTPATH);
  PRINTNL 				OBJROOT = %(!;§BUILDPATH);
  IFDEF OSX_UNIVERSAL
    IFDEF XCODE4
      PRINTNL 				OTHER_CFLAGS = "-force_cpusubtype_ALL";
      PRINTNL 				OTHER_CPLUSPLUSFLAGS = "-force_cpusubtype_ALL";
    ELSE
      PRINTNL 				OTHER_CFLAGS = "-force_cpusubtype_ALL -maltivec";
      PRINTNL 				OTHER_CPLUSPLUSFLAGS = "-force_cpusubtype_ALL -maltivec";
    ENDIF
  ENDIF
  IF %(ASMFLAGS)!=""
    PRINTNL 				NASM_OTHER_FLAGS = "%(ASMFLAGS)";
  ENDIF
  IF %(SDK)
    PRINTNL 				SDKROOT = %(SDK);
  ENDIF
  IF %(DEVICEFAMILY)
    PRINTNL				TARGETED_DEVICE_FAMILY = %(DEVICEFAMILY);
  ENDIF
  IF %(SDK_PATH)
    PRINTNL 				USER_HEADER_SEARCH_PATHS = %(SDK_PATH)/include;
  ENDIF
  IF %(CC_PREFIX)
    PRINTNL 				CC = "%(CC_PREFIX)gcc";
    PRINTNL 				CXX = "%(CC_PREFIX)gcc";
    PRINTNL 				LD = "%(CC_PREFIX)gcc";
    PRINTNL 				STRIP = "%(CC_PREFIX)strip";
  ENDIF
  IF %(FRAMEWORK_PATH) || %(HAS_PRIVATE_FRAMEWORK)
    PRINTNL 				FRAMEWORK_SEARCH_PATHS =  (
    PRINTNL					"$(inherited)",
    IF %(FRAMEWORK_PATH)
      PRINTNL					%(FRAMEWORK_PATH),
    ENDIF
    IF %(HAS_PRIVATE_FRAMEWORK)
      PRINTNL					"\"$(SDKROOT)$(SYSTEM_LIBRARY_DIR)/PrivateFrameworks\"",
    ENDIF
    PRINTNL					);
  ENDIF
  PRINTNL 			};
  IFDEF OSX_UNIVERSAL
    IFDEF IX86_64
      PRINTNL 			name = "Debug x86_64";
    ELSE
      PRINTNL 			name = "Debug ppc";
    ENDIF
  ELSE
    PRINTNL 			name = Debug;
  ENDIF
  PRINTNL 		};
  IFDEF OSX_UNIVERSAL
    PRINTNL 		00177BB40B5E830800EA36F7 /* Debug i386 */ = {
    PRINTNL 			isa = XCBuildConfiguration;
    PRINTNL 			buildSettings = {
    IF %(ARCHS)
      PRINTNL 				ARCHS = %(ARCHS);
    ELSE
      PRINTNL 				ARCHS = i386;
    ENDIF
//    PRINTNL 				BUILD_VARIANTS = debug;
    PRINTNL 				GCC_MODEL_TUNING = G5;
    PRINTNL 				COPY_PHASE_STRIP = NO;
    IFNDEF XCODE4
      PRINTNL 				GCC_ALTIVEC_EXTENSIONS = YES;
    ENDIF
    PRINTNL 				GCC_WARN_ABOUT_RETURN_TYPE = YES;
    PRINTNL 				GCC_WARN_UNUSED_VARIABLE = YES;
    PRINTNL 				GCC_VERSION = "%(GCC_VERSION)";
    PRINTNL 				ONLY_LINK_ESSENTIAL_SYMBOLS = YES;
//    PRINTNL 				ZERO_LINK = YES;
    IFDEF OSX_UNIVERSAL
      PRINTNL 				PREBINDING = NO;
    ENDIF
    PRINTNL 				SYMROOT = %(¯!;§OUTPUTPATH);
    PRINTNL 				OBJROOT = %(!;§BUILDPATH);
    PRINTNL 				OTHER_CFLAGS = "-force_cpusubtype_ALL";
    PRINTNL 				OTHER_CPLUSPLUSFLAGS = "-force_cpusubtype_ALL";
    IF %(ASMFLAGS)!=""
      PRINTNL 				NASM_OTHER_FLAGS = "%(ASMFLAGS)";
    ENDIF
    IFNDEF XCODE4
      PRINTNL 				SDKROOT = /Developer/SDKs/MacOSX10.4u.sdk;
    ENDIF
    IF %(SDK_PATH)
      PRINTNL 				USER_HEADER_SEARCH_PATHS = %(SDK_PATH)/include;
    ENDIF
    IF %(CC_PREFIX)
      PRINTNL 				CC = "%(CC_PREFIX)gcc";
      PRINTNL 				CXX = "%(CC_PREFIX)gcc";
      PRINTNL 				LD = "%(CC_PREFIX)gcc";
      PRINTNL 				STRIP = "%(CC_PREFIX)strip";
    ENDIF
    IF %(FRAMEWORK_PATH) || %(HAS_PRIVATE_FRAMEWORK)
      PRINTNL 				FRAMEWORK_SEARCH_PATHS =  (
      PRINTNL					"$(inherited)",
      IF %(FRAMEWORK_PATH)
        PRINTNL					%(FRAMEWORK_PATH),
      ENDIF
      IF %(HAS_PRIVATE_FRAMEWORK)
        PRINTNL					"\"$(SDKROOT)$(SYSTEM_LIBRARY_DIR)/PrivateFrameworks\"",
      ENDIF
      PRINTNL					);
    ENDIF
    PRINTNL 			};
    PRINTNL 			name = "Debug i386";
    PRINTNL 		};
  ENDIF
  IFDEF OSX_UNIVERSAL
    PRINTNL 		00377BB50B5E830800EA36F7 /* Release Universal */ = {
  ELSE
    PRINTNL		00377BB50B5E830800EA36F7 /* Release */ = {
  ENDIF
  PRINTNL 			isa = XCBuildConfiguration;
  PRINTNL 			buildSettings = {
  IF %(ARCHS)
    PRINTNL 				ARCHS = %(ARCHS);
  ELSE
    IFDEF OSX_UNIVERSAL
      IFDEF IX86_64
        PRINTNL 				ARCHS = "$(ARCHS_STANDARD_32_64_BIT)";
      ELSE
        PRINTNL 				ARCHS = (
        PRINTNL 					i386,
        PRINTNL 					ppc,
        PRINTNL 				);
      ENDIF
    ELSE
      PRINTNL 				ARCHS = ppc;
    ENDIF
  ENDIF
  PRINTNL 				GCC_GENERATE_DEBUGGING_SYMBOLS = NO;
  PRINTNL 				GCC_SYMBOLS_PRIVATE_EXTERN = YES;
  PRINTNL 				GCC_MODEL_TUNING = G5;
//  PRINTNL 				COPY_PHASE_STRIP = YES;
  PRINTNL 				DEPLOYMENT_POSTPROCESSING = YES;
  IFNDEF XCODE4
    PRINTNL 				GCC_ALTIVEC_EXTENSIONS = YES;
  ENDIF
  PRINTNL 				GCC_WARN_ABOUT_RETURN_TYPE = YES;
  PRINTNL 				GCC_WARN_UNUSED_VARIABLE = YES;
  PRINTNL 				GCC_VERSION = "%(GCC_VERSION)";
  PRINTNL 				DEAD_CODE_STRIPPING = YES;
//  PRINTNL 				ZERO_LINK = YES;
  IFDEF OSX_UNIVERSAL
    PRINTNL 				PREBINDING = NO;
  ENDIF
  PRINTNL 				SYMROOT = %(¯!;§OUTPUTPATH);
  PRINTNL 				OBJROOT = %(!;§BUILDPATH);
//  PRINTNL 				OTHER_CFLAGS = "-force_cpusubtype_ALL -maltivec";
//  PRINTNL 				OTHER_CPLUSPLUSFLAGS = "-force_cpusubtype_ALL -maltivec";
  PRINTNL 				OTHER_CFLAGS = "-force_cpusubtype_ALL";
  PRINTNL 				OTHER_CPLUSPLUSFLAGS = "-force_cpusubtype_ALL";
  IF %(ASMFLAGS)!=""
    PRINTNL 				NASM_OTHER_FLAGS = "%(ASMFLAGS)";
  ENDIF
  IF %(SDK)
    PRINTNL 				SDKROOT = %(SDK);
  ENDIF
  IF %(DEVICEFAMILY)
    PRINTNL				TARGETED_DEVICE_FAMILY = %(DEVICEFAMILY);
  ENDIF
  IF %(SDK_PATH)
    PRINTNL 				USER_HEADER_SEARCH_PATHS = %(SDK_PATH)/include;
  ENDIF
  IF %(CC_PREFIX)
    PRINTNL 				CC = "%(CC_PREFIX)gcc";
    PRINTNL 				CXX = "%(CC_PREFIX)gcc";
    PRINTNL 				LD = "%(CC_PREFIX)gcc";
    PRINTNL 				STRIP = "%(CC_PREFIX)strip";
  ENDIF
  IF %(FRAMEWORK_PATH) || %(HAS_PRIVATE_FRAMEWORK)
    PRINTNL 				FRAMEWORK_SEARCH_PATHS =  (
    PRINTNL					"$(inherited)",
    IF %(FRAMEWORK_PATH)
      PRINTNL					%(FRAMEWORK_PATH),
    ENDIF
    IF %(HAS_PRIVATE_FRAMEWORK)
      PRINTNL					"\"$(SDKROOT)$(SYSTEM_LIBRARY_DIR)/PrivateFrameworks\"",
    ENDIF
    PRINTNL					);
  ENDIF
  PRINTNL 			};
  IFDEF OSX_UNIVERSAL
    PRINTNL 			name = "Release Universal";
  ELSE
    PRINTNL 			name = "Release";
  ENDIF
  PRINTNL 		};
  PRINTNL /* End XCBuildConfiguration section */
  PRINTNL 
  PRINTNL /* Begin XCConfigurationList section */
  FOR EACH USE
    PRINTNL 		%(XCODEGRPUID5) /* Build configuration list for PBXNativeTarget "%(NAME)" */ = {
    PRINTNL 			isa = XCConfigurationList;
    PRINTNL 			buildConfigurations = (
    IFDEF OSX_UNIVERSAL
      PRINTNL 				%(_XCODEGRPUID4) /* Release Universal */,
      PRINTNL 				%(XCODEGRPUID4) /* Debug other */,
      PRINTNL 				%(_XCODEGRPUID8) /* Debug i386 */,
    ELSE
      PRINTNL 				%(_XCODEGRPUID4) /* Release */,
      PRINTNL 				%(XCODEGRPUID4) /* Debug */,
    ENDIF
    PRINTNL 			);
    PRINTNL 			defaultConfigurationIsVisible = 0;
    IFDEF OSX_UNIVERSAL
      PRINTNL 			defaultConfigurationName = "Release Universal";
    ELSE
      PRINTNL 			defaultConfigurationName = Release;
    ENDIF
    PRINTNL 		};
  ENDFOR
  IFDEF USE_MAKEFILE
    PRINTNL 		D92591D40C6B42CE00BBDBBC /* Build configuration list for PBXLegacyTarget "%(NAME)" */ = {
  ELSE
    PRINTNL 		AA277BB30B5E830800EA36F7 /* Build configuration list for PBXProject "%(NAME)" */ = {
  ENDIF
  PRINTNL 			isa = XCConfigurationList;
  PRINTNL 			buildConfigurations = (
  IFDEF OSX_UNIVERSAL
    PRINTNL 				00377BB50B5E830800EA36F7 /* Release Universal */,
    PRINTNL 				00177BB40B5E830800EA36F7 /* Debug i386 */,
    PRINTNL 				00277BB40B5E830800EA36F7 /* Debug other */,
  ELSE
    PRINTNL 				00377BB50B5E830800EA36F7 /* Release */,
    PRINTNL 				00277BB40B5E830800EA36F7 /* Debug */,
  ENDIF
  PRINTNL 			);
  PRINTNL 			defaultConfigurationIsVisible = 0;
  IFDEF OSX_UNIVERSAL
    PRINTNL 			defaultConfigurationName = "Release Universal";
  ELSE
    PRINTNL 			defaultConfigurationName = Release;
  ENDIF
  PRINTNL 		};
  PRINTNL /* End XCConfigurationList section */
  PRINTNL 	};
  PRINTNL 	rootObject = 91B15E841AA80083484172DE /* Project object */;
  PRINTNL }

ENDFOR
