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

extern qboolean MyVisible_No_Supression ( gentity_t *self, gentity_t *other );
extern void AIMOD_MOVEMENT_CheckAvoidance ( gentity_t *bot, usercmd_t *ucmd );

// In unique_coverspots.c
extern void AIMOD_SP_MOVEMENT_CoverSpot_Attack ( gentity_t *bot, usercmd_t *ucmd, qboolean useProne );


//special node visibility check for bot linkages..
//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.

//3 = team door entity in the way.
int
SP_NodeVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg;
	vec3_t	mins, maxs;

	VectorCopy( botTraceMins, mins );
	VectorCopy( botTraceMaxs, maxs );
	VectorCopy( org2, newOrg );

	newOrg[2] += 4; // Look from up a little...

	trap_Trace( &tr, org1, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*MASK_WATER*/ );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	if
	(
		tr.fraction != 1 &&
		tr.entityNum != -1 &&
		tr.entityNum != ENTITYNUM_NONE &&
		tr.entityNum < ENTITYNUM_MAX_NORMAL &&
		(
			!strcmp( g_entities[tr.entityNum].classname, "func_door") || !strcmp( g_entities[tr.entityNum].classname,
																				  "func_door_rotating")
		)
	)
	{// A door in the way.. See if we have access...
		if ( !g_entities[tr.entityNum].allowteams )
		{
			return ( 2 );
		}

		return ( 3 );
	}

	return ( 0 );
}

//===========================================================================
// Routine      : AIMOD_SP_MOVEMENT_Pick_New_Destination

// Description  : Single Player AI Destination Selection
qboolean
AIMOD_SP_MOVEMENT_Pick_New_Destination ( gentity_t *bot, usercmd_t *ucmd )
{
	qboolean	good = qfalse;
	int			tries = 0;

	while (!good)
	{
		vec3_t up_pos, down_pos, good_pos;
		trace_t tr;
		int max_tries = (12*(bot_cpu_use.integer/100));

		if (max_tries < 1)
			max_tries = 1;

		if (tries > max_tries)
		{// Try again next frame!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->bot_sniping_time = level.time + 1000;
			return qfalse; 
		}

		tries++;

		VectorCopy(bot->bot_sp_spawnpos, bot->bot_sp_destination);
		if (g_gametype.integer != GT_SUPREMACY)
		{
			bot->bot_sp_destination[0] += (512-(64*Q_TrueRand(0, 16)));
			bot->bot_sp_destination[1] += (512-(64*Q_TrueRand(0, 16)));
		}
		else
		{// Supremacy flag capture, stay near the flag! (in capture range!)
			bot->bot_sp_destination[0] += (256-(64*Q_TrueRand(0, 8)));
			bot->bot_sp_destination[1] += (256-(64*Q_TrueRand(0, 8)));
		}

		VectorCopy(bot->bot_sp_destination, up_pos);
		up_pos[2]+=256;
		VectorCopy(bot->bot_sp_destination, down_pos);
		down_pos[2]-=256;

		trap_Trace( &tr, up_pos, NULL, NULL, down_pos, -1, MASK_SHOT | MASK_WATER );
			
		if ( tr.startsolid || 
			tr.allsolid  ||
			tr.contents & CONTENTS_STRUCTURAL ||
			tr.contents & CONTENTS_DETAIL ||
			tr.contents & CONTENTS_NODROP ||
			tr.contents & CONTENTS_STRUCTURAL ||
			tr.contents & CONTENTS_DONOTENTER_LARGE ||
			tr.contents & CONTENTS_DONOTENTER ||
			tr.contents & CONTENTS_MONSTERCLIP ||
			tr.contents & CONTENTS_PLAYERCLIP ||
			tr.contents & CONTENTS_MOVER ||
			tr.contents & CONTENTS_MISSILECLIP ||
			tr.contents & CONTENTS_SLIME ||
			tr.contents & CONTENTS_LAVA )
		{// Not a good end spot!
			continue;
		}

		VectorCopy(bot->bot_sp_destination, bot->bot_sp_destination);
		bot->bot_sp_destination[2]=tr.endpos[2]+1;

		VectorCopy(bot->bot_sp_destination, good_pos);
		good_pos[2]=tr.endpos[2]+8/*16*/;
			
		if ( HeightDistance(bot->r.currentOrigin, good_pos) < 64 
			&& SP_NodeVisible(bot->r.currentOrigin, good_pos, bot->s.number) )
		{
			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			good = qtrue;
			//G_Printf("%s - Position good!\n", bot->client->pers.netname);

			bot->bot_sniping_time = 0;
			bot->bot_stand_guard = 0;
			return qtrue;
		}
	}

	return qfalse;
}

//===========================================================================
// Routine      : AIMOD_SP_MOVEMENT_Move

