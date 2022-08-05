#include "../game/q_global_defines.h"

#ifdef __NPC__
#include "g_local.h"

extern vmCvar_t bot_thinktime;

//extern int AIMOD_NAVIGATION_FindClosestReachableNodeTo(vec3_t origin, int r, int type, gentity_t *master);
extern int AIMOD_NAVIGATION_FindClosestReachableNodeTo(vec3_t origin, int r, int type);
extern void Bullet_Fire (gentity_t *ent, float spread, int damage, qboolean distance_falloff);
extern qboolean NPCVisible (gentity_t *self, gentity_t *other);
extern int SP_NodeVisible ( vec3_t org1, vec3_t org2, int ignore );
extern qboolean NPC_NodesLoaded ( void );
extern void NPC_Setup( gentity_t *NPC );
extern void NPC_AI_Move( gentity_t *NPC );
extern float VectorDistanceNoHeight ( vec3_t v1, vec3_t v2 );
extern float HeightDistance ( vec3_t v1, vec3_t v2 );
extern void G_UcmdMoveForDir ( gentity_t *self, usercmd_t *cmd, vec3_t dir );
void AIMOD_NPC_SP_MOVEMENT_Move ( gentity_t *NPC, usercmd_t *ucmd ); // below
//extern float	NPC_TravelTime ( gentity_t *bot );
extern void			AIMOD_MOVEMENT_IncreaseLinkageCost ( gentity_t *bot );
extern int AIMOD_MOVEMENT_ReachableBy ( gentity_t *bot, vec3_t goal );
extern void AIMOD_MOVEMENT_CheckAvoidance ( gentity_t *bot, usercmd_t *ucmd );
extern void AIMOD_MOVEMENT_Setup_Strafe ( gentity_t *bot, usercmd_t *ucmd );
extern qboolean		AIMOD_MOVEMENT_WaypointTouched ( gentity_t *bot, int touch_node, qboolean onLadder );
void NPC_Set_New_Path ( gentity_t *bot, usercmd_t *ucmd );
void NPC_Queue_New_Path ( gentity_t *bot, usercmd_t *ucmd );

qboolean IsCoopGametype ( void )
{
	switch (g_gametype.integer)
	{
	case GT_SINGLE_PLAYER:
	case GT_COOP:
	case GT_NEWCOOP:
		return qtrue;
		break;
	default:
		break;
	}

	return qfalse;
}

// Some simple anims ready for use... Unique1... define __ANIM_INFO__ to get more from player moves.
#define NPC_LEGS_ANIM_WALK 37
#define NPC_LEGS_ANIM_WALK_BACK 556
#define NPC_LEGS_ANIM_STAND 575
#define NPC_LEGS_ANIM_CROUCH 42
#define NPC_LEGS_ANIM_CROUCH_WALK 63
#define NPC_LEGS_ANIM_JUMP 612
#define NPC_LEGS_ANIM_RUN 549

#define NPC_TORSO_ANIM_WALK 37
#define NPC_TORSO_ANIM_WALK_BACK 44
#define NPC_TORSO_ANIM_STAND 575
#define NPC_TORSO_ANIM_CROUCH 575
#define NPC_TORSO_ANIM_CROUCH_WALK 575
#define NPC_TORSO_ANIM_JUMP 100
#define NPC_TORSO_ANIM_RUN 37


#define LUGER_SPREAD				BG_GetWeaponSpread(WP_LUGER)
#define LUGER_DAMAGE				BG_GetWeaponDamage(WP_LUGER) // JPW

#define SILENCER_DAMAGE				BG_GetWeaponDamage(WP_SILENCED_LUGER)
#define SILENCER_SPREAD				BG_GetWeaponSpread(WP_SILENCED_LUGER)

#define AKIMBO_LUGER_DAMAGE			BG_GetWeaponDamage(WP_AKIMBO_LUGER)
#define AKIMBO_LUGER_SPREAD			BG_GetWeaponSpread(WP_AKIMBO_LUGER)

#define AKIMBO_SILENCEDLUGER_DAMAGE	BG_GetWeaponDamage(WP_AKIMBO_SILENCEDLUGER)
#define AKIMBO_SILENCEDLUGER_SPREAD	BG_GetWeaponSpread(WP_AKIMBO_SILENCEDLUGER)

#define COLT_SPREAD					BG_GetWeaponSpread(WP_COLT)
#define	COLT_DAMAGE					BG_GetWeaponDamage(WP_COLT) // JPW

#define SILENCED_COLT_DAMAGE		BG_GetWeaponDamage(WP_SILENCED_COLT)
#define SILENCED_COLT_SPREAD		BG_GetWeaponSpread(WP_SILENCED_COLT)

#define AKIMBO_COLT_DAMAGE			BG_GetWeaponDamage(WP_AKIMBO_COLT)
#define AKIMBO_COLT_SPREAD			BG_GetWeaponSpread(WP_AKIMBO_COLT)

#define AKIMBO_SILENCEDCOLT_DAMAGE	BG_GetWeaponDamage(WP_AKIMBO_SILENCEDCOLT)
#define AKIMBO_SILENCEDCOLT_SPREAD	BG_GetWeaponSpread(WP_AKIMBO_SILENCEDCOLT)

#define MP40_SPREAD					BG_GetWeaponSpread(WP_MP40)
#define	MP40_DAMAGE					BG_GetWeaponDamage(WP_MP40) // JPW
#define THOMPSON_SPREAD				BG_GetWeaponSpread(WP_THOMPSON)
#define	THOMPSON_DAMAGE				BG_GetWeaponDamage(WP_THOMPSON) // JPW
#define STEN_SPREAD					BG_GetWeaponSpread(WP_STEN)
#define	STEN_DAMAGE					BG_GetWeaponDamage(WP_STEN) // JPW

#define GARAND_SPREAD				BG_GetWeaponSpread(WP_GARAND)
#define	GARAND_DAMAGE				BG_GetWeaponDamage(WP_GARAND) // JPW

#define KAR98_SPREAD				BG_GetWeaponSpread(WP_KAR98)
#define	KAR98_DAMAGE				BG_GetWeaponDamage(WP_KAR98)

#define CARBINE_SPREAD				BG_GetWeaponSpread(WP_CARBINE)
#define	CARBINE_DAMAGE				BG_GetWeaponDamage(WP_CARBINE)

#define	KAR98_GREN_DAMAGE			BG_GetWeaponDamage(WP_GREN_KAR98)

#define MOBILE_MG42_SPREAD			BG_GetWeaponSpread(WP_MOBILE_MG42)
#define	MOBILE_MG42_DAMAGE			BG_GetWeaponDamage(WP_MOBILE_MG42)

#define FG42_SPREAD					BG_GetWeaponSpread(WP_FG42)
#define	FG42_DAMAGE					BG_GetWeaponDamage(WP_FG42) // JPW

#define FG42SCOPE_SPREAD			BG_GetWeaponSpread(WP_FG42SCOPE)
#define	FG42SCOPE_DAMAGE			BG_GetWeaponDamage(WP_FG42SCOPE) // JPW
#define K43_SPREAD					BG_GetWeaponSpread(WP_K43)
#define	K43_DAMAGE					BG_GetWeaponDamage(WP_K43)

#define GARANDSCOPE_SPREAD			BG_GetWeaponSpread(WP_GARAND_SCOPE)
#define GARANDSCOPE_DAMAGE			BG_GetWeaponDamage(WP_GARAND_SCOPE)

#define K43SCOPE_SPREAD				BG_GetWeaponSpread(WP_K43_SCOPE)
#define K43SCOPE_DAMAGE				BG_GetWeaponDamage(WP_K43_SCOPE)

#ifdef __RW__
#define MOBILE_M1919A4_SPREAD		BG_GetWeaponSpread(WP_M1919A4)
#define	MOBILE_M1919A4_DAMAGE		BG_GetWeaponDamage(WP_M1919A4)

#ifdef __ETE__
#define GREASE_SPREAD				BG_GetWeaponSpread(WP_GREASE)
#define	GREASE_DAMAGE				BG_GetWeaponDamage(WP_GREASE) // JPW

#define MOBILE_MG34_SPREAD			BG_GetWeaponSpread(WP_MOBILE_MG34)
#define	MOBILE_MG34_DAMAGE			BG_GetWeaponDamage(WP_MOBILE_MG34)

#define MOBILE_M1919A6_SPREAD		BG_GetWeaponSpread(WP_M1919A6)
#define	MOBILE_M1919A6_DAMAGE		BG_GetWeaponDamage(WP_M1919A6)

#define G43_SPREAD					BG_GetWeaponSpread(WP_G43)
#define	G43_DAMAGE					BG_GetWeaponDamage(WP_G43) // JPW
#endif

#define M1CARBINE_SPREAD			BG_GetWeaponSpread(WP_M1CARBINE)
#define	M1CARBINE_DAMAGE			BG_GetWeaponDamage(WP_M1CARBINE)
#define M1CARBINE_SCOPE_SPREAD		BG_GetWeaponSpread(WP_M1CARBINE_AIM)
#define	M1CARBINE_SCOPE_DAMAGE		BG_GetWeaponDamage(WP_M1CARBINE_AIM)

#define BAR_SPREAD					BG_GetWeaponSpread(WP_PPS)
#define	BAR_DAMAGE					BG_GetWeaponDamage(WP_PPS) // JPW

#define BAR_AIM_SPREAD				BG_GetWeaponSpread(WP_PPS_AIM)
#define	BAR_AIM_DAMAGE				BG_GetWeaponDamage(WP_PPS_AIM) // JPW

#define K98_AIM_SPREAD				BG_GetWeaponSpread(WP_K98_AIM)
#define	K98_AIM_DAMAGE				BG_GetWeaponDamage(WP_K98_AIM) // JPW
#define K98_SPREAD					BG_GetWeaponSpread(WP_K98)
#define	K98_DAMAGE					BG_GetWeaponDamage(WP_K98) // JPW

#define M1GARAND_AIM_SPREAD			BG_GetWeaponSpread(WP_M1GARAND_AIM)
#define	M1GARAND_AIM_DAMAGE			BG_GetWeaponDamage(WP_M1GARAND_AIM) // JPW
#define M1GARAND_SPREAD				BG_GetWeaponSpread(WP_M1GARAND)
#define	M1GARAND_DAMAGE				BG_GetWeaponDamage(WP_M1GARAND) // JPW

#define MP44_AIM_SPREAD				BG_GetWeaponSpread(WP_MP44_AIM)
#define	MP44_AIM_DAMAGE				BG_GetWeaponDamage(WP_MP44_AIM) // JPW
#define MP44_SPREAD					BG_GetWeaponSpread(WP_MP44)
#define	MP44_DAMAGE					BG_GetWeaponDamage(WP_MP44) // JPW

#define MP40_AIM_SPREAD				BG_GetWeaponSpread(WP_MP40_AIM)
#define	MP40_AIM_DAMAGE				BG_GetWeaponDamage(WP_MP40_AIM) // JPW
#define THOMPSON_AIM_SPREAD			BG_GetWeaponSpread(WP_THOMPSON_AIM)
#define	THOMPSON_AIM_DAMAGE			BG_GetWeaponDamage(WP_THOMPSON_AIM) // JPW
#endif

#define LUGER_DIST					BG_GetWeaponDistance(WP_LUGER)
#define SILENCER_DIST				BG_GetWeaponDistance(WP_SILENCED_LUGER)
#define AKIMBO_LUGER_DIST			BG_GetWeaponDistance(WP_AKIMBO_LUGER)
#define AKIMBO_SILENCEDLUGER_DIST	BG_GetWeaponDistance(WP_AKIMBO_SILENCEDLUGER)
#define	COLT_DIST					BG_GetWeaponDistance(WP_COLT) // JPW
#define SILENCED_COLT_DIST			BG_GetWeaponDistance(WP_SILENCED_COLT)
#define AKIMBO_COLT_DIST			BG_GetWeaponDistance(WP_AKIMBO_COLT)
#define AKIMBO_SILENCEDCOLT_DIST	BG_GetWeaponDistance(WP_AKIMBO_SILENCEDCOLT)
#define	MP40_DIST					BG_GetWeaponDistance(WP_MP40) // JPW
#define	THOMPSON_DIST				BG_GetWeaponDistance(WP_THOMPSON) // JPW
#define	STEN_DIST					BG_GetWeaponDistance(WP_STEN) // JPW
#define	GARAND_DIST					BG_GetWeaponDistance(WP_GARAND) // JPW
#define	KAR98_DIST					BG_GetWeaponDistance(WP_KAR98)
#define	CARBINE_DIST				BG_GetWeaponDistance(WP_CARBINE)
#define	KAR98_GREN_DIST				BG_GetWeaponDistance(WP_GREN_KAR98)
#define	MOBILE_MG42_DIST			BG_GetWeaponDistance(WP_MOBILE_MG42)
#define	FG42_DIST					BG_GetWeaponDistance(WP_FG42) // JPW
#define	FG42SCOPE_DIST				BG_GetWeaponDistance(WP_FG42SCOPE) // JPW
#define	K43_DIST					BG_GetWeaponDistance(WP_K43)
#define GARANDSCOPE_DIST			BG_GetWeaponDistance(WP_GARAND_SCOPE)
#define K43SCOPE_DIST				BG_GetWeaponDistance(WP_K43_SCOPE)
#define	MOBILE_M1919A4_DIST			BG_GetWeaponDistance(WP_M1919A4)

#ifdef __RW__
#ifdef __ETE__
#define G43_AIM_SPREAD				BG_GetWeaponSpread(WP_G43_AIM)
#define	G43_AIM_DAMAGE				BG_GetWeaponDamage(WP_G43_AIM) // JPW
#define	G43_DIST					BG_GetWeaponDistance(WP_G43) // JPW
#define	G43_AIM_DIST				BG_GetWeaponDistance(WP_G43_AIM) // JPW
#define G43_AIM_SPREAD				BG_GetWeaponSpread(WP_G43_AIM)
#define	GREASE_DIST					BG_GetWeaponDistance(WP_GREASE) // JPW
#define GREASE_AIM_SPREAD			BG_GetWeaponSpread(WP_GREASE_AIM)
#define	GREASE_AIM_DAMAGE			BG_GetWeaponDamage(WP_GREASE_AIM) // JPW
#define	GREASE_AIM_DIST				BG_GetWeaponDistance(WP_GREASE_AIM) // JPW
#define	MOBILE_MG34_DIST			BG_GetWeaponDistance(WP_MOBILE_MG34)
#define	MOBILE_M1919A6_DIST			BG_GetWeaponDistance(WP_M1919A6)
#endif
#define M1CARBINE_DIST				BG_GetWeaponDistance(WP_M1CARBINE)
#define	M1CARBINE_SCOPE_DIST		BG_GetWeaponDistance(WP_M1CARBINE_AIM)
#define BAR_DIST					BG_GetWeaponDistance(WP_PPS)
#define BAR_AIM_DIST				BG_GetWeaponDistance(WP_PPS_AIM)
#define K98_AIM_DIST				BG_GetWeaponDistance(WP_K98_AIM)
#define	K98_DIST					BG_GetWeaponDistance(WP_K98) // JPW
#define M1GARAND_AIM_DIST			BG_GetWeaponDistance(WP_M1GARAND_AIM)
#define	M1GARAND_DIST				BG_GetWeaponDistance(WP_M1GARAND) // JPW
#define	MP44_AIM_DIST				BG_GetWeaponDistance(WP_MP44_AIM) // JPW
#define MP44_DIST					BG_GetWeaponDistance(WP_MP44)
#define MP40_AIM_DIST				BG_GetWeaponDistance(WP_MP40_AIM)
#define	THOMPSON_AIM_DIST			BG_GetWeaponDistance(WP_THOMPSON_AIM) // JPW
#endif

/*#define NPC_SOLDIER 0	// 0
#define NPC_MEDIC 1		// 1
#define NPC_ENGINEER 2	// 2
#define NPC_FIELDOPS 3	// 3
#define NPC_COVERTOPS 4	// 4*/

qboolean node_spawned_at[MAX_PATHLIST_NODES];

// UQ1: .npc files need to match this!!!
stringID_table_t NPC_ClassTable[] =
{
	ENUM2STRING( PC_SOLDIER ),
	ENUM2STRING( PC_MEDIC ),
	ENUM2STRING( PC_ENGINEER ),
	ENUM2STRING( PC_FIELDOPS ),
	ENUM2STRING( PC_COVERTOPS ),
};

void G_ListWeaponBanks ( void )
{
	int bnk, cyc;

	for(bnk = 0; bnk < MAX_WEAP_BANKS_MP; bnk++) 
	{
		G_Printf("Bank: %i: ", bnk);

		for(cyc = 0; cyc < MAX_WEAPS_IN_BANK_MP; cyc++) 
		{
				// found the current weapon
				if (weapBanksMultiPlayer[bnk][cyc] == 0)
					G_Printf("%i (-) ", cyc);
				else
					G_Printf("%i (%s) ", cyc, WeaponTableStrings[weapBanksMultiPlayer[bnk][cyc]].name);
		}

		G_Printf("\n");
	}
}

int NPC_ShortenASTARRoute(/*short*/ int *pathlist, int number)
{// Wrote this to make the routes a little smarter (shorter)... No point looping back to the same places... Unique1
	/*short*/ int temppathlist[MAX_PATHLIST_NODES];
	int count = 0;
//	int count2 = 0;
	int temp, temp2;
	int link;

	for (temp = number; temp >= 0; temp--)
	{
		qboolean shortened = qfalse;

		for (temp2 = 0; temp2 < temp; temp2++)
		{
			for (link = 0; link < nodes[pathlist[temp]].enodenum; link++)
			{
				if (nodes[pathlist[temp]].links[link].targetNode == pathlist[temp2])
				{// Found a shorter route...
					if (OrgVisible(nodes[pathlist[temp2]].origin, nodes[pathlist[temp]].origin, -1))
					{
						temppathlist[count] = pathlist[temp2];
						temp = temp2;
						count++;
						shortened = qtrue;
					}
				}
			}
		}

		if (!shortened)
		{
			temppathlist[count] = pathlist[temp];
			count++;
		}
	}

	pathlist = temppathlist;

	//G_Printf("ShortenASTARRoute: Path size reduced from %i to %i nodes...\n", number, count);
	return count;
}

int NPC_CreateDumbRoute(int from, int to, /*short*/ int *pathlist)
{// Create a route not using links...
	int count = 0;
	int number = 0;
	int upto = 0;

	if (to < from)
	{
		if (from - to > MAX_PATHLIST_NODES)
			return -1;

		for (upto = from; upto > to; upto--)
		{
			pathlist[number] = upto;
			number++;
		}

		count = number;
	}
	else
	{
		if (to - from > MAX_PATHLIST_NODES)
			return -1;

		for (upto = to; upto > from; upto--)
		{
			pathlist[number] = upto;
			number++;
		}

		count = number;
	}

	return count-1;
}

extern int GetFCost(gentity_t *bot, int to, int num, int parentNum, float *gcost); // Below...
extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );
extern qboolean HaveSequentialNodes ( void );
extern qboolean AI_PM_SlickTrace ( vec3_t point, int clientNum );

int NPC_CreatePathAStar(gentity_t *NPC, int from, int to, /*short*/ int *pathlist)
{
	//all the data we have to hold...since we can't do dynamic allocation, has to be MAX_PATHLIST_NODES
	//we can probably lower this later - eg, the open list should never have more than at most a few dozen items on it
	/*short*/ int openlist[MAX_NODES+1];	//add 1 because it's a binary heap, and they don't use 0 - 1 is the first used index
	float gcost[MAX_NODES];
	int fcost[MAX_NODES];
	char list[MAX_NODES];	//0 is neither, 1 is open, 2 is closed - char because it's the smallest data type
	/*short*/ int parent[MAX_NODES];

	/*short*/ int numOpen = 0;
	/*short*/ int atNode, temp, newnode=-1;
	qboolean found = qfalse;
	int count = -1;
	float gc;
	int i, j, u, v, m;
	vec3_t vec;
	qboolean onIce = AI_PM_SlickTrace(NPC->r.currentOrigin, NPC->s.number);

#ifdef __NPC_FAST_ROUTING__
	if (IsCoopGametype())
	{// In COOP, we always want shortest routes possible...
		count = NPC_CreateDumbRoute(from, to, pathlist);
	
		if (count > 0)
		{
			//count = NPC_ShortenASTARRoute(pathlist, count);
			return count;
		}
	}
#endif //__NPC_FAST_ROUTING__

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

	//make sure we have valid data before calculating everything
	if ((from == NODE_INVALID) || (to == NODE_INVALID) || (from >= MAX_NODES) || (to >= MAX_NODES) || (from == to))
		return -1;

	openlist[1] = from;		//add the starting node to the open list
	numOpen++;
	gcost[from] = 0;		//its f and g costs are obviously 0
	fcost[from] = 0;

	while (1)
	{
		if (numOpen != 0)	//if there are still items in the open list
		{
			//pop the top item off of the list
			atNode = openlist[1];	
			list[atNode] = 2;		//put the node on the closed list so we don't check it again
			numOpen--;

			openlist[1] = openlist[numOpen+1];	//move the last item in the list to the top position
			v = 1;

			//this while loop reorders the list so that the new lowest fcost is at the top again
			while (1)
			{
				u = v;
				if ((2*u+1) < numOpen)	//if NPCh children exist
				{
					if (fcost[openlist[u]] >= fcost[openlist[2*u]])
						v = 2*u;
					if (fcost[openlist[v]] >= fcost[openlist[2*u+1]])
						v = 2*u+1;
				}
				else
				{
					if ((2*u) < numOpen)	//if only one child exists
					{
						if (fcost[openlist[u]] >= fcost[openlist[2*u]])
							v = 2*u;
					}
				}

				if (u != v)		//if they're out of order, swap this item with its parent
				{
					temp = openlist[u];
					openlist[u] = openlist[v];
					openlist[v] = temp;
				}
				else
					break;
			}

			if (nodes[atNode].enodenum <= MAX_NODELINKS)
			for (i = 0; i < nodes[atNode].enodenum; i++)	//loop through all the links for this node
			{
				newnode = nodes[atNode].links[i].targetNode;	

				if (newnode > number_of_nodes)
					continue;

				if (newnode < 0)
					continue;

				if (nodes[atNode].links[i].flags & PATH_BLOCKED)	//if this path is blocked, skip it
					continue;
				if ( nodes[atNode].links[i].flags & PATH_DANGER )
					continue;																//if this path is blocked, skip it
				//skip any unreachable nodes
				if ((nodes[newnode].type & NODE_ALLY_UNREACHABLE) && (NPC->s.teamNum == TEAM_ALLIES))
					continue;
				if ((nodes[newnode].type & NODE_AXIS_UNREACHABLE) && (NPC->s.teamNum == TEAM_AXIS))
					continue;

				if (!onIce && (nodes[newnode].type & NODE_ICE))
					continue; // NPCs never go on ice!

				/*if ( BAD_WP_Height( nodes[atNode].origin, nodes[newnode].origin) )
				{																		// Height diff is greater then plane distance... Block it and skip it..
					BOT_MakeLinkSemiUnreachable( NPC, atNode, i );
					continue;
				}*/

				if (list[newnode] == 2)		//if this node is on the closed list, skip it
					continue;

				/*if (VectorDistance(nodes[from].origin, nodes[newnode].origin) < 1024)
				{// UQ1: Can avoid this check if it is a long way away...
					if (AIMOD_MOVEMENT_Explosive_Near_Waypoint(newnode))					
					{// UQ1: If explosive near it. Skip it!
						continue;
					}
				}*/

				if (list[newnode] != 1)		//if this node is not already on the open list
				{
					openlist[++numOpen] = newnode;	//add the new node to the open list
					list[newnode] = 1;
					parent[newnode] = atNode;	//record the node's parent

					if (newnode == to)			//if we've found the goal, don't keep computing paths!
						break;			//this will break the 'for' and go all the way to 'if (list[to] == 1)'

					fcost[newnode] = GetFCost(NPC, to, newnode, parent[newnode], gcost);	//store it's f cost value

					//this loop re-orders the heap so that the lowest fcost is at the top 
					m = numOpen;
					while (m != 1)	//while this item isn't at the top of the heap already
					{
						if (fcost[openlist[m]] <= fcost[openlist[m/2]])	//if it has a lower fcost than its parent
						{
							temp = openlist[m/2];
							openlist[m/2] = openlist[m];
							openlist[m] = temp;				//swap them
							m /= 2;
						}
						else
							break;
					}
				}
				else		//if this node is already on the open list
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

					if (gc < gcost[newnode])	//if the new gcost is less (ie, this path is shorter than what we had before)
					{
						parent[newnode] = atNode;	//set the new parent for this node
						gcost[newnode] = gc;		//and the new g cost

						for (j = 1; j < numOpen; j++)	//loop through all the items on the open list
						{
							if (openlist[j] == newnode)	//find this node in the list
							{
								//calculate the new fcost and store it
								fcost[newnode] = GetFCost(NPC, to, newnode, parent[newnode], gcost);

								//reorder the list again, with the lowest fcost item on top
								m = j;
								while (m != 1)
								{
									if (fcost[openlist[m]] < fcost[openlist[m/2]])	//if the item has a lower fcost than it's parent
									{
										temp = openlist[m/2];
										openlist[m/2] = openlist[m];
										openlist[m] = temp;				//swap them
										m /= 2;
									}
									else
										break;
								}
								break;	//exit the 'for' loop because we already changed this node
							} //if
						} //for
					} //if (gc < gcost[newnode])
				} //if (list[newnode] != 1) --> else
			} //for (loop through links)
		} //if (numOpen != 0)
		else
		{
			found = qfalse;		//there is no path between these nodes
			break;
		}

		if (list[to] == 1)		//if the destination node is on the open list, we're done
		{
			found = qtrue;
			break;
		}
	} //while (1)

	if (found == qtrue)		//if we found a path
	{
		count = 0;

		temp = to;				//start at the end point
		while (temp != from)	//travel along the path (backwards) until we reach the starting point
		{
			if (count+1 >= MAX_PATHLIST_NODES)
				return -1; // UQ1: Added to stop crash if path is too long for the memory allocation...

			pathlist[count++] = temp;	//add the node to the pathlist and increment the count
			temp = parent[temp];		//move to the parent of this node to continue the path
		}

		pathlist[count++] = from;		//add the beginning node to the end of the pathlist
	}
	
	/*if (!found && HaveSequentialNodes())
	{
		count = NPC_CreateDumbRoute(from, to, pathlist);
	}*/

	return count;	//return the number of nodes in the path, -1 if not found
}

extern int BotMP_Find_POP_Goal_EntityNum ( gentity_t *bot, int ignoreEnt, int ignoreEnt2, vec3_t current_org ); // ai_dmgoal_mp.c
extern int BotMP_Engineer_Find_POP_Goal_EntityNum ( gentity_t *bot, int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum );
extern int BotMP_FieldOps_Find_POP_Goal_EntityNum (  gentity_t *bot, int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum );
extern int BotMP_Soldier_Find_POP_Goal_EntityNum ( gentity_t *bot, int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum );
extern int BotMP_Sniper_Find_Goal_EntityNum ( gentity_t *bot, int ignoreEnt, int ignoreEnt2, vec3_t current_org, int teamNum );
extern void UpdateAttackerDirection( void );

int NPC_FindGoal( gentity_t *NPC ) 
{// Returns goal waypoint...
	int entNum = -1;
	int best_target = -1;
	float best_dist = 99999.9f;

	// Update direction for defence bots/NPCs...
	UpdateAttackerDirection();

	best_target = -1; // Initialize with each loop...
	best_dist = 99999.9f; // Initialize with each loop...

	if (NPC->playerType == PC_ENGINEER)
		entNum = BotMP_Engineer_Find_POP_Goal_EntityNum( NPC, NPC->s.number, NPC->current_target_entity, NPC->r.currentOrigin, NPC->s.teamNum );
	else if (NPC->playerType == PC_SOLDIER)
		entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( NPC, NPC->s.number, NPC->current_target_entity, NPC->r.currentOrigin, NPC->s.teamNum );
	else if (NPC->playerType == PC_FIELDOPS)
		entNum = BotMP_FieldOps_Find_POP_Goal_EntityNum( NPC, NPC->s.number, NPC->current_target_entity, NPC->r.currentOrigin, NPC->s.teamNum );
	else
		entNum = BotMP_Find_POP_Goal_EntityNum( NPC, NPC->s.number, NPC->current_target_entity, NPC->r.currentOrigin );

	if (entNum >= 0)
	{
		int loop = 0;

		for (loop = 0; loop < number_of_nodes;loop++)
		{// Find closest node(s) to the objective...
			float dist = VectorDistance(nodes[loop].origin, g_entities[entNum].r.currentOrigin);

			if ( dist <= 256 && dist < best_dist )
			{
				best_dist = VectorDistance(nodes[loop].origin, g_entities[entNum].r.currentOrigin);
				best_target = loop;
			}
		}

		memset(NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES);
	}

	if (best_target >= 0)
	{// Found a goal.. Use it...
		NPC->current_target_entity = entNum;
		return best_target;
	}

	// If we got here, then we failed to find a path to a goal...
	return -1;
}

extern vmCvar_t	npc_simplenav;
extern /*short*/ int BotGetNextNode(gentity_t *bot/*, int *state*/); // unique_movement.c

//===========================================================================
// Routine      : NPC_MOVEMENT_ReachableBy
// Description  : Determine if a blocked goal vector is reachable.. Returns how to get there...
#define NOT_REACHABLE -1
#define REACHABLE 0
#define REACHABLE_JUMP 1
#define REACHABLE_DUCK 2
#define REACHABLE_STRAFE_LEFT 3
#define REACHABLE_STRAFE_RIGHT 4

