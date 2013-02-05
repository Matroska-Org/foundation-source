RESOURCEPATH=%(¯OUTPUTPATH)
IFDEF TARGET_OSX
  IF %(TYPE)==EXE
    IFDEF TARGET_IPHONE
      PRINTNL OUTPUT = %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(PROJECT_NAME).app/%(PROJECT_NAME)
      IF %(PLIST_FILE)!=""
        PLIST_PATH = %(¯OUTPUTPATH)%(+OUTPUT_DIR)%(PROJECT_NAME).app
        PRINTNL PLIST = %(!PLIST_PATH)/Info.plist
      ENDIF
      OUTPUT_DIR=%(OUTPUT_DIR)%(PROJECT_NAME).app
      RESOURCEPATH=%(¯OUTPUTPATH)%(PROJECT_PATH).app/
    ELSE
      PRINTNL OUTPUT = %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(PROJECT_NAME).app/Contents/MacOS/%(PROJECT_NAME)
      IF %(PLIST_FILE)!=""
        PLIST_PATH = %(¯OUTPUTPATH)%(+OUTPUT_DIR)%(PROJECT_NAME).app/Contents
        PRINTNL PLIST = %(!PLIST_PATH)/Info.plist
      ENDIF
      RESOURCEPATH=%(¯OUTPUTPATH)%(PROJECT_PATH).app/Contents/Resources/
      OUTPUT_DIR=%(OUTPUT_DIR)%(PROJECT_NAME).app/Contents/MacOS
      FOR EACH ICON
        ../INSTALL += %(|NAME).icns
      ENDFOR
    ENDIF
  ELSE
    PRINTNL OUTPUT = %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT)
  ENDIF
ELSE
  PRINTNL OUTPUT = %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT)
ENDIF
PRINTNL BUILD = %(!BUILDPATH)
IFDEF TARGET_PALMOS
  PRINTNL BUILD_M68K = %(!BUILDPATH)m68k/%(PATH)
ENDIF
IF defined(TARGET_PS2SDK) && %(IOP)
  CC = %(iop_cc)
  CXX = %(iop_cxx)
  AR = %(iop_ar)
  STRIP = %(iop_strip)
  CCFLAGS = %(iop_ccflags)
  SHFLAGS = %(iop_shflags)
  LFLAGS = %(iop_lflags)
ENDIF
PRINTNL 

IF ("%(CC)"!="")
  PRINTNL CC = %(CC)
ENDIF
IF ("%(CXX)"!="")
  PRINTNL CXX = %(CXX)
ENDIF
IF ("%(CC_M68K)"!="")
  PRINTNL CC_M68K = %(CC_M68K)
ENDIF
IF ("%(CXX_HOST)"!="")
  PRINTNL CXX_HOST = %(CXX_HOST)
ENDIF
IF ("%(WINDRES)"!="")
  PRINTNL WINDRES = %(WINDRES)
ENDIF
IF (%(ASM)!="")
  PRINTNL ASM = %(ASM)
ENDIF
IF (%(NASM)!="")
  PRINTNL NASM = %(NASM)
ENDIF
IF (%(AR)!="")
  PRINTNL AR = %(AR)
ENDIF
IF ("%(CCENV)"!="")
  PRINTNL CCENV = %(CCENV)
ENDIF
IF ("%(STRIP)"!="")
  PRINTNL STRIP = %(STRIP)
ENDIF
PRINTNL

IF (%(TYPE)==dll)
  DEFINE += %(<^NAME)_EXPORTS
ENDIF

// make sure we don't use the one from another configuration
REMOVE %(PATH)%(PRECOMPILED_HEADER).gch
REMOVE %(PATH)%(PRECOMPILED_HEADER).gch.%(NAME)
IF "%(PRECOMPILED_HEADER)"!="" && "%(PRECOMPILED_HEADER_FORCED)"!=no
  PRINTNL CCFLAGS += -include %(PRECOMPILED_HEADER)
ENDIF
IF %(OS_INCLUDE)!=""
  PRINTNL CCFLAGS += %(|OS_INCLUDE:%=-isystem '%')
ENDIF
PRINTNL CCFLAGS += %(DEFINE:%=-D%) -I . %(@INCLUDE:%=-I '%') %(@SYSINCLUDE:%=-I '%') %(@CONFIG_INCLUDE:%=-I '%')
IF %(CCFLAGS)!=""
  PRINTNL CCFLAGS += %(CCFLAGS)
ENDIF
IF %(CFLAGS)!=""
  PRINTNL CFLAGS += %(CFLAGS)
ENDIF
IF "%(UI_FORM_QT4)"!=""
  PRINTNL CCFLAGS += -I %(!BUILDPATH)%(PATH)GeneratedFiles
ENDIF
PRINTNL CXXFLAGS += %(CXXFLAGS) -fno-for-scope
IF %(ASMFLAGS)!="" || %(DEFINE)!=""
  PRINTNL ASMFLAGS += %(ASMFLAGS) %(DEFINE:%=-D%)
ENDIF
PRINTNL NASMFLAGS += %(NASMFLAGS) %(DEFINE:%=-D%) %(@INCLUDE:%=-I%) %(@CONFIG_INCLUDE:%=-I '%')
PRINTNL SFLAGS += %(SFLAGS) %(DEFINE:%=-D%) %(@INCLUDE:%=-I%) %(@CONFIG_INCLUDE:%=-I '%')
IFDEF RESOURCE_COREC 
  PRINTNL CORERCFLAGS += %(CORERCFLAGS)
