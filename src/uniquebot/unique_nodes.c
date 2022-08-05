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

/*
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
NODES FUNCTION LISTING
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
AIMOD_NODES_SaveNodes
AIMOD_NODES_LoadNodes
WaypointThink
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
*/

/*
                   .___             
   ____   ____   __| _/____   ______
  /    \ /  _ \ / __ |/ __ \ /  ___/
 |   |  (  <_> ) /_/ \  ___/ \___ \ 
 |___|  /\____/\____ |\___  >____  >
      \/            \/    \/     \/ 

    uniquebot_nodes.c 
    Pathing routines for the FUSION bot.
================================================================
Should you decide to release a modified version of FUSION, you MUST
include the following text in the documentation and source of your 
modification*/
#include "../game/g_local.h"
#include "../botai/ai_main.h"
qboolean		nodes_loaded = qfalse;

// Flags*/
//-------------------------------------------------------
qboolean		newmap = qtrue;

// Total number of nodes that are items
//-------------------------------------------------------
int				number_of_itemnodes;

// Total number of nodes
//-------------------------------------------------------
int				number_of_nodes;

// array for node data
//-------------------------------------------------------
//node_t			nodes[MAX_NODES];
node_t			*nodes;
node_convert_t	nodes_convert[MAX_NODES];


/* */
void
AIMOD_NODES_AddLinks ( void )
{

} extern void	AIMOD_MAPPING_CreateNodeConnection ( int from, int to );
extern void		AIMOD_MAPPING_SetConnectionCost ( int from, int to, int new_cost );

//===========================================================================
// Routine      : AIMOD_NODES_SaveNodes
// Description  : Save nodes to .nod file on disk
extern void		AIMOD_MAPPING_MakeLinks ( void );
#ifdef __BOT_OLD_NOD_FORMAT__


/* */
void
AIMOD_NODES_SaveNodes ( void )
{
	FILE		*pOut;
	char		filename[60];
	int			j;
	int			version = 6;
	vmCvar_t	mapname;
	qboolean	dorebuild = qtrue;
	j = 0;
	if ( !dorebuild )
	{
		G_Printf( "Nodes not saved, Node Table already exists!" );
		return;
	}

	// Resolve paths
	//-------------------------------------------------------
	AIMOD_MAPPING_MakeLinks();
	G_Printf( "^3*** ^3%s: ^7Saving node table.\n", GAME_VERSION );
	strcpy( filename, "uniquemod/nodes/" );

	////////////////////
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

	///////////////////
	strcat( filename, ".nod" );
	if ( (pOut = fopen( filename, "wb")) == NULL )
	{
		return; // bail
	}

	// Write Version
	fwrite( &version, sizeof(int), 1, pOut );

	// Write Count
	fwrite( &number_of_nodes, sizeof(int), 1, pOut );

	// Write Nodes
	fwrite( nodes, sizeof(node_t), number_of_nodes, pOut );
	fclose( pOut );
	G_Printf( "^3*** ^3%s: ^7Node table saved %i nodes to file %s.\n", GAME_VERSION, number_of_nodes, filename );
}
#endif //__BOT_OLD_NOD_FORMAT__


//===========================================================================
// Routine      : NodesLoaded

// Description  : Have we got loaded nodes?
qboolean
NodesLoaded ( void )
{
	if ( nodes_loaded == qtrue )
	{
		return ( qtrue );
	}
	else
	{
		return ( qfalse );
	}
}

//===========================================================================
// Routine      : AIMOD_NODES_LoadNodes
// Description  : Loads nodes from .nod file on disk
extern void AIMOD_MAPPING_MakeLinks ( void );
extern void BotMP_Update_Goal_Lists ( void );
extern int	GoalType ( int entNum, int teamNum );
extern int	num_bot_objectives;
extern int	num_bot_goals;
extern int	BotObjectives[MAX_GENTITIES];
extern int	BotGoals[MAX_GENTITIES];


/* */
void
AIMOD_NODES_SetObjectiveFlags ( int node )
{										// Find objects near this node.
	int loop = 0;
	if ( nodes[node /*number_of_nodes*/ ].type & NODE_AXIS_UNREACHABLE )
	{
		nodes[node].objTeam |= TEAM_ALLIES;
	}
	else if ( nodes[node /*number_of_nodes*/ ].type & NODE_ALLY_UNREACHABLE )
	{
		nodes[node].objTeam |= TEAM_AXIS;
	}

	// Init them first...
	nodes[node].objectNum[0] = nodes[node].objectNum[1] = nodes[node].objectNum[2] = ENTITYNUM_NONE;
	nodes[node].objEntity = -1;
	nodes[node].objFlags = -1;
	if ( !nodes[node].objTeam )
	{
		nodes[node].objTeam = -1;
	}

	nodes[node].objType = -1;
	if ( num_bot_objectives <= 0 && num_bot_goals <= 0 )
	{
		return;							// Have no goals..
	}

	// Now link objectives...
	for ( loop = 0; loop < num_bot_objectives; loop++ )
	{

		//		int nameNum = 0;
		int type = 0;
		if ( VectorDistance( g_entities[BotObjectives[loop]].r.currentOrigin, nodes[node].origin) > 96 )
		{
			continue;					// Too far dont even think about it... lol..
		}

		// NOTE: ALLIES are opposite for BUILD/DESTROY goals.
		type = GoalType( BotObjectives[loop], TEAM_AXIS );
		if
		(
			type != BOTGOAL_CONSTRUCT &&
			type != BOTGOAL_DESTROY &&
			type != BOTGOAL_FLAG &&
			type != BOTGOAL_CHECKPOINT &&
			type != OBJECTIVE_CAPTURE &&
			type != BOTGOAL_POPFLAG
		)
		{
			continue;					// No point marking the others on nodes...
		}

		// We found an objective here...
		nodes[node].objectNum[0] = BotObjectives[loop];
		nodes[node].objEntity = BotObjectives[loop];
		nodes[node].objFlags = -1;		// Not currently used...
		if ( !nodes[node].objTeam )
		{
			nodes[node].objTeam = -1;	// Not currently used...
		}

		if ( type == BOTGOAL_CONSTRUCT )
		{
			nodes[node].objType = OBJECTIVE_BUILD;
		}
		else if ( type == BOTGOAL_DESTROY )
		{
			nodes[node].objType = OBJECTIVE_DYNAMITE;
		}
		else if ( type == BOTGOAL_FLAG || type == BOTGOAL_CHECKPOINT )
		{
			nodes[node].objType = OBJECTIVE_FLAG;
		}
		else if ( type == BOTGOAL_POPFLAG )
		{
			nodes[node].objType = OBJECTIVE_POPFLAG;
		}
		else if ( type == BOTGOAL_OBJECTIVE )
		{
			nodes[node].objType = OBJECTIVE_CAPTURE;
		}

		return;							// We're done here!
	}

	// Now link goals...
	for ( loop = 0; loop < num_bot_goals; loop++ )
	{

		//		int nameNum = 0;
		int type = 0;
		if ( VectorDistance( g_entities[BotGoals[loop]].r.currentOrigin, nodes[node].origin) > 96 )
		{
			continue;					// Too far dont even think about it... lol..
		}

		// NOTE: ALLIES are opposite for BUILD/DESTROY goals.
		type = GoalType( BotGoals[loop], TEAM_AXIS );
		if
		(
			type != BOTGOAL_CONSTRUCT &&
			type != BOTGOAL_DESTROY &&
			type != BOTGOAL_FLAG &&
			type != BOTGOAL_CHECKPOINT &&
			type != OBJECTIVE_CAPTURE &&
			type != BOTGOAL_POPFLAG
		)
		{
			continue;					// No point marking the others on nodes...
		}

		// We found an objective here...
		nodes[node].objectNum[0] = BotGoals[loop];
		nodes[node].objEntity = BotGoals[loop];
		nodes[node].objFlags = -1;		// Not currently used...
		if ( !nodes[node].objTeam )
		{
			nodes[node].objTeam = -1;	// Not currently used...
		}

		if ( type == BOTGOAL_CONSTRUCT )
		{
			nodes[node].objType = OBJECTIVE_BUILD;
		}
		else if ( type == BOTGOAL_DESTROY )
		{
			nodes[node].objType = OBJECTIVE_DYNAMITE;
		}
		else if ( type == BOTGOAL_FLAG || type == BOTGOAL_CHECKPOINT )
		{
			nodes[node].objType = OBJECTIVE_FLAG;
		}
		else if ( type == BOTGOAL_POPFLAG )
		{
			nodes[node].objType = OBJECTIVE_POPFLAG;
		}
		else if ( type == BOTGOAL_OBJECTIVE )
		{
			nodes[node].objType = OBJECTIVE_CAPTURE;
		}

		return;							// We're done here!
	}
}

