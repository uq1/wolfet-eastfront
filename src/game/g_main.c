#include "g_local.h"
#include "etpro_mdx.h"

// Include the "External"/"Public" components of AI_Team
#include "../botai/ai_teamX.h"

wordDictionary censorDictionary;
wordDictionary censorNamesDictionary;

level_locals_t	level;

extern void *B_Alloc ( int size );
extern void B_Free ( void *ptr );

#ifdef __ETE__
extern void UQ_DoFogVariation ( void );
#endif //__ETE__

#ifdef __BOT__
qboolean wp_memory_initialized = qfalse;

#ifndef __CUDA__
void AIMod_BOT_Init_Memory ( void )
{
	//while (!BG_IsMemoryInitialized())
	//{
	//}

	if (!wp_memory_initialized)
	{
		nodes = B_Alloc( (sizeof(node_t)+1)*MAX_NODES );
		wp_memory_initialized = qtrue;
	}
}

void AIMod_BOT_Free_Memory ( void )
{
	if (wp_memory_initialized)
	{
		B_Free(nodes);
		wp_memory_initialized = qfalse;
	}
}

#else //__CUDA__
__device__ __host__ void AIMod_BOT_Init_Memory ( void )
{
	size_t size = ((sizeof(node_t)+1)*MAX_NODES); 

	if (!wp_memory_initialized)
	{
		if (CudaAvailable)
		{
			nodes = B_Alloc( size );
			//cudaMalloc(&nodes, size);
			//nodes = malloc(size);
			wp_memory_initialized = qtrue;
		}
		else
		{
			nodes = B_Alloc( size );
			wp_memory_initialized = qtrue;
		}
	}
}

__device__ __host__ void AIMod_BOT_Free_Memory ( void )
{
	if (wp_memory_initialized)
	{
		if (CudaAvailable)
		{
			B_Free(nodes);
			//cudaFree(nodes);
			//free(nodes);
		}
		else
		{
			B_Free(nodes);
			wp_memory_initialized = qfalse;
		}
	}
}
#endif //__CUDA__
#endif //__BOT__

/*
// NPC Fodder.. Currently unused!
#ifdef __NPC__
qboolean npc_wp_memory_initialized = qfalse;

extern node_t	*npc_nodes;
extern node_t	*begin_nodes;

void AIMod_NPC_Init_Memory ( void )
{
	if (!npc_wp_memory_initialized)
	{
		npc_nodes = B_Alloc( (sizeof(node_t)+1)*MAX_NODES );
		begin_nodes = B_Alloc( (sizeof(node_t)+1)*MAX_NODES );
	}
}

void AIMod_NPC_Free_Memory ( void )
{
	if (npc_wp_memory_initialized)
	{
		B_Free(npc_nodes);
		B_Free(begin_nodes);
	}
}
#endif //__NPC__*/

#ifdef __ETE__
int redtickets = 0;
int bluetickets = 0;

vmCvar_t	g_redTickets;
vmCvar_t	g_blueTickets;

int next_flag_check = 0;

int next_endgame_flag_check = 0;

int num_red_flags = 0;
int num_blue_flags = 0;

void AdjustTickets ( void )
{// Count flags and adjust (add to) ticket numbers for each team...
	int total_num_flags = GetNumberOfPOPFlags();
	int red_flags = 0;
	int blue_flags = 0;
	int i;
	int red_original = redtickets;
	int blue_original = bluetickets;

	if (next_endgame_flag_check > level.time)
		return;

	// Check/Record flag numbers info for endgame every 100ms...
	next_endgame_flag_check = level.time + 100;

	for (i=0; i<=total_num_flags;i++)
	{
		if (flag_list[i].flagentity)
		{
			if (flag_list[i].flagentity->s.modelindex == TEAM_AXIS)
				red_flags++;
			if (flag_list[i].flagentity->s.modelindex == TEAM_ALLIES)
				blue_flags++;
		}
	}
	
	// Record how many flags each team owns for endgame checks...
	num_red_flags = red_flags;
	num_blue_flags = blue_flags;

	//G_Printf("%i red flags. %i blue flags. %i flags total.\n", num_red_flags, num_blue_flags, total_num_flags);

	if (next_flag_check > level.time)
		return;

	if (total_num_flags < 4)
		next_flag_check = level.time + 30000;
	else if (total_num_flags < 7)
		next_flag_check = level.time + 60000;
	else
		next_flag_check = level.time + 90000;

	redtickets+=red_flags;
	if (redtickets > g_redTickets.integer)
		redtickets = g_redTickets.integer;

	bluetickets+=blue_flags;
	if (bluetickets > g_blueTickets.integer)
		bluetickets = g_blueTickets.integer;

	if (red_original != redtickets || blue_original != bluetickets)
	{// Transmit if required only...
		trap_SendServerCommand( -1, va("tkt %i %i", redtickets, bluetickets ));
	}
}
#endif

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
	int			modificationCount;	// for tracking changes
	qboolean	trackChange;		// track this variable, and announce if changed
	qboolean	fConfigReset;		// OSP: set this var to the default on a config reset
	qboolean	teamShader;			// track and if changed, update shader state
} cvarTable_t;

gentity_t		g_entities[MAX_GENTITIES];
gclient_t		g_clients[MAX_CLIENTS];

g_campaignInfo_t g_campaigns[MAX_CAMPAIGNS];
int				saveGamePending;	// 0 = no, 1 = check, 2 = loading

mapEntityData_Team_t mapEntityData[2];

#ifdef __BOT__
extern void CheckSequentialNodes ( void );

#ifdef __BOT_FAST_VISIBILITY__
extern void AIMOD_VISIBILITY_LoadVisibilities( void );
#endif //__BOT_FAST_VISIBILITY__

extern void SOD_SPAWN_SaveBots();
extern void Weapon_Syringe(gentity_t *ent);
extern void Bot_Revive ( gentity_t *bot, gentity_t *traceEnt );
extern void BotInitBotGameEntities(void);
extern void SOD_SPAWN_LoadBots();
extern void AIMOD_NODES_SaveNodes( void );
extern void SOD_SPAWN_SaveBots();
extern void BotWaypointRender(void);
extern void BotWaypointRenderRoute(void);
extern void BotCoverSpotRender ( void );
extern void UpdateNodes( void );

void Load_Bot_FirstNames(void);
void Load_Bot_LastNames(void);

extern int no_mount_time[MAX_CLIENTS]; // Don't try to mount emplaced for so many secs...
extern int player_total (void);

extern int axisObjectives[256];
extern int numAxisObjectives;
extern int alliedObjectives[256];
extern int numAlliedObjectives;

int next_bot_check;
int kicktime = 0;
int	nodes_read = 0;
qboolean routes_loaded = qfalse;

int next_satchel_available_time[MAX_CLIENTS]; // Next time covert can use a satchel...

// AIMod
vmCvar_t		bot_minplayers;
vmCvar_t		bot_coop_minplayers;
vmCvar_t		bot_ratio;
vmCvar_t		bot_cpu_use;// This cvar should be a %...
vmCvar_t		bot_skill;
vmCvar_t		bot_node_edit;
vmCvar_t		bot_node_edit_tankonly;
vmCvar_t		bot_auto_waypoint;
//vmCvar_t		bot_forced_auto_waypoint; // This one will force random movement to cover the map with new points...
vmCvar_t		bot_thinktime;
//vmCvar_t		bot_findgoal;
//vmCvar_t		bot_profile;
vmCvar_t		bot_debug;
vmCvar_t		bot_min_snipe_time;
vmCvar_t		bot_fakePing;		// jaquboss
vmCvar_t		bot_allowPlaceSwap; // end
vmCvar_t		bot_coverspot_render;
vmCvar_t		bot_coverspot_enable;
// End of AIMod cvars.
#endif //__BOT__

#ifdef __NPC__
vmCvar_t		npc_minplayers;
vmCvar_t		npc_simplenav;
#endif //__NPC__

#ifdef __ETE__
vmCvar_t	g_ticketPercent;
vmCvar_t	g_redTickets;
vmCvar_t	g_blueTickets;
vmCvar_t	g_redTicketRatio;
vmCvar_t	g_blueTicketRatio;
vmCvar_t	g_realmedic;
vmCvar_t	g_noSelfHeal;
#endif //__ETE__


vmCvar_t	g_gametype;
vmCvar_t	g_fraglimit;
vmCvar_t	g_timelimit;
vmCvar_t	g_friendlyFire;
vmCvar_t	g_password;
vmCvar_t	sv_privatepassword;
vmCvar_t	g_maxclients;
vmCvar_t	g_maxGameClients;
vmCvar_t	g_minGameClients;		// NERVE - SMF
vmCvar_t	g_dedicated;
vmCvar_t	g_speed;
vmCvar_t	g_gravity;
vmCvar_t	g_cheats;
vmCvar_t	g_knockback;
//vmCvar_t	g_quadfactor;
vmCvar_t	g_forcerespawn;
vmCvar_t	g_inactivity;
#ifdef __DEBUGGING__
vmCvar_t	g_debugMove;
vmCvar_t	g_debugDamage;
vmCvar_t	g_debugAlloc;
vmCvar_t	g_debugBullets;	//----(SA)	added
vmCvar_t	g_debugPole;
#endif //__DEBUGGING__
vmCvar_t	g_motd;
#ifdef ALLOW_GSYNC
vmCvar_t	g_synchronousClients;
#endif // ALLOW_GSYNC
vmCvar_t	g_warmup;

// NERVE - SMF
vmCvar_t	g_warmupLatch;
vmCvar_t	g_nextTimeLimit;
vmCvar_t	g_showHeadshotRatio;
vmCvar_t	g_userTimeLimit;
vmCvar_t	g_userAlliedRespawnTime;
vmCvar_t	g_userAxisRespawnTime;
vmCvar_t	g_currentRound;
vmCvar_t	g_noTeamSwitching;
vmCvar_t	g_altStopwatchMode;
vmCvar_t	g_gamestate;
vmCvar_t	g_swapteams;
// -NERVE - SMF

vmCvar_t	g_restarted;
vmCvar_t	g_log;
vmCvar_t	g_logSync;
//vmCvar_t	g_podiumDist;
//vmCvar_t	g_podiumDrop;
vmCvar_t	voteFlags;
vmCvar_t	g_complaintlimit;		// DHM - Nerve
vmCvar_t	g_ipcomplaintlimit;
vmCvar_t	g_filtercams;
vmCvar_t	g_maxlives;				// DHM - Nerve
vmCvar_t	g_maxlivesRespawnPenalty;
vmCvar_t	g_voiceChatsAllowed;	// DHM - Nerve
vmCvar_t	g_alliedmaxlives;		// Xian
vmCvar_t	g_axismaxlives;			// Xian
vmCvar_t	g_fastres;				// Xian
vmCvar_t	g_knifeonly;			// Xian
vmCvar_t	g_enforcemaxlives;		// Xian

vmCvar_t	g_needpass;
vmCvar_t	g_balancedteams;
vmCvar_t	g_doWarmup;
vmCvar_t	g_teamAutoJoin;
vmCvar_t	g_teamForceBalance;
//josh: balance average playerrating between teams
//Can be used in conjuction with g_balancedteams
vmCvar_t	g_teamForceBalance_playerrating;
vmCvar_t	g_banIPs;
vmCvar_t	g_filterBan;
//vmCvar_t	g_rankings;
vmCvar_t	g_smoothClients;
vmCvar_t	pmove_fixed;
vmCvar_t	pmove_msec;

// Rafael
vmCvar_t	g_scriptName;		// name of script file to run (instead of default for that map)

vmCvar_t	g_developer;
#ifndef __ETE__
vmCvar_t	g_userAim;
#endif //__ETE__
vmCvar_t	g_footstepAudibleRange;
// JPW NERVE multiplayer reinforcement times
vmCvar_t		g_redlimbotime;
vmCvar_t		g_bluelimbotime;
// charge times for character class special weapons
vmCvar_t		g_medicChargeTime;
vmCvar_t		g_engineerChargeTime;
vmCvar_t		g_LTChargeTime;
vmCvar_t		g_soldierChargeTime;
// screen shakey magnitude multiplier

// Gordon
vmCvar_t		g_antilag;

// Jaquboss
vmCvar_t		g_tankSpawnDelay;

// OSP
vmCvar_t		g_spectatorInactivity;
vmCvar_t		match_latejoin;
vmCvar_t		match_minplayers;
vmCvar_t		match_mutespecs;
vmCvar_t		match_readypercent;
vmCvar_t		match_timeoutcount;
vmCvar_t		match_timeoutlength;
vmCvar_t		match_warmupDamage;
vmCvar_t		server_autoconfig;
vmCvar_t		team_maxPanzers;
vmCvar_t		team_maxMG42s;
vmCvar_t		team_maxFlamers;
vmCvar_t		team_maxMortars;
vmCvar_t		team_maxGrenLaunchers;
vmCvar_t		team_maxplayers;
vmCvar_t		team_nocontrols;
vmCvar_t		server_motd0;
vmCvar_t		server_motd1;
vmCvar_t		server_motd2;
vmCvar_t		server_motd3;
vmCvar_t		server_motd4;
vmCvar_t		server_motd5;
vmCvar_t		vote_allow_comp;
vmCvar_t		vote_allow_gametype;
vmCvar_t		vote_allow_kick;
vmCvar_t		vote_allow_map;
vmCvar_t		vote_allow_matchreset;
vmCvar_t		vote_allow_mutespecs;
vmCvar_t		vote_allow_nextmap;
vmCvar_t		vote_allow_pub;
vmCvar_t		vote_allow_referee;
vmCvar_t		vote_allow_shuffleteamsxp;
vmCvar_t		vote_allow_swapteams;
vmCvar_t		vote_allow_friendlyfire;
vmCvar_t		vote_allow_timelimit;
vmCvar_t		vote_allow_warmupdamage;
vmCvar_t		vote_allow_antilag;
vmCvar_t		vote_allow_balancedteams;
vmCvar_t		vote_allow_muting;
vmCvar_t		vote_allow_surrender;
vmCvar_t		vote_allow_restartcampaign;
vmCvar_t		vote_allow_nextcampaign;
vmCvar_t		vote_allow_poll;
vmCvar_t		vote_allow_maprestart;
vmCvar_t		vote_allow_shufflenorestart;
vmCvar_t		vote_limit;
vmCvar_t		vote_percent;
vmCvar_t		z_serverflags;

// forty - airstrike block
vmCvar_t g_asblock;

vmCvar_t		g_covertopsChargeTime;
vmCvar_t		refereePassword;
#ifdef __DEBUGGING__
vmCvar_t		g_debugConstruct;
#endif //__DEBUGGING__
vmCvar_t		g_landminetimeout;

// Variable for setting the current level of debug printing/logging
// enabled in bot scripts and regular scripts.
// Added by Mad Doctor I, 8/23/2002
vmCvar_t		g_scriptDebugLevel;
vmCvar_t		g_movespeed;

vmCvar_t g_axismapxp;
vmCvar_t g_alliedmapxp;
vmCvar_t g_oldCampaign;
vmCvar_t g_currentCampaign;
vmCvar_t g_currentCampaignMap;

// Arnout: for LMS
vmCvar_t g_axiswins;
vmCvar_t g_alliedwins;
vmCvar_t g_lms_teamForceBalance;
vmCvar_t g_lms_roundlimit;
vmCvar_t g_lms_matchlimit;
vmCvar_t g_lms_currentMatch;
vmCvar_t g_lms_lockTeams;
vmCvar_t g_lms_followTeamOnly;

#ifdef SAVEGAME_SUPPORT
vmCvar_t		g_reloading;
#endif // SAVEGAME_SUPPORT

vmCvar_t		url;

vmCvar_t		g_letterbox;
vmCvar_t		bot_enable;

#ifdef __DEBUGGING__
vmCvar_t		g_debugSkills;
#endif //__DEBUGGING__
vmCvar_t		g_heavyWeaponRestriction;
vmCvar_t		g_autoFireteams;

vmCvar_t		g_nextmap;
vmCvar_t		g_nextcampaign;

vmCvar_t		g_disableComplaints;

 // tjw: non config
 vmCvar_t g_reset;
  
 // tjw
 vmCvar_t g_shrubbot;
 vmCvar_t g_hitsounds;
 vmCvar_t g_hitsound_default;
 vmCvar_t g_hitsound_helmet;
 vmCvar_t g_hitsound_head;
 vmCvar_t g_hitsound_team_warn_allies;
 vmCvar_t g_hitsound_team_warn_axis;
 vmCvar_t g_hitsound_team_helmet;
 vmCvar_t g_hitsound_team_head;
 vmCvar_t g_hitsound_team_default;
 vmCvar_t g_playDead;
 vmCvar_t g_shove;
 vmCvar_t g_dragCorpse;
 vmCvar_t g_classChange;
 vmCvar_t g_forceLimboHealth;
 vmCvar_t g_privateMessages;
 vmCvar_t g_XPSave;
 vmCvar_t g_XPSaveFile;
 vmCvar_t g_XPSaveMaxAge_xp;
vmCvar_t g_XPSaveMaxAge;
 vmCvar_t g_weapons; // see WPF_ defines
 vmCvar_t g_goomba;
 vmCvar_t g_spawnInvul;
 vmCvar_t g_spinCorpse;
 vmCvar_t g_teamChangeKills;
 vmCvar_t g_activeTeamBalance;
 vmCvar_t g_ATB;
 vmCvar_t g_ATB_diff;
 vmCvar_t g_ATB_minXP;
 vmCvar_t g_ATB_alliedHoldoff;
 vmCvar_t g_ATB_axisHoldoff;
 vmCvar_t g_ATB_minPlayers;
 vmCvar_t g_ATB_swap;
 vmCvar_t g_ATB_rating; // see ATBR_ defines
 vmCvar_t g_logAdmin;
 vmCvar_t g_maxTeamLandmines;
 vmCvar_t g_mapConfigs;
 vmCvar_t g_packDistance;
 vmCvar_t g_tossDistance;
 vmCvar_t g_dropHealth;
 vmCvar_t g_dropAmmo;
 vmCvar_t g_intermissionTime;
 vmCvar_t g_intermissionReadyPercent;
 vmCvar_t g_hitboxes; // see HBF_ defines
 vmCvar_t g_debugHitboxes;
 vmCvar_t g_voting; // see VF_ defines
 vmCvar_t g_moverScale;
 vmCvar_t g_ammoCabinetTime;
 vmCvar_t g_healthCabinetTime;
 vmCvar_t g_spectator; // see SPECF_ defines
 vmCvar_t g_maxWarp;
 vmCvar_t g_dropObj;
 vmCvar_t g_serverInfo; // see SIF_ defines
 vmCvar_t g_goombaFlags; // see GBF_ defines
 vmCvar_t g_shoveSound;
 vmCvar_t g_shoveNoZ;
 vmCvar_t g_stats; // see STATF_ defines
 vmCvar_t g_poisonSound;
 vmCvar_t g_poisonFlags; // see POISF_ defines
 vmCvar_t g_tyranny;
 vmCvar_t g_mapScriptDirectory;
 vmCvar_t g_campaignFile;
 vmCvar_t g_fightSound;
 vmCvar_t g_fear;
 vmCvar_t g_obituary;
 vmCvar_t g_coverts;
 vmCvar_t g_shortcuts;
 vmCvar_t g_shuffle_rating; // see SHUFR_ defines
 vmCvar_t g_XPDecay; // see XPDF_ defines
 vmCvar_t g_XPDecayRate;
 vmCvar_t g_XPDecayFloor;
 vmCvar_t g_maxXP;
 
 // Josh
 vmCvar_t g_logOptions;
 vmCvar_t g_censor;
 vmCvar_t g_censorNames;
 vmCvar_t g_censorPenalty;
 vmCvar_t g_censorMuteTime;
 vmCvar_t g_killRating;
 vmCvar_t g_playerRating;
 vmCvar_t g_playerRating_mapPad;
 vmCvar_t g_playerRating_minplayers;
 vmCvar_t g_skills;
 vmCvar_t g_misc;
 vmCvar_t g_doubleJumpHeight; // see MISC_* defines (some in bg_public.h)
 //unlagged server options
 vmCvar_t	sv_fps;
 vmCvar_t	g_skipCorrection;
 vmCvar_t	g_truePing;
 //unlagged server options
 
 // dvl
 vmCvar_t g_slashKill;
 
 // Michael
 vmCvar_t g_skillSoldier;
 vmCvar_t g_skillMedic;
 vmCvar_t g_skillEngineer;
 vmCvar_t g_skillFieldOps;
 vmCvar_t g_skillCovertOps;
 vmCvar_t g_skillBattleSense;
 vmCvar_t g_skillLightWeapons;
 
 // matt
 vmCvar_t g_teamDamageRestriction;
 vmCvar_t g_minHits;
 vmCvar_t g_autoTempBan;
 vmCvar_t g_autoTempBanTime;
 vmCvar_t g_poison;
 vmCvar_t g_medics;
 vmCvar_t g_alliedSpawnInvul;
 vmCvar_t g_axisSpawnInvul;
 vmCvar_t g_msgs;
 vmCvar_t g_msgpos;
 vmCvar_t g_throwableKnives;
 vmCvar_t g_maxKnives;
 vmCvar_t g_knifeDamage;
 vmCvar_t g_throwKnifeWait;
 
 vmCvar_t team_maxMedics;
 vmCvar_t team_maxEngineers;
 vmCvar_t team_maxFieldOps;
 vmCvar_t team_maxCovertOps;
 
 // forty - min air strike and arty intervals
 vmCvar_t g_minAirstrikeTime;
 vmCvar_t g_minArtyTime; 
 
 // forty - constructible xp sharing
 vmCvar_t g_constructiblexpsharing;
 
 // forty - realistic hitboxes
 vmCvar_t g_realHead;
 
 // gabriel - medic health regeneration
 vmCvar_t g_medicHealthRegen;
 
 // gabriel - party panzers
 vmCvar_t g_partyPanzersEnabled;
 vmCvar_t g_partyPanzersPattern;
 vmCvar_t g_partyPanzersDamageMode;
 
 // gabriel - panzer modifiers
 vmCvar_t g_panzersVulnerable;
 vmCvar_t g_panzersSpeed;
 vmCvar_t g_panzersGravity;
 
 // forty - sudden death dyno and friends
 vmCvar_t g_dyno;
 
 // forty - canister kicking
 vmCvar_t g_canisterKick;
 
 // perro - Killing Spree
 vmCvar_t g_killingSpree;
 vmCvar_t g_killingSpreePos;
 vmCvar_t g_killingSpreeMsg_tk;
 vmCvar_t g_killingSpreeMsg_end;
 vmCvar_t g_killingSpreeMsg_futility;
 vmCvar_t g_killingSpreeMsg_default;
 
 // perro - tunable weapons
 vmCvar_t g_dmg;
 vmCvar_t g_dmgKnife;
 vmCvar_t g_dmgSten;
 vmCvar_t g_dmgInfRifle;
 vmCvar_t g_dmgFG42;
 vmCvar_t g_dmgPistol;
 vmCvar_t g_dmgSMG;
 vmCvar_t g_dmgMG42;
 vmCvar_t g_dmgPTRS;//Masteries, antitank rifle has his own damage value
 vmCvar_t g_dmgFG42Scope;
 vmCvar_t g_dmgSniper;
 vmCvar_t g_dmgGrenade;
 vmCvar_t g_dmgGrenadeRadius;
 vmCvar_t g_dmgGLauncher;
 vmCvar_t g_dmgGLauncherRadius;
 vmCvar_t g_dmgLandmine;
 vmCvar_t g_dmgLandmineRadius;
 vmCvar_t g_dmgSatchel;
 vmCvar_t g_dmgSatchelRadius;
 vmCvar_t g_dmgPanzer;
 vmCvar_t g_dmgPanzerRadius;
 vmCvar_t g_dmgMortar;
 vmCvar_t g_dmgMortarRadius;
 vmCvar_t g_dmgDynamite;
 vmCvar_t g_dmgDynamiteRadius;
 vmCvar_t g_dmgArty;
 vmCvar_t g_dmgArtyRadius;
 vmCvar_t g_dmgAir;
 vmCvar_t g_dmgAirRadius;
 vmCvar_t g_dmgMG;
 vmCvar_t g_dmgFlamer;
 vmCvar_t g_dmgHeadShotMin;
 vmCvar_t g_dmgHeadShotRatio;
 
 // perro - Stamina nudge
 vmCvar_t g_staminaRecharge;
 
 // perro - reflect friendly fire
 vmCvar_t g_reflectFriendlyFire;
 vmCvar_t g_reflectFFWeapons;
 vmCvar_t g_friendlyFireOpts;
 
 // perro - Damage-based XP
 vmCvar_t g_damageXP;
 vmCvar_t g_damageXPLevel;

vmCvar_t		g_hunkMegs;

vmCvar_t		s_radioStation;

#ifdef __DEBUGGING__
vmCvar_t	aicast_debug;
vmCvar_t	aicast_debugname;
vmCvar_t	aicast_scripts;
#endif //__DEBUGGING__

vmCvar_t		g_camo_axis;
vmCvar_t		g_camo_allies;

vmCvar_t		g_camo_allies_default;
vmCvar_t		g_camo_axis_default;

vmCvar_t		g_dropSecondaryWeapons;
vmCvar_t		g_dropGrenades;

#ifdef __VEHICLES__
vmCvar_t		g_allWeaponsDamageTanks;
#endif //__VEHICLES__

vmCvar_t		g_realism;

