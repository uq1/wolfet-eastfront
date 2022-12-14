// g_local.h -- local definitions for game module

#include "q_shared.h"
#include "bg_public.h"
#include "g_public.h"
#include "etenhanced.h"

#include "../game/be_aas.h"
#include "../game/q_global_defines.h"

extern qboolean SSE_CPU;

//==================================================================

// the "gameversion" client command will print this plus compile date
/*
#ifndef PRE_RELEASE_DEMO
#define GAMEVERSION			"ETEnhanced"
#else
//#define GAMEVERSION			"You look like you need a monkey!"
#define GAMEVERSION			"ettest"
#endif // PRE_RELEASE_DEMO
*/

#ifdef __EF__
#define	GAME_VERSION	"East Front"
#else //!__EF__
#ifdef __ETE__
#define	GAME_VERSION	"ET Enhanced"
#else // AIMod-ET
#define	GAME_VERSION	"AIMod-ET"
#endif //__ETE__
#endif //__EF__

#define	GAMEVERSION GAME_VERSION

#define BODY_QUEUE_SIZE		8

#define	EVENT_VALID_MSEC	300
#define	CARNAGE_REWARD_TIME	3000

#define	INTERMISSION_DELAY_TIME	1000

#define MG42_MULTIPLAYER_HEALTH 350				// JPW NERVE

#ifdef __BOT__
#ifndef MAX_NODES
#define MAX_NODES           65536//19500//10000//8000//32000//16000	// Maximum Nodes (8000)
#define MAX_PATHLIST_NODES  4096/*8192*//*MAX_NODES*///1024	// MAX_NODES??
#endif
#endif //__BOT__

// How long do bodies last?
// SP : Axis: 20 seconds
//		Allies: 30 seconds
// MP : Both 10 seconds
//#define BODY_TIME(t) ((g_gametype.integer != GT_SINGLE_PLAYER || g_gametype.integer == GT_COOP) ? 10000 : (t) == TEAM_AXIS ? 20000 : 30000)
#define BODY_TIME(t) ((t) == TEAM_AXIS ? 20000 : 20000)

#define MAX_MG42_HEAT			1500.f

// gentity->flags
#define	FL_GODMODE				0x00000010
#define	FL_NOTARGET				0x00000020
#ifdef __VEHICLES__
#define FL_EXTRA_BBOX			0x00000080
#endif //__VEHICLES__
#define	FL_TEAMSLAVE			0x00000400	// not the first on the team
#define FL_NO_KNOCKBACK			0x00000800
#define FL_DROPPED_ITEM			0x00001000
#define FL_NO_BOTS				0x00002000	// spawn point not for bot use
#define FL_NO_HUMANS			0x00004000	// spawn point just for bots
#define	FL_AI_GRENADE_KICK		0x00008000	// an AI has already decided to kick this grenade
// Rafael
#define FL_NOFATIGUE			0x00010000	// cheat flag no fatigue

#define FL_TOGGLE				0x00020000	//----(SA)	ent is toggling (doors use this for ex.)
#define FL_KICKACTIVATE			0x00040000	//----(SA)	ent has been activated by a kick (doors use this too for ex.)
#define	FL_SOFTACTIVATE			0x00000040	//----(SA)	ent has been activated while 'walking' (doors use this too for ex.)
#define	FL_DEFENSE_GUARD		0x00080000	// warzombie defense pose

#define	FL_BLANK				0x00100000
#define	FL_BLANK2				0x00200000
#define	FL_NO_MONSTERSLICK		0x00400000
#define	FL_NO_HEADCHECK			0x00800000

#define	FL_NODRAW				0x01000000

#define TKFL_MINES				0x00000001
#define TKFL_AIRSTRIKE			0x00000002
#define TKFL_MORTAR				0x00000004

// movers are things like doors, plats, buttons, etc
typedef enum {
	MOVER_POS1,
	MOVER_POS2,
	MOVER_POS3,
	MOVER_1TO2,
	MOVER_2TO1,
	// JOSEPH 1-26-00
	MOVER_2TO3,
	MOVER_3TO2,
	// END JOSEPH

	// Rafael
	MOVER_POS1ROTATE,
	MOVER_POS2ROTATE,
	MOVER_1TO2ROTATE,
	MOVER_2TO1ROTATE
} moverState_t;

// door AI sound ranges
#define HEAR_RANGE_DOOR_LOCKED		128	// really close since this is a cruel check
#define HEAR_RANGE_DOOR_KICKLOCKED	512
#define HEAR_RANGE_DOOR_OPEN		256
#define HEAR_RANGE_DOOR_KICKOPEN	768

// DHM - Nerve :: Worldspawn spawnflags to indicate if a gametype is not supported
#define NO_GT_WOLF		1
#define NO_STOPWATCH	2
#define NO_CHECKPOINT	4
#define NO_LMS			8

#define MAX_CONSTRUCT_STAGES 3

#define ALLOW_AXIS_TEAM			1
#define ALLOW_ALLIED_TEAM		2
#define ALLOW_DISGUISED_CVOPS	4

// RF, different types of dynamic area flags
#define	AAS_AREA_ENABLED					0x0000
#define	AAS_AREA_DISABLED					0x0001
#define	AAS_AREA_AVOID						0x0010
#define	AAS_AREA_TEAM_AXIS					0x0020
#define	AAS_AREA_TEAM_ALLIES				0x0040
#define	AAS_AREA_TEAM_AXIS_DISGUISED		0x0080
#define	AAS_AREA_TEAM_ALLIES_DISGUISED		0x0100

//============================================================================

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;
typedef struct g_serverEntity_s g_serverEntity_t;

//====================================================================
//

extern int num_stealth_areas;
extern vec3_t stealth_area_origin[1024];
extern int stealth_area_radius[1024];

// Scripting, these structure are not saved into savegames (parsed each start)
typedef struct
{
	char	*actionString;
	qboolean (*actionFunc)(gentity_t *ent, char *params);
	int		hash;
} g_script_stack_action_t;
//
typedef struct
{
	//
	// set during script parsing
	g_script_stack_action_t		*action;			// points to an action to perform
	char						*params;
} g_script_stack_item_t;
//
// Gordon: need to up this, forest has a HUGE script for the tank.....
//#define	G_MAX_SCRIPT_STACK_ITEMS	128
//#define	G_MAX_SCRIPT_STACK_ITEMS	176
// RF, upped this again for the tank
// Gordon: and again...
#define	G_MAX_SCRIPT_STACK_ITEMS	196
//
typedef struct
{
	g_script_stack_item_t	items[G_MAX_SCRIPT_STACK_ITEMS];
	int						numItems;
} g_script_stack_t;
//
typedef struct
{
	int					eventNum;			// index in scriptEvents[]
	char				*params;			// trigger targetname, etc
	g_script_stack_t	stack;
} g_script_event_t;
//
typedef struct
{
	char		*eventStr;
	qboolean	(*eventMatch)( g_script_event_t *event, char *eventParm );
	int			hash;
} g_script_event_define_t;
//
// Script Flags
#define	SCFL_GOING_TO_MARKER	0x1
#define	SCFL_ANIMATING			0x2
#define SCFL_FIRST_CALL			0x4
//
// Scripting Status (NOTE: this MUST NOT contain any pointer vars)
typedef struct
{
	int		scriptStackHead, scriptStackChangeTime;
	int		scriptEventIndex;	// current event containing stack of actions to perform
	// scripting system variables
	int		scriptId;				// incremented each time the script changes
	int		scriptFlags;
	int		actionEndTime;			// time to end the current action
	char	*animatingParams;		// Gordon: read 8 lines up for why i love this code ;)
} g_script_status_t;
//
#define	G_MAX_SCRIPT_ACCUM_BUFFERS 10
//
void G_Script_ScriptEvent( gentity_t *ent, char *eventStr, char *params );
//====================================================================

typedef struct g_constructible_stats_s {
	float	chargebarreq;
	float	constructxpbonus;
	float	destructxpbonus;
	int		health;
	int		weaponclass;
	int		duration;
} g_constructible_stats_t;

#define NUM_CONSTRUCTIBLE_CLASSES	3

extern g_constructible_stats_t g_constructible_classes[NUM_CONSTRUCTIBLE_CLASSES];

qboolean G_WeaponIsExplosive(  meansOfDeath_t mod );
int G_GetWeaponClassForMOD( meansOfDeath_t mod );

//====================================================================

#define MAX_NETNAME			36

#define	CFOFS(x) ((int)&(((gclient_t *)0)->x))

#define MAX_COMMANDER_TEAM_SOUNDS 16

 //unlagged - true ping
 #define NUM_PING_SAMPLES 64
 //unlagged - true ping
 

typedef struct commanderTeamChat_s {
	int index;
} commanderTeamChat_t;

 // forty - realistic hitboxes
 //         based on lerpFrame_t
 typedef struct {
 
 	qhandle_t	oldFrameModel;
 	qhandle_t	frameModel;
 
 	int		oldFrame;
 	int		oldFrameTime;		// time when ->oldFrame was exactly on
 	int		oldFrameSnapshotTime;
 
 	vec3_t		oldFramePos;
 
 	int		frame;
 	int		frameTime;		// time when ->frame will be exactly on
 
 	float		yawAngle;
 	qboolean	yawing;
 	float		pitchAngle;
 	qboolean	pitching;
 
 	int		moveSpeed;
 
 	int		animationNumber;	// may include ANIM_TOGGLEBIT
 	int		oldAnimationNumber;	// may include ANIM_TOGGLEBIT
 	animation_t	*animation;
 	int		animationTime;		// time when the first frame of the animation will be exact
 	float		animSpeedScale;
 
 } glerpFrame_t;
 

#ifdef __BOT__
// Contains the botspecific information, botstate is an gentity_t parameter
struct sodbot_s {

	int clientNum;				// Clientnum of the bot client

	char *name;					// Bot name
	short int skill;			// Bot skill

	int next_node;				// The next node we'll be visiting
	int previous_node;			// The last node we visited before current_node
	int current_node;			// The node we're coming from or are standing at
	int goal_node;				// The end point of our path, the node we're heading to
	int exit_node;				// A way outa here!

	int tries;					// Number of tries // fretn
	int	node_timeout;			// when the node is time out

	vec3_t	move_vector;

	gentity_t *goal_entity;		// Entity we're heading to, if one

};
// Struct typedefs
typedef struct sodbot_s sodbot_t;
#define MAX_BOTNAME 36 // AIMod

//the bot input, will be converted to an usercmd_t
typedef struct bot_input_s
{
	float thinktime;		//time since last output (in seconds)
	vec3_t dir;				//movement direction
	float speed;			//speed in the range [0, 400]
	vec3_t viewangles;		//the view angles
	int actionflags;		//one of the ACTION_? flags
	int weapon;				//weapon to use
} bot_input_t;

typedef enum {
	AIVOICE_REQUEST,
	AIVOICE_TALK,
	AIVOICE_FUNTALK,
	AIVOICE_MAX
} aiVoiceType_t;

//the bot input, will be converted to an usercmd_t
typedef struct aiVoice_s
{
	gentity_t	*target;
	int			mode;
	const		char *id;
	qboolean	voiceonly;
	int			delay;
	qboolean	played;

} aiVoice_t;
/*
typedef struct aiFireTeam_s
{
	gentity_t	*leader;
	int			num_members;
	gentity_t	*members[16];
	int			goal_node;
	int			pathsize;
	int	pathlist[MAX_PATHLIST_NODES];
} aiFireTeam_t;

extern	int					num_aiFireTeams;
extern	aiFireTeam_t		aiFireTeams[];
*/
#endif //__BOT__

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#endif //_WIN32

struct gentity_s {
	entityState_t	s;				// communicated by server to clients
	entityShared_t	r;				// shared by both the server system and game

	//qint64 weapons;

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	struct gclient_s	*client;			// NULL if not a client

	qboolean	inuse;
	
	vec3_t		instantVelocity;	// ydnar: per entity instantaneous velocity, set per frame

	char		*classname;			// set in QuakeEd
	int			spawnflags;			// set in QuakeEd

	qboolean	neverFree;			// if true, FreeEntity will only unlink
									// bodyque uses this

	int			flags;				// FL_* variables

	char		*model;
	char		*model2;
	int			freetime;			// level.time when the object was freed
	
	int			eventTime;			// events will be cleared EVENT_VALID_MSEC after set
	qboolean	freeAfterEvent;
	qboolean	unlinkAfterEvent;

	qboolean	physicsObject;		// if true, it can be pushed by movers and fall off edges
									// all game items are physicsObjects, 
	float		physicsBounce;		// 1.0 = continuous bounce, 0.0 = no bounce
	int			clipmask;			// brushes with this content value will be collided against
									// when moving.  items and corpses do not collide against
									// players, for instance

	int			realClipmask;		// Arnout: use these to backup the contents value when we go to state under construction
	int			realContents;
	qboolean	realNonSolidBModel;	// For script_movers with spawnflags 2 set

	// movers
	moverState_t moverState;
	int			soundPos1;
	int			sound1to2;
	int			sound2to1;
	int			soundPos2;
	int			soundLoop;
	int			sound2to3;
	int			sound3to2;
	int			soundPos3;

	int			soundSoftopen;
	int			soundSoftendo;
	int			soundSoftclose;
	int			soundSoftendc;

	gentity_t	*parent;
#ifdef __VEHICLES__
	gentity_t	*turret_ent;
#endif //__VEHICLES__

	gentity_t	*nextTrain;
	gentity_t	*prevTrain;
	vec3_t		pos1, pos2, pos3;

	char		*message;

	int			timestamp;		// body queue sinking, etc

	float		angle;			// set in editor, -1 = up, -2 = down
	char		*target;

	char		*targetname;
	int			targetnamehash; // Gordon: adding a hash for this for faster lookups

	char		*team;
	gentity_t	*target_ent;

	float		speed;
	float		closespeed;		// for movers that close at a different speed than they open
	vec3_t		movedir;

	int			gDuration;
	int			gDurationBack;
	vec3_t		gDelta;
	vec3_t		gDeltaBack;

	vec3_t		muzzlePoint;

	int			nextthink;
	void		(*free)(gentity_t *self);
	void		(*think)(gentity_t *self);
	void		(*reached)(gentity_t *self);	// movers call this when hitting endpoint
	void		(*blocked)(gentity_t *self, gentity_t *other);
	void		(*touch)(gentity_t *self, gentity_t *other, trace_t *trace);
	void		(*use)(gentity_t *self, gentity_t *other, gentity_t *activator);
	void		(*pain)(gentity_t *self, gentity_t *attacker, int damage, vec3_t point);
	void		(*die)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod);

	int			pain_debounce_time;
	int			fly_sound_debounce_time;	// wind tunnel

	int			health;
	int			maxhealth; // UQ1: Added for tank max health...

	qboolean	takedamage;

	int			damage;
	int			splashDamage;	// quad will increase this without increasing radius
	int			splashRadius;
	int			methodOfDeath;
	int			splashMethodOfDeath;

	int			count;

	gentity_t	*chain;
	gentity_t	*enemy;
	gentity_t	*activator;
	gentity_t	*teamchain;		// next entity in team
	gentity_t	*teammaster;	// master of the team

	meansOfDeath_t	deathType;

	int			watertype;
	int			waterlevel;

	int			noise_index;

	// timing variables
	float		wait;
	float		random;

	// Rafael - sniper variable
	// sniper uses delay, random, radius
	int			radius;
	float		delay;

	// JOSEPH 10-11-99
	int			TargetFlag;
	float		duration;
	vec3_t		rotate;
	vec3_t		TargetAngles;
	// END JOSEPH

	gitem_t		*item;			// for bonus items

	// Ridah, AI fields
	char		*aiName;
	int			aiTeam;
	void		(*AIScript_AlertEntity)( gentity_t *ent );
	// done.

	char		*aiSkin;

	vec3_t		dl_color;
	char		*dl_stylestring;
	char		*dl_shader;
	int			dl_atten;

	qboolean	will_explode;

	int			key;			// used by:  target_speaker->nopvs, 

	qboolean	active;

	// Rafael - mg42
	float		harc;
	float		varc;

	int			props_frame_state;

	// Ridah
	int			missionLevel;		// mission we are currently trying to complete
									// gets reset each new level
	int			start_size;
	int			end_size;

	// Rafael props

	qboolean	isProp;

	int			mg42BaseEnt;

	gentity_t	*melee;

	char		*spawnitem;

	int			flameQuota, flameQuotaTime, flameBurnEnt;

	int			count2;

	int			grenadeExplodeTime;	// we've caught a grenade, which was due to explode at this time
	int			grenadeFired;		// the grenade entity we last fired

	char		*track;

	int			eFlags2;

	// entity scripting system
	char				*scriptName;

	int					numScriptEvents;
	g_script_event_t	*scriptEvents;	// contains a list of actions to perform for each event type
	g_script_status_t	scriptStatus;	// current status of scripting
	// the accumulation buffer
	int scriptAccumBuffer[G_MAX_SCRIPT_ACCUM_BUFFERS];

	qboolean	AASblocking;
	vec3_t		AASblocking_mins, AASblocking_maxs;
	float		accuracy;

	char		tagName[MAX_QPATH];		// name of the tag we are attached to
	gentity_t	*tagParent;
	gentity_t	*tankLink;

	int			lastHintCheckTime;			// DHM - Nerve
	int			voiceChatSquelch;			// DHM - Nerve
	int			voiceChatPreviousTime;		// DHM - Nerve
	int			lastBurnedFrameNumber;		// JPW - Nerve   : to fix FT instant-kill exploit

	entState_t	entstate;
	char		*constages;
	char		*desstages;
	char		*damageparent;
	int			conbmodels[MAX_CONSTRUCT_STAGES+1];
	int			desbmodels[MAX_CONSTRUCT_STAGES];
	int			partofstage;

	int			allowteams;

	int			spawnTime;

	gentity_t	*dmgparent;
	qboolean	dmginloop;

	// RF, used for linking of static entities for faster searching
	gentity_t	*botNextStaticEntity;
	int			spawnCount;					// incremented each time this entity is spawned
	int			botIgnoreTime, awaitingHelpTime;
	int			botIgnoreHealthTime, botIgnoreAmmoTime;
	int			botAltGoalTime;
	vec3_t		botGetAreaPos;
	int			botGetAreaNum;
	int			aiInactive;		// bots should ignore this goal
	int			goalPriority[2];

	int			tagNumber;		// Gordon: "handle" to a tag header
	
	int				linkTagTime;

	splinePath_t*	backspline;
	vec3_t			backorigin;
	float			backdelta;
	qboolean		back;
	qboolean		moving;

	int			botLastAttackedTime;
	int			botLastAttackedEnt;

	int			botAreaNum;			// last checked area num
	vec3_t		botAreaPos;

	// TAT 10/13/2002 - for seek cover sequence - we need a pointer to a server entity
	//		@ARNOUT - does this screw up the save game?
	g_serverEntity_t	*serverEntity;

	// What sort of surface are we standing on?
	int		surfaceFlags;

	char	tagBuffer[16];

	// bleh - ugly
	int		backupWeaponTime;
	int		mg42weapHeat;

	vec3_t	oldOrigin;

	qboolean runthisframe;

	g_constructible_stats_t	constructibleStats;

#ifdef __SHELLSHOCK__
	int			shellShockEndTime;
	int			shellShockStartTime;
#endif //__SHELLSHOCK__

	//bani
	int	etpro_misc_1;

	int flag_capture_team;

 	// forty - realistic hitboxes
 	glerpFrame_t	legsFrame;
 	glerpFrame_t	torsoFrame;
 	int		timeShiftTime;
 
 	// forty - dynamite chaining
 	gentity_t	*onobjective;

	float		scale; // UQ1: Added!

	int			vehicle_in_water_time;

//#ifdef __VEHICLES__
 	qboolean		tankBox;
 	int				TankMarker;
	int             ArtilleryMarker;//Masteries, probably needed
 	int				BackupHealth;
	int				turretYaw;
	int				tank_attack_time;
	vec3_t			tank_original_mins;
	vec3_t			tank_original_maxs;
	int				vehicle_pause_time;
	int				vehicle_bbox_cheat_time;
	int				vehicle_apc_next_ammo_health_check;
// Ridah, AI fields
	char		*aiAttributes;
//	char		*aiName;
//	int			aiTeam;
//	void		(*AIScript_AlertEntity)( gentity_t *ent );
//	qboolean	aiInactive;
	int			aiCharacter;	// the index of the type of character we are (from aicast_soldier.c)
//	char		*aiSkin;
	char		*aihSkin;
	int			bot_hear_enemy_time;
	int			bot_hear_enemy_check_time;
	int			bot_snipetime;
	int			bot_reload_time;
	vec3_t		bot_sp_spawnpos;
	int			bot_sp_indentifier;
	qboolean	bot_no_respawn;
	qboolean	bot_initialized;
//	vec3_t		bot_sp_waypoints[64];
	vec3_t		bot_sp_destination;
	vec3_t		bot_coverspot;
	int			bot_coverspot_time;
	int			bot_coverspot_next_check_time;
	gentity_t	*bot_coverspot_enemy;
	vec3_t		bot_coverspot_enemy_position;
	int			bot_coverspot_staydown_timer;
	vec3_t		bot_coverspot_completed_destination;
	qboolean	bot_coverspot_completed_destination_setup;
	qboolean	bot_coverspot_return;
