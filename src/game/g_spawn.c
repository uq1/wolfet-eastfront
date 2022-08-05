/*
 * name:		g_spawn.c
 *
 * desc:	//modified by Masteries 27//11/2007	
 *          //added misc_panzeriv,misc_kv1,misc_stug3
*/

#include "g_local.h"
qboolean G_SpawnStringExt( const char *key, const char *defaultString, char **out, const char* file, int line ) {
	int		i;

	if ( !level.spawning ) {
		*out = (char *)defaultString;
		// Gordon: 26/11/02: re-enabling
		// see InitMover
		G_Error( "G_SpawnString() called while not spawning, file %s, line %i", file, line );
	}

	for ( i = 0 ; i < level.numSpawnVars ; i++ ) {
		if ( !strcmp( key, level.spawnVars[i][0] ) ) {
			*out = level.spawnVars[i][1];
			return qtrue;
		}
	}

	*out = (char *)defaultString;
	return qfalse;
}

qboolean	G_SpawnFloatExt( const char *key, const char *defaultString, float *out, const char* file, int line ) {
	char		*s;
	qboolean	present;

	present = G_SpawnStringExt( key, defaultString, &s, file, line );
	*out = atof( s );
	return present;
}

qboolean	G_SpawnIntExt( const char *key, const char *defaultString, int *out, const char* file, int line ) {
	char		*s;
	qboolean	present;

	present = G_SpawnStringExt( key, defaultString, &s, file, line );
	*out = atoi( s );
	return present;
}

qboolean	G_SpawnVectorExt( const char *key, const char *defaultString, float *out, const char* file, int line ) {
	char		*s;
	qboolean	present;

	present = G_SpawnStringExt( key, defaultString, &s, file, line );
	sscanf( s, "%f %f %f", &out[0], &out[1], &out[2] );
	return present;
}

qboolean	G_SpawnVector2DExt( const char *key, const char *defaultString, float *out, const char* file, int line ) {
	char		*s;
	qboolean	present;

	present = G_SpawnStringExt( key, defaultString, &s, file, line );
	sscanf( s, "%f %f", &out[0], &out[1] );
	return present;
}



//
// fields are needed for spawning from the entity string
//
typedef enum {
	F_INT, 
	F_FLOAT,
	F_LSTRING,			// string on disk, pointer in memory, TAG_LEVEL
	F_GSTRING,			// string on disk, pointer in memory, TAG_GAME
	F_VECTOR,
	F_ANGLEHACK,
	F_ENTITY,			// index on disk, pointer in memory
	F_ITEM,				// index on disk, pointer in memory
	F_CLIENT,			// index on disk, pointer in memory
	F_IGNORE
} fieldtype_t;

typedef struct
{
	char	*name;
	int		ofs;
	fieldtype_t	type;
	int		flags;
} field_t;

field_t fields[] = {
	{"classname",	FOFS(classname),	F_LSTRING},
	{"origin",		FOFS(s.origin),		F_VECTOR},
	{"model",		FOFS(model),		F_LSTRING},
	{"model2",		FOFS(model2),		F_LSTRING},
	{"spawnflags",	FOFS(spawnflags),	F_INT},
	{"eflags",		FOFS(s.eFlags),		F_INT},
	{"svflags",		FOFS(r.svFlags),		F_INT},
	{"maxs",		FOFS(r.maxs),		F_VECTOR},
	{"mins",		FOFS(r.mins),		F_VECTOR},
	{"speed",		FOFS(speed),		F_FLOAT},
	{"closespeed",	FOFS(closespeed),	F_FLOAT},	//----(SA)	added
	{"target",		FOFS(target),		F_LSTRING},
	{"targetname",	FOFS(targetname),	F_LSTRING},
	{"message",		FOFS(message),		F_LSTRING},
	{"popup",		FOFS(message),		F_LSTRING},	// (SA) mutually exclusive from 'message', but makes the ent more logical for the level designer
	{"book",		FOFS(message),		F_LSTRING},	// (SA) mutually exclusive from 'message', but makes the ent more logical for the level designer
	{"team",		FOFS(team),			F_LSTRING},
	{"wait",		FOFS(wait),			F_FLOAT},
	{"random",		FOFS(random),		F_FLOAT},
	{"count",		FOFS(count),		F_INT},
	{"health",		FOFS(health),		F_INT},
	{"light",		0,					F_IGNORE},
	{"dmg",			FOFS(damage),		F_INT},
	{"angles",		FOFS(s.angles),		F_VECTOR},
	{"angle",		FOFS(s.angles),		F_ANGLEHACK},
	// JOSEPH 9-27-99
	{"duration",	FOFS(duration),		F_FLOAT},	
	{"rotate",		FOFS(rotate),		F_VECTOR},	
	// END JOSEPH
	{"degrees",		FOFS(angle),		F_FLOAT},
	{"time",		FOFS(speed),		F_FLOAT},

	//----(SA) additional ai field
	{"skin",		FOFS(aiSkin),		F_LSTRING},
	
	//----(SA) done

	// (SA) dlight lightstyles (made all these unique variables for testing)
	{"_color",		FOFS(dl_color),		F_VECTOR},		// color of the light	(the underscore is inserted by the color picker in QER)
	{"color",		FOFS(dl_color),		F_VECTOR},		// color of the light
	{"stylestring",	FOFS(dl_stylestring), F_LSTRING},	// user defined stylestring "fffndlsfaaaaaa" for example
	// done

	//----(SA)	
	{"shader",		FOFS(dl_shader), F_LSTRING},	// shader to use for a target_effect or dlight

	// (SA) for target_unlock
	{"key",			FOFS(key),		F_INT},
	// done

	// Rafael - mg42
	{"harc",		FOFS(harc),			F_FLOAT},
	{"varc",		FOFS(varc),			F_FLOAT},
	// done.

	// Rafael - sniper
	{"delay",	FOFS(delay),		F_FLOAT},
	{"radius",	FOFS(radius),		F_INT},

	// Ridah, for reloading savegames at correct mission spot
	{"missionlevel",	FOFS(missionLevel),	F_INT},

	// Rafel
	{"start_size", FOFS (start_size), F_INT},
	{"end_size", FOFS (end_size), F_INT},

	{"shard", FOFS (count), F_INT},

	// Rafael
	{"spawnitem",		FOFS(spawnitem),			F_LSTRING},

	{"track",			FOFS(track),				F_LSTRING},

	{"scriptName",		FOFS(scriptName),			F_LSTRING},

	{"shortname",		FOFS(message),				F_LSTRING},
	{"constages",		FOFS(constages),			F_LSTRING},
	{"desstages",		FOFS(desstages),			F_LSTRING},
	{"partofstage",		FOFS(partofstage),			F_INT},
	{"override",		FOFS(spawnitem),			F_LSTRING},

	{"damageparent",	FOFS(damageparent),			F_LSTRING},

	{"scale",		FOFS(scale),			F_FLOAT},

	{NULL}
};


typedef struct {
	char	*name;
	void	(*spawn)(gentity_t *ent);
} spawn_t;

