IF %(TYPE)==EXE
  IF %(INSTALL)
    RESOURCE_QT4 += %(PATH)%(NAME).qrc
    FILE %(¯PATH)%(NAME).qrc
    PRINTNL <RCC>
    PRINTNL 	<qresource prefix="/" >
    FOR EACH INSTALL
      PRINTNL 		<file alias="%(<NAME).%(>NAME)">%(!PATH)%(!NAME)</file>
    ENDFOR
    PRINTNL 	</qresource>
    PRINTNL </RCC>
  ENDIF
ENDIF
