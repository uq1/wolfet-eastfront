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

vec3_t	tankTraceMaxs = {64, 64, 78};
vec3_t	tankTraceMins = {-64, -64, -24};

/*
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
MAPPING FUNCTION LISTING
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
AIMOD_MAPPING_FollowPath
AIMOD_MAPPING_GrapFired
AIMOD_MAPPING_CheckForLadder
AIMOD_MAPPING_PathMap
AIMOD_MAPPING_InitNodes
AIMOD_MAPPING_ShowNode
AIMOD_MAPPING_DrawPath
AIMOD_MAPPING_ShowPath
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
*/

/*
    _____                       .__                
   /     \ _____  ______ ______ |__| ____    ____  
  /  \ /  \\__  \ \____ \\____ \|  |/    \  / ___\ 
 /    Y    \/ __ \|  |_> >  |_> >  |   |  \/ /_/  >
 \____|__  (____  /   __/|   __/|__|___|  /\___  / 
         \/     \/|__|   |__|           \//_____/  

    uniquebot_mapping.c 
    Mapping routines for the FUSION bot.
================================================================
Should you decide to release a modified version of FUSIO*/
#include "../game/g_local.h"

//#include "uniquebot.h"
// Total number of nodes that are items
//-------------------------------------------------------
int				number_of_itemnodes;

// Total number of nodes
//-------------------------------------------------------
extern int		number_of_nodes;
int				number_conn = 0;

// For debugging paths
//-------------------------------------------------------
int				show_path_from = -1;
int				show_path_to = -1;

// For adding, editing paths etc
qboolean		touch_connections = qfalse;
qboolean		relocate_node = qfalse;

// array for node data
//-------------------------------------------------------
int				current_node = -1;			// Holds the currently selected node globally
//node_t			nodes[MAX_NODES];
#ifdef __CUDA__
//__device__ node_t	*nodes;
__shared__ node_t	*nodes;
#else //!__CUDA__
node_t				*nodes;
#endif //__CUDA__

gentity_t		*visible_nodes[MAX_NODES];	// These are 'visible' node entities
gentity_t		*connections[500];			// That's an awful lot, but hey :)
int				enodecount = 0;
enode_t			enodes[200];
extern botai_t	botai;
void			AIMOD_MAPPING_TouchNode ( gentity_t *ent, gentity_t *other, trace_t *trace );
void			AIMOD_MAPPING_CreateVisibleNode ( int i );
//void			AIMOD_MAPPING_CalculateVisibleNodes ( void );
void                  AIMOD_MAPPING_CalculateVisibleNodes ( gentity_t *ent );
void			BotWaypointRender ( void );
extern float VectorDistanceNoHeight ( vec3_t v1, vec3_t v2 );
extern float HeightDistance ( vec3_t v1, vec3_t v2 );
extern int TankNodeVisible ( vec3_t org1, vec3_t org2, vec3_t mins, vec3_t maxs, int ignore );

// Node initialization stuff
void
AIMOD_MAPPING_InitMapNodes ( void )
{
	int i, j;
	number_of_nodes = 0;
	shownodes = qfalse;
	for ( i = 0; i < MAX_NODES; i++ )
	{
		nodes[i].enodenum = INVALID;
		for ( j = 0; j < MAX_NODELINKS; j++ )
		{
			nodes[i].links[j].cost = 999999;	// too high
			nodes[i].links[j].targetNode = INVALID;
		}
	}

	// Initialize the botai global struct
	botai.ctf = qtrue;							// AVD set this in loadnodes or something
	for ( i = 0; i < TEAM_NUM_TEAMS; i++ )
	{
		botai.flagstate[i].dropped = qfalse;
		botai.flagstate[i].valid = qtrue;
		botai.flagstate[i].carrier = INVALID;
		botai.flagstate[i].node = INVALID;
		botai.flagstate[i].entityNum = INVALID;
	}

	for ( i = 0; i < 200; i++ )
	{
		enodes[i].link_node = INVALID;
		enodes[i].num_routes = 0;
	}

	for ( i = 0; i < MAX_NODES; i++ )
	{
		visible_nodes[i] = NULL;
	}

	for ( i = 0; i < 500; i++ )
	{
		connections[i] = NULL;
	}
}

#ifdef __OLD__
extern vec3_t	tankTraceMaxs;
extern vec3_t	tankTraceMins;

//vec3_t	playerMins = {-18, -18, -24};
//vec3_t	playerMaxs = {18, 18, 48};
//vec3_t	botTraceMins = {-32, -32, -1};
//vec3_t	botTraceMaxs = {32, 32, 32};
//vec3_t	botTraceMins = {-24, -24, -1};
//vec3_t	botTraceMaxs = {24, 24, 32};
vec3_t			botTraceMins = { -20, -20, -1 };
vec3_t			botTraceMaxs = { 20, 20, 32 };


//extern int OrgVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore);
//special node visibility check for bot linkages..
//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.

//3 = team door entity in the way.
int
NodeVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg, newOrg2;
	vec3_t	mins, maxs;

	//vec3_t			botTraceMins = { -20, -20, -1 };
	//vec3_t			botTraceMaxs = { 20, 20, 32 };

	vec3_t	traceMins = { -10, -10, -1 };
	vec3_t	traceMaxs = { 10, 10, 15 };

	VectorCopy( traceMins, mins );
	VectorCopy( traceMaxs, maxs );
	VectorCopy( org2, newOrg );
	VectorCopy( org1, newOrg2 );

	//VectorCopy(org1, newOrg);
	//	newOrg[2]+=16; // Look from up a little...
	//newOrg[2] += 4; // Look from up a little...
	newOrg[2] += 16; // Look from up a little...
	newOrg2[2] += 16; // Look from up a little...

	//newOrg[2]+=32; // Look from up a little...
	//trap_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
	trap_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA /*MASK_WATER*/ );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	if
	(
		tr.fraction != 1 &&
		tr.entityNum != ENTITYNUM_NONE &&
		(
			!strcmp( g_entities[tr.entityNum].classname, "func_door") || !strcmp( g_entities[tr.entityNum].classname,
																				  "func_door_rotating")
		)
	)
	{				// A door in the way.. See if we have access...
		if ( !g_entities[tr.entityNum].allowteams )
		{
			return ( 2 );
		}

		return ( 3 );
	}

	return ( 0 );
}


/* */
int
NodeVisible2 ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg;
	vec3_t	mins, maxs;

	//VectorSet(mins, 0, 0, 0);
	//VectorSet(maxs, 16, 16, 32);
	VectorCopy( botTraceMins, mins );
	VectorCopy( botTraceMaxs, maxs );
	VectorCopy( org2, newOrg );

	//VectorCopy(org1, newOrg);
	//	newOrg[2]+=16; // Look from up a little...
	//	newOrg[2]+=8; // Look from up a little...
	newOrg[2] += 4; // Look from up a little...

	//trap_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
	trap_Trace( &tr, org1, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	if
	(
		tr.fraction != 1 &&
		tr.entityNum != ENTITYNUM_NONE &&
		(
			!strcmp( g_entities[tr.entityNum].classname, "func_door") || !strcmp( g_entities[tr.entityNum].classname,
																				  "func_door_rotating")
		)
	)
	{				// A door in the way.. See if we have access...
		if ( !g_entities[tr.entityNum].allowteams )
		{
			return ( 2 );
		}

		return ( 3 );
	}

	return ( 0 );
}


//extern int OrgVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore);
//special node visibility check for bot linkages..
//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.

//3 = team door entity in the way.
int
TankNodeVisible ( vec3_t org1, vec3_t org2, vec3_t mins, vec3_t maxs, int ignore )
{
	trace_t tr;
	vec3_t	newOrg, newOrg2;
	VectorCopy( org1, newOrg2 );
	VectorCopy( org2, newOrg );

	//VectorCopy(org1, newOrg);
	//newOrg[2]+=16; // Look from up a little...
	newOrg[2] += 4;		// Look from up a little...

	//	newOrg2[2]+=16; // Look from up a little...
	newOrg2[2] += 4;	// Look from up a little...

	//trap_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
	trap_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA );
	if ( tr.fraction == 1 )
	{

		//trap_Trace(&tr, newOrg, NULL, NULL, org2, ignore, MASK_SHOT/*MASK_PLAYERSOLID*/);

		/*		if (tr.fraction != 1 && tr.entityNum != ENTITYNUM_NONE 
			&& (!strcmp(g_entities[tr.entityNum].classname, "func_door") || !strcmp(g_entities[tr.entityNum].classname, "func_door_rotating")) )
		{// A door in the way.. See if we have access...
			if (!g_entities[tr.entityNum].allowteams)
				return 2;

			return 3;
		}*/
		return ( 1 );
	}

	if
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
	}

	return ( 0 );
}
#else //!__OLD__
//vec3_t	playerMins = {-18, -18, -24};
//vec3_t	playerMaxs = {18, 18, 48};
//vec3_t	botTraceMins = {-32, -32, -1};
//vec3_t	botTraceMaxs = {32, 32, 32};
//vec3_t	botTraceMins = {-24, -24, -1};
//vec3_t	botTraceMaxs = {24, 24, 32};
vec3_t			botTraceMins = { -20, -20, -1 };
vec3_t			botTraceMaxs = { 20, 20, 32 };

//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.

