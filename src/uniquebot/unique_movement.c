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


//===========================================================================
// Routine      : AIMOD_AAS_MOVEMENT_Move

// Description  : AAS Movement extras... Unused..
#ifdef __BOT_AAS__
void
AIMOD_AAS_MOVEMENT_Move ( gentity_t *bot, usercmd_t *ucmd )
{																// Make use of AAS stuff...
	return;
}
#endif //__BOT_AAS__

int		avoid_node_timer[MAX_NODES];

//
// Externals
//
qboolean AIMOD_MOVEMENT_Avoid_Explosives ( gentity_t *bot, usercmd_t *ucmd );

extern int Pickup_Weapon( gentity_t *ent, gentity_t *other );
extern int Pickup_Ammo (gentity_t *ent, gentity_t *other);

extern int			numAxisOnlyNodes;
extern int			numAlliedOnlyNodes;
extern int			myteam, otherteam;
extern void			Set_Best_AI_weapon ( gentity_t *bot );
//extern node_t		nodes[MAX_NODES];
node_t				*nodes;
extern int			AIMOD_Calculate_Goal ( gentity_t *bot );
extern void			Cmd_Activate_f ( gentity_t *ent );
extern qboolean	MyVisible ( gentity_t *self, gentity_t *other );
extern qboolean	MyVisible_No_Supression ( gentity_t *self, gentity_t *other );
extern qboolean AIMOD_AI_Spot_Safe_For_Grenade ( gentity_t *bot, vec3_t origin );
extern int			BotMP_Num_Defenders_Near ( vec3_t pos, gentity_t *bot );
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
#ifdef __BOT_AAS__
extern int			BotMP_Medic_Find_POP_Goal_EntityNum
					(
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum,
						bot_state_t *bs
					);
extern int			BotMP_Medic_Scan_For_Revives
					(
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum,
						bot_state_t *bs
					);
#else //!__BOT_AAS__
extern int			BotMP_Medic_Find_POP_Goal_EntityNum
					(
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum,
						gentity_t	*bot
					);