int NPC_MOVEMENT_ReachableBy(gentity_t *NPC, vec3_t goal)
{
    trace_t trace;
    vec3_t v, v2, eyes, mins, maxs, Org, forward, right, up, rightOrigin, leftOrigin;

	// First check direct movement...
	VectorCopy(NPC->r.currentOrigin, Org);

    VectorCopy(NPC->r.mins,v);
    v[2] += 18; // Stepsize
	trap_Trace(&trace, Org, v, NPC->r.maxs, goal, NPC->s.number, MASK_SOLID|MASK_OPAQUE);
    if (trace.fraction == 1.0)
        return REACHABLE; // Yes we can see it

	// Now look for jump access...
	VectorAdd( Org, playerMins, mins );
	VectorAdd( Org, playerMaxs, maxs );

	VectorCopy(NPC->r.mins,v);
    v[2] += 18; // Stepsize
	VectorCopy(Org, eyes);
	eyes[2]+=32;
	trap_Trace(&trace, eyes, v, NPC->r.maxs, goal, NPC->s.number, MASK_SOLID|MASK_OPAQUE);
    if (trace.fraction == 1.0)
        return REACHABLE_JUMP; // Yes we can see it

	// Now look for crouch access...
	//CROUCH_VIEWHEIGHT ??
	VectorCopy(NPC->r.mins,v);
    v[2] += 18; // Stepsize
	VectorCopy(NPC->r.maxs,v2);
    v2[2] *= 0.5; // Stepsize
	trap_Trace(&trace, Org, v, v2, goal, NPC->s.number, MASK_SOLID|MASK_OPAQUE);
    if (trace.fraction == 1.0)
        return REACHABLE_DUCK; // Yes we can see it

	// Now look for strafe access... Left or Right...
	AngleVectors( NPC->NPC_client->ps.viewangles, forward, right, up );
	
	VectorMA( NPC->r.currentOrigin, 64, right, rightOrigin );
	VectorMA( NPC->r.currentOrigin, -64, right, leftOrigin );

	if (OrgVisible(leftOrigin, nodes[NPC->current_node].origin, NPC->s.number))
	{
		NPC->bot_strafe_left_timer = level.time + 201;
		VectorCopy(leftOrigin, NPC->bot_strafe_target_position);
		return REACHABLE_STRAFE_LEFT; // Yes we can see it
	}

	if (OrgVisible(rightOrigin, nodes[NPC->current_node].origin, NPC->s.number))
	{
		NPC->bot_strafe_right_timer = level.time + 201;
		VectorCopy(rightOrigin, NPC->bot_strafe_target_position);
		return REACHABLE_STRAFE_RIGHT; // Yes we can see it
	}

	// And strafing a bit further away...
	VectorMA( NPC->r.currentOrigin, 128, right, rightOrigin );
	VectorMA( NPC->r.currentOrigin, -128, right, leftOrigin );

	if (OrgVisible(leftOrigin, nodes[NPC->current_node].origin, NPC->s.number))
	{
		NPC->bot_strafe_left_timer = level.time + 401;
		VectorCopy(leftOrigin, NPC->bot_strafe_target_position);
		return REACHABLE_STRAFE_LEFT; // Yes we can see it
	}

	if (OrgVisible(rightOrigin, nodes[NPC->current_node].origin, NPC->s.number))
	{
		NPC->bot_strafe_right_timer = level.time + 401;
		VectorCopy(rightOrigin, NPC->bot_strafe_target_position);
		return REACHABLE_STRAFE_RIGHT; // Yes we can see it
	}

	// And strafing a bit further away...
	VectorMA( NPC->r.currentOrigin, 192, right, rightOrigin );
	VectorMA( NPC->r.currentOrigin, -192, right, leftOrigin );

	if (OrgVisible(leftOrigin, nodes[NPC->current_node].origin, NPC->s.number))
	{
		NPC->bot_strafe_left_timer = level.time + 601;
		VectorCopy(leftOrigin, NPC->bot_strafe_target_position);
		return REACHABLE_STRAFE_LEFT; // Yes we can see it
	}

	if (OrgVisible(rightOrigin, nodes[NPC->current_node].origin, NPC->s.number))
	{
		NPC->bot_strafe_right_timer = level.time + 601;
		VectorCopy(rightOrigin, NPC->bot_strafe_target_position);
		return REACHABLE_STRAFE_RIGHT; // Yes we can see it
	}

	return NOT_REACHABLE;
}

//extern void MuzzleRayTracing(gentity_t*, int);
extern void ClientEvents( gentity_t *ent, int oldEventSequence );
extern void ClientImpacts( gentity_t *ent, pmove_t *pm );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void G_StoreNPCPosition(gentity_t* ent)
{
	int				currentTime;
	clientMarker_t*	pMarker;

	if (ent->inuse 
 		&& (ent->NPC_client->sess.sessionTeam == TEAM_AXIS || ent->NPC_client->sess.sessionTeam == TEAM_ALLIES)
 		&& ent->r.linked
 		&& ent->health > 0
 		&& (ent->NPC_client->ps.pm_flags & PMF_LIMBO) == 0
		&& ent->NPC_client->ps.pm_type == PM_NORMAL)
	{
		pMarker = ent->NPC_client->clientMarkers + ent->NPC_client->topMarker;

		// new frame, mark the old marker's time as the end of the last frame
		if (pMarker->time < level.time)
		{
			pMarker->time = level.previousTime;
			ent->NPC_client->topMarker = (ent->NPC_client->topMarker == MAX_CLIENT_MARKERS - 1) ? 0 : ent->NPC_client->topMarker + 1;
			pMarker = ent->NPC_client->clientMarkers + ent->NPC_client->topMarker;
		}

		currentTime = level.previousTime + trap_Milliseconds() - bot_thinktime.integer;//level.FRAMETIME;

		if (currentTime > level.time)
			// owwie, we just went into the next frame... let's push them back
			currentTime = level.time;

		VectorCopy(ent->r.mins, pMarker->mins);
		VectorCopy(ent->r.maxs, pMarker->maxs);
		VectorCopy(ent->r.currentOrigin, pMarker->origin);
//		pMarker->servertime = level.time;
		pMarker->time = currentTime;
#ifdef UNLAGGED // ??? Must be fixed ???
		pMarker->eFlags = ent->NPC_client->ps.eFlags;
		pMarker->pmf_flags = ent->NPC_client->ps.pm_flags;
#endif
	}
}

/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_NPCStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap ) {
	int		i;

	if (ps->clientNum == 0)
	{
		G_Printf("BG_NPCStateToEntityState: ps->clientNum == 0\n");
		return;
	}

	s->eType = ET_NPC;
	//s->number = ps->clientNum;

	s->pos.trType = TR_INTERPOLATE;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap ) {
		SnapVector( s->pos.trBase );
	}

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}

	if (ps->movementDir > 128)
		s->angles2[YAW] = (float)ps->movementDir - 256;
	else
		s->angles2[YAW] = ps->movementDir;

//	s->modelindex2	= ps->persistant[PERS_HWEAPON_USE];
	s->legsAnim		= ps->legsAnim;
	s->torsoAnim	= ps->torsoAnim;
//	s->clientNum	= ps->clientNum;	// ET_PLAYER looks here instead of at number
										// so corpses can also reference the proper config
	// Ridah, let clients know if this person is using a mounted weapon
	// so they don't show any client muzzle flashes
	if( ps->eFlags & EF_MOUNTEDTANK ) {
		ps->eFlags &= ~EF_MG42_ACTIVE;
		ps->eFlags &= ~EF_AAGUN_ACTIVE;
	} else {
//		SETUP_MOUNTEDGUN_STATUS( ps );
	}

	s->eFlags = ps->eFlags;

	ps->stats[STAT_HEALTH] = g_entities[s->number].health;

	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

// from MP
	if ( ps->externalEvent ) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if ( ps->entityEventSequence < ps->eventSequence ) {
		int		seq;

		if ( ps->entityEventSequence < ps->eventSequence - MAX_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_EVENTS-1);
		s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}
// end
	// Ridah, now using a circular list of events for all entities
	// add any new events that have been added to the playerState_t
	// (possibly overwriting entityState_t events)
	for (i = ps->oldEventSequence; i != ps->eventSequence; i++) {
		s->events[s->eventSequence & (MAX_EVENTS-1)] = ps->events[i & (MAX_EVENTS-1)];
		s->eventParms[s->eventSequence & (MAX_EVENTS-1)] = ps->eventParms[i & (MAX_EVENTS-1)];
		s->eventSequence++;
	}
	ps->oldEventSequence = ps->eventSequence;

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( ps->powerups[ i ] ) {
			s->powerups |= 1 << i;
		}
	}

	#ifdef __LEANING__ 
	s->constantLight = ps->leanf*1000; // Does really *1000 make it better?
	#endif

	s->nextWeapon = ps->nextWeapon;	// Ridah
//	s->loopSound = ps->loopSound;
//	s->teamNum = ps->teamNum;

#ifdef __RW__
	s->curweapAnim = ps->weapAnim;
	s->aiState = AISTATE_COMBAT;
#else
	s->aiState = ps->aiState;		// xkan, 1/10/2003
#endif
	s->clientNum = -1;//ps->clientNum;
}

/*
========================
BG_NPCStateToEntityStateExtraPolate

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_NPCStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, int time, qboolean snap ) {
	int		i;

	if (ps->clientNum == 0)
	{
		G_Printf("BG_NPCStateToEntityState: ps->clientNum == 0\n");
		return;
	}

	s->eType = ET_NPC;
	//s->number = ps->clientNum;

/*	s->pos.trType = TR_LINEAR_STOP;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap ) {
		SnapVector( s->pos.trBase );
	}
	// set the trDelta for flag direction and linear prediction
	VectorCopy( ps->velocity, s->pos.trDelta );
	// set the time for linear prediction
	s->pos.trTime = time;
	// set maximum extra polation time
	s->pos.trDuration = BG_GetServerFrameRate(); // 1000 / sv_fps (default = 20)

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}
*/
	s->pos.trType = TR_LINEAR_STOP;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap ) {
		SnapVector( s->pos.trBase );
	}
	// set the trDelta for flag direction and linear prediction
	VectorCopy( ps->velocity, s->pos.trDelta );
	// set the time for linear prediction
	s->pos.trTime = time;
	// set maximum extra polation time
	s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}

	//G_Printf("Movedir is %i.\n",ps->movementDir);
	s->angles2[YAW] = ps->movementDir;
//	s->modelindex2	= ps->persistant[PERS_HWEAPON_USE];
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
//	s->clientNum = ps->clientNum;		// ET_PLAYER looks here instead of at number
										// so corpses can also reference the proper config

	if( ps->eFlags & EF_MOUNTEDTANK ) {
		ps->eFlags &= ~EF_MG42_ACTIVE;
		ps->eFlags &= ~EF_AAGUN_ACTIVE;
	} else {
		//SETUP_MOUNTEDGUN_STATUS( ps );
	}

	s->eFlags = ps->eFlags;

	ps->stats[STAT_HEALTH] = g_entities[s->number].health;

	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	if ( ps->externalEvent ) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if ( ps->entityEventSequence < ps->eventSequence ) {
		int		seq;

		if ( ps->entityEventSequence < ps->eventSequence - MAX_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_EVENTS-1);
		s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}

	// Ridah, now using a circular list of events for all entities
	// add any new events that have been added to the playerState_t
	// (possibly overwriting entityState_t events)
	for (i = ps->oldEventSequence; i != ps->eventSequence; i++) {
		s->events[s->eventSequence & (MAX_EVENTS-1)] = ps->events[i & (MAX_EVENTS-1)];
		s->eventParms[s->eventSequence & (MAX_EVENTS-1)] = ps->eventParms[i & (MAX_EVENTS-1)];
		s->eventSequence++;
	}
	ps->oldEventSequence = ps->eventSequence;

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( ps->powerups[ i ] ) {
			s->powerups |= 1 << i;
		}
	}

	s->nextWeapon = ps->nextWeapon;	// Ridah
//	s->teamNum = ps->teamNum;

#ifdef __RW__
	s->curweapAnim = ps->weapAnim;
	s->aiState = AISTATE_COMBAT;		// xkan, 1/10/2003
#else
	s->aiState = ps->aiState;		// xkan, 1/10/2003
#endif

	s->clientNum = -1;//ps->clientNum;
}

/*
==================
SetNPCClientViewAngle

==================
*/
void SetNPCClientViewAngle(gentity_t *ent, vec3_t angle) {
	int			i;

	// set the delta angle
	for (i=0 ; i<3 ; i++) {
		int		cmdAngle;

		cmdAngle = ANGLE2SHORT(angle[i]);
		ent->NPC_client->ps.delta_angles[i] = cmdAngle - ent->NPC_client->pers.cmd.angles[i];
	}
	VectorCopy(angle, ent->s.angles);
	VectorCopy (ent->s.angles, ent->NPC_client->ps.viewangles);
}

void SetNPCClientViewAnglePitch(gentity_t *ent, vec_t angle) {
	int	cmdAngle;

	cmdAngle = ANGLE2SHORT(angle);
	ent->NPC_client->ps.delta_angles[PITCH] = cmdAngle - ent->NPC_client->pers.cmd.angles[PITCH];

	ent->s.angles[ PITCH ] = 0;
	VectorCopy(ent->s.angles, ent->NPC_client->ps.viewangles);
}

extern void			BotSetViewAngles ( gentity_t *bot, float thinktime );

/*
==============
NPCSetViewAngle
==============
*/
float NPCSetViewAngle(float angle, float ideal_angle, float speed) 
{
	float move;

	angle = AngleMod(angle);
	ideal_angle = AngleMod(ideal_angle);
	if (angle == ideal_angle) return angle;
	move = ideal_angle - angle;
	if (ideal_angle > angle) {
		if (move > 180.0) move -= 360.0;
	}
	else {
		if (move < -180.0) move += 360.0;
	}
	if (move > 0) {
		if (move > speed) move = speed;
	}
	else {
		if (move < -speed) move = -speed;
	}
	return AngleMod(angle + move);
}

/*
==============
NPCSetViewAngles
==============
*/
void NPCSetViewAngles(gentity_t *NPC, float thinktime) 
{// Just keep NPC->bot_ideal_view_angles correct and call this to make it realistic...
	/*float diff, factor, maxchange, anglespeed;
	int i;

	if (NPC->bot_ideal_view_angles[PITCH] > 180) NPC->bot_ideal_view_angles[PITCH] -= 360;

	//
	factor = 0.15;
	maxchange = 240;
	maxchange *= thinktime;

	for (i = 0; i < 2; i++) 
	{
		diff = fabs(AngleDifference(NPC->NPC_client->ps.viewangles[i], NPC->bot_ideal_view_angles[i]));
		anglespeed = diff * factor;

		if (anglespeed > maxchange) anglespeed = maxchange;

		NPC->NPC_client->ps.viewangles[i] = NPCSetViewAngle(NPC->NPC_client->ps.viewangles[i], NPC->bot_ideal_view_angles[i], anglespeed);
	}

	if (NPC->NPC_client->ps.viewangles[PITCH] > 180) NPC->NPC_client->ps.viewangles[PITCH] -= 360;

	//trap_EA_View(NPC->s.number, NPC->NPC_client->ps.viewangles);
	//VectorCopy(NPC->NPC_client->ps.viewangles, NPC->s.angles);
	SetNPCClientViewAngle(NPC, NPC->NPC_client->ps.viewangles);*/
	BotSetViewAngles( NPC, thinktime );
}

/*static entityState_t* G_NPCGetEntityState(int entityNum)
{
	if (entityNum < 0 || entityNum >= MAX_GENTITIES)
		return NULL;

	return &g_entities[entityNum].s;
}*/

void NPCThink( int entityNum ); // below...

void NPCThink_real( gentity_t *ent ) 
{
	int			msec, oldEventSequence, monsterslick = 0;
	pmove_t		pm;
	usercmd_t	*ucmd;
	gclient_t	*client = ent->NPC_client;

	client->ps.clientNum = ent->s.number;

#ifdef __RW__
	if (client->ps.weapon != WP_KNIFE2)
		client->ps.extFlags &= ~EX_DEFUSING;
#endif

/*	if( ent->s.dl_intensity == TAG_HOOK || ent->s.dl_intensity == TAG_HOOK_DRIVER || ent->s.dl_intensity == TAG_HOOK_GUNNER1 || ent->s.dl_intensity == TAG_HOOK_GUNNER2 ) {
		client->pmext.centerangles[YAW] = g_entities[ent->s.otherEntityNum].r.currentAngles[ YAW ];
		client->pmext.centerangles[PITCH] = g_entities[ent->s.otherEntityNum].r.currentAngles[ YAW ];
	}*/

	if( (ent->s.eFlags & EF_MOUNTEDTANK) && ent->tagParent) {
		client->pmext.centerangles[YAW] = ent->tagParent->r.currentAngles[ YAW ];
		client->pmext.centerangles[PITCH] = ent->tagParent->r.currentAngles[ PITCH ];
	}

//#ifdef _DEBUG
//	if (g_debugBullets.integer > 1)
//	{
//		if (!ent->NPC_client->ps.persistant[PERS_HWEAPON_USE])
//			MuzzleRayTracing(ent, ent->s.weapon);
//	}
//#endif

	// mark the time, so the connection sprite can be removed
	ucmd = &ent->NPC_client->pers.cmd;

	ent->NPC_client->ps.identifyClient = ucmd->identClient;		// NERVE - SMF

	// sanity check the command time to prevent speedup cheating
	if ( ucmd->serverTime > level.time + 200 ) {
		ucmd->serverTime = level.time + 200;
//		G_Printf("serverTime <<<<<\n" );
	}
	if ( ucmd->serverTime < level.time - 1000 ) {
		ucmd->serverTime = level.time - 1000;
//		G_Printf("serverTime >>>>>\n" );
	}

	msec = ucmd->serverTime - client->ps.commandTime;
	// following others may result in bad times, but we still want
	// to check for follow toggles

	if ( msec > 200 ) {
		msec = 200;
	}

	if ( pmove_fixed.integer || client->pers.pmoveFixed ) {
		ucmd->serverTime = ((ucmd->serverTime + pmove_msec.integer-1) / pmove_msec.integer) * pmove_msec.integer;
	}

	if( !(ucmd->flags & 0x01) || ucmd->forwardmove || ucmd->rightmove || ucmd->upmove || ucmd->wbuttons || ucmd->doubleTap ) {
		ent->r.svFlags &= ~(SVF_SELF_PORTAL_EXCLUSIVE|SVF_SELF_PORTAL);
	}

	if (level.match_pause != PAUSE_NONE) 
	{
		ucmd->buttons = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		ucmd->wbuttons = 0;
		ucmd->doubleTap = 0;

		// freeze player (RELOAD_FAILED still allowed to move/look)
		if(level.match_pause != PAUSE_NONE) {
			client->ps.pm_type = PM_FREEZE;
		}
	} else if ( client->noclip ) {
		client->ps.pm_type = PM_NOCLIP;
	} else if ( client->ps.stats[STAT_HEALTH] <= 0 ) {
		client->ps.pm_type = PM_DEAD;
	} else {
		client->ps.pm_type = PM_NORMAL;
	}

#ifdef __RW__
	client->ps.pm_flags2 = 0;
#else
	client->ps.aiState = AISTATE_COMBAT;
#endif

	client->ps.gravity = g_gravity.value;

	// set speed
	client->ps.speed = g_speed.value;

	if( client->speedScale )				// Goalitem speed scale
		client->ps.speed *= (client->speedScale * 0.01);

	// set up for pmove
	oldEventSequence = client->ps.eventSequence;

	client->currentAimSpreadScale = (float)client->ps.aimSpreadScale/255.0;

	memset (&pm, 0, sizeof(pm));

	pm.ps = &client->ps;
	pm.ps->pm_type = PM_NORMAL;
	pm.ps->runSpeedScale = 0.8f;
	VectorCopy(ent->r.mins, pm.mins);
	VectorCopy(ent->r.maxs, pm.maxs);
	pm.isNPC = qtrue;
	pm.entityNum = ent->s.number;
//	pm.ps->clientNum = ent->s.number;
//	ent->s.clientNum = ent->s.number;
	pm.pmext = &client->pmext;
	
	if (ent->playerType < 0)
		ent->playerType = PC_SOLDIER;

	pm.character = client->pers.character = ent->character = BG_GetCharacter( ent->s.teamNum, ent->playerType );;
	ent->s.weapon = ucmd->weapon = ent->NPC_client->ps.weapon = ent->weaponchoice;
	pm.cmd = *ucmd;
	pm.oldcmd = client->pers.oldcmd;

	// MrE: always use capsule for AI and player
	pm.trace = trap_TraceCapsule;
	if ( pm.ps->pm_type == PM_DEAD ) {
		pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;
		// DHM-Nerve added:: EF_DEAD is checked for in Pmove functions, but wasn't being set
		//              until after Pmove
		pm.ps->eFlags |= EF_DEAD;
		// dhm-Nerve end
	} else if( pm.ps->pm_type == PM_SPECTATOR ) {
		pm.trace = trap_TraceCapsuleNoEnts;
	} else {
		pm.tracemask = MASK_PLAYERSOLID;
	}
	//DHM - Nerve :: We've gone back to using normal bbox traces
	//pm.trace = trap_Trace;
	pm.pointcontents = trap_PointContents;
//	pm.getentitystate = G_NPCGetEntityState;
#ifdef __DEBUGGING__
	pm.debugLevel = g_debugMove.integer;
#endif //__DEBUGGING__
	pm.noFootsteps = qfalse;

	pm.pmove_fixed = pmove_fixed.integer | client->pers.pmoveFixed;
	pm.pmove_msec = pmove_msec.integer;

	pm.noWeapClips = qfalse;

	VectorCopy( client->ps.origin, client->oldOrigin );

	// NERVE - SMF
	pm.gametype = g_gametype.integer;
	pm.ltChargeTime = level.lieutenantChargeTime[ent->s.teamNum-1];
	pm.soldierChargeTime = level.soldierChargeTime[ent->s.teamNum-1];
	pm.engineerChargeTime = level.engineerChargeTime[ent->s.teamNum-1];
	pm.medicChargeTime = level.medicChargeTime[ent->s.teamNum-1];
	// -NERVE - SMF

	pm.skill = client->sess.skill;

	client->pmext.airleft = ent->NPC_client->airOutTime - level.time;

	pm.covertopsChargeTime = level.covertopsChargeTime[client->sess.sessionTeam-1];

	pm.leadership = qfalse;

	// Gordon: bit hacky, stop the slight lag from client -> server even on locahost, switching back to the weapon you were holding
	//			and then back to what weapon you should have, became VERY noticible for the kar98/carbine + gpg40, esp now i've added the
	//			animation locking
	if( level.time - client->pers.lastSpawnTime < 1000 ) {
		pm.cmd.weapon = client->ps.weapon;
	}

	monsterslick = Pmove( &pm );

	// Gordon: thx to bani for this
	// ikkyo - fix leaning players bug
	VectorCopy( client->ps.velocity, ent->s.pos.trDelta );
	SnapVector( ent->s.pos.trDelta );
	// end

	// server cursor hints
	// TAT 1/10/2003 - bots don't need to check for cursor hints

	ent->s.animMovetype = 0;

	// DHM - Nerve :: Set animMovetype to 1 if ducking
	if ( ent->NPC_client->ps.pm_flags & PMF_DUCKED)
		ent->s.animMovetype |= 1;

	// save results of pmove
	if ( ent->NPC_client->ps.eventSequence != oldEventSequence ) {
		ent->eventTime = level.time;
		ent->r.eventTime = level.time;
	}
/*
	// UQ1: Add AI states for alert and query anims...
	if (ent->NPC_client->ps.aiState == AISTATE_RELAXED && ent->enemy)
	{// Switch to query state!
		ent->NPC_client->ps.aiState = AISTATE_QUERY;
		ent->npc_query_timer = level.time + 2000;
	}
	else if (ent->NPC_client->ps.aiState == AISTATE_QUERY && ent->npc_query_timer < level.time && ent->enemy)
	{// Switch to alert state!
		ent->NPC_client->ps.aiState = AISTATE_ALERT;
		ent->npc_alert_timer = level.time + 1000;
	}
	else if (ent->NPC_client->ps.aiState == AISTATE_ALERT && ent->npc_alert_timer < level.time && ent->enemy)
	{// Switch to combat state!
		ent->NPC_client->ps.aiState = AISTATE_COMBAT;
		ent->npc_query_timer = 0;
		ent->npc_alert_timer = 0;
	}
	else if (ent->NPC_client->ps.aiState == AISTATE_COMBAT && ent->enemy)
	{// Stay on combat state!
		ent->NPC_client->ps.aiState = AISTATE_COMBAT;
		ent->npc_query_timer = 0;
		ent->npc_alert_timer = 0;
	}
	else
	{// Init relaxed state...
		ent->NPC_client->ps.aiState = AISTATE_RELAXED;
		ent->npc_query_timer = 0;
		ent->npc_alert_timer = 0;
	}

	switch (ent->NPC_client->ps.aiState)
	{
	case AISTATE_QUERY:
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_QUERY, qtrue, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_RELAXING, qfalse, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_ALERT, qfalse, qfalse );
		break;
	case AISTATE_ALERT:
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_QUERY, qfalse, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_RELAXING, qfalse, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_ALERT, qtrue, qfalse );
		break;
	case AISTATE_COMBAT:
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_QUERY, qfalse, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_RELAXING, qfalse, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_ALERT, qfalse, qfalse );
		break;
	case AISTATE_RELAXED:
	default:
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_QUERY, qfalse, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_RELAXING, qtrue, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_ALERT, qfalse, qfalse );
		break;
	}
*/
	if (g_smoothClients.integer) {
		BG_NPCStateToEntityStateExtraPolate( &ent->NPC_client->ps, &ent->s, ent->NPC_client->ps.commandTime, qtrue );
	}
	else {
		BG_NPCStateToEntityState( &ent->NPC_client->ps, &ent->s, qtrue );
	}

	if ( !( ent->NPC_client->ps.eFlags & EF_FIRING ) ) {
		client->fireHeld = qfalse;		// for grapple
	}

	// use the snapped origin for linking so it matches client predicted versions
	VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );

	VectorCopy (pm.mins, ent->r.mins);
	VectorCopy (pm.maxs, ent->r.maxs);

	ent->waterlevel = pm.waterlevel;
	ent->watertype = pm.watertype;

	// link entity now, after any personal teleporters have been used
	trap_LinkEntity (ent);
	if ( !ent->NPC_client->noclip ) {
		G_TouchTriggers( ent );
	}

	// NOTE: now copy the exact origin over otherwise clients can be snapped into solid
	VectorCopy( ent->NPC_client->ps.origin, ent->r.currentOrigin );

	// store the client's current position for antilag traces
	G_StoreNPCPosition( ent );

	// touch other objects
	ClientImpacts( ent, &pm );

	// save results of triggers and client events
	if (ent->NPC_client->ps.eventSequence != oldEventSequence) {
		ent->eventTime = level.time;
	}

	// swap and latch button actions
	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons = client->buttons & ~client->oldbuttons;
//	client->latched_buttons |= client->buttons & ~client->oldbuttons;	// FIXME:? (SA) MP method (causes problems for us.  activate 'sticks')

	//----(SA)	added
	client->oldwbuttons = client->wbuttons;
	client->wbuttons = ucmd->wbuttons;
	client->latched_wbuttons = client->wbuttons & ~client->oldwbuttons;
//	client->latched_wbuttons |= client->wbuttons & ~client->oldwbuttons;	// FIXME:? (SA) MP method

	// Rafael - Activate
	// Ridah, made it a latched event (occurs on keydown only)
	if( client->latched_buttons & BUTTON_ACTIVATE ) {
		Cmd_Activate_f( ent );
	}

	ent->s.dl_intensity = ent->health;

#ifdef __NPC_NAMES__
	if (ent->bot_special_action_thinktime < level.time && ent->bot_special_action_thinktime > level.time - 1000)
	{
		trap_SendServerCommand( -1, va("npc_name %i %s", ent->s.number, ent->NPC_client->pers.netname ));
		ent->bot_special_action_thinktime = level.time + 30000; // Only send once every 30 seconds... (in case of any new clients)
	}
#endif //__NPC_NAMES__
}

/*
==================
NPCThink

A new command has arrived from the client
==================
*/
void NPCThink( int entityNum ) {
	gentity_t *ent;

	ent = g_entities + entityNum;

	ent->NPC_client->pers.oldcmd = ent->NPC_client->pers.cmd;

	// mark the time we got info, so we can display the
	// phone jack if they don't get any for a while
//	ent->NPC_client->lastCmdTime = level.time;

#ifdef ALLOW_GSYNC
	if ( !g_synchronousClients.integer )
#endif // ALLOW_GSYNC
	{
		NPCThink_real( ent );
	}
}

/*
==================
LookAtEnemy
==================
*/
void LookAtEnemy(gentity_t *NPC) 
{
	vec3_t		dir, angles, flat_angles;
	usercmd_t	*ucmd = &NPC->NPC_client->pers.cmd;
	vec3_t		enemy_org;

	VectorCopy(NPC->enemy->r.currentOrigin, enemy_org);
	enemy_org[2]+=8;

#ifdef __NPC__
		if ((NPC->enemy && NPC->enemy->client)
			&& ((NPC->enemy->client->ps.pm_flags & PMF_DUCKED) || (NPC->enemy->client->ps.eFlags & EF_CROUCHING)))
				enemy_org[2] -= 10; // Aim a little lower if they are crouching...
		else if ((NPC->enemy && NPC->enemy->NPC_client)
			&& ((NPC->enemy->NPC_client->ps.pm_flags & PMF_DUCKED) || (NPC->enemy->NPC_client->ps.eFlags & EF_CROUCHING)))
#else //!__NPC__
		if ((NPC->enemy->client->ps.pm_flags & PMF_DUCKED) || (NPC->enemy->client->ps.eFlags & EF_CROUCHING))
#endif //__NPC__
			enemy_org[2] -= 10; // Aim a little lower if they are crouching...

#ifdef __NPC__
		if ((NPC->enemy && NPC->enemy->client)
			&& (NPC->enemy->client->ps.eFlags & EF_PRONE))
			enemy_org[2] -= 20; // Aim even lower if they are prone!
		else if ((NPC->enemy && NPC->enemy->NPC_client)
			&& (NPC->enemy->NPC_client->ps.eFlags & EF_PRONE))
#else //!__NPC__
		if (NPC->enemy->client->ps.eFlags & EF_PRONE)
#endif //__NPC__
			enemy_org[2] -= 20; // Aim even lower if they are prone!

	VectorSubtract (enemy_org, NPC->r.currentOrigin, dir);

	if ( NPC->s.weapon == WP_GRENADE_LAUNCHER || NPC->s.weapon == WP_GRENADE_PINEAPPLE )
	{
		dir[2] += VectorDistance( NPC->r.currentOrigin, NPC->enemy->r.currentOrigin ) / 10;
	}

	if ( NPC->s.weapon == WP_GPG40 || NPC->s.weapon == WP_M7 )
	{
		dir[2] += VectorDistance( NPC->r.currentOrigin, NPC->enemy->r.currentOrigin ) / 10;
	}

	vectoangles(dir, angles);

	VectorCopy(angles, flat_angles);
	
/*	if (Q_TrueRand(0,1) == 0)
		flat_angles[YAW] += Q_TrueRand(0, 5-NPC->skillchoice);
	else
		flat_angles[YAW] -= Q_TrueRand(0, 5-NPC->skillchoice);

	if (Q_TrueRand(0,1) == 0)
		flat_angles[PITCH] += Q_TrueRand(0, 5-NPC->skillchoice);
	else
		flat_angles[PITCH] -= Q_TrueRand(0, 5-NPC->skillchoice);*/

	if (NPC->skillchoice == 0)
    {
        // Can't hit a good strafing player
        flat_angles[0] += (random()-0.5) * 20;
        flat_angles[1] += (random()-0.5) * 20;
    }
    else if (NPC->skillchoice == 1)
    {
        // Will get ya every now and then
        flat_angles[0] += (random()-0.5) * 10;
        flat_angles[1] += (random()-0.5) * 10;
    }
    else if (NPC->skillchoice == 2)
    {
        // Good competetion for some of the best players
        flat_angles[0] += (random()-0.5) * 5;
        flat_angles[1] += (random()-0.5) * 5;
    }
    else if (NPC->skillchoice >= 3)
    {
        // Never misses, unloads on ya!
        flat_angles[0] = flat_angles[0];
        flat_angles[1] = flat_angles[1];
    }

	if ( NPC->s.weapon == WP_GRENADE_LAUNCHER || NPC->s.weapon == WP_GRENADE_PINEAPPLE )
	{
		flat_angles[2] += VectorDistance( NPC->r.currentOrigin, NPC->enemy->r.currentOrigin ) / 10;
		flat_angles[ROLL] = 0;
		VectorCopy( flat_angles, NPC->s.angles );
		ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
		ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
		ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
		return;
	}

	if ( NPC->s.weapon == WP_GPG40 || NPC->s.weapon == WP_M7 )
	{
		flat_angles[2] += VectorDistance( NPC->r.currentOrigin, NPC->enemy->r.currentOrigin ) / 10;
		flat_angles[ROLL] = 0;
		VectorCopy( flat_angles, NPC->s.angles );
		ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
		ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
		ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
		return;
	}

	flat_angles[ROLL] = 0;

	VectorCopy(angles, NPC->bot_ideal_view_angles);
	NPCSetViewAngles(NPC, 100);
//	VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
	ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
	ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
	ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
}

