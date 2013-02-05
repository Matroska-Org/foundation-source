IF DEFINED(TARGET_WINCE)
  DEBUGCLASS = DebuggerTool
  EXECLASS = RemoteExecutable
  ARGCLASS = Arguments
ELSE
  DEBUGCLASS = DebugSettings
  EXECLASS = Command
  ARGCLASS = CommandArguments
  WORKINGDIR = %(~|OUTPUTPATH)
ENDIF

PRINTNL <?xml version="1.0" encoding="Windows-1252"?>
PRINTNL <VisualStudioUserFile
PRINTNL 	ProjectType="Visual C++"
PRINTNL 	Version="%(VSVERSION)"
PRINTNL 	ShowAllFiles="false"
PRINTNL 	>
PRINTNL 	<Configurations>
PRINTNL 		<Configuration
PRINTNL 			Name="%(TARG_DEBUG)|%(TARGNAME)"
PRINTNL 			>
PRINTNL 			<%(DEBUGCLASS)
PRINTNL 				%(EXECLASS)="%(EXECUTABLE_DEBUG)"
IF %(WORKINGDIR)
  PRINTNL 				WorkingDirectory="%(WORKINGDIR)debug%(OUTPUTDIR)"
ENDIF
PRINTNL 				%(ARGCLASS)="%(ARGUMENTS_DEBUG)"
PRINTNL 			/>
PRINTNL 		</Configuration>
PRINTNL 		<Configuration
PRINTNL 			Name="%(TARG_RELEASE)|%(TARGNAME)"
PRINTNL 			>
PRINTNL 			<%(DEBUGCLASS)
PRINTNL 				%(EXECLASS)="%(EXECUTABLE_RELEASE)"
IF %(WORKINGDIR)
  PRINTNL 				WorkingDirectory="%(WORKINGDIR)release%(OUTPUTDIR)"
ENDIF
PRINTNL 				%(ARGCLASS)=""
PRINTNL 			/>
PRINTNL 		</Configuration>
PRINTNL 	</Configurations>
PRINTNL </VisualStudioUserFile>
