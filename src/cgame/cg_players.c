/*
 * name:		cg_players.c
 *
 * desc:		handle the media and animation for player entities
 *              //modified by Masteries 28/11/2007
*/              //add code to CG_tank
                //add code in  line 2773
 

#include "cg_local.h"
#include "../game/bg_classes.h"

#define	SWING_RIGHT	1
#define SWING_LEFT	2

#define JUMP_HEIGHT				56
#define SWINGSPEED				0.3

static int			dp_realtime;
static float		jumpHeight;

animation_t		*lastTorsoAnim;
animation_t		*lastLegsAnim;

extern const char* cg_skillRewards[SK_NUM_SKILLS][NUM_SKILL_LEVELS-1];

#ifdef __ETE__
centity_t *CG_SecondaryEntityFor (int clientNum )
{// UQ1: If primary s.number is given, will give secondary, if secondary s.number is given it will give the primary :)
	centity_t *cent = &cg_entities[cg_entities[clientNum].currentState.dl_intensity];
	int i;

	if (cent->currentState.dl_intensity == clientNum && clientNum != cg_entities[clientNum].currentState.dl_intensity)
		return cent;

	// Forced initialization...
	for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++)
	{
		centity_t *ent = &cg_entities[i];

		if (!ent)
			continue;

		if (ent->currentState.eType == ET_SECONDARY)
		{
			cg_entities[ent->currentState.dl_intensity].currentState.dl_intensity = i;
			break;
		}
	}

	return &cg_entities[cg_entities[clientNum].currentState.dl_intensity];
}
#endif //__ETE__

#ifdef __ETE__
qboolean HitSoundsRegistered = qfalse;

sfxHandle_t hitSound1;
sfxHandle_t hitSound2;
sfxHandle_t hitSound3;
sfxHandle_t hitSound4;

void CG_HitSound ( int clientNum )
{
	centity_t *cent = &cg_entities[clientNum];
	int choice = 1;
	qboolean localClient = qfalse;

	if (clientNum == cg.clientNum || clientNum == cg.snap->ps.clientNum)
		localClient = qtrue;

	if (!HitSoundsRegistered)
	{// Register hit sounds on first usage...
		hitSound1 = trap_S_RegisterSound( "sound/player/pain1.wav", qfalse );
		hitSound2 = trap_S_RegisterSound( "sound/player/pain2.wav", qfalse );
		hitSound3 = trap_S_RegisterSound( "sound/player/pain3.wav", qfalse );
		hitSound4 = trap_S_RegisterSound( "sound/player/pain4.wav", qfalse );
		
		HitSoundsRegistered = qtrue;
	}

	choice = Q_TrueRand(1,4);

	if (localClient)
	{
		switch( choice ) 
		{
			case 1:
				trap_S_StartSound (NULL, clientNum, CHAN_BODY, hitSound1 );
				break;
			case 2:
				trap_S_StartSound (NULL, clientNum, CHAN_BODY, hitSound2 );
				break;
			case 3:
				trap_S_StartSound (NULL, clientNum, CHAN_BODY, hitSound3 );
				break;
			default:
				trap_S_StartSound (NULL, clientNum, CHAN_BODY, hitSound4 );
				break;
		}
	}
	else
	{
		switch( choice ) 
		{
			case 1:
				trap_S_StartSound (cent->lerpOrigin, clientNum, CHAN_AUTO, hitSound1 );
				break;
			case 2:
				trap_S_StartSound (cent->lerpOrigin, clientNum, CHAN_AUTO, hitSound2 );
				break;
			case 3:
				trap_S_StartSound (cent->lerpOrigin, clientNum, CHAN_AUTO, hitSound3 );
				break;
			default:
				trap_S_StartSound (cent->lerpOrigin, clientNum, CHAN_AUTO, hitSound4 );
				break;
		}
	}

	//trap_S_StartSound( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx );
}

qboolean CG_IsForward (centity_t *self, centity_t *other)
{
	vec3_t	vec;
	float	dot;
	vec3_t	forward;

	float	dotConst = 0.0;	//the dotConst values come from this idea - after the dot-product, 90 degrees to either side gives you 0.0
	//float	dotConst = 0.254;	//the dotConst values come from this idea - after the dot-product, 90 degrees to either side gives you 0.0
								//directly in front gives you 1.0, and directly behind gives you -1.0 (cosine).  

	/*switch(self->skillchoice)	//vary the bot's fov by his skill
	{
	case 0:
		dotConst = 0.174;			//160 degrees - like cg_fov 80
		break;
	case 1:
		dotConst = 0.0;				//180 --DEFAULT - like cg_fov 90
		break;
	case 2:
		dotConst = -0.087;			//190 - like cg_fov 95
		break;
	case 3:
		dotConst = -0.216;			//205 - like cg_fov 102.5 - this is closer to a real human's fov, but NOT in a game
		break;
	default:
		dotConst = -0.286;			//205 - like cg_fov 102.5 - this is closer to a real human's fov, but NOT in a game
		break;
	}*/
	
	//get the direction in front of the bot
	AngleVectors (self->lerpAngles, forward, NULL, NULL);
	//get the direction to the 'other'
	VectorSubtract (other->lerpOrigin, self->lerpOrigin, vec);
	//make 'vec' (which points from the bot to the 'other') have length 1 (necessary for math...)
	VectorNormalize (vec);
	//essentially get the angle between the bot and the 'other'
	dot = DotProduct (vec, forward);
	//if the 'other' is in the bot's fov, return true (yes, the 'other' is in fron of the bot)
	if (dot > dotConst)
		return qtrue;

	//otherwise return false
	return qfalse;
}

qboolean CG_IsBack (centity_t *self, centity_t *other)
{
	vec3_t	vec;
	float	dot;
	vec3_t	forward;

	float	dotConst = 0.0;	//the dotConst values come from this idea - after the dot-product, 90 degrees to either side gives you 0.0
	//float	dotConst = 0.254;	//the dotConst values come from this idea - after the dot-product, 90 degrees to either side gives you 0.0
								//directly in front gives you 1.0, and directly behind gives you -1.0 (cosine).  

	//get the direction in front of the bot
	AngleVectors (self->lerpAngles, forward, NULL, NULL);
	forward[0]=0-forward[0];
	forward[1]=0-forward[1];
	forward[2]=0-forward[2];
	//get the direction to the 'other'
	VectorSubtract (other->lerpOrigin, self->lerpOrigin, vec);
	//make 'vec' (which points from the bot to the 'other') have length 1 (necessary for math...)
	VectorNormalize (vec);
	//essentially get the angle between the bot and the 'other'
	dot = DotProduct (vec, forward);
	//if the 'other' is in the bot's fov, return true (yes, the 'other' is in fron of the bot)
	if (dot > dotConst)
		return qtrue;

	//otherwise return false
	return qfalse;
}

qboolean CG_IsLeft (centity_t *self, centity_t *other)
{
	vec3_t	vec;
	float	dot;
	vec3_t	forward;

	float	dotConst = 0.0;	//the dotConst values come from this idea - after the dot-product, 90 degrees to either side gives you 0.0
	//float	dotConst = 0.254;	//the dotConst values come from this idea - after the dot-product, 90 degrees to either side gives you 0.0
								//directly in front gives you 1.0, and directly behind gives you -1.0 (cosine).  

	//get the direction in front of the bot
	AngleVectors (self->lerpAngles, NULL, forward, NULL);
	forward[0]=0-forward[0];
	forward[1]=0-forward[1];
	forward[2]=0-forward[2];
	//get the direction to the 'other'
	VectorSubtract (other->lerpOrigin, self->lerpOrigin, vec);
	//make 'vec' (which points from the bot to the 'other') have length 1 (necessary for math...)
	VectorNormalize (vec);
	//essentially get the angle between the bot and the 'other'
	dot = DotProduct (vec, forward);
	//if the 'other' is in the bot's fov, return true (yes, the 'other' is in fron of the bot)
	if (dot > dotConst)
		return qtrue;

	//otherwise return false
	return qfalse;
}

qboolean CG_IsRight (centity_t *self, centity_t *other)
{
	vec3_t	vec;
	float	dot;
	vec3_t	forward;

	float	dotConst = 0.0;	//the dotConst values come from this idea - after the dot-product, 90 degrees to either side gives you 0.0
	//float	dotConst = 0.234;	//the dotConst values come from this idea - after the dot-product, 90 degrees to either side gives you 0.0
								//directly in front gives you 1.0, and directly behind gives you -1.0 (cosine).  

	//get the direction in front of the bot
	AngleVectors (self->lerpAngles, NULL, forward, NULL);
	//get the direction to the 'other'
	VectorSubtract (other->lerpOrigin, self->lerpOrigin, vec);
	//make 'vec' (which points from the bot to the 'other') have length 1 (necessary for math...)
	VectorNormalize (vec);
	//essentially get the angle between the bot and the 'other'
	dot = DotProduct (vec, forward);
	//if the 'other' is in the bot's fov, return true (yes, the 'other' is in fron of the bot)
	if (dot > dotConst)
		return qtrue;

	//otherwise return false
	return qfalse;
}

enum {
	POS_FORWARD,
	POS_BACK,
	POS_LEFT,
	POS_RIGHT,
	POS_NONE
};

int CG_GetSoundPosition (centity_t *self, centity_t *other)
{
	if (CG_IsForward(self, other))
		return POS_FORWARD;
	else if (CG_IsBack(self, other))
		return POS_BACK;
	else if (CG_IsLeft(self, other))
		return POS_LEFT;
	else if (CG_IsRight(self, other))
		return POS_RIGHT;
	else // Should never happen!
		return POS_FORWARD;
}

qboolean WizzSoundsRegistered = qfalse;

sfxHandle_t wizzSound1;
sfxHandle_t wizzSound2;
sfxHandle_t wizzSound3;
sfxHandle_t wizzSound4;
sfxHandle_t wizzSound5;
sfxHandle_t wizzSound6;
sfxHandle_t wizzSound7;

void CG_WizzSound ( int clientNum, int shooterNum )
{// Bullet wizzing by!
	centity_t *cent = &cg_entities[clientNum];
	centity_t *shooter = &cg_entities[shooterNum];
	vec3_t	sound_origin_L, sound_origin_R, forward;
	int		choice = Q_TrueRand(1,7);
	int posinfo;

	if (!cent || !shooter)
		return;

	if (clientNum != cg.clientNum && clientNum != cg.snap->ps.clientNum)
		return;

	posinfo = CG_GetSoundPosition (shooter, cent);
	
	AngleVectors (cent->lerpAngles, forward, NULL, NULL);

	// Work out positional info...
	switch (posinfo)
	{
		case POS_FORWARD:
			VectorMA( cent->lerpOrigin, 96, forward, sound_origin_L );
			VectorMA( cent->lerpOrigin, 96, forward, sound_origin_R );
			break;
		case POS_BACK:
			VectorMA( cent->lerpOrigin, -96, forward, sound_origin_L );
			VectorMA( cent->lerpOrigin, -96, forward, sound_origin_R );
			break;
		case POS_LEFT:
			VectorMA( cent->lerpOrigin, 96, forward, sound_origin_L );
			VectorMA( cent->lerpOrigin, 256, forward, sound_origin_R );
			break;
		case POS_RIGHT:
			VectorMA( cent->lerpOrigin, 256, forward, sound_origin_L );
			VectorMA( cent->lerpOrigin, 96, forward, sound_origin_R );
			break;
	}

	if (!WizzSoundsRegistered)
	{// Register hit sounds on first usage...
		wizzSound1 = trap_S_RegisterSound( "sound/weapons/wizzsounds/whiz1.wav", qfalse );
		wizzSound2 = trap_S_RegisterSound( "sound/weapons/wizzsounds/whiz2.wav", qfalse );
		wizzSound3 = trap_S_RegisterSound( "sound/weapons/wizzsounds/whiz3.wav", qfalse );
		wizzSound4 = trap_S_RegisterSound( "sound/weapons/wizzsounds/whiz4.wav", qfalse );
		wizzSound5 = trap_S_RegisterSound( "sound/weapons/wizzsounds/whiz5.wav", qfalse );
		wizzSound6 = trap_S_RegisterSound( "sound/weapons/wizzsounds/whiz6.wav", qfalse );
		wizzSound7 = trap_S_RegisterSound( "sound/weapons/wizzsounds/whiz7.wav", qfalse );
		WizzSoundsRegistered = qtrue;
	}

	if (choice == 1)
	{
		trap_S_StartSound (sound_origin_L, clientNum, CHAN_AUTO, wizzSound1 );
		trap_S_StartSound (sound_origin_R, clientNum, CHAN_AUTO, wizzSound1 );
	}
	else if (choice == 2)
	{
		trap_S_StartSound (sound_origin_L, clientNum, CHAN_AUTO, wizzSound2 );
		trap_S_StartSound (sound_origin_R, clientNum, CHAN_AUTO, wizzSound2 );
	}
	else if (choice == 3)
	{
		trap_S_StartSound (sound_origin_L, clientNum, CHAN_AUTO, wizzSound3 );
		trap_S_StartSound (sound_origin_R, clientNum, CHAN_AUTO, wizzSound3 );
	}
	else if (choice == 4)
	{
		trap_S_StartSound (sound_origin_L, clientNum, CHAN_AUTO, wizzSound4 );
		trap_S_StartSound (sound_origin_R, clientNum, CHAN_AUTO, wizzSound4 );
	}
	else if (choice == 5)
	{
		trap_S_StartSound (sound_origin_L, clientNum, CHAN_AUTO, wizzSound5 );
		trap_S_StartSound (sound_origin_R, clientNum, CHAN_AUTO, wizzSound5 );
	}
	else if (choice == 6)
	{
		trap_S_StartSound (sound_origin_L, clientNum, CHAN_AUTO, wizzSound6 );
		trap_S_StartSound (sound_origin_R, clientNum, CHAN_AUTO, wizzSound6 );
	}
	else if (choice == 7)
	{
		trap_S_StartSound (sound_origin_L, clientNum, CHAN_AUTO, wizzSound7 );
		trap_S_StartSound (sound_origin_R, clientNum, CHAN_AUTO, wizzSound7 );
	}
}
#endif //__ETE__

/*
================
CG_EntOnFire
================
*/
qboolean CG_EntOnFire( centity_t *cent ) {
	if ( cent->currentState.number == cg.snap->ps.clientNum ) {
		// TAT 11/15/2002 - the player is always starting out on fire, which is easily seen in cinematics
		//		so make sure onFireStart is not 0
		return	( cg.snap->ps.onFireStart
			&& (cg.snap->ps.onFireStart < cg.time)
			&& ((cg.snap->ps.onFireStart+2000) > cg.time) );
	}
	else {
		return	(	(cent->currentState.onFireStart < cg.time) &&
					(cent->currentState.onFireEnd > cg.time));
	}
}

/*
================
CG_IsCrouchingAnim
================
*/
qboolean CG_IsCrouchingAnim( animModelInfo_t* animModelInfo, int animNum ) {
	animation_t *anim;

	// FIXME: make compatible with new scripting
	animNum &= ~ANIM_TOGGLEBIT;
	//
	anim = BG_GetAnimationForIndex( animModelInfo, animNum );
	//
	if( anim->movetype & ((1<<ANIM_MT_IDLECR)|(1<<ANIM_MT_WALKCR)|(1<<ANIM_MT_WALKCRBK)) )
		return qtrue;
	//
	return qfalse;
}

/*
// UQ1: Do this one day :)
#define MAX_CUSTOMSOUNDS		64
#define MAX_CUSTOMSOUND_PATH	512

typedef struct customsounds_s	// Sounds Table Structure
{
	char		soundFile[MAX_CUSTOMSOUND_PATH];		// Sound name.
	qhandle_t	sound_id;						// Sound's qhandle
} customsounds_t;				// MyMusic Table Typedef

customsounds_t	customsounds[MAX_CUSTOMSOUNDS];
int				customsound_total = 0;
*/

/*
================
CG_CustomSound
================
*/
sfxHandle_t	CG_CustomSound( int clientNum, const char *soundName ) {
	if ( soundName[0] != '*' ) {
		return trap_S_RegisterSound( soundName, qfalse );
	}

	return 0;
}

/*
=============================================================================

CLIENT INFO

=============================================================================
*/

/*
===================
CG_LoadClientInfo

Load it now, taking the disk hits.
This will usually be deferred to a safe time
===================
*/
static void CG_LoadClientInfo( int clientNum ) {
	int			i;

	// reset any existing players and bodies, because they might be in bad
	// frames for this new model
	for( i = 0 ; i < MAX_GENTITIES ; i++ ) {
		if ( cg_entities[i].currentState.clientNum == clientNum && cg_entities[i].currentState.eType == ET_PLAYER ) {
			CG_ResetPlayerEntity( &cg_entities[i] );
		}
	}
}

void CG_ParseTeamXPs( int n ) {
	int i, j;
	char* cs = (char*)CG_ConfigString( CS_AXIS_MAPS_XP + n );
	const char* token;

	for( i = 0; i < MAX_MAPS_PER_CAMPAIGN; i++ ) {
		for( j = 0; j < SK_NUM_SKILLS; j++ ) {
			token = COM_ParseExt( &cs, qfalse );

			if( !token || !*token ) {
				return;
			}

			if( n == 0 ) {
				cgs.tdbAxisMapsXP[ j ][ i ] = atoi( token );
			} else {
				cgs.tdbAlliedMapsXP[ j ][ i ] = atoi( token );
			}
		}
	}
}

void CG_LimboPanel_SendSetupMsg( qboolean forceteam );

/*
======================
CG_NewClientInfo
======================
*/
void CG_NewClientInfo( int clientNum ) {
	clientInfo_t *ci;
	clientInfo_t newInfo;
	const char	*configstring;
	const char	*v;
	// forty - Chruker Bug fix 020
	//int	oldclass;

	ci = &cgs.clientinfo[clientNum];

	configstring = CG_ConfigString( clientNum + CS_PLAYERS );
	if ( !*configstring ) {
		memset( ci, 0, sizeof( *ci ) );
		return;		// player just left
	}

	// build into a temp buffer so the defer checks can use
	// the old value
	memset( &newInfo, 0, sizeof( newInfo ) );

	// Gordon: grabbing some older stuff, if it's a new client, tinfo will update within one second anyway, otherwise you get the health thing flashing red
	// NOTE: why are we bothering to do all this setting up of a new clientInfo_t anyway? it was all for deffered clients iirc, which we dont have
	newInfo.location[0] =	ci->location[0];
	newInfo.location[1] =	ci->location[1];
	newInfo.health =		ci->health;
	newInfo.fireteamData =	ci->fireteamData;
	newInfo.clientNum =		clientNum;
	newInfo.selected =		ci->selected;
	newInfo.totalWeapAcc =	ci->totalWeapAcc;

	// isolate the player's name
	v = Info_ValueForKey(configstring, "n");
	Q_strncpyz( newInfo.name, v, sizeof( newInfo.name ) );
	Q_strncpyz( newInfo.cleanname, v, sizeof( newInfo.cleanname ) );
	Q_CleanStr( newInfo.cleanname );
	

	// bot skill
	v = Info_ValueForKey( configstring, "skill" );
	newInfo.botSkill = atoi( v );

	// team
	v = Info_ValueForKey( configstring, "t" );
	newInfo.team = atoi( v );

	// class
	v = Info_ValueForKey( configstring, "c" );
	newInfo.cls = atoi( v );

	// rank
	v = Info_ValueForKey( configstring, "r" );
	newInfo.rank = atoi( v );

	v = Info_ValueForKey( configstring, "f" );
	newInfo.fireteam = atoi( v );

	v = Info_ValueForKey( configstring, "m" );
	if( *v ) {
		int i;
		char buf[2];
		buf[1] = '\0';
		for( i = 0; i < SK_NUM_SKILLS; i++ ) {
			buf[0] = *v;
			newInfo.medals[i] = atoi( buf );
			v++;
		}
	}

	v = Info_ValueForKey( configstring, "ch" );
	if( *v ) {
		newInfo.character = cgs.gameCharacters[atoi(v)];
	}

	v = Info_ValueForKey( configstring, "s");
	if( *v ) {
		int i;

		for( i = 0; i < SK_NUM_SKILLS; i++ ) {
			char skill[2];

			skill[0] = v[i];
			skill[1] = '\0';

			newInfo.skill[i] = atoi(skill);
		}
	}

	// diguiseName
	v = Info_ValueForKey( configstring, "dn" );
	Q_strncpyz( newInfo.disguiseName, v, sizeof( newInfo.disguiseName ) );

	// disguiseRank
	v = Info_ValueForKey( configstring, "dr" );
	newInfo.disguiseRank = atoi( v );

	// Gordon: weapon and latchedweapon ( FIXME: make these more secure )
	v = Info_ValueForKey( configstring, "w" );
	newInfo.weapon = atoi( v );

	v = Info_ValueForKey( configstring, "lw" );
	newInfo.latchedweapon = atoi( v );

	v = Info_ValueForKey( configstring, "sw" );
	newInfo.secondaryweapon = atoi( v );

	v = Info_ValueForKey( configstring, "ref" );
	newInfo.refStatus = atoi( v );

	// Gordon: detect rank/skill changes client side
	if( clientNum == cg.clientNum ) {
		int i;

		if( newInfo.team != cgs.clientinfo[ cg.clientNum ].team ) {
			if( cgs.autoFireteamCreateEndTime != cg.time + 20000) { 
				cgs.autoFireteamCreateEndTime = 0;
			}
			if( cgs.autoFireteamJoinEndTime != cg.time + 20000) { 
				cgs.autoFireteamJoinEndTime = 0;
			}
		}

		if( newInfo.rank > cgs.clientinfo[ cg.clientNum ].rank ) {

			CG_SoundPlaySoundScript( cgs.clientinfo[cg.clientNum].team == TEAM_ALLIES ? rankSoundNames_Allies[ newInfo.rank ] : rankSoundNames_Axis[ newInfo.rank ], NULL, -1, qtrue );

			CG_AddPMItemBig( PM_RANK, va("Promoted to rank %s!", cgs.clientinfo[ cg.clientNum ].team == TEAM_AXIS ? rankNames_Axis[newInfo.rank] : rankNames_Allies[newInfo.rank] ), rankicons[ newInfo.rank ][ 0 ].shader );
		}

		// CHRUKER: b020 - Make sure player class and primary weapons are correct for 
		// subsequent calls to CG_LimboPanel_SendSetupMsg 
		CG_LimboPanel_Setup();

		for( i = 0; i < SK_NUM_SKILLS; i++ ) {
			// save skillpoints information first
			newInfo.skillpoints[i] = cgs.clientinfo[cg.clientNum].skillpoints[i];

			if( newInfo.skill[i] > cgs.clientinfo[ cg.clientNum ].skill[i] ) {
				// Gordon: slick hack so that funcs we call use teh new value now
				cgs.clientinfo[ cg.clientNum ].skill[ i ] = newInfo.skill[ i ];

				if( newInfo.skill[i] == 4 && i == SK_HEAVY_WEAPONS ) {
					if( cgs.clientinfo[ cg.clientNum ].skill[SK_LIGHT_WEAPONS] >= 4 ) {
						// CHRUKER: b020 - Only select SMG (2) if using the single gun (0) 
						if( cgs.ccSelectedWeapon2 == 0 ) {
							// forty - Chruker Bug fix 020
							//oldclass = cgs.ccSelectedClass;
							//cgs.ccSelectedClass = newInfo.cls;
							CG_LimboPanel_SetSelectedWeaponNumForSlot( 1, 2 ); // Selects SMG
						CG_LimboPanel_SendSetupMsg( qfalse );
							// forty - Chruker Bug fix 020
							//cgs.ccSelectedClass = oldclass;
						}
					} else {
						// forty - Chruker Bug fix 020
						//oldclass = cgs.ccSelectedClass;
						//cgs.ccSelectedClass = newInfo.cls;
						CG_LimboPanel_SetSelectedWeaponNumForSlot( 1, 1 ); // Selects SMG
						CG_LimboPanel_SendSetupMsg( qfalse );
						//cgs.ccSelectedClass = oldclass;
					}
				}

				if( newInfo.skill[i] == 4 && i == SK_LIGHT_WEAPONS ) {
					// CHRUKER: b020 - Only select Akimbo guns (1) if using the single gun (0)
					if( cgs.clientinfo[ cg.clientNum ].skill[SK_HEAVY_WEAPONS] >= 4 ) {
						if( cgs.ccSelectedWeapon2 == 0 ) {
							// forty - Chruker Bug fix 020
							//oldclass = cgs.ccSelectedClass;
							//cgs.ccSelectedClass = newInfo.cls;
							CG_LimboPanel_SetSelectedWeaponNumForSlot( 1, 1 ); // Selects Akimbo guns
							CG_LimboPanel_SendSetupMsg( qfalse );
							//cgs.ccSelectedClass = oldclass;
						}
					} else {
						// forty - Chruker Bug fix 020
						//oldclass = cgs.ccSelectedClass;
						//cgs.ccSelectedClass = newInfo.cls;
						CG_LimboPanel_SetSelectedWeaponNumForSlot( 1, 1 ); // Selects Akimbo guns
						CG_LimboPanel_SendSetupMsg( qfalse );
						//cgs.ccSelectedClass = oldclass;
					}					
				}

				CG_AddPMItemBig( PM_SKILL, va("Increased %s skill to level %i!", skillNames[i], newInfo.skill[i] ), cgs.media.skillPics[ i ] );

				CG_PriorityCenterPrint( va( "You have been rewarded with %s", cg_skillRewards[ i ][ newInfo.skill[i]-1 ]), SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.20), SMALLCHAR_WIDTH, 99999 );
			}
		}

		if( newInfo.team != cgs.clientinfo[ cg.clientNum ].team ) {
			// clear these
			memset( cg.artilleryRequestPos, 0, sizeof(cg.artilleryRequestPos) );
			memset( cg.artilleryRequestTime, 0, sizeof(cg.artilleryRequestTime) );
		}

		trap_Cvar_Set( "authLevel", va( "%i", newInfo.refStatus ) );

		if( newInfo.refStatus != ci->refStatus ) {
			if( newInfo.refStatus <= RL_NONE ) {
				const char *info = CG_ConfigString( CS_SERVERINFO );

				trap_Cvar_Set( "cg_ui_voteFlags", Info_ValueForKey(info, "voteFlags"));
				CG_Printf("[cgnotify]^3*** You have been stripped of your referee status! ***\n");

			} else {
				trap_Cvar_Set( "cg_ui_voteFlags", "0");
				CG_Printf("[cgnotify]^2*** You have been authorized \"%s\" status ***\n", ((newInfo.refStatus == RL_RCON) ? "rcon" : "referee"));
				CG_Printf("Type: ^3ref^7 (by itself) for a list of referee commands.\n");
			}
		}
	}


	// rain - passing the clientNum since that's all we need, and we
	// can't calculate it properly from the clientinfo
	CG_LoadClientInfo( clientNum );

	// replace whatever was there with the new one
	newInfo.infoValid = qtrue;
	*ci = newInfo;

	// make sure we have a character set
	if( !ci->character ) {
		ci->character = BG_GetCharacter( ci->team, ci->cls );
	}

	// Gordon: need to resort the fireteam list, incase ranks etc have changed
	CG_SortClientFireteam();
}

