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
#include "../cgame/cg_local.h"
#include "../game/q_global_defines.h"

extern void		CG_Printff ( const char *fmt );
extern void		trap_UpdateScreen( void );
extern vmCvar_t cg_waypoint_render;

#ifdef __BOT__

extern void sse_memset(void *dst, int n32, unsigned long i);

#ifdef _WIN32
// warning C4996: 'strcpy' was declared deprecated
#pragma warning( disable : 4996 )

// warning #1478: function "strcpy" (declared at line XX of "XXX") was declared "deprecated"
//#pragma warning( disable : 1478 )
#endif //_WIN32

extern void *B_Alloc ( int size );
extern void B_Free ( void *ptr );

#define	G_MAX_SCRIPT_ACCUM_BUFFERS 10
#define MAX_NODELINKS       32              // Maximum Node Links (12)
#define MAX_NODES           65536//19500//10000//8000//32000//16000	// Maximum Nodes (8000)
#define INVALID				-1

extern qboolean CPU_CHECKED;
extern qboolean SSE_CPU;

vec3_t			botTraceMins = { -20, -20, -1 };
vec3_t			botTraceMaxs = { 20, 20, 32 };

void AIMOD_NODES_LoadNodes2 ( void );
float RoofHeightAt ( vec3_t org );
void AIMod_AutoWaypoint_Init_Memory ( void ); // below...
void AIMod_AutoWaypoint_Free_Memory ( void ); // below...
void AIMod_AutoWaypoint_Optimizer ( void ); // below...
void AIMod_AutoWaypoint_Cleaner ( qboolean quiet, qboolean null_links_only, qboolean relink_only, qboolean multipass, qboolean initial_pass, qboolean extra ); // below...

//  _  _         _       _____
// | \| |___  __| |___  |_   _|  _ _ __  ___ ___
// | .` / _ \/ _` / -_)   | || || | '_ \/ -_|_-<
// |_|\_\___/\__,_\___|   |_| \_, | .__/\___/__/
//                            |__/|_|
//------------------------------------------------------------------------------------

//===========================================================================
// Description  : Node flags + Link Flags...
#define NODE_INVALID				-1
#define NODE_MOVE					0       // Move Node
#define NODE_OBJECTIVE				1
#define NODE_TARGET					2
//#define NODE_TARGETSELECT			4
#define NODE_LAND_VEHICLE			4
#define NODE_FASTHOP				8
#define NODE_COVER					16
#define NODE_WATER					32
#define NODE_LADDER					64      // Ladder Node
#define	NODE_MG42					128		//node is at an mg42
#define	NODE_DYNAMITE				256
#define	NODE_BUILD					512
#define	NODE_JUMP					1024
#define	NODE_DUCK					2048
#define	NODE_ICE					4096	// Node is located on ice (slick)...
#define NODE_ALLY_UNREACHABLE		8192
#define NODE_AXIS_UNREACHABLE		16384
#define	NODE_AXIS_DELIVER			32768	//place axis should deliver stolen documents/objective
#define	NODE_ALLY_DELIVER			65536	//place allies should deliver stolen documents/objective

//===========================================================================
// Description  : NPC Node flags + Link Flags...
#define NPC_NODE_INVALID				-1
#define NPC_NODE_MOVE					0       // Move Node
#define NPC_NODE_OBJECTIVE				1		// Objective position... Do something... (Unused in 0.1 for npcs)
#define NPC_NODE_ROUTE_BEGIN			2		// The beginning of an NPC route...
#define NPC_NODE_ROUTE_END				4		// The end of an NPC route...
#define NPC_NODE_JUMP					8		// Need a jump here...
#define NPC_NODE_DUCK					16		// Need to duck here...
#define NPC_NODE_WATER					32		// Node is in/on water...
#define NPC_NODE_ICE					64      // Node is on ice...
#define	NPC_NODE_LADDER					128		// Ladder Node
#define	NPC_NODE_MG42					256		// Node is at an mg42
#define	NPC_NODE_BUILD					512		// Need to build something here... (Unused in 0.1 for npcs)
#define	NPC_NODE_ALLY_UNREACHABLE		1024	// Axis only...
#define	NPC_NODE_AXIS_UNREACHABLE		2048	// Allied only...
#define	NPC_NODE_COVER					4096	// Cover point...

// Node finding flags...
#define NODEFIND_BACKWARD			1      // For selecting nodes behind us.
#define NODEFIND_FORCED				2      // For manually selecting nodes (without using links)
#define NODEFIND_ALL				4      // For selecting all nodes

// Objective types...
#define	OBJECTIVE_DYNAMITE			0		//blow this objective up!
#define	OBJECTIVE_DYNOMITE			0		// if next objective is OBJ_DYNOMITE => engi's are important
#define	OBJECTIVE_STEAL				1		//steal the documents!
#define	OBJECTIVE_CAPTURE			2		//get the flag - not intented for checkpoint, but for spawn flags
#define	OBJECTIVE_BUILD				3		//get the flag - not intented for checkpoint, but for spawn flags
#define	OBJECTIVE_CARRY				4
#define	OBJECTIVE_FLAG				5
#define	OBJECTIVE_POPFLAG			6
#define	OBJECTIVE_AXISONLY			128
#define	OBJECTIVE_ALLYSONLY			256

// es_fix : added comments to flags
// This way the bot knows how to approach the next node
#define		PATH_NORMAL				0		// Bot moves normally to next node : ie equivalant to pressing the forward move key on your keyboard
#define		PATH_CROUCH				1		//bot should duck and walk toward next node
#define		PATH_SPRINT				2		//bot should sprint to next node
#define		PATH_JUMP				4		//bot should jump to next node
#define		PATH_WALK				8		//bot should walk to next node
#define		PATH_BLOCKED			16		//path to next node is blocked
#define		PATH_LADDER				32		//ladders!
#define		PATH_NOTANKS			64		//No Land Vehicles...
#define		PATH_DANGER				128		//path to next node is dangerous - Lava/Slime/Water

//------------------------------------------------------------------------------------

//  _  _         _       ___     _         _ _    _
// | \| |___  __| |___  | _ \___| |__ _  _(_) |__| |
// | .` / _ \/ _` / -_) |   / -_) '_ \ || | | / _` |
// |_|\_\___/\__,_\___| |_|_\___|_.__/\_,_|_|_\__,_|
//------------------------------------------------------------------------------------
qboolean    dorebuild;          // for rebuilding nodes
qboolean	shownodes;
qboolean	nodes_loaded;
//------------------------------------------------------------------------------------

//  _  _         _       _    _      _     ___ _               _
// | \| |___  __| |___  | |  (_)_ _ | |__ / __| |_ _ _ _  _ __| |_ _  _ _ _ ___
// | .` / _ \/ _` / -_) | |__| | ' \| / / \__ \  _| '_| || / _|  _| || | '_/ -_)
// |_|\_\___/\__,_\___| |____|_|_||_|_\_\ |___/\__|_|  \_,_\__|\__|\_,_|_| \___|
//------------------------------------------------------------------------------------
typedef struct nodelink_s       // Node Link Structure
{
    /*short*/ int       targetNode; // Target Node
    float           cost;       // Cost for PathSearch algorithm
	int				flags;
}nodelink_t;                    // Node Link Typedef
//------------------------------------------------------------------------------------
//  _  _         _       ___ _               _
// | \| |___  __| |___  / __| |_ _ _ _  _ __| |_ _  _ _ _ ___
// | .` / _ \/ _` / -_) \__ \  _| '_| || / _|  _| || | '_/ -_)
// |_|\_\___/\__,_\___| |___/\__|_|  \_,_\__|\__|\_,_|_| \___|
//------------------------------------------------------------------------------------
typedef struct node_s           // Node Structure
{
    vec3_t      origin;         // Node Origin
    int         type;           // Node Type
    short int   enodenum;		// Mostly just number of players around this node
	nodelink_t  links[MAX_NODELINKS];	// Store all links
	short int	objectNum[3];		//id numbers of any world objects associated with this node (used only with unreachable flags)
	int			objFlags;			//objective flags if this node is an objective node
	short int	objTeam;			//the team that should complete this objective
	short int	objType;			//what type of objective this is - see OBJECTIVE_XXX flags
	short int	objEntity;			//the entity number of what object to dynamite at a dynamite objective node
} node_t;                       // Node Typedef

typedef struct nodelink_convert_s       // Node Link Structure
{
    /*short*/ int       targetNode; // Target Node
    float           cost;       // Cost for PathSearch algorithm
}nodelink_convert_t;            // Node Link Typedef

typedef struct node_convert_s           // Node Structure
{
    vec3_t				origin;         // Node Origin
    int					type;           // Node Type
    short int			enodenum;		// Mostly just number of players around this node
	nodelink_convert_t  links[MAX_NODELINKS];	// Store all links
} node_convert_t;                       // Node Typedef
//------------------------------------------------------------------------------------

typedef struct enode_s
{
	int			link_node;
	int			num_routes;				// Number of alternate routes available, maximum 5
	int			routes[5];				// Possible alternate routes to reach the node
	team_t		team;
} enode_t;

int		number_of_nodes = 0;
int		optimized_number_of_nodes = 0;
int		aw_num_nodes = 0;
int		optimized_aw_num_nodes = 0;
node_t	*nodes;
node_t	*optimized_nodes;

#define BOT_MOD_NAME	"aimod"
//#define NOD_VERSION		1.1f
float NOD_VERSION = 1.1f;

extern vmCvar_t fs_game;

float aw_percent_complete = 0.0f;
char task_string1[255];
char task_string2[255];
char task_string3[255];
char last_node_added_string[255];

extern void AIMOD_SaveMapCoordFile ( void );
extern qboolean AIMOD_LoadMapCoordFile ( void );
#ifdef __OLD__
extern void AIMod_GetMapBounts ( vec3_t mapMins, vec3_t mapMaxs );
#else //!__OLD__
extern void AIMod_GetMapBounts ( void );
#endif //__OLD__
extern qboolean CG_SpawnVector2D( const char *key, const char *defaultString, float *out );


float waypoint_distance_multiplier = 1.0f;

//#define waypoint_scatter_distance			24
int waypoint_scatter_distance = 24;
//#define waypoint_scatter_distance			31
//#define waypoint_scatter_distance			47
int outdoor_waypoint_scatter_distance = 63;

time_t	aw_stage_start_time = 0;
float	aw_last_percent = 0;
time_t	aw_last_percent_time = 0;
time_t	aw_last_times[100];
int		aw_num_last_times = 0;

