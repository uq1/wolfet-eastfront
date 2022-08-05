/*
 * name:		g_combat.c
 *
 * desc:	    //line 1608 some modifies by Masteries 28/11/2007	
 *
*/

#include "g_local.h"
#include "../game/q_shared.h"
#include "../game/botlib.h"		//bot lib interface
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../botai/botai.h"			//bot ai interface
#include "../botai/ai_main.h"
#include "../botai/chars.h"
#include "../botai/ai_team.h"
#include "../botai/ai_dmq3.h"
#include "etpro_mdx.h"

extern void BotRecordKill( int client, int enemy );
extern void BotRecordPain( int client, int enemy, int mod );
extern void BotRecordDeath( int client, int enemy );
/*static*/ void G_Obituary(int mod, int target, int attacker);

extern vec3_t muzzleTrace;

/*
=============
GetPainLocation
=============
*/
int GetPainLocation( gentity_t *ent, vec3_t point )
{
	static char *painTagNames[] = {
		"tag_head",
		"tag_chest",
		"tag_torso",
		"tag_groin",
		"tag_armright",
		"tag_armleft",
		"tag_legright",
		"tag_legleft",
		NULL,
	};

	int tagIndex, bestTag;
	float bestDist, dist;
	orientation_t or;

	// first make sure the client is able to retrieve tag information
	if (!trap_GetTag( ent->s.number, 0, painTagNames[0], &or )) {
		return 0;
	}

	// find a correct animation to play, based on the body orientation at previous frame
	for (tagIndex=0, bestDist=0, bestTag=-1; painTagNames[tagIndex]; tagIndex++) {
		// grab the tag with this name
		if (trap_GetTag( ent->s.number, 0, painTagNames[tagIndex], &or )) {
			dist = VectorDistance( or.origin, point );
			if (!bestDist || dist < bestDist) {
				bestTag = tagIndex;
				bestDist = dist;
			}
		}
	}

	if (bestTag >= 0) {
		return bestTag+1;
	}

	return 0;
}

/*
============
AddScore

Adds score to both the client and his team
============
*/
void AddScore( gentity_t *ent, int score ) {
	if ( !ent || !ent->client ) {
		return;
	}
	// no scoring during pre-match warmup
	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	if( g_gametype.integer == GT_WOLF_LMS ) {
		return;
	}

	//ent->client->ps.persistant[PERS_SCORE] += score;
	ent->client->sess.game_points += score;

//	level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
	CalculateRanks();
}

/*
============
AddKillScore

Adds score to both the client and his team, only used for playerkills, for lms
============
*/
void AddKillScore( gentity_t *ent, int score ) {
	if ( !ent || !ent->client ) {
		return;
	}
	// no scoring during pre-match warmup
	if ( level.warmupTime ) {
		return;
	}

	// someone already won
	if( level.lmsWinningTeam )
		return;

	if( g_gametype.integer == GT_WOLF_LMS ) {
		ent->client->ps.persistant[PERS_SCORE] += score;
		level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
	}
	ent->client->sess.game_points += score;

	CalculateRanks();
}

extern vmCvar_t		g_dropSecondaryWeapons;
extern vmCvar_t		g_dropGrenades;

/*
=================
TossClientItems

Toss the weapon and powerups for the killed player
=================
*/
void TossClientItems( gentity_t *self ) {
	/*gitem_t		*item;
	int			weapon;
	gentity_t	*drop = 0;

	// drop the weapon if not a gauntlet or machinegun
	weapon = self->s.weapon;

	// make a special check to see if they are changing to a new
	// weapon that isn't the mg or gauntlet.  Without this, a client
	// can pick up a weapon, be killed, and not drop the weapon because
	// their weapon change hasn't completed yet and they are still holding the MG.

	// (SA) always drop what you were switching to
	if( self->client->ps.weaponstate == WEAPON_DROPPING ) {
		weapon = self->client->pers.cmd.weapon;
	}

	if( !( COM_BitCheck( self->client->ps.weapons, weapon ) ) ) {
		return;
	}

	if((self->client->ps.persistant[PERS_HWEAPON_USE])) {
		return;
	}

	// JPW NERVE don't drop these weapon types
	switch( weapon ) {
		case WP_NONE:
		case WP_KNIFE:
		case WP_DYNAMITE:
		case WP_ARTY:
		case WP_MEDIC_SYRINGE:
		case WP_SMOKETRAIL:
		case WP_MAPMORTAR:
		case VERYBIGEXPLOSION:
		case WP_MEDKIT:
		case WP_BINOCULARS:
		case WP_PLIERS:
		case WP_SMOKE_MARKER:
		case WP_TRIPMINE:
		case WP_SMOKE_BOMB:
		case WP_DUMMY_MG42:
		case WP_LOCKPICK:
		case WP_MEDIC_ADRENALINE:
			return;
		case WP_MORTAR_SET:
			weapon = WP_MORTAR;
			break;
		case WP_K43_SCOPE:
			weapon = WP_K43;
			break;
		case WP_GARAND_SCOPE:
			weapon = WP_GARAND;
			break;
		case WP_FG42SCOPE:
			weapon = WP_FG42;
			break;
		case WP_M7:
			weapon = WP_CARBINE;
			break;
		case WP_GPG40:
			weapon = WP_KAR98;
			break;
		case WP_MOBILE_MG42_SET:
			weapon = WP_MOBILE_MG42;
			break;
	}

	// find the item type for this weapon
	item = BG_FindItemForWeapon( weapon );
	// spawn the item
	
	drop = Drop_Item( self, item, 0, qfalse );
	drop->count = self->client->ps.ammoclip[BG_FindClipForWeapon(weapon)];
	drop->item->quantity = self->client->ps.ammoclip[BG_FindClipForWeapon(weapon)];*/

	weapon_t primaryWeapon, secondaryWeapon;

	if( g_gamestate.integer == GS_INTERMISSION ) {
		return;
	}

	primaryWeapon = G_GetPrimaryWeaponForClient( self->client );

	if( primaryWeapon ) {
		// drop our primary weapon
		G_DropWeapon( self, primaryWeapon );
	}

	if (self->client && g_dropSecondaryWeapons.integer)
	{
		secondaryWeapon = G_GetSecondaryWeaponForClient( self->client );

		if( secondaryWeapon ) {
			// drop our secondary weapon
			switch (secondaryWeapon)
			{// Drop 2 of akimbos!
			case WP_AKIMBO_SILENCEDCOLT:
				secondaryWeapon = WP_SILENCED_COLT;
				G_DropWeapon( self, secondaryWeapon );
				break;
			case WP_AKIMBO_SILENCEDLUGER:
				secondaryWeapon = WP_SILENCED_LUGER;
				G_DropWeapon( self, secondaryWeapon );
				break;
			case WP_AKIMBO_COLT:
				secondaryWeapon = WP_COLT;
				G_DropWeapon( self, secondaryWeapon );
				break;
			case WP_AKIMBO_LUGER:
				secondaryWeapon = WP_LUGER;
				G_DropWeapon( self, secondaryWeapon );
				break;
			default:
				break;
			}

			G_DropWeapon( self, secondaryWeapon );
		}
	}

	if (self->client && g_dropGrenades.integer)
	{
		if (self->client->sess.sessionTeam == TEAM_AXIS)
		{
			int i = 0;

			for (i = 0; i < self->client->ps.ammo[WP_GRENADE_LAUNCHER] && i < 5; i++)
				G_DropWeapon( self, WP_GRENADE_LAUNCHER );
		}
		else
		{
			int i = 0;

			for (i = 0; i < self->client->ps.ammo[WP_GRENADE_PINEAPPLE] && i < 5; i++)
				G_DropWeapon( self, WP_GRENADE_PINEAPPLE );
		}
	}
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker ) {
	vec3_t		dir;
	vec3_t		angles;

	if ( attacker && attacker != self ) {
		VectorSubtract (attacker->s.pos.trBase, self->s.pos.trBase, dir);
	} else if ( inflictor && inflictor != self ) {
		VectorSubtract (inflictor->s.pos.trBase, self->s.pos.trBase, dir);
	} else {
		self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
		return;
	}

	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw ( dir );

	angles[YAW] = vectoyaw ( dir );
	angles[PITCH] = 0; 
	angles[ROLL] = 0;
}

/*
==================
GibEntity
==================
*/
void GibEntity( gentity_t *self, int killer ) 
{
	gentity_t *other=&g_entities[killer];
	vec3_t dir;

	VectorClear( dir );
	if (other->inuse) {
		if (other->client) {
			VectorSubtract( self->r.currentOrigin, other->r.currentOrigin, dir );
			VectorNormalize( dir );
		} else if (!VectorCompare(other->s.pos.trDelta, vec3_origin)) {
			VectorNormalize2( other->s.pos.trDelta, dir );
		}
	}

	G_AddEvent( self, EV_GIB_PLAYER, DirToByte(dir) );
	self->takedamage = qfalse;
	self->s.eType = ET_INVISIBLE;
	self->r.contents = 0;
}


/*
================
TankExplode

Explode a tank 
================
*/
void TankExplode( gentity_t *ent ) {
	vec3_t		dir;
	vec3_t		origin;


	ent->takedamage = qfalse;
	ent->s.eType = ET_INVISIBLE;
	ent->r.contents = 0;
	ent->TankMarker = 0;



	// splash damage
	{
		vec3_t origin;
		trace_t tr;

		VectorCopy( ent->r.currentOrigin, origin );
		origin[2] += 4;

		trap_Trace( &tr, origin, vec3_origin, vec3_origin, origin, ENTITYNUM_NONE, MASK_SHOT );
		G_RadiusDamage( origin, ent, ent, 55, 300, ent, MOD_EXPLOSIVE );	

	}

	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin, qfalse, ent->s.effect2Time );
	SnapVector( origin );
	G_SetOrigin( ent, origin );

	// we don't have a valid direction, so just point straight up
	dir[0] = dir[1] = 0;
	dir[2] = 1;

		{		
		gentity_t* tent = G_TempEntity(ent->r.currentOrigin, EV_MISSILE_MISS_LARGE);
		tent->s.eventParm = DirToByte( dir );
		}


		{		
		gentity_t* tent = G_TempEntity(ent->r.currentOrigin, EV_SHAKE);
		tent->s.onFireStart = 500;
		tent->r.svFlags |= SVF_BROADCAST;
		}

}


/*
==================
body_die
==================
*/
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	if(self->health <= GIB_HEALTH) {
		GibEntity(self, 0);
	}
}


// these are just for logging, the client prints its own messages
char *modNames[] =
{
	"MOD_UNKNOWN",
	"MOD_MACHINEGUN",
	"MOD_BROWNING",
	"MOD_MG42",
	"MOD_GRENADE",
	"MOD_ROCKET",

	// (SA) modified wolf weap mods
	"MOD_KNIFE",
	"MOD_LUGER",
	"MOD_COLT",
	"MOD_MP40",
	"MOD_THOMPSON",
	"MOD_STEN",
	"MOD_GARAND",
	"MOD_SNOOPERSCOPE",
	"MOD_SILENCER",	//----(SA)	
	"MOD_FG42",
	"MOD_FG42SCOPE",
	"MOD_PANZERFAUST",
	"MOD_GRENADE_LAUNCHER",
	"MOD_FLAMETHROWER",
	"MOD_GRENADE_PINEAPPLE",
	"MOD_CROSS",
	// end

	"MOD_MAPMORTAR",
	"MOD_MAPMORTAR_SPLASH",

	"MOD_KICKED",
	"MOD_GRABBER",

	"MOD_DYNAMITE",
	"MOD_AIRSTRIKE", // JPW NERVE
	"MOD_SYRINGE",	// JPW NERVE
	"MOD_AMMO",	// JPW NERVE
	"MOD_ARTY",	// JPW NERVE

	"MOD_WATER",
	"MOD_SLIME",
	"MOD_LAVA",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TARGET_LASER",
	"MOD_TRIGGER_HURT",
	"MOD_EXPLOSIVE",

	"MOD_CARBINE",
	"MOD_KAR98",
	"MOD_GPG40",
	"MOD_M7",
	"MOD_LANDMINE",
	"MOD_SATCHEL",
	"MOD_TRIPMINE",
	"MOD_SMOKEBOMB",
	"MOD_MOBILE_MG42",
	"MOD_SILENCED_COLT",
	"MOD_GARAND_SCOPE",

	"MOD_CRUSH_CONSTRUCTION",
	"MOD_CRUSH_CONSTRUCTIONDEATH",
	"MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER",

	"MOD_K43",
	"MOD_K43_SCOPE",

	"MOD_MORTAR",

	"MOD_AKIMBO_COLT",
	"MOD_AKIMBO_LUGER",
	"MOD_AKIMBO_SILENCEDCOLT",
	"MOD_AKIMBO_SILENCEDLUGER",

	"MOD_SMOKEGRENADE",

	// RF
	"MOD_SWAP_PLACES",
	"MOD_TANKSHELL",
	"MOD_PTRSSHELL",

	"MOD_PPSH",
	"MOD_PPS",
	"MOD_STG44",
	"MOD_SVT40",
	"MOD_30CAL",

	// OSP -- keep these 2 entries last
	"MOD_SWITCHTEAM",

	"MOD_GOOMBA",
	"MOD_POISON",
	"MOD_FEAR",
	"MOD_THROWN_KNIFE",
	"MOD_REFLECTED_FF",
};