ENDIF

IFDEF TARGET_PALMOS
  PRINTNL CFLAGS_M68K = %(CFLAGS_M68K) %(DEFINE:%=-D '%') %(@INCLUDE:%=-I '%') %(@SYSINCLUDE:%=-I '%')  %(@CONFIG_INCLUDE:%=-I '%') -D "PROJECT_OUTPUT=%(<OUTPUT)" -D "PROJECT_FOURCC=%(PROJECT_FOURCC)"
  PRINTNL LFLAGS_M68K = %(LFLAGS_M68K)
  PRINTNL 
  PRINTNL PEAL = %(!BUILDPATH)peal-postlink
  PRINTNL PFLAGS = %(EXPORT:%=-k %) -s 1000
  PRINTNL PEALDIR = %(!ROOTPATH)tools/peal/
  PRINTNL PEALSRC = $(PEALDIR)postlink/image.cc $(PEALDIR)postlink/postlinker.cc $(PEALDIR)postlink/relocation.cc $(PEALDIR)postlink/section.cc 
  PRINTNL PEALSRC += $(PEALDIR)postlink/symbol.cc $(PEALDIR)postlink/symboltable.cc $(PEALDIR)postlink/complain.cc
  PRINTNL 

  FOR EACH INSTALL
    PRINTNL RESOURCE_M68K += $(BUILD_M68K)%(¯!NAME).ro
  ENDFOR
ENDIF

IF defined(TARGET_WIN) && %(TYPE)!=lib
  IF %(TYPE)==con
    PRINTNL LFLAGS += -mconsole
  ELSE
    PRINTNL LFLAGS += -mwindows
  ENDIF
ENDIF

IF defined(TARGET_ANDROID)
  IF %(LIBS)!=""
    PRINTNL LFLAGS += %(@LIBS:%=%(§CONFIG_ANDROID_NDK)/build/platforms/android-4/arch-arm/usr/lib/lib%.so)
  ENDIF
ENDIF

IF (%(TYPE)==dll) && "%(SHFLAGS)"!=""
  PRINTNL LFLAGS += %(SHFLAGS)
ENDIF

IF (%(TYPE)==lib)
  PRINTNL LFLAGS += -static
ENDIF

IF defined(TARGET_PALMOS) && (%(TYPE)!=lib) && !%(NOLIB)
  PRINTNL LFLAGS += -Wl,--whole-archive
ENDIF

FOR EACH LINKFILE
  PRINTNL LFLAGS += -T%(!NAME)
ENDFOR

FOR EACH CRT0
  PRINTNL LFLAGS += %(!NAME)
ENDFOR

IF %(DEF) && defined(TARGET_WIN32)
    PRINTNL LIBS += %(!DEF)
ENDIF
FOR EACH USE
  IF (!%(NOLIB))
    IF (%(TYPE)==dll) && !defined(TARGET_PALMOS)
      ../USEBUILT += %(NAME)
      FOR EACH LIBS
        ../../LIBS += %(NAME)
      ENDFOR
      FOR EACH LIBINCLUDE
        ../../LIBINCLUDE += %(NAME)
      ENDFOR
    ELSE
      PRINTNL LIBS += %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_LIB)
    ENDIF
  ENDIF
  FOR EACH USEBUILT
    IF (!%(NOLIB))
      FOR EACH LIBS
        ../../../LIBS += %(NAME)
      ENDFOR
    ENDIF
  ENDFOR
ENDFOR

IF %(SYSLIBS)!=""
  PRINTNL LFLAGS += %(<SYSLIBS:%=-l%)
ENDIF
IF "%(USELIB)"
  LIBS += %(USELIB:%=%_%(PLATFORMLIB).lib)
ENDIF

PRINTNL LFLAGS += $(LIBS) %(FRAMEWORK:%=-framework %) %(<USEBUILT:%=-l%) %(RPATH:%=-Wl,-rpath,%) %(@LIBINCLUDE:%=-L'%') %(<LIBS:%=-l%) %(LFLAGS) %(¯!OUTPUTPATH:%=-L'%')

IF "%(CONFIG_QTOPIA_PREFIX)"!=""
  PRINTNL LFLAGS += -Wl,-rpath,%(CONFIG_QTOPIA_PREFIX)/lib
ENDIF

IF (defined(TARGET_PALMOS) || defined(TARGET_ANDROID)) && (%(TYPE)!=lib) && !%(NOLIB)
  PRINTNL LFLAGS += -Wl,--no-whole-archive
ENDIF

IF (%(TYPE)==dll) && %(SHFLAGS1)!=""
  PRINTNL LFLAGS += %(SHFLAGS1)
ENDIF

FOR EACH SOURCE
  IF (%(>NAME)==c || %(>NAME)==cpp || %(>NAME)==m || (%(ASM)!="" && %(>NAME)==asm) || (%(NASM)!="" && %(>NAME)==nas) ||%(>NAME)==S || %(>NAME)==rc || (%(DVP_AS)!="" && %(>NAME)==vsm))
    IF "%(FUNC)"==""
      PRINTNL SOURCE += %(!NAME)
    ELSE
      FOR EACH FUNC 
        PRINTNL SOURCE += %(/../NAME)%(<../NAME).%(NAME).%(>../NAME)
        IF %(>../NAME)==c
          ../../FUNC_C += %(NAME)
        ELIF %(>../NAME)==S
          ../../FUNC_S += %(NAME)
        ENDIF
      ENDFOR
    ENDIF
  ENDIF
  IF defined(TARGET_PS2SDK) && (%(>NAME)==lst || %(>NAME)==tab)
    PRINTNL SOURCE += %(!NAME)
  ENDIF
