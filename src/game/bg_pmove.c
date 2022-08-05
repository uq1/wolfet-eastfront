

// bg_pmove.c -- both games player movement code
// takes a playerstate and a usercmd as input and returns a modifed playerstate
//add barrel code for self-propelled guns(jagpanther, stug3...) Masteries 23/12/2007 line 6470
//and for Antitank Artillery  line 4667

#ifdef CGAMEDLL
#include "../cgame/cg_local.h"
#else
#include "q_shared.h"
#include "bg_public.h"
#endif // CGAMEDLL

#ifdef GAMEDLL
#include "g_local.h"
#endif //GAMEDLL
#include "bg_local.h"

qboolean BG_CanSetupMobileMG42Here ( void ); // UQ1: below!

#ifdef __VEHICLES__
extern qboolean	PM_VehicleTraces ( float time_left, trace_t tr );
#endif //__VEHICLES__

#ifdef CGAMEDLL
#define PM_GameType cg_gameType.integer
#define PM_UW_SYRINGE 0 // tjw: no client mod
#define PM_UW_PLIERS 0 // tjw: no client mod
#define PM_CORPSE_SPIN 0 // tjw: no client mod
#define PM_DOUBLE_JUMP (cgs.misc & MISC_DOUBLE_JUMP) // josh: no client mod
#define PM_DOUBLE_JUMP_HEIGHT cgs.doubleJumpHeight // josh: added to client
#define PM_GARAND_RELOADS 0 // tjw: no client mod
#define PM_STAMINA_RECHARGE cgs.staminaRecharge
#define PM_JUMP_STAMINA (cgs.misc & MISC_JUMP_STAMINA) 
#define PM_TRACEHEAD (cgs.misc & MISC_TRACEHEAD)

#elif GAMEDLL
#define PM_GameType g_gametype.integer
#define PM_UW_SYRINGE (g_weapons.integer & WPF_UNDERWATER_SYRINGE)
#define PM_UW_PLIERS (g_weapons.integer & WPF_UNDERWATER_PLIERS)
#define PM_CORPSE_SPIN g_spinCorpse.integer
#define PM_DOUBLE_JUMP (g_misc.integer & MISC_DOUBLE_JUMP)
#define PM_DOUBLE_JUMP_HEIGHT g_doubleJumpHeight.value
#define PM_GARAND_RELOADS (g_weapons.integer & WPF_GARAND_RELOADS)
#define PM_STAMINA_RECHARGE g_staminaRecharge.value // Perro
#define PM_JUMP_STAMINA (g_misc.integer & MISC_JUMP_STAMINA)
#define PM_TRACEHEAD (g_misc.integer & MISC_TRACEHEAD)
#endif

#define PM_IsSinglePlayerGame() (PM_GameType == GT_SINGLE_PLAYER || PM_GameType == GT_COOP || PM_GameType == GT_NEWCOOP)

vec3_t	tempTankTraceMaxs = {64, 64, 78};
vec3_t	tempTankTraceMins = {-64, -64, -24};

// JPW NERVE -- stuck this here so it can be seen client & server side
float Com_GetFlamethrowerRange(void) {
	return 2500; // multiplayer range is longer for balance
}
// jpw

pmove_t		*pm;
pml_t		pml;

// movement parameters
float	pm_stopspeed = 100;
//float	pm_duckScale = 0.25;

//vec3_t	tankMaxs = {112, 112, 78};
//vec3_t	tankMins = {-112, -112, -24};
//vec3_t	tankMins = {-164, -112, -24};
//vec3_t	tankMaxs = {164, 164, 78};
//vec3_t	tankMins = {-164, -164, -24};
//vec3_t	tankMins = {-112, -112, -24};
//vec3_t	tankMaxs = {112, 112, 78};
vec3_t	tankMins = {-112, -112, -24};
vec3_t	tankMaxs = {112, 112, 48};

vec3_t	heavyTankMins = {-112, -112, -24};
vec3_t	heavyTankMaxs = {112, 112, 48};

vec3_t	mediumTankMins = {-96, -96, -24};
vec3_t	mediumTankMaxs = {96, 96, 40};

vec3_t	lightTankMins = {-80, -80, -24};
vec3_t	lightTankMaxs = {80, 80, 32};

//----(SA)	modified
float	pm_waterSwimScale	= 0.5;
float	pm_waterWadeScale	= 0.70;
float	pm_slagSwimScale	= 0.30;
float	pm_slagWadeScale	= 0.70;

float	pm_proneSpeedScale	= 0.21;	// was: 0.18 (too slow) then: 0.24 (too fast)

float	pm_accelerate		= 10;
float	pm_airaccelerate	= 1;
float	pm_wateraccelerate	= 4;
float	pm_slagaccelerate	= 2;
float	pm_flyaccelerate	= 8;

float	pm_friction			= 6;
float	pm_waterfriction	= 1;
float	pm_slagfriction		= 1;
float	pm_flightfriction	= 3;
float	pm_ladderfriction	= 14;
float	pm_spectatorfriction = 5.0f;

//----(SA)	end

int		c_pmove = 0;

#define TRIPMINE_RANGE 512.f


#ifdef GAMEDLL

// In just the GAME DLL, we want to store the groundtrace surface stuff,
// so we don't have to keep tracing.
void ClientStoreSurfaceFlags( int clientNum, int surfaceFlags);

#endif

qboolean BG_CanSetupMobileMG42Here ( void );

/*
===============
PM_AddEvent

===============
*/
void PM_AddEvent( int newEvent ) {
	BG_AddPredictableEventToPlayerstate( newEvent, 0, pm->ps );
}

void PM_AddEventExt( int newEvent, int eventParm ) {
	BG_AddPredictableEventToPlayerstate( newEvent, eventParm, pm->ps );
}

int PM_IdleAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
		case WP_SATCHEL_DET:
		case WP_MORTAR_SET:
		case WP_MEDIC_ADRENALINE:
		case WP_MOBILE_MG42_SET:
#ifdef __EF__
		case WP_30CAL_SET:
		case WP_PTRS_SET://Masteries, I think that this is correct
#endif //__EF__
			return WEAP_IDLE2;
		default:
			return WEAP_IDLE1;
	}
}

int PM_AltSwitchFromForWeapon ( int weapon ) {
	switch( weapon ) {
//		case WP_MEDIC_SYRINGE:
//			return WEAP_DROP;
		default:
			return WEAP_ALTSWITCHFROM;
	}
}

int PM_AltSwitchToForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
		case WP_MORTAR:
		case WP_MOBILE_MG42:
#ifdef __EF__
		case WP_30CAL:
		case WP_PTRS:
#endif //__EF__
			return WEAP_ALTSWITCHFROM;
//		case WP_MEDIC_SYRINGE:
//			return WEAP_RAISE;

		default:
			return WEAP_ALTSWITCHTO;
	}
}

int PM_AttackAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
		case WP_SATCHEL_DET:
		case WP_MEDIC_ADRENALINE:
		case WP_MOBILE_MG42_SET:
#ifdef __EF__
		case WP_30CAL_SET:
		case WP_PTRS_SET://Masteries, I think that this is correct
#endif //__EF__
			return WEAP_ATTACK2;

		default:
			return WEAP_ATTACK1;
	}
}

int PM_LastAttackAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
		case WP_MOBILE_MG42_SET:
#ifdef __EF__
		case WP_30CAL_SET:
		case WP_PTRS_SET://Masteries
#endif //__EF__
			return WEAP_ATTACK2;
		case WP_MORTAR_SET:
			return WEAP_ATTACK1;

		default:
			return WEAP_ATTACK_LASTSHOT;
	}
}

int PM_ReloadAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
			return WEAP_RELOAD2;
		case WP_MOBILE_MG42_SET:
#ifdef __EF__
		case WP_30CAL_SET:
		case WP_PTRS_SET://Masteries, hmmm...I am not sure
#endif //__EF__
			return WEAP_RELOAD3;
		default:
			if( pm )
			{
				if( pm->skill[SK_LIGHT_WEAPONS] >= 2 && BG_isLightWeaponSupportingFastReload( weapon )  )
					return WEAP_RELOAD2;	// faster reload
				else
					return WEAP_RELOAD1;
			}
			else
				return WEAP_RELOAD1;
	}
}

int PM_RaiseAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
			return WEAP_RELOAD3;
		case WP_MOBILE_MG42_SET:
#ifdef __EF__
		case WP_30CAL_SET:
		case WP_PTRS_SET://Masteries, hahaha  PTRS in all places
#endif //__EF__
			return WEAP_DROP2;
		case WP_SATCHEL_DET:
			return WEAP_RELOAD2;

		default:
			return WEAP_RAISE;
	}
}

int PM_DropAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
			return WEAP_DROP2;
		case WP_SATCHEL_DET:
			return WEAP_RELOAD1;

		default:
			return WEAP_DROP;
	}
}

/*
===============
PM_AddTouchEnt
===============
*/
void PM_AddTouchEnt( int entityNum ) {
	int		i;

	if ( entityNum == ENTITYNUM_WORLD ) {
		return;
	}
	if ( pm->numtouch == MAXTOUCH ) {
		return;
	}

	// see if it is already added
	for ( i = 0 ; i < pm->numtouch ; i++ ) {
		if ( pm->touchents[ i ] == entityNum ) {
			return;
		}
	}

	// add it
	pm->touchents[pm->numtouch] = entityNum;
	pm->numtouch++;
}

/*
===============
PM_AddVehicleTouchEnt
===============
*/
#ifdef __ADVANCED_VEHICLES__

void PM_AddVehicleTouchEnt( int entityNum, vec3_t hitOrigin ) {
// END
	int		i;

	if ( entityNum == ENTITYNUM_WORLD ) {
		return;
	}
	if ( pm->numtouch == MAXTOUCH ) {
		return;
	}

	// see if it is already added
	for ( i = 0 ; i < pm->numtouch ; i++ ) {
		if ( pm->touchents[ i ] == entityNum ) {
			return;
		}
	}

	// add it
	pm->touchents[pm->numtouch] = entityNum;
// STONELANCE
	VectorCopy(hitOrigin, pm->touchPos[pm->numtouch]);
// END
	pm->numtouch++;
}
#endif //__ADVANCED_VEHICLES__

/*
==============
PM_StartWeaponAnim
==============
*/
static void PM_StartWeaponAnim( int anim ) {
	if ( pm->ps->pm_type >= PM_DEAD )
		return;
	
	if ( pm->pmext->weapAnimTimer > 0 )
		return;

	if(pm->cmd.weapon == WP_NONE)
		return;

	pm->ps->weapAnim = ( ( pm->ps->weapAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;	
}

void PM_ContinueWeaponAnim( int anim ) {
	if(pm->cmd.weapon == WP_NONE)
		return;

	if ( ( pm->ps->weapAnim & ~ANIM_TOGGLEBIT ) == anim ) {
		return;
	}
	if ( pm->pmext->weapAnimTimer > 0 ) {
		return;		// a high priority animation is running
	}
	PM_StartWeaponAnim( anim );
}

/*
==================
PM_ClipVelocity

Slide off of the impacting surface
==================
*/
void PM_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce ) {
	float	backoff;
	float	change;
	int		i;
	
	backoff = DotProduct (in, normal);
	
	if ( backoff < 0 ) {
		backoff *= overbounce;
	} else {
		backoff /= overbounce;
	}

	for ( i=0 ; i<3 ; i++ ) {
		change = normal[i]*backoff;
		out[i] = in[i] - change;
	}
}

/*
==================
PM_TraceAll

finds worst trace of body/legs, for collision.
==================
*/
void	PM_TraceVehicle( trace_t *trace, float *legsOffset, vec3_t start, vec3_t end );

void PM_TraceLegs( trace_t *trace, float *legsOffset, vec3_t start, vec3_t end, trace_t *bodytrace, vec3_t viewangles, void (tracefunc)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask ), int ignoreent, int tracemask )
{
	trace_t steptrace;
	vec3_t ofs, org, point;
	vec3_t flatforward;
	float angle;

#ifdef __VEHICLES__
	if ( pm->ps->eFlags & EF_VEHICLE ) 
	{
		PM_VehicleTraces(pml.frametime, *trace);
		return;
	}
#endif //__VEHICLES__

	// zinx - don't let players block legs
	tracemask &= ~(CONTENTS_BODY | CONTENTS_CORPSE);

	if (legsOffset) {
		*legsOffset = 0;
	}

	angle = DEG2RAD(viewangles[YAW]);
	flatforward[0] = cos(angle);
	flatforward[1] = sin(angle);
	flatforward[2] = 0;

	if(pm->ps->eFlags & EF_PRONE)
		VectorScale(flatforward, -32, ofs);
	else
		VectorScale(flatforward, 32, ofs);

	VectorAdd(start, ofs, org);
	VectorAdd(end, ofs, point);
	tracefunc(trace, org, playerlegsProneMins, playerlegsProneMaxs, point, ignoreent, tracemask);
	if (!bodytrace || trace->fraction < bodytrace->fraction ||
	    trace->allsolid) {
		// legs are clipping sooner than body
		// see if our legs can step up
		ofs[2] += STEPSIZE;

		VectorAdd(start, ofs, org);
		VectorAdd(end, ofs, point);
		tracefunc(&steptrace, org, playerlegsProneMins, playerlegsProneMaxs, point, ignoreent, tracemask);
		if (!steptrace.allsolid && !steptrace.startsolid &&
		    steptrace.fraction > trace->fraction) {
			// the step trace did better -- use it instead
			*trace = steptrace;

			// get legs offset
			if (legsOffset) {
				*legsOffset = ofs[2];

				//int BG_PlayAnimName( playerState_t *ps, animModelInfo_t *animModelInfo, char *animName, animBodyPart_t bodyPart, qboolean setTimer, qboolean isContinue, qboolean force );
				///*animResult =*/ BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_CLIMBUP, qtrue );
				//BG_PlayAnimName( pm->ps, pm->character->animModelInfo, "BOTH_CLIMB", ANIM_BP_BOTH, qtrue, qfalse, qtrue );

				VectorCopy(steptrace.endpos, org);
				VectorCopy(steptrace.endpos, point);
				point[2] -= STEPSIZE;

				tracefunc(&steptrace, org, playerlegsProneMins, playerlegsProneMaxs, point, ignoreent, tracemask);
				if (!steptrace.allsolid) {
					*legsOffset = ofs[2] - (org[2] - steptrace.endpos[2]);
				}
			}
		}
	}
}

void PM_TraceHead(trace_t *trace,
 				  vec3_t start,
 				  vec3_t end,
 				  trace_t *bodytrace,
 				  vec3_t viewangles,
 				  void (tracefunc)(trace_t *results,
 					const vec3_t start,
 					const vec3_t mins,
 					const vec3_t maxs,
 					const vec3_t end,
 					int passEntityNum,
 					int contentMask),
 				  int ignoreent,
 				  int tracemask )
 {
 	vec3_t ofs;
 	vec3_t flatforward;
 	vec3_t point;
 	float angle;
 	// tjw: more than just head, try to make a box for all the
 	//      player model that extends out (weapons and arms too)
 	vec3_t	mins = { -18.f, -18.f, -2.f };
 	vec3_t	maxs = { 18.f, 18.f, 10.f };
 
 	// don't let players block head
 	tracemask &= ~(CONTENTS_BODY | CONTENTS_CORPSE);
 
 	angle = DEG2RAD(viewangles[YAW]);
 	flatforward[0] = cos(angle);
 	flatforward[1] = sin(angle);
 	flatforward[2] = 0;
 
 	if(pm->ps->eFlags & EF_PRONE) {;
 		VectorScale(flatforward, 36, ofs);
 	}
 	else {
 		VectorScale(flatforward, -36, ofs);
 	}
 
 	VectorAdd(end, ofs, point);
 	tracefunc(trace, start, mins, maxs, point, ignoreent, tracemask);
 }
 
 
 /* Traces all player bboxes -- body. legs, and head */
 void	PM_TraceAllParts( trace_t *trace, float *legsOffset, vec3_t start, vec3_t end )
  {
  	pm->trace(trace, start, pm->mins, pm->maxs, end, pm->ps->clientNum, pm->tracemask);
  	
 	// legs and head
 	if((pm->ps->eFlags & EF_PRONE) ||
 		(pm->ps->eFlags & EF_DEAD) ||
 		(pm->ps->eFlags & EF_PLAYDEAD)) {
 
  		trace_t legtrace;
 		trace_t headtrace;
 		qboolean adjust = qfalse;
  
 		PM_TraceLegs( &legtrace, legsOffset, start, end, trace,
 			pm->ps->viewangles, pm->trace, pm->ps->clientNum,
 			pm->tracemask);
  
 		if(legtrace.fraction < trace->fraction ||
  		    legtrace.startsolid ||
  		    legtrace.allsolid) {
 
  			*trace = legtrace;
 			adjust = qtrue;
 		}
 
 		if(PM_TRACEHEAD) {
 			PM_TraceHead( &headtrace, start, end, trace,
 				pm->ps->viewangles, pm->trace, pm->ps->clientNum,
 				pm->tracemask);
 
 			if(headtrace.fraction < trace->fraction ||
 			    headtrace.startsolid ||
 			    headtrace.allsolid) {
 
 				*trace = headtrace;
 				adjust = qtrue;
 			}
 		}
 
 		if(adjust) {
 			VectorSubtract(end, start, trace->endpos);
 			VectorMA(start, trace->fraction, trace->endpos,
 				trace->endpos);
		}
	}
}

#ifdef __VEHICLES__
#ifdef CGAMEDLL
extern void CG_RailTrail2( clientInfo_t *ci, vec3_t start, vec3_t end );
#endif //CGAMEDLL

void	PM_TraceAllVehicleParts( trace_t *trace, float *legsOffset, vec3_t start, vec3_t end )
{
	pm->trace(trace, start, pm->ps->mins, pm->ps->maxs, end, pm->ps->clientNum, pm->tracemask);
}
#endif //__VEHICLES__

void PM_TraceAll( trace_t *trace, vec3_t start, vec3_t end )
{
#ifdef __VEHICLES__
	if ( pm->ps->eFlags & EF_VEHICLE ) 
		PM_TraceAllVehicleParts( trace, NULL, start, end );
	else
		PM_TraceAllParts( trace, NULL, start, end );
#else //!__VEHICLES__
	PM_TraceAllParts( trace, NULL, start, end );
#endif //__VEHICLES__
}

/*
========================
PM_ExertSound

plays random exertion sound when sprint key is press
========================
*/
/*static void PM_ExertSound (void)
{
	int rval;
	static int	oldexerttime = 0;
	static int	oldexertcnt = 0;

	if (pm->cmd.serverTime > oldexerttime + 500)
		oldexerttime = pm->cmd.serverTime;
	else
		return;
	
	rval = rand()%3;

	if (oldexertcnt != rval)
		 oldexertcnt = rval;
	else
		oldexertcnt++;
	
	if (oldexertcnt > 2)
		oldexertcnt = 0;
				
	if (oldexertcnt == 1)
		PM_AddEvent (EV_EXERT2);
	else if (oldexertcnt == 2)
		PM_AddEvent (EV_EXERT3);
	else
		PM_AddEvent (EV_EXERT1);
}*/


/*
==================
PM_Friction

Handles both ground friction and water friction
==================
*/
static void PM_Friction( void ) {
	vec3_t	vec;
	float	*vel;
	float	speed, newspeed, control;
	float	drop;

	vel = pm->ps->velocity;
	
	VectorCopy( vel, vec );
	if ( pml.walking ) {
		vec[2] = 0;	// ignore slope movement
	}

	speed = VectorLength(vec);
	// rain - #179 don't do this for PM_SPECTATOR/PM_NOCLIP, we always want them to stop
	if (speed < 1 && pm->ps->pm_type != PM_SPECTATOR && pm->ps->pm_type != PM_NOCLIP) {
		vel[0] = 0;
		vel[1] = 0;		// allow sinking underwater
		// FIXME: still have z friction underwater?
		return;
	}

	drop = 0;

	// apply end of dodge friction
	if( pm->cmd.serverTime - pm->pmext->dodgeTime < 350 &&
		pm->cmd.serverTime - pm->pmext->dodgeTime > 250 ) {
		drop += speed*20*pml.frametime;
	}

	// apply ground friction
	if ( pm->waterlevel <= 1 ) {
#ifdef __VEHICLES__
#ifdef GAMEDLL
		if ((pm->ps->eFlags & EF_VEHICLE) && (pml.groundTrace.surfaceFlags & SURF_SLICK) && g_entities[pm->ps->clientNum].r.svFlags & SVF_BOT)
		{// No sliding for bots in tanks!
			drop += speed*pm_spectatorfriction*pml.frametime;
		}
		else if ((g_entities[pm->ps->clientNum].r.svFlags & SVF_BOT) && (pml.groundTrace.surfaceFlags & SURF_SLICK))
		{// No sliding for bots at all lol!
			drop += speed*pm_spectatorfriction*pml.frametime;
		}
		else if (pm->isNPC && (pml.groundTrace.surfaceFlags & SURF_SLICK))
		{// No sliding for NPCs at all lol!
			drop += speed*pm_spectatorfriction*pml.frametime;
		}
		else
#endif //GAMEDLL
#endif //__VEHICLES__
		if ( pml.walking && !(pml.groundTrace.surfaceFlags & SURF_SLICK) ) {
			// if getting knocked back, no friction
			if ( ! (pm->ps->pm_flags & PMF_TIME_KNOCKBACK) 
#ifdef __VEHICLES__
				&& !(pm->ps->eFlags & EF_VEHICLE)
#endif //__VEHICLES__
				) 
			{
				control = speed < pm_stopspeed ? pm_stopspeed : speed;
				drop += control*pm_friction*pml.frametime;
			} else if (! (pm->ps->pm_flags & PMF_TIME_KNOCKBACK)) {
				control = speed < 250 ? 250 : speed;
				drop += control*4*pml.frametime; // low friction
			}
		}
	}

	// apply water friction even if just wading
	if ( pm->waterlevel ) {	
		if ( pm->watertype == CONTENTS_SLIME )	//----(SA)	slag
			drop += speed*pm_slagfriction*pm->waterlevel*pml.frametime;
		else
			drop += speed*pm_waterfriction*pm->waterlevel*pml.frametime;
	}

	if ( pm->ps->pm_type == PM_SPECTATOR) {
		drop += speed*pm_spectatorfriction*pml.frametime;
	}

	// apply ladder strafe friction
	if ( pml.ladder ) {
		drop += speed*pm_ladderfriction*pml.frametime;
	}

	// scale the velocity
	newspeed = speed - drop;
	if (newspeed < 0) {
		newspeed = 0;
	}
	newspeed /= speed;

	// rain - if we're barely moving and barely slowing down, we want to
	// help things along--we don't want to end up getting snapped back to
	// our previous speed
	if (pm->ps->pm_type == PM_SPECTATOR || pm->ps->pm_type == PM_NOCLIP) {
		if (drop < 1.0f && speed < 3.0f) {
			newspeed = 0.0;
		}
	}

	// rain - used VectorScale instead of multiplying by hand
	VectorScale(vel, newspeed, vel);
}


/*
==============
PM_Accelerate

Handles user intended acceleration
==============
*/
static void PM_Accelerate( vec3_t wishdir, float wishspeed, float accel ) {
#if 1
	// q2 style
	int			i;
	float		addspeed, accelspeed, currentspeed;

	currentspeed = DotProduct (pm->ps->velocity, wishdir);
	addspeed = wishspeed - currentspeed;
	if (addspeed <= 0) {
		return;
	}
	accelspeed = accel*pml.frametime*wishspeed;
	if (accelspeed > addspeed) {
		accelspeed = addspeed;
	}
	
	// Ridah, variable friction for AI's
	if (pm->ps->groundEntityNum != ENTITYNUM_NONE) {
		accelspeed *= (1.0 / pm->ps->friction);
	}
	if (accelspeed > addspeed) {
		accelspeed = addspeed;
	}

	for (i=0 ; i<3 ; i++) {
		pm->ps->velocity[i] += accelspeed*wishdir[i];	
	}
#else
	// proper way (avoids strafe jump maxspeed bug), but feels bad
	vec3_t		wishVelocity;
	vec3_t		pushDir;
	float		pushLen;
	float		canPush;

	VectorScale( wishdir, wishspeed, wishVelocity );
	VectorSubtract( wishVelocity, pm->ps->velocity, pushDir );
	pushLen = VectorNormalize( pushDir );

	canPush = accel*pml.frametime*wishspeed;
	if (canPush > pushLen) {
		canPush = pushLen;
	}

	VectorMA( pm->ps->velocity, canPush, pushDir, pm->ps->velocity );
#endif
}



// JPW NERVE -- added because I need to check single/multiplayer instances and branch accordingly
#ifdef CGAMEDLL
	extern vmCvar_t			cg_gameType;
	extern vmCvar_t			cg_movespeed;
#endif
#ifdef GAMEDLL
	extern	vmCvar_t		g_gametype;
	extern	vmCvar_t		g_movespeed;
#endif

/*
============
PM_CmdScale

Returns the scale factor to apply to cmd movements
This allows the clients to use axial -127 to 127 values for all directions
without getting a sqrt(2) distortion in speed.
============
*/
static float PM_CmdScale( usercmd_t *cmd ) {
	int		max;
	float	total;
	float	scale;

#ifdef CGAMEDLL
	int gametype = cg_gameType.integer;
	int movespeed = cg_movespeed.integer;
#elif GAMEDLL
	int gametype = g_gametype.integer;
	int movespeed = g_movespeed.integer;
#endif

	max = abs( cmd->forwardmove );
	if ( abs( cmd->rightmove ) > max ) {
		max = abs( cmd->rightmove );
	}
	if ( abs( cmd->upmove ) > max ) {
		max = abs( cmd->upmove );
	}
	if ( !max ) {
		return 0;
	}

	total = sqrt( cmd->forwardmove * cmd->forwardmove
		+ cmd->rightmove * cmd->rightmove + cmd->upmove * cmd->upmove );
	scale = (float)pm->ps->speed * max / ( 127.0 * total );

#if defined (__FRONTLINE__) || defined (__EF__)
		if (pm->cmd.buttons & BUTTON_SPRINT 
		&& pm->pmext->sprintTime >  50 
		&& pm->waterlevel < 2 
#ifdef __VEHICLES__
		&& !(pm->ps->eFlags & EF_VEHICLE)
#endif //__VEHICLES__
		&& !(pm->ps->pm_flags & PMF_LADDER)) // need to check if sprinting will have effect to avoid stamina draining with no sprinting
	{
		pm->ps->eFlags &= ~EF_AIMING;
		pm->pmext->adsTime = -pm->cmd.serverTime;
	//	pm->pmext->sprinting = qtrue;
	}
	else {
	//	pm->pmext->sprinting = qfalse;
	}
#endif //defined (__FRONTLINE__) || defined (__EF__)
	if (pm->cmd.buttons & BUTTON_SPRINT && pm->pmext->sprintTime > 50 
#ifdef __VEHICLES__
		&& !(pm->ps->eFlags & EF_VEHICLE ) 
#endif //__VEHICLES__
		)
	{
		scale *= pm->ps->sprintSpeedScale;
	}
	else
		scale *= pm->ps->runSpeedScale;
	
	if (pm->ps->pm_type == PM_NOCLIP)
		scale *= 3;

// JPW NERVE -- half move speed if heavy weapon is carried
// this is the counterstrike way of doing it -- ie you can switch to a non-heavy weapon and move at
// full speed.  not completely realistic (well, sure, you can run faster with the weapon strapped to your
// back than in carry position) but more fun to play.  If it doesn't play well this way we'll bog down the
// player if the own the weapon at all.
//
#ifdef __VEHICLES__
	if ( !(pm->ps->eFlags & EF_VEHICLE ) ) 
	{
#endif //__VEHICLES__
		if ((pm->ps->weapon == WP_PANZERFAUST) ||
			(pm->ps->weapon == WP_MOBILE_MG42) ||
			(pm->ps->weapon == WP_MOBILE_MG42_SET) ||
#ifdef __EF__ //Masteries, PTRS reduce movement
			(pm->ps->weapon == WP_30CAL_SET) ||
			(pm->ps->weapon == WP_PTRS) ||      
            (pm->ps->weapon == WP_PTRS_SET) ||
#endif //__EF__
			(pm->ps->weapon == WP_MORTAR)) {
			if( pm->skill[SK_HEAVY_WEAPONS] >= 3 ) 
			{
				scale *= 0.75;
			} else {
				scale *= 0.5;
			}
		}
	
		if (pm->ps->weapon == WP_FLAMETHROWER) 
		{ // trying some different balance for the FT
			if( !(pm->skill[SK_HEAVY_WEAPONS] >= 3) || pm->cmd.buttons & BUTTON_ATTACK )
				scale *= 0.7;
		}

		if (gametype == GT_SINGLE_PLAYER || gametype == GT_COOP) 
		{// Adjust the movespeed
			scale *= (((float) movespeed)/(float) 127);
		} // if (gametype == GT_SINGLE_PLAYER)...

#ifdef __WEAPON_AIM__
		if (pm->ps->eFlags & EF_AIMING)
		{// Adjust the movespeed
			scale *= (((float) movespeed)/(float) 127);
		}
#endif //__WEAPON_AIM__

#if defined (__FRONTLINE__) || defined (__EF__)
		if (pm->ps->pm_flags & PMF_LADDER)
		{// Adjust the movespeed
			scale *= (((float) movespeed)/(float) 127);
		}
#endif //defined (__FRONTLINE__) || defined (__EF__)

#ifdef __SHELLSHOCK__
		if ((HasExtFlag(pm->ps->clientNum, EX_SHELLSHOCK)) /*&& (PM_ShellShock & (SHELLSHOCK_ENABLED|SHELLSHOCK_JITTER))*/)
			scale *= 0.7;
#endif //__SHELLSHOCK__

		if (g_realism.integer && pm->ps->stats[STAT_HEALTH] < pm->ps->stats[STAT_MAX_HEALTH] && pm->ps->stats[STAT_HEALTH] > 0)
		{// In realsim scale will be based on how low health is...
			float frac = 1 - ((pm->ps->stats[STAT_MAX_HEALTH] - pm->ps->stats[STAT_HEALTH]) / pm->ps->stats[STAT_MAX_HEALTH]);
			scale *= frac;
		}

		if (pm->ps->pm_type != PM_SPECTATOR)
		{
			qboolean doing_torso_anim = qfalse;

			switch (pm->ps->torsoAnim)
			{// UQ1: Add some slowness based on anim...
			/*case BOTH_CLIMB_DISMOUNT:// Getting off a ladder/etc, slow them down for the length of the anim...
				//scale *= 0.15f;
				scale = 0.0f;
				doing_torso_anim = qtrue;
				break;*/
			case BOTH_PAIN1:
			case BOTH_PAIN2:
			case BOTH_PAIN3:
			case BOTH_PAIN4:
			case BOTH_PAIN5:
			case BOTH_PAIN6:
			case BOTH_PAIN7:
				scale *= 0.0f;//0.15f;
				doing_torso_anim = qtrue;
				break;
			case BOTH_PAIN8: // Dazed...
			case BOTH_SALUTE:
			case BOTH_DEATH1:
				scale = 0.0f;
				doing_torso_anim = qtrue;
				break;
			default:
				break;
			}

			/*if (!doing_torso_anim)
			{
				switch (pm->ps->legsAnim)
				{// UQ1: Add some slowness based on anim...
				case LEGS_LAND: // UQ1: Testing, when landing after a fall, actually move faster for a moment...
					scale = 0.0f;
					break;
				case LEGS_BOOT: // kicking animation
					scale = 0.0f;
					break;
				default:
					break;
				}
			}*/
		}

#ifdef __VEHICLES__
	}
#endif //__VEHICLES__

	return scale;
}

#ifdef __VEHICLES__
#ifdef CGAMEDLL
extern void CG_RailTrail2( clientInfo_t *ci, vec3_t start, vec3_t end );
#endif //CGAMEDLL

