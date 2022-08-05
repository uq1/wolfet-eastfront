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
extern qboolean AIMOD_MOVEMENT_Explosive_Near_Position ( vec3_t position );
extern qboolean AIMOD_MOVEMENT_Avoid_Explosives ( gentity_t *bot, usercmd_t *ucmd );

qboolean			coverspots_generated = qfalse;
static int			num_cover_spots = 0;
static int			cover_nodes[MAX_NODES];

extern vmCvar_t bot_coverspot_render;
int			gCPRenderTime = 0;

/* */
void
BotCoverSpotRender ( void )
{
	int			i;
	gentity_t	*plum;
	int			total_drawn = 0;
	
	if ( !bot_coverspot_render.integer )
	{
		return;
	}

	if ( gCPRenderTime > level.time )
	{
		return;
	}

	gCPRenderTime = level.time + 100;

	for ( i = 0; i < num_cover_spots; i++ )
	{
		if (G_GentitiesAvailable() < MIN_SPARE_GENTITIES)
			break;

		//if ( cover_nodes[i] )
		{
			if (VectorDistance( g_entities[0].r.currentOrigin, nodes[cover_nodes[i]].origin) < 512)
			{
				vec3_t	up;
				VectorCopy( nodes[cover_nodes[i]].origin, up );
				up[2] += 16;//i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.

				total_drawn++;
			}
			/*else
			{
				G_Printf("Spot %i - node %i at (%f %f %f) is out of view range!\n", i, cover_nodes[i], nodes[cover_nodes[i]].origin[0], nodes[cover_nodes[i]].origin[1], nodes[cover_nodes[i]].origin[2]);
			}*/
		}
	}

	if (total_drawn > 0)
		G_Printf("%i (close) cover spots renderred this frame or %i (map total).\n", total_drawn, num_cover_spots);
}

qboolean
AIMOD_SP_MOVEMENT_Is_CoverSpot_Weapon ( int weapon )
{
	qboolean retval = qtrue;

	switch (weapon)
	{
		case WP_NONE:
		case WP_KNIFE:
		case WP_MEDIC_SYRINGE:
		case WP_AMMO:
		case WP_ARTY:
		case WP_DYNAMITE:
		case WP_MAPMORTAR:
		case VERYBIGEXPLOSION:
		case WP_MEDKIT:
		case WP_BINOCULARS:
		case WP_PLIERS:
		case WP_LANDMINE:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
		case WP_TRIPMINE:
		case WP_DUMMY_MG42:
		case WP_MORTAR:
		case WP_LOCKPICK:
		case WP_MORTAR_SET:
		case WP_MEDIC_ADRENALINE:
		case WP_MOBILE_MG42_SET:
#ifdef __EF__
		case WP_30CAL_SET:
#endif //__EF__
			retval = qfalse;
			break;
		default:
			retval = qtrue;
			break;
	}

	return (retval);
}

vmCvar_t fs_game;

qboolean
AIMOD_LoadCoverPoints ( void )
{
	FILE			*pIn;
	vmCvar_t		mapname;
	int				i = 0;
	int				num_map_waypoints = 0;

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	///////////////////
	//open the node file for reading, return false on error
	trap_Cvar_Register( &fs_game, "fs_game", "", CVAR_SERVERINFO | CVAR_ROM );

	if ( (pIn = fopen( va("%s/nodes/%s.cpw", fs_game.string, mapname.string), "rb")) == NULL )
	{
		if ( (pIn = fopen( va("nodes/%s.cpw", mapname.string), "rb")) == NULL )
		{
			G_Printf( "^1*** ^3%s^5: Reading coverpoints from ^7%s/nodes/%s.cpw^3 failed^5!!!\n", GAME_VERSION, fs_game.string, mapname.string );
			return qfalse;
		}
	}

	fread( &num_map_waypoints, sizeof(int), 1, pIn );

	if (num_map_waypoints != number_of_nodes)
	{// Is an old file! We need to make a new one!
		G_Printf( "^1*** ^3%s^5: Reading coverpoints from ^7%s/nodes/%s.cpw^3 failed^5!!!\n", GAME_VERSION, fs_game.string, mapname.string );
		fclose( pIn );
		return qfalse;
	}

	fread( &num_cover_spots, sizeof(int), 1, pIn );
	for ( i = 0; i < num_cover_spots; i++ )
	{
		fread( &cover_nodes[i], sizeof(int), 1, pIn );
		nodes[cover_nodes[i]].type |= NODE_COVER;

		//G_Printf("Cover spot #%i (node %i) is at %f %f %f.\n", i, cover_nodes[i], nodes[cover_nodes[i]].origin[0], nodes[cover_nodes[i]].origin[1], nodes[cover_nodes[i]].origin[2]);
	}

	fclose( pIn );

	G_Printf( "^1*** ^3%s^5: Successfully loaded %i cover points from file ^7%s/nodes/%s.cpw^5.\n", GAME_VERSION, num_cover_spots, fs_game.string, mapname.string);

	return qtrue;
}

void
AIMOD_SaveCoverPoints ( void )
{
	fileHandle_t	f;
	int				i;
	vmCvar_t		mapname;
	char			map[64] = "";
	char			filename[60];

	G_Printf( "^3*** ^3%s: ^7Saving cover point table.\n", "AUTO-WAYPOINTER" );
	strcpy( filename, "nodes/" );

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	Q_strcat( filename, sizeof(filename), mapname.string );

	///////////////////
	//try to open the output file, return if it failed
	trap_FS_FOpenFile( va( "%s.cpw", filename), &f, FS_WRITE );
	if ( !f )
	{
		G_Printf( "^1*** ^3ERROR^5: Error opening cover point file ^7%s.cpw^5!!!\n", "AUTO-WAYPOINTER", filename );
		return;
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );	//get the map name
	strcpy( map, mapname.string );

	trap_FS_Write( &number_of_nodes, sizeof(int), f );							//write the number of nodes in the map
									//write the map name
	trap_FS_Write( &num_cover_spots, sizeof(int), f );							//write the number of nodes in the map

	for ( i = 0; i < num_cover_spots; i++ )											//loop through all the nodes
	{
		trap_FS_Write( &(cover_nodes[i]), sizeof(int), f );
	}

	trap_FS_FCloseFile( f );		

	G_Printf( "^3*** ^3%s: ^5Cover point table saved to file ^7%s.cpw^5.\n", "AUTO-WAYPOINTER", filename );
}