// This was the old (pre-version 6) format flags... Using new ones, so we need conversion..
#define OLD_NODE_MOVE			0		// Move Node
#define OLD_NODE_OBJECTIVE		1
#define OLD_NODE_TARGET			2
#define OLD_NODE_TARGETSELECT	3
#define OLD_NODE_FASTHOP		4
#define OLD_NODE_COVER			5
#define OLD_NODE_WATER			6
#define OLD_NODE_LADDER			7		// Ladder Node
#define OLD_NODE_MG42			8
#define OLD_NODE_DYNAMITE		9
#define OLD_NODE_BUILD			10
#define OLD_NODE_JUMP			11
#define OLD_NODE_DUCK			12


/* */
int
AIMOD_NODES_Convert_Old_Flags ( int node )
{
	int newtype = 0;
	if ( nodes[node].type == OLD_NODE_OBJECTIVE )
	{
		newtype |= NODE_OBJECTIVE;
	}

	if ( nodes[node].type == OLD_NODE_FASTHOP )
	{
		newtype |= NODE_FASTHOP;
	}

	if ( nodes[node].type == OLD_NODE_WATER )
	{
		newtype |= NODE_WATER;
	}

	if ( nodes[node].type == OLD_NODE_LADDER )
	{
		newtype |= NODE_LADDER;
	}

	if ( nodes[node].type == OLD_NODE_MG42 )
	{
		newtype |= NODE_MG42;
	}

	if ( nodes[node].type == OLD_NODE_DYNAMITE )
	{
		newtype |= NODE_DYNAMITE;
	}

	if ( nodes[node].type == OLD_NODE_BUILD )
	{
		newtype |= NODE_BUILD;
	}

	if ( nodes[node].type == OLD_NODE_JUMP )
	{
		newtype |= NODE_JUMP;
	}

	if ( nodes[node].type == OLD_NODE_DUCK )
	{
		newtype |= NODE_DUCK;
	}

	return ( newtype );
}

#ifdef __BOT_OLD_NOD_FORMAT__


/* */
void
AIMOD_NODES_LoadNodes ( void )
{
	FILE		*pIn;
	int			i, j /*, counter, axisenodecounter, alliedenodecounter*/ ;
	char		filename[60];
	int			version;
	vmCvar_t	mapname;
	qboolean	dorebuild = qtrue;

	// Set up bot goal entity lists...
	BotMP_Update_Goal_Lists();
	for ( i = 0; i < max_clients; i++ )
	{
		VectorSet( g_entities[i].last_added_waypoint, 0, 0, 0 );
	}

	i = 0;
	j = 0;
	strcpy( filename, "uniquemod/nodes/" );

	////////////////////
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

	///////////////////
	//strcat(filename,level.mapname);
	strcat( filename, ".nod" );
	if ( (pIn = fopen( filename, "rb")) == NULL )
	{

		// Create item table
		//-------------------------------------------------------
		G_Printf( "^1*** ^3%s^1: ^5 No node file found, please create one...", GAME_VERSION );

		//        AIMOD_ITEMS_BuildItemNodeTable(qfalse);
		dorebuild = qtrue;
		G_Printf( "done.\n" );

		//-------------------------------------------------------
		return;
	}

	// determine version
	//-------------------------------------------------------
	// Read Version
	fread( &version, sizeof(int), 1, pIn );
	if ( version == 3 )
	{				// Load and convert the old style node table.
		G_Printf( "^1*** ^3%s^1: ^5 Converting old style (version 3) node table...", GAME_VERSION );

		// Read Count
		fread( &number_of_nodes, sizeof(int), 1, pIn );
		fread( nodes_convert, sizeof(node_convert_t), number_of_nodes, pIn );
		fclose( pIn );
		for ( i = 0; i < number_of_nodes; i++ )
		{
			for ( j = 0; j < MAX_NODELINKS; j++ )
			{
				if
				(
					!(
						nodes_convert[i].links[j].targetNode >= 0 &&
						nodes_convert[i].links[j].targetNode < number_of_nodes
					)
				)
				{	// Initialize unused links...
					nodes_convert[i].links[j].targetNode = INVALID;
					nodes_convert[i].links[j].cost = 999999;
				}

				// Copy the node table to the real one...
				nodes[i].enodenum = nodes_convert[i].enodenum;
				VectorCopy( nodes_convert[i].origin, nodes[i].origin );
				AIMOD_NODES_SetObjectiveFlags( i );
				nodes[i].links[j].targetNode = nodes_convert[i].links[j].targetNode;
				nodes[i].links[j].cost = nodes_convert[i].links[j].cost;
				nodes[i].links[j].flags = 0;
				nodes[i].type = AIMOD_NODES_Convert_Old_Flags( i );
			}
		}

		G_Printf( "^7 DONE^5!\n" );

		// Make links and resave file...
		AIMOD_NODES_SaveNodes();
	}
	else if ( version == 4 )
	{				// Load and convert the old style node table.
		G_Printf( "^1*** ^3%s^1: ^5 Converting old style (version 4) node table...", GAME_VERSION );

		// Read Count
		fread( &number_of_nodes, sizeof(int), 1, pIn );
		fread( nodes_convert, sizeof(node_convert_t), number_of_nodes, pIn );
		fclose( pIn );
		for ( i = 0; i < number_of_nodes; i++ )
		{
			for ( j = 0; j < MAX_NODELINKS; j++ )
			{
				if
				(
					!(
						nodes_convert[i].links[j].targetNode >= 0 &&
						nodes_convert[i].links[j].targetNode < number_of_nodes
					)
				)
				{	// Initialize unused links...
					nodes_convert[i].links[j].targetNode = INVALID;
					nodes_convert[i].links[j].cost = 999999;
				}

				// Copy the node table to the real one...
				nodes[i].enodenum = nodes_convert[i].enodenum;
				VectorCopy( nodes_convert[i].origin, nodes[i].origin );
				AIMOD_NODES_SetObjectiveFlags( i );
				nodes[i].links[j].targetNode = nodes_convert[i].links[j].targetNode;
				nodes[i].links[j].cost = nodes_convert[i].links[j].cost;
				nodes[i].links[j].flags = 0;
				nodes[i].type = AIMOD_NODES_Convert_Old_Flags( i );
			}
		}

		G_Printf( "^7 DONE^5!\n" );

		// Make links and resave file...
		AIMOD_NODES_SaveNodes();
	}
	else if ( version == 5 )
	{				// Just load the new style node table.
		G_Printf( "^1*** ^3%s^1: ^5 Converting old style (version 5) node table...", GAME_VERSION );

		// Read Count
		fread( &number_of_nodes, sizeof(int), 1, pIn );
		fread( nodes, sizeof(node_t), number_of_nodes, pIn );
		for ( i = 0; i < number_of_nodes; i++ )
		{
			for ( j = 0; j < MAX_NODELINKS; j++ )
			{
				if ( !(nodes[i].links[j].targetNode >= 0 && nodes[i].links[j].targetNode < number_of_nodes) )
				{
					nodes[i].links[j].targetNode = INVALID;
					nodes[i].links[j].cost = 999999;
					nodes[i].links[j].flags = 0;
					nodes[i].objectNum[0] = nodes[i].objectNum[1] = nodes[i].objectNum[2] = ENTITYNUM_NONE;
				}

				// This version had incorrect node flags.. Fix them...
				AIMOD_NODES_SetObjectiveFlags( i );
				nodes[i].type = AIMOD_NODES_Convert_Old_Flags( i );
			}
		}

		fclose( pIn );
		G_Printf( "^7 DONE^5!\n" );

		// Make links and resave file...
		AIMOD_NODES_SaveNodes();
	}
	else if ( version == 6 )
	{				// Just load the new style node table.
		G_Printf( "^1*** ^3%s^1: ^5 Loading node table...", GAME_VERSION );

		// Read Count
		fread( &number_of_nodes, sizeof(int), 1, pIn );
		fread( nodes, sizeof(node_t), number_of_nodes, pIn );
		for ( i = 0; i < number_of_nodes; i++ )
		{
			for ( j = 0; j < MAX_NODELINKS; j++ )
			{
				if ( !(nodes[i].links[j].targetNode >= 0 && nodes[i].links[j].targetNode < number_of_nodes) )
				{
					nodes[i].links[j].targetNode = INVALID;
					nodes[i].links[j].cost = 999999;
					nodes[i].links[j].flags = 0;
					nodes[i].objectNum[0] = nodes[i].objectNum[1] = nodes[i].objectNum[2] = ENTITYNUM_NONE;
				}
			}
		}

		fclose( pIn );
		G_Printf( "^7 DONE^5!\n" );
	}
	else
	{

		// Create item table
		//-------------------------------------------------------
		G_Printf( "^1*** ^3%s^1: ^5 No node file found, please create one...", GAME_VERSION );

		//        AIMOD_ITEMS_BuildItemNodeTable(qfalse);
		dorebuild = qtrue;
		G_Printf( "done.\n" );

		//-------------------------------------------------------
		return;		// bail
	}

	G_Printf( "done.\n" );
	G_Printf( "^1*** ^3%s^1: ^5 Total Nodes: ^7%i^5.\n", GAME_VERSION, number_of_nodes );
	dorebuild = qfalse;
	nodes_loaded = qtrue;
}
#endif //__BOT_OLD_NOD_FORMAT__
#ifndef __BOT_OLD_NOD_FORMAT__

