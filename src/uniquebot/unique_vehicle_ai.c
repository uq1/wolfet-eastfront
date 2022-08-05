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


#include "../game/q_shared.h"
#ifdef __BOT__
#ifdef __VEHICLES__
#include "../game/g_local.h"
#include "../game/botlib.h"
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_char.h"
#include "../game/be_ai_chat.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../game/be_ai_weap.h"
#include "../botai/botai.h"
#include "../botai/inv.h"

//
#include "../botai/ai_main.h"
#include "../botai/ai_dmq3.h"
#include "../botai/ai_cmd.h"
#include "../botai/ai_team.h"

//
#include "../botai/ai_dmnet_mp.h"

// Key AI constants
#include "../botai/ai_distances.h"
#include "../botai/chars.h"
extern vmCvar_t		bot_cpu_use;
extern float		TravelTime ( gentity_t *bot );
extern void			ClientThink_real ( gentity_t *ent );
extern void			AIMOD_MOVEMENT_Attack ( gentity_t *bot, usercmd_t *ucmd );
extern void			AIMOD_MOVEMENT_AttackMove ( gentity_t *bot, usercmd_t *ucmd );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodes ( gentity_t *bot, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodesTo ( gentity_t *bot, vec3_t origin, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodeTo ( vec3_t origin, int r, int type );
extern qboolean		BAD_WP_Height ( vec3_t start, vec3_t end );
extern qboolean		AIMOD_MOVEMENT_CurrentNodeReachable ( gentity_t *bot );
extern qboolean		AIMOD_MOVEMENT_WaypointTouched ( gentity_t *bot, int touch_node, qboolean onLadder );
extern /*short*/ int	BotGetNextNode ( gentity_t *bot /*, int *state*/ ); // below...
extern void			BotSetViewAngles ( gentity_t *bot, float thinktime );
extern void			G_UcmdMoveForDir ( gentity_t *self, usercmd_t *cmd, vec3_t dir );
extern int			Unique_FindGoal ( gentity_t *bot );
extern int			CreatePathAStar ( gentity_t *bot, int from, int to, /*short*/ int *pathlist );
extern int			SP_NodeVisible ( vec3_t org1, vec3_t org2, int ignore );
extern int			Find_POP_Capture_Point ( gentity_t *bot, vec3_t current_org, int teamNum );
extern float		VectorDistanceNoHeight ( vec3_t v1, vec3_t v2 );
extern void			AIMOD_MOVEMENT_IncreaseLinkageCost ( gentity_t *bot );

// In unique_goal.c
extern int			Find_Goal_Teammate ( gentity_t *bot );
extern int			Find_Goal_Enemy ( gentity_t *bot );
extern int			BotMP_Sniper_Find_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
extern int			BotMP_Find_POP_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
extern int			BotMP_Engineer_Find_POP_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
extern int			BotMP_FieldOps_Find_POP_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
extern int			BotMP_Soldier_Find_POP_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
extern void			BotMP_Update_Goal_Lists ( void );
extern void			BotDropToFloor ( gentity_t *ent );
qboolean			have_land_vehicle_waypoints = qfalse;
qboolean			land_vehicle_waypoints_checked = qfalse;


/* */
qboolean
BOT_HaveVehicleWaypoints ( void )
{
	int i;
	if ( land_vehicle_waypoints_checked )
	{
		return ( have_land_vehicle_waypoints );
	}

	land_vehicle_waypoints_checked = qtrue;
	for ( i = 0; i < number_of_nodes; i++ )
	{
		if ( nodes[i].type & NODE_LAND_VEHICLE )
		{
			have_land_vehicle_waypoints = qtrue;
			return ( qtrue );
		}
	}

	return ( qfalse );
}


//===========================================================================
// Routine      : Vehicle_FindGoal

// Description  : Find a new goal entity and node.
int
Vehicle_FindGoal ( gentity_t *bot )
{							// Returns goal waypoint...
	int		entNum = -1;
	int		best_target = -1;
	float	best_dist = 99999.9f;
	
	bot->bot_sniping = qfalse;
	VectorSet( bot->bot_sniper_spot, 0, 0, 0 );

	// Init the bot constructible marker...
	bot->bot_goal_constructible = qfalse;

	// Init the bot destroyable marker...
	bot->bot_goal_destroyable = qfalse;
	best_target = -1;		// Initialize with each loop...
	best_dist = 99999.9f;	// Initialize with each loop...
	
	if (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
	{
		entNum = Find_POP_Capture_Point( bot, bot->r.currentOrigin, bot->client->sess.sessionTeam );
	}

	if ( (entNum < 0 || Q_TrueRand(0,2) == 0) 
		&& bot->enemy 
		&& bot->enemy->health > 0 
		&& !(bot->enemy->s.eFlags & EF_DEAD) 
		&& !(bot->enemy->client && (bot->enemy->client->ps.pm_flags & PMF_LIMBO)) )
	{
		entNum = bot->enemy->s.number;
	}

	if ( entNum < 0 )
	{
		entNum = Find_Goal_Enemy( bot );
	}

	if ( entNum >= 0 )
	{
		best_target = AIMOD_NAVIGATION_FindClosestReachableNode( &g_entities[entNum], NODE_DENSITY, NODEFIND_TANK, bot );
	}

	if ( best_target >= 0 )
	{// Found a goal.. Use it...
		bot->current_target_entity = entNum;
		bot->goalentity = &g_entities[entNum];

		// Return the best choice...
		return ( best_target );
	}
	else
	{
		bot->current_target_entity = -1;
		bot->goalentity = NULL;
	}

	// If we got here, then we failed to find a path to a goal...
	return ( -1 );
}


/* */
void
Bot_Set_New_Vehicle_Path ( gentity_t *bot, usercmd_t *ucmd )
{
	bot->longTermGoal = -1;

	if (SSE_CPU)
	{
		sse_memset( bot->pathlist, NODE_INVALID, ((sizeof(int)) * MAX_PATHLIST_NODES)/8 );
	}
	else
	{
		memset( bot->pathlist, NODE_INVALID, ((sizeof(int)) * MAX_PATHLIST_NODES) );
	}

	bot->pathsize = 0;

	if ( bot->last_astar_path_think > level.time - 2000 )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	bot->last_astar_path_think = level.time;

	// Initialize...
	bot->shortTermGoal = -1;

	bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_TANK, bot );

	bot->goalentity = NULL;
	bot->longTermGoal = Vehicle_FindGoal( bot );

	if ( !bot->longTermGoal && bot->goalentity && bot->goalentity->client && bot->goalentity->health > 0 )
	{// If our goal entity is a player/bot, continue heading toward them...
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( bot->goalentity, NODE_DENSITY, NODEFIND_TANK, bot );
	}

	if ( bot->longTermGoal < 0 )
	{													// Find an enemy to head for...
		int best_ent = Find_Goal_Enemy( bot );

		if ( best_ent >= 0 )
		{
			bot->goalentity = &g_entities[best_ent];
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( bot->goalentity, NODE_DENSITY, NODEFIND_TANK, bot );
		}
	}

	if ( bot->longTermGoal < 0 )
	{													// Just get us out of trouble...
		bot->goalentity = NULL;
		bot->longTermGoal = -1;//bot->current_node + 5;
		return;
	}

	bot->last_node = bot->current_node - 1;
	bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
	bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
	bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;
}


//===========================================================================
// Routine      : Vehicle_BotEntityCloserOnFuturePath

// Description  : Is an entity closer to one of our current nodes then to us now?
qboolean
Vehicle_BotEntityCloserOnFuturePath ( gentity_t *bot, gentity_t *ent )
{					// Return qtrue if future nodes are closer to "ent" then the current one...
	int		pathsize = bot->pathsize;
	float	current_dist = VectorDistance( ent->r.currentOrigin, nodes[bot->current_node].origin );
	while ( pathsize > 0 )
	{
		if ( !nodes[bot->pathlist[pathsize - 1]].origin )
		{
			break;
		}

		if ( VectorDistance( ent->r.currentOrigin, nodes[bot->pathlist[pathsize - 1]].origin) < current_dist )
		{
			return ( qtrue );
		}

		pathsize--; //mark that we've moved another node
	}

	return ( qfalse );
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_VehicleAttack
// Description  : Attack movement
//              : Just basic move and avoidance

// TODO         : Change routine for more advanced attack movement
void
AIMOD_MOVEMENT_VehicleAttack ( gentity_t *bot, usercmd_t *ucmd )
{
	float	firedelay;
	vec3_t	target;

	// Make sure we continue along our current route after the attack is over...
	bot->node_timeout = level.time + ( TravelTime( bot) * 4 );
	bot->bot_last_good_move_time = level.time;
	
	if ( !bot->enemy/* || bot->bot_enemy_visible_time < level.time*/ )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;										// Shouldn't have got this far...
	}

	// Set the attack
	//-------------------------------------------------------
	firedelay = ( random() - 0.5 ) * 5;

	// Aim
	VectorCopy( bot->enemy->r.currentOrigin, target );

	// Set direction
	//-------------------------------------------------------
	VectorSubtract( target, bot->r.currentOrigin, bot->attack_vector );
	vectoangles( bot->attack_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );
	
	if (!MyInFOV(bot->enemy, bot, 3, 90))
	{												// Wait until we are looking toward them to fire...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		ucmd->buttons = 0;
		return;
	}

	// Added Skill Level Settings for Attack
	bot->tank_attack_time = level.time;
	if ( bot->skillchoice == 0 )
	{
		if ( firedelay < 0.10 )
		{
			if ( bot->client->ps.persistant[PERS_TANKUSED] == VEHICLE_TYPE_FLAMETANK
				&& VectorDistance(bot->r.currentOrigin, bot->enemy->r.currentOrigin) < 1500 )
			{
				ucmd->buttons = BUTTON_ATTACK;		// Flame tank, use flame is they are in range...
			}
			else if ( bot->enemy->client && (bot->enemy->client->ps.eFlags & EF_VEHICLE) )
			{
				ucmd->buttons = BUTTON_ATTACK;		// Attacking another vehicle... Use turret.
			}
			else
			{
				ucmd->wbuttons = WBUTTON_ATTACK2;	// Attacking a humanoid... Use MG.
			}
		}
	}
	else if ( bot->skillchoice == 1 )
	{
		if ( firedelay < 0.20 )
		{
			if ( bot->client->ps.persistant[PERS_TANKUSED] == VEHICLE_TYPE_FLAMETANK
				&& VectorDistance(bot->r.currentOrigin, bot->enemy->r.currentOrigin) < 1500 )
			{
				ucmd->buttons = BUTTON_ATTACK;		// Flame tank, use flame is they are in range...
			}
			else if ( bot->enemy->client && bot->enemy->client->ps.eFlags & EF_VEHICLE )
			{
				ucmd->buttons = BUTTON_ATTACK;		// Attacking another vehicle... Use turret.
			}
			else
			{
				ucmd->wbuttons = WBUTTON_ATTACK2;	// Attacking a humanoid... Use MG.
			}
		}
	}
	else if ( bot->skillchoice == 2 )
	{
		if ( firedelay < 0.40 )
		{
			if ( bot->client->ps.persistant[PERS_TANKUSED] == VEHICLE_TYPE_FLAMETANK
				&& VectorDistance(bot->r.currentOrigin, bot->enemy->r.currentOrigin) < 1500 )
			{
				ucmd->buttons = BUTTON_ATTACK;		// Flame tank, use flame is they are in range...
			}
			else if ( bot->enemy->client && bot->enemy->client->ps.eFlags & EF_VEHICLE )
			{
				ucmd->buttons = BUTTON_ATTACK;		// Attacking another vehicle... Use turret.
			}
			else
			{
				ucmd->wbuttons = WBUTTON_ATTACK2;	// Attacking a humanoid... Use MG.
			}
		}
	}
	else if ( bot->skillchoice >= 3 )
	{
		if ( bot->client->ps.persistant[PERS_TANKUSED] == VEHICLE_TYPE_FLAMETANK
			&& VectorDistance(bot->r.currentOrigin, bot->enemy->r.currentOrigin) < 1500 )
		{
			ucmd->buttons = BUTTON_ATTACK;		// Flame tank, use flame is they are in range...
		}
		else if ( bot->enemy->client && bot->enemy->client->ps.eFlags & EF_VEHICLE )
		{
			ucmd->buttons = BUTTON_ATTACK;			// Attacking another vehicle... Use turret.
		}
		else
		{
			ucmd->wbuttons = WBUTTON_ATTACK2;		// Attacking a humanoid... Use MG.
		}
	}

	bot->teamPauseTime = level.time;
}

vec3_t	visible_end;


/* */
int
Tank_PositionVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg, targetOrg;
	vec3_t	tankTraceMaxs = {96, 96, 0/*24*//*40*/};
	vec3_t	tankTraceMins = {-96, -96, 0};

	VectorCopy( org1, newOrg );
	newOrg[2] += 32;	// Look from up a little...

	VectorCopy( org2, targetOrg );
	targetOrg[2] += 32;	// Look from up a little...

	trap_Trace( &tr, newOrg, tankTraceMins, tankTraceMaxs, targetOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA | CONTENTS_WATER );
	VectorCopy( tr.endpos, visible_end );
	
	if ( tr.startsolid || tr.allsolid )
	{
		return ( 4 );
	}

	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	/*if
	(
		tr.fraction != 1 &&
		tr.entityNum != ENTITYNUM_NONE &&
		(
			!strcmp( g_entities[tr.entityNum].classname, "func_door") || !strcmp( g_entities[tr.entityNum].classname,
																				  "func_door_rotating")
		)
	)
	{					// A door in the way.. See if we have access...
		if ( !g_entities[tr.entityNum].allowteams )
		{
			return ( 2 );
		}

		return ( 3 );
	}*/

	return ( 0 );
}


/* */
int
AIMOD_Vehicle_CheckNextNode ( gentity_t *bot )
{
	int		/*i,*/ outcome;
	//vec3_t	forward, forward_pos, back_pos, forward_pos2, back_pos2;
	if ( !bot->next_node || !bot->current_node )
	{
		bot->vehicle_pause_time = level.time + 2000;
		return -1;
	}

/*	// UQ1: Try to fix any times we are stuck in a solid...
	AngleVectors( bot->r.currentAngles, forward, NULL, NULL );
	VectorMA( bot->r.currentOrigin, 128, forward, forward_pos );
	VectorMA( bot->r.currentOrigin, 64, forward, forward_pos2 );
	VectorMA( bot->r.currentOrigin, -128, forward, back_pos );
	VectorMA( bot->r.currentOrigin, -64, forward, back_pos2 );
	outcome = PositionVisible( bot->r.currentOrigin, forward_pos, bot->s.number );
	if ( outcome == 4 )
	{		// In a solid! Cheat: Teleport direct to our node!
		vec3_t	good_dir, angle;
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, good_dir );
		vectoangles( good_dir, angle );
		AngleVectors( angle, forward, NULL, NULL );
		VectorMA( bot->r.currentOrigin, 8, forward, forward_pos );
		VectorCopy( forward_pos, bot->r.currentOrigin );
		VectorCopy( forward_pos, bot->s.origin );
		VectorCopy( forward_pos, bot->client->ps.origin );
	}
	else if ( outcome != 1 )
	{
		vec3_t	good_dir, angle;
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, good_dir );
		vectoangles( good_dir, angle );
		AngleVectors( angle, forward, NULL, NULL );
		VectorMA( bot->r.currentOrigin, 8, forward, forward_pos );
		VectorCopy( forward_pos, bot->r.currentOrigin );
		VectorCopy( forward_pos, bot->s.origin );
		VectorCopy( forward_pos, bot->client->ps.origin );
	}

	outcome = PositionVisible( bot->r.currentOrigin, back_pos, bot->s.number );
	if ( outcome == 4 )
	{		// In a solid! Cheat: Teleport direct to our node!
		vec3_t	good_dir, angle;
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, good_dir );
		vectoangles( good_dir, angle );
		AngleVectors( angle, forward, NULL, NULL );
		VectorMA( bot->r.currentOrigin, 8, forward, forward_pos );
		VectorCopy( forward_pos, bot->r.currentOrigin );
		VectorCopy( forward_pos, bot->s.origin );
		VectorCopy( forward_pos, bot->client->ps.origin );
	}
	else if ( outcome != 1 )
	{
		vec3_t	good_dir, angle;
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, good_dir );
		vectoangles( good_dir, angle );
		AngleVectors( angle, forward, NULL, NULL );
		VectorMA( bot->r.currentOrigin, 8, forward, forward_pos );
		VectorCopy( forward_pos, bot->r.currentOrigin );
		VectorCopy( forward_pos, bot->s.origin );
		VectorCopy( forward_pos, bot->client->ps.origin );
	}*/

	// UQ1: Try to fix any times we are stuck in a solid...
	//AngleVectors( bot->r.currentAngles, forward, NULL, NULL );
	//VectorMA( bot->r.currentOrigin, 128, forward, forward_pos );
	//VectorMA( bot->r.currentOrigin, 64, forward, forward_pos2 );
	//VectorMA( bot->r.currentOrigin, -128, forward, back_pos );
	//VectorMA( bot->r.currentOrigin, -64, forward, back_pos2 );
	//outcome = Tank_PositionVisible( bot->r.currentOrigin, forward_pos, bot->s.number );
	outcome = Tank_PositionVisible( bot->r.currentOrigin, nodes[bot->current_node].origin, bot->s.number );
	
	return outcome;

	/*if ( outcome != 1 )
	{		// Stuck! Disable bbox!
		bot->vehicle_bbox_cheat_time = level.time + 1000;
	}*/
}

/* */
qboolean
AIMOD_Vehicle_PlayerInFront ( gentity_t *bot )
{
	int i;

#ifdef __NPC__
	for ( i = 0; i < MAX_GENTITIES; i++ )
#else //!__NPC__
	for ( i = 0; i < MAX_CLIENTS; i++ )
#endif //__NPC__
	{
		gentity_t	*ent = &g_entities[i];

#ifdef __NPC__
		if ( !ent || (!ent->client && !ent->NPC_client) )
#else //!__NPC__
		if ( !ent || !ent->client )
#endif //__NPC__
		{
			continue;
		}

		if (ent->client)
		{
			if ( ent->client->ps.stats[STAT_HEALTH] <= 0 )
				continue;

			if ( ent->client->pers.connected != CON_CONNECTED )
				continue;

			if ( ent->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
				continue;								// Don't aim at spectators lol...

			if ( ent->client->ps.pm_flags & PMF_LIMBO )
				continue;
		}

		if ( ent == bot )
		{
			continue;
		}

		if ( ent->client && (ent->client->ps.eFlags & EF_VEHICLE)
			&& ((bot->current_node && VectorDistance( ent->r.currentOrigin, nodes[bot->current_node].origin) < 360) || VectorDistance( ent->r.currentOrigin, bot->r.currentOrigin) < 360))
		{	// Tell the tank to pause for a few seconds to let the entity move off...
			if (MyInFOV( ent, bot, 90/*110*/, 30/*90*/))
			{
				bot->vehicle_pause_time = level.time + 100;
				return ( qtrue );
			}
		}
		else if ( (bot->current_node && VectorDistance( ent->r.currentOrigin, nodes[bot->current_node].origin) < 128) 
			|| VectorDistance( ent->r.currentOrigin, bot->r.currentOrigin) < 128/*256*/)
		{	// Tell the tank to pause for a few seconds to let the entity move off...
			if (MyInFOV( ent, bot, 90/*110*/, 30/*90*/))
			{
				bot->vehicle_pause_time = level.time + 100;
				return ( qtrue );
			}
		}
	}

	return ( qfalse );
}

extern qboolean MyInFOV2 ( vec3_t origin, gentity_t *from, int hFOV, int vFOV );

//===========================================================================
// Routine      : AIMOD_MOVEMENT_VehicleMovementBackup

// Description  : Fallback navigation. For emergency routing...
void
AIMOD_MOVEMENT_VehicleMovementBackup ( gentity_t *bot, usercmd_t *ucmd )
{																// Fallback navigation. returns if it was used or not as qtrue/qfalse.
	//int		vis;
	//vec3_t	forward, forward_pos;
	//AngleVectors( bot->s.angles, forward, NULL, NULL );
	//VectorCopy(bot->r.currentOrigin, forward_pos);
	//VectorMA( forward_pos, 512, forward, forward_pos );
	//vis = Tank_PositionVisible( bot->r.currentOrigin, forward_pos, bot->s.number );

	//0 = wall in way
	//1 = player or no obstruction
	//2 = useable door in the way.
	//3 = door entity in the way.
	if ( bot->client )
	{
		vec3_t forward_pos, forward, viewangles;

		VectorCopy(bot->s.angles, viewangles);
		viewangles[2] = 0;
		viewangles[0] = 0;

		VectorCopy(bot->r.currentOrigin, forward_pos);
		AngleVectors (viewangles, forward, NULL, NULL);
		VectorMA( forward_pos, 192, forward, forward_pos );

		if ( !MyInFOV2(forward_pos, bot, 3, 90) )
		{
			BotSetViewAngles( bot, 100 );
			VectorCopy(bot->client->ps.viewangles, bot->s.angles);
			bot->bot_last_vischeck_node_time = level.time + 5000;
			
			// Velocity...
			bot->bot_last_good_velocity_time = level.time + 500;
			VectorCopy(bot->r.currentOrigin, bot->bot_movetarget_stored_pos);
		}
		else if ( bot->bot_last_good_velocity_time < level.time )
		{// We havn't moved at all... Let's randomize things.
			vec3_t	temp;

			VectorCopy( bot->s.angles, temp );
			temp[0] = 0;
			temp[2] = 0;
			
			if (Q_TrueRand(0,2) < 2)
			{// Mostly try to continue in this direction as often as possible...
				temp[1] += Q_TrueRand( -70, +70 );
			}
			else
			{
				temp[1] = Q_TrueRand( 0, 360 );
			}

			bot->client->AImovetime = level.time + 2000;		// 10 seconds?
			VectorCopy( temp, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			VectorCopy(bot->client->ps.viewangles, bot->s.angles);

			bot->bot_last_vischeck_node_time = level.time + 5000;

			// Velocity...
			bot->bot_last_good_velocity_time = level.time + 500;
			VectorCopy(bot->r.currentOrigin, bot->bot_movetarget_stored_pos);

			//G_Printf("%s move reset!\n", bot->client->pers.netname);
		}
		else
		{
			trace_t tr;
			vec3_t	down_pos;

			BotSetViewAngles( bot, 100 );
			VectorCopy(bot->client->ps.viewangles, bot->s.angles);

			forward_pos[2]+=48;
			VectorCopy(forward_pos, down_pos);
			down_pos[2]-=96; // Max allowed terrain drop distance (compared to 48 forward)

			trap_Trace( &tr, forward_pos, NULL, NULL, down_pos, bot->s.number, MASK_PLAYERSOLID | MASK_WATER );

			if (tr.fraction != 1 
				&& !(tr.allsolid)
				&& !(tr.startsolid)
				&& !(tr.contents & CONTENTS_WATER) 
				&& !(tr.contents & CONTENTS_LAVA) 
				&& !(tr.contents & CONTENTS_SLIME))
			{
				ucmd->forwardmove = 127;

				//if (VectorLength(bot->client->ps.velocity) > 16)
					bot->bot_last_good_velocity_time = level.time + 500;
			}
			else
			{
				bot->bot_last_good_velocity_time = 0;
			}

			//G_Printf("%s moving!\n", bot->client->pers.netname);
		}
	}
}

extern void BOT_MakeLinkSemiUnreachable ( gentity_t *bot, int node, int linkNum );
extern void BOT_IncreaseLinkCost ( gentity_t *bot, int node, int linkNum );

void AIMOD_VEHICLE_MOVEMENT_IncreaseLinkageCost ( gentity_t *bot )
{
	int			loop;
	qboolean	found = qfalse;

	if (!bot || !bot->last_node || !bot->current_node)
		return;

	for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
	{			// Find the link we last used and mark it...
		if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
		{		// Mark it as Dangerous...
			BOT_IncreaseLinkCost( bot, bot->last_node, loop );
			found = qtrue;
			break;
		}
	}

	if (!found /*&& bot->last_node == bot->current_node - 1*/)
	{// A bad current node, disable vehicles here!
		nodes[bot->current_node].type &= ~NODE_LAND_VEHICLE;
	}
}

//===========================================================================
// Routine      : AIMOD_MOVEMENT_VehicleMove

// Description  : Main movement (following node paths)
void
AIMOD_MOVEMENT_VehicleMove ( gentity_t *bot, usercmd_t *ucmd )
{
	qboolean	onLadder = qfalse;
//	int			outcome;

	if ( bot->health <= 0 )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if ( bot->client->ps.pm_flags & PMF_LIMBO )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	// ---------------------------------------------
	// OK... First the movement for non-routed maps.
	// ---------------------------------------------

	if ( !nodes_loaded )
	{
		AIMOD_MOVEMENT_VehicleMovementBackup( bot, ucmd );
		bot->bot_last_position_check = level.time;
		bot->bot_last_good_move_time = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		return;
	}

	// ---------------------------------------------
	// OK... Now the movement for routed maps.
	// ---------------------------------------------

	if ( bot->bot_last_position_check < level.time - 2000 || bot->bot_last_good_move_time < level.time - 500 )
	{
		if ( VectorDistanceNoHeight( bot->bot_last_position, bot->r.currentOrigin) <= 4/*64*/  )
		{// We are stuck... Find a new route!
			AIMOD_VEHICLE_MOVEMENT_IncreaseLinkageCost( bot );

			bot->current_node = -1;
			bot->longTermGoal = -1;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;

			bot->bot_last_good_move_time = level.time;
			//G_Printf("AI VEHICLE: %s position check reset! (1)\n", bot->client->pers.netname);
		}
		/*else if (bot->bot_last_good_move_time < level.time - 500)
		{
			AIMOD_VEHICLE_MOVEMENT_IncreaseLinkageCost( bot );

			bot->current_node = -1;
			bot->longTermGoal = -1;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;

			bot->bot_last_good_move_time = level.time;
			G_Printf("AI VEHICLE: %s move time check reset! (2)\n", bot->client->pers.netname);
		}*/

		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
	}

	if ( bot->current_node < 0 
		|| bot->current_node > number_of_nodes 
		|| bot->longTermGoal < 0 
		|| bot->longTermGoal > number_of_nodes)
	{
		Bot_Set_New_Vehicle_Path( bot, ucmd );
		bot->bot_last_good_move_time = level.time;
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

		if ( bot->current_node < 0 || bot->current_node > number_of_nodes || bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes)
		{// Still none... Wait!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			//bot->vehicle_pause_time = level.time + 1000;
			bot->bot_random_move_time = level.time + 4000;
			bot->bot_last_good_move_time = level.time;
			
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

			//G_Printf("AI VEHICLE: %s node reset! Failed to find a path!\n", bot->client->pers.netname);
			return;
		}
		else
		{
			//G_Printf("AI VEHICLE: %s node reset! OK! Path Found!\n", bot->client->pers.netname);
		}
	}

	if ( (bot->enemy && bot->bot_enemy_visible_time > level.time) || (bot->enemy && VectorDistance(bot->r.currentOrigin, bot->enemy->r.currentOrigin) < 256) )
	{
		BotSetViewAngles( bot, 100 );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		AIMOD_MOVEMENT_VehicleAttack( bot, ucmd );
		bot->bot_last_good_move_time = level.time;
		
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

		//G_Printf("AI VEHICLE: %s attacking!\n", bot->client->pers.netname);
		return;
	}

	if ( bot->vehicle_pause_time > level.time)
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		bot->bot_last_good_move_time = level.time;
		
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

		//G_Printf("AI VEHICLE: %s pause time!\n", bot->client->pers.netname);
		return;
	}
	
	/*if (bot->bot_random_move_time > level.time)
	{
		bot->current_node = -1;
		bot->longTermGoal = -1;
		AIMOD_MOVEMENT_VehicleMovementBackup( bot, ucmd );
		bot->bot_last_good_move_time = level.time;
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		return;
	}*/

	if ( AIMOD_Vehicle_PlayerInFront(bot) )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		bot->vehicle_pause_time = level.time + 100;
		bot->bot_last_good_move_time = level.time;
		
		//G_Printf("AI VEHICLE: %s player in front!\n", bot->client->pers.netname);

		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		return;
	}

#ifdef __ETE__
	if ( bot->current_node >= 0 )
	{					// We have a routed map.. Do some flag capture checks...
		if
		(
			bot->client->ps.stats[STAT_FLAG_CAPTURE] > 0 &&
			g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != bot->client->sess.sessionTeam &&
			bot->current_node > 0
		)
		{
			if
			(
				!Vehicle_BotEntityCloserOnFuturePath( bot, &g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]]) &&
				!Have_Close_Enemy( bot)
			)
			{// If we're capturing a flag in POP gametype don't move, just defend current pos...
				bot->bot_last_good_move_time = level.time;
				
				bot->node_timeout = level.time + TravelTime( bot );
				bot->vehicle_pause_time = level.time + 100;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				//G_Printf("AI VEHICLE: %s capture flag!\n", bot->client->pers.netname);

				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

				return; // Don't move while capturing a flag...
			}
		}
	}
