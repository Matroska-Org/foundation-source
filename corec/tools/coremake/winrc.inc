FOR EACH DLL,EXE,CON
  IFNDEF COREMAKE_STATIC
    FOR EACH INSTALL
      ../INSTALL_CAB += %(|NAME)
      ADD ../SOURCE %(|NAME)
        _COPY_ITEM = 1
      ENDADD
      DELETE
    ENDFOR
  ENDIF

  FOR EACH SOURCE
    IF %(>NAME)==rc
      ../SOURCERC += %(|NAME)
      DELETE
    ENDIF
  ENDFOR

  FOR EACH USE
    IF %(TYPE)==lib
      FOR EACH SOURCE
        IF %(>NAME)==rc
          ../../SOURCERC += %(|NAME)
        ENDIF
      ENDFOR
    ENDIF
  ENDFOR

  IF "%(=SOURCERC)"!="0" || "%(ICON)"!="" || "%(INSTALL)"!=""
    IF "%(=SOURCERC)"=="1" && "%(ICON)"=="" && "%(INSTALL)"==""
      SOURCE += %(|SOURCERC)
    ELSE
      SOURCE += %(¯PATH)%(NAME)_win32.rc
      FILE %(¯PATH)%(NAME)_win32.rc
      FOR EACH SOURCERC
        ../INCLUDERC += %(|PATH)
        PRINTNL #include "%(!NAME)"
      ENDFOR
      IF "%(ICON)"!=""
        IFDEF TARGET_WINCE
          PRINTNL 1000 ICON DISCARDABLE "%(!ICON)_wince.ico"
        ELSE
          PRINTNL 1000 ICON DISCARDABLE "%(!ICON)_win32.ico"
        ENDIF
      ENDIF
      FOR EACH INSTALL
        PRINTNL 1000 %(<NAME).%(>NAME) DISCARDABLE "%(!NAME)"
      ENDFOR
    ENDIF
  ENDIF
ENDFOR
