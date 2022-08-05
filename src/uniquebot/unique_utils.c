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
#include "../game/g_local.h"
typedef enum
{
	SPOT_ORIGIN,
	SPOT_CHEST,
	SPOT_HEAD,
	SPOT_HEAD_LEAN,
	SPOT_WEAPON,
	SPOT_LEGS,
	SPOT_GROUND
} spot_t;
#ifdef __BOT__

/*
============================================================================================

Description: This file contains utility functions, mainly those not directly related to bots

============================================================================================
*/

/*
###################### Function Descriptions ##################################

qboolean MyInfront (gentity_t *self, gentity_t *other)
	The difference between 'MyInfront' and 'infront' is that 'MyInfront' takes into account the 
	bot's skill (skill affects FOV of the bot)

qboolean MyVisible (gentity_t *self, gentity_t *other)
	The difference between MyVisible and 'visible' is that 'MyVisible' takes into account the
	viewheights of the bot and his target, making for more realistic enemy detection

int GetCost(int from, int to)
	Gets the total distance required to move from one node to another node by following
	node paths

int CreatePathAStar(gentity_t *bot, int from, int to, int *pathlist)
	Uses A* pathfinding to determine the shortest path from one node to another.
	Will likely be modified later

int GetFCost(gentity_t *bot, int to, int num, int parentNum, float *gcost)
	Utility function used by CreatePathAStar to determine the 'F cost' of a given
	node, based on the path used to reach it and its distance from the goal

###############################################################################
*/

//
// Team Doors...
//
extern int		AxisOnlyFirstNode;
extern int		AlliedOnlyFirstNode;
typedef struct axisDoors_s
{
	vec3_t	origin;
} axisDoors_t;
axisDoors_t		axisDoors[MAX_NODES];
int				numAxisDoors = 0;
typedef struct alliedDoors_s
{
	vec3_t	origin;
} alliedDoors_t;
alliedDoors_t	alliedDoors[MAX_NODES];
int				numAlliedDoors = 0;


/* */
void
BOT_AddAxisDoor ( vec3_t origin )
{
	VectorCopy( origin, axisDoors[numAxisDoors].origin );

	//G_Printf("Added axis door at %f %f %f!\n", axisDoors[numAxisDoors].origin[0], axisDoors[numAxisDoors].origin[1], axisDoors[numAxisDoors].origin[2]);
	numAxisDoors++;
}


/* */
void
BOT_AddAlliedDoor ( vec3_t origin )
{
	VectorCopy( origin, alliedDoors[numAlliedDoors].origin );

	//G_Printf("Added allied door!\n");
	numAlliedDoors++;
}

qboolean	TeamDoorsSetup = qfalse;


/* */
void
BOT_SetupTeamDoorWaypoints ( void )
{

	/*int i, j, k;
	int teamDoorNodes[512];
	int teamDoorTotal = 0;

	for (i = 0; i < numAxisDoors; i++)
	{
		for (j = 0; j < number_of_nodes; j++)
		{
			if (VectorDistance(nodes[j].origin, axisDoors[i].origin) > 64)
				continue; // too far away to bother with...

			teamDoorNodes[teamDoorTotal] = j;
			teamDoorTotal++;
		}
	}

	// Now check links to this node and mark them...
	for (i = 0; i < number_of_nodes; i++)
	{
		for (j = 0; j < nodes[i].enodenum; j++)
		{
			for (k = 0; k < teamDoorTotal; k++)
			{
				if (nodes[i].links[j].targetNode == teamDoorNodes[k])
				{// Found a link to our door.. Mark it!
					//G_Printf("node %i, link %i marked as axis only! (TEAM DOOR)\n", i, j);
					nodes[i].links[j].flags = NODE_ALLY_UNREACHABLE;
				}
			}
		}
	}

	// Init and do allies...
	teamDoorTotal = 0;

	for (i = 0; i < numAlliedDoors; i++)
	{
		for (j = 0; j < number_of_nodes; j++)
		{
			if (VectorDistance(nodes[j].origin, alliedDoors[i].origin) > 64)
				continue; // too far away to bother with...

			teamDoorNodes[teamDoorTotal] = j;
			teamDoorTotal++;
		}
	}

	// Now check links to this node and mark them...
	for (i = 0; i < number_of_nodes; i++)
	{
		for (j = 0; j < nodes[i].enodenum; j++)
		{
			for (k = 0; k < teamDoorTotal; k++)
			{
				if (nodes[i].links[j].targetNode == teamDoorNodes[k])
				{// Found a link to our door.. Mark it!
					//G_Printf("node %i, link %i marked as allies only! (TEAM DOOR)\n", i, j);
					nodes[i].links[j].flags & NODE_AXIS_UNREACHABLE;
				}
			}
		}
	}*/
}


/* */
int
BOT_GetTeamDoorNodeAndLink ( gentity_t *bot )
{

	/*int i, j;

	for (i = 0; i < number_of_nodes; i++)
	{
		qboolean good = qfalse;
		vec3_t eyes;
		int goodlink = -1;
		float best_dist = 9999.9f;
		float dist;

		if (VectorDistance(nodes[i].origin, bot->r.currentOrigin) > 512)
			continue;

		if (nodes[i].origin[2] - bot->r.currentOrigin[2] > 96)
			continue;

		dist = VectorDistance(bot->r.currentOrigin, nodes[i].origin);

		if (dist > best_dist)
			continue;

		VectorCopy(bot->r.currentOrigin, eyes);
		eyes[2]+=32;

		if (!OrgVisible(eyes, nodes[i].origin, bot->s.number))
			continue; // This node is not on the other side of the door...

		// See if this node has a team door flag...
		for (j = 0; j < nodes[i].enodenum; j++)
		{
			if ( (bot->client->sess.sessionTeam == TEAM_ALLIES && (nodes[i].links[j].flags & NODE_AXIS_UNREACHABLE) ) 
				|| (bot->client->sess.sessionTeam == TEAM_AXIS && (nodes[i].links[j].flags & NODE_ALLY_UNREACHABLE) ) )
			{
				VectorCopy(nodes[nodes[i].links[j].targetNode].origin, eyes);
				eyes[2]+=32;

				if (OrgVisible(eyes, nodes[i].origin, bot->s.number))
					continue; // This node is not on the other side of the door...

				best_dist = dist;
				good = qtrue;
				goodlink = j;
			}
		}

		if (!good)
			continue;

		// Store the node as the short term goal...
		bot->shortTermGoal = i;

		// Found a good node for using this team door...
		return goodlink;
	}*/
	return ( -1 );
}


/* */
int
GetTeamStartNode ( gentity_t *bot )
{								// Find the closest team node...
	int		i;
	int		best_node = -1;
	float	best_dist = 64.0f;	//256.9f;
	int		flags = -1;
	int		first_node = 0;
	if ( bot->client->sess.sessionTeam == TEAM_AXIS )
	{
		flags = NODE_ALLY_UNREACHABLE;
		first_node = AxisOnlyFirstNode;
	}
	else
	{
		flags = NODE_AXIS_UNREACHABLE;
		first_node = AlliedOnlyFirstNode;
	}

	for ( i = first_node; i < number_of_nodes; i++ )
	{
		float	dist;
		if ( !(nodes[i].type & flags) )
		{
			continue;
		}

		dist = VectorDistance( nodes[i].origin, bot->r.currentOrigin );
		if ( dist < best_dist )
		{
			vec3_t	eyes;
			VectorCopy( bot->r.currentOrigin, eyes );
			eyes[2] += 32;
			if ( OrgVisible( nodes[i].origin, eyes, bot->s.number) )
			{
				best_node = i;
			}
		}
	}

	return ( best_node );
}


/* */
int
GetTeamEndNode ( gentity_t *bot, int start_node )
{		// Find the farthe team node in this chain...
	int		up_best_node = -1;
	int		down_best_node = -1;
	int		flags = -1;
	int		first_node = 0;
	if ( bot->client->sess.sessionTeam == TEAM_AXIS )
	{
		flags = NODE_ALLY_UNREACHABLE;
		first_node = AxisOnlyFirstNode;
	}
	else
	{
		flags = NODE_AXIS_UNREACHABLE;
		first_node = AlliedOnlyFirstNode;
	}

	//if (VectorDistance(bot->r.currentOrigin, nodes[start_node+1].origin) > VectorDistance(bot->r.currentOrigin, nodes[start_node-1].origin))
	{	// Go upwards...
		int upto = start_node + 1;
		while ( 1 )
		{
			upto++;
			if ( !(nodes[upto].type & flags) || VectorDistance( nodes[upto].origin, nodes[upto + 1].origin) > 256 )
			{
				break;
			}
		}

		up_best_node = upto - 1;
	}

	//else
	{	// Go downwards...
		int upto = start_node - 1;
		while ( 1 )
		{
			upto--;
			if ( !(nodes[upto].type & flags) || VectorDistance( nodes[upto].origin, nodes[upto - 1].origin) > 256 )
			{
				break;
			}
		}

		down_best_node = upto + 1;
	}

	if
	(
		VectorDistance( nodes[up_best_node].origin, nodes[bot->longTermGoal].origin) < VectorDistance( nodes[down_best_node].origin,
																									   nodes[bot->longTermGoal].origin)
	)
	{
		return ( up_best_node );
	}
	else
	{
		return ( down_best_node );
	}
}


/* */
int
CreateTeamRoute ( gentity_t *bot, /*short*/ int *pathlist )
{						// Create a route not using links...
	int count = 0;
	int number = 0;
	int upto = 0;
	int from = GetTeamStartNode( bot );
	int to;
	if ( from < 0 )
	{
		return ( -1 );	// None found...
	}

	to = GetTeamEndNode( bot, from );
	if ( to < 0 )
	{
		return ( -1 );	// None found...
	}

	bot->current_node = from;
	if ( to < from )
	{
		if ( from - to > MAX_PATHLIST_NODES )
		{
			return ( -1 );
		}

		for ( upto = from; upto > to; upto-- )
		{
			pathlist[number] = upto;
			number++;
		}

		count = number;
	}
	else
	{
		if ( to - from > MAX_PATHLIST_NODES )
		{
			return ( -1 );
		}

		for ( upto = to; upto > from; upto-- )
		{
			pathlist[number] = upto;
			number++;
		}

		count = number;
	}

	//G_Printf("From = %i. To = %i. Count = %i.\n", from, to, count);
	return ( count - 1 );
}

//
// Visibilities
//


/*
===========================================================================
MyInfront
my modified version of the built-in 'infront' function
I changed it so that we can have the bots field of vision 'fov'
change based on its skill
returns true if 'other' is in the bot's field of vision
===========================================================================
*/
qboolean
MyInfront ( gentity_t *self, gentity_t *other )
{
	vec3_t	vec;
	float	dot;
	vec3_t	forward;
	float	dotConst = 0.0;			//the dotConst values come from this idea - after the dot-product, 90 degrees to either side gives you 0.0

	//directly in front gives you 1.0, and directly behind gives you -1.0 (cosine).
	switch ( self->skillchoice )	//vary the bot's fov by his skill
	{
		case 0:
			dotConst = 0.174;		//160 degrees - like cg_fov 80
			break;
		case 1:
			dotConst = 0.0;			//180 --DEFAULT - like cg_fov 90
			break;
		case 2:
			dotConst = -0.087;		//190 - like cg_fov 95
			break;
		case 3:
			dotConst = -0.216;		//205 - like cg_fov 102.5 - this is closer to a real human's fov, but NOT in a game
			break;
		default:
			dotConst = -0.286;		//205 - like cg_fov 102.5 - this is closer to a real human's fov, but NOT in a game
			break;
	}

	//get the direction in front of the bot
	AngleVectors( self->s.angles, forward, NULL, NULL );

	//get the direction to the 'other'
	VectorSubtract( other->r.currentOrigin, self->r.currentOrigin, vec );

	//make 'vec' (which points from the bot to the 'other') have length 1 (necessary for math...)
	VectorNormalize( vec );

	//essentially get the angle between the bot and the 'other'
	dot = DotProduct( vec, forward );

	//if the 'other' is in the bot's fov, return true (yes, the 'other' is in fron of the bot)
	if ( dot > dotConst )
	{
		return ( qtrue );
	}

	//otherwise return false
	return ( qfalse );
}


/* */
qboolean
MyInFront2 ( vec3_t spot, vec3_t from, vec3_t fromAngles, float threshHold )
{
	vec3_t	dir, forward, angles;
	float	dot;
	VectorSubtract( spot, from, dir );
	dir[2] = 0;
	VectorNormalize( dir );
	VectorCopy( fromAngles, angles );
	angles[0] = 0;
	AngleVectors( angles, forward, NULL, NULL );
	dot = DotProduct( dir, forward );
	return ( dot > threshHold );
}


/* */
void
BOT_GetMuzzlePoint ( gentity_t *bot )
{
	vec3_t	forward, right, up;
	AngleVectors( bot->client->ps.viewangles, forward, right, up );
	CalcMuzzlePoint( bot, bot->client->ps.weapon, forward, right, up, bot->bot_muzzlepoint );
}

#ifdef __BOT_FAST_VISIBILITY__
extern qboolean AIMOD_VISIBILITY_CheckVisibility ( int from, int to );
extern int		AIMOD_VISIBILITY_ClosestNode ( gentity_t *self );
#endif //__BOT_FAST_VISIBILITY__

/* */
int
ShortenASTARRoute ( /*short*/ int *pathlist, int number )
{					// Wrote this to make the routes a little smarter (shorter)... No point looping back to the same places... Unique1
	/*short*/ int	temppathlist[MAX_PATHLIST_NODES];
	int			count = 0;

	//	int count2 = 0;
	int			temp, temp2;
	int			link;
	int			upto = 0;
	for ( temp = number; temp >= 0; temp-- )
	{
		qboolean	shortened = qfalse;
		for ( temp2 = 0; temp2 < temp; temp2++ )
		{
			for ( link = 0; link < nodes[pathlist[temp]].enodenum; link++ )
			{
				if ( nodes[pathlist[temp]].links[link].flags & PATH_BLOCKED )
				{
					continue;
				}

				if ( nodes[pathlist[temp]].links[link].flags & PATH_DANGER )
				{
					continue;
				}

#ifdef __VEHICLES__

				//				if ((bot->client->ps.eFlags & EF_VEHICLE) && nodes[bot->current_node].links[link].flags & PATH_NOTANKS)	//if this path is blocked, skip it
				//					continue;
#endif //__VEHICLES__

				//if (nodes[nodes[pathlist[temp]].links[link].targetNode].origin[2] > nodes[pathlist[temp]].origin[2] + 32)
				//	continue;
				if ( nodes[pathlist[temp]].links[link].targetNode == pathlist[temp2] )
				{	// Found a shorter route...
					//if (OrgVisible(nodes[pathlist[temp2]].origin, nodes[pathlist[temp]].origin, -1))
					{
						temppathlist[count] = pathlist[temp2];
						temp = temp2;
						count++;
						shortened = qtrue;
					}
				}
			}
		}

		if ( !shortened )
		{
			temppathlist[count] = pathlist[temp];
			count++;
		}
	}

	upto = count;

	//pathlist = temppathlist;
	for ( temp = 0; temp < count; temp++ )
	{
		pathlist[temp] = temppathlist[upto];
		upto--;
	}

	//G_Printf("ShortenASTARRoute: Path size reduced from %i to %i nodes...\n", number, count);
	return ( count );
}


/* */
int
CreateDumbRoute ( int from, int to, /*short*/ int *pathlist )
{																// Create a route not using links...
	int count = 0;
	int number = 0;

	//	int node = 0;
	//	int loop = 0;
	int upto = 0;
	if ( to < from )
	{
		if ( from - to > MAX_PATHLIST_NODES )
		{
			return ( -1 );
		}

		for ( upto = from; upto > to; upto-- )
		{
			if ( upto != to && nodes[upto].links[0].targetNode != pathlist[number - 1] )
			{													// Not sequential!
				count = 0;
				break;
			}

			pathlist[number] = upto;
			number++;
		}

		count = number;
	}
	else
	{
		if ( to - from > MAX_PATHLIST_NODES )
		{
			return ( -1 );
		}

		for ( upto = to; upto > from; upto-- )
		{
			if ( upto != from && nodes[pathlist[number - 1]].links[0].targetNode != upto )
			{													// Not sequential!
				count = 0;
				break;
			}

			pathlist[number] = upto;
			number++;
		}

		count = number;
	}

	/*	G_Printf("**PATH**\n");
	for (loop = 0; loop < count; loop++)
	{
		G_Printf("%i\n", pathlist[loop]);
	}
	G_Printf("**END**\n");*/
	return ( count - 1 );
}

/*
===========================================================================
CreatePathAStar
This function uses the A* pathfinding algorithm to determine the
shortest path between any two nodes.
It's fairly complex, so I'm not really going to explain it much.
Look up A* and binary heaps for more info.
pathlist stores the ideal path between the nodes, in reverse order,
and the return value is the number of nodes in that path
===========================================================================
*/
int GetFCost ( gentity_t *bot, int to, int num, int parentNum, float *gcost );	// Below...

/*
=========================
START A* Pathfinding Code
=========================
*/
typedef struct nodeWaypoint_s
{
	int wpNum;
	int f;
	int g;
	int h;
	int pNum;
} nodeWaypoint_t;

//Don't use the 0 slot of this array.  It's a binary heap and it's based on 1 being the first slot.
nodeWaypoint_t	OpenList[MAX_NODES + 1];
nodeWaypoint_t	CloseList[MAX_NODES];


/* */
qboolean
OpenListEmpty ( void )
{

	//since we're using a binary heap, in theory, if the first slot is empty, the heap
	//is empty.
	if ( OpenList[1].wpNum != -1 )
	{
		return ( qfalse );
	}

	return ( qtrue );
}


//Scans for the given wp on the Open List and returns it's OpenList position.

//Returns -1 if not found.
int
FindOpenList ( int wpNum )
{
	int i;
	for ( i = 1; i < MAX_NODES + 1 && OpenList[i].wpNum != -1; i++ )
	{
		if ( OpenList[i].wpNum == wpNum )
		{
			return ( i );
		}
	}

	return ( -1 );
}


//Scans for the given wp on the Close List and returns it's CloseList position.

//Returns -1 if not found.
int
FindCloseList ( int wpNum )
{
	int i;
	for ( i = 0; i < MAX_NODES && CloseList[i].wpNum != -1; i++ )
	{
		if ( CloseList[i].wpNum == wpNum )
		{
			return ( i );
		}
	}

	return ( -1 );
}

//Maximum distance allowed between nodes for them to count as seqencial wp.
#define MAX_NODE_DIS	512 //1000 //512 //250


//Add this wpNum to the open list.
void
AddOpenList ( gentity_t *bot, int wpNum, int parent, int end )
{
	int i;
	int f;
	int g = 0;
	int h;

	if ( parent == -1 )
	{	//This is the start point, just use zero for g
		g = 0;
	}
	else if ( wpNum == parent + 1 )
	{
		i = FindCloseList( parent );
		if ( i != -1 )
		{
			g = CloseList[i].g + VectorDistance( nodes[parent].origin, nodes[wpNum].origin );
		}
		else
		{
			G_Printf( "Couldn't find parent on CloseList in AddOpenList().  This shouldn't happen.\n" );
		}
	}
	else if ( wpNum == parent - 1 )
	{
		i = FindCloseList( parent );
		if ( i != -1 )
		{
			g = CloseList[i].g + VectorDistance( nodes[wpNum].origin, nodes[wpNum + 1].origin );
		}
		else
		{
			G_Printf( "Couldn't find parent on CloseList in AddOpenList().  This shouldn't happen.\n" );
		}
	}
	else
	{	//nonsequencal parent/wpNum
		i = FindCloseList( parent );
		if ( i != -1 )
		{
			g = OpenList[i].g + VectorDistance( nodes[wpNum].origin, nodes[parent].origin );
		}
		else
		{
			G_Printf( "Couldn't find parent on CloseList in AddOpenList().  This shouldn't happen.\n" );
		}
	}

	//Find first open slot or the slot that this wpNum already occupies.
	for ( i = 1; i < MAX_NODES + 1 && OpenList[i].wpNum != -1; i++ )
	{
		if ( OpenList[i].wpNum == wpNum )
		{
			break;
		}
	}

	h = VectorDistance( nodes[wpNum].origin, nodes[end].origin );
	f = g + h;
	OpenList[i].f = f;
	OpenList[i].g = g;
	OpenList[i].h = h;
	OpenList[i].pNum = parent;
	OpenList[i].wpNum = wpNum;
	while ( OpenList[i].f <= OpenList[i / 2].f && i != 1 )
	{	//binary parent has higher f than child
		int ftemp = OpenList[i / 2].f;
		int gtemp = OpenList[i / 2].g;
		int htemp = OpenList[i / 2].h;
		int pNumtemp = OpenList[i / 2].pNum;
		int wptemp = OpenList[i / 2].wpNum;

		//G_Printf("AddOpenList\n");
		OpenList[i / 2].f = OpenList[i].f;
		OpenList[i / 2].g = OpenList[i].g;
		OpenList[i / 2].h = OpenList[i].h;
		OpenList[i / 2].pNum = OpenList[i].pNum;
		OpenList[i / 2].wpNum = OpenList[i].wpNum;
		OpenList[i].f = ftemp;
		OpenList[i].g = gtemp;
		OpenList[i].h = htemp;
		OpenList[i].pNum = pNumtemp;
		OpenList[i].wpNum = wptemp;
		i = i / 2;
	}

	return;
}


