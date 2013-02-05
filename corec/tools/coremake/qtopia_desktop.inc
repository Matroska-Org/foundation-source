IF %(TYPE)==EXE
  FILE %(¯PATH)%(NAME).desktop
  PRINTNL [Desktop Entry]
  PRINTNL Exec=%(NAME)
  PRINTNL Type=Application
  IF %(ICON)!=""
    PRINTNL Icon=%(NAME)/%(<ICON)_32x32
  ENDIF
  IF %(PROJECT_NAME)!=""
    PRINTNL Name[]="%(PROJECT_NAME)"
  ELSE
    PRINTNL Name[]="%(NAME)"
  ENDIF
  FOR EACH SOURCE
    FOR EACH MIME_TYPE
      ../../MIMES+=%(NAME)
    ENDFOR
  ENDFOR
  FOR EACH USE
    FOR EACH SOURCE
      FOR EACH MIME_TYPE
        ../../../MIMES+=%(NAME)
      ENDFOR
    ENDFOR
  ENDFOR
  IF %(MIMES)!=""
    FOR EACH MIMES
      IF %(FOR_LAST)
        ../MY_MIMES+=%(NAME)
      ELSE
        ../MY_MIMES+=%(NAME),
      ENDIF
    ENDFOR
    PRINTNL MimeType="%(MY_MIMES)"
  ENDIF
ENDIF
