#include "../game/q_shared.h"

#ifdef __ETE__

#include "g_local.h"

//
// ETE Supression System
//

void ClearSupression ( gentity_t *ent )
{
	ent->supression = 0;

	// Set it here for transmission...
	ent->s.density = ent->supression;
}

void CalculateSuppression ( gentity_t *attacker, vec3_t viewangles, vec3_t start, vec3_t end )
{// Call from weapon fire code...
	int	clientNum = 0;
	
	for (clientNum = 0; clientNum < MAX_CLIENTS; clientNum++)
	{
		gentity_t	*ent = &g_entities[clientNum];
		vec3_t		angles, dir, aangles;
		float		fov = 8;

		if (!ent)
			continue;

		if (!ent->client)
			continue;

		//if (ent->health <= 0)
		//{
		//	ClearSupression(ent);
		//	continue;
		//}

		if (ent->client->ps.stats[STAT_HEALTH] <= 0)
		{
			ClearSupression(ent);
			continue;
		}

		if (ent->client->ps.pm_flags & PMF_LIMBO)
		{
			ClearSupression(ent);
			continue;
		}

		if (ent->client->sess.sessionTeam == TEAM_SPECTATOR)
		{
			ClearSupression(ent);
			continue;
		}

		if (OnSameTeam(ent, attacker))
			continue;

		// Adjust for close targets..
		if (VectorDistance(end, start) < VectorDistance(ent->r.currentOrigin, start))
			continue; // Too far! Bullet didnt go that far lol!
		else if (VectorDistance(ent->r.currentOrigin, start) < 128)
			fov = 120;
		else if (VectorDistance(ent->r.currentOrigin, start) < 256)
			fov = 90;
		else if (VectorDistance(ent->r.currentOrigin, start) < 512)
			fov = 48;
		else if (VectorDistance(ent->r.currentOrigin, start) < 1024)
			fov = 32;
		else if (VectorDistance(ent->r.currentOrigin, start) < 2048)
			fov = 24;
		else if (VectorDistance(ent->r.currentOrigin, start) < 4096)
			fov = 16;
		else
			continue; // Way too far!

		VectorSubtract (ent->r.currentOrigin, start, dir);
		vectoangles(dir, angles);

		VectorSubtract (end, start, dir);
		vectoangles(dir, aangles);

		if (VectorDistance(angles, aangles) > fov)
			continue;

		ent->supression+=5;

		if (ent->supression > 100)
			ent->supression = 100;

		// Set it here for transmission...
		ent->s.density = ent->supression;
		ent->supression_agressor = attacker;

		if ( !ent
			|| !ent->client
			|| ent->client->pers.connected != CON_CONNECTED 
			//|| ent->health <= 0 
			|| ent->client->ps.stats[STAT_HEALTH] <= 0 
			|| (ent->client->ps.pm_flags & PMF_LIMBO) 
			|| ent->client->sess.sessionTeam == TEAM_SPECTATOR )
		{
			ClearSupression(ent);
			return;
		}
	}
}

void CheckSupression ( gentity_t *ent )
{// Call from clientthink...
	if (ent->supression > 100)
		ent->supression = 100;

	if ( !ent
		|| !ent->client
		|| ent->client->pers.connected != CON_CONNECTED 
		//|| ent->health <= 0 
		|| (ent->client->ps.eFlags & EF_DEAD)
		|| ent->client->ps.stats[STAT_HEALTH] <= 0 
		|| (ent->client->ps.pm_flags & PMF_LIMBO) 
		|| ent->client->sess.sessionTeam == TEAM_SPECTATOR )
	{
		ClearSupression(ent);
		return;
	}

	if (ent->supression_reduction_timer <= level.time)
	{// Slowly reduce supression...
		if (ent->supression > 0)
			ent->supression--;

		ent->supression_reduction_timer = level.time + 250;//500;
	}

	// Set it here for transmission...
	ent->s.density = ent->supression;
}

//
// Ammo & Health Crates...
//

extern qboolean AddMagicAmmo(gentity_t *receiver, int numOfClips);
extern qboolean ClientNeedsAmmo( int client );
extern gentity_t *Drop_Item( gentity_t *ent, gitem_t *item, float angle, qboolean novelocity);

//===========================================================================
// Routine      : ammo_crate_think
// Description  : Main think function for ammo crates.
void ammo_crate_think( gentity_t *ent )
{
	vec3_t	mins;
	vec3_t	maxs;
	int		touch[MAX_GENTITIES];
	int		num = 0;
	int		i = 0;
	float	crate_use_distance = ent->s.effect1Time;

#ifdef __ETE__
	int		num_flags = GetNumberOfPOPFlags();

	if ((g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) && num_flags >= 2)
	{
		float	best_distance = 99999.9f;
		int		best_flag = 0;

		for (i = 0; i <= num_flags; i++)
		{
			float distance = VectorDistance(flag_list[i].flagentity->s.origin, ent->s.origin);
			
			if (distance <= best_distance)
			{
				best_flag = i;
				best_distance = distance;
			}
		}

		if (best_distance < 99999.9)
		{
			int flagteam = flag_list[best_flag].flagentity->s.modelindex;

			if (flagteam != TEAM_ALLIES && flagteam != TEAM_AXIS)
				ent->s.density = 0; // Use closed box!
			else
				ent->s.density = 1; // Use open box!
		}
	}
#endif //__ETE__

	VectorSet( mins, 0-(crate_use_distance), 0-(crate_use_distance), 0-(crate_use_distance) );
	VectorSet( maxs, crate_use_distance, crate_use_distance, crate_use_distance );

	VectorAdd( mins, ent->s.origin, mins );
	VectorAdd( maxs, ent->s.origin, maxs );

	if (ent->nextthink > level.time)
		return;

	if (!ent->s.number)
		return;

#ifdef __ETE__
	if (ent->s.density == 0)
	{// Box is currently closed! Don't think any more!
		// Set next think...
		ent->nextthink = level.time + 2000;
		return; 
	}
#endif //__ETE__

	// Run the think... Look for players...
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for ( i=0 ; i<num ; i++ ) 
	{
		gentity_t *hit = &g_entities[touch[i]];

		if (!hit)
			continue;

		//G_Printf("%s is in box!\n", hit->classname);

		if (!hit->client)
			continue;

		if (hit->s.eType != ET_PLAYER)
			continue;

		if (hit->s.number > MAX_CLIENTS)
			continue;

		if (!hit->client->sess.sessionTeam)
			continue;

		if (!hit->client->pers.connected)
			continue;

		if (hit->health <= 0)
			continue;

#ifdef __VEHICLES__
		if (hit->client->ps.eFlags & EF_VEHICLE)
			continue;
#endif //__VEHICLES__

//		if (VectorDistance(hit->r.currentOrigin, ent->s.origin) > crate_use_distance)
//			continue;

		// Give them ammo!
		//G_Printf("%s should be recieving ammo!\n", hit->client->pers.netname);
		/*if (ClientNeedsAmmo(i))
		{
			AddMagicAmmo(hit, 1);
		}*/
		if (ClientNeedsAmmo(touch[i]))
		{
			gitem_t		*item;
			gentity_t	*drop = NULL;
			item = BG_FindItem("Ammo Pack");
			drop = Drop_Item (hit, item, 0, qfalse);
			drop->count = 1;
			continue;
		}
	}

	// Set next think...
	ent->nextthink = level.time + 2000;
}

//===========================================================================
// Routine      : SP_ammo_crate_spawn
// Description  : Spawn an ammo crate.
void SP_ammo_crate_spawn ( gentity_t* ent )
{// Ammo Crate... Used by map entitiy to call the flag's spawn...
	char *model, *model2;
	int radius = 0;

	if (ent->state != 1)
	{
		G_SpawnString("model", "", &model); // Custom model..
		G_SpawnString("model2", "", &model2); // Custom model..
		G_SpawnInt("radius", "", &radius); // Point's capture radius..
		G_SpawnInt( "spawnflags", "", &ent->spawnflags );
		G_SpawnFloat( "scale", "", &ent->scale );
		G_SpawnFloat( "angle", "", &ent->angle );
	}

	G_SetOrigin( ent, ent->s.origin );

	if(ent->state == 1 || !model || model == "" || !strcmp(model, "" ))
	{// ent->model can be specified...
		//ent->model = "models/multiplayer/supplies/ammobox_wm.md3";
		ent->model = "models/doa/doa_cabinet/ammo_open.md3";
		ent->scale = 2.0f;
		
		if (!radius)
		{// Default radius...
			ent->s.effect1Time = 64; // Set capture radius...
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
			ent->s.effect1Time = 64; // Set capture radius...
		}
		else
		{
			ent->s.effect1Time = radius; // Set capture radius...
		}
	}

	if(ent->state == 1 || !model2 || model2 == "" || !strcmp(model2, "" ))
	{// ent->model can be specified...
		//ent->model = "models/multiplayer/supplies/healthbox_wm.md3";
		ent->model2 = "models/doa/doa_cabinet/ammo_close.md3";
	}
	else
	{
		ent->model2 = model;
	}

	if (ent->scale <= 0)
		ent->scale = 1.0f;

	// To transmit scale to clients!
	ent->s.angles2[0] = ent->scale;

	ent->s.angles[YAW] = ent->angle;

	ent->s.modelindex = G_ModelIndex (ent->model);
	ent->s.modelindex2 = G_ModelIndex (ent->model2);

	ent->targetname = NULL;
	ent->classname = "ammo_crate";
	ent->s.eType = ET_AMMO_CRATE;
	ent->think = ammo_crate_think;
	
	// UQ1: Need to fix this! Only basic angles for now!
	if (ent->angle == 90 || ent->angle == 270)
	{
		VectorSet( ent->r.mins, -16, -32, 0 );
		VectorSet( ent->r.maxs, 16, 32, 64 );
	}
	else if (ent->angle == 45 || ent->angle == 135 || ent->angle == 225 || ent->angle == 315)
	{
		VectorSet( ent->r.mins, -24, -24, 0 );
		VectorSet( ent->r.maxs, 24, 24, 64 );
	}
	else
	{
		VectorSet( ent->r.mins, -32, -16, 0 );
		VectorSet( ent->r.maxs, 32, 16, 64 );
	}

	ent->r.contents = CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_MONSTERCLIP;

	trap_LinkEntity (ent);

	G_Printf("Spawned ammo crate at %f %f %f.\n", ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);

	ent->nextthink = level.time;
}