void AIMod_AutoWaypoint_DrawProgress ( void )
{
	int				flags = 64|128;
	float			frac;
	rectDef_t		rect;
	time_t			total_seconds_left = 0;
	int				seconds_left = 0;
	int				minutes_left = 0;
	qboolean		estimating = qfalse;

	if (aw_percent_complete == 0.0f)
	{// Init timer...
		aw_stage_start_time = time(NULL);//trap_Milliseconds();
		aw_last_percent_time = aw_stage_start_time;
		aw_last_percent = 0;
		aw_num_last_times = 0;
	}

	if (aw_percent_complete > 100.0f)
	{// UQ: It can happen... Somehow... LOL!
		aw_percent_complete = 100.0f;
		aw_last_percent = 0;
		aw_num_last_times = 0;
	}

	if (aw_percent_complete >= 1)
	{
		time_t time_so_far = time(NULL)/*trap_Milliseconds()*/ - aw_stage_start_time;
		float percent_left = (100 - aw_percent_complete);
		float percent_completed = (aw_percent_complete * 0.01);

		//total_seconds_left = ((percent_left / aw_percent_complete) * time_so_far);
		//total_seconds_left = ((aw_percent_complete/100) * time_so_far) * percent_left;//(time_so_far / aw_percent_complete) * percent_left;
		//total_seconds_left = (float)((((time_so_far * (((double)100 / (double)aw_percent_complete) - 1)) * 4) * 0.0001) * (float)((aw_percent_complete * 2) * 0.01));
		total_seconds_left = (((percent_left / aw_percent_complete) * time_so_far) * 0.0000001 ) * 0.6;
		minutes_left = total_seconds_left/60;
		seconds_left = total_seconds_left-(minutes_left*60);
	}
	else
	{
		estimating = qtrue;
	}

	// Draw the bar!
	frac = (float)((aw_percent_complete)*0.01);

	rect.w = 500;
	rect.h = 30;
	
	rect.x = 69;
	rect.y = 369;

	// draw the background, then filled bar, then border
	CG_FillRect( rect.x, rect.y, rect.w, rect.h, popBG );
	CG_FilledBar( rect.x, rect.y, rect.w, rect.h, popRed, NULL, NULL, frac, flags );
	CG_DrawRect_FixedBorder( rect.x, rect.y, rect.w, rect.h, POP_HUD_BORDERSIZE, popBorder );

	CG_Text_Paint_Ext((rect.x), (rect.y + (rect.h*0.5) - 100), 0.22, 0.22, colorWhite, va("^3AUTO-WAYPOINTING^5 - Please wait..."), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
	CG_Text_Paint_Ext((rect.x), (rect.y + (rect.h*0.5) - 60), 0.22, 0.22, colorWhite, task_string1, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
	CG_Text_Paint_Ext((rect.x), (rect.y + (rect.h*0.5) - 40), 0.22, 0.22, colorWhite, task_string2, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
	CG_Text_Paint_Ext((rect.x), (rect.y + (rect.h*0.5) - 20), 0.22, 0.22, colorWhite, task_string3, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
	CG_Text_Paint_Ext((rect.x + (rect.w*0.5) - 35), (rect.y + (rect.h*0.5) + 8), 0.40, 0.40, colorWhite, va("^7%.2f%%", aw_percent_complete), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
	
	if (!estimating && aw_percent_complete > 1.0f)
	{
		if (seconds_left >= 10)
			CG_Text_Paint_Ext((rect.x + 160), (rect.y + (rect.h*0.5) + 30), 0.16, 0.16, colorWhite, va("^3%i ^5minutes ^3%i ^5seconds remaining (estimated)", minutes_left, seconds_left), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
		else
			CG_Text_Paint_Ext((rect.x + 160), (rect.y + (rect.h*0.5) + 30), 0.16, 0.16, colorWhite, va("^3%i ^5minutes ^30%i ^5seconds remaining (estimated)", minutes_left, seconds_left), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
	}
	else
		CG_Text_Paint_Ext((rect.x + 160), (rect.y + (rect.h*0.5) + 30), 0.16, 0.16, colorWhite, va("^5    ... estimating time remaining ..."), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );

	CG_Text_Paint_Ext((rect.x + 80), (rect.y + (rect.h*0.5) + 50), 0.22, 0.22, colorWhite, last_node_added_string, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );

	trap_R_SetColor( NULL );
}

qboolean AW_Map_Has_Waypoints ( void )
{
	fileHandle_t	f;
	char			filename[60];
	int				len = 0;
	vmCvar_t		mapname;

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
	if (cgs.gametype == GT_SUPREMACY || cgs.gametype == GT_SUPREMACY_CAMPAIGN)
	{
		len = trap_FS_FOpenFile( va( "nodes/%s_supremacy.bwp", filename), &f, FS_READ );
		trap_FS_FCloseFile(f);
		
		if( len <= 0 )
		{
			len = trap_FS_FOpenFile( va( "nodes/%s.bwp", filename), &f, FS_READ );
			trap_FS_FCloseFile(f);
			
			if( len <= 0 )
			{
				FILE *file;

				file = fopen( va( "%s/nodes/%s_supremacy.bwp", fs_game.string, filename), "r" );

				if ( !file )
					file = fopen( va( "ef/nodes/%s_supremacy.bwp", filename), "r" );

				if ( !file )
					file = fopen( va( "efbotest/nodes/%s_supremacy.bwp", filename), "r" );

				if ( !file )
				{
					file = fopen( va( "%s/nodes/%s.bwp", fs_game.string, filename), "r" );

					/*if ( !file )
						file = fopen( va( "ef/nodes/%s.bwp", filename), "r" );

					if ( !file )
						file = fopen( va( "efbotest/nodes/%s.bwp", filename), "r" );*/

					if ( !file )
					{
						return qfalse;
					}
				}

				fclose(file);
			}
		}
	}
	else
	{
		len = trap_FS_FOpenFile( va( "nodes/%s.bwp", filename), &f, FS_READ );
		trap_FS_FCloseFile(f);
		
		if( len <= 0 )
		{
			FILE *file;
			file = fopen( va( "%s/nodes/%s.bwp", fs_game.string, filename), "r" );

			if ( !file )
			{
				return qfalse;
			}

			fclose(file);
		}
	}

	return qtrue;
}

#ifdef __CUDA__
/* */
__device__ __host__ float VectorDistanceNoHeight ( vec3_t v1, vec3_t v2 )
{
	__shared__ vec3_t	dir;
	__shared__ vec3_t	v1a, v2a;
	VectorCopy( v1, v1a );
	VectorCopy( v2, v2a );
	v2a[2] = v1a[2];
	VectorSubtract( v2a, v1a, dir );
	return ( VectorLength( dir) );
}


/* */
__device__ __host__ float HeightDistance ( vec3_t v1, vec3_t v2 )
{
	__shared__ vec3_t	dir;
	__shared__ vec3_t	v1a, v2a;
	VectorCopy( v1, v1a );
	VectorCopy( v2, v2a );
	v2a[0] = v1a[0];
	v2a[1] = v1a[1];
	VectorSubtract( v2a, v1a, dir );
	return ( VectorLength( dir) );
}

/* */
__device__ __host__ qboolean BAD_WP_Height ( vec3_t start, vec3_t end )
{
	__shared__ float height_diff = HeightDistance(start, end);

	if (start[2] < end[2] && height_diff >= 48)
		return ( qtrue );

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
#else //!__CUDA__
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
	float height_diff = HeightDistance(start, end);

	if (start[2] < end[2] && height_diff >= 48)
		return ( qtrue );

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
#endif //__CUDA__

//special node visibility check for bot linkages..
//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.

//3 = team door entity in the way.
int
AW_NodeVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg, newOrg2;
//	vec3_t	mins, maxs;

	//VectorCopy( botTraceMins, mins );
	//VectorCopy( botTraceMaxs, maxs );
	VectorCopy( org2, newOrg );

	//newOrg[2] += 16; // Look from up a little...

	VectorCopy( org1, newOrg2 );

	//newOrg2[2] += 16; // Look from up a little...

	CG_Trace( &tr, newOrg2, NULL/*mins*/, NULL/*maxs*/, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	
	if ( tr.fraction == 1 && !(tr.contents & CONTENTS_LAVA) )
	{
		return ( 1 );
	}

	return ( 0 );
}

qboolean
CheckSolid ( vec3_t org )
{
	/*trace_t tr;
	vec3_t	newOrg;
	vec3_t	mins, maxs;

	vec3_t			traceMins = { -10, -10, 0 };
	vec3_t			traceMaxs = { 10, 10, 8 };

	VectorCopy( traceMins, mins );
	VectorCopy( traceMaxs, maxs );
	VectorCopy( org, newOrg );

	newOrg[2] += 24; // Look from up a little...

	CG_Trace( &tr, org, mins, maxs, newOrg, -1, MASK_PLAYERSOLID );
	if ( tr.startsolid || tr.allsolid )
	{
		return ( qtrue );
	}*/

	return ( qfalse );
}

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
	//vec3_t			traceMins = { -20, -20, -1 };
	//vec3_t			traceMaxs = { 20, 20, 32 };

	vec3_t			traceMins = { -10, -10, -1 };
	vec3_t			traceMaxs = { 10, 10, 48/*32*/ };
//	vec3_t			traceMins = { -20, -20, -1 };
//	vec3_t			traceMaxs = { 20, 20, 48 };

	//vec3_t	traceMins = { -10, -10, -1 };
	//vec3_t	traceMaxs = { 10, 10, 15 };

	VectorCopy( traceMins, mins );
	VectorCopy( traceMaxs, maxs );
	VectorCopy( org2, newOrg );
	VectorCopy( org1, newOrg2 );

	newOrg[2] += 8; // Look from up a little...
	newOrg2[2] += 8; // Look from up a little...

	//CG_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
	CG_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	

	if (tr.startsolid || tr.allsolid)
		return ( 0 );

	if ( tr.fraction == 1 && !(tr.contents & CONTENTS_LAVA))
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( tr2.fraction == 1 && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if (tr.surfaceFlags & SURF_LADDER)
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( (tr2.fraction == 1 || (tr2.surfaceFlags & SURF_LADDER)) && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if ( tr.fraction != 1 
		&& tr.entityNum != ENTITYNUM_NONE 
		&& tr.entityNum < ENTITYNUM_MAX_NORMAL )
	{
		switch (cg_entities[tr.entityNum].currentState.eType)
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
			{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
				/*trace_t tr2;
				CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA );

				if ( tr2.fraction != 1 
					&& tr2.entityNum != ENTITYNUM_NONE 
					&& tr2.entityNum < ENTITYNUM_MAX_NORMAL )
				{
					if (tr2.entityNum == tr.entityNum)
						return ( 4 );
				}*/
				trace_t tr2;
				CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

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

	/*VectorCopy( org2, newOrg );
	VectorCopy( org1, newOrg2 );

	newOrg[2] += 16; // Look from up a little...
	newOrg2[2] += 16; // Look from up a little...

	//CG_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
	CG_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID );
	if ( tr.fraction == 1 && !(tr.contents & CONTENTS_LAVA) )
	{
		return ( 1 );
	}

	if ( tr.fraction != 1 
		&& tr.entityNum != ENTITYNUM_NONE 
		&& tr.entityNum < ENTITYNUM_MAX_NORMAL )
	{
		switch (cg_entities[tr.entityNum].currentState.eType)
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
		case ET_VEHICLE:
		case ET_PARTICLE_SYSTEM:
			return ( 1 );
		default:
			break;
		}
	}*/

	return ( 0 );
}

int
NodeVisibleJump ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg, newOrg2;
	vec3_t	mins, maxs;

	//vec3_t			botTraceMins = { -20, -20, -1 };
	//vec3_t			botTraceMaxs = { 20, 20, 32 };
	//vec3_t			traceMins = { -20, -20, -1 };
	//vec3_t			traceMaxs = { 20, 20, 32 };

	vec3_t			traceMins = { -10, -10, -1 };
	vec3_t			traceMaxs = { 10, 10, 48/*32*/ };
	//vec3_t			traceMins = { -20, -20, -1 };
	//vec3_t			traceMaxs = { 20, 20, 48 };

	//vec3_t	traceMins = { -10, -10, -1 };
	//vec3_t	traceMaxs = { 10, 10, 15 };

	VectorCopy( traceMins, mins );
	VectorCopy( traceMaxs, maxs );
	VectorCopy( org2, newOrg );
	VectorCopy( org1, newOrg2 );

	newOrg[2] += 16; // Look from up a little...
	newOrg2[2] += 16; // Look from up a little...

	//CG_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
	CG_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	
	if (tr.startsolid || tr.allsolid)
		return ( 0 );

	if ( tr.fraction == 1 && !(tr.contents & CONTENTS_LAVA))
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( tr2.fraction == 1 && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if (tr.surfaceFlags & SURF_LADDER)
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( (tr2.fraction == 1 || (tr2.surfaceFlags & SURF_LADDER)) && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if ( tr.fraction != 1 
		&& tr.entityNum != ENTITYNUM_NONE 
		&& tr.entityNum < ENTITYNUM_MAX_NORMAL )
	{
		switch (cg_entities[tr.entityNum].currentState.eType)
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
			{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
				/*trace_t tr2;
				CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA );

				if ( tr2.fraction != 1 
					&& tr2.entityNum != ENTITYNUM_NONE 
					&& tr2.entityNum < ENTITYNUM_MAX_NORMAL )
				{
					if (tr2.entityNum == tr.entityNum)
						return ( 4 );
				}*/
				trace_t tr2;
				CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

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

	//vec3_t			botTraceMins = { -20, -20, -1 };
	//vec3_t			botTraceMaxs = { 20, 20, 32 };
	//vec3_t			traceMins = { -20, -20, -1 };
	//vec3_t			traceMaxs = { 20, 20, 32 };

	vec3_t			traceMins = { -10, -10, -1 };
	vec3_t			traceMaxs = { 10, 10, 32/*16*/ };
	//vec3_t			traceMins = { -20, -20, -1 };
	//vec3_t			traceMaxs = { 20, 20, 32 };

	//vec3_t	traceMins = { -10, -10, -1 };
	//vec3_t	traceMaxs = { 10, 10, 15 };

	VectorCopy( traceMins, mins );
	VectorCopy( traceMaxs, maxs );
	VectorCopy( org2, newOrg );
	VectorCopy( org1, newOrg2 );

	newOrg[2] += 1; // Look from up a little...
	newOrg2[2] += 1; // Look from up a little...

	//CG_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
	CG_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	
	if (tr.startsolid || tr.allsolid)
		return ( 0 );

	if ( tr.fraction == 1 && !(tr.contents & CONTENTS_LAVA))
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( tr2.fraction == 1 && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if (tr.surfaceFlags & SURF_LADDER)
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( (tr2.fraction == 1 || (tr2.surfaceFlags & SURF_LADDER)) && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if ( tr.fraction != 1 
		&& tr.entityNum != ENTITYNUM_NONE 
		&& tr.entityNum < ENTITYNUM_MAX_NORMAL )
	{
		switch (cg_entities[tr.entityNum].currentState.eType)
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
			{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
				/*trace_t tr2;
				CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA );

				if ( tr2.fraction != 1 
					&& tr2.entityNum != ENTITYNUM_NONE 
					&& tr2.entityNum < ENTITYNUM_MAX_NORMAL )
				{
					if (tr2.entityNum == tr.entityNum)
						return ( 4 );
				}*/
				trace_t tr2;
				CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

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


/* */
int
NodeVisible2 ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg;
	vec3_t	mins, maxs;

	VectorCopy( botTraceMins, mins );
	VectorCopy( botTraceMaxs, maxs );
	VectorCopy( org2, newOrg );

	newOrg[2] += 4; // Look from up a little...

	//CG_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
	CG_Trace( &tr, org1, mins, maxs, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	
	if (tr.startsolid || tr.allsolid)
		return ( 0 );

	if ( tr.fraction == 1 && !(tr.contents & CONTENTS_LAVA))
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		CG_Trace( &tr2, org1, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( tr2.fraction == 1 && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if (tr.surfaceFlags & SURF_LADDER)
	{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
		trace_t tr2;
		CG_Trace( &tr2, org1, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

		if ( (tr2.fraction == 1 || (tr2.surfaceFlags & SURF_LADDER)) && !(tr2.contents & CONTENTS_LAVA) )
		{
			return ( 1 );
		}
	}

	if ( tr.fraction != 1 
		&& tr.entityNum != ENTITYNUM_NONE 
		&& tr.entityNum < ENTITYNUM_MAX_NORMAL )
	{
		switch (cg_entities[tr.entityNum].currentState.eType)
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
			{// UQ1: Add non-box trace check! Make sure a corner of the box trace was not the only hit!
				/*trace_t tr2;
				CG_Trace( &tr2, newOrg2, NULL, NULL, newOrg, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA );

				if ( tr2.fraction != 1 
					&& tr2.entityNum != ENTITYNUM_NONE 
					&& tr2.entityNum < ENTITYNUM_MAX_NORMAL )
				{
					if (tr2.entityNum == tr.entityNum)
						return ( 4 );
				}*/
				trace_t tr2;
				CG_Trace( &tr2, org1, NULL, NULL, newOrg, tr.entityNum, MASK_PLAYERSOLID | CONTENTS_LAVA );

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

float FloorNormalAt ( vec3_t org ); // Below...

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

	//newOrg[2] += 4;		// Look from up a little...
	//newOrg2[2] += 4;	// Look from up a little...
	//newOrg[2] += 12;		// Look from up a little...
	//newOrg2[2] += 12;	// Look from up a little...

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

	CG_Trace( &tr, newOrg, mins, maxs, newOrg2, ignore, MASK_PLAYERSOLID | CONTENTS_LAVA/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA*/ /*MASK_WATER*/ );
	
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

#ifdef __OLD_COVER_SPOTS__
int		num_axis_cover_spots = 0;
vec3_t	axis_cover_spots[65536];
int		num_allied_cover_spots = 0;
vec3_t	allied_cover_spots[65536];
int		num_fake_cover_spots = 0;

void
AIMOD_SaveCoverPoints ( void )
{
	fileHandle_t	f;
	int				i;
	vmCvar_t		mapname;
	char			map[64] = "";
	char			filename[60];

	CG_Printf( "^3*** ^3%s: ^7Saving cover point table.\n", GAME_VERSION );
	strcpy( filename, "nodes/" );

	////////////////////
//	trap_Cvar_VariableStringBuffer( "g_scriptName", filename, sizeof(filename) );
//	if ( strlen( filename) > 0 )
//	{
//		trap_Cvar_Register( &mapname, "g_scriptName", "", CVAR_ROM );
//	}
//	else
//	{
		trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );
//	}

	Q_strcat( filename, sizeof(filename), mapname.string );

	///////////////////
	//try to open the output file, return if it failed
	trap_FS_FOpenFile( va( "%s.cpf", filename), &f, FS_WRITE );
	if ( !f )
	{
		CG_Printf( "^1*** ^3ERROR^5: Error opening cover point file ^7%s.cpf^5!!!\n", filename );
		return;
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );	//get the map name
	strcpy( map, mapname.string );
									//write the map name
	trap_FS_Write( &num_axis_cover_spots, sizeof(/*short*/ int), f );							//write the number of nodes in the map

	for ( i = 0; i < num_axis_cover_spots; i++ )											//loop through all the nodes
	{
		trap_FS_Write( &(axis_cover_spots[i]), sizeof(vec3_t), f );
	}

	trap_FS_Write( &num_allied_cover_spots, sizeof(/*short*/ int), f );							//write the number of nodes in the map

	for ( i = 0; i < num_allied_cover_spots; i++ )											//loop through all the nodes
	{
		trap_FS_Write( &(allied_cover_spots[i]), sizeof(vec3_t), f );
	}

	trap_FS_FCloseFile( f );		

	CG_Printf( "^3*** ^3%s: ^5Cover point table saved to file ^7%s.cpf^5.\n", GAME_VERSION, filename );
}

void AIMOD_Generate_Cover_Spots ( void )
{
	int update_timer = 0;

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Generating cover points...\n") );
	strcpy( task_string3, va("^5Generating cover points...") );
	trap_UpdateScreen();

	aw_percent_complete = 0.0f;

	{// Need to make some from waypoint list if we can!
		// Try loading from a file first (for speed)
		if (number_of_nodes > 0)
		{
			int i = 0;
			int tests = 0;

//#pragma omp parallel for
			for (i = 0; i < number_of_nodes; i++)
			{
				int j = 0;
				int num_found = 0;

				// Draw a nice little progress bar ;)
				aw_percent_complete = (float)((float)i/(float)number_of_nodes)*100.0f;

				update_timer++;

				if (update_timer >= 300)
				{
					trap_UpdateScreen();
					update_timer = 0;
				}

				for (j = 0; j < number_of_nodes; j++)
				{
					if (VectorDistance(nodes[i].origin, nodes[j].origin) < /*128*/(waypoint_scatter_distance*waypoint_distance_multiplier)*4)
					{
						qboolean	bad = qfalse;
						int			z = 0;

						for (z = 0; z < nodes[i].enodenum; z++)
						{
							if (nodes[i].links[z].targetNode == j)
							{
								bad = qtrue;
								break;
							}
						}

						if (!bad)
						{// We found a close node thats not a link! Must be blocked by something!
							vec3_t org1, org2;

							VectorCopy(nodes[i].origin, org1);
							VectorCopy(nodes[j].origin, org2);
							//org1[2]+=24;
							//org2[2]+=24;

							//if (!AW_NodeVisible(org1, org2, -1))
							{
								org1[2]+=32;//48;
								org2[2]+=64;//48;

								if (AW_NodeVisible(org1, org2, -1) >= 1)
								{
									num_found++;

									if (num_found > 1)
										break;
								}
							}
						}
					}
				}

				if (num_found > 1)
				{
					VectorCopy(nodes[i].origin, axis_cover_spots[num_axis_cover_spots]);
					num_axis_cover_spots++;
					VectorCopy(nodes[i].origin, allied_cover_spots[num_allied_cover_spots]);
					num_allied_cover_spots++;
				}
			}

			/*if (bot_debug.integer)
			{
				CG_Printf("^4*** ^3BOT DEBUG^5: Generated ^7%i^5 cover spots for each team.\n", num_allied_cover_spots);
				CG_Printf("^4*** ^3BOT DEBUG^5: There are also ^7%i^5 fake cover spots for each team (converted).\n", num_fake_cover_spots);
			}*/


			// Save them for fast loading next time!
			AIMOD_SaveCoverPoints();
		}
	}

	aw_percent_complete = 0.0f;
}
#else //!__OLD_COVER_SPOTS__

int			num_cover_spots = 0;
int			cover_nodes[MAX_NODES];

//standard visibility check
int
OrgVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	CG_Trace( &tr, org1, NULL, NULL, org2, ignore, MASK_SOLID | MASK_OPAQUE | MASK_WATER );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	return ( 0 );
}

void
AIMOD_SaveCoverPoints ( void )
{
	fileHandle_t	f;
	int				i;
	vmCvar_t		mapname;
	char			map[64] = "";
	char			filename[60];

	CG_Printf( "^3*** ^3%s: ^7Saving cover point table.\n", "AUTO-WAYPOINTER" );
	strcpy( filename, "nodes/" );

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	Q_strcat( filename, sizeof(filename), mapname.string );

	///////////////////
	//try to open the output file, return if it failed
	trap_FS_FOpenFile( va( "%s.cpw", filename), &f, FS_WRITE );
	if ( !f )
	{
		CG_Printf( "^1*** ^3ERROR^5: Error opening cover point file ^7%s.cpw^5!!!\n", "AUTO-WAYPOINTER", filename );
		return;
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );	//get the map name
	Q_strncpyz( map, mapname.string, 64 );

	trap_FS_Write( &number_of_nodes, sizeof(int), f );							//write the number of nodes in the map
									//write the map name
	trap_FS_Write( &num_cover_spots, sizeof(int), f );							//write the number of nodes in the map

	for ( i = 0; i < num_cover_spots; i++ )											//loop through all the nodes
	{
		trap_FS_Write( &(cover_nodes[i]), sizeof(int), f );
	}

	trap_FS_FCloseFile( f );		

	CG_Printf( "^3*** ^3%s: ^5Cover point table saved to file ^7%s.cpw^5.\n", "AUTO-WAYPOINTER", filename );
}

void AIMOD_Generate_Cover_Spots ( void )
{
	int x = 0;

	num_cover_spots = 0;

	for (x = 0; x < MAX_NODES; x++)
	{// Init them!
		cover_nodes[x] = -1;
	}

	{// Need to make some from waypoint list if we can!
		if (number_of_nodes > 40000)
		{
			CG_Printf("^3*** ^3%s: ^5Too many waypoints to make cover spots. Use ^3/awo^5 (auto-waypoint optimizer) to reduce the numbers!\n", "AUTO-WAYPOINTER");
			return;
		}

		if (number_of_nodes > 0)
		{
			int i = 0;
			int update_timer = 0;

			CG_Printf( "^1*** ^3%s^1: ^5Generating and saving coverspot waypoints list.\n", "AUTO-WAYPOINTER" );
			strcpy( task_string3, va("^5Generating and saving coverspot waypoints list...") );
			trap_UpdateScreen();

			aw_percent_complete = 0.0f;

			for (i = 0; i < number_of_nodes; i++)
			{
				int		j = 0;
//				int		num_found = 0;
				vec3_t	up_org2;

				// Draw a nice little progress bar ;)
				aw_percent_complete = (float)((float)i/(float)number_of_nodes)*100.0f;

				update_timer++;

				if (update_timer >= 100)
				{
					trap_UpdateScreen();
					update_timer = 0;
				}

				VectorCopy(nodes[i].origin, up_org2);
				up_org2[2]+=DEFAULT_VIEWHEIGHT; // Standing height!

				for (j = 0; j < number_of_nodes; j++)
				{
					if (VectorDistance(nodes[i].origin, nodes[j].origin) < 512)
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
							up_org[2]+=DEFAULT_VIEWHEIGHT; // Standing height!

							if (OrgVisible(up_org, up_org2, -1))
							{
								nodes[i].type |= NODE_COVER;
								cover_nodes[num_cover_spots] = i;
								num_cover_spots++;

								strcpy( last_node_added_string, va("^5Waypoint ^3%i ^5set as a cover waypoint.", i) );
								break;
							}
						}
					}
				}
			}

			//if (bot_debug.integer)
			{
				CG_Printf( "^1*** ^3%s^1: ^5 Generated ^7%i^5 coverspot waypoints.\n", "AUTO-WAYPOINTER", num_cover_spots );
			}

			// Save them for fast loading next time!
			AIMOD_SaveCoverPoints();
		}
	}
}
#endif //__OLD_COVER_SPOTS__

#ifdef __CUDA__
__device__ __host__ qboolean AIMod_Check_Slope_Between ( vec3_t org1, vec3_t org2 ) {  
	__shared__ vec3_t	orgA, orgB;
	__shared__ trace_t tr;
	__shared__ vec3_t	forward, right, up, dir;
	__shared__ vec3_t	testangles;

	VectorCopy(org1, orgA);
	VectorSubtract(org2, org1, dir);
	vectoangles(dir, testangles);
	AngleVectors( testangles, forward, right, up );
	VectorMA(orgA, (VectorDistanceNoHeight(org1, org2)*0.25), forward, orgA);
	
	orgA[2] += 16;

	VectorCopy(orgA, orgB);
	orgB[2] -= 256;//-65000;
	
	__host__ CG_Trace( &tr, orgA, NULL, NULL, orgB, -1, MASK_PLAYERSOLID );
	
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
	
	__host__ CG_Trace( &tr, orgA, NULL, NULL, orgB, -1, MASK_PLAYERSOLID );
	
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
	
	__host__ CG_Trace( &tr, orgA, NULL, NULL, orgB, -1, MASK_PLAYERSOLID );
	
	if (tr.endpos[2]+56 < org1[2])
		return qfalse;

	if ( tr.fraction == 1 || (tr.contents & CONTENTS_LAVA) )
		return qfalse;

	return qtrue;
}

__device__ __host__ qboolean NodeIsAlreadyALink ( int start_node, int end_node, int num_links )
{
	__shared__ int i = 0;

	for (i = 0; i < num_links; i++)
	{
		if (nodes[start_node].links[i].targetNode == end_node)
			return qtrue;
	}

	return qfalse;
}

__device__ __host__ qboolean AIMOD_MAPPING_InFOV ( vec3_t spot, vec3_t from, vec3_t fromAngles, int hFOV, int vFOV )
{
	__shared__ vec3_t	deltaVector, angles, deltaAngles;
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
#else //!__CUDA__
qboolean AIMod_Check_Slope_Between ( vec3_t org1, vec3_t org2 ) {  
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
	
	CG_Trace( &tr, orgA, NULL, NULL, orgB, -1, MASK_PLAYERSOLID );
	
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
	
	CG_Trace( &tr, orgA, NULL, NULL, orgB, -1, MASK_PLAYERSOLID );
	
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
	
	CG_Trace( &tr, orgA, NULL, NULL, orgB, -1, MASK_PLAYERSOLID );
	
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
#endif //__CUDA__

#define MAX_NODE_LINK_DISTANCE 256//128//128//256
#define MAX_LADDER_NODE_LINK_DISTANCE 256//128//256//512//96
#define MAX_TANK_NODE_LINK_DISTANCE 256//128//512

#ifdef __CUDA__
__device__ __host__ int AIMOD_MAPPING_CreateNodeLinks ( int node )
{
	__shared__ int		loop = 0;
	__shared__ int		current_angle = 0;
	__shared__ vec3_t	angles;
	__shared__ int		linknum = 0;
	__shared__ vec3_t	tankMaxsSize = {64, 64, 0};
	__shared__ vec3_t	tankMinsSize = {-64, -64, 0};
	__shared__ int		best_node = -1;
	__shared__ int		best_flags = 0;
	__shared__ int		best_dist = 1024.0f;
	__shared__ int		in_range_nodes[MAX_NODES];
	__shared__ int		in_range_flags[MAX_NODES];
	__shared__ int		num_in_range_nodes = 0;

#ifdef __VEHICLES__
	// First add tank links...
	if (nodes[node].type & NODE_LAND_VEHICLE)
	{
		num_in_range_nodes = 0;
		best_node = -1;
		best_flags = 0;
		
		/*if (number_of_nodes >= 40000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 1.5) + 0.1f;
		else if (number_of_nodes >= 35000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 1.75) + 0.1f;
		else if (number_of_nodes >= 30000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 2.5) + 0.1f;
		else if (number_of_nodes >= 25000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 3) + 0.1f;
		else if (number_of_nodes >= 20000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 3.25) + 0.1f;
		else if (number_of_nodes >= 15000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 3.5) + 0.1f;
		else if (number_of_nodes >= 12000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 3.75) + 0.1f;
		else if (number_of_nodes >= 10000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 4) + 0.1f;
		else if (number_of_nodes >= 8000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 4.5) + 0.1f;
		else if (number_of_nodes >= 5000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 4.75) + 0.1f;
		else
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 5) + 0.1f;*/

		best_dist = MAX_TANK_NODE_LINK_DISTANCE;

		angles[PITCH] = 0;
		angles[YAW] = current_angle;
		angles[ROLL] = 0;

		// Find the best (closest) node on each angle...
		for ( loop = 0; loop < number_of_nodes; loop++ )
		{
			__shared__ int visCheck = -1;
			__shared__ float dist = VectorDistance(nodes[loop].origin, nodes[node].origin);

			if (dist > best_dist)
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

			__host__ visCheck = TankNodeVisible( nodes[loop].origin, nodes[node].origin, tankMinsSize, tankMaxsSize, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					in_range_nodes[num_in_range_nodes] = loop;
					num_in_range_nodes++;
				}
			}
		}

		for ( current_angle = 0; current_angle <= 360; current_angle+=45 ) // 8 angles tested, 45 degrees each...
		{
			__shared__ int second_best_node = -1;
			__shared__ int third_best_node = -1;
			best_node = -1;
			best_flags = 0;
			best_dist = best_dist + 0.1f;

			angles[PITCH] = 0;
			angles[YAW] = current_angle;
			angles[ROLL] = 0;

			for ( loop = 0; loop < num_in_range_nodes; loop++ )
			{
				__shared__ float dist = VectorDistance(nodes[in_range_nodes[loop]].origin, nodes[node].origin);

				if (dist > best_dist)
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

	/*if (number_of_nodes >= 40000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 1.5) + 0.1f;
	else if (number_of_nodes >= 35000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 1.75) + 0.1f;
	else if (number_of_nodes >= 30000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 2.5) + 0.1f;
	else if (number_of_nodes >= 25000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 3) + 0.1f;
	else if (number_of_nodes >= 20000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 3.25) + 0.1f;
	else if (number_of_nodes >= 15000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 3.5) + 0.1f;
	else if (number_of_nodes >= 12000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 3.75) + 0.1f;
	else if (number_of_nodes >= 10000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 4) + 0.1f;
	else if (number_of_nodes >= 8000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 4.5) + 0.1f;
	else if (number_of_nodes >= 5000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 4.75) + 0.1f;
	else
		best_dist = (MAX_NODE_LINK_DISTANCE * 5) + 0.1f;*/

	best_dist = MAX_NODE_LINK_DISTANCE;

	// Find the best (closest) node on each angle...
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		__shared__ int visCheck = -1;
		__shared__ float dist = VectorDistance(nodes[loop].origin, nodes[node].origin);

		if (dist > best_dist)
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

		__host__ visCheck = NodeVisible( nodes[loop].origin, nodes[node].origin, -1 );

		//0 = wall in way
		//1 = player or no obstruction
		//2 = useable door in the way.
		//3 = door entity in the way.
		if ( visCheck == 1 )
		{
			if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
			{
				in_range_nodes[num_in_range_nodes] = loop;
				num_in_range_nodes++;
			}
		}
		else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
		{
			if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
			{
				in_range_nodes[num_in_range_nodes] = loop;
				in_range_flags[num_in_range_nodes] = 0;
				num_in_range_nodes++;
			}
		}
		else
		{// Look for jump node links...
			__host__ visCheck = NodeVisibleJump( nodes[loop].origin, nodes[node].origin, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					in_range_nodes[num_in_range_nodes] = loop;
					in_range_flags[num_in_range_nodes] = PATH_JUMP;
					num_in_range_nodes++;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					in_range_nodes[num_in_range_nodes] = loop;
					in_range_flags[num_in_range_nodes] = PATH_JUMP;
					num_in_range_nodes++;
				}
			}
			else
			{// Look for crouch node links...
				__host__ visCheck = NodeVisibleCrouch( nodes[loop].origin, nodes[node].origin, -1 );

				//0 = wall in way
				//1 = player or no obstruction
				//2 = useable door in the way.
				//3 = door entity in the way.
				if ( visCheck == 1 )
				{
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						in_range_nodes[num_in_range_nodes] = loop;
						in_range_flags[num_in_range_nodes] = PATH_CROUCH;
						num_in_range_nodes++;
					}
				}
				else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
				{
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
		

		/*if (number_of_nodes >= 40000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 1.5) + 0.1f;
		else if (number_of_nodes >= 35000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 1.75) + 0.1f;
		else if (number_of_nodes >= 30000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 2.5) + 0.1f;
		else if (number_of_nodes >= 25000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 3) + 0.1f;
		else if (number_of_nodes >= 20000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 3.25) + 0.1f;
		else if (number_of_nodes >= 15000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 3.5) + 0.1f;
		else if (number_of_nodes >= 12000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 3.75) + 0.1f;
		else if (number_of_nodes >= 10000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 4) + 0.1f;
		else if (number_of_nodes >= 8000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 4.5) + 0.1f;
		else if (number_of_nodes >= 5000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 4.75) + 0.1f;
		else
			best_dist = (MAX_NODE_LINK_DISTANCE * 5) + 0.1f;*/

		best_dist = MAX_NODE_LINK_DISTANCE;

		angles[PITCH] = 0;
		angles[YAW] = current_angle;
		angles[ROLL] = 0;

		for ( loop = 0; loop < num_in_range_nodes; loop++ )
		{
			float dist = VectorDistance(nodes[in_range_nodes[loop]].origin, nodes[node].origin);

			if (dist > best_dist)
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

	/*if (number_of_nodes >= 40000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 1.5) + 0.1f;
	else if (number_of_nodes >= 35000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 1.75) + 0.1f;
	else if (number_of_nodes >= 30000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 2.5) + 0.1f;
	else if (number_of_nodes >= 25000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3) + 0.1f;
	else if (number_of_nodes >= 20000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.25) + 0.1f;
	else if (number_of_nodes >= 15000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.5) + 0.1f;
	else if (number_of_nodes >= 12000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.75) + 0.1f;
	else if (number_of_nodes >= 10000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4) + 0.1f;
	else if (number_of_nodes >= 8000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4.5) + 0.1f;
	else if (number_of_nodes >= 5000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4.75) + 0.1f;
	else
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 5) + 0.1f;*/

	best_dist = MAX_LADDER_NODE_LINK_DISTANCE;

	angles[PITCH] = 90;
	angles[YAW] = 0;
	angles[ROLL] = 0;

	// Find the best (closest) node on each angle...
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		__shared__ float dist = VectorDistance(nodes[loop].origin, nodes[node].origin);

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if (dist > best_dist)
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

			__host__ visCheck = NodeVisible( nodes[node].origin, nodes[loop].origin, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					best_dist = dist;
					best_node = loop;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					best_dist = dist;
					best_node = loop;
				}
			}
			else
			{// Look for jump node links...
				__host__ visCheck = NodeVisibleJump( nodes[node].origin, nodes[loop].origin, -1 );

				//0 = wall in way
				//1 = player or no obstruction
				//2 = useable door in the way.
				//3 = door entity in the way.
				if ( visCheck == 1 )
				{
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						best_dist = dist;
						best_node = loop;
						best_flags = PATH_JUMP;
					}
				}
				else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
				{
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						best_dist = dist;
						best_node = loop;
						best_flags = PATH_JUMP;
					}
				}
				else
				{// Look for crouch node links...
					__host__ visCheck = NodeVisibleCrouch( nodes[node].origin, nodes[loop].origin, -1 );

					//0 = wall in way
					//1 = player or no obstruction
					//2 = useable door in the way.
					//3 = door entity in the way.
					if ( visCheck == 1 )
					{
						if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							best_dist = dist;
							best_node = loop;
							best_flags = PATH_CROUCH;
						}
					}
					else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
					{
						if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
	
	/*if (number_of_nodes >= 40000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 1.5) + 0.1f;
	else if (number_of_nodes >= 35000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 1.75) + 0.1f;
	else if (number_of_nodes >= 30000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 2.5) + 0.1f;
	else if (number_of_nodes >= 25000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3) + 0.1f;
	else if (number_of_nodes >= 20000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.25) + 0.1f;
	else if (number_of_nodes >= 15000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.5) + 0.1f;
	else if (number_of_nodes >= 12000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.75) + 0.1f;
	else if (number_of_nodes >= 10000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4) + 0.1f;
	else if (number_of_nodes >= 8000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4.5) + 0.1f;
	else if (number_of_nodes >= 5000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4.75) + 0.1f;
	else
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 5) + 0.1f;*/

	best_dist = MAX_LADDER_NODE_LINK_DISTANCE;

	angles[PITCH] = -90;
	angles[YAW] = 0;
	angles[ROLL] = 0;

	// Find the best (closest) node on each angle...
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		__shared__ float dist = VectorDistance(nodes[loop].origin, nodes[node].origin);

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if (dist > best_dist)
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
			__shared__ int visCheck = -1;
			__shared__ vec3_t temp;

			if (AIMOD_MAPPING_InFOV(nodes[loop].origin, nodes[node].origin, angles, 48/*20*/, 48/*20*/))
			{
				continue;
			}

			__host__ visCheck = NodeVisible( nodes[node].origin, nodes[loop].origin, -1 );

			if (visCheck <= 0)
			{
				// UQ1: look from above to try to find jump-down places where the old system failed!
				VectorCopy(nodes[node].origin, temp);
				temp[0] = nodes[loop].origin[0];
				temp[1] = nodes[loop].origin[1];

				__host__ visCheck = NodeVisible( nodes[node].origin, temp, -1 );

				if (visCheck == 1)
				{
					__host__ visCheck = NodeVisible( temp, nodes[loop].origin, -1 );
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
				//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					best_dist = dist;
					best_node = loop;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					best_dist = dist;
					best_node = loop;
				}
			}
			else
			{// Look for jump node links...
				__host__ visCheck = NodeVisibleJump( nodes[node].origin, nodes[loop].origin, -1 );

				//0 = wall in way
				//1 = player or no obstruction
				//2 = useable door in the way.
				//3 = door entity in the way.
				if ( visCheck == 1 )
				{
					//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						best_dist = dist;
						best_node = loop;
						best_flags = PATH_JUMP;
					}
				}
				else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
				{
					//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						best_dist = dist;
						best_node = loop;
						best_flags = PATH_JUMP;
					}
				}
				else
				{// Look for crouch node links...
					__host__ visCheck = NodeVisibleCrouch( nodes[node].origin, nodes[loop].origin, -1 );

					//0 = wall in way
					//1 = player or no obstruction
					//2 = useable door in the way.
					//3 = door entity in the way.
					if ( visCheck == 1 )
					{
						//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							best_dist = dist;
							best_node = loop;
							best_flags = PATH_CROUCH;
						}
					}
					else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
					{
						//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
#else //!__CUDA__
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
		
		if (number_of_nodes >= 40000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 1.5) + 0.1f;
		else if (number_of_nodes >= 35000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 1.75) + 0.1f;
		else if (number_of_nodes >= 30000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 2.5) + 0.1f;
		else if (number_of_nodes >= 25000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 3) + 0.1f;
		else if (number_of_nodes >= 20000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 3.25) + 0.1f;
		else if (number_of_nodes >= 15000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 3.5) + 0.1f;
		else if (number_of_nodes >= 12000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 3.75) + 0.1f;
		else if (number_of_nodes >= 10000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 4) + 0.1f;
		else if (number_of_nodes >= 8000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 4.5) + 0.1f;
		else if (number_of_nodes >= 5000)
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 4.75) + 0.1f;
		else
			best_dist = (MAX_TANK_NODE_LINK_DISTANCE * 5) + 0.1f;

		angles[PITCH] = 0;
		angles[YAW] = current_angle;
		angles[ROLL] = 0;

		// Find the best (closest) node on each angle...
		for ( loop = 0; loop < number_of_nodes; loop++ )
		{
			 int visCheck = -1;
			 float dist = VectorDistance(nodes[loop].origin, nodes[node].origin);

			if (dist > best_dist)
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
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
			best_dist = best_dist + 0.1f;

			angles[PITCH] = 0;
			angles[YAW] = current_angle;
			angles[ROLL] = 0;

			for ( loop = 0; loop < num_in_range_nodes; loop++ )
			{
				 float dist = VectorDistance(nodes[in_range_nodes[loop]].origin, nodes[node].origin);

				if (dist > best_dist)
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

	if (number_of_nodes >= 40000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 1.5) + 0.1f;
	else if (number_of_nodes >= 35000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 1.75) + 0.1f;
	else if (number_of_nodes >= 30000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 2.5) + 0.1f;
	else if (number_of_nodes >= 25000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 3) + 0.1f;
	else if (number_of_nodes >= 20000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 3.25) + 0.1f;
	else if (number_of_nodes >= 15000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 3.5) + 0.1f;
	else if (number_of_nodes >= 12000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 3.75) + 0.1f;
	else if (number_of_nodes >= 10000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 4) + 0.1f;
	else if (number_of_nodes >= 8000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 4.5) + 0.1f;
	else if (number_of_nodes >= 5000)
		best_dist = (MAX_NODE_LINK_DISTANCE * 4.75) + 0.1f;
	else
		best_dist = (MAX_NODE_LINK_DISTANCE * 5) + 0.1f;

	// Find the best (closest) node on each angle...
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		 int visCheck = -1;
		 float dist = VectorDistance(nodes[loop].origin, nodes[node].origin);

		if (dist > best_dist)
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
			if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
			{
				in_range_nodes[num_in_range_nodes] = loop;
				num_in_range_nodes++;
			}
		}
		else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
		{
			if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					in_range_nodes[num_in_range_nodes] = loop;
					in_range_flags[num_in_range_nodes] = PATH_JUMP;
					num_in_range_nodes++;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						in_range_nodes[num_in_range_nodes] = loop;
						in_range_flags[num_in_range_nodes] = PATH_CROUCH;
						num_in_range_nodes++;
					}
				}
				else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
				{
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
		

		if (number_of_nodes >= 40000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 1.5) + 0.1f;
		else if (number_of_nodes >= 35000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 1.75) + 0.1f;
		else if (number_of_nodes >= 30000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 2.5) + 0.1f;
		else if (number_of_nodes >= 25000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 3) + 0.1f;
		else if (number_of_nodes >= 20000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 3.25) + 0.1f;
		else if (number_of_nodes >= 15000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 3.5) + 0.1f;
		else if (number_of_nodes >= 12000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 3.75) + 0.1f;
		else if (number_of_nodes >= 10000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 4) + 0.1f;
		else if (number_of_nodes >= 8000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 4.5) + 0.1f;
		else if (number_of_nodes >= 5000)
			best_dist = (MAX_NODE_LINK_DISTANCE * 4.75) + 0.1f;
		else
			best_dist = (MAX_NODE_LINK_DISTANCE * 5) + 0.1f;

		angles[PITCH] = 0;
		angles[YAW] = current_angle;
		angles[ROLL] = 0;

		for ( loop = 0; loop < num_in_range_nodes; loop++ )
		{
			float dist = VectorDistance(nodes[in_range_nodes[loop]].origin, nodes[node].origin);

			if (dist > best_dist)
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

	if (number_of_nodes >= 40000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 1.5) + 0.1f;
	else if (number_of_nodes >= 35000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 1.75) + 0.1f;
	else if (number_of_nodes >= 30000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 2.5) + 0.1f;
	else if (number_of_nodes >= 25000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3) + 0.1f;
	else if (number_of_nodes >= 20000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.25) + 0.1f;
	else if (number_of_nodes >= 15000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.5) + 0.1f;
	else if (number_of_nodes >= 12000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.75) + 0.1f;
	else if (number_of_nodes >= 10000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4) + 0.1f;
	else if (number_of_nodes >= 8000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4.5) + 0.1f;
	else if (number_of_nodes >= 5000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4.75) + 0.1f;
	else
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 5) + 0.1f;

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

		if (dist > best_dist)
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
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					best_dist = dist;
					best_node = loop;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						best_dist = dist;
						best_node = loop;
						best_flags = PATH_JUMP;
					}
				}
				else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
				{
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
						if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							best_dist = dist;
							best_node = loop;
							best_flags = PATH_CROUCH;
						}
					}
					else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
					{
						if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
	
	if (number_of_nodes >= 40000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 1.5) + 0.1f;
	else if (number_of_nodes >= 35000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 1.75) + 0.1f;
	else if (number_of_nodes >= 30000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 2.5) + 0.1f;
	else if (number_of_nodes >= 25000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3) + 0.1f;
	else if (number_of_nodes >= 20000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.25) + 0.1f;
	else if (number_of_nodes >= 15000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.5) + 0.1f;
	else if (number_of_nodes >= 12000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 3.75) + 0.1f;
	else if (number_of_nodes >= 10000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4) + 0.1f;
	else if (number_of_nodes >= 8000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4.5) + 0.1f;
	else if (number_of_nodes >= 5000)
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 4.75) + 0.1f;
	else
		best_dist = (MAX_LADDER_NODE_LINK_DISTANCE * 5) + 0.1f;

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

		if (dist > best_dist)
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
				//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					best_dist = dist;
					best_node = loop;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
					//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						best_dist = dist;
						best_node = loop;
						best_flags = PATH_JUMP;
					}
				}
				else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
				{
					//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
						//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							best_dist = dist;
							best_node = loop;
							best_flags = PATH_CROUCH;
						}
					}
					else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
					{
						//if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
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
#endif //__CUDA__

#ifdef __OLD__
/* */
int
AIMOD_MAPPING_CreateNodeLinks_OLD ( int node )
{
	int		loop = 0;
	int		linknum = 0;
	//vec3_t	tankMaxsSize = {96, 96, 0};
	//vec3_t	tankMinsSize = {-96, -96, 0};
	vec3_t	tankMaxsSize = {80, 80, 0};
	vec3_t	tankMinsSize = {-80, -80, 0};
	//vec3_t	tankMaxsSize = {128, 128, 1};
	//vec3_t	tankMinsSize = {-128, -128, -1};

#ifdef __VEHICLES__
	if (nodes[node].type & NODE_LAND_VEHICLE)
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		vec3_t	tmp;
		VectorCopy( nodes[node].origin, tmp );

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if (!(nodes[loop].type & NODE_LAND_VEHICLE))
		{
			continue;
		}

		if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
		{
			continue;
		}

		if (NodeIsAlreadyALink(node, loop, linknum))
			continue;

		if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= 64)
		{
			int visCheck = TankNodeVisible( nodes[loop].origin, tmp, tankMinsSize, tankMaxsSize, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4);

					if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
						nodes[node].links[linknum].cost = 65000.0f;

					linknum++;
				}
			}
		}
	}

	if (linknum < MAX_NODELINKS && (nodes[node].type & NODE_LAND_VEHICLE))
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		vec3_t	tmp;
		VectorCopy( nodes[node].origin, tmp );

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if (!(nodes[loop].type & NODE_LAND_VEHICLE))
		{
			continue;
		}

		if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
		{
			continue;
		}

		if (NodeIsAlreadyALink(node, loop, linknum))
			continue;

		if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= 128
			&& VectorDistance( nodes[loop].origin, nodes[node].origin) > 64)
		{
			int visCheck = TankNodeVisible( nodes[loop].origin, tmp, tankMinsSize, tankMaxsSize, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4);

					if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
						nodes[node].links[linknum].cost = 65000.0f;

					linknum++;
				}
			}
		}
	}

	if (linknum < MAX_NODELINKS && (nodes[node].type & NODE_LAND_VEHICLE))
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		vec3_t	tmp;
		VectorCopy( nodes[node].origin, tmp );

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if (!(nodes[loop].type & NODE_LAND_VEHICLE))
		{
			continue;
		}

		if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
		{
			continue;
		}

		if (NodeIsAlreadyALink(node, loop, linknum))
			continue;

		if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= 256
			&& VectorDistance( nodes[loop].origin, nodes[node].origin) > 128)
		{
			int visCheck = TankNodeVisible( nodes[loop].origin, tmp, tankMinsSize, tankMaxsSize, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4);

					if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
						nodes[node].links[linknum].cost = 65000.0f;

					linknum++;
				}
			}
		}
	}

	if (linknum < MAX_NODELINKS && (nodes[node].type & NODE_LAND_VEHICLE))
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		vec3_t	tmp;
		VectorCopy( nodes[node].origin, tmp );

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if (!(nodes[loop].type & NODE_LAND_VEHICLE))
		{
			continue;
		}

		if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
		{
			continue;
		}

		if (NodeIsAlreadyALink(node, loop, linknum))
			continue;

		if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= 512
			&& VectorDistance( nodes[loop].origin, nodes[node].origin) > 384)
		{
			int visCheck = TankNodeVisible( nodes[loop].origin, tmp, tankMinsSize, tankMaxsSize, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4);

					if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
						nodes[node].links[linknum].cost = 65000.0f;

					linknum++;
				}
			}
		}
	}
#endif //__VEHICLES__

	if (linknum >= MAX_NODELINKS)
		CG_Printf("Node %i - hit max links on first vehicle pass!\n", node);

	if (linknum < MAX_NODELINKS)
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		vec3_t	tmp;
		VectorCopy( nodes[node].origin, tmp );

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
		{
			continue;
		}

		//if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= (waypoint_scatter_distance*waypoint_distance_multiplier)*4 )
		//if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= (waypoint_scatter_distance*waypoint_distance_multiplier)*2 )
		if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= (waypoint_scatter_distance*waypoint_distance_multiplier)*1.5 )
		{
			int visCheck = NodeVisible( nodes[loop].origin, tmp, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 /*|| loop == node - 1*/ )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin) * 4);

					if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
						nodes[node].links[linknum].cost = 65000.0f;

					linknum++;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4) * 100;
					nodes[node].links[linknum].flags |= PATH_JUMP;

					if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
						nodes[node].links[linknum].cost = 65000.0f;

					linknum++;
				}
			}
			else
			{// Look for jump node links...
				visCheck = NodeVisibleJump( nodes[loop].origin, tmp, -1 );

				//0 = wall in way
				//1 = player or no obstruction
				//2 = useable door in the way.
				//3 = door entity in the way.
				if ( visCheck == 1 )
				{
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						nodes[node].links[linknum].targetNode = loop;
						nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4) * 10;
						nodes[node].links[linknum].flags |= PATH_JUMP;

						if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
							nodes[node].links[linknum].cost = 65000.0f;

						linknum++;
					}
				}
				else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
				{
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						nodes[node].links[linknum].targetNode = loop;
						nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4) * 100;
						nodes[node].links[linknum].flags |= PATH_JUMP;

						if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
							nodes[node].links[linknum].cost = 65000.0f;

						linknum++;
					}
				}
				else
				{// Look for crouch node links...
					visCheck = NodeVisibleCrouch( nodes[loop].origin, tmp, -1 );

					//0 = wall in way
					//1 = player or no obstruction
					//2 = useable door in the way.
					//3 = door entity in the way.
					if ( visCheck == 1 )
					{
						if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							nodes[node].links[linknum].targetNode = loop;
							nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4) * 4;
							nodes[node].links[linknum].flags |= PATH_CROUCH;

							if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
								nodes[node].links[linknum].cost = 65000.0f;

							linknum++;
						}
					}
					else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
					{
						if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							nodes[node].links[linknum].targetNode = loop;
							nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4) * 100;
							nodes[node].links[linknum].flags |= PATH_CROUCH;

							if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
								nodes[node].links[linknum].cost = 65000.0f;

							linknum++;
						}
					}
				}
			}
		}
	}

	if (linknum >= MAX_NODELINKS)
		CG_Printf("Node %i - hit max links on first normal pass!\n", node);