//Remove the first element from the OpenList.
void
RemoveFirstOpenList ( void )
{
	int i = 0;
	for ( i = 1; i < MAX_NODES + 1 && OpenList[i].wpNum != -1; i++ )
	{

	}

	i = i--;
	if ( OpenList[i].wpNum == -1 )
	{
		G_Printf( "Crap!  The last entry scanner in RemoveFirstOpenList() totally failed.\n" );
	}

	if ( OpenList[1].wpNum == OpenList[i].wpNum )
	{	//the first slot is the only thing on the list. blank it.
		OpenList[1].f = -1;
		OpenList[1].g = -1;
		OpenList[1].h = -1;
		OpenList[1].pNum = -1;
		OpenList[1].wpNum = -1;
		return;
	}

	//shift last entry to start
	OpenList[1].f = OpenList[i].f;
	OpenList[1].g = OpenList[i].g;
	OpenList[1].h = OpenList[i].h;
	OpenList[1].pNum = OpenList[i].pNum;
	OpenList[1].wpNum = OpenList[i].wpNum;
	OpenList[i].f = -1;
	OpenList[i].g = -1;
	OpenList[i].h = -1;
	OpenList[i].pNum = -1;
	OpenList[i].wpNum = -1;
	while
	(
		(OpenList[i].f >= OpenList[i * 2].f && OpenList[i * 2].wpNum != -1) ||
		(OpenList[i].f >= OpenList[i * 2 + 1].f && OpenList[i * 2 + 1].wpNum != -1)
	)
	{

		//		G_Printf("RemoveFirstOpenList\n");
		if ( (OpenList[i * 2].f < OpenList[i * 2 + 1].f) || OpenList[i * 2 + 1].wpNum == -1 )
		{
			int ftemp = OpenList[i * 2].f;
			int gtemp = OpenList[i * 2].g;
			int htemp = OpenList[i * 2].h;
			int pNumtemp = OpenList[i * 2].pNum;
			int wptemp = OpenList[i * 2].wpNum;
			OpenList[i * 2].f = OpenList[i].f;
			OpenList[i * 2].g = OpenList[i].g;
			OpenList[i * 2].h = OpenList[i].h;
			OpenList[i * 2].pNum = OpenList[i].pNum;
			OpenList[i * 2].wpNum = OpenList[i].wpNum;
			OpenList[i].f = ftemp;
			OpenList[i].g = gtemp;
			OpenList[i].h = htemp;
			OpenList[i].pNum = pNumtemp;
			OpenList[i].wpNum = wptemp;
			i = i * 2;
		}
		else if ( OpenList[i * 2 + 1].wpNum != -1 )
		{
			int ftemp = OpenList[i * 2 + 1].f;
			int gtemp = OpenList[i * 2 + 1].g;
			int htemp = OpenList[i * 2 + 1].h;
			int pNumtemp = OpenList[i * 2 + 1].pNum;
			int wptemp = OpenList[i * 2 + 1].wpNum;
			OpenList[i * 2 + 1].f = OpenList[i].f;
			OpenList[i * 2 + 1].g = OpenList[i].g;
			OpenList[i * 2 + 1].h = OpenList[i].h;
			OpenList[i * 2 + 1].pNum = OpenList[i].pNum;
			OpenList[i * 2 + 1].wpNum = OpenList[i].wpNum;
			OpenList[i].f = ftemp;
			OpenList[i].g = gtemp;
			OpenList[i].h = htemp;
			OpenList[i].pNum = pNumtemp;
			OpenList[i].wpNum = wptemp;
			i = i * 2 + 1;
		}
		else
		{
			G_Printf( "Something went wrong in RemoveFirstOpenList().\n" );
			return;
		}
	}

	//sorting complete
	return;
}


//Adds a given OpenList wp to the closed list
void
AddCloseList ( int openListpos )
{
	int i;
	if ( OpenList[openListpos].wpNum != -1 )
	{
		for ( i = 0; i < MAX_NODES; i++ )
		{
			if ( CloseList[i].wpNum == -1 )
			{	//open slot, fill it.  heheh.
				CloseList[i].f = OpenList[openListpos].f;
				CloseList[i].g = OpenList[openListpos].g;
				CloseList[i].h = OpenList[openListpos].h;
				CloseList[i].pNum = OpenList[openListpos].pNum;
				CloseList[i].wpNum = OpenList[openListpos].wpNum;
				return;
			}
		}

		G_Printf( "CloseList was full in AddCloseList().  This shouldn't happen.\n" );
		return;
	}

	G_Printf( "Bad openListpos given to AddCloseList().\n" );
	return;
}


//Clear out the Route
void
ClearRoute ( /*short*/ int *Route )
{
	int i;
	for ( i = 0; i < MAX_PATHLIST_NODES; i++ )
	{
		Route[i] = -1;
	}
}


/* */
void
AddtoRoute ( int wpNum, /*short*/ int *Route )
{
	int i;
	for ( i = 0; i < MAX_PATHLIST_NODES && Route[i] != -1; i++ )
	{

	}

	if ( Route[i] == -1 && i < MAX_PATHLIST_NODES )
	{	//found the first empty slot
		while ( i > 0 )
		{
			Route[i] = Route[i - 1];
			i--;
		}
	}
	else
	{
		G_Printf( "Empty Route slot couldn't be found in AddtoRoute.  This shouldn't happen.\n" );
		return;
	}

	if ( i == 0 )
	{
		Route[0] = wpNum;
	}
}


//find a given wpNum on the given route and return it's address.  return -1 if not on route.

//use wpNum = -1 to find the last wp on route.
int
FindOnRoute ( int wpNum, /*short*/ int *Route )
{
	int i;
	for ( i = 0; i < MAX_PATHLIST_NODES && Route[i] != wpNum; i++ )
	{

	}

	//Special find end route command stuff
	if ( wpNum == -1 )
	{
		i = i--;
		if ( Route[i] != -1 )
		{	//found it
			return ( i );
		}

		//otherwise, this is a empty route list
		return ( -1 );
	}

	if ( wpNum == Route[i] )
	{		//Success!
		return ( i );
	}

	//Couldn't find it
	return ( -1 );
}


//Copy Route
void
CopyRoute ( /*short*/ int *routesource, /*short*/ int *routedest )
{
	int i;
	for ( i = 0; i < MAX_NODES; i++ )
	{
		routedest[i] = routesource[i];
	}
}


/* */
int
Route_Count ( /*short*/ int *route )
{
	int count = 0;
	while ( route[count] != -1 )
	{

		//if (count < 100)
		//	G_Printf("%i - %i.\n", count, route[count]);
		count++;
	}

	//	assert(0);
	return ( count );
}


//Find the ideal (shortest) route between the start wp and the end wp
//badwp is for situations where you need to recalc a path when you dynamically discover
//that a wp is bad (door locked, blocked, etc).

//doRoute = actually set botRoute
float
FindIdealPathtoWP ( gentity_t *bot, int start, int end, int badwp, /*short*/ int *Route )
{
	int		i;
	float	dist = -1;
	if ( end == -1 )
	{
		return ( -1 );
	}

	if ( start == end )
	{
		ClearRoute( Route );
		AddtoRoute( end, Route );
		return ( 0 );
	}

	//reset node lists
	for ( i = 0; i < MAX_NODES; i++ )
	{
		OpenList[i].wpNum = -1;
		OpenList[i].f = -1;
		OpenList[i].g = -1;
		OpenList[i].h = -1;
		OpenList[i].pNum = -1;
		CloseList[i].wpNum = -1;
		CloseList[i].f = -1;
		CloseList[i].g = -1;
		CloseList[i].h = -1;
		CloseList[i].pNum = -1;
	}

	AddOpenList( bot, start, -1, end );

	//G_Printf("FindIdealPathtoWP (start %i - end %i)\n", start, end);
	while ( !OpenListEmpty() && FindOpenList( end) == -1 )
	{								//open list not empty
		//we're using a binary pile so the first slot always has the lowest f score
		AddCloseList( 1 );
		i = OpenList[1].wpNum;
		RemoveFirstOpenList();

		//Add surrounding nodes
#ifdef __ASTAR_EXTRAS__
		if ( i > 0 && i + 1 <= number_of_nodes && nodes[i + 1].enodenum /*&& nodes[i+1]->inuse*/ )
		{
			if
			(
				VectorDistance( nodes[i].origin, nodes[i + 1].origin) < MAX_NODE_DIS &&
				FindCloseList( i + 1) == -1 &&
				i + 1 != badwp
			)
			{						//Add next sequencial node
				AddOpenList( bot, i + 1, i, end );
			}
		}

		if ( i > 0 )
		{
			if
			(
				VectorDistance( nodes[i - 1].origin, nodes[i].origin) < MAX_NODE_DIS &&
				nodes[i - 1].enodenum /*&& nodes[i-1]->inuse*/ &&
				FindCloseList( i - 1) == -1 &&
				i - 1 != badwp
			)
			{						//Add previous sequencial node
				AddOpenList( bot, i - 1, i, end );
			}
		}
#endif //__ASTAR_EXTRAS__

		//if(nodes[i].links[1].targetNode >= 0)
		{							//add all the wp's neighbors to the list
			int x;
			for ( x = 0; x < nodes[i].enodenum /*MAX_NODELINKS*/ ; x++ )
			{
				if ( nodes[i].links[x].targetNode < 0 )
				{
					break;
				}

				if ( VectorDistance( nodes[i].origin, nodes[nodes[i].links[x].targetNode].origin) > 48 )
				{
					continue;
				}

				if ( nodes[i].links[x].flags & PATH_BLOCKED )
				{					//if this path is blocked, skip it
					continue;
				}

				if ( nodes[i].links[x].flags & PATH_DANGER )
				{					//if this path is blocked, skip it
					continue;
				}

#ifdef __VEHICLES__
				if ( bot->client && (bot->client->ps.eFlags & EF_VEHICLE) && nodes[i].links[x].flags & PATH_NOTANKS )
				{					//if this path is blocked, skip it
					continue;
				}

				if ( !(nodes[bot->current_node].type & NODE_ICE) )
				{
					if
					(
						bot->client &&
						(bot->client->ps.eFlags & EF_VEHICLE) &&
						(nodes[nodes[i].links[x].targetNode].type & NODE_ICE)
					)
					{
						continue;	// Tank driving bots should NEVER go onto ice!
					}
				}
#endif //__VEHICLES__

				//skip any unreachable nodes
				if
				(
					bot->client &&
					(nodes[nodes[i].links[x].targetNode].type & NODE_ALLY_UNREACHABLE) &&
					(bot->client->sess.sessionTeam == TEAM_ALLIES)
				)
				{
					continue;
				}

				if
				(
					bot->client &&
					(nodes[nodes[i].links[x].targetNode].type & NODE_AXIS_UNREACHABLE) &&
					(bot->client->sess.sessionTeam == TEAM_AXIS)
				)
				{
					continue;
				}

				if ( x != badwp && FindCloseList( nodes[i].links[x].targetNode) == -1 )
				{

					//G_Printf("AddOpenList\n");
					AddOpenList( bot, nodes[i].links[x].targetNode, i, end );
				}
			}
		}
	}

	i = FindOpenList( end );

	//G_Printf("FindOpenList is %i\n", i);
	if ( i != -1 )
	{								//we have a valid route to the end point
		ClearRoute( Route );

		//G_Printf("AddtoRoute\n");
		AddtoRoute( end, Route );
		dist = OpenList[i].g;
		i = OpenList[i].pNum;
		i = FindCloseList( i );
		while ( i != -1 )
		{

			//G_Printf("AddtoRoute2\n");
			AddtoRoute( CloseList[i].wpNum, Route );
			i = FindCloseList( CloseList[i].pNum );
		}

		//assert(0);
		return ( dist );
	}

	return ( -1 );
}


/* */
void
BOT_MakeLinkUnreachable ( gentity_t *bot, int node, int linkNum )
{

	//G_Printf("Bot %s has marked node %i, link %i as unreachable.\n", bot->client->pers.netname, node, linkNum);
#ifdef __VEHICLES__
	if ( bot->client && bot->client->ps.eFlags & EF_VEHICLE )
	{
		if ( !(nodes[node].links[linkNum].flags & PATH_NOTANKS) )
		{
			nodes[node].links[linkNum].flags |= PATH_NOTANKS;
		}

		return;
	}
#endif //__VEHICLES__
	if ( !(nodes[node].links[linkNum].flags & PATH_BLOCKED) )
	{
		nodes[node].links[linkNum].flags |= PATH_BLOCKED;
	}
}

/* */
void
BOT_MakeLinkSemiUnreachable ( gentity_t *bot, int node, int linkNum )
{// Set a really really high cost!
	nodes[node].links[linkNum].cost = 60000.0f;
}

void
BOT_IncreaseLinkCost ( gentity_t *bot, int node, int linkNum )
{// Set a really really high cost!
	if (nodes[node].links[linkNum].cost < 1000) // Set it fairly high at the start!
		nodes[node].links[linkNum].cost = 1000.0f;

	nodes[node].links[linkNum].cost *= 2;
}

/* */
void
BOT_MakeLinkDangerous ( gentity_t *bot, int node, int linkNum )
{
	if ( !(nodes[node].links[linkNum].flags & PATH_DANGER) )
	{
		nodes[node].links[linkNum].flags |= PATH_DANGER;
		nodes[node].links[linkNum].cost = 60000.0f;
	}
}

//
// The utilities for faster (then vector/float) integer maths...
//
typedef long int	intvec_t;
typedef intvec_t	intvec3_t[3];
extern void			intToVectorCopy ( const intvec3_t in, vec3_t out );
extern void			intVectorCopy ( const intvec3_t in, intvec3_t out );
extern long int		intVectorDistance ( intvec3_t v1, intvec3_t v2 );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodeWithIgnore ( gentity_t *bot, int ignore, int r, int type );
extern int			NodeVisible ( vec3_t org1, vec3_t org2, int ignore );


/* */
float
VectorDistanceNoHeight ( vec3_t v1, vec3_t v2 )
{
	vec3_t	dir;
	vec3_t	v1a, v2a;
	VectorCopy( v1, v1a );
	VectorCopy( v2, v2a );
	v2a[2] = v1a[2];
	VectorSubtract( v2a, v1a, dir );
	return ( VectorLength( dir) );
}


/* */
float
HeightDistance ( vec3_t v1, vec3_t v2 )
{
	vec3_t	dir;
	vec3_t	v1a, v2a;
	VectorCopy( v1, v1a );
	VectorCopy( v2, v2a );
	v2a[0] = v1a[0];
	v2a[1] = v1a[1];
	VectorSubtract( v2a, v1a, dir );
	return ( VectorLength( dir) );
}


/* */
qboolean
BAD_WP_Height ( vec3_t start, vec3_t end )
{
	/*float distance = VectorDistanceNoHeight(start, end);
	float height_diff = HeightDistance(start, end);

	if (distance > 8)
	{// < 8 is probebly a ladder...
		if ((start[2] + 32 < end[2]) 
			&& (height_diff*1.5) > distance 
			&& distance > 48)
			return qtrue;

		if ((start[2] < end[2] + 48) 
			&& (height_diff*1.5) > distance 
			&& distance > 48)
			return qtrue;
	}*/

	return ( qfalse );
}


