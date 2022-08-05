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

//
// Externals
//
extern int			numAxisOnlyNodes;
extern int			numAlliedOnlyNodes;
extern int			myteam, otherteam;
extern void			Set_Best_AI_weapon ( gentity_t *bot );
//extern node_t		nodes[MAX_NODES];
extern node_t		*nodes;
extern int			AIMOD_Calculate_Goal ( gentity_t *bot );
extern void			Cmd_Activate_f ( gentity_t *ent );
extern qboolean		MyVisible ( gentity_t *self, gentity_t *other );
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
extern int			AIMOD_NAVIGATION_FindClosestReachableNode ( gentity_t *bot, int r, int type, gentity_t *master );
extern int			AIMOD_NAVIGATION_FindCloseReachableNode ( gentity_t *bot, int r, int type );
extern int			AIMOD_NAVIGATION_FindFarReachableNode ( gentity_t *bot, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodesTo ( gentity_t *bot, vec3_t origin, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodes ( gentity_t *bot, int r, int type );
extern void AIMOD_MOVEMENT_CheckAvoidance ( gentity_t *bot, usercmd_t *ucmd );
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

// In unique_movement.c
extern /*short*/ int		BotGetNextNode ( gentity_t *bot /*, int *state*/ );				// below...
extern void				Bot_Set_New_Path ( gentity_t *bot, usercmd_t *ucmd );			// below
extern void				AIMOD_MOVEMENT_AttackMove ( gentity_t *bot, usercmd_t *ucmd );	// below...

// In unique_coverspots.c
extern void AIMOD_SP_MOVEMENT_CoverSpot_Attack ( gentity_t *bot, usercmd_t *ucmd, qboolean useProne );

//
// Locals
//
int next_bot_team_check = 0;
extern vmCvar_t bot_minplayers;

qboolean G_BOT_Check_Commands_Fireteam ( gentity_t *ent )
{
	int i;

	for (i = 0; i < MAX_CLIENTS; i++)
	{// Find fieldops bots.....
		gentity_t *test = &g_entities[i];

		if (!test)
			continue;

		if (!test->client)
			continue;

		if (!(test->r.svFlags & SVF_BOT))
			continue;

		if (level.clients[ test->s.number ].sess.sessionTeam == TEAM_SPECTATOR)
			continue;

		if (test->followtarget == ent)
			return qtrue;
	}

	return qfalse;
}

void G_BOT_Check_Fire_Teams ( gentity_t *ent )
{// UQ1: For bots and fireteams... (when respawning) - UQ1: Now runs most frames, avoid loops!!!
	fireteamData_t* fireTeam;

	//if (trap_Milliseconds() - level.frameTime > MAX_BOT_THINKFRAME)
	//	return; // Try next frame!

#ifdef __VEHICLES__
	if (ent->client->ps.eFlags & EF_VEHICLE)
	{
		ent->followtarget = NULL;
		G_SecondaryEntityFor(ent->s.number)->s.density = 0; // Remove fireteam sprite above head!
		G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = 0;
		ent->bot_fireteam_order = FT_ORDER_DEFAULT;
		return;
	}
#endif //__VEHICLES__

	if (G_IsOnFireteam(ent->s.number, &fireTeam))
	{// Commander *must* be a real player... Checks here...
		gentity_t *leader = &g_entities[(int)fireTeam->joinOrder[0]];

		if (leader 
			&& leader->client 
			&& leader->client->pers.connected == CON_CONNECTED 
			&& OnSameTeam(ent, leader)
			&& level.clients[ leader->s.number ].sess.sessionTeam != TEAM_SPECTATOR
			&& !(leader->r.svFlags & SVF_BOT)
			/*&& !(leader->client->ps.pm_flags & PMF_LIMBO)*/)
		{// Set our leader again on each respawn...
			ent->followtarget = leader;
			G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
			G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = ent->followtarget->s.number;
			//ent->bot_fireteam_order = FT_ORDER_DEFAULT;
		}
		else
		{
			ent->followtarget = NULL;
			G_SecondaryEntityFor(ent->s.number)->s.density = 0; // Remove fireteam sprite above head!
			G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = 0;
			G_RemoveClientFromFireteams( ent->s.number, qtrue, qfalse );
			ent->bot_fireteam_order = FT_ORDER_DEFAULT;
			//G_Printf("%s has left fireteam.\n", ent->client->pers.netname);
		}
	}
	else
	{// Commander is a BOT, or have no commanders yet (so create some teams)...
		if (level.numConnectedClients < bot_minplayers.integer)
			return;
		
//#ifdef __AI_FIRETEAM_BOT_LEADERS__
		if (ent->followtarget)
		{// Not in a fireteam, but still following someone.. Check status (most likely in a BOT team)...
			if (!OnSameTeam(ent->followtarget, ent) 
				|| !ent->followtarget->client
				|| ent->followtarget->client->pers.connected != CON_CONNECTED
				|| level.clients[ ent->followtarget->s.number ].sess.sessionTeam == TEAM_SPECTATOR
				/*|| (ent->followtarget->client->ps.pm_flags & PMF_LIMBO)*/ )
			{
				ent->followtarget = NULL;
				G_SecondaryEntityFor(ent->s.number)->s.density = 0; // Remove fireteam sprite above head!
				G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = 0;
				ent->bot_fireteam_order = FT_ORDER_DEFAULT;

				G_Printf("%s has left ai fireteam.\n", ent->client->pers.netname);
			}
		}
		else
		{// Create BOT teams...
			if (next_bot_team_check < level.time) // UQ1: Avoid doing this every frame. Added a timer...
			{// Make bots form groups with eachother... If there is no players to form up with...
				int num_axis_bots = 0;
				int num_axis_fieldops = 0; // UQ1: Changed this to make engineer's the boss.. They have more+better goals..
				int axis_fieldops[MAX_CLIENTS];
				int axis_bots[MAX_CLIENTS];
				int num_allied_bots = 0;
				int num_allied_fieldops = 0;
				int allied_fieldops[MAX_CLIENTS];
				int allied_bots[MAX_CLIENTS];
				int i;

				//G_Printf("%s - checking for fireteams.\n", ent->client->pers.netname);

				for (i = 0; i < MAX_CLIENTS; i++)
				{// Find fieldops bots.....
					gentity_t *test = &g_entities[i];

					if (!test)
						continue;

					if (!test->client)
						continue;

					if (!(test->r.svFlags & SVF_BOT))
						continue;

					if (level.clients[ test->s.number ].sess.sessionTeam == TEAM_SPECTATOR)
						continue;

					if (test->followtarget)
						continue;

					if (level.clients[ test->s.number ].sess.sessionTeam == TEAM_AXIS)
					{
						if (test->client->sess.playerType != /*PC_ENGINEER*/PC_FIELDOPS)
						{
							axis_bots[num_axis_bots] = i;
							num_axis_bots++;
							continue;
						}
					}
					else
					{
						if (test->client->sess.playerType != /*PC_ENGINEER*/PC_FIELDOPS)
						{
							allied_bots[num_allied_bots] = i;
							num_allied_bots++;
							continue;
						}
					}

					/*if (test->client->sess.playerType == PC_ENGINEER)
					{// leave engineers out of fireteams so they can go do their jobs...

					}
					else*/ if (G_BOT_Check_Commands_Fireteam(&g_entities[test->s.number]))
					{// Already commanding a fireteam, don't give them any more bots...

					}
					else if (level.clients[ test->s.number ].sess.sessionTeam == TEAM_AXIS)
					{
						axis_fieldops[num_axis_fieldops] = i;
						num_axis_fieldops++;
					}
					else
					{
						allied_fieldops[num_allied_fieldops] = i;
						num_allied_fieldops++;
					}
				}

				//G_Printf("num_axis_fieldops %i num_allied_fieldops %i.\n", num_axis_fieldops, num_allied_fieldops);
				//G_Printf("num_axis_bots %i num_allied_bots %i.\n", num_axis_bots, num_allied_bots);

				if (num_axis_fieldops > 0)
				{
					for (i = 0; i < num_axis_fieldops; i++)
					{// Found no real players... Team up the bots!
						gentity_t *boss = &g_entities[axis_fieldops[i]];
						int add_number = num_axis_bots/num_axis_fieldops;
						int j;
						qboolean have_medic = qfalse;
						qboolean have_covert = qfalse;
						//qboolean have_engineer = qfalse;
						qboolean have_soldier = qfalse;

						if (boss->client->sess.playerType == PC_ENGINEER)
						{// Just give engineers a medic and a soldier...
							if (add_number > 2)
								add_number = 2;

							for (j = 0; j < num_axis_bots/*add_number*/; j++)
							{
								gentity_t *bot = &g_entities[axis_bots[j]];

								if (bot->followtarget)
									continue;
	
								if (bot->client->sess.playerType == PC_MEDIC && !have_medic)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;
	
									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_medic = qtrue;
								}
								else if (bot->client->sess.playerType == PC_SOLDIER && !have_soldier)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;

									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_soldier = qtrue;
								}
	
								if (have_medic && have_soldier)
									break; // No point thinking any more!
							}
						}
						else
						{
							for (j = 0; j < num_axis_bots/*add_number*/; j++)
							{
								gentity_t *bot = &g_entities[axis_bots[j]];

								if (bot->followtarget)
									continue;
	
								if (bot->client->sess.playerType == PC_MEDIC && !have_medic)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;
	
									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_medic = qtrue;
								}
//#ifdef __USE_COVOPS_IN_AI_FIRETEAMS__ // UQ1: Snipers should snipe instead!
								else if (!BG_IsScopedWeapon2(bot->client->sess.playerWeapon) && bot->client->sess.playerType == PC_COVERTOPS && !have_covert)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;

									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_covert = qtrue;
								}
//#endif //__USE_COVOPS_IN_AI_FIRETEAMS__
								/*else if (bot->client->sess.playerType == PC_FIELDOPS PC_ENGINEER && !have_engineer)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;

									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_engineer = qtrue;
								}*/
								else if (bot->client->sess.playerType == PC_SOLDIER && !have_soldier)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;

									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_soldier = qtrue;
								}
	
								if (have_medic 
#ifdef __USE_COVOPS_IN_AI_FIRETEAMS__
									&& have_covert 
#endif //__USE_COVOPS_IN_AI_FIRETEAMS__
									/*&& have_engineer*/ 
									&& have_soldier)
									break; // No point thinking any more!
							}
						}
					}
				}
				
				if (num_allied_fieldops > 0)
				{
					for (i = 0; i < num_allied_fieldops; i++)
					{// Found no real players... Team up the bots!
						gentity_t *boss = &g_entities[allied_fieldops[i]];
						int add_number = num_allied_bots/num_allied_fieldops;
						int j;
						qboolean have_medic = qfalse;
						qboolean have_covert = qfalse;
						//qboolean have_engineer = qfalse;
						qboolean have_soldier = qfalse;

						if (boss->client->sess.playerType == PC_ENGINEER)
						{// Just give engineers a medic and a soldier...
							if (add_number > 2)
								add_number = 2;
							
							for (j = 0; j < num_allied_bots/*add_number*/; j++)
							{
								gentity_t *bot = &g_entities[allied_bots[j]];

								if (bot->followtarget)
									continue;

								if (bot->client->sess.playerType == PC_MEDIC && !have_medic)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;

									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_medic = qtrue;
								}
								else if (bot->client->sess.playerType == PC_SOLDIER && !have_soldier)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;
	
									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_soldier = qtrue;
								}

								if (have_medic && have_soldier)
									break; // No point thinking any more!
							}
						}
						else
						{
							for (j = 0; j < num_allied_bots/*add_number*/; j++)
							{
								gentity_t *bot = &g_entities[allied_bots[j]];

								if (bot->followtarget)
									continue;

								if (bot->client->sess.playerType == PC_MEDIC && !have_medic)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;

									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_medic = qtrue;
								}
//#ifdef __USE_COVOPS_IN_AI_FIRETEAMS__
								else if (!BG_IsScopedWeapon2(bot->client->sess.playerWeapon) && bot->client->sess.playerType == PC_COVERTOPS && !have_covert)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;

									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_covert = qtrue;
								}
//#endif //__USE_COVOPS_IN_AI_FIRETEAMS__
								/*else if (bot->client->sess.playerType == PC_FIELDOPS PC_ENGINEER && !have_engineer)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;

									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_engineer = qtrue;
								}*/
								else if (bot->client->sess.playerType == PC_SOLDIER && !have_soldier)
								{
									bot->followtarget = boss;
									G_SecondaryEntityFor(ent->s.number)->s.density = 998; // Fireteam sprite above head!
									G_SecondaryEntityFor(ent->s.number)->s.dmgFlags = boss->s.number;
									bot->bot_fireteam_order = FT_ORDER_DEFAULT;
	
									//G_Printf("%s joined bot %s's fireteam.\n", ent->client->pers.netname, boss->client->pers.netname);
									have_soldier = qtrue;
								}

								if (have_medic 
#ifdef __USE_COVOPS_IN_AI_FIRETEAMS__
									&& have_covert 
#endif //__USE_COVOPS_IN_AI_FIRETEAMS__
									/*&& have_engineer*/ 
									&& have_soldier)
									break; // No point thinking any more!
							}
						}
					}
				}

				next_bot_team_check = level.time + 10000; // Check every 10 seconds...
			}
		}
