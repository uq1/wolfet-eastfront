/*
                                                                                                            
  **    ***    ***            ***   ****                                   *               ***              
  **    ***    ***            ***  *****                                 ***               ***              
  ***  *****  ***             ***  ***                                   ***                                
  ***  *****  ***    *****    *** ******   *****    *** ****    ******  ******    *****    ***  *** ****    
  ***  *****  ***   *******   *** ******  *******   *********  ***  *** ******   *******   ***  *********   
  ***  ** **  ***  **** ****  ***  ***   ***   ***  ***   ***  ***       ***    ***   ***  ***  ***   ***   
  *** *** *** ***  ***   ***  ***  ***   *********  ***   ***  ******    ***    *********  ***  ***   ***   
   ****** ******   ***   ***  ***  ***   *********  ***   ***   ******   ***    *********  ***  ***   ***   
   *****   *****   ***   ***  ***  ***   ***        ***   ***    ******  ***    ***        ***  ***   ***   
   *****   *****   **** ****  ***  ***   ****  ***  ***   ***       ***  ***    ****  ***  ***  ***   ***   
   ****     ****    *******   ***  ***    *******   ***   ***  ***  ***  *****   *******   ***  ***   ***   
    ***     ***      *****    ***  ***     *****    ***   ***   ******    ****    *****    ***  ***   ***   
                                                                                                            
            ******** **                 ******                        *  **  **                             
            ******** **                 ******                       **  **  **                             
               **    **                 **                           **  **                                 
               **    ** **    ***       **      ** *  ****   ** **  **** **  **  ** **    ***               
               **    ******  *****      *****   ****  ****   ****** **** **  **  ******  *****              
               **    **  **  ** **      *****   **   **  **  **  **  **  **  **  **  **  ** **              
               **    **  **  *****      **      **   **  **  **  **  **  **  **  **  **  *****              
               **    **  **  **         **      **   **  **  **  **  **  **  **  **  **  **                 
               **    **  **  ** **      **      **   **  **  **  **  **  **  **  **  **  ** **              
               **    **  **  *****      **      **    ****   **  **  *** **  **  **  **  *****              
               **    **  **   ***       **      **    ****   **  **  *** **  **  **  **   ***               

*/


#include			"../game/q_shared.h"
#include			"../game/g_local.h"
#ifdef __BOT__
#include			"unique_includes.h"

extern void AIMOD_MOVEMENT_CheckAvoidance ( gentity_t *bot, usercmd_t *ucmd );

// In unique_coverspots.c
extern void AIMOD_SP_MOVEMENT_CoverSpot_Attack ( gentity_t *bot, usercmd_t *ucmd, qboolean useProne );
extern void AIMOD_SP_MOVEMENT_CoverSpot_Do_Actual_Attack ( gentity_t *bot, usercmd_t *ucmd );

//===========================================================================
// Routine      : AIMOD_Combat_Weapons_Check

// Description  : Check bot's weapon during attacks...
void
AIMOD_Combat_Weapons_Check ( gentity_t *bot, usercmd_t *ucmd )
{
	if (!bot->enemy)
		return;

#ifndef __BOT_NO_AMMO_CHEAT__
	if (COM_BitCheck( bot->client->ps.weapons, WP_GPG40))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_GPG40)] = 1;

	if (COM_BitCheck( bot->client->ps.weapons, WP_CARBINE))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_CARBINE)] = 1;