qboolean	PM_VehicleAngleTraces ( vec3_t angles )
{
	qboolean movefail = qfalse;

	// Front half!
	if (pm->ps->eFlags & EF_VEHICLE)
	{// Check the center!
		trace_t tr;
		vec3_t  end2, origin2, forward, right, up, use_origin;
#ifdef CGAMEDLL
		vec3_t  end3;
#endif //CGAMEDLL

		VectorCopy(pm->ps->origin, use_origin);
		use_origin[2]+=72;

		VectorCopy(pm->ps->origin, origin2);
		origin2[2]+=72;

		// Start: Calculate forward and right...
		angles[2]=0;
		AngleVectors (angles, forward, NULL, NULL);
		AngleVectors (angles, NULL, right, NULL);
		AngleVectors (angles, NULL, NULL, up);
		forward[2] = 0;
		right[2] = 0;
		up[2] = 0;
		// End: Calculate forward and right...

		{// Check the right hand side!
			VectorCopy(pm->ps->origin, use_origin);
			use_origin[2]+=72;

			VectorCopy(pm->ps->origin, origin2);
			origin2[2]+=72;

			// Which angles?
			VectorMA( use_origin, 192, forward, use_origin );

			// calculate position we are trying to move to
			VectorMA( use_origin, 64, pm->ps->velocity, end2 );

			//VectorMA( use_origin, 64, right, use_origin );
			VectorMA( origin2, 64, right, origin2 );
			VectorMA( end2, 64, right, end2 );

			//PM_TraceAll( &tr, origin2, end2 );
			pm->trace(&tr, origin2, NULL, NULL, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
			CG_RailTrail2( NULL, origin2, end2 );
			VectorCopy(end2, end3);
#endif //CGAMEDLL

			if (tr.fraction < 1)
			{
				return qfalse;
			}
			//else
			{// Check the left hand side!
				VectorCopy(pm->ps->origin, use_origin);
				use_origin[2]+=72;

				VectorCopy(pm->ps->origin, origin2);
				origin2[2]+=72;

				// Which angles?
				VectorMA( use_origin, 192, forward, use_origin );

				// calculate position we are trying to move to
				VectorMA( use_origin, 64, pm->ps->velocity, end2 );

				//VectorMA( use_origin, -64, right, use_origin );
				VectorMA( origin2, -64, right, origin2 );
				VectorMA( end2, -64, right, end2 );

				//PM_TraceAll( &tr, origin2, end2 );
				pm->trace(&tr, origin2, NULL, NULL, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
				CG_RailTrail2( NULL, origin2, end2 );
				CG_RailTrail2( NULL, end2, end3 );
#endif //CGAMEDLL

				if (tr.fraction < 1)
				{
					//return qfalse;
					movefail = qtrue;
				}
			}
		}
	}

	if ((pm->ps->eFlags & EF_VEHICLE) && movefail)
	{// Check the center!
		trace_t tr;
		vec3_t  end2, origin2, forward, right, up, use_origin;
#ifdef CGAMEDLL
		vec3_t  end3;
#endif //CGAMEDLL

		movefail = qfalse;

		VectorCopy(pm->ps->origin, use_origin);
		use_origin[2]+=72;

		VectorCopy(pm->ps->origin, origin2);
		origin2[2]+=72;

		// Start: Calculate forward and right...
		angles[2]=0;
		AngleVectors (angles, forward, NULL, NULL);
		AngleVectors (angles, NULL, right, NULL);
		AngleVectors (angles, NULL, NULL, up);
		forward[2] = 0;
		right[2] = 0;
		up[2] = 0;
		// End: Calculate forward and right...
			
		//else
		{// Check the right hand side!
			VectorCopy(pm->ps->origin, use_origin);
			use_origin[2]+=72;

			VectorCopy(pm->ps->origin, origin2);
			origin2[2]+=72;

			// Which angles?
			VectorMA( use_origin, 192, forward, use_origin );

			// calculate position we are trying to move to
			VectorMA( use_origin, 64, pm->ps->velocity, end2 );

			//VectorMA( use_origin, 64, right, use_origin );
			VectorMA( origin2, 64, right, origin2 );
			VectorMA( end2, 64, right, end2 );

			//PM_TraceAll( &tr, origin2, end2 );
			pm->trace(&tr, origin2, NULL, NULL, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
			CG_RailTrail2( NULL, origin2, end2 );
			VectorCopy(end2, end3);
#endif //CGAMEDLL

			if (tr.fraction < 1)
			{
				return qfalse;
			}
			//else
			{// Check the left hand side!
				VectorCopy(pm->ps->origin, use_origin);
				use_origin[2]+=72;

				VectorCopy(pm->ps->origin, origin2);
				origin2[2]+=72;

				// Which angles?
				VectorMA( use_origin, 192, forward, use_origin );

				// calculate position we are trying to move to
				VectorMA( use_origin, 64, pm->ps->velocity, end2 );

				//VectorMA( use_origin, -64, right, use_origin );
				VectorMA( origin2, -64, right, origin2 );
				VectorMA( end2, -64, right, end2 );

				//PM_TraceAll( &tr, origin2, end2 );
				pm->trace(&tr, origin2, NULL, NULL, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
				CG_RailTrail2( NULL, origin2, end2 );
				CG_RailTrail2( NULL, end2, end3 );
#endif //CGAMEDLL

				if (tr.fraction < 1)
				{
					//return qfalse;
					movefail = qtrue;
				}
			}
		}
	}

	if (movefail)
	{
		pm->pml_tank_movetime = 1;
		return qfalse;
	}

	// Back half!
	if (pm->ps->eFlags & EF_VEHICLE)
	{// Check the center!
		trace_t tr;
		vec3_t  end2, origin2, forward, right, up, use_origin;
#ifdef CGAMEDLL
		vec3_t  end3;
#endif //CGAMEDLL

		VectorCopy(pm->ps->origin, use_origin);
		use_origin[2]+=72;

		VectorCopy(pm->ps->origin, origin2);
		origin2[2]+=72;

		// Start: Calculate forward and right...
		angles[2]=0;
		AngleVectors (angles, forward, NULL, NULL);
		AngleVectors (angles, NULL, right, NULL);
		AngleVectors (angles, NULL, NULL, up);
		forward[2] = 0;
		right[2] = 0;
		up[2] = 0;
		// End: Calculate forward and right...

		{// Check the right hand side!
			VectorCopy(pm->ps->origin, use_origin);
			use_origin[2]+=72;

			VectorCopy(pm->ps->origin, origin2);
			origin2[2]+=72;

			// Which angles?
			VectorMA( use_origin, -192, forward, use_origin );

			// calculate position we are trying to move to
			VectorMA( use_origin, 64, pm->ps->velocity, end2 );

			//VectorMA( use_origin, 64, right, use_origin );
			VectorMA( origin2, 64, right, origin2 );
			VectorMA( end2, 64, right, end2 );

			//PM_TraceAll( &tr, origin2, end2 );
			pm->trace(&tr, origin2, NULL, NULL, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
			CG_RailTrail2( NULL, origin2, end2 );
			VectorCopy(end2, end3);
#endif //CGAMEDLL

			if (tr.fraction < 1)
			{
				return qfalse;
			}
			//else
			{// Check the left hand side!
				VectorCopy(pm->ps->origin, use_origin);
				use_origin[2]+=72;

				VectorCopy(pm->ps->origin, origin2);
				origin2[2]+=72;

				// Which angles?
				VectorMA( use_origin, -192, forward, use_origin );

				// calculate position we are trying to move to
				VectorMA( use_origin, 64, pm->ps->velocity, end2 );

				//VectorMA( use_origin, -64, right, use_origin );
				VectorMA( origin2, -64, right, origin2 );
				VectorMA( end2, -64, right, end2 );

				//PM_TraceAll( &tr, origin2, end2 );
				pm->trace(&tr, origin2, NULL, NULL, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
				CG_RailTrail2( NULL, origin2, end2 );
				CG_RailTrail2( NULL, end2, end3 );
#endif //CGAMEDLL

				if (tr.fraction < 1)
				{
					//return qfalse;
					movefail = qtrue;
				}
			}
		}
	}

	if ((pm->ps->eFlags & EF_VEHICLE) && movefail)
	{// Check the center!
		trace_t tr;
		vec3_t  end2, origin2, forward, right, up, use_origin;
#ifdef CGAMEDLL
		vec3_t  end3;
#endif //CGAMEDLL

		movefail = qfalse;

		VectorCopy(pm->ps->origin, use_origin);
		use_origin[2]+=72;

		VectorCopy(pm->ps->origin, origin2);
		origin2[2]+=72;

		// Start: Calculate forward and right...
		angles[2]=0;
		AngleVectors (angles, forward, NULL, NULL);
		AngleVectors (angles, NULL, right, NULL);
		AngleVectors (angles, NULL, NULL, up);
		forward[2] = 0;
		right[2] = 0;
		up[2] = 0;
		// End: Calculate forward and right...
			
		//else
		{// Check the right hand side!
			VectorCopy(pm->ps->origin, use_origin);
			use_origin[2]+=72;

			VectorCopy(pm->ps->origin, origin2);
			origin2[2]+=72;

			// Which angles?
			VectorMA( use_origin, -192, forward, use_origin );

			// calculate position we are trying to move to
			VectorMA( use_origin, 64, pm->ps->velocity, end2 );

			//VectorMA( use_origin, 64, right, use_origin );
			VectorMA( origin2, 64, right, origin2 );
			VectorMA( end2, 64, right, end2 );

			//PM_TraceAll( &tr, origin2, end2 );
			pm->trace(&tr, origin2, NULL, NULL, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
			CG_RailTrail2( NULL, origin2, end2 );
			VectorCopy(end2, end3);
#endif //CGAMEDLL

			if (tr.fraction < 1)
			{
				return qfalse;
			}
			//else
			{// Check the left hand side!
				VectorCopy(pm->ps->origin, use_origin);
				use_origin[2]+=72;

				VectorCopy(pm->ps->origin, origin2);
				origin2[2]+=72;

				// Which angles?
				VectorMA( use_origin, -192, forward, use_origin );

				// calculate position we are trying to move to
				VectorMA( use_origin, 64, pm->ps->velocity, end2 );

				//VectorMA( use_origin, -64, right, use_origin );
				VectorMA( origin2, -64, right, origin2 );
				VectorMA( end2, -64, right, end2 );

				//PM_TraceAll( &tr, origin2, end2 );
				pm->trace(&tr, origin2, NULL, NULL, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
				CG_RailTrail2( NULL, origin2, end2 );
				CG_RailTrail2( NULL, end2, end3 );
#endif //CGAMEDLL

				if (tr.fraction < 1)
				{
					//return qfalse;
					movefail = qtrue;
				}
			}
		}
	}

	if (movefail)
	{
		pm->pml_tank_movetime = 1;
		return qfalse;
	}

	pm->pml_tank_movetime = 0;
	return qtrue;
}
#endif //__VEHICLES__

/*
================
PM_SetMovementDir

Determine the rotation of the legs reletive
to the facing dir
================
*/
static void PM_SetMovementDir( void ) {
// Ridah, changed this for more realistic angles (at the cost of more network traffic?)
#if 1
	float	speed;
	vec3_t	moved;
	int moveyaw;

	VectorSubtract (pm->ps->origin, pml.previous_origin, moved);

	if (	(pm->cmd.forwardmove || pm->cmd.rightmove)
		&&	(pm->ps->groundEntityNum != ENTITYNUM_NONE)
		&&	(speed = VectorLength( moved ))
		&&	(speed > pml.frametime*5))		// if moving slower than 20 units per second, just face head angles
	{
		vec3_t	dir;

		VectorNormalize2( moved, dir );
		vectoangles( dir, dir );

		moveyaw = (int)AngleDelta( dir[YAW], pm->ps->viewangles[YAW] );

		if (pm->cmd.forwardmove < 0)
			moveyaw = (int)AngleNormalize180(moveyaw + 180);

		if (abs(moveyaw) > 75)
		{
			if (moveyaw > 0)
			{
				moveyaw = 75;
			}
			else
			{
				moveyaw = -75;
			}
		}

/*#ifdef __VEHICLES__
		if (pm->ps->eFlags & EF_VEHICLE)
		{// UQ1: We need to make tanks not swing into walls!
			vec3_t angles;

			VectorCopy(pm->ps->viewangles, angles);
			angles[YAW] = moveyaw;

			if (!PM_VehicleAngleTraces ( angles ))
				pm->ps->movementDir = 0;
			else
				pm->ps->movementDir = (signed char)moveyaw;
		}
		else
#endif //__VEHICLES__*/
			pm->ps->movementDir = (signed char)moveyaw;
	}
	else
	{
		pm->ps->movementDir = 0;
	}
#else
	if ( pm->cmd.forwardmove || pm->cmd.rightmove ) {
		if ( pm->cmd.rightmove == 0 && pm->cmd.forwardmove > 0 ) {
			pm->ps->movementDir = 0;
		} else if ( pm->cmd.rightmove < 0 && pm->cmd.forwardmove > 0 ) {
			pm->ps->movementDir = 1;
		} else if ( pm->cmd.rightmove < 0 && pm->cmd.forwardmove == 0 ) {
			pm->ps->movementDir = 2;
		} else if ( pm->cmd.rightmove < 0 && pm->cmd.forwardmove < 0 ) {
			pm->ps->movementDir = 3;
		} else if ( pm->cmd.rightmove == 0 && pm->cmd.forwardmove < 0 ) {
			pm->ps->movementDir = 4;
		} else if ( pm->cmd.rightmove > 0 && pm->cmd.forwardmove < 0 ) {
			pm->ps->movementDir = 5;
		} else if ( pm->cmd.rightmove > 0 && pm->cmd.forwardmove == 0 ) {
			pm->ps->movementDir = 6;
		} else if ( pm->cmd.rightmove > 0 && pm->cmd.forwardmove > 0 ) {
			pm->ps->movementDir = 7;
		}
	} else {
		// if they aren't actively going directly sideways,
		// change the animation to the diagonal so they
		// don't stop too crooked
		if ( pm->ps->movementDir == 2 ) {
			pm->ps->movementDir = 1;
		} else if ( pm->ps->movementDir == 6 ) {
			pm->ps->movementDir = 7;
		} 
	}
#endif
}


/*
=============
PM_CheckJump
=============
*/
static qboolean PM_CheckJump( void ) {
	// no jumpin when prone
	if( pm->ps->eFlags & EF_PRONE ) {
		return qfalse;
	}

	// JPW NERVE -- jumping in multiplayer uses and requires sprint juice (to prevent turbo skating, sprint + jumps)
	// don't allow jump accel
	//if (pm->cmd.serverTime - pm->pmext->jumpTime < 850)
	if (pm->cmd.serverTime - pm->ps->jumpTime < 850 )	{// Arnout: NOTE : TEMP DEBUG
		return qfalse;
	}

	// don't allow if player tired 
	// Perro: If g_misc 16 is set, require stamina to jump
	if (PM_JUMP_STAMINA){
		if (pm->pmext->sprintTime < 750) // JPW pulled this per id request; made airborne jumpers wildly inaccurate with gunfire to compensate
			return qfalse;
	}
	// jpw

	if ( pm->ps->pm_flags & PMF_RESPAWNED ) {
		return qfalse;		// don't allow jump until all buttons are up
	}

	if ( pm->cmd.upmove < 10 ) {
		// not holding jump
		return qfalse;
	}

	// must wait for jump to be released
	if ( pm->ps->pm_flags & PMF_JUMP_HELD ) {
		// clear upmove so cmdscale doesn't lower running speed
		pm->cmd.upmove = 0;
		return qfalse;
	}

	pml.groundPlane = qfalse;		// jumping away
	pml.walking = qfalse;
	pm->ps->pm_flags |= PMF_JUMP_HELD;
#if defined (__FRONTLINE__) || defined (__EF__)
	pm->ps->eFlags &= ~EF_AIMING; // unselect ironsights
	pm->pmext->adsTime = -pm->cmd.serverTime;
#endif //defined (__FRONTLINE__) || defined (__EF__)

	pm->ps->groundEntityNum = ENTITYNUM_NONE;
	pm->ps->velocity[2] = JUMP_VELOCITY;
	PM_AddEvent( EV_JUMP );
	
	if ( pm->cmd.forwardmove >= 0 ) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMP, qfalse, qtrue );
		pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
	} else {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMPBK, qfalse, qtrue );
		pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
	}

	return qtrue;
}
/*
=============
PM_CheckDoubleJump
=============
*/
static qboolean PM_CheckDoubleJump( void ) {

	if ( !PM_DOUBLE_JUMP ) {
		return qfalse;
	}

	// no jumpin when prone
	if( pm->ps->eFlags & EF_PRONE ) {
		return qfalse;
	}

	if (PM_JUMP_STAMINA){
		if (pm->pmext->sprintTime < 750) // JPW pulled this per id request; made airborne jumpers wildly inaccurate with gunfire to compensate
			return qfalse;
	}

	// josh: Don't allow if already double jumping
	if (pm->ps->pm_flags & PMF_DOUBLEJUMPING)	{
			return qfalse;
	}
	
	// jpw

	// Only allow double jump when going up
	if (pm->ps->velocity[2] <= 0) {
			return qfalse;
	}

	if ( pm->ps->pm_flags & PMF_RESPAWNED ) {
		return qfalse;		// don't allow jump until all buttons are up
	}

	if ( pm->cmd.upmove < 10 ) {
		// not holding jump
		return qfalse;
	}

	// must wait for jump to be released
	if ( pm->ps->pm_flags & PMF_JUMP_HELD ) {
		// clear upmove so cmdscale doesn't lower running speed
		pm->cmd.upmove = 0;
		return qfalse;
	}

	pml.groundPlane = qfalse;		// jumping away
	pml.walking = qfalse;
	pm->ps->pm_flags |= PMF_JUMP_HELD;

	pm->ps->groundEntityNum = ENTITYNUM_NONE;
	pm->ps->velocity[2] = PM_DOUBLE_JUMP_HEIGHT*JUMP_VELOCITY;
	PM_AddEvent( EV_JUMP );
	
	if ( pm->cmd.forwardmove >= 0 ) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMP, qfalse, qtrue );
		pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
	} else {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMPBK, qfalse, qtrue );
		pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
	}

	return qtrue;
}

/*
=============
PM_CheckWaterJump
=============
*/
static qboolean	PM_CheckWaterJump( void ) {
	vec3_t	spot;
	int		cont;
	vec3_t	flatforward;

	if (pm->ps->pm_time) {
		return qfalse;
	}

	// check for water jump
	if ( pm->waterlevel != 2 ) {
		return qfalse;
	}

	flatforward[0] = pml.forward[0];
	flatforward[1] = pml.forward[1];
	flatforward[2] = 0;
	VectorNormalize (flatforward);

	VectorMA (pm->ps->origin, 30, flatforward, spot);
	spot[2] += 4;
	cont = pm->pointcontents (spot, pm->ps->clientNum );
	if ( !(cont & CONTENTS_SOLID) ) {
		return qfalse;
	}

	spot[2] += 16;
	cont = pm->pointcontents (spot, pm->ps->clientNum );
	if ( cont ) {
		return qfalse;
	}

	// jump out of water
	VectorScale (pml.forward, 200, pm->ps->velocity);
	pm->ps->velocity[2] = 350;

	pm->ps->pm_flags |= PMF_TIME_WATERJUMP;
	pm->ps->pm_time = 2000;

	return qtrue;
}


/*
 * PM_CheckPlayDead
 * see if this player can lay down and look dead
*/
static qboolean PM_CheckPlayDead (void)
{
	trace_t trace;

	if(pm->ps->pm_type != PM_PLAYDEAD) return qfalse;

	// PM_PLAYDEAD is a one time only pm_type
	pm->ps->pm_type = PM_NORMAL;

	if(!(pm->ps->eFlags & EF_PLAYDEAD)) {
		if(pm->ps->pm_flags & PMF_LADDER) {
			return qfalse;
		}

		if(pm->ps->persistant[PERS_HWEAPON_USE] || 
			pm->ps->eFlags & EF_MOUNTEDTANK) {

			return qfalse;
		}

		if(pm->waterlevel > 1) {
			return qfalse;
		}

		pm->mins[0] = pm->ps->mins[0];
		pm->mins[1] = pm->ps->mins[1];

		pm->maxs[0] = pm->ps->maxs[0];
		pm->maxs[1] = pm->ps->maxs[1];

		pm->mins[2] = pm->ps->mins[2];
		pm->maxs[2] = pm->ps->crouchMaxZ;

		pm->ps->eFlags |= EF_PLAYDEAD;
		PM_TraceAll(&trace, pm->ps->origin, pm->ps->origin);
		pm->ps->eFlags &= ~EF_PLAYDEAD;

		if(!trace.startsolid && !trace.allsolid) {
			pm->ps->eFlags |= EF_PLAYDEAD;
			pm->ps->eFlags |= EF_DEAD;
			pm->ps->pm_type = PM_DEAD;

			// client uses stats[STAT_HEALTH] as the indicator for 
			// revive sprites and player position 
			pm->ps->stats[STAT_HEALTH] = 0;

			// make the hitbox like a dead guy
			pm->maxs[2] = pm->ps->maxs[2] = pm->ps->crouchMaxZ;
			return qtrue;
		}
	}
	else {
		// see if we have the space to stop playing dead
		pm->mins[0] = pm->ps->mins[0];
		pm->mins[1] = pm->ps->mins[1];

		pm->maxs[0] = pm->ps->maxs[0];
		pm->maxs[1] = pm->ps->maxs[1];

		pm->mins[2] = pm->ps->mins[2];
		pm->maxs[2] = pm->ps->crouchMaxZ;

		pm->ps->eFlags &= ~EF_PLAYDEAD;
		PM_TraceAll(&trace, pm->ps->origin, pm->ps->origin);
		pm->ps->eFlags |= EF_PLAYDEAD;

		if(trace.allsolid) {
			// this would have been turned up to ent->health
			// before running a PM_PLAYDEAD.  Set it back down 
			// if we can't stand up.
			pm->ps->stats[STAT_HEALTH] = 0;
			return qfalse;
		}

		// crouch for a bit
		pm->ps->pm_flags |= PMF_DUCKED;

		// turn the hitbox back up	
		pm->maxs[2] = pm->ps->maxs[2] = pm->ps->standViewHeight;

		// stop playdead
		pm->ps->eFlags &= ~EF_PLAYDEAD;
		pm->ps->eFlags &= ~EF_DEAD;

		// this is need to force the end of the fallen animation
		// because of the timer hack in bg_animation.c
		pm->ps->torsoTimer = 0;
		BG_AnimScriptEvent(pm->ps,
			pm->character->animModelInfo,
			ANIM_ET_JUMPBK,
			qfalse,
			qtrue);

		// don't jump for a bit
		pm->pmext->jumpTime = pm->cmd.serverTime - 650;
		pm->ps->jumpTime = pm->cmd.serverTime - 650;
		
		return qtrue;
	}
	return qfalse;
}

/*
==============
PM_CheckProne

Sets mins, maxs, and pm->ps->viewheight
==============
*/
static qboolean PM_CheckProne (void)
{
	//Com_Printf( "%i: PM_CheckProne (%i)\n", pm->cmd.serverTime, pm->pmext->proneGroundTime );

	if( !(pm->ps->eFlags & EF_PRONE) ) {
		// needs to be on the ground
//		if( !pml.walking ) {
//			return qfalse;
//		}

		// can't go prone on ladders
		if( pm->ps->pm_flags & PMF_LADDER ) {
			return qfalse;
		}

		// no prone when using mg42's
		if( pm->ps->persistant[PERS_HWEAPON_USE] || pm->ps->eFlags & EF_MOUNTEDTANK ) {
			return qfalse;
		}

		if( pm->ps->weaponDelay && pm->ps->weapon == WP_PANZERFAUST ) {
			return qfalse;
		}

		if( pm->ps->weapon == WP_MORTAR_SET ) {
			return qfalse;
		}

		// can't go prone while swimming
		if( pm->waterlevel > 1 ) {
			return qfalse;
		}

		// can't go prone when fiddling with mg42
		//if( pm->ps->weaponstate == WEAPON_FROMPRONE ) {
		//	return qfalse;
		//}

		if(((pm->ps->pm_flags & PMF_DUCKED && pm->cmd.doubleTap == DT_FORWARD) || (pm->cmd.wbuttons & WBUTTON_PRONE)) && 
			pm->cmd.serverTime - -pm->pmext->proneTime > 750) {

			trace_t trace;
			pm->mins[0] = pm->ps->mins[0];
			pm->mins[1] = pm->ps->mins[1];

			pm->maxs[0] = pm->ps->maxs[0];
			pm->maxs[1] = pm->ps->maxs[1];

			pm->mins[2] = pm->ps->mins[2];
			pm->maxs[2] = pm->ps->crouchMaxZ;

			pm->ps->eFlags |= EF_PRONE;
			PM_TraceAll( &trace, pm->ps->origin, pm->ps->origin );
			pm->ps->eFlags &= ~EF_PRONE;

			if(trace.fraction == 1.0f) {
				// go prone
				pm->ps->pm_flags |= PMF_DUCKED;	// crouched as well
				pm->ps->eFlags |= EF_PRONE;
#if defined (__FRONTLINE__) || defined (__EF__)
				pm->ps->eFlags &= ~EF_AIMING;	// stop ironsights
				pm->pmext->adsTime = -pm->cmd.serverTime;
#endif //defined (__FRONTLINE__) || defined (__EF__)
				pm->pmext->proneTime = pm->cmd.serverTime;	// timestamp 'go prone'
				pm->pmext->proneGroundTime = pm->cmd.serverTime;
			}
		}
	}

	if( pm->ps->eFlags & EF_PRONE ) {
		if( pm->waterlevel > 1 ||
			pm->ps->pm_type == PM_DEAD ||
			pm->ps->eFlags & EF_MOUNTEDTANK ||
// zinx - what was the reason for this, anyway? removing fixes bug 424
//			pm->cmd.serverTime - pm->pmext->proneGroundTime > 450 ||
			((pm->cmd.doubleTap == DT_BACK || 
			  pm->cmd.upmove > 10 || 
			  pm->cmd.wbuttons & WBUTTON_PRONE) 
			 && pm->cmd.serverTime - pm->pmext->proneTime > 750)) {

			trace_t trace;

			// see if we have the space to stop prone
			pm->mins[0] = pm->ps->mins[0];
			pm->mins[1] = pm->ps->mins[1];

			pm->maxs[0] = pm->ps->maxs[0];
			pm->maxs[1] = pm->ps->maxs[1];

			pm->mins[2] = pm->ps->mins[2];

			pm->ps->eFlags &= ~EF_PRONE;
			PM_TraceAll( &trace, pm->ps->origin, pm->ps->origin );
			pm->ps->eFlags |= EF_PRONE;
			
			if(trace.fraction == 1.0f) {
				// crouch for a bit
				pm->ps->pm_flags |= PMF_DUCKED;

				// stop prone
				pm->ps->eFlags &= ~EF_PRONE;
				pm->ps->eFlags &= ~EF_PRONE_MOVING;
#ifdef __FRONTLINE
				pm->ps->eFlags &= ~EF_AIMING;	// stop ironsights
				pm->pmext->adsTime = -pm->cmd.serverTime;
#endif
				pm->pmext->proneTime = -pm->cmd.serverTime;	// timestamp 'stop prone'
				// tjw: don't let them keep scope out when
				//      standing from prone or they will
				//      look right through a wall
				switch(pm->ps->weapon) {
				case WP_FG42SCOPE:
					PM_BeginWeaponChange(
						WP_FG42SCOPE,
						WP_FG42,
						qfalse);
					break;
				case WP_GARAND_SCOPE:
					PM_BeginWeaponChange(
						WP_GARAND_SCOPE,
						WP_GARAND,
						qfalse);
					break;
				case WP_K43_SCOPE:
					PM_BeginWeaponChange(
						WP_K43_SCOPE,
						WP_K43,
						qfalse);
					break;
#ifdef __EF__
				case WP_30CAL_SET:
					PM_BeginWeaponChange(
						WP_30CAL_SET,
						WP_30CAL,
						qfalse);
					break;

                case WP_PTRS_SET://Masteries, from prone to stand up
					PM_BeginWeaponChange(
						WP_PTRS_SET,
						WP_PTRS,
						qfalse);
					break;
#endif //__EF__
				case WP_MOBILE_MG42_SET:
					PM_BeginWeaponChange(
						WP_MOBILE_MG42_SET,
						WP_MOBILE_MG42,
						qfalse);
					break;
				}

				// don't jump for a bit
				pm->pmext->jumpTime = pm->cmd.serverTime - 650;
				pm->ps->jumpTime = pm->cmd.serverTime - 650;
			}
		}
	}

	if( pm->ps->eFlags & EF_PRONE ) {
		//float frac;
		
		// See if we are moving
		float spd = VectorLength( pm->ps->velocity );
		qboolean userinput = abs(pm->cmd.forwardmove) + abs(pm->cmd.rightmove) > 10 ? qtrue : qfalse;

		if( userinput && spd > 40.f && !(pm->ps->eFlags & EF_PRONE_MOVING) ) {
			pm->ps->eFlags |= EF_PRONE_MOVING;

			switch( pm->ps->weapon ) {
				case WP_FG42SCOPE: PM_BeginWeaponChange( WP_FG42SCOPE, WP_FG42, qfalse ); break;
				case WP_GARAND_SCOPE: PM_BeginWeaponChange( WP_GARAND_SCOPE, WP_GARAND, qfalse ); break;
				case WP_K43_SCOPE: PM_BeginWeaponChange( WP_K43_SCOPE, WP_K43, qfalse ); break;
			}
		} else if( !userinput && spd < 20.0f && (pm->ps->eFlags & EF_PRONE_MOVING) ) {
			pm->ps->eFlags &= ~EF_PRONE_MOVING;
		}

		pm->mins[0] = pm->ps->mins[0];
		pm->mins[1] = pm->ps->mins[1];

		pm->maxs[0] = pm->ps->maxs[0];
		pm->maxs[1] = pm->ps->maxs[1];

		pm->mins[2] = pm->ps->mins[2];

		//frac = (pm->cmd.serverTime - pm->pmext->proneTime) / 500.f;
		//if( frac > 1.f )
		//	frac = 1.f;

 
		//pm->maxs[2] = pm->ps->maxs[2] - pm->ps->standViewHeight - PRONE_VIEWHEIGHT;
		// tjw: it appears that 12 is the magic number 
		//      for the minimum maxs[2] that prevents
		//      player from getting stuck into the world.
		pm->maxs[2] = 12;
		pm->ps->viewheight = PRONE_VIEWHEIGHT;

		return( qtrue );
	}

	return( qfalse );
}

/*
=============
PM_CheckDodge
=============
*/
static qboolean PM_CheckDodge( void ) {
	// JPW NERVE -- jumping in multiplayer uses and requires sprint juice (to prevent turbo skating, sprint + jumps)
		// don't allow jump accel
 		//if (pm->cmd.serverTime - pm->pmext->jumpTime < 850)
		//	return qfalse;

		// don't allow if player tired 
//		if (pm->pmext->sprintTime < 2500) // JPW pulled this per id request; made airborne jumpers wildly inaccurate with gunfire to compensate
//			return qfalse;
	// jpw

	// Disabled for now
	return qfalse;

/*	if ( pm->pmext->sprintTime < 3000 )
		return qfalse;

	if( pm->cmd.serverTime - pm->pmext->dodgeTime < 350 )
		return qtrue; 	// Already dodging

	if ( pm->ps->pm_flags & PMF_RESPAWNED || pm->ps->pm_flags & PMF_DUCKED || pm->ps->eFlags & EF_PRONE ) {
		return qfalse;		// don't allow dodge until all buttons are up, player is prone or crouching
	}

	if ( pm->cmd.doubleTap != DT_MOVELEFT && pm->cmd.doubleTap != DT_MOVERIGHT ) {
		// no dodge issued
		return qfalse;
	}

	// must wait for jump to be released
	//if ( pm->ps->pm_flags & PMF_JUMP_HELD ) {
	//	// clear upmove so cmdscale doesn't lower running speed
	//	pm->cmd.upmove = 0;
	//	return qfalse;
	//}

	pml.groundPlane = qfalse;		// jumping away
	pml.walking = qfalse;
	//pm->ps->pm_flags |= PMF_JUMP_HELD;

	pm->ps->groundEntityNum = ENTITYNUM_NONE;
	pm->ps->velocity[2] = 130;
	PM_AddEvent( EV_JUMP );
	
	//if ( pm->cmd.forwardmove >= 0 ) {
	//	BG_AnimScriptEvent( pm->ps, ANIM_ET_JUMP, qfalse, qtrue );
	//	pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
	//} else {
	//	BG_AnimScriptEvent( pm->ps, ANIM_ET_JUMPBK, qfalse, qtrue );
	//	pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
	//}

	pm->pmext->dtmove = pm->cmd.doubleTap;
	pm->pmext->dodgeTime = pm->cmd.serverTime;
	pm->pmext->jumpTime = pm->cmd.serverTime - 200;	// Arnout: prevent bunnyhopping
	pm->ps->jumpTime = pm->cmd.serverTime - 200;	// Arnout: prevent bunnyhopping

	pm->pmext->sprintTime -= 3000;

	return qtrue;*/
}

//============================================================================

/*
===================
PM_WaterJumpMove

Flying out of the water
===================
*/
static void PM_WaterJumpMove( void ) {
	// waterjump has no control, but falls

	PM_StepSlideMove( qtrue );

	pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
	if (pm->ps->velocity[2] < 0) {
		// cancel as soon as we are falling down again
		pm->ps->pm_flags &= ~PMF_ALL_TIMES;
		pm->ps->pm_time = 0;
	}
}

/*
===================
PM_WaterMove

===================
*/
static void PM_WaterMove( void ) {
	int		i;
	vec3_t	wishvel;
	float	wishspeed;
	vec3_t	wishdir;
	float	scale;
	float	vel;

	if ( PM_CheckWaterJump() ) {
		PM_WaterJumpMove();
		return;
	}
#if 0
	// jump = head for surface
	if ( pm->cmd.upmove >= 10 ) {
		if (pm->ps->velocity[2] > -300) {
			if ( pm->watertype == CONTENTS_WATER ) {
				pm->ps->velocity[2] = 100;
			} else if (pm->watertype == CONTENTS_SLIME) {
				pm->ps->velocity[2] = 80;
			} else {
				pm->ps->velocity[2] = 50;
			}
		}
	}
#endif
	PM_Friction ();

	scale = PM_CmdScale( &pm->cmd );
	//
	// user intentions
	//
	if ( !scale ) {
		wishvel[0] = 0;
		wishvel[1] = 0;
		wishvel[2] = -60;		// sink towards bottom
//		wishvel[2] = -10;	//----(SA)	mod for DM
	} else {
		for (i=0 ; i<3 ; i++)
			wishvel[i] = scale * pml.forward[i]*pm->cmd.forwardmove + scale * pml.right[i]*pm->cmd.rightmove;

		wishvel[2] += scale * pm->cmd.upmove;
	}

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	if ( pm->watertype == CONTENTS_SLIME ) {	//----(SA)	slag
		if ( wishspeed > pm->ps->speed * pm_slagSwimScale ) {
			wishspeed = pm->ps->speed * pm_slagSwimScale;
		}

		PM_Accelerate (wishdir, wishspeed, pm_slagaccelerate);
	}
	else {
		if ( wishspeed > pm->ps->speed * pm_waterSwimScale ) {
			wishspeed = pm->ps->speed * pm_waterSwimScale;
		}

		PM_Accelerate (wishdir, wishspeed, pm_wateraccelerate);
	}


	// make sure we can go up slopes easily under water
	if ( pml.groundPlane && DotProduct( pm->ps->velocity, pml.groundTrace.plane.normal ) < 0 ) {
		vel = VectorLength(pm->ps->velocity);
		// slide along the ground plane
		PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal, 
			pm->ps->velocity, OVERCLIP );

		VectorNormalize(pm->ps->velocity);
		VectorScale(pm->ps->velocity, vel, pm->ps->velocity);
	}

	PM_SlideMove( qfalse );
}

// TTimo gcc: defined but not used
#if 0
/*
===================
PM_InvulnerabilityMove

Only with the invulnerability powerup
===================
*/
static void PM_InvulnerabilityMove( void ) {
	pm->cmd.forwardmove = 0;
	pm->cmd.rightmove = 0;
	pm->cmd.upmove = 0;
	VectorClear(pm->ps->velocity);
}
#endif

/*
===================
PM_FlyMove

Only with the flight powerup
===================
*/
static void PM_FlyMove( void ) {
	int		i;
	vec3_t	wishvel;
	float	wishspeed;
	vec3_t	wishdir;
	float	scale;

	// normal slowdown
	PM_Friction ();

	scale = PM_CmdScale( &pm->cmd );

	//
	// user intentions
	//
	if ( !scale ) {
		wishvel[0] = 0;
		wishvel[1] = 0;
		wishvel[2] = 0;
	} else {
		for (i=0 ; i<3 ; i++) {
			wishvel[i] = scale * pml.forward[i]*pm->cmd.forwardmove + scale * pml.right[i]*pm->cmd.rightmove;
		}

		wishvel[2] += scale * pm->cmd.upmove;
	}

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	PM_Accelerate (wishdir, wishspeed, pm_flyaccelerate);

	PM_StepSlideMove( qfalse );
}


/*
===================
PM_AirMove

===================
*/
static void PM_AirMove( void ) {
	int			i;
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;
	usercmd_t	cmd;

	if ( PM_CheckDoubleJump () ) {
		pm->ps->pm_flags |=  PMF_DOUBLEJUMPING;
		// jumped away
		if ( pm->waterlevel > 1 ) {
			PM_WaterMove();
		} //else {
		//	PM_AirMove();
		//}

		// this will probably never happen so the second jump won't
		// drain your sprint meter. Just like shrub.
		// Take out the if {} to make it drain
		if (!(pm->cmd.serverTime - pm->pmext->jumpTime < 850)) {

			pm->pmext->sprintTime -= 2500;
			if (pm->pmext->sprintTime < 0)
				pm->pmext->sprintTime = 0;

			pm->pmext->jumpTime = pm->cmd.serverTime;
		}

		// JPW NERVE
		pm->ps->jumpTime = pm->cmd.serverTime;	// Arnout: NOTE : TEMP DEBUG
		
		//return;
	}

	PM_Friction();

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.rightmove;

	// project moves down to flat plane
	if( pm->cmd.serverTime - pm->pmext->dodgeTime < 350 ) {
		pml.forward[2] = fmove = 0;
		smove = pm->pmext->dtmove == DT_MOVELEFT ? -2070 : 2070;
		scale = 1.f;
	} else {
		cmd = pm->cmd;
		scale = PM_CmdScale( &cmd );

	// Ridah, moved this down, so we use the actual movement direction
		// set the movementDir so clients can rotate the legs for strafing
	//	PM_SetMovementDir();


		pml.forward[2] = 0;
		pml.right[2] = 0;
	}
	VectorNormalize (pml.forward);
	VectorNormalize (pml.right);

	for ( i = 0 ; i < 2 ; i++ ) {
		wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
	}
	wishvel[2] = 0;

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	wishspeed *= scale;

	// not on ground, so little effect on velocity
	PM_Accelerate (wishdir, wishspeed, pm_airaccelerate);

	// we may have a ground plane that is very steep, even
	// though we don't have a groundentity
	// slide along the steep plane
	if ( pml.groundPlane ) {
		PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal, 
			pm->ps->velocity, OVERCLIP );
	}

#if 0
	//ZOID:  If we are on the grapple, try stair-stepping
	//this allows a player to use the grapple to pull himself
	//over a ledge
	if (pm->ps->pm_flags & PMF_GRAPPLE_PULL)
		PM_StepSlideMove ( qtrue );
	else
		PM_SlideMove ( qtrue );
#endif

	PM_StepSlideMove ( qtrue );

// Ridah, moved this down, so we use the actual movement direction
	// set the movementDir so clients can rotate the legs for strafing
	PM_SetMovementDir();
}