#ifdef __VEHICLES__
	if (linknum < MAX_NODELINKS && (nodes[node].type & NODE_LAND_VEHICLE) )
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		vec3_t	tmp;
		VectorCopy( nodes[node].origin, tmp );

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if (!(nodes[loop].type & NODE_LAND_VEHICLE))
		{
			continue;
		}

		if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
		{
			continue;
		}

		if (NodeIsAlreadyALink(node, loop, linknum))
			continue;

		if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= 580/*(waypoint_scatter_distance*waypoint_distance_multiplier)*16*/
			&& VectorDistance( nodes[loop].origin, nodes[node].origin) > 512/*(waypoint_scatter_distance*waypoint_distance_multiplier)*3*/)
		{
			int visCheck = TankNodeVisible( nodes[loop].origin, tmp, tankMinsSize, tankMaxsSize, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4);

					if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
						nodes[node].links[linknum].cost = 65000.0f;

					linknum++;
				}
			}
		}
	}
#endif //__VEHICLES__

	// Add more if possible...
	if (linknum < MAX_NODELINKS)
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		vec3_t	tmp;
		VectorCopy( nodes[node].origin, tmp );

		if ( linknum >= MAX_NODELINKS )
		{
			break;
		}

		if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
		{
			continue;
		}

		if (NodeIsAlreadyALink(node, loop, linknum))
			continue;

		if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= (waypoint_scatter_distance*waypoint_distance_multiplier)*8 )
		{
			int visCheck = NodeVisible( nodes[loop].origin, tmp, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4);

					if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
						nodes[node].links[linknum].cost = 65000.0f;

					linknum++;
				}
			}
			else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
			{
				if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4) * 100;

					if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
						nodes[node].links[linknum].cost = 65000.0f;

					linknum++;
				}
			}
			else
			{// Look for jump node links...
				visCheck = NodeVisibleJump( nodes[loop].origin, tmp, -1 );

				//0 = wall in way
				//1 = player or no obstruction
				//2 = useable door in the way.
				//3 = door entity in the way.
				if ( visCheck == 1 )
				{
					if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
					{
						nodes[node].links[linknum].targetNode = loop;
						nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4) * 10;
						nodes[node].links[linknum].flags |= PATH_JUMP;

						if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
							nodes[node].links[linknum].cost = 65000.0f;

						linknum++;
					}
					else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
					{
						if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							nodes[node].links[linknum].targetNode = loop;
							nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4) * 100;
							nodes[node].links[linknum].flags |= PATH_JUMP;

							if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
								nodes[node].links[linknum].cost = 65000.0f;

							linknum++;
						}
					}
				}
				else
				{// Look for crouch node links...
					visCheck = NodeVisibleCrouch( nodes[loop].origin, tmp, -1 );

					//0 = wall in way
					//1 = player or no obstruction
					//2 = useable door in the way.
					//3 = door entity in the way.
					if ( visCheck == 1)
					{
						if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							nodes[node].links[linknum].targetNode = loop;
							nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4) * 4;
							nodes[node].links[linknum].flags |= PATH_CROUCH;

							if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
								nodes[node].links[linknum].cost = 65000.0f;

							linknum++;
						}
					}
					else if ( visCheck == 2 || visCheck == 3 || visCheck == 4 )
					{
						if (AIMod_Check_Slope_Between(nodes[node].origin, nodes[loop].origin))
						{
							nodes[node].links[linknum].targetNode = loop;
							nodes[node].links[linknum].cost = (VectorDistance( nodes[loop].origin, nodes[node].origin ) + (HeightDistance(nodes[loop].origin, nodes[node].origin)) * 4) * 100;
							nodes[node].links[linknum].flags |= PATH_CROUCH;

							if ((nodes[loop].type & NODE_ICE) || (nodes[loop].type & NODE_WATER))
								nodes[node].links[linknum].cost = 65000.0f;

							linknum++;
						}
					}
				}
			}
		}
	}

	nodes[node].enodenum = linknum;

	return ( linknum );
}
#endif //__OLD__

/* */
qboolean
AI_PM_SlickTrace ( vec3_t point, int clientNum )
{
	trace_t trace;
	vec3_t	point2;
	VectorCopy( point, point2 );
	point2[2] = point2[2] - 0.25f;

	CG_Trace( &trace, point, botTraceMins, botTraceMaxs, point2, clientNum, MASK_SHOT );

	if ( trace.surfaceFlags & SURF_SLICK )
	{
		return ( qtrue );
	}
	else
	{
		return ( qfalse );
	}
}

/* */
void
AIMOD_MAPPING_CreateSpecialNodeFlags ( int node )
{	// Need to check for duck (etc) nodes and mark them...
	trace_t tr;
	vec3_t	up, down, temp, uporg;
	//vec3_t	tankMaxsSize = {96, 96, 0};
	//vec3_t	tankMinsSize = {-96, -96, 0};
	vec3_t	tankMaxsSize = {80, 80, 0};
	vec3_t	tankMinsSize = {-80, -80, 0};
	//vec3_t	tankMaxsSize = {112, 112, 1};
	//vec3_t	tankMinsSize = {-112, -112, -1};
	//float slope;

	VectorCopy( nodes[node].origin, temp );
	temp[2] += 1;
	nodes[node].type &= ~NODE_DUCK;
	VectorCopy( nodes[node].origin, up );
	up[2] += 16550;
	CG_Trace( &tr, nodes[node].origin, NULL, NULL, up, -1, MASK_SHOT | MASK_OPAQUE | MASK_WATER );
	
	if ( VectorDistance( nodes[node].origin, tr.endpos) <= 72 )
	{	// Could not see the up pos.. Need to duck to go here!
		nodes[node].type |= NODE_DUCK;
		//CG_Printf( "^4*** ^3%s^5: Node ^7%i^5 marked as a duck node.\n", GAME_VERSION, node );
	}

	VectorCopy( nodes[node].origin, up );
	up[2] = RoofHeightAt(nodes[node].origin) - 16;
	VectorCopy( nodes[node].origin, down );
	down[2] -= 64;

	CG_Trace( &tr, up, NULL, NULL, down, -1, MASK_SHOT | MASK_OPAQUE | MASK_WATER );

	if (tr.contents & CONTENTS_WATER)
	{
		nodes[node].type |= NODE_WATER;
	}

	//if ( AI_PM_SlickTrace( nodes[node].origin, -1) )
	if ( tr.surfaceFlags & SURF_SLICK )
	{	// This node is on slippery ice... Mark it...
		nodes[node].type |= NODE_ICE;
		//CG_Printf( "^4*** ^3%s^5: Node ^7%i^5 marked as an ice (slick) node.\n", GAME_VERSION, node );
	}

	/*slope = FloorNormalAt(nodes[node].origin);

	if (slope > 40 || slope < -40)
	{// Extreme slope!
		VectorCopy(nodes[node].origin, temp);
		temp[2]+=48;

		VectorCopy(nodes[node].origin, uporg);
		uporg[2]+=60;

		if ( TankNodeVisible( temp, uporg, tankMinsSize, tankMaxsSize, -1) == 1 )
		{
			nodes[node].type |= NODE_LAND_VEHICLE;
		}
	}
	else if (slope > 36 || slope < -36)
	{// Very heavy slope!
		VectorCopy(nodes[node].origin, temp);
		temp[2]+=64;

		VectorCopy(nodes[node].origin, uporg);
		uporg[2]+=44;

		if ( TankNodeVisible( temp, uporg, tankMinsSize, tankMaxsSize, -1) == 1 )
		{
			nodes[node].type |= NODE_LAND_VEHICLE;
		}
	}
	else if (slope > 30 || slope < -30)
	{// Heavy slope!
		VectorCopy(nodes[node].origin, temp);
		temp[2]+=48;

		VectorCopy(nodes[node].origin, uporg);
		uporg[2]+=60;

		if ( TankNodeVisible( temp, uporg, tankMinsSize, tankMaxsSize, -1) == 1 )
		{
			nodes[node].type |= NODE_LAND_VEHICLE;
		}
	}
	else if (slope > 22 || slope < -22)
	{// Mid->Heigh range slope...
		VectorCopy(nodes[node].origin, temp);
		temp[2]+=32;

		VectorCopy(nodes[node].origin, uporg);
		uporg[2]+=76;

		if ( TankNodeVisible( temp, uporg, tankMinsSize, tankMaxsSize, -1) == 1 )
		{
			nodes[node].type |= NODE_LAND_VEHICLE;
		}
	}
	else if (slope > 16 || slope < -16)
	{// Mid range slope...
		VectorCopy(nodes[node].origin, temp);
		temp[2]+=24;

		VectorCopy(nodes[node].origin, uporg);
		uporg[2]+=84;

		if ( TankNodeVisible( temp, uporg, tankMinsSize, tankMaxsSize, -1) == 1 )
		{
			nodes[node].type |= NODE_LAND_VEHICLE;
		}
	}
	else if (slope > 8 || slope < -8)
	{// Slight slope...
		VectorCopy(nodes[node].origin, temp);
		temp[2]+=16;

		VectorCopy(nodes[node].origin, uporg);
		uporg[2]+=92;

		if ( TankNodeVisible( temp, uporg, tankMinsSize, tankMaxsSize, -1) == 1 )
		{
			nodes[node].type |= NODE_LAND_VEHICLE;
		}
	}
	else*/
	{// Basicly flat!
		VectorCopy(nodes[node].origin, uporg);
		uporg[2]+=104;

		if ( TankNodeVisible( nodes[node].origin, uporg, tankMinsSize, tankMaxsSize, -1) == 1 )
		{
			nodes[node].type |= NODE_LAND_VEHICLE;
		}
	}
}

//#define __BOT_AUTOWAYPOINT_OPTIMIZE__

#ifdef __CUDA__
/* */
__device__ __host__ void AIMOD_MAPPING_MakeLinks ( void )
{
	__shared__ int		loop = 0;
	__shared__ int		final_tests = 0;
	__shared__ int		update_timer = 0;

#ifdef __BOT_AUTOWAYPOINT_OPTIMIZE__
	good_nodes = B_Alloc( (sizeof(node_t)+1)*MAX_NODES );
#endif //__BOT_AUTOWAYPOINT_OPTIMIZE__

	if ( aw_num_nodes > 0 )
	{
		number_of_nodes = aw_num_nodes;
	}

	aw_percent_complete = 0.0f;
 	strcpy( last_node_added_string, va(" ") );

	CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^5Creating waypoint linkages and flags for ^3%i^5 waypoints...\n", number_of_nodes );
	strcpy( task_string3, va("^5Creating waypoint linkages and flags for ^3%i^5 waypoints...", number_of_nodes) );
	trap_UpdateScreen();

	final_tests = 0;
	update_timer = 0;
	aw_percent_complete = 0.0f;
	trap_UpdateScreen();

	//number_of_nodes = total_good_count;
	
//#pragma omp parallel for
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{// Do links...
		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)loop/(float)number_of_nodes)*100.0f);

		nodes[loop].enodenum = 0;

		update_timer++;

		if (update_timer >= 100)
		{
			trap_UpdateScreen();
			update_timer = 0;
		}

		// Also check if the node needs special flags...
		AIMOD_MAPPING_CreateSpecialNodeFlags( loop );

		AIMOD_MAPPING_CreateNodeLinks( loop );

		strcpy( last_node_added_string, va("^5Created ^3%i ^5links for waypoint ^7%i^5.", nodes[loop].enodenum, loop) );
	}

	aw_percent_complete = 0.0f;
	strcpy( last_node_added_string, va(" ") );
	aw_num_nodes = number_of_nodes;
	trap_UpdateScreen();
}

/* */
__device__ __host__ void AIMOD_NODES_SetObjectiveFlags ( int node )
{										// Find objects near this node.
	// Init them first...
	nodes[node].objectNum[0] = nodes[node].objectNum[1] = nodes[node].objectNum[2] = ENTITYNUM_NONE;
	nodes[node].objEntity = -1;
	nodes[node].objFlags = -1;
	nodes[node].objTeam = -1;
	nodes[node].objType = -1;
}

/*////////////////////////////////////////////////
ConnectNodes
Connects 2 nodes and sets the flags for the path between them
/*/


///////////////////////////////////////////////
__device__ __host__ qboolean ConnectNodes ( int from, int to, int flags )
{

	//check that we don't have too many connections from the 'from' node already
	if ( nodes[from].enodenum + 1 > MAX_NODELINKS )
	{
		return ( qfalse );
	}

	//check that we are creating a path between 2 valid nodes
	if ( (nodes[from].type == NODE_INVALID) || (to == NODE_INVALID) || from > MAX_NODES || from < 0 || to > MAX_NODES || to < 0)
	{	//nodes[to].type is invalid on LoadNodes()
		return ( qfalse );
	}

	//update the individual nodes
	nodes[from].links[nodes[from].enodenum].targetNode = to;
	nodes[from].links[nodes[from].enodenum].flags = flags;
	nodes[from].enodenum++;
	return ( qtrue );
}
#else //!__CUDA__
/* */
void
AIMOD_MAPPING_MakeLinks ( void )
{
	int		loop = 0;
//	node_t	*good_nodes;
//	int		upto = 0;
//	int		total_good_count = 0;
//	int		bad_nodes = 0;
	int		final_tests = 0;
	int		update_timer = 0;

#ifdef __BOT_AUTOWAYPOINT_OPTIMIZE__
	good_nodes = B_Alloc( (sizeof(node_t)+1)*MAX_NODES );
#endif //__BOT_AUTOWAYPOINT_OPTIMIZE__

	if ( aw_num_nodes > 0 )
	{
		number_of_nodes = aw_num_nodes;
	}

	aw_percent_complete = 0.0f;
 	strcpy( last_node_added_string, va(" ") );

	CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^5Creating waypoint linkages and flags for ^3%i^5 waypoints...\n", number_of_nodes );
	strcpy( task_string3, va("^5Creating waypoint linkages and flags for ^3%i^5 waypoints...", number_of_nodes) );
	trap_UpdateScreen();

	final_tests = 0;
	update_timer = 0;
	aw_percent_complete = 0.0f;
	trap_UpdateScreen();

	//number_of_nodes = total_good_count;
	
//#pragma omp parallel for
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{// Do links...
		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)loop/(float)number_of_nodes)*100.0f);

		nodes[loop].enodenum = 0;

		update_timer++;

		if (update_timer >= 100)
		{
			trap_UpdateScreen();
			update_timer = 0;
		}

		// Also check if the node needs special flags...
		AIMOD_MAPPING_CreateSpecialNodeFlags( loop );

		AIMOD_MAPPING_CreateNodeLinks( loop );

		strcpy( last_node_added_string, va("^5Created ^3%i ^5links for waypoint ^7%i^5.", nodes[loop].enodenum, loop) );
	}

	aw_percent_complete = 0.0f;
	strcpy( last_node_added_string, va(" ") );
	aw_num_nodes = number_of_nodes;
	trap_UpdateScreen();
}

/* */
void
AIMOD_NODES_SetObjectiveFlags ( int node )
{										// Find objects near this node.
	// Init them first...
	nodes[node].objectNum[0] = nodes[node].objectNum[1] = nodes[node].objectNum[2] = ENTITYNUM_NONE;
	nodes[node].objEntity = -1;
	nodes[node].objFlags = -1;
	nodes[node].objTeam = -1;
	nodes[node].objType = -1;
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
	if ( (nodes[from].type == NODE_INVALID) || (to == NODE_INVALID) || from > MAX_NODES || from < 0 || to > MAX_NODES || to < 0)
	{	//nodes[to].type is invalid on LoadNodes()
		return ( qfalse );
	}

	//update the individual nodes
	nodes[from].links[nodes[from].enodenum].targetNode = to;
	nodes[from].links[nodes[from].enodenum].flags = flags;
	nodes[from].enodenum++;
	return ( qtrue );
}
#endif //__CUDA__

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
	if ( number_of_nodes + 1 > MAX_NODES )
	{
		return ( qfalse );
	}

	VectorCopy( origin, nodes[number_of_nodes].origin );	//set the node's position

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
		nodes[number_of_nodes].objTeam |= TEAM_ALLIES;
		numAlliedOnlyNodes++;
	}

	if ( nodes[number_of_nodes].type & NODE_ALLY_UNREACHABLE )
	{
		if ( AxisOnlyFirstNode < 0 )
		{
			AxisOnlyFirstNode = number_of_nodes;
		}
		nodes[number_of_nodes].objTeam |= TEAM_AXIS;
		numAxisOnlyNodes++;
	}

	number_of_nodes++;
	return ( qtrue );
}

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
	short int		temp, fix_aas_nodes;

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
	if (cgs.gametype == GT_SUPREMACY || cgs.gametype == GT_SUPREMACY_CAMPAIGN)
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
						//CG_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
						//CG_Printf( "^1*** ^3       ^5  You need to make bot routes for this map.\n" );
						//CG_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n" );
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
				//CG_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
				//CG_Printf( "^1*** ^3       ^5  You need to make bot routes for this map.\n" );
				//CG_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n" );
				AIMOD_NODES_LoadNodes2();
				return;
			}
		}
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_ROM | CVAR_SERVERINFO );	//get the map name
	Q_strncpyz( mp, mapname.string, 64 );
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
		CG_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
		CG_Printf( "^1*** ^3       ^5  Old node file detected.\n" );
		//trap_FS_FCloseFile( f );
		fclose(f);
		return;
	}

	//trap_FS_Read( &map, strlen( mp) + 1, f );			//make sure the file is for the current map
	fread( &map, strlen( mp) + 1, 1, f);
	if ( Q_stricmp( map, mp) != 0 )
	{
		CG_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
		CG_Printf( "^1*** ^3       ^5  Node file is not for this map!\n" );
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
		Load_AddNode( vec, flags, objNum, objFlags );	//add the node

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
	CG_Printf( "^1*** ^3%s^5: Successfully loaded %i waypoints from waypoint file ^7nodes/%s.bwp^5.\n", GAME_VERSION,
			  number_of_nodes, filename );

	nodes_loaded = qtrue;

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
	short int		temp, fix_aas_nodes;

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
	if (cgs.gametype == GT_SUPREMACY || cgs.gametype == GT_SUPREMACY_CAMPAIGN)
	{
		trap_FS_FOpenFile( va( "nodes/%s_supremacy.bwp", filename), &f, FS_READ );
		if ( !f )
		{
			trap_FS_FOpenFile( va( "nodes/%s.bwp", filename), &f, FS_READ );
			if ( !f )
			{
				CG_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
				CG_Printf( "^1*** ^3       ^5  You need to make bot routes for this map.\n", filename );
				CG_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n", filename );
				//AIMOD_NODES_LoadNodes2();
				return;
			}
		}
	}
	else
	{
		trap_FS_FOpenFile( va( "nodes/%s.bwp", filename), &f, FS_READ );
		if ( !f )
		{
			CG_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
			CG_Printf( "^1*** ^3       ^5  You need to make bot routes for this map.\n", filename );
			CG_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n", filename );
			//AIMOD_NODES_LoadNodes2();
			return;
		}
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_ROM | CVAR_SERVERINFO );	//get the map name
	Q_strncpyz( mp, mapname.string, 64 );
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
		CG_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
		CG_Printf( "^1*** ^3       ^5  Old node file detected.\n" );
		trap_FS_FCloseFile( f );
		return;
	}

	trap_FS_Read( &map, strlen( mp) + 1, f );			//make sure the file is for the current map
	if ( Q_stricmp( map, mp) != 0 )
	{
		CG_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!!\n", filename );
		CG_Printf( "^1*** ^3       ^5  Node file is not for this map!\n" );
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
		Load_AddNode( vec, flags, objNum, objFlags );	//add the node

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
	CG_Printf( "^1*** ^3%s^5: Successfully loaded %i waypoints from waypoint file ^7nodes/%s.bwp^5.\n", GAME_VERSION,
			  number_of_nodes, filename );
	nodes_loaded = qtrue;

	//number_of_nodes = numberNodes;
	return;
}

/* */
void
AIMOD_NODES_SaveNodes_Autowaypointed ( void )
{
	fileHandle_t	f;
	int				i;
	/*short*/ int		j;
	float			version = NOD_VERSION;										//version is 1.0 for now
	char			name[] = BOT_MOD_NAME;
	vmCvar_t		mapname;
	char			map[64] = "";
	char			filename[60];
	/*short*/ int		num_nodes = number_of_nodes;
	
	aw_num_nodes = number_of_nodes;
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
	trap_FS_FOpenFile( va( "nodes/%s.bwp", filename), &f, FS_WRITE );
	if ( !f )
	{
		CG_Printf( "^1*** ^3ERROR^5: Error opening node file ^7nodes/%s.bwp^5!!!\n", filename );
		return;
	}

//#pragma omp parallel for
	for ( i = 0; i < aw_num_nodes/*num_nodes*/; i++ )
	{
//#pragma omp parallel for
		for ( j = 0; j < MAX_NODELINKS; j++ )
		{
			nodes[i].links[j].targetNode = INVALID;
			nodes[i].links[j].cost = 999999;
			nodes[i].links[j].flags = 0;
			nodes[i].objectNum[0] = nodes[i].objectNum[1] = nodes[i].objectNum[2] = ENTITYNUM_NONE;
		}
	}

//	RemoveDoorsAndDestroyablesForSave();
//	num_nodes = number_of_nodes;

	// Resolve paths
	//-------------------------------------------------------
	AIMOD_MAPPING_MakeLinks();

	num_nodes = aw_num_nodes;

//#pragma omp parallel for
	for ( i = 0; i < aw_num_nodes/*num_nodes*/; i++ )
	{
		// Set node objective flags..
		AIMOD_NODES_SetObjectiveFlags( i );
	}
	
	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );	//get the map name
	Q_strncpyz( map, mapname.string, 64 );
	trap_FS_Write( &name, strlen( name) + 1, f );								//write the mod name to the file
	trap_FS_Write( &version, sizeof(float), f );								//write the version of this file
	trap_FS_Write( &map, strlen( map) + 1, f );									//write the map name
	trap_FS_Write( &num_nodes, sizeof(/*short*/ int), f );							//write the number of nodes in the map
	
	/*if ( nodes[num_nodes].type & NODE_AXIS_UNREACHABLE )
	{
		nodes[num_nodes].objTeam |= TEAM_ALLIES;
	}
	else if ( nodes[num_nodes].type & NODE_ALLY_UNREACHABLE )
	{
		nodes[num_nodes].objTeam |= TEAM_AXIS;
	}*/

	for ( i = 0; i < aw_num_nodes/*num_nodes*/; i++ )											//loop through all the nodes
	{
		//write all the node data to the file
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
	{
		//short int	fix = 1;
		short int	fix = 0;
		trap_FS_Write( &fix, sizeof(short int), f );
	}

	trap_FS_FCloseFile( f );													//close the file
	CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^5Successfully saved node file ^7nodes/%s.bwp^5.\n", filename );
}

//
// The utilities for faster (then vector/float) integer maths...
//
typedef long int	intvec_t;
typedef intvec_t	intvec3_t[3];

#ifdef __CUDA__
/* */
__device__ __host__ void intToVectorCopy ( const intvec3_t in, vec3_t out )
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}


/* */
__device__ __host__ void intVectorCopy ( const intvec3_t in, intvec3_t out )
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}


/* */
__device__ __host__ intvec_t intVectorLength ( const intvec3_t v )
{
	return ( sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2]) );
}


/* */
__device__ __host__ void intVectorSubtract ( const intvec3_t veca, const intvec3_t vecb, intvec3_t out )
{
	out[0] = veca[0] - vecb[0];
	out[1] = veca[1] - vecb[1];
	out[2] = veca[2] - vecb[2];
}


/* */
__device__ __host__ long int intVectorDistance ( intvec3_t v1, intvec3_t v2 )
{
	__shared__ intvec3_t	dir;
	intVectorSubtract( v2, v1, dir );
	return ( intVectorLength( dir) );
}
#else //!__CUDA__
/* */
void
intToVectorCopy ( const intvec3_t in, vec3_t out )
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}


/* */
void
intVectorCopy ( const intvec3_t in, intvec3_t out )
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}


/* */
intvec_t
intVectorLength ( const intvec3_t v )
{
	return ( sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2]) );
}


/* */
void
intVectorSubtract ( const intvec3_t veca, const intvec3_t vecb, intvec3_t out )
{
	out[0] = veca[0] - vecb[0];
	out[1] = veca[1] - vecb[1];
	out[2] = veca[2] - vecb[2];
}


/* */
long int
intVectorDistance ( intvec3_t v1, intvec3_t v2 )
{
	intvec3_t	dir;
	intVectorSubtract( v2, v1, dir );
	return ( intVectorLength( dir) );
}
#endif //__CUDA__

//
// Now the actual number crunching and visualizations...

extern float BG_GetGroundHeightAtPoint( vec3_t pos );
extern qboolean BG_TraceMapLoaded ( void );
extern void CG_GenerateTracemap( void );

#ifdef __CUDA__
__device__ __host__ qboolean Waypoint_FloorSurfaceOK ( int surfaceFlags )
{
	if (surfaceFlags == 0)
		return qtrue;

	if (surfaceFlags & SURF_METAL)
		return qtrue;

	if (surfaceFlags & SURF_WOOD)
		return qtrue;

	if (surfaceFlags & SURF_GRASS)
		return qtrue;

	if (surfaceFlags & SURF_GRAVEL)
		return qtrue;

	if (surfaceFlags & SURF_GLASS)
		return qtrue;

	if (surfaceFlags & SURF_SNOW)
		return qtrue;

	if (surfaceFlags & SURF_RUBBLE)
		return qtrue;
	
	if (surfaceFlags & SURF_CARPET)
		return qtrue;

	if (surfaceFlags & SURF_LANDMINE)
		return qtrue;

	if (surfaceFlags & SURF_ROOF)
		return qtrue;
	
	return qfalse;	
}
#else //!__CUDA__
qboolean Waypoint_FloorSurfaceOK ( int surfaceFlags )
{
	if (surfaceFlags == 0)
		return qtrue;

	if (surfaceFlags & SURF_METAL)
		return qtrue;

	if (surfaceFlags & SURF_WOOD)
		return qtrue;

	if (surfaceFlags & SURF_GRASS)
		return qtrue;

	if (surfaceFlags & SURF_GRAVEL)
		return qtrue;

	if (surfaceFlags & SURF_GLASS)
		return qtrue;

	if (surfaceFlags & SURF_SNOW)
		return qtrue;

	if (surfaceFlags & SURF_RUBBLE)
		return qtrue;
	
	if (surfaceFlags & SURF_CARPET)
		return qtrue;

	if (surfaceFlags & SURF_LANDMINE)
		return qtrue;

	if (surfaceFlags & SURF_ROOF)
		return qtrue;
	
	return qfalse;	
}
#endif //__CUDA__

float GroundHeightAt ( vec3_t org )
{
	trace_t tr;
	vec3_t org1, org2;
//	float height = 0;

	VectorCopy(org, org1);
	org1[2]+=48;

	VectorCopy(org, org2);
	org2[2]= -65536.0f;

	CG_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_PLAYERSOLID);//CONTENTS_PLAYERCLIP | MASK_SHOT /*| MASK_OPAQUE*/ | MASK_WATER );
	
	if ( tr.startsolid || tr.allsolid )
	{
		return -65536.0f;
	}

	if ( tr.surfaceFlags & SURF_SKY )
	{// Sky...
		return -65536.0f;
	}

	if ( (tr.surfaceFlags & SURF_NODRAW) && (tr.surfaceFlags & SURF_NOMARKS) && !Waypoint_FloorSurfaceOK(tr.surfaceFlags) )
	{// Sky...
		return -65536.0f;
	}

	return tr.endpos[2];
}

#ifdef __OLD__
float FloorHeightAt ( vec3_t org )
{
	trace_t tr;
	vec3_t org1, org2;
//	float height = 0;

	VectorCopy(org, org1);
	org1[2]+=48;

	VectorCopy(org, org2);
	org2[2]= -65536.0f;

	CG_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_PLAYERSOLID );//CONTENTS_PLAYERCLIP | MASK_SHOT /*| MASK_OPAQUE*/ | MASK_WATER );
	
	if ( tr.surfaceFlags & SURF_SKY )
	{// Sky...
		return 65536.0f;
	}

	if ( (tr.surfaceFlags & SURF_NODRAW) && (tr.surfaceFlags & SURF_NOMARKS) && !Waypoint_FloorSurfaceOK(tr.surfaceFlags) )
	{// Sky...
		return 65536.0f;
	}

	if ( (tr.surfaceFlags & SURF_NONSOLID) 
		&& (tr.surfaceFlags & SURF_NOMARKS)
		&& (tr.surfaceFlags & SURF_NOLIGHTMAP) )
	{// Water...
		return tr.endpos[2];
	}

	if ( tr.startsolid || tr.allsolid /*|| tr.contents & CONTENTS_WATER*/ )
	{
		return 65536.0f;
	}

	return tr.endpos[2];
}
#endif //__OLD__

float FloorNormalAt ( vec3_t org )
{// UQ1: improved it again, now checks slope here in a faster way that also stops multiple checks later! :) Also add mover check :)
	trace_t tr;
	vec3_t org1, org2, slopeangles;
	float pitch = 0;

	VectorCopy(org, org1);
	org1[2]+=48;

	VectorCopy(org, org2);
	org2[2]= -65536.0f;

	CG_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_PLAYERSOLID );

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

qboolean aw_floor_trace_hit_mover = qfalse;

float FloorHeightAt ( vec3_t org )
{// UQ1: improved it again, now checks slope here in a faster way that also stops multiple checks later! :) Also add mover check :)
	trace_t tr;
	vec3_t org1, org2, slopeangles;
	float pitch = 0;

	aw_floor_trace_hit_mover = qfalse;

	VectorCopy(org, org1);
	org1[2]+=48;

	VectorCopy(org, org2);
	org2[2]= -65536.0f;

	CG_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_PLAYERSOLID );//CONTENTS_PLAYERCLIP | MASK_SHOT /*| MASK_OPAQUE*/ | MASK_WATER );
	
	if (tr.endpos[2] < -65000)
		return -65536.0f;

	if ( tr.fraction != 1 
		&& tr.entityNum != ENTITYNUM_NONE 
		&& tr.entityNum < ENTITYNUM_MAX_NORMAL )
	{
		if (cg_entities[tr.entityNum].currentState.eType == ET_MOVER)
		{// Hit a mover... Add waypoints at all of them!
			aw_floor_trace_hit_mover = qtrue;
			return tr.endpos[2];
		}
	}

	if ( tr.surfaceFlags & SURF_SKY )
	{// Sky...
		return 65536.0f;
	}

	if ( (tr.surfaceFlags & SURF_NODRAW) && (tr.surfaceFlags & SURF_NOMARKS) && !Waypoint_FloorSurfaceOK(tr.surfaceFlags) )
	{// Sky...
		return 65536.0f;
	}

	// Added -- Check slope...
	vectoangles( tr.plane.normal, slopeangles );

	pitch = slopeangles[0];
	
	if (pitch > 180)
		pitch -= 360;

	if (pitch < -180)
		pitch += 360;

	pitch += 90.0f;

	if (pitch > 46.0f || pitch < -46.0f)
		return 65536.0f; // bad slope...

	if ( (tr.surfaceFlags & SURF_NONSOLID) 
		&& (tr.surfaceFlags & SURF_NOMARKS)
		&& (tr.surfaceFlags & SURF_NOLIGHTMAP) )
	{// Water...
		return tr.endpos[2];
	}

	if ( tr.startsolid || tr.allsolid /*|| tr.contents & CONTENTS_WATER*/ )
	{
		return 65536.0f;
	}

	return tr.endpos[2];
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

	CG_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_PLAYERSOLID);//CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA | MASK_WATER );
	
	if ( tr.startsolid || tr.allsolid )
	{
		return -65536.0f;
	}

	return tr.endpos[2];
}