void SP_info_player_start (gentity_t *ent);
void SP_info_player_checkpoint (gentity_t *ent);
void SP_info_player_deathmatch (gentity_t *ent);
void SP_info_player_intermission (gentity_t *ent);
void SP_info_firstplace(gentity_t *ent);
void SP_info_secondplace(gentity_t *ent);
void SP_info_thirdplace(gentity_t *ent);
void SP_info_podium(gentity_t *ent);

void SP_func_plat (gentity_t *ent);
void SP_func_static (gentity_t *ent);
void SP_func_leaky(gentity_t *ent);	//----(SA)	added
void SP_func_rotating (gentity_t *ent);
void SP_func_bobbing (gentity_t *ent);
void SP_func_pendulum( gentity_t *ent );
void SP_func_button (gentity_t *ent);
void SP_func_explosive (gentity_t *ent);
void SP_func_door (gentity_t *ent);
void SP_func_train (gentity_t *ent);
void SP_func_timer (gentity_t *self);
// JOSEPH 1-26-00
void SP_func_train_rotating (gentity_t *ent);
void SP_func_secret (gentity_t *ent);
// END JOSEPH
// Rafael
void SP_func_door_rotating (gentity_t *ent);
// RF
void SP_func_constructible( gentity_t *ent );
void SP_func_brushmodel( gentity_t *ent );
void SP_misc_constructiblemarker( gentity_t *ent );
void SP_target_explosion( gentity_t *ent );
void SP_misc_landmine( gentity_t *ent );

void SP_tank( gentity_t *ent );
void SP_panzer( gentity_t *ent );
void SP_panzeriv( gentity_t *ent );//new
void SP_stug3( gentity_t *ent );//new
void SP_kv1( gentity_t *ent );//new
void SP_t34( gentity_t *ent );
void SP_bt7( gentity_t *ent );
void SP_flak88( gentity_t *ent );//Artillery Masteries
void SP_pak( gentity_t *ent );
void SP_tank_heavy_assault (gentity_t *self);
void SP_tank_heavy (gentity_t *self);
void SP_tank_medium (gentity_t *self);
void SP_tank_light (gentity_t *self);
void SP_artillery (gentity_t *self);
void SP_flametank (gentity_t *self);//Unique1
void SP_tigertank (gentity_t *self);
void SP_axis_apc (gentity_t *self);
void SP_allied_apc (gentity_t *self);
void SP_panzer2 (gentity_t *self);

void SP_trigger_always (gentity_t *ent);
void SP_trigger_multiple (gentity_t *ent);
void SP_trigger_push (gentity_t *ent);
void SP_trigger_teleport (gentity_t *ent);
void SP_trigger_hurt (gentity_t *ent);

void SP_trigger_heal(gentity_t *ent);	// xkan,	9/17/2002
void SP_trigger_ammo(gentity_t *ent);	// xkan,	9/17/2002

// Gordon
void SP_misc_cabinet_health(gentity_t* self);
void SP_misc_cabinet_supply(gentity_t* self);

//---- (SA) Wolf triggers
void SP_trigger_concussive_dust(gentity_t *ent); // JPW NERVE
void SP_trigger_once	( gentity_t *ent );
//---- done

void SP_target_remove_powerups( gentity_t *ent );
void SP_target_give (gentity_t *ent);
void SP_target_delay (gentity_t *ent);
void SP_target_speaker (gentity_t *ent);
void SP_target_print (gentity_t *ent);
void SP_target_laser (gentity_t *self);
void SP_target_character (gentity_t *ent);
void SP_target_score( gentity_t *ent );
void SP_target_teleporter( gentity_t *ent );
void SP_target_relay (gentity_t *ent);
void SP_target_kill (gentity_t *ent);
void SP_target_position (gentity_t *ent);
void SP_target_location (gentity_t *ent);
void SP_target_push (gentity_t *ent);
void SP_target_script_trigger (gentity_t *ent);
void SP_misc_beam( gentity_t *self );

//---- (SA) Wolf targets
// targets
void SP_target_alarm		( gentity_t *ent );
void SP_target_counter		( gentity_t *ent );
void SP_target_lock			( gentity_t *ent );
void SP_target_effect		( gentity_t *ent );
void SP_target_fog			( gentity_t *ent );
void SP_target_autosave		( gentity_t *ent );

// entity visibility dummy
void SP_misc_vis_dummy	(gentity_t *ent);
void SP_misc_vis_dummy_multiple	(gentity_t *ent);

//----(SA) done

void SP_light (gentity_t *self);
void SP_info_null (gentity_t *self);
void SP_info_notnull (gentity_t *self);
void SP_info_camp (gentity_t *self);
void SP_path_corner (gentity_t *self);
void SP_path_corner_2 (gentity_t *self);
void SP_info_limbo_camera (gentity_t *self);
void SP_info_train_spline_main (gentity_t *self);
//void SP_bazooka (gentity_t *self);

void SP_misc_teleporter_dest (gentity_t *self);
void SP_misc_model(gentity_t *ent);
void SP_misc_gamemodel(gentity_t *ent);
void SP_misc_portal_camera(gentity_t *ent);
void SP_misc_portal_surface(gentity_t *ent);
void SP_misc_light_surface(gentity_t *ent);
void SP_misc_grabber_trap(gentity_t *ent);
void SP_misc_spotlight(gentity_t *ent);	//----(SA)	added

void SP_misc_commandmap_marker( gentity_t *ent );

void SP_shooter_rocket( gentity_t *ent );
void SP_shooter_grenade( gentity_t *ent );

void SP_team_CTF_redplayer( gentity_t *ent );
void SP_team_CTF_blueplayer( gentity_t *ent );

void SP_team_CTF_redspawn( gentity_t *ent );
void SP_team_CTF_bluespawn( gentity_t *ent );

// JPW NERVE for multiplayer spawnpoint selection
void SP_team_WOLF_objective( gentity_t *ent );
// jpw

void SP_team_WOLF_checkpoint( gentity_t *ent );		// DHM - Nerve

// JOSEPH 1-18-00
void SP_props_box_32 (gentity_t *self);
void SP_props_box_48 (gentity_t *self);
void SP_props_box_64 (gentity_t *self);
// END JOSEPH

// Ridah
#ifdef __NPC__
void SP_ai_soldier( gentity_t *ent );
void SP_ai_american( gentity_t *ent );
void SP_ai_zombie( gentity_t *ent );
void SP_ai_warzombie( gentity_t *ent );
void SP_ai_femzombie( gentity_t *ent );
void SP_ai_undead( gentity_t *ent );
void SP_ai_marker( gentity_t *ent );
void SP_ai_effect( gentity_t *ent );
void SP_ai_trigger( gentity_t *ent );
void SP_ai_venom( gentity_t *ent );
void SP_ai_loper( gentity_t *ent );
void SP_ai_sealoper( gentity_t *ent );
void SP_ai_boss_helga( gentity_t *ent );
void SP_ai_boss_heinrich( gentity_t *ent );	//----(SA)	added
void SP_ai_eliteguard( gentity_t *ent );
void SP_ai_stimsoldier_dual( gentity_t *ent );
void SP_ai_stimsoldier_rocket( gentity_t *ent );
void SP_ai_stimsoldier_tesla( gentity_t *ent );
void SP_ai_supersoldier( gentity_t *ent );
void SP_ai_blackguard( gentity_t *ent );
void SP_ai_protosoldier( gentity_t *ent );
void SP_ai_rejectxcreature( gentity_t *ent );
void SP_ai_frogman( gentity_t *ent );
void SP_ai_partisan( gentity_t *ent );
void SP_ai_civilian( gentity_t *ent );
void SP_ai_chimp( gentity_t *ent );	//----(SA)	added
void NPC_CoopSpawn( gentity_t *NPC );
#endif //__NPC__
// done.