extern int Count_NPC_Players ( void );

int mounts_added = 0;

int mg42_count ( void )
{
	int entnum;
	int count = 0;

	for (entnum = MAX_CLIENTS; entnum < MAX_GENTITIES; entnum++)
	{
		gentity_t *ent = &g_entities[entnum];

		if (!ent)
			continue;

		if (ent->s.eType != ET_MG42_BARREL)
			continue;

		if (ent->NPC_Class != CLASS_HUMANOID)
			continue;

		count++;
	}

	return count;
}

qboolean mg42_scan ( int node )
{
	int entnum;

	for (entnum = MAX_CLIENTS; entnum < MAX_GENTITIES; entnum++)
	{
		gentity_t *ent = &g_entities[entnum];

		if (!ent)
			continue;

		if (ent->s.eType != ET_MG42_BARREL)
			continue;

		if (VectorDistance(nodes[node].origin, ent->s.origin) < 128)
			return qtrue;
	}

	return qfalse;
}

gentity_t *NPC_FindMG42 ( gentity_t *NPC )
{
	gentity_t *mg42 = NULL;
	gentity_t *best = NULL;
	int entity = 0;
	float best_dist = 999.9f;

	for (entity = MAX_CLIENTS; entity < MAX_GENTITIES; entity++)
	{
		mg42 = &g_entities[entity];

		if (!mg42)
			continue;

		if (mg42->s.eType != ET_MG42_BARREL)
			continue;

		if (VectorDistance(mg42->r.currentOrigin, NPC->r.currentOrigin) > best_dist)
			continue;

		best_dist = VectorDistance(mg42->r.currentOrigin, NPC->r.currentOrigin);
		best = mg42;
	}

	//G_Printf("NPC %i is mounted on entity %i!\n", NPC->s.number, best->s.number);

	return best;
}

qboolean above_average_node_height ( int nodeNum )
{// ... From a random sample ...
	int node;
	int test;
	int bads_found = 0;

	for (test = 0; test < 64; test++)
	{// 16 tests... (quick)...
		node = Q_TrueRand(0, number_of_nodes);

		if (nodes[node].origin[2] >= nodes[nodeNum].origin[2])
		{
			bads_found++;
		}
	}

	if (bads_found > 8/*18*/)
		return qfalse;

	return qtrue;
}

qboolean spawnpoint_close_to_bad_object( int nodeNum )
{
	int entnum;

	for (entnum = 0; entnum < MAX_GENTITIES; entnum++)
	{
		gentity_t *ent = &g_entities[entnum];

		if (!ent)
			continue;

		if ( ent->s.eType == ET_MOVER )
		{
			if (VectorDistance(ent->s.origin, nodes[nodeNum].origin) < 340)
				return qtrue;
		}

		if ( ent->s.eType == ET_NPC )
		{
			if (VectorDistance(ent->s.origin, nodes[nodeNum].origin) < 340)
				return qtrue;
		}

		if ( ent->s.eType == ET_PLAYER )
		{
			if (VectorDistance(ent->r.currentOrigin, nodes[nodeNum].origin) < 340)
				return qtrue;
		}
		
		if ( ent->classname == "team_CTF_blueplayer" 
			|| ent->classname == "team_CTF_bluespawn"
			|| ent->classname == "info_player_start"
			|| ent->classname == "info_player_checkpoint"
			|| ent->classname == "info_player_deathmatch"
			|| ent->classname == "info_player_start"
			)
		{
			if (VectorDistance(ent->s.origin, nodes[nodeNum].origin) < 1024)
				return qtrue;
		}
	}

	return qfalse;
}

void NPC_DropHealth(gentity_t *ent)
{
	vec3_t launchvel, launchspot;
	int packs = 0;
	int i;

	if(g_realism.integer)
		return;

	packs = Q_TrueRand(1,5);
	
	for(i=0; i<packs; i++) {
		launchvel[0] = crandom();
		launchvel[1] = crandom();
		launchvel[2] = 0;
		VectorScale(launchvel, 100, launchvel);
		launchvel[2] = (100 + (g_tossDistance.integer * 10));
		VectorCopy(ent->r.currentOrigin, launchspot);
		
		Weapon_Medic_Ext(ent,
			launchspot,
			launchspot,
			launchvel);
	}
}

void NPC_DropAmmo(gentity_t *ent)
{
	vec3_t launchvel, launchspot;
	int packs = 0;
	int i;

	packs = Q_TrueRand(1,5);

	for(i=0; i<packs; i++) {
		launchvel[0] = crandom();
		launchvel[1] = crandom();
		launchvel[2] = 0;

		VectorScale(launchvel, 100, launchvel);
		launchvel[2] = (100 + (g_tossDistance.integer * 10));
		VectorCopy(ent->r.currentOrigin, launchspot);
		
		Weapon_MagicAmmo_Ext(ent,
			launchspot,
			launchspot,
			launchvel);
	}
}

void NPC_AI_Inform_Team_Of_Enemy ( int team, gentity_t *self, gentity_t *enemy )
{
	int i = MAX_CLIENTS;

	for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++)
	{
		gentity_t *ent = &g_entities[i];

		if (!ent)
			continue;

		if (ent->s.eType != ET_NPC)
			continue;

		if (ent->s.teamNum != team)
			continue;

		if (VectorDistance(ent->r.currentOrigin, self->r.currentOrigin) > 1000)
			continue;

		if (ent->enemy && ent->bot_enemy_visible_time >= level.time)
			continue; // Already got an enemy of their own!

		// OK. Found a friendly with same team as me, within range of the enemy...
		ent->enemy = enemy;

		if (Q_TrueRand(0,10) > 2)
			ent->bot_last_visible_enemy = enemy; // Skip talking if alerted...
	}
}

//===========================================================================
// Routine      : NPC_die
// Description  : When an NPC dies, use this instead of the player_die function... We have no client info...

extern void G_Obituary(int mod, int target, int attacker);
extern int GetPainLocation( gentity_t *ent, vec3_t point );

void npc_pain(gentity_t *ent, gentity_t *attacker, int damage, vec3_t point)
{
	#define		PAIN_THRESHOLD		25
	#define		STUNNED_THRESHOLD	30
//	float		dist;
	qboolean	forceStun = qfalse;
	float		painThreshold, stunnedThreshold;
	gentity_t	*self = ent;

	painThreshold = PAIN_THRESHOLD * 1;//cs->attributes[PAIN_THRESHOLD_SCALE];
	stunnedThreshold = STUNNED_THRESHOLD * 1;//cs->attributes[PAIN_THRESHOLD_SCALE];

	if (!ent->NPC_client)
		return;

	if (ent->health <= 0/*GIB_HEALTH + 1*/)
		return;

	if (!ent->enemy || ent->bot_enemy_visible_time <= level.time)
	{// React and inform friendlies...
		ent->enemy = attacker;
		NPC_AI_Inform_Team_Of_Enemy( ent->s.teamNum, ent, ent->enemy );

		if (Q_TrueRand(0,10) > 2)
			ent->bot_last_visible_enemy = attacker; // Skip talking if alerted...
	}

	// if they are already playing another animation, we might get confused and cut it off, so don't play a pain
	if ( ent->NPC_client->ps.torsoTimer || ent->NPC_client->ps.legsTimer ) {
		return;
	}

	// if we are waiting for our weapon to fire (throwing a grenade)
	if (ent->NPC_client->ps.weaponDelay) {
		return;
	}

	if (attacker->s.weapon == WP_FLAMETHROWER) {	// flames should be recognized more often, since they stay onfire until they're dead anyway
		painThreshold = 1;
		stunnedThreshold = 99999;	// dont be stunned
	}

	if (!Q_stricmp(attacker->classname, "props_statue")) {
		damage = 99999;	// try and force a stun
		forceStun = qtrue;
	}

	/*if (attacker->s.weapon == WP_FLAMETHROWER) {
		damage *= 2;
		//if (cs->attributes[PAIN_THRESHOLD_SCALE] <= 1.0) {
		//	damage = 99999;
		//}
	}

	// now check the damageQuota to see if we should play a pain animation
	// first reduce the current damageQuota with time
	if (cs->damageQuotaTime && cs->damageQuota > 0) {
		cs->damageQuota -= (int)((1.0 + (bot_skill.value/5)) * ((float)(level.time - cs->damageQuotaTime)/1000) * (7.5 + cs->attributes[ATTACK_SKILL]*10.0));
		if (cs->damageQuota < 0)
			cs->damageQuota = 0;
	}

	// if it's been a long time since our last pain, scale it up
	/*if (cs->painSoundTime < level.time - 1000) {
		float scale;
		scale = (float)(level.time - cs->painSoundTime - 1000) / 1000.0;
		if (scale > 4.0)
			scale = 4.0;
		damage = (int)((float)damage * (1.0 + (scale * (1.0 - 0.5*bot_skill.value/5))));
	}

	// adjust the new damage with distance, if they are really close, scale it down, to make it
	// harder to get through the game by continually rushing the enemies
	if ((attacker->s.weapon != WP_FLAMETHROWER) && ((dist = VectorDistance( ent->r.currentOrigin, attacker->r.currentAngles )) < 384)) {
		damage -= (int)((float)damage * (1.0 - (dist/384.0)) * (0.5 + 0.5*bot_skill.value/GSKILL_MAX));
	}

	// add the new damage
	cs->damageQuota += damage;
	cs->damageQuotaTime = level.time;

	if (forceStun) {
		damage = 99999;	// try and force a stun
		cs->damageQuota = painThreshold+1;
	}*/

	// if it's over the threshold, play a pain

	// don't do this if crouching, or we might clip through the world

	if (1/*cs->damageQuota > painThreshold*/) {
		int delay;

		// stunned?
		if (damage > stunnedThreshold && (forceStun || (rand()%2))) {	// stunned
			BG_UpdateConditionValue( ent->s.number, ANIM_COND_STUNNED, qtrue, qfalse );
		}
		// enemy weapon
		if (attacker->client) {
			BG_UpdateConditionValue( ent->s.number, ANIM_COND_ENEMY_WEAPON, attacker->s.weapon, qtrue );
		}
		if (point) {
			// location
			BG_UpdateConditionValue( ent->s.number, ANIM_COND_IMPACT_POINT, GetPainLocation(ent, point), qtrue );
		} else {
			BG_UpdateConditionValue( ent->s.number, ANIM_COND_IMPACT_POINT, 0, qfalse );
		}

		if (self->s.eFlags & EF_PRONE)
			BG_UpdateConditionValue( self->s.number, ANIM_COND_PRONING, qtrue, qfalse );
		else
			BG_UpdateConditionValue( self->s.number, ANIM_COND_PRONING, qfalse, qfalse );

		if (self->s.eFlags & EF_CROUCHING)
			BG_UpdateConditionValue( self->s.number, ANIM_COND_CROUCHING, qtrue, qfalse );
		else
			BG_UpdateConditionValue( self->s.number, ANIM_COND_CROUCHING, qfalse, qfalse );

		// pause while we play a pain
		delay = 500;//BG_AnimScriptEvent( &ent->client->ps, ANIM_ET_PAIN, qfalse, qtrue );

		ent->NPC_client->ps.pm_time = BG_AnimScriptEvent( &ent->NPC_client->ps, ent->NPC_client->pers.character->animModelInfo, ANIM_ET_PAIN, qfalse, qtrue );

		// turn off temporary conditions
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_STUNNED, 0, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_ENEMY_WEAPON, 0, qfalse );
		BG_UpdateConditionValue( ent->s.number, ANIM_COND_IMPACT_POINT, 0, qfalse );

		/*if (delay >= 0) {
			// setup game stuff to handle the character movements, etc
			cs->pauseTime = level.time + delay + 250;
			cs->lockViewAnglesTime = cs->pauseTime;
			// make sure we stop crouching
			cs->attackcrouch_time = 0;
			// don't fire while in pain?
			cs->triggerReleaseTime = cs->pauseTime;
			// stay crouching if we were before the pain
			if (cs->bs->cur_ps.viewheight == cs->bs->cur_ps.crouchViewHeight) {
				cs->attackcrouch_time = level.time + (float)(cs->pauseTime - level.time) + 500;
			}
		}*/

		// if we didnt just play a scripted sound, then play one of the default sounds
		/*if (cs->lastScriptSound < level.time) {
			G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( aiDefaults[ent->aiCharacter].soundScripts[PAINSOUNDSCRIPT] ) );
		}

		// reset the quota
		cs->damageQuota = 0;
		cs->damageQuotaTime = 0;
		//
		cs->painSoundTime = cs->pauseTime + (int)(1000*(bot_skill.value/GSKILL_MAX));		// add a bit more of a buffer before the next one
		*/

//		ent->NPC_client->ps.pm_time = BG_AnimScriptEvent( &ent->NPC_client->ps, ent->NPC_client->pers.character->animModelInfo, ANIM_ET_PAIN, qfalse, qtrue );
	}
}

/*
=============
BodyUnlink
  
Called by BodySink
=============
*/
void NPC_BodyUnlink( gentity_t *ent ) {
	trap_UnlinkEntity( ent );
	ent->physicsObject = qfalse;
	ent->think = G_FreeEntity;
	ent->nextthink = level.time + 100;
}
                
/*
=============
BodySink

After sitting around for five seconds, fall into the ground and dissapear 
=============
*/ 
void NPC_BodySink( gentity_t *ent ) {
	/*ent->physicsObject = qfalse;
	// tjw: BODY_TIME is how long they last all together, not just sink anim
	//ent->nextthink = level.time + BODY_TIME(BODY_TEAM(ent))+1500;
	ent->nextthink = level.time + 4000;
    ent->think = NPC_BodyUnlink;
    ent->s.pos.trType = TR_LINEAR;
    ent->s.pos.trTime = level.time;
    VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
	VectorSet( ent->s.pos.trDelta, 0, 0, -8 );
	VectorSet( ent->NPC_client->ps.velocity, 0, 0, -8 ); // UQ1: No longer ET_CORPSE, need to set velocity as it overwrites trDelta...
*/
	ent->s.pos.trType = TR_LINEAR;//TR_STATIONARY;
	ent->physicsObject = qfalse;

	if (ent->bot_blind_routing_time <= level.time)
	{
		ent->bot_blind_routing_time = level.time + 4000;
		ent->bot_dont_give_supplies_end = ent->r.currentOrigin[2] - 64;
	}
	
	ent->s.origin[2] -= 1;
	ent->r.currentOrigin[2] -= 1;
	ent->NPC_client->ps.origin[2] -= 1;
	VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );

	//NPCThink_real(ent);

	ent->nextthink = level.time + 100;

	if (ent->r.currentOrigin[2] <= ent->bot_dont_give_supplies_end)
	{
		ent->think = NPC_BodyUnlink;
	}
}

void NPC_Death_Animate (gentity_t *self)
{
	int i = 0;

	NPCThink_real(self);

	if (self->NPC_client->ps.pm_time > 0)
	{
		self->nextthink = level.time + 100;
		return; // While animating the death...
	}

	self->NPC_client->ps.eFlags |= EF_DEAD;
	self->NPC_client->ps.pm_type = PM_DEAD;

	self->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	// DHM - Nerve :: allow bullets to pass through bbox
	// Gordon: need something to allow the hint for covert ops
	self->r.contents = CONTENTS_CORPSE;

	self->timestamp = level.time;
	self->physicsObject = qtrue;
	self->physicsBounce = 0;		// don't bounce
	if (self->s.groundEntityNum == ENTITYNUM_NONE) {
		self->s.pos.trType = TR_GRAVITY;
		self->s.pos.trTime = level.time;
		//VectorCopy(ent->client->ps.velocity, body->s.pos.trDelta);
		VectorSet(self->s.pos.trDelta, 0, 0, 0);
	} else {
		self->s.pos.trType = TR_STATIONARY;
	}

	self->s.event = 0;

	// DHM - Clear out event system
	for (i=0; i<MAX_EVENTS; i++)
		self->s.events[i] = 0;

	self->s.eventSequence = 0;

	// DHM - Nerve
	// change the animation to the last-frame only, so the sequence
	// doesn't repeat anew for the body
	switch (self->s.legsAnim & ~ANIM_TOGGLEBIT)
	{
	case BOTH_DEATH1:
	case BOTH_DEAD1:
	default:
		self->s.torsoAnim = self->s.legsAnim = BOTH_DEAD1;
		break;
	case BOTH_DEATH2:
	case BOTH_DEAD2:
		self->s.torsoAnim = self->s.legsAnim = BOTH_DEAD2;
		break;
	case BOTH_DEATH3:
	case BOTH_DEAD3:
		self->s.torsoAnim = self->s.legsAnim = BOTH_DEAD3;
		break;
	}

	//NPC_BodySink(self);
	self->think = NPC_BodySink;
	self->nextthink = level.time + 100;
}

void NPC_die(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath) {
	int			/*contents = 0,*/ killer = ENTITYNUM_WORLD;
	char		*killerName = "<world>";
//	qboolean	nogib = qtrue;
	gitem_t		*item = NULL;
	gentity_t	*ent;
	qboolean	killedintank = qfalse;
	weapon_t	weap = BG_WeaponForMOD(meansOfDeath);

#ifdef __RW__
	self->s.curweapAnim = WEAP_IDLE1;
#endif

	if (attacker != self && attacker->s.eType == ET_NPC)
	{// If the attacker was an npc then set the MOD value now...
		meansOfDeath = GetAmmoTableData(attacker->s.weapon)->mod;
	}

	if (attacker == self) 
	{
	} else if (OnSameTeam(self, attacker)) {
		G_LogTeamKill(	attacker,	weap);
	} else {
		G_LogDeath(self,		weap);
		G_LogKill(	attacker,	weap);

		if (attacker->client)
			AddScore(attacker, WOLF_FRAG_BONUS);

		if (inflictor->client)
			AddScore(inflictor, WOLF_FRAG_BONUS);

		if (g_gamestate.integer == GS_PLAYING) {
			if (attacker->client) {
				attacker->client->combatState |= (1<<COMBATSTATE_KILLEDPLAYER);
			}
		}
	}

	// RF, record this death in AAS system so that bots avoid areas which have high death rates
	if (!OnSameTeam(self, attacker)) 
	{
//#ifndef __RW__
//		BotRecordTeamDeath(self->s.number);
//#endif
		self->isProp = qfalse;	// were we teamkilled or not?
	} else {
		self->isProp = qtrue;
	}	

	if (self->tankLink) {
		G_LeaveTank(self, qfalse);

		killedintank = qtrue;
	}

	if (g_gamestate.integer == GS_INTERMISSION) {
		return;
	}

	// OSP - death stats handled out-of-band of G_Damage for external calls
	//G_addStats(self, attacker, damage, meansOfDeath);
	// OSP

	G_AddEvent(self, EV_STOPSTREAMINGSOUND, 0);

	if (attacker) {
		killer = attacker->s.number;
#ifdef __NPC__
		if (attacker->s.eType == ET_NPC)
			killerName = (attacker->NPC_client) ? attacker->NPC_client->pers.netname : "<non-client>";
		else
#endif //__NPC__
		killerName = (attacker->client) ? attacker->client->pers.netname : "<non-client>";
	}

#ifdef __NPC__
	if ((attacker == 0 || killer < 0) || !(g_entities[killer].s.eType == ET_NPC) ) {
#else //!__NPC__
	if (attacker == 0 || killer < 0 || killer >= MAX_CLIENTS) {
#endif //__NPC__
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

#ifdef __NPC__
	if (attacker != self && attacker->s.eType == ET_NPC)
	{// If the attacker was an npc then set the MOD value now...
		meansOfDeath = GetAmmoTableData(attacker->s.weapon)->mod;
		killer = attacker->s.number;
	}
	else if (attacker != self && attacker->s.eType == ET_PLAYER)
	{
		killer = attacker->s.number;
	}
#endif //__NPC__

	// broadcast the death event to everyone
	ent = G_TempEntity(self->r.currentOrigin, EV_OBITUARY);
	ent->s.eventParm = meansOfDeath;
	ent->s.otherEntityNum = self->s.number;
	ent->s.otherEntityNum2 = killer;
	ent->s.teamNum = self->s.teamNum;
	ent->s.density = ET_NPC;
	ent->s.effect1Time = attacker->s.eType; // Send attacker type...
	ent->s.effect2Time = attacker->s.teamNum; // Send attacker team...
	ent->r.svFlags = SVF_BROADCAST;	// send to everyone

	self->enemy = attacker;

	if (item)
	{
		vec3_t		launchvel	= { 0, 0, 0 };
		gentity_t*	flag		= LaunchItem(item, self->r.currentOrigin, launchvel, self->s.number);

		flag->s.modelindex2 = self->s.otherEntityNum2;// JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
		flag->message = self->message;	// DHM - Nerve :: also restore item name
		// Clear out player's temp copies
		self->s.otherEntityNum2 = 0;
		self->message = NULL;
	}

	// send a fancy "MEDIC!" scream.  Sissies, ain' they?
/*	if (self->client != NULL)
	{
		if (self->health > GIB_HEALTH && meansOfDeath != MOD_SUICIDE && meansOfDeath != MOD_SWITCHTEAM)
			G_AddEvent(self, EV_MEDIC_CALL, 0);
	}*/

	self->takedamage = qtrue;	// can still be gibbed
	self->r.contents = CONTENTS_CORPSE;

	//self->s.angles[2] = 0;
	self->s.powerups = 0;
	self->s.loopSound = 0;
	
	trap_LinkEntity(self);

	// remove powerups
	memset( self->NPC_client->ps.powerups, 0, sizeof(self->NPC_client->ps.powerups) );

	//if(nogib)
	{
		// normal death
		// for the no-blood option, we need to prevent the health
		// from going to gib level
		if ( self->health <= GIB_HEALTH ) {
			self->health = GIB_HEALTH + 1;
		}

		if (1) 
		{// UQ1: Better death anims! :)
			// enemy weapon
			if (attacker->client) {
				BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_WEAPON, attacker->s.weapon, qtrue );
			}
			if (self->last_hit_point) {
				// location
				BG_UpdateConditionValue( self->s.number, ANIM_COND_IMPACT_POINT, GetPainLocation(self, self->last_hit_point), qtrue );
			} else {
				BG_UpdateConditionValue( self->s.number, ANIM_COND_IMPACT_POINT, 0, qfalse );
			}

			if (self->s.eFlags & EF_PRONE)
				BG_UpdateConditionValue( self->s.number, ANIM_COND_PRONING, qtrue, qfalse );
			else
				BG_UpdateConditionValue( self->s.number, ANIM_COND_PRONING, qfalse, qfalse );

			if (self->s.eFlags & EF_CROUCHING)
				BG_UpdateConditionValue( self->s.number, ANIM_COND_CROUCHING, qtrue, qfalse );
			else
				BG_UpdateConditionValue( self->s.number, ANIM_COND_CROUCHING, qfalse, qfalse );

			self->NPC_client->ps.pm_time = BG_AnimScriptEvent( &self->NPC_client->ps, self->NPC_client->pers.character->animModelInfo, ANIM_ET_DEATH, qfalse, qtrue );

			// turn off temporary conditions
			BG_UpdateConditionValue( self->s.number, ANIM_COND_STUNNED, 0, qfalse );
			BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_WEAPON, 0, qfalse );
			BG_UpdateConditionValue( self->s.number, ANIM_COND_IMPACT_POINT, 0, qfalse );
		}

		// record the death animation to be used later on by the corpse
		self->NPC_client->torsoDeathAnim = self->NPC_client->ps.torsoAnim;
		self->NPC_client->legsDeathAnim = self->NPC_client->ps.legsAnim;

		G_AddEvent( self, EV_DEATH1 + 1, killer );

		// the body can still be gibbed
		self->die = body_die;
	}

	G_FadeItems(self, MOD_SATCHEL);

	CalculateRanks();

	/* ================================== */
	/* BEGIN: Weapon/Pack/Tools dropping! */
	/* ================================== */

	G_DropWeapon( self, self->s.weapon );

	if (IsCoopGametype())
	{// In COOP and SP, drop extra stuff...
		if (self->playerType == PC_ENGINEER)
		{
			G_DropWeapon( self, WP_DYNAMITE );
			G_DropWeapon( self, WP_PLIERS );
		}
		else if (self->playerType == PC_MEDIC)
		{
			NPC_DropHealth(self);

			// Sometimes drop a bonus!
			//if (Q_TrueRand(1, 10) == 10)
				G_DropWeapon( self, WP_MEDIC_SYRINGE );

			//if (Q_TrueRand(1, 10) == 10)
				G_DropWeapon( self, WP_MEDIC_ADRENALINE );

			//if (Q_TrueRand(1, 10) == 10)
				G_DropWeapon( self, WP_MEDKIT );
		}
		else if (self->playerType == PC_FIELDOPS)
		{
			NPC_DropAmmo(self);

			// Sometimes drop a bonus!
			//if (Q_TrueRand(1, 10) == 10)
				G_DropWeapon( self, WP_SMOKE_MARKER );

			//if (Q_TrueRand(1, 10) == 10)
				G_DropWeapon( self, WP_AMMO );
		}
		else if (self->playerType == PC_COVERTOPS)
		{
			// Sometimes drop a bonus!
			//if (Q_TrueRand(1, 10) == 10)
				G_DropWeapon( self, WP_SMOKE_BOMB );
		}
	
		// Sometimes drop a bonus!
		if (Q_TrueRand(1, 5) == 5)
			G_DropWeapon( self, WP_GRENADE_LAUNCHER );
	}
	else
	{// Normal gametypes with ammo/medkit drop cvars set...
		if (self->playerType == PC_FIELDOPS && g_dropAmmo.integer != 0)
			NPC_DropAmmo(self);

		if (self->playerType == PC_MEDIC && g_dropHealth.integer != 0)
			NPC_DropHealth(self);
	}

	/* ================================== */
	/*  END: Weapon/Pack/Tools dropping!  */
	/* ================================== */

	self->think = NPC_Death_Animate;
	self->nextthink = level.time + 100;
}

void SetNPCWeapon ( gentity_t *NPC )
{
	// The new way to spawn bot weapons.. Look up ammo table instead.. 
	// Should allow us to add more weapons easier! Maybe we should use this for normal clients too???
	int					pc				= PC_SOLDIER;
	int					loop			= 0;
	int					primaryCount	= 0;
//	int					nadesCount		= 0;
	bg_playerclass_t*	classInfo		= NULL;

	if (IsCoopGametype())
	{// Loaded from .npc script...
		NPC->s.weapon = NPC->weaponchoice;
		return;
	}

	// Let's make sure the bots actually have a class first...
	pc = NPC->playerType;

	classInfo = BG_GetPlayerClassInfo(NPC->s.teamNum, pc);

	// And now find a main weapon we can use...
	while (classInfo->classWeapons[loop] != WP_NONE && loop < MAX_WEAPS_PER_CLASS)
	{
		primaryCount++;
		loop++;
	}

	loop = 0;

/*	// And now find a nade type we can use...
	while (classInfo->classWeapons[4][loop].weapon != WP_NONE && loop < MAX_WEAPS_PER_CLASS)
	{
		nadesCount++;
		loop++;
	}*/

	// Now select a weapon from the allowed list randomly...
	if (primaryCount > 0)
		NPC->s.weapon = classInfo->classWeapons[Q_TrueRand(0, primaryCount - 1)];

	while (NPC->s.weapon == WP_MORTAR && primaryCount > 0) // never select mortar!
		NPC->s.weapon = classInfo->classWeapons[Q_TrueRand(0, primaryCount - 1)];

	// Now select a nade from the allowed list randomly...
/*	if (nadesCount > 0)
		client->sess.playerGrenade = classInfo->classWeapons[4][Q_TrueRand(0, nadesCount - 1)].weapon;
*/
	NPC->weaponchoice = NPC->s.weapon;
}

extern gentity_t *SelectSpectatorSpawnPoint(vec3_t origin, vec3_t angles);
#ifdef __ETE__
extern gentity_t *NPCSelectPOPSpawnpoint ( gentity_t *ent );
extern int redtickets;
extern int bluetickets;
#endif //__ETE__

extern char *PickName ( int teamNum );