void AIMOD_Generate_Cover_Spots_Real ( void )
{
	int x = 0;

	num_cover_spots = 0;

	for (x = 0; x < MAX_NODES; x++)
	{// Init them!
		cover_nodes[x] = -1;
		
		if (x < number_of_nodes && nodes[x].type & NODE_COVER)
			nodes[x].type &= ~NODE_COVER;
	}

	{// Need to make some from waypoint list if we can!
		if (number_of_nodes > 40000)
		{
			G_Printf("^3*** ^3%s: ^5Too many waypoints to make cover spots. Use ^3/awo^5 (auto-waypoint optimizer) to reduce the numbers!\n", "AUTO-WAYPOINTER");
			return;
		}

		if (number_of_nodes > 0)
		{
			int i = 0;

			G_Printf( "^1*** ^3%s^1: ^5Generating and saving coverspot waypoints list.\n", "AUTO-WAYPOINTER" );

			for (i = 0; i < number_of_nodes; i++)
			{
				int		j = 0;
				vec3_t	up_org2;

				VectorCopy(nodes[i].origin, up_org2);
				up_org2[2]+=64;//DEFAULT_VIEWHEIGHT; // Standing height!

				for (j = 0; j < number_of_nodes; j++)
				{
					if (nodes[j].type & NODE_COVER)
						continue;

					if (VectorDistance(nodes[i].origin, nodes[j].origin) < 512
						&& (HeightDistance(nodes[i].origin, nodes[j].origin) < 24 || nodes[i].origin < nodes[j].origin))
					{
						vec3_t up_org;

						VectorCopy(nodes[j].origin, up_org);
						up_org[2]+=32;//CROUCH_VIEWHEIGHT; // Crouch height!

						if (OrgVisible(up_org, up_org2, -1))
						{
							continue;
						}
						else
						{
							VectorCopy(nodes[j].origin, up_org);
							up_org[2]+=24;//CROUCH_VIEWHEIGHT; // Crouch height!

							if (OrgVisible(up_org, up_org2, -1))
							{
								continue;
							}
							else
							{
								VectorCopy(nodes[j].origin, up_org);
								up_org[2]+=DEFAULT_VIEWHEIGHT; // Standing height!

								if (OrgVisible(up_org, up_org2, -1))
								{
									nodes[j].type |= NODE_COVER;
									cover_nodes[num_cover_spots] = j;
									num_cover_spots++;
									break;
								}
							}
						}
					}
				}
			}

			//if (bot_debug.integer)
			{
				G_Printf( "^1*** ^3%s^1: ^5 Generated ^7%i^5 coverspot waypoints.\n", "AUTO-WAYPOINTER", num_cover_spots );
			}

			// Save them for fast loading next time!
			AIMOD_SaveCoverPoints();
		}
	}
}

void AIMOD_Generate_Cover_Spots ( void )
{
	// Try loading from a file first (for speed)
	if (number_of_nodes > 0)
		if (AIMOD_LoadCoverPoints())
			return;

	if (!coverspots_generated)
	{// Need to make some from waypoint list if we can!
		coverspots_generated = qtrue;

		if (number_of_nodes > 40000)
			return;

		AIMOD_Generate_Cover_Spots_Real();
	}
}

void SP_Axis_Cover_Spot ( gentity_t *ent )
{
	G_FreeEntity(ent);
}

void SP_Allied_Cover_Spot ( gentity_t *ent )
{
	G_FreeEntity(ent);
}

extern vmCvar_t bot_coverspot_enable;
extern qboolean	MyVisible_No_Supression ( gentity_t *self, gentity_t *other );
extern qboolean MyInFOV2 ( vec3_t origin, gentity_t *from, int hFOV, int vFOV );
qboolean EnemyVisible ( vec3_t from, gentity_t *other, int ignoreEnt );
qboolean CoverspotEnemyVisible ( vec3_t from, gentity_t *other, int ignoreEnt );
extern int CreatePathAStar ( gentity_t *bot, int from, int to, /*short*/ int *pathlist );

