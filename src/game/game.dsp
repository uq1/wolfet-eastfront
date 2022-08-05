# Microsoft Developer Studio Project File - Name="game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=game - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "game.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "game.mak" CFG="game - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "game - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "game - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Wolfenstein MP/src/game", LTMAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "game - Win32 Debug"

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
# ADD BASE CPP /nologo /G5 /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "BUILDING_REF_GL" /D "DEBUG" /D "GAMEDLL" /D "__BOT__" /D "__ETE__" /FR /YX /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /GX /ZI /Od /D "BUILDING_REF_GL" /D "DEBUG" /D "GAMEDLL" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_WINDLL" /D "_AFXDLL" /FR /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /incremental:no /map /debug /machine:I386 /out:"..\Debug\qagame_mp_x86.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /incremental:no /map /debug /machine:I386 /out:"..\Debug\qagame_mp_x86.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "game - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /W4 /GX /Zi /Ob1 /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "GAMEDLL" /YX /GF /c
# ADD CPP /nologo /G5 /MD /W4 /GX /Zi /Ob1 /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "GAMEDLL" /D "_WINDLL" /D "_AFXDLL" /FR /YX /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /map /machine:I386 /out:"..\Release\qagame_mp_x86.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /map /machine:I386 /out:"..\Release\qagame_mp_x86.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "game - Win32 Debug"
# Name "game - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\ai_cast.c
DEP_CPP_AI_CA=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\ai_cast_global.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ai_cast_characters.c
DEP_CPP_AI_CAS=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\ai_cast_global.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ai_cast_events.c
DEP_CPP_AI_CAST=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\ai_cast_global.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ai_cast_fight.c
DEP_CPP_AI_CAST_=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\ai_cast_global.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ai_weap.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ai_cast_func_attack.c
DEP_CPP_AI_CAST_F=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\ai_cast_global.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ai_cast_func_boss1.c
DEP_CPP_AI_CAST_FU=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\ai_cast_global.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ai_cast_funcs.c
DEP_CPP_AI_CAST_FUN=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\ai_cast_global.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ai_cast_script.c
DEP_CPP_AI_CAST_S=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ai_cast_sight.c
DEP_CPP_AI_CAST_SI=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\ai_cast_global.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ai_cast_think.c
DEP_CPP_AI_CAST_T=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\ai_cast_global.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\botai\ai_cmd.c
DEP_CPP_AI_CM=\
	"..\botai\ai_cmd.h"\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	"..\botai\chars.h"\
	"..\botai\inv.h"\
	"..\botai\match.h"\
	"..\botai\syn.h"\
	".\be_aas.h"\
	".\be_ai_char.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ai_weap.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmgoal_mp.c
DEP_CPP_AI_DM=\
	"..\botai\ai_cmd.h"\
	"..\botai\ai_dmgoal_mp.h"\
	"..\botai\ai_dmnet_mp.h"\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	".\be_aas.h"\
	".\be_ai_char.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ai_weap.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmnet_mp.c
DEP_CPP_AI_DMN=\
	"..\botai\ai_cmd.h"\
	"..\botai\ai_dmnet_mp.h"\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	"..\botai\chars.h"\
	"..\botai\inv.h"\
	"..\botai\match.h"\
	"..\botai\syn.h"\
	".\be_aas.h"\
	".\be_ai_char.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ai_weap.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmq3.c
DEP_CPP_AI_DMQ=\
	"..\botai\ai_cmd.h"\
	"..\botai\ai_distances.h"\
	"..\botai\ai_dmgoal_mp.h"\
	"..\botai\ai_dmnet_mp.h"\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_matrix.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	"..\botai\chars.h"\
	"..\botai\inv.h"\
	"..\botai\match.h"\
	"..\botai\syn.h"\
	".\be_aas.h"\
	".\be_ai_char.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ai_weap.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\botai\ai_main.c
DEP_CPP_AI_MA=\
	"..\botai\ai_cmd.h"\
	"..\botai\ai_distances.h"\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	"..\botai\chars.h"\
	"..\botai\inv.h"\
	"..\botai\syn.h"\
	".\be_aas.h"\
	".\be_ai_char.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ai_weap.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\botai\ai_script.c
