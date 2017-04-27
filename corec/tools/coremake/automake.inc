CONFIG

// make sure to use fixed output names for lib and dll
FOR EACH LIB,DLL
  OUTPUT = lib%(NAME).la
ENDFOR

ALLDIRS = ./
AC_OUTPUT = ./Makefile

FOR EACH WORKSPACE
  FOR REVERSE EACH USE
    x = %(PATH)
    WHILE "%(x)"!="./"
      ../../ALLDIRS += %(|x)
      ../../AC_OUTPUT += %(|x)Makefile
      x = %(?x)
    ENDWHILE

    IF "%(../../ACTIVETARGET)"=="" && (%(TYPE)=="exe" || %(TYPE)=="con")
       ../../ACTIVETARGET = %(PATH)%(OUTPUT)
    ENDIF

  ENDFOR
ENDFOR

FILE %(ฏ)Makefile.cvs
PRINTNL default: all
PRINTNL all:
PRINTNL 	aclocal
PRINTNL 	libtoolize --force
PRINTNL 	autoheader
PRINTNL 	automake --add-missing --foreign
PRINTNL 	autoconf

FILE %(ฏ)configure.in
PRINTNL AC_INIT(root,1.0)
PRINTNL 
PRINTNL AC_CONFIG_SRCDIR(configure.in)
PRINTNL AM_CONFIG_HEADER(%(@BUILDPATH)/config.h)
PRINTNL AC_CONFIG_AUX_DIR(%(@BUILDPATH))
PRINTNL 
PRINTNL AM_INIT_AUTOMAKE
PRINTNL 
PRINTNL AC_LANG_C
PRINTNL AC_PROG_CC
PRINTNL AM_PROG_LIBTOOL
PRINTNL 
PRINTNL AC_OUTPUT(%(AC_OUTPUT))
PRINTNL 

FOR EACH ALLDIRS

    FILE %(ฏNAME)Makefile.am
    PRINTNL AUTOMAKE_OPTIONS = foreign
    PRINTNL INCLUDES= $(all_includes)

    FOR EACH ../ALLDIRS
      IF "%(?NAME)"=="%(../NAME)"
        ../SUBDIRS += %(|@NAME)
      ENDIF
    ENDFOR

    IF "%(SUBDIRS)"!=""
      PRINTNL SUBDIRS = %(SUBDIRS)
    ENDIF

    FOR EACH ../LIB,../DLL,../EXE,../CON
      IF "%(PATH)"=="%(../NAME)"

        IF %(TYPE)==LIB || %(TYPE)==DLL
          OUTPUT2 = %(<OUTPUT)_%(>OUTPUT)
          OUTPUT2_LDADD = %(OUTPUT2)_LIBADD
          ../noinst_LTLIBRARIES += %(OUTPUT)
          IF %(TYPE)==LIB
            LDFLAGS = -static
          ENDIF
          IF %(TYPE)==DLL
            LDFLAGS = -module
          ENDIF
        ELSE
          IF "%(../ACTIVE)"==""
            ../ACTIVE = %(!PATH)%(OUTPUT)
          ENDIF
          OUTPUT2 = %(OUTPUT)
          OUTPUT2_LDADD = %(OUTPUT2)_LDADD
          ../noinst_PROGRAMS += %(OUTPUT)
        ENDIF

        LDADD += %(<LIBS:%=-l%)

        FOR EACH USE
          ../DEPENDENCIES += %(!PATH)%(OUTPUT)
          IF (!%(NOLIB))
            ../LDADD += %(!PATH)%(OUTPUT)
          ENDIF
        ENDFOR

        //PRINTNL %(OUTPUT2)_CPPFLAGS = $(AM_CPPFLAGS) %(DEFINE:%=-D "%") %(@INCLUDE:%=-I$(srcdir)/%) %(@SYSINCLUDE:%=-I$(srcdir)/%)
        ../AM_CPPFLAGS += %(DEFINE:%=-D "%") %(บ@INCLUDE:%=-I%)  %(บบ@INCLUDE:%=-I$(srcdir)/%) %(บ@SYSINCLUDE:%=-I%) %(บบ@SYSINCLUDE:%=-I$(srcdir)/%)
        IF %(TYPE)!=LIB
          LDFLAGS += %(บ@RPATH:%=-R %) %(บบ@RPATH:%=-R $(srcdir)/%) %(บ@LIBINCLUDE:%=-L%) %(บบ@LIBINCLUDE:%=-L$(srcdir)/%)
        ENDIF
        PRINTNL
        FOR EACH SOURCE
          IF "%(>NAME)"=="c" || "%(>NAME)"=="cpp"
            ../SOURCES += %(!NAME)
          ENDIF
          IF (%(ASM)!="") && "%(>NAME)"=="asm"
            ../LDADD += %(<NAME).lo
            ../DEPENDENCIES += %(<NAME).lo
          ENDIF
        ENDFOR
        FOR EACH HEADER
          ../../noinst_HEADERS += %(!NAME)
        ENDFOR

        IF "%(DEPENDENCIES)"!=""
          PRINTNL %(OUTPUT2)_DEPENDENCIES = %(DEPENDENCIES)
        ENDIF
        IF %(LDFLAGS) != ""
          PRINTNL %(OUTPUT2)_LDFLAGS = %(LDFLAGS)
        ENDIF
        IF "%(LDADD)"!=""
          PRINTNL %(OUTPUT2_LDADD) = %(LDADD)
        ENDIF

        IF "%(SOURCES)"!=""
          PRINTNL %(OUTPUT2)_SOURCES = %(SOURCES)
          PRINTNL 
        ENDIF

        IF (%(ASM)!="")
          FOR EACH SOURCE
            IF "%(>NAME)"=="asm"
              PRINTNL %(<NAME).lo: %(!NAME)
              PRINTNL 	$(LIBTOOL) --tag=CC --mode=compile %(ASM) %(ASMFLAGS) $(AM_CPPFLAGS) $< -o $@ -prefer-non-pic
              PRINTNL 
            ENDIF
          ENDFOR
        ENDIF

      ENDIF
    ENDFOR

    IF "%(noinst_HEADERS)"!=""
      PRINTNL noinst_HEADERS = %(noinst_HEADERS)
    ENDIF

    IF "%(AM_CPPFLAGS)"!=""
      PRINTNL AM_CPPFLAGS = %(AM_CPPFLAGS)
    ENDIF

    IF "%(noinst_LTLIBRARIES)"!=""
      PRINTNL noinst_LTLIBRARIES = %(noinst_LTLIBRARIES)
    ENDIF

    IF "%(noinst_PROGRAMS)"!=""
      PRINTNL noinst_PROGRAMS = %(noinst_PROGRAMS)
    ENDIF

ENDFOR