//3 = team door entity in the way.
int
NodeVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg, newOrg2;
	vec3_t	mins, maxs;

	vec3_t			traceMins = { -10, -10, -1 };
	vec3_t			traceMaxs = { 10, 10, 48/*32*/ };

	VectorCopy( traceMins, mins );
	VectorCopy( traceMaxs, maxs );
	VectorCopy( org2, newOrg );
	VectorCopy( org1, newOrg2 );

	newOrg[2] += 8; // Look from up a little...
	newOrg2[2] += 8; // Look from up a little...

	trap_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	

	if (tr.startsolid || tr.allsolid)
		return ( 0 );

	if ( tr.fraction == 1 && !(tr.contents & CONTENTS_LAVA))
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		trap_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( tr2.fraction == 1 && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if (tr.surfaceFlags & SURF_LADDER)
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		trap_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( (tr2.fraction == 1 || (tr2.surfaceFlags & SURF_LADDER)) && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if ( tr.fraction != 1 
		&& tr.entityNum != ENTITYNUM_NONE 
		&& tr.entityNum < ENTITYNUM_MAX_NORMAL )
	{
		switch (g_entities[tr.entityNum].s.eType)
		{
		case ET_MOVER:
		case ET_ITEM:
		case ET_PORTAL:
		case ET_TELEPORT_TRIGGER:
		case ET_INVISIBLE:
		case ET_OID_TRIGGER:
		case ET_EXPLOSIVE_INDICATOR:
		case ET_EXPLOSIVE:
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
		case ET_HEALER:
		case ET_SUPPLIER:
		case ET_COMMANDMAP_MARKER:
		case ET_WOLF_OBJECTIVE:
		case ET_SECONDARY:
		case ET_FLAG:
		case ET_ASSOCIATED_SPAWNAREA:
		case ET_UNASSOCIATED_SPAWNAREA:
		case ET_CONSTRUCTIBLE_SANDBAGS:
		case ET_CONSTRUCTIBLE_MG_NEST:
#ifdef __VEHICLES__
		case ET_VEHICLE:
#endif //__VEHICLES__
		case ET_PARTICLE_SYSTEM:
			{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
				trace_t tr2;
				trap_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

				if ( tr2.fraction == 1 && !(tr2.contents & CONTENTS_LAVA) )
				{
					return ( 4 );
				}
			}
			break;
		default:
			break;
		}
	}

	return ( 0 );
}

int
NodeVisibleJump ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg, newOrg2;
	vec3_t	mins, maxs;

	vec3_t			traceMins = { -10, -10, -1 };
	vec3_t			traceMaxs = { 10, 10, 48/*32*/ };

	VectorCopy( traceMins, mins );
	VectorCopy( traceMaxs, maxs );
	VectorCopy( org2, newOrg );
	VectorCopy( org1, newOrg2 );

	newOrg[2] += 16; // Look from up a little...
	newOrg2[2] += 16; // Look from up a little...

	trap_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	
	if (tr.startsolid || tr.allsolid)
		return ( 0 );

	if ( tr.fraction == 1 && !(tr.contents & CONTENTS_LAVA))
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		trap_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( tr2.fraction == 1 && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if (tr.surfaceFlags & SURF_LADDER)
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		trap_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( (tr2.fraction == 1 || (tr2.surfaceFlags & SURF_LADDER)) && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if ( tr.fraction != 1 
		&& tr.entityNum != ENTITYNUM_NONE 
		&& tr.entityNum < ENTITYNUM_MAX_NORMAL )
	{
		switch (g_entities[tr.entityNum].s.eType)
		{
		case ET_MOVER:
		case ET_ITEM:
		case ET_PORTAL:
		case ET_TELEPORT_TRIGGER:
		case ET_INVISIBLE:
		case ET_OID_TRIGGER:
		case ET_EXPLOSIVE_INDICATOR:
		case ET_EXPLOSIVE:
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
		case ET_HEALER:
		case ET_SUPPLIER:
		case ET_COMMANDMAP_MARKER:
		case ET_WOLF_OBJECTIVE:
		case ET_SECONDARY:
		case ET_FLAG:
		case ET_ASSOCIATED_SPAWNAREA:
		case ET_UNASSOCIATED_SPAWNAREA:
		case ET_CONSTRUCTIBLE_SANDBAGS:
		case ET_CONSTRUCTIBLE_MG_NEST:
#ifdef __VEHICLES__
		case ET_VEHICLE:
#endif //__VEHICLES__
		case ET_PARTICLE_SYSTEM:
			{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
				trace_t tr2;
				trap_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

				if ( tr2.fraction == 1 && !(tr2.contents & CONTENTS_LAVA) )
				{
					return ( 4 );
				}
			}
			break;
		default:
			break;
		}
	}

	return ( 0 );
}

int
NodeVisibleCrouch ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg, newOrg2;
	vec3_t	mins, maxs;

	vec3_t			traceMins = { -10, -10, -1 };
	vec3_t			traceMaxs = { 10, 10, 32/*16*/ };

	VectorCopy( traceMins, mins );
	VectorCopy( traceMaxs, maxs );
	VectorCopy( org2, newOrg );
	VectorCopy( org1, newOrg2 );

	newOrg[2] += 1; // Look from up a little...
	newOrg2[2] += 1; // Look from up a little...

	trap_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	
	if (tr.startsolid || tr.allsolid)
		return ( 0 );

	if ( tr.fraction == 1 && !(tr.contents & CONTENTS_LAVA))
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		trap_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( tr2.fraction == 1 && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if (tr.surfaceFlags & SURF_LADDER)
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		trap_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( (tr2.fraction == 1 || (tr2.surfaceFlags & SURF_LADDER)) && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if ( tr.fraction != 1 
		&& tr.entityNum != ENTITYNUM_NONE 
		&& tr.entityNum < ENTITYNUM_MAX_NORMAL )
	{
		switch (g_entities[tr.entityNum].s.eType)
		{
		case ET_MOVER:
		case ET_ITEM:
		case ET_PORTAL:
		case ET_TELEPORT_TRIGGER:
		case ET_INVISIBLE:
		case ET_OID_TRIGGER:
		case ET_EXPLOSIVE_INDICATOR:
		case ET_EXPLOSIVE:
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
		case ET_HEALER:
		case ET_SUPPLIER:
		case ET_COMMANDMAP_MARKER:
		case ET_WOLF_OBJECTIVE:
		case ET_SECONDARY:
		case ET_FLAG:
		case ET_ASSOCIATED_SPAWNAREA:
		case ET_UNASSOCIATED_SPAWNAREA:
		case ET_CONSTRUCTIBLE_SANDBAGS:
		case ET_CONSTRUCTIBLE_MG_NEST:
#ifdef __VEHICLES__
		case ET_VEHICLE:
#endif //__VEHICLES__
		case ET_PARTICLE_SYSTEM:
			{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
				trace_t tr2;
				trap_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

				if ( tr2.fraction == 1 && !(tr2.contents & CONTENTS_LAVA) )
				{
					return ( 4 );
				}
			}
			break;
		default:
			break;
		}
	}

	return ( 0 );
}

float FloorNormalAt ( vec3_t org )
{// UQ1: improved it again, now checks slope here in a faster way that also stops multiple checks later! :) Also add mover check :)
	trace_t tr;
	vec3_t org1, org2, slopeangles;
	float pitch = 0;

	VectorCopy(org, org1);
	org1[2]+=48;

	VectorCopy(org, org2);
	org2[2]= -65536.0f;

	trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_PLAYERSOLID );

	// Added -- Check slope...
	vectoangles( tr.plane.normal, slopeangles );

	pitch = slopeangles[0];
	
	if (pitch > 180)
		pitch -= 360;

	if (pitch < -180)
		pitch += 360;

	pitch += 90.0f;

	return pitch;
}

//extern int OrgVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore);
//special node visibility check for bot linkages..
//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.
//3 = team door entity in the way.
int
TankNodeVisible ( vec3_t org1, vec3_t org2, vec3_t mins, vec3_t maxs, int ignore )
{
	trace_t tr;
	vec3_t	newOrg, newOrg2;
	float slope;

	VectorCopy( org1, newOrg );
	VectorCopy( org2, newOrg2 );

	slope = FloorNormalAt(newOrg2);

	if (slope > 40 || slope < -40)
	{// Extreme slope!
		newOrg[2] += 72;		// Look from up a little...
		newOrg2[2] += 72;	// Look from up a little...
	}
	else if (slope > 36 || slope < -36)
	{// Very heavy slope!
		newOrg[2] += 64;		// Look from up a little...
		newOrg2[2] += 64;	// Look from up a little...
	}
	else if (slope > 30 || slope < -30)
	{// Heavy slope!
		newOrg[2] += 56;		// Look from up a little...
		newOrg2[2] += 56;	// Look from up a little...
	}
	else if (slope > 22 || slope < -22)
	{// Mid->Heigh range slope...
		newOrg[2] += 48;		// Look from up a little...
		newOrg2[2] += 48;	// Look from up a little...
	}
	else if (slope > 16 || slope < -16)
	{// Mid range slope...
		newOrg[2] += 32;		// Look from up a little...
		newOrg2[2] += 32;	// Look from up a little...
	}
	else if (slope > 8 || slope < -8)
	{// Slight slope...
		newOrg[2] += 28;		// Look from up a little...
		newOrg2[2] += 28;	// Look from up a little...
	}
	else if (slope > 4 || slope < -4)
	{// Slight slope...
		newOrg[2] += 24;		// Look from up a little...
		newOrg2[2] += 24;	// Look from up a little...
	}
	else
	{// Basicly flat!
		newOrg[2] += 12;		// Look from up a little...
		newOrg2[2] += 12;	// Look from up a little...
	}

	trap_Trace( &tr, newOrg, mins, maxs, newOrg2, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	
	if ( tr.startsolid )
	{
		return ( 0 );
	}

	if ( tr.fraction == 1 && !(tr.contents & CONTENTS_LAVA) )
	{
		return ( 1 );
	}

	return ( 0 );
}
#endif //__OLD__

extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );
int				upto_node = 0;
int				upto_link = 0;

//int last_upto_node = -1;
int				link_check_start_time = 0;

#define __REALTIME_NODELINK_CHECKS__

// UQ1: Spread this across many frames! Checks one link each frame...
void
AIMOD_MAPPING_Realtime_NodeLinks_Checking ( void )
{									// Call this each frame...
#ifdef __REALTIME_NODELINK_CHECKS__
	vec3_t	tmp;
	int		visCheck;
	int		i;
	vec3_t	tankMinsSize, tankMaxsSize;
	VectorCopy( tankTraceMins, tankMinsSize );
	VectorCopy( tankTraceMaxs, tankMaxsSize );
	if ( number_of_nodes <= 0 )
	{
		return;
	}

	/*	if (last_upto_node != upto_node)
	{
		G_Printf("Realtime_NodeLinks_Checking: Up to node %i.\n", upto_node);
		last_upto_node = upto_node;
	}*/
	if ( upto_node == 0 && upto_link == 0 )
	{
		link_check_start_time = trap_Milliseconds();
	}

#ifdef __VEHICLES__
	for ( i = 0; i < 2; i++ )
#else //!__VEHICLES__
	for ( i = 0; i < 5; i++ )
#endif //__VEHICLES__
	{
#ifdef __VEHICLES__
		if ( nodes[i].type & NODE_LAND_VEHICLE )
		{
			continue;
		}
#endif //__VEHICLES__
		upto_link++;
		if ( upto_link >= nodes[upto_node].enodenum || nodes[upto_node].links[upto_link].targetNode < 0 )
		{
			upto_link = 0;
			upto_node++;
			if ( upto_node > number_of_nodes )
			{
				upto_node = 0;	// Back to the start next frame :)

				if (bot_debug.integer)
				{
					int cur_time = trap_Milliseconds();
					G_Printf( "^2*** ^3BOT DEBUG^5: All node links checked in ^3%f^5 seconds.\n", (float)((cur_time - link_check_start_time)/1000) );
					link_check_start_time = cur_time;
				}
			}
		}

		VectorCopy( nodes[nodes[upto_node].links[upto_link].targetNode].origin, tmp );
		if ( BAD_WP_Height( nodes[upto_node].origin, tmp) )
		{
			nodes[upto_node].links[upto_link].flags |= PATH_BLOCKED;
			nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
			return;
		}

		visCheck = NodeVisible( nodes[upto_node].origin, tmp, -1 );

		//0 = wall in way
		//1 = player or no obstruction
		//2 = useable door in the way.
		//3 = door entity in the way.
		if ( visCheck == 1 || visCheck == 2 || visCheck == 3 || upto_node == i + 1 )
		{						// Is OK!
			//			if (nodes[upto_node].links[upto_link].flags & PATH_BLOCKED)
			//			{
			nodes[upto_node].links[upto_link].flags &= ~PATH_BLOCKED;

			//			}
			//			else if (nodes[upto_node].links[upto_link].flags & PATH_NOTANKS)
			//			{
			//				nodes[upto_node].links[upto_link].flags = 0;
			//				nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
			//			}
			//			else
			//			{
			//				nodes[upto_node].links[upto_link].flags = 0;
			//			}
		}
		else
		{						// Is bad currently! Mark it!
			nodes[upto_node].links[upto_link].flags |= PATH_BLOCKED;
		}

/*#ifdef __VEHICLES__
		visCheck = TankNodeVisible(nodes[upto_node].origin, tmp, tankMinsSize, tankMaxsSize, -1);

		if ( visCheck == 1 )
		{// Is OK!
			nodes[upto_node].links[upto_link].flags &= ~PATH_NOTANKS;
		}
		else
		{// Is bad currently! Mark it!
			nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
		}
#endif //__VEHICLES__*/
	}
#endif //__REALTIME_NODELINK_CHECKS__
}


// UQ1: Run on startup... Set initial blocking...
void
AIMOD_MAPPING_Initial_NodeLinks_Checking ( void )
{
	vec3_t	tmp;
	int		visCheck;
	vec3_t	tankMinsSize, tankMaxsSize;
	VectorCopy( tankTraceMins, tankMinsSize );
	VectorCopy( tankTraceMaxs, tankMaxsSize );
	if ( number_of_nodes <= 0 )
	{
		return;
	}

	G_Printf( "^1*** ^3%s^5: Running initial node link checks!\n", GAME_VERSION );
	while ( 1 )
	{
#ifdef __VEHICLES__
		if ( nodes[upto_node].type & NODE_LAND_VEHICLE )
		{
			upto_link = 0;
			upto_node++;
			if ( upto_node > number_of_nodes )
			{
				upto_node = 0;
				break;	// DONE!
			}
		}
#endif //__VEHICLES__
		upto_link++;
		if ( upto_link >= nodes[upto_node].enodenum || nodes[upto_node].links[upto_link].targetNode < 0 )
		{
			upto_link = 0;
			upto_node++;
			if ( upto_node > number_of_nodes )
			{
				upto_node = 0;
				break;	// DONE!
			}
		}

		VectorCopy( nodes[nodes[upto_node].links[upto_link].targetNode].origin, tmp );
		if ( BAD_WP_Height( nodes[upto_node].origin, tmp) )
		{
			nodes[upto_node].links[upto_link].flags |= PATH_BLOCKED;
			nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
			return;
		}

		visCheck = NodeVisible( nodes[upto_node].origin, tmp, -1 );

		//0 = wall in way
		//1 = player or no obstruction
		//2 = useable door in the way.
		//3 = door entity in the way.
		if
		(
			visCheck == 1 ||
			visCheck == 2 ||
			visCheck == 3 ||
			upto_node == nodes[upto_node].links[upto_link].targetNode - 1
		)
		{				// Is OK!
			//			if (nodes[upto_node].links[upto_link].flags & PATH_BLOCKED)
			//			{
			nodes[upto_node].links[upto_link].flags &= ~PATH_BLOCKED;

			//			}
			//			else if (nodes[upto_node].links[upto_link].flags & PATH_NOTANKS)
			//			{
			//				nodes[upto_node].links[upto_link].flags = 0;
			//				nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
			//			}
			//			else
			//			{
			//				nodes[upto_node].links[upto_link].flags = 0;
			//			}
		}
		else
		{				// Is bad currently! Mark it!
			nodes[upto_node].links[upto_link].flags |= PATH_BLOCKED;
		}

		/*#ifdef __VEHICLES__
		visCheck = TankNodeVisible(nodes[upto_node].origin, tmp, tankMinsSize, tankMaxsSize, -1);

		if ( visCheck == 1)
		{// Is OK!
			nodes[upto_node].links[upto_link].flags &= ~PATH_NOTANKS;
		}
		else
		{// Is bad currently! Mark it!
			nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
		}
#endif //__VEHICLES__*/
	}

	G_Printf( "^1*** ^3%s^5: Initial node link checks completed!\n", GAME_VERSION );
}

float RoofHeightAt ( vec3_t org )
{
	trace_t tr;
	vec3_t org1, org2;
//	float height = 0;

	VectorCopy(org, org1);
	org1[2]+=4;

	VectorCopy(org, org2);
	org2[2]= 65536.0f;

	trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_ALL);
	
	if ( tr.startsolid || tr.allsolid )
	{
		return -65536.0f;
	}

	return tr.endpos[2];
}

qboolean AIMod_Mapping_Check_Slope_Between ( vec3_t org1, vec3_t org2 ) {  
	vec3_t	orgA, orgB;
	trace_t tr;
	vec3_t	forward, right, up, dir;
	vec3_t	testangles;

	VectorCopy(org1, orgA);
	VectorSubtract(org2, org1, dir);
	vectoangles(dir, testangles);
	AngleVectors( testangles, forward, right, up );
	VectorMA(orgA, (VectorDistanceNoHeight(org1, org2)*0.25), forward, orgA);
	
	orgA[2] += 16;

	VectorCopy(orgA, orgB);
	orgB[2] -= 256;//-65000;
	
	trap_Trace( &tr, orgA, NULL, NULL, orgB, -1, MASK_PLAYERSOLID );
	
	if (tr.endpos[2]+56 < org1[2])
		return qfalse;

	if ( tr.fraction == 1 || (tr.contents & CONTENTS_LAVA) )
		return qfalse;

	VectorCopy(org1, orgA);
	VectorSubtract(org2, org1, dir);
	vectoangles(dir, testangles);
	AngleVectors( testangles, forward, right, up );
	VectorMA(orgA, (VectorDistanceNoHeight(org1, org2)*0.5), forward, orgA);
	
	orgA[2] += 16;

	VectorCopy(orgA, orgB);
	orgB[2] -= 256;//-65000;
	
	trap_Trace( &tr, orgA, NULL, NULL, orgB, -1, MASK_PLAYERSOLID );
	
	if (tr.endpos[2]+56 < org1[2])
		return qfalse;

	if ( tr.fraction == 1 || (tr.contents & CONTENTS_LAVA) )
		return qfalse;

	VectorCopy(org1, orgA);
	VectorSubtract(org2, org1, dir);
	vectoangles(dir, testangles);
	AngleVectors( testangles, forward, right, up );
	VectorMA(orgA, (VectorDistanceNoHeight(org1, org2)*0.75), forward, orgA);
	
	orgA[2] += 16;

	VectorCopy(orgA, orgB);
	orgB[2] -= 256;//-65000;
	
	trap_Trace( &tr, orgA, NULL, NULL, orgB, -1, MASK_PLAYERSOLID );
	
	if (tr.endpos[2]+56 < org1[2])
		return qfalse;

	if ( tr.fraction == 1 || (tr.contents & CONTENTS_LAVA) )
		return qfalse;

	return qtrue;
}

qboolean NodeIsAlreadyALink ( int start_node, int end_node, int num_links )
{
	int i = 0;

	for (i = 0; i < num_links; i++)
	{
		if (nodes[start_node].links[i].targetNode == end_node)
			return qtrue;
	}

	return qfalse;
}

qboolean
AIMOD_MAPPING_InFOV ( vec3_t spot, vec3_t from, vec3_t fromAngles, int hFOV, int vFOV )
{
	vec3_t	deltaVector, angles, deltaAngles;
	VectorSubtract( spot, from, deltaVector );
	vectoangles( deltaVector, angles );
	deltaAngles[PITCH] = AngleDelta( fromAngles[PITCH], angles[PITCH] );
	deltaAngles[YAW] = AngleDelta( fromAngles[YAW], angles[YAW] );
	
	if ( fabs( deltaAngles[PITCH]) <= vFOV && fabs( deltaAngles[YAW]) <= hFOV )
	{
		return ( qtrue );
	}

	return ( qfalse );
}

#define MAX_NODE_LINK_DISTANCE 256
#define MAX_LADDER_NODE_LINK_DISTANCE 512//96
#define MAX_TANK_NODE_LINK_DISTANCE 512

int
AIMOD_MAPPING_CreateNodeLinks ( int node )
{
	int		loop = 0;
	int		current_angle = 0;
	vec3_t	angles;
	int		linknum = 0;
	vec3_t	tankMaxsSize = {64, 64, 0};
	vec3_t	tankMinsSize = {-64, -64, 0};
	int		best_node = -1;
	int		best_flags = 0;
	int		best_dist = 1024.0f;
	int		in_range_nodes[MAX_NODES];
	int		in_range_flags[MAX_NODES];
	int		num_in_range_nodes = 0;

#ifdef __VEHICLES__
	// First add tank links...
	if (nodes[node].type & NODE_LAND_VEHICLE)
	{
		num_in_range_nodes = 0;
		best_node = -1;
		best_flags = 0;
		best_dist = MAX_TANK_NODE_LINK_DISTANCE + 0.1f;

		angles[PITCH] = 0;
		angles[YAW] = current_angle;
		angles[ROLL] = 0;

		// Find the best (closest) node on each angle...
		for ( loop = 0; loop < number_of_nodes; loop++ )
		{
			int visCheck = -1;
			float dist = VectorDistance(nodes[loop].origin, nodes[node].origin);

			if (dist > MAX_TANK_NODE_LINK_DISTANCE)
			{
				continue;
			}

			if (!(nodes[loop].type & NODE_LAND_VEHICLE))
			{
				continue;
			}

			if (nodes[loop].type & NODE_WATER)
			{// Vehicles never go in water!
				continue;
			}

			if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
			{
				continue;
			}

			visCheck = TankNodeVisible( nodes[loop].origin, nodes[node].origin, tankMinsSize, tankMaxsSize, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					in_range_nodes[num_in_range_nodes] = loop;
					num_in_range_nodes++;
				}
			}
		}

		for ( current_angle = 0; current_angle <= 360; current_angle+=45 ) // 8 angles tested, 45 degrees each...
		{
			int second_best_node = -1;
			int third_best_node = -1;
			best_node = -1;
			best_flags = 0;
			best_dist = MAX_TANK_NODE_LINK_DISTANCE + 0.1f;

			angles[PITCH] = 0;
			angles[YAW] = current_angle;
			angles[ROLL] = 0;

			for ( loop = 0; loop < num_in_range_nodes; loop++ )
			{
				float dist = VectorDistance(nodes[in_range_nodes[loop]].origin, nodes[node].origin);

				if (dist > MAX_TANK_NODE_LINK_DISTANCE)
				{
					continue;
				}

				if ( linknum >= MAX_NODELINKS-2 )
				{
					break;
				}

				if (NodeIsAlreadyALink(node, in_range_nodes[loop], linknum))
				{
					continue;
				}

				if (dist < best_dist)
				{
					if (!AIMOD_MAPPING_InFOV(nodes[in_range_nodes[loop]].origin, nodes[node].origin, angles, 45, 170))
					{
						continue;
					}
				
					best_dist = dist;
					third_best_node = second_best_node;
					second_best_node = best_node;
					best_node = in_range_nodes[loop];
				}
			}

			if (best_node != -1)
			{
				nodes[node].links[linknum].targetNode = best_node;
				nodes[node].links[linknum].cost = (VectorDistance( nodes[best_node].origin, nodes[node].origin ) + (HeightDistance(nodes[best_node].origin, nodes[node].origin)) * 4);

				if ((nodes[best_node].type & NODE_ICE) || (nodes[best_node].type & NODE_WATER))
					nodes[node].links[linknum].cost = 65000.0f;

				linknum++;
			}

			if (second_best_node)
			{// Add second best for vehicle waypoints as well!
				nodes[node].links[linknum].targetNode = second_best_node;
				nodes[node].links[linknum].cost = (VectorDistance( nodes[second_best_node].origin, nodes[node].origin ) + (HeightDistance(nodes[second_best_node].origin, nodes[node].origin)) * 4);

				if ((nodes[second_best_node].type & NODE_ICE) || (nodes[second_best_node].type & NODE_WATER))
					nodes[node].links[linknum].cost = 65000.0f;

				linknum++;
			}

			if (third_best_node)
			{// Add second best for vehicle waypoints as well!
				nodes[node].links[linknum].targetNode = third_best_node;
				nodes[node].links[linknum].cost = (VectorDistance( nodes[third_best_node].origin, nodes[node].origin ) + (HeightDistance(nodes[second_best_node].origin, nodes[node].origin)) * 4);

				if ((nodes[third_best_node].type & NODE_ICE) || (nodes[third_best_node].type & NODE_WATER))
					nodes[node].links[linknum].cost = 65000.0f;

				linknum++;
			}
		}
	}
#endif //__VEHICLES__

	// Now add normal links...
	num_in_range_nodes = 0;
	best_node = -1;
	best_flags = 0;
	best_dist = MAX_NODE_LINK_DISTANCE + 0.1f;

	// Find the best (closest) node on each angle...
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		int visCheck = -1;
		float dist = VectorDistance(nodes[loop].origin, nodes[node].origin);

		if (dist > MAX_NODE_LINK_DISTANCE)
		{
			continue;
		}

		if (!(nodes[node].type & NODE_LAND_VEHICLE) && (nodes[loop].type & NODE_LAND_VEHICLE))
		{
			continue;
		}

		if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
		{
			continue;
		}

		visCheck = NodeVisible( nodes[loop].origin, nodes[node].origin, -1 );

		//0 = wall in way
		//1 = player or no obstruction
		//2 = useable door in the way.
		//3 = door entity in the way.
		if ( visCheck == 1 )
		{
			if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
			{
				in_range_nodes[num_in_range_nodes] = loop;
				num_in_range_nodes++;
			}
		}
		else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
		{
			if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
			{
				in_range_nodes[num_in_range_nodes] = loop;
				in_range_flags[num_in_range_nodes] = 0;
				num_in_range_nodes++;
			}
		}
		else
		{// Look for jump node links...
			visCheck = NodeVisibleJump( nodes[loop].origin, nodes[node].origin, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					in_range_nodes[num_in_range_nodes] = loop;
					in_range_flags[num_in_range_nodes] = PATH_JUMP;
					num_in_range_nodes++;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					in_range_nodes[num_in_range_nodes] = loop;
					in_range_flags[num_in_range_nodes] = PATH_JUMP;
					num_in_range_nodes++;
				}
			}
			else
			{// Look for crouch node links...
				visCheck = NodeVisibleCrouch( nodes[loop].origin, nodes[node].origin, -1 );

				//0 = wall in way
				//1 = player or no obstruction
				//2 = useable door in the way.
				//3 = door entity in the way.
				if ( visCheck == 1 )
				{
					if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						in_range_nodes[num_in_range_nodes] = loop;
						in_range_flags[num_in_range_nodes] = PATH_CROUCH;
						num_in_range_nodes++;
					}
				}
				else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
				{
					if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						in_range_nodes[num_in_range_nodes] = loop;
						in_range_flags[num_in_range_nodes] = PATH_CROUCH;
						num_in_range_nodes++;
					}
				}
			}
		}
	}

	for ( current_angle = 0; current_angle <= 360; current_angle+=45 ) // 8 angles tested, 45 degrees each...
	{
		best_node = -1;
		best_flags = 0;
		best_dist = MAX_NODE_LINK_DISTANCE + 0.1f;

		angles[PITCH] = 0;
		angles[YAW] = current_angle;
		angles[ROLL] = 0;

		for ( loop = 0; loop < num_in_range_nodes; loop++ )
		{
			float dist = VectorDistance(nodes[in_range_nodes[loop]].origin, nodes[node].origin);

			if (dist > MAX_NODE_LINK_DISTANCE)
			{
				continue;
			}

			if ( linknum >= MAX_NODELINKS-2 )
			{
				break;
			}

			if (NodeIsAlreadyALink(node, in_range_nodes[loop], linknum))
			{
				continue;
			}

			if (dist < best_dist)
			{
				if (!AIMOD_MAPPING_InFOV(nodes[in_range_nodes[loop]].origin, nodes[node].origin, angles, 45, 170))
				{
					continue;
				}
				
				best_dist = dist;
				best_node = in_range_nodes[loop];
				best_flags = in_range_flags[num_in_range_nodes];
			}
		}

		if (best_node != -1)
		{
			nodes[node].links[linknum].targetNode = best_node;

			if (best_flags)
			{// Mark it as much harder to reach!
				nodes[node].links[linknum].cost = (VectorDistance( nodes[best_node].origin, nodes[node].origin ) + (HeightDistance(nodes[best_node].origin, nodes[node].origin)) * 4) * 100;
			}
			else
			{
				nodes[node].links[linknum].cost = (VectorDistance( nodes[best_node].origin, nodes[node].origin ) + (HeightDistance(nodes[best_node].origin, nodes[node].origin)) * 4);
			}

			if ((nodes[best_node].type & NODE_ICE) || (nodes[best_node].type & NODE_WATER))
				nodes[node].links[linknum].cost = 65000.0f;

			if (best_flags)
			{// Add any special link flags...
				nodes[node].links[linknum].flags |= best_flags;
			}

			linknum++;
		}
	}

	// Now add directly up links (ladders/lifts)...
	best_node = -1;
	best_flags = 0;
	best_dist = MAX_LADDER_NODE_LINK_DISTANCE + 0.1f;

	angles[PITCH] = 90;
	angles[YAW] = 0;
	angles[ROLL] = 0;

	// Find the best (closest) node on each angle...
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		float dist = VectorDistance(nodes[loop].origin, nodes[node].origin);

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if (dist > MAX_LADDER_NODE_LINK_DISTANCE)
		{
			continue;
		}

		if (!(nodes[node].type & NODE_LAND_VEHICLE) && (nodes[loop].type & NODE_LAND_VEHICLE))
		{
			continue;
		}

		if ( /*BAD_WP_Height( nodes[node].origin, nodes[loop].origin)*/VectorDistanceNoHeight(nodes[node].origin, nodes[loop].origin) > 128 )
		{
			continue;
		}

		if (NodeIsAlreadyALink(node, loop, linknum))
		{
			continue;
		}

		if (dist < best_dist)
		{
			int visCheck = -1;

			if (AIMOD_MAPPING_InFOV(nodes[loop].origin, nodes[node].origin, angles, 20, 20))
			{
				continue;
			}

			visCheck = NodeVisible( nodes[node].origin, nodes[loop].origin, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					best_dist = dist;
					best_node = loop;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					best_dist = dist;
					best_node = loop;
				}
			}
			else
			{// Look for jump node links...
				visCheck = NodeVisibleJump( nodes[node].origin, nodes[loop].origin, -1 );

				//0 = wall in way
				//1 = player or no obstruction
				//2 = useable door in the way.
				//3 = door entity in the way.
				if ( visCheck == 1 )
				{
					if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						best_dist = dist;
						best_node = loop;
						best_flags = PATH_JUMP;
					}
				}
				else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
				{
					if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						best_dist = dist;
						best_node = loop;
						best_flags = PATH_JUMP;
					}
				}
				else
				{// Look for crouch node links...
					visCheck = NodeVisibleCrouch( nodes[node].origin, nodes[loop].origin, -1 );

					//0 = wall in way
					//1 = player or no obstruction
					//2 = useable door in the way.
					//3 = door entity in the way.
					if ( visCheck == 1 )
					{
						if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							best_dist = dist;
							best_node = loop;
							best_flags = PATH_CROUCH;
						}
					}
					else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
					{
						if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							best_dist = dist;
							best_node = loop;
							best_flags = PATH_CROUCH;
						}
					}
				}
			}
		}
	}

	if (best_node != -1)
	{
		nodes[node].links[linknum].targetNode = best_node;

		if (best_flags)
		{// Mark it as much harder to reach!
			nodes[node].links[linknum].cost = (VectorDistance( nodes[best_node].origin, nodes[node].origin ) + (HeightDistance(nodes[best_node].origin, nodes[node].origin)) * 4) * 100;
		}
		else
		{
			nodes[node].links[linknum].cost = (VectorDistance( nodes[best_node].origin, nodes[node].origin ) + (HeightDistance(nodes[best_node].origin, nodes[node].origin)) * 4);
		}

		if ((nodes[best_node].type & NODE_ICE) || (nodes[best_node].type & NODE_WATER))
			nodes[node].links[linknum].cost = 65000.0f;
		else
			nodes[node].links[linknum].cost *= 100.0f; // These always cost more!

		if (best_flags)
		{// Add any special link flags...
			nodes[node].links[linknum].flags |= best_flags;
		}

		linknum++;
	}

	// Now add directly down links (ladders/lifts)...
	best_node = -1;
	best_flags = 0;
	best_dist = MAX_LADDER_NODE_LINK_DISTANCE + 0.1f;

	angles[PITCH] = -90;
	angles[YAW] = 0;
	angles[ROLL] = 0;

	// Find the best (closest) node on each angle...
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		float dist = VectorDistance(nodes[loop].origin, nodes[node].origin);

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if (dist > MAX_LADDER_NODE_LINK_DISTANCE)
		{
			continue;
		}

		if (!(nodes[node].type & NODE_LAND_VEHICLE) && (nodes[loop].type & NODE_LAND_VEHICLE))
		{
			continue;
		}

		if ( /*BAD_WP_Height( nodes[node].origin, nodes[loop].origin)*/VectorDistanceNoHeight(nodes[node].origin, nodes[loop].origin) > 128 )
		{
			continue;
		}

		if (NodeIsAlreadyALink(node, loop, linknum))
		{
			continue;
		}

		if (dist < best_dist)
		{
			int visCheck = -1;
			vec3_t temp;

			if (AIMOD_MAPPING_InFOV(nodes[loop].origin, nodes[node].origin, angles, 48/*20*/, 48/*20*/))
			{
				continue;
			}

			visCheck = NodeVisible( nodes[node].origin, nodes[loop].origin, -1 );

			if (visCheck <= 0)
			{
				// UQ1: look from above to try to find jump-down places where the old system failed!
				VectorCopy(nodes[node].origin, temp);
				temp[0] = nodes[loop].origin[0];
				temp[1] = nodes[loop].origin[1];

				visCheck = NodeVisible( nodes[node].origin, temp, -1 );

				if (visCheck == 1)
				{
					visCheck = NodeVisible( temp, nodes[loop].origin, -1 );
				}
			}

			/*temp[2] += 48;
			visCheck = NodeVisible( temp, nodes[loop].origin, -1 );

			if (visCheck <= 0)
			{
				VectorCopy(nodes[node].origin, temp);
				temp[2] = RoofHeightAt(nodes[node].origin) - 48;
				visCheck = NodeVisible( temp, nodes[loop].origin, -1 );
			}*/

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				//if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					best_dist = dist;
					best_node = loop;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				//if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					best_dist = dist;
					best_node = loop;
				}
			}
			else
			{// Look for jump node links...
				visCheck = NodeVisibleJump( nodes[node].origin, nodes[loop].origin, -1 );

				//0 = wall in way
				//1 = player or no obstruction
				//2 = useable door in the way.
				//3 = door entity in the way.
				if ( visCheck == 1 )
				{
					//if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						best_dist = dist;
						best_node = loop;
						best_flags = PATH_JUMP;
					}
				}
				else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
				{
					//if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						best_dist = dist;
						best_node = loop;
						best_flags = PATH_JUMP;
					}
				}
				else
				{// Look for crouch node links...
					visCheck = NodeVisibleCrouch( nodes[node].origin, nodes[loop].origin, -1 );

					//0 = wall in way
					//1 = player or no obstruction
					//2 = useable door in the way.
					//3 = door entity in the way.
					if ( visCheck == 1 )
					{
						//if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							best_dist = dist;
							best_node = loop;
							best_flags = PATH_CROUCH;
						}
					}
					else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
					{
						//if (AIMod_Mapping_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							best_dist = dist;
							best_node = loop;
							best_flags = PATH_CROUCH;
						}
					}
				}
			}
		}
	}

	if (best_node != -1)
	{
		nodes[node].links[linknum].targetNode = best_node;

		if (best_flags)
		{// Mark it as much harder to reach!
			nodes[node].links[linknum].cost = (VectorDistance( nodes[best_node].origin, nodes[node].origin ) + (HeightDistance(nodes[best_node].origin, nodes[node].origin)) * 4) * 100;
		}
		else
		{
			nodes[node].links[linknum].cost = (VectorDistance( nodes[best_node].origin, nodes[node].origin ) + (HeightDistance(nodes[best_node].origin, nodes[node].origin)) * 4);
		}

		if ((nodes[best_node].type & NODE_ICE) || (nodes[best_node].type & NODE_WATER))
			nodes[node].links[linknum].cost = 65000.0f;
		else
			nodes[node].links[linknum].cost *= 100.0f; // These always cost more!

		if (best_flags)
		{// Add any special link flags...
			nodes[node].links[linknum].flags |= best_flags;
		}

		linknum++;
	}

	nodes[node].enodenum = linknum;
	return ( linknum );
}