/*
===================
PM_TankMove

===================
*/
static void PM_TankMove( void ) {
#ifdef __VEHICLES__
	int			i;
	vec3_t		wishvel;
	float		fmove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;
	usercmd_t	cmd;
	float		accelerate;
	float		vel;
	vec3_t		mins, maxs;

	pm->ps->eFlags &= ~EF_PRONE; // have you ever seen proning tanks??

	//VectorCopy( tankMaxs, pm->maxs ); // hmm solve everywhere
	//VectorCopy( tankMins, pm->mins );
	switch (pm->ps->stats[STAT_VEHICLE_CLASS])
	{
	case VEHICLE_CLASS_HEAVY_TANK:
		VectorCopy (heavyTankMaxs,maxs);
		VectorCopy (heavyTankMins,mins);
		break;
	case VEHICLE_CLASS_MEDIUM_TANK:
	case VEHICLE_CLASS_FLAMETANK:
		VectorCopy (mediumTankMaxs,maxs);
		VectorCopy (mediumTankMins,mins);
		break;
	case VEHICLE_CLASS_LIGHT_TANK:
		VectorCopy (lightTankMaxs,maxs);
		VectorCopy (lightTankMins,mins);
		break;
	case VEHICLE_CLASS_APC:
		VectorCopy (lightTankMaxs,maxs);
		VectorCopy (lightTankMins,mins);
		break;
	case VEHICLE_CLASS_JEEP:
		VectorCopy (lightTankMaxs,maxs);
		VectorCopy (lightTankMins,mins);
		break;
	case VEHICLE_CLASS_CAR:
		VectorCopy (lightTankMaxs,maxs);
		VectorCopy (lightTankMins,mins);
		break;
	case VEHICLE_CLASS_SHIP:
		VectorCopy (heavyTankMaxs,maxs);
		VectorCopy (heavyTankMins,mins);
		break;
	case VEHICLE_CLASS_PLANE:
		VectorCopy (lightTankMaxs,maxs);
		VectorCopy (lightTankMins,mins);
		break;
    case VEHICLE_CLASS_ARTILLERY:
		VectorCopy (mediumTankMaxs,maxs);
		VectorCopy (mediumTankMins,mins);
		break;
	default:
		VectorCopy (tankMaxs,maxs);
		VectorCopy (tankMins,mins);
		break;
	}

	VectorCopy( maxs, pm->maxs ); // hmm solve everywhere
	VectorCopy( mins, pm->mins );

	// @TODO
	if ( pm->waterlevel > 2 && DotProduct( pml.forward, pml.groundTrace.plane.normal ) > 0 ) {
		// begin swimming
		PM_WaterMove();
		return;
	}

	if( pm->waterlevel <= 1 && PM_CheckDodge () ) {
		PM_AirMove();
		return;
	}

	PM_Friction ();

	if (pm->ps->stats[STAT_VEHICLE_CLASS] == VEHICLE_CLASS_ARTILLERY)
		pm->cmd.forwardmove = 0; // ARTY can't move forward/back...

	fmove = pm->cmd.forwardmove;

	cmd = pm->cmd;
	scale = PM_CmdScale( &cmd );

	// project moves down to flat plane
	pml.forward[2] = 0;
	pml.right[2] = 0;

	// project the forward and right directions onto the ground plane
	PM_ClipVelocity (pml.forward, pml.groundTrace.plane.normal, pml.forward, OVERCLIP );
	PM_ClipVelocity (pml.right, pml.groundTrace.plane.normal, pml.right, OVERCLIP );
	//
	VectorNormalize (pml.forward);
	VectorNormalize (pml.right);

	for ( i = 0 ; i < 3 ; i++ ) {
		wishvel[i] = pml.forward[i]*fmove;
	}

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	wishspeed *= scale;

	// clamp the speed lower if wading or walking on the bottom
	if ( pm->waterlevel ) {
		float	waterScale;

		waterScale = pm->waterlevel / 3.0;
		if ( pm->watertype == CONTENTS_SLIME )	//----(SA)	slag
			waterScale = 1.0 - ( 1.0 - pm_slagSwimScale ) * waterScale;
		else
			waterScale = 1.0 - ( 1.0 - pm_waterSwimScale ) * waterScale;

		if ( wishspeed > pm->ps->speed * waterScale ) {
			wishspeed = pm->ps->speed * waterScale;
		}
	}

	switch (pm->ps->stats[STAT_VEHICLE_CLASS])
	{
		case VEHICLE_CLASS_HEAVY_TANK:
			wishspeed *= 0.75;
			break;
		case VEHICLE_CLASS_MEDIUM_TANK:
		case VEHICLE_CLASS_FLAMETANK:
			wishspeed *= 0.9;
			break;
		case VEHICLE_CLASS_LIGHT_TANK:
			wishspeed *= 1.2;
			break;
		case VEHICLE_CLASS_APC:
			wishspeed *= 1.8;
			break;
		case VEHICLE_CLASS_JEEP:
			wishspeed *= 2.0;
			break;
		case VEHICLE_CLASS_CAR:
			wishspeed *= 2.25;
			break;
		case VEHICLE_CLASS_SHIP:
			wishspeed *= 0.5;
			break;
		case VEHICLE_CLASS_PLANE:
			wishspeed *= 4.0;
			break;
        case VEHICLE_CLASS_ARTILLERY://Masteries, Artillery can rotate to right/left, but cannot go forward/backward
			wishspeed *= 0.25;//very slow, like real artillery
			break;
		default:
			wishspeed *= 1.0;
			break;
	}

	accelerate = 7; // lower than player , higher than slick

	PM_Accelerate (wishdir, wishspeed, accelerate);

//	pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime; 

//	slope //

	vel = VectorLength(pm->ps->velocity);

	// slide along the ground plane
	PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal, pm->ps->velocity, OVERCLIP );

	// don't do anything if standing still
	if (!pm->ps->velocity[0] && !pm->ps->velocity[1]) {
		return;
	}

	// don't decrease velocity when going up or down a slope
	VectorNormalize(pm->ps->velocity);
	VectorScale(pm->ps->velocity, vel, pm->ps->velocity);

	PM_StepSlideMove( qtrue/*qfalse*/ );

// Ridah, moved this down, so we use the actual movement direction
	// set the movementDir so clients can rotate the legs for strafing
	PM_SetMovementDir();
#endif //__VEHICLES__
}


/*
===================
PM_WalkMove

===================
*/
static void PM_WalkMove( void ) {
	int			i;
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;
	usercmd_t	cmd;
	float		accelerate;
	float		vel;
//	float botBonus = 1.0;

/*#ifdef CGAMEDLL
	int gametype = cg_gameType.integer;
#elif GAMEDLL
	int gametype = g_gametype.integer;
#endif*/

	if ( pm->waterlevel > 2 && DotProduct( pml.forward, pml.groundTrace.plane.normal ) > 0 ) {
		// begin swimming
		PM_WaterMove();
		return;
	}

	if ( PM_CheckJump () ) {
		// jumped away
		if ( pm->waterlevel > 1 ) {
			PM_WaterMove();
		} else {
			PM_AirMove();
		}

		if (!(pm->cmd.serverTime - pm->pmext->jumpTime < 850)) {

			pm->pmext->sprintTime -= 2500;
			if (pm->pmext->sprintTime < 0)
				pm->pmext->sprintTime = 0;

			pm->pmext->jumpTime = pm->cmd.serverTime;
		}

		// JPW NERVE
		pm->ps->jumpTime = pm->cmd.serverTime;	// Arnout: NOTE : TEMP DEBUG
		
		return;
	} else/* if( !PM_CheckProne() )*/ {
		if( pm->waterlevel <= 1 && PM_CheckDodge () ) {
			PM_AirMove();
			return;
		}
	}

	/*if( pm->waterlevel <= 1 && PM_CheckDodge () ) {
		PM_AirMove();
		return;
	}*/

	PM_Friction ();

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.rightmove;

	cmd = pm->cmd;
	scale = PM_CmdScale( &cmd );

// Ridah, moved this down, so we use the actual movement direction
	// set the movementDir so clients can rotate the legs for strafing
//	PM_SetMovementDir();

	// project moves down to flat plane
	pml.forward[2] = 0;
	pml.right[2] = 0;

	// project the forward and right directions onto the ground plane
	PM_ClipVelocity (pml.forward, pml.groundTrace.plane.normal, pml.forward, OVERCLIP );
	PM_ClipVelocity (pml.right, pml.groundTrace.plane.normal, pml.right, OVERCLIP );
	//
	VectorNormalize (pml.forward);
	VectorNormalize (pml.right);

	for ( i = 0 ; i < 3 ; i++ ) {
		wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
	}
	// when going up or down slopes the wish velocity should Not be zero
//	wishvel[2] = 0;

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	wishspeed *= scale;

	// clamp the speed lower if prone
	if ( pm->ps->eFlags & EF_PRONE ) {
		if ( wishspeed > pm->ps->speed * pm_proneSpeedScale ) {
			wishspeed = pm->ps->speed * pm_proneSpeedScale;
		}
	} else if ( pm->ps->pm_flags & PMF_DUCKED ) { // clamp the speed lower if ducking
		/*if ( wishspeed > pm->ps->speed * pm_duckScale ) {
			wishspeed = pm->ps->speed * pm_duckScale;
		}*/
		if ( wishspeed > pm->ps->speed * pm->ps->crouchSpeedScale ) {
			wishspeed = pm->ps->speed * pm->ps->crouchSpeedScale;
		}
	}

	// clamp the speed lower if wading or walking on the bottom
	if ( pm->waterlevel ) {
		float	waterScale;

		waterScale = pm->waterlevel / 3.0;
		if ( pm->watertype == CONTENTS_SLIME )	//----(SA)	slag
			waterScale = 1.0 - ( 1.0 - pm_slagSwimScale ) * waterScale;
		else
			waterScale = 1.0 - ( 1.0 - pm_waterSwimScale ) * waterScale;

		if ( wishspeed > pm->ps->speed * waterScale ) {
			wishspeed = pm->ps->speed * waterScale;
		}
	}

	// when a player gets hit, they temporarily lose
	// full control, which allows them to be moved a bit
#ifdef GAMEDLL
	if ( (!(g_entities[pm->ps->clientNum].r.svFlags & SVF_BOT) && !pm->isNPC && ( pml.groundTrace.surfaceFlags & SURF_SLICK )) || (pm->ps->pm_flags & PMF_TIME_KNOCKBACK) ) {
#else //!GAMEDLL
	if ( ( pml.groundTrace.surfaceFlags & SURF_SLICK ) || pm->ps->pm_flags & PMF_TIME_KNOCKBACK ) {
#endif //GAMEDLL
		accelerate = pm_airaccelerate;
	} 
	else {
		accelerate = pm_accelerate;
	}

	PM_Accelerate (wishdir, wishspeed, accelerate);

	//Com_Printf("velocity = %1.1f %1.1f %1.1f\n", pm->ps->velocity[0], pm->ps->velocity[1], pm->ps->velocity[2]);
	//Com_Printf("velocity1 = %1.1f\n", VectorLength(pm->ps->velocity));

#ifdef GAMEDLL
	if ( (!(g_entities[pm->ps->clientNum].r.svFlags & SVF_BOT) && !pm->isNPC && ( pml.groundTrace.surfaceFlags & SURF_SLICK )) || (pm->ps->pm_flags & PMF_TIME_KNOCKBACK) ) {
#else //!GAMEDLL
	if ( ( pml.groundTrace.surfaceFlags & SURF_SLICK ) || pm->ps->pm_flags & PMF_TIME_KNOCKBACK ) {
#endif //GAMEDLL
		pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
	} 
	else {
		// don't reset the z velocity for slopes
		//pm->ps->velocity[2] = 0;
	}

//----(SA)	added
	// show breath when standing on 'snow' surfaces
	if(pml.groundTrace.surfaceFlags & SURF_SNOW)
		pm->ps->eFlags |= EF_BREATH;
	else
		pm->ps->eFlags &= ~EF_BREATH;
//----(SA)	end

	vel = VectorLength(pm->ps->velocity);

	// slide along the ground plane
	PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal, pm->ps->velocity, OVERCLIP );

	// don't do anything if standing still
	if (!pm->ps->velocity[0] && !pm->ps->velocity[1]) {
		if( pm->ps->eFlags & EF_PRONE ) {
			pm->pmext->proneGroundTime = pm->cmd.serverTime;
		}
		return;
	}

	// don't decrease velocity when going up or down a slope
	VectorNormalize(pm->ps->velocity);
	VectorScale(pm->ps->velocity, vel, pm->ps->velocity);

	PM_StepSlideMove( qfalse );

// Ridah, moved this down, so we use the actual movement direction
	// set the movementDir so clients can rotate the legs for strafing
	PM_SetMovementDir();
}


/*
==============
PM_DeadMove
==============
*/
static void PM_DeadMove( void ) {
	float	forward;

	if ( !pml.walking ) {
		return;
	}

	// extra friction

	forward = VectorLength (pm->ps->velocity);
	forward -= 20;
	if ( forward <= 0 ) {
		VectorClear (pm->ps->velocity);
	} else {
		VectorNormalize (pm->ps->velocity);
		VectorScale (pm->ps->velocity, forward, pm->ps->velocity);
	}

}


/*
===============
PM_NoclipMove
===============
*/
static void PM_NoclipMove( void ) {
	float	speed, drop, friction, control, newspeed;
	int			i;
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;

	pm->ps->viewheight = DEFAULT_VIEWHEIGHT;

	// friction

	speed = VectorLength (pm->ps->velocity);
	if (speed < 1)
	{
		VectorCopy (vec3_origin, pm->ps->velocity);
	}
	else
	{
		drop = 0;

		friction = pm_friction*1.5;	// extra friction
		control = speed < pm_stopspeed ? pm_stopspeed : speed;
		drop += control*friction*pml.frametime;

		// scale the velocity
		newspeed = speed - drop;
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;

		VectorScale (pm->ps->velocity, newspeed, pm->ps->velocity);
	}

	// accelerate
	scale = PM_CmdScale( &pm->cmd );

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.rightmove;
	
	for (i=0 ; i<3 ; i++)
		wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
	wishvel[2] += pm->cmd.upmove;

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	wishspeed *= scale;

	PM_Accelerate( wishdir, wishspeed, pm_accelerate );

	// move
	VectorMA (pm->ps->origin, pml.frametime, pm->ps->velocity, pm->ps->origin);
}

//============================================================================

/*
================
PM_FootstepForSurface

Returns an event number apropriate for the groundsurface
================
*/
static int PM_FootstepForSurface( void ) 
{
#ifdef GAMEDLL
	// In just the GAME DLL, we want to store the groundtrace surface stuff,
	// so we don't have to keep tracing.
	ClientStoreSurfaceFlags(pm->ps->clientNum, pml.groundTrace.surfaceFlags);

#endif // GAMEDLL

	return BG_FootstepForSurface( pml.groundTrace.surfaceFlags );
}

/*
=================
PM_CrashLand

Check for hard landings that generate sound events
=================
*/
static void PM_CrashLand( void ) {
	float		delta;
	float		dist;
	float		vel, acc;
	float		t;
	float		a, b, c, den;

#ifdef __NPC__
	if (pm->isNPC)
		return; // UQ1: Disabled. Stop it messing local client view! And save bandwidth...
#endif //__NPC__

	// Ridah, only play this if coming down hard
	if (!pm->ps->legsTimer)	{
		if (pml.previous_velocity[2] < -220)
		{
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_LAND, qfalse, qtrue );
		}
	}

	// calculate the exact velocity on landing
	dist = pm->ps->origin[2] - pml.previous_origin[2];
	vel = pml.previous_velocity[2];
	acc = -pm->ps->gravity;

	a = acc / 2;
	b = vel;
	c = -dist;

	den =  b * b - 4 * a * c;
	if ( den < 0 ) {
		return;
	}
	t = (-b - sqrt( den ) ) / ( 2 * a );

	delta = vel + t * acc;
	delta = delta*delta * 0.0001;

	// never take falling damage if completely underwater
	if ( pm->waterlevel == 3 ) {
		return;
	}

	// reduce falling damage if there is standing water
	if ( pm->waterlevel == 2 ) {
		delta *= 0.25;
	}
	if ( pm->waterlevel == 1 ) {
		delta *= 0.5;
	}

	if ( delta < 1 ) {
		return;
	}

	// create a local entity event to play the sound

	// SURF_NODAMAGE is used for bounce pads where you don't ever
	// want to take damage or play a crunch sound
	if ( !(pml.groundTrace.surfaceFlags & SURF_NODAMAGE) && !pm->predict
#ifdef __VEHICLES__
		&& !(pm->ps->eFlags & EF_VEHICLE) 
#endif //__VEHICLES__
		)  
	{
		if ( pm->debugLevel )
			Com_Printf ("delta: %5.2f\n", delta);

/* JPW NERVE removed from MP, breaks too many levels and skill as no-fall-damage indicator isn't obvious
		// Rafael gameskill
		if (bg_pmove_gameskill_integer == 1)
		{
			if (delta > 7)
				delta = 8;
		}
		// done
*/

		if (delta > 77)
		{
			PM_AddEventExt( EV_FALL_NDIE, PM_FootstepForSurface() );
		}
		//else if (delta > 67)
		//{
		//	PM_AddEvent(EV_FALL_DMG_75);
		//}
		else if (delta > 67)
		{
			PM_AddEventExt( EV_FALL_DMG_50, PM_FootstepForSurface() );
		}
		//else if (delta > 48)
		//{
		//	PM_AddEvent(EV_FALL_DMG_30);
		//}
		else if (delta > 58)
		{
			// this is a pain grunt, so don't play it if dead
			if ( pm->ps->stats[STAT_HEALTH] > 0 ) 
			{
				PM_AddEventExt( EV_FALL_DMG_25, PM_FootstepForSurface() );
			}
		}
		else if (delta > 48)
		{
			// this is a pain grunt, so don't play it if dead
			if ( pm->ps->stats[STAT_HEALTH] > 0 ) 
			{
				PM_AddEventExt( EV_FALL_DMG_15, PM_FootstepForSurface() );
			}
		}
		else if (delta > 38.75)
		{
			// this is a pain grunt, so don't play it if dead
			if ( pm->ps->stats[STAT_HEALTH] > 0 ) 
			{
				PM_AddEventExt( EV_FALL_DMG_10, PM_FootstepForSurface() );
			}
		}
		else if ( delta > 7 ) 
		{
			PM_AddEventExt( EV_FALL_SHORT, PM_FootstepForSurface() );
		} 
		else 
		{
			PM_AddEventExt( EV_FOOTSTEP, PM_FootstepForSurface() );
		}
	}

	// rain - when falling damage happens, velocity is cleared, but
	// this needs to happen in pmove, not g_active!  (prediction will be
	// wrong, otherwise.)
	if (delta > 38.75)
		VectorClear(pm->ps->velocity);

	// start footstep cycle over
	pm->ps->bobCycle = 0;
}



/*
=============
PM_CorrectAllSolid
=============
*/
static int PM_CorrectAllSolid( trace_t *trace ) {
	int			i, j, k;
	vec3_t		point;

	if ( pm->debugLevel ) {
		Com_Printf("%i:allsolid\n", c_pmove);
	}

	// jitter around
 	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			for (k = -1; k <= 1; k++) {
				VectorCopy(pm->ps->origin, point);
				point[0] += (float) i;
				point[1] += (float) j;
				point[2] += (float) k;
				PM_TraceAll(trace, point, point);
				if ( !trace->allsolid ) {
					point[0] = pm->ps->origin[0];
					point[1] = pm->ps->origin[1];
					point[2] = pm->ps->origin[2] - 0.25;

					PM_TraceAll(trace, pm->ps->origin, point);
					pml.groundTrace = *trace;
					return qtrue;
				}
			}
		}
	}

	pm->ps->groundEntityNum = ENTITYNUM_NONE;
	pml.groundPlane = qfalse;
	pml.walking = qfalse;

	return qfalse;
}


/*
=============
PM_GroundTraceMissed

The ground trace didn't hit a surface, so we are in freefall
=============
*/
static void PM_GroundTraceMissed( void ) {
	trace_t		trace;
	vec3_t		point;

	if ( pm->ps->groundEntityNum != ENTITYNUM_NONE ) {
		// we just transitioned into freefall
		if ( pm->debugLevel ) {
			Com_Printf("%i:lift\n", c_pmove);
		}

		// if they aren't in a jumping animation and the ground is a ways away, force into it
		// if we didn't do the trace, the player would be backflipping down staircases
		VectorCopy( pm->ps->origin, point );
		point[2] -= 64;

		PM_TraceAll( &trace, pm->ps->origin, point );
		if ( trace.fraction == 1.0 ) {
			if ( pm->cmd.forwardmove >= 0 ) {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMP, qfalse, qtrue );
				pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
			} else {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMPBK, qfalse, qtrue );
				pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
			}
		}
	}

	// If we've never yet touched the ground, it's because we're spawning, so don't 
	// set to "in air"
	if (pm->ps->groundEntityNum != -1)
	{
		// Signify that we're in mid-air
		pm->ps->groundEntityNum = ENTITYNUM_NONE;

	} // if (pm->ps->groundEntityNum != -1)...
	pml.groundPlane = qfalse;
	pml.walking = qfalse;
}


/*
=============
PM_GroundTrace
=============
*/
static void PM_GroundTrace( void ) {
	vec3_t		point;
	trace_t		trace;

	point[0] = pm->ps->origin[0];
	point[1] = pm->ps->origin[1];


	if ( pm->ps->eFlags & EF_MG42_ACTIVE || pm->ps->eFlags & EF_AAGUN_ACTIVE ) {
		point[2] = pm->ps->origin[2] - 1.f;
	} else {
		//point[2] = pm->ps->origin[2] - 0.01f;
		point[2] = pm->ps->origin[2] - 0.25f;
	}

	PM_TraceAllParts( &trace, &pm->pmext->proneLegsOffset, pm->ps->origin, point );
	pml.groundTrace = trace;

	// do something corrective if the trace starts in a solid...
	if ( trace.allsolid && !(pm->ps->eFlags & EF_MOUNTEDTANK)) {
		if ( !PM_CorrectAllSolid(&trace) )
			return;
	}

	// if the trace didn't hit anything, we are in free fall
	if ( trace.fraction == 1.0 ) {
		PM_GroundTraceMissed();
		pml.groundPlane = qfalse;
		pml.walking = qfalse;
		return;
	}

	// check if getting thrown off the ground
	if ( pm->ps->velocity[2] > 0 && DotProduct( pm->ps->velocity, trace.plane.normal ) > 10 && !( pm->ps->eFlags & EF_PRONE ) ) {
		if ( pm->debugLevel ) {
			Com_Printf("%i:kickoff\n", c_pmove);
		}
		// go into jump animation
		if ( pm->cmd.forwardmove >= 0 ) {
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMP, qfalse, qfalse );
			pm->ps->pm_flags &= ~PMF_BACKWARDS_JUMP;
		} else {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_JUMPBK, qfalse, qfalse );
			pm->ps->pm_flags |= PMF_BACKWARDS_JUMP;
		}

		pm->ps->groundEntityNum = ENTITYNUM_NONE;
		pml.groundPlane = qfalse;
		pml.walking = qfalse;
		return;
	}

	// slopes that are too steep will not be considered onground
	{
		float minNormal;

#ifdef __VEHICLES__
		if (pm->ps->eFlags & EF_VEHICLE) {
			minNormal = 0.85;
		} else {
#endif //__VEHICLES__
			minNormal = MIN_WALK_NORMAL;
#ifdef __VEHICLES__
		}
#endif //__VEHICLES__

		if ( trace.plane.normal[2] < minNormal ) 
		{
			if ( pm->debugLevel ) {
				Com_Printf("%i:steep\n", c_pmove);
			}
			// FIXME: if they can't slide down the slope, let them
			// walk (sharp crevices)
			pm->ps->groundEntityNum = ENTITYNUM_NONE;
			pml.groundPlane = qtrue;
			pml.walking = qfalse;
			return;
		}
	}

	pml.groundPlane = qtrue;
	pml.walking = qtrue;
	pm->ps->pm_flags &= ~PMF_DOUBLEJUMPING;

	// hitting solid ground will end a waterjump
	if (pm->ps->pm_flags & PMF_TIME_WATERJUMP)
	{
		pm->ps->pm_flags &= ~(PMF_TIME_WATERJUMP | PMF_TIME_LAND);
		pm->ps->pm_time = 0;
	}

	if ( pm->ps->groundEntityNum == ENTITYNUM_NONE ) {
		// just hit the ground
		if ( pm->debugLevel ) {
			Com_Printf("%i:Land\n", c_pmove);
		}
		
		PM_CrashLand();

		// don't do landing time if we were just going down a slope
		if ( pml.previous_velocity[2] < -200 ) {
			// don't allow another jump for a little while
			pm->ps->pm_flags |= PMF_TIME_LAND;
			pm->ps->pm_time = 250;
		}
	}

	pm->ps->groundEntityNum = trace.entityNum;

	// don't reset the z velocity for slopes
//	pm->ps->velocity[2] = 0;

	PM_AddTouchEnt( trace.entityNum );
}


/*
=============
PM_SetWaterLevel	FIXME: avoid this twice?  certainly if not moving
=============
*/
static void PM_SetWaterLevel( void ) {
	vec3_t		point;
	int			cont;
	int			sample1;
	int			sample2;

	//
	// get waterlevel, accounting for ducking
	//
	pm->waterlevel = 0;
	pm->watertype = 0;

	// Ridah, modified this
	point[0] = pm->ps->origin[0];
	point[1] = pm->ps->origin[1];
	point[2] = pm->ps->origin[2] + pm->ps->mins[2] + 1;	
	cont = pm->pointcontents( point, pm->ps->clientNum );

	if ( cont & MASK_WATER ) {
		sample2 = pm->ps->viewheight - pm->ps->mins[2];
		sample1 = sample2 / 2;

		pm->watertype = cont;
		pm->waterlevel = 1;
		point[2] = pm->ps->origin[2] + pm->ps->mins[2] + sample1;
		cont = pm->pointcontents (point, pm->ps->clientNum );
		if ( cont & MASK_WATER ) {
			pm->waterlevel = 2;
			point[2] = pm->ps->origin[2] + pm->ps->mins[2] + sample2;
			cont = pm->pointcontents (point, pm->ps->clientNum );
			if ( cont & MASK_WATER ){
				pm->waterlevel = 3;
			}
		}
	}
	// done.

	// UNDERWATER
	BG_UpdateConditionValue( pm->ps->clientNum, ANIM_COND_UNDERWATER, (pm->waterlevel > 2), qtrue );

}

/*
==============
PM_CheckDuck

Sets mins, maxs, and pm->ps->viewheight
==============
*/
static void PM_CheckDuck (void)
{
	trace_t	trace;

	// Ridah, modified this for configurable bounding boxes
	pm->mins[0] = pm->ps->mins[0];
	pm->mins[1] = pm->ps->mins[1];

	pm->maxs[0] = pm->ps->maxs[0];
	pm->maxs[1] = pm->ps->maxs[1];

	pm->mins[2] = pm->ps->mins[2];

	if( pm->ps->pm_type == PM_DEAD ) {
		pm->maxs[2] = pm->ps->maxs[2];			// NOTE: must set death bounding box in game code
		pm->ps->viewheight = pm->ps->deadViewHeight;
		return;
	}

	if( (pm->cmd.upmove < 0 && !(pm->ps->eFlags & EF_MOUNTEDTANK) && !(pm->ps->pm_flags & PMF_LADDER) ) || pm->ps->weapon == WP_MORTAR_SET )
	{	// duck
		pm->ps->pm_flags |= PMF_DUCKED;
	}
	else
	{	// stand up if possible
		if (pm->ps->pm_flags & PMF_DUCKED)
		{
			// try to stand up
			pm->maxs[2] = pm->ps->maxs[2];
			PM_TraceAll( &trace, pm->ps->origin, pm->ps->origin );
			if (trace.fraction == 1.0f)
				pm->ps->pm_flags &= ~PMF_DUCKED;
		}
	}

	if (pm->ps->pm_flags & PMF_DUCKED)
	{
		pm->maxs[2] = pm->ps->crouchMaxZ;
		pm->ps->viewheight = pm->ps->crouchViewHeight;
	}
	else
	{
		pm->maxs[2] = pm->ps->maxs[2];
		pm->ps->viewheight = pm->ps->standViewHeight;
	}
	// done.
}



//===================================================================


/*
===============
PM_Footsteps
===============
*/
static void PM_Footsteps( void ) {
	float		bobmove;
	int			old;
	qboolean	footstep;
	qboolean	iswalking;
	int			animResult=-1;

#ifdef __VEHICLES__
	if (pm->ps->eFlags & EF_VEHICLE) 
	{
		return;
	}
#endif //__VEHICLES__

	if (pm->ps->eFlags & EF_DEAD) {

		if( pm->ps->pm_flags & PMF_FLAILING ) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_FLAILING, qtrue );

			if( !pm->ps->pm_time )
				pm->ps->pm_flags &= ~PMF_FLAILING;	// the eagle has landed
		} 
		else if(!pm->ps->pm_time && !(pm->ps->pm_flags & PMF_LIMBO) ) { // DHM - Nerve :: before going to limbo, play a wounded/fallen animation
			if ( pm->ps->groundEntityNum == ENTITYNUM_NONE ) {
				// takeoff!
				pm->ps->pm_flags |= PMF_FLAILING;
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_FLAILING, qtrue );
			} else {
				//animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_FALLEN, qtrue );
			}
		}

		return;
	}

	iswalking = qfalse;

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	pm->xyspeed = sqrt( pm->ps->velocity[0] * pm->ps->velocity[0] +  pm->ps->velocity[1] * pm->ps->velocity[1] );
	
	// mg42, always idle
	if ( pm->ps->persistant[PERS_HWEAPON_USE] ) {
		animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLE, qtrue );
		//
		return;
	}

	// swimming
	if ( pm->waterlevel > 2 ) {

		if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_SWIMBK, qtrue );
		} else {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_SWIM, qtrue );
		}

		return;
	}

	// in the air
	if ( pm->ps->groundEntityNum == ENTITYNUM_NONE ) {
		if(pm->ps->pm_flags & PMF_LADDER)				// on ladder
		{
			if (pm->ps->velocity[2] >= 0)
			{
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_CLIMBUP, qtrue );
				//BG_PlayAnimName( pm->ps, "BOTH_CLIMB", ANIM_BP_BOTH, qfalse, qtrue, qfalse );
			}
			else if (pm->ps->velocity[2] < 0)
			{
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_CLIMBDOWN, qtrue );
				//BG_PlayAnimName( pm->ps, "BOTH_CLIMB_DOWN", ANIM_BP_BOTH, qfalse, qtrue, qfalse );
			}
		}

		return;
	}

	// if not trying to move
	if ( !pm->cmd.forwardmove && !pm->cmd.rightmove ) {
		if (  pm->xyspeed < 5 ) {
			pm->ps->bobCycle = 0;	// start at beginning of cycle again
		}
		if (pm->xyspeed > 120) {
			return;	// continue what they were doing last frame, until we stop
		}

		if ( pm->ps->eFlags & EF_PRONE ) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLEPRONE, qtrue );
		} else if ( pm->ps->pm_flags & PMF_DUCKED ) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLECR, qtrue );
		}

		if (animResult < 0) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLE, qtrue );
		}
		//
		return;
	}

	footstep = qfalse;


	if ( pm->ps->eFlags & EF_PRONE ) {
		bobmove = 0.2;	// prone characters bob slower
		if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_PRONEBK, qtrue );
		} else {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_PRONE, qtrue );
		}
		// prone characters never play footsteps
	} else if ( pm->ps->pm_flags & PMF_DUCKED ) {
		bobmove = 0.5;	// ducked characters bob much faster
		if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_WALKCRBK, qtrue );
		} else {
			animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_WALKCR, qtrue );
		}
		// ducked characters never play footsteps
	} else 	if ( pm->ps->pm_flags & PMF_BACKWARDS_RUN ) {
		if ( !( pm->cmd.buttons & BUTTON_WALKING )
#if defined (__FRONTLINE__) || defined (__EF__)
		 && !( pm->ps->eFlags & EF_AIMING)
#endif //defined (__FRONTLINE__) || defined (__EF__)
			) {
			bobmove = 0.4;	// faster speeds bob faster
			footstep = qtrue;
			// check for strafing
			if ( pm->cmd.rightmove && !pm->cmd.forwardmove ) {
				if ( pm->cmd.rightmove > 0 ) {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFERIGHT, qtrue );
				} else {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFELEFT, qtrue );
				}
			}
			if (animResult < 0) {	// if we havent found an anim yet, play the run
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_RUNBK, qtrue );
			}
		} else {
			bobmove = 0.3;
			// check for strafing
			if ( pm->cmd.rightmove && !pm->cmd.forwardmove ) {
				if ( pm->cmd.rightmove > 0 ) {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFERIGHT, qtrue );
				} else {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFELEFT, qtrue );
				}
			}
			if (animResult < 0) {	// if we havent found an anim yet, play the run
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_WALKBK, qtrue );
			}
		}

	} else {

		if ( !( pm->cmd.buttons & BUTTON_WALKING )
#if defined (__FRONTLINE__) || defined (__EF__)
		 && !( pm->ps->eFlags & EF_AIMING)
#endif //defined (__FRONTLINE__) || defined (__EF__)
			) {
			bobmove = 0.4;	// faster speeds bob faster
			footstep = qtrue;
			// check for strafing
			if ( pm->cmd.rightmove && !pm->cmd.forwardmove ) {
				if ( pm->cmd.rightmove > 0 ) {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFERIGHT, qtrue );
				} else {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFELEFT, qtrue );
				}
			}
			if (animResult < 0) {	// if we havent found an anim yet, play the run
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_RUN, qtrue );
			}
		} else {
			bobmove = 0.3;	// walking bobs slow
			if ( pm->cmd.rightmove && !pm->cmd.forwardmove ) {
				if ( pm->cmd.rightmove > 0 ) {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFERIGHT, qtrue );
				} else {
					animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_STRAFELEFT, qtrue );
				}
			}
			if (animResult < 0) {	// if we havent found an anim yet, play the run
				animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_WALK, qtrue );
			}
		}
	}

	// if no anim found yet, then just use the idle as default
	if (animResult < 0) {
		animResult = BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLE, qtrue );
	}

	// check for footstep / splash sounds
	old = pm->ps->bobCycle;
	pm->ps->bobCycle = (int)( old + bobmove * pml.msec ) & 255;

	// if we just crossed a cycle boundary, play an apropriate footstep event
	if (iswalking) {
		// sounds much more natural this way
		if (old > pm->ps->bobCycle) {
		
			if ( pm->waterlevel == 0 ) {
				if ( footstep && !pm->noFootsteps ) {
					PM_AddEventExt( EV_FOOTSTEP, PM_FootstepForSurface() );
				}
			} else if ( pm->waterlevel == 1 ) {
				// splashing
				PM_AddEvent( EV_FOOTSPLASH );
			} else if ( pm->waterlevel == 2 ) {
				// wading / swimming at surface
				PM_AddEvent( EV_SWIM );
			} else if ( pm->waterlevel == 3 ) {
				// no sound when completely underwater
			}

		}
	} else if ( ( ( old + 64 ) ^ ( pm->ps->bobCycle + 64 ) ) & 128 ) {
		
/*		if (pm->ps->sprintExertTime && pm->waterlevel <= 2)
			PM_ExertSound ();*/

		if ( pm->waterlevel == 0 ) {
			// on ground will only play sounds if running
			if ( footstep && !pm->noFootsteps ) {
				PM_AddEventExt( EV_FOOTSTEP, PM_FootstepForSurface() );
			}
		} else if ( pm->waterlevel == 1 ) {
			// splashing
			PM_AddEvent( EV_FOOTSPLASH );
		} else if ( pm->waterlevel == 2 ) {
			// wading / swimming at surface
			PM_AddEvent( EV_SWIM );
		} else if ( pm->waterlevel == 3 ) {
			// no sound when completely underwater

		}
	}
}