/*
=============================================================================

PLAYER ANIMATION

=============================================================================
*/

bg_playerclass_t* CG_PlayerClassForClientinfo( clientInfo_t *ci, centity_t* cent) {
	int team, cls;

	if( cent && (cent->currentState.eType == ET_CORPSE || cent->currentState.eType == ET_NPC_CORPSE) ) {
		return BG_GetPlayerClassInfo( cent->currentState.modelindex, cent->currentState.modelindex2 );
	}

	if( cent && cent->currentState.powerups & (1 << PW_OPS_DISGUISED) ) {
		team = ci->team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;

		// rain - fixed incorrect class determination (was & 6,
		// should be & 7)
		cls = (cent->currentState.powerups >> PW_OPS_CLASS_1) & 7;

		return BG_GetPlayerClassInfo( team, cls );
	}

	return BG_GetPlayerClassInfo( ci->team, ci->cls );
}

/*
===============
CG_SetLerpFrameAnimation

may include ANIM_TOGGLEBIT
===============
*/
static void CG_SetLerpFrameAnimation( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation ) {
	animation_t	*anim;

	bg_character_t* character = CG_CharacterForClientinfo( ci, cent );

	if( !character ) {
		return;
	}

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;

	if( newAnimation < 0 || newAnimation >= character->animModelInfo->numAnimations ) {
		CG_Error( "CG_SetLerpFrameAnimation: Bad animation number: %i", newAnimation );
	}

	anim = character->animModelInfo->animations[ newAnimation ];

	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;

	if( cg_debugAnim.integer == 1 ) {
		CG_Printf( "Anim: %i, %s\n", newAnimation, character->animModelInfo->animations[newAnimation]->name );
	}
}

/*
===============
CG_RunLerpFrame

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
void CG_RunLerpFrame( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, float speedScale ) {
	int			f;
	animation_t	*anim;

	// debugging tool to get no animations
	if( cg_animSpeed.integer == 0 ) {
		lf->oldFrame = lf->frame = lf->backlerp = 0;
		return;
	}

	// see if the animation sequence is switching
	if( ci && (newAnimation != lf->animationNumber || !lf->animation) ) {
		CG_SetLerpFrameAnimation( cent, ci, lf, newAnimation );
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if( cg.time >= lf->frameTime ) {
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		// get the next frame based on the animation
		anim = lf->animation;
		if( !anim->frameLerp ) {
			return;		// shouldn't happen
		}
		if( cg.time < lf->animationTime ) {
			lf->frameTime = lf->animationTime;		// initial lerp
		} else {
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;
		}
		f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		f *= speedScale;		// adjust for haste, etc
		if( f >= anim->numFrames ) {
			f -= anim->numFrames;
			if ( anim->loopFrames ) {
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			} else {
				f = anim->numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}
		lf->frame = anim->firstFrame + f;
		lf->frameModel = anim->mdxFile;

		if( cg.time > lf->frameTime ) {
			lf->frameTime = cg.time;
			if( cg_debugAnim.integer ) {
				CG_Printf( "Clamp lf->frameTime\n");
			}
		}
	}

	if( lf->frameTime > cg.time + 200 ) {
		lf->frameTime = cg.time;
	}

	if( lf->oldFrameTime > cg.time ) {
		lf->oldFrameTime = cg.time;
	}
	// calculate current lerp value
	if( lf->frameTime == lf->oldFrameTime ) {
		lf->backlerp = 0;
	} else {
		lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}


/*
===============
CG_ClearLerpFrame
===============
*/
static void CG_ClearLerpFrame( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int animationNumber ) {
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetLerpFrameAnimation( cent, ci, lf, animationNumber );
	if( lf->animation ) {
		lf->oldFrame = lf->frame = lf->animation->firstFrame;
		lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
	}
}

/*
===============
CG_SetLerpFrameAnimationRate

may include ANIM_TOGGLEBIT
===============
*/
void CG_SetLerpFrameAnimationRate( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation ) {
	animation_t		*anim, *oldanim;
	int				transitionMin = -1, oldAnimTime, oldAnimNum;
	qboolean		firstAnim = qfalse;

	bg_character_t *character = CG_CharacterForClientinfo( ci, cent );

	if( !character ) {
		return;
	}

	oldAnimTime	= lf->animationTime;
	oldanim		= lf->animation;
	oldAnimNum	= lf->animationNumber;

	if( !lf->animation ) {
		firstAnim = qtrue;
	}

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;

	if ( newAnimation < 0 || newAnimation >= character->animModelInfo->numAnimations ) {
		CG_Error( "CG_SetLerpFrameAnimationRate: Bad animation number: %i", newAnimation );
	}

	anim = character->animModelInfo->animations[ newAnimation ];

	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;

	if( !(anim->flags & ANIMFL_FIRINGANIM) || (lf != &cent->pe.torso) ) {
		if( (lf == &cent->pe.legs) && (CG_IsCrouchingAnim( character->animModelInfo, newAnimation ) != CG_IsCrouchingAnim( character->animModelInfo, oldAnimNum )) ) {
			if( anim->moveSpeed || (anim->movetype & ((1<<ANIM_MT_TURNLEFT)|(1<<ANIM_MT_TURNRIGHT))) )	// if unknown movetype, go there faster
				transitionMin = lf->frameTime + 200;	// slowly raise/drop
			else
				transitionMin = lf->frameTime + 350;	// slowly raise/drop
		} else if( anim->moveSpeed )
			transitionMin = lf->frameTime + 120;	// always do some lerping (?)
		else	// not moving, so take your time
			transitionMin = lf->frameTime + 170;	// always do some lerping (?)

		if( oldanim && oldanim->animBlend ) { //transitionMin < lf->frameTime + oldanim->animBlend) {
			transitionMin = lf->frameTime + oldanim->animBlend;
			lf->animationTime = transitionMin;
		} else {
			// slow down transitions according to speed
			if( anim->moveSpeed && lf->animSpeedScale < 1.0 )
				lf->animationTime += anim->initialLerp;

			if( lf->animationTime < transitionMin )
				lf->animationTime = transitionMin;
		}
	}

	// if first anim, go immediately
	if( firstAnim ) {
		lf->frameTime = cg.time - 1;
		lf->animationTime = cg.time - 1;
		lf->frame = anim->firstFrame;
		lf->frameModel = anim->mdxFile;
	}

	if( cg_debugAnim.integer == 1 ) {			// DHM - Nerve :: extra debug info
		CG_Printf( "Anim: %i, %s\n", newAnimation, character->animModelInfo->animations[newAnimation]->name );
	}
//CG_Printf("[fT%6d->%-6d] NA %d%s cgT %d\n", lf->oldFrameTime, lf->frameTime, lf->animationNumber, firstAnim?" (FIRST)":"", cg.time);
}

/*
===============
CG_RunLerpFrameRate

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
void CG_RunLerpFrameRate( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, centity_t *cent, int recursion ) {
	int			f;
	animation_t	*anim, *oldAnim;
	animation_t *otherAnim=NULL;
	qboolean	isLadderAnim;

	qboolean	done = qfalse;	// break out if we would loop forever

	int snapTime, snapPosTime;
	vec3_t snapOrg;

#define	ANIM_SCALEMAX_LOW	1.1
#define	ANIM_SCALEMAX_HIGH	1.6

#define	ANIM_SPEEDMAX_LOW	100
#define	ANIM_SPEEDMAX_HIGH	20

	// debugging tool to get no animations
	if( cg_animSpeed.integer == 0 ) {
		lf->oldFrame = lf->frame = lf->backlerp = 0;
		return;
	}

	isLadderAnim = lf->animation && (lf->animation->flags & ANIMFL_LADDERANIM);

	oldAnim = lf->animation;

	// see if the animation sequence is switching
	if( newAnimation != lf->animationNumber || !lf->animation ) {
		CG_SetLerpFrameAnimationRate( cent, ci, lf, newAnimation );
	}

	// Ridah, make sure the animation speed is updated when possible
	anim = lf->animation;

	// check for forcing last frame
	if( 
		// xkan, 12/27/2002 - In SP, corpse also stays at the last frame (of the death animation)
		// so that the death animation can end up in different positions
		// and the body will stay in that position
		( /*CG_IsSinglePlayer() 
		 &&*/ (cent->currentState.eType == ET_CORPSE || cent->currentState.eType == ET_NPC_CORPSE) 
		&& lf->frame == anim->firstFrame + anim->numFrames - 1 ) ) {
		lf->oldFrame = lf->frame = anim->firstFrame + anim->numFrames - 1;
		lf->oldFrameModel = lf->frameModel = anim->mdxFile;
		lf->backlerp = 0;
		return;
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	while( cg.time > lf->frameTime && !done ) {
		// Figure out the best snap to use for moveSpeed calc
		if (lf->frameTime > cg.snap->serverTime) {
			if (cg.nextSnap) {
#ifdef DEBUG
				if (lf->frameTime < cg.nextSnap->serverTime) {
					CG_Printf("nextSnap out of date!\n");
				}
#endif
				snapTime = cg.nextSnap->serverTime;
				snapPosTime = cent->nextState.pos.trTime;
				VectorCopy( cent->nextState.pos.trBase, snapOrg );

				if (cent->currentState.number == cg.snap->ps.clientNum) {
					VectorCopy( cg.nextSnap->ps.origin, snapOrg );
					snapPosTime = cg.nextSnap->ps.commandTime;
				}
			} else {
#ifdef DEBUG
				CG_Printf("snap out of date!\n");
#endif
				snapTime = cg.snap->serverTime;
				snapPosTime = cent->currentState.pos.trTime;
				VectorCopy( cent->currentState.pos.trBase, snapOrg );
			}
		} else {
			snapTime = cg.snap->serverTime;
			snapPosTime = cent->currentState.pos.trTime;
			VectorCopy( cent->currentState.pos.trBase, snapOrg );
		}

		// Calculate move speed
		if (lf->oldFrameSnapshotTime < snapPosTime) {
			// We have a new snapshot, and thus a new distance
			float serverDelta;

		// calculate the speed at which we moved over the last frame
			// zinx - changed to use server position instead of lerp'd position
			if( cent->currentState.number == cg.snap->ps.clientNum ) {
				if( isLadderAnim ) { // only use Z axis for speed
						lf->oldFramePos[0] = snapOrg[0];
						lf->oldFramePos[1] = snapOrg[1];
				} else {	// only use x/y axis
					lf->oldFramePos[2] = snapOrg[2];
				}
			} else {
				if( isLadderAnim ) { // only use Z axis for speed
					lf->oldFramePos[0] = snapOrg[0];
					lf->oldFramePos[1] = snapOrg[1];
				}
			}
			serverDelta = (float)(snapPosTime - lf->oldFrameSnapshotTime) / 1000.0;
			lf->moveSpeed = Distance( snapOrg, lf->oldFramePos ) / serverDelta;

			VectorCopy( snapOrg, lf->oldFramePos );
			lf->oldFrameSnapshotTime = snapPosTime;
		}

		// calculate speed for new frame
		if (anim->moveSpeed) {
			// convert moveSpeed to a factor of this animation's movespeed
			lf->animSpeedScale = lf->moveSpeed / (float)anim->moveSpeed;
	} else {
		// move at normal speed
		lf->animSpeedScale = 1.0;
	}
	// adjust with manual setting (pain anims)
	lf->animSpeedScale *= cent->pe.animSpeed;

		// restrict the speed range
		if( lf->animSpeedScale < 0.25 ) {	// if it's too slow, then a really slow spped, combined with a sudden take-off, can leave them playing a really slow frame while they a moving really fast
			if (lf->animSpeedScale < 0.01 && isLadderAnim)
				lf->animSpeedScale = 0.0;
			else
				lf->animSpeedScale = 0.25;
		} else if( lf->animSpeedScale > ANIM_SCALEMAX_LOW ) {

			if( !(anim->flags & ANIMFL_LADDERANIM) ) {
				// allow slower anims to speed up more than faster anims
				if( anim->moveSpeed > ANIM_SPEEDMAX_LOW ) {
					lf->animSpeedScale = ANIM_SCALEMAX_LOW;
				} else if( anim->moveSpeed < ANIM_SPEEDMAX_HIGH ) {
					if( lf->animSpeedScale > ANIM_SCALEMAX_HIGH )
						lf->animSpeedScale = ANIM_SCALEMAX_HIGH;
				} else {
					lf->animSpeedScale = ANIM_SCALEMAX_HIGH - (ANIM_SCALEMAX_HIGH - ANIM_SCALEMAX_LOW) * (float)(anim->moveSpeed - ANIM_SPEEDMAX_HIGH) / (float)(ANIM_SPEEDMAX_LOW - ANIM_SPEEDMAX_HIGH);
				}
			} else if( lf->animSpeedScale > 4.0 ) {
				lf->animSpeedScale = 4.0;
			}

		}

		// move to new frame
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		if( lf == &cent->pe.legs ) {
			otherAnim = cent->pe.torso.animation;
		} else if( lf == &cent->pe.torso ) {
			otherAnim = cent->pe.legs.animation;
		}

		// get the next frame based on the animation
		if( !lf->animSpeedScale ) {
			// stopped on the ladder, so stay on the same frame
			f = lf->frame - anim->firstFrame;
			lf->frameTime += anim->frameLerp;		// don't wait too long before starting to move again
		} else if ( lf->oldAnimationNumber != lf->animationNumber &&
				  (!anim->moveSpeed || lf->oldFrame < anim->firstFrame || lf->oldFrame >= anim->firstFrame + anim->numFrames) ) {	// Ridah, added this so walking frames don't always get reset to 0, which can happen in the middle of a walking anim, which looks wierd
			lf->frameTime = lf->animationTime;		// initial lerp
			if (oldAnim && anim->moveSpeed) {	// keep locomotions going continuously
				f = (lf->frame - oldAnim->firstFrame) + 1;
				while (f < 0) {
					f += anim->numFrames;
				}
			} else {
				f = 0;
			}
		} else if ( (lf == &cent->pe.legs) && otherAnim && !(anim->flags & ANIMFL_FIRINGANIM) && ((lf->animationNumber & ~ANIM_TOGGLEBIT) == (cent->pe.torso.animationNumber & ~ANIM_TOGGLEBIT)) && (!anim->moveSpeed) ) {
			// legs should synch with torso
			f = cent->pe.torso.frame - otherAnim->firstFrame;
			if (f >= anim->numFrames || f < 0) {
				f = 0;	// wait at the start for the legs to catch up (assuming they are still in an old anim)
			}
			lf->frameTime = cent->pe.torso.frameTime;
			done = qtrue;
		} else if ( (lf == &cent->pe.torso) && otherAnim && !(anim->flags & ANIMFL_FIRINGANIM) && ((lf->animationNumber & ~ANIM_TOGGLEBIT) == (cent->pe.legs.animationNumber & ~ANIM_TOGGLEBIT)) && (otherAnim->moveSpeed) ) {
			// torso needs to sync with legs
			f = cent->pe.legs.frame - otherAnim->firstFrame;
			if (f >= anim->numFrames || f < 0) {
				f = 0;	// wait at the start for the legs to catch up (assuming they are still in an old anim)
			}
			lf->frameTime = cent->pe.legs.frameTime;
			done = qtrue;
		} else {
			lf->frameTime = lf->oldFrameTime + (int)((float)anim->frameLerp * (1.0 / lf->animSpeedScale));
			if( anim->flags & ANIMFL_REVERSED ) {
					f = (anim->numFrames - 1) - ( (lf->frame - anim->firstFrame) - 1 );
				} else {
					f = (lf->frame - anim->firstFrame) + 1;
				}
			}
		//f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		if ( f >= anim->numFrames ) {
			f -= anim->numFrames;
			if ( anim->loopFrames ) {
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			} else {
				f = anim->numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
				done = qtrue;
			}
		}
		if( anim->flags & ANIMFL_REVERSED ) {
			lf->frame = anim->firstFrame + anim->numFrames - 1 - f;
			lf->frameModel = anim->mdxFile;
		} else {
			lf->frame = anim->firstFrame + f;
			lf->frameModel = anim->mdxFile;
		}

		lf->oldAnimationNumber = lf->animationNumber;
		oldAnim = anim;
//CG_Printf("[fT%6d->%-6d] NF %4d->%-4d s %1.4f sT %d cgT %d\n", lf->oldFrameTime, lf->frameTime, lf->oldFrame, lf->frame, lf->animSpeedScale, snapTime, cg.time);
	}

	// Gordon: BIG hack, occaisionaly (VERY occaisionally), the frametime gets totally wacked
	if( lf->frameTime > cg.time + 5000 ) {
		lf->frameTime = cg.time;
	}

	// calculate current lerp value
	if( lf->frameTime == lf->oldFrameTime ) {
		lf->backlerp = 0;
	} else {
		lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}

/*
===============
CG_ClearLerpFrameRate
===============
*/
void CG_ClearLerpFrameRate( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int animationNumber ) {
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetLerpFrameAnimationRate( cent, ci, lf, animationNumber );
	if( lf->animation ) {
		lf->oldFrame = lf->frame = lf->animation->firstFrame;
		lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
	}
}

