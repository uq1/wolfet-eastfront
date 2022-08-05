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
#if defined( __BOT__ ) || defined( __BOT_AAS__ )
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

//
#include "../botai/ai_main.h"
#include "../botai/ai_team.h"
#include "../botai/ai_dmq3.h"
#include "../botai/ai_cmd.h"

//
#include "../botai/ai_dmnet_mp.h"
#include "../botai/ai_dmgoal_mp.h"
static bot_goal_t	target;
int					BotObjectives[MAX_GENTITIES];
int					num_bot_objectives = 0;
int					BotGoals[MAX_GENTITIES];
int					num_bot_goals = 0;
int					next_goal_update = -1;
extern int			AIMOD_NAVIGATION_FindClosestReachableNodeTo ( vec3_t origin, int r, int type );


/* */
float
TouchDistance_For_GoalType ( int goalType )
{								// Return's acceptable touch distance for a bot goal type...
	float	distance = 4.0f;	// Default...
	if ( goalType == BOTGOAL_CONSTRUCT || goalType == BOTGOAL_DESTROY )
	{
		distance = 64.0f;
	}
	else if ( goalType == BOTGOAL_ACTIVATE || goalType == BOTGOAL_USE )
	{
		distance = 16.0f;
	}
	else if ( goalType == BOTGOAL_FINDHEALTH || goalType == BOTGOAL_FINDAMMO || goalType == BOTGOAL_FOLLOW )
	{
		distance = 24.0f;
	}
	else if ( goalType == BOTGOAL_GIVEHEALTH || goalType == BOTGOAL_GIVEAMMO )
	{
		distance = 64.0f;
	}
	else if ( goalType == BOTGOAL_POPFLAG )
	{
		distance = 128.0f;		// Well within the capture range...
	}
	else if ( goalType == BOTGOAL_REVIVE )
	{
		distance = 64.0f;
	}

	return ( distance );
}

int					numAxisObjectives;
int					axisObjectives[256];
int					numAlliedObjectives;
int					alliedObjectives[256];
int					num_axis_buildables = 0;
int					axisBuildables[256];
int					num_allied_buildables = 0;
int					alliedBuildables[256];
int					num_axis_destroyables = 0;
int					axisDestroyables[256];
int					num_allied_destroyables = 0;
int					alliedDestroyables[256];
int					numSniperGoals;
int					SniperGoals[256];
int					numDefencePoints;
int					DefencePoints[256];
int					numAxisDefencePoints = 0;
int					numAlliedDefencePoints = 0;

static const char	*botObjectiveList[] =
{

	"func_constructible",
	"misc_constructiblemarker",
	"team_WOLF_objective",
	"misc_control_point",
	"explosive_indicator",

	"sandbag_constructible",
	"mg_nest_constructible",

	//"script_mover",
	//	"trigger_objective_info",
	"misc_commandmap_marker",

	//"func_explosive",
	"team_CTF_redflag",
	"team_CTF_blueflag",

	"misc_cabinet_health",
	"misc_cabinet_supply",
	"ammo_crate",
	"health_crate",

/*#ifdef __VEHICLES__
	"misc_tank",
	"misc_panzer",
	"panzerivtank",
	"t34tank",
	"bt7tank",
	"kv1tank",
	"flametank",
	"stug3tank",
	"tigertank",
	"flak88antitank",
	"pakantitank",
	"misc_panzeriv",
    "misc_stug3",
    "misc_kv1",
    "misc_flak88",
    "misc_pak",
	"npc_tank",
	"npc_churchhill",
	"npc_panzer",
	"npc_tank_heavy",
	"npc_tank_medium",
	"npc_tank_light",
    "npc_artillery",
	"npc_flametank",
	"churchilltank",
	"panzertank",
	"panzer2tank",
#endif //__VEHICLES__*/
	"",
};


/* */
qboolean
BotMP_Is_Primary_Goal ( int entityNum )
{
	int loop = 0;
	while ( botObjectiveList[loop] )
	{
		if ( g_entities[entityNum].classname == botObjectiveList[loop] )
		{
			return ( qtrue );
		}

		loop++;
	}

	return ( qfalse );
}

static const char	*botGoalsList[] =
{
	"ai_node",
	"ai_marker",
	"item_botroam",
	"misc_mg42",
	"misc_aagun",
	"misc_cabinet_health",
	"misc_cabinet_supply",
	"func_secret",
	"func_button",
	"info_camp",
	"func_constructible",
	"team_CTF_redflag",
	"team_CTF_blueflag",
	"info_player_checkpoint",
	"team_WOLF_objective",
	"team_WOLF_checkpoint",
	"misc_control_point",

	"sandbag_constructible",
	"mg_nest_constructible",

	//	"script_mover",
	//	"trigger_objective_info",
	"info_player_checkpoint",
	"misc_mg42base",

	//"func_explosive",
	"bot_sniper_spot",
	"bot_attractor",
	"bot_axis_cover_spot",
	"bot_allied_cover_spot",
	"bot_seek_cover_spot",
	"bot_seek_cover_sequence",
	"bot_axis_seek_cover_spot",
	"bot_allied_seek_cover_spot",
	"bot_jump_source",
	"bot_allied_attack_spot",
	"bot_axis_attack_spot",
	"misc_stealth_area",
	"ammo_crate",
	"health_crate",
/*#ifdef __VEHICLES__
	"misc_tank",
	"misc_panzer",
	"panzerivtank",
	"t34tank",
	"bt7tank",
	"kv1tank",
	"flametank",
	"stug3tank",
	"tigertank",
	"flak88antitank",
	"pakantitank",
	"misc_panzeriv",
    "misc_stug3",
    "misc_kv1",
    "misc_flak88",
    "misc_pak",
	"npc_tank",
	"npc_churchhill",
	"npc_panzer",
	"npc_tank_heavy",
	"npc_tank_medium",
	"npc_tank_light",
    "npc_artillery",
	"npc_flametank",
	"churchilltank",
	"panzertank",
	"panzer2tank",
#endif //__VEHICLES__*/
	"",
};
static const char	*botSniperGoalsList[] = { "bot_sniper_spot", "misc_stealth_area", "", };
static const char	*botDefenceGoalsList[] = { "bot_allied_defence_point", "bot_axis_defence_point", "misc_stealth_area", "", };
#define AXIS_OBJECTIVE		1
#define ALLIED_OBJECTIVE	2
#define MESSAGE_OVERRIDE	4
#define TANK				8
#define ONLY_AXIS			0
#define ONLY_ALLIED			1
#define ISOBJECTIVE			2
#define ISHEALTHAMMOCABINET 4
#define ISCOMMANDPOST		8


/* */
int
GoalType ( int entNum, int teamNum )
{								// Returns goal type for a goal number. Takes teamNum into account for constructables...
	int type = BOTGOAL_ROAM;	// Default.
	if ( !Q_stricmp( g_entities[entNum].classname, "func_constructible") 
		|| !Q_stricmp( g_entities[entNum].classname, "sandbag_constructible")
		|| !Q_stricmp( g_entities[entNum].classname, "mg_nest_constructible") )
	{
		if ( g_entities[entNum].s.teamNum == teamNum )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else
		{
			type = BOTGOAL_DESTROY;
		}
	}
	else if
		(
			!Q_stricmp( g_entities[entNum].classname, "misc_commandmap_marker") ||
			g_entities[entNum].s.eType == ET_COMMANDMAP_MARKER
		)
	{
		if ( teamNum == TEAM_AXIS && (g_entities[entNum].spawnflags == ONLY_AXIS) )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else if ( teamNum == TEAM_ALLIES && (g_entities[entNum].spawnflags == ONLY_ALLIED) )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else if ( teamNum != TEAM_AXIS && (g_entities[entNum].spawnflags == ONLY_AXIS) )
		{
			type = BOTGOAL_DESTROY;
		}
		else if ( teamNum != TEAM_ALLIES && (g_entities[entNum].spawnflags == ONLY_ALLIED) )
		{
			type = BOTGOAL_DESTROY;
		}
		else
		{
			type = BOTGOAL_ROAM;
		}
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "trigger_objective_info") )
	{
		if ( g_entities[entNum].spawnflags & TANK )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else if ( (g_entities[entNum].spawnflags & AXIS_OBJECTIVE) && teamNum == TEAM_AXIS )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else if ( (g_entities[entNum].spawnflags & ALLIED_OBJECTIVE) && teamNum == TEAM_ALLIES )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else
		{
			type = BOTGOAL_DESTROY;
		}
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "ai_node") )
	{
		type = BOTGOAL_ROAM;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "ai_marker") )
	{
		type = BOTGOAL_ROAM;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "item_botroam") )
	{
		type = BOTGOAL_ROAM;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_mg42") )
	{
		type = BOTGOAL_USE;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_aagun") )
	{
		type = BOTGOAL_USE;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_cabinet_health") )
	{
		type = BOTGOAL_FINDHEALTH;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "health_crate") )
	{
		type = BOTGOAL_FINDHEALTH;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_cabinet_supply") )
	{
		type = BOTGOAL_FINDAMMO;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "ammo_crate") )
	{
		type = BOTGOAL_FINDAMMO;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "func_secret") )
	{
		type = BOTGOAL_ROAM;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "info_camp") )
	{
		type = BOTGOAL_CAMP;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "team_CTF_redflag") )
	{
		type = BOTGOAL_FLAG;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "team_CTF_blueflag") )
	{
		type = BOTGOAL_FLAG;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "info_player_checkpoint") )
	{
		type = BOTGOAL_CHECKPOINT;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "team_WOLF_objective") )
	{

		// We'll need to implement this...

		/*if (level.teamExchange)
		{
			if (ent->spawnflags & 1) {
				ent->count2 = TEAM_ALLIES;
			} else if (ent->spawnflags & 2) {
				ent->count2 = TEAM_AXIS;
			}
		}*/
		type = BOTGOAL_OBJECTIVE;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "team_WOLF_checkpoint") )
	{
		type = BOTGOAL_CHECKPOINT;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_control_point") )
	{
		type = BOTGOAL_POPFLAG;
	}
	else
	{
		type = BOTGOAL_ROAM;
	}

	return ( type );
}


/* */
qboolean
BotGoalValidForTeam ( int entNum, int teamNum )
{								// Is this goal for our team???
	int type = BOTGOAL_ROAM;	// Default.
	if ( !Q_stricmp( g_entities[entNum].classname, "func_constructible")
		|| !Q_stricmp( g_entities[entNum].classname, "sandbag_constructible")
		|| !Q_stricmp( g_entities[entNum].classname, "mg_nest_constructible") )
	{
		if ( g_entities[entNum].s.teamNum == teamNum )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else
		{
			type = BOTGOAL_DESTROY;
		}
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "trigger_objective_info") )
	{
		if ( g_entities[entNum].spawnflags & TANK )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else if ( (g_entities[entNum].spawnflags & AXIS_OBJECTIVE) && teamNum == TEAM_AXIS )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else if ( (g_entities[entNum].spawnflags & ALLIED_OBJECTIVE) && teamNum == TEAM_ALLIES )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else
		{
			return ( qfalse );
		}
	}
	else if
		(
			!Q_stricmp( g_entities[entNum].classname, "misc_commandmap_marker") ||
			g_entities[entNum].s.eType == ET_COMMANDMAP_MARKER
		)
	{
		if ( teamNum == TEAM_AXIS && g_entities[entNum].spawnflags & ONLY_AXIS )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else if ( teamNum == TEAM_ALLIES && g_entities[entNum].spawnflags & ONLY_ALLIED )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else if ( teamNum != TEAM_AXIS && g_entities[entNum].spawnflags & ONLY_AXIS )
		{
			type = BOTGOAL_DESTROY;
		}
		else if ( teamNum != TEAM_ALLIES && g_entities[entNum].spawnflags & ONLY_ALLIED )
		{
			type = BOTGOAL_DESTROY;
		}
		else
		{
			type = BOTGOAL_ROAM;
		}

		return ( qtrue );
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_stealth_area") )
	{
		type = BOTGOAL_ROAM;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "ai_node") )
	{
		type = BOTGOAL_ROAM;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "ai_marker") )
	{
		type = BOTGOAL_ROAM;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "item_botroam") )
	{
		type = BOTGOAL_ROAM;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_mg42") )
	{
		type = BOTGOAL_USE;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_aagun") )
	{
		type = BOTGOAL_USE;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_cabinet_health") )
	{
		type = BOTGOAL_FINDHEALTH;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "health_crate") )
	{
		type = BOTGOAL_FINDHEALTH;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_cabinet_supply") )
	{
		type = BOTGOAL_FINDAMMO;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "ammo_crate") )
	{
		type = BOTGOAL_FINDAMMO;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "func_secret") )
	{
		type = BOTGOAL_ROAM;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "info_camp") )
	{
		type = BOTGOAL_CAMP;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "team_CTF_redflag") )
	{
		type = BOTGOAL_FLAG;
		if ( teamNum != TEAM_ALLIES )
		{
			return ( qfalse );
		}
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "team_CTF_blueflag") )
	{
		type = BOTGOAL_FLAG;
		if ( teamNum != TEAM_AXIS )
		{
			return ( qfalse );
		}
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "info_player_checkpoint") )
	{
		type = BOTGOAL_CHECKPOINT;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "team_WOLF_objective") )
	{
		type = BOTGOAL_OBJECTIVE;
		if ( g_entities[entNum].count2 != teamNum )
		{
			return ( qfalse );
		}
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "team_WOLF_checkpoint") )
	{

		//type = BOTGOAL_CHECKPOINT;
		//if (g_entities[entNum].count != teamNum)
		//			return qfalse;
	}
	else if ( !Q_stricmp( g_entities[entNum].classname, "misc_control_point") )
	{
		type = BOTGOAL_POPFLAG;
		if ( g_entities[entNum].s.modelindex == teamNum && g_entities[entNum].s.time2 <= 0 )
		{
			return ( qfalse );
		}
	}
	else
	{
		type = BOTGOAL_ROAM;
	}

	return ( qtrue );
}


/* */
qboolean
Is_Engineer_Goal ( int entNum )
{	// Returns qtrue if the goal is for an engineer...
	/*
	"func_constructible",
	"misc_constructiblemarker",
	"team_WOLF_objective",
	"misc_control_point",
	"explosive_indicator",
	"script_mover",
	"trigger_objective_info",
*/
	if
	(
		!Q_stricmp( g_entities[entNum].classname, "misc_commandmap_marker") ||
		g_entities[entNum].s.eType == ET_COMMANDMAP_MARKER
	)
	{
		if ( (g_entities[entNum].spawnflags & ISCOMMANDPOST) || (g_entities[entNum].spawnflags & ISOBJECTIVE) )
		{
			return ( qtrue );
		}
		else
		{
			return ( qfalse );
		}
	}

	if ( !Q_stricmp( g_entities[entNum].classname, "func_constructible")
		|| !Q_stricmp( g_entities[entNum].classname, "sandbag_constructible")
		|| !Q_stricmp( g_entities[entNum].classname, "mg_nest_constructible") )
	{
		return ( qtrue );
	}

	if ( !Q_stricmp( g_entities[entNum].classname, "misc_constructiblemarker") )
	{
		return ( qtrue );
	}

	if ( !Q_stricmp( g_entities[entNum].classname, "explosive_indicator") )
	{
		return ( qtrue );
	}

	if ( !Q_stricmp( g_entities[entNum].classname, "func_explosive") )
	{
		return ( qtrue );
	}

	if ( !Q_stricmp( g_entities[entNum].classname, "script_mover") )
	{
		return ( qtrue );
	}

	//	else if (!Q_stricmp( g_entities[entNum].classname, "misc_mg42" ))
	//		type = BOTGOAL_USE;
	//	else if (!Q_stricmp( g_entities[entNum].classname, "misc_aagun" ))
	//		type = BOTGOAL_USE;
	return ( qfalse );
}


/* */
qboolean
BotMP_Is_Acceptable_Goal_Entity ( int entityNum )
{
	int loop = 0;
	while ( botGoalsList[loop] )
	{
		if ( g_entities[entityNum].classname == botGoalsList[loop] )
		{
			return ( qtrue );
		}

		loop++;
	}

	return ( qfalse );
}

//extern node_t	nodes[MAX_NODES];
extern node_t	*nodes;
extern int		number_of_nodes;
vec3_t			snipe_points[256];
int				num_snipe_points = 0;
qboolean		snipe_points_generated = qfalse;
extern int		AIMOD_NAVIGATION_FindAbsoluteClosestReachableNode ( gentity_t *bot, int r, int type );


/* */
void
BotMP_Add_Snipe_Point ( gentity_t *ent )
{

	/*	//G_Printf("Snipe point added!\n");
	VectorCopy(ent->s.origin, snipe_points[num_snipe_points]);
	num_snipe_points++;
	G_FreeEntity(ent);*/
}