float RoofHeightAt ( vec3_t org );

/* */
void
AIMOD_MAPPING_CreateSpecialNodeFlags ( int node )
{	// Need to check for duck (etc) nodes and mark them...
	trace_t tr;
	vec3_t	up, down, temp, uporg;
	vec3_t	tankMaxsSize = {80, 80, 0};
	vec3_t	tankMinsSize = {-80, -80, 0};

	VectorCopy( nodes[node].origin, temp );
	temp[2] += 1;
	nodes[node].type &= ~NODE_DUCK;
	VectorCopy( nodes[node].origin, up );
	up[2] += 16550;
	trap_Trace( &tr, nodes[node].origin, NULL, NULL, up, -1, MASK_SHOT | MASK_OPAQUE | MASK_WATER );
	
	if ( VectorDistance( nodes[node].origin, tr.endpos) <= 72 )
	{	// Could not see the up pos.. Need to duck to go here!
		nodes[node].type |= NODE_DUCK;
	}

	VectorCopy( nodes[node].origin, up );
	up[2] = RoofHeightAt(nodes[node].origin) - 16;
	VectorCopy( nodes[node].origin, down );
	down[2] -= 64;

	trap_Trace( &tr, up, NULL, NULL, down, -1, MASK_SHOT | MASK_OPAQUE | MASK_WATER );

	if (tr.contents & CONTENTS_WATER)
	{
		nodes[node].type |= NODE_WATER;
	}

	//if ( AI_PM_SlickTrace( nodes[node].origin, -1) )
	if ( tr.surfaceFlags & SURF_SLICK )
	{	// This node is on slippery ice... Mark it...
		nodes[node].type |= NODE_ICE;
	}

	VectorCopy(nodes[node].origin, uporg);
	uporg[2]+=104;

	if ( TankNodeVisible( nodes[node].origin, uporg, tankMinsSize, tankMaxsSize, -1) == 1 )
	{
		nodes[node].type |= NODE_LAND_VEHICLE;
	}
}