/*
===============
CG_PlayerAnimation
===============
*/
static void CG_PlayerAnimation( centity_t *cent, refEntity_t *body ) {
	clientInfo_t	*ci;
	int				clientNum;
	int				animIndex, tempIndex;
	bg_character_t	*character;

	clientNum = cent->currentState.clientNum;

	ci = &cgs.clientinfo[ clientNum ];
	character = CG_CharacterForClientinfo( ci, cent );

	if( !character ) {
		return;
	}

	if( cg_noPlayerAnims.integer ) {
		body->frame = body->oldframe = body->torsoFrame = body->oldTorsoFrame = 0;
		body->frameModel = body->oldframeModel = body->torsoFrameModel= body->oldTorsoFrameModel = character->animModelInfo->animations[0]->mdxFile;
		return;
	}

	// default to whatever the legs are currently doing
	animIndex = cent->currentState.legsAnim;

	// do the shuffle turn frames locally
	if( !(cent->currentState.eFlags & EF_DEAD) && cent->pe.legs.yawing ) {
		//CG_Printf("turn: %i\n", cg.time );
		tempIndex = BG_GetAnimScriptAnimation( clientNum, character->animModelInfo, cent->currentState.aiState, (cent->pe.legs.yawing == SWING_RIGHT ? ANIM_MT_TURNRIGHT : ANIM_MT_TURNLEFT) );
		if (tempIndex > -1) {
			animIndex = tempIndex;
		}
	}
	// run the animation
	CG_RunLerpFrameRate( ci, &cent->pe.legs, animIndex, cent, 0 );

	body->oldframe = cent->pe.legs.oldFrame;
	body->frame = cent->pe.legs.frame;
	body->backlerp = cent->pe.legs.backlerp;
	body->frameModel = cent->pe.legs.frameModel;
	body->oldframeModel = cent->pe.legs.oldFrameModel;

	CG_RunLerpFrameRate( ci, &cent->pe.torso, cent->currentState.torsoAnim, cent, 0 );

	body->oldTorsoFrame = cent->pe.torso.oldFrame;
	body->torsoFrame = cent->pe.torso.frame;
	body->torsoBacklerp = cent->pe.torso.backlerp;
	body->torsoFrameModel = cent->pe.torso.frameModel;
	body->oldTorsoFrameModel = cent->pe.torso.oldFrameModel;
}




/*
=============================================================================

PLAYER ANGLES

=============================================================================
*/

/*
==================
CG_SwingAngles
==================
*/
static void CG_SwingAngles( float destination, float swingTolerance, float clampTolerance,
					float speed, float *angle, qboolean *swinging ) {
	float	swing;
	float	move;
	float	scale;

	if ( !*swinging ) {
		// see if a swing should be started
		float centerAngle;

		// zinx - use predictable center so server can match cgame easier
		centerAngle = rint(*angle / swingTolerance) * swingTolerance;

		swing = AngleSubtract( destination, *angle );
		if ( swing >= swingTolerance || swing < -swingTolerance ) {
			*swinging = qtrue;
		}
	}

	if ( !*swinging ) {
		return;
	}
	
	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract( destination, *angle );
	scale = fabs( swing );
	scale *= 0.05;
	if (scale < 0.5)
		scale = 0.5;



	// swing towards the destination angle
	if ( swing >= 0 ) {
		move = cg.frametime * scale * speed;
		if ( move >= swing ) {
			move = swing;
			*swinging = qfalse;
		} else {
			*swinging = SWING_LEFT;		// left
		}
		*angle = AngleMod( *angle + move );
	} else if ( swing < 0 ) {
		move = cg.frametime * scale * -speed;
		if ( move <= swing ) {
			move = swing;
			*swinging = qfalse;
		} else {
			*swinging = SWING_RIGHT;	// right
		}
		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance ) {
		*angle = AngleMod( destination - (clampTolerance - 1) );
	} else if ( swing < -clampTolerance ) {
		*angle = AngleMod( destination + (clampTolerance - 1) );
	}
}

/*
=================
CG_AddPainTwitch
=================
*/
static void CG_AddPainTwitch( centity_t *cent, vec3_t torsoAngles ) {
	int		t;
	float	f;
	int		duration;
	float	direction;

	if( !cent->pe.animSpeed ) {
		// we need to inititialize this stuff
		cent->pe.painAnimLegs = -1;
		cent->pe.painAnimTorso = -1;
		cent->pe.animSpeed = 1.0;
	}

	if( cent->currentState.eFlags & EF_DEAD ) {
		cent->pe.painAnimLegs = -1;
		cent->pe.painAnimTorso = -1;
		cent->pe.animSpeed = 1.0;
		return;
	}

	if( cent->pe.painDuration ) {
		duration = cent->pe.painDuration;
	} else {
		duration = PAIN_TWITCH_TIME;
	}
	direction = (float)duration * 0.085;
	if (direction > 30)
		direction = 30;
	if (direction < 10)
		direction = 10;
	direction *= (float)(cent->pe.painDirection*2) - 1;

	t = cg.time - cent->pe.painTime;
	if( t >= duration ) {
		return;
	}

	f = 1.0 - (float)t / duration;
	if( cent->pe.painDirection ) {
		torsoAngles[ROLL] += 20 * f;
	} else {
		torsoAngles[ROLL] -= 20 * f;
	}
}

#ifdef __VEHICLES__

float
TankSetAngle ( float angle, float ideal_angle, float speed )
{
	float	move;
	angle = AngleMod( angle );
	ideal_angle = AngleMod( ideal_angle );
	if ( angle == ideal_angle )
	{
		return ( angle );
	}

	move = ideal_angle - angle;
	if ( ideal_angle > angle )
	{
		if ( move > 180.0 )
		{
			move -= 360.0;
		}
	}
	else
	{
		if ( move < -180.0 )
		{
			move += 360.0;
		}
	}

	if ( move > 0 )
	{
		if ( move > speed )
		{
			move = speed;
		}
	}
	else
	{
		if ( move < -speed )
		{
			move = -speed;
		}
	}

	return ( AngleMod( angle + move) );
}

/*
==============
AngleDifference
==============
*/
float AngleDifference(float ang1, float ang2) {
	float diff;

	diff = ang1 - ang2;
	if (ang1 > ang2) {
		if (diff > 180.0) diff -= 360.0;
	}
	else {
		if (diff < -180.0) diff += 360.0;
	}
	return diff;
}

void
CG_LerpTankAngles ( centity_t *cent, vec3_t original_angles, vec3_t slope_angles, float thinktime )
{// UQ1: Adjust the angles over time to make the tank look smooth!
	float	diff, factor, maxchange, anglespeed, fulldiff;
	int		i;

	fulldiff = fabs( AngleDifference( original_angles[0], slope_angles[0]) );
	fulldiff += fabs( AngleDifference( original_angles[1], slope_angles[1]) );
	fulldiff += fabs( AngleDifference( original_angles[2], slope_angles[2]) );

	thinktime = 200/*50*/ * fulldiff;

	if ( slope_angles[PITCH] > 180 )
	{
		slope_angles[PITCH] -= 360;
	}

	//
	factor = 0.15;
	maxchange = 240;
	maxchange *= thinktime;
	for ( i = 0; i < 2; i++ )
	{
		diff = fabs( AngleDifference( original_angles[i], slope_angles[i]) );
		anglespeed = diff * factor;
		if ( anglespeed > maxchange )
		{
			anglespeed = maxchange;
		}

		slope_angles[i] = TankSetAngle( original_angles[i], slope_angles[i], anglespeed );
	}

	if ( slope_angles[PITCH] > 180 )
	{
		slope_angles[PITCH] -= 360;
	}
}

/*
=========================================
		CG_tank_slope -- @fixme should be in pmove
=========================================
*/

#define POS_FW 0
#define POS_BC 1
#define POS_L 2
#define POS_R 3
#define POS_MAX 4

/*
	 _____
	|  0  |
    |     |
    |2   3|
	|     |
	|__1__|
*/

void CG_tank_slope ( centity_t *cent, vec3_t slopeangles ) {  
	int i;
	trace_t	 trace;
	vec3_t	forward, right, up, start, end;
	vec3_t	dir, angles;
	vec3_t	testpos[POS_MAX];
	vec3_t	testangles;
	vec3_t	boxMins= {-8, -8, -8}; // @fixme , tune this to be more smooth on delailed terrian (eg. railroad )
	vec3_t	boxMaxs= {8, 8, 8};
	float	pitch, roll;
	qboolean inAir=qfalse;

	// do not take player viewangle in mind
	testangles[0] = testangles[2] = 0;
	testangles[1] = cent->lerpAngles[1];

	AngleVectors( testangles, forward, right, up );

	for ( i=0; i < POS_MAX; i++ ){
		float fw, rt;

		fw = 64;//32;//tankMins[0]+tankMaxs[0]*0.5; // fixme later i have to be sure now
		rt = 48;//16;//tankMins[1]+tankMaxs[1]*0.5;

		VectorCopy(cent->lerpOrigin, start);
			
		switch (i){
		case POS_FW:
		VectorMA ( start, fw , forward , start);
			break;
		case POS_BC:
		VectorMA ( start, -fw , forward , start);
			break;
		case POS_L:
		VectorMA ( start, -rt , right , start);
			break;
		case POS_R:
		VectorMA ( start, rt , right , start);
			break;
		}

		VectorMA ( start, -32 , up , end);
		VectorMA ( start, 16 , up , start); // raise the start pos

		trap_CM_BoxTrace( &trace, start, end, boxMins, boxMaxs,  0, MASK_PLAYERSOLID );

		VectorCopy(trace.endpos, testpos[i]);

	}
		
// inair check
	if ( cent->currentState.groundEntityNum == ENTITYNUM_NONE ) // note if we use point 0 it feels bad
		inAir = qtrue;
// end


	VectorSubtract(testpos[0], testpos[1], dir);
	vectoangles (dir, angles);

/* suck a lot
	if ( inAir && !cent->pe.lastSlopeInit  ){
		cent->pe.lastSlopeTime = cg.time;
		cent->pe.lastSlopeInit = qtrue;
	} else if ( inAir ) { 
		cent->pe.lastSlope = 0.06*(cg.time - cent->pe.lastSlopeTime); // fixme? 
		if ( cent->pe.lastSlope > 45 ) // clamp it
			cent->pe.lastSlope = 45;
	} else if ( !inAir && cent->pe.lastSlopeInit ) { // bump a bit
		cent->pe.lastSlopeTime = cg.time;
		cent->pe.lastSlopeInit = qfalse;
	} else if ( cg.time - cent->pe.lastSlopeTime < 500 && !cent->pe.lastSlopeInit  ) { 
		cent->pe.lastSlope = -0.05*(cg.time - cent->pe.lastSlopeTime);
	} else if ( cent->pe.lastSlope != 0 ) { // now just go back
		if (cg.time - cent->pe.lastSlopeTime < 1000 && cent->pe.lastSlope < -0.05 )
		cent->pe.lastSlope += 0.05;
		else 
		cent->pe.lastSlope = 0;
		cent->pe.lastSlopeInit = qfalse;
	}*/
 
	pitch=angles[0];//+cent->pe.lastSlope;

	VectorSubtract(testpos[3], testpos[2], dir);
	vectoangles (dir, angles);

	roll=angles[0];

	slopeangles[0]=pitch;
	slopeangles[1]=0;
	slopeangles[2]=roll;

//	CG_Printf("Angles computed: %f %f %f \n" , slopeangles[0], slopeangles[1], slopeangles[2]);


/*
gives angles required to get to "destination" from "from"
say you have 2 traces for the 2 front wheels
use the highest one as the "from" and lowest as the "dest"
*/
	/* Unique1 - Let's try making the changes to slope a little slower (basic physics) */
	if (!cent->last_tank_angles_initialized)
	{
		VectorCopy(slopeangles, cent->last_tank_angles);
		cent->last_tank_angles_initialized = qtrue;
	}

	testangles[0] = cent->last_tank_angles[0];
	CG_LerpTankAngles(cent, testangles, slopeangles, 100 );
	VectorCopy(slopeangles, cent->last_tank_angles);
} 
#endif

extern vmCvar_t cg_realview;

/*
===============
CG_PlayerAngles

Handles seperate torso motion

  legs pivot based on direction of movement

  head always looks exactly at cent->lerpAngles

  if motion < 20 degrees, show in head only
  if < 45 degrees, also show in torso
===============
*/
static void CG_PlayerAngles( centity_t *cent, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] ) {
	vec3_t			legsAngles, torsoAngles, headAngles;
	float			dest;
	vec3_t			velocity;
	float			speed;
	float			clampTolerance;
	int				legsSet, torsoSet;
	clientInfo_t	*ci;
	bg_character_t	*character;
	float	swingSpeed;

	ci = &cgs.clientinfo[ cent->currentState.clientNum ];

	character = CG_CharacterForClientinfo( ci, cent );

	if( !character ) {
		return;
	}

	legsSet = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;
	torsoSet = cent->currentState.torsoAnim & ~ANIM_TOGGLEBIT;

	VectorCopy( cent->lerpAngles, headAngles );
	headAngles[YAW] = AngleMod( headAngles[YAW] );
	VectorClear( legsAngles );
	VectorClear( torsoAngles );

	// --------- yaw -------------

	// allow yaw to drift a bit, unless these conditions don't allow them
	if( !(BG_GetConditionBitFlag( cent->currentState.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE ) ||
		  BG_GetConditionBitFlag( cent->currentState.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLECR ) )/*
		||	 (BG_GetConditionValue( cent->currentState.clientNum, ANIM_COND_MOVETYPE, qfalse ) & ((1<<ANIM_MT_STRAFELEFT) | (1<<ANIM_MT_STRAFERIGHT)) )*/) {

		// always point all in the same direction
		cent->pe.torso.yawing = qtrue;	// always center
		cent->pe.torso.pitching = qtrue;	// always center
		cent->pe.legs.yawing = qtrue;	// always center

	// if firing, make sure torso and head are always aligned
	} else if( BG_GetConditionValue( cent->currentState.clientNum, ANIM_COND_FIRING, qtrue ) ) {
		cent->pe.torso.yawing = qtrue;	// always center
		cent->pe.torso.pitching = qtrue;	// always center
	}

	// adjust legs for movement dir
	if( cent->currentState.eFlags & EF_DEAD || cent->currentState.eFlags & EF_MOUNTEDTANK ) {
		// don't let dead bodies twitch
		legsAngles[YAW] = headAngles[YAW];
		torsoAngles[YAW] = headAngles[YAW];
	} else {
		legsAngles[YAW] = headAngles[YAW] + cent->currentState.angles2[YAW];

		if( !(cent->currentState.eFlags & EF_FIRING) ) {
			torsoAngles[YAW] = headAngles[YAW] + 0.35 * cent->currentState.angles2[YAW];
			clampTolerance = 90;
		} else {	// must be firing
			torsoAngles[YAW] = headAngles[YAW];	// always face firing direction
			//if (fabs(cent->currentState.angles2[YAW]) > 30)
			//	legsAngles[YAW] = headAngles[YAW];
			clampTolerance = 60;
		}

		swingSpeed = cg_swingSpeed.value;
		clampTolerance = 150;

#ifdef __VEHICLES__
		if ( cent->currentState.eFlags & EF_VEHICLE ) 
		{
			CG_SwingAngles( torsoAngles[YAW], 0, clampTolerance, swingSpeed, &cent->pe.torso.yawAngle, &cent->pe.torso.yawing );
			CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, swingSpeed, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );

			torsoAngles[YAW] = cent->pe.torso.yawAngle;
			legsAngles[YAW] = cent->pe.legs.yawAngle;
		} 
		else 
		{
#endif //__VEHICLES__
			// torso
			CG_SwingAngles( torsoAngles[YAW], 25, clampTolerance, swingSpeed, &cent->pe.torso.yawAngle, &cent->pe.torso.yawing );

			// if the legs are yawing (facing heading direction), allow them to rotate a bit, so we don't keep calling
			// the legs_turn animation while an AI is firing, and therefore his angles will be randomizing according to their accuracy

			if( BG_GetConditionBitFlag( ci->clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE) ) {
				cent->pe.legs.yawing = qfalse; // set it if they really need to swing
				CG_SwingAngles( legsAngles[YAW], 20, clampTolerance, 0.5*swingSpeed, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
			} else if( strstr( BG_GetAnimString( character->animModelInfo, legsSet ), "strafe" ) ) {
				cent->pe.legs.yawing = qfalse; // set it if they really need to swing
				legsAngles[YAW] = headAngles[YAW];
				CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, swingSpeed, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
			} else if(cent->pe.legs.yawing) {
				CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, swingSpeed, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
			} else {
				CG_SwingAngles( legsAngles[YAW], 40, clampTolerance, swingSpeed, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
			}

			torsoAngles[YAW] = cent->pe.torso.yawAngle;
			legsAngles[YAW] = cent->pe.legs.yawAngle;
#ifdef __VEHICLES__
		} 
#endif //__VEHICLES__
	}

	// --------- pitch -------------

	// only show a fraction of the pitch angle in the torso
	if( headAngles[PITCH] > 180 ) {
		dest = (-360 + headAngles[PITCH]) * 0.75;
	} else {
		dest = headAngles[PITCH] * 0.75;
	}
	//CG_SwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
	//torsoAngles[PITCH] = cent->pe.torso.pitchAngle;

	if( cent->currentState.eFlags & EF_PRONE ) {
		torsoAngles[PITCH] = legsAngles[PITCH] - 3;
	} else {
		CG_SwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
		torsoAngles[PITCH] = cent->pe.torso.pitchAngle;
	}


	// --------- roll -------------
#ifdef __VEHICLES__
	if (!(cent->currentState.eFlags & EF_VEHICLE))
	{ 
#endif //__VEHICLES__
		// lean towards the direction of travel
		VectorCopy( cent->currentState.pos.trDelta, velocity );
		speed = VectorNormalize( velocity );
		if( speed ) 
		{ 
			vec3_t	axis[3];
			float	side;

			speed *= 0.05;

			AnglesToAxis( legsAngles, axis );
			side = speed * DotProduct( velocity, axis[1] );
			legsAngles[ROLL] -= side;

			side = speed * DotProduct( velocity, axis[0] );
			legsAngles[PITCH] += side;
		} 
#ifdef __VEHICLES__
	} 
#endif //__VEHICLES__ // lol you could remove this one unique :)

#ifdef __VEHICLES__
	// new code
	if ( cent->currentState.eFlags & EF_VEHICLE) 
	{  
		vec3_t	tankslope;

		CG_tank_slope(cent, tankslope);

	
		torsoAngles[0] = headAngles[0] = legsAngles[0] = tankslope[0];
		torsoAngles[2] = headAngles[2] = legsAngles[2] = tankslope[2];

	} 
#endif //__VEHICLES__

	// pain twitch
	CG_AddPainTwitch( cent, torsoAngles );

#if defined (__FRONTLINE__) || defined (__EF__)
	torsoAngles[ROLL] += cent->currentState.constantLight *1.8 ;
	headAngles[ROLL] += cent->currentState.constantLight *1.8 ;
#endif //defined (__FRONTLINE__) || defined (__EF__)

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson && cg_realview.integer > 0 )
	{
		torsoAngles[PITCH] = -90;
	}
	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
}

vec3_t shadow_angles;

static void CG_PlayerShadowAngles( centity_t *cent, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] ) {
	vec3_t			legsAngles, torsoAngles, headAngles;
	float			dest;
	vec3_t			velocity;
	float			speed;
	float			clampTolerance;
	int				legsSet, torsoSet;
	clientInfo_t	*ci;
	bg_character_t	*character;
	float	swingSpeed;

	ci = &cgs.clientinfo[ cent->currentState.clientNum ];

	character = CG_CharacterForClientinfo( ci, cent );

	if( !character ) {
		return;
	}

	legsSet = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;
	torsoSet = cent->currentState.torsoAnim & ~ANIM_TOGGLEBIT;

	VectorCopy( cent->lerpAngles, headAngles );
	headAngles[YAW] = AngleMod( headAngles[YAW] );
	VectorClear( legsAngles );
	VectorClear( torsoAngles );

	// --------- yaw -------------

	// allow yaw to drift a bit, unless these conditions don't allow them
	if( !(BG_GetConditionBitFlag( cent->currentState.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE ) ||
		  BG_GetConditionBitFlag( cent->currentState.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLECR ) )/*
		||	 (BG_GetConditionValue( cent->currentState.clientNum, ANIM_COND_MOVETYPE, qfalse ) & ((1<<ANIM_MT_STRAFELEFT) | (1<<ANIM_MT_STRAFERIGHT)) )*/) {

		// always point all in the same direction
		cent->pe.torso.yawing = qtrue;	// always center
		cent->pe.torso.pitching = qtrue;	// always center
		cent->pe.legs.yawing = qtrue;	// always center

	// if firing, make sure torso and head are always aligned
	} else if( BG_GetConditionValue( cent->currentState.clientNum, ANIM_COND_FIRING, qtrue ) ) {
		cent->pe.torso.yawing = qtrue;	// always center
		cent->pe.torso.pitching = qtrue;	// always center
	}

	// adjust legs for movement dir
	if( cent->currentState.eFlags & EF_DEAD || cent->currentState.eFlags & EF_MOUNTEDTANK ) {
		// don't let dead bodies twitch
		legsAngles[YAW] = headAngles[YAW];
		torsoAngles[YAW] = headAngles[YAW];
	} else {
		legsAngles[YAW] = headAngles[YAW] + cent->currentState.angles2[YAW];

		if( !(cent->currentState.eFlags & EF_FIRING) ) {
			torsoAngles[YAW] = headAngles[YAW] + 0.35 * cent->currentState.angles2[YAW];
			clampTolerance = 90;
		} else {	// must be firing
			torsoAngles[YAW] = headAngles[YAW];	// always face firing direction
			//if (fabs(cent->currentState.angles2[YAW]) > 30)
			//	legsAngles[YAW] = headAngles[YAW];
			clampTolerance = 60;
		}

		swingSpeed = cg_swingSpeed.value;
		clampTolerance = 150;

#ifdef __VEHICLES__
		if ( cent->currentState.eFlags & EF_VEHICLE ) 
		{
			CG_SwingAngles( torsoAngles[YAW], 0, clampTolerance, swingSpeed, &cent->pe.torso.yawAngle, &cent->pe.torso.yawing );
			CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, swingSpeed, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );

			torsoAngles[YAW] = cent->pe.torso.yawAngle;
			legsAngles[YAW] = cent->pe.legs.yawAngle;
		} 
		else 
		{
#endif //__VEHICLES__
			// torso
			CG_SwingAngles( torsoAngles[YAW], 25, clampTolerance, swingSpeed, &cent->pe.torso.yawAngle, &cent->pe.torso.yawing );

			// if the legs are yawing (facing heading direction), allow them to rotate a bit, so we don't keep calling
			// the legs_turn animation while an AI is firing, and therefore his angles will be randomizing according to their accuracy

			if( BG_GetConditionBitFlag( ci->clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE) ) {
				cent->pe.legs.yawing = qfalse; // set it if they really need to swing
				CG_SwingAngles( legsAngles[YAW], 20, clampTolerance, 0.5*swingSpeed, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
			} else if( strstr( BG_GetAnimString( character->animModelInfo, legsSet ), "strafe" ) ) {
				cent->pe.legs.yawing = qfalse; // set it if they really need to swing
				legsAngles[YAW] = headAngles[YAW];
				CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, swingSpeed, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
			} else if(cent->pe.legs.yawing) {
				CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, swingSpeed, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
			} else {
				CG_SwingAngles( legsAngles[YAW], 40, clampTolerance, swingSpeed, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
			}

			torsoAngles[YAW] = cent->pe.torso.yawAngle;
			legsAngles[YAW] = cent->pe.legs.yawAngle;
#ifdef __VEHICLES__
		} 
#endif //__VEHICLES__
	}

	// --------- pitch -------------

	// only show a fraction of the pitch angle in the torso
	if( headAngles[PITCH] > 180 ) {
		dest = (-360 + headAngles[PITCH]) * 0.75;
	} else {
		dest = headAngles[PITCH] * 0.75;
	}
	//CG_SwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
	//torsoAngles[PITCH] = cent->pe.torso.pitchAngle;

	if( cent->currentState.eFlags & EF_PRONE ) {
		torsoAngles[PITCH] = legsAngles[PITCH] - 3;
	} else {
		CG_SwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
		torsoAngles[PITCH] = cent->pe.torso.pitchAngle;
	}


	// --------- roll -------------
#ifdef __VEHICLES__
	if (!(cent->currentState.eFlags & EF_VEHICLE))
	{ 
#endif //__VEHICLES__
		// lean towards the direction of travel
		VectorCopy( cent->currentState.pos.trDelta, velocity );
		speed = VectorNormalize( velocity );
		if( speed ) 
		{ 
			vec3_t	axis[3];
			float	side;

			speed *= 0.05;

			AnglesToAxis( legsAngles, axis );
			side = speed * DotProduct( velocity, axis[1] );
			legsAngles[ROLL] -= side;

			side = speed * DotProduct( velocity, axis[0] );
			legsAngles[PITCH] += side;
		} 
#ifdef __VEHICLES__
	} 
#endif //__VEHICLES__ // lol you could remove this one unique :)

#ifdef __VEHICLES__
	// new code
	if ( cent->currentState.eFlags & EF_VEHICLE) 
	{  
		vec3_t	tankslope;

		CG_tank_slope(cent, tankslope);

	
		torsoAngles[0] = headAngles[0] = legsAngles[0] = tankslope[0];
		torsoAngles[2] = headAngles[2] = legsAngles[2] = tankslope[2];

	} 
#endif //__VEHICLES__

	// pain twitch
	CG_AddPainTwitch( cent, torsoAngles );

#if defined (__FRONTLINE__) || defined (__EF__)
	torsoAngles[ROLL] += cent->currentState.constantLight *1.8 ;
	headAngles[ROLL] += cent->currentState.constantLight *1.8 ;
#endif //defined (__FRONTLINE__) || defined (__EF__)

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
//	headAngles[PITCH] = 90;
//	torsoAngles[PITCH] = 90;
	legsAngles[PITCH] += 90;
	legsAngles[ROLL] += 180;
	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
	VectorCopy(legsAngles, shadow_angles);
}

/*
==============
CG_BreathPuffs
==============
*/
static void CG_BreathPuffs( centity_t *cent, refEntity_t *head ) {
	clientInfo_t	*ci;
	vec3_t			up, forward;
	int				contents;
	vec3_t			mang, morg, maxis[3];

	ci = &cgs.clientinfo[ cent->currentState.number ];

	if( !cg_enableBreath.integer ) {
		return;
	}

	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
		return;
	}

	if( !(cent->currentState.eFlags & EF_DEAD) ) {
		return;
	}

	// allow cg_enableBreath to force everyone to have breath
	if( !(cent->currentState.eFlags & EF_BREATH) ) {
		return;
	}

	contents = CG_PointContents( head->origin, 0 );
	if( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		return;
	}
	if( ci->breathPuffTime > cg.time ) {
		return;
	}

	CG_GetOriginForTag( cent, head, "tag_mouth", 0, morg, maxis );
	AxisToAngles( maxis, mang );
	AngleVectors( mang, forward, NULL, up );

	//push the origin out a tad so it's not right in the guys face (tad==4)
	VectorMA(morg, 4, forward, morg);

	forward[0] = up[0] * 8 + forward[0] * 5;
	forward[1] = up[1] * 8 + forward[1] * 5;
	forward[2] = up[2] * 8 + forward[2] * 5;

	CG_SmokePuff( morg, forward, 4, 1, 1, 1, 0.5f, 2000, cg.time, cg.time + 400, 0, cgs.media.shotgunSmokePuffShader );

	ci->breathPuffTime = cg.time + 3000 + random()*1000;
}