//===========================================================================
// Routine      : NPC_Spawn
// Description  : Spawn an NPC.
void NPC_Spawn( void )
{
	gentity_t *NPC = G_Spawn();
	gentity_t *spawnPoint = NULL;
	vec3_t spawn_origin, spawn_angles;

	NPC->s.teamNum = PickTeam(-1);

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;

	if (NPC->s.teamNum != TEAM_AXIS && NPC->s.teamNum != TEAM_ALLIES) 
	{
		spawnPoint = SelectSpectatorSpawnPoint(spawn_origin, spawn_angles);
	} 
	else 
	{// FIXME: Add parachute support...
#ifdef __ETE__
		if ( !level.intermissiontime && (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) )
		{
			spawnPoint = NPCSelectPOPSpawnpoint(NPC);

			if (!spawnPoint)
				return;

			if (NPC->s.teamNum == TEAM_AXIS)
			{// Decrease the tickets for this team...
				redtickets--;
			}
			else
			{// Decrease the tickets for this team...
				bluetickets--;
			}

			trap_SendServerCommand( -1, va("tkt %i %i", redtickets, bluetickets ));
		}
		else
#endif //__ETE__
			spawnPoint = SelectCTFSpawnPoint(NPC->s.teamNum, TEAM_ACTIVE, spawn_origin, spawn_angles, 0);
	}

	VectorCopy(spawnPoint->s.origin, NPC->s.origin);

	NPC_Setup( NPC );

	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class; // For cgame access...

	// Solids...
	NPC->clipmask	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->r.contents	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	NPC->s.pos.trType = TR_GRAVITY;

	VectorCopy(playerMins, NPC->r.mins);
	VectorCopy(playerMaxs, NPC->r.maxs);

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	//NPC->playerType = Q_TrueRand(PC_SOLDIER, NUM_PLAYER_CLASSES-1);//NPC_SOLDIER;
	NPC->playerType = PC_SOLDIER;
	//NPC->s.modelindex2 = NPC->playerType;
	NPC->s.modelindex2 = CLASS_HUMANOID;
	//NPC->s.density = NPC->playerType;
	//NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );

	SetNPCWeapon(NPC);

	//if (!NPC->NPC_client)
	{// Set up an NPC client structure...
		gclient_t *client;
		gentity_t *ent = NPC;

		NPC->NPC_client = G_Alloc(sizeof(*client));
		client = NPC->NPC_client;

		memset(client, 0, sizeof(*client));
		memset(&client->ps, 0, sizeof(client->ps));
		memset(&client->ps.weapons, 0, sizeof(client->ps.weapons));
		memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
		memset(client->ps.ammoclip, 0, sizeof(client->ps.ammoclip));
		// JPW NERVE -- zero out all ammo counts
		//memset(client->ps.Ammo, 0, sizeof(client->ps.Ammo));

		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;			// DHM - Nerve

		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;

		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;

		VectorCopy(NPC->s.origin, NPC->NPC_client->ps.origin);

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;
		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
//		client->ps.persistant[PERS_CHOOSED_WEAPON] = (client->sess.playerWeapon2 << 8) | client->sess.playerWeapon;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy(ent->r.mins, client->ps.mins);
		VectorCopy(ent->r.maxs, client->ps.maxs);
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;
		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;	//----(SA)	added
		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;
		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink(ent-g_entities);
		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		VectorCopy(ent->NPC_client->ps.origin, ent->r.currentOrigin);
		trap_LinkEntity(ent);
		// run the presend to set anything else
		//ClientEndFrame(ent);
		// clear entity state values
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=569
		//G_ResetMarkers(ent);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;

		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

/*	if (NPC_NodesLoaded())
	{
		if (Q_irand(0,10) <= 1) // Occasionally, one should also be an attacker...
			NPC->NPC_Humanoid_Type |= HUMANOID_TYPE_FODDER_ATTACKER;
		
		NPC->NPC_Humanoid_Type |= HUMANOID_TYPE_FODDER;
	}*/
	
	//G_Printf("NPC Spawned...\n");

	// Transmit name in 1 second...
	NPC->bot_special_action_thinktime = level.time + 1000;

	NPC->s.dl_intensity = NPC->health;
	NPC->NPC_client->ps.stats[STAT_HEALTH] = NPC->health;
	NPC->nextthink = level.time - 1;
	//NPC->bot_random_move_time = level.time + 2000 + Q_TrueRand(1000, 8000);
	NPCThink(NPC->s.number);

	// Send a name to the clients...
	strcpy(NPC->NPC_client->pers.netname, PickName( NPC->s.teamNum ));
	//trap_SendServerCommand( -1, va("npc_name %i %s", NPC->s.number, NPC->NPC_client->pers.netname ));
}

static qboolean NPC_LoadData_ParseError( int handle, char *format, ... )
{
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start( argptr, format );
	Q_vsnprintf( string, sizeof(string), format, argptr );
	va_end( argptr );

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine( handle, filename, &line );

	if (filename[0])
		Com_Printf( S_COLOR_RED "NPC_LoadData: ERROR - %s, line %d: %s\n", filename, line, string );
	else
		Com_Printf( S_COLOR_RED "NPC_LoadData: ERROR - %s\n", string );

	trap_PC_FreeSource( handle );

	return qfalse;
}

qboolean NPC_String_Parse(int handle, char *out, size_t size) 
{
	pc_token_t token;

	if( !trap_PC_ReadToken(handle, &token) )
		return qfalse;

	Q_strncpyz( out, token.string, size );
    return qtrue;
}

#define ACTION_NONE			0
#define ACTION_RUNANDHIDE	1
#define ACTION_GOTOENTITY	2

stringID_table_t NPC_ActionTable[] =
{
	ENUM2STRING( ACTION_NONE ),
	ENUM2STRING( ACTION_RUNANDHIDE ),
	ENUM2STRING( ACTION_GOTOENTITY ),
};

qboolean NPC_LoadData(gentity_t *NPC, const char* szFileName)
{
	pc_token_t	token;
	int			handle;
//	int			total	= 1;
	char		string[MAX_TOKEN_CHARS];

	handle = trap_PC_LoadSource( va("npcs/%s.npc", szFileName) );

	if( !handle )
		return qfalse;

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) ) 
	{
		return NPC_LoadData_ParseError( handle, "expected '{'" );
	}

	while (1)
	{
		if( !trap_PC_ReadToken( handle, &token )) 
		{
			return NPC_LoadData_ParseError( handle, "unexpected eof" );
		}

		if( !Q_stricmp( token.string, "}" ) ) 
		{
			break;
		}
		else if( !Q_stricmp( token.string, "class" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected class type" );
			}
			else
			{
				NPC->playerType = (int)GetIDForString( NPC_ClassTable, string );
#ifdef _DEBUG
				G_Printf("^4*** ^3NPC DEBUG^5: LoadNPC (^3%s^5): ^7%s^5 (^7%i^5).\n", szFileName, string, NPC->playerType);
#endif //_DEBUG
			}
		}
		else if( !Q_stricmp( token.string, "skill" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected skill level" );
			}
			else
			{
				NPC->skillchoice = atoi(string);
				//G_Printf("^4*** ^3NPC DEBUG^5: Skill: ^7%i^5.\n", NPC->skillchoice);
			}
		}
		else if( !Q_stricmp( token.string, "weapon" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected weapon choice" );
			}
			else
			{
				NPC->weaponchoice = (int)GetIDForString( WeaponTableStrings, string );
				//G_Printf("^4*** ^3NPC DEBUG^5: Weapon: ^7%s^5.\n", string);
			}
		}
		else if( !Q_stricmp( token.string, "patrol" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected patrol true/false" );
			}
			else
			{
				//G_Printf("^4*** ^3NPC DEBUG^5: Patrol: ^7%s^5.\n", string);

				if( !Q_stricmp( string, "true" ) )
					NPC->npc_patrol = qtrue;
				else
					NPC->npc_patrol = qfalse;
			}
		}
		else if( !Q_stricmp( token.string, "mounted" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected mounted true/false" );
			}
			else
			{
				//G_Printf("^4*** ^3NPC DEBUG^5: Mounted: ^7%s^5.\n", string);

				if( !Q_stricmp( string, "true" ) )
					NPC->npc_mount = qtrue;
				else
					NPC->npc_mount = qfalse;
			}
		}
		else if( !Q_stricmp( token.string, "hunter" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected hunter true/false" );
			}
			else
			{
				//G_Printf("^4*** ^3NPC DEBUG^5: Hunter: ^7%s^5.\n", string);

				if( !Q_stricmp( string, "true" ) )
					NPC->npc_hunter = qtrue;
				else
					NPC->npc_hunter = qfalse;
			}
		}
		else if( !Q_stricmp( token.string, "alertentity" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected alertentity <number>" );
			}
			else
			{
				NPC->target_ent = &g_entities[atoi(string)];
			}
		}
		else if( !Q_stricmp( token.string, "alertevent" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected alertevent type" );
			}
			else
			{
				NPC->action = (int)GetIDForString( NPC_ActionTable, string );
			}
		}
	}

	trap_PC_FreeSource( handle );

	return qtrue;
}

//===========================================================================
// Routine      : NPC_CoopSpawn
// Description  : Spawn an NPC for coop gametype.

extern void G_RegisterPlayerClasses( void );
extern qboolean characters_loaded;