cvarTable_t		gameCvarTable[] = {
	// don't override the cheat state set by the system
	{ &g_cheats, "sv_cheats", "", 0, qfalse },

	// noset vars
	{ NULL, "gamename", GAMEVERSION , CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },
	{ NULL, "gamedate", __DATE__ , CVAR_ROM, 0, qfalse  },
	{ &g_restarted, "g_restarted", "0", CVAR_ROM, 0, qfalse  },
	{ &g_reset, "g_reset", "0", CVAR_ROM, 0, qfalse  },
	{ NULL, "sv_mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },

	// latched vars
	{ &g_gametype, "g_gametype", "4", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse  },		// Arnout: default to GT_WOLF_CAMPAIGN

// JPW NERVE multiplayer stuffs
	{ &g_redlimbotime, "g_redlimbotime", "30000", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
	{ &g_bluelimbotime, "g_bluelimbotime", "30000", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
	{ &g_medicChargeTime, "g_medicChargeTime", "45000", CVAR_LATCH, 0, qfalse, qtrue },
	{ &g_engineerChargeTime, "g_engineerChargeTime", "30000", CVAR_LATCH, 0, qfalse, qtrue },
	{ &g_LTChargeTime, "g_LTChargeTime", "40000", CVAR_LATCH, 0, qfalse, qtrue },
	{ &g_soldierChargeTime, "g_soldierChargeTime", "20000", CVAR_LATCH, 0, qfalse, qtrue },
// jpw

	{ &g_covertopsChargeTime, "g_covertopsChargeTime", "30000", CVAR_LATCH, 0, qfalse, qtrue },
	{ &g_landminetimeout, "g_landminetimeout", "1", CVAR_ARCHIVE, 0, qfalse, qtrue },

	{ &g_maxclients, "sv_maxclients", "20", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },			// NERVE - SMF - made 20 from 8
	{ &g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },
	{ &g_minGameClients, "g_minGameClients", "8", CVAR_SERVERINFO, 0, qfalse  },								// NERVE - SMF

	// change anytime vars
	{ &g_fraglimit, "fraglimit", "0", /*CVAR_SERVERINFO |*/ CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },

#ifdef ALLOW_GSYNC
	{ &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO | CVAR_CHEAT, 0, qfalse  },
#endif // ALLOW_GSYNC

	{ &g_friendlyFire, "g_friendlyFire", "1", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue, qtrue },

	{ &g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE  },							// NERVE - SMF - merge from team arena

	{ &g_warmup, "g_warmup", "60", CVAR_ARCHIVE, 0, qtrue  },
	{ &g_doWarmup, "g_doWarmup", "0", CVAR_ARCHIVE, 0, qtrue  },

	// NERVE - SMF
	{ &g_warmupLatch, "g_warmupLatch", "1", 0, 0, qfalse },

	{ &g_nextTimeLimit, "g_nextTimeLimit", "0", CVAR_WOLFINFO, 0, qfalse  },
	{ &g_currentRound, "g_currentRound", "0", CVAR_WOLFINFO, 0, qfalse, qtrue },
	{ &g_altStopwatchMode, "g_altStopwatchMode", "0", CVAR_ARCHIVE, 0, qtrue, qtrue },
	{ &g_gamestate, "gamestate", "-1", CVAR_WOLFINFO | CVAR_ROM, 0, qfalse  },

	{ &g_noTeamSwitching, "g_noTeamSwitching", "0", CVAR_ARCHIVE, 0, qtrue  },

	{ &g_showHeadshotRatio, "g_showHeadshotRatio", "0", 0, 0, qfalse  },

	{ &g_userTimeLimit, "g_userTimeLimit", "0", 0, 0, qfalse, qtrue },
	{ &g_userAlliedRespawnTime, "g_userAlliedRespawnTime", "0", 0, 0, qfalse, qtrue },
	{ &g_userAxisRespawnTime, "g_userAxisRespawnTime", "0", 0, 0, qfalse, qtrue },
	
	{ &g_swapteams, "g_swapteams", "0", CVAR_ROM, 0, qfalse, qtrue },
	// -NERVE - SMF

	{ &g_log, "g_log", "", CVAR_ARCHIVE, 0, qfalse },
	{ &g_logSync, "g_logSync", "0", CVAR_ARCHIVE, 0, qfalse },

	{ &g_password, "g_password", "none", CVAR_USERINFO, 0, qfalse },
	{ &sv_privatepassword, "sv_privatepassword", "", CVAR_TEMP, 0, qfalse },
	{ &g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, 0, qfalse },
	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=500
	{ &g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, 0, qfalse },

	{ &g_dedicated, "dedicated", "0", 0, 0, qfalse },

	{ &g_speed, "g_speed", "320", 0, 0, qtrue, qtrue },
	{ &g_gravity, "g_gravity", "800", 0, 0, qtrue, qtrue },
	{ &g_knockback, "g_knockback", "1000", 0, 0, qtrue, qtrue },
//	{ &g_quadfactor, "g_quadfactor", "3", 0, 0, qtrue },
	
	{ &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qtrue },
	{ &g_balancedteams, "g_balancedteams", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qtrue },
	{ &g_forcerespawn, "g_forcerespawn", "0", 0, 0, qtrue },
	{ &g_forcerespawn, "g_forcerespawn", "0", 0, 0, qtrue },
	{ &g_inactivity, "g_inactivity", "0", 0, 0, qtrue },
#ifdef __DEBUGGING__
	{ &g_debugMove, "g_debugMove", "0", 0, 0, qfalse },
	{ &g_debugDamage, "g_debugDamage", "0", CVAR_CHEAT, 0, qfalse },
	{ &g_debugAlloc, "g_debugAlloc", "0", 0, 0, qfalse },
//	{ &g_debugBullets, "g_debugBullets", "0", CVAR_CHEAT, 0, qfalse},	//----(SA)	added
	{ &g_debugBullets, "g_debugBullets", "0", 0, 0, qfalse},
	{ &g_debugPole, "g_debugPole", "128", CVAR_ARCHIVE, 0, qtrue},
#endif //__DEBUGGING__
	{ &g_motd, "g_motd", "", CVAR_ARCHIVE, 0, qfalse },

//	{ &g_podiumDist, "g_podiumDist", "80", 0, 0, qfalse },
//	{ &g_podiumDrop, "g_podiumDrop", "70", 0, 0, qfalse },

	{ &voteFlags, "voteFlags", "0", CVAR_TEMP|CVAR_ROM|CVAR_SERVERINFO, 0, qfalse },

	{ &g_complaintlimit, "g_complaintlimit", "6", CVAR_ARCHIVE, 0, qtrue },						// DHM - Nerve
	{ &g_ipcomplaintlimit, "g_ipcomplaintlimit", "3", CVAR_ARCHIVE, 0, qtrue },
	{ &g_filtercams, "g_filtercams", "0", CVAR_ARCHIVE, 0, qfalse },
	{ &g_maxlives, "g_maxlives", "0", CVAR_ARCHIVE|CVAR_LATCH|CVAR_SERVERINFO, 0, qtrue },		// DHM - Nerve
	{ &g_maxlivesRespawnPenalty, "g_maxlivesRespawnPenalty", "0", CVAR_ARCHIVE|CVAR_LATCH|CVAR_SERVERINFO, 0, qtrue },
	{ &g_voiceChatsAllowed, "g_voiceChatsAllowed", "4", CVAR_ARCHIVE, 0, qfalse },				// DHM - Nerve

	{ &g_alliedmaxlives, "g_alliedmaxlives", "0", CVAR_LATCH|CVAR_SERVERINFO, 0, qtrue },		// Xian
	{ &g_axismaxlives, "g_axismaxlives", "0", CVAR_LATCH|CVAR_SERVERINFO, 0, qtrue },			// Xian
	{ &g_fastres, "g_fastres", "0", CVAR_ARCHIVE, 0, qtrue, qtrue },							// Xian - Fast Medic Resing
	{ &g_knifeonly, "g_knifeonly", "0", 0, 0, qtrue },											// Xian - Fast Medic Resing
	{ &g_enforcemaxlives, "g_enforcemaxlives", "0", CVAR_ARCHIVE, 0, qtrue },					// Xian - Gestapo enforce maxlives stuff by temp banning

	{ &g_developer, "developer", "0", CVAR_TEMP, 0, qfalse },
//	{ &g_rankings, "g_rankings", "0", 0, 0, qfalse },
#ifndef __ETE__
	{ &g_userAim, "g_userAim", "1", CVAR_CHEAT, 0, qfalse },
#endif //__ETE__
	{ &g_smoothClients, "g_smoothClients", "1", 0, 0, qfalse },
	{ &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, 0, qfalse },
	{ &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, 0, qfalse },

	{ &g_footstepAudibleRange, "g_footstepAudibleRange", "256", CVAR_CHEAT, 0, qfalse },

	{ &g_scriptName, "g_scriptName", "", CVAR_CHEAT, 0, qfalse },

	{ &g_antilag, "g_antilag", "1", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },

 	{ NULL, "Players_Axis", "", CVAR_ROM, 0, qfalse, qfalse },
 	{ NULL, "Players_Allies", "", CVAR_ROM, 0, qfalse, qfalse },
 	//bani - #184
 	{ NULL, "P", "", CVAR_ROM, 0, qfalse, qfalse },
 
 	{ NULL, "campaign_maps", "", CVAR_ROM, 0, qfalse, qfalse },
 	{ NULL, "C", "", CVAR_ROM, 0, qfalse, qfalse },

	{ &g_tankSpawnDelay, "g_tankSpawnDelay", "15000", CVAR_ARCHIVE, 0, qfalse },


//	//bani - #184
//	{ NULL, "P", "", CVAR_SERVERINFO_NOUPDATE, 0, qfalse, qfalse },

	{ &refereePassword, "refereePassword", "none", 0, 0, qfalse},
	{ &g_spectatorInactivity, "g_spectatorInactivity", "0", 0, 0, qfalse, qfalse },
	{ &match_latejoin,		"match_latejoin", "1", 0, 0, qfalse, qfalse },
	{ &match_minplayers,	"match_minplayers", MATCH_MINPLAYERS, 0, 0, qfalse, qfalse },
	{ &match_mutespecs,		"match_mutespecs", "0", 0, 0, qfalse, qtrue },
	{ &match_readypercent,	"match_readypercent", "100", 0, 0, qfalse, qtrue },
	{ &match_timeoutcount,	"match_timeoutcount", "3", 0, 0, qfalse, qtrue },
	{ &match_timeoutlength,	"match_timeoutlength", "180", 0, 0, qfalse, qtrue },
	{ &match_warmupDamage,	"match_warmupDamage", "1", 0, 0, qfalse },
	{ &server_autoconfig, "server_autoconfig", "0", 0, 0, qfalse, qfalse },
	{ &server_motd0,	"server_motd0", " ^NEnemy Territory ^7MOTD ", 0, 0, qfalse, qfalse },
	{ &server_motd1,	"server_motd1", "", 0, 0, qfalse, qfalse },
	{ &server_motd2,	"server_motd2", "", 0, 0, qfalse, qfalse },
	{ &server_motd3,	"server_motd3", "", 0, 0, qfalse, qfalse },
	{ &server_motd4,	"server_motd4", "", 0, 0, qfalse, qfalse },
	{ &server_motd5,	"server_motd5", "", 0, 0, qfalse, qfalse },
	{ &team_maxPanzers, "team_maxPanzers", "-1", 0, 0, qfalse, qfalse },
	{ &team_maxMG42s, "team_maxMG42s", "-1", 0, 0, qfalse, qfalse },
	{ &team_maxFlamers, "team_maxFlamers", "-1", 0, 0, qfalse, qfalse },
	{ &team_maxMortars, "team_maxMortars", "-1", 0, 0, qfalse, qfalse },
	{ &team_maxGrenLaunchers, "team_maxGrenLaunchers", "-1", 0, 0, qfalse, qfalse },
	{ &team_maxplayers, "team_maxplayers", "0", 0, 0, qfalse, qfalse },
	{ &team_nocontrols, "team_nocontrols", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_comp,			"vote_allow_comp", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_gametype,		"vote_allow_gametype", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_kick,			"vote_allow_kick", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_map,			"vote_allow_map", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_matchreset,	"vote_allow_matchreset", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_mutespecs,	"vote_allow_mutespecs", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_nextmap,		"vote_allow_nextmap", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_pub,			"vote_allow_pub", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_referee,		"vote_allow_referee", "0", 0, 0, qfalse, qfalse },
	{ &vote_allow_shuffleteamsxp,	"vote_allow_shuffleteamsxp", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_swapteams,	"vote_allow_swapteams", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_friendlyfire,	"vote_allow_friendlyfire", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_timelimit,	"vote_allow_timelimit", "0", 0, 0, qfalse, qfalse },
	{ &vote_allow_warmupdamage,	"vote_allow_warmupdamage", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_antilag,		"vote_allow_antilag", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_balancedteams,"vote_allow_balancedteams", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_muting,		"vote_allow_muting", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_surrender,	"vote_allow_surrender", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_restartcampaign,	"vote_allow_restartcampaign", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_nextcampaign,	"vote_allow_nextcampaign", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_poll,	"vote_allow_poll", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_maprestart,	"vote_allow_maprestart", "1", 0, 0, qfalse, qfalse },
	{ &vote_allow_shufflenorestart,	"vote_allow_shufflenorestart", "1", 0, 0, qfalse, qfalse },

	{ &vote_limit,		"vote_limit", "5", 0, 0, qfalse, qfalse },
	{ &vote_percent,	"vote_percent", "50", 0, 0, qfalse, qfalse },

	// state vars
	{ &z_serverflags, "z_serverflags", "0", 0, 0, qfalse, qfalse },
#ifdef __DEBUGGING__
	{ &g_debugConstruct, "g_debugConstruct", "0", CVAR_CHEAT, 0, qfalse },
#endif //__DEBUGGING__
	{ &g_scriptDebug, "g_scriptDebug", "0", CVAR_CHEAT, 0, qfalse },

	// What level of detail do we want script printing to go to.
	{ &g_scriptDebugLevel, "g_scriptDebugLevel", "0", CVAR_CHEAT, 0, qfalse },

	// How fast do we want Allied single player movement?
//	{ &g_movespeed, "g_movespeed", "127", CVAR_CHEAT, 0, qfalse },
	{ &g_movespeed, "g_movespeed", "76", CVAR_CHEAT, 0, qfalse },

	// Arnout: LMS	
	{ &g_lms_teamForceBalance,	"g_lms_teamForceBalance",	"1",	CVAR_ARCHIVE },
	{ &g_lms_roundlimit,		"g_lms_roundlimit",			"3",	CVAR_ARCHIVE },
	{ &g_lms_matchlimit,		"g_lms_matchlimit",			"2",	CVAR_ARCHIVE },
	{ &g_lms_currentMatch,		"g_lms_currentMatch",		"0",	CVAR_ROM, 0, qfalse, qtrue },
	{ &g_lms_lockTeams,			"g_lms_lockTeams",			"0",	CVAR_ARCHIVE },
	{ &g_lms_followTeamOnly,	"g_lms_followTeamOnly",		"1",	CVAR_ARCHIVE },
	{ &g_axiswins,				"g_axiswins",				"0",	CVAR_ROM, 0, qfalse, qtrue },
	{ &g_alliedwins,			"g_alliedwins",				"0",	CVAR_ROM, 0, qfalse, qtrue },
	{ &g_axismapxp,				"g_axismapxp",				"0",	CVAR_ROM, 0, qfalse, qtrue },
	{ &g_alliedmapxp,			"g_alliedmapxp",			"0",	CVAR_ROM, 0, qfalse, qtrue },

	{ &g_oldCampaign,			"g_oldCampaign",			"",		CVAR_ROM, 0, },
	{ &g_currentCampaign,		"g_currentCampaign",		"",		CVAR_WOLFINFO | CVAR_ROM, 0, },
	{ &g_currentCampaignMap,	"g_currentCampaignMap",		"0",	CVAR_WOLFINFO | CVAR_ROM, 0, },
	

#ifdef SAVEGAME_SUPPORT
	{ &g_reloading, "g_reloading", "0", CVAR_ROM },
#endif // SAVEGAME_SUPPORT

	// configured by the server admin, points to the web pages for the server
	{ &url, "URL", "", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },

	{ &g_letterbox, "cg_letterbox", "0", CVAR_TEMP	},
	{ &bot_enable,	"bot_enable",	"0", 0			},
#ifdef __DEBUGGING__
	{ &g_debugSkills,	"g_debugSkills", "0", 0		},
#endif //__DEBUGGING__
	{ &g_heavyWeaponRestriction, "g_heavyWeaponRestriction", "100", CVAR_ARCHIVE|CVAR_SERVERINFO },
	{ &g_autoFireteams, "g_autoFireteams", "1", CVAR_ARCHIVE },

	{ &g_nextmap, "nextmap", "", CVAR_TEMP },
	{ &g_nextcampaign, "nextcampaign", "", CVAR_TEMP },

	{ &g_disableComplaints, "g_disableComplaints", "0", CVAR_ARCHIVE },

	// tjw
	{ &g_shrubbot, "g_shrubbot", "", 0, 0 },
	{ &g_hitsounds, "g_hitsounds", "0", 0 },
	{ &g_hitsound_default, "g_hitsound_default", "sound/weapons/impact/flesh2.wav", 0 },
	{ &g_hitsound_helmet, "g_hitsound_helmet", "sound/weapons/impact/metal4.wav", 0 },
	{ &g_hitsound_head, "g_hitsound_head", "sound/weapons/impact/flesh4.wav", 0 },
	{ &g_hitsound_team_warn_allies, "g_hitsound_team_warn_allies", "sound/chat/allies/26a.wav", 0 },
	{ &g_hitsound_team_warn_axis, "g_hitsound_team_warn_axis", "sound/chat/axis/26a.wav", 0 },
	{ &g_hitsound_team_helmet, "g_hitsound_team_helmet", "sound/weapons/impact/metal4.wav", 0 },
	{ &g_hitsound_team_head, "g_hitsound_team_head", "sound/weapons/impact/flesh4.wav", 0 },
	{ &g_hitsound_team_default, "g_hitsound_team_default", "sound/weapons/impact/flesh2.wav", 0 },
	{ &g_playDead, "g_playDead", "0", 0 },
	{ &g_shove, "g_shove", "0", 0 },
	{ &g_dragCorpse, "g_dragCorpse", "0", 0 },
	{ &g_classChange, "g_classChange", "0", 0 },
	{ &g_forceLimboHealth, "g_forceLimboHealth", "1", 0 },
	{ &g_privateMessages, "g_privateMessages", "0", 0 },
	{ &g_XPSave, "g_XPSave", "0", 0 },
	{ &g_XPSaveFile, "g_XPSaveFile", "xpsave.cfg", 0 },
	{ &g_XPSaveMaxAge_xp, "g_XPSaveMaxAge_xp", "86400", 0 },
	{ &g_XPSaveMaxAge, "g_XPSaveMaxAge", "604800", 0 },
	{ &g_weapons, "g_weapons", "0", 0 },
	{ &g_goomba, "g_goomba", "0", 0 },
	{ &g_spawnInvul, "g_spawnInvul", "3", 0 },
	{ &g_spinCorpse, "g_spinCorpse", "0", 0 },
	{ &g_teamChangeKills, "g_teamChangeKills", "1", 0 },
	// g_activeTeamBalance is going to be deprecated or removed
	{ &g_activeTeamBalance, "g_activeTeamBalance", "0", 0 },
	{ &g_ATB, "g_ATB", "0", 0 },
	{ &g_ATB_diff, "g_ATB_diff", "75", 0 },
	{ &g_ATB_minXP, "g_ATB_minXP", "300", 0 },
	{ &g_ATB_axisHoldoff, "g_ATB_axisHoldoff", "5", 0 },
	{ &g_ATB_alliedHoldoff, "g_ATB_alliedHoldoff", "5", 0 },
	{ &g_ATB_minPlayers, "g_ATB_minPlayers", "5", 0 },
	{ &g_ATB_swap, "g_ATB_swap", "1", 0 },
	{ &g_ATB_rating, "g_ATB_rating", "3", 0 },
	{ &g_logAdmin, "g_logAdmin", "", 0 },
	{ &g_maxTeamLandmines, "g_maxTeamLandmines", "10", 0 },
	{ &g_mapConfigs, "g_mapConfigs", "", 0 },
	{ &g_packDistance, "g_packDistance", "0", 0 },
	{ &g_tossDistance, "g_tossDistance", "0", 0 },
	{ &g_dropHealth, "g_dropHealth", "0", 0 },
	{ &g_dropAmmo, "g_dropAmmo", "0", 0 },
	{ &g_intermissionTime, "g_intermissionTime", "60", 0 },
	{ &g_intermissionReadyPercent, "g_intermissionReadyPercent", "100", 0 },
	{ &g_hitboxes, "g_hitboxes", "31", 0 },
	// cheat protected this because it will overflow gentities
	// enabled on a full server
	{ &g_debugHitboxes, "g_debugHitboxes", "0", CVAR_CHEAT },
	{ &g_voting, "g_voting", "0", 0 },
	{ &g_moverScale, "g_moverScale", "1.0", 0 },
	{ &g_ammoCabinetTime, "g_ammoCabinetTime", "60000", 0 },
	{ &g_healthCabinetTime, "g_healthCabinetTime", "10000", 0 },
	{ &g_spectator, "g_spectator", "0", 0 },
	{ &g_maxWarp, "g_maxWarp", "4", 0 },
	{ &g_dropObj, "g_dropObj", "0", 0 },
	//{ &g_serverInfo, "g_serverInfo", "1", 0 },
	{ &g_goombaFlags, "g_goombaFlags", "0", 0 },
	{ &g_shoveSound, "g_shoveSound",  "sound/weapons/grenade/gren_throw.wav", 0 },
	{ &g_shoveNoZ, "g_shoveNoZ", "0", 0 },
	{ &g_stats, "g_stats", "0", 0 },
	{ &g_poisonSound, "g_poisonSound", "sound/player/gurp2.wav", 0 },
	{ &g_poisonFlags, "g_poisonFlags", "7", 0 },
	{ &g_tyranny, "g_tyranny", "0", CVAR_ROM|CVAR_SERVERINFO_NOUPDATE },
	{ &g_mapScriptDirectory, "g_mapScriptDirectory", "", 0 },
	{ &g_campaignFile, "g_campaignFile", "", 0 },
	{ &g_fightSound, "g_fightSound", "", 0 },
	{ &g_fear, "g_fear", "2000", 0 },
	{ &g_obituary, "g_obituary", "2", 0 },
	{ &g_coverts, "g_coverts", "0", 0 },
	{ &g_shortcuts, "g_shortcuts", "0", 0 },
	{ &g_shuffle_rating, "g_shuffle_rating", "3", 0 },
	{ &g_XPDecay, "g_XPDecay", "0", 0 },
	{ &g_XPDecayRate, "g_XPDecayRate", "0.0", 0 },
	{ &g_XPDecayFloor, "g_XPDecayFloor", "0", 0 },
	{ &g_maxXP, "g_maxXP", "-1", 0 },

	// Josh
	{ &g_logOptions, "g_logOptions", "0", 0 },
	{ &g_censor, "g_censor", "", 0 },
	{ &g_censorNames, "g_censorNames", "", 0 },
	{ &g_censorPenalty, "g_censorPenalty", "0", 0 },
	{ &g_censorMuteTime, "g_censorMuteTime", "60", 0 },
	{ &g_killRating, "g_killRating", "3", 0 },
	{ &g_playerRating, "g_playerRating", "3", 0 },
	{ &g_playerRating_mapPad, "g_playerRating_mapPad", "50", 0 },
	{ &g_playerRating_minplayers, "g_playerRating_minplayers", "8", 0 },
	{ &g_teamForceBalance_playerrating, "g_teamForceBalance_playerrating", "0", 0 },
	{ &g_skills, "g_skills", "0", 0 },
	{ &g_misc, "g_misc", "0", 0 },
	{ &g_doubleJumpHeight, "g_doubleJumpHeight", "1.4", 0 },
	// dvl
	{ &g_slashKill, "g_slashKill", "0", 0 },
	// Michael
	{ &g_skillSoldier, "skill_soldier", "20 50 90 140", 0},
	{ &g_skillMedic, "skill_medic", "20 50 90 140", 0 },
	{ &g_skillEngineer, "skill_engineer", "20 50 90 140", 0 },
	{ &g_skillFieldOps, "skill_fieldops", "20 50 90 140", 0 },
	{ &g_skillCovertOps, "skill_covertops", "20 50 90 140", 0 },
	{ &g_skillBattleSense, "skill_battlesense", "20 50 90 140", 0 },
	{ &g_skillLightWeapons, "skill_lightweapons", "20 50 90 140", 0 },

// unlagged server options
	{ &sv_fps, "sv_fps", "20", CVAR_SYSTEMINFO, 0, qfalse },
	{ &g_truePing, "g_truePing", "0",  0},
	{ &g_skipCorrection, "g_skipCorrection", "1",  0},
// unlagged server options
	
	// matt
	{ &g_teamDamageRestriction, "g_teamDamageRestriction", "0", 0 },
	{ &g_minHits, "g_minHits", "6", 0 },
	{ &g_autoTempBan, "g_autoTempBan", "0", 0},
	{ &g_autoTempBanTime, "g_autoTempBanTime", "1800", 0 },
	{ &g_poison, "g_poison", "0", 0 },
	{ &g_medics, "g_medics", "0", 0 },
	{ &g_alliedSpawnInvul, "g_alliedSpawnInvul", "0", 0 },
	{ &g_axisSpawnInvul, "g_axisSpawnInvul", "0", 0 },
	{ &g_msgs, "g_msgs", "0", 0 },
	{ &g_msgpos, "g_msgpos", "0", 0 },
	{ &g_throwableKnives, "g_throwableKnives", "0", 0 },
	{ &g_maxKnives, "g_maxKnives", "5", 0 },
	{ &g_knifeDamage, "g_knifeDamage", "35", 0 },
	{ &g_throwKnifeWait, "g_throwKnifeWait", "2", 0 },

	{ &team_maxMedics, "team_maxMedics", "-1", 0 },
	{ &team_maxEngineers, "team_maxEngineers", "-1", 0 },
	{ &team_maxFieldOps, "team_maxFieldOps", "-1", 0 },
	{ &team_maxCovertOps, "team_maxCovertOps", "-1", 0 },

	// forty - min air strike and arty intervals
	{ &g_minAirstrikeTime, "g_minAirstrikeTime",	"10", 0},
	{ &g_minArtyTime, "g_minArtyTime",  "10",	0},

	// forty - constructible xp sharing
	{ &g_constructiblexpsharing, "g_constructiblexpsharing", "0", 0},
	
	// forty - airstrike block
	{ &g_asblock, "g_asblock", "0", 0 },

	// forty - realistic hitboxes
	{ &g_realHead, "g_realHead", "0", 0 },

	// gabriel - medic health regeneration
	{ &g_medicHealthRegen, "g_medicHealthRegen", "0", 0},

	// gabriel - party panzers
	{ &g_partyPanzersEnabled, "g_partyPanzersEnabled", "0", 0},
	{ &g_partyPanzersPattern, "g_partyPanzersPattern", "10001;01010;00100;01010;10001", 0},
	{ &g_partyPanzersDamageMode, "g_partyPanzersDamageMode", "2", 0},

	// gabriel - panzer modifiers
	{ &g_panzersVulnerable, "g_panzersVulnerable", "0", 0},
	{ &g_panzersSpeed, "g_panzersSpeed", "2500", 0},
	{ &g_panzersGravity, "g_panzersGravity", "0", 0},
	
	// forty - sudden death dyno and friends
	{ &g_dyno, "g_dyno", "0", 0 },

	// forty - canister kicking
	{ &g_canisterKick, "g_canisterKick", "0", 0 },
	
	// perro - killing spree
	{ &g_killingSpree, "g_killingSpree", "0", 0},
	{ &g_killingSpreePos, "g_killingSpreePos", "0", 0},
	{ &g_killingSpreeMsg_tk, "g_killingSpreeMsg_tk", 
		"[n]^8's killing spree was stopped by a ^1TEAMKILL^8 "
		"from ^7[a]^8 after ^1[k] ^8kills!",
		0},
	{ &g_killingSpreeMsg_end, "g_killingSpreeMsg_end",
		"[n]^8's killing spree came to an end by "
		"^7[a]^8 after ^1[k] ^8kills.", 
		0},
	{ &g_killingSpreeMsg_futility, "g_killingSpreeMsg_futility",
		"[n] ^8seems to be having a bad day... ^7[d] ^8deaths "
		"without a kill!",
		0},
	
	{ &g_killingSpreeMsg_default, "g_killingSpreeMsg_default",
		"[n] ^8is on a killing spree! ^7[k] ^8kills without a death!",
		0},

	// perro - tunable weapons
	{ &g_dmg, "g_dmg", "0", 0 },
	{ &g_dmgKnife, "g_dmgKnife", "10", 0 },
	{ &g_dmgSten, "g_dmgSten", "14", 0 },
	{ &g_dmgInfRifle, "g_dmgInfRifle", "34", 0 },
	{ &g_dmgFG42, "g_dmgFG42", "15", 0 },
	{ &g_dmgPistol, "g_dmgPistol", "18", 0 },
	{ &g_dmgSMG, "g_dmgSMG", "18", 0 },
	{ &g_dmgMG42, "g_dmgMG42", "18", 0 },
    { &g_dmgPTRS, "g_dmgPTRS", "70", 0 },//Masteries, this is the damage value? from 170 to 70
	{ &g_dmgFG42Scope, "g_dmgFG42Scope", "30", 0 },
	{ &g_dmgSniper, "g_dmgSniper", "50", 0 },
	{ &g_dmgGrenade, "g_dmgGrenade", "250", 0 },
	{ &g_dmgGrenadeRadius, "g_dmgGrenadeRadius", "250", 0 },
	{ &g_dmgGLauncher, "g_dmgGLauncher", "250", 0 },
	{ &g_dmgGLauncherRadius, "g_dmgGLauncherRadius", "250", 0 },
	{ &g_dmgLandmine, "g_dmgLandmine", "250", 0 },
	{ &g_dmgLandmineRadius, "g_dmgLandmineRadius", "250", 0 },
	{ &g_dmgSatchel, "g_dmgSatchel", "250", 0 },
	{ &g_dmgSatchelRadius, "g_dmgSatchelRadius", "250", 0 },
	{ &g_dmgPanzer, "g_dmgPanzer", "400", 0 },
	{ &g_dmgPanzerRadius, "g_dmgPanzerRadius", "300", 0 },
	{ &g_dmgMortar, "g_dmgMortar", "400", 0 },
	{ &g_dmgMortarRadius, "g_dmgMortarRadius", "400", 0 },
	{ &g_dmgDynamite, "g_dmgDynamite", "400", 0 },
	{ &g_dmgDynamiteRadius, "g_dmgDynamiteRadius", "400", 0 },
	{ &g_dmgArty, "g_dmgArty", "400", 0 },
	{ &g_dmgArtyRadius, "g_dmgArtyRadius", "400", 0 },
	{ &g_dmgAir, "g_dmgAir", "400", 0 },
	{ &g_dmgAirRadius, "g_dmgAirRadius", "400", 0 },
	{ &g_dmgMG, "g_dmgMG", "20", 0 },
	{ &g_dmgFlamer, "g_dmgFlamer", "5", 0 },
	{ &g_dmgHeadShotMin, "g_dmgHeadShotMin", "50", 0 },
	{ &g_dmgHeadShotRatio, "g_dmgHeadShotRatio", "2.0", 0 },
	// Perro - Stamina
	{ &g_staminaRecharge, "g_staminaRecharge", "1.0", 0 },
	// Perro - Reflect Friendly Fire
	{ &g_reflectFriendlyFire, "g_reflectFriendlyFire", "0.0", 0 },
	{ &g_reflectFFWeapons, "g_reflectFFWeapons", "31", 0 },
	{ &g_friendlyFireOpts, "g_friendlyFireOpts", "0", 0 },

	{ &g_damageXP, "g_damageXP", "0", 0 },
	{ &g_damageXPLevel, "g_damageXPLevel", "50", 0 },

	{ NULL, "mod_version", __DATE__/*ETE_VERSION*/, CVAR_SERVERINFO | CVAR_ROM },
#ifdef __EF__
	//{ NULL, "mod_url", "http://eastfront.flame-guards.com/", CVAR_SERVERINFO | CVAR_ROM },
	{ NULL, "mod_url", "http://eastfront.co.nr/", CVAR_SERVERINFO | CVAR_ROM },
#else //!__EF__
	{ NULL, "mod_url", "http://ETEnhanced.org", CVAR_SERVERINFO | CVAR_ROM },
#endif //__EF__

#ifdef __BOT__
	{ &bot_minplayers, "bot_minplayers", "0", CVAR_ARCHIVE },
	{ &bot_coop_minplayers, "bot_coop_minplayers", "8", CVAR_ARCHIVE },
	{ &bot_ratio, "bot_ratio", "50", CVAR_ARCHIVE },
	{ &bot_cpu_use, "bot_cpu_use", "100", CVAR_ARCHIVE },// This cvar should be a %...
	{ &bot_skill, "bot_skill", "3", CVAR_ARCHIVE },
	{ &bot_node_edit, "bot_node_edit", "0", CVAR_ARCHIVE },
	{ &bot_node_edit_tankonly, "bot_node_edit_tankonly", "0", CVAR_ARCHIVE },
	{ &bot_auto_waypoint, "bot_auto_waypoint", "0", CVAR_ARCHIVE },
//	{ &bot_forced_auto_waypoint, "bot_forced_auto_waypoint", "0", CVAR_ARCHIVE },
	{ &bot_fakePing, "bot_fakePing", "0", CVAR_ARCHIVE },
	{ &bot_allowPlaceSwap, "bot_allowPlaceSwap", "1", CVAR_ARCHIVE },
	{ &bot_coverspot_render, "bot_coverspot_render", "0", CVAR_ARCHIVE },
	{ &bot_coverspot_enable, "bot_coverspot_enable", "1", CVAR_ARCHIVE },
	{ &bot_debug, "bot_debug", "0", CVAR_ARCHIVE },
	{ &bot_min_snipe_time, "bot_min_snipe_time", "5000", CVAR_ARCHIVE },
#endif //__BOT__

#ifdef __NPC__
	{ &npc_minplayers, "npc_minplayers", "0", CVAR_ARCHIVE },
	{ &npc_simplenav, "npc_simplenav", "0", CVAR_ARCHIVE },
#endif //__NPC__

#ifdef __ETE__
	{ &g_ticketPercent, "g_ticketPercent", "100", CVAR_ARCHIVE /*| CVAR_SERVERINFO*/, 0 , qtrue },

	{ &g_redTickets, "g_redTickets", "200", CVAR_ARCHIVE },
	{ &g_blueTickets, "g_blueTickets", "200", CVAR_ARCHIVE },
	{ &g_redTicketRatio, "g_redTicketRatio", "1", CVAR_ARCHIVE },
	{ &g_blueTicketRatio, "g_blueTicketRatio", "1", CVAR_ARCHIVE },
	{ &g_realmedic, "g_realmedic", "0", CVAR_ARCHIVE },
	{ &g_noSelfHeal, "g_noSelfHeal", "0", CVAR_ARCHIVE },
#endif //__ETE__

#ifdef __MUSIC_ENGINE__
	{ &s_radioStation, "s_radioStation", "", CVAR_SERVERINFO | CVAR_ARCHIVE },
#endif //__MUSIC_ENGINE__

#ifdef __DEBUGGING__
	{ &aicast_debug, "aicast_debug", "0", CVAR_ARCHIVE },
	{ &aicast_debugname, "aicast_debugname", "0", CVAR_ARCHIVE },
	{ &aicast_scripts, "aicast_scripts", "0", CVAR_ARCHIVE },
#endif //__DEBUGGING__

	{ &g_camo_axis, "g_camo_axis", "temperate", CVAR_SERVERINFO /*| CVAR_LATCH*/, 0, qfalse },
	{ &g_camo_allies, "g_camo_allies", "temperate", CVAR_SERVERINFO /*| CVAR_LATCH*/, 0, qfalse },

	{ &g_camo_allies_default, "g_camo_allies_default", "",  0, 0, qfalse },
	{ &g_camo_axis_default, "g_camo_axis_default", "", 0, 0, qfalse },

	{ &g_dropSecondaryWeapons, "g_dropSecondaryWeapons", "1", CVAR_ARCHIVE },
	{ &g_dropGrenades, "g_dropGrenades", "1", CVAR_ARCHIVE },

#ifdef __VEHICLES__
	{ &g_allWeaponsDamageTanks, "g_allWeaponsDamageTanks", "1", CVAR_ARCHIVE },
#endif //__VEHICLES__

	//{ &g_hunkMegs, "g_hunkMegs", "32", CVAR_ARCHIVE },
	{ &g_hunkMegs, "g_hunkMegs", "80", CVAR_ARCHIVE },

	{ &g_realism, "g_realism", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
};

// bk001129 - made static to avoid aliasing
static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[0] );


void G_InitGame( int levelTime, int randomSeed, int restart );
void G_RunFrame( int levelTime );
void G_ShutdownGame( int restart );
void CheckExitRules( void );
// Josh:
void InitCensorStructure( void );
void InitCensorNamesStructure( void );

qboolean G_SnapshotCallback( int entityNum, int clientNum ) {
	gentity_t* ent = &g_entities[ entityNum ];

	if (entityNum > MAX_GENTITIES || !ent)
		return qfalse; // UQ1: et 2.55 hack.. wtf?

	if( ent->s.eType == ET_MISSILE ) {
		if( ent->s.weapon == WP_LANDMINE ) {
			return G_LandmineSnapshotCallback( entityNum, clientNum );
		}
	}

	return qtrue;
}

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export on
#endif
#endif
int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 ) {
#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export off
#endif
#endif

#ifdef __CUDA__
	if (!CudaInitialized)
	{
		InitCUDA();
		CudaInitialized = qtrue;
	}
#endif //__CUDA__

	switch ( command ) {
	case GAME_INIT:
		EnableStackTrace();
		G_InitGame( arg0, arg1, arg2 );
		return 0;
	case GAME_SHUTDOWN:
		G_ShutdownGame( arg0 );
		DisableStackTrace();
		return 0;
	case GAME_CLIENT_CONNECT:
		return (int)ClientConnect( arg0, arg1, arg2 );
	case GAME_CLIENT_THINK:
		ClientThink( arg0 );
		return 0;
	case GAME_CLIENT_USERINFO_CHANGED:
		ClientUserinfoChanged( arg0 );
		return 0;
	case GAME_CLIENT_DISCONNECT:
		ClientDisconnect( arg0 );
		return 0;
	case GAME_CLIENT_BEGIN:
		ClientBegin( arg0 );
		return 0;
	case GAME_CLIENT_COMMAND:
		ClientCommand( arg0 );
		return 0;
	case GAME_RUN_FRAME:
		G_RunFrame( arg0 );
		return 0;
	case GAME_CONSOLE_COMMAND:
 		return ConsoleCommand();
	case BOTAI_START_FRAME:
#ifndef __BOT_AAS__
		return 0;
#else //__BOT_AAS__
		return BotAIStartFrame( arg0 );
#endif //__BOT_AAS__
	case BOT_VISIBLEFROMPOS:
#ifndef __BOT_AAS__
		return qfalse;
#else //__BOT_AAS__
		return BotVisibleFromPos( (float *)arg0, arg1, (float *)arg2, arg3, arg4 );
#endif //__BOT_AAS__
	case BOT_CHECKATTACKATPOS:
#ifndef __BOT_AAS__
		return qfalse;
#else //__BOT_AAS__
		return BotCheckAttackAtPos( arg0, arg1, (float *)arg2, arg3, arg4 );
#endif //__BOT_AAS__
	case GAME_SNAPSHOT_CALLBACK:
		return G_SnapshotCallback( arg0, arg1 );
	case GAME_MESSAGERECEIVED:
		return -1;
	}

	return -1;
}

void QDECL G_Printf( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Printf( text );
}
//bani
void QDECL G_Printf( const char *fmt, ... )_attribute((format(printf,1,2)));

void QDECL G_DPrintf( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	if (!g_developer.integer)
		return;

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Printf( text );
}
//bani
void QDECL G_DPrintf( const char *fmt, ... )_attribute((format(printf,1,2)));

void QDECL G_Error( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Error( text );
}
//bani
void QDECL G_Error( const char *fmt, ... )_attribute((format(printf,1,2)));

/*
==============
G_CursorHintIgnoreEnt: returns whether the ent should be ignored 
for cursor hint purpose (because the ent may have the designed content type
but nevertheless should not display any cursor hint)
==============
*/
static qboolean G_CursorHintIgnoreEnt(gentity_t *traceEnt, gentity_t *clientEnt) {
	return (traceEnt->s.eType == ET_OID_TRIGGER || traceEnt->s.eType == ET_TRIGGER_MULTIPLE) ? qtrue : qfalse;
}

/*
==============
G_CheckForCursorHints
	non-AI's check for cursor hint contacts

	server-side because there's info we want to show that the client
	just doesn't know about.  (health or other info of an explosive,invisible_users,items,etc.)

	traceEnt is the ent hit by the trace, checkEnt is the ent that is being
	checked against (in case the traceent was an invisible_user or something)
	
==============
*/

qboolean G_EmplacedGunIsMountable( gentity_t* ent, gentity_t* other ) {
	if( Q_stricmp( ent->classname, "misc_mg42" ) && Q_stricmp( ent->classname, "misc_aagun" ) ) {
		return qfalse;
	}

	if( !other->client ) {
		return qfalse;
	}

	if( BG_IsScopedWeapon( other->client->ps.weapon ) ) {
		return qfalse;
	}

	if( other->client->ps.pm_flags & PMF_DUCKED ) {
		return qfalse;
	}

	if( other->client->ps.persistant[PERS_HWEAPON_USE] ) {
		return qfalse;
	}

	if( ent->r.currentOrigin[2] - other->r.currentOrigin[2] >= 40 ) {
		return qfalse;
	}
	
	if( ent->r.currentOrigin[2] - other->r.currentOrigin[2] < 0 ) {
		return qfalse;
	}

	if( ent->s.frame != 0 ) {
		return qfalse;
	}

	if( ent->active ) {
		return qfalse;
	}

	if( other->client->ps.grenadeTimeLeft ) {
		return qfalse;
	}

	if( infront( ent, other ) ) {
		return qfalse;
	}

	return qtrue;
}

qboolean G_EmplacedGunIsRepairable( gentity_t* ent, gentity_t* other ) {
	if( Q_stricmp( ent->classname, "misc_mg42" ) && Q_stricmp( ent->classname, "misc_aagun" ) ) {
		return qfalse;
	}

	if( !other->client ) {
		return qfalse;
	}

	if( BG_IsScopedWeapon( other->client->ps.weapon ) ) {
		return qfalse;
	}

	if( other->client->ps.persistant[PERS_HWEAPON_USE] ) {
		return qfalse;
	}

	if( ent->s.frame == 0 ) {
		return qfalse;
	}

	return qtrue;
}

#ifdef __ETE__
extern qboolean UQ_Climbable ( vec3_t org, vec3_t angles, int ignoreEnt );
#endif //__ETE__

void G_CheckForCursorHints( gentity_t *ent ) {
	vec3_t		forward, right, up, offset, end;
	trace_t		*tr;
	float		dist;
	float		chMaxDist = CH_MAX_DIST;
	gentity_t	*checkEnt, *traceEnt = 0;
  	playerState_t *ps;
	int			hintType, hintDist, hintVal;
	qboolean	zooming, indirectHit;	// indirectHit means the checkent was not the ent hit by the trace (checkEnt!=traceEnt)
	int			trace_contents;			// DHM - Nerve
	int			numOfIgnoredEnts = 0;

	if( !ent->client ) {
		return;
	}

	ps = &ent->client->ps;

#ifdef SAVEGAME_SUPPORT
	// don't change anything if reloading.  just set the exit hint
	if( (g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_COOP || g_gametype.integer == GT_NEWCOOP) && g_reloading.integer == RELOAD_NEXTMAP_WAITING ) {
		ps->serverCursorHint = HINT_EXIT;
		ps->serverCursorHintVal = 0;
		return;
	}
#endif // SAVEGAME_SUPPORT

	indirectHit = qfalse;

	zooming = (qboolean)(ps->eFlags & EF_ZOOMING);

	AngleVectors (ps->viewangles, forward, right, up);

	VectorCopy( ps->origin, offset );
	if(ps->viewangles[PITCH] > 30 && (ps->eFlags & EF_CROUCHING))
		offset[2] += 30;
	else
	offset[2] += ps->viewheight;

	// lean
	if( ps->leanf ) {
		VectorMA( offset, ps->leanf, right, offset );
	}

	if( zooming ) {
		VectorMA( offset, CH_MAX_DIST_ZOOM, forward, end );
	} else {
		VectorMA( offset, chMaxDist, forward, end );
	}

	tr = &ps->serverCursorHintTrace;

	trace_contents = (CONTENTS_TRIGGER|CONTENTS_SOLID|CONTENTS_MISSILECLIP|CONTENTS_BODY|CONTENTS_CORPSE);
	trap_Trace( tr, offset, NULL, NULL, end, ps->clientNum, trace_contents );

	// reset all
	hintType	= ps->serverCursorHint		= HINT_NONE;
	hintVal		= ps->serverCursorHintVal	= 0;
	
	dist = VectorDistanceSquared( offset, tr->endpos );

	if( zooming ) {
		hintDist	= CH_MAX_DIST_ZOOM;
	} else {
		hintDist	= chMaxDist;
	}


#ifdef __ETE__
	ent->client->touchEnt = NULL;

	if ( ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER || ps->weapon == WP_PLIERS )
	{
		gentity_t *test_ent = &g_entities[tr->entityNum];

		if((test_ent->s.eType == ET_CONSTRUCTIBLE_SANDBAGS || test_ent->s.eType == ET_CONSTRUCTIBLE_MG_NEST)
			&& (test_ent->s.dl_intensity == SB_UNCONSTRUCTED || test_ent->s.dl_intensity == SB_CONSTRUCTING) )
		{
			if (VectorDistance(test_ent->r.currentOrigin, ent->r.currentOrigin) <= 64)
			{
				ent->client->touchEnt = test_ent;
				ps->serverCursorHint = HINT_CONSTRUCTIBLE;
				ps->serverCursorHintVal = (int)test_ent->s.effect1Time;
				return;
			}
		}
	}
#endif //__ETE__

	// Arnout: building something - add this here because we don't have anything solid to trace to - quite ugly-ish
	if( ent->client->touchingTOI && ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
		gentity_t* constructible;
		if ((constructible = G_IsConstructible( ent->client->sess.sessionTeam, ent->client->touchingTOI ))) {
			ps->serverCursorHint = HINT_CONSTRUCTIBLE;
			ps->serverCursorHintVal = (int)constructible->s.angles2[0];
			return;
		}
	}

	if( ps->stats[ STAT_PLAYER_CLASS ] == PC_COVERTOPS ) {
		if(ent->client->landmineSpottedTime && level.time - ent->client->landmineSpottedTime < 500) {
			ps->serverCursorHint = HINT_LANDMINE;
			ps->serverCursorHintVal	= ent->client->landmineSpotted ? ent->client->landmineSpotted->count2 : 0;
			return;
		}
	}

	if( tr->fraction == 1 ) {
		return;
	}

	traceEnt = &g_entities[tr->entityNum];

#ifdef __VEHICLES__
	if (traceEnt 
		&& (traceEnt->s.eType == ET_VEHICLE || (traceEnt->s.eType == ET_PLAYER && traceEnt->client && (traceEnt->client->ps.eFlags & EF_VEHICLE)))
		&& !(ent->client->ps.eFlags & EF_VEHICLE))
	{
		hintDist = CH_ACTIVATE_DIST;
		hintType = HINT_ACTIVATE;
		return;
	}

	if ( (traceEnt->client && (traceEnt->client->ps.eFlags & EF_VEHICLE) && OnSameTeam(traceEnt, ent)) 
			|| !Q_stricmp (traceEnt->classname, "churchilltank") 
			|| !Q_stricmp (traceEnt->classname, "panzertank") 
			|| !Q_stricmp (traceEnt->classname, "panzerivtank") 
			|| !Q_stricmp (traceEnt->classname, "kv1tank") 
            || !Q_stricmp (traceEnt->classname, "stug3tank")
			|| !Q_stricmp (traceEnt->classname, "tigertank")
			|| !Q_stricmp (traceEnt->classname, "flak88antitank")
			|| !Q_stricmp (traceEnt->classname, "pakantitank")
			|| !Q_stricmp (traceEnt->classname, "panzer2tank")	)
	{
		if ( traceEnt->health < traceEnt->maxhealth )
		{
			ps->serverCursorHint = HINT_CONSTRUCTIBLE;
			ps->serverCursorHintVal = (int)((float)((float)traceEnt->health/(float)traceEnt->maxhealth)*(float)100);
			return;
		}
	}
#endif //__VEHICLES__

	while( G_CursorHintIgnoreEnt(traceEnt, ent) && numOfIgnoredEnts < 10 ) {
		// xkan, 1/9/2003 - we may hit multiple invalid ents at the same point
		// count them to prevent too many loops
		numOfIgnoredEnts++;

		// xkan, 1/8/2003 - advance offset (start point) past the entity to ignore
		VectorMA( tr->endpos, 0.1, forward, offset );

		trap_Trace( tr, offset, NULL, NULL, end, traceEnt->s.number, trace_contents );

		// xkan, 1/8/2003 - (hintDist - dist) is the actual distance in the above
		// trap_Trace call. update dist accordingly.
		dist += VectorDistanceSquared( offset, tr->endpos );
		if(tr->fraction == 1) {
			return;
		}
		traceEnt = &g_entities[tr->entityNum];
	}

	if( tr->entityNum == ENTITYNUM_WORLD ) {
		if ((tr->contents & CONTENTS_WATER) && !(ps->powerups[PW_BREATHER])) {
			hintDist = CH_WATER_DIST;
			hintType = HINT_WATER;
		} else if( (tr->surfaceFlags & SURF_LADDER) && !(ps->pm_flags & PMF_LADDER) ) { // ladder
			hintDist = CH_LADDER_DIST;
			hintType = HINT_LADDER;
#ifdef __ETE__
		/*}  else if( tr->fraction < 1 && !(ps->pm_flags & PMF_LADDER) ) { // UQ1: check for climbing out of water!
			vec3_t offset2;

			VectorCopy( ps->origin, offset2 );
			if(ps->viewangles[PITCH] > 30 && (ps->eFlags & EF_CROUCHING))
				offset2[2] += 30;
			else
				offset2[2] += ps->viewheight;

			if (UQ_Climbable( offset2, ps->viewangles, ent->s.number ))
			{
				hintDist = CH_LADDER_DIST;
				hintType = HINT_LADDER;
			}*/
		} else if(ps->pm_flags & PMF_LADDER) {
			hintDist = CH_LADDER_DIST;
			hintType = HINT_LADDER;
#endif //__ETE__
		}
	} else if( tr->entityNum < MAX_CLIENTS ) {
		// Show medics a syringe if they can revive someone

		if ( traceEnt->client && traceEnt->client->sess.sessionTeam == ent->client->sess.sessionTeam) {
			if(ps->stats[ STAT_PLAYER_CLASS ] == PC_MEDIC && 
				traceEnt->client->ps.pm_type == PM_DEAD && 
				!(traceEnt->client->ps.pm_flags & PMF_LIMBO) && 
				!(traceEnt->client->ps.eFlags & EF_PLAYDEAD)) {
					hintDist	= CH_REVIVE_DIST; 
					hintType	= HINT_REVIVE;
			}
		} else if(traceEnt->client && traceEnt->client->isCivilian) {
			// xkan, 1/6/2003 - check for civilian, show neutral cursor (no matter which team)		
			hintType = HINT_PLYR_NEUTRAL;
			hintDist = CH_FRIENDLY_DIST;	// far, since this will be used to determine whether to shoot bullet weaps or not
		}
	} else {
		checkEnt = traceEnt;

		// Arnout: invisible entities don't show hints
		if( traceEnt->entstate == STATE_INVISIBLE || traceEnt->entstate == STATE_UNDERCONSTRUCTION ) {
			return;
		}

		// check invisible_users first since you don't want to draw a hint based
		// on that ent, but rather on what they are targeting.
		// so find the target and set checkEnt to that to show the proper hint.
		if( traceEnt->s.eType == ET_GENERAL ) {

			// ignore trigger_aidoor.  can't just not trace for triggers, since I need invisible_users...
			// damn, I would like to ignore some of these triggers though.

			if( !Q_stricmp(traceEnt->classname, "trigger_aidoor") ) {
				return;
			}

			if(!Q_stricmp(traceEnt->classname, "func_invisible_user")) {
				indirectHit = qtrue;

				// DHM - Nerve :: Put this back in only in multiplayer
				if(traceEnt->s.dmgFlags) {	// hint icon specified in entity
					hintType = traceEnt->s.dmgFlags;
					hintDist = CH_ACTIVATE_DIST;
					checkEnt = 0;
				} else { // use target for hint icon
					checkEnt = G_FindByTargetname( NULL, traceEnt->target);
					if(!checkEnt) {		// no target found
						hintType = HINT_BAD_USER;
						hintDist = CH_MAX_DIST_ZOOM;	// show this one from super far for debugging
					}
				}
			}
		}


		if(checkEnt) {

			// TDF This entire function could be the poster boy for converting to OO programming!!!
			// I'm making this into a switch in a vain attempt to make this readable so I can find which
			// brackets don't match!!!
			//
			// tjw: OO programming won't help those who refuse to use 
			// procedures and insist on stretching lines and tab depth
			// into oblivion.  It amazes me how people think that using 
			// objects are going to magically enable them write readable 
			// code.

			switch (checkEnt->s.eType) {
#ifdef __VEHICLES__
				case ET_VEHICLE:
					hintDist = CH_ACTIVATE_DIST;
					hintType = HINT_ACTIVATE;
					break;
#endif //__VEHICLES__
				case ET_CORPSE:
					if( !ent->client->ps.powerups[PW_BLUEFLAG] && !ent->client->ps.powerups[PW_REDFLAG] && !ent->client->ps.powerups[PW_OPS_DISGUISED]) {
						if( BODY_TEAM(traceEnt) < 4 && BODY_TEAM(traceEnt) != ent->client->sess.sessionTeam && traceEnt->nextthink == traceEnt->timestamp + BODY_TIME(BODY_TEAM(traceEnt))) {
							if( ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_COVERTOPS ) {
								hintDist	= 48;
								hintType	= HINT_UNIFORM;
								hintVal		= BODY_VALUE(traceEnt);
								if( hintVal > 255 ) {
									hintVal = 255;
								}
								break;
							}
						}
							}
					// class stealing
					if(!g_classChange.integer) break;
					if(BODY_TEAM(traceEnt) == ent->client->sess.sessionTeam) {
						hintDist	= 48;
						hintType	= HINT_UNIFORM;
						hintVal		= BODY_VALUE(traceEnt);
						if( hintVal > 255 ) {
							hintVal = 255;
						}
					}
					break;
				case ET_GENERAL:
				case ET_MG42_BARREL:
				case ET_AAGUN:
					hintType = HINT_FORCENONE;

					if( G_EmplacedGunIsMountable( traceEnt, ent) ) {
						hintDist = CH_ACTIVATE_DIST;
						hintType = HINT_MG42;	
						hintVal = 0;
					} else {
						if( ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER && G_EmplacedGunIsRepairable( traceEnt, ent)) {
							hintType = HINT_BUILD;
							hintDist = CH_BREAKABLE_DIST;
							hintVal = traceEnt->health;
							if( hintVal > 255 ) {
								hintVal = 255;
							}
						} else {
							hintDist = 0;
							hintType = ps->serverCursorHint = HINT_FORCENONE;
							hintVal = ps->serverCursorHintVal = 0;
						}
					}
					break;
				case ET_EXPLOSIVE:
				{
					if( checkEnt->spawnflags & EXPLOSIVE_TANK ) {
						hintDist = CH_BREAKABLE_DIST * 2;
						hintType = HINT_TANK;
						hintVal	 = ps->serverCursorHintVal	= 0;	// no health for tank destructibles
					} else {
						switch( checkEnt->constructibleStats.weaponclass ) {
						case 0:
							hintDist = CH_BREAKABLE_DIST;
							hintType = HINT_BREAKABLE;
							hintVal  = checkEnt->health;		// also send health to client for visualization
							break;
						case 1:
							hintDist = CH_BREAKABLE_DIST * 2;
							hintType = HINT_SATCHELCHARGE;
							hintVal	 = ps->serverCursorHintVal = 0;	// no health for satchel charges
							break;
						case 2:
							hintDist = 0;
							hintType = ps->serverCursorHint = HINT_FORCENONE;
							hintVal = ps->serverCursorHintVal = 0;

							if( checkEnt->parent && checkEnt->parent->s.eType == ET_OID_TRIGGER ) {
								if( ( (ent->client->sess.sessionTeam == TEAM_AXIS) && (checkEnt->parent->spawnflags & ALLIED_OBJECTIVE) ) ||
									( (ent->client->sess.sessionTeam == TEAM_ALLIES) && (checkEnt->parent->spawnflags & AXIS_OBJECTIVE) ) ) {
									hintDist = CH_BREAKABLE_DIST * 2;
									hintType = HINT_BREAKABLE_DYNAMITE;
									hintVal	 = ps->serverCursorHintVal	= 0;	// no health for dynamite
								}
							}
							break;
						default:
							if( checkEnt->health > 0 ) {
								hintDist = CH_BREAKABLE_DIST;
								hintType = HINT_BREAKABLE;
								hintVal  = checkEnt->health;		// also send health to client for visualization
							} else {
								hintDist = 0;
								hintType = ps->serverCursorHint		= HINT_FORCENONE;
								hintVal	 = ps->serverCursorHintVal	= 0;
							}
							break;
						}
					}

					break;
				}
#ifdef __ETE__
				case ET_CONSTRUCTIBLE_MG_NEST:
				case ET_CONSTRUCTIBLE_SANDBAGS:
					if( checkEnt->s.dl_intensity == SB_CONSTRUCTING || checkEnt->s.dl_intensity == SB_CONSTRUCTED ) {
						// only show hint for players who can blow it up
						if( checkEnt->s.teamNum != ent->client->sess.sessionTeam ) {
							switch( ent->client->sess.playerType ) {
							case PC_COVERTOPS:
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_SATCHELCHARGE;
								hintVal	 = ps->serverCursorHintVal	= 0;	// no health for satchel charges
								break;
							case PC_ENGINEER:
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_BREAKABLE_DYNAMITE;
								hintVal	 = ps->serverCursorHintVal	= 0;	// no health for dynamite
								break;
							default:
								hintDist = 0;
								hintType = ps->serverCursorHint		= HINT_FORCENONE;
								hintVal	 = ps->serverCursorHintVal	= 0;
								break;
							}
						} else {
							hintDist = 0;
							hintType = ps->serverCursorHint		= HINT_FORCENONE;
							hintVal	 = ps->serverCursorHintVal	= 0;
							break;
						}
						return;
					}
					else
					{
						if (ent->client->sess.playerType == PC_ENGINEER)
						{
							if (VectorDistance(checkEnt->r.currentOrigin, ent->r.currentOrigin) <= 64)
							{
								hintType = HINT_BUILD;
								hintDist = CH_BREAKABLE_DIST;
								hintVal = checkEnt->health;
								if( hintVal > 255 ) {
									hintVal = 255;
								}
							}
						}
					}
					break;
#endif //__ETE__
				case ET_CONSTRUCTIBLE:
					if( G_ConstructionIsPartlyBuilt( checkEnt ) && !(checkEnt->spawnflags & CONSTRUCTIBLE_INVULNERABLE) ) {
						// only show hint for players who can blow it up
						if( checkEnt->s.teamNum != ent->client->sess.sessionTeam ) {
							switch( checkEnt->constructibleStats.weaponclass ) {
							case 0:
								hintDist = CH_BREAKABLE_DIST;
								hintType = HINT_BREAKABLE;
								hintVal  = checkEnt->health;		// also send health to client for visualization
								break;
							case 1:
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_SATCHELCHARGE;
								hintVal	 = ps->serverCursorHintVal	= 0;	// no health for satchel charges
								break;
							case 2:
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_BREAKABLE_DYNAMITE;
								hintVal	 = ps->serverCursorHintVal	= 0;	// no health for dynamite
								break;
							default:
								hintDist = 0;
								hintType = ps->serverCursorHint		= HINT_FORCENONE;
								hintVal	 = ps->serverCursorHintVal	= 0;
								break;
							}
						} else {
							hintDist = 0;
							hintType = ps->serverCursorHint		= HINT_FORCENONE;
							hintVal	 = ps->serverCursorHintVal	= 0;
							return;
						}
					}

					break;
				case ET_ALARMBOX:
					if(checkEnt->health > 0) {
						hintType = HINT_ACTIVATE;
					}
					break;

				case ET_ITEM: 
				{
					gitem_t* it = &bg_itemlist[checkEnt->item - bg_itemlist];

					hintDist = CH_ACTIVATE_DIST;

					switch(it->giType) {
						case IT_HEALTH:
							hintType = HINT_HEALTH;
							break;
						case IT_TREASURE:
							hintType = HINT_TREASURE;
							break;
						case IT_WEAPON: {
								qboolean canPickup = COM_BitCheck( ent->client->ps.weapons, it->giTag );

								if( !canPickup ) {
									if( it->giTag == WP_AMMO ) {
										canPickup = qtrue;
									}
								}

								if( !canPickup ) {
									canPickup = G_CanPickupWeapon( it->giTag, ent );
								}

								if( canPickup ) {
									hintType = HINT_WEAPON;
								}
								break;
							}
						case IT_AMMO:	
							hintType = HINT_AMMO;
							break;
						case IT_ARMOR:	
							hintType = HINT_ARMOR;
							break;
						case IT_HOLDABLE:
							hintType = HINT_HOLDABLE;
							break;
						case IT_KEY:
							hintType = HINT_INVENTORY;
							break;
						case IT_TEAM:
							if ( !Q_stricmp( traceEnt->classname, "team_CTF_redflag" ) && ent->client->sess.sessionTeam == TEAM_ALLIES )
								hintType = HINT_POWERUP;
							else if ( !Q_stricmp( traceEnt->classname, "team_CTF_blueflag" ) && ent->client->sess.sessionTeam == TEAM_AXIS )
								hintType = HINT_POWERUP;
							break;
						case IT_BAD:
						default:
							break;
					}

					break;
				}
				case ET_MOVER:
					if(!Q_stricmp( checkEnt->classname, "script_mover" ) ) {
						if( G_TankIsMountable( checkEnt, ent ) ) {
							hintDist = CH_ACTIVATE_DIST;
							hintType = HINT_ACTIVATE;
						}
					} else if( !Q_stricmp( checkEnt->classname, "func_door_rotating" ) ) {
						if( checkEnt->moverState == MOVER_POS1ROTATE ) { // stationary/closed
							hintDist = CH_DOOR_DIST;
							hintType = HINT_DOOR_ROTATING;
							if( checkEnt->key == -1 || !G_AllowTeamsAllowed( checkEnt, ent ) ) { // locked
								hintType = HINT_DOOR_ROTATING_LOCKED;
							}
						}
					} else if(!Q_stricmp(checkEnt->classname, "func_door")) {
						if(checkEnt->moverState == MOVER_POS1) { // stationary/closed
							hintDist = CH_DOOR_DIST;
							hintType = HINT_DOOR;

							if( checkEnt->key == -1 || !G_AllowTeamsAllowed( checkEnt, ent ) ) { // locked
								hintType = HINT_DOOR_LOCKED;
							}
						}
					} else if(!Q_stricmp(checkEnt->classname, "func_button")) {
						hintDist = CH_ACTIVATE_DIST;
						hintType = HINT_BUTTON;
					} else if(!Q_stricmp(checkEnt->classname, "props_flamebarrel")) {
						hintDist = CH_BREAKABLE_DIST*2;
						hintType = HINT_BREAKABLE;
					} else if(!Q_stricmp(checkEnt->classname, "props_statue")) {
						hintDist = CH_BREAKABLE_DIST*2;
						hintType = HINT_BREAKABLE;
					}
					
					break;
				case ET_MISSILE:
				case ET_BOMB:
					if ( ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) 
					{
						hintDist	= CH_BREAKABLE_DIST;
						hintType	= HINT_DISARM;
						hintVal		= checkEnt->health;		// also send health to client for visualization
						if ( hintVal > 255 )
							hintVal = 255;
					}
					

					// hint icon specified in entity (and proper contact was made, so hintType was set)
					// first try the checkent...
					if( checkEnt->s.dmgFlags && hintType ) {
						hintType = checkEnt->s.dmgFlags;
					}

					// then the traceent
					if( traceEnt->s.dmgFlags && hintType ) {
						hintType = traceEnt->s.dmgFlags;
					}

					break;
				default:
					break;
			}

			if(zooming) {
				hintDist = CH_MAX_DIST_ZOOM;

				// zooming can eat a lot of potential hints
				switch(hintType) {

					// allow while zooming
					case HINT_PLAYER:
					case HINT_TREASURE:
					case HINT_LADDER:
					case HINT_EXIT:
					case HINT_NOEXIT:
					case HINT_PLYR_FRIEND:
					case HINT_PLYR_NEUTRAL:
					case HINT_PLYR_ENEMY:
					case HINT_PLYR_UNKNOWN:
						break;

					default:
						return;
				}
			}
		}
	} 

	if( dist <= Square( hintDist )) {
		ps->serverCursorHint = hintType;
		ps->serverCursorHintVal = hintVal;
	}

}

#ifdef __BOT__
void G_BotCheckForCursorHints(gentity_t *ent)
{
	vec3_t		forward, right, up, offset, end, angles;
	trace_t		*tr;
	float		dist;
	float		chMaxDist = 96;
	gentity_t	*checkEnt, *traceEnt = 0;
  	playerState_t *ps;
	int			hintType, hintDist, hintVal;
	qboolean	zooming, indirectHit;	// indirectHit means the checkent was not the ent hit by the trace (checkEnt!=traceEnt)
	int			trace_contents;			// DHM - Nerve
	int			numOfIgnoredEnts = 0;

	if (!ent->client) {
		return;
	}

	CalcMuzzlePoints(ent, ent->client->ps.weapon);

	// Init the bot_can_build marker before checks...
	ent->bot_can_build = qfalse;

	ps = &ent->client->ps;

#ifdef SAVEGAME_SUPPORT
	// don't change anything if reloading.  just set the exit hint
	if ((g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_COOP || g_gametype.integer == GT_NEWCOOP) && g_reloading.integer == RELOAD_NEXTMAP_WAITING) {
		ps->serverCursorHint = HINT_EXIT;
		ps->serverCursorHintVal = 0;
		return;
	}
#endif // SAVEGAME_SUPPORT

	indirectHit = qfalse;

	zooming = qfalse;//(qboolean)(ps->eFlags & EF_ZOOMING);

	AngleVectors (ps->viewangles, forward, right, up);

	VectorCopy(ps->origin, offset);
	offset[2] += ps->viewheight;

	// lean
	if (ps->leanf) {
		VectorMA(offset, ps->leanf, right, offset);
	}

//	if (zooming) {
//		VectorMA(offset, CH_MAX_DIST_ZOOM, forward, end);
//	} else {
		VectorMA(offset, chMaxDist, forward, end);
//	}

	tr = &ps->serverCursorHintTrace;

	trace_contents = (CONTENTS_TRIGGER|CONTENTS_SOLID|CONTENTS_MISSILECLIP|CONTENTS_BODY|CONTENTS_CORPSE);
	trap_Trace(tr, offset, NULL, NULL, end, ps->clientNum, trace_contents);

//	if (!tr)
//		return;

	// reset all
	hintType	= ps->serverCursorHint		= HINT_NONE;
	hintVal		= ps->serverCursorHintVal	= 0;

	dist = VectorDistanceSquared(offset, tr->endpos);

//	if (zooming) {
//		hintDist	= CH_MAX_DIST_ZOOM;
//	} else {
		hintDist	= chMaxDist;
//	}

#ifdef __ETE__
	if ( ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER || ps->weapon == WP_PLIERS )
	{
		gentity_t *test_ent = &g_entities[tr->entityNum];

		if((test_ent->s.eType == ET_CONSTRUCTIBLE_SANDBAGS || test_ent->s.eType == ET_CONSTRUCTIBLE_MG_NEST)
			&& (test_ent->s.dl_intensity == SB_UNCONSTRUCTED || test_ent->s.dl_intensity == SB_CONSTRUCTING) )
		{
			if (VectorDistance(test_ent->r.currentOrigin, ent->r.currentOrigin) <= 64)
			{
				ent->client->touchEnt = test_ent;
				ps->serverCursorHint = HINT_CONSTRUCTIBLE;
				ps->serverCursorHintVal = (int)test_ent->s.effect1Time;
				return;
			}
		}
	}
#endif //__ETE__

	// Arnout: building something - add this here because we don't have anything solid to trace to - quite ugly-ish
	if (ent->client->touchingTOI && ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER) 
	{
		{
			gentity_t* constructible	= G_IsConstructible(ent->client->sess.sessionTeam, ent->client->touchingTOI);

			if (constructible != NULL) {
				ps->serverCursorHint = HINT_CONSTRUCTIBLE;
				ps->serverCursorHintVal = (int)constructible->s.angles2[0];

				if (ent->is_bot && ent->client->sess.playerType == PC_ENGINEER)
				{// Fix it!
					ent->beStillEng = level.time + 5000;

					AIMOD_Bot_Aim_At_Object( ent, constructible );

					if (ent->client->ps.weapon != WP_PLIERS)	
					{// Switch to pliers and use them!
						BOT_ChangeWeapon(ent, WP_PLIERS);
					}

					if (ent->client->ps.weapon == WP_PLIERS)	
					{
						FireWeapon(ent);
					}

					if (ent->bot_last_chat_time < level.time)
					{
						ent->bot_last_chat_time = level.time + 10000;
						G_Voice(ent, NULL, SAY_TEAM, va("ConstructionCommencing"), qfalse);
					}

					ent->bot_can_build = qtrue;
					ent->bot_job_wait_time = level.time + 2000;
				}
				return;
			}
		}
	}
	else if (ent->client->touchingTOI)
	{// Non engineer's need to call for help!
		{
			gentity_t* constructible	= G_IsConstructible(ent->client->sess.sessionTeam, ent->client->touchingTOI);
			if (constructible != NULL) 
			{
				ps->serverCursorHint = HINT_CONSTRUCTIBLE;
				ps->serverCursorHintVal = (int)constructible->s.angles2[0];

				if (ent->bot_last_chat_time < level.time)
				{
					ent->bot_last_chat_time = level.time + 10000;
					G_Voice(ent, NULL, SAY_TEAM, va("NeedEngineer"), qfalse);
				}
				return;
			}
		}
	}

	if (ps->stats[ STAT_PLAYER_CLASS ] == PC_COVERTOPS) {
		if (ent->client->landmineSpottedTime && level.time - ent->client->landmineSpottedTime < 500) {
			ps->serverCursorHint = HINT_LANDMINE;
			ps->serverCursorHintVal	= ent->client->landmineSpotted ? ent->client->landmineSpotted->count2 : 0;
			return;
		}
	}

	if (tr->fraction == 1) {
		return;
	}

	traceEnt = &g_entities[tr->entityNum];

#ifdef __VEHICLES__
/*	if ( (traceEnt->client && (traceEnt->client->ps.eFlags & EF_VEHICLE) && OnSameTeam(traceEnt, ent)) 
			|| !Q_stricmp (traceEnt->classname, "churchilltank") 
			|| !Q_stricmp (traceEnt->classname, "panzertank") 
			|| !Q_stricmp (traceEnt->classname, "panzerivtank") 
			|| !Q_stricmp (traceEnt->classname, "kv1tank") 
            || !Q_stricmp (traceEnt->classname, "stug3tank")
			|| !Q_stricmp (traceEnt->classname, "tigertank")
			|| !Q_stricmp (traceEnt->classname, "flak88antitank")
			|| !Q_stricmp (traceEnt->classname, "pakantitank"))
	{
		if ( traceEnt->health < traceEnt->maxhealth )
		{
			vec3_t eyes;

			ps->serverCursorHint = HINT_CONSTRUCTIBLE;
			ps->serverCursorHintVal = (int)((float)((float)traceEnt->health/(float)traceEnt->maxhealth)*(float)100);

			ent->beStill = level.time + 10000;

			if (ent->client->ps.weapon != WP_PLIERS)	
			{// Switch to plyers and use them!
				BOT_ChangeWeapon(ent, WP_PLIERS);
			}
		
			if (ent->client->ps.weapon == WP_PLIERS)	
			{
				FireWeapon(ent);
			}

			// Head for our object!
			BOT_GetMuzzlePoint( ent );
			VectorCopy(ent->bot_muzzlepoint, eyes);
			VectorSubtract (traceEnt->r.currentOrigin, eyes, ent->move_vector);
			vectoangles (ent->move_vector, ent->bot_ideal_view_angles);
			VectorCopy( ent->bot_ideal_view_angles, ent->s.angles );
			return;
		}
	}*/

	// UQ1: Enable when AI works with tanks...
	/*if (BOT_HaveVehicleWaypoints() 
		&& traceEnt 
		&& (traceEnt->s.eType == ET_VEHICLE || (traceEnt->s.eType == ET_PLAYER && traceEnt->client && (traceEnt->client->ps.eFlags & EF_VEHICLE)))
		&& !(ent->client->ps.eFlags & EF_VEHICLE)
		&& ent->client->sess.playerType != PC_ENGINEER)
	{
		ent->current_node = -1;
		ent->next_node = -1;
		ent->last_node = -1;
		ent->longTermGoal = -1;
		Cmd_Activate_f( ent );
		return;
	}*/
#endif //__VEHICLES__

	while(traceEnt && G_CursorHintIgnoreEnt(traceEnt, ent) && numOfIgnoredEnts < 10) {
		// xkan, 1/9/2003 - we may hit multiple invalid ents at the same point
		// count them to prevent too many loops
		numOfIgnoredEnts++;

		// xkan, 1/8/2003 - advance offset (start point) past the entity to ignore
		VectorMA(tr->endpos, 0.1, forward, offset);

		trap_Trace(tr, offset, NULL, NULL, end, traceEnt->s.number, trace_contents);

		// xkan, 1/8/2003 - (hintDist - dist) is the actual distance in the above
		// trap_Trace call. update dist accordingly.
		dist += VectorDistanceSquared(offset, tr->endpos);
		if (tr->fraction == 1) {
			return;
		}
		traceEnt = &g_entities[tr->entityNum];
	}

	if (tr->entityNum == ENTITYNUM_WORLD) {
/*#ifndef __ETE__
		if ((tr->contents & CONTENTS_WATER) && !(ps->powerups[PW_BREATHER])) {
			hintDist = CH_WATER_DIST;
			hintType = HINT_WATER;
		} else if ((tr->surfaceFlags & SURF_LADDER) && !(ps->pm_flags & PMF_LADDER)) { // ladder
			hintDist = CH_LADDER_DIST;
			hintType = HINT_LADDER;
		}
#endif*/
#ifdef __ETE__
		if( tr->fraction < 1 && /*(tr->contents & CONTENTS_WATER) &&*/ !(ps->pm_flags & PMF_LADDER) ) { // UQ1: check for climbing out of water!
			vec3_t offset2;

			VectorCopy( ps->origin, offset2 );
			if(ps->viewangles[PITCH] > 30 && (ps->eFlags & EF_CROUCHING))
				offset2[2] += 30;
			else
				offset2[2] += ps->viewheight;

			if (UQ_Climbable( offset2, ps->viewangles, ent->s.number ))
			{
				hintDist = CH_LADDER_DIST;
				hintType = HINT_LADDER;
			}
		}
#endif //__ETE__
	} else if (tr->entityNum < MAX_CLIENTS) {
		// Show medics a syringe if they can revive someone

		if (traceEnt->client && traceEnt->client->sess.sessionTeam == ent->client->sess.sessionTeam) {
			if (ps->stats[ STAT_PLAYER_CLASS ] == PC_MEDIC && traceEnt->client->ps.pm_type == PM_DEAD && !(traceEnt->client->ps.pm_flags & PMF_LIMBO)) 
			{
				vec3_t eyes;

				hintDist	= 48; // JPW NERVE matches weapon_syringe in g_weapon.c
				hintType	= HINT_REVIVE;

				BOT_GetMuzzlePoint( ent );
				VectorCopy(ent->bot_muzzlepoint, eyes);
				VectorSubtract (traceEnt->r.currentOrigin, eyes, ent->move_vector);
				vectoangles (ent->move_vector, ent->bot_ideal_view_angles);
				VectorCopy( ent->bot_ideal_view_angles, ent->s.angles );
				//ent->s.angles[2]-=90;

				if (ent->client->ps.weapon != WP_MEDIC_SYRINGE)	
				{// Switch to plyers and use them!
					BOT_ChangeWeapon(ent, WP_MEDIC_SYRINGE);
				}

				if (ent->client->ps.weapon == WP_MEDIC_SYRINGE)	
				{
					FireWeapon(ent);
				}

				ent->bot_job_wait_time = level.time + 2000;
			}
		} else if (traceEnt->client && traceEnt->client->isCivilian) {
			// xkan, 1/6/2003 - check for civilian, show neutral cursor (no matter which team)
			hintType = HINT_PLYR_NEUTRAL;
			hintDist = CH_FRIENDLY_DIST;	// far, since this will be used to determine whether to shoot bullet weaps or not
		}
	} else {
		checkEnt = traceEnt;

		// Arnout: invisible entities don't show hints
		if (traceEnt->entstate == STATE_INVISIBLE || traceEnt->entstate == STATE_UNDERCONSTRUCTION) {
			return;
		}

		// check invisible_users first since you don't want to draw a hint based
		// on that ent, but rather on what they are targeting.
		// so find the target and set checkEnt to that to show the proper hint.
		if (traceEnt->s.eType == ET_GENERAL) {

			// ignore trigger_aidoor.  can't just not trace for triggers, since I need invisible_users...
			// damn, I would like to ignore some of these triggers though.

			if (!Q_stricmp(traceEnt->classname, "trigger_aidoor")) {
				if (ent->last_use_time < level.time  && no_mount_time[ent->s.clientNum] < level.time)
				{
					ent->last_use_time = level.time + 1000;
					Cmd_Activate_f(ent);
				}
				return;
			}

			if (!Q_stricmp(traceEnt->classname, "func_invisible_user")) {
				indirectHit = qtrue;

				// DHM - Nerve :: Put this back in only in multiplayer
				if (traceEnt->s.dmgFlags) {	// hint icon specified in entity
					hintType = traceEnt->s.dmgFlags;
					hintDist = CH_ACTIVATE_DIST;
					checkEnt = 0;
					
					if (ent->last_use_time < level.time  && no_mount_time[ent->s.clientNum] < level.time)
					{
						ent->last_use_time = level.time + 1000;
						Cmd_Activate_f(ent);
					}
				} else { // use target for hint icon
					checkEnt = G_FindByTargetname(NULL, traceEnt->target);
					if (!checkEnt) {		// no target found
						hintType = HINT_BAD_USER;
						hintDist = CH_MAX_DIST_ZOOM;	// show this one from super far for debugging
					}
				}
			}
		}


		if (checkEnt) {

			// TDF This entire function could be the poster boy for converting to OO programming!!!
			// I'm making this into a switch in a vain attempt to make this readable so I can find which
			// brackets don't match!!!

			switch (checkEnt->s.eType) {
				case ET_CORPSE:
					if (!ent->client->ps.powerups[PW_BLUEFLAG] && !ent->client->ps.powerups[PW_REDFLAG] && !ent->client->ps.powerups[PW_OPS_DISGUISED]) {
						if (BODY_TEAM(traceEnt) < 4 && BODY_TEAM(traceEnt) != ent->client->sess.sessionTeam && traceEnt->nextthink == traceEnt->timestamp + BODY_TIME(BODY_TEAM(traceEnt))) {
							if (ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_COVERTOPS) {
								hintDist	= 48;
								hintType	= HINT_UNIFORM;
								hintVal		= BODY_VALUE(traceEnt);
								if (hintVal > 255) {
									hintVal = 255;
								}

								if (ent->is_bot && ent->client->sess.playerType == PC_COVERTOPS)
								{// Take clothes!
									if (ent->last_use_time < level.time  && no_mount_time[ent->s.clientNum] < level.time)
									{
										ent->beStill = level.time + 1000;
										Cmd_Activate_f(ent);
									}
								}
							}
						}
					}
					break;
				case ET_GENERAL:
				case ET_MG42_BARREL:
				case ET_AAGUN:
					hintType = HINT_FORCENONE;

					if (G_EmplacedGunIsMountable(traceEnt, ent)) {
						hintDist = CH_ACTIVATE_DIST;
						hintType = HINT_MG42;
						hintVal = 0;

						if (ent->is_bot && ent->client->sess.playerType != PC_ENGINEER)
						{
							if (ent->last_use_time < level.time  && no_mount_time[ent->s.clientNum] < level.time)
							{
								ent->beStill = level.time + 1000;
								Cmd_Activate_f(ent);
							}
						}

					} else {
						if (ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER && G_EmplacedGunIsRepairable(traceEnt, ent)) {
							hintType = HINT_BUILD;
							hintDist = CH_BREAKABLE_DIST;
							hintVal = traceEnt->health;
							if (hintVal > 255) {
								hintVal = 255;
							}

							if (ent->is_bot)
							{
								if (ent->r.svFlags & SVF_BOT)
								{// Tell bots to build it...
									ent->beStill = level.time + 10000;
		
									AIMOD_Bot_Aim_At_Object( ent, traceEnt );

									if (ent->client->ps.weapon != WP_PLIERS)	
									{// Switch to plyers and use them!
										BOT_ChangeWeapon(ent, WP_PLIERS);
									}
		
									if (ent->client->ps.weapon == WP_PLIERS)	
									{
										FireWeapon(ent);
									}

									/*if (ent->bot_last_chat_time < level.time)
									{
										ent->bot_last_chat_time = level.time + 10000;
										G_Voice(ent, NULL, SAY_TEAM, va("OnDefense"), qfalse);
									}*/

									ent->bot_can_build = qtrue;
									ent->bot_job_wait_time = level.time + 2000;
								}
								return;
								//ent->bot_can_build = qtrue;
							}
						} else if (G_EmplacedGunIsRepairable(traceEnt, ent)) {
							hintType = HINT_BUILD;
							hintDist = CH_BREAKABLE_DIST;
							hintVal = traceEnt->health;
							if (hintVal > 255) {
								hintVal = 255;
							}

							if (ent->bot_last_chat_time < level.time)
							{
								ent->bot_last_chat_time = level.time + 10000;
								G_Voice(ent, NULL, SAY_TEAM, va("NeedEngineer"), qfalse);
							}
						} else {
							hintDist = 0;
							hintType = ps->serverCursorHint = HINT_FORCENONE;
							hintVal = ps->serverCursorHintVal = 0;
						}
					}
					break;
				case ET_EXPLOSIVE:
				{
					if (checkEnt->spawnflags & EXPLOSIVE_TANK) {
						hintDist = CH_BREAKABLE_DIST * 2;
						hintType = HINT_TANK;
						hintVal	 = ps->serverCursorHintVal	= 0;	// no health for tank destructibles

						if (ent->is_bot && ent->client->sess.playerType == PC_ENGINEER && checkEnt->health > 0)
						{// Lay dynomite here!
							ent->beStillEng = level.time + 10000;
							
							if (ent->client->ps.weapon != WP_DYNAMITE)	
							{// Switch to plyers and use them!
								BOT_ChangeWeapon(ent, WP_DYNAMITE);
							}
		
							if (ent->client->ps.weapon == WP_DYNAMITE && next_satchel_available_time[ent->s.clientNum] < level.time)
							{
								FireWeapon(ent);
								//weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
								next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
							}

							// Head for our object!
							AIMOD_Bot_Aim_At_Object( ent, checkEnt );

							if (ent->bot_last_chat_time < level.time)
							{
								ent->bot_last_chat_time = level.time + 10000;
								G_Voice(ent, NULL, SAY_TEAM, va("CoverMe"), qfalse);
							}

							ent->bot_job_wait_time = level.time + 2000;
						}
						else if (ent->is_bot && checkEnt->health > 0)
						{// Grenades???
							ent->beStill = level.time + 2000;

							if (ent->bot_last_chat_time < level.time)
							{
								ent->bot_last_chat_time = level.time + 10000;
								G_Say(ent, NULL, SAY_TEAM, va("Need coverring fire!"));
							}

							// Head for our object!
							AIMOD_Bot_Aim_At_Object( ent, checkEnt );

							if (ent->client->sess.sessionTeam == TEAM_AXIS)
							{
								if (ent->client->ps.weapon != WP_GRENADE_LAUNCHER)
								{
									BOT_ChangeWeapon(ent, WP_GRENADE_LAUNCHER);
								}
								else
								{
									weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
								}
							}
							else
							{
								if (ent->client->ps.weapon != WP_GRENADE_PINEAPPLE)
								{
									BOT_ChangeWeapon(ent, WP_GRENADE_PINEAPPLE);
								}
								else
								{
									weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
								}
							}

							ent->bot_job_wait_time = level.time + 2000;
						}
					} else {
						switch(checkEnt->constructibleStats.weaponclass) {
						case 0:
							hintDist = CH_BREAKABLE_DIST;
							hintType = HINT_BREAKABLE;
							hintVal  = checkEnt->health;		// also send health to client for visualization

							if (ent->is_bot && ent->client->sess.playerType == PC_COVERTOPS)
							{// Lay satchel charge here!
								ent->beStillEng = level.time + 3000;
								
								if (ent->client->ps.weapon != WP_SATCHEL)	
								{// Switch to plyers and use them!
									BOT_ChangeWeapon(ent, WP_SATCHEL);
								}
		
								if (ent->client->ps.weapon == WP_SATCHEL)	
								{
									if (next_satchel_available_time[ent->s.clientNum] < level.time)
									{
										weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
										next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
									}
								}

								ent->bot_job_wait_time = level.time + 2000;
							}
							else if (ent->is_bot && ent->client->sess.playerType == PC_ENGINEER)
							{// Lay dynomite here!
								ent->beStillEng = level.time + 10000;
								
								if (ent->client->ps.weapon != WP_DYNAMITE)	
								{// Switch to plyers and use them!
									BOT_ChangeWeapon(ent, WP_DYNAMITE);
								}
		
								if (ent->client->ps.weapon == WP_DYNAMITE)	
								{
									if (next_satchel_available_time[ent->s.clientNum] < level.time)
									{
										weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
										next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
									}
								}

								// Head for our object!
								AIMOD_Bot_Aim_At_Object( ent, checkEnt );

								ent->bot_job_wait_time = level.time + 2000;
							}
							else if (ent->is_bot)
							{// Grenades???
								ent->beStill = level.time + 2000;
								G_Say(ent, NULL, SAY_TEAM, va("Need coverring fire!"));

								// Head for our object!
								AIMOD_Bot_Aim_At_Object( ent, checkEnt );

								if (ent->client->sess.sessionTeam == TEAM_AXIS)
								{
									if (ent->client->ps.weapon != WP_GRENADE_LAUNCHER)
									{
										BOT_ChangeWeapon(ent, WP_GRENADE_LAUNCHER);
									}
									else
									{
										weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
									}
								}
								else
								{
									if (ent->client->ps.weapon != WP_GRENADE_PINEAPPLE)
									{
										BOT_ChangeWeapon(ent, WP_GRENADE_PINEAPPLE);
									}
									else
									{
										weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
									}
								}

								ent->bot_job_wait_time = level.time + 2000;
							}
							break;
						case 1:
							hintDist = CH_BREAKABLE_DIST * 2;
							hintType = HINT_SATCHELCHARGE;
							hintVal	 = ps->serverCursorHintVal = 0;	// no health for satchel charges

							if (ent->is_bot && ent->client->sess.playerType == PC_COVERTOPS)
							{// Lay satchel charge here!
								ent->beStillEng = level.time + 3000;
								
								if (ent->client->ps.weapon != WP_SATCHEL)	
								{// Switch to plyers and use them!
									BOT_ChangeWeapon(ent, WP_SATCHEL);
								}
		
								if (ent->client->ps.weapon == WP_SATCHEL)	
								{
									if (next_satchel_available_time[ent->s.clientNum] < level.time)
									{
										weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
										next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
									}
								}

								ent->bot_job_wait_time = level.time + 2000;
							}
							else if (ent->is_bot && ent->client->sess.playerType == PC_ENGINEER)
							{// Lay dynomite here!
								ent->beStillEng = level.time + 10000;
								
								if (ent->client->ps.weapon != WP_DYNAMITE)	
								{// Switch to plyers and use them!
									BOT_ChangeWeapon(ent, WP_DYNAMITE);
								}
		
								if (ent->client->ps.weapon == WP_DYNAMITE)	
								{
									if (next_satchel_available_time[ent->s.clientNum] < level.time)
									{
										weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
										next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
									}
								}

								// Head for our object!
								AIMOD_Bot_Aim_At_Object( ent, checkEnt );
								ent->bot_job_wait_time = level.time + 2000;
							}
							break;
						case 2:
							hintDist = 0;
							hintType = ps->serverCursorHint = HINT_FORCENONE;
							hintVal = ps->serverCursorHintVal = 0;

							if (checkEnt->parent && checkEnt->parent->s.eType == ET_OID_TRIGGER) 
							{
								if (((ent->client->sess.sessionTeam == TEAM_AXIS) && (checkEnt->parent->spawnflags & ALLIED_OBJECTIVE)) ||
									((ent->client->sess.sessionTeam == TEAM_ALLIES) && (checkEnt->parent->spawnflags & AXIS_OBJECTIVE))) {
									hintDist = CH_BREAKABLE_DIST * 2;
									hintType = HINT_BREAKABLE_DYNAMITE;
									hintVal	 = ps->serverCursorHintVal	= 0;	// no health for dynamite

									if (ent->is_bot && ent->client->sess.playerType == PC_ENGINEER)
									{// Lay dynomite here!
										ent->beStillEng = level.time + 10000;
										
										if (ent->client->ps.weapon != WP_DYNAMITE)	
										{// Switch to plyers and use them!
											BOT_ChangeWeapon(ent, WP_DYNAMITE);
										}
		
										if (ent->client->ps.weapon == WP_DYNAMITE)	
										{
											if (next_satchel_available_time[ent->s.clientNum] < level.time)
											{
												weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
												next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
											}
										}

										// Head for our object!
										AIMOD_Bot_Aim_At_Object( ent, checkEnt );

										if (ent->bot_last_chat_time < level.time)
										{
											ent->bot_last_chat_time = level.time + 10000;
											G_Voice(ent, NULL, SAY_TEAM, va("CoverMe"), qfalse);
										}

										ent->bot_job_wait_time = level.time + 2000;
									}
									else
									{
										if (ent->bot_last_chat_time < level.time)
										{
											ent->bot_last_chat_time = level.time + 10000;
											G_Voice(ent, NULL, SAY_TEAM, va("NeedEngineer"), qfalse);
										}
									}
								}
							}
							break;
						default:
							if (checkEnt->health > 0) {
								hintDist = CH_BREAKABLE_DIST;
								hintType = HINT_BREAKABLE;
								hintVal  = checkEnt->health;		// also send health to client for visualization

								// Head for our object!
								VectorSubtract(checkEnt->r.currentOrigin, ent->r.currentOrigin, ent->move_vector);
								vectoangles(ent->move_vector, angles);
								VectorCopy(angles, ent->s.angles);
								FireWeapon(ent);
							} else {
								hintDist = 0;
								hintType = ps->serverCursorHint		= HINT_FORCENONE;
								hintVal	 = ps->serverCursorHintVal	= 0;
							}
							break;
						}
					}

					break;
				}
#ifdef __ETE__
				case ET_CONSTRUCTIBLE_MG_NEST:
				case ET_CONSTRUCTIBLE_SANDBAGS:
					if( checkEnt->s.dl_intensity == SB_CONSTRUCTING || checkEnt->s.dl_intensity == SB_CONSTRUCTED ) {
						// only show hint for players who can blow it up
						if( checkEnt->s.teamNum != ent->client->sess.sessionTeam ) {
							switch( ent->client->sess.playerType ) {
							case PC_COVERTOPS:
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_SATCHELCHARGE;
								hintVal	 = ps->serverCursorHintVal	= 0;	// no health for satchel charges

								if (ent->is_bot && ent->client->sess.playerType == PC_COVERTOPS)
								{// Lay satchel charge here!
									ent->beStillEng = level.time + 3000;
									
									if (ent->client->ps.weapon != WP_SATCHEL)	
									{// Switch to plyers and use them!
										BOT_ChangeWeapon(ent, WP_SATCHEL);
									}
		
									if (ent->client->ps.weapon == WP_SATCHEL)	
									{
										if (next_satchel_available_time[ent->s.clientNum] < level.time)
										{
											weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
											next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
										}
									}
								}

								ent->bot_job_wait_time = level.time + 2000;

								break;
							case PC_ENGINEER:
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_BREAKABLE_DYNAMITE;
								hintVal	 = ps->serverCursorHintVal	= 0;	// no health for dynamite

								if (ent->is_bot && ent->client->sess.playerType == PC_ENGINEER)
								{// Lay dynomite here!
									ent->beStillEng = level.time + 10000;
									
									if (ent->client->ps.weapon != WP_DYNAMITE)	
									{// Switch to plyers and use them!
										BOT_ChangeWeapon(ent, WP_DYNAMITE);
									}
		
									if (ent->client->ps.weapon == WP_DYNAMITE)	
									{
										if (next_satchel_available_time[ent->s.clientNum] < level.time)
										{
											weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
											next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
										}
									}

									// Head for our object!
									AIMOD_Bot_Aim_At_Object( ent, checkEnt );

									if (ent->bot_last_chat_time < level.time)
									{
										ent->bot_last_chat_time = level.time + 10000;
										G_Voice(ent, NULL, SAY_TEAM, va("CoverMe"), qfalse);
									}

									ent->bot_job_wait_time = level.time + 2000;
								}

								break;
							default:
								hintDist = 0;
								hintType = ps->serverCursorHint		= HINT_FORCENONE;
								hintVal	 = ps->serverCursorHintVal	= 0;
								break;
							}
						} else {
							hintDist = 0;
							hintType = ps->serverCursorHint		= HINT_FORCENONE;
							hintVal	 = ps->serverCursorHintVal	= 0;
							return;
						}
					}

					break;
#endif //__ETE__
				case ET_CONSTRUCTIBLE:
					if (G_ConstructionIsPartlyBuilt(checkEnt) && !(checkEnt->spawnflags & CONSTRUCTIBLE_INVULNERABLE)) {
						// only show hint for players who can blow it up
						if (checkEnt->s.teamNum != ent->client->sess.sessionTeam) {
							switch(checkEnt->constructibleStats.weaponclass) {
							case 0:
								hintDist = CH_BREAKABLE_DIST;
								hintType = HINT_BREAKABLE;
								hintVal  = checkEnt->health;		// also send health to client for visualization

								if (ent->is_bot && ent->client->sess.playerType == PC_ENGINEER)
								{// Lay dynomite here!
									ent->beStillEng = level.time + 10000;
									
									if (ent->client->ps.weapon != WP_DYNAMITE)	
									{// Switch to plyers and use them!
										BOT_ChangeWeapon(ent, WP_DYNAMITE);
									}
		
									if (ent->client->ps.weapon == WP_DYNAMITE)	
									{
										if (next_satchel_available_time[ent->s.clientNum] < level.time)
										{
											weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
											next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
										}
									}

									// Head for our object!
									AIMOD_Bot_Aim_At_Object( ent, checkEnt );

									if (ent->bot_last_chat_time < level.time)
									{
										ent->bot_last_chat_time = level.time + 10000;
										G_Voice(ent, NULL, SAY_TEAM, va("CoverMe"), qfalse);
									}

									ent->bot_job_wait_time = level.time + 2000;
								}
								else if (ent->is_bot && ent->client->sess.playerType == PC_COVERTOPS)
								{// Lay satchel charge here!
									ent->beStillEng = level.time + 3000;
									
									if (ent->client->ps.weapon != WP_SATCHEL)	
									{// Switch to plyers and use them!
										BOT_ChangeWeapon(ent, WP_SATCHEL);
									}
		
									if (ent->client->ps.weapon == WP_SATCHEL)	
									{
										if (next_satchel_available_time[ent->s.clientNum] < level.time)
										{
											weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
											next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
										}
									}

									ent->bot_job_wait_time = level.time + 2000;
								}
								else
								{
									// Head for our object!
									AIMOD_Bot_Aim_At_Object( ent, checkEnt );
									FireWeapon(ent);
								}
								break;
							case 1:
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_SATCHELCHARGE;
								hintVal	 = ps->serverCursorHintVal	= 0;	// no health for satchel charges

								if (ent->is_bot && ent->client->sess.playerType == PC_COVERTOPS)
								{// Lay satchel charge here!
									ent->beStillEng = level.time + 3000;
									
									if (ent->client->ps.weapon != WP_SATCHEL)	
									{// Switch to plyers and use them!
										BOT_ChangeWeapon(ent, WP_SATCHEL);
									}
		
									if (ent->client->ps.weapon == WP_SATCHEL)	
									{
										if (next_satchel_available_time[ent->s.clientNum] < level.time)
										{
											weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
											next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
										}
									}

									ent->bot_job_wait_time = level.time + 2000;
								}
								else if (ent->is_bot && ent->client->sess.playerType == PC_ENGINEER)
								{// Lay dynomite here!
									ent->beStillEng = level.time + 10000;
									
									if (ent->client->ps.weapon != WP_DYNAMITE)	
									{// Switch to plyers and use them!
										BOT_ChangeWeapon(ent, WP_DYNAMITE);
									}
		
									if (ent->client->ps.weapon == WP_DYNAMITE)	
									{
										if (next_satchel_available_time[ent->s.clientNum] < level.time)
										{
											weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
											next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
										}
									}

									// Head for our object!
									AIMOD_Bot_Aim_At_Object( ent, checkEnt );

									ent->bot_job_wait_time = level.time + 2000;
								}
								break;
							case 2:
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_BREAKABLE_DYNAMITE;
								hintVal	 = ps->serverCursorHintVal	= 0;	// no health for dynamite

								if (ent->is_bot && ent->client->sess.playerType == PC_ENGINEER)
								{// Lay dynomite here!
									ent->beStillEng = level.time + 10000;
									
									if (ent->client->ps.weapon != WP_DYNAMITE)	
									{// Switch to plyers and use them!
										BOT_ChangeWeapon(ent, WP_DYNAMITE);
									}
		
									if (ent->client->ps.weapon == WP_DYNAMITE)	
									{
										if (next_satchel_available_time[ent->s.clientNum] < level.time)
										{
											weapon_grenadelauncher_fire(ent, ent->client->ps.weapon);
											next_satchel_available_time[ent->s.clientNum] = level.time + 10000;
										}
									}

									// Head for our object!
									AIMOD_Bot_Aim_At_Object( ent, checkEnt );

									ent->bot_job_wait_time = level.time + 2000;
								}
								else
								{
									if (ent->bot_last_chat_time < level.time)
									{
										ent->bot_last_chat_time = level.time + 10000;
										G_Voice(ent, NULL, SAY_TEAM, va("NeedEngineer"), qfalse);
									}
								}

								break;
							default:
								hintDist = 0;
								hintType = ps->serverCursorHint		= HINT_FORCENONE;
								hintVal	 = ps->serverCursorHintVal	= 0;
								break;
							}
						} else {
							hintDist = 0;
							hintType = ps->serverCursorHint		= HINT_FORCENONE;
							hintVal	 = ps->serverCursorHintVal	= 0;
							return;
						}
					}

					break;
				case ET_ALARMBOX:
					if (checkEnt->health > 0) {
						hintType = HINT_ACTIVATE;

						if (ent->is_bot)
						{
							if (ent->last_use_time < level.time  && no_mount_time[ent->s.clientNum] < level.time)
							{
								ent->beStill = level.time + 1000;
								Cmd_Activate_f(ent);
							}

							// Head for our object!
							AIMOD_Bot_Aim_At_Object( ent, checkEnt );
						}
					}
					break;

				case ET_ITEM:
				{
					gitem_t* it = checkEnt->item;

					hintDist = CH_ACTIVATE_DIST;

					switch(it->giType) {
						case IT_HEALTH:
							hintType = HINT_HEALTH;
							if (ent->last_use_time < level.time  && no_mount_time[ent->s.clientNum] < level.time)
							{
								ent->beStill = level.time + 1000;
								Cmd_Activate_f(ent);
							}
							AIMOD_Bot_Aim_At_Object( ent, checkEnt );
							break;
						case IT_TREASURE:
							hintType = HINT_TREASURE;

							if (ent->is_bot)
							{
								if (ent->last_use_time < level.time  && no_mount_time[ent->s.clientNum] < level.time)
								{
									ent->beStill = level.time + 1000;
									Cmd_Activate_f(ent);
								}

								// Head for our object!
								AIMOD_Bot_Aim_At_Object( ent, checkEnt );
							}

							break;
						case IT_WEAPON: {
							qboolean canPickup = BG_WeaponIsPrimaryForClassAndTeam(ent->client->sess.playerType, ent->client->ps.teamNum, it->giTag);

								if (!canPickup) {
									if (it->giTag == WP_AMMO) {
										canPickup = qtrue;
									}
								}

								if (!canPickup) {
									canPickup = G_CanPickupWeapon(it->giTag, ent);
								}

								if (canPickup) {
									hintType = HINT_WEAPON;
								}
								AIMOD_Bot_Aim_At_Object( ent, checkEnt );
								break;
							}
						case IT_AMMO:
							hintType = HINT_AMMO;
							break;
						case IT_ARMOR:
							hintType = HINT_ARMOR;
							break;
						case IT_HOLDABLE:
							hintType = HINT_HOLDABLE;
							break;
						case IT_KEY:
							hintType = HINT_INVENTORY;
							break;
						case IT_TEAM:
							if (!Q_stricmp(traceEnt->classname, "team_CTF_redflag") && ent->client->sess.sessionTeam == TEAM_ALLIES)
								hintType = HINT_POWERUP;
							else if (!Q_stricmp(traceEnt->classname, "team_CTF_blueflag") && ent->client->sess.sessionTeam == TEAM_AXIS)
								hintType = HINT_POWERUP;
							break;
						case IT_BAD:
						default:
							break;
					}

					break;
				}
				case ET_MOVER:
					if (!Q_stricmp(checkEnt->classname, "script_mover")) {
						if (G_TankIsMountable(checkEnt, ent)) {
							hintDist = CH_ACTIVATE_DIST;
							hintType = HINT_ACTIVATE;

							if (ent->is_bot)
							{
								if (ent->last_use_time < level.time && no_mount_time[ent->s.clientNum] < level.time)
									Cmd_Activate_f(ent);

								if (ent->beStill <= level.time)
									//ent->beStill = level.time + 60000; // 60 secs on mover.
									ent->beStill = level.time + 20000; // 20 secs on mover.

								// Head for our object!
								AIMOD_Bot_Aim_At_Object( ent, checkEnt );
							}
						}
					} else if (!Q_stricmp(checkEnt->classname, "func_door_rotating")) {
						if (checkEnt->moverState == MOVER_POS1ROTATE) { // stationary/closed
							hintDist = CH_DOOR_DIST;
							hintType = HINT_DOOR_ROTATING;
							if (checkEnt->key == -1 || !G_AllowTeamsAllowed(checkEnt, ent)) { // locked
								hintType = HINT_DOOR_ROTATING_LOCKED;
							}

							if (ent->is_bot && hintType != HINT_DOOR_ROTATING_LOCKED)
							{
								if (ent->last_use_time < level.time && no_mount_time[ent->s.clientNum] < level.time)
									Cmd_Activate_f(ent);

								if (ent->beStill <= level.time)
									ent->beStill = level.time + 1000;

								ent->node_timeout = level.time + 10000;//6000;//4000;
								ent->bot_last_good_move_time = level.time;

								// Head for our object!
								AIMOD_Bot_Aim_At_Object( ent, checkEnt );
							}
							else
							{// Mark the waypoints near the door as team only route...
								int i;

								if (!checkEnt->bot_initialized)
								{
									for (i = 0; i < number_of_nodes; i++)
									{
										if (VectorDistance(ent->r.currentOrigin, nodes[i].origin) < 96)
										{
											if (ent->client->sess.sessionTeam == TEAM_AXIS)
												nodes[i].type |= NODE_AXIS_UNREACHABLE;
											else
												nodes[i].type |= NODE_ALLY_UNREACHABLE;
										}
									}

									checkEnt->bot_initialized = qtrue;
								}

								// Find a new path...
								ent->current_node = -1;
								ent->longTermGoal = -1;
							}
						}
					} else if (!Q_stricmp(checkEnt->classname, "func_door")) {
						if (checkEnt->moverState == MOVER_POS1) { // stationary/closed
							hintDist = CH_DOOR_DIST;
							hintType = HINT_DOOR;

							if (checkEnt->key == -1 || !G_AllowTeamsAllowed(checkEnt, ent)) 
							{ // locked
								int i;

								hintType = HINT_DOOR_LOCKED;

								if (!checkEnt->bot_initialized)
								{
									for (i = 0; i < number_of_nodes; i++)
									{
										if (VectorDistance(ent->r.currentOrigin, nodes[i].origin) < 96)
										{
											if (ent->client->sess.sessionTeam == TEAM_AXIS)
												nodes[i].type |= NODE_AXIS_UNREACHABLE;
											else
												nodes[i].type |= NODE_ALLY_UNREACHABLE;
										}
									}

									checkEnt->bot_initialized = qtrue;
								}

								// Find a new path...
								ent->current_node = -1;
								ent->longTermGoal = -1;
							}
							else if (ent->is_bot
								&& checkEnt->key != -1 && G_AllowTeamsAllowed(checkEnt, ent))
							{
								if (ent->last_use_time < level.time && no_mount_time[ent->s.clientNum] < level.time)
									Cmd_Activate_f(ent);

								if (ent->beStill <= level.time)
									ent->beStill = level.time + 1000;

								ent->node_timeout = level.time + 10000;//6000;//4000;
								ent->bot_last_good_move_time = level.time;

								// Head for our object!
								AIMOD_Bot_Aim_At_Object( ent, checkEnt );
							}
						}
					} else if (!Q_stricmp(checkEnt->classname, "func_button")) {
						hintDist = CH_ACTIVATE_DIST;
						hintType = HINT_BUTTON;
						if (ent->is_bot)
						{
							if (ent->last_use_time < level.time  && no_mount_time[ent->s.clientNum] < level.time)
							{
								ent->beStill = level.time + 1000;
								Cmd_Activate_f(ent);
							}
						}
					} else if (!Q_stricmp(checkEnt->classname, "props_flamebarrel")) {
						hintDist = CH_BREAKABLE_DIST*2;
						hintType = HINT_BREAKABLE;

						// Head for our object!
						AIMOD_Bot_Aim_At_Object( ent, checkEnt );
						FireWeapon(ent);
					} else if (!Q_stricmp(checkEnt->classname, "props_statue")) {
						hintDist = CH_BREAKABLE_DIST*2;
						hintType = HINT_BREAKABLE;

						// Head for our object!
						AIMOD_Bot_Aim_At_Object( ent, checkEnt );
						FireWeapon(ent);
					}

					break;
				case ET_MISSILE:
				case ET_BOMB:
					if (ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER)
					{
						hintDist	= CH_BREAKABLE_DIST;
						hintType	= HINT_DISARM;
						hintVal		= checkEnt->health;		// also send health to client for visualization
						if (hintVal > 255)
							hintVal = 255;

						if (ent->is_bot && checkEnt->s.teamNum >= 4)
						{
							ent->beStill = level.time + 10000;

							AIMOD_Bot_Aim_At_Object( ent, checkEnt );

							if (ent->client->ps.weapon != WP_PLIERS)	
							{// Switch to plyers and use them!
								BOT_ChangeWeapon(ent, WP_PLIERS);
							}
		
							if (ent->client->ps.weapon == WP_PLIERS)	
							{
								FireWeapon(ent);
							}

							if (ent->bot_last_chat_time < level.time)
							{
								ent->bot_last_chat_time = level.time + 10000;
								G_Voice(ent, NULL, SAY_TEAM, va("OnDefense"), qfalse);
							}

							ent->bot_can_build = qtrue;
							ent->bot_job_wait_time = level.time + 2000;
						}
					}
					else
					{
						if (ent->bot_last_chat_time < level.time)
						{
							ent->bot_last_chat_time = level.time + 10000;
							G_Voice(ent, NULL, SAY_TEAM, va("NeedEngineer"), qfalse);
						}
					}

					// hint icon specified in entity (and proper contact was made, so hintType was set)
					// first try the checkent...
					if (checkEnt->s.dmgFlags && hintType) {
						hintType = checkEnt->s.dmgFlags;
					}

					// then the traceent
					if (traceEnt->s.dmgFlags && hintType) {
						hintType = traceEnt->s.dmgFlags;
					}

					break;
				default:
					break;
			}

			if (zooming) {
				hintDist = CH_MAX_DIST_ZOOM;

				// zooming can eat a lot of potential hints
				switch(hintType) {

					// allow while zooming
					case HINT_PLAYER:
					case HINT_TREASURE:
					case HINT_LADDER:
					case HINT_EXIT:
					case HINT_NOEXIT:
					case HINT_PLYR_FRIEND:
					case HINT_PLYR_NEUTRAL:
					case HINT_PLYR_ENEMY:
					case HINT_PLYR_UNKNOWN:
						break;

					default:
						return;
				}
			}
		}
	}

	if (dist <= Square(hintDist)) {
		ps->serverCursorHint = hintType;
		ps->serverCursorHintVal = hintVal;
	}

}
#endif

void G_SetTargetName( gentity_t* ent, char* targetname ) {
	if( targetname && *targetname ) {
		ent->targetname = targetname;
		ent->targetnamehash = BG_StringHashValue(targetname);
	} else {
		ent->targetnamehash = -1;
	}
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void ) {
	gentity_t	*e, *e2;
	int		i, j;
	int		c, c2;

	c = 0;
	c2 = 0;
	for ( i=1, e=g_entities+i ; i < level.num_entities ; i++,e++ ){
		if (!e->inuse)
			continue;
		
		if (!e->team)
			continue;

		if (e->flags & FL_TEAMSLAVE)
			continue;
		
		if (!Q_stricmp (e->classname, "func_tramcar"))
		{
			if (e->spawnflags & 8) // leader
			{
				e->teammaster = e;
			}
			else 
			{
				continue;
			}
		}
		
		c++;
		c2++;
		for (j=i+1, e2=e+1 ; j < level.num_entities ; j++,e2++)
		{
			if (!e2->inuse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (!strcmp(e->team, e2->team))
			{
				c2++;
				e2->teamchain = e->teamchain;
				e->teamchain = e2;
				e2->teammaster = e;
//				e2->key = e->key;	// (SA) I can't set the key here since the master door hasn't finished spawning yet and therefore has a key of -1
				e2->flags |= FL_TEAMSLAVE;

				if (!Q_stricmp (e2->classname, "func_tramcar"))
				{
					trap_UnlinkEntity (e2);
				}
		
				// make sure that targets only point at the master
				if ( e2->targetname ) {
					G_SetTargetName( e, e2->targetname );
					
					// Rafael
					// note to self: added this because of problems
					// pertaining to keys and double doors
					if (Q_stricmp (e2->classname, "func_door_rotating"))
						e2->targetname = NULL;
				}
			}
		}
	}

		G_Printf ("%i teams with %i entities\n", c, c2);
}


/*
==============
G_RemapTeamShaders
==============
*/
void G_RemapTeamShaders() {
}


/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void )
{
	int i;
	cvarTable_t	*cv;
	qboolean remapped = qfalse;

	level.server_settings = 0;

	trap_Cvar_Register(&g_serverInfo, "g_serverInfo", "1", CVAR_ROM);

	for (i=0, cv=gameCvarTable; i<gameCvarTableSize; i++, cv++) {

		if((g_serverInfo.integer & SIF_CHARGE) &&
			(!Q_stricmp(cv->cvarName, "g_medicChargeTime") ||	
			!Q_stricmp(cv->cvarName, "g_LTChargeTime") ||	
			!Q_stricmp(cv->cvarName, "g_engineerChargeTime") ||	
			!Q_stricmp(cv->cvarName, "g_soldierChargeTime") ||	
			!Q_stricmp(cv->cvarName, "g_covertopsChargeTime"))) {

			cv->cvarFlags |= CVAR_SERVERINFO;

		}

		trap_Cvar_Register(cv->vmCvar,
			cv->cvarName,
			cv->defaultString,
			cv->cvarFlags);
		if(cv->vmCvar) {
			cv->modificationCount = cv->vmCvar->modificationCount;
			// OSP - Update vote info for clients, if necessary
			if(!G_IsSinglePlayerGame()) {
				G_checkServerToggle(cv->vmCvar);
			}
		}

		remapped = (remapped || cv->teamShader);
	}

	if(g_serverInfo.integer & SIF_PLAYERS) {
		trap_Cvar_Register(NULL, "Players_Axis", "",
			CVAR_SERVERINFO_NOUPDATE);
		trap_Cvar_Register(NULL, "Players_Allies", "",
			CVAR_SERVERINFO_NOUPDATE);
	}

	if(g_serverInfo.integer & SIF_P) {
		trap_Cvar_Register(NULL, "P", "", CVAR_SERVERINFO_NOUPDATE);
	}

#ifdef __ETE__
	if( (g_gametype.integer == GT_WOLF_CAMPAIGN || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) &&
#else //!__ETE__
	if( g_gametype.integer == GT_WOLF_CAMPAIGN &&
#endif //__ETE__
		(g_serverInfo.integer & SIF_CAMPAIGN_MAPS)) {

		trap_Cvar_Register(NULL, "campaign_maps", "",
			CVAR_SERVERINFO_NOUPDATE);
	}
	
#ifdef __ETE__
	if( (g_gametype.integer == GT_WOLF_CAMPAIGN || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) &&
#else //!__ETE__
	if( g_gametype.integer == GT_WOLF_CAMPAIGN &&
#endif //__ETE__
			(g_serverInfo.integer & SIF_C) ) {
		trap_Cvar_Register(NULL, "C", "", CVAR_SERVERINFO_NOUPDATE);
	}

	if(remapped) {
		G_RemapTeamShaders();
	}

	// check some things
	// DHM - Gametype is currently restricted to supported types only
	if((g_gametype.integer < GT_SINGLE_PLAYER/*GT_WOLF*/ || g_gametype.integer >= GT_MAX_GAME_TYPE)) {
		G_Printf( "g_gametype %i is out of range, defaulting to GT_WOLF(%i)\n", g_gametype.integer, GT_WOLF );
		trap_Cvar_Set( "g_gametype", va("%i",GT_WOLF) );
		trap_Cvar_Update( &g_gametype );
	}

	// OSP
	if(!G_IsSinglePlayerGame()) {
		trap_SetConfigstring(CS_SERVERTOGGLES, va("%d", level.server_settings));
		if(match_readypercent.integer < 1) trap_Cvar_Set("match_readypercent", "1");
	}

	if(pmove_msec.integer < 8) {
		trap_Cvar_Set("pmove_msec", "8");
	} else if(pmove_msec.integer > 33) {
		trap_Cvar_Set("pmove_msec", "33");
	}

}

static qboolean G_IsVoteFlagCvar(cvarTable_t *cv)
{
	if(cv->vmCvar == &vote_allow_comp ||
		cv->vmCvar == &vote_allow_gametype ||
		cv->vmCvar == &vote_allow_kick ||
		cv->vmCvar == &vote_allow_map ||
		cv->vmCvar == &vote_allow_matchreset ||
		cv->vmCvar == &vote_allow_mutespecs ||
		cv->vmCvar == &vote_allow_nextmap ||
		cv->vmCvar == &vote_allow_pub ||
		cv->vmCvar == &vote_allow_referee ||
		cv->vmCvar == &vote_allow_shuffleteamsxp ||
		cv->vmCvar == &vote_allow_swapteams ||
		cv->vmCvar == &vote_allow_friendlyfire ||
		cv->vmCvar == &vote_allow_timelimit ||
		cv->vmCvar == &vote_allow_warmupdamage ||
		cv->vmCvar == &vote_allow_antilag ||
		cv->vmCvar == &vote_allow_balancedteams ||
		cv->vmCvar == &vote_allow_muting ||
		cv->vmCvar == &vote_allow_surrender ||
		cv->vmCvar == &vote_allow_restartcampaign ||
		cv->vmCvar == &vote_allow_nextcampaign ||
		cv->vmCvar == &vote_allow_poll ||
		cv->vmCvar == &vote_allow_maprestart) {

		return qtrue;
	}
	return qfalse;
}

qboolean G_IsETESkillCvar(vmCvar_t *c) {
	return (
		c == &g_skillLightWeapons ||
		c == &g_skillBattleSense ||
		c == &g_skillSoldier ||
		c == &g_skillMedic ||
		c == &g_skillEngineer ||
		c == &g_skillFieldOps ||
		c == &g_skillCovertOps
		);
}

void G_UpdateETESkillInfo() {
	char cs[MAX_INFO_STRING];
	cs[0] = '\0';
	Info_SetValueForKey(cs, "skill_lightweapons", va("%s", g_skillLightWeapons.string));
	Info_SetValueForKey(cs, "skill_battlesense",  va("%s", g_skillBattleSense.string));
	Info_SetValueForKey(cs, "skill_soldier",      va("%s", g_skillSoldier.string));
	Info_SetValueForKey(cs, "skill_medic",        va("%s", g_skillMedic.string));
	Info_SetValueForKey(cs, "skill_engineer",     va("%s", g_skillEngineer.string));
	Info_SetValueForKey(cs, "skill_fieldops",     va("%s", g_skillFieldOps.string));
	Info_SetValueForKey(cs, "skill_covertops",    va("%s", g_skillCovertOps.string));
	trap_SetConfigstring(CS_ETE_SKILLLEVELS, cs);
}

qboolean G_IsETEnhancedinfoCvar(vmCvar_t *c)
{
	return (
		c == &g_misc ||
		c == &g_doubleJumpHeight ||
		c == &g_staminaRecharge ||
		c == &g_weapons ||
		c == &g_coverts ||
		c == &g_medics ||
		c == &g_throwableKnives
		);
		
}

void G_UpdateETEnhancedinfo(void)
{
	char cs[MAX_INFO_STRING];
	cs[0] = '\0';

	Info_SetValueForKey(cs, "g_misc",va("%i", g_misc.integer));
	Info_SetValueForKey(cs, "g_doubleJumpHeight",va("%f", g_doubleJumpHeight.value));
	Info_SetValueForKey(cs, "g_staminaRecharge",va("%f", g_staminaRecharge.value));
	Info_SetValueForKey(cs, "g_weapons",va("%i", g_weapons.integer));
	Info_SetValueForKey(cs, "g_coverts",va("%i", g_coverts.integer));
	Info_SetValueForKey(cs, "g_medics",va("%i", g_medics.integer));
	Info_SetValueForKey(cs, "g_throwableKnives",va("%i", g_throwableKnives.integer));
	trap_SetConfigstring(CS_ETE_INFO, cs);
}

/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars( void )
{
	int i;
	cvarTable_t	*cv;
	qboolean fToggles = qfalse;
	qboolean fVoteFlags = qfalse;
	qboolean remapped = qfalse;
	qboolean chargetimechanged = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		if ( cv->vmCvar ) {
			trap_Cvar_Update( cv->vmCvar );

			if(cv->modificationCount != cv->vmCvar->modificationCount) {
				cv->modificationCount = cv->vmCvar->modificationCount;

				if ( cv->trackChange && !(cv->cvarFlags & CVAR_LATCH) ) {
					trap_SendServerCommand( -1, va("print \"Server:[lof] %s [lon]changed to[lof] %s\n\"", cv->cvarName, cv->vmCvar->string ) );
				}

				if (cv->teamShader) {
					remapped = qtrue;
				}

				if( cv->vmCvar == &g_filtercams ) {
					trap_SetConfigstring( CS_FILTERCAMS, va( "%i", g_filtercams.integer ) );
				}

				if( cv->vmCvar == &g_soldierChargeTime ) {
					level.soldierChargeTime[0] = g_soldierChargeTime.integer * level.soldierChargeTimeModifier[0];
					level.soldierChargeTime[1] = g_soldierChargeTime.integer * level.soldierChargeTimeModifier[1];
					chargetimechanged = qtrue;
				} else if( cv->vmCvar == &g_medicChargeTime ) {
					level.medicChargeTime[0] = g_medicChargeTime.integer * level.medicChargeTimeModifier[0];
					level.medicChargeTime[1] = g_medicChargeTime.integer * level.medicChargeTimeModifier[1];
					chargetimechanged = qtrue;
				} else if( cv->vmCvar == &g_engineerChargeTime ) {
					level.engineerChargeTime[0] = g_engineerChargeTime.integer * level.engineerChargeTimeModifier[0];
					level.engineerChargeTime[1] = g_engineerChargeTime.integer * level.engineerChargeTimeModifier[1];
					chargetimechanged = qtrue;
				} else if( cv->vmCvar == &g_LTChargeTime ) {
					level.lieutenantChargeTime[0] = g_LTChargeTime.integer * level.lieutenantChargeTimeModifier[0];
					level.lieutenantChargeTime[1] = g_LTChargeTime.integer * level.lieutenantChargeTimeModifier[1];
					chargetimechanged = qtrue;
				}
				else if( cv->vmCvar == &g_covertopsChargeTime ) {
					level.covertopsChargeTime[0] = g_covertopsChargeTime.integer * level.covertopsChargeTimeModifier[0];
					level.covertopsChargeTime[1] = g_covertopsChargeTime.integer * level.covertopsChargeTimeModifier[1];
					chargetimechanged = qtrue;
				}
				else if(cv->vmCvar == &match_readypercent) {
					if(match_readypercent.integer < 1) trap_Cvar_Set(cv->cvarName, "1");
					else if(match_readypercent.integer > 100) trap_Cvar_Set(cv->cvarName, "100");
				}
				else if(cv->vmCvar == &g_warmup) {
					if(g_gamestate.integer != GS_PLAYING && !G_IsSinglePlayerGame()) {
						level.warmupTime = level.time + (((g_warmup.integer < 10) ? 11 : g_warmup.integer + 1) * 1000);
						trap_SetConfigstring(CS_WARMUP, va("%i", level.warmupTime));
					}
				}
				// Moved this check out of the main world think loop
				else if(cv->vmCvar == &g_gametype) {
					int worldspawnflags = g_entities[ENTITYNUM_WORLD].spawnflags;
					int gt, gametype;
					char buffer[32];

					trap_Cvar_LatchedVariableStringBuffer( "g_gametype", buffer, sizeof( buffer ) );
					gametype = atoi( buffer );


#ifdef __ETE__
					if( (gametype == GT_WOLF_CAMPAIGN || gametype == GT_SUPREMACY_CAMPAIGN) &&
#else //!__ETE__
					if(gametype == GT_WOLF_CAMPAIGN &&
#endif //__ETE__
						gametype != g_gametype.integer ) {
						if( !G_MapIsValidCampaignStartMap() ) {
							gt = g_gametype.integer;
							if( gt != GT_WOLF_LMS ) {
								if( !(worldspawnflags & NO_GT_WOLF) ) {
									gt = GT_WOLF;	// Default wolf
								} else {
									gt = GT_WOLF_LMS;	// Last man standing
								}
							}

							G_Printf( "Map '%s' isn't a valid campaign start map, resetting game type to '%i'\n", level.rawmapname, gt );
							trap_Cvar_Set( "g_gametype", va( "%i", gt ) );
						}
						continue;
					}

					if(!level.latchGametype && g_gamestate.integer == GS_PLAYING && 
					  ( ( ( g_gametype.integer == GT_WOLF || g_gametype.integer == GT_WOLF_CAMPAIGN ) && (worldspawnflags & NO_GT_WOLF)) ||	
					  (g_gametype.integer == GT_WOLF_STOPWATCH && (worldspawnflags & NO_STOPWATCH)) ||
					  (g_gametype.integer == GT_WOLF_LMS && (worldspawnflags & NO_LMS)) )
					  ) {

						if( !(worldspawnflags & NO_GT_WOLF) ) {
							gt = GT_WOLF;	// Default wolf
						} else {
							gt = GT_WOLF_LMS;	// Last man standing
						}

						level.latchGametype = qtrue;
						AP("print \"Invalid gametype was specified, Restarting\n\"");
						trap_SendConsoleCommand( EXEC_APPEND, va("wait 2 ; g_gametype %i ; map_restart 10 0\n", gt ) );
					}
				} else if(cv->vmCvar == &pmove_msec) {
					if(pmove_msec.integer < 8) {
						trap_Cvar_Set(cv->cvarName, "8");
					} else if(pmove_msec.integer > 33) {
						trap_Cvar_Set(cv->cvarName, "33");
					}
				}
				else if(G_IsETEnhancedinfoCvar(cv->vmCvar)) {
					G_UpdateETEnhancedinfo();
				}
				else if ( G_IsETESkillCvar(cv->vmCvar) ) {
					G_UpdateETESkillInfo();
				}
				// OSP - Update vote info for clients, if necessary
				else if(!G_IsSinglePlayerGame()) {

					if(G_IsVoteFlagCvar(cv)) {
						fVoteFlags = qtrue;
					} else {
						fToggles = (G_checkServerToggle(cv->vmCvar) || fToggles);
					}
				}
			}
		}
	}

	if(fVoteFlags) {
		G_voteFlags();
	}

	if(fToggles) {
		trap_SetConfigstring(CS_SERVERTOGGLES, va("%d", level.server_settings));
	}

	if (remapped) {
		G_RemapTeamShaders();
	}

	if( chargetimechanged ) {
		char cs[MAX_INFO_STRING];
		cs[0] = '\0';
		Info_SetValueForKey( cs, "axs_sld", va("%i", level.soldierChargeTime[0]) );
		Info_SetValueForKey( cs, "ald_sld", va("%i", level.soldierChargeTime[1]) );
		Info_SetValueForKey( cs, "axs_mdc", va("%i", level.medicChargeTime[0]) );
		Info_SetValueForKey( cs, "ald_mdc", va("%i", level.medicChargeTime[1]) );
		Info_SetValueForKey( cs, "axs_eng", va("%i", level.engineerChargeTime[0]) );
		Info_SetValueForKey( cs, "ald_eng", va("%i", level.engineerChargeTime[1]) );
		Info_SetValueForKey( cs, "axs_lnt", va("%i", level.lieutenantChargeTime[0]) );
		Info_SetValueForKey( cs, "ald_lnt", va("%i", level.lieutenantChargeTime[1]) );
		Info_SetValueForKey( cs, "axs_cvo", va("%i", level.covertopsChargeTime[0]) );
		Info_SetValueForKey( cs, "ald_cvo", va("%i", level.covertopsChargeTime[1]) );
		trap_SetConfigstring( CS_CHARGETIMES, cs );
	}
}

// Reset particular server variables back to defaults if a config is voted in.
void G_wipeCvars(void)
{
	int			i;
	cvarTable_t	*pCvars;

	for(i=0, pCvars=gameCvarTable; i<gameCvarTableSize; i++, pCvars++) {
		if(pCvars->vmCvar && pCvars->fConfigReset) {
			G_Printf("set %s %s\n", pCvars->cvarName, pCvars->defaultString);
			trap_Cvar_Set(pCvars->cvarName, pCvars->defaultString);
		}
	}

	G_UpdateCvars();
}

//bani - #113
#define SNIPSIZE 250

//copies max num chars from beginning of dest into src and returns pointer to new src
char *strcut( char *dest, char *src, int num ) {
	int i;

	if( !dest || !src || !num )
		return NULL;
	for( i = 0 ; i < num ; i++ ) {
		if( (char)*src ) {
			*dest = *src;
			dest++;
			src++;
		} else { 
			break;
		}
	}
	*dest = (char)0;
	return src;
}

//g_{axies,allies}mapxp overflows and crashes the server
void bani_clearmapxp( void ) {
	trap_SetConfigstring( CS_AXIS_MAPS_XP, "" );
	trap_SetConfigstring( CS_ALLIED_MAPS_XP, "" );

	trap_Cvar_Set( va( "%s_axismapxp0", GAMEVERSION ), "" );
	trap_Cvar_Set( va( "%s_alliedmapxp0", GAMEVERSION ), "" );
}

void bani_storemapxp( void ) {
	char cs[MAX_STRING_CHARS];
	char u[MAX_STRING_CHARS];
	char *k;
	int i, j;

	//axis
	trap_GetConfigstring( CS_AXIS_MAPS_XP, cs, sizeof(cs) );
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		Q_strcat( cs, sizeof( cs ), va( " %i", (int)level.teamXP[ i ][ 0 ] ) );
	}
	trap_SetConfigstring( CS_AXIS_MAPS_XP, cs );

	j = 0;
	k = strcut( u, cs, SNIPSIZE );
	while( strlen( u ) ) {
		//"to be continued..."
		if( strlen( u ) == SNIPSIZE ) {
			strcat( u, "+" );
		}
		trap_Cvar_Set( va( "%s_axismapxp%i", GAMEVERSION, j ), u );
		j++;
		k = strcut( u, k, SNIPSIZE );
	}

	//allies
	trap_GetConfigstring( CS_ALLIED_MAPS_XP, cs, sizeof(cs) );
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		Q_strcat( cs, sizeof( cs ), va( " %i", (int)level.teamXP[ i ][ 1 ] ) );
	}
	trap_SetConfigstring( CS_ALLIED_MAPS_XP, cs );

	j = 0;
	k = strcut( u, cs, SNIPSIZE );
	while( strlen( u ) ) {
		//"to be continued..."
		if( strlen( u ) == SNIPSIZE ) {
			strcat( u, "+" );
		}
		trap_Cvar_Set( va( "%s_alliedmapxp%i", GAMEVERSION, j ), u );
		j++;
		k = strcut( u, k, SNIPSIZE );
	}
}

void bani_getmapxp( void ) {
	int j;
	char s[MAX_STRING_CHARS];
	char t[MAX_STRING_CHARS];

	j = 0;
	trap_Cvar_VariableStringBuffer( va( "%s_axismapxp%i", GAMEVERSION, j ), s, sizeof(s) );
	//reassemble string...
	while( strrchr( s, '+' ) ) {
		j++;
		*strrchr( s, '+' ) = (char)0;
		trap_Cvar_VariableStringBuffer( va( "%s_axismapxp%i", GAMEVERSION, j ), t, sizeof(t) );
		strcat( s, t );
	}
	trap_SetConfigstring( CS_AXIS_MAPS_XP, s );

	j = 0;
	trap_Cvar_VariableStringBuffer( va( "%s_alliedmapxp%i", GAMEVERSION, j ), s, sizeof(s) );
	//reassemble string...
	while( strrchr( s, '+' ) ) {
		j++;
		*strrchr( s, '+' ) = (char)0;
		trap_Cvar_VariableStringBuffer( va( "%s_alliedmapxp%i", GAMEVERSION, j ), t, sizeof(t) );
		strcat( s, t );
	}
	trap_SetConfigstring( CS_ALLIED_MAPS_XP, s );
}

#ifdef __PARTICLE_SYSTEM__
/*
===============
G_GenerateParticleFileList

Make a list of particle files for each client to parse since fsr
the client does not have trap_FS_GetFileList
===============
*/
static void G_GenerateParticleFileList( void )
{
	int   i, numFiles, fileLen;
	char  fileList[ MAX_PARTICLE_FILES * MAX_QPATH ];
	char  fileName[ MAX_QPATH ];
	char  *filePtr;

	numFiles = trap_FS_GetFileList( "scripts", ".particle", fileList, MAX_PARTICLE_FILES * MAX_QPATH );
	filePtr = fileList;
  
	for( i = 0; i < numFiles; i++, filePtr += fileLen + 1 )
	{
		fileLen = strlen( filePtr );
		strcpy( fileName, "scripts/" );
		strcat( fileName, filePtr );
		trap_SetConfigstring( CS_PARTICLE_FILES + i, fileName );  
	}
}
#endif //__PARTICLE_SYSTEM__

/*
============
G_InitGame

============
*/

#ifdef __BOT__
int constructible_check_time = 3000;
qboolean constructible_check_done = qfalse;
extern void AutoBuildConstruction( gentity_t* constructible );
extern void AIMOD_MAPPING_Initial_NodeLinks_Checking ( void );
extern void	AICast_Init (void);
extern void AICast_ScriptLoad ( void );
extern void AIMOD_Generate_Cover_Spots ( void );
extern void AIMOD_AI_InitNodeContentsFlags ( void );
//#ifdef __BOT_AAS__
//extern int AAS_LoadAASFile(char *filename);
//#endif //__BOT_AAS__

#ifdef _WIN32
extern void Begin_Bot_Pathfind_Queue ( void );
extern void Shutdown_Bot_Pathfind_Queue ( void );
#endif //_WIN32
#endif //__BOT__

#ifdef __NPC__
int next_NPC_check;
extern void COOP_NPC_Check ( void );
extern int Count_NPC_Players ( void );
extern qboolean G_EntitiesFree( void );
extern void NPC_Spawn( void );
//extern int next_plane_time;
#endif

#ifdef __ETE__
extern void SUPREMACY_LoadGameInfo ( void );
extern float	current_fog_view_distance;
extern qboolean fog_kludge_checked;

gentity_t *G_SecondaryEntityFor (int clientNum )
{// UQ1: If primary s.number is given, will give secondary, if secondary s.number is given it will give the primary :)
	int i;

	if (clientNum != g_entities[clientNum].s.dl_intensity)
		return &g_entities[g_entities[clientNum].s.dl_intensity];
	
	// Forced initialization...
	for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++)
	{
		gentity_t *ent = &g_entities[i];

		if (!ent)
			continue;

		if (ent->s.eType == ET_SECONDARY)
		{
			g_entities[ent->s.dl_intensity].s.dl_intensity = i;
		}
	}

	return &g_entities[g_entities[clientNum].s.dl_intensity];
}

void G_ListSecondaryEntities ( void )
{
	int i;

	for (i = 0; i < MAX_CLIENTS; i++)
	{// One for each client... Much more data we can use for each then!
		gentity_t *ent = G_SecondaryEntityFor(i);

		if (ent->s.eType == ET_SECONDARY)
			G_Printf("Client %i. Secondary is %i. [ET_SECONDARY]\n", i, g_entities[i].s.dl_intensity);
		else
			G_Printf("Client %i. Secondary is %i. [not ET_SECONDARY]\n", i, g_entities[i].s.dl_intensity);
	}

	for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++)
	{
		gentity_t *ent = &g_entities[i];

		if (!ent)
			continue;

		if (ent->s.eType == ET_SECONDARY)
			G_Printf("Secondary found (ent %i). Linked to client %i.\n", ent->s.number, ent->s.dl_intensity);
	}
}