/*
===============
CG_TrailItem
===============
*/
/*static void CG_TrailItem( centity_t *cent, qhandle_t hModel ) {
	refEntity_t		ent;
	vec3_t			angles;
	qboolean		ducking;

	// DHM - Nerve :: Don't draw icon above your own head
	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
		return;

	memset( &ent, 0, sizeof( ent ) );

	VectorCopy( cent->lerpAngles, angles );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
	AnglesToAxis( angles, ent.axis );

	// DHM - Nerve :: adjusted values
	VectorCopy( cent->lerpOrigin, ent.origin );

	// Account for ducking
	if ( cent->currentState.clientNum == cg.snap->ps.clientNum )
		ducking = (cg.snap->ps.pm_flags & PMF_DUCKED);
	else
		ducking = (qboolean)cent->currentState.animMovetype;

	if ( ducking )
		ent.origin[2] += 38;
	else
		ent.origin[2] += 56;

	ent.hModel = hModel;
	trap_R_AddRefEntityToScene( &ent );
}*/

/*
===============
CG_PlayerFloatSprite

Float a sprite over the player's head
DHM - Nerve :: added height parameter
===============
*/
static void CG_PlayerFloatSprite( centity_t *cent, qhandle_t shader, int height ) {
	int				rf;
	refEntity_t		ent;

	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
		rf = RF_THIRD_PERSON;		// only show in mirrors
	} else {
		rf = 0;
	}

	memset( &ent, 0, sizeof( ent ) );
	VectorCopy( cent->lerpOrigin, ent.origin );
	ent.origin[2] += height;			// DHM - Nerve :: was '48'

	// Account for ducking
	if ( cent->currentState.clientNum == cg.snap->ps.clientNum ) {
		if( cg.snap->ps.pm_flags & PMF_DUCKED )
			ent.origin[2] -= 18;
	} else {
		if( (qboolean)cent->currentState.animMovetype )
			ent.origin[2] -= 18;
	}

	ent.reType = RT_SPRITE;
	ent.customShader = shader;
	ent.radius = 6.66;
	ent.renderfx = rf;
	ent.shaderRGBA[0] = 255;
	ent.shaderRGBA[1] = 255;
	ent.shaderRGBA[2] = 255;
	ent.shaderRGBA[3] = 255;
	trap_R_AddRefEntityToScene( &ent );
}

void CG_ListSecondaries_f ( void )
{
	int i;

	for (i = 0; i < MAX_CLIENTS; i++)
	{// One for each client... Much more data we can use for each then!
		centity_t *ent = CG_SecondaryEntityFor(i);

		if (ent->currentState.eType == ET_SECONDARY)
			CG_Printf("Client %i. Secondary is %i. [ET_SECONDARY]\n", i, cg_entities[i].currentState.dl_intensity);
		else
			CG_Printf("Client %i. Secondary is %i. [not ET_SECONDARY]\n", i, cg_entities[i].currentState.dl_intensity);
	}

	for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++)
	{
		centity_t *ent = &cg_entities[i];

		if (!ent)
			continue;

		if (ent->currentState.eType == ET_SECONDARY)
			CG_Printf("Secondary found (ent %i). Linked to client %i.\n", ent->currentState.number, ent->currentState.dl_intensity);
	}
}

/*
===============
CG_PlayerSprites

Float sprites over the player's head
===============
*/
static void CG_PlayerSprites( centity_t *cent ) {
	int		team;

	/*
	cgs.media.coverspotShader = trap_R_RegisterShader( "sprites/voiceChat" );
	cgs.media.coverspotMoveShader = trap_R_RegisterShader( "sprites/balloon3" );

	float			coverspot_start_distance;
	qboolean		last_move_coverspot;
	*/

#ifdef __ETE__
	if (CG_SecondaryEntityFor(cent->currentState.number)->currentState.density == 998 
		&& CG_SecondaryEntityFor(cent->currentState.number)->currentState.dmgFlags == cg.clientNum)
	{// Following my commander (this client)!
		CG_PlayerFloatSprite( cent, cgs.media.coverspotShader, 56 );
	}
	else if (CG_SecondaryEntityFor(cent->currentState.number)->currentState.density == 999)
	{// Moving to cover!
		CG_PlayerFloatSprite( cent, cgs.media.coverspotMoveShader, 56 );
	}
	else if (CG_SecondaryEntityFor(cent->currentState.number)->currentState.density == 1000)
	{// At cover point!
		CG_PlayerFloatSprite( cent, cgs.media.coverspotShader, 56 );
	}
#endif //__ETE__

	if( cent->currentState.powerups & ( 1 << PW_REDFLAG ) ||
		cent->currentState.powerups & ( 1 << PW_BLUEFLAG ) ) {
		CG_PlayerFloatSprite( cent, cgs.media.objectiveShader, 56 );
		return;
	}

//	if ( cent->currentState.eFlags & EF_CONNECTION ) {
//		CG_PlayerFloatSprite( cent, cgs.media.disconnectIcon, 48 );
//		return;
//	}

	if ( cent->currentState.powerups & (1<<PW_INVULNERABLE) ) {
		CG_PlayerFloatSprite( cent, cgs.media.spawnInvincibleShader, 56 );
		return;
	}

	team = cgs.clientinfo[ cent->currentState.clientNum ].team;

	// DHM - Nerve :: If this client is a medic, draw a 'revive' icon over
	//					dead players that are not in limbo yet.
	if( (cent->currentState.eFlags & EF_DEAD)
		&& cent->currentState.number == cent->currentState.clientNum
		&& cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_MEDIC
		&& cg.snap->ps.stats[ STAT_HEALTH ] > 0
		&& cg.snap->ps.persistant[PERS_TEAM] == team ) {

		CG_PlayerFloatSprite( cent, cgs.media.medicReviveShader, 8 );
		return;
	}

	// DHM - Nerve :: show voice chat signal so players know who's talking
	if( cent->voiceChatSpriteTime > cg.time && cg.snap->ps.persistant[PERS_TEAM] == team ) {
		CG_PlayerFloatSprite( cent, cent->voiceChatSprite, 56 );
		return;
	}

	// DHM - Nerve :: only show talk icon to team-mates
	if( cent->currentState.eFlags & EF_TALK && cg.snap->ps.persistant[PERS_TEAM] == team ) {
		CG_PlayerFloatSprite( cent, cgs.media.balloonShader, 48 );
		return;
	}

	{
		fireteamData_t* ft;
		if ((ft = CG_IsOnFireteam( cent->currentState.number ))) {
			if( ft == CG_IsOnFireteam( cg.clientNum ) && cgs.clientinfo[ cent->currentState.number ].selected ) {
				CG_PlayerFloatSprite( cent, cgs.media.fireteamicons[ft->ident], 56 );				
			}
		}		
	}
}

/*
===============
CG_PlayerShadow

Returns the Z component of the surface being shadowed

  should it return a full plane instead of a Z?
===============
*/
#define	SHADOW_DISTANCE		64
#define	ZOFS	6.0
#define	SHADOW_MIN_DIST	250.0
#define	SHADOW_MAX_DIST	512.0

typedef struct {
	char *tagname;
	float size;
	float maxdist;
	float maxalpha;
	qhandle_t shader;
} shadowPart_t;

static qboolean CG_PlayerShadow( centity_t *cent, float *shadowPlane )
{
	vec3_t			end;
	trace_t			trace;
	float			dist, distFade;
	int				tagIndex, subIndex;
	vec3_t			origin, angles, axis[ 3 ];
	vec4_t			projection = { 0, 0, -1, 64 };
	shadowPart_t	shadowParts[] = {
		{"tag_footleft",	10,	4,	1.0,	0},
		{"tag_footright",	10,	4,	1.0,	0},
		{"tag_torso",		18,	96,	0.8,	0},
		{NULL, 0}
	};

	shadowParts[0].shader = cgs.media.shadowFootShader;		//DAJ pulled out of initliization
	shadowParts[1].shader = cgs.media.shadowFootShader;
	shadowParts[2].shader = cgs.media.shadowTorsoShader;

	*shadowPlane = 0;

	if ( cg_shadows.integer == 0 || using_uq_fog ) {
		return qfalse;
	}

	// send a trace down from the player to the ground
	VectorCopy( cent->lerpOrigin, end );
	end[2] -= SHADOW_DISTANCE;

	trap_CM_BoxTrace( &trace, cent->lerpOrigin, end, NULL, NULL, 0, MASK_PLAYERSOLID );

	// no shadow if too high
	//%	if ( trace.fraction == 1.0 || trace.fraction == 0.0f ) {
	//%		return qfalse;
	//%	}

	*shadowPlane = trace.endpos[2] + 1;

	if ( cg_shadows.integer != 1 ) {	// no mark for stencil or projection shadows
		return qtrue;
	}

	// no shadows when dead
	if( cent->currentState.eFlags & EF_DEAD ) {
		return qfalse;
	}
	
	// fade the shadow out with height
	//%	alpha = 1.0 - trace.fraction;

	// add the mark as a temporary, so it goes directly to the renderer
	// without taking a spot in the cg_marks array
	dist = VectorDistance( cent->lerpOrigin, cg.refdef_current->vieworg );	//%	cg.snap->ps.origin );
	distFade = 1.0f;
	if (!(cent->currentState.eFlags & EF_ZOOMING) && (dist > SHADOW_MIN_DIST)) {
		if( dist > SHADOW_MAX_DIST )
		{
			if (dist > SHADOW_MAX_DIST*2)
				return qfalse;
			else	// fade out
				distFade = 1.0f - ((dist - SHADOW_MAX_DIST) / SHADOW_MAX_DIST);
			
			if( distFade > 1.0f )
				distFade = 1.0f;
			else if( distFade < 0.0f )
				distFade = 0.0f;
		}
		
		// set origin
		VectorCopy( cent->lerpOrigin, origin );
		
		// project it onto the shadow plane
		if( origin[2] < *shadowPlane )
			origin[2] = *shadowPlane;
		
		// ydnar: add a bit of height so foot shadows don't clip into sloped geometry as much
		origin[ 2 ] += 18.0f;
		
		//%	alpha *= distFade;
		
		// ydnar: decal remix
		//%	CG_ImpactMark( cgs.media.shadowTorsoShader, trace.endpos, trace.plane.normal, 
		//%		0, alpha,alpha,alpha,1, qfalse, 16, qtrue, -1 );
		CG_ImpactMark( cgs.media.shadowTorsoShader, origin, projection, 18.0f,
				cent->lerpAngles[ YAW ], distFade, distFade, distFade, distFade, -1 );
		return qtrue;
	}
	
	if (dist < SHADOW_MAX_DIST) {	// show more detail
		// now add shadows for the various body parts
		for (tagIndex=0; shadowParts[tagIndex].tagname; tagIndex++ ) {
			// grab each tag with this name
			for (subIndex=0; (subIndex = CG_GetOriginForTag( cent, &cent->pe.bodyRefEnt, shadowParts[tagIndex].tagname, subIndex, origin, axis )) >= 0; subIndex++)
			{
				// project it onto the shadow plane
				if (origin[2] < *shadowPlane)
					origin[2] = *shadowPlane;
				
				// ydnar: add a bit of height so foot shadows don't clip into sloped geometry as much
				origin[ 2 ] += 5.0f;
				
				#if 0
					alpha = 1.0 - ((origin[2] - (*shadowPlane+ZOFS)) / shadowParts[tagIndex].maxdist);
					if (alpha < 0)
						continue;
					if (alpha > shadowParts[tagIndex].maxalpha)
						alpha = shadowParts[tagIndex].maxalpha;
					alpha *= (1.0 - distFade);
					origin[2] = *shadowPlane;
				#endif
				
				AxisToAngles( axis, angles );

				// ydnar: decal remix
				//%	CG_ImpactMark( shadowParts[tagIndex].shader, origin, trace.plane.normal, 
				//%		angles[YAW]/*cent->pe.legs.yawAngle*/, alpha,alpha,alpha,1, qfalse, shadowParts[tagIndex].size, qtrue, -1 );
				
				//%	CG_ImpactMark( shadowParts[ tagIndex ].shader, origin, up, 
				//%			cent->lerpAngles[ YAW ], 1.0f, 1.0f, 1.0f, 1.0f, qfalse, shadowParts[ tagIndex ].size, qtrue, -1 );
				CG_ImpactMark( shadowParts[ tagIndex ].shader, origin, projection, shadowParts[ tagIndex ].size,
						angles[ YAW ], distFade, distFade, distFade, distFade, -1 );
			}
		}
	}

	return qtrue;
}

/*
===============
CG_PlayerSplash

Draw a mark at the water surface
===============
*/
static void CG_PlayerSplash( centity_t *cent ) {
	vec3_t		start, end;
	trace_t		trace;
	int			contents;
	polyVert_t	verts[4];

	if ( !cg_shadows.integer || using_uq_fog ) {
		return;
	}

	VectorCopy( cent->lerpOrigin, end );
	end[2] -= 24;

	// if the feet aren't in liquid, don't make a mark
	// this won't handle moving water brushes, but they wouldn't draw right anyway...
	contents = CG_PointContents( end, 0 );
	if ( !( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) ) {
		return;
	}

	VectorCopy( cent->lerpOrigin, start );
	start[2] += 32;

	// if the head isn't out of liquid, don't make a mark
	contents = CG_PointContents( start, 0 );
	if ( contents & ( CONTENTS_SOLID | CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		return;
	}

	// trace down to find the surface
	trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) );

	if ( trace.fraction == 1.0 ) {
		return;
	}

	// create a mark polygon
	VectorCopy( trace.endpos, verts[0].xyz );
	verts[0].xyz[0] -= 32;
	verts[0].xyz[1] -= 32;
	verts[0].st[0] = 0;
	verts[0].st[1] = 0;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;

	VectorCopy( trace.endpos, verts[1].xyz );
	verts[1].xyz[0] -= 32;
	verts[1].xyz[1] += 32;
	verts[1].st[0] = 0;
	verts[1].st[1] = 1;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;

	VectorCopy( trace.endpos, verts[2].xyz );
	verts[2].xyz[0] += 32;
	verts[2].xyz[1] += 32;
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;

	VectorCopy( trace.endpos, verts[3].xyz );
	verts[3].xyz[0] += 32;
	verts[3].xyz[1] -= 32;
	verts[3].st[0] = 1;
	verts[3].st[1] = 0;
	verts[3].modulate[0] = 255;
	verts[3].modulate[1] = 255;
	verts[3].modulate[2] = 255;
	verts[3].modulate[3] = 255;

	trap_R_AddPolyToScene( cgs.media.wakeMarkShader, 4, verts );
}

//==========================================================================

/*
===============
CG_AddRefEntityWithPowerups

Adds a piece with modifications or duplications for powerups
Also called by CG_Missile for quad rockets, but nobody can tell...
===============
*/
void CG_AddRefEntityWithPowerups( refEntity_t *ent, int powerups, int team, entityState_t *es, const vec3_t fireRiseDir ) {
	centity_t *cent;
	refEntity_t backupRefEnt;//, parentEnt;
	qboolean	onFire=qfalse;
	float		alpha=0.0;
	float		fireStart, fireEnd;

	cent = &cg_entities[es->number];

	ent->entityNum = es->number;

/*	if (cent->pe.forceLOD) {
		ent->reFlags |= REFLAG_FORCE_LOD;
	}*/

	backupRefEnt = *ent;

	if (CG_EntOnFire(&cg_entities[es->number])) {
		ent->reFlags |= REFLAG_FORCE_LOD;
	}

	trap_R_AddRefEntityToScene( ent );

#ifdef __VEHICLES__
	if (!(es->eFlags & EF_VEHICLE) ) 
	{
#endif //__VEHICLES__
		if (!onFire && CG_EntOnFire(&cg_entities[es->number])) 
		{
			onFire = qtrue;
			// set the alpha
			if ( ent->entityNum == cg.snap->ps.clientNum ) {
				fireStart = cg.snap->ps.onFireStart;
				fireEnd = cg.snap->ps.onFireStart + 1500;
			}
			else {
				fireStart = es->onFireStart;
				fireEnd = es->onFireEnd;
			}

			alpha = (cg.time - fireStart) / 1500.0;
			if (alpha > 1.0) {
				alpha = (fireEnd - cg.time) / 1500.0;
				if (alpha > 1.0) {
					alpha = 1.0;
				}
			}
		}

		if (onFire) {
			if (alpha < 0.0) alpha = 0.0;
			ent->shaderRGBA[3] = (unsigned char)(255.0*alpha);
			VectorCopy( fireRiseDir, ent->fireRiseDir );
			if (VectorCompare(ent->fireRiseDir, vec3_origin)) {
				VectorSet( ent->fireRiseDir, 0, 0, 1 );
			}
			ent->customShader = cgs.media.onFireShader;
			trap_R_AddRefEntityToScene( ent );

			ent->customShader = cgs.media.onFireShader2;
			trap_R_AddRefEntityToScene( ent );

			if (ent->hModel == cent->pe.bodyRefEnt.hModel)
				trap_S_AddLoopingSound( ent->origin, vec3_origin, cgs.media.flameCrackSound, (int)(255.0*alpha), 0 );
		} 
#ifdef __VEHICLES__
	} // tank
#endif //__VEHICLES__

	*ent = backupRefEnt;
}

char	*vtosf( const vec3_t v ) {
	static	int		index;
	static	char	str[8][64];
	char	*s;

	// use an array so that multiple vtos won't collide
	s = str[index];
	index = (index + 1)&7;

	Com_sprintf (s, 64, "(%f %f %f)", v[0], v[1], v[2]);

	return s;
}


/*
===============
CG_AnimPlayerConditions

	predict, or calculate condition for this entity, if it is not the local client
===============
*/
void CG_AnimPlayerConditions( bg_character_t *character, centity_t *cent ) {
	entityState_t	*es;
	int				legsAnim;

	if( !character ) {
		return;
	}
	if( cg.snap && cg.snap->ps.clientNum == cent->currentState.number && !cg.renderingThirdPerson && cent->currentState.eType != ET_NPC && cent->currentState.eType != ET_NPC_CORPSE ) {
		return;
	}

	es = &cent->currentState;

	// WEAPON
	if( es->eFlags & EF_ZOOMING ) {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_WEAPON, WP_BINOCULARS, qtrue );
#if defined (__FRONTLINE__) || defined (__EF__)
	} else if( (es->eFlags & EF_AIMING )
		&& es->weapon != WP_AKIMBO_COLT
		&& es->weapon != WP_AKIMBO_SILENCEDCOLT
		&& es->weapon != WP_AKIMBO_LUGER
		&& es->weapon != WP_AKIMBO_SILENCEDLUGER
		&& es->weapon != WP_COLT 
		&& es->weapon != WP_SILENCED_COLT 
		&& es->weapon != WP_SILENCED_LUGER 
		&& es->weapon != WP_PANZERFAUST 
		&& es->weapon != WP_LUGER) {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_WEAPON, WP_K43_SCOPE, qtrue );