// Rafael particles
void SP_target_smoke (gentity_t *ent);
// done.

// (SA) dlights
void SP_dlight(gentity_t *ent);
// done
void SP_corona(gentity_t *ent);

void SP_mg42 (gentity_t *ent);
void SP_aagun (gentity_t *ent);

//----(SA)	
//void SP_shooter_zombiespit (gentity_t *ent);
void SP_shooter_mortar (gentity_t *ent);

// alarm
void SP_alarm_box(gentity_t *ent);
//----(SA)	end

void SP_trigger_flagonly( gentity_t *ent );		// DHM - Nerve
void SP_trigger_flagonly_multiple( gentity_t *ent );		// DHM - Nerve
void SP_trigger_objective_info( gentity_t *ent );	// DHM - Nerve

void SP_gas (gentity_t *ent);
void SP_target_rumble (gentity_t *ent);

// Mad Doc - TDF
// put this back in for single player bots
void SP_trigger_aidoor (gentity_t *ent);

// Rafael
//void SP_trigger_aidoor (gentity_t *ent);
void SP_SmokeDust (gentity_t *ent);
void SP_Dust (gentity_t *ent);
void SP_props_sparks (gentity_t *ent);
void SP_props_gunsparks (gentity_t *ent);

// Props
void SP_Props_Bench (gentity_t *ent);
void SP_Props_Radio (gentity_t *ent);
void SP_Props_Chair (gentity_t *ent);
void SP_Props_ChairHiback (gentity_t *ent);
void SP_Props_ChairSide (gentity_t *ent);
void SP_Props_ChairChat (gentity_t *ent);
void SP_Props_ChairChatArm (gentity_t *ent);
void SP_Props_DamageInflictor (gentity_t *ent);
void SP_Props_Locker_Tall (gentity_t *ent);
void SP_Props_Desklamp (gentity_t *ent);
void SP_Props_Flamebarrel (gentity_t *ent);
void SP_crate_64 (gentity_t *ent);
void SP_Props_Flipping_Table (gentity_t *ent);
void SP_crate_32 (gentity_t *self);
void SP_Props_Crate32x64 (gentity_t *ent);
void SP_Props_58x112tablew (gentity_t *ent);
void SP_Props_RadioSEVEN (gentity_t *ent);
//void SP_propsFireColumn (gentity_t *ent);
void SP_props_flamethrower (gentity_t *ent);

void SP_func_invisible_user (gentity_t *ent);

void SP_lightJunior( gentity_t *self );

//void SP_props_me109 (gentity_t *ent);
void SP_misc_flak (gentity_t *ent);

void SP_props_snowGenerator (gentity_t *ent);

void SP_props_decoration (gentity_t *ent);
void SP_props_decorBRUSH (gentity_t *ent);
void SP_props_statue (gentity_t *ent);
void SP_props_statueBRUSH (gentity_t *ent);
void SP_skyportal (gentity_t *ent);

// RF, scripting
void SP_script_model_med(gentity_t *ent);
void SP_script_mover(gentity_t *ent);
void SP_script_multiplayer(gentity_t *ent);			// DHM - Nerve

void SP_props_footlocker (gentity_t *self);
void SP_misc_firetrails (gentity_t *ent);
void SP_trigger_deathCheck (gentity_t *ent);
void SP_misc_spawner (gentity_t *ent);
void SP_props_decor_Scale (gentity_t *ent);

void SP_bot_landminespot_spot( gentity_t *ent );
void SP_bot_sniper_spot( gentity_t *ent );
void SP_bot_attractor( gentity_t *ent );
void SP_bot_seek_cover_spot( gentity_t *ent );
void SP_bot_seek_cover_sequence( gentity_t *ent );
void SP_bot_axis_seek_cover_spot( gentity_t *ent );
void SP_bot_jump_source( gentity_t *ent );
void SP_bot_jump_dest( gentity_t *ent );
void SP_bot_landmine_area( gentity_t *ent );

// UQ1: Bot defence points..
void SP_bot_allied_defence_point( gentity_t *ent );
void SP_bot_axis_defence_point( gentity_t *ent );

void SP_ai_marker( gentity_t *ent );

// Gordon: debris test
void SP_func_debris( gentity_t* ent );
// ===================

#ifdef __ETE__
// Unique1: OM Flag system spawns...
void OM_Flag_Spawn ( gentity_t* ent );
void OM_Associated_Spawnarea ( gentity_t* ent );
void OM_Unassociated_Spawnarea ( gentity_t* ent );
void SP_ammo_crate_spawn ( gentity_t* ent );
void SP_health_crate_spawn ( gentity_t* ent );
void SP_sandbags_spawn ( gentity_t* ent );
void SP_Stealth_Area( gentity_t *ent );
#endif //__ETE__

#ifdef __PARTICLE_SYSTEM__
void	SP_misc_particle_system ( gentity_t *ent );
#endif //__PARTICLE_SYSTEM__

#ifdef __BOT__

void SP_AddBot ( gentity_t *ent );
void SP_AddAlliedBot ( gentity_t *ent );

#ifndef __NPC__
void SP_AddNPC ( gentity_t *ent );
#endif //__NPC__

void SP_Axis_Cover_Spot ( gentity_t *ent );
void SP_Allied_Cover_Spot ( gentity_t *ent );

#ifdef __NPC_NEW_COOP__
void SP_spawnpoint ( gentity_t *ent );
#endif //__NPC_NEW_COOP__
#endif //__BOT__

#ifdef __NPC__
void NPC_CinematicPlane_Enter ( gentity_t *ent );
void NPC_CinematicPlane_Exit ( gentity_t *ent );
#endif //__NPC__