void G_SecondaryThink ( gentity_t *ed )
{
	//ed->nextthink = level.time + 1000;
	//trap_LinkEntity( ed );
}

void G_InitSecondaryEntity (int clientNum )
{// UQ1: Init all values for this client's secondary entity!
	gentity_t *ent = G_SecondaryEntityFor(clientNum);

	// UQ1: *NOTE* The ones that are commented out probebly should never be used!
	ent->s.eType = ET_SECONDARY; // UQ1: ***NEVER*** CHANGE THIS!!!
	ent->s.dl_intensity = clientNum; // used to link to main entity! ***NEVER USE THIS***
	//ent->s.clientNum
	//ent->s.number
	//ent->s.modelindex = 0; // UQ1: Can't use this for some reason! :(
	//ent->s.modelindex2 = 0; // UQ1: Probebly can't use this either... :(
	
	/* ??? May be able to use these ones... Dunno... ??? */
	//ent->s.event
	//ent->s.eventParm
	//ent->s.eventParms
	//ent->s.events
	//ent->s.eventSequence
	/* ??? END ??? */

	ent->s.aiState = -1;
	VectorSet(ent->s.angles, 0, 0, 0);
	VectorSet(ent->s.angles2, 0, 0, 0);
	ent->s.animMovetype = -1;
	VectorSet(ent->s.apos.trBase, 0, 0, 0);
	VectorSet(ent->s.apos.trDelta, 0, 0, 0);
	ent->s.apos.trDuration = 0;
	ent->s.apos.trTime = 0;
	ent->s.apos.trType = 0;
	ent->s.constantLight = 0;
	ent->s.density =  // UQ1: Used for sprites above their heads... (BOT Coverspots and Followers)-1;
	ent->s.dmgFlags = // UQ1: Used for sprites above their heads... (BOT Followers)-1;
	ent->s.effect1Time = 0;
	ent->s.effect2Time = 0;
	ent->s.effect3Time = 0;
	ent->s.eFlags = 0;
	ent->s.frame = 0;
	ent->s.groundEntityNum = -1;
	ent->s.legsAnim = 0;
	ent->s.loopSound = 0;
	ent->s.nextWeapon = 0;
	ent->s.onFireEnd = 0;
	ent->s.onFireStart = 0;
	VectorSet(ent->s.origin, 0, 0, 0);
	VectorSet(ent->s.origin2, 0, 0, 0);
	ent->s.otherEntityNum = -1;
	ent->s.otherEntityNum2 = -1;
	VectorSet(ent->s.pos.trBase, 0, 0, 0);
	VectorSet(ent->s.pos.trDelta, 0, 0, 0);
	ent->s.pos.trDuration = 0;
	ent->s.pos.trTime = 0;
	ent->s.pos.trType = 0;
	ent->s.powerups = 0;
	ent->s.solid = 0;
	ent->s.teamNum = 0;
	ent->s.time = 0;
	ent->s.time2 = 0;
	ent->s.torsoAnim = 0;
	ent->s.weapon = 0; // Weapon model on the player's back...
	ent->r.svFlags = SVF_BROADCAST;
}

