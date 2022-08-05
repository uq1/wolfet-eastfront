#include "q_shared.h"
#ifdef __AAS_BOTS__
//===========================================================================
//
// Name:			ai_cast_funcs.c
// Function:		Wolfenstein AI Character Decision Making
// Programmer:		Ridah
// Tab Size:		4 (real tabs)
//===========================================================================

#include "../game/g_local.h"
#include "../game/q_shared.h"
#include "../game/botlib.h"		//bot lib interface
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../botai/botai.h"			//bot ai interface

#include "ai_cast.h"
#include "ai_cast_global.h"

// TTimo: unused
//static vec3_t forward, right, up;

//=================================================================================
//
// Helga, the first boss
//
//=================================================================================

#define HELGA_SPIRIT_BUILDUP_TIME		8000	// last for this long
#define HELGA_SPIRIT_FADEOUT_TIME		1000
#define HELGA_SPIRIT_DLIGHT_RADIUS_MAX	384
#define	HELGA_SPIRIT_FIRE_INTERVAL		1000

extern int		lastZombieSpiritAttack;

char *AIFunc_Helga_SpiritAttack( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_Helga_SpiritAttack_Start( cast_state_t *cs )
{
	return "AIFunc_Helga_SpiritAttack";
}

//=================================================================================
//
// Standing melee attacks
//
//=================================================================================

#define	NUM_HELGA_ANIMS		3
#define	MAX_HELGA_IMPACTS	3
int helgaHitTimes[NUM_HELGA_ANIMS][MAX_HELGA_IMPACTS] = {	// up to three hits per attack
	{ANIMLENGTH(16,20),-1},
	{ANIMLENGTH(11,20),ANIMLENGTH(19,20),-1},
	{ANIMLENGTH(10,20),ANIMLENGTH(17,20),ANIMLENGTH(26,20)},
};
int helgaHitDamage[NUM_HELGA_ANIMS] = {
	20,
	14,
	12
};

/*
================
AIFunc_Helga_Melee
================
*/
char *AIFunc_Helga_Melee( cast_state_t *cs )
{
	return NULL;
}

/*
================
AIFunc_Helga_MeleeStart
================
*/
char *AIFunc_Helga_MeleeStart( cast_state_t *cs )
{
	return "AIFunc_Helga_Melee";
}


//===================================================================

/*
==============
AIFunc_FlameZombie_Portal
==============
*/
char *AIFunc_FlameZombie_Portal( cast_state_t *cs )
{
	return NULL;
}

/*
==============
AIFunc_FlameZombie_PortalStart
==============
*/
char *AIFunc_FlameZombie_PortalStart( cast_state_t *cs )
{
	return "AIFunc_FlameZombie_Portal";
}


//=================================================================================
//
// Heinrich, the LAST boss
//
//=================================================================================

//
// Special Sound Precache
//

typedef enum
{
	HEINRICH_SWORDIMPACT,
	HEINRICH_SWORDLUNGE_START,
	HEINRICH_SWORDKNOCKBACK_START,
	HEINRICH_SWORDKNOCKBACK_WEAPON,
	HEINRICH_SWORDSIDESLASH_START,
	HEINRICH_SWORDSIDESLASH_WEAPON,
	HEINRICH_EARTHQUAKE_START,
	HEINRICH_RAISEDEAD_START,
	HEINRICH_TAUNT_GOODHEALTH,
	HEINRICH_TAUNT_LOWHEALTH,

	MAX_HEINRICH_SOUNDS
} heinrichSounds_t;

char *heinrichSounds[MAX_HEINRICH_SOUNDS] = {
	"heinrichSwordImpact",
	"heinrichSwordLungeStart",
	"heinrichSwordKnockbackStart",
	"heinrichSwordKnockbackWeapon",
	"heinrichSwordSideSlashStart",
	"heinrichSwordSideSlashWeapon",
	"heinrichSwordEarthquakeStart",
	"heinrichRaiseWarriorStart",
	"heinrichTauntGoodHealth",
	"heinrichTauntLowHealth",
};

int	heinrichSoundIndex[MAX_HEINRICH_SOUNDS];

void AICast_Heinrich_SoundPrecache( void )
{
}

void AICast_Heinrich_Taunt( cast_state_t *cs )
{
}

#define HEINRICH_LUNGE_DELAY	ANIMLENGTH(15,20)
#define	HEINRICH_LUNGE_RANGE	170
#define	HEINRICH_LUNGE_DAMAGE	(50+rand()%20)

char *AIFunc_Heinrich_SwordLunge( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_Heinrich_SwordLungeStart( cast_state_t *cs )
{
	return "AIFunc_Heinrich_SwordLunge";
}

#define HEINRICH_KNOCKBACK_DELAY	ANIMLENGTH(26,20)
#define	HEINRICH_KNOCKBACK_RANGE	150
#define	HEINRICH_KNOCKBACK_DAMAGE	(60+rand()%20)

char *AIFunc_Heinrich_SwordKnockback( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_Heinrich_SwordKnockbackStart( cast_state_t *cs )
{
	return "AIFunc_Heinrich_SwordKnockback";
}

#define HEINRICH_SLASH_DELAY	ANIMLENGTH(17,25)
#define	HEINRICH_SLASH_RANGE	140
#define	HEINRICH_SLASH_DAMAGE	(30+rand()%15)

char *AIFunc_Heinrich_SwordSideSlash( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_Heinrich_SwordSideSlashStart( cast_state_t *cs )
{
	return "AIFunc_Heinrich_SwordSideSlash";
}

#define	HEINRICH_STOMP_DELAY		900
#define	HEINRICH_STOMP_RANGE		1024.0
#define	HEINRICH_STOMP_VELOCITY_Z	420
#define	HEINRICH_STOMP_DAMAGE		35

char *AIFunc_Heinrich_Earthquake( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_Heinrich_MeleeStart( cast_state_t *cs )
{
	// shutup compiler
	return NULL;
}

#define	HEINRICH_RAISEDEAD_DELAY		1200
#define	HEINRICH_RAISEDEAD_COUNT		3
int	lastRaise;

char *AIFunc_Heinrich_RaiseDead( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_Heinrich_RaiseDeadStart( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_Heinrich_SpawnSpiritsStart( cast_state_t *cs )
{
	return NULL;
}
#endif //__AAS_BOTS__