#endif //defined (__FRONTLINE__) || defined (__EF__)
	} else {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_WEAPON, es->weapon, qtrue );
	}

	// MOUNTED
	if( (es->eFlags & EF_MG42_ACTIVE) || (es->eFlags & EF_MOUNTEDTANK) ) {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOUNTED, MOUNTED_MG42, qtrue );
	} else if (es->eFlags & EF_AAGUN_ACTIVE) {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOUNTED, MOUNTED_AAGUN, qtrue );
	} else {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOUNTED, MOUNTED_UNUSED, qtrue );
	}

	// UNDERHAND
	BG_UpdateConditionValue( es->clientNum, ANIM_COND_UNDERHAND, cent->lerpAngles[0] > 0, qtrue );

	if( es->eFlags & EF_CROUCHING ) {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_CROUCHING, qtrue, qtrue );
	} else {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_CROUCHING, qfalse, qtrue );
	}

	if( es->eFlags & EF_FIRING ) {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_FIRING, qtrue, qtrue );
	} else {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_FIRING, qfalse, qtrue );
	}

	// reverse engineer the legs anim -> movetype (if possible)
	legsAnim = es->legsAnim & ~ANIM_TOGGLEBIT;
	if( character->animModelInfo->animations[legsAnim]->movetype ) {
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOVETYPE, character->animModelInfo->animations[legsAnim]->movetype, qfalse );
	}

}

/*
typedef struct animation_s {
	qhandle_t	mdxFile;
	char		name[MAX_QPATH];
	int			firstFrame;
	int			numFrames;
	int			loopFrames;			// 0 to numFrames
	int			frameLerp;			// msec between frames
	int			initialLerp;		// msec to get to first frame
	int			moveSpeed;
	int			animBlend;			// take this long to blend to next anim
} animation_t;
*/

static animation_t multi_tankAnims[] = {
	{ 0,	"",	0,		1,		0,		1000/15,	1000/15	},	// idle
	{ 0,	"",	50,		15,		15,		1000/15,	1000/15	},	// moving
	{ 0,	"",	40,		10,		0,		1000/15,	1000/15	},	// stopping
	{ 0,	"",	50,		-10,	0,		1000/15,	1000/15	},	// starting
	{ 0,	"",	65,		10,		0,		1000/7,		1000/7	},	// shooting
};

// dhm - end


/*
==============
CG_TankSetAnim
==============
*/
static void CG_TankSetAnim(centity_t *cent, lerpFrame_t *lf, int newAnim) {
	// transition animation
/*	lf->animationNumber =   cent->currentState.frame;

	lf->animation = &multi_tankAnims[ cent->currentState.frame ];*/
	lf->animationNumber =   newAnim;

	lf->animation = &multi_tankAnims[ newAnim ];


	lf->animationTime = lf->frameTime + lf->animation->initialLerp;
}

#ifdef __VEHICLES__
void CG_tank_turret_slope ( centity_t *cent, vec3_t slopeangles ) {  
	int i;
	trace_t	 trace;
	vec3_t	forward, right, up, start, end;
	vec3_t	dir, angles;
	vec3_t	testpos[POS_MAX];
	vec3_t	testangles;
	vec3_t	boxMins= {-8, -8, -8}; // @fixme , tune this to be more smooth on delailed terrian (eg. railroad )
	vec3_t	boxMaxs= {8, 8, 8};
	float	pitch, roll;
	qboolean inAir=qfalse;

	// do not take player viewangle in mind
	testangles[0] = testangles[2] = 0;
	testangles[1] = cent->lerpAngles[1];

	AngleVectors( testangles, forward, right, up );

	for ( i=0; i < POS_MAX; i++ ){
		float fw, rt;

		fw = 64;//32;//tankMins[0]+tankMaxs[0]*0.5; // fixme later i have to be sure now
		rt = 48;//16;//tankMins[1]+tankMaxs[1]*0.5;

		VectorCopy(cent->lerpOrigin, start);
			
		switch (i){
		case POS_FW:
		VectorMA ( start, fw , forward , start);
			break;
		case POS_BC:
		VectorMA ( start, -fw , forward , start);
			break;
		case POS_L:
		VectorMA ( start, -rt , right , start);
			break;
		case POS_R:
		VectorMA ( start, rt , right , start);
			break;
		}

		VectorMA ( start, -32 , up , end);
		VectorMA ( start, 16 , up , start); // raise the start pos

		trap_CM_BoxTrace( &trace, start, end, boxMins, boxMaxs,  0, MASK_PLAYERSOLID );

		VectorCopy(trace.endpos, testpos[i]);
	}
		
// inair check
	if ( cent->currentState.groundEntityNum == ENTITYNUM_NONE ) // note if we use point 0 it feels bad
		inAir = qtrue;
// end

	VectorSubtract(testpos[1], testpos[0], dir);
	vectoangles (dir, angles);

	pitch=angles[0];//+cent->pe.lastSlope;

	VectorSubtract(testpos[2], testpos[3], dir);
	vectoangles (dir, angles);

	roll=angles[0];

	slopeangles[0]=pitch;
	slopeangles[1]=0;
	slopeangles[2]=roll;

	/* Unique1 - Let's try making the changes to slope a little slower (basic physics) */
	if (!cent->last_tank_angles_initialized)
	{
		VectorCopy(slopeangles, cent->last_tank_turret_angles);
		cent->last_tank_turret_angles_initialized = qtrue;
	}

	testangles[0] = cent->last_tank_turret_angles[0];
	CG_LerpTankAngles(cent, testangles, slopeangles, 100 );
	VectorCopy(slopeangles, cent->last_tank_turret_angles);
} 

/*
=================================
CG_TANK

  @Fixme , a lot of to clear and to do
  also copy changed code to NPC

=================================
*/

void CG_TankLightCorona( vec3_t org, vec3_t dir, vec3_t color, float scale )
{
	trace_t		tr;
	int			r, g, b;
	qboolean	visible = qfalse,
				behind = qfalse,
				toofar = qfalse;

	float dot, dist;

	if(cg_coronas.integer == 0) {	// if set to '0' no coronas
		return;
	}

	r = color[0];
	g = color[1];
	b = color[2];

	// only coronas that are in your PVS are being added

	VectorSubtract(cg.refdef_current->vieworg, org, dir);

	dist = VectorNormalize2(dir, dir);

#ifdef __ETE__
	if (cg_coronafardist.integer < 4096)
	{// UQ1: Longer range on coronas for ETE...
		trap_Cvar_Set( "cg_coronafardist", "4096" );
	}
#endif //__ETE__

	if(dist > cg_coronafardist.integer) {	// performance variable cg_coronafardist will keep down super long traces
		toofar = qtrue;
	}

	dot = DotProduct(dir, cg.refdef_current->viewaxis[0] );
	if(dot>=-0.6)		// assumes ~90 deg fov	(SA) changed value to 0.6 (screen corner at 90 fov)
		behind = qtrue;		// use the dot to at least do trivial removal of those behind you.
							// yeah, I could calc side planes to clip against, but would that be worth it? (much better than dumb dot>= thing?)

//	CG_Printf("dot: %f\n", dot);

	if(cg_coronas.integer == 2) {	// if set to '2' trace everything
		behind = qfalse;
		toofar = qfalse;
	}


	if(!behind && !toofar) {
		CG_Trace( &tr, cg.refdef_current->vieworg, NULL, NULL, org, -1, MASK_SOLID|CONTENTS_BODY );	// added blockage by players.  not sure how this is going to be since this is their bb, not their model (too much blockage)

#ifdef __ETE__
		if(tr.fraction == 1 || VectorDistance(tr.endpos, org) <= 24)
			visible = qtrue;
#else //!__ETE__
		if(tr.fraction == 1)
			visible = qtrue;
#endif //__ETE__

		trap_R_AddCoronaToScene( org, (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)scale / 255.0f, -1, visible);
	}
}

void CG_Tank(centity_t *cent) {
	entityState_t		*cs;
	lerpFrame_t			*tanklf;
	refEntity_t		body;
	refEntity_t		barrel;
	refEntity_t		turret;
	refEntity_t		flash;
	refEntity_t		tracks;
	refEntity_t     player;//Masteries
	vec3_t			playerOrigin = { 0,0,0 };
	vec3_t			lightorigin;
	int				clientNum;
	int				renderfx;
	//static int	startTime;
	qboolean	thirdperson=qfalse;
	vec3_t	velocity;
	int		speed;

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.clientNum;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		CG_Error( "Bad clientNum on player entity");
	}

	memset( &body, 0, sizeof(body) );
	memset( &turret, 0, sizeof(turret) );
	memset( &barrel, 0, sizeof(barrel) );
	memset( &flash, 0, sizeof( flash ) );
	memset( &tracks, 0, sizeof( tracks ) );
    memset( &player, 0,	sizeof( player ));//Masteries

	VectorCopy( cent->lerpOrigin, playerOrigin );

	// get the rotation information @TODO fix all there
	CG_PlayerAngles( cent, body.axis, body.torsoAxis, turret.axis );


	// copy the torso rotation to the accessories
	AxisCopy( body.torsoAxis, barrel.axis );
	AxisCopy( body.torsoAxis, turret.axis );

	renderfx = 0;
	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
		thirdperson = qtrue;			// only draw in mirrors
	}

	// draw the player in cameras
	if( cg.cameraMode )
		thirdperson = qfalse;
	//	renderfx &= ~RF_THIRD_PERSON;
	

	renderfx |= RF_LIGHTING_ORIGIN;			// use the same origin for all

	VectorCopy( playerOrigin, lightorigin );
	lightorigin[2] += 31;

	// get a speed
	VectorCopy(cent->currentState.pos.trDelta, velocity);
	speed = VectorNormalize(velocity);

    //////////////
	//ANIMATIONS//


	cs = &cent->currentState;

	tanklf = &cent->lerpFrame;

	// initial setup
	if(!tanklf->oldFrameTime) {
		tanklf->frameTime		=
		tanklf->oldFrameTime	= cg.time;

		CG_TankSetAnim(cent, tanklf, 0);

		tanklf->frame			=
		tanklf->oldFrame		= tanklf->animation->firstFrame;
	}

	if( cg.time - cent->muzzleFlashTime < 1200 && tanklf->animationNumber !=4  ) {  
		cent->pe.lightningFiring = 1;	// dont play start/stop sounds
		CG_TankSetAnim(cent, tanklf,  4);
	} else if ( cg.time - cent->muzzleFlashTime >= 1200 && cent->pe.lightningFiring == 1) {
		cent->pe.lightningFiring = 2;
	}

	if ( speed > 15  ) {  
		if ( tanklf->animationNumber != 1 &&  cent->pe.lightningFiring != 1  ) {  
			CG_TankSetAnim(cent, tanklf,  1);
			if ( cent->pe.lightningFiring == 2  ) {
			cent->pe.lightningFiring = 0;
			} else {
			//trap_S_StartSound (NULL, cs->number, CHAN_AUTO, cgs.media.tankStartSound ); // weird :(
			}
		} else {  
		trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.tankMoveSound, 255, 0 );
		}  
	} else {  
		if ( tanklf->animationNumber != 0 &&  cent->pe.lightningFiring != 1 ) {  
			CG_TankSetAnim(cent, tanklf,  0);
			if ( cent->pe.lightningFiring == 2  ) {
			cent->pe.lightningFiring = 0;
			} else {
			trap_S_StartSound (NULL, cs->number, CHAN_AUTO, cgs.media.tankStopSound ); 
			}
		} else {  
		trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.tankIdleSound, 255, 0 );
		}  
	}   


	CG_RunLerpFrame(cent, NULL, tanklf, 0, 1);	// use existing lerp code rather than re-writing

	body.frame		= tanklf->frame;
	body.oldframe	= tanklf->oldFrame;
	body.backlerp	= tanklf->backlerp;
	//
	// RENDERING
	//

	body.customSkin	= 0;
	body.hModel		= cgs.media.tankModel[cent->currentState.effect1Time];

	VectorCopy( playerOrigin, body.origin );
	VectorCopy( lightorigin, body.lightingOrigin );
	body.renderfx = renderfx;
	VectorCopy( body.origin, body.oldorigin );	// don't positionally lerp at all

	cent->pe.bodyRefEnt = body;

	// (SA) only need to set this once...
	VectorCopy( lightorigin, barrel.lightingOrigin );
	VectorCopy( lightorigin, turret.lightingOrigin );
	VectorCopy( lightorigin, tracks.lightingOrigin );
	barrel.renderfx = renderfx;
	turret.renderfx = renderfx;
	tracks.renderfx = renderfx;

	CG_AddRefEntityWithPowerups( &body,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );

///////////////// COAX MuzzleFlash,Masteries Antitank Artillery doesn´t have machinegun!
	if ( (cent->currentState.effect1Time != VEHICLE_TYPE_FLAK88) && (cent->currentState.effect1Time != VEHICLE_TYPE_PAK) ){ 
		vec3_t	angles;

		flash.hModel = cgs.media.tankFlashModel;

		angles[YAW]		= 0;
		angles[PITCH]	= 0;
		angles[ROLL]	= crandom() * 10;
		AnglesToAxis( angles, flash.axis );


		CG_PositionRotatedEntityOnTag( &flash, &body, "tag_flash2");

		VectorCopy (flash.origin ,cent->pe.coaxMuzzle);

		if ( cg.time - cent->muzzleFlashTime2 < MUZZLE_FLASH_TIME) { 
			CG_AddRefEntityWithPowerups( &flash,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );	
			trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );
		}

	} 

	////////////////// Churchill tower/barrel or other tanks  Masteries, I do and extended or condition for easier way to add new tanks
	if ( (cent->currentState.effect1Time == VEHICLE_TYPE_CHURCHILL) 
		|| (cent->currentState.effect1Time == VEHICLE_TYPE_KV1) 
		|| (cent->currentState.effect1Time == VEHICLE_TYPE_PANZERIV) 
		|| (cent->currentState.effect1Time == VEHICLE_TYPE_TIGERTANK)
		|| (cent->currentState.effect1Time == VEHICLE_TYPE_FLAMETANK)
		|| (cent->currentState.effect1Time == VEHICLE_TYPE_T34) 
		|| (cent->currentState.effect1Time == VEHICLE_TYPE_BT7) 
		|| (cent->currentState.effect1Time == VEHICLE_TYPE_PANZER2) ){ 
		vec3_t	turretAngles;
		vec3_t	tankAngles;

		AxisToAngles( body.axis, tankAngles );

		if (cent->currentState.eFlags & EF_TURRETMOVE ) {  
			trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.tankTurretSound, 255, 0 );
			cent->pe.turretRotating = qtrue;
			cent->pe.turretStopTime = cg.time;
		} else if ( cg.time - cent->pe.turretStopTime < 300 ) {
			trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.tankTurretSound, 255, 0 );
		} else if (cent->pe.turretRotating && cg.time - cent->pe.turretStopTime > 400 ){
			trap_S_StartSound (NULL, cs->number, CHAN_AUTO, cgs.media.tankTurretEndSound );
			cent->pe.turretRotating = qfalse;
		}

		// UQ1: Fixed the turret rotation bug! -- it was a bitch! :)
		turret.hModel = cgs.media.tankTurretModel[cent->currentState.effect1Time];
		turret.customSkin = 0;
		
		turretAngles[PITCH] = 0;
		turretAngles[YAW] = cent->currentState.effect2Time - tankAngles[YAW];
		turretAngles[ROLL] = 0;

		if (cent->currentState.effect1Time == VEHICLE_TYPE_TIGERTANK) 
		{// UQ1: Tag is backwards on Tiger???
			turretAngles[PITCH] = -180;
			turretAngles[YAW] = 360-(cent->currentState.effect2Time - tankAngles[YAW]);
			turretAngles[YAW] -= 180;
		}

		AnglesToAxis( turretAngles, turret.axis );
		//CG_PositionEntityOnTag( &turret,	&body,	"tag_turret", 0, NULL);
		CG_PositionRotatedEntityOnTag( &turret,	&body,	"tag_turret" );
		CG_AddRefEntityWithPowerups( &turret,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );

		barrel.hModel = cgs.media.tankBarrelModel[cent->currentState.effect1Time];
		turretAngles[PITCH] = 0 - cent->currentState.effect3Time /*- tankAngles[PITCH]*/;
		turretAngles[YAW] = 0;

		if (cent->currentState.effect1Time == VEHICLE_TYPE_KV1) 
		{// UQ1: Tag is backwards on KV1???
			turretAngles[PITCH] = cent->currentState.effect3Time;
			turretAngles[YAW] -= 180; 
		}

		AnglesToAxis( turretAngles, barrel.axis );	
		//CG_PositionEntityOnTag( &barrel,	&turret,	"tag_barrel", 0, NULL);
		CG_PositionRotatedEntityOnTag( &barrel,	&turret,	"tag_barrel" );
		CG_AddRefEntityWithPowerups( &barrel,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );

		//turretAngles[YAW] = cent->currentState.effect2Time - tankAngles[YAW];
		turretAngles[YAW] = cent->currentState.effect2Time;
		AnglesToAxis( turretAngles, flash.axis );
		CG_PositionEntityOnTag( &flash, &barrel, "tag_flash", 0, NULL);

		if( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )	{   
			CG_AddRefEntityWithPowerups( &flash, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );	
			trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );
		}

		if (cent->currentState.effect1Time == VEHICLE_TYPE_FLAMETANK)
		{// Record turret angle and position for flamer...
			vec3_t offset;

			VectorSet( offset, 0, 0, 0 );

			turretAngles[PITCH] = cent->currentState.effect3Time;
			turretAngles[YAW] = cent->currentState.effect2Time;
			VectorCopy(turretAngles, cent->flameTankTurretAngles);
			VectorCopy(CG_GetTagOrigin(barrel, "tag_flash", &offset), cent->flameTankTurretOrigin);
		}
////////////////////////// Jagdpanther barrel and others self propelled guns like stug3
	} else if ( cent->currentState.effect1Time == VEHICLE_TYPE_PANZER || cent->currentState.effect1Time == VEHICLE_TYPE_STUG3) { 
		vec3_t	turretAngles;
		vec3_t	tankAngles;

		VectorCopy( cent->lerpAngles, turretAngles);
		turretAngles[2] = 0;
		AxisToAngles( body.axis, tankAngles );

		turretAngles[1] = cent->currentState.effect2Time;
		turretAngles[0] = tankAngles[0]+cent->currentState.effect3Time;
		turretAngles[2] = tankAngles[2];

		barrel.hModel = cgs.media.tankBarrelModel[cent->currentState.effect1Time];
		barrel.customSkin = 0;
		CG_PositionEntityOnTag( &barrel,	&body,	"tag_turret", 0, NULL);
		AnglesToAxis( turretAngles, barrel.axis );	
		CG_AddRefEntityWithPowerups( &barrel,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );

		//Masteries, for attach the player model to the antitank gun; doesn´t work?
        if (( cent->currentState.effect1Time == VEHICLE_TYPE_FLAK88) || ( cent->currentState.effect1Time == VEHICLE_TYPE_PAK))
		{
			CG_PositionEntityOnTag( &player,	&body,	"tag_playerpo", 0, NULL);
		} 

		CG_PositionEntityOnTag( &flash, &barrel, "tag_flash", 0, NULL);
		AnglesToAxis( turretAngles, flash.axis );

		if( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )	
		{   
			CG_AddRefEntityWithPowerups( &flash, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );	
			trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );
		}  	   
	} else if (cent->currentState.effect1Time == VEHICLE_TYPE_FLAK88) { 
		vec3_t	turretAngles;
		vec3_t	tankAngles;

		VectorCopy( cent->lerpAngles, turretAngles);
		turretAngles[2] = 0;
		AxisToAngles( body.axis, tankAngles );

		turretAngles[1] = cent->currentState.effect2Time;
		turretAngles[0] = tankAngles[0]+cent->currentState.effect3Time;
		turretAngles[2] = tankAngles[2];

		barrel.hModel = cgs.media.tankBarrelModel[cent->currentState.effect1Time];
		barrel.customSkin = 0;
		CG_PositionEntityOnTag( &barrel,	&body,	"tag_barrel", 0, NULL);
		AnglesToAxis( turretAngles, barrel.axis );	
		CG_AddRefEntityWithPowerups( &barrel,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );

		//Masteries, for attach the player model to the antitank gun; doesn´t work? -- UQ1: Working great!
        if (( cent->currentState.effect1Time == VEHICLE_TYPE_FLAK88) || ( cent->currentState.effect1Time == VEHICLE_TYPE_PAK))
		{
			CG_PositionEntityOnTag( &player,	&body,	"tag_playerpo", 0, NULL);
		} 

		CG_PositionEntityOnTag( &flash, &barrel, "tag_flash", 0, NULL);
		AnglesToAxis( turretAngles, flash.axis );

		if( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )	
		{   
			CG_AddRefEntityWithPowerups( &flash, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );	
			trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );
		}  	   
	} else if (cent->currentState.effect1Time == VEHICLE_TYPE_PAK) { 
		vec3_t	turretAngles;
		vec3_t	tankAngles;

		VectorCopy( cent->lerpAngles, turretAngles);
		turretAngles[2] = 0;
		AxisToAngles( body.axis, tankAngles );

		turretAngles[1] = cent->currentState.effect2Time;
		turretAngles[0] = tankAngles[0]+cent->currentState.effect3Time;
		turretAngles[2] = tankAngles[2];

		barrel.hModel = cgs.media.tankBarrelModel[cent->currentState.effect1Time];
		barrel.customSkin = 0;
		CG_PositionEntityOnTag( &barrel,	&body,	"tag_turret", 0, NULL);
		AnglesToAxis( turretAngles, barrel.axis );	
		CG_AddRefEntityWithPowerups( &barrel,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );

		//Masteries, for attach the player model to the antitank gun; doesn´t work? -- UQ1: Working great!
        if (( cent->currentState.effect1Time == VEHICLE_TYPE_FLAK88) || ( cent->currentState.effect1Time == VEHICLE_TYPE_PAK))
		{
			CG_PositionEntityOnTag( &player,	&body,	"tag_playerpo", 0, NULL);
		} 

		CG_PositionEntityOnTag( &flash, &barrel, "tag_flash", 0, NULL);
		AnglesToAxis( turretAngles, flash.axis );

		if( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )	
		{   
			CG_AddRefEntityWithPowerups( &flash, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );	
			trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );
		}  	
	}

	{// UQ1: Add any white lights (with beam and corona)...
		vec3_t offset;
		vec3_t color;
		float scale = 1.0f;
		int num = 0;

		VectorSet( offset, 0, 0, 0 );
		VectorSet( color, 255, 255, 255 );

		for (num = 1; num < 4; num++)
		{
			orientation_t or;

			if( trap_R_LerpTag( &or, &body, va("tag_light%i", num), 0 ) >= 0 )
			{// Have this light!
				vec3_t lightOrigin, lightAngles, normal;

				VectorCopy(CG_GetTagOrigin(body, va("tag_light%i", num), &offset), lightOrigin);
				AxisToAngles( or.axis, lightAngles );

				AngleVectors( lightAngles, normal, NULL, NULL );

				CG_TankLightCorona( lightOrigin, normal, color, scale );
				trap_R_AddLightToScene2( lightOrigin/*normal*/, 256, 100.0f, color[0], color[1], color[2], 0, REF_DIRECTED_DLIGHT );
			}
		}
	}

	{// UQ1: Add any red lights (corona only)...
		vec3_t offset;
		vec3_t color;
		float scale = 1.0f;
		int num = 0;

		VectorSet( offset, 0, 0, 0 );
		VectorSet( color, 64, 64, 255 );

		for (num = 1; num < 4; num++)
		{
			orientation_t or;

			if( trap_R_LerpTag( &or, &body, va("tag_redlight%i", num), 0 ) >= 0 )
			{// Have this light!
				vec3_t lightOrigin, lightAngles, normal;

				VectorCopy(CG_GetTagOrigin(body, va("tag_redlight%i", num), &offset), lightOrigin);
				AxisToAngles( or.axis, lightAngles );

				AngleVectors( lightAngles, normal, NULL, NULL );

				CG_TankLightCorona( lightOrigin, normal, color, scale );
				//trap_R_AddLightToScene( normal, 256, 100.0f, color[0], color[1], color[2], 0, REF_DIRECTED_DLIGHT );
			}
		}
	}