/*
==================
player_die
==================
*/
void BotRecordTeamDeath( int client );

 char *G_KillSpreeSanitize(char *text)
 {
 	static char n[MAX_SAY_TEXT] = {""};
 
 	if(!*text)
 		return n;
 	Q_strncpyz(n, text, sizeof(n));
 	Q_strncpyz(n, Q_StrReplace(n, "[a]", "(a)"), sizeof(n));
 	Q_strncpyz(n, Q_StrReplace(n, "[n]", "(n)"), sizeof(n));
 	Q_strncpyz(n, Q_StrReplace(n, "[d]", "(d)"), sizeof(n));
 	Q_strncpyz(n, Q_StrReplace(n, "[k]", "(k)"), sizeof(n));
 	return n;
 }
 
 
 static void G_KillSpree(gentity_t *ent, gentity_t *attacker)
 {
 	char cmd[5];
 	char *s;
 	char *ksmsg;
 	char aname[MAX_NAME_LENGTH] = {"*unknown*"};
 	char nname[MAX_NAME_LENGTH] = {"*unknown*"};
 	char spreemsg[MAX_CVAR_VALUE_STRING];
 	int  strlevel;
 
 	if(!ent || !ent->client)
 		return;
 	if(!g_killingSpree.integer)
 		return;
 	
 	Q_strncpyz(cmd, "chat", sizeof(cmd));
 	if(g_killingSpreePos.integer & MSGPOS_LEFT_BANNER)
 		Q_strncpyz(cmd, "cpm", sizeof(cmd));
 	if(g_killingSpreePos.integer & MSGPOS_CENTER)
 		Q_strncpyz(cmd, "cp", sizeof(cmd));
 		
 	// tjw: set up attacker's name
 	if(ent == attacker) {
 		Q_strncpyz(aname, "himself", sizeof(aname));
 	}
 	else if(attacker && attacker->client) {
 		Q_strncpyz(aname,
 			G_KillSpreeSanitize(attacker->client->pers.netname),
 			sizeof(aname));
 	}
 	Q_strncpyz(nname,
 		G_KillSpreeSanitize(ent->client->pers.netname),
 		sizeof(nname));
 
 	// Perro:  Killing Sprees -- print a message indicating
 	// end of streak...
 	if (ent->client->sess.kstreak >= g_killingSpree.integer) {
 		if(OnSameTeam(ent, attacker) && ent != attacker) {
 			s = g_killingSpreeMsg_tk.string;
 		}
 		else {
 			s = g_killingSpreeMsg_end.string;
 		}
 		
 
 		ksmsg = Q_StrReplace(s, "[k]",
 			va("%d", ent->client->sess.kstreak));
 		ksmsg = Q_StrReplace(ksmsg, "[a]", aname);
 		ksmsg = Q_StrReplace(ksmsg, "[n]", nname);
 		AP(va("%s \"%s\"", cmd, ksmsg));
 	}
 
 	// now actually stop any killing spree that might be
 	// going onthis will, however, start or continue a
 	// death spree
 	ent->client->sess.kstreak = 0;
 	ent->client->sess.dstreak++;
 
 	if (!(ent->client->sess.dstreak % g_killingSpree.integer)) {
 		ksmsg = Q_StrReplace(g_killingSpreeMsg_futility.string,
 			"[d]",
 			va("%d", ent->client->sess.dstreak));
 		ksmsg = Q_StrReplace(ksmsg, "[n]", nname);
 		AP(va("%s \"%s\"", cmd, ksmsg));
 	}
 
 	if(attacker && 
 		attacker->client && 
 		attacker != ent &&
 		(!(OnSameTeam( ent, attacker )))) {
 		attacker->client->sess.kstreak++;
 		attacker->client->sess.dstreak=0;
 		// check to see if this is the best so far for this map
 		if (attacker->client->sess.kstreak > 
 					level.maxspree_count ) {
 			level.maxspree_count = 
 				attacker->client->sess.kstreak;
 			level.maxspree_player[0] = '\0';
 			Q_strncpyz(level.maxspree_player, 
 				attacker->client->pers.netname,
 				sizeof(level.maxspree_player));
 		}
 		
 		if((attacker->client->sess.kstreak %
 				g_killingSpree.integer)) {
 			return;
 		}
 
 		ksmsg = g_killingSpreeMsg_default.string;
 		strlevel = (attacker->client->sess.kstreak /g_killingSpree.integer);
 		// dynamic cvar allows numerous admin-defined spree levels and messages
 		trap_Cvar_VariableStringBuffer(va("g_killingSpreeMsg_%d",strlevel),
 			spreemsg,
 			sizeof(spreemsg));
 		if (Q_stricmp(spreemsg,"")) {
 			ksmsg = spreemsg;
 		}
 		
 		ksmsg = Q_StrReplace(ksmsg,
 			"[k]",
 			va("%d", attacker->client->sess.kstreak));
 		ksmsg = Q_StrReplace(ksmsg,
 			"[n]", aname);
 		
 		AP(va("%s \"%s\"", cmd, ksmsg));
 	}
 }

extern void G_DropKnife(gentity_t *ent);

#ifdef __EF__
extern void CheckSupression ( gentity_t *ent );
#endif //__EF__

void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
	int			contents = 0, i, killer = ENTITYNUM_WORLD;
	char		*killerName = "<world>";
	qboolean	nogib = qtrue;
	gentity_t	*ent;
	qboolean	killedintank = qfalse;

	//float			timeLived;
	weapon_t	weap;// = BG_WeaponForMOD( meansOfDeath );

#ifdef __EF__
	ClearSupression(self);
#endif //__EF__

#ifdef __SHELLSHOCK__
	{
		entityState_t *es = SecondaryEntityState(self->s.number);
		es->eFlags &= ~EX_SHELLSHOCK;
	}
#endif //__SHELLSHOCK__
 
 	// tjw: for g_shortcuts
 	if(attacker && attacker->client) {
 		self->client->pers.lastkiller_client = attacker->s.clientNum;
 		attacker->client->pers.lastkilled_client = self->s.clientNum;
 	}
 
 	// tjw: if player has /kill'ed in response to recent
 	//      damage, then we need to credit the last attacker
 	//      with proper XP.
 	if(meansOfDeath == MOD_FEAR)
 		weap = BG_WeaponForMOD(self->client->lasthurt_mod);
 	else
 		weap = BG_WeaponForMOD(meansOfDeath);
 	
 	// xkan, 1/13/2003 - record the time we died.
 	// tjw: moved this to death instead of limbo
 	//      this is used for G_DropLimboHealth()
 	if(!self->client->deathTime)
 		self->client->deathTime = level.time;
 
 	//unlagged - backward reconciliation #2
 	// make sure the body shows up in the client's current position
 	G_ReAdjustSingleClientPosition( self );
 	//unlagged - backward reconciliation #2
 
 	self->client->pmext.poisoned = qfalse;
 	
 	// these should not end a killing streak nor create a death streak
 	if (meansOfDeath != MOD_SUICIDE && meansOfDeath != MOD_SWITCHTEAM)
 		G_KillSpree(self, attacker);

//	G_Printf( "player_die\n" );

	if(attacker == self) {
		if(self->client) {
			self->client->pers.playerStats.suicides++;
			trap_PbStat ( self - g_entities , "suicide" , 
 				va("%d %d %d", self->client->sess.sessionTeam,
 				self->client->sess.playerType, weap)) ;
 		}
 		
  		}
 	else if(OnSameTeam( self, attacker )) {
  		G_LogTeamKill(	attacker,	weap );
 	}
 	else {
		G_LogDeath( self,		weap );
		G_LogKill(	attacker,	weap );

		if( g_gamestate.integer == GS_PLAYING ) {
			if( attacker->client ) {
				attacker->client->combatState |= (1<<COMBATSTATE_KILLEDPLAYER);
			}
		}
#ifdef __BOT__
		if ( self->r.svFlags & SVF_BOT && rand() % 100 > 50 
			&& !(attacker->r.svFlags & SVF_BOT) 
			&& meansOfDeath != MOD_FLAMETHROWER
			&& meansOfDeath != MOD_PANZERFAUST) // great shot when killed by players, 50% chance
		G_AI_Voice( self, NULL, SAY_ALL, "GreatShot", qfalse, 500+rand() % 1000, AIVOICE_FUNTALK);
#endif
	}

	// RF, record this death in AAS system so that bots avoid areas which have high death rates
	if( !OnSameTeam( self, attacker ) ) {
		BotRecordTeamDeath( self->s.number );

		self->isProp = qfalse;	// were we teamkilled or not?
	} else {
		self->isProp = qtrue;
	}	

	// if we got killed by a landmine, update our map
	if( self->client && meansOfDeath == MOD_LANDMINE ) {
		// if it's an enemy mine, update both teamlists
		mapEntityData_t	*mEnt;

		if((mEnt = G_FindMapEntityData(&mapEntityData[0], inflictor-g_entities)) != NULL) {
			G_FreeMapEntityData( &mapEntityData[0], mEnt );
		}

		if((mEnt = G_FindMapEntityData(&mapEntityData[1], inflictor-g_entities)) != NULL) {
			G_FreeMapEntityData( &mapEntityData[1], mEnt );
		}
	}

	{
		mapEntityData_t	*mEnt;
		mapEntityData_Team_t *teamList = self->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[1] : &mapEntityData[0];	// swapped, cause enemy team

		mEnt = G_FindMapEntityDataSingleClient( teamList, NULL, self->s.number, -1 );
		
		while( mEnt ) {
			if( mEnt->type == ME_PLAYER_DISGUISED ) {
				mapEntityData_t* mEntFree = mEnt;

				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, self->s.number, -1 );

				G_FreeMapEntityData( teamList, mEntFree );
			} else {
				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, self->s.number, -1 );
			}
		}
	}

	if( self->tankLink ) {
		//gentity_t *tank = G_LeaveTank( self, qfalse );
		G_LeaveTank( self, qfalse );

//		if (tank)
//			tank_die(tank, tank, tank, 10000, MOD_UNKNOWN);

		killedintank = qtrue;
	}

	if((self->client->ps.pm_type == PM_DEAD  && 
		!(self->client->ps.eFlags & EF_PLAYDEAD)) || 
		g_gamestate.integer == GS_INTERMISSION ) {
		return;
	}

	if(self->client->ps.eFlags & EF_PLAYDEAD) {
		// forty - restore the weapon so they can be properly dropped on limbo
		self->s.weapon = self->client->limboDropWeapon;
		self->client->ps.weapon = self->client->limboDropWeapon;
	}

	// the player really died this time
	self->client->ps.eFlags &= ~EF_PLAYDEAD;

	// OSP - death stats handled out-of-band of G_Damage for external calls
	if(meansOfDeath == MOD_FEAR) {
		G_addStats(self, attacker, damage, self->client->lasthurt_mod);
		// tjw: this would normally be handled in G_Damage()
		G_AddKillSkillPoints(attacker,
			self->client->lasthurt_mod, 0, qfalse);
	}
	else {
	G_addStats(self, attacker, damage, meansOfDeath);
	}
	// OSP

	self->client->ps.pm_type = PM_DEAD;

	G_AddEvent( self, EV_STOPSTREAMINGSOUND, 0);

	if(attacker) {
		killer = attacker->s.number;

		killerName = (attacker->client) ? attacker->client->pers.netname : "<non-client>";
	}

	if(attacker == 0 || killer < 0 || killer >= MAX_CLIENTS) {
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if(g_gamestate.integer == GS_PLAYING) {
		char *obit;

		if(meansOfDeath < 0 || meansOfDeath >= sizeof(modNames) / sizeof(modNames[0])) {
			obit = "<bad obituary>";
		} else {
			obit = modNames[meansOfDeath];
		}

		G_LogPrintf("Kill: %i %i %i: %s killed %s by %s\n", killer, self->s.number, meansOfDeath, killerName, self->client->pers.netname, obit );
	}

	// RF, record bot kills
	if (attacker->r.svFlags & SVF_BOT) {
		BotRecordKill( attacker->s.number, self->s.number );
	}

	if(
		g_logOptions.integer & LOGOPTS_REPORT_GIBS && 
		self->health <= GIB_HEALTH 
	) {
		if(attacker == self || killer == ENTITYNUM_WORLD )
			AP(va("cpm \"%s ^7was gibbed.\"", self->client->pers.netname));
		else if(OnSameTeam(attacker, self))
		{
			AP(va("cpm \"%s ^3WAS GIBBED BY TEAMMATE^7 %s\"", self->client->pers.netname, killerName));
		}
		else
		{
			AP(va("cpm \"%s ^7was gibbed by^7 %s\"", self->client->pers.netname, killerName));
		}
	}

#ifdef __NPC__
	if (attacker != self && attacker->s.eType == ET_NPC)
	{// If the attacker was an npc then set the MOD value now...
		meansOfDeath = GetAmmoTableData(attacker->s.weapon)->mod;
		killer = attacker->s.number;
	}
#endif //__NPC__

	// broadcast the death event to everyone
	if (g_obituary.integer == OBIT_SERVER_ONLY ||
		(g_obituary.integer == OBIT_CLIENT_PREF &&
			(meansOfDeath == MOD_GOOMBA ||
			meansOfDeath == MOD_POISON ||
			meansOfDeath == MOD_FEAR ||
			meansOfDeath == MOD_REFLECTED_FF ||
			meansOfDeath == MOD_THROWN_KNIFE))) {

		G_Obituary(meansOfDeath, self->s.number, killer);
	}
	else if(g_obituary.integer) {
	ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
	ent->s.eventParm = meansOfDeath;
	ent->s.otherEntityNum = self->s.number;
	ent->s.otherEntityNum2 = killer;
	ent->s.teamNum = self->s.teamNum;
	ent->s.density = ET_GENERAL;
	ent->s.effect1Time = attacker->s.eType; // Send attacker type...
	ent->s.effect2Time = attacker->s.teamNum; // Send attacker team...
	ent->r.svFlags = SVF_BROADCAST;	// send to everyone
	}

	self->enemy = attacker;

	self->client->ps.persistant[PERS_KILLED]++;

	 // CHRUKER: b010 - Make sure covert ops looses their disguises
	 if(self->client->ps.powerups[PW_OPS_DISGUISED])
		 self->client->ps.powerups[PW_OPS_DISGUISED] = 0;

	// JPW NERVE -- if player is holding ticking grenade, drop it
	if ((self->client->ps.grenadeTimeLeft) && (self->s.weapon != WP_DYNAMITE) && (self->s.weapon != WP_LANDMINE) && (self->s.weapon != WP_SATCHEL) && (self->s.weapon != WP_TRIPMINE)) {
		vec3_t launchvel, launchspot;

		launchvel[0] = crandom();
		launchvel[1] = crandom();
		launchvel[2] = random();
		VectorScale( launchvel, 160, launchvel );
		VectorCopy(self->r.currentOrigin, launchspot);
		launchspot[2] += 40;
		
		{
			// Gordon: fixes premature grenade explosion, ta bani ;)
			gentity_t *m = fire_grenade(self, launchspot, launchvel, self->s.weapon);
			m->damage = 0;
		}
	}

	if (attacker && attacker->client) {
		if ( attacker == self || OnSameTeam (self, attacker ) ) {

			// DHM - Nerve :: Complaint lodging
			if(attacker != self && 
				level.warmupTime <= 0 && 
				g_gamestate.integer == GS_PLAYING &&
				!G_shrubbot_permission(attacker, SBF_IMMUNITY) &&
				!attacker->client->sess.referee) {

				if( attacker->client->pers.localClient ) {
					trap_SendServerCommand( self-g_entities, "complaint -4" );
				} else {
					if( meansOfDeath != MOD_CRUSH_CONSTRUCTION && meansOfDeath != MOD_CRUSH_CONSTRUCTIONDEATH && meansOfDeath != MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER ) {
						if( g_complaintlimit.integer ) {

							if( !(meansOfDeath == MOD_LANDMINE && g_disableComplaints.integer & TKFL_MINES ) &&
								!((meansOfDeath == MOD_ARTY || meansOfDeath == MOD_AIRSTRIKE) && g_disableComplaints.integer & TKFL_AIRSTRIKE ) &&
								!(meansOfDeath == MOD_MORTAR && g_disableComplaints.integer & TKFL_MORTAR ) ) {
								trap_SendServerCommand( self-g_entities, va( "complaint %i", attacker->s.number ) );
								self->client->pers.complaintClient = attacker->s.clientNum;

#ifdef __BOT__
								if (attacker->r.svFlags & SVF_BOT)
								{
									self->client->pers.complaintClient = -5;
								}
#endif //__BOT__

								self->client->pers.complaintEndTime = level.time + 20500;
							}
						}
					}
				}
			}

			// high penalty to offset medic heal
			if( g_gametype.integer == GT_WOLF_LMS ) {
				AddKillScore( attacker, WOLF_FRIENDLY_PENALTY );
			}
		} else {

			//G_AddExperience( attacker, 1 );

			// JPW NERVE -- mostly added as conveneience so we can tweak from the #defines all in one place
			AddScore(attacker, WOLF_FRAG_BONUS);

			if( g_gametype.integer == GT_WOLF_LMS ) {
				if( level.firstbloodTeam == -1 )
					level.firstbloodTeam = attacker->client->sess.sessionTeam;

				AddKillScore( attacker, WOLF_FRAG_BONUS );
			}

		}
			attacker->client->lastKillTime = level.time;
	} else {
		AddScore( self, -1 );

		if( g_gametype.integer == GT_WOLF_LMS )
			AddKillScore( self, -1 );
	}

	// Add team bonuses
	Team_FragBonuses(self, inflictor, attacker);

	G_DropItems(self);

	G_DropKnife(self);

	// send a fancy "MEDIC!" scream.  Sissies, ain' they?
/*
// UQ1: Disable this for now, its a bit annoying...
	if (self->client != NULL) {
		if( self->health > GIB_HEALTH && meansOfDeath != MOD_SUICIDE && meansOfDeath != MOD_SWITCHTEAM 
#ifdef __VEHICLES__
			&& !(self->client->ps.eFlags & EF_VEHICLE) 
#endif //__VEHICLES__
			) {
			G_AddEvent( self, EV_MEDIC_CALL, 0 );
		}
	}
*/

	Cmd_Score_f( self );		// show scores

	// send updated scores to any clients that are following this one,
	// or they would get stale scoreboards
	for(i=0; i<level.numConnectedClients; i++) {
		gclient_t *client = &level.clients[level.sortedClients[i]];

		if(client->pers.connected != CON_CONNECTED) continue;
		if(client->sess.sessionTeam != TEAM_SPECTATOR) continue;

		if(client->sess.spectatorClient == self->s.number) {
			Cmd_Score_f(g_entities + level.sortedClients[i]);
		}
	}

	self->takedamage = qtrue;	// can still be gibbed
	self->r.contents = CONTENTS_CORPSE;

	//self->s.angles[2] = 0;
	self->s.powerups = 0;
	self->s.loopSound = 0;
	
	self->client->limboDropWeapon = self->s.weapon; // store this so it can be dropped in limbo

	LookAtKiller( self, inflictor, attacker );
	self->client->ps.viewangles[0] = 0;
	self->client->ps.viewangles[2] = 0;
	//VectorCopy( self->s.angles, self->client->ps.viewangles );

//	trap_UnlinkEntity( self );

	// ydnar: so bodies don't clip into world
	self->r.maxs[2] = self->client->ps.crouchMaxZ;	
	self->client->ps.maxs[2] = self->client->ps.crouchMaxZ;	


	trap_LinkEntity( self );

	// don't allow respawn until the death anim is done
	// g_forcerespawn may force spawning at some later time
	self->client->respawnTime = level.timeCurrent + 800;

	// remove powerups
	memset( self->client->ps.powerups, 0, sizeof(self->client->ps.powerups) );

	// never gib in a nodrop
	// FIXME: contents is always 0 here
	if ( self->health <= GIB_HEALTH && !(contents & CONTENTS_NODROP) ) {
		GibEntity( self, killer );
		nogib = qfalse;
	}

	if(nogib)
	{
		// normal death
		// for the no-blood option, we need to prevent the health
		// from going to gib level
		if ( self->health <= GIB_HEALTH ) {
			self->health = GIB_HEALTH + 1;
		}
/*
// UQ1: This sucks!
		// Arnout: re-enable this for flailing
			// DHM - Play death animation, and set pm_time to delay 'fallen' animation
			//if( G_IsSinglePlayerGame() && self->client->sess.sessionTeam == TEAM_ALLIES ) {
			//	// play "falldown" animation since allies bots won't ever die completely
			//	self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_FALLDOWN, qfalse, qtrue );
			//	G_StartPlayerAppropriateSound(self, "death");
			//} else {
				self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_DEATH, qfalse, qtrue );
				// death animation script already contains sound
			//}
*/
		if (1) 
		{// UQ1: Better death anims! :)
			// enemy weapon
			if (attacker->client) {
				BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_WEAPON, attacker->s.weapon, qtrue );
			}
			if (self->last_hit_point) {
				int hit_location = GetPainLocation(self, self->last_hit_point);

				if (hit_location == 1 && !g_realism.integer)
				{// UQ1: Head shot! If not in realism modes, fake it as another location because of insta-limbo...
					switch (meansOfDeath)
					{
					case MOD_SNOOPERSCOPE:
					case MOD_FG42SCOPE:
					case MOD_GARAND_SCOPE:
						// Sniped, use real head location for insta-limbo...
						BG_UpdateConditionValue( self->s.number, ANIM_COND_IMPACT_POINT, GetPainLocation(self, self->last_hit_point), qtrue );
						break;
					default:
						// Not sniped, use fake location...
						BG_UpdateConditionValue( self->s.number, ANIM_COND_IMPACT_POINT, 2 /* Fake Chest Hit */, qtrue );
						break;
					}
				}
				else
				{
					BG_UpdateConditionValue( self->s.number, ANIM_COND_IMPACT_POINT, GetPainLocation(self, self->last_hit_point), qtrue );
				}
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

			self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_DEATH, qfalse, qtrue );

			// turn off temporary conditions
			BG_UpdateConditionValue( self->s.number, ANIM_COND_STUNNED, 0, qfalse );
			BG_UpdateConditionValue( self->s.number, ANIM_COND_ENEMY_WEAPON, 0, qfalse );
			BG_UpdateConditionValue( self->s.number, ANIM_COND_IMPACT_POINT, 0, qfalse );
		}

		// record the death animation to be used later on by the corpse
		self->client->torsoDeathAnim = self->client->ps.torsoAnim;
		self->client->legsDeathAnim = self->client->ps.legsAnim;

		G_AddEvent( self, EV_DEATH1 + 1, killer );

		// the body can still be gibbed
		self->die = body_die;
	}

	if( meansOfDeath == MOD_MACHINEGUN ) {
		switch( self->client->sess.sessionTeam ) {
			case TEAM_AXIS:
				level.axisMG42Counter = level.time;
				break;
			case TEAM_ALLIES:
				level.alliesMG42Counter = level.time;
				break;
			default:
				break;
		}
	}

	G_FadeItems( self, MOD_SATCHEL );

	CalculateRanks();