//===========================================================================
// Routine      : health_crate_think
// Description  : Main think function for health crates.
void health_crate_think( gentity_t *ent )
{
	vec3_t	mins;
	vec3_t	maxs;
	int		touch[MAX_GENTITIES];
	int		num = 0;
	int		i = 0;
	float	crate_use_distance = ent->s.effect1Time;

#ifdef __ETE__
	int		num_flags = GetNumberOfPOPFlags();

	if ((g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) && num_flags >= 2)
	{
		float	best_distance = 99999.9f;
		int		best_flag = 0;

		for (i = 0; i <= num_flags; i++)
		{
			float distance = VectorDistance(flag_list[i].flagentity->s.origin, ent->s.origin);
			
			if (distance <= best_distance)
			{
				best_flag = i;
				best_distance = distance;
			}
		}

		if (best_distance < 99999.9)
		{
			int flagteam = flag_list[best_flag].flagentity->s.modelindex;

			if (flagteam != TEAM_ALLIES && flagteam != TEAM_AXIS)
				ent->s.density = 0; // Use closed box!
			else
				ent->s.density = 1; // Use open box!
		}
	}
#endif //__ETE__

	VectorSet( mins, 0-(crate_use_distance), 0-(crate_use_distance), 0-(crate_use_distance) );
	VectorSet( maxs, crate_use_distance, crate_use_distance, crate_use_distance );

	VectorAdd( mins, ent->s.origin, mins );
	VectorAdd( maxs, ent->s.origin, maxs );

	if (ent->nextthink > level.time)
		return;

	if (!ent->s.number)
		return;

#ifdef __ETE__
	if (ent->s.density == 0)
	{// Box is currently closed! Don't think any more!
		// Set next think...
		ent->nextthink = level.time + 2000;
		return; 
	}
#endif //__ETE__

	// Run the think... Look for players...
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for ( i=0 ; i<num ; i++ ) 
	{
		gentity_t *hit = &g_entities[touch[i]];

		if (!hit)
			continue;

		//G_Printf("%s is in box!\n", hit->classname);

		if (!hit->client)
			continue;

		if (hit->s.eType != ET_PLAYER)
			continue;

		if (hit->s.number > MAX_CLIENTS)
			continue;

		if (!hit->client->sess.sessionTeam)
			continue;

		if (!hit->client->pers.connected)
			continue;

		if (hit->health <= 0)
			continue;

#ifdef __VEHICLES__
		if (hit->client->ps.eFlags & EF_VEHICLE)
			continue;
#endif //__VEHICLES__

//		if (VectorDistance(hit->r.currentOrigin, ent->s.origin) > crate_use_distance)
//			continue;

		// Give them ammo!
		//G_Printf("%s should be recieving health!\n", hit->client->pers.netname);
		if (hit->client->ps.stats[STAT_HEALTH] < hit->client->ps.stats[STAT_MAX_HEALTH])
		{
			gitem_t		*item;
			gentity_t	*drop = NULL;
			item = BG_FindItemForClassName("item_health");
			drop = Drop_Item (hit, item, 0, qfalse);
			drop->count = 1;
			continue;
		}
	}

	// Set next think...
	ent->nextthink = level.time + 2000;
}

//===========================================================================
// Routine      : SP_health_crate_spawn
// Description  : Spawn a health crate.
void SP_health_crate_spawn ( gentity_t* ent )
{// Health Crate... Used by map entitiy to call the flag's spawn...
	char *model, *model2;
	int radius = 0;

	if(g_realism.integer)
	{// UQ1: No health crates in realism mode!
		G_FreeEntity(ent);
		return;
	}

	if (ent->state != 1)
	{
		G_SpawnString("model", "", &model); // Custom model..
		G_SpawnString("model2", "", &model2); // Custom model..
		G_SpawnInt("radius", "", &radius); // Point's capture radius..
		G_SpawnInt( "spawnflags", "", &ent->spawnflags );
		G_SpawnFloat( "scale", "", &ent->scale );
		G_SpawnFloat( "angle", "", &ent->angle );
	}

	G_SetOrigin( ent, ent->s.origin );

	if(ent->state == 1 || !model || model == "" || !strcmp(model, "" ))
	{// ent->model can be specified...
		//ent->model = "models/multiplayer/supplies/healthbox_wm.md3";
		ent->model = "models/doa/doa_cabinet/health_open.md3";
		ent->scale = 2.0f;

		if (!radius)
		{// Default radius...
			ent->s.effect1Time = 64; // Set capture radius...
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
			ent->s.effect1Time = 64; // Set capture radius...
		}
		else
		{
			ent->s.effect1Time = radius; // Set capture radius...
		}
	}

	if(ent->state == 1 || !model2 || model2 == "" || !strcmp(model2, "" ))
	{// ent->model can be specified...
		//ent->model = "models/multiplayer/supplies/healthbox_wm.md3";
		ent->model2 = "models/doa/doa_cabinet/health_close.md3";
	}
	else
	{
		ent->model2 = model;
	}

	if (ent->scale <= 0)
		ent->scale = 1.0f;

	// To transmit scale to clients!
	ent->s.angles2[0] = ent->scale;

	ent->s.angles[YAW] = ent->angle;

	ent->s.modelindex = G_ModelIndex (ent->model);
	ent->s.modelindex2 = G_ModelIndex (ent->model2);

	ent->targetname = NULL;
	ent->classname = "health_crate";
	ent->s.eType = ET_HEALTH_CRATE;
	ent->think = health_crate_think;
	
	// UQ1: Need to fix this! Only basic angles for now!
	if (ent->angle == 90 || ent->angle == 270)
	{
		VectorSet( ent->r.mins, -16, -32, 0 );
		VectorSet( ent->r.maxs, 16, 32, 64 );
	}
	else if (ent->angle == 45 || ent->angle == 135 || ent->angle == 225 || ent->angle == 315)
	{
		VectorSet( ent->r.mins, -24, -24, 0 );
		VectorSet( ent->r.maxs, 24, 24, 64 );
	}
	else
	{
		VectorSet( ent->r.mins, -32, -16, 0 );
		VectorSet( ent->r.maxs, 32, 16, 64 );
	}

	ent->r.contents = CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_MONSTERCLIP;

	trap_LinkEntity (ent);

	G_Printf("Spawned health crate at %f %f %f.\n", ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);

	ent->nextthink = level.time;
}

/*
{
angle "40"
model "models/mapobjects/miltary_trim/sandbag1_45.md3"
origin "-1994 1633 146"
classname "misc_gamemodel"
targetname "browning_sbags_1"
}
*/

//===========================================================================
// Routine      : SP_sandbags_spawn
// Description  : Spawn sandbags.
void SP_sandbags_spawn ( gentity_t* ent )
{// Sandbags...
	G_SpawnFloat( "angle", "", &ent->angle );
	G_SetOrigin( ent, ent->s.origin );
	ent->model = "models/mapobjects/miltary_trim/sandbag1_45.md3";
	ent->s.angles[YAW] = ent->angle;
	ent->s.modelindex = G_ModelIndex (ent->model);
	ent->targetname = NULL;
	ent->classname = "misc_sandbags";
	ent->s.eType = ET_SANDBAG;
	
	// UQ1: Need to fix this! Only basic angles for now!
	if (ent->angle == 90 || ent->angle == 270)
	{
		VectorSet( ent->r.mins, -16, -48, 0 );
		VectorSet( ent->r.maxs, 16, 48, 24 );
	}
	else
	{
		VectorSet( ent->r.mins, -48, -16, 0 );
		VectorSet( ent->r.maxs, 48, 16, 24 );
	}

	ent->r.contents = CONTENTS_SOLID/*|CONTENTS_BODY|CONTENTS_MONSTERCLIP*/;

	trap_LinkEntity (ent);
}

qboolean TrySandbagConstruction( gentity_t *ent )
{
	if (ent->client->touchEnt->s.eType == ET_CONSTRUCTIBLE_SANDBAGS)
	{
		if (ent->client->touchEnt->s.dl_intensity != SB_CONSTRUCTED)
		{
			ent->client->touchEnt->s.effect1Time++;
			return ( qtrue );
		}
	}

	return ( qfalse );
}