DEP_CPP_AI_SC=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\botai\ai_script_actions.c
DEP_CPP_AI_SCR=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	"..\botai\inv.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\botai\ai_team.c
DEP_CPP_AI_TE=\
	"..\botai\ai_cmd.h"\
	"..\botai\ai_distances.h"\
	"..\botai\ai_dmnet_mp.h"\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	"..\botai\inv.h"\
	".\be_aas.h"\
	".\be_ai_char.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ai_weap.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_animation.c
DEP_CPP_BG_AN=\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_animgroup.c
DEP_CPP_BG_ANI=\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_campaign.c
DEP_CPP_BG_CA=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\cg_local.h"\
	"..\cgame\cg_public.h"\
	"..\cgame\tr_types.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_local.h"\
	"..\ui\ui_public.h"\
	"..\ui\ui_shared.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_character.c
DEP_CPP_BG_CH=\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_classes.c
DEP_CPP_BG_CL=\
	".\bg_classes.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_lib.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\bg_malloc.c
DEP_CPP_BG_MA=\
	".\bg_lib.h"\
	".\bg_malloc.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_misc.c
DEP_CPP_BG_MI=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\cg_local.h"\
	"..\cgame\cg_public.h"\
	"..\cgame\tr_types.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_physics.c
DEP_CPP_BG_PH=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_local.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_pmove.c
DEP_CPP_BG_PM=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\cg_local.h"\
	"..\cgame\cg_public.h"\
	"..\cgame\tr_types.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_local.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_slidemove.c
DEP_CPP_BG_SL=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\cg_local.h"\
	"..\cgame\cg_public.h"\
	"..\cgame\tr_types.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_local.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_sscript.c
DEP_CPP_BG_SS=\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_stats.c
DEP_CPP_BG_ST=\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_tracemap.c
DEP_CPP_BG_TR=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\cg_local.h"\
	"..\cgame\cg_public.h"\
	"..\cgame\tr_types.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\bg_wheel_forces.c
DEP_CPP_BG_WH=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_local.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\etpro_mdx.c
DEP_CPP_ETPRO=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\etpro_mdx.h"\
	".\etpro_mdx_lut.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_active.c
DEP_CPP_G_ACT=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\etpro_mdx.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_alarm.c
DEP_CPP_G_ALA=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_antilag.c
DEP_CPP_G_ANT=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_bot.c
DEP_CPP_G_BOT=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	"..\botai\chars.h"\
	".\be_aas.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_buddy_list.c
DEP_CPP_G_BUD=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_character.c
DEP_CPP_G_CHA=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\etpro_mdx.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_client.c
DEP_CPP_G_CLI=\
	"..\..\etmain\ui\menudef.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_cmds.c
DEP_CPP_G_CMD=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_cmds_ext.c
DEP_CPP_G_CMDS=\
	"..\..\etmain\ui\menudef.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_combat.c
DEP_CPP_G_COM=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	"..\botai\chars.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\etpro_mdx.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_config.c
DEP_CPP_G_CON=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_crash.c
DEP_CPP_G_CRA=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_fireteams.c
DEP_CPP_G_FIR=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_items.c
DEP_CPP_G_ITE=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_main.c
DEP_CPP_G_MAI=\
	"..\botai\ai_teamX.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\etpro_mdx.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_match.c
DEP_CPP_G_MAT=\
	"..\..\etmain\ui\menudef.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_mem.c
DEP_CPP_G_MEM=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_malloc.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_misc.c
DEP_CPP_G_MIS=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_missile.c
DEP_CPP_G_MISS=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_mover.c
DEP_CPP_G_MOV=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_multiview.c
DEP_CPP_G_MUL=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_props.c
DEP_CPP_G_PRO=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_referee.c
DEP_CPP_G_REF=\
	"..\..\etmain\ui\menudef.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_save.c
DEP_CPP_G_SAV=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	"..\botai\chars.h"\
	".\be_aas.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
NODEP_CPP_G_SAV=\
	"..\botai\ai_func_decs.h"\
	"..\botai\ai_funcs.h"\
	".\g_func_decs.h"\
	".\g_funcs.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_script.c
DEP_CPP_G_SCR=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_script_actions.c
DEP_CPP_G_SCRI=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_session.c
DEP_CPP_G_SES=\
	"..\..\etmain\ui\menudef.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_shrubbot.c
DEP_CPP_G_SHR=\
	"..\..\etmain\ui\menudef.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_spawn.c
