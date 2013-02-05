IF !DEFINED(CARBIDE_VS)
#define CARBIDE_12
ENDIF
#define OUTPUT_EXE %(PROJECT_PATH).app
#define OUTPUT_CON %(PROJECT_PATH).app
#define OUTPUT_DLL %(NAME).dll
#define OUTPUT_LIB %(NAME).lib

IF DEFINED(CARBIDE_20)
  CARBIDE_SUBDIR=carbide_20
ELIF DEFINED(CARBIDE_12)
  CARBIDE_SUBDIR=carbide_12
ELSE
  CARBIDE_SUBDIR=carbide
ENDIF

CONFIG

FOR EACH LIB,DLL,EXE,CON
  IF DEFINED(CARBIDE_12) || DEFINED(CARBIDE_20)
    MAKEPATH=build/%(CARBIDE_SUBDIR)/%(PATH)%(NAME)/
  ELSE
    MAKEPATH=%(PATH)
  ENDIF
ENDFOR

MKDIR %(OUTPUTPATH)

FOR EACH WORKSPACE

  FILE %(¯PATH)make.bat
  PRINTNLDOS @if not "%ProgramFiles(x86)%"=="" cmd /c "path=%ProgramFiles(x86)%\common files\symbian\tools;%PATH% && make.exe @%(DEVICES_ID) %1 %2 %3 %4"
  PRINTNLDOS @if "%ProgramFiles(x86)%"=="" cmd /c "path=%ProgramFiles%\common files\symbian\tools;%PATH% && make.exe @%(DEVICES_ID) %1 %2 %3 %4"

  FILE %(¯PATH)bld.inf
  PRINTNL PRJ_PLATFORMS 
  PRINTNL %(TARGPLATFORM)
  PRINTNL 
  PRINTNL PRJ_MMPFILES 
  FOR EACH USE
    IF %(TYPE)==exe
      IF defined(SYMBIAN90) && defined(SERIES60) && %(ICON)!=""
        PRINTNL gnumakefile %(¯~!PATH)%(PROJECT_PATH)_icon.mak
      ENDIF
    ENDIF
    PRINTNL %(¯~!MAKEPATH)%(NAME).mmp
  ENDFOR

  IF !DEFINED(CARBIDE_VS)
    FILE %(¯PATH)Makefile
    IF (%(TARGPLATFORM)==wins) || (%(TARGPLATFORM)==winscw)
      PRINTNL .PHONY: all install %(USE)
      PRINTNL 
      PRINTNL all: install
      FOR EACH USE
        FOR EACH USELIB
          IF "%(~!+÷../LIBINCLUDE:%=%%(<NAME)_%(PLATFORMLIB).lib)"!=""
            PRINTNL 	@copy /b /y %(~!+÷../LIBINCLUDE:%=%%(<NAME)_%(PLATFORMLIB).lib) %EPOCROOT%EPOC32\RELEASE\%(TARGCPU)\%(TARGMODE)\%(<NAME).lib
          ENDIF
        ENDFOR
      ENDFOR
      PRINTNL 	@bldmake bldfiles %(TARGPLATFORM) %(TARGMODE)
      PRINTNL 	@abld build %(TARGPLATFORM) %(TARGMODE)
      PRINTNL vc6: install
      PRINTNL 	@bldmake bldfiles
      PRINTNL 	@abld makefile vc6
      DSWPATH = %(!PATH)%(NAME)_wins.dsw
      PRINTNL 	@echo Microsoft Developer Studio Workspace File, Format Version 6.00>%(DSWPATH)
      PRINTNL 	@echo # WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!>>%(DSWPATH)
      PRINTNL 	@perl -e "print chr(10);" >>%(DSWPATH)
      FOR EACH USE
        IF (%(TYPE)!=LIB_CSHARP) && (%(TYPE)!=DLL_CSHARP) && (%(TYPE)!=CON_CSHARP) && (%(TYPE)!=EXE_CSHARP)
          PRINTNL 	@echo ###############################################################################>>%(../DSWPATH)
          PRINTNL 	@perl -e "print chr(10);" >>%(../DSWPATH)
          DSPPATH = %EPOCROOT%/epoc32/BUILD%(:^|MAKEPATH)%(^NAME)/WINS/%(^NAME).DSP
          IF (%(TYPE)==EXE)
            PRINTNL 	@perl -e "print 'Project: '.chr(34).'%(^PROJECT_PATH)'.chr(34).'=%(~DSPPATH) - Package Owner=<4>'.chr(10);" >>%(../DSWPATH)
          ELSE
            PRINTNL 	@perl -e "print 'Project: '.chr(34).'%(^NAME)'.chr(34).'=%(~DSPPATH) - Package Owner=<4>'.chr(10);" >>%(../DSWPATH)
          ENDIF
          PRINTNL 	@perl -e "print chr(10);" >>%(../DSWPATH)
          PRINTNL 	@perl -e "print 'Package=<5>'.chr(10).'{{{'.chr(10).'}}}'.chr(10).chr(10);" >>%(../DSWPATH)
          PRINTNL 	@perl -e "print 'Package=<4>'.chr(10).'{{{'.chr(10);" >>%(../DSWPATH)
          FOR EACH USE
            IF ((%(TYPE)!=LIB) || (%(../TYPE)!=LIB)) && (%(TYPE)!=LIB_CSHARP) && (%(TYPE)!=DLL_CSHARP) && (%(TYPE)!=CON_CSHARP) && (%(TYPE)!=EXE_CSHARP)
              PRINTNL 	@echo     Begin Project Dependency>>%(../../DSWPATH)
              PRINTNL 	@echo     Project_Dep_Name %(^NAME)>>%(../../DSWPATH)
              PRINTNL 	@echo     End Project Dependency>>%(../../DSWPATH)
            ENDIF
          ENDFOR
          FOR EACH DEP
            IF (%(TYPE)!="LIB_CSHARP") && (%(TYPE)!="DLL_CSHARP") && (%(TYPE)!="CON_CSHARP") && (%(TYPE)!="EXE_CSHARP")
              PRINTNL 	@echo     Begin Project Dependency>>%(../../DSWPATH)
              PRINTNL 	@echo     Project_Dep_Name %(NAME)>>%(../../DSWPATH)
              PRINTNL 	@echo     End Project Dependency>>%(../../DSWPATH)
            ENDIF
          ENDFOR
          PRINTNL 	@perl -e "print '}}}'.chr(10).chr(10);" >>%(../DSWPATH)
        ENDIF
      ENDFOR
      PRINTNL 	@echo ###############################################################################>>%(DSWPATH)
      PRINTNL 	@perl -e "print chr(10).'Global:'.chr(10).chr(10).'Package=<5>'.chr(10).'{{{'.chr(10).'}}}'.chr(10);" >>%(DSWPATH)
      PRINTNL 	@perl -e "print chr(10).'Package=<3>'.chr(10).'{{{'.chr(10).'}}}'.chr(10).chr(10);" >>%(DSWPATH)
      PRINTNL 	@echo ###############################################################################>>%(DSWPATH)
      PRINTNL 	@perl -e "print chr(10);" >>%(DSWPATH)
      PRINTNL install: 
      FOR EACH USE
        IF (%(TYPE)==EXE)
            IF %(INSTALL)
              IF DEFINED(SYMBIAN90)
                INSTALLDIR = private\%(#PROJECT_UID_3rd)
              ELSE
                INSTALLDIR = system\apps\%(PROJECT_PATH)
              ENDIF
              PRINTNL 	@if not exist %EPOCROOT%Epoc32\release\%(TARGPLATFORM)\%(TARGMODE)\z\%(INSTALLDIR) mkdir %EPOCROOT%Epoc32\release\%(TARGPLATFORM)\%(TARGMODE)\z\%(INSTALLDIR)
              FOR EACH INSTALL
                PRINTNL 	@copy /b /y %(~NAME) %EPOCROOT%Epoc32\release\%(TARGPLATFORM)\%(TARGMODE)\z\%(INSTALLDIR)
              ENDFOR
            ENDIF
        ENDIF
      ENDFOR
    ELSE
      PRINTNL .PHONY: all build %(USE)
      PRINTNL 
      PRINTNL all: build
      FOR EACH USE, DEP
        IF (%(TYPE)==EXE)
          PRINTNL 	@cd %(¯~@PATH) && makesis -d$(EPOCROOT) %(PROJECT_PATH).pkg
          PRINTNL 	@if not exist %(~@OUTPUTPATH) md %(~@OUTPUTPATH)
          IF DEFINED(SYMBIAN90) && %(SYMBIAN_CERT) && %(SYMBIAN_KEY)
            PRINTNL 	@signsis "%(¯~!PATH)%(PROJECT_PATH).sis" "%(~!OUTPUTPATH)%(PROJECT_PATH).sis" "%(~!SYMBIAN_CERT)" "%(~!SYMBIAN_KEY)" %(SYMBIAN_PASS) && del /q "%(¯~!PATH)%(PROJECT_PATH).sis"
          ELSE
            PRINTNL 	@cmd /c move /y "%(¯~!PATH)%(PROJECT_PATH).sis" "%(~!OUTPUTPATH)%(PROJECT_PATH).sis"
          ENDIF
        ENDIF
      ENDFOR
      PRINTNL build:
      FOR EACH USE
        FOR EACH USELIB
          IF "%(~!+÷../LIBINCLUDE:%=%%(<NAME)_%(PLATFORMLIB).lib)"!=""
            PRINTNL 	@copy /b /y %(~!+÷../LIBINCLUDE:%=%%(<NAME)_%(PLATFORMLIB).lib) %EPOCROOT%EPOC32\RELEASE\%(TARGCPU)\%(TARGMODE)\%(<NAME).lib
          ENDIF
        ENDFOR
      ENDFOR
      PRINTNL 	@bldmake bldfiles %(TARGPLATFORM) %(TARGMODE)
      PRINTNL 	@abld build %(TARGPLATFORM) %(TARGMODE)
    ENDIF
    FOR EACH USE
      PRINTNL %(NAME): 
      FOR EACH USELIB
        IF "%(~!+÷../LIBINCLUDE:%=%%(<NAME)_%(PLATFORMLIB).lib)"!=""
          PRINTNL 	@copy /b /y %(~!+÷../LIBINCLUDE:%=%%(<NAME)_%(PLATFORMLIB).lib) %EPOCROOT%EPOC32\RELEASE\%(TARGCPU)\%(TARGMODE)\%(<NAME).lib
        ENDIF
      ENDFOR
      PRINTNL 	@bldmake bldfiles %(TARGPLATFORM) %(TARGMODE)
      PRINTNL 	@abld build %(TARGPLATFORM) %(TARGMODE) %(NAME)
    ENDFOR
    PRINTNL clean:
    PRINTNL 	@bldmake bldfiles
    PRINTNL 	@abld clean
    PRINTNL 	@bldmake clean
  ENDIF

ENDFOR

FOR EACH LIB,DLL,EXE,CON

  IF (%(PROJECT_UID_3rd)=="") && (%(PROJECT_UID)!="")
    PROJECT_UID_3rd = %(PROJECT_UID)
  ENDIF

  FILE %(¯MAKEPATH)%(NAME).mmp

  IF %(~!CONFIG_FILE)!="" // IFDEF ?
    PRINTNL #include "%(~!CONFIG_FILE)"
    PRINTNL 
  ENDIF


  IF %(TARGPLATFORM)==GCCE && "%(TARGMODE)"=="UREL"
    IFDEF CONFIG_COREPROTECT
      PRINTNL OPTION GCCE -O2 -fno-unit-at-a-time -mno-apcs-frame -ffunction-sections -fdata-sections
    ELSE
      PRINTNL OPTION GCCE -O2 -fno-unit-at-a-time -mno-apcs-frame 
    ENDIF
  ENDIF

  IF %(TYPE)==EXE || %(TYPE)==CON
    IF defined(SYMBIAN90)
      PRINTNL TARGETPATH        \sys\bin
      PRINTNL TARGET    	%(PROJECT_PATH).exe
      PRINTNL TARGETTYPE	EXE
      IF %(TYPE)==EXE
        PRINTNL UID             0x100039CE %(PROJECT_UID_3rd)
      ELSE
        PRINTNL UID             0 %(PROJECT_UID_3rd)
      ENDIF
      IF "%(SYMBIAN_CAPABILITY)"!=""
        PRINTNL CAPABILITY %(SYMBIAN_CAPABILITY)
      ELSE
        PRINTNL CAPABILITY NONE
      ENDIF
    ELSE
      PRINTNL TARGET    	%(OUTPUT)
      PRINTNL TARGETTYPE	app
      IF %(TYPE)==EXE
        PRINTNL UID             0x100039CE %(PROJECT_UID)
      ELSE
        PRINTNL UID             0 %(PROJECT_UID)
      ENDIF
      PRINTNL TARGETPATH      \system\apps\%(PROJECT_PATH)
    ENDIF
    PRINTNL 
    PRINTNL EPOCHEAPSIZE 0x8000 0x1000000
    PRINTNL EPOCSTACKSIZE 0x8000

  ELIF (%(TYPE)==DLL)
    IF %(DLL_TYPE)=="MDL"
      IF defined(SYMBIAN90)
        PRINTNL TARGET		%(OUTPUT)
        PRINTNL TARGETTYPE	PLUGIN
        PRINTNL UID             0x10009D8D %(PROJECT_UID)
      ELSE
        PRINTNL TARGET		%(NAME).mdl
        PRINTNL TARGETTYPE	MDL
        PRINTNL UID             0x10003A19 %(PROJECT_UID)
      ENDIF
    ELSE
      PRINTNL TARGET		%(OUTPUT)
      PRINTNL TARGETTYPE	DLL
      PRINTNL UID             0x1000008d %(PROJECT_UID)
    ENDIF
    PRINTNL 
    IF %(PROJECT_VENDOR_UID)!=""
      PRINTNL VENDORID        %(PROJECT_VENDOR_UID)
    ENDIF
    IF defined(SYMBIAN90)
      IF "%(SYMBIAN_CAPABILITY)"!=""
        PRINTNL CAPABILITY %(SYMBIAN_CAPABILITY)
      ELSE
        PRINTNL CAPABILITY NONE
      ENDIF
    ELSE
      IF %(DLL_TYPE)=="MDL"
        PRINTNL TARGETPATH      \system\recogs\
      ELSE
        IF "%(OUTPUT)" != "%(OUTPUT_DLL)"
          PRINTNL TARGETPATH      \system\apps\%(PROJECT_PATH)
        ENDIF
      ENDIF
    ENDIF

    DEFINE += %(<^NAME)_EXPORTS

  ELIF (%(TYPE)==LIB)
    PRINTNL TARGET    	%(OUTPUT)
    PRINTNL TARGETTYPE	lib
  ENDIF

  PRINTNL 

  PRINTNL #define TARGET_SYMBIAN
  PRINTNL #define %(TARGDEFINE)
  PRINTNL 
  IF "%(TARGDEFINE2)"!=""
    PRINTNL MACRO %(TARGDEFINE2)
  ENDIF
  FOR EACH DEFINE
    PRINTNL MACRO %(NAME) 
  ENDFOR
  PRINTNL 

  // source files
  PRINTNL SOURCEPATH	%(~!§ROOTPATH)
  DIRPUSH %(!ROOTPATH)
  FOR EACH SOURCE
    IF ((%(>NAME)==c)||(%(>NAME)==cpp)||(%(>NAME)==S && defined(ARM)))
      PRINTNL SOURCE		%(~!NAME)
    ENDIF
  ENDFOR
  DIRPOP
  PRINTNL 

  PRINTNL SOURCEPATH	.
  IF (%(TYPE)==EXE)
    IF defined(SYMBIAN90)
      IF defined(UIQ) && "%(ICON)"!=""
        PRINTNL START BITMAP	%(PROJECT_PATH)Icon.mbm
        PRINTNL 	HEADER
        PRINTNL 	TARGETPATH		\resource\apps
        PRINTNL 	SOURCE	c24		%(~!ICON)_uiq3_18x18.bmp
        PRINTNL 	SOURCE	8		%(~!ICON)_uiq3_18x18_mask.bmp
        PRINTNL 	SOURCE	c24		%(~!ICON)_uiq3_40x40.bmp
        PRINTNL 	SOURCE	8		%(~!ICON)_uiq3_40x40_mask.bmp
        PRINTNL 	SOURCE	c24		%(~!ICON)_uiq3_64x64.bmp
        PRINTNL 	SOURCE	8		%(~!ICON)_uiq3_64x64_mask.bmp
        PRINTNL END
        PRINTNL 
      ENDIF
    
      PRINTNL START RESOURCE %(PROJECT_PATH).rss
      PRINTNL 	HEADER
      PRINTNL 	TARGETPATH      \resource\apps
      PRINTNL 	LANG SC
      PRINTNL END
      PRINTNL 
      PRINTNL START RESOURCE %(PROJECT_PATH)_reg.rss
      IF %(TARGPLATFORM)==winscw
        PRINTNL 	TARGETPATH      \private\10003a3f\apps
      ELSE
        PRINTNL 	TARGETPATH      \private\10003a3f\import\apps
      ENDIF
      PRINTNL 	LANG SC
      PRINTNL END
      PRINTNL 
    ELSE
      PRINTNL RESOURCE %(PROJECT_PATH).rss
      IF defined(SERIES60)
        PRINTNL RESOURCE %(PROJECT_PATH)_caption.rss
      ENDIF
      PRINTNL 
      IF "%(ICON)"!=""
        IF defined(SERIES60) || defined(SERIES80)
          PRINTNL AIF %(PROJECT_PATH).aif . %(PROJECT_PATH)_aif.rss c16 %(~!ICON)_s60_44x44.bmp %(~!ICON)_s60_44x44_mask.bmp %(~!ICON)_s60_42x29.bmp %(~!ICON)_s60_42x29_mask.bmp
        ELIF defined(SERIES90)
          PRINTNL AIF %(PROJECT_PATH).aif . %(PROJECT_PATH)_aif.rss c16 %(~!ICON)_s90_25x20.bmp %(~!ICON)_s90_25x20_mask.bmp %(~!ICON)_s90_64x50.bmp %(~!ICON)_s90_64x50_mask.bmp
        ELIF defined(UIQ)
          PRINTNL AIF %(PROJECT_PATH).aif . %(PROJECT_PATH)_aif.rss c16 %(~!ICON)_uiq_20x16.bmp %(~!ICON)_uiq_20x16_mask.bmp %(~!ICON)_uiq_32x32.bmp %(~!ICON)_uiq_32x32_mask.bmp
        ENDIF
      ELSE
        PRINTNL AIF %(PROJECT_PATH).aif . %(PROJECT_PATH)_aif.rss 
      ENDIF
      PRINTNL 
    ENDIF
  ENDIF
  IF (%(TYPE)==DLL) && (%(DLL_TYPE)=="MDL")
    IF defined(SYMBIAN90)
      PRINTNL START RESOURCE %(PROJECT_PATH).rss
      PRINTNL 	TARGET %(PROJECT_PATH).rsc
      PRINTNL END
      ENDIF
  ENDIF

  FOR EACH INCLUDE
    ../USERINCLUDE += %(|NAME)
  ENDFOR
  FOR EACH EXPINCLUDE
    ../USERINCLUDE += %(|NAME)
  ENDFOR
  FOR EACH SUBINCLUDE
    ../USERINCLUDE += %(|NAME)
  ENDFOR

  IF %(CONFIG_FILE)
    USERINCLUDE += %(/|CONFIG_FILE)
  ENDIF

  FOR EACH USERINCLUDE
    PRINTNL USERINCLUDE %(~!§NAME)
  ENDFOR
  PRINTNL 

  PRINTNL SYSTEMINCLUDE	\epoc32\include
  PRINTNL SYSTEMINCLUDE	\epoc32\include\libc
  IF defined(SYMBIAN90)
    IF %(DLL_TYPE)=="MDL"
      PRINTNL SYSTEMINCLUDE	\epoc32\include\ecom
    ENDIF
  ENDIF
  IF defined(SERIES60)
    IF defined(SYMBIAN90)
      PRINTNL SYSTEMINCLUDE	\epoc32\include\mmf\plugin
    ENDIF
  ENDIF
  FOR EACH SYSINCLUDE
    PRINTNL SYSTEMINCLUDE	%(~!§NAME)
  ENDFOR
  PRINTNL 

  // libraries
  IF (%(TYPE)!=LIB)
    FOR EACH USE
      IF (!%(NOLIB))
        IF (%(TYPE)==DLL)
          PRINTNL LIBRARY %(NAME).lib
        ELSE
          PRINTNL STATICLIBRARY %(NAME).lib
        ENDIF
     ENDIF
    ENDFOR
    FOR EACH USELIB
      PRINTNL STATICLIBRARY %(<NAME).lib
    ENDFOR
    PRINTNL
  ENDIF

  // symbian libraries
  IF (%(TYPE)==EXE) && %(TARGPLATFORM)!=wins && %(TARGPLATFORM)!=winscw
    IF !defined(SYMBIAN90)
      PRINTNL LIBRARY edllstub.lib
    ENDIF
  ENDIF
  IF (%(TYPE)!=LIB)
    FOR EACH LIBS
      PRINTNL LIBRARY %(NAME)
    ENDFOR
    IF defined(UIQ)
      IF defined(SYMBIAN90)
        PRINTNL LIBRARY qikcore.lib
        PRINTNL STATICLIBRARY qikalloc.lib
        PRINTNL LIBRARY qikallocdll.lib
      ENDIF
    ENDIF
    PRINTNL LANG SC
    PRINTNL
  ENDIF
  
  // DEF files
  IF (%(TYPE)==DLL)
    PRINTNL
    PRINTNL NOSTRICTDEF
    IF (%(DEF)!="")
      PRINTNL #if defined(WINS)
      PRINTNL deffile %(~!DEF)_wins.def
      PRINTNL #elif defined(GCC32)
      PRINTNL deffile %(~!DEF)_gcc.def
      PRINTNL #else
      PRINTNL deffile %(~!DEF)_eabi.def
      PRINTNL #endif
//    ELSE
// not needed when using EXPORT_C in the code
//      PRINTNL #if defined(WINS)
//      PRINTNL deffile .\symbian\%(NAME)_wins.def
//      PRINTNL #elif defined(GCC32)
//      PRINTNL deffile .\symbian\%(NAME)_gcc.def
//      PRINTNL #else
//      PRINTNL deffile .\symbian\%(NAME)_eabi.def
//      PRINTNL #endif
    ENDIF
  ENDIF

  // create additional files...

  IF (%(TYPE)==EXE)

    FILE %(¯MAKEPATH)%(PROJECT_PATH).rss
    IFDEF SYMBIAN90
      PRINTNL #include <appinfo.rh>
      PRINTNL 
    ENDIF

    PRINTNL #define %(TARGDEFINE)
    PRINTNL 
    FOR EACH SOURCE
      IF (%(>NAME)==rss)
        PRINTNL #include "%(~!NAME)"
      ENDIF
    ENDFOR

    IF defined(SYMBIAN90)
      PRINTNL 
      PRINTNL rls_string STRING_%(NAME)_caption_string "%(PROJECT_NAME)"
      PRINTNL 
      PRINTNL RESOURCE LOCALISABLE_APP_INFO r_%(NAME)_localisable_app_info
      PRINTNL {
      PRINTNL   short_caption = STRING_%(NAME)_caption_string;
      PRINTNL   caption_and_icon = 
      PRINTNL   CAPTION_AND_ICON_INFO
      PRINTNL   {
      PRINTNL     caption = STRING_%(NAME)_caption_string;
      IF %(ICON)!=""
        IF defined(SERIES60)
          PRINTNL     number_of_icons = 1;
          PRINTNL     icon_file = "\\resource\\apps\\%(PROJECT_PATH).mif";
        ELSE
          PRINTNL     number_of_icons = 3;
          PRINTNL     icon_file = "\\resource\\apps\\%(PROJECT_PATH)Icon.mbm";
        ENDIF
      ENDIF
      PRINTNL   };
      IF %(QIKVIEW)!=""
        PRINTNL   view_list =
        PRINTNL   {
        FOR EACH QIKVIEW
          PRINTNL   VIEW_DATA
          PRINTNL   {
          PRINTNL     uid = %(UID);
          PRINTNL     screen_mode = %(SCREEN_MODE);
          PRINTNL     caption_and_icon = 
          PRINTNL     {
          PRINTNL       CAPTION_AND_ICON_INFO
          PRINTNL       {
          PRINTNL       }
          PRINTNL     };							
          IF %(FOR_LAST)
            PRINTNL   }
          ELSE
            PRINTNL   },
          ENDIF
        ENDFOR
        PRINTNL   };
      ENDIF
      PRINTNL }
    ENDIF

    IF defined(SYMBIAN90) && defined(SERIES60)
     IF %(ICON)!=""
      FILE %(¯PATH)%(PROJECT_PATH)_icon.mak
      PRINTNL ifeq (WINS,$(findstring WINS, $(PLATFORM)))
      PRINTNL ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
      PRINTNL else
      PRINTNL ZDIR=$(EPOCROOT)epoc32\data\z
      PRINTNL endif
      PRINTNL 
      PRINTNL TARGETDIR=$(ZDIR)\resource\apps
      PRINTNL ICONTARGETFILENAME=$(TARGETDIR)\%(PROJECT_PATH).mif
      PRINTNL 
      PRINTNL do_nothing :
      PRINTNL 	@rem do_nothing
      PRINTNL 
      PRINTNL MAKMAKE : do_nothing
      PRINTNL 
      PRINTNL BLD : do_nothing
      PRINTNL 
      PRINTNL CLEAN :
      PRINTNL 	del /q $(ICONTARGETFILENAME)
      PRINTNL 
      PRINTNL LIB : do_nothing
      PRINTNL 
      PRINTNL CLEANLIB : do_nothing
      PRINTNL 
      PRINTNL RESOURCE : %(~!ICON)_s60.svg
      PRINTNL 	mifconv $(ICONTARGETFILENAME) %(MIFCONV) /c32 %(~!ICON)_s60.svg
      PRINTNL 		
      PRINTNL FREEZE : do_nothing
      PRINTNL 
      PRINTNL SAVESPACE : do_nothing
      PRINTNL 
      PRINTNL RELEASABLES :
      PRINTNL 	@echo $(ICONTARGETFILENAME)
      PRINTNL 
      PRINTNL FINAL : do_nothing
      PRINTNL 
     ENDIF
    ENDIF

    IFDEF SYMBIAN90
      FILE %(¯PATH)%(PROJECT_PATH)_dll.txt
      FOR EACH USE
         IF %(TYPE)==DLL && "%(OUTPUT)" != "%(OUTPUT_DLL)"
           PRINTNL %(OUTPUT)
           HAVE_DLL_TXT = 1
         ENDIF
      ENDFOR
    ENDIF

    FILE %(¯PATH)%(NAME)_rsg.h
    PRINTNL #include <%(PROJECT_PATH).rsg>

    IF "%(TARGSIS)"!=""
      TARGEPOC = \
      TARGETMODE = %(TARGMODE)
      FILE %(¯PATH)%(PROJECT_PATH).pkg
      #include "symbian_pkg.inc"
    ENDIF

    IF %(PROJECT_MIME)!=""
      MIME_TYPES += %(PROJECT_MIME)
    ENDIF
    FOR EACH SOURCE
      FOR EACH MIME_TYPE
        ../../MIME_TYPES += %(NAME)
      ENDFOR
    ENDFOR
    FOR EACH USE
      FOR EACH SOURCE
        FOR EACH MIME_TYPE
          ../../../MIME_TYPES += %(NAME)
        ENDFOR
      ENDFOR
    ENDFOR

    IFNDEF SYMBIAN90

      FILE %(¯MAKEPATH)%(PROJECT_PATH)_aif.rss
      PRINTNL #include <aiftool.rh>
      PRINTNL 
      PRINTNL RESOURCE AIF_DATA
      PRINTNL     {
      PRINTNL     app_uid=%(PROJECT_UID);
      PRINTNL     caption_list=
      PRINTNL     { 
      PRINTNL         CAPTION { code=ELangEnglish; caption="%(PROJECT_NAME)"; }
      PRINTNL     };
      PRINTNL     num_icons=2;
      PRINTNL     embeddability=KAppNotEmbeddable;
      PRINTNL     newfile=KAppDoesNotSupportNewFile;
      IF %(MIME_TYPES)!=""
        PRINTNL     datatype_list=
        PRINTNL         {
        FOR EACH MIME_TYPES
          IF %(FOR_LAST)
            PRINTNL         DATATYPE { priority=EDataTypePriorityLow; type="%(NAME)"; }
          ELSE
            PRINTNL         DATATYPE { priority=EDataTypePriorityLow; type="%(NAME)"; },
          ENDIF
        ENDFOR
        PRINTNL         };
      ENDIF
      PRINTNL     }

      FILE %(¯MAKEPATH)%(PROJECT_PATH)_caption.rss
      PRINTNL #include <apcaptionfile.rh>
      PRINTNL 
      PRINTNL RESOURCE CAPTION_DATA
      PRINTNL {
      PRINTNL     caption="%(PROJECT_NAME)";
      PRINTNL     shortcaption="%(PROJECT_NAME)";
      PRINTNL }

    ELSE

      FILE %(¯MAKEPATH)%(PROJECT_PATH)_backup.xml
      PRINTNL <?xml version="1.0" standalone="yes"?>
      PRINTNL <backup_registration>
      PRINTNL   <system_backup/>
      PRINTNL   <restore requires_reboot = "no"/>
      PRINTNL   <passive_backup>
      PRINTNL     <include_directory name="\" />
      PRINTNL   </passive_backup>
      IF %(SYMBIAN_BACKUP)!=""
        PRINTNL   <public_backup>
        FOR EACH SYMBIAN_BACKUP
          PRINTNL     <include_file name="C:\%(NAME)"/>
        ENDFOR
        PRINTNL   </public_backup>
      ENDIF
      PRINTNL </backup_registration>

      FILE %(¯MAKEPATH)%(PROJECT_PATH)_reg.rss
      PRINTNL #include <appinfo.rh>
      PRINTNL #include <%(PROJECT_PATH).rsg>
      PRINTNL 
      PRINTNL UID2 KUidAppRegistrationResourceFile
      IF %(PROJECT_UID_3rd)
        PRINTNL UID3 %(PROJECT_UID_3rd)
      ENDIF
      PRINTNL 
      PRINTNL rls_string STRING_%(NAME)_resource_file "\\resource\\apps\\%(PROJECT_PATH)"
      PRINTNL 
      PRINTNL RESOURCE APP_REGISTRATION_INFO 
      PRINTNL {
      PRINTNL app_file="%(PROJECT_PATH)";
      PRINTNL hidden = KAppNotHidden;
      PRINTNL embeddability = KAppNotEmbeddable;
      PRINTNL //embeddability = KAppEmbeddable;
      PRINTNL newfile = KAppDoesNotSupportNewFile;
      PRINTNL launch = KAppLaunchInForeground;
      PRINTNL localisable_resource_file =  STRING_%(NAME)_resource_file;
      PRINTNL localisable_resource_id = R_%(^NAME)_LOCALISABLE_APP_INFO;
      IF %(MIME_TYPES)!=""
        PRINTNL datatype_list=
        PRINTNL     {
        FOR EACH MIME_TYPES
          IF %(FOR_LAST)
            PRINTNL         DATATYPE { priority=EDataTypePriorityLow; type="%(NAME)"; }
          ELSE
            PRINTNL         DATATYPE { priority=EDataTypePriorityLow; type="%(NAME)"; },
          ENDIF
        ENDFOR
        PRINTNL     };
      ENDIF
      PRINTNL }

    ENDIF
  ENDIF

  IF (%(TYPE)==DLL) && (%(DLL_TYPE)=="MDL")
    IF defined(SYMBIAN90)
      FILE %(¯MAKEPATH)%(PROJECT_PATH).rss
      PRINTNL #include <RegistryInfo.rh>
      PRINTNL 
      PRINTNL RESOURCE REGISTRY_INFO r_registry
      PRINTNL {
      PRINTNL     dll_uid = %(PROJECT_UID); // should match name of this file
      PRINTNL     interfaces =
      PRINTNL     {
      PRINTNL         INTERFACE_INFO
      PRINTNL         {
      PRINTNL             interface_uid = 0x101F7D87;  // const for all Data Recognizers
      PRINTNL             implementations =
      PRINTNL             {
      PRINTNL                 IMPLEMENTATION_INFO
      PRINTNL                 {
      PRINTNL                     implementation_uid = %(HOST_SYMBIAN_UID); // unique implementation uid
      PRINTNL                     version_no = 1;
      PRINTNL                     display_name = "%(HOST_SYMBIAN_NAME)";
      PRINTNL                     default_data = "";
      PRINTNL                     opaque_data = "";
      PRINTNL                 }
      PRINTNL             };
      PRINTNL         }
      PRINTNL     };
      PRINTNL }
    ELSE
    ENDIF
  ENDIF

ENDFOR

IF !DEFINED(CARBIDE_VS)
#include "carbide.inc"
ENDIF