ENDFOR
IFDEF TARGET_PS2SDK
  IF %(ICON)
    PRINTNL SOURCE += %(!ICON)_ps2.icn
  ENDIF
ENDIF
FOR EACH RESOURCE_QT4
    PRINTNL SOURCE += %(!BUILDPATH)%(PATH)GeneratedFiles/qrc_%(~<!NAME).cpp
ENDFOR
FOR EACH HEADER_QT4
    PRINTNL SOURCE += %(!BUILDPATH)%(PATH)GeneratedFiles/moc_%(~<!NAME).cpp
ENDFOR
PRINTNL 


FOR EACH UI_FORM_QT4
    PRINTNL GENERATED += %(!BUILDPATH)%(PATH)GeneratedFiles/ui_%(~<!NAME).h
ENDFOR
FOR EACH RESOURCE_QT4
    PRINTNL GENERATED += %(!BUILDPATH)%(PATH)GeneratedFiles/qrc_%(~<!NAME).cpp
ENDFOR
FOR EACH HEADER_QT4
    PRINTNL GENERATED += %(!BUILDPATH)%(PATH)GeneratedFiles/moc_%(~<!NAME).cpp
ENDFOR
IF "%(PRECOMPILED_HEADER)"!=""
  PRINTNL GENERATED += %(!PATH)%(PRECOMPILED_HEADER).gch
ENDIF
PRINTNL 

PRINTNL ifeq ($(V),1)
PRINTNL  hide = $(empty)
PRINTNL else
PRINTNL  hide = @
PRINTNL endif
PRINTNL 

PRINTNL ifeq ($(DEBUG),1)
// TODO: should alter the temporary files and destination file paths since it's not the same build
PRINTNL else
PRINTNL  CCFLAGS += -DNDEBUG
PRINTNL  SFLAGS += -DNDEBUG
PRINTNL endif
PRINTNL 

PRINTNL OBJS:=$(SOURCE:%(!ROOTPATH)%.c=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%.c=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.cpp=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%.cpp=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.rc=$(BUILD)%_res.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%.rc=$(BUILD)%_res.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.S=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%.S=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.m=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%.m=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.nas=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%.nas=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.asm=$(BUILD)%.%(NAME).o)
PRINTNL OBJS:=$(OBJS:%.asm=$(BUILD)%.%(NAME).o)
IF defined(TARGET_PS2SDK)
  PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.lst=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%.lst=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.tab=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%.tab=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.vsm=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%.vsm=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.icn=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%.icn=$(BUILD)%.%(NAME).o)
ENDIF
PRINTNL 

PRINTNL DEPS:=$(SOURCE:%(!ROOTPATH)%.c=$(BUILD)%.%(NAME).o.d)
PRINTNL DEPS:=$(DEPS:%.c=$(BUILD)%.%(NAME).o.d)
PRINTNL DEPS:=$(DEPS:%(!ROOTPATH)%.cpp=$(BUILD)%.%(NAME).o.d)
PRINTNL DEPS:=$(DEPS:%.cpp=$(BUILD)%.%(NAME).o.d)
PRINTNL DEPS:=$(DEPS:%(!ROOTPATH)%.rc=$(BUILD)%_res.%(NAME).o.d)
PRINTNL DEPS:=$(DEPS:%.rc=$(BUILD)%_res.%(NAME).o.d)
PRINTNL DEPS:=$(DEPS:%(!ROOTPATH)%.S=$(BUILD)%.%(NAME).o.d)
PRINTNL DEPS:=$(DEPS:%.S=$(BUILD)%.%(NAME).o.d)
PRINTNL DEPS:=$(DEPS:%(!ROOTPATH)%.m=$(BUILD)%.%(NAME).o.d)
PRINTNL DEPS:=$(DEPS:%.m=$(BUILD)%.%(NAME).o.d)
PRINTNL DEPS:=$(DEPS:%(!ROOTPATH)%.nas=)
PRINTNL DEPS:=$(DEPS:%.nas=)
PRINTNL DEPS:=$(DEPS:%(!ROOTPATH)%.asm=)
PRINTNL DEPS:=$(DEPS:%.asm=)
IF defined(TARGET_PS2SDK)
  PRINTNL DEPS:=$(DEPS:%(!ROOTPATH)%.lst=$(BUILD)%.%(NAME).o.d)
  PRINTNL DEPS:=$(DEPS:%.lst=$(BUILD)%.%(NAME).o.d)
  PRINTNL DEPS:=$(DEPS:%(!ROOTPATH)%.tab=$(BUILD)%.%(NAME).o.d)
  PRINTNL DEPS:=$(DEPS:%.tab=$(BUILD)%.%(NAME).o.d)
  PRINTNL DEPS:=$(DEPS:%(!ROOTPATH)%.vsm=$(BUILD)%.%(NAME).o.d)
  PRINTNL DEPS:=$(DEPS:%.vsm=$(BUILD)%.%(NAME).o.d)
  PRINTNL DEPS:=$(DEPS:%(!ROOTPATH)%.icn=$(BUILD)%.%(NAME).o.d)
  PRINTNL DEPS:=$(DEPS:%.icn=$(BUILD)%.%(NAME).o.d)