/*
==============
PM_WaterEvents

Generate sound events for entering and leaving water
==============
*/
static void PM_WaterEvents( void ) {		// FIXME?
	//
	// if just entered a water volume, play a sound
	//
	if (!pml.previous_waterlevel && pm->waterlevel) {
		PM_AddEvent( EV_WATER_TOUCH );
	}

	//
	// if just completely exited a water volume, play a sound
	//
	if (pml.previous_waterlevel && !pm->waterlevel) {
		PM_AddEvent( EV_WATER_LEAVE );
	}

	//
	// check for head just going under water
	//
	if (pml.previous_waterlevel != 3 && pm->waterlevel == 3) {
		PM_AddEvent( EV_WATER_UNDER );
	}

	//
	// check for head just coming out of water
	//
	if (pml.previous_waterlevel == 3 && pm->waterlevel != 3) {
 		if( pm->pmext->airleft < 6000 ) {
 			PM_AddEventExt( EV_WATER_CLEAR, 1 );
		} else {
			PM_AddEventExt( EV_WATER_CLEAR, 0 );
		}
	}
}


/*
==============
PM_BeginWeaponReload
==============
*/
static void PM_BeginWeaponReload( int weapon ) {
	//gitem_t* item;
	int reloadTime;

	// only allow reload if the weapon isn't already occupied (firing is okay)
	if(pm->ps->weaponstate != WEAPON_READY && pm->ps->weaponstate != WEAPON_FIRING )
		return;

	//if(((weapon == WP_CARBINE) && pm->ps->ammoclip[WP_CARBINE] != 0) || ((weapon == WP_MOBILE_MG42 || weapon == WP_MOBILE_MG42_SET) && pm->ps->ammoclip[WP_MOBILE_MG42] != 0) || ((weapon == WP_GARAND || weapon == WP_GARAND_SCOPE) && pm->ps->ammoclip[WP_GARAND] != 0)) {
	//	return;	// Gordon: no reloading of the carbine until clip is empty
	//}
#if defined (__FRONTLINE__) || defined (__EF__)
	pm->ps->eFlags &= ~EF_AIMING;
	pm->pmext->adsTime = -pm->cmd.serverTime;
#endif //defined (__FRONTLINE__) || defined (__EF__)

	switch(weapon) {
	case WP_CARBINE:
		if(PM_GARAND_RELOADS)
			break;
		if(pm->ps->ammoclip[WP_CARBINE] != 0)
			return;
		break;
	case WP_GARAND:
	case WP_GARAND_SCOPE:
		if(PM_GARAND_RELOADS)
			break;
		if(pm->ps->ammoclip[WP_GARAND] != 0)
			return;
		break;
	case WP_MOBILE_MG42:
	case WP_MOBILE_MG42_SET:
		if(pm->ps->ammoclip[WP_MOBILE_MG42] != 0)
			return;
#ifdef __EF__
	case WP_30CAL:
	case WP_30CAL_SET:
		if(pm->ps->ammoclip[WP_30CAL] != 0)
			return;
	case WP_PTRS:
	case WP_PTRS_SET:
        if(pm->ps->ammoclip[WP_PTRS] != 0)
			return;
#endif //__EF__
		break;
	}

	if((weapon <= WP_NONE || weapon > WP_DYNAMITE) && !(weapon >= WP_KAR98 && weapon < WP_NUM_WEAPONS))
		return;

	// tjw: this should be handled in PM_CheckForReload()
	/*
	item = BG_FindItemForWeapon(weapon);
	if(!item) {
		return;
	}
	// Gordon: fixing reloading with a full clip
	if(pm->ps->ammoclip[item->giAmmoIndex] >= GetAmmoTableData(weapon)->maxclip) {
		return;
	}
	*/

	// no reload when you've got a chair in your hands
/*	if(pm->ps->eFlags & EF_MELEE_ACTIVE)
		return;*/

	// no reload when leaning (this includes manual and auto reloads)
#if !defined (__FRONTLINE__) && !defined (__EF__)
	if(pm->ps->leanf)
		return;
#endif //!defined (__FRONTLINE__) && !defined (__EF__)

	// (SA) easier check now that the animation system handles the specifics
	switch(weapon) {
		case WP_DYNAMITE:
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
//		case WP_LANDMINE:
//		case WP_TRIPMINE:
		case WP_SMOKE_BOMB:
			break;

		default:
			// DHM - Nerve :: override current animation (so reloading after firing will work)
			if( (pm->ps->eFlags & EF_PRONE) || (weapon == WP_MOBILE_MG42_SET && BG_CanSetupMobileMG42Here()) ) {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOADPRONE, qfalse, qtrue );
#ifdef __EF__
			} else if( (pm->ps->eFlags & EF_PRONE) || ((weapon == WP_30CAL_SET || weapon == WP_PTRS_SET) && BG_CanSetupMobileMG42Here()) ) { //Masteries, added reload capabilities to PTRS
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOADPRONE, qfalse, qtrue );
#endif //__EF__
			} else {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RELOAD, qfalse, qtrue );
			}
			break;
	}

    if( weapon != WP_MORTAR && weapon != WP_MORTAR_SET )
		PM_ContinueWeaponAnim(PM_ReloadAnimForWeapon(pm->ps->weapon));

	// okay to reload while overheating without tacking the reload time onto the end of the
	// current weaponTime (the reload time is partially absorbed into the overheat time)
	reloadTime = GetAmmoTableData(weapon)->reloadTime;
	if( pm->skill[SK_LIGHT_WEAPONS] >= 2 && BG_isLightWeaponSupportingFastReload( weapon ) ) {
		reloadTime *= .65f;
	}
	if( pm->ps->weaponstate == WEAPON_READY )
		pm->ps->weaponTime += reloadTime;
	else if( pm->ps->weaponTime < reloadTime)
		pm->ps->weaponTime += (reloadTime - pm->ps->weaponTime);

	pm->ps->weaponstate = WEAPON_RELOADING;
	PM_AddEvent( EV_FILL_CLIP );	// play reload sound
}

static void PM_ReloadClip( int weapon );

/*
===============
PM_BeginWeaponChange
===============
*/
void PM_BeginWeaponChange( int oldweapon, int newweapon, qboolean reload ) {	//----(SA)	modified to play 1st person alt-mode transition animations.
	int switchtime;
	qboolean altSwitchAnim = qfalse;

	if( pm->ps->pm_flags & PMF_RESPAWNED ) {
		return;		// don't allow weapon switch until all buttons are up
	}

	if( newweapon <= WP_NONE || newweapon >= WP_NUM_WEAPONS ) {
		return;
	}

	if( !( COM_BitCheck( pm->ps->weapons, newweapon ) ) ) {
		return;
	}
	
	if( pm->ps->weaponstate == WEAPON_DROPPING || pm->ps->weaponstate == WEAPON_DROPPING_TORELOAD ) {
		return;
	}

	// Gordon: don't allow change during spinup
	if( pm->ps->weaponDelay ) {
		return;
	}

	// don't allow switch if you're holding a hot potato or dynamite
	if( pm->ps->grenadeTimeLeft > 0 ) {
		return;
	}

#if defined (__FRONTLINE__) || defined (__EF__)
	pm->ps->eFlags &= ~EF_AIMING;
	pm->pmext->adsTime = -pm->cmd.serverTime;
#endif //defined (__FRONTLINE__) || defined (__EF__)

	pm->ps->nextWeapon = newweapon;

	switch( newweapon ) {
		case WP_CARBINE:
		case WP_KAR98:
			if( newweapon != weapAlts[oldweapon] ) {
				PM_AddEvent( EV_CHANGE_WEAPON );
			}
			break;
		case WP_DYNAMITE:
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_SMOKE_BOMB:
			// initialize the timer on the potato you're switching to
			pm->ps->grenadeTimeLeft = 0;
			PM_AddEvent( EV_CHANGE_WEAPON );
			break;
		case WP_MORTAR_SET:
			if( pm->ps->eFlags & EF_PRONE ) {
				return;
			}

			if( pm->waterlevel == 3 ) {
				return;
			}	
			PM_AddEvent( EV_CHANGE_WEAPON );
			break;
		default:
			//----(SA)	only play the weapon switch sound for the player
			PM_AddEvent( reload ? EV_CHANGE_WEAPON_2 : EV_CHANGE_WEAPON );
			break;
	}

	// it's an alt mode, play different anim
	if(newweapon == weapAlts[oldweapon]) {
		PM_StartWeaponAnim(PM_AltSwitchFromForWeapon(oldweapon));
	} else {
		PM_StartWeaponAnim(PM_DropAnimForWeapon(oldweapon));
	}

	switchtime = 250;	// dropping/raising usually takes 1/4 sec.
	// sometimes different switch times for alt weapons
	switch(oldweapon) {
		case WP_CARBINE:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 0;
				if(	!pm->ps->ammoclip[newweapon] && pm->ps->ammo[newweapon] ) {
					PM_ReloadClip( newweapon );
				}
			}
			break;
		case WP_M7:
			if(newweapon == weapAlts[oldweapon])
				switchtime = 0;
			break;
		case WP_KAR98:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 0;
				if(	!pm->ps->ammoclip[newweapon] && pm->ps->ammo[newweapon] ) {
					PM_ReloadClip( newweapon );
				}
			}
			break;
		case WP_GPG40:
			if(newweapon == weapAlts[oldweapon])
				switchtime = 0;
			break;
		case WP_LUGER:
			if(newweapon == weapAlts[oldweapon])
				switchtime = 0;
			break;
		case WP_SILENCED_LUGER:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 1000;
				//switchtime = 0;
				altSwitchAnim = qtrue;
			}
			break;
		case WP_COLT:
			if(newweapon == weapAlts[oldweapon])
				switchtime = 0;
			break;
		case WP_SILENCED_COLT:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 1000;
				//switchtime = 1300;
				//switchtime = 0;
				altSwitchAnim = qtrue;
			}
			break;
		case WP_FG42:
		case WP_FG42SCOPE:
			if(newweapon == weapAlts[oldweapon])
				switchtime = 50;	// fast
			break;
#ifdef __EF__
		case WP_30CAL:
			if( newweapon == weapAlts[oldweapon] ) {
				vec3_t axis[3];

				switchtime = 0;

				VectorCopy( pml.forward, axis[0] );
				VectorCopy( pml.right, axis[2] );
				CrossProduct( axis[0], axis[2], axis[1] );
				AxisToAngles( axis, pm->pmext->mountedWeaponAngles );
			}
		case WP_30CAL_SET:
			if( newweapon == weapAlts[oldweapon] ) {
				switchtime = 0;
			}
			break;
        case WP_PTRS://Masteries, added the capabilitie to set the PTRS
			if( newweapon == weapAlts[oldweapon] ) {
				vec3_t axis[3];

				switchtime = 0;

				VectorCopy( pml.forward, axis[0] );
				VectorCopy( pml.right, axis[2] );
				CrossProduct( axis[0], axis[2], axis[1] );
				AxisToAngles( axis, pm->pmext->mountedWeaponAngles );
			}
		case WP_PTRS_SET:
			if( newweapon == weapAlts[oldweapon] ) {
				switchtime = 0;
			}
			break;
#endif //__EF__
		case WP_MOBILE_MG42:
			if( newweapon == weapAlts[oldweapon] ) {
				vec3_t axis[3];

				switchtime = 0;

				VectorCopy( pml.forward, axis[0] );
				VectorCopy( pml.right, axis[2] );
				CrossProduct( axis[0], axis[2], axis[1] );
				AxisToAngles( axis, pm->pmext->mountedWeaponAngles );
			}
		case WP_MOBILE_MG42_SET:
			if( newweapon == weapAlts[oldweapon] ) {
				switchtime = 0;
			}
			break;
		case WP_MORTAR:
			if( newweapon == weapAlts[oldweapon] ) {
				vec3_t axis[3];

				switchtime = 0;

				VectorCopy( pml.forward, axis[0] );
				VectorCopy( pml.right, axis[2] );
				CrossProduct( axis[0], axis[2], axis[1] );
				AxisToAngles( axis, pm->pmext->mountedWeaponAngles );
			}
			break;
		case WP_MORTAR_SET:
			if( newweapon == weapAlts[oldweapon] ) {
				switchtime = 0;
			}
			break;
	}

	// play an animation
	if( altSwitchAnim ) {
		if( (pm->ps->eFlags & EF_PRONE) || (newweapon == WP_MOBILE_MG42_SET && BG_CanSetupMobileMG42Here()) ) {
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_UNDO_ALT_WEAPON_MODE_PRONE, qfalse, qfalse );
#ifdef __EF__
		} else if( (pm->ps->eFlags & EF_PRONE) || ((newweapon == WP_30CAL_SET || newweapon == WP_PTRS_SET) && BG_CanSetupMobileMG42Here()) )  {
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_UNDO_ALT_WEAPON_MODE_PRONE, qfalse, qfalse );
#endif //__EF__
		} else {
			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_UNDO_ALT_WEAPON_MODE, qfalse, qfalse );
		}
	} else {
		BG_AnimScriptEvent( pm->ps,pm->character->animModelInfo,  ANIM_ET_DROPWEAPON, qfalse, qfalse );
	}

	if( reload ) {
		pm->ps->weaponstate = WEAPON_DROPPING_TORELOAD;
	} else {
		pm->ps->weaponstate = WEAPON_DROPPING;
	}

	pm->ps->weaponTime += switchtime;
}


/*
===============
PM_FinishWeaponChange
===============
*/
static void PM_FinishWeaponChange( void ) {
	int oldweapon, newweapon, switchtime;
	qboolean altSwitchAnim = qfalse;
	qboolean doSwitchAnim = qtrue;

	newweapon = pm->ps->nextWeapon;
//	pm->ps->nextWeapon = newweapon;
	if ( newweapon < WP_NONE || newweapon >= WP_NUM_WEAPONS ) {
		newweapon = WP_NONE;
	}

	if ( !( COM_BitCheck( pm->ps->weapons, newweapon ) ) ) {
		newweapon = WP_NONE;
	}

	oldweapon = pm->ps->weapon;

	pm->ps->weapon = newweapon;

	if( pm->ps->weaponstate == WEAPON_DROPPING_TORELOAD ) {
		pm->ps->weaponstate = WEAPON_RAISING_TORELOAD;
	} else {
		pm->ps->weaponstate = WEAPON_RAISING;
	}

	switch(newweapon)
	{
		// don't really care about anim since these weapons don't show in view.  
		// However, need to set the animspreadscale so they are initally at worst accuracy
		case WP_K43_SCOPE:
		case WP_GARAND_SCOPE:
		case WP_FG42SCOPE:
			pm->ps->aimSpreadScale = 255;			// initially at lowest accuracy
			pm->ps->aimSpreadScaleFloat = 255.0f;	// initially at lowest accuracy
			break;
		case WP_SILENCED_LUGER:
			pm->pmext->silencedSideArm |= 1;
			break;
		case WP_LUGER:
			pm->pmext->silencedSideArm &= ~1;
			break;
		case WP_SILENCED_COLT:
			pm->pmext->silencedSideArm |= 1;
			break;
		case WP_COLT:
			pm->pmext->silencedSideArm &= ~1;
			break;
		case WP_CARBINE:
			pm->pmext->silencedSideArm &= ~2;
			break;
		case WP_M7:
			pm->pmext->silencedSideArm |= 2;
			break;
		case WP_KAR98:
			pm->pmext->silencedSideArm &= ~2;
			break;
		case WP_GPG40:
			pm->pmext->silencedSideArm |= 2;
			break;
//		case WP_MEDIC_SYRINGE:
//			pm->pmext->silencedSideArm &= ~4;
//			break;
//		case WP_MEDIC_ADRENALINE:
//			pm->pmext->silencedSideArm |= 4;
//			break;
		default:
			break;
	}

	// doesn't happen too often (player switched weapons away then back very quickly)
	if(oldweapon == newweapon)
		return;

	// dropping/raising usually takes 1/4 sec.
	switchtime = 250;

	// sometimes different switch times for alt weapons
	switch(newweapon) {
		case WP_LUGER:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 0;
				//switchtime = 50;
				//switchtime = 1050;
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_SILENCED_LUGER:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 1190;
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_COLT:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 0;
				//switchtime = 1050;
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_SILENCED_COLT:
			if(newweapon == weapAlts[oldweapon]) {
				//switchtime = 1300;
				switchtime = 1190;
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_CARBINE:
			if(newweapon == weapAlts[oldweapon]) {
				//switchtime = 2000;
				if( pm->ps->ammoclip[ BG_FindAmmoForWeapon(oldweapon) ] ) {
					switchtime = 1347;
				} else {
					switchtime = 0;
					doSwitchAnim = qfalse;
				}
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_M7:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 2350;
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_KAR98:
			if(newweapon == weapAlts[oldweapon]) {
				//switchtime = 2000;
				if( pm->ps->ammoclip[ BG_FindAmmoForWeapon(oldweapon) ] ) {
					switchtime = 1347;
				} else {
					switchtime = 0;
					doSwitchAnim = qfalse;
				}
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_GPG40:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 2350;
				altSwitchAnim = qtrue ;
			}
			break;
		case WP_FG42:
		case WP_FG42SCOPE:
			if(newweapon == weapAlts[oldweapon])
				switchtime = 50;	// fast
			break;
		case WP_MOBILE_MG42:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 1722;
			}
			break;
		case WP_MOBILE_MG42_SET:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 1250;
			}
			break;
#ifdef __EF__
		case WP_30CAL:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 1722;
			}
			break;
		case WP_30CAL_SET:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 1250;
			}
			break;
        case WP_PTRS:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 1722;
			}
			break;
		case WP_PTRS_SET:
			if(newweapon == weapAlts[oldweapon]) {
				switchtime = 1250;
			}
			break;
#endif //__EF__
		case WP_MORTAR:
			if( newweapon == weapAlts[oldweapon] ) {
				switchtime = 1000;
				altSwitchAnim = qtrue;
			}
			break;
		case WP_MORTAR_SET:
			if( newweapon == weapAlts[oldweapon] ) {
				switchtime = 1667;
				altSwitchAnim = qtrue;
			}
			break;
	}

	pm->ps->weaponTime += switchtime;

	BG_UpdateConditionValue( pm->ps->clientNum, ANIM_COND_WEAPON, newweapon, qtrue );

	// play an animation
	if( doSwitchAnim ) {
		if( altSwitchAnim ) {
			if( (pm->ps->eFlags & EF_PRONE) || (newweapon == WP_MOBILE_MG42_SET && BG_CanSetupMobileMG42Here()) ) {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_DO_ALT_WEAPON_MODE_PRONE, qfalse, qfalse );
#ifdef __EF__
			} else if( (pm->ps->eFlags & EF_PRONE) || ((newweapon == WP_30CAL_SET || newweapon == WP_PTRS_SET) && BG_CanSetupMobileMG42Here()))  {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_DO_ALT_WEAPON_MODE_PRONE, qfalse, qfalse );
#endif //__EF__
			} else {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_DO_ALT_WEAPON_MODE, qfalse, qfalse );
			}
		} else {
			if( (pm->ps->eFlags & EF_PRONE) || (newweapon == WP_MOBILE_MG42_SET && BG_CanSetupMobileMG42Here()) ) {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RAISEWEAPONPRONE, qfalse, qfalse );
#ifdef __EF__
			} else if( (pm->ps->eFlags & EF_PRONE) || ((newweapon == WP_30CAL_SET || newweapon == WP_PTRS_SET) && BG_CanSetupMobileMG42Here()))  {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RAISEWEAPONPRONE, qfalse, qfalse );
#endif //__EF__
			} else {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_RAISEWEAPON, qfalse, qfalse );
			}
		}

		// alt weapon switch was played when switching away, just go into idle
		if(weapAlts[oldweapon] == newweapon) {
			PM_StartWeaponAnim(PM_AltSwitchToForWeapon(newweapon));
		} else {
			PM_StartWeaponAnim(PM_RaiseAnimForWeapon(newweapon));
		}
	}
}


/*
==============
PM_ReloadClip
==============
*/
static void PM_ReloadClip( int weapon ) {
	int ammoreserve, ammoclip, ammomove;

	ammoreserve = pm->ps->ammo[ BG_FindAmmoForWeapon(weapon)];
	ammoclip	= pm->ps->ammoclip[BG_FindClipForWeapon( weapon )];

	ammomove = GetAmmoTableData(weapon)->maxclip - ammoclip;

	if(ammoreserve < ammomove)
		ammomove = ammoreserve;

	if(ammomove) {
		pm->ps->ammo[ BG_FindAmmoForWeapon(weapon)] -= ammomove;
		pm->ps->ammoclip[BG_FindClipForWeapon(weapon)] += ammomove;
	}

	// reload akimbo stuff
	if( BG_IsAkimboWeapon( weapon ) ) {
		PM_ReloadClip( BG_AkimboSidearm( weapon ) );
	}
}

/*
==============
PM_FinishWeaponReload
==============
*/

static void PM_FinishWeaponReload(void) {
	PM_ReloadClip(pm->ps->weapon);			// move ammo into clip
	pm->ps->weaponstate = WEAPON_READY;		// ready to fire
	PM_StartWeaponAnim(PM_IdleAnimForWeapon(pm->ps->weapon));
}


/*
==============
PM_CheckforReload
==============
*/
void PM_CheckForReload( int weapon ) {
	qboolean autoreload;
	qboolean reloadRequested;
	int clipWeap, ammoWeap;

	if(pm->noWeapClips)	// no need to reload
		return;

	// GPG40 and M7 don't reload
	// kw: can't reload throwing knifes too
#ifdef QAGAME
	if( ((!g_entities[pm->ps->clientNum].r.svFlags & SVF_BOT) && (weapon == WP_GPG40 || weapon == WP_M7)) || weapon == WP_KNIFE)
#else //!QAGAME
	if( weapon == WP_GPG40 || weapon == WP_M7 || weapon == WP_KNIFE)
#endif //QAGAME
		return;

	// user is forcing a reload (manual reload)
	reloadRequested = (qboolean)(pm->cmd.wbuttons & WBUTTON_RELOAD);

	switch(pm->ps->weaponstate) {
		case WEAPON_RAISING:
		case WEAPON_RAISING_TORELOAD:
		case WEAPON_DROPPING:
		case WEAPON_DROPPING_TORELOAD:
		case WEAPON_READYING:
		case WEAPON_RELAXING:
		case WEAPON_RELOADING:
			return;
		default:
			break;
	}

	autoreload = pm->pmext->bAutoReload || !IS_AUTORELOAD_WEAPON(weapon);
	clipWeap = BG_FindClipForWeapon(weapon);
	ammoWeap = BG_FindAmmoForWeapon(weapon);

#ifdef __WEAPON_AIM__
	/*if (pm->ps->eFlags & EF_AIMING)
	{
		autoreload = qfalse;
		reloadRequested = qfalse;
	}*/
	if (pm->ps->eFlags & EF_AIMING && (autoreload && reloadRequested))
	{// Try un-aiming for auto-reloads...
		pm->ps->eFlags &= ~EF_AIMING;
#ifdef GAMEDLL
		g_entities[pm->ps->clientNum].s.eFlags &= ~EF_AIMING;

#ifdef __NPC__
		if (pm->isNPC)
			g_entities[pm->ps->clientNum].NPC_client->ps.eFlags &= ~EF_AIMING;
		else
#endif //__NPC__
			g_entities[pm->ps->clientNum].client->ps.eFlags &= ~EF_AIMING;

#endif //GAMEDLL
#ifdef CGAMEDLL
		cg_entities[pm->ps->clientNum].currentState.eFlags &= ~EF_AIMING;
#endif //CGAMEDLL
	}
#endif //__WEAPON_AIM__

	if (pm->ps->eFlags & EF_ZOOMING && (autoreload && reloadRequested))
	{// Try un-zooming for auto-reloads...
		pm->ps->eFlags &= ~EF_ZOOMING;
#ifdef GAMEDLL
		g_entities[pm->ps->clientNum].s.eFlags &= ~EF_ZOOMING;

#ifdef __NPC__
		if (pm->isNPC)
			g_entities[pm->ps->clientNum].NPC_client->ps.eFlags &= ~EF_ZOOMING;
		else
#endif //__NPC__
			g_entities[pm->ps->clientNum].client->ps.eFlags &= ~EF_ZOOMING;

#endif //GAMEDLL
#ifdef CGAMEDLL
		cg_entities[pm->ps->clientNum].currentState.eFlags &= ~EF_ZOOMING;
#endif //CGAMEDLL
	}

	switch( weapon ) {
		case WP_FG42SCOPE:
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
		case WP_GPG40: // UQ1: Added for bots only!
		case WP_M7: // UQ1: Added for bots only!
			if( reloadRequested && pm->ps->ammo[ammoWeap] && pm->ps->ammoclip[clipWeap] < GetAmmoTableData(weapon)->maxclip) {
				PM_BeginWeaponChange( weapon, weapAlts[weapon], !(pm->ps->ammo[ammoWeap]) ? qfalse : qtrue );
			}
			return;
		default:
			break;
	}

	if( pm->ps->weaponTime <= 0 ) {
		qboolean doReload = qfalse;

		if( reloadRequested ) {
			if( pm->ps->ammo[ammoWeap] ) {
				if( pm->ps->ammoclip[clipWeap] < GetAmmoTableData(weapon)->maxclip ) {
					doReload = qtrue;
				}

				// akimbo should also check other weapon status
				if( BG_IsAkimboWeapon( weapon ) ) {
					if( pm->ps->ammoclip[BG_FindClipForWeapon(BG_AkimboSidearm(weapon))] < GetAmmoTableData(BG_FindClipForWeapon(BG_AkimboSidearm(weapon)))->maxclip )
						doReload = qtrue;
				}
			}
		} else if( autoreload ) {
			if(	!pm->ps->ammoclip[clipWeap] && pm->ps->ammo[ammoWeap] ) {
				if( BG_IsAkimboWeapon( weapon ) ) {
					if( !pm->ps->ammoclip[BG_FindClipForWeapon(BG_AkimboSidearm(weapon))] )
						doReload = qtrue;
				} /*else if( BG_IsAkimboSideArm(weapon, pm->ps) ) {
					if( !pm->ps->ammoclip[BG_FindClipForWeapon(BG_AkimboForSideArm(weapon))] )
						doReload = qtrue;
				}*/ else
					doReload = qtrue;
			}
		}

		if( doReload )
			PM_BeginWeaponReload( weapon );
	}
}

/*
==============
PM_SwitchIfEmpty
==============
*/
static void PM_SwitchIfEmpty(void)
{
	// weapon from here down will be a thrown explosive
	if(	pm->ps->weapon != WP_GRENADE_LAUNCHER &&
		pm->ps->weapon != WP_GRENADE_PINEAPPLE &&
		pm->ps->weapon != WP_DYNAMITE &&
		pm->ps->weapon != WP_SMOKE_BOMB &&
		pm->ps->weapon != WP_LANDMINE &&
		pm->ps->weapon != WP_SATCHEL_DET) {
		return;
	}

	if(pm->ps->ammoclip[ BG_FindClipForWeapon(pm->ps->weapon)])	// still got ammo in clip
		return;

	if(pm->ps->ammo[ BG_FindAmmoForWeapon(pm->ps->weapon)])	// still got ammo in reserve
		return;

	// If this was the last one, remove the weapon and switch away before the player tries to fire next

	// NOTE: giving grenade ammo to a player will re-give him the weapon (if you do it through add_ammo())
	switch(pm->ps->weapon) {
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_DYNAMITE:
			COM_BitClear( pm->ps->weapons, pm->ps->weapon);
	}

	PM_AddEvent( EV_NOAMMO );
}


/*
==============
PM_WeaponUseAmmo
	accounts for clips being used/not used
==============
*/
void PM_WeaponUseAmmo( int wp, int amount ) {
	int takeweapon;

	if(pm->noWeapClips)
		pm->ps->ammo[ BG_FindAmmoForWeapon(wp)] -= amount;
	else {
		takeweapon = BG_FindClipForWeapon(wp);

		if( BG_IsAkimboWeapon( wp ) ) {
			if( !BG_AkimboFireSequence( wp, pm->ps->ammoclip[BG_FindClipForWeapon(wp)], pm->ps->ammoclip[BG_FindClipForWeapon(BG_AkimboSidearm(wp))] ) )
				takeweapon = BG_AkimboSidearm(wp);
		}

		pm->ps->ammoclip[takeweapon] -= amount;
	}
}


/*
==============
PM_WeaponAmmoAvailable
	accounts for clips being used/not used
==============
*/
int PM_WeaponAmmoAvailable( int wp ) {
	int takeweapon;

	if(pm->noWeapClips)
		return pm->ps->ammo[ BG_FindAmmoForWeapon(wp)];
	else {
		//return pm->ps->ammoclip[BG_FindClipForWeapon( wp )];
		takeweapon = BG_FindClipForWeapon( wp );
		
		if( BG_IsAkimboWeapon( wp ) ) {
			if( !BG_AkimboFireSequence( wp, pm->ps->ammoclip[BG_FindClipForWeapon(wp)], pm->ps->ammoclip[BG_FindClipForWeapon(BG_AkimboSidearm(wp))] ) )
				takeweapon = BG_AkimboSidearm(wp);
		}

		return pm->ps->ammoclip[takeweapon];
	}
}

/*
==============
PM_WeaponClipEmpty
	accounts for clips being used/not used
==============
*/
int PM_WeaponClipEmpty( int wp ) {
	
	// kw: knives now use ammo as max throwing knifes indicator.
	//     This hack fixes throwing knife double sound/animation 
	//     when it's clip is empty.
	if( wp == WP_KNIFE ) {
		return 0;
	}
	
	if(pm->noWeapClips) {
		if(!(pm->ps->ammo[ BG_FindAmmoForWeapon(wp)]))
			return 1;
	}else{
		if(!(pm->ps->ammoclip[BG_FindClipForWeapon(wp)]))
			return 1;
	}

	return 0;
}


/*
==============
PM_CoolWeapons
==============
*/
void PM_CoolWeapons( void ) {
	int wp, maxHeat;

	for(wp=0;wp<WP_NUM_WEAPONS;wp++) {
		// if you have the weapon
		if( COM_BitCheck( pm->ps->weapons, wp) ) {
			// and it's hot
			if(pm->ps->weapHeat[wp]) {
				if( pm->skill[SK_HEAVY_WEAPONS] >= 2 && pm->ps->stats[STAT_PLAYER_CLASS] == PC_SOLDIER ) {
					pm->ps->weapHeat[wp] -= ((float)GetAmmoTableData(wp)->coolRate * 2.f * pml.frametime);
				} else {
					pm->ps->weapHeat[wp] -= ((float)GetAmmoTableData(wp)->coolRate * pml.frametime);
				}

				if(pm->ps->weapHeat[wp] < 0)
					pm->ps->weapHeat[wp] = 0;

			}
		}
	}

	// a weapon is currently selected, convert current heat value to 0-255 range for client transmission
	if(pm->ps->weapon) {
		if( pm->ps->persistant[PERS_HWEAPON_USE] || pm->ps->eFlags & EF_MOUNTEDTANK ) {
			// rain - floor to prevent 8-bit wrap
			pm->ps->curWeapHeat = floor( ( ( (float)pm->ps->weapHeat[WP_DUMMY_MG42] / MAX_MG42_HEAT) ) * 255.0f);
		} else {
			// rain - #172 - don't divide by 0
			maxHeat = GetAmmoTableData(pm->ps->weapon)->maxHeat;
			
			// rain - floor to prevent 8-bit wrap
			if (maxHeat != 0)
				pm->ps->curWeapHeat = floor( ( ( (float)pm->ps->weapHeat[pm->ps->weapon] / (float)maxHeat) ) * 255.0f);
			else
				pm->ps->curWeapHeat = 0; 
		}

//		if(pm->ps->weapHeat[pm->ps->weapon])
//			Com_Printf("pm heat: %d, %d\n", pm->ps->weapHeat[pm->ps->weapon], pm->ps->curWeapHeat);
	}

}

/*
==============
PM_AdjustAimSpreadScale
==============
*/
//#define	AIMSPREAD_DECREASE_RATE		300.0f
#define	AIMSPREAD_DECREASE_RATE		200.0f		// (SA) when I made the increase/decrease floats (so slower weapon recover could happen for scoped weaps) the average rate increased significantly
#define	AIMSPREAD_INCREASE_RATE		800.0f
#define	AIMSPREAD_VIEWRATE_MIN		30.0f		// degrees per second
#define	AIMSPREAD_VIEWRATE_RANGE	120.0f		// degrees per second

