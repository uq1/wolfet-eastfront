# Microsoft Developer Studio Project File - Name="ui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ui - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ui.mak" CFG="ui - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Wolfenstein MP/src/ui", QTMAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /W3 /GX /Ob1 /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UI_EXPORTS" /D "_MBCS" /YX /GF /c
# ADD CPP /nologo /G5 /W3 /GX /Ob1 /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UI_EXPORTS" /D "_MBCS" /YX /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib /nologo /base:"0x40000000" /subsystem:windows /dll /map /machine:I386 /out:"..\Release\ui_mp_x86.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib /nologo /base:"0x40000000" /subsystem:windows /dll /map /machine:I386 /out:"..\Release\ui_mp_x86.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UI_EXPORTS" /D "_MBCS" /FR /YX /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /GX /ZI /Od /D "_USRDLL" /D "UI_EXPORTS" /D "_MBCS" /D "_X86_" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /YX /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib /nologo /base:"0x40000000" /subsystem:windows /dll /pdb:"..\Debug\ui.pdb" /map /debug /machine:I386 /out:"..\Debug\ui_mp_x86.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib /nologo /base:"0x40000000" /subsystem:windows /dll /pdb:"..\Debug\ui.pdb" /map /debug /machine:I386 /out:"..\Debug\ui_mp_x86.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ui - Win32 Release"
# Name "ui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\game\bg_campaign.c
DEP_CPP_BG_CA=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\cg_local.h"\
	"..\cgame\cg_public.h"\
	"..\cgame\tr_types.h"\
	"..\game\be_aas.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\g_local.h"\
	"..\game\g_public.h"\
	"..\game\g_shrubbot.h"\
	"..\game\g_team.h"\
	"..\game\g_xpsave.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	".\keycodes.h"\
	".\ui_local.h"\
	".\ui_public.h"\
	".\ui_shared.h"\
	
NODEP_CPP_BG_CA=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_classes.c
DEP_CPP_BG_CL=\
	"..\game\bg_classes.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
NODEP_CPP_BG_CL=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_lib.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\game\bg_misc.c
DEP_CPP_BG_MI=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\cg_local.h"\
	"..\cgame\cg_public.h"\
	"..\cgame\tr_types.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	".\keycodes.h"\
	".\ui_shared.h"\
	
NODEP_CPP_BG_MI=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=..\cgame\cg_musicengine.c
DEP_CPP_CG_MU=\
	"..\cgame\bass.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
NODEP_CPP_CG_MU=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\q_math.c
DEP_CPP_Q_MAT=\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
NODEP_CPP_Q_MAT=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\q_shared.c
DEP_CPP_Q_SHA=\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
NODEP_CPP_Q_SHA=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ui.def
# End Source File
# Begin Source File

SOURCE=.\ui_atoms.c
DEP_CPP_UI_AT=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\tr_types.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	".\keycodes.h"\
	".\ui_local.h"\
	".\ui_public.h"\
	".\ui_shared.h"\
	
NODEP_CPP_UI_AT=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ui_gameinfo.c
DEP_CPP_UI_GA=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\tr_types.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	".\keycodes.h"\
	".\ui_local.h"\
	".\ui_public.h"\
	".\ui_shared.h"\
	
NODEP_CPP_UI_GA=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ui_loadpanel.c
DEP_CPP_UI_LO=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\tr_types.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	".\keycodes.h"\
	".\ui_local.h"\
	".\ui_public.h"\
	".\ui_shared.h"\
	
NODEP_CPP_UI_LO=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ui_main.c
DEP_CPP_UI_MA=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\tr_types.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	".\keycodes.h"\
	".\ui_local.h"\
	".\ui_public.h"\
	".\ui_shared.h"\
	
NODEP_CPP_UI_MA=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ui_players.c
DEP_CPP_UI_PL=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\tr_types.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	".\keycodes.h"\
	".\ui_local.h"\
	".\ui_public.h"\
	".\ui_shared.h"\
	