/*void BotMP_Generate_Snipe_Positions ( void )
{
	float highest = -64000;
	float lowest = 64000;
	float max_diff;
	int nodeNum = 0;

	if (number_of_nodes <= 0)
		return;

	if (snipe_points_generated)
		return;

	snipe_points_generated = qtrue;

	if (num_snipe_points > 0)
		return; // Have real ones!

	for (nodeNum = 0; nodeNum < number_of_nodes; nodeNum++)
	{// First, find the highest and  points...
		if (nodes[nodeNum].origin[2] > highest)
			highest = nodes[nodeNum].origin[2];
		
		if (nodes[nodeNum].origin[2] < lowest)
			lowest = nodes[nodeNum].origin[2];
	}

	max_diff = highest - lowest;
	max_diff *= 0.99; // Only use the top 1%...

	for (nodeNum = 0; nodeNum < number_of_nodes; nodeNum++)
	{// Now, mark the sniper nodes...
		if (num_snipe_points >= 256)
			break; // Enough already!

		if (highest - nodes[nodeNum].origin[2] > max_diff)
		{// Add a new sniper point to our list...
			VectorCopy(nodes[nodeNum].origin, snipe_points[num_snipe_points]);
			num_snipe_points++;
		}
	}

	num_snipe_points--;

	if (num_snipe_points < 0)
		num_snipe_points = 0;

//	if (num_snipe_points > 0)
//		return; // Have real ones!

//	num_snipe_points = 0;
}*/
qboolean	attackingTeamSet = qfalse;


/* */
void
Set_Attacking_Team ( void )
{			// Set the level's attacking team number...
	if ( !attackingTeamSet )
	{
		if ( numAxisDefencePoints > 0 || numAlliedDefencePoints > 0 )
		{	// PRIMARY: If one team has more defence points, they are the obvious defenders...
			if ( numAlliedDefencePoints <= numAxisDefencePoints )
			{
				level.attackingTeam = TEAM_AXIS;
			}
			else
			{
				level.attackingTeam = TEAM_ALLIES;
			}
		}
		else if ( num_allied_destroyables >= num_axis_destroyables )
		{	// FALLBACK: Whoever has the most destroyables on the first think must be the attacking team... (I hope hehehe)...
			level.attackingTeam = TEAM_ALLIES;
		}
		else
		{
			level.attackingTeam = TEAM_AXIS;
		}

		attackingTeamSet = qtrue;
	}
}


/* */
void
BotMP_Init_Goal_Lists ( void )
{
	int loop = 0;
	for ( loop = 0; loop < 1024; loop++ )
	{
		BotObjectives[loop] = -1;
		BotGoals[loop] = -1;
	}

	for ( loop = 0; loop < 256; loop++ )
	{
		alliedDestroyables[loop] = -1;
		axisDestroyables[loop] = -1;
		alliedBuildables[loop] = -1;
		axisBuildables[loop] = -1;
	}
}

extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );
extern int		NodeVisible ( vec3_t org1, vec3_t org2, int ignore );


/* */
void
BotMP_Calculate_In_Range_Nodes ( gentity_t *ent )
{
	int		i;
	int		upto = 0;
	int		best_nodes[MAX_NODES];
	int		best_node = -1;
	int		selectable_nodes[MAX_NODES];
	int		selection_upto = 0;
	vec3_t	origin;
	if ( number_of_nodes <= 0 )
	{
		return;
	}

	if ( ent->num_in_range_nodes > 0 )
	{
		return; // Already worked it out!
	}

	if ( ent->r.currentOrigin[0] != 0 )
	{
		VectorCopy( ent->r.currentOrigin, origin );
	}
	else if ( ent->s.origin[0] != 0 )
	{
		VectorCopy( ent->s.origin, origin );
	}
	else
	{
		VectorCopy( ent->s.pos.trBase, origin );
	}

	origin[2]+=48;

	ent->is_bot_goal = qtrue;
	best_nodes[0] = -1;

	for ( i = 0; i < number_of_nodes; i++ )
	{
		float	best_weight = 32.0f;
		float	weight = VectorDistance( origin, nodes[i].origin );
		if ( weight > best_weight )
		{
			continue;
		}

		if ( BAD_WP_Height( origin, nodes[i].origin) )
		{
			continue;
		}

		selectable_nodes[selection_upto] = i;
		selection_upto++;
	}

	if (selection_upto < 2 || (!Q_stricmp( ent->classname, va( "bot_sniper_spot")) && selection_upto < 1))
	{
		for ( i = 0; i < number_of_nodes; i++ )
		{
			float	best_weight = 64.0f;//256.0f;
			float	weight = VectorDistance( origin, nodes[i].origin );
			if ( weight > best_weight )
			{
				continue;
			}

			if ( BAD_WP_Height( origin, nodes[i].origin) )
			{
				continue;
			}

			selectable_nodes[selection_upto] = i;
			selection_upto++;
		}
	}

	if (selection_upto < 2 || (!Q_stricmp( ent->classname, va( "bot_sniper_spot")) && selection_upto < 1))
	{
		for ( i = 0; i < number_of_nodes; i++ )
		{
			float	best_weight = 96.0f;//256.0f;
			float	weight = VectorDistance( origin, nodes[i].origin );
			if ( weight > best_weight )
			{
				continue;
			}

			if ( BAD_WP_Height( origin, nodes[i].origin) )
			{
				continue;
			}

			selectable_nodes[selection_upto] = i;
			selection_upto++;
		}
	}

	if (selection_upto < 2 || (!Q_stricmp( ent->classname, va( "bot_sniper_spot")) && selection_upto < 1))
	{
		for ( i = 0; i < number_of_nodes; i++ )
		{
			float	best_weight = 128.0f;//256.0f;
			float	weight = VectorDistance( origin, nodes[i].origin );
			if ( weight > best_weight )
			{
				continue;
			}

			if ( BAD_WP_Height( origin, nodes[i].origin) )
			{
				continue;
			}

			selectable_nodes[selection_upto] = i;
			selection_upto++;
		}
	}

	if (selection_upto < 2 || (!Q_stricmp( ent->classname, va( "bot_sniper_spot")) && selection_upto < 1))
	{
		for ( i = 0; i < number_of_nodes; i++ )
		{
			float	best_weight = 192.0f;//256.0f;
			float	weight = VectorDistance( origin, nodes[i].origin );
			if ( weight > best_weight )
			{
				continue;
			}

			if ( BAD_WP_Height( origin, nodes[i].origin) )
			{
				continue;
			}

			selectable_nodes[selection_upto] = i;
			selection_upto++;
		}
	}

	if (selection_upto < 2 || (!Q_stricmp( ent->classname, va( "bot_sniper_spot")) && selection_upto < 1))
	{
		for ( i = 0; i < number_of_nodes; i++ )
		{
			float	best_weight = 256.0f;//256.0f;
			float	weight = VectorDistance( origin, nodes[i].origin );
			if ( weight > best_weight )
			{
				continue;
			}

			if ( BAD_WP_Height( origin, nodes[i].origin) )
			{
				continue;
			}

			selectable_nodes[selection_upto] = i;
			selection_upto++;
		}
	}

	for ( i = 0; i < selection_upto; i++ )
	{
		int		j;
		float	best_weight = 256.0f;
		for ( j = 0; j < selection_upto; j++ )
		{
			qboolean	already_in_list = qfalse;
			int			z;
			float		weight2 = VectorDistance( origin, nodes[selectable_nodes[j]].origin );
			if ( weight2 > best_weight )
			{
				continue;
			}

			for ( z = 0; z < upto; z++ )
			{
				if ( best_nodes[z] == selectable_nodes[j] )
				{
					already_in_list = qtrue;
					break;
				}
			}

			if ( !already_in_list && weight2 < best_weight )
			{	// Best so far...
				best_node = selectable_nodes[j];
				best_weight = weight2;
			}
		}

		best_nodes[upto] = best_node;
		upto++;
	}

	for ( i = 0; i < upto; i++ )
	{			// Return the closest one that is visible, checking visibilities only as far as needed for CPU time saving...
		if ( NodeVisible( origin, nodes[best_nodes[i]].origin, ent->s.number) >= 1 )
		{
			ent->in_range_nodes[ent->num_in_range_nodes] = best_nodes[i];
			ent->num_in_range_nodes++;
		}
	}

	if ( ent->num_in_range_nodes <= 0 )
	{
		for ( i = 0; i < upto; i++ )
		{
			ent->in_range_nodes[ent->num_in_range_nodes] = best_nodes[i];
			ent->num_in_range_nodes++;
		}
	}

	if ( ent->num_in_range_nodes <= 0 )
	{			// Failed!!! Set one to -1
		ent->in_range_nodes[ent->num_in_range_nodes] = -1;
		ent->num_in_range_nodes = 0;//++;
	}

	//	G_Printf("Entity %i (%s) has %i in-range nodes. Origin %f %f %f.\n", ent->s.number, ent->classname, ent->num_in_range_nodes,
	//		origin[0], origin[1], origin[2]);
}

extern void BotDropToFloor ( gentity_t *ent );


/* */
void
SP_bot_allied_defence_point ( gentity_t *ent )
{
	ent->s.origin[2] += 16;
	if ( !(ent->spawnflags & 1) )
	{
		BotDropToFloor( ent );
	}

	ent->r.svFlags |= SVF_NOCLIENT;
	ent->s.eType = ET_DEFENCEPOINT_HINT;
	ent->s.teamNum = TEAM_ALLIES;
}


/* */
void
SP_bot_axis_defence_point ( gentity_t *ent )
{
	ent->s.origin[2] += 16;
	if ( !(ent->spawnflags & 1) )
	{
		BotDropToFloor( ent );
	}

	ent->r.svFlags |= SVF_NOCLIENT;
	ent->s.eType = ET_DEFENCEPOINT_HINT;
	ent->s.teamNum = TEAM_AXIS;
}

qboolean snipe_spots_generated = qfalse;

vmCvar_t fs_game;

qboolean
AIMOD_LoadSniperSpots ( void )
{
	FILE			*pIn;
	vmCvar_t		mapname;
	int				i = 0;
	int				num_map_waypoints = 0;
	int				num_sniper_spots = 0;
	vec3_t			spot_org;

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	///////////////////
	//open the node file for reading, return false on error
	trap_Cvar_Register( &fs_game, "fs_game", "", CVAR_SERVERINFO | CVAR_ROM );

	if ( (pIn = fopen( va("%s/nodes/%s.spf", fs_game.string, mapname.string), "rb")) == NULL )
	{
		if ( (pIn = fopen( va("nodes/%s.spf", mapname.string), "rb")) == NULL )
		{
			G_Printf( "^1*** ^3%s^5: Reading sniper spots from ^7%s/nodes/%s.spf^3 failed^5!!!\n", GAME_VERSION, fs_game.string, mapname.string );
			return qfalse;
		}
	}

	fread( &num_map_waypoints, sizeof(int), 1, pIn );

	if (num_map_waypoints != number_of_nodes)
	{// Is an old file! We need to make a new one!
		G_Printf( "^1*** ^3%s^5: Reading sniper spots from ^7%s/nodes/%s.spf^3 failed^5!!!\n", GAME_VERSION, fs_game.string, mapname.string );
		fclose( pIn );
		return qfalse;
	}

	fread( &num_sniper_spots, sizeof(int), 1, pIn );
	
	for ( i = 0; i < num_sniper_spots; i++ )
	{
		gentity_t *spot;

		fread( &spot_org, sizeof(vec3_t), 1, pIn );
		
		spot = G_Spawn();
		spot->classname = "bot_sniper_spot";
		VectorCopy(spot_org, spot->s.origin);
		//G_Printf("Loaded spot at %f %f %f.\n", spot_org[0], spot_org[1], spot_org[2]);
	}

	fclose( pIn );

	G_Printf( "^1*** ^3%s^5: Successfully loaded %i sniper spots from file ^7%s/nodes/%s.spf^5.\n", GAME_VERSION, num_sniper_spots, fs_game.string, mapname.string);

	return qtrue;
}

void
AIMOD_SaveSniperSpots ( void )
{
	fileHandle_t	f;
	int				i;
	vmCvar_t		mapname;
	char			map[64] = "";
	char			filename[60];
	int				num_sniper_spots = 0;
	vec3_t			spots[256];

	for (i = 0; i < MAX_GENTITIES; i++)
	{
		gentity_t *ent = &g_entities[i];

		if (!ent)
			continue;

		if (Q_stricmp(ent->classname, va( "bot_sniper_spot")))
			continue;

		VectorCopy(ent->s.origin, spots[num_sniper_spots]);
		num_sniper_spots++;
	}

	G_Printf( "^3*** ^3%s: ^7Saving sniper spots table.\n", "AUTO-WAYPOINTER" );
	strcpy( filename, "nodes/" );

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	Q_strcat( filename, sizeof(filename), mapname.string );

	///////////////////
	//try to open the output file, return if it failed
	trap_FS_FOpenFile( va( "%s.spf", filename), &f, FS_WRITE );
	if ( !f )
	{
		G_Printf( "^1*** ^3ERROR^5: Error opening sniper spots file ^7%s.spf^5!!!\n", "AUTO-WAYPOINTER", filename );
		return;
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );	//get the map name
	strcpy( map, mapname.string );

	trap_FS_Write( &number_of_nodes, sizeof(int), f );							//write the number of nodes in the map
	trap_FS_Write( &num_sniper_spots, sizeof(int), f );							//write the number of sniper spots in the map

	for ( i = 0; i < num_sniper_spots; i++ )											//loop through all the nodes
	{
		trap_FS_Write( &(spots[i]), sizeof(vec3_t), f );
	}

	trap_FS_FCloseFile( f );		

	G_Printf( "^3*** ^3%s: ^5Sniper spots table saved to file ^7%s.spf^5.\n", "AUTO-WAYPOINTER", filename );
}