void constructible_sandbag_unconstructed_think( gentity_t *ent )
{
	vec3_t origin;

	trap_UnlinkEntity(ent);
	// Draw the crates and flag!
	ent->model			= "models/mapobjects/cmarker/cmarker_crates.md3";
	
	if (ent->s.teamNum == TEAM_ALLIES)
		ent->aiSkin			= "models/mapobjects/cmarker/allied_crates.skin";
	else
		ent->aiSkin			= "models/mapobjects/cmarker/axis_crates.skin";

	ent->s.modelindex = G_ModelIndex (ent->model);
	ent->s.modelindex2 = -1;
	ent->s.effect3Time	= G_SkinIndex( ent->aiSkin );
	ent->targetname = NULL;

	// UQ1: Need to fix this! Only basic angles for now!
	if (ent->angle == 90 || ent->angle == 270 )
	{
		VectorSet( ent->r.mins, -6, -16, 0 );
		VectorSet( ent->r.maxs, 6, 16, 24 );
	}
	else
	{
		VectorSet( ent->r.mins, -16, -6, 0 );
		VectorSet( ent->r.maxs, 16, 6, 24 );
	}

	ent->r.contents = CONTENTS_SOLID | CONTENTS_MISSILECLIP/*|CONTENTS_BODY|CONTENTS_MONSTERCLIP*/;
	ent->clipmask =	  CONTENTS_SOLID | CONTENTS_MISSILECLIP;

	ent->s.origin[2] = ent->bot_coverspot_enemy_position[1];
	ent->r.currentOrigin[2] = ent->bot_coverspot_enemy_position[1];
	ent->s.pos.trBase[2] = ent->bot_coverspot_enemy_position[1];
	ent->s.pos.trType = TR_STATIONARY;
	ent->s.pos.trTime = level.time;
	VectorSet(ent->s.pos.trDelta, 0, 0, 0);

	trap_LinkEntity (ent);
}

void constructible_sandbag_constructing_think( gentity_t *ent )
{
	trap_UnlinkEntity(ent);
	// Draw the crates and flag!
	ent->model			= "models/mapobjects/cmarker/cmarker_crates.md3";
	
	if (ent->s.teamNum == TEAM_ALLIES)
		ent->aiSkin			= "models/mapobjects/cmarker/allied_crates.skin";
	else
		ent->aiSkin			= "models/mapobjects/cmarker/axis_crates.skin";

	ent->s.modelindex = G_ModelIndex (ent->model);
	ent->model2 = "models/mapobjects/miltary_trim/sandbag1_45.md3";
	ent->s.modelindex2 = G_ModelIndex (ent->model2);
	ent->s.effect3Time	= G_SkinIndex( ent->aiSkin );
	ent->targetname = NULL;
	
	// UQ1: Need to fix this! Only basic angles for now!
	if (ent->angle == 90 || ent->angle == 270 )
	{
		VectorSet( ent->r.mins, -16, -48, 0 );
		VectorSet( ent->r.maxs, 16, 48, 24 );
	}
	else
	{
		VectorSet( ent->r.mins, -48, -16, 0 );
		VectorSet( ent->r.maxs, 48, 16, 24 );
	}

	ent->r.contents = CONTENTS_SOLID/*|CONTENTS_BODY|CONTENTS_MONSTERCLIP*/;

	trap_LinkEntity (ent);
}

void constructible_sandbag_constructed_think( gentity_t *ent )
{
	trap_UnlinkEntity(ent);
	ent->model = "models/mapobjects/miltary_trim/sandbag1_45.md3";
	ent->model2 = "";
	ent->s.modelindex = G_ModelIndex (ent->model);
	ent->s.modelindex2 = -1;
	ent->targetname = NULL;
	ent->s.effect3Time	= 0;
	
	// UQ1: Need to fix this! Only basic angles for now!
	if (ent->angle == 90 || ent->angle == 270 )
	{
		VectorSet( ent->r.mins, -16, -48, 0 );
		VectorSet( ent->r.maxs, 16, 48, 24 );
	}
	else
	{
		VectorSet( ent->r.mins, -48, -16, 0 );
		VectorSet( ent->r.maxs, 48, 16, 24 );
	}

	ent->r.contents = CONTENTS_SOLID/*|CONTENTS_BODY|CONTENTS_MONSTERCLIP*/;

	trap_LinkEntity (ent);
}

void constructible_sandbag_destroyed_think( gentity_t *ent )
{
	trap_UnlinkEntity(ent);
	ent->think = G_FreeEntity;
}

void constructible_sandbag_think( gentity_t *ent )
{
	if (ent->nextthink > level.time)
		return;

	// Change the state if needed first...
	if (ent->s.dl_intensity == SB_UNCONSTRUCTED && ent->s.effect1Time > 0)
		ent->s.dl_intensity = SB_CONSTRUCTING;

	if (ent->s.dl_intensity == SB_CONSTRUCTING && ent->s.effect1Time > 150)
		ent->s.dl_intensity = SB_CONSTRUCTED;

	if (ent->health <= 0)
		ent->s.dl_intensity = SB_DESTROYED;

	// And now do the real thinking...
	switch (ent->s.dl_intensity)
	{
		case SB_UNCONSTRUCTED:
			constructible_sandbag_unconstructed_think(ent);
			break;
		case SB_CONSTRUCTING:
			constructible_sandbag_constructing_think(ent);
			break;
		case SB_CONSTRUCTED:
			constructible_sandbag_constructed_think(ent);
			break;
		case SB_DESTROYED:
			constructible_sandbag_destroyed_think(ent);
			break;
		default:
			constructible_sandbag_unconstructed_think(ent);
			break;
	}

	ent->nextthink = level.time + (2*FRAMETIME);
}

extern vec3_t mg42_spawn_position;
extern void SP_mg42 (gentity_t *self);

qboolean Try_MG_Nest_Construction( gentity_t *ent )
{
	if (ent->client->touchEnt->s.eType == ET_CONSTRUCTIBLE_MG_NEST)
	{
		if (ent->client->touchEnt->s.dl_intensity != SB_CONSTRUCTED)
		{
			ent->client->touchEnt->s.effect1Time++;
			return ( qtrue );
		}
	}

	return ( qfalse );
}

float AirdropGetSkyHeight( gentity_t *ent )
{
	vec3_t   dest;
	trace_t	 tr;

	// drop to floor
	ent->r.currentOrigin[2] += 4.0f;	// fixes QErad -> engine bug?
	VectorSet( dest, ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2] + 16550 );
	trap_Trace( &tr, ent->r.currentOrigin, NULL, NULL, dest, ent->s.number, MASK_PLAYERSOLID );
	
	return tr.endpos[2];
}

void constructible_mg_nest_unconstructed_think( gentity_t *ent )
{
	vec3_t origin;

	trap_UnlinkEntity(ent);
	// Draw the crates and flag!
	ent->model			= "models/mapobjects/cmarker/cmarker_crates.md3";
	
	if (ent->s.teamNum == TEAM_ALLIES)
		ent->aiSkin			= "models/mapobjects/cmarker/allied_crates.skin";
	else
		ent->aiSkin			= "models/mapobjects/cmarker/axis_crates.skin";

	ent->s.modelindex = G_ModelIndex (ent->model);
	ent->s.modelindex2 = -1;
	ent->s.effect3Time	= G_SkinIndex( ent->aiSkin );
	ent->targetname = NULL;

	// UQ1: Need to fix this! Only basic angles for now!
	if (ent->angle == 90 || ent->angle == 270 )
	{
		VectorSet( ent->r.mins, -6, -16, 0 );
		VectorSet( ent->r.maxs, 6, 16, 24 );
	}
	else
	{
		VectorSet( ent->r.mins, -16, -6, 0 );
		VectorSet( ent->r.maxs, 16, 6, 24 );
	}

	ent->r.contents = CONTENTS_SOLID | CONTENTS_MISSILECLIP/*|CONTENTS_BODY|CONTENTS_MONSTERCLIP*/;
	ent->clipmask =	  CONTENTS_SOLID | CONTENTS_MISSILECLIP;

	// get current position
	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin, qfalse, ent->s.effect2Time );

	if (origin[2] <= ent->bot_coverspot_enemy_position[1])
	{
		ent->s.origin[2] = ent->bot_coverspot_enemy_position[1];
		ent->r.currentOrigin[2] = ent->bot_coverspot_enemy_position[1];
		ent->s.pos.trBase[2] = ent->bot_coverspot_enemy_position[1];
		ent->s.pos.trType = TR_STATIONARY;
		ent->s.pos.trTime = level.time;
		VectorSet(ent->s.pos.trDelta, 0, 0, 0);
	}

	trap_LinkEntity (ent);
}

