FOR EACH WORKSPACE

  REMOVE %(¯PATH)make.bat
  FILE %(¯PATH)GNUmakefile
  IFDEF TARGET_QTOPIA
    IF %(QPEDIR)
      // needed to run some of the Qtopia/Greenphone (stupid) scripts like gph
      PRINTNL PREFIX?=%(QPEDIR)/image
      PRINTNL DPREFIX?=%(QPEDIR)/dimage/opt/Qtopia
      PRINTNL D="-d"
    ENDIF
  ENDIF
  IF defined(NO_MAKE_EXE) || defined(NO_MAKE_EXE_JAVA)
    FOR EACH USE
      IF %(TYPE)==EXE && defined(NO_MAKE_EXE)
        // do nothing
      ELIF %(TYPE)==EXE_JAVA && defined(NO_MAKE_EXE_JAVA)
        // do nothing
      ELSE
        ../ALL_NAMES += %(NAME)
      ENDIF
    ENDFOR
  ELSE
    ALL_NAMES = %(USE)
  ENDIF
  PRINTNL .PHONY: all clean depend cleandep install %(ALL_NAMES)
  PRINTNL 
  PRINTNL all: %(ALL_NAMES)
  PRINTNL 	
  IFDEF TARGET_PS2SDK
    PRINT run:
    FOR EACH USE
      IF %(TYPE)==EXE || %(TYPE)==CON
        PRINT  %(NAME)_run
      ENDIF
    ENDFOR
    PRINTNL 	
    PRINTNL 	
    PRINTNL reset:
    PRINTNL 	@$(PS2DEV)/bin/ps2client reset
    PRINTNL
  ENDIF
  FOR EACH USE
    IF defined(TARGET_PS2SDK) // direct references is too slow otherwise
      PRINTNL %(NAME): %(USE)
    ELSE
      PRINTNL %(NAME):
    ENDIF
    PRINTNL 	@$(MAKE) -C %(¯!PATH) -f %(NAME).mak
    IFDEF TARGET_PS2SDK
      IF %(TYPE)==EXE || %(TYPE)==CON
        PRINTNL %(NAME)_run: %(NAME)
        PRINTNL 	@$(PS2DEV)/bin/ps2client execee host:%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT)
      ENDIF
    ENDIF
  ENDFOR
  PRINTNL clean depend cleandep install:
  FOR EACH USE
    PRINTNL 	@$(MAKE) -C %(¯!PATH) -f %(NAME).mak $@
  ENDFOR
//  PRINTNL 	@rm -rf %(!BUILDPATH)
//  PRINTNL 	@rm -rf %(!OUTPUTPATH)

  IFDEF TARGET_QTOPIA
    PRINTNL install:
    FOR EACH USE
      IF %(TYPE)==EXE
        PRINTNL 	@$(MAKE) -C %(¯!PATH) -f %(NAME).mak install
      ENDIF
    ENDFOR
  ENDIF

ENDFOR

FOR EACH LIB,DLL,EXE,CON
  IF "%(TRANSLATION_QT4)"!=""
    FILE %(¯PATH)%(NAME).tmp.pro
    #include "qmake.inc"
  ENDIF
ENDFOR

FOR EACH LIB,DLL,EXE,CON,EXE_JAVA

  IF defined(TARGET_PALMOS) && (%(TYPE)==exe)
    SOURCE_M68K += %(¯PATH)%(NAME)_palmos.rcp
    FILE %(¯PATH)%(NAME)_palmos.rcp
    PRINTNL APPLICATIONICONNAME 1000 "%(PROJECT_NAME)"

    IF "%(ICON)"!=""
      PRINTNL 
      PRINTNL ICONFAMILYEX 
      PRINTNL BEGIN
      PRINTNL 	BITMAP "%(!ICON)_palmos_22x22.bmp" BPP 8 TRANSPARENT 255 0 255 DENSITY 72
      PRINTNL 	BITMAP "%(!ICON)_palmos_44x44.bmp" BPP 8 TRANSPARENT 255 0 255 DENSITY 144
      PRINTNL END
      PRINTNL 
      PRINTNL SMALLICONFAMILYEX 
      PRINTNL BEGIN
      PRINTNL 	BITMAP "%(!ICON)_palmos_15x9.bmp" BPP 8 TRANSPARENT 255 0 255 DENSITY 72
      PRINTNL 	BITMAP "%(!ICON)_palmos_30x18.bmp" BPP 8 TRANSPARENT 255 0 255 DENSITY 144
      PRINTNL END
    ENDIF
  ENDIF

  IFDEF TARGET_OSX
    IF (%(TYPE)==EXE || %(TYPE)==DLL)
      PLIST_FILE = %(¯PATH)%(NAME).plist
      FILE %(¯PATH)%(NAME).plist
      #include "osx_plist.inc"
    ENDIF
  ENDIF

  IFDEF TARGET_QTOPIA
    #include "qtopia_desktop.inc"
    #include "qtopia_resource.inc"
  ENDIF

  FILE %(¯PATH)%(NAME).mak
  #include "gcc_mak.inc"

  IFDEF TARGET_PALMOS
    IF (%(TYPE)==exe || %(TYPE)==dll)
      FILE %(¯PATH)%(NAME)_palmos.def
      IF (%(TYPE)==exe)
        PRINTNL application { "%(PROJECT_NAME)" "%('PROJECT_FOURCC)" backup }
      ELSE
        PRINTNL database "%('PROJECT_FOURCC)" { "%(PROJECT_NAME)" "%('PROJECT_TYPE)" backup } 
      ENDIF
    ENDIF
    IF "%(INSTALL)"!="" 
      // 1: input
      // 2: split
      // 3: fourcc
      // 4: output
      FILEBIN %(¯PATH)%(NAME)_split.sh
      SCRIPT
      PRINTNL #!/bin/sh
      PRINTNL 
      PRINTNL total=`wc -c $1 | cut -d" " -f1`
      PRINTNL rm -f $4.rcp
      PRINTNL 
      PRINTNL n=0
      PRINTNL while [ $total -gt `expr $n "*" $2` ]
      PRINTNL do
      PRINTNL   if [ $total -gt `expr $n "*" $2 + $2` ]
      PRINTNL   then
      PRINTNL     part=$2
      PRINTNL   else
      PRINTNL     part=`expr $total - $n "*" $2`
      PRINTNL   fi
      PRINTNL   dd iflag=binary oflag=binary bs=1 skip=`expr $n "*" $2` count=$part if=$1 of=$4.$n 2>nul
      PRINTNL   echo DATA \"$3\" ID `expr 1000 + $n` \"$4.$n\" >>$4.rcp
      PRINTNL   n=`expr $n + 1`
      PRINTNL done
    ENDIF
  ENDIF

ENDFOR
