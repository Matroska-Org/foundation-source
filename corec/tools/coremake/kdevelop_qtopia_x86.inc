#define IX86

#define ASM yasm

#define ASMFLAGS -f elf
#define LFLAGS

#define TARGET_LINUX
#define TARGET_QTOPIA

// at least to remove the dependency on iconv
#define GLIBC

#define COMPILER_GCC

#define OUTPUT_EXE       %(NAME)
#define OUTPUT_CON       %(NAME)
#define OUTPUT_DLL       lib%(NAME).so
#define OUTPUT_DLL_LIB   lib%(NAME).so
#define OUTPUT_LIB       lib%(NAME).a

CONFIG

IF "%(QPEDIR)"!=""
#define QTDIR %(QPEDIR)/qtopiacore/target
ENDIF

FOR EACH WORKSPACE
  FILE %(¯PATH)%(NAME).kdevelop
  PRINTNL <?xml version = '1.0'?>
  PRINTNL <kdevelop>
  PRINTNL   <general>
  PRINTNL     <version>$VERSION$</version>
  PRINTNL     <projectmanagement>KDevTrollProject</projectmanagement>
  PRINTNL     <primarylanguage>C++</primarylanguage>
  PRINTNL     <keywords>
  PRINTNL       <keyword>Qt</keyword>
  PRINTNL     </keywords>
  PRINTNL     <ignoreparts/>
  PRINTNL   </general>
  PRINTNL   <kdevfileview>
  PRINTNL     <groups>
  PRINTNL       <group pattern="*.cpp;*.cxx;*.h;*.c" name="Sources" />
  PRINTNL       <group pattern="*.ui" name="User Interface" />
  PRINTNL       <group pattern="*.png" name="Icons" />
  PRINTNL       <group pattern="*.desktop" name="Desktop Entry"/>
  PRINTNL       <group pattern="*.proj" name="CoreMake project"/>
  PRINTNL       <group pattern="*" name="Others" />
  PRINTNL       <hidenonprojectfiles>false</hidenonprojectfiles>
  PRINTNL       <hidenonlocation>false</hidenonlocation>
  PRINTNL     </groups>
  PRINTNL     <tree>
  PRINTNL       <hidenonprojectfiles>false</hidenonprojectfiles>
  PRINTNL       <hidepatterns>*.o,*.lo,CVS</hidepatterns>
  PRINTNL     </tree>
  PRINTNL   </kdevfileview>
  PRINTNL   <kdevdoctreeview>
  PRINTNL     <ignoretocs>
  PRINTNL       <toc>ada</toc>
  PRINTNL       <toc>ada_bugs_gcc</toc>
  PRINTNL       <toc>bash</toc>
  PRINTNL       <toc>bash_bugs</toc>
  PRINTNL       <toc>clanlib</toc>
  PRINTNL       <toc>fortran_bugs_gcc</toc>
  PRINTNL       <toc>gnome1</toc>
  PRINTNL       <toc>gnustep</toc>
  PRINTNL       <toc>gtk</toc>
  PRINTNL       <toc>gtk_bugs</toc>
  PRINTNL       <toc>haskell</toc>
  PRINTNL       <toc>haskell_bugs_ghc</toc>
  PRINTNL       <toc>java_bugs_gcc</toc>
  PRINTNL       <toc>java_bugs_sun</toc>
  PRINTNL       <toc>kde2book</toc>
  PRINTNL       <toc>libstdc++</toc>
  PRINTNL       <toc>opengl</toc>
  PRINTNL       <toc>pascal_bugs_fp</toc>
  PRINTNL       <toc>php</toc>
  PRINTNL       <toc>php_bugs</toc>
  PRINTNL       <toc>perl</toc>
  PRINTNL       <toc>perl_bugs</toc>
  PRINTNL       <toc>python</toc>
  PRINTNL       <toc>python_bugs</toc>
  PRINTNL       <toc>qt-kdev3</toc>
  PRINTNL       <toc>ruby</toc>
  PRINTNL       <toc>ruby_bugs</toc>
  PRINTNL       <toc>sdl</toc>
  PRINTNL       <toc>stl</toc>
  PRINTNL       <toc>sw</toc>
  PRINTNL       <toc>w3c-dom-level2-html</toc>
  PRINTNL       <toc>w3c-svg</toc>
  PRINTNL       <toc>w3c-uaag10</toc>
  PRINTNL       <toc>wxwidgets_bugs</toc>
  PRINTNL     </ignoretocs>
  PRINTNL     <ignoreqt_xml>
  PRINTNL       <toc>Guide to the Qt Translation Tools</toc>
  PRINTNL       <toc>Qt Assistant Manual</toc>
  PRINTNL       <toc>Qt Designer Manual</toc>
  PRINTNL       <toc>Qt Reference Documentation</toc>
  PRINTNL       <toc>qmake User Guide</toc>
  PRINTNL     </ignoreqt_xml>
  PRINTNL     <ignoredoxygen>
  PRINTNL       <toc>KDE Libraries (Doxygen)</toc>
  PRINTNL     </ignoredoxygen>
  PRINTNL   </kdevdoctreeview>
  PRINTNL   <kdevdebugger>
  PRINTNL     <general>
  PRINTNL       <dbgshell>/usr/bin/libtool</dbgshell>
  PRINTNL       <programargs/>
  IF "%(KDEVELOP_GDBPATH)"!=""
    PRINTNL       <gdbpath>%(KDEVELOP_GDBPATH)</gdbpath>
  ELSE
    PRINTNL       <gdbpath>/usr/bin/</gdbpath>
  ENDIF
  PRINTNL       <breakonloadinglibs>true</breakonloadinglibs>
  PRINTNL       <separatetty>false</separatetty>
  PRINTNL       <floatingtoolbar>false</floatingtoolbar>
  IF "%(KDEVELOP_GDBSCRIPT)"!=""
  PRINTNL       <runGdbScript>%(|PATH)%(NAME).kdevelop_gdbscript</runGdbScript>
  ENDIF
  PRINTNL     </general>
  PRINTNL     <display>
  PRINTNL       <staticmembers>false</staticmembers>
  PRINTNL       <demanglenames>true</demanglenames>
  PRINTNL     </display>
  PRINTNL   </kdevdebugger>
  PRINTNL   <kdevtrollproject>
  PRINTNL     <general>
  PRINTNL       <activedir></activedir>
  PRINTNL     </general>
  PRINTNL     <run>
