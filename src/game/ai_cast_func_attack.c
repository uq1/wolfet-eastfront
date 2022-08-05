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

//=================================================================================
//
// ZOMBIE SPECIAL ATTACKS
//
//=================================================================================

/*
============
AIFunc_ZombieFlameAttack()

  Zombie "Flaming Bats" attack.

  NOTE: this actually uses the EFFECT3 slot for client-side effects (others are taken)
============
*/

#define	ZOMBIE_FLAME_DURATION		4000

char *AIFunc_ZombieFlameAttack( cast_state_t *cs )
{
	//
	//
	return NULL;
}

char *AIFunc_ZombieFlameAttackStart( cast_state_t *cs )
{
	return "AIFunc_ZombieFlameAttack";
}



/*
============
AIFunc_ZombieAttack2()

  Zombie "Evil Spirit" attack.

  Character draws the light from surrounding walls (expanding negative light) and builds
  up to the release of a flying translucent skull with trail effect (and beady eyes).

  Spirit should track it's enemy slightly, inflicting lots of damage, removing sprint bar,
  and effecting sight temporarily.

  Speed of spirit is effected by skill level, higher skill = faster speed

  Spirits inflicting AI soldiers should kill instantly, removing all flesh from the
  soldier's face (draw skull under head model, then fade head model away over a short period).
============
*/
extern void weapon_zombiespirit (gentity_t *ent, gentity_t *missile);

#define ZOMBIE_SPIRIT_BUILDUP_TIME		6000	// last for this long
#define ZOMBIE_SPIRIT_FADEOUT_TIME		1000
#define ZOMBIE_SPIRIT_DLIGHT_RADIUS_MAX	256
#define	ZOMBIE_SPIRIT_FIRE_INTERVAL		1000

int		lastZombieSpiritAttack;

char *AIFunc_ZombieAttack2( cast_state_t *cs )
{

	//
	//
	return NULL;
}

char *AIFunc_ZombieAttack2Start( cast_state_t *cs )
{
	return "AIFunc_ZombieAttack2";
}


/*
================
AIFunc_ZombieMelee
================
*/

int zombieHitDamage[5] = {
	16,
	16,
	16,
	12,
	20
};

#define	NUM_ZOMBIE_ANIMS	5
int zombieHitTimes[NUM_ZOMBIE_ANIMS][3] = {	// up to three hits per attack
	{ANIMLENGTH(11,20),-1},
	{ANIMLENGTH(9,20),-1},
	{ANIMLENGTH(9,20),-1},
	{ANIMLENGTH(8,20),ANIMLENGTH(16,20),-1},
	{ANIMLENGTH(8,20),ANIMLENGTH(15,20),ANIMLENGTH(24,20)},
};

char *AIFunc_ZombieMelee( cast_state_t *cs )
{
	return NULL;
}

/*
================
AIFunc_ZombieMeleeStart
================
*/
char *AIFunc_ZombieMeleeStart( cast_state_t *cs )
{
	return "AIFunc_ZombieMelee";
}

//=================================================================================
//
// LOPER MELEE ATTACK
//
//=================================================================================

#define	NUM_LOPERMELEE_ANIMS	2
int loperHitTimes[NUM_LOPERMELEE_ANIMS] = {
	(ANIMLENGTH(3,15)),
	(ANIMLENGTH(6,15))
};

#define	LOPER_MELEE_DAMAGE			20
#define	LOPER_MELEE_RANGE			48

/*
===============
AIFunc_LoperAttack1()

  Loper's close range melee attack
===============
*/
char *AIFunc_LoperAttack1( cast_state_t *cs )
{
	return NULL;
}


char *AIFunc_LoperAttack1Start( cast_state_t *cs )
{
	return "AIFunc_LoperAttack1";
}

//=================================================================================
//
// LOPER LEAP ATTACK
//
//=================================================================================

#define	LOPER_LEAP_ANIM				LEGS_EXTRA3
#define	LOPER_LEAP_FPS				15

// update for a version of the loper new today (6/26)
//#define	LOPER_LEAP_FRAME_COUNT		4
#define	LOPER_LEAP_FRAME_COUNT		10
#define	LOPER_LEAP_DURATION			(LOPER_LEAP_FRAME_COUNT*(1000/LOPER_LEAP_FPS))


#define	LOPER_LAND_ANIM				LEGS_EXTRA4
#define	LOPER_LAND_FPS				15

// update for a version of the loper new today (6/26)
//#define	LOPER_LAND_FRAME_COUNT		17
#define	LOPER_LAND_FRAME_COUNT		21
#define	LOPER_LAND_DURATION			(LOPER_LAND_FRAME_COUNT*(1000/LOPER_LAND_FPS))


#define	LOPER_LEAP_DAMAGE			10
#define	LOPER_LEAP_DELAY			100	// note, this needs to be something, or they'll think they're done before they even started
#define	LOPER_LEAP_RANGE			200
#define	LOPER_LEAP_VELOCITY_START	750.0
#define	LOPER_LEAP_VELOCITY_END		1050.0
#define	LOPER_LEAP_VELOCITY_Z		300
#define	LOPER_LEAP_LAND_MOMENTUM	250