#endif
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
extern int			CreatePathAStar ( gentity_t *bot, int from, int to, /*short*/ int *pathlist );
extern int			OrgVisibleBox ( vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodeTo ( vec3_t origin, int r, int type );
extern int			AIMOD_NAVIGATION_FindFollowRouteClosestReachableNodeTo ( gentity_t *owner, vec3_t origin, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNode ( gentity_t *bot, int r, int type, gentity_t *master );
extern int			AIMOD_NAVIGATION_FindCloseReachableNode ( gentity_t *bot, int r, int type );
extern int			AIMOD_NAVIGATION_FindFarReachableNode ( gentity_t *bot, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodesTo ( gentity_t *bot, vec3_t origin, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodes ( gentity_t *bot, int r, int type );
extern void			Check_VoiceChats ( gentity_t *bot );
extern void			AI_ResetJob ( gentity_t *bot );
extern int			JobType ( gentity_t *bot );
extern void			AIMOD_Player_Special_Needs_Clear ( int clientNum );
extern qboolean		nodes_loaded;
extern qboolean		NodesLoaded ( void );
extern void			AIMOD_AI_DropMed ( gentity_t *bot );
extern void			AIMOD_AI_DropAmmo ( gentity_t *bot );
extern int			number_of_nodes;
extern gentity_t	*Dynamite_Entity ( gentity_t *ent );
extern gentity_t	*foundplayer;								// Closest found player to us after player_close calls... // AIMOD_nodes.c
extern void			G_BotCheckForCursorHints ( gentity_t *ent );
extern int			no_mount_time[MAX_CLIENTS];					// Don't try to mount emplaced for so many secs...
extern int			next_satchel_available_time[MAX_CLIENTS];	// Next time covert can use a satchel... - g_main.c
extern vec3_t		snipe_points[256];
extern int			num_snipe_points;
extern qboolean		above_average_node_height ( int nodeNum );
extern int			BotMP_Defender_Find_POP_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
//extern vmCvar_t		bot_forced_auto_waypoint;					// Forced random bots to auto-waypoint the map...
extern qboolean		MyInfront ( gentity_t *self, gentity_t *other );
extern qboolean		OnIce[MAX_CLIENTS];							// Set if bot is on ice or not.
extern qboolean		Can_Airstrike ( gentity_t *ent/*, vec3_t origin*/ );				// AIMOD_navigation.c
extern qboolean		Can_Arty ( gentity_t *ent, vec3_t origin );						// AIMOD_navigation.c
extern qboolean		Teammate_SpashDamage_OK ( gentity_t *ent, vec3_t origin );		// AIMOD_navigation.c
extern void			Weapon_Artillery ( gentity_t *ent );							// g_weapon.c
extern gentity_t	*weapon_grenadelauncher_fire ( gentity_t *ent, int grenType );	// g_weapon.c
extern int			NodeVisible ( vec3_t org1, vec3_t org2, int ignore );
extern vmCvar_t		bot_skill;
extern float		anglemod ( float a );
extern vmCvar_t		bot_thinktime;

// In unique_goal.c
extern int			Find_Goal_Teammate ( gentity_t *bot );
extern int			Find_Goal_Enemy ( gentity_t *bot );

// In unique_fireteam_ai.c
extern qboolean AIMOD_MOVEMENT_FireTeam_AI ( gentity_t *bot, usercmd_t *ucmd, qboolean new_follow_route, qboolean do_attack_move );

// In unique_coverspots.c
extern void AIMOD_SP_MOVEMENT_CoverSpot_Attack ( gentity_t *bot, usercmd_t *ucmd, qboolean useProne );
extern qboolean AIMOD_SP_MOVEMENT_Is_CoverSpot_Weapon ( int weapon );

//
// Locals
//
vec3_t				last_position[MAX_CLIENTS];
qboolean			debug_mode = qfalse;
/*short*/ int			BotGetNextNode ( gentity_t *bot /*, int *state*/ );				// below...
void				NowHeal ( gentity_t *bot );
vec3_t				last_vector[MAX_CLIENTS];
int					last_node_set_time[MAX_CLIENTS];
int					randmomization_time[MAX_CLIENTS];
int					next_drop[MAX_CLIENTS];
int					out_of_trouble[MAX_CLIENTS];
int					reset_time[MAX_CLIENTS];
int					next_dynamite_test[MAX_CLIENTS];
int					next_visibility_test[MAX_CLIENTS];
vec3_t				planted_origin[MAX_CLIENTS];									// The origin we dropped a satchel charge at!
int					next_turret_angle_change[MAX_CLIENTS];							// Next time to change angles while on a tank or mg42 or aagun.
qboolean			runaway_node[MAX_CLIENTS];
vec3_t				SP_Bot_Position[MAX_CLIENTS];
int					Unique_FindGoal ( gentity_t *bot );
qboolean			last_medic_check[MAX_CLIENTS];
int					last_medic_check_time[MAX_CLIENTS];
int					last_node_hit_time[MAX_CLIENTS];
int					duck_time[MAX_CLIENTS];
qboolean			AIMOD_MOVEMENT_CanMove ( gentity_t *bot, int direction );		// Below...
void				Bot_Set_New_Path ( gentity_t *bot, usercmd_t *ucmd );			// below
void				AIMOD_MOVEMENT_AttackMove ( gentity_t *bot, usercmd_t *ucmd );	// below...
int					next_arty[MAX_CLIENTS];
int					next_grenade[MAX_CLIENTS];
int					after_enemy[MAX_CLIENTS];


//===========================================================================
//
// Bot visibility code...
//
//===========================================================================
//===========================================================================
// Routine      : OrgVisibleCurve

// Description  : Check visibility at same heights...
int
OrgVisibleCurve ( vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore )
{	// Check visibility at same heights...
	trace_t tr;
	vec3_t	evenorg1;
	VectorCopy( org1, evenorg1 );
	evenorg1[2] = org2[2];
	trap_Trace( &tr, evenorg1, mins, maxs, org2, ignore, MASK_SOLID );
	if ( tr.fraction == 1 && !tr.startsolid && !tr.allsolid )
	{
		trap_Trace( &tr, evenorg1, mins, maxs, org1, ignore, MASK_SOLID );
		if ( tr.fraction == 1 && !tr.startsolid && !tr.allsolid )
		{
			return ( 1 );
		}
	}

	return ( 0 );
}


//===========================================================================
// Routine      : NodeOrgVisible
// Description  : Special node visibility check for bots... Returns the values below...
// 0 = wall in way
// 1 = player or no obstruction
// 2 = useable door in the way.
// 3 = useable team door in the way.

// 4 = door entity in the way.
int
NodeOrgVisible ( gentity_t *bot, vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg;

	// Look from up a little for bots..
	VectorCopy( org1, newOrg );
	newOrg[2] += 32;
	trap_Trace( &tr, newOrg, NULL, NULL, org2, ignore, MASK_SOLID );
	if ( tr.fraction == 1 )
	{
		trap_Trace( &tr, newOrg, NULL, NULL, org2, ignore, MASK_PLAYERSOLID );
		if
		(
			tr.fraction != 1 &&
			tr.entityNum != ENTITYNUM_NONE &&
			!strcmp( g_entities[tr.entityNum].classname, "func_door")
		)
		{	// A door in the way.. See if we have access...
			if ( !g_entities[tr.entityNum].allowteams )
			{
				return ( 2 );
			}

			if ( (g_entities[tr.entityNum].allowteams & TEAM_ALLIES) && bot->client->sess.sessionTeam == TEAM_ALLIES )
			{
				return ( 3 );
			}

			if ( (g_entities[tr.entityNum].allowteams & ALLOW_AXIS_TEAM) && bot->client->sess.sessionTeam == TEAM_AXIS )
			{
				return ( 3 );
			}

			return ( 4 );
		}

		return ( 1 );
	}

	return ( 0 );
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Avoidance_Needed
// Description  : Returns the what is/isn't blocking us.
// Do we need to avoid something?
// 0 = Not blocked.
// 1 = Blocked by world.
// 2 = Blocked by player.
int
AIMOD_MOVEMENT_Avoidance_Needed ( gentity_t *bot )
{
	trace_t tr;
	vec3_t	newOrg;
	VectorCopy( nodes[bot->current_node].origin, newOrg );
	trap_Trace( &tr, bot->r.currentOrigin, NULL, NULL, newOrg, bot->s.number, MASK_PLAYERSOLID );
	if ( tr.fraction == 1 && tr.entityNum == ENTITYNUM_WORLD )
	{
		return ( 1 );
	}
	else if ( tr.fraction == 1 && g_entities[tr.entityNum].s.eType == ET_PLAYER )
	{
		return ( 2 );
	}

	return ( 0 );
}

extern vec3_t	botTraceMins;
extern vec3_t	botTraceMaxs;

//===========================================================================
// Routine      : AIMOD_MOVEMENT_ReachableBy
// Description  : Determine if a blocked goal vector is reachable.. Returns how to get there...
// Details:
//				-1 is Not reachable at all...
//				 0 is All clear to goal...
//				 1 is Jump...
//				 2 is Duck...
//				 3 is Strafe Left...
//				 4 is Strafe Right...
//
#define NOT_REACHABLE			- 1
#define REACHABLE				0
#define REACHABLE_JUMP			1
#define REACHABLE_DUCK			2
#define REACHABLE_STRAFE_LEFT	3
#define REACHABLE_STRAFE_RIGHT	4


/* */
int
AIMOD_MOVEMENT_ReachableBy ( gentity_t *bot, vec3_t goal )
{
	trace_t				trace;
	vec3_t /*v, v2,*/ eyes, mins, maxs, Org, forward, right, up, rightOrigin, leftOrigin;
	vec3_t			botCrouchTraceMaxs = { 20, 20, 16 };

	// First check direct movement...
	VectorCopy( bot->r.currentOrigin, Org );

	trap_Trace( &trace, Org, botTraceMins, botTraceMaxs, goal, bot->s.number, MASK_PLAYERSOLID/*MASK_SOLID | MASK_OPAQUE */);
	if ( trace.fraction == 1.0 )
	{
		return ( REACHABLE );				// Yes we can see it
	}

	// Now look for jump access...
	VectorAdd( Org, playerMins, mins );
	VectorAdd( Org, playerMaxs, maxs );

	VectorCopy( Org, eyes );
	eyes[2] += 32;//16//32;

	trap_Trace( &trace, eyes, botTraceMins, botTraceMaxs, goal, bot->s.number, MASK_PLAYERSOLID/*MASK_SOLID | MASK_OPAQUE */ );
	if ( trace.fraction == 1.0 )
	{
		return ( REACHABLE_JUMP );			// Yes we can see it
	}

	// Now look for crouch access...
	VectorCopy( Org, eyes );

	trap_Trace( &trace, eyes, botTraceMins, botCrouchTraceMaxs, goal, bot->s.number, MASK_PLAYERSOLID/*MASK_SOLID | MASK_OPAQUE */ );
	if ( trace.fraction == 1.0 )
	{
		return ( REACHABLE_DUCK );			// Yes we can see it
	}

	// Now look for strafe access... Left or Right...
#ifdef __NPC__
	if ( bot->NPC_client )
	{
		AngleVectors( bot->NPC_client->ps.viewangles, forward, right, up );
	}
	else
	{
		AngleVectors( bot->client->ps.viewangles, forward, right, up );
	}

#else //!__NPC__
	AngleVectors( bot->client->ps.viewangles, forward, right, up );
#endif //__NPC__

	VectorMA( bot->r.currentOrigin, 64, right, rightOrigin );
	VectorMA( bot->r.currentOrigin, -64, right, leftOrigin );
	if ( NodeVisible( leftOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_left_timer = level.time + 201;
		VectorCopy( leftOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( NodeVisible( rightOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_right_timer = level.time + 201;
		VectorCopy( rightOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	// And strafing a bit further away...
	VectorMA( bot->r.currentOrigin, 128, right, rightOrigin );
	VectorMA( bot->r.currentOrigin, -128, right, leftOrigin );
	if ( NodeVisible( leftOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_left_timer = level.time + 401;
		VectorCopy( leftOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( NodeVisible( rightOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_right_timer = level.time + 401;
		VectorCopy( rightOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	// And strafing a bit further away...
	VectorMA( bot->r.currentOrigin, 192, right, rightOrigin );
	VectorMA( bot->r.currentOrigin, -192, right, leftOrigin );
	if ( NodeVisible( leftOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_left_timer = level.time + 601;
		VectorCopy( leftOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( NodeVisible( rightOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_right_timer = level.time + 601;
		VectorCopy( rightOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	// And strafing a bit further away...
	VectorMA( bot->r.currentOrigin, 256, right, rightOrigin );
	VectorMA( bot->r.currentOrigin, -256, right, leftOrigin );
	if ( NodeVisible( leftOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_left_timer = level.time + 801;
		VectorCopy( leftOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( NodeVisible( rightOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_right_timer = level.time + 801;
		VectorCopy( rightOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	// And strafing a bit further away...
	VectorMA( bot->r.currentOrigin, 512, right, rightOrigin );
	VectorMA( bot->r.currentOrigin, -512, right, leftOrigin );
	if ( NodeVisible( leftOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_left_timer = level.time + 1601;
		VectorCopy( leftOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( NodeVisible( rightOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_right_timer = level.time + 1601;
		VectorCopy( rightOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	return ( NOT_REACHABLE );
}

extern void BOT_MakeLinkSemiUnreachable ( gentity_t *bot, int node, int linkNum );
extern void BOT_IncreaseLinkCost ( gentity_t *bot, int node, int linkNum );

void AIMOD_MOVEMENT_IncreaseLinkageCost ( gentity_t *bot )
{
	int loop;

	if (!bot || !bot->last_node || !bot->current_node)
		return;

	for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
	{			// Find the link we last used and mark it...
		if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
		{		// Mark it as Dangerous...
			BOT_IncreaseLinkCost( bot, bot->last_node, loop );
			break;
		}
	}
}

//===========================================================================
// Routine      : AIMOD_MOVEMENT_CurrentNodeReachable
// Description  : Quick vis check of our current node...
extern int		ReachableBox ( vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore );
extern void		BOT_MakeLinkDangerous ( gentity_t *bot, int node, int linkNum );
extern vec3_t	botTraceMins;
extern vec3_t	botTraceMaxs;
extern vec3_t	tankTraceMins;
extern vec3_t	tankTraceMaxs;


/* */
qboolean
AIMOD_MOVEMENT_CurrentNodeReachable ( gentity_t *bot )
{							// Quick vis check of our current node...
	vec3_t	bot_origin;
	int		reachable = 0;
	if ( bot->bot_last_reachable_check == bot->current_node )
	{
		return ( qtrue );
	}

	bot->bot_last_reachable_check = bot->current_node;
	if ( !bot->current_node || !bot->next_node || !bot->longTermGoal )
	{
		return ( qtrue );	// Ignore until we have a path...
	}

	VectorCopy( bot->r.currentOrigin, bot_origin );
	bot_origin[2] += 32/*4*/ /*16*/ ;

	// Trace to the next node..
#ifdef __VEHICLES__
	if ( bot->client && bot->client->ps.eFlags & EF_VEHICLE )
	{
		reachable = ReachableBox( bot_origin, tankTraceMins, tankTraceMaxs, nodes[bot->current_node].origin,
								  bot->s.number );
	}
	else
#endif //__VEHICLES__
		reachable = ReachableBox( bot_origin, botTraceMins, botTraceMaxs, nodes[bot->current_node].origin, bot->s.number );

	//if (reachable == 0 /* Blocked */ || reachable == 2 /* Lava/Water/Slime*/ )
	if ( /*reachable == 0 || */ reachable == 2 /* Lava/Water/Slime*/ )
	{
		if ( reachable == 2 /* Lava/Water/Slime*/ )
		{					// Dangerous...
			int loop = 0;
			for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
			{				// Find the link we last used and mark it...
				if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
				{			// Mark it as Dangerous...
					BOT_MakeLinkDangerous( bot, bot->last_node, loop );
					break;
				}
			}
		}
		else
		{					// Blocked...
			int			loop = 0;
			qboolean	onLadder = qfalse;
			if
			(
				bot->client &&
				(
					bot->client->ps.serverCursorHint == HINT_LADDER ||
					bot->client->ps.torsoAnim == BOTH_CLIMB ||
					bot->client->ps.legsAnim == BOTH_CLIMB
				)
			)
			{
				onLadder = qtrue;
			}

#ifdef __NPC__
			if
			(
				bot->NPC_client &&
				(
					bot->NPC_client->ps.serverCursorHint == HINT_LADDER ||
					bot->NPC_client->ps.torsoAnim == BOTH_CLIMB ||
					bot->NPC_client->ps.legsAnim == BOTH_CLIMB
				)
			)
			{
				onLadder = qtrue;
			}
#endif //__NPC__
			if ( !onLadder )
			{
				for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
				{			// Find the link we last used and mark it...
					if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
					{		// Mark it as Dangerous...
						BOT_MakeLinkSemiUnreachable( bot, bot->last_node, loop );
						break;
					}
				}
			}
		}

		return ( qfalse );
	}

	return ( qtrue );
}

/*
//===========================================================================
// Routine      : AIMOD_MOVEMENT_CanMove
// Description  : Checks if bot can move (really just checking ground)
//              : Not an accurate check, but does a decent enough job
//              : and looks for lava/slime.
qboolean AIMOD_MOVEMENT_CanMove(gentity_t *bot, int direction)
{
    vec3_t forward, right;
    vec3_t offset,start,end;
    vec3_t angles;
    trace_t tr;

    // Now check to see if move will move us off an edge
    //-------------------------------------------------------
    VectorCopy(bot->s.angles,angles);
    
    if(direction == BOT_MOVE_LEFT)
        angles[1] += 90;
    else if(direction == BOT_MOVE_RIGHT)
        angles[1] -= 90;
    else if(direction == BOT_MOVE_BACK)
        angles[1] -=180;
    //-------------------------------------------------------
    
    // Set up the vectors
    //-------------------------------------------------------
    AngleVectors (angles, forward, right, NULL);
    
    VectorSet(offset, 36, 0, 24);
    G_ProjectSource (bot->s.origin, offset, forward, right, start);
        
    VectorSet(offset, 36, 0, -100);
    G_ProjectSource (bot->s.origin, offset, forward, right, end);
    
    //tr = gi.trace(start, NULL, NULL, end, bot, MASK_SOLID|MASK_OPAQUE);
	trap_Trace(&tr,start, NULL, NULL, end, bot->s.number, MASK_SOLID|MASK_OPAQUE);
    
    if(tr.fraction == 1.0 || tr.contents & (CONTENTS_LAVA|CONTENTS_SLIME))
    {
        if(debug_mode)
            G_Printf("%s: move blocked\n",bot->client->pers.netname);
        return qfalse;   
    }
    
    // Can Move
    return qtrue;
}
*/


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Visible_For_Attacking

// Description  : Is our enemy attackable???
qboolean
AIMOD_MOVEMENT_Visible_For_Attacking ( gentity_t *self, gentity_t *other )
{
	return ( MyVisible( self, other) );
}

//===========================================================================
//
// Bot user command code...
//
//===========================================================================
//===========================================================================
// Routine      : G_AdjustforStrafe
// Description  : Adjusts the moveDir to account for strafing...

/*void G_AdjustforStrafe(gentity_t *bot, vec3_t moveDir)
{
	vec3_t right;

	if(!bot->strafeDir || bot->strafeTime < level.time )
	{//no strafe
		return;
	}

	AngleVectors(bot->client->ps.viewangles, NULL, right, NULL);

	//flaten up/down
	right[2] = 0;

	if(bot->strafeDir == 2)
	{//strafing left
		VectorScale(right, -1, right);
	}

	//We assume that moveDir has been normalized before this function.
	VectorAdd(moveDir, right, moveDir);
	VectorNormalize(moveDir);
}*/


//===========================================================================
// Routine      : G_UcmdMoveForDir

// Description  : Set a valid ucmd move for the current move direction...
void
G_UcmdMoveForDir ( gentity_t *self, usercmd_t *cmd, vec3_t dir )
{
	vec3_t	forward, right, up;
	float	speed = 127.0f;
	if ( cmd->buttons & BUTTON_WALKING )
	{
		speed = 64.0f;
	}

#ifdef __NPC__
	if ( self->NPC_client )
	{
		AngleVectors( self->NPC_client->ps.viewangles, forward, right, up );
	}
	else
	{
		AngleVectors( self->client->ps.viewangles, forward, right, up );
	}

#else //!__NPC__
	AngleVectors( self->client->ps.viewangles, forward, right, up );
#endif //__NPC__
	dir[2] = 0;
	VectorNormalize( dir );
	cmd->forwardmove = DotProduct( forward, dir ) * speed;
	cmd->rightmove = DotProduct( right, dir ) * speed;

	//cmd->upmove = abs(forward[3] ) * dir[3] * speed;
}


//===========================================================================
//
// Bot misc code...
//
//===========================================================================
//===========================================================================
// Routine      : AIMOD_HaveAmmoForWeapon

// Description  : Does the bot have any ammo for this weapon...
qboolean
AIMOD_HaveAmmoForWeapon ( playerState_t *ps, weapon_t wep )
{
	weapon_t	cur_wep = wep;

	if (wep == WP_KNIFE)
		return qtrue;
	
	if ( wep == WP_MOBILE_MG42_SET )
		cur_wep = WP_MOBILE_MG42;
#ifdef __EF__
	else if ( wep == WP_30CAL_SET )
		cur_wep = WP_30CAL;
#endif //__EF__
	else if (wep == WP_GARAND_SCOPE)
		cur_wep = WP_GARAND;
	else if (wep == WP_FG42SCOPE)
		cur_wep = WP_FG42;
	else if (wep == WP_K43_SCOPE)
		cur_wep = WP_K43;
	else if (wep == WP_SILENCED_LUGER)
		cur_wep = WP_LUGER;
	else if (wep == WP_SILENCED_COLT)
		cur_wep = WP_COLT;
	else if (wep == WP_AKIMBO_SILENCEDCOLT)
		cur_wep = WP_COLT;
	else if (wep == WP_AKIMBO_SILENCEDLUGER)
		cur_wep = WP_LUGER;
	else if (wep == WP_AKIMBO_COLT)
		cur_wep = WP_COLT;
	else if (wep == WP_AKIMBO_LUGER)
		cur_wep = WP_LUGER;
	else
		cur_wep = wep;

	if ( ps->ammoclip[BG_FindClipForWeapon(cur_wep)] <= 0 && ps->ammo[BG_FindAmmoForWeapon(cur_wep)] <= 0 )
	{
		return ( qfalse );
	}

	return ( qtrue );
}


//===========================================================================
// Routine      : AIMOD_NeedAmmoForWeapon

// Description  : Does the bot need any ammo for this weapon...
qboolean
AIMOD_NeedAmmoForWeapon ( playerState_t *ps, weapon_t wep )
{
	weapon_t	cur_wep = wep;
	int maxammo = 0;

	if ( wep == WP_MOBILE_MG42_SET )
		cur_wep = WP_MOBILE_MG42;
#ifdef __EF__
	else if ( wep == WP_30CAL_SET )
		cur_wep = WP_30CAL;
#endif //__EF__
	else if (wep == WP_GARAND_SCOPE)
		cur_wep = WP_GARAND;
	else if (wep == WP_FG42SCOPE)
		cur_wep = WP_FG42;
	else if (wep == WP_K43_SCOPE)
		cur_wep = WP_K43;
	else if (wep == WP_SILENCED_LUGER)
		cur_wep = WP_LUGER;
	else if (wep == WP_SILENCED_COLT)
		cur_wep = WP_COLT;
	else if (wep == WP_AKIMBO_SILENCEDCOLT)
		cur_wep = WP_COLT;
	else if (wep == WP_AKIMBO_SILENCEDLUGER)
		cur_wep = WP_LUGER;
	else if (wep == WP_AKIMBO_COLT)
		cur_wep = WP_COLT;
	else if (wep == WP_AKIMBO_LUGER)
		cur_wep = WP_LUGER;
	else
		cur_wep = wep;
	
	if ( cur_wep == WP_GRENADE_LAUNCHER || cur_wep == WP_GRENADE_PINEAPPLE )
	{
		maxammo = BG_GrenadesForClass( ps->stats[STAT_PLAYER_CLASS], g_entities[ps->clientNum].client->sess.skill );
	}
	else if ( wep == WP_GARAND_SCOPE )
	{
		maxammo = BG_MaxAmmoForWeapon( WP_GARAND, g_entities[ps->clientNum].client->sess.skill );
	}
	else if ( wep == WP_K43_SCOPE )
	{
		maxammo = BG_MaxAmmoForWeapon( WP_K43, g_entities[ps->clientNum].client->sess.skill );
	}
	else if ( wep == WP_FG42SCOPE )
	{
		maxammo = BG_MaxAmmoForWeapon( WP_FG42, g_entities[ps->clientNum].client->sess.skill );
	}

	//	else if (wep == WP_KAR98)
	//		maxammo = BG_MaxAmmoForWeapon( WP_GPG40, g_entities[ps->clientNum].client->sess.skill );
	//	else if (wep == WP_CARBINE)
	//		maxammo = BG_MaxAmmoForWeapon( WP_M7, g_entities[ps->clientNum].client->sess.skill );
	else if ( cur_wep == WP_MEDKIT || cur_wep == WP_AMMO || cur_wep == WP_PLIERS || cur_wep == WP_KNIFE )
	{
		return ( qfalse );
	}
	else
	{
		maxammo = BG_MaxAmmoForWeapon( cur_wep, g_entities[ps->clientNum].client->sess.skill );
	}

	if ( ps->ammo[BG_FindAmmoForWeapon(cur_wep)] <= maxammo * 0.75 /*0.25*/ )
	{
		return ( qtrue );
	}

	return ( qfalse );
}


//===========================================================================
// Routine      : AIMOD_InformTeamOfArty

// Description  : Inform nearby teammates of an arty strike area...
void
AIMOD_InformTeamOfArty ( gentity_t *bot )
{	// Test.. See if we can not vischek as often by assigning enemies in a fast way...
	int loop;
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*ent = &g_entities[loop];
		if ( !ent )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}

		if ( ent->health > 0 )
		{
			continue;
		}

		if ( !OnSameTeam( bot, ent) )
		{
			continue;
		}

		if ( VectorDistance( bot->enemy->r.currentOrigin, ent->r.currentOrigin) > 256 )
		{
			continue;
		}

		ent->bot_runaway_time = level.time + 20000;
	}
}


//===========================================================================
// Routine      : AIMOD_InformTeamOfAirstrike

// Description  : Inform nearby teammates of an airstrike area...
void
AIMOD_InformTeamOfAirstrike ( gentity_t *bot )
{	// Test.. See if we can not vischek as often by assigning enemies in a fast way...
	int loop;
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*ent = &g_entities[loop];
		if ( !ent )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}

		if ( ent->health > 0 )
		{
			continue;
		}

		if ( !OnSameTeam( bot, ent) )
		{
			continue;
		}

		if ( VectorDistance( bot->enemy->r.currentOrigin, ent->r.currentOrigin) > 256 )
		{
			continue;
		}

		ent->bot_runaway_time = level.time + 12000;
	}
}


//===========================================================================
// Routine      : AIMOD_InformTeamOfGrenade

// Description  : Inform nearby teammates of a grenade...
void
AIMOD_InformTeamOfGrenade ( gentity_t *bot )
{	// Test.. See if we can not vischek as often by assigning enemies in a fast way...
	int loop;
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*ent = &g_entities[loop];
		if ( !ent )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}

		if ( ent->health > 0 )
		{
			continue;
		}

		if ( !OnSameTeam( bot, ent) )
		{
			continue;
		}

		if ( VectorDistance( bot->enemy->r.currentOrigin, ent->r.currentOrigin) > 256 )
		{
			continue;
		}

		ent->bot_runaway_time = level.time + 6000;
	}
}


//===========================================================================
// Routine      : AIMOD_InformTeamOfEnemy

// Description  : See if we can not vischek as often by assigning enemies in a fast way...
void
AIMOD_InformTeamOfEnemy ( gentity_t *bot )
{	// Test.. See if we can not vischek as often by assigning enemies in a fast way...
	/*	int loop;

	for (loop = 0;loop < MAX_CLIENTS;loop++)
	{
		gentity_t *ent = &g_entities[loop];

		if (!ent)
			continue;

		if (!ent->client)
			continue;

		if (ent->health > 0)
			continue;

		if (!OnSameTeam(bot, ent))
			continue;

		if (ent->client->sess.playerType == PC_SOLDIER)
			continue; // Engineers shouldn't share enemies, they have work to do.

		if (VectorDistance(bot->r.currentOrigin, ent->r.currentOrigin) > 512)
			continue;

		if (!ent->enemy 
			|| VectorDistance(ent->r.currentOrigin, ent->enemy->r.currentOrigin) < VectorDistance(ent->r.currentOrigin, bot->enemy->r.currentOrigin))
		continue;

		if (!MyVisible(ent, bot->enemy))
			continue;

		// OK.. Set their enemy to ours!
		ent->enemy = bot->enemy;
		ent->bot_enemy_visible_time = level.time + 100;
	}
*/
}


//===========================================================================
// Routine      : Is_Medic_Close

// Description  : Is there a medic close to us???
qboolean
Is_Medic_Close ( int targetNum )
{
	int			j;

	//qboolean ok = qfalse;
	gentity_t	*bot = &g_entities[targetNum];
	for ( j = 0; j < MAX_CLIENTS; j++ )
	{
		gentity_t	*test = &g_entities[j];
		if ( !test )
		{
			continue;
		}

		if ( !test->client )
		{
			continue;
		}

		if ( test->health <= 0 )
		{
			continue;
		}

		if ( test->health <= 0 )
		{
			continue;
		}

		if ( test->client->sess.sessionTeam == TEAM_SPECTATOR || test->client->ps.pm_flags & PMF_LIMBO )
		{
			continue;
		}

		if ( !OnSameTeam( test, bot) )
		{
			continue;
		}

		if ( test->client->sess.playerType != PC_MEDIC )
		{
			continue;
		}

		if ( VectorDistance( test->r.currentOrigin, bot->r.currentOrigin) > 1024 )
		{
			continue;
		}

		// Found a medic close by!
		return ( qtrue );
	}

	// None found...
	return ( qfalse );
}


//===========================================================================
//
// Bot view angles code...
//
//===========================================================================
//===========================================================================
// Routine      : BotSetViewAngle

// Description  : Set bot view angles over time... Called from BotSetViewAngles (below)...
float
BotSetViewAngle ( float angle, float ideal_angle, float speed )
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


//===========================================================================
// Routine      : BotSetViewAngles

// Description  : Set bot view angles over time...
void
BotSetViewAngles ( gentity_t *bot, float thinktime )
{	// Just keep bot->bot_ideal_view_angles correct and call this to make it realistic...
	float	diff, factor, maxchange, anglespeed, fulldiff;
	int		i;
#ifdef __NPC__
	if ( bot->NPC_client )
	{
		fulldiff = fabs( AngleDifference( bot->NPC_client->ps.viewangles[0], bot->bot_ideal_view_angles[0]) );
		fulldiff += fabs( AngleDifference( bot->NPC_client->ps.viewangles[1], bot->bot_ideal_view_angles[1]) );
		fulldiff += fabs( AngleDifference( bot->NPC_client->ps.viewangles[2], bot->bot_ideal_view_angles[2]) );
	}
	else
	{
		fulldiff = fabs( AngleDifference( bot->client->ps.viewangles[0], bot->bot_ideal_view_angles[0]) );
		fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[1], bot->bot_ideal_view_angles[1]) );
		fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[2], bot->bot_ideal_view_angles[2]) );
	}

#else //!__NPC__
	fulldiff = fabs( AngleDifference( bot->client->ps.viewangles[0], bot->bot_ideal_view_angles[0]) );
	fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[1], bot->bot_ideal_view_angles[1]) );
	fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[2], bot->bot_ideal_view_angles[2]) );
#endif //__NPC__
	thinktime = 50 * fulldiff;

	//	if (fulldiff >= 180)
	//		bot->bot_turn_wait_time = level.time + thinktime;
	if ( bot->bot_ideal_view_angles[PITCH] > 180 )
	{
		bot->bot_ideal_view_angles[PITCH] -= 360;
	}

	//
	factor = 0.15;
	maxchange = 240;
	maxchange *= thinktime;
	for ( i = 0; i < 2; i++ )
	{
#ifdef __NPC__
		if ( bot->NPC_client )
		{
			diff = fabs( AngleDifference( bot->NPC_client->ps.viewangles[i], bot->bot_ideal_view_angles[i]) );
			anglespeed = diff * factor;
			if ( anglespeed > maxchange )
			{
				anglespeed = maxchange;
			}

			bot->NPC_client->ps.viewangles[i] = BotSetViewAngle( bot->NPC_client->ps.viewangles[i],
																 bot->bot_ideal_view_angles[i], anglespeed );
		}
		else
		{
			diff = fabs( AngleDifference( bot->client->ps.viewangles[i], bot->bot_ideal_view_angles[i]) );
			anglespeed = diff * factor;
			if ( anglespeed > maxchange )
			{
				anglespeed = maxchange;
			}

			bot->client->ps.viewangles[i] = BotSetViewAngle( bot->client->ps.viewangles[i],
															 bot->bot_ideal_view_angles[i], anglespeed );
		}

#else //!__NPC__
		diff = fabs( AngleDifference( bot->client->ps.viewangles[i], bot->bot_ideal_view_angles[i]) );
		anglespeed = diff * factor;
		if ( anglespeed > maxchange )
		{
			anglespeed = maxchange;
		}

		bot->client->ps.viewangles[i] = BotSetViewAngle( bot->client->ps.viewangles[i], bot->bot_ideal_view_angles[i],
														 anglespeed );
#endif //__NPC__
	}

#ifdef __NPC__
	if ( bot->NPC_client )
	{
		if ( bot->NPC_client->ps.viewangles[PITCH] > 180 )
		{
			bot->NPC_client->ps.viewangles[PITCH] -= 360;
		}

		VectorCopy( bot->NPC_client->ps.viewangles, bot->s.angles );
	}
	else
	{
		if ( bot->client->ps.viewangles[PITCH] > 180 )
		{
			bot->client->ps.viewangles[PITCH] -= 360;
		}

		VectorCopy( bot->client->ps.viewangles, bot->s.angles );
	}

#else //!__NPC__
	if ( bot->client->ps.viewangles[PITCH] > 180 )
	{
		bot->client->ps.viewangles[PITCH] -= 360;
	}

	//trap_EA_View(bot->s.number, bot->client->ps.viewangles);
	VectorCopy( bot->client->ps.viewangles, bot->s.angles );
#endif //__NPC__
}


//===========================================================================
//
// Bot nodes and generic movement code...
//
//===========================================================================

//===========================================================================
// Routine      : AIMOD_MOVEMENT_Fallback

// Description  : Fallback navigation. For emergency routing...
qboolean
AIMOD_MOVEMENT_Fallback ( gentity_t *bot, usercmd_t *ucmd )
{															// Fallback navigation. returns if it was used or not as qtrue/qfalse.
	if ( NodesLoaded() == qfalse /*|| bot_forced_auto_waypoint.integer*/ )
	{														// Random for unrouted maps.
		int		vis;
		vec3_t	forward, forward_pos;
		AngleVectors( bot->s.angles, forward, NULL, NULL );
		VectorCopy(bot->r.currentOrigin, forward_pos);
		VectorMA( forward_pos, 96, forward, forward_pos );
		vis = NodeVisible( bot->r.currentOrigin, forward_pos, bot->s.number );

		//0 = wall in way
		//1 = player or no obstruction
		//2 = useable door in the way.
		//3 = door entity in the way.
		if ( bot->client )
		{
			if ( vis != 1 && vis != 2 )
			{														// We havn't moved at all... Let's randomize things.
				vec3_t	temp;

				if ( bot->last_use_time < level.time && no_mount_time[bot->s.clientNum] < level.time && bot->client->sess.playerType != PC_ENGINEER )
				{
					Cmd_Activate_f( bot );
				}

				VectorCopy( bot->s.angles, temp );
				temp[0] = bot->s.angles[0] = 0;
				temp[2] = bot->s.angles[2] = 0;
				temp[1] = Q_TrueRand( 0, 360 );

				bot->client->AImovetime = level.time + 2000;		// 10 seconds?
				VectorCopy( temp, bot->s.angles );
				//ucmd->forwardmove = 127;
			}
			else
			{
				ucmd->forwardmove = 127;

				if ( VectorLength(bot->client->ps.velocity) < 24 && Q_TrueRand(0,10) < 3 )
				{
					ucmd->upmove = 127;									// If we are moving too slowly, sometimes jump (obstacles)
					//G_Printf("Jump type 5\n");
				}
			}

			if ( bot->waterlevel )
			{
				ucmd->upmove = 127;									// Try to always be on surface...
			}
		}

#ifdef __NPC__
		else if ( bot->NPC_client )
		{
			//if (VectorLength(bot->NPC_client->ps.velocity) < 64 )
			if ( vis != 1 && vis != 2 )
			{														// We havn't moved at all... Let's randomize things.
				vec3_t	temp;

				VectorCopy( bot->s.angles, temp );
				temp[0] = bot->s.angles[0] = 0;
				temp[2] = bot->s.angles[2] = 0;
				temp[1] = Q_TrueRand( 0, 360 );
				bot->NPC_client->AImovetime = level.time + 2000;	// 10 seconds?
				VectorCopy( temp, bot->s.angles );
				ucmd->forwardmove = 120;
			}
			else
			{
				ucmd->forwardmove = 120;
			}

			if ( bot->waterlevel )
			{
				ucmd->upmove = 120;									// Try to always be on surface...
			}
		}
#endif //__NPC__

		return ( qtrue );
	}

	return ( qfalse );
}

//===========================================================================
// Routine      : AIMOD_MOVEMENT_Backup

// Description  : Fallback navigation. For emergency routing...
void
AIMOD_MOVEMENT_Backup ( gentity_t *bot, usercmd_t *ucmd )
{																// Fallback navigation. returns if it was used or not as qtrue/qfalse.
	int		vis;
	vec3_t	forward, forward_pos;
	AngleVectors( bot->s.angles, forward, NULL, NULL );
	VectorCopy(bot->r.currentOrigin, forward_pos);
	VectorMA( forward_pos, 96, forward, forward_pos );
	vis = NodeVisible( bot->r.currentOrigin, forward_pos, bot->s.number );

	//0 = wall in way
	//1 = player or no obstruction
	//2 = useable door in the way.
	//3 = door entity in the way.
	if ( bot->client )
	{
		if ( vis != 1 && vis != 2 )
		{														// We havn't moved at all... Let's randomize things.
			vec3_t	temp;

			if ( bot->last_use_time < level.time && no_mount_time[bot->s.clientNum] < level.time && bot->client->sess.playerType != PC_ENGINEER )
			{
				Cmd_Activate_f( bot );
			}

			VectorCopy( bot->s.angles, temp );
			temp[0] = bot->s.angles[0] = 0;
			temp[2] = bot->s.angles[2] = 0;
			temp[1] = Q_TrueRand( 0, 360 );

			bot->client->AImovetime = level.time + 2000;		// 10 seconds?
			VectorCopy( temp, bot->s.angles );
			//ucmd->forwardmove = 127;
		}
		else
		{
			ucmd->forwardmove = 127;

			if ( VectorLength(bot->client->ps.velocity) < 24 && Q_TrueRand(0,10) < 3 )
			{
				ucmd->upmove = 127;									// If we are moving too slowly, sometimes jump (obstacles)
				//G_Printf("Jump type 6\n");
			}
		}

		if ( bot->waterlevel )
		{
			ucmd->upmove = 127;									// Try to always be on surface...
		}
	}
}

//===========================================================================
// Routine      : AIMOD_MOVEMENT_Backup_Pick_New_Destination

// Description  : Single Player style AI Destination Selection
qboolean
AIMOD_MOVEMENT_Backup_Pick_New_Destination ( gentity_t *bot, usercmd_t *ucmd )
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

		VectorCopy(bot->r.currentOrigin, bot->bot_sp_destination);
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
// Routine      : AIMOD_MOVEMENT_Backup

// Description  : Single Player style AI Movement
void
AIMOD_MOVEMENT_Backup2 ( gentity_t *bot, usercmd_t *ucmd )
{
	if ( !bot->bot_initialized )
	{// Initialization...
		G_SetOrigin(bot, bot->r.currentOrigin);
		bot->bot_initialized = qtrue;

		// Initialize move destination...
		VectorCopy(bot->r.currentOrigin, bot->bot_sp_destination);
	}

	if (bot->enemy)
	{// If this bot has an enemy, check cover spots!
		if ( Have_Close_Enemy( bot) &&
			bot->bot_enemy_visible_time > level.time &&
			bot->enemy == bot->bot_last_visible_enemy &&
			bot->bot_runaway_time < level.time
			&& AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ))
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
			if (number_of_nodes > 0 && bot->current_node)
			{
				//AIMOD_MOVEMENT_AttackMove( bot, ucmd );
				AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			}
			else
			{
				//AIMOD_MOVEMENT_Attack( bot, ucmd );
				AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			}
#endif //__OLD_COVER_SPOTS__
			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
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
			if (number_of_nodes > 0 && bot->current_node)
			{
				//AIMOD_MOVEMENT_AttackMove( bot, ucmd );
				AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			}
			else
			{
				//AIMOD_MOVEMENT_Attack( bot, ucmd );
				AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			}
#endif //__OLD_COVER_SPOTS__
			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			return;
		}
	}

	// Actual movement here...
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
		else if (!AIMOD_MOVEMENT_Backup_Pick_New_Destination( bot, ucmd ))
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
			if ( bot->last_use_time < level.time && bot->client->sess.playerType != PC_ENGINEER )
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
		else if (!AIMOD_MOVEMENT_Backup_Pick_New_Destination( bot, ucmd ))
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
		else if (!AIMOD_MOVEMENT_Backup_Pick_New_Destination( bot, ucmd ))
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
}


//#ifndef __BOT_PERFECTION__
//===========================================================================
// Routine      : AIMOD_MOVEMENT_From_Void

// Description  : Fallback navigation. For emergency routing...
void
AIMOD_MOVEMENT_From_Void ( gentity_t *bot, usercmd_t *ucmd )
{														// Forced fallback navigation (out of voids)...
	float	diff;
	vec3_t	fwdOrg;
	vec3_t	fwd;
	diff = fabs( AngleDifference( bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]) );
	if ( diff > 90.0f )
	{													// Delay large turns...
		BotSetViewAngles( bot, 100 );
		ucmd->forwardmove = 0;
		return;
	}

	if ( VectorLength( bot->client->ps.velocity) < 64 )
	{													// We havn't moved at all... Let's randomize things.
		vec3_t	temp;
		if ( bot->last_use_time < level.time && no_mount_time[bot->s.clientNum] < level.time && bot->client->sess.playerType != PC_ENGINEER )
		{
			Cmd_Activate_f( bot );
		}

		VectorCopy( bot->s.angles, temp );
		temp[0] = bot->s.angles[0];
		temp[2] = bot->s.angles[2];
		temp[1] = bot->s.angles[1] + Q_TrueRand( 0, 360 );
		bot->client->AImovetime = level.time + 10000;	// 10 seconds?

		// Set up ideal view angles for this node...
		VectorCopy( temp, bot->bot_ideal_view_angles );
		bot->bot_ideal_view_angles[2] = 0;				// Always look straight.. Don't look up or down at nodes...
		diff = fabs( AngleDifference( bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]) );
		if ( diff > 90.0f )
		{												// Delay large turns...
			BotSetViewAngles( bot, 100 );
			ucmd->forwardmove = 0;
		}
		else
		{
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
			VectorCopy( bot->r.currentOrigin, last_vector[bot->s.clientNum] );
			AngleVectors( bot->s.angles, fwd, NULL, NULL );
			VectorCopy( bot->r.currentOrigin, fwdOrg );
			fwdOrg[0] += ( fwd[0] * 64 );
			fwdOrg[1] += ( fwd[1] * 64 );
			fwdOrg[2] += ( fwd[2] * 64 );
			if ( NodeVisible( bot->r.currentOrigin, fwdOrg, bot->s.number) >= 1 )
			{
				if ( ucmd->buttons & BUTTON_WALKING )
				{
					ucmd->forwardmove = 64;
				}
				else
				{
					ucmd->forwardmove = 127;
				}
			}
		}
	}
	else
	{
		if ( bot->last_use_time < level.time && no_mount_time[bot->s.clientNum] < level.time && bot->client->sess.playerType != PC_ENGINEER )
		{
			Cmd_Activate_f( bot );
		}

		diff = fabs( AngleDifference( bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]) );
		if ( diff > 90.0f )
		{												// Delay large turns...
			BotSetViewAngles( bot, 100 );
			ucmd->forwardmove = 0;
		}
		else
		{
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
			VectorCopy( bot->r.currentOrigin, last_vector[bot->s.clientNum] );
			AngleVectors( bot->s.angles, fwd, NULL, NULL );
			VectorCopy( bot->r.currentOrigin, fwdOrg );
			fwdOrg[0] += ( fwd[0] * 64 );
			fwdOrg[1] += ( fwd[1] * 64 );
			fwdOrg[2] += ( fwd[2] * 64 );
			if ( NodeVisible( bot->r.currentOrigin, fwdOrg, bot->s.number) >= 1 )
			{
				if ( ucmd->buttons & BUTTON_WALKING )
				{
					ucmd->forwardmove = 64;
				}
				else
				{
					ucmd->forwardmove = 127;
				}
			}
		}
	}

	if ( bot->waterlevel )
	{
		ucmd->upmove = 120;								// Try to always be on surface...
	}
}


//#endif
//===========================================================================
// Routine      : AIMOD_MOVEMENT_Fallback2

// Description  : Fallback navigation. For emergency routing...
void
AIMOD_MOVEMENT_Fallback2 ( gentity_t *bot, usercmd_t *ucmd )
{														// Fallback navigation. For emergency routing...
	vec3_t	fwdOrg;
	vec3_t	fwd;
	if ( VectorLength( bot->client->ps.velocity) < 64 )
	{													// We havn't moved at all... Let's randomize things.
		vec3_t	temp;
		if ( bot->last_use_time < level.time && no_mount_time[bot->s.clientNum] < level.time && bot->client->sess.playerType != PC_ENGINEER )
		{
			Cmd_Activate_f( bot );
		}

		VectorCopy( bot->s.angles, temp );
		temp[0] = bot->s.angles[0];
		temp[2] = bot->s.angles[2];
		temp[1] = bot->s.angles[1] - rand() % 360;
		bot->client->AImovetime = level.time + 10000;	// 10 seconds?
		VectorCopy( temp, bot->s.angles );
		AngleVectors( bot->s.angles, fwd, NULL, NULL );
		VectorCopy( bot->r.currentOrigin, fwdOrg );
		fwdOrg[0] += ( fwd[0] * 64 );
		fwdOrg[1] += ( fwd[1] * 64 );
		fwdOrg[2] += ( fwd[2] * 64 );
		if ( NodeVisible( bot->r.currentOrigin, fwdOrg, bot->s.number) >= 1 )
		{
			ucmd->forwardmove = 120;
		}
	}
	else
	{
		AngleVectors( bot->s.angles, fwd, NULL, NULL );
		VectorCopy( bot->r.currentOrigin, fwdOrg );
		fwdOrg[0] += ( fwd[0] * 64 );
		fwdOrg[1] += ( fwd[1] * 64 );
		fwdOrg[2] += ( fwd[2] * 64 );
		if ( NodeVisible( bot->r.currentOrigin, fwdOrg, bot->s.number) >= 1 )
		{
			ucmd->forwardmove = 120;
		}
	}

	if ( bot->waterlevel )
	{
		ucmd->upmove = 120;								// Try to always be on surface...
	}
}

/*
//===========================================================================
// Routine      : fast_find_runaway
// Description  : Find a node to run away to...
int fast_find_runaway ( vec3_t origin )
{
	int i = 0;

	for (i = 0;i < number_of_nodes;i+=rand()%4)
	{
		float dist = VectorDistance(nodes[i].origin, origin);
		
		if (dist > 800 && dist < 1000)
			break;
	}

	if (i >= number_of_nodes)
		return -1;

	return i;
}*/


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Defender_Find_Sniper_Spot

// Description  : Find a sniper position (defender version).
int
AIMOD_MOVEMENT_Defender_Find_Sniper_Spot ( gentity_t *bot )
{

	/*	int entNum = BotMP_Defender_Find_POP_Goal_EntityNum( bot, bot->current_target_entity, bot->current_target_entity, bot->r.currentOrigin, bot->client->sess.sessionTeam );
	gentity_t *ent = &g_entities[entNum];
	int best_point = -1;
	int second_best_point = -1;
	int third_best_point = -1;
	int loop = 0;
	float best_dist = 99999.9f;

	if (ent)
	{// We have a goal to defend near... Find a spot close to it...
		for (loop = 0; loop < num_snipe_points; loop++)
		{
			if (VectorDistance(snipe_points[best_point], ent->r.currentOrigin) < best_dist)
			{
				third_best_point = second_best_point;
				second_best_point = best_point;
				best_point = loop;
				best_dist = VectorDistance(snipe_points[best_point], bot->r.currentOrigin);
			}
		}
	}
	else
	{
		for (loop = 0; loop < num_snipe_points; loop++)
		{
			if (VectorDistance(snipe_points[best_point], bot->r.currentOrigin) < best_dist)
			{
				third_best_point = second_best_point;
				second_best_point = best_point;
				best_point = loop;
				best_dist = VectorDistance(snipe_points[best_point], bot->r.currentOrigin);
			}
		}
	}

	// Return what we found...
	if (third_best_point >= 0)
	{
		if (Q_TrueRand(0,2) == 2)
		{
			return third_best_point;
		}
		else if (Q_TrueRand(0,2) == 2)
		{
			return second_best_point;
		}
		else
		{
			return best_point;
		}
	}
	else if (second_best_point >= 0)
	{
		if (Q_TrueRand(0,1) == 1)
		{
			return second_best_point;
		}
		else
		{
			return best_point;
		}
	}
	else
	{
		return best_point;
	}*/
	if ( num_snipe_points >= 1 )
	{
		return ( Q_TrueRand( 0, num_snipe_points - 1) );
	}
	else
	{
		return ( -1 );
	}
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Find_Sniper_Spot

// Description  : Find a sniper position.
int
AIMOD_MOVEMENT_Find_Sniper_Spot ( gentity_t *bot )
{

	/*	int best_point = -1;
	int loop = 0;
	float best_height = -64000.0f;
	int entNum = -1;
	int best_target = -1;
	float best_dist = 99999.9f;

	if (bot->bot_snipe_node && bot->bot_snipe_node > 0 && VectorDistance(nodes[bot->bot_snipe_node].origin, bot->r.currentOrigin) < 1024)
	{// Keep heading to our current snipe node!
		VectorCopy(nodes[bot->bot_snipe_node].origin, bot->bot_sniper_spot);

		// Set up this single node in long term goal array...
		memset(bot->longTermGoalNodes, NODE_INVALID, 32);
		bot->longTermGoalNodes[0] = bot->bot_snipe_node;
		bot->longTermGoalNodes_total = 1;

		return bot->bot_snipe_node;
	}

	for (loop = 0; loop < num_snipe_points; loop++)
	{
		if (VectorDistance(snipe_points[best_point], bot->r.currentOrigin) < best_dist 
			&& Q_TrueRand(0,2) == 1 ) // Mix it up a little...
		{
			best_dist = VectorDistance(snipe_points[best_point], bot->r.currentOrigin);
			best_point = loop;
		}
	}

	if (best_point < 0)
	{
		for (loop = 0; loop < num_snipe_points; loop++)
		{// Force it this time to be the very closest...
			if (VectorDistance(snipe_points[best_point], bot->r.currentOrigin) < best_dist )
			{
				best_dist = VectorDistance(snipe_points[best_point], bot->r.currentOrigin);
				best_point = loop;
			}
		}
	}

	for (loop = 0; loop < number_of_nodes; loop++)
	{// Find closest node(s) to the objective...
		float dist = VectorDistance(nodes[loop].origin, snipe_points[best_point]);

		if ( dist < best_dist )
		{
			memset(bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES);

			best_dist = VectorDistance(nodes[loop].origin, snipe_points[best_point]);
			best_target = loop;
		}
	}

	if (best_target > 0) // We found a snipe point...
	{
		//G_Printf("^3BOT DEBUG^5: ^7%s^5 found a snipe node (#^7%i^5).\n", bot->client->pers.netname, best_target);
		VectorCopy(nodes[best_target].origin, bot->bot_sniper_spot);
		//bot->bot_snipe_node = best_target;
		//memset(bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES);

		// Set up this single node in long term goal array...
		//memset(bot->longTermGoalNodes, NODE_INVALID, 32);
		//bot->longTermGoalNodes[0] = best_target;
		//bot->longTermGoalNodes_total = 1;

		return best_target;
	}

	// Didn't find one!
	return AIMOD_MOVEMENT_Defender_Find_Sniper_Spot( bot );*/
	if ( num_snipe_points >= 1 )
	{
		return ( Q_TrueRand( 0, num_snipe_points - 1) );
	}
	else
	{
		return ( -1 );
	}
}

//===========================================================================
// Routine      : Unique_FindGoal
// Description  : Find a new goal entity and node.
extern int	AIMOD_NAVIGATION_FindAbsoluteClosestReachableNode ( gentity_t *bot, int r, int type );
extern int	BotMP_Sniper_Find_Goal_EntityNum
			(
				gentity_t	*bot,
				int			ignoreEnt,
				int			ignoreEnt2,
				vec3_t		current_org,
				int			teamNum
			);


/* */
int
Unique_FindGoal ( gentity_t *bot )
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
	if ( bot->bot_defender )
	{						// Bot Objective Defenders goal finding...
#ifndef __BOT_NO_SNIPE_NODES__
		if (BG_IsScopedWeapon( bot->s.weapon) || BG_IsScopedWeapon2( bot->s.weapon ) || bot->s.weapon == WP_PANZERFAUST
			/*bot->s.weapon == WP_FG42 ||
			bot->s.weapon == WP_K43 ||
			bot->s.weapon == WP_GARAND ||
			bot->s.weapon == WP_PANZERFAUST ||
			bot->s.weapon == WP_FG42SCOPE ||
			bot->s.weapon == WP_K43_SCOPE ||
			bot->s.weapon == WP_GARAND_SCOPE*/
		)
		{
			entNum = BotMP_Sniper_Find_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
													   bot->client->sess.sessionTeam );
		}

		if ( !entNum )
#endif //__BOT_NO_SNIPE_NODES__

			// Find a standard defence point...
			entNum = BotMP_Defender_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity,
															 bot->r.currentOrigin, bot->client->sess.sessionTeam );
		if ( entNum >= 0 )
		{
			if (g_entities[entNum].num_in_range_nodes > 0)
				best_target = g_entities[entNum].in_range_nodes[0];
			else
				best_target = AIMOD_NAVIGATION_FindClosestReachableNode( &g_entities[entNum], NODE_DENSITY, NODEFIND_ALL,
																	 bot );
		}

		if ( best_target >= 0 )
		{					// Found a goal.. Use it...
			bot->current_target_entity = entNum;
			bot->goalentity = &g_entities[entNum];

			//			G_Printf("%s: found a goal for entity %i (best_target %i)! (current node %i)\n", bot->client->pers.netname, bot->current_node, entNum, best_target);
			// Return the best choice...
			return ( best_target );
		}

		//G_Printf("%s: failed to find a goal for entity %i! (current node %i)\n", bot->client->pers.netname, bot->current_node, entNum);
		// If we didn't find a goal, return -1.
		//return ( -1 );
		//return ( bot->current_node );
	}

	if ( bot->followtarget )
	{
		entNum = bot->followtarget->s.number;
	}
	else if ( bot->movetarget )
	{
		entNum = bot->movetarget->s.number;
	}
	else if ( bot->enemy )
	{
		entNum = bot->enemy->s.number;
	}

#ifdef __NPC__
	if (BG_IsScopedWeapon( bot->s.weapon) || BG_IsScopedWeapon2( bot->s.weapon ) || bot->s.weapon == WP_PANZERFAUST
			/*bot->s.weapon == WP_FG42 ||
			bot->s.weapon == WP_K43 ||
			bot->s.weapon == WP_GARAND ||
			bot->s.weapon == WP_PANZERFAUST ||
			bot->s.weapon == WP_FG42SCOPE ||
			bot->s.weapon == WP_K43_SCOPE ||
			bot->s.weapon == WP_GARAND_SCOPE*/
		)
	{
		entNum = BotMP_Sniper_Find_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												   bot->client->sess.sessionTeam );
	}
	/*else if
		(
			bot->NPC_client &&
			(
				BG_IsScopedWeapon( bot->NPC_client->ps.weapon) ||
				bot->NPC_client->ps.weapon == WP_FG42 ||
				bot->NPC_client->ps.weapon == WP_K43 ||
				bot->NPC_client->ps.weapon == WP_GARAND ||
				bot->NPC_client->ps.weapon == WP_PANZERFAUST ||
				bot->NPC_client->ps.weapon == WP_FG42SCOPE ||
				bot->NPC_client->ps.weapon == WP_K43_SCOPE ||
				bot->NPC_client->ps.weapon == WP_GARAND_SCOPE
			)
		)
	{
		entNum = BotMP_Sniper_Find_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												   bot->NPC_client->sess.sessionTeam );
	}*/
	else if ( bot->client && bot->client->sess.playerType == PC_ENGINEER )
	{
		entNum = BotMP_Engineer_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->client->sess.sessionTeam );
	}
	else if ( bot->NPC_client && bot->NPC_client->sess.playerType == PC_ENGINEER )
	{
		entNum = BotMP_Engineer_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->NPC_client->sess.sessionTeam );
	}
	else if
		(
			bot->client &&
			(bot->client->sess.playerType == PC_SOLDIER || bot->client->sess.playerType == PC_COVERTOPS)
		)
	{
		entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														bot->client->sess.sessionTeam );
	}
	else if
		(
			bot->NPC_client &&
			(bot->NPC_client->sess.playerType == PC_SOLDIER || bot->NPC_client->sess.playerType == PC_COVERTOPS)
		)
	{
		entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														bot->NPC_client->sess.sessionTeam );
	}
	else if ( bot->client && bot->client->sess.playerType == PC_FIELDOPS )
	{
		entNum = BotMP_FieldOps_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->client->sess.sessionTeam );
	}
	else if ( bot->NPC_client && bot->NPC_client->sess.playerType == PC_FIELDOPS )
	{
		entNum = BotMP_FieldOps_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->NPC_client->sess.sessionTeam );
	}
	else if ( bot->client && bot->client->sess.playerType == PC_MEDIC )
#ifdef __BOT_AAS__
		entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bot->current_target_entity, bot->r.currentOrigin,
													  bot->client->sess.sessionTeam, &botstates[bot->s.number] );
#else //!__BOT_AAS__
	entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bot->current_target_entity, bot->r.currentOrigin,
												  bot->client->sess.sessionTeam, bot );
#endif //__BOT_AAS__

	//	else if (bot->NPC_client && bot->NPC_client->sess.playerType == PC_MEDIC)
	//		entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bot->current_target_entity, bot->r.currentOrigin, bot->NPC_client->sess.sessionTeam, bot );
	else if ( bot->NPC_client )
	{
		entNum = BotMP_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												bot->NPC_client->sess.sessionTeam );
	}
	else
	{
		entNum = BotMP_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												bot->client->sess.sessionTeam );
	}

#else //!__NPC__
	if
	(
		BG_IsScopedWeapon( bot->client->ps.weapon) ||
		bot->client->ps.weapon == WP_FG42 ||
		bot->client->ps.weapon == WP_K43 ||
		bot->client->ps.weapon == WP_GARAND ||
		bot->client->ps.weapon == WP_PANZERFAUST ||
		bot->client->ps.weapon == WP_FG42SCOPE ||
		bot->client->ps.weapon == WP_K43_SCOPE ||
		bot->client->ps.weapon == WP_GARAND_SCOPE
	)
	{
		entNum = BotMP_Sniper_Find_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												   bot->client->sess.sessionTeam );
	}
	else if ( bot->client->sess.playerType == PC_ENGINEER )
	{
		entNum = BotMP_Engineer_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->client->sess.sessionTeam );
	}
	else if ( bot->client->sess.playerType == PC_SOLDIER || bot->client->sess.playerType == PC_COVERTOPS )
	{
		entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														bot->client->sess.sessionTeam );
	}
	else if ( bot->client->sess.playerType == PC_FIELDOPS )
	{
		entNum = BotMP_FieldOps_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->client->sess.sessionTeam );
	}
	else if ( bot->client->sess.playerType == PC_MEDIC )