ENDIF
PRINTNL 

IF "%(USEBUILT)"
  PRINTNL USEBUILT:=%(USEBUILT)
  PRINTNL
ENDIF

IFDEF RESOURCE_COREC
  IF (%(TYPE)!=lib)
    FOR EACH INSTALL
      PRINTNL OBJS += $(BUILD)%(NAME).%(../NAME).o
    ENDFOR
    PRINTNL 
  ENDIF
ENDIF

IF defined(TARGET_PALMOS) && "%(SOURCE_M68K)"
  OUTPUT_M68K = %(!BUILDPATH)%(NAME).m68k
  FOR EACH SOURCE_M68K
    IF (%(>NAME)==c)
      PRINTNL SOURCE_M68K += %(!NAME)
    ENDIF
    IF (%(>NAME)==rcp)
      PRINTNL RESOURCE_M68K += %(!NAME)
    ENDIF
  ENDFOR
  PRINTNL OBJS_M68K=$(SOURCE_M68K:%.c=$(BUILD_M68K)%.%(NAME).o)
  PRINTNL RES_M68K=$(RESOURCE_M68K:%.rcp=$(BUILD_M68K)%.ro)
ENDIF

PRINTNL .PHONY: all prebuild postbuild install clean distclean cleandep $(LIBS)
PRINTNL 
PRINTNL all: prebuild $(LIBS) $(GENERATED) $(OUTPUT) postbuild
PRINTNL 
IF "%(PRECOMPILED_HEADER)"!=""
  PRINTNL prebuild:
//  PRINTNL 	$(hide)echo prebuild
  PRINTNL 	$(hide)-test ! -f %(PRECOMPILED_HEADER).gch.%(NAME) || mv %(PRECOMPILED_HEADER).gch.%(NAME) %(!PATH)%(PRECOMPILED_HEADER).gch
  PRINTNL 
ENDIF
IF "%(PRECOMPILED_HEADER)"!=""
  PRINTNL postbuild:
//  PRINTNL 	$(hide)echo postbuild
  PRINTNL 	$(hide)-test ! -f %(!PATH)%(PRECOMPILED_HEADER).gch || mv %(!PATH)%(PRECOMPILED_HEADER).gch %(PRECOMPILED_HEADER).gch.%(NAME)
  PRINTNL 
ENDIF

IF defined(TARGET_PALMOS)
  IF (%(TYPE)==lib)
    PRINTNL $(OUTPUT): $(OBJS) 
    PRINTNL 	$(hide)echo linking $@
    PRINTNL 	$(hide)mkdir -p %(¯!OUTPUTPATH)%(OUTPUT_DIR)
    PRINTNL 	$(hide)$(AR) rcs $@ $(OBJS)
  ELSE
    PRINTNL $(OUTPUT): %(!BUILDPATH)%(NAME)_arm.ro %(¯!PATH)%(NAME)_palmos.def %(OUTPUT_M68K) $(RES_M68K) $(PEAL)
    PRINTNL 	$(hide)mkdir -p %(¯!OUTPUTPATH)
    IF !%(NOLIB)
    PRINTNL 	$(hide)$(PEAL) -l %(NAME) -o %(¯!OUTPUTPATH)lib%(NAME).a %(!BUILDPATH)%(NAME).elf
    ENDIF
    PRINTNL 	$(hide)build-prc %(¯!PATH)%(NAME)_palmos.def -o $@ %(!BUILDPATH)%(NAME)_arm.ro %(OUTPUT_M68K) $(RES_M68K)
    IF ("%(OUTPUT)"=="%(NAME).prc") && ("%(PROJECT_NAME)"!="%(NAME)")
      PRINTNL 	$(hide)cp $(OUTPUT) "%(¯!OUTPUTPATH)%(PROJECT_NAME).prc"
    ENDIF
    IF %(TYPE)=exe
      FOR EACH USE
        IF %(TYPE)=dll
          ../INSTALL_ZIP += "%(¯!OUTPUTPATH)%(PROJECT_NAME).prc"
        ENDIF
      ENDFOR
      IF "%(INSTALL_ZIP)"!=""
        INSTALL_ZIP += "%(¯!OUTPUTPATH)%(PROJECT_NAME).prc"
        PRINTNL 	$(hide)zip -X -q -o "%(¯!OUTPUTPATH)%(PROJECT_NAME).zip" %(INSTALL_ZIP)
      ENDIF
    ENDIF
    PRINTNL
    PRINTNL %(!BUILDPATH)%(NAME)_arm.ro: %(!BUILDPATH)%(NAME).elf $(PEAL)
    PRINTNL 	$(hide)$(PEAL) $(PFLAGS) -o $@ %(!BUILDPATH)%(NAME).elf
    PRINTNL
    PRINTNL %(!BUILDPATH)%(NAME).m68k: $(OBJS_M68K)
    PRINTNL 	$(hide)echo linking m68k
    PRINTNL 	$(hide)mkdir -p %(!BUILDPATH)
    PRINTNL 	$(hide)$(CC_M68K) $(CFLAGS_M68K) -static $(OBJS_M68K) $(LFLAGS_M68K) -o $@
    PRINTNL 
    PRINTNL %(!BUILDPATH)%(NAME).elf: $(OBJS) $(LIBS)
    PRINTNL 	$(hide)echo linking arm
    PRINTNL 	$(hide)mkdir -p %(!BUILDPATH)
    PRINTNL 	$(hide)$(CCENV) $(CC) $(OBJS) -o %(!BUILDPATH)%(NAME).elf $(LFLAGS) -Wl,-Map=%(!BUILDPATH)%(NAME).map
    PRINTNL 
    PRINTNL $(PEAL): $(PEALSRC)
    PRINTNL 	$(hide)echo compiling peal-postlink
    PRINTNL 	$(hide)$(CXX_HOST) $(PEALSRC) -o $(PEAL)
    PRINTNL 	$(hide)chmod u+x $(PEAL)
  ENDIF
  PRINTNL 
  PRINTNL $(BUILD_M68K)%.%(NAME).o: %.c
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD_M68K)$(*D)  
  PRINTNL 	$(hide)$(CC_M68K) -Wall $(CFLAGS_M68K) -c $< -o $@
  PRINTNL 
  PRINTNL $(BUILD_M68K)%.ro: %.rcp
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD_M68K)$(*D)  
  PRINTNL 	$(hide)pilrc -I $(*D) -q -ro $< $@
  PRINTNL 

  FOR EACH INSTALL
    PRINTNL $(BUILD_M68K)%(¯!NAME).ro: %(!NAME)
    PRINTNL 	$(hide)echo compiling $<
    PRINTNL 	$(hide)mkdir -p $(BUILD_M68K)%(¯/NAME) 
    PRINTNL 	$(hide)%(../NAME)_split.sh %(!NAME) 64512 %(,NAME) $(BUILD_M68K)%(¯!NAME)
    PRINTNL 	$(hide)pilrc -q -ro $(BUILD_M68K)%(¯!NAME).rcp $@
    PRINTNL 
  ENDFOR

