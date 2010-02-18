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
  PRINTNL BUILDDEP = %(!BUILDPATH)deps/
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
  PRINTNL CCFLAGS += %(CCFLAGS) %(DEFINE:%=-D%) -I . %(@INCLUDE:%=-I '%') %(@SYSINCLUDE:%=-I '%') %(@CONFIG_INCLUDE:%=-I '%')
  PRINTNL CFLAGS += %(CFLAGS)
  IF "%(UI_FORM_QT4)"!=""
    PRINTNL CCFLAGS += -I %(!BUILDPATH)%(PATH)GeneratedFiles
  ENDIF
  PRINTNL CXXFLAGS += %(CXXFLAGS) -fno-for-scope
  PRINTNL ASMFLAGS += %(ASMFLAGS) %(DEFINE:%=-D%)
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

  IF "%(USELIB)"
    LIBS += %(USELIB:%=%_%(PLATFORMLIB).lib)
  ENDIF

  PRINTNL LFLAGS += $(LIBS) %(FRAMEWORK:%=-framework %) %(<USEBUILT:%=-l%) %(RPATH:%=-Wl,-rpath,%) %(@LIBINCLUDE:%=-L'%') %(<LIBS:%=-l%) %(LFLAGS) %(¯!OUTPUTPATH:%=-L'%')

  IF "%(CONFIG_QTOPIA_PREFIX)"!=""
    PRINTNL LFLAGS += -Wl,-rpath,%(CONFIG_QTOPIA_PREFIX)/lib
  ENDIF

  IF defined(TARGET_PALMOS) && (%(TYPE)!=lib) && !%(NOLIB)
    PRINTNL LFLAGS += -Wl,--no-whole-archive
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

  PRINTNL DEPS:=$(SOURCE:%.c=$(BUILDDEP)%.%(NAME).d)
  PRINTNL DEPS:=$(DEPS:%.cpp=$(BUILDDEP)%.%(NAME).d)
  PRINTNL DEPS:=$(DEPS:%.S=$(BUILDDEP)%.%(NAME).d)
  PRINTNL DEPS:=$(DEPS:%.m=$(BUILDDEP)%.%(NAME).d)
  PRINTNL DEPS:=$(DEPS:%.tab=$(BUILDDEP)%.%(NAME).d)
  PRINTNL DEPS:=$(DEPS:%.lst=$(BUILDDEP)%.%(NAME).d)
  PRINTNL DEPS:=$(DEPS:%.rc=)
  PRINTNL DEPS:=$(DEPS:%.nas=)
  PRINTNL DEPS:=$(DEPS:%.asm=)
  PRINTNL DEPS:=$(DEPS:%.vsm=)
  PRINTNL DEPS:=$(DEPS:%.icn=)

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
    PRINTNL GENERATED += %(PRECOMPILED_HEADER).gch
  ENDIF
  PRINTNL 

  PRINTNL OBJS:=$(SOURCE:%(!ROOTPATH)%.c=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%.c=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.cpp=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%.cpp=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.rc=$(BUILD)%_res.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%.rc=$(BUILD)%_res.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.S=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%.S=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.nas=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%.nas=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%(!ROOTPATH)%.m=$(BUILD)%.%(NAME).o)
  PRINTNL OBJS:=$(OBJS:%.m=$(BUILD)%.%(NAME).o)
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

  PRINTNL ifeq ($(MAKECMDGOALS),depend)
  PRINTNL .PHONY: all prebuild postbuild install clean distclean depend cleandep $(LIBS)
  PRINTNL else
  PRINTNL .PHONY: all prebuild postbuild install clean distclean depend cleandep $(LIBS) $(DEPS)
  PRINTNL endif
  PRINTNL 
  PRINTNL all: prebuild $(GENERATED) $(OUTPUT) postbuild
  PRINTNL 
  PRINTNL prebuild:
//  PRINTNL 	@echo prebuild
  IF "%(PRECOMPILED_HEADER)"!=""
    PRINTNL 	@-test ! -f %(PRECOMPILED_HEADER).gch.%(NAME) || mv %(PRECOMPILED_HEADER).gch.%(NAME) %(PRECOMPILED_HEADER).gch
  ENDIF
  PRINTNL 
  PRINTNL postbuild:
//  PRINTNL 	@echo postbuild
  IF "%(PRECOMPILED_HEADER)"!=""
    PRINTNL 	@-test ! -f %(PRECOMPILED_HEADER).gch || mv %(PRECOMPILED_HEADER).gch %(PRECOMPILED_HEADER).gch.%(NAME)
  ENDIF
  PRINTNL 

  IF defined(TARGET_PALMOS)
    IF (%(TYPE)==lib)
      PRINTNL $(OUTPUT): $(OBJS) 
      PRINTNL 	@echo linking
      PRINTNL 	@mkdir -p %(¯!OUTPUTPATH)%(OUTPUT_DIR)
      PRINTNL 	@$(AR) rcs $@ $(OBJS)
    ELSE
      PRINTNL $(OUTPUT): %(!BUILDPATH)%(NAME)_arm.ro %(¯!PATH)%(NAME)_palmos.def %(OUTPUT_M68K) $(RES_M68K) $(PEAL)
      PRINTNL 	@mkdir -p %(¯!OUTPUTPATH)
      IF !%(NOLIB)
      PRINTNL 	@$(PEAL) -l %(NAME) -o %(¯!OUTPUTPATH)lib%(NAME).a %(!BUILDPATH)%(NAME).elf
      ENDIF
      PRINTNL 	@build-prc %(¯!PATH)%(NAME)_palmos.def -o $@ %(!BUILDPATH)%(NAME)_arm.ro %(OUTPUT_M68K) $(RES_M68K)
      IF ("%(OUTPUT)"=="%(NAME).prc") && ("%(PROJECT_NAME)"!="%(NAME)")
        PRINTNL 	@cp $(OUTPUT) "%(¯!OUTPUTPATH)%(PROJECT_NAME).prc"
      ENDIF
      IF %(TYPE)=exe
        FOR EACH USE
          IF %(TYPE)=dll
            ../INSTALL_ZIP += "%(¯!OUTPUTPATH)%(PROJECT_NAME).prc"
          ENDIF
        ENDFOR
        IF "%(INSTALL_ZIP)"!=""
          INSTALL_ZIP += "%(¯!OUTPUTPATH)%(PROJECT_NAME).prc"
          PRINTNL 	@zip -X -q -o "%(¯!OUTPUTPATH)%(PROJECT_NAME).zip" %(INSTALL_ZIP)
        ENDIF
      ENDIF
      PRINTNL
      PRINTNL %(!BUILDPATH)%(NAME)_arm.ro: %(!BUILDPATH)%(NAME).elf $(PEAL)
      PRINTNL 	@$(PEAL) $(PFLAGS) -o $@ %(!BUILDPATH)%(NAME).elf
      PRINTNL
      PRINTNL %(!BUILDPATH)%(NAME).m68k: $(OBJS_M68K)
      PRINTNL 	@echo linking m68k
      PRINTNL 	@mkdir -p %(!BUILDPATH)
      PRINTNL 	@$(CC_M68K) $(CFLAGS_M68K) -static $(OBJS_M68K) $(LFLAGS_M68K) -o $@
      PRINTNL 
      PRINTNL %(!BUILDPATH)%(NAME).elf: $(OBJS) $(LIBS)
      PRINTNL 	@echo linking arm
      PRINTNL 	@mkdir -p %(!BUILDPATH)
      PRINTNL 	@$(CCENV) $(CC) $(OBJS) -o %(!BUILDPATH)%(NAME).elf $(LFLAGS) -Wl,-Map=%(!BUILDPATH)%(NAME).map
      PRINTNL 
      PRINTNL $(PEAL): $(PEALSRC)
      PRINTNL 	@echo compiling peal-postlink
      PRINTNL 	@$(CXX_HOST) $(PEALSRC) -o $(PEAL)
      PRINTNL 	@chmod u+x $(PEAL)
    ENDIF
    PRINTNL 
    PRINTNL $(BUILD_M68K)%.%(NAME).o: %.c
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD_M68K)$(*D)  
    PRINTNL 	@$(CC_M68K) -Wall $(CFLAGS_M68K) -c $< -o $@
    PRINTNL 
    PRINTNL $(BUILD_M68K)%.ro: %.rcp
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD_M68K)$(*D)  
    PRINTNL 	@pilrc -I $(*D) -q -ro $< $@
    PRINTNL 

    FOR EACH INSTALL
      PRINTNL $(BUILD_M68K)%(¯!NAME).ro: %(!NAME)
      PRINTNL 	@echo compiling $<
      PRINTNL 	@mkdir -p $(BUILD_M68K)%(¯/NAME) 
      PRINTNL 	@%(../NAME)_split.sh %(!NAME) 64512 %(,NAME) $(BUILD_M68K)%(¯!NAME)
      PRINTNL 	@pilrc -q -ro $(BUILD_M68K)%(¯!NAME).rcp $@
      PRINTNL 
    ENDFOR

  ELSE
    IFNDEF TARGET_QTOPIA
      IFNDEF RESOURCE_COREC
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

    PRINTNL $(OUTPUT): $(OBJS) $(LIBS) $(USEBUILT) $(ICON) $(INSTALL) $(PLIST)
    PRINTNL 	@echo linking
    PRINTNL 	@mkdir -p %(¯!OUTPUTPATH)%(OUTPUT_DIR)
    IF (%(TYPE)==lib)
      PRINTNL 	@$(AR) rcs $@ $(OBJS)
      IF "%(EXPORT_SVN)"
        PRINTNL 	@echo exporting for SVN
        IF "%(HOST_WIN32)"!=""
          PRINTNL 	@if not exist "%(!EXPORT_SVN)export/lib/%(SVNDIR)/static/" mkdir -p "%(!EXPORT_SVN)export/lib/%(SVNDIR)/static/"
        ELSE
          PRINTNL 	@test -d "%(!EXPORT_SVN)export/lib/%(SVNDIR)/static/" || mkdir -p "%(!EXPORT_SVN)export/lib/%(SVNDIR)/static/"
        ENDIF
        PRINTNL 	@cp -f "$@" "%(!EXPORT_SVN)export/lib/%(SVNDIR)/static/"
      ENDIF
    ELSE
      PRINTNL 	@$(CXX) $(CCFLAGS) $(OBJS) -o $@ $(LFLAGS) 
      //-Wl,-Map=%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(NAME).map
      IF "%(STRIP)"
        PRINTNL 	@%(STRIP) %(STRIPFLAGS) $@
      ENDIF
      IF (%(TYPE)==dll)
        IF defined(TARGET_WIN32) && "%(EXPORT_MSVC_LIB)"!=""
          PRINTNL 	@lib -machine:i386 -def:"%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_DLL_DEF)" -out:"%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(<NAME).lib"
        ENDIF
        IF "%(EXPORT_SVN)"
          PRINTNL 	@echo exporting for SVN
          IF "%(HOST_WIN32)"!=""
            PRINTNL 	@if not exist "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/" mkdir -p "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
          ELSE
            PRINTNL 	@test -d "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/" || mkdir -p "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
          ENDIF
          PRINTNL 	@cp -f "$@" "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
          PRINTNL 	@cp -f "%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_DLL_LIB)" "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
          IF "%(OUTPUT_DLL_DEF)"!=""
            PRINTNL 	@cp -f "%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_DLL_DEF)" "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
          ENDIF
          IF defined(TARGET_WIN32) && "%(EXPORT_MSVC_LIB)"!=""
            PRINTNL 	@cp -f "%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(<NAME).lib" "%(!EXPORT_SVN)export/lib/%(SVNDIR)/dynamic/"
          ENDIF
        ENDIF
      ENDIF
    ENDIF
    IF "%(TRANSLATION_QT4)"!=""
      PRINTNL 	@echo processing translations
      PRINTNL 	@"$(QTDIR)/bin/lupdate" %(NAME).tmp.pro
      PRINTNL 	@"$(QTDIR)/bin/lrelease" %(NAME).tmp.pro
      FOR EACH TRANSLATION_QT4
        PRINTNL 	@cp -f "%(?NAME)%(<NAME:%=%.qm)" "%(¯!OUTPUTPATH)%(OUTPUT_DIR)"
      ENDFOR
    ENDIF

    IF %(PLIST_FILE)!=""
      PRINTNL
      PRINTNL %(!PLIST_PATH)/Info.plist: %(!PLIST_FILE)
      PRINTNL 	@echo copy $<
      PRINTNL 	@mkdir -p %(!PLIST_PATH)
      PRINTNL 	@cp -f $< $@
    ENDIF

    IFNDEF TARGET_QTOPIA
      IFDEF TARGET_IPHONE
        IF %(ICON)
          PRINTNL 
          PRINTNL %(!RESOURCEPATH)icon.png: %(!ICON)_iphone.png
          PRINTNL 	@echo copy $<
          PRINTNL 	@mkdir -p %(!RESOURCEPATH)
          PRINTNL 	@cp -f $< $@
        ENDIF
      ENDIF
      IFDEF RESOURCE_COREC 
        IF (%(TYPE)!=lib)
          PRINTNL 
          FOR EACH INSTALL
            PRINTNL $(BUILD)%(NAME).%(../NAME).o: %(!NAME)
            PRINTNL 	@echo compiling $<
            DIRPUSH %(!BUILDDIR)
            PRINTNL 	@mkdir -p $(BUILD)%(¯!/NAME)
            DIRPOP
            PRINTNL 	@corerc -corec $(CORERCFLAGS) $< | $(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -w -x c -c - -o $@
            PRINTNL 
          ENDFOR
        ENDIF
      ELSE
        PRINTNL 
        FOR EACH INSTALL
          PRINTNL 
          PRINTNL %(!RESOURCEPATH)%(<NAME).%(>NAME): %(!NAME)
          PRINTNL 	@echo copy $<
          PRINTNL 	@mkdir -p %(!RESOURCEPATH)
          PRINTNL 	@cp -f $< $@
        ENDFOR
      ENDIF
    ENDIF

  ENDIF

    IF "%(USEBUILT)" || "%(USE)"
      PRINTNL
      FOR EACH USEBUILT
        PRINTNL %(NAME):
        PRINTNL 	@$(MAKE) -C %(¯!PATH) -f %(NAME).mak
      ENDFOR
      IF !defined(TARGET_PS2SDK) // otherwise it has too many references
        FOR EACH USE
          IF (%(TYPE)==dll) && !defined(TARGET_PALMOS)
            PRINTNL %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT):
            PRINTNL 	@$(MAKE) -C %(¯!PATH) -f %(NAME).mak
          ELSE
            PRINTNL %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_LIB):
            PRINTNL 	@$(MAKE) -C %(¯!PATH) -f %(NAME).mak
          ENDIF
        ENDFOR
      ENDIF
    ENDIF

  IF "%(PRECOMPILED_HEADER)"!=""
    PRINTNL
    PRINTNL $(OBJS): %(PRECOMPILED_HEADER).gch
  ENDIF
  
  PRINTNL 
  IF ("%(WINDRES)"!="")
    PRINTNL $(BUILD)%_res.%(NAME).o: %(!ROOTPATH)%.rc
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)  
    PRINTNL 	@$(WINDRES) -F pe-i386 -I$(*D) $< $@
    PRINTNL 
    PRINTNL $(BUILD)%_res.%(NAME).o: %.rc
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)  
    PRINTNL 	@$(WINDRES) -F pe-i386 -I$(*D) $< $@
    PRINTNL 
  ENDIF
  PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.c
  PRINTNL 	@echo compiling $<
  PRINTNL 	@mkdir -p $(BUILD)$(*D)
  PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %.c
  PRINTNL 	@echo compiling $<
  PRINTNL 	@mkdir -p $(BUILD)$(*D)
  PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@
  PRINTNL 
  FOR EACH FUNC_C
    PRINTNL $(BUILD)%.%(NAME).%(../NAME).o: %(!ROOTPATH)%.c
    PRINTNL 	@echo compiling $< %(NAME)
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -DF_%(NAME) -c $< -o $@
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).%(../NAME).o: %.c
    PRINTNL 	@echo compiling $< %(NAME)
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -DF_%(NAME) -c $< -o $@
    PRINTNL 
  ENDFOR
  PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.m
  PRINTNL 	@echo compiling $<
  PRINTNL 	@mkdir -p $(BUILD)$(*D)
  PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %.m
  PRINTNL 	@echo compiling $<
  PRINTNL 	@mkdir -p $(BUILD)$(*D)
  PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.S
  PRINTNL 	@echo compiling $<
  PRINTNL 	@mkdir -p $(BUILD)$(*D)
  PRINTNL 	@$(CCENV) $(CC) -Wall $(SFLAGS) -c $< -o $@
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %.S
  PRINTNL 	@echo compiling $<
  PRINTNL 	@mkdir -p $(BUILD)$(*D)
  PRINTNL 	@$(CCENV) $(CC) -Wall $(SFLAGS) -c $< -o $@
  PRINTNL 
  FOR EACH FUNC_S
    PRINTNL $(BUILD)%.%(NAME).%(../NAME).o: %(!ROOTPATH)%.S
    PRINTNL 	@echo compiling $< %(NAME)
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(CCENV) $(CC) -Wall $(SFLAGS) -DF_%(NAME) -c $< -o $@
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).%(../NAME).o: %.S
    PRINTNL 	@echo compiling $< %(NAME)
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(CCENV) $(CC) -Wall $(SFLAGS) -DF_%(NAME) -c $< -o $@
    PRINTNL 
  ENDFOR
  PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.cpp
  PRINTNL 	@echo compiling $<
  PRINTNL 	@mkdir -p $(BUILD)$(*D)
  PRINTNL 	@$(CCENV) $(CXX) -Wall $(CXXFLAGS) $(CCFLAGS) -c $< -o $@
  PRINTNL 
  PRINTNL $(BUILD)%.%(NAME).o: %.cpp
  PRINTNL 	@echo compiling $<
  PRINTNL 	@mkdir -p $(BUILD)$(*D)
  PRINTNL 	@$(CCENV) $(CXX) -Wall $(CXXFLAGS) $(CCFLAGS) -c $< -o $@
  PRINTNL 
  IF (%(ASM)!="")
    PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.asm
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(ASM) $(ASMFLAGS) -o $@ $<
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).o: %.asm
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(ASM) $(ASMFLAGS) -o $@ $<
    PRINTNL 
  ENDIF
  IF (%(NASM)!="")
    PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.nas
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(NASM) $(NASMFLAGS) -o $@ $<
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).o: %.nas
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(NASM) $(NASMFLAGS) -o $@ $<
    PRINTNL 
  ENDIF
  IF defined(TARGET_PS2SDK)
    PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.vsm
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(CCENV) %(DVP_AS) --warn -I $(<D) $< -o $@
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).o: %.vsm
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(CCENV) %(DVP_AS) --warn -I $(<D) $< -o $@
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.icn
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@corerc -n config_icon $(CORERCFLAGS) $< | $(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -w -x c -c - -o $@
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).o: %.icn
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@corerc -n config_icon $(CORERCFLAGS) $< | $(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -w -x c -c - -o $@
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.tab
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -x c -include $(<D)/irx_imports.h -c $< -o $@
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).o: %.tab
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -x c -include $(<D)/irx_imports.h -c $< -o $@
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).o: %(!ROOTPATH)%.lst
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -x c -include $(<D)/irx_imports.h -c $< -o $@
    PRINTNL 
    PRINTNL $(BUILD)%.%(NAME).o: %.lst
    PRINTNL 	@echo compiling $<
    PRINTNL 	@mkdir -p $(BUILD)$(*D)
    PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -x c -include $(<D)/irx_imports.h -c $< -o $@
    PRINTNL 
  ENDIF
  IF "%(UI_FORM_QT4)"!=""
    FOR EACH UI_FORM_QT4
      PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/ui_%.h: %(!NAME)
    ENDFOR