void G_CreateSecondaryEntities ( void )
{// UQ1: Create a second set of entities for each player slot... For extra data tansmission...
	int i;

	for (i = 0; i < MAX_CLIENTS; i++)
	{// One for each client... Much more data we can use for each then!
		gentity_t *newEnt = G_Spawn();
		g_entities[i].s.dl_intensity = newEnt->s.number;
		G_InitSecondaryEntity(i);
		trap_LinkEntity( newEnt );
	}
}

qboolean snowstep_kludge_map = qfalse;

void G_CheckForSnowFootstepKludge ( void )
{
	vmCvar_t mapname;
	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	if( !Q_stricmp( mapname.string, "mp_trenchtoast" ) )
		snowstep_kludge_map = qtrue;
	//else if( !Q_stricmp( cgs.mapname, "maps/mp_trenchtoast.bsp" ) )
	//	snowstep_kludge_map = qtrue;
	else
		snowstep_kludge_map = qfalse;
}
#endif //__ETE__

extern int UQ_Get_CPU_Info( void );

void G_InitGame( int levelTime, int randomSeed, int restart ) {
	int					i;
	char				cs[MAX_INFO_STRING];
	char				mapConfig[MAX_STRING_CHARS];

	if (g_dedicated.integer)
		Com_Printf ("======================== [Game Initialization] ========================\n");
	else
		G_Printf ("^4======================== ^5[^3Game Initialization^5] ^4========================\n");

	G_Printf ("gamename: %s\n", GAMEVERSION);
	G_Printf ("gamedate: %s\n", __DATE__);
	
	// Display CPU Info...
	UQ_Get_CPU_Info();

	srand( randomSeed );

#ifdef __ETE__
	// Init randomized fog!
	fog_kludge_checked = qfalse;
	current_fog_view_distance = 0;
#endif //__ETE__

	//bani - make sure pak2.pk3 gets referenced on server so pure checks pass
	trap_FS_FOpenFile( "pak2.dat", &i, FS_READ );
	trap_FS_FCloseFile( i );

	// set some level globals
	i = level.server_settings;
	{
		qboolean oldspawning = level.spawning;
		voteInfo_t votedata;

		memcpy( &votedata, &level.voteInfo, sizeof( voteInfo_t ) );

		memset( &level, 0, sizeof( level ) );

		memcpy( &level.voteInfo, &votedata, sizeof( voteInfo_t ) );

		level.spawning = oldspawning;
	}
	level.time = levelTime;
	level.startTime = levelTime;
	level.server_settings = i;

	G_RegisterCvars();

	/*{// UQ1: Added. Force com_hunkmegs to 128!
		vmCvar_t	com_hunkmegs;
		vmCvar_t	mapname;

		trap_Cvar_Register( &com_hunkmegs, "com_hunkmegs", "128", CVAR_ARCHIVE | CVAR_LATCH );
		trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

		if (com_hunkmegs.integer < 256)
		{
			if(g_dedicated.integer > 0) // UI should handle non-dedicated...
			{
				G_Printf( "^4*** ^3%s^5: Restarting server to update com_hunkmegs.\n", GAME_VERSION );
				//trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
				//trap_SendConsoleCommand( EXEC_APPEND, "in_restart\n" );
				trap_Cvar_Set( "com_hunkmegs", "256" );
				trap_SendConsoleCommand( EXEC_NOW, va( "map %s\n", mapname.string ) );
			}
		}
	}*/

	G_InitMemory();

#ifdef __BOT__
	AIMod_BOT_Init_Memory();
#endif //__BOT__
/*
// NPC Fodder, currently unused...
#ifdef __NPC__
	AIMod_NPC_Init_Memory();
#endif //__NPC__
*/

	G_UpdateETEnhancedinfo();
	G_UpdateETESkillInfo();

	if(g_camo_axis_default.string[0]) {
		trap_Cvar_Set( "g_camo_axis", g_camo_axis_default.string ); // reset camo directory before all
	} else {
		trap_Cvar_Set( "g_camo_axis", "temperate" ); // reset camo directory before all
	}
	if(g_camo_allies_default.string[0]) {
		trap_Cvar_Set( "g_camo_allies", g_camo_allies_default.string ); // reset camo directory before all
	} else {
		trap_Cvar_Set( "g_camo_allies", "temperate" ); // reset camo directory before all
	}
	
	if(g_weapons.integer & WPF_GARAND_EQUALITY) {

		// forty - allows admins to balance the clip size between the allies and axis
		// rifles. allied rifles get same clip sizes and max ammo as the k43.
	
		// maxclip
		GetAmmoTableData(WP_GARAND)->maxclip		= GetAmmoTableData(WP_K43)->maxclip;
		GetAmmoTableData(WP_GARAND_SCOPE)->maxclip	= GetAmmoTableData(WP_K43)->maxclip;
		GetAmmoTableData(WP_CARBINE)->maxclip		= GetAmmoTableData(WP_K43)->maxclip;

		// maxammo
		GetAmmoTableData(WP_GARAND)->maxammo		= GetAmmoTableData(WP_K43)->maxammo;
		GetAmmoTableData(WP_GARAND_SCOPE)->maxammo	= GetAmmoTableData(WP_K43)->maxammo;
		GetAmmoTableData(WP_CARBINE)->maxammo		= GetAmmoTableData(WP_K43)->maxammo;

		//starting clip...
		GetAmmoTableData(WP_GARAND)->defaultStartingClip		= GetAmmoTableData(WP_K43)->defaultStartingClip;
		GetAmmoTableData(WP_GARAND_SCOPE)->defaultStartingClip	= GetAmmoTableData(WP_K43)->defaultStartingClip;
		GetAmmoTableData(WP_CARBINE)->defaultStartingClip 		= GetAmmoTableData(WP_K43)->defaultStartingClip;
	
		//starting ammoreserve....
		GetAmmoTableData(WP_GARAND)->defaultStartingAmmo		= GetAmmoTableData(WP_K43)->defaultStartingAmmo;
		GetAmmoTableData(WP_GARAND_SCOPE)->defaultStartingAmmo	= GetAmmoTableData(WP_K43)->defaultStartingAmmo;
		GetAmmoTableData(WP_CARBINE)->defaultStartingAmmo 		= GetAmmoTableData(WP_K43)->defaultStartingAmmo;

	}

	// Xian enforcemaxlives stuff	
	/*
	we need to clear the list even if enforce maxlives is not active
	in case the g_maxlives was changed, and a map_restart happened
	*/
	ClearMaxLivesBans();
	
	// just for verbosity
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( g_enforcemaxlives.integer &&
			( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) ) { 
			G_Printf ( "EnforceMaxLives-Cleared GUID List\n" );
		}
	}

	G_ProcessIPBans();

	// NERVE - SMF - intialize gamestate
	if ( g_gamestate.integer == GS_INITIALIZE ) {
		// OSP
		trap_Cvar_Set( "gamestate", va( "%i", GS_WARMUP ) );
	}

	for( i =0; i < level.numConnectedClients; i++ ) {
		level.clients[ level.sortedClients[ i ] ].sess.spawnObjectiveIndex = 0;
	}

	// RF, init the anim scripting
	level.animScriptData.soundIndex = G_SoundIndex;
	level.animScriptData.playSound = G_AnimScriptSound;

	level.warmupModificationCount = g_warmup.modificationCount;

	level.soldierChargeTime[0] = level.soldierChargeTime[1] = g_soldierChargeTime.integer;
	level.medicChargeTime[0] = level.medicChargeTime[1] = g_medicChargeTime.integer;
	level.engineerChargeTime[0] = level.engineerChargeTime[1] = g_engineerChargeTime.integer;
	level.lieutenantChargeTime[0] = level.lieutenantChargeTime[1] = g_LTChargeTime.integer;

	level.covertopsChargeTime[0] = level.covertopsChargeTime[1] = g_covertopsChargeTime.integer;

	level.soldierChargeTimeModifier[0] = level.soldierChargeTimeModifier[1] = 1.f;
	level.medicChargeTimeModifier[0] = level.medicChargeTimeModifier[1] = 1.f;
	level.engineerChargeTimeModifier[0] = level.engineerChargeTimeModifier[1] = 1.f;
	level.lieutenantChargeTimeModifier[0] = level.lieutenantChargeTimeModifier[1] = 1.f;
	level.covertopsChargeTimeModifier[0] = level.covertopsChargeTimeModifier[1] = 1.f;

	cs[0] = '\0';
	Info_SetValueForKey( cs, "axs_sld", va("%i", level.soldierChargeTime[0]) );
	Info_SetValueForKey( cs, "ald_sld", va("%i", level.soldierChargeTime[1]) );
	Info_SetValueForKey( cs, "axs_mdc", va("%i", level.medicChargeTime[0]) );
	Info_SetValueForKey( cs, "ald_mdc", va("%i", level.medicChargeTime[1]) );
	Info_SetValueForKey( cs, "axs_eng", va("%i", level.engineerChargeTime[0]) );
	Info_SetValueForKey( cs, "ald_eng", va("%i", level.engineerChargeTime[1]) );
	Info_SetValueForKey( cs, "axs_lnt", va("%i", level.lieutenantChargeTime[0]) );
	Info_SetValueForKey( cs, "ald_lnt", va("%i", level.lieutenantChargeTime[1]) );
	Info_SetValueForKey( cs, "axs_cvo", va("%i", level.covertopsChargeTime[0]) );
	Info_SetValueForKey( cs, "ald_cvo", va("%i", level.covertopsChargeTime[1]) );
	trap_SetConfigstring( CS_CHARGETIMES, cs );
	trap_SetConfigstring( CS_FILTERCAMS, va( "%i", g_filtercams.integer ) );

	G_SoundIndex( "sound/misc/referee.wav"	);
	G_SoundIndex( "sound/misc/vote.wav"		);
	G_SoundIndex( "sound/player/gurp1.wav"	);
	G_SoundIndex( "sound/player/gurp2.wav"	);

	if( g_gametype.integer == GT_WOLF_LMS ) {
		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
		Info_SetValueForKey( cs, "winner", "-1" );
		trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );

		level.firstbloodTeam = -1;

		if( g_currentRound.integer == 0 ) {
			trap_Cvar_Set( "g_axiswins", "0" );
			trap_Cvar_Set( "g_alliedwins", "0" );

			trap_Cvar_Update( &g_axiswins );
			trap_Cvar_Update( &g_alliedwins );
		}

		trap_SetConfigstring( CS_ROUNDSCORES1, va("%i", g_axiswins.integer ) );
		trap_SetConfigstring( CS_ROUNDSCORES2, va("%i", g_alliedwins.integer ) );
	}

	if( g_gametype.integer == GT_WOLF ) {
		//bani - #113
		bani_clearmapxp();
	}

	if( g_gametype.integer == GT_WOLF_STOPWATCH ) {
		//bani - #113
		bani_clearmapxp();
	}


	trap_GetServerinfo( cs, sizeof( cs ) );
	Q_strncpyz( level.rawmapname, Info_ValueForKey( cs, "mapname" ), sizeof(level.rawmapname) );

	G_ParseCampaigns();
	if( g_gametype.integer == GT_WOLF_CAMPAIGN 
#ifdef __ETE__
		|| g_gametype.integer == GT_SUPREMACY_CAMPAIGN 
#endif //__ETE__
		) {
		char maps[(64*MAX_MAPS_PER_CAMPAIGN)+MAX_MAPS_PER_CAMPAIGN];
		char *map;
		int current = g_campaigns[level.currentCampaign].current;

		if(current == 0 || level.newCampaign) {

			trap_Cvar_Set( "g_axiswins", "0" );
			trap_Cvar_Set( "g_alliedwins", "0" );

			//bani - #113
			bani_clearmapxp();

			trap_Cvar_Update( &g_axiswins );
			trap_Cvar_Update( &g_alliedwins );

		} else {
			//bani - #113
			bani_getmapxp();
		}

		maps[0] = '\0';
		for(i=0; i<MAX_MAPS_PER_CAMPAIGN; i++) {
			map = g_campaigns[level.currentCampaign].mapnames[i];
			if(*map) {
				Q_strcat(maps, sizeof(maps), map);
				Q_strcat(maps, sizeof(maps), ",");
			}
		}
		if(maps[0]) 
			maps[strlen(maps)-1] = '\0';
		trap_Cvar_Set("campaign_maps", maps);

		trap_Cvar_Set("C", va("%d,%d",
				g_campaigns[level.currentCampaign].current+1,
				g_campaigns[level.currentCampaign].mapCount));
	}

	trap_SetConfigstring( CS_SCRIPT_MOVER_NAMES, "" );	// clear out

	G_DebugOpenSkillLog();

	if ( g_log.string[0] ) {
		if ( g_logSync.integer ) {
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND_SYNC );
		} else {
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND );
		}
		if ( !level.logFile ) {
			G_Printf( "WARNING: Couldn't open logfile: %s\n", g_log.string );
		} else {
			G_LogPrintf("-----------------------------------------------------------------------\n" );
			G_LogPrintf("InitGame: %s\n", cs );
		}
	} else {
			G_Printf( "Not logging to disk.\n" );
	}
	if ( g_shrubbot.string[0] ) {
		G_shrubbot_readconfig(NULL, 0);
	}

	if(g_mapConfigs.string[0]) {
		
		Q_strncpyz(mapConfig, "exec ", sizeof(mapConfig));
		Q_strcat(mapConfig, sizeof(mapConfig), g_mapConfigs.string);
		Q_strcat(mapConfig, sizeof(mapConfig), "/default.cfg\n");
		trap_SendConsoleCommand(EXEC_APPEND, mapConfig);

		Q_strncpyz(mapConfig, "exec ", sizeof(mapConfig));
		Q_strcat(mapConfig, sizeof(mapConfig), g_mapConfigs.string);
		Q_strcat(mapConfig, sizeof(mapConfig), "/");
		Q_strcat(mapConfig, sizeof(mapConfig), level.rawmapname);
		Q_strcat(mapConfig, sizeof(mapConfig), ".cfg\n");
		trap_SendConsoleCommand(EXEC_APPEND, mapConfig);
	}

	G_InitWorldSession();

	// DHM - Nerve :: Clear out spawn target config strings
	trap_GetConfigstring( CS_MULTI_INFO, cs, sizeof(cs) );
	Info_SetValueForKey( cs, "numspawntargets", "0" );
	trap_SetConfigstring( CS_MULTI_INFO, cs );

	for ( i=CS_MULTI_SPAWNTARGETS; i<CS_MULTI_SPAWNTARGETS + MAX_MULTI_SPAWNTARGETS; i++ ) {
		trap_SetConfigstring( i, "" );
	}

	G_ResetTeamMapData();

	// initialize all entities for this game
	memset( g_entities, 0, MAX_GENTITIES * sizeof(g_entities[0]) );
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients.integer;
	memset( g_clients, 0, MAX_CLIENTS * sizeof(g_clients[0]) );
	level.clients = g_clients;

	// set client fields on player ents
	for ( i=0 ; i<level.maxclients ; i++ ) {
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS;

	// let the server system know where the entites are
	trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ), 
		&level.clients[0].ps, sizeof( level.clients[0] ) );

	if (g_gametype.integer == GT_SINGLE_PLAYER) {
		char loading[4];

		trap_Cvar_VariableStringBuffer( "savegame_loading", loading, sizeof(loading) );

		if( atoi(loading) )
			saveGamePending = 2;
		else
			saveGamePending = 1;
	} else {
		saveGamePending = 0;
	}