//	qboolean	bot_coverspot_displaying;
//	vec3_t		bot_coverspot_original_position;

	// SP Bot Script Settings...
	int			NPC_PlayerType;
	qboolean	NPC_Patroller;
	qboolean	NPC_Mounted;
	qboolean	NPC_Hunter;
	// done.

#ifdef __ETE__
	int			supression;
	int			supression_reduction_timer;
	gentity_t	*supression_agressor;
	vec3_t		supressLocation;
	int			supressTimer;
#endif //__ETE__

	vec3_t		last_hit_point;

//#endif //__VEHICLES__
#ifdef __BOT__
	// AIMod Stuff -- UQ1: Need to clear these out a bit :(
	qboolean		is_bot_goal;
	int				in_range_nodes[256];
	int				num_in_range_nodes;
	int				bot_random_move_time;
	int				bot_ladder_time;
	int				bot_no_ladder_use_time;
	int				bot_last_enemy_seen_time[MAX_GENTITIES/*MAX_CLIENTS*/];
	int				bot_next_voice_time;
	int				bot_last_frame_time;
	int				bot_last_good_velocity_time;
	int				bot_last_vischeck_node_time;
	int				bot_last_position_check;
	vec3_t			bot_last_position;
	qboolean		is_bot;
	int				beStill;
	int				beStillEng;
	int				nextEnemyCheck;
	int				bot_next_close_enemy_check;
	qboolean		lastEnemy;
	int				botClassnum; // Stored bot class number
	float			headshotDamageScale;
	sodbot_t		*bs;
	int				lastMedicVoiceChat;
	int				solid;
	int             state;          // Bot State
    int				action;			// Bot action it is undertaking
	int				weaponchoice;
	int				skillchoice;
	int				bot_last_node_link_flags;
	int				bot_last_node_link_node;

	// nodes
    int             current_node;   // Current Node
	int             current_nodes[32];   // Current Nodes
	int				current_nodes_total;
    int             goal_node;      // Goal Node
    int             next_node;      // Next Node
    int             last_node;      // Last Node
	int             exit_node;      // Exit Node
	int				needed_node;	// Node for medics/fldops/engineer's when called by a player...
	int             node_timeout;   // Node Timeout
//    int             tries;          // Number of Tries

//    qboolean        is_jumping;     // Is Bot Jumping?
//    qboolean        bot_client;     // Bot Client
	char			bot_name[MAX_BOTNAME];
    float           teamPauseTime;  // To stop the centipede effect and seperate the team out a little
    gentity_t       *lastkilledby;  // Last Killed By (set in ClientObituary)

    // movement
//	vec3_t          temp_move_vector;   // Move Vector
    vec3_t          move_vector;        // Move Vector
//    float           next_move_time;     // Next Move Time
    float           wander_timeout;     // Wander Timeout
    float           suicide_timeout;    // Suicide Timeout
//    float           shooting_timeout;   // Shoot Timeout

//	int				mynum;
	gentity_t		*movetarget;
	gentity_t		*followtarget;
	int				followtarget_waypoint_time;
	int				bot_forced_forward_move;
	int				bot_fireteam_order;
	int				bot_fireteam_ammo;
	vec3_t			bot_fireteam_order_coverspot;
	vec3_t			bot_fireteam_order_assault_position;
	vec3_t			bot_fireteam_order_supression_position;
	gentity_t		*bot_fireteam_order_supression_enemy;
	int				bot_fireteam_order_supression_enemy_check_time;
	qboolean		bot_fireteam_attack_in_route;
	int				bot_fireteam_next_route_update;
	int				bot_explosive_avoid_time;
	int				bot_explosive_avoid_waypoint;
	int				bot_explosive_avoid_waypoint_avoiding;
	gentity_t		*goalentity;
	float			yaw_speed;
	int				bot_next_enemy_scan_time;
	int				bot_duck_time;
	int				bot_jump_time;
	int				bot_strafe_left_time;
	int				bot_strafe_right_time;
	// New A* pathing..
	int				last_astar_path_think;
	int				last_shorten_path_node;
	int				pathsize;
	/*short*/ int		pathlist[MAX_PATHLIST_NODES];
	int				shortTermGoal;
	int				long_term_goal_timer;
	int				longterm_pathsize;
//	/*short*/ int		longterm_pathlist[MAX_PATHLIST_NODES];
	//vec3_t			bad_origin;
	int				longTermGoal;
	int				longTermGoalTries;
	int				longTermGoalNodes[32];
	int				longTermGoalNodes_total;
	int				current_target_entity;
	vec3_t			bot_goal_position;
	gentity_t		*bot_goal_entity;
	bot_input_t		*bi;
	qboolean		bot_goal_destroyable;
	qboolean		bot_goal_constructible;
	qboolean		bot_can_build;
	qboolean		bot_wanderring;
	int				bot_route_think_delay;
	int				bot_last_good_move_time;
	int				bot_last_real_move_time;
	int				bot_last_attack_time;
	int				bot_job_wait_time;
	int				bot_last_move_fix_time;
	int				bot_last_chat_time;
	vec3_t			bot_sniper_spot;
	int				bot_sniping_time;
	qboolean		bot_sniping;
	int				bot_snipe_node;
	int				bot_runaway_time;
	qboolean		bot_creating_path;
	qboolean		bot_queue_lock;
	
	vec3_t			bot_muzzlepoint;

	qboolean		bot_defender;
	qboolean		bot_stand_guard;

	int				aiFireTeam;

	qboolean	bot_after_ammo;

	int				bot_name_team;

#ifdef _WIN32
	DWORD		bot_pathfind_thread; // UQ1: Added to thread out finding paths...
#endif //_WIN32

	// Pathfinding cpu savers...
	int				bot_patrol_time;
	int				bot_next_goalfind_time;

	int				bot_fastsnipe_check;
	qboolean		bot_fastsnipe_active;
	vec3_t			bot_fastsnipe_pos;

	vec3_t			bot_enemy_stored_pos;
	int				bot_enemy_visible_time;
	gentity_t		*bot_last_visible_enemy;
	vec3_t			bot_followtarget_stored_pos;
	int				bot_followtarget_visible_time;
	vec3_t			bot_movetarget_stored_pos;
	int				bot_movetarget_visible_time;

	int				bot_special_action_thinktime;

	int				bot_strafe_left_timer;
	int				bot_strafe_right_timer;
	vec3_t			bot_strafe_target_position;
	vec3_t			bot_ideal_view_angles;
	qboolean		bot_end_fire_next_frame; // For releasing grenades...
	usercmd_t		*bot_last_ucmd;
	int				bot_blind_routing_time;
	qboolean		team_leader;
	vec3_t			AAS_goal_origin;
	vec3_t			last_good_node_point;
	vec3_t			last_added_waypoint;
/*	int				routing_pathsize;
	vec3_t			routing_pathlist[MAX_PATHLIST_NODES/4];*/
	int				bot_mg42_set_time;
	int				bot_mg42_set_staydown_time;
	vec3_t			attack_vector;
	int				strafeDir;
	int				strafeTime;
	int				bot_wait_for_supplies;
	int				bot_wait_for_supplies_end;
	int				bot_dont_give_supplies_end;
	int				bot_last_reachable_check;
	int				bot_turn_wait_time;
	aiVoice_t		AIvoiceChat[AIVOICE_MAX];
	gentity_t		*quick_job_target;
	int				quick_job_target_visible_time;
	int				quick_job_time;
	int				quick_job_type;
	int				next_quick_job_scan;
	int				next_corpse_scan;
	int				last_helped_client;
	int				last_helped_time;
	int				next_sp_move_scan;
	int				next_coverspot_check;
	int				next_enemy_scan_time;
	int				next_nodefind_time;
#endif //__BOT__
#ifdef __NPC__
	struct gclient_s	*NPC_client;				// NPC's ver of a clientstate...
	int					NPC_Class;
	int					NPC_Humanoid_Type;
	int					NPC_CoverPoint_Timer;
	int					NPC_CoverPoint_LookUpTimer;
	float				NPC_CoverPoint_Last_Distance;
	qboolean			NPC_CoverPoint;
	qboolean			Airstrike_Ready;
	int					playerType;
	bg_character_t		*character;
	int					characterIndex;
	int					next_enemy_check;
	qboolean			reversed_route;
	int					next_fire_time;
	int					NPC_reload_timer;
	int					NPC_shots_fired;
	vec3_t				SP_NPC_Position;
	gentity_t			*npc_mg42;

	//	gentity_t		*npc_mg42_base;
	qboolean			npc_mount;
	qboolean			npc_hunter;
	qboolean			npc_patrol;
	int					npc_patrol_start_node;
	int					npc_patrol_end_node;
	int					npc_next_talk;
	int					npc_next_idle_talk;
//	lcvp_node_t			lcvp_nodes[100];
//	Destroyer_node_t	Destroyer_nodes[100];
	vec3_t				npc_ideal_view_angles;
	vec3_t				npc_view_angles;
	vec3_t				npc_velocity;
	int					npc_spawnpoints[10];
	int					npc_driver;
	int					npc_gunner1;
	int					npc_gunner2;

	int					npc_query_timer;
	int					npc_alert_timer;
#endif //__NPC__

	int					last_use_time;

	gentity_t *death_self;
	gentity_t *death_inflictor;
	gentity_t *death_attacker;
	int death_damage;
	int death_meansOfDeath;

	qboolean			player_mg42;
};

#ifdef __BOT__
#define BOT_JOB_NONE		0
#define BOT_JOB_SYRINGE		1
#define BOT_JOB_MEDKIT		2
#define BOT_JOB_AMMO		3
#define BOT_JOB_FOLLOW		4
#define BOT_JOB_LOST		5

#define BOT_QUICKJOB_NONE		0
#define BOT_QUICKJOB_GETAMMO	1
#define BOT_QUICKJOB_GETMEDKIT	2
#define BOT_QUICKJOB_GETCORPSE	3
#define BOT_QUICKJOB_GIVEAMMO	4
#define BOT_QUICKJOB_GIVEMEDKIT 5
#define BOT_QUICKJOB_REVIVE		6
#define BOT_QUICKJOB_ENGINEER	7
#endif //__BOT__

// Ridah
//#include "ai_cast_global.h"
// done.

typedef enum {
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
} clientConnected_t;

typedef enum {
	SPECTATOR_NOT,
	SPECTATOR_FREE,
	SPECTATOR_FOLLOW/*,
	SPECTATOR_SCOREBOARD*/
} spectatorState_t;

typedef enum {
	COMBATSTATE_COLD,
	COMBATSTATE_DAMAGEDEALT,
	COMBATSTATE_DAMAGERECEIVED,
	COMBATSTATE_KILLEDPLAYER
} combatstate_t;

typedef enum {
	TEAM_BEGIN,		// Beginning a team game, spawn at base
	TEAM_ACTIVE		// Now actively playing
} playerTeamStateState_t;

typedef struct {
	playerTeamStateState_t	state;

	int			location[2];

	int			captures;
	int			basedefense;
	int			carrierdefense;
	int			flagrecovery;
	int			fragcarrier;
	int			assists;

	float		lasthurtcarrier;
	float		lastreturnedflag;
	float		flagsince;
	float		lastfraggedcarrier;
} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define	FOLLOW_ACTIVE1	-1
#define	FOLLOW_ACTIVE2	-2


// OSP - weapon stat counters
typedef struct {
	unsigned int atts;
	unsigned int deaths;
	unsigned int headshots;
	unsigned int hits;
	unsigned int kills;
} weapon_stat_t;


// client data that stays across multiple levels or tournament restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
typedef struct {
	team_t		sessionTeam;
	int			spectatorTime;		// for determining next-in-line to play
	spectatorState_t	spectatorState;
	int			spectatorClient;	// for chasecam and follow mode
	int			playerType;			// DHM - Nerve :: for GT_WOLF
	int			playerWeapon;		// DHM - Nerve :: for GT_WOLF
	int			playerWeapon2;		// Gordon: secondary weapon
	int			spawnObjectiveIndex; // JPW NERVE index of objective to spawn nearest to (returned from UI)
	int			latchPlayerType;	// DHM - Nerve :: for GT_WOLF not archived
	int			latchPlayerWeapon;	// DHM - Nerve :: for GT_WOLF not archived
	int			latchPlayerWeapon2;	// Gordon: secondary weapon
	int			ignoreClients[MAX_CLIENTS / (sizeof(int)*8)];
	qboolean	muted;
	float		skillpoints[SK_NUM_SKILLS];		// Arnout: skillpoints
	float		startskillpoints[SK_NUM_SKILLS];// Gordon: initial skillpoints at map beginning
	float		startxptotal;
	int			skill[SK_NUM_SKILLS];			// Arnout: skill
	int			rank;							// Arnout: rank
	int			medals[SK_NUM_SKILLS];			// Arnout: medals

	// OSP
	int			coach_team;
	int			damage_given;
	int			damage_received;
	int			deaths;
	int			game_points;
	int			kills;
	int			referee;
	int			rounds;
	int			spec_invite;
	int			spec_team;
	int			suicides;
	int			team_damage;
	int			team_kills;

	// matt
	float team_hits;
	float hits;
	int numBinocs;

	// Perro - Killing Streaks, death streaks
	int				kstreak;
	int				dstreak;
	// Perro - damage-based XP
	int				XPdmg;

	// josh
	int auto_mute_time;
	int match_killrating;
	int overall_killrating;
	int playerrating;

	// tjw
	int ATB_count;

	weapon_stat_t aWeaponStats[WS_MAX+1];	// Weapon stats.  +1 to avoid invalid weapon check
	// OSP

	qboolean	versionOK;
} clientSession_t;

//
#define	MAX_VOTE_COUNT		3

#define PICKUP_ACTIVATE	0	// pickup items only when using "+activate"
#define PICKUP_TOUCH	1	// pickup items when touched
#define PICKUP_FORCE	2	// pickup the next item when touched (and reset to PICKUP_ACTIVATE when done)

// OSP -- multiview handling
#define MULTIVIEW_MAXVIEWS	16
typedef struct {
	qboolean	fActive;
	int			entID;
	gentity_t	*camera;
} mview_t;

typedef struct ipFilter_s {
	unsigned	mask;
	unsigned	compare;
} ipFilter_t;

#define MAX_COMPLAINTIPS 5

// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct {
	clientConnected_t	connected;	
	usercmd_t	cmd;				// we would lose angles if not persistant
	usercmd_t	oldcmd;				// previous command processed by pmove()
	qboolean	localClient;		// true if "ip" info key is "localhost"
	qboolean	initialSpawn;		// the first spawn should be at a cool location
	qboolean	predictItemPickup;	// based on cg_predictItems userinfo
	qboolean	pmoveFixed;			//
	char		netname[MAX_NETNAME];

	int			autoActivate;		// based on cg_autoactivate userinfo		(uses the PICKUP_ values above)

	int			maxHealth;			// for handicapping
	int			enterTime;			// level.time the client entered the game
	int			connectTime;		// DHM - Nerve :: level.time the client first connected to the server
	playerTeamState_t teamState;	// status in teamplay games
	int			voteCount;			// to prevent people from constantly calling votes
	int			teamVoteCount;		// to prevent people from constantly calling votes

	int			complaints;				// DHM - Nerve :: number of complaints lodged against this client
	int			complaintClient;		// DHM - Nerve :: able to lodge complaint against this client
	int			complaintEndTime;		// DHM - Nerve :: until this time has expired

	int			lastReinforceTime;		// DHM - Nerve :: last reinforcement

	qboolean	teamInfo;			// send team overlay updates?

	qboolean	bAutoReloadAux;			// TTimo - auxiliary storage for pmoveExt_t::bAutoReload, to achieve persistance

	int			applicationClient;		// Gordon: this client has requested to join your fireteam
	int			applicationEndTime;		// Gordon: you have X seconds to reply or this message will self destruct!

	int			invitationClient;		// Gordon: you have been invited to join this client's fireteam
	int			invitationEndTime;		// Gordon: quickly now, chop chop!.....

	int			propositionClient;		// Gordon: propositionClient2 has requested you invite this client to join the fireteam
	int			propositionClient2;		// Gordon: 
	int			propositionEndTime;		// Gordon: tick, tick, tick....

	int			autofireteamEndTime;
	int			autofireteamCreateEndTime;
	int			autofireteamJoinEndTime;

	playerStats_t	playerStats;

	//gentity_t	*wayPoint;

	int			lastBattleSenseBonusTime;
	int			lastHQMineReportTime;
	int			lastCCPulseTime;

	int			lastSpawnTime;
	int			lastTeamChangeTime;

	char		botScriptName[MAX_NETNAME];

	// OSP
	unsigned int	autoaction;			// End-of-match auto-requests
	unsigned int	clientFlags;		// Client settings that need server involvement
	unsigned int	clientMaxPackets;	// Client com_maxpacket settings
	unsigned int	clientTimeNudge;	// Client cl_timenudge settings
	int				cmd_debounce;		// Dampening of command spam
	unsigned int	invite;				// Invitation to a team to join
	mview_t			mv[MULTIVIEW_MAXVIEWS];	// Multiview portals
	int				mvCount;			// Number of active portals
	int				mvReferenceList;	// Reference list used to generate views after a map_restart
	int				mvScoreUpdate;		// Period to send score info to MV clients
	int				panzerDropTime;		// Time which a player dropping panzer still "has it" if limiting panzer counts
	int				panzerSelectTime;	// *when* a client selected a panzer as spawn weapon
	qboolean		ready;				// Ready state to begin play
	// OSP

	bg_character_t	*character;
    int				characterIndex;

	ipFilter_t		complaintips[MAX_COMPLAINTIPS];

	// tjw 
	int hitsounds;
	qboolean slashKill;
	int lastkilled_client;
	int	lastrevive_client;
	int	lastkiller_client;
	int	lastammo_client;
	int	lasthealth_client;

//unlagged - true ping
	int			realPing;
	int			pingsamples[NUM_PING_SAMPLES];
	int			samplehead;
//unlagged - true ping
} clientPersistant_t;

typedef struct {
	vec3_t mins;
	vec3_t maxs;

	vec3_t origin;

	//josh: Need these for BuildHead/Legs
	int eFlags; // s.eFlags to ps.eFlags
	int viewheight; // ps for both
	int pm_flags; // ps for both
	vec3_t viewangles; // s.apos.trBase to ps.viewangles

	int time;

	// forty - realistic headboxes - torso markers
 	qhandle_t torsoOldFrameModel;
 	qhandle_t torsoFrameModel;
	int torsoOldFrame;
	int torsoFrame;
	int torsoOldFrameTime;
	int torsoFrameTime;
	float torsoYawAngle;	
	float torsoPitchAngle;	
	qboolean torsoYawing;
	qboolean torsoPitching;

	// forty - realistic headboxes - leg markers
 	qhandle_t legsOldFrameModel;
 	qhandle_t legsFrameModel;
	int legsOldFrame;
	int legsFrame;
	int legsOldFrameTime;
	int legsFrameTime;
	float legsYawAngle;	
	float legsPitchAngle;	
	qboolean legsYawing;
	qboolean legsPitching;

} clientMarker_t;



#define MAX_CLIENT_MARKERS 10

// CHRUKER: b068 - These two are only referenced in g_stats.c and there
//          they don't do anything.
//#define NUM_SOLDIERKILL_TIMES 10
//#define SOLDIERKILL_MAXTIME 60000

#define LT_SPECIAL_PICKUP_MOD	3		// JPW NERVE # of times (minus one for modulo) LT must drop ammo before scoring a point
#define MEDIC_SPECIAL_PICKUP_MOD	4	// JPW NERVE same thing for medic

// Gordon: debris test
typedef struct debrisChunk_s {
	vec3_t	origin;
	int		model;
	vec3_t	velocity;
	char	target[32];
	char	targetname[32];
} debrisChunk_t;

#define MAX_DEBRISCHUNKS		256
// ===================

// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s {
	// ps MUST be the first element, because the server expects it
	playerState_t	ps;				// communicated by server to clients

	// the rest of the structure is private to game
	clientPersistant_t	pers;
	clientSession_t		sess;

	qboolean	noclip;
//unlagged - smooth clients #1
	// this is handled differently now
/*
	int			lastCmdTime;		// level.time of last usercmd_t, for EF_CONNECTION
									// we can't just use pers.lastCommand.time, because
									// of the g_sycronousclients case
*/
//unlagged - smooth clients #1

	int			buttons;
	int			oldbuttons;
	int			latched_buttons;

	int			wbuttons;
	int			oldwbuttons;
	int			latched_wbuttons;
	vec3_t		oldOrigin;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int			damage_blood;		// damage taken out of health
	int			damage_knockback;	// impact damage
	vec3_t		damage_from;		// origin for vector calculation
	qboolean	damage_fromWorld;	// if true, don't use the damage_from vector

//	int			accurateCount;		// for "impressive" reward sound