void 
AIMOD_SP_MOVEMENT_CoverSpot_Do_Actual_Attack ( gentity_t *bot, usercmd_t *ucmd )
{
	if (bot->bot_reload_time >= level.time)
		return;

	CalcMuzzlePoints(bot, bot->client->ps.weapon);

	if (bot->client->ps.ammoclip[BG_FindClipForWeapon(bot->client->ps.weapon)] > 0)
	{
		if (bot->client->ps.weaponstate != WEAPON_RAISING
			&& bot->client->ps.weaponstate != WEAPON_RAISING_TORELOAD
			&& bot->client->ps.weaponstate != WEAPON_DROPPING
			&& bot->client->ps.weaponstate != WEAPON_DROPPING_TORELOAD
			//&& bot->client->ps.weaponstate != WEAPON_READYING	// getting from 'ready' to 'firing'
			//&& bot->client->ps.weaponstate != WEAPON_RELAXING	// weapon is ready, but since not firing, it's on it's way to a "relaxed" stance
			//&& bot->client->ps.weaponstate != WEAPON_FIRING
			//&& bot->client->ps.weaponstate != WEAPON_FIRINGALT
			&& bot->client->ps.weaponstate != WEAPON_RELOADING )
		{
			if (bot->next_fire_time <= level.time)
			{
				vec3_t		selfView, otherView;
				trace_t		tr;

				VectorCopy(bot->muzzlePoint, selfView);

				VectorCopy(bot->enemy->r.currentOrigin, otherView);

#ifdef __NPC__
				if ((bot->enemy && bot->enemy->client)
					&& ((bot->enemy->client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->client->ps.eFlags & EF_CROUCHING)))
						otherView[2] -= 20; // Aim a little lower if they are crouching...
				else if ((bot->enemy && bot->enemy->NPC_client)
					&& ((bot->enemy->NPC_client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->NPC_client->ps.eFlags & EF_CROUCHING)))
#else //!__NPC__
				if ((bot->enemy->client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->client->ps.eFlags & EF_CROUCHING))
#endif //__NPC__
					otherView[2] -= 20; // Aim a little lower if they are crouching...

#ifdef __NPC__
				if ((bot->enemy && bot->enemy->client)
					&& (bot->enemy->client->ps.eFlags & EF_PRONE))
					otherView[2] -= 30;//48; // Aim even lower if they are prone!
				else if ((bot->enemy && bot->enemy->NPC_client)
					&& (bot->enemy->NPC_client->ps.eFlags & EF_PRONE))
#else //!__NPC__
				if (bot->enemy->client->ps.eFlags & EF_PRONE)
#endif //__NPC__
					otherView[2] -= 30;//48; // Aim even lower if they are prone!

				trap_Trace( &tr, selfView, NULL, NULL, otherView, bot->s.number, MASK_SHOT );

				if (tr.entityNum >= 0)
				{
					gentity_t *hit = &g_entities[tr.entityNum];

					if (hit && (hit->client || hit->NPC_client))
						if (OnSameTeam(bot, hit))
							return; // Do not shoot friendly players...
				}

				ucmd->buttons = BUTTON_ATTACK;

				bot->bot_last_attack_time = level.time;

				if (BG_IsScopedWeapon(bot->s.weapon))
				{// Longer delay between sniper shots to aim!
					bot->next_fire_time = level.time + 3500;
				}
				else if (VectorDistance(bot->r.currentOrigin, bot->enemy->r.currentOrigin) >= 600 && Q_irand(0,4) == 4)
				{// Force a delay between firing normal weapons too (sometimes)... Use bursts, don't waste ammo!
					bot->next_fire_time = level.time + 2000;
				}
				else
				{
					bot->next_fire_time = 0;
				}
			}
			return;
		}
	}
	else if (bot->client->ps.ammoclip[BG_FindClipForWeapon(bot->client->ps.weapon)] <= 0 && bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->ps.weapon)] <= 0)
	{// Need a new weapon!
		bot->bot_snipetime = 0;
		Set_Best_AI_weapon( bot );
		return;
	}
	else
	{// Need a reload!
		ucmd->wbuttons = WBUTTON_RELOAD;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = -48;
		bot->bot_reload_time = level.time + 4000;
		return;
	}
}

void AIMOD_SP_MOVEMENT_CoverSpot_Aim_At_Enemy ( gentity_t *bot )
{
	vec3_t enemy_dir, enemy_org;

	VectorCopy(bot->enemy->r.currentOrigin, enemy_org);

#ifdef __NPC__
	if ((bot->enemy && bot->enemy->client)
		&& ((bot->enemy->client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->client->ps.eFlags & EF_CROUCHING)))
			enemy_org[2] -= 32; // Aim a little lower if they are crouching...
	else if ((bot->enemy && bot->enemy->NPC_client)
		&& ((bot->enemy->NPC_client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->NPC_client->ps.eFlags & EF_CROUCHING)))
#else //!__NPC__
	if ((bot->enemy->client->ps.pm_flags & PMF_DUCKED) || (bot->enemy->client->ps.eFlags & EF_CROUCHING))
#endif //__NPC__
		enemy_org[2] -= 32; // Aim a little lower if they are crouching...

#ifdef __NPC__
	if ((bot->enemy && bot->enemy->client)
		&& (bot->enemy->client->ps.eFlags & EF_PRONE))
		enemy_org[2] -= 48;//48; // Aim even lower if they are prone!
	else if ((bot->enemy && bot->enemy->NPC_client)
		&& (bot->enemy->NPC_client->ps.eFlags & EF_PRONE))
#else //!__NPC__
	if (bot->enemy->client->ps.eFlags & EF_PRONE)
#endif //__NPC__
	enemy_org[2] -= 48;//48; // Aim even lower if they are prone!

	VectorSubtract( enemy_org, bot->r.currentOrigin, enemy_dir );
	vectoangles( enemy_dir, bot->bot_ideal_view_angles );

	VectorSubtract( bot->enemy->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
	vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );
}

void AIMOD_Bot_Aim_At_Object ( gentity_t *bot, gentity_t *object )
{
	vec3_t enemy_org;

	if (!object)
		return;

	VectorCopy(object->s.origin, enemy_org);
	VectorSubtract( object->s.origin, bot->r.currentOrigin, bot->move_vector );
	vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );
}

void AIMOD_SP_MOVEMENT_CoverSpot_Attack ( gentity_t *bot, usercmd_t *ucmd, qboolean useProne )
{
	vec3_t enemy_dir, enemy_org;
	qboolean inFOV = qfalse;

	bot->bot_last_attack_time = level.time;

	if (!AIMOD_SP_MOVEMENT_Is_CoverSpot_Weapon(bot->client->ps.weapon))
	{
		Set_Best_AI_weapon( bot );
		return;
	}

#ifndef __BOT_NO_AMMO_CHEAT__
	if (COM_BitCheck( bot->client->ps.weapons, WP_GPG40))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_GPG40)] = 1;

	if (COM_BitCheck( bot->client->ps.weapons, WP_CARBINE))
		bot->client->ps.ammoclip[BG_FindClipForWeapon(WP_CARBINE)] = 1;