//#endif //__AI_FIRETEAM_BOT_LEADERS__
	}
}

// Fireteam order types...
/*
typedef enum {
	FT_ORDER_NONE,				// 0
	FT_ORDER_DEFAULT,			// 0
	FT_ORDER_COVER,				// 1
	FT_ORDER_SUPRESS,			// 2
	FT_ORDER_ASSAULT			// 3
}  fireteam_orders_t;
*/

qboolean AIMOD_MOVEMENT_FireTeam_AI_Order_Take_Cover ( gentity_t *bot, usercmd_t *ucmd, qboolean new_follow_route, qboolean do_attack_move )
{// Take cover in specified position... Only attack enemies that come close enough to attack...
	float distance = VectorDistance(bot->r.currentOrigin, bot->bot_fireteam_order_coverspot);

	if (distance <= 8)
	{// At the spot.. Hold cover here...
		if ( bot->enemy
			&& (bot->enemy->enemy == bot || MyVisible(bot->enemy, bot))
			&& bot->bot_enemy_visible_time > level.time
			&& bot->enemy == bot->bot_last_visible_enemy 
			&& AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ))
		{// Attack if an enemy is attacking us...
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			//AIMOD_MOVEMENT_Attack(bot, ucmd);
			AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
			return ( qtrue );
		}

		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		
		if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
		{
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;
		}

		return ( qtrue );
	}

	if (bot->longTermGoal >= 0)
	{
		if (distance > 8)
		{// In route, either following waypoint path, or moving into actual marked position...
			if (VectorDistance(bot->bot_fireteam_order_coverspot, bot->r.currentOrigin) < 256)
			{// Close enough maybe to move straight to our coverspot directly...
				if (SP_NodeVisible(bot->r.currentOrigin, bot->bot_fireteam_order_coverspot, bot->s.number))
				{// Can see our cover spot.. Go straight there!
					VectorSubtract( bot->bot_fireteam_order_coverspot, bot->r.currentOrigin, bot->move_vector );
					bot->move_vector[ROLL] = 0;
					vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
					BotSetViewAngles( bot, 100 );
					G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

					if (VectorLength(bot->client->ps.velocity) < 16)
					{
						//ucmd->upmove = 127;
						AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
					}
					else if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
					{// Stay down low for this final movement...
						ucmd->upmove = -48;
						bot->client->ps.pm_flags |= PMF_DUCKED;
					}

					return ( qtrue );
				}
			}
		}

		if (VectorDistance(nodes[bot->longTermGoal].origin, bot->bot_fireteam_order_coverspot) > 256
			&& VectorDistance(bot->r.currentOrigin, bot->bot_fireteam_order_coverspot) > 256 )
		{
			new_follow_route = qtrue;
			bot->longTermGoal = -1;
		}
		else if ( bot->enemy
			&& bot->bot_enemy_visible_time > level.time
			&& bot->enemy == bot->bot_last_visible_enemy )
		{// Attack if needed, but keep moving to our commander...
			do_attack_move = qtrue;
			bot->bot_fireteam_attack_in_route = qtrue;
		}
		else
		{
			bot->bot_fireteam_attack_in_route = qfalse;
		}
	}
	else
	{// Need to set goal again!
		new_follow_route = qtrue;
		bot->longTermGoal = -1;
	}
	
	return ( qfalse );
}