//	int			accuracy_shots;		// total number of shots
//	int			accuracy_hits;		// total number of hits

	//
	int			lastkilled_client;	// last client that this client killed
	int			lasthurt_client;	// last client that damaged this client
	int			lasthurt_mod;		// type of damage the client did
	int			lasthurt_time;		// level.time of last damage
	int			lastrevive_client;
	int			lastkiller_client;
	int			lastammo_client;
	int			lasthealth_client;

	// timers
	int			respawnTime;		// can respawn when time > this, force after g_forcerespwan
	int			inactivityTime;		// kick players when time > this
	qboolean	inactivityWarning;	// qtrue if the five seoond warning has been given
	int			rewardTime;			// clear the EF_AWARD_IMPRESSIVE, etc when time > this

	int			airOutTime;

	int			lastKillTime;		// for multiple kill rewards

	qboolean	fireHeld;			// used for hook
	gentity_t	*hook;				// grapple hook if out

	int			switchTeamTime;		// time the player switched teams

	// timeResidual is used to handle events that happen every second
	// like health / armor countdowns and regeneration
	int			timeResidual;

	float		currentAimSpreadScale;

	gentity_t	*persistantPowerup;
	int			portalID;
	int			ammoTimes[WP_NUM_WEAPONS];
	int			invulnerabilityTime;

	gentity_t	*cameraPortal;				// grapple hook if out
	vec3_t		cameraOrigin;

	int			dropWeaponTime; // JPW NERVE last time a weapon was dropped
	int			limboDropWeapon; // JPW NERVE weapon to drop in limbo
	int			deployQueueNumber; // JPW NERVE player order in reinforcement FIFO queue
	int			lastBurnTime; // JPW NERVE last time index for flamethrower burn
	int			PCSpecialPickedUpCount; // JPW NERVE used to count # of times somebody's picked up this LTs ammo (or medic health) (for scoring)
	int			saved_persistant[MAX_PERSISTANT];	// DHM - Nerve :: Save ps->persistant here during Limbo

	gentity_t	*touchingTOI;	// Arnout: the trigger_objective_info a player is touching this frame

	int			lastConstructibleBlockingWarnTime;
	int			lastConstructibleBlockingWarnEnt;

	// CHRUKER: b068 - These two are only referenced in g_stats.c and
	//          there they don't do anything.
	//int			soldierKillMarker;
	//int			soliderKillTimes[NUM_SOLDIERKILL_TIMES];

	int			landmineSpottedTime;
	gentity_t*	landmineSpotted;

	int			speedScale;

	combatstate_t	combatState;

	int				topMarker;
	clientMarker_t	clientMarkers[MAX_CLIENT_MARKERS];
	clientMarker_t	backupMarker;

	gentity_t		*tempHead;	// Gordon: storing a temporary head for bullet head shot detection
	gentity_t		*tempLeg;	// Arnout: storing a temporary leg for bullet head shot detection

	int				botSlotNumber;  // the slot the bot falls into (set up in the initial UI screen)
	// END		xkan, 8/27/2002

	int				flagParent;

	// the next 2 are used to play the proper animation on the body
	int				torsoDeathAnim;
	int				legsDeathAnim;

	int				lastSpammyCentrePrintTime;
	pmoveExt_t		pmext;
	qboolean		isCivilian;		// whether this is a civilian
	int				deathTime;		// if we are dead, when did we die

	int				lastHealTimes[2];
	int				lastAmmoTimes[2];

	char			disguiseNetname[MAX_NETNAME];
	int				disguiseRank;

	int				medals;
	float			acc;

	qboolean		hasaward;
	qboolean		wantsscore;
	qboolean		maxlivescalced;

 	int				XPSave_lives;
 	qboolean		XPSave_loaded;
 	int				flametime; 
 
 //unlagged - smooth clients #1
 	// the last frame number we got an update from this client
 	int			lastUpdateFrame;
 //unlagged - smooth clients #1
 
 //unlagged - backward reconciliation #1
 // an approximation of the actual server time we received this
 	// command (not in 50ms increments)
 	int			frameOffset;
 //unlagged - backward reconciliation #1
 	qboolean warping;
 	qboolean warped;

#ifdef __BOT__
	int				AIbaseweapon;
	int				AISpecialTime;
	int				AISpecialCount;
	gentity_t		*attacker;
	int				lastAttackTime;
	int				AImovetime;
#endif
#ifdef __ETE__
	gentity_t		*touchEnt;
#endif //__ETE__
};

typedef struct {
	char	modelname[32];
	int		model;
} brushmodelInfo_t;

typedef struct limbo_cam_s {
	qboolean	hasEnt;
	int			targetEnt;
	vec3_t		angles;
	vec3_t		origin;
	qboolean	spawn;
	int			info;
} limbo_cam_t;

#define MAX_LIMBO_CAMS 32


// this structure is cleared as each map is entered
#define	MAX_SPAWN_VARS			64
#define	MAX_SPAWN_VARS_CHARS	2048
#define VOTE_MAXSTRING			256		// Same value as MAX_STRING_TOKENS

// tjw: increased from 8 to 10 for compatability with maps that relied on 
//      it before Project: Bug Fix #055
#define	MAX_SCRIPT_ACCUM_BUFFERS	10

#define MAX_BUFFERED_CONFIGSTRINGS 128

typedef struct voteInfo_s {
	char		voteString[MAX_STRING_CHARS];
	int			voteTime;				// level.time vote was called
	int			voteYes;
	int			voteNo;
	int			numVotingClients;		// set by CalculateRanks
	int			numVotingTeamClients[2];
	int			(*vote_fn)(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
	char		vote_value[VOTE_MAXSTRING];	// Desired vote item setting.
 	int         voteCaller; // id of the vote caller
 	int         voteTeam;   // id of the vote caller's team
} voteInfo_t;

#define MAX_FORCECVARS 64

#ifdef __BOT__

typedef enum {
	BOTGOAL_CONSTRUCT,	// Engineer goal... Construction.
	BOTGOAL_DESTROY,	// Engineer goal... Destruction.
	BOTGOAL_CAMP,		// Generic goal... Camp spot.
	BOTGOAL_COVERSPOT,	// Generic goal... Cover spot.
	BOTGOAL_ROAM,		// Generic goal... Roaming Location.
	BOTGOAL_REVIVE,		// Medic goal... Revive a teammate.
	BOTGOAL_GIVEHEALTH,	// Medic goal... A teammate needs health.
	BOTGOAL_GIVEAMMO,	// Field-Ops goal... A teammate needs ammo.
	BOTGOAL_FINDHEALTH, // Generic goal... Health supplies.
	BOTGOAL_FINDAMMO,	// Generic goal... Ammo supplies.
	BOTGOAL_ENEMY,		// Generic goal... An enemy.
	BOTGOAL_FLAG,		// Generic goal... ET Flag.
	BOTGOAL_POPFLAG,	// Generic goal... POP Capture point (flag).
	BOTGOAL_ACTIVATE,	// Generic goal... Activate something.
	BOTGOAL_USE,		// Generic goal... Use something (MG42, AAGUN, etc). Any time we want to camp as well...
	BOTGOAL_FOLLOW,		// Generic goal... Following a leader.
	BOTGOAL_OBJECTIVE,	// Generic goal... An objective item... (Gold, secret docs, etc)...
	BOTGOAL_CHECKPOINT,	// Generic goal... ET Checkpoint (flag).
} goalTypes_t;

typedef struct sod_level_info_s {
	char		*sodScript;				// fretn
	int			numWantedSnipers;
	int			numWantedPanzers;
	int			numWantedVenoms;
	int			numWantedFlamers;
	int			numWantedEngineers;
	int			numWantedMedics;
	int			numWantedLieutenants;
	int			objectives[MAX_OBJECTIVES];
	int			numObjectives;
	int			nextObjective;
	qboolean	flag;

} sod_level_info_t;
#endif //__BOT__

typedef struct {
	struct gclient_s	*clients;		// [maxclients]

	struct gentity_s	*gentities;
	int			gentitySize;
	int			num_entities;		// current number, <= MAX_GENTITIES

	int			warmupTime;			// restart match at this time

	fileHandle_t	logFile;

	char		rawmapname[MAX_QPATH];

	// store latched cvars here that we want to get at often
	int			maxclients;

	int			framenum;
	int			time;					// in msec
	int			overTime;
	int			previousTime;			// so movers can back up when blocked
	int			frameTime;				// Gordon: time the frame started, for antilag stuff

	int			startTime;				// level.time the map was started

	int			teamScores[TEAM_NUM_TEAMS];
	int			lastTeamLocationTime;		// last time of client team location update

	qboolean	newSession;				// don't use any old session data, because
										// we changed gametype

	qboolean	restarted;				// waiting for a map_restart to fire

	int			numConnectedClients;
	int			numNonSpectatorClients;	// includes connecting clients
	int			numPlayingClients;		// connected, non-spectators
	int			sortedClients[MAX_CLIENTS];		// sorted by score
	int			follow1, follow2;		// clientNums for auto-follow spectators

//	int			snd_fry;				// sound index for standing in lava

	int			warmupModificationCount;	// for detecting if g_warmup is changed

	voteInfo_t	voteInfo;

	int			numTeamClients[2];
	int			numVotingTeamClients[2];

	// spawn variables
	qboolean	spawning;				// the G_Spawn*() functions are valid
	int			numSpawnVars;
	char		*spawnVars[MAX_SPAWN_VARS][2];	// key / value pairs
	int			numSpawnVarChars;
	char		spawnVarChars[MAX_SPAWN_VARS_CHARS];

	// intermission state
	int			intermissionQueued;		// intermission was qualified, but
										// wait INTERMISSION_DELAY_TIME before
										// actually going there so the last
										// frag can be watched.  Disable future
										// kills during this delay
	int			intermissiontime;		// time the intermission was started
	char		*changemap;
	int			exitTime;
	vec3_t		intermission_origin;	// also used for spectator spawns
	vec3_t		intermission_angle;
	qboolean	lmsDoNextMap;			// should LMS do a map_restart or a vstr nextmap

	int			bodyQueIndex;			// dead bodies
	gentity_t	*bodyQue[BODY_QUEUE_SIZE];

	int			portalSequence;
	// Ridah
	char		*scriptAI;
	int			reloadPauseTime;		// don't think AI/client's until this time has elapsed
	int			reloadDelayTime;		// don't start loading the savegame until this has expired

	int			capturetimes[4]; // red, blue, none, spectator for WOLF_MP_CPH
	int			redReinforceTime, blueReinforceTime; // last time reinforcements arrived in ms
	int			redNumWaiting, blueNumWaiting; // number of reinforcements in queue
	vec3_t		spawntargets[MAX_MULTI_SPAWNTARGETS]; // coordinates of spawn targets
	int			numspawntargets; // # spawntargets in this map

	// RF, entity scripting
	char		*scriptEntity;

	// player/AI model scripting (server repository)
	animScriptData_t	animScriptData;

	int			totalHeadshots;
	int			missedHeadshots;
	qboolean	lastRestartTime;

	int			numFinalDead[2];		// DHM - Nerve :: unable to respawn and in limbo (per team)
	int			numOidTriggers;			// DHM - Nerve

	qboolean	latchGametype;			// DHM - Nerve

	// RF
	int			attackingTeam;			// which team is attacking
	int			explosiveTargets[2];	// attackers need to explode something to get through
	qboolean	captureFlagMode;
	qboolean	initStaticEnts;
	qboolean	initSeekCoverChains;
	char		*botScriptBuffer;
	int			globalAccumBuffer[MAX_SCRIPT_ACCUM_BUFFERS];

	int			soldierChargeTime[2];
	int			medicChargeTime[2];
	int			engineerChargeTime[2];
	int			lieutenantChargeTime[2];

	int			covertopsChargeTime[2];

	int			lastMapEntityUpdate;
	int			objectiveStatsAllies[MAX_OBJECTIVES];
	int			objectiveStatsAxis[MAX_OBJECTIVES];

	int			lastSystemMsgTime[2];

	float		soldierChargeTimeModifier[2];
	float		medicChargeTimeModifier[2];
	float		engineerChargeTimeModifier[2];
	float		lieutenantChargeTimeModifier[2];
	float		covertopsChargeTimeModifier[2];

	int			firstbloodTeam;
	int			teamEliminateTime;
	int			lmsWinningTeam;

	int			campaignCount;
	int			currentCampaign;
	qboolean	newCampaign;

	brushmodelInfo_t	brushModelInfo[128];
	int					numBrushModels;
	gentity_t	*gameManager;
 
    // record last time we loaded, so we can hack around sighting issues on reload
    int   lastLoadTime;

	qboolean doorAllowTeams;	// used by bots to decide whether or not to use team travel flags

	// Gordon: for multiplayer fireteams
	fireteamData_t	fireTeams[MAX_FIRETEAMS];

	qboolean	ccLayers;

	// OSP
	int			dwBlueReinfOffset;
	int			dwRedReinfOffset;
	qboolean	fLocalHost;
	qboolean	fResetStats;
	int			match_pause;				// Paused state of the match
	qboolean	ref_allready;				// Referee forced match start
	int			server_settings;
	int			sortedStats[MAX_CLIENTS];	// sorted by weapon stat
	int			timeCurrent;				// Real game clock
	int			timeDelta;					// Offset from internal clock - used to calculate real match time
	// OSP

	qboolean	mapcoordsValid, tracemapLoaded;
	vec3_t		mapcoordsMins, mapcoordsMaxs;

	char	tinfoAxis[1400];
	char	tinfoAllies[1400];

// Gordon: debris test
	int				numDebrisChunks;
	debrisChunk_t	debrisChunks[MAX_DEBRISCHUNKS];
// ===================

	qboolean	disableTankExit;
	qboolean	disableTankEnter;

	int			axisBombCounter, alliedBombCounter;
	// forty - arty/airstrike rate limiting
	int			axisArtyCounter, alliedArtyCounter;
	int			axisAutoSpawn, alliesAutoSpawn;
	int			axisMG42Counter, alliesMG42Counter;

	int			lastClientBotThink;

	limbo_cam_t	limboCams[MAX_LIMBO_CAMS];
	int			numLimboCams;

	int			numActiveAirstrikes[2];

	float		teamXP[SK_NUM_SKILLS][2];

	commanderTeamChat_t commanderSounds[2][MAX_COMMANDER_TEAM_SOUNDS];
	int					commanderSoundInterval[2];
	int					commanderLastSoundTime[2];

	qboolean	tempTraceIgnoreEnts[ MAX_GENTITIES ];

 	int ATB_holdoff;
 	int ATB_lastteam;
 	int nextBanner;
 	char forceCvars[MAX_FORCECVARS][2][MAX_CVAR_VALUE_STRING];
 	int forceCvarCount;
 
 	//forty - sudden death dyno
 	qboolean suddendeath;
 	qboolean testEndRound;
 	qboolean testEndRoundResult;
 
 	//perro - longest spree (count and player name) for this level
 	int maxspree_count;
 	char maxspree_player[MAX_STRING_CHARS];
 
 	// tjw: track CS usage
 	int csLen[MAX_CONFIGSTRINGS];
 	int csLenTotal;

#ifdef __BOT__
	sod_level_info_t	sodInfo;	// AIMod
#endif
	int		numSecrets;
	int		lastGrenadeKick;
} level_locals_t;

typedef struct {
	char		mapnames[MAX_MAPS_PER_CAMPAIGN][MAX_QPATH];
	//arenaInfo_t	arenas[MAX_MAPS_PER_CAMPAIGN];
	int			mapCount;
	int			current;

	char		shortname[256];
	char		next[256];
	int			typeBits;
} g_campaignInfo_t;

//
// g_spawn.c
//
#define		G_SpawnString(		key, def, out ) G_SpawnStringExt	( key, def, out, __FILE__, __LINE__ )
#define		G_SpawnFloat(		key, def, out ) G_SpawnFloatExt		( key, def, out, __FILE__, __LINE__ )
#define		G_SpawnInt(			key, def, out ) G_SpawnIntExt		( key, def, out, __FILE__, __LINE__ )
#define		G_SpawnVector(		key, def, out ) G_SpawnVectorExt	( key, def, out, __FILE__, __LINE__ )
#define		G_SpawnVector2D(	key, def, out ) G_SpawnVector2DExt	( key, def, out, __FILE__, __LINE__ )

qboolean	G_SpawnStringExt( const char *key, const char *defaultString, char **out, const char* file, int line );	// spawn string returns a temporary reference, you must CopyString() if you want to keep it
qboolean	G_SpawnFloatExt	( const char *key, const char *defaultString, float *out, const char* file, int line );
qboolean	G_SpawnIntExt	( const char *key, const char *defaultString, int *out, const char* file, int line );
qboolean	G_SpawnVectorExt( const char *key, const char *defaultString, float *out, const char* file, int line );
qboolean	G_SpawnVector2DExt( const char *key, const char *defaultString, float *out, const char* file, int line );
gentity_t *G_SpawnGEntityFromSpawnVars( void );

void		G_SpawnEntitiesFromString( void );
char *G_NewString( const char *string );
// Ridah
qboolean G_CallSpawn( gentity_t *ent );
// done.
char *G_AddSpawnVarToken( const char *string );
void G_ParseField( const char *key, const char *value, gentity_t *ent );

//
// g_cmds.c
//Josh: max words and word lengths for censor dictionary
#define DICT_MAX_WORDS 50
#define DICT_MAX_WORD_LENGTH 20
// Josh: word dictionary for censor
typedef struct {
	char	words[DICT_MAX_WORDS][DICT_MAX_WORD_LENGTH];
	int num_words;
} wordDictionary;
qboolean G_CensorText(char *text, wordDictionary *dictionary);
void G_PrivateMessage(gentity_t *ent);
void G_PlayDead(gentity_t *ent);
void G_TeamDamageStats(gentity_t *ent);
void Cmd_Score_f (gentity_t *ent);
void StopFollowing( gentity_t *ent );
//void BroadcastTeamChange( gclient_t *client, int oldTeam );
void G_TeamDataForString( const char* teamstr, int clientNum, team_t* team, spectatorState_t* sState, int* specClient );
qboolean SetTeam( gentity_t *ent, char *s, qboolean force, weapon_t w1, weapon_t w2, qboolean setweapons );
void G_SetClientWeapons( gentity_t* ent, weapon_t w1, weapon_t w2, qboolean updateclient );
void Cmd_FollowCycle_f( gentity_t *ent, int dir );
void Cmd_Kill_f( gentity_t *ent );
void Cmd_SwapPlacesWithBot_f( gentity_t *ent, int botNum );
void G_EntitySound( gentity_t *ent, const char *soundId, int volume );
void G_EntitySoundNoCut( gentity_t *ent, const char *soundId, int volume );
qboolean G_MatchOnePlayer(int *plist, char *err, int len);
int ClientNumbersFromString( char *s, int *plist );
int ClientNumberFromString( gentity_t *to, char *s );
void DecolorString( char *in, char *out );
void SanitizeString( char *in, char *out, qboolean fToLower );
void G_DropItems(gentity_t *self);
char *ConcatArgs( int start );
char *Q_SayConcatArgs(int start);
char *Q_AddCR(char *s);
int Q_SayArgc();
qboolean Q_SayArgv(int n, char *buffer, int bufferLength);

//
// g_items.c
//
void G_RunItem( gentity_t *ent );
void RespawnItem( gentity_t *ent );

void UseHoldableItem( gentity_t *ent, int item );
void PrecacheItem (gitem_t *it);
gentity_t *Drop_Item( gentity_t *ent, gitem_t *item, float angle, qboolean novelocity );
gentity_t *LaunchItem( gitem_t *item, vec3_t origin, vec3_t velocity, int ownerNum );
void SetRespawn (gentity_t *ent, float delay);
void G_SpawnItem (gentity_t *ent, gitem_t *item);
void FinishSpawningItem( gentity_t *ent );
void Think_Weapon (gentity_t *ent);
int ArmorIndex (gentity_t *ent);
void Fill_Clip (playerState_t *ps, int weapon);
int Add_Ammo (gentity_t *ent, int weapon, int count, qboolean fillClip);
void Touch_Item (gentity_t *ent, gentity_t *other, trace_t *trace);
qboolean AddMagicAmmo(gentity_t *receiver, int numOfClips);
weapon_t G_GetPrimaryWeaponForClient( gclient_t *client );
weapon_t G_GetSecondaryWeaponForClient( gclient_t *client );
void G_DropWeapon( gentity_t *ent, weapon_t weapon );
int G_ClassCount( gentity_t *ent, int playerType, team_t team);
qboolean G_IsClassFull( gentity_t *ent, int playerType, team_t team);

gentity_t *LaunchBinocs( gitem_t *item, vec3_t origin, vec3_t velocity, int ownerNum );
void G_DropBinocs( gentity_t *ent );
void G_BinocMasters(qboolean endOfMatch);
int QDECL G_SortPlayersByBinocs( const void *a, const void *b );

// Perro: Streakers (aka Killing Spree)
int QDECL G_SortPlayersByStreak( const void *a, const void *b );
void G_Streakers(qboolean endOfMatch);
void G_MapLongStreak(qboolean endOfMatch);

// Touch_Item_Auto is bound by the rules of autoactivation (if cg_autoactivate is 0, only touch on "activate")
void Touch_Item_Auto (gentity_t *ent, gentity_t *other, trace_t *trace);

void Prop_Break_Sound (gentity_t *ent);
void Spawn_Shard (gentity_t *ent, gentity_t *inflictor, int quantity, int type);

//
// g_utils.c
//
// Ridah
int G_FindConfigstringIndex( const char *name, int start, int max, qboolean create );
// done.
int		G_ModelIndex( char *name );
int		G_SoundIndex( const char *name );
int		G_SkinIndex( const char *name );
int		G_ShaderIndex( char *name );
int		G_CharacterIndex( const char *name );
int		G_StringIndex( const char* string );
qboolean G_AllowTeamsAllowed( gentity_t *ent, gentity_t *activator );
void	G_UseEntity( gentity_t *ent, gentity_t *other, gentity_t *activator );
qboolean G_IsWeaponDisabled( gentity_t* ent, weapon_t weapon, team_t team, qboolean quiet);
void	G_TeamCommand( team_t team, char *cmd );
void	G_KillBox (gentity_t *ent);
gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match);
gentity_t* G_FindByTargetname(gentity_t *from, const char* match);
gentity_t* G_FindByTargetnameFast(gentity_t *from, const char* match, int hash);
gentity_t *G_PickTarget (char *targetname);
void	G_UseTargets (gentity_t *ent, gentity_t *activator);
void	G_SetMovedir ( vec3_t angles, vec3_t movedir);