void PM_AdjustAimSpreadScale( void ) {
//	int		increase, decrease, i;
	int		i;
	float	increase, decrease;		// (SA) was losing lots of precision on slower weapons (scoped)
	float	viewchange, cmdTime, wpnScale;

	// all weapons are very inaccurate in zoomed mode
	if(pm->ps->eFlags & EF_ZOOMING) {
		pm->ps->aimSpreadScale = 255;
		pm->ps->aimSpreadScaleFloat = 255;
		return;
	}

	cmdTime = (float)(pm->cmd.serverTime - pm->oldcmd.serverTime) / 1000.0;

	wpnScale = 0.0f;
	switch(pm->ps->weapon) {
	case WP_LUGER:
	case WP_SILENCED_LUGER:
	case WP_AKIMBO_LUGER:
	case WP_AKIMBO_SILENCEDLUGER:
// rain - luger and akimbo are supposed to be balanced
//		wpnScale = 0.5f;
//		break;
	case WP_COLT:
	case WP_SILENCED_COLT:
	case WP_AKIMBO_COLT:
	case WP_AKIMBO_SILENCEDCOLT:
		wpnScale = 0.4f;		// doesn't fire as fast, but easier to handle than luger
		break;
	case WP_MP40:
		wpnScale = 0.6f;		// 2 handed, but not as long as mauser, so harder to keep aim
		break;
	case WP_GARAND:
		wpnScale = 0.5f;
		break;
	case WP_K43_SCOPE:
	case WP_GARAND_SCOPE:
	case WP_FG42SCOPE:
		if( pm->skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] >= 3 ) {
			wpnScale = 5.f;
		} else {
			wpnScale = 10.f;
		}
		break;
	case WP_K43:
		wpnScale = 0.5f;
		break;
	case WP_MOBILE_MG42:
	case WP_MOBILE_MG42_SET:
#ifdef __EF__
		case WP_30CAL:
		case WP_30CAL_SET:
		case WP_PTRS://Masteries, I have some dudes about this line...
		case WP_PTRS_SET:
#endif //__EF__
		wpnScale = 0.9f;
		break;
	case WP_FG42:
		wpnScale = 0.6f;
		break;
	case WP_THOMPSON:
		wpnScale = 0.6f;
		break;
#ifdef __EF__
		case WP_PPSH:
		case WP_STG44:
		case WP_PPS:
		case WP_SVT40:
#endif //__EF__
	case WP_STEN:
		wpnScale = 0.6f;
		break;
	case WP_KAR98:
	case WP_CARBINE:
		wpnScale = 0.5f;
		break;
	}

	if (wpnScale) {

		// JPW NERVE crouched players recover faster (mostly useful for snipers)
		if( pm->ps->eFlags & EF_CROUCHING || pm->ps->eFlags & EF_PRONE ) {
			wpnScale *= 0.5;
		}

		decrease = (cmdTime * AIMSPREAD_DECREASE_RATE) / wpnScale;

		viewchange = 0;
		// take player movement into account (even if only for the scoped weapons)
		// TODO: also check for jump/crouch and adjust accordingly
		if(BG_IsScopedWeapon(pm->ps->weapon)) {
			for(i = 0; i < 2; i++) {
				viewchange += fabs(pm->ps->velocity[i]);
			}
#ifdef __WEAPON_AIM__
		} else if (pm->ps->eFlags & EF_AIMING) {
			for(i = 0; i < 2; i++) {
				viewchange += fabs(pm->ps->velocity[i]);
			}
#endif //__WEAPON_AIM__
		} else {
			// take player view rotation into account
			for( i = 0; i < 2; i++ ) {
				// tjw: angles need to be normalized since they're
				//      often > 65535.  
				//      FIXME: Maybe this normalization should in Pmove()?
				// tjw: need to use AngleSubtract() instead of just minus.
				//viewchange += fabs( SHORT2ANGLE(pm->cmd.angles[i]) - SHORT2ANGLE(pm->oldcmd.angles[i]) );
				viewchange += fabs(AngleSubtract(
					SHORT2ANGLE(AngleNormalizeInt(pm->cmd.angles[i])),
					SHORT2ANGLE(AngleNormalizeInt(pm->oldcmd.angles[i]))));
			}
		}

		viewchange = (float)viewchange / cmdTime;	// convert into this movement for a second
		viewchange -= AIMSPREAD_VIEWRATE_MIN / wpnScale;
		if (viewchange <= 0) {
			viewchange = 0;
		} else if( viewchange > (AIMSPREAD_VIEWRATE_RANGE / wpnScale) ) {
			viewchange = AIMSPREAD_VIEWRATE_RANGE / wpnScale;
		}

		// now give us a scale from 0.0 to 1.0 to apply the spread increase
		viewchange = viewchange / (float)(AIMSPREAD_VIEWRATE_RANGE / wpnScale);

		increase = (int)(cmdTime * viewchange * AIMSPREAD_INCREASE_RATE);
	} else {
		increase = 0;
		decrease = AIMSPREAD_DECREASE_RATE;
	}

	// update the aimSpreadScale
	pm->ps->aimSpreadScaleFloat += (increase - decrease);
	if (pm->ps->aimSpreadScaleFloat < 0) pm->ps->aimSpreadScaleFloat = 0;
	if (pm->ps->aimSpreadScaleFloat > 255) pm->ps->aimSpreadScaleFloat = 255;

	pm->ps->aimSpreadScale = (int)pm->ps->aimSpreadScaleFloat;	// update the int for the client
}

#define weaponstateFiring (pm->ps->weaponstate == WEAPON_FIRING || pm->ps->weaponstate == WEAPON_FIRINGALT)

#define GRENADE_DELAY	250

/*
==============
PM_Weapon

Generates weapon events and modifes the weapon counter
==============
*/

#define VENOM_LOW_IDLE	WEAP_IDLE1
#define VENOM_HI_IDLE	WEAP_IDLE2
#define VENOM_RAISE		WEAP_ATTACK1
#define VENOM_ATTACK	WEAP_ATTACK2
#define VENOM_LOWER		WEAP_ATTACK_LASTSHOT

//#define DO_WEAPON_DBG 1

static void PM_Weapon( void ) {
	int			addTime = 0; // TTimo: init
	int			ammoNeeded;
	qboolean	delayedFire;	//----(SA)  true if the delay time has just expired and this is the frame to send the fire event
	int			aimSpreadScaleAdd;
	int			weapattackanim;
	qboolean	akimboFire;
#ifdef DO_WEAPON_DBG
	static int weaponstate_last = -1;
#endif

	// don't allow attack until all buttons are up
	if ( pm->ps->pm_flags & PMF_RESPAWNED ) {
		return;
	}

	// ignore if spectator
	if ( pm->ps->persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
		return;
	}

	// check for dead player
	if ( pm->ps->stats[STAT_HEALTH] <= 0 ) {
		if(!(pm->ps->pm_flags & PMF_LIMBO)) {
			PM_CoolWeapons();
		}

		//pm->ps->weapon = WP_NONE;
		return;
	}

	//%	if( pm->ps->eFlags & EF_PRONE_MOVING )
	//%		return;

	// special mounted mg42 handling
	switch(pm->ps->persistant[PERS_HWEAPON_USE]) {
		case 1:
//			PM_CoolWeapons(); // Gordon: Arnout says this is how it's wanted ( bleugh ) no cooldown on weaps while using mg42, but need to update heat on mg42 itself
			if( pm->ps->weapHeat[WP_DUMMY_MG42] ) {
				pm->ps->weapHeat[WP_DUMMY_MG42] -= (300.f * pml.frametime);

				if( pm->ps->weapHeat[WP_DUMMY_MG42] < 0 )
					pm->ps->weapHeat[WP_DUMMY_MG42] = 0;

				// rain - floor() to prevent 8-bit wrap
				pm->ps->curWeapHeat = floor( ( ( (float)pm->ps->weapHeat[WP_DUMMY_MG42] / MAX_MG42_HEAT) ) * 255.0f);
			}

			if( pm->ps->weaponTime > 0 ) {
				pm->ps->weaponTime -= pml.msec;
				if (pm->ps->weaponTime <= 0) {
					if ( !(pm->cmd.buttons & BUTTON_ATTACK) ) {
						pm->ps->weaponTime = 0;
						return;
					}
				} else {
					return;
				}
			}

			if( pm->cmd.buttons & BUTTON_ATTACK ) {
//				qboolean is_ptrs = qfalse;

				if(PM_IsSinglePlayerGame()) {
					pm->ps->weapHeat[WP_DUMMY_MG42] += MG42_RATE_OF_FIRE_SP;
				} else {
					pm->ps->weapHeat[WP_DUMMY_MG42] += MG42_RATE_OF_FIRE_MP;
				}

				PM_AddEvent( EV_FIRE_WEAPON_MG42 );

				if(PM_IsSinglePlayerGame()) {
					pm->ps->weaponTime += MG42_RATE_OF_FIRE_SP;
				} else {
					pm->ps->weaponTime += MG42_RATE_OF_FIRE_MP;
				}

				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
				pm->ps->viewlocked = 2;		// this enable screen jitter when firing

#ifdef __SCREW_THIS__
#ifdef CGAMEDLL
				if (pm->ps->eFlags & EF_MG42_ACTIVE)
				{// find the mg42 we're attached to
					int num;

					for ( num = 0 ; num < cg.snap->numEntities ; num++ ) {
						centity_t *mg42 = &cg_entities[ cg.snap->entities[ num ].number ];
						
						if( mg42->currentState.eType == ET_MG42_BARREL && mg42->currentState.otherEntityNum == pm->ps->clientNum ) 
						{// found it, is it a ptrs?
							if (mg42->currentState.density == 2)
								is_ptrs = qtrue;

							break;
						}
					}

					if( is_ptrs )
					{// Can only fire once every 2 secs...
						pm->ps->weaponTime = MAX_MG42_HEAT;	// cap heat to max
						PM_AddEvent( EV_WEAP_OVERHEAT );
						pm->ps->weaponTime = 2000;		// force "heat recovery minimum" to 2 sec right now
						pm->ps->weapHeat[WP_DUMMY_MG42] = MAX_MG42_HEAT;
					}
					else if( pm->ps->weapHeat[WP_DUMMY_MG42] >= MAX_MG42_HEAT ) 
					{
						pm->ps->weaponTime = MAX_MG42_HEAT;	// cap heat to max
						PM_AddEvent( EV_WEAP_OVERHEAT );
						pm->ps->weaponTime = 2000;		// force "heat recovery minimum" to 2 sec right now
					}	
				}
				else if( pm->ps->weapHeat[WP_DUMMY_MG42] >= MAX_MG42_HEAT ) {
					pm->ps->weaponTime = MAX_MG42_HEAT;	// cap heat to max
					PM_AddEvent( EV_WEAP_OVERHEAT );
					pm->ps->weaponTime = 2000;		// force "heat recovery minimum" to 2 sec right now
				}	
#endif //CGAMEDLL
#ifdef QAGAME
				if (pm->ps->eFlags & EF_MG42_ACTIVE)
				{// find the mg42 we're attached to
					int num;

					for ( num = MAX_CLIENTS ; num < MAX_GENTITIES ; num++ ) {
						gentity_t *mg42 = &g_entities[ num ];
						
						if( mg42->s.eType == ET_MG42_BARREL && mg42->s.otherEntityNum == pm->ps->clientNum ) 
						{// found it, is it a ptrs?
							if (mg42->s.density == 2)
								is_ptrs = qtrue;

							break;
						}
					}

					if( is_ptrs )
					{// Can only fire once every 2 secs...
						pm->ps->weaponTime = MAX_MG42_HEAT;	// cap heat to max
						PM_AddEvent( EV_WEAP_OVERHEAT );
						pm->ps->weaponTime = 2000;		// force "heat recovery minimum" to 2 sec right now
						pm->ps->weapHeat[WP_DUMMY_MG42] = MAX_MG42_HEAT;
					}
					else if( pm->ps->weapHeat[WP_DUMMY_MG42] >= MAX_MG42_HEAT ) 
					{
						pm->ps->weaponTime = MAX_MG42_HEAT;	// cap heat to max
						PM_AddEvent( EV_WEAP_OVERHEAT );
						pm->ps->weaponTime = 2000;		// force "heat recovery minimum" to 2 sec right now
					}	
				}
				else if( pm->ps->weapHeat[WP_DUMMY_MG42] >= MAX_MG42_HEAT ) {
					pm->ps->weaponTime = MAX_MG42_HEAT;	// cap heat to max
					PM_AddEvent( EV_WEAP_OVERHEAT );
					pm->ps->weaponTime = 2000;		// force "heat recovery minimum" to 2 sec right now
				}		
#endif //QAGAME
#else //!__SCREW_THIS__
				// UQ1: Bah! Can't be botherred with this mounted PTRS right now... Too much work...
				if( pm->ps->weapHeat[WP_DUMMY_MG42] >= MAX_MG42_HEAT ) {
					pm->ps->weaponTime = MAX_MG42_HEAT;	// cap heat to max
					PM_AddEvent( EV_WEAP_OVERHEAT );
					pm->ps->weaponTime = 2000;		// force "heat recovery minimum" to 2 sec right now
				}
#endif //__SCREW_THIS__
			}
			return;
		case 2:
			if( pm->ps->weaponTime > 0 ) {
				pm->ps->weaponTime -= pml.msec;
				if (pm->ps->weaponTime <= 0) {
					if ( !(pm->cmd.buttons & BUTTON_ATTACK) ) {
						pm->ps->weaponTime = 0;
						return;
					}
				} else {
					return;
				}
			}

			if( pm->cmd.buttons & BUTTON_ATTACK ) {
				PM_AddEvent( EV_FIRE_WEAPON_AAGUN );

				pm->ps->weaponTime += AAGUN_RATE_OF_FIRE;

				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
//				pm->ps->viewlocked = 2;		// this enable screen jitter when firing		
			}
			return;
	}

	if( pm->ps->eFlags & EF_MOUNTEDTANK ) {
//		PM_CoolWeapons(); // Gordon: Arnout says this is how it's wanted ( bleugh ) no cooldown on weaps while using mg42, but need to update heat on mg42 itself
		if( pm->ps->weapHeat[WP_DUMMY_MG42] ) {
			pm->ps->weapHeat[WP_DUMMY_MG42] -= (300.f * pml.frametime);

			if( pm->ps->weapHeat[WP_DUMMY_MG42] < 0 )
				pm->ps->weapHeat[WP_DUMMY_MG42] = 0;

			// rain - floor() to prevent 8-bit wrap
			pm->ps->curWeapHeat = floor(((float)pm->ps->weapHeat[WP_DUMMY_MG42] / MAX_MG42_HEAT) * 255.0f);
		}

		if( pm->ps->weaponTime > 0 ) {
			pm->ps->weaponTime -= pml.msec;
			if (pm->ps->weaponTime <= 0) {
				if ( !(pm->cmd.buttons & BUTTON_ATTACK) ) {
					pm->ps->weaponTime = 0;
					return;
				}
			} else {
				return;
			}
		}



		if( pm->cmd.buttons & BUTTON_ATTACK ) {
			pm->ps->weapHeat[WP_DUMMY_MG42] += MG42_RATE_OF_FIRE_MP;

			PM_AddEvent( EV_FIRE_WEAPON_MOUNTEDMG42 );

			pm->ps->weaponTime += MG42_RATE_OF_FIRE_MP;

			BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
			//pm->ps->viewlocked = 2;		// this enable screen jitter when firing

			if( pm->ps->weapHeat[WP_DUMMY_MG42] >= MAX_MG42_HEAT ) {
				pm->ps->weaponTime = MAX_MG42_HEAT;	// cap heat to max
				PM_AddEvent( EV_WEAP_OVERHEAT );
				pm->ps->weaponTime = 2000;		// force "heat recovery minimum" to 2 sec right now
			}
	
		}
		return;
	}

#ifdef __VEHICLES__
	if( pm->ps->eFlags & EF_VEHICLE ) 
	{ 
		qboolean can_altFire = qtrue;
		qboolean can_Fire = qtrue;

		if( pm->ps->weaponTime > 0 ) 
		{ 
			pm->ps->weaponTime -= pml.msec;

			if (pm->ps->weaponTime <= 0) { 
				if ( !(pm->cmd.wbuttons & WBUTTON_ATTACK2) ) {
					pm->ps->weaponTime = 0;
					can_altFire = qfalse;
				}
			} else {
				can_altFire = qfalse;
			}
		}

		if( can_altFire 
			&& pm->ps->weapHeat[WP_DUMMY_MG42] > 0 ) 
		{ 
			pm->ps->weapHeat[WP_DUMMY_MG42] -= pml.msec;

			if (pm->ps->weapHeat[WP_DUMMY_MG42] <= 0) 
			{ 
				pm->ps->weapHeat[WP_DUMMY_MG42] = 0;
			} 
		}

		// rain - floor() to prevent 8-bit wrap
		pm->ps->curWeapHeat = floor(((float)pm->ps->weapHeat[WP_DUMMY_MG42] / MAX_MG42_HEAT) * 255.0f);

		//Com_Printf("Current WP_DUMMY_MG42 heat is %i (%i%).\n", pm->ps->weapHeat[WP_DUMMY_MG42], pm->ps->curWeapHeat);

   //Masteries, artillery doesn´t have machinegun
		if (pm->ps->persistant[PERS_TANKUSED] == VEHICLE_TYPE_AXIS_APC
			|| pm->ps->persistant[PERS_TANKUSED] == VEHICLE_TYPE_ALLIED_APC)
		{// APC's can't fire??? -- Maybe we should add an mg42 turret?

		}
		else if( can_altFire
			&& (pm->cmd.wbuttons & WBUTTON_ATTACK2)
			&& pm->ps->persistant[PERS_TANKUSED] != VEHICLE_TYPE_FLAK88
			&& pm->ps->persistant[PERS_TANKUSED] != VEHICLE_TYPE_PAK ) 
		{  // COAX
			PM_AddEvent( EV_TANK_MG_FIRE );
			pm->ps->weapHeat[WP_DUMMY_MG42] += 66;
			pm->ps->weaponTime = 66;

			// rain - floor() to prevent 8-bit wrap
			pm->ps->curWeapHeat = floor(((float)pm->ps->weapHeat[WP_DUMMY_MG42] / MAX_MG42_HEAT) * 255.0f);

			if( pm->ps->weapHeat[WP_DUMMY_MG42] >= MAX_MG42_HEAT ) 
			{
				pm->ps->weapHeat[WP_DUMMY_MG42] = MAX_MG42_HEAT;
				pm->ps->weaponTime = MAX_MG42_HEAT;	// cap heat to max
				PM_AddEvent( EV_WEAP_OVERHEAT );
			}
		}

		if( pm->ps->grenadeTimeLeft > 0 ) { 
			pm->ps->grenadeTimeLeft -= pml.msec;
			if (pm->ps->grenadeTimeLeft <= 0) { 
				if ( !(pm->cmd.buttons & BUTTON_ATTACK) ) {
					pm->ps->grenadeTimeLeft = 0;
					can_Fire = qfalse;
				}
			} else {
				can_Fire = qfalse;
			}
		}

		if( can_Fire 
			&& (pm->cmd.buttons & BUTTON_ATTACK)
			&& pm->ps->grenadeTimeLeft <= 0 ) 
		{  // MAINGUN
			if (pm->ps->persistant[PERS_TANKUSED] == VEHICLE_TYPE_AXIS_APC
				|| pm->ps->persistant[PERS_TANKUSED] == VEHICLE_TYPE_ALLIED_APC)
			{// APC's can't fire??? -- Maybe we should add an mg42 turret?

			}
			else if (pm->ps->persistant[PERS_TANKUSED] == VEHICLE_TYPE_FLAMETANK)
			{
				PM_AddEvent( EV_FLAME_TANK_FIRE );
				pm->ps->grenadeTimeLeft += 100 /*DELAY_LOW*/;	// WILL PREVENT MAINGUN FROM SHOOTING, WEIRD, @TODO
			}
			else
			{   
				if(pm->ps->persistant[PERS_TANKUSED] == VEHICLE_TYPE_PANZER2)//Masteries, panzer2 has an automatic PTRS style gun instead a normal cannon
				{
				PM_AddEvent( EV_TANK_PTRS_FIRE );
				pm->ps->grenadeTimeLeft += 400;//is an automatic gun
				}
				else//normal cannon
				{
				PM_AddEvent( EV_TANK_FIRE );
				pm->ps->grenadeTimeLeft += 4500;	// WILL PREVENT MAINGUN FROM SHOOTING, WEIRD, @TODO
				}
			}
		}

		return;
	} // EF_VEHICLE
#endif //__VEHICLES__
#ifdef CGAMEDLL
	else
	{
#ifdef __BOT__
		if ( !(pm->cmd.wbuttons & WBUTTON_ATTACK2) 
			&& cg.current_commander_crosshair_spin 
			&& pm->ps->ping >= 0 )
		{// Ready to acknowledge BOT fireteam orders... Do it!
			CG_BOT_FireTeam_Orders_Finalize();
		}
		else 
#endif //__BOT__
		if ( pm->cmd.wbuttons & WBUTTON_ATTACK2 )
		{
#ifdef __BOT__
			if ( pm->ps->ping >= 0 && CG_BOT_FireTeam_Orders())
			{// Ignore if giving orders...
				
			}
			else
#endif //__BOT__
			if (pm->ps->clientNum == cg.clientNum && cg_entities[cg.clientNum].last_altfire_time < cg.time - 1000)
			{
				CG_AltWeapon_f();
				cg_entities[cg.clientNum].last_altfire_time = cg.time;
			}
		}
	}
#endif //CGAMEDLL

	pm->watertype = 0;

	if( BG_IsAkimboWeapon( pm->ps->weapon ) )
		akimboFire = BG_AkimboFireSequence( pm->ps->weapon, pm->ps->ammoclip[BG_FindClipForWeapon(pm->ps->weapon)], pm->ps->ammoclip[BG_FindClipForWeapon(BG_AkimboSidearm(pm->ps->weapon))] );
	else
		akimboFire = qfalse;

  // TTimo
  // https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=416
#ifdef DO_WEAPON_DBG
  if (pm->ps->weaponstate != weaponstate_last) {
    #ifdef CGAMEDLL
    Com_Printf(" CGAMEDLL\n");
    #else
    Com_Printf("!CGAMEDLL\n");
    #endif
		switch(pm->ps->weaponstate) {
			case WEAPON_READY:
				Com_Printf(" -- WEAPON_READY\n");
				break;
			case WEAPON_RAISING:
				Com_Printf(" -- WEAPON_RAISING\n");
				break;
			case WEAPON_RAISING_TORELOAD:
				Com_Printf(" -- WEAPON_RAISING_TORELOAD\n");
				break;
			case WEAPON_DROPPING:
				Com_Printf(" -- WEAPON_DROPPING\n");
				break;
			case WEAPON_READYING:
				Com_Printf(" -- WEAPON_READYING\n");
				break;
			case WEAPON_RELAXING:
				Com_Printf(" -- WEAPON_RELAXING\n");
				break;
			case WEAPON_DROPPING_TORELOAD:
				Com_Printf(" -- WEAPON_DROPPING_TORELOAD\n");
				break;
			case WEAPON_FIRING:
				Com_Printf(" -- WEAPON_FIRING\n");
				break;
			case WEAPON_FIRINGALT:
				Com_Printf(" -- WEAPON_FIRINGALT\n");
				break;
			case WEAPON_RELOADING:
				Com_Printf(" -- WEAPON_RELOADING\n");
				break;
		}
    weaponstate_last = pm->ps->weaponstate;
  }
#endif

	// weapon cool down
	PM_CoolWeapons();

	// check for weapon recoil
	// do the recoil before setting the values, that way it will be shown next frame and not this
	if( pm->pmext->weapRecoilTime ) {
		vec3_t muzzlebounce;
		int i, deltaTime;

		deltaTime = pm->cmd.serverTime - pm->pmext->weapRecoilTime;
		VectorCopy( pm->ps->viewangles, muzzlebounce );

		if( deltaTime > pm->pmext->weapRecoilDuration )
			deltaTime = pm->pmext->weapRecoilDuration;

		for( i = pm->pmext->lastRecoilDeltaTime; i < deltaTime; i += 15 ) {
			if( pm->pmext->weapRecoilPitch > 0.f ) {
				muzzlebounce[PITCH] -= 2*pm->pmext->weapRecoilPitch*cos(2.5*(i)/pm->pmext->weapRecoilDuration);
				muzzlebounce[PITCH] -= 0.25*random()*(1.0f-(i)/pm->pmext->weapRecoilDuration);
			}

			if( pm->pmext->weapRecoilYaw > 0.f ) {
				muzzlebounce[YAW] += 0.5*pm->pmext->weapRecoilYaw*cos(1.0-(i)*3/pm->pmext->weapRecoilDuration);
				muzzlebounce[YAW] += 0.5*crandom()*(1.0f-(i)/pm->pmext->weapRecoilDuration);
			}
		}

		// set the delta angle
		for( i = 0; i < 3; i++ ) {
			int cmdAngle;

			cmdAngle = ANGLE2SHORT(muzzlebounce[i]);
			pm->ps->delta_angles[i] = cmdAngle - pm->cmd.angles[i];
		}
		VectorCopy( muzzlebounce, pm->ps->viewangles );

		if( deltaTime == pm->pmext->weapRecoilDuration ) {
			pm->pmext->weapRecoilTime = 0;
			pm->pmext->lastRecoilDeltaTime = 0;
		} else {
			pm->pmext->lastRecoilDeltaTime = deltaTime;
		}
	}

	delayedFire = qfalse;

	if(pm->ps->weapon == WP_GRENADE_LAUNCHER || pm->ps->weapon == WP_GRENADE_PINEAPPLE || pm->ps->weapon == WP_DYNAMITE || pm->ps->weapon == WP_SMOKE_BOMB) {
		if( pm->ps->grenadeTimeLeft > 0 ) {
			qboolean forcethrow = qfalse;

			if( pm->ps->weapon == WP_DYNAMITE ) {
				pm->ps->grenadeTimeLeft += pml.msec;

				// JPW NERVE -- in multiplayer, dynamite becomes strategic, so start timer @ 30 seconds
				if (pm->ps->grenadeTimeLeft < 5000)
					pm->ps->grenadeTimeLeft = 5000;

			} else {
				pm->ps->grenadeTimeLeft -= pml.msec;

				if( pm->ps->grenadeTimeLeft <= 100 ) { // give two frames advance notice so there's time to launch and detonate
					forcethrow = qtrue;

					pm->ps->grenadeTimeLeft = 100;
				}
			}

			if( !(pm->cmd.buttons & BUTTON_ATTACK) || forcethrow || pm->ps->eFlags & EF_PRONE_MOVING ) {
				if( pm->ps->weaponDelay == GetAmmoTableData(pm->ps->weapon)->fireDelayTime || forcethrow) {
					// released fire button.  Fire!!!
#ifdef __OLD_GRENADE_TOSS__
					if( pm->ps->eFlags & EF_PRONE ) {
#endif //__OLD_GRENADE_TOSS__
						if( akimboFire ) {
							BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON2PRONE, qfalse, qtrue );
						} else {
							BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qtrue );
						}
#ifdef __OLD_GRENADE_TOSS__
					} else {
						if( akimboFire ) {
							BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON2, qfalse, qtrue );
						} else {
							BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
						}
					}
#endif //__OLD_GRENADE_TOSS__
				}
			} else {
				return;
			}
		}
	}

	if( pm->ps->weaponDelay > 0 ) {
		pm->ps->weaponDelay -= pml.msec;

		if ( pm->ps->weaponDelay <= 0 ) {
			pm->ps->weaponDelay = 0;
			delayedFire = qtrue;		// weapon delay has expired.  Fire this frame

			// double check the player is still holding the fire button down for these weapons
			// so you don't get a delayed "non-fire" (fire hit and released, then shot fires)
			switch(pm->ps->weapon) {
				default:
					break;
			}
		}
	}

	if (pm->ps->weaponstate == WEAPON_RELAXING) {
		pm->ps->weaponstate = WEAPON_READY;
		return;
	}

	if( pm->ps->eFlags & EF_PRONE_MOVING && !delayedFire )
		return;

	// make weapon function
	if ( pm->ps->weaponTime > 0 ) {
		pm->ps->weaponTime -= pml.msec;
		if( !(pm->cmd.buttons & BUTTON_ATTACK) && pm->ps->weaponTime < 0 )
			pm->ps->weaponTime = 0;

		// Gordon: aha, THIS is the kewl quick fire mode :)
		// JPW NERVE -- added back for multiplayer pistol balancing
		if ( pm->ps->weapon == WP_LUGER || pm->ps->weapon == WP_COLT || pm->ps->weapon == WP_SILENCED_LUGER || pm->ps->weapon == WP_SILENCED_COLT ||
				pm->ps->weapon == WP_KAR98 || pm->ps->weapon == WP_K43 || pm->ps->weapon == WP_CARBINE || pm->ps->weapon == WP_GARAND ||
				pm->ps->weapon == WP_GARAND_SCOPE || pm->ps->weapon == WP_K43_SCOPE || BG_IsAkimboWeapon( pm->ps->weapon ) ) {
// rain - moved releasedFire into pmext instead of ps
			if ( pm->pmext->releasedFire ) {
				if ( pm->cmd.buttons & BUTTON_ATTACK ) {
					// rain - akimbo weapons only have a 200ms delay, so
					// use a shorter time for quickfire (#255)
					if (BG_IsAkimboWeapon(pm->ps->weapon)) {
						if (pm->ps->weaponTime <= 50)
							pm->ps->weaponTime = 0;
					} else {
						if (pm->ps->weaponTime <= 150)
							pm->ps->weaponTime = 0;
					}
				}
			} else if (!(pm->cmd.buttons & BUTTON_ATTACK)) {
// rain - moved releasedFire into pmext instead of ps
				pm->pmext->releasedFire = qtrue;
			}
		}
	}


	// check for weapon change
	// can't change if weapon is firing, but can change
	// again if lowering or raising

	if( (pm->ps->weaponTime <= 0 || ( !weaponstateFiring && pm->ps->weaponDelay <= 0 )) && !delayedFire) {
		if ( pm->ps->weapon != pm->cmd.weapon ) {
			PM_BeginWeaponChange( pm->ps->weapon, pm->cmd.weapon, qfalse );	//----(SA)	modified
		}
	}

	if( pm->ps->weaponDelay > 0 ) {
		return;
	}

	// check for clip change
	PM_CheckForReload( pm->ps->weapon );

	if( pm->ps->weaponTime > 0 || pm->ps->weaponDelay > 0 ) {
		return;
	}

	if( pm->ps->weaponstate == WEAPON_RELOADING ) {
		PM_FinishWeaponReload();
	}

	// change weapon if time
	if( pm->ps->weaponstate == WEAPON_DROPPING || pm->ps->weaponstate == WEAPON_DROPPING_TORELOAD ) {
		PM_FinishWeaponChange();
		return;
	}

	if( pm->ps->weaponstate == WEAPON_RAISING ) {
		pm->ps->weaponstate = WEAPON_READY;

//		if( pm->ps->eFlags & EF_PRONE && pm->ps->weapon == WP_MOBILE_MG42 )
//			pm->pmext->proneMG42Zoomed = qtrue;

		PM_StartWeaponAnim(PM_IdleAnimForWeapon(pm->ps->weapon));
		return;
	} else if( pm->ps->weaponstate == WEAPON_RAISING_TORELOAD ) {
		pm->ps->weaponstate = WEAPON_READY;

		PM_BeginWeaponReload( pm->ps->weapon );

		return;
	}


	if(pm->ps->weapon == WP_NONE)	// this is possible since the player starts with nothing
		return;


	// JPW NERVE -- in multiplayer, don't allow panzerfaust or dynamite to fire if charge bar isn't full
	if( pm->ps->weapon == WP_PANZERFAUST ) {
		if( pm->ps->eFlags & EF_PRONE ) {
			return;
		}

		if( pm->skill[SK_HEAVY_WEAPONS] >= 1 ) {
			if (pm->cmd.serverTime - pm->ps->classWeaponTime < pm->soldierChargeTime * 0.66f)
				return;
		} else if (pm->cmd.serverTime - pm->ps->classWeaponTime < pm->soldierChargeTime)
			return;
	}

	if( pm->ps->weapon == WP_GPG40 || pm->ps->weapon == WP_M7 ) {
		if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->engineerChargeTime*0.5f))
			return;
	}

	if( pm->ps->weapon == WP_MORTAR_SET ) {
		if( pm->skill[SK_HEAVY_WEAPONS] >= 1 ) {
			// CHRUKER: b069 - Was using "0.5f*(1-0.3f)", however
			//          the 0.33f is used everywhere else, and
			//          is more precise.
			if( pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->soldierChargeTime*0.33f) ) {
				return;
			}
		} else if( pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->soldierChargeTime*0.5f) ) {
			return;
		}
		
		if( !delayedFire ) {
			pm->ps->weaponstate = WEAPON_READY;
		}
	}

	if (pm->ps->weapon == WP_SMOKE_BOMB || pm->ps->weapon == WP_SATCHEL) {
		if( pm->skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] >= 2 ) {
			if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->covertopsChargeTime*0.66f))
				return;
		} else if (pm->cmd.serverTime - pm->ps->classWeaponTime < pm->covertopsChargeTime) {
			return;
		}
	}

	if( pm->ps->weapon == WP_LANDMINE ) {
		if( pm->skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 2 ) {
			if( pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->engineerChargeTime*0.33f) ) {
				return;
			}
		} else if( pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->engineerChargeTime*0.5f) ) {
			return;
		}
	}

	if( pm->ps->weapon == WP_DYNAMITE ) {
		if (pm->skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 3 ) {
			if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->engineerChargeTime*0.66f))
				return;
		} else if (pm->cmd.serverTime - pm->ps->classWeaponTime < pm->engineerChargeTime)
			return;
	}

	if( pm->ps->weapon == WP_AMMO ) {
		if (pm->skill[SK_SIGNALS] >= 1 ) {
			if( pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->ltChargeTime*0.15f) ) {
				if( pm->cmd.buttons & BUTTON_ATTACK ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_NOPOWER, qtrue, qfalse );
				}
				return;
			}
		} else if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->ltChargeTime*0.25f)) {
			// rain - #202 - ^^ properly check ltChargeTime here, not medicChargeTime
			if( pm->cmd.buttons & BUTTON_ATTACK ) {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_NOPOWER, qtrue, qfalse );
			}
			return;
		}
	}

	if( pm->ps->weapon == WP_MEDKIT ) {
		if (pm->skill[SK_FIRST_AID] >= 2 ) {
			if( pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->medicChargeTime*0.15f) ) {
				if( pm->cmd.buttons & BUTTON_ATTACK ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_NOPOWER, qtrue, qfalse );		
				}
				return;
			}
		} else if (pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->medicChargeTime*0.25f)) {
			if( pm->cmd.buttons & BUTTON_ATTACK ) {
				BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_NOPOWER, qtrue, qfalse );
			}
			return;
		}
	}

	if( pm->ps->weapon == WP_SMOKE_MARKER ) {
		if( pm->skill[SK_SIGNALS] >= 2 ) {
			if(pm->cmd.serverTime - pm->ps->classWeaponTime < (pm->ltChargeTime*0.66f) ) {
				return;
			}
		} else if( pm->cmd.serverTime - pm->ps->classWeaponTime < pm->ltChargeTime ) {
			return;
		}
	}

	if( pm->ps->weapon == WP_AIRDROP_MARKER ) {
		if ( pm->cmd.serverTime - pm->ps->classWeaponTime < pm->covertopsChargeTime ) {
			return;
		}
	}

	if( pm->ps->weapon == WP_MEDIC_ADRENALINE ) {
		if (pm->cmd.serverTime - pm->ps->classWeaponTime < pm->covertopsChargeTime)
			return;
	}