void AIMOD_Movement_FireTeam_AI_Order_Supress_VisibleEnemy ( gentity_t *bot )
{// Return's the closest visible enemy to our supression position...
	int i;
	float best_dist = 1024.0f;
	gentity_t *best = NULL;

	if (bot->bot_fireteam_order_supression_enemy_check_time > level.time )
	{// Avoid too many repetative thinks...
		return;
	}

	bot->bot_fireteam_order_supression_enemy_check_time = level.time + 1000;

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		gentity_t *ent = &g_entities[i];
		float dist;

		if (!ent)
			continue;

		if (!ent->client)
			continue;

		if (OnSameTeam(bot, ent))
			continue;

		dist = VectorDistance(bot->bot_fireteam_order_supression_position, ent->r.currentOrigin);

		if (dist < best_dist)
		{
			if (MyVisible(bot, ent))
			{
				best = ent;
				best_dist = dist;
			}
		}
	}

	bot->bot_fireteam_order_supression_enemy = best;
}

int AIMOD_Movement_FireTeam_AI_Order_Supress_NumEnemies ( gentity_t *bot )
{// Return's the closest visible enemy to our supression position...
	int i = 0, count = 0;
	float supress_dist = 512.0f;

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		gentity_t *ent = &g_entities[i];
		float dist;

		if (!ent)
			continue;

		if (!ent->client)
			continue;

		if (OnSameTeam(bot, ent))
			continue;

		dist = VectorDistance(bot->bot_fireteam_order_supression_position, ent->r.currentOrigin);

		if (dist < supress_dist)
		{
			count++;
		}
	}

	return ( count );
}