void	G_InitGentity( gentity_t *e );
int	G_GentitiesAvailable();
gentity_t	*G_Spawn (void);
gentity_t *G_TempEntity( vec3_t origin, int event );
gentity_t* G_PopupMessage( popupMessageType_t type );
void	G_Sound( gentity_t *ent, int soundIndex );
void	G_AnimScriptSound( int soundIndex, vec3_t org, int client );
void	G_FreeEntity( gentity_t *e );
//qboolean	G_EntitiesFree( void );

void	G_TouchTriggers (gentity_t *ent);
void	G_TouchSolids (gentity_t *ent);

float	*tv (float x, float y, float z);
char	*vtos( const vec3_t v );

void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm );
void G_AddEvent( gentity_t *ent, int event, int eventParm );
void G_SetOrigin( gentity_t *ent, vec3_t origin );
void AddRemap(const char *oldShader, const char *newShader, float timeOffset);
const char *BuildShaderStateConfig();
void G_SetAngle( gentity_t *ent, vec3_t angle );

qboolean infront (gentity_t *self, gentity_t *other);

void G_ProcessTagConnect(gentity_t *ent, qboolean clearAngles);

void G_SetEntState( gentity_t *ent, entState_t state );
void G_ParseCampaigns( void );
qboolean G_MapIsValidCampaignStartMap( void );

team_t G_GetTeamFromEntity( gentity_t *ent );

//
// g_combat.c
//
void G_AdjustedDamageVec( gentity_t *ent, vec3_t origin, vec3_t vec );
qboolean CanDamage (gentity_t *targ, vec3_t origin);
void G_Damage (gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod);
qboolean G_RadiusDamage (vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod);
qboolean etpro_RadiusDamage( vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod, qboolean clientsonly );
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath );
void TossClientItems( gentity_t *self );
gentity_t* G_BuildHead(gentity_t *ent);
gentity_t* G_BuildLeg(gentity_t *ent);
qboolean IsFFReflectable(int mod);

// damage flags
#define DAMAGE_RADIUS				0x00000001	// damage was indirect
#define DAMAGE_HALF_KNOCKBACK		0x00000002	// Gordon: do less knockback
#define DAMAGE_NO_KNOCKBACK			0x00000008	// do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION		0x00000020  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NO_TEAM_PROTECTION	0x00000010  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_DISTANCEFALLOFF		0x00000040	// distance falloff

//
// g_missile.c
//
void G_RunMissile( gentity_t *ent );
void G_RunBomb( gentity_t *ent );
int G_PredictMissile( gentity_t *ent, int duration, vec3_t endPos, qboolean allowBounce );
void G_TripMinePrime(gentity_t* ent);
qboolean G_HasDroppedItem(gentity_t* ent, int modType);

// Rafael zombiespit
void G_RunDebris( gentity_t *ent );

//DHM - Nerve :: server side flamethrower collision
void G_RunFlamechunk( gentity_t *ent );

//----(SA) removed unused q3a weapon firing
gentity_t *fire_flamechunk (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_tankflamechunk (gentity_t *self, vec3_t start, vec3_t dir);

gentity_t *fire_tankshell (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_grenade (gentity_t *self, vec3_t start, vec3_t aimdir, int grenadeWPID);
gentity_t *fire_rocket (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_speargun (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_tankptrs (gentity_t *self, vec3_t start, vec3_t dir);//Masteries, panzer 2

#define Fire_Lead( ent, activator, spread, damage, muzzle, forward, right, up ) Fire_Lead_Ext( ent, activator, spread, damage, muzzle, forward, right, up, MOD_MACHINEGUN )
void Fire_Lead_Ext( gentity_t *ent, gentity_t *activator, float spread, int damage, vec3_t muzzle, vec3_t forward, vec3_t right, vec3_t up, int mod );
void fire_lead (gentity_t *self,  vec3_t start, vec3_t dir, int damage);
qboolean visible (gentity_t *self, gentity_t *other);

gentity_t *fire_mortar (gentity_t *self, vec3_t start, vec3_t dir);
gentity_t *fire_flamebarrel (gentity_t *self, vec3_t start, vec3_t dir);
// done

//
// g_mover.c
//
gentity_t *G_TestEntityPosition( gentity_t *ent );
void G_RunMover( gentity_t *ent );
qboolean G_MoverPush( gentity_t *pusher, vec3_t move, vec3_t amove, gentity_t **obstacle );
void Use_BinaryMover( gentity_t *ent, gentity_t *other, gentity_t *activator );
void G_Activate( gentity_t *ent, gentity_t *activator );

void G_TryDoor(gentity_t *ent, gentity_t *other, gentity_t *activator);	//----(SA)	added

void InitMoverRotate ( gentity_t *ent );

void InitMover( gentity_t *ent );
void SetMoverState( gentity_t *ent, moverState_t moverState, int time );

void func_constructible_underconstructionthink( gentity_t *ent );

//
// g_tramcar.c
//
void Reached_Tramcar (gentity_t *ent);

//
// g_trigger.c
//
void Think_SetupObjectiveInfo( gentity_t *ent);

//
// g_misc.c
//
void TeleportPlayer( gentity_t *player, vec3_t origin, vec3_t angles );
void mg42_fire( gentity_t *other );
void mg42_stopusing( gentity_t *self );
void aagun_fire( gentity_t *other );


//
// g_weapon.c
//
qboolean AccuracyHit( gentity_t *target, gentity_t *attacker );
void CalcMuzzlePoint ( gentity_t *ent, int weapon, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint );
void SnapVectorTowards( vec3_t v, vec3_t to );
gentity_t *weapon_grenadelauncher_fire (gentity_t *ent, int grenadeWPID);
void G_PlaceTripmine(gentity_t* ent);
void G_FadeItems(gentity_t* ent, int modType);
gentity_t *G_FindSatchel(gentity_t* ent);
void G_ExplodeMines(gentity_t* ent);
qboolean G_ExplodeSatchels(gentity_t* ent);
void G_FreeSatchel( gentity_t* ent );
int G_GetWeaponDamage( int weapon );

void CalcMuzzlePoints(gentity_t *ent, int weapon);
void CalcMuzzlePointForActivate ( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint );
void Weapon_MagicAmmo_Ext(gentity_t *ent, 
						  vec3_t viewpos,
						  vec3_t tosspos,
						  vec3_t velocity);
void Weapon_Medic_Ext(gentity_t *ent, 
						  vec3_t viewpos,
						  vec3_t tosspos,
						  vec3_t velocity);

//
// g_client.c
//
team_t TeamCount( int ignoreClientNum, int team );			// NERVE - SMF - merge from team arena
team_t PickTeam( int ignoreClientNum );
void SetClientViewAngle( gentity_t *ent, vec3_t angle );
gentity_t *SelectSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles );
void respawn (gentity_t *ent);
void BeginIntermission (void);
void InitClientPersistant (gclient_t *client);
void InitClientResp (gclient_t *client);
void InitBodyQue (void);
void ClientSpawn( gentity_t *ent, qboolean revived, qboolean teamChange, qboolean restoreHealth);
void player_die (gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod);
void AddScore( gentity_t *ent, int score );
void AddKillScore( gentity_t *ent, int score );
void CalculateRanks( void );
qboolean SpotWouldTelefrag( gentity_t *spot );
qboolean G_CheckForExistingModelInfo( bg_playerclass_t* classInfo, const char *modelName, animModelInfo_t **modelInfo );
void G_StartPlayerAppropriateSound(gentity_t *ent, char* soundType);
void G_AddClassSpecificTools(gclient_t *client);
void SetWolfSpawnWeapons( gclient_t *client );
void limbo( gentity_t *ent, qboolean makeCorpse, qboolean headShot ); // JPW NERVE
void reinforce(gentity_t *ent); // JPW NERVE

//
// g_character.c
//

qboolean G_RegisterCharacter( const char *characterFile, bg_character_t *character );
void G_RegisterPlayerClasses( void );
//void G_SetCharacter( gclient_t *client, bg_character_t *character, qboolean custom );
void G_UpdateCharacter( gclient_t *client );

//
// g_svcmds.c
//
qboolean ConsoleCommand( void );
void G_ProcessIPBans(void);
qboolean G_FilterIPBanPacket( char *from );
qboolean G_FilterMaxLivesPacket (char *from);
qboolean G_FilterMaxLivesIPPacket( char *from );
void AddMaxLivesGUID( char *str );
void AddMaxLivesBan( const char *str );
void ClearMaxLivesBans();
void AddIPBan( const char *str );

void Svcmd_ShuffleTeams_f(void);


//
// g_weapon.c
//
void FireWeapon( gentity_t *ent );
void G_BurnMeGood( gentity_t *self, gentity_t *body, gentity_t *chunk );

//
// IsSilencedWeapon
//
// Description: Is the specified weapon a silenced weapon?
// Written: 12/26/2002
//
qboolean IsSilencedWeapon
(
	// The type of weapon in question.  Is it silenced?
	int weaponType
);


//
// p_hud.c
//
void MoveClientToIntermission (gentity_t *client);
void G_SetStats (gentity_t *ent);
void G_SendScore( gentity_t *client );

//
// g_cmds.c
//
void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message, qboolean localize ); // JPW NERVE removed static declaration so it would link
qboolean Cmd_CallVote_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue );
void Cmd_Follow_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue );
void Cmd_Say_f( gentity_t *ent, int mode, qboolean arg0 );
void Cmd_Team_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue );
void Cmd_SetWeapons_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue );
void Cmd_SetClass_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue );
void G_PlaySound_Cmd(void);
//
// g_pweapon.c
//


//
// g_main.c
//
void FindIntermissionPoint( void );
void G_RunThink (gentity_t *ent);
void QDECL G_LogPrintf( const char *fmt, ... )_attribute((format(printf,1,2)));
void SendScoreboardMessageToAllClients( void );
void QDECL G_Printf( const char *fmt, ... )_attribute((format(printf,1,2)));
void QDECL G_DPrintf( const char *fmt, ... )_attribute((format(printf,1,2)));
void QDECL G_Error( const char *fmt, ... )_attribute((format(printf,1,2)));
// Is this a single player type game - sp or coop?
qboolean G_IsSinglePlayerGame();
int G_EndRoundCountdown();
void LogExit( const char *string );

//
// g_client.c
//
char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot );
void ClientUserinfoChanged( int clientNum );
void ClientDisconnect( int clientNum );
void ClientBegin( int clientNum );
void ClientCommand( int clientNum );
float ClientHitboxMaxZ(gentity_t *hitEnt);

//
// g_active.c
//
void ClientThink( int clientNum );
void ClientEndFrame( gentity_t *ent );
void G_RunClient( gentity_t *ent );
qboolean ClientNeedsAmmo( int client );
qboolean ClientOutOfAmmo( int client );

// Does ent have enough "energy" to call artillery?
qboolean ReadyToCallArtillery(gentity_t* ent);
// to call airstrike?
qboolean ReadyToCallAirstrike(gentity_t* ent);
// to use smoke grenade?
qboolean ReadyToThrowSmoke(gentity_t *ent);
// Are we ready to construct?  Optionally, will also update the time while we are constructing
qboolean ReadyToConstruct(gentity_t *ent, gentity_t *constructible, qboolean updateState);
// tjw: move the player to other team if necessary



//
// g_team.c
//
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 );
int Team_ClassForString( char *string );
int QDECL G_SortPlayersByKillRating( const void *a, const void *b );
int QDECL G_SortPlayersByPlayerRating( const void *a, const void *b );
void G_ActiveTeamBalance();

//
// g_mem.c
//
void *G_Alloc( int size );
void G_InitMemory( void );
void Svcmd_GameMem_f( void );

//
// g_session.c
//
void G_ReadSessionData( gclient_t *client );
void G_InitSessionData( gclient_t *client, char *userinfo );

void G_InitWorldSession( void );
void G_WriteSessionData( qboolean restart );

void G_CalcRank( gclient_t* client );

//
// g_bot.c
//
void G_InitBots( qboolean restart );
char *G_GetBotInfoByNumber( int num );
char *G_GetBotInfoByName( const char *name );
void G_CheckBotSpawn( void );
void G_QueueBotBegin( int clientNum );
qboolean G_BotConnect( int clientNum, qboolean restart );
void Svcmd_AddBot_f( void );
void Svcmd_SpawnBot( );
void G_SpawnBot( const char *text );
int Bot_GetWeaponForClassAndTeam( int classNum, int teamNum, const char *weaponName );
void G_BotParseCharacterParms( char *characterFile, int *characterInt );

// ai_main.c
#define MAX_FILEPATH			144
int BotAIThinkFrame(int time);
void G_SetAASBlockingEntity( gentity_t *ent, int blocking );
qboolean BotSinglePlayer();
qboolean BotCoop();
//gentity_t *BotCheckBotGameEntity( gentity_t *ent );
gentity_t *BotFindEntity( gentity_t *from, int fieldofs, char *match );
void GetBotAmmoPct(int clientNum, int *ammoPct, int *ammoclipPct);
void BotCalculateMg42Spots(void);

// ai_dmq3.c
//returns the number of the client with the given name
int ClientFromName(char *name);
void BotMoveToIntermission( int client );
qboolean BotVisibleFromPos( vec3_t srcorigin, int srcnum, vec3_t destorigin, int destent, qboolean dummy );
qboolean BotCheckAttackAtPos(	int entnum, int enemy, vec3_t pos, qboolean ducking, qboolean allowHitWorld);

// ai_main.c
// TTimo - wraps to BotGetTargetExplosives, without dependency to bot_target_s (which breaks gcc build)
int GetTargetExplosives( team_t team, qboolean ignoreDynamite );

//bot settings
typedef struct bot_settings_s
{
	char characterfile[MAX_FILEPATH];
	float skill;
	char team[MAX_FILEPATH];
	int squadNum;		// xkan, 10/10/2002
} bot_settings_t;

int BotAISetup( int restart );
//void BotInitBotGameEntities(void);
int BotAIShutdown( int restart );
int BotAILoadMap( int restart );
int BotAISetupClient_ORIGINAL(int client, struct bot_settings_s *settings);
int BotAISetupClient(int client);
int BotAIShutdownClient( int client);
int BotAIStartFrame( int time );
void BotTestAAS(vec3_t origin);
void BotSetIdealViewAngles(int clientNum, vec3_t angle);


// g_cmd.c
void Cmd_Activate_f (gentity_t *ent);
void Cmd_Activate2_f (gentity_t *ent);
qboolean Do_Activate_f(gentity_t *ent, gentity_t *traceEnt);
gentity_t *G_LeaveTank( gentity_t* ent, qboolean position );


// Ridah

// g_save.c
#ifdef SAVEGAME_SUPPORT
qboolean G_SaveGame(char *username);
void G_LoadGame(void);
qboolean G_SavePersistant(char *nextmap);
void G_LoadPersistant(void);
#endif // SAVEGAME_SUPPORT

// g_script.c
void G_Script_ScriptParse( gentity_t *ent );
qboolean G_Script_ScriptRun( gentity_t *ent );
void G_Script_ScriptEvent( gentity_t *ent, char *eventStr, char *params );
void G_Script_ScriptLoad( void );
void G_Script_EventStringInit( void );

void mountedmg42_fire( gentity_t *other );
#ifdef __VEHICLES__
void tank_mgfire( gentity_t *ent );
void tank_fire( gentity_t *ent );
void flametank_fire( gentity_t *ent );
void ptrstank_fire( gentity_t *ent );//Masteries, for panzer 2
#endif
void script_mover_use(gentity_t *ent, gentity_t *other, gentity_t *activator);
void script_mover_blocked( gentity_t *ent, gentity_t *other );

float AngleDifference(float ang1, float ang2);

// g_props.c
void Props_Chair_Skyboxtouch (gentity_t *ent);

// ai_script.c
void Bot_ScriptLoad( void );
qboolean Bot_ScriptInitBot( int entnum );
void Bot_ScriptEvent( int entityNum, char *eventStr, char *params );

void Bot_TeamScriptEvent( int team, char *eventStr, char *params );


#include "g_team.h" // teamplay specific stuff

extern	level_locals_t	level;
extern	gentity_t		g_entities[];	//DAJ was explicit set to MAX_ENTITIES
extern g_campaignInfo_t g_campaigns[];
extern int				saveGamePending;

#define	FOFS(x) ((int)&(((gentity_t *)0)->x))

extern	vmCvar_t	g_gametype;

extern	vmCvar_t	g_log;
extern	vmCvar_t	g_dedicated;
extern	vmCvar_t	g_cheats;
extern	vmCvar_t	g_maxclients;			// allow this many total, including spectators
extern	vmCvar_t	g_maxGameClients;		// allow this many active
extern	vmCvar_t	g_minGameClients;		// NERVE - SMF - we need at least this many before match actually starts
extern	vmCvar_t	g_restarted;
extern	vmCvar_t	g_reset;

extern	vmCvar_t	g_fraglimit;
extern	vmCvar_t	g_timelimit;
extern	vmCvar_t	g_friendlyFire;
extern	vmCvar_t	g_password;
extern	vmCvar_t	sv_privatepassword;
extern	vmCvar_t	g_gravity;
extern	vmCvar_t	g_speed;
extern	vmCvar_t	g_knockback;
//extern	vmCvar_t	g_quadfactor;
extern	vmCvar_t	g_forcerespawn;
extern	vmCvar_t	g_inactivity;
#ifdef __DEBUGGING__
extern	vmCvar_t	g_debugMove;
extern	vmCvar_t	g_debugAlloc;
extern	vmCvar_t	g_debugDamage;
extern	vmCvar_t	g_debugBullets;	//----(SA)	added
#endif //__DEBUGGING__
#ifdef ALLOW_GSYNC
extern	vmCvar_t	g_synchronousClients;
#endif // ALLOW_GSYNC
extern	vmCvar_t	g_motd;
extern	vmCvar_t	g_warmup;
extern	vmCvar_t	voteFlags;

// DHM - Nerve :: The number of complaints allowed before kick/ban
extern	vmCvar_t	g_complaintlimit;
extern	vmCvar_t	g_ipcomplaintlimit;
extern	vmCvar_t	g_filtercams;
extern	vmCvar_t	g_maxlives;				// DHM - Nerve :: number of respawns allowed (0==infinite)
extern	vmCvar_t	g_maxlivesRespawnPenalty;
extern	vmCvar_t	g_voiceChatsAllowed;	// DHM - Nerve :: number before spam control
extern	vmCvar_t	g_alliedmaxlives;		// Xian
extern	vmCvar_t	g_axismaxlives;			// Xian
extern	vmCvar_t	g_fastres;				// Xian - Fast medic res'ing
extern	vmCvar_t	g_knifeonly;			// Xian - Wacky Knife-Only rounds
extern	vmCvar_t	g_enforcemaxlives;		// Xian - Temp ban with maxlives between rounds

extern	vmCvar_t	g_needpass;
extern	vmCvar_t	g_balancedteams;
extern	vmCvar_t	g_doWarmup;
extern	vmCvar_t	g_teamAutoJoin;
extern	vmCvar_t	g_teamForceBalance;
// josh: force player ratings averages per team to be close
extern	vmCvar_t	g_teamForceBalance_playerrating;
extern	vmCvar_t	g_banIPs;
extern	vmCvar_t	g_filterBan;
//extern	vmCvar_t	g_rankings;
extern	vmCvar_t	g_smoothClients;
extern	vmCvar_t	pmove_fixed;
extern	vmCvar_t	pmove_msec;

//Rafael
extern	vmCvar_t	g_scriptName;		// name of script file to run (instead of default for that map)

extern	vmCvar_t	g_scriptDebug;

extern	vmCvar_t	g_userAim;
extern	vmCvar_t	g_developer;

extern	vmCvar_t	g_footstepAudibleRange;
// JPW NERVE multiplayer
extern vmCvar_t		g_redlimbotime;
extern vmCvar_t		g_bluelimbotime;
extern vmCvar_t		g_medicChargeTime;
extern vmCvar_t		g_engineerChargeTime;
extern vmCvar_t		g_LTChargeTime;
extern vmCvar_t		g_soldierChargeTime;
// jpw