#ifdef __VEHICLES__
	if ( self->client->ps.eFlags & EF_VEHICLE ) 
	{
		TankExplode( self );
		//G_LeaveTank(self, qfalse);
		limbo( self, qfalse, qfalse );
	} else 
#endif //__VEHICLES__
	if( killedintank ) {
		limbo( self, qfalse, qfalse ); // but no corpse
	}
	else if((meansOfDeath == MOD_SUICIDE || meansOfDeath == MOD_FEAR)
		&& g_gamestate.integer == GS_PLAYING) {

		limbo( self, qtrue, qfalse );
	}
	else if( g_gametype.integer == GT_WOLF_LMS ) {
		if( !G_CountTeamMedics( self->client->sess.sessionTeam, qtrue ) ) {
			limbo( self, qtrue, qfalse );
		}
	}
/*
	if ((g_gametype.integer <= GT_COOP || g_gametype.integer == GT_NEWCOOP) && (self->r.svFlags & SVF_BOT))
	{// A dead bot in sp/coop is really dead!
		self->bot_no_respawn = qtrue;
	}*/

	/*if (!nogib)
	{// UQ1: Gore???
		gentity_t *tent = G_TempEntity( self->r.currentOrigin, EV_RUBBLE);
		VectorCopy (self->r.currentOrigin, tent->s.origin);
		
	}*/

	if (self->last_hit_point) 
	{// UQ1: Have a hit location, check for a rifle headshot for special animation (and instant death!)...
		int hit_location = GetPainLocation(self, self->last_hit_point);

		if (hit_location == 1)
		{// UQ1: Head shot! If it was a rifle, there are no revives! - This allows us to use a nicer animation...
			if (g_realism.integer)
			{// Realism mode, any rifle headshot allows no revives...
				switch (meansOfDeath)
				{
				case MOD_GARAND:
				case MOD_SNOOPERSCOPE:
				case MOD_FG42:
				case MOD_FG42SCOPE:
				case MOD_CARBINE:
				case MOD_KAR98:
				case MOD_GARAND_SCOPE:
				case MOD_K43:
				case MOD_K43_SCOPE:
				case MOD_SVT40:
					limbo( self, qtrue, qtrue );
					break;
				default:
					break;
				}
			}
			else {// Arcade mode, any scope headshot allows no revives...
				switch (meansOfDeath)
				{
				case MOD_SNOOPERSCOPE:
				case MOD_FG42SCOPE:
				case MOD_GARAND_SCOPE:
					limbo( self, qtrue, qtrue );
					break;
				default:
					break;
				}
			}
		}
	}
}


qboolean IsHeadShotWeapon (int mod) {
	// players are allowed headshots from these weapons
	if (	mod == MOD_LUGER ||
			mod == MOD_COLT ||
			mod == MOD_AKIMBO_COLT ||
			mod == MOD_AKIMBO_LUGER ||
			mod == MOD_AKIMBO_SILENCEDCOLT ||
			mod == MOD_AKIMBO_SILENCEDLUGER ||
			mod == MOD_MP40 ||
			mod == MOD_THOMPSON ||
			mod == MOD_PPS ||
			mod == MOD_PPSH ||
			mod == MOD_STG44 ||
			mod == MOD_SVT40 ||
			//mod == MOD_30CAL ||
			mod == MOD_STEN ||
			mod == MOD_GARAND
			
			|| mod == MOD_KAR98
			|| mod == MOD_K43
			|| mod == MOD_K43_SCOPE		
			|| mod == MOD_CARBINE
			|| mod == MOD_GARAND
			|| mod == MOD_GARAND_SCOPE
			|| mod == MOD_SILENCER
			|| mod == MOD_SILENCED_COLT
			|| mod == MOD_FG42
			|| mod == MOD_FG42SCOPE

			|| mod == MOD_KNIFE
			)
		return qtrue;

	return qfalse;
}

gentity_t* G_BuildHead(gentity_t *ent) {
	gentity_t* head;
	orientation_t or;			// DHM - Nerve
	gclient_t		*client;
#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}
#else //!__NPC__
	client = ent->client;
#endif //__NPC__

	head = G_Spawn ();
	head->classname = "head";
	
	// zinx - moved up here so realistic hitboxes can override
	VectorSet (head->r.mins , -6, -6, -2); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
	VectorSet (head->r.maxs , 6, 6, 10); // changed this z from 0 to 6

	if (trap_GetTag( ent->s.number, 0, "tag_head", &or )) {
		G_SetOrigin( head, or.origin );
	} else if ((g_realHead.integer & REALHEAD_HEAD)) {
		// zinx - realistic hitboxes
		grefEntity_t refent;

		mdx_gentity_to_grefEntity( ent, &refent, ent->timeShiftTime?ent->timeShiftTime:level.time );
		mdx_head_position( ent, &refent, or.origin );
		G_SetOrigin( head, or.origin );
		VectorSet (head->r.mins , -6, -6, -6);
		VectorSet (head->r.maxs , 6, 6, 6);
	} else {
		float height, dest;
		vec3_t v, angles, forward, up, right;
		VectorClear(v);
		G_SetOrigin (head, ent->r.currentOrigin); 

		if((client->ps.eFlags & EF_PRONE)) {
			height = client->ps.viewheight - 60;
		}
		else if((client->ps.eFlags & EF_DEAD) ||
			(client->ps.eFlags & EF_PLAYDEAD)) {

			height = client->ps.viewheight - 64;
		} else if( client->ps.pm_flags & PMF_DUCKED ) {	// closer fake offset for 'head' box when crouching
			height = client->ps.crouchViewHeight - 12;
		} else {
			height = client->ps.viewheight;
		}

		
		VectorCopy( client->ps.viewangles, angles );

		// NERVE - SMF - this matches more closely with WolfMP models
		if ( angles[PITCH] > 180 ) {
			dest = (-360 + angles[PITCH]) * 0.75;
		} else {
			dest = angles[PITCH] * 0.75;
		}
		angles[PITCH] = dest;

		// tjw: the angles need to be clamped for prone 
		//      or the head entity will be underground or
		//      far too tall
		if((client->ps.eFlags & EF_PRONE)) {
			if((client->ps.eFlags & EF_MOTION)) 
				angles[PITCH] = -15;
			else 
				angles[PITCH] = -10;
		}

		AngleVectors( angles, forward, right, up );
		if( client->ps.eFlags & EF_PRONE ) {
			if((client->ps.eFlags & EF_MOTION)) {
				VectorScale(forward, 24, v);
			}
			else {
				VectorScale(forward, 28, v);
				VectorMA(v, 7, right, v);
			}
		}
		else if((client->ps.eFlags & EF_DEAD) ||
			(client->ps.eFlags & EF_PLAYDEAD)) {
			// tjw: -28 is right for the head but it makes
			//      a small gap between the head and body
			//      that cannot be hit.  I think this is worse.
			//VectorScale(forward, -28, v);
			VectorScale(forward, -26, v);
			VectorMA(v, 5, right, v);
		}
		else {
			// tjw: when moving, the head is drawn
			//      down and forward
			if((client->ps.eFlags & EF_MOTION)) { 

				if((client->ps.pm_flags & PMF_DUCKED)) {
					height += 2;
					VectorMA(v, 18, forward, v);
				}
				else {
					height -= 10;
					VectorMA(v, 10, forward, v);
				}
			}
			else {
				VectorScale( forward, 5, v);
			}
			VectorMA(v, 5, right, v);
		}
		VectorMA(v, 18, up, v);

		VectorAdd( v, head->r.currentOrigin, head->r.currentOrigin );
		head->r.currentOrigin[2] += height / 2;
	}

	VectorCopy (head->r.currentOrigin, head->s.origin);

	// tjw: this seems equiv to setting all to 0
	//VectorCopy (ent->r.currentAngles, head->s.angles); 
	//VectorCopy (head->s.angles, head->s.apos.trBase);
	//VectorCopy (head->s.angles, head->s.apos.trDelta);

	// forty - realistic hitboxes
	//VectorSet (head->r.mins , -6, -6, -2); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
	//VectorSet (head->r.maxs , 6, 6, 10); // changed this z from 0 to 6
	head->clipmask = CONTENTS_SOLID;
	head->r.contents = CONTENTS_SOLID;
	head->parent = ent;
	head->s.eType = ET_TEMPHEAD;

	trap_LinkEntity (head);
	
	return head;
}