void constructible_mg_nest_constructing_think( gentity_t *ent )
{
	trap_UnlinkEntity(ent);
	// Draw the crates and flag!
	ent->model			= "models/mapobjects/cmarker/cmarker_crates.md3";
	
	if (ent->s.teamNum == TEAM_ALLIES)
		ent->aiSkin			= "models/mapobjects/cmarker/allied_crates.skin";
	else
		ent->aiSkin			= "models/mapobjects/cmarker/axis_crates.skin";

	ent->s.modelindex = G_ModelIndex (ent->model);
	ent->model2 = "models/mapobjects/miltary_trim/sandbag1_45.md3";
	ent->s.modelindex2 = G_ModelIndex (ent->model2);
	ent->s.effect3Time	= G_SkinIndex( ent->aiSkin );
	ent->targetname = NULL;
	
	// UQ1: Need to fix this! Only basic angles for now!
	if (ent->angle == 90 || ent->angle == 270 )
	{
		VectorSet( ent->r.mins, -6, -48, 0 );
		VectorSet( ent->r.maxs, 6, 48, 24 );
	}
	else
	{
		VectorSet( ent->r.mins, -48, -6, 0 );
		VectorSet( ent->r.maxs, 48, 6, 24 );
	}

	ent->r.contents = CONTENTS_SOLID/*|CONTENTS_BODY|CONTENTS_MONSTERCLIP*/;
	ent->clipmask =			CONTENTS_SOLID;

	trap_LinkEntity (ent);

	if (!ent->bot_goal_entity)
	{// Spawn the MG as well... But set is as in the "under construction" state...
		gentity_t	*mg42 = G_Spawn();
		vec3_t		forward;
			
		mg42->player_mg42 = qtrue;

		VectorCopy(ent->r.currentOrigin, ent->s.origin);
		
		G_SetAngle(mg42, ent->s.angles);
		G_SetOrigin(mg42, ent->s.origin);

		VectorCopy(ent->s.origin, mg42->s.origin);
		VectorCopy(ent->s.origin, mg42->r.currentOrigin);
		mg42->s.origin[2]+=16;
		mg42->r.currentOrigin[2]+=16;
		
		VectorCopy(ent->s.angles, mg42->s.angles);
		VectorCopy(ent->s.angles, mg42->r.currentAngles);
		mg42->s.angles[YAW]-=90;
		mg42->r.currentAngles[YAW]-=90;

		AngleVectors( mg42->s.angles, forward, NULL, NULL );
		VectorMA( mg42->r.currentOrigin, -32, forward, mg42->r.currentOrigin );
		VectorMA( mg42->s.origin, -32, forward, mg42->s.origin );
		
		// Mark the MG as still constructing...
		mg42->s.onFireEnd = 1;

		// Let the mg42 temp ent know it is linked to the sandbag entity...
		mg42->bot_goal_entity = ent;

		// Temp link the sandbag to the mg42...
		ent->bot_goal_entity = mg42;

		if (ent->s.teamNum == TEAM_ALLIES)
		{// 30 CAL
			mg42->s.density = 1;
		}
		else
		{// MG 42
			mg42->s.density = 0;
		}

		//G_Printf("1. Spawning at %f %f %f (angles %f %f %f)\n", mg42->s.origin[0], mg42->s.origin[1], mg42->s.origin[2], mg42->s.angles[0], mg42->s.angles[1], mg42->s.angles[2]);
		SP_mg42(mg42);
	}
}

void constructible_mg_nest_constructed_think( gentity_t *ent )
{
	trap_UnlinkEntity(ent);
	ent->model = "models/mapobjects/miltary_trim/sandbag1_45.md3";
	ent->model2 = "";
	ent->s.modelindex = G_ModelIndex (ent->model);
	ent->s.modelindex2 = -1;
	ent->targetname = NULL;
	ent->s.effect3Time	= 0;
	
	// UQ1: Need to fix this! Only basic angles for now!
	if (ent->angle == 90 || ent->angle == 270 )
	{
		VectorSet( ent->r.mins, -6, -48, 0 );
		VectorSet( ent->r.maxs, 6, 48, 24 );
	}
	else
	{
		VectorSet( ent->r.mins, -48, -6, 0 );
		VectorSet( ent->r.maxs, 48, 6, 24 );
	}

	ent->r.contents = CONTENTS_SOLID/*|CONTENTS_BODY|CONTENTS_MONSTERCLIP*/;
	ent->clipmask =			CONTENTS_SOLID;

	trap_LinkEntity (ent);

	// Mark it's MG as fully constructed...
	ent->bot_goal_entity->s.onFireEnd = 0;
	ent->bot_goal_entity->bot_goal_entity->s.onFireEnd = 0;

	ent->bot_goal_entity->clipmask =		CONTENTS_SOLID;
	ent->bot_goal_entity->r.contents =		CONTENTS_TRIGGER;
	ent->bot_goal_entity->bot_goal_entity->clipmask =		0;//CONTENTS_SOLID;
	ent->bot_goal_entity->bot_goal_entity->r.contents =		0;//CONTENTS_SOLID;
}

void constructible_mg_nest_destroyed_think( gentity_t *ent )
{
	// Unlink the MG's base...
	trap_UnlinkEntity(ent->bot_goal_entity->bot_goal_entity);
	G_FreeEntity(ent->bot_goal_entity->bot_goal_entity);

	// Remove the MG...
	trap_UnlinkEntity(ent->bot_goal_entity);
	G_FreeEntity(ent->bot_goal_entity);

	// Remove the sandbags...
	trap_UnlinkEntity(ent);
	ent->think = G_FreeEntity;
}

void constructible_mg_nest_think( gentity_t *ent )
{
	if (ent->nextthink > level.time)
		return;

	// Change the state if needed first...
	if (ent->s.dl_intensity == SB_UNCONSTRUCTED && ent->s.effect1Time > 0)
		ent->s.dl_intensity = SB_CONSTRUCTING;

	if (ent->s.dl_intensity == SB_CONSTRUCTING && ent->s.effect1Time > 250)
		ent->s.dl_intensity = SB_CONSTRUCTED;

	if (ent->health <= 0)
		ent->s.dl_intensity = SB_DESTROYED;

	// And now do the real thinking...
	switch (ent->s.dl_intensity)
	{
		case SB_UNCONSTRUCTED:
			constructible_mg_nest_unconstructed_think(ent);
			break;
		case SB_CONSTRUCTING:
			constructible_mg_nest_constructing_think(ent);
			break;
		case SB_CONSTRUCTED:
			constructible_mg_nest_constructed_think(ent);
			break;
		case SB_DESTROYED:
			constructible_mg_nest_destroyed_think(ent);
			break;
		default:
			constructible_mg_nest_unconstructed_think(ent);
			break;
	}

	ent->nextthink = level.time + (2*FRAMETIME);
}

/*typedef enum {
	AIRDROP_TYPE_SANDBAGS,
	AIRDROP_TYPE_MG_NEST,
} airdrop_types_t;*/

/*
===============
AirdropDropToFloor
===============
*/
void AirdropDropToFloor( gentity_t *ent )
{
	vec3_t   dest;
	trace_t	 tr;

	// drop to floor
	ent->r.currentOrigin[2] += 16.0f;	// fixes QErad -> engine bug?
	VectorSet( dest, ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2] - 4096 );
	trap_Trace( &tr, ent->r.currentOrigin, NULL, NULL, dest, ent->s.number, MASK_PLAYERSOLID );
	G_SetOrigin( ent, tr.endpos );
	VectorCopy( ent->r.currentOrigin, ent->s.origin );
}

qboolean AirdropCheckSky( gentity_t *ent )
{
	vec3_t   dest;
	trace_t	 tr;

	// drop to floor
	ent->r.currentOrigin[2] += 4.0f;	// fixes QErad -> engine bug?
	VectorSet( dest, ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2] + 16550 );
	trap_Trace( &tr, ent->r.currentOrigin, NULL, NULL, dest, ent->s.number, MASK_PLAYERSOLID );
	
	if (!(tr.surfaceFlags & SURF_SKY) && !(tr.surfaceFlags & SURF_NOIMPACT) && !(tr.surfaceFlags & SURF_NOMARKS))
		return qfalse;

	return qtrue;
}