extern vmCvar_t		g_covertopsChargeTime;
#ifdef __DEBUGGING__
extern vmCvar_t		g_debugConstruct;
#endif //__DEBUGGING__
extern vmCvar_t		g_landminetimeout;

// What level of detail do we want script printing to go to.
extern vmCvar_t		g_scriptDebugLevel;

// How fast do SP player and allied bots move?
extern vmCvar_t		g_movespeed;

extern vmCvar_t g_axismapxp;
extern vmCvar_t g_alliedmapxp;

extern vmCvar_t g_oldCampaign;
extern vmCvar_t g_currentCampaign;
extern vmCvar_t g_currentCampaignMap;

// Arnout: for LMS
extern vmCvar_t g_axiswins;
extern vmCvar_t g_alliedwins;
extern vmCvar_t g_lms_teamForceBalance;
extern vmCvar_t g_lms_roundlimit;
extern vmCvar_t g_lms_matchlimit;
extern vmCvar_t g_lms_currentMatch;
extern vmCvar_t g_lms_lockTeams;
extern vmCvar_t g_lms_followTeamOnly;

#ifdef SAVEGAME_SUPPORT
extern	vmCvar_t	g_reloading;
#endif // SAVEGAME_SUPPORT

// NERVE - SMF
extern vmCvar_t		g_warmupLatch;
extern vmCvar_t		g_nextTimeLimit;
extern vmCvar_t		g_showHeadshotRatio;
extern vmCvar_t		g_userTimeLimit;
extern vmCvar_t		g_userAlliedRespawnTime;
extern vmCvar_t		g_userAxisRespawnTime;
extern vmCvar_t		g_currentRound;
extern vmCvar_t		g_noTeamSwitching;
extern vmCvar_t		g_altStopwatchMode;
extern vmCvar_t		g_gamestate;
extern vmCvar_t		g_swapteams;
// -NERVE - SMF

//Gordon
extern vmCvar_t		g_antilag;

// Jaqbuoss

extern vmCvar_t		g_tankSpawnDelay;

// OSP
extern vmCvar_t		refereePassword;
extern vmCvar_t		g_spectatorInactivity;
extern vmCvar_t		match_latejoin;
extern vmCvar_t		match_minplayers;
extern vmCvar_t		match_mutespecs;
extern vmCvar_t		match_readypercent;
extern vmCvar_t		match_timeoutcount;
extern vmCvar_t		match_timeoutlength;
extern vmCvar_t		match_warmupDamage;
extern vmCvar_t		server_autoconfig;
extern vmCvar_t		server_motd0;
extern vmCvar_t		server_motd1;
extern vmCvar_t		server_motd2;
extern vmCvar_t		server_motd3;
extern vmCvar_t		server_motd4;
extern vmCvar_t		server_motd5;
extern vmCvar_t		team_maxPanzers;
extern vmCvar_t		team_maxMortars;
extern vmCvar_t		team_maxMG42s;
extern vmCvar_t		team_maxFlamers;
extern vmCvar_t		team_maxGrenLaunchers;
extern vmCvar_t		team_maxplayers;
extern vmCvar_t		team_nocontrols;
//
// NOTE!!! If any vote flags are added, MAKE SURE to update the voteFlags struct in bg_misc.c w/appropriate info,
//         menudef.h for the mask and g_main.c for vote_allow_* flag updates
//
extern vmCvar_t		vote_allow_comp;
extern vmCvar_t		vote_allow_gametype;
extern vmCvar_t		vote_allow_kick;
extern vmCvar_t		vote_allow_map;
extern vmCvar_t		vote_allow_matchreset;
extern vmCvar_t		vote_allow_mutespecs;
extern vmCvar_t		vote_allow_nextmap;
extern vmCvar_t		vote_allow_pub;
extern vmCvar_t		vote_allow_referee;
extern vmCvar_t		vote_allow_shuffleteamsxp;
extern vmCvar_t		vote_allow_swapteams;
extern vmCvar_t		vote_allow_friendlyfire;
extern vmCvar_t		vote_allow_timelimit;
extern vmCvar_t		vote_allow_warmupdamage;
extern vmCvar_t		vote_allow_antilag;
extern vmCvar_t		vote_allow_balancedteams;
extern vmCvar_t		vote_allow_muting;
extern vmCvar_t		vote_allow_surrender;
extern vmCvar_t		vote_allow_restartcampaign;
extern vmCvar_t		vote_allow_nextcampaign;
extern vmCvar_t		vote_allow_poll;
extern vmCvar_t		vote_allow_maprestart;
extern vmCvar_t		vote_allow_shufflenorestart;
extern vmCvar_t		vote_limit;
extern vmCvar_t		vote_percent;
extern vmCvar_t		z_serverflags;
extern vmCvar_t		g_letterbox;
extern vmCvar_t		bot_enable;

#ifdef __DEBUGGING__
extern vmCvar_t		g_debugSkills;
#endif //__DEBUGGING__
extern vmCvar_t		g_heavyWeaponRestriction;
extern vmCvar_t		g_autoFireteams;

extern vmCvar_t		g_nextmap;
extern vmCvar_t		g_nextcampaign;

extern vmCvar_t		g_disableComplaints;

// tjw
extern vmCvar_t	g_shrubbot;
extern vmCvar_t	g_hitsounds; // see HSF_* defines
extern vmCvar_t	g_hitsound_default;
extern vmCvar_t	g_hitsound_helmet;
extern vmCvar_t	g_hitsound_head;
extern vmCvar_t	g_hitsound_team_warn_allies;
extern vmCvar_t	g_hitsound_team_warn_axis;
extern vmCvar_t	g_hitsound_team_helmet;
extern vmCvar_t	g_hitsound_team_head;
extern vmCvar_t	g_hitsound_team_default;
extern vmCvar_t	g_playDead;
extern vmCvar_t	g_shove;
extern vmCvar_t g_dragCorpse;
extern vmCvar_t g_classChange;
extern vmCvar_t g_forceLimboHealth;
extern vmCvar_t	g_privateMessages;
extern vmCvar_t	g_XPSave;
extern vmCvar_t	g_XPSaveFile;
extern vmCvar_t	g_XPSaveMaxAge_xp;
extern vmCvar_t	g_XPSaveMaxAge;
extern vmCvar_t g_weapons; // see WPF_* defines
extern vmCvar_t g_goomba;
extern vmCvar_t g_spawnInvul;
extern vmCvar_t g_spinCorpse;
extern vmCvar_t g_teamChangeKills;
// g_activeTeamBalance is going to be deprecated or removed
extern vmCvar_t g_activeTeamBalance;
extern vmCvar_t g_ATB;
extern vmCvar_t g_ATB_diff;
extern vmCvar_t g_ATB_minXP;
extern vmCvar_t g_ATB_alliedHoldoff;
extern vmCvar_t g_ATB_axisHoldoff;
extern vmCvar_t g_ATB_minPlayers;
extern vmCvar_t g_ATB_swap;
extern vmCvar_t g_ATB_rating; // see ATBR_* defines
extern vmCvar_t g_logAdmin;
extern vmCvar_t g_maxTeamLandmines;
extern vmCvar_t g_mapConfigs;
extern vmCvar_t g_packDistance;
extern vmCvar_t g_tossDistance;
extern vmCvar_t g_dropHealth;
extern vmCvar_t g_dropAmmo;
extern vmCvar_t g_intermissionTime;
extern vmCvar_t g_intermissionReadyPercent;
extern vmCvar_t g_hitboxes; // see HBF_* defines
extern vmCvar_t g_debugHitboxes;
extern vmCvar_t g_voting; // see VF_* defines
extern vmCvar_t g_moverScale;
extern vmCvar_t g_ammoCabinetTime;
extern vmCvar_t g_healthCabinetTime;
extern vmCvar_t g_spectator; // see SPECF_* defines
extern vmCvar_t g_maxWarp;
extern vmCvar_t g_dropObj;
extern vmCvar_t g_serverInfo; // see SIF_* defines
extern vmCvar_t g_goombaFlags; // see GBF_* defines
extern vmCvar_t g_shoveSound;
extern vmCvar_t g_shoveNoZ;
extern vmCvar_t g_stats; // see STATF_* defines
extern vmCvar_t g_poisonSound;
extern vmCvar_t g_poisonFlags; // see POISF_* defines
extern vmCvar_t g_tyranny;
extern vmCvar_t g_mapScriptDirectory;
extern vmCvar_t g_campaignFile;
extern vmCvar_t g_fightSound;
extern vmCvar_t g_fear;
extern vmCvar_t g_obituary;
extern vmCvar_t g_coverts;
extern vmCvar_t g_shortcuts;
extern vmCvar_t g_shuffle_rating;
extern vmCvar_t g_XPDecay;
extern vmCvar_t g_XPDecayRate;
extern vmCvar_t g_XPDecayFloor;
extern vmCvar_t g_maxXP;

// Josh
extern vmCvar_t g_logOptions;
extern vmCvar_t g_censor;
extern vmCvar_t g_censorNames;
extern vmCvar_t g_censorPenalty;
extern vmCvar_t g_censorMuteTime;
extern vmCvar_t g_skills;
extern vmCvar_t g_misc; // see MISC_* defines (some in bg_public.h)
extern vmCvar_t g_doubleJumpHeight;
extern vmCvar_t g_killRating;
extern vmCvar_t g_playerRating;
// Amount of wins to add to each side to stabilize mean early on
extern vmCvar_t g_playerRating_mapPad;
// minimum # of players to count the map towards rating
extern vmCvar_t g_playerRating_minplayers;
// unlagged server options
extern	vmCvar_t	sv_fps;
extern	vmCvar_t	g_skipCorrection;
extern	vmCvar_t	g_truePing;
// unlagged server options

// dvl
extern vmCvar_t g_slashKill;

// Michael

// for changing amount of xp needed to level up each skill
extern vmCvar_t g_skillSoldier;
extern vmCvar_t g_skillMedic;
extern vmCvar_t g_skillEngineer;
extern vmCvar_t g_skillFieldOps;
extern vmCvar_t g_skillCovertOps;
extern vmCvar_t g_skillBattleSense;
extern vmCvar_t g_skillLightWeapons;

// matt
extern vmCvar_t g_teamDamageRestriction;
extern vmCvar_t g_minHits;
extern vmCvar_t g_autoTempBan;
extern vmCvar_t g_autoTempBanTime;
extern vmCvar_t g_poison;
extern vmCvar_t g_medics;
extern vmCvar_t g_alliedSpawnInvul;
extern vmCvar_t g_axisSpawnInvul;
extern vmCvar_t g_msgs;
extern vmCvar_t g_msgpos;
extern vmCvar_t g_throwableKnives;
extern vmCvar_t g_maxKnives;
extern vmCvar_t g_knifeDamage;
extern vmCvar_t g_throwKnifeWait;

extern vmCvar_t team_maxMedics;
extern vmCvar_t team_maxEngineers;
extern vmCvar_t team_maxFieldOps;
extern vmCvar_t team_maxCovertOps;

// forty - arty/airstrike rate limiting
extern vmCvar_t	g_minAirstrikeTime;
extern vmCvar_t	g_minArtyTime;

// forty - constructible xp sharing
extern vmCvar_t g_constructiblexpsharing;

// forty - airstrike block
extern vmCvar_t g_asblock;

// - forty - realistic hitboxes
extern vmCvar_t g_realHead;

// gabriel - medic health regeneration
extern vmCvar_t g_medicHealthRegen;

// gabriel - party panzers
extern vmCvar_t g_partyPanzersEnabled;
extern vmCvar_t g_partyPanzersPattern;
extern vmCvar_t g_partyPanzersDamageMode;

// gabriel - panzer modifiers
extern vmCvar_t g_panzersVulnerable;
extern vmCvar_t g_panzersSpeed;
extern vmCvar_t g_panzersGravity;

// forty - sudden death dyno and friends
extern vmCvar_t g_dyno;

// forty - canister kicking
extern vmCvar_t g_canisterKick;

// perro - killing spree
extern vmCvar_t g_killingSpree;
extern vmCvar_t g_killingSpreePos;
extern vmCvar_t g_killingSpreeMsg_tk;
extern vmCvar_t g_killingSpreeMsg_end;
extern vmCvar_t g_killingSpreeMsg_futility;
extern vmCvar_t g_killingSpreeMsg_default;

// perro - tunable weapons
extern vmCvar_t g_dmg;
extern vmCvar_t g_dmgKnife;
extern vmCvar_t g_dmgSten;
extern vmCvar_t g_dmgInfRifle;
extern vmCvar_t g_dmgFG42;
extern vmCvar_t g_dmgPistol;
extern vmCvar_t g_dmgSMG;
extern vmCvar_t g_dmgMG42;
extern vmCvar_t g_dmgPTRS;//Masteries
extern vmCvar_t g_dmgFG42Scope;
extern vmCvar_t g_dmgSniper;
extern vmCvar_t g_dmgGrenade;
extern vmCvar_t g_dmgGrenadeRadius;
extern vmCvar_t g_dmgGLauncher;
extern vmCvar_t g_dmgGLauncherRadius;
extern vmCvar_t g_dmgLandmine;
extern vmCvar_t g_dmgLandmineRadius;
extern vmCvar_t g_dmgSatchel;
extern vmCvar_t g_dmgSatchelRadius;
extern vmCvar_t g_dmgPanzer;
extern vmCvar_t g_dmgPanzerRadius;
extern vmCvar_t g_dmgMortar;
extern vmCvar_t g_dmgMortarRadius;
extern vmCvar_t g_dmgDynamite;
extern vmCvar_t g_dmgDynamiteRadius;
extern vmCvar_t g_dmgArty;
extern vmCvar_t g_dmgArtyRadius;
extern vmCvar_t g_dmgAir;
extern vmCvar_t g_dmgAirRadius;
extern vmCvar_t g_dmgMG;
extern vmCvar_t g_dmgFlamer;
extern vmCvar_t g_dmgHeadShotMin;
extern vmCvar_t g_dmgHeadShotRatio;
// Perro - stamina
extern vmCvar_t g_staminaRecharge;
// Perro - reflect friendly fire
extern vmCvar_t g_reflectFriendlyFire;
extern vmCvar_t g_reflectFFWeapons;
extern vmCvar_t g_friendlyFireOpts;
// Perro - damage-based XP
extern vmCvar_t g_damageXP;
extern vmCvar_t g_damageXPLevel;

extern vmCvar_t	bot_debug;					// if set, draw "thought bubbles" for crosshair-selected bot
extern vmCvar_t bot_min_snipe_time;			// minimum time between bot switching away from sniper zoom...
extern vmCvar_t	bot_debug_curAINode;		// the text of the current ainode for the bot begin debugged
extern vmCvar_t	bot_debug_alertState;		// alert state of the bot being debugged
extern vmCvar_t	bot_debug_pos;				// coords of the bot being debugged
extern vmCvar_t	bot_debug_weaponAutonomy;	// weapon autonomy of the bot being debugged
extern vmCvar_t	bot_debug_movementAutonomy;	// movement autonomy of the bot being debugged
extern vmCvar_t	bot_debug_cover_spot;		// What cover spot are we going to?
extern vmCvar_t	bot_debug_anim;				// what animation is the bot playing?

extern vmCvar_t g_camo_axis;				// jaquboss , cammo number
extern vmCvar_t g_camo_allies;				// jaquboss , cammo number
extern vmCvar_t g_camo_allies_default;
extern vmCvar_t g_camo_axis_default;

extern vmCvar_t		g_realism;