/* */
int
AIMod_Wander_Pathing ( gentity_t *bot, int from, int to, /*short*/ int *pathlist )
{
	int pathsize = 0;
	int j;
	int i;
	int start_time = trap_Milliseconds();																// We will time limit this and see how far they get...

	/*float best_dist = 9999.0;
	int best_ent = -1;
	int best_start = -1;

	// First, check and see if any friendly bots have found their way to a good spot.. And copy their path if they have...
	for (j = 0; j < MAX_CLIENTS; j++)
	{
		gentity_t *ent = &g_entities[j];
		float dist;

		if (!ent)
			continue;

		if (!ent->client)
			continue;

		if (bot == ent)
			continue;

		if (!OnSameTeam(bot, ent))
			continue;

		if (!(ent->r.svFlags & SVF_BOT))
			continue;

		if (ent->pathsize <= 0)
			continue;

		if (VectorDistance(bot->r.currentOrigin, ent->r.currentOrigin) < 512)
			continue;

		for (i = ent->pathsize; i > 0; i--)
		{
			if (VectorDistance(bot->r.currentOrigin, nodes[ent->pathlist[i]].origin) < 256)
			{
				if (NodeVisible(bot->r.currentOrigin, nodes[ent->pathlist[i]].origin, bot->s.number) >= 1)
				{
					best_start = i;
					break;
				}
			}
		}

		if (best_start < 0)
			continue;

		dist = VectorDistance(nodes[ent->pathlist[0]].origin, nodes[ent->pathlist[ent->pathsize]].origin);

		if (dist > best_dist)
		{
			best_dist = dist;
			best_ent = j;
		}
	}

	if (best_ent >= 0)
	{// We found someone to copy!
		gentity_t *ent = &g_entities[best_ent];

		memcpy(bot->pathlist, ent->pathlist, sizeof(ent->pathlist));
		
		bot->pathsize = best_start;

		G_Printf("%s copied their path from %s.\n", bot->client->pers.netname, ent->client->pers.netname);
		return -2; // Marks that we copied someone!
	}*/
	memset( pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
	while ( 1 )
	{
		if ( nodes[from].enodenum - 1 <= 0 )
		{																								// Bad node, no links.. Pick another...
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNodeWithIgnore( bot, from, NODE_DENSITY,
																					 NODEFIND_ALL );
			from = bot->current_node;
		}

		pathlist[pathsize] = nodes[from].links[Q_TrueRand( 0, nodes[from].enodenum - 1 )].targetNode;
		if ( nodes[pathlist[pathsize]].enodenum - 1 > 0 )
		{
			break;
		}
	}

	pathsize++;
	for ( j = 0; j < 512 /*128*/ ; j++ )
	{
		int choice;
		while ( 1 )
		{
			qboolean	bad = qfalse;
			int			flags;
			choice = Q_TrueRand( 0, nodes[pathlist[pathsize - 1]].enodenum - 1 );
			pathlist[pathsize] = nodes[pathlist[pathsize - 1]].links[choice].targetNode;
			flags = nodes[pathlist[pathsize - 1]].links[choice].flags;
			if ( nodes[pathlist[pathsize]].enodenum - 1 <= 0 )
			{
				continue;
			}

			if ( flags & PATH_BLOCKED )
			{
				continue;
			}

			if ( flags & PATH_DANGER )
			{
				continue;
			}

#ifdef __VEHICLES__
			if ( bot->client && (bot->client->ps.eFlags & EF_VEHICLE) && flags & PATH_NOTANKS )
			{																							//if this path is blocked, skip it
				continue;
			}
#endif //__VEHICLES__
			if ( BAD_WP_Height( nodes[pathlist[pathsize - 1]].origin, nodes[pathlist[pathsize]].origin) )
			{
				continue;
			}

			for ( i = 0; i < pathsize - 1; i++ )
			{
				if
				(
					pathlist[i] == pathlist[pathsize] ||
					VectorDistance( nodes[pathlist[i]].origin, nodes[pathlist[pathsize]].origin) < 256	/*128*/
				)
				{
					bad = qtrue;
					break;
				}
			}

			if ( bad )
			{
				if ( trap_Milliseconds() - start_time > 2 /*5*/ /*25*/ )
				{
					return ( pathsize - 1 );
				}

				continue;
			}

			if ( nodes[pathlist[pathsize]].enodenum - 1 > 0 )
			{
				break;
			}
		}

		pathsize++;
	}

	return ( pathsize - 1 );
}

qboolean	map_has_sequential_nodes = qfalse;
qboolean	map_sequential_checked = qfalse;


/* */
void
CheckSequentialNodes ( void )
{
	int i, j;
	int consecutive_nodes = 0;

	//int tries = 0;
	map_sequential_checked = qtrue;
	for ( i = 0; i < number_of_nodes; i++ )
	{
		qboolean	good = qfalse;
		for ( j = 0; j < nodes[i].enodenum; j++ )
		{
			if ( nodes[i].links[j].targetNode == i + 1 )
			{
				good = qtrue;
			}
		}

		if ( good )
		{
			consecutive_nodes++;
			if ( consecutive_nodes > number_of_nodes * 0.9 /*0.6*/ /*0.4*/ /*0.85*/ /*/5*/ /*128*/ )
			{
				map_has_sequential_nodes = qtrue;
				G_Printf( "^4*** ^3%s^5: Map supports sequential bot waypointing.\n", GAME_VERSION );
				return;
			}
		}

		/*else
			tries++;

		if (tries > 5)
		{
			G_Printf("^4*** ^3%s^5: Map does not support sequential bot waypointing (Must be using auto-generated waypoints).\n", GAME_VERSION);
			return;
		}*/
	}

	G_Printf( "^4*** ^3%s^5: Map does not support sequential bot waypointing (Must be using auto-generated waypoints).\n",
			  GAME_VERSION );
}


/* */
qboolean
HaveSequentialNodes ( void )
{
	if ( !map_sequential_checked )
	{
		CheckSequentialNodes();
	}

	return ( map_has_sequential_nodes );
}

extern int TankNodeVisible ( vec3_t org1, vec3_t org2, vec3_t mins, vec3_t maxs, int ignore );

/*
=========================
END A* Pathfinding Code
=========================
*/

extern qboolean SSE_CPU;

#define NODE_INVALID -1
#define MAX_NODELINKS 32

int			*openlist;												//add 1 because it's a binary heap, and they don't use 0 - 1 is the first used index
float		*gcost;
int			*fcost;
char		*list;														//0 is neither, 1 is open, 2 is closed - char because it's the smallest data type
int			*parent;

qboolean PATHFINDING_MEMORY_ALLOCATED = qfalse;

void AllocatePathFindingMemory()
{
	if (PATHFINDING_MEMORY_ALLOCATED) return;

	openlist = malloc(sizeof(int)*(MAX_NODES));
	gcost = malloc(sizeof(float)*(MAX_NODES));
	fcost = malloc(sizeof(int)*(MAX_NODES));
	list = malloc(sizeof(char)*(MAX_NODES));
	parent = malloc(sizeof(int)*(MAX_NODES));

	PATHFINDING_MEMORY_ALLOCATED = qtrue;
}

#ifndef __CUDA__
int CreatePathAStar ( gentity_t *bot, int from, int to, /*short*/ int *pathlist )
{
	//all the data we have to hold...since we can't do dynamic allocation, has to be MAX_NODES
	//we can probably lower this later - eg, the open list should never have more than at most a few dozen items on it
	int			numOpen = 0;
	int			atNode, temp, newnode = -1;
	qboolean	found = qfalse;
	int			count = -1;
	float		gc;
	int			i, j, u, v, m;
	vec3_t		vec;

	if ( (from == NODE_INVALID) || (to == NODE_INVALID) || (from >= MAX_NODES) || (to >= MAX_NODES) || (from == to) )
	{
		return ( -1 );
	}

	// Check if memory needs to be allocated...
	AllocatePathFindingMemory();

	//clear out all the arrays - UQ1: Added - only allocate total nodes for map for speed...
	if (SSE_CPU)
	{
		sse_memset( openlist, 0, ((sizeof(int)) * (number_of_nodes + 1))/8 );
		sse_memset( fcost, 0, ((sizeof(int)) * number_of_nodes)/8 );
		sse_memset( list, 0, ((sizeof(char)) * number_of_nodes)/8 );
		sse_memset( parent, 0, ((sizeof(int)) * number_of_nodes)/8 );
		sse_memset( gcost, 0, ((sizeof(float)) * number_of_nodes)/8 );
	}
	else
	{
		memset( openlist, 0, ((sizeof(int)) * (number_of_nodes + 1)) );
		memset( fcost, 0, ((sizeof(int)) * number_of_nodes) );
		memset( list, 0, ((sizeof(char)) * number_of_nodes) );
		memset( parent, 0, ((sizeof(int)) * number_of_nodes) );
		memset( gcost, 0, ((sizeof(float)) * number_of_nodes) );
	}
	
	for (i = 0; i < number_of_nodes; i++)
	{
		gcost[i] = Distance(nodes[i].origin, nodes[to].origin); 
	}

	openlist[number_of_nodes + 1] = 0;

	openlist[1] = from;																	//add the starting node to the open list
	numOpen++;
	gcost[from] = 0;																	//its f and g costs are obviously 0
	fcost[from] = 0;

	while ( 1 )
	{
		if ( numOpen != 0 )																//if there are still items in the open list
		{
			//pop the top item off of the list
			atNode = openlist[1];
			list[atNode] = 2;															//put the node on the closed list so we don't check it again
			numOpen--;
			openlist[1] = openlist[numOpen + 1];										//move the last item in the list to the top position
			v = 1;

			//this while loop reorders the list so that the new lowest fcost is at the top again
			while ( 1 )
			{
				u = v;
				if ( (2 * u + 1) < numOpen )											//if both children exist
				{
					if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
					{
						v = 2 * u;
					}

					if ( fcost[openlist[v]] >= fcost[openlist[2 * u + 1]] )
					{
						v = 2 * u + 1;
					}
				}
				else
				{
					if ( (2 * u) < numOpen )											//if only one child exists
					{
						if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
						{
							v = 2 * u;
						}
					}
				}

				if ( u != v )															//if they're out of order, swap this item with its parent
				{
					temp = openlist[u];
					openlist[u] = openlist[v];
					openlist[v] = temp;
				}
				else
				{
					break;
				}
			}

			if (nodes[atNode].enodenum <= MAX_NODELINKS)
			for ( i = 0; i < nodes[atNode].enodenum; i++ )								//loop through all the links for this node
			{
				newnode = nodes[atNode].links[i].targetNode;

				if (newnode > number_of_nodes)
					continue;

				if (newnode < 0)
					continue;

				if ( nodes[atNode].links[i].flags & PATH_BLOCKED )
				{																		//if this path is blocked, skip it
					continue;
				}

				if ( nodes[atNode].links[i].flags & PATH_DANGER )
				{																		//if this path is blocked, skip it
					continue;
				}

/*#ifdef __NPC__
				if ( bot->s.eType == ET_NPC && nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE )
				{																		//if this path is blocked, skip it
					continue;
				}
#endif //__NPC__*/

#ifdef __VEHICLES__
				/*if
				(
					bot->client &&
					!(bot->client->ps.eFlags & EF_VEHICLE) &&
					(nodes[nodes[atNode].links[i].targetNode].type & NODE_LAND_VEHICLE)
				)
				{
					continue;															// Non-Tank driving bots should NOT use tank nodes!
				}*/

				if (bot->client)
				{
					if (bot->client->ps.eFlags & EF_VEHICLE)
					{
						// Check == first for speed...
						if (nodes[atNode].links[i].flags == PATH_NOTANKS)
						{//if this path is blocked, skip it
							continue;
						}

						if (nodes[atNode].links[i].flags & PATH_NOTANKS)
						{//if this path is blocked, skip it
							continue;
						}

						// Check == first for speed...
						if (nodes[nodes[atNode].links[i].targetNode].type == NODE_LAND_VEHICLE)
						{// Tank driving bots should only use tank nodes!
						
						}
						else if (!(nodes[nodes[atNode].links[i].targetNode].type & NODE_LAND_VEHICLE))
						{// Tank driving bots should only use tank nodes!
							continue;
						}

						// Check == first for speed...
						if (!(nodes[bot->current_node].type == NODE_ICE))
						{
							if (nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE)
							{// Avoid ice!
								continue;
							}

							if (nodes[nodes[atNode].links[i].targetNode].type & NODE_ICE)
							{// Avoid ice!
								continue;
							}
						}

						if (!(nodes[bot->current_node].type & NODE_ICE))
						{// Check == first for speed...
							if (nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE)
							{// Avoid ice!
								continue;
							}

							if (nodes[nodes[atNode].links[i].targetNode].type & NODE_ICE)
							{// Avoid ice!
								continue;
							}
						}
					}
				}

#endif //__VEHICLES__
				/*if ( BAD_WP_Height( nodes[atNode].origin, nodes[newnode].origin) )
				{																		// Height diff is greater then plane distance... Block it and skip it..
					if (bot->client)
						BOT_MakeLinkSemiUnreachable( bot, atNode, i );

					continue;
				}*/

				//skip any unreachable nodes
				if (bot && bot->client)
				{
					// UQ1: Added for faster check the ==
					if (nodes[newnode].type == NODE_AXIS_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_AXIS)						
						{
							continue;
						}
					}

					if (nodes[newnode].type == NODE_ALLY_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_ALLIES)	
						{
							continue;
						}
					}

					if (nodes[newnode].type & NODE_AXIS_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_AXIS)						
						{
							continue;
						}
					}

					if (nodes[newnode].type & NODE_ALLY_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_ALLIES)	
						{
							continue;
						}
					}
				}

/*#ifdef __VEHICLES__
				if (bot->client 
					&& (bot->client->ps.eFlags & EF_VEHICLE) )
				{
					if (Tank_PositionVisible(nodes[atNode].origin, nodes[newnode].origin, bot->s.number) != 1)
						continue;
				}
#endif //__VEHICLES__*/

				if ( list[newnode] == 2 )
				{																		//if this node is on the closed list, skip it
					continue;
				}

				/*if (VectorDistance(nodes[from].origin, nodes[newnode].origin) < 1024)
				{// UQ1: Can avoid this check if it is a long way away...
					if (AIMOD_MOVEMENT_Explosive_Near_Waypoint(newnode))					
					{// UQ1: If explosive near it. Skip it!
						continue;
					}
				}*/

				//if (AIMOD_MOVEMENT_Explosive_Near_Waypoint(newnode) 
				//	&& VectorDistance(bot->r.currentOrigin, nodes[newnode].origin) > 380)
				//	continue;

				if ( list[newnode] != 1 )												//if this node is not already on the open list
				{
					openlist[++numOpen] = newnode;										//add the new node to the open list
					list[newnode] = 1;
					parent[newnode] = atNode;											//record the node's parent
					
					if ( newnode == to )
					{																	//if we've found the goal, don't keep computing paths!
						break;															//this will break the 'for' and go all the way to 'if (list[to] == 1)'
					}

					fcost[newnode] = GetFCost( bot, to, newnode, parent[newnode], gcost );	//store it's f cost value

					//this loop re-orders the heap so that the lowest fcost is at the top
					m = numOpen;

					while ( m != 1 )													//while this item isn't at the top of the heap already
					{
						if ( fcost[openlist[m]] <= fcost[openlist[m / 2]] )				//if it has a lower fcost than its parent
						{
							temp = openlist[m / 2];
							openlist[m / 2] = openlist[m];
							openlist[m] = temp;											//swap them
							m /= 2;
						}
						else
						{
							break;
						}
					}
				}
				else										//if this node is already on the open list
				{
					gc = gcost[atNode];
					//VectorSubtract( nodes[newnode].origin, nodes[atNode].origin, vec );
					//gc += VectorLength( vec );				//calculate what the gcost would be if we reached this node along the current path
					if (nodes[atNode].links[i].cost)
					{// UQ1: Already have a cost value, skip the calculations!
						gc += nodes[atNode].links[i].cost;
					}
					else
					{
						VectorSubtract( nodes[newnode].origin, nodes[atNode].origin, vec );
						gc += VectorLength( vec );				//calculate what the gcost would be if we reached this node along the current path
						nodes[atNode].links[i].cost = gc;
					}

					if ( gc < gcost[newnode] )				//if the new gcost is less (ie, this path is shorter than what we had before)
					{
						parent[newnode] = atNode;			//set the new parent for this node
						gcost[newnode] = gc;				//and the new g cost

						for ( j = 1; j < numOpen; j++ )		//loop through all the items on the open list
						{
							if ( openlist[j] == newnode )	//find this node in the list
							{
								//calculate the new fcost and store it
								fcost[newnode] = GetFCost( bot, to, newnode, parent[newnode], gcost );

								//reorder the list again, with the lowest fcost item on top
								m = j;

								while ( m != 1 )
								{
									if ( fcost[openlist[m]] < fcost[openlist[m / 2]] )	//if the item has a lower fcost than it's parent
									{
										temp = openlist[m / 2];
										openlist[m / 2] = openlist[m];
										openlist[m] = temp;								//swap them
										m /= 2;
									}
									else
									{
										break;
									}
								}
								break;													//exit the 'for' loop because we already changed this node
							}															//if
						}																//for
					}											//if (gc < gcost[newnode])
				}												//if (list[newnode] != 1) --> else
			}													//for (loop through links)
		}														//if (numOpen != 0)
		else
		{
			found = qfalse;										//there is no path between these nodes
			break;
		}

		if ( list[to] == 1 )									//if the destination node is on the open list, we're done
		{
			found = qtrue;
			break;
		}
	}															//while (1)

	if ( found == qtrue )							//if we found a path, and are trying to store the pathlist...
	{
		count = 0;
		temp = to;												//start at the end point
		while ( temp != from )									//travel along the path (backwards) until we reach the starting point
		{
			if (count+1 >= MAX_PATHLIST_NODES)
				return -1; // UQ1: Added to stop crash if path is too long for the memory allocation...

			pathlist[count++] = temp;							//add the node to the pathlist and increment the count
			temp = parent[temp];								//move to the parent of this node to continue the path
		}

		pathlist[count++] = from;								//add the beginning node to the end of the pathlist
		return ( count );
	}

/*	if ( !found && map_has_sequential_nodes )
	{															// We can attempt dumb (sequential) routing!
		count = CreateDumbRoute( from, to, pathlist );
		if ( count > 0 )
		{
			return ( count );
		}
	}*/

	return ( count );											//return the number of nodes in the path, -1 if not found
}

int
CreateAltPathAStar ( gentity_t *bot, int from, int to, /*short*/ int *pathlist )
{// UQ1: This version will find an alternate route, it will probebly be longer, but it should vary things :)
	//all the data we have to hold...since we can't do dynamic allocation, has to be MAX_NODES
	//we can probably lower this later - eg, the open list should never have more than at most a few dozen items on it
	int			numOpen = 0;
	int			atNode, temp, newnode = -1;
	qboolean	found = qfalse;
	int			count = -1;
	float		gc;
	int			i, j, u, v, m, z;
	vec3_t		vec;

	// Direct path data...
	int			direct_pathlist[MAX_PATHLIST_NODES];
	vec3_t		avoid_zones[MAX_PATHLIST_NODES];
	int			num_avoid_zones = 0;
	int			direct_pathsize = CreatePathAStar( bot, from, to, direct_pathlist );
	int			increments = 1;

	if ( (from == NODE_INVALID) || (to == NODE_INVALID) || (from >= MAX_NODES) || (to >= MAX_NODES) || (from == to) )
	{
		return ( -1 );
	}

	// Check if memory needs to be allocated...
	AllocatePathFindingMemory();

	if (direct_pathsize <= 10 || VectorDistance(nodes[from].origin, nodes[to].origin) <= 512)
	{// Too short for an alt path (or no path found)!
		for ( i = 0; i < direct_pathsize; i++)
		{// Copy the direct path and return it!
			pathlist[i] = direct_pathlist[i];
		}

#ifdef __ALT_PATH_DEBUG__
		if (direct_pathsize > 1)
			G_Printf("^1Failed making an alt path, original path is too short (^7%i^1)!\n", direct_pathsize);
#endif //__ALT_PATH_DEBUG__

		return ( direct_pathsize );
	}

	increments = (direct_pathsize * 0.15f);
	
	for (i = increments; i < direct_pathsize; i+=increments)
	{
		if (VectorDistance(nodes[direct_pathlist[i]].origin, nodes[from].origin) > 512
			&& VectorDistance(nodes[direct_pathlist[i]].origin, nodes[to].origin) > 512)
		{
			VectorCopy(nodes[direct_pathlist[i]].origin, avoid_zones[num_avoid_zones]);
			num_avoid_zones++;
		}
	}

	/*for (z = 0; z < num_avoid_zones; z++)
	{
		G_Printf("Avoid zone %i at %f %f %f.\n", z, avoid_zones[z][0], avoid_zones[z][1], avoid_zones[z][2]);
	}*/

	//clear out all the arrays - UQ1: Added - only allocate total nodes for map for speed...
	if (SSE_CPU)
	{
		sse_memset( openlist, 0, ((sizeof(int)) * (number_of_nodes + 1))/8 );
		sse_memset( fcost, 0, ((sizeof(int)) * number_of_nodes)/8 );
		sse_memset( list, 0, ((sizeof(char)) * number_of_nodes)/8 );
		sse_memset( parent, 0, ((sizeof(int)) * number_of_nodes)/8 );
		sse_memset( gcost, 0, ((sizeof(float)) * number_of_nodes)/8 );
	}
	else
	{
		memset( openlist, 0, ((sizeof(int)) * (number_of_nodes + 1)) );
		memset( fcost, 0, ((sizeof(int)) * number_of_nodes) );
		memset( list, 0, ((sizeof(char)) * number_of_nodes) );
		memset( parent, 0, ((sizeof(int)) * number_of_nodes) );
		memset( gcost, 0, ((sizeof(float)) * number_of_nodes) );
	}
	
	for (i = 0; i < number_of_nodes; i++)
	{
		gcost[i] = Distance(nodes[i].origin, nodes[to].origin); 
	}

	openlist[MAX_NODES] = 0;

	if ( (from == NODE_INVALID) || (to == NODE_INVALID) || (from >= MAX_NODES) || (to >= MAX_NODES) || (from == to) )
	{
		return ( -1 );
	}

	openlist[1] = from;																	//add the starting node to the open list
	numOpen++;
	gcost[from] = 0;																	//its f and g costs are obviously 0
	fcost[from] = 0;

	while ( 1 )
	{
		if ( numOpen != 0 )																//if there are still items in the open list
		{
			//pop the top item off of the list
			atNode = openlist[1];
			list[atNode] = 2;															//put the node on the closed list so we don't check it again
			numOpen--;
			openlist[1] = openlist[numOpen + 1];										//move the last item in the list to the top position
			v = 1;

			//this while loop reorders the list so that the new lowest fcost is at the top again
			while ( 1 )
			{
				u = v;
				if ( (2 * u + 1) < numOpen )											//if both children exist
				{
					if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
					{
						v = 2 * u;
					}

					if ( fcost[openlist[v]] >= fcost[openlist[2 * u + 1]] )
					{
						v = 2 * u + 1;
					}
				}
				else
				{
					if ( (2 * u) < numOpen )											//if only one child exists
					{
						if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
						{
							v = 2 * u;
						}
					}
				}

				if ( u != v )															//if they're out of order, swap this item with its parent
				{
					temp = openlist[u];
					openlist[u] = openlist[v];
					openlist[v] = temp;
				}
				else
				{
					break;
				}
			}

			if (nodes[atNode].enodenum <= MAX_NODELINKS)
			for ( i = 0; i < nodes[atNode].enodenum; i++ )								//loop through all the links for this node
			{
				qboolean is_in_an_avoid_zone = qfalse;

				newnode = nodes[atNode].links[i].targetNode;

				if (newnode > number_of_nodes)
					continue;

				if (newnode < 0)
					continue;

				if ( nodes[atNode].links[i].flags & PATH_BLOCKED )
				{																		//if this path is blocked, skip it
					continue;
				}

				if ( nodes[atNode].links[i].flags & PATH_DANGER )
				{																		//if this path is blocked, skip it
					continue;
				}

/*#ifdef __NPC__
				if ( bot->s.eType == ET_NPC && nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE )
				{																		//if this path is blocked, skip it
					continue;
				}
#endif //__NPC__*/

#ifdef __VEHICLES__
				/*if
				(
					bot->client &&
					!(bot->client->ps.eFlags & EF_VEHICLE) &&
					(nodes[nodes[atNode].links[i].targetNode].type & NODE_LAND_VEHICLE)
				)
				{
					continue;															// Non-Tank driving bots should NOT use tank nodes!
				}*/

				if (bot->client)
				{
					if (bot->client->ps.eFlags & EF_VEHICLE)
					{
						// Check == first for speed...
						if (nodes[atNode].links[i].flags == PATH_NOTANKS)
						{//if this path is blocked, skip it
							continue;
						}

						if (nodes[atNode].links[i].flags & PATH_NOTANKS)
						{//if this path is blocked, skip it
							continue;
						}

						// Check == first for speed...
						if (nodes[nodes[atNode].links[i].targetNode].type == NODE_LAND_VEHICLE)
						{// Tank driving bots should only use tank nodes!
						
						}
						else if (!(nodes[nodes[atNode].links[i].targetNode].type & NODE_LAND_VEHICLE))
						{// Tank driving bots should only use tank nodes!
							continue;
						}

						// Check == first for speed...
						if (!(nodes[bot->current_node].type == NODE_ICE))
						{
							if (nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE)
							{// Avoid ice!
								continue;
							}

							if (nodes[nodes[atNode].links[i].targetNode].type & NODE_ICE)
							{// Avoid ice!
								continue;
							}
						}

						if (!(nodes[bot->current_node].type & NODE_ICE))
						{// Check == first for speed...
							if (nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE)
							{// Avoid ice!
								continue;
							}

							if (nodes[nodes[atNode].links[i].targetNode].type & NODE_ICE)
							{// Avoid ice!
								continue;
							}
						}
					}
				}

#endif //__VEHICLES__
				/*if ( BAD_WP_Height( nodes[atNode].origin, nodes[newnode].origin) )
				{																		// Height diff is greater then plane distance... Block it and skip it..
					if (bot->client)
						BOT_MakeLinkSemiUnreachable( bot, atNode, i );

					continue;
				}*/

				//skip any unreachable nodes
				if (bot && bot->client)
				{
					// UQ1: == added for faster skips...
					if (nodes[newnode].type == NODE_AXIS_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_AXIS)						
						{
							continue;
						}
					}

					if (nodes[newnode].type == NODE_ALLY_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_ALLIES)	
						{
							continue;
						}
					}

					if (nodes[newnode].type & NODE_AXIS_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_AXIS)						
						{
							continue;
						}
					}

					if (nodes[newnode].type & NODE_ALLY_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_ALLIES)	
						{
							continue;
						}
					}
				}

/*#ifdef __VEHICLES__
				if (bot->client 
					&& (bot->client->ps.eFlags & EF_VEHICLE) )
				{
					if (Tank_PositionVisible(nodes[atNode].origin, nodes[newnode].origin, bot->s.number) != 1)
						continue;
				}
#endif //__VEHICLES__*/

				if ( list[newnode] == 2 )
				{																		//if this node is on the closed list, skip it
					continue;
				}

				/*if (VectorDistance(nodes[from].origin, nodes[newnode].origin) < 1024)
				{// UQ1: Can avoid this check if it is a long way away...
					if (AIMOD_MOVEMENT_Explosive_Near_Waypoint(newnode))					
					{// UQ1: If explosive near it. Skip it!
						continue;
					}
				}*/

				//if (AIMOD_MOVEMENT_Explosive_Near_Waypoint(newnode) 
				//	&& VectorDistance(bot->r.currentOrigin, nodes[newnode].origin) > 380)
				//	continue;

				for (z = 0; z < num_avoid_zones; z++)
				{// Check we are not close to an avoid zone!
					if (VectorDistance(avoid_zones[z], nodes[newnode].origin) <= 512)
					{// Too close to an avoid zone! Don't use this waypoint!
						is_in_an_avoid_zone = qtrue;
						//G_Printf("Node %i is in avoid zone.\n", newnode);
						break;
					}
				}

				if (is_in_an_avoid_zone)
				{																		//if this node is on the avoid list, skip it  and mark it!
					//list[newnode] = 2; // Add to closed list and stop any more checks on it!
					//G_Printf("Node %i skipped.\n", newnode);
					continue;
				}

				if ( list[newnode] != 1 )												//if this node is not already on the open list
				{
					openlist[++numOpen] = newnode;										//add the new node to the open list
					list[newnode] = 1;
					parent[newnode] = atNode;											//record the node's parent
					
					if ( newnode == to )
					{																	//if we've found the goal, don't keep computing paths!
						break;															//this will break the 'for' and go all the way to 'if (list[to] == 1)'
					}

					fcost[newnode] = GetFCost( bot, to, newnode, parent[newnode], gcost );	//store it's f cost value

					//this loop re-orders the heap so that the lowest fcost is at the top
					m = numOpen;

					while ( m != 1 )													//while this item isn't at the top of the heap already
					{
						if ( fcost[openlist[m]] <= fcost[openlist[m / 2]] )				//if it has a lower fcost than its parent
						{
							temp = openlist[m / 2];
							openlist[m / 2] = openlist[m];
							openlist[m] = temp;											//swap them
							m /= 2;
						}
						else
						{
							break;
						}
					}
				}
				else										//if this node is already on the open list
				{
					gc = gcost[atNode];
					//VectorSubtract( nodes[newnode].origin, nodes[atNode].origin, vec );
					//gc += VectorLength( vec );				//calculate what the gcost would be if we reached this node along the current path
					if (nodes[atNode].links[i].cost)
					{// UQ1: Already have a cost value, skip the calculations!
						gc += nodes[atNode].links[i].cost;
					}
					else
					{
						VectorSubtract( nodes[newnode].origin, nodes[atNode].origin, vec );
						gc += VectorLength( vec );				//calculate what the gcost would be if we reached this node along the current path
						nodes[atNode].links[i].cost = gc;
					}

					if ( gc < gcost[newnode] )				//if the new gcost is less (ie, this path is shorter than what we had before)
					{
						parent[newnode] = atNode;			//set the new parent for this node
						gcost[newnode] = gc;				//and the new g cost

						for ( j = 1; j < numOpen; j++ )		//loop through all the items on the open list
						{
							if ( openlist[j] == newnode )	//find this node in the list
							{
								//calculate the new fcost and store it
								fcost[newnode] = GetFCost( bot, to, newnode, parent[newnode], gcost );

								//reorder the list again, with the lowest fcost item on top
								m = j;

								while ( m != 1 )
								{
									if ( fcost[openlist[m]] < fcost[openlist[m / 2]] )	//if the item has a lower fcost than it's parent
									{
										temp = openlist[m / 2];
										openlist[m / 2] = openlist[m];
										openlist[m] = temp;								//swap them
										m /= 2;
									}
									else
									{
										break;
									}
								}
								break;													//exit the 'for' loop because we already changed this node
							}															//if
						}																//for
					}											//if (gc < gcost[newnode])
				}												//if (list[newnode] != 1) --> else
			}													//for (loop through links)
		}														//if (numOpen != 0)
		else
		{
			found = qfalse;										//there is no path between these nodes
			break;
		}

		if ( list[to] == 1 )									//if the destination node is on the open list, we're done
		{
			found = qtrue;
			break;
		}
	}															//while (1)

	if ( found == qtrue )							//if we found a path, and are trying to store the pathlist...
	{
		count = 0;
		temp = to;												//start at the end point
		while ( temp != from )									//travel along the path (backwards) until we reach the starting point
		{
			if (count+1 >= MAX_PATHLIST_NODES)
			{
#ifdef __ALT_PATH_DEBUG__
				G_Printf("Failed making an alt path (count+1 >= MAX_PATHLIST_NODES)!\n");
#endif //__ALT_PATH_DEBUG__
				return -1; // UQ1: Added to stop crash if path is too long for the memory allocation...
			}

			pathlist[count++] = temp;							//add the node to the pathlist and increment the count
			temp = parent[temp];								//move to the parent of this node to continue the path
		}

		pathlist[count++] = from;								//add the beginning node to the end of the pathlist
	}

	if (count <= 0)
	{// No alt path found!
		for ( i = 0; i < direct_pathsize; i++)
		{// Copy the direct path and return it!
			pathlist[i] = direct_pathlist[i];
		}

#ifdef __ALT_PATH_DEBUG__
		if (bot && bot->client)
			G_Printf("%s ^1- Failed making an alt path!\n", bot->client->pers.netname);
		else
			G_Printf("%s ^1- Failed making an alt path!\n", "non-bot");
#endif //__ALT_PATH_DEBUG__

		return ( direct_pathsize );
	}