/*	if( pm->ps->weapon == WP_TRIPMINE ) {
		trace_t trace;
		vec3_t start, end, forward;

		VectorCopy( pm->ps->origin, start );
		start[2] += pm->ps->viewheight;

		AngleVectors(pm->ps->viewangles, forward, NULL, NULL);

		VectorMA(start, 64, forward, end);

		pm->trace(&trace, start, NULL, NULL, end, pm->ps->clientNum, MASK_SHOT);

		if(trace.fraction == 1.f) {
			return; // didnt hit a nearby wall
		}

		if(trace.surfaceFlags & SURF_NOIMPACT) {
			return;
		}

		if(trace.entityNum != ENTITYNUM_WORLD) {
			return; // hit a player, door, etc
		}

		VectorCopy(trace.endpos, start);
		VectorMA(start, TRIPMINE_RANGE, trace.plane.normal, end);

		pm->trace(&trace, start, NULL, NULL, end, pm->ps->clientNum, MASK_SHOT);

		if(trace.fraction == 1.f) {
			return; // gap to opposite wall was too big
		}

		if(trace.surfaceFlags & SURF_NOIMPACT) {
			return;
		}

		if(trace.entityNum != ENTITYNUM_WORLD) {
			return; // hit a player, door, etc
		}
	}*/

	// check for fire
	// if not on fire button and there's not a delayed shot this frame...
	// consider also leaning, with delayed attack reset
	if((!(pm->cmd.buttons & (BUTTON_ATTACK | WBUTTON_ATTACK2)) && !delayedFire
#if !defined (__FRONTLINE__) && !defined (__EF__)
	  || (pm->ps->leanf != 0 && pm->ps->weapon != WP_GRENADE_LAUNCHER && pm->ps->weapon != WP_GRENADE_PINEAPPLE && pm->ps->weapon != WP_SMOKE_BOMB))
#endif //!defined (__FRONTLINE__) && !defined (__EF__)
	  ))
	{
		pm->ps->weaponTime	= 0;
		pm->ps->weaponDelay	= 0;
	
		if(weaponstateFiring) {	// you were just firing, time to relax
			PM_ContinueWeaponAnim(PM_IdleAnimForWeapon(pm->ps->weapon));
		}

		pm->ps->weaponstate = WEAPON_READY;
		return;
	}

	// a not mounted mortar can't fire
	if( pm->ps->weapon == WP_MORTAR ) {
		return;
	}

#ifdef SAVEGAME_SUPPORT
	if( pm->reloading ) {
		return;
	}
#endif // SAVEGAME_SUPPORT

	// player is zooming - no fire
	// JPW NERVE in MP, LT needs to zoom to call artillery
	if(pm->ps->eFlags & EF_ZOOMING) {
#ifdef GAMEDLL
		if(pm->ps->stats[STAT_PLAYER_CLASS] == PC_FIELDOPS) {
			pm->ps->weaponTime += 500;
			PM_AddEvent( EV_FIRE_WEAPON );
		}
#endif
		return;
	}

	// player is underwater - no fire
	if(pm->waterlevel == 3) {
		if(	pm->ps->weapon != WP_KNIFE &&
			pm->ps->weapon != WP_GRENADE_LAUNCHER &&
			pm->ps->weapon != WP_GRENADE_PINEAPPLE &&
			pm->ps->weapon != WP_DYNAMITE &&
			pm->ps->weapon != WP_LANDMINE &&
			pm->ps->weapon != WP_TRIPMINE &&
			pm->ps->weapon != WP_SMOKE_BOMB &&
			!(pm->ps->weapon == WP_MEDIC_SYRINGE 
				&& PM_UW_SYRINGE) &&
			!(pm->ps->weapon == WP_PLIERS
				&& PM_UW_PLIERS)
			) {

				// event for underwater 'click' for nofire
				// tjw:  since this depends on a server setting
				//       don't let the client predict this
				//       event.
#ifdef GAMEDLL
				PM_AddEvent(EV_NOFIRE_UNDERWATER);
#endif
				pm->ps->weaponTime	= 500;
				// avoid insta-fire after water exit on delayed weapon attacks
				pm->ps->weaponDelay	= 0;			
				return;
		}
	}

	// start the animation even if out of ammo
	switch(pm->ps->weapon)
	{
		default:
			if(!weaponstateFiring) {
				// delay so the weapon can get up into position before firing (and showing the flash)
				pm->ps->weaponDelay = GetAmmoTableData(pm->ps->weapon)->fireDelayTime;
			} else {
				if( pm->ps->eFlags & EF_PRONE ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qtrue );
				} else {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
				}
			}
			break;
		// machineguns should continue the anim, rather than start each fire
		case WP_MP40:
		case WP_THOMPSON:
		case WP_STEN:
		case WP_MEDKIT:					// NERVE - SMF
		case WP_PLIERS:					// NERVE - SMF
		case WP_SMOKE_MARKER:			// NERVE - SMF
		case WP_FG42:
		case WP_FG42SCOPE:
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
		case WP_LOCKPICK:
#ifdef __EF__
		case WP_PPSH:
		case WP_STG44:
		case WP_PPS:
		case WP_SVT40:
		case WP_30CAL:
		case WP_30CAL_SET:
		case WP_AIRDROP_MARKER:			// UQ1
#endif //__EF__

			if(!weaponstateFiring) {
				pm->ps->weaponDelay = GetAmmoTableData(pm->ps->weapon)->fireDelayTime;
			} else {
				if( pm->ps->eFlags & EF_PRONE /*|| (pm->ps->weapon == WP_MOBILE_MG42_SET && BG_CanSetupMobileMG42Here())*/ ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qtrue, qtrue );
				} else {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qtrue, qtrue );
				}
			}
			break;
	  //case WP_PTRS://Masteries, PTRS only works in Prone mode
		case WP_PTRS_SET:
            if(!weaponstateFiring) {
				pm->ps->weaponDelay = GetAmmoTableData(pm->ps->weapon)->fireDelayTime;
			} else {
				if( pm->ps->eFlags & EF_PRONE /*|| (pm->ps->weapon == WP_MOBILE_MG42_SET && BG_CanSetupMobileMG42Here())*/ ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qtrue, qtrue );
				} 
			}
			break;
		case WP_PANZERFAUST:
		case WP_LUGER:
		case WP_COLT:
		case WP_GARAND:
		case WP_K43:
		case WP_KAR98:
		case WP_CARBINE:
		case WP_GPG40:
		case WP_M7:
		case WP_SILENCED_LUGER:
		case WP_SILENCED_COLT:
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
			if(!weaponstateFiring) {
				// JPW NERVE -- pfaust has spinup time in MP
				if (pm->ps->weapon == WP_PANZERFAUST)
					PM_AddEvent( EV_SPINUP );
				// jpw

				pm->ps->weaponDelay = GetAmmoTableData(pm->ps->weapon)->fireDelayTime;
			} else {
				if( pm->ps->eFlags & EF_PRONE ) {
					if( akimboFire ) {
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON2PRONE, qfalse, qtrue );
					} else {
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qtrue );
					}
				} else {
					if( akimboFire ) {
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON2, qfalse, qtrue );
					} else {
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
					}
				}
			}
			break;
		case WP_MORTAR_SET:
			if(!weaponstateFiring) {
				PM_AddEvent( EV_SPINUP );
				PM_StartWeaponAnim(PM_AttackAnimForWeapon(WP_MORTAR_SET));
				pm->ps->weaponDelay = GetAmmoTableData(pm->ps->weapon)->fireDelayTime;
			} else {
				if( pm->ps->eFlags & EF_PRONE ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qtrue );
				} else {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
				}
			}
			break;

		// melee
		case WP_KNIFE:
			if(!delayedFire) {
				if( pm->ps->eFlags & EF_PRONE ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qfalse );
				} else {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qfalse );
				}
			}
			break;

		// throw
		case WP_DYNAMITE:
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_SMOKE_BOMB:
			if(!delayedFire) {
				if(PM_WeaponAmmoAvailable(pm->ps->weapon)) {
					if(pm->ps->weapon == WP_DYNAMITE) {
						pm->ps->grenadeTimeLeft = 50;
					} else {
						pm->ps->grenadeTimeLeft = 4000; // start at four seconds and count down
					}

					PM_StartWeaponAnim(PM_AttackAnimForWeapon(pm->ps->weapon));
				}

				pm->ps->weaponDelay = GetAmmoTableData(pm->ps->weapon)->fireDelayTime;
			}
			break;
		case WP_LANDMINE:
			if(!delayedFire) {
				if(PM_WeaponAmmoAvailable(pm->ps->weapon)) {
					if( pm->ps->eFlags & EF_PRONE ) {
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON2PRONE, qfalse, qtrue );
					} else {
						BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
					}
				}

				pm->ps->weaponDelay = GetAmmoTableData(pm->ps->weapon)->fireDelayTime;
			}
			break;
		case WP_TRIPMINE:
		case WP_SATCHEL:
			if(!delayedFire) {
				if(PM_WeaponAmmoAvailable(pm->ps->weapon)) {
					PM_StartWeaponAnim(PM_AttackAnimForWeapon(pm->ps->weapon));
				}

				pm->ps->weaponDelay = GetAmmoTableData(pm->ps->weapon)->fireDelayTime;
			}
			break;
		case WP_SATCHEL_DET:
			if(!weaponstateFiring) {
				PM_AddEvent( EV_SPINUP );
				pm->ps->weaponDelay = GetAmmoTableData(pm->ps->weapon)->fireDelayTime;
				PM_ContinueWeaponAnim(PM_AttackAnimForWeapon(WP_SATCHEL_DET));
			} else {
				if( pm->ps->eFlags & EF_PRONE ) {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPONPRONE, qfalse, qtrue );
				} else {
					BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_FIREWEAPON, qfalse, qtrue );
				}
			}

			break;
	}

	pm->ps->weaponstate = WEAPON_FIRING;

	// Gordon: reset player disguise on firing
//	if( pm->ps->weapon != WP_SMOKE_BOMB && pm->ps->weapon != WP_SATCHEL && pm->ps->weapon != WP_SATCHEL_DET ) {	// Arnout: not for these weapons
//		pm->ps->powerups[PW_OPS_DISGUISED] = 0;
//	}

	// check for out of ammo

	ammoNeeded = GetAmmoTableData(pm->ps->weapon)->uses;

	if( pm->ps->weapon ) {
		int			ammoAvailable;
		qboolean	reloading, playswitchsound = qtrue;

		ammoAvailable = PM_WeaponAmmoAvailable(pm->ps->weapon);

		if(ammoNeeded > ammoAvailable)
		{
			// you have ammo for this, just not in the clip
			reloading = (qboolean)(ammoNeeded <= pm->ps->ammo[ BG_FindAmmoForWeapon(pm->ps->weapon)]);

			// if not in auto-reload mode, and reload was not explicitely requested, just play the 'out of ammo' sound
			if (!pm->pmext->bAutoReload && IS_AUTORELOAD_WEAPON(pm->ps->weapon) && !(pm->cmd.wbuttons & WBUTTON_RELOAD))
			{        
				reloading = qfalse;
			}

			switch(pm->ps->weapon) {
				// Ridah, only play if using a triggered weapon
				case WP_DYNAMITE:
				case WP_GRENADE_LAUNCHER:
				case WP_GRENADE_PINEAPPLE:
				case WP_LANDMINE:
				case WP_TRIPMINE:
				case WP_SMOKE_BOMB:
					playswitchsound = qfalse;
					break;

				// some weapons not allowed to reload.  must switch back to primary first
				case WP_FG42SCOPE:
				case WP_GARAND_SCOPE:
				case WP_K43_SCOPE:
					reloading = qfalse;
					break;
			}

			if(playswitchsound) {
				if( reloading ) {
					PM_AddEvent( EV_EMPTYCLIP );
				} else {
					PM_AddEvent( EV_NOAMMO );
				}
			}

			if(reloading) {
				PM_ContinueWeaponAnim(PM_ReloadAnimForWeapon(pm->ps->weapon));
			} else {
				PM_ContinueWeaponAnim(PM_IdleAnimForWeapon(pm->ps->weapon));
				pm->ps->weaponTime += 500;
			}

			return;
		}
	}

	if(pm->ps->weaponDelay > 0)
		// if it hits here, the 'fire' has just been hit and the weapon dictated a delay.
		// animations have been started, weaponstate has been set, but no weapon events yet. (except possibly EV_NOAMMO)
		// checks for delayed weapons that have already been fired are return'ed above.
		return;

#ifdef GAMEDLL
	if ( !(g_entities[pm->ps->clientNum].r.svFlags & SVF_BOT) && !pm->isNPC && (!(pm->ps->eFlags & EF_PRONE) && (pml.groundTrace.surfaceFlags & SURF_SLICK)) ) {
#else //!GAMEDLL
	if( !(pm->ps->eFlags & EF_PRONE) && (pml.groundTrace.surfaceFlags & SURF_SLICK) ) { 
#endif //GAMEDLL
		float fwdmove_knockback = 0.f;
		float bckmove_knockback = 0.f;

		switch( pm->ps->weapon ) {
			case WP_MOBILE_MG42:	fwdmove_knockback = 4000.f;
									fwdmove_knockback = 400.f;
									break;
			case WP_PANZERFAUST:	fwdmove_knockback = 32000.f;
									bckmove_knockback = 1200.f;
									break;
			case WP_FLAMETHROWER:	fwdmove_knockback = 2000.f;
									bckmove_knockback = 40.f;
									break;
		}

		if( fwdmove_knockback > 0.f ) {
			// Add some knockback on slick
			vec3_t kvel;
			float mass = 200;

			if( DotProduct( pml.forward, pm->ps->velocity ) > 0  ) {
				VectorScale( pml.forward, -1.f * (fwdmove_knockback / mass), kvel );	// -1 as we get knocked backwards
			} else {
				VectorScale( pml.forward, -1.f * (fwdmove_knockback / mass), kvel );	// -1 as we get knocked backwards
			}

			VectorAdd( pm->ps->velocity, kvel, pm->ps->velocity );

			if ( !pm->ps->pm_time ) {
				pm->ps->pm_time = 100;
				pm->ps->pm_flags |= PMF_TIME_KNOCKBACK;
			}
		}
	}

	// take an ammo away if not infinite
	if(PM_WeaponAmmoAvailable(pm->ps->weapon) != -1 ) {
		// Rafael - check for being mounted on mg42
		if (!(pm->ps->persistant[PERS_HWEAPON_USE]) && !(pm->ps->eFlags & EF_MOUNTEDTANK)) {
			PM_WeaponUseAmmo(pm->ps->weapon, ammoNeeded);
		}
	}


	// fire weapon

	// add weapon heat
	if(GetAmmoTableData(pm->ps->weapon)->maxHeat)
		pm->ps->weapHeat[pm->ps->weapon] += GetAmmoTableData(pm->ps->weapon)->nextShotTime;

	// first person weapon animations

	// if this was the last round in the clip, play the 'lastshot' animation
	// this animation has the weapon in a "ready to reload" state
	if( BG_IsAkimboWeapon( pm->ps->weapon ) ) {
		if( akimboFire ) {
			weapattackanim = WEAP_ATTACK1;
		} else {
			weapattackanim = WEAP_ATTACK2;
		}
	} else {
		if( PM_WeaponClipEmpty( pm->ps->weapon ) ) {
			weapattackanim = PM_LastAttackAnimForWeapon(pm->ps->weapon);
		} else {
			weapattackanim = PM_AttackAnimForWeapon(pm->ps->weapon);
		}
	}

	switch(pm->ps->weapon) {
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_DYNAMITE:
		case WP_K43:
		case WP_KAR98:
		case WP_GPG40:
		case WP_CARBINE:
		case WP_M7:
		case WP_LANDMINE:
		case WP_TRIPMINE:
		case WP_SMOKE_BOMB:
			PM_StartWeaponAnim(weapattackanim);
			break;

		case WP_MP40:
		case WP_THOMPSON:
		case WP_STEN:
		case WP_MEDKIT:
		case WP_PLIERS:
		case WP_SMOKE_MARKER:
		case WP_SATCHEL_DET:
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
		case WP_LOCKPICK:
#ifdef __EF__
		case WP_PPSH:
		case WP_STG44:
		case WP_PPS:
		case WP_SVT40:
		case WP_30CAL:
		case WP_30CAL_SET:
		//case WP_PTRS://Masteries, I don´t know well...
		case WP_PTRS_SET:
		case WP_AIRDROP_MARKER:
#endif //__EF__
			PM_ContinueWeaponAnim(weapattackanim);
			break;

		case WP_MORTAR_SET:
			break;	// no animation

		default:
			// RF, testing
//			PM_ContinueWeaponAnim(weapattackanim);
			PM_StartWeaponAnim(weapattackanim);
			break;
	}

	// JPW NERVE -- in multiplayer, pfaust fires once then switches to pistol since it's useless for a while
	if ( (pm->ps->weapon == WP_PANZERFAUST) || (pm->ps->weapon == WP_SMOKE_MARKER ) || (pm->ps->weapon == WP_DYNAMITE) || (pm->ps->weapon == WP_SMOKE_BOMB) || (pm->ps->weapon == WP_LANDMINE) || (pm->ps->weapon == WP_SATCHEL)
#ifdef __EF__
		|| (pm->ps->weapon == WP_AIRDROP_MARKER )
#endif //__EF__
		)
		PM_AddEvent( EV_NOAMMO );
	// jpw

	if( pm->ps->weapon == WP_SATCHEL ) {
		pm->ps->ammoclip[WP_SATCHEL_DET] = 1;
		pm->ps->ammo[WP_SATCHEL] = 0;
		pm->ps->ammoclip[WP_SATCHEL] = 0;
		PM_BeginWeaponChange( WP_SATCHEL, WP_SATCHEL_DET, qfalse );
	}

	// WP_M7 and WP_GPG40 run out of ammo immediately after firing their last grenade
	if( (pm->ps->weapon == WP_M7 || pm->ps->weapon == WP_GPG40) && !pm->ps->ammo[ BG_FindAmmoForWeapon(pm->ps->weapon)] ) {
		PM_AddEvent( EV_NOAMMO );
	}

	if( pm->ps->weapon == WP_MORTAR_SET && !pm->ps->ammo[WP_MORTAR] ) {
		PM_AddEvent( EV_NOAMMO );
		//PM_BeginWeaponChange( WP_MORTAR_SET, WP_MORTAR, qfalse );
	}

	if( BG_IsAkimboWeapon( pm->ps->weapon ) ) {
		if( akimboFire ) {
			PM_AddEvent( EV_FIRE_WEAPON );
		} else {
			PM_AddEvent( EV_FIRE_WEAPONB );
		}
	} else {
		if( PM_WeaponClipEmpty( pm->ps->weapon ) ) {
			PM_AddEvent( EV_FIRE_WEAPON_LASTSHOT );
		} else {
			PM_AddEvent( EV_FIRE_WEAPON );
		}
	}

	// RF
// rain - moved releasedFire into pmext instead of ps
	pm->pmext->releasedFire = qfalse;
	pm->ps->lastFireTime = pm->cmd.serverTime;

	aimSpreadScaleAdd = 0;

	switch( pm->ps->weapon ) {
	case WP_KNIFE:
	case WP_PANZERFAUST:
	case WP_DYNAMITE:
	case WP_GRENADE_LAUNCHER:
	case WP_GRENADE_PINEAPPLE:
	case WP_FLAMETHROWER:
	case WP_GPG40:
	case WP_M7:
	case WP_LANDMINE:
	case WP_TRIPMINE:
	case WP_SMOKE_BOMB:
	case WP_MORTAR_SET:
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		break;

	case WP_LUGER:
	case WP_SILENCED_LUGER:
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;
// rain - colt and luger are supposed to be balanced
//		aimSpreadScaleAdd = 35;
		aimSpreadScaleAdd = 20;
		break;

	case WP_COLT:
	case WP_SILENCED_COLT:
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		aimSpreadScaleAdd = 20;
		break;

	case WP_AKIMBO_COLT:
	case WP_AKIMBO_SILENCEDCOLT:
		// if you're firing an akimbo weapon, and your other gun is dry,
		// nextshot needs to take 2x time
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;

		// added check for last shot in both guns so there's no delay for the last shot
		if( !pm->ps->ammoclip[BG_FindClipForWeapon(pm->ps->weapon)] ) {
			if( !akimboFire )
				addTime = 2 * GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		} else if( !pm->ps->ammoclip[BG_FindClipForWeapon(BG_AkimboSidearm(pm->ps->weapon))] ) {
			if( akimboFire )
				addTime = 2 * GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		}

		aimSpreadScaleAdd = 20;
		break;

	case WP_AKIMBO_LUGER:
	case WP_AKIMBO_SILENCEDLUGER:
		// if you're firing an akimbo weapon, and your other gun is dry,
		// nextshot needs to take 2x time
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;

		// rain - fixed the swapped usage of akimboFire vs. the colt
		// so that the last shot isn't delayed
		if( !pm->ps->ammoclip[BG_FindClipForWeapon(pm->ps->weapon)] ) {
			if( !akimboFire )
				addTime = 2 * GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		} else if( !pm->ps->ammoclip[BG_FindClipForWeapon(BG_AkimboSidearm(pm->ps->weapon))] ) {
			if( akimboFire )
				addTime = 2 * GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		}

// rain - colt and luger are supposed to be balanced
//		aimSpreadScaleAdd = 35;
		aimSpreadScaleAdd = 20;
		break;

	case WP_GARAND:
	case WP_K43:
	case WP_KAR98:
	case WP_CARBINE:
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		aimSpreadScaleAdd = 50;
		break;

	case WP_GARAND_SCOPE:
	case WP_K43_SCOPE:
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;

		aimSpreadScaleAdd = 200;
		// jpw

		break;

	case WP_FG42:
	case WP_FG42SCOPE:
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		aimSpreadScaleAdd = 200/2.f;
		break;

	case WP_MP40:
	case WP_THOMPSON:
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		aimSpreadScaleAdd = 15+rand()%10;	// (SA) new values for DM
		break;

#ifdef __EF__
	case WP_PPSH:
	case WP_STG44:
	case WP_PPS:
	case WP_SVT40:
#endif //__EF__
	case WP_STEN:
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		aimSpreadScaleAdd = 15+rand()%10;	// (SA) new values for DM
		break;

#ifdef __EF__
	case WP_30CAL:
	case WP_30CAL_SET:
	case WP_PTRS://Masteries
	case WP_PTRS_SET:
#endif //__EF__
	case WP_MOBILE_MG42:
	case WP_MOBILE_MG42_SET:
		if(weapattackanim == WEAP_ATTACK_LASTSHOT) {
			addTime = 2000;
		} else {
			addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		}
		aimSpreadScaleAdd = 20;
		break;
// JPW NERVE
	case WP_MEDIC_SYRINGE:
	case WP_MEDIC_ADRENALINE:
	case WP_AMMO:
	// TAT 1/30/2003 - lockpick will use value in table too
	case WP_LOCKPICK:
		addTime = GetAmmoTableData(pm->ps->weapon)->nextShotTime;
		break;
// jpw
	// JPW: engineers disarm bomb "on the fly" (high sample rate) but medics & LTs throw out health pack/smoke grenades slow
	// NERVE - SMF
	case WP_PLIERS:
		addTime = 50;
		break;
	case WP_MEDKIT:
		addTime = 1000;
		break;
	case WP_SMOKE_MARKER:
		addTime = 1000;
		break;
	// -NERVE - SMF
#ifdef __EF__
	case WP_AIRDROP_MARKER:
		addTime = 1000;
		break;
#endif //__EF__
	default:
		break;
	}
	
	// set weapon recoil
	pm->pmext->lastRecoilDeltaTime = 0;

	switch( pm->ps->weapon ) {
	case WP_GARAND_SCOPE:
	case WP_K43_SCOPE:
		pm->pmext->weapRecoilTime = pm->cmd.serverTime;
		pm->pmext->weapRecoilDuration = 300;
		pm->pmext->weapRecoilYaw = crandom() * .5f;

		if( pm->skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] >= 3 ) {
			pm->pmext->weapRecoilPitch = .25f;
		} else {
			pm->pmext->weapRecoilPitch = .5f;
		}
		break;
	case WP_MOBILE_MG42:
		pm->pmext->weapRecoilTime = pm->cmd.serverTime;
		pm->pmext->weapRecoilDuration = 200;
		if( pm->ps->pm_flags & PMF_DUCKED || pm->ps->eFlags & EF_PRONE ) {
			pm->pmext->weapRecoilYaw = crandom() * .5f;
			pm->pmext->weapRecoilPitch = .45f * random() * .15f;
		} else {
			pm->pmext->weapRecoilYaw = crandom() * .25f;
			pm->pmext->weapRecoilPitch = .75f * random() * .2f;
		}
		break;
	/*case WP_MOBILE_MG42_SET:
		pm->pmext->weapRecoilTime = 0;
		pm->pmext->weapRecoilYaw = 0.f;
		break;*/
	case WP_FG42SCOPE:
		pm->pmext->weapRecoilTime = pm->cmd.serverTime;
		pm->pmext->weapRecoilDuration = 100;
		pm->pmext->weapRecoilYaw = 0.f;
		pm->pmext->weapRecoilPitch = .45f * random() * .15f;

		if( pm->skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] >= 3 ) {
			pm->pmext->weapRecoilPitch *= .5f;
		}
		break;
	case WP_LUGER:
	case WP_SILENCED_LUGER:
	case WP_AKIMBO_LUGER:
	case WP_AKIMBO_SILENCEDLUGER:
	case WP_COLT:
	case WP_SILENCED_COLT:
	case WP_AKIMBO_COLT:
	case WP_AKIMBO_SILENCEDCOLT:
		pm->pmext->weapRecoilTime = pm->cmd.serverTime;
		pm->pmext->weapRecoilDuration = pm->skill[SK_LIGHT_WEAPONS] >= 3 ? 70 : 100;
		pm->pmext->weapRecoilYaw = 0.f;//crandom() * .1f;
		pm->pmext->weapRecoilPitch = pm->skill[SK_LIGHT_WEAPONS] >= 3 ? .25f * random() * .15f : .45f * random() * .15f;
		break;
	default:
		pm->pmext->weapRecoilTime = 0;
		pm->pmext->weapRecoilYaw = 0.f;
		break;
	}

	// check for overheat

	// the weapon can overheat, and it's hot
	if(	GetAmmoTableData(pm->ps->weapon)->maxHeat && pm->ps->weapHeat[pm->ps->weapon]) {
		// it is overheating
		if(pm->ps->weapHeat[pm->ps->weapon] >= GetAmmoTableData(pm->ps->weapon)->maxHeat) {
			pm->ps->weapHeat[pm->ps->weapon] = GetAmmoTableData(pm->ps->weapon)->maxHeat;	// cap heat to max
			PM_AddEvent( EV_WEAP_OVERHEAT);
//			PM_StartWeaponAnim(WEAP_IDLE1);	// removed.  client handles anim in overheat event
			addTime = 2000;		// force "heat recovery minimum" to 2 sec right now
		}
	}

	// add the recoil amount to the aimSpreadScale
//	pm->ps->aimSpreadScale += 3.0*aimSpreadScaleAdd;
//	if (pm->ps->aimSpreadScale > 255) pm->ps->aimSpreadScale = 255;
	pm->ps->aimSpreadScaleFloat += 3.0*aimSpreadScaleAdd;
	if (pm->ps->aimSpreadScaleFloat > 255)
		pm->ps->aimSpreadScaleFloat = 255;

	if( pm->skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] >= 3 && pm->ps->stats[STAT_PLAYER_CLASS] == PC_COVERTOPS ) {
		pm->ps->aimSpreadScaleFloat *= .5f;
	}

	pm->ps->aimSpreadScale = (int)(pm->ps->aimSpreadScaleFloat);

	pm->ps->weaponTime += addTime;

	PM_SwitchIfEmpty();
}


/*
================
PM_Animate
================
*/
#define MYTIMER_SALUTE	 1133	// 17 frames, 15 fps
#define MYTIMER_DISMOUNT 667	// 10 frames, 15 fps

/*
================
PM_DropTimers
================
*/
static void PM_DropTimers( void ) {
	// drop misc timing counter
	if ( pm->ps->pm_time ) {
		if ( pml.msec >= pm->ps->pm_time ) {
			pm->ps->pm_flags &= ~PMF_ALL_TIMES;
			pm->ps->pm_time = 0;
		} else {
			pm->ps->pm_time -= pml.msec;
		}
	}

	// drop animation counter
	if ( pm->ps->legsTimer > 0 ) {
		pm->ps->legsTimer -= pml.msec;
		if ( pm->ps->legsTimer < 0 ) {
			pm->ps->legsTimer = 0;
		}
	}

	if ( pm->ps->torsoTimer > 0 ) {
		pm->ps->torsoTimer -= pml.msec;
		if ( pm->ps->torsoTimer < 0 ) {
			pm->ps->torsoTimer = 0;
		}
	}

	// first person weapon counter
	if ( pm->pmext->weapAnimTimer > 0 ) {
		pm->pmext->weapAnimTimer -= pml.msec;
		if ( pm->pmext->weapAnimTimer < 0 ) {
			pm->pmext->weapAnimTimer = 0;
		}
	}
}



#define LEAN_MAX	28.0f
#define LEAN_TIME_TO	200.0f	// time to get to/from full lean
#define LEAN_TIME_FR	300.0f	// time to get to/from full lean

/*
==============
PM_CalcLean

==============
*/
void PM_UpdateLean(playerState_t *ps, usercmd_t *cmd, pmove_t *tpm) {
	vec3_t		start, end, tmins, tmaxs, right;
	int			leaning = 0;	// -1 left, 1 right
	float		leanofs = 0;
	vec3_t		viewangles;
	trace_t		trace;

	if( (cmd->wbuttons & (WBUTTON_LEANLEFT|WBUTTON_LEANRIGHT))  && !cmd->forwardmove && cmd->upmove <= 0 ) {
		// if both are pressed, result is no lean
		if(cmd->wbuttons & WBUTTON_LEANLEFT)
			leaning -= 1;
		if(cmd->wbuttons & WBUTTON_LEANRIGHT)
			leaning += 1;
	}

	if(	BG_PlayerMounted(ps->eFlags) ) {
		leaning = 0;	// leaning not allowed on mg42
	}

#if !defined (__FRONTLINE__) && !defined (__EF__)
	if(ps->eFlags & EF_FIRING)
		leaning = 0;	// not allowed to lean while firing
#endif //!defined (__FRONTLINE__) && !defined (__EF__)

  // ATVI Wolfenstein Misc #479 - initial fix to #270 would crash in g_synchronousClients 1 situation
	if( ps->weaponstate == WEAPON_FIRING && ps->weapon == WP_DYNAMITE )
		leaning = 0; // not allowed while tossing dynamite
	
	if( ps->eFlags & EF_PRONE || ps->weapon == WP_MORTAR_SET )
		leaning = 0;	// not allowed to lean while prone

	leanofs = ps->leanf;


	if(!leaning) {	// go back to center position
		if ( leanofs > 0 ) {		// right
			//FIXME: play lean anim backwards?
			leanofs -= (((float)pml.msec/(float)LEAN_TIME_FR)*LEAN_MAX);
			if ( leanofs < 0 )
				leanofs = 0;
		}
		else if ( leanofs < 0 ) {	// left
			//FIXME: play lean anim backwards?
			leanofs += (((float)pml.msec/(float)LEAN_TIME_FR)*LEAN_MAX);
			if ( leanofs > 0 )
				leanofs = 0;
		}
	}

	if(leaning) {
		if(leaning > 0) {	// right
			if(leanofs < LEAN_MAX)
				leanofs += (((float)pml.msec/(float)LEAN_TIME_TO)*LEAN_MAX);

			if(leanofs > LEAN_MAX)
				leanofs = LEAN_MAX;

		}
		else {				// left
			if(leanofs > -LEAN_MAX)
				leanofs -= (((float)pml.msec/(float)LEAN_TIME_TO)*LEAN_MAX);

			if(leanofs < -LEAN_MAX)
				leanofs = -LEAN_MAX;

		}
	}

	ps->leanf = leanofs;

	if(leaning){
		VectorCopy( ps->origin, start );
		start[2] += ps->viewheight;

		VectorCopy( ps->viewangles, viewangles );
		viewangles[ROLL] += leanofs/2.0f;
		AngleVectors( viewangles, NULL, right, NULL );
		VectorMA( start, leanofs, right, end );		
		
		VectorSet( tmins, -8, -8, -7 ); // ATVI Wolfenstein Misc #472, bumped from -4 to cover gun clipping issue
		VectorSet( tmaxs, 8, 8, 4 );

		if( pm )
			pm->trace (&trace, start, tmins, tmaxs, end, ps->clientNum, MASK_PLAYERSOLID);
		else
			tpm->trace (&trace, start, tmins, tmaxs, end, ps->clientNum, MASK_PLAYERSOLID);

		ps->leanf *= trace.fraction;
	}

#if !defined (__FRONTLINE__) && !defined (__EF__)
	if(ps->leanf)
		cmd->rightmove = 0;		// also disallowed in cl_input ~391
#endif //!defined (__FRONTLINE__) && !defined (__EF__)
}