// Description  : Single Player AI Movement (following per-bot node paths)
void
AIMOD_SP_MOVEMENT_Move ( gentity_t *bot, usercmd_t *ucmd )
{
	if ( !bot->bot_initialized )
	{// Initialization...
		VectorCopy(bot->bot_sp_spawnpos, bot->r.currentOrigin);
		G_SetOrigin(bot, bot->r.currentOrigin);
		bot->bot_initialized = qtrue;

		// Initialize move destination...
		VectorCopy(bot->r.currentOrigin, bot->bot_sp_destination);
	}

#ifdef __BOT_AAS__
	if ( trap_AAS_Initialized() )
	{														// If aas data is loaded, use it instead of nodes...
		AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
		return;
	}
#endif //__BOT_AAS__

	if ( HeightDistance(bot->r.currentOrigin, bot->bot_sp_spawnpos) > 64 )
	{// Must have fallen down somewhere, set a new spawn pos...
		VectorCopy(bot->r.currentOrigin, bot->bot_sp_spawnpos);
	}

	if (bot->enemy)
	{// If this bot has an enemy, check cover spots!
		if ( Have_Close_Enemy( bot) &&
			bot->bot_enemy_visible_time > level.time &&
			bot->enemy == bot->bot_last_visible_enemy &&
			bot->bot_runaway_time < level.time )
		{	// Do attacking first...
			//AIMOD_MOVEMENT_Attack( bot, ucmd );
			AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			return;
		}
		else
		{// Use cover spots!
#ifdef __OLD_COVER_SPOTS__
			AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
#else //!__OLD_COVER_SPOTS__
			/*if (number_of_nodes > 0 && bot->current_node)
			{
				AIMOD_MOVEMENT_AttackMove( bot, ucmd );
			}
			else
			{*/
				//AIMOD_MOVEMENT_Attack( bot, ucmd );
				AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			//}
#endif //__OLD_COVER_SPOTS__
			//AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			return;
		}
	}

	if (bot->supression > 0 && bot->supression_agressor)
	{// Use cover spots!
		if (MyVisible(bot, bot->supression_agressor))
		{
			bot->enemy = bot->supression_agressor;
			bot->bot_enemy_visible_time = level.time + 1000;
			bot->bot_last_visible_enemy = bot->enemy;
#ifdef __OLD_COVER_SPOTS__
			AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
#else //!__OLD_COVER_SPOTS__
			/*if (number_of_nodes > 0 && bot->current_node)
			{
				//AIMOD_MOVEMENT_AttackMove( bot, ucmd );
				AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			}
			else
			{*/
				//AIMOD_MOVEMENT_Attack( bot, ucmd );
				AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			//}
#endif //__OLD_COVER_SPOTS__
			//AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			return;
		}
	}

	// Actual movement here...
	/*if (bot->bot_last_position_check < level.time - 2000)
	{
		if (VectorDistance(bot->r.currentOrigin, bot->bot_last_position) < 8)
		{

		}
		else
		{
			bot->bot_last_real_move_time = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			bot->bot_last_position_check = level.time;
		}
	}*/

	if ( bot->bot_stand_guard > level.time )
	{// Stand guard here for a moment...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		bot->bot_sniping_time = level.time + 1000;
		AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
		return;
	}
	else if ( bot->bot_sniping_time > level.time )
	{// Ready to find a new destination...
		if (bot->next_sp_move_scan > level.time)
		{// Avoid FPS issues by not looking every frame when stuck!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->bot_sniping_time = level.time + 5000;
			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			return;
		}
		else if (!AIMOD_SP_MOVEMENT_Pick_New_Destination( bot, ucmd ))
		{
			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->next_sp_move_scan = level.time + 2000+Q_TrueRand(0, 1500);
			return;
		}
	}
	else if (VectorDistanceNoHeight(bot->r.currentOrigin, bot->bot_sp_destination) <= 16)
	{// Hit current waypoint...
		bot->bot_stand_guard = level.time + Q_TrueRand(2000, 6000);
		bot->bot_sniping_time = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		//G_Printf("%s - Hit destination!\n", bot->client->pers.netname);
		AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
		return;
	}
	else if (VectorLength(bot->client->ps.velocity) < 12)
	{// Stuck against a wall or something, find a new destination...
		if (SP_NodeVisible(bot->r.currentOrigin, bot->bot_sp_destination, bot->s.number) >= 2)
		{// A door, open it!
			if ( bot->last_use_time < level.time )
			{// Only use the door once each 2 secs max...
				bot->last_use_time = level.time + 2000;
				Cmd_Activate_f( bot );
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
				return;
			}
		}
		else if (bot->next_sp_move_scan > level.time)
		{// Avoid FPS issues by not looking every frame when stuck!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->bot_sniping_time = level.time + 5000;
			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			return;
		}
		else if (!AIMOD_SP_MOVEMENT_Pick_New_Destination( bot, ucmd ))
		{
			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->next_sp_move_scan = level.time + 2000+Q_TrueRand(0, 1500);
			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			return;
		}
	}

	if (!SP_NodeVisible(bot->r.currentOrigin, bot->bot_sp_destination, bot->s.number))
	{// Stop bots running into walls!
		if (bot->next_sp_move_scan > level.time)
		{// Avoid FPS issues by not looking every frame when stuck!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->bot_sniping_time = level.time + 5000;
			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			return;
		}
		else if (!AIMOD_SP_MOVEMENT_Pick_New_Destination( bot, ucmd ))
		{
			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->next_sp_move_scan = level.time + 2000+Q_TrueRand(0, 1500);
			return;
		}
	}

	// Set up ideal view angles for this destination & move...
	VectorSubtract( bot->bot_sp_destination, bot->r.currentOrigin, bot->move_vector );
	bot->move_vector[ROLL] = 0;
	vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );
	ucmd->buttons |= BUTTON_WALKING;
	G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

	AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
}

#endif //__BOT__