void Create_Constructible_Airdrop( gentity_t *ent ) 
{
	gentity_t	*constructible = G_Spawn();
	vec3_t		forward, org, velocity;
	int			diff1, diff2, diff3, diff4, diff5;

	if (!AirdropCheckSky(ent))
	{// Failed. Indoors or covered by a player...
		if(ent->client) {
			G_SayTo( ent->parent, ent->parent, 2, COLOR_YELLOW, "Pilot: ", "Aborting, can't see target.", qtrue );
			G_GlobalClientEvent( EV_AIRSTRIKEMESSAGE, 1, ent->parent-g_entities );
			return;
		}

		return;
	}

	//
	// The Constructible Itself!
	//

	if (ent->client)
		AngleVectors (ent->client->ps.viewangles, forward, NULL, NULL);
	else
		AngleVectors (ent->s.angles2, forward, NULL, NULL);

	//push the origin out a tad so it's not right in the guys face (tad==4)
	VectorMA(ent->r.currentOrigin, 64, forward, org);

	if (ent->client)
		org[2]-=26;

	G_SetOrigin( constructible, org );
	AirdropDropToFloor( constructible );
	
	if (!ent->client)
		constructible->r.currentOrigin[2]+=16;
	else
		constructible->r.currentOrigin[2]-=5;

	if (ent->client)
	{
		diff1 = abs(ent->client->ps.viewangles[YAW] - 0);
		diff2 = abs(ent->client->ps.viewangles[YAW] - 90);
		diff3 = abs(ent->client->ps.viewangles[YAW] - 180);
		diff4 = abs(ent->client->ps.viewangles[YAW] - 270);
		diff5 = abs(ent->client->ps.viewangles[YAW] - 360);
	}
	else
	{
		diff1 = abs(ent->s.angles2[YAW] - 0);
		diff2 = abs(ent->s.angles2[YAW] - 90);
		diff3 = abs(ent->s.angles2[YAW] - 180);
		diff4 = abs(ent->s.angles2[YAW] - 270);
		diff5 = abs(ent->s.angles2[YAW] - 360);
	}

	if (ent->client)
		constructible->s.teamNum = ent->client->sess.sessionTeam;
	else
		constructible->s.teamNum = ent->s.teamNum;

	constructible->health = 100;
	constructible->model = "models/mapobjects/miltary_trim/sandbag1_45.md3";
	
	if (ent->client)
		constructible->s.angles[YAW] = ent->client->ps.viewangles[YAW]+90;
	else
		constructible->s.angles[YAW] = ent->s.angles2[YAW]+90;
	
	if (diff1 < diff2 && diff1 < diff3 && diff1 < diff4 && diff1 < diff5)
		constructible->angle = 90;
	else if (diff2 < diff1 && diff2 < diff3 && diff2 < diff4 && diff2 < diff5)
		constructible->angle = 180;
	else if (diff3 < diff1 && diff3 < diff2 && diff3 < diff4 && diff3 < diff5)
		constructible->angle = 270;
	else if (diff4 < diff1 && diff4 < diff2 && diff4 < diff3 && diff4 < diff5)
		constructible->angle = 0;
	else if (diff5 < diff2 && diff5 < diff3 && diff5 < diff4 && diff5 < diff1)
		constructible->angle = 90;

	constructible->s.angles[PITCH] = 0;
	constructible->s.angles[ROLL] = 0;
	constructible->s.modelindex = G_ModelIndex (ent->model);
	constructible->s.modelindex2 = -1;
	constructible->s.effect1Time = 0;
	constructible->s.effect3Time = 0;
	constructible->s.dl_intensity = SB_UNCONSTRUCTED;

	constructible->targetname = NULL;
	
	// UQ1: Need to fix this! Only basic angles for now!
	VectorSet( constructible->r.mins, -16, -16, 0 );
	VectorSet( constructible->r.maxs, 16, 16, 32 );

	constructible->r.contents = CONTENTS_SOLID/*|CONTENTS_BODY|CONTENTS_MONSTERCLIP*/;
	constructible->clipmask =	CONTENTS_SOLID;

	// For dropping from sky...
	constructible->bot_coverspot_enemy_position[0] = AirdropGetSkyHeight(ent) - 32;
	constructible->bot_coverspot_enemy_position[1] = constructible->s.origin[2];
	constructible->s.origin[2] = constructible->bot_coverspot_enemy_position[0];
	constructible->r.currentOrigin[2] = constructible->bot_coverspot_enemy_position[0];
	VectorCopy(constructible->s.origin, constructible->s.pos.trBase);
	constructible->s.pos.trType = TR_GRAVITY;
	constructible->s.pos.trTime = level.time;
	velocity[0] = 0;
	velocity[1] = 0;
	velocity[2] = 0;
	VectorScale(velocity, 100, velocity);
	velocity[2] = 10;
	VectorCopy( velocity, constructible->s.pos.trDelta );
	constructible->s.eFlags |= EF_BOUNCE/*EF_BOUNCE_HALF*/;

	switch (ent->action)
	{// Which type is it?
	case AIRDROP_TYPE_SANDBAGS:
		constructible->classname = "sandbag_constructible";
		constructible->s.eType = ET_CONSTRUCTIBLE_SANDBAGS;
		constructible->think = constructible_sandbag_think;
		constructible->nextthink = level.time + (2*FRAMETIME);
		constructible->action = ent->action;
		constructible_sandbag_think(constructible);
		break;
	case AIRDROP_TYPE_MG_NEST:
		constructible->classname = "mg_nest_constructible";
		constructible->s.eType = ET_CONSTRUCTIBLE_MG_NEST;
		constructible->think = constructible_mg_nest_think;
		constructible->nextthink = level.time + (2*FRAMETIME);
		constructible->action = ent->action;
		constructible_mg_nest_think(constructible);
		break;
	default:
		ent->action = AIRDROP_TYPE_SANDBAGS;
		constructible->classname = "sandbag_constructible";
		constructible->s.eType = ET_CONSTRUCTIBLE_SANDBAGS;
		constructible->think = constructible_sandbag_think;
		constructible->nextthink = level.time + (2*FRAMETIME);
		constructible->action = ent->action;
		constructible_sandbag_think(constructible);
		break;
	}
}

int num_stealth_areas = 0;
vec3_t stealth_area_origin[1024];
int stealth_area_radius[1024];

extern void Create_Cover_Spot ( gentity_t *ent );

/*
===============
SP_Stealth_Area
===============
*/
void SP_Stealth_Area( gentity_t *ent ) {
	int radius;

	G_SpawnInt("radius", "", &radius); // Point's capture radius..

	if (radius <= 0)
		radius = 512;

	ent->radius = radius;

	AirdropDropToFloor( ent );
	ent->s.origin[2]-=32;
	VectorCopy(ent->s.origin, stealth_area_origin[num_stealth_areas]);
	//stealth_area_origin[num_stealth_areas][2]-=32;
	stealth_area_radius[num_stealth_areas] = radius;
	
	//G_Printf("Added stealth area %i at %f %f %f (radius %i).\n", num_stealth_areas, stealth_area_origin[num_stealth_areas][0], stealth_area_origin[num_stealth_areas][1], stealth_area_origin[num_stealth_areas][2], stealth_area_radius[num_stealth_areas]);

	num_stealth_areas++;

//	G_FreeEntity(ent);
	ent->r.svFlags |= SVF_NOCLIENT;
	ent->s.eType = ET_SNIPER_HINT;

#ifdef __OLD_COVER_SPOTS__
	Create_Cover_Spot(ent);
#endif //__OLD_COVER_SPOTS__
}

//
// Unique's Fog Variation!
//

void UQ_SetGlobalFog( qboolean restore, int duration, vec3_t color, float depthForOpaque ) 
{
	if( restore )
	{// Clear values for clients...
		trap_SetConfigstring( CS_FOGVARS, va("%f %f %f %f %f %f %i", 0.0f, (float)depthForOpaque, 1.0f, (float)color[0], (float)color[1], (float)color[2], duration) );
		return;
	}

	trap_SetConfigstring( CS_FOGVARS, va("%f %f %f %f %f %f %i", 1.0f, (float)depthForOpaque, 255.0f, (float)color[0], (float)color[1], (float)color[2], duration) );
}

int		next_fog_check = 0;
float	current_fog_view_distance = 3012;
float	maximum_fog_view_distance = 5648;
float	minimum_fog_view_distance = 1024;
vec3_t	fog_color;

vmCvar_t mapname;
qboolean is_fog_kludge_map = qfalse;
qboolean fog_kludge_checked = qfalse;

qboolean UQ_Fog_Kludge ( void )
{
	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	if (is_fog_kludge_map)
	{
		return qtrue;
	}

	if (!fog_kludge_checked)
	{// UQ1: Load from file!
		int len = 0;
		fileHandle_t file;

		next_fog_check = level.time - 10000;

		// Init fog color before reading.. In case the file does not set it!
		fog_color[0] = 1.0f;
		fog_color[1] = 1.0f;
		fog_color[2] = 1.0f;

		len = trap_FS_FOpenFile(va("maps/%s.fog", mapname.string), &file, FS_READ);

		fog_kludge_checked = qtrue;

		if (len > 0)
		{
			char		*text_p;
			char		*token;
			char		bigTextBuffer[5000];

			is_fog_kludge_map = qtrue;

			trap_FS_Read( bigTextBuffer, len, file );
			bigTextBuffer[len] = 0;
			trap_FS_FCloseFile( file );

			// parse the text
			text_p = bigTextBuffer;

			G_Printf( "^1*** ^3%s^5: Successfully loaded map fog info from file ^7/maps/%s.fog^5.\n", GAME_VERSION, mapname.string);

			token = COM_Parse( &text_p );

			if ( !token ) {
				return qtrue;
			}

			current_fog_view_distance = atof(token);

			token = COM_Parse( &text_p );
			
			if ( !token ) {
				return qtrue;
			}

			minimum_fog_view_distance = atof(token);

			token = COM_Parse( &text_p );
			
			if ( !token ) {
				return qtrue;
			}

			maximum_fog_view_distance = atof(token);

			token = COM_Parse( &text_p );
			
			if ( !token ) {
				fog_color[0] = 1;
				fog_color[1] = 1;
				fog_color[2] = 1;
				return qtrue;
			}
			
			fog_color[0] = atof(token);

			token = COM_Parse( &text_p );
			
			if ( !token ) {
				return qtrue;
			}
			
			fog_color[1] = atof(token);

			token = COM_Parse( &text_p );
			
			if ( !token ) {
				return qtrue;
			}
			
			fog_color[2] = atof(token);

			token = COM_Parse( &text_p );

			if (token[0] && Q_TrueRand(0,9) <= 4)
			{// UQ1: Added. Allow for a secondary setting (randomly used)...
				current_fog_view_distance = atof(token);

				token = COM_Parse( &text_p );
			
				if ( !token ) {
					return qtrue;
				}

				minimum_fog_view_distance = atof(token);

				token = COM_Parse( &text_p );
			
				if ( !token ) {
					return qtrue;
				}

				maximum_fog_view_distance = atof(token);

				token = COM_Parse( &text_p );
			
				if ( !token ) {
					fog_color[0] = 1;
					fog_color[1] = 1;
					fog_color[2] = 1;
					return qtrue;
				}
			
				fog_color[0] = atof(token);

				token = COM_Parse( &text_p );
			
				if ( !token ) {
					return qtrue;
				}
			
				fog_color[1] = atof(token);

				token = COM_Parse( &text_p );
			
				if ( !token ) {
					return qtrue;
				}
			
				fog_color[2] = atof(token);
			}

			return qtrue;
		}
		else
		{
			G_Printf( "^1*** ^3%s^5: No fog info for this map. If you want fog, create ^7/maps/%s.fog^5.\n", GAME_VERSION, mapname.string);
		}
	}

	return qfalse;
}