gentity_t* G_BuildLeg(gentity_t *ent) {
	gentity_t* leg;
	vec3_t flatforward, org;
	//orientation_t or;			// DHM - Nerve
	gclient_t		*client;
#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}
#else //!__NPC__
	client = ent->client;
#endif //__NPC__

	if( !(client->ps.eFlags & EF_PRONE) &&
		!(client->ps.eFlags & EF_DEAD) &&
		!(client->ps.eFlags & EF_PLAYDEAD))
		return NULL;

	leg = G_Spawn ();
	leg->classname = "leg";

	if ((g_realHead.integer & REALHEAD_HEAD)) 
	{
		// zinx - realistic hitboxes
		grefEntity_t refent;

		mdx_gentity_to_grefEntity( ent, &refent, ent->timeShiftTime?ent->timeShiftTime:level.time );
		mdx_legs_position( ent, &refent, org );
		org[2] += client->pmext.proneLegsOffset;
		org[2] -= (playerlegsProneMins[2] + playerlegsProneMaxs[2]) * 0.5;
	} else {
		AngleVectors( client->ps.viewangles, flatforward, NULL, NULL );
		flatforward[2] = 0;
		VectorNormalizeFast( flatforward );
		if(client->ps.eFlags & EF_PRONE) 
		{
			org[0] = ent->r.currentOrigin[0] + flatforward[0] * -32;
			org[1] = ent->r.currentOrigin[1] + flatforward[1] * -32;
		}
		else {
			org[0] = ent->r.currentOrigin[0] + flatforward[0] * 32;
			org[1] = ent->r.currentOrigin[1] + flatforward[1] * 32;
		}

		org[2] = ent->r.currentOrigin[2] + client->pmext.proneLegsOffset;
	}

	G_SetOrigin( leg, org );

	VectorCopy( leg->r.currentOrigin, leg->s.origin );

	// tjw: seems to be just setting all to 0?
	//VectorCopy( ent->r.currentAngles, leg->s.angles ); 
	//VectorCopy( leg->s.angles, leg->s.apos.trBase );
	//VectorCopy( leg->s.angles, leg->s.apos.trDelta );

	VectorCopy( playerlegsProneMins, leg->r.mins );
	VectorCopy( playerlegsProneMaxs, leg->r.maxs );
	leg->clipmask = CONTENTS_SOLID;
	leg->r.contents = CONTENTS_SOLID;
	leg->parent = ent;
	leg->s.eType = ET_TEMPLEGS;

	trap_LinkEntity( leg );
	
	return leg;
}

qboolean IsHeadShot( gentity_t *attacker, gentity_t *targ, vec3_t dir, vec3_t point, int mod ) {
	gentity_t	*head;
	trace_t		tr;
	vec3_t		start, end;
	gentity_t	*traceEnt;

	// not a player or critter so bail
	if( !(targ->client) )
		return qfalse;

	//if( targ->health <= 0 )
	//	return qfalse;

	if (!IsHeadShotWeapon (mod) ) {
		return qfalse;
	}
	//if (g_antilag.integer) {
	//	// josh: Backwards reconcile the head. OUCH!
	//	G_AdjustSingleClientPosition(targ,attacker->client->pers.cmd.serverTime,NULL);
	//}

#ifdef __VEHICLES__
	if (targ && targ->client && (targ->client->ps.eFlags & EF_VEHICLE)) 
		return qfalse;
#endif //__VEHICLES__

	head = G_BuildHead( targ );
	
	// trace another shot see if we hit the head
	VectorCopy( point, start );
	VectorMA( start, 64, dir, end );
	trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );
		
	traceEnt = &g_entities[ tr.entityNum ];

#ifdef __DEBUGGING__
	if( g_debugBullets.integer >= 3 ) {	// show hit player head bb
		gentity_t *tent;
		vec3_t b1, b2;
		VectorCopy(head->r.currentOrigin, b1);
		VectorCopy(head->r.currentOrigin, b2);
		VectorAdd(b1, head->r.mins, b1);
		VectorAdd(b2, head->r.maxs, b2);
		tent = G_TempEntity( b1, EV_RAILTRAIL );
		VectorCopy(b2, tent->s.origin2);
		tent->s.dmgFlags = 1;

		// show headshot trace
		// end the headshot trace at the head box if it hits
		if( tr.fraction != 1 ) {
			VectorMA(start, (tr.fraction * 64), dir, end);
		}
		tent = G_TempEntity( start, EV_RAILTRAIL );
		VectorCopy(end, tent->s.origin2);
		tent->s.dmgFlags = 0;
	}
#endif //__DEBUGGING__

	if (g_antilag.integer) {
		G_ReAdjustSingleClientPosition(targ);
	}

	G_FreeEntity( head );

	if( traceEnt == head ) {
		level.totalHeadshots++;		// NERVE - SMF
		return qtrue;
	} else
		level.missedHeadshots++;	// NERVE - SMF

	return qfalse;
}

qboolean IsLegShot( gentity_t *attacker, gentity_t *targ, vec3_t dir, vec3_t point, int mod ) {
	float height;
	float theight;
	gentity_t *leg;

	if (!(targ->client))
		return qfalse;

	//if (targ->health <= 0)
	//	return qfalse;

	if(!point) {
		return qfalse;
	}

	if(!IsHeadShotWeapon(mod)) {
		return qfalse;
	}

	//if (g_antilag.integer) {
	//	// josh: Backwards reconcile the legs. Whoah! Where'd dey go?
	//	G_AdjustSingleClientPosition(targ,attacker->client->pers.cmd.serverTime,NULL);
	//}
	leg = G_BuildLeg( targ );

	if( leg ) {
		gentity_t	*traceEnt;
		vec3_t		start, end;
		trace_t		tr;

		// trace another shot see if we hit the legs
		VectorCopy( point, start );
		VectorMA( start, 64, dir, end );
		trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );
			
		traceEnt = &g_entities[ tr.entityNum ];

#ifdef __DEBUGGING__
		if( g_debugBullets.integer >= 3 ) {	// show hit player head bb
			gentity_t *tent;
			vec3_t b1, b2;
			VectorCopy( leg->r.currentOrigin, b1 );
			VectorCopy( leg->r.currentOrigin, b2 );
			VectorAdd( b1, leg->r.mins, b1 );
			VectorAdd( b2, leg->r.maxs, b2 );
			tent = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, tent->s.origin2 );
			tent->s.dmgFlags = 1;

			// show headshot trace
			// end the headshot trace at the head box if it hits
			if( tr.fraction != 1 ) {
				VectorMA( start, (tr.fraction * 64), dir, end );
			}
			tent = G_TempEntity( start, EV_RAILTRAIL );
			VectorCopy( end, tent->s.origin2 );
			tent->s.dmgFlags = 0;
		}
#endif //__DEBUGGING__

		if (g_antilag.integer) {
			G_ReAdjustSingleClientPosition(targ);
		}

		G_FreeEntity( leg );

		if( traceEnt == leg ) {
			return qtrue;
		}
	} else {
		height = point[2] - targ->r.absmin[2];
		theight = targ->r.absmax[2] - targ->r.absmin[2];

		if(height < (theight * 0.4f)) {
			return qtrue;
		}
	}

	return qfalse;
}

qboolean IsArmShot( gentity_t *targ, gentity_t* ent, vec3_t point, int mod ) {
	vec3_t path, view;
	vec_t dot;

	if (!(targ->client))
		return qfalse;

	if (targ->health <= 0)
		return qfalse;

	if(!IsHeadShotWeapon (mod)) {
		return qfalse;
	}

	VectorSubtract(targ->client->ps.origin, point, path);
	path[2] = 0;

	AngleVectors(targ->client->ps.viewangles, view, NULL, NULL);
	view[2] = 0;

	VectorNormalize(path);

	dot = DotProduct(path, view);

	if(dot > 0.4f || dot < -0.75f ) {
		return qfalse;
	}

	return qtrue;
}

void G_Hitsound(gentity_t *targ,
		gentity_t *attacker,
		int mod,
		qboolean gib,
		qboolean headShot) 
{
	gentity_t *hs_ent;
	gclient_t *client;
	int snd;

	if(!(g_hitsounds.integer & HSF_ENABLE))
		return;
	if((g_hitsounds.integer & HSF_NO_POISON) && mod == MOD_POISON)
		return;
	if(!attacker->client)
		return;
	if(!attacker->client->pers.hitsounds)
		return;
	if(!targ->client) 
		return;
	if(mod == MOD_GOOMBA) 
		return;
	if(targ->health <= 0 && (g_hitsounds.integer & HSF_SILENT_CORPSE))
		return;

	client = targ->client;

	hs_ent = G_TempEntity(attacker->client->ps.origin, 
		       EV_GLOBAL_CLIENT_SOUND);
	hs_ent->s.teamNum = (attacker->client - level.clients);

	// default hitsound
	snd = G_SoundIndex(g_hitsound_default.string);

	if((g_hitsounds.integer & HSF_NO_CORPSE_HEAD)) 
		headShot = qfalse;

	// vsay "hold your fire" on the first hit of a teammate
	// only applies if the player has been hurt before
	// and the match is not in warmup.
	if(OnSameTeam(targ, attacker)) {
		if(!(g_hitsounds.integer & HSF_NO_TEAM_WARN) &&
			(!client->lasthurt_mod || 
			 client->lasthurt_client != attacker->s.number) &&
			g_gamestate.integer == GS_PLAYING &&
			!gib) {
			
			if(client->sess.sessionTeam == TEAM_AXIS) {
				snd = G_SoundIndex(
					g_hitsound_team_warn_axis.string);
			}
			else {
				snd = G_SoundIndex(
					g_hitsound_team_warn_allies.string);
			}
		}
		else if(headShot) {

			if(!(targ->client->ps.eFlags & EF_HEADSHOT)) {
				snd = G_SoundIndex(
					g_hitsound_team_helmet.string);
			}
			else { 
				snd = G_SoundIndex(
					g_hitsound_team_head.string);
			}
		}
		else {
			snd = G_SoundIndex(g_hitsound_team_default.string);
		}
	}
	else if(headShot) {
		if(!(targ->client->ps.eFlags & EF_HEADSHOT))
			snd = G_SoundIndex(g_hitsound_helmet.string);
		else 
			snd = G_SoundIndex(g_hitsound_head.string);
	}
	hs_ent->s.eventParm = snd;
}

/*
============
G_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
	DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
	DAMAGE_NO_ARMOR			armor does not protect from this damage
	DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
	DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/

#ifdef __VEHICLES__
extern vmCvar_t g_allWeaponsDamageTanks;
#endif //__VEHICLES__

void player_pain(gentity_t *ent, gentity_t *attacker, int damage, vec3_t point)
{
	#define		PAIN_THRESHOLD		25
	#define		STUNNED_THRESHOLD	30
//	float		dist;
	qboolean	forceStun=qfalse;
	float		painThreshold, stunnedThreshold;
	gentity_t	*self = ent;

	painThreshold = PAIN_THRESHOLD * 1;//cs->attributes[PAIN_THRESHOLD_SCALE];
	stunnedThreshold = STUNNED_THRESHOLD * 1;//cs->attributes[PAIN_THRESHOLD_SCALE];

	if (!ent->client)
		return;

	if (ent->health <= 0/*GIB_HEALTH + 1*/)
		return;

	// if they are already playing another animation, we might get confused and cut it off, so don't play a pain
	if ( ent->client->ps.torsoTimer || ent->client->ps.legsTimer ) {
		return;
	}

	// if we are waiting for our weapon to fire (throwing a grenade)
	if (ent->client->ps.weaponDelay) {
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

		ent->client->ps.pm_time = BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo, ANIM_ET_PAIN, qfalse, qtrue );

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

//		ent->client->ps.pm_time = BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo, ANIM_ET_PAIN, qfalse, qtrue );
	}
}

#ifdef __SHELLSHOCK__
void new_player_pain(gentity_t *ent, int damage, int mod, vec3_t point)
{
	if (!ent->client)
		return;

	if (ent->client->ps.stats[STAT_HEALTH] > damage 
		&& damage 
		&& G_WeaponIsExplosive(mod) 
		&& !HasExtFlag(ent->s.number, EX_SHELLSHOCK))
	{
		int duration = 15000.0f * (((float)(damage) / (float)(ent->client->ps.stats[STAT_MAX_HEALTH])));
		entityState_t *es = SecondaryEntityState(ent->s.number);

		es->eFlags |= EX_SHELLSHOCK;
		ent->shellShockStartTime = level.time;
		ent->shellShockEndTime = level.time + duration;
		es->effect1Time = duration;
		es->effect2Time = 0;

		//G_Printf("Beginning shellshock! es->number is %i. Duration is %i. Effecttime is %i.\n", es->number, es->effect1Time, es->effect2Time);
	}
}
#endif //__SHELLSHOCK__

#ifdef __NPC__
extern void npc_pain(gentity_t *ent, gentity_t *attacker, int damage, vec3_t point);
#endif //__NPC__