spawn_t	spawns[] = {
	// info entities don't do anything at all, but provide positional
	// information for things controlled by other processes
	{"info_player_start", SP_info_player_start},
	{"info_player_checkpoint", SP_info_player_checkpoint},
	{"info_player_deathmatch", SP_info_player_deathmatch},
	{"info_player_intermission", SP_info_player_intermission},
	{"info_null", SP_info_null},
	{"info_notnull", SP_info_notnull},			// use target_position instead
	{"info_notnull_big", SP_info_notnull},		// use target_position instead
	{"info_camp", SP_info_camp},

	// Gordon: debris test
	{"func_debris",					SP_func_debris				},
	// ===================

	{"func_plat", SP_func_plat},
	{"func_button", SP_func_button},
	{"func_explosive", SP_func_explosive},
	{"func_door", SP_func_door},
	{"func_static", SP_func_static},
	{"func_leaky", SP_func_leaky},
	{"func_rotating", SP_func_rotating},
	{"func_bobbing", SP_func_bobbing},
	{"func_pendulum", SP_func_pendulum},
	{"func_train", SP_func_train},
	{"func_group", SP_info_null},
	// JOSEPH 1-26-00
	{"func_train_rotating", SP_func_train_rotating},
	{"func_secret", SP_func_secret},
	// END JOSEPH
	// Rafael
	{"func_door_rotating", SP_func_door_rotating},

	{"func_timer", SP_func_timer},			// rename trigger_timer?

	{"func_invisible_user", SP_func_invisible_user},

	// Triggers are brush objects that cause an effect when contacted
	// by a living player, usually involving firing targets.
	// While almost everything could be done with
	// a single trigger class and different targets, triggered effects
	// could not be client side predicted (push and teleport).
	{"trigger_always", SP_trigger_always},
	{"trigger_multiple", SP_trigger_multiple},
	{"trigger_push", SP_trigger_push},
	{"trigger_teleport", SP_trigger_teleport},
	{"trigger_hurt", SP_trigger_hurt},

	//---- (SA) Wolf triggers
	{"trigger_concussive_dust", SP_trigger_concussive_dust}, // JPW NERVE
	{"trigger_once",		SP_trigger_once},
	//---- done

	// Mad Doc - TDf
	// I'm going to put trigger_aidoors back in. I'll make sure they only work in single player
	{"trigger_aidoor", SP_trigger_aidoor},
	// START	xkan,	9/17/2002
	{"trigger_heal", SP_trigger_heal},
	{"trigger_ammo", SP_trigger_ammo},
	// END		xkan,	9/17/2002
	
	// Gordon: 16/12/02: adding the model things to go with the triggers
	{"misc_cabinet_health", SP_misc_cabinet_health},
	{"misc_cabinet_supply", SP_misc_cabinet_supply},
	// end


	// Rafael
//	{"trigger_aidoor", SP_trigger_aidoor},
//	{"trigger_deathCheck",SP_trigger_deathCheck},
	
	// targets perform no action by themselves, but must be triggered
	// by another entity
	{"target_give", SP_target_give},
	{"target_remove_powerups", SP_target_remove_powerups},
	{"target_delay", SP_target_delay},
	{"target_speaker", SP_target_speaker},
	{"target_print", SP_target_print},
	{"target_laser", SP_target_laser},
	{"target_score", SP_target_score},
	{"target_teleporter", SP_target_teleporter},
	{"target_relay", SP_target_relay},
	{"target_kill", SP_target_kill},
	{"target_position", SP_target_position},
	{"target_location", SP_target_location},
	{"target_push", SP_target_push},
	{"target_script_trigger", SP_target_script_trigger},

	//---- (SA) Wolf targets
	{"target_alarm",		SP_target_alarm},
	{"target_counter",		SP_target_counter},
	{"target_lock",			SP_target_lock},
	{"target_effect",		SP_target_effect},
	{"target_fog",			SP_target_fog},
	{"target_autosave",		SP_target_autosave},	//----(SA)	added
	//---- done

	{"target_rumble", SP_target_rumble},


	{"light", SP_light},
	
	{"lightJunior", SP_lightJunior}, 

	{"path_corner",					SP_path_corner},
	{"path_corner_2",				SP_path_corner_2},

	{"info_train_spline_main",		SP_info_train_spline_main},
	{"info_train_spline_control",	SP_path_corner_2},
	{"info_limbo_camera",			SP_info_limbo_camera},

	{"misc_teleporter_dest", SP_misc_teleporter_dest},
	{"misc_model", SP_misc_model},
	{"misc_gamemodel", SP_misc_gamemodel},
	{"misc_portal_surface", SP_misc_portal_surface},
	{"misc_portal_camera", SP_misc_portal_camera},

	{"misc_commandmap_marker", SP_misc_commandmap_marker},

	{"misc_vis_dummy",		SP_misc_vis_dummy},
	{"misc_vis_dummy_multiple",		SP_misc_vis_dummy_multiple},
	{"misc_light_surface",	SP_misc_light_surface},
	{"misc_grabber_trap",	SP_misc_grabber_trap},
	{"misc_spotlight",		SP_misc_spotlight},


	{"misc_mg42", SP_mg42},
	{"misc_aagun", SP_aagun},

	{"misc_flak", SP_misc_flak},
	{"misc_firetrails", SP_misc_firetrails},

	{"shooter_rocket", SP_shooter_rocket},
	{"shooter_grenade", SP_shooter_grenade},

	{"shooter_mortar", SP_shooter_mortar},
	{"alarm_box", SP_alarm_box},

	// Gordon: FIXME remove
//	{"team_CTF_redplayer", SP_team_CTF_redplayer},
//	{"team_CTF_blueplayer", SP_team_CTF_blueplayer},
	{"team_CTF_redplayer", SP_team_CTF_redspawn},
	{"team_CTF_blueplayer", SP_team_CTF_bluespawn},

	{"team_CTF_redspawn", SP_team_CTF_redspawn},
	{"team_CTF_bluespawn", SP_team_CTF_bluespawn},

	{"team_WOLF_objective", SP_team_WOLF_objective},

	{"team_WOLF_checkpoint", SP_team_WOLF_checkpoint},

	{"target_smoke", SP_target_smoke},

	{"misc_spawner", SP_misc_spawner},

	{"props_box_32", SP_props_box_32},
	{"props_box_48", SP_props_box_48},
	{"props_box_64", SP_props_box_64},

	{"props_smokedust", SP_SmokeDust},
	{"props_dust", SP_Dust},
	{"props_sparks", SP_props_sparks},
	{"props_gunsparks", SP_props_gunsparks},

	{"props_bench", SP_Props_Bench},
	{"props_radio", SP_Props_Radio},
	{"props_chair", SP_Props_Chair},
	{"props_chair_hiback", SP_Props_ChairHiback},
	{"props_chair_side", SP_Props_ChairSide},
	{"props_chair_chat", SP_Props_ChairChat},
	{"props_chair_chatarm", SP_Props_ChairChatArm},
	{"props_damageinflictor", SP_Props_DamageInflictor},
	{"props_locker_tall",SP_Props_Locker_Tall},
	{"props_desklamp", SP_Props_Desklamp},
	{"props_flamebarrel", SP_Props_Flamebarrel},	
	{"props_crate_64", SP_crate_64},
	{"props_flippy_table", SP_Props_Flipping_Table},
	{"props_crate_32", SP_crate_32},
	{"props_crate_32x64", SP_Props_Crate32x64},
	{"props_58x112tablew", SP_Props_58x112tablew},
	{"props_radioSEVEN", SP_Props_RadioSEVEN},
	{"props_snowGenerator", SP_props_snowGenerator},
//	{"props_FireColumn", SP_propsFireColumn},
	{"props_decoration", SP_props_decoration},
	{"props_decorBRUSH", SP_props_decorBRUSH},
	{"props_statue", SP_props_statue},
	{"props_statueBRUSH", SP_props_statueBRUSH},
	{"props_skyportal", SP_skyportal},
	{"props_footlocker", SP_props_footlocker},
	{"props_flamethrower", SP_props_flamethrower},
	{"props_decoration_scale",SP_props_decor_Scale},

	{"dlight",		SP_dlight},

	{"corona",		SP_corona},

	{"trigger_flagonly",			SP_trigger_flagonly			},
	{"trigger_flagonly_multiple",	SP_trigger_flagonly_multiple},

	{"test_gas", SP_gas},
	{"trigger_objective_info", SP_trigger_objective_info},

	// RF, scripting
	{"script_model_med", SP_script_model_med},
	{"script_mover", SP_script_mover},
	{"script_multiplayer", SP_script_multiplayer},

	{"bot_landminespot_spot", SP_bot_landminespot_spot},
	{"bot_sniper_spot", SP_bot_sniper_spot},
	{"bot_attractor", SP_bot_attractor},
	{"bot_seek_cover_spot", SP_bot_seek_cover_spot},
	{"bot_seek_cover_sequence", SP_bot_seek_cover_sequence},
	{"bot_axis_seek_cover_spot", SP_bot_axis_seek_cover_spot},
	{"bot_jump_source", SP_bot_jump_source},
	{"bot_jump_dest", SP_bot_jump_dest},

	{"bot_allied_defence_point", SP_bot_allied_defence_point},
	{"bot_axis_defence_point", SP_bot_axis_defence_point},

	{"ai_marker", SP_ai_marker},
	{"bot_landmine_area", SP_bot_landmine_area},

	{"func_constructible",	SP_func_constructible},
	{"func_brushmodel",		SP_func_brushmodel},
	{"misc_beam",			SP_misc_beam},
	{"misc_constructiblemarker", SP_misc_constructiblemarker},
	{"target_explosion",	SP_target_explosion },
	{"misc_landmine",		SP_misc_landmine },
#ifdef __VEHICLES__
	{"misc_tank", SP_tank},//churchhill
	{"misc_panzer", SP_panzer},//jagpanther
	{"misc_panzeriv", SP_panzeriv},   //panzer iv(new)
    {"misc_stug3", SP_stug3},    //stug 3(new)
    {"misc_kv1", SP_kv1},        //kv 1(new)
    {"misc_flak88", SP_flak88},//Artillery
    {"misc_pak", SP_pak},
	{"misc_panzer2", SP_panzer2},
	{"misc_t34", SP_t34},
	{"misc_bt7", SP_bt7},
	{"npc_tank", SP_tank},//NPC�?
	{"npc_churchhill", SP_tank},//NPC�?
	{"npc_panzer", SP_panzer},//NPC�?
	{"npc_tank_heavy_assault", SP_tank_heavy_assault},
	{"npc_tank_heavy", SP_tank_heavy},
	{"npc_tank_medium", SP_tank_medium},
	{"npc_tank_light", SP_tank_light},
    {"npc_artillery", SP_artillery},
	{"npc_flametank", SP_flametank},
	{"npc_tiger", SP_tigertank},
	{"npc_axis_apc", SP_axis_apc},
	{"npc_allied_apc", SP_allied_apc},
	{"npc_russian_apc", SP_allied_apc},
	{"npc_panzer2", SP_panzer2},
#endif //__VEHICLES__
#ifdef __ETE__
	{"misc_control_point",	OM_Flag_Spawn },
	{"misc_associated_spawnarea",	OM_Associated_Spawnarea },
	{"misc_unassociated_spawnarea",	OM_Unassociated_Spawnarea },
	{"ammo_crate",	SP_ammo_crate_spawn },
	{"health_crate",	SP_health_crate_spawn },
	{"misc_sandbags",	SP_sandbags_spawn },
	{"misc_stealth_area",	SP_Stealth_Area },
#endif

#ifdef __BOT__
#ifndef __NPC_NEW_COOP__
	/* RTCW Single Player Spawners */
	{ "ai_zombie", SP_AddBot },
	{ "ai_warzombie", SP_AddBot },
	{ "ai_soldier", SP_AddBot },
	{ "ai_venom", SP_AddBot },
	{ "ai_loper", SP_AddBot },
	{ "ai_blackguard", SP_AddBot },
	{ "ai_eliteguard", SP_AddBot },
	{ "ai_boss_helga", SP_AddBot },
	{ "ai_boss_heinrich", SP_AddBot },
	{ "ai_protosoldier", SP_AddBot },
	{ "ai_supersoldier", SP_AddBot },
	{ "ai_stimsoldier_dual", SP_AddBot },
	{ "ai_stimsoldier_rocket", SP_AddBot },
	{ "ai_stimsoldier_tesla", SP_AddBot },
	{ "ai_frogman", SP_AddBot },
	{ "ai_partisan", SP_AddAlliedBot },
	{ "ai_civilian", SP_AddAlliedBot },
	{ "ai_american", SP_AddAlliedBot },
#endif //__NPC_NEW_COOP__
#ifndef __NPC__
#ifndef __NPC_NEW_COOP__
	{ "npc", SP_AddNPC },
#endif //__NPC_NEW_COOP__
#endif //__NPC__

	{ "bot_axis_cover_spot", SP_Axis_Cover_Spot },
	{ "bot_allied_cover_spot", SP_Allied_Cover_Spot },

	// Ridah
#ifdef __NPC__
#ifndef __NPC_NEW_COOP__
	{"ai_soldier", SP_ai_soldier},
	{"ai_american", SP_ai_american},
	{"ai_zombie", SP_ai_zombie},
	{"ai_warzombie", SP_ai_warzombie},
	{"ai_venom", SP_ai_venom},
	{"ai_loper", SP_ai_loper},
	{"ai_boss_helga", SP_ai_boss_helga},
	{"ai_boss_heinrich", SP_ai_boss_heinrich},	//----(SA)	
	{"ai_eliteguard", SP_ai_eliteguard},
	{"ai_stimsoldier_dual", SP_ai_stimsoldier_dual},
	{"ai_stimsoldier_rocket", SP_ai_stimsoldier_rocket},
	{"ai_stimsoldier_tesla", SP_ai_stimsoldier_tesla},
	{"ai_supersoldier", SP_ai_supersoldier},
	{"ai_protosoldier", SP_ai_protosoldier},
	{"ai_frogman", SP_ai_frogman},
	{"ai_blackguard", SP_ai_blackguard},
	{"ai_partisan", SP_ai_partisan},
	{"ai_civilian", SP_ai_civilian},
	{"npc", NPC_CoopSpawn },
#endif //__NPC_NEW_COOP__
#endif //__NPC__

#ifdef __NPC_NEW_COOP__
	{"ai_soldier", SP_spawnpoint},
	{"ai_american", SP_spawnpoint},
	{"ai_zombie", SP_spawnpoint},
	{"ai_warzombie", SP_spawnpoint},
	{"ai_venom", SP_spawnpoint},
	{"ai_loper", SP_spawnpoint},
	{"ai_boss_helga", SP_spawnpoint},
	{"ai_boss_heinrich", SP_spawnpoint},	//----(SA)	
	{"ai_eliteguard", SP_spawnpoint},
	{"ai_stimsoldier_dual", SP_spawnpoint},
	{"ai_stimsoldier_rocket", SP_spawnpoint},
	{"ai_stimsoldier_tesla", SP_spawnpoint},
	{"ai_supersoldier", SP_spawnpoint},
	{"ai_protosoldier", SP_spawnpoint},
	{"ai_frogman", SP_spawnpoint},
	{"ai_blackguard", SP_spawnpoint},
	{"ai_partisan", SP_spawnpoint},
	{"ai_civilian", SP_spawnpoint},
	{"npc", SP_spawnpoint },
#endif //__NPC_NEW_COOP__

#ifdef __NPC__
	{"npc_cinematicplane_enter", NPC_CinematicPlane_Enter },
	{"npc_cinematicplane_exit", NPC_CinematicPlane_Exit },
#endif //__NPC__
//	{"ai_marker", SP_ai_marker},
//	{"ai_effect", SP_ai_effect},
//	{"ai_trigger", SP_ai_trigger},
	// done.
#endif //__BOT__

#ifdef __PARTICLE_SYSTEM__
	{ "misc_particle_system", SP_misc_particle_system },
#endif //__PARTICLE_SYSTEM__

	{0, 0}
};