#define	SURF_NODAMAGE			0x00000001	// never give falling damage
#define	SURF_SLICK				0x00000002	// effects game physics
#define	SURF_SKY				0x00000004	// lighting from environment map
#define	SURF_LADDER				0x00000008
#define	SURF_NOIMPACT			0x00000010	// don't make missile explosions
#define	SURF_NOMARKS			0x00000020	// don't leave missile marks
#define SURF_SPLASH				0x00000040	// out of surf's, so replacing unused 'SURF_FLESH' - and as SURF_CERAMIC wasn't used, it's now SURF_SPLASH
#define	SURF_NODRAW				0x00000080	// don't generate a drawsurface at all
#define	SURF_HINT				0x00000100	// make a primary bsp splitter
#define	SURF_SKIP				0x00000200	// completely ignore, allowing non-closed brushes
#define	SURF_NOLIGHTMAP			0x00000400	// surface doesn't need a lightmap
#define	SURF_POINTLIGHT			0x00000800	// generate lighting info at vertexes
#define	SURF_METAL  			0x00001000	// clanking footsteps
#define	SURF_NOSTEPS			0x00002000	// no footstep sounds
#define	SURF_NONSOLID			0x00004000	// don't collide against curves with this set
#define SURF_LIGHTFILTER		0x00008000	// act as a light filter during q3map -light
#define	SURF_ALPHASHADOW		0x00010000	// do per-pixel light shadow casting in q3map
#define	SURF_NODLIGHT			0x00020000	// don't dlight even if solid (solid lava, skies)
#define	SURF_WOOD				0x00040000
#define	SURF_GRASS				0x00080000
#define SURF_GRAVEL				0x00100000
#define SURF_GLASS				0x00200000	// out of surf's, so replacing unused 'SURF_SMGROUP'
#define SURF_SNOW				0x00400000
#define SURF_ROOF				0x00800000
#define	SURF_RUBBLE				0x01000000
#define	SURF_CARPET				0x02000000
#define SURF_MONSTERSLICK		0x04000000	// slick surf that only affects ai's
#define SURF_MONSLICK_W			0x08000000
#define SURF_MONSLICK_N			0x10000000
#define SURF_MONSLICK_E			0x20000000
#define SURF_MONSLICK_S			0x40000000
#define SURF_LANDMINE			0x80000000	// ydnar: ok to place landmines on this surface

#define	CONTENTS_SOLID				0x00000001
#define	CONTENTS_LIGHTGRID			0x00000004
#define	CONTENTS_LAVA				0x00000008
#define	CONTENTS_SLIME				0x00000010
#define	CONTENTS_WATER				0x00000020
#define	CONTENTS_FOG				0x00000040
#define	CONTENTS_MISSILECLIP		0x00000080
#define CONTENTS_ITEM				0x00000100
#define CONTENTS_MOVER				0x00004000
#define	CONTENTS_AREAPORTAL			0x00008000
#define	CONTENTS_PLAYERCLIP			0x00010000
#define	CONTENTS_MONSTERCLIP		0x00020000
#define	CONTENTS_TELEPORTER			0x00040000
#define	CONTENTS_JUMPPAD			0x00080000
#define CONTENTS_CLUSTERPORTAL		0x00100000
#define CONTENTS_DONOTENTER			0x00200000
#define CONTENTS_DONOTENTER_LARGE	0x00400000
#define	CONTENTS_ORIGIN				0x01000000	// removed before bsping an entity
#define	CONTENTS_BODY				0x02000000	// should never be on a brush, only in game
#define	CONTENTS_CORPSE				0x04000000
#define	CONTENTS_DETAIL				0x08000000	// brushes not used for the bsp

#define	CONTENTS_STRUCTURAL		0x10000000	// brushes used for the bsp
#define	CONTENTS_TRANSLUCENT	0x20000000	// don't consume surface fragments inside
#define	CONTENTS_TRIGGER		0x40000000
#define	CONTENTS_NODROP			0x80000000	// don't leave bodies or items (death fog, lava)

void CG_ShowSurface ( void )
{
	vec3_t org, down_org;
	trace_t tr;

	VectorCopy(cg_entities[cg.clientNum].lerpOrigin, org);
	VectorCopy(cg_entities[cg.clientNum].lerpOrigin, down_org);
	down_org[2]-=48;

	// Do forward test...
	CG_Trace( &tr, org, NULL, NULL, down_org, -1, MASK_ALL );

	//
	// Surface
	//

	CG_Printf("Current surface flags (%i):\n", tr.surfaceFlags);

	if (tr.surfaceFlags & SURF_NODAMAGE)
		CG_Printf("SURF_NODAMAGE ");

	if (tr.surfaceFlags & SURF_SLICK)
		CG_Printf("SURF_SLICK ");

	if (tr.surfaceFlags & SURF_SKY)
		CG_Printf("SURF_SKY ");

	if (tr.surfaceFlags & SURF_LADDER)
		CG_Printf("SURF_LADDER ");

	if (tr.surfaceFlags & SURF_NOIMPACT)
		CG_Printf("SURF_NOIMPACT ");

	if (tr.surfaceFlags & SURF_NOMARKS)
		CG_Printf("SURF_NOMARKS ");

	if (tr.surfaceFlags & SURF_SPLASH)
		CG_Printf("SURF_SPLASH ");

	if (tr.surfaceFlags & SURF_NODRAW)
		CG_Printf("SURF_NODRAW ");

	if (tr.surfaceFlags & SURF_HINT)
		CG_Printf("SURF_HINT ");

	if (tr.surfaceFlags & SURF_SKIP)
		CG_Printf("SURF_SKIP ");

	if (tr.surfaceFlags & SURF_NOLIGHTMAP)
		CG_Printf("SURF_NOLIGHTMAP ");

	if (tr.surfaceFlags & SURF_METAL)
		CG_Printf("SURF_METAL ");

	if (tr.surfaceFlags & SURF_NOSTEPS)
		CG_Printf("SURF_NOSTEPS ");

	if (tr.surfaceFlags & SURF_NONSOLID)
		CG_Printf("SURF_NONSOLID ");

	if (tr.surfaceFlags & SURF_LIGHTFILTER)
		CG_Printf("SURF_LIGHTFILTER ");

	if (tr.surfaceFlags & SURF_ALPHASHADOW)
		CG_Printf("SURF_ALPHASHADOW ");

	if (tr.surfaceFlags & SURF_NODLIGHT)
		CG_Printf("SURF_NODLIGHT ");

	if (tr.surfaceFlags & SURF_WOOD)
		CG_Printf("SURF_WOOD ");

	if (tr.surfaceFlags & SURF_GRASS)
		CG_Printf("SURF_GRASS ");

	if (tr.surfaceFlags & SURF_GRAVEL)
		CG_Printf("SURF_GRAVEL ");

	if (tr.surfaceFlags & SURF_GLASS)
		CG_Printf("SURF_GLASS ");

	if (tr.surfaceFlags & SURF_SNOW)
		CG_Printf("SURF_SNOW ");

	if (tr.surfaceFlags & SURF_ROOF)
		CG_Printf("SURF_ROOF ");

	if (tr.surfaceFlags & SURF_RUBBLE)
		CG_Printf("SURF_RUBBLE ");

	if (tr.surfaceFlags & SURF_CARPET)
		CG_Printf("SURF_CARPET ");

	if (tr.surfaceFlags & SURF_MONSTERSLICK)
		CG_Printf("SURF_MONSTERSLICK ");

	if (tr.surfaceFlags & SURF_MONSLICK_W)
		CG_Printf("SURF_MONSLICK_W ");

	if (tr.surfaceFlags & SURF_MONSLICK_N)
		CG_Printf("SURF_MONSLICK_N ");

	if (tr.surfaceFlags & SURF_MONSLICK_E)
		CG_Printf("SURF_MONSLICK_E ");

	if (tr.surfaceFlags & SURF_MONSLICK_S)
		CG_Printf("SURF_MONSLICK_S ");

	if (tr.surfaceFlags & SURF_LANDMINE)
		CG_Printf("SURF_LANDMINE ");

	CG_Printf("\n");

	//
	// Contents...
	//

	CG_Printf("Current contents flags (%i):\n", tr.surfaceFlags);

	if (tr.contents & CONTENTS_SOLID)
		CG_Printf("CONTENTS_SOLID ");

	if (tr.contents & CONTENTS_LIGHTGRID)
		CG_Printf("CONTENTS_LIGHTGRID ");

	if (tr.contents & CONTENTS_LAVA)
		CG_Printf("CONTENTS_LAVA ");

	if (tr.contents & CONTENTS_SLIME)
		CG_Printf("CONTENTS_SLIME ");

	if (tr.contents & CONTENTS_WATER)
		CG_Printf("CONTENTS_WATER ");

	if (tr.contents & CONTENTS_FOG)
		CG_Printf("CONTENTS_FOG ");

	if (tr.contents & CONTENTS_MISSILECLIP)
		CG_Printf("CONTENTS_MISSILECLIP ");

	if (tr.contents & CONTENTS_ITEM)
		CG_Printf("CONTENTS_ITEM ");

	if (tr.contents & CONTENTS_MOVER)
		CG_Printf("CONTENTS_MOVER ");

	if (tr.contents & CONTENTS_AREAPORTAL)
		CG_Printf("CONTENTS_AREAPORTAL ");

	if (tr.contents & CONTENTS_PLAYERCLIP)
		CG_Printf("CONTENTS_PLAYERCLIP ");

	if (tr.contents & CONTENTS_MONSTERCLIP)
		CG_Printf("CONTENTS_MONSTERCLIP ");

	if (tr.contents & CONTENTS_TELEPORTER)
		CG_Printf("CONTENTS_TELEPORTER ");

	if (tr.contents & CONTENTS_JUMPPAD)
		CG_Printf("CONTENTS_JUMPPAD ");

	if (tr.contents & CONTENTS_CLUSTERPORTAL)
		CG_Printf("CONTENTS_CLUSTERPORTAL ");

	if (tr.contents & CONTENTS_DONOTENTER)
		CG_Printf("CONTENTS_DONOTENTER ");

	if (tr.contents & CONTENTS_DONOTENTER_LARGE)
		CG_Printf("CONTENTS_DONOTENTER_LARGE ");

	if (tr.contents & CONTENTS_ORIGIN)
		CG_Printf("CONTENTS_ORIGIN ");

	if (tr.contents & CONTENTS_BODY)
		CG_Printf("CONTENTS_BODY ");

	if (tr.contents & CONTENTS_CORPSE)
		CG_Printf("CONTENTS_CORPSE ");

	if (tr.contents & CONTENTS_DETAIL)
		CG_Printf("CONTENTS_DETAIL ");

	if (tr.contents & CONTENTS_STRUCTURAL)
		CG_Printf("CONTENTS_STRUCTURAL ");

	if (tr.contents & CONTENTS_TRANSLUCENT)
		CG_Printf("CONTENTS_TRANSLUCENT ");

	if (tr.contents & CONTENTS_TRIGGER)
		CG_Printf("CONTENTS_TRIGGER ");

	if (tr.contents & CONTENTS_NODROP)
		CG_Printf("CONTENTS_NODROP ");

	CG_Printf("\n");
}

#define POS_FW 0
#define POS_BC 1
#define POS_L 2
#define POS_R 3
#define POS_MAX 4

/*
	 _____
	|  0  |
    |     |
    |2   3|
	|     |
	|__1__|
*/

#ifdef __OLD__
void CG_ShowSlope ( void ) {  
	int i;
	vec3_t	org;
	trace_t	 trace;
	vec3_t	forward, right, up, start, end;
	vec3_t	dir, angles;
	vec3_t	testpos[POS_MAX];
	vec3_t	testangles;
	vec3_t	boxMins= {-8, -8, -8}; // @fixme , tune this to be more smooth on delailed terrian (eg. railroad )
	vec3_t	boxMaxs= {8, 8, 8};
	float	pitch, roll, yaw, roof;
	vec3_t	slopeangles;

	VectorCopy(cg_entities[cg.clientNum].lerpOrigin, org);

	testangles[0] = testangles[1] = testangles[2] = 0;

	AngleVectors( testangles, forward, right, up );

	roof = RoofHeightAt( org );
	roof -= 16;

//#pragma omp parallel for
	for ( i=0; i < POS_MAX; i++ ){
		float fw, rt;

		fw = 32;//64;
		rt = 32;//48;

		VectorCopy(org, start);
			
		switch (i){
		case POS_FW:
		VectorMA ( start, fw , forward , start);
			break;
		case POS_BC:
		VectorMA ( start, -fw , forward , start);
			break;
		case POS_L:
		VectorMA ( start, -rt , right , start);
			break;
		case POS_R:
		VectorMA ( start, rt , right , start);
			break;
		}

		VectorMA ( start, -8000 , up , end);
		//VectorMA ( start, 96/*16*/ , up , start); // raise the start pos
		start[2] = roof;

		trap_CM_BoxTrace( &trace, start, end, boxMins, boxMaxs,  0, MASK_PLAYERSOLID );

		VectorCopy(trace.endpos, testpos[i]);
	}
		
	VectorSubtract(testpos[0], testpos[1], dir);
	vectoangles (dir, angles);

	pitch=angles[0];
	if (pitch > 180)
		pitch -= 360;

	if (pitch < -180)
		pitch += 360;

	VectorSubtract(testpos[3], testpos[2], dir);
	vectoangles (dir, angles);

	roll=angles[0];
	if (roll > 180)
		roll -= 360;

	if (roll < -180)
		roll += 360;

	VectorSubtract(testpos[1], testpos[2], dir);
	vectoangles (dir, angles);

	yaw=angles[0];
	if (yaw > 180)
		yaw -= 360;

	if (yaw < -180)
		yaw += 360;

	slopeangles[0]=pitch;
	slopeangles[1]=yaw;//0;
	slopeangles[2]=roll;

	CG_Printf("Slope is %f %f %f\n", slopeangles[0], slopeangles[1], slopeangles[2]);
}
#endif //__OLD__

void CG_ShowSlope ( void ) {  
	vec3_t	org;
	trace_t	 trace;
	vec3_t	forward, right, up, start, end;
	vec3_t	testangles;
//	vec3_t	boxMins= {-1, -1, -1};
//	vec3_t	boxMaxs= {1, 1, 1};
	float	pitch, roll, yaw, roof;
	vec3_t	slopeangles;

	VectorCopy(cg_entities[cg.clientNum].lerpOrigin, org);

	testangles[0] = testangles[1] = testangles[2] = 0;

	AngleVectors( testangles, forward, right, up );

	roof = RoofHeightAt( org );
	roof -= 16;

	VectorCopy(org, start);
	VectorMA ( start, -65000 , up , end);
	start[2] = roof;

	CG_Trace( &trace, start, NULL, NULL, end, cg.clientNum, MASK_PLAYERSOLID );

	vectoangles( trace.plane.normal, slopeangles );

	pitch = slopeangles[0];
	if (pitch > 180)
		pitch -= 360;

	if (pitch < -180)
		pitch += 360;

	pitch += 90.0f;

	yaw = slopeangles[1];
	if (yaw > 180)
		yaw -= 360;

	if (yaw < -180)
		yaw += 360;

	roll = slopeangles[2];
	if (roll > 180)
		roll -= 360;

	if (roll < -180)
		roll += 360;

	CG_Printf("Slope is %f %f %f\n", pitch, roll, yaw);
}

#ifdef __OLD__
qboolean AIMod_AutoWaypoint_Check_Stepps ( vec3_t org )
{// return qtrue if NOT stepps...
	trace_t		trace;
	vec3_t		testangles, forward, right, up, start, end;
	float		roof = 0.0f, last_height = 0.0f, last_diff = 0.0f, this_diff = 0.0f;
	int			i = 0, num_same_diffs = 0, num_zero_diffs = 0;

	testangles[0] = testangles[1] = testangles[2] = 0;
	AngleVectors( testangles, forward, right, up );

	roof = RoofHeightAt( org );
	roof -= 16;

	last_height = FloorHeightAt( org );

	for (i = 0; i < 64; i+=4)
	{
		VectorCopy(org, start);
		VectorMA ( start, i , forward , start);
		VectorMA ( start, -8000 , up , end);
		start[2] = roof;//RoofHeightAt( start );
		//start[2] -= 16;

		trap_CM_BoxTrace( &trace, start, end, NULL, NULL,  0, MASK_PLAYERSOLID );

		if (num_zero_diffs > 4 && num_same_diffs <= 0)
		{// Flat in this direction. Skip!
			num_same_diffs = 0;
			num_zero_diffs = 0;
			break;
		}

		if (trace.endpos[2] == last_height)
		{
			num_zero_diffs++;
			continue;
		}

		if (last_height > trace.endpos[2])
			this_diff = last_height - trace.endpos[2];
		else
			this_diff = trace.endpos[2] - last_height;

		if (this_diff > 24)
		{
			num_same_diffs = 0;
			num_zero_diffs = 0;
			break;
		}

		if (last_diff == this_diff)
			num_same_diffs++;

		last_diff = this_diff;
		last_height = trace.endpos[2];

		if (num_same_diffs > 1 && num_zero_diffs <= 0)
		{// Save thinking.. It's obviously a slope!
			num_same_diffs = 0;
			num_zero_diffs = 0;
			return qtrue;
		}

		if (num_same_diffs > 1 && num_zero_diffs > 1)
			break; // We seem to have found stepps...
	}

	if (num_same_diffs > 1 && num_zero_diffs > 1)
		return qfalse; // We seem to have found stepps...

	num_same_diffs = 0;
	num_zero_diffs = 0;
	last_height = FloorHeightAt( org );
	last_diff = 0;
	this_diff = 0;

	for (i = 0; i < 64; i+=4)
	{
		VectorCopy(org, start);
		VectorMA ( start, i , right , start);
		VectorMA ( start, -8000 , up , end);
		start[2] = roof;//RoofHeightAt( start );
		//start[2] -= 16;

		trap_CM_BoxTrace( &trace, start, end, NULL, NULL,  0, MASK_PLAYERSOLID );

		if (num_zero_diffs > 4 && num_same_diffs <= 0)
		{// Flat in this direction. Skip!
			num_same_diffs = 0;
			num_zero_diffs = 0;
			break;
		}

		if (trace.endpos[2] == last_height)
		{
			num_zero_diffs++;
			continue;
		}

		if (last_height > trace.endpos[2])
			this_diff = last_height - trace.endpos[2];
		else
			this_diff = trace.endpos[2] - last_height;

		if (this_diff > 24)
		{
			num_same_diffs = 0;
			num_zero_diffs = 0;
			break;
		}

		if (last_diff == this_diff)
			num_same_diffs++;

		last_diff = this_diff;
		last_height = trace.endpos[2];

		if (num_same_diffs > 1 && num_zero_diffs <= 0)
		{// Save thinking.. It's obviously a slope!
			num_same_diffs = 0;
			num_zero_diffs = 0;
			return qtrue;
		}

		if (num_same_diffs > 1 && num_zero_diffs > 1)
			break; // We seem to have found stepps...
	}

	if (num_same_diffs > 1 && num_zero_diffs > 1)
		return qfalse; // We seem to have found stepps...

	num_same_diffs = 0;
	num_zero_diffs = 0;
	last_height = FloorHeightAt( org );
	last_diff = 0;
	this_diff = 0;

	for (i = 0; i < 64; i+=4)
	{
		VectorCopy(org, start);
		VectorMA ( start, 0-i , forward , start);
		VectorMA ( start, -8000 , up , end);
		start[2] = roof;//RoofHeightAt( start );
		//start[2] -= 16;

		trap_CM_BoxTrace( &trace, start, end, NULL, NULL,  0, MASK_PLAYERSOLID );

		if (num_zero_diffs > 4 && num_same_diffs <= 0)
		{// Flat in this direction. Skip!
			num_same_diffs = 0;
			num_zero_diffs = 0;
			break;
		}

		if (trace.endpos[2] == last_height)
		{
			num_zero_diffs++;
			continue;
		}

		if (last_height > trace.endpos[2])
			this_diff = last_height - trace.endpos[2];
		else
			this_diff = trace.endpos[2] - last_height;

		if (this_diff > 24)
		{
			num_same_diffs = 0;
			num_zero_diffs = 0;
			break;
		}

		if (last_diff == this_diff)
			num_same_diffs++;

		last_diff = this_diff;
		last_height = trace.endpos[2];

		if (num_same_diffs > 1 && num_zero_diffs <= 0)
		{// Save thinking.. It's obviously a slope!
			num_same_diffs = 0;
			num_zero_diffs = 0;
			return qtrue;
		}

		if (num_same_diffs > 1 && num_zero_diffs > 1)
			break; // We seem to have found stepps...
	}

	if (num_same_diffs > 1 && num_zero_diffs > 1)
		return qfalse; // We seem to have found stepps...

	num_same_diffs = 0;
	num_zero_diffs = 0;
	last_height = FloorHeightAt( org );
	last_diff = 0;
	this_diff = 0;

	for (i = 0; i < 64; i+=4)
	{
		VectorCopy(org, start);
		VectorMA ( start, 0-i , right , start);
		VectorMA ( start, -8000 , up , end);
		start[2] = roof;//RoofHeightAt( start );
		//start[2] -= 16;

		trap_CM_BoxTrace( &trace, start, end, NULL, NULL,  0, MASK_PLAYERSOLID );

		if (num_zero_diffs > 4 && num_same_diffs <= 0)
		{// Flat in this direction. Skip!
			num_same_diffs = 0;
			num_zero_diffs = 0;
			break;
		}

		if (trace.endpos[2] == last_height)
		{
			num_zero_diffs++;
			continue;
		}

		if (last_height > trace.endpos[2])
			this_diff = last_height - trace.endpos[2];
		else
			this_diff = trace.endpos[2] - last_height;

		if (this_diff > 24)
		{
			num_same_diffs = 0;
			num_zero_diffs = 0;
			break;
		}

		if (last_diff == this_diff)
			num_same_diffs++;

		last_diff = this_diff;
		last_height = trace.endpos[2];

		if (num_same_diffs > 1 && num_zero_diffs <= 0)
		{// Save thinking.. It's obviously a slope!
			num_same_diffs = 0;
			num_zero_diffs = 0;
			return qtrue;
		}

		if (num_same_diffs > 1 && num_zero_diffs > 1)
			break; // We seem to have found stepps...
	}

	if (num_same_diffs > 1 && num_zero_diffs > 1)
		return qfalse; // We seem to have found stepps...

	return qtrue;
}
#endif //__OLD__

qboolean AIMod_AutoWaypoint_Check_Stepps ( vec3_t org )
{// return qtrue if NOT stepps...
	return qtrue;
}

qboolean AIMod_AutoWaypoint_Check_Slope ( vec3_t org )
{
	return qfalse;
}

#ifdef __OLD__
qboolean AIMod_AutoWaypoint_Check_Slope ( vec3_t org ) 
{  
	int i;
	trace_t	 trace;
	vec3_t	forward, right, up, start, end;
	vec3_t	dir, angles;
	vec3_t	testpos[POS_MAX];
	vec3_t	testangles;
	//vec3_t	boxMins= {-8, -8, -8}; // @fixme , tune this to be more smooth on delailed terrian (eg. railroad )
	//vec3_t	boxMaxs= {8, 8, 8};
	//vec3_t	boxMins= {-4, -4, -4}; // @fixme , tune this to be more smooth on delailed terrian (eg. railroad )
	//vec3_t	boxMaxs= {4, 4, 4};
	vec3_t	boxMins= {-1, -1, -1}; // @fixme , tune this to be more smooth on delailed terrian (eg. railroad )
	vec3_t	boxMaxs= {1, 1, 1};
	float	pitch, roll, yaw, roof;
	vec3_t	slopeangles;

	testangles[0] = testangles[1] = testangles[2] = 0;

	AngleVectors( testangles, forward, right, up );

	roof = RoofHeightAt( org );
	roof -= 16;

//#pragma omp parallel for
	for ( i=0; i < POS_MAX; i++ ){
		float fw, rt;

		fw = 4;//8;//32;//64;
		rt = 4;//8;//32;//48;

		VectorCopy(org, start);
			
		switch (i){
		case POS_FW:
		VectorMA ( start, fw , forward , start);
			break;
		case POS_BC:
		VectorMA ( start, -fw , forward , start);
			break;
		case POS_L:
		VectorMA ( start, -rt , right , start);
			break;
		case POS_R:
		VectorMA ( start, rt , right , start);
			break;
		}

		VectorMA ( start, -8000 , up , end);
		//VectorMA ( start, 96/*16*/ , up , start); // raise the start pos
		start[2] = roof;

		trap_CM_BoxTrace( &trace, start, end, boxMins, boxMaxs,  0, MASK_PLAYERSOLID );

		VectorCopy(trace.endpos, testpos[i]);
	}
		
	VectorSubtract(testpos[0], testpos[1], dir);
	vectoangles (dir, angles);

	pitch=angles[0];
	if (pitch > 180)
		pitch -= 360;

	if (pitch < -180)
		pitch += 360;

	VectorSubtract(testpos[3], testpos[2], dir);
	vectoangles (dir, angles);

	roll=angles[0];
	if (roll > 180)
		roll -= 360;

	if (roll < -180)
		roll += 360;

	VectorSubtract(testpos[1], testpos[2], dir);
	vectoangles (dir, angles);

	yaw=angles[0];
	if (yaw > 180)
		yaw -= 360;

	if (yaw < -180)
		yaw += 360;

	slopeangles[0]=pitch;
	slopeangles[1]=yaw;//0;
	slopeangles[2]=roll;

	if (slopeangles[0] > 40.0f || slopeangles[1] > 40.0f || slopeangles[2] > 40.0f)
		return AIMod_AutoWaypoint_Check_Stepps(org);
	else if (slopeangles[0] < -40.0f || slopeangles[1] < -40.0f || slopeangles[2] < -40.0f)
		return AIMod_AutoWaypoint_Check_Stepps(org);
	else
		return qfalse;
}
#endif //__OLD__

vec3_t fixed_position;

void RepairPosition ( intvec3_t org1 )
{
//	trace_t tr;
	vec3_t	/*newOrg, newOrg2,*/ forward, right, up;
	vec3_t	angles = { 0, 0, 0 };

	AngleVectors(angles, forward, right, up);

	// Init fixed_position
	fixed_position[0] = org1[0];
	fixed_position[1] = org1[1];
	fixed_position[2] = org1[2];

#ifdef __AW_UNUSED__
	// Prepare for forward test...
	VectorCopy( fixed_position, newOrg );
	VectorCopy( fixed_position, newOrg2 );
	VectorMA(newOrg2, 64, forward, newOrg2);

	// Do forward test...
	CG_Trace( &tr, newOrg, NULL, NULL, newOrg2, -1, MASK_PLAYERSOLID/*MASK_SOLID|MASK_WATER*//*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA | MASK_WATER*/ );

	if ( tr.fraction != 1 )
	{// Repair the position...
		float move_ammount = VectorDistance(tr.endpos, newOrg2);

		VectorMA(newOrg, 0-(move_ammount+1), forward, newOrg);
	}
	
	VectorCopy(newOrg, fixed_position);

	// Prepare for back test...
	VectorCopy( fixed_position, newOrg );
	VectorCopy( fixed_position, newOrg2 );
	VectorMA(newOrg2, -64, forward, newOrg2);

	// Do back test...
	CG_Trace( &tr, newOrg, NULL, NULL, newOrg2, -1, MASK_PLAYERSOLID/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA | MASK_WATER*/ );

	if ( tr.fraction != 1 )
	{// Repair the position...
		float move_ammount = VectorDistance(tr.endpos, newOrg2);

		VectorMA(newOrg, move_ammount+1, forward, newOrg);
	}
	
	VectorCopy(newOrg, fixed_position);

	// Prepare for right test...
	VectorCopy( fixed_position, newOrg );
	VectorCopy( fixed_position, newOrg2 );
	VectorMA(newOrg2, 64, right, newOrg2);

	// Do right test...
	CG_Trace( &tr, newOrg, NULL, NULL, newOrg2, -1, MASK_PLAYERSOLID/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA | MASK_WATER*/ );

	if ( tr.fraction != 1 )
	{// Repair the position...
		float move_ammount = VectorDistance(tr.endpos, newOrg2);

		VectorMA(newOrg, 0-(move_ammount+1), right, newOrg);
	}
	
	VectorCopy(newOrg, fixed_position);

	// Prepare for left test...
	VectorCopy( fixed_position, newOrg );
	VectorCopy( fixed_position, newOrg2 );
	VectorMA(newOrg2, -64, right, newOrg2);

	// Do left test...
	CG_Trace( &tr, newOrg, NULL, NULL, newOrg2, -1, MASK_PLAYERSOLID/*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA | MASK_WATER*/ );

	if ( tr.fraction != 1 )
	{// Repair the position...
		float move_ammount = VectorDistance(tr.endpos, newOrg2);

		VectorMA(newOrg, move_ammount+1, right, newOrg);
	}
	
	VectorCopy(newOrg, fixed_position);

	// Prepare for solid test...
	VectorCopy( fixed_position, newOrg );
	VectorCopy( fixed_position, newOrg2 );
	VectorMA(newOrg2, 16, up, newOrg2);

	// Do start-solid test...
	CG_Trace( &tr, newOrg, NULL, NULL, newOrg2, -1, MASK_PLAYERSOLID /*CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA | MASK_WATER*/ );

	if ( tr.fraction != 1 || tr.startsolid || tr.contents & CONTENTS_WATER)
	{// Bad waypoint. Remove it!
		fixed_position[0] = 0.0f;
		fixed_position[1] = 0.0f;
		fixed_position[2] = -65536.0f;
		return;
	}

	// New floor test...
	/*fixed_position[2]=FloorHeightAt(fixed_position)+16;

	if (fixed_position[2] == -65536.0f || fixed_position[2] == 65536.0f)
	{// Bad waypoint. Remove it!
		fixed_position[0] = 0.0f;
		fixed_position[1] = 0.0f;
		fixed_position[2] = -65536.0f;
		return;
	}*/

	//
	// Let's try also centralizing the points...
	//
/*
	// Prepare for forward test...
	VectorCopy( fixed_position, newOrg );
	VectorCopy( fixed_position, newOrg2 );
	VectorMA(newOrg2, 65536, forward, newOrg2);

	// Do forward test...
	CG_Trace( &tr, newOrg, NULL, NULL, newOrg2, -1, CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA | MASK_WATER );
	
	if ( VectorDistance(newOrg, tr.endpos) < 256 )
	{// Possibly a hallway.. Can we centralize it?
		float move_ammount = VectorDistance(newOrg, tr.endpos);

		// Prepare for back test...
		VectorCopy( fixed_position, newOrg );
		VectorCopy( fixed_position, newOrg2 );
		VectorMA(newOrg2, -65536, forward, newOrg2);

		// Do back test...
		CG_Trace( &tr, newOrg, NULL, NULL, newOrg2, -1, CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA | MASK_WATER );

		if ( VectorDistance(newOrg, tr.endpos) < 256 )
		{
			move_ammount -= VectorDistance(newOrg, tr.endpos);
			VectorMA(newOrg, move_ammount, forward, newOrg);
			VectorCopy(newOrg, fixed_position);
		}
	}

	// Prepare for right test...
	VectorCopy( fixed_position, newOrg );
	VectorCopy( fixed_position, newOrg2 );
	VectorMA(newOrg2, 64, right, newOrg2);

	// Do right test...
	CG_Trace( &tr, newOrg, NULL, NULL, newOrg2, -1, CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA | MASK_WATER );

	if ( VectorDistance(newOrg, tr.endpos) < 256 )
	{// Possibly a hallway.. Can we centralize it?
		float move_ammount = VectorDistance(newOrg, tr.endpos);

		// Prepare for left test...
		VectorCopy( fixed_position, newOrg );
		VectorCopy( fixed_position, newOrg2 );
		VectorMA(newOrg2, -65536, right, newOrg2);

		// Do back test...
		CG_Trace( &tr, newOrg, NULL, NULL, newOrg2, -1, CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA | MASK_WATER );

		if ( VectorDistance(newOrg, tr.endpos) < 256 )
		{
			move_ammount -= VectorDistance(newOrg, tr.endpos);
			VectorMA(newOrg, move_ammount, right, newOrg);
			VectorCopy(newOrg, fixed_position);
		}
	}*/
#endif //__AW_UNUSED__
}

