

XPStyle on
CRCCheck on

Var REINSTALL

Function un.onInit
	SetShellVarContext current
	ReadRegStr $R0 HKLM "SOFTWARE\EastFront" "InstallPath"
	IfErrors err
		StrCpy $INSTDIR $R0
		Goto done
	err:
		MessageBox MB_OK "Could not find EastFront installation to uninstall!"
		Abort
	done:
FunctionEnd

Function .onInit
	SetShellVarContext current
	ReadRegStr $R0 HKLM "SOFTWARE\EastFront" "InstallPath"
	IfErrors etdefault
		StrCpy $INSTDIR $R0
		Goto dllcheck
	etdefault:
		ReadRegStr $R0 HKLM "SOFTWARE\Activision\Wolfenstein - Enemy Territory" "InstallPath"
		IfErrors dllcheck
			StrCpy $INSTDIR $R0

	dllcheck:
;	IfFileExists "$EXEDIR\qagame_mp_x86.dll" done
;		MessageBox MB_OK "Could not find qagame_mp_x86.dll, quitting."
;		Abort
	done:
FunctionEnd

Function GetParent
 
   Exch $R0
   Push $R1
   Push $R2
   Push $R3
   
   StrCpy $R1 0
   StrLen $R2 $R0
   
   loop:
     IntOp $R1 $R1 + 1
     IntCmp $R1 $R2 get 0 get
     StrCpy $R3 $R0 1 -$R1
     StrCmp $R3 "\" get
     Goto loop
   
   get:
     StrCpy $R0 $R0 -$R1
     
     Pop $R3
     Pop $R2
     Pop $R1
     Exch $R0
     
FunctionEnd


!include "MUI.nsh"

!define MUI_ABORTWARNING

!define MUI_PAGE_CUSTOMFUNCTION_LEAVE CheckReinstall
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION "Select your Enemy Territory Directory"
!define MUI_DIRECTORYPAGE_TEXT_TOP "Please select your Enemy Territory directory.  This is the directory on your system that contains the file 'ET.exe'.  This installer will install all files into a directory named 'ef' inside of the directroy you select."
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"


; The name of the installer
Name "EastFront"


; The file to write
OutFile "setup.exe"

; The default installation directory
InstallDir ""

Section "EastFront Files" qagame
  SectionIn RO
  CreateDirectory "$INSTDIR\ef"
  CreateDirectory "$INSTDIR\ef\MyMusic"
  CreateDirectory "$INSTDIR\ef\maps"
  CreateDirectory "$INSTDIR\ef\nodes"
  SetOutPath "$INSTDIR"
  File /r "dist\*"
