#include "q_shared.h"
#include "bg_public.h"
#include "bg_classes.h"

//{WP_PPSH,							WP_STG44,		WP_PPS,									WP_SVT40,							WP_30CAL,									WP_STEN,										WP_GARAND,								WP_PANZERFAUST,						WP_FLAMETHROWER,			WP_KAR98,				WP_CARBINE,					WP_FG42,		WP_K43,		WP_MOBILE_MG42,		WP_MORTAR,		WP_MP40,		WP_THOMPSON,		0 },

bg_playerclass_t bg_allies_playerclasses[NUM_PLAYER_CLASSES] = {
	{	
		PC_SOLDIER,
#ifdef __ORIGINAL_PLAYER_SKINS__
		"characters/temperate/allied/soldier.char",
#else //!__ORIGINAL_PLAYER_SKINS__
		"allied/soldier.char",
#endif //__ORIGINAL_PLAYER_SKINS__
		"ui/assets/mp_gun_blue.tga",
		"ui/assets/mp_arrow_blue.tga",
		{ 
#ifdef __EF__
			WP_PPSH,
			WP_PPS,
			WP_30CAL,
			WP_PTRS,//Masteries
#else //!__EF__
			WP_MOBILE_MG42,
			WP_THOMPSON,
#endif //__EF__
			WP_FLAMETHROWER,
			WP_PANZERFAUST,
			WP_MORTAR
		},
	},

	{
		PC_MEDIC,
#ifdef __ORIGINAL_PLAYER_SKINS__
		"characters/temperate/allied/medic.char",
#else //!__ORIGINAL_PLAYER_SKINS__
		"allied/medic.char",
#endif //__ORIGINAL_PLAYER_SKINS__
		"ui/assets/mp_health_blue.tga",
		"ui/assets/mp_arrow_blue.tga",
		{ 
#ifdef __EF__
			WP_PPSH,
			WP_PPS,
			WP_SVT40,
#else //!__EF__
			WP_THOMPSON,
#endif //__EF__
		},	
	},

	{
		PC_ENGINEER,
#ifdef __ORIGINAL_PLAYER_SKINS__
		"characters/temperate/allied/engineer.char",
#else //!__ORIGINAL_PLAYER_SKINS__
		"allied/engineer.char",
#endif //__ORIGINAL_PLAYER_SKINS__
		"ui/assets/mp_wrench_blue.tga",
		"ui/assets/mp_arrow_blue.tga",
		{ 
#ifdef __EF__
			WP_PPSH,
			WP_SVT40,
			WP_PPS,
#else //!__EF__
			WP_THOMPSON,
#endif //__EF__
			WP_CARBINE,
		},	
	},

	{
		PC_FIELDOPS,
#ifdef __ORIGINAL_PLAYER_SKINS__
		"characters/temperate/allied/fieldops.char",
#else //!__ORIGINAL_PLAYER_SKINS__
		"allied/fieldops.char",
#endif //__ORIGINAL_PLAYER_SKINS__
		"ui/assets/mp_ammo_blue.tga",
		"ui/assets/mp_arrow_blue.tga",
		{ 
#ifdef __EF__
			WP_PPSH,
			WP_SVT40,
			WP_PPS,
#else //!__EF__
			WP_THOMPSON,
#endif //__EF__
		},	
	},

	{
		PC_COVERTOPS,
#ifdef __ORIGINAL_PLAYER_SKINS__
		"characters/temperate/allied/cvops.char",
#else //!__ORIGINAL_PLAYER_SKINS__
		"allied/cvops.char",
#endif //__ORIGINAL_PLAYER_SKINS__
		"ui/assets/mp_spy_blue.tga",
		"ui/assets/mp_arrow_blue.tga",
		{ 
			WP_STEN,
			WP_FG42,
			WP_GARAND,
#ifdef __EF__
			WP_SVT40,
#endif //__EF__
		},	
	},
};

bg_playerclass_t bg_axis_playerclasses[NUM_PLAYER_CLASSES] = {
	{
		PC_SOLDIER,
#ifdef __ORIGINAL_PLAYER_SKINS__
		"characters/temperate/axis/soldier.char",
#else //!__ORIGINAL_PLAYER_SKINS__
		"axis/soldier.char",
#endif //__ORIGINAL_PLAYER_SKINS__
		"ui/assets/mp_gun_red.tga",
		"ui/assets/mp_arrow_red.tga",
		{ 
#ifdef __EF__
			WP_STG44,
            WP_PTRS,//Masteries, for early tests ingame; later, germans 
#endif //__EF__     //will have their own antitank rifle
			WP_MP40,
			WP_MOBILE_MG42,
			WP_FLAMETHROWER,
			WP_PANZERFAUST,
			WP_MORTAR
		},	
	},

	{
		PC_MEDIC,
#ifdef __ORIGINAL_PLAYER_SKINS__
		"characters/temperate/axis/medic.char",
#else //!__ORIGINAL_PLAYER_SKINS__
		"axis/medic.char",
#endif //__ORIGINAL_PLAYER_SKINS__
		"ui/assets/mp_health_red.tga",
		"ui/assets/mp_arrow_red.tga",
		{ 
			WP_MP40,
			WP_KAR98,
#ifdef __EF__
			WP_STG44,
#endif //__EF__
		},	
	},

	{
		PC_ENGINEER,
#ifdef __ORIGINAL_PLAYER_SKINS__
		"characters/temperate/axis/engineer.char",
#else //!__ORIGINAL_PLAYER_SKINS__
		"axis/engineer.char",
#endif //__ORIGINAL_PLAYER_SKINS__
		"ui/assets/mp_wrench_red.tga",
		"ui/assets/mp_arrow_red.tga",
		{ 
			WP_MP40,
			WP_KAR98,
#ifdef __EF__
			WP_STG44,
#endif //__EF__
		},	
	},

	{
		PC_FIELDOPS,
#ifdef __ORIGINAL_PLAYER_SKINS__
		"characters/temperate/axis/fieldops.char",
#else //!__ORIGINAL_PLAYER_SKINS__
		"axis/fieldops.char",
#endif //__ORIGINAL_PLAYER_SKINS__
		"ui/assets/mp_ammo_red.tga",
		"ui/assets/mp_arrow_red.tga",
		{ 
			WP_MP40,
			//WP_STEN, // UQ1: Sucks, but they need another option...
			WP_KAR98,
#ifdef __EF__
			WP_STG44,
#endif //__EF__
		},	
	},

	{
		PC_COVERTOPS,
#ifdef __ORIGINAL_PLAYER_SKINS__
		"characters/temperate/axis/cvops.char",
#else //!__ORIGINAL_PLAYER_SKINS__
		"axis/cvops.char",
#endif //__ORIGINAL_PLAYER_SKINS__
		"ui/assets/mp_spy_red.tga",
		"ui/assets/mp_arrow_red.tga",
		{ 
			WP_STEN,
			WP_FG42,
			WP_K43,
#ifdef __EF__
			WP_STG44,
#endif //__EF__
		},	
	},
};

