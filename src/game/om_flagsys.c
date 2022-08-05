/*
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
Unique1's FlagSys Code.
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
Spawn_Flag_Base
flag_AI_Think_Axis
flag_AI_Think_Allied
flag_AI_Think
OM_Flag_Spawn_Test
OM_Flag_Spawn
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
*/

#include "../game/q_shared.h"

#ifdef __ETE__

#include "g_local.h"

extern int OrgVisibleBox2 ( vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore );

//===========================================================================
// Routine      : Spawn_Flag_Base
// Description  : Spawn a base to sit the flagpole on. We might not need this! May be useful for moveable bases on tanks/etc???
/*void Spawn_Flag_Base ( vec3_t origin )
{// Blue Flag...
	gentity_t* ent = G_Spawn( "misc_control_point_base" );

	VectorCopy(origin, ent->s.origin);
	G_SetOrigin( ent, origin );

	ent->model = "models/map_objects/mp/flag_base.md3";
	ent->s.modelindex = G_ModelIndex( ent->model );
	ent->targetname = NULL;
	ent->classname = "control_point_base";
	ent->s.eType = ET_GENERAL;
	
	VectorSet( ent->r.mins, -16, -16, 0 );
	VectorSet( ent->r.maxs, 16, 16, 16 );

	ent->r.contents = CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_MONSTERCLIP;//CONTENTS_SOLID;

	trap_LinkEntity (ent);
}*/

//===========================================================================
// Routine      : flag_AI_Think_Allies
// Description  : Main think function for flags.
void flag_AI_Think_Allies( gentity_t *ent )
{
	vec3_t mins;
	vec3_t maxs;
	int touch[MAX_GENTITIES];
	int num = 0;
	int i = 0;

	VectorSet( mins, (float) 0-(ent->s.effect1Time*0.5), (float) 0-(ent->s.effect1Time*0.5), -100 );
	VectorSet( maxs, (float) ent->s.effect1Time*0.5, (float) ent->s.effect1Time*0.5, 300 );

	VectorAdd( mins, ent->s.origin, mins );
	VectorAdd( maxs, ent->s.origin, maxs );

	if (ent->nextthink > level.time)
		return;

	if (!ent->s.number)
		return;

	// Run the think... Look for captures...
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	if (ent->s.time2 < 0)
	{
		ent->s.time2 = 0;
	}

	for ( i=0 ; i<num ; i++ ) 
	{
		gentity_t *hit = &g_entities[touch[i]];
		gclient_t *client;
		int currentNum;

		if (!hit)
			continue;

		if (hit->client)
			client = hit->client;
#ifdef __NPC__
		else if (hit->s.eType == ET_NPC)
			client = hit->NPC_client;
#endif //__NPC__

		if (
#ifdef __NPC__
			hit->s.eType != ET_NPC &&
#endif //__NPC__
			!hit->client)
			continue;

		if (hit->s.eType != ET_PLAYER
#ifdef __NPC__
			&& hit->s.eType != ET_NPC
#endif //__NPC__
			)
			continue;

		if (hit->s.number > MAX_CLIENTS
#ifdef __NPC__
			&& hit->s.eType != ET_NPC
#endif //__NPC__
			)
			continue;

		if (
#ifdef __NPC__
			hit->s.eType != ET_NPC &&
#endif //__NPC__
			!hit->client->sess.sessionTeam)
			continue;

		if (
#ifdef __NPC__
			hit->s.eType != ET_NPC &&
#endif //__NPC__
			!hit->client->pers.connected)
			continue;

		if (hit->health <= 0)
			continue;

		if (hit->client && VectorDistance(hit->client->ps.origin, ent->s.origin) > (float) ent->s.effect1Time*0.5)
			continue;

#ifdef __NPC__
		if (hit->s.eType == ET_NPC && VectorDistance(hit->r.currentOrigin, ent->s.origin) > (float) ent->s.effect1Time*0.5)
			continue;
#endif //__NPC__

		currentNum = client->ps.stats[STAT_CAPTURE_ENTITYNUM];

#ifdef __NPC__
		if ((hit->s.eType == ET_PLAYER || hit->s.eType == ET_NPC) && client->sess.sessionTeam == TEAM_ALLIES )
#else //!__NPC__
		if (hit->s.eType == ET_PLAYER && client->sess.sessionTeam == TEAM_ALLIES )
#endif //__NPC__
		{
			ent->s.time2--;
			client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
			client->ps.stats[STAT_FLAG_CAPTURE] = 1;
		}
#ifdef __NPC__
		else if ((hit->s.eType == ET_PLAYER || hit->s.eType == ET_NPC) && client->sess.sessionTeam == TEAM_AXIS )
#else //!__NPC__
		else if (hit->s.eType == ET_PLAYER && client->sess.sessionTeam == TEAM_AXIS )
#endif //__NPC__
		{
			ent->s.time2++;
			client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
			client->ps.stats[STAT_FLAG_CAPTURE] = 1;
		}
		else if (client->ps.stats[STAT_FLAG_CAPTURE]
			&& currentNum
			&& currentNum != ent->s.number
			&& currentNum > 0)
		{// In case we're capturing another flag somewhere...

		}
		else
		{
			client->ps.stats[STAT_CAPTURE_ENTITYNUM] = 0;
			client->ps.stats[STAT_FLAG_CAPTURE] = 0;
		}
	}

	if (ent->s.time2 >= 100)
	{// We're being captured! Change the flag color!
		ent->s.time2 = 0;
		ent->s.modelindex = TEAM_FREE;
		G_Script_ScriptEvent( ent, "trigger", "free_capture" );
		trap_SendServerCommand( -1, va("snd %i %s", (int)TEAM_ALLIES, "sound/vo/general/allies/hq_objlost.wav" ));
	}

	if (ent->s.time2 < 0)
	{
		ent->s.time2 = 0;
	}

	// Set next think...
	ent->nextthink = level.time + 250;
}