#ifdef __BOT__
	// Ridah
	if (g_gametype.integer <= GT_COOP || g_gametype.integer == GT_NEWCOOP) {
//		char s[10];

		// Ridah, initialize cast AI system
		// DHM - Nerve :: Moved this down so that it only happens in SinglePlayer games
		AICast_Init ();
		// done.

		AICast_ScriptLoad();

/*		trap_Cvar_VariableStringBuffer( "g_missionStats", s, sizeof(s) );
		if (strlen(s) < 1) {
			// g_missionStats is used to get the player to press a key to begin
			trap_Cvar_Set( "g_missionStats", "xx" );
		}

		for(i=0;i<8;i++)	 {	// max objective cvars: 8 (FIXME: use #define somewhere)
			trap_Cvar_Set( va("g_objective%i", i+1), "0" );	// clear the objective ROM cvars
		}*/
		//trap_Cvar_Set("cg_yougotMail", "0");
	}
#endif //__BOT__

	// load level script
	G_Script_ScriptLoad();

	// reserve some spots for dead player bodies
	InitBodyQue();

	numSplinePaths = 0 ;
	numPathCorners = 0;

	G_xpsave_readconfig();
	if(!((g_XPSave.integer & XPSF_NR_EVER) ||

#ifdef __ETE__
	( (g_gametype.integer == GT_WOLF_CAMPAIGN || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) &&
#else //!__ETE__
	(g_gametype.integer == GT_WOLF_CAMPAIGN &&
#endif //__ETE__
		!(g_campaigns[level.currentCampaign].current == 0 ||
		  level.newCampaign)) ||
		
		((g_gametype.integer == GT_WOLF_STOPWATCH ||
		g_gametype.integer == GT_WOLF_LMS) &&
		g_currentRound.integer))) {
		
		G_xpsave_resetxp();
	}

	// START	Mad Doctor I changes, 8/21/2002
	// This needs to be called before G_SpawnEntitiesFromString, or the 
	// bot entities get trashed.
	//initialize the bot game entities
//	BotInitBotGameEntities();
	// END		Mad Doctor I changes, 8/21/2002

#ifdef __ETE__
	// UQ1: Create a second set of entities for each player slot... For extra data tansmission...
	G_CreateSecondaryEntities();
#endif //__ETE__

	// TAT 11/13/2002
	//		similarly set up the Server entities
	InitServerEntities();

	// parse the key/value pairs and spawn gentities
	G_SpawnEntitiesFromString();

#ifdef __PARTICLE_SYSTEM__
	// New Particle System...
	G_GenerateParticleFileList( );
#endif //__PARTICLE_SYSTEM__

	// TAT 11/13/2002 - entities are spawned, so now we can do setup
	InitialServerEntitySetup();

	// Gordon: debris test
	G_LinkDebris();

	// Gordon: link up damage parents
	G_LinkDamageParents();

	BG_ClearScriptSpeakerPool();

	BG_LoadSpeakerScript( va( "sound/maps/%s.sps", level.rawmapname ) );

	// ===================

	if( !level.gameManager ) {
		G_Printf( "^1*** ^3%s^5: ^1ERROR^5 - No 'script_multiplayer' found in map\n", GAME_VERSION );
	}

	level.tracemapLoaded = qfalse;
	if( !BG_LoadTraceMap( level.rawmapname, level.mapcoordsMins, level.mapcoordsMaxs ) ) {
		G_Printf( "^1*** ^3%s^5: ^1ERROR^5 - No tracemap found for map %s (maps/%s_tracemap.tga).\n", GAME_VERSION, level.rawmapname, level.rawmapname );
	} else {
		level.tracemapLoaded = qtrue;
		G_Printf( "^4*** ^3%s^5: Tracemap loaded for map %s (maps/%s_tracemap.tga).\n", GAME_VERSION, level.rawmapname, level.rawmapname );
	}

	// Link all the splines up
	BG_BuildSplinePaths();

	// create the camera entity that will communicate with the scripts
//	G_SpawnScriptCamera();

	// general initialization
	G_FindTeams();

	if (g_dedicated.integer)
		G_Printf ("^4=======================================================================\n");
	else
		Com_Printf ("=======================================================================\n");

	trap_PbStat ( -1 , "INIT" , "GAME" ) ;

/*
#ifndef NO_BOT_SUPPORT
	if ( bot_enable.integer ) {
		BotAISetup( restart );
#ifdef __BOT_AAS__
		BotAILoadMap( restart );
#endif //__BOT_AAS__
		G_InitBots( restart );
	}
#endif // NO_BOT_SUPPORT
*/
#ifndef NO_BOT_SUPPORT
/*	if ( bot_enable.integer ) {
		BotAISetup( restart );
//		BotAILoadMap( restart );
		G_InitBots( restart );
	}*/
	trap_Cvar_Set("bot_enable", "1");
	trap_Cvar_Set("bot_developer", "1");
	//trap_Cvar_Set("bot_debug", "1");

#ifdef __BOT_AAS__
	if ( bot_enable.integer ) {
		BotAISetup( restart );

		//trap_Cvar_Register(&mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM);
		//AAS_LoadAASFile(va("maps/%s.aas", mapname.string));

		BotAILoadMap( restart );
		G_InitBots( restart );
		trap_BotInitLevelItems();
	}
#endif //__BOT_AAS__
#endif // NO_BOT_SUPPORT

	G_RemapTeamShaders();

	BG_ClearAnimationPool();

	BG_ClearCharacterPool();

	BG_InitWeaponStrings();

	G_RegisterPlayerClasses();

	// Match init work
		G_loadMatchGame();

	// Reinstate any MV views for clients -- need to do this after all init is complete
	// --- maybe not the best place to do this... seems to be some race conditions on map_restart
	G_spawnPrintf(DP_MVSPAWN, level.time + 2000, NULL);

#ifdef __BOT__
	AIMOD_NODES_LoadNodes();//Unique1.

	if (nodes_loaded)
		CheckSequentialNodes(); // Set up sequential node info...

	if (bot_coverspot_enable.integer)
		AIMOD_Generate_Cover_Spots();

	if (number_of_nodes > 0)
		BotMP_Generate_Sniper_Spots();

	SOD_SPAWN_LoadBots();
	next_bot_check = level.time + 6000;// First check in 6 seconds.

	// Load the bot names databases...
	Load_Bot_FirstNames();
	Load_Bot_LastNames();

#ifdef __BOT_FAST_VISIBILITY__
	AIMOD_VISIBILITY_LoadVisibilities();
#endif //__BOT_FAST_VISIBILITY__

#ifdef __NPC__
//	NPC_NODES_LoadNodes();

//	next_plane_time = level.time + 5000;// This needs spawnpoint set up before it is called... So delay the first plane...
	next_NPC_check = level.time + 200;// Check every 200 mili-seconds.
	
	if (g_gametype.integer < GT_WOLF || g_gametype.integer == GT_NEWCOOP)
		COOP_NPC_Check(); // Add random npcs if the map is unsupported...
#endif //__NPC__

//	AICast_StartFrame( level.time/*ent->client->pers.cmd.serverTime*/ );
#endif //__BOT__

#ifdef __BOT__
	// We always have one extra bot main objecxtive after initgame... Remove it..
	if (numAxisObjectives > 0)
		numAxisObjectives--;

	if (numAlliedObjectives > 0)
		numAlliedObjectives--;

//	AIMOD_MAPPING_Initial_NodeLinks_Checking();
	AIMOD_AI_InitNodeContentsFlags();

	// Prepare for auto-waypointer...
	constructible_check_time = level.time + 1000;
	constructible_check_done = qfalse;

#ifdef _WIN32
	// Begin the bot pathfind thread...
	Begin_Bot_Pathfind_Queue();
#endif //_WIN32
#endif //__BOT__

#ifdef __ETE__
	if (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
	{
		redtickets = (g_redTickets.integer * (g_ticketPercent.integer*0.01)) * g_redTicketRatio.integer;
		bluetickets = (g_redTickets.integer * (g_ticketPercent.integer*0.01)) * g_blueTicketRatio.integer;
	}
#endif //__ETE__

#ifdef __ETE__
	// UQ1: Load supremacy info files...
	if ((g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) 
		&& GetNumberOfPOPFlags() <= 0)
		SUPREMACY_LoadGameInfo();

	G_CheckForSnowFootstepKludge();

	UQ_DoFogVariation();
#endif //__ETE__

	if (sizeof(playerState_t) != 1452) // UQ1: Playerstate size check - so we can modify playerstate!!!
		G_Error("^1ERROR^5: Playerstate: size is ^3%i^5. Should be ^71452^5!\n", sizeof(playerState_t));
}



/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart ) {

#ifdef __BOT__
	if (bot_auto_waypoint.integer)
		AIMOD_NODES_SaveNodes();

	SOD_SPAWN_SaveBots();
#endif //__BOT__

	// Arnout: gametype latching
	if	( 
		( ( g_gametype.integer == GT_WOLF || g_gametype.integer == GT_WOLF_CAMPAIGN ) && (g_entities[ENTITYNUM_WORLD].r.worldflags & NO_GT_WOLF)) ||
		(g_gametype.integer == GT_WOLF_STOPWATCH && (g_entities[ENTITYNUM_WORLD].r.worldflags & NO_STOPWATCH)) ||
		(g_gametype.integer == GT_WOLF_LMS && (g_entities[ENTITYNUM_WORLD].r.worldflags & NO_LMS))		
		) {

		if ( !(g_entities[ENTITYNUM_WORLD].r.worldflags & NO_GT_WOLF) )
			trap_Cvar_Set( "g_gametype", va("%i", GT_WOLF) );
		else
			trap_Cvar_Set( "g_gametype", va("%i", GT_WOLF_LMS) );

		trap_Cvar_Update( &g_gametype );
	}

	G_Printf ("==== ShutdownGame ====\n");

	G_DebugCloseSkillLog();

	if ( level.logFile ) {
		G_LogPrintf("ShutdownGame:\n" );
		G_LogPrintf("-----------------------------------------------------------------------\n" );
		trap_FS_FCloseFile( level.logFile );
		level.logFile = 0;
	}

	// write all the client session data so we can get it back
	G_WriteSessionData( restart );

#ifndef NO_BOT_SUPPORT
	if ( bot_enable.integer ) {
		BotAIShutdown( restart );
	}
#endif // NO_BOT_SUPPORT

	// zinx - realistic hitboxes
	mdx_cleanup();

	G_shrubbot_cleanup();
	G_xpsave_cleanup();

#ifdef __BOT__
#ifdef _WIN32
	// Shutdown the pathfind thread...
	Shutdown_Bot_Pathfind_Queue();
#endif //_WIN32

	AIMod_BOT_Free_Memory();
#endif //__BOT__
/*
// NPC Fodder, currently unused...
#ifdef __NPC__
	AIMod_NPC_Free_Memory();
#endif //__NPC__
*/

	// Init memory!
	BG_MemoryClose();
}