#endif //__BOT_NO_AMMO_CHEAT__

	if (bot->bot_duck_time < level.time - 5000
		|| bot->supression >= 80
		|| (bot->client->ps.ammoclip[BG_FindClipForWeapon(bot->client->ps.weapon)] <= 0 && bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->ps.weapon)] > 0))
	{
		AIMOD_SP_MOVEMENT_CoverSpot_Aim_At_Enemy( bot );

		if (bot->enemy && bot->bot_enemy_visible_time >= level.time && MyInFOV(bot->enemy, bot, 12, 25))
		{// Still shoot if we can hit them! Cover spots are not always perfect!
			AIMOD_SP_MOVEMENT_CoverSpot_Do_Actual_Attack( bot, ucmd );
			return;
		}

		if (bot->supression >= 100
			|| (bot->supression >= 80 && Q_TrueRand(0, 9) < 9)
			|| (bot->supression >= 50 && Q_TrueRand(0, 9) < 8)
			|| (bot->supression >= 30 && Q_TrueRand(0, 9) < 7)
			|| (bot->supression >= 20 && Q_TrueRand(0, 9) < 6)
			|| Q_TrueRand(0, 9) < 5
			|| (bot->client->ps.ammoclip[BG_FindClipForWeapon(bot->client->ps.weapon)] <= 0 && bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->ps.weapon)] > 0))
		{// Good chance that they will duck!
			bot->bot_duck_time = level.time + Q_TrueRand(2000, 5000);
			
			if ((bot->client->ps.eFlags & EF_AIMING))
				bot->client->ps.eFlags &= ~EF_AIMING;

			ucmd->upmove = -127;
			
			if (useProne)
			{
				if (!(bot->client->ps.eFlags & EF_PRONE) && !(bot->client->ps.eFlags & EF_PRONE_MOVING))
					bot->client->ps.eFlags |= EF_PRONE;

				if (bot->client->ps.pm_flags & PMF_DUCKED)
					bot->client->ps.pm_flags &= ~PMF_DUCKED;

				if (bot->client->ps.eFlags & EF_CROUCHING)
					bot->client->ps.eFlags &= ~EF_CROUCHING;
			}
			else
			{
				if (!(bot->client->ps.pm_flags & PMF_DUCKED))
					bot->client->ps.pm_flags |= PMF_DUCKED;

				if (bot->client->ps.eFlags & EF_PRONE)
					bot->client->ps.eFlags &= ~EF_PRONE;

				if (bot->client->ps.eFlags & EF_PRONE_MOVING)
					bot->client->ps.eFlags &= ~EF_PRONE_MOVING;
			}

			//if (!(bot->client->ps.eFlags & EF_CROUCHING))
			//	bot->client->ps.eFlags |= EF_CROUCHING;

			if ( bot->client->ps.ammoclip[BG_FindClipForWeapon(bot->client->ps.weapon)] < GetAmmoTableData(BG_FindAmmoForWeapon(bot->client->ps.weapon))->maxclip
				&& bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->ps.weapon)] > 0 )
			{// Shall we reload while ducked???
				ucmd->wbuttons = WBUTTON_RELOAD;
			}

			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = -48;
			return;
		}
	}

	if (bot->bot_duck_time > level.time)
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = -48;

		AIMOD_SP_MOVEMENT_CoverSpot_Aim_At_Enemy( bot );

		if (bot->enemy && bot->bot_enemy_visible_time >= level.time && MyInFOV(bot->enemy, bot, 12, 25))
		{// Still shoot if we can hit them! Cover spots are not always perfect!
			AIMOD_SP_MOVEMENT_CoverSpot_Do_Actual_Attack( bot, ucmd );
			return;
		}
			
		if (useProne)
		{
			if (!(bot->client->ps.eFlags & EF_PRONE) && !(bot->client->ps.eFlags & EF_PRONE_MOVING))
				bot->client->ps.eFlags |= EF_PRONE;

			if (bot->client->ps.pm_flags & PMF_DUCKED)
				bot->client->ps.pm_flags &= ~PMF_DUCKED;

			if (bot->client->ps.eFlags & EF_CROUCHING)
				bot->client->ps.eFlags &= ~EF_CROUCHING;
		}
		else
		{
			if (!(bot->client->ps.pm_flags & PMF_DUCKED))
				bot->client->ps.pm_flags |= PMF_DUCKED;

			if (bot->client->ps.eFlags & EF_PRONE)
				bot->client->ps.eFlags &= ~EF_PRONE;

			if (bot->client->ps.eFlags & EF_PRONE_MOVING)
				bot->client->ps.eFlags &= ~EF_PRONE_MOVING;
		}

		if ( bot->client->ps.ammoclip[BG_FindClipForWeapon(bot->client->ps.weapon)] < GetAmmoTableData(BG_FindAmmoForWeapon(bot->client->ps.weapon))->maxclip
			&& bot->client->ps.ammo[BG_FindAmmoForWeapon(bot->client->ps.weapon)] > 0 )
		{// Shall we reload while ducked???
			ucmd->wbuttons = WBUTTON_RELOAD;
		}
		return;
	}
	else
	{
		if (bot->client->ps.pm_flags & PMF_DUCKED)
			bot->client->ps.pm_flags &= ~PMF_DUCKED;

		if (bot->client->ps.eFlags & EF_CROUCHING)
			bot->client->ps.eFlags &= ~EF_CROUCHING;

		if (bot->client->ps.eFlags & EF_PRONE)
			bot->client->ps.eFlags &= ~EF_PRONE;

		if (bot->client->ps.eFlags & EF_PRONE_MOVING)
			bot->client->ps.eFlags &= ~EF_PRONE_MOVING;
	}

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
		enemy_org[2] -= 30;//48; // Aim even lower if they are prone!
	else if ((bot->enemy && bot->enemy->NPC_client)
		&& (bot->enemy->NPC_client->ps.eFlags & EF_PRONE))
