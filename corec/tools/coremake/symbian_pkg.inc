  PRINTNL &EN
  IF "%(PROJECT_BUILD)"==""
    PROJECT_BUILD = 0
  ENDIF
  IFDEF SYMBIAN90
    IF %(PROJECT_UID_3rd)
      PRINTNL #{"%(PROJECT_NAME)"},(%(PROJECT_UID_3rd)),%(PROJECT_VERSION_MAJOR),%(PROJECT_VERSION_MINOR)%(PROJECT_VERSION_REVISION),%(PROJECT_BUILD)
    ENDIF
    IF "%(PROJECT_VENDOR)"==""
      PROJECT_VENDOR = Vendor
    ENDIF
    IF "%(SYMBIAN_VENDOR)"==""
      SYMBIAN_VENDOR = %(PROJECT_VENDOR)
    ENDIF
    PRINTNL %{"%(PROJECT_VENDOR)-EN"}
    PRINTNL :"%(SYMBIAN_VENDOR)"
    PRINTNL %(TARGSIS)
    PRINTNL "%(TARGEPOC)Epoc32\release\%(TARGPLATFORM)\%(TARGETMODE)\%(PROJECT_PATH).exe"-"!:\sys\bin\%(PROJECT_PATH).exe"
    FOR EACH USE, DEP
      IF %(NAME)==coreprotect_sdk
        PRINTNL "%(../TARGEPOC)Epoc32\release\%(TARGPLATFORM)\%(../TARGETMODE)\%(../PROJECT_PATH).cpb"-"!:\private\%(#../PROJECT_UID_3rd)\%(../PROJECT_PATH).cpb"
      ENDIF
      IF %(TYPE)==DLL
        IF %(DLL_TYPE)=="MDL"
          PRINTNL "%(../TARGEPOC)Epoc32\release\%(../TARGPLATFORM)\%(../TARGETMODE)\%(OUTPUT)"-"!:\sys\bin\%(#PROJECT_UID).dll"
          PRINTNL "%(../TARGEPOC)Epoc32\data\z\resource\plugins\%(NAME).rsc"-"!:\resource\plugins\%(#PROJECT_UID).rsc"
        ELSE
          PRINTNL "%(../TARGEPOC)Epoc32\release\%(../TARGPLATFORM)\%(../TARGETMODE)\%(OUTPUT)"-"!:\sys\bin\%(OUTPUT)"
        ENDIF
      ENDIF
    ENDFOR
    PRINTNL "%(TARGEPOC)Epoc32\data\z\resource\apps\%(PROJECT_PATH).rsc"-"!:\resource\apps\%(PROJECT_PATH).rsc"
    IF %(ICON)!=""
      IF defined(SERIES60)
        PRINTNL "%(TARGEPOC)Epoc32\data\z\resource\apps\%(PROJECT_PATH).mif"-"!:\resource\apps\%(PROJECT_PATH).mif"
      ELSE
        PRINTNL "%(TARGEPOC)Epoc32\data\z\resource\apps\%(PROJECT_PATH)Icon.mbm"-"!:\resource\apps\%(PROJECT_PATH)Icon.mbm"
      ENDIF
    ENDIF
    PRINTNL "%(TARGEPOC)Epoc32\data\z\private\10003a3f\import\apps\%(PROJECT_PATH)_reg.rsc"-"!:\private\10003a3f\import\apps\%(PROJECT_PATH)_reg.rsc"
    IF HAVE_DLL_TXT
      PRINTNL "%(¯~!PATH)%(PROJECT_PATH)_dll.txt"-"!:\private\%(#PROJECT_UID_3rd)\dll.txt"
    ENDIF
    FOR EACH INSTALL
      PRINTNL "%(~!NAME)"-"!:\private\%(#PROJECT_UID_3rd)\%(<NAME).%(>NAME)"
    ENDFOR
    FOR EACH UNINSTALL
      PRINTNL ""-"!:\private\%(#PROJECT_UID_3rd)\%(NAME)",FN
    ENDFOR
    PRINTNL "%(¯~!MAKEPATH)%(PROJECT_PATH)_backup.xml"-"!:\private\%(#PROJECT_UID_3rd)\backup_registration.xml"
  ELSE
    PRINTNL #{"%(PROJECT_NAME)"},(%(PROJECT_UID)),%(PROJECT_VERSION_MAJOR),%(PROJECT_VERSION_MINOR)%(PROJECT_VERSION_REVISION),%(PROJECT_BUILD)
    PRINTNL %(TARGSIS)
    PRINTNL "%(TARGEPOC)Epoc32\release\%(TARGPLATFORM)\%(TARGETMODE)\%(OUTPUT)"-"!:\system\apps\%(PROJECT_PATH)\%(OUTPUT)"
    FOR EACH USE, DEP
      IF %(NAME)==coreprotect_sdk
        // requires to patch the SDK toolchain(s) with coreprotect (see build.bat in coreprotect)
        PRINTNL "%(../TARGEPOC)Epoc32\release\%(../TARGPLATFORM)\%(../TARGETMODE)\%(../PROJECT_PATH).cpb"-"!:\system\apps\%(../PROJECT_PATH)\%(../PROJECT_PATH).cpb"
      ENDIF
      IF %(TYPE)==DLL
        IF %(DLL_TYPE)=="MDL"
          PRINTNL "%(../TARGEPOC)Epoc32\release\%(../TARGPLATFORM)\%(../TARGETMODE)\%(NAME).mdl"-"!:\system\recogs\%(#PROJECT_UID).mdl"
        ELSE
          PRINTNL "%(../TARGEPOC)Epoc32\release\%(../TARGPLATFORM)\%(../TARGETMODE)\%(OUTPUT)"-"!:\system\apps\%(../PROJECT_PATH)\%(OUTPUT)"
        ENDIF
      ENDIF
    ENDFOR
    IF %(TARGSIS_S60_1st)
      PRINTNL "%(TARGEPOC)Epoc32\release\%(TARGPLATFORM)\%(TARGETMODE)\%(PROJECT_PATH).rsc"-"!:\system\apps\%(PROJECT_PATH)\%(PROJECT_PATH).rsc"
      PRINTNL "%(TARGEPOC)Epoc32\release\%(TARGPLATFORM)\%(TARGETMODE)\%(PROJECT_PATH).aif"-"!:\system\apps\%(PROJECT_PATH)\%(PROJECT_PATH).aif"
    ELSE
      PRINTNL "%(TARGEPOC)Epoc32\data\z\system\apps\%(PROJECT_PATH)\%(PROJECT_PATH).rsc"-"!:\system\apps\%(PROJECT_PATH)\%(PROJECT_PATH).rsc"
      PRINTNL "%(TARGEPOC)Epoc32\data\z\system\apps\%(PROJECT_PATH)\%(PROJECT_PATH).aif"-"!:\system\apps\%(PROJECT_PATH)\%(PROJECT_PATH).aif"
    ENDIF
    FOR EACH INSTALL
      PRINTNL "%(~!NAME)"-"!:\system\apps\%(PROJECT_PATH)\%(<NAME).%(>NAME)"
    ENDFOR
    FOR EACH UNINSTALL
      PRINTNL ""-"!:\system\apps\%(PROJECT_PATH)\%(NAME)",FN
    ENDFOR
  ENDIF