bg_playerclass_t* BG_GetPlayerClassInfo( int team, int cls ) {
	bg_playerclass_t* teamList;

	if( cls < PC_SOLDIER || cls >= NUM_PLAYER_CLASSES ) {
		cls = PC_SOLDIER;
	}

	switch( team ) {
		default:
		case TEAM_AXIS:
			teamList = bg_axis_playerclasses;
			break;
		case TEAM_ALLIES:
			teamList = bg_allies_playerclasses;
			break;
	}

	return &teamList[cls];
}

bg_playerclass_t* BG_PlayerClassForPlayerState(playerState_t* ps) {
	return BG_GetPlayerClassInfo(ps->persistant[PERS_TEAM], ps->stats[STAT_PLAYER_CLASS]);
}

qboolean BG_ClassHasWeapon(bg_playerclass_t* classInfo, weapon_t weap) {
	int i;

	if(!weap) {
		return qfalse;
	}

	for( i = 0; i < MAX_WEAPS_PER_CLASS; i++) {
		if(classInfo->classWeapons[i] == weap) {
			return qtrue;
		}
	}
	return qfalse;
}

qboolean BG_WeaponIsPrimaryForClassAndTeam( int classnum, team_t team, weapon_t weapon )
{
	bg_playerclass_t *classInfo;

	if( team == TEAM_ALLIES ) {
		classInfo = &bg_allies_playerclasses[classnum];

		if( BG_ClassHasWeapon( classInfo, weapon ) ) {
			return qtrue;
		}
	} else if( team == TEAM_AXIS ) {
		classInfo = &bg_axis_playerclasses[classnum];

		if( BG_ClassHasWeapon( classInfo, weapon ) ) {
			return qtrue;
		}
	}

	return qfalse;
}

const char* BG_ShortClassnameForNumber( int classNum ) {
	switch( classNum ) {
		case PC_SOLDIER:
			return "Soldr";
		case PC_MEDIC:
			return "Medic";
		case PC_ENGINEER:
			return "Engr";
		case PC_FIELDOPS:
			return "FdOps";
		case PC_COVERTOPS:
			return "CvOps";
		default:
			return "^1ERROR";
	}
}

const char* BG_ClassnameForNumber( int classNum ) {
	switch( classNum ) {
		case PC_SOLDIER:
			return "Soldier";
		case PC_MEDIC:
			return "Medic";
		case PC_ENGINEER:
			return "Engineer";
		case PC_FIELDOPS:
			return "Field Ops";
		case PC_COVERTOPS:
			return "Covert Ops";
		default:
			return "^1ERROR";
	}
}

const char* BG_ClassLetterForNumber( int classNum ) {
	switch( classNum ) {
		case PC_SOLDIER:
			return "S";
		case PC_MEDIC:
			return "M";
		case PC_ENGINEER:
			return "E";
		case PC_FIELDOPS:
			return "F";
		case PC_COVERTOPS:
			return "C";
		default:
			return "^1E";
	}
}

int BG_ClassTextToClass(char *token) {
	if (!Q_stricmp(token, "soldier")) {
		return PC_SOLDIER;
	} else if (!Q_stricmp(token, "medic")) {
		return PC_MEDIC;
	} else if (!Q_stricmp(token, "lieutenant")) { // FIXME: remove from missionpack
		return PC_FIELDOPS;
	} else if (!Q_stricmp(token, "fieldops")) {
		return PC_FIELDOPS;
	} else if (!Q_stricmp(token, "engineer")) {
		return PC_ENGINEER;
	} else if (!Q_stricmp(token, "covertops")) {
		return PC_COVERTOPS;
	}

	return -1;
}

skillType_t BG_ClassSkillForClass( int classnum ) {
	skillType_t classskill[NUM_PLAYER_CLASSES] = { SK_HEAVY_WEAPONS, SK_FIRST_AID, SK_EXPLOSIVES_AND_CONSTRUCTION, SK_SIGNALS, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS };

	if( classnum < 0 || classnum >= NUM_PLAYER_CLASSES ) {
		return SK_BATTLE_SENSE;
	}

	return classskill[ classnum ];
}