vec3_t	aw_ladder_positions[MAX_NODES];
int		aw_num_ladder_positions = 0;
int		aw_total_waypoints = 0;
/*
int AIMod_AutoWaypoint_Ladders_Angle_Count ( vec3_t org )
{
	vec3_t			org1, org2, forward, right, up, testangles;
	int				i = 0;
	int				start_angle, end_angle;

	start_angle = -1;
	end_angle = -1;

	VectorCopy(org, org1);
	org1[2]+=16;

	for (i = 0; i < 361; i++)
	{
		trace_t		trace;

		VectorCopy(org, org2);

		testangles[0] = testangles[1] = testangles[2] = 0;
		testangles[YAW] = (float)i;
		AngleVectors( testangles, forward, right, up );
		VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
		org2[2]+=16;

		trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );
		
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

			VectorCopy(org, org2);

			testangles[0] = testangles[1] = testangles[2] = 0;
			testangles[YAW] = (float)i;
			AngleVectors( testangles, forward, right, up );
			VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
			org2[2]+=16;

			trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );
		
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

			VectorCopy(org, org2);

			testangles[0] = testangles[1] = testangles[2] = 0;
			testangles[YAW] = (float)i;
			AngleVectors( testangles, forward, right, up );
			VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
			org2[2]+=16;

			trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );
		
			if (trace.surfaceFlags & SURF_LADDER)
			{// Ladder found here! Make the waypoints!
				end_angle = i;
			}
		}
	}

	return (end_angle - start_angle);
}*/

vec3_t aw_best_ladder_spot;
/*
void AIMod_AutoWaypoint_FindLadderCenter ( vec3_t original_org )
{
	vec3_t			org1, best_position;
	int				angle_count = 0, best_angle_count = 0;
	int				positionX = -32;
	int				positionY = -32;

	for (positionX = -32; positionX < 33; positionX += 4)
	{
		VectorCopy( original_org, org1 );
		org1[0] += positionX;
		org1[1] += positionY;

		angle_count = AIMod_AutoWaypoint_Ladders_Angle_Count(org1);

		if (angle_count > best_angle_count)
		{
			best_angle_count = angle_count;
			VectorCopy(org1, best_position);
		}
	}

	positionX = -32;
	positionY = 32;

	for (positionX = -32; positionX < 33; positionX += 4)
	{
		VectorCopy( original_org, org1 );
		org1[0] += positionX;
		org1[1] += positionY;

		angle_count = AIMod_AutoWaypoint_Ladders_Angle_Count(org1);

		if (angle_count > best_angle_count)
		{
			best_angle_count = angle_count;
			VectorCopy(org1, best_position);
		}
	}

	positionX = -32;
	positionY = -32;

	for (positionY = -32; positionY < 33; positionY += 4)
	{
		VectorCopy( original_org, org1 );
		org1[0] += positionX;
		org1[1] += positionY;

		angle_count = AIMod_AutoWaypoint_Ladders_Angle_Count(org1);

		if (angle_count > best_angle_count)
		{
			best_angle_count = angle_count;
			VectorCopy(org1, best_position);
		}
	}

	positionX = 32;

	for (positionY = -32; positionY < 33; positionY += 4)
	{
		VectorCopy( original_org, org1 );
		org1[0] += positionX;
		org1[1] += positionY;

		angle_count = AIMod_AutoWaypoint_Ladders_Angle_Count(org1);

		if (angle_count > best_angle_count)
		{
			best_angle_count = angle_count;
			VectorCopy(org1, best_position);
		}
	}

	VectorCopy( best_position, aw_best_ladder_spot);
}*/

#ifdef __CUDA__
__device__ __host__ void AIMod_AutoWaypoint_Check_Create_Ladder_Waypoints ( vec3_t original_org, vec3_t angles )
{
	__shared__ vec3_t			org1, org2, forward, right, up, last_endpos, ladder_pos;
	__shared__ qboolean		complete = qfalse;
	__shared__ vec3_t			traceMins = { -20, -20, -1 };
	__shared__ vec3_t			traceMaxs = { 20, 20, 32 };

	VectorCopy(original_org, org1);
	//org1[2] = FloorHeightAt(org1);

	VectorCopy(original_org, org2);
	AngleVectors( angles, forward, right, up );
	VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
	//org2[2] = FloorHeightAt(org2);

	while (!complete)
	{
		__shared__ trace_t		trace;

		__host__ trap_CM_BoxTrace( &trace, org1, org2, traceMins, traceMaxs,  0, MASK_PLAYERSOLID );

		if (trace.surfaceFlags & SURF_LADDER)
		{// Ladder found here! Add a new waypoint!
			__shared__ short int	objNum[3] = { 0, 0, 0 };

			VectorCopy(trace.endpos, ladder_pos);
			Load_AddNode( ladder_pos, 0, objNum, 0 );	//add the node
			aw_total_waypoints++;
			VectorCopy(ladder_pos, last_endpos);
		}
		else
		{// We found the top of the ladder... Add 1 more waypoint and we are done! :)
			__shared__ short int	objNum[3] = { 0, 0, 0 };

			last_endpos[2]+=16;

			Load_AddNode( last_endpos, 0, objNum, 0 );	//add the node
			aw_total_waypoints++;

			complete = qtrue;
		}

		org1[2]+=16;
		org2[2]+=16;
	}
}

__device__ __host__ void AIMod_AutoWaypoint_Check_For_Ladders ( vec3_t org )
{
	__shared__ vec3_t			org1, org2, forward, right, up, testangles;
	__shared__ int				i = 0;
	__shared__ int				start_angle, end_angle;

	start_angle = -1;
	end_angle = -1;

	for ( i = 0; i < aw_num_ladder_positions; i++)
	{// Do a quick check to make sure we do not do the same ladder twice!
		if (VectorDistanceNoHeight(org, aw_ladder_positions[i]) < 128)
			return;
	}

	// UQ1: Not working :(
//	if (AIMod_AutoWaypoint_Ladders_Angle_Count( org ) <= 0)
//		return;

//	AIMod_AutoWaypoint_FindLadderCenter( org );
//	VectorCopy(aw_best_ladder_spot, org1);
	VectorCopy(org, aw_best_ladder_spot);
	VectorCopy(org, org1);
	
	org1[2]+=16;

	for (i = 0; i < 361; i++)
	{
		__shared__ trace_t		trace;

		VectorCopy(aw_best_ladder_spot/*org*/, org2);

		testangles[0] = testangles[1] = testangles[2] = 0;
		testangles[YAW] = (float)i;
		AngleVectors( testangles, forward, right, up );
		VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
		org2[2]+=16;

		__host__ trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );
		
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
			__shared__ trace_t		trace;

			VectorCopy(aw_best_ladder_spot/*org*/, org2);

			testangles[0] = testangles[1] = testangles[2] = 0;
			testangles[YAW] = (float)i;
			AngleVectors( testangles, forward, right, up );
			VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
			org2[2]+=16;

			__host__ trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );
		
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
			__shared__ trace_t		trace;

			VectorCopy(aw_best_ladder_spot/*org*/, org2);

			testangles[0] = testangles[1] = testangles[2] = 0;
			testangles[YAW] = (float)i;
			AngleVectors( testangles, forward, right, up );
			VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
			org2[2]+=16;

			__host__ trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );
		
			if (trace.surfaceFlags & SURF_LADDER)
			{// Ladder found here! Make the waypoints!
				end_angle = i;
			}
		}
	}

	if (start_angle != -1 && end_angle != -1 
		&& ( end_angle - start_angle >= 30 /*|| (start_angle <= 0 && end_angle - start_angle >= 20)*/ ) )
	{// We found one... Find the center angle and use it (hopefully the most central part of the ladder!)
		__shared__ int			middle_angle = ((end_angle - start_angle) * 0.5) + start_angle;
		__shared__ trace_t		trace;

		CG_Printf("ladder %i: start_angle was %i. end_angle was %i. middle_angle is %i.\n", aw_num_ladder_positions, start_angle, end_angle, middle_angle);

		VectorCopy(aw_best_ladder_spot/*org*/, org2);

		testangles[0] = testangles[1] = testangles[2] = 0;
		testangles[YAW] = (float)middle_angle;
		AngleVectors( testangles, forward, right, up );
		VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
		org2[2]+=16;

		trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );

		strcpy( last_node_added_string, va("^5Adding ladder (^3%i^5) waypoints at ^7%f %f %f^5.", aw_num_ladder_positions+1, trace.endpos[0], trace.endpos[1], trace.endpos[2]) );
		AIMod_AutoWaypoint_Check_Create_Ladder_Waypoints(org1, testangles);
		VectorCopy(trace.endpos, aw_ladder_positions[aw_num_ladder_positions]);
		aw_num_ladder_positions++;
	}
}
#else //!__CUDA__
void AIMod_AutoWaypoint_Check_Create_Ladder_Waypoints ( vec3_t original_org, vec3_t angles )
{
	vec3_t			org1, org2, forward, right, up, last_endpos, ladder_pos;
	qboolean		complete = qfalse;
	vec3_t			traceMins = { -20, -20, -1 };
	vec3_t			traceMaxs = { 20, 20, 32 };

	VectorCopy(original_org, org1);
	//org1[2] = FloorHeightAt(org1);

	VectorCopy(original_org, org2);
	AngleVectors( angles, forward, right, up );
	VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
	//org2[2] = FloorHeightAt(org2);

	while (!complete)
	{
		trace_t		trace;

		trap_CM_BoxTrace( &trace, org1, org2, traceMins, traceMaxs,  0, MASK_PLAYERSOLID );

		if (trace.surfaceFlags & SURF_LADDER)
		{// Ladder found here! Add a new waypoint!
			short int	objNum[3] = { 0, 0, 0 };

			VectorCopy(trace.endpos, ladder_pos);
			Load_AddNode( ladder_pos, 0, objNum, 0 );	//add the node
			aw_total_waypoints++;
			VectorCopy(ladder_pos, last_endpos);
		}
		else
		{// We found the top of the ladder... Add 1 more waypoint and we are done! :)
			short int	objNum[3] = { 0, 0, 0 };

			last_endpos[2]+=16;

			Load_AddNode( last_endpos, 0, objNum, 0 );	//add the node
			aw_total_waypoints++;

			complete = qtrue;
		}

		org1[2]+=16;
		org2[2]+=16;
	}
}

void AIMod_AutoWaypoint_Check_For_Ladders ( vec3_t org )
{
	vec3_t			org1, org2, forward, right, up, testangles;
	int				i = 0;
	int				start_angle, end_angle;

	start_angle = -1;
	end_angle = -1;

	for ( i = 0; i < aw_num_ladder_positions; i++)
	{// Do a quick check to make sure we do not do the same ladder twice!
		if (VectorDistanceNoHeight(org, aw_ladder_positions[i]) < 128)
			return;
	}

	// UQ1: Not working :(
//	if (AIMod_AutoWaypoint_Ladders_Angle_Count( org ) <= 0)
//		return;

//	AIMod_AutoWaypoint_FindLadderCenter( org );
//	VectorCopy(aw_best_ladder_spot, org1);
	VectorCopy(org, aw_best_ladder_spot);
	VectorCopy(org, org1);
	
	org1[2]+=16;

	for (i = 0; i < 361; i++)
	{
		trace_t		trace;

		VectorCopy(aw_best_ladder_spot/*org*/, org2);

		testangles[0] = testangles[1] = testangles[2] = 0;
		testangles[YAW] = (float)i;
		AngleVectors( testangles, forward, right, up );
		VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
		org2[2]+=16;

		trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );
		
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

			VectorCopy(aw_best_ladder_spot/*org*/, org2);

			testangles[0] = testangles[1] = testangles[2] = 0;
			testangles[YAW] = (float)i;
			AngleVectors( testangles, forward, right, up );
			VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
			org2[2]+=16;

			trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );
		
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

			VectorCopy(aw_best_ladder_spot/*org*/, org2);

			testangles[0] = testangles[1] = testangles[2] = 0;
			testangles[YAW] = (float)i;
			AngleVectors( testangles, forward, right, up );
			VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
			org2[2]+=16;

			trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );
		
			if (trace.surfaceFlags & SURF_LADDER)
			{// Ladder found here! Make the waypoints!
				end_angle = i;
			}
		}
	}

	if (start_angle != -1 && end_angle != -1 
		&& ( end_angle - start_angle >= 30 /*|| (start_angle <= 0 && end_angle - start_angle >= 20)*/ ) )
	{// We found one... Find the center angle and use it (hopefully the most central part of the ladder!)
		int			middle_angle = ((end_angle - start_angle) * 0.5) + start_angle;
		trace_t		trace;

		CG_Printf("ladder %i: start_angle was %i. end_angle was %i. middle_angle is %i.\n", aw_num_ladder_positions, start_angle, end_angle, middle_angle);

		VectorCopy(aw_best_ladder_spot/*org*/, org2);

		testangles[0] = testangles[1] = testangles[2] = 0;
		testangles[YAW] = (float)middle_angle;
		AngleVectors( testangles, forward, right, up );
		VectorMA ( org2, waypoint_scatter_distance*3 , forward , org2);
		org2[2]+=16;

		trap_CM_BoxTrace( &trace, org1, org2, NULL, NULL,  0, MASK_PLAYERSOLID );

		strcpy( last_node_added_string, va("^5Adding ladder (^3%i^5) waypoints at ^7%f %f %f^5.", aw_num_ladder_positions+1, trace.endpos[0], trace.endpos[1], trace.endpos[2]) );
		AIMod_AutoWaypoint_Check_Create_Ladder_Waypoints(org1, testangles);
		VectorCopy(trace.endpos, aw_ladder_positions[aw_num_ladder_positions]);
		aw_num_ladder_positions++;
	}
}
#endif //__CUDA__

//#define DEFAULT_AREA_SEPERATION 512
#define DEFAULT_AREA_SEPERATION 340
#define MAP_BOUNDS_OFFSET 512 //2048

#ifdef __CUDA__
__device__ __host__ void AIMod_AutoWaypoint_StandardMethod( void )
{// Advanced method for multi-level maps...
	__shared__ int			i;
	__shared__ float		startx = -MAX_MAP_SIZE, starty = -MAX_MAP_SIZE, startz = -MAX_MAP_SIZE;
	__shared__ float		orig_startx, orig_starty, orig_startz;
	__shared__ int			areas = 0, total_waypoints = 0, total_areas = 0;
	__shared__ intvec3_t	*arealist;
	__shared__ float		map_size, temp, original_waypoint_scatter_distance = waypoint_scatter_distance;
	__shared__ vec3_t		mapMins, mapMaxs;
	__shared__ int			total_tests = 0, final_tests = 0;
	__shared__ int			start_time = trap_Milliseconds();
	__shared__ int			update_timer = 0;
	__shared__ int			area_skipper = 1;

	trap_Cvar_Set("cg_waypoint_render", "0");
	trap_UpdateScreen();
	trap_UpdateScreen();
	trap_UpdateScreen();

	AIMod_AutoWaypoint_Init_Memory();

/*	if (!BG_TraceMapLoaded())
	{
		if (!BG_LoadTraceMap(cgs.rawmapname, cg.mapcoordsMins, cg.mapcoordsMaxs ))
		{
			CG_GenerateTracemap();
		}
	}*/

	if (!cg.mapcoordsValid)
	{
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^7Map Coordinates are invalid. Can not use auto-waypointer!\n");
		return;
	}

	arealist = B_Alloc((sizeof(intvec3_t)+1)*512000);

	AIMod_GetMapBounts();

	VectorCopy(cg.mapcoordsMins, mapMins);
	VectorCopy(cg.mapcoordsMaxs, mapMaxs);

	if (mapMaxs[0] < mapMins[0])
	{
		temp = mapMins[0];
		mapMins[0] = mapMaxs[0];
		mapMaxs[0] = temp;
	}

	if (mapMaxs[1] < mapMins[1])
	{
		temp = mapMins[1];
		mapMins[1] = mapMaxs[1];
		mapMaxs[1] = temp;
	}

	if (mapMaxs[2] < mapMins[2])
	{
		temp = mapMins[2];
		mapMins[2] = mapMaxs[2];
		mapMaxs[2] = temp;
	}

	map_size = VectorDistance(mapMins, mapMaxs);

	// Work out the best scatter distance to use for this map size...
	if (map_size > 96000)
	{
		waypoint_scatter_distance *= 1.5;
	}
	else if (map_size > 32768)
	{
		waypoint_scatter_distance *= 1.35;
	}
	else if (map_size > 24000)
	{
		waypoint_scatter_distance *= 1.25;
	}
	else if (map_size > 16550)
	{
		waypoint_scatter_distance *= 1.10;
	}
	else if (map_size > 8192)
	{
		waypoint_scatter_distance *= 1.02;
	}

	orig_startx = mapMaxs[0]+MAP_BOUNDS_OFFSET/*-16*/;
	orig_starty = mapMaxs[1]+MAP_BOUNDS_OFFSET/*-16*/;
	orig_startz = mapMaxs[2]+MAP_BOUNDS_OFFSET/*-16*/;

	startx = orig_startx;
	starty = orig_starty;
	startz = orig_startz;

/*	while ( startx > mapMins[0]-MAP_BOUNDS_OFFSET )
	{
		while ( starty > mapMins[1]-MAP_BOUNDS_OFFSET )
		{
			while ( startz > mapMins[2]-MAP_BOUNDS_OFFSET )
			{
				startz -= waypoint_scatter_distance;
				total_tests++;
			}
			
			startz = orig_startz;
			starty -= waypoint_scatter_distance;
		}

		starty = orig_starty;
		startx -= waypoint_scatter_distance;
	}*/

	while ( startx > mapMins[0]-MAP_BOUNDS_OFFSET )
	{
		total_tests++;
		startx -= waypoint_scatter_distance;
	}

	startx = orig_startx;
	starty = orig_starty;
	startz = orig_startz;

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^7Map bounds are ^3%.2f %.2f %.2f ^7to ^3%.2f %.2f %.2f^7.\n", mapMins[0], mapMins[1], mapMins[2], mapMaxs[0], mapMaxs[1], mapMaxs[2]) );
	strcpy( task_string1, va("^7Map bounds are ^3%.2f %.2f %.2f ^7to ^3%.2f %.2f %.2f^7.", mapMins[0], mapMins[1], mapMins[2], mapMaxs[0], mapMaxs[1], mapMaxs[2]) );
	trap_UpdateScreen();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Generating AI waypoints. This could take a while... (Map size ^3%.2f^5)\n", map_size) );
	strcpy( task_string2, va("^5Generating AI waypoints. This could take a while... (Map size ^3%.2f^5)", map_size) );
	trap_UpdateScreen();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5First pass. Finding temporary waypoints...\n") );
	strcpy( task_string3, va("^5First pass. Finding temporary waypoints...") );
	trap_UpdateScreen();

	while ( startx > mapMins[0]-MAP_BOUNDS_OFFSET )
	{
		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

		//update_timer++;

		//if (update_timer >= 5000)
		//{
			trap_UpdateScreen();
		//	update_timer = 0;
		//}

		while ( starty > mapMins[1]-MAP_BOUNDS_OFFSET )
		{
			__shared__ vec3_t last_org, new_org;
			__shared__ float floor, current_floor = startz;

			VectorSet(last_org, 0, 0, 0);
			VectorSet(new_org, startx, starty, startz);

			floor = FloorHeightAt(new_org);

			if (floor == -65536.0f || floor <= mapMins[2]-MAP_BOUNDS_OFFSET)
			{// Can skip this one!
				/*while (startz > floor && startz > mapMins[2]-MAP_BOUNDS_OFFSET)
				{// We can skip some checks, until we get to our hit position...
					startz -= waypoint_scatter_distance;
					final_tests++;

					// Draw a nice little progress bar ;)
					aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

					update_timer++;

					if (update_timer >= 5000)
					{
						trap_UpdateScreen();
						update_timer = 0;
					}
				}*/

				startz = orig_startz;
				starty -= waypoint_scatter_distance;
				continue;
			}

			while ( startz > mapMins[2]-MAP_BOUNDS_OFFSET )
			{
				__shared__ vec3_t org;
				__shared__ float orig_floor;

				if (current_floor == -65536.0f || current_floor <= mapMins[2]-MAP_BOUNDS_OFFSET)
				{// Can skip this one!
					/*while (startz > floor && startz > mapMins[2]-MAP_BOUNDS_OFFSET)
					{// We can skip some checks, until we get to our hit position...
						startz -= waypoint_scatter_distance;
						final_tests++;

						// Draw a nice little progress bar ;)
						aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

						update_timer++;

						if (update_timer >= 5000)
						{
							trap_UpdateScreen();
							update_timer = 0;
						}
					}*/

					startz = orig_startz;
					starty -= waypoint_scatter_distance;
					break;
				}

				/*update_timer++;

				if (update_timer >= 5000)
				{
					trap_UpdateScreen();
					update_timer = 0;
				}*/

				VectorSet(org, startx, starty, startz);

				org[2]=FloorHeightAt(org);
				
				if (org[2] == -65536.0f)
				{
					/*while (startz > floor && startz > mapMins[2]-MAP_BOUNDS_OFFSET)
					{// We can skip some checks, until we get to our hit position...
						startz -= waypoint_scatter_distance;
						final_tests++;

						// Draw a nice little progress bar ;)
						aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

						update_timer++;

						if (update_timer >= 5000)
						{
							trap_UpdateScreen();
							update_timer = 0;
						}
					}*/

					startz = orig_startz;
					starty -= waypoint_scatter_distance;
					break;
				}

//				final_tests++;

				// Draw a nice little progress bar ;)
//				aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

				if (org[2] == 65536.0f)
				{// Marks a start-solid or on top of the sky... Skip...
					startz -= waypoint_scatter_distance;
					continue;
				}

				if (VectorDistance(org, last_org) < (waypoint_scatter_distance))
				{
					startz -= waypoint_scatter_distance;
					continue;
				}

				/*if (CheckSolid( org ) || AIMod_AutoWaypoint_Check_Slope(org))
				{// Bad slope angles here!
					startz -= waypoint_scatter_distance;
					continue;
				}*/

				// Raise node a little to add it...(for visibility)
				orig_floor = org[2];
				org[2] += 8;

				strcpy( last_node_added_string, va("^5Adding temp waypoint ^3%i ^5at ^7%f %f %f^5.", areas/*+areas2+areas3*/, org[0], org[1], org[2]) );

				VectorCopy( org, arealist[areas] );
				areas++;

				if (aw_floor_trace_hit_mover)
				{// Need to generate waypoints to the top/bottom of the mover...
					__shared__ float temp_roof, temp_ground;
					__shared__ vec3_t temp_org, temp_org2;
					
					VectorCopy(org, temp_org2);
					VectorCopy(org, temp_org);
					temp_org[2] += 24; // Start above the lift...
					temp_roof = RoofHeightAt(temp_org);

					VectorCopy(org, temp_org);
					temp_org[2] -= 24; // Start below the lift...
					temp_ground = GroundHeightAt(temp_org);

					temp_org2[2] = temp_roof;

					if (temp_roof < 64000 && HeightDistance(temp_org, temp_org2) >= 128)
					{// Looks like it goes up!
						VectorCopy(org, temp_org);
						temp_org[2] += 24;

						while (temp_org[2] <= temp_org2[2])
						{// Add waypoints all the way up!
							VectorCopy( temp_org, arealist[areas] );
							areas++;
							temp_org[2] += 24;
						}
					}

					temp_org2[2] = temp_ground;

					if (temp_roof > -64000 && HeightDistance(temp_org, temp_org2) >= 128)
					{// Looks like it goes up!
						VectorCopy(org, temp_org);
						temp_org[2] -= 24;

						while (temp_org[2] >= temp_org2[2])
						{// Add waypoints all the way up!
							VectorCopy( temp_org, arealist[areas] );
							areas++;
							temp_org[2] -= 24;
						}
					}
				}

				// Lower current org a back to where it was before raising it above...
				org[2] = orig_floor;
					
				VectorCopy(org, last_org);

				floor = org[2];
				current_floor = floor;

				startz -= waypoint_scatter_distance;

				while (startz > floor && startz > mapMins[2]-MAP_BOUNDS_OFFSET)
				{// We can skip some checks, until we get to our current floor height...
					startz -= waypoint_scatter_distance;
					/*final_tests++;

					// Draw a nice little progress bar ;)
					aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

					update_timer++;

					if (update_timer >= 5000)
					{
						trap_UpdateScreen();
						update_timer = 0;
					}*/
				}
			}

			startz = orig_startz;
			starty -= waypoint_scatter_distance;
		}

		starty = orig_starty;
		startx -= waypoint_scatter_distance;

		/* */
		final_tests++;
	}

	if (areas < 65536)
	{// UQ1: Can use them all!
		aw_percent_complete = 0.0f;
		strcpy( task_string3, va("^5Final (cleanup) pass. Building final waypoints...") );
		trap_UpdateScreen();

		total_areas = areas;

//#pragma omp parallel for
		for ( i = 0; i < areas; i++ )
		{
			__shared__ vec3_t		area_org;
			__shared__ short int	objNum[3] = { 0, 0, 0 };

			// Draw a nice little progress bar ;)
			aw_percent_complete = (float)((float)((float)i/(float)total_areas)*100.0f);
				
			update_timer++;

			area_org[0] = arealist[i][0];
			area_org[1] = arealist[i][1];
			area_org[2] = arealist[i][2];

			if (area_org[2] <= -65536.0f)
			{// This is a bad height!
				continue;
			}

			if (update_timer >= 500)
			{
				strcpy( last_node_added_string, va("^5Adding waypoint ^3%i ^5at ^7%f %f %f^5.", total_waypoints, area_org[0], area_org[1], area_org[2]) );
				trap_UpdateScreen();
				update_timer = 0;
			}
			
			Load_AddNode( area_org, 0, objNum, 0 );	//add the node
			total_waypoints++;
		}

		// Ladders...
		aw_total_waypoints = total_waypoints;
		aw_percent_complete = 0.0f;
		strcpy( task_string3, va("^5Looking for ladders...") );
		trap_UpdateScreen();

		for ( i = 0; i < total_waypoints; i++ )
		{
			// Draw a nice little progress bar ;)
			aw_percent_complete = (float)((float)((float)i/(float)total_waypoints)*100.0f);
				
			update_timer++;

			if (update_timer >= 100)
			{
				trap_UpdateScreen();
				update_timer = 0;
			}

			AIMod_AutoWaypoint_Check_For_Ladders( nodes[i].origin );
		}

		total_waypoints = aw_total_waypoints;
		// End Ladders...

		strcpy( task_string3, va("^5Saving the generated waypoints.") );
		trap_UpdateScreen();

		number_of_nodes = total_waypoints;
		AIMOD_NODES_SaveNodes_Autowaypointed();

		CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Waypoint database created successfully in ^3%.2f ^5seconds with ^7%i ^5waypoints.\n",
				 (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );

		strcpy( task_string3, va("^5Waypoint database created successfully in ^3%.2f ^5seconds with ^7%i ^5waypoints.\n", (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );
		trap_UpdateScreen();

		aw_percent_complete = 0.0f;
		strcpy( task_string3, va("^5Informing the server to load & test the new waypoints...") );
		trap_UpdateScreen();

		trap_SendConsoleCommand( "!loadnodes\n" );

		aw_percent_complete = 0.0f;
		strcpy( task_string3, va("^5Waypoint auto-generation is complete...") );
		trap_UpdateScreen();

		waypoint_scatter_distance = original_waypoint_scatter_distance;
		
		//AIMOD_Generate_Cover_Spots();

		B_Free(arealist);

		AIMod_AutoWaypoint_Free_Memory();

		aw_percent_complete = 0.0f;
		trap_UpdateScreen();

		return;
	}

	total_areas = areas;

	if (total_areas < 40000)
	{
		waypoint_distance_multiplier = 1.5f;
	}
	else if (total_areas < 64000)
	{
		waypoint_distance_multiplier = 2.0f;
	}
	else
	{
		waypoint_distance_multiplier = 2.5f;
	}

	if (total_areas < 40000)
	{
		area_skipper = 1;
	}
	else if (total_areas < 64000)
	{
		area_skipper = 2;
	}
	else if (total_areas < 96000)
	{
		area_skipper = 3;
	}
	else if (total_areas < 128000)
	{
		area_skipper = 4;
	}
	else
	{
		area_skipper = 5;
	}

#ifdef __AW_UNUSED__
	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Second (repair) pass. Adjusting waypoint positions...") );
	trap_UpdateScreen();

//#pragma omp parallel for
	for ( i = 0; i < areas; i++ )
	{
		vec3_t original_position = { arealist[i][0], arealist[i][1], arealist[i][2] };

		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)i/(float)total_areas)*100.0f);
		
		update_timer++;

		if (update_timer >= 500)
		{
			trap_UpdateScreen();
			update_timer = 0;
		}

		RepairPosition( arealist[i] );

		if (VectorDistance(fixed_position, original_position) > 0)
		{// New position.. Fix it!
			strcpy( last_node_added_string, va("^5Temp waypoint ^3%i ^5moved to ^7%f %f %f^5.", i, fixed_position[0], fixed_position[1], fixed_position[2]) );
			arealist[i][0] = fixed_position[0];
			arealist[i][1] = fixed_position[1];
			arealist[i][2] = fixed_position[2];
		}
	}
#endif //__AW_UNUSED__

	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Final (cleanup) pass. Building final waypoints...") );
	trap_UpdateScreen();

//#pragma omp parallel for
	for ( i = 0; i < areas; i+=area_skipper/*i++*/ )
	{
		__shared__ vec3_t		area_org;
		__shared__ short int	objNum[3] = { 0, 0, 0 };
//		qboolean	bad = qfalse;
		//int			j;

		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)i/(float)total_areas)*100.0f);
				
		update_timer++;

		area_org[0] = arealist[i][0];
		area_org[1] = arealist[i][1];
		area_org[2] = arealist[i][2];

		if (area_org[2] <= -65536.0f)
		{// This is a bad height!
			continue;
		}