//===========================================================================
// Routine      : flag_AI_Think_Axis
// Description  : Main think function for flags.
void flag_AI_Think_Axis( gentity_t *ent )
{
	vec3_t mins;
	vec3_t maxs;
	int touch[MAX_GENTITIES];
	int num = 0;
	int i = 0;

	VectorSet( mins, (float) 0-(ent->s.effect1Time*0.5), (float) 0-(ent->s.effect1Time*0.5), -100 );
	VectorSet( maxs, (float) ent->s.effect1Time*0.5, (float) ent->s.effect1Time*0.5, 300 );

	VectorAdd( mins, ent->s.origin, mins );
	VectorAdd( maxs, ent->s.origin, maxs );

	if (ent->nextthink > level.time)
		return;

	if (!ent->s.number)
		return;

	// Run the think... Look for captures...
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	if (ent->s.time2 < 0)
	{
		ent->s.time2 = 0;
	}

	for ( i=0 ; i<num ; i++ ) 
	{
		gentity_t *hit = &g_entities[touch[i]];
		gclient_t *client;
		int currentNum;

		if (!hit)
			continue;

		if (hit->client)
			client = hit->client;
#ifdef __NPC__
		else if (hit->s.eType == ET_NPC)
			client = hit->NPC_client;
#endif //__NPC__

		if (
#ifdef __NPC__
			hit->s.eType != ET_NPC &&
#endif //__NPC__
			!hit->client)
			continue;

		if (hit->s.eType != ET_PLAYER
#ifdef __NPC__
			&& hit->s.eType != ET_NPC
#endif //__NPC__
			)
			continue;

		if (hit->s.number > MAX_CLIENTS
#ifdef __NPC__
			&& hit->s.eType != ET_NPC
#endif //__NPC__
			)
			continue;

		if (
#ifdef __NPC__
			hit->s.eType != ET_NPC &&
#endif //__NPC__
			!hit->client->sess.sessionTeam)
			continue;

		if (
#ifdef __NPC__
			hit->s.eType != ET_NPC &&
#endif //__NPC__
			!hit->client->pers.connected)
			continue;

		if (hit->health <= 0)
			continue;

		if (hit->client && VectorDistance(hit->client->ps.origin, ent->s.origin) > (float) ent->s.effect1Time*0.5)
			continue;

#ifdef __NPC__
		if (hit->s.eType == ET_NPC && VectorDistance(hit->r.currentOrigin, ent->s.origin) > (float) ent->s.effect1Time*0.5)
			continue;
#endif //__NPC__

		currentNum = client->ps.stats[STAT_CAPTURE_ENTITYNUM];

#ifdef __NPC__
		if ((hit->s.eType == ET_PLAYER || hit->s.eType == ET_NPC) && client->sess.sessionTeam == TEAM_AXIS )
#else //__NPC__
		if (hit->s.eType == ET_PLAYER && client->sess.sessionTeam == TEAM_AXIS )
#endif //__NPC__
		{
			ent->s.time2--;
			client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
			client->ps.stats[STAT_FLAG_CAPTURE] = 1;
		}
#ifdef __NPC__
		else if ((hit->s.eType == ET_PLAYER || hit->s.eType == ET_NPC) && client->sess.sessionTeam == TEAM_ALLIES )
#else //__NPC__
		else if (hit->s.eType == ET_PLAYER && client->sess.sessionTeam == TEAM_ALLIES )
#endif //__NPC__
		{
			ent->s.time2++;
			client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
			client->ps.stats[STAT_FLAG_CAPTURE] = 1;
		}
		else if (client->ps.stats[STAT_FLAG_CAPTURE]
			&& currentNum
			&& currentNum != ent->s.number
			&& currentNum > 0)
		{// In case we're capturing another flag somewhere...

		}
		else
		{
			client->ps.stats[STAT_CAPTURE_ENTITYNUM] = 0;
			client->ps.stats[STAT_FLAG_CAPTURE] = 0;
		}
	}

	if (ent->s.time2 >= 100)
	{// We're being captured! Change the flag color!
		ent->s.time2 = 0;
		ent->s.modelindex = TEAM_FREE;
		G_Script_ScriptEvent( ent, "trigger", "free_capture" );
		trap_SendServerCommand( -1, va("snd %i %s", (int)TEAM_AXIS, "sound/vo/general/axis/hq_objlost.wav" ));
	}

	if (ent->s.time2 < 0)
	{
		ent->s.time2 = 0;
	}

	// Set next think...
	ent->nextthink = level.time + 250;
}

//===========================================================================
// Routine      : flag_AI_Think_Neutral
// Description  : Main think function for flags.
void flag_AI_Think_Neutral( gentity_t *ent )
{
	vec3_t mins;
	vec3_t maxs;
	int touch[MAX_GENTITIES];
	int num = 0;
	int i = 0;

	VectorSet( mins, (float) 0-(ent->s.effect1Time*0.5), (float) 0-(ent->s.effect1Time*0.5), -100 );
	VectorSet( maxs, (float) ent->s.effect1Time*0.5, (float) ent->s.effect1Time*0.5, 300 );

	VectorAdd( mins, ent->s.origin, mins );
	VectorAdd( maxs, ent->s.origin, maxs );

	if (ent->nextthink > level.time)
		return;

	if (!ent->s.number)
		return;

	// Run the think... Look for captures...
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	if (ent->s.time2 < 0)
	{
		ent->s.time2 = 0;
	}

	for ( i=0 ; i<num ; i++ ) 
	{
		gentity_t *hit = &g_entities[touch[i]];
		gclient_t *client;
		int currentNum;

		if (!hit)
			continue;

		if (hit->client)
			client = hit->client;
#ifdef __NPC__
		else if (hit->s.eType == ET_NPC)
			client = hit->NPC_client;
#endif //__NPC__

		if (
#ifdef __NPC__
			hit->s.eType != ET_NPC &&
#endif //__NPC__
			!hit->client)
			continue;

		if (hit->s.eType != ET_PLAYER
#ifdef __NPC__
			&& hit->s.eType != ET_NPC
#endif //__NPC__
			)
			continue;

		if (hit->s.number > MAX_CLIENTS
#ifdef __NPC__
			&& hit->s.eType != ET_NPC
#endif //__NPC__
			)
			continue;

		if (
#ifdef __NPC__
			hit->s.eType != ET_NPC &&
#endif //__NPC__
			!hit->client->sess.sessionTeam)
			continue;

		if (
#ifdef __NPC__
			hit->s.eType != ET_NPC &&
#endif //__NPC__
			!hit->client->pers.connected)
			continue;

		if (hit->health <= 0)
			continue;

		if (hit->client && VectorDistance(hit->client->ps.origin, ent->s.origin) > (float) ent->s.effect1Time*0.5)
			continue;

#ifdef __NPC__
		if (hit->s.eType == ET_NPC && VectorDistance(hit->r.currentOrigin, ent->s.origin) > (float) ent->s.effect1Time*0.5)
			continue;
#endif //__NPC__

		currentNum = client->ps.stats[STAT_CAPTURE_ENTITYNUM];

#ifdef __NPC__
		if ((hit->s.eType == ET_PLAYER || hit->s.eType == ET_NPC) && client->sess.sessionTeam == TEAM_AXIS )
#else //!__NPC__
		if (hit->s.eType == ET_PLAYER && client->sess.sessionTeam == TEAM_AXIS )
#endif //__NPC__
		{
			if (ent->flag_capture_team == TEAM_ALLIES)
			{
				ent->s.time2--;
				client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
				client->ps.stats[STAT_FLAG_CAPTURE] = 1;
			}
			if (ent->flag_capture_team == TEAM_ALLIES && ent->s.time2 < 0)
			{
				ent->flag_capture_team = TEAM_AXIS;
				ent->s.time2 = 1;
				client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
				client->ps.stats[STAT_FLAG_CAPTURE] = 1;
			}
			else if (ent->flag_capture_team == TEAM_AXIS)
			{
				ent->flag_capture_team = TEAM_AXIS;
				ent->s.time2++;
				client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
				client->ps.stats[STAT_FLAG_CAPTURE] = 1;
			}
			else
			{
				ent->flag_capture_team = TEAM_AXIS;
				ent->s.time2++;
				client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
				client->ps.stats[STAT_FLAG_CAPTURE] = 1;
			}
		}
#ifdef __NPC__
		else if ((hit->s.eType == ET_PLAYER || hit->s.eType == ET_NPC) && client->sess.sessionTeam == TEAM_ALLIES )
#else //!__NPC__
		else if (hit->s.eType == ET_PLAYER && client->sess.sessionTeam == TEAM_ALLIES )
#endif //__NPC__
		{
			if (ent->flag_capture_team == TEAM_AXIS)
			{
				ent->s.time2--;
				client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
				client->ps.stats[STAT_FLAG_CAPTURE] = 1;
			}
			if (ent->flag_capture_team == TEAM_AXIS && ent->s.time2 < 0)
			{
				ent->flag_capture_team = TEAM_ALLIES;
				ent->s.time2 = 1;
				client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
				client->ps.stats[STAT_FLAG_CAPTURE] = 1;
			}
			else if (ent->flag_capture_team == TEAM_ALLIES)
			{
				ent->flag_capture_team = TEAM_ALLIES;
				ent->s.time2++;
				client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
				client->ps.stats[STAT_FLAG_CAPTURE] = 1;
			}
			else
			{
				ent->flag_capture_team = TEAM_ALLIES;
				ent->s.time2++;
				client->ps.stats[STAT_CAPTURE_ENTITYNUM] = ent->s.number;
				client->ps.stats[STAT_FLAG_CAPTURE] = 1;
			}
		}
		else if (client->ps.stats[STAT_FLAG_CAPTURE]
			&& currentNum
			&& currentNum != ent->s.number
			&& currentNum > 0)
		{// In case we're capturing another flag somewhere...

		}
		else
		{
			client->ps.stats[STAT_CAPTURE_ENTITYNUM] = 0;
			client->ps.stats[STAT_FLAG_CAPTURE] = 0;
		}
	}

	if (ent->s.time2 >= 100)
	{
		if (ent->flag_capture_team == TEAM_ALLIES)
		{
			ent->s.modelindex = TEAM_ALLIES;
			G_Script_ScriptEvent( ent, "trigger", "allied_capture" );
			ent->s.time2 = 0;
			trap_SendServerCommand( -1, va("snd %i %s", (int)TEAM_ALLIES, "sound/vo/general/allies/hq_objcap.wav" ));
			//G_Printf("Allies capture flag!\n");
		}
		else
		{
			ent->s.modelindex = TEAM_AXIS;
			G_Script_ScriptEvent( ent, "trigger", "axis_capture" );
			ent->s.time2 = 0;
			trap_SendServerCommand( -1, va("snd %i %s", (int)TEAM_AXIS, "sound/vo/general/axis/hq_objcap.wav" ));
			//G_Printf("Axis capture flag!\n");
		}
	}

	if (ent->s.time2 < 0)
	{
		ent->s.time2 = 0;
	}

	// Set next think...
	ent->nextthink = level.time + 250;
}