// THIS WILL SET UP TRACKS SHADERS
// @FIXME this is very ugly code
	VectorCopy( body.origin, tracks.origin);
	AxisCopy( body.axis, tracks.axis );

	// UQ1: Cleaned up a bit...
	if (speed > 0)
	{
		switch (cent->currentState.effect1Time)
		{
		case VEHICLE_TYPE_FLAK88:
		case VEHICLE_TYPE_PAK:
			break; // No tracks...
		case VEHICLE_TYPE_CHURCHILL:
		case VEHICLE_TYPE_KV1:
		case VEHICLE_TYPE_PANZERIV:
        case VEHICLE_TYPE_T34:
		case VEHICLE_TYPE_BT7:
		case VEHICLE_TYPE_FLAMETANK:
		case VEHICLE_TYPE_TIGERTANK:
		case VEHICLE_TYPE_PANZER2:
			tracks.customShader	= cgs.media.tankShader[0];
			tracks.hModel = cgs.media.tankTracksModel[cent->currentState.effect1Time];
			CG_AddRefEntityWithPowerups( &tracks,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
			break;
		case VEHICLE_TYPE_PANZER:
		case VEHICLE_TYPE_STUG3:
			tracks.customShader	= cgs.media.tankShader[1];
			tracks.hModel = cgs.media.tankTracksModel[cent->currentState.effect1Time];
			CG_AddRefEntityWithPowerups( &tracks,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
			break;
		case VEHICLE_TYPE_AXIS_APC:
		case VEHICLE_TYPE_ALLIED_APC:
			tracks.customShader	= cgs.media.tankShader[0];
			tracks.hModel = cgs.media.tankTracksModel[cent->currentState.effect1Time];
			CG_AddRefEntityWithPowerups( &tracks,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
			break;
		default:
			break;
		}
	}
	else if (speed < 0)
	{// UQ1: Reverse???
		switch (cent->currentState.effect1Time)
		{
		case VEHICLE_TYPE_FLAK88:
		case VEHICLE_TYPE_PAK:
			break; // No tracks...
		case VEHICLE_TYPE_CHURCHILL:
		case VEHICLE_TYPE_KV1:
		case VEHICLE_TYPE_PANZERIV:
        case VEHICLE_TYPE_T34:
		case VEHICLE_TYPE_BT7:
		case VEHICLE_TYPE_FLAMETANK:
		case VEHICLE_TYPE_TIGERTANK:
		case VEHICLE_TYPE_PANZER2:
			tracks.customShader	= cgs.media.tankShader[4];
			tracks.hModel = cgs.media.tankTracksModel[cent->currentState.effect1Time];
			CG_AddRefEntityWithPowerups( &tracks,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
			break;
		case VEHICLE_TYPE_PANZER:
		case VEHICLE_TYPE_STUG3:
			tracks.customShader	= cgs.media.tankShader[5];
			tracks.hModel = cgs.media.tankTracksModel[cent->currentState.effect1Time];
			CG_AddRefEntityWithPowerups( &tracks,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
			break;
		case VEHICLE_TYPE_AXIS_APC:
		case VEHICLE_TYPE_ALLIED_APC:
			tracks.customShader	= cgs.media.tankShader[4];
			tracks.hModel = cgs.media.tankTracksModel[cent->currentState.effect1Time];
			CG_AddRefEntityWithPowerups( &tracks,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
			break;
		default:
			break;
		}
	}
	else
	{
		switch (cent->currentState.effect1Time)
		{
		case VEHICLE_TYPE_FLAK88:
		case VEHICLE_TYPE_PAK:
			break; // No tracks...
		case VEHICLE_TYPE_CHURCHILL:
		case VEHICLE_TYPE_KV1:
		case VEHICLE_TYPE_PANZERIV:
        case VEHICLE_TYPE_T34:
		case VEHICLE_TYPE_BT7:
		case VEHICLE_TYPE_FLAMETANK:
		case VEHICLE_TYPE_TIGERTANK:
			tracks.customShader	= cgs.media.tankShader[2];
			tracks.hModel = cgs.media.tankTracksModel[cent->currentState.effect1Time];
			CG_AddRefEntityWithPowerups( &tracks,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
			break;
		case VEHICLE_TYPE_PANZER:
		case VEHICLE_TYPE_STUG3:
			tracks.customShader	= cgs.media.tankShader[3];
			tracks.hModel = cgs.media.tankTracksModel[cent->currentState.effect1Time];
			CG_AddRefEntityWithPowerups( &tracks,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
			break;
		case VEHICLE_TYPE_AXIS_APC:
		case VEHICLE_TYPE_ALLIED_APC:
			tracks.customShader	= cgs.media.tankShader[2];
			tracks.hModel = cgs.media.tankTracksModel[cent->currentState.effect1Time];
			CG_AddRefEntityWithPowerups( &tracks,	cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
			break;
		default:
			break;
		}
	}

//	memcpy( &cent->refEnt, &body, sizeof(refEntity_t) );

}

#endif

#ifdef __ETE__
extern void CG_CheckClientVisibility ( centity_t *cent );
#endif //__ETE__

void CG_RealPlayerShadow( centity_t *cent, vec3_t playerOrigin2 )
{
#ifdef __REAL_SHADOWS__
	clientInfo_t	*ci;
	refEntity_t		body;
	refEntity_t		head;
	refEntity_t		acc;
	vec3_t			lightorigin;
	int				clientNum,i;
	int				renderfx;
	qboolean		shadow;
	float			shadowPlane;
//	float			gumsflappin = 0;	// talking amplitude
	qboolean		usingBinocs = qfalse;
	centity_t		*cgsnap;
	bg_character_t	*character;
	float			hilightIntensity = 0.f;
	vec3_t			playerOrigin;
	vec3_t			forward;

	VectorCopy(playerOrigin2, playerOrigin);
	//playerOrigin[2]-=34;
	playerOrigin[2]+=32;
	//playerOrigin[2]+=40;

	cgsnap = &cg_entities[cg.snap->ps.clientNum];

	shadow = qfalse;												// gjd added to make sure it was initialized
	shadowPlane = 0.0;												// ditto

	// if set to invisible, skip
	if( cent->currentState.eFlags & EF_NODRAW ) {
		return;
	}

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.clientNum;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		CG_Error( "Bad clientNum on player entity");
	}
	ci = &cgs.clientinfo[ clientNum ];

/*#ifdef __WEAPON_AIM__
#ifndef __FRONTLINE__
	if ((cent->currentState.eFlags & EF_AIMING) 
		&& cent->currentState.weapon != WP_COLT 
		&& cent->currentState.weapon != WP_LUGER 
		&& cent->currentState.weapon != WP_PANZERFAUST 
		&& cent->currentState.weapon != WP_MOBILE_MG42 
#ifdef __EF__
		&& cent->currentState.weapon != WP_30CAL 
#endif //__EF__
		&& !( cent->currentState.eFlags & EF_PRONE ) )
		cent->currentState.torsoAnim = 78;
#endif //!__FRONTLINE__
#endif //__WEAPON_AIM__ // YUCK!!*/

	// it is possible to see corpses from disconnected players that may
	// not have valid clientinfo
	if ( !ci->infoValid ) {
		return;
	}

	character = CG_CharacterForClientinfo( ci, cent );

	memset( &body, 0, sizeof(body) );
	memset( &head, 0, sizeof(head) );
	memset( &acc, 0, sizeof(acc) );

	// get the rotation information
	CG_PlayerShadowAngles( cent, body.axis, body.torsoAxis, head.axis );

	AngleVectors (shadow_angles, forward, NULL, NULL);
	VectorMA( playerOrigin, 82, forward, playerOrigin );

	// FIXME: move this into CG_PlayerAngles
	if(	cgsnap == cent && (cg.snap->ps.pm_flags & PMF_LADDER) )
		memcpy( body.torsoAxis, body.axis, sizeof(body.torsoAxis) );

	// copy the torso rotation to the accessories
	AxisCopy( body.torsoAxis, acc.axis );

	// calculate client-side conditions
	CG_AnimPlayerConditions( character, cent );

	// get the animation state (after rotation, to allow feet shuffle)
	CG_PlayerAnimation( cent, &body );


	// forcibly set binoc animation
	if( cent->currentState.eFlags & EF_ZOOMING ) {
		usingBinocs = qtrue;
	}

	// get the player model information
	renderfx = 0;
	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
		renderfx = RF_THIRD_PERSON;			// only draw in mirrors
	}

	// draw the player in cameras
	if( cg.cameraMode )
		renderfx &= ~RF_THIRD_PERSON;

	if( cg_shadows.integer == 3 && shadow ) {
		renderfx |= RF_SHADOW_PLANE;
	}

	renderfx |= RF_LIGHTING_ORIGIN;			// use the same origin for all

	// set renderfx for accessories
	acc.renderfx = renderfx;

	VectorCopy( playerOrigin, lightorigin );
	lightorigin[2] += 31;

	{
		vec3_t	dist;
		vec_t	distSquared;

		VectorSubtract( lightorigin, cg.refdef_current->vieworg, dist );
		distSquared = VectorLengthSquared( dist );
		if( distSquared > Square(384.f) ) {
			renderfx |= RF_MINLIGHT;

			distSquared -= Square(384.f);

			if( distSquared > Square(768.f) ) {
                hilightIntensity = 1.f;
			} else {
				hilightIntensity = 1.f * (distSquared / Square(768.f));
			}

			//CG_Printf( "%f\n", hilightIntensity );
		}
	}

	body.hilightIntensity = hilightIntensity;
	head.hilightIntensity = hilightIntensity;
	acc.hilightIntensity = hilightIntensity;

	//
	// add the body
	//
	body.customSkin	= shadow_skin;
	body.hModel		= character->mesh;

	VectorCopy( playerOrigin, body.origin );
	VectorCopy( lightorigin, body.lightingOrigin );
	body.shadowPlane = shadowPlane;
	body.renderfx = renderfx;
	VectorCopy( body.origin, body.oldorigin );	// don't positionally lerp at all

	//cent->pe.bodyRefEnt = body;

	// if the model failed, allow the default nullmodel to be displayed
	// Gordon: whoever wrote that comment sucks
	if( !body.hModel ) {
		return;
	}

	// (SA) only need to set this once...
	VectorCopy( lightorigin, acc.lightingOrigin );

	CG_AddRefEntityWithPowerups( &body,	cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );

	//
	// add the head
	//

	if( !(head.hModel = character->hudhead) ) {
		return;
	}

	head.customSkin = character->hudheadskin;

	head.customSkin	= shadow_skin;
	head.customShader = shadow_shader;

	VectorCopy( lightorigin, head.lightingOrigin );

	CG_PositionRotatedEntityOnTag( &head, &body, "tag_head" );

	head.shadowPlane = shadowPlane;
	head.renderfx = renderfx;

	if( cent->currentState.eFlags & EF_FIRING ) {
		cent->pe.lastFiredWeaponTime = 0;
		cent->pe.weaponFireTime += cg.frametime;
	} else {
		if( cent->pe.weaponFireTime > 500 && cent->pe.weaponFireTime ) {
			cent->pe.lastFiredWeaponTime = cg.time;
		}

		cent->pe.weaponFireTime = 0;
	}

	if( cent->currentState.eType != ET_CORPSE && cent->currentState.eType != ET_NPC_CORPSE && !(cent->currentState.eFlags & EF_DEAD) ) {
		hudHeadAnimNumber_t anim;

		if( cent->pe.weaponFireTime > 500 ) {
			anim = HD_ATTACK;
		} else if( cg.time - cent->pe.lastFiredWeaponTime < 500 ) {
			anim = HD_ATTACK_END;
		} else {
			anim = HD_IDLE1;
		}

		CG_HudHeadAnimation( character, &cent->pe.head, &head.oldframe, &head.frame, &head.backlerp, anim );
	} else {
		head.frame = 0;
		head.oldframe = 0;
		head.backlerp = 0.f;
	}

	// set blinking flag
	CG_AddRefEntityWithPowerups( &head, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );

	cent->pe.headRefEnt = head;

	// add the 

	//shadow = CG_PlayerShadow( cent, &shadowPlane );

	// set the shadowplane for accessories
	//acc.shadowPlane = shadowPlane;

	//CG_BreathPuffs( cent, &head );

	//
	// add the gun / barrel / flash
	//
//	if( !(cent->currentState.eFlags & EF_DEAD) ) {			// NERVE - SMF
//		CG_AddPlayerWeapon( &body, NULL, cent );
//	}

	//
	// add binoculars (if it's not the player)
	//
//	if( usingBinocs ) {			// NERVE - SMF
//		acc.hModel = cgs.media.thirdPersonBinocModel;
//		CG_PositionEntityOnTag( &acc, &body, "tag_weapon", 0, NULL );
//		CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
//	}

	//
	// add accessories
	//
	for( i = ACC_BELT_LEFT; i < ACC_MAX; i++ ) {
		if( !(character->accModels[i]) )
			continue;
		acc.hModel = character->accModels[i];
		acc.customSkin = character->accSkins[i];

		acc.customSkin	= shadow_skin;
		acc.customShader = shadow_shader;

			// Gordon: looted corpses dont have any accsserories, evil looters :E
			if( !((cent->currentState.eType == ET_CORPSE || cent->currentState.eType == ET_NPC_CORPSE) && cent->currentState.time2 == 1 )) {
				switch(i)
				{
					case ACC_BELT_LEFT:
						CG_PositionEntityOnTag( &acc,	&body,	"tag_bright", 0, NULL);
						break;
					case ACC_BELT_RIGHT:
						CG_PositionEntityOnTag( &acc,	&body,	"tag_bleft", 0, NULL);
						break;
					case ACC_BELT:
						CG_PositionEntityOnTag( &acc,	&body,	"tag_ubelt", 0, NULL);
						break;
					case ACC_BACK:
						CG_PositionEntityOnTag( &acc,	&body,	"tag_back", 0, NULL);
						break;
					case ACC_HAT:			//hat
					case ACC_RANK:
						if( cent->currentState.eFlags & EF_HEADSHOT ) {
							continue;
						}
					case ACC_MOUTH2:		// hat2
					case ACC_MOUTH3:		// hat3					
						if( i == ACC_RANK ) {
							if( ci->rank == 0 ) {
								continue;
							}
							acc.customShader = rankicons[ ci->rank ][ 1 ].shader;

							acc.customSkin	= shadow_skin;
							acc.customShader = shadow_shader;
						}
						CG_PositionEntityOnTag( &acc,	&head,	"tag_mouth", 0, NULL);
						break;
				
					// weapon and weapon2
					// these are used by characters who have permanent weapons attached to their character in the skin
					case ACC_WEAPON:	// weap
						CG_PositionEntityOnTag( &acc,	&body,	"tag_weapon", 0, NULL);
						break;
					case ACC_WEAPON2:	// weap2
						CG_PositionEntityOnTag( &acc,	&body,	"tag_weapon2", 0, NULL);
						break;
					default:
						continue;
				}
				CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
			}
	}
#endif //__REAL_SHADOWS__
}

#ifdef __ETE__
extern void CG_AddDirtBulletParticles( vec3_t origin, vec3_t dir, int speed, int duration, int count, float randScale, float width, float height, float alpha, qhandle_t shader);
extern void CG_ImpactMark( qhandle_t markShader, vec3_t origin, vec4_t projection, float radius, float orientation, float r, float g, float b, float a, int lifeTime );

extern qboolean snowstep_kludge_map;

void CG_AddSnowFootStepParticles ( centity_t *cent, refEntity_t body )
{
	vec3_t end, dir;
	vec4_t projection;
	trace_t tr;

	if (cent->currentState.eType == ET_PLAYER && cgs.clientinfo[cent->currentState.number].team == TEAM_SPECTATOR)
		return;

	VectorSet(dir, 0, 0, 0);

	VectorCopy(cent->lerpOrigin, end);
	end[2]-=48;

	CG_Trace( &tr, cent->lerpOrigin, NULL, NULL, end, cent->currentState.number, MASK_ALL/*MASK_PLAYERSOLID*/);

	if(tr.fraction == 1)
	{
		return; // In the air, no footsteps...
	}

	if (!(tr.surfaceFlags & SURF_SNOW)/* || !(snowstep_kludge_map && ((tr.surfaceFlags & SURF_GRAVEL) || (tr.surfaceFlags & SURF_GRASS)))*/ )
	{
		return; // Not on snow, no snow footsteps...
	}

	VectorSet( projection, 0, 0, -1 );
	CG_AddDirtBulletParticles( tr.endpos, dir, 190, 900, 15, 0.05, 20, 8, 0.2, cgs.media.dirtParticle2Shader);
	//CG_ImpactMark( cgs.media.burnMarkShader, tr.endpos, projection, 30.0f, random() * 360.0f, 1.0f, 1.0f, 1.0f, 1.0f, cg_bloodTime.integer * 1000 );
}
#endif //__ETE__

extern int Q_TrueRand ( int low, int high );
extern vmCvar_t cg_realview_dist;
extern float current_realview_offset;
extern vec3_t	current_realview_forward;

#ifdef __VEHICLES__
extern void CG_RegisterTank ( int type );
#endif //__VEHICLES__

/*
===============
CG_Player
===============
*/
void CG_Player( centity_t *cent )
{
	clientInfo_t	*ci;
	refEntity_t		body;
	refEntity_t		head;
	refEntity_t		acc;
	vec3_t			playerOrigin;
	vec3_t			lightorigin;
	int				clientNum,i;
	int				renderfx;
	qboolean		shadow;
	float			shadowPlane;
//	float			gumsflappin = 0;	// talking amplitude
	qboolean		usingBinocs = qfalse;
	centity_t		*cgsnap;
	bg_character_t	*character;
	float			hilightIntensity = 0.f;

	cgsnap = &cg_entities[cg.snap->ps.clientNum];

	shadow = qfalse;												// gjd added to make sure it was initialized
	shadowPlane = 0.0;												// ditto

	// if set to invisible, skip
	if( cent->currentState.eFlags & EF_NODRAW ) {
		return;
	}

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.clientNum;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		CG_Error( "Bad clientNum on player entity");
	}
	ci = &cgs.clientinfo[ clientNum ];

/*#ifdef __WEAPON_AIM_
#ifndef __FRONTLINE__
	if ((cent->currentState.eFlags & EF_AIMING) 
		&& cent->currentState.weapon != WP_COLT 
		&& cent->currentState.weapon != WP_LUGER 
		&& cent->currentState.weapon != WP_PANZERFAUST 
		&& cent->currentState.weapon != WP_MOBILE_MG42 
		&& !( cent->currentState.eFlags & EF_PRONE ) )
		cent->currentState.torsoAnim = 78;
#endif
#endif //__WEAPON_AIM__ // YUCK!!*/

	// it is possible to see corpses from disconnected players that may
	// not have valid clientinfo
	if ( !ci->infoValid ) {
		return;
	}

#ifdef __VEHICLES__
	if ( cent->currentState.eFlags & EF_VEHICLE ) 
	{// UQ1: New dynamic loading of vehicle models as required! Let's save memory!
		CG_RegisterTank( cent->currentState.effect1Time );
	}
#endif //__VEHICLES__

	character = CG_CharacterForClientinfo( ci, cent );

	if( cent->currentState.eFlags & EF_MOUNTEDTANK ) {
		VectorCopy( cg_entities[ cg_entities[ cent->currentState.clientNum ].tagParent ].mountedMG42Player.origin, playerOrigin );
	} else if( cent->currentState.eFlags & EF_MG42_ACTIVE || cent->currentState.eFlags & EF_AAGUN_ACTIVE ) { 	// Arnout: see if we're attached to a gun
		centity_t *mg42;
		int num;

		// find the mg42 we're attached to
		for ( num = 0 ; num < cg.snap->numEntities ; num++ ) {
			mg42 = &cg_entities[ cg.snap->entities[ num ].number ];
			if( mg42->currentState.eType == ET_MG42_BARREL &&
				mg42->currentState.otherEntityNum == cent->currentState.number ) {
				// found it, clamp behind gun
				vec3_t	forward, right, up;

				//AngleVectors (mg42->s.apos.trBase, forward, right, up);
				AngleVectors ( cent->lerpAngles, forward, right, up );
				VectorMA ( mg42->currentState.pos.trBase, -36, forward, playerOrigin );
				playerOrigin[2] = cent->lerpOrigin[2];
				break;
			}
		}

		if( num == cg.snap->numEntities ) {
			VectorCopy( cent->lerpOrigin, playerOrigin );
		}
	} else {
		if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson && cg_realview.integer > 0 )
		{
			/*trace_t	tr;
			vec3_t	origin, origin2, angles2, forward;
			float		shortest_frac = 1;

			VectorCopy(cg.refdef.vieworg, origin);
		
			VectorCopy(cent->lerpAngles, angles2);
			angles2[PITCH]=0;
			AngleVectors (angles2, forward, NULL, NULL);
			VectorCopy(origin, origin2);
			VectorMA( origin, cg_realview_dist.value, forward, origin2 );

			CG_Trace(&tr, origin, NULL, NULL, origin2, cg.clientNum, MASK_PLAYERSOLID);
			shortest_frac = tr.fraction;

			while (origin[2]-8 > cent->lerpOrigin[2])
			{
				origin[2]-=8;
				origin2[2]-=8;
				CG_Trace(&tr, origin, NULL, NULL, origin2, cg.clientNum, MASK_PLAYERSOLID);
			
				if (tr.fraction < shortest_frac)
					shortest_frac = tr.fraction;
			}

			current_realview_offset = shortest_frac;

			if (shortest_frac == 1)
			{// Can see all the way, use normal origin...
				VectorCopy( cent->lerpOrigin, playerOrigin );
			}
			else
			{// We can not see all the way.. We need to adjust the position for the 3rd person model...
				float dist = cg_realview_dist.value * (1-shortest_frac);
				
				//CG_Printf("RealView: Body moved back by %f.\n", (float)(0-dist));

				if (dist <= 0)
					dist = 2;

				VectorCopy(cent->lerpOrigin, origin2);
				VectorMA( origin2, 0-dist, forward, origin2 );
				VectorCopy(origin2, playerOrigin);
			}*/

			/*if (current_realview_offset == 1)
			{// Can see all the way, use normal origin...
				VectorCopy( cent->lerpOrigin, playerOrigin );
			}
			else
			{// We can not see all the way.. We need to adjust the position for the 3rd person model...
				float		dist = cg_realview_dist.value * (1-current_realview_offset);
				vec3_t	origin2;
				
				if (dist <= 0)
					dist = 1;

				CG_Printf("RealView: Body moved back by %f.\n", (float)(0-dist));

				VectorCopy(cent->lerpOrigin, origin2);
				VectorMA( origin2, 0-dist, current_realview_forward, origin2 );
				VectorCopy(origin2, playerOrigin);
			}

			VectorMA( playerOrigin, 0-cg_realview_dist.value, current_realview_forward, playerOrigin );*/
			VectorCopy( cent->lerpOrigin, playerOrigin );
			VectorMA( playerOrigin, 0-cg_realview_dist.value, current_realview_forward, playerOrigin );
		}
		else
		{
			VectorCopy( cent->lerpOrigin, playerOrigin );
		}
	}

	// UQ1: Test - Player Shadows
	//CG_RealPlayerShadow( cent, playerOrigin );

	memset( &body, 0, sizeof(body) );
	memset( &head, 0, sizeof(head) );
	memset( &acc, 0, sizeof(acc) );

	// get the rotation information
	CG_PlayerAngles( cent, body.axis, body.torsoAxis, head.axis );

	// FIXME: move this into CG_PlayerAngles
	if(	cgsnap == cent && (cg.snap->ps.pm_flags & PMF_LADDER) )
		memcpy( body.torsoAxis, body.axis, sizeof(body.torsoAxis) );

	// copy the torso rotation to the accessories
	AxisCopy( body.torsoAxis, acc.axis );

	// calculate client-side conditions
	CG_AnimPlayerConditions( character, cent );

	// get the animation state (after rotation, to allow feet shuffle)
	CG_PlayerAnimation( cent, &body );


	// forcibly set binoc animation
	if( cent->currentState.eFlags & EF_ZOOMING ) {
		usingBinocs = qtrue;
	}

	// add the any sprites hovering above the player
	// rain - corpses don't get icons (fireteam check ran out of bounds)
	if( cent->currentState.eType != ET_CORPSE && cent->currentState.eType != ET_NPC_CORPSE ) {
		CG_PlayerSprites( cent );
	}

	// add a water splash if partially in and out of water
	CG_PlayerSplash( cent );

	// get the player model information
	renderfx = 0;

	if( cent->currentState.number == cg.snap->ps.clientNum 
		&& !cg.renderingThirdPerson 
		&& cg_realview.integer > 0 
		&& cgs.gamestate != GS_INTERMISSION 
		&& cgs.gamestate != GS_RESET)
	{

	}
	else if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
		renderfx = RF_THIRD_PERSON;			// only draw in mirrors
	}

	// draw the player in cameras
	if( cg.cameraMode )
		renderfx &= ~RF_THIRD_PERSON;

	if( cg_shadows.integer == 3 && shadow ) {
		renderfx |= RF_SHADOW_PLANE;
	}

	renderfx |= RF_LIGHTING_ORIGIN;			// use the same origin for all

	// set renderfx for accessories
	acc.renderfx = renderfx;

	VectorCopy( playerOrigin, lightorigin );
	lightorigin[2] += 31;

	{
		vec3_t	dist;
		vec_t	distSquared;

		VectorSubtract( lightorigin, cg.refdef_current->vieworg, dist );
		distSquared = VectorLengthSquared( dist );
		if( distSquared > Square(384.f) ) {
			renderfx |= RF_MINLIGHT;

			distSquared -= Square(384.f);

			if( distSquared > Square(768.f) ) {
                hilightIntensity = 1.f;
			} else {
				hilightIntensity = 1.f * (distSquared / Square(768.f));
			}

			//CG_Printf( "%f\n", hilightIntensity );
		}
	}

	body.hilightIntensity = hilightIntensity;
	head.hilightIntensity = hilightIntensity;
	acc.hilightIntensity = hilightIntensity;

	//
	// add the body
	//
	if( (cent->currentState.eType == ET_CORPSE || cent->currentState.eType == ET_NPC_CORPSE) && cent->currentState.time2 == 1 ) {
		body.hModel		= character->undressedCorpseModel;
		body.customSkin	= character->undressedCorpseSkin;
	} else if (cent->currentState.eFlags & EF_CLOAKED && ((cent->currentState.eFlags & EF_PRONE ) || (cent->currentState.eFlags & EF_CROUCHING))) {
		body.customSkin	= stlth_skin;
		body.hModel		= character->mesh;
	} else {
		body.customSkin	= character->skin;
		body.hModel		= character->mesh;
	}

	VectorCopy( playerOrigin, body.origin );
	VectorCopy( lightorigin, body.lightingOrigin );
	body.shadowPlane = shadowPlane;
	body.renderfx = renderfx;
	VectorCopy( body.origin, body.oldorigin );	// don't positionally lerp at all

	cent->pe.bodyRefEnt = body;

	// if the model failed, allow the default nullmodel to be displayed
	// Gordon: whoever wrote that comment sucks
	if( !body.hModel ) {
		return;
	}

	// (SA) only need to set this once...
	VectorCopy( lightorigin, acc.lightingOrigin );

/*	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson && cg_realview.integer > 0 )
	{
		float scale = 1.3;

		body.origin[2]+=16;
		body.oldorigin[2]+=16;
		VectorScale( body.axis[0], scale, body.axis[0] );
		VectorScale( body.axis[1], scale, body.axis[1] );
		VectorScale( body.axis[2], scale, body.axis[2] );
		body.nonNormalizedAxes = qtrue;
	}
	else
	{
		if (!cent->player_scaled)
		{
			cent->player_scale = (Q_TrueRand(100, 130)*0.01);
			cent->player_scaled = qtrue;
		}
		
		body.origin[2]+=(cent->player_scale*12);
		body.oldorigin[2]+=(cent->player_scale*12);
		VectorScale( body.axis[0], cent->player_scale, body.axis[0] );
		VectorScale( body.axis[1], cent->player_scale, body.axis[1] );
		VectorScale( body.axis[2], cent->player_scale, body.axis[2] );
		body.nonNormalizedAxes = qtrue;
	}*/

	CG_AddRefEntityWithPowerups( &body,	cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );

	// ydnar debug
	#if 0
	{
		int		y;
		vec3_t	oldOrigin;
		
		VectorCopy( body.origin, oldOrigin );
		body.origin[ 0 ] -= 20;
		//body.origin[ 0 ] -= 20 * 36;
		for( y = 0; y < 40; y++ )
		{
			body.origin[ 0 ] += 1;
			//body.origin[ 0 ] += 36;
			//body.origin[ 2 ] = BG_GetGroundHeightAtPoint( body.origin ) + (oldOrigin[2] - BG_GetGroundHeightAtPoint( oldOrigin ));
			body.frame += (y & 1) ? 1 : -1;
			body.oldframe += (y & 1) ? -1 : 1;
			CG_AddRefEntityWithPowerups( &body,	cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
		}
		VectorCopy( oldOrigin, body.origin );
	}
	#endif
	
// DEBUG
	/*{
		int			x, zd, zu;
		vec3_t		bmins, bmaxs;

		x = (cent->currentState.solid & 255);
		zd = ((cent->currentState.solid>>8) & 255);
		zu = ((cent->currentState.solid>>16) & 255) - 32;

		bmins[0] = bmins[1] = -x;
		bmaxs[0] = bmaxs[1] = x;
		bmins[2] = -zd;
		bmaxs[2] = zu;

		VectorAdd( bmins, cent->lerpOrigin, bmins );
		VectorAdd( bmaxs, cent->lerpOrigin, bmaxs );

		CG_RailTrail( NULL, bmins, bmaxs, 1 );
	}*/

	/*{
		orientation_t tag;
		int idx;
		vec3_t start;
		vec3_t ends[3];
		vec3_t axis[3];
		trap_R_LerpTag( &tag, &body, "tag_head", 0 );

		VectorCopy( body.origin, start );

		for( idx = 0; idx < 3; idx++ ) {
			VectorMA( start, tag.origin[idx], body.axis[idx], start );
		}

		MatrixMultiply( tag.axis, body.axis, axis );

		for( idx = 0; idx < 3; idx++ ) {
			VectorMA( start, 32, axis[idx], ends[idx] );
			CG_RailTrail2( NULL, start, ends[idx] );
		}
	}
	{
		vec3_t mins, maxs;
		VectorCopy( cg.predictedPlayerState.mins, mins );
		VectorCopy( cg.predictedPlayerState.maxs, maxs );

		if( cg.predictedPlayerState.eFlags & EF_PRONE ) {
			maxs[2] = maxs[2] - (cg.predictedPlayerState.standViewHeight - PRONE_VIEWHEIGHT + 8);
		} else if( cg.predictedPlayerState.pm_flags & PMF_DUCKED ) {
			maxs[2] = cg.predictedPlayerState.crouchMaxZ;
		}

		VectorAdd( cent->lerpOrigin, mins, mins );
		VectorAdd( cent->lerpOrigin, maxs, maxs );
		CG_RailTrail( NULL, mins, maxs, 1 );

		if( cg.predictedPlayerState.eFlags & EF_PRONE ) {
			vec3_t org, forward;

			// The legs
			VectorCopy( playerlegsProneMins, mins );
			VectorCopy( playerlegsProneMaxs, maxs );

			AngleVectors( cent->lerpAngles, forward, NULL, NULL );
			forward[2] = 0;
			VectorNormalizeFast( forward );

			org[0] = cent->lerpOrigin[0] + forward[0] * -32;
			org[1] = cent->lerpOrigin[1] + forward[1] * -32;
			org[2] = cent->lerpOrigin[2] + cg.pmext.proneLegsOffset;

			VectorAdd( org, mins, mins );
			VectorAdd( org, maxs, maxs );
			CG_RailTrail( NULL, mins, maxs, 1 );

			// And the head
			VectorSet( mins, -6, -6, -22 );
			VectorSet( maxs, 6, 6, -10 );

			org[0] = cent->lerpOrigin[0] + forward[0] * 12;
			org[1] = cent->lerpOrigin[1] + forward[1] * 12;
			org[2] = cent->lerpOrigin[2];

			VectorAdd( org, mins, mins );
			VectorAdd( org, maxs, maxs );
			CG_RailTrail( NULL, mins, maxs, 1 );
		}
	}*/
// DEBUG

	//
	// add the head
	//

	if( !(head.hModel = character->hudhead) ) {
		return;
	}
	head.customSkin = character->hudheadskin;

	if (cent->currentState.eFlags & EF_CLOAKED && ((cent->currentState.eFlags & EF_PRONE ) || (cent->currentState.eFlags & EF_CROUCHING)))
	{
		head.customSkin	= stlth_skin;
		head.customShader = stlth_shader;
	}

	VectorCopy( lightorigin, head.lightingOrigin );

	CG_PositionRotatedEntityOnTag( &head, &body, "tag_head" );
/*
#ifdef __PARTICLE_SYSTEM__
	if (CG_EntOnFire(cent))
	{// UQ1: Add particle system flames if on fire!
		if (cent->currentState.eType == ET_CORPSE)
		{
			centity_t *owner = &cg_entities[cent->currentState.clientNum];

			if (owner->flameTrailPS)
				CG_DestroyParticleSystem( &owner->flameTrailPS );
		}
		else if (!cent->flameTrailPS)
		{
			if (cgs.media.onFirePS <= 0)
				cgs.media.onFirePS = CG_RegisterParticleSystem( "onFirePS" );

			if (cent->flameTrailPS)
				CG_DestroyParticleSystem( &cent->flameTrailPS );

			cent->flameTrailPS = CG_SpawnNewParticleSystem( cgs.media.onFirePS );
			CG_SetParticleSystemTag( cent->flameTrailPS, body, body.hModel, "tag_torso" ); //tag_chest //tag_head //tag_armleft //tag_armright //tag_legleft //tag_legright
			CG_SetParticleSystemCent( cent->flameTrailPS, cent );
			CG_AttachParticleSystemToTag( cent->flameTrailPS );
		}
		else
		{
			CG_SetParticleSystemTag( cent->flameTrailPS, body, body.hModel, "tag_torso" ); //tag_chest //tag_head //tag_armleft //tag_armright //tag_legleft //tag_legright
			CG_SetParticleSystemCent( cent->flameTrailPS, cent );
			CG_AttachParticleSystemToTag( cent->flameTrailPS );
		}
	}
	else
	{
		if (cent->flameTrailPS)
			CG_DestroyParticleSystem( &cent->flameTrailPS );

		if (cent->currentState.eType == ET_CORPSE)
		{
			centity_t *owner = &cg_entities[cent->currentState.clientNum];

			if (owner->flameTrailPS)
				CG_DestroyParticleSystem( &owner->flameTrailPS );
		}
	}
#endif //__PARTICLE_SYSTEM__
*/
	head.shadowPlane = shadowPlane;
	head.renderfx = renderfx;

	if( cent->currentState.eFlags & EF_FIRING ) {
		cent->pe.lastFiredWeaponTime = 0;
		cent->pe.weaponFireTime += cg.frametime;
	} else {
		if( cent->pe.weaponFireTime > 500 && cent->pe.weaponFireTime ) {
			cent->pe.lastFiredWeaponTime = cg.time;
		}

		cent->pe.weaponFireTime = 0;
	}

	if( cent->currentState.eType != ET_CORPSE && cent->currentState.eType != ET_NPC_CORPSE && !(cent->currentState.eFlags & EF_DEAD) ) {
		hudHeadAnimNumber_t anim;

		if( cent->pe.weaponFireTime > 500 ) {
			anim = HD_ATTACK;
		} else if( cg.time - cent->pe.lastFiredWeaponTime < 500 ) {
			anim = HD_ATTACK_END;
		} else {
			anim = HD_IDLE1;
		}

		CG_HudHeadAnimation( character, &cent->pe.head, &head.oldframe, &head.frame, &head.backlerp, anim );
	} else {
		head.frame = 0;
		head.oldframe = 0;
		head.backlerp = 0.f;
	}

	
	// set blinking flag
	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson && cg_realview.integer > 0 )
	{

	}
	else
		CG_AddRefEntityWithPowerups( &head, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );

	cent->pe.headRefEnt = head;

	// add the 

	shadow = CG_PlayerShadow( cent, &shadowPlane );

	// set the shadowplane for accessories
	acc.shadowPlane = shadowPlane;

	CG_BreathPuffs( cent, &head );

	//
	// add the gun / barrel / flash
	//
	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson && cg_realview.integer > 0 )
	{

	}
	else
		if( !(cent->currentState.eFlags & EF_DEAD) ) {			// NERVE - SMF
			CG_AddPlayerWeapon( &body, NULL, cent );
		}

	//
	// add binoculars (if it's not the player)
	//
	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson && cg_realview.integer > 0 )
	{

	}
	else
		if( usingBinocs ) {			// NERVE - SMF
			acc.hModel = cgs.media.thirdPersonBinocModel;
			CG_PositionEntityOnTag( &acc, &body, "tag_weapon", 0, NULL );
			CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
		}

	//
	// add accessories
	//
	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson && cg_realview.integer > 0 )
	{

	}
	else
	for( i = ACC_BELT_LEFT; i < ACC_MAX; i++ ) {
		if( !(character->accModels[i]) )
			continue;
		acc.hModel = character->accModels[i];
		acc.customSkin = character->accSkins[i];

		if (cent->currentState.eFlags & EF_CLOAKED && ((cent->currentState.eFlags & EF_PRONE ) || (cent->currentState.eFlags & EF_CROUCHING)))
		{
			acc.customSkin	= stlth_skin;
			acc.customShader = stlth_shader;
		}

			// Gordon: looted corpses dont have any accsserories, evil looters :E
			if( !((cent->currentState.eType == ET_CORPSE || cent->currentState.eType == ET_NPC_CORPSE) && cent->currentState.time2 == 1 )) {
				switch(i)
				{
					case ACC_BELT_LEFT:
						CG_PositionEntityOnTag( &acc,	&body,	"tag_bright", 0, NULL);
						break;
					case ACC_BELT_RIGHT:
						CG_PositionEntityOnTag( &acc,	&body,	"tag_bleft", 0, NULL);
						break;
					case ACC_BELT:
						CG_PositionEntityOnTag( &acc,	&body,	"tag_ubelt", 0, NULL);
						break;
					case ACC_BACK:
						CG_PositionEntityOnTag( &acc,	&body,	"tag_back", 0, NULL);
						break;
					case ACC_HAT:			//hat
					case ACC_RANK:
						if( cent->currentState.eFlags & EF_HEADSHOT ) {
							continue;
						}
					case ACC_MOUTH2:		// hat2
					case ACC_MOUTH3:		// hat3					
						if( i == ACC_RANK ) {
							if( ci->rank == 0 ) {
								continue;
							}
							acc.customShader = rankicons[ ci->rank ][ 1 ].shader;

							if (cent->currentState.eFlags & EF_CLOAKED && ((cent->currentState.eFlags & EF_PRONE ) || (cent->currentState.eFlags & EF_CROUCHING)))
							{
								acc.customSkin	= stlth_skin;
								acc.customShader = stlth_shader;
							}
						}
						CG_PositionEntityOnTag( &acc,	&head,	"tag_mouth", 0, NULL);
						break;
				
					// weapon and weapon2
					// these are used by characters who have permanent weapons attached to their character in the skin
					case ACC_WEAPON:	// weap
						CG_PositionEntityOnTag( &acc,	&body,	"tag_weapon", 0, NULL);
						break;
					case ACC_WEAPON2:	// weap2
						CG_PositionEntityOnTag( &acc,	&body,	"tag_weapon2", 0, NULL);
						break;
					default:
						continue;
				}
				CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
			}
	}

//#ifdef __ETE__
//	CG_CheckClientVisibility( cent );
//#endif //__ETE__
}