#else //!__NPC__
	if (bot->enemy->client->ps.eFlags & EF_PRONE)
#endif //__NPC__
		enemy_org[2] -= 30;//48; // Aim even lower if they are prone!

	VectorSubtract( enemy_org, bot->r.currentOrigin, enemy_dir );
	vectoangles( enemy_dir, bot->bot_ideal_view_angles );
	
	if ( bot->client->ps.weapon == WP_SMOKE_MARKER
		|| bot->client->ps.weapon == WP_GRENADE_LAUNCHER 
		|| bot->client->ps.weapon == WP_GRENADE_PINEAPPLE
		|| bot->client->ps.weapon == WP_GPG40 
		|| bot->client->ps.weapon == WP_M7 )
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

		// Set up ideal view angles for this enemy...
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		BotSetViewAngles( bot, 100 );

		inFOV = MyInFOV2(enemy_org, bot, 12, 12);
	}
	else
	{
		AIMOD_SP_MOVEMENT_CoverSpot_Aim_At_Enemy( bot );

		//BotSetViewAngles( bot, 100 );
		//inFOV = qtrue;
		 inFOV = MyInFOV(bot->enemy, bot, 12, 25);
	}

	// Artillery...
	if ( bot->enemy
		&& VectorDistance(bot->bot_ideal_view_angles, bot->client->ps.viewangles) <= 4
		&& next_arty[bot->s.clientNum] < level.time 
		&& bot->client->ps.stats[STAT_PLAYER_CLASS] == PC_FIELDOPS )
	{
		if (VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 768 
			&& Can_Arty( bot, bot->enemy->r.currentOrigin))
		{
			if ( MyInFOV(bot->enemy, bot, 3, 3))
			{
				int skill_level = bot->skillchoice;

				if (skill_level <= 0)
					skill_level = 1;

				Weapon_Artillery( bot );
				next_arty[bot->s.clientNum] = level.time + ( 50000 /  skill_level );
				return;
			}
		}
	}

	if (inFOV && bot->bot_end_fire_next_frame)
	{// Make sure we release the grenade the frame after we hit attack, or it won't release!
		ucmd->buttons &= ~BUTTON_ATTACK;
		bot->bot_end_fire_next_frame = qfalse;
			
		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}
	else if (inFOV)
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
		else if (!bot->bot_fireteam_attack_in_route)
		{// Walk for other weapons...
			if (!(ucmd->buttons & BUTTON_WALKING))
				ucmd->buttons |= BUTTON_WALKING;

#ifdef __WEAPON_AIM__
			if (!(bot->client->ps.eFlags & EF_AIMING))
				bot->client->ps.eFlags |= EF_AIMING;
#endif //__WEAPON_AIM__
		}

		if (inFOV && bot->bot_end_fire_next_frame /*&& MyInFOV(bot->enemy, bot, 45, 45)*/)
		{// Throw grenade/smoke marker...
			CalcMuzzlePoints(bot, bot->client->ps.weapon);

			//if (VectorDistance(bot->r.currentOrigin, bot->enemy->r.currentOrigin) < 256 
			//	|| MyVisible_No_Supression(bot, bot->enemy))
			{
				ucmd->buttons |= BUTTON_ATTACK;
				return;
			}
			/*else
			{
				VectorSubtract( bot->enemy->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
				BotSetViewAngles( bot, 100 );
			}*/

			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
		}
	}

	if (inFOV)
		AIMOD_SP_MOVEMENT_CoverSpot_Do_Actual_Attack( bot, ucmd );

	ucmd->forwardmove = 0;
	ucmd->rightmove = 0;
	ucmd->upmove = 0;
}

qboolean AIMOD_SP_MOVEMENT_CoverSpot_BadNodeType ( int node )
{
	if (nodes[node].type & NODE_ICE)
		return qtrue;

	if (nodes[node].type & NODE_ICE)
		return qtrue;

	if (nodes[node].type & NODE_WATER)
		return qtrue;

	return qfalse;
}