extern void ControlPoint_Register(gentity_t *self);
extern void ControlPoint_Update( gentity_t *self, gentity_t *other, gentity_t *activator );

//===========================================================================
// Routine      : flag_AI_Think
// Description  : Main think function for flags.
void flag_AI_Think( gentity_t *ent )
{// Pick appropriate flag type for thinking... Axis/Allied.
	if (!ent->message)
	{// Initialize the control point command map/limbo spawnpoint info!
		ent->message = Q_malloc(sizeof(va("Neutral control point")));

		if (ent->s.modelindex == TEAM_ALLIES)
		{
			Q_strncpyz( ent->message, va("Allied control point"), 32 );
			ControlPoint_Register(ent);
			flag_AI_Think_Allies(ent);
			ent->aiTeam = ent->s.modelindex;
		}
		else if (ent->s.modelindex == TEAM_AXIS)
		{
			Q_strncpyz( ent->message, va("Axis control point"), 32 );
			ControlPoint_Register(ent);
			flag_AI_Think_Axis(ent);
			ent->aiTeam = ent->s.modelindex;
		}
		else
		{
			Q_strncpyz( ent->message, va("Neutral control point"), 32 );
			ControlPoint_Register(ent);
			flag_AI_Think_Neutral(ent);
			ent->aiTeam = ent->s.modelindex;
		}

		return;
	}

	if (ent->s.modelindex == TEAM_ALLIES)
	{
		if (ent->aiTeam != ent->s.modelindex)
		{
			Q_strncpyz( ent->message, va("Allied control point"), 32 );
			ControlPoint_Update( ent, NULL, NULL );
			ent->aiTeam = ent->s.modelindex;
		}

		flag_AI_Think_Allies(ent);
	}
	else if (ent->s.modelindex == TEAM_AXIS)
	{
		if (ent->aiTeam != ent->s.modelindex)
		{
			Q_strncpyz( ent->message, va("Axis control point"), 32 );
			ControlPoint_Update( ent, NULL, NULL );
			ent->aiTeam = ent->s.modelindex;
		}

		flag_AI_Think_Axis(ent);
	}
	else
	{
		if (ent->aiTeam != ent->s.modelindex)
		{
			Q_strncpyz( ent->message, va("Neutral control point"), 32 );
			ControlPoint_Update( ent, NULL, NULL );
			ent->aiTeam = ent->s.modelindex;
		}

		flag_AI_Think_Neutral(ent);
	}
}

qboolean spots_filled[1024];

void AddFlag_Spawn ( int flagnum, vec3_t origin, vec3_t angles )
{// Add a new spawn to the list for this flag...
	if (flag_list[flagnum].num_spawnpoints < 128/*64*/)
	{// Create a new spawn point.
//		G_Printf("A new spawn point has been extrapolated. ^1(^5#^7%i^1)^5\n", flag_list[flagnum].num_spawnpoints);

		VectorCopy( origin, flag_list[flagnum].spawnpoints[flag_list[flagnum].num_spawnpoints] ); // Copy this spawnpoint's location to the end of the list.
		VectorCopy( angles, flag_list[flagnum].spawnangles[flag_list[flagnum].num_spawnpoints] ); // Copy this spawnpoint's location to the end of the list.
		flag_list[flagnum].num_spawnpoints++;
	}
	else
	{
		if (!spots_filled[flagnum])
		{// Finished making spawns for this flag...
			//G_Printf("Calculated 64 spawns for flag #%i.\n", flagnum);
			spots_filled[flagnum] = qtrue;
		}
	}
}

qboolean TooCloseToOtherSpawnpoint ( int flagnum, vec3_t origin )
{
	int i = 0;

	for (i = 0;i < flag_list[flagnum].num_spawnpoints;i++)
	{
		if (VectorDistance(flag_list[flagnum].spawnpoints[i], origin) < 64)
			return qtrue;
	}

	return qfalse;
}

//extern int OrgVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore);
extern qboolean CheckBelowOK(vec3_t origin);
extern qboolean CheckEntitiesInSpot(vec3_t point);