#endif //__BOT_NO_AMMO_CHEAT__

	if
	(
		bot->client->ps.weapon == WP_FG42 &&
		(VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024 || (bot->bot_coverspot_time > level.time && VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) < 8))
	)
	{
		if ( COM_BitCheck( bot->client->ps.weapons, WP_FG42SCOPE) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_FG42SCOPE) )
		{
			if (bot->client->ps.weapon != WP_FG42SCOPE)
			{
				BOT_ChangeWeapon(bot, WP_FG42SCOPE);
				return;
			}
		}
	}
	else if
		(
			bot->client->ps.weapon == WP_K43 &&
			(VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024 || (bot->bot_coverspot_time > level.time && VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) < 8))
		)
	{
		if ( COM_BitCheck( bot->client->ps.weapons, WP_K43_SCOPE) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_K43_SCOPE) )
		{
			if (bot->client->ps.weapon != WP_K43_SCOPE)
			{
				BOT_ChangeWeapon(bot, WP_K43_SCOPE);
				return;
			}
		}
	}
	else if
		(
			bot->client->ps.weapon == WP_GARAND &&
			(VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024 || (bot->bot_coverspot_time > level.time && VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) < 8))
		)
	{
		if ( COM_BitCheck( bot->client->ps.weapons, WP_GARAND_SCOPE) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_GARAND_SCOPE) )
		{
			if (bot->client->ps.weapon != WP_GARAND_SCOPE)
			{
				BOT_ChangeWeapon(bot, WP_GARAND_SCOPE);
				return;
			}
		}
	}

	if
	(
		COM_BitCheck( bot->client->ps.weapons, WP_SMOKE_MARKER) &&
		bot->enemy &&
		bot->client->sess.playerType == PC_FIELDOPS &&
		VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) < 512
	)
	{	// Switch to a grenade if close to an enemy...
		if (bot->client->ps.weapon != WP_SMOKE_MARKER)
		{
			BOT_ChangeWeapon(bot, WP_SMOKE_MARKER);
			return;
		}
	}
}


//===========================================================================
// Routine      : AIMOD_AttackMovement_Fallback

// Description  : Fallback movement while attacking...
qboolean
AIMOD_AttackMovement_Fallback ( gentity_t *bot, usercmd_t *ucmd )
{															// Fallback navigation. returns if it was used or not as qtrue/qfalse.
	//G_Printf("**1** BOT should be firing.\n");

#ifndef __BOT_NO_AMMO_CHEAT__
	if (COM_BitCheck( bot->client->ps.weapons, WP_GPG40))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_GPG40)] = 1;

	if (COM_BitCheck( bot->client->ps.weapons, WP_CARBINE))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_CARBINE)] = 1;
#endif //__BOT_NO_AMMO_CHEAT__

	if (bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK)
	{
		ucmd->buttons = BUTTON_ATTACK;
		return ( qtrue );
	}

	if ( NodesLoaded() == qfalse /*|| bot_forced_auto_waypoint.integer*/ )
	{														// Random for unrouted maps.
		vec3_t	dir;

		Set_Best_AI_weapon( bot );
		//AIMOD_Combat_Weapons_Check( bot, ucmd );

		if ( VectorLength( bot->client->ps.velocity) < 64 )
		{													// We havn't moved at all... Let's randomize things.
			vec3_t	temp;
			if ( bot->last_use_time < level.time && no_mount_time[bot->s.clientNum] < level.time )
			{
				Cmd_Activate_f( bot );
			}

			VectorCopy( bot->move_vector, temp );
			temp[0] = bot->move_vector[0];
			temp[2] = bot->move_vector[2];
			temp[1] = bot->move_vector[1] - rand() % 360;
			bot->client->AImovetime = level.time + 10000;	// 10 seconds?
			VectorCopy( temp, bot->move_vector );
			VectorSubtract( temp, bot->r.currentOrigin, dir );
			if ( bot /*VectorNormalize( dir ) > 32*/ )
			{
				if ( bot->bot_turn_wait_time <= level.time )
				{
					G_UcmdMoveForDir( bot, ucmd, dir );
					AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
				}
			}
			else
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
			}
		}
		else
		{
			VectorSubtract( bot->move_vector, bot->r.currentOrigin, dir );
			if ( bot /*VectorNormalize( dir ) > 32*/ )
			{
				if ( bot->bot_turn_wait_time <= level.time )
				{
					G_UcmdMoveForDir( bot, ucmd, dir );
					AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
				}
			}
			else
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
			}
		}

		if ( bot->waterlevel )
		{
			ucmd->upmove = 120;								// Try to always be on surface...
		}

		if ( AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ) && MyInFOV(bot->enemy, bot, 12, 25) )
			AIMOD_SP_MOVEMENT_CoverSpot_Do_Actual_Attack( bot, ucmd );

		return ( qtrue );
	}

	return ( qfalse );
}