#ifdef __ALT_PATH_DEBUG__
	{
		int same_count = 0;

		for ( i = 0; i < direct_pathsize && i < count; i++)
		{// Copy the direct path and return it!
			if (pathlist[i] == direct_pathlist[i])
				same_count++;
		}

		if (bot && bot->client)
			G_Printf("%s ^5- Alt path created ^3OK^5! ^7%i^5 waypoints used (^7%i^5 in original path) same count is ^7%i^5.\n", bot->client->pers.netname, count, direct_pathsize, same_count);
		else
			G_Printf("%s ^5- Alt path created ^3OK^5! ^7%i^5 waypoints used (^7%i^5 in original path) same count is ^7%i^5.\n", "non-bot", count, direct_pathsize, same_count);
		
		G_Printf("^5Normal path: ");
		for ( i = 0; i < direct_pathsize; i++)
			G_Printf("^7%i ", direct_pathlist[i]);
		G_Printf("^5\n");

		G_Printf("^5Alt path: ");
		for ( i = 0; i < count && i < count; i++)
			G_Printf("^7%i ", pathlist[i]);
		G_Printf("^5\n");
	}
#endif //__ALT_PATH_DEBUG__

	return ( count );											//return the number of nodes in the path, -1 if not found
}

#else //__CUDA__

__shared__ __host__ int CreatePathAStar ( gentity_t *bot, int from, int to, /*short*/ int *pathlist )
{
	//all the data we have to hold...since we can't do dynamic allocation, has to be MAX_NODES
	//we can probably lower this later - eg, the open list should never have more than at most a few dozen items on it
	__shared__ int			openlist[MAX_NODES + 1];												//add 1 because it's a binary heap, and they don't use 0 - 1 is the first used index
	__shared__ float		gcost[MAX_NODES];
	__shared__ int			fcost[MAX_NODES];
	__shared__ char			list[MAX_NODES];														//0 is neither, 1 is open, 2 is closed - char because it's the smallest data type
	__shared__ int			parent[MAX_NODES];
	__shared__ int			numOpen = 0;
	__shared__ int			atNode, temp, newnode = -1;
	__shared__ qboolean		found = qfalse;
	__shared__ int			count = -1;
	__shared__ float		gc;
	__shared__ int			i, j, u, v, m;
	__shared__ vec3_t		vec;

	//clear out all the arrays - UQ1: Added - only allocate total nodes for map for speed...
	/*if (CudaAvailable)
	{
		cudaMemset( openlist, 0, ((sizeof(int)) * (number_of_nodes + 1))/8 );
		cudaMemset( fcost, 0, ((sizeof(int)) * number_of_nodes)/8 );
		cudaMemset( list, 0, ((sizeof(char)) * number_of_nodes)/8 );
		cudaMemset( parent, 0, ((sizeof(int)) * number_of_nodes)/8 );
		cudaMemset( gcost, 0, ((sizeof(float)) * number_of_nodes)/8 );
	}
	else*/ if (SSE_CPU)
	{
		sse_memset( openlist, 0, ((sizeof(int)) * (number_of_nodes + 1))/8 );
		sse_memset( fcost, 0, ((sizeof(int)) * number_of_nodes)/8 );
		sse_memset( list, 0, ((sizeof(char)) * number_of_nodes)/8 );
		sse_memset( parent, 0, ((sizeof(int)) * number_of_nodes)/8 );
		sse_memset( gcost, 0, ((sizeof(float)) * number_of_nodes)/8 );
	}
	else
	{
		memset( openlist, 0, ((sizeof(int)) * (number_of_nodes + 1)) );
		memset( fcost, 0, ((sizeof(int)) * number_of_nodes) );
		memset( list, 0, ((sizeof(char)) * number_of_nodes) );
		memset( parent, 0, ((sizeof(int)) * number_of_nodes) );
		memset( gcost, 0, ((sizeof(float)) * number_of_nodes) );
	}

	openlist[MAX_NODES] = 0;

	if ( (from == NODE_INVALID) || (to == NODE_INVALID) || (from >= MAX_NODES) || (to >= MAX_NODES) || (from == to) )
	{
		return ( -1 );
	}

	openlist[1] = from;																	//add the starting node to the open list
	numOpen++;
	gcost[from] = 0;																	//its f and g costs are obviously 0
	fcost[from] = 0;

	while ( 1 )
	{
		if ( numOpen != 0 )																//if there are still items in the open list
		{
			//pop the top item off of the list
			atNode = openlist[1];
			list[atNode] = 2;															//put the node on the closed list so we don't check it again
			numOpen--;
			openlist[1] = openlist[numOpen + 1];										//move the last item in the list to the top position
			v = 1;

			//this while loop reorders the list so that the new lowest fcost is at the top again
			while ( 1 )
			{
				u = v;
				if ( (2 * u + 1) < numOpen )											//if both children exist
				{
					if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
					{
						v = 2 * u;
					}

					if ( fcost[openlist[v]] >= fcost[openlist[2 * u + 1]] )
					{
						v = 2 * u + 1;
					}
				}
				else
				{
					if ( (2 * u) < numOpen )											//if only one child exists
					{
						if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
						{
							v = 2 * u;
						}
					}
				}

				if ( u != v )															//if they're out of order, swap this item with its parent
				{
					temp = openlist[u];
					openlist[u] = openlist[v];
					openlist[v] = temp;
				}
				else
				{
					break;
				}
			}

			if (nodes[atNode].enodenum <= MAX_NODELINKS)
			for ( i = 0; i < nodes[atNode].enodenum; i++ )								//loop through all the links for this node
			{
				newnode = nodes[atNode].links[i].targetNode;

				if (newnode > number_of_nodes)
					continue;

				if (newnode < 0)
					continue;

				if ( nodes[atNode].links[i].flags & PATH_BLOCKED )
				{																		//if this path is blocked, skip it
					continue;
				}

				if ( nodes[atNode].links[i].flags & PATH_DANGER )
				{																		//if this path is blocked, skip it
					continue;
				}

/*#ifdef __NPC__
				if ( bot->s.eType == ET_NPC && nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE )
				{																		//if this path is blocked, skip it
					continue;
				}
#endif //__NPC__*/

#ifdef __VEHICLES__
				/*if
				(
					bot->client &&
					!(bot->client->ps.eFlags & EF_VEHICLE) &&
					(nodes[nodes[atNode].links[i].targetNode].type & NODE_LAND_VEHICLE)
				)
				{
					continue;															// Non-Tank driving bots should NOT use tank nodes!
				}*/

				if (bot->client)
				{
					if (bot->client->ps.eFlags & EF_VEHICLE)
					{
						// Check == first for speed...
						if (nodes[atNode].links[i].flags == PATH_NOTANKS)
						{//if this path is blocked, skip it
							continue;
						}

						if (nodes[atNode].links[i].flags & PATH_NOTANKS)
						{//if this path is blocked, skip it
							continue;
						}

						// Check == first for speed...
						if (nodes[nodes[atNode].links[i].targetNode].type == NODE_LAND_VEHICLE)
						{// Tank driving bots should only use tank nodes!
						
						}
						else if (!(nodes[nodes[atNode].links[i].targetNode].type & NODE_LAND_VEHICLE))
						{// Tank driving bots should only use tank nodes!
							continue;
						}

						// Check == first for speed...
						if (!(nodes[bot->current_node].type == NODE_ICE))
						{
							if (nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE)
							{// Avoid ice!
								continue;
							}

							if (nodes[nodes[atNode].links[i].targetNode].type & NODE_ICE)
							{// Avoid ice!
								continue;
							}
						}

						if (!(nodes[bot->current_node].type & NODE_ICE))
						{// Check == first for speed...
							if (nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE)
							{// Avoid ice!
								continue;
							}

							if (nodes[nodes[atNode].links[i].targetNode].type & NODE_ICE)
							{// Avoid ice!
								continue;
							}
						}
					}
				}

#endif //__VEHICLES__
				/*if ( BAD_WP_Height( nodes[atNode].origin, nodes[newnode].origin) )
				{																		// Height diff is greater then plane distance... Block it and skip it..
					if (bot->client)
						BOT_MakeLinkSemiUnreachable( bot, atNode, i );

					continue;
				}*/

				//skip any unreachable nodes
				if (bot && bot->client)
				{
					// UQ1: Added for faster check the ==
					if (nodes[newnode].type == NODE_AXIS_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_AXIS)						
						{
							continue;
						}
					}

					if (nodes[newnode].type == NODE_ALLY_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_ALLIES)	
						{
							continue;
						}
					}

					if (nodes[newnode].type & NODE_AXIS_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_AXIS)						
						{
							continue;
						}
					}

					if (nodes[newnode].type & NODE_ALLY_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_ALLIES)	
						{
							continue;
						}
					}
				}

/*#ifdef __VEHICLES__
				if (bot->client 
					&& (bot->client->ps.eFlags & EF_VEHICLE) )
				{
					if (Tank_PositionVisible(nodes[atNode].origin, nodes[newnode].origin, bot->s.number) != 1)
						continue;
				}
#endif //__VEHICLES__*/

				if ( list[newnode] == 2 )
				{																		//if this node is on the closed list, skip it
					continue;
				}

				/*if (VectorDistance(nodes[from].origin, nodes[newnode].origin) < 1024)
				{// UQ1: Can avoid this check if it is a long way away...
					if (AIMOD_MOVEMENT_Explosive_Near_Waypoint(newnode))					
					{// UQ1: If explosive near it. Skip it!
						continue;
					}
				}*/

				//if (AIMOD_MOVEMENT_Explosive_Near_Waypoint(newnode) 
				//	&& VectorDistance(bot->r.currentOrigin, nodes[newnode].origin) > 380)
				//	continue;

				if ( list[newnode] != 1 )												//if this node is not already on the open list
				{
					openlist[++numOpen] = newnode;										//add the new node to the open list
					list[newnode] = 1;
					parent[newnode] = atNode;											//record the node's parent
					
					if ( newnode == to )
					{																	//if we've found the goal, don't keep computing paths!
						break;															//this will break the 'for' and go all the way to 'if (list[to] == 1)'
					}

					fcost[newnode] = GetFCost( bot, to, newnode, parent[newnode], gcost );	//store it's f cost value

					//this loop re-orders the heap so that the lowest fcost is at the top
					m = numOpen;

					while ( m != 1 )													//while this item isn't at the top of the heap already
					{
						if ( fcost[openlist[m]] <= fcost[openlist[m / 2]] )				//if it has a lower fcost than its parent
						{
							temp = openlist[m / 2];
							openlist[m / 2] = openlist[m];
							openlist[m] = temp;											//swap them
							m /= 2;
						}
						else
						{
							break;
						}
					}
				}
				else										//if this node is already on the open list
				{
					gc = gcost[atNode];
					//VectorSubtract( nodes[newnode].origin, nodes[atNode].origin, vec );
					//gc += VectorLength( vec );				//calculate what the gcost would be if we reached this node along the current path
					if (nodes[atNode].links[i].cost)
					{// UQ1: Already have a cost value, skip the calculations!
						gc += nodes[atNode].links[i].cost;
					}
					else
					{
						VectorSubtract( nodes[newnode].origin, nodes[atNode].origin, vec );
						gc += VectorLength( vec );				//calculate what the gcost would be if we reached this node along the current path
						nodes[atNode].links[i].cost = gc;
					}

					if ( gc < gcost[newnode] )				//if the new gcost is less (ie, this path is shorter than what we had before)
					{
						parent[newnode] = atNode;			//set the new parent for this node
						gcost[newnode] = gc;				//and the new g cost

						for ( j = 1; j < numOpen; j++ )		//loop through all the items on the open list
						{
							if ( openlist[j] == newnode )	//find this node in the list
							{
								//calculate the new fcost and store it
								fcost[newnode] = GetFCost( bot, to, newnode, parent[newnode], gcost );

								//reorder the list again, with the lowest fcost item on top
								m = j;

								while ( m != 1 )
								{
									if ( fcost[openlist[m]] < fcost[openlist[m / 2]] )	//if the item has a lower fcost than it's parent
									{
										temp = openlist[m / 2];
										openlist[m / 2] = openlist[m];
										openlist[m] = temp;								//swap them
										m /= 2;
									}
									else
									{
										break;
									}
								}
								break;													//exit the 'for' loop because we already changed this node
							}															//if
						}																//for
					}											//if (gc < gcost[newnode])
				}												//if (list[newnode] != 1) --> else
			}													//for (loop through links)
		}														//if (numOpen != 0)
		else
		{
			found = qfalse;										//there is no path between these nodes
			break;
		}

		if ( list[to] == 1 )									//if the destination node is on the open list, we're done
		{
			found = qtrue;
			break;
		}
	}															//while (1)

	if ( found == qtrue )							//if we found a path, and are trying to store the pathlist...
	{
		count = 0;
		temp = to;												//start at the end point
		while ( temp != from )									//travel along the path (backwards) until we reach the starting point
		{
			if (count+1 >= MAX_PATHLIST_NODES)
				return -1; // UQ1: Added to stop crash if path is too long for the memory allocation...

			pathlist[count++] = temp;							//add the node to the pathlist and increment the count
			temp = parent[temp];								//move to the parent of this node to continue the path
		}

		pathlist[count++] = from;								//add the beginning node to the end of the pathlist
		return ( count );
	}

/*	if ( !found && map_has_sequential_nodes )
	{															// We can attempt dumb (sequential) routing!
		count = CreateDumbRoute( from, to, pathlist );
		if ( count > 0 )
		{
			return ( count );
		}
	}*/

	return ( count );											//return the number of nodes in the path, -1 if not found
}