/*
===============
G_CallSpawn

Finds the spawn function for the entity and calls it,
returning qfalse if not found
===============
*/
qboolean G_CallSpawn( gentity_t *ent ) {
	spawn_t	*s;
	gitem_t	*item;

	if ( !ent->classname ) {
		G_Printf ("G_CallSpawn: NULL classname\n");
		return qfalse;
	}

	// check item spawn functions
	for ( item=bg_itemlist+1 ; item->classname ; item++ ) {
		if ( !strcmp(item->classname, ent->classname) ) {
			// found it
			if(g_gametype.integer != GT_WOLF_LMS) { // Gordon: lets not have items in last man standing for the moment
				G_SpawnItem( ent, item );

				G_Script_ScriptParse( ent );
				G_Script_ScriptEvent( ent, "spawn", "" );
			} else {
				return qfalse;
			}
			return qtrue;
		}
	}

	// check normal spawn functions
	for ( s=spawns ; s->name ; s++ ) {
		if ( !strcmp(s->name, ent->classname) ) {
			// found it
			s->spawn(ent);

			// RF, entity scripting
			if (/*ent->s.number >= MAX_CLIENTS &&*/ ent->scriptName) {
				G_Script_ScriptParse( ent);
				G_Script_ScriptEvent( ent, "spawn", "" );
			}

			return qtrue;
		}
	}
	G_Printf ("%s doesn't have a spawn function\n", ent->classname);
	return qfalse;
}