DEP_CPP_G_SPA=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_stats.c
DEP_CPP_G_STA=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_sv_entities.c
DEP_CPP_G_SV_=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_svcmds.c
DEP_CPP_G_SVC=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_syscalls.c
DEP_CPP_G_SYS=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_systemmsg.c
DEP_CPP_G_SYST=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_target.c
DEP_CPP_G_TAR=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_team.c
DEP_CPP_G_TEA=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_teammapdata.c
DEP_CPP_G_TEAM=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_trigger.c
DEP_CPP_G_TRI=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_unique_entities.c
DEP_CPP_G_UNI=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_utils.c
DEP_CPP_G_UTI=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_vote.c
DEP_CPP_G_VOT=\
	"..\..\etmain\ui\menudef.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_weapon.c
DEP_CPP_G_WEA=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\g_xpsave.c
DEP_CPP_G_XPS=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\game.def
# End Source File
# Begin Source File

SOURCE=.\npc_ai.c
DEP_CPP_NPC_A=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\npc_air_vehicle.c
DEP_CPP_NPC_AI=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\npc_humanoid.c
DEP_CPP_NPC_H=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\npc_humanoid_wp.c
DEP_CPP_NPC_HU=\
	"..\botai\ai_main.h"\
	".\be_aas.h"\
	".\be_ai_goal.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\om_flagsys.c
DEP_CPP_OM_FL=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\om_spawnsys.c
DEP_CPP_OM_SP=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\progressbar\ProgressWnd.cpp
DEP_CPP_PROGR=\
	"..\uniquebot\progressbar\ProgressWnd.h"\
	"..\uniquebot\progressbar\StdAfx.h"\
	
# End Source File
# Begin Source File

SOURCE=.\q_math.c
DEP_CPP_Q_MAT=\
	"..\..\etmain\ui\menudef.h"\
	"..\cgame\cg_local.h"\
	"..\cgame\cg_public.h"\
	"..\cgame\tr_types.h"\
	"..\ui\keycodes.h"\
	"..\ui\ui_shared.h"\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=.\q_shared.c
DEP_CPP_Q_SHA=\
	".\bg_lib.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_ai.c
DEP_CPP_UNIQU=\
	"..\botai\ai_cmd.h"\
	"..\botai\ai_distances.h"\
	"..\botai\ai_dmnet_mp.h"\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	"..\botai\chars.h"\
	"..\botai\inv.h"\
	".\be_aas.h"\
	".\be_ai_char.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ai_weap.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_attack.c
DEP_CPP_UNIQUE=\
	"..\botai\ai_main.h"\
	"..\uniquebot\unique_includes.h"\
	".\be_aas.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_autowaypoint.c
DEP_CPP_UNIQUE_=\
	"..\botai\ai_main.h"\
	".\be_aas.h"\
	".\be_ai_goal.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_coverspots.c
DEP_CPP_UNIQUE_C=\
	"..\botai\ai_main.h"\
	"..\uniquebot\unique_includes.h"\
	".\be_aas.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_fireteam_ai.c
DEP_CPP_UNIQUE_F=\
	"..\botai\ai_main.h"\
	"..\uniquebot\unique_includes.h"\
	".\be_aas.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_goal.c
DEP_CPP_UNIQUE_G=\
	"..\botai\ai_cmd.h"\
	"..\botai\ai_dmgoal_mp.h"\
	"..\botai\ai_dmnet_mp.h"\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	".\be_aas.h"\
	".\be_ai_char.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ai_weap.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_items.c
DEP_CPP_UNIQUE_I=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_mapping.c
DEP_CPP_UNIQUE_M=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_mem.c
DEP_CPP_UNIQUE_ME=\
	"..\botai\ai_main.h"\
	".\be_aas.h"\
	".\be_ai_goal.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_movement.c
DEP_CPP_UNIQUE_MO=\
	"..\botai\ai_main.h"\
	"..\uniquebot\unique_includes.h"\
	".\be_aas.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_navigation.c
DEP_CPP_UNIQUE_N=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_nodes.c
DEP_CPP_UNIQUE_NO=\
	"..\botai\ai_main.h"\
	".\be_aas.h"\
	".\be_ai_goal.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_sp_movement.c
DEP_CPP_UNIQUE_S=\
	"..\botai\ai_main.h"\
	"..\uniquebot\unique_includes.h"\
	".\be_aas.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_spawn.c
DEP_CPP_UNIQUE_SP=\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\botai.h"\
	".\ai_cast.h"\
	".\ai_cast_fight.h"\
	".\ai_cast_global.h"\
	".\be_aas.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_utils.c