void UQ_DoFogVariation ( void )
{// If it is a supported map, vary fog over time...
	int			choice1 = 0,		choice2 = 0,		diff = 0;
	vec3_t	color;
	qboolean init = qfalse;

	if (!UQ_Fog_Kludge())
	{
		return;
	}

	if (fog_color[0] == 0 && fog_color[1] == 0 && fog_color[2] == 0)
	{
		fog_color[0] = 1;
		fog_color[1] = 1;
		fog_color[2] = 1;
	}

	VectorCopy(fog_color, color);

	if (current_fog_view_distance <= 0)
	{// Init!
		init = qtrue;
		current_fog_view_distance = 3012;
		maximum_fog_view_distance = 5648;
		minimum_fog_view_distance = 1024;

		UQ_SetGlobalFog( qfalse, 1/*0*/, color, current_fog_view_distance );
		return;
	}
 
	if (next_fog_check > level.time)
	{
		return;
	}

	next_fog_check = level.time + 10000;

	if (init)
	{// Map has restarted. Init the fog distance instantly!
		UQ_SetGlobalFog( qfalse, 1/*0*/, color, current_fog_view_distance );
		return;
	}

	choice1 = Q_TrueRand(0, 8);
	choice2 = Q_TrueRand(1,10);
	diff = choice1*64;

	if (choice2 >= 6 && current_fog_view_distance + diff < maximum_fog_view_distance)
	{// Avoid going above maximum fog range...
		current_fog_view_distance = current_fog_view_distance + diff;
		UQ_SetGlobalFog( qfalse, 10000, color, current_fog_view_distance );
	}
	else if (choice2 < 6 && current_fog_view_distance - diff < minimum_fog_view_distance)
	{// Avoid going below minimum fog range...
		current_fog_view_distance = current_fog_view_distance + diff;
		UQ_SetGlobalFog( qfalse, 10000, color, current_fog_view_distance );
	}
	else
	{
		current_fog_view_distance = current_fog_view_distance - diff;
		UQ_SetGlobalFog( qfalse, 10000, color, current_fog_view_distance );
	}

	//G_Printf("Current fog distance is %f.\n", current_fog_view_distance);
}

//
// Unique's climbing out of water!
//

qboolean UQ_Climbable ( vec3_t org, vec3_t angles, int ignoreEnt )
{
	vec3_t		org1, org2, forward, angles2;
	qboolean	canClimb = qfalse;

	VectorCopy(angles, angles2);
	angles2[ROLL] = 0;
	
	VectorCopy(org, org1);
	VectorCopy(org, org2);

	AngleVectors (angles2, forward, NULL, NULL);
	VectorMA( org2, 128, forward, org2 );

	while (org1[2] < org[2]+64)
	{
		trace_t tr;

		org1[2]+=8;
		org2[2]+=8;

		trap_Trace( &tr, org1, NULL, NULL, org2, ignoreEnt, MASK_PLAYERSOLID & ~CONTENTS_BODY );

		//Com_Printf("GAME: Trace from %f %f %f -> %f %f %f --- Fraction is %f.\n", org1[0], org1[1], org1[2], org2[0], org2[1], org2[2], tr.fraction);

		if (tr.fraction == 1 && (tr.entityNum > ENTITYNUM_MAX_NORMAL || tr.entityNum == ENTITYNUM_NONE))
		{// Can climb it!
			canClimb = qtrue;
			break;
		}
	}

	return canClimb;
}

//
// Supremacy gametype editing!
//

int		supremacy_edit_num_flags = 0;
vec3_t	supremacy_edit_flags[1024];
int		supremacy_edit_flags_team[1024];

void SUPREMACY_AddFlag ( vec3_t origin, int team )
{
	VectorCopy(origin, supremacy_edit_flags[supremacy_edit_num_flags]);
	supremacy_edit_flags[supremacy_edit_num_flags][2]-=80;
	supremacy_edit_flags_team[supremacy_edit_num_flags] = team;
	supremacy_edit_num_flags++;
}

int		supremacy_edit_num_vehicles = 0;
vec3_t	supremacy_edit_vehicles_origins[1024];
float	supremacy_edit_vehicles_angle[1024];
int		supremacy_edit_vehicles_type[1024];

void SUPREMACY_AddVehicle ( vec3_t origin, vec3_t angles, int vehicleType )
{
	float angle;

	angle = angles[YAW];

	while (angle > 360)
		angle -= 360;

	while (angle < -360)
		angle += 360;

	// Force 90 degree angles...
	if (angle > -45 && angle < 45)
	{
		angle = 0;
	}
	else if (angle >= 45 && angle <= 135)
	{
		angle = 90;
	}
	else if (angle >= 135 && angle <= 225)
	{
		angle = 180;
	}
	else if (angle >= 225 && angle <= 315)
	{
		angle = 270;
	}
	else if (angle >= 315 && angle <= 405)
	{
		angle = 360;
	}
	else if (angle <= -45 && angle >= -135)
	{
		angle = 90;
	}
	else if (angle <= -135 && angle >= -225)
	{
		angle = 180;
	}
	else if (angle <= -225 && angle >= -315)
	{
		angle = 270;
	}
	else if (angle <= -315 && angle >= -405)
	{
		angle = 360;
	}

	VectorCopy(origin, supremacy_edit_vehicles_origins[supremacy_edit_num_vehicles]);
	supremacy_edit_vehicles_angle[supremacy_edit_num_vehicles] = angle;
	supremacy_edit_vehicles_type[supremacy_edit_num_vehicles] = vehicleType;
	supremacy_edit_num_vehicles++;
}

int		supremacy_edit_num_ammo_crates = 0;
vec3_t	supremacy_edit_ammo_crates_origins[1024];
float	supremacy_edit_ammo_crates_angle[1024];

void SUPREMACY_AddAmmoCrate ( vec3_t origin, vec3_t angles )
{
	float angle;

	angle = angles[YAW];

	while (angle > 360)
		angle -= 360;

	while (angle < -360)
		angle += 360;

	// Force 90 degree angles...
	if (angle > -45 && angle < 45)
	{
		angle = 0;
	}
	else if (angle >= 45 && angle <= 135)
	{
		angle = 90;
	}
	else if (angle >= 135 && angle <= 225)
	{
		angle = 180;
	}
	else if (angle >= 225 && angle <= 315)
	{
		angle = 270;
	}
	else if (angle >= 315 && angle <= 405)
	{
		angle = 360;
	}
	else if (angle <= -45 && angle >= -135)
	{
		angle = 90;
	}
	else if (angle <= -135 && angle >= -225)
	{
		angle = 180;
	}
	else if (angle <= -225 && angle >= -315)
	{
		angle = 270;
	}
	else if (angle <= -315 && angle >= -405)
	{
		angle = 360;
	}

	angle -= 90;

	VectorCopy(origin, supremacy_edit_ammo_crates_origins[supremacy_edit_num_ammo_crates]);
	supremacy_edit_ammo_crates_origins[supremacy_edit_num_ammo_crates][2]-=64;
	supremacy_edit_ammo_crates_angle[supremacy_edit_num_ammo_crates] = angle;
	supremacy_edit_num_ammo_crates++;
}

int		supremacy_edit_num_health_crates = 0;
vec3_t	supremacy_edit_health_crates_origins[1024];
float	supremacy_edit_health_crates_angle[1024];