//===================================================================

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and bg_*.c can link

void QDECL Com_Error ( int level, const char *error, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	Q_vsnprintf (text, sizeof(text), error, argptr);
	va_end (argptr);

	G_Error( "%s", text);
}
//bani
void QDECL Com_Error( int level, const char *error, ... )_attribute((format(printf,2,3)));

void QDECL Com_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

	G_Printf ("%s", text);
}
//bani  
void QDECL Com_Printf( const char *msg, ... )_attribute((format(printf,1,2)));

#endif

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
SortRanks

=============
*/
int QDECL SortRanks( const void *a, const void *b ) {
	gclient_t	*ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( /*ca->sess.spectatorState == SPECTATOR_SCOREBOARD ||*/ ca->sess.spectatorClient < 0 ) {
		return 1;
	}
	if ( /*cb->sess.spectatorState == SPECTATOR_SCOREBOARD ||*/ cb->sess.spectatorClient < 0  ) {
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING ) {
		return 1;
	}
	if ( cb->pers.connected == CON_CONNECTING ) {
		return -1;
	}


	// then spectators
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		if ( ca->sess.spectatorTime < cb->sess.spectatorTime ) {
			return -1;
		}
		if ( ca->sess.spectatorTime > cb->sess.spectatorTime ) {
			return 1;
		}
		return 0;
	}
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR ) {
		return 1;
	}
	if ( cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		return -1;
	}

	if( g_gametype.integer == GT_WOLF_LMS ) {
		// then sort by score
		if ( ca->ps.persistant[PERS_SCORE]
			> cb->ps.persistant[PERS_SCORE] ) {
			return -1;
		}
		if ( ca->ps.persistant[PERS_SCORE]
			< cb->ps.persistant[PERS_SCORE] ) {
			return 1;
		}
	} else {
		int i, totalXP[2];

		for( totalXP[0] = totalXP[1] = 0, i = 0; i < SK_NUM_SKILLS; i++ ) {
			totalXP[0] += ca->sess.skillpoints[i];
			totalXP[1] += cb->sess.skillpoints[i];
		}

		// then sort by xp
		if ( totalXP[0] > totalXP[1] ) {
			return -1;
		}
		if ( totalXP[0] < totalXP[1] ) {
			return 1;
		}
	}
	return 0;
}

//bani - #184
//(relatively) sane replacement for OSP's Players_Axis/Players_Allies
void etpro_PlayerInfo( void ) {
	//128 bits
	char playerinfo[MAX_CLIENTS + 1];
	gentity_t *e;
	team_t playerteam;
	int i;
	int lastclient;

	memset( playerinfo, 0, sizeof( playerinfo ) );

	lastclient = -1;
	e = &g_entities[0];
	for ( i = 0; i < MAX_CLIENTS; i++, e++ ) {
		if( e->client == NULL || e->client->pers.connected == CON_DISCONNECTED ) {
			playerinfo[i] = '-';
			continue;
		}

		//keep track of highest connected/connecting client
		lastclient = i;

		if( e->inuse == qfalse ) {
			playerteam = 0;
		} else {
			playerteam = e->client->sess.sessionTeam;
		}
		playerinfo[i] = (char)'0' + playerteam;
	}
	//terminate the string, if we have any non-0 clients
	if( lastclient != -1 ) {
		playerinfo[lastclient+1] = (char)0;
	} else {
		playerinfo[0] = (char)0;
	}

	trap_Cvar_Set( "P", playerinfo );
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void ) {
	int		i;
//	int		rank;
//	int		score;
//	int		newScore;
	char	teaminfo[TEAM_NUM_TEAMS][256];	// OSP
	gclient_t	*cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
	level.voteInfo.numVotingClients = 0;		// don't count bots

	level.numFinalDead[0] = 0;		// NERVE - SMF
	level.numFinalDead[1] = 0;		// NERVE - SMF

	level.voteInfo.numVotingTeamClients[ 0 ] = 0;
	level.voteInfo.numVotingTeamClients[ 1 ] = 0;

	for ( i = 0; i < TEAM_NUM_TEAMS; i++ ) {
		if( i < 2 ) {
			level.numTeamClients[i] = 0;
		}
		teaminfo[i][0] = 0;			// OSP
	}

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_DISCONNECTED ) {
			int team = level.clients[i].sess.sessionTeam;

			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;

			if ( team != TEAM_SPECTATOR ) {
				level.numNonSpectatorClients++;
			
				// OSP
				Q_strcat(teaminfo[team], sizeof(teaminfo[team])-1, va("%d ", level.numConnectedClients));
			
				// decide if this should be auto-followed
				if ( level.clients[i].pers.connected == CON_CONNECTED ) {
					int teamIndex = level.clients[i].sess.sessionTeam == TEAM_AXIS ? 0 : 1;
					level.numPlayingClients++;
					if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
						level.voteInfo.numVotingClients++;
					}

					if( level.clients[i].sess.sessionTeam == TEAM_AXIS ||
						level.clients[i].sess.sessionTeam == TEAM_ALLIES ) {
						if( g_gametype.integer == GT_WOLF_LMS ) {
							if( g_entities[i].health <= 0 || level.clients[i].ps.pm_flags & PMF_LIMBO ) {
								level.numFinalDead[teamIndex]++;
							}
						} else {
							if( level.clients[i].ps.persistant[PERS_RESPAWNS_LEFT] == 0 && g_entities[i].health <= 0 ) {
								level.numFinalDead[teamIndex]++;
							}
						}

						level.numTeamClients[teamIndex]++;
						if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
							level.voteInfo.numVotingTeamClients[ teamIndex ]++;
						}
					}

					if ( level.follow1 == -1 ) {
						level.follow1 = i;
					} else if ( level.follow2 == -1 ) {
						level.follow2 = i;
					}
				}
			}
		}
	}

	// OSP
	for(i=0; i<TEAM_NUM_TEAMS; i++) {
		if(0 == teaminfo[i][0]) Q_strncpyz(teaminfo[i], "(None)", sizeof(teaminfo[i]));
	}

	qsort( level.sortedClients, level.numConnectedClients, 
		sizeof(level.sortedClients[0]), SortRanks );

	// set the rank value for all clients that are connected and not spectators
		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
		for ( i = 0;  i < level.numConnectedClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
		if ( level.teamScores[TEAM_AXIS] == level.teamScores[TEAM_ALLIES] ) {
				cl->ps.persistant[PERS_RANK] = 2;
		} else if ( level.teamScores[TEAM_AXIS] > level.teamScores[TEAM_ALLIES] ) {
				cl->ps.persistant[PERS_RANK] = 0;
			} else {
				cl->ps.persistant[PERS_RANK] = 1;
			}
		}

	// set the CS_SCORES1/2 configstrings, which will be visible to everyone
//	trap_SetConfigstring( CS_SCORES1, va("%i", level.teamScores[TEAM_AXIS] ) );
//	trap_SetConfigstring( CS_SCORES2, va("%i", level.teamScores[TEAM_ALLIES] ) );

	trap_SetConfigstring( CS_FIRSTBLOOD, va("%i", level.firstbloodTeam ) );
	trap_SetConfigstring( CS_ROUNDSCORES1, va("%i", g_axiswins.integer ) );
	trap_SetConfigstring( CS_ROUNDSCORES2, va("%i", g_alliedwins.integer ) );

	trap_Cvar_Set("Players_Axis", teaminfo[TEAM_AXIS]);
	trap_Cvar_Set("Players_Allies", teaminfo[TEAM_ALLIES]);

	//bani - #184
	etpro_PlayerInfo();

	// if we are at the intermission, send the new info to everyone
	if( g_gamestate.integer == GS_INTERMISSION ) {
		SendScoreboardMessageToAllClients();
	} else {
		// see if it is time to end the level
		CheckExitRules();
	}
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void ) {
	int		i;

	for(i=0; i<level.numConnectedClients; i++) {
		if(level.clients[level.sortedClients[i]].pers.connected == CON_CONNECTED) {
			level.clients[level.sortedClients[i]].wantsscore = qtrue;
//			G_SendScore(g_entities + level.sortedClients[i]);
		}
	}
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent ) {
//	float			timeLived;

	// take out of follow mode if needed
	if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
		StopFollowing( ent );
	}

	/*if ( ent->client->sess.sessionTeam == TEAM_AXIS || ent->client->sess.sessionTeam == TEAM_ALLIES ) {
		timeLived = (level.time - ent->client->pers.lastSpawnTime) * 0.001f;
	
		G_AddExperience( ent, min((timeLived * timeLived) * 0.00005f, 5) );
	}*/

	// move to the spot
	VectorCopy( level.intermission_origin, ent->s.origin );
	VectorCopy( level.intermission_origin, ent->client->ps.origin );
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pm_type = PM_INTERMISSION;

	// clean up powerup info
	// memset( ent->client->ps.powerups, 0, sizeof(ent->client->ps.powerups) );

	ent->client->ps.eFlags = 0;
	ent->s.eFlags = 0;
	ent->s.eType = ET_GENERAL;
	ent->s.modelindex = 0;
	ent->s.loopSound = 0;
	ent->s.event = 0;
	ent->s.events[0] = ent->s.events[1] = ent->s.events[2] = ent->s.events[3] = 0;		// DHM - Nerve
	ent->r.contents = 0;

	// todo: call bot routine so they can process the transition to intermission (send voice chats, etc)
	BotMoveToIntermission( ent->s.number );
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/

void FindIntermissionPoint( void ) {
	gentity_t	*ent = NULL, *target;
	vec3_t		dir;
	char		cs[MAX_STRING_CHARS];		// DHM - Nerve
	char		*buf;						// DHM - Nerve
	int			winner;						// DHM - Nerve

	// NERVE - SMF - if the match hasn't ended yet, and we're just a spectator
	if( !level.intermissiontime ) {
		// try to find the intermission spawnpoint with no team flags set
		ent = G_Find( NULL, FOFS(classname), "info_player_intermission" );

		for( ; ent; ent = G_Find (ent, FOFS(classname), "info_player_intermission") ) {
			if( !ent->spawnflags )
				break;
		}
	}

	trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
	buf = Info_ValueForKey( cs, "winner" );
	winner = atoi( buf );

	// Change from scripting value for winner (0==AXIS, 1==ALLIES) to spawnflag value
	if( winner == 0 ) {
		winner = TEAM_AXIS;
	} else {
		winner = TEAM_ALLIES;
	}


	if( !ent ) {
		ent = G_Find( NULL, FOFS(classname), "info_player_intermission" );
		while( ent ) {
			if( ent->spawnflags & winner ) {
				break;
			}

			ent = G_Find( ent, FOFS(classname), "info_player_intermission" );
		}
	}

	if( !ent ) {	// the map creator forgot to put in an intermission point...
		SelectSpawnPoint ( vec3_origin, level.intermission_origin, level.intermission_angle );
	} else {
		VectorCopy( ent->s.origin, level.intermission_origin );
		VectorCopy( ent->s.angles, level.intermission_angle );
		// if it has a target, look towards it
		if ( ent->target ) {
			target = G_PickTarget( ent->target );
			if ( target ) {
				VectorSubtract( target->s.origin, level.intermission_origin, dir );
				vectoangles( dir, level.intermission_angle );
			}
		}
	}

}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void ) {
	int			i;
	gentity_t	*client;
	int itime;

	if( g_gamestate.integer == GS_INTERMISSION ) {
		return;		// already active
	}

	level.intermissiontime = level.time;

	// tjw: jaybird's fix for tricking the client into drawing
	//      the correct countdown timer.
	itime = level.intermissiontime;
	if(g_intermissionTime.integer > 0) {
		itime -= (60000 - (g_intermissionTime.integer * 1000));
	}
	trap_SetConfigstring(CS_INTERMISSION_START_TIME,
		va("%i", itime));
	trap_Cvar_Set("gamestate", va( "%i", GS_INTERMISSION));
	trap_Cvar_Update( &g_gamestate );

	FindIntermissionPoint();

	// move all clients to the intermission point
	for (i=0 ; i< level.maxclients ; i++) {
		client = g_entities + i;
		if (!client->inuse)
			continue;
		MoveClientToIntermission( client );
	}

	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();

}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar 

=============
*/
void ExitLevel (void) {
	int		i;
	gclient_t *cl;

#ifdef __ETE__
	if( (g_gametype.integer == GT_WOLF_CAMPAIGN || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) ) {
#else //!__ETE__
	if(g_gametype.integer == GT_WOLF_CAMPAIGN) {
#endif //__ETE__
		g_campaignInfo_t *campaign = &g_campaigns[level.currentCampaign];

		if( campaign->current + 1 < campaign->mapCount ) {
			trap_Cvar_Set( "g_currentCampaignMap", va( "%i", campaign->current + 1 ) );

#ifdef __BOT__
				SOD_SPAWN_SaveBots(); // This actually kicks them...
#endif //__BOT__

#if 0
			if( g_developer.integer )
				trap_SendConsoleCommand( EXEC_APPEND, va( "devmap %s\n", campaign->mapnames[campaign->current + 1] ) );
			else
#endif
				trap_SendConsoleCommand( EXEC_APPEND, va( "map %s\n", campaign->mapnames[campaign->current + 1] ) );

#ifdef __BOT__
				SOD_SPAWN_SaveBots(); // This actually kicks them...
#endif //__BOT__

		} else {
			char s[MAX_STRING_CHARS];
			trap_Cvar_VariableStringBuffer( "nextcampaign", s, sizeof(s) );

			if( *s ) {
				trap_SendConsoleCommand( EXEC_APPEND, "vstr nextcampaign\n" );
			} else {
				// restart the campaign
				trap_Cvar_Set( "g_currentCampaignMap", "0" );
#if 0
				if( g_developer.integer )
					trap_SendConsoleCommand( EXEC_APPEND, va( "devmap %s\n", campaign->mapnames[0] ) );
				else
#endif
					trap_SendConsoleCommand( EXEC_APPEND, va( "map %s\n", campaign->mapnames[0] ) );

#ifdef __BOT__
					SOD_SPAWN_SaveBots(); // This actually kicks them...
#endif //__BOT__

			}

			// FIXME: do we want to do something else here?
			//trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
		}
	} else if( g_gametype.integer == GT_WOLF_LMS ) {
		if( level.lmsDoNextMap ) {
			trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
		} else {
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		}
	} else {
		trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
	}
	level.changemap = NULL;
	level.intermissiontime = 0;

	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_AXIS] = 0;
	level.teamScores[TEAM_ALLIES] = 0;
#ifdef __ETE__
	if( g_gametype.integer != GT_WOLF_CAMPAIGN && g_gametype.integer != GT_SUPREMACY_CAMPAIGN ) {
#else //!__ETE__
	if( g_gametype.integer != GT_WOLF_CAMPAIGN ) {
#endif //__ETE__
		for ( i=0 ; i< g_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			cl->ps.persistant[PERS_SCORE] = 0;
		}
	}

	// we need to do this here before chaning to CON_CONNECTING
	G_WriteSessionData( qfalse );

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for (i=0 ; i< g_maxclients.integer ; i++) {

		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			level.clients[i].pers.connected = CON_CONNECTING;
			trap_UnlinkEntity(&g_entities[i]);
		}
	}

	G_LogPrintf( "ExitLevel: executed\n" );
}

// dvl - real time stamp
char *G_GetRealTime(void) 
{
	qtime_t tm;

	trap_RealTime(&tm);
	return va("%2i:%s%i:%s%i",
				tm.tm_hour,
				(tm.tm_min > 9 ? "" : "0"),// minute padding
				tm.tm_min,
				(tm.tm_sec > 9 ? "" : "0"),// second padding
				tm.tm_sec );
}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_LogPrintf( const char *fmt, ... ) {
	va_list		argptr;
	char		string[1024];
	int			min, tens, sec, l;

	sec = level.time / 1000;

	// dvl - real time stamps
	if(g_logOptions.integer & LOGOPTS_REALTIME) {
		Com_sprintf( string, sizeof(string), "%s ", G_GetRealTime() );
	} else {
	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;
		Com_sprintf( string, 
				sizeof(string), 
				"%i:%i%i ", 
				min, tens, sec );
	}

	l = strlen( string );

	va_start( argptr, fmt );
	Q_vsnprintf( string + l, sizeof( string ) - l, fmt, argptr );
	va_end( argptr );

	if ( g_dedicated.integer ) {
		G_Printf( "%s", string + l );
	}

	if ( !level.logFile ) {
		return;
	}

	trap_FS_Write( string, strlen( string ), level.logFile );
}
//bani
void QDECL G_LogPrintf( const char *fmt, ... )_attribute((format(printf,1,2)));

void QDECL G_LogPrintf2( const char *fmt, ... ) {
	va_list		argptr;
	char		string[1024];
	int			min, tens, sec, l;

	sec = level.time / 1000;

	// dvl - real time stamps
	if(g_logOptions.integer & LOGOPTS_REALTIME) {
		Com_sprintf( string, sizeof(string), "%s ", G_GetRealTime() );
	} else {
	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;
		Com_sprintf( string, 
				sizeof(string), 
				"%i:%i%i ", 
				min, tens, sec );
	}

	l = strlen( string );

	va_start( argptr, fmt );
	Q_vsnprintf( string + l, sizeof( string ) - l, fmt, argptr );
	va_end( argptr );

	/*if ( g_dedicated.integer ) {
		G_Printf( "%s", string + l );
	}*/

	if ( !level.logFile ) {
		return;
	}

	trap_FS_Write( string, strlen( string ), level.logFile );
}
//bani
void QDECL G_LogPrintf2( const char *fmt, ... )_attribute((format(printf,1,2)));

// tjw: returns the max number of seconds remaining in a dynamite
//      plant that would end the round if it were to explode
//      
//      returns -1 if there are no round-ending plants
int G_EndRoundCountdown()
{
	int i = 0;
	int j = 0;
	gentity_t *e = &g_entities[MAX_CLIENTS];
	vec3_t origin = {0,0,0};
	vec3_t mins = {0,0,0};
	vec3_t maxs = {0,0,0};
	int touch[MAX_GENTITIES];
	int num;
	gentity_t *hit = NULL;
	int think = -1;
	int	globalAccumBuffer[MAX_SCRIPT_ACCUM_BUFFERS];
	int scriptAccumBuffer[G_MAX_SCRIPT_ACCUM_BUFFERS];

	level.testEndRound = qtrue;
	level.testEndRoundResult = qfalse;
	memcpy(globalAccumBuffer, level.globalAccumBuffer,
		sizeof(globalAccumBuffer));
	memcpy(scriptAccumBuffer, level.gameManager->scriptAccumBuffer,
		sizeof(scriptAccumBuffer));
	for(i = MAX_CLIENTS; i<level.num_entities; i++, e++) {
		if(!e->inuse)
			continue;
		if(Q_stricmp(e->classname, "dynamite"))
			continue;
		// not armed
		if(e->s.teamNum >= 4)
			continue;
		VectorCopy(e->r.currentOrigin, origin);
		SnapVector(origin);
		VectorAdd(origin, e->r.mins, mins);
		VectorAdd(origin, e->r.maxs, maxs);
		num = trap_EntitiesInBox(mins, maxs, touch, MAX_GENTITIES);
		for(j=0; j<num ; j++ ) {
			hit = &g_entities[touch[j]];
			if(!( hit->r.contents & CONTENTS_TRIGGER)) 
				continue;
			if(!(hit->s.eType == ET_OID_TRIGGER))
				continue;
			if (!(hit->spawnflags & (AXIS_OBJECTIVE|ALLIED_OBJECTIVE)))
				continue;
			// only if it targets a func_explosive
			if(hit->target_ent &&
				Q_stricmp( hit->target_ent->classname, "func_explosive"))
				continue;
			// spawnflags 128 = disabled
			if((hit->spawnflags & 128))
				continue;
			G_UseTargets(hit, e);
			if(e->nextthink > think) {
				think = e->nextthink;
			}
		}
	}
	level.testEndRound = qfalse;
	memcpy(level.globalAccumBuffer, globalAccumBuffer,
		sizeof(globalAccumBuffer));
	memcpy(level.gameManager->scriptAccumBuffer, scriptAccumBuffer,
		sizeof(scriptAccumBuffer));

	if(level.testEndRoundResult && (think >= level.time)) {
		level.testEndRoundResult = qfalse;
		return ((think - level.time) / 1000);
	}
	level.testEndRoundResult = qfalse;
	return -1;
}

qboolean G_SuddenDeath()
{
	int remaining = G_EndRoundCountdown();

	if (!(g_dyno.integer & DYNO_SUDDENDEATH))
		return qfalse;

	if(remaining >= 0) {
		if(!(level.time % 1000))
			AP(va("cp \"^1Sudden Death! (%d)\" 1", remaining));
		return qtrue;
	}
	return qfalse;

}