/*
================
PM_UpdateViewAngles

This can be used as another entry point when only the viewangles
are being updated isntead of a full move

	!! NOTE !! Any changes to mounted/prone view should be duplicated in BotEntityWithinView()
================
*/
// rain - take a tracemask as well - we can't use anything out of pm
void PM_UpdateViewAngles( playerState_t *ps, pmoveExt_t *pmext, usercmd_t *cmd, void (trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask ), int tracemask ) {	//----(SA)	modified
	short	temp;
	int		i;
	pmove_t tpm;
	vec3_t oldViewAngles;

	// DHM - Nerve :: Added support for PMF_TIME_LOCKPLAYER
	if ( ps->pm_type == PM_INTERMISSION || ps->pm_flags & PMF_TIME_LOCKPLAYER ) {
		return;		// no view changes at all
	}

	if ( ps->pm_type != PM_SPECTATOR && ps->stats[STAT_HEALTH] <= 0 ) {

		// DHM - Nerve :: Allow players to look around while 'wounded' or lock to a medic if nearby
		temp = cmd->angles[1] + ps->delta_angles[1];
		// rain - always allow this.  viewlocking will take precedence
		// if a medic is found
		// rain - using a full short and converting on the client so that
		// we get >1 degree resolution
		ps->stats[STAT_DEAD_YAW] = temp;
		// tjw: don't let corpses bury their heads in the sand
		ps->viewangles[2] = 0;
		cmd->angles[2] = 0;

		ps->viewangles[0] = 0;
		cmd->angles[0] = 0;

		if(!PM_CORPSE_SPIN)
			return;
	}

	VectorCopy( ps->viewangles, oldViewAngles );

	// circularly clamp the angles with deltas
	for (i=0 ; i<3 ; i++) {
		temp = cmd->angles[i] + ps->delta_angles[i];
		if ( i == PITCH ) {
			// don't let the player look up or down more than 90 degrees
			if ( temp > 16000 ) {
				ps->delta_angles[i] = 16000 - cmd->angles[i];
				temp = 16000;
			} else if ( temp < -16000 ) {
				ps->delta_angles[i] = -16000 - cmd->angles[i];
				temp = -16000;
			}
		}
		ps->viewangles[i] = SHORT2ANGLE(temp);
	}

#ifdef __VEHICLES__
	if ( ps->eFlags & EF_VEHICLE ) 
	{
		float yaw, oldYaw;
		float degsSec = 250; // fixme do some lower number but allow faster turret

		yaw = ps->viewangles[YAW];
		oldYaw = oldViewAngles[YAW];

		if ( yaw - oldYaw > 180 ) {
			yaw -= 360;
		}
		if ( yaw - oldYaw < -180 ) {
			yaw += 360;
		}

		if( yaw > oldYaw ) {
			if( yaw - oldYaw > degsSec * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw + degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		} else if( oldYaw > yaw ) {
			if( oldYaw - yaw > degsSec * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw - degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}
	}
#endif //__VEHICLES__

	if( BG_PlayerMounted(ps->eFlags) ) {
		float yaw, oldYaw;
		float degsSec = MG42_YAWSPEED;
		float arcMin, arcMax, arcDiff;

		yaw = ps->viewangles[YAW];
		oldYaw = oldViewAngles[YAW];

		if ( yaw - oldYaw > 180 ) {
			yaw -= 360;
		}
		if ( yaw - oldYaw < -180 ) {
			yaw += 360;
		}

		if( yaw > oldYaw ) {
			if( yaw - oldYaw > degsSec * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw + degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		} else if( oldYaw > yaw ) {
			if( oldYaw - yaw > degsSec * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw - degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}

		// limit harc and varc
		
		// pitch (varc)
		arcMax = pmext->varc;
		if(ps->eFlags & EF_AAGUN_ACTIVE) {
			arcMin = 0;
		} else if (ps->eFlags & EF_MOUNTEDTANK) {
			float angle;

			arcMin = 14;
			arcMax = 50;

			angle = cos( DEG2RAD( AngleNormalize180( pmext->centerangles[ 1 ] - ps->viewangles[ 1 ] )));
			angle = -AngleNormalize360(angle * AngleNormalize180( 0 - pmext->centerangles[ 0 ] ));

			pmext->centerangles[ PITCH ] = angle;
		} else {
			arcMin = pmext->varc / 2;
		}

		arcDiff = AngleNormalize180( ps->viewangles[PITCH] - pmext->centerangles[PITCH] );

		if( arcDiff > arcMin ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->centerangles[PITCH] + arcMin );

			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		} else if( arcDiff < -arcMax ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->centerangles[PITCH] - arcMax );
			
			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		}

		if (!(ps->eFlags & EF_MOUNTEDTANK)) {
			// yaw (harc)
			arcMin = arcMax = pmext->harc;
			arcDiff = AngleNormalize180( ps->viewangles[YAW] - pmext->centerangles[YAW] );

			if( arcDiff > arcMin ) {
				ps->viewangles[YAW] = AngleNormalize180( pmext->centerangles[YAW] + arcMin );

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			} else if( arcDiff < -arcMax ) {
				ps->viewangles[YAW] = AngleNormalize180( pmext->centerangles[YAW] - arcMax );
				
				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}
	} else if( ps->weapon == WP_MORTAR_SET ) {
		float degsSec = 60.f;
		float yaw, oldYaw;
		float pitch, oldPitch;
		float pitchMax = 30.f;
		float yawDiff, pitchDiff;

		yaw = ps->viewangles[YAW];
		oldYaw = oldViewAngles[YAW];

		if ( yaw - oldYaw > 180 ) {
			yaw -= 360;
		}
		if ( yaw - oldYaw < -180 ) {
			yaw += 360;
		}

		if( yaw > oldYaw ) {
			if( yaw - oldYaw > degsSec * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw + degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		} else if( oldYaw > yaw ) {
			if( oldYaw - yaw > degsSec * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw - degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}

		pitch = ps->viewangles[PITCH];
		oldPitch = oldViewAngles[PITCH];

		if ( pitch - oldPitch > 180 ) {
			pitch -= 360;
		}
		if ( pitch - oldPitch < -180 ) {
			pitch += 360;
		}

		if( pitch > oldPitch ) {
			if( pitch - oldPitch > degsSec * pml.frametime ) {
				ps->viewangles[PITCH] = oldPitch + degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
			}
		} else if( oldPitch > pitch ) {
			if( oldPitch - pitch > degsSec * pml.frametime ) {
				ps->viewangles[PITCH] = oldPitch - degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
			}
		}

		// yaw
		yawDiff = ps->viewangles[YAW] - pmext->mountedWeaponAngles[YAW];

		if( yawDiff > 180 ) {
			yawDiff -= 360;
		} else if( yawDiff < -180 ) {
			yawDiff += 360;
		}

		if( yawDiff > 30 ) {
			ps->viewangles[YAW] = AngleNormalize180( pmext->mountedWeaponAngles[YAW] + 30.f );

			// Set delta_angles properly
			ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
		} else if( yawDiff < -30 ) {
			ps->viewangles[YAW] = AngleNormalize180( pmext->mountedWeaponAngles[YAW] - 30.f );
			
			// Set delta_angles properly
			ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
		}

		// pitch
		pitchDiff = ps->viewangles[PITCH] - pmext->mountedWeaponAngles[PITCH];

		if( pitchDiff > 180 ) {
			pitchDiff -= 360;
		} else if( pitchDiff < -180 ) {
			pitchDiff += 360;
		}

		if( pitchDiff > (pitchMax -10.f) ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->mountedWeaponAngles[PITCH] + (pitchMax -10.f) );

			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		} else if( pitchDiff < -(pitchMax) ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->mountedWeaponAngles[PITCH] - (pitchMax) );
			
			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		}
	}
	else if((ps->eFlags & EF_PRONE) ||
			(ps->eFlags & EF_DEAD) ||
			(ps->eFlags & EF_PLAYDEAD)) {

		//float degsSec = 60.f;
		float /*yaw, */oldYaw;
		trace_t traceres; // rain - renamed
		int newDeltaAngle = ps->delta_angles[YAW];
		float pitchMax = 40.f;
		float yawDiff, pitchDiff;

		//yaw = ps->viewangles[YAW];
		oldYaw = oldViewAngles[YAW];

		/*if ( yaw - oldYaw > 180 ) {
			yaw -= 360;
		}
		if ( yaw - oldYaw < -180 ) {
			yaw += 360;
		}

		if( yaw > oldYaw ) {
			if( yaw - oldYaw > degsSec * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw + degsSec * pml.frametime;

				// Set delta_angles properly
				newDeltaAngle = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		} else if( oldYaw > yaw ) {
			if( oldYaw - yaw > degsSec * pml.frametime ) {
				ps->viewangles[YAW] = oldYaw - degsSec * pml.frametime;

				// Set delta_angles properly
				newDeltaAngle = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}*/

		// Check if we are allowed to rotate to there
		if( ps->weapon == WP_MOBILE_MG42_SET ) {
			pitchMax = 20.f;

			// yaw
			yawDiff = ps->viewangles[YAW] - pmext->mountedWeaponAngles[YAW];

			if( yawDiff > 180 ) {
				yawDiff -= 360;
			} else if( yawDiff < -180 ) {
				yawDiff += 360;
			}

			if( yawDiff > 20 ) {
				ps->viewangles[YAW] = AngleNormalize180( pmext->mountedWeaponAngles[YAW] + 20.f );

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			} else if( yawDiff < -20 ) {
				ps->viewangles[YAW] = AngleNormalize180( pmext->mountedWeaponAngles[YAW] - 20.f );
				
				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}
#ifdef __EF__      //Masteries added PTRS code
		if( ( ps->weapon == WP_30CAL_SET ) || (ps->weapon == WP_PTRS_SET ) ) {
			pitchMax = 20.f;

			// yaw
			yawDiff = ps->viewangles[YAW] - pmext->mountedWeaponAngles[YAW];

			if( yawDiff > 180 ) {
				yawDiff -= 360;
			} else if( yawDiff < -180 ) {
				yawDiff += 360;
			}

			if( yawDiff > 20 ) {
				ps->viewangles[YAW] = AngleNormalize180( pmext->mountedWeaponAngles[YAW] + 20.f );

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			} else if( yawDiff < -20 ) {
				ps->viewangles[YAW] = AngleNormalize180( pmext->mountedWeaponAngles[YAW] - 20.f );
				
				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
		}
#endif //__EF__

		// pitch
		pitchDiff = ps->viewangles[PITCH] - pmext->mountedWeaponAngles[PITCH];

		if( pitchDiff > 180 ) {
			pitchDiff -= 360;
		} else if( pitchDiff < -180 ) {
			pitchDiff += 360;
		}

		if( pitchDiff > pitchMax ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->mountedWeaponAngles[PITCH] + pitchMax );

			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		} else if( pitchDiff < -pitchMax ) {
			ps->viewangles[PITCH] = AngleNormalize180( pmext->mountedWeaponAngles[PITCH] - pitchMax );
			
			// Set delta_angles properly
			ps->delta_angles[PITCH] = ANGLE2SHORT(ps->viewangles[PITCH]) - cmd->angles[PITCH];
		}

		// Check if we rotated into a wall with our legs, if so, undo yaw
		if( ps->viewangles[YAW] != oldYaw ) {
			// see if we have the space to go prone
			// we know our main body isn't in a solid, check for our legs

			ps->delta_angles[YAW] = newDeltaAngle;

			// rain - bugfix - use supplied trace - pm may not be set
			PM_TraceLegs( &traceres, &pmext->proneLegsOffset, ps->origin, ps->origin, NULL, ps->viewangles, pm->trace, ps->clientNum, tracemask );

			if(traceres.fraction < 1.0f) {
				// starting in a solid, no space
				ps->viewangles[YAW] = oldYaw;
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->viewangles[YAW]) - cmd->angles[YAW];
			}
			
			memset(&traceres, 0, sizeof(traceres));

			PM_TraceHead(&traceres, ps->origin, ps->origin,
				NULL, ps->viewangles, pm->trace, ps->clientNum,
				tracemask );

			if(traceres.fraction < 1.0f) {
				ps->viewangles[YAW] = oldYaw;
				ps->delta_angles[YAW] = 
					ANGLE2SHORT(ps->viewangles[YAW]) 
					- cmd->angles[YAW];
			} 

		}
	}

	tpm.trace = trace;
//	tpm.trace (&trace, start, tmins, tmaxs, end, ps->clientNum, MASK_PLAYERSOLID);

	PM_UpdateLean( ps, cmd, &tpm );
}

#ifdef __VEHICLES__
/*
========================================
PM_UpdateViewAnglesVehicle

This can be used as another entry point when only the viewangles
are being updated isntead of a full move
========================================
*/
void PM_UpdateViewAnglesVehicle( playerState_t *ps, pmoveExt_t *pmext, usercmd_t *cmd, void (trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask ), int tracemask ) {     //----(SA)      modified
	short	temp;
	int		i;
	vec3_t oldViewAngles;
	float oldTurretYaw;
	float yaw, oldYaw;
	float degsSec = 150; // fixme do some lower number but allow faster turret

	// DHM - Nerve :: Added support for PMF_TIME_LOCKPLAYER
	if ( ps->pm_type == PM_INTERMISSION || ps->pm_flags & PMF_TIME_LOCKPLAYER  ) {
		return;		// no view changes at all
	}

	
	VectorCopy( ps->viewangles, oldViewAngles );
	oldTurretYaw =  ps->stats[STAT_DEAD_YAW];

	// circularly clamp the angles with deltas
	for (i=0 ; i<3 ; i++) {

		temp = cmd->angles[i] + ps->delta_angles[i];
		if ( i == PITCH ) {
			// don't let the player look up or down more than 90 degrees
			if ( temp > 16000 ) {
				ps->delta_angles[i] = 16000 - cmd->angles[i];
				temp = 16000;
			} else if ( temp < -16000 ) {
				ps->delta_angles[i] = -16000 - cmd->angles[i];
				temp = -16000;
			}
		}
		if ( i == YAW ){
			int angle;

			angle =	SHORT2ANGLE( temp );

			if ( cmd->forwardmove < 0 )
		ps->viewangles[i] += (cmd->rightmove * 0.5f) * pml.frametime;
			else/* if ( cmd->forwardmove > 0 ) */
		ps->viewangles[i] += (cmd->rightmove * -0.5f) * pml.frametime;
			// panzer barrel and all self-propelled artillery like stug 3, limit horizontal move  MASTERIES 
		if ( (ps->persistant[PERS_TANKUSED] == VEHICLE_TYPE_PANZER) || (ps->persistant[PERS_TANKUSED] == VEHICLE_TYPE_STUG3) || (ps->persistant[PERS_TANKUSED] == VEHICLE_TYPE_FLAK88) || (ps->persistant[PERS_TANKUSED] == VEHICLE_TYPE_PAK) ){   // works OK
			// stolen from MG code
			float arcMin, arcDiff, arcMax;

			arcMin = arcMax = 25;
			arcDiff = AngleNormalize180( angle - ps->viewangles[YAW] );

			if( arcDiff > arcMin ) {
				angle = AngleNormalize180( ps->viewangles[YAW] + arcMin );
				ps->delta_angles[YAW] = ANGLE2SHORT(angle) - cmd->angles[YAW];
			} else if( arcDiff < -arcMax ) {
				angle = AngleNormalize180( ps->viewangles[YAW] - arcMax );
				ps->delta_angles[YAW] = ANGLE2SHORT(angle) - cmd->angles[YAW];
			}
		}

		ps->stats[STAT_DEAD_YAW] = angle;

		} else {
		ps->viewangles[i] = SHORT2ANGLE(temp);
		}
	}
		
//////// TURRET /////////

		yaw = ps->stats[STAT_DEAD_YAW];
		oldYaw = oldTurretYaw;

		if ( yaw - oldYaw > 180 ) {
			yaw -= 360;
		}
		if ( yaw - oldYaw < -180 ) {
			yaw += 360;
		}


		if( yaw > oldYaw ) {
			if( yaw - oldYaw > degsSec * pml.frametime ) {
				ps->stats[STAT_DEAD_YAW] = oldYaw + degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->stats[STAT_DEAD_YAW]) - cmd->angles[YAW];
			}
		} else if( oldYaw > yaw ) {
			if( oldYaw - yaw > degsSec * pml.frametime ) {
				ps->stats[STAT_DEAD_YAW] = oldYaw - degsSec * pml.frametime;

				// Set delta_angles properly
				ps->delta_angles[YAW] = ANGLE2SHORT(ps->stats[STAT_DEAD_YAW]) - cmd->angles[YAW];
			}
		}


cmd->rightmove=0;

}
#endif

/*
================
PM_CheckLadderMove

  Checks to see if we are on a ladder
================
*/
qboolean	ladderforward;
vec3_t		laddervec;

//
// Unique's climbing out of water!
//

#ifdef __ETE__
extern float VectorDistanceNoHeight ( vec3_t v1, vec3_t v2 );

qboolean PM_UQ_Climbable ( vec3_t org, vec3_t forward, vec3_t right, int ignoreEnt, float trace_dist, qboolean already_on_ladder, qboolean inWater )
{
	vec3_t		org1, org2;
	qboolean	canClimb = qfalse;
	float		last_distance = 99999.9f;
	//qboolean	same_dist_found = qfalse;
	int			same_dist_found = 0;
	float		top_height = 0;
	trace_t		tr;

#ifdef QAGAME
	if (g_entities[ignoreEnt].bot_no_ladder_use_time > level.time)
		return qfalse; // To stop bot's getting stuck half way...
#endif //QAGAME

	VectorCopy(org, org1);
	VectorCopy(org, org2);

	VectorMA( org2, 64/*128*/, forward, org2 );

	org1[2]+=8;
	org2[2]+=8;
	pm->trace( &tr, org1, NULL, NULL, org2, ignoreEnt, /*MASK_ALL*/MASK_PLAYERSOLID & ~CONTENTS_BODY );

	if (!already_on_ladder && !inWater && tr.fraction == 1)
	{
/*#ifndef CGAMEDLL
		G_Printf(va("tr.fraction == 1!\n"));
#else //CGAMEDLL
		CG_Printf(va("tr.fraction == 1!\n"));
#endif //CGAMEDLL*/
		return qfalse;
	}

	if (!already_on_ladder && !inWater && tr.entityNum >= 0 && tr.entityNum <= ENTITYNUM_MAX_NORMAL)
	{
#ifdef CGAMEDLL
		switch (cg_entities[tr.entityNum].currentState.eType)
#else //!CGAMEDLL
		switch (g_entities[tr.entityNum].s.eType)
#endif //CGAMEDLL
		{
		case ET_MOVER:
		case ET_MOVERSCALED:
		case ET_CABINET_H:
		case ET_CABINET_A:
		case ET_HEALER:
		case ET_SUPPLIER:
#ifdef __ETE__
		case ET_FLAG:
		case ET_AMMO_CRATE:
		case ET_HEALTH_CRATE:
		case ET_SANDBAG:
		case ET_CONSTRUCTIBLE_SANDBAGS:
		case ET_MG_NEST:
		case ET_CONSTRUCTIBLE_MG_NEST:
			return qfalse; // Can not climb these!
		case ET_PLAYER: // UQ1: Can climb over players in a vehicle!
#ifdef CGAMEDLL
			if (!(cg_entities[tr.entityNum].currentState.eFlags & EF_VEHICLE))
#else //!CGAMEDLL
			if (!(g_entities[tr.entityNum].s.eFlags & EF_VEHICLE))
#endif //CGAMEDLL
				return qfalse;
#endif //__ETE__
		default:
			break;
		}
	}

	VectorMA( org2, 32, right, org2 );
	VectorMA( org1, 32, right, org1 );

	pm->trace( &tr, org1, NULL, NULL, org2, ignoreEnt, /*MASK_ALL*/MASK_PLAYERSOLID & ~CONTENTS_BODY );

	if (!already_on_ladder && !inWater && tr.fraction == 1)
	{
/*#ifndef CGAMEDLL
		G_Printf(va("tr.fraction == 1!\n"));
#else //CGAMEDLL
		CG_Printf(va("tr.fraction == 1!\n"));
#endif //CGAMEDLL*/
		return qfalse;
	}

	VectorMA( org2, -64, right, org2 );
	VectorMA( org1, -64, right, org1 );

	pm->trace( &tr, org1, NULL, NULL, org2, ignoreEnt, /*MASK_ALL*/MASK_PLAYERSOLID & ~CONTENTS_BODY );

	if (!already_on_ladder && !inWater && tr.fraction == 1)
	{
/*#ifndef CGAMEDLL
		G_Printf(va("tr.fraction == 1!\n"));
#else //CGAMEDLL
		CG_Printf(va("tr.fraction == 1!\n"));
#endif //CGAMEDLL*/
		return qfalse;
	}

	VectorCopy(org, org1);
	VectorCopy(org, org2);

	VectorMA( org2, trace_dist, forward, org2 );

	while (org1[2] < org[2]+128)
	{
		float		distance;

		org1[2]+=8;
		org2[2]+=8;

		// First check that the new org1 is reachable from where we are!
		pm->trace( &tr, org, pm->mins, pm->maxs, org1, ignoreEnt, MASK_PLAYERSOLID & ~CONTENTS_BODY );

		if (tr.fraction != 1)
		{// Solid above, cant climb here!
			canClimb = qfalse;
			break;
		}

		pm->trace( &tr, org1, pm->mins, pm->maxs, org2, ignoreEnt, MASK_PLAYERSOLID & ~CONTENTS_BODY );

		distance = tr.fraction;//VectorDistanceNoHeight(tr.endpos, org1);
		
		if (distance == last_distance && distance != 1)
		{// Found 2 distances the same, its a wall, not a slope!
			//same_dist_found = qtrue;
			same_dist_found++;
		}
		else if (distance != last_distance && distance != 1)
		{
			same_dist_found = 0;
		}

		//Com_Printf("BG: Trace from %f %f %f -> %f %f %f --- Fraction is %f. (same_dist: %i)\n", org1[0], org1[1], org1[2], org2[0], org2[1], org2[2], tr.fraction, (int)same_dist_found);

		if (tr.fraction == 1 && (tr.entityNum > ENTITYNUM_MAX_NORMAL || tr.entityNum == ENTITYNUM_NONE))
		{
			if (same_dist_found > 1 || already_on_ladder)
			{
				vec3_t slopeangles;
				float pitch;

				// Added -- Check slope...
				vectoangles( tr.plane.normal, slopeangles );

				pitch = slopeangles[0];
	
				if (pitch > 180)
					pitch -= 360;

				if (pitch < -180)
					pitch += 360;

				pitch += 90.0f;

				if ( !already_on_ladder && !inWater && ((pitch > 0 && (pitch > 185.0f || pitch < 175.0f)) || (pitch < 0 && (pitch < -185.0f || pitch > -175.0f))) )
				{// bad slope... not ~90 degrees...
/*#ifndef CGAMEDLL
					G_Printf(va("slope is %f.\n", pitch));
#else //CGAMEDLL
					CG_Printf(va("slope is %f.\n", pitch));
#endif //CGAMEDLL*/
					canClimb = qfalse;
					break; 
				}

				if (already_on_ladder || org1[2] >= org[2]+24 || inWater)
				{// Can climb it!
					canClimb = qtrue;
					top_height = org2[2];
				}
			}

			break;
		}

		last_distance = distance;
	}

	if (canClimb && !inWater && !already_on_ladder)
	{
		if (top_height - org[2] <= 56)
			return qfalse; // Can jump this.. No need for climbing!
	}

	return canClimb;
}
#endif //__ETE__

void PM_CheckLadderMove (void) {
	vec3_t	spot;
	vec3_t	flatforward;
	trace_t	trace;
	float	tracedist;
	#define	TRACE_LADDER_DIST	48.0
	qboolean wasOnLadder;
#ifdef __ETE__
	qboolean is_climbable = qfalse;
#endif //__ETE__
	
	if (pm->ps->pm_time)
		return;

	//if (pm->ps->pm_flags & PM_DEAD)
	//	return;

	if (pml.walking) {
		tracedist = 1.0;
	} else {
		tracedist = TRACE_LADDER_DIST;
	}

	wasOnLadder = ((pm->ps->pm_flags & PMF_LADDER) != 0);

	pml.ladder = qfalse;
	pm->ps->pm_flags &= ~PMF_LADDER;	// clear ladder bit
	ladderforward = qfalse;

	/*
	if (pm->ps->eFlags & EF_DEAD) {	// dead bodies should fall down ladders
		return;
	}

	if (pm->ps->pm_flags & PM_DEAD && pm->ps->stats[STAT_HEALTH] <= 0)
	{
		return;
	}
	*/
	if (pm->ps->stats[STAT_HEALTH] <= 0)
	{
		pm->ps->groundEntityNum = ENTITYNUM_NONE;
		pml.groundPlane = qfalse;
		pml.walking = qfalse;
		return;
	}

	// Can't climb ladders while prone
	if( pm->ps->eFlags & EF_PRONE ) {
		return;
	}

	// check for ladder
	flatforward[0] = pml.forward[0];
	flatforward[1] = pml.forward[1];
	flatforward[2] = 0;
	VectorNormalize (flatforward);

	VectorMA (pm->ps->origin, tracedist, flatforward, spot);
	pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, spot, pm->ps->clientNum, pm->tracemask);
	if ((trace.fraction < 1) && (trace.surfaceFlags & SURF_LADDER))
	{
		pml.ladder = qtrue;
	}

#ifdef __ETE__
	if (!pml.ladder && trace.fraction < 1 /*&& (trace.contents & CONTENTS_WATER)*/)
	{
		vec3_t offset2, flatright;
		qboolean inWater = qfalse;

		flatright[0] = pml.right[0];
		flatright[1] = pml.right[1];
		flatright[2] = 0;
		VectorNormalize (flatforward);

		VectorCopy( pm->ps->origin, offset2 );
		offset2[2] += 48;
	
		if (pm->watertype == CONTENTS_WATER)
			inWater = qtrue;

		pml.ladder = PM_UQ_Climbable( pm->ps->origin, flatforward, flatright, pm->ps->clientNum, tracedist+32, wasOnLadder, inWater );

		if (pml.ladder)
		{
			is_climbable = qtrue;
		}
	}
#endif //__ETE__
/*
	if (!pml.ladder && DotProduct(pm->ps->velocity, pml.forward) < 0) {
		// trace along the negative velocity, so we grab onto a ladder if we are trying to reverse onto it from above the ladder
		flatforward[0] = -pm->ps->velocity[0];
		flatforward[1] = -pm->ps->velocity[1];
		flatforward[2] = 0;
		VectorNormalize (flatforward);

		VectorMA (pm->ps->origin, tracedist, flatforward, spot);
		pm->trace (&trace, pm->ps->origin, pm->mins, pm->maxs, spot, pm->ps->clientNum, pm->tracemask);
		if ((trace.fraction < 1) && (trace.surfaceFlags & SURF_LADDER))
		{
			pml.ladder = qtrue;
		}
	}
*/
	if (pml.ladder) {
		VectorCopy( trace.plane.normal, laddervec );
	}

	if (pml.ladder && !pml.walking && (trace.fraction * tracedist > 1.0)) {
		vec3_t mins;
		// if we are only just on the ladder, don't do this yet, or it may throw us back off the ladder
		pml.ladder = qfalse;
		VectorCopy( pm->mins, mins );
		mins[2] = -1;
		VectorMA (pm->ps->origin, -tracedist, laddervec, spot);
		pm->trace (&trace, pm->ps->origin, mins, pm->maxs, spot, pm->ps->clientNum, pm->tracemask);
		
#ifdef __ETE__
		if ((trace.fraction < 1) && ((trace.surfaceFlags & SURF_LADDER) || is_climbable)) 
#else //!__ETE__
		if ((trace.fraction < 1) && (trace.surfaceFlags & SURF_LADDER)) 
#endif //__ETE__
		{
			ladderforward = qtrue;
			pml.ladder = qtrue;
			pm->ps->pm_flags |= PMF_LADDER;	// set ladder bit
		} else {
			pml.ladder = qfalse;
		}
	} else if (pml.ladder) {
		pm->ps->pm_flags |= PMF_LADDER;	// set ladder bit
	}

	// create some up/down velocity if touching ladder
	if ( pml.ladder )
	{
		if (pml.walking) {
			// we are currently on the ground, only go up and prevent X/Y if we are pushing forwards
			if (pm->cmd.forwardmove <= 0) {
				pml.ladder = qfalse;
			}
		}
	}

	// if we have just dismounted the ladder at the top, play dismount
	if (!pml.ladder && wasOnLadder && pm->ps->velocity[2] > 0) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_CLIMB_DISMOUNT, qfalse, qfalse );
	}
	// if we have just mounted the ladder
#ifdef __ETE__
	if (pml.ladder && !wasOnLadder && pm->ps->velocity[2] < 0 && !is_climbable) {	// only play anim if going down ladder
#else //!__ETE__
	if (pml.ladder && !wasOnLadder && pm->ps->velocity[2] < 0) {	// only play anim if going down ladder
#endif //__ETE__
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_CLIMB_MOUNT, qfalse, qfalse );
	}
}

/*
============
PM_LadderMove
============
*/
void PM_LadderMove (void) {
	float	wishspeed, scale;
	vec3_t	wishdir, wishvel;
	float	upscale;

	if (ladderforward) {
		// move towards the ladder
		VectorScale( laddervec, -200.0, wishvel );
		pm->ps->velocity[0] = wishvel[0];
		pm->ps->velocity[1] = wishvel[1];
	}

	upscale = (pml.forward[2] + 0.5)*2.5;
	if (upscale > 1.0)
		upscale = 1.0;
	else if (upscale < -1.0)
		upscale = -1.0;

	// forward/right should be horizontal only
	pml.forward[2] = 0;
	pml.right[2] = 0;
	VectorNormalize (pml.forward);
	VectorNormalize (pml.right);

	// move depending on the view, if view is straight forward, then go up
	// if view is down more then X degrees, start going down
	// if they are back pedalling, then go in reverse of above
	scale = PM_CmdScale( &pm->cmd );
	VectorClear( wishvel );

	if( pm->cmd.forwardmove ) {
		wishvel[2] = 0.9 * upscale * scale * (float)pm->cmd.forwardmove;
	}
//Com_Printf("wishvel[2] = %i, fwdmove = %i\n", (int)wishvel[2], (int)pm->cmd.forwardmove );

	if( pm->cmd.rightmove ) {
		// strafe, so we can jump off ladder
		vec3_t	ladder_right, ang;
		vectoangles( laddervec, ang );
		AngleVectors( ang, NULL, ladder_right, NULL );

		// if we are looking away from the ladder, reverse the right vector
		if( DotProduct( laddervec, pml.forward ) < 0 )
			VectorInverse( ladder_right );

		//VectorMA( wishvel, 0.5 * scale * (float)pm->cmd.rightmove, pml.right, wishvel );
		VectorMA( wishvel, 0.5 * scale * (float)pm->cmd.rightmove, ladder_right, wishvel );
	}

	// do strafe friction
	PM_Friction();

	if( pm->ps->velocity[0] < 1 && pm->ps->velocity[0] > -1  )
		pm->ps->velocity[0] = 0;
	if( pm->ps->velocity[1] < 1&& pm->ps->velocity[1] > -1 )
		pm->ps->velocity[1] = 0;

	wishspeed = VectorNormalize2( wishvel, wishdir );

	PM_Accelerate( wishdir, wishspeed, pm_accelerate );
	if( !wishvel[2] )
	{
		if (pm->ps->velocity[2] > 0)
		{
			pm->ps->velocity[2] -= pm->ps->gravity * pml.frametime;
			if (pm->ps->velocity[2] < 0)
				pm->ps->velocity[2]  = 0;
		}
		else
		{
			pm->ps->velocity[2] += pm->ps->gravity * pml.frametime;
			if (pm->ps->velocity[2] > 0)
				pm->ps->velocity[2]  = 0;
		}
	}

//Com_Printf("vel[2] = %i\n", (int)pm->ps->velocity[2] );

	PM_StepSlideMove (qfalse);	// no gravity while going up ladder

	// always point legs forward
	pm->ps->movementDir = 0;
}


/*
==============
PM_Sprint
==============
*/
void PM_Sprint( void ) {
	int rechargebase;

	if (pm->cmd.buttons & BUTTON_SPRINT && (pm->cmd.forwardmove || pm->cmd.rightmove) && !(pm->ps->pm_flags & PMF_DUCKED) && !(pm->ps->eFlags & EF_PRONE) ) {
		if( pm->ps->powerups[PW_ADRENALINE] ) {
			pm->pmext->sprintTime = SPRINTTIME;
		} else if(pm->ps->powerups[PW_NOFATIGUE]) {
			// take time from powerup before taking it from sprintTime
			pm->ps->powerups[PW_NOFATIGUE] -= 50;

			// (SA) go ahead and continue to recharge stamina at double 
			// rate with stamina powerup even when exerting
			pm->pmext->sprintTime += 10;
			if (pm->pmext->sprintTime > SPRINTTIME)
				pm->pmext->sprintTime = SPRINTTIME;

			if(pm->ps->powerups[PW_NOFATIGUE] < 0)
				 pm->ps->powerups[PW_NOFATIGUE] = 0;
		}
		// JPW NERVE -- sprint time tuned for multiplayer
		else  {
			// JPW NERVE adjusted for framerate independence
			pm->pmext->sprintTime -= 5000*pml.frametime;
		}
		// jpw

		if (pm->pmext->sprintTime < 0)
			pm->pmext->sprintTime = 0;
		
		if (!pm->ps->sprintExertTime)
		{
			pm->ps->sprintExertTime = 1;
		}
	}
	else 
	{	
		// JPW NERVE -- in multiplayer, recharge faster for top 75% of sprint bar 
		// (for people that *just* use it for jumping, not sprint) this code was 
		// mucked about with to eliminate client-side framerate-dependancy in wolf single player
		if( pm->ps->powerups[PW_ADRENALINE] ) {
			pm->pmext->sprintTime = SPRINTTIME;
		} else if(pm->ps->powerups[PW_NOFATIGUE]) {	// (SA) recharge at 2x with stamina powerup
			pm->pmext->sprintTime += 10;
		} else {
				rechargebase = (float)(500 * PM_STAMINA_RECHARGE);

#ifdef GAMEDLL // Gordon: FIXME: predict leadership clientside
			if( pm->leadership ) {
				rechargebase = (float)(500 * PM_STAMINA_RECHARGE) + 500;
			} else 
#endif // GAMEDLL
			{
				if( pm->skill[SK_BATTLE_SENSE] >= 2 )
					rechargebase *= 1.6f;
			}

			pm->pmext->sprintTime += rechargebase*pml.frametime;		// JPW NERVE adjusted for framerate independence
			if (pm->pmext->sprintTime > 5000)
				pm->pmext->sprintTime += rechargebase*pml.frametime;	// JPW NERVE adjusted for framerate independence
			// jpw
		}
		if (pm->pmext->sprintTime > SPRINTTIME) {
			pm->pmext->sprintTime = SPRINTTIME;
		}

		pm->ps->sprintExertTime = 0;
	}

}

#ifdef __ADVANCED_VEHICLES__
extern void *BG_Alloc ( int size );

car_t *vehicles[MAX_CLIENTS];
qboolean vehicles_initialized = qfalse;

void Init_Vehicles ( void )
{
	if (vehicles_initialized)
		return;

	vehicles_initialized = qtrue;

	*vehicles = BG_Alloc((sizeof(car_t)*MAX_CLIENTS)+1);
}
#endif //__ADVANCED_VEHICLES__