//  PRINTNL       <mainprogram>./kdev2</mainprogram>
  PRINTNL       <programargs/>
  PRINTNL       <envvars>
  PRINTNL         <envvar value="%(QTDIR)" name="QTDIR" />
  IF %(QPEDIR)
    PRINTNL         <envvar value="%(QPEDIR)" name="QPEDIR" />
    PRINTNL         <envvar value="%(QPEDIR)/lib:%(QTDIR)/lib:$LD_LIBRARY_PATH" name="LD_LIBRARY_PATH" />
  ELSE
    PRINTNL         <envvar value="%(QTDIR)/lib:$LD_LIBRARY_PATH" name="LD_LIBRARY_PATH" />
  ENDIF
  PRINTNL       </envvars>
  PRINTNL       <directoryradio>custom</directoryradio>
  PRINTNL       <customdirectory>/</customdirectory>
  FOR EACH USE
    IF %(TYPE)==EXE
      PRINTNL       <mainprogram>%(¯|OUTPUTPATH)%(NAME)</mainprogram>
    ENDIF
  ENDFOR
  PRINTNL     </run>
  PRINTNL     <envvars/>
  PRINTNL     <make>
  PRINTNL       <abortonerror>true</abortonerror>
  PRINTNL       <runmultiplejobs>false</runmultiplejobs>
  PRINTNL       <numberofjobs>1</numberofjobs>
  PRINTNL       <dontact>false</dontact>
  PRINTNL       <makebin>make -f Makefile</makebin>
  PRINTNL       <prio>0</prio>
  PRINTNL       <envvars>
  IF %(QPEDIR)
    PRINTNL         <envvar value="%(QPEDIR)" name="QPEDIR" />
  ENDIF
  PRINTNL         <envvar value="%(QTDIR)" name="QTDIR" />
  PRINTNL       </envvars>
  PRINTNL     </make>
  PRINTNL   </kdevtrollproject>
  PRINTNL   <kdevfilecreate>
  PRINTNL     <filetypes/>
  PRINTNL     <useglobaltypes/>
  PRINTNL   </kdevfilecreate>
  PRINTNL   <cppsupportpart>
  PRINTNL     <codecompletion>
  PRINTNL       <codehinting outputview="1" enablech="0" selectview="0" />
  PRINTNL     </codecompletion>
  PRINTNL     <classstore>
  PRINTNL       <enablepcs>false</enablepcs>
  PRINTNL       <enablepp>false</enablepp>
  PRINTNL       <preparsing/>
  PRINTNL     </classstore>
  PRINTNL     <filetemplates>
  PRINTNL       <choosefiles>false</choosefiles>
  PRINTNL       <interfaceURL/>
  PRINTNL       <implementationURL/>
  PRINTNL       <interfacesuffix>.h</interfacesuffix>
  PRINTNL       <implementationsuffix>.cpp</implementationsuffix>
  PRINTNL       <lowercasefilenames>true</lowercasefilenames>
  PRINTNL     </filetemplates>
  PRINTNL   </cppsupportpart>
  PRINTNL   <kdevcppsupport>
  PRINTNL     <references/>
  PRINTNL     <codecompletion>
  PRINTNL       <includeGlobalFunctions>true</includeGlobalFunctions>
  PRINTNL       <includeTypes>true</includeTypes>
  PRINTNL       <includeEnums>true</includeEnums>
  PRINTNL       <includeTypedefs>false</includeTypedefs>
  PRINTNL       <automaticCodeCompletion>true</automaticCodeCompletion>
  PRINTNL       <automaticArgumentsHint>true</automaticArgumentsHint>
  PRINTNL       <automaticHeaderCompletion>true</automaticHeaderCompletion>
  PRINTNL       <codeCompletionDelay>250</codeCompletionDelay>
  PRINTNL       <argumentsHintDelay>400</argumentsHintDelay>
  PRINTNL       <headerCompletionDelay>250</headerCompletionDelay>
  PRINTNL     </codecompletion>
  PRINTNL     <qt>
  PRINTNL       <used>true</used>
  PRINTNL       <version>4</version>
  PRINTNL       <root>%(QTDIR)</root>
  PRINTNL     </qt>
  PRINTNL   </kdevcppsupport>
  PRINTNL </kdevelop>

  IF "%(KDEVELOP_GDBSCRIPT)"!=""
    FILE %(¯PATH)%(NAME).kdevelop_gdbscript
    PRINTNL %(KDEVELOP_GDBSCRIPT)
  ENDIF

  FILE %(¯PATH)Makefile
  IF %(QPEDIR)
    // needed to run some of the Qtopia/Greenphone (stupid) scripts like gph
    PRINTNL PREFIX?=%(QPEDIR)/image
    PRINTNL DPREFIX?=%(QPEDIR)/dimage/opt/Qtopia
    PRINTNL D="-d"
  ENDIF
  PRINTNL all: %(USE)
  PRINTNL
  FOR EACH USE
    PRINTNL %(NAME): %(USE)
    PRINTNL 	@cd %(!PATH) && $(MAKE) -f %(NAME).mak
  ENDFOR
  PRINTNL
  PRINTNL clean:
  FOR EACH USE
    IF %(TYPE)==EXE
      PRINTNL 	@cd %(!PATH) && $(MAKE) -f %(NAME).mak clean
    ENDIF
  ENDFOR
  PRINTNL 
  PRINTNL install:
  FOR EACH USE
    IF %(TYPE)==EXE
      PRINTNL 	@cd %(!PATH) && $(MAKE) -f %(NAME).mak install
    ENDIF
  ENDFOR
  PRINTNL 
  PRINTNL .phony: all install %(USE)
  PRINTNL

  FOR EACH USE
    #include "qtopia_desktop.inc"
    #include "qtopia_resource.inc"

    FILE %(¯PATH)%(NAME).mak
    #include "gcc_mak.inc"
  ENDFOR
ENDFOR