#ifdef __BOT_AAS__
		entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bot->current_target_entity, bot->r.currentOrigin,
													  bot->client->sess.sessionTeam, &botstates[bot->s.number] );
#else //!__BOT_AAS__
	entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bot->current_target_entity, bot->r.currentOrigin,
												  bot->client->sess.sessionTeam, bot );
#endif //__BOT_AAS__
	else
	{
		entNum = BotMP_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												bot->client->sess.sessionTeam );
	}
#endif //__NPC__
	if ( entNum < 0 )
	{
#ifdef __NPC__
		if ( bot->NPC_client )
		{
			entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity,
															bot->r.currentOrigin, bot->NPC_client->sess.sessionTeam );
		}
		else
#endif //__NPC__
			entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity,
															bot->r.currentOrigin, bot->client->sess.sessionTeam );
	}

	if ( entNum >= 0 )
	{
		if (g_entities[entNum].num_in_range_nodes > 0)
			best_target = g_entities[entNum].in_range_nodes[0];
		else
			best_target = AIMOD_NAVIGATION_FindClosestReachableNode( &g_entities[entNum], NODE_DENSITY, NODEFIND_ALL, bot );
	}

	if ( best_target >= 0 )
	{						// Found a goal.. Use it...
		bot->current_target_entity = entNum;
		bot->goalentity = &g_entities[entNum];

		// Return the best choice...
		return ( best_target );
	}

	//	G_Printf("%s: failed to find a goal for entity %i! (current node %i)\n", bot->client->pers.netname, entNum, bot->current_node);
	//	if (entNum > 0)
	//		G_Printf("Entity location %f %f %f.\n", g_entities[entNum].r.currentOrigin[0], g_entities[entNum].r.currentOrigin[1], g_entities[entNum].r.currentOrigin[2]);
	// If we got here, then we failed to find a path to a goal...
	return ( -1 );
}


//===========================================================================
// Routine      : BotGetNextNode
// Description  : if the bot has reached a node, this function selects the next node
//				  that he will go to, and returns it right now it's being developed,

//				  feel free to experiment
/*short*/ int
BotGetNextNode ( gentity_t *bot /*, int *state*/ )
{
	/*short*/ int	node = NODE_INVALID;

	//we should never call this in BOTSTATE_MOVE with no goal
	//setup the goal/path in HandleIdleState
	if ( bot->longTermGoal == NODE_INVALID )
	{
		return ( NODE_INVALID );
	}

	if ( bot->pathsize == 0 )					//if the bot is at the end of his path, this shouldn't have been called
	{
		bot->longTermGoal = NODE_INVALID;		//reset to having no goal
		return ( NODE_INVALID );
	}

	node = bot->pathlist[bot->pathsize - 1];	//pathlist is in reverse order
	bot->pathsize--;							//mark that we've moved another node
	return ( node );
}


//===========================================================================
// Routine      : BotEntityCloserOnFuturePath

// Description  : Is an entity closer to one of our current nodes then to us now?
qboolean
BotEntityCloserOnFuturePath ( gentity_t *bot, gentity_t *ent )
{					// Return qtrue if future nodes are closer to "ent" then the current one...
	//int		pathsize = bot->pathsize;
	//float	current_dist = VectorDistance( ent->r.currentOrigin, nodes[bot->current_node].origin );
	/*while ( pathsize > 0 )
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
	}*/

	if (!(bot->next_node <= 0 || bot->next_node > number_of_nodes))
	{
		if (VectorDistance(bot->r.currentOrigin, ent->r.currentOrigin) > VectorDistance(nodes[bot->next_node].origin, ent->r.currentOrigin))
			return ( qtrue );
	}

	return ( qfalse );
}


/* */
float
TravelTime ( gentity_t *bot )
{

	/*float dist;
	float travel_time;

	if (bot->current_node)
	{
		dist = VectorDistance(bot->r.currentOrigin, nodes[bot->current_node].origin);
		travel_time = (dist*0.02)*1000;
		//G_Printf("Travel time set to %f (distance %f) for bot %s.\n", travel_time, dist, bot->client->pers.netname);
	}
	else
	{
		travel_time = 10000;
	}

	if (travel_time < 2000)
		travel_time = 2000;
		*/
	float	travel_time = 10000;//6000;//4000;//10000;
	return ( travel_time );
}

int		next_explosive_check = 0;
vec3_t	bad_spots[MAX_GENTITIES];
float	bad_spots_splash_range[MAX_GENTITIES];
int		bad_spot_count = 0;

qboolean AIMOD_MOVEMENT_Explosive_Near_Waypoint ( int wp )
{// Check bad spot list (explosives that will go off)..
#ifdef __BOT_EXPLOSIVE_AVOID__
	int i;

	for (i = 0; i < bad_spot_count; i++)
	{
		if (VectorDistance(bad_spots[i], nodes[wp].origin) > bad_spots_splash_range[i])
			continue;

		// It is bad!
		return ( qtrue );
	}
#endif //__BOT_EXPLOSIVE_AVOID__

	if (!wp || wp <= 0 || wp > MAX_NODES)
		return ( qfalse );

	if (avoid_node_timer[wp] > level.time)
		return ( qtrue );

	return ( qfalse );
}

qboolean AIMOD_MOVEMENT_Explosive_Near_Object_Position ( gentity_t *object )
{// Check bad spot list (explosives that will go off)..
	if ( !object )
		return ( qfalse );

	if ( object->num_in_range_nodes > 0 )
	{
		int i = 0;

		for (i = 0; i < object->num_in_range_nodes; i++)
		{
			if (AIMOD_MOVEMENT_Explosive_Near_Waypoint(object->in_range_nodes[i]))
				return ( qtrue );
		}
	}

	return ( qfalse );
}

qboolean AIMOD_MOVEMENT_Explosive_Near_Position ( vec3_t position )
{// Check bad spot list (explosives that will go off).. -- UQ1: Costly!!! Now disabled by default with __BOT_EXPLOSIVE_AVOID_CPU_SAVER__
#ifdef __BOT_EXPLOSIVE_AVOID__
	int i;

	for (i = 0; i < bad_spot_count; i++)
	{
		if (VectorDistance(bad_spots[i], position) > bad_spots_splash_range[i])
			continue;

		// It is bad!
		return ( qtrue );
	}
#endif //__BOT_EXPLOSIVE_AVOID__
	int i;

	for (i = 0; i < number_of_nodes; i++)
	{
		if (avoid_node_timer[i] > level.time
			&& VectorDistance(nodes[i].origin, position) < 512)
		{
			return ( qtrue );
		}
	}

	return ( qfalse );
}

extern qboolean Pickup_IsHeavyWeapon( weapon_t weap );

gentity_t *GetBestAmmoEntity( gentity_t *bot )
{
	int		loop = 0, best = -1;
	float	best_dist = 99999.9f;

	for ( loop = MAX_CLIENTS; loop < MAX_GENTITIES; loop++ )
	{
		gentity_t	*test = &g_entities[loop];

		if ( !test )
		{
			continue;
		}

		/*if ( test->item && (test->item->giType == IT_AMMO || test->item->giType == IT_WEAPON) )
		{
			if ( test->item->giType != IT_AMMO )
			{
				if( test->item->giType == IT_WEAPON
					&& (test->item->giTag == WP_LUGER
					|| test->item->giTag == WP_COLT
					|| test->item->giTag == WP_AKIMBO_LUGER
					|| test->item->giTag == WP_AKIMBO_COLT
					|| test->item->giTag == WP_AKIMBO_SILENCEDLUGER
					|| test->item->giTag == WP_AKIMBO_SILENCEDCOLT))
					continue;

				//if ( !(test->item->giType == IT_WEAPON && test->item->giTag == bot->client->sess.playerWeapon))
				if (!(test->item->giType == IT_WEAPON 
					&& (!Pickup_IsHeavyWeapon(test->item->giTag) || bot->client->sess.playerType == PC_SOLDIER)))
				{// UQ1: In EF we can now pickup almost anything...
					continue;
				}
			}
		}
		else*/ if ( Q_stricmp( test->classname, "misc_cabinet_supply") 
			&& Q_stricmp( test->classname, "ammo_crate") 
#ifdef __VEHICLES__
			&& !(test->s.eType == ET_VEHICLE && test->s.weapon == VEHICLE_CLASS_APC)
#endif //__VEHICLES__
			)
		{
			continue;
		}
		
		if (!Q_stricmp( test->classname, "misc_cabinet_supply") && test->count < 2)
		{// Check there is actually ammo at the cabinet before using it!
			continue;
		}

		if ( VectorDistance( bot->r.currentOrigin, test->r.currentOrigin) > best_dist )
		{
			continue;
		}

		best_dist = VectorDistance( bot->r.currentOrigin, test->r.currentOrigin );
		best = test->s.number;
	}

	if ( best > 0 )
	{
		return &g_entities[best];
	}

	// None found.. Let's try finding a fieldops!
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*test = &g_entities[loop];

		if ( !test )
		{
			continue;
		}

		if ( !test->client )
		{
			continue;
		}

		if ( test->client->sess.playerType != PC_FIELDOPS )
		{
			continue;
		}

		if ( test->client->ps.stats[STAT_HEALTH] <= 0 )
		{
			continue;
		}

#ifdef __VEHICLES__
		if ((test->client->ps.eFlags & EF_VEHICLE)
			&& test->client->ps.persistant[PERS_TANKUSED] != VEHICLE_TYPE_AXIS_APC
			&& test->client->ps.persistant[PERS_TANKUSED] != VEHICLE_TYPE_ALLIED_APC)
		{// Can only get ammo from fieldops in an APC vehicle, no others!
			continue;
		}
#endif //__VEHICLES__

		if ( !OnSameTeam(bot, test) )
		{
			continue;
		}

		if ( VectorDistance( bot->r.currentOrigin, test->r.currentOrigin) > best_dist )
		{
			continue;
		}

		best_dist = VectorDistance( bot->r.currentOrigin, test->r.currentOrigin );
		best = test->s.number;
	}

	if ( best > 0 )
	{
		return &g_entities[best];
	}

	return NULL;
}

qboolean IsAmmoEntity( gentity_t *test )
{
	if ( !test )
	{
		return qfalse;
	}

	if ( test->item && (test->item->giType == IT_AMMO /*|| test->item->giType == IT_WEAPON*/) )
	{
		return qtrue;
	}

	if ( !Q_stricmp( test->classname, "misc_cabinet_supply") || !Q_stricmp( test->classname, "ammo_crate"))
	{
		return qtrue;
	}

	if ( test->client && test->client->sess.playerType == PC_FIELDOPS )
	{
		return qtrue;
	}

	return qfalse;
}

extern qboolean map_has_sequential_nodes;
extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );
extern vmCvar_t bot_minplayers;
extern vmCvar_t npc_minplayers;

int pathfind_goal_failures[MAX_NODES];
int pathfind_from_failures[MAX_NODES];

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#include <mmsystem.h>
#include <stdio.h>
#include <conio.h>

void Bot_Queue_New_Path ( gentity_t *bot, usercmd_t *ucmd );

#pragma warning( disable : 4028 )
#pragma warning( disable : 4024 )

DWORD		bot_pathfind_thread = 0;
DWORD		bot_pathfind_thread2 = 0;
DWORD		bot_pathfind_thread3 = 0;
DWORD		bot_pathfind_thread4 = 0;
qboolean	bot_pathfind_queue_active = qtrue;
qboolean	bot_pathfind_queue_active2 = qtrue;
qboolean	bot_pathfind_queue_active3 = qtrue;
qboolean	bot_pathfind_queue_active4 = qtrue;

#ifdef __NPC__
int			bot_path_queue_start_node[MAX_GENTITIES];
int			bot_path_queue_goal_node[MAX_GENTITIES];

extern		void NPC_Queue_New_Path ( gentity_t *bot, usercmd_t *ucmd );
extern		float NPC_TravelTime ( gentity_t *bot );
#else //!__NPC__
int			bot_path_queue_start_node[MAX_CLIENTS];
int			bot_path_queue_goal_node[MAX_CLIENTS];
#endif //__NPC__

#ifdef __CUDA__
__device__ __host__ void Bot_Process_Queued_Bot_Pathfinds ( void )
#else //!__CUDA__
void Bot_Process_Queued_Bot_Pathfinds ( void )
#endif //__CUDA__
{
	int i = 0;

#ifdef __NPC__
	for (i = 0; i < MAX_GENTITIES; i++)
#else //!__NPC__
	for (i = 0; i < MAX_CLIENTS; i++)
#endif //__NPC__
	{
		gentity_t *bot = &g_entities[i];

		if (!bot)
			continue;

#ifdef __NPC__
		if (!bot->client && bot->s.eType != ET_NPC)
		{
			bot->bot_creating_path = qfalse;
			continue;
		}
#else //!__NPC__
		if (!bot->client)
		{
			bot->bot_creating_path = qfalse;
			continue;
		}
#endif //__NPC__

		if (!bot->bot_creating_path)
			continue;

		// Entity locking while creating their path in another thread...
		if (bot->bot_queue_lock)
			continue;

		bot->bot_queue_lock = qtrue;

		// OK, make a new path for them!
		bot->current_node = bot_path_queue_start_node[i];
		bot->longTermGoal = bot_path_queue_goal_node[i];
		bot->followtarget_waypoint_time = level.time;
		bot->last_node = bot->current_node - 1;
		
		if (!(bot->client && bot->followtarget && bot->bot_fireteam_order != FT_ORDER_DEFAULT) 
			&& Q_TrueRand(0,2) == 1 )
		{// Use alt route if possible for every 2nd (randomized) pathfind...
			bot->pathsize = CreateAltPathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
		}
		else
		{
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
		}

		if (bot->pathsize <= 0)
		{// Failed, find another goal and retry!
			if (bot->client)
			{
				bot->bot_creating_path = qfalse;
				bot->longTermGoal = Q_TrueRand(0, number_of_nodes-1);

				if (!(bot->client && bot->followtarget && bot->bot_fireteam_order != FT_ORDER_DEFAULT) 
					&& Q_TrueRand(0,2) == 1 )
				{// Use alt route if possible for every 2nd (randomized) pathfind...
					bot->pathsize = CreateAltPathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
				}
				else
				{
					bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
				}

				if (bot->pathsize > 0)
				{
					bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
					bot->node_timeout = level.time + (TravelTime( bot )*2);
					bot->bot_last_good_move_time = level.time;
					bot->bot_last_position_check = level.time;
					VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
					bot->bot_random_move_time = 0;
				}
				else
				{
					bot->current_node = -1;
					bot->longTermGoal = -1;
					bot->bot_random_move_time = level.time + 5000 + (Q_TrueRand(0, 5000));
				}

				bot->bot_queue_lock = qfalse;
				continue;
			}
#ifdef __NPC__
			else
			{
				bot->bot_creating_path = qfalse;
				bot->longTermGoal = Q_TrueRand(0, number_of_nodes-1);

				if (!(bot->client && bot->followtarget && bot->bot_fireteam_order != FT_ORDER_DEFAULT) 
					&& Q_TrueRand(0,1) == 1 )
				{// Use alt route if possible for every 2nd (randomized) pathfind...
					bot->pathsize = CreateAltPathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
				}
				else
				{
					bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
				}

				if (bot->pathsize > 0)
				{
					bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
					bot->node_timeout = level.time + (TravelTime( bot )*2);
					bot->bot_last_good_move_time = level.time;
					bot->bot_last_position_check = level.time;
					VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
					bot->bot_random_move_time = 0;
				}
				else
				{
					bot->current_node = -1;
					bot->longTermGoal = -1;
					bot->bot_random_move_time = level.time + 5000 + (Q_TrueRand(0, 5000));
				}

				bot->bot_queue_lock = qfalse;
				continue;
			}
#endif //__NPC__
		}

		bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
		bot->node_timeout = level.time + (TravelTime( bot )*2);
		bot->bot_last_good_move_time = level.time;
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		bot->bot_random_move_time = 0;

		bot->bot_creating_path = qfalse;
		bot->bot_queue_lock = qfalse;
	}
}

qboolean pathfind_thread_active = qfalse;
qboolean pathfind_thread_active2 = qfalse;
qboolean pathfind_thread_active3 = qfalse;
qboolean pathfind_thread_active4 = qfalse;

void AIMOD_MOVEMENT_Explosive_Checks ( void );

/* */
void __cdecl
Bot_Execute_Pathfind_Queue ( DWORD index )
{
	while (bot_pathfind_queue_active)
	{
		Bot_Process_Queued_Bot_Pathfinds();
		AIMOD_MOVEMENT_Explosive_Checks(); // UQ1: May as well use the thread for this too! :)
		Sleep( 500 );
	}

	bot_pathfind_thread = 0;

	pathfind_thread_active = qfalse;

	/* _endthread given to terminate */
	_endthread();
}

/* */
void __cdecl
Bot_Execute_Pathfind_Queue2 ( DWORD index )
{
	while (bot_pathfind_queue_active2)
	{
		Bot_Process_Queued_Bot_Pathfinds();
		Sleep( 250 );
	}

	bot_pathfind_thread2 = 0;

	pathfind_thread_active2 = qfalse;

	/* _endthread given to terminate */
	_endthread();
}

/* */
void __cdecl
Bot_Execute_Pathfind_Queue3 ( DWORD index )
{
	while (bot_pathfind_queue_active3)
	{
		Bot_Process_Queued_Bot_Pathfinds();
		Sleep( 250 );
	}

	bot_pathfind_thread3 = 0;

	pathfind_thread_active3 = qfalse;

	/* _endthread given to terminate */
	_endthread();
}

/* */
void __cdecl
Bot_Execute_Pathfind_Queue4 ( DWORD index )
{
	while (bot_pathfind_queue_active4)
	{
		Bot_Process_Queued_Bot_Pathfinds();
		Sleep( 250 );
	}

	bot_pathfind_thread4 = 0;

	pathfind_thread_active4 = qfalse;

	/* _endthread given to terminate */
	_endthread();
}

void Shutdown_Bot_Pathfind_Queue ( void )
{
	bot_pathfind_queue_active = qfalse;
	bot_pathfind_queue_active2 = qfalse;
	bot_pathfind_queue_active3 = qfalse;
	bot_pathfind_queue_active4 = qfalse;

	while (pathfind_thread_active)
		Sleep( 50 );

	while (pathfind_thread_active2)
		Sleep( 50 );

	while (pathfind_thread_active3)
		Sleep( 50 );

	while (pathfind_thread_active4)
		Sleep( 50 );
}

void Begin_Bot_Pathfind_Queue ( void )
{
	// open URL in a new thread (so that main thread is free)
	bot_pathfind_thread = _beginthread( Bot_Execute_Pathfind_Queue, 0, LOWORD( 0) );
	pathfind_thread_active = qtrue;

	Sleep( 500 );

	bot_pathfind_thread2 = _beginthread( Bot_Execute_Pathfind_Queue2, 0, LOWORD( 0) );
	pathfind_thread_active2 = qtrue;

	Sleep( 250 );

	bot_pathfind_thread3 = _beginthread( Bot_Execute_Pathfind_Queue3, 0, LOWORD( 0) );
	pathfind_thread_active3 = qtrue;
	
	Sleep( 250 );

	bot_pathfind_thread4 = _beginthread( Bot_Execute_Pathfind_Queue4, 0, LOWORD( 0) );
	pathfind_thread_active4 = qtrue;
}

//===========================================================================
// Routine      : Bot_Queue_New_Path
// Description  : Find a new path. (Now queued for the pathfinding thread...)

/* */
void
Bot_Queue_New_Path ( gentity_t *bot, usercmd_t *ucmd )
{
	if (bot->bot_creating_path)
	{// UQ1: Added windows threading to create paths with less FPS lag...
		return;
	}

	if (SSE_CPU)
	{
		sse_memset( bot->pathlist, NODE_INVALID, ((sizeof(int)) * MAX_PATHLIST_NODES)/8 );
	}
	else
	{
		memset( bot->pathlist, NODE_INVALID, ((sizeof(int)) * MAX_PATHLIST_NODES) );
	}

	bot->bot_creating_path = qtrue;

	bot->pathsize = 0;

	// Initialize...
	bot->shortTermGoal = -1;

#ifdef __VEHICLES__
	if (bot->client->ps.eFlags & EF_VEHICLE)
		bot_path_queue_start_node[bot->s.number] = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_TANK, bot );
	else