__shared__ __host__ int CreateAltPathAStar ( gentity_t *bot, int from, int to, /*short*/ int *pathlist )
{// UQ1: This version will find an alternate route, it will probebly be longer, but it should vary things :)
	//all the data we have to hold...since we can't do dynamic allocation, has to be MAX_NODES
	//we can probably lower this later - eg, the open list should never have more than at most a few dozen items on it
	__shared__ int			openlist[MAX_NODES + 1];												//add 1 because it's a binary heap, and they don't use 0 - 1 is the first used index
	__shared__ float		gcost[MAX_NODES];
	__shared__ int			fcost[MAX_NODES];
	__shared__ char			list[MAX_NODES];														//0 is neither, 1 is open, 2 is closed - char because it's the smallest data type
	__shared__ int			parent[MAX_NODES];
	__shared__ int			numOpen = 0;
	__shared__ int			atNode, temp, newnode = -1;
	__shared__ qboolean		found = qfalse;
	__shared__ int			count = -1;
	__shared__ float		gc;
	__shared__ int			i, j, u, v, m, z;
	__shared__ vec3_t		vec;

	// Direct path data...
	__shared__ int			direct_pathlist[MAX_PATHLIST_NODES];
	__shared__ vec3_t		avoid_zones[MAX_PATHLIST_NODES];
	__shared__ int			num_avoid_zones = 0;
	__shared__ int			direct_pathsize = CreatePathAStar( bot, from, to, direct_pathlist );
	__shared__ int			increments = 1;

	if (direct_pathsize <= 10 || VectorDistance(nodes[from].origin, nodes[to].origin) <= 512)
	{// Too short for an alt path (or no path found)!
		for ( i = 0; i < direct_pathsize; i++)
		{// Copy the direct path and return it!
			pathlist[i] = direct_pathlist[i];
		}

#ifdef __ALT_PATH_DEBUG__
		if (direct_pathsize > 1)
			G_Printf("^1Failed making an alt path, original path is too short (^7%i^1)!\n", direct_pathsize);
#endif //__ALT_PATH_DEBUG__

		return ( direct_pathsize );
	}

	increments = (direct_pathsize * 0.15f);
	
	for (i = increments; i < direct_pathsize; i+=increments)
	{
		if (VectorDistance(nodes[direct_pathlist[i]].origin, nodes[from].origin) > 512
			&& VectorDistance(nodes[direct_pathlist[i]].origin, nodes[to].origin) > 512)
		{
			VectorCopy(nodes[direct_pathlist[i]].origin, avoid_zones[num_avoid_zones]);
			num_avoid_zones++;
		}
	}

	/*for (z = 0; z < num_avoid_zones; z++)
	{
		G_Printf("Avoid zone %i at %f %f %f.\n", z, avoid_zones[z][0], avoid_zones[z][1], avoid_zones[z][2]);
	}*/

	//clear out all the arrays - UQ1: Added - only allocate total nodes for map for speed...
	if (CudaAvailable)
	{
		cudaMemset( openlist, 0, ((sizeof(int)) * (number_of_nodes + 1))/8 );
		cudaMemset( fcost, 0, ((sizeof(int)) * number_of_nodes)/8 );
		cudaMemset( list, 0, ((sizeof(char)) * number_of_nodes)/8 );
		cudaMemset( parent, 0, ((sizeof(int)) * number_of_nodes)/8 );
		cudaMemset( gcost, 0, ((sizeof(float)) * number_of_nodes)/8 );
	}
	else if (SSE_CPU)
	{
		sse_memset( openlist, 0, ((sizeof(int)) * (number_of_nodes + 1))/8 );
		sse_memset( fcost, 0, ((sizeof(int)) * number_of_nodes)/8 );
		sse_memset( list, 0, ((sizeof(char)) * number_of_nodes)/8 );
		sse_memset( parent, 0, ((sizeof(int)) * number_of_nodes)/8 );
		sse_memset( gcost, 0, ((sizeof(float)) * number_of_nodes)/8 );
	}
	else
	{
		memset( openlist, 0, ((sizeof(int)) * (number_of_nodes + 1)) );
		memset( fcost, 0, ((sizeof(int)) * number_of_nodes) );
		memset( list, 0, ((sizeof(char)) * number_of_nodes) );
		memset( parent, 0, ((sizeof(int)) * number_of_nodes) );
		memset( gcost, 0, ((sizeof(float)) * number_of_nodes) );
	}

	openlist[MAX_NODES] = 0;

	if ( (from == NODE_INVALID) || (to == NODE_INVALID) || (from >= MAX_NODES) || (to >= MAX_NODES) || (from == to) )
	{
		return ( -1 );
	}

	openlist[1] = from;																	//add the starting node to the open list
	numOpen++;
	gcost[from] = 0;																	//its f and g costs are obviously 0
	fcost[from] = 0;

	while ( 1 )
	{
		if ( numOpen != 0 )																//if there are still items in the open list
		{
			//pop the top item off of the list
			atNode = openlist[1];
			list[atNode] = 2;															//put the node on the closed list so we don't check it again
			numOpen--;
			openlist[1] = openlist[numOpen + 1];										//move the last item in the list to the top position
			v = 1;

			//this while loop reorders the list so that the new lowest fcost is at the top again
			while ( 1 )
			{
				u = v;
				if ( (2 * u + 1) < numOpen )											//if both children exist
				{
					if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
					{
						v = 2 * u;
					}

					if ( fcost[openlist[v]] >= fcost[openlist[2 * u + 1]] )
					{
						v = 2 * u + 1;
					}
				}
				else
				{
					if ( (2 * u) < numOpen )											//if only one child exists
					{
						if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
						{
							v = 2 * u;
						}
					}
				}

				if ( u != v )															//if they're out of order, swap this item with its parent
				{
					temp = openlist[u];
					openlist[u] = openlist[v];
					openlist[v] = temp;
				}
				else
				{
					break;
				}
			}

			if (nodes[atNode].enodenum <= MAX_NODELINKS)
			for ( i = 0; i < nodes[atNode].enodenum; i++ )								//loop through all the links for this node
			{
				qboolean is_in_an_avoid_zone = qfalse;

				newnode = nodes[atNode].links[i].targetNode;

				if (newnode > number_of_nodes)
					continue;

				if (newnode < 0)
					continue;

				if ( nodes[atNode].links[i].flags & PATH_BLOCKED )
				{																		//if this path is blocked, skip it
					continue;
				}

				if ( nodes[atNode].links[i].flags & PATH_DANGER )
				{																		//if this path is blocked, skip it
					continue;
				}

/*#ifdef __NPC__
				if ( bot->s.eType == ET_NPC && nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE )
				{																		//if this path is blocked, skip it
					continue;
				}
#endif //__NPC__*/

#ifdef __VEHICLES__
				/*if
				(
					bot->client &&
					!(bot->client->ps.eFlags & EF_VEHICLE) &&
					(nodes[nodes[atNode].links[i].targetNode].type & NODE_LAND_VEHICLE)
				)
				{
					continue;															// Non-Tank driving bots should NOT use tank nodes!
				}*/

				if (bot->client)
				{
					if (bot->client->ps.eFlags & EF_VEHICLE)
					{
						// Check == first for speed...
						if (nodes[atNode].links[i].flags == PATH_NOTANKS)
						{//if this path is blocked, skip it
							continue;
						}

						if (nodes[atNode].links[i].flags & PATH_NOTANKS)
						{//if this path is blocked, skip it
							continue;
						}

						// Check == first for speed...
						if (nodes[nodes[atNode].links[i].targetNode].type == NODE_LAND_VEHICLE)
						{// Tank driving bots should only use tank nodes!
						
						}
						else if (!(nodes[nodes[atNode].links[i].targetNode].type & NODE_LAND_VEHICLE))
						{// Tank driving bots should only use tank nodes!
							continue;
						}

						// Check == first for speed...
						if (!(nodes[bot->current_node].type == NODE_ICE))
						{
							if (nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE)
							{// Avoid ice!
								continue;
							}

							if (nodes[nodes[atNode].links[i].targetNode].type & NODE_ICE)
							{// Avoid ice!
								continue;
							}
						}

						if (!(nodes[bot->current_node].type & NODE_ICE))
						{// Check == first for speed...
							if (nodes[nodes[atNode].links[i].targetNode].type == NODE_ICE)
							{// Avoid ice!
								continue;
							}

							if (nodes[nodes[atNode].links[i].targetNode].type & NODE_ICE)
							{// Avoid ice!
								continue;
							}
						}
					}
				}

#endif //__VEHICLES__
				/*if ( BAD_WP_Height( nodes[atNode].origin, nodes[newnode].origin) )
				{																		// Height diff is greater then plane distance... Block it and skip it..
					if (bot->client)
						BOT_MakeLinkSemiUnreachable( bot, atNode, i );

					continue;
				}*/

				//skip any unreachable nodes
				if (bot && bot->client)
				{
					// UQ1: == added for faster skips...
					if (nodes[newnode].type == NODE_AXIS_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_AXIS)						
						{
							continue;
						}
					}

					if (nodes[newnode].type == NODE_ALLY_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_ALLIES)	
						{
							continue;
						}
					}

					if (nodes[newnode].type & NODE_AXIS_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_AXIS)						
						{
							continue;
						}
					}

					if (nodes[newnode].type & NODE_ALLY_UNREACHABLE)
					{
						if (bot->client->sess.sessionTeam == TEAM_ALLIES)	
						{
							continue;
						}
					}
				}

/*#ifdef __VEHICLES__
				if (bot->client 
					&& (bot->client->ps.eFlags & EF_VEHICLE) )
				{
					if (Tank_PositionVisible(nodes[atNode].origin, nodes[newnode].origin, bot->s.number) != 1)
						continue;
				}
#endif //__VEHICLES__*/

				if ( list[newnode] == 2 )
				{																		//if this node is on the closed list, skip it
					continue;
				}

				/*if (VectorDistance(nodes[from].origin, nodes[newnode].origin) < 1024)
				{// UQ1: Can avoid this check if it is a long way away...
					if (AIMOD_MOVEMENT_Explosive_Near_Waypoint(newnode))					
					{// UQ1: If explosive near it. Skip it!
						continue;
					}
				}*/

				//if (AIMOD_MOVEMENT_Explosive_Near_Waypoint(newnode) 
				//	&& VectorDistance(bot->r.currentOrigin, nodes[newnode].origin) > 380)
				//	continue;

				for (z = 0; z < num_avoid_zones; z++)
				{// Check we are not close to an avoid zone!
					if (VectorDistance(avoid_zones[z], nodes[newnode].origin) <= 512)
					{// Too close to an avoid zone! Don't use this waypoint!
						is_in_an_avoid_zone = qtrue;
						//G_Printf("Node %i is in avoid zone.\n", newnode);
						break;
					}
				}

				if (is_in_an_avoid_zone)
				{																		//if this node is on the avoid list, skip it  and mark it!
					//list[newnode] = 2; // Add to closed list and stop any more checks on it!
					//G_Printf("Node %i skipped.\n", newnode);
					continue;
				}

				if ( list[newnode] != 1 )												//if this node is not already on the open list
				{
					openlist[++numOpen] = newnode;										//add the new node to the open list
					list[newnode] = 1;
					parent[newnode] = atNode;											//record the node's parent
					
					if ( newnode == to )
					{																	//if we've found the goal, don't keep computing paths!
						break;															//this will break the 'for' and go all the way to 'if (list[to] == 1)'
					}

					fcost[newnode] = GetFCost( bot, to, newnode, parent[newnode], gcost );	//store it's f cost value

					//this loop re-orders the heap so that the lowest fcost is at the top
					m = numOpen;

					while ( m != 1 )													//while this item isn't at the top of the heap already
					{
						if ( fcost[openlist[m]] <= fcost[openlist[m / 2]] )				//if it has a lower fcost than its parent
						{
							temp = openlist[m / 2];
							openlist[m / 2] = openlist[m];
							openlist[m] = temp;											//swap them
							m /= 2;
						}
						else
						{
							break;
						}
					}
				}
				else										//if this node is already on the open list
				{
					gc = gcost[atNode];
					//VectorSubtract( nodes[newnode].origin, nodes[atNode].origin, vec );
					//gc += VectorLength( vec );				//calculate what the gcost would be if we reached this node along the current path
					if (nodes[atNode].links[i].cost)
					{// UQ1: Already have a cost value, skip the calculations!
						gc += nodes[atNode].links[i].cost;
					}
					else
					{
						VectorSubtract( nodes[newnode].origin, nodes[atNode].origin, vec );
						gc += VectorLength( vec );				//calculate what the gcost would be if we reached this node along the current path
						nodes[atNode].links[i].cost = gc;
					}

					if ( gc < gcost[newnode] )				//if the new gcost is less (ie, this path is shorter than what we had before)
					{
						parent[newnode] = atNode;			//set the new parent for this node
						gcost[newnode] = gc;				//and the new g cost

						for ( j = 1; j < numOpen; j++ )		//loop through all the items on the open list
						{
							if ( openlist[j] == newnode )	//find this node in the list
							{
								//calculate the new fcost and store it
								fcost[newnode] = GetFCost( bot, to, newnode, parent[newnode], gcost );

								//reorder the list again, with the lowest fcost item on top
								m = j;

								while ( m != 1 )
								{
									if ( fcost[openlist[m]] < fcost[openlist[m / 2]] )	//if the item has a lower fcost than it's parent
									{
										temp = openlist[m / 2];
										openlist[m / 2] = openlist[m];
										openlist[m] = temp;								//swap them
										m /= 2;
									}
									else
									{
										break;
									}
								}
								break;													//exit the 'for' loop because we already changed this node
							}															//if
						}																//for
					}											//if (gc < gcost[newnode])
				}												//if (list[newnode] != 1) --> else
			}													//for (loop through links)
		}														//if (numOpen != 0)
		else
		{
			found = qfalse;										//there is no path between these nodes
			break;
		}

		if ( list[to] == 1 )									//if the destination node is on the open list, we're done
		{
			found = qtrue;
			break;
		}
	}															//while (1)

	if ( found == qtrue )							//if we found a path, and are trying to store the pathlist...
	{
		count = 0;
		temp = to;												//start at the end point
		while ( temp != from )									//travel along the path (backwards) until we reach the starting point
		{
			if (count+1 >= MAX_PATHLIST_NODES)
			{
#ifdef __ALT_PATH_DEBUG__
				G_Printf("Failed making an alt path (count+1 >= MAX_PATHLIST_NODES)!\n");
#endif //__ALT_PATH_DEBUG__
				return -1; // UQ1: Added to stop crash if path is too long for the memory allocation...
			}

			pathlist[count++] = temp;							//add the node to the pathlist and increment the count
			temp = parent[temp];								//move to the parent of this node to continue the path
		}

		pathlist[count++] = from;								//add the beginning node to the end of the pathlist
	}

	if (count <= 0)
	{// No alt path found!
		for ( i = 0; i < direct_pathsize; i++)
		{// Copy the direct path and return it!
			pathlist[i] = direct_pathlist[i];
		}

#ifdef __ALT_PATH_DEBUG__
		if (bot && bot->client)
			G_Printf("%s ^1- Failed making an alt path!\n", bot->client->pers.netname);
		else
			G_Printf("%s ^1- Failed making an alt path!\n", "non-bot");
#endif //__ALT_PATH_DEBUG__

		return ( direct_pathsize );
	}

#ifdef __ALT_PATH_DEBUG__
	{
		int same_count = 0;

		for ( i = 0; i < direct_pathsize && i < count; i++)
		{// Copy the direct path and return it!
			if (pathlist[i] == direct_pathlist[i])
				same_count++;
		}

		if (bot && bot->client)
			G_Printf("%s ^5- Alt path created ^3OK^5! ^7%i^5 waypoints used (^7%i^5 in original path) same count is ^7%i^5.\n", bot->client->pers.netname, count, direct_pathsize, same_count);
		else
			G_Printf("%s ^5- Alt path created ^3OK^5! ^7%i^5 waypoints used (^7%i^5 in original path) same count is ^7%i^5.\n", "non-bot", count, direct_pathsize, same_count);
		
		G_Printf("^5Normal path: ");
		for ( i = 0; i < direct_pathsize; i++)
			G_Printf("^7%i ", direct_pathlist[i]);
		G_Printf("^5\n");

		G_Printf("^5Alt path: ");
		for ( i = 0; i < count && i < count; i++)
			G_Printf("^7%i ", pathlist[i]);
		G_Printf("^5\n");
	}
#endif //__ALT_PATH_DEBUG__

	return ( count );											//return the number of nodes in the path, -1 if not found
}

#endif //__CUDA__

/*
===========================================================================
UpdateNodes
Called every frame to update the unreachable/objective flags for nodes,
so that the bots are aware an obstruction has been removed, etc
===========================================================================
*/
void
UpdateNodes ( void )
{
	int i, j;

	//	qboolean open[3];
	for ( i = 0; i < number_of_nodes; i++ )					//loop through all the nodes
	{

		//if this node has the unreachable flag set

		/*		if (nodes[i].type & (NODE_ALLY_UNREACHABLE|NODE_AXIS_UNREACHABLE))
		{
			int j;

			for (j = 0; j < 3; j++)		//loop through all the entity nums linked to the unreachable flag
			{
				open[j] = qfalse;		//default to false (still blocked)

				//if there isn't a linked object, skip it
				if ((nodes[i].objectNum[j] == ENTITYNUM_NONE) || (nodes[i].objectNum[j] == 0))
					continue;

				if (g_entities[nodes[i].objectNum[j]].spawnflags == 0)	//if the spawnflags are 0, it is now open
					open[j] = qtrue;
			}

			if (open[0] || open[1] || open[2])	//if any of them are now open
				nodes[i].type &= ~(NODE_ALLY_UNREACHABLE|NODE_AXIS_UNREACHABLE);	//clear the unreachable flags (and the objective flag, so bots don't keep setting it as a goal)
		}*/

		//if we blew up a dynamite objective, then clear the objective flag //(flip to construct - unique1)

		/*		if ((nodes[i].type & NODE_OBJECTIVE) && (nodes[i].objType == OBJECTIVE_DYNAMITE))
		{
			if (g_entities[nodes[i].objEntity].spawnflags == 0)
			{
				nodes[i].type &= ~NODE_OBJECTIVE;
				//nodes[i].objFlags &= ~OBJECTIVE_DYNAMITE;
				//nodes[i].objFlags |= OBJECTIVE_BUILD;
			}
		}*/

		//loop through all the links from this node
		for ( j = 0; j < nodes[i].enodenum; j++ )
		{
			if ( nodes[i].links[j].flags & PATH_BLOCKED )	//if the path is blocked
			{
				trace_t tr;
				vec3_t	start, end;
				VectorCopy( nodes[i].origin, start );
				VectorCopy( nodes[nodes[i].links[j].targetNode].origin, end );
				if ( BAD_WP_Height( start, end) )
				{
					continue;
				}

				start[2] += 16;
				end[2] += 16;

				//do a trace to see if there is still anything blocking the path
				trap_Trace( &tr, start, NULL, NULL, end, ENTITYNUM_NONE,
							MASK_SHOT | MASK_OPAQUE | MASK_WATER	/*MASK_ALL*//*MASK_DEADSOLID*/
						   );
				if ( tr.entityNum >= 0 )
				{													// Hit an entity, must still be blocked...
					gentity_t	*ent = &g_entities[tr.entityNum];
					if ( !ent )
					{
						continue;
					}

					if ( ent->client
#ifdef __NPC__
					|| ent->NPC_client
#endif //__NPC__
					)
					{
						nodes[i].links[j].flags &= ~PATH_BLOCKED;
					}
				}
				else												//if the path is open, clear the 'blocked' flag
				if ( tr.fraction == 1.0 )
				{
					nodes[i].links[j].flags &= ~PATH_BLOCKED;
				}
			}
			else if ( nodes[i].links[j].flags & PATH_DANGER )		//if the path is dangerous
			{
				trace_t tr;
				vec3_t	start, end;
				VectorCopy( nodes[i].origin, start );
				VectorCopy( nodes[nodes[i].links[j].targetNode].origin, end );
				if ( BAD_WP_Height( start, end) )
				{
					continue;
				}

				start[2] += 16;
				end[2] += 16;

				//do a trace to see if there is still anything blocking the path
				trap_Trace( &tr, start, NULL, NULL, end, ENTITYNUM_NONE,
							MASK_SHOT | MASK_OPAQUE | MASK_WATER	/*MASK_ALL*//*MASK_DEADSOLID*/
						   );
				if ( tr.entityNum >= 0 )
				{													// Hit an entity, must still be blocked...
					gentity_t	*ent = &g_entities[tr.entityNum];
					if ( !ent )
					{
						continue;
					}

					if ( ent->client
#ifdef __NPC__
					|| ent->NPC_client
#endif //__NPC__
					)
					{
						nodes[i].links[j].flags &= ~PATH_DANGER;
					}
				}
				else

				//if the path is open, clear the 'blocked' flag
				{
					if ( (tr.contents & CONTENTS_LAVA)				 /*|| (tr.contents & CONTENTS_SLIME) || (tr.contents & CONTENTS_WATER)*/ )
					{												// Still Dangerous!
					}
					else if ( tr.fraction == 1.0 )
					{												// Seems to be OK now! Clear the danger flag!
						nodes[i].links[j].flags &= ~PATH_DANGER;
					}
				}
			}

			/*else if (nodes[atNode].links[i].flags & PATH_NOTANKS)
			{

			}*/
		}
	}
}

float FloorHeightAt ( vec3_t org )
{
	trace_t tr;
	vec3_t org1, org2;
//	float height = 0;

	VectorCopy(org, org1);
	org1[2]+=48;

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

float BOT_FloorHeightAt ( gentity_t *bot )
{
	trace_t tr;
	vec3_t org;
	vec3_t org1, org2;
//	float height = 0;

	VectorCopy(bot->r.currentOrigin, org);

	VectorCopy(org, org1);
	org1[2]+=48;

	VectorCopy(org, org2);
	org2[2]= -65536.0f;

	trap_Trace( &tr, org1, NULL, NULL, org2, bot->s.number, MASK_PLAYERSOLID );
	
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

extern int AI_PM_GroundTrace ( vec3_t point, int clientNum );

/*
===========================================================================
GetFCost
Utility function used by A* pathfinding to calculate the
cost to move between nodes towards a goal.  Using the A*
algorithm F = G + H, G here is the distance along the node
paths the bot must travel, and H is the straight-line distance
to the goal node.
Returned as an int because more precision is unnecessary and it
will slightly speed up heap access
===========================================================================
*/
int
GetFCost ( gentity_t *bot, int to, int num, int parentNum, float *this_gcost )
{
	float	gc = 0;
	float	hc = 0;
	vec3_t	v;
	
	if ( this_gcost[num] == -1 )
	{
		if ( parentNum != -1 )
		{
			gc = this_gcost[parentNum];
			VectorSubtract( nodes[num].origin, nodes[parentNum].origin, v );
			gc += VectorLength( v );

			gc += HeightDistance(nodes[num].origin, nodes[parentNum].origin) * 4;

			// UQ1: Make WATER, BARB WIRE, and ICE nodes cost more!
			if (nodes[num].type & NODE_WATER)
			{// Huge cost for water nodes!
				//gc+=(gc*100);
				//gc = 64000.0f;
				gc = 0.0f;
				this_gcost[num] = gc;
				return gc;
			}
			else if (nodes[num].type & NODE_ICE)
			{// Huge cost for ice(slick) nodes!
				//gc+=(gc*100);
				//gc = 64000.0f;
				gc = 0.0f;
				this_gcost[num] = gc;
				return gc;
			}

			if (gc > 64000)
				gc = 64000.0f;

			/*if (nodes[num].type & NODE_COVER)
			{// Encorage the use of cover spots!
				gc = 0.0f;
			}*/
		}

		this_gcost[num] = gc;
	}
	else
	{
		gc = this_gcost[num];
		//G_Printf("gcost for num %i is %f\n", num, gc);
	}

	VectorSubtract( nodes[to].origin, nodes[num].origin, v );
	hc = VectorLength( v );

	/*if (nodes[num].type & NODE_WATER)
	{// Huge cost for water nodes!
		hc*=2;
	}
	else if (nodes[num].type & NODE_ICE)
	{// Huge cost for ice(slick) nodes!
		hc*=4;
	}*/

	/*if (nodes[num].type & NODE_COVER)
	{// Encorage the use of cover spots!
		hc *= 0.5;
	}*/

	return (int) ( gc + hc );
}
#endif //__BOT__


//standard visibility check
int
OrgVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	trap_Trace( &tr, org1, NULL, NULL, org2, ignore, MASK_SOLID | MASK_OPAQUE | MASK_WATER );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	return ( 0 );
}


//visibility check with hull trace
int
OrgVisibleBox ( vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore )
{
	trace_t tr;
	trap_Trace( &tr, org1, mins, maxs, org2, ignore, MASK_SOLID | MASK_OPAQUE | MASK_WATER );
	if ( tr.fraction == 1 && !tr.startsolid && !tr.allsolid )
	{
		return ( 1 );
	}

	return ( 0 );
}


// Area contents check
int
ReachableBox ( vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore )
{
	trace_t tr;

	//int contents_mask |= (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER);
	trap_Trace( &tr, org1, mins, maxs, org2, ignore, MASK_SHOT /*MASK_SOLID|MASK_OPAQUE*/ /*|MASK_WATER*/ );
	if ( tr.fraction == 1 && !tr.startsolid && !tr.allsolid )
	{
		if ( (tr.contents & CONTENTS_LAVA) /*|| (tr.contents & CONTENTS_SLIME) || (tr.contents & CONTENTS_WATER)*/ )
		{
			return ( 2 );	// Hit lava/water/slime...
		}
		else
		{
			return ( 1 );
		}
	}

	return ( 0 );
}


/*
void CalcEntitySpot ( gentity_t *ent, spot_t spot, vec3_t point ) 
*/
void
CalcEntitySpot ( const gentity_t *ent, const spot_t spot, vec3_t point )
{
	vec3_t	forward, up, right;
	vec3_t	start, end;
	trace_t tr;
	if ( !ent )
	{
		return;
	}

	switch ( spot )
	{
		case SPOT_ORIGIN:
			if ( VectorCompare( ent->r.currentOrigin, vec3_origin) )
			{															//brush
				VectorSubtract( ent->r.absmax, ent->r.absmin, point );	//size
				VectorMA( ent->r.absmin, 0.5, point, point );
			}
			else
			{
				VectorCopy( ent->r.currentOrigin, point );
			}
			break;
		case SPOT_CHEST:
		case SPOT_HEAD:

			/*if ( ent->client && VectorLengthSquared( ent->client->renderInfo.eyePoint ) )
		{//Actual tag_head eyespot!
			//FIXME: Stasis aliens may have a problem here...
			VectorCopy( ent->client->renderInfo.eyePoint, point );
			if ( ent->client->NPC_class == CLASS_ATST )
			{//adjust up some
				point[2] += 28;//magic number :)
			}
			if ( ent->NPC )
			{//always aim from the center of my bbox, so we don't wiggle when we lean forward or backwards
				point[0] = ent->r.currentOrigin[0];
				point[1] = ent->r.currentOrigin[1];
			}
		}
		else*/
			{
				VectorCopy( ent->r.currentOrigin, point );
				if ( ent->client )
				{
					point[2] += ent->client->ps.viewheight;
				}
			}

			if ( spot == SPOT_CHEST && ent->client )
			{
				point[2] -= ent->r.maxs[2] * 0.2f;
			}
			break;
		case SPOT_HEAD_LEAN:

			/*if ( ent->client && VectorLengthSquared( ent->client->renderInfo.eyePoint ) )
		{//Actual tag_head eyespot!
			//FIXME: Stasis aliens may have a problem here...
			VectorCopy( ent->client->renderInfo.eyePoint, point );
			if ( ent->client->NPC_class == CLASS_ATST )
			{//adjust up some
				point[2] += 28;//magic number :)
			}
			if ( ent->NPC )
			{//always aim from the center of my bbox, so we don't wiggle when we lean forward or backwards
				point[0] = ent->r.currentOrigin[0];
				point[1] = ent->r.currentOrigin[1];
			}
			//NOTE: automatically takes leaning into account!
		}
		else*/
			{
				VectorCopy( ent->r.currentOrigin, point );
				if ( ent->client )
				{
					point[2] += ent->client->ps.viewheight;
				}

				//AddLeanOfs ( ent, point );
			}
			break;

		//FIXME: implement...
		//case SPOT_CHEST:
		//Returns point 3/4 from tag_torso to tag_head?
		//break;
		case SPOT_LEGS:
			VectorCopy( ent->r.currentOrigin, point );
			point[2] += ( ent->r.mins[2] * 0.5 );
			break;
		case SPOT_WEAPON:

			/*if( ent->NPC && !VectorCompare( ent->NPC->shootAngles, vec3_origin ) && !VectorCompare( ent->NPC->shootAngles, ent->client->ps.viewangles ))
		{
			AngleVectors( ent->NPC->shootAngles, forward, right, up );
		}
		else*/
			{
				AngleVectors( ent->client->ps.viewangles, forward, right, up );
			}

			CalcMuzzlePoint( (gentity_t *) ent, ent->client->ps.weapon, forward, right, up, point );

			//NOTE: automatically takes leaning into account!
			break;
		case SPOT_GROUND:

			// if entity is on the ground, just use it's absmin
			if ( ent->s.groundEntityNum != -1 )
			{
				VectorCopy( ent->r.currentOrigin, point );
				point[2] = ent->r.absmin[2];
				break;
			}

			// if it is reasonably close to the ground, give the point underneath of it
			VectorCopy( ent->r.currentOrigin, start );
			start[2] = ent->r.absmin[2];
			VectorCopy( start, end );
			end[2] -= 64;
			trap_Trace( &tr, start, ent->r.mins, ent->r.maxs, end, ent->s.number, MASK_PLAYERSOLID );
			if ( tr.fraction < 1.0 )
			{
				VectorCopy( tr.endpos, point );
				break;
			}

			// otherwise just use the origin
			VectorCopy( ent->r.currentOrigin, point );
			break;
		default:
			VectorCopy( ent->r.currentOrigin, point );
			break;
	}
}


/* */
qboolean
MyInFOV3 ( vec3_t spot, vec3_t from, vec3_t fromAngles, int hFOV, int vFOV )
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


//NPC to position
qboolean
MyInFOV2 ( vec3_t origin, gentity_t *from, int hFOV, int vFOV )
{
	vec3_t	fromAngles, eyes;
	if ( from->client )
	{
		VectorCopy( from->client->ps.viewangles, fromAngles );
	}
	else
	{
		VectorCopy( from->s.angles, fromAngles );
	}

	CalcEntitySpot( from, SPOT_HEAD, eyes );
	return ( MyInFOV3( origin, eyes, fromAngles, hFOV, vFOV) );
}


//Entity to entity
qboolean
MyInFOV ( gentity_t *ent, gentity_t *from, int hFOV, int vFOV )
{
	vec3_t	eyes;
	vec3_t	spot;
	vec3_t	deltaVector;
	vec3_t	angles, fromAngles;
	vec3_t	deltaAngles;
	if ( from->client )
	{

		/*if( !VectorCompare( from->client->renderInfo.eyeAngles, vec3_origin ) )
		{//Actual facing of tag_head!
			//NOTE: Stasis aliens may have a problem with this?
			VectorCopy( from->client->renderInfo.eyeAngles, fromAngles );
		}
		else*/
		{
			VectorCopy( from->client->ps.viewangles, fromAngles );
		}
	}
	else
	{
		VectorCopy( from->s.angles, fromAngles );
	}

	CalcEntitySpot( from, SPOT_HEAD_LEAN, eyes );
	CalcEntitySpot( ent, SPOT_ORIGIN, spot );
	VectorSubtract( spot, eyes, deltaVector );
	vectoangles( deltaVector, angles );
	deltaAngles[PITCH] = AngleDelta( fromAngles[PITCH], angles[PITCH] );
	deltaAngles[YAW] = AngleDelta( fromAngles[YAW], angles[YAW] );
	if ( fabs( deltaAngles[PITCH]) <= vFOV && fabs( deltaAngles[YAW]) <= hFOV )
	{
		return ( qtrue );
	}

	CalcEntitySpot( ent, SPOT_HEAD, spot );
	VectorSubtract( spot, eyes, deltaVector );
	vectoangles( deltaVector, angles );
	deltaAngles[PITCH] = AngleDelta( fromAngles[PITCH], angles[PITCH] );
	deltaAngles[YAW] = AngleDelta( fromAngles[YAW], angles[YAW] );
	if ( fabs( deltaAngles[PITCH]) <= vFOV && fabs( deltaAngles[YAW]) <= hFOV )
	{
		return ( qtrue );
	}

	CalcEntitySpot( ent, SPOT_LEGS, spot );
	VectorSubtract( spot, eyes, deltaVector );
	vectoangles( deltaVector, angles );
	deltaAngles[PITCH] = AngleDelta( fromAngles[PITCH], angles[PITCH] );
	deltaAngles[YAW] = AngleDelta( fromAngles[YAW], angles[YAW] );
	if ( fabs( deltaAngles[PITCH]) <= vFOV && fabs( deltaAngles[YAW]) <= hFOV )
	{
		return ( qtrue );
	}

	return ( qfalse );
}

/*
===========================================================================
MyVisible
This is just a copy of the built-in 'visible' function
but accounting for the players' viewheight.  This lets the
bots see players slightly more often and in a more realistic way
===========================================================================
*/
qboolean	RoughlyVisible ( gentity_t *self, gentity_t *other );
qboolean	EnemyRoughlyVisible ( gentity_t *self, gentity_t *other );
extern void CalcMuzzlePoint ( gentity_t *ent, int weapon, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint );
#ifdef __NPC__

qboolean ValidEnemySeenTimeCheck ( gentity_t *self, gentity_t *other );
qboolean CanSuppress ( gentity_t *attacker, gentity_t *target );

qboolean
MyVisible_Fast ( gentity_t *self, gentity_t *other )
{// Very fast direct scan to avoid supression checks, etc...
	if (!self || !other)
		return qfalse;

	if (self->client && (other->client || other->s.eType == ET_NPC))
	{
		if (!OnSameTeam(self, other))
		{
			vec3_t		selfView, otherView;
			trace_t		tr;

			VectorCopy(self->r.currentOrigin, selfView);
			selfView[2]+=42;

			VectorCopy(other->r.currentOrigin, otherView);
			otherView[2]+=42;

			trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );

			if (tr.entityNum == other->s.number)
				return qtrue;

			if (tr.fraction == 1.0f)
				return qtrue;
		}
	}
	else
	{
		vec3_t		selfView, otherView;
		trace_t		tr;

		VectorCopy(self->r.currentOrigin, selfView);
		selfView[2]+=42;

		VectorCopy(other->r.currentOrigin, otherView);
		otherView[2]+=32;

		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );

		if (tr.entityNum == other->s.number)
			return qtrue;

		if (tr.fraction == 1.0f)
			return qtrue;
	}

	return qfalse;
}