//=====================================================================

extern void CG_ClearWeapLerpFrame( clientInfo_t *ci, lerpFrame_t *lf, int animationNumber );

/*
===============
CG_ResetPlayerEntity

A player just came into view or teleported, so reset all animation info
===============
*/
void CG_ResetPlayerEntity( centity_t *cent ) {
	// Gordon: these are unused
//	cent->errorTime = -99999;		// guarantee no error decay added
//	cent->extrapolated = qfalse;	

	if (!(cent->currentState.eFlags & EF_DEAD)) {
		CG_ClearLerpFrameRate( cent, &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.legs, cent->currentState.legsAnim );
		CG_ClearLerpFrame( cent, &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.torso, cent->currentState.torsoAnim );

		memset( &cent->pe.legs, 0, sizeof( cent->pe.legs ) );
		cent->pe.legs.yawAngle = cent->rawAngles[YAW];
		cent->pe.legs.yawing = qfalse;
		cent->pe.legs.pitchAngle = 0;
		cent->pe.legs.pitching = qfalse;

		memset( &cent->pe.torso, 0, sizeof( cent->pe.legs ) );
		cent->pe.torso.yawAngle = cent->rawAngles[YAW];
		cent->pe.torso.yawing = qfalse;
		cent->pe.torso.pitchAngle = cent->rawAngles[PITCH];
		cent->pe.torso.pitching = qfalse;
	}

	BG_EvaluateTrajectory( &cent->currentState.pos, cg.time, cent->lerpOrigin, qfalse, cent->currentState.effect2Time );
	BG_EvaluateTrajectory( &cent->currentState.apos, cg.time, cent->lerpAngles, qtrue, cent->currentState.effect2Time  );

	VectorCopy( cent->lerpOrigin, cent->rawOrigin );
	VectorCopy( cent->lerpAngles, cent->rawAngles );

	if ( cg_debugPosition.integer ) {
		CG_Printf("%i ResetPlayerEntity yaw=%i\n", cent->currentState.number, cent->pe.torso.yawAngle );
	}

	cent->pe.painAnimLegs = -1;
	cent->pe.painAnimTorso = -1;
	cent->pe.animSpeed = 1.0;

}

void CG_GetBleedOrigin( vec3_t head_origin, vec3_t body_origin, int fleshEntityNum )
{
		clientInfo_t		*ci;
		refEntity_t			body;
		refEntity_t			head;
		centity_t			*cent, backupCent;
		bg_character_t		*character;

		ci = &cgs.clientinfo[ fleshEntityNum ];

		if ( !ci->infoValid ) {
			return;
		}

		character = CG_CharacterForClientinfo( ci, NULL );

		cent = &cg_entities [ fleshEntityNum ];
		backupCent = *cent;

		memset( &body, 0, sizeof(body) );
		memset( &head, 0, sizeof(head) );

		CG_PlayerAngles( cent, body.axis, body.torsoAxis, head.axis );
		CG_PlayerAnimation ( cent, &body );

		body.hModel = character->mesh;
		if( !body.hModel ) {
			return;
		}

		head.hModel = character->hudhead;
		if( !head.hModel ) {
			return;
		}

		VectorCopy( cent->lerpOrigin, body.origin );
		VectorCopy( body.origin, body.oldorigin );

		// Ridah, restore the cent so we don't interfere with animation timings
		*cent = backupCent;

		CG_PositionRotatedEntityOnTag( &head, &body, "tag_head");
	
		VectorCopy( head.origin, head_origin );
		VectorCopy( body.origin, body_origin );
}