int AIMOD_SP_MOVEMENT_CoverSpot_FindCoverSpot ( gentity_t *bot )
{
	int		best_node = -1;
	int		i = 0;
	float	best_dist = 4096.0f;
	int		num_in_range_spots = 0;
	int		in_range_spots[MAX_NODES];

	if (bot->bot_next_goalfind_time > level.time)
		return -1;

	bot->bot_next_goalfind_time = level.time + 5000;

	//if (number_of_nodes > 16000)
	//	best_dist = 2048.0f; // Less CPU usage with large waypoint list!

	for (i = 0; i < num_cover_spots; i++)
	{
		float	distance = VectorDistance(nodes[cover_nodes[i]].origin, bot->r.currentOrigin);

		if (distance >= best_dist)
			continue;

		if (AIMOD_SP_MOVEMENT_CoverSpot_BadNodeType(cover_nodes[i]))
			continue;

		in_range_spots[num_in_range_spots] = i;
		num_in_range_spots++;
	}

	//G_Printf("%s - Num in-range spots is %i [1st pass].\n", bot->client->pers.netname, num_in_range_spots);

	if (num_in_range_spots > 256)
	{
		best_dist = 2048.0f; // Less CPU usage with large waypoint list!
		num_in_range_spots = 0;

		for (i = 0; i < num_cover_spots; i++)
		{
			float	distance = VectorDistance(nodes[cover_nodes[i]].origin, bot->r.currentOrigin);

			if (distance >= best_dist)
				continue;

			if (AIMOD_SP_MOVEMENT_CoverSpot_BadNodeType(cover_nodes[i]))
				continue;

			in_range_spots[num_in_range_spots] = i;
			num_in_range_spots++;
		}

		//G_Printf("%s - Num in-range spots is %i [2nd pass].\n", bot->client->pers.netname, num_in_range_spots);
	}

	if (num_in_range_spots > 256)
	{
		best_dist = 1024.0f; // Less CPU usage with large waypoint list!
		num_in_range_spots = 0;

		for (i = 0; i < num_cover_spots; i++)
		{
			float	distance = VectorDistance(nodes[cover_nodes[i]].origin, bot->r.currentOrigin);

			if (distance >= best_dist)
				continue;

			if (AIMOD_SP_MOVEMENT_CoverSpot_BadNodeType(cover_nodes[i]))
				continue;

			in_range_spots[num_in_range_spots] = i;
			num_in_range_spots++;
		}

		//G_Printf("%s - Num in-range spots is %i [3rd pass].\n", bot->client->pers.netname, num_in_range_spots);
	}

	if (num_in_range_spots > 256)
	{
		best_dist = 512.0f; // Less CPU usage with large waypoint list!
		num_in_range_spots = 0;

		for (i = 0; i < num_cover_spots; i++)
		{
			float	distance = VectorDistance(nodes[cover_nodes[i]].origin, bot->r.currentOrigin);

			if (distance >= best_dist)
				continue;

			if (AIMOD_SP_MOVEMENT_CoverSpot_BadNodeType(cover_nodes[i]))
				continue;

			in_range_spots[num_in_range_spots] = i;
			num_in_range_spots++;
		}

		//G_Printf("%s - Num in-range spots is %i [4th pass].\n", bot->client->pers.netname, num_in_range_spots);
	}

	for (i = 0; i < num_in_range_spots; i++)
	{
		vec3_t	up_org;
		float	distance = VectorDistance(nodes[cover_nodes[in_range_spots[i]]].origin, bot->r.currentOrigin);

		VectorCopy(nodes[cover_nodes[in_range_spots[i]]].origin, up_org);
		up_org[2]+=32;//CROUCH_VIEWHEIGHT; // Crouch height!

		if (CoverspotEnemyVisible(up_org, bot->enemy, bot->s.number))
			continue;

		VectorCopy(nodes[cover_nodes[in_range_spots[i]]].origin, up_org);
		up_org[2]+=DEFAULT_VIEWHEIGHT; // Standing height!

		if (CoverspotEnemyVisible(up_org, bot->enemy, bot->s.number))
		{
			int			j = 0;
			qboolean	bad = qfalse;

			for (j = 0; j < MAX_CLIENTS; j++)
			{
				gentity_t *test = &g_entities[j];

				if (!test)
					continue;

				if (!test->client)
					continue;

				if (VectorDistance(nodes[cover_nodes[in_range_spots[i]]].origin, test->r.currentOrigin) < 32)
				{
					bad = qtrue; // Someone else is already there!
					break;
				}

				if (!(test->r.svFlags & SVF_BOT))
				{
					bad = qtrue; // Don't bother to check non-player's waypoints...
					break;
				}

				if (test->longTermGoal == cover_nodes[in_range_spots[i]] || (test->current_node == cover_nodes[in_range_spots[i]] && test->enemy))
				{
					bad = qtrue; // Another bot with the same goal, or us using this spot!
					break;
				}
			}

			if (!bad)
			{
				best_node = cover_nodes[in_range_spots[i]];
				best_dist = distance;
			}
		}
	}

	//if (best_node)
	//	G_Printf("%s - Found a cover spot!\n", bot->client->pers.netname);
	//else
	//	G_Printf("%s - Did not find a cover spot!!!\n", bot->client->pers.netname);

	return best_node;
}

extern vmCvar_t g_allWeaponsDamageTanks;

//===========================================================================
// Routine      : AIMOD_SP_MOVEMENT_CoverSpot_Movement