/* */
qboolean
NPC_MyVisible ( gentity_t *self, gentity_t *other )
{
	vec3_t		selfView, otherView;								//basically stores the locations of the players' eyes
	trace_t		tr;													//used in performing the collision info
	gentity_t	*traceEnt;											//holds the entity returned by the trace function
	vec3_t		orig_mins, orig_maxs;

	if (MyVisible_Fast(self, other))
		return qtrue;

	if (other->client || other->s.eType == ET_NPC)
	{
		if (!OnSameTeam(self, other))
		{
			if (!ValidEnemySeenTimeCheck(self, other))
				return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...

			if (CanSuppress( self, other ))
				return qtrue;
		}
	}

	if (other->client)
	{
		VectorCopy(other->client->ps.mins, orig_mins);
		VectorCopy(other->client->ps.maxs, orig_maxs);

		if (other->s.eFlags & EF_PRONE)
		{
			VectorSet(other->client->ps.mins, -128, -128, -20);
			VectorSet(other->client->ps.maxs, 128, 128, 20);
		}
		else if (other->s.eFlags & EF_CROUCHING)
		{
			VectorSet(other->client->ps.mins, -128, -128, -32);
			VectorSet(other->client->ps.maxs, 128, 128, 32);
		}
		else
		{
			VectorSet(other->client->ps.mins, -128, -128, -128);
			VectorSet(other->client->ps.maxs, 128, 128, 128);
		}

		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(other->r.mins, orig_mins);
		VectorCopy(other->r.maxs, orig_maxs);
		VectorSet(other->r.mins, -128, -128, -128);
		VectorSet(other->r.maxs, 128, 128, 128);
	}

	VectorCopy(self->r.currentOrigin, selfView);
	selfView[2] += 40;

#ifdef __NPC__
	if ( other->NPC_client )
	{
		VectorCopy( other->r.currentOrigin, otherView );			//copy the target's origin
		otherView[2] += ( other->NPC_client->ps.viewheight * 0.5 ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}
	else
#endif //__NPC__
	if ( other->client )
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += ( other->client->ps.viewheight-8 ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}
	else
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += ( 48 * 0.5 );							//add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}

	if (other->client)
	{
		VectorCopy(orig_mins, other->client->ps.mins);
		VectorCopy(orig_maxs, other->client->ps.maxs);
		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(orig_mins, other->r.mins);
		VectorCopy(orig_maxs, other->r.maxs);
	}

	traceEnt = &g_entities[tr.entityNum];							//set traceEnt to the entity the shot would hit

	if ( traceEnt == other || tr.fraction == 1 )										//if it would hit the player, return true
	{
		return ( qtrue );
	}

	return qfalse;
}
#endif //__NPC__

/* */
qboolean
MyVisible_Reverse ( gentity_t *self, gentity_t *other )
{// A copy of myvisible simply to switch self and other...
	vec3_t		selfView, otherView;								//basically stores the locations of the players' eyes
	trace_t		tr;													//used in performing the collision info
	gentity_t	*traceEnt;											//holds the entity returned by the trace function

	if (MyVisible_Fast(self, other))
		return qtrue;

#ifdef __NPC__
	if ( self->NPC_client )
	{
		return ( NPC_MyVisible( self, other) );
	}
#endif //__NPC__

	VectorCopy(self->r.currentOrigin, selfView);
	selfView[2] += 48;

	if ( self->client && /*infront( self, other) &&*/ (self->client->ps.eFlags & EF_MG42_ACTIVE || self->client->ps.eFlags & EF_MOUNTEDTANK) )
	{// Look from forward of the gun so its not in our LOS...
		vec3_t	forward, angles;
		VectorCopy(self->client->ps.viewangles, angles);
		angles[2] = 0;
		AngleVectors( angles, forward, NULL, NULL );
		VectorMA( self->r.currentOrigin, 128, forward, selfView );
		selfView[2] += 64;
	}

#ifdef __BOT_FAST_VISIBILITY__
	{
		int closestNodeSelf = AIMOD_VISIBILITY_ClosestNode( self );
		int closestNodeOther = AIMOD_VISIBILITY_ClosestNode( other );
		if ( closestNodeSelf >= 0 && closestNodeOther >= 0 )
		{
			return ( AIMOD_VISIBILITY_CheckVisibility( closestNodeSelf, closestNodeOther) );
		}
	}
#endif //__BOT_FAST_VISIBILITY__

#ifdef __NPC__
	if ( other->NPC_client )
	{
		VectorCopy( other->r.currentOrigin, otherView );			//copy the target's origin
		otherView[2] += 48;

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}
	else
#endif //__NPC__
	if ( other->client )
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += 48;

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}
	else
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += 48;

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}

	traceEnt = &g_entities[tr.entityNum];							//set traceEnt to the entity the shot would hit

	if ( self->client && other->client && !OnSameTeam(self, other) )
	{// Are we looking for an enemy visibility? Check our trace end is not friendly!
		if ( traceEnt->client && OnSameTeam(self, traceEnt) )
		{// NEVER shoot a friendly!
			return ( qfalse );
		}
	}

	if ( traceEnt == other || tr.fraction == 1 )										//if it would hit the player, return true
	{
		return ( qtrue );
	}

//	if ( VectorDistance( tr.endpos, otherView ) < 32)
//	{
//		return ( qtrue );
//	}

	return ( qfalse );												//it would hit something other than the player (or nothing) so return false
}

qboolean VisibleFrom ( vec3_t org, gentity_t *ent )
{
	trace_t		tr;
	gentity_t	*traceEnt;

	trap_Trace( &tr, org, NULL, NULL, ent->r.currentOrigin, -1, MASK_SHOT );

	traceEnt = &g_entities[tr.entityNum];
	
	if ( traceEnt == ent || tr.fraction == 1 )
		return qtrue;

	return qfalse;
}

qboolean SupressVisibleFrom ( vec3_t org, gentity_t *ent )
{
	trace_t		tr;
	gentity_t	*traceEnt;

	trap_Trace( &tr, org, NULL, NULL, ent->r.currentOrigin, -1, MASK_SHOT );

	traceEnt = &g_entities[tr.entityNum];
	
	if ( (traceEnt == ent || tr.fraction == 1) && VectorDistance(org, tr.endpos) < 128 )
		return qtrue;
	
	return qfalse;
}

qboolean CanSuppress ( gentity_t *attacker, gentity_t *target )
{// Can the bot supress a hiding enemy? -- UQ1: Replaces MyVisible code (for enemies).. Should work out better I hope!
	vec3_t		start, end, forward, right, up;
	gentity_t	*ent = target;
	vec3_t		angles, dir, aangles;
	float		fov = 8;
	trace_t		tr;
	qboolean	canAttack = qtrue;
	gentity_t	*traceEnt;

	if (MyVisible_Fast(attacker, target))
		return qtrue;

	if (!ent)
		return qfalse;

	if (!ent->client && ent->s.eType != ET_NPC)
		return qfalse;

	if (ent->s.eType != ET_NPC && ent->client->pers.connected != CON_CONNECTED)
		return qfalse;

	if (ent->health <= 0)
		return qfalse;

	if (ent->s.eType != ET_NPC && ent->client->ps.stats[STAT_HEALTH] <= 0)
		return qfalse;

	if (ent->s.eType != ET_NPC && (ent->client->ps.pm_flags & PMF_LIMBO))
		return qfalse;

	if (OnSameTeam(ent, attacker))
		return qfalse;

	if (attacker->supressTimer > level.time && attacker->enemy && attacker->enemy == ent)
	{
		if (VectorDistance(attacker->supressLocation, ent->r.currentOrigin) < 128)
			return qtrue;

		attacker->supressTimer = 0;
	}

	VectorCopy(ent->r.currentOrigin, end);
	end[2] += 32;

	if (visible(attacker, ent))
	{
		VectorCopy(end, attacker->supressLocation);
		attacker->supressTimer = level.time + 3000 + Q_TrueRand(0, 2000);
		attacker->enemy = ent;
		return qtrue;
	}

	VectorCopy(attacker->r.currentOrigin, start);
	
	if (attacker->client && (attacker->client->ps.eFlags & EF_CROUCHING))
		start[2] += 16;
	else if (attacker->s.eType == ET_NPC && (attacker->NPC_client->ps.eFlags & EF_CROUCHING))
		start[2] += 16;
	else
		start[2] += 32;

	VectorCopy(ent->r.currentOrigin, end);
	end[2] += 48;

	trap_Trace( &tr, start, NULL, NULL, end, attacker->s.number, MASK_SHOT );

	traceEnt = &g_entities[tr.entityNum];
	if ( traceEnt == ent || tr.fraction == 1 )
	{
		VectorCopy(end, attacker->supressLocation);
		attacker->enemy = ent;
		attacker->supressTimer = level.time + 2000 + Q_TrueRand(0, 2000);
		return qtrue;
	}

	VectorCopy(tr.endpos, end);
	end[2] += 32;

	// Adjust for close targets..
	if (VectorDistance(end, start) < VectorDistance(ent->r.currentOrigin, start) - 128)
		canAttack = qfalse;
	else if (VectorDistance(ent->r.currentOrigin, start) < 128)
		fov = 120;
	else if (VectorDistance(ent->r.currentOrigin, start) < 256)
		fov = 90;
	else if (VectorDistance(ent->r.currentOrigin, start) < 512)
		fov = 60;
	else if (VectorDistance(ent->r.currentOrigin, start) < 1024)
		fov = 48;
	else if (VectorDistance(ent->r.currentOrigin, start) < 2048)
		fov = 24;
	else if (VectorDistance(ent->r.currentOrigin, start) < 4096)
		fov = 8;
	else
		canAttack = qfalse;

	VectorSubtract (ent->r.currentOrigin, start, dir);
	vectoangles(dir, angles);

	VectorSubtract (end, start, dir);
	vectoangles(dir, aangles);

	if (VectorDistance(angles, aangles) > fov)
		canAttack = qfalse;

	if (canAttack && SupressVisibleFrom(end, ent))
	{
		VectorCopy(end, attacker->supressLocation);
		attacker->supressTimer = level.time + 2000 + Q_TrueRand(0, 2000);
		attacker->enemy = ent;
		return qtrue;
	}

	// Now try again but to the right a bit...
	canAttack = qtrue;

	VectorCopy(ent->r.currentOrigin, end);
	end[2] += 32;

	trap_Trace( &tr, start, NULL, NULL, end, attacker->s.number, MASK_SHOT );

	traceEnt = &g_entities[tr.entityNum];
	if ( traceEnt == ent || tr.fraction == 1 )
	{
		VectorCopy(end, attacker->supressLocation);
		attacker->supressTimer = level.time + 2000 + Q_TrueRand(0, 2000);
		attacker->enemy = ent;
		return qtrue;
	}

	VectorCopy(tr.endpos, end);
	end[2] += 32;

	if (ent->client)
		AngleVectors( ent->client->ps.viewangles, forward, right, up );
	else if (ent->NPC_client)
		AngleVectors( ent->NPC_client->ps.viewangles, forward, right, up );
	else
		AngleVectors( ent->s.angles, forward, right, up );

	VectorMA( end, 64, right, end );

	// Adjust for close targets..
	if (VectorDistance(end, start) < VectorDistance(ent->r.currentOrigin, start) - 128)
		canAttack = qfalse;
	else if (VectorDistance(ent->r.currentOrigin, start) < 128)
		fov = 120;
	else if (VectorDistance(ent->r.currentOrigin, start) < 256)
		fov = 90;
	else if (VectorDistance(ent->r.currentOrigin, start) < 512)
		fov = 60;
	else if (VectorDistance(ent->r.currentOrigin, start) < 1024)
		fov = 48;
	else if (VectorDistance(ent->r.currentOrigin, start) < 2048)
		fov = 24;
	else if (VectorDistance(ent->r.currentOrigin, start) < 4096)
		fov = 8;
	else
		canAttack = qfalse;

	VectorSubtract (ent->r.currentOrigin, start, dir);
	vectoangles(dir, angles);

	VectorSubtract (end, start, dir);
	vectoangles(dir, aangles);

	if (VectorDistance(angles, aangles) > fov)
		canAttack = qfalse;

	if (canAttack && SupressVisibleFrom(end, ent))
	{
		VectorCopy(end, attacker->supressLocation);
		attacker->supressTimer = level.time + 2000 + Q_TrueRand(0, 2000);
		attacker->enemy = ent;
		return qtrue;
	}
	
	// Now try again but to the left a bit...
	canAttack = qtrue;

	VectorCopy(ent->r.currentOrigin, end);
	end[2] += 32;

	trap_Trace( &tr, start, NULL, NULL, end, attacker->s.number, MASK_SHOT );

	traceEnt = &g_entities[tr.entityNum];
	if ( traceEnt == ent || tr.fraction == 1 )
	{
		VectorCopy(end, attacker->supressLocation);
		attacker->supressTimer = level.time + 2000 + Q_TrueRand(0, 2000);
		attacker->enemy = ent;
		return qtrue;
	}

	VectorCopy(tr.endpos, end);
	end[2] += 32;
	
	if (ent->client)
		AngleVectors( ent->client->ps.viewangles, forward, right, up );
	else if (ent->NPC_client)
		AngleVectors( ent->NPC_client->ps.viewangles, forward, right, up );
	else
		AngleVectors( ent->s.angles, forward, right, up );

	VectorMA( end, -64, right, end );

	// Adjust for close targets..
	if (VectorDistance(end, start) < VectorDistance(ent->r.currentOrigin, start) - 128)
		canAttack = qfalse;
	else if (VectorDistance(ent->r.currentOrigin, start) < 128)
		fov = 120;
	else if (VectorDistance(ent->r.currentOrigin, start) < 256)
		fov = 90;
	else if (VectorDistance(ent->r.currentOrigin, start) < 512)
		fov = 60;
	else if (VectorDistance(ent->r.currentOrigin, start) < 1024)
		fov = 48;
	else if (VectorDistance(ent->r.currentOrigin, start) < 2048)
		fov = 24;
	else if (VectorDistance(ent->r.currentOrigin, start) < 4096)
		fov = 8;
	else
		canAttack = qfalse;

	VectorSubtract (ent->r.currentOrigin, start, dir);
	vectoangles(dir, angles);

	VectorSubtract (end, start, dir);
	vectoangles(dir, aangles);

	if (VectorDistance(angles, aangles) > fov)
		canAttack = qfalse;

	if (canAttack && SupressVisibleFrom(end, ent))
	{
		VectorCopy(end, attacker->supressLocation);
		attacker->supressTimer = level.time + 2000 + Q_TrueRand(0, 2000);
		attacker->enemy = ent;
		return qtrue;
	}

	return qfalse;
}