void SUPREMACY_AddHealthCrate ( vec3_t origin, vec3_t angles )
{
	float angle;

	angle = angles[YAW];

	while (angle > 360)
		angle -= 360;

	while (angle < -360)
		angle += 360;

	// Force 90 degree angles...
	if (angle > -45 && angle < 45)
	{
		angle = 0;
	}
	else if (angle >= 45 && angle <= 135)
	{
		angle = 90;
	}
	else if (angle >= 135 && angle <= 225)
	{
		angle = 180;
	}
	else if (angle >= 225 && angle <= 315)
	{
		angle = 270;
	}
	else if (angle >= 315 && angle <= 405)
	{
		angle = 360;
	}
	else if (angle <= -45 && angle >= -135)
	{
		angle = 90;
	}
	else if (angle <= -135 && angle >= -225)
	{
		angle = 180;
	}
	else if (angle <= -225 && angle >= -315)
	{
		angle = 270;
	}
	else if (angle <= -315 && angle >= -405)
	{
		angle = 360;
	}

	angle -= 90;

	VectorCopy(origin, supremacy_edit_ammo_crates_origins[supremacy_edit_num_health_crates]);
	supremacy_edit_health_crates_origins[supremacy_edit_num_health_crates][2]-=64;
	supremacy_edit_health_crates_angle[supremacy_edit_num_health_crates] = angle;
	supremacy_edit_num_health_crates++;
}

// Repair stuff..
void SUPREMACY_ChangeFlagTeam ( int flag_number, int new_team )
{
	supremacy_edit_flags_team[flag_number] = new_team;
}

float SUPREMACY_FloorHeightAt ( vec3_t org )
{
	trace_t tr;
	vec3_t org1, org2;
//	float height = 0;

	VectorCopy(org, org1);
	org1[2]+=256;

	VectorCopy(org, org2);
	org2[2]= -65536.0f;

	trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_PLAYERSOLID );
	
	if ( (tr.surfaceFlags & SURF_NONSOLID) 
		&& (tr.surfaceFlags & SURF_NOMARKS)
		&& (tr.surfaceFlags & SURF_NOLIGHTMAP) )
	{// Water...
		return tr.endpos[2];
	}

	if ( tr.startsolid || tr.allsolid )
	{
		return -65536.0f;
	}

	return tr.endpos[2];
}

void SUPREMACY_RemoveEntAt ( vec3_t org )
{
	trace_t tr;
	vec3_t org1, org2;
//	float height = 0;

	VectorCopy(org, org1);
	org1[2]+=256;

	VectorCopy(org, org2);
	org2[2]= -65536.0f;

	trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_PLAYERSOLID );
	
	if (tr.entityNum < ENTITYNUM_MAX_NORMAL)
	{
		G_FreeEntity(&g_entities[tr.entityNum]);
	}
}

void SUPREMACY_RaiseAllSupremacyEnts ( int modifier )
{
	int i;

	for (i = 0; i < supremacy_edit_num_flags; i++)
	{
		supremacy_edit_flags[i][2] = SUPREMACY_FloorHeightAt( supremacy_edit_flags[i] );
	}

	for (i = 0; i < supremacy_edit_num_vehicles; i++)
	{
		supremacy_edit_vehicles_origins[i][2] = SUPREMACY_FloorHeightAt( supremacy_edit_flags[i] );
	}

	for (i = 0; i < supremacy_edit_num_ammo_crates; i++)
	{
		supremacy_edit_ammo_crates_origins[i][2] = SUPREMACY_FloorHeightAt( supremacy_edit_flags[i] );
	}

	for (i = 0; i < supremacy_edit_num_health_crates; i++)
	{
		supremacy_edit_health_crates_origins[i][2] = SUPREMACY_FloorHeightAt( supremacy_edit_flags[i] );
	}
}

void SUPREMACY_RemoveAllSupremacyEnts ( void )
{
	int i;

	for (i = 0; i < supremacy_edit_num_flags; i++)
	{
		SUPREMACY_RemoveEntAt( supremacy_edit_flags[i] );
	}

	for (i = 0; i < supremacy_edit_num_vehicles; i++)
	{
		SUPREMACY_RemoveEntAt( supremacy_edit_flags[i] );
	}

	for (i = 0; i < supremacy_edit_num_ammo_crates; i++)
	{
		SUPREMACY_RemoveEntAt( supremacy_edit_flags[i] );
	}

	for (i = 0; i < supremacy_edit_num_health_crates; i++)
	{
		SUPREMACY_RemoveEntAt( supremacy_edit_flags[i] );
	}
}

void SUPREMACY_RemoveAmmoCrate ( void )
{
	supremacy_edit_num_ammo_crates--;
}

void SUPREMACY_RemoveHealthCrate ( void )
{
	supremacy_edit_num_health_crates--;
}

/*
int		supremacy_edit_num_flags = 0;
vec3_t	supremacy_edit_flags[1024];
int		supremacy_edit_flags_team[1024];

int		supremacy_edit_num_vehicles = 0;
vec3_t	supremacy_edit_vehicles_origins[1024];
float	supremacy_edit_vehicles_angle[1024];
int		supremacy_edit_vehicles_type[1024];

int		supremacy_edit_num_ammo_crates = 0;
vec3_t	supremacy_edit_ammo_crates_origins[1024];
float	supremacy_edit_ammo_crates_angle[1024];

int		supremacy_edit_num_health_crates = 0;
vec3_t	supremacy_edit_health_crates_origins[1024];
float	supremacy_edit_health_crates_angle[1024];
*/

void SUPREMACY_ShowInfo ( void )
{
	int				i;

	G_Printf("^3FLAGS:\n");

	for (i = 0; i < supremacy_edit_num_flags; i++)
	{
		if (supremacy_edit_flags_team[i] == TEAM_AXIS)
			G_Printf("^5[^2Flag #^3%i^5] - ^1AXIS\n", i);
		else if (supremacy_edit_flags_team[i] == TEAM_ALLIES)
			G_Printf("^5[^2Flag #^3%i^5] - ^4ALLIES\n", i);
		else
			G_Printf("^5[^2Flag #^3%i^5] - ^7NEUTRAL\n", i);
	}

	G_Printf("^5Total of ^3%i^5 supremacy flags.\n", supremacy_edit_num_flags-1);

#ifdef __VEHICLES__
	G_Printf("^3VEHICLES:\n");

	for (i = 0; i < supremacy_edit_num_vehicles; i++)
	{
		if (supremacy_edit_vehicles_type[i] == VEHICLE_CLASS_LIGHT_TANK)
			G_Printf("^5[^2Vehicle #^3%i^5] - ^1LIGHT TANK\n", i);
		else if (supremacy_edit_vehicles_type[i] == VEHICLE_CLASS_MEDIUM_TANK)
			G_Printf("^5[^2Vehicle #^3%i^5] - ^1MEDIUM TANK\n", i);
		else if (supremacy_edit_vehicles_type[i] == VEHICLE_CLASS_FLAMETANK)
			G_Printf("^5[^2Vehicle #^3%i^5] - ^1FLAMER TANK\n", i);
		else
			G_Printf("^5[^2Vehicle #^3%i^5] - ^1HEAVY TANK\n", i);
	}
#endif //__VEHICLES__

	G_Printf("^5Total of ^3%i^5 supremacy vehicles.\n", supremacy_edit_num_vehicles-1);
	G_Printf("^3AMMO CRATES: ^5Total of ^3%i^5 supremacy ammo crates.\n", supremacy_edit_num_ammo_crates-1);
	G_Printf("^3HEALTH CRATES: ^5Total of ^3%i^5 supremacy health crates.\n", supremacy_edit_num_health_crates-1);
}

void SUPREMACY_SaveGameInfo ( void )
{
	vmCvar_t		mapname;
	char			filename[60];
	fileHandle_t	f;
	int				i;

	strcpy( filename, "supremacy/" );

	trap_Cvar_VariableStringBuffer( "g_scriptName", filename, sizeof(filename) );
	if ( strlen( filename) > 0 )
	{
		trap_Cvar_Register( &mapname, "g_scriptName", "", CVAR_ROM );
	}
	else
	{
		trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );
	}

	Q_strcat( filename, sizeof(filename), mapname.string );

	trap_FS_FOpenFile( va( "supremacy/%s.info", filename), &f, FS_WRITE );

	// Save flags...
	trap_FS_Write( &supremacy_edit_num_flags, sizeof(int), f );

	for (i = 0; i < supremacy_edit_num_flags; i++)
	{
		trap_FS_Write( &supremacy_edit_flags[i], sizeof(vec3_t), f );
		trap_FS_Write( &supremacy_edit_flags_team[i], sizeof(int), f );
	}

	// Save vehicles...
	trap_FS_Write( &supremacy_edit_num_vehicles, sizeof(int), f );

	for (i = 0; i < supremacy_edit_num_vehicles; i++)
	{
		trap_FS_Write( &supremacy_edit_vehicles_origins[i], sizeof(vec3_t), f );
		trap_FS_Write( &supremacy_edit_vehicles_angle[i], sizeof(float), f );
		trap_FS_Write( &supremacy_edit_vehicles_type[i], sizeof(int), f );
	}

	// Save ammo crates...
	trap_FS_Write( &supremacy_edit_num_ammo_crates, sizeof(int), f );

	for (i = 0; i < supremacy_edit_num_ammo_crates; i++)
	{
		trap_FS_Write( &supremacy_edit_ammo_crates_origins[i], sizeof(vec3_t), f );
		trap_FS_Write( &supremacy_edit_ammo_crates_angle[i], sizeof(float), f );
	}

	// Save health crates...
	trap_FS_Write( &supremacy_edit_num_health_crates, sizeof(int), f );

	for (i = 0; i < supremacy_edit_num_health_crates; i++)
	{
		trap_FS_Write( &supremacy_edit_health_crates_origins[i], sizeof(vec3_t), f );
		trap_FS_Write( &supremacy_edit_health_crates_angle[i], sizeof(float), f );
	}

	trap_FS_FCloseFile( f );													//close the file
	G_Printf( "^1*** ^3%s^5: Successfully saved supremacy info file ^7supremacy/%s.info^5.\n", GAME_VERSION, filename );
}

