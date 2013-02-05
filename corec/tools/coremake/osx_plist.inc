PRINTNL <?xml version="1.0" encoding="UTF-8"?>
PRINTNL <!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
PRINTNL <plist version="1.0">
PRINTNL <dict>

HAS_EXTENSIONS = 
HAS_PROTOCOLS =

#IFNDEF TARGET_IPHONE
FOR EACH SOURCE
  FOR EACH MIME_TYPE
    IF "%(EXTENSION)"!=""
      ../../HAS_EXTENSIONS = 1
    ENDIF
  ENDFOR
ENDFOR
FOR EACH USE
  FOR EACH SOURCE
    FOR EACH MIME_TYPE
      IF "%(EXTENSION)"!=""
        ../../../HAS_EXTENSIONS = 1
      ENDIF
    ENDFOR
  ENDFOR
ENDFOR

ENDIF // !TARGET_IPHONE

IFDEF TARGET_IPHONE_SDK
  FOR EACH SOURCE
    FOR EACH PROTOCOL
        ../../HAS_PROTOCOLS = 1
    ENDFOR
  ENDFOR
  FOR EACH USE
    FOR EACH SOURCE
      FOR EACH PROTOCOL
        ../../../HAS_PROTOCOLS = 1
      ENDFOR
    ENDFOR
  ENDFOR

ENDIF // TARGET_IPHONE_SDK

IFDEF TARGET_IPHONE_SDK
    IFDEF CONFIG_BACKGROUND_AUDIO
	PRINTNL	<key>UIBackgroundModes</key>
	PRINTNL	<array>
	PRINTNL	    <string>audio</string>
	PRINTNL	</array>
    ENDIF
ENDIF
PRINTNL 	<key>CFBundleDevelopmentRegion</key>
PRINTNL 	<string>English</string>
IF %(HAS_EXTENSIONS)
  PRINTNL 	<key>CFBundleDocumentTypes</key>
  PRINTNL 	<array>
  FOR EACH SOURCE
    FOR EACH MIME_TYPE
      PRINTNL 		<dict>
      IF "%(EXTENSION)"!=""
        PRINTNL 			<key>CFBundleTypeExtensions</key>
        PRINTNL 			<array>
        FOR EACH EXTENSION
          PRINTNL 				<string>%(NAME)</string>
        ENDFOR
        PRINTNL 			</array>
      ENDIF
      PRINTNL 			<key>CFBundleTypeMIMETypes</key>
      PRINTNL 			<array>
      PRINTNL 				<string>%(NAME)</string>
      PRINTNL 			</array>
      PRINTNL 			<key>CFBundleTypeRole</key>
      PRINTNL 			<string>Viewer</string>
      PRINTNL 		</dict>
    ENDFOR
  ENDFOR
  FOR EACH USE
    FOR EACH SOURCE
      FOR EACH MIME_TYPE
        PRINTNL 		<dict>
        IF %(EXTENSION)
          PRINTNL 			<key>CFBundleTypeExtensions</key>
          PRINTNL 			<array>
          FOR EACH EXTENSION
            PRINTNL 				<string>%(NAME)</string>
          ENDFOR
          PRINTNL 			</array>
        ENDIF
        PRINTNL 			<key>CFBundleTypeMIMETypes</key>
        PRINTNL 			<array>
        PRINTNL 				<string>%(NAME)</string>
        PRINTNL 			</array>
        PRINTNL 			<key>CFBundleTypeRole</key>
        PRINTNL 			<string>Viewer</string>
        PRINTNL 		</dict>
      ENDFOR
    ENDFOR
  ENDFOR
  PRINTNL 	</array>
ENDIF

IF %(HAS_PROTOCOLS)
  PRINTNL		<key>CFBundleURLTypes</key>
  PRINTNL		<array>
  PRINTNL			<dict>
  PRINTNL				<key>CFBundleURLName</key>
  PRINTNL				<string>com.%(PROJECT_VENDOR).%(NAME)</string>
  PRINTNL				<key>CFBundleURLSchemes</key>
  PRINTNL				<array>
  FOR EACH SOURCE
    FOR EACH PROTOCOL
      PRINTNL					<string>%(NAME)</string>
    ENDFOR
  ENDFOR
  FOR EACH USE
    FOR EACH SOURCE
      FOR EACH PROTOCOL
        PRINTNL					<string>%(NAME)</string>
      ENDFOR
    ENDFOR
  ENDFOR
  PRINTNL				</array>
  PRINTNL			</dict>
  PRINTNL		</array>