qboolean validity_visible (gentity_t *self, gentity_t *other)
{
	vec3_t		spot1, spot2;
	trace_t		tr;
	gentity_t	*traceEnt;
	vec3_t		orig_mins, orig_maxs;

	if (MyVisible_Fast(self, other))
		return qtrue;

	if (other->client)
	{
		VectorCopy(other->client->ps.mins, orig_mins);
		VectorCopy(other->client->ps.maxs, orig_maxs);

		// Set up temp high hitbox...
		VectorSet(other->client->ps.mins, -18, -18, -24);
		VectorSet(other->client->ps.maxs, 18, 18, 96);
		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(other->r.mins, orig_mins);
		VectorCopy(other->r.maxs, orig_maxs);

		// Set up temp high hitbox...
		VectorSet(other->r.mins, -18, -18, -24);
		VectorSet(other->r.maxs, 18, 18, 96);
	}

	VectorCopy(self->r.currentOrigin, spot1);
	spot1[2] += 48;

	VectorCopy(other->r.currentOrigin, spot2);
	spot2[2] += 48;

	// Do first test trace!
	trap_Trace (&tr, spot1, NULL, NULL, spot2, self->s.number, MASK_SHOT);

	traceEnt = &g_entities[ tr.entityNum ];

	if (traceEnt == other || tr.fraction == 1)
	{
		if (other->client)
		{
			// Remove temp high hitbox...
			VectorCopy(orig_mins, other->client->ps.mins);
			VectorCopy(orig_maxs, other->client->ps.maxs);
			VectorCopy(other->client->ps.mins, other->r.mins);
			VectorCopy(other->client->ps.maxs, other->r.maxs);
		}
		else
		{
			// Remove temp high hitbox...
			VectorCopy(orig_mins, other->r.mins);
			VectorCopy(orig_maxs, other->r.maxs);
		}

		return qtrue;
	}

	//
	// Second (higher test trace)...
	//

	VectorCopy(self->r.currentOrigin, spot1);
	spot1[2] += 64;

	VectorCopy(other->r.currentOrigin, spot2);
	spot2[2] += 64;

	// Do first test trace!
	trap_Trace (&tr, spot1, NULL, NULL, spot2, self->s.number, MASK_SHOT);

	traceEnt = &g_entities[ tr.entityNum ];

	if (traceEnt == other || tr.fraction == 1)
	{
		if (other->client)
		{
			// Remove temp high hitbox...
			VectorCopy(orig_mins, other->client->ps.mins);
			VectorCopy(orig_maxs, other->client->ps.maxs);
			VectorCopy(other->client->ps.mins, other->r.mins);
			VectorCopy(other->client->ps.maxs, other->r.maxs);
		}
		else
		{
			// Remove temp high hitbox...
			VectorCopy(orig_mins, other->r.mins);
			VectorCopy(orig_maxs, other->r.maxs);
		}

		return qtrue;
	}

	//
	// Third (highest test trace)...
	//

	VectorCopy(self->r.currentOrigin, spot1);
	spot1[2] += 90;

	VectorCopy(other->r.currentOrigin, spot2);
	spot2[2] += 90;

	// Do first test trace!
	trap_Trace (&tr, spot1, NULL, NULL, spot2, self->s.number, MASK_SHOT);

	traceEnt = &g_entities[ tr.entityNum ];

	if (traceEnt == other || tr.fraction == 1)
	{
		if (other->client)
		{
			// Remove temp high hitbox...
			VectorCopy(orig_mins, other->client->ps.mins);
			VectorCopy(orig_maxs, other->client->ps.maxs);
			VectorCopy(other->client->ps.mins, other->r.mins);
			VectorCopy(other->client->ps.maxs, other->r.maxs);
		}
		else
		{
			// Remove temp high hitbox...
			VectorCopy(orig_mins, other->r.mins);
			VectorCopy(orig_maxs, other->r.maxs);
		}

		return qtrue;
	}

	if (other->client)
	{
		// Remove temp high hitbox...
		VectorCopy(orig_mins, other->client->ps.mins);
		VectorCopy(orig_maxs, other->client->ps.maxs);
		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		// Remove temp high hitbox...
		VectorCopy(orig_mins, other->r.mins);
		VectorCopy(orig_maxs, other->r.maxs);
	}

	return qfalse;	
}

qboolean ValidEnemySeenTimeCheck ( gentity_t *self, gentity_t *other )
{
	if (self->bot_last_enemy_seen_time[other->s.number] < level.time - 30000)
	{// 30 seconds of valid seen time between checks! Check again now!
		if (!validity_visible(self, other))
		{
			self->bot_last_enemy_seen_time[other->s.number] = 0;
			return qfalse;
		}
		else
		{
			self->bot_last_enemy_seen_time[other->s.number] = level.time;
		}
	}
	else
	{// Should still be valid for supression/etc!
		return qtrue;
	}

	return qfalse;
}

/* */
qboolean
MyVisible ( gentity_t *self, gentity_t *other )
{
	vec3_t		selfView, otherView;								//basically stores the locations of the players' eyes
	trace_t		tr;													//used in performing the collision info
	gentity_t	*traceEnt;											//holds the entity returned by the trace function
	vec3_t		orig_mins, orig_maxs;
	vec3_t		forward, right, up;

	if (MyVisible_Fast(self, other))
		return qtrue;

#ifdef __NPC__
	if ( self->NPC_client )
	{
		return ( NPC_MyVisible( self, other) );
	}
#endif //__NPC__

	if (self->client && (other->client || other->s.eType == ET_NPC))
	{
		if (!OnSameTeam(self, other))
		{
			if (!ValidEnemySeenTimeCheck(self, other))
				return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...

			if (CanSuppress( self, other ))
				return qtrue;
		}
	}

	if (other->client)
	{
		VectorCopy(other->client->ps.mins, orig_mins);
		VectorCopy(other->client->ps.maxs, orig_maxs);

		if (other->s.eFlags & EF_PRONE)
		{
			VectorSet(other->client->ps.mins, -128, -128, -20);
			VectorSet(other->client->ps.maxs, 128, 128, 20);
		}
		else if (other->s.eFlags & EF_CROUCHING)
		{
			VectorSet(other->client->ps.mins, -128, -128, -32);
			VectorSet(other->client->ps.maxs, 128, 128, 32);
		}
		else
		{
			VectorSet(other->client->ps.mins, -128, -128, -128);
			VectorSet(other->client->ps.maxs, 128, 128, 128);
		}

		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(other->r.mins, orig_mins);
		VectorCopy(other->r.maxs, orig_maxs);
		VectorSet(other->r.mins, -128, -128, -128);
		VectorSet(other->r.maxs, 128, 128, 128);
	}

	AngleVectors( self->client->ps.viewangles, forward, right, up );
	CalcMuzzlePoint( self, self->client->ps.weapon, forward, right, up, selfView );

#ifdef __BOT_FAST_VISIBILITY__
	{
		int closestNodeSelf = AIMOD_VISIBILITY_ClosestNode( self );
		int closestNodeOther = AIMOD_VISIBILITY_ClosestNode( other );
		if ( closestNodeSelf >= 0 && closestNodeOther >= 0 )
		{
			return ( AIMOD_VISIBILITY_CheckVisibility( closestNodeSelf, closestNodeOther) );
		}
	}
#endif //__BOT_FAST_VISIBILITY__

#ifdef __NPC__
	if ( other->NPC_client )
	{
		VectorCopy( other->r.currentOrigin, otherView );			//copy the target's origin
		otherView[2] += ( other->NPC_client->ps.viewheight * 0.5 ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}
	else
#endif //__NPC__
	if ( other->client )
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += ( other->client->ps.viewheight-8 ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}
	else
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += ( 48 * 0.5 );							//add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}

	if (other->client)
	{
		VectorCopy(orig_mins, other->client->ps.mins);
		VectorCopy(orig_maxs, other->client->ps.maxs);
		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(orig_mins, other->r.mins);
		VectorCopy(orig_maxs, other->r.maxs);
	}

	traceEnt = &g_entities[tr.entityNum];							//set traceEnt to the entity the shot would hit

	if ( self->client && other->client && !OnSameTeam(self, other) )
	{// Are we looking for an enemy visibility? Check our trace end is not friendly!
		if ( traceEnt->client && OnSameTeam(self, traceEnt) )
		{// NEVER shoot a friendly!
			return ( qfalse );
		}
	}

	if ( traceEnt == other || tr.fraction == 1.0f )										//if it would hit the player, return true
	{
		return ( qtrue );
	}

	//return CanSuppress( self, other );
	return qfalse;
}

/* */
qboolean
MyVisible_Medic ( gentity_t *self, gentity_t *other )
{
	vec3_t		selfView, otherView;								//basically stores the locations of the players' eyes
	trace_t		tr;													//used in performing the collision info
	gentity_t	*traceEnt;											//holds the entity returned by the trace function
	vec3_t		orig_mins, orig_maxs;
	vec3_t		forward, right, up;

	if (self->client && (other->client || other->s.eType == ET_NPC))
	{
		if (!OnSameTeam(self, other))
		{
			if (!ValidEnemySeenTimeCheck(self, other))
				return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...
		}
	}

	if (other->client)
	{
		VectorCopy(other->client->ps.mins, orig_mins);
		VectorCopy(other->client->ps.maxs, orig_maxs);

		if (other->s.eFlags & EF_PRONE)
		{
			VectorSet(other->client->ps.mins, -18, -18, -48);
			VectorSet(other->client->ps.maxs, 18, 18, 48);
		}
		else if (other->s.eFlags & EF_CROUCHING)
		{
			VectorSet(other->client->ps.mins, -18, -18, -48);
			VectorSet(other->client->ps.maxs, 18, 18, 48);
		}
		else
		{
			VectorSet(other->client->ps.mins, -18, -18, -128);
			VectorSet(other->client->ps.maxs, 18, 18, 128);
		}

		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(other->r.mins, orig_mins);
		VectorCopy(other->r.maxs, orig_maxs);
		VectorSet(other->r.mins, -18, -18, -128);
		VectorSet(other->r.maxs, 18, 18, 128);
	}

	AngleVectors( self->client->ps.viewangles, forward, right, up );
	CalcMuzzlePoint( self, self->client->ps.weapon, forward, right, up, selfView );

#ifdef __NPC__
	if ( other->NPC_client )
	{
		VectorCopy( other->r.currentOrigin, otherView );			//copy the target's origin
		otherView[2] += ( other->NPC_client->ps.viewheight * 0.5 ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}
	else
#endif //__NPC__
	if ( other->client )
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += ( other->client->ps.viewheight-8 ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}
	else
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += ( 48 * 0.5 );							//add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}

	if (other->client)
	{
		VectorCopy(orig_mins, other->client->ps.mins);
		VectorCopy(orig_maxs, other->client->ps.maxs);
		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(orig_mins, other->r.mins);
		VectorCopy(orig_maxs, other->r.maxs);
	}

	traceEnt = &g_entities[tr.entityNum];							//set traceEnt to the entity the shot would hit

	if ( self->client && other->client && !OnSameTeam(self, other) )
	{// Are we looking for an enemy visibility? Check our trace end is not friendly!
		if ( traceEnt->client && OnSameTeam(self, traceEnt) )
		{// NEVER shoot a friendly!
			return ( qfalse );
		}
	}

	if ( traceEnt == other )										//if it would hit the player, return true
	{
		return ( qtrue );
	}

	//return CanSuppress( self, other );
	return qfalse;
}

/* */
qboolean
EnemyVisible ( vec3_t from, gentity_t *other, int ignoreEnt )
{
	vec3_t		otherView;								//basically stores the locations of the players' eyes
	trace_t		tr;													//used in performing the collision info
	gentity_t	*traceEnt;											//holds the entity returned by the trace function
	vec3_t		orig_mins, orig_maxs;
	gentity_t	*self = NULL;

	if (ignoreEnt >= 0)
		self = &g_entities[ignoreEnt];

	if (self && self->client && other->client)
	{
		if (!OnSameTeam(self, other))
		{
			if (!ValidEnemySeenTimeCheck(self, other))
				return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...
		}
	}

	if (other->client)
	{
		VectorCopy(other->client->ps.mins, orig_mins);
		VectorCopy(other->client->ps.maxs, orig_maxs);

		if (other->s.eFlags & EF_PRONE)
		{
			VectorSet(other->client->ps.mins, -128, -128, -20);
			VectorSet(other->client->ps.maxs, 128, 128, 20);
		}
		else if (other->s.eFlags & EF_CROUCHING)
		{
			VectorSet(other->client->ps.mins, -128, -128, -32);
			VectorSet(other->client->ps.maxs, 128, 128, 32);
		}
		else
		{
			VectorSet(other->client->ps.mins, -128, -128, -128);
			VectorSet(other->client->ps.maxs, 128, 128, 128);
		}

		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(other->r.mins, orig_mins);
		VectorCopy(other->r.maxs, orig_maxs);
		VectorSet(other->r.mins, -128, -128, -128);
		VectorSet(other->r.maxs, 128, 128, 128);
	}

#ifdef __NPC__
	if ( other->NPC_client )
	{
		VectorCopy( other->r.currentOrigin, otherView );			//copy the target's origin
		otherView[2] += 40;//( other->NPC_client->ps.viewheight * 0.5 ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}
	else
#endif //__NPC__
	if ( other->client )
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += 40;//( other->client->ps.viewheight-8 ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}
	else
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += 40;//( 48 * 0.5 );							//add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}

	if (other->client)
	{
		VectorCopy(orig_mins, other->client->ps.mins);
		VectorCopy(orig_maxs, other->client->ps.maxs);
		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(orig_mins, other->r.mins);
		VectorCopy(orig_maxs, other->r.maxs);
	}

	traceEnt = &g_entities[tr.entityNum];							//set traceEnt to the entity the shot would hit

	if ( traceEnt == other || tr.fraction == 1 )										//if it would hit the player, return true
	{
		return ( qtrue );
	}

	return ( qfalse );
}

/* */
qboolean
CoverspotEnemyVisible ( vec3_t from, gentity_t *other, int ignoreEnt )
{
	vec3_t		otherView;								//basically stores the locations of the players' eyes
	trace_t		tr;													//used in performing the collision info
	gentity_t	*traceEnt;											//holds the entity returned by the trace function
	vec3_t		dir, angles, right;
	vec3_t		orig_mins, orig_maxs;
	gentity_t	*self = NULL;

	if (ignoreEnt >= 0)
		self = &g_entities[ignoreEnt];

	//if (MyVisible_Fast( self, other ))
	//	return qtrue;

	if (self && self->client && other->client)
	{
		if (!OnSameTeam(self, other))
		{
			if (!ValidEnemySeenTimeCheck(self, other))
				return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...
		}
	}

	VectorSubtract(other->r.currentOrigin, from, dir);
	vectoangles(dir, angles);
	AngleVectors(angles, NULL, right, NULL);

	if (other->client)
	{
		VectorCopy(other->client->ps.mins, orig_mins);
		VectorCopy(other->client->ps.maxs, orig_maxs);

		if (other->s.eFlags & EF_PRONE)
		{
			VectorSet(other->client->ps.mins, -128, -128, -20);
			VectorSet(other->client->ps.maxs, 128, 128, 20);
		}
		else if (other->s.eFlags & EF_CROUCHING)
		{
			VectorSet(other->client->ps.mins, -128, -128, -32);
			VectorSet(other->client->ps.maxs, 128, 128, 32);
		}
		else
		{
			VectorSet(other->client->ps.mins, -128, -128, -128);
			VectorSet(other->client->ps.maxs, 128, 128, 128);
		}

		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(other->r.mins, orig_mins);
		VectorCopy(other->r.maxs, orig_maxs);
		VectorSet(other->r.mins, -128, -128, -128);
		VectorSet(other->r.maxs, 128, 128, 128);
	}
	
#ifdef __NPC__
	if ( other->NPC_client )
	{
		VectorCopy( other->r.currentOrigin, otherView );			//copy the target's origin
		otherView[2] += 40;//( other->NPC_client->ps.viewheight /** 0.5*/ ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}
	else
#endif //__NPC__
	if ( other->client )
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += 40;//( other->client->ps.viewheight/*-8*/ ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}
	else
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += 40;//( 48 /** 0.5*/ );							//add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}

	if (other->client)
	{
		VectorCopy(orig_mins, other->client->ps.mins);
		VectorCopy(orig_maxs, other->client->ps.maxs);
		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(orig_mins, other->r.mins);
		VectorCopy(orig_maxs, other->r.maxs);
	}

	traceEnt = &g_entities[tr.entityNum];							//set traceEnt to the entity the shot would hit

	if ( traceEnt == other || tr.fraction == 1 )										//if it would hit the player, return true
	{
		return ( qtrue );
	}

	// Second check, to the right of the enemy a bit...

	if (other->client)
	{
		VectorCopy(other->client->ps.mins, orig_mins);
		VectorCopy(other->client->ps.maxs, orig_maxs);

		if (other->s.eFlags & EF_PRONE)
		{
			VectorSet(other->client->ps.mins, -128, -128, -20);
			VectorSet(other->client->ps.maxs, 128, 128, 20);
		}
		else if (other->s.eFlags & EF_CROUCHING)
		{
			VectorSet(other->client->ps.mins, -128, -128, -32);
			VectorSet(other->client->ps.maxs, 128, 128, 32);
		}
		else
		{
			VectorSet(other->client->ps.mins, -128, -128, -128);
			VectorSet(other->client->ps.maxs, 128, 128, 128);
		}

		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(other->r.mins, orig_mins);
		VectorCopy(other->r.maxs, orig_maxs);
		VectorSet(other->r.mins, -128, -128, -128);
		VectorSet(other->r.maxs, 128, 128, 128);
	}

#ifdef __NPC__
	if ( other->NPC_client )
	{
		VectorCopy( other->r.currentOrigin, otherView );			//copy the target's origin
		otherView[2] += 40;//( other->NPC_client->ps.viewheight /** 0.5*/ ); //add the target's viewheight
		VectorMA( otherView, 16, right, otherView );

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}
	else
#endif //__NPC__
	if ( other->client )
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += 40;//( other->client->ps.viewheight/*-8*/ ); //add the target's viewheight
		VectorMA( otherView, 16, right, otherView );

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}
	else
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += 40;//( 48 /** 0.5*/ );							//add the target's viewheight
		VectorMA( otherView, 16, right, otherView );

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}

	if (other->client)
	{
		VectorCopy(orig_mins, other->client->ps.mins);
		VectorCopy(orig_maxs, other->client->ps.maxs);
		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(orig_mins, other->r.mins);
		VectorCopy(orig_maxs, other->r.maxs);
	}

	traceEnt = &g_entities[tr.entityNum];							//set traceEnt to the entity the shot would hit

	if ( traceEnt == other || tr.fraction == 1 )										//if it would hit the player, return true
	{
		return ( qtrue );
	}

	// Second check, to the left of the enemy a bit...

	if (other->client)
	{
		VectorCopy(other->client->ps.mins, orig_mins);
		VectorCopy(other->client->ps.maxs, orig_maxs);

		if (other->s.eFlags & EF_PRONE)
		{
			VectorSet(other->client->ps.mins, -128, -128, -20);
			VectorSet(other->client->ps.maxs, 128, 128, 20);
		}
		else if (other->s.eFlags & EF_CROUCHING)
		{
			VectorSet(other->client->ps.mins, -128, -128, -32);
			VectorSet(other->client->ps.maxs, 128, 128, 32);
		}
		else
		{
			VectorSet(other->client->ps.mins, -128, -128, -128);
			VectorSet(other->client->ps.maxs, 128, 128, 128);
		}

		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(other->r.mins, orig_mins);
		VectorCopy(other->r.maxs, orig_maxs);
		VectorSet(other->r.mins, -128, -128, -128);
		VectorSet(other->r.maxs, 128, 128, 128);
	}

#ifdef __NPC__
	if ( other->NPC_client )
	{
		VectorCopy( other->r.currentOrigin, otherView );			//copy the target's origin
		otherView[2] += 40;//( other->NPC_client->ps.viewheight /** 0.5*/ ); //add the target's viewheight
		VectorMA( otherView, -16, right, otherView );

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}
	else
#endif //__NPC__
	if ( other->client )
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += 40;//( other->client->ps.viewheight/*-8*/ ); //add the target's viewheight
		VectorMA( otherView, -16, right, otherView );

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}
	else
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += 40;//( 48 /** 0.5*/ );							//add the target's viewheight
		VectorMA( otherView, -16, right, otherView );

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, from, NULL, NULL, otherView, ignoreEnt, CONTENTS_SOLID|CONTENTS_PLAYERCLIP/*MASK_SHOT*/ );
	}

	if (other->client)
	{
		VectorCopy(orig_mins, other->client->ps.mins);
		VectorCopy(orig_maxs, other->client->ps.maxs);
		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(orig_mins, other->r.mins);
		VectorCopy(orig_maxs, other->r.maxs);
	}

	traceEnt = &g_entities[tr.entityNum];							//set traceEnt to the entity the shot would hit

	if ( traceEnt == other || tr.fraction == 1 )										//if it would hit the player, return true
	{
		return ( qtrue );
	}

	return ( qfalse );
}

/* */
qboolean
MyVisible_No_Supression ( gentity_t *self, gentity_t *other )
{
	vec3_t		selfView, otherView;								//basically stores the locations of the players' eyes
	trace_t		tr;													//used in performing the collision info
	gentity_t	*traceEnt;											//holds the entity returned by the trace function
	vec3_t		orig_mins, orig_maxs;
	//vec3_t		forward, right, up;

	if (MyVisible_Fast( self, other ))
		return qtrue;

#ifdef __NPC__
	if ( self->NPC_client )
	{
		return ( NPC_MyVisible( self, other) );
	}
#endif //__NPC__

	//if (self->client && (other->client || other->s.eType == ET_NPC))
	//{
	//	if (!OnSameTeam(self, other))
	//	{
	//		if (!ValidEnemySeenTimeCheck(self, other))
	//			return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...
	//	}
	//}

	VectorCopy(self->r.currentOrigin, selfView);

	if (self->s.eFlags & EF_PRONE)
		selfView[2] += 16;
	else if (self->s.eFlags & EF_CROUCHING)
		selfView[2] += 24;
	else
		selfView[2] += 48;

	if (other->client)
	{
		VectorCopy(other->client->ps.mins, orig_mins);
		VectorCopy(other->client->ps.maxs, orig_maxs);

		if (other->s.eFlags & EF_PRONE)
		{
			VectorSet(other->client->ps.mins, -128, -128, -20);
			VectorSet(other->client->ps.maxs, 128, 128, 20);
		}
		else if (other->s.eFlags & EF_CROUCHING)
		{
			VectorSet(other->client->ps.mins, -128, -128, -32);
			VectorSet(other->client->ps.maxs, 128, 128, 32);
		}
		else
		{
			VectorSet(other->client->ps.mins, -128, -128, -128);
			VectorSet(other->client->ps.maxs, 128, 128, 128);
		}

		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(other->r.mins, orig_mins);
		VectorCopy(other->r.maxs, orig_maxs);
		VectorSet(other->r.mins, -128, -128, -128);
		VectorSet(other->r.maxs, 128, 128, 128);
	}

#ifdef __NPC__
	if ( other->NPC_client )
	{
		VectorCopy( other->r.currentOrigin, otherView );			//copy the target's origin
		
		if (other->s.eFlags & EF_PRONE)
			otherView[2] += 16;
		else if (other->s.eFlags & EF_CROUCHING)
			otherView[2] += 24;
		else
			otherView[2] += 48;

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}
	else
#endif //__NPC__
	if ( other->client )
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		
		if (other->s.eFlags & EF_PRONE)
			otherView[2] += 16;
		else if (other->s.eFlags & EF_CROUCHING)
			otherView[2] += 24;
		else
			otherView[2] += 48;

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}
	else
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		
		if (other->s.eFlags & EF_PRONE)
			otherView[2] += 16;
		else if (other->s.eFlags & EF_CROUCHING)
			otherView[2] += 24;
		else
			otherView[2] += 48;

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	}

	if (other->client)
	{
		VectorCopy(orig_mins, other->client->ps.mins);
		VectorCopy(orig_maxs, other->client->ps.maxs);
		VectorCopy(other->client->ps.mins, other->r.mins);
		VectorCopy(other->client->ps.maxs, other->r.maxs);
	}
	else
	{
		VectorCopy(orig_mins, other->r.mins);
		VectorCopy(orig_maxs, other->r.maxs);
	}

	traceEnt = &g_entities[tr.entityNum];							//set traceEnt to the entity the shot would hit

	if ( self->client && other->client && !OnSameTeam(self, other) )
	{// Are we looking for an enemy visibility? Check our trace end is not friendly!
		if ( traceEnt->client && OnSameTeam(self, traceEnt) )
		{// NEVER shoot a friendly!
			return ( qfalse );
		}
	}

	if ( traceEnt == other || tr.fraction == 1 || VectorDistance(tr.endpos, other->r.currentOrigin) < 64 )										//if it would hit the player, return true
	{
		return ( qtrue );
	}

	return ( qfalse );
}