#endif //__ETE__

	// Now we need to check if we have hit our current node...
	if ( AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
	{																// At current node.. Pick next in the list...
		if ( bot->current_node == bot->longTermGoal )
		{																// We're there! Find a new goal... FIXME: We should actually do something at most goals..
			bot->longTermGoal = -1;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->node_timeout = level.time + ( TravelTime( bot) * 4 );	//6000;//4000;
			bot->bot_last_good_move_time = level.time;
			
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			//G_Printf("%s goal waypoint touched!\n", bot->client->pers.netname);
			return;
		}

		bot->last_node = bot->current_node;
		bot->current_node = bot->next_node;
		bot->next_node = BotGetNextNode( bot );

		// Set our last good move time to now...
		bot->node_timeout = level.time + TravelTime( bot );
	}

	if (bot->current_node < 0 || bot->current_node > number_of_nodes)
	{
		bot->current_node = -1;
		bot->longTermGoal = -1;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		bot->node_timeout = level.time + ( TravelTime( bot) * 4 );	//6000;//4000;
		bot->bot_last_good_move_time = level.time;
			
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		return;
	}

	// Set up ideal view angles for this enemy...
	VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
	bot->move_vector[ROLL] = 0;
	vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );

	VectorCopy(bot->client->ps.viewangles, bot->s.angles);
	//G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

	//if ( VectorDistance( bot->client->ps.viewangles, bot->bot_ideal_view_angles) < 16 )
	if ( MyInFOV2(nodes[bot->current_node].origin, bot, 3, 90) )
	{
		ucmd->forwardmove = 127;
		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
	}
	//else
	//{
	//	G_Printf("%s not facing waypoint!\n", bot->client->pers.netname);
	//}
}
#endif //__VEHICLES__
#endif //__BOT__