ELSE
  IFNDEF TARGET_QTOPIA
    IF !defined(RESOURCE_COREC) && !defined(TARGET_ANDROID)
      FOR EACH INSTALL
        PRINTNL INSTALL += %(!RESOURCEPATH)%(<NAME).%(>NAME)
      ENDFOR
    ENDIF
    IFDEF TARGET_IPHONE
      IF %(ICON)
        PRINTNL ICON = %(!RESOURCEPATH)icon.png
      ENDIF
    ENDIF
    PRINTNL 
  ENDIF

  IF (%(TYPE)==lib)
    PRINTNL $(OUTPUT): $(OBJS) $(USEBUILT) $(ICON) $(INSTALL) $(PLIST)
  ELSE
    PRINTNL $(OUTPUT): $(OBJS) $(LIBS) $(USEBUILT) $(ICON) $(INSTALL) $(PLIST)
  ENDIF
  PRINTNL 	$(hide)echo linking $@
  PRINTNL 	$(hide)mkdir -p %(¯!OUTPUTPATH)%(OUTPUT_DIR)
  IF (%(TYPE)==lib)
    PRINTNL 	$(hide)$(AR) rcs $@ $(OBJS)
    IF "%(EXPORT_SVN)"
      PRINTNL 	$(hide)echo exporting for SVN
      IF "%(HOST_WIN32)"!=""
        PRINTNL 	$(hide)if not exist "%(!EXPORT_SVN)export/lib/%(SVNDIR)/static/" mkdir -p "%(!EXPORT_SVN)export/lib/%(SVNDIR)/static/"
      ELSE
        PRINTNL 	$(hide)test -d "%(!EXPORT_SVN)export/lib/%(SVNDIR)/static/" || mkdir -p "%(!EXPORT_SVN)export/lib/%(SVNDIR)/static/"
      ENDIF
      PRINTNL 	$(hide)cp -f "$@" "%(!EXPORT_SVN)export/lib/%(SVNDIR)/static/"
    ENDIF
  ELSE
    IF %(TYPE)==dll && defined(TARGET_ANDROID)
      PRINTNL 	$(hide)$(CC) -nostdlib -Wl,-soname,%(OUTPUT_DLL) -Wl,-shared,-Bsymbolic $(OBJS) $(LFLAGS) -o $@
    ELSE
      PRINTNL 	$(hide)$(CXX) $(CCFLAGS) $(OBJS) -o $@ $(LFLAGS) 
      //-Wl,-Map=%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(NAME).map
    ENDIF
    IF "%(STRIP)"
      PRINTNL 	$(hide)$(STRIP) %(STRIPFLAGS) $@
    ENDIF
    IF %(TYPE)==dll && defined(TARGET_ANDROID)
      PRINTNL 	$(hide)mkdir -p %(!BUILDDIR)libs/%(ANDROID_ABI)
      PRINTNL 	$(hide)install -p $@ %(!BUILDDIR)libs/%(ANDROID_ABI)
    ENDIF
    IF (%(TYPE)==dll)
      IF defined(TARGET_WIN32) && "%(EXPORT_MSVC_LIB)"!=""
        PRINTNL 	$(hide)lib -machine:i386 -def:"%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_DLL_DEF)" -out:"%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(<NAME).lib"
      ENDIF
      IF "%(EXPORT_SVN)"
        PRINTNL 	$(hide)echo exporting for SVN
        IF "%(HOST_WIN32)"!=""
          PRINTNL 	$(hide)if not exist "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/" mkdir -p "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
        ELSE
          PRINTNL 	$(hide)test -d "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/" || mkdir -p "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
        ENDIF
        PRINTNL 	$(hide)cp -f "$@" "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
        PRINTNL 	$(hide)cp -f "%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_DLL_LIB)" "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
        IF "%(OUTPUT_DLL_DEF)"!=""
          PRINTNL 	$(hide)cp -f "%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_DLL_DEF)" "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
        ENDIF
        IF defined(TARGET_WIN32) && "%(EXPORT_MSVC_LIB)"!=""
          PRINTNL 	$(hide)cp -f "%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(<NAME).lib" "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
        ENDIF
      ENDIF
    ENDIF
  ENDIF
  IF "%(TRANSLATION_QT4)"!=""
    PRINTNL 	$(hide)echo processing translations
    PRINTNL 	$(hide)"$(QTDIR)/bin/lupdate" %(NAME).tmp.pro
    PRINTNL 	$(hide)"$(QTDIR)/bin/lrelease" %(NAME).tmp.pro
    FOR EACH TRANSLATION_QT4
      PRINTNL 	$(hide)cp -f "%(?NAME)%(<NAME:%=%.qm)" "%(¯!OUTPUTPATH)%(OUTPUT_DIR)"
    ENDFOR
  ENDIF

  IF %(PLIST_FILE)!=""
    PRINTNL
    PRINTNL %(!PLIST_PATH)/Info.plist: %(!PLIST_FILE)
    PRINTNL 	$(hide)echo copy $<
    PRINTNL 	$(hide)mkdir -p %(!PLIST_PATH)
    PRINTNL 	$(hide)cp -f $< $@
  ENDIF

  IFNDEF TARGET_QTOPIA
    IFDEF TARGET_IPHONE
      IF %(ICON)
        PRINTNL 
        PRINTNL %(!RESOURCEPATH)icon.png: %(!ICON)_iphone.png
        PRINTNL 	$(hide)echo copy $<
        PRINTNL 	$(hide)mkdir -p %(!RESOURCEPATH)
        PRINTNL 	$(hide)cp -f $< $@
      ENDIF
    ENDIF
    IFDEF RESOURCE_COREC 
      IF (%(TYPE)!=lib)
        PRINTNL 
        FOR EACH INSTALL
          PRINTNL $(BUILD)%(NAME).%(../NAME).o: %(!NAME)
          PRINTNL 	$(hide)echo compiling $<
          DIRPUSH %(!BUILDDIR)
          PRINTNL 	$(hide)mkdir -p $(BUILD)%(¯!/NAME)
          DIRPOP
          PRINTNL 	$(hide)corerc -corec $(CORERCFLAGS) $< | $(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -w -x c -c - -o $@
          PRINTNL 
        ENDFOR
      ENDIF
    ELIF !defined(TARGET_ANDROID)
      PRINTNL 
      FOR EACH INSTALL
        PRINTNL 
        PRINTNL %(!RESOURCEPATH)%(<NAME).%(>NAME): %(!NAME)
        PRINTNL 	$(hide)echo copy $<
        PRINTNL 	$(hide)mkdir -p %(!RESOURCEPATH)
        PRINTNL 	$(hide)cp -f $< $@
      ENDFOR
    ENDIF
  ENDIF

ENDIF

  IF "%(USEBUILT)" || "%(USE)"
    PRINTNL
    FOR EACH USEBUILT
      PRINTNL %(NAME):
      PRINTNL 	$(hide)$(MAKE) -C %(¯!§PATH) -f %(NAME).mak
    ENDFOR
    IF !defined(TARGET_PS2SDK) // otherwise it has too many references
      FOR EACH USE
        IF (%(TYPE)==dll) && !defined(TARGET_PALMOS)
          PRINTNL %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT):
          PRINTNL 	$(hide)$(MAKE) -C %(¯!§PATH) -f %(NAME).mak
        ELSE
          PRINTNL %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_LIB):
          PRINTNL 	$(hide)$(MAKE) -C %(¯!§PATH) -f %(NAME).mak
        ENDIF
      ENDFOR
    ENDIF
  ENDIF