void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,  vec3_t dir, vec3_t point, int damage, int dflags, int mod ) {
	gclient_t	*client;
	int			take;
	int			save;
	int			knockback;
	qboolean	headShot;
	qboolean	wasAlive;
	qboolean	tank=qfalse;
	hitRegion_t	hr = HR_NUM_HITREGIONS;
	int			limbo_health;
 	int			hsDamage; //Perro: tunable headshot damage
 	int			ffDamage; //Perro: FF reflecting damage
 	float		hsRatio; // Replacement for g_dmg 8

	if (!targ->takedamage) {
		return;
	}

//	G_Printf("Damage target: %i.\n", targ->s.number);

#ifdef SAVEGAME_SUPPORT
	if( g_gametype.integer == GT_SINGLE_PLAYER && ( g_reloading.integer || saveGamePending ) )
		return;
#endif // SAVEGAME_SUPPORT

//	trap_SendServerCommand( -1, va("print \"%i\n\"\n", targ->health) );

	// the intermission has allready been qualified for, so don't
	// allow any extra scoring
	// CHRUKER: b024 - Don't do damage if at warmup and warmupdamage is
	//          set to 'None' and the target is a client.
	if(level.intermissionQueued ||
		(g_gamestate.integer != GS_PLAYING &&
		 match_warmupDamage.integer == 0 &&
		targ->client)) {

		return;
	}

	if ( !inflictor ) {
		inflictor = &g_entities[ENTITYNUM_WORLD];
	}
	if ( !attacker ) {
		attacker = &g_entities[ENTITYNUM_WORLD];
	}

#ifdef __VEHICLES__
	if ( (targ->client && targ->client->ps.eFlags & EF_VEHICLE) 
		|| targ->s.eType == ET_VEHICLE
		/*|| !Q_stricmp (targ->classname, "panzertank") 
		|| !Q_stricmp (targ->classname, "churchilltank")  
		|| !Q_stricmp (targ->classname, "panzerivtank") 
		|| !Q_stricmp (targ->classname, "kv1tank") 
		|| !Q_stricmp (targ->classname, "stug3tank") 
		|| !Q_stricmp (targ->classname, "tigertank") 
		|| !Q_stricmp (targ->classname, "flak88antitank") 
		|| !Q_stricmp (targ->classname, "pakantitank")*/) 
	{
		tank = qtrue;
	}

	if ( tank && !G_WeaponIsExplosive( mod ) && mod != MOD_WATER && !g_allWeaponsDamageTanks.integer ) {
		return;
	}

	if ( tank ) {
		switch (mod)
		{
		case MOD_GRENADE_LAUNCHER:
		case MOD_GRENADE_PINEAPPLE:
		case MOD_MORTAR:
		case MOD_GPG40:
		case MOD_M7:
			damage*=1.2;
			//damage *= 0.1;
			break;
		case MOD_LANDMINE:
		case MOD_PANZERFAUST:
		case MOD_TANKSHELL:
        case MOD_PTRSSHELL://Masteries
			damage*=1.5;
			//damage *= 0.2;
			break;
		case MOD_AIRSTRIKE:
		case MOD_ARTY:
			damage*=0.4;
			//damage *= 0.05; // so players will not bother as much to spam airstrikes;
			break;
		case MOD_FLAMETHROWER:
			damage*=(int)((float)Q_TrueRand(4, 12)*(float)0.5); // UQ1: Vary flamer damage quite a lot...
			break;
		case MOD_WATER: // Drowing...
			damage*=1.0;
			break;
		default: // UQ1: g_allWeaponsDamageTanks.integer
			if (Q_TrueRand(0,5) == 1)
				damage = Q_TrueRand(1,25);
			else
				return;
			break;
		}
	}
#endif //__VEHICLES__

#ifdef __SHELLSHOCK__
	if (!tank)
	{
		new_player_pain(targ, damage, mod, point);
	}
#endif //__SHELLSHOCK__

	if (point)
		VectorCopy(point, targ->last_hit_point);
	else
		VectorClear(targ->last_hit_point);

	// Arnout: invisible entities can't be damaged
	if( targ->entstate == STATE_INVISIBLE ||
		targ->entstate == STATE_UNDERCONSTRUCTION ) {
		return;
	}

#ifdef __BOT__
	if (targ->client && targ->r.svFlags & SVF_BOT)
	{// We want bots to react to being shot by someone...
		if (attacker->client)
		{
			if (!OnSameTeam(targ, attacker))
			{
				targ->client->attacker = attacker;

/*				//if (!targ->enemy)
				{
					targ->enemy = attacker;
					AIMOD_InformTeamOfEnemy(targ);
				}*/
			}
		}
	}
#endif //__BOT__

	// perro:	optionally - exit if damage was dealt by a player who is no longer
	//			on a team (spec, disconnected, etc).  (g_dmg 64)
	if (attacker && attacker->client && (g_dmg.integer & COMBAT_NOSPEC_DMG))
	{	
		if (attacker->client->sess.sessionTeam != TEAM_AXIS &&
			attacker->client->sess.sessionTeam != TEAM_ALLIES)
		{
			return;
		}
	}
	// optional exit if target is disguised covie and should not take
	// splash damage (g_covert 16)
	if (targ -> client &&
		targ->client->ps.powerups[PW_OPS_DISGUISED] &&
		(g_coverts.integer & COVERTF_DISGUISE_NOSPLASH) &&
		G_WeaponIsExplosive(mod) &&
		mod != MOD_TELEFRAG )
	{
		return;
	}

	// xkan, 12/23/2002 - was the bot alive before applying any damage?
	wasAlive = (targ->health > 0);


	limbo_health = FORCE_LIMBO_HEALTH;
	if(g_forceLimboHealth.integer == 1) limbo_health = -150;

	// Arnout: combatstate
	if( targ->client && attacker && attacker->client && attacker != targ ) {
		if( g_gamestate.integer == GS_PLAYING ) {
			if( !OnSameTeam( attacker, targ ) ) {
				targ->client->combatState |= (1<<COMBATSTATE_DAMAGERECEIVED);
				attacker->client->combatState |= (1<<COMBATSTATE_DAMAGEDEALT);
			}
		}
	}

// JPW NERVE
	if ((targ->waterlevel >= 3) && (mod == MOD_FLAMETHROWER))
		return;
// jpw

	// shootable doors / buttons don't actually have any health
	if ( targ->s.eType == ET_MOVER && !(targ->isProp) && !targ->scriptName) {
		if ( targ->use && targ->moverState == MOVER_POS1 ) {
			G_UseEntity( targ, inflictor, attacker );
		}
		return;
	}


	// TAT 11/22/2002
	//		In the old code, this check wasn't done for props, so I put that check back in to make props_statue properly work	
	// 4 means destructible
	if ( targ->s.eType == ET_MOVER && (targ->spawnflags & 4) && !targ->isProp ) 
	{
		if( !G_WeaponIsExplosive( mod ) ) {
			return;
		}

		// check for team
		if( G_GetTeamFromEntity( inflictor ) == G_GetTeamFromEntity( targ ) ) {
			return;
		}
	} else if ( targ->s.eType == ET_EXPLOSIVE ) {
		if( targ->parent && G_GetWeaponClassForMOD( mod ) == 2 ) {
			return;
		}

		if( G_GetTeamFromEntity( inflictor ) == G_GetTeamFromEntity( targ ) ) {
			return;
		}

		if( G_GetWeaponClassForMOD( mod ) < targ->constructibleStats.weaponclass ) {
			return;
		}
	}
	else if ( targ->s.eType == ET_MISSILE && targ->methodOfDeath == MOD_LANDMINE ) {
		if( targ->s.modelindex2 ) {
			if( G_WeaponIsExplosive( mod ) ) {
				mapEntityData_t	*mEnt;

				if((mEnt = G_FindMapEntityData(&mapEntityData[0], targ-g_entities)) != NULL) {
					G_FreeMapEntityData( &mapEntityData[0], mEnt );
				}

				if((mEnt = G_FindMapEntityData(&mapEntityData[1], targ-g_entities)) != NULL) {
					G_FreeMapEntityData( &mapEntityData[1], mEnt );
				}

				if( attacker && attacker->client ) {
					AddScore( attacker, 1 );
					//G_AddExperience( attacker, 1.f );
				}

				G_ExplodeMissile(targ);
			}
		}
		return;
	} else if ( targ->s.eType == ET_CONSTRUCTIBLE ) {

		if( G_GetTeamFromEntity( inflictor ) == G_GetTeamFromEntity( targ ) ) {
			return;
		}

		if( G_GetWeaponClassForMOD( mod ) < targ->constructibleStats.weaponclass ) {
			return;
		}
		//bani - fix #238
		if ( mod == MOD_DYNAMITE ) {
			if( !( inflictor->etpro_misc_1 & 1 ) )
				return;
		}
	}

#ifdef __NPC__
	if ( targ->NPC_client )
		client = targ->NPC_client;
	else
		client = targ->client;
#else //!__NPC__
	client = targ->client;
#endif //__NPC__

	if ( client ) {
		if ( client->noclip || client->ps.powerups[PW_INVULNERABLE] ) {
			return;
		}
	}

	// check for godmode
	if ( targ->flags & FL_GODMODE ) {
		return;
	}

	if ( !dir ) {
		dflags |= DAMAGE_NO_KNOCKBACK;
	} else {
		VectorNormalize(dir);
	}

	knockback = damage;
	if ( knockback > 200 ) {
		knockback = 200;
	}
	if ( targ->flags & FL_NO_KNOCKBACK ) {
		knockback = 0;
	}
	if ( dflags & DAMAGE_NO_KNOCKBACK ) {
		knockback = 0;
	} else if( dflags & DAMAGE_HALF_KNOCKBACK ) {
		knockback *= 0.5f;
	}
	
	// ydnar: set weapons means less knockback
#ifdef __EF__
	if( client && (client->ps.weapon == WP_MORTAR_SET || client->ps.weapon == WP_MOBILE_MG42_SET || client->ps.weapon == WP_30CAL_SET || client->ps.weapon == WP_PTRS_SET) )
#else //!__EF__
	if( client && (client->ps.weapon == WP_MORTAR_SET || client->ps.weapon == WP_MOBILE_MG42_SET) )
#endif //__EF__
		knockback *= 0.5;

	// perro: prevents nade-boosting sploit when FF is off
	// unless people want it specifically
	if( targ->client && 
		(g_friendlyFire.integer == 0) && 
		(!(g_friendlyFireOpts.integer & FFOPTS_ALLOW_BOOSTING)) &&
		OnSameTeam(targ, attacker) ) {
		knockback = 0;
	}
	// perro: prevent funky knockback if g_dmgXXX is set below zero
	if (knockback <= 0){
		knockback = 0;
	}
	
	// figure momentum add, even if the damage won't be taken
	if ( knockback && targ->client ) {
		vec3_t	kvel;
		float	mass;

		mass = 200;

		VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
		VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);

		if (targ == attacker && !(	mod != MOD_ROCKET &&
									mod != MOD_GRENADE &&
									mod != MOD_GRENADE_LAUNCHER &&
									mod != MOD_DYNAMITE
									&& mod != MOD_GPG40
									&& mod != MOD_M7
									&& mod != MOD_LANDMINE
									))
		{
			targ->client->ps.velocity[2] *= 0.25;
		}

		// set the timer so that the other client can't cancel
		// out the movement immediately
		if ( !targ->client->ps.pm_time ) {
			int		t;

			t = knockback * 2;
			if ( t < 50 ) {
				t = 50;
			}
			if ( t > 200 ) {
				t = 200;
			}
			targ->client->ps.pm_time = t;
			targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		}
	}

	if ( damage <= 0 ) {
		damage = 0;
	}
	take = damage;
	save = 0;

	// adrenaline junkie!
	if( targ->client && targ->client->ps.powerups[PW_ADRENALINE] ) {
		take *= .5f;
	}

	// save some from flak jacket
/*	if( targ->client && targ->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 4 && targ->client->sess.playerType == PC_ENGINEER 
#ifdef __VEHICLES__
		&& !(targ->client->ps.eFlags & EF_VEHICLE) 
#endif //__VEHICLES__
		) {*/
 	if( targ->client 
#ifdef __VEHICLES__
		&& !(targ->client->ps.eFlags & EF_VEHICLE) 
#endif //__VEHICLES__
 	    && targ->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 4
 	    && (targ->client->sess.playerType == PC_ENGINEER 
 	      || (g_skills.integer & SKILLS_FLAK))) {
		if( mod == MOD_GRENADE ||
			mod == MOD_GRENADE_LAUNCHER ||
			mod == MOD_ROCKET ||
			mod == MOD_GRENADE_PINEAPPLE ||
			mod == MOD_MAPMORTAR ||
			mod == MOD_MAPMORTAR_SPLASH || 
			mod == MOD_EXPLOSIVE ||
			mod == MOD_LANDMINE ||
			mod == MOD_GPG40 ||
			mod == MOD_M7 ||
			mod == MOD_SATCHEL ||
			mod == MOD_ARTY ||
			mod == MOD_AIRSTRIKE ||
			mod == MOD_DYNAMITE ||
			mod == MOD_MORTAR ||
			mod == MOD_PANZERFAUST ||
			mod == MOD_MAPMORTAR ) {
			take -= take * .5f;
		}
	}

  // perro: optionally do half-damage if the target is a covie in disguise (g_coverts 8)
  if (targ -> client &&
	  targ->client->ps.powerups[PW_OPS_DISGUISED] &&
	  (g_coverts.integer & COVERTF_DISGUISE_HALFDMG) )
  {
		take *= .5f;
  }

	headShot = IsHeadShot(attacker, targ, dir, point, mod);
	if ( headShot ) {
			// Perro: Allow tunables to work with both traditional and new damage code
			if (g_dmgHeadShotMin.integer <= 0){
				hsDamage = 0;
			}else if (g_dmgHeadShotMin.integer >= 500){
				hsDamage = 500;
			}else {
				hsDamage = g_dmgHeadShotMin.integer;
			}
		
			// Perro: Alternate head shot using tjw's g_dmgHeadShotRatio 

			if (g_dmgHeadShotRatio.value <= 0){
				hsRatio = 0.0f;
			}else if (g_dmgHeadShotRatio.value >= 100.f){
				hsRatio = 100.f;
			}else {
				hsRatio = g_dmgHeadShotRatio.value;
			}

			if( take * 2 < hsDamage ) // head shots, all weapons, do minimum hsDamage points damage
				take = hsDamage;
		else
				take *= hsRatio; // sniper rifles can do full-kill (and knock into limbo)


		if( dflags & DAMAGE_DISTANCEFALLOFF ) {
			vec_t dist;
			vec3_t shotvec;
			float scale;

			VectorSubtract( point, muzzleTrace, shotvec );
			dist = VectorLength( shotvec );

			// ~~~---______
			// zinx - start at 100% at 1500 units (and before),
			// and go to 20% at 2500 units (and after)
			// 1500 to 2500 -> 0.0 to 1.0
			scale = (dist - 1500.f) / (2500.f - 1500.f);
			// 0.0 to 1.0 -> 0.0 to 0.8
			scale *= 0.8f;
			// 0.0 to 0.8 -> 1.0 to 0.2
			scale = 1.0f - scale;
			// And, finally, cap it.
			if (scale > 1.0f) scale = 1.0f;
			else if (scale < 0.2f) scale = 0.2f;

			//Perro: Optional Advanced Combat - stricter scaling
			if (g_dmg.integer & COMBAT_USE_ALTDIST){
				// 100% at 500 units, to 20% at 1500 units and beyond
				scale = (dist - 500.f) / (1500.f - 500.f);				
				scale *= 0.8f;
				scale = 1.0f - scale;
				if (scale > 1.0f) scale = 1.0f;
				else if (scale < 0.2f) scale = 0.2f;
			}
			take *= scale;
		}

		//Perro: compute damage reduction for non-sniper rifles if player is wearing helmet
		if( !(targ->client->ps.eFlags & EF_HEADSHOT) ) {
			if( mod != MOD_K43_SCOPE &&
				mod != MOD_GARAND_SCOPE ) {
				take *= .8f;	// helmet gives us some protection
			}
		}

		// OSP - Record the headshot
		if(client && attacker && attacker->client
#ifndef DEBUG_STATS
		  && attacker->client->sess.sessionTeam != targ->client->sess.sessionTeam
#endif
		  ) {
			G_addStatsHeadShot(attacker, mod);
		}

#ifdef __DEBUGGING__
		if( g_debugBullets.integer ) {
			trap_SendServerCommand( attacker-g_entities, "print \"Head Shot\n\"\n");
		}
#endif //__DEBUGGING__

		G_LogRegionHit( attacker, HR_HEAD );
		hr = HR_HEAD;
	} else if ( IsLegShot(attacker, targ, dir, point, mod) ) {
		G_LogRegionHit( attacker, HR_LEGS );
		hr = HR_LEGS;
		//Perro: Optional Adv. Combat - consider hit locations
		if (g_dmg.integer & COMBAT_USE_HITLOC){
			take *= 0.6f;			
		}
#ifdef __DEBUGGING__
		if( g_debugBullets.integer ) {
			trap_SendServerCommand( attacker-g_entities, "print \"Leg Shot\n\"\n");
		}
#endif //__DEBUGGING__
	} else if ( IsArmShot(targ, attacker, point, mod) ) {
		G_LogRegionHit( attacker, HR_ARMS );
		hr = HR_ARMS;
		//Perro: Optional Adv. Combat - consider hit locations
		if (g_dmg.integer & COMBAT_USE_HITLOC){
		take *= 0.4f;	
		}
#ifdef __DEBUGGING__
		if( g_debugBullets.integer ) {
			trap_SendServerCommand( attacker-g_entities, "print \"Arm Shot\n\"\n");
		}
#endif //__DEBUGGING__
	} else if (targ->client && targ->health > 0 && IsHeadShotWeapon( mod ) ) {
		G_LogRegionHit( attacker, HR_BODY );
		hr = HR_BODY;
#ifdef __DEBUGGING__
		if( g_debugBullets.integer ) {
			trap_SendServerCommand( attacker-g_entities, "print \"Body Shot\n\"\n");
		}
#endif //__DEBUGGING__
	}