/* */
void
AIMOD_MAPPING_MakeLinks ( void )
{
	int		loop = 0;

	G_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^5Creating waypoint linkages and flags for ^3%i^5 waypoints...\n", number_of_nodes );

	for ( loop = 0; loop < number_of_nodes; loop++ )
	{// Do links...
		nodes[loop].enodenum = 0;

		// Also check if the node needs special flags...
		AIMOD_MAPPING_CreateSpecialNodeFlags( loop );
		AIMOD_MAPPING_CreateNodeLinks( loop );
	}
}

/* */
void
AIMOD_MAPPING_AddNode ( int clientNum, int teamNum )
{
	int		i;
	vec3_t	v;
	if ( teamNum == 99 )
	{																// I will use 99 as a vehicle only node...
		nodes[number_of_nodes].type |= NODE_LAND_VEHICLE;
	}

	if ( teamNum == TEAM_ALLIES )
	{
		nodes[number_of_nodes].objTeam |= TEAM_ALLIES;
		nodes[number_of_nodes].type |= NODE_AXIS_UNREACHABLE;
	}
	else if ( teamNum == TEAM_AXIS )
	{
		nodes[number_of_nodes].type |= NODE_ALLY_UNREACHABLE;
		nodes[number_of_nodes].objTeam |= TEAM_AXIS;
	}

	if ( relocate_node )
	{
		if ( current_node == INVALID || current_node > MAX_NODES )
		{															// Safety comes first :-p
			return;
		}

		// Put node at new origin, create new path-costs
		VectorCopy( g_entities[clientNum].r.currentOrigin, nodes[current_node].origin );
		for ( i = 0; i < MAX_NODELINKS; i++ )
		{
			if ( nodes[current_node].links[i].targetNode != INVALID )
			{
				VectorSubtract( nodes[current_node].origin, nodes[nodes[current_node].links[i].targetNode].origin, v );
				nodes[current_node].links[i].cost = VectorLength( v );
			}
		}

		// Recreate visible entity if necessary
		if ( visible_nodes[current_node] != NULL )
		{
			G_FreeEntity( visible_nodes[current_node] );
			visible_nodes[current_node] = NULL;
		}

		// Do links...
		//AIMOD_MAPPING_CreateNodeLinks( current_node ); // Do this on save...
		AIMOD_MAPPING_CalculateVisibleNodes( &g_entities[clientNum] );
	}
	else if ( number_of_nodes < MAX_NODES )
	{																// Setup the node's origin for now...
		//		VectorCopy(g_entities[clientNum].r.currentOrigin, g_entities[clientNum].last_added_waypoint);
		VectorCopy( g_entities[clientNum].r.currentOrigin, nodes[number_of_nodes].origin );
		if ( !nodes[number_of_nodes].type )
		{
			nodes[number_of_nodes].type = NODE_MOVE;				// Default node type
		}

		// Create a connection between this new node and the previous one
		for ( i = 0; i < MAX_NODELINKS; i++ )
		{
			nodes[number_of_nodes].links[i].targetNode = INVALID;	// Set all links of the NEW node to invalid
		}

		// Do links...
		//AIMOD_MAPPING_CreateNodeLinks( current_node ); // Do this on save isntead...
		G_Printf( "^4*** ^3%s^5: Node %i created.\n", GAME_VERSION, number_of_nodes + 1 );
		current_node = number_of_nodes;
		number_of_nodes++;											// We just added a node
		AIMOD_MAPPING_CalculateVisibleNodes( &g_entities[clientNum] );
	}
	else
	{																// No longer able to add nodes
		G_Printf( "Maximum number of nodes reached.\n" );
	}
}