NODEP_CPP_UI_PL=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ui_shared.c
DEP_CPP_UI_SH=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\tr_types.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	".\keycodes.h"\
	".\ui_local.h"\
	".\ui_public.h"\
	".\ui_shared.h"\
	
NODEP_CPP_UI_SH=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ui_syscalls.c
DEP_CPP_UI_SY=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\tr_types.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	".\keycodes.h"\
	".\ui_local.h"\
	".\ui_public.h"\
	".\ui_shared.h"\
	
NODEP_CPP_UI_SY=\
	"..\game\bg_lib.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ui_util.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_mem.c
DEP_CPP_UNIQU=\
	"..\botai\ai_main.h"\
	"..\game\be_aas.h"\
	"..\game\be_ai_goal.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\botlib.h"\
	"..\game\etenhanced.h"\
	"..\game\g_local.h"\
	"..\game\g_public.h"\
	"..\game\g_shrubbot.h"\
	"..\game\g_team.h"\
	"..\game\g_xpsave.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
NODEP_CPP_UNIQU=\
	"..\game\bg_lib.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\cgame\bass.h
# End Source File
# Begin Source File

SOURCE=..\game\bg_classes.h
# End Source File
# Begin Source File

SOURCE=..\game\bg_public.h
# End Source File
# Begin Source File

SOURCE=.\keycodes.h
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\menudef.h
# End Source File
# Begin Source File

SOURCE=..\game\q_global_defines.h
# End Source File
# Begin Source File

SOURCE=..\game\q_shared.h
# End Source File
# Begin Source File

SOURCE=..\game\surfaceflags.h
# End Source File
# Begin Source File

SOURCE=..\cgame\tr_types.h
# End Source File
# Begin Source File

SOURCE=.\ui_local.h
# End Source File
# Begin Source File

SOURCE=.\ui_public.h
# End Source File
# Begin Source File

SOURCE=.\ui_shared.h
# End Source File
# End Group
# Begin Group "Menu Files"

# PROP Default_Filter "*.menu"
# Begin Source File

SOURCE=..\..\etmain\ui\credits_activision.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\credits_additional.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\credits_idsoftware.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\credits_quit.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\credits_splashdamage.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\etpub.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\etpub_customize_hud.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\global.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\hostgame.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\hostgame_advanced.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\hostgame_advanced_default.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\hostgame_dedicated_warning.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_disconnect.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_main.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_messagemode.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_serverinfo.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_tapout.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_tapoutlms.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_vote.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_vote_disabled.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_vote_map.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_vote_misc.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_vote_misc_refrcon.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_vote_players.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\ingame_vote_players_warn.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\main.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\menudef.h
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\menumacros.h
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\menus.txt
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\options.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\options_controls.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\options_controls_default.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\options_customise_game.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\options_customise_hud.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\options_system.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\options_system_gamma.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\playonline.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\playonline_connecttoip.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\playonline_disablepb.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\playonline_enablepb.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\playonline_serverinfo.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\popup_autoupdate.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\popup_errormessage.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\popup_errormessage_pb.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\popup_hostgameerrormessage.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\popup_password.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\profile.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\profile_create.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\profile_create_error.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\profile_create_initial.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\profile_delete.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\profile_delete_error.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\profile_rename.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\quit.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\rec_restart.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\vid_confirm.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\vid_restart.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\viewreplay.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\viewreplay_delete.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\wm_ftquickmessage.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\wm_ftquickmessageAlt.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\wm_quickmessage.menu
# End Source File
# Begin Source File

SOURCE=..\..\etmain\ui\wm_quickmessageAlt.menu
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\loadnget.rc
# End Source File
# End Group
# Begin Group "HUD files"

# PROP Default_Filter ".hud"
# Begin Source File

SOURCE=..\..\etmain\hud\deej.hud
# End Source File
# Begin Source File

SOURCE=..\..\etmain\hud\stumpy.hud
# End Source File
# End Group
# Begin Source File

SOURCE=..\cgame\bass.dll
# End Source File
# End Target
# End Project