/*//////////////////////////////////////////////
AddNode
creates a new waypoint/node with the specified values
*/

/////////////////////////////////////////////
int numAxisOnlyNodes = 0;
int AxisOnlyFirstNode = -1;
int numAlliedOnlyNodes = 0;
int AlliedOnlyFirstNode = -1;


/* */
qboolean
Load_AddNode ( vec3_t origin, int fl, short int *ents, int objFl )
{

	//	int objTeam;
	//make sure we don't already have too many nodes
	//curently MAX_NODES is 8000
	if ( number_of_nodes + 1 > MAX_NODES )
	{
		return ( qfalse );
	}

	VectorCopy( origin, nodes[number_of_nodes].origin );	//set the node's position

	//nodes[number_of_nodes].targetNode = number_of_nodes;				//set it's number
	nodes[number_of_nodes].type = fl;						//set the flags (NODE_OBJECTIVE, for example)
	nodes[number_of_nodes].objectNum[0] = ents[0];			//set any map objects associated with this node
	nodes[number_of_nodes].objectNum[1] = ents[1];			//only applies to objects linked to the unreachable flag
	nodes[number_of_nodes].objectNum[2] = ents[2];
	nodes[number_of_nodes].objFlags = objFl;				//set the objective flags
	if ( nodes[number_of_nodes].type & NODE_AXIS_UNREACHABLE )
	{
		if ( AlliedOnlyFirstNode < 0 )
		{
			AlliedOnlyFirstNode = number_of_nodes;
		}

		//G_Printf("Node %i is allied only!\n", number_of_nodes);
		nodes[number_of_nodes].objTeam |= TEAM_ALLIES;
		numAlliedOnlyNodes++;
	}

	if ( nodes[number_of_nodes].type & NODE_ALLY_UNREACHABLE )
	{
		if ( AxisOnlyFirstNode < 0 )
		{
			AxisOnlyFirstNode = number_of_nodes;
		}

		//G_Printf("Node %i is axis only!\n", number_of_nodes);
		nodes[number_of_nodes].objTeam |= TEAM_AXIS;
		numAxisOnlyNodes++;
	}

	//get some objective-related data out of the flags (this is based on the .wps format - see the forum)

	/*	nodes[number_of_nodes].objEntity = ((objFl & 0x0FFF0) >> 7);
	nodes[number_of_nodes].objType = ((objFl & 0x0000F) >> 2);
	
	objTeam = (objFl & 1);
	if (objTeam == 0)
		nodes[number_of_nodes].objTeam = TEAM_ALLIES;
	else if (objTeam == 1)
		nodes[number_of_nodes].objTeam = TEAM_AXIS;
*/
	number_of_nodes++;
	return ( qtrue );
}

/*////////////////////////////////////////////////
ConnectNodes
Connects 2 nodes and sets the flags for the path between them
/*/


///////////////////////////////////////////////
qboolean
ConnectNodes ( int from, int to, int flags )
{

	//check that we don't have too many connections from the 'from' node already
	if ( nodes[from].enodenum + 1 > MAX_NODELINKS )
	{
		return ( qfalse );
	}

	//check that we are creating a path between 2 valid nodes
	if ( (nodes[from].type == NODE_INVALID) || (to == NODE_INVALID) || from > MAX_NODES || from < 0 || to > MAX_NODES || to < 0 )
	{	//nodes[to].type is invalid on LoadNodes()
		return ( qfalse );
	}

	//update the individual nodes
	nodes[from].links[nodes[from].enodenum].targetNode = to;
	nodes[from].links[nodes[from].enodenum].flags = flags;
	nodes[from].enodenum++;
	return ( qtrue );
}

#define BOT_MOD_NAME	"aimod"
//#define NOD_VERSION		1.1f
float NOD_VERSION = 1.1f;