/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string ) {
	int				i;
	gclient_t		*cl;
	char			cs[MAX_STRING_CHARS];
	g_mapstat_t *mapstat = G_xpsave_mapstat(level.rawmapname);
	int winner = -1;

	// NERVE - SMF - do not allow LogExit to be called in non-playing gamestate
	if ( g_gamestate.integer != GS_PLAYING )
		return;

	G_LogPrintf( "Exit: %s\n", string );

	level.intermissionQueued = level.time;

	// this will keep the clients from playing any voice sounds
	// that will get cut off when the queued intermission starts
	trap_SetConfigstring( CS_INTERMISSION, "1" );

	for( i = 0; i < level.numConnectedClients; i++ ) {
		int		ping;

		cl = &level.clients[level.sortedClients[i]];

		// rain - #105 - use G_MakeUnready instead
		G_MakeUnready(&g_entities[level.sortedClients[i]]);

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		if ( cl->pers.connected == CON_CONNECTING ) {
			continue;
		}

		// CHRUKER: b016 - Make sure all the stats are recalculated
		//          and accurate
		G_CalcRank(cl);

		ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

		G_LogPrintf( "score: %i  ping: %i  client: %i %s\n", cl->ps.persistant[PERS_SCORE], ping, level.sortedClients[i], cl->pers.netname );
	}

	// CHRUKER: b016 - Moved here because we needed the stats to be
	//          up-to-date before sending	
	// NERVE - SMF - send gameCompleteStatus message to master servers
	trap_SendConsoleCommand( EXEC_APPEND, "gameCompleteStatus\n" );

	G_LogPrintf("red:%i  blue:%i\n",
		level.teamScores[TEAM_AXIS],
		level.teamScores[TEAM_ALLIES]);

	trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
	winner = atoi( Info_ValueForKey( cs, "winner" ) );


	// NERVE - SMF
	if( g_gametype.integer == GT_WOLF_STOPWATCH ) {
		char	cs[MAX_STRING_CHARS];
		int		defender;

		trap_GetConfigstring( CS_MULTI_INFO, cs, sizeof(cs) );
		defender = atoi( Info_ValueForKey( cs, "defender" ) );

		// NERVE - SMF
		if ( !g_currentRound.integer ) {
			if ( winner == defender ) {
				// if the defenders won, use default timelimit
				trap_Cvar_Set( "g_nextTimeLimit", va( "%f", g_timelimit.value ) );
			}
			else {
				// use remaining time as next timer
				trap_Cvar_Set( "g_nextTimeLimit", va( "%f", (level.timeCurrent - level.startTime) / 60000.f ) );
			}
		}
		else {
			// reset timer
			trap_Cvar_Set( "g_nextTimeLimit", "0" );
		}

		trap_Cvar_Set( "g_currentRound", va( "%i", !g_currentRound.integer ) );

		//bani - #113
		bani_storemapxp();
	}
	// -NERVE - SMF

#ifdef __ETE__
	else if( (g_gametype.integer == GT_WOLF_CAMPAIGN || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)) {
#else //!__ETE__
	else if(g_gametype.integer == GT_WOLF_CAMPAIGN) {
#endif //__ETE__
		int		highestskillpoints, highestskillpointsclient, j, teamNum;
		// CHRUKER: b017 - Preventing medals from being handed out left and right
		int		highestskillpointsincrease = 0;

		if( winner == 0 ) {
			//mapstat->axis_wins++;
			g_axiswins.integer |= (1 << g_campaigns[level.currentCampaign].current);
			trap_Cvar_Set( "g_axiswins", va( "%i", g_axiswins.integer ) );
			trap_Cvar_Update( &g_axiswins );
		} else if( winner == 1 ) {
			//mapstat->allied_wins++;
			g_alliedwins.integer |= (1 << g_campaigns[level.currentCampaign].current);
			trap_Cvar_Set( "g_alliedwins", va( "%i", g_alliedwins.integer ) );
			trap_Cvar_Update( &g_alliedwins );
		}

		trap_SetConfigstring( CS_ROUNDSCORES1, va("%i", g_axiswins.integer ) );
		trap_SetConfigstring( CS_ROUNDSCORES2, va("%i", g_alliedwins.integer ) );

		//bani - #113
		bani_storemapxp();

		// award medals
		for( teamNum = TEAM_AXIS; teamNum <= TEAM_ALLIES; teamNum++ ) {
			for( i = 0; i < SK_NUM_SKILLS; i++ ) {
				highestskillpoints = 0;
				highestskillpointsclient = -1;
				for( j = 0; j < level.numConnectedClients; j++ ) {
					cl = &level.clients[level.sortedClients[j]];

					if( cl->sess.sessionTeam != teamNum )
						continue;

					// CHRUKER: b017 
					if( cl->sess.skill[i] < 1 )
						continue;
					// CHRUKER: b017
					if ( i == SK_BATTLE_SENSE || i == SK_LIGHT_WEAPONS ) {
					if( cl->sess.skillpoints[i] > highestskillpoints ) {
						highestskillpoints = cl->sess.skillpoints[i];
						highestskillpointsclient = j;
					}
				}
					else {
						if( (cl->sess.skillpoints[i] - cl->sess.startskillpoints[i]) > highestskillpointsincrease ) {
							highestskillpointsincrease = (cl->sess.skillpoints[i] - cl->sess.startskillpoints[i]);
							highestskillpointsclient = j; 
						}
					} // b017
				}

				if( highestskillpointsclient >= 0 ) {
					// highestskillpointsclient is the first client that has this highest
					// score. See if there are more clients with this same score. If so,
					// give them medals too
					for( j = highestskillpointsclient; j < level.numConnectedClients; j++ ) {
						cl = &level.clients[level.sortedClients[j]];

						if( cl->sess.sessionTeam != teamNum )
							continue;

						 // CHRUKER: b017 
						if( cl->sess.skill[i] < 1 )
							continue;

						 // CHRUKER: b017 
						if ( i == SK_BATTLE_SENSE || i == SK_LIGHT_WEAPONS ) {
						if( cl->sess.skillpoints[i] == highestskillpoints ) {
							cl->sess.medals[i]++;

							ClientUserinfoChanged( level.sortedClients[j] );
						}
					}
						else {
							if( (cl->sess.skillpoints[i] - cl->sess.startskillpoints[i]) == highestskillpointsincrease ) {
								cl->sess.medals[i]++;
								ClientUserinfoChanged( level.sortedClients[j] );
							}
						} // b017
					}
				}
			}
		}
	} else if( g_gametype.integer == GT_WOLF_LMS ) {
		int		roundLimit = g_lms_roundlimit.integer < 3 ? 3 : g_lms_roundlimit.integer;
		int		numWinningRounds = (roundLimit / 2) + 1;

		roundLimit -= 1;	// -1 as it starts at 0

		if( winner == -1 ) {
			// who drew first blood?
			if( level.firstbloodTeam == TEAM_AXIS )
				winner = 0;
			else
				winner = 1;
		}

		if( winner == 0 ) {
			//mapstat->axis_wins++;
			trap_Cvar_Set( "g_axiswins", va( "%i", g_axiswins.integer + 1 ) );
			trap_Cvar_Update( &g_axiswins );
		} else {
			//mapstat->allied_wins++;
			trap_Cvar_Set( "g_alliedwins", va( "%i", g_alliedwins.integer + 1 ) );
			trap_Cvar_Update( &g_alliedwins );
		}

		if( g_currentRound.integer >= roundLimit || g_axiswins.integer == numWinningRounds || g_alliedwins.integer == numWinningRounds ) {
			trap_Cvar_Set( "g_currentRound", "0" );
			if( g_lms_currentMatch.integer + 1 >= g_lms_matchlimit.integer ) {
				trap_Cvar_Set( "g_lms_currentMatch", "0" );
				level.lmsDoNextMap = qtrue;
			} else {
				trap_Cvar_Set( "g_lms_currentMatch", va( "%i", g_lms_currentMatch.integer + 1 ) );
				level.lmsDoNextMap = qfalse;
			}
		} else {
			trap_Cvar_Set( "g_currentRound", va( "%i", g_currentRound.integer + 1 ) );
			trap_Cvar_Update( &g_currentRound );
		}
	} else if( g_gametype.integer == GT_WOLF ) {
 		//if(winner == 0)
 		//	mapstat->axis_wins++;
 		//else if(winner == 1)
 		//	mapstat->allied_wins++;
  		//bani - #113
  		bani_storemapxp();
	}

	G_BuildEndgameStats();

 	if(level.numPlayingClients >=
 		g_playerRating_minplayers.integer && mapstat) {
 
 		if(winner == 0)
 			mapstat->axis_wins++;
 		else if(winner == 1)
 			mapstat->allied_wins++;
 	}
 
 	//Josh: Calculate playerratings
 	if (g_playerRating.integer) {
 		G_CalculatePlayerRatings();
 	}
 
 	for( i = 0; i < level.numConnectedClients; i++ ) {
 		G_xpsave_add(&g_entities[level.sortedClients[i]]);
 	}
 	G_xpsave_writeconfig();
 
 	// tjw: when the map finishes normally this must be cleared 	
 	trap_Cvar_Set( "g_reset", "0");
/*
#ifdef __BOT__
	SOD_SPAWN_SaveBots(); // This actually kicks them...
#endif //__BOT__
*/
}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
#ifdef __BOT__
extern int G_CountBotPlayers( int team );
#endif //__BOT__

void CheckIntermissionExit( void ) {
	static int fActions = 0;
	qboolean exit = qfalse;
	int i;
	// rain - for #105
	gclient_t *cl;
	int ready = 0, notReady = 0;

	// OSP - end-of-level auto-actions
	//		  maybe make the weapon stats dump available to single player?
	if(!(fActions & EOM_WEAPONSTATS) && level.time - level.intermissiontime > 300) {
		G_matchInfoDump(EOM_WEAPONSTATS);
		fActions |= EOM_WEAPONSTATS;
	}
	if(!(fActions & EOM_MATCHINFO) && level.time - level.intermissiontime > 800) {
		G_matchInfoDump(EOM_MATCHINFO);
		fActions |= EOM_MATCHINFO;
		if( (g_weapons.integer & WPF_DROP_BINOCS) &&
				(g_misc.integer & MISC_BINOC_MASTER) ) {
			G_BinocMasters(qtrue);
		}
		//Perro: Display active and long k-spree info
		if (g_killingSpree.integer &&
				(g_misc.integer & MISC_KILLSTREAKS) ) {
			G_Streakers(qtrue);
			G_MapLongStreak(qtrue);
		}
	}

	for( i = 0; i < level.numConnectedClients; i++ ) {
		// rain - #105 - spectators and people who are still loading
		// don't have to be ready at the end of the round.
		// additionally, make readypercent apply here.

		cl = level.clients + level.sortedClients[i];

#ifdef __BOT__
		if (G_CountBotPlayers(TEAM_AXIS) + G_CountBotPlayers(TEAM_ALLIES) == level.numConnectedClients-1)
		{// A single player spectating... Let them choose when to continue...
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			} else if ( cl->pers.ready || ( g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT ) ) {
				ready++;
			} else {
				notReady++;
			}
		}
		else
		{
			if ( cl->pers.connected != CON_CONNECTED || cl->sess.sessionTeam == TEAM_SPECTATOR ) {
				continue;
			} else if ( cl->pers.ready || ( g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT ) ) {
				ready++;
			} else {
				notReady++;
			}
		}
#else //!__BOT__
		if ( cl->pers.connected != CON_CONNECTED || cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		} else if ( cl->pers.ready || ( g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT ) ) {
			ready++;
		} else {
			notReady++;
		}
#endif //__BOT__
	}
	if(!level.numConnectedClients) return;
	if((100.0f * (ready / (level.numConnectedClients * 1.0f))) >= 
	       (g_intermissionReadyPercent.value * 1.0f)) {

		exit = qtrue;
	}

	// Gordon: changing this to a minute for now
	// tjw: making this a cvar
	if(!exit && (level.time < (level.intermissiontime + 
		(1000 * g_intermissionTime.integer)))) {

		return;
	}

#ifdef __BOT__
	SOD_SPAWN_SaveBots(); // This actually kicks them...
#endif //__BOT__

	ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void ) {
	int		a/*, b*/;
	char	cs[MAX_STRING_CHARS];
	char	*buf;

	// DHM - Nerve :: GT_WOLF checks the current value of
		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );

		buf = Info_ValueForKey( cs, "winner" );
		a = atoi( buf );

		return a == -1;
}