#endif //__VEHICLES__
		bot_path_queue_start_node[bot->s.number] = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );

	if ( bot_path_queue_start_node[bot->s.number] < 0 )
	{
		bot->bot_creating_path = qfalse;
		return;
	}

	if (bot->bot_after_ammo)
	{// Find ammo!
		gentity_t *best = GetBestAmmoEntity(bot);

		if (best)
		{
			bot->goalentity = best;
#ifdef __VEHICLES__
			if (bot->client->ps.eFlags & EF_VEHICLE)
				bot_path_queue_goal_node[bot->s.number] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( best->r.currentOrigin, NODE_DENSITY, NODEFIND_TANK );
			else
#endif //__VEHICLES__
				bot_path_queue_goal_node[bot->s.number] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( best->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );

			bot->last_node = bot->current_node - 1;
			
			if (bot_path_queue_goal_node[bot->s.number] >= 0)
				return;
		}
	}

	if (bot->followtarget && bot->bot_fireteam_order != FT_ORDER_DEFAULT)
	{// Check our fireteam orders...
		switch ( bot->bot_fireteam_order )
		{
		case FT_ORDER_COVER:
			bot_path_queue_goal_node[bot->s.number] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->bot_fireteam_order_coverspot, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;

			if (bot_path_queue_goal_node[bot->s.number] >= 0)
				return;
		case FT_ORDER_SUPRESS:
			bot_path_queue_goal_node[bot->s.number] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->bot_fireteam_order_supression_position, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			
			if (bot_path_queue_goal_node[bot->s.number] >= 0)
				return;
		case FT_ORDER_ASSAULT:
			bot_path_queue_goal_node[bot->s.number] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->bot_fireteam_order_assault_position, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			
			if (bot_path_queue_goal_node[bot->s.number] >= 0)
				return;
		default:
			// Set new route to our commander!
			bot->goalentity = bot->followtarget;
			bot_path_queue_goal_node[bot->s.number] = AIMOD_NAVIGATION_FindFollowRouteClosestReachableNodeTo( bot->goalentity, bot->goalentity->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			
			if (bot_path_queue_goal_node[bot->s.number] >= 0)
				return;
		}
	}

	bot->goalentity = NULL;
	bot_path_queue_goal_node[bot->s.number] = Unique_FindGoal( bot );

	if ( bot_path_queue_goal_node[bot->s.number] < 0 )
	{													// Find an enemy to head for...
		int best_ent = Find_Goal_Enemy( bot );

		if ( best_ent >= 0 )
		{
			bot->goalentity = &g_entities[best_ent];
#ifdef __VEHICLES__
			if (bot->client->ps.eFlags & EF_VEHICLE)
				bot_path_queue_goal_node[bot->s.number] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->goalentity->r.currentOrigin, NODE_DENSITY, NODEFIND_TANK );
			else
#endif //__VEHICLES__
				bot_path_queue_goal_node[bot->s.number] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->goalentity->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
		}
	}

	if ( bot_path_queue_goal_node[bot->s.number] < 0 )
	{													// Just get us out of trouble...
		bot->goalentity = NULL;
		bot_path_queue_goal_node[bot->s.number] = Q_TrueRand(0, number_of_nodes);
	}
}
#endif //_WIN32

//===========================================================================
// Routine      : Bot_Set_New_Path
// Description  : Find a new path.

/* */
void
Bot_Set_New_Path ( gentity_t *bot, usercmd_t *ucmd )
{
#if defined (_WIN32) && defined (__BOT_THREADING__)
	if (bot->bot_creating_path)
	{// UQ1: Added windows threading to create paths with less FPS lag...
		return;
	}
#endif //defined (_WIN32) && defined (__BOT_THREADING__)

	if (SSE_CPU)
	{
		sse_memset( bot->pathlist, NODE_INVALID, ((sizeof(int)) * MAX_PATHLIST_NODES)/8 );
	}
	else
	{
		memset( bot->pathlist, NODE_INVALID, ((sizeof(int)) * MAX_PATHLIST_NODES) );
	}

	bot->pathsize = 0;

	// Initialize...
	bot->shortTermGoal = -1;

#ifdef __VEHICLES__
	if (bot->client->ps.eFlags & EF_VEHICLE)
		bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_TANK, bot );
	else
#endif //__VEHICLES__
		bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );

	if ( bot->current_node < 0 )
	{
		//G_Printf("%s failed to find a closest node!\n", bot->client->pers.netname);
		return;
	}

	if (bot->bot_after_ammo)
	{// Find ammo!
		gentity_t *best = GetBestAmmoEntity(bot);

		if (best)
		{
			bot->goalentity = best;
#ifdef __VEHICLES__
			if (bot->client->ps.eFlags & EF_VEHICLE)
				bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( best->r.currentOrigin, NODE_DENSITY, NODEFIND_TANK );
			else
#endif //__VEHICLES__
				bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( best->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
			bot->last_node = bot->current_node - 1;

			if (Q_TrueRand(0,2) == 1)
			{// Use alt route if possible for every 2nd (randomized) pathfind...
				bot->pathsize = CreateAltPathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			}
			else
			{
				bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			}
			bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

			if (bot->pathsize)
				return;
		}
	}

	if (bot->followtarget && bot->bot_fireteam_order != FT_ORDER_DEFAULT)
	{// Check our fireteam orders...
		switch ( bot->bot_fireteam_order )
		{
		case FT_ORDER_COVER:
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->bot_fireteam_order_coverspot, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			bot->last_node = bot->current_node - 1;
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

			if (bot->pathsize)
				return;
		case FT_ORDER_SUPRESS:
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->bot_fireteam_order_supression_position, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			bot->last_node = bot->current_node - 1;
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

			if (bot->pathsize)
				return;
		case FT_ORDER_ASSAULT:
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->bot_fireteam_order_assault_position, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			bot->last_node = bot->current_node - 1;
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

			if (bot->pathsize)
				return;
		default:
			// Set new route to our commander!
			bot->goalentity = bot->followtarget;
			bot->longTermGoal = AIMOD_NAVIGATION_FindFollowRouteClosestReachableNodeTo( bot->goalentity, bot->goalentity->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			bot->last_node = bot->current_node - 1;
			if (Q_TrueRand(0,2) == 1)
			{// Use alt route if possible for every 2nd (randomized) pathfind...
				bot->pathsize = CreateAltPathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			}
			else
			{
				bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			}
			bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

			if (bot->pathsize)
				return;
		}
	}

	//if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
	//{													// Try to roam out of problems...
		bot->goalentity = NULL;
		bot->longTermGoal = Unique_FindGoal( bot );
	/*}
	else
	{													// Work out why we got here, and pick another goal based on the reason...
		if ( VectorDistance( nodes[bot->longTermGoal].origin, bot->r.currentOrigin) < 32 )
		{												// We found our current goal. Select another...
			bot->goalentity = NULL;
			bot->longTermGoal = Unique_FindGoal( bot );
		}
		else if
			(
				(bot->goalentity && bot->goalentity->client) &&
				(bot->goalentity->health <= 0 || (bot->goalentity->client->ps.pm_flags & PMF_LIMBO))
			)
		{												// If our goal entity is a player/bot, and they are dead... Select another goal...
			bot->goalentity = NULL;
			bot->longTermGoal = Unique_FindGoal( bot );
		}
	}*/

	if ( bot->longTermGoal < 0 )
	{													// Find an enemy to head for...
		int best_ent = Find_Goal_Enemy( bot );

		if ( best_ent >= 0 )
		{
			bot->goalentity = &g_entities[best_ent];
#ifdef __VEHICLES__
			if (bot->client->ps.eFlags & EF_VEHICLE)
				bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->goalentity->r.currentOrigin, NODE_DENSITY, NODEFIND_TANK );
			else
#endif //__VEHICLES__
				bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->goalentity->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
		}
	}

	if ( bot->longTermGoal < 0 )
	{													// Just get us out of trouble...
		bot->goalentity = NULL;
		bot->longTermGoal = Q_TrueRand(0, number_of_nodes);
	}

	bot->last_node = bot->current_node - 1;

	if (Q_TrueRand(0,2) == 1)
	{// Use alt route if possible for every 2nd (randomized) pathfind...
		bot->pathsize = CreateAltPathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
	}
	else
	{
		bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
	}

	bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
	bot->node_timeout = level.time + (TravelTime( bot )*2);
	bot->bot_last_good_move_time = level.time;
	bot->bot_last_position_check = level.time;
	VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Shorten_Path

// Description  : Quickly shorten the current path... Skip any extra nodes we can...
void
AIMOD_MOVEMENT_Shorten_Path ( gentity_t *bot )
{

	//#ifdef __BOT_SHORTEN_ROUTING__
	int pathsize = bot->pathsize;
	int link;
	if ( !bot->current_node || !bot->next_node || !bot->longTermGoal || pathsize < 5 )
	{
		return; // No point shortening this path...
	}

	if ( bot->bot_wanderring )
	{
		return; // Don't shorten wanderring bot's paths...
	}

	// Don't do this continually on the same node...
	if ( bot->last_shorten_path_node == bot->current_node )
	{
		return;
	}

	//bot->last_shorten_path_node = bot->current_node;
	while ( pathsize > 0 )
	{
		if ( !nodes[bot->pathlist[pathsize - 1]].origin )
		{
			break;
		}

		for ( link = 0; link < nodes[bot->current_node].enodenum; link++ )
		{
			if ( !nodes[bot->current_node].links[link].targetNode )
			{
				break;
			}

			if ( nodes[bot->current_node].links[link].flags & PATH_BLOCKED )
			{	//if this path is blocked, skip it
				continue;
			}

			if ( nodes[bot->current_node].links[link].flags & PATH_DANGER )
			{	//if this path is dangerous, skip it
				continue;
			}

#ifdef __VEHICLES__
			if
			(
				bot->client &&
				(bot->client->ps.eFlags & EF_VEHICLE) &&
				nodes[bot->current_node].links[link].flags & PATH_NOTANKS
			)
			{	//if this path is blocked, skip it
				continue;
			}
#endif //__VEHICLES__

			if ( nodes[bot->current_node].links[link].targetNode == bot->pathlist[pathsize - 1] )
			{
				if (NodeVisible(bot->r.currentOrigin, nodes[nodes[bot->current_node].links[link].targetNode].origin, bot->s.number) >= 1)
				{
					bot->pathsize = pathsize - 1;
					bot->next_node = bot->pathlist[bot->pathsize - 1];	//pathlist is in reverse order

					//G_Printf("%s's path was shortened from %i nodes to %i nodes.\n", bot->client->pers.netname, original_size, bot->pathsize);
					// Path has been shortened...
					return;
				}
			}
		}

		pathsize--;														//mark that we've moved another node
	}

	//#endif //__BOT_SHORTEN_ROUTING__
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_ENG_ReadyToDynamite

// Description  : Is this engineer ready to drop some dynamite???
qboolean
AIMOD_MOVEMENT_ENG_ReadyToDynamite ( gentity_t *bot )
{
	usercmd_t	*ucmd = &bot->client->pers.cmd;
	if
	(
		bot->bot_goal_destroyable &&
		bot->bot_goal_entity &&
		VectorDistance( bot->bot_goal_position, bot->r.currentOrigin) < 256
	)
	{
		vec3_t	origin, mins, maxs;
		int		touch[MAX_GENTITIES];
		int		numListedEntities;
		int		i;
		VectorCopy( bot->bot_goal_entity->r.currentOrigin, origin );
		VectorAdd( origin, bot->bot_goal_entity->r.mins, mins );
		VectorAdd( origin, bot->bot_goal_entity->r.maxs, maxs );
		numListedEntities = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
		for ( i = 0; i < numListedEntities; i++ )
		{
			gentity_t	*hit = &g_entities[touch[i]];
			if ( hit == bot )
			{
				gentity_t	*dynamite = Dynamite_Entity( bot );
				if ( !dynamite && next_dynamite_test[bot->s.number] <= level.time )
				{
					vec3_t	angles;
					bot->beStillEng = level.time + 10000;
					bot->client->pers.cmd.weapon = WP_DYNAMITE;
					if ( next_satchel_available_time[bot->s.clientNum] < level.time )
					{

						//weapon_grenadelauncher_fire(bot, bot->client->ps.weapon);
						next_dynamite_test[bot->s.number] = level.time + ( 1000 / (bot->skillchoice + 1) );
						ucmd->buttons |= BUTTON_ATTACK;
						next_satchel_available_time[bot->s.clientNum] = level.time + 10000;
					}

					// Head for our object!
					VectorSubtract( bot->bot_goal_entity->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
					vectoangles( bot->move_vector, angles );
					VectorCopy( angles, bot->s.angles );
					if ( bot->bot_last_chat_time < level.time )
					{
						bot->bot_last_chat_time = level.time + 10000;
						G_Voice( bot, NULL, SAY_TEAM, va( "CoverMe"), qfalse );
					}
				}

				return ( qtrue );
			}
		}
	}

	return ( qfalse );
}

#ifdef __ETE__
vmCvar_t	g_realmedic;
vmCvar_t	g_noSelfHeal;
#endif //__ETE__

void AIMOD_MOVEMENT_CheckAvoidance ( gentity_t *bot, usercmd_t *ucmd )
{
	qboolean onLadder = qfalse;
	gclient_t *client = bot->client;

	if (bot->s.eType == ET_NPC)
		client = bot->NPC_client;

	if (client->ps.serverCursorHint == HINT_LADDER || client->ps.torsoAnim == BOTH_CLIMB || client->ps.legsAnim == BOTH_CLIMB)
	{
		onLadder = qtrue;
	}

	if ( VectorLength( client->ps.velocity) < 24 
		&& !onLadder 
		&& bot->bot_jump_time < level.time 
		&& bot->bot_duck_time < level.time
		&& bot->bot_strafe_left_time < level.time
		&& bot->bot_strafe_right_time < level.time )
	{
		if (Q_TrueRand(0,10) <= 5)
		{// Try Jumping...
			//bot->bot_jump_time = level.time + 500;
			ucmd->upmove = 127;
		}
		else
		{// Try ducking..
			bot->bot_duck_time = level.time + 500;
			
			if ( !(client->ps.pm_flags & PMF_DUCKED) )
			{												// For special duck nodes, make the bot duck down...
				ucmd->upmove = -48;
				client->ps.pm_flags |= PMF_DUCKED;
			}
		}
	}
}

extern void BOT_ChangeWeapon( gentity_t *bot, int weaponChoice );

//===========================================================================
// Routine      : AIMOD_MOVEMENT_SpecialMoves

// Description  : Bot Special Movements (medic, fldops, cvt, etc...)
qboolean
AIMOD_MOVEMENT_SpecialMoves ( gentity_t *bot, usercmd_t *ucmd )
{
	vec3_t	eyes;

	if ( !bot->skillchoice )
	{
		bot->skillchoice = Q_irand(1,5);
	}

	if ( no_mount_time[bot->s.clientNum] > level.time )
	{
		bot->beStillEng = 0;
		bot->beStill = 0;
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{																								// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{																								// Release the emplaced gun we are on to do our job...
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}
	}

	// Special Movements.
	VectorCopy( bot->r.currentOrigin, eyes );
	eyes[2] += 48;
	
	if ( bot->client->ps.weapon == WP_DYNAMITE && next_dynamite_test[bot->s.number] <= level.time )
	{																									// Make sure we plant!
		gentity_t	*dynamite = Dynamite_Entity( bot );
		if ( dynamite == NULL )
		{
			next_dynamite_test[bot->s.number] = level.time + ( 1000 / (bot->skillchoice + 1) );
			ucmd->buttons |= BUTTON_ATTACK;
			bot->needed_node = -1;

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;

			bot->bot_job_wait_time = level.time + 2000;

			return ( qtrue );
		}
	}

	// Check for any generic ingame sounds we might need...
	Check_VoiceChats( bot );

	if
	(
		(bot->beStillEng > level.time || AIMOD_MOVEMENT_ENG_ReadyToDynamite( bot)) &&
		(bot->client->sess.playerType == PC_ENGINEER)
	)
	{																									// Got some building to do!
		gentity_t	*dynamite = Dynamite_Entity( bot );

		if ( dynamite != NULL )
		{																								// Dynamite Found. Head toward it and help plant.
			if ( dynamite->s.teamNum >= 4 )
			{																							// Dynamite not planted.
				bot->beStillEng = level.time + 2000;
				bot->beStill = level.time + 2000;

				if ( bot->client->ps.weapon != WP_PLIERS )
				{		
					if (bot->client->ps.weapon != WP_PLIERS)	
					{// Switch to plyers and use them!
						BOT_ChangeWeapon(bot, WP_PLIERS);
					}

					bot->needed_node = -1;
					if ( !(bot->client->ps.eFlags & EF_CROUCHING) )
					{

						//ucmd->upmove = -127; // Crouch?
						bot->client->ps.eFlags |= EF_CROUCHING;
						ucmd->upmove = -48;																// Crouch...
						bot->client->ps.pm_flags |= PMF_DUCKED;
					}

					// Set our last good move time to now...
					bot->bot_last_good_move_time = level.time;

					bot->bot_job_wait_time = level.time + 2000;

					return ( qtrue );
				}

				if ( !(bot->client->ps.eFlags & EF_CROUCHING) )
				{

					bot->client->ps.eFlags |= EF_CROUCHING;
					ucmd->upmove = -48;																	// Crouch...
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}

				VectorSubtract( dynamite->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
				/*AIMOD_Bot_Aim_At_Object(bot, dynamite);

				if ( VectorDistance( dynamite->r.currentOrigin, bot->r.currentOrigin) > 48 )
				{
					G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
				}*/

				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
				BotSetViewAngles( bot, 100 );
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
				VectorCopy( bot->r.currentOrigin, last_vector[bot->s.clientNum] );
				AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
				
				if ( bot->client->ps.weapon == WP_PLIERS )
				{
					ucmd->buttons |= BUTTON_ATTACK;
				}

				bot->needed_node = -1;

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;

				bot->bot_job_wait_time = level.time + 2000;

				return ( qtrue );
			}
			else
			{																							// Planted already. Get away from here!
				next_dynamite_test[bot->s.number] = level.time + 2000;
				bot->client->ps.eFlags &= ~EF_CROUCHING;
				G_Voice( bot, NULL, SAY_TEAM, va( "FireInTheHole"), qfalse );
				bot->beStillEng = 0;
				bot->beStill = 0;
				
				if ( NodesLoaded() == qtrue )
				{
					bot->beStillEng = 0;
					if ( runaway_node[bot->s.clientNum] == qfalse )
					{
						bot->bot_job_wait_time = level.time + 2000;
#ifdef _WIN32
						if (bot_debug.integer && bot->goalentity 
							&& !Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot"))
							&& VectorDistance(bot->r.currentOrigin, bot->goalentity->s.origin) < 512)
							G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^7 queued a new path! - distance to goal was %f. (1)\n", bot->client->pers.netname, VectorDistance(bot->r.currentOrigin, bot->goalentity->s.origin) );

						Bot_Queue_New_Path( bot, ucmd );

						bot->node_timeout = level.time + (TravelTime( bot )*2);
						bot->bot_last_good_move_time = level.time;
						bot->bot_last_position_check = level.time;
						VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
						//bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
#else //!_WIN32
						Bot_Set_New_Path( bot, ucmd );
			
						bot->node_timeout = level.time + (TravelTime( bot )*2);
						bot->bot_last_good_move_time = level.time;
						bot->bot_last_position_check = level.time;
						VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

						if (bot->current_node < 0 || bot->current_node > number_of_nodes || bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes)
						{
							bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
							return;
						}
#endif //_WIN32
						return ( qfalse );
					}

					if ( bot->next_node >= 0 )
					{
						VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
						vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
						BotSetViewAngles( bot, 100 );
						G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
						VectorCopy( bot->r.currentOrigin, last_vector[bot->s.clientNum] );

						AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
					}
					else
					{
						ucmd->forwardmove = -120;
					}
				}
				else
				{
					ucmd->forwardmove = -120;
				}

				if ( bot->client->ps.eFlags & EF_CROUCHING )
				{																						// We're still crouching.. Get up.
					//ucmd->upmove = 127;																	// Jump up from crouch.
					bot->client->ps.eFlags &= ~EF_CROUCHING;
				}

				Set_Best_AI_weapon( bot );
				bot->needed_node = -1;

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;

				bot->bot_job_wait_time = level.time + 2000;

				return ( qtrue );
			}
		}
		else
		{																								// No dynamite around. Use pliers to build something.
			if ( bot->bot_can_build )
			{
				if ( !(bot->client->ps.eFlags & EF_CROUCHING) )
				{
					ucmd->upmove = -127;																// Crouch?
					bot->client->ps.eFlags |= EF_CROUCHING;
				}

				if (bot->client->ps.weapon != WP_PLIERS)
				{
					BOT_ChangeWeapon(bot, WP_PLIERS);
				}

				ucmd->buttons |= BUTTON_ATTACK;
				bot->needed_node = -1;

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;

				bot->bot_job_wait_time = level.time + 2000;

				return ( qtrue );
			}
			else
			{																							// Must have finished building... Init our goal and path...
				bot->beStillEng = 0;
				Set_Best_AI_weapon( bot );
				return ( qfalse );
			}
		}
	}

	runaway_node[bot->s.clientNum] = qfalse;														// Initialize it!

	if ( ((bot->client->ps.eFlags & EF_MOUNTEDTANK) || (bot->client->ps.eFlags & EF_AAGUN_ACTIVE)) && bot->tankLink )
	{																									// Force no movement on emplaced/tank gun users... They stay on their gun until death!
		//bot->beStill = level.time + 2000;
		if ( next_turret_angle_change[bot->s.clientNum] < level.time )
		{																								// Now set angles randomly.
			bot->s.angles[YAW] += 5;																	//ang_mod;
			next_turret_angle_change[bot->s.clientNum] = level.time + 40;
		}

		VectorCopy( bot->tankLink->r.currentOrigin, bot->r.currentOrigin );
		bot->r.currentOrigin[2] += 96;
		VectorCopy( bot->tankLink->r.currentOrigin, bot->client->ps.origin );
		bot->client->ps.origin[2] += 96;
		if ( bot->beStill < level.time - 1000 )
		{
			bot->beStill = level.time + 20000;															// 20 secs on mover.
		}
		else if ( bot->beStill <= level.time && !bot->enemy )
		{
			G_LeaveTank( bot, qtrue );
		}

		bot->needed_node = -1;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;

		return ( qtrue );
	}
	else if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
	{																									// Force no movement on emplaced/tank gun users... They stay on their gun until death!
		int ang_mod = rand() % 10;
		int ang_dir = rand() % 10;
		bot->beStill = level.time + 2000;
		if ( next_turret_angle_change[bot->s.clientNum] < level.time )
		{																								// Now set angles randomly.
			if ( ang_dir > 4 )
			{
				bot->s.angles[YAW] += ang_mod;
			}
			else
			{
				bot->s.angles[YAW] -= ang_mod;
			}

			next_turret_angle_change[bot->s.clientNum] = level.time + 40;
		}

		bot->needed_node = -1;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;

		return ( qtrue );
	}

	if ( bot->beStill > level.time )
	{		
		// Got something to do! Turret or tank gun?
		bot->needed_node = -1;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;

		return ( qtrue );
	}

	if ( bot->bot_wait_for_supplies > level.time )
	{																									// Wait for medic/ammo packs...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;

		if (bot->enemy)
		{
			AIMOD_SP_MOVEMENT_CoverSpot_Aim_At_Enemy( bot );

			if (!AIMOD_SP_MOVEMENT_Is_CoverSpot_Weapon(bot->client->ps.weapon))
			{
				Set_Best_AI_weapon( bot );
				return ( qtrue );
			}
				
			if (MyInFOV(bot->enemy, bot, 12, 25))
			{
				CalcMuzzlePoints(bot, bot->client->ps.weapon);

				if (VectorDistance(bot->r.currentOrigin, bot->enemy->r.currentOrigin) < 256 
					|| MyVisible_No_Supression(bot, bot->enemy))
				{
					//ucmd->buttons |= BUTTON_ATTACK;
					AIMOD_SP_MOVEMENT_CoverSpot_Do_Actual_Attack( bot, ucmd );
				}
			}
		}

		return ( qtrue );
	}

	//
	// Now the actual JobTypes code...
	//

	if ( bot->beStillEng > level.time && bot->client->ps.weapon == WP_PLIERS )
	{
		bot->needed_node = -1;

		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		ucmd->buttons |= BUTTON_ATTACK;

		bot->bot_job_wait_time = level.time + 2000;

		return ( qtrue );
	}

	// Need to continue normal movememnt... So return qfalse.
	return ( qfalse );
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Setup_Strafe

// Description  : If we need to strafe, then set it up with this!
void
AIMOD_MOVEMENT_Setup_Strafe ( gentity_t *bot, usercmd_t *ucmd )
{
	vec3_t	forward, right, up, rightOrigin, leftOrigin, temp;
	trace_t tr;
#ifdef __NPC__
	if ( bot->client )
	{
		AngleVectors( bot->client->ps.viewangles, forward, right, up );
		VectorMA( bot->r.currentOrigin, 64, right, rightOrigin );
		VectorMA( bot->r.currentOrigin, -64, right, leftOrigin );
	}
	else if ( bot->NPC_client )
	{
		AngleVectors( bot->NPC_client->ps.viewangles, forward, right, up );
		VectorMA( bot->r.currentOrigin, 64, right, rightOrigin );
		VectorMA( bot->r.currentOrigin, -64, right, leftOrigin );
	}
	else
	{
		AngleVectors( bot->s.angles, forward, right, up );
		VectorMA( bot->r.currentOrigin, 64, right, rightOrigin );
		VectorMA( bot->r.currentOrigin, -64, right, leftOrigin );
	}

#else //!__NPC__
	AngleVectors( bot->client->ps.viewangles, forward, right, up );
	VectorMA( bot->r.currentOrigin, 64, right, rightOrigin );
	VectorMA( bot->r.currentOrigin, -64, right, leftOrigin );
#endif //__NPC__
	if ( NodeVisible( rightOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		VectorCopy( rightOrigin, bot->bot_strafe_target_position );
		bot->bot_strafe_right_timer = level.time + 1001;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 64;
		ucmd->upmove = 0;
		//bot->longTermGoal = -1;
		//bot->current_node = -1;
		return;
	}
	else if ( NodeVisible( leftOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		VectorCopy( leftOrigin, bot->bot_strafe_target_position );
		bot->bot_strafe_left_timer = level.time + 1001;
		ucmd->forwardmove = 0;
		ucmd->rightmove = -64;
		ucmd->upmove = 0;
		//bot->longTermGoal = -1;
		//bot->current_node = -1;
		return;
	}
	/*else
	{
		//ucmd->upmove = 127;
		//ucmd->forwardmove = 127;
		//bot->bot_jump_time = level.time + 1001;
		bot->longTermGoal = -1;
		return;
	}*/

	// Prepare to set another player/bot's (our blocker) strafe dir...
	VectorCopy( bot->r.currentOrigin, temp );
	temp[0] += forward[0] * 64;
	temp[1] += forward[1] * 64;
	temp[2] += forward[2] * 64;
	trap_Trace( &tr, bot->r.currentOrigin, NULL, NULL, temp, -1, MASK_SHOT | MASK_OPAQUE );
#ifdef __NPC__
	if ( tr.entityNum && (g_entities[tr.entityNum].r.svFlags & SVF_BOT || g_entities[tr.entityNum].s.eType == ET_NPC) )
#else //!__NPC__
		if ( tr.entityNum && (tr.entityNum < MAX_CLIENTS && g_entities[tr.entityNum].r.svFlags & SVF_BOT) )
#endif //__NPC__
		{	// A bot is stopping us... Tell them to strafe other direction to us...
#ifdef __NPC__
			if ( g_entities[tr.entityNum].client && bot->bot_strafe_left_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
				VectorCopy( leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}
			else if ( g_entities[tr.entityNum].NPC_client && bot->bot_strafe_left_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].NPC_client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
				VectorCopy( leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}
			else if ( g_entities[tr.entityNum].client && bot->bot_strafe_right_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
				VectorCopy( rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}
			else if ( g_entities[tr.entityNum].NPC_client && bot->bot_strafe_right_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].NPC_client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
				VectorCopy( rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}

#else //!__NPC__
			if ( bot->bot_strafe_left_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
				VectorCopy( leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}
			else if ( bot->bot_strafe_right_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
				VectorCopy( rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}
#endif //__NPC__
		}
}

extern float	VectorDistanceNoHeight ( vec3_t v1, vec3_t v2 );
extern float	HeightDistance ( vec3_t v1, vec3_t v2 );
extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );
extern qboolean HaveSequentialNodes ( void );


/* */
int
AIMOD_Movement_SniperClosestEnemy ( gentity_t *bot )
{
	int		client;
	float	closest = 99999.0f;
	int		closest_enemy = -1;
	for ( client = 0; client < MAX_CLIENTS; client++ )
	{
		gentity_t	*ent = &g_entities[client];
		if ( !ent )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}

		if ( VectorDistance( bot->r.currentOrigin, ent->r.currentOrigin) > closest )
		{
			continue;
		}

		if ( OnSameTeam( bot, ent) )
		{
			continue;
		}

		closest = VectorDistance( bot->r.currentOrigin, ent->r.currentOrigin );
		closest_enemy = client;
	}

	return ( closest_enemy );
}

int test_last_message[MAX_CLIENTS];

/*
==============
getAltWeapon
==============
*/
static int getAltWeapon( int weapnum ) {
/*	if(weapnum > MAX_WEAP_ALTS) Gordon: seems unneeded
		return weapnum;*/

	if(weapAlts[weapnum])
		return weapAlts[weapnum];

	return weapnum;
}

qboolean 
AIMOD_Movement_SniperCheckReload ( gentity_t *bot, usercmd_t *ucmd, qboolean avoiding_explosives )
{
	if (bot->bot_reload_time >= level.time)
	{
		return qtrue; // Still reloading...
	}

	if (BG_IsScopedWeapon( bot->s.weapon ))
	{// Currently sniping, check ammo...
		if ( bot->client->ps.ammoclip[BG_FindClipForWeapon((weapon_t)bot->s.weapon)] <= 0 )
		{// No ammo, force reload!
			if ( bot->client->ps.ammoclip[BG_FindClipForWeapon((weapon_t)bot->s.weapon)] <= 0
				 && bot->client->ps.ammo[BG_FindAmmoForWeapon((weapon_t)bot->s.weapon)] > 0)
			{// Have more ammo to reload with, so reload this weapon...
				if (bot_debug.integer)
					G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^5's is reloading his sniper weapon.\n", bot->client->pers.netname );

				ucmd->weapon = getAltWeapon(bot->s.weapon);
				bot->bot_reload_time = level.time + 3000;
				return qtrue;
			}
			else
			{// Has no more ammo at all... Stop sniping and set a new path...
				// Force weapon change...
				bot->bot_reload_time = 0;
				bot->bot_snipetime = 0;
				
				// Do the actual switch...
				Set_Best_AI_weapon( bot );
				
				// Force a delay befor switching again, and reset the goal so the sniper finds new ammo...
				bot->bot_reload_time = level.time + 3000;
				bot->longTermGoal = -1;
				bot->current_node = -1;
				bot->goalentity = NULL;
				return qtrue;
			}
		}
	}

	return qfalse;
}

/* */
void
AIMOD_Movement_SniperMove ( gentity_t *bot, usercmd_t *ucmd, qboolean avoiding_explosives )
{
	if (AIMOD_Movement_SniperCheckReload( bot, ucmd, avoiding_explosives ))
	{
		return; // Check for reloading...
	}

	if ( test_last_message[bot->s.number] < level.time - 3000 )
	{
		if (bot_debug.integer)
			G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^5's has found his sniper point. (currently sniping)\n",
				  bot->client->pers.netname );

		test_last_message[bot->s.number] = level.time;
	}

	if ( (bot->bot_explosive_avoid_waypoint >= 0 || avoiding_explosives))
	{// Avoiding an explosion, keep moving!

	}
	else if ( bot->enemy && bot->bot_enemy_visible_time >= level.time && AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ) )
	{															// Do attacking first...
		if (bot_debug.integer)
			G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^5's has found his sniper point and is attacking!!!\n",
				  bot->client->pers.netname );

		bot->bot_snipetime = level.time + bot_min_snipe_time.integer;
		AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
		bot->beStill = level.time + 2000;
		return;
	}
	else
	{
		trace_t tr;
		vec3_t	forward, right, up, fwdPoint;
		float	fulldiff = 0;
		
		fulldiff = fabs( AngleDifference( bot->client->ps.viewangles[0], bot->bot_ideal_view_angles[0]) );
		fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[1], bot->bot_ideal_view_angles[1]) );
		fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[2], bot->bot_ideal_view_angles[2]) );
		
		if ( fulldiff < 5 )
		{
			int closest_enemy = AIMOD_Movement_SniperClosestEnemy( bot );

			if ( !closest_enemy )
			{
				AngleVectors( bot->client->ps.viewangles, forward, right, up );

				// Set up ideal view angles for this enemy...
				bot->move_vector[0] += Q_TrueRand( -5, 5 );
				bot->move_vector[1] += Q_TrueRand( -25, 25 );
				bot->move_vector[2] += Q_TrueRand( -5, 5 );
				VectorMA( bot->r.currentOrigin, 65500, forward, fwdPoint );
				trap_Trace( &tr, bot->r.currentOrigin, NULL, NULL, fwdPoint, bot->s.number, MASK_SHOT );

				if ( VectorDistance( bot->r.currentOrigin, tr.endpos) < 512 )
				{												// If looking at ground, look up a bit!
					bot->move_vector[PITCH] += 10;
				}

				if ( tr.surfaceFlags & SURF_SKY )
				{												// If looking at sky, look down a bit...
					bot->move_vector[PITCH] -= 10;
				}

				if ( VectorDistance(tr.endpos, bot->r.currentOrigin) < 1024)
				{
					int ammount = Q_irand(65, 180);

					if (Q_irand(0,1) == 1)
						bot->move_vector[YAW] -= ammount;
					else
						bot->move_vector[YAW] += ammount;
				}

				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			}
			else
			{
				vec3_t	enemy_dir, enemy_angles;
				float	enemydiff = 0;

				AngleVectors( bot->client->ps.viewangles, forward, right, up );

				// Set up ideal view angles for this enemy...
				bot->move_vector[0] += Q_TrueRand( -5, 5 );
				bot->move_vector[1] += Q_TrueRand( -25, 25 );
				bot->move_vector[2] += Q_TrueRand( -5, 5 );
				VectorMA( bot->r.currentOrigin, 65500, forward, fwdPoint );
				trap_Trace( &tr, bot->r.currentOrigin, NULL, NULL, fwdPoint, bot->s.number, MASK_SHOT );

				if ( VectorDistance( bot->r.currentOrigin, tr.endpos) < 512 )
				{												// If looking at ground, look up a bit!
					bot->move_vector[PITCH] += 10;
				}

				if ( tr.surfaceFlags & SURF_SKY )
				{												// If looking at sky, look down a bit...
					bot->move_vector[PITCH] -= 10;
				}

				VectorSubtract( g_entities[closest_enemy].r.currentOrigin, bot->r.currentOrigin, enemy_dir );
				vectoangles( enemy_dir, enemy_angles );
				enemydiff = fabs( AngleDifference( bot->bot_ideal_view_angles[0], enemy_angles[0]) );
				enemydiff += fabs( AngleDifference( bot->bot_ideal_view_angles[1], enemy_angles[1]) );
				enemydiff += fabs( AngleDifference( bot->bot_ideal_view_angles[2], enemy_angles[2]) );
				
				if ( enemydiff > 45 )
				{												// Look somewhat toward them...
					bot->move_vector[1] = enemy_angles[1] + Q_TrueRand( -25, 25 );
				}

				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			}
		}

		BotSetViewAngles( bot, 100 );
	}

	ucmd->forwardmove = 0;
	ucmd->rightmove = 0;
	ucmd->upmove = 0;
	bot->node_timeout = level.time + ( TravelTime( bot) * 2 );	// Duck moves take longer, so...

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;
	return;
}


/* */
qboolean
AIMOD_MOVEMENT_WaypointTouched ( gentity_t *bot, int touch_node, qboolean onLadder )
{
	vec3_t	node_origin;
	float	node_distance;
	float	heightdiff;
	gclient_t *client = bot->client;

	if (bot->s.eType == ET_NPC)
		client = bot->NPC_client;

	VectorCopy( nodes[touch_node].origin, node_origin );
	node_distance = VectorDistanceNoHeight( bot->r.currentOrigin, node_origin );
	heightdiff = HeightDistance( bot->r.currentOrigin, node_origin );
	
	if ( onLadder )
	{// Need vertical checks on ladders...
		if ( node_distance < 4 && heightdiff < 2 )
		{
			return ( qtrue );
		}

		return ( qfalse );
	}

	if ( heightdiff > 64 && node_origin[2] < bot->r.currentOrigin[2] && !bot->waterlevel)
		return ( qfalse ); // If the wp is below us, we need to move a bit further till we are at its height (drop down)...

	/*if (BG_IsScopedWeapon(bot->s.weapon) || BG_IsScopedWeapon2(bot->s.weapon))
	{// *sigh* Snipers need to be more exact to climb ledges, etc...
		if ( node_distance < 64 && heightdiff < 4 )
		{
			return ( qtrue );
		}

		return ( qfalse );
	}*/

/*#ifdef __VEHICLES__
	if ( client && (client->ps.eFlags & EF_VEHICLE))
	{
		if ( bot->next_node >= 0 
			&& bot->next_node < number_of_nodes 
			&& node_distance < 128 
			&& Tank_PositionVisible(bot->r.currentOrigin, nodes[bot->next_node].origin, bot->s.number) )
		{							// At the node...
			return ( qtrue );
		}
	}
	else
#endif //__VEHICLES__*/
	if ( bot->next_node >= 0 
		&& bot->next_node < number_of_nodes 
		&& node_distance < 64 
		&& NodeVisible(bot->r.currentOrigin, nodes[bot->next_node].origin, bot->s.number) )
	{							// At the node...
		return ( qtrue );
	}

	if ( bot->next_node >= 0 
		&& bot->next_node < number_of_nodes 
		&& node_distance < 256 
		&& nodes[touch_node].type == NODE_ICE
		&& NodeVisible(bot->r.currentOrigin, nodes[bot->next_node].origin, bot->s.number) )
	{							// At the node...
		return ( qtrue );
	}

	if ( node_distance < 16 )
	{							// At the node...
		return ( qtrue );
	}

/*#ifdef __VEHICLES__
	if (client && (client->ps.eFlags & EF_VEHICLE))
	{
		if (node_distance < 96 )
		{
			return qtrue;
		}
	}
#endif //__VEHICLES__*/
	return ( qfalse );
}

extern vmCvar_t bot_minplayers;
#ifdef __VEHICLES__
extern void		AIMOD_MOVEMENT_VehicleMove ( gentity_t *bot, usercmd_t *ucmd );
#endif //__VEHICLES__

void AIMOD_MOVEMENT_SetupStealth ( gentity_t *bot, usercmd_t *ucmd )
{
	if ( (bot->client->ps.eFlags & EF_CLOAKED) )
	{
		if ( !(bot->client->ps.eFlags & EF_PRONE) && !(bot->client->ps.pm_flags & PMF_DUCKED) )
		{// We can stealth! Do it!
			//bot->client->ps.eFlags |= EF_CROUCHING;
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;
		}
	}
}

vmCvar_t bot_cpu_use;

void AIMOD_MOVEMENT_Explosive_Checks ( void )
{// Avoid explosive devices...
	int i;
	int current_time = level.time;

	// Avoid running this loop too often... Every .5 secs should be ok.
	if (next_explosive_check > current_time)
		return;

	next_explosive_check = current_time + 100;

	for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++)
	{
		gentity_t	*ent = &g_entities[i];
		int			j;

		if (!ent)
			continue;

		if (ent->freetime == current_time)
			continue;

		if (!ent->classname || !ent->classname[0] || strlen(ent->classname) <= 0)
			continue;

		if (!(!Q_stricmp( "weapon_dynamite", ent->classname)
			|| !Q_stricmp( "dynamite", ent->classname)
			|| !Q_stricmp( "grenade", ent->classname)
			|| !Q_stricmp( "smoke_bomb", ent->classname)
			|| !Q_stricmp( "gpg40_grenade", ent->classname)
			|| !Q_stricmp( "m7_grenade", ent->classname)
			|| !Q_stricmp( "mortar_grenade", ent->classname)
			//|| !Q_stricmp( "landmine", ent->classname)
			|| !Q_stricmp( "props_explosion", ent->classname)
			|| !Q_stricmp( "air strike", ent->classname)
			|| !Q_stricmp( "satchel_charge", ent->classname))
			&& !ent->will_explode)
			continue;

		// UQ1: Fix me.. For teams...
//		if (!Q_stricmp( "landmine", ent->classname)
//			&& ent->s.teamNum != client->sess.sessionTeam + 4

//		if (!ent->will_explode 
//			&& !( !Q_stricmp( "weapon_dynamite", ent->classname) || !Q_stricmp( "dynamite", ent->classname) ))
//			continue;

		if ((!Q_stricmp( "weapon_dynamite", ent->classname) || !Q_stricmp( "dynamite", ent->classname )) 
			&& !(ent->s.teamNum < 4) )
			continue; // Dynamite is not set!

		for (j = 0; j < number_of_nodes; j++)
		{
			if (ent->splashRadius && ent->splashRadius >= 64)
			{
				if (VectorDistance(nodes[j].origin, ent->r.currentOrigin) < ent->splashRadius * 1.5)
				{
					avoid_node_timer[j] = current_time + 5000;
					//G_Printf("WP %i set as explosive!\n", j);
				}
			}
			else
			{
				if (VectorDistance(nodes[j].origin, ent->r.currentOrigin) < 768)
				{
					avoid_node_timer[j] = current_time + 5000;
					//G_Printf("WP %i set as explosive!\n", j);
				}
			}
		}
	}
}

qboolean AIMOD_MOVEMENT_Avoid_Explosives ( gentity_t *bot, usercmd_t *ucmd )
{// UQ1: Now returns qtrue when we are at the cover spot and waiting! Otherwise qfalse to continue movement to avoid goal, or normal movement!
	if (!bot || !bot->client)
		return qfalse;

	if (bot->current_node < 0)
		return qfalse;

	if (bot->bot_explosive_avoid_waypoint != bot->longTermGoal)
	{// Init avoid waypoint!
		bot->bot_explosive_avoid_waypoint = -1;
		bot->bot_explosive_avoid_waypoint_avoiding = -1;
	}

	if (bot->bot_explosive_avoid_waypoint_avoiding >= 0
		&& !AIMOD_MOVEMENT_Explosive_Near_Waypoint(bot->bot_explosive_avoid_waypoint_avoiding))
	{// Explosion has gone off, continue as normal!
		bot->bot_explosive_avoid_waypoint = -1;
		bot->bot_explosive_avoid_waypoint_avoiding = -1;
		return qfalse;
	}

	if (bot->current_node && bot->current_node == bot->bot_explosive_avoid_waypoint)
	{
		qboolean onLadder = qfalse;

		if (bot->client->ps.serverCursorHint == HINT_LADDER ||
			bot->client->ps.torsoAnim == BOTH_CLIMB ||
			bot->client->ps.legsAnim == BOTH_CLIMB ||
			bot->client->ps.torsoAnim == BOTH_CLIMB_DOWN ||
			bot->client->ps.legsAnim == BOTH_CLIMB_DOWN ||
			(bot->client->ps.pm_flags & PMF_LADDER))
			onLadder = qtrue;

		if (AIMOD_MOVEMENT_WaypointTouched(bot, bot->current_node, onLadder))
		{// Get down and wait!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;

			if (!(bot->s.eFlags & EF_CROUCHING))
			{
				bot->s.eFlags |= EF_CROUCHING;
				ucmd->upmove = -48;
			}
			else
				ucmd->upmove = 0;

			return qtrue;
		}
	}

	if (!AIMOD_MOVEMENT_Explosive_Near_Waypoint(bot->current_node)
		&& !AIMOD_MOVEMENT_Explosive_Near_Waypoint(bot->next_node))
	{
		return qfalse;
	}

	if (bot->bot_explosive_avoid_waypoint < 0)
	{// Find a spot to run for!
		int		i;
		int		best = -1;
		float	best_dist = 9999.9f;

		bot->bot_explosive_avoid_waypoint_avoiding = bot->current_node;

		for (i = 0; i < number_of_nodes; i+=Q_TrueRand(1,10)/*i++*/)
		{
			float dist = VectorDistance(nodes[i].origin, bot->r.currentOrigin);

			if (dist > 256 && dist < best_dist)
			{
				if (/*(nodes[i].type & NODE_COVER) &&*/ !AIMOD_MOVEMENT_Explosive_Near_Waypoint(i))
				{
					best_dist = dist;
					best = i;
				}
			}
		}

		bot->bot_explosive_avoid_waypoint = best;

		if (bot->bot_explosive_avoid_waypoint)
		{// Create a new route!
#ifdef __VEHICLES__
			if (bot->client->ps.eFlags & EF_VEHICLE)
				bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_TANK, bot );
			else
#endif //__VEHICLES__
				bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );

			bot->longTermGoal = bot->bot_explosive_avoid_waypoint;
			bot->last_node = bot->current_node - 1;

			if (Q_TrueRand(0,2) == 1)
			{// Use alt route if possible for every 2nd (randomized) pathfind...
				bot->pathsize = CreateAltPathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			}
			else
			{
				bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			}

			if (bot->pathsize)
			{
				bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
				bot->node_timeout = level.time + (TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			}
		}
	}

	return qfalse;
}