//    PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/ui_%.h: %.ui
    PRINTNL 	@echo UICing $<
    PRINTNL 	@mkdir -p %(!BUILDPATH)%(PATH)GeneratedFiles/$(*D)
    PRINTNL 	@"$(QTDIR)/bin/uic" -o $@ $<
    PRINTNL 
  ENDIF
  IF "%(RESOURCE_QT4)"!=""
    FOR EACH RESOURCE_QT4
      PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/qrc_%.cpp: %(!NAME)
    ENDFOR
//    PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/qrc_%.cpp: %.qrc
    PRINTNL 	@echo RCCing $<
    PRINTNL 	@mkdir -p $(BUILD)GeneratedFiles/$(*D)
    PRINTNL 	@"$(QTDIR)/bin/rcc" -name $< -o $@ $<
    PRINTNL 
  ENDIF
  IF "%(HEADER_QT4)"!=""
    FOR EACH HEADER_QT4
      PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/moc_%.cpp: %(!NAME)
    ENDFOR
//    PRINTNL %(!BUILDPATH)%(PATH)GeneratedFiles/moc_%.cpp: %.h
    PRINTNL 	@echo MOCing $<
    PRINTNL 	@mkdir -p $(BUILD)GeneratedFiles/$(*D)
    PRINTNL 	@"$(QTDIR)/bin/moc" %(@INCLUDE:%=-I '%') %(@SYSINCLUDE:%=-I '%')  %(@CONFIG_INCLUDE:%=-I '%') $< -o $@
    PRINTNL 
  ENDIF
  IF "%(PRECOMPILED_HEADER)"!=""
    PRINTNL %.%(>PRECOMPILED_HEADER).gch: %.%(>PRECOMPILED_HEADER)
    PRINTNL 	@echo precompiling header $<
    PRINTNL 	@-rm -f $@
    IF "%(>SOURCE)"=="c"
      PRINTNL 	@$(CCENV) $(CC) -Wall $(CFLAGS) $(CCFLAGS) -c $< -o $@
    ELSE
      PRINTNL 	@$(CCENV) $(CXX) -Wall $(CXXFLAGS) $(CCFLAGS) -c $< -o $@
    ENDIF
    PRINTNL
  ENDIF

  FOR EACH SOURCE
    IF (%(w)!="") && (%(>NAME)==c)
      PRINTNL $(BUILD)%(¯NAME:%.c=%).%(../NAME).o: %(!NAME)
      PRINTNL 	@echo compiling $<
      PRINTNL 	@mkdir -p $(BUILD)%(¯/NAME)
      PRINTNL 	@$(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -w -c $< -o $@
      PRINTNL 
    ENDIF
    IF (%(w)!="") && (%(>NAME)==cpp)
      PRINTNL $(BUILD)%(¯NAME:%.cpp=%).%(../NAME).o: %(!NAME)
      PRINTNL 	@echo compiling $<
      PRINTNL 	@mkdir -p $(BUILD)%(¯/NAME)
      PRINTNL 	@$(CCENV) $(CXX) $(CXXFLAGS) $(CCFLAGS) -w -c $< -o $@
      PRINTNL 
    ENDIF
  ENDFOR

/*
  FOR EACH USE
//    PRINTNL %(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(OUTPUT_LIB):
    PRINTNL %(NAME):
    PRINTNL 	@$(MAKE) -C %(¯!PATH) -f %(NAME).mak
    PRINTNL 
  ENDFOR
*/

  PRINTNL $(BUILDDEP)%.%(NAME).d: %.c
  PRINTNL 	@echo generating dependency for $<
  PRINTNL 	@mkdir -p $(BUILDDEP)$(*D)
  PRINTNL 	@$(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -w $< -MM -MT '$$(BUILD)$*.%(NAME).o $$(BUILDDEP)$*.%(NAME).d' > $@
  PRINTNL 

  PRINTNL $(BUILDDEP)%.%(NAME).d: %.cpp
  PRINTNL 	@echo generating dependency for $<
  PRINTNL 	@mkdir -p $(BUILDDEP)$(*D)
  PRINTNL 	@$(CCENV) $(CXX) $(CFLAGS) $(CXXFLAGS) -w $< -MM -MT '$$(BUILD)$*.%(NAME).o $$(BUILDDEP)$*.%(NAME).d' > $@
  PRINTNL 

  PRINTNL $(BUILDDEP)%.%(NAME).d: %.S
  PRINTNL 	@echo generating dependency for $<
  PRINTNL 	@mkdir -p $(BUILDDEP)$(*D)
  PRINTNL 	@$(CCENV) $(CC) $(SFLAGS) -w $< -MM -MT '$$(BUILD)$*.%(NAME).o $$(BUILDDEP)$*.%(NAME).d' > $@
  PRINTNL 

  PRINTNL $(BUILDDEP)%.%(NAME).d: %.m
  PRINTNL 	@echo generating dependency for $<
  PRINTNL 	@mkdir -p $(BUILDDEP)$(*D)
  PRINTNL 	@$(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -w $< -MM -MT '$$(BUILD)$*.%(NAME).o $$(BUILDDEP)$*.%(NAME).d' > $@
  PRINTNL 

  PRINTNL $(BUILDDEP)%.%(NAME).d: %.tab
  PRINTNL 	@echo generating dependency for $<
  PRINTNL 	@mkdir -p $(BUILDDEP)$(*D)
  PRINTNL 	@$(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -x c -include $(<D)/irx_imports.h -w $< -MM -MT '$$(BUILD)$*.%(NAME).o $$(BUILDDEP)$*.%(NAME).d' > $@
  PRINTNL 

  PRINTNL $(BUILDDEP)%.%(NAME).d: %.lst
  PRINTNL 	@echo generating dependency for $<
  PRINTNL 	@mkdir -p $(BUILDDEP)$(*D)
  PRINTNL 	@$(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -x c -include $(<D)/irx_imports.h -w $< -MM -MT '$$(BUILD)$*.%(NAME).o $$(BUILDDEP)$*.%(NAME).d' > $@
  PRINTNL 

  FOR EACH FUNC_C
    PRINTNL $(BUILDDEP)%.%(NAME).%(../NAME).d: %.c
    PRINTNL 	@echo generating dependency for $< %(NAME)
    PRINTNL 	@mkdir -p $(BUILDDEP)$(*D)
    PRINTNL 	@$(CCENV) $(CC) $(CFLAGS) $(CCFLAGS) -DF_%(NAME) -w $< -MM -MT '$$(BUILD)$*.%(NAME).o $$(BUILDDEP)$*.%(NAME).%(../NAME).d' > $@
    PRINTNL 
  ENDFOR
  FOR EACH FUNC_S
    PRINTNL $(BUILDDEP)%.%(NAME).%(../NAME).d: %.S
    PRINTNL 	@echo generating dependency for $< %(NAME)
    PRINTNL 	@mkdir -p $(BUILDDEP)$(*D)
    PRINTNL 	@$(CCENV) $(CC) $(SFLAGS) -DF_%(NAME) -w $< -MM -MT '$$(BUILD)$*.%(NAME).o $$(BUILDDEP)$*.%(NAME).%(../NAME).d' > $@
    PRINTNL 
  ENDFOR

  PRINTNL clean:
  PRINTNL 	@rm -f $(OBJS)
  PRINTNL 	@rm -f $(OUTPUT)
  IF "%(PRECOMPILED_HEADER)"!=""
    PRINTNL 	@rm -f %(PRECOMPILED_HEADER).gch %(PRECOMPILED_HEADER).gch.%(NAME)
  ENDIF
  PRINTNL

  PRINTNL distclean:
  PRINTNL 	@rm -f $(OBJS) $(LIBS)
  PRINTNL 	@rm -f $(OUTPUT)
  FOR EACH USE
    IF (!%(NOLIB))
      PRINTNL 	@$(MAKE) -C %(¯!PATH) -f %(NAME).mak distclean
    ENDIF
  ENDFOR
  IF "%(PRECOMPILED_HEADER)"!=""
    PRINTNL 	@rm -f %(PRECOMPILED_HEADER).gch %(PRECOMPILED_HEADER).gch.%(NAME)
  ENDIF
  PRINTNL

  IFDEF TARGET_QTOPIA
    IF %(TYPE)==EXE
      PRINTNL install:
      PRINTNL 	@echo Installing %(NAME)...
      PRINTNL 	@cp -f "%(!PATH)%(NAME).desktop" "$$QPEDIR/image/apps/Applications/"
      PRINTNL 	@cp -f "%(¯!OUTPUTPATH)%(+OUTPUT_DIR)%(NAME)" "$$QPEDIR/image/bin/%(NAME)"
      IF %(ICON)
        PRINTNL 	@$$QPEDIR/bin/installpic "en_US" 14 16 22 %(!PATH) $$QPEDIR/image/pics/%(NAME) %(!ICON)_32x32.png
      ENDIF
      PRINTNL 
    ENDIF
  ENDIF

  PRINTNL cleandep:
  PRINTNL 	@rm -f $(DEPS)
  IF !defined(TARGET_PS2SDK) // too noisy
    FOR EACH USE
      IF (!%(NOLIB)) && (%(NAME)!=%(../NAME))
        PRINTNL 	@$(MAKE) -C %(¯!PATH) -f %(NAME).mak cleandep
      ENDIF
    ENDFOR
  ENDIF
  PRINTNL 
  PRINTNL depend: $(DEPS)
  IF !defined(TARGET_PS2SDK) // too noisy
    FOR EACH USE
      IF (!%(NOLIB)) && (%(NAME)!=%(../NAME))
        PRINTNL 	@$(MAKE) -C %(¯!PATH) -f %(NAME).mak depend
      ENDIF
    ENDFOR
  ENDIF
  PRINTNL 
  PRINTNL -include $(DEPS)
  PRINTNL 