// -------------------------------------------
// -------------------------------------------
// AVD Creates entities for all the nodes

// -------------------------------------------
void
AIMOD_MAPPING_ShowNodes ( gentity_t *ent )
{
	if ( shownodes )
	{
		return;
	}

	G_Printf( "Node entities created.\n" );
	shownodes = qtrue;

	//AIMOD_MAPPING_CalculateVisibleNodes(ent);
	BotWaypointRender();
}


// -------------------------------------------
void
AIMOD_MAPPING_ShowConnection ( int from, int to )
{
	if ( number_conn < 500 && from != INVALID && to != INVALID && from != to )
	{
		connections[number_conn] = G_Spawn();
		connections[number_conn]->s.eType = ET_EVENTS + EV_RAILTRAIL;
		connections[number_conn]->classname = "node_connection";
		connections[number_conn]->eventTime = level.time;
		connections[number_conn]->r.eventTime = level.time;
		connections[number_conn]->freeAfterEvent = qfalse;
		G_SetOrigin( connections[number_conn], nodes[from].origin );
		trap_LinkEntity( connections[number_conn] );
		VectorCopy( nodes[to].origin, connections[number_conn]->s.origin2 );
		connections[number_conn]->s.otherEntityNum2 = 0;
		connections[number_conn]->s.dmgFlags = 0;
		number_conn++;
	}
}