void PreCalculate_Flag_Spawnpoints( int flagnum, vec3_t angles, vec3_t origin )
{
	vec3_t fwd, point;
	int tries = 0, tries2 = 0;
	//qboolean visible = qfalse;
	qboolean alt = qfalse;

	origin[2]+=48/*64*//*32*/;
	VectorCopy(origin, point);
		
	AngleVectors( angles, fwd, NULL, NULL );

	while (1)//visible == qfalse)
	{// In case we need to try a second spawnpoint.
		vec3_t playerMins, playerMaxs;

		playerMins[0] = -15;
		playerMins[1] = -15;
		playerMins[2] = -1;
		playerMaxs[0] = 15;
		playerMaxs[1] = 15;
		playerMaxs[2] = 64;//96;//1;

		tries = 0;
		tries2 = 0;

		while (tries < 32/*16*/)
		{
			tries++;
			tries2 = 0;

			while (tries2 < 128)
			{
				int num_tries; // For secondary spawns. (Behind point).

				tries2++;

				num_tries = tries2;

				if (tries2 <= 16)
				{
				}
				else if (tries2 <= 32)
				{
					num_tries-=16;
				}
				else if (tries2 <= 48)
				{
					num_tries-=32;
				}
				else
				{
					num_tries-=48;
				}
				
				VectorCopy(origin, point);

				if (alt)
				{
					if (tries2 <= 8)
					{
						point[0] -= 1+(tries*64);
						point[1] -= 1+(num_tries*64);
					}
					else if (tries2 <= 16)
					{
						point[0] -= 1+(tries*64);
						point[1] -= 1-(num_tries*64);
					}
					else if (tries2 <= 24)
					{
						point[0] -= 1-(tries*64);
						point[1] -= 1+(num_tries*64);
					}
					else
					{
						point[0] -= 1-(tries*64);
						point[1] -= 1-(num_tries*64);
					}
				}
				else
				{
					if (tries2 <= 8)
					{
						point[0] += 1+(tries*64);
						point[1] += 1+(num_tries*64);
					}
					else if (tries2 <= 16)
					{
						point[0] += 1+(tries*64);
						point[1] += 1-(num_tries*64);
					}
					else if (tries2 <= 24)
					{
						point[0] += 1-(tries*64);
						point[1] += 1+(num_tries*64);
					}
					else
					{
						point[0] += 1-(tries*64);
						point[1] += 1-(num_tries*64);
					}
				}

				if (OrgVisibleBox(origin, playerMins, playerMaxs, point, flag_list[flagnum].flagentity->s.number)
					&& CheckBelowOK(point)
					&& !CheckEntitiesInSpot(point) 
					//&& VectorDistance(point, origin) > 128
					&& !TooCloseToOtherSpawnpoint(flagnum, point))
				{
					AddFlag_Spawn(flagnum, point, angles);

					if (flag_list[flagnum].num_spawnpoints >= 128/*64*/)
					{
						//G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].num_spawnpoints, flagnum);
						return;
					}
				}
			}
		}

		if (alt)
			break;
		else
			alt = qtrue;
	}
}

qboolean have_not_loaded_waypoints = qtrue;

void CALL_PreCalculate_Flag_Spawnpoints( gentity_t *ent, int flagnum, vec3_t angles, vec3_t origin )
{
	if (number_of_nodes <= 0 && have_not_loaded_waypoints)
	{
		AIMOD_NODES_LoadNodes();
		have_not_loaded_waypoints = qfalse;
	}

	PreCalculate_Flag_Spawnpoints( flagnum, angles, origin );

	if (flag_list[flagnum].num_spawnpoints < 128/*64*/)
	{// Try to create some more by using a far away spawnpoint as a start point...
		int i;
		float	best_distance = 0;
		vec3_t	best_point;
		float	second_best_distance = 0;
		vec3_t	second_best_point;
		float	third_best_distance = 0;
		vec3_t	third_best_point;

		// Init vectors.
		VectorSet(best_point, 0, 0, 0);
		VectorSet(second_best_point, 0, 0, 0);
		VectorSet(third_best_point, 0, 0, 0);

		for (i = 0; i < flag_list[flagnum].num_spawnpoints; i++)
		{
			if (VectorDistance(flag_list[flagnum].spawnpoints[i], ent->s.origin) <= best_distance)
				continue;

			third_best_distance = second_best_distance;
			VectorCopy(second_best_point, third_best_point);

			second_best_distance = best_distance;
			VectorCopy(best_point, second_best_point);

			VectorCopy(flag_list[flagnum].spawnpoints[i], best_point);
			best_distance = VectorDistance(flag_list[flagnum].spawnpoints[i], ent->s.origin);
		}

		if (best_distance > 0)
		{
			if (second_best_distance > 0)
			{
				if (third_best_distance > 0)
				{// Found 3...
					int choice = Q_TrueRand(0,2);

					if ( choice == 1)
					{
						PreCalculate_Flag_Spawnpoints( flagnum, ent->s.angles, best_point );
					}
					else if ( choice == 2)
					{
						PreCalculate_Flag_Spawnpoints( flagnum, ent->s.angles, second_best_point );
					}
					else
					{
						PreCalculate_Flag_Spawnpoints( flagnum, ent->s.angles, third_best_point );
					}
				}
				else
				{// Found 2...
					int choice = Q_TrueRand(0,1);

					if ( choice == 1)
					{
						PreCalculate_Flag_Spawnpoints( flagnum, ent->s.angles, best_point );
					}
					else
					{
						PreCalculate_Flag_Spawnpoints( flagnum, ent->s.angles, second_best_point );
					}
				}
			}
			else
			{// Only a primary.
				PreCalculate_Flag_Spawnpoints( flagnum, ent->s.angles, best_point );
			}
		}
	}

	if (flag_list[flagnum].num_spawnpoints < 128/*64*/ && number_of_nodes > 0)
	{// Use map waypoints as spawns!
		int		best_list[128];
		float	best_list_distances[128];
		int		num_best = 0;
		int		i = 0;

		for (i = 0; i < number_of_nodes; i++)
		{
			float dist = VectorDistance(nodes[i].origin, origin);

			if (dist < 1024)
			{
				vec3_t org;

				VectorCopy(nodes[i].origin, org);
				org[2] += 16;

				if (CheckBelowOK(org)
					&& !CheckEntitiesInSpot(org) 
					&& !TooCloseToOtherSpawnpoint(flagnum, org))
				{
					if (num_best + flag_list[flagnum].num_spawnpoints < 128)
					{// Continue adding to the list...
						best_list[num_best] = i;
						best_list_distances[num_best] = dist;
						num_best++;
					}
					else
					{// OK, list is maxed out, check for one to replace! (Sorting)
						int		j = 0;
						float	furthest_dist = 0.0f;
						int		furthest_wp = -1;

						// Find the furthest one to replace...
						for (j = 0; j < num_best; j++)
						{
							if (best_list_distances[j] > furthest_dist)
							{
								furthest_dist = best_list_distances[j];
								furthest_wp = j;
							}
						}

						if (furthest_dist > dist)
						{// Replace it!
							best_list[furthest_wp] = i;
							best_list_distances[furthest_wp] = dist;
						}
					}
				}
			}
		}

		for (i = 0; i < num_best; i++)
		{
			if (flag_list[flagnum].num_spawnpoints < 128)
			{
				vec3_t org;

				VectorCopy(nodes[best_list[i]].origin, org);
				org[2] += 16;

				AddFlag_Spawn(flagnum, org, angles);
			}
			else
			{
				break;
			}
		}
	}

	G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].num_spawnpoints, flagnum);
}

int num_flags = 0; // Automatically number the flags if not specified by map makers...