/*
===============
CG_GetTag
===============
*/
qboolean CG_GetTag( int clientNum, char *tagname, orientation_t *or ) {
	clientInfo_t	*ci;
	centity_t		*cent;
	refEntity_t		*refent;
	vec3_t			tempAxis[3];
	vec3_t			org;
	int				i;

	ci = &cgs.clientinfo[ clientNum ];

	if( cg.snap && clientNum == cg.snap->ps.clientNum && cg.renderingThirdPerson ) {
		cent = &cg.predictedPlayerEntity;
	} else {
		cent = &cg_entities[ci->clientNum];
		if (!cent->currentValid)
			return qfalse;		// not currently in PVS
	}

	refent = &cent->pe.bodyRefEnt;

	if( trap_R_LerpTag( or, refent, tagname, 0 ) < 0 )
		return qfalse;

	VectorCopy( refent->origin, org );

	for( i = 0 ; i < 3 ; i++ ) {
		VectorMA( org, or->origin[i], refent->axis[i], org );
	}

	VectorCopy( org, or->origin );

	// rotate with entity
	MatrixMultiply( refent->axis, or->axis, tempAxis );
	memcpy( or->axis, tempAxis, sizeof(vec3_t) * 3 );

	return qtrue;
}

/*
===============
CG_GetWeaponTag
===============
*/
qboolean CG_GetWeaponTag( int clientNum, char *tagname, orientation_t *or ) {
	clientInfo_t	*ci;
	centity_t		*cent;
	refEntity_t		*refent;
	vec3_t			tempAxis[3];
	vec3_t			org;
	int				i;

	ci = &cgs.clientinfo[ clientNum ];

	if( cg.snap && clientNum == cg.snap->ps.clientNum && cg.renderingThirdPerson ) {
		cent = &cg.predictedPlayerEntity;
	} else {
		cent = &cg_entities[ci->clientNum];
		if (!cent->currentValid)
			return qfalse;		// not currently in PVS
	}

	if (cent->pe.gunRefEntFrame < cg.clientFrame - 1)
		return qfalse;

	refent = &cent->pe.gunRefEnt;

	if (trap_R_LerpTag( or, refent, tagname, 0 ) < 0)
		return qfalse;

	VectorCopy( refent->origin, org );

	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( org, or->origin[i], refent->axis[i], org );
	}

	VectorCopy( org, or->origin );

	// rotate with entity
	MatrixMultiply( refent->axis, or->axis, tempAxis );
	memcpy( or->axis, tempAxis, sizeof(vec3_t) * 3 );

	return qtrue;
}

// =============
// Menu Versions
// =============

/*
==================
CG_SwingAngles_Limbo
==================
*/
static void CG_SwingAngles_Limbo( float destination, float swingTolerance, float clampTolerance,
					float speed, float *angle, qboolean *swinging ) {
	float	swing;
	float	move;
	float	scale;

	if ( !*swinging ) {
		// see if a swing should be started
		swing = AngleSubtract( *angle, destination );
		if ( swing > swingTolerance || swing < -swingTolerance ) {
			*swinging = qtrue;
		}
	}

	if ( !*swinging ) {
		return;
	}
	
	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract( destination, *angle );
	scale = fabs( swing );
	if ( scale < swingTolerance * 0.5 ) {
		scale = 0.5;
	} else if ( scale < swingTolerance ) {
		scale = 1.0;
	} else {
		scale = 2.0;
	}

	// swing towards the destination angle
	if ( swing >= 0 ) {
		move = cg.frametime * scale * speed;
		if ( move >= swing ) {
			move = swing;
			*swinging = qfalse;
		}
		*angle = AngleMod( *angle + move );
	} else if ( swing < 0 ) {
		move = cg.frametime * scale * -speed;
		if ( move <= swing ) {
			move = swing;
			*swinging = qfalse;
		}
		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance ) {
		*angle = AngleMod( destination - (clampTolerance - 1) );
	} else if ( swing < -clampTolerance ) {
		*angle = AngleMod( destination + (clampTolerance - 1) );
	}
}

/*
===============
CG_PlayerAngles_Limbo
===============
*/
void CG_PlayerAngles_Limbo( playerInfo_t *pi, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] ) {
	vec3_t		legsAngles, torsoAngles, headAngles;
	float		dest;

	VectorCopy( pi->viewAngles, headAngles );
	headAngles[YAW] = AngleMod( headAngles[YAW] );
	VectorClear( legsAngles );
	VectorClear( torsoAngles );

	torsoAngles[YAW] = 180;
	legsAngles[YAW] = 180;
	headAngles[YAW] = 180;

	// --------- pitch -------------

	// only show a fraction of the pitch angle in the torso
	if ( headAngles[PITCH] > 180 ) {
		dest = (-360 + headAngles[PITCH]) * 0.75;
	} else {
		dest = headAngles[PITCH] * 0.75;
	}
	CG_SwingAngles_Limbo( dest, 15, 30, 0.1, &pi->torso.pitchAngle, &pi->torso.pitching );
	torsoAngles[PITCH] = pi->torso.pitchAngle;

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );

	AnglesSubtract( legsAngles, pi->moveAngles, legsAngles );		// NERVE - SMF

	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
}

animation_t *CG_GetLimboAnimation( playerInfo_t *pi, const char *name ) {
	int				i;
	bg_character_t	*character = BG_GetCharacter( pi->teamNum, pi->classNum );

	if( !character ) {
		return NULL;
	}

	for( i = 0; i < character->animModelInfo->numAnimations; i++ ) {
		if ( !Q_stricmp( character->animModelInfo->animations[i]->name, name ) ) {
			return character->animModelInfo->animations[i];
		}
	}

	return character->animModelInfo->animations[0];	// safe fallback so we never end up without an animation (which will crash the game)
}

int CG_GetSelectedWeapon( void ) {
	return 0;
}

void CG_DrawPlayer_Limbo( float x, float y, float w, float h, playerInfo_t *pi, int time, clientInfo_t *ci, qboolean animatedHead) {
	refdef_t		refdef;
	refEntity_t		body;
	refEntity_t		head;
	refEntity_t		gun;
	refEntity_t		barrel;
	refEntity_t		acc;
	vec3_t			origin;
	int				renderfx;
	vec3_t			mins = {-16, -16, -24};
	vec3_t			maxs = {16, 16, 32};
	float			len;
//	float			xx;
	vec4_t			hcolor = { 1, 0, 0, 0.5 };
	bg_character_t	*character = BG_GetCharacter( pi->teamNum, pi->classNum );
	int				i;

	dp_realtime = time;

	CG_AdjustFrom640( &x, &y, &w, &h );
	y -= jumpHeight;

	memset( &refdef, 0, sizeof( refdef ) );
	memset( &body, 0, sizeof(body) );
	memset( &head, 0, sizeof(head) );
	memset( &acc, 0, sizeof(acc) );

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

/*	refdef.fov_x = (int)((float)refdef.width / 640.0f * 90.0f);
	xx = refdef.width / tan( refdef.fov_x / 360 * M_PI );
	refdef.fov_y = atan2( refdef.height, xx );
	refdef.fov_y *= ( 360 / M_PI );*/

	refdef.fov_x = 35;
	refdef.fov_y = 35;

	// calculate distance so the player nearly fills the box

	// START Mad Doc - TDF
	// for "talking heads", we calc the origin differently
	// FIXME - this is stupid - should all be character driven - NO CODE HACKS FOR SPECIFIC THINGS THAT CAN BE DONE CLEANLY
	if (animatedHead == qfalse)
	{
	// END Mad Doc - TDF	
		len = 0.9 * ( maxs[2] - mins[2] );							// NERVE - SMF - changed from 0.7
		origin[0] = pi->y -70 + (len / tan( DEG2RAD(refdef.fov_x) * 0.5 ));
		origin[1] = 0.5 * ( mins[1] + maxs[1] );
		origin[2] = pi->z -23 + (-0.5 * ( mins[2] + maxs[2] ));
	}
	else
	{
		// for "talking head" animations, we want to center just below the face
		// we precalculated this elsewhere
		VectorCopy(pi->headOrigin, origin);
	}
	// END Mad Doc - TDF

	refdef.time = dp_realtime;

	trap_R_SetColor( hcolor );
	trap_R_ClearScene();
	trap_R_SetColor( NULL );

	// get the rotation information
	CG_PlayerAngles_Limbo( pi, body.axis, body.torsoAxis, head.axis );
	
	renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;

	acc.renderfx = renderfx;
 

	//
	// add the body
	//

	body.hModel = character->mesh;
	body.customSkin = character->skin;

	body.renderfx = renderfx;

	VectorCopy( origin, body.origin );
	VectorCopy( origin, body.lightingOrigin );
	VectorCopy( body.origin, body.oldorigin );

	if( cg.time >= pi->torso.frameTime ) {
		pi->torso.oldFrameTime = pi->torso.frameTime;
		pi->torso.oldFrame = pi->torso.frame;
		pi->torso.oldFrameModel = pi->torso.frameModel = pi->torso.animation->mdxFile;

		while( cg.time >= pi->torso.frameTime ) {
			pi->torso.frameTime += (pi->torso.animation->duration / (float)(pi->torso.animation->numFrames));
			pi->torso.frame++;

			if( pi->torso.frame >= pi->torso.animation->firstFrame + pi->torso.animation->numFrames ) {
				pi->torso.frame = pi->torso.animation->firstFrame;
			}
		}
	}

	if( pi->torso.frameTime == pi->torso.oldFrameTime ) {
		pi->torso.backlerp = 0;
	} else {
		pi->torso.backlerp = 1.0 - (float)( cg.time - pi->torso.oldFrameTime ) / ( pi->torso.frameTime - pi->torso.oldFrameTime );
	}

	if( cg.time >= pi->legs.frameTime ) {
		pi->legs.oldFrameTime = pi->legs.frameTime;
		pi->legs.oldFrame = pi->legs.frame;
		pi->legs.oldFrameModel = pi->legs.frameModel = pi->legs.animation->mdxFile;

		while( cg.time >= pi->legs.frameTime ) {
			pi->legs.frameTime += (pi->legs.animation->duration / (float)(pi->legs.animation->numFrames));
			pi->legs.frame++;

			if( pi->legs.frame >= pi->legs.animation->firstFrame + pi->legs.animation->numFrames ) {
				pi->legs.frame = pi->legs.animation->firstFrame;
			}
		}
	}

	if( pi->legs.frameTime == pi->legs.oldFrameTime ) {
		pi->legs.backlerp = 0;
	} else {
		pi->legs.backlerp = 1.0 - (float)( cg.time - pi->legs.oldFrameTime ) / ( pi->legs.frameTime - pi->legs.oldFrameTime );
	}

	body.oldTorsoFrame = pi->torso.oldFrame;
	body.torsoFrame = pi->torso.frame;
	body.torsoBacklerp = pi->torso.backlerp;
	body.torsoFrameModel = pi->torso.frameModel;
	body.oldTorsoFrameModel = pi->torso.oldFrameModel;

	body.oldframe = pi->legs.oldFrame;
	body.frame = pi->legs.frame;
	body.backlerp = pi->legs.backlerp;
	body.frameModel = pi->legs.frameModel;
	body.oldframeModel = pi->legs.oldFrameModel;

	trap_R_AddRefEntityToScene( &body );

	//
	// add the head
	//

	head.hModel = character->hudhead;
	if( !head.hModel ) {
		return;
	}
	head.customSkin = character->headSkin;

	VectorCopy( origin, head.lightingOrigin );

	CG_PositionRotatedEntityOnTag( &head, &body, "tag_head" );

	head.renderfx = renderfx;

	head.frame = 0;
	head.oldframe = 0;
	head.backlerp = 0.f;

	trap_R_AddRefEntityToScene( &head );

	AxisCopy( body.torsoAxis, acc.axis );
	VectorCopy( origin, acc.lightingOrigin );


	for( i = ACC_BELT_LEFT; i < ACC_MAX; i++ ) {
		if( !(character->accModels[i]) )
			continue;
		acc.hModel = character->accModels[i];
		acc.customSkin = character->accSkins[i];

		switch(i) {
			case ACC_BELT_LEFT:
				CG_PositionEntityOnTag( &acc,	&body,	"tag_bright", 0, NULL);
				break;
			case ACC_BELT_RIGHT:
				CG_PositionEntityOnTag( &acc,	&body,	"tag_bleft", 0, NULL);
				break;

			case ACC_BELT:
				CG_PositionEntityOnTag( &acc,	&body,	"tag_ubelt", 0, NULL);
				break;
			case ACC_BACK:
				CG_PositionEntityOnTag( &acc,	&body,	"tag_back", 0, NULL);
				break;
			
			case ACC_HAT:			// hat
			case ACC_MOUTH2:		// hat2
			case ACC_MOUTH3:		// hat3
				CG_PositionEntityOnTag( &acc,	&head,	"tag_mouth", 0, NULL);
				break;
			
			// weapon and weapon2
			// these are used by characters who have permanent weapons attached to their character in the skin
			case ACC_WEAPON:	// weap
				CG_PositionEntityOnTag( &acc,	&body,	"tag_weapon", 0, NULL);
				break;
			case ACC_WEAPON2:	// weap2
				CG_PositionEntityOnTag( &acc,	&body,	"tag_weapon2", 0, NULL);
				break;

			default:
				continue;
		}

		trap_R_AddRefEntityToScene( &acc );
	}

	//
	// add the gun
	//
	{
		int weap = CG_GetSelectedWeapon();

		memset( &gun, 0, sizeof(gun) );
		memset( &barrel, 0, sizeof(barrel) );

		gun.hModel = cg_weapons[weap].weaponModel[W_TP_MODEL].model;

		VectorCopy( origin, gun.lightingOrigin );
		CG_PositionEntityOnTag( &gun, &body, "tag_weapon", 0, NULL );
		gun.renderfx = renderfx;
		trap_R_AddRefEntityToScene( &gun );
	}

	// Save out the old render info so we don't kill the LOD system here
	trap_R_SaveViewParms();

	//
	// add an accent light
	//
	origin[0] -= 100;	// + = behind, - = in front
	origin[1] += 100;	// + = left, - = right
	origin[2] += 100;	// + = above, - = below
	//%	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 0 );
	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 1.0, 0, 0 );

	origin[0] -= 100;
	origin[1] -= 100;
	origin[2] -= 100;
	//%	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 0 );
	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 1.0, 0, 0 );

	trap_R_RenderScene( &refdef );

	// Reset the view parameters
	trap_R_RestoreViewParms();
}

weaponType_t weaponTypes[] = {
	{ WP_MP40,										"MP 40"			},
	{ WP_THOMPSON,							"THOMPSON"		},
	{ WP_STEN,										"STEN",			},
	{ WP_PANZERFAUST,						"PANZERFAUST",	},
	{ WP_FLAMETHROWER,					"FLAMETHROWER",	},
	{ WP_KAR98,									"K43", 			},
	{ WP_CARBINE,								"M1 GARAND", 	},
	{ WP_FG42,										"FG42",			},
	{ WP_GARAND,								"M1 GARAND",	},
	{ WP_MOBILE_MG42,						"MOBILE MG42",	},
	{ WP_K43,										"K43",			},
	{ WP_MORTAR,								"MORTAR",		},
	{ WP_COLT,										"COLT",			},
	{ WP_LUGER,									"LUGER",		},
	{ WP_AKIMBO_COLT,						"AKIMBO COLTS",	},
	{ WP_AKIMBO_LUGER,					"AKIMBO LUGERS",},
	{ WP_SILENCED_COLT,					"COLT",			},
	{ WP_SILENCED_LUGER,								"LUGER",		},
	{ WP_AKIMBO_SILENCEDCOLT,		"AKIMBO COLTS",	},
	{ WP_AKIMBO_SILENCEDLUGER,	"AKIMBO LUGERS",},
#ifdef __EF__
	{ WP_PPSH,										"PPSH",		},
	{ WP_STG44,									"STG-44",		},
	{ WP_PPS,										"PPS",			},
	{ WP_SVT40,									"SVT-40",		},
	{ WP_30CAL,									"30CAL",		},
	{ WP_30CAL_SET,							"30CAL",		},
	{ WP_PTRS,                        "PTRS 14,5mm",     },
    { WP_PTRS_SET,                        "PTRS 14,5mm",     },
#endif //__EF__
	{ WP_NONE,									NULL,			},
	{ -1,													NULL,			},
};

weaponType_t* WM_FindWeaponTypeForWeapon ( weapon_t weapon ) {
	weaponType_t* w = weaponTypes;

	if(!weapon) {
		return NULL;
	}

	while(w->weapindex != -1) {
		if(w->weapindex == weapon) {
			return w;
		}
		w++;
	}
	return NULL;
}

void WM_RegisterWeaponTypeShaders () {
	weaponType_t* w = weaponTypes;

	while(w->weapindex) {	
//		w->shaderHandle = trap_R_RegisterShaderNoMip( w->shader );
		w++;
	}
}

void CG_MenuSetAnimation( playerInfo_t *pi, const char* legsAnim, const char* torsoAnim, qboolean force, qboolean clearpending ) {
	lastLegsAnim = pi->legs.animation = CG_GetLimboAnimation(pi, legsAnim);
	lastTorsoAnim = pi->torso.animation = CG_GetLimboAnimation(pi, torsoAnim);

	if( force ) {
		pi->legs.oldFrame = pi->legs.frame = pi->legs.animation->firstFrame;
		pi->torso.oldFrame = pi->torso.frame = pi->torso.animation->firstFrame;
		
		pi->legs.frameTime = cg.time;
		pi->torso.frameTime = cg.time;

		pi->legs.oldFrameModel = pi->legs.frameModel = pi->legs.animation->mdxFile;
		pi->torso.oldFrameModel = pi->torso.frameModel = pi->torso.animation->mdxFile;

		pi->numPendingAnimations = 0;
	} else {
		pi->legs.oldFrame = pi->legs.frame;
		pi->legs.oldFrameModel = pi->legs.frameModel;
		pi->legs.frame = pi->legs.animation->firstFrame;
		pi->torso.oldFrame = pi->torso.frame;
		pi->torso.oldFrameModel = pi->torso.frameModel;
		pi->torso.frame = pi->torso.animation->firstFrame;

		pi->legs.frameTime += 200; // Give them some time to lerp between animations
		pi->torso.frameTime += 200;
	}

	if( clearpending ) {
		pi->numPendingAnimations = 0;
	}
}

void CG_MenuPendingAnimation( playerInfo_t *pi, const char* legsAnim, const char* torsoAnim, int delay ) {
	if( pi->numPendingAnimations >= 4 ) {
		return;
	}

	if( !pi->numPendingAnimations ) {
		pi->pendingAnimations[pi->numPendingAnimations].pendingAnimationTime =	cg.time + delay;
	} else {
		pi->pendingAnimations[pi->numPendingAnimations].pendingAnimationTime =	pi->pendingAnimations[pi->numPendingAnimations-1].pendingAnimationTime + delay;
	}
	pi->pendingAnimations[pi->numPendingAnimations].pendingLegsAnim =	legsAnim;
	pi->pendingAnimations[pi->numPendingAnimations].pendingTorsoAnim =	torsoAnim;

	lastLegsAnim = CG_GetLimboAnimation(pi, legsAnim);
	lastTorsoAnim = CG_GetLimboAnimation(pi, torsoAnim);
	pi->numPendingAnimations++;
}

void CG_MenuCheckPendingAnimation( playerInfo_t *pi ) {
	int i;

	if(pi->numPendingAnimations <= 0) {
		return;
	}

	if(pi->pendingAnimations[0].pendingAnimationTime && pi->pendingAnimations[0].pendingAnimationTime < cg.time) {
		CG_MenuSetAnimation(pi, pi->pendingAnimations[0].pendingLegsAnim, pi->pendingAnimations[0].pendingTorsoAnim, qfalse, qfalse);

		for(i = 0; i < 3; i++ ) {
			memcpy( &pi->pendingAnimations[i], &pi->pendingAnimations[i+1], sizeof(pendingAnimation_t));
		}

		pi->numPendingAnimations--;
	}
}


void CG_SetHudHeadLerpFrameAnimation( bg_character_t* ch, lerpFrame_t *lf, int newAnimation ) {
	animation_t	*anim;

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;

	if ( newAnimation < 0 || newAnimation >= MAX_HD_ANIMATIONS ) {
		CG_Error( "Bad animation number (CG_SetHudHeadLerpFrameAnimation): %i", newAnimation );
	}

	anim = &ch->hudheadanimations[ newAnimation ];

	lf->animation		= anim;
	lf->animationTime	= lf->frameTime + anim->initialLerp;
}

void CG_ClearHudHeadLerpFrame( bg_character_t* ch, lerpFrame_t *lf, int animationNumber )
{
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetHudHeadLerpFrameAnimation( ch, lf, animationNumber );
	lf->oldFrame = lf->frame = lf->animation->firstFrame;
	lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
}

void CG_RunHudHeadLerpFrame( bg_character_t* ch, lerpFrame_t *lf, int newAnimation, float speedScale ) {
	int			f;
	animation_t	*anim;



	// see if the animation sequence is switching
	if( !lf->animation ) {
		CG_ClearHudHeadLerpFrame( ch, lf, newAnimation );
	} else if( newAnimation != lf->animationNumber ) {
		CG_SetHudHeadLerpFrameAnimation( ch, lf, newAnimation );
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if( cg.time >= lf->frameTime ) {
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		// get the next frame based on the animation
		anim = lf->animation;
		if ( !anim->frameLerp ) {
			return;		// shouldn't happen
		}
		if ( cg.time < lf->animationTime ) {
			lf->frameTime = lf->animationTime;		// initial lerp
		} else {
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;
		}
		f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		f *= speedScale;		// adjust for haste, etc
		if ( f >= anim->numFrames ) {
			f -= anim->numFrames;
			if ( anim->loopFrames ) {
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			} else {
				f = anim->numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}
		lf->frame = anim->firstFrame + f;
		lf->frameModel = anim->mdxFile;
		if ( cg.time > lf->frameTime ) {
			lf->frameTime = cg.time;
		}
	}

	if ( lf->frameTime > cg.time + 200 ) {
		lf->frameTime = cg.time;
	}

	if ( lf->oldFrameTime > cg.time ) {
		lf->oldFrameTime = cg.time;
	}
	// calculate current lerp value
	if ( lf->frameTime == lf->oldFrameTime ) {
		lf->backlerp = 0;
	} else {
		lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}

void CG_HudHeadAnimation( bg_character_t* ch, lerpFrame_t* lf, int *oldframe, int *frame, float *backlerp, hudHeadAnimNumber_t animation ) {
//	centity_t *cent = &cg.predictedPlayerEntity;

	CG_RunHudHeadLerpFrame( ch, lf, (int)animation, 1.f );

	*oldframe		= lf->oldFrame;
	*frame			= lf->frame;
	*backlerp		= lf->backlerp;
}