//#pragma omp parallel
/*		for (j = 0; j < number_of_nodes; j++)
		{
			vec3_t area_org2;

			area_org2[0] = nodes[j].origin[0];
			area_org2[1] = nodes[j].origin[1];
			area_org2[2] = nodes[j].origin[2];

			if (VectorDistance(area_org, area_org2) < waypoint_scatter_distance*waypoint_distance_multiplier)
			{
				bad = qtrue;
				break;
			}
		}

		if (bad)
		{
			continue;
		}*/

		if (update_timer >= 100)
		{
			strcpy( last_node_added_string, va("^5Adding waypoint ^3%i ^5at ^7%f %f %f^5.", total_waypoints, area_org[0], area_org[1], area_org[2]) );
			trap_UpdateScreen();
			update_timer = 0;
		}
		
		Load_AddNode( area_org, 0, objNum, 0 );	//add the node
		total_waypoints++;
	}

	// Ladders...
	aw_total_waypoints = total_waypoints;
	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Looking for ladders...") );
	trap_UpdateScreen();

	for ( i = 0; i < total_waypoints; i++ )
	{
		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)i/(float)total_waypoints)*100.0f);
				
		update_timer++;

		if (update_timer >= 100)
		{
			trap_UpdateScreen();
			update_timer = 0;
		}

		AIMod_AutoWaypoint_Check_For_Ladders( nodes[i].origin );
	}

	total_waypoints = aw_total_waypoints;
	// End Ladders...

	strcpy( task_string3, va("^5Saving the generated waypoints.\n") );
	trap_UpdateScreen();

	number_of_nodes = total_waypoints;
	AIMOD_NODES_SaveNodes_Autowaypointed();

	//AIMOD_Generate_Cover_Spots();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Waypoint database created successfully in ^3%.2f ^5seconds with ^7%i ^5waypoints.\n",
				 (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );

	strcpy( task_string3, va("^5Waypoint database created successfully in ^3%.2f ^5seconds with ^7%i ^5waypoints.\n", (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );
	trap_UpdateScreen();

	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Informing the server to load & test the new waypoints...") );
	trap_UpdateScreen();

	trap_SendConsoleCommand( "!loadnodes\n" );

	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Waypoint auto-generation is complete...") );
	trap_UpdateScreen();

	waypoint_scatter_distance = original_waypoint_scatter_distance;

	B_Free(arealist);

	AIMod_AutoWaypoint_Free_Memory();

	aw_percent_complete = 0.0f;
	trap_UpdateScreen();
}
#else //!__CUDA__
void AIMod_AutoWaypoint_StandardMethod( void )
{// Advanced method for multi-level maps...
	int			i;
	float		startx = -MAX_MAP_SIZE, starty = -MAX_MAP_SIZE, startz = -MAX_MAP_SIZE;
	float		orig_startx, orig_starty, orig_startz;
	int			areas = 0, total_waypoints = 0, total_areas = 0;
	intvec3_t	*arealist;
	float		map_size, temp, original_waypoint_scatter_distance = waypoint_scatter_distance;
	vec3_t		mapMins, mapMaxs;
	int			total_tests = 0, final_tests = 0;
	int			start_time = trap_Milliseconds();
	int			update_timer = 0;
	int			area_skipper = 1;

	trap_Cvar_Set("cg_waypoint_render", "0");
	trap_UpdateScreen();
	trap_UpdateScreen();
	trap_UpdateScreen();

	AIMod_AutoWaypoint_Init_Memory();

/*	if (!BG_TraceMapLoaded())
	{
		if (!BG_LoadTraceMap(cgs.rawmapname, cg.mapcoordsMins, cg.mapcoordsMaxs ))
		{
			CG_GenerateTracemap();
		}
	}*/

	if (!cg.mapcoordsValid)
	{
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^7Map Coordinates are invalid. Can not use auto-waypointer!\n");
		return;
	}

	arealist = B_Alloc((sizeof(intvec3_t)+1)*512000);

	AIMod_GetMapBounts();

	VectorCopy(cg.mapcoordsMins, mapMins);
	VectorCopy(cg.mapcoordsMaxs, mapMaxs);

	if (mapMaxs[0] < mapMins[0])
	{
		temp = mapMins[0];
		mapMins[0] = mapMaxs[0];
		mapMaxs[0] = temp;
	}

	if (mapMaxs[1] < mapMins[1])
	{
		temp = mapMins[1];
		mapMins[1] = mapMaxs[1];
		mapMaxs[1] = temp;
	}

	if (mapMaxs[2] < mapMins[2])
	{
		temp = mapMins[2];
		mapMins[2] = mapMaxs[2];
		mapMaxs[2] = temp;
	}

	map_size = VectorDistance(mapMins, mapMaxs);

	// Work out the best scatter distance to use for this map size...
	if (map_size > 96000)
	{
		waypoint_scatter_distance *= 1.5;
	}
	else if (map_size > 32768)
	{
		waypoint_scatter_distance *= 1.35;
	}
	else if (map_size > 24000)
	{
		waypoint_scatter_distance *= 1.25;
	}
	else if (map_size > 16550)
	{
		waypoint_scatter_distance *= 1.10;
	}
	else if (map_size > 8192)
	{
		waypoint_scatter_distance *= 1.02;
	}

	orig_startx = mapMaxs[0]+MAP_BOUNDS_OFFSET/*-16*/;
	orig_starty = mapMaxs[1]+MAP_BOUNDS_OFFSET/*-16*/;
	orig_startz = mapMaxs[2]+MAP_BOUNDS_OFFSET/*-16*/;

	startx = orig_startx;
	starty = orig_starty;
	startz = orig_startz;

/*	while ( startx > mapMins[0]-MAP_BOUNDS_OFFSET )
	{
		while ( starty > mapMins[1]-MAP_BOUNDS_OFFSET )
		{
			while ( startz > mapMins[2]-MAP_BOUNDS_OFFSET )
			{
				startz -= waypoint_scatter_distance;
				total_tests++;
			}
			
			startz = orig_startz;
			starty -= waypoint_scatter_distance;
		}

		starty = orig_starty;
		startx -= waypoint_scatter_distance;
	}*/

	while ( startx > mapMins[0]-MAP_BOUNDS_OFFSET )
	{
		total_tests++;
		startx -= waypoint_scatter_distance;
	}

	startx = orig_startx;
	starty = orig_starty;
	startz = orig_startz;

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^7Map bounds are ^3%.2f %.2f %.2f ^7to ^3%.2f %.2f %.2f^7.\n", mapMins[0], mapMins[1], mapMins[2], mapMaxs[0], mapMaxs[1], mapMaxs[2]) );
	strcpy( task_string1, va("^7Map bounds are ^3%.2f %.2f %.2f ^7to ^3%.2f %.2f %.2f^7.", mapMins[0], mapMins[1], mapMins[2], mapMaxs[0], mapMaxs[1], mapMaxs[2]) );
	trap_UpdateScreen();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Generating AI waypoints. This could take a while... (Map size ^3%.2f^5)\n", map_size) );
	strcpy( task_string2, va("^5Generating AI waypoints. This could take a while... (Map size ^3%.2f^5)", map_size) );
	trap_UpdateScreen();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5First pass. Finding temporary waypoints...\n") );
	strcpy( task_string3, va("^5First pass. Finding temporary waypoints...") );
	trap_UpdateScreen();

	while ( startx > mapMins[0]-MAP_BOUNDS_OFFSET )
	{
		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

		//update_timer++;

		//if (update_timer >= 5000)
		//{
			trap_UpdateScreen();
		//	update_timer = 0;
		//}

		while ( starty > mapMins[1]-MAP_BOUNDS_OFFSET )
		{
			vec3_t last_org, new_org;
			float floor, current_floor = startz;

			VectorSet(last_org, 0, 0, 0);
			VectorSet(new_org, startx, starty, startz);

			floor = FloorHeightAt(new_org);

			if (floor == -65536.0f || floor <= mapMins[2]-MAP_BOUNDS_OFFSET)
			{// Can skip this one!
				/*while (startz > floor && startz > mapMins[2]-MAP_BOUNDS_OFFSET)
				{// We can skip some checks, until we get to our hit position...
					startz -= waypoint_scatter_distance;
					final_tests++;

					// Draw a nice little progress bar ;)
					aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

					update_timer++;

					if (update_timer >= 5000)
					{
						trap_UpdateScreen();
						update_timer = 0;
					}
				}*/

				startz = orig_startz;
				starty -= waypoint_scatter_distance;
				continue;
			}

			while ( startz > mapMins[2]-MAP_BOUNDS_OFFSET )
			{
				vec3_t org;
				float orig_floor;

				if (current_floor == -65536.0f || current_floor <= mapMins[2]-MAP_BOUNDS_OFFSET)
				{// Can skip this one!
					/*while (startz > floor && startz > mapMins[2]-MAP_BOUNDS_OFFSET)
					{// We can skip some checks, until we get to our hit position...
						startz -= waypoint_scatter_distance;
						final_tests++;

						// Draw a nice little progress bar ;)
						aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

						update_timer++;

						if (update_timer >= 5000)
						{
							trap_UpdateScreen();
							update_timer = 0;
						}
					}*/

					startz = orig_startz;
					starty -= waypoint_scatter_distance;
					break;
				}

				/*update_timer++;

				if (update_timer >= 5000)
				{
					trap_UpdateScreen();
					update_timer = 0;
				}*/

				VectorSet(org, startx, starty, startz);

				org[2]=FloorHeightAt(org);
				
				if (org[2] == -65536.0f)
				{
					/*while (startz > floor && startz > mapMins[2]-MAP_BOUNDS_OFFSET)
					{// We can skip some checks, until we get to our hit position...
						startz -= waypoint_scatter_distance;
						final_tests++;

						// Draw a nice little progress bar ;)
						aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

						update_timer++;

						if (update_timer >= 5000)
						{
							trap_UpdateScreen();
							update_timer = 0;
						}
					}*/

					startz = orig_startz;
					starty -= waypoint_scatter_distance;
					break;
				}

//				final_tests++;

				// Draw a nice little progress bar ;)
//				aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

				if (org[2] == 65536.0f)
				{// Marks a start-solid or on top of the sky... Skip...
					startz -= waypoint_scatter_distance;
					continue;
				}

				if (VectorDistance(org, last_org) < (waypoint_scatter_distance))
				{
					startz -= waypoint_scatter_distance;
					continue;
				}

				/*if (CheckSolid( org ) || AIMod_AutoWaypoint_Check_Slope(org))
				{// Bad slope angles here!
					startz -= waypoint_scatter_distance;
					continue;
				}*/

				// Raise node a little to add it...(for visibility)
				orig_floor = org[2];
				org[2] += 8;

				strcpy( last_node_added_string, va("^5Adding temp waypoint ^3%i ^5at ^7%f %f %f^5.", areas/*+areas2+areas3*/, org[0], org[1], org[2]) );

				VectorCopy( org, arealist[areas] );
				areas++;

				if (aw_floor_trace_hit_mover)
				{// Need to generate waypoints to the top/bottom of the mover...
					float temp_roof, temp_ground;
					vec3_t temp_org, temp_org2;
					
					VectorCopy(org, temp_org2);
					VectorCopy(org, temp_org);
					temp_org[2] += 24; // Start above the lift...
					temp_roof = RoofHeightAt(temp_org);

					VectorCopy(org, temp_org);
					temp_org[2] -= 24; // Start below the lift...
					temp_ground = GroundHeightAt(temp_org);

					temp_org2[2] = temp_roof;

					if (temp_roof < 64000 && HeightDistance(temp_org, temp_org2) >= 128)
					{// Looks like it goes up!
						VectorCopy(org, temp_org);
						temp_org[2] += 24;

						while (temp_org[2] <= temp_org2[2])
						{// Add waypoints all the way up!
							VectorCopy( temp_org, arealist[areas] );
							areas++;
							temp_org[2] += 24;
						}
					}

					temp_org2[2] = temp_ground;

					if (temp_roof > -64000 && HeightDistance(temp_org, temp_org2) >= 128)
					{// Looks like it goes up!
						VectorCopy(org, temp_org);
						temp_org[2] -= 24;

						while (temp_org[2] >= temp_org2[2])
						{// Add waypoints all the way up!
							VectorCopy( temp_org, arealist[areas] );
							areas++;
							temp_org[2] -= 24;
						}
					}
				}

				// Lower current org a back to where it was before raising it above...
				org[2] = orig_floor;
					
				VectorCopy(org, last_org);

				floor = org[2];
				current_floor = floor;

				startz -= waypoint_scatter_distance;

				while (startz > floor && startz > mapMins[2]-MAP_BOUNDS_OFFSET)
				{// We can skip some checks, until we get to our current floor height...
					startz -= waypoint_scatter_distance;
					/*final_tests++;

					// Draw a nice little progress bar ;)
					aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);

					update_timer++;

					if (update_timer >= 5000)
					{
						trap_UpdateScreen();
						update_timer = 0;
					}*/
				}
			}

			startz = orig_startz;
			starty -= waypoint_scatter_distance;
		}

		starty = orig_starty;
		startx -= waypoint_scatter_distance;

		/* */
		final_tests++;
	}

#ifdef __BOT_ACCEPT_ALL_AW_NODES__
	if (areas < 65536)
	{// UQ1: Can use them all!
		aw_percent_complete = 0.0f;
		strcpy( task_string3, va("^5Final (cleanup) pass. Building final waypoints...") );
		trap_UpdateScreen();

		total_areas = areas;

//#pragma omp parallel for
		for ( i = 0; i < areas; i++ )
		{
			vec3_t		area_org;
			short int	objNum[3] = { 0, 0, 0 };

			// Draw a nice little progress bar ;)
			aw_percent_complete = (float)((float)((float)i/(float)total_areas)*100.0f);
				
			update_timer++;

			if (update_timer >= 500)
			{
				trap_UpdateScreen();
				update_timer = 0;
			}

			area_org[0] = arealist[i][0];
			area_org[1] = arealist[i][1];
			area_org[2] = arealist[i][2];

			if (area_org[2] <= -65536.0f)
			{// This is a bad height!
				continue;
			}

			strcpy( last_node_added_string, va("^5Adding waypoint ^3%i ^5at ^7%f %f %f^5.", total_waypoints, area_org[0], area_org[1], area_org[2]) );
			Load_AddNode( area_org, 0, objNum, 0 );	//add the node
			total_waypoints++;
		}

		// Ladders...
		aw_total_waypoints = total_waypoints;
		aw_percent_complete = 0.0f;
		strcpy( task_string3, va("^5Looking for ladders...") );
		trap_UpdateScreen();

		for ( i = 0; i < total_waypoints; i++ )
		{
			// Draw a nice little progress bar ;)
			aw_percent_complete = (float)((float)((float)i/(float)total_waypoints)*100.0f);
				
			update_timer++;

			if (update_timer >= 100)
			{
				trap_UpdateScreen();
				update_timer = 0;
			}

			AIMod_AutoWaypoint_Check_For_Ladders( nodes[i].origin );
		}

		total_waypoints = aw_total_waypoints;
		// End Ladders...

		strcpy( task_string3, va("^5Saving the generated waypoints.") );
		trap_UpdateScreen();

		number_of_nodes = total_waypoints;
		AIMOD_NODES_SaveNodes_Autowaypointed();

		CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Waypoint database created successfully in ^3%.2f ^5seconds with ^7%i ^5waypoints.\n",
				 (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );

		strcpy( task_string3, va("^5Waypoint database created successfully in ^3%.2f ^5seconds with ^7%i ^5waypoints.\n", (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );
		trap_UpdateScreen();

		aw_percent_complete = 0.0f;
		strcpy( task_string3, va("^5Informing the server to load & test the new waypoints...") );
		trap_UpdateScreen();

		trap_SendConsoleCommand( "!loadnodes\n" );

		aw_percent_complete = 0.0f;
		strcpy( task_string3, va("^5Waypoint auto-generation is complete...") );
		trap_UpdateScreen();

		waypoint_scatter_distance = original_waypoint_scatter_distance;
		
		//AIMOD_Generate_Cover_Spots();

		B_Free(arealist);

		AIMod_AutoWaypoint_Free_Memory();

		aw_percent_complete = 0.0f;
		trap_UpdateScreen();

		return;
	}
#endif //__BOT_ACCEPT_ALL_AW_NODES__

	total_areas = areas;

	if (total_areas < 40000)
	{
		waypoint_distance_multiplier = 1.5f;
	}
	else if (total_areas < 64000)
	{
		waypoint_distance_multiplier = 2.0f;
	}
	else
	{
		waypoint_distance_multiplier = 2.5f;
	}

	if (total_areas < 40000)
	{
		area_skipper = 1;
	}
	else if (total_areas < 64000)
	{
		area_skipper = 2;
	}
	else if (total_areas < 96000)
	{
		area_skipper = 3;
	}
	else if (total_areas < 128000)
	{
		area_skipper = 4;
	}
	else
	{
		area_skipper = 5;
	}

#ifdef __AW_UNUSED__
	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Second (repair) pass. Adjusting waypoint positions...") );
	trap_UpdateScreen();

//#pragma omp parallel for
	for ( i = 0; i < areas; i++ )
	{
		vec3_t original_position = { arealist[i][0], arealist[i][1], arealist[i][2] };

		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)i/(float)total_areas)*100.0f);
		
		update_timer++;

		if (update_timer >= 500)
		{
			trap_UpdateScreen();
			update_timer = 0;
		}

		RepairPosition( arealist[i] );

		if (VectorDistance(fixed_position, original_position) > 0)
		{// New position.. Fix it!
			strcpy( last_node_added_string, va("^5Temp waypoint ^3%i ^5moved to ^7%f %f %f^5.", i, fixed_position[0], fixed_position[1], fixed_position[2]) );
			arealist[i][0] = fixed_position[0];
			arealist[i][1] = fixed_position[1];
			arealist[i][2] = fixed_position[2];
		}
	}
#endif //__AW_UNUSED__

	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Final (cleanup) pass. Building final waypoints...") );
	trap_UpdateScreen();

//#pragma omp parallel for
	for ( i = 0; i < areas; /*i+=area_skipper*/i++ )
	{
		vec3_t		area_org;
		short int	objNum[3] = { 0, 0, 0 };
		int j = 0;

		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)i/(float)total_areas)*100.0f);
				
		update_timer++;

		if (update_timer >= 100)
		{
			trap_UpdateScreen();
			update_timer = 0;
		}

		area_org[0] = arealist[i][0];
		area_org[1] = arealist[i][1];
		area_org[2] = arealist[i][2];

		if (area_org[2] <= -65536.0f)
		{// This is a bad height!
			continue;
		}

		{
			int num_found = 0;

			for (j = 0; j < areas; j++)
			{
				// Let's check if theres lots of others here, if so, we can disable this one...
				vec3_t area_org2;

				if (j == i)
					continue;

				area_org2[0] = arealist[j][0];
				area_org2[1] = arealist[j][1];
				area_org2[2] = arealist[j][2];

				if (area_org2[2] <= -65536.0f)
				{// This is a bad height!
					continue;
				}

				if (VectorDistance(area_org, area_org2) <= waypoint_scatter_distance*3/*waypoint_distance_multiplier*/)
				{
					num_found++;

					if (num_found > 8)
						break; // Found enough to skip the rest of the check...
				}
			}

			if (num_found > 8)
			{
				// Plenty of other nodes around here, we can disable this one (hopefully)...
				arealist[i][2] = -65536.0f; // Disable it!
				//CG_Printf( va("Node %i disabled as there are %i other options nearby.\n", i, num_found) );
				continue;
			}
		}

//#pragma omp parallel
/*		for (j = 0; j < number_of_nodes; j++)
		{
			vec3_t area_org2;

			area_org2[0] = nodes[j].origin[0];
			area_org2[1] = nodes[j].origin[1];
			area_org2[2] = nodes[j].origin[2];

			if (VectorDistance(area_org, area_org2) < waypoint_scatter_distance*waypoint_distance_multiplier)
			{
				bad = qtrue;
				break;
			}
		}

		if (bad)
		{
			continue;
		}*/

		strcpy( last_node_added_string, va("^5Adding waypoint ^3%i ^5at ^7%f %f %f^5.", total_waypoints, area_org[0], area_org[1], area_org[2]) );
		Load_AddNode( area_org, 0, objNum, 0 );	//add the node
		total_waypoints++;
	}

	// Ladders...
	aw_total_waypoints = total_waypoints;
	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Looking for ladders...") );
	trap_UpdateScreen();

	for ( i = 0; i < total_waypoints; i++ )
	{
		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)i/(float)total_waypoints)*100.0f);
				
		update_timer++;

		if (update_timer >= 100)
		{
			trap_UpdateScreen();
			update_timer = 0;
		}

		AIMod_AutoWaypoint_Check_For_Ladders( nodes[i].origin );
	}

	total_waypoints = aw_total_waypoints;
	// End Ladders...

	strcpy( task_string3, va("^5Saving the generated waypoints.\n") );
	trap_UpdateScreen();

	number_of_nodes = total_waypoints;
	AIMOD_NODES_SaveNodes_Autowaypointed();

	//AIMOD_Generate_Cover_Spots();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Waypoint database created successfully in ^3%.2f ^5seconds with ^7%i ^5waypoints.\n",
				 (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );

	strcpy( task_string3, va("^5Waypoint database created successfully in ^3%.2f ^5seconds with ^7%i ^5waypoints.\n", (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );
	trap_UpdateScreen();

	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Informing the server to load & test the new waypoints...") );
	trap_UpdateScreen();

	trap_SendConsoleCommand( "!loadnodes\n" );

	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Waypoint auto-generation is complete...") );
	trap_UpdateScreen();

	waypoint_scatter_distance = original_waypoint_scatter_distance;

	B_Free(arealist);

	AIMod_AutoWaypoint_Free_Memory();

	aw_percent_complete = 0.0f;
	trap_UpdateScreen();
}
#endif //__CUDA__

void AIMod_AutoWaypoint_OutdoorMethod( void )
{// Fast version using tracemaps.. Will only work on single layer levels...
	int			i;
	float		startx = -MAX_MAP_SIZE, starty = -MAX_MAP_SIZE, startz = -MAX_MAP_SIZE;
	float		orig_startx, orig_starty, orig_startz;
	int			areas = 0, total_waypoints = 0;
	intvec3_t	arealist[MAX_MAP_SIZE];
	float		temp;
	vec3_t		mapMins, mapMaxs;
	int			total_tests = 0, final_tests = 0;
	int			start_time = trap_Milliseconds();
	int			update_timer = 0;

	trap_Cvar_Set("cg_waypoint_render", "0");
	trap_UpdateScreen();
	trap_UpdateScreen();
	trap_UpdateScreen();

	AIMod_AutoWaypoint_Init_Memory();

	if (!BG_TraceMapLoaded())
	{
		if (!BG_LoadTraceMap(cgs.rawmapname, cg.mapcoordsMins, cg.mapcoordsMaxs ))
		{
			CG_GenerateTracemap();
		}
	}

	AIMod_GetMapBounts();

	VectorCopy(cg.mapcoordsMins, mapMins);
	VectorCopy(cg.mapcoordsMaxs, mapMaxs);

	startx = mapMins[0];
	starty = mapMins[1];
	startz = mapMins[2];

	if (startx > mapMaxs[0])
	{
		temp = mapMaxs[0];
		mapMaxs[0] = startx;
		startx = temp;
		mapMins[0] = startx;
	}

	if (starty > mapMaxs[1])
	{
		temp = mapMaxs[1];
		mapMaxs[1] = starty;
		starty = temp;
		mapMins[1] = starty;
	}

	if (startz > mapMaxs[2])
	{
		temp = mapMaxs[2];
		mapMaxs[2] = startz;
		startz = temp;
		mapMins[2] = startz;
	}

	orig_startx = startx;
	orig_starty = starty;
	orig_startz = startz;

//#pragma omp parallel
	while ( startx < mapMaxs[0] )
	{
//#pragma omp parallel
		while ( starty < mapMaxs[1] )
		{
			total_tests++;
			starty += outdoor_waypoint_scatter_distance;
		}

		starty = mapMins[1];
		startx += outdoor_waypoint_scatter_distance;
	}

	startx = orig_startx;
	starty = orig_starty;
	startz = orig_startz;

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^7Map bounds are ^3%f %f %f ^7to ^3%f %f %f^7.\n", mapMins[0], mapMins[1], mapMins[2], mapMaxs[0], mapMaxs[1], mapMaxs[2]) );
	strcpy( task_string1, va("^7Map bounds are ^3%f %f %f ^7to ^3%f %f %f^7.", mapMins[0], mapMins[1], mapMins[2], mapMaxs[0], mapMaxs[1], mapMaxs[2]) );
	trap_UpdateScreen();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^7Generating waypoints. This should not take too long...\n") );
	strcpy( task_string2, va("^7Generating waypoints. This should not take too long...") );
	trap_UpdateScreen();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Single pass. Please wait...\n") );
	strcpy( task_string3, va("^5Single pass. Please wait...") );
	trap_UpdateScreen();

//#pragma omp parallel
	while ( startx < mapMaxs[0] )
	{
//#pragma omp parallel
		while ( starty < mapMaxs[1] )
		{
			vec3_t org;
			qboolean bad = qfalse;

			final_tests++;

			// Draw a nice little progress bar ;)
			aw_percent_complete = (float)((float)((float)final_tests/(float)total_tests)*100.0f);
			//trap_UpdateScreen();
			
			update_timer++;

			if (update_timer >= 500)
			{
				trap_UpdateScreen();
				update_timer = 0;
			}

			VectorSet(org, startx, starty, startz);
			org[2]+=16;
			org[2]=BG_GetGroundHeightAtPoint(org);

			if (org[2] == -65536.0f)
			{
				starty += outdoor_waypoint_scatter_distance;
				continue;
			}

//#pragma omp parallel
			for (i = 0; i < areas; i++)
			{
				vec3_t area_org;

				area_org[0] = arealist[i][0];
				area_org[1] = arealist[i][1];
				area_org[2] = arealist[i][2];

				if (VectorDistance(area_org, org) < outdoor_waypoint_scatter_distance)
				{
					bad = qtrue;
					break;
				}
			}

			if (bad)
			{
				//trap_UpdateScreen();
				starty += outdoor_waypoint_scatter_distance;
				continue;
			}

			//CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Adding waypoint ^3%i ^7at ^7%f %f %f^5. (^3%.2f^5%% complete)\n", areas, org[0], org[1], org[2], (float)((float)((float)final_tests/(float)total_tests)*100.0f));
			strcpy( last_node_added_string, va("^5Adding waypoint ^3%i ^7at ^7%f %f %f^5.", areas, org[0], org[1], org[2]) );

			VectorCopy( org, arealist[areas] );
			areas++;
			starty += outdoor_waypoint_scatter_distance;
		}

		starty = mapMins[1];
		startx += outdoor_waypoint_scatter_distance;
	}

//#pragma omp parallel for
	for ( i = 0; i < areas; i++ )
	{
		vec3_t area_org;
		short int	objNum[3] = { 0, 0, 0 };

		area_org[0] = arealist[i][0];
		area_org[1] = arealist[i][1];
		area_org[2] = arealist[i][2];

		Load_AddNode( area_org, 0, objNum, 0 );	//add the node
		total_waypoints++;
	}

	// Ladders...
	aw_total_waypoints = total_waypoints;
	aw_percent_complete = 0.0f;
	strcpy( task_string3, va("^5Looking for ladders...") );
	trap_UpdateScreen();

	for ( i = 0; i < total_waypoints; i++ )
	{
		// Draw a nice little progress bar ;)
		aw_percent_complete = (float)((float)((float)i/(float)total_waypoints)*100.0f);
				
		update_timer++;

		if (update_timer >= 200)
		{
			trap_UpdateScreen();
			update_timer = 0;
		}

		AIMod_AutoWaypoint_Check_For_Ladders( nodes[i].origin );
	}

	total_waypoints = aw_total_waypoints;
	// End Ladders...

	//total_waypoints--;
	AIMOD_NODES_SaveNodes_Autowaypointed();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Waypoint database created successfully in ^3%f ^5seconds with ^7%i ^5waypoints.\n",
				 (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );

	strcpy( task_string3, va("^5Waypoint database created successfully in ^3%f ^5seconds with ^7%i ^5waypoints.\n", (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );

	trap_UpdateScreen();

	aw_percent_complete = 0.0f;
	trap_UpdateScreen();

	trap_SendConsoleCommand( "!loadnodes\n" );

	AIMod_AutoWaypoint_Free_Memory();
}

qboolean wp_memory_initialized = qfalse;

void AIMod_AutoWaypoint_Init_Memory ( void )
{
	//if (wp_memory_initialized == qfalse)
	{
		nodes = B_Alloc( (sizeof(node_t)+1)*MAX_NODES );
		wp_memory_initialized = qtrue;
	}
}

void AIMod_AutoWaypoint_Free_Memory ( void )
{
	//if (wp_memory_initialized == qtrue)
	{
		B_Free(nodes);
		wp_memory_initialized = qfalse;
	}
}

void AIMod_AutoWaypoint_Clean ( void )
{
	char	str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 )
	{
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^7Usage:\n" );
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^3/awc <method>^5.\n" );
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^5Available methods are:\n" );
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^3\"relink\" ^5- Just do relinking.\n");
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^3\"clean\" ^5- Do a full clean.\n");
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^3\"multipass\" ^5- Do a multi-pass full clean (max optimize).\n");
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^3\"extra\" ^5- Do a full clean (but remove more - good if the number is still too high after optimization).\n");
		trap_UpdateScreen();
		return;
	}

	trap_Argv( 1, str, sizeof(str) );
	
	if ( Q_stricmp( str, "relink") == 0 )
	{
		AIMod_AutoWaypoint_Cleaner(qtrue, qtrue, qtrue, qfalse, qfalse, qfalse);
	}
	else if ( Q_stricmp( str, "clean") == 0 )
	{
		AIMod_AutoWaypoint_Cleaner(qtrue, qtrue, qfalse, qfalse, qfalse, qfalse);
	}
	else if ( Q_stricmp( str, "multipass") == 0 )
	{
		AIMod_AutoWaypoint_Cleaner(qtrue, qtrue, qfalse, qtrue, qfalse, qfalse);
	}
	else if ( Q_stricmp( str, "extra") == 0 )
	{
		AIMod_AutoWaypoint_Cleaner(qtrue, qtrue, qfalse, qtrue, qfalse, qtrue);
	}
}

/* */
/*void
AIMod_AutoWaypoint ( void )
{
	// UQ1: Check if we have an SSE CPU.. It can speed up our memory allocation by a lot!
	if (!CPU_CHECKED)
		UQ_Get_CPU_Info();

	AIMod_AutoWaypoint_StandardMethod();

	if (number_of_nodes < 32768)
		AIMod_AutoWaypoint_Cleaner(qtrue, qtrue, qfalse);
	else
		AIMod_AutoWaypoint_Optimizer();

	//while (optimize_again)
	//	AIMod_AutoWaypoint_Optimizer(qtrue);
}*/

/* */
void
AIMod_AutoWaypoint ( void )
{
	char	str[MAX_TOKEN_CHARS];

	// UQ1: Check if we have an SSE CPU.. It can speed up our memory allocation by a lot!
	if (!CPU_CHECKED)
		UQ_Get_CPU_Info();

	if ( trap_Argc() < 2 )
	{
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^7Usage:\n" );
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^3/autowaypoint <method> ^5or ^3/aw <method>^5.\n" );
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^5Available methods are:\n" );
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^3\"standard\" ^5- For standard multi-level maps.\n");
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^3\"noclean\" ^5- For standard multi-level maps (with no cleaning passes).\n");
		CG_Printf( "^4*** ^3AUTO-WAYPOINTER^4: ^3\"outdoor_only\" ^5- For standard single level maps.\n");
		trap_UpdateScreen();
		return;
	}

	//AIMod_AutoWaypoint_Init_Memory();

	trap_Argv( 1, str, sizeof(str) );
	
	if ( Q_stricmp( str, "standard") == 0 )
	{
		AIMod_AutoWaypoint_StandardMethod();
	}
	else if ( Q_stricmp( str, "noclean") == 0 )
	{
		AIMod_AutoWaypoint_StandardMethod();
		return;
	}
	else if ( Q_stricmp( str, "outdoor_only") == 0 )
	{
		AIMod_AutoWaypoint_OutdoorMethod();
	}

	AIMod_AutoWaypoint_Cleaner(qtrue, qtrue, qfalse, qtrue, qtrue, qtrue);
}