#ifndef __BOT_PERFECTION__
//===========================================================================
// Routine      : AIMOD_AttackMovement_From_Void

// Description  : Attack movement while in a void...
void
AIMOD_AttackMovement_From_Void ( gentity_t *bot, usercmd_t *ucmd )
{														// Forced fallback navigation (out of voids)...
	//G_Printf("**2** BOT should be firing.\n");

#ifndef __BOT_NO_AMMO_CHEAT__
	if (COM_BitCheck( bot->client->ps.weapons, WP_GPG40))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_GPG40)] = 1;

	if (COM_BitCheck( bot->client->ps.weapons, WP_CARBINE))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_CARBINE)] = 1;
#endif //__BOT_NO_AMMO_CHEAT__

	if (bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK)
	{
		ucmd->buttons = BUTTON_ATTACK;
		return;
	}

	if ( VectorLength( bot->client->ps.velocity) < 64 )
	{													// We havn't moved at all... Let's randomize things.
		vec3_t	temp;
		vec3_t	dir;

		Set_Best_AI_weapon( bot );
		//AIMOD_Combat_Weapons_Check( bot, ucmd );

		if ( bot->last_use_time < level.time && no_mount_time[bot->s.clientNum] < level.time )
		{
			Cmd_Activate_f( bot );
		}

		VectorCopy( bot->move_vector, temp );
		temp[0] = bot->move_vector[0];
		temp[2] = bot->move_vector[2];
		temp[1] = bot->move_vector[1] - rand() % 360;
		bot->client->AImovetime = level.time + 10000;	// 10 seconds?
		VectorCopy( temp, bot->move_vector );
		VectorSubtract( temp, bot->r.currentOrigin, dir );
		if ( bot->bot_turn_wait_time <= level.time )
		{
			G_UcmdMoveForDir( bot, ucmd, dir );
			AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
		}
		else
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
		}
	}
	else
	{
		vec3_t	dir;

		VectorSubtract( bot->move_vector, bot->r.currentOrigin, dir );
		if ( bot->bot_turn_wait_time <= level.time )
		{
			G_UcmdMoveForDir( bot, ucmd, dir );
			AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
		}
		else
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
		}
	}

	if ( bot->waterlevel )
	{
		ucmd->upmove = 120;								// Try to always be on surface...
	}
	if ( AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ) && MyInFOV(bot->enemy, bot, 12, 25) )
		AIMOD_SP_MOVEMENT_CoverSpot_Do_Actual_Attack( bot, ucmd );
}
#endif //__BOT_PERFECTION__

extern void AIMOD_SP_MOVEMENT_CoverSpot_Movement ( gentity_t *bot, usercmd_t *ucmd );
//===========================================================================
// Routine      : AIMOD_MOVEMENT_Attack
// Description  : Attack movement
//              : Just basic move and avoidance

// TODO         : Change routine for more advanced attack movement
void
AIMOD_MOVEMENT_Attack ( gentity_t *bot, usercmd_t *ucmd )
{
	//AIMOD_SP_MOVEMENT_CoverSpot_Attack( bot, ucmd );
	AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
}

//===========================================================================
// Routine      : AIMOD_MOVEMENT_AttackMove

// Description  : Movement while attacking...
void
AIMOD_MOVEMENT_AttackMove ( gentity_t *bot, usercmd_t *ucmd )
{
#ifndef __BOT_NO_AMMO_CHEAT__
	if (COM_BitCheck( bot->client->ps.weapons, WP_GPG40))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_GPG40)] = 1;

	if (COM_BitCheck( bot->client->ps.weapons, WP_CARBINE))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_CARBINE)] = 1;