/* */
void
AIMOD_NODES_FixAASNodes ( void )
{
	int i;
	G_Printf( "^1*** ^3%s^5: Repairing loaded AAS node file.\n", GAME_VERSION );
	for ( i = 0; i < number_of_nodes; i++ )
	{
		trace_t tr;
		vec3_t	tempPos;
		VectorCopy( nodes[i].origin, tempPos );
		tempPos[2] -= 32000;
		trap_Trace( &tr, nodes[i].origin, NULL, NULL, tempPos, ENTITYNUM_NONE, MASK_SOLID );
		VectorCopy( tr.endpos, nodes[i].origin );
		nodes[i].origin[2] += 16;
	}

	G_Printf( "^1*** ^3%s^5: Loaded AAS node file repaired.\n", GAME_VERSION );
	AIMOD_NODES_SaveNodes();
}

/*/////////////////////////////////////////////
LoadNodes
Initializes the nodes for a map from a file
The file must be in a .pk3 file in the mod folder
This function is completely dependent on the file format for the node files
as well as the format of the node_t structure
*/

/////////////////////////////////////////////
qboolean	fix_aas_nodes = qfalse;
extern vmCvar_t fs_game;

void AIMOD_NODES_LoadNodes2 ( void );

/* */
void
AIMOD_NODES_LoadNodes ( void )
{
	FILE			*f;
	int				i, j;
	char			filename[60];
	vmCvar_t		mapname;
	short int		objNum[3] = { 0, 0, 0 },
	objFlags, numLinks;
	int				flags;
	vec3_t			vec;
	short int		fl2;
	int				target;
	char			name[] = BOT_MOD_NAME;
	char			nm[64] = "";
	float			version;
	char			map[64] = "";
	char			mp[64] = "";
	/*short*/ int		numberNodes;
	short int		temp;

	//while (!BG_IsMemoryInitialized())
	//{
	//}

	// Set up bot goal entity lists...
	BotMP_Update_Goal_Lists();

	// Auto-Rounting...

	/*	for (i = 0; i < MAX_CLIENTS;i++)
	{
		VectorSet(g_entities[i].last_added_waypoint, 0,0,0);
	}*/
	i = 0;
	strcpy( filename, "nodes/" );

	////////////////////
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

	trap_Cvar_Register( &fs_game, "fs_game", "", CVAR_SERVERINFO | CVAR_ROM );
	//G_Printf("fs_game is %s\n", fs_game.string);

	///////////////////
	//open the node file for reading, return false on error
	if (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
	{
		f = fopen( va( "%s/nodes/%s_supremacy.bwp", fs_game.string, filename), "rb" );
		
		if ( !f )
		{
			f = fopen( va( "nodes/%s_supremacy.bwp", filename), "rb" );

			if ( !f )
			{
				f = fopen( va( "%s/nodes/%s.bwp", fs_game.string, filename), "rb" );

				if ( !f )
				{
					f = fopen( va( "nodes/%s.bwp", filename), "rb" );
				
					if ( !f )
					{
						//G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
						//G_Printf( "^1*** ^3       ^5  You need to make bot routes for this map.\n" );
						//G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n" );
						AIMOD_NODES_LoadNodes2();
						return;
					}
				}
			}
		}
	}
	else
	{
		f = fopen( va( "%s/nodes/%s.bwp", fs_game.string, filename), "rb" );
		
		if ( !f )
		{
			f = fopen( va( "nodes/%s.bwp", filename), "rb" );
		
			if ( !f )
			{
				//G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
				//G_Printf( "^1*** ^3       ^5  You need to make bot routes for this map.\n" );
				//G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n" );
				AIMOD_NODES_LoadNodes2();
				return;
			}
		}
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_ROM | CVAR_SERVERINFO );	//get the map name
	strcpy( mp, mapname.string );
	//trap_FS_Read( &nm, strlen( name) + 1, f );									//read in a string the size of the mod name (+1 is because all strings end in hex '00')
	fread( &nm, strlen( name) + 1, 1, f);
	/*if ( Q_stricmp( nm, name) != 0 )
	{
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
		G_Printf( "^1*** ^3       ^5  Incompatible or corrupt node file.\n", filename );
		G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n", filename );
		//trap_FS_FCloseFile( f );
		fclose(f);
		return;											//if the file doesn't start with the mod name, it's not a valid .nod file, so abort
	}*/

	//trap_FS_Read( &version, sizeof(float), f );			//read and make sure the version is the same
	fread( &version, sizeof(float), 1, f);
	if ( version != NOD_VERSION && version != 1.0f)
	{
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
		G_Printf( "^1*** ^3       ^5  Old node file detected.\n" );
		G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n" );
		//trap_FS_FCloseFile( f );
		fclose(f);
		return;
	}

	//trap_FS_Read( &map, strlen( mp) + 1, f );			//make sure the file is for the current map
	fread( &map, strlen( mp) + 1, 1, f);
	if ( Q_stricmp( map, mp) != 0 )
	{
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
		G_Printf( "^1*** ^3       ^5  Node file is not for this map!\n" );
		G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n" );
		//trap_FS_FCloseFile( f );
		fclose(f);
		return;
	}

	//trap_FS_Read( &numberNodes, sizeof(short int), f ); //read in the number of nodes in the map
	if (version == NOD_VERSION)
	{
		fread( &numberNodes, sizeof(/*short*/ int), 1, f);
	}
	else
	{
		fread( &temp, sizeof(short int), 1, f);
		numberNodes = temp;
	}

	for (i = 0; i < number_of_nodes; i++)
	{// Init all nodes!
		nodes[i].enodenum = 0;
		nodes[i].objFlags = 0;
		nodes[i].objEntity = 0;
		nodes[i].objTeam = 0;
		nodes[i].objType = 0;
		nodes[i].type = 0;
	}

	number_of_nodes = 0;

	for ( i = 0; i < numberNodes; i++ )					//loop through all the nodes
	{
		//read in all the node info stored in the file
		//trap_FS_Read(&num, sizeof(short int), f);			//used for links, but not written directly to nodes
		//trap_FS_Read( &vec, sizeof(vec3_t), f );
		fread( &vec, sizeof(vec3_t), 1, f);
		//trap_FS_Read( &flags, sizeof(int), f );
		fread( &flags, sizeof(int), 1, f);
		//trap_FS_Read( objNum, sizeof(short int) * 3, f );
		fread( objNum, (sizeof(short int) * 3), 1, f);
		//trap_FS_Read( &objFlags, sizeof(short int), f );
		fread( &objFlags, sizeof(short int), 1, f);
		//trap_FS_Read( &numLinks, sizeof(short int), f );
		fread( &numLinks, sizeof(short int), 1, f);

		/*		VectorCopy(vec, nodes[i].origin);
		nodes[i].objectNum = objNum;
		nodes[i].objFlags = objFlags;
		nodes[i].enodenum = numLinks;
		nodes[i].objEntity = -1;
		nodes[i].objTeam = 0;
		nodes[i].objType = -1;
		nodes[i].type = flags;*/
#ifndef _WIN32
		Load_AddNode( vec, flags, objNum, objFlags );	//add the node
#else //_WIN32
		VectorCopy( vec, nodes[i].origin );	//set the node's position

		nodes[i].type = flags;						//set the flags (NODE_OBJECTIVE, for example)
		nodes[i].objectNum[0] = objNum[0];			//set any map objects associated with this node
		nodes[i].objectNum[1] = objNum[1];			//only applies to objects linked to the unreachable flag
		nodes[i].objectNum[2] = objNum[2];
		nodes[i].objFlags = objFlags;				//set the objective flags
	
		if ( nodes[i].type & NODE_AXIS_UNREACHABLE )
		{
			if ( AlliedOnlyFirstNode < 0 )
			{
				AlliedOnlyFirstNode = i;
			}

			nodes[i].objTeam |= TEAM_ALLIES;
			numAlliedOnlyNodes++;
		}

		if ( nodes[i].type & NODE_ALLY_UNREACHABLE )
		{
			if ( AxisOnlyFirstNode < 0 )
			{
				AxisOnlyFirstNode = i;
			}

			nodes[i].objTeam |= TEAM_AXIS;
			numAxisOnlyNodes++;
		}

		number_of_nodes++;
#endif //_WIN32

		//loop through all of the links and read the data
		for ( j = 0; j < numLinks; j++ )
		{
			//trap_FS_Read( &target, sizeof(short int), f );
			if (version == NOD_VERSION)
			{
				fread( &target, sizeof(/*short*/ int), 1, f);
			}
			else
			{
				fread( &temp, sizeof(short int), 1, f);
				target = temp;
			}

			//trap_FS_Read( &fl2, sizeof(short int), f );
			fread( &fl2, sizeof(short int), 1, f);
			ConnectNodes( i, target, fl2 );				//add any links
		}

		// Set node objective flags..
		AIMOD_NODES_SetObjectiveFlags( i );
	}

	//trap_FS_Read( &fix_aas_nodes, sizeof(short int), f );
	fread( &fix_aas_nodes, sizeof(short int), 1, f);
	//trap_FS_FCloseFile( f );							//close the file
	fclose(f);
	G_Printf( "^1*** ^3%s^5: Successfully loaded %i waypoints from waypoint file ^7nodes/%s.bwp^5.\n", GAME_VERSION,
			  number_of_nodes, filename );
	nodes_loaded = qtrue;
	if ( fix_aas_nodes )
	{
		AIMOD_NODES_FixAASNodes();
	}

	//number_of_nodes = numberNodes;
	return;
}

/* */
void
AIMOD_NODES_LoadNodes2 ( void )
{
	fileHandle_t	f;
	int				i, j;
	char			filename[60];
	vmCvar_t		mapname;
	short int		objNum[3] = { 0, 0, 0 },
	objFlags, numLinks;
	int				flags;
	vec3_t			vec;
	short int		fl2;
	int				target;
	char			name[] = BOT_MOD_NAME;
	char			nm[64] = "";
	float			version;
	char			map[64] = "";
	char			mp[64] = "";
	/*short*/ int		numberNodes;
	short int		temp;

	//while (!BG_IsMemoryInitialized())
	//{
	//}

	// Set up bot goal entity lists...
	BotMP_Update_Goal_Lists();

	// Auto-Rounting...

	/*	for (i = 0; i < MAX_CLIENTS;i++)
	{
		VectorSet(g_entities[i].last_added_waypoint, 0,0,0);
	}*/
	i = 0;
	strcpy( filename, "nodes/" );

	////////////////////
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

	///////////////////
	//open the node file for reading, return false on error
	if (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
	{
		trap_FS_FOpenFile( va( "nodes/%s_supremacy.bwp", filename), &f, FS_READ );
		if ( !f )
		{
			/*trap_FS_FOpenFile( va( "nodes/%s.bwp", filename), &f, FS_READ );
			if ( !f )
			{
				//G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
				//G_Printf( "^1*** ^3       ^5  You need to make bot routes for this map.\n", filename );
				//G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n", filename );
				AIMOD_NODES_LoadNodes2();
				return;
			}*/
			G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
			G_Printf( "^1*** ^3       ^5  You need to make bot routes for this map.\n", filename );
			G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n", filename );
		}
	}
	else
	{
		trap_FS_FOpenFile( va( "nodes/%s.bwp", filename), &f, FS_READ );
		if ( !f )
		{
			G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
			G_Printf( "^1*** ^3       ^5  You need to make bot routes for this map.\n", filename );
			G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n", filename );
			//AIMOD_NODES_LoadNodes2();
			return;
		}
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_ROM | CVAR_SERVERINFO );	//get the map name
	strcpy( mp, mapname.string );
	trap_FS_Read( &nm, strlen( name) + 1, f );									//read in a string the size of the mod name (+1 is because all strings end in hex '00')
	/*if ( Q_stricmp( nm, name) != 0 )
	{
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
		G_Printf( "^1*** ^3       ^5  Incompatible or corrupt node file.\n", filename );
		G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n", filename );
		trap_FS_FCloseFile( f );
		return;											//if the file doesn't start with the mod name, it's not a valid .nod file, so abort
	}*/

	trap_FS_Read( &version, sizeof(float), f );			//read and make sure the version is the same
	if ( version != NOD_VERSION && version != 1.0f )
	{
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
		G_Printf( "^1*** ^3       ^5  Old node file detected.\n" );
		G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n" );
		trap_FS_FCloseFile( f );
		return;
	}

	trap_FS_Read( &map, strlen( mp) + 1, f );			//make sure the file is for the current map
	if ( Q_stricmp( map, mp) != 0 )
	{
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
		G_Printf( "^1*** ^3       ^5  Node file is not for this map!\n" );
		G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n" );
		trap_FS_FCloseFile( f );
		return;
	}

	if (version == NOD_VERSION)
	{
		trap_FS_Read( &numberNodes, sizeof(/*short*/ int), f ); //read in the number of nodes in the map
	}
	else
	{
		trap_FS_Read( &temp, sizeof(short int), f ); //read in the number of nodes in the map
		numberNodes = temp;
	}

	for (i = 0; i < number_of_nodes; i++)
	{// Init all nodes!
		nodes[i].enodenum = 0;
		nodes[i].objFlags = 0;
		nodes[i].objEntity = 0;
		nodes[i].objTeam = 0;
		nodes[i].objType = 0;
		nodes[i].type = 0;
	}

	number_of_nodes = 0;

	for ( i = 0; i < numberNodes; i++ )					//loop through all the nodes
	{
		//read in all the node info stored in the file
		//trap_FS_Read(&num, sizeof(short int), f);			//used for links, but not written directly to nodes
		trap_FS_Read( &vec, sizeof(vec3_t), f );
		trap_FS_Read( &flags, sizeof(int), f );
		trap_FS_Read( objNum, sizeof(short int) * 3, f );
		trap_FS_Read( &objFlags, sizeof(short int), f );
		trap_FS_Read( &numLinks, sizeof(short int), f );

		/*		VectorCopy(vec, nodes[i].origin);
		nodes[i].objectNum = objNum;
		nodes[i].objFlags = objFlags;
		nodes[i].enodenum = numLinks;
		nodes[i].objEntity = -1;
		nodes[i].objTeam = 0;
		nodes[i].objType = -1;
		nodes[i].type = flags;*/
#ifndef _WIN32
		Load_AddNode( vec, flags, objNum, objFlags );	//add the node
#else //_WIN32
		VectorCopy( vec, nodes[i].origin );	//set the node's position

		nodes[i].type = flags;						//set the flags (NODE_OBJECTIVE, for example)
		nodes[i].objectNum[0] = objNum[0];			//set any map objects associated with this node
		nodes[i].objectNum[1] = objNum[1];			//only applies to objects linked to the unreachable flag
		nodes[i].objectNum[2] = objNum[2];
		nodes[i].objFlags = objFlags;				//set the objective flags
	
		if ( nodes[i].type & NODE_AXIS_UNREACHABLE )
		{
			if ( AlliedOnlyFirstNode < 0 )
			{
				AlliedOnlyFirstNode = i;
			}

			nodes[i].objTeam |= TEAM_ALLIES;
			numAlliedOnlyNodes++;
		}

		if ( nodes[i].type & NODE_ALLY_UNREACHABLE )
		{
			if ( AxisOnlyFirstNode < 0 )
			{
				AxisOnlyFirstNode = i;
			}

			nodes[i].objTeam |= TEAM_AXIS;
			numAxisOnlyNodes++;
		}

		number_of_nodes++;
#endif //_WIN32

		//loop through all of the links and read the data
		for ( j = 0; j < numLinks; j++ )
		{
			if (version == NOD_VERSION)
			{
				trap_FS_Read( &target, sizeof(/*short*/ int), f );
			}
			else
			{
				trap_FS_Read( &temp, sizeof(short int), f );
				target = temp;
			}

			trap_FS_Read( &fl2, sizeof(short int), f );
			ConnectNodes( i, target, fl2 );				//add any links
		}

		// Set node objective flags..
		AIMOD_NODES_SetObjectiveFlags( i );
	}

	trap_FS_Read( &fix_aas_nodes, sizeof(short int), f );
	trap_FS_FCloseFile( f );							//close the file
	G_Printf( "^1*** ^3%s^5: Successfully loaded %i waypoints from waypoint file ^7nodes/%s.bwp^5.\n", GAME_VERSION,
			  number_of_nodes, filename );
	nodes_loaded = qtrue;
	if ( fix_aas_nodes )
	{
		AIMOD_NODES_FixAASNodes();
	}

	//number_of_nodes = numberNodes;
	return;
}

void ShowLinkInfo ( int wp )
{
	int i = 0, unreachable = 0;

	if (wp < 0 || wp > number_of_nodes || !wp)
		return;

	for (i = 0; i < nodes[wp].enodenum; i++)
	{
		if (!nodes[wp].links[i].targetNode)
			continue;

		if (!OrgVisible(nodes[wp].origin, nodes[nodes[wp].links[i].targetNode].origin, -1))
			unreachable++;
	}

	G_Printf("Waypoint %i has %i links (%i seem to be unreachable).\n", wp, nodes[wp].enodenum, unreachable);
	G_Printf("Links are to waypoints: ");

	for (i = 0; i < nodes[wp].enodenum; i++)
	{
		if (i+1 == nodes[wp].enodenum)
			G_Printf(va("%i.\n", nodes[wp].links[i].targetNode));
		else
			G_Printf(va("%i, ", nodes[wp].links[i].targetNode));
	}
}

void G_ShowSurface ( int surfaceFlags, int contents, qboolean start_solid, qboolean all_solid  )
{
	if (start_solid)
		G_Printf("Started in solid!\n");

	if (all_solid)
		G_Printf("All in solid!\n");

	//
	// Surface
	//

	G_Printf("Current surface flags (%i):\n", surfaceFlags);

	if (surfaceFlags & SURF_NODAMAGE)
		G_Printf("SURF_NODAMAGE ");

	if (surfaceFlags & SURF_SLICK)
		G_Printf("SURF_SLICK ");

	if (surfaceFlags & SURF_SKY)
		G_Printf("SURF_SKY ");

	if (surfaceFlags & SURF_LADDER)
		G_Printf("SURF_LADDER ");

	if (surfaceFlags & SURF_NOIMPACT)
		G_Printf("SURF_NOIMPACT ");

	if (surfaceFlags & SURF_NOMARKS)
		G_Printf("SURF_NOMARKS ");

	if (surfaceFlags & SURF_SPLASH)
		G_Printf("SURF_SPLASH ");

	if (surfaceFlags & SURF_NODRAW)
		G_Printf("SURF_NODRAW ");

	if (surfaceFlags & SURF_HINT)
		G_Printf("SURF_HINT ");

	if (surfaceFlags & SURF_SKIP)
		G_Printf("SURF_SKIP ");

	if (surfaceFlags & SURF_NOLIGHTMAP)
		G_Printf("SURF_NOLIGHTMAP ");

	if (surfaceFlags & SURF_METAL)
		G_Printf("SURF_METAL ");

	if (surfaceFlags & SURF_NOSTEPS)
		G_Printf("SURF_NOSTEPS ");

	if (surfaceFlags & SURF_NONSOLID)
		G_Printf("SURF_NONSOLID ");

	if (surfaceFlags & SURF_LIGHTFILTER)
		G_Printf("SURF_LIGHTFILTER ");

	if (surfaceFlags & SURF_ALPHASHADOW)
		G_Printf("SURF_ALPHASHADOW ");

	if (surfaceFlags & SURF_NODLIGHT)
		G_Printf("SURF_NODLIGHT ");

	if (surfaceFlags & SURF_WOOD)
		G_Printf("SURF_WOOD ");

	if (surfaceFlags & SURF_GRASS)
		G_Printf("SURF_GRASS ");

	if (surfaceFlags & SURF_GRAVEL)
		G_Printf("SURF_GRAVEL ");

	if (surfaceFlags & SURF_GLASS)
		G_Printf("SURF_GLASS ");

	if (surfaceFlags & SURF_SNOW)
		G_Printf("SURF_SNOW ");

	if (surfaceFlags & SURF_ROOF)
		G_Printf("SURF_ROOF ");

	if (surfaceFlags & SURF_RUBBLE)
		G_Printf("SURF_RUBBLE ");

	if (surfaceFlags & SURF_CARPET)
		G_Printf("SURF_CARPET ");

	if (surfaceFlags & SURF_MONSTERSLICK)
		G_Printf("SURF_MONSTERSLICK ");

	if (surfaceFlags & SURF_MONSLICK_W)
		G_Printf("SURF_MONSLICK_W ");

	if (surfaceFlags & SURF_MONSLICK_N)
		G_Printf("SURF_MONSLICK_N ");

	if (surfaceFlags & SURF_MONSLICK_E)
		G_Printf("SURF_MONSLICK_E ");

	if (surfaceFlags & SURF_MONSLICK_S)
		G_Printf("SURF_MONSLICK_S ");

	if (surfaceFlags & SURF_LANDMINE)
		G_Printf("SURF_LANDMINE ");

	G_Printf("\n");

	//
	// Contents...
	//

	G_Printf("Current contents flags (%i):\n", contents);

	if (contents & CONTENTS_SOLID)
		G_Printf("CONTENTS_SOLID ");

	if (contents & CONTENTS_LIGHTGRID)
		G_Printf("CONTENTS_LIGHTGRID ");

	if (contents & CONTENTS_LAVA)
		G_Printf("CONTENTS_LAVA ");

	if (contents & CONTENTS_SLIME)
		G_Printf("CONTENTS_SLIME ");

	if (contents & CONTENTS_WATER)
		G_Printf("CONTENTS_WATER ");

	if (contents & CONTENTS_FOG)
		G_Printf("CONTENTS_FOG ");

	if (contents & CONTENTS_MISSILECLIP)
		G_Printf("CONTENTS_MISSILECLIP ");

	if (contents & CONTENTS_ITEM)
		G_Printf("CONTENTS_ITEM ");

	if (contents & CONTENTS_MOVER)
		G_Printf("CONTENTS_MOVER ");

	if (contents & CONTENTS_AREAPORTAL)
		G_Printf("CONTENTS_AREAPORTAL ");

	if (contents & CONTENTS_PLAYERCLIP)
		G_Printf("CONTENTS_PLAYERCLIP ");

	if (contents & CONTENTS_MONSTERCLIP)
		G_Printf("CONTENTS_MONSTERCLIP ");

	if (contents & CONTENTS_TELEPORTER)
		G_Printf("CONTENTS_TELEPORTER ");

	if (contents & CONTENTS_JUMPPAD)
		G_Printf("CONTENTS_JUMPPAD ");

	if (contents & CONTENTS_CLUSTERPORTAL)
		G_Printf("CONTENTS_CLUSTERPORTAL ");

	if (contents & CONTENTS_DONOTENTER)
		G_Printf("CONTENTS_DONOTENTER ");

	if (contents & CONTENTS_DONOTENTER_LARGE)
		G_Printf("CONTENTS_DONOTENTER_LARGE ");

	if (contents & CONTENTS_ORIGIN)
		G_Printf("CONTENTS_ORIGIN ");

	if (contents & CONTENTS_BODY)
		G_Printf("CONTENTS_BODY ");

	if (contents & CONTENTS_CORPSE)
		G_Printf("CONTENTS_CORPSE ");

	if (contents & CONTENTS_DETAIL)
		G_Printf("CONTENTS_DETAIL ");

	if (contents & CONTENTS_STRUCTURAL)
		G_Printf("CONTENTS_STRUCTURAL ");

	if (contents & CONTENTS_TRANSLUCENT)
		G_Printf("CONTENTS_TRANSLUCENT ");

	if (contents & CONTENTS_TRIGGER)
		G_Printf("CONTENTS_TRIGGER ");

	if (contents & CONTENTS_NODROP)
		G_Printf("CONTENTS_NODROP ");

	G_Printf("\n");
}

//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.
//3 = team door entity in the way.

int tr_s_flags = 0;
int tr_c_flags = 0;
qboolean tr_all_solid = qfalse;
qboolean tr_start_solid = qfalse;

int
VisInfo_NodeVisible ( vec3_t org1, vec3_t org2, int ignore )
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

	trap_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID /*| CONTENTS_LAVA*//*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	
	if ( tr.fraction == 1 /*&& !(tr.contents & CONTENTS_LAVA)*/ )
	{
		return ( 1 );
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
			return ( 2 );
		default:
			break;
		}
	}

	if (tr.allsolid)
		tr_all_solid = qtrue;
	else
		tr_all_solid = qfalse;

	if (tr.startsolid)
		tr_start_solid = qtrue;
	else
		tr_start_solid = qfalse;

	tr_s_flags = tr.surfaceFlags;
	tr_c_flags = tr.contents;
	
	return ( 0 );
}