//
// Autowaypoint Optimizer...
//

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
GetFCost ( centity_t *bot, int to, int num, int parentNum, float *gcost )
{
	float	gc = 0;
	float	hc = 0;
	vec3_t	v;
	
	if ( gcost[num] == -1 )
	{
		if ( parentNum != -1 )
		{
			gc = gcost[parentNum];
			VectorSubtract( nodes[num].origin, nodes[parentNum].origin, v );
			gc += VectorLength( v );

			gc += HeightDistance(nodes[num].origin, nodes[parentNum].origin) * 4;

			// UQ1: Make WATER, BARB WIRE, and ICE nodes cost more!
			if (nodes[num].type & NODE_WATER)
			{// Huge cost for water nodes!
				//gc+=(gc*100);
				gc = 65000.0f;
			}
			else if (nodes[num].type & NODE_ICE)
			{// Huge cost for ice(slick) nodes!
				//gc+=(gc*100);
				gc = 65000.0f;
			}

			if (gc > 65000)
				gc = 65000.0f;

			/*if (nodes[num].type & NODE_COVER)
			{// Encorage the use of cover spots!
				gc = 0.0f;
			}*/
		}

		gcost[num] = gc;
	}
	else
	{
		gc = gcost[num];
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

#define MAX_PATHLIST_NODES  4096/*8192*//*MAX_NODES*///1024	// MAX_NODES??

int
CreatePathAStar ( centity_t *bot, int from, int to, int *pathlist )
{
	int	openlist[MAX_NODES + 2];												//add 1 because it's a binary heap, and they don't use 0 - 1 is the first used index
	float		gcost[MAX_NODES+1];
	int			fcost[MAX_NODES+1];
	char			list[MAX_NODES+1];														//0 is neither, 1 is open, 2 is closed - char because it's the smallest data type
	int	parent[MAX_NODES+1];
	int	numOpen = 0;
	int	atNode, temp, newnode = -1;
	qboolean	found = qfalse;
	int			count = -1;
	float		gc;
	int			i, j, u, v, m;
	vec3_t		vec;

	/*for (i = 0; i < MAX_NODES; i++)
	{
		gcost[i] = -1.0f;
		fcost[i] = 0;
		openlist[i] = 0;
		parent[i] = 0;
		list[i] = 0;
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

	openlist[MAX_NODES+1] = 0;

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

				if (nodes[newnode].objectNum[0] == 1)
					continue; // Skip water/ice disabled node!

				//if (newnode > number_of_nodes)
				//	continue;

				/*if ( nodes[atNode].links[i].flags & PATH_BLOCKED )
				{																		//if this path is blocked, skip it
					continue;
				}

				if ( nodes[atNode].links[i].flags & PATH_DANGER )
				{																		//if this path is blocked, skip it
					continue;
				}*/

/*#ifdef __VEHICLES__
				if
				(
					bot->client &&
					(bot->client->ps.eFlags & EF_VEHICLE) &&
					(nodes[atNode].links[i].flags & PATH_NOTANKS)
				)
				{																		//if this path is blocked, skip it
					continue;
				}

				if
				(
					bot->client &&
					(bot->client->ps.eFlags & EF_VEHICLE) &&
					!(nodes[nodes[atNode].links[i].targetNode].type & NODE_LAND_VEHICLE)
				)
				{
					continue;															// Tank driving bots should only use tank nodes!
				}

				if ( bot->client 
					&& (bot->client->ps.eFlags & EF_VEHICLE)  
					&& !(nodes[bot->current_node].type & NODE_ICE) )
				{
					if
					(
						bot->client &&
						(bot->client->ps.eFlags & EF_VEHICLE) &&
						(nodes[nodes[atNode].links[i].targetNode].type & NODE_ICE)
					)
					{
						continue;														// Tank driving bots should NEVER go onto ice!
					}
				}
#endif //__VEHICLES__*/

				if ( list[newnode] == 2 )
				{																		//if this node is on the closed list, skip it
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
				return -1; // UQ1: Added to stop crash if path is too long for the memory allocation...

			pathlist[count++] = temp;							//add the node to the pathlist and increment the count
			temp = parent[temp];								//move to the parent of this node to continue the path
		}

		pathlist[count++] = from;								//add the beginning node to the end of the pathlist
		return ( count );
	}

	return ( count );											//return the number of nodes in the path, -1 if not found
}

qboolean wp_optimize_memory_initialized = qfalse;

void AIMod_AutoWaypoint_Optimize_Init_Memory ( void )
{
	//if (wp_memory_initialized == qfalse)
	{
		optimized_nodes = B_Alloc( (sizeof(node_t)+1)*MAX_NODES );
		wp_optimize_memory_initialized = qtrue;
	}
}

void AIMod_AutoWaypoint_Optimize_Free_Memory ( void )
{
	//if (wp_memory_initialized == qtrue)
	{
		B_Free(optimized_nodes);
		wp_optimize_memory_initialized = qfalse;
	}
}

/* */
qboolean
Optimize_AddNode ( vec3_t origin, int fl, short int *ents, int objFl )
{
	if ( optimized_number_of_nodes + 1 > MAX_NODES )
	{
		return ( qfalse );
	}

	VectorCopy( origin, optimized_nodes[optimized_number_of_nodes].origin );	//set the node's position

	nodes[optimized_number_of_nodes].type = fl;						//set the flags (NODE_OBJECTIVE, for example)
	nodes[optimized_number_of_nodes].objectNum[0] = ents[0];			//set any map objects associated with this node
	nodes[optimized_number_of_nodes].objectNum[1] = ents[1];			//only applies to objects linked to the unreachable flag
	nodes[optimized_number_of_nodes].objectNum[2] = ents[2];
	nodes[optimized_number_of_nodes].objFlags = objFl;				//set the objective flags
	optimized_number_of_nodes++;
	return ( qtrue );
}

qboolean Is_Waypoint_Entity ( int eType )
{
	switch (eType)
	{
	//case ET_GENERAL:
	//case ET_PLAYER:
	case ET_ITEM:
	case ET_MOVER:
	case ET_PORTAL:
	case ET_SPEAKER:
	case ET_PUSH_TRIGGER:
	case ET_TELEPORT_TRIGGER:
	case ET_OID_TRIGGER:			// DHM - Nerve :: Objective Info Display
	case ET_EXPLOSIVE_INDICATOR:	// NERVE - SMF
	case ET_EXPLOSIVE:			// brush that will break into smaller bits when damaged
	case ET_EF_SPOTLIGHT:
	case ET_ALARMBOX:
	case ET_TRAP:
	case ET_MOVERSCALED:
	case ET_CONSTRUCTIBLE_INDICATOR:
	case ET_CONSTRUCTIBLE:
	case ET_CONSTRUCTIBLE_MARKER:
	case ET_WAYPOINT:
	case ET_TANK_INDICATOR:
	case ET_TANK_INDICATOR_DEAD:
	case ET_BOTGOAL_INDICATOR:
	case ET_MG42_BARREL:			// Arnout: MG42 barrel
	case ET_TRIGGER_MULTIPLE:
	case ET_TRIGGER_FLAGONLY:
	case ET_TRIGGER_FLAGONLY_MULTIPLE:
	case ET_AAGUN:
	case ET_CABINET_H:
	case ET_CABINET_A:
	case ET_HEALER:
	case ET_SUPPLIER:
	case ET_LANDMINE_HINT:		// Gordon: landmine hint for botsetgoalstate filter
	case ET_ATTRACTOR_HINT:		// Gordon: attractor hint for botsetgoalstate filter
	case ET_SNIPER_HINT:			// Gordon: sniper hint for botsetgoalstate filter
	case ET_LANDMINESPOT_HINT:	// Gordon: landminespot hint for botsetgoalstate filter
	case ET_DEFENCEPOINT_HINT:	// Unique1: defence point hint.
	case ET_COMMANDMAP_MARKER:
	case ET_WOLF_OBJECTIVE:
#ifdef __ETE__
	case ET_FLAG:
	case ET_ASSOCIATED_SPAWNAREA:
	case ET_UNASSOCIATED_SPAWNAREA:
	case ET_AMMO_CRATE:
	case ET_HEALTH_CRATE:
	case ET_SANDBAG:
	case ET_CONSTRUCTIBLE_SANDBAGS:
	case ET_MG_NEST:
	case ET_CONSTRUCTIBLE_MG_NEST:
#endif //__ETE__
#ifdef __VEHICLES__
	case ET_VEHICLE:
#endif //__VEHICLES__
#ifdef __NPC__
	case ET_NPC:
#endif //__NPC__
	case ET_SPAWNPOINT:
		return qtrue;
		break;
	default:
		break;
	}

	return qfalse;
}

int ClosestNodeTo(vec3_t origin, qboolean isEntity)
{
	int		i;
	float	AREA_SEPERATION = DEFAULT_AREA_SEPERATION;
	float	closest_dist = AREA_SEPERATION*1.5;
	int		closest_node = -1;

	for (i = 0; i < number_of_nodes; i++)
	{
		float dist = VectorDistance(nodes[i].origin, origin);

		if (dist >= closest_dist)
			continue;

		if (nodes[i].objectNum[0] == 1)
			continue; // Skip water/ice disabled node!

		if (isEntity)
		{
			if (HeightDistance(origin, nodes[i].origin) > 64)
				continue;
		}

		closest_dist = dist;
		closest_node = i;
	}

	return closest_node;
}

qboolean bad_surface = qfalse;
int		aw_num_bad_surfaces = 0;

/* */
int
AI_PM_GroundTrace ( vec3_t point, int clientNum )
{
	//vec3_t	playerMins = {-18, -18, -24};
	//vec3_t	playerMaxs = {18, 18, 48};
	trace_t trace;
	vec3_t	endpoint, startpoint;
	
	VectorCopy( point, endpoint );
	endpoint[2] -= 128.0f;
	VectorCopy( point, startpoint );
	startpoint[2] += 16.0f;

	bad_surface = qfalse;

	//CG_Trace( &trace, startpoint, playerMins, playerMaxs, endpoint, clientNum, (MASK_PLAYERSOLID | MASK_WATER) & ~CONTENTS_BODY/*MASK_SHOT*/ );
	CG_Trace( &trace, startpoint, NULL, NULL, endpoint, clientNum, (MASK_PLAYERSOLID | MASK_WATER) & ~CONTENTS_BODY/*MASK_SHOT*/ );

	if ( (trace.surfaceFlags & SURF_NODRAW) && (trace.surfaceFlags & SURF_NOMARKS) && !Waypoint_FloorSurfaceOK(trace.surfaceFlags) )
		bad_surface = qtrue;

	return (trace.contents);
}

void AIMOD_AI_InitNodeContentsFlags ( void )
{
	int i;

	for (i = 0; i < number_of_nodes; i++)
	{
		int contents = AI_PM_GroundTrace(nodes[i].origin, -1);

		nodes[i].objectNum[0] = 0;
		nodes[i].objFlags = 0;

		if ((contents & CONTENTS_LAVA) || (contents & CONTENTS_SLIME))
		{// ICE/BARB WIRE, avoid if possible!
			if (!(nodes[i].type & NODE_ICE))
				nodes[i].type |= NODE_ICE;
		}

		if (contents & CONTENTS_WATER)
		{
			nodes[i].type |= NODE_WATER;
		}

		if (bad_surface)
		{// Made code also check for sky surfaces, should be able to remove a lot of crappy waypoints on the skybox!
			nodes[i].objectNum[0] = 1;
			aw_num_bad_surfaces++;
		}
	}
}

/* */
void
AIMod_AutoWaypoint_Optimizer ( void )
{
	qboolean quiet = qfalse;
	qboolean null_links_only = qfalse;
	int i = 0, j = 0, k = 0, l = 0;//, m = 0;
	int	total_calculations = 0;
	int	calculations_complete = 0;
	int	*areas;//[16550];
	int num_areas = 0;
	float map_size;
	vec3_t mapMins, mapMaxs;
	float temp;
	float AREA_SEPERATION = DEFAULT_AREA_SEPERATION;
	int screen_update_timer = 0;
	int entities_start = 0;
	char	str[MAX_TOKEN_CHARS];
	int	node_disable_ticker = 0;
	int	num_disabled_nodes = 0;
	int num_nolink_nodes = 0;
	int	node_disable_ratio = 2;
	qboolean	bad_surfaces_only = qfalse;
	qboolean	noiceremove = qfalse;
	qboolean	nowaterremove = qfalse;
	int	start_wp_total = 0;

	aw_num_bad_surfaces = 0;

	trap_Cvar_Set("cg_waypoint_render", "0");
	trap_UpdateScreen();
	trap_UpdateScreen();
	trap_UpdateScreen();

	// UQ1: start - First handle the command line options...
	trap_Argv( 1, str, sizeof(str) );

	if (!quiet && str[0])
	{// Use player specified area seperation...
		float area_sep = 0.0f;

		if (!Q_stricmp(str, "help") || !Q_stricmp(str, "commands"))
		{
			CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^7NOTE: The following command line options are available...\n");
			CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^3badsurfsonly  ^4- ^5Remove waypoints on bad surfaces only (includes sky, ice, water).\n");
			CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^3nulllinksonly ^4- ^5Remove waypoints with no links to nearby waypoints (can be used with badsurfsonly).\n");
			CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^3noiceremove   ^4- ^5Do not automaticly remove some waypoints on ice (can be used with badsurfsonly).\n");
			CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^3nowaterremove ^4- ^5Do not automaticly remove some waypoints on/in water (can be used with badsurfsonly).\n");
			CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^3#             ^4- ^5Use specified area scatter distance (default is ^7%i^5).\n", (int)DEFAULT_AREA_SEPERATION);
			return;
		}

		if (!Q_stricmp(str, "badsurfsonly"))
			bad_surfaces_only = qtrue;
		else if (!Q_stricmp(str, "noiceremove"))
			noiceremove = qtrue;
		else if (!Q_stricmp(str, "nowaterremove"))
			nowaterremove = qtrue;
		else if (!Q_stricmp(str, "nulllinksonly"))
			null_links_only = qtrue;
		else
			area_sep = atof(str);

		trap_Argv( 2, str, sizeof(str) );

		if (str[0])
		{// Use player specified area seperation...
			if (!Q_stricmp(str, "badsurfsonly"))
				bad_surfaces_only = qtrue;
			else if (!Q_stricmp(str, "noiceremove"))
				noiceremove = qtrue;
			else if (!Q_stricmp(str, "nowaterremove"))
				nowaterremove = qtrue;
			else if (!Q_stricmp(str, "nulllinksonly"))
				null_links_only = qtrue;
			else
				area_sep = atof(str);

			trap_Argv( 3, str, sizeof(str) );

			if (str[0])
			{// Use player specified area seperation...
				if (!Q_stricmp(str, "badsurfsonly"))
					bad_surfaces_only = qtrue;
				else if (!Q_stricmp(str, "noiceremove"))
					noiceremove = qtrue;
				else if (!Q_stricmp(str, "nowaterremove"))
					nowaterremove = qtrue;
				else if (!Q_stricmp(str, "nulllinksonly"))
					null_links_only = qtrue;
				else
					area_sep = atof(str);

				trap_Argv( 4, str, sizeof(str) );

				if (str[0])
				{// Use player specified area seperation...
					if (!Q_stricmp(str, "badsurfsonly"))
						bad_surfaces_only = qtrue;
					else if (!Q_stricmp(str, "noiceremove"))
						noiceremove = qtrue;
					else if (!Q_stricmp(str, "nowaterremove"))
						nowaterremove = qtrue;
					else if (!Q_stricmp(str, "nulllinksonly"))
						null_links_only = qtrue;
					else
						area_sep = atof(str);

					trap_Argv( 5, str, sizeof(str) );

					if (str[0])
					{// Use player specified area seperation...
						if (!Q_stricmp(str, "badsurfsonly"))
							bad_surfaces_only = qtrue;
						else if (!Q_stricmp(str, "noiceremove"))
							noiceremove = qtrue;
						else if (!Q_stricmp(str, "nowaterremove"))
							nowaterremove = qtrue;
						else if (!Q_stricmp(str, "nulllinksonly"))
							null_links_only = qtrue;
						else
							area_sep = atof(str);
					}
				}
			}
		}

		if (area_sep != 0)
			AREA_SEPERATION = area_sep;
	}
	else if (!quiet)
	{
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^7NOTE: The following command line options are available...\n");
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^3badsurfsonly  ^4- ^5Remove waypoints on bad surfaces only (includes sky, ice, water).\n");
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^3nulllinksonly ^4- ^5Remove waypoints with no links to nearby waypoints (can be used with badsurfsonly).\n");
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^3noiceremove   ^4- ^5Do not automaticly remove some waypoints on ice (can be used with badsurfsonly).\n");
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^3nowaterremove ^4- ^5Do not automaticly remove some waypoints on/in water (can be used with badsurfsonly).\n");
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^3#             ^4- ^5Use specified area scatter distance (default is ^7%i^5).\n", (int)DEFAULT_AREA_SEPERATION);
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^7No command line specified, using defaults...\n");
	}
	// UQ1: end - First handle the command line options...

	// UQ1: Check if we have an SSE CPU.. It can speed up our memory allocation by a lot!
	if (!CPU_CHECKED)
		UQ_Get_CPU_Info();

	AIMod_AutoWaypoint_Init_Memory();
	AIMod_AutoWaypoint_Optimize_Init_Memory();

	if (number_of_nodes > 0)
	{// UQ1: Init nodes list!
		number_of_nodes = 0; 
		optimized_number_of_nodes = 0;
		
		if (SSE_CPU)
		{
			sse_memset( nodes, 0, ((sizeof(node_t)+1)*MAX_NODES)/8 );
			sse_memset( optimized_nodes, 0, ((sizeof(node_t)+1)*MAX_NODES)/8 );
		}
		else
		{
			memset( nodes, 0, ((sizeof(node_t)+1)*MAX_NODES) );
			memset( optimized_nodes, 0, ((sizeof(node_t)+1)*MAX_NODES) );
		}
	}

	AIMOD_NODES_LoadNodes();

	if (number_of_nodes <= 0)
	{
		AIMod_AutoWaypoint_Free_Memory();
		AIMod_AutoWaypoint_Optimize_Free_Memory();
		return;
	}

	start_wp_total = number_of_nodes;

	areas = B_Alloc( (sizeof(int)+1)*512000 );

	/*AIMod_GetMapBounts( mapMins, mapMaxs );

	if (mapMaxs[0] < mapMins[0])
	{
		temp = mapMins[0];
		mapMins[0] = mapMaxs[0];
		mapMaxs[0] = temp;
	}

	if (mapMaxs[1] < mapMins[1])
	{
		temp = mapMins[1];
		mapMins[1] = mapMaxs[1];
		mapMaxs[1] = temp;
	}

	if (mapMaxs[2] < mapMins[2])
	{
		temp = mapMins[2];
		mapMins[2] = mapMaxs[2];
		mapMaxs[2] = temp;
	}*/

	//AIMod_GetMapBounts( mapMins, mapMaxs );
	AIMod_GetMapBounts();

	VectorCopy(cg.mapcoordsMins, mapMins);
	VectorCopy(cg.mapcoordsMaxs, mapMaxs);

	if (mapMaxs[0] < mapMins[0])
	{
		temp = mapMins[0];
		mapMins[0] = mapMaxs[0];
		mapMaxs[0] = temp;
	}

	if (mapMaxs[1] < mapMins[1])
	{
		temp = mapMins[1];
		mapMins[1] = mapMaxs[1];
		mapMaxs[1] = temp;
	}

	if (mapMaxs[2] < mapMins[2])
	{
		temp = mapMins[2];
		mapMins[2] = mapMaxs[2];
		mapMaxs[2] = temp;
	}

	map_size = VectorDistance(mapMins, mapMaxs);

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^7Map bounds (^7%f %f %f ^5by ^7%f %f %f^5).\n", mapMins[0], mapMins[1], mapMins[2], mapMaxs[0], mapMaxs[1], mapMaxs[2]) );

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^7Optimizing waypoint list...\n") );
	strcpy( task_string1, va("^7Optimizing waypoint list....") );
	trap_UpdateScreen();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^7This should not take too long...\n") );
	strcpy( task_string2, va("^7This should not take too long...") );
	trap_UpdateScreen();

	if (!bad_surfaces_only)
	{
		CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5First pass. Creating area lists... Please wait...\n") );
		strcpy( task_string3, va("^5First pass. Creating area lists... Please wait...") );
		trap_UpdateScreen();
	}

	// UQ1: Set node types..
	AIMOD_AI_InitNodeContentsFlags();

	/*if (number_of_nodes > 128000)
		node_disable_ratio = 24;
	else if (number_of_nodes > 96000)
		node_disable_ratio = 22;
	else if (number_of_nodes > 72000)
		node_disable_ratio = 20;
	else if (number_of_nodes > 64000)
		node_disable_ratio = 18;
	else if (number_of_nodes > 48000)
		node_disable_ratio = 16;
	else*/ if (number_of_nodes > 32000)
		node_disable_ratio = 8;
	else if (number_of_nodes > 24000)
		node_disable_ratio = 6;
	else if (number_of_nodes > 16000)
		node_disable_ratio = 4;
	else if (number_of_nodes > 8000)
		node_disable_ratio = 2;

	// Disable some ice/water ndoes...
	for (i = 0; i < number_of_nodes; i++)
	{
		if (nodes[i].enodenum <= 0)
		{// Remove all waypoints without any links...
			nodes[i].objectNum[0] = 1;
			num_nolink_nodes++;
			continue;
		}

		if ((!nowaterremove && (nodes[i].type & NODE_WATER)) || (!noiceremove && (nodes[i].type & NODE_ICE)))
		{
			if (node_disable_ticker < node_disable_ratio)
			{// Remove 2 out of every (node_disable_ratio)..
				nodes[i].objectNum[0] = 1;
				num_disabled_nodes++;
				node_disable_ticker++;
			}
			else
			{
				node_disable_ticker = 0;
			}
		}
	}

	CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Disabled ^3%i^5 water/ice waypoints.\n", num_disabled_nodes);
	CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Disabled ^3%i^5 bad surfaces.\n", aw_num_bad_surfaces);
	CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Disabled ^3%i^5 waypoints without links.\n", num_nolink_nodes);

	if (num_disabled_nodes == 0 && aw_num_bad_surfaces == 0 && num_nolink_nodes == 0)
	{// No point relinking and saving...
		B_Free(areas);
		AIMod_AutoWaypoint_Free_Memory();
		AIMod_AutoWaypoint_Optimize_Free_Memory();
		return;
	}

	if (!bad_surfaces_only && !null_links_only )
	{
		i = mapMins[0]-MAP_BOUNDS_OFFSET;
		j = mapMins[1]-MAP_BOUNDS_OFFSET;
		k = mapMins[2]-MAP_BOUNDS_OFFSET;

		while (i < mapMaxs[0]+MAP_BOUNDS_OFFSET)
		{
			while (j < mapMaxs[1]+MAP_BOUNDS_OFFSET)
			{
				while (k < mapMaxs[2]+MAP_BOUNDS_OFFSET)
				{
					total_calculations++;

					k += AREA_SEPERATION;
				}

				j += AREA_SEPERATION/**1.5*/;
				k = mapMins[2]-MAP_BOUNDS_OFFSET;
			}

			i += AREA_SEPERATION/**1.5*/;
			j = mapMins[1]-MAP_BOUNDS_OFFSET;
		}

		//CG_Printf("Total calcs is %i\n", total_calculations);

		i = mapMins[0]-MAP_BOUNDS_OFFSET;
		j = mapMins[1]-MAP_BOUNDS_OFFSET;
		k = mapMins[2]-MAP_BOUNDS_OFFSET;
	
		calculations_complete = 0;

		while (i < mapMaxs[0]+MAP_BOUNDS_OFFSET)
		{
			int num_nodes_added = 0;

			while (j < mapMaxs[1]+MAP_BOUNDS_OFFSET)
			{
				while (k < mapMaxs[2]+MAP_BOUNDS_OFFSET)
				{
					vec3_t		area_org;
					int			closest_node = -1;
					qboolean	skip = qfalse;

					calculations_complete++;

					// Draw a nice little progress bar ;)
					aw_percent_complete = (float)((float)((float)(calculations_complete)/(float)(total_calculations))*100.0f);

					num_nodes_added++;
				
					if (num_nodes_added > 20)
					{
						trap_UpdateScreen();
						num_nodes_added = 0;
					}

					area_org[0] = i;
					area_org[1] = j;
					area_org[2] = k;

					closest_node = ClosestNodeTo(area_org, qfalse);

					if (closest_node == -1)
					{
						//CG_Printf("No closest node at %f %f %f.\n", area_org[0], area_org[1], area_org[2]);
						k += AREA_SEPERATION;
						continue;
					}

					for (l = 0; l < num_areas; l++)
					{
						if (areas[l] == closest_node)
						{
							//CG_Printf("area %i skipped\n", l);
							skip = qtrue;
							break;
						}
					}

					if (!skip)
					{
						strcpy( last_node_added_string, va("^5Adding area ^3%i ^5at ^7%f %f %f^5.", num_areas, nodes[closest_node].origin[0], nodes[closest_node].origin[1], nodes[closest_node].origin[2]) );
						areas[num_areas] = closest_node;
						num_areas++;
					}

					k += AREA_SEPERATION;
				}

				j += AREA_SEPERATION/**1.5*/;
				k = mapMins[2]-MAP_BOUNDS_OFFSET;
			}

			i += AREA_SEPERATION/**1.5*/;
			j = mapMins[1]-MAP_BOUNDS_OFFSET;
		}

		CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Second pass. Adding goal area entities... Please wait...\n") );
		strcpy( task_string3, va("^5Second pass. Adding goal area entities... Please wait...") );
		trap_UpdateScreen();

		entities_start = num_areas;

		for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++)
		{
			centity_t	*cent = &cg_entities[i];
			int				closest_node = -1;
			qboolean	skip = qfalse;

			// Draw a nice little progress bar ;)
			aw_percent_complete = (float)((float)((float)(i-MAX_CLIENTS)/(float)(MAX_GENTITIES-MAX_CLIENTS))*100.0f);

			screen_update_timer++;
				
			if (screen_update_timer > 10)
			{
				trap_UpdateScreen();
				screen_update_timer = 0;
			}

			if (!cent)
				continue;

			if (!Is_Waypoint_Entity(cent->currentState.eType))
				continue;

			closest_node = ClosestNodeTo(cent->currentState.origin, qtrue);

			if (closest_node == -1)
			{
				continue;
			}

			for (l = 0; l < num_areas; l++)
			{
				if (areas[l] == closest_node)
				{
					skip = qtrue;
					break;
				}
			}

			if (!skip)
			{
				strcpy( last_node_added_string, va("^5Adding (entity) area ^3%i ^5at ^7%f %f %f^5.", num_areas, nodes[closest_node].origin[0], nodes[closest_node].origin[1], nodes[closest_node].origin[2]) );
				areas[num_areas] = closest_node;
				num_areas++;
			}
		}

		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Generated ^3%i^5 areas.\n", num_areas);

		CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Final pass. Creating waypoints... Please wait...\n") );
		strcpy( task_string3, va("^5Final pass. Creating waypoints... Please wait...") );
		trap_UpdateScreen();

		total_calculations = num_areas*num_areas;
		calculations_complete = 0;

		for (i = 0; i < num_areas; i++)
		{
			vec3_t		org;
			int				num_nodes_added = 0;

			VectorCopy(nodes[areas[i]].origin, org);
		
			for (j = 0; j < num_areas; j++)
			{
				vec3_t		org2;
				int				pathlist[MAX_NODES];
				int				pathsize = 0;
				int				node = -1;

				calculations_complete++;

				// Draw a nice little progress bar ;)
				aw_percent_complete = (float)((float)((float)(calculations_complete)/(float)(total_calculations))*100.0f);

				num_nodes_added++;
				
				if (num_nodes_added > 50)
				{
					trap_UpdateScreen();
					num_nodes_added = 0;
				}

				if (i == j)
					continue;

				VectorCopy(nodes[areas[j]].origin, org2);

				if (i >= entities_start || j >= entities_start)
				{// Extra checking for entities!
					if (VectorDistance(org, org2) > AREA_SEPERATION*8/*6*//*2*//*8*/)
						continue;
				}
				else
				{
					if (VectorDistance(org, org2) > AREA_SEPERATION*1.6/*2*//*8*/)
						continue;
				}

				pathsize = CreatePathAStar( NULL, areas[i], areas[j], pathlist );
						
				if (pathsize > 0)
				{
					node = areas[i];	//pathlist is in reverse order

					while (node != -1 && pathsize > 0)
					{
						short int		objNum[3] = { 0, 0, 0 };
						qboolean	skip = qfalse;

						if (!skip && nodes[node].objEntity != 1)
						{
							num_nodes_added++;

							if (num_nodes_added > 100)
							{
								strcpy( last_node_added_string, va("^5Adding waypoint ^3%i ^5at ^7%f %f %f^5.", optimized_number_of_nodes, nodes[node].origin[0], nodes[node].origin[1], nodes[node].origin[2]) );
								trap_UpdateScreen();
								num_nodes_added = 0;
							}

							Optimize_AddNode( nodes[node].origin, 0, objNum, 0 );	//add the node
							nodes[node].objEntity = 1;
						}

						node = pathlist[pathsize - 1];	//pathlist is in reverse order
						pathsize--;
					}
				}
			}
		}
	}
	else
	{// bad_surfaces_only
		CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Final pass. Creating waypoints... Please wait...\n") );
		strcpy( task_string3, va("^5Creating waypoints... Please wait...") );
		trap_UpdateScreen();

		total_calculations = number_of_nodes;
		calculations_complete = 0;

		for (i = 0; i < number_of_nodes; i++)
		{
			int				num_nodes_added = 0;
			short int		objNum[3] = { 0, 0, 0 };

			if (nodes[i].objEntity != 1 && nodes[i].objectNum[0] != 1)
			{
				num_nodes_added++;

				if (num_nodes_added > 100)
				{
					strcpy( last_node_added_string, va("^5Adding waypoint ^3%i ^5at ^7%f %f %f^5.", optimized_number_of_nodes, nodes[i].origin[0], nodes[i].origin[1], nodes[i].origin[2]) );
					trap_UpdateScreen();
					num_nodes_added = 0;
				}

				Optimize_AddNode( nodes[i].origin, 0, objNum, 0 );	//add the node
				nodes[i].objEntity = 1;
			}
		}

		aw_percent_complete = 0.0f;
		trap_UpdateScreen();
	}

	B_Free(areas);

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Waypoint list reduced from ^3%i^5 waypoints to ^3%i^5 waypoints.\n", number_of_nodes, optimized_number_of_nodes) );

	// Work out the best scatter distance to use for this map size...
	if (map_size > 96000)
	{
		waypoint_scatter_distance *= 1.5;
	}
	else if (map_size > 32768)
	{
		waypoint_scatter_distance *= 1.35;
	}
	else if (map_size > 24000)
	{
		waypoint_scatter_distance *= 1.25;
	}
	else if (map_size > 16550)
	{
		waypoint_scatter_distance *= 1.10;
	}
	else if (map_size > 8192)
	{
		waypoint_scatter_distance *= 1.02;
	}
	
	if (number_of_nodes < 20000)
	{
		waypoint_distance_multiplier = 1.5f;
	}
	else if (number_of_nodes < 32000)
	{
		waypoint_distance_multiplier = 2.0f;
	}
	else
	{
		waypoint_distance_multiplier = 2.5f;
	}

	number_of_nodes = 0;

	// Copy over the new list!
	memcpy(nodes, optimized_nodes, (sizeof(node_t)*MAX_NODES)+1);
	number_of_nodes = optimized_number_of_nodes;

	/*for (i = 0; i < optimized_number_of_nodes; i++)
	{
		VectorCopy(optimized_nodes[i].origin, nodes[number_of_nodes].origin);
		optimized_nodes[i].enodenum = 0;
		number_of_nodes++;
	}*/

	aw_num_nodes = number_of_nodes;

	// Save the new list...
	AIMOD_NODES_SaveNodes_Autowaypointed();

	aw_percent_complete = 0.0f;
	trap_UpdateScreen();

	// Remake cover spots...
	if (start_wp_total != number_of_nodes)
		AIMOD_Generate_Cover_Spots();

	aw_percent_complete = 0.0f;
	trap_UpdateScreen();

	AIMod_AutoWaypoint_Free_Memory();
	AIMod_AutoWaypoint_Optimize_Free_Memory();

	trap_SendConsoleCommand( "!loadnodes\n" );
}