/* */
void
AIMOD_MAPPING_SetConnectionCost ( int from, int to, int new_cost )
{
	int i;
	for ( i = 0; i < MAX_NODELINKS; i++ )
	{
		if ( nodes[from].links[i].targetNode == to )
		{
			if ( new_cost == 32000 )
			{
				nodes[from].links[i].targetNode = INVALID;
				nodes[from].links[i].cost = 0;
				G_Printf( "Removed link.\n" );
			}
			else
			{

				//G_Printf("Updated cost from %i to %i : %i  (was %f).\n", from+1, to+1, new_cost, nodes[from].links[i].cost);
				nodes[from].links[i].cost = (float) new_cost;
			}

			return;
		}
	}
}


// AVD Sets the type of a node
void
AIMOD_MAPPING_SetNodeType ( int node, int node_type )
{

	/*
	if (node == INVALID)
		node = current_node;

    nodes[node].type = node_type;

	G_Printf("Type of node %i set to %i.\n", node+1, node_type);
	*/
}


/* */
void
G_TestLine ( vec3_t start, vec3_t end, int color, int time )
{
	gentity_t	*te;
	te = G_TempEntity( start, EV_DEBUG_LINE /*EV_TESTLINE*/ );
	VectorCopy( start, te->s.origin );
	VectorCopy( start, te->s.pos.trBase );
	VectorCopy( end, te->s.origin2 );
	te->s.time2 = time;
	te->s.weapon = color;
	te->r.svFlags |= SVF_BROADCAST;
}


/* */
void
G_MarkGoalNode ( int node )
{
	gentity_t	*plum;
	plum = G_TempEntity( nodes[node].origin, EV_GOALNODE );
	plum->r.svFlags |= SVF_BROADCAST;
	plum->s.time = node;
}

int				gWPRenderTime = 0;
int				gWPRenderedFrame = 0;
int				gWPNum = 0;
int				gLastPrintedIndex = 0;
extern int		num_links[MAX_NODES];					// AIMOD_navigation.c
extern void		AIMOD_NAVIGATION_Set_NumLinks ( void ); // AIMOD_navigation.c
extern vmCvar_t bot_node_edit;
extern vmCvar_t bot_node_edit_tankonly;
qboolean		linksdone = qfalse;

extern qboolean InFOV2( vec3_t origin, gentity_t *from, int hFOV, int vFOV );

void BotWaypointRenderRoute(void)
{
	int i;
	int bestindex;
	gentity_t *plum;
	int num_renderred = 0;
	gentity_t *ent = &g_entities[0];

	if (!bot_node_edit.integer)
	{
		return;
	}

	if (g_dedicated.integer)
		return; // Don't need them visible in dedicated mode for fvk sake..

	if (!ent || !ent->client)
		return; // Wait till they are ingame!

	if (ent->pathsize <= 0)
		return;

	bestindex = 0;

	if (gWPRenderTime > level.time)
	{
		return;
	}

	gWPRenderTime = level.time + 100;

	i = ent->pathsize-1;

	while (i >= 0)
	{
		vec3_t /*vieworg,*/ destorg;

		if (!ent->pathlist[i])
		{
			i--;
			continue;
		}

		//VectorCopy(ent->r.currentOrigin, vieworg);
		//vieworg[2]+=32;
		VectorCopy(nodes[ent->pathlist[i]].origin, destorg);
		destorg[2]+=16;

		if (num_renderred < 128)		
		{
			if (G_GentitiesAvailable() < MIN_SPARE_GENTITIES)
				break;

			plum = G_TempEntity( destorg, EV_SCOREPLUM );
			plum->r.svFlags |= SVF_BROADCAST;
			plum->s.time = i;

			if ( nodes[i].type == NODE_COVER )
			{
				plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
			}
			else if ( nodes[i].type & NODE_JUMP || nodes[i].type & NODE_FASTHOP )
			{
				plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
			}
			else if ( nodes[i].type & NODE_DUCK )
			{
				plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
			}
			else if ( nodes[i].type & NODE_LAND_VEHICLE )
			{
				plum->s.weapon = 4;		// Mark this as a vehicle node for the tank icon.
			}
			else
			{
				plum->s.weapon = 0;
			}
	
			num_renderred++;
		}
		else
		{
			gWPRenderedFrame = 0;
			break;
		}

		i--;
	}

	if (i >= gWPNum)
	{
		gWPRenderTime = level.time + 100; //wait a bit after we finish doing the whole trail
		gWPRenderedFrame = 0;
	}
}


/* */
void
BotWaypointRender ( void )
{
	int			i, n;
	int			inc_checker;
	int			bestindex;
	int			gotbestindex;
	float		bestdist;
	float		checkdist;
	gentity_t	*plum;
	gentity_t	*viewent;
	char		*flagstr;
	vec3_t		a;
	vec3_t		eyes;
	VectorCopy( g_entities[0].r.currentOrigin, eyes );
	eyes[2] += 32;
	
	if ( bot_node_edit.integer )
	{
		shownodes = qtrue;
	}

	if ( !shownodes )
	{
		return;
	}

	gWPNum = number_of_nodes;

	/*	if (!linksdone)
	{
		AIMOD_NAVIGATION_Set_NumLinks();
		linksdone = qtrue;
	}*/
	bestindex = 0;
	if ( gWPRenderTime > level.time )
	{
		goto checkprint;
	}

	gWPRenderTime = level.time + 300;		// 100
	gWPRenderedFrame = 0;
	i = 0;									//gWPRenderedFrame;
	inc_checker = 0;						//gWPRenderedFrame;
	while ( i < gWPNum )
	{
		if ( nodes[i].origin )
		{
			if ( bot_node_edit_tankonly.integer && !(nodes[i].type & NODE_LAND_VEHICLE) )
			{
				i++;
				continue;
			}

			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, nodes[i].origin) < 128 &&
				OrgVisible( nodes[i].origin, eyes, -1)
			)
			{
				vec3_t	up;

				if (G_GentitiesAvailable() < MIN_SPARE_GENTITIES)
					break;

				VectorCopy( nodes[i].origin, up );
				up[2] += 64;//i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				if ( nodes[i].type == NODE_COVER )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( nodes[i].type & NODE_JUMP || nodes[i].type & NODE_FASTHOP )
				{
					plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
				}
				else if ( nodes[i].type & NODE_DUCK )
				{
					plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
				}
				else if ( nodes[i].type & NODE_LAND_VEHICLE )
				{
					plum->s.weapon = 4;		// Mark this as a vehicle node for the tank icon.
				}
				else
				{
					plum->s.weapon = 0;
				}

				n = 0;
				gWPRenderedFrame++;

				/*{
					int z;

					for (z = 0; z < nodes[i].enodenum; z++)
					{
						DebugLine(nodes[i].origin, nodes[nodes[i].links[z].targetNode].origin, 1);
					}
				}*/
			}
		}

		/*		else
		{
			gWPRenderedFrame = 0;
			break;
		}*/

		//if ((i - inc_checker) > 4)
		if ( gWPRenderedFrame > 64 /*128*/ )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	i = 0;									//gWPRenderedFrame;
	inc_checker = 0;						//gWPRenderedFrame;
	while ( i < gWPNum )
	{
		if ( gWPRenderedFrame > 64 /*128*/ )
		{
			break;
		}

		if ( nodes[i].origin )
		{
			if ( bot_node_edit_tankonly.integer && !(nodes[i].type & NODE_LAND_VEHICLE) )
			{
				i++;
				continue;
			}

			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, nodes[i].origin) < 512 &&
				VectorDistance( g_entities[0].r.currentOrigin, nodes[i].origin) >= 128 &&
				OrgVisible( nodes[i].origin, eyes, -1)
			)
			{
				vec3_t	up;

				if (G_GentitiesAvailable() < MIN_SPARE_GENTITIES)
					break;

				VectorCopy( nodes[i].origin, up );
				up[2] += 64;//i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				if ( nodes[i].type == NODE_COVER )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( nodes[i].type & NODE_JUMP || nodes[i].type & NODE_FASTHOP )
				{
					plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
				}
				else if ( nodes[i].type & NODE_DUCK )
				{
					plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
				}
				else if ( nodes[i].type & NODE_LAND_VEHICLE )
				{
					plum->s.weapon = 4;		// Mark this as a vehicle node for the tank icon.
				}
				else
				{
					plum->s.weapon = 0;
				}

				n = 0;
				gWPRenderedFrame++;

				/*{
					int z;

					for (z = 0; z < nodes[i].enodenum; z++)
					{
						DebugLine(nodes[i].origin, nodes[nodes[i].links[z].targetNode].origin, 1);
					}
				}*/
			}
		}

		/*		else
		{
			gWPRenderedFrame = 0;
			break;
		}*/

		//if ((i - inc_checker) > 4)
		if ( gWPRenderedFrame > 64 /*128*/ )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	i = 0;									//gWPRenderedFrame;
	inc_checker = 0;						//gWPRenderedFrame;
	while ( i < gWPNum )
	{
		if ( gWPRenderedFrame > 64 /*128*/ )
		{
			break;
		}

		if ( nodes[i].origin )
		{
			if ( bot_node_edit_tankonly.integer && !(nodes[i].type & NODE_LAND_VEHICLE) )
			{
				i++;
				continue;
			}

			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, nodes[i].origin) < 1024 &&
				VectorDistance( g_entities[0].r.currentOrigin, nodes[i].origin) >= 512 &&
				OrgVisible( nodes[i].origin, eyes, -1)
			)
			{
				vec3_t	up;

				if (G_GentitiesAvailable() < MIN_SPARE_GENTITIES)
					break;

				VectorCopy( nodes[i].origin, up );
				up[2] += 64;//i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				if ( nodes[i].type == NODE_COVER )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( nodes[i].type & NODE_JUMP || nodes[i].type & NODE_FASTHOP )
				{
					plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
				}
				else if ( nodes[i].type & NODE_DUCK )
				{
					plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
				}
				else if ( nodes[i].type & NODE_LAND_VEHICLE )
				{
					plum->s.weapon = 4;		// Mark this as a vehicle node for the tank icon.
				}
				else
				{
					plum->s.weapon = 0;
				}

				n = 0;
				gWPRenderedFrame++;

				/*{
					int z;

					for (z = 0; z < nodes[i].enodenum; z++)
					{
						DebugLine(nodes[i].origin, nodes[nodes[i].links[z].targetNode].origin, 1);
					}
				}*/
			}
		}

		/*		else
		{
			gWPRenderedFrame = 0;
			break;
		}*/

		//if ((i - inc_checker) > 4)
		if ( gWPRenderedFrame > 64 /*128*/ )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	if ( i >= gWPNum )
	{
		gWPRenderTime = level.time + 300;	//1500; //wait a bit after we finish doing the whole trail
		gWPRenderedFrame = 0;
	}