IF "%(PRECOMPILED_HEADER)"!=""
  PRINTNL
  PRINTNL $(OBJS): %(!PATH)%(PRECOMPILED_HEADER).gch
ENDIF

PRINTNL 
IF ("%(WINDRES)"!="")
  PRINTNL $(BUILD)%_res.%(NAME).o: %(!ROOTPATH)%.rc
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)  
  PRINTNL 	$(hide)$(WINDRES) -F pe-i386 -I$(*D) $< $@
  PRINTNL 
  PRINTNL $(BUILD)%_res.%(NAME).o: %.rc
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)  
  PRINTNL 	$(hide)$(WINDRES) -F pe-i386 -I$(*D) $< $@
  PRINTNL 
ENDIF
PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.c
PRINTNL 	$(hide)echo compiling $<
PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@ -MMD -MP -MF $@.d
PRINTNL 
PRINTNL $(BUILD)%.%(NAME).o: %.c
PRINTNL 	$(hide)echo compiling $<
PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@ -MMD -MP -MF $@.d
PRINTNL 
FOR EACH FUNC_C
  PRINTNL $(BUILD)%.%(NAME).%(../NAME).o: %(!ROOTPATH)%.c
  PRINTNL 	$(hide)echo compiling $< %(NAME)
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -DF_%(NAME) -c $< -o $@ -MMD -MP -MF $@.d
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).%(../NAME).o: %.c
  PRINTNL 	$(hide)echo compiling $< %(NAME)
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -DF_%(NAME) -c $< -o $@ -MMD -MP -MF $@.d
  PRINTNL 