DEP_CPP_UNIQUE_U=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_vehicle_ai.c
DEP_CPP_UNIQUE_V=\
	"..\botai\ai_cmd.h"\
	"..\botai\ai_distances.h"\
	"..\botai\ai_dmnet_mp.h"\
	"..\botai\ai_dmq3.h"\
	"..\botai\ai_main.h"\
	"..\botai\ai_team.h"\
	"..\botai\botai.h"\
	"..\botai\chars.h"\
	"..\botai\inv.h"\
	".\be_aas.h"\
	".\be_ai_char.h"\
	".\be_ai_chat.h"\
	".\be_ai_gen.h"\
	".\be_ai_goal.h"\
	".\be_ai_move.h"\
	".\be_ai_weap.h"\
	".\be_ea.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\botlib.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_visibility.c
DEP_CPP_UNIQUE_VI=\
	".\be_aas.h"\
	".\bg_lib.h"\
	".\bg_physics.h"\
	".\bg_public.h"\
	".\etenhanced.h"\
	".\g_local.h"\
	".\g_public.h"\
	".\g_shrubbot.h"\
	".\g_team.h"\
	".\g_xpsave.h"\
	".\q_global_defines.h"\
	".\q_shared.h"\
	".\surfaceflags.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\botai\ai_botScriptTrigger.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_botScriptTriggerX.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_cmd.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_distances.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmgoal_mp.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmgoal_sp.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmnet_mp.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmnet_sp.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmq3.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_main.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_matrix.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_team.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_teamX.h
# End Source File
# Begin Source File

SOURCE=.\be_aas.h
# End Source File
# Begin Source File

SOURCE=.\be_ai_char.h
# End Source File
# Begin Source File

SOURCE=.\be_ai_chat.h
# End Source File
# Begin Source File

SOURCE=.\be_ai_gen.h
# End Source File
# Begin Source File

SOURCE=.\be_ai_goal.h
# End Source File
# Begin Source File

SOURCE=.\be_ai_move.h
# End Source File
# Begin Source File

SOURCE=.\be_ai_weap.h
# End Source File
# Begin Source File

SOURCE=.\be_ea.h
# End Source File
# Begin Source File

SOURCE=.\bg_classes.h
# End Source File
# Begin Source File

SOURCE=.\bg_local.h
# End Source File
# Begin Source File

SOURCE=.\bg_malloc.h
# End Source File
# Begin Source File

SOURCE=.\bg_physics.h
# End Source File
# Begin Source File

SOURCE=.\bg_public.h
# End Source File
# Begin Source File

SOURCE=..\botai\botai.h
# End Source File
# Begin Source File

SOURCE=.\botlib.h
# End Source File
# Begin Source File

SOURCE=..\botai\chars.h
# End Source File
# Begin Source File

SOURCE=.\etenhanced.h
# End Source File
# Begin Source File

SOURCE=.\etpro_mdx.h
# End Source File
# Begin Source File

SOURCE=.\etpro_mdx_lut.h
# End Source File
# Begin Source File

SOURCE=.\g_local.h
# End Source File
# Begin Source File

SOURCE=.\g_public.h
# End Source File
# Begin Source File

SOURCE=.\g_shrubbot.h
# End Source File
# Begin Source File

SOURCE=.\g_team.h
# End Source File
# Begin Source File

SOURCE=.\g_xpsave.h
# End Source File
# Begin Source File

SOURCE=..\botai\inv.h
# End Source File
# Begin Source File

SOURCE=..\botai\match.h
# End Source File
# Begin Source File

SOURCE=..\uniquebot\progressbar\ProgressWnd.h
# End Source File
# Begin Source File

SOURCE=.\q_global_defines.h
# End Source File
# Begin Source File

SOURCE=.\q_shared.h
# End Source File
# Begin Source File

SOURCE=.\surfaceflags.h
# End Source File
# Begin Source File

SOURCE=..\botai\syn.h
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_includes.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\uniquebot\progressbar\res\ProgressDemo.ico
# End Source File
# Begin Source File

SOURCE=..\uniquebot\progressbar\ProgressDemo.rc
# End Source File
# Begin Source File

SOURCE=..\uniquebot\progressbar\res\ProgressDemoDoc.ico
# End Source File
# Begin Source File

SOURCE=..\uniquebot\progressbar\res\Toolbar.bmp
# End Source File
# End Group
# End Target
# End Project
