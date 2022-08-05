# Microsoft Developer Studio Project File - Name="cgame" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=cgame - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cgame.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cgame.mak" CFG="cgame - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cgame - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cgame - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Wolfenstein MP/src/cgame", HTMAAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cgame - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /W4 /GX /Ob1 /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "CGAMEDLL" /YX /GF /c
# ADD CPP /nologo /G5 /MD /W4 /GX /Ob1 /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "CGAMEDLL" /D "_X86_" /D "_AFXDLL" /YX /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib /nologo /base:"0x30000000" /subsystem:windows /dll /map /machine:I386 /out:"..\Release\cgame_mp_x86.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib winmm.lib /nologo /base:"0x30000000" /subsystem:windows /dll /map /machine:I386 /out:"..\Release\cgame_mp_x86.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cgame - Win32 Debug"

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
# ADD BASE CPP /nologo /G5 /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "CGAMEDLL" /D "__MUSIC_ENGINE__" /D "__ETE__" /D "__BOT__" /D "__ENTITY_OVERRIDES__" /FR /YX /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /GX /ZI /Od /D "CGAMEDLL" /D "_X86_" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /YX /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib winmm.lib /nologo /base:"0x30000000" /subsystem:windows /dll /map /debug /machine:I386 /out:"..\Debug\cgame_mp_x86.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib /nologo /base:"0x30000000" /subsystem:windows /dll /map /debug /machine:I386 /out:"..\Debug\cgame_mp_x86.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "cgame - Win32 Release"
# Name "cgame - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=..\game\bg_animation.c
DEP_CPP_BG_AN=\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_animgroup.c
DEP_CPP_BG_ANI=\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_campaign.c
DEP_CPP_BG_CA=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\be_aas.h"\
	"..\game\bg_lib.h"\
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
	"..\ui\keycodes.h"\
	"..\ui\ui_local.h"\
	"..\ui\ui_public.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_character.c
DEP_CPP_BG_CH=\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_classes.c
DEP_CPP_BG_CL=\
	"..\game\bg_classes.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_lib.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\game\bg_misc.c
DEP_CPP_BG_MI=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_physics.c
DEP_CPP_BG_PH=\
	"..\game\be_aas.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_local.h"\
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
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_pmove.c
DEP_CPP_BG_PM=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\be_aas.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_local.h"\
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
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_slidemove.c
DEP_CPP_BG_SL=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\be_aas.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_local.h"\
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
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_sscript.c
DEP_CPP_BG_SS=\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_stats.c
DEP_CPP_BG_ST=\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_tracemap.c
DEP_CPP_BG_TR=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\be_aas.h"\
	"..\game\bg_lib.h"\
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
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\bg_wheel_forces.c
DEP_CPP_BG_WH=\
	"..\game\be_aas.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_local.h"\
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
	
# End Source File
# Begin Source File

SOURCE=.\cg_ai_fireteam.c
DEP_CPP_CG_AI=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_classes.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_atmospheric.c
DEP_CPP_CG_AT=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_character.c
DEP_CPP_CG_CH=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_commandmap.c
DEP_CPP_CG_CO=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_consolecmds.c
DEP_CPP_CG_CON=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_debriefing.c
DEP_CPP_CG_DE=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_draw.c
DEP_CPP_CG_DR=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_drawtools.c
DEP_CPP_CG_DRA=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_effects.c
DEP_CPP_CG_EF=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_ents.c
DEP_CPP_CG_EN=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_event.c
DEP_CPP_CG_EV=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_fireteamoverlay.c
DEP_CPP_CG_FI=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_fireteams.c
DEP_CPP_CG_FIR=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_flamethrower.c
DEP_CPP_CG_FL=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_hud.c
DEP_CPP_CG_HU=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_info.c
DEP_CPP_CG_IN=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_limbopanel.c
DEP_CPP_CG_LI=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_loadpanel.c
DEP_CPP_CG_LO=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_localents.c
DEP_CPP_CG_LOC=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_main.c
DEP_CPP_CG_MA=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_marks.c
DEP_CPP_CG_MAR=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_missionbriefing.c
DEP_CPP_CG_MI=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_multiview.c
DEP_CPP_CG_MU=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_local.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_musicengine.c
DEP_CPP_CG_MUS=\
	"..\game\bg_lib.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	".\bass.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_newDraw.c