qboolean HasPortalFlags ( int surfaceFlags, int contents )
{
	if ( ( (surfaceFlags & SURF_NOMARKS) && (surfaceFlags & SURF_NOIMPACT) && (surfaceFlags & SURF_NODRAW) && (contents & CONTENTS_PLAYERCLIP) && (contents & CONTENTS_TRANSLUCENT) ) )
		return qtrue;

	return qfalse;
}

extern float VectorDistance(vec3_t v1, vec3_t v2);

void ShowWaypointVisibilityInfo ( int wp1, int wp2 )
{
	int visible = VisInfo_NodeVisible( nodes[wp1].origin, nodes[wp2].origin, -1 );

	G_Printf("Distance between waypoint %i and waypoint %i is %f.\n", wp1, wp2, VectorDistance(nodes[wp1].origin, nodes[wp2].origin));

	if (visible == 1)
	{
		G_Printf("Waypoint %i is visible from waypoint %i.\n", wp1, wp2);
		return;
	}
	else if (visible == 2)
	{
		G_Printf("Waypoint %i is probebly visible from waypoint %i. Hit an entity.\n", wp1, wp2);
		return;
	}
	else
	{
		G_Printf("Waypoint %i is not visible from waypoint %i.\n", wp1, wp2);
		G_ShowSurface( tr_s_flags, tr_c_flags, tr_start_solid, tr_all_solid );

		if (HasPortalFlags(tr_s_flags, tr_c_flags))
			G_Printf("Those look to be portal flags. Probebly reachable...\n");
	}
}