#ifndef __CUDA__
void BotMP_Generate_Sniper_Spots ( void )
{// UQ1: *FIXME* Add them based on how many waypoints are visible beneath them!!!
	int loop = 0;
	qboolean bad_spot = qfalse;
	int num_found = 0;
	int num_possible_spots = 0;
	int possible_spots[MAX_NODES];
	int num_final_spots = 0;
	int final_spots[MAX_NODES];
	int best_high_node = -1;
	float best_high_height = -999999.0f;
	int best_low_node = -1;
	float best_low_height = 999999.0f;
	float use_height = -64000.0f;
	int remove_percentage = 0;
	int num_removed = 0;
	int num_added = 0;
	int goals[3];
	gentity_t *goal_ents[3];

	goals[0] = -1;
	goal_ents[0] = NULL;
	goals[1] = -1;
	goal_ents[1] = NULL;
	goals[2] = -1;
	goal_ents[2] = NULL;
	

	if (snipe_spots_generated || number_of_nodes <= 0)
		return;

	snipe_spots_generated = qtrue;

	for ( loop = 0; loop < MAX_GENTITIES; loop++ )
	{
		int			nameNum = 0;
		gentity_t	*ent = &g_entities[loop];

		if (goals[2] >= 0)
			break;

		for ( nameNum = 0; Q_stricmp( botGoalsList[nameNum], ""); nameNum++ )
		{
			if ( ent->classname && !Q_stricmp( ent->classname, va( "%s", botGoalsList[nameNum])) )
			{
				if (goals[0] < 0)
				{
					goals[0] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( ent->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
					goal_ents[0] = ent;
				}
				else if (goals[1] < 0)
				{
					goals[1] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( ent->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
					goal_ents[1] = ent;
				}
				else if (goals[2] < 0)
				{
					goals[2] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( ent->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
					goal_ents[2] = ent;
					break;
				}
				else
				{
					break;
				}
			}
		}
	}

	for ( loop = 0; loop < MAX_GENTITIES; loop++ )
	{
//		int			nameNum = 0;
		gentity_t	*ent = &g_entities[loop];

		if ( !ent )
		{
			continue;
		}

		if ( !ent->classname )
		{
			continue;
		}

		if ( !Q_stricmp( ent->classname, va( "bot_sniper_spot")) )
		{
			num_found++;
		}
	}

	if (num_found >= 10)
		return; // The map already has enough of them...

	if (AIMOD_LoadSniperSpots())
		return;

	for (loop = 0; loop < number_of_nodes; loop++)
	{// First fight the highest and lowest nodes...
		if (nodes[loop].origin[2] > best_high_height)
		{
			best_high_height = nodes[loop].origin[2];
			best_high_node = loop;
		}

		if (nodes[loop].origin[2] < best_low_height)
		{
			best_low_height = nodes[loop].origin[2];
			best_low_node = loop;
		}
	}

	// Now we have a best height, pick an above average height to set as the minimum.
	use_height = best_low_height + ((best_high_height - best_low_height) * 0.65);

	for (loop = 0; loop < number_of_nodes; loop++)
	{// Go through the nodes and find some possible spots...
		int loop2;

		if (nodes[loop].origin[2] >= use_height)
		{
			int number_of_snipable_waypoints = 0;
			/*short*/ int	pathlist[MAX_NODES];

			if ( !(CreatePathAStar( goal_ents[0], goals[0], loop, pathlist ) > 0
				|| CreatePathAStar( goal_ents[1], goals[1], loop, pathlist ) > 0
				|| CreatePathAStar( goal_ents[2], goals[2], loop, pathlist ) > 0) )
				continue;

			for (loop2 = 0; loop2 < number_of_nodes; loop2++)
			{// First a fast check for numbers without visibilities!
				if (VectorDistance(nodes[loop].origin, nodes[loop2].origin) < 1024 || VectorDistance(nodes[loop].origin, nodes[loop2].origin) > 2048)
					continue; // Too close or too far to bother checking...

				if (nodes[loop].origin[2] - nodes[loop2].origin[2] >= 256)
					continue; // Not far enough below us!

				number_of_snipable_waypoints++;

				if (number_of_snipable_waypoints >= 25)
					break; // Plenty!
			}

			if (number_of_snipable_waypoints < 25)
				continue; // Not enough!

			number_of_snipable_waypoints = 0;

			// Now do vis checks on the points...

			for (loop2 = 0; loop2 < number_of_nodes; loop2++)
			{
				vec3_t test_pos;

				if (VectorDistance(nodes[loop].origin, nodes[loop2].origin) < 1024 || VectorDistance(nodes[loop].origin, nodes[loop2].origin) > 2048)
					continue; // Too close or too far to bother checking...

				if (nodes[loop].origin[2] - nodes[loop2].origin[2] >= 256)
					continue; // Not far enough below us!

				VectorCopy(nodes[loop].origin, test_pos);
				test_pos[2]+=48;

				//if (NodeVisible(test_pos, nodes[loop2].origin, -1))
				if (NodeVisible(nodes[loop2].origin, test_pos, -1))
				{
					number_of_snipable_waypoints++;

					if (number_of_snipable_waypoints >= 5)
					{
						break;
					}
				}
			}

			if (number_of_snipable_waypoints >= 5)
			{
				possible_spots[num_possible_spots] = loop;
				num_possible_spots++;
			}
		}
	}

	if (num_possible_spots <= 48)
	{// Add some without the height check... Just so coverts have some goals...
		for (loop = 0; loop < number_of_nodes; loop++)
		{// Go through the nodes and find some possible spots...
			int loop2;
			int number_of_snipable_waypoints = 0;
			/*short*/ int	pathlist[MAX_NODES];

			if ( !(CreatePathAStar( goal_ents[0], goals[0], loop, pathlist ) > 0
				|| CreatePathAStar( goal_ents[1], goals[1], loop, pathlist ) > 0
				|| CreatePathAStar( goal_ents[2], goals[2], loop, pathlist ) > 0) )
				continue;

			for (loop2 = 0; loop2 < number_of_nodes; loop2++)
			{// First a fast check for numbers without visibilities!
				if (VectorDistance(nodes[loop].origin, nodes[loop2].origin) < 1024 || VectorDistance(nodes[loop].origin, nodes[loop2].origin) > 2048)
					continue; // Too close or too far to bother checking...

				number_of_snipable_waypoints++;

				if (number_of_snipable_waypoints >= 25)
					break; // Plenty!
			}

			if (number_of_snipable_waypoints < 25)
				continue; // Not enough!

			number_of_snipable_waypoints = 0;

			// Now do vis checks on the points...

			for (loop2 = 0; loop2 < number_of_nodes; loop2++)
			{
				vec3_t test_pos, test_pos2;
					
				if (VectorDistance(nodes[loop].origin, nodes[loop2].origin) < 1024 || VectorDistance(nodes[loop].origin, nodes[loop2].origin) > 2048)
					continue; // Too close or too far to bother checking...

				VectorCopy(nodes[loop].origin, test_pos);
				test_pos[2]+=48;

				VectorCopy(nodes[loop2].origin, test_pos2);
				test_pos2[2]+=48;

				if (NodeVisible(test_pos2, test_pos, -1))
				{
					number_of_snipable_waypoints++;

					if (number_of_snipable_waypoints >= 5)
					{
						break;
					}
				}
			}

			if (number_of_snipable_waypoints >= 5)
			{
				possible_spots[num_possible_spots] = loop;
				num_possible_spots++;
			}
		}
	}

	// Always add the first one from the list...
	final_spots[num_final_spots] = possible_spots[0];
	num_final_spots++;

	for (loop = 0; loop < num_possible_spots; loop++)
	{// Go through the possible nodes and find the best spots...
		int loop2 = 0;

		bad_spot = qfalse;

		for (loop2 = 0; loop2 < num_final_spots; loop2++)
		{// Check we do not have any too close by (to save on entities)...
			if (VectorDistance(nodes[final_spots[loop2]].origin, nodes[possible_spots[loop]].origin) < 96/*256*/)
			{
				bad_spot = qtrue;
				break;
			}
		}

		if (!bad_spot)
		{// Looks good! Use it!
			final_spots[num_final_spots] = possible_spots[loop];
			num_final_spots++;
		}
	}

	if (num_final_spots >= 64)
	{// Too many! Need to reduce numbers!
		remove_percentage = ((float)(64/(float)num_final_spots)) * 100.0f;
	}

	// We now have spots, now generate the entities!
	for (loop = 0; loop < num_final_spots; loop++)
	{
		gentity_t *spot;

		if (remove_percentage >= 1)
		{// Too many! Need to reduce numbers! -- Randomized, so not exact, but it varies the spots between games...
			if (Q_irand(0, 100) >= remove_percentage)
			{
				num_removed++;
				continue;
			}
		}

		spot = G_Spawn();
		spot->classname = "bot_sniper_spot";
		VectorCopy(nodes[final_spots[loop]].origin, spot->s.origin);
		num_added++;

		if (num_added >= 64)
			break;
	}

	G_Printf( "^4*** ^3%s^4: ^5Generated ^7%i^5 sniper points for this map.\n", GAME_VERSION, (num_final_spots-1)-num_removed );
	
	AIMOD_SaveSniperSpots();
}
#else //__CUDA__
__device__ __host__ void BotMP_Generate_Sniper_Spots ( void )
{// UQ1: *FIXME* Add them based on how many waypoints are visible beneath them!!!
	__shared__ int loop = 0;
	__shared__ qboolean bad_spot = qfalse;
	__shared__ int num_found = 0;
	__shared__ int num_possible_spots = 0;
	__shared__ int possible_spots[MAX_NODES];
	__shared__ int num_final_spots = 0;
	__shared__ int final_spots[MAX_NODES];
	__shared__ int best_high_node = -1;
	__shared__ float best_high_height = -999999.0f;
	__shared__ int best_low_node = -1;
	__shared__ float best_low_height = 999999.0f;
	__shared__ float use_height = -64000.0f;
	__shared__ int remove_percentage = 0;
	__shared__ int num_removed = 0;
	__shared__ int num_added = 0;
	__shared__ int goals[3];
	__shared__ gentity_t *goal_ents[3];

	goals[0] = -1;
	goal_ents[0] = NULL;
	goals[1] = -1;
	goal_ents[1] = NULL;
	goals[2] = -1;
	goal_ents[2] = NULL;

	if (snipe_spots_generated || number_of_nodes <= 0)
		return;

	snipe_spots_generated = qtrue;

	for ( loop = 0; loop < MAX_GENTITIES; loop++ )
	{
		__shared__ int			nameNum = 0;
		__shared__ gentity_t	*ent = &g_entities[loop];

		if (goals[2] >= 0)
			break;

		for ( nameNum = 0; Q_stricmp( botGoalsList[nameNum], ""); nameNum++ )
		{
			if ( ent->classname && !Q_stricmp( ent->classname, va( "%s", botGoalsList[nameNum])) )
			{
				if (goals[0] < 0)
				{
					goals[0] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( ent->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
					goal_ents[0] = ent;
				}
				else if (goals[1] < 0)
				{
					goals[1] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( ent->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
					goal_ents[1] = ent;
				}
				else if (goals[2] < 0)
				{
					goals[2] = AIMOD_NAVIGATION_FindClosestReachableNodeTo( ent->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
					goal_ents[2] = ent;
					break;
				}
				else
				{
					break;
				}
			}
		}
	}

	for ( loop = 0; loop < MAX_GENTITIES; loop++ )
	{
		__shared__ gentity_t	*ent = &g_entities[loop];

		if ( !ent )
		{
			continue;
		}

		if ( !ent->classname )
		{
			continue;
		}

		if ( !Q_stricmp( ent->classname, va( "bot_sniper_spot")) )
		{
			num_found++;
		}
	}

	if (num_found >= 10)
		return; // The map already has enough of them...

	if (AIMOD_LoadSniperSpots())
		return;

	for (loop = 0; loop < number_of_nodes; loop++)
	{// First fight the highest and lowest nodes...
		if (nodes[loop].origin[2] > best_high_height)
		{
			best_high_height = nodes[loop].origin[2];
			best_high_node = loop;
		}

		if (nodes[loop].origin[2] < best_low_height)
		{
			best_low_height = nodes[loop].origin[2];
			best_low_node = loop;
		}
	}

	// Now we have a best height, pick an above average height to set as the minimum.
	use_height = best_low_height + ((best_high_height - best_low_height) * 0.45);

	for (loop = 0; loop < number_of_nodes; loop++)
	{// Go through the nodes and find some possible spots...
		__shared__ int loop2;

		if (nodes[loop].origin[2] >= use_height)
		{
			__shared__ int number_of_snipable_waypoints = 0;
			__shared__ /*short*/ int	pathlist[MAX_NODES];

			if ( !(CreatePathAStar( goal_ents[0], goals[0], loop, pathlist ) > 0
				|| CreatePathAStar( goal_ents[1], goals[1], loop, pathlist ) > 0
				|| CreatePathAStar( goal_ents[2], goals[2], loop, pathlist ) > 0) )
				continue;

			for (loop2 = 0; loop2 < number_of_nodes; loop2++)
			{// First a fast check for numbers without visibilities!
				if (VectorDistance(nodes[loop].origin, nodes[loop2].origin) < 1024 || VectorDistance(nodes[loop].origin, nodes[loop2].origin) > 2048)
					continue; // Too close or too far to bother checking...

				if (nodes[loop].origin[2] - nodes[loop2].origin[2] >= 256)
					continue; // Not far enough below us!

				number_of_snipable_waypoints++;

				if (number_of_snipable_waypoints >= 25)
					break; // Plenty!
			}

			if (number_of_snipable_waypoints < 25)
				continue; // Not enough!

			number_of_snipable_waypoints = 0;

			// Now do vis checks on the points...

			for (loop2 = 0; loop2 < number_of_nodes; loop2++)
			{
				__shared__ vec3_t test_pos;

				if (VectorDistance(nodes[loop].origin, nodes[loop2].origin) < 1024 || VectorDistance(nodes[loop].origin, nodes[loop2].origin) > 2048)
					continue; // Too close or too far to bother checking...

				if (nodes[loop].origin[2] - nodes[loop2].origin[2] >= 256)
					continue; // Not far enough below us!

				VectorCopy(nodes[loop].origin, test_pos);
				test_pos[2]+=48;

				//if (NodeVisible(test_pos, nodes[loop2].origin, -1))
				if (NodeVisible(nodes[loop2].origin, test_pos, -1))
				{
					number_of_snipable_waypoints++;

					if (number_of_snipable_waypoints >= 5)
					{
						break;
					}
				}
			}

			if (number_of_snipable_waypoints >= 5)
			{
				possible_spots[num_possible_spots] = loop;
				num_possible_spots++;
			}
		}
	}

	if (num_possible_spots <= 48)
	{// Add some without the height check... Just so coverts have some goals...
		for (loop = 0; loop < number_of_nodes; loop++)
		{// Go through the nodes and find some possible spots...
			__shared__ int loop2;
			__shared__ int number_of_snipable_waypoints = 0;
			__shared__ /*short*/ int	pathlist[MAX_NODES];

			if ( !(CreatePathAStar( goal_ents[0], goals[0], loop, pathlist ) > 0
				|| CreatePathAStar( goal_ents[1], goals[1], loop, pathlist ) > 0
				|| CreatePathAStar( goal_ents[2], goals[2], loop, pathlist ) > 0) )
				continue;

			for (loop2 = 0; loop2 < number_of_nodes; loop2++)
			{// First a fast check for numbers without visibilities!
				if (VectorDistance(nodes[loop].origin, nodes[loop2].origin) < 1024 || VectorDistance(nodes[loop].origin, nodes[loop2].origin) > 2048)
					continue; // Too close or too far to bother checking...

				number_of_snipable_waypoints++;

				if (number_of_snipable_waypoints >= 25)
					break; // Plenty!
			}

			if (number_of_snipable_waypoints < 25)
				continue; // Not enough!

			number_of_snipable_waypoints = 0;

			// Now do vis checks on the points...

			for (loop2 = 0; loop2 < number_of_nodes; loop2++)
			{
				__shared__ vec3_t test_pos, test_pos2;
					
				if (VectorDistance(nodes[loop].origin, nodes[loop2].origin) < 1024 || VectorDistance(nodes[loop].origin, nodes[loop2].origin) > 2048)
					continue; // Too close or too far to bother checking...

				VectorCopy(nodes[loop].origin, test_pos);
				test_pos[2]+=48;

				VectorCopy(nodes[loop2].origin, test_pos2);
				test_pos2[2]+=48;

				if (NodeVisible(test_pos2, test_pos, -1))
				{
					number_of_snipable_waypoints++;

					if (number_of_snipable_waypoints >= 5)
					{
						break;
					}
				}
			}

			if (number_of_snipable_waypoints >= 5)
			{
				possible_spots[num_possible_spots] = loop;
				num_possible_spots++;
			}
		}
	}

	// Always add the first one from the list...
	final_spots[num_final_spots] = possible_spots[0];
	num_final_spots++;

	for (loop = 0; loop < num_possible_spots; loop++)
	{// Go through the possible nodes and find the best spots...
		__shared__ int loop2 = 0;

		bad_spot = qfalse;

		for (loop2 = 0; loop2 < num_final_spots; loop2++)
		{// Check we do not have any too close by (to save on entities)...
			if (VectorDistance(nodes[final_spots[loop2]].origin, nodes[possible_spots[loop]].origin) < 96/*256*/)
			{
				bad_spot = qtrue;
				break;
			}
		}

		if (!bad_spot)
		{// Looks good! Use it!
			final_spots[num_final_spots] = possible_spots[loop];
			num_final_spots++;
		}
	}

	if (num_final_spots >= 48)
	{// Too many! Need to reduce numbers!
		remove_percentage = ((float)(48/(float)num_final_spots)) * 100.0f;
	}

	// We now have spots, now generate the entities!
	for (loop = 0; loop < num_final_spots; loop++)
	{
		__shared__ gentity_t *spot;

		if (remove_percentage >= 1)
		{// Too many! Need to reduce numbers! -- Randomized, so not exact, but it varies the spots between games...
			if (Q_irand(0, 100) >= remove_percentage)
			{
				num_removed++;
				continue;
			}
		}

		spot = G_Spawn();
		spot->classname = "bot_sniper_spot";
		VectorCopy(nodes[final_spots[loop]].origin, spot->s.origin);
		num_added++;

		if (num_added >= 64)
			break;
	}

	G_Printf( "^4*** ^3%s^4: ^5Generated ^7%i^5 sniper points for this map.\n", GAME_VERSION, (num_final_spots-1)-num_removed );
	
	AIMOD_SaveSniperSpots();
}
#endif //__CUDA__


#ifndef __CUDA__

/* */
void
BotMP_Update_Goal_Lists ( void )
{				// Create arrays containing up to date valid goal entity numbers...
	int loop = 0;

	// Initialize the max values...
	num_bot_objectives = 0;
	num_bot_goals = 0;
	num_axis_buildables = 0;
	num_allied_buildables = 0;
	num_axis_destroyables = 0;
	num_allied_destroyables = 0;
	numAlliedDefencePoints = 0;
	numAxisDefencePoints = 0;
	numDefencePoints = 0;
	numSniperGoals = 0;

	// Initialize the lists so there is no left-overs...
	BotMP_Init_Goal_Lists();
	for ( loop = 0; loop < MAX_GENTITIES; loop++ )
	{
		int			nameNum = 0;
		gentity_t	*ent = &g_entities[loop];
//		qboolean	snipe_point_converted = qfalse;

		if ( !ent )
		{
			continue;
		}

		if ( !ent->classname )
		{
			continue;
		}
		{		// Constructibles get their own listing as well... For faster engineer thinking...
			gentity_t	*constructible = G_IsConstructible( TEAM_AXIS, ent );
			if ( constructible != NULL )
			{	// Is still buildable (valid)...
				axisBuildables[num_axis_buildables] = ent->s.number;		//ent->target_ent->s.number;//ent->s.number;
				num_axis_buildables++;
				BotMP_Calculate_In_Range_Nodes( ent );

				//G_Printf("^4*** ^3%s^5: Axis Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
			}

			constructible = G_IsConstructible( TEAM_ALLIES, ent );
			if ( constructible != NULL )
			{																// Is still buildable (valid)...
				alliedBuildables[num_allied_buildables] = ent->s.number;	//ent->target_ent->s.number;//ent->s.number;
				num_allied_buildables++;
				BotMP_Calculate_In_Range_Nodes( ent );

				//G_Printf("^4*** ^3%s^5: Allied Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
			}
		}

		//for (nameNum = 0;botObjectiveList[nameNum] != "";nameNum++)
		for ( nameNum = 0; Q_stricmp( botObjectiveList[nameNum], ""); nameNum++ )
		{
#ifndef __VC6__
			if ( ent->classname && !Q_stricmp( ent->classname, va( "%s", botObjectiveList[nameNum])) )
#else //__VC6__
				if ( ent->classname == botObjectiveList[nameNum] )
#endif //__VC6__
				{
					BotObjectives[num_bot_objectives] = loop;
					num_bot_objectives++;
					BotMP_Calculate_In_Range_Nodes( ent );
					break;
				}
		}

		//for (nameNum = 0;botGoalsList[nameNum] != "";nameNum++)
		for ( nameNum = 0; Q_stricmp( botGoalsList[nameNum], ""); nameNum++ )
		{
#ifndef __VC6__
			if ( ent->classname && !Q_stricmp( ent->classname, va( "%s", botGoalsList[nameNum])) )
#else //__VC6__
				if ( ent->classname == botGoalsList[nameNum] )
#endif //__VC6__
				{
					BotGoals[num_bot_goals] = loop;
					num_bot_goals++;
					BotMP_Calculate_In_Range_Nodes( ent );
					break;
				}
		}

		//for (nameNum = 0;botGoalsList[nameNum] != "";nameNum++)
		for ( nameNum = 0; Q_stricmp( botSniperGoalsList[nameNum], ""); nameNum++ )
		{
#ifndef __VC6__
			if ( ent->classname && !Q_stricmp( ent->classname, va( "%s", botSniperGoalsList[nameNum])) )
#else //__VC6__
				if ( ent->classname == botSniperGoalsList[nameNum] )
#endif //__VC6__
				{
					SniperGoals[numSniperGoals] = loop;
					numSniperGoals++;
					BotMP_Calculate_In_Range_Nodes( ent );
					break;
				}
		}

		for ( nameNum = 0; Q_stricmp( botDefenceGoalsList[nameNum], ""); nameNum++ )
		{
#ifndef __VC6__
			if ( ent->classname && !Q_stricmp( ent->classname, va( "%s", botDefenceGoalsList[nameNum])) )
#else //__VC6__
				if ( ent->classname == botDefenceGoalsList[nameNum] )
#endif //__VC6__
				{
					DefencePoints[numDefencePoints] = loop;
					numDefencePoints++;
					BotMP_Calculate_In_Range_Nodes( ent );
					if ( ent->s.teamNum == TEAM_AXIS )
					{
						numAxisDefencePoints++;
					}
					else if ( ent->s.teamNum == TEAM_ALLIES )
					{
						numAlliedDefencePoints++;
					}
					break;
				}
		}
	}

	// Create our destroyables list...
	for ( loop = 0; loop < MAX_GENTITIES; loop++ )
	{
		gentity_t	*ent = &g_entities[loop];
		gentity_t	*trav = NULL;
		if ( !ent )
		{
			continue;
		}

		if ( !ent->parent )
		{
			continue;
		}

		trav = ent->parent;

		//G_Printf("Scriptname is %s. Targetname is %s.\n", ent->scriptName, ent->targetname);
		if ( !(trav->spawnflags & ALLIED_OBJECTIVE) && !(trav->spawnflags & AXIS_OBJECTIVE) )
		{
			continue;
		}

		if ( !(ent->r.contents & CONTENTS_TRIGGER) )
		{

			//G_Printf("Denied because ( !( ent->r.contents & CONTENTS_TRIGGER ) )\n");
			continue;
		}

		if
		(
			trav->s.eType != ET_OID_TRIGGER &&
			trav->s.eType != ET_EXPLOSIVE_INDICATOR &&
			trav->s.eType != ET_TANK_INDICATOR
		)
		{

			//G_Printf("Denied because ( NO TRIGGER )\n");
			continue;
		}

		if ( ent->spawnflags & 128 )
		{

			//G_Printf("Denied because ( ent->spawnflags & 128 )\n");
			continue;
		}

		if ( !trav->target_ent )
		{
			continue;
		}

		if ( trav->target_ent && Q_stricmp( trav->target_ent->classname, "func_explosive") )
		{

			//G_Printf("Denied because ( Q_stricmp( trav->target_ent->classname, \"func_explosive\" ) - classname is %s. )\n", trav->target_ent->classname);
			continue;
		}

		if ( trav->spawnflags & OBJECTIVE_DESTROYED )
		{

			//G_Printf("Denied because ( trav->spawnflags & OBJECTIVE_DESTROYED )\n");
			continue;
		}

		// invulnerable
		if ( ent->spawnflags & CONSTRUCTIBLE_INVULNERABLE || (trav && trav->spawnflags & 8) )
		{

			//G_Printf("Denied because ( ent->spawnflags & CONSTRUCTIBLE_INVULNERABLE || (trav && trav->spawnflags & 8) )\n");
			continue;
		}

		if
		(
			ent->s.eType == ET_CONSTRUCTIBLE &&
			!(G_ConstructionIsPartlyBuilt( ent) || G_ConstructionIsFullyBuilt( ent))
		)
		{

			//G_Printf("Denied because ( NOT BUILT )\n");
			continue;
		}

		//G_Printf("Scriptname is %s. Targetname is %s. Origin is %f %f %f. OK!!!!\n", ent->scriptName, ent->targetname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
		if ( trav->spawnflags & AXIS_OBJECTIVE )
		{
			alliedDestroyables[num_allied_destroyables] = trav->s.number;
			num_allied_destroyables++;
			BotMP_Calculate_In_Range_Nodes( ent );
		}

		if ( trav->spawnflags & ALLIED_OBJECTIVE )
		{
			axisDestroyables[num_axis_destroyables] = trav->s.number;
			num_axis_destroyables++;
			BotMP_Calculate_In_Range_Nodes( ent );
		}
	}

	/*num_bot_objectives--;
	num_bot_goals--;
	num_axis_buildables--;
	num_allied_buildables--;
	num_allied_destroyables--;
	num_axis_destroyables--;*/

	//BotMP_Generate_Snipe_Positions();
#ifdef _DEBUG
	if (bot_debug.integer)
	{
		G_Printf( "^2*** ^3BOT DEBUG^5: There are currently ^7%i^5 bot objectives and ^7%i^5 bot goals.\n",
				  num_bot_objectives, num_bot_goals );
		G_Printf( "^2*** ^3BOT DEBUG^5: There are ^7%i^5 axis buildables and ^7%i^5 allied buildables.\n",
				  num_axis_buildables, num_allied_buildables );
		G_Printf( "^2*** ^3BOT DEBUG^5: There are ^7%i^5 axis destroyables and ^7%i^5 allied destroyables.\n",
				  num_axis_destroyables, num_allied_destroyables );
		G_Printf( "^2*** ^3BOT DEBUG^5: There are ^7%i^5 axis defence points and ^7%i^5 allied defence points.\n",
				  numAxisDefencePoints, numAlliedDefencePoints );
		G_Printf( "^2*** ^3BOT DEBUG^5: There are ^7%i^5 sniper points.\n", numSniperGoals );
	}
#endif //_DEBUG

	if ( num_bot_objectives < 0 )
	{
		num_bot_objectives = 0;
	}

	if ( num_bot_goals < 0 )
	{
		num_bot_goals = 0;
	}

	if ( num_axis_buildables < 0 )
	{
		num_axis_buildables = 0;
	}

	if ( num_allied_buildables < 0 )
	{
		num_allied_buildables = 0;
	}

	if ( num_allied_destroyables < 0 )
	{
		num_allied_destroyables = 0;
	}

	if ( num_axis_destroyables < 0 )
	{
		num_axis_destroyables = 0;
	}

	if ( numSniperGoals < 0 )
	{
		numSniperGoals = 0;
	}

	// Set up our attacking team number in the level stats...
	Set_Attacking_Team();
}

#else //__CUDA__

__device__ __host__ void BotMP_Update_Goal_Lists ( void )
{				// Create arrays containing up to date valid goal entity numbers...
	__shared__ int loop = 0;

	// Initialize the max values...
	__shared__ num_bot_objectives = 0;
	__shared__ num_bot_goals = 0;
	__shared__ num_axis_buildables = 0;
	__shared__ num_allied_buildables = 0;
	__shared__ num_axis_destroyables = 0;
	__shared__ num_allied_destroyables = 0;
	__shared__ numAlliedDefencePoints = 0;
	__shared__ numAxisDefencePoints = 0;
	__shared__ numDefencePoints = 0;
	__shared__ numSniperGoals = 0;

	// Initialize the lists so there is no left-overs...
	BotMP_Init_Goal_Lists();

	for ( loop = 0; loop < MAX_GENTITIES; loop++ )
	{
		__shared__ int			nameNum = 0;
		__shared__ gentity_t	*ent = &g_entities[loop];
//		__shared__ qboolean	snipe_point_converted = qfalse;

		if ( !ent )
		{
			continue;
		}

		if ( !ent->classname )
		{
			continue;
		}
		{		// Constructibles get their own listing as well... For faster engineer thinking...
			__shared__ gentity_t	*constructible = G_IsConstructible( TEAM_AXIS, ent );
			
			if ( constructible != NULL )
			{	// Is still buildable (valid)...
				axisBuildables[num_axis_buildables] = ent->s.number;		//ent->target_ent->s.number;//ent->s.number;
				num_axis_buildables++;
				BotMP_Calculate_In_Range_Nodes( ent );

				//G_Printf("^4*** ^3%s^5: Axis Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
			}

			constructible = G_IsConstructible( TEAM_ALLIES, ent );

			if ( constructible != NULL )
			{																// Is still buildable (valid)...
				alliedBuildables[num_allied_buildables] = ent->s.number;	//ent->target_ent->s.number;//ent->s.number;
				num_allied_buildables++;
				BotMP_Calculate_In_Range_Nodes( ent );

				//G_Printf("^4*** ^3%s^5: Allied Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
			}
		}

		//for (nameNum = 0;botObjectiveList[nameNum] != "";nameNum++)
		for ( nameNum = 0; Q_stricmp( botObjectiveList[nameNum], ""); nameNum++ )
		{
#ifndef __VC6__
			if ( ent->classname && !Q_stricmp( ent->classname, va( "%s", botObjectiveList[nameNum])) )
#else //__VC6__
				if ( ent->classname == botObjectiveList[nameNum] )
#endif //__VC6__
				{
					BotObjectives[num_bot_objectives] = loop;
					num_bot_objectives++;
					BotMP_Calculate_In_Range_Nodes( ent );
					break;
				}
		}

		//for (nameNum = 0;botGoalsList[nameNum] != "";nameNum++)
		for ( nameNum = 0; Q_stricmp( botGoalsList[nameNum], ""); nameNum++ )
		{
#ifndef __VC6__
			if ( ent->classname && !Q_stricmp( ent->classname, va( "%s", botGoalsList[nameNum])) )
#else //__VC6__
				if ( ent->classname == botGoalsList[nameNum] )
#endif //__VC6__
				{
					BotGoals[num_bot_goals] = loop;
					num_bot_goals++;
					BotMP_Calculate_In_Range_Nodes( ent );
					break;
				}
		}

		//for (nameNum = 0;botGoalsList[nameNum] != "";nameNum++)
		for ( nameNum = 0; Q_stricmp( botSniperGoalsList[nameNum], ""); nameNum++ )
		{
#ifndef __VC6__
			if ( ent->classname && !Q_stricmp( ent->classname, va( "%s", botSniperGoalsList[nameNum])) )
#else //__VC6__
				if ( ent->classname == botSniperGoalsList[nameNum] )
#endif //__VC6__
				{
					SniperGoals[numSniperGoals] = loop;
					numSniperGoals++;
					BotMP_Calculate_In_Range_Nodes( ent );
					break;
				}
		}

		for ( nameNum = 0; Q_stricmp( botDefenceGoalsList[nameNum], ""); nameNum++ )
		{
#ifndef __VC6__
			if ( ent->classname && !Q_stricmp( ent->classname, va( "%s", botDefenceGoalsList[nameNum])) )
#else //__VC6__
				if ( ent->classname == botDefenceGoalsList[nameNum] )
#endif //__VC6__
				{
					DefencePoints[numDefencePoints] = loop;
					numDefencePoints++;
					BotMP_Calculate_In_Range_Nodes( ent );
					if ( ent->s.teamNum == TEAM_AXIS )
					{
						numAxisDefencePoints++;
					}
					else if ( ent->s.teamNum == TEAM_ALLIES )
					{
						numAlliedDefencePoints++;
					}
					break;
				}
		}
	}

	// Create our destroyables list...
	for ( loop = 0; loop < MAX_GENTITIES; loop++ )
	{
		__shared__ gentity_t	*ent = &g_entities[loop];
		__shared__ gentity_t	*trav = NULL;

		if ( !ent )
		{
			continue;
		}

		if ( !ent->parent )
		{
			continue;
		}

		trav = ent->parent;

		//G_Printf("Scriptname is %s. Targetname is %s.\n", ent->scriptName, ent->targetname);
		if ( !(trav->spawnflags & ALLIED_OBJECTIVE) && !(trav->spawnflags & AXIS_OBJECTIVE) )
		{
			continue;
		}

		if ( !(ent->r.contents & CONTENTS_TRIGGER) )
		{

			//G_Printf("Denied because ( !( ent->r.contents & CONTENTS_TRIGGER ) )\n");
			continue;
		}

		if
		(
			trav->s.eType != ET_OID_TRIGGER &&
			trav->s.eType != ET_EXPLOSIVE_INDICATOR &&
			trav->s.eType != ET_TANK_INDICATOR
		)
		{

			//G_Printf("Denied because ( NO TRIGGER )\n");
			continue;
		}

		if ( ent->spawnflags & 128 )
		{

			//G_Printf("Denied because ( ent->spawnflags & 128 )\n");
			continue;
		}

		if ( !trav->target_ent )
		{
			continue;
		}

		if ( trav->target_ent && Q_stricmp( trav->target_ent->classname, "func_explosive") )
		{

			//G_Printf("Denied because ( Q_stricmp( trav->target_ent->classname, \"func_explosive\" ) - classname is %s. )\n", trav->target_ent->classname);
			continue;
		}

		if ( trav->spawnflags & OBJECTIVE_DESTROYED )
		{

			//G_Printf("Denied because ( trav->spawnflags & OBJECTIVE_DESTROYED )\n");
			continue;
		}

		// invulnerable
		if ( ent->spawnflags & CONSTRUCTIBLE_INVULNERABLE || (trav && trav->spawnflags & 8) )
		{

			//G_Printf("Denied because ( ent->spawnflags & CONSTRUCTIBLE_INVULNERABLE || (trav && trav->spawnflags & 8) )\n");
			continue;
		}

		if
		(
			ent->s.eType == ET_CONSTRUCTIBLE &&
			!(G_ConstructionIsPartlyBuilt( ent) || G_ConstructionIsFullyBuilt( ent))
		)
		{

			//G_Printf("Denied because ( NOT BUILT )\n");
			continue;
		}

		//G_Printf("Scriptname is %s. Targetname is %s. Origin is %f %f %f. OK!!!!\n", ent->scriptName, ent->targetname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
		if ( trav->spawnflags & AXIS_OBJECTIVE )
		{
			alliedDestroyables[num_allied_destroyables] = trav->s.number;
			num_allied_destroyables++;
			BotMP_Calculate_In_Range_Nodes( ent );
		}

		if ( trav->spawnflags & ALLIED_OBJECTIVE )
		{
			axisDestroyables[num_axis_destroyables] = trav->s.number;
			num_axis_destroyables++;
			BotMP_Calculate_In_Range_Nodes( ent );
		}
	}

	/*num_bot_objectives--;
	num_bot_goals--;
	num_axis_buildables--;
	num_allied_buildables--;
	num_allied_destroyables--;
	num_axis_destroyables--;*/

	//BotMP_Generate_Snipe_Positions();
#ifdef _DEBUG
	if (bot_debug.integer)
	{
		G_Printf( "^2*** ^3BOT DEBUG^5: There are currently ^7%i^5 bot objectives and ^7%i^5 bot goals.\n",
				  num_bot_objectives, num_bot_goals );
		G_Printf( "^2*** ^3BOT DEBUG^5: There are ^7%i^5 axis buildables and ^7%i^5 allied buildables.\n",
				  num_axis_buildables, num_allied_buildables );
		G_Printf( "^2*** ^3BOT DEBUG^5: There are ^7%i^5 axis destroyables and ^7%i^5 allied destroyables.\n",
				  num_axis_destroyables, num_allied_destroyables );
		G_Printf( "^2*** ^3BOT DEBUG^5: There are ^7%i^5 axis defence points and ^7%i^5 allied defence points.\n",
				  numAxisDefencePoints, numAlliedDefencePoints );
		G_Printf( "^2*** ^3BOT DEBUG^5: There are ^7%i^5 sniper points.\n", numSniperGoals );
	}
#endif //_DEBUG

	if ( num_bot_objectives < 0 )
	{
		num_bot_objectives = 0;
	}

	if ( num_bot_goals < 0 )
	{
		num_bot_goals = 0;
	}

	if ( num_axis_buildables < 0 )
	{
		num_axis_buildables = 0;
	}

	if ( num_allied_buildables < 0 )
	{
		num_allied_buildables = 0;
	}

	if ( num_allied_destroyables < 0 )
	{
		num_allied_destroyables = 0;
	}

	if ( num_axis_destroyables < 0 )
	{
		num_axis_destroyables = 0;
	}

	if ( numSniperGoals < 0 )
	{
		numSniperGoals = 0;
	}

	// Set up our attacking team number in the level stats...
	Set_Attacking_Team();
}

#endif //__CUDA__

extern int	CreatePathAStar ( gentity_t *bot, int from, int to, /*short*/ int *pathlist );


/* */
qboolean
BotMP_Can_Reach_Waypoint ( gentity_t *bot, int goalNode )
{	// Check if we can reach a waypoint...
	/*short*/ int	pathlist[MAX_NODES];
	int			count = -1;

	count = CreatePathAStar( bot, bot->current_node, goalNode, pathlist );
	
	if ( count >= 0 )
	{
		return ( qtrue );
	}

	return ( qfalse );
}

/* */
qboolean
BotMP_Can_Reach_Entity ( gentity_t *bot, int entityNum )
{	// Check if we can reach an entity...
/*	gentity_t	*goal = &g_entities[entityNum];
	int			goalNode = -1;

	if (SSE_CPU)
	{
		sse_memset( bot->pathlist, NODE_INVALID, ((sizeof(int)) * MAX_PATHLIST_NODES)/8 );
	}
	else
	{
		memset( bot->pathlist, NODE_INVALID, ((sizeof(int)) * MAX_PATHLIST_NODES) );
	}

	bot->pathsize = 0;

	if ( !goal )
	{
		return ( qfalse );
	}

	if ( !bot->current_node )
	{
		bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
	}

	if (goal->num_in_range_nodes > 0)
	{
		//goalNode = goal->in_range_nodes[Q_TrueRand(0, goal->num_in_range_nodes)];
		int i;
		qboolean found_reachable = qfalse;

		for (i = 0; i < goal->num_in_range_nodes; i++)
		{
			qboolean reachable = qfalse;

			goalNode = goal->in_range_nodes[i];
			reachable = BotMP_Can_Reach_Waypoint( bot, goalNode);

			if (!found_reachable && reachable)
				found_reachable = qtrue;
		}

		if (found_reachable)
		{
			return found_reachable;
		}
		else
		{
			//goalNode = AIMOD_NAVIGATION_FindClosestReachableNode( goal, NODE_DENSITY, NODEFIND_ALL, bot );
			goalNode = goal->in_range_nodes[Q_TrueRand(0, goal->num_in_range_nodes)];
		}
	}
	else
	{
		goalNode = AIMOD_NAVIGATION_FindClosestReachableNode( goal, NODE_DENSITY, NODEFIND_ALL, bot );
	}

	if ( !goalNode )
	{
		return ( qfalse );
	}

	return ( BotMP_Can_Reach_Waypoint( bot, goalNode) );*/

	return qtrue;
}

/* */
int
BotMP_Pick_Random_Goal ( void )
{	// Totally random goals... For cinematic planes mainly... Returns entity number...
	int choice = Q_TrueRand( 0, num_bot_goals - 1 );
	return ( BotGoals[choice] );
}

#ifdef __ETE__


/* */
int
Find_POP_Capture_Point ( gentity_t *bot, vec3_t current_org, int teamNum )
{
	int		i;
	float	best_dist = 65000.0f;
	int		best_flag = -1;
	int		second_best_flag = -1;
	int		total_num_flags = GetNumberOfPOPFlags();
	for ( i = 0; i <= total_num_flags; i++ )
	{
		if ( flag_list[i].flagentity )
		{
			if ( flag_list[i].flagentity->s.modelindex != teamNum	 /*|| (flag_list[i].flagentity->s.modelindex == teamNum && flag_list[i].flagentity->s.time2 != 0)*/ )
			{
				if
				(
					VectorDistance( flag_list[i].flagentity->s.origin, current_org) < best_dist &&
					BotMP_Can_Reach_Entity( bot, flag_list[i].flagentity->s.number)
				)
				{
					best_dist = VectorDistance( flag_list[i].flagentity->s.origin, current_org );
					second_best_flag = best_flag;
					best_flag = flag_list[i].flagentity->s.number;
				}
			}
		}
	}

	if ( second_best_flag >= 0 )
	{
		if ( Q_TrueRand( 0, 3) == 0 )
		{
			return ( second_best_flag );
		}
		else
		{
			return ( best_flag );
		}
	}
	else
	{
		return ( best_flag );
	}
}
#endif //__ETE__

gentity_t	*attackerSpawn = NULL;
vec3_t		goodDirection;
int			next_AttackerUpdate = 5000;
qboolean	direction_initialized = qfalse;

/* */
//
// Generic objective goal finding code...
//

int
BotMP_Find_Objective_Goal
(
	gentity_t	*bot,
	int			ignoreEnt,
	int			ignoreEnt2,
	vec3_t		current_org,
	int			teamNum
)
{
#ifdef __ETE__
	if ( g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN )
	{
		int best_ent = Find_POP_Capture_Point( bot, current_org, teamNum );
		if ( best_ent >= 0 )
		{	// We have found a goal flag... Use it first!
			return ( best_ent );
		}
	}
#endif //__ETE__

	if ( teamNum == TEAM_AXIS && numAxisObjectives > 0 )
	{
		int		choice = 0;
		int		best_choice = -1;
		int		second_best = -1;
		int		third_best = -1;
		float	best_dist = 99999.9f;
		float	best_dir_test = 99999.9f;
		for ( choice = 0; choice <= numAxisObjectives; choice++ )
		{	// Find one of the closest ones...
			//if (Is_Engineer_Goal( BotObjectives[choice] ))
			{
				if
				(
					VectorDistance( current_org, g_entities[axisObjectives[choice]].r.currentOrigin) < best_dist &&
					VectorDistance( current_org, g_entities[axisObjectives[choice]].r.currentOrigin) > 512
				)
				{
					if
					(
						teamNum != level.attackingTeam &&
						VectorDistance( g_entities[axisObjectives[choice]].r.currentOrigin, goodDirection) > best_dir_test
					)
					{
						continue;
					}

					if ( axisObjectives[choice] == ignoreEnt || axisObjectives[choice] == ignoreEnt2 )
					{
						continue;
					}

					if ( !BotMP_Can_Reach_Entity( bot, axisObjectives[choice]) )
					{
						continue;
					}

					third_best = second_best;
					second_best = best_choice;
					best_choice = choice;
					best_dir_test = VectorDistance( g_entities[axisObjectives[choice]].r.currentOrigin, goodDirection );
					best_dist = VectorDistance( current_org, g_entities[axisObjectives[choice]].r.currentOrigin );
				}
			}
		}

		if ( best_choice != -1 && second_best != -1 && third_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 3 );
			if ( pick == 0 )
			{
				choice = third_best;
			}
			else if ( pick == 1 )
			{
				choice = second_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else if ( best_choice != -1 && second_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 2 );
			if ( pick == 0 )
			{
				choice = second_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else if ( best_choice != -1 && third_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 2 );
			if ( pick == 0 )
			{
				choice = third_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else
		{
			choice = best_choice;
		}

		if ( choice != -1 )
		{
			choice = axisObjectives[choice];
			return ( choice );
		}
	}
	else if ( teamNum == TEAM_ALLIES && numAlliedObjectives > 0 )
	{
		int		choice = 0;
		int		best_choice = -1;
		int		second_best = -1;
		int		third_best = -1;
		float	best_dist = 99999.9f;
		float	best_dir_test = 99999.9f;
		for ( choice = 0; choice <= numAlliedObjectives; choice++ )
		{		// Find one of the closest ones...
			//if (Is_Engineer_Goal( BotObjectives[choice] ))
			{
				if
				(
					VectorDistance( current_org, g_entities[alliedObjectives[choice]].r.currentOrigin) < best_dist &&
					VectorDistance( current_org, g_entities[alliedObjectives[choice]].r.currentOrigin) > 512
				)
				{
					if
					(
						teamNum != level.attackingTeam &&
						VectorDistance( g_entities[alliedObjectives[choice]].r.currentOrigin, goodDirection) > best_dir_test
					)
					{
						continue;
					}

					if ( alliedObjectives[choice] == ignoreEnt || alliedObjectives[choice] == ignoreEnt2 )
					{
						continue;
					}

					if ( !BotMP_Can_Reach_Entity( bot, alliedObjectives[choice]) )
					{
						continue;
					}

					third_best = second_best;
					second_best = best_choice;
					best_choice = choice;
					best_dir_test = VectorDistance( g_entities[alliedObjectives[choice]].r.currentOrigin, goodDirection );
					best_dist = VectorDistance( current_org, g_entities[alliedObjectives[choice]].r.currentOrigin );
				}
			}
		}

		if ( best_choice != -1 && second_best != -1 && third_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 3 );
			if ( pick == 0 )
			{
				choice = third_best;
			}
			else if ( pick == 1 )
			{
				choice = second_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else if ( best_choice != -1 && second_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 2 );
			if ( pick == 0 )
			{
				choice = second_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else if ( best_choice != -1 && third_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 2 );
			if ( pick == 0 )
			{
				choice = third_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else
		{
			choice = best_choice;
		}

		if ( choice != -1 )
		{
			choice = alliedObjectives[choice];
			return ( choice );
		}
	}

	if ( num_bot_objectives > 0 )
	{			// Find objectives as primary goal...
		int		choice = 0;
		int		best_choice = -1;
		int		second_best = -1;
		int		third_best = -1;
		float	best_dist = 99999.9f;
		float	best_dir_test = 99999.9f;
		for ( choice = 0; choice < num_bot_objectives; choice++ )
		{		// Find one of the closest ones...
			//if (Is_Engineer_Goal( BotObjectives[choice] ))
			{
				if
				(
					VectorDistance( current_org, g_entities[BotObjectives[choice]].r.currentOrigin) < best_dist &&
					VectorDistance( current_org, g_entities[BotObjectives[choice]].r.currentOrigin) > 512 &&
					BotGoalValidForTeam( BotObjectives[choice], teamNum)
				)
				{
					if
					(
						teamNum != level.attackingTeam &&
						VectorDistance( g_entities[BotObjectives[choice]].r.currentOrigin, goodDirection) > best_dir_test
					)
					{
						continue;
					}

					if ( BotObjectives[choice] == ignoreEnt || BotObjectives[choice] == ignoreEnt2 )
					{
						continue;
					}

					if ( !BotMP_Can_Reach_Entity( bot, BotObjectives[choice]) )
					{
						continue;
					}

					third_best = second_best;
					second_best = best_choice;
					best_choice = choice;
					best_dir_test = VectorDistance( g_entities[BotObjectives[choice]].r.currentOrigin, goodDirection );
					best_dist = VectorDistance( current_org, g_entities[BotObjectives[choice]].r.currentOrigin );
				}
			}
		}

		if ( best_choice != -1 && second_best != -1 && third_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 3 );
			if ( pick == 0 )
			{
				choice = third_best;
			}
			else if ( pick == 1 )
			{
				choice = second_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else if ( best_choice != -1 && second_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 2 );
			if ( pick == 0 )
			{
				choice = second_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else if ( best_choice != -1 && third_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 2 );
			if ( pick == 0 )
			{
				choice = third_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else
		{
			choice = best_choice;
		}

		if ( choice != -1 )
		{
			choice = BotObjectives[choice];
			return ( choice );
		}
	}

	if ( num_bot_goals > 0 )
	{			// Standard goals otherwise...
		int		choice = 0;
		int		best_choice = -1;
		int		second_best = -1;
		int		third_best = -1;
		float	best_dist = 99999.9f;
		float	best_dir_test = 99999.9f;
		for ( choice = 0; choice < num_bot_goals; choice++ )
		{		// Find one of the closest ones...
			//if (Is_Engineer_Goal( BotGoals[choice] ))
			{
				if
				(
					VectorDistance( current_org, g_entities[BotGoals[choice]].r.currentOrigin) < best_dist &&
					VectorDistance( current_org, g_entities[BotGoals[choice]].r.currentOrigin) > 512 &&
					BotGoalValidForTeam( BotObjectives[choice], teamNum)
				)
				{
					if
					(
						teamNum != level.attackingTeam &&
						VectorDistance( g_entities[BotGoals[choice]].r.currentOrigin, goodDirection) > best_dir_test
					)
					{
						continue;
					}

					if ( BotGoals[choice] == ignoreEnt || BotGoals[choice] == ignoreEnt2 )
					{
						continue;
					}

					if ( !BotMP_Can_Reach_Entity( bot, BotGoals[choice]) )
					{
						continue;
					}

					third_best = second_best;
					second_best = best_choice;
					best_choice = choice;
					best_dir_test = VectorDistance( g_entities[BotGoals[choice]].r.currentOrigin, goodDirection );
					best_dist = VectorDistance( current_org, g_entities[BotGoals[choice]].r.currentOrigin );
				}
			}
		}

		if ( best_choice != -1 && second_best != -1 && third_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 3 );
			if ( pick == 0 )
			{
				choice = third_best;
			}
			else if ( pick == 1 )
			{
				choice = second_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else if ( best_choice != -1 && second_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 2 );
			if ( pick == 0 )
			{
				choice = second_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else if ( best_choice != -1 && third_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 2 );
			if ( pick == 0 )
			{
				choice = third_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else
		{
			choice = best_choice;
		}

		if ( choice != -1 )
		{
			choice = BotGoals[choice];
			return ( choice );
		}
	}

	return ( -1 );
}

/* */
void
UpdateAttackerDirection ( void )
{
	vec3_t	spawn_origin, spawn_angles;
	if ( !direction_initialized )
	{														// At the start of the match, we must initialize this...
		VectorSet( goodDirection, 0, 0, 0 );
		direction_initialized = qtrue;
	}

	if ( next_AttackerUpdate > level.time )
	{
		return;
	}

	next_AttackerUpdate = level.time + 10000;
	attackerSpawn = SelectCTFSpawnPoint( level.attackingTeam, TEAM_ACTIVE, spawn_origin, spawn_angles, 0 );
	VectorCopy( attackerSpawn->s.origin, goodDirection );	// Keep goodDirection position for finding definding team's goals...
}

/* */
int
Find_Defence_Point ( gentity_t *bot )
{
	int		i;
	float	best_dist = 65000.0f;
	int		best_point = -1;
	int		second_best_point = -1;
	UpdateAttackerDirection();
	for ( i = 0; i < numDefencePoints; i++ )
	{
		gentity_t	*ent = &g_entities[DefencePoints[i]];
		float		enemy_distance, weight;
		float		distance;
		if ( !ent )
		{
			continue;
		}

		if ( bot->client && ent->s.teamNum != bot->client->sess.sessionTeam )
		{
			continue;
		}

#ifdef __NPC__
		if ( bot->NPC_client && ent->s.teamNum != bot->s.teamNum )
		{
			continue;
		}
#endif //__NPC__

		enemy_distance = VectorDistance( ent->s.origin, goodDirection );
		distance = VectorDistance( ent->s.origin, bot->r.currentOrigin );
		weight = enemy_distance + distance;
		if ( weight <= best_dist && BotMP_Can_Reach_Entity( bot, ent->s.number) )
		{	// Find the closest one to 1. Enemy and 2. The bot itself...
			best_dist = weight;
			second_best_point = best_point;
			best_point = ent->s.number;
		}
	}

	if ( second_best_point >= 0 )
	{
		if ( Q_TrueRand( 0, 3) == 0 )
		{
			return ( second_best_point );
		}
		else
		{
			return ( best_point );
		}
	}
	else
	{
		return ( best_point );
	}
}

extern int	BotMP_Soldier_Find_POP_Goal_EntityNum
			(
				gentity_t	*bot,
				int			ignoreEnt,
				int			ignoreEnt2,
				vec3_t		current_org,
				int			teamNum
			);


/* */
int
Find_Goal_Teammate ( gentity_t *bot )
{	// Find a teammate to use as our goal...
	int			i;
	gentity_t	*best_ent = NULL;
	gentity_t	*second_best_ent = NULL;
	float		best_dist = 9999.9f;
	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		gentity_t	*ent = &g_entities[i];
		float		dist;
		if ( !ent )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}

		if ( ent->health <= 0 )
		{
			continue;
		}

		if ( ent == bot )
		{
			continue;
		}

		if ( !(ent->r.svFlags & SVF_BOT) )
		{
			continue;
		}

		if ( !OnSameTeam( ent, bot) )
		{
			continue;
		}

		if ( ent->longTermGoal <= 0 )
		{
			continue;
		}

		if ( ent->current_node <= 0 )
		{
			continue;
		}

		if ( !BotMP_Can_Reach_Entity( bot, ent->s.number) )
		{
			continue;
		}

		dist = VectorDistance( ent->r.currentOrigin, bot->r.currentOrigin );
		if ( dist < best_dist )
		{
			second_best_ent = best_ent;
			best_ent = ent;
			best_dist = dist;
		}
	}

	if ( second_best_ent && best_ent )
	{
		if ( Q_TrueRand( 0, 3) == 1 )
		{
			bot->current_target_entity = second_best_ent->s.number;
			return ( second_best_ent->current_node );
		}
		else
		{
			bot->current_target_entity = best_ent->s.number;
			return ( best_ent->current_node );
		}
	}
	else
	{
		if ( best_ent )
		{
			bot->current_target_entity = best_ent->s.number;
			return ( best_ent->current_node );
		}
		else
		{
			bot->current_target_entity = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, -1, -1, bot->r.currentOrigin,
																				bot->client->sess.sessionTeam );
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( g_entities[bot->current_target_entity].r.currentOrigin,
																			 NODE_DENSITY, NODEFIND_ALL );
			return ( bot->longTermGoal );
		}
	}

	return ( -1 );
}

int BotMP_Soldier_Find_POP_Goal_EntityNum
	(
		gentity_t	*bot,
		int			ignoreEnt,
		int			ignoreEnt2,
		vec3_t		current_org,
		int			teamNum
	);


/* */
int
Find_Goal_Enemy ( gentity_t *bot )
{	// Find an enemy to use as our goal...
	int			i;
	gentity_t	*best_ent = NULL;
	gentity_t	*second_best_ent = NULL;
	float		best_dist = 9999.9f;

#ifdef __NPC__
	for ( i = 0; i < MAX_GENTITIES; i++ )
#else //!__NPC__
	for ( i = 0; i < MAX_CLIENTS; i++ )
#endif //__NPC__
	{
		gentity_t	*ent = &g_entities[i];
		float		dist;
		if ( !ent )
		{
			continue;
		}

#ifdef __NPC__
		if ( !ent->client && !ent->NPC_client )
#else //!__NPC__
		if ( !ent->client )
#endif //__NPC__
		{
			continue;
		}

		if ( ent == bot )
		{
			continue;
		}

		if ( ent->health <= 0 )
		{
			continue;
		}

		if ( OnSameTeam( ent, bot) )
		{
			continue;
		}

#ifdef __NPC__
		if ( ent->client && ent->client->ps.pm_flags & PMF_LIMBO )
#else //!__NPC__
		if ( ent->client->ps.pm_flags & PMF_LIMBO )
#endif //__NPC__
		{
			continue;
		}

		if ( ent->goalentity == bot )
		{
			continue;
		}

		if ( ent->current_target_entity == bot->s.number )
		{
			continue;
		}

		if ( !BotMP_Can_Reach_Entity( bot, ent->s.number) )
		{
			continue;
		}

		dist = VectorDistance( ent->r.currentOrigin, bot->r.currentOrigin );
		if ( dist < best_dist )
		{
			second_best_ent = best_ent;
			best_ent = ent;
			best_dist = dist;
		}
	}

	if ( second_best_ent && best_ent )
	{
		if ( Q_TrueRand( 0, 3) == 1 )
		{
			return ( second_best_ent->s.number );
		}
		else
		{
			return ( best_ent->s.number );
		}
	}
	else
	{
		if ( best_ent )
		{
			return ( best_ent->s.number );
		}
		else
		{
			return ( -1 );
		}

		//return BotMP_Soldier_Find_POP_Goal_EntityNum ( bot, -1, -1, bot->r.currentOrigin, bot->client->sess.sessionTeam );
	}

	return -1;
}


/* */
int
BotMP_Soldier_Find_POP_Goal_EntityNum ( gentity_t *bot, int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum )
{			// Return standard goals only for soldiers...
	//	gentity_t *goal = NULL;
	//	int loop = 0;
	int best_ent;
	int choice = Q_TrueRand(0,2);

	if ( next_goal_update < level.time )
	{			// Update new goal lists.. Unique1
		BotMP_Update_Goal_Lists();
		next_goal_update = level.time + 10000;
	}

#ifdef __ETE__
	if ( choice == 1 && (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) )
	{// 1 in 2 times try capture point goal first!
		int best_ent2 = Find_POP_Capture_Point( bot, current_org, teamNum );
		if ( best_ent2 >= 0 )
		{	// We have found a goal flag... Use it first!
			return ( best_ent2 );
		}
	}
#endif //__ETE__

	if ( choice == 2 && num_bot_goals > 0)
	{
		int choice2 = BotGoals[Q_TrueRand(0, num_bot_goals-1)];
	
		if (choice2 != ignoreEnt && choice2 != ignoreEnt2)
			return choice2;

		return choice2;
	}

	best_ent = Find_Defence_Point( bot );
	if ( best_ent >= 0 )
	{		// We have found a defence point goal... Use it!
		return ( best_ent );
	}

	best_ent = Find_Goal_Enemy( bot );
	if ( best_ent >= 0 )
	{		// We have found a goal enemy... Use it first!
		return ( best_ent );
	}

	{
		int best_ent2 = BotMP_Find_Objective_Goal(bot, ignoreEnt, ignoreEnt2, current_org, teamNum);
		if ( best_ent2 >= 0 )
		{		// We have found a goal enemy... Use it first!
			return ( best_ent2 );
		}
	}


	/*	if (num_bot_goals > 0)
	{
		int choice = BotGoals[Q_TrueRand(0, num_bot_goals-1)];
	
		if (choice != ignoreEnt && choice != ignoreEnt2)
			return choice;

		return choice;
	}*/

	// And if all else fails, head to an enemy...
	{
		int best_ent2 = Find_Goal_Enemy( bot );
		if ( best_ent2 >= 0 )
		{		// We have found a goal enemy... Use it first!
			return ( best_ent2 );
		}
	}

	return ( -1 );
}


/* */
int
BotMP_Sniper_Find_Goal_EntityNum ( gentity_t *bot, int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum )
{
	int choice = Q_TrueRand(0,2);

	if ( next_goal_update < level.time )
	{			// Update new goal lists.. Unique1
		BotMP_Update_Goal_Lists();
		next_goal_update = level.time + 10000;
	}

	if ( numSniperGoals > 0 )
	{// Try to find a real sniper point first!
		int		choice2 = Q_irand(0, numSniperGoals);

		choice2 = SniperGoals[choice2];
		return ( choice2 );
	}

	if ( numSniperGoals > 0 )
	{			// Try to find a real sniper point first!
		int		choice2 = 0;
		int		best_choice = -1;
		int		second_best = -1;
		int		third_best = -1;
		float	best_dist = 99999.9f;
		float	best_dir_test = 99999.9f;
		for ( choice2 = 0; choice2 <= numSniperGoals; choice2++ )
		{		// Find one of the closest ones...
			if
			(
				VectorDistance( current_org, g_entities[SniperGoals[choice2]].r.currentOrigin) < best_dist &&
				VectorDistance( current_org, g_entities[SniperGoals[choice2]].r.currentOrigin) > 4096/*512*/
			)
			{
				if
				(
					teamNum != level.attackingTeam &&
					VectorDistance( g_entities[SniperGoals[choice2]].r.currentOrigin, goodDirection) > best_dir_test
				)
				{
					continue;
				}

				if ( SniperGoals[choice2] == ignoreEnt || SniperGoals[choice2] == ignoreEnt2 )
				{
					continue;
				}

				if ( !BotMP_Can_Reach_Entity( bot, SniperGoals[choice2]) )
				{
					continue;
				}

				third_best = second_best;
				second_best = best_choice;
				best_choice = choice2;
				best_dir_test = VectorDistance( g_entities[SniperGoals[choice2]].r.currentOrigin, goodDirection );
				best_dist = VectorDistance( current_org, g_entities[SniperGoals[choice2]].r.currentOrigin );
			}
		}

		if ( best_choice != -1 && second_best != -1 && third_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 3 );
			if ( pick == 0 )
			{
				choice = third_best;
			}
			else if ( pick == 1 )
			{
				choice = second_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else if ( best_choice != -1 && second_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 2 );
			if ( pick == 0 )
			{
				choice = second_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else if ( best_choice != -1 && third_best != -1 )
		{		// Pick one...
			int pick = Q_TrueRand( 0, 2 );
			if ( pick == 0 )
			{
				choice = third_best;
			}
			else
			{	// Twice the chance...
				choice = best_choice;
			}
		}
		else
		{
			choice = best_choice;
		}

		if ( choice != -1 )
		{
			choice = SniperGoals[choice];
			return ( choice );
		}
	}

#ifdef __ETE__
	if ( choice == 1 && (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) )
	{// 1 in 2 times try capture point goal first!
		int best_ent = Find_POP_Capture_Point( bot, current_org, teamNum );
		if ( best_ent >= 0 )
		{	// We have found a goal flag... Use it first!
			return ( best_ent );
		}
	}
#endif //__ETE__

	if ( choice == 2 && num_bot_goals > 0)
	{
		int choice2 = BotGoals[Q_TrueRand(0, num_bot_goals-1)];
	
		if (choice2 != ignoreEnt && choice2 != ignoreEnt2)
			return choice2;

		return choice2;
	}

#ifdef __ETE__
	if ( g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN )
	{
		int best_ent = Find_POP_Capture_Point( bot, current_org, teamNum );
		if ( best_ent >= 0 )
		{		// We have found a goal flag... Use it first!
			return ( best_ent );
		}
	}
#endif //__ETE__
/*	{
		int best_ent = Find_Defence_Point( bot );
		if ( best_ent >= 0 )
		{		// We have found a defence point goal... Use it!
			return ( best_ent );
		}
	}
*/
	return ( BotMP_Soldier_Find_POP_Goal_EntityNum( bot, ignoreEnt, ignoreEnt2, current_org, teamNum) );

	return ( -1 );
}

/* */
int
BotMP_FieldOps_Find_POP_Goal_EntityNum
(
	gentity_t	*bot,
	int			ignoreEnt,
	int			ignoreEnt2,
	vec3_t		current_org,
	int			teamNum
)
{			// Return fieldops goals as priority here...
	//	gentity_t *goal = NULL;
	//	int loop = 0;
	int best_ent;
	int choice = Q_TrueRand(0,2);
#ifdef __ETE__
	if ( choice == 1 && (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) )
	{// 1 in 2 times try capture point goal first!
		int best_ent2 = Find_POP_Capture_Point( bot, current_org, teamNum );
		if ( best_ent2 >= 0 )
		{	// We have found a goal flag... Use it first!
			return ( best_ent2 );
		}
	}
#endif //__ETE__

	if ( choice == 2 && num_bot_goals > 0)
	{
		int choice2 = BotGoals[Q_TrueRand(0, num_bot_goals-1)];
	
		if (choice2 != ignoreEnt && choice2 != ignoreEnt2)
			return choice2;

		return choice2;
	}

	if ( next_goal_update < level.time )
	{		// Update new goal lists.. Unique1
		BotMP_Update_Goal_Lists();
		next_goal_update = level.time + 10000;
	}

	best_ent = Find_Defence_Point( bot );
	if ( best_ent >= 0 )
	{		// We have found a defence point goal... Use it!
		return ( best_ent );
	}

/*  {
		int best_ent = BotMP_Find_Objective_Goal(bot, ignoreEnt, ignoreEnt2, current_org, teamNum);
		if ( best_ent >= 0 )
		{		// We have found a goal enemy... Use it first!
			return ( best_ent );
		}
	}
*/

	// And if all else fails, head to an enemy...
	{
		int best_ent2 = Find_Goal_Enemy( bot );
		if ( best_ent2 >= 0 )
		{		// We have found a goal enemy... Use it first!
			return ( best_ent2 );
		}
	}

	return ( -1 );
}

#ifdef __BOT_AAS__
int BotMP_Medic_Scan_For_Revives (int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum, bot_state_t *bs)
#else //!__BOT_AAS__
int BotMP_Medic_Scan_For_Revives (int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum, gentity_t *bot)
#endif //__BOT_AAS__
{				// Return medic goals ONLY here...
	//	gentity_t *goal = NULL;
	//	int loop = 0;
	int		client = 0;
	int		best_heal = -1;
	float	best_dist = 99999.9f;
	int		best_type = BOTGOAL_GIVEHEALTH;
	if ( next_goal_update < level.time )
	{			// Update new goal lists.. Unique1
		BotMP_Update_Goal_Lists();
		next_goal_update = level.time + 10000;
	}

	//G_Printf("%s is looking for a medic goal!\n", g_entities[bs->client].client->pers.netname);
	// Find medic revive/heal targets...
	for ( client = 0; client < MAX_CLIENTS; client++ )
	{
		gentity_t	*heal_target = &g_entities[client];
		if ( !heal_target )
		{
			continue;
		}
		if ( !heal_target->client )
		{
			continue;
		}

		//		if (!heal_target->client->pers.connected != CON_CONNECTED)
		//			continue;
#ifdef __BOT_AAS__
		if ( g_entities[bs->client].client->sess.sessionTeam != heal_target->client->sess.sessionTeam )
#else //!__BOT_AAS__
			if ( bot->client->sess.sessionTeam != heal_target->client->sess.sessionTeam )
#endif //__BOT_AAS__
				continue;
		if ( heal_target->client->sess.sessionTeam == TEAM_SPECTATOR || heal_target->client->ps.pm_flags & PMF_LIMBO )
		{
			continue;
		}
		if ( heal_target->health >= heal_target->client->ps.stats[STAT_MAX_HEALTH] )
		{
			continue;
		}
#ifndef __BOT_AAS__
		if ( !BotMP_Can_Reach_Entity(bot, heal_target->s.number) )
		{
			continue;
		}
#endif //__BOT_AAS__
		if ( best_type != BOTGOAL_REVIVE && heal_target->health <= 0 )
		{		// Our current best isn't a revive.. If this is, override distance checks...
			best_type = BOTGOAL_REVIVE;
			best_heal = client;
			best_dist = VectorDistance( current_org, heal_target->r.currentOrigin );

			//G_Printf("New bestheal (%i)!\n", client);
		}
		else if ( best_type == BOTGOAL_REVIVE && heal_target->health <= 0 )
		{		// Our current best is a revive.. If this is, do distance checks...
			if ( VectorDistance(current_org, heal_target->r.currentOrigin) > best_dist )
			{

				//G_Printf("New bestheal (%i) is too far!\n", client);
				continue;
			}
			best_type = BOTGOAL_REVIVE;
			best_heal = client;
			best_dist = VectorDistance( current_org, heal_target->r.currentOrigin );

			//G_Printf("New bestheal (%i)!\n", client);
		}
		else if
			(
				best_type == BOTGOAL_REVIVE &&
				heal_target->health > 0 &&
				VectorDistance(current_org, heal_target->r.currentOrigin) > 512
			)
		{		// Our current best is a revive.. If this is not, and it's distance is not really close, ignore it...
			continue;
		}
		else
		{
			if ( VectorDistance(current_org, heal_target->r.currentOrigin) > best_dist )
			{

				//G_Printf("New bestheal (%i) is too far!\n", client);
				continue;
			}
			if ( heal_target->health > 0 )
			{
				best_type = BOTGOAL_GIVEHEALTH;
			}
			else
			{
				best_type = BOTGOAL_REVIVE;
			}
			best_heal = client;
			best_dist = VectorDistance( current_org, heal_target->r.currentOrigin );

			//G_Printf("New bestheal (%i)!\n", client);
		}
	}
	if ( best_heal >= 0 )
	{

		//G_Printf("%s found a revive or heal goal (%s)!\n", g_entities[bs->client].client->pers.netname, g_entities[best_heal].client->pers.netname);
		if ( g_entities[best_heal].health > 0 )
		{
			best_type = BOTGOAL_GIVEHEALTH;
		}
		else
		{
			best_type = BOTGOAL_REVIVE;
		}
#ifdef __BOT_AAS__
		bs->goalType = best_type;
#endif //__BOT_AAS__
		return ( best_heal );
	}

#ifdef __ETE__
	if ( g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN )
	{
		int best_ent = Find_POP_Capture_Point( bot, current_org, teamNum );
		if ( best_ent >= 0 )
		{	// We have found a goal flag... Use it first!
			return ( best_ent );
		}
	}
#endif //__ETE__

	return ( -1 );
}

#ifdef __BOT_AAS__
int BotMP_Medic_Find_POP_Goal_EntityNum
	(int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum, bot_state_t *bs)
#else //!__BOT_AAS__
int BotMP_Medic_Find_POP_Goal_EntityNum (int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum, gentity_t *bot)
#endif //__BOT_AAS__
{				// Return medic goals as priority here...
	//	gentity_t *goal = NULL;
	//	int loop = 0;
	//	int client = 0;
	//	int best_heal = -1;
	//	float	best_dist = 99999.9f;
	//	int	best_type = BOTGOAL_GIVEHEALTH;
	int			heal_goal = -1;
#ifdef __BOT_AAS__
	gentity_t	*bot = &g_entities[bs->client];
#endif //__BOT_AAS__
	if ( next_goal_update < level.time )
	{			// Update new goal lists.. Unique1
		BotMP_Update_Goal_Lists();
		next_goal_update = level.time + 10000;
	}
#ifdef __BOT_AAS__
	heal_goal = BotMP_Medic_Scan_For_Revives( ignoreEnt, ignoreEnt2, current_org, teamNum, bs );
#else //!__BOT_AAS__
	heal_goal = BotMP_Medic_Scan_For_Revives( ignoreEnt, ignoreEnt2, current_org, teamNum, bot );
#endif //__BOT_AAS__
	if ( heal_goal >= 0 )
	{
		return ( heal_goal );
	}
#ifdef __ETE__
	if ( g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN )
	{
		int best_ent = Find_POP_Capture_Point( bot, current_org, teamNum );
		if ( best_ent >= 0 )
		{		// We have found a goal flag... Use it first!
			return ( best_ent );
		}
	}
#endif //__ETE__
	{
		int best_ent = Find_Defence_Point( bot );
		if ( best_ent >= 0 )
		{		// We have found a defence point goal... Use it!
			return ( best_ent );
		}
	}

/*  {
		int best_ent = BotMP_Find_Objective_Goal(bot, ignoreEnt, ignoreEnt2, current_org, teamNum);
		if ( best_ent >= 0 )
		{		// We have found a goal enemy... Use it first!
			return ( best_ent );
		}
	}
*/

	// And if all else fails, head to an enemy...
	{
		int best_ent = Find_Goal_Enemy( bot );
		if ( best_ent >= 0 )
		{		// We have found a goal enemy... Use it first!
			return ( best_ent );
		}
	}
	return ( -1 );
}
extern void BotBuildStaticEntityCache ( void );


/* */
int
BotMP_Engineer_Find_POP_Goal_EntityNum
(
	gentity_t	*bot,
	int			ignoreEnt,
	int			ignoreEnt2,
	vec3_t		current_org,
	int			teamNum
)
{			// Return engineering goals as priority here...
	//	gentity_t *goal = NULL;
	int loop = 0;
	int choice = Q_TrueRand(0,4);

	if ( next_goal_update < level.time )
	{		// Update new goal lists.. Unique1
		BotMP_Update_Goal_Lists();
		next_goal_update = level.time + 10000;
	}

	// Initialize the constractible goal marker...
	bot->bot_goal_constructible = qfalse;

	// Initialize the destroyable goal marker...
	bot->bot_goal_destroyable = qfalse;

#ifdef __ETE__
	if ( choice == 1 && (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) )
	{// 1 in 2 times try capture point goal first!
		int best_ent = Find_POP_Capture_Point( bot, current_org, teamNum );
		if ( best_ent >= 0 )
		{	// We have found a goal flag... Use it first!
			return ( best_ent );
		}
	}
#endif //__ETE__

	if ( choice == 2 && num_bot_goals > 0)
	{
		int choice2 = BotGoals[Q_TrueRand(0, num_bot_goals-1)];
	
		if (choice2 != ignoreEnt && choice2 != ignoreEnt2)
			return choice2;

		return choice2;
	}

	// Find a team destroyable as a primary goal!
	if ( teamNum == TEAM_ALLIES && num_allied_destroyables > 0 && Q_TrueRand( 0, 3) < 2 )
	{
		int		best_destroyable = -1;
		int		second_best_destroyable = -1;
		float	best_destroyable_dist = 9999.9f;
		for ( loop = 0; loop < num_allied_destroyables; loop++ )
		{
			float		dist;
			gentity_t	*destroyable = &g_entities[alliedDestroyables[loop]];
			dist = VectorDistance( current_org, destroyable->r.currentOrigin );
			if ( !BotMP_Can_Reach_Entity( bot, alliedDestroyables[loop]) )
			{
				continue;
			}

			if ( dist < best_destroyable_dist )
			{
				if ( best_destroyable >= 0 )
				{
					second_best_destroyable = best_destroyable;
				}

				best_destroyable_dist = dist;
				best_destroyable = alliedDestroyables[loop];
			}
		}

		if ( teamNum == level.attackingTeam && best_destroyable >= 0 )
		{	// If we are the attacking team, destroyables take highest priority ALWAYS!
			if ( second_best_destroyable >= 0 )
			{
				bot->bot_goal_destroyable = qtrue;
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					gentity_t	*ent = &g_entities[second_best_destroyable];

					//G_Printf("^4*** ^3%s^5: Allied Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					bot->bot_goal_entity = ent;
					return ( second_best_destroyable );
				}
				else
				{
					gentity_t	*ent = &g_entities[second_best_destroyable];

					//G_Printf("^4*** ^3%s^5: Allied Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					bot->bot_goal_entity = ent;
					return ( best_destroyable );
				}
			}
			else
			{
				gentity_t	*ent = &g_entities[best_destroyable];

				//G_Printf("^4*** ^3%s^5: Allied Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
				VectorCopy( ent->s.origin, bot->bot_goal_position );
				bot->bot_goal_destroyable = qtrue;
				bot->bot_goal_entity = ent;
				return ( best_destroyable );
			}
		}
		else
		{	// Must be the defending team.. Only use this as a goal if we are close!
			int best = -1;
			if ( second_best_destroyable >= 0 )
			{
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					best = second_best_destroyable;
				}
				else
				{
					best = best_destroyable;
				}
			}
			else
			{
				best = best_destroyable;
			}

			if ( best >= 0 )
			{
				if ( VectorDistance( current_org, g_entities[best].r.currentOrigin) < 1024 )
				{
					gentity_t	*ent = &g_entities[best];

					//G_Printf("^4*** ^3%s^5: Allied Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					bot->bot_goal_destroyable = qtrue;
					bot->bot_goal_entity = ent;
					return ( best );
				}
			}
		}
	}

	if ( teamNum == TEAM_AXIS && num_axis_destroyables > 0 && Q_TrueRand( 0, 3) < 2 )
	{
		int		best_destroyable = -1;
		int		second_best_destroyable = -1;
		float	best_destroyable_dist = 9999.9f;
		for ( loop = 0; loop < num_axis_destroyables; loop++ )
		{
			float		dist;
			gentity_t	*destroyable = &g_entities[axisDestroyables[loop]];
			dist = VectorDistance( current_org, destroyable->r.currentOrigin );
			if ( !BotMP_Can_Reach_Entity( bot, axisDestroyables[loop]) )
			{
				continue;
			}

			if ( dist < best_destroyable_dist )
			{
				if ( best_destroyable >= 0 )
				{
					second_best_destroyable = best_destroyable;
				}

				best_destroyable_dist = dist;
				best_destroyable = axisDestroyables[loop];
			}
		}

		if ( teamNum == level.attackingTeam && best_destroyable >= 0 )
		{	// If we are the attacking team, destroyables take highest priority ALWAYS!
			if ( second_best_destroyable >= 0 )
			{
				bot->bot_goal_destroyable = qtrue;
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					gentity_t	*ent = &g_entities[second_best_destroyable];

					//G_Printf("^4*** ^3%s^5: Axis Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					bot->bot_goal_entity = ent;
					return ( second_best_destroyable );
				}
				else
				{
					gentity_t	*ent = &g_entities[best_destroyable];

					//G_Printf("^4*** ^3%s^5: Axis Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					bot->bot_goal_entity = ent;
					return ( best_destroyable );
				}
			}
			else
			{
				gentity_t	*ent = &g_entities[best_destroyable];

				//G_Printf("^4*** ^3%s^5: Axis Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
				VectorCopy( ent->s.origin, bot->bot_goal_position );
				bot->bot_goal_destroyable = qtrue;
				bot->bot_goal_entity = ent;
				return ( best_destroyable );
			}
		}
		else
		{	// Must be the defending team.. Only use this as a goal if we are close!
			int best = -1;
			if ( second_best_destroyable >= 0 )
			{
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					best = second_best_destroyable;
				}
				else
				{
					best = best_destroyable;
				}
			}
			else
			{
				best = best_destroyable;
			}

			if ( best >= 0 )
			{
				if ( VectorDistance( current_org, g_entities[best].r.currentOrigin) < 1024 )
				{
					gentity_t	*ent = &g_entities[best];

					//G_Printf("^4*** ^3%s^5: Axis Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					bot->bot_goal_destroyable = qtrue;
					bot->bot_goal_entity = ent;
					return ( best );
				}
			}
		}
	}

	// End of destroyables...
	// First, go do required building...
	if ( teamNum == TEAM_ALLIES && num_allied_buildables > 0 )
	{
		int		best_destroyable = -1;
		int		second_best_destroyable = -1;
		float	best_destroyable_dist = 9999.9f;
		for ( loop = 0; loop < num_allied_buildables; loop++ )
		{
			float		dist;
			gentity_t	*destroyable = &g_entities[alliedBuildables[loop]];
			dist = VectorDistance( current_org, destroyable->s.origin );
			if ( !BotMP_Can_Reach_Entity( bot, alliedBuildables[loop]) )
			{
				continue;
			}

			if ( dist < best_destroyable_dist )
			{
				if ( best_destroyable >= 0 )
				{
					second_best_destroyable = best_destroyable;
				}

				best_destroyable_dist = dist;
				best_destroyable = alliedBuildables[loop];
			}
		}

		if ( best_destroyable >= 0 )
		{	// If we are the attacking team, destroyables take highest priority ALWAYS!
			if ( second_best_destroyable >= 0 )
			{
				bot->bot_goal_constructible = qtrue;
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					gentity_t	*ent = &g_entities[best_destroyable];

					//G_Printf("^4*** ^3%s^5: Allied Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( second_best_destroyable );
				}
				else
				{
					gentity_t	*ent = &g_entities[best_destroyable];

					//G_Printf("^4*** ^3%s^5: Allied Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( best_destroyable );
				}
			}
			else
			{
				gentity_t	*ent = &g_entities[best_destroyable];

				//G_Printf("^4*** ^3%s^5: Allied Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
				VectorCopy( ent->s.origin, bot->bot_goal_position );
				bot->bot_goal_constructible = qtrue;
				return ( best_destroyable );
			}
		}
		else
		{	// Must be the defending team.. Only use this as a goal if we are close!
			int best = -1;
			if ( second_best_destroyable >= 0 )
			{
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					best = second_best_destroyable;
				}
				else
				{
					best = best_destroyable;
				}
			}
			else
			{
				best = best_destroyable;
			}

			if ( best >= 0 )
			{
				if ( VectorDistance( current_org, g_entities[best].s.origin) < 1024 )
				{
					gentity_t	*ent = &g_entities[best];

					//G_Printf("^4*** ^3%s^5: Allied Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					bot->bot_goal_constructible = qtrue;
					return ( best );
				}
			}
		}
	}

	if ( teamNum == TEAM_AXIS && num_axis_buildables > 0 )
	{
		int		best_destroyable = -1;
		int		second_best_destroyable = -1;
		float	best_destroyable_dist = 9999.9f;
		for ( loop = 0; loop < num_axis_buildables; loop++ )
		{
			float		dist;
			gentity_t	*destroyable = &g_entities[axisBuildables[loop]];
			dist = VectorDistance( current_org, destroyable->s.origin );
			if ( !BotMP_Can_Reach_Entity( bot, axisBuildables[loop]) )
			{
				continue;
			}

			if ( dist < best_destroyable_dist )
			{
				if ( best_destroyable >= 0 )
				{
					second_best_destroyable = best_destroyable;
				}

				best_destroyable_dist = dist;
				best_destroyable = axisBuildables[loop];
			}
		}

		if ( best_destroyable >= 0 )
		{	// If we are the attacking team, destroyables take highest priority ALWAYS!
			if ( second_best_destroyable >= 0 )
			{
				bot->bot_goal_constructible = qtrue;
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					gentity_t	*ent = &g_entities[second_best_destroyable];

					//G_Printf("^4*** ^3%s^5: Axis Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( second_best_destroyable );
				}
				else
				{
					gentity_t	*ent = &g_entities[best_destroyable];

					//G_Printf("^4*** ^3%s^5: Axis Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( best_destroyable );
				}
			}
			else
			{
				gentity_t	*ent = &g_entities[best_destroyable];

				//G_Printf("^4*** ^3%s^5: Axis Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
				VectorCopy( ent->s.origin, bot->bot_goal_position );
				bot->bot_goal_constructible = qtrue;
				return ( best_destroyable );
			}
		}
		else
		{	// Must be the defending team.. Only use this as a goal if we are close!
			int best = -1;
			if ( second_best_destroyable >= 0 )
			{
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					best = second_best_destroyable;
				}
				else
				{
					best = best_destroyable;
				}
			}
			else
			{
				best = best_destroyable;
			}

			if ( best >= 0 )
			{
				if ( VectorDistance( current_org, g_entities[best].s.origin) < 1024 )
				{
					gentity_t	*ent = &g_entities[best];

					//G_Printf("^4*** ^3%s^5: Axis Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					bot->bot_goal_constructible = qtrue;
					return ( best );
				}
			}
		}
	}

#ifdef __ETE__
	if ( g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN )
	{
		int best_ent = Find_POP_Capture_Point( bot, current_org, teamNum );
		if ( best_ent >= 0 )
		{	// We have found a goal flag... Use it first!
			return ( best_ent );
		}
	}
#endif //__ETE__
	{
		int best_ent = Find_Defence_Point( bot );
		if ( best_ent >= 0 )
		{	// We have found a defence point goal... Use it!
			return ( best_ent );
		}
	}

	// And if all else fails, head to an enemy...
	{
		int best_ent = Find_Goal_Enemy( bot );
		if ( best_ent >= 0 )
		{	// We have found a goal enemy... Use it first!
			return ( best_ent );
		}
	}

	return BotMP_FieldOps_Find_POP_Goal_EntityNum(bot, ignoreEnt, ignoreEnt2, bot->r.currentOrigin, teamNum);

	//G_Printf("ENG: Failed to find a bot goal!\n");
	return ( -1 );
}


/* */
int
BotMP_Num_Defenders_Near ( vec3_t pos, gentity_t *bot )
{
	int i;
	int number = 0;
	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		gentity_t	*ent = &g_entities[i];
		if ( !ent )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}

		if ( ent->health <= 0 )
		{
			continue;
		}

		if ( !ent->bot_defender )
		{
			continue;
		}

		if ( VectorDistance( pos, ent->r.currentOrigin) > 256 )
		{
			continue;
		}

		if ( !OnSameTeam( bot, ent) )
		{
			continue;
		}

		number++;
	}

	// Return the number found...
	return ( number );
}


/* */
int
BotMP_Defender_Find_POP_Goal_EntityNum
(
	gentity_t	*bot,
	int			ignoreEnt,
	int			ignoreEnt2,
	vec3_t		current_org,
	int			teamNum
)
{			// Return engineering goals as priority here...
	if ( next_goal_update < level.time )
	{		// Update new goal lists.. Unique1
		BotMP_Update_Goal_Lists();
		next_goal_update = level.time + 10000;
	}

#ifdef __ETE__
	if ( g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN )
	{
		int best_ent = -1;
		if ( teamNum == TEAM_ALLIES )
		{
			best_ent = Find_POP_Capture_Point( bot, current_org, TEAM_AXIS );
		}
		else
		{	//if (teamNum == TEAM_AXIS)
			best_ent = Find_POP_Capture_Point( bot, current_org, TEAM_ALLIES );
		}

		if ( best_ent >= 0 )
		{	// We have found a goal flag... Use it first!
			return ( best_ent );
		}
	}
#endif //__ETE__
	{
		int best_ent = Find_Defence_Point( bot );
		if ( best_ent >= 0 )
		{	// We have found a defence point goal... Use it!
			return ( best_ent );
		}
	}

	// Initialize the constractible goal marker...
	bot->bot_goal_constructible = qfalse;

	// Initialize the destroyable goal marker...
	bot->bot_goal_destroyable = qfalse;

/*
	// Find a team destroyable as a primary goal!
	if ( teamNum == TEAM_AXIS && num_allied_destroyables > 0 && Q_TrueRand( 0, 3) < 2 )
	{
		int		best_destroyable = -1;
		int		second_best_destroyable = -1;
		float	best_destroyable_dist = 9999.9f;
		for ( loop = 0; loop < num_allied_destroyables; loop++ )
		{
			float		dist;
			gentity_t	*destroyable = &g_entities[alliedDestroyables[loop]];
			dist = VectorDistance( current_org, destroyable->r.currentOrigin );
			if ( !BotMP_Can_Reach_Entity( bot, alliedDestroyables[loop]) )
			{
				continue;
			}

			if ( dist < best_destroyable_dist && BotMP_Num_Defenders_Near( destroyable->r.currentOrigin, bot) < 4 )
			{
				second_best_destroyable = best_destroyable;
				best_destroyable_dist = dist;
				best_destroyable = alliedDestroyables[loop];
			}
		}

		if ( best_destroyable >= 0 )
		{	// If we are the attacking team, destroyables take highest priority ALWAYS!
			if ( second_best_destroyable >= 0 )
			{

				//bot->bot_goal_destroyable = qtrue;
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					gentity_t	*ent = &g_entities[second_best_destroyable];

					//G_Printf("^4*** ^3%s^5: Allied Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( second_best_destroyable );
				}
				else
				{
					gentity_t	*ent = &g_entities[second_best_destroyable];

					//G_Printf("^4*** ^3%s^5: Allied Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( best_destroyable );
				}
			}
			else
			{
				gentity_t	*ent = &g_entities[best_destroyable];

				//G_Printf("^4*** ^3%s^5: Allied Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
				VectorCopy( ent->s.origin, bot->bot_goal_position );
				bot->bot_goal_destroyable = qtrue;
				return ( best_destroyable );
			}
		}
	}

	if ( teamNum == TEAM_ALLIES && num_axis_destroyables > 0 && Q_TrueRand( 0, 3) < 2 )
	{
		int		best_destroyable = -1;
		int		second_best_destroyable = -1;
		float	best_destroyable_dist = 9999.9f;
		for ( loop = 0; loop < num_axis_destroyables; loop++ )
		{
			float		dist;
			gentity_t	*destroyable = &g_entities[axisDestroyables[loop]];
			dist = VectorDistance( current_org, destroyable->r.currentOrigin );
			if ( !BotMP_Can_Reach_Entity( bot, axisDestroyables[loop]) )
			{
				continue;
			}

			if ( dist < best_destroyable_dist && BotMP_Num_Defenders_Near( destroyable->r.currentOrigin, bot) < 4 )
			{
				second_best_destroyable = best_destroyable;
				best_destroyable_dist = dist;
				best_destroyable = axisDestroyables[loop];
			}
		}

		if ( best_destroyable >= 0 )
		{	// If we are the attacking team, destroyables take highest priority ALWAYS!
			if ( second_best_destroyable >= 0 )
			{

				//bot->bot_goal_destroyable = qtrue;
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					gentity_t	*ent = &g_entities[second_best_destroyable];

					//G_Printf("^4*** ^3%s^5: Axis Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( second_best_destroyable );
				}
				else
				{
					gentity_t	*ent = &g_entities[best_destroyable];

					//G_Printf("^4*** ^3%s^5: Axis Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( best_destroyable );
				}
			}
			else
			{
				gentity_t	*ent = &g_entities[best_destroyable];

				//G_Printf("^4*** ^3%s^5: Axis Destructable %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
				VectorCopy( ent->s.origin, bot->bot_goal_position );
				bot->bot_goal_destroyable = qtrue;
				return ( best_destroyable );
			}
		}
	}

	// End of destroyables...
	// Secondly, go do required building...
	if ( teamNum == TEAM_ALLIES && num_allied_buildables > 0 )
	{
		int		best_destroyable = -1;
		int		second_best_destroyable = -1;
		float	best_destroyable_dist = 9999.9f;
		for ( loop = 0; loop < num_allied_buildables; loop++ )
		{
			float		dist;
			gentity_t	*destroyable = &g_entities[alliedBuildables[loop]];
			dist = VectorDistance( current_org, destroyable->s.origin );
			if ( !BotMP_Can_Reach_Entity( bot, alliedBuildables[loop]) )
			{
				continue;
			}

			if ( dist < best_destroyable_dist )
			{
				if ( best_destroyable >= 0 )
				{
					second_best_destroyable = best_destroyable;
				}

				best_destroyable_dist = dist;
				best_destroyable = alliedBuildables[loop];
			}
		}

		if ( best_destroyable >= 0 )
		{	// If we are the attacking team, destroyables take highest priority ALWAYS!
			if ( second_best_destroyable >= 0 )
			{
				bot->bot_goal_constructible = qtrue;
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					gentity_t	*ent = &g_entities[best_destroyable];

					//G_Printf("^4*** ^3%s^5: Allied Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( second_best_destroyable );
				}
				else
				{
					gentity_t	*ent = &g_entities[best_destroyable];

					//G_Printf("^4*** ^3%s^5: Allied Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( best_destroyable );
				}
			}
			else
			{
				gentity_t	*ent = &g_entities[best_destroyable];

				//G_Printf("^4*** ^3%s^5: Allied Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
				VectorCopy( ent->s.origin, bot->bot_goal_position );
				bot->bot_goal_constructible = qtrue;
				return ( best_destroyable );
			}
		}
	}

	if ( teamNum == TEAM_AXIS && num_axis_buildables > 0 )
	{
		int		best_destroyable = -1;
		int		second_best_destroyable = -1;
		float	best_destroyable_dist = 9999.9f;
		for ( loop = 0; loop < num_axis_buildables; loop++ )
		{
			float		dist;
			gentity_t	*destroyable = &g_entities[axisBuildables[loop]];
			dist = VectorDistance( current_org, destroyable->s.origin );
			if ( !BotMP_Can_Reach_Entity( bot, axisBuildables[loop]) )
			{
				continue;
			}

			if ( dist < best_destroyable_dist )
			{
				if ( best_destroyable >= 0 )
				{
					second_best_destroyable = best_destroyable;
				}

				best_destroyable_dist = dist;
				best_destroyable = axisBuildables[loop];
			}
		}

		if ( best_destroyable >= 0 )
		{	// If we are the attacking team, destroyables take highest priority ALWAYS!
			if ( second_best_destroyable >= 0 )
			{
				bot->bot_goal_constructible = qtrue;
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					gentity_t	*ent = &g_entities[second_best_destroyable];

					//G_Printf("^4*** ^3%s^5: Axis Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( second_best_destroyable );
				}
				else
				{
					gentity_t	*ent = &g_entities[best_destroyable];

					//G_Printf("^4*** ^3%s^5: Axis Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
					VectorCopy( ent->s.origin, bot->bot_goal_position );
					return ( best_destroyable );
				}
			}
			else
			{
				gentity_t	*ent = &g_entities[best_destroyable];

				//G_Printf("^4*** ^3%s^5: Axis Constructible %s at position %f %f %f.\n", GAME_VERSION, ent->classname, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2]);
				VectorCopy( ent->s.origin, bot->bot_goal_position );
				bot->bot_goal_constructible = qtrue;
				return ( best_destroyable );
			}
		}
	}


	// And if all else fails, head to an enemy...
	{
		int best_ent = Find_Goal_Enemy( bot );
		if ( best_ent >= 0 )
		{	// We have found a goal enemy... Use it first!
			return ( best_ent );
		}
	}*/

	return ( -1 );
}


/* */
int
BotMP_Find_POP_Goal_EntityNum ( gentity_t *bot, int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum )
{
	int best_ent;
	if ( next_goal_update < level.time )
	{		// Update new goal lists.. Unique1
		BotMP_Update_Goal_Lists();
		next_goal_update = level.time + 10000;
	}
/*  {
		int best_ent = BotMP_Find_Objective_Goal(bot, ignoreEnt, ignoreEnt2, current_org, teamNum);
		if ( best_ent >= 0 )
		{		// We have found a goal enemy... Use it first!
			return ( best_ent );
		}
	}
*/

#ifdef __ETE__
	if ( g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN )
	{
		best_ent = Find_POP_Capture_Point( bot, current_org, teamNum );
		if ( best_ent >= 0 )
		{		// We have found a goal flag... Use it first!
			return ( best_ent );
		}
	}
#endif //__ETE__
/*	best_ent = Find_Defence_Point( bot );
	if ( best_ent >= 0 )
	{			// We have found a defence point goal... Use it!
		return ( best_ent );
	}*/

	if ( num_bot_goals > 0 )
	{
		int choice = BotGoals[Q_TrueRand( 0, num_bot_goals - 1 )];
		if ( choice != ignoreEnt && choice != ignoreEnt2 )
		{
			return ( choice );
		}

		return ( choice );
	}

	//G_Printf("Failed to find a bot goal!\n");
	return ( -1 );
}
#endif //#if defined(__BOT__) || defined(__BOT_AAS__)

#ifdef __BOT_AAS__
/*
===================
BotMP_MedicScan
===================
*/
qboolean
BotMP_MedicScan ( bot_state_t *bs )
{
	bot_goal_t	bestTarget, secondary;
	int			teamlist[64], numTeammates, t, gotTarget, defendTarget, targetTime;
	int			entNum = -1;
	if ( bs->areanum <= 0 )
	{
		BotDefaultNode( bs );
	}

	// Limit BotMP_FindGoal checks..
	if ( bs->medicScanTime > level.time )
	{
		return ( qtrue );
	}
	else
	{
		bs->medicScanTime = level.time + 1000;
	}

	BotClearGoal( &target );
	BotClearGoal( &secondary );
	numTeammates = BotNumTeamMates( bs, teamlist, 64 );
	gotTarget = qfalse;
	entNum = BotMP_Medic_Scan_For_Revives( entNum, bs->targetEntityNumber, g_entities[bs->client].r.currentOrigin,
										   g_entities[bs->client].client->sess.sessionTeam, bs );
	if ( entNum == -1 )
	{

		//G_Printf("No ents on this map to head for!\n");
		return ( qfalse );
	}

	if ( BotGoalForEntity( bs, entNum, &target, BGU_HIGH) )
	{
		target.flags = GFL_LEADER;
		t = trap_AAS_AreaTravelTimeToGoalArea( bs->areanum, bs->origin, target.areanum, bs->tfl );
		if ( !t || t > MAX_BOTLEADER_TRAVEL )
		{
			bs->leader = -1;
		}
		else
		{
			targetTime = t;
			gotTarget = qtrue;
			defendTarget = qtrue;
			bestTarget = target;
		}
	}
	else
	{
		bs->leader = -1;
	}

#ifdef _DEBUG
	if ( bs->goalType == BOTGOAL_REVIVE )
	{
		if (bot_debug.integer)
			G_Printf( "BOT DEBUG: (Medic) %s's new revive goal is at target.areanum %i (%s).\n",
				  g_entities[bs->client].client->pers.netname, target.areanum,
				  g_entities[target.entitynum].client->pers.netname );
	}
	else
	{
		if (bot_debug.integer)
			G_Printf( "BOT DEBUG: (Medic) %s's new heal goal is at target.areanum %i (%s).\n",
				  g_entities[bs->client].client->pers.netname, target.areanum,
				  g_entities[target.entitynum].client->pers.netname );
	}
#endif //_DEBUG
	if ( target.areanum > 0 )
	{
		bs->target_goal = target;
	}

	if ( bs->target_goal.entitynum )
	{

		// Set our target entity number...
		bs->targetEntityNumber = bs->target_goal.entitynum;
		return ( qtrue );
	}
	else
	{

		// Setup the ROAM goaltype for this NULL goal...
		bs->goalType = BOTGOAL_ROAM;
		return ( qfalse );
	}
}


/*
===================
BotMP_FindGoal
===================
*/
qboolean
BotMP_FindGoal ( bot_state_t *bs )
{
	bot_goal_t	bestTarget, secondary;
	int			teamlist[64], numTeammates, t, gotTarget, defendTarget, targetTime;
	int			entNum = -1;

	// Update direction for defence bots...
	UpdateAttackerDirection();
	if ( bs->areanum <= 0 )
	{
		BotDefaultNode( bs );
	}

	// Limit BotMP_FindGoal checks..
	if ( bs->next_targetgoal_set > level.time )
	{
		return ( qtrue );
	}
	else
	{
		bs->next_targetgoal_set = level.time + 10000;
	}

	if ( bs->last_findspecialgoals > level.time - 1600 )
	{
		return ( qfalse );
	}

	bs->last_findspecialgoals = level.time + rand() % 400;
	if ( bs->ignore_specialgoal_time > level.time )
	{
		return ( qfalse );
	}

	BotClearGoal( &target );
	BotClearGoal( &secondary );
	numTeammates = BotNumTeamMates( bs, teamlist, 64 );
	gotTarget = qfalse;
	while ( target.areanum <= 0 )
	{
		qboolean goodgoal = qfalse;

		if ( g_entities[bs->client].client->sess.playerType == PC_ENGINEER )
		{
			entNum = BotMP_Engineer_Find_POP_Goal_EntityNum( &g_entities[bs->client], entNum, bs->targetEntityNumber,
															 g_entities[bs->client].r.currentOrigin,
															 g_entities[bs->client].client->sess.sessionTeam );
		}
		else if
			(
				g_entities[bs->client].client->sess.playerType == PC_SOLDIER ||
				g_entities[bs->client].client->sess.playerType == PC_COVERTOPS
			)
		{
			entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( &g_entities[bs->client], entNum, bs->targetEntityNumber,
															g_entities[bs->client].r.currentOrigin,
															g_entities[bs->client].client->sess.sessionTeam );
		}
		else if ( g_entities[bs->client].client->sess.playerType == PC_FIELDOPS )
		{
			entNum = BotMP_FieldOps_Find_POP_Goal_EntityNum( &g_entities[bs->client], entNum, bs->targetEntityNumber,
															 g_entities[bs->client].r.currentOrigin,
															 g_entities[bs->client].client->sess.sessionTeam );
		}
		else if ( g_entities[bs->client].client->sess.playerType == PC_MEDIC )
		{
			entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bs->targetEntityNumber,
														  g_entities[bs->client].r.currentOrigin,
														  g_entities[bs->client].client->sess.sessionTeam, bs );
		}
		else
		{
			entNum = BotMP_Find_POP_Goal_EntityNum( &g_entities[bs->client], entNum, bs->targetEntityNumber,
													g_entities[bs->client].r.currentOrigin,
													g_entities[bs->client].client->sess.sessionTeam );
		}

		if ( entNum == -1 )
		{
			//G_Printf( "No ents on this map to head for!\n" );
			//break;
			entNum = BotMP_Find_POP_Goal_EntityNum( &g_entities[bs->client], entNum, bs->targetEntityNumber,
													g_entities[bs->client].r.currentOrigin,
													g_entities[bs->client].client->sess.sessionTeam );
		}

		goodgoal = BotGoalForEntity( bs, entNum, &target, BGU_HIGH);

		if (!goodgoal)
		{
			if (target.areanum <= 0)
			{
				qboolean bad = qtrue;

				while (bad)
				{
					gentity_t *test = NULL;

					entNum = Q_TrueRand(0, 1000);
					test = &g_entities[entNum];

					if (!test)
						continue;

					goodgoal = BotGoalForEntity( bs, entNum, &target, BGU_HIGH);
					
					if (!goodgoal || target.areanum <= 0)
						continue;

					bad = qfalse;
				}
			}
		}

		if ( goodgoal )
		{
//			if (target.areanum <= 0)
//				target.areanum = 1;

//			if ( entNum == -1 )
//			{
//				G_Printf( "No ents on this map to head for!\n" );
//				break;
//			}

			target.flags = GFL_LEADER;
			t = trap_AAS_AreaTravelTimeToGoalArea( bs->areanum, bs->origin, target.areanum, bs->tfl );
			if ( !t || t > MAX_BOTLEADER_TRAVEL || t < 5000 )
			{
				bs->leader = -1;
			}
			else
			{
				targetTime = t;
				gotTarget = qtrue;
				defendTarget = qtrue;
				bestTarget = target;
			}
		}
		else
		{
			bs->leader = -1;
		}
	}

	if
	(
		(bs->goalType == BOTGOAL_GIVEHEALTH || bs->goalType == BOTGOAL_REVIVE) &&
		(target.entitynum < 0 || target.entitynum >= MAX_CLIENTS)
	)
	{
		bs->goalType = GoalType( bs->target_goal.entitynum, g_entities[bs->client].client->sess.sessionTeam );
	}

	//bs->goalType = BOTGOAL_ROAM;
//#ifdef _DEBUG
	if ( bs->goalType == BOTGOAL_REVIVE )
	{
		if (bot_debug.integer)
			G_Printf( "BOT DEBUG: (Medic) %s's new revive goal is at target.areanum %i (%s).\n",
				  g_entities[bs->client].client->pers.netname, target.areanum,
				  g_entities[target.entitynum].client->pers.netname );
	}
	else if ( bs->goalType == BOTGOAL_GIVEHEALTH )
	{
		if (bot_debug.integer)
			G_Printf( "BOT DEBUG: (Medic) %s's new heal goal is at target.areanum %i (%s).\n",
				  g_entities[bs->client].client->pers.netname, target.areanum,
				  g_entities[target.entitynum].client->pers.netname );
	}
	else
	{
		if (bot_debug.integer)
			G_Printf( "BOT DEBUG: %s's new goal is at target.areanum %i (%s).\n",
				  g_entities[bs->client].client->pers.netname, target.areanum, g_entities[target.entitynum].classname );
	}
//#endif //_DEBUG
	if ( target.areanum > 0 )
	{
		bs->target_goal = target;
	}

	//else if (secondary.areanum > 0)
	//	bs->target_goal = secondary;
	if ( bs->target_goal.entitynum )
	{

		// Setup the goaltype for this goal...
		if ( g_entities[bs->target_goal.entitynum].s.eType != ET_PLAYER )
		{	// Player goals will be set by the BotMP_*_Find_POP_Goal_EntityNum procedures...
			bs->goalType = GoalType( bs->target_goal.entitynum, g_entities[bs->client].client->sess.sessionTeam );
		}

		// Set our target entity number...
		bs->targetEntityNumber = bs->target_goal.entitynum;
		return ( qtrue );
	}
	else
	{

		// Setup the ROAM goaltype for this NULL goal...
		bs->goalType = BOTGOAL_ROAM;
		return ( qfalse );
	}
}
#endif //__BOT__ || __BOT_AAS__