/* */
qboolean
MyVisible_PATH ( gentity_t *self, gentity_t *other )
{
	vec3_t		selfView, otherView;								//basically stores the locations of the players' eyes
	trace_t		tr;													//used in performing the collision info
	gentity_t	*traceEnt;											//holds the entity returned by the trace function
	vec3_t		forward, right, up;

	if (MyVisible_Fast( self, other ))
		return qtrue;

#ifdef __NPC__
	if ( self->NPC_client )
	{
		return ( NPC_MyVisible( self, other) );
	}
#endif //__NPC__

	if (self->client && (other->client || other->s.eType == ET_NPC))
	{
		if (!OnSameTeam(self, other))
		{
			if (!ValidEnemySeenTimeCheck(self, other))
				return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...
		}
	}

	if (self->client && (other->client || other->s.eType == ET_NPC))
	{
		if (!OnSameTeam(self, other))
			return CanSuppress( self, other );
	}

	AngleVectors( self->client->ps.viewangles, forward, right, up );
	//CalcMuzzlePoint( self, self->client->ps.weapon, forward, right, up, selfView );
	VectorCopy( self->r.currentOrigin, selfView);
	selfView[2]+=48;

#ifdef __BOT_FAST_VISIBILITY__
	{
		int closestNodeSelf = AIMOD_VISIBILITY_ClosestNode( self );
		int closestNodeOther = AIMOD_VISIBILITY_ClosestNode( other );
		if ( closestNodeSelf >= 0 && closestNodeOther >= 0 )
		{
			return ( AIMOD_VISIBILITY_CheckVisibility( closestNodeSelf, closestNodeOther) );
		}
	}
#endif //__BOT_FAST_VISIBILITY__

#ifdef __NPC__
	if ( other->NPC_client )
	{
		VectorCopy( other->r.currentOrigin, otherView );			//copy the target's origin
		otherView[2] += ( other->NPC_client->ps.viewheight * 0.5 ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_PLAYERSOLID );
	}
	else
#endif //__NPC__
	if ( other->client )
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += ( other->client->ps.viewheight-8 ); //add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_PLAYERSOLID );
	}
	else
	{
		VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
		otherView[2] += ( 48 * 0.5 );							//add the target's viewheight

		//check if a shot from the bot's viewheight to the player's viewheight would hit
		trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_PLAYERSOLID );
	}

	traceEnt = &g_entities[tr.entityNum];							//set traceEnt to the entity the shot would hit

	if ( self->client && other->client && !OnSameTeam(self, other) )
	{// Are we looking for an enemy visibility? Check our trace end is not friendly!
		if ( traceEnt->client && OnSameTeam(self, traceEnt) )
		{// NEVER shoot a friendly!
			return ( qfalse );
		}
	}

	if ( traceEnt == other || tr.fraction == 1 )										//if it would hit the player, return true
	{
		return ( qtrue );
	}

	return CanSuppress( self, other );
}


/* */
qboolean
MyVisible_OLD ( gentity_t *self, gentity_t *other )
{
	vec3_t		selfView, otherView;						//basically stores the locations of the players' eyes
	trace_t		tr;											//used in performing the collision info
	gentity_t	*traceEnt;									//holds the entity returned by the trace function
	vec3_t		original_mins, original_maxs;
	vec3_t		original_r_mins, original_r_maxs;

	if (MyVisible_Fast( self, other ))
		return qtrue;

#ifdef __NPC__
	if ( other->s.eType != ET_NPC && other->s.eType != ET_PLAYER )
	{
		return ( qfalse );
	}

	if ( !other->client && !other->NPC_client )
	{
		return ( qfalse );
	}

#else //!__NPC__
	if ( other->s.eType != ET_PLAYER )
	{
		return ( qfalse );
	}

	if ( !other->client )
	{
		return ( qfalse );
	}
#endif //__NPC__
#ifdef __BOT_FAST_VISIBILITY__
	{
		int closestNodeSelf = AIMOD_VISIBILITY_ClosestNode( self );
		int closestNodeOther = AIMOD_VISIBILITY_ClosestNode( other );
		if ( closestNodeSelf >= 0 && closestNodeOther >= 0 )
		{
			return ( AIMOD_VISIBILITY_CheckVisibility( closestNodeSelf, closestNodeOther) );
		}
	}
#endif //__BOT_FAST_VISIBILITY__
	if
	(
		self->client &&
		other->client &&
		infront( self, other) &&
		(self->client->ps.eFlags & EF_MG42_ACTIVE || self->client->ps.eFlags & EF_MOUNTEDTANK)
	)
	{														// Look from forward of the gun so its not in our LOS...
		vec3_t	forward;
		AngleVectors( self->client->ps.viewangles, forward, NULL, NULL );
		VectorMA( self->r.currentOrigin, 64, forward, selfView );
	}

#ifdef __NPC__
	else if
		(
			self->NPC_client &&
			infront( self, other) &&
			(self->NPC_client->ps.eFlags & EF_MG42_ACTIVE || self->NPC_client->ps.eFlags & EF_MOUNTEDTANK)
		)
	{														// Look from forward of the gun so its not in our LOS...
		vec3_t	forward;
		AngleVectors( self->NPC_client->ps.viewangles, forward, NULL, NULL );
		VectorMA( self->r.currentOrigin, 64, forward, selfView );
	}
#endif //__NPC__
	else
	{
		VectorCopy( self->r.currentOrigin, selfView );		//copy the bot's origin to this variable
	}

#ifdef __NPC__
	if ( self->NPC_client )
	{
		selfView[2] += self->NPC_client->ps.viewheight;		//add the bot's viewheight	
	}
	else
#endif //__NPC__
		selfView[2] += self->client->ps.viewheight;			//add the bot's viewheight	
	VectorCopy( other->r.currentOrigin, otherView );		//copy the target's origin
#ifdef __NPC__
	if ( other->NPC_client )
	{
		otherView[2] += other->NPC_client->ps.viewheight;	//add the target's viewheight
	}
	else
#endif //__NPC__
		otherView[2] += other->client->ps.viewheight;		//add the target's viewheight
	if ( other->client )
	{														// Make them easier to spot...
		VectorCopy( other->client->ps.mins, original_mins );
		VectorCopy( other->client->ps.maxs, original_maxs );
		VectorCopy( other->r.mins, original_r_mins );
		VectorCopy( other->r.maxs, original_r_maxs );
		other->client->ps.maxs[0] += 16;
		other->client->ps.maxs[1] += 16;
		other->client->ps.maxs[2] += otherView[2] + 16;
		other->r.maxs[0] += 16;
		other->r.maxs[1] += 16;
		other->r.maxs[2] += otherView[2] + 16;
	}
	else
	{
		VectorCopy( other->r.mins, original_r_mins );
		VectorCopy( other->r.maxs, original_r_maxs );
		other->r.maxs[0] += 16;
		other->r.maxs[1] += 16;
		other->r.maxs[2] += otherView[2] + 16;
	}

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	if ( other->client )
	{
		VectorCopy( original_mins, other->client->ps.mins );
		VectorCopy( original_maxs, other->client->ps.maxs );
		VectorCopy( original_r_mins, other->r.mins );
		VectorCopy( original_r_maxs, other->r.maxs );
	}
	else
	{
		VectorCopy( original_r_mins, other->r.mins );
		VectorCopy( original_r_maxs, other->r.maxs );
	}

	traceEnt = &g_entities[tr.entityNum];					//set traceEnt to the entity the shot would hit
	if ( traceEnt == other || tr.fraction == 1 )								//if it would hit the player, return true
	{
		return ( qtrue );
	}

	//	if (EnemyRoughlyVisible(self, other))
	//		return qtrue;
	return ( qfalse );										//it would hit something other than the player (or nothing) so return false
}


/* */
qboolean
EnemyRoughlyVisible ( gentity_t *self, gentity_t *other )
{
	vec3_t		selfView, otherView;					//basically stores the locations of the players' eyes
	trace_t		tr;										//used in performing the collision info
	gentity_t	*traceEnt;								//holds the entity returned by the trace function
	vec3_t		origMins, origMaxs;

	if (MyVisible_Fast( self, other ))
		return qtrue;

	if (self->client && (other->client || other->s.eType == ET_NPC))
	{
		if (!OnSameTeam(self, other))
		{
			if (!ValidEnemySeenTimeCheck(self, other))
				return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...
		}
	}

#ifdef __NPC__
	if ( other->s.eType != ET_NPC && other->s.eType != ET_PLAYER )
	{
		return ( qfalse );
	}

	if ( !other->client && !other->NPC_client )
	{
		return ( qfalse );
	}

#else //!__NPC__
	if ( other->s.eType != ET_PLAYER )
	{
		return ( qfalse );
	}

	if ( !other->client )
	{
		return ( qfalse );
	}
#endif //__NPC__
#ifdef __BOT_FAST_VISIBILITY__
	{
		int closestNodeSelf = AIMOD_VISIBILITY_ClosestNode( self );
		int closestNodeOther = AIMOD_VISIBILITY_ClosestNode( other );
		if ( closestNodeSelf >= 0 && closestNodeOther >= 0 )
		{
			return ( AIMOD_VISIBILITY_CheckVisibility( closestNodeSelf, closestNodeOther) );
		}
	}
#endif //__BOT_FAST_VISIBILITY__
	VectorCopy( other->r.mins, origMins );
	VectorCopy( other->r.maxs, origMaxs );
	other->r.mins[0] = -64;
	other->r.mins[1] = -64;
	other->r.mins[2] = -96;
	other->r.maxs[0] = 64;
	other->r.maxs[1] = 64;
	other->r.maxs[2] = 96;
	if
	(
		self->client &&
		other->client &&
		infront( self, other) &&
		(self->client->ps.eFlags & EF_MG42_ACTIVE || self->client->ps.eFlags & EF_MOUNTEDTANK)
	)
	{													// Look from forward of the gun so its not in our LOS...
		vec3_t	forward;
		AngleVectors( self->client->ps.viewangles, forward, NULL, NULL );
		VectorMA( self->r.currentOrigin, 64, forward, selfView );
	}

#ifdef __NPC__
	else if
		(
			self->NPC_client &&
			infront( self, other) &&
			(self->NPC_client->ps.eFlags & EF_MG42_ACTIVE || self->NPC_client->ps.eFlags & EF_MOUNTEDTANK)
		)
	{													// Look from forward of the gun so its not in our LOS...
		vec3_t	forward;
		AngleVectors( self->NPC_client->ps.viewangles, forward, NULL, NULL );
		VectorMA( self->r.currentOrigin, 64, forward, selfView );
	}
#endif //__NPC__
	else
	{
		VectorCopy( self->r.currentOrigin, selfView );	//copy the bot's origin to this variable
	}

#ifdef __NPC__
	if ( self->NPC_client )
	{
		selfView[2] += self->NPC_client->ps.viewheight; //add the bot's viewheight	
	}
	else
#endif //__NPC__
		selfView[2] += self->client->ps.viewheight;		//add the bot's viewheight	
	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin

	/*#ifdef __NPC__
	if (other->NPC_client)
		otherView[2] += other->NPC_client->ps.viewheight;		//add the target's viewheight
	else
#endif //__NPC__
		otherView[2] += other->client->ps.viewheight;		//add the target's viewheight*/
	otherView[2] += 64;									// Because we have a larger bbox...

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	VectorCopy( origMins, other->r.mins );
	VectorCopy( origMaxs, other->r.maxs );
	if ( traceEnt == other || tr.fraction == 1 )							//if it would hit the player, return true
	{
		return ( qtrue );
	}

	//if (RoughlyVisible(self, other))
	//	return qtrue;
	return ( qfalse );									//it would hit something other than the player (or nothing) so return false
}


/* */
qboolean
RoughlyVisible ( gentity_t *self, gentity_t *other )
{														// For wizz sounds...
	vec3_t		selfView, otherView;					//basically stores the locations of the players' eyes
	trace_t		tr;										//used in performing the collision info
	gentity_t	*traceEnt;								//holds the entity returned by the trace function
	vec3_t		origMins, origMaxs;

	if (MyVisible_Fast( self, other ))
		return qtrue;

	if (self->client && (other->client || other->s.eType == ET_NPC))
	{
		if (!OnSameTeam(self, other))
		{
			if (!ValidEnemySeenTimeCheck(self, other))
				return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...
		}
	}

#ifdef __NPC__
	if ( other->s.eType != ET_NPC && other->s.eType != ET_PLAYER )
	{
		return ( qfalse );
	}

	if ( !other->client && !other->NPC_client )
	{
		return ( qfalse );
	}

#else //!__NPC__
	if ( other->s.eType != ET_PLAYER )
	{
		return ( qfalse );
	}

	if ( !other->client )
	{
		return ( qfalse );
	}
#endif //__NPC__
#ifdef __BOT_FAST_VISIBILITY__
	{
		int closestNodeSelf = AIMOD_VISIBILITY_ClosestNode( self );
		int closestNodeOther = AIMOD_VISIBILITY_ClosestNode( other );
		if ( closestNodeSelf >= 0 && closestNodeOther >= 0 )
		{
			return ( AIMOD_VISIBILITY_CheckVisibility( closestNodeSelf, closestNodeOther) );
		}
	}
#endif //__BOT_FAST_VISIBILITY__
	VectorCopy( other->r.mins, origMins );
	VectorCopy( other->r.maxs, origMaxs );
	other->r.mins[0] = -256;
	other->r.mins[1] = -256;
	other->r.mins[2] = -256;
	other->r.maxs[0] = 256;
	other->r.maxs[1] = 256;
	other->r.maxs[2] = 256;
	if
	(
		self->client &&
		other->client &&
		infront( self, other) &&
		(self->client->ps.eFlags & EF_MG42_ACTIVE || self->client->ps.eFlags & EF_MOUNTEDTANK)
	)
	{													// Look from forward of the gun so its not in our LOS...
		vec3_t	forward;
		AngleVectors( self->client->ps.viewangles, forward, NULL, NULL );
		VectorMA( self->r.currentOrigin, 64, forward, selfView );
	}

#ifdef __NPC__
	else if
		(
			self->NPC_client &&
			infront( self, other) &&
			(self->NPC_client->ps.eFlags & EF_MG42_ACTIVE || self->NPC_client->ps.eFlags & EF_MOUNTEDTANK)
		)
	{													// Look from forward of the gun so its not in our LOS...
		vec3_t	forward;
		AngleVectors( self->NPC_client->ps.viewangles, forward, NULL, NULL );
		VectorMA( self->r.currentOrigin, 64, forward, selfView );
	}
#endif //__NPC__
	else
	{
		VectorCopy( self->r.currentOrigin, selfView );	//copy the bot's origin to this variable
	}

#ifdef __NPC__
	if ( self->NPC_client )
	{
		selfView[2] += self->NPC_client->ps.viewheight; //add the bot's viewheight	
	}
	else
#endif //__NPC__
		selfView[2] += self->client->ps.viewheight;		//add the bot's viewheight	
	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin

	/*#ifdef __NPC__
	if (other->NPC_client)
		otherView[2] += other->NPC_client->ps.viewheight;		//add the target's viewheight
	else
#endif //__NPC__
		otherView[2] += other->client->ps.viewheight;		//add the target's viewheight*/
	otherView[2] += 64;

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	VectorCopy( origMins, other->r.mins );
	VectorCopy( origMaxs, other->r.maxs );
	if ( traceEnt == other || tr.fraction == 1 )							//if it would hit the player, return true
	{
		return ( qtrue );
	}

	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin
	otherView[2] += 96;

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	VectorCopy( origMins, other->r.mins );
	VectorCopy( origMaxs, other->r.maxs );
	if ( traceEnt == other || tr.fraction == 1 )							//if it would hit the player, return true
	{
		return ( qtrue );
	}

	return ( qfalse );									//it would hit something other than the player (or nothing) so return false
}


/* */
qboolean
RoughlyVisible3 ( gentity_t *self, gentity_t *other )
{
	vec3_t	selfView, otherView;						//basically stores the locations of the players' eyes
	trace_t tr;

	if (MyVisible_Fast( self, other ))
		return qtrue;

	if (self->client && (other->client || other->s.eType == ET_NPC))
	{
		if (!OnSameTeam(self, other))
		{
			if (!ValidEnemySeenTimeCheck(self, other))
				return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...
		}
	}

	if ( self == other )
	{
		return ( qfalse );
	}

	VectorCopy( self->r.currentOrigin, selfView );		//copy the bot's origin to this variable
#ifdef __NPC__
	if ( self->NPC_client )
	{
		selfView[2] += self->NPC_client->ps.viewheight; //add the bot's viewheight	
	}
	else
#endif //__NPC__
		selfView[2] += self->client->ps.viewheight;		//add the bot's viewheight	
	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin
	otherView[2] += 60;									//look above them...
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	if ( tr.fraction < 0.9 )
	{
		return ( qtrue );
	}

	return ( qtrue );
}


/* */
qboolean
RoughlyVisible2 ( gentity_t *self, gentity_t *other )
{
	vec3_t		selfView, otherView;					//basically stores the locations of the players' eyes
	trace_t		tr;										//used in performing the collision info
	gentity_t	*traceEnt;								//holds the entity returned by the trace function
	VectorCopy( self->r.currentOrigin, selfView );		//copy the bot's origin to this variable

	if (MyVisible_Fast( self, other ))
		return qtrue;

	if (self->client && (other->client || other->s.eType == ET_NPC))
	{
		if (!OnSameTeam(self, other))
		{
			if (!ValidEnemySeenTimeCheck(self, other))
				return qfalse; // UQ1: Make sure the enemy has actually been really seen recently before allowing supression, etc...
		}
	}

#ifdef __NPC__
	if ( self->NPC_client )
	{
		selfView[2] += self->NPC_client->ps.viewheight; //add the bot's viewheight	
	}
	else
#endif //__NPC__
		selfView[2] += self->client->ps.viewheight;		//add the bot's viewheight	

	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin
	otherView[2] += 40;									//add the target's viewheight

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	if ( traceEnt == other || tr.fraction == 1)
	{													//if it would hit the player, return true
		return ( qtrue );
	}

	otherView[2] += 40;									//add the target's viewheight

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	if ( traceEnt == other || tr.fraction == 1 )
	{													//if it would hit the player, return true
		return ( qtrue );
	}

	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin
	otherView[1] += 40;
	otherView[2] += 60;									//add the target's viewheight

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	if ( traceEnt == other || tr.fraction == 1 )
	{													//if it would hit the player, return true
		return ( qtrue );
	}

	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin
	otherView[1] -= 40;
	otherView[2] += 60;									//add the target's viewheight

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	if ( traceEnt == other || tr.fraction == 1 )
	{													//if it would hit the player, return true
		return ( qtrue );
	}

	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin
	otherView[0] += 40;
	otherView[2] += 60;									//add the target's viewheight

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	if ( traceEnt == other || tr.fraction == 1 )
	{													//if it would hit the player, return true
		return ( qtrue );
	}

	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin
	otherView[0] -= 40;
	otherView[2] += 60;									//add the target's viewheight

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	if ( traceEnt == other || tr.fraction == 1 )
	{													//if it would hit the player, return true
		return ( qtrue );
	}

	return ( qfalse );									//it would hit something other than the player (or nothing) so return false
}


/* */
qboolean
MyObjectVisible ( gentity_t *self, gentity_t *other )
{
	vec3_t		selfView, otherView;					//basically stores the locations of the players' eyes
	trace_t		tr;										//used in performing the collision info
	gentity_t	*traceEnt;								//holds the entity returned by the trace function

	if (MyVisible_Fast( self, other ))
		return qtrue;

#ifdef __BOT_FAST_VISIBILITY__
	{
		int closestNodeSelf = AIMOD_VISIBILITY_ClosestNode( self );
		int closestNodeOther = AIMOD_VISIBILITY_ClosestNode( other );
		if ( closestNodeSelf >= 0 && closestNodeOther >= 0 )
		{
			return ( AIMOD_VISIBILITY_CheckVisibility( closestNodeSelf, closestNodeOther) );
		}
	}
#endif //__BOT_FAST_VISIBILITY__
	VectorCopy( self->r.currentOrigin, selfView );		//copy the bot's origin to this variable
#ifdef __NPC__
	if ( self->NPC_client )
	{
		selfView[2] += self->NPC_client->ps.viewheight; //add the bot's viewheight	
	}
	else
#endif //__NPC__
		selfView[2] += self->client->ps.viewheight;		//add the bot's viewheight	
	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin
	otherView[2] += 40;									//add the target's viewheight

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	if ( traceEnt == other || tr.fraction == 1 )
	{													//if it would hit the player, return true
		return ( qtrue );
	}

	return ( qfalse );									//it would hit something other than the player (or nothing) so return false
}