/* */
void
RemoveDoorsAndDestroyablesForSave ( void )
{
	int i;
	for ( i = MAX_CLIENTS; i < MAX_GENTITIES; i++ )
	{
		gentity_t	*ent = &g_entities[i];
		if ( ent->s.eType == ET_INVISIBLE || ent->entstate == STATE_INVISIBLE || (ent->s.powerups & STATE_INVISIBLE) )
		{
			G_FreeEntity( &g_entities[i] );
			continue;
		}

		if ( !ent || !ent->classname || !ent->classname[0] )
		{
			continue;
		}

		if ( !strcmp( ent->classname, "func_door") || !strcmp( ent->classname, "func_door_rotating") )
		{
			G_FreeEntity( &g_entities[i] );
		}
		else if ( !strcmp( ent->classname, "func_explosive") )
		{
			G_FreeEntity( &g_entities[i] );
		}
	}
}


/* */
void
AIMOD_NODES_RemoveTeamNodes ( void )
{
	int i;
	for ( i = 0; i < number_of_nodes; i++ )
	{
		if ( nodes[i].type & NODE_AXIS_UNREACHABLE || nodes[i].type & NODE_ALLY_UNREACHABLE )
		{
			nodes[i].type = NODE_MOVE;
		}
	}
}

/*///////////////////////////////////////////////////
SaveNodes
saves all the current nodes and links to a file
default filename is <mapname>.nod
This function depends on the file format for .nod files
and the node_t structure format
*/