qboolean AIMOD_MOVEMENT_FireTeam_AI_Order_Supress ( gentity_t *bot, usercmd_t *ucmd, qboolean new_follow_route, qboolean do_attack_move )
{// Supress enemy position...
	vec3_t supress_pos;
	int enemy_count = AIMOD_Movement_FireTeam_AI_Order_Supress_NumEnemies(bot);

	VectorCopy(bot->bot_fireteam_order_supression_position, supress_pos);
	supress_pos[2]+=Q_TrueRand(-16, 16);

	if (enemy_count < 1)
	{// No enemies there. No point in firing.. Just look in supression direction and wait...
		VectorSubtract( supress_pos, bot->r.currentOrigin, bot->move_vector );
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
	
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return ( qtrue );
	}

	// Check for an actual visible enemy to fire at...
	AIMOD_Movement_FireTeam_AI_Order_Supress_VisibleEnemy(bot);

	if (bot->bot_fireteam_order_supression_enemy)
	{// Have an actual enemy position.. Shoot there instead of the default supression position...
		VectorCopy(bot->bot_fireteam_order_supression_enemy->r.currentOrigin, supress_pos);
		supress_pos[2]+=16;
	}

	VectorSubtract( supress_pos, bot->r.currentOrigin, bot->move_vector );
	vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );
	
	ucmd->forwardmove = 0;
	ucmd->rightmove = 0;
	ucmd->upmove = 0;

	ucmd->buttons |= BUTTON_ATTACK;

#ifndef __BOT_NO_SUPRESSION_AMMO_CHEAT__
	if (bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->ps.weapon)] < GetAmmoTableData(BG_FindAmmoForWeapon(bot->client->ps.weapon))->maxammo * 0.75)
	{
		if (bot->bot_fireteam_ammo <= 0) // Give them some ammo clips!
			bot->bot_fireteam_ammo = GetAmmoTableData(BG_FindAmmoForWeapon(bot->client->ps.weapon))->maxammo * 0.75;

		bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->ps.weapon)] = bot->bot_fireteam_ammo;
	}
#endif //__BOT_NO_SUPRESSION_AMMO_CHEAT__

	return ( qtrue );
}

qboolean AIMOD_MOVEMENT_FireTeam_AI_Order_Assault ( gentity_t *bot, usercmd_t *ucmd, qboolean new_follow_route, qboolean do_attack_move )
{// Assault enemy position...
	float distance = VectorDistance(bot->r.currentOrigin, bot->bot_fireteam_order_assault_position);
	qboolean is_visible_enemy = qfalse;
	
	if (bot->enemy && bot->enemy->client)
	{
		is_visible_enemy = MyVisible(bot->enemy, bot);
	}

	if (distance < 258 && is_visible_enemy)
	{
		// Initialize move destination...
		bot->bot_initialized = qtrue;
		VectorCopy(bot->bot_fireteam_order_assault_position, bot->bot_sp_spawnpos);

		AIMOD_SP_MOVEMENT_Move( bot, ucmd );
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}
	else if (is_visible_enemy)
	{
		do_attack_move = qtrue;
		bot->bot_fireteam_attack_in_route = qtrue;
	}
	else
	{
		bot->bot_fireteam_attack_in_route = qfalse;
	}

	if (bot->longTermGoal >= 0)
	{
		if (VectorDistance(nodes[bot->longTermGoal].origin, bot->bot_fireteam_order_assault_position) > 256
			&& distance > 256 )
		{
			new_follow_route = qtrue;
			bot->longTermGoal = -1;
		}
	}
	else
	{// Need to set goal again!
		new_follow_route = qtrue;
		bot->longTermGoal = -1;
	}

	return ( qfalse );
}