// Description  : Single Player AI cover spot usage...
void
AIMOD_SP_MOVEMENT_CoverSpot_Movement ( gentity_t *bot, usercmd_t *ucmd )
{
	qboolean find_new_coverspot = qfalse;
	qboolean force_attack = qfalse;
	qboolean useProne = qfalse;
	qboolean bad_node_type = qfalse;

#ifndef __NO_SNIPER_POINTS__
	if (bot->bot_snipetime >= level.time || (nodes_loaded && bot->goalentity && !Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot"))))
	{
		if ( AIMOD_MOVEMENT_SniperFunc( bot, ucmd, qfalse ) )
			return;
	}
#endif //__NO_SNIPER_POINTS__

	if (!bot->enemy)
	{
		bot->bot_coverspot_enemy = NULL;
		AIMOD_MOVEMENT_Move( bot, ucmd );
		return;
	}

#ifdef __NPC__
	if (bot->enemy && bot->enemy->s.eType == ET_NPC)
	{
		if (bot->enemy->health <= 0 || OnSameTeam(bot, bot->enemy))
		{
			bot->enemy = NULL;
			bot->bot_coverspot_enemy = NULL;
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
	}
#endif //__NPC__

	if (bot->enemy)
	{
		if (bot->supression <= 20)
		{// When not supressed, dont use cover!
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
		else if (bot->supression <= 40 && Q_TrueRand(0, 100) > 1)
		{// When only lightly supressed, dont use cover as often!
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
		else if (bot->supression <= 60 && Q_TrueRand(0, 100) > 20)
		{// When only partially supressed, dont use cover as often!
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
		else if (bot->enemy->bot_coverspot_enemy && bot->enemy->bot_coverspot_enemy == bot)
		{// If our enemy is using a cover spot, don't use one ourself, avoid stalemate...
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
		else if (bot->client->ps.stats[STAT_HEALTH] <= 25)
		{// Low health, take cover with anything except for a knife... (they should make a run for their current goal instead -- possibly health/ammo?)
			switch (bot->client->ps.weapon)
			{
			case WP_KNIFE:
				AIMOD_MOVEMENT_Move( bot, ucmd );
				return;
			default:
				break;
			}
		}
		else if ((bot->enemy->s.eFlags & EF_PRONE) || (bot->enemy->s.eFlags & EF_CROUCHING))
		{// If the enemy is prone/crouching at cover, then we should not... Avoid stalemate!
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
		else if (VectorDistance(bot->enemy->r.currentOrigin, bot->r.currentOrigin) < 300)
		{// If close, do attack move instead... Avoid stalemates with grenades, etc...
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
		else if (!(bot->client->ps.weapon == WP_FG42SCOPE || bot->client->ps.weapon == WP_FG42 || bot->client->ps.weapon == WP_K43_SCOPE || bot->client->ps.weapon == WP_K43 || bot->client->ps.weapon == WP_GARAND_SCOPE || bot->client->ps.weapon == WP_GARAND || bot->client->ps.weapon == WP_PANZERFAUST)
			&& VectorDistance(bot->enemy->r.currentOrigin, bot->r.currentOrigin) > 2048)
		{// If distant and we are not a sniper, do attack move instead...
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
		else if ( bot->client->ps.powerups[PW_REDFLAG] || bot->client->ps.powerups[PW_BLUEFLAG] || botai.flagstate[bot->client->ps.teamNum].carrier == bot->s.clientNum )
		{// Carrying the flag! Never stop...
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
		else if ( bot->bot_after_ammo )
		{// After ammo! Never stop...
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
		else if ( (bot->bot_explosive_avoid_waypoint >= 0 || (bot->current_node && AIMOD_MOVEMENT_Explosive_Near_Waypoint(bot->current_node)))
			&& !AIMOD_MOVEMENT_Avoid_Explosives ( bot, ucmd ))
		{// Avoiding an explosion, keep moving!
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
#ifndef __BOT_EXPLOSIVE_AVOID_CPU_SAVER__
		else if ( (bot->bot_explosive_avoid_waypoint >= 0 || AIMOD_MOVEMENT_Explosive_Near_Position( bot->r.currentOrigin ))
			&& !AIMOD_MOVEMENT_Avoid_Explosives ( bot, ucmd ))
		{// Avoiding an explosion, keep moving!
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
#endif //__BOT_EXPLOSIVE_AVOID_CPU_SAVER__
		else
		{// If using a pistol or knife, don't take cover... (they should make a run for their current goal instead -- possibly ammo?)
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
					bot->bot_after_ammo = qtrue;
					bot->current_node = -1;
					bot->next_node = -1;
					bot->last_node = -1;
					bot->longTermGoal = -1;
					bot->goalentity = NULL;
				}
				AIMOD_MOVEMENT_Move( bot, ucmd );
				return;
			default:
				break;
			}
		}

#ifdef __VEHICLES__
		if (bot->enemy->client && (bot->enemy->client->ps.eFlags & EF_VEHICLE))
		{// Enemy is a tank.. Don't take cover, shoot while running away (but only if our goal is distant)!
			if (bot->longTermGoal > 0 && bot->longTermGoal < number_of_nodes)
			{// We got a long term goal...
				if (VectorDistance(bot->r.currentOrigin, nodes[bot->longTermGoal].origin) > 2048)
				{// Goal is distant, check visibility to the enemy...
					if (!SP_NodeVisible(bot->enemy->r.currentOrigin, nodes[bot->longTermGoal].origin, bot->enemy->s.number))
					{// Our goal is not visible to them, keep running away while shooting...
						AIMOD_MOVEMENT_Move( bot, ucmd );
						return;
					}
				}
			}
		}
#endif //__VEHICLES__
	}

	if (bot->enemy && bot->enemy->client)
	{
		if (bot->enemy->client->ps.stats[STAT_HEALTH] <= 0
			|| (bot->enemy->client->ps.pm_flags & PMF_LIMBO) 
			|| bot->enemy->client->sess.sessionTeam == TEAM_SPECTATOR
			|| OnSameTeam(bot, bot->enemy))
		{
			bot->enemy = NULL;
			bot->bot_coverspot_enemy = NULL;
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}

#ifdef __VEHICLES__
		if ((bot->enemy->client->ps.eFlags & EF_VEHICLE) 
			&& !(bot->client->ps.eFlags & EF_VEHICLE) 
			&& bot->client->ps.weapon != WP_PANZERFAUST
			&& bot->client->ps.weapon != WP_SMOKE_MARKER
			&& bot->client->ps.weapon != WP_ARTY
			&& bot->client->ps.weapon != WP_GRENADE_PINEAPPLE
			&& bot->client->ps.weapon != WP_GRENADE_LAUNCHER
			&& !g_allWeaponsDamageTanks.integer)
		{// If we can't hurt the tank, don't try to attack, run away!
			bot->enemy = NULL;
			bot->bot_coverspot_enemy = NULL;
			AIMOD_MOVEMENT_Move( bot, ucmd );
			return;
		}
#endif //__VEHICLES__
	}

	if (!bot_coverspot_enable.integer || num_cover_spots <= 0)
	{// Coverspots disabled! Normal attack code..
		if (bot->current_node && bot->longTermGoal)
		{
			bot->bot_fireteam_attack_in_route = qtrue;
			AIMOD_MOVEMENT_Move( bot, ucmd );
		}
		else if (number_of_nodes > 0)
		{
			//AIMOD_MOVEMENT_AttackMove( bot, ucmd );
			bot->bot_fireteam_attack_in_route = qtrue;
			AIMOD_MOVEMENT_Move( bot, ucmd );
		}
		else
		{
			//AIMOD_MOVEMENT_Attack( bot, ucmd );
			AIMOD_SP_MOVEMENT_CoverSpot_Attack( bot, ucmd, qfalse );
			bot->node_timeout = level.time + (TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		}

		return;
	}

	if (bot->current_node > 0)
		if (AIMOD_SP_MOVEMENT_CoverSpot_BadNodeType(bot->current_node))
			bad_node_type = qtrue;

	if (bot->enemy && !bad_node_type)
	{// Check if we can use cover where we are right now, and skip pathfinding, etc...
		vec3_t up_org;

		VectorCopy(bot->r.currentOrigin, up_org);
		up_org[2]+=32;//CROUCH_VIEWHEIGHT; // Crouch height!

		if (!CoverspotEnemyVisible(up_org, bot->enemy, bot->s.number))
		{
			VectorCopy(bot->r.currentOrigin, up_org);
			up_org[2]+=DEFAULT_VIEWHEIGHT; // Standing height!

			if (CoverspotEnemyVisible(up_org, bot->enemy, bot->s.number))
				force_attack = qtrue;
		}

		if (!force_attack)
		{
			VectorCopy(bot->r.currentOrigin, up_org);
			up_org[2]+=24;//CROUCH_VIEWHEIGHT; // Prone height!

			if (!CoverspotEnemyVisible(up_org, bot->enemy, bot->s.number))
			{
				VectorCopy(bot->r.currentOrigin, up_org);
				up_org[2]+=DEFAULT_VIEWHEIGHT; // Standing height!

				if (CoverspotEnemyVisible(up_org, bot->enemy, bot->s.number))
				{
					force_attack = qtrue;
					useProne = qtrue;
				}
			}
		}
	}

	if (!force_attack
		&& bot->longTermGoal 
		&& !bad_node_type
		&& (nodes[bot->longTermGoal].type & NODE_COVER))
	{
		vec3_t up_org;

		VectorCopy(nodes[bot->longTermGoal].origin, up_org);
		up_org[2]+=32;//CROUCH_VIEWHEIGHT; // Crouch height!

		if (CoverspotEnemyVisible(up_org, bot->enemy, bot->s.number))
		{
			find_new_coverspot = qtrue;
		}
		else
		{
			VectorCopy(nodes[bot->longTermGoal].origin, up_org);
			up_org[2]+=DEFAULT_VIEWHEIGHT; // Standing height!

			if (!CoverspotEnemyVisible(up_org, bot->enemy, bot->s.number))
				find_new_coverspot = qtrue;
		}
	}

	if (force_attack && AIMOD_AI_Enemy_Is_Viable( bot, bot->enemy ))
	{
		if (bot_debug.integer)
			G_Printf("%s attacking %s from it's forced attack cover spot!\n", bot->client->pers.netname, bot->enemy->client->pers.netname);

		bot->bot_coverspot_enemy = bot->enemy;
		AIMOD_SP_MOVEMENT_CoverSpot_Attack( bot, ucmd, useProne );
		bot->node_timeout = level.time + (TravelTime( bot )*2);
		bot->bot_last_good_move_time = level.time;
		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
		return;
	}
	else
	{// Find a cover spot, or move to the one we have!
		if (!bot->current_node || !bot->longTermGoal 
			|| bot->current_node > number_of_nodes || bot->longTermGoal > number_of_nodes 
			|| !(nodes[bot->longTermGoal].type & NODE_COVER))
		{// Need a new path to a coverspot!
			int best_goal = AIMOD_SP_MOVEMENT_CoverSpot_FindCoverSpot(bot);
			int pathlist[MAX_PATHLIST_NODES];
			int	pathsize = 0;
			int	new_current_node = bot->current_node;
			int	new_longterm_goal = -1;

			if (!best_goal)
			{
				if (bot_debug.integer)
					G_Printf("%s failed to find a cover spot for enemy %s!\n", bot->client->pers.netname, bot->enemy->client->pers.netname);

				AIMOD_MOVEMENT_Move( bot, ucmd );
				return;
			}

			if (!bot->current_node 
				|| bot->current_node > number_of_nodes 
				|| !SP_NodeVisible(bot->r.currentOrigin, nodes[bot->current_node].origin, bot->s.number))
			{
				new_current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
			}

			if ( new_current_node < 0 )
			{
				AIMOD_MOVEMENT_Move( bot, ucmd );
				return;
			}

			memset( pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
			pathsize = 0;

			new_longterm_goal = best_goal;
			bot->last_node = bot->current_node - 1;
			pathsize = CreatePathAStar( bot, new_current_node, new_longterm_goal, pathlist );

			if (pathsize < 0)
			{
				AIMOD_MOVEMENT_Move( bot, ucmd );
				return;
			}

			bot->pathsize = pathsize;
			bot->goalentity = NULL;
			bot->current_node = new_current_node;
			bot->longTermGoal = new_longterm_goal;
			memcpy(bot->pathlist, pathlist, ((sizeof(int))*MAX_PATHLIST_NODES));
			bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
			bot->bot_coverspot_enemy = bot->enemy;
		}

		if (bot_debug.integer)
			G_Printf("%s is moving to his coverspot for enemy %s.\n", bot->client->pers.netname, bot->enemy->client->pers.netname);

		AIMOD_MOVEMENT_Move( bot, ucmd );
	}
}

#endif //__BOT__