ENDFOR
PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.m
PRINTNL 	$(hide)echo compiling $<
PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@ -MMD -MP -MF $@.d
PRINTNL 
PRINTNL $(BUILD)%.%(NAME).o: %.m
PRINTNL 	$(hide)echo compiling $<
PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@ -MMD -MP -MF $@.d
PRINTNL 
PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.S
PRINTNL 	$(hide)echo compiling $<
PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(SFLAGS) -c $< -o $@ -MMD -MP -MF $@.d
PRINTNL 
PRINTNL $(BUILD)%.%(NAME).o: %.S
PRINTNL 	$(hide)echo compiling $<
PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(SFLAGS) -c $< -o $@ -MMD -MP -MF $@.d
PRINTNL 
FOR EACH FUNC_S
  PRINTNL $(BUILD)%.%(NAME).%(../NAME).o: %(!ROOTPATH)%.S
  PRINTNL 	$(hide)echo compiling $< %(NAME)
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(SFLAGS) -DF_%(NAME) -c $< -o $@ -MMD -MP -MF $@.d
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).%(../NAME).o: %.S
  PRINTNL 	$(hide)echo compiling $< %(NAME)
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(SFLAGS) -DF_%(NAME) -c $< -o $@ -MMD -MP -MF $@.d
  PRINTNL 
ENDFOR
PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.cpp
PRINTNL 	$(hide)echo compiling $<
PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
PRINTNL 	$(hide)$(CCENV) $(CXX) -Wall $(CXXFLAGS) $(CCFLAGS) -c $< -o $@ -MMD -MP -MF $@.d
PRINTNL 
PRINTNL $(BUILD)%.%(NAME).o: %.cpp
PRINTNL 	$(hide)echo compiling $<
PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
PRINTNL 	$(hide)$(CCENV) $(CXX) -Wall $(CXXFLAGS) $(CCFLAGS) -c $< -o $@ -MMD -MP -MF $@.d
PRINTNL 
IF (%(ASM)!="")
  PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.asm
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(ASM) $(ASMFLAGS) -o $@ $<
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %.asm
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(ASM) $(ASMFLAGS) -o $@ $<
  PRINTNL 
ENDIF
IF (%(NASM)!="")
  PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.nas
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(NASM) $(NASMFLAGS) -o $@ $<
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %.nas
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(NASM) $(NASMFLAGS) -o $@ $<
  PRINTNL 
ENDIF
IF defined(TARGET_PS2SDK)
  PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.vsm
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(CCENV) %(DVP_AS) --warn -I $(<D) $< -o $@
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %.vsm
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(CCENV) %(DVP_AS) --warn -I $(<D) $< -o $@
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.icn
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)corerc -n config_icon $(CORERCFLAGS) $< | $(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -w -x c -c - -o $@
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %.icn
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)corerc -n config_icon $(CORERCFLAGS) $< | $(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -w -x c -c - -o $@
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.tab
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -x c -include $(<D)/irx_imports.h -c $< -o $@ -MMD -MP -MF $@.d
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %.tab
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -x c -include $(<D)/irx_imports.h -c $< -o $@ -MMD -MP -MF $@.d
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.lst
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -x c -include $(<D)/irx_imports.h -c $< -o $@ -MMD -MP -MF $@.d
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %.lst
  PRINTNL 	$(hide)echo compiling $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)$(*D)
  PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -x c -include $(<D)/irx_imports.h -c $< -o $@ -MMD -MP -MF $@.d
  PRINTNL 
ENDIF
IF "%(UI_FORM_QT4)"!=""
  FOR EACH UI_FORM_QT4
    PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/ui_%.h: %(!NAME)
  ENDFOR
//    PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/ui_%.h: %.ui
  PRINTNL 	$(hide)echo UICing $<
  PRINTNL 	$(hide)mkdir -p %(!BUILDPATH)%(PATH)GeneratedFiles/$(*D)
  PRINTNL 	$(hide)"$(QTDIR)/bin/uic" -o $@ $<
  PRINTNL 
ENDIF
IF "%(RESOURCE_QT4)"!=""
  FOR EACH RESOURCE_QT4
    PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/qrc_%.cpp: %(!NAME)
  ENDFOR
//    PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/qrc_%.cpp: %.qrc
  PRINTNL 	$(hide)echo RCCing $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)GeneratedFiles/$(*D)
  PRINTNL 	$(hide)"$(QTDIR)/bin/rcc" -name $< -o $@ $<
  PRINTNL 
ENDIF
IF "%(HEADER_QT4)"!=""
  FOR EACH HEADER_QT4
    PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/moc_%.cpp: %(!NAME)
  ENDFOR
//    PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/moc_%.cpp: %.h
  PRINTNL 	$(hide)echo MOCing $<
  PRINTNL 	$(hide)mkdir -p $(BUILD)GeneratedFiles/$(*D)
  PRINTNL 	$(hide)"$(QTDIR)/bin/moc" %(@INCLUDE:%=-I '%') %(@SYSINCLUDE:%=-I '%')  %(@CONFIG_INCLUDE:%=-I '%') $< -o $@
  PRINTNL 