void	trap_Printf( const char *fmt );
void	trap_Error( const char *fmt );
int		trap_Milliseconds( void );
int		trap_Argc( void );
void	trap_Argv( int n, char *buffer, int bufferLength );
void	trap_Args( char *buffer, int bufferLength );
int		trap_FS_Write( const void *buffer, int len, fileHandle_t f );
int		trap_FS_Rename( const char *from, const char *to );
int		trap_FS_GetFileList( const char *path, const char *extension, char *listbuf, int bufsize );
void	trap_SendConsoleCommand( int exec_when, const char *text );
void	trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags );
void	trap_Cvar_Update( vmCvar_t *cvar );
void	trap_Cvar_Set( const char *var_name, const char *value );
int		trap_Cvar_VariableIntegerValue( const char *var_name );
float	trap_Cvar_VariableValue( const char *var_name );
void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_Cvar_LatchedVariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *gameClients, int sizeofGameClient );
void	trap_DropClient( int clientNum, const char *reason, int length );
void	trap_SendServerCommand( int clientNum, const char *text );
void	trap_SetConfigstring( int num, const char *string );
void	trap_GetConfigstring( int num, char *buffer, int bufferSize );
void	trap_GetUserinfo( int num, char *buffer, int bufferSize );
void	trap_SetUserinfo( int num, const char *buffer );
void	trap_GetServerinfo( char *buffer, int bufferSize );
void	trap_SetBrushModel( gentity_t *ent, const char *name );
void	trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void	trap_TraceCapsule( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void	trap_TraceCapsuleNoEnts( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void	trap_TraceNoEnts( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
int		trap_PointContents( const vec3_t point, int passEntityNum );
qboolean trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 );
void	trap_AdjustAreaPortalState( gentity_t *ent, qboolean open );
qboolean trap_AreasConnected( int area1, int area2 );
void	trap_LinkEntity( gentity_t *ent );
void	trap_UnlinkEntity( gentity_t *ent );
int		trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount );
qboolean trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
qboolean trap_EntityContactCapsule( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
int		trap_BotAllocateClient( int clientNum );
void	trap_BotFreeClient( int clientNum );
void	trap_GetUsercmd( int clientNum, usercmd_t *cmd );
qboolean	trap_GetEntityToken( char *buffer, int bufferSize );
qboolean trap_GetTag( int clientNum, int tagFileNumber, char *tagName, orientation_t *or );
qboolean trap_LoadTag( const char* filename );

int		trap_RealTime( qtime_t *qtime );

int		trap_DebugPolygonCreate(int color, int numPoints, vec3_t *points);
void	trap_DebugPolygonDelete(int id);

int		trap_BotLibSetup( void );
int		trap_BotLibShutdown( void );
int		trap_BotLibVarSet(char *var_name, char *value);
int		trap_BotLibVarGet(char *var_name, char *value, int size);
int		trap_BotLibDefine(char *string);
int		trap_BotLibStartFrame(float time);
int		trap_BotLibLoadMap(const char *mapname);
int		trap_BotLibUpdateEntity(int ent, void /* struct bot_updateentity_s */ *bue);
int		trap_BotLibTest(int parm0, char *parm1, vec3_t parm2, vec3_t parm3);

int		trap_BotGetSnapshotEntity( int clientNum, int sequence );
int		trap_BotGetServerCommand(int clientNum, char *message, int size);
//int		trap_BotGetConsoleMessage(int clientNum, char *message, int size);
void	trap_BotUserCommand(int client, usercmd_t *ucmd);

void		trap_AAS_EntityInfo(int entnum, void /* struct aas_entityinfo_s */ *info);

int			trap_AAS_Initialized(void);
void		trap_AAS_PresenceTypeBoundingBox(int presencetype, vec3_t mins, vec3_t maxs);
float		trap_AAS_Time(void);

// Ridah
void		trap_AAS_SetCurrentWorld(int index);
// done.

int			trap_AAS_PointAreaNum(vec3_t point);
int			trap_AAS_TraceAreas(vec3_t start, vec3_t end, int *areas, vec3_t *points, int maxareas);
int			trap_AAS_BBoxAreas(vec3_t absmins, vec3_t absmaxs, int *areas, int maxareas);
void		trap_AAS_AreaCenter(int areanum, vec3_t center);
qboolean	trap_AAS_AreaWaypoint(int areanum, vec3_t center);

int			trap_AAS_PointContents(vec3_t point);
int			trap_AAS_NextBSPEntity(int ent);
int			trap_AAS_ValueForBSPEpairKey(int ent, char *key, char *value, int size);
int			trap_AAS_VectorForBSPEpairKey(int ent, char *key, vec3_t v);
int			trap_AAS_FloatForBSPEpairKey(int ent, char *key, float *value);
int			trap_AAS_IntForBSPEpairKey(int ent, char *key, int *value);

int			trap_AAS_AreaReachability(int areanum);
int			trap_AAS_AreaLadder(int areanum);

int			trap_AAS_AreaTravelTimeToGoalArea(int areanum, vec3_t origin, int goalareanum, int travelflags);

int			trap_AAS_Swimming(vec3_t origin);
int			trap_AAS_PredictClientMovement(void /* aas_clientmove_s */ *move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize);

// Ridah, route-tables
void		trap_AAS_RT_ShowRoute( vec3_t srcpos, int	srcnum, int destnum );
int			trap_AAS_NearestHideArea(int srcnum, vec3_t origin, int areanum, int enemynum, vec3_t enemyorigin, int enemyareanum, int travelflags, float maxdist, vec3_t distpos);
int			trap_AAS_ListAreasInRange(vec3_t srcpos, int srcarea, float range, int travelflags, float **outareas, int maxareas);
int			trap_AAS_AvoidDangerArea(vec3_t srcpos, int srcarea, vec3_t dangerpos, int dangerarea, float range, int travelflags);
int			trap_AAS_Retreat
(
	// Locations of the danger spots (AAS area numbers)
	int *dangerSpots,
	// The number of danger spots
	int dangerSpotCount,
	vec3_t srcpos, 
	int srcarea, 
	vec3_t dangerpos, 
	int dangerarea, 
	// Min range from startpos
	float range, 
	// Min range from danger
	float dangerRange,
	int travelflags
);
int			trap_AAS_AlternativeRouteGoals(vec3_t start, vec3_t goal, int travelflags,
										 aas_altroutegoal_t *altroutegoals, int maxaltroutegoals,
										 int color);
void		trap_AAS_SetAASBlockingEntity( vec3_t absmin, vec3_t absmax, int blocking );
void		trap_AAS_RecordTeamDeathArea( vec3_t srcpos, int srcarea, int team, int teamCount, int travelflags );
// done.

void	trap_EA_Say(int client, char *str);
void	trap_EA_SayTeam(int client, char *str);
void	trap_EA_UseItem(int client, char *it);
void	trap_EA_DropItem(int client, char *it);
void	trap_EA_UseInv(int client, char *inv);
void	trap_EA_DropInv(int client, char *inv);
void	trap_EA_Gesture(int client);
void	trap_EA_Command(int client, char *command);

void	trap_EA_SelectWeapon(int client, int weapon);
void	trap_EA_Talk(int client);
void	trap_EA_Attack(int client);
void	trap_EA_Reload(int client);
void	trap_EA_Activate(int client);
void	trap_EA_Respawn(int client);
void	trap_EA_Jump(int client);
void	trap_EA_DelayedJump(int client);
void	trap_EA_Crouch(int client);
void	trap_EA_Walk(int client);
void	trap_EA_MoveUp(int client);
void	trap_EA_MoveDown(int client);
void	trap_EA_MoveForward(int client);
void	trap_EA_MoveBack(int client);
void	trap_EA_MoveLeft(int client);
void	trap_EA_MoveRight(int client);
void	trap_EA_Move(int client, vec3_t dir, float speed);
void	trap_EA_View(int client, vec3_t viewangles);
void	trap_EA_Prone(int client);

void	trap_EA_EndRegular(int client, float thinktime);
void	trap_EA_GetInput(int client, float thinktime, void /* struct bot_input_s */ *input);
void	trap_EA_ResetInput(int client, void *init);


int		trap_BotLoadCharacter(char *charfile, int skill);
void	trap_BotFreeCharacter(int character);
float	trap_Characteristic_Float(int character, int index);
float	trap_Characteristic_BFloat(int character, int index, float min, float max);
int		trap_Characteristic_Integer(int character, int index);
int		trap_Characteristic_BInteger(int character, int index, int min, int max);
void	trap_Characteristic_String(int character, int index, char *buf, int size);

int		trap_BotAllocChatState(void);
void	trap_BotFreeChatState(int handle);
void	trap_BotQueueConsoleMessage(int chatstate, int type, char *message);
void	trap_BotRemoveConsoleMessage(int chatstate, int handle);
int		trap_BotNextConsoleMessage(int chatstate, void /* struct bot_consolemessage_s */ *cm);
int		trap_BotNumConsoleMessages(int chatstate);
void	trap_BotInitialChat(int chatstate, char *type, int mcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 );
int		trap_BotNumInitialChats(int chatstate, char *type);
int		trap_BotReplyChat(int chatstate, char *message, int mcontext, int vcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 );
int		trap_BotChatLength(int chatstate);
void	trap_BotEnterChat(int chatstate, int client, int sendto);
void	trap_BotGetChatMessage(int chatstate, char *buf, int size);
int		trap_StringContains(char *str1, char *str2, int casesensitive);
int		trap_BotFindMatch(char *str, void /* struct bot_match_s */ *match, unsigned long int context);
void	trap_BotMatchVariable(void /* struct bot_match_s */ *match, int variable, char *buf, int size);
void	trap_UnifyWhiteSpaces(char *string);
void	trap_BotReplaceSynonyms(char *string, unsigned long int context);
int		trap_BotLoadChatFile(int chatstate, char *chatfile, char *chatname);
void	trap_BotSetChatGender(int chatstate, int gender);
void	trap_BotSetChatName(int chatstate, char *name);
void	trap_BotResetGoalState(int goalstate);
void	trap_BotRemoveFromAvoidGoals(int goalstate, int number);
void	trap_BotResetAvoidGoals(int goalstate);
void	trap_BotPushGoal(int goalstate, void /* struct bot_goal_s */ *goal);
void	trap_BotPopGoal(int goalstate);
void	trap_BotEmptyGoalStack(int goalstate);
void	trap_BotDumpAvoidGoals(int goalstate);
void	trap_BotDumpGoalStack(int goalstate);
void	trap_BotGoalName(int number, char *name, int size);
int		trap_BotGetTopGoal(int goalstate, void /* struct bot_goal_s */ *goal);
int		trap_BotGetSecondGoal(int goalstate, void /* struct bot_goal_s */ *goal);
int		trap_BotChooseLTGItem(int goalstate, vec3_t origin, int *inventory, int travelflags);
int		trap_BotChooseNBGItem(int goalstate, vec3_t origin, int *inventory, int travelflags, void /* struct bot_goal_s */ *ltg, float maxtime);
int		trap_BotTouchingGoal(vec3_t origin, void /* struct bot_goal_s */ *goal);
int		trap_BotItemGoalInVisButNotVisible(int viewer, vec3_t eye, vec3_t viewangles, void /* struct bot_goal_s */ *goal);
int		trap_BotGetNextCampSpotGoal(int num, void /* struct bot_goal_s */ *goal);
int		trap_BotGetMapLocationGoal(char *name, void /* struct bot_goal_s */ *goal);
int		trap_BotGetLevelItemGoal(int index, char *classname, void /* struct bot_goal_s */ *goal);
float	trap_BotAvoidGoalTime(int goalstate, int number);
void	trap_BotInitLevelItems(void);
void	trap_BotUpdateEntityItems(void);
int		trap_BotLoadItemWeights(int goalstate, char *filename);
void	trap_BotFreeItemWeights(int goalstate);
void	trap_BotInterbreedGoalFuzzyLogic(int parent1, int parent2, int child);
void	trap_BotSaveGoalFuzzyLogic(int goalstate, char *filename);
void	trap_BotMutateGoalFuzzyLogic(int goalstate, float range);
int		trap_BotAllocGoalState(int state);
void	trap_BotFreeGoalState(int handle);

void	trap_BotResetMoveState(int movestate);
void	trap_BotMoveToGoal(void /* struct bot_moveresult_s */ *result, int movestate, void /* struct bot_goal_s */ *goal, int travelflags);
int		trap_BotMoveInDirection(int movestate, vec3_t dir, float speed, int type);
void	trap_BotResetAvoidReach(int movestate);
void	trap_BotResetLastAvoidReach(int movestate);
int		trap_BotReachabilityArea(vec3_t origin, int testground);
int		trap_BotMovementViewTarget(int movestate, void /* struct bot_goal_s */ *goal, int travelflags, float lookahead, vec3_t target);
int		trap_BotPredictVisiblePosition(vec3_t origin, int areanum, void /* struct bot_goal_s */ *goal, int travelflags, vec3_t target);
int		trap_BotAllocMoveState(void);
void	trap_BotFreeMoveState(int handle);
void	trap_BotInitMoveState(int handle, void /* struct bot_initmove_s */ *initmove);
// Ridah
void	trap_BotInitAvoidReach(int handle);
// done.

int		trap_BotChooseBestFightWeapon(int weaponstate, int *inventory);
void	trap_BotGetWeaponInfo(int weaponstate, int weapon, void /* struct weaponinfo_s */ *weaponinfo);
int		trap_BotLoadWeaponWeights(int weaponstate, char *filename);
int		trap_BotAllocWeaponState(void);
void	trap_BotFreeWeaponState(int weaponstate);
void	trap_BotResetWeaponState(int weaponstate);

int		trap_GeneticParentsAndChildSelection(int numranks, float *ranks, int *parent1, int *parent2, int *child);

void	trap_SnapVector( float *v );

void		trap_SendMessage( int clientNum, char *buf, int buflen );
messageStatus_t	trap_MessageStatus( int clientNum );

void G_ExplodeMissile( gentity_t *ent );

void Svcmd_StartMatch_f(void);
void Svcmd_ResetMatch_f(qboolean fDoReset, qboolean fDoRestart);
void Svcmd_SwapTeams_f(void);

void trap_PbStat ( int clientNum , char *category , char *values ) ;

// g_antilag.c
void G_StoreClientPosition( gentity_t* ent );
void G_AdjustClientPositions( gentity_t* ent, int time, qboolean forward);
void G_ResetMarkers( gentity_t* ent );
void G_HistoricalTrace( gentity_t* ent, trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void G_HistoricalTraceBegin( gentity_t *ent );
void G_HistoricalTraceEnd( gentity_t *ent );
void G_Trace( gentity_t* ent, trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void G_AdjustSingleClientPosition( gentity_t* ent, int time, gentity_t* debugger);
void G_ReAdjustSingleClientPosition( gentity_t* ent );
void G_PredictPmove(gentity_t *ent, float frametime);

#define BODY_VALUE(ENT) ENT->watertype
#define BODY_TEAM(ENT) ENT->s.modelindex
#define BODY_CLASS(ENT) ENT->s.modelindex2
#define BODY_CHARACTER(ENT) ENT->s.onFireStart
#define BODY_WEAPON(ENT) ENT->s.nextWeapon

//g_buddy_list.c

#define MAX_FIRE_TEAMS 8

typedef struct {
	char name[32];
	char clientbits[8];
	char requests[8];
	int leader;
	qboolean open;
	qboolean valid;
} fireteam_t;

void Cmd_FireTeam_MP_f(gentity_t* ent);
int G_IsOnAFireTeam(int clientNum);
qboolean G_IsOnFireteam(int entityNum, fireteamData_t** teamNum);

/*
void G_SetWayPoint( gentity_t* ent, wayPointType_t wayPointType, vec3_t loc );
void G_RemoveWayPoint( gclient_t *client );
*/

void G_RemoveFromAllIgnoreLists( int clientNum );


//g_teammapdata.c

typedef struct mapEntityData_s {
	vec3_t			org;
	int				yaw;
	int				data;
	char			type;
	int				startTime;
	int				singleClient;

	int				status;
	int				entNum;
	struct mapEntityData_s *next, *prev;
} mapEntityData_t;

typedef struct mapEntityData_Team_s {
	mapEntityData_t mapEntityData_Team[MAX_GENTITIES];
	mapEntityData_t *freeMapEntityData;					// single linked list
	mapEntityData_t activeMapEntityData;				// double linked list
} mapEntityData_Team_t;

extern mapEntityData_Team_t mapEntityData[2];

void G_InitMapEntityData( mapEntityData_Team_t *teamList );
mapEntityData_t *G_FreeMapEntityData( mapEntityData_Team_t *teamList, mapEntityData_t *mEnt );
mapEntityData_t *G_AllocMapEntityData( mapEntityData_Team_t *teamList );
mapEntityData_t *G_FindMapEntityData( mapEntityData_Team_t *teamList, int entNum );
mapEntityData_t *G_FindMapEntityDataSingleClient( mapEntityData_Team_t *teamList, mapEntityData_t *start, int entNum, int clientNum );

void G_ResetTeamMapData();
void G_UpdateTeamMapData();

void G_SetupFrustum( gentity_t* ent );
void G_SetupFrustum_ForBinoculars( gentity_t* ent );
qboolean G_VisibleFromBinoculars ( gentity_t* viewer, gentity_t* ent, vec3_t origin );

void G_LogTeamKill(		gentity_t* ent,	weapon_t weap );
void G_LogDeath(		gentity_t* ent,	weapon_t weap );
void G_LogKill(			gentity_t* ent,	weapon_t weap );
void G_LogRegionHit(	gentity_t* ent, hitRegion_t hr );
//void G_SetPlayerRank(	gentity_t* ent );
//void G_AddExperience(	gentity_t* ent, float exp );

// Skills
void G_ResetXP(gentity_t *ent);
void G_SetPlayerScore( gclient_t *client );
void G_SetPlayerSkill( gclient_t *client, skillType_t skill );
void G_AddSkillPoints( gentity_t *ent, skillType_t skill, float points );
void G_LoseSkillPoints( gentity_t *ent, skillType_t skill, float points );
void G_XPDecay(gentity_t *ent, int seconds, qboolean force);
void G_AddDamageXP ( gentity_t *attacker, meansOfDeath_t mod);
void G_AddKillSkillPoints( gentity_t *attacker, meansOfDeath_t mod, hitRegion_t hr, qboolean splash );
void G_AddKillSkillPointsForDestruction( gentity_t *attacker, meansOfDeath_t mod, g_constructible_stats_t *constructibleStats );
void G_LoseKillSkillPoints( gentity_t *tker, meansOfDeath_t mod, hitRegion_t hr, qboolean splash );

void G_DebugOpenSkillLog( void );
void G_DebugCloseSkillLog( void );
void G_DebugAddSkillLevel( gentity_t *ent, skillType_t skill );
void G_DebugAddSkillPoints( gentity_t *ent, skillType_t skill, float points, const char *reason );
void G_ReassignSkillLevel( skillType_t );

typedef enum {
	SM_NEED_MEDIC,
	SM_NEED_ENGINEER,
	SM_NEED_LT,
	SM_NEED_COVERTOPS,
	SM_LOST_MEN,
	SM_OBJ_CAPTURED,
	SM_OBJ_LOST,
	SM_OBJ_DESTROYED,
	SM_CON_COMPLETED,
	SM_CON_FAILED,
	SM_CON_DESTROYED,
	SM_NUM_SYS_MSGS,
} sysMsg_t;

void G_CheckForNeededClasses( void );
void G_CheckMenDown( void );
void G_SendMapEntityInfo( gentity_t* e );
void G_SendSystemMessage( sysMsg_t message, int team );
int G_GetSysMessageNumber( const char* sysMsg );
int G_CountTeamLandmines ( team_t team );
qboolean G_SweepForLandmines( vec3_t origin, float radius, int team );

void G_AddClientToFireteam( int entityNum, int leaderNum );
void G_InviteToFireTeam( int entityNum, int otherEntityNum );
void GetBotAmmo(int clientNum, int *weapon, int *ammo, int *ammoclip);
void G_UpdateFireteamConfigString(fireteamData_t* ft);
void G_RemoveClientFromFireteams( int entityNum, qboolean update, qboolean print );

void G_PrintClientSpammyCenterPrint(int entityNum, char* text);

void aagun_fire( gentity_t *other );

// TAT 11/13/2002
//		Server only entities

struct g_serverEntity_s
{
	qboolean	inuse;

	char		*classname;			// set in QuakeEd
	int			spawnflags;			// set in QuakeEd

	// our parent entity
	g_serverEntity_t	*parent;

	// pointer to the next server entity
	g_serverEntity_t	*nextServerEntity;
	g_serverEntity_t	*target_ent;
	g_serverEntity_t	*chain;

	char		*name;
	char		*target;

	vec3_t		origin;				// Let's try keeping this simple, and just having an origin
	vec3_t		angles;				// facing angle (for a seek cover spot)
	int			number;				// identifier for this entity
	int			team;				// which team?  seek cover spots need this
	int			areaNum;			// This thing doesn't move, so we should only need to calc the areanum once
	int			botIgnoreTime;		// So that multiple bots don't use the same seek cover spot

	void		(*setup)(g_serverEntity_t *self);		// Setup function called once after all server objects are created
};

// clear out all the sp entities
void InitServerEntities(void);
// get the server entity with the passed in number
g_serverEntity_t *GetServerEntity(int num);
// Give a gentity_t, create a sp entity, copy all pertinent data, and return it
g_serverEntity_t *CreateServerEntity(gentity_t *ent);
// These server entities don't get to update every frame, but some of them have to set themselves up
//		after they've all been created
//		So we want to give each entity the chance to set itself up after it has been created
void InitialServerEntitySetup();
// Like G_Find, but for server entities
g_serverEntity_t *FindServerEntity( g_serverEntity_t *from, int fieldofs, char *match );


#define	SE_FOFS(x) ((int)&(((g_serverEntity_t *)0)->x))


// Match settings
#define PAUSE_NONE		0x00	// Match is NOT paused.
#define PAUSE_UNPAUSING	0x01	// Pause is about to expire

// HRESULTS
#define G_OK			0
#define G_INVALID		-1
#define G_NOTFOUND	-2


#define AP(x) trap_SendServerCommand(-1, x)					// Print to all
#define CP(x) trap_SendServerCommand(ent-g_entities, x)		// Print to an ent
#define CPx(x, y) trap_SendServerCommand(x, y)				// Print to id = x
#define SP(x) G_shrubbot_print(ent, x)

#define PAUSE_NONE		0x00	// Match is NOT paused.
#define PAUSE_UNPAUSING	0x01	// Pause is about to expire

#define ZSF_COMP		0x01	// Have comp settings loaded for current gametype?

#define HELP_COLUMNS	4

#define CMD_DEBOUNCE	5000	// 5s between cmds

#define EOM_WEAPONSTATS	0x01	// Dump of player weapon stats at end of match.
#define EOM_MATCHINFO	0x02	// Dump of match stats at end of match.

#define AA_STATSALL		0x01	// Client AutoAction: Dump ALL player stats
#define AA_STATSTEAM	0x02	// Client AutoAction: Dump TEAM player stats


// "Delayed Print" ent enumerations
typedef enum {
	DP_PAUSEINFO,		// Print current pause info
	DP_UNPAUSING,		// Print unpause countdown + unpause
	DP_CONNECTINFO,		// Display OSP info on connect
	DP_MVSPAWN			// Set up MV views for clients who need them
} enum_t_dp;


// Remember: Axis = RED, Allies = BLUE ... right?!

// Team extras
typedef struct {
	qboolean	spec_lock;
	qboolean	team_lock;
	char		team_name[24];
	int			team_score;
	int			timeouts;
} team_info;



///////////////////////
// g_main.c
//
void G_UpdateCvars(void);
void G_wipeCvars(void);
void CheckVote(void);



///////////////////////
// g_cmds_ext.c
//
qboolean G_commandCheck(gentity_t *ent, char *cmd, qboolean fDoAnytime);
qboolean G_commandHelp(gentity_t *ent, char *pszCommand, unsigned int dwCommand);
qboolean G_cmdDebounce(gentity_t *ent, const char *pszCommand);
void G_commands_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
void G_lock_cmd(gentity_t *ent, unsigned int dwCommand, qboolean state);
void G_pause_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
void G_players_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fDump);
void G_ready_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fDump);
void G_say_teamnl_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
void G_scores_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
void G_specinvite_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fLock);
void G_speclock_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fLock);
void G_statsall_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fDump);
void G_teamready_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fDump);
void G_weaponRankings_cmd(gentity_t *ent, unsigned int dwCommand, qboolean state);
void G_weaponStats_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fDump);
void G_weaponStatsLeaders_cmd(gentity_t* ent, qboolean doTop, qboolean doWindow);
void G_VoiceTo( gentity_t *ent, gentity_t *other, int mode, const char *id, qboolean voiceonly );
void G_hitsounds_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);



///////////////////////
// g_config.c
//
void G_configSet(int mode, qboolean doComp);



///////////////////////
// g_match.c
//
// Josh: Player Rating Calculation
void G_CalculatePlayerRatings();
float G_GetWinProbability(team_t team, gentity_t *ent, qboolean updateWeights);
void G_addStats(gentity_t *targ, gentity_t *attacker, int dmg_ref, int mod);
void G_addStatsHeadShot(gentity_t *attacker, int mod);
qboolean G_allowPanzer(gentity_t *ent);
int G_checkServerToggle(vmCvar_t *cv);
char *G_createStats(gentity_t *refEnt);
void G_deleteStats(int nClient);
qboolean G_desiredFollow(gentity_t *ent, int nTeam);
void G_globalSound(char *sound);
void G_initMatch(void);
void G_loadMatchGame(void);
void G_matchInfoDump(unsigned int dwDumpType);
void G_printMatchInfo(gentity_t *ent);
void G_parseStats(char *pszStatsInfo);
void G_printFull(char *str, gentity_t *ent);
void G_resetModeState(void);
void G_resetRoundState(void);
void G_spawnPrintf(int print_type, int print_time, gentity_t *owner);
void G_statsPrint(gentity_t *ent, int nType);
unsigned int G_weapStatIndex_MOD(unsigned int iWeaponMOD);

///////////////////////
// g_multiview.c
//
qboolean G_smvCommands(gentity_t *ent, char *cmd);
void G_smvAdd_cmd(gentity_t *ent);
void G_smvAddTeam_cmd(gentity_t *ent, int nTeam);
void G_smvDel_cmd(gentity_t *ent);
//
void G_smvAddView(gentity_t *ent, int pID);
void G_smvAllRemoveSingleClient(int pID);
unsigned int G_smvGenerateClientList(gentity_t *ent);
qboolean G_smvLocateEntityInMVList(gentity_t *ent, int pID, qboolean fRemove);
void G_smvRegenerateClients(gentity_t *ent, int clientList);
void G_smvRemoveEntityInMVList(gentity_t *ent, mview_t *ref);
void G_smvRemoveInvalidClients(gentity_t *ent, int nTeam);
qboolean G_smvRunCamera(gentity_t *ent);
void G_smvUpdateClientCSList(gentity_t *ent);