extern void AIMOD_Generate_Cover_Spots_Real ( void );

///////////////////////////////////////////////////
void
AIMOD_NODES_SaveNodes ( void )
{
	fileHandle_t	f;
	int				i;
	/*short*/ int		j;
	float			version = NOD_VERSION;	//version is 1.0 for now
	char			name[] = BOT_MOD_NAME;
	vmCvar_t		mapname;
	char			map[64] = "";
	char			filename[60];
	/*short*/ int		num_nodes = number_of_nodes;
	strcpy( filename, "nodes/" );

	////////////////////
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

	///////////////////
	//try to open the output file, return if it failed
	if (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
		trap_FS_FOpenFile( va( "nodes/%s_supremacy.bwp", filename), &f, FS_WRITE );
	else
		trap_FS_FOpenFile( va( "nodes/%s.bwp", filename), &f, FS_WRITE );

	if ( !f )
	{
		G_Printf( "^1*** ^3ERROR^5: Error opening node file ^7nodes/%s.bwp^5!!!\n", filename );
		return;
	}

	for ( i = 0; i < num_nodes; i++ )
	{
		for ( j = 0; j < MAX_NODELINKS; j++ )
		{
			nodes[i].links[j].targetNode = INVALID;
			nodes[i].links[j].cost = 999999;
			nodes[i].links[j].flags = 0;
			nodes[i].objectNum[0] = nodes[i].objectNum[1] = nodes[i].objectNum[2] = ENTITYNUM_NONE;
		}
	}

	RemoveDoorsAndDestroyablesForSave();
	num_nodes = number_of_nodes;

	// Resolve paths
	//-------------------------------------------------------
	AIMOD_MAPPING_MakeLinks();
	num_nodes = number_of_nodes;
	for ( i = 0; i < num_nodes; i++ )
	{
		// Set node objective flags..
		AIMOD_NODES_SetObjectiveFlags( i );
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );	//get the map name
	strcpy( map, mapname.string );
	trap_FS_Write( &name, strlen( name) + 1, f );								//write the mod name to the file
	trap_FS_Write( &version, sizeof(float), f );								//write the version of this file
	trap_FS_Write( &map, strlen( map) + 1, f );									//write the map name
	trap_FS_Write( &num_nodes, sizeof(/*short*/ int), f );							//write the number of nodes in the map
	if ( nodes[num_nodes].type & NODE_AXIS_UNREACHABLE )
	{
		nodes[num_nodes].objTeam |= TEAM_ALLIES;
	}
	else if ( nodes[num_nodes].type & NODE_ALLY_UNREACHABLE )
	{
		nodes[num_nodes].objTeam |= TEAM_AXIS;
	}

	for ( i = 0; i < num_nodes; i++ )											//loop through all the nodes
	{

		//write all the node data to the file
		//trap_FS_Write(&(nodes[i].targetNode), sizeof (short int), f);
		trap_FS_Write( &(nodes[i].origin), sizeof(vec3_t), f );
		trap_FS_Write( &(nodes[i].type), sizeof(int), f );
		trap_FS_Write( &(nodes[i].objectNum), sizeof(short int) * 3, f );
		trap_FS_Write( &(nodes[i].objFlags), sizeof(short int), f );
		trap_FS_Write( &(nodes[i].enodenum), sizeof(short int), f );
		for ( j = 0; j < nodes[i].enodenum; j++ )
		{
			trap_FS_Write( &(nodes[i].links[j].targetNode), sizeof(/*short*/ int), f );
			trap_FS_Write( &(nodes[i].links[j].flags), sizeof(short int), f );
		}
	}

	trap_FS_FCloseFile( f );													//close the file
	G_Printf( "^1*** ^3%s^5: Successfully saved node file ^7nodes/%s.bwp^5.\n", GAME_VERSION, filename );

	// And make cover spots again!
	AIMOD_Generate_Cover_Spots_Real();
}
#endif //__BOT_OLD_NOD_FORMAT__
vmCvar_t	bot_auto_waypoint;
extern void AIMOD_MAPPING_AddNode ( int clientNum, int teamNum );


/* */
void
WaypointThink ( gentity_t *ent )
{
	float		distance, min_distance;
	int			i;
	qboolean	force_node = qfalse;
	if ( ent->r.svFlags & SVF_BOT )
	{
		return;
	}

	if ( bot_auto_waypoint.integer )					// is auto waypoint on?
	{
		vec3_t	up;

		// find the distance from the last used waypoint
		distance = VectorDistance( ent->last_added_waypoint, ent->r.currentOrigin );
		if ( distance > 96 )
		{
			min_distance = 9999.0;

			// check that no other reachable waypoints are nearby...
			for ( i = 0; i < number_of_nodes; i++ )
			{
				distance = VectorDistance( nodes[i].origin, ent->r.currentOrigin );
				if ( distance < 256 )
				{
					if ( OrgVisible( ent->r.currentOrigin, nodes[i].origin, ent->client->ps.clientNum) )
					{
						if ( distance < min_distance )
						{
							min_distance = distance;
						}
					}
				}
			}

			// make sure nearest waypoint is far enough away...
			if ( min_distance >= 96 )
			{
				force_node = qtrue;
			}
		}

		VectorCopy( ent->r.currentOrigin, up );
		up[2] += 16;
		if ( force_node )
		{
			VectorCopy( ent->r.currentOrigin, ent->last_good_node_point );
			VectorCopy( ent->last_good_node_point, ent->last_added_waypoint );
			AIMOD_MAPPING_AddNode( ent->s.number, -1 ); // place a waypoint here
			return;
		}

		if ( OrgVisible( up, ent->last_added_waypoint, ent->s.number) )
		{												// this spot is better then our last selection..
			VectorCopy( ent->r.currentOrigin, ent->last_good_node_point );
		}

		if ( !OrgVisible( up, ent->last_added_waypoint, ent->s.number) )
		{												// We need to move the client back for a moment to make the point... So...
			vec3_t	original;
			VectorCopy( ent->r.currentOrigin, original );
			G_SetOrigin( ent, ent->last_good_node_point );
			VectorCopy( ent->last_good_node_point, ent->r.currentOrigin );
			VectorCopy( ent->last_good_node_point, ent->client->ps.origin );
			AIMOD_MAPPING_AddNode( ent->s.number, -1 ); // place a waypoint here
			G_SetOrigin( ent, original );
			VectorCopy( original, ent->r.currentOrigin );
			VectorCopy( original, ent->client->ps.origin );
			VectorCopy( ent->last_good_node_point, ent->last_added_waypoint );
			VectorCopy( original, ent->last_good_node_point );
		}
	}

	/*	float distance, min_distance;
	int i;
	qboolean force_node = qfalse;

	if (bot_auto_waypoint.integer)  // is auto waypoint on?
	{
		vec3_t up;

		// find the distance from the last used waypoint
		distance = VectorDistance(ent->last_added_waypoint, ent->r.currentOrigin);

		if (distance > 96)
		{
			min_distance = 9999.0;

	        // check that no other reachable waypoints are nearby...
			for (i=0; i < number_of_nodes; i++)
			{
				distance = VectorDistance(nodes[i].origin, ent->r.currentOrigin);

				if (distance < 256)
				{
					if (OrgVisible(ent->r.currentOrigin, nodes[i].origin, ent->client->ps.clientNum))
					{
						if (distance < min_distance)
							min_distance = distance;
					}
				}
			}

			// make sure nearest waypoint is far enough away...
			if (min_distance >= 96)
				 force_node = qtrue;
		}

		VectorCopy(ent->r.currentOrigin, up);
		up[2]+=16;

		if (OrgVisibleBox(up, ent->r.mins, ent->r.maxs, ent->last_added_waypoint, ent->s.number) || force_node )
		{// this spot is better then our last selection..
			VectorCopy(ent->r.currentOrigin, ent->last_good_node_point);
		}

		if (!ent->routing_pathsize || ent->routing_pathsize < 0 || ent->routing_pathsize > (MAX_PATHLIST_NODES/4)*1)
			ent->routing_pathsize = 0;

		if (!OrgVisibleBox(up, ent->r.mins, ent->r.maxs, ent->last_added_waypoint, ent->s.number) || force_node)
		{// Time to add our last_good_node_point as a node... // will delay this and make it work in chains ( > 1 players )
			VectorCopy(ent->last_added_waypoint, ent->routing_pathlist[ent->routing_pathsize]);
			ent->routing_pathsize++;

			if (ent->routing_pathsize > MAX_PATHLIST_NODES/4)
			{// Store our route...
				vec3_t stored_origin;
				int i = 0;

				VectorCopy(ent->r.currentOrigin, stored_origin);

				for (i = 0; i < ent->routing_pathsize; i++)
				{
					// We need to move the client back for a moment to make the point... So...
					G_SetOrigin(ent, ent->routing_pathlist[i]);
					VectorCopy(ent->routing_pathlist[i], ent->r.currentOrigin);
					VectorCopy(ent->routing_pathlist[i], ent->client->ps.origin);
					AIMOD_MAPPING_AddNode( ent->s.number );  // place a waypoint here
				}
				
				// And return straight away to our real position... As if we never moved hehehe...
				G_SetOrigin(ent, stored_origin);
				VectorCopy(stored_origin, ent->r.currentOrigin);
				VectorCopy(stored_origin, ent->client->ps.origin);
			}
		}
	}
*/
}
#endif