qboolean AIMod_Is_Avoiding_Explosive ( gentity_t *bot )
{
	if (bot->bot_explosive_avoid_waypoint >= 0)
		return qtrue;

	return qfalse;
}

extern int	num_bot_objectives;
extern int	BotObjectives[MAX_GENTITIES];
extern int	num_bot_goals;
extern int	BotGoals[MAX_GENTITIES];

int
AIMod_CountNodeReachableGoals ( gentity_t *bot, int node )
{
//	/*short*/ int	pathlist[MAX_NODES];
	int			pathsize;
	int			i;
	int			total_found = 0;

	for ( i = 0; i < num_bot_objectives; i++ )
	{
		gentity_t		*goal = &g_entities[BotObjectives[i]];
		int				goal_node = AIMOD_NAVIGATION_FindClosestReachableNode( goal, NODE_DENSITY, NODEFIND_ALL, bot );
		pathsize = CreatePathAStar( bot, node, goal_node, bot->pathlist );

		if (pathsize > 0)
			total_found++;
	}

	for ( i = 0; i < num_bot_goals; i++ )
	{
		gentity_t		*goal = &g_entities[BotGoals[i]];
		int				goal_node = AIMOD_NAVIGATION_FindClosestReachableNode( goal, NODE_DENSITY, NODEFIND_ALL, bot );
		pathsize = CreatePathAStar( bot, node, goal_node, bot->pathlist );

		if (pathsize > 0)
			total_found++;
	}

	return total_found;
}

int next_route_create = 0;

void AIMOD_MOVEMENT_Check_CreateRoute ( gentity_t *bot )
{// Create a new waypoint path when there is no choice!
/*	int i;

	//if (bot->current_node < 0 || bot->current_node > number_of_nodes)
	//	return;

	if (next_route_create > level.time)
		return;

	next_route_create = level.time + 2000;

	if (bot->current_node < 0 || bot->current_node > number_of_nodes)
	{// Make one where we are standing!
		for (i = 0; i < number_of_nodes; i++)
		{
			if (VectorDistance(nodes[i].origin, bot->r.currentOrigin) < 256)
			{
				vec3_t upNode;

				VectorCopy(nodes[i].origin, upNode);
				upNode[2]+=48;

				if (NodeVisible(upNode, bot->r.currentOrigin, bot->s.number))
				{
					bot->current_node = i;
					break;
				}
			}
		}

		if (bot->current_node < 0 || bot->current_node > number_of_nodes)
		{
			short int objNum[3] = {0, 0, 0};

			Load_AddNode( bot->r.currentOrigin, 0, objNum, 0 );	//add the node

			AIMOD_MAPPING_CreateNodeLinks( number_of_nodes-1 );

			// Also check if the node needs special flags...
			AIMOD_MAPPING_CreateSpecialNodeFlags( number_of_nodes-1 );
		}

		//if (nodes[number_of_nodes-1].enodenum > 0 && AIMod_CountNodeReachableGoals( bot, bot->current_node ) > 4)
		//	return; // This is ok, we have links already... Use them...
	}
	else if (AIMod_CountNodeReachableGoals( bot, bot->current_node ) > 4)
		return; // Have at least 4 already..

	for (i = 0; i < number_of_nodes; i++)
	{
		if (VectorDistance(nodes[i].origin, nodes[bot->current_node].origin) < 2048 
			&& VectorDistance(nodes[i].origin, nodes[bot->current_node].origin) > 512
			&& nodes[i].origin[2] <= nodes[bot->current_node].origin[2]-48)
		{
			vec3_t upNode;

			VectorCopy(nodes[i].origin, upNode);
			upNode[2]+=48;

			if (NodeVisible(upNode, nodes[bot->current_node].origin, bot->s.number))
			{
				qboolean	add = qtrue;
				int			size = CreatePathAStar(bot, bot->current_node, i, NULL);
				vec3_t		current_org;
				int			start_node = number_of_nodes;
				int			j;

				if (size > 0)
					continue;

				if (AIMod_CountNodeReachableGoals( bot, i ) < 2 )
					continue;

				VectorCopy(nodes[bot->current_node].origin, current_org);

				// Found one!
				while (add)
				{// Create the route...
					vec3_t dir, angles, forward, origin;
					short int objNum[3] = {0, 0, 0};

					VectorCopy(current_org, origin);
					
					VectorSubtract( nodes[i].origin, nodes[bot->current_node].origin, dir );
					vectoangles( dir, angles );
					
					AngleVectors (angles, forward, NULL, NULL);
					VectorMA( origin, 64, forward, origin );
					
					Load_AddNode( origin, 0, objNum, 0 );	//add the node

					if (VectorDistance(origin, nodes[i].origin) < 64)
					{
						add = qfalse;
						break;
					}

					VectorCopy(origin, current_org);
				}

				// Make the link from our current node...
				if (nodes[bot->current_node].enodenum >= MAX_NODELINKS-1)
				{
					nodes[bot->current_node].links[MAX_NODELINKS-1].targetNode = start_node;
					nodes[bot->current_node].links[MAX_NODELINKS-1].cost = VectorDistance(nodes[bot->current_node].origin, nodes[start_node].origin);
					nodes[bot->current_node].links[MAX_NODELINKS-1].flags = 0;
				}
				else
				{
					nodes[bot->current_node].links[nodes[bot->current_node].enodenum].targetNode = start_node;
					nodes[bot->current_node].links[nodes[bot->current_node].enodenum].cost = VectorDistance(nodes[bot->current_node].origin, nodes[start_node].origin);
					nodes[bot->current_node].links[nodes[bot->current_node].enodenum].flags = 0;
				}

				for (j = start_node; j < number_of_nodes; j++)
				{// Link the new nodes in...
					AIMOD_MAPPING_CreateNodeLinks( j );

					// Also check if the node needs special flags...
					AIMOD_MAPPING_CreateSpecialNodeFlags( j );
				}

				// Done.. exit!
				//break;
			}
		}
	}*/
}

extern int GoalType ( int entNum, int teamNum );

int FlagsForNodeLink ( int from, int to )
{
	int loop;
	
	for ( loop = 0; loop < nodes[from].enodenum; loop++ )
	{													// Find the link we last used and mark it...
		if ( nodes[from].links[loop].targetNode == to )
		{												// Mark it as blocked...
			return nodes[from].links[loop].flags;
			break;
		}
	}

	return -1;
}

qboolean IgnoreTraceEntType ( int ent_type )
{
	switch (ent_type)
		{
		case ET_PLAYER:
		case ET_NPC:
		case ET_MOVER:
		case ET_ITEM:
		case ET_PORTAL:
//		case ET_PUSH_TRIGGER:
		case ET_TELEPORT_TRIGGER:
		case ET_INVISIBLE:
		case ET_OID_TRIGGER:
		case ET_EXPLOSIVE_INDICATOR:
		case ET_EXPLOSIVE:
//		case ET_EF_SPOTLIGHT:
//		case ET_ALARMBOX:
		case ET_MOVERSCALED:
		case ET_CONSTRUCTIBLE_INDICATOR:
		case ET_CONSTRUCTIBLE:
		case ET_CONSTRUCTIBLE_MARKER:
		case ET_WAYPOINT:
		case ET_TANK_INDICATOR:
		case ET_TANK_INDICATOR_DEAD:
		case ET_BOTGOAL_INDICATOR:
		case ET_CORPSE:
		case ET_SMOKER:
		case ET_TRIGGER_MULTIPLE:
		case ET_TRIGGER_FLAGONLY:
		case ET_TRIGGER_FLAGONLY_MULTIPLE:
//		case ET_CABINET_H:
//		case ET_CABINET_A:
		case ET_HEALER:
		case ET_SUPPLIER:
		case ET_COMMANDMAP_MARKER:
		case ET_WOLF_OBJECTIVE:
		case ET_SECONDARY:
		case ET_FLAG:
		case ET_ASSOCIATED_SPAWNAREA:
		case ET_UNASSOCIATED_SPAWNAREA:
//		case ET_AMMO_CRATE:
//		case ET_HEALTH_CRATE:
		case ET_CONSTRUCTIBLE_SANDBAGS:
		case ET_CONSTRUCTIBLE_MG_NEST:
#ifdef __VEHICLES__
		case ET_VEHICLE:
#endif //__VEHICLES__
		case ET_PARTICLE_SYSTEM:
			return ( qtrue );
		default:
			break;
		}

	return qfalse;
}

void AIMOD_MOVEMENT_FindAmmo ( gentity_t *bot, usercmd_t *ucmd )
{// This bot needs to find some more ammo! Set it up if required here!
	//if (!(bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->sess.playerWeapon)] < GetAmmoTableData( BG_FindAmmoForWeapon(bot->client->sess.playerWeapon) )->maxammo * 0.75))
	if (bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->sess.playerWeapon)] != 0)
	{// We already have enough ammo!
		bot->goalentity = NULL;
		bot->longTermGoal = -1;
		bot->current_node = -1;
		bot->bot_after_ammo = qfalse;
	}
	else if (!bot->bot_after_ammo)
	{// Init waypoints to find a new route to ammo this frame!
		//G_Printf("Screwed waypointing at 9\n");
		bot->goalentity = NULL;
		bot->longTermGoal = -1;
		bot->current_node = -1;
		bot->bot_after_ammo = qtrue;
	}
	else if (!bot->goalentity)
	{// Init waypoints because the ammo we were after no longer exists!
		//G_Printf("Screwed waypointing at 10\n");
		bot->goalentity = NULL;
		bot->longTermGoal = -1;
		bot->current_node = -1;
		bot->bot_after_ammo = qtrue;
	}
	else if (!IsAmmoEntity(bot->goalentity) && bot->bot_after_ammo)
	{
		bot->goalentity = NULL;
		bot->longTermGoal = -1;
		bot->current_node = -1;
		bot->bot_after_ammo = qfalse;
	}
}