/*
=============
G_NewString

Builds a copy of the string, translating \n to real linefeeds
so message texts can be multi-line
=============
*/
char *G_NewString( const char *string ) {
	char	*newb, *new_p;
	int		i,l;
	
	l = strlen(string) + 1;

	newb = G_Alloc( l );

	new_p = newb;

	// turn \n into a real linefeed
	for ( i=0 ; i< l ; i++ ) {
		if (string[i] == '\\' && i < l-1) {
			i++;
			if (string[i] == 'n') {
				*new_p++ = '\n';
			} else {
				*new_p++ = '\\';
			}
		} else {
			*new_p++ = string[i];
		}
	}
	
	return newb;
}




/*
===============
G_ParseField

Takes a key/value pair and sets the binary values
in a gentity
===============
*/
void G_ParseField( const char *key, const char *value, gentity_t *ent ) {
	field_t	*f;
	byte	*b;
	float	v;
	vec3_t	vec;

	for ( f=fields ; f->name ; f++ ) {
		if ( !Q_stricmp(f->name, key) ) {
			// found it
			b = (byte *)ent;

			switch( f->type ) {
			case F_LSTRING:
				*(char **)(b+f->ofs) = G_NewString (value);
				break;
			case F_VECTOR:
				sscanf (value, "%f %f %f", &vec[0], &vec[1], &vec[2]);
				((float *)(b+f->ofs))[0] = vec[0];
				((float *)(b+f->ofs))[1] = vec[1];
				((float *)(b+f->ofs))[2] = vec[2];
				break;
			case F_INT:
				*(int *)(b+f->ofs) = atoi(value);
				break;
			case F_FLOAT:
				*(float *)(b+f->ofs) = atof(value);
				break;
			case F_ANGLEHACK:
				v = atof(value);
				((float *)(b+f->ofs))[0] = 0;
				((float *)(b+f->ofs))[1] = v;
				((float *)(b+f->ofs))[2] = 0;
				break;
			default:
			case F_IGNORE:
				break;
			}
			return;
		}
	}
}




/*
===================
G_SpawnGEntityFromSpawnVars

Spawn an entity and fill in all of the level fields from
level.spawnVars[], then call the class specfic spawn function
===================
*/
gentity_t *G_SpawnGEntityFromSpawnVars( void ) {
	int			i;
	gentity_t	*ent;
	char		*str;

	// get the next free entity
	ent = G_Spawn();

	for ( i = 0 ; i < level.numSpawnVars ; i++ ) {
		G_ParseField( level.spawnVars[i][0], level.spawnVars[i][1], ent );
	}

	// check for "notteam" / "notfree" flags
	G_SpawnInt( "notteam", "0", &i );
	if ( i ) {
		G_FreeEntity( ent );
		//return;
		return NULL;
	}

	// allowteams handling
	G_SpawnString( "allowteams", "", &str );
	if( str[0] ) {
		str = Q_strlwr( str );
		if( strstr( str, "axis" ) ) {
			ent->allowteams |= ALLOW_AXIS_TEAM;
		}
		if( strstr( str, "allies" ) ) {
			ent->allowteams |= ALLOW_ALLIED_TEAM;
		}
		if( strstr( str, "cvops" ) ) {
			ent->allowteams |= ALLOW_DISGUISED_CVOPS;
		}
	}

	if( ent->targetname && *ent->targetname ) {
		ent->targetnamehash = BG_StringHashValue( ent->targetname );
	} else {
		ent->targetnamehash = -1;
	}

	// move editor origin to pos
	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	VectorCopy( ent->s.origin, ent->r.currentOrigin );

	// if we didn't get a classname, don't bother spawning anything
	if ( !G_CallSpawn( ent ) ) {
		G_FreeEntity( ent );
	}


	return ent;
}




/*
====================
G_AddSpawnVarToken
====================
*/
char *G_AddSpawnVarToken( const char *string ) {
	int		l;
	char	*dest;

	l = strlen( string );
	if ( level.numSpawnVarChars + l + 1 > MAX_SPAWN_VARS_CHARS ) {
		G_Error( "G_AddSpawnVarToken: MAX_SPAWN_VARS" );
	}

	dest = level.spawnVarChars + level.numSpawnVarChars;
	memcpy( dest, string, l+1 );

	level.numSpawnVarChars += l + 1;

	return dest;
}

