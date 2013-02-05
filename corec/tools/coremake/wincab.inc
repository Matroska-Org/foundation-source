IF %(CAB)!="" && %(CABWIZ)!=""
  SOURCE += %(¯PATH)%(NAME)_wince.inf
  FILE %(¯PATH)%(NAME)_wince.inf
  PRINTNLDOS [Version]
  PRINTNLDOS Signature="$Windows NT$"
  PRINTNLDOS Provider="%(PROJECT_VENDOR)"
  PRINTNLDOS CESignature="$Windows CE$"
  PRINTNLDOS 
  PRINTNLDOS [CEStrings]
  IF "%(APPNAME_CAB)"!=""
    PRINTNLDOS AppName="%(APPNAME_CAB)"
  ELSE
    PRINTNLDOS AppName="%(PROJECT_NAME)"
  ENDIF
  PRINTNLDOS InstallDir=%CE1%\%(PROJECT_PATH)
  PRINTNLDOS 
  PRINTNLDOS [CEDevice] 
  IF %(CABCPU)
    PRINTNLDOS ProcessorType = %(CABCPU)
  ENDIF
  IFDEF CONFIG_WINCE2
    PRINTNLDOS VersionMin = 2.0
    PRINTNLDOS VersionMax = 2.999
  ELSE
    PRINTNLDOS VersionMin = 3.0
    PRINTNLDOS VersionMax = 100.0
    IFDEF ARM
      PRINTNLDOS BuildMax = 0xE0000000
      IF %(CABUNSUPPORTED)
        PRINTNLDOS UnsupportedPlatforms = %(CABUNSUPPORTED)
      ENDIF
    ENDIF
  ENDIF
  PRINTNLDOS 
  PRINTNLDOS [DefaultInstall]
  PRINTNLDOS CEShortcuts=Shortcuts
  IF %(REGISTER_CAB) || %(EXTRA_REGISTER_CAB)
    PRINTNLDOS AddReg=RegKeys
  ENDIF
  PRINTNLDOS CopyFiles=Files
  IF %(TYPE)==DLL
    COM_INSTALL = "%(OUTPUT)"
  ENDIF
  FOR EACH USE, DEP
    IF %(TYPE)==DLL
      IF %(SETUP)!=""
        PRINTNLDOS CESetupDLL="%(OUTPUT)"
      ELIF %(COM_COMPONENT)!=""
        IF %(../COM_INSTALL)!=""
          ../COM_INSTALL += ,
        ENDIF
        ../COM_INSTALL += "%(OUTPUT)"
      ENDIF
    ENDIF
  ENDFOR
  IF %(COM_INSTALL)!=""
    PRINTNLDOS CESelfRegister=%(COM_INSTALL)
  ENDIF
  PRINTNLDOS 
  PRINTNLDOS [SourceDisksNames]
  PRINTNLDOS 1 = ,"",,"%(~|OUTPUTPATH)\release"
  DISK = 1
  FOR EACH INSTALL_CAB
    INC ../DISK
    DISK = %(../DISK)
    PRINTNLDOS %(DISK) = ,"",,"%(~/|NAME)"
  ENDFOR
  PRINTNLDOS 
  PRINTNLDOS [SourceDisksFiles]
  PRINTNLDOS "%(OUTPUT)"=1
  FOR EACH USE, DEP
    IF %(TYPE)==DLL
      PRINTNLDOS "%(OUTPUT)"=1
    ENDIF
  ENDFOR 
  FOR EACH INSTALL_CAB
    PRINTNLDOS "%(<NAME).%(>NAME)"=%(DISK)
  ENDFOR
  PRINTNLDOS 
  PRINTNLDOS [DestinationDirs]
  PRINTNLDOS Shortcuts=0,"%CE11%"
  PRINTNLDOS Files=0,"%InstallDir%"
  PRINTNLDOS DefaultDestDir = 0, %InstallDir%
  PRINTNLDOS 
  PRINTNLDOS [Files]
  PRINTNLDOS "%(OUTPUT)",,,0
  FOR EACH USE, DEP
    IF %(TYPE)==DLL
      PRINTNLDOS "%(OUTPUT)",,,0
    ENDIF
  ENDFOR 
  FOR EACH INSTALL_CAB
    PRINTNLDOS "%(<NAME).%(>NAME)",,,0
  ENDFOR
  PRINTNLDOS 
  PRINTNLDOS [Shortcuts]
  PRINTNLDOS "%(PROJECT_NAME).lnk",0,"%(OUTPUT)"
  IF %(REGISTER_CAB) || %(EXTRA_REGISTER_CAB)
    PRINTNLDOS 
    PRINTNLDOS [RegKeys]
    REGEXE=%(OUTPUT)
    FOR EACH REGISTER_CAB
      PRINTNLDOS HKCR,.%(NAME),Default,0x0,"%(NAME)file"
      PRINTNLDOS HKCR,%(NAME)file\shell\open\command,Default,0x0,"""%InstallDir%\\%(REGEXE)""" """%%1"""
      PRINTNLDOS HKCR,%(NAME)file\DefaultIcon,Default,0x0,"%InstallDir%\\%(REGEXE),0"
      IF %(DESCRIPTION)
        PRINTNLDOS HKCR,%(NAME)file,Default,0x0,"%(DESCRIPTION)"
      ENDIF
      PRINTNLDOS 
    ENDFOR
    FOR EACH EXTRA_REGISTER_CAB
      PRINTNLDOS %(NAME)
    ENDFOR
  ENDIF

  SOURCE += %(¯PATH)%(NAME)_wince.ini
  FILE %(¯PATH)%(NAME)_wince.ini
  PRINTNLDOS [CEAppManager]
  PRINTNLDOS Version      = 1.0
  PRINTNLDOS Component    = %(PROJECT_PATH)
  PRINTNLDOS 
  PRINTNLDOS [%(PROJECT_PATH)]
  PRINTNLDOS Description  = %(PROJECT_NAME)
  PRINTNLDOS Uninstall    = %(PROJECT_PATH)
  PRINTNLDOS CabFiles     = %(PROJECT_NAME).%(CABNAME).cab
ENDIF