//===========================================================================
// Routine      : OM_Flag_Spawn_Test
// Description  : Spawn a flag. For testing...
void OM_Flag_Spawn_Test ( vec3_t origin )
{// Spawn a Red Flag at origin... Use this for testing...
	gentity_t* ent = G_Spawn();
	int flagnum = 0;

	if (flagnum < 0)
	{// We dont have a flag number.. Generate them for this map...
		flagnum = num_flags;
		num_flags++;
	}

	ent->classname = "misc_control_point";

	VectorCopy(origin, ent->s.origin);

	// Spawn a little away from player testing the flag...
	origin[0]+=32;
	origin[1]+=32;

	// Sink a little...
	origin[2]-=32;	
	
	G_SetOrigin( ent, origin );

//	ent->spawnflags = ALLIED_OBJECTIVE;
	ent->model = "models/multiplayer/flagpole/flagpole.md3";
    
	ent->s.modelindex = TEAM_AXIS;
	ent->s.modelindex2 = G_ModelIndex (ent->model);

	ent->targetname = NULL;
	ent->classname = "misc_control_point";
	ent->s.eType = ET_FLAG;
	ent->think = flag_AI_Think;
	ent->s.time2 = 0;

	ent->s.weapon = 1; // Marks an animated flag model...

	VectorSet( ent->r.mins, -8, -8, 0 );
	VectorSet( ent->r.maxs, 8, 8, 96 );

	ent->s.effect1Time = 1000; // Set capture radius...
	
	ent->r.contents = CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_MONSTERCLIP;

	trap_LinkEntity (ent);

	G_Printf("Attack flag spawned at origin: { %f %f %f }\n", origin[0], origin[1], origin[2]);
	ent->nextthink = level.time;

//	Spawn_Flag_Base(origin);

	flag_list[flagnum].flagentity = ent;
	flag_list[flagnum].num_spawnpoints = 0;

	CALL_PreCalculate_Flag_Spawnpoints( ent, flagnum, ent->s.angles, ent->s.origin );
}