void NPC_CoopSpawn( gentity_t *NPC )
{
//	trace_t tr;
//	vec3_t end;
	
	if (!IsCoopGametype())
	{
		G_FreeEntity(NPC);
		return;
	}

	if (!characters_loaded)
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	// Find the ground from our spawnpoint... (for SP_NPC_Position)
/*	VectorCopy(NPC->s.origin, end);
	end[2]-=1024;
	trap_Trace( &tr, NPC->s.origin, NULL, NULL, end, NPC->s.number, MASK_SHOT );
	VectorCopy(tr.endpos, NPC->s.origin);*/

	NPC->s.teamNum = TEAM_AXIS;//PickTeam(-1);

	if (NPC->spawnflags & TEAM_ALLIES)
		NPC->s.teamNum = TEAM_ALLIES;//PickTeam(-1);

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;

	NPC_Setup( NPC );

	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class; // For cgame access...

	// Load the NPC script for NPC->scriptName
	if (!NPC->scriptName)
		NPC->scriptName = "default";

	if (!NPC_LoadData(NPC, NPC->scriptName))
	{
		G_Printf("Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName);

		NPC->scriptName = "default";
		
		if (!NPC_LoadData(NPC, NPC->scriptName))
		{
			G_Printf("Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName);
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy(NPC->s.origin, NPC->SP_NPC_Position);

	// Solids...
	NPC->clipmask	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->r.contents	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	NPC->s.pos.trType = TR_GRAVITY;

	VectorCopy(playerMins, NPC->r.mins);
	VectorCopy(playerMaxs, NPC->r.maxs);

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	//NPC->s.modelindex2 = NPC->playerType;
	//NPC->s.modelindex2 = NPC->playerType;
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->s.density = NPC->playerType;
	NPC->s.onFireStart = NPC->playerType;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );

	SetNPCWeapon(NPC);

	//if (!NPC->NPC_client)
	{// Set up an NPC client structure...
		gclient_t *client;
		gentity_t *ent = NPC;

		NPC->NPC_client = G_Alloc(sizeof(*client));
		client = NPC->NPC_client;

		memset(client, 0, sizeof(*client));
		memset(&client->ps, 0, sizeof(client->ps));
		memset(&client->ps.weapons, 0, sizeof(client->ps.weapons));
		memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
		memset(client->ps.ammoclip, 0, sizeof(client->ps.ammoclip));
		// JPW NERVE -- zero out all ammo counts
		//memset(client->ps.Ammo, 0, sizeof(client->ps.Ammo));

		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;			// DHM - Nerve

		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;

		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;

		VectorCopy(NPC->s.origin, NPC->NPC_client->ps.origin);

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;
		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
//		client->ps.persistant[PERS_CHOOSED_WEAPON] = (client->sess.playerWeapon2 << 8) | client->sess.playerWeapon;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy(ent->r.mins, client->ps.mins);
		VectorCopy(ent->r.maxs, client->ps.maxs);
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;
		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;	//----(SA)	added
		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;
		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink(ent-g_entities);
		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		VectorCopy(ent->NPC_client->ps.origin, ent->r.currentOrigin);
		trap_LinkEntity(ent);
		// run the presend to set anything else
		//ClientEndFrame(ent);
		// clear entity state values
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=569
		//G_ResetMarkers(ent);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;

		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}
	
	//G_Printf("NPC Spawned...\n");

	// Transmit name in 1 second...
	NPC->bot_special_action_thinktime = level.time + 1000;

	NPC->s.dl_intensity = NPC->health;
	NPC->nextthink = level.time - 1;
	NPCThink(NPC->s.number);

	// Send a name to the clients...
	strcpy(NPC->NPC_client->pers.netname, PickName( NPC->s.teamNum ));
	//trap_SendServerCommand( -1, va("npc_name %i %s", NPC->s.number, NPC->NPC_client->pers.netname ));
}

void NPC_CoopSoldier( gentity_t *NPC )
{
	if (!IsCoopGametype())
	{
		G_FreeEntity(NPC);
		return;
	}

	if (!characters_loaded)
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;

	if (NPC->spawnflags & TEAM_ALLIES)
		NPC->s.teamNum = TEAM_ALLIES;

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;

	NPC_Setup( NPC );

	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class; // For cgame access...

	// Load the NPC script for NPC->scriptName
	if (!NPC->scriptName)
		NPC->scriptName = "default";

	if (!NPC_LoadData(NPC, NPC->scriptName))
	{
		G_Printf("Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName);

		NPC->scriptName = "default";
		
		if (!NPC_LoadData(NPC, NPC->scriptName))
		{
			G_Printf("Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName);
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy(NPC->s.origin, NPC->SP_NPC_Position);

	// Solids...
	NPC->clipmask	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->r.contents	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	NPC->s.pos.trType = TR_GRAVITY;

	VectorCopy(playerMins, NPC->r.mins);
	VectorCopy(playerMaxs, NPC->r.maxs);

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );

	SetNPCWeapon(NPC);

	{// Set up an NPC client structure...
		gclient_t *client;
		gentity_t *ent = NPC;

		NPC->NPC_client = G_Alloc(sizeof(*client));
		client = NPC->NPC_client;

		memset(client, 0, sizeof(*client));
		memset(&client->ps, 0, sizeof(client->ps));
		memset(&client->ps.weapons, 0, sizeof(client->ps.weapons));
		memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
		memset(client->ps.ammoclip, 0, sizeof(client->ps.ammoclip));

		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;			// DHM - Nerve

		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;

		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;

		VectorCopy(NPC->s.origin, NPC->NPC_client->ps.origin);

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;
		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy(ent->r.mins, client->ps.mins);
		VectorCopy(ent->r.maxs, client->ps.maxs);
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;
		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;	//----(SA)	added
		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;
		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink(ent-g_entities);
		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		VectorCopy(ent->NPC_client->ps.origin, ent->r.currentOrigin);
		trap_LinkEntity(ent);
		// clear entity state values
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;

		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	// Transmit name in 1 second...
	NPC->bot_special_action_thinktime = level.time + 1000;

	NPC->s.dl_intensity = NPC->health;
	NPC->nextthink = level.time - 1;
	NPCThink(NPC->s.number);

	// Send a name to the clients...
	strcpy(NPC->NPC_client->pers.netname, PickName( NPC->s.teamNum ));
	//trap_SendServerCommand( -1, va("npc_name %i %s", NPC->s.number, NPC->NPC_client->pers.netname ));
}

void NPC_CoopVenomSoldier( gentity_t *NPC )
{
	if (!IsCoopGametype())
	{
		G_FreeEntity(NPC);
		return;
	}

	if (!characters_loaded)
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;

	if (NPC->spawnflags & TEAM_ALLIES)
		NPC->s.teamNum = TEAM_ALLIES;

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;

	NPC_Setup( NPC );

	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class; // For cgame access...

	// Load the NPC script for NPC->scriptName
	if (!NPC->scriptName)
		NPC->scriptName = "mp42";

	if (!NPC_LoadData(NPC, NPC->scriptName))
	{
		G_Printf("Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName);

		NPC->scriptName = "default";
		
		if (!NPC_LoadData(NPC, NPC->scriptName))
		{
			G_Printf("Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName);
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy(NPC->s.origin, NPC->SP_NPC_Position);

	// Solids...
	NPC->clipmask	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->r.contents	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	NPC->s.pos.trType = TR_GRAVITY;

	VectorCopy(playerMins, NPC->r.mins);
	VectorCopy(playerMaxs, NPC->r.maxs);

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );

	SetNPCWeapon(NPC);

	{// Set up an NPC client structure...
		gclient_t *client;
		gentity_t *ent = NPC;

		NPC->NPC_client = G_Alloc(sizeof(*client));
		client = NPC->NPC_client;

		memset(client, 0, sizeof(*client));
		memset(&client->ps, 0, sizeof(client->ps));
		memset(&client->ps.weapons, 0, sizeof(client->ps.weapons));
		memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
		memset(client->ps.ammoclip, 0, sizeof(client->ps.ammoclip));

		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;			// DHM - Nerve

		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;

		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;

		VectorCopy(NPC->s.origin, NPC->NPC_client->ps.origin);

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;
		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy(ent->r.mins, client->ps.mins);
		VectorCopy(ent->r.maxs, client->ps.maxs);
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;
		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;	//----(SA)	added
		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;
		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink(ent-g_entities);
		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		VectorCopy(ent->NPC_client->ps.origin, ent->r.currentOrigin);
		trap_LinkEntity(ent);
		// clear entity state values
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;

		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	// Transmit name in 1 second...
	NPC->bot_special_action_thinktime = level.time + 1000;

	NPC->s.dl_intensity = NPC->health;
	NPC->nextthink = level.time - 1;
	NPCThink(NPC->s.number);

	// Send a name to the clients...
	strcpy(NPC->NPC_client->pers.netname, PickName( NPC->s.teamNum ));
	//trap_SendServerCommand( -1, va("npc_name %i %s", NPC->s.number, NPC->NPC_client->pers.netname ));
}

void NPC_CoopBlackGuard( gentity_t *NPC )
{
	if (!IsCoopGametype())
	{
		G_FreeEntity(NPC);
		return;
	}

	if (!characters_loaded)
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;

	if (NPC->spawnflags & TEAM_ALLIES)
		NPC->s.teamNum = TEAM_ALLIES;

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;

	NPC_Setup( NPC );

	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class; // For cgame access...

	// Load the NPC script for NPC->scriptName
	if (!NPC->scriptName)
		NPC->scriptName = "fieldops";

	if (!NPC_LoadData(NPC, NPC->scriptName))
	{
		G_Printf("Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName);

		NPC->scriptName = "default";
		
		if (!NPC_LoadData(NPC, NPC->scriptName))
		{
			G_Printf("Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName);
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy(NPC->s.origin, NPC->SP_NPC_Position);

	// Solids...
	NPC->clipmask	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->r.contents	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	NPC->s.pos.trType = TR_GRAVITY;

	VectorCopy(playerMins, NPC->r.mins);
	VectorCopy(playerMaxs, NPC->r.maxs);

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );

	SetNPCWeapon(NPC);

	{// Set up an NPC client structure...
		gclient_t *client;
		gentity_t *ent = NPC;

		NPC->NPC_client = G_Alloc(sizeof(*client));
		client = NPC->NPC_client;

		memset(client, 0, sizeof(*client));
		memset(&client->ps, 0, sizeof(client->ps));
		memset(&client->ps.weapons, 0, sizeof(client->ps.weapons));
		memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
		memset(client->ps.ammoclip, 0, sizeof(client->ps.ammoclip));

		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;			// DHM - Nerve

		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;

		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;

		VectorCopy(NPC->s.origin, NPC->NPC_client->ps.origin);

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;
		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy(ent->r.mins, client->ps.mins);
		VectorCopy(ent->r.maxs, client->ps.maxs);
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;
		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;	//----(SA)	added
		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;
		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink(ent-g_entities);
		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		VectorCopy(ent->NPC_client->ps.origin, ent->r.currentOrigin);
		trap_LinkEntity(ent);
		// clear entity state values
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;

		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	// Transmit name in 1 second...
	NPC->bot_special_action_thinktime = level.time + 1000;

	NPC->s.dl_intensity = NPC->health;
	NPC->nextthink = level.time - 1;
	NPCThink(NPC->s.number);

	// Send a name to the clients...
	strcpy(NPC->NPC_client->pers.netname, PickName( NPC->s.teamNum ));
	//trap_SendServerCommand( -1, va("npc_name %i %s", NPC->s.number, NPC->NPC_client->pers.netname ));
}

void NPC_CoopEliteGuard( gentity_t *NPC )
{
	if (!IsCoopGametype())
	{
		G_FreeEntity(NPC);
		return;
	}

	if (!characters_loaded)
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;

	if (NPC->spawnflags & TEAM_ALLIES)
		NPC->s.teamNum = TEAM_ALLIES;

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;

	NPC_Setup( NPC );

	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class; // For cgame access...

	// Load the NPC script for NPC->scriptName
	if (!NPC->scriptName)
		NPC->scriptName = "akimbo";

	if (!NPC_LoadData(NPC, NPC->scriptName))
	{
		G_Printf("Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName);

		NPC->scriptName = "default";
		
		if (!NPC_LoadData(NPC, NPC->scriptName))
		{
			G_Printf("Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName);
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy(NPC->s.origin, NPC->SP_NPC_Position);

	// Solids...
	NPC->clipmask	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->r.contents	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	NPC->s.pos.trType = TR_GRAVITY;

	VectorCopy(playerMins, NPC->r.mins);
	VectorCopy(playerMaxs, NPC->r.maxs);

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );

	SetNPCWeapon(NPC);

	{// Set up an NPC client structure...
		gclient_t *client;
		gentity_t *ent = NPC;

		NPC->NPC_client = G_Alloc(sizeof(*client));
		client = NPC->NPC_client;

		memset(client, 0, sizeof(*client));
		memset(&client->ps, 0, sizeof(client->ps));
		memset(&client->ps.weapons, 0, sizeof(client->ps.weapons));
		memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
		memset(client->ps.ammoclip, 0, sizeof(client->ps.ammoclip));

		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;			// DHM - Nerve

		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;

		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;

		VectorCopy(NPC->s.origin, NPC->NPC_client->ps.origin);

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;
		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy(ent->r.mins, client->ps.mins);
		VectorCopy(ent->r.maxs, client->ps.maxs);
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;
		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;	//----(SA)	added
		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;
		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink(ent-g_entities);
		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		VectorCopy(ent->NPC_client->ps.origin, ent->r.currentOrigin);
		trap_LinkEntity(ent);
		// clear entity state values
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;

		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	// Transmit name in 1 second...
	NPC->bot_special_action_thinktime = level.time + 1000;

	NPC->s.dl_intensity = NPC->health;
	NPC->nextthink = level.time - 1;
	NPCThink(NPC->s.number);

	// Send a name to the clients...
	strcpy(NPC->NPC_client->pers.netname, PickName( NPC->s.teamNum ));
	//trap_SendServerCommand( -1, va("npc_name %i %s", NPC->s.number, NPC->NPC_client->pers.netname ));
}

void NPC_CoopHelga( gentity_t *NPC )
{
	if (!IsCoopGametype())
	{
		G_FreeEntity(NPC);
		return;
	}

	if (!characters_loaded)
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;

	if (NPC->spawnflags & TEAM_ALLIES)
		NPC->s.teamNum = TEAM_ALLIES;

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;

	NPC_Setup( NPC );

	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class; // For cgame access...

	// Load the NPC script for NPC->scriptName
	if (!NPC->scriptName)
		NPC->scriptName = "akimbo";

	if (!NPC_LoadData(NPC, NPC->scriptName))
	{
		G_Printf("Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName);

		NPC->scriptName = "default";
		
		if (!NPC_LoadData(NPC, NPC->scriptName))
		{
			G_Printf("Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName);
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy(NPC->s.origin, NPC->SP_NPC_Position);

	// Solids...
	NPC->clipmask	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->r.contents	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	NPC->s.pos.trType = TR_GRAVITY;

	VectorCopy(playerMins, NPC->r.mins);
	VectorCopy(playerMaxs, NPC->r.maxs);

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );

	SetNPCWeapon(NPC);

	{// Set up an NPC client structure...
		gclient_t *client;
		gentity_t *ent = NPC;

		NPC->NPC_client = G_Alloc(sizeof(*client));
		client = NPC->NPC_client;

		memset(client, 0, sizeof(*client));
		memset(&client->ps, 0, sizeof(client->ps));
		memset(&client->ps.weapons, 0, sizeof(client->ps.weapons));
		memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
		memset(client->ps.ammoclip, 0, sizeof(client->ps.ammoclip));

		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;			// DHM - Nerve

		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;

		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;

		VectorCopy(NPC->s.origin, NPC->NPC_client->ps.origin);

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;
		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy(ent->r.mins, client->ps.mins);
		VectorCopy(ent->r.maxs, client->ps.maxs);
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;
		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;	//----(SA)	added
		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;
		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink(ent-g_entities);
		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		VectorCopy(ent->NPC_client->ps.origin, ent->r.currentOrigin);
		trap_LinkEntity(ent);
		// clear entity state values
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;

		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	// Transmit name in 1 second...
	NPC->bot_special_action_thinktime = level.time + 1000;

	NPC->s.dl_intensity = NPC->health;
	NPC->nextthink = level.time - 1;
	NPCThink(NPC->s.number);

	// Send a name to the clients...
	strcpy(NPC->NPC_client->pers.netname, PickName( NPC->s.teamNum ));
	//trap_SendServerCommand( -1, va("npc_name %i %s", NPC->s.number, NPC->NPC_client->pers.netname ));
}

void NPC_CoopProtoSoldier( gentity_t *NPC )
{
	if (!IsCoopGametype())
	{
		G_FreeEntity(NPC);
		return;
	}

	if (!characters_loaded)
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;

	if (NPC->spawnflags & TEAM_ALLIES)
		NPC->s.teamNum = TEAM_ALLIES;

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;

	NPC_Setup( NPC );

	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class; // For cgame access...

	// Load the NPC script for NPC->scriptName
	if (!NPC->scriptName)
		NPC->scriptName = "panzer";

	if (!NPC_LoadData(NPC, NPC->scriptName))
	{
		G_Printf("Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName);

		NPC->scriptName = "default";
		
		if (!NPC_LoadData(NPC, NPC->scriptName))
		{
			G_Printf("Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName);
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy(NPC->s.origin, NPC->SP_NPC_Position);

	// Solids...
	NPC->clipmask	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->r.contents	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	NPC->s.pos.trType = TR_GRAVITY;

	VectorCopy(playerMins, NPC->r.mins);
	VectorCopy(playerMaxs, NPC->r.maxs);

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );

	SetNPCWeapon(NPC);

	{// Set up an NPC client structure...
		gclient_t *client;
		gentity_t *ent = NPC;

		NPC->NPC_client = G_Alloc(sizeof(*client));
		client = NPC->NPC_client;

		memset(client, 0, sizeof(*client));
		memset(&client->ps, 0, sizeof(client->ps));
		memset(&client->ps.weapons, 0, sizeof(client->ps.weapons));
		memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
		memset(client->ps.ammoclip, 0, sizeof(client->ps.ammoclip));

		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;			// DHM - Nerve

		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;

		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;

		VectorCopy(NPC->s.origin, NPC->NPC_client->ps.origin);

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;
		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy(ent->r.mins, client->ps.mins);
		VectorCopy(ent->r.maxs, client->ps.maxs);
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;
		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;	//----(SA)	added
		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;
		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink(ent-g_entities);
		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		VectorCopy(ent->NPC_client->ps.origin, ent->r.currentOrigin);
		trap_LinkEntity(ent);
		// clear entity state values
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;

		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	// Transmit name in 1 second...
	NPC->bot_special_action_thinktime = level.time + 1000;

	NPC->s.dl_intensity = NPC->health;
	NPC->nextthink = level.time - 1;
	NPCThink(NPC->s.number);

	// Send a name to the clients...
	strcpy(NPC->NPC_client->pers.netname, PickName( NPC->s.teamNum ));
	//trap_SendServerCommand( -1, va("npc_name %i %s", NPC->s.number, NPC->NPC_client->pers.netname ));
}

void NPC_CoopSuperSoldier( gentity_t *NPC )
{
	if (!IsCoopGametype())
	{
		G_FreeEntity(NPC);
		return;
	}

	if (!characters_loaded)
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;

	if (NPC->spawnflags & TEAM_ALLIES)
		NPC->s.teamNum = TEAM_ALLIES;

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;

	NPC_Setup( NPC );

	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class; // For cgame access...

	// Load the NPC script for NPC->scriptName
	if (!NPC->scriptName)
		NPC->scriptName = "panzer";

	if (!NPC_LoadData(NPC, NPC->scriptName))
	{
		G_Printf("Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName);

		NPC->scriptName = "default";
		
		if (!NPC_LoadData(NPC, NPC->scriptName))
		{
			G_Printf("Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName);
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy(NPC->s.origin, NPC->SP_NPC_Position);

	// Solids...
	NPC->clipmask	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->r.contents	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	NPC->s.pos.trType = TR_GRAVITY;

	VectorCopy(playerMins, NPC->r.mins);
	VectorCopy(playerMaxs, NPC->r.maxs);

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );

	SetNPCWeapon(NPC);

	{// Set up an NPC client structure...
		gclient_t *client;
		gentity_t *ent = NPC;

		NPC->NPC_client = G_Alloc(sizeof(*client));
		client = NPC->NPC_client;

		memset(client, 0, sizeof(*client));
		memset(&client->ps, 0, sizeof(client->ps));
		memset(&client->ps.weapons, 0, sizeof(client->ps.weapons));
		memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
		memset(client->ps.ammoclip, 0, sizeof(client->ps.ammoclip));

		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;			// DHM - Nerve

		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;

		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;

		VectorCopy(NPC->s.origin, NPC->NPC_client->ps.origin);

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;
		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy(ent->r.mins, client->ps.mins);
		VectorCopy(ent->r.maxs, client->ps.maxs);
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;
		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;	//----(SA)	added
		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;
		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink(ent-g_entities);
		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		VectorCopy(ent->NPC_client->ps.origin, ent->r.currentOrigin);
		trap_LinkEntity(ent);
		// clear entity state values
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;

		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	// Transmit name in 1 second...
	NPC->bot_special_action_thinktime = level.time + 1000;

	NPC->s.dl_intensity = NPC->health;
	NPC->nextthink = level.time - 1;
	NPCThink(NPC->s.number);

	// Send a name to the clients...
	strcpy(NPC->NPC_client->pers.netname, PickName( NPC->s.teamNum ));
	//trap_SendServerCommand( -1, va("npc_name %i %s", NPC->s.number, NPC->NPC_client->pers.netname ));
}

extern vec3_t	playerMins;
extern vec3_t	playerMaxs;

//===========================================================================
// Routine      : Debug_NPC_Spawn
// Description  : Spawn an NPC.
void Debug_NPC_Spawn( gentity_t *CallEnt )
{
	vec3_t fwd;
	gentity_t *NPC = G_Spawn();

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;

	VectorCopy(CallEnt->r.currentOrigin, NPC->s.origin);
	AngleVectors( CallEnt->client->ps.viewangles, fwd, NULL, NULL );
	NPC->s.origin[0]+=(fwd[0]*64);
	NPC->s.origin[1]+=(fwd[1]*64);
	NPC->s.origin[2]+=(fwd[2]*64);

	NPC_Setup( NPC );

	NPC->model = CallEnt->model;
	NPC->s.modelindex = CallEnt->s.modelindex;

	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class; // For cgame access...

	//NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC_SOLDIER;

	NPC->s.teamNum = PickTeam(-1);//CallEnt->client->sess.sessionTeam;

	// Solids...
	NPC->clipmask	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->r.contents	= CONTENTS_SOLID | CONTENTS_PLAYERCLIP;//CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	NPC->s.pos.trType = TR_GRAVITY;

	VectorCopy(playerMins, NPC->r.mins);
	VectorCopy(playerMaxs, NPC->r.maxs);

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->playerType = Q_TrueRand(PC_SOLDIER, NUM_PLAYER_CLASSES-1);//NPC_SOLDIER;
	//NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;
	//NPC->s.modelindex2 = NPC->playerType;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );

	SetNPCWeapon(NPC);

	//if (!NPC->NPC_client)
	{// Set up an NPC client structure...
		gclient_t *client;
		gentity_t *ent = NPC;

		NPC->NPC_client = G_Alloc(sizeof(*client));
		client = NPC->NPC_client;

		memset(client, 0, sizeof(*client));
		memset(&client->ps, 0, sizeof(client->ps));
		memset(&client->ps.weapons, 0, sizeof(client->ps.weapons));
		memset(client->ps.ammo, 0, sizeof(client->ps.ammo));
		memset(client->ps.ammoclip, 0, sizeof(client->ps.ammoclip));
		// JPW NERVE -- zero out all ammo counts
		//memset(client->ps.Ammo, 0, sizeof(client->ps.Ammo));

		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;			// DHM - Nerve
		
		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;

		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;

		VectorCopy(NPC->s.origin, NPC->NPC_client->ps.origin);

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;
		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
//		client->ps.persistant[PERS_CHOOSED_WEAPON] = (client->sess.playerWeapon2 << 8) | client->sess.playerWeapon;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy(ent->r.mins, client->ps.mins);
		VectorCopy(ent->r.maxs, client->ps.maxs);
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - (client->ps.standViewHeight - client->ps.crouchViewHeight);
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;
		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;	//----(SA)	added
		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;
		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink(ent-g_entities);
		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		VectorCopy(ent->NPC_client->ps.origin, ent->r.currentOrigin);
		trap_LinkEntity(ent);
		// run the presend to set anything else
		//ClientEndFrame(ent);
		// clear entity state values
		BG_NPCStateToEntityState(&client->ps, &ent->s, qtrue);
		// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=569
		//G_ResetMarkers(ent);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;

		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}
	
	//G_Printf("Debug NPC Spawned...\n");

	// Transmit name in 1 second...
	NPC->bot_special_action_thinktime = level.time + 1000;

	NPC->s.dl_intensity = NPC->health;
	NPC->nextthink = level.time - 1;
	NPCThink(NPC->s.number);

	// Send a name to the clients...
	strcpy(NPC->NPC_client->pers.netname, PickName( NPC->s.teamNum ));
	//trap_SendServerCommand( -1, va("npc_name %i %s", NPC->s.number, NPC->NPC_client->pers.netname ));
}

void COOP_NPC_SelectType ( gentity_t *NPC )
{
	int choice = Q_TrueRand(0, 28);

	switch (choice)
	{
		case 0:
		case 1:
			NPC->scriptName = "akimbo";
			break;
		case 2:
			NPC->scriptName = "fieldops";
			break;
		case 3:
		case 4:
		case 5:
			NPC->scriptName = "flamer";
			break;
		case 6:
			NPC->scriptName = "grenader";
			{ // Find high spots for these...
				int nodeNum = Q_TrueRand(0, number_of_nodes);
			
				while (node_spawned_at[nodeNum] || spawnpoint_close_to_bad_object(nodeNum) || !above_average_node_height ( nodeNum ))
					nodeNum = Q_TrueRand(0, number_of_nodes);

				VectorCopy(nodes[nodeNum].origin, NPC->s.origin);
				node_spawned_at[nodeNum] = qtrue;
			}
			break;
		case 7:
			NPC->scriptName = "medic";
			break;
		case 8:
		case 9:
		case 10:
			NPC->scriptName = "mp42";
			break;
		case 11:
		case 12:
			NPC->scriptName = "panzer";
			if (Q_TrueRand(0,10) < 5)
			{ // Find high spots for these...
				int nodeNum = Q_TrueRand(0, number_of_nodes);
			
				while (node_spawned_at[nodeNum] || spawnpoint_close_to_bad_object(nodeNum) || !above_average_node_height ( nodeNum ))
					nodeNum = Q_TrueRand(0, number_of_nodes);

				VectorCopy(nodes[nodeNum].origin, NPC->s.origin);
				node_spawned_at[nodeNum] = qtrue;
			}
			break;
		case 13:
		case 14:
		case 15:
			NPC->scriptName = "sniper";
			{ // Find high spots for these...
				int nodeNum = Q_TrueRand(0, number_of_nodes);
			
				while (node_spawned_at[nodeNum] || spawnpoint_close_to_bad_object(nodeNum) || !above_average_node_height ( nodeNum ))
					nodeNum = Q_TrueRand(0, number_of_nodes);

				VectorCopy(nodes[nodeNum].origin, NPC->s.origin);
				node_spawned_at[nodeNum] = qtrue;
			}
			break;
/*		case 16:
			if (mg42_count() > mounts_added)
			{
				NPC->scriptName = "mounted";
				{ // Find high spots for these...
					int nodeNum = Q_TrueRand(0, number_of_nodes);
			
					while (node_spawned_at[nodeNum] || !mg42_scan(nodeNum) )
						nodeNum = Q_TrueRand(0, number_of_nodes);

					VectorCopy(nodes[nodeNum].origin, NPC->s.origin);
					node_spawned_at[nodeNum] = qtrue;
				}
				mounts_added++;
			}
			else
			{// An extra default... No spot to put another mg42 opperator...
				NPC->scriptName = "default";
			}
			break;*/
		case 17:
		case 18:
			NPC->scriptName = "rifle";
			break;
		case 19:
		case 20:
			NPC->scriptName = "rifle_grenade";
			break;
		case 21:
			NPC->scriptName = "sniper2";
			{ // Find high spots for these...
				int nodeNum = Q_TrueRand(0, number_of_nodes);
			
				while (node_spawned_at[nodeNum] || spawnpoint_close_to_bad_object(nodeNum) || !above_average_node_height ( nodeNum ))
					nodeNum = Q_TrueRand(0, number_of_nodes);

				VectorCopy(nodes[nodeNum].origin, NPC->s.origin);
				node_spawned_at[nodeNum] = qtrue;
			}
			break;
		default:
			NPC->scriptName = "default";
			break;
	}

	//NPC->scriptName = "mounted";
}

extern vec3_t	botTraceMins;
extern vec3_t	botTraceMaxs;

//special node visibility check for bot linkages..
//0 = wall in way
//1 = player or no obstruction
int
NPC_SpawnNodeVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg2, newOrg1;
	vec3_t	mins, maxs;

	VectorCopy( botTraceMins, mins );
	VectorCopy( botTraceMaxs, maxs );

	VectorCopy( org2, newOrg2 );
	VectorCopy( org1, newOrg1 );

	newOrg2[2] += 4; // Look from up a little...
	newOrg1[2] += 4; // Look from up a little...

	trap_Trace( &tr, newOrg1, mins, maxs, newOrg2, ignore, CONTENTS_PLAYERCLIP | MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA /*MASK_WATER*/ );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	VectorCopy( org2, newOrg2 );
	VectorCopy( org1, newOrg1 );

	newOrg2[2] += 16; // Look from up a little..
	newOrg1[2] += 16; // Look from up a little....

	trap_Trace( &tr, newOrg1, mins, maxs, newOrg2, ignore, MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA /*MASK_WATER*/ );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	VectorCopy( org2, newOrg2 );
	VectorCopy( org1, newOrg1 );

	newOrg2[2] += 32; // Look from up a little..
	newOrg1[2] += 32; // Look from up a little....

	trap_Trace( &tr, newOrg1, mins, maxs, newOrg2, ignore, MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA /*MASK_WATER*/ );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	VectorCopy( org2, newOrg2 );
	VectorCopy( org1, newOrg1 );

	newOrg2[2] += 48; // Look from up a little...
	newOrg1[2] += 48; // Look from up a little...

	trap_Trace( &tr, newOrg1, mins, maxs, newOrg2, ignore, MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA /*MASK_WATER*/ );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	VectorCopy( org2, newOrg2 );
	VectorCopy( org1, newOrg1 );

	newOrg2[2] += 64; // Look from up a little...
	newOrg1[2] += 64; // Look from up a little...

	trap_Trace( &tr, newOrg1, mins, maxs, newOrg2, ignore, MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA /*MASK_WATER*/ );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	VectorCopy( org2, newOrg2 );
	VectorCopy( org1, newOrg1 );

	newOrg2[2] += 128; // Look from up a little...
	newOrg1[2] += 128; // Look from up a little...

	trap_Trace( &tr, newOrg1, mins, maxs, newOrg2, ignore, MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA /*MASK_WATER*/ );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	return ( 0 );
}

//#define __NPC_NEW_COOP__

#ifdef __NPC_NEW_COOP__
int next_npc_spawn_check = 0;

qboolean NPC_Spot_Is_Visible( vec3_t origin )
{
	int i = 0;

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		gentity_t *ent = &g_entities[i];
		int z = 0;

		if (!ent)
			continue;

		if (!ent->client)
			continue;

		if (ent->client->pers.connected != CON_CONNECTED)
			continue;

		//if (ent->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR)
		//	continue;

		if (NPC_SpawnNodeVisible(ent->r.currentOrigin, origin, i))
			return qtrue;

		//ent->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, ent );
		//G_Printf("Current node is %i.\n", ent->current_node);
		
		//if (NPC_SpawnNodeVisible(origin, nodes[ent->current_node].origin, ent->s.number))
		//	return qtrue;

		/*
		for ( z = 0; z < nodes[ent->current_node].enodenum ; z++ )
		{// Check from all node links as well!
			if (SP_NodeVisible(origin, nodes[nodes[ent->current_node].links[z].targetNode].origin, ent->s.number))
				return qtrue;
		}
		*/
		/*for ( z = 0; z < number_of_nodes ; z++ )
		{// Check from all close nodes as well!
			if (VectorDistance(nodes[z].origin, ent->r.currentOrigin) > 64) // 256
				continue;

			if (NPC_SpawnNodeVisible(origin, nodes[z].origin, ent->s.number))
				return qtrue;
		}*/
	}

	return qfalse;
}
#endif //__NPC_NEW_COOP__

qboolean coop_npcs_checked = qfalse;

int		num_SP_SpawnPoints = 0;
vec3_t	SP_SpawnPoints[4096];

void SP_spawnpoint ( gentity_t *ent )
{
	VectorCopy(ent->r.currentOrigin, SP_SpawnPoints[num_SP_SpawnPoints]);
	num_SP_SpawnPoints++;
	G_FreeEntity(ent);
}

void COOP_NPC_Check ( void )
{// Does this map require us to randomly pick spots for npcs from the bot node positions??? -- (Unsupported map)
	int nodeNum = 0;

#ifdef __NPC_NEW_COOP__
	int i = 0;
	int j = 0;

	if (number_of_nodes <= 0)
	{
		G_Printf("*** No nodes to generate NPC spawnpoints!\n");
		return;
	}

	if (num_SP_SpawnPoints <= 0)
	{// Generate spawnpoints from nodes!
		if (number_of_nodes <= 256)
		{
			G_Printf("*** Not enough nodes to generate NPC spawnpoints!\n");
			return; // Not enough!
		}

		if (number_of_nodes <= 2048)
		{// Not many nodes for this map! Be less picky!
			while ( num_SP_SpawnPoints < 64 ) // 64 NPCs randomly per map???
			{
				nodeNum = Q_TrueRand(0, number_of_nodes);
			
				while (node_spawned_at[nodeNum] || spawnpoint_close_to_bad_object(nodeNum))
					nodeNum = Q_TrueRand(0, number_of_nodes);

				node_spawned_at[nodeNum] = qtrue;
				VectorCopy(nodes[nodeNum].origin, SP_SpawnPoints[num_SP_SpawnPoints]);
				num_SP_SpawnPoints++;
			}
		}
		else
		{
			while ( num_SP_SpawnPoints < 512 ) // 256 NPCs randomly per map???
			{
				nodeNum = Q_TrueRand(0, number_of_nodes);
			
				while (node_spawned_at[nodeNum] || spawnpoint_close_to_bad_object(nodeNum))
					nodeNum = Q_TrueRand(0, number_of_nodes);

				node_spawned_at[nodeNum] = qtrue;
				VectorCopy(nodes[nodeNum].origin, SP_SpawnPoints[num_SP_SpawnPoints]);
				num_SP_SpawnPoints++;
			}
		}

		G_Printf("*** Generated %i coop NPC spawn areas.\n", num_SP_SpawnPoints);
	}

	if (next_npc_spawn_check > level.time)
		return;

	next_npc_spawn_check = level.time + 5000;

	if (Count_NPC_Players() > (4*level.numPlayingClients))
		return; // Map specified some NPCs... Don't need to generate them...

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		gentity_t *ent = &g_entities[i];
		int num_added = 0;

		if (!ent)
			continue;

		if (!ent->client)
			continue;

		if (ent->client->pers.connected != CON_CONNECTED)
			continue;

		if (ent->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR)
			continue; // Don't spawn enemies for spectators!

		if (num_SP_SpawnPoints > 0)
		{// We have some NPC points specified, use them for speed! Checking all the nodes is too slow!
			for (j = 0; j < num_SP_SpawnPoints; j++)
			{
				if (VectorDistance(ent->r.currentOrigin, SP_SpawnPoints[j]) < 1024 && VectorDistance(ent->r.currentOrigin, SP_SpawnPoints[j]) > 512)
				{
					if (spawnpoint_close_to_bad_object(j))
						continue;

					if (!NPC_Spot_Is_Visible( SP_SpawnPoints[j] ))
					{
						gentity_t *NPC = G_Spawn();
						NPC->bot_coverspot_enemy = ent; // Set our enemy as the person we spawned for!

						VectorCopy(SP_SpawnPoints[j], NPC->s.origin);
						VectorSet(NPC->s.angles, 0, 0, 0);
						COOP_NPC_SelectType ( NPC );
						NPC_CoopSpawn( NPC );

						num_added++;
					}

					if (num_added >= 2)
						break;
				}
			}

			if (num_added > 0)
			{
				G_Printf("Added %i npc's for player %s. [1024]\n", num_added, ent->client->pers.netname);
				continue;
			}

			for (j = 0; j < num_SP_SpawnPoints; j++)
			{
				if (VectorDistance(ent->r.currentOrigin, SP_SpawnPoints[j]) < 2048 && VectorDistance(ent->r.currentOrigin, SP_SpawnPoints[j]) > 512)
				{
					if (spawnpoint_close_to_bad_object(j))
						continue;

					if (!NPC_Spot_Is_Visible( SP_SpawnPoints[j] ))
					{
						gentity_t *NPC = G_Spawn();
						NPC->bot_coverspot_enemy = ent; // Set our enemy as the person we spawned for!

						VectorCopy(SP_SpawnPoints[j], NPC->s.origin);
						VectorSet(NPC->s.angles, 0, 0, 0);
						COOP_NPC_SelectType ( NPC );
						NPC_CoopSpawn( NPC );

						num_added++;
					}

					if (num_added >= 2)
						break;
				}
			}

			if (num_added > 0)
			{
				G_Printf("Added %i npc's for player %s. [1024]\n", num_added, ent->client->pers.netname);
				continue;
			}

			for (j = 0; j < num_SP_SpawnPoints; j++)
			{
				if (VectorDistance(ent->r.currentOrigin, SP_SpawnPoints[j]) < 4096 && VectorDistance(ent->r.currentOrigin, SP_SpawnPoints[j]) > 512)
				{
					if (spawnpoint_close_to_bad_object(j))
						continue;

					if (!NPC_Spot_Is_Visible( SP_SpawnPoints[j] ))
					{
						gentity_t *NPC = G_Spawn();
						NPC->bot_coverspot_enemy = ent; // Set our enemy as the person we spawned for!

						VectorCopy(SP_SpawnPoints[j], NPC->s.origin);
						VectorSet(NPC->s.angles, 0, 0, 0);
						COOP_NPC_SelectType ( NPC );
						NPC_CoopSpawn( NPC );

						num_added++;
					}

					if (num_added >= 2)
						break;
				}
			}

			G_Printf("Added %i npc's for player %s. [4096]\n", num_added, ent->client->pers.netname);
		}
		else
		{// No NPC's specified for this map... Use node list! SLOW!!!
			for (j = 0; j < number_of_nodes; j+=Q_TrueRand(1, 8))
			{
				if (VectorDistance(ent->r.currentOrigin, nodes[j].origin) < 1024 && VectorDistance(ent->r.currentOrigin, nodes[j].origin) > 512)
				{
					if (spawnpoint_close_to_bad_object(j))
						continue;

					if (!NPC_Spot_Is_Visible( nodes[j].origin ))
					{
						gentity_t *NPC = G_Spawn();
						NPC->bot_coverspot_enemy = ent; // Set our enemy as the person we spawned for!

						VectorCopy(nodes[j].origin, NPC->s.origin);
						VectorSet(NPC->s.angles, 0, 0, 0);
						COOP_NPC_SelectType ( NPC );
						NPC_CoopSpawn( NPC );

						num_added++;
					}

					if (num_added >= 8)
						break;
				}
			}

			if (num_added > 0)
			{
				G_Printf("Added %i npc's for player %s. [1024]\n", num_added, ent->client->pers.netname);
				continue;
			}

			for (j = 0; j < number_of_nodes; j+=Q_TrueRand(1, 8))
			{
				if (VectorDistance(ent->r.currentOrigin, nodes[j].origin) < 2048 && VectorDistance(ent->r.currentOrigin, nodes[j].origin) > 512)
				{
					if (spawnpoint_close_to_bad_object(j))
						continue;

					if (!NPC_Spot_Is_Visible( nodes[j].origin ))
					{
						gentity_t *NPC = G_Spawn();
						NPC->bot_coverspot_enemy = ent; // Set our enemy as the person we spawned for!

						VectorCopy(nodes[j].origin, NPC->s.origin);
						VectorSet(NPC->s.angles, 0, 0, 0);
						COOP_NPC_SelectType ( NPC );
						NPC_CoopSpawn( NPC );

						num_added++;
					}

					if (num_added >= 8)
						break;
				}
			}

			if (num_added > 0)
			{
				G_Printf("Added %i npc's for player %s. [2048]\n", num_added, ent->client->pers.netname);
				continue;
			}

			for (j = 0; j < number_of_nodes; j+=Q_TrueRand(1, 8))
			{
				if (VectorDistance(ent->r.currentOrigin, nodes[j].origin) < 4096 && VectorDistance(ent->r.currentOrigin, nodes[j].origin) > 512)
				{
					if (spawnpoint_close_to_bad_object(j))
						continue;

					if (!NPC_Spot_Is_Visible( nodes[j].origin ))
					{
						gentity_t *NPC = G_Spawn();
						NPC->bot_coverspot_enemy = ent; // Set our enemy as the person we spawned for!

						VectorCopy(nodes[j].origin, NPC->s.origin);
						VectorSet(NPC->s.angles, 0, 0, 0);
						COOP_NPC_SelectType ( NPC );
						NPC_CoopSpawn( NPC );

						num_added++;
					}

					if (num_added >= 8)
						break;
				}
			}

			G_Printf("Added %i npc's for player %s. [4096]\n", num_added, ent->client->pers.netname);
		}
	}
#else //!__NPC_NEW_COOP__
	if (number_of_nodes <= 20)
		return;

	if (coop_npcs_checked)
		return;
	else
		coop_npcs_checked = qtrue;

	if (Count_NPC_Players() > 0)
		return; // Map specified some NPCs... Don't need to generate them...

#ifdef __ETE__
	G_Printf("^1*** ^3%s^5: No COOP \"npc\" entities specified for this map...\n", GAME_VERSION);
	G_Printf("                 ^5 : %s will be forced to generate spawnpoints from\n", GAME_VERSION);
	G_Printf("                 ^5 : random nodes! This is much less realistic!\n");
#else //!__ETE__
	G_Printf("^1*** ^3AIMod-ET^5: No COOP \"npc\" entities specified for this map...\n");
	G_Printf("           ^5 : AIMod-ET will be forced to generate spawnpoints from\n");
	G_Printf("           ^5 : random nodes! This is much less realistic!\n");
#endif //__ETE__

	while ( Count_NPC_Players() < 96 && Count_NPC_Players() < number_of_nodes * 0.1 ) // 96 NPCs randomly per map???
	{
		gentity_t *NPC = G_Spawn();
		nodeNum = Q_TrueRand(0, number_of_nodes);
			
		while (node_spawned_at[nodeNum] || spawnpoint_close_to_bad_object(nodeNum))
			nodeNum = Q_TrueRand(0, number_of_nodes);

		node_spawned_at[nodeNum] = qtrue;
		
		VectorCopy(nodes[nodeNum].origin, NPC->s.origin);
		VectorSet(NPC->s.angles, 0, 0, 0);
		COOP_NPC_SelectType ( NPC );
		NPC_CoopSpawn( NPC );
	}
#endif //__NPC_NEW_COOP__
}

extern void NPC_AI_FODDER_Humanoid_Move( gentity_t *NPC );

#define __NPC_STRAFE__

extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );

int
NPC_FindClosestNode ( gentity_t *bot, int r, int type, gentity_t *master )
{
	int		i;
	float	closest;
	float	nonLOSclosest;
	float	nonFOVclosest;
	float	dist;
	int		node = -1;
	int		secondbest = -1;
	vec3_t	v;
	float	rng;
	vec3_t	maxs, mins;

	VectorCopy( playerMins, mins );
	VectorCopy( playerMaxs, maxs );

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );
	closest = rng + 1;											// Cuz if we found none in the range, we just found none .
	nonLOSclosest = rng + 1;									// Cuz if we found none in the range, we just found none .
	nonFOVclosest = rng + 1;									// Cuz if we found none in the range, we just found none .

	//
	// Will attempt a close only check first with vis checks...
	//

	closest = 128.0f;										//64.0f;

	// Try with visibility check first!
/*	for ( i = 0; i < number_of_nodes; i += Q_TrueRand( 1, 4) )	//i++ ) // Randomize a little???
	{
		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
		dist = VectorLength( v );
		if
		(
			dist < closest &&
			i != bot->current_node &&
			i != bot->next_node
			&&
			!BAD_WP_Height( bot->r.currentOrigin, nodes[i].origin)
		)
		{
			if ( !(nodes[i].type & NODE_ICE) )
			{													// Not on ice (slick)...
				if (SP_NodeVisible(nodes[i].origin, bot->r.currentOrigin, bot->s.number))
				{
					secondbest = node;
					node = i;
					closest = dist;
				}
			}
		}
	}

	if (!node)*/
	{
		closest = 128.0f;										//64.0f;

		for ( i = 0; i < number_of_nodes; i += Q_TrueRand( 1, 4) )	//i++ ) // Randomize a little???
		{
			VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
			dist = VectorLength( v );
			if
			(
				dist < closest &&
				i != bot->current_node &&
				i != bot->next_node
				&&
				!BAD_WP_Height( bot->r.currentOrigin, nodes[i].origin)
			)
			{
				if ( !(nodes[i].type & NODE_ICE) )
				{													// Not on ice (slick)...
					secondbest = node;
					node = i;
					closest = dist;
				}
			}
		}
	}

	if ( secondbest > 0 && Q_TrueRand( 0, 2) == 2 )
	{
		return ( secondbest );
	}
	else
	{
		return ( node );
	}
}

int
NPC_FindClosestNodeToOrg ( vec3_t org )
{
	int		i;
	float	closest;
//	float	nonLOSclosest;
//	float	nonFOVclosest;
	float	dist;
	int		node = -1;
	int		secondbest = -1;
	vec3_t	v;
//	float	rng;
	vec3_t	maxs, mins;

	VectorCopy( playerMins, mins );
	VectorCopy( playerMaxs, maxs );

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	//
	// Will attempt a close only check first with vis checks...
	//

	closest = 128.0f;										//64.0f;

	for ( i = 0; i < number_of_nodes; i += Q_TrueRand( 1, 4) )	//i++ ) // Randomize a little???
	{
		VectorSubtract( nodes[i].origin, org, v );
		dist = VectorLength( v );
		if
		(
			dist < closest &&
			!BAD_WP_Height( org, nodes[i].origin)
		)
		{
			if ( !(nodes[i].type & NODE_ICE) )
			{													// Not on ice (slick)...
				secondbest = node;
				node = i;
				closest = dist;
			}
		}
	}

	if ( secondbest > 0 && Q_TrueRand( 0, 2) == 2 )
	{
		return ( secondbest );
	}
	else
	{
		return ( node );
	}
}

extern int			NodeVisible ( vec3_t org1, vec3_t org2, int ignore );
extern qboolean IgnoreTraceEntType ( int ent_type );

/* */
qboolean
AIMOD_NPC_MOVEMENT_WaypointTouched ( gentity_t *NPC, int touch_node, qboolean onLadder )
{
	vec3_t	node_origin;
	float	node_distance;
	float	heightdiff;
	VectorCopy( nodes[touch_node].origin, node_origin );
	node_distance = VectorDistanceNoHeight( NPC->r.currentOrigin, node_origin );
	heightdiff = HeightDistance( NPC->r.currentOrigin, node_origin );
	
	if ( onLadder && node_distance < 8 )
	{							// Need vertical checks on ladders...
		{
			return ( qtrue );
		}
	}

	if ( node_distance < 32 && nodes[touch_node].type == NODE_ICE )
	{							// Close enough when we are sliding...
		return ( qtrue );
	}

	if ( NPC->next_node >= 0 
		&& NPC->next_node < number_of_nodes 
		&& node_distance < 64 
		&& NodeVisible(NPC->r.currentOrigin, nodes[NPC->next_node].origin, NPC->s.number) )
	{							// At the node...
		return ( qtrue );
	}

	if ( node_distance < 16)
	{							// At the node...
		return ( qtrue );
	}

	return ( qfalse );
}

//===========================================================================
// Routine      : AIMOD_MOVEMENT_Backup

// Description  : Fallback navigation. For emergency routing...
void
AIMOD_NPC_MOVEMENT_Backup ( gentity_t *bot, usercmd_t *ucmd )
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
	if ( bot->s.eType == ET_NPC )
	{
		if ( vis != 1 /*&& vis != 2*/ /*|| (VectorLength(bot->NPC_client->ps.velocity) < 24 && bot->NPC_client->AImovetime < level.time + 1700)*/ )
		{														// We havn't moved at all... Let's randomize things.
			vec3_t	temp;

			VectorCopy( bot->s.angles, temp );
			temp[0] = bot->s.angles[0] = 0;
			temp[2] = bot->s.angles[2] = 0;
			temp[1] = Q_TrueRand( 0, 360 );

			bot->NPC_client->AImovetime = level.time + 2000;		// 10 seconds?
			VectorCopy( temp, bot->s.angles );

			ucmd->angles[PITCH] = ANGLE2SHORT(bot->s.angles[PITCH]);
			ucmd->angles[YAW]	= ANGLE2SHORT(bot->s.angles[YAW]);
			ucmd->angles[ROLL]	= ANGLE2SHORT(bot->s.angles[ROLL]);
		}
		else
		{
			ucmd->forwardmove = 127;
			ucmd->angles[PITCH] = ANGLE2SHORT(bot->s.angles[PITCH]);
			ucmd->angles[YAW]	= ANGLE2SHORT(bot->s.angles[YAW]);
			ucmd->angles[ROLL]	= ANGLE2SHORT(bot->s.angles[ROLL]);

			if ( VectorLength(bot->NPC_client->ps.velocity) < 24 && Q_TrueRand(0,10) < 3 )
			{
				ucmd->upmove = 127;									// If we are moving too slowly, sometimes jump (obstacles)
			}
		}

		if ( bot->waterlevel )
		{
			ucmd->upmove = 127;									// Try to always be on surface...
		}
	}
}

/* */
float
NPC_TravelTime ( gentity_t *bot )
{
	float	travel_time = 20000;
	return ( travel_time );
}

extern qboolean NPC_MOVEMENT_Find_Cover_Spot ( gentity_t *bot, usercmd_t *ucmd );
extern int FlagsForNodeLink ( int from, int to );
extern void BOT_MakeLinkSemiUnreachable ( gentity_t *bot, int node, int linkNum );

void NPC_AI_Humanoid_Move( gentity_t *NPC )
{
//	vec3_t origin, angles, move, amove;
//	gentity_t *obstacle = NULL;
	
/*	G_SetMovedir (NPC->s.angles, NPC->movedir);

	NPC->s.pos.trDelta[0] = 120;
	NPC->s.pos.trDelta[1] = 120;
	NPC->s.pos.trDelta[2] = 120;
	NPC->instantVelocity[0] = 120;
	NPC->instantVelocity[1] = 120;
	NPC->instantVelocity[2] = 120;

	trap_SnapVector(NPC->instantVelocity);
	trap_SnapVector(NPC->s.pos.trDelta);*/

/*	if (NPC_NodesLoaded() && (NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER))
	{// Humanoid fodder type NPC routing here...
		NPC_AI_FODDER_Humanoid_Move(NPC);
		return;
	}
	else*/
	{// Humanoid type NPC routing here... (Now default)
		usercmd_t *ucmd = &NPC->NPC_client->pers.cmd;
		float diff;

		//G_Printf("NPC %i moving!\n", NPC->s.number);

//		if( NPC->s.dl_intensity == TAG_HOOK || NPC->s.dl_intensity == TAG_HOOK_DRIVER || NPC->s.dl_intensity == TAG_HOOK_GUNNER1 || NPC->s.dl_intensity == TAG_HOOK_GUNNER2 )
//			return; // No moving while riding an NPC...

#ifdef __WEAPON_AIM__
		if (NPC->NPC_client->ps.eFlags & EF_AIMING)
			ucmd->buttons |= BUTTON_WALKING;
		else
			ucmd->buttons &= ~BUTTON_WALKING;
#endif //__WEAPON_AIM__

		if (IsCoopGametype())
		{// Coop NPCs need their own routing...
			vec3_t testpos;

			VectorCopy(NPC->r.currentOrigin, testpos);
			testpos[2] = NPC->SP_NPC_Position[2];

			if( NPC->npc_mount )
			{
				if (!(NPC->NPC_client->ps.eFlags & EF_MG42_ACTIVE))
				{
					NPC->npc_mg42 = NPC_FindMG42( NPC );

					if (NPC->npc_mg42)
						NPC->npc_mount = qfalse;
				}
			}

			if (NPC->npc_mg42)
			{// We have an MG42 to mount ourselves on.. Do it!
				if (NPC->NPC_client->ps.eFlags & EF_MG42_ACTIVE)
				{// We're mounted already...

				}
				else if (!(NPC->NPC_client->ps.eFlags & EF_MG42_ACTIVE))
				{// Use our mounted gun!
//					gentity_t *base = &g_entities[NPC->npc_mg42->mg42BaseEnt];
					gentity_t *gun = NPC->npc_mg42;
					gclient_t* cl = NPC->NPC_client;
//					vec3_t	point, forward;

					/*AngleVectors (gun->s.apos.trBase, forward, NULL, NULL);
					VectorMA (gun->r.currentOrigin, -36, forward, point);
					point[2] = NPC->r.currentOrigin[2];

					// Save initial position
					VectorCopy(point, NPC->TargetAngles);*/
					VectorSubtract( gun->r.currentOrigin, NPC->r.currentOrigin, NPC->move_vector);
					vectoangles(NPC->move_vector, NPC->s.angles);


					//G_SetOrigin(NPC, point);

					// Zero out velocity
					VectorCopy(vec3_origin, NPC->NPC_client->ps.velocity);
					VectorCopy(vec3_origin, NPC->s.pos.trDelta);

					gun->active = qtrue;
					NPC->active = qtrue;
					gun->r.ownerNum = NPC->s.number;
					VectorCopy (gun->s.angles, gun->TargetAngles);
					gun->s.otherEntityNum = NPC->s.number;

					cl->pmext.harc = gun->harc;
					cl->pmext.varc = gun->varc;
					VectorCopy(gun->s.angles, cl->pmext.centerangles);
					cl->pmext.centerangles[PITCH] = AngleNormalize180(cl->pmext.centerangles[PITCH]);
					cl->pmext.centerangles[YAW] = AngleNormalize180(cl->pmext.centerangles[YAW]);
					cl->pmext.centerangles[ROLL] = AngleNormalize180(cl->pmext.centerangles[ROLL]);

					NPC->backupWeaponTime = NPC->NPC_client->ps.weaponTime;
					NPC->NPC_client->ps.weaponTime = gun->backupWeaponTime;
//					NPC->NPC_client->ps.weaponsHeat[WEAPON_OVERHEAT_SLOT(WP_DUMMY_MG42)] = gun->mg42weapHeat;

					// Woop we got through, let's use the entity
					NPC->npc_mg42->use( NPC->npc_mg42, NPC, 0 );

					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;

					NPC->NPC_client->ps.eFlags |= EF_MG42_ACTIVE;
				}
				return;
			}

#ifdef __SCRIPTED_SP_NPC__
			if (NPC->npc_hunter && NPC->enemy)
			{// Find and kill them!
				if (NPC->enemy->health <= 0 || (NPC->enemy->client->ps.pm_flags & PMF_LIMBO) || OnSameTeam(NPC, NPC->enemy) || NPC->s.teamNum == TEAM_SPECTATOR )
				{// They are dead... Don't move... Initialize goal so they head back to their spawn position...
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					NPC->enemy = NULL;
					NPC->longTermGoal = -1;
					return; // next think...
				}

				if (NPC->bot_strafe_right_timer >= level.time)
				{
					if (VectorDistance(NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8)
					{
						ucmd->forwardmove = 64;
						ucmd->upmove = 0;
						ucmd->rightmove = 64;
						return;
					}
					else
					{
						NPC->bot_strafe_right_timer = 0;
						//ucmd->upmove = 127;
					}
				}

				if (NPC->bot_strafe_left_timer >= level.time)
				{
					if (VectorDistance(NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8)
					{
						ucmd->forwardmove = 64;
						ucmd->upmove = 0;
						ucmd->rightmove = -64;
						return;
					}
					else
					{
						NPC->bot_strafe_left_timer = 0;
						//ucmd->upmove = 127;
					}
				}

				if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes || NPC->current_node < 0 || NPC->next_node < 0 || NPC->node_timeout <= level.time)
				{// We hit a problem in route, or don't have one yet.. Find a new goal...
					memset(NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES);
					NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
					NPC->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( NPC->enemy->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL, NPC );

					if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes)
					{// Try to roam out of problems...
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						return; // next think...
					}

					NPC->pathsize = NPC_CreatePathAStar(NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist);
					NPC->next_node = BotGetNextNode(NPC);		//move to this node first, since it's where our path starts from
					NPC->node_timeout = level.time + 5000;
				}

				if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes)
				{// Try to roam out of problems...
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // next think...
				}

				if (VectorDistance(nodes[NPC->longTermGoal].origin, NPC->r.currentOrigin) < 48)
				{// We're there! Find a new goal...
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // Next think...
				}

				if (NPC->current_node <= 0 || NPC->current_node >= number_of_nodes)
				{
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // Next think...
				}

				if (VectorDistance(nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 48)
				{// At current node.. Pick next in the list...
					NPC->last_node = NPC->current_node;
					NPC->current_node = NPC->next_node;
					NPC->next_node = BotGetNextNode(NPC);
					NPC->node_timeout = level.time + 5000;
				}

				if (NPC->enemy && NPC->action > level.time - 100 
					&& VectorDistance(NPC->r.currentOrigin, NPC->enemy->r.currentOrigin) < 256)
				{// We're close enough to our enemy now... Don't move any more...
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // Next think...
				} 

				if (NPC->bot_last_good_move_time < level.time - 1000
					&& NPC->bot_last_move_fix_time < level.time - 2000 )
				{
					int result = -1;

					// So we don't vis check too often...
					NPC->bot_last_move_fix_time = level.time;

					result = NPC_MOVEMENT_ReachableBy(NPC, nodes[NPC->current_node].origin);

					if (result == NOT_REACHABLE)
					{// -1 is Not reachable at all... Reset our long term goal so it finds a new path next think!
						NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
						NPC->longTermGoal = 0;
						ucmd->forwardmove = 0;
						ucmd->upmove = 0;
						ucmd->rightmove = 0;
						return;
					}

					if (result == REACHABLE_JUMP)
					{// 1 is Jump...
						//ucmd->upmove = 127;
						ucmd->forwardmove = 127;
						NPC->NPC_client->ps.velocity[2] = 256;
						return;
					}

					if (result == REACHABLE_DUCK)
					{// 2 is Duck...
						if (!(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
						{// For special duck nodes, make the bot duck down...
							//ucmd->upmove = -48;
							NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
						}
					}

					if (result == REACHABLE_STRAFE_LEFT)
					{// 3 is Strafe Left...
						ucmd->rightmove = -48;
						return;
					}

					if (result == REACHABLE_STRAFE_RIGHT)
					{// 4 is Strafe Right...
						ucmd->rightmove = 48;
						return;
					}
				}

				if (NPC->last_node < 0 || NPC->last_node >= number_of_nodes)
				{
					if (NPC->current_node >= 0 && NPC->current_node <= number_of_nodes)
					{
						NPC->last_node = NPC->current_node-1;
					}
					else
					{
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						return; // Next think...
					}
				}

				if ( nodes[NPC->current_node].type & NODE_JUMP 
					|| nodes[NPC->current_node].origin[2] - NPC->r.currentOrigin[2] > 8 )
				{// For special jump nodes, make the bot jump...
					//ucmd->upmove = 127;
				}
				else if ( nodes[NPC->current_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
				{// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
				else if ( nodes[NPC->last_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
				{// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
				else if ( !( nodes[NPC->current_node].type & NODE_DUCK ) && !( nodes[NPC->last_node].type & NODE_DUCK )
					&& !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_CROUCHING) )
				{// Get up?
					//ucmd->upmove = 127; 
					NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
				}
				else if ( !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_PRONE) )
				{// Get up?
					//ucmd->upmove = 127; 
					NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
				}

				if (NPC->bot_strafe_right_timer < level.time && NPC->bot_strafe_left_timer < level.time
					&& ( ( VectorLength(NPC->NPC_client->ps.velocity) < 24 && !( nodes[NPC->current_node].type & NODE_DUCK ) && !( nodes[NPC->last_node].type & NODE_DUCK ) ) 
					/*|| NPC->bot_last_good_move_time < level.time - 3000*/ ) )
				{// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
					vec3_t forward, right, up, rightOrigin, leftOrigin, temp;
					trace_t tr;

					AngleVectors( NPC->NPC_client->ps.viewangles, forward, right, up );
					VectorMA( NPC->r.currentOrigin, 64, right, rightOrigin );
					VectorMA( NPC->r.currentOrigin, -64, right, leftOrigin );

					if (OrgVisible(rightOrigin, nodes[NPC->current_node].origin, NPC->s.number))
					{
						VectorCopy(rightOrigin, NPC->bot_strafe_target_position);
						NPC->bot_strafe_right_timer = level.time + 101;
						ucmd->forwardmove = 0;
						ucmd->rightmove = 64;
						ucmd->upmove = 0;
					}
					else if (OrgVisible(leftOrigin, nodes[NPC->current_node].origin, NPC->s.number))
					{
						VectorCopy(leftOrigin, NPC->bot_strafe_target_position);
						NPC->bot_strafe_left_timer = level.time + 100;
						ucmd->forwardmove = 0;
						ucmd->rightmove = -64;
						ucmd->upmove = 0;
					}
					else
					{
						//ucmd->upmove = 127;
						ucmd->forwardmove = 127;
						NPC->NPC_client->ps.velocity[2] = 256;
					}

					// Prepare to set another player/bot's (our blocker) strafe dir...
					VectorCopy(NPC->r.currentOrigin, temp);
					temp[0]+=forward[0]*64;
					temp[1]+=forward[1]*64;
					temp[2]+=forward[2]*64;

					trap_Trace(&tr, NPC->r.currentOrigin, NULL, NULL, temp, -1, MASK_SHOT|MASK_OPAQUE);

					if (tr.entityNum && (tr.entityNum < MAX_CLIENTS && g_entities[tr.entityNum].r.svFlags & SVF_BOT))
					{// A bot is stopping us... Tell them to strafe other direction to us...
						if (NPC->bot_strafe_left_timer >= level.time)
						{
							g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;

							AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
							VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
							VectorCopy(leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
						}
						else if (NPC->bot_strafe_right_timer >= level.time)
						{
							g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;	

							AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
							VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
							VectorCopy(rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
						}
					}
				}

				if (NPC->bot_strafe_right_timer >= level.time || NPC->bot_strafe_left_timer >= level.time)
				{
					return;
				}

				// Set up ideal view angles for this node...
				VectorSubtract (nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector);
				vectoangles (NPC->move_vector, NPC->bot_ideal_view_angles);
				NPC->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

				diff = fabs(AngleDifference(NPC->NPC_client->ps.viewangles[YAW], NPC->bot_ideal_view_angles[YAW]));

				if (diff > 90.0f)
				{// Delay large turns...
					NPCSetViewAngles(NPC, 500);
					VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
					ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
					ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
					ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
				}
				else
				{
					VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
					ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
					ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
					ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
					ucmd->forwardmove = 127;
					NPC->bot_last_good_move_time = level.time;
				}

				return;
			}

			if (NPC->npc_hunter && !NPC->enemy 
				&& VectorDistance(NPC->SP_NPC_Position, NPC->r.currentOrigin) > 256)
			{// Return to our marker!
				if (NPC->bot_strafe_right_timer >= level.time)
				{
					if (VectorDistance(NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8)
					{
						ucmd->forwardmove = 64;
						ucmd->upmove = 0;
						ucmd->rightmove = 64;
						return;
					}
					else
					{
						NPC->bot_strafe_right_timer = 0;
						//ucmd->upmove = 127;
					}
				}

				if (NPC->bot_strafe_left_timer >= level.time)
				{
					if (VectorDistance(NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8)
					{
						ucmd->forwardmove = 64;
						ucmd->upmove = 0;
						ucmd->rightmove = -64;
						return;
					}
					else
					{
						NPC->bot_strafe_left_timer = 0;
						//ucmd->upmove = 127;
					}
				}

				if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes || NPC->current_node < 0 || NPC->next_node < 0 || NPC->node_timeout <= level.time)
				{// We hit a problem in route, or don't have one yet.. Find a new goal...
					memset(NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES);
					NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
					NPC->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( NPC->SP_NPC_Position, NODE_DENSITY, NODEFIND_ALL, NPC );

					if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes)
					{// Try to roam out of problems...
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						return; // next think...
					}

					NPC->pathsize = NPC_CreatePathAStar(NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist);
					NPC->next_node = BotGetNextNode(NPC);		//move to this node first, since it's where our path starts from
					NPC->node_timeout = level.time + 5000;
				}

				if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes)
				{// Try to roam out of problems...
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // next think...
				}

				if (VectorDistance(nodes[NPC->longTermGoal].origin, NPC->r.currentOrigin) < 48)
				{// We're there! Find a new goal...
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // Next think...
				}

				if (NPC->current_node <= 0 || NPC->current_node >= number_of_nodes)
				{
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // Next think...
				}

				if (VectorDistance(nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 48)
				{// At current node.. Pick next in the list...
					NPC->last_node = NPC->current_node;
					NPC->current_node = NPC->next_node;
					NPC->next_node = BotGetNextNode(NPC);
					NPC->node_timeout = level.time + 5000;
				}

				if (NPC->enemy && NPC->action > level.time - 100 
					&& VectorDistance(NPC->r.currentOrigin, NPC->SP_NPC_Position) < 256)
				{// We're close enough to our spawn position now... Don't move any more...
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // Next think...
				} 

				if (NPC->bot_last_good_move_time < level.time - 1000
					&& NPC->bot_last_move_fix_time < level.time - 2000 )
				{
					int result = -1;

					// So we don't vis check too often...
					NPC->bot_last_move_fix_time = level.time;

					result = NPC_MOVEMENT_ReachableBy(NPC, nodes[NPC->current_node].origin);

					if (result == NOT_REACHABLE)
					{// -1 is Not reachable at all... Reset our long term goal so it finds a new path next think!
						NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
						NPC->longTermGoal = 0;
						ucmd->forwardmove = 0;
						ucmd->upmove = 0;
						ucmd->rightmove = 0;
						return;
					}

					if (result == REACHABLE_JUMP)
					{// 1 is Jump...
						//ucmd->upmove = 127;
						ucmd->forwardmove = 127;
						NPC->NPC_client->ps.velocity[2] = 256;
						return;
					}

					if (result == REACHABLE_DUCK)
					{// 2 is Duck...
						if (!(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
						{// For special duck nodes, make the bot duck down...
							//ucmd->upmove = -48;
							NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
						}
					}

					if (result == REACHABLE_STRAFE_LEFT)
					{// 3 is Strafe Left...
						ucmd->rightmove = -48;
						return;
					}

					if (result == REACHABLE_STRAFE_RIGHT)
					{// 4 is Strafe Right...
						ucmd->rightmove = 48;
						return;
					}
				}

				if (NPC->last_node < 0 || NPC->last_node >= number_of_nodes)
				{
					if (NPC->current_node >= 0 && NPC->current_node <= number_of_nodes)
					{
						NPC->last_node = NPC->current_node-1;
					}
					else
					{
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						return; // Next think...
					}
				}

				if ( nodes[NPC->current_node].type & NODE_JUMP 
					|| nodes[NPC->current_node].origin[2] - NPC->r.currentOrigin[2] > 8 )
				{// For special jump nodes, make the bot jump...
					//ucmd->upmove = 127;
				}
				else if ( nodes[NPC->current_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
				{// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
				else if ( nodes[NPC->last_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
				{// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
				else if ( !( nodes[NPC->current_node].type & NODE_DUCK ) && !( nodes[NPC->last_node].type & NODE_DUCK )
					&& !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_CROUCHING) )
				{// Get up?
					//ucmd->upmove = 127; 
					NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
				}
				else if ( !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_PRONE) )
				{// Get up?
					//ucmd->upmove = 127; 
					NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
				}

				if (NPC->bot_strafe_right_timer < level.time && NPC->bot_strafe_left_timer < level.time
					&& ( ( VectorLength(NPC->NPC_client->ps.velocity) < 24 && !( nodes[NPC->current_node].type & NODE_DUCK ) && !( nodes[NPC->last_node].type & NODE_DUCK ) ) 
					/*|| NPC->bot_last_good_move_time < level.time - 3000*/ ) )
				{// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
					vec3_t forward, right, up, rightOrigin, leftOrigin, temp;
					trace_t tr;

					AngleVectors( NPC->NPC_client->ps.viewangles, forward, right, up );
					VectorMA( NPC->r.currentOrigin, 64, right, rightOrigin );
					VectorMA( NPC->r.currentOrigin, -64, right, leftOrigin );

					if (OrgVisible(rightOrigin, nodes[NPC->current_node].origin, NPC->s.number))
					{
						VectorCopy(rightOrigin, NPC->bot_strafe_target_position);
						NPC->bot_strafe_right_timer = level.time + 101;
						ucmd->forwardmove = 0;
						ucmd->rightmove = 64;
						ucmd->upmove = 0;
					}
					else if (OrgVisible(leftOrigin, nodes[NPC->current_node].origin, NPC->s.number))
					{
						VectorCopy(leftOrigin, NPC->bot_strafe_target_position);
						NPC->bot_strafe_left_timer = level.time + 100;
						ucmd->forwardmove = 0;
						ucmd->rightmove = -64;
						ucmd->upmove = 0;
					}
					else
					{
						//ucmd->upmove = 127;
						ucmd->forwardmove = 127;
						NPC->NPC_client->ps.velocity[2] = 256;
					}

					// Prepare to set another player/bot's (our blocker) strafe dir...
					VectorCopy(NPC->r.currentOrigin, temp);
					temp[0]+=forward[0]*64;
					temp[1]+=forward[1]*64;
					temp[2]+=forward[2]*64;

					trap_Trace(&tr, NPC->r.currentOrigin, NULL, NULL, temp, -1, MASK_SHOT|MASK_OPAQUE);

					if (tr.entityNum && (tr.entityNum < MAX_CLIENTS && g_entities[tr.entityNum].r.svFlags & SVF_BOT))
					{// A bot is stopping us... Tell them to strafe other direction to us...
						if (NPC->bot_strafe_left_timer >= level.time)
						{
							g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;

							AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
							VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
							VectorCopy(leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
						}
						else if (NPC->bot_strafe_right_timer >= level.time)
						{
							g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;	

							AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
							VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
							VectorCopy(rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
						}
					}
				}

				if (NPC->bot_strafe_right_timer >= level.time || NPC->bot_strafe_left_timer >= level.time)
				{
					return;
				}

				// Set up ideal view angles for this node...
				VectorSubtract (nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector);
				vectoangles (NPC->move_vector, NPC->bot_ideal_view_angles);
				NPC->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

				diff = fabs(AngleDifference(NPC->NPC_client->ps.viewangles[YAW], NPC->bot_ideal_view_angles[YAW]));

				if (diff > 90.0f)
				{// Delay large turns...
					NPCSetViewAngles(NPC, 500);
					VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
					ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
					ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
					ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
				}
				else
				{
					VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
					ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
					ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
					ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
					
					if (ucmd->upmove > 0) // We need to jump something...
					{
						ucmd->forwardmove = 127;
						ucmd->buttons &= ~BUTTON_WALKING;
					}
					else
					{
						ucmd->forwardmove = 64;
						ucmd->buttons |= BUTTON_WALKING;
					}

					NPC->bot_last_good_move_time = level.time;
				}

				return;
			}

			if (NPC->npc_patrol)
			{// We're a patroling NPC!
				if (NPC->bot_strafe_right_timer >= level.time)
				{
					if (VectorDistance(NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8)
					{
						ucmd->forwardmove = 64;
						ucmd->upmove = 0;
						ucmd->rightmove = 64;
						return;
					}
					else
					{
						NPC->bot_strafe_right_timer = 0;
						//ucmd->upmove = 127;
					}
				}

				if (NPC->bot_strafe_left_timer >= level.time)
				{
					if (VectorDistance(NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8)
					{
						ucmd->forwardmove = 64;
						ucmd->upmove = 0;
						ucmd->rightmove = -64;
						return;
					}
					else
					{
						NPC->bot_strafe_left_timer = 0;
						//ucmd->upmove = 127;
					}
				}

				if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes || NPC->current_node < 0 || NPC->next_node < 0 || NPC->node_timeout <= level.time)
				{// We hit a problem in route, or don't have one yet.. Find a new goal...
					int		best_node = -1;
					float	best_dist = 1024.0f;
					int		node = 0;

					memset(NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES);
					NPC->current_node = NPC->npc_patrol_start_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );

					for (node = 0; node < number_of_nodes; node++)
					{// Find a good node to patrol to/from...
						float node_dist = VectorDistance(NPC->r.currentOrigin, nodes[node].origin);
						
						if ( node_dist < best_dist && node_dist > 512 )
						{
							best_dist = node_dist;
							best_node = node;
						}
					}

					if (best_node != -1)
					{
						NPC->longTermGoal = NPC->npc_patrol_end_node = best_node;
					}
					else
					{// We didnt find a good node to patrol to/from... Just stand there...
						NPC->npc_patrol = qfalse;
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						return;
					}

					if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes)
					{// Try to roam out of problems...
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						return; // next think...
					}

					NPC->pathsize = NPC_CreatePathAStar(NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist);
					NPC->next_node = BotGetNextNode(NPC);		//move to this node first, since it's where our path starts from
					NPC->node_timeout = level.time + 5000;
				}

				if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes)
				{// Try to roam out of problems...
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // next think...
				}

				if (VectorDistance(nodes[NPC->longTermGoal].origin, NPC->r.currentOrigin) < 48)
				{// We're there! Head back to our original point again...
					NPC->longTermGoal = NPC->npc_patrol_start_node;

					if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes)
					{// Try to roam out of problems...
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						return; // next think...
					}

					NPC->pathsize = NPC_CreatePathAStar(NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist);
					NPC->next_node = BotGetNextNode(NPC);		//move to this node first, since it's where our path starts from
					NPC->node_timeout = level.time + 5000;

					return; // Next think...
				}

				if (NPC->current_node <= 0 || NPC->current_node >= number_of_nodes)
				{
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // Next think...
				}

				if (VectorDistance(nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 48)
				{// At current node.. Pick next in the list...
					NPC->last_node = NPC->current_node;
					NPC->current_node = NPC->next_node;
					NPC->next_node = BotGetNextNode(NPC);
					NPC->node_timeout = level.time + 5000;
				}

				if (NPC->bot_last_good_move_time < level.time - 1000
					&& NPC->bot_last_move_fix_time < level.time - 2000 )
				{
					int result = -1;

					// So we don't vis check too often...
					NPC->bot_last_move_fix_time = level.time;

					result = NPC_MOVEMENT_ReachableBy(NPC, nodes[NPC->current_node].origin);

					if (result == NOT_REACHABLE)
					{// -1 is Not reachable at all... Reset our long term goal so it finds a new path next think!
						NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
						NPC->longTermGoal = -1;
						ucmd->forwardmove = 0;
						ucmd->upmove = 0;
						ucmd->rightmove = 0;
						return;
					}

					if (result == REACHABLE_JUMP)
					{// 1 is Jump...
						//ucmd->upmove = 127;
						ucmd->forwardmove = 127;
						NPC->NPC_client->ps.velocity[2] = 256;
						return;
					}

					if (result == REACHABLE_DUCK)
					{// 2 is Duck...
						if (!(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
						{// For special duck nodes, make the bot duck down...
							//ucmd->upmove = -48;
							NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
						}
					}

					if (result == REACHABLE_STRAFE_LEFT)
					{// 3 is Strafe Left...
						ucmd->rightmove = -48;
						return;
					}

					if (result == REACHABLE_STRAFE_RIGHT)
					{// 4 is Strafe Right...
						ucmd->rightmove = 48;
						return;
					}
				}

				if (NPC->last_node < 0 || NPC->last_node >= number_of_nodes)
				{
					if (NPC->current_node >= 0 && NPC->current_node <= number_of_nodes)
					{
						NPC->last_node = NPC->current_node-1;
					}
					else
					{
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						return; // Next think...
					}
				}

				if ( nodes[NPC->current_node].type & NODE_JUMP 
					|| nodes[NPC->current_node].origin[2] - NPC->r.currentOrigin[2] > 8 )
				{// For special jump nodes, make the bot jump...
					//ucmd->upmove = 127;
				}
				else if ( nodes[NPC->current_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
				{// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
				else if ( nodes[NPC->last_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
				{// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
				else if ( !( nodes[NPC->current_node].type & NODE_DUCK ) && !( nodes[NPC->last_node].type & NODE_DUCK )
					&& !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_CROUCHING) )
				{// Get up?
					//ucmd->upmove = 127; 
					NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
				}
				else if ( !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_PRONE) )
				{// Get up?
					//ucmd->upmove = 127; 
					NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
				}

				if (NPC->bot_strafe_right_timer < level.time && NPC->bot_strafe_left_timer < level.time
					&& ( ( VectorLength(NPC->NPC_client->ps.velocity) < 24 && !( nodes[NPC->current_node].type & NODE_DUCK ) && !( nodes[NPC->last_node].type & NODE_DUCK ) ) 
					/*|| NPC->bot_last_good_move_time < level.time - 3000*/ ) )
				{// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
					vec3_t forward, right, up, rightOrigin, leftOrigin, temp;
					trace_t tr;

					AngleVectors( NPC->NPC_client->ps.viewangles, forward, right, up );
					VectorMA( NPC->r.currentOrigin, 64, right, rightOrigin );
					VectorMA( NPC->r.currentOrigin, -64, right, leftOrigin );

					if (OrgVisible(rightOrigin, nodes[NPC->current_node].origin, NPC->s.number))
					{
						VectorCopy(rightOrigin, NPC->bot_strafe_target_position);
						NPC->bot_strafe_right_timer = level.time + 101;
						ucmd->forwardmove = 0;
						ucmd->rightmove = 64;
						ucmd->upmove = 0;
					}
					else if (OrgVisible(leftOrigin, nodes[NPC->current_node].origin, NPC->s.number))
					{
						VectorCopy(leftOrigin, NPC->bot_strafe_target_position);
						NPC->bot_strafe_left_timer = level.time + 100;
						ucmd->forwardmove = 0;
						ucmd->rightmove = -64;
						ucmd->upmove = 0;
					}
					else
					{
						//ucmd->upmove = 127;
						ucmd->forwardmove = 127;
						NPC->NPC_client->ps.velocity[2] = 256;
					}

					// Prepare to set another player/bot's (our blocker) strafe dir...
					VectorCopy(NPC->r.currentOrigin, temp);
					temp[0]+=forward[0]*64;
					temp[1]+=forward[1]*64;
					temp[2]+=forward[2]*64;

					trap_Trace(&tr, NPC->r.currentOrigin, NULL, NULL, temp, -1, MASK_SHOT|MASK_OPAQUE);

					if (tr.entityNum && (tr.entityNum < MAX_CLIENTS && g_entities[tr.entityNum].r.svFlags & SVF_BOT))
					{// A bot is stopping us... Tell them to strafe other direction to us...
						if (NPC->bot_strafe_left_timer >= level.time)
						{
							g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;

							AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
							VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
							VectorCopy(leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
						}
						else if (NPC->bot_strafe_right_timer >= level.time)
						{
							g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;	

							AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
							VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
							VectorCopy(rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
						}
					}
				}

				if (NPC->bot_strafe_right_timer >= level.time || NPC->bot_strafe_left_timer >= level.time)
				{
					return;
				}

				// Set up ideal view angles for this node...
				VectorSubtract (nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector);
				vectoangles (NPC->move_vector, NPC->bot_ideal_view_angles);
				NPC->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

				diff = fabs(AngleDifference(NPC->NPC_client->ps.viewangles[YAW], NPC->bot_ideal_view_angles[YAW]));

				if (diff > 90.0f)
				{// Delay large turns...
					NPCSetViewAngles(NPC, 500);
					VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
					ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
					ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
					ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
				}
				else
				{
					VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
					ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
					ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
					ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
					
					if (ucmd->upmove > 0) // We need to jump something...
					{
						ucmd->forwardmove = 127;
						ucmd->buttons &= ~BUTTON_WALKING;
					}
					else
					{
						ucmd->forwardmove = 64;
						ucmd->buttons |= BUTTON_WALKING;
					}

					NPC->bot_last_good_move_time = level.time;
				}

				return;
			}
			else
#endif //__SCRIPTED_SP_NPC__
			//
			// ... STANDARD SINGLE PLAYER MOVEMENT ...
			//

			{// Standard SP NPC movement, stroll around the spawnpoint!
				vec3_t org;
				usercmd_t *ucmd = &NPC->NPC_client->pers.cmd;

#ifdef __NPC_NEW_COOP__
				// Head for our enemy!  -- UQ1: FIXME - Head for cover point near enemy!

				if (NPC->bot_strafe_right_timer >= level.time)
				{
					if (VectorDistance(NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8)
					{
						ucmd->forwardmove = 64;
						ucmd->upmove = 0;
						ucmd->rightmove = 64;
						return;
					}
					else
					{
						NPC->bot_strafe_right_timer = 0;
						//ucmd->upmove = 127;
					}
				}

				if (NPC->bot_strafe_left_timer >= level.time)
				{
					if (VectorDistance(NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8)
					{
						ucmd->forwardmove = 64;
						ucmd->upmove = 0;
						ucmd->rightmove = -64;
						return;
					}
					else
					{
						NPC->bot_strafe_left_timer = 0;
						//ucmd->upmove = 127;
					}
				}

				if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes || NPC->current_node < 0 || NPC->next_node < 0 || NPC->node_timeout <= level.time || NPC->bot_last_move_fix_time < level.time)
				{// We hit a problem in route, or don't have one yet.. Find a new goal...
					int		best_node = -1;
					float	best_dist = 1024.0f;
					int		node = 0;

					NPC->bot_last_move_fix_time = level.time + 5000; // Every 5 secs we should set a new path!

					memset(NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES);
					NPC->current_node = NPC_FindClosestNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );

					if (NPC->bot_fastsnipe_check > 4)
					{// Too many route failures!
//#ifdef _DEBUG
						//G_Printf("NPC %i kicked because of too many route failures!\n", NPC->s.number);
//#endif //_DEBUG
						NPC->think = G_FreeEntity;
						NPC->bot_fastsnipe_check = 0;
						return;
					}

					if (NPC->current_node < 0 || NPC->current_node > number_of_nodes)
					{
						//NPC->think = G_FreeEntity;
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						NPC->bot_fastsnipe_check++;
						return;
					}

					if (NPC_MOVEMENT_Find_Cover_Spot( NPC, ucmd ))
					{
						best_node = NPC_FindClosestNodeToOrg( NPC->bot_coverspot );
						//G_Printf("NPC %i is using a covertspot!\n", NPC->s.number);
					}
					else
					{
						best_node = NPC_FindClosestNode( NPC->bot_coverspot_enemy, NODE_DENSITY, NODEFIND_ALL, NPC->bot_coverspot_enemy );
						//G_Printf("NPC %i is *NOT* using a covertspot!\n", NPC->s.number);
					}

					if (best_node != -1)
					{
						NPC->longTermGoal = NPC->npc_patrol_end_node = best_node;
					}
					else
					{
						NPC->longTermGoal = -1;
					}

					if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes)
					{// Try to roam out of problems...
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						//NPC->think = G_FreeEntity;
						//NPC->bot_fastsnipe_check++;
						NPC->bot_fastsnipe_check++;
						return; // next think...
					}

					NPC->pathsize = NPC_CreatePathAStar(NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist);
					NPC->next_node = BotGetNextNode(NPC);		//move to this node first, since it's where our path starts from
					NPC->node_timeout = level.time + 5000;

					if (NPC->pathsize <= 0)
					{
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						//NPC->think = G_FreeEntity;
						NPC->bot_fastsnipe_check++;
						return; // next think...
					}

					// Found a route. Reset the fail count.
					NPC->bot_fastsnipe_check = 0;
				}

				if (NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes)
				{// Try to roam out of problems...
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					//NPC->think = G_FreeEntity;
					return; // next think...
				}

				if ( VectorDistance(nodes[NPC->longTermGoal].origin, NPC->r.currentOrigin) < 48
					|| (NPC->bot_coverspot && VectorDistance(NPC->bot_coverspot, NPC->r.currentOrigin) < 256) && SP_NodeVisible(NPC->r.currentOrigin, NPC->bot_coverspot, NPC->s.number) )
				{// We're there! Do coverspot stuff if needed!
					if (NPC->bot_coverspot && VectorDistance(NPC->bot_coverspot, NPC->r.currentOrigin) < 256)
					{
						if (VectorDistance(NPC->bot_coverspot, NPC->r.currentOrigin) >= 32 
							&& SP_NodeVisible(NPC->r.currentOrigin, NPC->bot_coverspot, NPC->s.number))
						{// Move to the cover spot!
							// Set up ideal view angles for this node...
							VectorSubtract (NPC->bot_coverspot, NPC->r.currentOrigin, NPC->move_vector);
							vectoangles (NPC->move_vector, NPC->bot_ideal_view_angles);
							NPC->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

							VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
							ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
							ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
							ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
					
							ucmd->forwardmove = 127;

							NPC->bot_last_good_move_time = level.time;
							return;
						}
						else if (VectorDistance(NPC->bot_coverspot, NPC->r.currentOrigin) < 32)
						{// We are there! Do coverspot actions...
							ucmd->forwardmove = 0;
							ucmd->rightmove = 0;
							ucmd->upmove = 0;

							/*if (Q_TrueRand(0,10) >= 8 || NPC->bot_coverspot_staydown_timer > level.time)
							{
								NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
								NPC->bot_coverspot_staydown_timer = level.time + 10000;
							}
							else
							{
								//NPC->NPC_client->ps.pm_flags &= ~PMF_DUCKED;
								NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
								NPC->bot_coverspot_staydown_timer = 0;
							}*/
							return;
						}

					}
					else
					{
						NPC->longTermGoal = -1;//NPC->npc_patrol_start_node;

						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
					}
					return; // Next think...
				}

				if (NPC->current_node <= 0 || NPC->current_node >= number_of_nodes)
				{
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return; // Next think...
				}

				if (VectorDistance(nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 48)
				{// At current node.. Pick next in the list...
					NPC->last_node = NPC->current_node;
					NPC->current_node = NPC->next_node;
					NPC->next_node = BotGetNextNode(NPC);
					NPC->node_timeout = level.time + 5000;
				}

				if (NPC->bot_last_good_move_time < level.time - 1000
					&& NPC->bot_last_move_fix_time < level.time - 2000 )
				{
					int result = -1;

					// So we don't vis check too often...
					NPC->bot_last_move_fix_time = level.time;

					result = NPC_MOVEMENT_ReachableBy(NPC, nodes[NPC->current_node].origin);

					if (result == NOT_REACHABLE)
					{// -1 is Not reachable at all... Reset our long term goal so it finds a new path next think!
						NPC->current_node = NPC_FindClosestNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
						NPC->longTermGoal = -1;
						ucmd->forwardmove = 0;
						ucmd->upmove = 0;
						ucmd->rightmove = 0;
						return;
					}

					if (result == REACHABLE_JUMP)
					{// 1 is Jump...
						//ucmd->upmove = 127;
						ucmd->forwardmove = 127;
						NPC->NPC_client->ps.velocity[2] = 256;
						return;
					}

					if (result == REACHABLE_DUCK)
					{// 2 is Duck...
						if (!(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
						{// For special duck nodes, make the bot duck down...
							//ucmd->upmove = -48;
							NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
						}
					}

					if (result == REACHABLE_STRAFE_LEFT)
					{// 3 is Strafe Left...
						ucmd->rightmove = -48;
						return;
					}

					if (result == REACHABLE_STRAFE_RIGHT)
					{// 4 is Strafe Right...
						ucmd->rightmove = 48;
						return;
					}
				}

				if (NPC->last_node < 0 || NPC->last_node >= number_of_nodes)
				{
					if (NPC->current_node >= 0 && NPC->current_node <= number_of_nodes)
					{
						NPC->last_node = NPC->current_node-1;
					}
					else
					{
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						return; // Next think...
					}
				}

				if ( nodes[NPC->current_node].type & NODE_JUMP 
					|| nodes[NPC->current_node].origin[2] - NPC->r.currentOrigin[2] > 8 )
				{// For special jump nodes, make the bot jump...
					//ucmd->upmove = 127;
				}
				else if ( nodes[NPC->current_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
				{// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
				else if ( nodes[NPC->last_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
				{// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
				else if ( !( nodes[NPC->current_node].type & NODE_DUCK ) && !( nodes[NPC->last_node].type & NODE_DUCK )
					&& !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_CROUCHING) )
				{// Get up?
					//ucmd->upmove = 127; 
					NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
				}
				else if ( !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_PRONE) )
				{// Get up?
					//ucmd->upmove = 127; 
					NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
				}

				if (NPC->bot_strafe_right_timer < level.time && NPC->bot_strafe_left_timer < level.time
					&& ( ( VectorLength(NPC->NPC_client->ps.velocity) < 24 && !( nodes[NPC->current_node].type & NODE_DUCK ) && !( nodes[NPC->last_node].type & NODE_DUCK ) ) 
					/*|| NPC->bot_last_good_move_time < level.time - 3000*/ ) )
				{// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
					vec3_t forward, right, up, rightOrigin, leftOrigin, temp;
					trace_t tr;

					AngleVectors( NPC->NPC_client->ps.viewangles, forward, right, up );
					VectorMA( NPC->r.currentOrigin, 64, right, rightOrigin );
					VectorMA( NPC->r.currentOrigin, -64, right, leftOrigin );

					if (OrgVisible(rightOrigin, nodes[NPC->current_node].origin, NPC->s.number))
					{
						VectorCopy(rightOrigin, NPC->bot_strafe_target_position);
						NPC->bot_strafe_right_timer = level.time + 101;
						ucmd->forwardmove = 0;
						ucmd->rightmove = 64;
						ucmd->upmove = 0;
					}
					else if (OrgVisible(leftOrigin, nodes[NPC->current_node].origin, NPC->s.number))
					{
						VectorCopy(leftOrigin, NPC->bot_strafe_target_position);
						NPC->bot_strafe_left_timer = level.time + 100;
						ucmd->forwardmove = 0;
						ucmd->rightmove = -64;
						ucmd->upmove = 0;
					}
					else
					{
						//ucmd->upmove = 127;
						ucmd->forwardmove = 127;
						NPC->NPC_client->ps.velocity[2] = 256;
					}

					// Prepare to set another player/bot's (our blocker) strafe dir...
					VectorCopy(NPC->r.currentOrigin, temp);
					temp[0]+=forward[0]*64;
					temp[1]+=forward[1]*64;
					temp[2]+=forward[2]*64;

					trap_Trace(&tr, NPC->r.currentOrigin, NULL, NULL, temp, -1, MASK_SHOT|MASK_OPAQUE);

					if (tr.entityNum && (tr.entityNum < MAX_CLIENTS && g_entities[tr.entityNum].r.svFlags & SVF_BOT))
					{// A bot is stopping us... Tell them to strafe other direction to us...
						if (NPC->bot_strafe_left_timer >= level.time)
						{
							g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;

							AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
							VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
							VectorCopy(leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
						}
						else if (NPC->bot_strafe_right_timer >= level.time)
						{
							g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;	

							AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
							VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
							VectorCopy(rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
						}
					}

					if (tr.entityNum && (g_entities[tr.entityNum].s.eType == ET_NPC))
					{// A bot is stopping us... Tell them to strafe other direction to us...
						if (NPC->bot_strafe_left_timer >= level.time)
						{
							g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;

							AngleVectors( g_entities[tr.entityNum].NPC_client->ps.viewangles, forward, right, up );
							VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
							VectorCopy(leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
						}
						else if (NPC->bot_strafe_right_timer >= level.time)
						{
							g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;	

							AngleVectors( g_entities[tr.entityNum].NPC_client->ps.viewangles, forward, right, up );
							VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
							VectorCopy(rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
						}
					}
				}

				if (NPC->bot_strafe_right_timer >= level.time || NPC->bot_strafe_left_timer >= level.time)
				{
					return;
				}

				// Set up ideal view angles for this node...
				VectorSubtract (nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector);
				vectoangles (NPC->move_vector, NPC->bot_ideal_view_angles);
				NPC->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

				diff = fabs(AngleDifference(NPC->NPC_client->ps.viewangles[YAW], NPC->bot_ideal_view_angles[YAW]));

				if (diff > 90.0f)
				{// Delay large turns...
					NPCSetViewAngles(NPC, 500);
					VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
					ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
					ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
					ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
				}
				else
				{
					VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
					ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
					ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
					ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
					
					/*if (ucmd->upmove > 0) // We need to jump something...
					{
						ucmd->forwardmove = 127;
						ucmd->buttons &= ~BUTTON_WALKING;
					}
					else
					{
						ucmd->forwardmove = 64;
						ucmd->buttons |= BUTTON_WALKING;
					}*/
					ucmd->forwardmove = 127;

					NPC->bot_last_good_move_time = level.time;
				}

				return;

#else //!__NPC_NEW_COOP__
				VectorCopy(NPC->r.currentOrigin, org);

				NPC->bot_initialized = qtrue;

				VectorCopy(org, NPC->bot_sp_spawnpos);
				AIMOD_NPC_SP_MOVEMENT_Move( NPC, ucmd );
				return;
#endif //__NPC_NEW_COOP__
			}

			return;
		}

		//
		// ... NO WAYPOINTING ...
		//

		else if (!nodes_loaded)
		{// Randomize!
			if (!NPC->oldOrigin)
			{
				VectorCopy(NPC->r.currentOrigin, NPC->oldOrigin);
			}

			{
				float velocity = VectorDistance(NPC->r.currentOrigin, NPC->oldOrigin);

				if (velocity < 8)
				{// Randomize our direction.. If too slow...
					NPC->bot_ideal_view_angles[PITCH] = 0;
					NPC->bot_ideal_view_angles[YAW] = Q_TrueRand(-360, 360);
					NPC->bot_ideal_view_angles[ROLL] = 0;
				}

				NPCSetViewAngles(NPC, 100);
				VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
				ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
				ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
				ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);

				ucmd->forwardmove = 127;
				ucmd->upmove = 0;
				ucmd->rightmove = 0;

				// Set origin ready for getting velocity next frame...
				VectorCopy(NPC->r.currentOrigin, NPC->oldOrigin);
				return;
			}
		}
		/*else if (NPC->bot_blind_routing_time > level.time)
		{
			vec3_t org;
			usercmd_t *ucmd = &NPC->NPC_client->pers.cmd;

			VectorCopy(NPC->r.currentOrigin, org);

			NPC->bot_initialized = qtrue;

			VectorCopy(org, NPC->bot_sp_spawnpos);
			AIMOD_NPC_SP_MOVEMENT_Move( NPC, ucmd );
			return;
		}*/
/*		else if ( npc_simplenav.integer || NPC->bot_blind_routing_time > level.time)
		{// Mindless routing for using large numbers of NPCs...
//			vec3_t fwd;

			if (NPC->current_node <= 0 || NPC->current_node >= number_of_nodes || NPC->node_timeout <= level.time)
			{// Find a new node...
				if (NPC->current_node <= 0)
				{// Default to forwards...
					NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
					NPC->last_node = NPC->current_node-1;
					NPC->next_node = NPC->current_node+1;
					NPC->node_timeout = level.time + 5000;
					NPC->reversed_route = qfalse;
				}
				else if (NPC->current_node >= number_of_nodes)
				{// Switch to reversed route...
					NPC->current_node = NPC->current_node-1;
					NPC->last_node = NPC->current_node+1;
					NPC->next_node = NPC->current_node-1;
					NPC->node_timeout = level.time + 5000;
					NPC->reversed_route = qtrue;
				}
				else
				{// Default to forwards...
					NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
					NPC->last_node = NPC->current_node-1;
					NPC->next_node = NPC->current_node+1;
					NPC->node_timeout = level.time + 5000;
					NPC->reversed_route = qfalse;
				}
			}

			if (NPC->current_node <= 0 || NPC->current_node >= number_of_nodes)
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return; // Next think...
			}

			if (VectorDistance(nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 48)
			{// At current node.. Pick next in the list...
				if (NPC->reversed_route)
				{
					NPC->last_node = NPC->current_node;
					NPC->current_node = NPC->next_node;
					NPC->next_node = NPC->current_node-1;
					NPC->node_timeout = level.time + 5000;
					NPC->reversed_route = qtrue;
				}
				else
				{
					NPC->last_node = NPC->current_node;
					NPC->current_node = NPC->next_node;
					NPC->next_node = NPC->current_node+1;
					NPC->node_timeout = level.time + 5000;
					NPC->reversed_route = qfalse;
				}
			}
		}*/

		//
		// ... WAYPOINTED NPC MOVEMENT ...
		//

		else
		{// Standard navigation.. Uses pathfinding...
			qboolean	onLadder = qfalse;
			trace_t		tr;
			vec3_t		viewpos;
			int			node_link_flags = -1;
			gentity_t	*bot = NPC; // UQ1: Added to make copy/paste easier lol...

			if (NPC->bot_random_move_time > level.time)
			{
				// Initialize move destination...
				if (!bot->bot_initialized)
				{
					bot->bot_initialized = qtrue;
					VectorCopy(bot->r.currentOrigin, bot->bot_sp_spawnpos);
				}

				AIMOD_NPC_SP_MOVEMENT_Move( NPC, ucmd );
				return;
			}

			bot->bot_initialized = qfalse;

			// Ladder check...
			VectorCopy(NPC->r.currentOrigin, viewpos);
			viewpos[2]+=32;

			if (NPC->NPC_client)
			{
				if (NPC->NPC_client->ps.serverCursorHint == HINT_LADDER ||
					NPC->NPC_client->ps.torsoAnim == BOTH_CLIMB ||
					NPC->NPC_client->ps.legsAnim == BOTH_CLIMB ||
					NPC->NPC_client->ps.torsoAnim == BOTH_CLIMB_DOWN ||
					NPC->NPC_client->ps.legsAnim == BOTH_CLIMB_DOWN ||
					(NPC->NPC_client->ps.pm_flags & PMF_LADDER))
						onLadder = qtrue;
			}

			if (!onLadder)
			{
				VectorCopy(NPC->r.currentOrigin, viewpos);
				viewpos[2]+=32;

				trap_Trace(&tr, viewpos, NULL, NULL, nodes[NPC->current_node].origin, NPC->s.number, MASK_PLAYERSOLID);

				if (tr.surfaceFlags & SURF_LADDER)
				{
					onLadder = qtrue;
				}
			}

			if ( bot->current_node >= 0 && bot->next_node >= 0 )
			{// Do some waypoint validity checking...
				if (bot->current_node >= number_of_nodes)
				{
					if (bot_debug.integer)
						G_Printf("NPC DEBUG: bot->current_node >= number_of_nodes\n");

					bot->current_node = -1;
				}

				if (bot->next_node >= number_of_nodes)
				{
					if (bot_debug.integer)
						G_Printf("NPC DEBUG: bot->next_node >= number_of_nodes\n");

					bot->next_node = -1;
				}
			}
/*
			if ( bot->current_node >= 0 && bot->current_node < number_of_nodes )
			{// Check our path for bad stuff... Mark it if found!
				trace_t			tr;
				vec3_t			org1, org2;

				VectorCopy(bot->r.currentOrigin, org1);
				VectorCopy(nodes[bot->current_node].origin, org2);

				org1[2]+=16;
				org2[2]+=16;

				trap_Trace( &tr, org1, NULL, NULL, org2, bot->s.number, MASK_WATER );

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
						bot->current_node = -1;
						bot->longTermGoal = -1;

						if (bot_debug.integer)
							G_Printf("NPC DEBUG: %s - Hit barb wire\n", bot->NPC_client->pers.netname);
					}
				}
				else
				{
					if (((tr.entityNum >= 0 && tr.entityNum < ENTITYNUM_MAX_NORMAL && g_entities[tr.entityNum].s.eType == ET_MOVER && g_entities[tr.entityNum].allowteams && (g_entities[tr.entityNum].allowteams & bot->NPC_client->sess.sessionTeam)) 
						|| tr.entityNum == ENTITYNUM_WORLD) && !(tr.surfaceFlags & SURF_LADDER))
					{// A useable door is in the way! That is ok!

					}
					else if (((tr.entityNum >= 0 && tr.entityNum < ENTITYNUM_MAX_NORMAL && !IgnoreTraceEntType(g_entities[tr.entityNum].s.eType)) 
						|| tr.entityNum == ENTITYNUM_WORLD) && !(tr.surfaceFlags & SURF_LADDER))
					{// Solid in the way. We can not continue!
						int loop = 0;

						bot->current_node = -1;
						bot->longTermGoal = -1;

						if (bot_debug.integer)
							G_Printf("NPC DEBUG: %s - solid in the way\n", bot->NPC_client->pers.netname);

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

						bot->current_node = -1;
						bot->longTermGoal = -1;

						if (bot_debug.integer)
							G_Printf("NPC DEBUG: %s - unusable mover/door/etc in path\n", bot->NPC_client->pers.netname);

						for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
						{													// Find the link we last used and mark it...
							if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
							{												// Mark it with extreme cost!
								BOT_MakeLinkSemiUnreachable( bot, bot->last_node, loop );
								break;
							}
						}
					}
				}
			}*/

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

			if ( bot->current_node < 0 
				|| bot->longTermGoal < 0 
				|| bot->current_node > number_of_nodes 
				|| bot->longTermGoal > number_of_nodes )
			{
				/*if (bot_debug.integer)
				{
					if (bot->current_node < 0)
						G_Printf("NPC DEBUG: %s - NPC->current_node < 0 (%i)\n", bot->NPC_client->pers.netname, bot->current_node);
					else if (bot->longTermGoal < 0)
						G_Printf("NPC DEBUG: NPC->longTermGoal < 0\n");
					else if (NPC->current_node > number_of_nodes)
						G_Printf("NPC DEBUG: NPC->current_node > number_of_nodes\n");
					else if (NPC->longTermGoal > number_of_nodes)
						G_Printf("NPC DEBUG: NPC->longTermGoal > number_of_nodes\n");
				}*/

				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

#ifdef _WIN32
				NPC_Queue_New_Path( bot, ucmd );

				bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
				//bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
				return;
#else //!_WIN32
				NPC_Set_New_Path( bot, ucmd );
			
				bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
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

			if ((g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
				&& bot->next_node
				&& bot->NPC_client->ps.stats[STAT_FLAG_CAPTURE] > 0 
				&& g_entities[bot->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != bot->s.teamNum
				&& (!bot->next_node || VectorDistance(bot->r.currentOrigin, g_entities[bot->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.origin) <= VectorDistance(nodes[bot->next_node].origin, g_entities[bot->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.origin)))
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
				return;
			}
			else if ((g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
				&& !bot->next_node
				&& bot->NPC_client->ps.stats[STAT_FLAG_CAPTURE] > 0 
				&& g_entities[bot->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != bot->s.teamNum)
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
				return;
			}

			/*if ( bot->node_timeout < level.time - 2000 )
			{
				trace_t tr;
				vec3_t viewpos;

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
				bot->current_node = -1;
				bot->longTermGoal = -1;
		
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				if (bot_debug.integer)
					G_Printf("NPC DEBUG: %s - bot->node_timeout < level.time - 2000\n", bot->NPC_client->pers.netname);

				bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
				return;
			}

			if ( bot->bot_last_position_check < level.time - 2000 || bot->bot_last_good_move_time < level.time - 3000)
			{// Check if we have moved in the last 2 seconds... (Are we stuck somewhere?)
				if ( VectorDistanceNoHeight( bot->bot_last_position, bot->r.currentOrigin) <= 8  )
				{	// We are stuck... Find a new route!
					// First see if we should disable this link..
					trace_t tr;
					vec3_t viewpos;

					//G_Printf("Screwed at 1.\n");

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

					if (bot_debug.integer)
					{
						if (bot->bot_last_good_move_time < level.time - 3000)
							G_Printf("NPC DEBUG: %s - bot->bot_last_good_move_time < level.time - 3000\n", bot->NPC_client->pers.netname);
						else
							G_Printf("NPC DEBUG: %s - VectorDistanceNoHeight( bot->bot_last_position, bot->r.currentOrigin) <= 8 (%f)\n", bot->NPC_client->pers.netname, VectorDistanceNoHeight( bot->bot_last_position, bot->r.currentOrigin));
					}

					// Find a new route...
					bot->current_node = -1;
					bot->longTermGoal = -1;
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
					bot->bot_last_good_move_time = level.time;
					bot->bot_last_position_check = level.time;
					VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
					bot->bot_random_move_time = level.time + 500 + Q_TrueRand(5000, 10000);
					return;
				}

				bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
				bot->bot_last_good_move_time = level.time;
				bot->bot_last_position_check = level.time;
				VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			}*/

			if ( bot->current_node && AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
			{// At current node.. Pick next in the list...
				if ( bot->current_node == bot->longTermGoal )
				{// Hit our long term goal...
					bot->current_node = -1;
					bot->longTermGoal = -1;
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
					bot->bot_last_good_move_time = level.time;
					bot->bot_last_position_check = level.time;
					VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

					if (bot_debug.integer)
					{
						G_Printf("NPC DEBUG: %s - hit his long term goal!\n", bot->NPC_client->pers.netname);
					}
					return;
				}
				else
				{// Continue on to next node...
					bot->last_node = bot->current_node;
					bot->current_node = bot->next_node;
					bot->next_node = BotGetNextNode( bot );
					bot->node_timeout = level.time + NPC_TravelTime( bot );

					if (bot->current_node > number_of_nodes)
					{
						if (bot_debug.integer)
							G_Printf("NPC DEBUG: Next node is invalid!\n");

						bot->current_node = -1;
						bot->next_node = -1;
						return;
					}

					if ( nodes[bot->current_node].type & NODE_DUCK )
					{
						bot->node_timeout = level.time + ( NPC_TravelTime( bot) * 2 );	// Duck moves take longer, so...
					}

					// Set our last good move time to now...
					bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
					bot->bot_last_good_move_time = level.time;
					VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
					bot->node_timeout = level.time + NPC_TravelTime( bot );
				}
			}

			// Set up ideal view angles for this enemy...
			VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
			bot->move_vector[ROLL] = 0;
			
			if (/*do_attack_move &&*/ bot->enemy)
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

			//NPCSetViewAngles( bot, 100 );
			//G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

			diff = fabs(AngleDifference(NPC->NPC_client->ps.viewangles[YAW], NPC->bot_ideal_view_angles[YAW]));

			if (diff > 90.0f)
			{// Delay large turns...
				NPCSetViewAngles(NPC, 500);
				VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
				ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
				ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
				ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
			}
			else
			{
				VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
				ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
				ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
				ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
				ucmd->forwardmove = 127;

				//if (nodes[bot->current_node].type == NODE_ICE)
				//	ucmd->forwardmove = 48; // Walk on ice...

				NPC->bot_last_good_move_time = level.time;
			}

			if (VectorLength(NPC->NPC_client->ps.velocity) < 1 && ucmd->upmove == 0)
			{// For bots that get their heads stuck in solids.. This may free them...
				ucmd->upmove = -48;
				NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
			}


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
				if ( !(bot->NPC_client->ps.pm_flags & PMF_DUCKED) )
				{													// For special duck nodes, make the bot duck down...
					ucmd->upmove = -48;
					bot->NPC_client->ps.pm_flags |= PMF_DUCKED;
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
					VectorLength(bot->NPC_client->ps.velocity) < 16 &&
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
					if ( !(bot->NPC_client->ps.pm_flags & PMF_DUCKED) )
					{												// For special duck nodes, make the bot duck down...
						ucmd->upmove = -48;
						bot->NPC_client->ps.pm_flags |= PMF_DUCKED;
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

			if ( nodes[bot->current_node].type & NODE_DUCK && !(bot->NPC_client->ps.pm_flags & PMF_DUCKED) )
			{			// For special duck nodes, make the bot duck down...
				ucmd->upmove = -48;
				bot->NPC_client->ps.pm_flags |= PMF_DUCKED;
			}
			else if ( nodes[bot->last_node].type & NODE_DUCK && !(bot->NPC_client->ps.pm_flags & PMF_DUCKED) )
			{			// For special duck nodes, make the bot duck down...
				ucmd->upmove = -48;
				bot->NPC_client->ps.pm_flags |= PMF_DUCKED;
			}
			else if
				(
					!(nodes[bot->current_node].type & NODE_DUCK) &&
					!(nodes[bot->last_node].type & NODE_DUCK) &&
					!(bot->NPC_client->ps.eFlags & EF_CLOAKED) &&
					!bot->enemy &&
					(bot->NPC_client->ps.eFlags & EF_CROUCHING)
				)
			{			// Get up?
				//ucmd->upmove = 127;
				bot->NPC_client->ps.eFlags &= ~EF_CROUCHING;
			}
			else if
				(
					!(nodes[bot->current_node].type & NODE_DUCK) &&
					!(nodes[bot->last_node].type & NODE_DUCK) &&
					!(bot->NPC_client->ps.eFlags & EF_CLOAKED) &&
					!bot->enemy &&
					(bot->NPC_client->ps.eFlags & EF_PRONE)
				)
			{			// Get up?
				//ucmd->upmove = 127;
				bot->NPC_client->ps.eFlags &= ~EF_PRONE;

				if (bot->NPC_client->ps.eFlags & EF_PRONE_MOVING)
					bot->NPC_client->ps.eFlags &= ~EF_PRONE_MOVING;
			}
			else if
				(
					!(nodes[bot->current_node].type & NODE_DUCK) &&
					!(nodes[bot->last_node].type & NODE_DUCK) &&
					!(bot->NPC_client->ps.eFlags & EF_CLOAKED) &&
					!bot->enemy &&
					(bot->NPC_client->ps.eFlags & EF_PRONE_MOVING)
				)
			{			// Get up?
				//ucmd->upmove = 127;
				bot->NPC_client->ps.eFlags &= ~EF_PRONE_MOVING;

				if (bot->NPC_client->ps.eFlags & EF_PRONE)
					bot->NPC_client->ps.eFlags &= ~EF_PRONE;
			}
			else if ( !bot->enemy && (bot->NPC_client->ps.eFlags & EF_PRONE) )
			{			// Get up?
				//ucmd->upmove = 127;
				bot->NPC_client->ps.eFlags &= ~EF_PRONE;
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
				if ( !(bot->NPC_client->ps.pm_flags & PMF_DUCKED) )
				{												// For special duck nodes, make the bot duck down...
					ucmd->upmove = -48;
					bot->NPC_client->ps.pm_flags |= PMF_DUCKED;
					//bot->bot_duck_time = level.time + 500;
				}
				else if ( VectorLength(bot->NPC_client->ps.velocity) <= 8)
				{// Still not moving, try prone :)
					if (!(bot->NPC_client->ps.eFlags & EF_PRONE) && !(bot->NPC_client->ps.eFlags & EF_PRONE_MOVING))
						bot->NPC_client->ps.eFlags |= EF_PRONE;

					if (bot->NPC_client->ps.pm_flags & PMF_DUCKED)
						bot->NPC_client->ps.pm_flags &= ~PMF_DUCKED;

					if (bot->NPC_client->ps.eFlags & EF_CROUCHING)
						bot->NPC_client->ps.eFlags &= ~EF_CROUCHING;
				}
			}

			if (((node_link_flags & PATH_JUMP) || (nodes[bot->current_node].type & NODE_JUMP))
				&& (VectorDistance(bot->r.currentOrigin, nodes[bot->current_node].origin) < 56 || VectorLength(bot->NPC_client->ps.velocity) < 24) )
			{
				ucmd->upmove = 127;
				//G_Printf("Jump type 3\n");
				//bot->bot_jump_time = level.time + 500;
			}

			if ( nodes[bot->current_node].origin[2] - bot->r.currentOrigin[2] > 8/*24*/ 
				&& !onLadder 
				&& VectorLength(bot->NPC_client->ps.velocity) < 24
				&& !(bot->NPC_client->ps.pm_flags & PMF_DUCKED) )
			{	// Do we need to jump to our next node?
				ucmd->upmove = 127;
				//G_Printf("Jump type 4\n");
			}

			AIMOD_MOVEMENT_CheckAvoidance( bot, ucmd );

#ifdef __BOT_STRAFE__
			if
			(
				bot->bot_strafe_right_timer < level.time &&
				bot->bot_strafe_left_timer < level.time &&
				VectorLength( bot->NPC_client->ps.velocity) < 16 &&
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

			if (VectorLength(bot->NPC_client->ps.velocity) < 1 && ucmd->upmove == 0)
			{// For bots that get their heads stuck in solids.. This may free them...
				ucmd->upmove = -48;
				bot->NPC_client->ps.pm_flags |= PMF_DUCKED;
			}

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
		}
	}
}

extern qboolean SSE_CPU;
extern int			Find_Goal_Enemy ( gentity_t *bot );

#ifdef _WIN32
extern int			bot_path_queue_start_node[MAX_GENTITIES];
extern int			bot_path_queue_goal_node[MAX_GENTITIES];

//===========================================================================
// Routine      : NPC_Queue_New_Path
// Description  : Find a new path. (Now queued for the pathfinding thread...)

/* */
void
NPC_Queue_New_Path ( gentity_t *bot, usercmd_t *ucmd )
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

	bot_path_queue_start_node[bot->s.number] = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );

	if ( bot_path_queue_start_node[bot->s.number] < 0 )
	{
		bot->bot_creating_path = qfalse;
		return;
	}

	bot->goalentity = NULL;
	bot_path_queue_goal_node[bot->s.number] = NPC_FindGoal( bot );

	if ( bot_path_queue_goal_node[bot->s.number] < 0 )
	{													// Find an enemy to head for...
		int best_ent = Find_Goal_Enemy( bot );

		if ( best_ent >= 0 )
		{
			bot->goalentity = &g_entities[best_ent];
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

/* */
void
NPC_Set_New_Path ( gentity_t *bot, usercmd_t *ucmd )
{
	if (SSE_CPU)
	{
		sse_memset( bot->pathlist, NODE_INVALID, ((sizeof(int)) * MAX_PATHLIST_NODES)/8 );
	}
	else
	{
		memset( bot->pathlist, NODE_INVALID, ((sizeof(int)) * MAX_PATHLIST_NODES) );
	}

	bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
	bot->bot_last_good_move_time = level.time;
	bot->bot_last_position_check = level.time;
	VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

	bot->pathsize = 0;

	// Initialize...
	bot->shortTermGoal = -1;
	bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );

	if ( bot->current_node < 0 )
	{
		if (bot_debug.integer)
			G_Printf("NPC DEBUG: %s failed to find a closest node!\n", bot->NPC_client->pers.netname);
		return;
	}

	/*if (bot->followtarget && bot->bot_fireteam_order != FT_ORDER_DEFAULT)
	{// Check our fireteam orders...
		switch ( bot->bot_fireteam_order )
		{
		case FT_ORDER_COVER:
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->bot_fireteam_order_coverspot, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			bot->last_node = bot->current_node - 1;
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			return;
		case FT_ORDER_SUPRESS:
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->bot_fireteam_order_supression_position, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			bot->last_node = bot->current_node - 1;
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			return;
		case FT_ORDER_ASSAULT:
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->bot_fireteam_order_assault_position, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			bot->last_node = bot->current_node - 1;
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			return;
		default:
			// Set new route to our commander!
			bot->goalentity = bot->followtarget;
			bot->longTermGoal = AIMOD_NAVIGATION_FindFollowRouteClosestReachableNodeTo( bot->goalentity, bot->goalentity->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
			bot->followtarget_waypoint_time = level.time;
			bot->last_node = bot->current_node - 1;
			if (Q_TrueRand(0,2) == 2)
			{// Use alt route if possible for every 3rd (randomized) pathfind...
				bot->pathsize = CreateAltPathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			}
			else
			{
				bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			}
			bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
			bot->bot_last_good_move_time = level.time;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			return;
		}
	}*/

	bot->goalentity = NULL;
	//bot->longTermGoal = Unique_FindGoal( bot );
	bot->longTermGoal = NPC_FindGoal( bot );

	if ( bot->longTermGoal < 0 )
	{													// Find an enemy to head for...
		int best_ent = Find_Goal_Enemy( bot );

		if ( best_ent >= 0 )
		{
			bot->goalentity = &g_entities[best_ent];
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->goalentity->r.currentOrigin, NODE_DENSITY, NODEFIND_ALL );
		}
	}

	if ( bot->longTermGoal < 0 )
	{													// Just get us out of trouble...
		bot->goalentity = NULL;
		bot->longTermGoal = Q_TrueRand(0, number_of_nodes-1);
	}

	bot->last_node = bot->current_node - 1;

	if (Q_TrueRand(0,3) == 1)
	{// Use alt route if possible for every 3rd (randomized) pathfind...
		bot->pathsize = CreateAltPathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
	}
	else
	{
		bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
	}

	bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
	bot->node_timeout = level.time + (NPC_TravelTime( bot )*2);
	bot->bot_last_good_move_time = level.time;
	bot->bot_last_position_check = level.time;
	VectorCopy( bot->r.currentOrigin, bot->bot_last_position );

	if (bot_debug.integer)
		G_Printf("NPC DEBUG: %s found a closest node (%i) and path (size: %i)!\n", bot->NPC_client->pers.netname, bot->current_node, bot->pathsize);
}

extern vmCvar_t g_allWeaponsDamageTanks;

qboolean NPC_Enemy_Is_Viable ( gentity_t *bot, gentity_t *enemy )
{
	if (VectorDistance(bot->r.currentOrigin, enemy->r.currentOrigin) > 64)
	{// Check range...
		switch (bot->s.weapon)
		{
		case WP_KNIFE:
			return qfalse; // Too far away for a knife...
		case WP_LUGER:
		case WP_COLT:
		case WP_SILENCED_LUGER:
		case WP_SILENCED_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
		case WP_AKIMBO_SILENCEDLUGER:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_LUGER:
			if (VectorDistance(bot->r.currentOrigin, enemy->r.currentOrigin) > 1024)
				return qfalse; // Too far away for a pistol...
		default:
			break;
		}
	}

	if (enemy->client)
	{
		if (enemy->client->ps.stats[STAT_HEALTH] <= 0
			|| (enemy->client->ps.pm_flags & PMF_LIMBO) 
			|| enemy->client->sess.sessionTeam == TEAM_SPECTATOR
			|| OnSameTeam(bot, enemy))
		{
			return qfalse;
		}

#ifdef __VEHICLES__
		if (bot->client)
		{
			if ((enemy->client->ps.eFlags & EF_VEHICLE) 
				&& bot->client->ps.weapon != WP_PANZERFAUST
				&& bot->client->ps.weapon != WP_SMOKE_MARKER
				&& bot->client->ps.weapon != WP_ARTY
				&& bot->client->ps.weapon != WP_GRENADE_PINEAPPLE
				&& bot->client->ps.weapon != WP_GRENADE_LAUNCHER
				&& !g_allWeaponsDamageTanks.integer)
			{// If we can't hurt the tank, don't try to attack, run away!
				return qfalse;
			}
		}
		else
		{
			if ((enemy->client->ps.eFlags & EF_VEHICLE) 
				&& bot->s.weapon != WP_PANZERFAUST
				&& bot->s.weapon != WP_SMOKE_MARKER
				&& bot->s.weapon != WP_ARTY
				&& bot->s.weapon != WP_GRENADE_PINEAPPLE
				&& bot->s.weapon != WP_GRENADE_LAUNCHER
				&& !g_allWeaponsDamageTanks.integer)
			{// If we can't hurt the tank, don't try to attack, run away!
				return qfalse;
			}
		}
#endif //__VEHICLES__
	}

#ifdef __NPC__
	if (enemy->s.eType == ET_NPC)
	{
		if (enemy->health <= 0 || OnSameTeam(bot, enemy))
		{
			return qfalse;
		}
	}
#endif //__NPC__

	return qtrue;
}

//===========================================================================
// Routine      : NPC_AI_LocateEnemy
// Description  : Scans for enemy (very simple, just picks any visible enemy)

extern qboolean is_fog_kludge_map;
extern float current_fog_view_distance;

qboolean NPC_AI_LocateEnemy(gentity_t *NPC)
{
	int i;
	float weight = 0;
	float bestweight = 0;
	gentity_t *tempent = NULL;
	gentity_t *bot = NPC;

	if (bot->bot_last_visible_enemy 
		&& bot->bot_enemy_visible_time >= level.time 
		&& NPC_Enemy_Is_Viable( bot, bot->bot_last_visible_enemy )
		&& MyVisible(NPC, NPC->bot_last_visible_enemy))
	{
		bot->enemy = bot->bot_last_visible_enemy;
		return qtrue;
	}

	if (is_fog_kludge_map)
	{
		bestweight = current_fog_view_distance;
	}
	else
	{
		bestweight = 4096;
	}

	// START: Quick close check!
	for (i = 0; i < MAX_GENTITIES; i++)
	{
		gentity_t *ent = &g_entities[i];

		if (!ent)
			continue;

		if ( ent->client )
		{
			if ( ent->client->pers.connected != CON_CONNECTED )
				continue;

			if ( ent->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
				continue;								// Don't aim at spectators lol...

			if ( OnSameTeam( bot, ent ) )
				continue;

			if ( ent->client->ps.stats[STAT_HEALTH] <= 0 || ent->health <= 0 )
				continue;

			if ( !NPC_Enemy_Is_Viable( bot, ent ))
				continue;

			weight = VectorDistance( bot->r.currentOrigin, ent->r.currentOrigin );

			if (weight >= bestweight)
				continue;

			if (weight > 128 && !MyVisible(bot, ent))
				continue;

			tempent = ent;
			bestweight = weight;
		}
		else if (ent->s.eType == ET_NPC)
		{
			if (ent->health <= 0)
				continue;

			if (ent->s.eFlags & EF_DEAD)
				continue;

			if (ent->NPC_Class != CLASS_HUMANOID)
				continue; // UQ1: Don't want bots sniping vehicles(planes) lol!!!

			if (OnSameTeam( bot, ent ))
				continue;

			if ( !NPC_Enemy_Is_Viable( bot, ent ))
				continue;

			weight = VectorDistance( bot->r.currentOrigin, ent->r.currentOrigin );

			if (weight >= bestweight)
				continue;

			if (weight > 128 && !MyVisible(bot, ent))
				continue;

			tempent = ent;
			bestweight = weight;
		}
	}

	if (tempent)
	{
		bot->lastEnemy = qtrue;
		bot->enemy = tempent;
		bot->bot_enemy_visible_time = level.time + 1000;
		bot->bot_last_visible_enemy = bot->enemy;
		return qtrue;
	}

	bot->enemy = NULL;
	return ( qfalse );
}

/*
int nextchat[MAX_CLIENTS];
// Check for any generic ingame sounds we might need...
void NPC_Check_VoiceChats (gentity_t *NPC)
{
	int clientNum = NPC->s.clientNum;
	int testclient = 0;

	if (!nextchat[NPC->s.clientNum])// Initialize!
		nextchat[NPC->s.clientNum] = level.time + Q_TrueRand(1000, 10000);

	if (nextchat[NPC->s.clientNum] <= 0)// Initialize!
		nextchat[NPC->s.clientNum] = level.time + Q_TrueRand(1000, 10000);

	while (	testclient < MAX_CLIENTS )
	{
		if (testclient == NPC->s.clientNum)
		{
			testclient++;
			continue;
		}

		if (VectorDistance(g_entities[testclient].r.currentOrigin, NPC->r.currentOrigin) <= 48)
		{// Too close.. Back Off Man!!!
			if (nextchat[NPC->s.clientNum] < level.time)
			{
				nextchat[NPC->s.clientNum] = level.time + Q_TrueRand(8000, 30000);

				if (rand()%10 < 6)//level.time < 5000)
					G_Voice( NPC, NULL, SAY_TEAM, "LetsGo", qfalse );
				else
					G_Voice( NPC, NULL, SAY_TEAM, "Move", qfalse );
			
				break;
			}
		}

		if (g_entities[testclient].is_NPC)
		{
			if (g_entities[testclient].current_node == NPC->current_node)
			{// Too close.. Back Off ManNPC!!!
				if (nextchat[NPC->s.clientNum] < level.time)
				{
					nextchat[NPC->s.clientNum] = level.time + Q_TrueRand(8000, 30000);
					//G_Voice( NPC, NULL, SAY_TEAM, "Move", qfalse );
					if (rand()%10 < 6)//level.time < 5000)
						G_Voice( NPC, NULL, SAY_TEAM, "LetsGo", qfalse );
					else
						G_Voice( NPC, NULL, SAY_TEAM, "Move", qfalse );
					break;
				}
			}
		}

		testclient++;
	}
}
*/

qboolean NPC_Coop_Player_Within_Range ( gentity_t *NPC )
{// In coop, let's save CPU time by only enabling NPCs that are withing range of a player...
	int i;

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		gentity_t *ent = &g_entities[i];

		if (!ent || !ent->client)
			continue;

		if (VectorDistance(NPC->r.currentOrigin, ent->r.currentOrigin) > 2048)
		{
			/*if (VectorDistance(NPC->r.currentOrigin, ent->r.currentOrigin) < 4096)
			{
				if (!MyVisible(ent, NPC))
					continue;
			}
			else*/
				continue;
		}

		return qtrue;
	}

	return qfalse;
}

qboolean NPC_Coop_Check_Wait( gentity_t *NPC );
extern void NPC_AI_Think( gentity_t *NPC );

qboolean NPC_Coop_Check_InUse ( gentity_t *NPC )
{
	qboolean activate = NPC_Coop_Player_Within_Range ( NPC );

	if (!activate)
	{
		//if (NPC->active)
		//	G_Printf("NPC %i deactivated.\n", NPC->s.number);

		NPC->think = NPC_Coop_Check_Wait;
		NPC->active = qfalse;
		return qfalse;
	}
	else
	{
		//if (!NPC->active)
		//	G_Printf("NPC %i activated.\n", NPC->s.number);

		NPC->think = NPC_AI_Think;
		NPC->active = qtrue;
		return qtrue;
	}
}

qboolean NPC_Coop_Check_Wait( gentity_t *NPC )
{
	if ( level.intermissiontime )
	{ // Don't think in level intermission time.
		return qfalse;
	}

	if (NPC->nextthink)
	{
		if (NPC->nextthink > level.time)
			return NPC->active;
	}

	if (!NPC_Coop_Check_InUse(NPC))
	{
		NPC->nextthink = level.time + 1000;
		return qtrue;
	}
	else
	{
		return qfalse;
	}
}

extern qboolean AIMOD_SP_MOVEMENT_Pick_New_Destination ( gentity_t *bot, usercmd_t *ucmd );

void
AIMOD_NPC_SP_MOVEMENT_Move ( gentity_t *NPC, usercmd_t *ucmd )
{// Single Player NPC style movement, walk around a central position...
	if ( !NPC->bot_initialized )
	{// Initialization...
		VectorCopy(NPC->bot_sp_spawnpos, NPC->r.currentOrigin);
		G_SetOrigin(NPC, NPC->r.currentOrigin);
		NPC->bot_initialized = qtrue;

		// Initialize move destination...
		VectorCopy(NPC->r.currentOrigin, NPC->bot_sp_destination);
	}

	if ( HeightDistance(NPC->r.currentOrigin, NPC->bot_sp_spawnpos) > 64 )
	{// Must have fallen down somewhere, set a new spawn pos...
		VectorCopy(NPC->r.currentOrigin, NPC->bot_sp_spawnpos);
	}

	// Actual movement here...
	if ( NPC->bot_stand_guard > level.time )
	{// Stand guard here for a moment...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		NPC->bot_sniping_time = level.time + 1000;
		return;
	}
	else if ( NPC->bot_sniping_time > level.time )
	{// Ready to find a new destination...
		if (NPC->next_sp_move_scan > level.time)
		{// Avoid FPS issues by not looking every frame when stuck!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->bot_sniping_time = level.time + 5000;
			return;
		}
		else if (!AIMOD_SP_MOVEMENT_Pick_New_Destination( NPC, ucmd ))
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->next_sp_move_scan = level.time + 2000+Q_TrueRand(0, 2500);
			return;
		}
	}
	else if (VectorDistanceNoHeight(NPC->r.currentOrigin, NPC->bot_sp_destination) <= 16)
	{// Hit current waypoint...
		NPC->bot_stand_guard = level.time + Q_TrueRand(2000, 6000);
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}
	else if (VectorLength(NPC->NPC_client->ps.velocity) < 12)
	{// Stuck against a wall or something, find a new destination...
		if (SP_NodeVisible(NPC->r.currentOrigin, NPC->bot_sp_destination, NPC->s.number) >= 2)
		{// A door, open it!
			if ( NPC->last_use_time < level.time )
			{// Only use the door once each 2 secs max...
				NPC->last_use_time = level.time + 2000;
				Cmd_Activate_f( NPC );
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;
			}
		}
		else if (NPC->next_sp_move_scan > level.time)
		{// Avoid FPS issues by not looking every frame when stuck!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->bot_sniping_time = level.time + 5000;
			return;
		}
		else if (!AIMOD_SP_MOVEMENT_Pick_New_Destination( NPC, ucmd ))
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->next_sp_move_scan = level.time + 2000+Q_TrueRand(0, 2500);
			return;
		}
	}

	if (!SP_NodeVisible(NPC->r.currentOrigin, NPC->bot_sp_destination, NPC->s.number))
	{// Stop NPCs running into walls!
		if (NPC->next_sp_move_scan > level.time)
		{// Avoid FPS issues by not looking every frame when stuck!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->bot_sniping_time = level.time + 5000;
			return;
		}
		else if (!AIMOD_SP_MOVEMENT_Pick_New_Destination( NPC, ucmd ))
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->next_sp_move_scan = level.time + 2000+Q_TrueRand(0, 2500);
			return;
		}
	}

	// Set up ideal view angles for this destination & move...
	VectorSubtract( NPC->bot_sp_destination, NPC->r.currentOrigin, NPC->move_vector );
	NPC->move_vector[ROLL] = 0;
	vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );
	NPCSetViewAngles( NPC, 100 );
	ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
	ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
	ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
	ucmd->buttons |= BUTTON_WALKING;
	G_UcmdMoveForDir( NPC, ucmd, NPC->move_vector );
}

int NPC_ReloadAnimForWeapon ( int weapon ) {
	switch( weapon ) {
		case WP_GPG40:
		case WP_M7:
			return WEAP_RELOAD2;
#ifdef __EF__
		case WP_30CAL_SET:
#endif //__EF__
		case WP_MOBILE_MG42_SET:
			return WEAP_RELOAD3;
		default:
			return WEAP_RELOAD1;
	}
}

//===========================================================================
// Routine      : NPC_AI_Humanoid_Think
// Description  : Main think function for humanoid NPCs.
void NPC_AI_Humanoid_Think( gentity_t *NPC )
{
	if ( level.intermissiontime )
	{ // Don't think in level intermission time.
		return;
	}

	if (NPC->nextthink)
	{
		if (NPC->nextthink > level.time)
			return;
	}

//	if (!NPC->s.eType)
//		return;

	if (NPC->s.eType != ET_NPC || NPC->s.number < MAX_CLIENTS)
		return;

/*	if( NPC->NPC_Class == CLASS_HUMANOID && IsCoopGametype())
	{// Single player or coop...
		if (NPC_Coop_Check_Wait( NPC ))
			return;
	}*/

/*	if( NPC->NPC_Class == CLASS_HUMANOID 
		&& (NPC->s.dl_intensity == TAG_HOOK || NPC->s.dl_intensity == TAG_HOOK_DRIVER || NPC->s.dl_intensity == TAG_HOOK_GUNNER1 || NPC->s.dl_intensity == TAG_HOOK_GUNNER2) )
	{
		if ( !Q_stricmp(NPC->classname,  "freed") )
		{
			NPC->s.dl_intensity = TAG_UNHOOK;
			
			if (NPC->client)
			{
				NPC->client->ps.eFlags &= ~EF_TAGCONNECT;
				NPC->client->ps.stats[STAT_TAG_CONNECT_INFO] = TAG_UNHOOK;
			}

			G_FreeEntity(NPC);
			NPC->s.eType = 0;
			return;
		}

		if ( !Q_stricmp(g_entities[NPC->s.otherEntityNum].classname,  "freed") )
		{
			NPC->s.dl_intensity = TAG_UNHOOK;
			
			if (NPC->client)
			{
				NPC->client->ps.eFlags &= ~EF_TAGCONNECT;
				NPC->client->ps.stats[STAT_TAG_CONNECT_INFO] = TAG_UNHOOK;
			}

			G_FreeEntity(NPC);
			NPC->s.eType = 0;
			return;
		}

		G_SetAngle(NPC, g_entities[NPC->s.otherEntityNum].r.currentAngles);
		VectorCopy(g_entities[NPC->s.otherEntityNum].r.currentAngles, NPC->r.currentAngles);
		VectorCopy(g_entities[NPC->s.otherEntityNum].r.currentAngles, NPC->s.angles);
		VectorCopy(g_entities[NPC->s.otherEntityNum].r.currentAngles, NPC->NPC_client->ps.viewangles);
		return; // Don't need to think if we're attached!
	}*/

	if (NPC->health <= 0)
	{// Free the NPC's entity... It's dead!
		if ( NPC->NPC_Class == CLASS_HUMANOID)
			NPC->die;
		else
			G_FreeEntity(NPC);
	}

	if ( NPC->NPC_Class == CLASS_HUMANOID 
		/*&& !( (NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER) && !((NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER_ATTACKER) || (NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER_DEFENDER)) )*/ )
	{// Humanoid NPCs can attack...
		qboolean enemy_vis = qfalse;

		/*if (NPC->bot_enemy_visible_time >= level.time)
		{
			enemy_vis = qtrue;
			NPC->bot_last_visible_enemy = NPC->enemy;
		}
		else if (NPC->enemy && MyVisible(NPC, NPC->enemy))
		{
			NPC->bot_enemy_visible_time = level.time + 1000;
			enemy_vis = qtrue;
			NPC->bot_last_visible_enemy = NPC->enemy;
		}
		else if (NPC->enemy && NPC->bot_enemy_visible_time < level.time - 500)
		{
			if (MyVisible(NPC, NPC->enemy))
			{
				NPC->bot_enemy_visible_time = level.time + 1000;
				enemy_vis = qtrue;
				NPC->bot_last_visible_enemy = NPC->enemy;
			}
			else
			{
				NPC->bot_enemy_visible_time = 0;
				NPC->enemy = NULL;
				enemy_vis = qfalse;
			}
		}
		else if (NPC->enemy 
			&& (OnSameTeam(NPC, NPC->enemy) || !NPC_Enemy_Is_Viable( NPC, NPC->enemy )) )
		{
			NPC->bot_enemy_visible_time = 0;
			NPC->enemy = NULL;
			enemy_vis = qfalse;
		}
		else
		{
			NPC->bot_enemy_visible_time = 0;
			NPC->enemy = NULL;
			enemy_vis = qfalse;
		}
		
		if (!NPC->enemy && NPC->next_enemy_check <= level.time)
		{// Look for a new enemy...
			if (NPC_AI_LocateEnemy(NPC))
			{
				NPC_AI_Inform_Team_Of_Enemy( NPC->s.teamNum, NPC, NPC->enemy );
				NPC->bot_enemy_visible_time = level.time + 1000;
				enemy_vis = qtrue;
			}

			NPC->next_enemy_check = level.time + 1000;
		}*/

		if ((!NPC->enemy || NPC->bot_enemy_visible_time <= level.time) && NPC->bot_next_enemy_scan_time <= level.time)
		{
			if ( AIMOD_AI_LocateEnemy( NPC ) )
			{
				NPC->bot_last_visible_enemy = NPC->enemy;
				NPC->bot_enemy_visible_time = level.time + 1000;
				NPC->bot_next_enemy_scan_time = level.time + 1000;
			}
			else
			{
				NPC->bot_last_visible_enemy = NPC->enemy = NULL;
				NPC->bot_enemy_visible_time = 0;
				NPC->bot_next_enemy_scan_time = level.time + 1000;
			}
		}

		if ( nodes_loaded && NPC->current_node && !(NPC->enemy && enemy_vis) )
		{// We have a routed map.. Do some flag capture checks...
			if
			(
				NPC->NPC_client->ps.stats[STAT_FLAG_CAPTURE] > 0 &&
				g_entities[NPC->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != NPC->s.teamNum &&
				NPC->current_node > 0
			)
			{
				vec3_t org;
				usercmd_t *ucmd = &NPC->NPC_client->pers.cmd;

				VectorCopy(g_entities[NPC->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.origin, org);
				org[0]+=64;
				org[1]+=64;

				NPC->bot_initialized = qtrue;

				VectorCopy(org, NPC->bot_sp_spawnpos);
				AIMOD_NPC_SP_MOVEMENT_Move( NPC, ucmd );
				NPCThink( NPC->s.number );
				return;
			}
		}

#ifdef __WEAPON_AIM__
		if (NPC->enemy && BG_Weapon_Is_Aimable(NPC->s.weapon))
		{
			if (VectorDistance(NPC->r.currentOrigin, NPC->enemy->r.currentOrigin) > 1024 && visible(NPC, NPC->enemy))
			{
				NPC->NPC_client->ps.eFlags |= EF_PRONE;
			}
			else if (VectorDistance(NPC->r.currentOrigin, NPC->enemy->r.currentOrigin) > 512)
			{
				NPC->NPC_client->ps.eFlags |= EF_AIMING;
			}
		}

		if (NPC->NPC_client && (!NPC->enemy || VectorDistance(NPC->r.currentOrigin, NPC->enemy->r.currentOrigin) < 512))
		{
			NPC->NPC_client->ps.eFlags &= ~EF_AIMING;
			NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
		}
#endif //__WEAPON_AIM__

		if (NPC->enemy)
		{// Check the current enemy validity...
			gclient_t *client = NULL;

			if (NPC->enemy->client)
				client = NPC->enemy->client;
			else if (NPC->enemy->NPC_client)
				client = NPC->enemy->NPC_client;

			if (NPC->enemy->health <= 0 
				|| (client && client->ps.stats[STAT_HEALTH] <= 0)
				|| (client && client->ps.pm_flags & PMF_LIMBO) 
				|| OnSameTeam(NPC, NPC->enemy) 
				|| NPC->enemy->s.teamNum == TEAM_SPECTATOR
				|| NPC->bot_enemy_visible_time < level.time - 1000)
			{// They are dead... Don't move... Initialize goal so they head back to their spawn position...
				usercmd_t *ucmd = &NPC->NPC_client->pers.cmd;

				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				NPC->enemy = NULL;
				NPC->bot_last_visible_enemy = NULL;
				NPC->bot_enemy_visible_time = 0;
				//NPC->longTermGoal = -1;
				NPC_AI_Move(NPC);
				NPCThink( NPC->s.number );
				return; // next think...
			}
		}

		if (NPC->enemy)
		{// Look at them and attack!
			NPC->node_timeout = level.time + (NPC_TravelTime( NPC )*2);
			NPC->bot_last_good_move_time = level.time;
			NPC->bot_last_position_check = level.time;
			VectorCopy( NPC->r.currentOrigin, NPC->bot_last_position );

			if (NPC->bot_enemy_visible_time >= level.time)
			{// Enemy is visible.. Fire!
				usercmd_t *ucmd = &NPC->NPC_client->pers.cmd;

				NPC->bot_last_position_check = level.time;
				NPC->bot_last_good_move_time = level.time;
				NPC->bot_last_move_fix_time = level.time;

				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;

				if (!NPC->npc_mg42)
				{// Duck for attacking...
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}

				LookAtEnemy(NPC);
				
				if (IsCoopGametype()
					&& NPC->bot_last_visible_enemy != NPC->enemy 
					&& NPC->npc_next_talk <= level.time 
					&& NPC->next_fire_time < level.time )
				{// Yell "HALT!", etc First and animate the bot..
					// sound
					G_AddEvent( NPC, EV_GENERAL_SOUND, G_SoundIndex( va("sound/coop/halt%i.wav", Q_TrueRand(0,18))) );
					NPC->npc_next_talk = level.time + (1000 * Q_TrueRand(10, 30)); // 10->30 secs between talk for each bot.
					NPC->action = level.time;
					NPC->next_fire_time = level.time + 1000;//3000;

					// Use alert anims...
					BG_UpdateConditionValue( NPC->s.number, ANIM_COND_QUERY, qfalse, qfalse );
					BG_UpdateConditionValue( NPC->s.number, ANIM_COND_RELAXING, qfalse, qfalse );
					BG_UpdateConditionValue( NPC->s.number, ANIM_COND_ALERT, qtrue, qfalse );
					//BG_AnimScriptEvent( &NPC->NPC_client->ps, NPC->NPC_client->pers.character->animModelInfo, ANIM_ET_INFORM_FRIENDLY_OF_ENEMY, qfalse, qtrue );
					//ANIM_ET_FIRSTSIGHT
				}
				else
				{// Fire when ready!
					BG_UpdateConditionValue( NPC->s.number, ANIM_COND_QUERY, qfalse, qfalse );
					BG_UpdateConditionValue( NPC->s.number, ANIM_COND_RELAXING, qfalse, qfalse );
					BG_UpdateConditionValue( NPC->s.number, ANIM_COND_ALERT, qfalse, qfalse );

					if (NPC->next_fire_time < level.time)
					{
						int weapon = NPC->s.weapon;

						switch (weapon)
						{
						case WP_MOBILE_MG42_SET:
							weapon = WP_MOBILE_MG42;
							break;
#ifdef __EF__
						case WP_30CAL_SET:
							weapon = WP_30CAL;
							break;
#endif //__EF__
						case WP_GPG40:
							weapon = WP_KAR98;
							break;
						case WP_CARBINE:
							weapon = WP_M7;
							break;
						case WP_GARAND_SCOPE:
							weapon = WP_GARAND;
							break;
						case WP_FG42SCOPE:
							weapon = WP_FG42;
							break;
						case WP_K43_SCOPE:
							weapon = WP_K43;
							break;
						case WP_SILENCED_LUGER:
							weapon = WP_LUGER;
							break;
						case WP_SILENCED_COLT:
							weapon = WP_COLT;
							break;
						case WP_AKIMBO_COLT:
							weapon = WP_COLT;
							break;
						case WP_AKIMBO_LUGER:
							weapon = WP_LUGER;
							break;
						case WP_AKIMBO_SILENCEDCOLT:
							weapon = WP_COLT;
							break;
						case WP_AKIMBO_SILENCEDLUGER:
							weapon = WP_LUGER;
							break;
						default:
							break;
						}

						if (NPC->NPC_shots_fired >= GetAmmoTableData(weapon)->maxclip && GetAmmoTableData(weapon)->maxclip > 0)
						{
							NPC->NPC_client->ps.weapAnim = NPC_ReloadAnimForWeapon(NPC->s.weapon);
							BG_AnimScriptEvent( &NPC->NPC_client->ps, NPC->NPC_client->pers.character->animModelInfo, ANIM_ET_RELOAD, qfalse, qtrue );
							NPC->NPC_client->ps.weaponstate = WEAPON_RELOADING;
							NPC->NPC_reload_timer = level.time + 3000;
							NPC->NPC_shots_fired = 0;
							NPCThink( NPC->s.number );
							G_AddEvent(NPC, EV_FILL_CLIP, -1);
							return;
						}

						if (NPC->NPC_reload_timer > level.time)
						{
							NPC->NPC_shots_fired = 0;
							NPCThink( NPC->s.number );
							return;
						}

						NPC->NPC_shots_fired++;

						if (/*NPC->NPC_client->ps.aiState == AISTATE_COMBAT
							&&*/ VectorDistance(NPC->s.angles, NPC->bot_ideal_view_angles) < 12)
						{// Wait till we are in combt state before actually firing!
							FireWeapon( NPC );
							G_AddEvent( NPC, EV_FIRE_WEAPON, 0 );
						}

						if (NPC->s.weapon == WP_GRENADE_LAUNCHER || NPC->s.weapon == WP_GRENADE_PINEAPPLE)
							NPC->next_fire_time = level.time + 8000;
						else
							NPC->next_fire_time = level.time + GetAmmoTableData(NPC->s.weapon)->nextShotTime;
					}
				}
				
				NPCThink( NPC->s.number );
				return;
			}
			else // not visible.. go back to moving...
			{
				NPC->enemy = NULL;
				NPC->bot_last_visible_enemy = NULL;

				BG_UpdateConditionValue( NPC->s.number, ANIM_COND_QUERY, qfalse, qfalse );
				BG_UpdateConditionValue( NPC->s.number, ANIM_COND_RELAXING, qtrue, qfalse );
				BG_UpdateConditionValue( NPC->s.number, ANIM_COND_ALERT, qfalse, qfalse );
		
#ifdef __WEAPON_AIM__
				if (BG_Weapon_Is_Aimable(NPC->s.weapon))
				{
					if (NPC->NPC_client->ps.eFlags & EF_AIMING)
					{
						NPC->NPC_client->ps.eFlags &= ~EF_AIMING;
					}
				}
#endif //__WEAPON_AIM__

				if (NPC->enemy && NPC->enemy->s.eType == ET_NPC)
				{
					if (NPC->enemy->health <= 0 )
					{// They are dead... Don't move... Initialize goal so they head back to their spawn position...
						NPC->enemy = NULL;
						//NPC->longTermGoal = -1;
					}
				}
				else
				{
					if (NPC->enemy 
						&& (NPC->enemy->health <= 0 || (NPC->enemy->client->ps.pm_flags & PMF_LIMBO) || NPC->enemy->client->sess.sessionTeam == TEAM_SPECTATOR) )
					{// They are dead... Don't move... Initialize goal so they head back to their spawn position...
						NPC->enemy = NULL;
						//NPC->longTermGoal = -1;
					}
				}

				if (!IsCoopGametype() && NPC->action < level.time - 10000)
					NPC->enemy = NULL; // Reset our enemy if we havn't found him in a while...

				NPC_AI_Move(NPC);
			}
		}
		else
			NPC_AI_Move(NPC);

		NPCThink( NPC->s.number );
	}
	else if ( NPC->NPC_Class == CLASS_HUMANOID && (NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER) )
	{// A humanoid fodder NPC...
		NPC_AI_Move(NPC);
		NPCThink( NPC->s.number );
	}
	else // Not a humanoid fodder...
		NPC_AI_Move(NPC);

	if (IsCoopGametype()
		&& NPC->npc_next_idle_talk < level.time
		&& NPC->s.teamNum == TEAM_AXIS)
	{// Idle NPC talking...
	/*	gentity_t *te;

		// sound
		te = G_TempEntity( NPC->r.currentOrigin, EV_GENERAL_SOUND );
		te->s.eventParm = G_SoundIndex( va("sound/idle/idle%i.wav", rand()%28) );*/

		G_AddEvent( NPC, EV_GENERAL_SOUND, G_SoundIndex( va("sound/idle/idle%i.wav", rand()%28)) );
		NPC->npc_next_idle_talk = level.time + 30000 + (Q_TrueRand(0, 30) * 1000); // 30->60 secs at least between talk for each bot.
	}

	// think again baby
	NPC->nextthink = level.time + bot_thinktime.integer/*FRAMETIME*/;
}
#endif //__NPC__