void AIMOD_MOVEMENT_FireTeam_AI_Issue_Order ( gentity_t *boss, int type, gentity_t *enemy, vec3_t position )
{
	fireteamData_t* fireTeam;

	if (G_IsOnFireteam(boss->s.number, &fireTeam))
	{// Commander *must* be a real player... Checks here...
		int i;
		gentity_t *leader = &g_entities[(int)fireTeam->joinOrder[0]];

		if (boss != leader)
		{
			return; // We don't own this fireteam.. Igonre the order..
		}

		switch ( type )
		{
		case FT_ORDER_COVER:
			for (i = 1; i < MAX_CLIENTS; i++)
			{
				gentity_t *bot = &g_entities[(int)fireTeam->joinOrder[i]];

				if (!bot)
					continue;

				if (!bot->client)
					continue;

				if (!(bot->r.svFlags & SVF_BOT))
					continue;

				bot->bot_fireteam_order = type;
				VectorCopy(position, bot->bot_fireteam_order_coverspot);

				G_Voice( bot, NULL, SAY_TEAM, va( "Affirmative"), qfalse );
			}

			trap_SendServerCommand( boss - g_entities, "cp \"^3Order Given^4: ^7Take Cover/Move To!\n\"" );
			break;
		case FT_ORDER_SUPRESS:
			for (i = 1; i < MAX_CLIENTS; i++)
			{
				gentity_t *bot = &g_entities[(int)fireTeam->joinOrder[i]];

				if (!bot)
					continue;

				if (!bot->client)
					continue;

				if (!(bot->r.svFlags & SVF_BOT))
					continue;

				bot->bot_fireteam_order = type;
				VectorCopy(position, bot->bot_fireteam_order_supression_position);

				bot->bot_fireteam_ammo = bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->ps.weapon)];
				
				G_Voice( bot, NULL, SAY_TEAM, va( "Affirmative"), qfalse );
			}

			trap_SendServerCommand( boss - g_entities, "cp \"^3Order Given^4: ^7Supress/Stand & Shoot!\n\"" );
			break;
		case FT_ORDER_ASSAULT:
			for (i = 1; i < MAX_CLIENTS; i++)
			{
				gentity_t *bot = &g_entities[(int)fireTeam->joinOrder[i]];

				if (!bot)
					continue;

				if (!bot->client)
					continue;

				if (!(bot->r.svFlags & SVF_BOT))
					continue;

				bot->bot_fireteam_order = type;
				VectorCopy(position, bot->bot_fireteam_order_assault_position);

				G_Voice( bot, NULL, SAY_TEAM, va( "Affirmative"), qfalse );
			}

			trap_SendServerCommand( boss - g_entities, "cp \"^3Order Given^4: ^7Assault/Attack Position!\n\"" );
			break;
		case FT_ORDER_RETURN:
			for (i = 1; i < MAX_CLIENTS; i++)
			{
				gentity_t *bot = &g_entities[(int)fireTeam->joinOrder[i]];

				if (!bot)
					continue;

				if (!bot->client)
					continue;

				if (!(bot->r.svFlags & SVF_BOT))
					continue;

				// Return to boss...
				bot->bot_fireteam_order = FT_ORDER_DEFAULT;

				G_Voice( bot, NULL, SAY_TEAM, va( "Affirmative"), qfalse );
			}

			trap_SendServerCommand( boss - g_entities, "cp \"^3Order Given^4: ^7Return To Commander!\n\"" );
			break;
		case FT_ORDER_TOGGLE_GROUP:
			// UQ1: TODO - Group code!
			break;
		default:
			break;
		}
	}
}


qboolean AIMOD_MOVEMENT_FireTeam_AI_Follow_Orders ( gentity_t *bot, usercmd_t *ucmd, qboolean new_follow_route, qboolean do_attack_move )
{// Does this BOT have specific team orders?
	switch ( bot->bot_fireteam_order )
	{
	case FT_ORDER_DEFAULT:
		return ( qfalse );
	case FT_ORDER_COVER:
		return AIMOD_MOVEMENT_FireTeam_AI_Order_Take_Cover( bot, ucmd, new_follow_route, do_attack_move );
	case FT_ORDER_SUPRESS:
		return AIMOD_MOVEMENT_FireTeam_AI_Order_Supress( bot, ucmd, new_follow_route, do_attack_move );
	case FT_ORDER_ASSAULT:
		return AIMOD_MOVEMENT_FireTeam_AI_Order_Assault( bot, ucmd, new_follow_route, do_attack_move );
	default:
		return ( qfalse );
	}

	return ( qfalse );
}