qboolean BG_CanSetupMobileMG42Here ( void )
{// UQ1: Testing to see if we can mount mg42 on objects while standing...
	trace_t				trace;
	vec3_t				angles, forward, muzzlePoint, start, end;

	VectorCopy(pm->ps->viewangles, angles);
	angles[PITCH] = 0;

	AngleVectors( angles, forward, NULL, NULL );

	VectorCopy( pm->ps->origin, muzzlePoint );
	//muzzlePoint[2] += pm->ps->viewheight;
	if( pm->ps->eFlags & EF_CROUCHING )
		muzzlePoint[2] += 10;
	else
		muzzlePoint[2] += 24;

	VectorMA( muzzlePoint, 24, forward, start );

	VectorCopy(start, end);
	end[2]-=64;

#ifdef CGAMEDLL
	CG_Trace( &trace, start, NULL, NULL, end, pm->ps->clientNum, MASK_SHOT );
#else //!CGAMEDLL
	trap_Trace( &trace, start, NULL, NULL, end, pm->ps->clientNum, MASK_SHOT );
#endif //CGAMEDLL

	if (trace.allsolid)
		return qfalse;

	if (trace.startsolid)
		return qfalse;

	if (trace.fraction == 1)
		return qfalse;

	if (VectorDistance(trace.endpos, start) < 16)
		return qtrue;

	return qfalse;
}

/*
================
PmoveSingle

================
*/
void trap_SnapVector( float *v );

void PmoveSingle (pmove_t *pmove) {
#ifdef __ADVANCED_VEHICLES__
	Init_Vehicles();
#endif //__ADVANCED_VEHICLES__

/*#ifdef __NPC__
	if ( pmove->isNPC )
	{
		pmove->character = BG_GetCharacterForPlayerstate( pmove->ps );
		pmove->character->animModelInfo = BG_GetCharacterForPlayerstate( pmove->ps )->animModelInfo;
	}
#endif //__NPC__*/

#ifdef __VEHICLES__
	if (pm->ps->eFlags & EF_VEHICLE)
	{
		if (!pm->tankbox_setup)
		{
			VectorCopy(pm->ps->mins, pm->original_mins);
			VectorCopy(pm->ps->maxs, pm->original_maxs);
			pm->tankbox_setup = qtrue;
		}

#ifdef GAMEDLL
		if ((g_entities[pm->ps->clientNum].r.svFlags & SVF_BOT) && g_entities[pm->ps->clientNum].vehicle_bbox_cheat_time > level.time)
		{// Cheat for bots in tanks when stuck... NULL bbox...
			vec3_t miniMins, miniMaxs;
			VectorSet(miniMins, 0, 0, -24);
			VectorSet(miniMaxs, 0, 0, 78);
			VectorCopy(miniMins, pm->ps->mins);
			VectorCopy(miniMaxs, pm->ps->maxs);
		}
		else
#endif //GAMEDLL
		{
			VectorCopy(tankMins, pm->ps->mins);
			VectorCopy(tankMaxs, pm->ps->maxs);
		}
	}
	else
	{
		if (!pm->tankbox_setup)
		{
			VectorCopy(pm->ps->mins, pm->original_mins);
			VectorCopy(pm->ps->maxs, pm->original_maxs);
			pm->tankbox_setup = qtrue;
		}

		VectorCopy(pm->original_mins, pm->ps->mins);
		VectorCopy(pm->original_maxs, pm->ps->maxs);
	}
#endif //__VEHICLES__

	// RF, update conditional values for anim system
	BG_AnimUpdatePlayerStateConditions( pmove );

	pm = pmove;

	// this counter lets us debug movement problems with a journal
	// by setting a conditional breakpoint fot the previous frame
	c_pmove++;

	// clear results
	pm->numtouch = 0;
	pm->watertype = 0;
	pm->waterlevel = 0;

	if ( pm->ps->stats[STAT_HEALTH] <= 0 ) {
		pm->tracemask &= ~CONTENTS_BODY;	// corpses can fly through bodies
		pm->ps->eFlags &= ~EF_ZOOMING;
	}


	// make sure walking button is clear if they are running, to avoid
	// proxy no-footsteps cheats
	if ( abs( pm->cmd.forwardmove ) > 64 || abs( pm->cmd.rightmove ) > 64 ) {
		pm->cmd.buttons &= ~BUTTON_WALKING;
	}

	// set the talk balloon flag
	if ( pm->cmd.buttons & BUTTON_TALK ) {
		pm->ps->eFlags |= EF_TALK;
	} else {
		pm->ps->eFlags &= ~EF_TALK;
	}

	// set the firing flag for continuous beam weapons

	pm->ps->eFlags &= ~(EF_FIRING | EF_ZOOMING);

#if defined (__FRONTLINE__) /*|| defined (__EF__)*/
	if ( !(pm->ps->eFlags & EF_PRONE) ) { 
		if ( pm->ps->eFlags & EF_TALK ) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_TALK, qtrue, qfalse );
		pm->ps->pm_flags |= PMF_RADIO;
		}
		if ( !(pm->ps->eFlags & EF_TALK) && pm->ps->pm_flags & PMF_RADIO) {
		BG_AnimScriptEvent( pm->ps, pm->character->animModelInfo, ANIM_ET_DROP, qfalse, qtrue );
		pm->ps->pm_flags &= ~PMF_RADIO;
		}
	}

////////////////////////////////////////////////////////////////////////
// Binoculars and scoped weapons

	if( pm->cmd.wbuttons & WBUTTON_ZOOM ) {
				if(!BG_PlayerMounted(pm->ps->eFlags)) {
					if ( pm->ps->weapon ==  WP_BINOCULARS ) {
					pm->ps->eFlags |= EF_ZOOMING;
								}
				}

	}
/////////////////////////////////////////////////////////////////////////
// Ironsights // TODO: persistant ironsights ?

	if( pm->ps->weaponstate != WEAPON_RELOADING
		&& ( (pm->cmd.wbuttons & WBUTTON_ZOOM )/*||( pm->cmd.wbuttons & WBUTTON_ATTACK2 && !(weapAlts[pm->ps->weapon] ))*/)
		&& pm->ps->stats[STAT_HEALTH] >= 0 
		&& !(pm->ps->eFlags & EF_AIMING) 
		&& !(pm->ps->eFlags & EF_VEHICLE) 
		&& !(pm->ps->eFlags & EF_PRONE_MOVING) 
		&& pm->cmd.serverTime - -pm->pmext->adsTime > 500 ) {
			if(!BG_IsScopedWeapon(pm->ps->weapon)) {	// don't allow binocs if using the sniper scope		
				if(BG_IsAdsWeapon(pm->ps->weapon) 
#ifndef __EF__
					|| pm->ps->eFlags & EF_BROWNING_ACTIVE
#endif //__EF__
					|| pm->ps->eFlags & EF_MG42_ACTIVE 
					|| pm->ps->eFlags & EF_MOUNTEDTANK )   {
					pm->ps->eFlags |= EF_AIMING;
					pm->pmext->adsTime = pm->cmd.serverTime;
			} 
		} 
	} 
	
	if ( pm->ps->eFlags & EF_AIMING && pm->cmd.wbuttons & WBUTTON_ZOOM  && pm->cmd.serverTime - pm->pmext->adsTime > 500 ) { // manual turning off
		pm->ps->eFlags &= ~EF_AIMING;
		pm->pmext->adsTime = -pm->cmd.serverTime;
	}

	if ( /*pm->ps->eFlags & EF_MG42_ACTIVE || pm->ps->eFlags & EF_BROWNING_ACTIVE ||*/ pm->ps->eFlags & EF_AAGUN_ACTIVE ){ // turn it off on mounted guns... , one less bug :)
		pm->ps->eFlags &= ~EF_AIMING;
		pm->pmext->adsTime = -pm->cmd.serverTime;
	}
	if ( !(BG_IsAdsWeapon(pm->ps->weapon)) 
		&& !(pm->ps->eFlags & EF_MG42_ACTIVE) 
#ifndef __EF__
		&& !(pm->ps->eFlags & EF_BROWNING_ACTIVE) 
#endif //__EF__
		&& !(pm->ps->eFlags & EF_MOUNTEDTANK)) { 
		pm->ps->eFlags &= ~EF_AIMING;
		pm->pmext->adsTime = -pm->cmd.serverTime;
	} 

	if( pm->ps->stats[STAT_HEALTH] < 1 || pm->ps->weaponstate == WEAPON_RELOADING || pm->ps->weaponstate == WEAPON_DROPPING || pm->ps->weaponstate == WEAPON_RAISING || pm->ps->eFlags & EF_VEHICLE || pm->ps->eFlags & EF_PRONE_MOVING || pm->ps->pm_flags & PMF_TIME_LOCKPLAYER ){ // these situations turn off ironsights
		pm->ps->eFlags &= ~EF_AIMING;
		pm->pmext->adsTime = -pm->cmd.serverTime;
	}
#else //!defined (__FRONTLINE__) /*|| defined (__EF__)*/
	if( pm->cmd.wbuttons & WBUTTON_ZOOM && pm->ps->stats[STAT_HEALTH] >= 0 && !( pm->ps->weaponDelay ) ) {
		if(pm->ps->stats[STAT_KEYS] & (1<<INV_BINOCS)) {	// (SA) binoculars are an inventory item (inventory==keys)
			if (!BG_IsScopedWeapon(pm->ps->weapon) &&		// don't allow binocs if using the sniper scope
				!BG_PlayerMounted(pm->ps->eFlags) &&		// or if mounted on a weapon
				// rain - #215 - don't allow binocs w/ mounted mob. MG42 or mortar either.
				pm->ps->weapon != WP_MOBILE_MG42_SET &&	
#ifdef __EF__
				pm->ps->weapon != WP_30CAL_SET &&
                pm->ps->weapon != WP_PTRS_SET &&
#endif //__EF__
				pm->ps->weapon != WP_MORTAR_SET)
			{
				pm->ps->eFlags |= EF_ZOOMING;
			}
#ifdef __WEAPON_AIM__	// fix all here please
			else if (!(pm->ps->eFlags & EF_AIMING)) {	// don't allow binocs if using the sniper scope
				if (!BG_PlayerMounted(pm->ps->eFlags)) {	// or if mounted on a weapon
					pm->ps->eFlags |= EF_AIMING;
				}
			}
			else if (pm->ps->eFlags & EF_AIMING) {	// don't allow binocs if using the sniper scope
				if (!BG_PlayerMounted(pm->ps->eFlags)) {	// or if mounted on a weapon
					pm->ps->eFlags &= ~EF_AIMING;
				}
			}
#endif //__WEAPON_AIM__
		}

		// don't allow binocs if in the middle of throwing grenade
		if( (pm->ps->weapon == WP_GRENADE_LAUNCHER || 
			pm->ps->weapon == WP_GRENADE_PINEAPPLE || 
			pm->ps->weapon == WP_DYNAMITE) && 
				pm->ps->grenadeTimeLeft > 0) {
			pm->ps->eFlags &= ~EF_ZOOMING;
		}
	}

#endif //defined (__FRONTLINE__) /*|| defined (__EF__)*/
	if ( !(pm->ps->pm_flags & PMF_RESPAWNED) &&
		  (pm->ps->pm_type != PM_INTERMISSION) ) {

		// check for ammo
		if(PM_WeaponAmmoAvailable(pm->ps->weapon)) {
			// check if zooming
			// DHM - Nerve :: Let's use the same flag we just checked above, Ok?
			if(!(pm->ps->eFlags & EF_ZOOMING)) {
#if !defined (__FRONTLINE__) && !defined (__EF__)
				if(!pm->ps->leanf) {
#endif //!defined (__FRONTLINE__) && !defined (__EF__)
					if ( pm->ps->weaponstate == WEAPON_READY || pm->ps->weaponstate == WEAPON_FIRING ) {

						// all clear, fire!
						if( pm->cmd.buttons & BUTTON_ATTACK && !(pm->cmd.buttons & BUTTON_TALK) )
							pm->ps->eFlags |= EF_FIRING;
					}
#if !defined (__FRONTLINE__) && !defined (__EF__)
				}
#endif //!defined (__FRONTLINE__) && !defined (__EF__)
			}
		}
	}


	if ( pm->ps->pm_flags & PMF_RESPAWNED ) {
		if( pm->ps->stats[STAT_PLAYER_CLASS] == PC_COVERTOPS ) {
			pm->pmext->silencedSideArm |= 1;
		}
	}

	// clear the respawned flag if attack and use are cleared
	if ( pm->ps->stats[STAT_HEALTH] > 0 && 
		!( pm->cmd.buttons & (BUTTON_ATTACK /*| BUTTON_USE_HOLDABLE*/) ) ) {
		pm->ps->pm_flags &= ~PMF_RESPAWNED;
	}

	// if talk button is down, dissallow all other input
	// this is to prevent any possible intercept proxy from
	// adding fake talk balloons
	if ( pmove->cmd.buttons & BUTTON_TALK ) {
		// keep the talk button set tho for when the cmd.serverTime > 66 msec
		// and the same cmd is used multiple times in Pmove
		pmove->cmd.buttons = BUTTON_TALK;
		pmove->cmd.wbuttons = 0;
		pmove->cmd.forwardmove = 0;
		pmove->cmd.rightmove = 0;
		pmove->cmd.upmove = 0;
		pmove->cmd.doubleTap = 0;
	}

#ifdef __VEHICLES__
	if ( pm->ps->eFlags & EF_VEHICLE  ) 
	{
		pmove->cmd.upmove = 0;
	}
#endif //__VEHICLES__

	// fretn, moved from engine to this very place
	// no movement while using a static mg42
	if (pm->ps->persistant[PERS_HWEAPON_USE])
	{
		pmove->cmd.forwardmove = 0;
		pmove->cmd.rightmove = 0;
		pmove->cmd.upmove = 0;
	}

	// clear all pmove local vars
	memset (&pml, 0, sizeof(pml));

	// determine the time
	pml.msec = pmove->cmd.serverTime - pm->ps->commandTime;
	if ( pml.msec < 1 ) {
		pml.msec = 1;
	} else if ( pml.msec > 200 ) {
		pml.msec = 200;
	}
	pm->ps->commandTime = pmove->cmd.serverTime;

	// save old org in case we get stuck
	VectorCopy (pm->ps->origin, pml.previous_origin);

	// save old velocity for crashlanding
	VectorCopy (pm->ps->velocity, pml.previous_velocity);

	pml.frametime = pml.msec * 0.001;

	// update the viewangles
#ifdef __NPC__
	if ( pm->isNPC )
	{
		PM_UpdateViewAngles( pm->ps, pm->pmext, &pm->cmd, pm->trace, pm->tracemask );	//----(SA)	modified
	}
	else
#endif //__NPC__
	if( pm->ps->pm_type != PM_FREEZE ){	// Arnout: added PM_FREEZE
		if (!(pm->ps->pm_flags & PMF_LIMBO))
		{ // JPW NERVE
        // rain - added tracemask
#ifdef __VEHICLES__
			if ( pm->ps->eFlags & EF_VEHICLE ) 
				PM_UpdateViewAnglesVehicle( pm->ps, pm->pmext, &pm->cmd, pm->trace, pm->tracemask );
			else
#endif
				PM_UpdateViewAngles( pm->ps, pm->pmext, &pm->cmd, pm->trace, pm->tracemask );   //----(SA)      modified
		}
	}

	AngleVectors (pm->ps->viewangles, pml.forward, pml.right, pml.up);

	if ( pm->cmd.upmove < 10 ) {
		// not holding jump
		pm->ps->pm_flags &= ~PMF_JUMP_HELD;
	}

	// decide if backpedaling animations should be used
	if ( pm->cmd.forwardmove < 0 ) {
		pm->ps->pm_flags |= PMF_BACKWARDS_RUN;
	} else if ( pm->cmd.forwardmove > 0 || ( pm->cmd.forwardmove == 0 && pm->cmd.rightmove ) ) {
		pm->ps->pm_flags &= ~PMF_BACKWARDS_RUN;
	}

	if ( pm->ps->pm_type >= PM_DEAD || pm->ps->pm_flags & (PMF_LIMBO|PMF_TIME_LOCKPLAYER) ) {			// DHM - Nerve
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
		pm->cmd.upmove = 0;
	}

	if ( pm->ps->pm_type == PM_SPECTATOR ) {
		PM_CheckDuck ();
		PM_FlyMove ();
		PM_DropTimers ();
		return;
	}

	if ( pm->ps->pm_type == PM_NOCLIP ) {
		PM_NoclipMove ();
		PM_DropTimers ();
		return;
	}

	if (pm->ps->pm_type == PM_FREEZE) {
		return;		// no movement at all
	}

	if ( pm->ps->pm_type == PM_INTERMISSION ) {
		return;		// no movement at all
	}

// STONELANCE - FIXME calculate water levels in bg_physics
	PM_SetWaterLevel();
	pml.previous_waterlevel = pmove->waterlevel;

	// set mins, maxs, and viewheight
#ifdef __ADVANCED_VEHICLES__
//	PM_CheckDuck ();
	VectorSet(pm->mins, -CAR_WIDTH/2, -CAR_WIDTH/2, -CAR_WIDTH/2);
	VectorSet(pm->maxs, CAR_WIDTH/2, CAR_WIDTH/2, CAR_WIDTH/2);
	pm->ps->viewheight = 0;

	// dont do physics if the player has been gibed
	if ( pm->ps->eFlags & EF_VEHICLE && pm->ps->stats[STAT_HEALTH] > GIB_HEALTH )
	{
		float	dot;
		int		i;

		if (!pm->car)
		{// Initialize the memory area!
			pm->car = vehicles[pm->ps->clientNum];			
			pm->car->initializeOnNextMove = qtrue;
			//PM_InitializeVehicle( pm->car, pm->ps->origin, pm->ps->viewangles, pm->ps->velocity );
		}

		pml.physicsSplit = 0;
		PM_DriveMove(pm->car, pml.frametime, qtrue);

		if( VectorNAN( pm->car->sBody.r ) )
			VectorClear( pm->car->sBody.r );
		if( VectorNAN( pm->car->sBody.v ) )
			VectorClear( pm->car->sBody.v );
		if( VectorNAN( pm->car->sBody.L ) )
			VectorClear( pm->car->sBody.L );

		// translate car values to player angles, etc
		VectorCopy(pm->car->sBody.v, pm->ps->velocity);
		VectorCopy(pm->car->sBody.r, pm->ps->origin);
		//VectorCopy(pm->car->sBody.L, pm->ps->angularMomentum);
		VectorCopy(pm->car->sBody.L, pm->ps->velocity);
		OrientationToAngles(pm->car->sBody.t, pm->ps->viewangles);

		if( VectorNAN( pm->ps->viewangles ) )
			VectorClear( pm->ps->viewangles );

//		VectorSubtract(pm->car->sPoints[FL_FRAME].r, pm->car->sPoints[FL_WHEEL].r, delta);
//		dot = DotProduct(delta, pm->car->sBody.up);
		dot = pm->car->sBody.curSpringLengths[FL_WHEEL] - CP_SPRING_MINLEN;
		if (dot > CP_SPRING_MAXLEN - CP_SPRING_MINLEN)
			dot = CP_SPRING_MAXLEN - CP_SPRING_MINLEN;
		else if (dot < 0)
			dot = 0;
		pm->ps->legsTimer = (int)(CP_SPRING_SCALE * dot);

//		VectorSubtract(pm->car->sPoints[FR_FRAME].r, pm->car->sPoints[FR_WHEEL].r, delta);
//		dot = DotProduct(delta, pm->car->sBody.up);
		dot = pm->car->sBody.curSpringLengths[FR_WHEEL] - CP_SPRING_MINLEN;
		if (dot > CP_SPRING_MAXLEN - CP_SPRING_MINLEN)
			dot = CP_SPRING_MAXLEN - CP_SPRING_MINLEN;
		else if (dot < 0)
			dot = 0;
		pm->ps->legsAnim = (int)(CP_SPRING_SCALE * dot);

//		VectorSubtract(pm->car->sPoints[RL_FRAME].r, pm->car->sPoints[RL_WHEEL].r, delta);
//		dot = DotProduct(delta, pm->car->sBody.up);
		dot = pm->car->sBody.curSpringLengths[RL_WHEEL] - CP_SPRING_MINLEN;
		if (dot > CP_SPRING_MAXLEN - CP_SPRING_MINLEN)
			dot = CP_SPRING_MAXLEN - CP_SPRING_MINLEN;
		else if (dot < 0)
			dot = 0;
		pm->ps->torsoTimer = (int)(CP_SPRING_SCALE * dot);

//		VectorSubtract(pm->car->sPoints[RR_FRAME].r, pm->car->sPoints[RR_WHEEL].r, delta);
//		dot = DotProduct(delta, pm->car->sBody.up);
		dot = pm->car->sBody.curSpringLengths[RR_WHEEL] - CP_SPRING_MINLEN;
		if (dot > CP_SPRING_MAXLEN - CP_SPRING_MINLEN)
			dot = CP_SPRING_MAXLEN - CP_SPRING_MINLEN;
		else if (dot < 0)
			dot = 0;
		pm->ps->torsoAnim = (int)(CP_SPRING_SCALE * dot);

//		pm->ps->stats[STAT_RPM] = pm->car->rpm;
//		pm->ps->stats[STAT_GEAR] = pm->car->gear;

		// used to keep track of time since last onGround for resetCar
/*
		VectorSet(delta, 0, 0, 1);
		for (i = 0; i < NUM_CAR_POINTS; i++){
			if (pm->car->sPoints[i].onGround && DotProduct(pm->car->sPoints[i].normals[0], delta) > 0.3){
				pm->car->sPoints[i].onGroundTime = pm->cmd.serverTime;
				pm->car->tPoints[i].onGroundTime = pm->cmd.serverTime;
			}
			else {
				pm->car->sPoints[i].offGroundTime = pm->cmd.serverTime;
				pm->car->tPoints[i].offGroundTime = pm->cmd.serverTime;
			}
		}
*/

		for (i = 0; i < FL_FRAME; i++)
		{
			if( pm->car->sPoints[i].onGround && pm->car->sPoints[i].normals[0][2] > 0.3f )
			{
				pm->car->wheelOnGroundTime = pm->cmd.serverTime;
				break;
			}
		}

		if( pm->car->wheelOnGroundTime != pm->cmd.serverTime )
			pm->car->wheelsOffGroundTime = pm->cmd.serverTime;

		for (i = FL_FRAME; i < NUM_CAR_POINTS; i++)
		{
			if( pm->car->sPoints[i].onGround )
			{
				pm->car->onGroundTime = pm->cmd.serverTime;
				break;
			}
		}

		if( pm->car->onGroundTime < pm->cmd.serverTime - 100 )
			pm->car->offGroundTime = pm->cmd.serverTime;
	}
#endif //__ADVANCED_VEHICLES__

	// ydnar: need gravity etc to affect a player with a set mortar
	if( pm->ps->weapon == WP_MORTAR_SET && pm->ps->pm_type == PM_NORMAL ) {
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
		pm->cmd.upmove = 0;
	}
/*
	// set watertype, and waterlevel
	PM_SetWaterLevel();
	pml.previous_waterlevel = pmove->waterlevel;


	if( !PM_CheckProne() ) {
		PM_CheckDuck();
	}*/

	// set watertype, and waterlevel
  	PM_SetWaterLevel();
  	pml.previous_waterlevel = pmove->waterlevel;
  
 	PM_CheckPlayDead();

  	// set mins, maxs, and viewheight
  	if( !PM_CheckProne() ) {
  		PM_CheckDuck();
  	}

	// set groundentity
	PM_GroundTrace();


	if ( pm->ps->pm_type == PM_DEAD ) {
		PM_DeadMove ();

		if( pm->ps->weapon == WP_MORTAR_SET ) {
			pm->ps->weapon = WP_MORTAR;
		}
	} else {
		if( pm->ps->weapon == WP_MOBILE_MG42_SET ) {
			if( !( pm->ps->eFlags & EF_PRONE ) && !BG_CanSetupMobileMG42Here() ) {
				PM_BeginWeaponChange( WP_MOBILE_MG42_SET, WP_MOBILE_MG42, qfalse );
#ifdef CGAMEDLL
				cg.weaponSelect = WP_MOBILE_MG42;
#endif // CGAMEDLL
			}
		}
#ifdef __EF__
		if( pm->ps->weapon == WP_30CAL_SET ) {
			if( !( pm->ps->eFlags & EF_PRONE ) && !BG_CanSetupMobileMG42Here() ) {
				PM_BeginWeaponChange( WP_30CAL_SET, WP_30CAL, qfalse );
#ifdef CGAMEDLL
				cg.weaponSelect = WP_30CAL;
#endif // CGAMEDLL
			}
		}
#endif //__EF__

//Masteries added PTRS code
#ifdef __EF__
		if( pm->ps->weapon == WP_PTRS_SET ) {
			if( !( pm->ps->eFlags & EF_PRONE ) && !BG_CanSetupMobileMG42Here() ) {
				PM_BeginWeaponChange( WP_PTRS_SET, WP_PTRS, qfalse );
#ifdef CGAMEDLL
				cg.weaponSelect = WP_PTRS;
#endif // CGAMEDLL
			}
		}
#endif //__EF__

		if( pm->ps->weapon == WP_SATCHEL_DET ) {
			if( !( pm->ps->ammoclip[ WP_SATCHEL_DET ] ) ) {
				// tjw: this causes the 2.60 client to freak out
				//PM_BeginWeaponChange( WP_SATCHEL_DET, WP_SATCHEL, qtrue );
#ifdef CGAMEDLL
				if(!(cgs.coverts & COVERTF_NO_SATCHEL_SWITCHBACK)) 
					cg.weaponSelect = WP_SATCHEL;
				else if(CG_WeaponSelectable(WP_STEN))
					cg.weaponSelect = WP_STEN;
				else if(CG_WeaponSelectable(WP_FG42))
					cg.weaponSelect = WP_FG42;
				else if(CG_WeaponSelectable(WP_GARAND))
					cg.weaponSelect = WP_GARAND;
				else if(CG_WeaponSelectable(WP_K43))
					cg.weaponSelect = WP_K43;
				else if(CG_WeaponSelectable(WP_SILENCED_LUGER))
					cg.weaponSelect = WP_SILENCED_LUGER;
				else if(CG_WeaponSelectable(WP_SILENCED_COLT))
					cg.weaponSelect = WP_SILENCED_COLT;
				else if(CG_WeaponSelectable(WP_AKIMBO_SILENCEDLUGER))
					cg.weaponSelect = WP_AKIMBO_SILENCEDLUGER;
				else if(CG_WeaponSelectable(WP_AKIMBO_SILENCEDCOLT))
					cg.weaponSelect = WP_AKIMBO_SILENCEDCOLT;
#endif // CGAMEDLL
			}
		}
	}

	// Ridah, ladders
	PM_CheckLadderMove();

	PM_DropTimers();

#ifdef __VEHICLES__
	if ( pm->ps->eFlags & EF_VEHICLE )
	{
		if (pml.walking) 
		{
			PM_TankMove();
		} 
		else 
		{
			PM_AirMove(); // fixme do tank specific function
		}
	} 
	else 
	{
#endif //__VEHICLES__
		
#ifdef __SHELLSHOCK__
		if ((HasExtFlag(pm->ps->clientNum, EX_SHELLSHOCK)) /*&& (PM_ShellShock & SHELLSHOCK_ENABLED) && (PM_ShellShock & SHELLSHOCK_JITTER)*/)
		{
			if (pm->cmd.rightmove == 0 && pm->cmd.forwardmove)
			{
				if (pml.ladder) {
					pm->cmd.rightmove = crandom() * 24;
				} else if (pm->ps->pm_flags & PMF_TIME_WATERJUMP) {
					pm->cmd.rightmove = crandom() * 32;
				} else if (pm->waterlevel > 1) {
					// swimming
					pm->cmd.rightmove = crandom() * 32;
				} else if (pml.walking && !(pm->ps->eFlags & EF_MOUNTEDTANK)) {
					// walking on ground
					pm->cmd.rightmove = crandom() * 64;
				} else if (!(pm->ps->eFlags & EF_MOUNTEDTANK)) {
					// airborne
					pm->cmd.rightmove = crandom() * 32;
				}
			}

			if (!(pm->ps->eFlags & EF_MOUNTEDTANK))
			{
				float	duration	= SecondaryEntityState(pm->ps->clientNum)->effect1Time;
				float	elapsed		= SecondaryEntityState(pm->ps->clientNum)->effect2Time;
				float	ratio		= (1.0f - (elapsed / duration));

				pm->ps->viewangles[ROLL] += crandom() * 2 * ratio;
			}
		}
#endif //__SHELLSHOCK__

		if (g_realism.integer && pm->ps->stats[STAT_HEALTH] < pm->ps->stats[STAT_MAX_HEALTH] && pm->ps->stats[STAT_HEALTH] > 0)
		{// In realsim, we stuble around based on how low health is...
			if (pm->cmd.rightmove == 0 && pm->cmd.forwardmove)
			{
				if (pml.ladder) {
					pm->cmd.rightmove = crandom() * 24;
				} else if (pm->ps->pm_flags & PMF_TIME_WATERJUMP) {
					pm->cmd.rightmove = crandom() * 32;
				} else if (pm->waterlevel > 1) {
					// swimming
					pm->cmd.rightmove = crandom() * 32;
				} else if (pml.walking && !(pm->ps->eFlags & EF_MOUNTEDTANK)) {
					// walking on ground
					pm->cmd.rightmove = crandom() * 64;
				} else if (!(pm->ps->eFlags & EF_MOUNTEDTANK)) {
					// airborne
					pm->cmd.rightmove = crandom() * 32;
				}
			}

			if (!(pm->ps->eFlags & EF_MOUNTEDTANK))
			{// Add randomized view angles...
				float frac = ((pm->ps->stats[STAT_MAX_HEALTH] - pm->ps->stats[STAT_HEALTH]) / pm->ps->stats[STAT_MAX_HEALTH]);
				pm->ps->viewangles[ROLL] += crandom() * 2 * frac;
			}
		}

		if (pml.ladder) 
		{
			PM_LadderMove();
		} 
		else if (pm->ps->pm_flags & PMF_TIME_WATERJUMP) 
		{
			PM_WaterJumpMove();
		} 
		else if ( pm->waterlevel > 1 ) 
		{// swimming
			PM_WaterMove();
		} 
		else if ( pml.walking && !(pm->ps->eFlags & EF_MOUNTEDTANK)) 
		{// walking on ground
			PM_WalkMove();
		} 
		else if (!(pm->ps->eFlags & EF_MOUNTEDTANK)) 
		{// airborne
			PM_AirMove();
		}
#ifdef __VEHICLES__
	}
#endif //__VEHICLES__

	if( pm->ps->eFlags & EF_MOUNTEDTANK ) {
		VectorClear( pm->ps->velocity );

		pm->ps->viewheight = DEFAULT_VIEWHEIGHT;

		BG_AnimScriptAnimation( pm->ps, pm->character->animModelInfo, ANIM_MT_IDLE, qtrue );
	}


	/*if( pm->ps->eFlags & EF_PRONE && !pml.walking ) {
		// can't be prone in midair
		pm->ps->eFlags &= ~EF_PRONE;
		pm->ps->eFlags &= ~EF_PRONE_MOVING;
		pm->pmext->proneTime = -pm->cmd.serverTime;	// timestamp 'stop prone'

		if( pm->ps->weapon == WP_MOBILE_MG42_SET ) {
			PM_BeginWeaponChange( WP_MOBILE_MG42_SET, WP_MOBILE_MG42, qfalse );
		}
	}*/
	
	PM_Sprint();

	// set groundentity, watertype, and waterlevel
	PM_GroundTrace();
	PM_SetWaterLevel();

	// weapons
	PM_Weapon();

	// footstep events / legs animations
	PM_Footsteps();

	// entering / leaving water splashes
	PM_WaterEvents();

	// snap some parts of playerstate to save network bandwidth
	trap_SnapVector( pm->ps->velocity );
//	SnapVector( pm->ps->velocity );
}


/*
================
Pmove

Can be called by either the server or the client
================
*/
int Pmove (pmove_t *pmove) {
	int			finalTime;

	// Ridah
/*	if (pmove->ps->eFlags & EF_DUMMY_PMOVE) {
		PmoveSingle( pmove );
		return (0);
	}*/
	// done.

	finalTime = pmove->cmd.serverTime;

	if ( finalTime < pmove->ps->commandTime ) {
		return (0);	// should not happen
	}

	if ( finalTime > pmove->ps->commandTime + 1000 ) {
		pmove->ps->commandTime = finalTime - 1000;
	}

	// after a loadgame, prevent huge pmove's
	if ( pmove->ps->pm_flags & PMF_TIME_LOAD ) {
		if ( finalTime - pmove->ps->commandTime > 50 ) {
			pmove->ps->commandTime = finalTime - 50;
		}
	}

	pmove->ps->pmove_framecount = (pmove->ps->pmove_framecount+1) & ((1<<PS_PMOVEFRAMECOUNTBITS)-1);

	// RF
	pm = pmove;
	PM_AdjustAimSpreadScale ();

//	startedTorsoAnim = -1;
//	startedLegAnim = -1;

	// chop the move up if it is too long, to prevent framerate
	// dependent behavior
	while ( pmove->ps->commandTime != finalTime ) {
		int		msec;

		msec = finalTime - pmove->ps->commandTime;

		if ( pmove->pmove_fixed ) {
			if ( msec > pmove->pmove_msec ) {
				msec = pmove->pmove_msec;
			}
		}
		else {
			// rain - this was 66 (15fps), but I've changed it to
			// 50 (20fps, max rate of mg42) to alleviate some of the
			// framerate dependency with the mg42.
			// in reality, this should be split according to sv_fps,
			// and pmove() shouldn't handle weapon firing
			if ( msec > 50 ) {
				msec = 50;
			}
		}
		pmove->cmd.serverTime = pmove->ps->commandTime + msec;
		PmoveSingle( pmove );

		if ( pmove->ps->pm_flags & PMF_JUMP_HELD ) {
			pmove->cmd.upmove = 20;
		}
	}

	// rain - sanity check weapon heat
	if (pmove->ps->curWeapHeat > 255)
		pmove->ps->curWeapHeat = 255;
	else if (pmove->ps->curWeapHeat < 0)
		pmove->ps->curWeapHeat = 0;

	//PM_CheckStuck();

	if ( (pm->ps->stats[STAT_HEALTH] <= 0 || pm->ps->pm_type == PM_DEAD ) && pml.groundTrace.surfaceFlags & SURF_MONSTERSLICK )
		return (pml.groundTrace.surfaceFlags);
	else 
		return (0);

}

// PmovePredict
//
// Used to calculate player movement for g_skipCorrection.
//
// Before calling PmovePredict() the following player state
// values should be backed up and then restored after the
// new values have been copied to the entity state .
//
// PM_GroundTrace() and friends modify
// 	ps->groundEntityNum
// 	ps->pm_flags
// 	ps->pm_time
// 	ps->eFlags
//
// PM_StepSlideMove() and friends modify
// 	ps->origin
// 	ps->velocity

void PmovePredict(pmove_t *pmove, float frametime)
{
	pm = pmove;
	memset(&pml, 0, sizeof(pml));
	pml.frametime = frametime;
	PM_GroundTrace();

	// tjw: don't bother to figure out gravity if already on the ground
	//      or moving on a ladder.
	if(pml.groundPlane || (pm->ps->pm_flags & PMF_LADDER))
		PM_StepSlideMove(qfalse);
	else
		PM_StepSlideMove(qtrue);
		
}