DEP_CPP_CG_NE=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_npc.c
DEP_CPP_CG_NP=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_classes.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_om_flagsys.c
DEP_CPP_CG_OM=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_particles.c
DEP_CPP_CG_PA=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_particlesys.c
DEP_CPP_CG_PAR=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_players.c
DEP_CPP_CG_PL=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_classes.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_playerstate.c
DEP_CPP_CG_PLA=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_polybus.c
DEP_CPP_CG_PO=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_popupmessages.c
DEP_CPP_CG_POP=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_predict.c
DEP_CPP_CG_PR=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_scoreboard.c
DEP_CPP_CG_SC=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_servercmds.c
DEP_CPP_CG_SE=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_snapshot.c
DEP_CPP_CG_SN=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
NODEP_CPP_CG_SN=\
	".\GameRanger SDK\GameRanger.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_sound.c
DEP_CPP_CG_SO=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_spawn.c
DEP_CPP_CG_SP=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_statsranksmedals.c
DEP_CPP_CG_ST=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_syscalls.c
DEP_CPP_CG_SY=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_tags.c
DEP_CPP_CG_TA=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_trails.c
DEP_CPP_CG_TR=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\cg_unique_autowaypoint.c
DEP_CPP_CG_UN=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_unique_entities.c
DEP_CPP_CG_UNI=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_view.c
DEP_CPP_CG_VI=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_weapons.c
DEP_CPP_CG_WE=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_classes.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=.\cg_window.c
DEP_CPP_CG_WI=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\etenhanced.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\cg_local.h"\
	".\cg_public.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\q_math.c
DEP_CPP_Q_MAT=\
	"..\game\bg_lib.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\game\q_shared.c
DEP_CPP_Q_SHA=\
	"..\game\bg_lib.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\ui\ui_shared.c
DEP_CPP_UI_SH=\
	"..\..\etmain\ui\menudef.h"\
	"..\game\bg_lib.h"\
	"..\game\bg_physics.h"\
	"..\game\bg_public.h"\
	"..\game\q_global_defines.h"\
	"..\game\q_shared.h"\
	"..\game\surfaceflags.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_local.h"\
	"..\ui\ui_public.h"\
	"..\ui\ui_shared.h"\
	".\tr_types.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_mem.c
DEP_CPP_UNIQU=\
	"..\botai\ai_main.h"\
	"..\game\be_aas.h"\
	"..\game\be_ai_goal.h"\
	"..\game\bg_lib.h"\
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
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\bass.h
# End Source File
# Begin Source File

SOURCE=.\basstest.h
# End Source File
# Begin Source File

SOURCE=..\game\bg_classes.h
# End Source File
# Begin Source File

SOURCE=..\game\bg_local.h
# End Source File
# Begin Source File

SOURCE=..\game\bg_public.h
# End Source File
# Begin Source File

SOURCE=.\cg_local.h
# End Source File
# Begin Source File

SOURCE=.\cg_public.h
# End Source File
# Begin Source File

SOURCE=.\eteclient.h
# End Source File
# Begin Source File

SOURCE=..\ui\keycodes.h
# End Source File
# Begin Source File

SOURCE=..\..\main\ui\menudef.h
# End Source File
# Begin Source File

SOURCE=..\game\q_global_defines.h
# End Source File
# Begin Source File

SOURCE=..\game\q_shared.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\game\surfaceflags.h
# End Source File
# Begin Source File

SOURCE=.\tr_types.h
# End Source File
# Begin Source File

SOURCE=..\ui\ui_shared.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter "rc"
# Begin Source File

SOURCE=loadnget.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\bass.dll
# End Source File
# Begin Source File

SOURCE=.\cgame.def
# End Source File
# End Target
# End Project