ENDIF

IF %(OSX_STRINGS)
  PRINTNL 	<key>CFBundleDevelopmentRegion</key>
  PRINTNL 	<string>%(OSX_STRINGS)</string>
ENDIF
PRINTNL 	<key>CFBundleExecutable</key>
IF %(PROJECT_PATH)
 PRINTNL 	<string>%(PROJECT_PATH)</string>
ELSE
 PRINTNL 	<string>%(NAME)</string>
ENDIF
IF %(PROJECT_NAME)
  PRINTNL 	<key>CFBundleGetInfoString</key>
  PRINTNL 	<string>%(PROJECT_NAME)</string>
ENDIF
IFNDEF TARGET_IPHONE
  IF %(OSX_ICON)
    PRINTNL 	<key>CFBundleIconFile</key>
    PRINTNL 	<string>%(<OSX_ICON).%(>OSX_ICON)</string>
  ELIF %(ICON)
    PRINTNL 	<key>CFBundleIconFile</key>
    PRINTNL 	<string>%(<ICON).icns</string>
  ENDIF
ELSE
  IF %(ICON)
    PRINTNL 	<key>CFBundleIconFile</key>
    PRINTNL 	<string>%(<ICON)_iphone2.png</string>
  ENDIF
  PRINTNL 	<key>UIRequiresPersistentWiFi</key>
  PRINTNL 	<true/>
ENDIF
IF %(PROJECT_VENDOR)
  PRINTNL 	<key>CFBundleIdentifier</key>
  PRINTNL 	<string>com.%(PROJECT_VENDOR).%(NAME)</string>
ENDIF
PRINTNL 	<key>CFBundleInfoDictionaryVersion</key>
PRINTNL 	<string>6.0</string>
PRINTNL 	<key>CFBundlePackageType</key>
IF (%(TYPE)==DLL)
  PRINTNL 	<string>BNDL</string>
ELSE
  PRINTNL 	<string>APPL</string>
ENDIF
IF %(PROJECT_BUILD)
  PRINTNL 	<key>CFBundleVersion</key>
  PRINTNL 	<string>%(PROJECT_BUILD)</string>
ENDIF
IF %(PROJECT_VERSION)
  PRINTNL 	<key>CFBundleShortVersionString</key>
  PRINTNL 	<string>%(PROJECT_VERSION)</string>
ENDIF
IF %(PROJECT_FOURCC)
  PRINTNL 	<key>CFBundleSignature</key>
  PRINTNL 	<string>%('PROJECT_FOURCC)</string>
ENDIF
MAIN_NIB_FILE =
MAIN_NIB_CLASS =
FOR EACH SOURCE
  IF ((%(>NAME)==nib) && (%(MAIN_NIB)))
    ../MAIN_NIB_FILE = %(NAME)
    ../MAIN_NIB_CLASS = %(MAIN_NIB)
  ENDIF
ENDFOR
FOR EACH USE
  FOR EACH SOURCE
    IF ((%(>NAME)==nib) && (%(MAIN_NIB)))
      ../../MAIN_NIB_FILE = %(NAME)
      ../../MAIN_NIB_CLASS = %(MAIN_NIB)
    ENDIF
  ENDFOR
ENDFOR
IF ((%(MAIN_NIB_FILE)) && (%(MAIN_NIB_CLASS)))
  PRINTNL 	<key>NSMainNibFile</key>
  PRINTNL 	<string>%(<MAIN_NIB_FILE)</string>
  PRINTNL 	<key>NSPrincipalClass</key>
  PRINTNL 	<string>%(MAIN_NIB_CLASS)</string>
ENDIF
FOR EACH PLIST_KEY
  PRINTNL 	<key>%(NAME)</key>
  PRINTNL 	<string>%(STRING)</string>
ENDFOR
PRINTNL </dict>
PRINTNL </plist>