float AIMod_Find_Ladder_Center_Angle ( gentity_t *bot )
{
	vec3_t			org1, org2, forward, right, up, testangles;
	int				i = 0;
	int				start_angle, end_angle;
	vec3_t			org, aw_best_ladder_spot;

	VectorCopy(bot->r.currentOrigin, org);

	start_angle = -1;
	end_angle = -1;

	VectorCopy(org, aw_best_ladder_spot);
	VectorCopy(org, org1);
	
	org1[2]+=16;

	for (i = 0; i < 361; i++)
	{
		trace_t		trace;

		VectorCopy(aw_best_ladder_spot, org2);

		testangles[0] = testangles[1] = testangles[2] = 0;
		testangles[YAW] = (float)i;
		AngleVectors( testangles, forward, right, up );
		VectorMA ( org2, 64 , forward , org2);
		org2[2]+=16;

		trap_Trace( &trace, org1, NULL, NULL, org2, bot->s.number, MASK_PLAYERSOLID );
		
		if (trace.surfaceFlags & SURF_LADDER)
		{// Ladder found here! Make the waypoints!
			if (start_angle == -1)
			{
				start_angle = i;
				end_angle = i;
			}
			else
			{
				end_angle = i;
			}
		}
		else
		{
			if (start_angle >= 0)
				break; // Found the end of the ladder...
		}
	}

	if (start_angle == 0)
	{// Scan negative direction for the start of the ladder!
		for (i = -90; i < 0; i++)
		{
			trace_t		trace;

			VectorCopy(aw_best_ladder_spot, org2);

			testangles[0] = testangles[1] = testangles[2] = 0;
			testangles[YAW] = (float)i;
			AngleVectors( testangles, forward, right, up );
			VectorMA ( org2, 64 , forward , org2);
			org2[2]+=16;

			trap_Trace( &trace, org1, NULL, NULL, org2, bot->s.number, MASK_PLAYERSOLID );
		
			if (trace.surfaceFlags & SURF_LADDER)
			{// Ladder found here! Make the waypoints!
				start_angle = i;
				break;
			}
		}
	}

	if (end_angle == 360 || end_angle == 361)
	{// Scan positive direction for the end of the ladder!
		for (i = 361; i < 450; i++)
		{
			trace_t		trace;

			VectorCopy(aw_best_ladder_spot, org2);

			testangles[0] = testangles[1] = testangles[2] = 0;
			testangles[YAW] = (float)i;
			AngleVectors( testangles, forward, right, up );
			VectorMA ( org2, 64 , forward , org2);
			org2[2]+=16;

			trap_Trace( &trace, org1, NULL, NULL, org2, bot->s.number, MASK_PLAYERSOLID );
		
			if (trace.surfaceFlags & SURF_LADDER)
			{// Ladder found here! Make the waypoints!
				end_angle = i;
			}
		}
	}

	if (start_angle != -1 && end_angle != -1 
		/*&& ( end_angle - start_angle >= 30 )*/ )
	{// We found one... Find the center angle and use it (hopefully the most central part of the ladder!)
		int			middle_angle = ((end_angle - start_angle) * 0.5) + start_angle;
		//G_Printf("ladder start_angle was %i. end_angle was %i. middle_angle is %i.\n", start_angle, end_angle, middle_angle);
		return middle_angle;
	}
	/*else
	{
		G_Printf("Failed to find ladder's center!\n");
	}*/

	return -1;
}

qboolean AIMOD_MOVEMENT_Is_On_Ladder ( gentity_t *bot )
{
	if (!bot)
		return qfalse;

	if (!bot->client)
		return qfalse;

	if (bot->client->ps.serverCursorHint == HINT_LADDER ||
		bot->client->ps.torsoAnim == BOTH_CLIMB ||
		bot->client->ps.legsAnim == BOTH_CLIMB ||
		bot->client->ps.torsoAnim == BOTH_CLIMB_DOWN ||
		bot->client->ps.legsAnim == BOTH_CLIMB_DOWN ||
		(bot->client->ps.pm_flags & PMF_LADDER) ||
		bot->bot_ladder_time > level.time)
		return qtrue;

	return qfalse;
}

qboolean AIMOD_MOVEMENT_Is_Swimming ( gentity_t *bot )
{
	if (!bot)
		return qfalse;

	if (!bot->client)
		return qfalse;

	if (bot->waterlevel)
		return qtrue;

	return qfalse;
}

qboolean AIMOD_MOVEMENT_Is_Sliding ( gentity_t *bot )
{
	if (!bot)
		return qfalse;

	if (!bot->client)
		return qfalse;

	if (AI_PM_SlickTrace( bot->r.currentOrigin, bot->s.number ))
		return qtrue;

	return qfalse;
}

qboolean AIMOD_MOVEMENT_Should_Not_Follow ( gentity_t *bot )
{
	if (AIMOD_MOVEMENT_Is_On_Ladder(bot) || AIMOD_MOVEMENT_Is_Swimming(bot) || AIMOD_MOVEMENT_Is_Sliding(bot))
	{
		return qtrue;
	}

	return qfalse;
}

qboolean AIMOD_MOVEMENT_Is_Link_From_Node ( int from, int to )
{
	int i;

	for (i = 0; i < nodes[from].enodenum; i++)
	{
		if (nodes[from].links[i].targetNode == to)
			return qtrue;
	}

	return qfalse;
}

extern float FloorHeightAt ( vec3_t org );
extern qboolean MyObjectVisible ( gentity_t *self, gentity_t *other );
extern qboolean MyInFOV2 ( vec3_t origin, gentity_t *from, int hFOV, int vFOV );
extern vec3_t	playerMins;
extern vec3_t	playerMins;

extern float BOT_FloorHeightAt ( gentity_t *bot );
extern stringID_table_t WeaponTableStrings[];

qboolean
AIMOD_MOVEMENT_SniperFunc ( gentity_t *bot, usercmd_t *ucmd, qboolean avoiding_explosives )
{// Return qtrue if move func needs to exit too...
	if (!BG_IsScopedWeapon(bot->s.weapon) && !BG_IsScopedWeapon2(bot->s.weapon))
		return qfalse; // not a sniper, continue on moving to another goal...

#ifndef __NO_SNIPER_POINTS__
	if ( nodes_loaded 
		&& bot->goalentity 
		&& !Q_stricmp( bot->goalentity->classname, va( "misc_stealth_area"))
		&& VectorDistance( nodes[bot->longTermGoal].origin, bot->r.currentOrigin) <= 64 )
	{// A sniper... Make him look around in snipe mode for enemies...
		if ( test_last_message[bot->s.number] < level.time - 3000 )
		{
			if (bot_debug.integer)
				G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^7 has found his stealth point (weapon: %s).\n", bot->client->pers.netname, GetStringForID(WeaponTableStrings, bot->s.weapon) );

			test_last_message[bot->s.number] = level.time;
		}

		if (BG_IsScopedWeapon2(bot->s.weapon))
		{// Make sure we have switched to our scope!
			//if (bot_debug.integer)
			//	G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^7 switching to his scope. (weapon: %s).\n", bot->client->pers.netname, GetStringForID(WeaponTableStrings, bot->s.weapon) );

			if (bot->bot_reload_time < level.time)
			{
				ucmd->weapon = getAltWeapon(bot->s.weapon);
				bot->next_fire_time = level.time + 3500;
			}
		}

		AIMOD_MOVEMENT_SetupStealth( bot, ucmd );

		bot->bot_last_real_move_time = level.time;
		bot->node_timeout = level.time + 5000;
		bot->bot_last_good_move_time = level.time + 5000;

		AIMOD_Movement_SniperMove( bot, ucmd, avoiding_explosives );

		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		//ucmd->upmove = 0;

		return qtrue;
	}
	else if ( nodes_loaded 
		&& bot->goalentity 
		&& !Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot"))
		&& VectorDistance( nodes[bot->longTermGoal].origin, bot->r.currentOrigin) <= 128/*64*//*128*/ )
	{// A sniper... Make him look around in snipe mode for enemies...
		if ( test_last_message[bot->s.number] < level.time - 3000 )
		{
			if (bot_debug.integer)
				G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^7 has found his sniper point (weapon: %s).\n", 
					bot->client->pers.netname, GetStringForID(WeaponTableStrings, bot->s.weapon) );

			test_last_message[bot->s.number] = level.time;
		}

		if (BG_IsScopedWeapon2(bot->s.weapon))
		{// Make sure we have switched to our scope!
			//if (bot_debug.integer)
			//	G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^7 switching to his scope. (weapon: %s).\n", bot->client->pers.netname, GetStringForID(WeaponTableStrings, bot->s.weapon) );

			if (bot->bot_reload_time < level.time)
			{
				ucmd->weapon = getAltWeapon(bot->s.weapon);
				bot->next_fire_time = level.time + 3500;
			}
		}

		bot->bot_last_real_move_time = level.time;
		bot->node_timeout = level.time + 5000;
		bot->bot_last_good_move_time = level.time + 5000;

		AIMOD_Movement_SniperMove( bot, ucmd, avoiding_explosives );

		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		return qtrue;
	}
	else if ( bot->enemy 
		&& AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy )
		&& VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 600 )
	{// A sniper with an enemy at range from it... Shoot with scope...
		if ( test_last_message[bot->s.number] < level.time - 3000 )
		{
			if (bot_debug.integer)
				G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^7 has found his sniper point (weapon: %s).\n", 
					bot->client->pers.netname, GetStringForID(WeaponTableStrings, bot->s.weapon) );

			test_last_message[bot->s.number] = level.time;
		}

		if (BG_IsScopedWeapon2(bot->s.weapon))
		{// Make sure we have switched to our scope!
			//if (bot_debug.integer)
			//	G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^7 switching to his scope. (weapon: %s).\n", bot->client->pers.netname, GetStringForID(WeaponTableStrings, bot->s.weapon) );

			if (bot->bot_reload_time < level.time)
			{
				ucmd->weapon = getAltWeapon(bot->s.weapon);
				bot->next_fire_time = level.time + 3500;
			}
		}

		bot->bot_last_real_move_time = level.time;
		bot->node_timeout = level.time + 5000;
		bot->bot_last_good_move_time = level.time + 5000;

		AIMOD_Movement_SniperMove( bot, ucmd, avoiding_explosives );

		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		return qtrue;
	}
	else if ( nodes_loaded && bot->goalentity && !Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot")) )
	{
		if ( test_last_message[bot->s.number] < level.time - 2*VectorDistance( nodes[bot->longTermGoal].origin, bot->r.currentOrigin)/*3000*/ )
		{
			if (bot_debug.integer)
				G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^5's distance from his sniper point is ^7%f^5 (weapon %s).\n",
					  bot->client->pers.netname, VectorDistance( nodes[bot->longTermGoal].origin, bot->r.currentOrigin), GetStringForID(WeaponTableStrings, bot->s.weapon) );
			test_last_message[bot->s.number] = level.time;
		}

		if (bot->node_timeout <= level.time)
		{
#ifdef _WIN32
			if (bot_debug.integer && bot->goalentity 
				&& !Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot"))
				&& VectorDistance(bot->r.currentOrigin, bot->goalentity->s.origin) < 512)
				G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^7 queued a new path! - distance to goal was %f. (2)\n", bot->client->pers.netname, VectorDistance(bot->r.currentOrigin, bot->goalentity->s.origin) );

			Bot_Queue_New_Path( bot, ucmd );

			bot->node_timeout = level.time + (TravelTime( bot )*4);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
#else //!_WIN32
			Bot_Set_New_Path( bot, ucmd );
			
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

			if (bot->current_node < 0 || bot->current_node > number_of_nodes || bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes)
			{
				bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
			}
#endif //_WIN32

			bot->bot_snipetime = 0;
		}
	}
#endif //__NO_SNIPER_POINTS__

	return qfalse;
}


void
AIMOD_MOVEMENT_Move ( gentity_t *bot, usercmd_t *ucmd )
{
	qboolean	onLadder = qfalse;
	qboolean	new_follow_route = qfalse;
	qboolean	do_attack_move = qfalse;
	qboolean	force_jump = qfalse;
	qboolean	force_crouch = qfalse;
	qboolean	avoiding_explosives = qfalse;
	qboolean	inFOV = qfalse;
	int			node_link_flags = -1;

	trap_Cvar_Register( &bot_debug, "bot_debug", "0", 0 );

	G_BotCheckForCursorHints( bot );

	if (level.time < bot->spawnTime + 2000)
	{// Crouch and move forward at spawn to clear the spawnpoint and stop us getting stuck in things...
		ucmd->upmove = -48;								// Crouch...
		bot->client->ps.eFlags |= EF_CROUCHING;
		ucmd->forwardmove = 127;
		ucmd->rightmove = Q_irand(-127, 127);
		return;
	}

#ifndef __NO_SNIPER_POINTS__
	if (bot->bot_snipetime >= level.time || (nodes_loaded && bot->goalentity && !Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot"))))
	{
		if ( AIMOD_MOVEMENT_SniperFunc( bot, ucmd, avoiding_explosives ) )
			return;
	}
	else
	{// This will force them to switch away from scope when moving...
		AIMOD_Movement_SniperCheckReload( bot, ucmd, avoiding_explosives );
	}
#endif //__NO_SNIPER_POINTS__

	if (AIMod_Is_Avoiding_Explosive(bot))
	{

	}
	else if (bot->bot_coverspot_enemy)
	{

	}
	else if ( bot->client->sess.playerType == PC_ENGINEER || bot->client->sess.playerType == PC_COVERTOPS )
	{														// Soldiers shouldn't need special moves...
		if ( AIMOD_MOVEMENT_SpecialMoves( bot, ucmd) )
		{
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			return;
		}
	}

	if (AIMod_Is_Avoiding_Explosive(bot))
	{

	}
	else if ( bot->beStill > level.time || bot->beStillEng > level.time || bot->bot_job_wait_time >= level.time )
	{
		// Got something to do! Turret or tank gun?
		bot->needed_node = -1;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		return;
	}

	/*if ( g_gametype.integer <= GT_COOP ||  g_gametype.integer == GT_NEWCOOP )
	{
		AIMOD_SP_MOVEMENT_Move( bot, ucmd );
		AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
		return;
	}*/

	if ( AIMOD_MOVEMENT_Fallback( bot, ucmd) )
	{
		bot->node_timeout = level.time + (TravelTime( bot )*2);
		bot->bot_last_good_move_time = level.time;
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		return;
	}
/*
	if (bot->bot_last_real_move_time < level.time - 45000
		&& bot->bot_last_attack_time < level.time - 45000)
	{// Have not been able to move in the last 45 seconds, probebly stuck in a solid... Suicide!
		bot->bot_creating_path = qfalse;
		bot->client->ps.stats[STAT_HEALTH] = bot->health = 0;
		// TTimo - if using /kill while at MG42
		bot->client->ps.persistant[PERS_HWEAPON_USE] = 0;
		player_die(bot, bot, bot, (g_gamestate.integer == GS_PLAYING) ? 100000 : 135, MOD_SUICIDE);
		return;
	}*/

	if (bot->bot_random_move_time > level.time 
		&& !AIMod_Is_Avoiding_Explosive(bot)
		&& !(bot->current_node >= 0 && bot->longTermGoal >= 0 && bot->pathsize > 0 && bot->bot_creating_path == qfalse))
	{
		//AIMOD_MOVEMENT_Backup( bot, ucmd );
		//AIMOD_MOVEMENT_Backup2( bot, ucmd );

		// Initialize move destination...
		if (!bot->bot_initialized)
		{
			bot->bot_initialized = qtrue;

			VectorCopy(bot->r.currentOrigin, bot->bot_sp_spawnpos);
		}

		AIMOD_SP_MOVEMENT_Move( bot, ucmd );
		/*bot->node_timeout = level.time + (TravelTime( bot )*2);
		bot->bot_last_good_move_time = level.time;
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );*/

		if (bot->bot_last_position_check < level.time - 2000)
		{
			if (VectorDistance(bot->r.currentOrigin, bot->bot_last_position) < 8)
			{
				bot->node_timeout = level.time + (TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
				return;
			}
			else
			{
				bot->bot_last_real_move_time = level.time;
			}

			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			bot->bot_last_position_check = level.time;
		}
		return;
	}
	else
	{
		bot->bot_initialized = qfalse;
	}

	//
	// Ammo/reload checks...
	//

	switch (bot->client->ps.weapon)
	{
	case WP_KNIFE:
	case WP_LUGER:
	case WP_COLT:
	case WP_SILENCED_LUGER:
	case WP_SILENCED_COLT:
	case WP_AKIMBO_SILENCEDCOLT:
	case WP_AKIMBO_SILENCEDLUGER:
	case WP_AKIMBO_COLT:
	case WP_AKIMBO_LUGER:
		if (!bot->bot_after_ammo)
		{// Find a new route to ammo!
			//if (bot_debug.integer)
			//	G_Printf("Bot needed new ammo (2)!\n");
			bot->bot_after_ammo = qtrue;
			bot->current_node = -1;
			bot->next_node = -1;
			bot->last_node = -1;
			bot->longTermGoal = -1;
			bot->goalentity = NULL;
		}
		break;
	default:
		break;
	}

	if (bot->client->ps.weapon > WP_MAX_PRIMARY)
	{// Check ammo!
		if (!bot->bot_after_ammo
			&& bot->client->ps.ammoclip[BG_FindClipForWeapon(bot->client->ps.weapon)] == 0
			&& bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->ps.weapon)] == 0)
		{// Need more ammo!
			//if (bot_debug.integer)
			//	G_Printf("Bot needed more ammo!\n");

			bot->bot_after_ammo = qtrue;
			bot->current_node = -1;
			bot->next_node = -1;
			bot->last_node = -1;
			bot->longTermGoal = -1;
			bot->goalentity = NULL;
		}
	}

	if (bot->client->ps.ammoclip[BG_FindClipForWeapon(bot->client->ps.weapon)] == 0
		&& bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->ps.weapon)] > 0)
	{
		ucmd->wbuttons = WBUTTON_RELOAD;
	}

	//
	// End of ammo/reload checks...
	//
/*
	if (bot->current_node && VectorLength(bot->client->ps.velocity) == 0)
	{// Check if stuck in a solid!
		if (bot->bot_last_good_velocity_time < level.time - 5000)
		{
			trace_t tr;
			vec3_t org, mins, maxs;

			VectorCopy(bot->r.currentOrigin, org);
			org[2]+=18;

			VectorCopy(playerMins, mins);
			VectorCopy(playerMaxs, maxs);

			trap_Trace( &tr, org, mins, maxs, org, bot->s.number, CONTENTS_SOLID | CONTENTS_PLAYERCLIP );

			if (tr.startsolid || tr.allsolid)
			{// OK. Bot is stuck in a solid... Move him to his node!
				TeleportPlayer( bot, nodes[bot->current_node].origin, bot->client->ps.viewangles );
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;
			}
		}
	}
	else
	{
		bot->bot_last_good_velocity_time = level.time;
	}*/

#ifdef __WEAPON_AIM__
	if (bot->followtarget && bot->followtarget->health <= 0 && bot->client->sess.playerType == PC_MEDIC)
	{

	}
	else if ( bot->client->ps.eFlags & EF_AIMING )
	{
		ucmd->buttons |= BUTTON_WALKING;
	}
	else if (AI_PM_SlickTrace( bot->r.currentOrigin, bot->s.number ))
	{// Always walk on ice!
		if (BG_Weapon_Is_Aimable(bot->client->ps.weapon))
			bot->client->ps.eFlags |= EF_AIMING;

		ucmd->buttons |= BUTTON_WALKING;
	}
	else
	{
		ucmd->buttons &= ~BUTTON_WALKING;
	}
#else //!__WEAPON_AIM__
	if (AI_PM_SlickTrace( bot->r.currentOrigin, bot->s.number ))
		ucmd->buttons |= BUTTON_WALKING;
#endif //__WEAPON_AIM__

	if (AIMOD_MOVEMENT_Is_On_Ladder(bot))
	{
		qboolean going_down = qfalse;

		if (bot->client->ps.serverCursorHint == HINT_LADDER ||
			bot->client->ps.torsoAnim == BOTH_CLIMB ||
			bot->client->ps.legsAnim == BOTH_CLIMB ||
			bot->client->ps.torsoAnim == BOTH_CLIMB_DOWN ||
			bot->client->ps.legsAnim == BOTH_CLIMB_DOWN ||
			(bot->client->ps.pm_flags & PMF_LADDER))
			bot->bot_ladder_time = level.time + 500;

		onLadder = qtrue;

		if ( bot->current_node < 0 
			|| bot->longTermGoal < 0 
			|| bot->current_node > number_of_nodes 
			|| bot->longTermGoal > number_of_nodes )
		{
			/*G_Printf("Screwed at 2. ");

			if (bot->current_node < 0)
				G_Printf("bot->current_node < 0\n");
			else if (bot->longTermGoal < 0)
				G_Printf("bot->longTermGoal < 0\n");
			else if (new_follow_route)
				G_Printf("new_follow_route\n");
			else if (bot->current_node > number_of_nodes)
				G_Printf("bot->current_node > number_of_nodes\n");
			else if (bot->longTermGoal > number_of_nodes)
				G_Printf("bot->longTermGoal > number_of_nodes\n");
			else 
				G_Printf("\n");*/

			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;

			if ( bot->enemy && bot->bot_enemy_visible_time > level.time /*&& bot->enemy == bot->bot_last_visible_enemy*/ && AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ) )
			{
				//AIMOD_MOVEMENT_Attack(bot, ucmd);
				AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
				bot->node_timeout = level.time + (TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
				return;
			}

#ifdef _WIN32
			if (bot_debug.integer && bot->goalentity 
				&& !Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot"))
				&& VectorDistance(bot->r.currentOrigin, bot->goalentity->s.origin) < 512)
				G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^7 queued a new path! - distance to goal was %f. (3)\n", bot->client->pers.netname, VectorDistance(bot->r.currentOrigin, bot->goalentity->s.origin) );

			Bot_Queue_New_Path( bot, ucmd );

			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			//bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
#else //!_WIN32
			Bot_Set_New_Path( bot, ucmd );
			
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

			if (bot->current_node < 0 || bot->current_node > number_of_nodes || bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes)
			{
				bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
				return;
			}
#endif //_WIN32
		}

		if (bot->current_node)
		{
			if (BOT_FloorHeightAt( bot ) < nodes[bot->current_node].origin[2])
			{// Next node is above our floor, must be going up!
				going_down = qfalse;
			}
			else
			{// Next node is below our floor, must be going down!
				going_down = qtrue;
			}
		}
		else if (bot->client->ps.torsoAnim == BOTH_CLIMB_DOWN || bot->client->ps.legsAnim == BOTH_CLIMB_DOWN)
		{
			going_down = qtrue;
		}
		else
		{
			going_down = qfalse;
		}

		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );

		BotSetViewAngles( bot, 100 );
		G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

		if (going_down)
		{
			ucmd->forwardmove = -127;
			ucmd->upmove = -127;

			if (VectorLength(bot->client->ps.velocity) < 24)
			{
				if (Q_TrueRand(0,1) == 1)
					ucmd->rightmove = 127;
				else
					ucmd->rightmove = -127;
			}
		}
		else
		{
			ucmd->forwardmove = 127;
			ucmd->upmove = 127;

			if (VectorLength(bot->client->ps.velocity) < 24)
			{
				if (Q_TrueRand(0,1) == 1)
					ucmd->rightmove = 127;
				else
					ucmd->rightmove = -127;
			}
		}

#ifndef __NO_SNIPER_POINTS__
		if ( AIMOD_MOVEMENT_SniperFunc( bot, ucmd, avoiding_explosives ) )
		{
			return;
		}
		else
		{// This will force them to switch away from scope when moving...
			AIMOD_Movement_SniperCheckReload( bot, ucmd, avoiding_explosives );
		}
