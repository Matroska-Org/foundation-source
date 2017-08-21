IF %(TYPE)==DLL || %(TYPE)==LIB
  PRINTNL 
  PRINTNL include $(CLEAR_VARS)
  PRINTNL LOCAL_MODULE := %(NAME)
  IF %(TARGARCH)=="arm"
    PRINTNL LOCAL_ARM_MODE := arm
  ENDIF
  PRINTNL LOCAL_SRC_FILES := \
  FOR EACH SOURCE
    IF %(FOR_LAST)
      PRINTNL     %(!NAME)
    ELSE
      PRINTNL     %(!NAME) \
    ENDIF
  ENDFOR
  PRINTNL 
  
  DLL_DEPS =
  LIB_DEPS =
  FOR EACH USE
    IF %(TYPE)==DLL
      ../DLL_DEPS += %(NAME)
    ELSE
      ../LIB_DEPS += %(NAME)
    ENDIF
  ENDFOR
  IF %(DLL_DEPS)!=""
    PRINTNL LOCAL_SHARED_LIBRARIES := \
    FOR EACH DLL_DEPS
      IF %(FOR_LAST)
        PRINTNL     %(NAME)
      ELSE
        PRINTNL     %(NAME) \
      ENDIF
    ENDFOR
    PRINTNL 
  ENDIF
  IF %(LIB_DEPS)!=""
    PRINTNL LOCAL_STATIC_LIBRARIES := \
    FOR EACH LIB_DEPS
      IF %(NAME)!=""
        IF %(FOR_LAST)
          PRINTNL     %(NAME)
        ELSE
          PRINTNL     %(NAME) \
        ENDIF
      ENDIF
    ENDFOR
    PRINTNL 
  ENDIF
  
  IF %(SYSLIBS)!="" && %(TYPE)==DLL
    PRINTNL LOCAL_LDLIBS := \
    FOR EACH SYSLIBS
      IF %(NAME)!=""
        IF %(FOR_LAST)
          PRINTNL     -l%(NAME)
        ELSE
          PRINTNL     -l%(NAME) \
        ENDIF
      ENDIF
    ENDFOR
    PRINTNL 
  ENDIF
  
  // the include folders are relative to project root
  FOR EACH INCLUDE
    ../USERINCLUDE += %(|NAME)
  ENDFOR
  FOR EACH EXPINCLUDE
    ../USERINCLUDE += %(|NAME)
  ENDFOR
  FOR EACH SUBINCLUDE
    ../USERINCLUDE += %(|NAME)
  ENDFOR

  DIRPUSH ..
  PRINTNL LOCAL_C_INCLUDES := \
  PRINTNL     %(@&CONFIG_INCLUDE:%= %) \
  FOR EACH USERINCLUDE
      PRINTNL     %(!§NAME) \
  ENDFOR
  PRINTNL 
  DIRPOP
  PRINTNL 

  IF %(DEFINE)!=""
    PRINTNL LOCAL_CFLAGS := %(DEFINE:%=-D%)
  ENDIF
  
  IF %(TYPE)==DLL
    IF %(LIBS)!=""
      PRINTNL LOCAL_LDLIBS += \
      FOR EACH LIBS
        IF %(NAME)!=""
          IF %(FOR_LAST)
            PRINTNL     -l%(NAME)
          ELSE
            PRINTNL     -l%(NAME) \
          ENDIF
        ENDIF
      ENDFOR
      PRINTNL 
    ENDIF
    
    PRINTNL LOCAL_LDFLAGS += -nostdlib
    
    PRINTNL include $(BUILD_SHARED_LIBRARY)
  ELSE
    PRINTNL include $(BUILD_STATIC_LIBRARY)
  ENDIF
  PRINTNL 
ENDIF