checkprint:
	if ( !bot_node_edit.integer )
	{
		return;
	}

	viewent = &g_entities[0];				//only show info to the first client
	if ( !viewent || !viewent->client )
	{										//client isn't in the game yet?
		return;
	}

	bestdist = 256;							//max distance for showing point info
	gotbestindex = 0;
	i = 0;
	while ( i < gWPNum )
	{
		if ( nodes[i].origin /*&& nodes[i]->inuse*/ )
		{
			VectorSubtract( viewent->client->ps.origin, nodes[i].origin, a );
			checkdist = VectorLength( a );
			if ( checkdist < bestdist )
			{
				bestdist = checkdist;
				bestindex = i;
				gotbestindex = 1;
			}
		}

		i++;
	}

	if ( gotbestindex && bestindex != gLastPrintedIndex )
	{
		if ( nodes[bestindex].type == NODE_JUMP )
		{
			flagstr = "Jump";
		}
		else if ( nodes[bestindex].type == NODE_FASTHOP )
		{
			flagstr = "Fast-Hop";
		}
		else if ( nodes[bestindex].type == NODE_WATER )
		{
			flagstr = "Water";
		}
		else if ( nodes[bestindex].type == NODE_LADDER )
		{
			flagstr = "Ladder";
		}
		else if ( nodes[bestindex].type == NODE_OBJECTIVE )
		{
			flagstr = "Objective";
		}
		else if ( nodes[bestindex].type == NODE_BUILD )
		{
			flagstr = "Build";
		}
		else if ( nodes[bestindex].type == NODE_DYNAMITE )
		{
			flagstr = "Dynamite";
		}
		else if ( nodes[bestindex].type == NODE_MG42 )
		{
			flagstr = "MG-42";
		}
		else if ( nodes[bestindex].type == NODE_COVER )
		{
			flagstr = "Coverspot Node";
		}
		else if ( nodes[bestindex].type & NODE_LAND_VEHICLE )
		{
			flagstr = "Land Vehicle Node";
		}
		else
		{
			flagstr = "None";
		}

		gLastPrintedIndex = bestindex;
		G_Printf( "^5Node ^7%i\n^5Flags - ^3%i^5 (^7%s^5)\n^5Origin - (^3%i %i %i^5)\n", (int) (bestindex),
				  (int) nodes[bestindex].type, flagstr, (int) (nodes[bestindex].origin[0]),
				  (int) (nodes[bestindex].origin[1]), (int) (nodes[bestindex].origin[2]) );

		if (G_GentitiesAvailable() >= MIN_SPARE_GENTITIES)
		{
			plum = G_TempEntity( nodes[bestindex].origin, EV_SCOREPLUM );
			plum->r.svFlags |= SVF_BROADCAST;
			plum->s.time = bestindex;			//render it once
		}
	}
	else if ( !gotbestindex )
	{
		gLastPrintedIndex = -1;
	}
}