qboolean G_ScriptAction_SetWinner( gentity_t *ent, char *params );

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules( void ) {
	char	cs[MAX_STRING_CHARS];

	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if( g_gamestate.integer == GS_INTERMISSION ) {
		CheckIntermissionExit ();
		return;
	}

	if ( level.intermissionQueued ) {
		level.intermissionQueued = 0;
		BeginIntermission();
		return;
	}

#ifdef __ETE__
	if ((g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) && GetNumberOfPOPFlags() > 0)
	{
		if (num_red_flags <= 0)
		{// Red team has lost!
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
			Info_SetValueForKey( cs, "winner", "1" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			LogExit( "Axis team eliminated." );
			trap_SendServerCommand( -1, "print \"^5=============================================\n\"");
			trap_SendServerCommand( -1, "print \"^4Allies ^7WIN^5: ^1Axis ^5have no more control points!\n\"");
			trap_SendServerCommand( -1, "print \"^5=============================================\n\"");

			if (g_dedicated.integer > 0)
			{
				G_Printf("^5=============================================\n");
				G_Printf("^4Allies ^7WIN^5: ^1Axis ^5have no more control points!\n");
				G_Printf("^5=============================================\n");
			}
		}
		else if (num_blue_flags <= 0)
		{// Blue team has lost!
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
			Info_SetValueForKey( cs, "winner", "0" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			LogExit( "Allied team eliminated." );
			trap_SendServerCommand( -1, "print \"^5=============================================\n\"");
			trap_SendServerCommand( -1, "print \"^1Axis ^7WIN^5: ^4Allies ^5have no more control points!\n\"");
			trap_SendServerCommand( -1, "print \"^5=============================================\n\"");

			if (g_dedicated.integer > 0)
			{
				G_Printf("^5=============================================\n");
				G_Printf("^1Axis ^7WIN^5: ^4Allies ^5have no more control points!\n");
				G_Printf("^5=============================================\n");
			}
		}
		else if (redtickets <= 0)
		{// Red team has lost!
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
			Info_SetValueForKey( cs, "winner", "1" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			LogExit( "Axis team eliminated." );
			trap_SendServerCommand( -1, "print \"^5========================================\n\"");
			trap_SendServerCommand( -1, "print \"^4Allies ^7WIN^5: ^1Axis ^5has ran out of tickets!\n\"");
			trap_SendServerCommand( -1, "print \"^5========================================\n\"");

			if (g_dedicated.integer > 0)
			{
				G_Printf("^5========================================\n");
				G_Printf("^4Allies ^7WIN^5: ^1Axis ^5has ran out of tickets!\n");
				G_Printf("^5========================================\n");
			}
		}
		else if (bluetickets <= 0)
		{// Blue team has lost!
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
			Info_SetValueForKey( cs, "winner", "0" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			LogExit( "Allied team eliminated." );
			trap_SendServerCommand( -1, "print \"^5=========================================\n\"");
			trap_SendServerCommand( -1, "print \"^1Axis ^7WIN^5: ^4Allies ^5have ran out of tickets!\n\"");
			trap_SendServerCommand( -1, "print \"^5=========================================\n\"");

			if (g_dedicated.integer > 0)
			{
				G_Printf("^5=========================================\n");
				G_Printf("^1Axis ^7WIN^5: ^4Allies ^5have ran out of tickets!\n");
				G_Printf("^5=========================================\n");
			}
		}
	}
#endif //__ETE__

	// tjw: is this really necessary?	
	//if ( level.numPlayingClients < 2 ) {
	//	return;
	//}

	// tjw: moved this check before timelimit for the case of multi-objective
	//      maps
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) {
			if ( level.numFinalDead[0] >= level.numTeamClients[0] && level.numTeamClients[0] > 0 ) {
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
				Info_SetValueForKey( cs, "winner", "1" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
				LogExit( "Axis team eliminated." );
			}
			else if ( level.numFinalDead[1] >= level.numTeamClients[1] && level.numTeamClients[1] > 0 ) {
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
				Info_SetValueForKey( cs, "winner", "0" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
				LogExit( "Allied team eliminated." );
			}
		}
	}

	if ( g_timelimit.value && !level.warmupTime ) {
		// OSP
		if((level.timeCurrent - level.startTime) >= (g_timelimit.value * 60000)) {
		// OSP

			// Check who has the most players alive
			if( g_gametype.integer == GT_WOLF_LMS ) {
				int axisSurvivors, alliedSurvivors;

				level.suddendeath = qfalse;

				axisSurvivors = level.numTeamClients[0] - level.numFinalDead[0];
				alliedSurvivors = level.numTeamClients[1] - level.numFinalDead[1];

				//bani - if team was eliminated < 3 sec before round end, we _properly_ end it here
				if( level.teamEliminateTime ) {
					LogExit( va( "%s team eliminated.", level.lmsWinningTeam == TEAM_ALLIES ? "Axis" : "Allied" ) );
				}

				if( axisSurvivors == alliedSurvivors ) {
					// First blood wins
					if( level.firstbloodTeam == TEAM_AXIS ) {
						trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
						Info_SetValueForKey( cs, "winner", "0" );
						trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
						LogExit( "Axis team wins by drawing First Blood." );
						trap_SendServerCommand( -1, "print \"Axis team wins by drawing First Blood.\n\"");
					} else if( level.firstbloodTeam == TEAM_ALLIES ) {
						trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
						Info_SetValueForKey( cs, "winner", "1" );
						trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
						LogExit( "Allied team wins by drawing First Blood." );
						trap_SendServerCommand( -1, "print \"Allied team wins by drawing First Blood.\n\"");
					} else {
						// no winner yet - sudden death!
						return;
					}
				} else if( axisSurvivors > alliedSurvivors ) {
					trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
					Info_SetValueForKey( cs, "winner", "0" );
					trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
					LogExit( "Axis team has the most survivors." );
					trap_SendServerCommand( -1, "print \"Axis team has the most survivors.\n\"");
					return;
				} else {
					trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
					Info_SetValueForKey( cs, "winner", "1" );
					trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
					LogExit( "Allied team has the most survivors." );
					trap_SendServerCommand( -1, "print \"Allied team has the most survivors.\n\"");
					return;
				}
#ifdef __ETE__
			} else if (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) {
				if ( redtickets > bluetickets ) {
					trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
					Info_SetValueForKey( cs, "winner", "0" );
					trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
					LogExit( "Axis team has the most tickets." );
					trap_SendServerCommand( -1, "print \"Axis team has the most tickets.\n\"");
					return;
				} else if (redtickets < bluetickets) {
					trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
					Info_SetValueForKey( cs, "winner", "1" );
					trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
					LogExit( "Allied team has the most tickets." );
					trap_SendServerCommand( -1, "print \"Allied team has the most tickets.\n\"");
					return;
				}
				else if ( num_red_flags > num_blue_flags ) {
					trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
					Info_SetValueForKey( cs, "winner", "0" );
					trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
					LogExit( "Axis team has the most control points." );
					trap_SendServerCommand( -1, "print \"Axis team has the most control points.\n\"");
					return;
				} else if (num_red_flags < num_blue_flags) {
					trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
					Info_SetValueForKey( cs, "winner", "1" );
					trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
					LogExit( "Allied team has the most control points." );
					trap_SendServerCommand( -1, "print \"Allied team has the most control points.\n\"");
					return;
				} else {
						// no winner yet - sudden death!
					return;
				}
#endif //__ETE__
			} else {
				// check for sudden death 
				if ( ScoreIsTied() ) {
					// score is tied, so don't end the game
					return;
				}
				else if(G_SuddenDeath()) {
					level.suddendeath = qtrue;
					return;
				}
			}

			if (level.gameManager ) {
				G_Script_ScriptEvent( level.gameManager, "trigger", "timelimit_hit" );
			}

			// NERVE - SMF - do not allow LogExit to be called in non-playing gamestate
			// - This already happens in LogExit, but we need it for the print command
			if ( g_gamestate.integer != GS_PLAYING )
				return;

			trap_SendServerCommand( -1, "print \"Timelimit hit.\n\"");
			LogExit( "Timelimit hit." );

			return;
		}
	}

	//bani - #444
	//i dont really get the point of the delay anyway, why not end it immediately like maxlives games?
	if( g_gametype.integer == GT_WOLF_LMS ) {
		if( !level.teamEliminateTime ) {
			if( level.numFinalDead[0] >= level.numTeamClients[0] && level.numTeamClients[0] > 0 ) {
				level.teamEliminateTime = level.time;
				level.lmsWinningTeam = TEAM_ALLIES;
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
				Info_SetValueForKey( cs, "winner", "1" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			} else if( level.numFinalDead[1] >= level.numTeamClients[1] && level.numTeamClients[1] > 0 ) {
				level.teamEliminateTime = level.time;
				level.lmsWinningTeam = TEAM_AXIS;
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
				Info_SetValueForKey( cs, "winner", "0" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			}
		} else if( level.teamEliminateTime + 3000 < level.time ) {
			LogExit( va( "%s team eliminated.", level.lmsWinningTeam == TEAM_ALLIES ? "Axis" : "Allied" ) );
		}
		return;
	}

	if ( level.numPlayingClients < 2 ) {
		return;
	}

	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) {
			if ( level.numFinalDead[0] >= level.numTeamClients[0] && level.numTeamClients[0] > 0 ) {
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
				Info_SetValueForKey( cs, "winner", "1" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
				LogExit( "Axis team eliminated." );
			}
			else if ( level.numFinalDead[1] >= level.numTeamClients[1] && level.numTeamClients[1] > 0 ) {
				trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof(cs) );
				Info_SetValueForKey( cs, "winner", "0" );
				trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
				LogExit( "Allied team eliminated." );
			}
		}
	}
}



/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/


/*
=============
CheckWolfMP

NERVE - SMF
=============
*/
/*
void CheckGameState() {
	gamestate_t current_gs;

	current_gs = trap_Cvar_VariableIntegerValue( "gamestate" );

	if ( level.intermissiontime && current_gs != GS_INTERMISSION ) {
		trap_Cvar_Set( "gamestate", va( "%i", GS_INTERMISSION ) );
		return;
	}

	if ( g_noTeamSwitching.integer && !trap_Cvar_VariableIntegerValue( "sv_serverRestarting" ) ) {
		if ( current_gs != GS_WAITING_FOR_PLAYERS && level.numPlayingClients <= 1 && level.lastRestartTime + 1000 < level.time ) {
			level.lastRestartTime = level.time;
			trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WAITING_FOR_PLAYERS ) );
		}
	}

	if ( current_gs == GS_WAITING_FOR_PLAYERS && g_minGameClients.integer > 1 && 
		level.numPlayingClients >= g_minGameClients.integer && level.lastRestartTime + 1000 < level.time ) {

		level.lastRestartTime = level.time;
		trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
	}

	if( g_gametype.integer == GT_WOLF_LMS && current_gs == GS_WAITING_FOR_PLAYERS && level.numPlayingClients > 1
		&& level.lastRestartTime + 1000 < level.time ) {
		level.lastRestartTime = level.time;
		trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
	}

	// if the warmup is changed at the console, restart it
	if ( current_gs == GS_WARMUP_COUNTDOWN && g_warmup.modificationCount != level.warmupModificationCount ) {
		level.warmupModificationCount = g_warmup.modificationCount;
		current_gs = GS_WARMUP;
	}

	// check warmup latch
	if ( current_gs == GS_WARMUP ) {
		int delay = g_warmup.integer;

		if( g_gametype.integer == GT_WOLF_CAMPAIGN || g_gametype.integer == GT_WOLF_LMS )
			delay *= 2;

		delay++;

		if ( delay < 6 ) {
			trap_Cvar_Set( "g_warmup", "5" );
			delay = 7;
		}

		level.warmupTime = level.time + ( delay * 1000 );
		trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
		trap_Cvar_Set( "gamestate", va( "%i", GS_WARMUP_COUNTDOWN ) );
	}
}
*/

/*
=============
CheckWolfMP

NERVE - SMF - Once a frame, check for changes in wolf MP player state
=============
*/
/*
void CheckWolfMP() {
  // TTimo unused
//	static qboolean latch = qfalse;

	// NERVE - SMF - check game state
	CheckGameState();

	if ( level.warmupTime == 0 ) {
		return;
	}


	// Only do the restart for MP
	if(g_gametype.integer != GT_SINGLE_PLAYER && g_gametype.integer != GT_COOP && g_gametype.integer != GT_NEWCOOP)

	// if the warmup time has counted down, restart
	if ( level.time > level.warmupTime ) {
		level.warmupTime += 10000;
		trap_Cvar_Set( "g_restarted", "1" );
		trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		level.restarted = qtrue;
		return;
	}
}
// -NERVE - SMF
*/
void CheckWolfMP() {
	// check because we run 6 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if(g_gametype.integer >= GT_WOLF) {
		if(g_gamestate.integer == GS_PLAYING || g_gamestate.integer == GS_INTERMISSION) {
			if(level.intermissiontime && g_gamestate.integer != GS_INTERMISSION) trap_Cvar_Set("gamestate", va( "%i", GS_INTERMISSION));
			return;
		}

		// check warmup latch
		if(g_gamestate.integer == GS_WARMUP) {
			if(!g_doWarmup.integer ||
			  (level.numPlayingClients >= match_minplayers.integer &&
			   level.lastRestartTime + 1000 < level.time && G_readyMatchState()))
			{
				int delay = (g_warmup.integer < 10) ? 11 : g_warmup.integer + 1;

				// Why scale these at all?  Minimum would mean 22s on Campaign and 44 on LMS....
				// Once people are ready, they want to get the show rolling :)
/*				if( g_gametype.integer == GT_WOLF_CAMPAIGN )
					delay *= 2;
				else if( g_gametype.integer == GT_WOLF_LMS && !g_doWarmup.integer )
					delay *= 4;
*/

				level.warmupTime = level.time + (delay * 1000);
				trap_Cvar_Set( "gamestate", va( "%i", GS_WARMUP_COUNTDOWN ) );
				trap_Cvar_Update(&g_gamestate);
				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
			}
		}

		// if the warmup time has counted down, restart
		if(g_gamestate.integer == GS_WARMUP_COUNTDOWN) {
			if(level.time > level.warmupTime) {
				level.warmupTime += 10000;
				trap_Cvar_Set( "g_restarted", "1" );
				trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
				level.restarted = qtrue;
				if(g_fightSound.string[0]) {
					G_globalSound(g_fightSound.string);
				}
				return;
			}
		}
	}
}

/*
==================
CheckVote
==================
*/
void CheckVote( void ) {
	int pcnt;
		int total;

	if(!level.voteInfo.voteTime ||
		level.voteInfo.vote_fn == NULL ||
		level.time - level.voteInfo.voteTime < 1000) {

		return;
	}

	pcnt = vote_percent.integer;

		if( pcnt > 99 ) {
			pcnt = 99;
		}
		if( pcnt < 1 ) {
			pcnt = 1;
		}

	if((g_voting.integer & VOTEF_USE_TOTAL_VOTERS) &&
		level.time - level.voteInfo.voteTime >= VOTE_TIME) {

		total = (level.voteInfo.voteYes 
			+ level.voteInfo.voteNo);
	}
	else if(level.voteInfo.vote_fn == G_Kick_v ||
		level.voteInfo.vote_fn == G_Surrender_v) {

		gentity_t *other = &g_entities[level.voteInfo.voteCaller];
		if(!other->client ||
			other->client->sess.sessionTeam == TEAM_SPECTATOR ) {

				total = level.voteInfo.numVotingClients;
			}
		else {
			total = level.voteInfo.numVotingTeamClients[other->client->sess.sessionTeam == TEAM_AXIS ? 0 : 1 ];
		}
	}
	else {
			total = level.voteInfo.numVotingClients;
		}

		if( level.voteInfo.voteYes > pcnt*total/100 ) {
			// execute the command, then remove the vote
			if(level.voteInfo.voteYes > total + 1) {
				// Don't announce some votes, as in comp mode, it is generally a ref
				// who is policing people who shouldn't be joining and players don't want
				// this sort of spam in the console
				if(level.voteInfo.vote_fn != G_Kick_v) {
					AP(va("cpm \"^5Referee changed setting! ^7(%s)\n\"", level.voteInfo.voteString));
				}
				G_LogPrintf("Referee Setting: %s\n", level.voteInfo.voteString);
			} else {
				AP("cpm \"^5Vote passed!\n\"");
				G_LogPrintf("Vote Passed: %s\n", level.voteInfo.voteString);
			}

			// Perform the passed vote
			level.voteInfo.vote_fn(NULL, 0, NULL, NULL, qfalse);

		// don't penalize player if the vote passes.
		if((g_voting.integer & VOTEF_NO_POPULIST_PENALTY)) {
			gentity_t *ent;

			ent = &g_entities[level.voteInfo.voteCaller];
			if(ent->client) 
				ent->client->pers.voteCount--;
		}

	}
	else if((level.voteInfo.voteNo &&
		level.voteInfo.voteNo >= (100-pcnt)*total/100) ||
		level.time - level.voteInfo.voteTime >= VOTE_TIME) {

			// same behavior as a no response vote
			AP(va("cpm \"^2Vote FAILED! ^3(%s)\n\"", level.voteInfo.voteString));
			G_LogPrintf("Vote Failed: %s\n", level.voteInfo.voteString);
	}
	else {
			// still waiting for a majority
			return;
		}

	level.voteInfo.voteTime = 0;
	trap_SetConfigstring(CS_VOTE_TIME, "");
}

#ifdef SAVEGAME_SUPPORT
/*
=============
CheckReloadStatus
=============
*/
void G_CheckReloadStatus(void) {
	// if we are waiting for a reload, check the delay time
	if(g_reloading.integer) {
		if (level.reloadDelayTime) {
			if (level.reloadDelayTime < level.time) {

				/*if (g_reloading.integer == RELOAD_NEXTMAP_WAITING) {
					trap_Cvar_Set( "g_reloading", va("%d", RELOAD_NEXTMAP) );	// set so sv_map_f will know it's okay to start a map
					if (g_cheats.integer)
						trap_SendConsoleCommand( EXEC_APPEND, va("spdevmap %s\n", level.nextMap) );
					else
						trap_SendConsoleCommand( EXEC_APPEND, va("spmap %s\n", level.nextMap ) );

				}*//* else if(g_reloading.integer == RELOAD_ENDGAME) {
					G_EndGame();	// kick out to the menu and start the "endgame" menu (credits, etc)

				}*/// else {
					// set the loadgame flag, and restart the server
					trap_Cvar_Set( "savegame_loading", "2" );	// 2 means it's a restart, so stop rendering until we are loaded
					trap_SendConsoleCommand( EXEC_INSERT, "map_restart\n" );
				//}

				level.reloadDelayTime = 0;
			}
		} else if (level.reloadPauseTime) {
			if (level.reloadPauseTime < level.time) {
				trap_Cvar_Set( "g_reloading", "0" );
				level.reloadPauseTime = 0;
			}
		}
	}
}

static void G_EnableRenderingThink(gentity_t *ent)
{
	trap_Cvar_Set( "cg_norender", "0" );
//		trap_S_FadeAllSound(1.0f, 1000);	// fade sound up
	G_FreeEntity( ent );
}

extern gentity_t *BotFindEntityForName( char *name );
extern void Bot_ScriptThink( void );

static void G_CheckLoadGame(void) {
	char loading[4];
	gentity_t *ent = NULL; // TTimo: VC6 'may be used without having been init'
	qboolean ready;

	// have we already done the save or load?
	if (!saveGamePending)
		return;

	// tell the cgame NOT to render the scene while we are waiting for things to settle
	//trap_Cvar_Set( "cg_norender", "1" );

	trap_Cvar_VariableStringBuffer( "savegame_loading", loading, sizeof(loading) );

	//trap_Cvar_Set( "g_reloading", "1" );	// moved down

	if (strlen( loading ) > 0 && atoi(loading) != 0)
	{
		trap_Cvar_Set( "g_reloading", "1" );

		// screen should be black if we are at this stage
		trap_SetConfigstring( CS_SCREENFADE, va("1 %i 1", level.time - 10 ) );

//		if (!reloading && atoi(loading) == 2) {
		if (!(g_reloading.integer) && atoi(loading) == 2) {
			// (SA) hmm, this seems redundant when it sets it above...
//			reloading = qtrue;	// this gets reset at the Map_Restart() since the server unloads the game dll
			trap_Cvar_Set( "g_reloading", "1" );
		}

		ready = qtrue;
		if ((ent = BotFindEntityForName("player")) == NULL)
			ready = qfalse;
		else if (!ent->client || ent->client->pers.connected != CON_CONNECTED)
			ready = qfalse;

		if (ready) {
			trap_Cvar_Set( "savegame_loading", "0" );	// in-case it aborts
			saveGamePending = 0;
			G_LoadGame();

			// RF, spawn a thinker that will enable rendering after the client has had time to process the entities and setup the display
			ent = G_Spawn();
			ent->nextthink = level.time + 200;
			ent->think = G_EnableRenderingThink;

			// wait for the clients to return from faded screen
			//trap_SetConfigstring( CS_SCREENFADE, va("0 %i 1500", level.time + 500) );
			trap_SetConfigstring( CS_SCREENFADE, va("0 %i 750", level.time + 500) );
			level.reloadPauseTime = level.time + 1100;

			// make sure sound fades up
			trap_SendServerCommand(-1, va("snd_fade 1 %d 0", 2000) );	//----(SA)	added

			Bot_ScriptThink();
		}
	} else {

		ready = qtrue;
		if ((ent = BotFindEntityForName("player")) == NULL)
			ready = qfalse;
		else if (!ent->client || ent->client->pers.connected != CON_CONNECTED)
			ready = qfalse;

		// not loading a game, we must be in a new level, so look for some persistant data to read in, then save the game
		if (ready) {
			G_LoadPersistant();		// make sure we save the game after we have brought across the items

			saveGamePending = 0;

// briefing menu will handle transition, just set a cvar for it to check for drawing the 'continue' button
			trap_SendServerCommand(-1, "rockandroll\n");

			level.reloadPauseTime = level.time + 1100;

			Bot_ScriptThink();
		}
	}
}
#endif // SAVEGAME_SUPPORT

/*
==================
CheckCvars
==================
*/
void CheckCvars( void ) {
	static int g_password_lastMod = -1;
	static int g_teamForceBalance_lastMod = -1;
	static int g_lms_teamForceBalance_lastMod = -1;
	static int g_censor_lastMod = -1;
	static int g_censorNames_lastMod = -1;
	static int g_skillSoldier_lastMod = -1;
	static int g_skillMedic_lastMod = -1;
	static int g_skillEngineer_lastMod = -1;
	static int g_skillFieldOps_lastMod = -1;
	static int g_skillCovertOps_lastMod = -1;
	static int g_skillBattleSense_lastMod = -1;
	static int g_skillLightWeapons_lastMod = -1;

	if ( g_censor.modificationCount != g_censor_lastMod ) {
		g_censor_lastMod = g_censor.modificationCount;
		InitCensorStructure();
	}

	if ( g_censorNames.modificationCount != g_censorNames_lastMod ) {
		g_censorNames_lastMod = g_censorNames.modificationCount;
		InitCensorNamesStructure();
	}

	if ( g_password.modificationCount != g_password_lastMod ) {
		g_password_lastMod = g_password.modificationCount;
		if ( *g_password.string && Q_stricmp( g_password.string, "none" ) ) {
			trap_Cvar_Set( "g_needpass", "1" );
		} else {
			trap_Cvar_Set( "g_needpass", "0" );
		}
	}

	if( g_gametype.integer == GT_WOLF_LMS ) {
		if( g_lms_teamForceBalance.modificationCount != g_lms_teamForceBalance_lastMod ) {
			g_lms_teamForceBalance_lastMod = g_lms_teamForceBalance.modificationCount;
			if ( g_lms_teamForceBalance.integer ) {
				trap_Cvar_Set( "g_balancedteams", "1" );
			} else {
				trap_Cvar_Set( "g_balancedteams", "0" );
			}
		}
	} else {
		if( g_teamForceBalance.modificationCount != g_teamForceBalance_lastMod ) {
			g_teamForceBalance_lastMod = g_teamForceBalance.modificationCount;
			if ( g_teamForceBalance.integer ) {
				trap_Cvar_Set( "g_balancedteams", "1" );
			} else {
				trap_Cvar_Set( "g_balancedteams", "0" );
			}
		}
	}

	if( g_skillSoldier.modificationCount != g_skillSoldier_lastMod ) {
		g_skillSoldier_lastMod = g_skillSoldier.modificationCount;
		BG_InitSkillLevelStructure( SK_HEAVY_WEAPONS, g_skillSoldier.string );
		G_ReassignSkillLevel( SK_HEAVY_WEAPONS );
	}

	if( g_skillMedic.modificationCount != g_skillMedic_lastMod ) {
		g_skillMedic_lastMod = g_skillMedic.modificationCount;
		BG_InitSkillLevelStructure( SK_FIRST_AID, g_skillMedic.string );
		G_ReassignSkillLevel( SK_FIRST_AID );
	}
	
	if( g_skillEngineer.modificationCount != g_skillEngineer_lastMod ) {
		g_skillEngineer_lastMod = g_skillEngineer.modificationCount;
		BG_InitSkillLevelStructure( SK_EXPLOSIVES_AND_CONSTRUCTION, g_skillEngineer.string );
		G_ReassignSkillLevel(SK_EXPLOSIVES_AND_CONSTRUCTION);
	}

	if( g_skillFieldOps.modificationCount != g_skillFieldOps_lastMod ) {
		g_skillFieldOps_lastMod = g_skillFieldOps.modificationCount;
		BG_InitSkillLevelStructure( SK_SIGNALS, g_skillFieldOps.string );
		G_ReassignSkillLevel(SK_SIGNALS);
	}

	if( g_skillCovertOps.modificationCount != g_skillCovertOps_lastMod ) {
		g_skillCovertOps_lastMod = g_skillCovertOps.modificationCount;
		BG_InitSkillLevelStructure( SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, g_skillCovertOps.string );
		G_ReassignSkillLevel(SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS);
	}

	if( g_skillBattleSense.modificationCount != g_skillBattleSense_lastMod ) {
		g_skillBattleSense_lastMod = g_skillBattleSense.modificationCount;
		BG_InitSkillLevelStructure( SK_BATTLE_SENSE, g_skillBattleSense.string );
		G_ReassignSkillLevel(SK_BATTLE_SENSE);
	}

	if( g_skillLightWeapons.modificationCount != g_skillLightWeapons_lastMod ) {
		g_skillLightWeapons_lastMod = g_skillLightWeapons.modificationCount;
		BG_InitSkillLevelStructure( SK_LIGHT_WEAPONS, g_skillLightWeapons.string );
		G_ReassignSkillLevel(SK_LIGHT_WEAPONS);
	}
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink (gentity_t *ent) {
	float	thinktime;

	// OSP - If paused, push nextthink
	if(level.match_pause != PAUSE_NONE && (ent - g_entities) >= g_maxclients.integer &&
	  ent->nextthink > level.time && strstr(ent->classname, "DPRINTF_") == NULL) {
		ent->nextthink += level.time - level.previousTime;
	}

#ifdef __BOT__
	if (ent->r.svFlags & SVF_BOT)
	{
		ent->think (ent);
		return;
	}
#endif //__BOT__
#ifdef __NPC__
	if (ent->s.eType == ET_NPC)
	{
		ent->think (ent);
		return;
	}
#endif //__NPC__

	// RF, run scripting
	if (ent->s.number >= MAX_CLIENTS) {
		G_Script_ScriptRun( ent );
	}

	thinktime = ent->nextthink;
	if (thinktime <= 0) {
		return;
	}
	if (thinktime > level.time) {
		return;
	}
	
	ent->nextthink = 0;
	if (!ent->think) {
		G_Error ( "NULL ent->think");
	}
	ent->think (ent);
}

void G_RunEntity( gentity_t* ent, int msec );

/*
======================
G_PositionEntityOnTag
======================
*/
qboolean G_PositionEntityOnTag( gentity_t *entity, gentity_t* parent, char *tagName ) {
	int				i;
	orientation_t	tag;
	vec3_t			axis[3];
	AnglesToAxis( parent->r.currentAngles, axis );

	VectorCopy( parent->r.currentOrigin, entity->r.currentOrigin );
	
	if(!trap_GetTag( -1, parent->tagNumber, tagName, &tag )) {
		return qfalse;
	}

	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( entity->r.currentOrigin, tag.origin[i], axis[i], entity->r.currentOrigin );
	}

	if( entity->client && entity->s.eFlags & EF_MOUNTEDTANK ) {
		// zinx - moved tank hack to here
		// bani - fix tank bb
		// zinx - figured out real values, only tag_player is applied,
		// so there are two left:
		// mg42upper attaches to tag_mg42nest[mg42base] at:
		// 0.03125, -1.171875, 27.984375
		// player attaches to tag_playerpo[mg42upper] at:
		// 3.265625, -1.359375, 2.96875
		// this is a hack, by the way.
		entity->r.currentOrigin[0] += 0.03125 + 3.265625;
		entity->r.currentOrigin[1] += -1.171875 + -1.359375;
		entity->r.currentOrigin[2] += 27.984375 + 2.96875;
	}

	G_SetOrigin( entity, entity->r.currentOrigin );

	if( entity->r.linked && !entity->client ) {
		if( !VectorCompare( entity->oldOrigin, entity->r.currentOrigin ) ) {
			trap_LinkEntity( entity );
		}
	}

	return qtrue;
}

void G_TagLinkEntity( gentity_t* ent, int msec ) {
	gentity_t* parent = &g_entities[ent->s.torsoAnim];
	vec3_t move, amove;
	gentity_t* obstacle;
	vec3_t origin, angles;
	vec3_t v;

	if( ent->linkTagTime >= level.time ) {
		return;
	}

	G_RunEntity( parent, msec );

	if (!(parent->s.eFlags & EF_PATH_LINK)) {
		if( parent->s.pos.trType == TR_LINEAR_PATH ) {
			int pos;
			float frac;

			if((ent->backspline = BG_GetSplineData( parent->s.effect2Time, &ent->back )) == NULL) {
				return;
			}

			ent->backdelta = parent->s.pos.trDuration ? (level.time - parent->s.pos.trTime) / ((float)parent->s.pos.trDuration) : 0;

			if(ent->backdelta < 0.f) {
				ent->backdelta = 0.f;
			} else if(ent->backdelta > 1.f) {
				ent->backdelta = 1.f;
			}

			if(ent->back) {
				ent->backdelta = 1 - ent->backdelta;
			}

			pos = floor(ent->backdelta * (MAX_SPLINE_SEGMENTS));
			if(pos >= MAX_SPLINE_SEGMENTS) {
				pos = MAX_SPLINE_SEGMENTS - 1;
				frac = ent->backspline->segments[pos].length;
			} else {
				frac = ((ent->backdelta * (MAX_SPLINE_SEGMENTS)) - pos) * ent->backspline->segments[pos].length;
			}
			

			VectorMA( ent->backspline->segments[pos].start, frac, ent->backspline->segments[pos].v_norm, v );
			if(parent->s.apos.trBase[0]) {
				BG_LinearPathOrigin2( parent->s.apos.trBase[0], &ent->backspline, &ent->backdelta, v, ent->back );
			}

			VectorCopy( v, origin );
			
			if(ent->s.angles2[0]) {
				BG_LinearPathOrigin2( ent->s.angles2[0], &ent->backspline, &ent->backdelta, v, ent->back );
			}

			VectorCopy( v, ent->backorigin );

			if(	ent->s.angles2[0] < 0 ) {
				VectorSubtract( v, origin, v );
				vectoangles( v, angles );
			} else if( ent->s.angles2[0] > 0 ){
				VectorSubtract( origin, v, v );
				vectoangles( v, angles );
			} else {
				VectorCopy( vec3_origin, origin );
			}

			ent->moving = qtrue;
		} else {
			ent->moving = qfalse;
		}
	} else {
		if(parent->moving) {
			VectorCopy( parent->backorigin, v );

			ent->back =			parent->back;
			ent->backdelta =	parent->backdelta;
			ent->backspline =	parent->backspline;

			VectorCopy( v, origin );
			
			if(ent->s.angles2[0]) {
				BG_LinearPathOrigin2( ent->s.angles2[0], &ent->backspline, &ent->backdelta, v, ent->back );
			}

			VectorCopy( v, ent->backorigin );

			if(	ent->s.angles2[0] < 0 ) {
				VectorSubtract( v, origin, v );
				vectoangles( v, angles );				
			} else if( ent->s.angles2[0] > 0 ){
				VectorSubtract( origin, v, v );
				vectoangles( v, angles );
			} else {
				VectorCopy( vec3_origin, origin );
			}

			ent->moving = qtrue;
		} else {
			ent->moving = qfalse;
		}
	}

	if( ent->moving ) {
		VectorSubtract( origin, ent->r.currentOrigin, move );
		VectorSubtract( angles, ent->r.currentAngles, amove );

		if(!G_MoverPush( ent, move, amove, &obstacle )) {
			script_mover_blocked( ent, obstacle );
		}

		VectorCopy( origin, ent->s.pos.trBase );
		VectorCopy( angles, ent->s.apos.trBase );
	} else {
		memset( &ent->s.pos, 0, sizeof( ent->s.pos ) );
		memset( &ent->s.apos, 0, sizeof( ent->s.apos ) );

		VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
		VectorCopy( ent->r.currentAngles, ent->s.apos.trBase );
	}

	ent->linkTagTime = level.time;
}

#ifdef __VEHICLES__
extern vec3_t	tankMins;
extern vec3_t	tankMaxs;
extern vec3_t	playerMins;
extern vec3_t	playerMaxs;

/*float Vehicle_Angle_Diff(vec3_t viewangles)
{
	int i;
	float diff = 0.0f, angle;
	vec3_t angles;

	VectorSet(angles, 0, 0, 0);

	for (i = 0; i < 2; i++)
	{
		angle = AngleMod(viewangles[i]);
		angles[i] = AngleMod(angles[i]);
		diff = angles[i] - angle;
		if (angles[i] > angle)
		{
			if (diff > 180.0) diff -= 360.0;
		}
		else
		{
			if (diff < -180.0) diff += 360.0;
		}
	}
	return diff;
}*/

#endif //__VEHICLES__

//qboolean eltz_hack1_done = qfalse;
//qboolean eltz_hack2_done = qfalse;

void G_RunEntity( gentity_t* ent, int msec ) {
	if( ent->runthisframe ) {
		return;
	}

#ifdef __NPC__
	if ( Q_stricmp(ent->classname,  "freed") == 0 )
	{	
		// Unique1: Shouldn't have got to here??? WTF!!!
		// Kamikazee: There seems to be no check for freed/existant ents before calling this func
		//	Quite normal that all freed entities pass by. Since they're freed, DON'T FREE them AGAIN.
		return;
	}
	
	if (ent->s.eType == ET_NPC)
	{
		if (!ent->NPC_client && ent->NPC_Class == CLASS_HUMANOID ) { // do not do this for nonhumanoid npcs such planes ..
			G_Printf("NPC %i - classname: %s - NO CLIENT STRUCT\n", ent->s.number, ent->classname); 
			return;
		}

		ent->think (ent);
		ent->runthisframe = qtrue;
		return;
	}
#endif //__NPC__

#ifdef __VEHICLES_DISABLED__
	if (ent->client && ent->client->ps.eFlags & EF_VEHICLE)
	{// bot/player in a tank... Keep mins and maxs up to date!
		vec3_t forward, right, up;
		vec3_t mins, maxs;
		vec3_t temp_pos;

		AngleVectors( ent->r.currentAngles, forward, right, up );

		// Top Front Right
		VectorSet(temp_pos, 0, 0, 0);
		VectorMA( temp_pos, tankMaxs[0], forward, temp_pos );
		VectorMA( temp_pos, tankMaxs[1], right, temp_pos );
		VectorMA( temp_pos, tankMaxs[2], up, maxs );

		// Bottom Back Left
		VectorSet(temp_pos, 0, 0, 0);
		VectorMA( temp_pos, tankMins[0], forward, temp_pos );
		VectorMA( temp_pos, tankMins[1], right, temp_pos );
		VectorMA( temp_pos, tankMins[2], up, mins );

		VectorCopy(mins, ent->r.mins);
		VectorCopy(maxs, ent->r.maxs);
		VectorCopy (ent->r.mins, ent->client->ps.mins);
		VectorCopy (ent->r.maxs, ent->client->ps.maxs);
		
/*		VectorCopy(tankMins, ent->r.mins);
		VectorCopy(tankMaxs, ent->r.maxs);
		VectorCopy (ent->r.mins, ent->client->ps.mins);
		VectorCopy (ent->r.maxs, ent->client->ps.maxs);*/
	}
	else if (ent->s.eType == ET_VEHICLE)
	{// A vehicle entity.. Keep mins and maxs up to date!
		VectorCopy(tankMins, ent->r.mins);
		VectorCopy(tankMaxs, ent->r.maxs);
	}
	else if (ent->client && !(ent->client->ps.eFlags & EF_VEHICLE))
	{
		vec3_t forward, right, up;
		vec3_t mins, maxs;
		vec3_t temp_pos;

		AngleVectors( ent->r.currentAngles, forward, right, up );

		// Top Front Right
		VectorSet(temp_pos, 0, 0, 0);
		VectorMA( temp_pos, playerMaxs[0], forward, temp_pos );
		VectorMA( temp_pos, playerMaxs[1], right, temp_pos );
		VectorMA( temp_pos, playerMaxs[2], up, maxs );

		// Bottom Back Left
		VectorSet(temp_pos, 0, 0, 0);
		VectorMA( temp_pos, playerMins[0], forward, temp_pos );
		VectorMA( temp_pos, playerMins[1], right, temp_pos );
		VectorMA( temp_pos, playerMins[2], up, mins );

		VectorCopy(mins, ent->r.mins);
		VectorCopy(maxs, ent->r.maxs);
		VectorCopy (ent->r.mins, ent->client->ps.mins);
		VectorCopy (ent->r.maxs, ent->client->ps.maxs);
		
		/*VectorCopy(playerMins, ent->r.mins);
		VectorCopy(playerMaxs, ent->r.maxs);
		VectorCopy (ent->r.mins, ent->client->ps.mins);
		VectorCopy (ent->r.maxs, ent->client->ps.maxs);*/
	}
#endif //__VEHICLES_DISABLED__

#ifdef __BOT__
	if (ent->r.svFlags & SVF_BOT)
	{
		ent->think (ent);
		ent->runthisframe = qtrue;
		return;
	}
#endif

	ent->runthisframe = qtrue;

	if( !ent->inuse ) {
		return;
	}

	if( ent->tagParent ) {

		G_RunEntity( ent->tagParent, msec );

		if( ent->tagParent ) {
			if( G_PositionEntityOnTag( ent, ent->tagParent, ent->tagName ) ) {
				if( !ent->client ) {
					if( !ent->s.density) {
						BG_EvaluateTrajectory( &ent->s.apos, level.time, ent->r.currentAngles, qtrue, ent->s.effect2Time  );
						VectorAdd( ent->tagParent->r.currentAngles, ent->r.currentAngles, ent->r.currentAngles );
					} else {
						BG_EvaluateTrajectory( &ent->s.apos, level.time, ent->r.currentAngles, qtrue, ent->s.effect2Time  );
					}
				}
			}
		}
	} else if (ent->s.eFlags & EF_PATH_LINK ) {

		G_TagLinkEntity( ent, msec );
	}

	// ydnar: hack for instantaneous velocity
	VectorCopy( ent->r.currentOrigin, ent->oldOrigin );

	// check EF_NODRAW status for non-clients
	if( ent-g_entities > level.maxclients ) {
		if (ent->flags & FL_NODRAW) {
			ent->s.eFlags |= EF_NODRAW;
		} else {
			ent->s.eFlags &= ~EF_NODRAW;
		}
	}


	// clear events that are too old
	if ( level.time - ent->eventTime > EVENT_VALID_MSEC ) {
		if ( ent->s.event ) {
			ent->s.event = 0;
		}
		if ( ent->freeAfterEvent ) {
			// tempEntities or dropped items completely go away after their event
			G_FreeEntity( ent );
			return;
		} else if ( ent->unlinkAfterEvent ) {
			// items that will respawn will hide themselves after their pickup event
			ent->unlinkAfterEvent = qfalse;
			trap_UnlinkEntity( ent );
		}
	}

	// temporary entities don't think
	if( ent->freeAfterEvent ) {
		return;
	}

	// Arnout: invisible entities don't think
	// NOTE: hack - constructible one does
	if( ent->s.eType != ET_CONSTRUCTIBLE && ( ent->entstate == STATE_INVISIBLE || ent->entstate == STATE_UNDERCONSTRUCTION ) ) {
		// Gordon: we want them still to run scripts tho :p
		if (ent->s.number >= MAX_CLIENTS) {
			G_Script_ScriptRun( ent );
		}
		return;
	}

	if ( !ent->r.linked && ent->neverFree ) {
		return;
	}

	if ( ent->s.eType == ET_MISSILE 
		|| ent->s.eType == ET_FLAMEBARREL 
		|| ent->s.eType == ET_FP_PARTS
		|| ent->s.eType == ET_FIRE_COLUMN
		|| ent->s.eType == ET_FIRE_COLUMN_SMOKE
		|| ent->s.eType == ET_EXPLO_PART
		|| ent->s.eType == ET_RAMJET) {

		// OSP - pausing
		if( level.match_pause == PAUSE_NONE ) {
			G_RunMissile( ent );
		} else {
			// During a pause, gotta keep track of stuff in the air
			ent->s.pos.trTime += level.time - level.previousTime;
			// Keep pulsing right for dynmamite
			if( ent->methodOfDeath == MOD_DYNAMITE ) {
				ent->s.effect1Time += level.time - level.previousTime;
			}
			G_RunThink(ent);
		}
		// OSP

		return;
	}

	// DHM - Nerve :: Server-side collision for flamethrower
	if( ent->s.eType == ET_FLAMETHROWER_CHUNK ) {
		G_RunFlamechunk( ent );
		
		// ydnar: hack for instantaneous velocity
		VectorSubtract( ent->r.currentOrigin, ent->oldOrigin, ent->instantVelocity );
		VectorScale( ent->instantVelocity, 1000.0f / msec, ent->instantVelocity );
		
		return;
	}

	if( ent->s.eType == ET_ITEM || ent->physicsObject ) {
		G_RunItem( ent );
		
		// ydnar: hack for instantaneous velocity
		VectorSubtract( ent->r.currentOrigin, ent->oldOrigin, ent->instantVelocity );
		VectorScale( ent->instantVelocity, 1000.0f / msec, ent->instantVelocity );
		
		return;
	}

	if ( ent->s.eType == ET_MOVER || ent->s.eType == ET_PROP)
	{
		G_RunMover( ent );
		
		// ydnar: hack for instantaneous velocity
		VectorSubtract( ent->r.currentOrigin, ent->oldOrigin, ent->instantVelocity );
		VectorScale( ent->instantVelocity, 1000.0f / msec, ent->instantVelocity );
		
		//
		// UQ1: BEGIN - Hack for eltz_beta1 bad mover...
		//
		if (/*(!eltz_hack1_done || !eltz_hack2_done) &&*/ (ent->s.number == 276 || ent->s.number == 279))
		{
			if (!Q_stricmp(level.rawmapname, "eltz_beta1"))
			{
				if (ent->s.number == 276)
				{
					//Com_Printf("^1*** ^3%s^5: ^7ELTZ_BETA1^5 - Bad movers hack #1 activated! Bad mover disabled!\n");
					g_entities[276].r.contents = 0;
					trap_UnlinkEntity(&g_entities[276]);
					//eltz_hack1_done = qtrue;
				}
				else if (ent->s.number == 279)
				{
					//Com_Printf("^1*** ^3%s^5: ^7ELTZ_BETA1^5 - Bad movers hack #2 activated! Bad mover disabled!\n");
					g_entities[279].r.contents = 0;
					trap_UnlinkEntity(&g_entities[279]);
					//eltz_hack2_done = qtrue;
				}
			}
		}
		//
		// UQ1: END - Hack for eltz_beta1 bad mover...
		//

		return;
	}
	
	if( ent-g_entities < MAX_CLIENTS ) {
		G_RunClient( ent );
		
		// ydnar: hack for instantaneous velocity
		VectorSubtract( ent->r.currentOrigin, ent->oldOrigin, ent->instantVelocity );
		VectorScale( ent->instantVelocity, 1000.0f / msec, ent->instantVelocity );
		
		return;
	}

	// OSP - multiview
	if( ent->s.eType == ET_PORTAL && G_smvRunCamera(ent) ) {
		return;
	}

	if((ent->s.eType == ET_HEALER || ent->s.eType == ET_SUPPLIER) && ent->target_ent) {
		ent->target_ent->s.onFireStart =	ent->health;
		ent->target_ent->s.onFireEnd =		ent->count;
	}

	G_RunThink( ent );
		
	// ydnar: hack for instantaneous velocity
	VectorSubtract( ent->r.currentOrigin, ent->oldOrigin, ent->instantVelocity );
	VectorScale( ent->instantVelocity, 1000.0f / msec, ent->instantVelocity );
}

#ifdef __BOT__
int next_node_update = 0;
#endif //__BOT__

void G_UpdateCSInfo()
{
	char cs[BIG_INFO_STRING] = {""};

	trap_GetServerinfo(cs, sizeof(cs));
	level.csLenTotal += (int)(strlen(cs) - level.csLen[0] + 1);
	level.csLen[0] = (strlen(cs) + 1);
	
	cs[0] = '\0';
	trap_GetConfigstring(CS_SYSTEMINFO, cs, sizeof(cs));
	level.csLenTotal += (int)(strlen(cs) - level.csLen[1] + 1);
	level.csLen[1] = (strlen(cs) + 1);
}

#ifdef __BOT__
extern void SP_Check_AddBots ( void );
extern void AIMOD_MOVEMENT_Explosive_Checks ( void );
#endif //__BOT__

#ifdef __NPC__
extern void NPC_Plane_Cinematics ( void );

extern int cinematic_plane_enterpos_total;
extern int cinematic_plane_exitpos_total;
#endif //__NPC__

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime ) {
	int			i, msec;
//	int			pass = 0;

#ifdef __ETE__
	// UQ1: Fog real-time variation...
	UQ_DoFogVariation();
#endif //__ETE__

	// if we are waiting for the level to restart, do nothing
	if ( level.restarted ) {
		return;
	}

#ifdef __BOT__
	if (number_of_nodes <= 0 && !constructible_check_done && level.time > constructible_check_time)
	{ // If no waypoints, build all the constructibles before auto-waypointing the map!
		if (g_dedicated.integer <= 0)
		{// Only do this if there is a local client... (not dedicated)
			G_Printf("^5Building all constructibles for auto-waypointer!\n");

			for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++)
			{
				gentity_t *constructible = &g_entities[i];

				if(constructible && constructible->inuse && constructible->s.eType == ET_CONSTRUCTIBLE)
				{
					G_Printf("^5Constructible ^3%i ^5(^7%s^5) forced to starting built...\n", constructible->s.number, constructible->classname);
					AutoBuildConstruction( constructible );
				}
			}
		}

		constructible_check_done = qtrue;
	}
#endif //__BOT__

#ifndef _WIN32
	// UQ1: Now threaded with the bot pathfinding thread for win32... :)
#ifdef __BOT__
	// Keep a list of explosive positions to avoid them...
	AIMOD_MOVEMENT_Explosive_Checks();
#endif //__BOT__
#endif //_WIN32

#ifdef __ETE__
	if (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
		AdjustTickets();
#endif //__ETE__

#ifdef __NPC__
	if (cinematic_plane_enterpos_total >= 0 && cinematic_plane_exitpos_total >= 0)
		NPC_Plane_Cinematics(); // testing on battery map for now...
#endif //__NPC__

#ifdef __BOT__
	if (bot_minplayers.integer > 62)
		trap_Cvar_Set( "bot_minplayers", "62" );

	if (bot_coop_minplayers.integer > 62)
		trap_Cvar_Set( "bot_coop_minplayers", "62" );

	// UQ1: Run this once each frame... Vischeck links over time..
	AIMOD_MAPPING_Realtime_NodeLinks_Checking();

	// Update nodes info each frame..
/*	if (next_node_update <= level.time)
	{
		UpdateNodes();
		next_node_update = level.time + 10000;
	}*/

	// Draw waypoints on screen if in node edit mode..
	if (bot_node_edit.integer == 2)
		BotWaypointRenderRoute();
	else if (bot_node_edit.integer)
		BotWaypointRender();
	else if (bot_coverspot_render.integer)
		BotCoverSpotRender();

	if (level.warmupTime || level.warmupTime > level.time || level.intermissiontime)
	{// Don't try to re-add bots in warmup.

	}
	else if (!next_bot_check || !level.time || level.warmupTime
		|| next_bot_check == 0)
	{
		if (!level.time)
		{
			level.time = 0;
		}
		else if (!next_bot_check || next_bot_check == 0)
		{
			next_bot_check = level.time + 5000;
		}
	}
#ifndef __NPC_NEW_COOP__
	else if ((g_gametype.integer <= GT_COOP || g_gametype.integer == GT_NEWCOOP) && next_bot_check <= level.time)
	{// Bot_Minplayers has no effect on coop!
		/*
		SP_Check_AddBots();
		next_bot_check = level.time + 5000;// Check every 5 of a seconds.
		*/

		int num = 0;

		while (Count_Bot_Players()+Count_Human_Players() < bot_coop_minplayers.integer 
			&& Count_Bot_Players()+Count_Human_Players() < g_maxclients.integer - 1
			&& num < 1)
		{// Need to spawn a bot. 2 max at a time.
			int				iskill;
			char			team[MAX_TOKEN_CHARS] = "none";
			char			name[MAX_TOKEN_CHARS] = "uniquebot";
//			team_t			teamnum = TEAM_ALLIES;

			strcpy(team, "blue");
			Q_strncpyz(name, va("%s", PickName(TEAM_ALLIES)), sizeof(name));

			if (bot_skill.integer >= 1)
			{
				iskill = bot_skill.integer;
			}
			else
			{
				iskill = Q_TrueRand(1,5);
			}

			if (SOD_SPAWN_AddBotShort(team, name, iskill, NULL))
			{
				G_Printf("^1*** ^3%s^5: ^5Spawned bot ^7%s^5 on ^4ALLIED^5 team with skill level ^7%d^5.\n", GAME_VERSION, name, iskill);
			}

			num++;
		}

		while (Count_Bot_Players()+Count_Human_Players() > bot_coop_minplayers.integer
			|| Count_Bot_Players()+Count_Human_Players() >= g_maxclients.integer - 1)
		{
			int clientNum = Pick_Bot_Kick();

			if (clientNum == -1)
				break;

			SOD_SPAWN_RemoveBot(clientNum);

			g_entities[clientNum].is_bot = qfalse;
			g_entities[clientNum].think = NULL;
		}

		next_bot_check = level.time + 2500;//500;//2500;//5000;// Check every 5 of a seconds.
	}
#endif //__NPC_NEW_COOP__
	else if (next_bot_check <= level.time)
	{// Ready to fill up with bots to the number set in bot_minplayers cvar.
		int num = 0;

		while (Count_Bot_Players()+Count_Human_Players() < bot_minplayers.integer 
			&& Count_Bot_Players()+Count_Human_Players() < g_maxclients.integer - 1
			&& num < 1)
		{// Need to spawn a bot. 2 max at a time.
			int				iskill;
			char			team[MAX_TOKEN_CHARS] = "none";
			char			name[MAX_TOKEN_CHARS] = "uniquebot";
			team_t			teamnum = PickTeam(-1);

			//G_Printf("Team %i\n", teamnum);

			if (teamnum == TEAM_AXIS)
			{
				strcpy(team, "red");
				Q_strncpyz(name, va("%s", PickName(TEAM_AXIS)), sizeof(name));
			}
			else
			{
				strcpy(team, "blue");
				Q_strncpyz(name, va("%s", PickName(TEAM_ALLIES)), sizeof(name));
			}

			if (bot_skill.integer >= 1)
			{
				iskill = bot_skill.integer;
			}
			else
			{
				iskill = Q_TrueRand(1,5);//rand()%5;
			}

			if (SOD_SPAWN_AddBotShort(team, name, iskill, NULL))
			{
				if (!Q_stricmp( team, "red" ))
					G_Printf("^1*** ^3%s^5: ^5Spawned bot ^7%s^5 on ^1AXIS^5 team with skill level ^7%d^5.\n", GAME_VERSION, name, iskill);
				else
					G_Printf("^1*** ^3%s^5: ^5Spawned bot ^7%s^5 on ^4ALLIED^5 team with skill level ^7%d^5.\n", GAME_VERSION, name, iskill);
			}

			num++;
		}

		while (Count_Bot_Players()+Count_Human_Players() > bot_minplayers.integer
			|| Count_Bot_Players()+Count_Human_Players() >= g_maxclients.integer - 1)
		{
			int clientNum = Pick_Bot_Kick();

			if (clientNum == -1)
				break;

			SOD_SPAWN_RemoveBot(clientNum);

			g_entities[clientNum].is_bot = qfalse;
			g_entities[clientNum].think = NULL;
		}

		next_bot_check = level.time + 2500;//500;//2500;//5000;// Check every 5 of a seconds.
	}
#endif //__BOT__

#ifdef __NPC__
#ifndef __BOT__ // if this hasnt been done above already...
	// Draw waypoints on screen if in node edit mode..
	if (bot_node_edit.integer)
		BotWaypointRender();
	else if (npc_node_edit.integer)
		NPC_FODDER_WaypointRender();
#endif //__BOT__

	if (g_gametype.integer == GT_COOP || g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_NEWCOOP)
	{
		// Map should spawn its own NPC list here...
	}
	// Don't try to re-add npcs in warmup.
	else if( level.warmupTime == 0 && ! level.intermissiontime )
	{
		if ( next_NPC_check == 0)
		{
			next_NPC_check = level.time + 5000;
		}
		else if (level.time != 0 && next_NPC_check <= level.time )
		{
			// Ready to fill up with bots to the number set in npc_minplayers cvar.
			int num = 0;

			if (G_EntitiesFree())
			{
				// Only spawn another if there are free entities left!
				while (Count_NPC_Players() < npc_minplayers.integer && num < 1)
				{
					// Need to spawn an NPC. 2 max at a time.
					NPC_Spawn();
					num++;
				}
			}

			next_NPC_check = level.time + 2000/*200*/;// Check every 200 mili-seconds. NPCs can spawn real fast... (no clientinfo)...
		}
	}

	if (g_gametype.integer < GT_WOLF || g_gametype.integer == GT_NEWCOOP)
		COOP_NPC_Check(); // Add random npcs if the map is unsupported...
#endif //__NPC__

	// tjw: track CS_SERVERINFO and CS_SYSTEMINFO changes which 
	//      can be made outside of the game code
	G_UpdateCSInfo();


	// tjw: workaround for q3 bug 
	//      levelTime will start over when the timelimit 
	//      expires on dual objective maps.
	if(level.previousTime > level.time)
		level.overTime = level.previousTime;
	levelTime = levelTime + level.overTime;

	// Handling of pause offsets
	if( level.match_pause == PAUSE_NONE ) {
		level.timeCurrent = levelTime - level.timeDelta;
	} else {
		level.timeDelta = levelTime - level.timeCurrent;
		if((level.time % 500) == 0) {
			// FIXME: set a PAUSE cs and let the client adjust their local starttimes
			//        instead of this spam
			trap_SetConfigstring(CS_LEVEL_START_TIME, va("%i", level.startTime + level.timeDelta));
		}
	}

	level.frameTime = trap_Milliseconds();

	level.framenum++;
	level.previousTime = level.time;
	level.time = levelTime;

	msec = level.time - level.previousTime;

	level.axisBombCounter -= msec;
	level.alliedBombCounter -= msec;
	// forty - arty/airstrike rate limiting.
	level.axisArtyCounter -= msec;
	level.alliedArtyCounter -= msec;

	if( level.axisBombCounter < 0 ) {
		level.axisBombCounter = 0;
	}
	if( level.alliedBombCounter < 0 ) {
		level.alliedBombCounter = 0;
	}
	
	// forty - arty/airstrike rate limiting.
	if( level.axisArtyCounter < 0 ) {
		level.axisArtyCounter = 0;
	}
	if( level.alliedArtyCounter < 0 ) {
		level.alliedArtyCounter = 0;
	}
	
#if 0
	if(trap_Cvar_VariableIntegerValue( "dbg_spam" )) {
		trap_SetConfigstring( CS_VOTE_STRING, va(
"vvvvvvvvvvvvvvvvvvvwiubgfiwebxqbwigb3qir4gviqrbegiuertbgiuoeyvqrugyveru\
qogyjvuqeyrvguqoehvrguorevqguoveruygqueorvguoqeyrvguyervguverougvequryvg\
uoerqvgouqevrguoerqvguoerqvguoyqevrguoyvreuogvqeuogiyvureoyvnguoeqvguoqe\
rvguoeqrvuoeqvrguoyvqeruogverquogvqoeurvgouqervguerqvgqiertbgiqerubgipqe\
rbgipqebgierqviqrviertqyvbgyerqvgieqrbgipqebrgpiqbergibepbreqgupqruiperq\
ubgipqeurbgpiqjefgpkeiueripgberipgubreugqeirpqgbipeqygbibgpibqpebiqgefpi\
mgbqepigjbriqpirbgipvbiqpgvbpqiegvbiepqbgqiebgipqgjebiperqbgpiqebpireqbg\
ipqbgipjqfebzipjgbqipqervbgiyreqvbgipqertvgbiprqbgipgbipertqjgbipubriuqi\
pjgpifjbqzpiebgipuerqbgpibuergpijfebgqiepgbiupreqbgpqegjfebzpigu4ebrigpq\
uebrgpiebrpgibqeripgubeqrpigubqifejbgipegbrtibgurepqgbn%i", level.time )
);
	}
#endif
 
#ifdef SAVEGAME_SUPPORT
	G_CheckLoadGame();
#endif // SAVEGAME_SUPPORT

	// get any cvar changes
	G_UpdateCvars();

//#pragma omp parallel for
	for( i = 0; i < level.num_entities; i++ ) {
		g_entities[i].runthisframe = qfalse;
		G_RunEntity( &g_entities[ i ], msec ); // UQ1: Moved from below, save doing 2 loops lol!
	}

	// go through all allocated objects
//#pragma omp parallel for
/*	for( i = 0; i < level.num_entities; i++ ) {
		G_RunEntity( &g_entities[ i ], msec );
	}*/

	G_ActiveTeamBalance();

	if(g_msgs.integer && (level.time % 1000 == 0) &&
			(((level.time/1000) % g_msgs.integer) == 0) ) {
		G_PrintBanners();
	}	

	if((g_weapons.integer & WPF_DROP_BINOCS) &&
			(g_misc.integer & MISC_BINOC_MASTER) &&
			((level.time % 1000) == 0) && 
			(((level.time / 1000) % 60) == 0) ) {
		G_BinocMasters(qfalse);
	}

	// Perro: Display current streaks -- but only do so if cvar is non-zero
	if ((g_killingSpree.integer > 0) && 
			(g_misc.integer & MISC_KILLSTREAKS) && 
			((level.time % 1000) == 0) && 
			(((level.time / 1000) % 60) == 0))  {
		G_Streakers(qfalse);
	}
	// Perro: Show the long streak periodically
	if ((g_killingSpree.integer > 0) && 
			(g_misc.integer & MISC_KILLSTREAKS) &&
			(level.maxspree_count) && 
			((level.time % 1000) == 0) && 
			(((level.time / 1000) % 120) == 0))  {
		G_MapLongStreak(qfalse);
	}

//#pragma omp parallel for
	for( i = 0; i < level.numConnectedClients; i++ ) {
		ClientEndFrame(&g_entities[level.sortedClients[i]]);
	}

	// NERVE - SMF
	CheckWolfMP();

	// see if it is time to end the level
	CheckExitRules();

	// update to team status?
	CheckTeamStatus();

	// cancel vote if timed out
	CheckVote();

	// for tracking changes
	CheckCvars();

	G_UpdateTeamMapData();

	if(level.gameManager) {
		level.gameManager->s.otherEntityNum = g_maxTeamLandmines.integer - G_CountTeamLandmines(TEAM_AXIS);
		level.gameManager->s.otherEntityNum2 = g_maxTeamLandmines.integer - G_CountTeamLandmines(TEAM_ALLIES);
	}

#ifdef SAVEGAME_SUPPORT
	// Check if we are reloading, and times have expired
	G_CheckReloadStatus();
#endif // SAVEGAME_SUPPORT
}

// Is this a single player type game - sp or coop?
qboolean G_IsSinglePlayerGame()
{
	if (g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_COOP || g_gametype.integer == GT_NEWCOOP)
		return qtrue;

	return qfalse;
}

// Josh
void InitCensorStructure( void )
{
	char wordList[MAX_CVAR_VALUE_STRING];
	char *nextWord;
	Q_strncpyz( wordList, g_censor.string, sizeof(g_censor.string) );
	censorDictionary.num_words = 0;
	nextWord = strtok(wordList," ,");
	while (nextWord && censorDictionary.num_words != DICT_MAX_WORDS) {
		Q_strncpyz( censorDictionary.words[censorDictionary.num_words], nextWord,
									sizeof(censorDictionary.words[censorDictionary.num_words]));
		nextWord = strtok(NULL," ,");
		if (!nextWord) {
			break;
		}
		censorDictionary.num_words++;
	}
	censorDictionary.num_words++;
}

// Josh
void InitCensorNamesStructure( void )
{
	char wordList[MAX_CVAR_VALUE_STRING];
	char *nextWord;
	Q_strncpyz( wordList, g_censorNames.string, sizeof(g_censorNames.string) );
	censorNamesDictionary.num_words = 0;
	nextWord = strtok(wordList," ,");
	while (nextWord && censorNamesDictionary.num_words != DICT_MAX_WORDS) {
		Q_strncpyz( censorNamesDictionary.words[censorNamesDictionary.num_words], nextWord,
									sizeof(censorNamesDictionary.words[censorNamesDictionary.num_words]));
		nextWord = strtok(NULL," ,");
		if (!nextWord) {
			break;
		}
		censorNamesDictionary.num_words++;
	}
	censorNamesDictionary.num_words++;
}

gentity_t *SecondaryEntity( int entityNum )
{
	return G_SecondaryEntityFor(entityNum);
}

entityState_t *SecondaryEntityState( int entityNum )
{
	return &SecondaryEntity(entityNum)->s;
}

qboolean HasExtFlag( int entityNum, int flag )
{
	if (SecondaryEntity(entityNum)->s.eFlags & flag)
		return qtrue;

	return qfalse;
}