///////////////////////
// g_referee.c
//
void Cmd_AuthRcon_f(gentity_t *ent);
void G_refAllReady_cmd(gentity_t *ent);
void G_ref_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
qboolean G_refCommandCheck(gentity_t *ent, char *cmd);
void G_refHelp_cmd(gentity_t *ent);
void G_refLockTeams_cmd(gentity_t *ent, qboolean fLock);
void G_refPause_cmd(gentity_t *ent, qboolean fPause);
void G_refPlayerPut_cmd(gentity_t *ent, int team_id);
void G_refRemove_cmd(gentity_t *ent);
void G_refSpeclockTeams_cmd(gentity_t *ent, qboolean fLock);
void G_refWarmup_cmd(gentity_t* ent);
void G_refWarning_cmd(gentity_t* ent);
void G_refMute_cmd(gentity_t *ent, qboolean mute);
int  G_refClientnumForName(gentity_t *ent, const char *name);
void G_refPrintf(gentity_t* ent, const char *fmt, ...)_attribute((format(printf,2,3)));
void G_PlayerBan(void);
void G_MakeReferee(void);
void G_RemoveReferee(void);
void G_MuteClient(void);
void G_UnMuteClient(void);



///////////////////////
// g_team.c
//
extern char *aTeams[TEAM_NUM_TEAMS];
extern team_info teamInfo[TEAM_NUM_TEAMS];

qboolean G_allowFollow(gentity_t *ent, int nTeam);
int G_blockoutTeam(gentity_t *ent, int nTeam);
qboolean G_checkReady(void);
qboolean G_readyMatchState(void);
void G_removeSpecInvite(int team);
void G_shuffleTeams(void);
void G_swapTeamLocks(void);
void G_swapTeams(void);
qboolean G_teamJoinCheck(int team_num, gentity_t *ent);
int  G_teamID(gentity_t *ent);
void G_teamReset(int team_num, qboolean fClearSpecLock);
void G_verifyMatchState(int team_id);
void G_updateSpecLock(int nTeam, qboolean fLock);



///////////////////////
// g_vote.c
//
int  G_voteCmdCheck(gentity_t *ent, char *arg, char *arg2, qboolean fRefereeCmd);
void G_voteFlags(void);
void G_voteHelp(gentity_t *ent, qboolean fShowVote);
void G_playersMessage(gentity_t *ent);
// Actual voting commands
int G_Comp_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Gametype_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Kick_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Mute_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_UnMute_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Map_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Campaign_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_MapRestart_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_MatchReset_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Mutespecs_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Nextmap_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Pub_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Referee_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_ShuffleTeams_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_StartMatch_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_SwapTeams_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_FriendlyFire_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Timelimit_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Warmupfire_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Unreferee_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_AntiLag_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_BalancedTeams_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Surrender_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_RestartCampaign_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_NextCampaign_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_Poll_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);
int G_ShuffleNoRestart_v(gentity_t *ent, unsigned int dwVoteIndex, char *arg, char *arg2, qboolean fRefereeCmd);


void G_LinkDebris( void );
void G_LinkDamageParents( void );
int EntsThatRadiusCanDamage( vec3_t origin, float radius, int *damagedList );

qboolean G_LandmineTriggered( gentity_t* ent );
qboolean G_LandmineArmed( gentity_t* ent );
qboolean G_LandmineUnarmed( gentity_t* ent );
team_t G_LandmineTeam( gentity_t* ent );
qboolean G_LandmineSpotted( gentity_t* ent );
gentity_t* G_FindSmokeBomb( gentity_t* start );
gentity_t* G_FindLandmine( gentity_t* start );
gentity_t* G_FindDynamite( gentity_t* start );
gentity_t* G_FindSatchels( gentity_t* start );
void G_SetTargetName( gentity_t* ent, char* targetname );
void G_KillEnts( const char* target, gentity_t* ignore, gentity_t* killer, meansOfDeath_t mod );
void trap_EngineerTrace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );

qboolean G_ConstructionIsPartlyBuilt( gentity_t* ent );

int G_CountTeamMedics( team_t team, qboolean alivecheck );
qboolean G_TankIsOccupied( gentity_t* ent );
qboolean G_TankIsMountable( gentity_t* ent, gentity_t* other );

qboolean G_ConstructionBegun( gentity_t* ent );
qboolean G_ConstructionIsFullyBuilt( gentity_t* ent );
qboolean G_ConstructionIsPartlyBuilt( gentity_t* ent );
gentity_t* G_ConstructionForTeam( gentity_t* toi, team_t team );
gentity_t* G_IsConstructible( team_t team, gentity_t* toi );
qboolean G_EmplacedGunIsRepairable( gentity_t* ent, gentity_t* other );
qboolean G_EmplacedGunIsMountable( gentity_t* ent, gentity_t* other );
void G_CheckForCursorHints(gentity_t *ent);
void G_CalcClientAccuracies( void );
void G_BuildEndgameStats( void );
int G_TeamCount( gentity_t* ent, team_t team, weapon_t weap );

qboolean G_IsFireteamLeader( int entityNum, fireteamData_t** teamNum );
fireteamData_t* G_FindFreePublicFireteam( team_t team );
void G_RegisterFireteam(/*const char* name,*/ int entityNum);

void weapon_callAirStrike(gentity_t *ent);
void weapon_checkAirStrikeThink2( gentity_t *ent );
void weapon_checkAirStrikeThink1( gentity_t *ent );
void weapon_callSecondPlane( gentity_t *ent );
qboolean weapon_checkAirStrike( gentity_t *ent );


void G_MakeReady( gentity_t* ent );
void G_MakeUnready( gentity_t* ent );

void SetPlayerSpawn( gentity_t* ent, int spawn, qboolean update );
void G_UpdateSpawnCounts( void );

void G_SetConfigStringValue( int num, const char* key, const char* value );
void G_GlobalClientEvent( int event, int param, int client );

void G_InitTempTraceIgnoreEnts( void );
void G_ResetTempTraceIgnoreEnts( void );
void G_TempTraceIgnoreEntity( gentity_t* ent );
void G_TempTraceIgnorePlayersAndBodies( void );

qboolean G_CanPickupWeapon( weapon_t weapon, gentity_t* ent );

qboolean G_LandmineSnapshotCallback( int entityNum, int clientNum );

#include "g_shrubbot.h"
#include "g_xpsave.h"
extern g_shrubbot_level_t *g_shrubbot_levels[MAX_SHRUBBOT_LEVELS];
extern g_shrubbot_admin_t *g_shrubbot_admins[MAX_SHRUBBOT_ADMINS];
extern g_shrubbot_ban_t *g_shrubbot_bans[MAX_SHRUBBOT_BANS];

#define CH_KNIFE_DIST           64      // from g_weapon.c
#define CH_LADDER_DIST          100
#define CH_WATER_DIST           100
#define CH_BREAKABLE_DIST       64
#define CH_DOOR_DIST            96
#define CH_ACTIVATE_DIST        96
#define CH_EXIT_DIST            256
#define CH_FRIENDLY_DIST        1024
#define CH_REVIVE_DIST          64
#define CH_MAX_DIST		1024    // use the largest value from above
#define CH_MAX_DIST_ZOOM        8192    // max dist for zooming hints

// tjw: g_XPSave flags
#define XPSF_ENABLE			1  // enable XP Save on disconnect
#define XPSF_NR_MAPRESET	2  // no reset on map restarts
#define XPSF_NR_EVER		4  // no reset ever
// call ClientDisconnect() on clients with the same GUID as the connecting
// client even if sv_wwwDlDisconnected is enabled. 
#define XPSF_WIPE_DUP_GUID	8


// tjw: g_hitboxes flags

// lower hitboxes when standing
#define HBF_STANDING 1
// lower hitboxes when crouched
#define HBF_CROUCHING 2
// lower hitboxes for corpses
#define HBF_CORPSE 4
// lower hitboxes for prone
#define HBF_PRONE 8
// lower hitboxes for playdead
#define HBF_PLAYDEAD 16

// tjw: g_hitsounds flags

// hitsounds enabled
#define HSF_ENABLE 1
// no hitsound when shooting corpses
#define HSF_SILENT_CORPSE 2
// no headshot sound when shooting corpse in the head
#define HSF_NO_CORPSE_HEAD 4
// no warning from teammates
#define HSF_NO_TEAM_WARN 8
// no hitsound from poison damage
#define HSF_NO_POISON 16

// tjw: g_voting flags

// use the number of voters instead of the total players to
// decide if a vote passes
#define VOTEF_USE_TOTAL_VOTERS 1
// successful votes do not count against vote_limit
#define VOTEF_NO_POPULIST_PENALTY 2
// append "(called by name)" in vote string
#define VOTEF_DISP_CALLER 4

// tjw: g_spectator flags

// freelook specs can click another player to follow
#define SPECF_FL_CLICK_FOLLOW 1
// when 'missing' another player in freelook mode, go
// to next available client
#define SPECF_FL_MISS_FOLLOW_NEXT 2
// specs continue following the same player when he goes
// into limbo
#define SPECF_PERSIST_FOLLOW 4
// specs go into freelook instead of following next 
// available player
#define SPECF_FL_ON_LIMBO 8

// tjw: g_serverInfo flags

// display team information with P
#define SIF_P 1
// display team information with Players_Allies and Players_Axis
#define SIF_PLAYERS 2
// display semi-colon delimited list of campaign maps 
#define SIF_CAMPAIGN_MAPS 4
// display current campaign status in form xx,yy
#define SIF_C 8
// display charge times
#define SIF_CHARGE 16

// tjw: g_goombaFlags 

// goomba can only damage enemies
#define GBF_ENEMY_ONLY 1 
// EV_FALL_SHORT (hopping) does not do damage
#define GBF_NO_HOP_DAMAGE 2
// EV_FALL_SHORT does not to damage to teammates
#define GBF_NO_HOP_TEAMDAMAGE 4
// goomba doesn't do any damage to faller
#define GBF_NO_SELF_DAMAGE 8
// instagib goomba damage
#define GBF_INSTAGIB 16


// josh: g_ATB
// Don't active team balance
#define ATB_OFF 0
// use XP for active team balancing
#define ATB_XP 1
// use playerrating for active team balancing
#define ATB_PLAYERRATING 2

// tjw: g_ATB_rating 

// use total player XP
#define ATBR_XP 1
// use XP over time
#define ATBR_XPRATE 2
// use killRating
#define ATBR_KILLRATING 3
// use playerRating
#define ATBR_PLAYERRATING 4

// tjw: g_stats 

// don't count hits when gibbing corpses
#define STATF_CORPSE_NO_HIT 1
// don't count shots when gibbing corpses
#define STATF_CORPSE_NO_SHOT 2

// tjw: g_poisonFlags

// show shaking to the client
#define POISF_VIEWLOCK 1
// make the head of player shake (as viewed from others)
#define POISF_BOBBLE 2
// make the player bend over (hurl) every 2 seconds (as veiwed from others)
#define POISF_HURL 4
// poisoned players cannot shoot
#define POISF_NO_ATTACK 8
// poisoned players are disoriented (head turned upside down)
#define POISF_DISORIENT 16

// tjw: g_obituary

// always send the obituary to the client as EV_OBITUARY
#define OBIT_CLIENT_ONLY 1
// send the obituary as EV_OBITUARY if we think the client, but only if we 
// think the client knows how to handle it
#define OBIT_CLIENT_PREF 2
// always let the server handle the obituary with cpm (or other means)
#define OBIT_SERVER_ONLY 3

// Josh: g_logOptions flags, only for GUIDs now (no WeaponStatsDMG or TStats)
#define LOGOPTS_OBIT_CHAT	1 // Server puts obituaries in chat.
#define LOGOPTS_ADR_COUNT	2 // Adrenaline Countdown
#define LOGOPTS_DIS_TAPCON	4 // Disable the tap-out confirmation dialog box
#define LOGOPTS_BAN_CONN	8 // Display connection attempts by banned players
#define LOGOPTS_REPORT_GIBS	16 // Report $victim was gibbed by $attacker
#define LOGOPTS_OMIT_ITEMS	32 // Omit "Item" lines from logfile 
#define LOGOPTS_GUID	128 // Log the GUIDs, had other functionality in shrubet 
#define LOGOPTS_PRIVMSG 256 // log all private messages
#define LOGOPTS_REALTIME 512 // real time stamps in log (dvl)
#define LOGOPTS_TK_WEAPON 2048 // print out how you TK'ed

// Josh: g_censorPenalty flags
#define CNSRPNLTY_KILL 1		// Gibs unless CNSRPNLTY_NOGIB
#define CNSRPNLTY_KICK 2		// Kick if the word is in their name
#define CNSRPNLTY_NOGIB 4		// Won't GIB, only kill
#define CNSRPNLTY_TMPMUTE 8 // Muted the next g_censorMuteTime.integer seconds

//Josh: g_skills flags
#define SKILLS_MINES 1
#define SKILLS_FLAK 2
#define SKILLS_ADREN 4

//Josh: g_killRating flags
//      if any flag is set, killrating is calculated
#define KILL_RATING_ENABLE 1	// Kill rating will be calculated
#define KILL_RATING_VISIBLE 2 // /killrating will work, end of game killers too

//Josh: g_playerRating flags
//      if any flag is set, playerrating is calculated
//      These aren't implemented yet
#define PLAYER_RATING_ENABLE 1 // calculate player ratings
#define PLAYER_RATING_VISIBLE 2 // end of game overals shown, plus /playerrating works

extern wordDictionary censorDictionary;
extern wordDictionary censorNamesDictionary;

// dvl
// used in the g_slashKill cvar
#define SLASHKILL_HALFCHARGE 1
#define SLASHKILL_ZEROCHARGE 2
#define SLASHKILL_SAMECHARGE 4
#define SLASHKILL_NOKILL 8
#define SLASHKILL_NOPOISON 16

void G_createClusterNade( gentity_t *ent, int numNades );
void G_MakePip( gentity_t *vic );

#define MSGPOS_CHAT 0
#define MSGPOS_CENTER 1
#define MSGPOS_LEFT_BANNER 2

qboolean G_FlingClient( gentity_t *vic, int flingType );
void G_PrintBanners();
void G_touchKnife( gentity_t *ent, gentity_t *other, trace_t *trace );
void G_throwKnife( gentity_t *ent );

// forty - airstrike block
#define ASBLOCK_ANNOUNCE 1
#define ASBLOCK_EASY 2
// Not implemented but reserving since it's in the shrub docs
#define ASBLOCK_PLAY_SOUND 4 
#define ASBLOCK_LVL3_FDOPS_NOBLOCK 8

// forty - sudden death dynamite and dynamite chaining.
#define DYNO_SUDDENDEATH 1
#define DYNO_CHAIN 2

// Perro - g_dmg flags for advanced combat options
#define COMBAT_USE_HITLOC 1
#define COMBAT_USE_ALTDIST 2
#define COMBAT_USE_CCBONUS 4
#define COMBAT_USE_BFALL 8
#define COMBAT_USE_IRUPG 16
#define COMBAT_USE_ALTSPRD 32
#define COMBAT_NOSPEC_DMG 64 // Prevent specs from dealing damage
// #define COMBAT_DAMAGE_OWN_VEHICLES 128 // future use

// Perro - g_reflectFFWeapons bitmask definitions
#define		REFLECT_FF_BULLETS 1
#define		REFLECT_FF_GRENADES 2
#define		REFLECT_FF_KNIVES	4
#define		REFLECT_FF_PANZER	8
#define		REFLECT_FF_FLAMER	16
#define		REFLECT_FF_MORTAR	32
#define		REFLECT_FF_SATCHEL	64
#define		REFLECT_FF_BOMBS	128
#define		REFLECT_FF_DYNAMITE	256
#define		REFLECT_FF_LANDMINES 512

// Perro- g_friendlyfireOpts masks
#define		FFOPTS_MINE_OVERRIDE 1
#define		FFOPTS_ALLOW_BOOSTING 2
#define		FFOPTS_NO_FRIENDLY_MINES 4

// tjw: g_shuffle_rating
#define SHUFR_XP	1
#define SHUFR_XPRATE	2
#define SHUFR_KRATING	3
#define SHUFR_PRATING	4

// tjw: g_XPDecay
#define XPDF_ENABLE 1
// don't decay xp when the client is disconnected (XPSave)
#define XPDF_NO_DISCONNECT_DECAY 2
// don't decay xp specific for the clients class
#define XPDF_NO_CLASS_DECAY 4
// don't decay xp if the client is on specators
#define XPDF_NO_SPEC_DECAY 8
// don't decay xp unless the gamestate is GS_PLAYING
#define XPDF_NO_GAMESTATE_DECAY 16
// don't decay xp while a player is actively playing
#define XPDF_NO_PLAYING_DECAY 32
// don't decay battle sense
#define XPDF_NO_BS_DECAY 64
// don't decay light weapons
#define XPDF_NO_LW_DECAY 128

// forty - canister kicking
void G_CanisterKick();

// forty - stack traces - g_crash.h
void EnableCoreDumps();
void DisableCoreDumps();
void EnableStackTrace();
void DisableStackTrace();

// unique_utils.c
int OrgVisible(vec3_t org1, vec3_t org2, int ignore);
int OrgVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore);
void*	Q_malloc(size_t size);
void BG_MemoryInit(int hunkMegs);
void BG_MemoryClose();
qboolean BG_IsMemoryInitialized();
void* Q_realloc(void *ptr, size_t size);
void Q_free(void *ptr);
void* Q_calloc(size_t nmemb, size_t size);
void* Q_malloc(size_t size);
qboolean MyInFOV ( gentity_t *ent, gentity_t *from, int hFOV, int vFOV );

// New random code.. Unique1
int Q_irand(int value1, int value2);
int Q_TrueRand ( int low, int high );

#ifdef __BOT__

// AIMod Stuff from here on...

#ifndef _uniquebot_H
#define _uniquebot_H
#endif

//extern int MAX_BOT_THINKFRAME;

#define MAX_BOT_THINKFRAME 5
//#define MAX_BOT_THINKFRAME 1
//#define MAX_BOT_THINKFRAME 20 // 20ms max per frame???
//#define MAX_BOT_THINKFRAME 1 // 1ms max per frame???

//  ___       __ _      _ _   _
// |   \ ___ / _(_)_ _ (_) |_(_)___ _ _  ___
// | |) / -_)  _| | ' \| |  _| / _ \ ' \(_-<
// |___/\___|_| |_|_||_|_|\__|_\___/_||_/__/
//------------------------------------------------------------------------------------
#define MAX_VISIBLENODES	50
//#define NODE_DENSITY        3000			// Node Density
#define NODE_DENSITY        2000			// Node Density
#define MAX_NODELINKS       32              // Maximum Node Links (12)
#define INVALID             -1              // Invalid Node Link
extern  cvar_t              *botchat;       // Bot Chat, off/on (0,1)
extern  cvar_t              *showpath;      // Show bot paths
//------------------------------------------------------------------------------------

//  ___ _      _    __                 ___ _____ _ _____ ___
// | _ \ |__ _| |_ / _|___ _ _ _ __   / __|_   _/_\_   _| __|
// |  _/ / _` |  _|  _/ _ \ '_| '  \  \__ \ | |/ _ \| | | _|
// |_| |_\__,_|\__|_| \___/_| |_|_|_| |___/ |_/_/ \_\_| |___|
//------------------------------------------------------------------------------------
#define PLATFORM_STATE_TOP          0   // Platform State Top
#define PLATFORM_STATE_BOTTOM       1   // Platform State Bottom
#define PLATFORM_STATE_UP           2   // Platform State Moving Up
#define PLATFORM_STATE_DOWN         3   // Platform State Moving Down
//------------------------------------------------------------------------------------

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
#define NODEFIND_TANK				4      // For finding tank only nodes...
#define NODEFIND_ALL				8      // For selecting all nodes

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

//  ___      _     ___ _____ _ _____ ___
// | _ ) ___| |_  / __|_   _/_\_   _| __|
// | _ \/ _ \  _| \__ \ | |/ _ \| | | _|
// |___/\___/\__| |___/ |_/_/ \_\_| |___|
//------------------------------------------------------------------------------------
#define BOT_STATE_STAND				0           // Bot Standing
#define BOT_STATE_MOVE				1           // Bot Moving
#define BOT_STATE_ATTACK			2           // Bot Attacking
#define BOT_STATE_WANDER			3           // Bot Wandering
#define BOT_STATE_FLEE				4           // Bot Fleeing
#define BOT_STATE_POSITION			5	        // Bot Position
#define BOT_STATE_GETFLAG			6			// Bot on flag 'recap' duty

#define ACTION_DEFAULT				0
#define ACTION_FLAGRECAP			1
#define ACTION_FLAGCAPTURE			2
#define ACTION_KILLCARRIER			3