#endif //__NO_SNIPER_POINTS__

		if (bot->bot_last_position_check < level.time - 2000/*1000*/)
		{
			if (VectorDistance(bot->r.currentOrigin, bot->bot_last_position) < 8/*24*/)
			{
				if (AIMOD_MOVEMENT_Is_Link_From_Node(bot->last_node, bot->current_node))
					AIMOD_MOVEMENT_IncreaseLinkageCost( bot );

				if (bot_debug.integer)
					G_Printf("Had to mark a node as bad!\n");

				bot->current_node = -1;
				bot->last_node = -1;
				bot->next_node = -1;
				bot->longTermGoal = -1;
				bot->bot_ladder_time = 0;
				bot->bot_no_ladder_use_time = level.time + 5000;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				bot->node_timeout = level.time + (TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
				return;
			}
			else
			{
				bot->bot_last_real_move_time = level.time;
			}

			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			bot->bot_last_position_check = level.time;
		}

		if ( bot->current_node && AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
		{	// At current node.. Pick next in the list...
			if ( bot->current_node == bot->longTermGoal )
			{// Hit our long term goal...
				//Bot_Set_New_Path( bot, ucmd );
				bot->current_node = -1;
				bot->longTermGoal = -1;
				bot->next_node = -1;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				bot->node_timeout = level.time + (TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

				if (bot_debug.integer)
				{
					G_Printf("BOT DEBUG: %s - hit his long term goal!\n", bot->client->pers.netname);
				}
				return;
			}
			else
			{// Continue on to next node...
				bot->last_node = bot->current_node;
				bot->current_node = bot->next_node;
				bot->next_node = BotGetNextNode( bot );
				bot->node_timeout = level.time + TravelTime( bot );

				if (bot->current_node > number_of_nodes)
				{
					if (bot_debug.integer)
						G_Printf("Screwed waypointing at 12\n");

					bot->current_node = -1;
					bot->next_node = -1;
					bot->longTermGoal = -1;
					bot->node_timeout = level.time + (TravelTime( bot )*2);
					bot->bot_last_good_move_time = level.time;
					bot->bot_last_position_check = level.time;
					VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
					return;
				}

				if ( nodes[bot->current_node].type & NODE_DUCK )
				{
					bot->node_timeout = level.time + ( TravelTime( bot) * 2 );	// Duck moves take longer, so...
				}

				// Set our last good move time to now...
				bot->node_timeout = level.time + (TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			}
		}

		bot->bot_last_good_move_time = level.time;
		return;
	}

	if ( bot->current_node >= 0 && bot->next_node >= 0 )
	{// Do some waypoint validity checking...
		if (bot->current_node >= number_of_nodes)
		{
			if (bot_debug.integer)
				G_Printf("Screwed waypointing at 13\n");

			bot->current_node = -1;
		}

		if (bot->next_node >= number_of_nodes)
		{
			bot->next_node = -1;
		}
		
		if (bot->current_node >= 0)
		{
			if (VectorDistance(nodes[bot->current_node].origin, bot->r.currentOrigin) > 1024)
			{// Obviously bad!
				if (bot_debug.integer)
					G_Printf("Screwed waypointing at 14\n");

				bot->current_node = -1;
				bot->next_node = -1;
			}
			// UQ1: OMG wtf was I thinking!
			/*else if (VectorDistance(nodes[bot->current_node].origin, bot->npc_ideal_view_angles) < VectorDistance(nodes[bot->current_node].origin, bot->r.currentOrigin))
			{// Fallen or become lost...
				if (bot_debug.integer)
					G_Printf("Bot has fallen or become lost...\n");

				bot->current_node = -1;
				bot->next_node = -1;
			}*/

			VectorCopy(bot->r.currentOrigin, bot->npc_ideal_view_angles);
		}
		else
		{
			VectorCopy(bot->r.currentOrigin, bot->npc_ideal_view_angles);
		}
	}
	else
	{
		VectorCopy(bot->r.currentOrigin, bot->npc_ideal_view_angles);
	}

#ifdef __BOT_SHORTEN_ROUTING__
	if ( nodes_loaded && bot->current_node > 0 && bot->pathsize > 10)
	{
		AIMOD_MOVEMENT_Shorten_Path( bot );
	}
#endif //__BOT_SHORTEN_ROUTING__

	if ( bot->client->ps.weapon == WP_MOBILE_MG42_SET )
	{
		return;
	}

#ifdef __EF__
	if ( bot->client->ps.weapon == WP_30CAL_SET )
	{
		return;
	}
#endif //__EF__

	if ( bot->health <= 0 )
	{
		return;
	}

	if ( bot->client->ps.pm_flags & PMF_LIMBO )
	{
		return;
	}

	if (AIMOD_MOVEMENT_Avoid_Explosives( bot, ucmd ))
		return; // Waiting for explosion at cover!

	if ( bot->current_node && AIMOD_MOVEMENT_Explosive_Near_Waypoint(bot->current_node))
	{// Avoiding an explosion, keep moving!
		avoiding_explosives = qtrue;
	}
#ifndef __BOT_EXPLOSIVE_AVOID_CPU_SAVER__
	else if ( AIMOD_MOVEMENT_Explosive_Near_Position( bot->r.currentOrigin ))
	{// Avoiding an explosion, keep moving!
		avoiding_explosives = qtrue;
	}
#endif //__BOT_EXPLOSIVE_AVOID_CPU_SAVER__

	if ( bot->current_node >= 0 && bot->current_node < number_of_nodes )
	{// Check our path for bad stuff... Mark it if found!
		trace_t			tr;
		vec3_t			org1, org2/*, angles, forward, dir*/;
//		float			floor = 0;

		VectorCopy(bot->r.currentOrigin, org1);
		VectorCopy(nodes[bot->current_node].origin, org2);

		org1[2]+=16;
		org2[2]+=16;

		trap_Trace( &tr, org1, NULL, NULL, org2, bot->s.number, /*MASK_SOLID |*/ MASK_WATER );

		if (tr.contents & CONTENTS_LAVA)
		{// It would hurt! Mark link as bad!
			int loop = 0;
			
			for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
			{													// Find the link we last used and mark it...
				if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
				{												// Mark it as blocked...
					BOT_MakeLinkSemiUnreachable( bot, bot->last_node, loop );
					break;
				}
			}

			if (!(trap_PointContents( bot->r.currentOrigin, bot->s.number ) & CONTENTS_LAVA))
			{// Not already being hurt, so find a new route before we get hurt!
				//G_Printf("Screwed waypointing at 15\n");
				bot->current_node = -1;
				bot->longTermGoal = -1;

				if (bot_debug.integer)
					G_Printf("BOT DEBUG: %s - Hit barb wire\n", bot->client->pers.netname);
			}
		}
		else
		{
			if (((tr.entityNum >= 0 && tr.entityNum < ENTITYNUM_MAX_NORMAL && g_entities[tr.entityNum].s.eType == ET_MOVER && g_entities[tr.entityNum].allowteams && (g_entities[tr.entityNum].allowteams & bot->client->sess.sessionTeam)) 
				|| tr.entityNum == ENTITYNUM_WORLD) && !(tr.surfaceFlags & SURF_LADDER))
			{// A useable door is in the way! That is ok!

			}
			else if (((tr.entityNum >= 0 && tr.entityNum < ENTITYNUM_MAX_NORMAL && !IgnoreTraceEntType(g_entities[tr.entityNum].s.eType)) 
				|| tr.entityNum == ENTITYNUM_WORLD) && !(tr.surfaceFlags & SURF_LADDER))
			{// Solid in the way. We can not continue!
				int loop = 0;

				//G_Printf("Screwed waypointing at 1\n");

				bot->current_node = -1;
				bot->longTermGoal = -1;

				if (bot_debug.integer)
					G_Printf("BOT DEBUG: %s - solid in the way\n", bot->client->pers.netname);

				for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
				{													// Find the link we last used and mark it...
					if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
					{												// Mark it with extreme cost!
						BOT_MakeLinkSemiUnreachable( bot, bot->last_node, loop );
						break;
					}
				}
			}
			else if (((tr.entityNum >= 0 && tr.entityNum < ENTITYNUM_MAX_NORMAL && IgnoreTraceEntType(g_entities[tr.entityNum].s.eType)) 
				|| tr.entityNum == ENTITYNUM_WORLD) && !(tr.surfaceFlags & SURF_LADDER))
			{// Movers/door/etc in the way! We can't continue!
				int loop = 0;

				//G_Printf("Screwed waypointing at 2\n");

				bot->current_node = -1;
				bot->longTermGoal = -1;

				if (bot_debug.integer)
					G_Printf("BOT DEBUG: %s - unusable mover/door/etc in path\n", bot->client->pers.netname);

				for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
				{													// Find the link we last used and mark it...
					if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
					{												// Mark it with extreme cost!
						BOT_MakeLinkSemiUnreachable( bot, bot->last_node, loop );
						break;
					}
				}
			}
#ifdef __BOT_FALL_CHECKING__
			else
			{// Check for falls!
				VectorSubtract(nodes[bot->current_node].origin, bot->r.currentOrigin, dir);
				vectoangles(dir, angles);
				AngleVectors (angles, forward, NULL, NULL);
				VectorMA( org2, 64,	forward,	org2 );
		
				floor = FloorHeightAt( org2 );

				while (floor == -65536.0f && org2[2] < bot->r.currentOrigin[2]+48)
				{// Start/End point in a solid!
					org2[2]+=8;
					floor = FloorHeightAt( org2 );
				}

				if (floor < bot->r.currentOrigin[2]-96)
				{// Don't move! We would fall!
					int loop = 0;

					for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
					{													// Find the link we last used and mark it...
						if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
						{												// Mark it as blocked...
							BOT_MakeLinkSemiUnreachable( bot, bot->last_node, loop );
							break;
						}
					}

					bot->current_node = -1;
					bot->longTermGoal = -1;

					if (bot_debug.integer)
						G_Printf("BOT DEBUG: %s - fall check\n", bot->client->pers.netname);
				}
			}
#endif //__BOT_FALL_CHECKING__
		}
	}

	if ( bot->bot_explosive_avoid_waypoint >= 0 || avoiding_explosives )
	{// Avoiding an explosion, keep moving!

	}
	else if (bot->bot_coverspot_enemy && bot->client->ps.weapon != WP_KNIFE)
	{

	}
	else if (bot->bot_explosive_avoid_time > level.time)
	{
		
	}
	else if (bot->bot_after_ammo && bot->goalentity && !Q_stricmp( bot->goalentity->classname, "misc_cabinet_supply") && bot->goalentity->count < 2)
	{// Not enough ammo left at this cabinet...
		if (bot_debug.integer)
			G_Printf("Screwed waypointing at 3\n");
		bot->current_node = -1;
		bot->longTermGoal = -1;
		bot->goalentity = NULL;
		bot->bot_after_ammo = qtrue;
	}
	/*else if (bot->client->sess.playerType != PC_FIELDOPS 
		&& (bot->client->ps.weapon == WP_KNIFE || bot->client->ps.weapon == WP_LUGER || bot->client->ps.weapon == WP_COLT || bot->client->ps.weapon == WP_SILENCED_COLT || bot->client->ps.weapon == WP_AKIMBO_SILENCEDCOLT || bot->client->ps.weapon == WP_AKIMBO_SILENCEDLUGER || bot->client->ps.weapon == WP_AKIMBO_COLT || bot->client->ps.weapon == WP_AKIMBO_LUGER || bot->client->ps.weapon == WP_SILENCED_LUGER))
	{// Need ammo??? Find some!
		if (!(bot->client->sess.playerWeapon == WP_PANZERFAUST &&  bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->sess.playerWeapon)] > 0) 
			&& !(bot->client->sess.playerWeapon == WP_FLAMETHROWER && bot->client->ps.ammoclip[BG_FindClipForWeapon(bot->client->sess.playerWeapon)] > GetAmmoTableData( BG_FindAmmoForWeapon(bot->client->sess.playerWeapon) )->maxclip * 0.15))
		{
			AIMOD_MOVEMENT_FindAmmo(bot, ucmd);

			if (bot->bot_after_ammo && bot->goalentity && IsAmmoEntity(bot->goalentity))
			{
				if (MyObjectVisible(bot, bot->goalentity))
				{
					VectorSubtract( bot->goalentity->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
					vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
					BotSetViewAngles( bot, 100 );
					G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
					AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
					bot->node_timeout = level.time + (TravelTime( bot )*2);
					bot->bot_last_good_move_time = level.time;
					bot->bot_last_position_check = level.time;
					VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
					return;
				}
			}
		}
	}*/
	else if (bot->client->sess.playerType != PC_FIELDOPS 
		&& bot->bot_after_ammo
		//&& bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->sess.playerWeapon)] < GetAmmoTableData( BG_FindAmmoForWeapon(bot->client->sess.playerWeapon) )->maxammo * 0.75)
		&& bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->sess.playerWeapon)] == 0)
	{// Need ammo??? Find some!
		AIMOD_MOVEMENT_FindAmmo(bot, ucmd);

		if (bot->bot_after_ammo && bot->goalentity && IsAmmoEntity(bot->goalentity))
		{
			if (MyObjectVisible(bot, bot->goalentity))
			{
				VectorSubtract( bot->goalentity->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
				BotSetViewAngles( bot, 100 );
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
				AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
				bot->node_timeout = level.time + (TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
				return;
			}
		}
	}
	else if (AIMOD_MOVEMENT_FireTeam_AI( bot, ucmd, new_follow_route, do_attack_move ))
	{// Check if we are using fireteam ai.. Skip the rest of the momement code in many cases...
		if (bot->bot_after_ammo && bot->goalentity && IsAmmoEntity(bot->goalentity))
		{
			bot->goalentity = NULL;
			bot->longTermGoal = -1;
			bot->current_node = -1;
			bot->bot_after_ammo = qfalse;
		}

		bot->bot_after_ammo = qfalse;
		return;
	}
	/*else if (bot->bot_after_ammo && bot->goalentity && IsAmmoEntity(bot->goalentity))
	{
		if (MyObjectVisible(bot, bot->goalentity))
		{
			VectorSubtract( bot->goalentity->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

			AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
			return;
		}
	}*/
	else
	{
		bot->bot_after_ammo = qfalse;
	}

	if (bot->bot_fireteam_attack_in_route)
	{
		do_attack_move = qtrue;
	}

	if ( bot->enemy
		&& bot->bot_enemy_visible_time > level.time
		&& bot->enemy == bot->bot_last_visible_enemy
		&& AIMod_Is_Avoiding_Explosive(bot) )
	{
		do_attack_move = qtrue;
	}
	else if ( bot->enemy
		&& bot->bot_enemy_visible_time > level.time
		&& bot->enemy == bot->bot_last_visible_enemy
		&& bot->client->ps.weapon == WP_SMOKE_MARKER )
	{
		do_attack_move = qtrue;
	}
#ifdef __BOTS_USE_GRENADES__
	else if ( bot->enemy
		&& bot->bot_enemy_visible_time > level.time
		&& bot->enemy == bot->bot_last_visible_enemy
		&& (bot->client->ps.weapon == WP_GRENADE_LAUNCHER || bot->client->ps.weapon == WP_GRENADE_PINEAPPLE) )
	{
		do_attack_move = qtrue;
	}
#endif //__BOTS_USE_GRENADES__
#ifdef __BOTS_USE_RIFLE_GRENADES__
	else if ( bot->enemy
		&& bot->bot_enemy_visible_time > level.time
		&& bot->enemy == bot->bot_last_visible_enemy
		&& (bot->client->ps.weapon == WP_GPG40 || bot->client->ps.weapon == WP_M7) )
	{
		do_attack_move = qtrue;
	}
#endif //__BOTS_USE_RIFLE_GRENADES__

	if (AIMod_Is_Avoiding_Explosive(bot))
	{

	}
	else if (bot->bot_coverspot_enemy)
	{
		do_attack_move = qtrue;
	}
	else if (bot->followtarget 
		&& bot->bot_fireteam_order != FT_ORDER_NONE)
	{

	}
	else if (bot->bot_explosive_avoid_time > level.time)
	{

	}
	else if (bot->followtarget 
		&& bot->bot_fireteam_order == FT_ORDER_NONE
		&& VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) > 256
		&& bot->enemy
		&& bot->beStillEng < level.time
		&& bot->beStill < level.time
		&& bot->bot_enemy_visible_time > level.time
		&& bot->enemy == bot->bot_last_visible_enemy
		&& bot->bot_runaway_time < level.time)
	{// Attack if needed, but keep moving to our commander...
		do_attack_move = qtrue;
	}
	else if (do_attack_move)
	{
		
	}
	else if ( bot->enemy &&
		bot->beStillEng < level.time &&
		bot->beStill < level.time &&
		bot->bot_enemy_visible_time > level.time &&
		bot->enemy == bot->bot_last_visible_enemy &&
		bot->bot_runaway_time < level.time
		&& AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ))
	{// If this bot has an enemy, check cover spots!
		if ( Have_Close_Enemy( bot) )
		{	// Do attacking first...
			//AIMOD_MOVEMENT_Attack( bot, ucmd );
			//return;
			do_attack_move = qtrue;
		}
		else
		{// Use cover spots!
#ifdef __OLD_COVER_SPOTS__
			AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );

			if (bot->bot_fireteam_attack_in_route)
			{
				do_attack_move = qtrue;
			}
			else
			{
				return;
			}
#else //!__OLD_COVER_SPOTS__
			if (number_of_nodes > 0 && bot->current_node)
			{
				//AIMOD_MOVEMENT_AttackMove( bot, ucmd );
				do_attack_move = qtrue;
			}
			else if (bot->bot_fireteam_attack_in_route)
			{
				do_attack_move = qtrue;
			}
			else
			{
				//AIMOD_MOVEMENT_Attack( bot, ucmd );
				AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
				bot->node_timeout = level.time + (TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
				return;
			}
#endif //__OLD_COVER_SPOTS__
		}
	}

	if (bot->enemy && MyVisible(bot, bot->enemy))
		do_attack_move = qtrue;

	if (AIMod_Is_Avoiding_Explosive(bot))
	{

	}
	else if (bot->bot_coverspot_enemy)
	{

	}
	else if (bot->followtarget 
		&& bot->bot_fireteam_order != FT_ORDER_NONE)
	{

	}
	else if (bot->bot_explosive_avoid_time > level.time)
	{

	}
	else if (bot->followtarget && bot->followtarget->health <= 0 && bot->client->sess.playerType == PC_MEDIC)
	{

	}
	else if ( /*bot->bot_stand_guard ||*/ bot->bot_patrol_time > level.time /*|| bot->bot_random_move_time > level.time*/ )
	{// A bot that is standing guard...
		// Initialize move destination...
		if (!bot->bot_initialized)
		{
			bot->bot_initialized = qtrue;

			VectorCopy(bot->r.currentOrigin, bot->bot_sp_spawnpos);
		}

		AIMOD_SP_MOVEMENT_Move( bot, ucmd );
		bot->node_timeout = level.time + (TravelTime( bot )*2);
		bot->bot_last_good_move_time = level.time;
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		return;
	}

	if (AIMod_Is_Avoiding_Explosive(bot))
	{

	}
	else if (bot->bot_coverspot_enemy)
	{

	}
	else if (bot->followtarget 
		&& bot->bot_fireteam_order != FT_ORDER_NONE)
	{

	}
	else if (bot->followtarget && bot->followtarget->health <= 0 && bot->client->sess.playerType == PC_MEDIC)
	{

	}
	else if (bot->bot_explosive_avoid_time > level.time)
	{

	}
	else if
	(
		(g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
		&& bot->client->ps.stats[STAT_FLAG_CAPTURE] > 0
		&& g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != bot->client->sess.sessionTeam
		&& (!bot->next_node || VectorDistance(bot->r.currentOrigin, g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.origin) <= VectorDistance(nodes[bot->next_node].origin, g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.origin))
	)
	{
		vec3_t org;

		VectorCopy(g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.origin, org);
		org[0]+=64;
		org[1]+=64;

		// Initialize move destination...
		bot->bot_initialized = qtrue;

		VectorCopy(org, bot->bot_sp_spawnpos);
		bot->bot_patrol_time = level.time + 5000;
		AIMOD_SP_MOVEMENT_Move( bot, ucmd );
		bot->node_timeout = level.time + (TravelTime( bot )*2);
		bot->bot_last_good_move_time = level.time;
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

		bot->bot_last_attack_time = level.time;
		return;
	}

	if ( bot->current_node < 0 
		|| bot->longTermGoal < 0 
		|| new_follow_route 
		|| bot->current_node > number_of_nodes 
		|| bot->longTermGoal > number_of_nodes )
	{
		/*G_Printf("Screwed at 2. ");

		if (bot->current_node < 0)
			G_Printf("bot->current_node < 0\n");
		else if (bot->longTermGoal < 0)
			G_Printf("bot->longTermGoal < 0\n");
		else if (new_follow_route)
			G_Printf("new_follow_route\n");
		else if (bot->current_node > number_of_nodes)
			G_Printf("bot->current_node > number_of_nodes\n");
		else if (bot->longTermGoal > number_of_nodes)
			G_Printf("bot->longTermGoal > number_of_nodes\n");
		else 
			G_Printf("\n");*/

		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		if ( bot->enemy && bot->bot_enemy_visible_time > level.time /*&& bot->enemy == bot->bot_last_visible_enemy*/ && AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ) )
		{
			//AIMOD_MOVEMENT_Attack(bot, ucmd);
			AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			return;
		}

#ifdef _WIN32
			if (bot_debug.integer && bot->goalentity 
				&& !Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot"))
				&& VectorDistance(bot->r.currentOrigin, bot->goalentity->s.origin) < 640)
			{
				G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^7 queued a new path! - distance to goal was %f. (4)\n", bot->client->pers.netname, VectorDistance(bot->r.currentOrigin, bot->goalentity->s.origin) );

				/*if (bot->current_node < 0)
					G_Printf("bot->current_node < 0\n");
				else if (bot->longTermGoal < 0)
					G_Printf("bot->longTermGoal < 0\n");
				else if (new_follow_route)
					G_Printf("new_follow_route\n");
				else if (bot->current_node > number_of_nodes)
					G_Printf("bot->current_node > number_of_nodes\n");
				else if (bot->longTermGoal > number_of_nodes)
					G_Printf("bot->longTermGoal > number_of_nodes\n");*/
			}

			Bot_Queue_New_Path( bot, ucmd );

			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			//bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
#else //!_WIN32
			Bot_Set_New_Path( bot, ucmd );
			
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

			if (bot->current_node < 0 || bot->current_node > number_of_nodes || bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes)
			{
				bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
				return;
			}
#endif //_WIN32
	}

	if ( bot->bot_initialized )
	{// SP UN-Initialization... For patroling...
		bot->bot_initialized = qfalse;

		// Initialize move destination...
		VectorCopy(bot->r.currentOrigin, bot->bot_sp_destination);
		VectorCopy(bot->r.currentOrigin, bot->bot_sp_spawnpos);
	}

	/*if (bot->followtarget)
	{

	}
	else if ( bot->client->sess.playerType == PC_COVERTOPS || (bot->client->sess.playerType == PC_SOLDIER && bot->client->sess.playerWeapon == WP_PANZERFAUST))
	{
		if (bot->client->ps.eFlags & EF_CLOAKED)
		{// At a cloak spot! Get ready and attack if needed!
			ucmd->forwardmove = 0;
			ucmd->upmove = 0;
			ucmd->rightmove = 0;

			if ( bot->enemy &&
				bot->beStillEng < level.time &&
				bot->beStill < level.time &&
				bot->bot_enemy_visible_time > level.time &&
				bot->enemy == bot->bot_last_visible_enemy )
			{// If this bot has an enemy, attack!
				AIMOD_MOVEMENT_Attack( bot, ucmd );
				AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
				return;
			}

			AIMOD_MOVEMENT_SetupStealth( bot, ucmd );
			return;
		}

		// ETE Stealth System...
		if (bot->bot_fastsnipe_active)
		{
			vec3_t origin;

			VectorCopy(bot->r.currentOrigin, origin);
			origin[2]+=16;//48;

			if (VectorDistance(origin, bot->bot_fastsnipe_pos) < 8)
			{
				AIMOD_MOVEMENT_SetupStealth( bot, ucmd );

				ucmd->forwardmove = 0;
				ucmd->upmove = 0;
				ucmd->rightmove = 0;
				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;
				return;
			}
			else if (SP_NodeVisible(origin, bot->bot_fastsnipe_pos, bot->s.number))
			{
				// Set up ideal view angles for this enemy...
				VectorSubtract( bot->bot_fastsnipe_pos, bot->r.currentOrigin, bot->move_vector );
				bot->move_vector[ROLL] = 0;
				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
				BotSetViewAngles( bot, 100 );
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

				AIMOD_MOVEMENT_SetupStealth( bot, ucmd );

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;

				if (VectorLength(bot->client->ps.velocity) < 16)
				{
					ucmd->upmove = 127;
				}

				return;
			}
			else
			{// Try again!
				bot->bot_fastsnipe_active = qfalse;
				VectorSet(bot->bot_fastsnipe_pos, 0, 0, 0);
			}
		}
		
		if (num_stealth_areas > 0)
		{
			int i;

			if (bot->bot_fastsnipe_check < level.time)
			{
				for (i = 0; i < num_stealth_areas; i++)
				{
					if (VectorDistance(stealth_area_origin[i], bot->r.currentOrigin) < 1024)
					{
						vec3_t origin;

						VectorCopy(bot->r.currentOrigin, origin);
						origin[2]+=16;//32;

						if (SP_NodeVisible(origin, stealth_area_origin[i], bot->s.number))
						{// This stealth pos is available and reacable, go there!
							// Set our last good move time to now...
							bot->bot_last_good_move_time = level.time;

							bot->bot_fastsnipe_active = qtrue;
							VectorCopy(stealth_area_origin[i], bot->bot_fastsnipe_pos);
							bot->bot_fastsnipe_check = level.time + 5000;
							return;
						}
					}
				}

				bot->bot_fastsnipe_check = level.time + 1000;
			}
		}
	}*/

	if ( /*bot->bot_last_good_move_time < level.time - 1000 ||*/ bot->node_timeout < level.time - 2000/*1000*/
		&& !(bot->bot_coverspot_enemy && bot->enemy && bot->enemy == bot->bot_coverspot_enemy) 
		&& bot->bot_snipetime < level.time && AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ))
	{
		trace_t tr;
		vec3_t viewpos;

		if ( bot->enemy && bot->bot_enemy_visible_time > level.time && bot->enemy == bot->bot_last_visible_enemy )
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			//AIMOD_MOVEMENT_Attack(bot, ucmd);
			AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			return;
		}

		// First see if we should disable this link..
		VectorCopy(bot->r.currentOrigin, viewpos);
		viewpos[2]+=32;

		trap_Trace(&tr, viewpos, NULL, NULL, nodes[bot->current_node].origin, bot->s.number, MASK_PLAYERSOLID);

		if ((
			(tr.entityNum >= 0 
			&& tr.entityNum < ENTITYNUM_MAX_NORMAL 
			&& !IgnoreTraceEntType(g_entities[tr.entityNum].s.eType)
			) 
			|| tr.entityNum == ENTITYNUM_WORLD) && !(tr.surfaceFlags & SURF_LADDER))
		{
			int loop = 0;
			
			for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
			{													// Find the link we last used and mark it...
				if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
				{												// Mark it as blocked...
					BOT_MakeLinkSemiUnreachable( bot, bot->last_node, loop );
					break;
				}
			}
		}
		else if ((
			(tr.entityNum >= 0 
			&& tr.entityNum < ENTITYNUM_MAX_NORMAL 
			&& IgnoreTraceEntType(g_entities[tr.entityNum].s.eType)
			) 
			|| tr.entityNum == ENTITYNUM_WORLD) && !(tr.surfaceFlags & SURF_LADDER))
		{
			AIMOD_MOVEMENT_IncreaseLinkageCost( bot );
		}

		// Find a new route...
		//G_Printf("Screwed waypointing at 5\n");
		bot->current_node = -1;
		bot->longTermGoal = -1;
		
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		if (bot_debug.integer)
			G_Printf("BOT DEBUG: %s - bot->node_timeout < level.time - 2000\n", bot->client->pers.netname);

		bot->node_timeout = level.time + (TravelTime( bot )*2);
		bot->bot_last_good_move_time = level.time;
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
		return;
	}

	/*if ( bot->bot_explosive_avoid_waypoint >= 0 || avoiding_explosives )
	{// Avoiding an explosion, keep moving!

	} 
	else if (bot->goalentity && !Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot")) && VectorDistance( bot->goalentity->r.currentOrigin, bot->r.currentOrigin) < 256)
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		
		if ( bot->enemy && bot->bot_enemy_visible_time > level.time && bot->enemy == bot->bot_last_visible_enemy )
		{
			//AIMOD_MOVEMENT_Attack(bot, ucmd);
			AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		}
		return;
	}*/

	if ( (bot->bot_last_position_check < level.time - 2000 || bot->bot_last_good_move_time < level.time - 3000)
		&& bot->bot_snipetime < level.time)
	{		// Check if we have moved in the last 2 seconds... (Are we stuck somewhere?)
		if (bot->bot_coverspot_enemy && bot->enemy && bot->enemy == bot->bot_coverspot_enemy)
		{
			bot->bot_last_real_move_time = level.time;
		}
		else if (bot->bot_coverspot_time > level.time)
		{
			bot->bot_last_real_move_time = level.time;
		}
		else if (bot->client->ps.stats[STAT_FLAG_CAPTURE] > 0 &&
			g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != bot->client->sess.sessionTeam)
		{
			bot->bot_last_attack_time = level.time;
		}
		/*else if
		(
			bot->goalentity &&
			!Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot")) &&
			VectorDistance( bot->goalentity->r.currentOrigin, bot->r.currentOrigin) < 256
		)
		{
			bot->bot_last_real_move_time = level.time;
		}*/
		else if ( VectorDistanceNoHeight( bot->bot_last_position, bot->r.currentOrigin) <= 8 /*64*/  )
		{	// We are stuck... Find a new route!
			// First see if we should disable this link..
			trace_t tr;
			vec3_t viewpos;

			//G_Printf("Screwed at 1.\n");

			if ( bot->bot_explosive_avoid_waypoint >= 0 || avoiding_explosives )
			{// Avoiding an explosion, keep moving!
				bot->bot_last_real_move_time = level.time;
			}
			else if ( bot->enemy && bot->bot_enemy_visible_time > level.time /*&& bot->enemy == bot->bot_last_visible_enemy*/ && AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ) )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				//AIMOD_MOVEMENT_Attack(bot, ucmd);
				AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
				bot->node_timeout = level.time + (TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
				bot->bot_last_real_move_time = level.time;
				return;
			}

			VectorCopy(bot->r.currentOrigin, viewpos);
			viewpos[2]+=32;

			trap_Trace(&tr, viewpos, NULL, NULL, nodes[bot->current_node].origin, bot->s.number, MASK_PLAYERSOLID);

			if (((tr.entityNum >= 0 && tr.entityNum < ENTITYNUM_MAX_NORMAL && !IgnoreTraceEntType(g_entities[tr.entityNum].s.eType)) 
				|| tr.entityNum == ENTITYNUM_WORLD) && !(tr.surfaceFlags & SURF_LADDER))
			{
				int loop = 0;
			
				for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
				{													// Find the link we last used and mark it...
					if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
					{												// Mark it as blocked...
						BOT_MakeLinkSemiUnreachable( bot, bot->last_node, loop );
						break;
					}
				}
			}
			else if (((tr.entityNum >= 0 && tr.entityNum < ENTITYNUM_MAX_NORMAL 
				&& IgnoreTraceEntType(g_entities[tr.entityNum].s.eType)) 
				|| tr.entityNum == ENTITYNUM_WORLD) && !(tr.surfaceFlags & SURF_LADDER))
			{
				AIMOD_MOVEMENT_IncreaseLinkageCost( bot );
			}

			// Find a new route...
			//G_Printf("Screwed waypointing at 6\n");
			bot->current_node = -1;
			bot->longTermGoal = -1;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);

			if (bot_debug.integer)
			{
				if (bot->bot_last_position_check < level.time - 2000)
					G_Printf("BOT DEBUG: %s - bot->node_timeout < level.time - 2000\n", bot->client->pers.netname);
				else if (bot->bot_last_good_move_time < level.time - 3000)
					G_Printf("BOT DEBUG: %s - bot->bot_last_good_move_time < level.time - 3000\n", bot->client->pers.netname);
			}
			return;
		}
		else if (VectorDistance(bot->r.currentOrigin, bot->bot_last_position) < 24)
		{
			if (Q_TrueRand(0,2) >= 1)
				force_jump = qtrue;
			else
				force_crouch = qtrue;

			bot->bot_last_real_move_time = level.time;
		}
		else
		{
			bot->bot_last_real_move_time = level.time;
		}

		bot->node_timeout = level.time + (TravelTime( bot )*2);
		bot->bot_last_good_move_time = level.time;
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
	}