/*
====================
G_ParseSpawnVars

Parses a brace bounded set of key / value pairs out of the
level's entity strings into level.spawnVars[]

This does not actually spawn an entity.
====================
*/
qboolean G_ParseSpawnVars( void ) {
	char		keyname[MAX_TOKEN_CHARS];
	char		com_token[MAX_TOKEN_CHARS];

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

	// parse the opening brace
	if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) {
		// end of spawn string
		return qfalse;
	}
	if ( com_token[0] != '{' ) {
		G_Error( "G_ParseSpawnVars: found %s when expecting {",com_token );
	}

	// go through all the key / value pairs
	while ( 1 ) {	
		// parse key
		if ( !trap_GetEntityToken( keyname, sizeof( keyname ) ) ) {
			G_Error( "G_ParseSpawnVars: EOF without closing brace" );
		}

		if ( keyname[0] == '}' ) {
			break;
		}
		
		// parse value	
		if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) {
			G_Error( "G_ParseSpawnVars: EOF without closing brace" );
		}

		if ( com_token[0] == '}' ) {
			G_Error( "G_ParseSpawnVars: closing brace without data" );
		}
		if ( level.numSpawnVars == MAX_SPAWN_VARS ) {
			G_Error( "G_ParseSpawnVars: MAX_SPAWN_VARS" );
		}
		level.spawnVars[ level.numSpawnVars ][0] = G_AddSpawnVarToken( keyname );
		level.spawnVars[ level.numSpawnVars ][1] = G_AddSpawnVarToken( com_token );
		level.numSpawnVars++;
	}

	return qtrue;
}

/*
====================
G_ParseSpawnVarsEx

Parses a brace bounded set of key / value pairs out of the
level's entity strings into level.spawnVars[]

This does not actually spawn an entity.
====================
*/
qboolean G_ParseSpawnVarsEx( int handle ) {
	pc_token_t	token;
	char		keyname[MAX_TOKEN_CHARS];
	
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

	// parse the opening brace
	if (trap_PC_ReadToken(handle, &token) == 0)
		// end of spawn string
		return qfalse;

	if (Q_stricmp(token.string, "{") != 0)
		G_Error( "G_ParseSpawnVarsEx: found %s when expecting {", token.string );

	// go through all the key / value pairs
	while ( 1 ) {	
		// parse key
		if (trap_PC_ReadToken( handle, &token) == 0)
			G_Error( "G_ParseSpawnVarsEx: EOF without closing brace" );

		if (Q_stricmp(token.string, "}") == 0)
			break;
		
		strcpy(keyname, token.string);

		// parse value	
		if (trap_PC_ReadToken( handle, &token) == 0)
			G_Error("G_ParseSpawnVarsEx: EOF without closing brace" );

		if (Q_stricmp(token.string, "}") == 0)
			G_Error("G_ParseSpawnVarsEx: closing brace without data");

		if (level.numSpawnVars == MAX_SPAWN_VARS)
			G_Error("G_ParseSpawnVarsEx: MAX_SPAWN_VARS");

		level.spawnVars[ level.numSpawnVars ][0] = G_AddSpawnVarToken(keyname);
		level.spawnVars[ level.numSpawnVars ][1] = G_AddSpawnVarToken(token.string);
		level.numSpawnVars++;
	}

	return qtrue;
}

extern vmCvar_t fs_game;

/* */
/*qboolean
AIMOD_LoadMapCoordFile ( void )
{// UQ1: Fallback time! ;)
	fileHandle_t	f;
	vmCvar_t		mapname;

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	trap_FS_FOpenFile( va( "maps/%s.mcf", mapname.string), &f, FS_READ );
	if ( !f )
	{
		G_Printf( "^1*** ^3SERVER^5: Reading from ^7maps/%s.mcf^3 failed^5!!!\n", mapname.string );
		return qfalse;
	}

	trap_FS_Read( &level.mapcoordsMins, sizeof(vec3_t), f );
	trap_FS_Read( &level.mapcoordsMaxs, sizeof(vec3_t), f );

	trap_FS_FCloseFile( f );							//close the file

	G_Printf( "^1*** ^3SERVER^5: Map coordinates loaded from ^7maps/%s.mcf^5.\n", mapname.string );

	return qtrue;
}*/



/*QUAKED worldspawn (0 0 0) ? NO_GT_WOLF NO_GT_STOPWATCH NO_GT_CHECKPOINT NO_LMS

Every map should have exactly one worldspawn.
"music"     Music wav file
"gravity"   800 is default gravity
"message" Text to print during connection process
"ambient"  Ambient light value (must use '_color')
"_color"    Ambient light color (must be used with 'ambient')
"sun"        Shader to use for 'sun' image
*/

extern qboolean AIMOD_LoadMapCoordFile ( void );
extern void AIMOD_SaveMapCoordFile ( void );
extern void AIMod_GetMapBounts ( vec3_t mapMins, vec3_t mapMaxs );

void SP_worldspawn( void ) {
	char	*s;

	G_SpawnString( "classname", "", &s );
	if ( Q_stricmp( s, "worldspawn" ) ) {
		G_Error( "SP_worldspawn: The first entity isn't 'worldspawn'" );
	}

	// make some data visible to connecting client
	trap_SetConfigstring( CS_GAME_VERSION, GAME_VERSION );

	trap_SetConfigstring( CS_LEVEL_START_TIME, va("%i", level.startTime ) );

	G_SpawnString( "music", "", &s );
	trap_SetConfigstring( CS_MUSIC, s );

	G_SpawnString( "message", "", &s );
	trap_SetConfigstring( CS_MESSAGE, s );				// map specific message

	G_SpawnString( "cclayers", "0", &s );
	if( atoi(s) )
		level.ccLayers = qtrue;

	level.mapcoordsValid = qfalse;

/*	if (AIMOD_LoadMapCoordFile())
	{
		if( G_SpawnVector2D( "mapcoordsmins", "-128 128", level.mapcoordsMins ) &&	// top left
			G_SpawnVector2D( "mapcoordsmaxs", "128 -128", level.mapcoordsMaxs ) ) 
		{	// bottom right
			level.mapcoordsValid = qtrue;
			G_Printf("Max coord loaded and valid!\n");
		}
	}*/
	if( G_SpawnVector2D( "mapcoordsmins", "-128 128", level.mapcoordsMins ) &&	// top left
		G_SpawnVector2D( "mapcoordsmaxs", "128 -128", level.mapcoordsMaxs ) ) {	// bottom right
		level.mapcoordsValid = qtrue;
	}

	if( !level.mapcoordsValid ) {
		if (!AIMOD_LoadMapCoordFile())
		{// Try loading from a file, or auto-generating them!
			// These start off reversed so that we can set them using traces...
			vec3_t mapMins, mapMaxs;//, mins, maxs;
			VectorSet( mapMins, MAX_MAP_SIZE, MAX_MAP_SIZE, MAX_MAP_SIZE );
			VectorSet( mapMaxs, -MAX_MAP_SIZE, -MAX_MAP_SIZE, -MAX_MAP_SIZE );
			G_Printf( va( "Finding map bounds.\n") );
			AIMod_GetMapBounts( mapMins, mapMaxs );
			VectorCopy(mapMins, level.mapcoordsMins);
			VectorCopy(mapMaxs, level.mapcoordsMaxs);
			level.mapcoordsValid = qtrue;

			if( G_SpawnVector2D( "mapcoordsmins", "-128 128", level.mapcoordsMins ) &&	// top left
				G_SpawnVector2D( "mapcoordsmaxs", "128 -128", level.mapcoordsMaxs ) ) 
			{	// bottom right
				level.mapcoordsValid = qtrue;
				//G_Printf("Max coord loaded and valid!\n");
			}
		}
		else
		{
			if( G_SpawnVector2D( "mapcoordsmins", "-128 128", level.mapcoordsMins ) &&	// top left
				G_SpawnVector2D( "mapcoordsmaxs", "128 -128", level.mapcoordsMaxs ) ) 
			{	// bottom right
				level.mapcoordsValid = qtrue;
				//G_Printf("Max coord loaded and valid!\n");
			}
		}
	}

	BG_InitLocations( level.mapcoordsMins, level.mapcoordsMaxs );
	BG_InitLocationNames( level.rawmapname );

	trap_SetConfigstring( CS_MOTD, g_motd.string );		// message of the day

	G_SpawnString( "gravity", "800", &s );
	trap_Cvar_Set( "g_gravity", s );

	G_SpawnString( "spawnflags", "0", &s );
	g_entities[ENTITYNUM_WORLD].spawnflags = atoi( s );
	g_entities[ENTITYNUM_WORLD].r.worldflags = g_entities[ENTITYNUM_WORLD].spawnflags;

	g_entities[ENTITYNUM_WORLD].s.number = ENTITYNUM_WORLD;
	g_entities[ENTITYNUM_WORLD].classname = "worldspawn";

	// see if we want a warmup time
	trap_SetConfigstring( CS_WARMUP, "" );
	if ( g_restarted.integer ) {
		trap_Cvar_Set( "g_restarted", "0" );
		level.warmupTime = 0;
	}

	if(g_gamestate.integer == GS_PLAYING) {
		G_initMatch();
	}
}