ENDIF
IF "%(PRECOMPILED_HEADER)"!=""
  PRINTNL %(!ROOTPATH)%.%(>PRECOMPILED_HEADER).gch: %(!ROOTPATH)%.%(>PRECOMPILED_HEADER)
  PRINTNL 	$(hide)echo precompiling header $<
  PRINTNL 	$(hide)-rm -f $@
  IF "%(>SOURCE)"=="c"
    PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@
  ELSE
    PRINTNL 	$(hide)$(CCENV) $(CXX) -Wall $(CXXFLAGS) $(CCFLAGS) -c $< -o $@
  ENDIF
  PRINTNL

  PRINTNL %.%(>PRECOMPILED_HEADER).gch: %.%(>PRECOMPILED_HEADER)
  PRINTNL 	$(hide)echo precompiling header $<
  PRINTNL 	$(hide)-rm -f $@
  IF "%(>SOURCE)"=="c"
    PRINTNL 	$(hide)$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@
  ELSE
    PRINTNL 	$(hide)$(CCENV) $(CXX) -Wall $(CXXFLAGS) $(CCFLAGS) -c $< -o $@
  ENDIF
  PRINTNL
ENDIF

FOR EACH SOURCE
  IF (%(w)!="") && (%(>NAME)==c)
    PRINTNL $(BUILD)%(¯NAME:%.c=%).%(../NAME).o: %(!NAME)
    PRINTNL 	$(hide)echo compiling $<
    PRINTNL 	$(hide)mkdir -p $(BUILD)%(¯/NAME)
    PRINTNL 	$(hide)$(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -w -c $< -o $@ -MMD -MP -MF $@.d
    PRINTNL 
  ENDIF
  IF (%(w)!="") && (%(>NAME)==cpp)
    PRINTNL $(BUILD)%(¯NAME:%.cpp=%).%(../NAME).o: %(!NAME)
    PRINTNL 	$(hide)echo compiling $<
    PRINTNL 	$(hide)mkdir -p $(BUILD)%(¯/NAME)
    PRINTNL 	$(hide)$(CCENV) $(CXX) $(CXXFLAGS) $(CCFLAGS) -w -c $< -o $@ -MMD -MP -MF $@.d
    PRINTNL 
  ENDIF
ENDFOR

/*
FOR EACH USE
//    PRINTNL %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_LIB):
  PRINTNL %(NAME):
  PRINTNL 	$(hide)$(MAKE) -C %(¯!§PATH) -f %(NAME).mak
  PRINTNL 
ENDFOR
*/

PRINTNL install: $(OUTPUT)
IF (%(TYPE)==EXE || %(TYPE)==CON)
  PRINTNL 	$(hide)install -c $(OUTPUT) /usr/local/bin
ENDIF
IF %(TYPE)==DLL
  PRINTNL 	$(hide)install -c $(OUTPUT) /usr/local/lib
ENDIF
PRINTNL 

PRINTNL clean:
PRINTNL 	$(hide)rm -f $(OBJS)
PRINTNL 	$(hide)rm -f $(OUTPUT)
IF "%(PRECOMPILED_HEADER)"!=""
  PRINTNL 	$(hide)rm -f %(!PATH)%(PRECOMPILED_HEADER).gch %(PRECOMPILED_HEADER).gch.%(NAME)
ENDIF
PRINTNL

PRINTNL cleandep:
PRINTNL 	$(hide)rm -f $(DEPS)
IF !defined(TARGET_PS2SDK) // too noisy
  FOR EACH USE
    IF (!%(NOLIB)) && (%(NAME)!=%(../NAME))
      PRINTNL 	$(hide)$(MAKE) -C %(¯!§PATH) -f %(NAME).mak cleandep
    ENDIF
  ENDFOR
ENDIF
PRINTNL 

PRINTNL distclean: cleandep
PRINTNL 	$(hide)rm -f $(OBJS) $(LIBS)
PRINTNL 	$(hide)rm -f $(OUTPUT)
FOR EACH USE
  IF (!%(NOLIB))
    PRINTNL 	$(hide)$(MAKE) -C %(¯!§PATH) -f %(NAME).mak distclean
  ENDIF
ENDFOR
IF "%(PRECOMPILED_HEADER)"!=""
  PRINTNL 	$(hide)rm -f %(!PATH)%(PRECOMPILED_HEADER).gch %(PRECOMPILED_HEADER).gch.%(NAME)
ENDIF
PRINTNL

IFDEF TARGET_QTOPIA
  IF %(TYPE)==EXE
    PRINTNL install:
    PRINTNL 	$(hide)echo Installing %(NAME)...
    PRINTNL 	$(hide)cp -f "%(!PATH)%(NAME).desktop" "$$QPEDIR/image/apps/Applications/"
    PRINTNL 	$(hide)cp -f "%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(NAME)" "$$QPEDIR/image/bin/%(NAME)"
    IF %(ICON)
      PRINTNL 	$(hide)$$QPEDIR/bin/installpic "en_US" 14 16 22 %(!PATH) $$QPEDIR/image/pics/%(NAME) %(!ICON)_32x32.png
    ENDIF
    PRINTNL 
  ENDIF
ENDIF

PRINTNL -include $(DEPS)
PRINTNL 