qboolean AIMOD_MOVEMENT_FireTeam_AI_Check_Orders ( gentity_t *bot )
{// Does this BOT have specific team orders?
	switch ( bot->bot_fireteam_order )
	{
	case FT_ORDER_DEFAULT:
		return ( qfalse );
	case FT_ORDER_COVER:
		return ( qtrue );
	case FT_ORDER_SUPRESS:
		return ( qtrue );
	case FT_ORDER_ASSAULT:
		return ( qtrue );
	default:
		return ( qfalse );
	}

	return ( qfalse );
}

extern qboolean MyVisible_PATH ( gentity_t *self, gentity_t *other );

qboolean AIMOD_MOVEMENT_FireTeam_AI ( gentity_t *bot, usercmd_t *ucmd, qboolean new_follow_route, qboolean do_attack_move )
{// Return qtrue if we need to skip normal movement...
	qboolean have_orders;

	if (!bot->followtarget)
		return ( qfalse );
	
#ifdef __VEHICLES__
	if (bot->followtarget && (bot->client->ps.eFlags & EF_VEHICLE))
	{
		bot->followtarget = NULL;
		G_SecondaryEntityFor(bot->s.number)->s.density = 0; // Remove fireteam sprite above head!
		G_SecondaryEntityFor(bot->s.number)->s.dmgFlags = 0;
		bot->bot_fireteam_order = FT_ORDER_DEFAULT;

		return ( qfalse );
	}

	if (bot->followtarget && bot->followtarget->client && (bot->followtarget->client->ps.eFlags & EF_VEHICLE))
	{
		bot->followtarget = NULL;
		G_SecondaryEntityFor(bot->s.number)->s.density = 0; // Remove fireteam sprite above head!
		G_SecondaryEntityFor(bot->s.number)->s.dmgFlags = 0;
		bot->bot_fireteam_order = FT_ORDER_DEFAULT;

		return ( qfalse );
	}
#endif //__VEHICLES__

	// Need to check if we have specific orders first!
	have_orders = AIMOD_MOVEMENT_FireTeam_AI_Check_Orders( bot );

	if (have_orders)
	{
		qboolean order = AIMOD_MOVEMENT_FireTeam_AI_Follow_Orders( bot, ucmd, new_follow_route, do_attack_move );

		if (order)
		{// Bot has orders and should not use normal movement code, so return qtrue.
			return ( qtrue );
		}
	}

	if (!have_orders && bot->followtarget)
	{
		if (bot->followtarget->r.svFlags & SVF_BOT)
		{
			if (bot->followtarget->current_node > 0 && bot->followtarget->current_node < number_of_nodes)
			{
				if (VectorDistance(nodes[bot->followtarget->current_node].origin, bot->r.currentOrigin) < 64
#ifdef __VEHICLES__
					|| ((bot->followtarget->s.eFlags & EF_VEHICLE) && VectorDistance(nodes[bot->followtarget->current_node].origin, bot->r.currentOrigin) < 192)
					|| ((bot->followtarget->s.eFlags & EF_VEHICLE) && VectorDistance(bot->followtarget->r.currentOrigin, bot->r.currentOrigin) < 192)
#endif //__VEHICLES__
					)
				{// We are in the way.. Move asside so our boss can get through!
					vec3_t angles, forward, move_origin;

					VectorSubtract( nodes[bot->followtarget->current_node].origin, bot->r.currentOrigin, bot->move_vector );
					vectoangles( bot->move_vector, angles );
					AngleVectors (angles, forward, NULL, NULL);

					// Set an origin 96 away from the node to move to...
					VectorCopy(nodes[bot->followtarget->current_node].origin, move_origin);

					VectorMA( move_origin, -96, forward, move_origin );

#ifdef __VEHICLES__
					if (bot->followtarget->s.eFlags & EF_VEHICLE)
						VectorMA( move_origin, -96, forward, move_origin );
#endif //__VEHICLES__

					VectorSubtract( move_origin, bot->r.currentOrigin, bot->move_vector );

					if (VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) < 128
#ifdef __VEHICLES__
						|| ((bot->followtarget->s.eFlags & EF_VEHICLE) && VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) < 192)
#endif //__VEHICLES__
						)
					{// Make them get out of the way quicker!
						ucmd->buttons |= BUTTON_SPRINT;
					}

					BotSetViewAngles( bot, 100 );
					G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
			
					if (VectorLength(bot->client->ps.velocity) < 16)
					{
						//ucmd->upmove = 127;
						AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
					}

					bot->bot_last_good_move_time = level.time;
					return ( qtrue );
				}
			}
		}
#ifdef __VEHICLES__
		else
		{
			if ((bot->followtarget->s.eFlags & EF_VEHICLE) && VectorDistance(bot->followtarget->r.currentOrigin, bot->r.currentOrigin) < 192)
			{// We are in the way.. Move asside so our boss can get through!
				vec3_t angles, forward, move_origin;

				VectorSubtract( nodes[bot->followtarget->current_node].origin, bot->r.currentOrigin, bot->move_vector );
				vectoangles( bot->move_vector, angles );
				AngleVectors (angles, forward, NULL, NULL);

				// Set an origin 96 away from the node to move to...
				VectorCopy(nodes[bot->followtarget->current_node].origin, move_origin);

				VectorMA( move_origin, -96, forward, move_origin );

				if (bot->followtarget->s.eFlags & EF_VEHICLE)
					VectorMA( move_origin, -96, forward, move_origin );

				VectorSubtract( move_origin, bot->r.currentOrigin, bot->move_vector );

				if (VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) < 192)
				{// Make them get out of the way quicker!
					ucmd->buttons |= BUTTON_SPRINT;
				}

				BotSetViewAngles( bot, 100 );
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
			
				if (VectorLength(bot->client->ps.velocity) < 16)
				{
					AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
				}

				bot->bot_last_good_move_time = level.time;
				return ( qtrue );
			}
		}