/*
==============
G_SpawnEntitiesFromString

Parses textual entity definitions out of an entstring and spawns gentities.
==============
*/
void G_SpawnEntitiesFromString( void ) {
#ifdef __ETE__
	int			handle;
#else //!__ETE__
#ifdef __ENTITY_OVERRIDES__
	int			handle;
#endif //__ENTITY_OVERRIDES__
#endif //__ETE__
	// allow calls to G_Spawn*()
	G_Printf( "Enable spawning!\n" );
	level.spawning = qtrue;
	level.numSpawnVars = 0;

	// the worldspawn is not an actual entity, but it still
	// has a "spawn" function to perform any global setup
	// needed by a level (setting configstrings or cvars, etc)
#ifdef __ETE__
	if (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
		handle = trap_PC_LoadSource(va("maps/%s_supremacy.ovrents", level.rawmapname));
	else
	if (g_gametype.integer == GT_WOLF_LMS)
		handle = trap_PC_LoadSource(va("maps/%s_lms.ovrents", level.rawmapname));
	else if (g_gametype.integer == GT_COOP || g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_NEWCOOP)
		handle = trap_PC_LoadSource(va("maps/%s_coop.ovrents", level.rawmapname));
	else
		handle = trap_PC_LoadSource(va("maps/%s.ovrents", level.rawmapname));

	if (handle)
	{
		if ( !G_ParseSpawnVarsEx(handle) )
		{
			G_Error( "SpawnEntities: no entities" );
			trap_PC_FreeSource(handle); // UQ added!
		}

		//trap_PC_FreeSource(handle); // UQ removed! See below...
	}
	else
#else //__ETE__
#ifdef __ENTITY_OVERRIDES__
	// Allows overload entities defintions instead use included into bsp
	if (g_gametype.integer == GT_WOLF_LMS)
		handle = trap_PC_LoadSource(va("maps/%s_lms.ovrents", level.rawmapname));
	else if (g_gametype.integer == GT_COOP || g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_NEWCOOP)
		handle = trap_PC_LoadSource(va("maps/%s_coop.ovrents", level.rawmapname));
	else
		handle = trap_PC_LoadSource(va("maps/%s.ovrents", level.rawmapname));

	if (handle)
	{
		if ( !G_ParseSpawnVarsEx(handle) )
		{
			G_Error( "SpawnEntities: no entities" );
			trap_PC_FreeSource(handle); // UQ added!
		}

		//trap_PC_FreeSource(handle); // UQ removed! See below...
	}
	else
#endif //__ENTITY_OVERRIDES__
#endif //__ETE__
	if ( !G_ParseSpawnVars() ) {
		G_Error( "SpawnEntities: no entities" );
	}
	SP_worldspawn();

#ifdef __ETE__ // UQ added.. We need to pass all the new entity list, not just worldspawn...
	if (handle)
	{// Pass all the new .ovrents file's entities one at a time...
		while( G_ParseSpawnVarsEx(handle) )
			G_SpawnGEntityFromSpawnVars();

		trap_PC_FreeSource(handle); // UQ: Release the handle here instead of above...
	}
	else
#else //!__ETE__
#ifdef __ENTITY_OVERRIDES__ // UQ added.. We need to pass all the new entity list, not just worldspawn...
	if (handle)
	{// Pass all the new .ovrents file's entities one at a time...
		while( G_ParseSpawnVarsEx(handle) )
			G_SpawnGEntityFromSpawnVars();

		trap_PC_FreeSource(handle); // UQ: Release the handle here instead of above...
	}
	else
#endif //__ENTITY_OVERRIDES__
#endif //__ETE__
	{// Parse ents from the actual map bsp.
		while( G_ParseSpawnVars() ) {
			G_SpawnGEntityFromSpawnVars();
		}	
	}


#ifdef __ETE__
	if (!handle) 
	{
		// parse possible external entities map files
		// it's used to add new ents to existing pure ET map
		if (g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
			handle = trap_PC_LoadSource(va("maps/%s_supremacy.entities", level.rawmapname));
		else if (g_gametype.integer == GT_WOLF_LMS)
			handle = trap_PC_LoadSource(va("maps/%s_lms.entities", level.rawmapname));
		else if (g_gametype.integer == GT_COOP || g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_NEWCOOP)
			handle = trap_PC_LoadSource(va("maps/%s_coop.entities", level.rawmapname));
		else
			handle = trap_PC_LoadSource(va("maps/%s.entities", level.rawmapname));

		if (handle)
		{
			if (G_ParseSpawnVarsEx(handle) == qfalse)
				G_Error( "SpawnEntities: no entities" );

			// parse ents
			while (G_ParseSpawnVarsEx(handle))
				G_SpawnGEntityFromSpawnVars();
	
			trap_PC_FreeSource(handle);
		}
	}
#else //!__ETE__
#ifdef __ENTITY_OVERRIDES__
	if (!handle) 
	{
		// parse possible external entities map files
		// it's used to add new ents to existing pure ET map
		if (g_gametype.integer == GT_WOLF_LMS)
			handle = trap_PC_LoadSource(va("maps/%s_lms.entities", level.rawmapname));
		else if (g_gametype.integer == GT_COOP || g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_NEWCOOP)
			handle = trap_PC_LoadSource(va("maps/%s_coop.entities", level.rawmapname));
		else
			handle = trap_PC_LoadSource(va("maps/%s.entities", level.rawmapname));

		if (handle)
		{
			if (G_ParseSpawnVarsEx(handle) == qfalse)
				G_Error( "SpawnEntities: no entities" );

			// parse ents
			while (G_ParseSpawnVarsEx(handle))
				G_SpawnGEntityFromSpawnVars();
	
			trap_PC_FreeSource(handle);
		}
	}
#endif //__ENTITY_OVERRIDES__
#endif //__ETE__


	G_Printf( "Disable spawning!\n" );
	level.spawning = qfalse;			// any future calls to G_Spawn*() will be errors
}