int			trigger_hurt_counter = 0;
centity_t	*trigger_hurt_list[MAX_GENTITIES];

void AIMod_AutoWaypoint_Trigger_Hurt_List_Setup ( void )
{// Generate a list for the current map to speed up processing...
	int i = 0;

	trigger_hurt_counter = 0;

	for (i = 0; i < MAX_GENTITIES; i++)
	{
		centity_t *cent = &cg_entities[i];

		if (cent->currentState.eType == ET_TRIGGER_HURT)
		{
			trigger_hurt_list[trigger_hurt_counter] = cent;
			trigger_hurt_counter++;
		}
	}
}

qboolean LocationIsNearTriggerHurt ( vec3_t origin )
{
	int i = 0;

	for (i = 0; i < trigger_hurt_counter; i++)
	{
		if (VectorDistance(trigger_hurt_list[i]->currentState.origin, origin) < 64)
			return qtrue;
	}

	return qfalse;
}

extern int Q_TrueRand ( int low, int high );

#ifdef __CUDA__
__device__ __host__ qboolean AIMod_AutoWaypoint_Cleaner_NodeHasLinkWithFewLinks ( int node )
{// Check the links of the current node for any nodes with very few links (so we dont remove an important node for reachability)
	__shared__ int i = 0;
	__shared__ int removed_neighbours_count = 0;
	
	for (i = 0; i < nodes[node].enodenum; i++)
	{
		if (nodes[node].enodenum > 64)
			continue; // Corrupt?

		if (nodes[nodes[node].links[i].targetNode].objectNum[0] == 1)
			removed_neighbours_count++;
	}

	if (nodes[node].enodenum - removed_neighbours_count <= 8/*4*/)
		return qtrue; // lready removed too many neighbours here! Remove no more!

	for (i = 0; i < nodes[node].enodenum; i++)
	{
		if (nodes[node].enodenum > 64)
			continue; // Corrupt?

		if (nodes[nodes[node].links[i].targetNode].enodenum <= 8/*4*/)
			return qtrue;
	}

	return qfalse;
}
#else //!__CUDA__
qboolean AIMod_AutoWaypoint_Cleaner_NodeHasLinkWithFewLinks ( int node )
{// Check the links of the current node for any nodes with very few links (so we dont remove an important node for reachability)
	int i = 0;
	int removed_neighbours_count = 0;
	
	for (i = 0; i < nodes[node].enodenum; i++)
	{
		if (nodes[node].enodenum > 64)
			continue; // Corrupt?

		if (nodes[nodes[node].links[i].targetNode].objectNum[0] == 1)
			removed_neighbours_count++;
	}

	if (nodes[node].enodenum - removed_neighbours_count <= 8/*4*/)
		return qtrue; // lready removed too many neighbours here! Remove no more!

	for (i = 0; i < nodes[node].enodenum; i++)
	{
		if (nodes[node].enodenum > 64)
			continue; // Corrupt?

		if (nodes[nodes[node].links[i].targetNode].enodenum <= 8/*4*/)
			return qtrue;
	}

	return qfalse;
}
#endif //__CUDA__

int num_dupe_nodes = 0;

#ifdef __CUDA__
__device__ __host__ void CheackForNearbyDupeNodes( int node )
{// Removes any nodes with the same links as this node to reduce numbers...
	__shared__ int i = 0;

	if (nodes[node].objectNum[0] == 1)
		return; // This node is disabled already...

	for (i = 0; i < number_of_nodes; i++)
	{
		__shared__ int j = 0;
		__shared__ int num_same = 0;

		if (i == node)
			continue;

		if (nodes[i].enodenum <= 0)
			continue;

		if (nodes[i].objectNum[0] == 1)
			continue;

		if (VectorDistance(nodes[node].origin, nodes[i].origin) >= 256)
			continue;

		if (nodes[i].enodenum > nodes[node].enodenum)
			continue; // Never remove a better node...

		for (j = 0; j < nodes[node].enodenum; j++)
		{
			__shared__ int k = 0;

			for (k = 0; k < nodes[i].enodenum; k++)
			{
				if (nodes[node].links[j].targetNode == nodes[i].links[k].targetNode)
				{
					num_same++;
					break;
				}
			}
		}

		if (num_same >= nodes[node].enodenum)
		{// Node i is a dupe node! Disable it!
			if ((nodes[i].type & NODE_LAND_VEHICLE) && !(nodes[node].type & NODE_LAND_VEHICLE))
			{// Never remove a vehicle node and replace it with a non vehicle one!

			}
			else
			{
				nodes[i].objectNum[0] = 1;
				num_dupe_nodes++;
			}
		}
	}
}
#else //!__CUDA__
void CheackForNearbyDupeNodes( int node )
{// Removes any nodes with the same links as this node to reduce numbers...
	int i = 0;

	if (nodes[node].objectNum[0] == 1)
		return; // This node is disabled already...

	for (i = 0; i < number_of_nodes; i++)
	{
		int j = 0;
		int num_same = 0;
		qboolean REMOVE_NODE = qtrue;

		if (i == node)
			continue;

		if (nodes[i].enodenum <= 0)
			continue;

		if (nodes[i].objectNum[0] == 1)
			continue;

		if (VectorDistance(nodes[node].origin, nodes[i].origin) >/*= 192*/ waypoint_scatter_distance*waypoint_distance_multiplier)
			continue;

		if (nodes[i].enodenum > nodes[node].enodenum)
			continue; // Never remove a better node...

		for (j = 0; j < nodes[node].enodenum; j++)
		{
			int k = 0;
			qboolean found = qfalse;

			for (k = 0; k < nodes[i].enodenum; k++)
			{
				if (nodes[node].links[j].targetNode == nodes[i].links[k].targetNode)
				{
					found = qtrue;
					break;
				}
			}

			if (!found)
			{
				REMOVE_NODE = qfalse;
				break;
			}
		}

		if (REMOVE_NODE)
		{// Node i is a dupe node! Disable it!
			if ((nodes[i].type & NODE_LAND_VEHICLE) && !(nodes[node].type & NODE_LAND_VEHICLE))
			{// Never remove a vehicle node and replace it with a non vehicle one!

			}
			else
			{
				nodes[i].objectNum[0] = 1;
				num_dupe_nodes++;
			}
		}
	}
}
#endif //__CUDA__
/*
qboolean CheckIfAnotherVehicleNodeIsNearby ( int node )
{
	int i = 0;
	int num_found = 0;

	for (i = 0; i < number_of_nodes; i++)
	{
		if ( i == node )
			continue;

		if (!(nodes[i].type & NODE_LAND_VEHICLE))
			continue;

		if (nodes[i].objectNum[0] == 1)
			continue;

		if (VectorDistance( nodes[i].origin, nodes[node].origin ) > 512)
			continue;

		num_found++;

		if (num_found >= 12)
			break;
	}

	if (num_found >= 12)
		return qtrue;

	return qfalse;
}*/

#ifdef __CUDA__
__device__ __host__ qboolean CheckIfTooManyLinksRemoved ( int node, qboolean extra )
{
	__shared__ int i = 0;
	__shared__ int num_found = 0;

	for (i = 0; i < nodes[node].enodenum; i++)
	{
		__shared__ int target = nodes[node].links[i].targetNode;

		if (target < 0)
			continue;

		if (target > number_of_nodes)
			continue;

		if (nodes[target].objectNum[0] == 1)
			continue;

		num_found++;

		if (extra)
		{
			if (num_found > 4)
				return qfalse;
		}
		else
		{
			if (num_found > 8)
				return qfalse;
		}
	}

	return qtrue;
}
#else //!__CUDA__
qboolean CheckIfTooManyLinksRemoved ( int node, qboolean extra )
{
	int i = 0;
	int num_found = 0;

	for (i = 0; i < nodes[node].enodenum; i++)
	{
		int target = nodes[node].links[i].targetNode;

		if (target < 0)
			continue;

		if (target > number_of_nodes)
			continue;

		if (nodes[target].objectNum[0] == 1)
			continue;

		num_found++;

		if (extra)
		{
			if (num_found > 4)
				return qfalse;
		}
		else
		{
			if (num_found > 8)
				return qfalse;
		}
	}

	return qtrue;
}
#endif //__CUDA__

#define __TEST_CLEANER__

#ifdef __TEST_CLEANER__

#endif //__TEST_CLEANER__

/* */
void
AIMod_AutoWaypoint_Cleaner ( qboolean quiet, qboolean null_links_only, qboolean relink_only, qboolean multipass, qboolean initial_pass, qboolean extra )
{
	int i = 0;//, j = 0;//, k = 0, l = 0;//, m = 0;
	int	total_calculations = 0;
	int	calculations_complete = 0;
	//int	*areas;//[16550];
//	int num_areas = 0;
	float map_size;
	vec3_t mapMins, mapMaxs;
	float temp;
//	float AREA_SEPERATION = DEFAULT_AREA_SEPERATION;
//	int screen_update_timer = 0;
//	int entities_start = 0;
	int	node_disable_ticker = 0;
	int	num_disabled_nodes = 0;
	int num_nolink_nodes = 0;
	int num_trigger_hurt_nodes = 0;
	int num_skiped_nodes = 0;
	int	node_disable_ratio = 2;
	int total_removed = 0;
//	qboolean	bad_surfaces_only = qfalse;
	qboolean	noiceremove = qfalse;
	qboolean	nowaterremove = qfalse;
	int			skip = 0;
//	int			skip_threshold = 2;
	qboolean	reducecount = qtrue;
	int			start_wp_total = 0;
	int			node_clean_ticker = 0;
	int			num_passes_completed = 0;

	aw_num_bad_surfaces = 0;
	num_dupe_nodes = 0;

	trap_Cvar_Set("cg_waypoint_render", "0");
	trap_UpdateScreen();
	trap_UpdateScreen();
	trap_UpdateScreen();

	// UQ1: Check if we have an SSE CPU.. It can speed up our memory allocation by a lot!
	if (!CPU_CHECKED)
		UQ_Get_CPU_Info();

	AIMod_AutoWaypoint_Init_Memory();
	AIMod_AutoWaypoint_Optimize_Init_Memory();

	if (number_of_nodes > 0)
	{// UQ1: Init nodes list!
		number_of_nodes = 0; 
		optimized_number_of_nodes = 0;
		
		if (SSE_CPU)
		{
			sse_memset( nodes, 0, ((sizeof(node_t)+1)*MAX_NODES)/8 );
			sse_memset( optimized_nodes, 0, ((sizeof(node_t)+1)*MAX_NODES)/8 );
		}
		else
		{
			memset( nodes, 0, ((sizeof(node_t)+1)*MAX_NODES) );
			memset( optimized_nodes, 0, ((sizeof(node_t)+1)*MAX_NODES) );
		}
	}

	AIMOD_NODES_LoadNodes();

	if (number_of_nodes <= 0)
	{
		AIMod_AutoWaypoint_Free_Memory();
		AIMod_AutoWaypoint_Optimize_Free_Memory();
		return;
	}

	start_wp_total = number_of_nodes;

	//areas = B_Alloc( (sizeof(int)+1)*512000 );

	//AIMod_GetMapBounts( mapMins, mapMaxs );
	AIMod_GetMapBounts();

	VectorCopy(cg.mapcoordsMins, mapMins);
	VectorCopy(cg.mapcoordsMaxs, mapMaxs);

	if (mapMaxs[0] < mapMins[0])
	{
		temp = mapMins[0];
		mapMins[0] = mapMaxs[0];
		mapMaxs[0] = temp;
	}

	if (mapMaxs[1] < mapMins[1])
	{
		temp = mapMins[1];
		mapMins[1] = mapMaxs[1];
		mapMaxs[1] = temp;
	}

	if (mapMaxs[2] < mapMins[2])
	{
		temp = mapMins[2];
		mapMins[2] = mapMaxs[2];
		mapMaxs[2] = temp;
	}

	map_size = VectorDistance(mapMins, mapMaxs);

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Map bounds (^7%f %f %f ^5by ^7%f %f %f^5).\n", mapMins[0], mapMins[1], mapMins[2], mapMaxs[0], mapMaxs[1], mapMaxs[2]) );

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Cleaning waypoint list...\n") );
	strcpy( task_string1, va("^7Cleaning waypoint list....") );
	trap_UpdateScreen();

	CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^7This could take a while...\n") );
	strcpy( task_string2, va("^7This could take a while...") );
	trap_UpdateScreen();

	// UQ1: Set node types..
	AIMOD_AI_InitNodeContentsFlags();
	AIMod_AutoWaypoint_Trigger_Hurt_List_Setup();

	if (number_of_nodes > 40000)
		node_disable_ratio = 10;
	else if (number_of_nodes > 36000)
		node_disable_ratio = 9;
	else if (number_of_nodes > 32000)
		node_disable_ratio = 8;
	else if (number_of_nodes > 30000)
		node_disable_ratio = 7;
	else if (number_of_nodes > 24000)
		node_disable_ratio = 6;
	else if (number_of_nodes > 22000)
		node_disable_ratio = 5;
	else if (number_of_nodes > 18000)
		node_disable_ratio = 4;
	else if (number_of_nodes > 14000)
		node_disable_ratio = 3;
	else if (number_of_nodes > 8000)
		node_disable_ratio = 2;

/*	if (number_of_nodes > 64000)
		skip_threshold = 4;
	else if (number_of_nodes > 48000)
		skip_threshold = 3;
	else if (number_of_nodes > 32000)
		skip_threshold = 2;
	else if (number_of_nodes > 10000)
		skip_threshold = 2;
	else
	{
		skip_threshold = 0;
		reducecount = qfalse;
	}*/

	while (1)
	{
		CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Cleaning waypoints list... Please wait...\n") );
		strcpy( task_string3, va("^5Cleaning waypoints list... Please wait...") );
		trap_UpdateScreen();

		total_calculations = number_of_nodes;
		calculations_complete = 0;
		node_clean_ticker = 0;
		num_nolink_nodes = 0;
		num_disabled_nodes = 0;
		num_trigger_hurt_nodes = 0;
		num_dupe_nodes = 0;
		aw_num_bad_surfaces = 0;
		num_skiped_nodes = 0;
		total_removed = 0;
		
		num_passes_completed++;

		// Work out the best scatter distance to use for this map size...
		if (map_size > 96000)
		{
			waypoint_scatter_distance *= 1.5;
		}
		else if (map_size > 32768)
		{
			waypoint_scatter_distance *= 1.35;
		}
		else if (map_size > 24000)
		{
			waypoint_scatter_distance *= 1.25;
		}
		else if (map_size > 16550)
		{
			waypoint_scatter_distance *= 1.10;
		}
		else if (map_size > 8192)
		{
			waypoint_scatter_distance *= 1.02;
		}
	
		if (number_of_nodes < 20000)
		{
			waypoint_distance_multiplier = 1.5f;
		}
		else if (number_of_nodes < 32000)
		{
			waypoint_distance_multiplier = 2.0f;
		}
		else
		{
			waypoint_distance_multiplier = 2.5f;
		}

		// Disable some ice/water ndoes...
		if (!relink_only)
		for (i = 0; i < number_of_nodes; i++)
		{
			node_clean_ticker++;
			calculations_complete++;

			if (number_of_nodes <= 5000 )
			{// No point relinking and saving...
	//			B_Free(areas);
				AIMod_AutoWaypoint_Free_Memory();
				AIMod_AutoWaypoint_Optimize_Free_Memory();
				CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Waypoint cleaner exited. The waypoint file has been optimized enough.\n");
				return;
			}

			// Draw a nice little progress bar ;)
			aw_percent_complete = (float)((float)((float)(calculations_complete)/(float)(total_calculations))*100.0f);

			if (node_clean_ticker > 100)
			{
				strcpy( last_node_added_string, va("^5Checking waypoint ^3%i ^5at ^7%f %f %f^5.", i, nodes[i].origin[0], nodes[i].origin[1], nodes[i].origin[2]) );
				trap_UpdateScreen();
				node_clean_ticker = 0;
			}

			if (nodes[i].objectNum[0] == 1)
				continue;

			if (nodes[i].enodenum <= 1/*0*/)
			{// Remove all waypoints without any links...
				nodes[i].objectNum[0] = 1;
				num_nolink_nodes++;
				continue;
			}

			if (relink_only)
				continue;

			if (nodes[i].enodenum > 2 && LocationIsNearTriggerHurt(nodes[i].origin))
			{// Lots of links, but also seems to be located close to barb wire (or other damage entity). Remove it!
				nodes[i].objectNum[0] = 1;
				num_trigger_hurt_nodes++;
				continue;
			}

#ifndef __TEST_CLEANER__
			if (extra)
			{
				if (nodes[i].enodenum >= 12/*16*/)
				{
					/*if ((nodes[i].type & NODE_LAND_VEHICLE) && !CheckIfAnotherVehicleNodeIsNearby(i))
					{// We need to never remove vehicle nodes when there is not enough others around...
					
					}
					else*/
					//if (!CheckIfTooManyLinksRemoved(i, extra))
					{
						nodes[i].objectNum[0] = 1;
						num_skiped_nodes++;
						continue;
					}
				}
			}
			else if (nodes[i].enodenum >= 16/*16*/)
			{
				/*if ((nodes[i].type & NODE_LAND_VEHICLE) && !CheckIfAnotherVehicleNodeIsNearby(i))
				{// We need to never remove vehicle nodes when there is not enough others around...
					
				}
				else*/
				if (!CheckIfTooManyLinksRemoved(i, extra))
				{
					nodes[i].objectNum[0] = 1;
					num_skiped_nodes++;
					continue;
				}
			}

			CheackForNearbyDupeNodes(i);
#else //__TEST_CLEANER__
			if (nodes[i].enodenum >= 1)
			{
				int num_links_alt_reachable = 0;
				int current_link = 0;
				int	reach_nodes[64];
				int	num_reach_nodes = 0;

				for (current_link = 0; current_link < nodes[i].enodenum; current_link++)
				{
					int			j = 0;
					int			current_target_node = nodes[i].links[current_link].targetNode;
					qboolean	return_to_start = qfalse;

					num_reach_nodes = 0;

					for (j = 0; j < nodes[i].enodenum; j++)
					{
						int k = 0;
						int target_node1 = nodes[i].links[j].targetNode;

						if (nodes[target_node1].objectNum[0] == 1)
							continue;

						for (k = 0; k < nodes[target_node1].enodenum; k++)
						{
							int target_node2 = nodes[target_node1].links[k].targetNode;

							if (target_node2 == i)
								continue;

							if (target_node2 == current_target_node)
							{
								num_links_alt_reachable++;
								reach_nodes[num_reach_nodes] = target_node2;
								num_reach_nodes++;
								return_to_start = qtrue;
								break;
							}
						}

						if (return_to_start)
							break;
					}
				}

				if (num_links_alt_reachable >= nodes[i].enodenum)
				{
					int k = 0;
					int num_alt_links = 0;

					for (k = 0; k < num_reach_nodes; k++)
					{
						int j = 0;
						int target_node1 = reach_nodes[k];

						for (j = 0; j < nodes[i].enodenum; j++)
						{
							qboolean found = qfalse;
							int l = 0;
							int target_node2 = nodes[i].links[j].targetNode;
						
							for (l = 0; l < nodes[target_node2].enodenum; l++)
							{
								int target_node3 = nodes[target_node2].links[l].targetNode;

								if (target_node3 == target_node1)
								{
									num_alt_links++;
									found = qtrue;
									break;
								}
							}

							if (found)
								break;
						}
					}

					if (num_alt_links >= num_reach_nodes)
					{
						nodes[i].objectNum[0] = 1;
						num_skiped_nodes++;
						continue;
					}
				}

				CheackForNearbyDupeNodes(i);
			}
#endif //__TEST_CLEANER__

			//if (nodes[i].enodenum <= 8/*4*/ || AIMod_AutoWaypoint_Cleaner_NodeHasLinkWithFewLinks(i))
			//	continue;

			/*if (initial_pass && reducecount)
			{
				if (skip == 0)
				{
					skip = 1;
				}
				else
				{
					nodes[i].objectNum[0] = 1;
					num_skiped_nodes++;

					skip = 0;
				}
			}*/

			/*if ((!nowaterremove && (nodes[i].type & NODE_WATER)) || (!noiceremove && (nodes[i].type & NODE_ICE)))
			{
				if (node_disable_ticker < node_disable_ratio)
				{// Remove 2 out of every (node_disable_ratio)..
					nodes[i].objectNum[0] = 1;
					num_disabled_nodes++;
					node_disable_ticker++;
				}
				else
				{
					node_disable_ticker = 0;
				}
			}*/
		}

		aw_percent_complete = 0.0f;
		trap_UpdateScreen();

		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Disabled ^3%i^5 water/ice waypoints.\n", num_disabled_nodes);
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Disabled ^3%i^5 bad surfaces.\n", aw_num_bad_surfaces);
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Disabled ^3%i^5 trigger hurt waypoints.\n", num_trigger_hurt_nodes);
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Disabled ^3%i^5 waypoints without links.\n", num_nolink_nodes);
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Disabled ^3%i^5 waypoints with duplicate links to a neighbor.\n", num_dupe_nodes);
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Disabled ^3%i^5 waypoints in over-waypointed areas.\n", num_skiped_nodes);

		total_removed = num_skiped_nodes + num_dupe_nodes + num_disabled_nodes + aw_num_bad_surfaces + num_nolink_nodes + num_trigger_hurt_nodes;

		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Disabled ^3%i^5 total waypoints in this run.\n", total_removed);

		if (total_removed <= 100/*== 0*/ && multipass && !relink_only )
		{
			CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Multipass waypoint cleaner completed in %i passes. Nothing left to remove.\n", num_passes_completed);
			AIMod_AutoWaypoint_Free_Memory();
			AIMod_AutoWaypoint_Optimize_Free_Memory();
			return;
		}
		else if (total_removed <= 100/*== 0*/ && !relink_only )
		{// No point relinking and saving...
//			B_Free(areas);
			AIMod_AutoWaypoint_Free_Memory();
			AIMod_AutoWaypoint_Optimize_Free_Memory();
			CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Waypoint cleaner completed. Nothing left to remove.\n");
			return;
		}

		if (multipass)
		{
			CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Creating new waypoints list (pass# ^7%i^5)... Please wait...\n", num_passes_completed) );
			strcpy( task_string3, va("^5Creating new waypoints list (pass# ^7%i^5)... Please wait...", num_passes_completed) );
		}
		else
		{
			CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Creating new waypoints list... Please wait...\n") );
			strcpy( task_string3, va("^5Creating new waypoints list... Please wait...") );
		}
		trap_UpdateScreen();

		total_calculations = number_of_nodes;
		calculations_complete = 0;

		for (i = 0; i < number_of_nodes; i++)
		{
			int				num_nodes_added = 0;
			short int		objNum[3] = { 0, 0, 0 };

			if (nodes[i].objEntity != 1 && nodes[i].objectNum[0] != 1)
			{
				num_nodes_added++;

				calculations_complete++;

				// Draw a nice little progress bar ;)
				aw_percent_complete = (float)((float)((float)(calculations_complete)/(float)(total_calculations))*100.0f);

				if (num_nodes_added > 100)
				{
					strcpy( last_node_added_string, va("^5Adding waypoint ^3%i ^5at ^7%f %f %f^5.", optimized_number_of_nodes, nodes[i].origin[0], nodes[i].origin[1], nodes[i].origin[2]) );
					trap_UpdateScreen();
					num_nodes_added = 0;
				}

				Optimize_AddNode( nodes[i].origin, 0, objNum, 0 );	//add the node
				nodes[i].objEntity = 1;
			}
		}

		calculations_complete = 0;
		aw_percent_complete = 0.0f;
		trap_UpdateScreen();

//		B_Free(areas);

		CG_Printf( va( "^4*** ^3AUTO-WAYPOINTER^4: ^5Waypoint list reduced from ^3%i^5 waypoints to ^3%i^5 waypoints.\n", number_of_nodes, optimized_number_of_nodes) );

		number_of_nodes = 0;

		// Copy over the new list!
		memcpy(nodes, optimized_nodes, (sizeof(node_t)*MAX_NODES)+1);
		number_of_nodes = optimized_number_of_nodes;
		optimized_number_of_nodes = 0;

		aw_num_nodes = number_of_nodes;

		// Save the new list...
		AIMOD_NODES_SaveNodes_Autowaypointed();

		aw_percent_complete = 0.0f;
		trap_UpdateScreen();
	
		CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Multipass waypoint cleaner pass #%i completed. Beginning next pass.\n", num_passes_completed);
		
		initial_pass = qfalse;

		if (!multipass)
		{// No point relinking and saving...
//			B_Free(areas);
			AIMod_AutoWaypoint_Free_Memory();
			AIMod_AutoWaypoint_Optimize_Free_Memory();
			CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Waypoint cleaner completed.\n");
			break;
		}

		if (!extra && number_of_nodes > 10000 && total_removed < 1000)
		{// Too many, we need an "extra" pass...
			extra = qtrue;
			CG_Printf("^4*** ^3AUTO-WAYPOINTER^4: ^5Not enough removed in this pass, forcing next pass to be an \"extra\" pass.\n");
		}
		else
		{
			extra = qfalse;
		}

		calculations_complete = 0;
		node_clean_ticker = 0;
		num_nolink_nodes = 0;
		num_disabled_nodes = 0;
		num_trigger_hurt_nodes = 0;
		num_dupe_nodes = 0;
		aw_num_bad_surfaces = 0;
		num_skiped_nodes = 0;
		total_removed = 0;
	}

	// Remake cover spots...
	if (start_wp_total != number_of_nodes)
		AIMOD_Generate_Cover_Spots();

	aw_percent_complete = 0.0f;
	trap_UpdateScreen();

	AIMod_AutoWaypoint_Free_Memory();
	AIMod_AutoWaypoint_Optimize_Free_Memory();

	trap_SendConsoleCommand( "!loadnodes\n" );
}

// ================================================================================================================
// Visualizer
// ================================================================================================================

#define NUMBER_SIZE		8

void CG_Waypoint( int wp_num ) {
	refEntity_t		re;
	vec3_t			angles, vec, dir, up;
	int				i, numdigits, digits[10], temp_num;

	memset( &re, 0, sizeof( re ) );

	VectorCopy(nodes[wp_num].origin, re.origin);
	re.origin[2]+=16;

	re.reType = RT_SPRITE;
	re.radius = 16;

	re.shaderRGBA[0] = 0xff;
	re.shaderRGBA[1] = 0x11;
	re.shaderRGBA[2] = 0x11;

	re.radius = NUMBER_SIZE / 2;

	VectorClear(angles);
	AnglesToAxis( angles, re.axis );

	VectorSubtract(cg.refdef.vieworg, re.origin, dir);
	CrossProduct(dir, up, vec);
	VectorNormalize(vec);

	temp_num = wp_num;

	for (numdigits = 0; !(numdigits && !temp_num); numdigits++) {
		digits[numdigits] = temp_num % 10;
		temp_num = temp_num / 10;
	}

	for (i = 0; i < numdigits; i++) {
		//VectorMA(nodes[wp_num].origin, (float) (((float) numdigits / 2) - i) * NUMBER_SIZE, vec, re.origin);
		re.customShader = cgs.media.numberShaders[digits[numdigits-1-i]];
		trap_R_AddRefEntityToScene( &re );
	}
}

qboolean LinkCanReachMe ( int wp_from, int wp_to )
{
	int i = 0;

	for (i = 0; i < nodes[wp_to].enodenum; i ++)
	{
		if (nodes[wp_to].links[i].targetNode == wp_from)
			return qtrue;
	}

	return qfalse;
}

void CG_AddWaypointLinkLine( int wp_from, int wp_to )
{
	refEntity_t		re;

	memset( &re, 0, sizeof( re ) );

	//re.reType = RT_LINE;
	re.reType = RT_RAIL_CORE;
	//RT_RAIL_CORE
	//RT_SPRITE

	//re.radius = 1;
	re.radius = 0.00001;

	if (nodes[wp_from].type & NODE_COVER)
	{// Cover spots show as yellow...
		re.shaderRGBA[0] = 0xff;
		re.shaderRGBA[1] = 0xff;
		re.shaderRGBA[2] = 0x00;
		re.shaderRGBA[3] = 0xff;
	}
	else if (LinkCanReachMe( wp_from, wp_to ))
	{// Link is bi-directional.. Display in white..
		re.shaderRGBA[0] = re.shaderRGBA[1] = re.shaderRGBA[2] = re.shaderRGBA[3] = 0xff;
	}
	else
	{// Link is uni-directional.. Display in red..
		re.shaderRGBA[0] = 0xff;
		re.shaderRGBA[1] = 0x00;
		re.shaderRGBA[2] = 0x00;
		re.shaderRGBA[3] = 0xff;
	}

	re.customShader = cgs.media.whiteShader;

	VectorCopy(nodes[wp_from].origin, re.origin);
	re.origin[2]+=16;
	VectorCopy(nodes[wp_to].origin, re.oldorigin);
	re.oldorigin[2]+=16;

	trap_R_AddRefEntityToScene( &re );
}

qboolean CURRENTLY_RENDERRING = qfalse;

qboolean InFOV( vec3_t spot, vec3_t from, vec3_t fromAngles, int hFOV, int vFOV )
{
	vec3_t	deltaVector, angles, deltaAngles;

	VectorSubtract ( spot, from, deltaVector );
	vectoangles ( deltaVector, angles );
	
	deltaAngles[PITCH]	= AngleDelta ( fromAngles[PITCH], angles[PITCH] );
	deltaAngles[YAW]	= AngleDelta ( fromAngles[YAW], angles[YAW] );

	if ( fabs ( deltaAngles[PITCH] ) <= vFOV && fabs ( deltaAngles[YAW] ) <= hFOV ) 
	{
		return qtrue;
	}

	return qfalse;
}

void DrawWaypoints()
{
	int node = 0;
	
	if (cg_waypoint_render.integer <= 0)
	{
		if (CURRENTLY_RENDERRING)
		{
			AIMod_AutoWaypoint_Free_Memory();
		}

		CURRENTLY_RENDERRING = qfalse;
		return;
	}

	if (!CURRENTLY_RENDERRING)
	{
		number_of_nodes = 0;

		AIMod_AutoWaypoint_Init_Memory();

		AIMOD_NODES_LoadNodes(); // Load node file on first check...
		//AIMOD_LoadCoverPoints();

		if (number_of_nodes <= 0)
		{
			AIMod_AutoWaypoint_Free_Memory();
			CURRENTLY_RENDERRING = qfalse;
			return;
		}

		CURRENTLY_RENDERRING = qtrue;
	}

	if (number_of_nodes <= 0) return; // If still no nodes, exit early...

	for (node = 0; node < number_of_nodes; node++)
	{
		// Draw anything closeish to us...
		int		len = 0;
		int		link = 0;
		vec3_t	delta;

		if (!InFOV( nodes[node].origin, cg.refdef.vieworg, cg.refdefViewAngles, 120, 120 ))
			continue;

		VectorSubtract( nodes[node].origin, cg.refdef.vieworg, delta );
		len = VectorLength( delta );
		
		if ( len < 20 ) continue;
		if ( len > 512 ) continue;

		//if (VectorDistance(cg_entities[cg.clientNum].lerpOrigin, nodes[node].origin) > 2048) continue;

		//CG_Waypoint( node );

		for (link = 0; link < nodes[node].enodenum; link++)
		{
			CG_AddWaypointLinkLine( node, nodes[node].links[link].targetNode );
		}
	}
}

//float	BAD_HEIGHTS[1024];
//int		NUM_BAD_HEIGHTS = 0;

/*
void AIMod_MarkBadHeight ( void )
{// Mark this height on the map as bad for waypoints...
	BAD_HEIGHTS[NUM_BAD_HEIGHTS] = cg.refdef.vieworg[2];
	NUM_BAD_HEIGHTS++;
	CG_Printf("Height %f marked as bad for waypoints.\n", cg.refdef.vieworg[2]);
}
*/

#endif //__BOT__