#endif //__VEHICLES__
	}

	if (!have_orders && bot->followtarget)
	{// For following bots...
		if (VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) < 256
			&& !(bot->followtarget->health <= 0 && bot->client->sess.playerType == PC_MEDIC))
		{// We are with our commander... Use cover spots???
			if (MyVisible(bot, bot->followtarget))
			{
				if ( bot->enemy &&
					bot->beStillEng < level.time &&
					bot->beStill < level.time &&
					bot->bot_enemy_visible_time > level.time &&
					bot->enemy == bot->bot_last_visible_enemy &&
					bot->bot_runaway_time < level.time )
				{// Attack if needed...
#ifdef __OLD_COVER_SPOTS__
					AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
#else //!__OLD_COVER_SPOTS__
					if (number_of_nodes > 0 && bot->current_node)
					{
						//AIMOD_MOVEMENT_AttackMove( bot, ucmd );
						return ( qfalse );
					}
					else
					{
						//AIMOD_MOVEMENT_Attack( bot, ucmd );
						AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
					}
#endif //__OLD_COVER_SPOTS__
					bot->bot_last_good_move_time = level.time;
					return ( qtrue );
				}

				if ( bot->bot_patrol_time > level.time )
				{// A bot that is standing guard...
					// Initialize move destination...
					if (!bot->bot_initialized)
					{
						bot->bot_initialized = qtrue;

						VectorCopy(bot->followtarget->r.currentOrigin, bot->bot_sp_spawnpos);
					}

					AIMOD_SP_MOVEMENT_Move( bot, ucmd );
					bot->bot_last_good_move_time = level.time;
					return ( qtrue );
				}
				else if (bot->bot_duck_time > level.time)
				{
					ucmd->forwardmove = 0;
					ucmd->upmove = 0;
					ucmd->rightmove = 0;
	
					if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
					{
						ucmd->upmove = -48;
						bot->client->ps.pm_flags |= PMF_DUCKED;
					}

					bot->bot_last_good_move_time = level.time;
					return ( qtrue );
				}

				if (Q_TrueRand(0, 1) == 0)
				{
					vec3_t org;

					VectorCopy(bot->followtarget->r.currentOrigin, org);
					org[0]+=64;
					org[1]+=64;

					// Initialize move destination...
					bot->bot_initialized = qtrue;

					VectorCopy(org, bot->bot_sp_spawnpos);
					bot->bot_patrol_time = level.time + 5000;
					AIMOD_SP_MOVEMENT_Move( bot, ucmd );
				}
				else
				{
					ucmd->forwardmove = 0;
					ucmd->upmove = 0;
					ucmd->rightmove = 0;
	
					if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
					{
						ucmd->upmove = -48;
						bot->client->ps.pm_flags |= PMF_DUCKED;
					}

					bot->bot_duck_time = level.time + 5000;
				}

				bot->bot_last_good_move_time = level.time;
				return ( qtrue );
			}
		}
		else if (!(bot->followtarget->health <= 0 && bot->client->sess.playerType == PC_MEDIC)
			&& !(bot->followtarget->client->ps.pm_flags & PMF_LIMBO)
			&& OnSameTeam(bot, bot->followtarget)
			&& MyVisible_PATH(bot, bot->followtarget))
		{// Avoid re-routing if possible...
			//bot->current_node = -1;
			//bot->longTermGoal = -1;

			if (bot->enemy &&
				bot->beStillEng < level.time &&
				bot->beStill < level.time &&
				bot->bot_enemy_visible_time > level.time &&
				bot->enemy == bot->bot_last_visible_enemy &&
				bot->bot_runaway_time < level.time)
			{
				vec3_t enemy_dir;

				VectorSubtract( bot->enemy->r.currentOrigin, bot->r.currentOrigin, enemy_dir );
				vectoangles( enemy_dir, bot->bot_ideal_view_angles );
				VectorSubtract( bot->followtarget->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );

				if (bot->client->ps.weapon != bot->client->sess.playerWeapon && bot->client->ps.weapon != bot->client->sess.playerWeapon2)
				{
					Set_Best_AI_weapon( bot );
				}
				else
				{
					ucmd->buttons |= BUTTON_ATTACK;
				}
			}
			else
			{
				VectorSubtract( bot->followtarget->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );

				if (VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) > 300)
				{// Make them catch up quicker!
					ucmd->buttons |= BUTTON_SPRINT;
				}
			}

			BotSetViewAngles( bot, 100 );
			G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
			
			if (VectorLength(bot->client->ps.velocity) < 16)
			{
				//ucmd->upmove = 127;
				AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
			}

			bot->bot_last_good_move_time = level.time;
			//G_Printf("Bot %s is following bot %s.\n", bot->client->pers.netname, bot->followtarget->client->pers.netname);
			return ( qtrue );
		}

		if (bot->followtarget->health <= 0 && bot->client->sess.playerType == PC_MEDIC)
		{// Medics ALWAYS prioritize healing thier leader!!! Skip all attacking and run for them!
			if (VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) < 256 
				&& MyVisible_PATH(bot, bot->followtarget))
			{// Do the revive!
				if (bot->client->ps.weapon != WP_MEDIC_SYRINGE)
				{
					BOT_ChangeWeapon(bot, WP_MEDIC_SYRINGE);
					VectorSubtract( bot->followtarget->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
					bot->move_vector[ROLL] = -90;
					vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
					BotSetViewAngles( bot, 100 );
					G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

					if (VectorLength(bot->client->ps.velocity) < 16)
					{
						//ucmd->upmove = 127;
						AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
					}
					return ( qtrue );
				}
				else if (VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) < 32)
				{
					VectorSubtract( bot->followtarget->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
					bot->move_vector[ROLL] = -90;
					vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
					BotSetViewAngles( bot, 100 );

					ucmd->upmove = -48;								// Crouch...
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;

					bot->client->ps.pm_flags |= PMF_DUCKED;
					ucmd->buttons = BUTTON_ATTACK;
					return ( qtrue );
				}
				else
				{
					VectorSubtract( bot->followtarget->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
					bot->move_vector[ROLL] = -90;
					vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
					BotSetViewAngles( bot, 100 );
					G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
					
					if (VectorLength(bot->client->ps.velocity) < 16)
					{
						//ucmd->upmove = 127;
						AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );
					}
					return ( qtrue );
				}
			}
			else if (bot->longTermGoal >= 0)
			{
				if (VectorDistance(nodes[bot->longTermGoal].origin, bot->followtarget->r.currentOrigin) > 128
					&& VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) > 128 
					&& bot->bot_fireteam_next_route_update < level.time)
				{
					new_follow_route = qtrue;
					bot->longTermGoal = -1;
					bot->bot_fireteam_next_route_update = level.time + Q_TrueRand(5000, 10000);

					if (bot_debug.integer)
						G_Printf("BOT DEBUG: %s (fireteam) - master moved\n", bot->client->pers.netname);
				}
			}
			else
			{
				if (bot->bot_fireteam_next_route_update < level.time)
				{
					new_follow_route = qtrue;
					bot->longTermGoal = -1;
					bot->bot_fireteam_next_route_update = level.time + Q_TrueRand(5000, 10000);

					if (bot_debug.integer)
						G_Printf("BOT DEBUG: %s (fireteam) - init master's position\n", bot->client->pers.netname);
				}
			}

			ucmd->buttons |= BUTTON_SPRINT;
		}
		else if (VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) < 128
			|| ((bot->followtarget->client->ps.eFlags & EF_VEHICLE) && VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) < 192)
			)
		{// We are with our commander... Use cover spots???
			bot->bot_last_real_move_time = level.time;

			if ( bot->enemy &&
				bot->beStillEng < level.time &&
				bot->beStill < level.time &&
				bot->bot_enemy_visible_time > level.time &&
				bot->enemy == bot->bot_last_visible_enemy &&
				bot->bot_runaway_time < level.time )
			{// Attack if needed...
#ifdef __OLD_COVER_SPOTS__
				AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
#else //!__OLD_COVER_SPOTS__
				if (number_of_nodes > 0 && bot->current_node)
				{
					//AIMOD_MOVEMENT_AttackMove( bot, ucmd );
					return ( qfalse );
				}
				else
				{
					//AIMOD_MOVEMENT_Attack( bot, ucmd );
					AIMOD_SP_MOVEMENT_CoverSpot_Attack(bot, ucmd, qtrue);
				}
#endif //__OLD_COVER_SPOTS__
				return ( qtrue );
			}

			ucmd->forwardmove = 0;
			ucmd->upmove = 0;
			ucmd->rightmove = 0;

			if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
			{
				ucmd->upmove = -48;
				bot->client->ps.pm_flags |= PMF_DUCKED;
			}

			bot->bot_last_good_move_time = level.time;
			return ( qtrue );
		}
		else
		{// Check how far our commander is from our goal...
			if (bot->longTermGoal >= 0)
			{
				if (VectorDistance(nodes[bot->longTermGoal].origin, bot->followtarget->r.currentOrigin) > 128
					&& VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) > 128 
					&& bot->bot_fireteam_next_route_update < level.time )
				{
					new_follow_route = qtrue;
					bot->longTermGoal = -1;
					bot->bot_fireteam_next_route_update = level.time + Q_TrueRand(5000, 10000);
				}
				else if ( bot->enemy &&
					bot->beStillEng < level.time &&
					bot->beStill < level.time &&
					bot->bot_enemy_visible_time > level.time &&
					bot->enemy == bot->bot_last_visible_enemy &&
					bot->bot_runaway_time < level.time )
				{// Attack if needed, but keep moving to our commander...
					do_attack_move = qtrue;
				}
			}
			else
			{
				if (bot->bot_fireteam_next_route_update < level.time)
				{
					new_follow_route = qtrue;
					bot->longTermGoal = -1;
					bot->bot_fireteam_next_route_update = level.time + Q_TrueRand(5000, 10000);
				}
			}
		}

		if (VectorDistance(bot->r.currentOrigin, bot->followtarget->r.currentOrigin) > 512)
		{// Make them catch up quicker!
			ucmd->buttons |= BUTTON_SPRINT;
		}
	}

	return ( qfalse );
}

#endif //__BOT__