#ifdef __DEBUGGING__
#ifndef DEBUG_STATS
	if ( g_debugDamage.integer )
#endif
	{
		G_Printf( "client:%i health:%i damage:%i mod:%s\n", targ->s.number, targ->health, take, modNames[mod] );
	}
#endif //__DEBUGGING__

	// Perro: Optional Advanced Combat - Close-in bonus here
	if (g_dmg.integer & COMBAT_USE_CCBONUS){
		// check for qualifying weapon
		if( dflags & DAMAGE_DISTANCEFALLOFF ) {
			vec_t dist;
			vec3_t shotvec;
			VectorSubtract( point, muzzleTrace, shotvec );
			dist = VectorLength( shotvec );
			// check for qualifying distance
			if (dist <=100){
				take *= 1.05; 
			}
		}
	}
		

	// check for completely getting out of the damage
	if ( !(dflags & DAMAGE_NO_PROTECTION) ) {
		// if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
		// if the attacker was on the same team
		if ( targ != attacker && OnSameTeam (targ, attacker)  ) {
			if ( (g_gamestate.integer != GS_PLAYING && match_warmupDamage.integer == 1)) {
				return;
			}
			else if (!g_friendlyFire.integer) {
				// record "fake" pain - although the bot is not really hurt, 
				// his feeling have been hurt :-)
				// well at least he wants to shout "watch your fire".
				if (targ->s.number < level.maxclients && targ->r.svFlags & SVF_BOT) {
					BotRecordPain( targ->s.number, attacker->s.number, mod );
				}
				//Perro:  Shrub-style reflecting damage.
				if (g_reflectFriendlyFire.value > 0){
					// is this weapon one that will reflect?
					if (IsFFReflectable(mod)){
						//do we have a viable attacker entity?  Better safe than sorry!
						if (attacker && targ && (targ->health > 0)){
							//compute how much damage to take away
							ffDamage = take  * g_reflectFriendlyFire.value;
							if (ffDamage <= 0){
								ffDamage = 0;
							}
							// apply the damage...
							attacker->health -= ffDamage;
							// fatal?
							if (attacker->health <=0 ){
								attacker->deathType = MOD_REFLECTED_FF;
								attacker->enemy = attacker;
								if (attacker->die){
									attacker->die(attacker, attacker, attacker, ffDamage, MOD_REFLECTED_FF);
								}
							} 
						}
					}
				}
			// is FF for mines override bit turned on?  If so, don't exit...
			if (!((mod == MOD_LANDMINE) && 
				(g_friendlyFireOpts.integer & FFOPTS_MINE_OVERRIDE))){
					return;
				}
			}
		}
	}

	// Perro: if headshot, toss the helmet (only once, though!)
	if (headShot){
		if( !(targ->client->ps.eFlags & EF_HEADSHOT) ) {
			G_AddEvent( targ, EV_LOSE_HAT, DirToByte(dir) );
		}
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if ( client ) {
		if ( attacker ) {
			client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
		} else {
			client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
		}
		client->damage_blood += take;
		client->damage_knockback += knockback;

		if ( dir ) {
			VectorCopy ( dir, client->damage_from );
			client->damage_fromWorld = qfalse;
		} else {
			VectorCopy ( targ->r.currentOrigin, client->damage_from );
			client->damage_fromWorld = qtrue;
		}
	}

	// add to the attacker's hit counter
	if ( attacker->client && 
			targ->client &&
			targ != attacker &&
			targ->health > limbo_health ) {
		
		if(OnSameTeam( targ, attacker)) {
			attacker->client->ps.persistant[PERS_HITS] -= damage;
		}
		else {
			attacker->client->ps.persistant[PERS_HITS] += damage;
		}
		G_Hitsound(targ,
			attacker,
			mod, 
			((targ->health - take) <= limbo_health),
			headShot);
				
	}

	// remember that this player has no helmet
	if(headShot) targ->client->ps.eFlags |= EF_HEADSHOT; 


	if (targ->client) {
		// set the last client who damaged the target
		targ->client->lasthurt_client = attacker->s.number;
		targ->client->lasthurt_mod = mod;
		targ->client->lasthurt_time = level.time;
	}

	// do the damage
	if( take ) {
		targ->health -= take;

		// Gordon: don't ever gib POWS
		if( ( targ->health <= 0 ) && ( targ->r.svFlags & SVF_POW ) ) {
			targ->health = -1;
		}

		// Ridah, can't gib with bullet weapons (except VENOM)
		// Arnout: attacker == inflictor can happen in other cases as well! (movers trying to gib things)
		//if ( attacker == inflictor && targ->health <= GIB_HEALTH) {
		if( targ->health <= GIB_HEALTH ) {
			if( !G_WeaponIsExplosive( mod ) ) {
				targ->health = GIB_HEALTH + 1;
			}
		}

// JPW NERVE overcome previous chunk of code for making grenades work again
//		if ((take > 190)) // 190 is greater than 2x mauser headshot, so headshots don't gib
		// Arnout: only player entities! messes up ents like func_constructibles and func_explosives otherwise
		if( ( (targ->s.number < MAX_CLIENTS) && (take > 190) ) && !(targ->r.svFlags & SVF_POW) ) {
			targ->health = GIB_HEALTH - 1;
		}

		if( targ->s.eType == ET_MOVER && !Q_stricmp( targ->classname, "script_mover" ) ) {
			targ->s.dl_intensity = 255.f * (targ->health / (float)targ->count);	// send it to the client
		}

		if (tank && targ->client)
		{// UQ1: new - tanks send percentage of maxhealth...
			targ->client->ps.stats[STAT_HEALTH] = (int)((float)((float)targ->health/(float)targ->maxhealth)*(float)100); // send health to clients
		}

		//G_Printf("health at: %d\n", targ->health);
		if( targ->health <= 0 ) {
			if( client && !wasAlive ) {
				targ->flags |= FL_NO_KNOCKBACK;
				// OSP - special hack to not count attempts for body gibbage
#ifdef __NPC__
				if( (targ->client && targ->client->ps.pm_type == PM_DEAD)
					|| (targ->NPC_client && targ->NPC_client->ps.pm_type == PM_DEAD)) {
#else //!__NPC__
				if( targ->client->ps.pm_type == PM_DEAD ) {
#endif //__NPC__
					G_addStats(targ, attacker, take, mod);
				}

				if( targ->client && (targ->health < limbo_health) && (targ->health > GIB_HEALTH) ) {
					limbo(targ, qtrue, qfalse);
				}

			} else {
				targ->sound1to2 = hr;
				targ->sound2to1 = mod;
				targ->sound2to3 = (dflags & DAMAGE_RADIUS) ? 1 : 0;

				if( client ) 
				{
					if( G_GetTeamFromEntity( inflictor ) != G_GetTeamFromEntity( targ ) ) 
					{
						G_AddKillSkillPoints( attacker, mod, hr, (dflags & DAMAGE_RADIUS) );
						// perro: if g_damageXP 1 is set, add some damage-based XP also
						// avoid attempting to give damage XP to the world
						// or awarding damage XP for 'killing' non-players or tks
						if (attacker && 
							attacker->client && 
							targ &&
							targ ->client &&
							(!(OnSameTeam(targ, attacker))) &&
							g_damageXP.integer == 1)
						{
							attacker->client->sess.XPdmg += take;
							G_AddDamageXP(attacker, mod);
						}
					}
				}

				if( targ->health < -999 ) {
					targ->health = -999;
				}


				targ->enemy = attacker;
				targ->deathType = mod;

				// Ridah, mg42 doesn't have die func (FIXME)
				if( targ->die ) {	
					// Kill the entity.  Note that this funtion can set ->die to another
					// function pointer, so that next time die is applied to the dead body.
					targ->die( targ, inflictor, attacker, take, mod );
					// OSP - kill stats in player_die function
				}

				if( targ->s.eType == ET_MOVER && !Q_stricmp( targ->classname, "script_mover" ) && (targ->spawnflags & 8) ) {
					return;	// reseructable script mover doesn't unlink itself but we don't want a second death script to be called
				}

				// if we freed ourselves in death function
				if (!targ->inuse)
					return;

				// RF, entity scripting
				if ( targ->health <= 0) {	// might have revived itself in death function
					if( targ->r.svFlags & SVF_BOT ) {
						// See if this is the first kill of this bot
						if (wasAlive)
							Bot_ScriptEvent( targ->s.number, "death", "" );
					} else if(	( targ->s.eType != ET_CONSTRUCTIBLE && targ->s.eType != ET_EXPLOSIVE ) ||
								( targ->s.eType == ET_CONSTRUCTIBLE && !targ->desstages ) )	{ // call manually if using desstages
						G_Script_ScriptEvent( targ, "death", "" );
					}
				}

				// RF, record bot death
				if (targ->s.number < level.maxclients && targ->r.svFlags & SVF_BOT) {
					BotRecordDeath( targ->s.number, attacker->s.number );

				}
			}

		} else if ( targ->pain ) {
			if (dir) {	// Ridah, had to add this to fix NULL dir crash
				VectorCopy (dir, targ->rotate);
				VectorCopy (point, targ->pos3); // this will pass loc of hit
			} else {
				VectorClear( targ->rotate );
				VectorClear( targ->pos3 );
			}

			targ->pain (targ, attacker, take, point);
		} else {
			// OSP - update weapon/dmg stats
			G_addStats(targ, attacker, take, mod);

			// Perro - if damage-based XP is on, take care of it here...
			if (attacker && 
				attacker->client && 
				targ &&
				targ ->client &&
				(!(OnSameTeam(targ, attacker))) &&
				g_damageXP.integer)
			{
				// increment the attacker's rolling damage total
				attacker->client->sess.XPdmg += take;
				G_AddDamageXP(attacker, mod);
			}

			// OSP
		}

		// RF, entity scripting
		G_Script_ScriptEvent( targ, "pain", va("%d %d", targ->health, targ->health+take) );
		if (targ->s.number < MAX_CLIENTS && (targ->r.svFlags & SVF_BOT)) {
			Bot_ScriptEvent( targ->s.number, "pain", va("%d %d", targ->health, targ->health+take) );
		}

		// RF, record bot pain
		if (targ->s.number < level.maxclients && targ->r.svFlags & SVF_BOT) {
			BotRecordPain( targ->s.number, attacker->s.number, mod );
		}

		// Ridah, this needs to be done last, incase the health is altered in one of the event calls
		if ( targ->client && (!(targ->client->ps.eFlags & EF_PLAYDEAD) || targ->health < 1)) {
			targ->client->ps.stats[STAT_HEALTH] = targ->health;
		}
		// Perro:  allow for reflecting teamdamage even when FF is on
		if ( targ != attacker && OnSameTeam (targ, attacker)  ) {
			if ( (g_gamestate.integer != GS_PLAYING && match_warmupDamage.integer == 1)) {
				return;
			}
			// don't double-damage if landmine FF override is on...
			if ((mod == MOD_LANDMINE) && 
				(g_friendlyFireOpts.integer & FFOPTS_MINE_OVERRIDE))
				return;
			if (g_reflectFriendlyFire.value > 0){
				// is this weapon one that will reflect?
				if (IsFFReflectable(mod)){
					if (attacker){
						ffDamage = take  * g_reflectFriendlyFire.value;
						if (ffDamage <= 0){
							ffDamage = 0;
						}
						attacker->health -= ffDamage;
						if (attacker->health <=0){
							attacker->deathType = MOD_REFLECTED_FF;
							if (attacker->die){
								attacker->enemy = attacker;
								attacker->die(attacker, attacker, attacker, ffDamage, MOD_REFLECTED_FF);
							}
						}
					}
				}
			}
		}
	}

	if (!tank)
	{
#ifdef __NPC__
		if (targ->s.eType == ET_NPC)
			npc_pain(targ, attacker, damage, point);
		else
#endif //__NPC__
			player_pain(targ, attacker, damage, point);
	}
}


/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/

void G_RailTrail( vec_t* start, vec_t* end ) {
	gentity_t* temp = G_TempEntity( start, EV_RAILTRAIL );
	VectorCopy( end, temp->s.origin2 );
	temp->s.dmgFlags = 0;
}

#define MASK_CAN_DAMAGE		(CONTENTS_SOLID | CONTENTS_BODY)

qboolean CanDamage (gentity_t *targ, vec3_t origin) {
	vec3_t	dest;
	trace_t	tr;
	vec3_t	midpoint;
	vec3_t offsetmins = { -16.f, -16.f, -16.f };
	vec3_t offsetmaxs = { 16.f, 16.f, 16.f };

	// use the midpoint of the bounds instead of the origin, because
	// bmodels may have their origin is 0,0,0
	// Gordon: well, um, just check then...
	if(targ->r.currentOrigin[0] || targ->r.currentOrigin[1] || targ->r.currentOrigin[2]) {
		VectorCopy( targ->r.currentOrigin, midpoint );

		if( targ->s.eType == ET_MOVER ) {
			midpoint[2] += 32;
		}
	} else {
		VectorAdd (targ->r.absmin, targ->r.absmax, midpoint);
		VectorScale (midpoint, 0.5, midpoint);
	}

//	G_RailTrail( origin, dest );

	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, midpoint, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if (tr.fraction == 1.0)
		return qtrue;

	if(&g_entities[tr.entityNum] == targ)
		return qtrue;

	if( targ->client ) {
		VectorCopy( targ->client->ps.mins, offsetmins );
		VectorCopy( targ->client->ps.maxs, offsetmaxs );
	}

	// this should probably check in the plane of projection, 
	// rather than in world coordinate
	VectorCopy (midpoint, dest);
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmaxs[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmaxs[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmins[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmaxs[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmins[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmaxs[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	// =========================

	VectorCopy (midpoint, dest);
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmins[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmins[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmins[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmins[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy (midpoint, dest);
	dest[0] += offsetmins[0];
	dest[1] += offsetmins[2];
	dest[2] += offsetmins[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	return qfalse;
}

void G_AdjustedDamageVec( gentity_t *ent, vec3_t origin, vec3_t v )
{
	int i;

	if (!ent->r.bmodel)
		VectorSubtract(ent->r.currentOrigin,origin,v); // JPW NERVE simpler centroid check that doesn't have box alignment weirdness
	else {
		for ( i = 0 ; i < 3 ; i++ ) {
			if ( origin[i] < ent->r.absmin[i] ) {
				v[i] = ent->r.absmin[i] - origin[i];
			} else if ( origin[i] > ent->r.absmax[i] ) {
				v[i] = origin[i] - ent->r.absmax[i];
			} else {
				v[i] = 0;
			}
		}
	}
}

/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage( vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod ) {
	float		points, dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			i, e;
	qboolean	hitClient = qfalse;
	float		boxradius;
	vec3_t		dest; 
	trace_t		tr;
	vec3_t		midpoint;
	int			flags = DAMAGE_RADIUS;

	if( mod == MOD_SATCHEL || mod == MOD_LANDMINE ) {
		flags |= DAMAGE_HALF_KNOCKBACK;
	}

	if( radius < 1 ) {
		radius = 1;
	}

	boxradius = 1.41421356 * radius; // radius * sqrt(2) for bounding box enlargement -- 
	// bounding box was checking against radius / sqrt(2) if collision is along box plane
	for( i = 0 ; i < 3 ; i++ ) {
		mins[i] = origin[i] - boxradius;
		maxs[i] = origin[i] + boxradius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for( e = 0 ; e < level.num_entities ; e++ ) {
		g_entities[e].dmginloop = qfalse;
	}

	for( e = 0 ; e < numListedEntities ; e++ ) {
		ent = &g_entities[entityList[ e ]];

		// forty - dyno chaining
		// only if within blast radius and both on the same objective 
		// or both or no objectives.
		if (
			(g_dyno.integer & DYNO_CHAIN) &&
			mod == MOD_DYNAMITE && 
			ent->s.weapon == WP_DYNAMITE 
		) {
			#ifdef DEBUG
				G_Printf("dyno chaining:inflictor: %x, ent: %x\n", inflictor->onobjective, ent->onobjective);
			#endif

			if(inflictor->onobjective == ent->onobjective) {
				//blow up the other dynamite now too since they are peers
				//set the nextthink just past us by a 1/4 of a second or so.
				ent->nextthink = level.time + 250;
			} 
		}

		if( ent == ignore ) {
			continue;
		}
		if( !ent->takedamage && ( !ent->dmgparent || !ent->dmgparent->takedamage )) {
			continue;
		}

		G_AdjustedDamageVec( ent, origin, v );

		dist = VectorLength( v );
		if ( dist >= radius ) {
			continue;
		}

		points = damage * ( 1.0 - dist / radius );

		if( CanDamage( ent, origin ) ) {
			if( ent->dmgparent ) {
				ent = ent->dmgparent;
			}

			if( ent->dmginloop ) {
				continue;
			}

			if( AccuracyHit( ent, attacker ) ) {
				hitClient = qtrue;
			}
			VectorSubtract (ent->r.currentOrigin, origin, dir);
			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;


			G_Damage( ent, inflictor, attacker, dir, origin, (int)points, flags, mod );
		} else {
			VectorAdd( ent->r.absmin, ent->r.absmax, midpoint );
			VectorScale( midpoint, 0.5, midpoint );
			VectorCopy( midpoint, dest );
		
			trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
			if( tr.fraction < 1.0 ) {
				VectorSubtract( dest, origin, dest );
				dist = VectorLength( dest );
				if( dist < radius * 0.2f ) { // closer than 1/4 dist
					if( ent->dmgparent ) {
						ent = ent->dmgparent;
					}

					if( ent->dmginloop ) {
						continue;
					}

					if( AccuracyHit( ent, attacker ) ) {
						hitClient = qtrue;
					}
					VectorSubtract (ent->r.currentOrigin, origin, dir);
					dir[2] += 24;
					G_Damage( ent, inflictor, attacker, dir, origin, (int)(points*0.1f), flags, mod );
				}
			}
		}
	}
	return hitClient;
}

/*
============
etpro_RadiusDamage
mutation of G_RadiusDamage which lets us selectively damage only clients or only non clients
============
*/
qboolean etpro_RadiusDamage( vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod, qboolean clientsonly ) {
	float		points, dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			i, e;
	qboolean	hitClient = qfalse;
	float		boxradius;
	vec3_t		dest; 
	trace_t		tr;
	vec3_t		midpoint;
	int			flags = DAMAGE_RADIUS;

	if( mod == MOD_SATCHEL || mod == MOD_LANDMINE ) {
		flags |= DAMAGE_HALF_KNOCKBACK;
	}

	if( radius < 1 ) {
		radius = 1;
	}

	boxradius = 1.41421356 * radius; // radius * sqrt(2) for bounding box enlargement -- 
	// bounding box was checking against radius / sqrt(2) if collision is along box plane
	for( i = 0 ; i < 3 ; i++ ) {
		mins[i] = origin[i] - boxradius;
		maxs[i] = origin[i] + boxradius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for( e = 0 ; e < level.num_entities ; e++ ) {
		g_entities[e].dmginloop = qfalse;
	}

	for( e = 0 ; e < numListedEntities ; e++ ) {
		ent = &g_entities[entityList[ e ]];

		// forty - dyno chaining
		// only if within blast radius and both on the same objective 
		// or both or no objectives.
		if (
			(g_dyno.integer & DYNO_CHAIN) &&
			mod == MOD_DYNAMITE && 
			ent->s.weapon == WP_DYNAMITE 
		) {
			#ifdef DEBUG
				G_Printf("dyno chaining:inflictor: %x, ent: %x\n", inflictor->onobjective, ent->onobjective);
			#endif

			if(inflictor->onobjective == ent->onobjective) {
				//blow up the other dynamite now too since they are peers
				//set the nextthink just past us by a 1/4 of a second or so.
				ent->nextthink = level.time + 250;
			} 
		}

		if( ent == ignore ) {
			continue;
		}
		if( !ent->takedamage && ( !ent->dmgparent || !ent->dmgparent->takedamage )) {
			continue;
		}

		// tjw: need to include corpses in clientsonly since they 
		//      will be neglected from G_TempTraceIgnorePlayersAndBodies();
		if( clientsonly && !ent->client && ent->s.eType != ET_CORPSE) {
			continue;
		}
		if( !clientsonly && ent->client ) {
			continue;
		}

		G_AdjustedDamageVec( ent, origin, v );

		dist = VectorLength( v );
		if ( dist >= radius ) {
			continue;
		}

		points = damage * ( 1.0 - dist / radius );

		if( CanDamage( ent, origin ) ) {
			if( ent->dmgparent ) {
				ent = ent->dmgparent;
			}

			if( ent->dmginloop ) {
				continue;
			}

			if( AccuracyHit( ent, attacker ) ) {
				hitClient = qtrue;
			}
			VectorSubtract (ent->r.currentOrigin, origin, dir);
			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;

			G_Damage( ent, inflictor, attacker, dir, origin, (int)points, flags, mod );
		} else {
			VectorAdd( ent->r.absmin, ent->r.absmax, midpoint );
			VectorScale( midpoint, 0.5, midpoint );
			VectorCopy( midpoint, dest );
		
			trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
			if( tr.fraction < 1.0 ) {
				VectorSubtract( dest, origin, dest );
				dist = VectorLength( dest );
				if( dist < radius * 0.2f ) { // closer than 1/4 dist
					if( ent->dmgparent ) {
						ent = ent->dmgparent;
					}

					if( ent->dmginloop ) {
						continue;
					}

					if( AccuracyHit( ent, attacker ) ) {
						hitClient = qtrue;
					}
					VectorSubtract (ent->r.currentOrigin, origin, dir);
					dir[2] += 24;
					G_Damage( ent, inflictor, attacker, dir, origin, (int)(points*0.1f), flags, mod );
				}
			}
		}
	}

	return hitClient;
}
//==========================================================================

char *G_ObituarySanitize(char *text)
{
	static char n[MAX_SAY_TEXT] = {""};

	if(!*text)
		return n;
	Q_strncpyz(n, text, sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[a]", "(a)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[v]", "(v)"), sizeof(n));
	return n;
}

/*
=============
G_Obituary
=============
*/
/*static*/ void G_Obituary(int mod, int target, int attacker) {
	char		*message;
	char		*tk;
	char 		targetName[MAX_NAME_LENGTH+2] = {"*unknown*"};
	char 		attackerName[MAX_NAME_LENGTH+2] = {"*unknown*"};
	char		obit[MAX_CVAR_VALUE_STRING]; //Perro - custom obits
	
    gentity_t	*gi, *ga; // JPW NERVE gi=target, ga = attacker
	//qhandle_t	deathShader = cgs.media.pmImages[PM_DEATH];

#ifndef __NPC__
	if ( target < 0 || target >= MAX_CLIENTS ) {
		G_Error( "G_Obituary: target out of range" );
	}
#endif //__NPC__

	gi = &g_entities[target];

#ifdef __NPC__
	if ( attacker < 0 && !g_entities[attacker].NPC_client  ) {
#else //!__NPC__
	if ( attacker < 0 || attacker >= MAX_CLIENTS ) {
#endif //__NPC__
		attacker = ENTITYNUM_WORLD;
		ga = NULL;
	} else {
		ga = &g_entities[attacker];
	}

#ifdef __NPC__
	if (gi->s.eType == ET_NPC)
	{
		//strcpy( targetName, "An NPC" );
		Q_strncpyz(targetName, G_ObituarySanitize(gi->NPC_client->pers.netname), sizeof(targetName));
		Q_strcat(targetName, sizeof(targetName), S_COLOR_CYAN);
	}
	else
#endif //__NPC__
	{
		Q_strncpyz(targetName, G_ObituarySanitize(gi->client->pers.netname), sizeof(targetName));
		Q_strcat(targetName, sizeof(targetName), S_COLOR_CYAN);
	}

	message = "";
	tk = "";

	// check for single client messages
	switch( mod ) {
	case MOD_SUICIDE:
		message = "[v]^7 committed suicide.";
		break;
	case MOD_FALLING:
		message = "[v]^7 fell to his death.";
		break;
	case MOD_CRUSH:
		message = "[v]^7 was crushed.";
		break;
	case MOD_WATER:
		message = "[v]^7 drowned.";
		break;
	case MOD_SLIME:
		message = "[v]^7 died by toxic materials.";
		break;
	case MOD_TRIGGER_HURT:
	case MOD_TELEFRAG: // rain - added TELEFRAG and TARGET_LASER, just in case
	case MOD_TARGET_LASER:
		message = "[v]^7 was killed.";
		break;
	case MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER:
		message = "[v]^7 got buried under a pile of rubble.";
		break;
	case MOD_LAVA: // rain
		message = "[v]^7 was incinerated.";
		break;
	default:
		message = NULL;
		break;
	}

	// Perro - Configurable Obits -- world objects: mod is used to index 
	// dynamic cvarwe want to assign message to match the cvar ONLY if
	// one of the above actually worked.  this avoids erroneous printing
	// of world/suicide obits during combat...
	// If the dynamic cvar is not present or is null, we will use the
	// default.
	if (message) {
		trap_Cvar_VariableStringBuffer(va("g_obit%d",mod),
				obit,
				sizeof(obit));
		if (Q_stricmp(obit,"")) {
				message = obit;
		}
	}

	if( attacker == target ) {
		switch (mod) {
		case MOD_DYNAMITE:
			message = "[v]^7 dynamited himself to pieces.";
			break;
		case MOD_GRENADE_LAUNCHER:
		case MOD_GRENADE_PINEAPPLE: // rain - added PINEAPPLE
			message = "[v]^7 dove on his own grenade.";
			break;
		case MOD_PANZERFAUST:
			message = "[v]^7 vaporized himself.";
			break;
		case MOD_FLAMETHROWER: // rain
			message = "[v]^7 played with fire.";
			break;
		case MOD_AIRSTRIKE:
			message = "[v]^7 obliterated himself.";
			break;
		case MOD_ARTY:
			message = "[v]^7 fired-for-effect on himself.";
			break;
		case MOD_EXPLOSIVE:
			message = "[v]^7 died in his own explosion.";
			break;
		// rain - everything from this point on is sorted by MOD, didn't
		// resort existing messages to avoid differences between pre
		// and post-patch code (for source patching)
		case MOD_GPG40:
		case MOD_M7: // rain
			//bani - more amusing, less wordy
			message = "[v]^7 ate his own rifle grenade.";
			break;
		case MOD_LANDMINE: // rain
			//bani - slightly more amusing
			message = "[v]^7 failed to spot his own landmine.";
			break;
		case MOD_SATCHEL: // rain
			message = "[v]^7 embraced his own satchel explosion.";
			break;
		case MOD_TRIPMINE: // rain - dormant code
			message = "[v]^7 forgot where his tripmine was.";
			break;
		case MOD_CRUSH_CONSTRUCTION: // rain
			message = "[v]^7 engineered himself into oblivion.";
			break;
		case MOD_CRUSH_CONSTRUCTIONDEATH: // rain
			message = "[v]^7 buried himself alive.";
			break;
		case MOD_MORTAR: // rain
			message = "[v]^7 never saw his own mortar round coming.";
			break;
		case MOD_SMOKEGRENADE: // rain
			// bani - more amusing
			message = "[v]^7 danced on his airstrike marker.";
			break;
		case MOD_REFLECTED_FF: // perro
			message = "[v]^7 was killed by his own friendly fire.";
			break;
		// no obituary message if changing teams
		case MOD_SWITCHTEAM:
			return;
		default:
			message = "[v]^7 killed himself.";
			break;
		}
		// Perro 
		// Configurable obits -- self-inflicted: use mod to index the 
		// dynamic cvar.  If not present or null, just use default...
		trap_Cvar_VariableStringBuffer(va("g_obit%d",mod),
			obit,
			sizeof(obit));
		if (Q_stricmp(obit,"")) {
			message = obit;
		}
	}
	if (message) {
		// perro: use the parser to replace [v] with the victim's name
		message = Q_StrReplace(message, 
					"[v]", 
					va("%s", targetName));

		if (g_logOptions.integer & LOGOPTS_OBIT_CHAT) {
			AP(va("chat \"%s\"",message));
		} else {
			trap_SendServerCommand( -1,va("cpm \"%s\n\"", message));
		}
		return;
	}

	// check for kill messages from the current clientNum
	if( ga ) {
		char	*s;

		//if ( gi->client && ga->client && gi->client->sess.sessionTeam == ga->client->sess.sessionTeam ) {
		if (OnSameTeam(gi, ga)) {
			if (mod == MOD_SWAP_PLACES) {
				s = va("%s %s", "You swapped places with" , targetName );
			} else {
				s = va("%s %s", "You killed ^1TEAMMATE^7" , targetName );
			}
		} else {
			s = va("%s %s", "You killed" , targetName );
		}
		trap_SendServerCommand( attacker, va("cp \"%s\"",s));
		// print the text message as well
	}

	// check for double client messages
	if ( !ga ) {
		strcpy( attackerName, "noname" );
#ifdef __NPC__
	} else if (ga->s.eType == ET_NPC) {
		//strcpy( attackerName, "an NPC" );
		Q_strncpyz(attackerName,
			G_ObituarySanitize(ga->NPC_client->pers.netname), sizeof(attackerName));
		Q_strcat(attackerName, sizeof(attackerName), S_COLOR_CYAN);
#endif //__NPC__
	} else {
		Q_strncpyz(attackerName,
			G_ObituarySanitize(ga->client->pers.netname),
			sizeof(attackerName));
		Q_strcat(attackerName, sizeof(attackerName), S_COLOR_CYAN);
	}

	if( ga ) {
		switch( mod ) {
		case MOD_KNIFE:
			message = "[v]^7 was stabbed by [a]^7's knife.";
			// josh: is the goat sound in standard ET? if so we could add this
			// OSP - goat luvin
//			if( attacker == cg.snap->ps.clientNum || target == cg.snap->ps.clientNum ) {
//				trap_S_StartSound( cg.snap->ps.origin, cg.snap->ps.clientNum, CHAN_AUTO, cgs.media.goatAxis );
//			}
			break;

		case MOD_AKIMBO_COLT:
		case MOD_AKIMBO_SILENCEDCOLT:
			message = "[v]^7 was killed by [a]^7's Akimbo TT-33s.";
			break;

		case MOD_AKIMBO_LUGER:
		case MOD_AKIMBO_SILENCEDLUGER:
			message = "[v]^7 was killed by [a]^7's Akimbo Luger 9mms.";
			break;

		case MOD_SILENCER:
		case MOD_LUGER:
			message = "[v]^7 was killed by [a]^7's Luger 9mm.";
			break;

		case MOD_SILENCED_COLT:
		case MOD_COLT:
			message = "[v]^7 was killed by [a]^7's TT-33.";
			break;

		case MOD_MP40:
			message = "[v]^7 was killed by [a]^7's MP-40.";
			break;

		case MOD_THOMPSON:
			message = "[v]^7 was killed by [a]^7's Thompson.";
			break;

		case MOD_PPSH:
			message = "[v]^7 was killed by [a]^7's PPSh.";
			break;

		case MOD_PPS:
			message = "[v]^7 was killed by [a]^7's PPS.";
			break;

		case MOD_STG44:
			message = "[v]^7 was killed by [a]^7's STG-44.";
			break;

		case MOD_SVT40:
			message = "[v]^7 was killed by [a]^7's SVT-40.";
			break;

		case MOD_30CAL:
			message = "[v]^7 was killed by [a]^7's Browning.";
			break;

		case MOD_STEN:
			message = "[v]^7 was killed by [a]^7's Sten.";
			break;

		case MOD_DYNAMITE:
			message = "[v]^7 was blasted by [a]^7's dynamite.";
			break;

		case MOD_PANZERFAUST:
			message = "[v]^7 was blasted by [a]^7's Panzerfaust.";
			break;

		case MOD_GRENADE_LAUNCHER:
		case MOD_GRENADE_PINEAPPLE:
			message = "[v]^7 was exploded by [a]^7's grenade.";
			break;

		case MOD_FLAMETHROWER:
			message = "[v]^7 was cooked by [a]^7's flamethrower.";
			break;

		case MOD_MORTAR:
			message = "[v]^7 never saw [a]^7's mortar round coming.";
			break;

		case MOD_MACHINEGUN:
			message = "[v]^7 was perforated by [a]^7's crew-served MG.";
			break;

		case MOD_BROWNING:
			message = "[v]^7 was perforated by [a]^7's tank-mounted browning 30cal.";
			break;

		case MOD_MG42:
			message = "[v]^7 was perforated by [a]^7's tank-mounted MG-42.";
			break;

		case MOD_AIRSTRIKE:
			message = "[v]^7 was blasted by [a]^7's support fire.";
			break;

		case MOD_ARTY:
			message = "[v]^7 was shelled by [a]^7's artillery support.";
			break;

		case MOD_SWAP_PLACES:
			message = "[v] ^2swapped places with^7 [a]^7.";
			break;

		case MOD_KAR98:	// same weapon really
		case MOD_K43:
			message = "[v]^7 was killed by [a]^7's K-43.";
			break;

		case MOD_CARBINE: // same weapon really
		case MOD_GARAND:
			message = "[v]^7 was killed by [a]^7's Garand.";
			break;

		case MOD_GPG40:
		case MOD_M7:
			message = "[v]^7 was killed by [a]^7's rifle grenade.";
			break;

		case MOD_LANDMINE:
			message = "[v]^7 failed to spot [a]^7's Landmine.";
			break;

		case MOD_CRUSH_CONSTRUCTION:
			message = "[v]^7 got caught in [a]^7's construction madness.";
			break;

		case MOD_CRUSH_CONSTRUCTIONDEATH:
			message = "[v]^7 got burried under [a]^7's rubble.";
			break;

		case MOD_MOBILE_MG42:
			message = "[v]^7 was mown down by [a]^7's Mobile MG-42.";
			break;

		case MOD_GARAND_SCOPE:
			message = "[v]^7 was silenced by [a]^7's Garand.";
			break;

		case MOD_K43_SCOPE:
			message = "[v]^7 was silenced by [a]^7's K-43.";
			break;

		case MOD_FG42:
			message = "[v]^7 was killed by [a]^7's FG-42.";
			break;

		case MOD_FG42SCOPE:
			message = "[v]^7 was sniped by [a]^7's FG-42.";
			break;

		case MOD_SATCHEL:
			message = "[v]^7 was blasted by [a]^7's Satchel Charge.";
			break;
		
		case MOD_TRIPMINE: // rain - dormant code
			message = "[v]^7 was detonated by [a]^7's trip mine.";
			break;
		
		case MOD_SMOKEGRENADE: // rain
			message = "[v]^7 stood on [a]^7's airstrike marker.";
			break;

		case MOD_POISON: // josh
			message = "[v]^7 was poisoned by [a]^7's needle.";
			break;

		case MOD_GOOMBA: // josh
			message = "[v]^7 experienced death from above from [a]^7.";
			break;

		case MOD_FEAR: // tjw
			message = "[v]^7 was scared to death by [a]^7.";
			break;
		case MOD_THROWN_KNIFE: // matt
			message = "[v]^7 was killed by [a]^7's throwing knife.";
			break;

		default:
			message = "[v]^7 was killed by [a]^7.";
			break;
		}


	// Perro 
	// Configurable obits -- team vs. other team use mod+200 to index the 
	// dynamic cvar. If not present or null, just use default...
	
	trap_Cvar_VariableStringBuffer(va("g_obit%d",mod+200),
			obit,
			sizeof(obit));
	if (Q_stricmp(obit,"")) {
		message = obit;
	}
	
		//if( gi->client->sess.sessionTeam == ga->client->sess.sessionTeam ) {
		if( OnSameTeam(gi, ga) ) {
			if(!(g_logOptions.integer & LOGOPTS_TK_WEAPON)) {
				tk = "^7";
				message = "[v] ^1WAS KILLED BY TEAMMATE^7 [a]";
			} else {
				tk = "^1TEAMKILL:^7 ";
				//message = "[v] ^7[a]";
			}

			// Perro 
			// Configurable obits -- tk events use mod+200tk to index
			// the dynamic cvar. If not present or null, just use default...
			trap_Cvar_VariableStringBuffer(va("g_obit%dtk",mod+200),
					obit,
					sizeof(obit));
			if (Q_stricmp(obit,"")) {
				message = obit;
			}
		}

		if (message) {
			// perro: use the parser to replace [v] with the victim's name
			message = Q_StrReplace(message, 
						"[v]", 
						va("%s", targetName));

			// perro: and again to replace [a] with attacker's name
			message = Q_StrReplace(message, 
						"[a]", 
						va("%s", attackerName));

			if (g_logOptions.integer & LOGOPTS_OBIT_CHAT) {
				AP(va(
						"chat \"%s%s\"",
						tk,
						message));
			} else {
				trap_SendServerCommand(-1,va(
						"cpm \"%s%s\n\"",
						tk,
						message));
			}
		return;
		}
	}


	// we don't know what it was
	switch( mod ) {
		default:
			if (g_logOptions.integer & LOGOPTS_OBIT_CHAT) {
				AP(va(
					"chat \"%s^7 died.\"",
					targetName));
			} else {
				trap_SendServerCommand(-1,va(
					"cpm \"%s^7 died.\n\"",
					targetName));
			}
			break;
	}
}

// Perro: Compares the g_reflectFFWeapons bitmask cvar to the method
//		  of death.  If the weapontype is flagged as reflectable
//		  this will return qtrue.
qboolean IsFFReflectable(int mod)
{
	qboolean rval;
	switch (mod) {
		// Gunplay (g_reflectFFWeapons 1)		
		case MOD_AKIMBO_COLT:
		case MOD_AKIMBO_SILENCEDCOLT:
		case MOD_AKIMBO_LUGER:
		case MOD_AKIMBO_SILENCEDLUGER:
		case MOD_SILENCER:
		case MOD_LUGER:
		case MOD_SILENCED_COLT:
		case MOD_COLT:
		case MOD_MP40:
		case MOD_THOMPSON:
		case MOD_PPSH:
		case MOD_PPS:
		case MOD_STG44:
		case MOD_STEN:
		case MOD_MACHINEGUN:
		case MOD_BROWNING:
		case MOD_MG42:
		case MOD_KAR98:
		case MOD_K43:
		case MOD_CARBINE:
		case MOD_GARAND:
		case MOD_MOBILE_MG42:
		case MOD_GARAND_SCOPE:
		case MOD_K43_SCOPE:
		case MOD_FG42:
		case MOD_FG42SCOPE:
			rval = (g_reflectFFWeapons.integer & REFLECT_FF_BULLETS);
			break;
		// Nades (g_reflectFFWeapons 2)
		case MOD_GRENADE_LAUNCHER:
		case MOD_GRENADE_PINEAPPLE:
		case MOD_GPG40:
		case MOD_M7:
		case MOD_SMOKEGRENADE:
			rval = (g_reflectFFWeapons.integer & REFLECT_FF_GRENADES);
			break;
		// Knives (g_reflectFFWeapons 4)
		case MOD_KNIFE:
		case MOD_THROWN_KNIFE:
			rval = (g_reflectFFWeapons.integer & REFLECT_FF_KNIVES);
			break;
		// Panzer (g_reflectFFWeapons 8)
		case MOD_PANZERFAUST:
			rval = (g_reflectFFWeapons.integer & REFLECT_FF_PANZER);
			break;
		// Flamer (g_reflectFFWeapons 16)
		case MOD_FLAMETHROWER:
			rval = (g_reflectFFWeapons.integer & REFLECT_FF_FLAMER);
			break;
		// Mortar (g_reflectFFWeapons 32)		
		case MOD_MORTAR:
			rval = (g_reflectFFWeapons.integer & REFLECT_FF_MORTAR);
			break;
		// Satchel (g_reflectFFWeapons 64)	
		case MOD_SATCHEL:
			rval = (g_reflectFFWeapons.integer & REFLECT_FF_SATCHEL);
			break;
		// Arty/Air (g_reflectFFWeapons 128)
		case MOD_AIRSTRIKE:
		case MOD_ARTY:
			rval = (g_reflectFFWeapons.integer & REFLECT_FF_BOMBS);
			break;
		// Dyna and Construction (g_reflectFFWeapons 256)
		case MOD_DYNAMITE:
		case MOD_CRUSH_CONSTRUCTION:
		case MOD_CRUSH_CONSTRUCTIONDEATH:
			rval = (g_reflectFFWeapons.integer & REFLECT_FF_DYNAMITE);
			break;
		// Land Mines (g_reflectFFWeapons 512)
		case MOD_LANDMINE:
			rval = (g_reflectFFWeapons.integer & REFLECT_FF_LANDMINES);
			break;
		// Everything else (if in doubt, don't reflect)
		default: 
			rval = qfalse;
			break;
	}
	return (rval);
}



//==========================================================================