#define BOT_MOVE_LEFT				0           // Bot Move Left
#define BOT_MOVE_RIGHT				1           // Bot Move Right
#define BOT_MOVE_FORWARD			2           // Bot Move Forward
#define BOT_MOVE_BACK				3           // Bot Move Back
//------------------------------------------------------------------------------------

//  ___ _               _    _    _
// |_ _| |_ ___ _ __   | |  (_)__| |_
//  | ||  _/ -_) '  \  | |__| (_-<  _|
// |___|\__\___|_|_|_| |____|_/__/\__|
//------------------------------------------------------------------------------------
#define ITEMLIST_NULLINDEX          0       // Item Null
#define ITEMLIST_BODYARMOR          1       // Item Body Armor
#define ITEMLIST_COMBATARMOR        2       // Item Combat Armor
#define ITEMLIST_JACKETARMOR        3       // Item Jacket Armor
#define ITEMLIST_ARMORSHARD         4       // Item Armor Shard
#define ITEMLIST_POWERSCREEN        5       // Item Powerscreen
#define ITEMLIST_POWERSHIELD        6       // Item Powershield

#define ITEMLIST_GRAPPLE            7       // Item Grapple

#define ITEMLIST_BLASTER            8       // Weapon Blaster
#define ITEMLIST_SHOTGUN            9       // Weapon Shotgun
#define ITEMLIST_SUPERSHOTGUN       10      // Weapon Supershotgun
#define ITEMLIST_MACHINEGUN         11      // Weapon Machinegun
#define ITEMLIST_CHAINGUN           12      // Weapon Chaingun
#define ITEMLIST_GRENADES           13      // Weapon Grenades
#define ITEMLIST_GRENADELAUNCHER    14      // Weapon Grenade Launcher
#define ITEMLIST_ROCKETLAUNCHER     15      // Weapon Rocket Launcher
#define ITEMLIST_HYPERBLASTER       16      // Weapon Hyper-Blaster
#define ITEMLIST_RAILGUN            17      // Weapon Railgun
#define ITEMLIST_BFG10K             18      // Weapon BFG

#define ITEMLIST_SHELLS             19      // Ammo Shells
#define ITEMLIST_BULLETS            20      // Ammo Bullets
#define ITEMLIST_CELLS              21      // Ammo Cells
#define ITEMLIST_ROCKETS            22      // Ammo Rockets
#define ITEMLIST_SLUGS              23      // Ammo Slugs
#define ITEMLIST_QUADDAMAGE         24      // Power-up Quad Damage
#define ITEMLIST_INVULNERABILITY    25      // Power-up Invulnerability
#define ITEMLIST_SILENCER           26      // Power-up Silencer
#define ITEMLIST_REBREATHER         27      // Power-up Rebreather
#define ITEMLIST_ENVIRONMENTSUIT    28      // Power-up Environmentsuit
#define ITEMLIST_ANCIENTHEAD        29      // Power-up Ancient Head
#define ITEMLIST_ADRENALINE         30      // Power-up Adrenaline
#define ITEMLIST_BANDOLIER          31      // Power-up Bandolier
#define ITEMLIST_AMMOPACK           32      // Power-up Ammo Pack
#define ITEMLIST_DATACD             33      // Power-up Data CD
#define ITEMLIST_POWERCUBE          34      // Power-up Power Cube
#define ITEMLIST_PYRAMIDKEY         35      // Power-up Pyramid Key
#define ITEMLIST_DATASPINNER        36      // Power-up Data Spinner
#define ITEMLIST_SECURITYPASS       37      // Power-up Security Pass
#define ITEMLIST_BLUEKEY            38      // Power-up Blue Key
#define ITEMLIST_REDKEY             39      // Power-up Red Key
#define ITEMLIST_COMMANDERSHEAD     40      // Power-up Commanders Head
#define ITEMLIST_AIRSTRIKEMARKER    41      // Power-up Airstrike Marker
#define ITEMLIST_HEALTH             42      // Power-up Health

#define ITEMLIST_FLAG1              43      // CTF Red Flag
#define ITEMLIST_FLAG2              44      // CTF Blue Flag
#define ITEMLIST_RESISTANCETECH     45      // CTF Tech - Resistance
#define ITEMLIST_STRENGTHTECH       46      // CTF Tech - Strength
#define ITEMLIST_HASTETECH          47      // CTF Tech - Haste
#define ITEMLIST_REGENERATIONTECH   48      // CTF Tech - Regeneration

#define ITEMLIST_HEALTH_SMALL       49      // Health - Small
#define ITEMLIST_HEALTH_MEDIUM      50      // Health - Medium
#define ITEMLIST_HEALTH_LARGE       51      // Health - Large
#define ITEMLIST_BOT                52      // Bot
#define ITEMLIST_PLAYER             53      // Player
#define ITEMLIST_HEALTH_MEGA        54      // Health - MEGA

sodbot_t SOD_SPAWN_ReadProfile( char *profile );
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

typedef struct flagstate_s {
	qboolean dropped;
	qboolean valid;		// Can be invalid if the flag is not reachable f.i.
	qboolean stolen;
	int node;			// Number of the node closest to the flag
	int entityNum;		// Index number of the entity for this flag
    int carrier;		// Playernumber of the player carrying the flag

} flagstate_t;

typedef struct botai_s {

	qboolean ctf;								// Is true if we have to capture flag, docs, gold or something
	flagstate_t flagstate[TEAM_NUM_TEAMS];

} botai_t;

extern botai_t botai;
extern int enodecount;
extern enode_t enodes[200];
//  ___ _               ___ _               _
// |_ _| |_ ___ _ __   / __| |_ _ _ _  _ __| |_ _  _ _ _ ___
//  | ||  _/ -_) '  \  \__ \  _| '_| || / _|  _| || | '_/ -_)
// |___|\__\___|_|_|_| |___/\__|_|  \_,_\__|\__|\_,_|_| \___|
//------------------------------------------------------------------------------------
typedef struct item_table_s     // Item Table Structure
{
    int         item;           // Item
    float       weight;         // Weight
    gentity_t   *ent;         // Entity // fretn
    int         node;           // Node

} item_table_t;                 // Item Table Typedef
//------------------------------------------------------------------------------------
//  ___     _                     _
// | __|_ _| |_ ___ _ _ _ _  __ _| |___
// | _|\ \ /  _/ -_) '_| ' \/ _` | (_-<
// |___/_\_\\__\___|_| |_||_\__,_|_/__/
//------------------------------------------------------------------------------------
//extern node_t       nodes[MAX_NODES];       // Node Table
extern node_t       *nodes;       // Node Table
extern item_table_t item_table[MAX_ITEMS]; // Item Table //fretn
extern qboolean     debug_mode;             // Debug Mode
extern int          number_of_nodes;        // Number Of Nodes
extern int          num_items;              // Number Of Items
extern int          num_players;            // Number of Players
extern gentity_t    *players[MAX_CLIENTS];  // Pointers to all players in the game //fretn

//------------------------------------------------------------------------------------

void AIMOD_MAPPING_Realtime_NodeLinks_Checking ( void );
void AIMOD_SPAWN_SpawnBot (char *team, char *name, int skill);
int AIMOD_SPAWN_FindFreeClient(void);

void BotMP_Add_Snipe_Point ( gentity_t *ent );
qboolean AIMOD_AI_Think (gentity_t *uniquebot);
void AIMOD_AI_BotPain(gentity_t *self, gentity_t *attacker, int damage, vec3_t point);
qboolean AIMOD_AI_LocateEnemy(gentity_t *uniquebot);
qboolean AIMOD_AI_inFOV(vec3_t viewangles, float fov, vec3_t angles);
qboolean Have_Close_Enemy ( gentity_t *bot );
void AIMOD_AI_DropMed(gentity_t *uniquebot);

qboolean AIMOD_ITEMS_IsReachable(gentity_t *uniquebot, vec3_t goal);
void AIMOD_ITEMS_uniquebotAdded(gentity_t *ent);

void AIMOD_MOVEMENT_Attack (gentity_t *uniquebot, usercmd_t *ucmd);
qboolean AIMOD_MOVEMENT_CanMove(gentity_t *uniquebot, int direction);
void AIMOD_MOVEMENT_Wander(gentity_t *uniquebot, usercmd_t *ucmd);
qboolean AIMOD_MOVEMENT_SpecialMove(gentity_t *uniquebot, usercmd_t *ucmd);
void AIMOD_MOVEMENT_Move(gentity_t *uniquebot, usercmd_t *ucmd);
void AIMOD_MOVEMENT_MoveToGoal(gentity_t *uniquebot, usercmd_t *ucmd);
void AIMOD_MOVEMENT_ChangeBotAngle (gentity_t *ent);

qboolean AIMOD_MAPPING_FollowPath(gentity_t *uniquebot);
void AIMOD_MAPPING_ShowNodes(gentity_t *ent);
void AIMOD_MAPPING_AddNode( int clientNum, int teamNum ); // fretn
void AIMOD_MAPPING_WriteNodes(); // fretn
int CreatePathAStar ( gentity_t *bot, int from, int to, /*short*/ int *pathlist );
int CreateAltPathAStar ( gentity_t *bot, int from, int to, /*short*/ int *pathlist );
void AIMOD_Bot_Aim_At_Object ( gentity_t *bot, gentity_t *object );
void BOT_ChangeWeapon( gentity_t *bot, int weaponChoice );

#ifdef __VEHICLES__
qboolean BOT_HaveVehicleWaypoints ( void );
#endif //__VEHICLES__

int AIMOD_NAVIGATION_FindClosestReachableNode(gentity_t *uniquebot, int range, int type, gentity_t *master);

void PM_CheckForReload(int weapon);

gentity_t *findradius (gentity_t *from, vec3_t org, float rad);
void G_ProjectSource (vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);
extern void G_Say(gentity_t *ent, gentity_t *target, int mode, const char *chatText);
void G_Voice( gentity_t *ent, gentity_t *target, int mode, const char *id, qboolean voiceonly );
extern void limbo ( gentity_t * ent , qboolean makeCorpse, qboolean headShot ) ;

/**
 * SODBOT PREPROCESSOR CONSTANTS
 */
#define INVALID		-1

#define MAX_VISIBLENODES	50

/**
 * SODBOT GLOBAL VARIABLES
 */
extern int number_of_bots;		// Holds the total number of bots

sodbot_t sodbotstates[MAX_BOTS];	// Holds the entire botstate table
gentity_t *sbots[MAX_BOTS];			// Holds pointers to the bot entities

/*
 * NODES GLOBAL VARIABLES
 */

//node_t		nodes[MAX_NODES];
node_t		*nodes;
qboolean	shownodes; // fretn

// Total number of nodes
//-------------------------------------------------------
int number_of_nodes;

/**
 * SODBOT FUNCTION HEADERS
 */
extern sodbot_t SOD_SPAWN_ReadProfile( char *profile );
extern qboolean SOD_SPAWN_AddBot( sodbot_t *bs, char *team, char *load_userinfo );
extern void SOD_SPAWN_CreateBot( gentity_t *bot, char *team_str );
extern int SOD_SPAWN_ChooseClass();
extern void SOD_AI_BotThink( gentity_t *bot );
extern void SOD_AI_parseServerCommand( gentity_t *bot, qboolean show);
extern void SOD_AI_Soldier (gentity_t *soldier);
extern void SOD_AI_Engineer (gentity_t *engineer);
extern void SOD_AI_Medic (gentity_t *medic);
extern void SOD_AI_Lt (gentity_t *lt);
extern void SOD_AI_ScriptLoad();
extern void SOD_AI_ScriptParse();
extern void SOD_AI_Determine_LR_Goal(gentity_t *bot);
extern vec3_t SP_Bot_Position[MAX_CLIENTS];
extern void SOD_MAPPING_InitMapNodes();
extern int OrgVisible(vec3_t org1, vec3_t org2, int ignore);
extern qboolean AddWeaponToPlayer( gclient_t *client, weapon_t weapon, int ammo, int ammoclip, qboolean setcurrent );
extern int AIMOD_NAVIGATION_FindRunawayReachableNode(gentity_t *uniquebot, int r, int type);
extern qboolean player_close ( vec3_t origin );
extern qboolean SOD_SPAWN_AddBotShort( char *team, char *name, int skill, char *userinfo );
extern void AIMOD_NODES_ResolveAllPaths();
//extern int BG_GrenadesForClass( int cls, int team, int* skills );
int BG_GrenadesForClass( int cls, int* skills );
extern void AIMOD_MAPPING_SetTouchConnection();
extern weapon_t BG_GrenadeTypeForTeam( team_t team );
extern void Select_SP_Spawnpoint ( gentity_t *ent ); // AIMOD_navigation.c - UQ1
float SOD_NODES_PathCost( int from, int to );
void SOD_NODES_LoadNodes( void );
int AIMOD_AI_SetMedkitAction( gentity_t *uniquebot, usercmd_t *ucmd );
int AIMOD_AI_SetLTAction( gentity_t *uniquebot, usercmd_t *ucmd );
int AIMOD_NAVIGATION_SelectLinkTo(int node, gentity_t *bot, vec3_t target_pos);
int AIMOD_Calculate_Goal_From_Nodenum(int nodenum);
// Character Case compare..
#define Q_strcasecmp(a, b) Q_stricmp(a, b)
//void AIMOD_Calculate_Route_Info ( void );
qboolean AI_PM_SlickTrace( vec3_t point, int clientNum );
//void AddExtraSpawnAmmo( gclient_t *client, weapon_t weaponNum);
float	anglemod(float a);
qboolean OriginWouldTelefrag( vec3_t origin );
int Count_Bot_Players ( void );
int Count_Human_Players ( void );
char *PickName ( int teamNum );
int Pick_Bot_Kick ( void );
void SOD_SPAWN_RemoveBot (int clientNum);
void SetBotWeapons ( gentity_t *bot );
void AIMOD_Player_Special_Needs_Clear (int clientNum);
void AI_ResetJob(gentity_t *bot);
void AIMOD_SaveBotInfo( void );
void AIMOD_LoadBotInfo( void );
void AIMOD_NODES_LoadNodes(void);
void AIMOD_NODES_SaveNodes(void);
void AI_Needs_Medic (int clientNum);
void AI_Needs_Ammo (int clientNum);
void set_medic_route ( gentity_t *ent);
void set_fieldops_route ( gentity_t *ent);
void set_engineer_route ( gentity_t *ent);
void set_follow_route ( gentity_t *ent);
qboolean NodesLoaded ( void );
void AIMOD_MAPPING_SetRelocateNode();
void AIMOD_MAPPING_SetNodeType( int node, int node_type );
void AIMOD_SPAWN_BotList(void);
void AIMOD_SPAWN_RemoveBot (int clientNum);
void BOT_GetMuzzlePoint ( gentity_t *bot );
void BOT_MakeLinkUnreachable ( gentity_t *bot, int node, int linkNum );
void BOT_AddAxisDoor(vec3_t origin);
void BOT_AddAlliedDoor(vec3_t origin);
int BOT_GetTeamDoorNodeAndLink ( gentity_t *bot );
int CreateTeamRoute(gentity_t *bot, /*short*/ int *pathlist);
void AIMOD_NODES_RemoveTeamNodes( void );
void AIMod_AutoWaypoint ( void );
// jaquboss , chatting //
void G_AI_Voice( gentity_t *bot, gentity_t *target, int mode, const char *id, qboolean voiceonly, int delay, aiVoiceType_t type );
void AI_RecordVoiceChat(  gentity_t *other, const char *id );
qboolean AIMOD_MOVEMENT_Explosive_Near_Waypoint ( int wp );
qboolean AIMOD_MOVEMENT_Is_On_Ladder ( gentity_t *bot );
qboolean AIMOD_MOVEMENT_Is_Swimming ( gentity_t *bot );
qboolean AIMOD_MOVEMENT_Is_Sliding ( gentity_t *bot );
qboolean AIMOD_MOVEMENT_Should_Not_Follow ( gentity_t *bot );
qboolean AIMOD_AI_Enemy_Is_Viable ( gentity_t *bot, gentity_t *enemy );
qboolean AIMOD_MOVEMENT_SniperFunc ( gentity_t *bot, usercmd_t *ucmd, qboolean avoiding_explosives );
void AIMOD_SP_MOVEMENT_CoverSpot_Do_Actual_Attack ( gentity_t *bot, usercmd_t *ucmd );
void AIMOD_SP_MOVEMENT_CoverSpot_Aim_At_Enemy ( gentity_t *bot );
void BotMP_Generate_Sniper_Spots ( void );

void Scan_AI_For_Followers ( int clientNum );
void G_BOT_Check_Fire_Teams ( gentity_t *ent );
void AIMOD_MOVEMENT_FireTeam_AI_Issue_Order ( gentity_t *boss, int type, gentity_t *enemy, vec3_t position );

extern stringID_table_t WeaponTableStrings[];

// Fireteam order types...
typedef enum {
	FT_ORDER_NONE,				// 0
	FT_ORDER_DEFAULT,			// 0
	FT_ORDER_COVER,				// 1
	FT_ORDER_SUPRESS,			// 2
	FT_ORDER_ASSAULT,			// 3
	FT_ORDER_RETURN,			// 4
	FT_ORDER_TOGGLE_GROUP		// 5
}  fireteam_orders_t;
#endif //__BOT__

qboolean MyVisible (gentity_t *self, gentity_t *other);

// New random code.. Unique1
int Q_irand(int value1, int value2);
int Q_TrueRand ( int low, int high );

// unique_mem.c
void *BG_Alloc ( int size );
void BG_Free(void *ptr);
void *BG_DebugAlloc ( int size, char *procedure );

#define BG_GetAmmoClip(ps, wp)			(ps)->ammoclip[wp]
#define BG_SetAmmoClip(ps, wp, val)		(ps)->ammoclip[wp] = val
#define BG_AddAmmoClip(ps, wp, val)		(ps)->ammoclip[wp] += val

#define BG_GetAmmo(ps, wp)				(ps)->ammoclip[wp]
#define BG_SetAmmo(ps, wp, val)			(ps)->ammoclip[wp] = val
#define BG_AddAmmo(ps, wp, val)			(ps)->ammoclip[wp] += val

#define Weapon_BitCheck(ps, wep)		BG_BitCheck((ps)->weapons, wep)
#define Weapon_BitSet(ps, wep)			BG_BitSet((ps)->weapons, wep)
#define Weapon_BitClear(ps, wep)		BG_BitClear((ps)->weapons, wep)

// bg_misc.c
//void BG_AddAmmo(playerState_t *ps, weapon_t weapon, int ammount);
//void BG_AddAmmoClip(playerState_t *ps, weapon_t weapon, int ammount);

extern vec3_t playerMins;
extern vec3_t playerMaxs;

#ifdef __ETE__
typedef struct {
	gentity_t	*flagentity;
	int			redAssociatedSpawnNumber;
	gentity_t	*redAssociatedSpawnareas[64];
	int			blueAssociatedSpawnNumber;
	gentity_t	*blueAssociatedSpawnareas[64];
	int			associated_red_spawnpoints_number;
	vec3_t		associated_red_spawnpoints[256];
	vec3_t		associated_red_spawnangles[256];
	int			associated_blue_spawnpoints_number;
	vec3_t		associated_blue_spawnpoints[256];
	vec3_t		associated_blue_spawnangles[256];
	int			redUnassociatedSpawnNumber;
	gentity_t	*redUnassociatedSpawnareas[64];
	int			blueUnassociatedSpawnNumber;
	gentity_t	*blueUnassociatedSpawnareas[64];
	int			unassociated_red_spawnpoints_number;
	vec3_t		unassociated_red_spawnpoints[256];
	vec3_t		unassociated_red_spawnangles[256];
	int			unassociated_blue_spawnpoints_number;
	vec3_t		unassociated_blue_spawnpoints[256];
	vec3_t		unassociated_blue_spawnangles[256];
//	gentity_t	*redNPCs[32];
//	gentity_t	*blueNPCs[32];
	vec3_t		spawnpoints[128/*64*/];
	vec3_t		spawnangles[128/*64*/];
	int				num_spawnpoints;
	float				other_flag_distance[1024];
	qboolean		other_flag_distance_set[1024];
} flag_list_t;

flag_list_t flag_list[1024];

qboolean POPSpawnpointAvailable ( gentity_t *ent );
int GetNumberOfPOPFlags ( void );
qboolean AdvancedWouldTelefrag(vec3_t point);
qboolean CheckEntitiesInSpot(vec3_t point);

// UQ1: Secondary client entities... Code is g_main.c
gentity_t *G_SecondaryEntityFor (int clientNum );
void G_InitSecondaryEntity (int clientNum );
void G_CreateSecondaryEntities ( void );
#endif //__ETE__

gentity_t *SecondaryEntity( int entityNum );
entityState_t *SecondaryEntityState( int entityNum );
qboolean HasExtFlag( int entityNum, int flag );
qboolean IsCoopGametype ( void );

#ifdef __VEHICLES__
void UpdateTankTurret ( gentity_t *ent );
void AddTankTurret ( gentity_t *ent );
void RemoveTankTurret ( gentity_t *ent );
#endif //__VEHICLES__