//===========================================================================
// Routine      : OM_Flag_Spawn
// Description  : Spawn a flag.
void OM_Flag_Spawn ( gentity_t* ent )
{// OM Flag... Used by map entitiy to call the flag's spawn...
	char *model;
	int radius = 0;
	int flagnum = 0;
	//int spawnflags = 0;

	G_SpawnInt("partofstage", "-1", &flagnum); // Map's flag number.. For linking things to this flag based on team...

	if (flagnum < 0)
	{// We dont have a flag number.. Generate them for this map...
		flagnum = num_flags;
		num_flags++;
	}

	G_SpawnString("model", "", &model); // Custom model..
	G_SpawnInt("radius", "", &radius); // Point's capture radius..
	G_SpawnInt( "spawnflags", "", &ent->spawnflags );
	G_SpawnString( "targetname", "", &ent->targetname );
	G_SpawnString( "scriptName", "", &ent->scriptName );

	//G_Printf("%i\n", ent->spawnflags);

	//ent->s.origin[2]-=32;
	G_SetOrigin( ent, ent->s.origin );

	if(!model || model == "" || !strcmp(model, "" ))
	{// ent->model can be specified...
		//G_Printf("Flag model set to default!\n");
		ent->model = "models/multiplayer/flagpole/flagpole.md3";
		ent->s.weapon = 1; // Marks an animated flag model...
		
		if (!radius)
		{// Default radius...
			ent->s.effect1Time = 1000; // Set capture radius...
		}
		else
		{
			ent->s.effect1Time = radius; // Set capture radius...
		}
	}
	else
	{
		ent->model = model;

		if (!radius)
		{// Default radius...
			ent->s.effect1Time = 1000; // Set capture radius...
		}
		else
		{
			ent->s.effect1Time = radius; // Set capture radius...
		}
	}

	if (ent->count)
	{// Marks an animated flag model...
		ent->s.weapon = 1; // Marks an animated flag model...
	}

	if (ent->spawnflags == AXIS_OBJECTIVE)
	{
		ent->s.modelindex = TEAM_ALLIES;
		ent->s.time2 = 0;
		G_Printf("^1*** ^5Flag ^7%i^5 at %f %f %f is owned by ^4TEAM_ALLIES^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}
	else if (ent->spawnflags == ALLIED_OBJECTIVE)
	{
		ent->s.modelindex = TEAM_AXIS;
		ent->s.time2 = 0;
		G_Printf("^1*** ^5Flag ^7%i^5 at %f %f %f is owned by ^1TEAM_AXIS^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}
	else
	{
//		ent->spawnflags = ALLIED_OBJECTIVE;
		ent->s.modelindex = TEAM_FREE;
		ent->s.time2 = 0; // Half way between taken/not taken...
		G_Printf("^1*** ^5Flag ^7%i^5 at %f %f %f is owned by ^7TEAM_NEUTRAL^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}

	ent->s.modelindex2 = G_ModelIndex (ent->model);

	//G_Printf("Model index set to: %i (%s).\n", ent->s.modelindex2, ent->model);

	ent->targetname = NULL;
	ent->classname = "misc_control_point";
	ent->s.eType = ET_FLAG;
	ent->think = flag_AI_Think;
	
	VectorSet( ent->r.mins, -8, -8, 0 );
	VectorSet( ent->r.maxs, 8, 8, 96 );

	ent->r.contents = CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_MONSTERCLIP;

	trap_LinkEntity (ent);

	ent->nextthink = level.time;

//	Spawn_Flag_Base(ent->s.origin);

	flag_list[flagnum].flagentity = ent;
	flag_list[flagnum].num_spawnpoints = 0;

	CALL_PreCalculate_Flag_Spawnpoints( ent, flagnum, ent->s.angles, ent->s.origin );
}

//===========================================================================
// Routine      : OM_Flag_Spawn_Loaded
// Description  : Spawn a flag from a supremacy script file.
void OM_Flag_Spawn_Loaded ( vec3_t origin, int team )
{// OM Flag... Used by map entitiy to call the flag's spawn...
//	char *model = "";
//	int radius = 0;
	int flagnum = 0;
	gentity_t* ent = G_Spawn();

	VectorCopy(origin, ent->s.origin);

	flagnum = num_flags;
	num_flags++;

	G_SetOrigin( ent, origin );

	ent->model = "models/multiplayer/flagpole/flagpole.md3";
	ent->s.weapon = 1; // Marks an animated flag model...
		
	ent->s.effect1Time = 1000; // Set capture radius...

	if (ent->count)
	{// Marks an animated flag model...
		ent->s.weapon = 1; // Marks an animated flag model...
	}

	if (team == TEAM_ALLIES)
	{
		ent->s.modelindex = TEAM_ALLIES;
		ent->s.time2 = 0;
		G_Printf("^1*** ^5Flag ^7%i^5 at %f %f %f is owned by ^4TEAM_ALLIES^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}
	else if (team == TEAM_AXIS)
	{
		ent->s.modelindex = TEAM_AXIS;
		ent->s.time2 = 0;
		G_Printf("^1*** ^5Flag ^7%i^5 at %f %f %f is owned by ^1TEAM_AXIS^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}
	else
	{
//		ent->spawnflags = ALLIED_OBJECTIVE;
		ent->s.modelindex = TEAM_FREE;
		ent->s.time2 = 0; // Half way between taken/not taken...
		G_Printf("^1*** ^5Flag ^7%i^5 at %f %f %f is owned by ^7TEAM_NEUTRAL^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}

	ent->s.modelindex2 = G_ModelIndex (ent->model);

	//G_Printf("Model index set to: %i (%s).\n", ent->s.modelindex2, ent->model);

	ent->targetname = NULL;
	ent->classname = "misc_control_point";
	ent->s.eType = ET_FLAG;
	ent->think = flag_AI_Think;
	
	VectorSet( ent->r.mins, -8, -8, 0 );
	VectorSet( ent->r.maxs, 8, 8, 96 );

	ent->r.contents = CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_MONSTERCLIP;

	trap_LinkEntity (ent);

	ent->nextthink = level.time;

//	Spawn_Flag_Base(ent->s.origin);

	flag_list[flagnum].flagentity = ent;
	flag_list[flagnum].num_spawnpoints = 0;

	CALL_PreCalculate_Flag_Spawnpoints( ent, flagnum, ent->s.angles, ent->s.origin );
}

qboolean red_associated_spots_filled[256];
qboolean blue_associated_spots_filled[256];

void AddFlag_Associated_Spawn ( int flagnum, int teamnum, vec3_t origin, vec3_t angles )
{// Add a new spawn to the list for this flag's external spawn area...
	if (teamnum == TEAM_AXIS)
	{
		if (flag_list[flagnum].associated_red_spawnpoints_number < 256)
		{// Create a new spawn point.
			VectorCopy( origin, flag_list[flagnum].associated_red_spawnpoints[flag_list[flagnum].associated_red_spawnpoints_number] ); // Copy this spawnpoint's location to the end of the list.
			VectorCopy( angles, flag_list[flagnum].associated_red_spawnangles[flag_list[flagnum].associated_red_spawnpoints_number] ); // Copy this spawnpoint's location to the end of the list.
			flag_list[flagnum].associated_red_spawnpoints_number++;
		}
		else
		{
			if (!red_associated_spots_filled[flagnum])
			{// Finished making spawns for this flag...
				G_Printf("Calculated 256 associated spawns for flag #%i.\n", flagnum);
				red_associated_spots_filled[flagnum] = qtrue;
			}
		}
	}
	else
	{
		if (flag_list[flagnum].associated_blue_spawnpoints_number < 256)
		{// Create a new spawn point.
			VectorCopy( origin, flag_list[flagnum].associated_blue_spawnpoints[flag_list[flagnum].associated_blue_spawnpoints_number] ); // Copy this spawnpoint's location to the end of the list.
			VectorCopy( angles, flag_list[flagnum].associated_blue_spawnangles[flag_list[flagnum].associated_blue_spawnpoints_number] ); // Copy this spawnpoint's location to the end of the list.
			flag_list[flagnum].associated_blue_spawnpoints_number++;
		}
		else
		{
			if (!blue_associated_spots_filled[flagnum])
			{// Finished making spawns for this flag...
				G_Printf("Calculated 256 associated spawns for flag #%i.\n", flagnum);
				blue_associated_spots_filled[flagnum] = qtrue;
			}
		}
	}
}

qboolean TooCloseToOtherAssociatedSpawnpoint ( int flagnum, int teamnum, vec3_t origin )
{
	int i = 0;

	if (teamnum == TEAM_AXIS)
	{
		for (i = 0;i < flag_list[flagnum].associated_red_spawnpoints_number;i++)
		{
			if (VectorDistance(flag_list[flagnum].associated_red_spawnpoints[i], origin) < 64)
				return qtrue;
		}
	}
	else
	{
		for (i = 0;i < flag_list[flagnum].associated_blue_spawnpoints_number;i++)
		{
			if (VectorDistance(flag_list[flagnum].associated_blue_spawnpoints[i], origin) < 64)
				return qtrue;
		}
	}

	return qfalse;
}

void PreCalculate_Area_Spawnpoints( int flagnum, int teamnum, vec3_t angles, vec3_t orig_origin )
{
	vec3_t fwd, point, origin;
	int tries = 0, tries2 = 0;
	qboolean alt = qfalse;

	VectorCopy(orig_origin, origin);
	origin[2]+=256;//32;
	VectorCopy(origin, point);
		
	AngleVectors( angles, fwd, NULL, NULL );

	while (1)//visible == qfalse)
	{// In case we need to try a second spawnpoint.
		vec3_t playerMins, playerMaxs;

		playerMins[0] = -15;
		playerMins[1] = -15;
		playerMins[2] = -1;
		playerMaxs[0] = 15;
		playerMaxs[1] = 15;
		playerMaxs[2] = 64;//96;//1;

		tries = 0;
		tries2 = 0;

		while (tries < 16)
		{
			tries++;
			tries2 = 0;

			while (tries2 < 128)
			{
				int num_tries; // For secondary spawns. (Behind point).

				tries2++;

				num_tries = tries2;

				if (tries2 <= 16)
				{
				}
				else if (tries2 <= 32)
				{
					num_tries-=16;
				}
				else if (tries2 <= 48)
				{
					num_tries-=32;
				}
				else
				{
					num_tries-=48;
				}
				
				VectorCopy(origin, point);

				if (alt)
				{
					if (tries2 <= 8)
					{
						point[0] -= 1+(tries*64);
						point[1] -= 1+(num_tries*64);
					}
					else if (tries2 <= 16)
					{
						point[0] -= 1+(tries*64);
						point[1] -= 1-(num_tries*64);
					}
					else if (tries2 <= 24)
					{
						point[0] -= 1-(tries*64);
						point[1] -= 1+(num_tries*64);
					}
					else
					{
						point[0] -= 1-(tries*64);
						point[1] -= 1-(num_tries*64);
					}
				}
				else
				{
					if (tries2 <= 8)
					{
						point[0] += 1+(tries*64);
						point[1] += 1+(num_tries*64);
					}
					else if (tries2 <= 16)
					{
						point[0] += 1+(tries*64);
						point[1] += 1-(num_tries*64);
					}
					else if (tries2 <= 24)
					{
						point[0] += 1-(tries*64);
						point[1] += 1+(num_tries*64);
					}
					else
					{
						point[0] += 1-(tries*64);
						point[1] += 1-(num_tries*64);
					}
				}

				if (OrgVisibleBox2(origin, playerMins, playerMaxs, point, -1)
					&& CheckBelowOK(point)
					&& !CheckEntitiesInSpot(point) 
					//&& VectorDistance(point, origin) > 128
					&& !TooCloseToOtherAssociatedSpawnpoint(flagnum, teamnum, point))
				{
					if (teamnum == TEAM_AXIS)
					{
						if (flag_list[flagnum].associated_red_spawnpoints_number >= 256)
						{
							//G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^1RED^5 associated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].associated_red_spawnpoints_number, flagnum);
						}
						else
							AddFlag_Associated_Spawn(flagnum, teamnum, point, angles);
					}
					else if (teamnum == TEAM_ALLIES)
					{
						if (flag_list[flagnum].associated_blue_spawnpoints_number >= 256)
						{
							//G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^4BLUE^5 associated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].associated_blue_spawnpoints_number, flagnum);
						}
						else
							AddFlag_Associated_Spawn(flagnum, teamnum, point, angles);
					}
					else
					{
						if (flag_list[flagnum].associated_blue_spawnpoints_number >= 256 && teamnum == TEAM_ALLIES)
						{
						}
						else if (flag_list[flagnum].associated_red_spawnpoints_number >= 256 && teamnum == TEAM_AXIS)
						{
						}
						else
						{
							AddFlag_Associated_Spawn(flagnum, teamnum, point, angles);
						}
					}
				}
			}
		}

		if (alt)
			break;
		else
			alt = qtrue;
	}

	if (teamnum == TEAM_AXIS)
		G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^1RED^5 associated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].associated_red_spawnpoints_number, flagnum);
	else if (teamnum == TEAM_ALLIES)
		G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^4BLUE^5 associated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].associated_blue_spawnpoints_number, flagnum);
	else
	{
		G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^1RED^5 associated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].associated_red_spawnpoints_number, flagnum);
		G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^4BLUE^5 associated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].associated_blue_spawnpoints_number, flagnum);
	}
}

//===========================================================================
// Routine      : OM_Associated_Spawnarea
// Description  : Spawn an associated spawn area.
void OM_Associated_Spawnarea ( gentity_t* ent )
{// OM Flag's Associated spawn area... Used by map entitiy to call the flag's external spawns...
	int flagnum = 0;

	G_SpawnInt("partofstage", "-1", &flagnum); // Map's flag number.. For linking things to this flag based on team...

	if (flagnum < 0)
	{// We dont have a flag number.. Generate them for this map...
		flagnum = num_flags;
		//num_flags++;
	}

	G_SpawnInt( "spawnflags", "", &ent->spawnflags );

	G_SetOrigin( ent, ent->s.origin );

	if (ent->spawnflags == ALLIED_OBJECTIVE)
	{
		ent->s.modelindex = TEAM_ALLIES;
		G_Printf("^1*** ^5Associated spawnpoints for flag# %i at %f %f %f is owned by ^4TEAM_ALLIES^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}
	else if (ent->spawnflags == AXIS_OBJECTIVE)
	{
		ent->s.modelindex = TEAM_AXIS;
		G_Printf("^1*** ^5Associated spawnpoints for flag# %i at %f %f %f is owned by ^4TEAM_AXIS^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}
	else
	{
		ent->s.modelindex = TEAM_FREE;
		G_Printf("^1*** ^5Associated spawnpoints for flag# %i at %f %f %f is owned by ^4TEAM_NEUTRAL^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}

	ent->targetname = NULL;
	ent->classname = "misc_associated_spawnarea";
	ent->s.eType = ET_ASSOCIATED_SPAWNAREA;
	//ent->think = associated_AI_Think;
	
//	ent->r.contents = CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_MONSTERCLIP;

//	trap_LinkEntity (ent);

	//ent->nextthink = level.time;

	if (ent->spawnflags == AXIS_OBJECTIVE)
	{
		flag_list[flagnum].redAssociatedSpawnareas[flag_list[flagnum].redAssociatedSpawnNumber] = ent;
		flag_list[flagnum].redAssociatedSpawnNumber++;
		PreCalculate_Area_Spawnpoints( flagnum, TEAM_AXIS, ent->s.angles, ent->s.origin );
	}
	else if (ent->spawnflags == ALLIED_OBJECTIVE)
	{
		flag_list[flagnum].blueAssociatedSpawnareas[flag_list[flagnum].blueAssociatedSpawnNumber] = ent;
		flag_list[flagnum].blueAssociatedSpawnNumber++;
		PreCalculate_Area_Spawnpoints( flagnum, TEAM_ALLIES, ent->s.angles, ent->s.origin );
	}
	else
	{
		flag_list[flagnum].redAssociatedSpawnareas[flag_list[flagnum].redAssociatedSpawnNumber] = ent;
		flag_list[flagnum].redAssociatedSpawnNumber++;
		PreCalculate_Area_Spawnpoints( flagnum, TEAM_AXIS, ent->s.angles, ent->s.origin );
		flag_list[flagnum].blueAssociatedSpawnareas[flag_list[flagnum].blueAssociatedSpawnNumber] = ent;
		flag_list[flagnum].blueAssociatedSpawnNumber++;
		PreCalculate_Area_Spawnpoints( flagnum, TEAM_ALLIES, ent->s.angles, ent->s.origin );
	}
}

qboolean red_unassociated_spots_filled[256];
qboolean blue_unassociated_spots_filled[256];

void AddFlag_Unassociated_Spawn ( int flagnum, int teamnum, vec3_t origin, vec3_t angles )
{// Add a new spawn to the list for this flag's external spawn area...
	if (teamnum == TEAM_AXIS)
	{
		if (flag_list[flagnum].unassociated_red_spawnpoints_number < 256)
		{// Create a new spawn point.
			VectorCopy( origin, flag_list[flagnum].unassociated_red_spawnpoints[flag_list[flagnum].unassociated_red_spawnpoints_number] ); // Copy this spawnpoint's location to the end of the list.
			VectorCopy( angles, flag_list[flagnum].unassociated_red_spawnangles[flag_list[flagnum].unassociated_red_spawnpoints_number] ); // Copy this spawnpoint's location to the end of the list.
			flag_list[flagnum].unassociated_red_spawnpoints_number++;
		}
		else
		{
			if (!red_unassociated_spots_filled[flagnum])
			{// Finished making spawns for this flag...
				G_Printf("Calculated 256 unassociated spawns for flag #%i.\n", flagnum);
				red_unassociated_spots_filled[flagnum] = qtrue;
			}
		}
	}
	else
	{
		if (flag_list[flagnum].unassociated_blue_spawnpoints_number < 256)
		{// Create a new spawn point.
			VectorCopy( origin, flag_list[flagnum].unassociated_blue_spawnpoints[flag_list[flagnum].unassociated_blue_spawnpoints_number] ); // Copy this spawnpoint's location to the end of the list.
			VectorCopy( angles, flag_list[flagnum].unassociated_blue_spawnangles[flag_list[flagnum].unassociated_blue_spawnpoints_number] ); // Copy this spawnpoint's location to the end of the list.
			flag_list[flagnum].unassociated_blue_spawnpoints_number++;
		}
		else
		{
			if (!blue_unassociated_spots_filled[flagnum])
			{// Finished making spawns for this flag...
				G_Printf("Calculated 256 unassociated spawns for flag #%i.\n", flagnum);
				blue_unassociated_spots_filled[flagnum] = qtrue;
			}
		}
	}
}

qboolean TooCloseToOtherUnassociatedSpawnpoint ( int flagnum, int teamnum, vec3_t origin )
{
	int i = 0;

	if (teamnum == TEAM_AXIS)
	{
		for (i = 0;i < flag_list[flagnum].unassociated_red_spawnpoints_number;i++)
		{
			if (VectorDistance(flag_list[flagnum].unassociated_red_spawnpoints[i], origin) < 64)
				return qtrue;
		}
	}
	else
	{
		for (i = 0;i < flag_list[flagnum].unassociated_blue_spawnpoints_number;i++)
		{
			if (VectorDistance(flag_list[flagnum].unassociated_blue_spawnpoints[i], origin) < 64)
				return qtrue;
		}
	}

	return qfalse;
}

void PreCalculate_Unassociated_Area_Spawnpoints( int flagnum, int teamnum, vec3_t angles, vec3_t origin )
{
	vec3_t fwd, point;
	int tries = 0, tries2 = 0;
	qboolean alt = qfalse;

	origin[2]+=32;
	VectorCopy(origin, point);
		
	AngleVectors( angles, fwd, NULL, NULL );

	while (1)//visible == qfalse)
	{// In case we need to try a second spawnpoint.
		vec3_t playerMins, playerMaxs;

		playerMins[0] = -15;
		playerMins[1] = -15;
		playerMins[2] = -1;
		playerMaxs[0] = 15;
		playerMaxs[1] = 15;
		playerMaxs[2] = 64;//96;//1;

		tries = 0;
		tries2 = 0;

		while (tries < 16)
		{
			tries++;
			tries2 = 0;

			while (tries2 < 128)
			{
				int num_tries; // For secondary spawns. (Behind point).

				tries2++;

				num_tries = tries2;

				if (tries2 <= 16)
				{
				}
				else if (tries2 <= 32)
				{
					num_tries-=16;
				}
				else if (tries2 <= 48)
				{
					num_tries-=32;
				}
				else
				{
					num_tries-=48;
				}
				
				VectorCopy(origin, point);

				if (alt)
				{
					if (tries2 <= 8)
					{
						point[0] -= 1+(tries*64);
						point[1] -= 1+(num_tries*64);
					}
					else if (tries2 <= 16)
					{
						point[0] -= 1+(tries*64);
						point[1] -= 1-(num_tries*64);
					}
					else if (tries2 <= 24)
					{
						point[0] -= 1-(tries*64);
						point[1] -= 1+(num_tries*64);
					}
					else
					{
						point[0] -= 1-(tries*64);
						point[1] -= 1-(num_tries*64);
					}
				}
				else
				{
					if (tries2 <= 8)
					{
						point[0] += 1+(tries*64);
						point[1] += 1+(num_tries*64);
					}
					else if (tries2 <= 16)
					{
						point[0] += 1+(tries*64);
						point[1] += 1-(num_tries*64);
					}
					else if (tries2 <= 24)
					{
						point[0] += 1-(tries*64);
						point[1] += 1+(num_tries*64);
					}
					else
					{
						point[0] += 1-(tries*64);
						point[1] += 1-(num_tries*64);
					}
				}

				if (OrgVisibleBox2(origin, playerMins, playerMaxs, point, -1)
					&& CheckBelowOK(point)
					&& !CheckEntitiesInSpot(point) 
					//&& VectorDistance(point, origin) > 128
					&& !TooCloseToOtherUnassociatedSpawnpoint(flagnum, teamnum, point))
				{
					if (teamnum == TEAM_AXIS)
					{
						if (flag_list[flagnum].unassociated_red_spawnpoints_number >= 256)
						{
							//G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^1RED^5 unassociated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].unassociated_red_spawnpoints_number, flagnum);
						}
						else
							AddFlag_Unassociated_Spawn(flagnum, teamnum, point, angles);
					}
					else if (teamnum == TEAM_ALLIES)
					{
						if (flag_list[flagnum].unassociated_blue_spawnpoints_number >= 256)
						{
							//G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^4BLUE^5 unassociated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].unassociated_blue_spawnpoints_number, flagnum);
						}
						else
							AddFlag_Unassociated_Spawn(flagnum, teamnum, point, angles);
					}
					else
					{
						if (flag_list[flagnum].unassociated_blue_spawnpoints_number >= 256 && teamnum == TEAM_ALLIES)
						{
						}
						else if (flag_list[flagnum].unassociated_red_spawnpoints_number >= 256 && teamnum == TEAM_AXIS)
						{
						}
						else
						{
							AddFlag_Unassociated_Spawn(flagnum, teamnum, point, angles);
						}
					}
				}
			}
		}

		if (alt)
			break;
		else
			alt = qtrue;
	}

	if (teamnum == TEAM_AXIS)
		G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^1RED^5 unassociated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].associated_red_spawnpoints_number, flagnum);
	else if (teamnum == TEAM_ALLIES)
		G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^4BLUE^5 unassociated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].associated_blue_spawnpoints_number, flagnum);
	else
	{
		G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^1RED^5 unassociated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].associated_red_spawnpoints_number, flagnum);
		G_Printf("^3*** ^3SUPREMACY^5: Added ^7%i^5 ^4BLUE^5 unassociated spawnpoints at flag #^7%i^5.\n", flag_list[flagnum].associated_blue_spawnpoints_number, flagnum);
	}
}

//===========================================================================
// Routine      : OM_Unassociated_Spawnarea
// Description  : Spawn an associated spawn area.
void OM_Unassociated_Spawnarea ( gentity_t* ent )
{// OM Flag's Associated spawn area... Used by map entitiy to call the flag's external spawns...
	int flagnum = 0;

	G_SpawnInt("partofstage", "-1", &flagnum); // Map's flag number.. For linking things to this flag based on team...

	if (flagnum < 0)
	{// We dont have a flag number.. Generate them for this map...
		flagnum = num_flags;
		//num_flags++;
	}

	G_SpawnInt( "spawnflags", "", &ent->spawnflags );

	G_SetOrigin( ent, ent->s.origin );

	if (ent->spawnflags == ALLIED_OBJECTIVE)
	{
		ent->s.modelindex = TEAM_ALLIES;
		G_Printf("^1*** ^5Unasociated spawnpoints for flag# %i at %f %f %f is owned by ^4TEAM_ALLIES^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}
	else if (ent->spawnflags == AXIS_OBJECTIVE)
	{
		ent->s.modelindex = TEAM_AXIS;
		G_Printf("^1*** ^5Unasociated spawnpoints for flag# %i at %f %f %f is owned by ^4TEAM_AXIS^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}
	else
	{
		ent->s.modelindex = TEAM_FREE;
		G_Printf("^1*** ^5Unasociated spawnpoints for flag# %i at %f %f %f is owned by ^4TEAM_NEUTRAL^5!\n", flagnum, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
	}

	ent->targetname = NULL;
	ent->classname = "misc_unassociated_spawnarea";
	ent->s.eType = ET_UNASSOCIATED_SPAWNAREA;
	//ent->think = associated_AI_Think;
	
//	ent->r.contents = CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_MONSTERCLIP;

//	trap_LinkEntity (ent);

	//ent->nextthink = level.time;

	if (ent->spawnflags == AXIS_OBJECTIVE)
	{
		flag_list[flagnum].redUnassociatedSpawnareas[flag_list[flagnum].redAssociatedSpawnNumber] = ent;
		flag_list[flagnum].redUnassociatedSpawnNumber++;
		PreCalculate_Unassociated_Area_Spawnpoints( flagnum, TEAM_AXIS, ent->s.angles, ent->s.origin );
	}
	else if (ent->spawnflags == ALLIED_OBJECTIVE)
	{
		flag_list[flagnum].blueUnassociatedSpawnareas[flag_list[flagnum].blueUnassociatedSpawnNumber] = ent;
		flag_list[flagnum].blueUnassociatedSpawnNumber++;
		PreCalculate_Unassociated_Area_Spawnpoints( flagnum, TEAM_ALLIES, ent->s.angles, ent->s.origin );
	}
	else
	{
		flag_list[flagnum].redUnassociatedSpawnareas[flag_list[flagnum].redAssociatedSpawnNumber] = ent;
		flag_list[flagnum].redUnassociatedSpawnNumber++;
		PreCalculate_Unassociated_Area_Spawnpoints( flagnum, TEAM_AXIS, ent->s.angles, ent->s.origin );
		flag_list[flagnum].blueUnassociatedSpawnareas[flag_list[flagnum].blueUnassociatedSpawnNumber] = ent;
		flag_list[flagnum].blueUnassociatedSpawnNumber++;
		PreCalculate_Unassociated_Area_Spawnpoints( flagnum, TEAM_ALLIES, ent->s.angles, ent->s.origin );
	}
}

#endif //__ETE__