; CopyFiles "$EXEDIR\qagame_mp_x86.dll" "$INSTDIR\ef\" 2209
; CopyFiles "$EXEDIR\cgame_mp_x86.dll" "$INSTDIR\ef\" 2229
; CopyFiles "$EXEDIR\ui_mp_x86.dll" "$INSTDIR\ef\" 777
; CopyFiles "$EXEDIR\ef-main.pk3" "$INSTDIR\ef\" 27476
; CopyFiles "$EXEDIR\ef-bins.pk3" "$INSTDIR\ef\" 1494
; CopyFiles "$EXEDIR\etenhanced.ico" "$INSTDIR\ef\" 8
; CopyFiles "$EXEDIR\Desktop.ini" "$INSTDIR\ef\" 1
; CopyFiles "$EXEDIR\maps\aascfg_lg.c" "$INSTDIR\ef\maps\" 2
; CopyFiles "$EXEDIR\MyMusic\generate_music_list.bat" "$INSTDIR\ef\MyMusic\" 1
; CopyFiles "$EXEDIR\MyMusic\internet_radio.list" "$INSTDIR\ef\MyMusic\" 1
; CopyFiles "$EXEDIR\MyMusic\ReadMe.txt" "$INSTDIR\ef\MyMusic\" 1
  WriteRegStr HKLM "Software\EastFront" "InstallPath" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EastFront" "DisplayName" "EastFront (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EastFront" "UninstallString" '"$INSTDIR\ef\uninstall.exe"'
  CreateShortCut "$INSTDIR\ef-ded.lnk" "$INSTDIR\ET.exe" "+set dedicated 2 +set com_hunkmegs 128 +set fs_game ef +exec server.cfg" "$EXEDIR\etenhanced.ico"
  CreateShortCut "$INSTDIR\ef.lnk" "$INSTDIR\ET.exe" "+set com_hunkmegs 128 +set fs_game ef" "$EXEDIR\etenhanced.ico"
  WriteUninstaller "$INSTDIR\ef\uninstall.exe"
SectionEnd

Section "Example Configuration" cfg
  CopyFiles "$EXEDIR\docs\Example\Commented\default.cfg" "$INSTDIR\ef\" 16
  CopyFiles "$EXEDIR\docs\Example\Commented\server.cfg" "$INSTDIR\ef\" 16
  CopyFiles "$EXEDIR\docs\Example\Commented\shrubbot.cfg" "$INSTDIR\ef\" 16
SectionEnd

Section "Start Menu Shortcuts" startmenu
  CreateDirectory "$SMPROGRAMS\EastFront\"
  CreateShortCut "$SMPROGRAMS\EastFront\Start EastFront.lnk" "$INSTDIR\ef.lnk" "" "EastFront" 0
  CreateShortCut "$SMPROGRAMS\EastFront\Start EastFront Dedicated.lnk" "$INSTDIR\ef-ded.lnk" "" "EastFront Dedicated Server" 0
  CreateShortCut "$SMPROGRAMS\EastFront\Edit server.cfg.lnk" "notepad.exe" "$INSTDIR\ef\server.cfg" "" 0 SW_SHOWNORMAL "" "Edit server.cfg"
  CreateShortCut "$SMPROGRAMS\EastFront\Edit default.cfg.lnk" "notepad.exe" "$INSTDIR\ef\default.cfg" ""  0 SW_SHOWNORMAL "" "Edit default.cfg"
  CreateShortCut "$SMPROGRAMS\EastFront\Edit shrubbot.cfg.lnk" "notepad.exe" "$INSTDIR\ef\shrubbot.cfg" "" 0 SW_SHOWNORMAL "" "Edit shrubbot.cfg"
  CreateShortCut "$SMPROGRAMS\EastFront\Uninstall.lnk" "$INSTDIR\ef\uninstall.exe" "" "" 0
SectionEnd

Section "Desktop Icon" desktop
  CreateShortCut "$DESKTOP\EastFront.lnk" "$INSTDIR\ef.lnk" "" "$INSTDIR\ef.lnk"
SectionEnd

UninstallText "You are about to uninstall EastFront from your computer.  If you do this, all files in your EastFront directory WILL BE DELETED."

Section "Uninstall"
  ; remove registry keys

  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\EastFront"
  DeleteRegKey HKLM "SOFTWARE\EastFront"

  ; remove shortcuts, if any
  Delete "$SMPROGRAMS\EastFront\*.*"
  Delete "$DESKTOP\EastFront.lnk"
  Delete "$INSTDIR\ef.lnk"
  Delete "$INSTDIR\ef-ded.lnk"

  ; remove directories used
  RMDir /r "$SMPROGRAMS\EastFront"
  RMDir /r "$INSTDIR\ef\maps"
  RMDir /r "$INSTDIR\ef\MyMusic"
  RMDir /r "$INSTDIR\ef"
  RMDir "$SMPROGRAMS\EastFront"
SectionEnd


LangString DESC_qagame ${LANG_ENGLISH} "Copies required game files to the EastFront directory inside your ET directory"
LangString DESC_cfg ${LANG_ENGLISH} "Installs commented server.cfg, default.cfg, and shrubbot.cfg files into your EastFront directory with the default settings."
LangString DESC_startmenu ${LANG_ENGLISH} "Installs links into your Programs menu."
LangString DESC_desktop ${LANG_ENGLISH} "Creates a EastFront icon on your desktop."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${qagame} $(DESC_qagame)
  !insertmacro MUI_DESCRIPTION_TEXT ${cfg} $(DESC_cfg)
  !insertmacro MUI_DESCRIPTION_TEXT ${startmenu} $(DESC_startmenu)
  !insertmacro MUI_DESCRIPTION_TEXT ${desktop} $(DESC_desktop)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function CheckReinstall
	IfFileExists "$INSTDIR\ET.exe" EndIf
		MessageBox MB_OK "$INSTDIR does not appear to contain a valid Enemy Territory installation."
		Abort
	EndIf:

	IfFileExists "$INSTDIR\ef\*.*" reinst
		GoTo fresh
	reinst:
		MessageBox MB_YESNO "EastFront is already installed in this directory.  Do you want to install over it?" IDYES doreinst
		Abort
	doreinst:
		StrCpy $REINSTALL "1"
		MessageBox MB_OK "Since you are re-installing, your EastFront .cfg files will NOT be overwritten by default.$\r$\nHowever, IF you choose to install the 'Example Configuration' component,$\r$\nyour existing .cfg files WILL BE OVERWRITTEN." 
		SectionSetFlags ${cfg} 0
		Goto done 
	fresh:
		; required if not reinstalling
		SectionSetFlags ${cfg} 17
	done:
FunctionEnd