extern vmCvar_t fs_game;

void OM_Flag_Spawn_Loaded ( vec3_t origin, int team );
void SP_ammo_crate_spawn ( gentity_t* ent );
void SP_health_crate_spawn ( gentity_t* ent );
void SP_tank_heavy (gentity_t *self);
void SP_tank_medium (gentity_t *self);
void SP_tank_light (gentity_t *self);
void SP_flametank (gentity_t *self);
void SP_tigertank (gentity_t *self);

void SUPREMACY_LoadGameInfo2 ( void )
{
	vmCvar_t		mapname;
	char			filename[60];
	FILE			*f;
	int				i;

	trap_Cvar_Register( &fs_game, "fs_game", "", CVAR_SERVERINFO | CVAR_ROM );
	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	f = fopen( va( "%s/supremacy/%s.info", fs_game.string, mapname.string), "rb" );
	
	if ( !f )
	{
		f = fopen( va( "supremacy/%s.info", filename), "rb" );

		if ( !f )
		{
			G_Printf( "^1*** ^3WARNING^5: Reading from ^7supremacy/%s.info^3 failed^5!!!\n", mapname.string );
			G_Printf( "^1*** ^3       ^5  No supremacy support found for this map!\n" );
			return;
		}
	}

	// Save flags...
	fread( &supremacy_edit_num_flags, sizeof(int), 1, f);

	for (i = 0; i < supremacy_edit_num_flags; i++)
	{
		fread( &supremacy_edit_flags[i], sizeof(vec3_t), 1, f );
		fread( &supremacy_edit_flags_team[i], sizeof(int), 1, f );

		OM_Flag_Spawn_Loaded( supremacy_edit_flags[i], supremacy_edit_flags_team[i]);
	}

	// Save vehicles...
	fread( &supremacy_edit_num_vehicles, sizeof(int), 1, f );

#ifdef __VEHICLES__
	for (i = 0; i < supremacy_edit_num_vehicles; i++)
	{
		fread( &supremacy_edit_vehicles_origins[i], sizeof(vec3_t), 1, f );
		fread( &supremacy_edit_vehicles_angle[i], sizeof(float), 1, f );
		fread( &supremacy_edit_vehicles_type[i], sizeof(int), 1, f );

		{
			gentity_t *self = G_Spawn();

			G_SetOrigin(self, supremacy_edit_vehicles_origins[i]);
			self->angle = supremacy_edit_vehicles_angle[i];

			switch (supremacy_edit_vehicles_type[i])
			{
			case VEHICLE_CLASS_LIGHT_TANK:
				SP_tank_light(self);
				break;
			case VEHICLE_CLASS_MEDIUM_TANK:
				SP_tank_medium(self);
				break;
			case VEHICLE_CLASS_FLAMETANK:
				SP_flametank(self);
				break;
			case VEHICLE_CLASS_HEAVY_TANK:
				SP_tank_heavy(self);
				break;
			default:		
				SP_tank_light(self);
				break;
			}
		}
	}
#endif //__VEHICLES__

	// Save ammo crates...
	fread( &supremacy_edit_num_ammo_crates, sizeof(int), 1, f );

	for (i = 0; i < supremacy_edit_num_ammo_crates; i++)
	{
		fread( &supremacy_edit_ammo_crates_origins[i], sizeof(vec3_t), 1, f );
		fread( &supremacy_edit_ammo_crates_angle[i], sizeof(float), 1, f );

		{
			gentity_t *self = G_Spawn();

			VectorCopy(supremacy_edit_ammo_crates_origins[i], self->s.origin);
			//G_SetOrigin(self, supremacy_edit_ammo_crates_origins[i]);
			self->angle = supremacy_edit_ammo_crates_angle[i];

			self->state = 1; // Ignore spawn vars...
			SP_ammo_crate_spawn(self);
		}
	}

	// Save health crates...
	fread( &supremacy_edit_num_health_crates, sizeof(int)+1, 1, f );

	for (i = 0; i < supremacy_edit_num_health_crates; i++)
	{
		fread( &supremacy_edit_health_crates_origins[i], sizeof(vec3_t), 1, f );
		fread( &supremacy_edit_health_crates_angle[i], sizeof(float), 1, f );

		{
			gentity_t *self = G_Spawn();

			VectorCopy(supremacy_edit_health_crates_origins[i], self->s.origin);
			//G_SetOrigin(self, supremacy_edit_health_crates_origins[i]);
			self->angle = supremacy_edit_health_crates_angle[i];

			self->state = 1; // Ignore spawn vars...
			SP_health_crate_spawn(self);
		}
	}

	fclose(f);													//close the file
	G_Printf( "^1*** ^3%s^5: Successfully loaded supremacy info file ^7supremacy/%s.info^5.\n", GAME_VERSION, mapname.string );
}

void SUPREMACY_LoadGameInfo ( void )
{
	vmCvar_t		mapname;
//	char			filename[60];
	fileHandle_t	f;
	int				i;

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	trap_FS_FOpenFile( va( "supremacy/%s.info", mapname.string), &f, FS_READ );

	if ( !f )
	{
		SUPREMACY_LoadGameInfo2();
		return;
	}

	// Save flags...
	trap_FS_Read( &supremacy_edit_num_flags, sizeof(int), f );

	for (i = 0; i < supremacy_edit_num_flags; i++)
	{
		trap_FS_Read( &supremacy_edit_flags[i], sizeof(vec3_t), f );
		trap_FS_Read( &supremacy_edit_flags_team[i], sizeof(int), f );

		OM_Flag_Spawn_Loaded( supremacy_edit_flags[i], supremacy_edit_flags_team[i]);

	}

	// Save vehicles...
	trap_FS_Read( &supremacy_edit_num_vehicles, sizeof(int), f );

#ifdef __VEHICLES__
	for (i = 0; i < supremacy_edit_num_vehicles; i++)
	{
		trap_FS_Read( &supremacy_edit_vehicles_origins[i], sizeof(vec3_t), f );
		trap_FS_Read( &supremacy_edit_vehicles_angle[i], sizeof(float), f );
		trap_FS_Read( &supremacy_edit_vehicles_type[i], sizeof(int), f );

		{
			gentity_t *self = G_Spawn();

			G_SetOrigin(self, supremacy_edit_vehicles_origins[i]);
			self->angle = supremacy_edit_vehicles_angle[i];

			switch (supremacy_edit_vehicles_type[i])
			{
			case VEHICLE_CLASS_LIGHT_TANK:
				SP_tank_light(self);
				break;
			case VEHICLE_CLASS_MEDIUM_TANK:
				SP_tank_medium(self);
				break;
			case VEHICLE_CLASS_FLAMETANK:
				SP_flametank(self);
				break;
			case VEHICLE_CLASS_HEAVY_TANK:
				SP_tank_heavy(self);
				break;
			default:		
				SP_tank_light(self);
				break;
			}
		}
	}
#endif //__VEHICLES__

	// Save ammo crates...
	trap_FS_Read( &supremacy_edit_num_ammo_crates, sizeof(int), f );

	for (i = 0; i < supremacy_edit_num_ammo_crates; i++)
	{
		trap_FS_Read( &supremacy_edit_ammo_crates_origins[i], sizeof(vec3_t), f );
		trap_FS_Read( &supremacy_edit_ammo_crates_angle[i], sizeof(float), f );

		{
			gentity_t *self = G_Spawn();

			VectorCopy(supremacy_edit_ammo_crates_origins[i], self->s.origin);
			//G_SetOrigin(self, supremacy_edit_ammo_crates_origins[i]);
			self->angle = supremacy_edit_ammo_crates_angle[i];

			self->state = 1; // Ignore spawn vars...
			SP_ammo_crate_spawn(self);
		}
	}

	// Save health crates...
	trap_FS_Read( &supremacy_edit_num_health_crates, sizeof(int), f );

	for (i = 0; i < supremacy_edit_num_health_crates; i++)
	{
		trap_FS_Read( &supremacy_edit_health_crates_origins[i], sizeof(vec3_t), f );
		trap_FS_Read( &supremacy_edit_health_crates_angle[i], sizeof(float), f );

		{
			gentity_t *self = G_Spawn();

			VectorCopy(supremacy_edit_health_crates_origins[i], self->s.origin);
			//G_SetOrigin(self, supremacy_edit_health_crates_origins[i]);
			self->angle = supremacy_edit_health_crates_angle[i];

			self->state = 1; // Ignore spawn vars...
			SP_health_crate_spawn(self);
		}
	}

	trap_FS_FCloseFile( f );													//close the file
	G_Printf( "^1*** ^3%s^5: Successfully loaded supremacy info file ^7supremacy/%s.info^5.\n", GAME_VERSION, mapname.string );
}

#endif //__ETE__