#ifndef __NO_SNIPER_POINTS__
	if ( AIMOD_MOVEMENT_SniperFunc( bot, ucmd, avoiding_explosives ) )
	{
		return;
	}
	else
	{// This will force them to switch away from scope when moving...
		AIMOD_Movement_SniperCheckReload( bot, ucmd, avoiding_explosives );
	}
#endif //__NO_SNIPER_POINTS__

	// Now we need to check if we have hit our current node...
	if ( AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
	{	// At current node.. Pick next in the list...
		if ( bot->current_node == bot->longTermGoal )
		{// Hit our long term goal...
			if ( bot->goalentity
				&& bot->client->sess.playerType == PC_ENGINEER 
				&& (GoalType(bot->goalentity->s.number, bot->client->sess.sessionTeam) == BOTGOAL_CONSTRUCT || GoalType(bot->goalentity->s.number, bot->client->sess.sessionTeam) == BOTGOAL_DESTROY))
			{
				VectorSubtract( bot->goalentity->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
				BotSetViewAngles( bot, 100 );
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

				AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );

				return;
			}
			else if (bot->goalentity && bot->bot_after_ammo && IsAmmoEntity(bot->goalentity))
			{// Head for ammo...
				if (!bot->goalentity)
				{// Init waypoints because the ammo we were after no longer exists!
					if ( bot->bot_explosive_avoid_waypoint >= 0 || avoiding_explosives )
					{// Avoiding an explosion, keep moving!

					}
					else if ( bot->enemy && bot->bot_enemy_visible_time > level.time && bot->enemy == bot->bot_last_visible_enemy )
					{
						//AIMOD_MOVEMENT_Attack(bot, ucmd);
						AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						bot->node_timeout = level.time + (TravelTime( bot )*2);
						bot->bot_last_good_move_time = level.time;
						bot->bot_last_position_check = level.time;
						VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
						return;
					}

					//G_Printf("Screwed waypointing at 7\n");

					bot->longTermGoal = -1;
					bot->current_node = -1;
					bot->bot_after_ammo = qtrue;
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					bot->bot_after_ammo = qfalse;
					return;
				}

				VectorSubtract( bot->goalentity->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
				AIMOD_Bot_Aim_At_Object( bot, bot->goalentity );
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

				AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );

				/*if (bot->goalentity->item && bot->goalentity->item->giType == IT_WEAPON)
				{
					// And make sure we pick it up!
					ucmd->buttons |= BUTTON_ACTIVATE;
				}
				else*/ if (bot->goalentity->item && bot->goalentity->item->giType == IT_AMMO)
				{
					Pickup_Ammo( bot->goalentity, bot );
				}

				bot->longTermGoal = -1;
				bot->current_node = -1;
				bot->bot_after_ammo = qfalse;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;
			}
			else
			{
				//Bot_Set_New_Path( bot, ucmd );
				bot->current_node = -1;
				bot->next_node = -1;
				bot->longTermGoal = -1;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				bot->node_timeout = level.time + (TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

				if (bot_debug.integer)
				{
					G_Printf("BOT DEBUG: %s - hit his long term goal!\n", bot->client->pers.netname);
				}

				if ( bot->bot_explosive_avoid_waypoint >= 0 || avoiding_explosives )
				{// Avoiding an explosion, keep moving!

				}
				else if ( bot->enemy && bot->bot_enemy_visible_time > level.time && bot->enemy == bot->bot_last_visible_enemy )
					//AIMOD_MOVEMENT_Attack(bot, ucmd);
					AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			}
			return;
		}
		else
		{// Continue on to next node...
			bot->last_node = bot->current_node;
			bot->current_node = bot->next_node;
			bot->next_node = BotGetNextNode( bot );
			bot->node_timeout = level.time + TravelTime( bot );

			if (bot->current_node > number_of_nodes)
			{
				//G_Printf("Screwed waypointing at 8\n");

				bot->current_node = -1;
				bot->next_node = -1;
				bot->longTermGoal = -1;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				if ( bot->bot_explosive_avoid_waypoint >= 0 || avoiding_explosives )
				{// Avoiding an explosion, keep moving!

				}
				else if ( bot->enemy && bot->bot_enemy_visible_time > level.time && bot->enemy == bot->bot_last_visible_enemy )
				{
					//AIMOD_MOVEMENT_Attack(bot, ucmd);
					AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
					bot->node_timeout = level.time + (TravelTime( bot )*2);
					bot->bot_last_good_move_time = level.time;
					bot->bot_last_position_check = level.time;
					VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
				}
				return;
			}

			if ( nodes[bot->current_node].type & NODE_DUCK )
			{
				bot->node_timeout = level.time + ( TravelTime( bot) * 2 );	// Duck moves take longer, so...
			}

			// Set our last good move time to now...
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		}
	}

	// Set up ideal view angles for this enemy...
	VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
	bot->move_vector[ROLL] = 0;

	if (do_attack_move && bot->enemy)
	{// Enemy is the dir we need to face, but keep moving toward our waypoint as well!
		vec3_t enemy_dir, enemy_org;

		VectorCopy(bot->enemy->r.currentOrigin, enemy_org);

#ifdef __NPC__
		if ((bot->enemy && bot->enemy->client)
			&& ((bot->enemy->client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->client->ps.eFlags & EF_CROUCHING)))
				enemy_org[2] -= 20; // Aim a little lower if they are crouching...
		else if ((bot->enemy && bot->enemy->NPC_client)
			&& ((bot->enemy->NPC_client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->NPC_client->ps.eFlags & EF_CROUCHING)))
#else //!__NPC__
		if ((bot->enemy->client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->client->ps.eFlags & EF_CROUCHING))
#endif //__NPC__
			enemy_org[2] -= 20; // Aim a little lower if they are crouching...

#ifdef __NPC__
		if ((bot->enemy && bot->enemy->client)
			&& (bot->enemy->client->ps.eFlags & EF_PRONE))
			enemy_org[2] -= 30; // Aim even lower if they are prone!
		else if ((bot->enemy && bot->enemy->NPC_client)
			&& (bot->enemy->NPC_client->ps.eFlags & EF_PRONE))
#else //!__NPC__
		if (bot->enemy->client->ps.eFlags & EF_PRONE)
#endif //__NPC__
			enemy_org[2] -= 30; // Aim even lower if they are prone!

		VectorSubtract( enemy_org, bot->r.currentOrigin, enemy_dir );
		vectoangles( enemy_dir, bot->bot_ideal_view_angles );
	}
	else
	{
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
	}

	if ( do_attack_move && bot->enemy 
		&& ( bot->client->ps.weapon == WP_SMOKE_MARKER
		|| bot->client->ps.weapon == WP_GRENADE_LAUNCHER 
		|| bot->client->ps.weapon == WP_GRENADE_PINEAPPLE
		|| bot->client->ps.weapon == WP_GPG40 
		|| bot->client->ps.weapon == WP_M7 ) )
	{// UQ1: TOTAL HACK!!! Why the hell do grenades and rifle grenades screw up angles???
		vec3_t enemy_dir, enemy_org;

		VectorCopy(bot->enemy->r.currentOrigin, enemy_org);

#ifdef __NPC__
		if ((bot->enemy && bot->enemy->client)
			&& ((bot->enemy->client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->client->ps.eFlags & EF_CROUCHING)))
			enemy_org[2] -= 20; // Aim a little lower if they are crouching...
		else if ((bot->enemy && bot->enemy->NPC_client)
			&& ((bot->enemy->NPC_client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->NPC_client->ps.eFlags & EF_CROUCHING)))
#else //!__NPC__
		if ((bot->enemy->client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->client->ps.eFlags & EF_CROUCHING))
#endif //__NPC__
			enemy_org[2] -= 20; // Aim a little lower if they are crouching...

#ifdef __NPC__
		if ((bot->enemy && bot->enemy->client)
			&& (bot->enemy->client->ps.eFlags & EF_PRONE))
			enemy_org[2] -= 30; // Aim even lower if they are prone!
		else if ((bot->enemy && bot->enemy->NPC_client)
			&& (bot->enemy->NPC_client->ps.eFlags & EF_PRONE))
#else //!__NPC__
		if (bot->enemy->client->ps.eFlags & EF_PRONE)
#endif //__NPC__
			enemy_org[2] -= 30; // Aim even lower if they are prone!

		enemy_org[2] += (VectorDistance(bot->r.currentOrigin, enemy_org)/20);
		VectorSubtract( enemy_org, bot->r.currentOrigin, enemy_dir );
		vectoangles( enemy_dir, bot->bot_ideal_view_angles );

		/*if (bot->grenadeFired < level.time - 10000)
		{
			fire_grenade( bot, bot->r.currentOrigin, enemy_dir, bot->client->ps.weapon );
			bot->grenadeFired = level.time;
		}*/

		// Set up ideal view angles for this enemy...
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		BotSetViewAngles( bot, 100 );

		inFOV = MyInFOV2(enemy_org, bot, 3, 3);
	}
	else
	{
		BotSetViewAngles( bot, 100 );
	}

	// Artillery...
	if (VectorDistance(bot->bot_ideal_view_angles, bot->client->ps.viewangles) > 4)
	{

	}
	else if ( bot->enemy
		&& next_arty[bot->s.clientNum] < level.time 
		&& bot->client->ps.stats[STAT_PLAYER_CLASS] == PC_FIELDOPS )
	{
		if (VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 768 
			&& AIMOD_AI_Spot_Safe_For_Grenade( bot, bot->enemy->r.currentOrigin )
			&& Can_Arty( bot, bot->enemy->r.currentOrigin))
		{
			int skill_level = bot->skillchoice;

			if (skill_level <= 0)
				skill_level = 1;

			Weapon_Artillery( bot );
			next_arty[bot->s.clientNum] = level.time + ( 50000 /  skill_level );
		}
	}

	G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

#ifdef __BOT_STRAFE__
	if ( bot->bot_strafe_right_timer >= level.time )
	{
		if ( VectorDistance( bot->bot_strafe_target_position, bot->r.currentOrigin) > 8 )
		{
			ucmd->forwardmove = 0;							//64;
			ucmd->upmove = 0;
			ucmd->rightmove = 64;
			return;
		}
		else
		{
			bot->bot_strafe_right_timer = 0;
			//ucmd->upmove = 127;
		}
	}

	if ( bot->bot_strafe_left_timer >= level.time )
	{
		if ( VectorDistance( bot->bot_strafe_target_position, bot->r.currentOrigin) > 8 )
		{
			ucmd->forwardmove = 0;							//64;
			ucmd->upmove = 0;
			ucmd->rightmove = -64;
			return;
		}
		else
		{
			bot->bot_strafe_left_timer = 0;
			//ucmd->upmove = 127;
		}
	}

	if ( bot->bot_duck_time > level.time )
	{
		if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
		{													// For special duck nodes, make the bot duck down...
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;
		}

		ucmd->forwardmove = 127;
		return;
	}
	else if ( bot->bot_strafe_left_time > level.time )
	{
		ucmd->rightmove = -48;
		return;
	}
	else if ( bot->bot_strafe_right_time > level.time )
	{
		ucmd->rightmove = 48;
		return;
	}
	else if ( bot->bot_jump_time > level.time )
	{
		ucmd->upmove = 127;
		//G_Printf("Jump type 1\n");
	}
	else if
		(
			bot->bot_last_good_move_time < level.time - 1000 &&
			bot->bot_last_move_fix_time < level.time - 1000 &&
			VectorLength(bot->client->ps.velocity) < 16 &&
			!onLadder
		)
	{														// The first type of route correction...
		int result = -1;

		// So we don't vis check too often...
		bot->bot_last_move_fix_time = level.time;

		result = AIMOD_MOVEMENT_ReachableBy( bot, nodes[bot->current_node].origin );
		
		/*if ( result == -1 )
		{													// -1 is Not reachable at all... Let's use some blind routing for a short time!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->longTermGoal = -1;
			bot->shortTermGoal = -1;
			return;
		}*/

		if ( result == 1 )
		{													// 1 is Jump...
			ucmd->upmove = 127;
			//G_Printf("Jump type 2\n");
			bot->bot_jump_time = level.time + 500;
		}

		if ( result == 2 )
		{													// 2 is Duck...
			if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
			{												// For special duck nodes, make the bot duck down...
				ucmd->upmove = -48;
				bot->client->ps.pm_flags |= PMF_DUCKED;
				bot->bot_duck_time = level.time + 2000;
			}
		}

		if ( result == 3 )
		{													// 3 is Strafe Left...
			ucmd->rightmove = -48;
			bot->bot_strafe_left_time = level.time + 3000;
			return;
		}

		if ( result == 4 )
		{													// 4 is Strafe Right...
			ucmd->rightmove = 48;
			bot->bot_strafe_right_time = level.time + 3000;
			return;
		}
	}
#endif //__BOT_STRAFE__

	if ( nodes[bot->current_node].type & NODE_DUCK && !(bot->client->ps.pm_flags & PMF_DUCKED) )
	{			// For special duck nodes, make the bot duck down...
		ucmd->upmove = -48;
		bot->client->ps.pm_flags |= PMF_DUCKED;
	}
	else if ( nodes[bot->last_node].type & NODE_DUCK && !(bot->client->ps.pm_flags & PMF_DUCKED) )
	{			// For special duck nodes, make the bot duck down...
		ucmd->upmove = -48;
		bot->client->ps.pm_flags |= PMF_DUCKED;
	}
	else if
		(
			!(nodes[bot->current_node].type & NODE_DUCK) &&
			!(nodes[bot->last_node].type & NODE_DUCK) &&
			!(bot->client->ps.eFlags & EF_CLOAKED) &&
			!bot->enemy &&
			(bot->client->ps.eFlags & EF_CROUCHING)
		)
	{			// Get up?
		//ucmd->upmove = 127;
		bot->client->ps.eFlags &= ~EF_CROUCHING;
	}
	else if
		(
			!(nodes[bot->current_node].type & NODE_DUCK) &&
			!(nodes[bot->last_node].type & NODE_DUCK) &&
			!(bot->client->ps.eFlags & EF_CLOAKED) &&
			!bot->enemy &&
			(bot->client->ps.eFlags & EF_PRONE)
		)
	{			// Get up?
		//ucmd->upmove = 127;
		bot->client->ps.eFlags &= ~EF_PRONE;

		if (bot->client->ps.eFlags & EF_PRONE_MOVING)
			bot->client->ps.eFlags &= ~EF_PRONE_MOVING;
	}
	else if
		(
			!(nodes[bot->current_node].type & NODE_DUCK) &&
			!(nodes[bot->last_node].type & NODE_DUCK) &&
			!(bot->client->ps.eFlags & EF_CLOAKED) &&
			!bot->enemy &&
			(bot->client->ps.eFlags & EF_PRONE_MOVING)
		)
	{			// Get up?
		//ucmd->upmove = 127;
		bot->client->ps.eFlags &= ~EF_PRONE_MOVING;

		if (bot->client->ps.eFlags & EF_PRONE)
			bot->client->ps.eFlags &= ~EF_PRONE;
	}
	else if ( !bot->enemy && (bot->client->ps.eFlags & EF_PRONE) )
	{			// Get up?
		//ucmd->upmove = 127;
		bot->client->ps.eFlags &= ~EF_PRONE;
	}

	if (bot->last_node > 0 
		&& bot->last_node < number_of_nodes
		&& bot->current_node > 0
		&& bot->current_node < number_of_nodes)
	{
		if (bot->bot_last_node_link_node != bot->current_node)
		{// Avoid this loop if possible...
			node_link_flags = FlagsForNodeLink( bot->last_node, bot->current_node );
			bot->bot_last_node_link_node = bot->current_node;
		}
		else
		{
			node_link_flags = bot->bot_last_node_link_flags;
		}
	}

	if ((node_link_flags & PATH_CROUCH) 
		|| (nodes[bot->current_node].type & NODE_DUCK)
		|| (nodes[bot->last_node].type & NODE_DUCK) )
	{
		if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
		{												// For special duck nodes, make the bot duck down...
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;
			//bot->bot_duck_time = level.time + 500;
		}
		else if ( VectorLength(bot->client->ps.velocity) <= 8)
		{// Still not moving, try prone :)
			if (!(bot->client->ps.eFlags & EF_PRONE) && !(bot->client->ps.eFlags & EF_PRONE_MOVING))
				bot->client->ps.eFlags |= EF_PRONE;

			if (bot->client->ps.pm_flags & PMF_DUCKED)
				bot->client->ps.pm_flags &= ~PMF_DUCKED;

			if (bot->client->ps.eFlags & EF_CROUCHING)
				bot->client->ps.eFlags &= ~EF_CROUCHING;
		}
	}

	if (((node_link_flags & PATH_JUMP) || (nodes[bot->current_node].type & NODE_JUMP))
		&& (VectorDistance(bot->r.currentOrigin, nodes[bot->current_node].origin) < 56 || VectorLength(bot->client->ps.velocity) < 24) )
	{
		ucmd->upmove = 127;
		//G_Printf("Jump type 3\n");
		//bot->bot_jump_time = level.time + 500;
	}

	if ( nodes[bot->current_node].origin[2] - bot->r.currentOrigin[2] > 8/*24*/ 
		&& !onLadder 
		&& VectorLength(bot->client->ps.velocity) < 24
		&& !(bot->client->ps.pm_flags & PMF_DUCKED) )
	{	// Do we need to jump to our next node?
		ucmd->upmove = 127;
		//G_Printf("Jump type 4\n");
	}

	// UQ1: Will try jumping before falls if not moving too.. May be a railing or somthing blocking!
	/*if ( bot->r.currentOrigin[2] - nodes[bot->current_node].origin[2] > 64 
		&& !onLadder 
		&& VectorLength(bot->client->ps.velocity) < 24
		&& !(bot->client->ps.pm_flags & PMF_DUCKED) )
	{	// Do we need to jump to our next node?
		ucmd->upmove = 127;
		//G_Printf("Jump type 4\n");
	}*/

//#ifdef __BOT_UNUSED__
	AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
//#endif //__BOT_UNUSED__

#ifdef __BOT_STRAFE__
	if
	(
		bot->bot_strafe_right_timer < level.time &&
		bot->bot_strafe_left_timer < level.time &&
		VectorLength( bot->client->ps.velocity) < 16 &&
		!(nodes[bot->current_node].type & NODE_DUCK) &&
		!(nodes[bot->last_node].type & NODE_DUCK) &&
		!onLadder
	)
	{	// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
		AIMOD_MOVEMENT_Setup_Strafe( bot, ucmd );
	}

	if ( bot->bot_strafe_right_timer >= level.time || bot->bot_strafe_left_timer >= level.time )
	{
		return;
	}
#endif //__BOT_STRAFE__

	if (force_jump)
	{
		ucmd->upmove = 127;
	}
	else if (force_crouch)
	{
		if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
		{												// For special duck nodes, make the bot duck down...
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;
		}
	}

	if (do_attack_move && bot->enemy)
	{// For attacks in route...
		if (bot->bot_end_fire_next_frame)
		{// Make sure we release the grenade the frame after we hit attack, or it won't release!
			ucmd->buttons &= ~BUTTON_ATTACK;
			bot->bot_end_fire_next_frame = qfalse;
			
			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
			return;
		}
		else
		{
			if ( bot->client->ps.weapon == WP_SMOKE_MARKER )
			{
				bot->bot_end_fire_next_frame = qtrue;
			}
#ifdef __BOTS_USE_GRENADES__
			else if ( bot->client->ps.weapon == WP_GRENADE_LAUNCHER || bot->client->ps.weapon == WP_GRENADE_PINEAPPLE )
			{
				bot->bot_end_fire_next_frame = qtrue;
			}
#endif //__BOTS_USE_GRENADES__
#ifdef __BOTS_USE_RIFLE_GRENADES__
			else if ( bot->client->ps.weapon == WP_GPG40 || bot->client->ps.weapon == WP_M7 )
			{
				bot->bot_end_fire_next_frame = qtrue;
			}
#endif //__BOTS_USE_RIFLE_GRENADES__
			else if (!bot->bot_fireteam_attack_in_route && !bot->bot_coverspot_enemy)
			{// Walk for other weapons...
				if (!(ucmd->buttons & BUTTON_WALKING))
					ucmd->buttons |= BUTTON_WALKING;

#ifdef __WEAPON_AIM__
				if (!(bot->client->ps.eFlags & EF_AIMING))
					bot->client->ps.eFlags |= EF_AIMING;
#endif //__WEAPON_AIM__
			}
		}

		if (bot->client->ps.weapon != bot->client->sess.playerWeapon && bot->client->ps.weapon != bot->client->sess.playerWeapon2)
		{
			Set_Best_AI_weapon( bot );
		}
		else
		{
			AIMOD_SP_MOVEMENT_CoverSpot_Aim_At_Enemy( bot );

			if (!AIMOD_SP_MOVEMENT_Is_CoverSpot_Weapon(bot->client->ps.weapon))
			{
				Set_Best_AI_weapon( bot );
				return;
			}

			if (inFOV || MyInFOV(bot->enemy, bot, 12, 25))
			{
				CalcMuzzlePoints(bot, bot->client->ps.weapon);

				if (VectorDistance(bot->r.currentOrigin, bot->enemy->r.currentOrigin) < 256 
					|| MyVisible_No_Supression(bot, bot->enemy))
					//ucmd->buttons |= BUTTON_ATTACK;
					AIMOD_SP_MOVEMENT_CoverSpot_Do_Actual_Attack( bot, ucmd );
			}
		}
	}
	else
	{
		if (VectorLength(bot->client->ps.velocity) < 1 && ucmd->upmove == 0)
		{// For bots that get their heads stuck in solids.. This may free them...
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;
		}
	}

	if ( bot->client->ps.weapon == WP_DYNAMITE)
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
	}

	if (bot->bot_after_ammo 
		|| (AIMod_Is_Avoiding_Explosive(bot) && AIMOD_MOVEMENT_Explosive_Near_Waypoint(bot->current_node)))
	{// Sprint for ammo.. Get back into the fight as soon as possible!
		ucmd->buttons |= BUTTON_SPRINT;
		
		// Make sure we are running!
		if (ucmd->buttons & BUTTON_WALKING)
			ucmd->buttons &= ~BUTTON_WALKING;

		if (bot->client->ps.eFlags & EF_AIMING)
			bot->client->ps.eFlags &= ~EF_AIMING;

		if (bot->client->ps.pm_flags & PMF_DUCKED)
			bot->client->ps.pm_flags &= ~PMF_DUCKED;

		if ((bot->client->ps.eFlags & EF_CROUCHING) && !(nodes[bot->current_node].type & NODE_DUCK))
			bot->client->ps.eFlags &= ~EF_CROUCHING;

		if ((bot->client->ps.eFlags & EF_PRONE) && !(nodes[bot->current_node].type & NODE_DUCK))
			bot->client->ps.eFlags &= ~EF_PRONE;

		if ((bot->client->ps.eFlags & EF_PRONE_MOVING) && !(nodes[bot->current_node].type & NODE_DUCK))
			bot->client->ps.eFlags &= ~EF_PRONE_MOVING;
	}
	else if ((nodes[bot->current_node].type & NODE_COVER) 
		&& bot->enemy 
		&& bot->current_node == bot->longTermGoal)
	{// At a cover spot, so duck as we travel this one!
		if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
		{												// For special duck nodes, make the bot duck down...
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;
		}
	}

	if ((bot->client->ps.eFlags & EF_MG42_ACTIVE) || (bot->client->ps.eFlags & EF_MOUNTEDTANK))
	{// Never try to move is using an mg2 or mounted on a tank!
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
	}

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;
}

//===========================================================================
// Routine      : AIMOD_MOVEMENT_ChangeBotAngle
// Description  : Make changes in angles a little more gradual, not so snappy

//              : but noticable. Modified from id code ChangeYaw.
void
AIMOD_MOVEMENT_ChangeBotAngle ( gentity_t *ent )
{
	float	ideal_yaw;
	float	ideal_pitch;
	float	current_yaw;
	float	current_pitch;
	float	move;
	float	speed;
	vec3_t	ideal_angle;

	// Normalize the move angle first
	//-------------------------------------------------------
	VectorNormalize( ent->move_vector );
	current_yaw = anglemod( ent->s.angles[YAW] );
	current_pitch = anglemod( ent->s.angles[PITCH] );
	vectoangles( ent->move_vector, ideal_angle );
	ideal_yaw = anglemod( ideal_angle[YAW] );
	ideal_pitch = anglemod( ideal_angle[PITCH] );

	//-------------------------------------------------------
	// Yaw
	//-------------------------------------------------------
	if ( current_yaw != ideal_yaw )
	{
		move = ideal_yaw - current_yaw;
		speed = ent->yaw_speed;
		if ( ideal_yaw > current_yaw )
		{
			if ( move >= 180 )
			{
				move = move - 360;
			}
		}
		else
		{
			if ( move <= -180 )
			{
				move = move + 360;
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

		ent->s.angles[YAW] = anglemod( current_yaw + move );
	}

	//-------------------------------------------------------
	// Pitch
	//-------------------------------------------------------
	if ( current_pitch != ideal_pitch )
	{
		move = ideal_pitch - current_pitch;
		speed = ent->yaw_speed;
		if ( ideal_pitch > current_pitch )
		{
			if ( move >= 180 )
			{
				move = move - 360;
			}
		}
		else
		{
			if ( move <= -180 )
			{
				move = move + 360;
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

		ent->s.angles[PITCH] = anglemod( current_pitch + move );
	}

	//-------------------------------------------------------
}
#endif