#endif //__BOT_NO_AMMO_CHEAT__

	//qboolean	onLadder = qfalse;

	// UQ1: Run attack code first, and override with move code :) Should be much neater ehehe
	//AIMOD_MOVEMENT_Attack(bot, ucmd);
	AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qfalse);

	if (bot->client->ps.weapon == WP_KNIFE)
		return;

	//if ( (bot->client->ps.eFlags & EF_PRONE) || (bot->client->ps.pm_flags & PMF_DUCKED) )
	//	return;

	//
	// End of Attack Code!!!
	//
	//
	// Now the movement code!
	//
	if ( bot->client->ps.eFlags & EF_ZOOMING )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if
	(
		bot->client->ps.weapon == WP_FG42SCOPE ||
		bot->client->ps.weapon == WP_K43_SCOPE ||
		bot->client->ps.weapon == WP_GARAND_SCOPE
	)
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if ( bot->client->ps.weapon == WP_MOBILE_MG42_SET )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

#ifdef __EF__
	if ( bot->client->ps.weapon == WP_30CAL_SET )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}
#endif //__EF__

#ifdef __WEAPON_AIM__
	if ( bot->client->ps.eFlags & EF_AIMING )
	{
		ucmd->buttons |= BUTTON_WALKING;
	}
	else
	{
		ucmd->buttons &= ~BUTTON_WALKING;
	}
#endif //__WEAPON_AIM__

	// Set up ideal angles for this node...
	ucmd->buttons &= ~BUTTON_WALKING; // We want to RUN to cover spots! :)
	
	VectorSubtract( bot->bot_coverspot, bot->r.currentOrigin, bot->move_vector );
	bot->move_vector[ROLL] = 0;
	
	G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
	AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );

	// Do we need to jump?
	if (VectorLength(bot->client->ps.velocity) < 16)
		ucmd->upmove = 127;

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;
}

void AIMOD_Combat_Weapons_Check ( gentity_t *bot, usercmd_t *ucmd );

//===========================================================================
// Routine      : AIMOD_MOVEMENT_CoverSpot_AttackMove

// Description  : Movement while attacking...
void
AIMOD_MOVEMENT_CoverSpot_AttackMove ( gentity_t *bot, usercmd_t *ucmd )
{
#ifndef __BOT_NO_AMMO_CHEAT__
	if (COM_BitCheck( bot->client->ps.weapons, WP_GPG40))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_GPG40)] = 1;

	if (COM_BitCheck( bot->client->ps.weapons, WP_CARBINE))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_CARBINE)] = 1;
#endif //__BOT_NO_AMMO_CHEAT__

	// UQ1: Run attack code first, and override with move code :) Should be much neater ehehe
	//AIMOD_MOVEMENT_Attack(bot, ucmd);
	AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qfalse);
	
	if (bot->client->ps.weapon == WP_KNIFE)
		return;

	if ( (bot->client->ps.eFlags & EF_PRONE) || (bot->client->ps.pm_flags & PMF_DUCKED) )
		return;

	//
	// End of Attack Code!!!
	//
	//
	// Now the movement code!
	//
	if ( bot->client->ps.eFlags & EF_ZOOMING )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if
	(
		bot->client->ps.weapon == WP_FG42SCOPE ||
		bot->client->ps.weapon == WP_K43_SCOPE ||
		bot->client->ps.weapon == WP_GARAND_SCOPE
	)
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if ( bot->client->ps.weapon == WP_MOBILE_MG42_SET )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

#ifdef __EF__
	if ( bot->client->ps.weapon == WP_30CAL_SET )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}
#endif //__EF__

#ifdef __WEAPON_AIM__
	if ( bot->client->ps.eFlags & EF_AIMING )
	{
		ucmd->buttons |= BUTTON_WALKING;
	}
	else
	{
		ucmd->buttons &= ~BUTTON_WALKING;
	}
#endif //__WEAPON_AIM__

	// Set up ideal angles for this node...
	ucmd->buttons &= ~BUTTON_WALKING; // We want to RUN to cover spots! :)
	
	VectorSubtract( bot->bot_coverspot, bot->r.currentOrigin, bot->move_vector );
	bot->move_vector[ROLL] = 0;
	
	G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
	AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );

	// Do we need to jump?
	if (VectorLength(bot->client->ps.velocity) < 16)
		ucmd->upmove = 127;

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;
}

#endif //__BOT__