/* */
void
BotRouteWaypointRender ( gentity_t *bot )
{											// Render this bot's route...
	int			i, n;
	int			inc_checker;
	int			bestindex;
	gentity_t	*plum;
	vec3_t		eyes;
	VectorCopy( g_entities[0].r.currentOrigin, eyes );
	eyes[2] += 32;
	bestindex = 0;
	if ( gWPRenderTime > level.time )
	{
		return;
	}

	gWPRenderTime = level.time + 300;
	gWPRenderedFrame = 0;
	i = 0;
	inc_checker = 0;
	while ( i < bot->pathsize )
	{
		if ( nodes[bot->pathlist[i]].origin )
		{
			if (							/*VectorDistance(g_entities[0].r.currentOrigin, nodes[bot->pathlist[i]].origin) < 128
				&&*/
					OrgVisible( nodes[bot->pathlist[i]].origin, eyes, -1)
				)
			{
				vec3_t	up;
				VectorCopy( nodes[bot->pathlist[i]].origin, up );
				up[2] += bot->pathlist[i] / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = bot->pathlist[i];
				if ( nodes[bot->pathlist[i]].type == NODE_COVER )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( nodes[bot->pathlist[i]].type == NODE_JUMP || nodes[bot->pathlist[i]].type == NODE_FASTHOP )
				{
					plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
				}
				else
				{
					plum->s.weapon = 0;
				}

				n = 0;
				gWPRenderedFrame++;

				/*{
					int z;

					for (z = 0; z < nodes[i].enodenum; z++)
					{
						DebugLine(nodes[i].origin, nodes[nodes[i].links[z].targetNode].origin, 1);
					}
				}*/
			}
		}

		if ( gWPRenderedFrame > 256 /*64*/ )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	if ( i >= bot->pathsize )
	{
		gWPRenderTime = level.time + 300;	//1500; //wait a bit after we finish doing the whole trail
		gWPRenderedFrame = 0;
	}
}


/* */
void
BotWaypointRenderORIG ( void )
{
	int			i, n;
	int			inc_checker;
	int			bestindex;
	int			gotbestindex;
	float		bestdist;
	float		checkdist;
	gentity_t	*plum;
	gentity_t	*viewent;
	char		*flagstr;
	vec3_t		a;

	//	vec3_t eyes;
	//VectorCopy(g_entities[0].r.currentOrigin, eyes);
	//eyes[2]+=32;
	if ( bot_node_edit.integer )
	{
		shownodes = qtrue;
	}

	if ( !shownodes )
	{
		return;
	}

	gWPNum = number_of_nodes;

	/*	if (!linksdone)
	{
		AIMOD_NAVIGATION_Set_NumLinks();
		linksdone = qtrue;
	}*/
	bestindex = 0;
	if ( gWPRenderTime > level.time )
	{
		goto checkprint;
	}

	gWPRenderTime = level.time + 100;
	i = 0;
	inc_checker = 0;
	while ( i < gWPNum )
	{
		if ( nodes[i].origin /*&& nodes[i]->inuse*/ /*OrgVisible(nodes[i].origin, eyes, -1)*/ )
		{

			//if (VectorDistance(g_entities[0].r.currentOrigin, nodes[i].origin) < 1500)
			//{
			plum = G_TempEntity( nodes[i].origin, EV_SCOREPLUM );
			plum->r.svFlags |= SVF_BROADCAST;
			plum->s.time = i;
			if ( nodes[i].type == NODE_COVER )
			{
				plum->s.weapon = 1;			// Mark this as a goal node for the yellow highlight.
			}
			else if ( nodes[i].type == NODE_JUMP || nodes[i].type == NODE_FASTHOP )
			{
				plum->s.weapon = 2;			// Mark this as a jump node for the red highlight.
			}
			else
			{
				plum->s.weapon = 0;
			}

			n = 0;
			gWPRenderedFrame++;

			//}
		}
		else
		{
			gWPRenderedFrame = 0;
			break;
		}

		if ( (i - inc_checker) > 4 )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	if ( i >= gWPNum )
	{
		gWPRenderTime = level.time + 1500;	//wait a bit after we finish doing the whole trail
		gWPRenderedFrame = 0;
	}

checkprint:
	if ( !bot_node_edit.integer )
	{
		return;
	}

	viewent = &g_entities[0];				//only show info to the first client
	if ( !viewent || !viewent->client )
	{										//client isn't in the game yet?
		return;
	}

	bestdist = 256;							//max distance for showing point info
	gotbestindex = 0;
	i = 0;
	while ( i < gWPNum )
	{
		if ( nodes[i].origin /*&& nodes[i]->inuse*/ )
		{
			VectorSubtract( viewent->client->ps.origin, nodes[i].origin, a );
			checkdist = VectorLength( a );
			if ( checkdist < bestdist )
			{
				bestdist = checkdist;
				bestindex = i;
				gotbestindex = 1;
			}
		}

		i++;
	}

	if ( gotbestindex && bestindex != gLastPrintedIndex )
	{
		if ( nodes[bestindex].type == NODE_JUMP )
		{
			flagstr = "Jump";
		}
		else if ( nodes[bestindex].type == NODE_FASTHOP )
		{
			flagstr = "Fast-Hop";
		}
		else if ( nodes[bestindex].type == NODE_WATER )
		{
			flagstr = "Water";
		}
		else if ( nodes[bestindex].type == NODE_LADDER )
		{
			flagstr = "Ladder";
		}
		else if ( nodes[bestindex].type == NODE_OBJECTIVE )
		{
			flagstr = "Objective";
		}
		else if ( nodes[bestindex].type == NODE_BUILD )
		{
			flagstr = "Build";
		}
		else if ( nodes[bestindex].type == NODE_DYNAMITE )
		{
			flagstr = "Dynamite";
		}
		else if ( nodes[bestindex].type == NODE_MG42 )
		{
			flagstr = "MG-42";
		}
		else if ( nodes[bestindex].type == NODE_COVER )
		{
			flagstr = "Coverspot Node";
		}
		else
		{
			flagstr = "None";
		}

		gLastPrintedIndex = bestindex;
		G_Printf( "^5Node ^7%i\n^5Flags - ^3%i^5 (^7%s^5)\n^5Origin - (^3%i %i %i^5)\n", (int) (bestindex),
				  (int) nodes[bestindex].type, flagstr, (int) (nodes[bestindex].origin[0]),
				  (int) (nodes[bestindex].origin[1]), (int) (nodes[bestindex].origin[2]) );
		plum = G_TempEntity( nodes[bestindex].origin, EV_SCOREPLUM );
		plum->r.svFlags |= SVF_BROADCAST;
		plum->s.time = bestindex;			//render it once
	}
	else if ( !gotbestindex )
	{
		gLastPrintedIndex = -1;
	}
}

char	*nodeclassname;


// AVD this... calculates the visible nodes... surprising huh... sucks tho', cuz it doesn't work well
void
AIMOD_MAPPING_CalculateVisibleNodes ( gentity_t *ent )
{
	BotWaypointRender();
}


/* */
void
AIMOD_MAPPING_CalculateVisibleNodesOLD ( gentity_t *ent )
{
	int		i;	//, /*j,*/ z;

	//	int min, max;//, diff, diffmax;
	//	int shown = 0;
	//	int vnodes[MAX_VISIBLENODES];
	float	distance = 0.0, total_distance = 0.0;
	int		num_visible = 0;

	//	vec3_t v;
	if ( current_node == INVALID )
	{
		current_node = 0;
	}

	if ( !shownodes /*|| current_node < 0 || current_node > MAX_NODES*/ )
	{
		return;
	}

	// Destroy ALL connections, bit cpu intensive, but hey

	/*	for (i = 0; i < number_conn; i++)
		if (connections[i] != NULL) {
			G_FreeEntity(connections[i]);
			connections[i] = NULL;
		}*/
	number_conn = 0;

	// Calculate all nodes in a certain range
	//	min = ( (current_node - 50 >= 0) ? (current_node - 50) : 0 );		// We're showing QUITE SOME nodes here :(
	//	max = ( (current_node + 50 < number_of_nodes) ? (current_node + 50) : number_of_nodes );
	// Run through all visible entities, destroy or create if necessary

	/*for (i = 0; i < number_of_nodes; i++)
	{
		if (i >= min && i <= max)
		{
			if (visible_nodes[i] == NULL)
				AIMOD_MAPPING_CreateVisibleNode(i);
			// Always recreate the connections for the node !
			for (z = 0; z < MAX_NODELINKS; z++)
				if (nodes[i].links[z].targetNode != INVALID)
					AIMOD_MAPPING_ShowConnection( i, nodes[i].links[z].targetNode );
		}
		else {
			if (visible_nodes[i] != NULL) {
				G_FreeEntity( visible_nodes[i] );
				visible_nodes[i] = NULL;
			}
		}
	}*/

	/*	if (current_node != INVALID)
		for (z = 0; z < MAX_NODELINKS; z++)
			if (nodes[current_node].links[z].targetNode != INVALID)
				AIMOD_MAPPING_ShowConnection( current_node, nodes[current_node].links[z].targetNode );
*/
	if ( nodeclassname )
	{
		for ( i = MAX_CLIENTS; i < MAX_GENTITIES; i++ )
		{
			if ( !Q_stricmp( g_entities[i].classname, "node") )
			{
				G_FreeEntity( &g_entities[i] );
			}
		}
	}

	// Show all nodes within a certain distance of the current_node
	for ( i = 0; i < number_of_nodes; i++ )
	{
		vec3_t	eyes;
		if ( num_visible > 500 )
		{
			break;
		}

		VectorCopy( ent->r.currentOrigin, eyes );
		eyes[2] += 16;
		distance = VectorDistance( ent->r.currentOrigin, nodes[i].origin );
		total_distance += distance;
		if ( distance < 1000.0 && OrgVisible( eyes, nodes[i].origin, -1) /*|| i == current_node*/ )
		{
			if ( visible_nodes[i] == NULL )
			{
				AIMOD_MAPPING_CreateVisibleNode( i );

				//nodeclassname = g_entities[i].classname;
			}

			num_visible++;
		}
		else if ( visible_nodes[i] != NULL )
		{
			G_FreeEntity( visible_nodes[i] );
			visible_nodes[i] = NULL;
		}
	}

	//G_Printf("Showing %i nodes, average distance was %f.\n", num_visible, (total_distance /(float) number_of_nodes) );
}


// AVD Destroy all node entities

// -------------------------------------------
void
AIMOD_MAPPING_HideNodes ( void )
{
	int i;
	if ( !shownodes )
	{
		return;
	}

	// Destroy all connections between the nodes
	for ( i = 0; i < number_conn; i++ )
	{
		if ( connections[i] != NULL )
		{
			G_FreeEntity( connections[i] );
		}
	}

	number_conn = 0;

	// Destroy all nodes
	for ( i = 0; i < number_of_nodes; i++ )
	{
		if ( visible_nodes[i] != NULL )
		{
			G_FreeEntity( visible_nodes[i] );
		}
	}

	shownodes = qfalse;
}


// -------------------------------------------
void
AIMOD_MAPPING_SetTouchConnection ( void )
{
	if ( touch_connections )
	{
		touch_connections = qfalse;
		G_Printf( "Create connection on node touch: ^1OFF.\n" );
	}
	else
	{
		touch_connections = qtrue;
		G_Printf( "Create connection on node touch: ^1ON.\n" );
	}
}


/* */
void
AIMOD_MAPPING_SetRelocateNode ( void )
{
	if ( relocate_node )
	{
		relocate_node = qfalse;
		G_Printf( "Relocate node: ^1OFF.\n" );
	}
	else
	{
		relocate_node = qtrue;
		G_Printf( "Relocate node: ^1ON.\n" );
	}
}


// AVD Touch and select nodes

// -------------------------------------------
void
AIMOD_MAPPING_TouchNode ( gentity_t *ent, gentity_t *other, trace_t *trace )
{
	int node_num = ent->current_node;
	if ( node_num != current_node )
	{	// Select this node
		// Create a connection on 'touch' of a node, reset after creating it
		if ( touch_connections )
		{
			AIMOD_MAPPING_SetTouchConnection();
		}

		current_node = node_num;
		AIMOD_MAPPING_CalculateVisibleNodes( ent );
		G_Printf( "Node %i selected.\n", current_node + 1 );
	}
}


// -------------------------------------------
// AVD Creates a new visible entity for the current node

// -------------------------------------------
void
AIMOD_MAPPING_CreateVisibleNode ( int i )
{

	// Add the node itself
	visible_nodes[i] = G_Spawn();
	visible_nodes[i]->active = qtrue;
	visible_nodes[i]->classname = "node";
	visible_nodes[i]->r.svFlags = /*SVF_USE_CURRENT_ORIGIN |*/ SVF_BROADCAST;
	visible_nodes[i]->s.modelindex = G_ModelIndex( "models/weapons2/grenade/blablabla.md3" );
	visible_nodes[i]->current_node = i; // This is the node number
	visible_nodes[i]->touch = AIMOD_MAPPING_TouchNode;
	visible_nodes[i]->clipmask = CONTENTS_SOLID | CONTENTS_MISSILECLIP;
	visible_nodes[i]->r.contents = CONTENTS_TRIGGER | CONTENTS_ITEM;

	// Body, so we can touch this node
	VectorSet( visible_nodes[i]->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );				//----(SA)	so items sit on the ground
	VectorCopy( visible_nodes[i]->r.mins, visible_nodes[i]->r.absmin );
	VectorSet( visible_nodes[i]->r.maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );	//----(SA)	so items sit on the ground
	VectorCopy( visible_nodes[i]->r.maxs, visible_nodes[i]->r.absmax );
	VectorCopy( nodes[i].origin, visible_nodes[i]->s.origin );
	G_SetOrigin( visible_nodes[i], visible_nodes[i]->s.origin );
	G_SetAngle( visible_nodes[i], visible_nodes[i]->s.angles );
	trap_LinkEntity( visible_nodes[i] );
}


// -------------------------------------------
// AVD Prints the list of nodes to a file

// -------------------------------------------
void
AIMOD_MAPPING_WriteNodes ( void )
{
	int				i, j;
	char			*buffptr;
	fileHandle_t	f;

	// Create the mapnode file
	trap_FS_FOpenFile( "nodelist.txt", &f, FS_WRITE );
	if ( !f )
	{
		G_Printf( "Failed to open '%s' for writing.\n", "nodelist.txt" );
		return;
	}

	// Run through all nodes and write them out
	for ( i = 0; i < number_of_nodes; i++ )
	{
		buffptr = va( "%i %i %i %i ", i, (int) nodes[i].origin[0], (int) nodes[i].origin[1], (int) nodes[i].origin[2] );

		// Write the node away
		for ( j = 0; j < MAX_NODELINKS; j++ )
		{
			if ( nodes[i].links[j].targetNode != INVALID )
			{
				strcat( buffptr, va( "%i %f ", nodes[i].links[j].targetNode, nodes[i].links[j].cost) );
			}
		}

		strcat( buffptr, "-1\n" );
		trap_FS_Write( buffptr, strlen( buffptr), f );
	}

	// Close the node file
	trap_FS_FCloseFile( f );
	G_Printf( "Node file created.\n" );
}

// -------------------------------------------
#endif