/*
===============
AIFunc_LoperAttack2()

  Loper's leaping long range attack
===============
*/
char *AIFunc_LoperAttack2( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_LoperAttack2Start( cast_state_t *cs )
{
	return "AIFunc_LoperAttack2";
}

//=================================================================================
//
// LOPER GROUND ATTACK
//
//=================================================================================

#define	LOPER_GROUND_ANIM				LEGS_EXTRA5
#define	LOPER_GROUND_FPS				10

#define	LOPER_GROUND_FRAME_COUNT		8
#define	LOPER_GROUND_DURATION			(LOPER_GROUND_FRAME_COUNT*(1000/LOPER_GROUND_FPS))

#define	LOPER_GROUND_DAMAGE				20

/*
===============
AIFunc_LoperAttack3()

  Loper's ground electrical attack
===============
*/
char *AIFunc_LoperAttack3( cast_state_t *cs )
{
	//
	return NULL;
}

char *AIFunc_LoperAttack3Start( cast_state_t *cs )
{
	return "AIFunc_LoperAttack3";
}

//=================================================================================
//
// STIM SOLDIER FLYING ATTACK
//
//=================================================================================

#define	STIMSOLDIER_FLYJUMP_ANIM			LEGS_EXTRA1
#define	STIMSOLDIER_FLYJUMP_FPS				15
#define	STIMSOLDIER_FLYJUMP_FRAME_COUNT		28
#define	STIMSOLDIER_FLYJUMP_DURATION		(STIMSOLDIER_FLYJUMP_FRAME_COUNT*(1000/STIMSOLDIER_FLYJUMP_FPS))
#define	STIMSOLDIER_FLYJUMP_DELAY			(STIMSOLDIER_FLYJUMP_DURATION+3000)

// hover plays continuously
#define	STIMSOLDIER_FLYHOVER_ANIM			LEGS_EXTRA2
#define	STIMSOLDIER_FLYHOVER_FPS			5

#define	STIMSOLDIER_FLYLAND_ANIM			LEGS_LAND
#define	STIMSOLDIER_FLYLAND_FPS				15
#define	STIMSOLDIER_FLYLAND_FRAME_COUNT		14
#define	STIMSOLDIER_FLYLAND_DURATION		(STIMSOLDIER_FLYLAND_FRAME_COUNT*(1000/STIMSOLDIER_FLYLAND_FPS))

#define	STIMSOLDIER_STARTJUMP_DELAY			(STIMSOLDIER_FLYJUMP_DURATION*0.5)

char *AIFunc_StimSoldierAttack1( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_StimSoldierAttack1Start( cast_state_t *cs )
{
	return "AIFunc_StimSoldierAttack1";
}

//=================================================================================
//
// STIM SOLDIER DUAL MACHINEGUN ATTACK
//
//=================================================================================

char *AIFunc_StimSoldierAttack2( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_StimSoldierAttack2Start( cast_state_t *cs )
{
	return "AIFunc_StimSoldierAttack2";
}

//=================================================================================
//
// BLACK GUARD MELEE KICK ATTACK
//
//=================================================================================

#define	BLACKGUARD_KICK_DELAY	300
#define	BLACKGUARD_KICK_RANGE	BLACKGUARD_MELEE_RANGE + 16
#define BLACKGUARD_KICK_DAMAGE	25

char *AIFunc_BlackGuardAttack1( cast_state_t *cs )
{
	return NULL;
}

char *AIFunc_BlackGuardAttack1Start( cast_state_t *cs )
{
	return "AIFunc_BlackGuardAttack1";
}


//=================================================================================
//
// REJECT X-CREATURE
//
//	Attacks are: backhand slap, blowtorch (small flamethrower)
//
//=================================================================================

////// Backhand attack (slap)

/*
==============
AIFunc_RejectAttack1
==============
*/
char *AIFunc_RejectAttack1( cast_state_t *cs )
{
	return NULL;
}

/*
==============
AIFunc_RejectAttack1Start
==============
*/
char *AIFunc_RejectAttack1Start( cast_state_t *cs ) {
	return "AIFunc_RejectAttack1";
}


//=================================================================================
//
// WARRIOR ZOMBIE
//
// Standing melee attacks
//
//=================================================================================

int warriorHitDamage[5] = {
	16,
	16,
	16,
	12,
	20
};

#define	NUM_WARRIOR_ANIMS	5
int warriorHitTimes[NUM_WARRIOR_ANIMS][3] = {	// up to three hits per attack
	{ANIMLENGTH(10,20),-1},
	{ANIMLENGTH(15,20),-1},
	{ANIMLENGTH(18,20),-1},
	{ANIMLENGTH(15,20),-1},
	{ANIMLENGTH(14,20),-1},
};

/*
================
AIFunc_WarriorZombieMelee
================
*/
char *AIFunc_WarriorZombieMelee( cast_state_t *cs )
{
	return NULL;
}

/*
================
AIFunc_WarriorZombieMeleeStart
================
*/
char *AIFunc_WarriorZombieMeleeStart( cast_state_t *cs )
{
	return "AIFunc_WarriorZombieMelee";
}

// Warrior "sight" animation
/*
================
AIFunc_WarriorZombieSight
================
*/
char *AIFunc_WarriorZombieSight( cast_state_t *cs )
{
	return NULL;
}

/*
================
AIFunc_WarriorZombieSightStart
================
*/
char *AIFunc_WarriorZombieSightStart( cast_state_t *cs )
{
	return NULL;
}

/*
================
AIFunc_WarriorZombieDefense
================
*/
char *AIFunc_WarriorZombieDefense( cast_state_t *cs )
{
	return NULL;
}

/*
================
AIFunc_WarriorZombieDefenseStart
================
*/
char *AIFunc_WarriorZombieDefenseStart( cast_state_t *cs )
{
	return "AIFunc_WarriorZombieDefense";
}
#endif //__AAS_BOTS__
