#include "g_local.h"   //modified by Masteries 27/11/2007
                       // *tankSpawn
void BotDebug(int clientNum);
void GetBotAutonomies(int clientNum, int *weapAutonomy, int *moveAutonomy);	
qboolean G_IsOnFireteam(int entityNum, fireteamData_t** teamNum);

#ifdef __BOT__
vmCvar_t bot_node_edit;
extern void AIMod_CheckMapPaths ( gentity_t *ent );
extern void AIMod_ShowNodeLinks ( gentity_t *ent );
vmCvar_t bot_fakePing;
extern void AIMOD_SP_Debug_GotoCP ( gentity_t *ent, int number );
extern void AIMod_CreateNewRoute ( gentity_t *ent );
extern void ShowWaypointVisibilityInfo ( int wp1, int wp2 );
extern void AIMOD_Generate_Cover_Spots_Real ( void );
#endif //__BOT__

#ifdef __VEHICLES__
extern void SP_tank (gentity_t *self);
extern void SP_panzer (gentity_t *self);
extern void SP_panzeriv (gentity_t *self);//new
extern void SP_kv1 (gentity_t *self);//new
extern void SP_stug3 (gentity_t *self);//new  masteries
extern void SP_t34 (gentity_t *self);
extern void SP_bt7 (gentity_t *self);
extern void SP_flak88 (gentity_t *self);//Artillery masteries
extern void SP_pak (gentity_t *self);//Artillery masteries
extern void SP_flametank (gentity_t *self);//Flametank by Unique1
extern void SP_tigertank (gentity_t *self);
extern void SP_axis_apc (gentity_t *self);
extern void SP_allied_apc (gentity_t *self);
extern void SP_panzer2 (gentity_t *self);
#endif //__VEHICLES__

#ifdef __ETE__
extern void Create_Constructible_Airdrop( gentity_t *ent );
extern void G_ListSecondaryEntities ( void );
extern vec3_t	fog_color;
extern float	current_fog_view_distance;
extern float	maximum_fog_view_distance;
extern float	minimum_fog_view_distance;
extern qboolean is_fog_kludge_map;
extern qboolean fog_kludge_checked;

extern void SUPREMACY_SaveGameInfo ( void );
extern void SUPREMACY_LoadGameInfo ( void );
extern void SUPREMACY_AddHealthCrate ( vec3_t origin, vec3_t angles );
extern void SUPREMACY_AddAmmoCrate ( vec3_t origin, vec3_t angles );
extern void SUPREMACY_AddVehicle ( vec3_t origin, vec3_t angles, int vehicleType );
extern void SUPREMACY_AddFlag ( vec3_t origin, int team );
extern void SUPREMACY_ChangeFlagTeam ( int flag_number, int new_team );
extern void SUPREMACY_RaiseAllSupremacyEnts ( int modifier );
extern void SUPREMACY_ShowInfo ( void );
extern void SUPREMACY_RemoveAllSupremacyEnts ( void );
extern void SUPREMACY_RemoveAmmoCrate ( void );
extern void SUPREMACY_RemoveHealthCrate ( void );
#endif //__ETE__

/*
==================
G_SendScore

Sends current scoreboard information
==================
*/
/*
void G_SendScore( gentity_t *ent ) {
	char		entry[128];
	int			i;
	gclient_t	*cl;
	int			numSorted;
	int			team, size, count;
	char		buffer[1024];
	char		startbuffer[32];

	// send the latest information on all clients
	numSorted = level.numConnectedClients;
	if ( numSorted > 64 ) {
		numSorted = 64;
	}

	i = 0;
	// Gordon: team doesnt actually mean team, ignore...
	for(team = 0; team < 2; team++) {
		*buffer = '\0';
		*startbuffer = '\0';
		if( team == 0 ) {
			Q_strncpyz(startbuffer, va("sc0 %i %i", level.teamScores[TEAM_AXIS], level.teamScores[TEAM_ALLIES]), 32 );
		} else {
			Q_strncpyz(startbuffer, "sc1", 32 );
		}
		size = strlen(startbuffer) + 1;
		count = 0;

		for(; i < numSorted ; i++) {
			int		ping, playerClass, respawnsLeft;

			cl = &level.clients[level.sortedClients[i]];

			if(g_entities[level.sortedClients[i]].r.svFlags & SVF_POW) {
				continue;
			}

			// NERVE - SMF - if on same team, send across player class
			// Gordon: FIXME: remove/move elsewhere?
			if ( cl->ps.persistant[PERS_TEAM] == ent->client->ps.persistant[PERS_TEAM] || G_smvLocateEntityInMVList(ent, level.sortedClients[i], qfalse)) {
				playerClass = cl->ps.stats[STAT_PLAYER_CLASS];
			} else {
				playerClass = 0;
			}

			// NERVE - SMF - number of respawns left
			respawnsLeft = cl->ps.persistant[PERS_RESPAWNS_LEFT];
			if( g_gametype.integer == GT_WOLF_LMS ) {
				if( g_entities[level.sortedClients[i]].health <= 0 ) {
					respawnsLeft = -2;
				}
			} else {
				if( (respawnsLeft == 0 && ((cl->ps.pm_flags & PMF_LIMBO) || (( level.intermissiontime ) && g_entities[level.sortedClients[i]].health <= 0))) ) {
					respawnsLeft = -2;
				}
			}

#ifdef __BOT__
			if (g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT && !bot_fakePing.integer){
				ping = -2;
			} else
#endif			
			if ( cl->pers.connected == CON_CONNECTING ) {
				ping = -1;
			} else {
				ping = cl->ps.ping < 999 ? cl->ps.ping : 999;
			}

			if( g_gametype.integer == GT_WOLF_LMS ) {
				Com_sprintf (entry, sizeof(entry), " %i %i %i %i %i %i %i", level.sortedClients[i], cl->ps.persistant[PERS_SCORE], ping, 
					(level.time - cl->pers.enterTime) / 60000, g_entities[level.sortedClients[i]].s.powerups, playerClass, respawnsLeft );
			} else {
				int j, totalXP;

				for( totalXP = 0, j = 0; j < SK_NUM_SKILLS; j++ ) {
					totalXP += cl->sess.skillpoints[j];
				}

				Com_sprintf (entry, sizeof(entry), " %i %i %i %i %i %i %i", level.sortedClients[i], totalXP, ping, 
					(level.time - cl->pers.enterTime) / 60000, g_entities[level.sortedClients[i]].s.powerups, playerClass, respawnsLeft );
			}

			if(size + strlen(entry) > 1000) {
				i--; // we need to redo this client in the next buffer (if we can)
				break;
			}
			size += strlen(entry);

			Q_strcat(buffer, 1024, entry);
			if( ++count >= 32 ) {
				i--; // we need to redo this client in the next buffer (if we can)
				break;
			}
		}

		if(count > 0 || team == 0) {
			trap_SendServerCommand( ent-g_entities, va("%s %i%s", startbuffer, count, buffer));
		}
	}
}
*/

 // Josh: print out their kill rating information
 void G_KillRatingStats(gentity_t *ent) {
 	/*
 	CPx(-1,
 		va("chat \"^f[ ^7%s ^f] [Match: ^3%d^f] [Session:^3%d^f]\"",
 		ent->client->pers.netname,
 		ent->client->sess.match_killrating,
 		ent->client->sess.overall_killrating));
 	*/
  	int			i;
 	char name[MAX_NETNAME];
  	gclient_t	*cl;
 	int sort[MAX_CLIENTS];
  
 	for(i=0; i < level.numConnectedClients; i++)
 		sort[i] = level.sortedClients[i];
 	qsort(sort, i, sizeof(int), G_SortPlayersByKillRating);
 
 	CP(va("print \"^f%-3s %-24s %-10s %-10s\n\"",
 		"ID", "Player Name", "Match", "Overall"));
 	CP("print \"^f"
 		"-----------------------------------------------\n\"");
 	for(i=0; i < level.numConnectedClients; i++) {
 		name[0] = '\0';
 		cl = &level.clients[sort[i]];
 		SanitizeString(cl->pers.netname, name, qtrue);
 		name[24] = '\0';
 		CP(va("print \"%s%-3d %-24s %-10d %-10d\n\"",
 			(ent->s.number == sort[i]) ? "^3" : "",
 			sort[i],
 			name,
 			cl->sess.match_killrating,
 			cl->sess.overall_killrating));
  	}
 }
  
 // Josh: print out their kill rating information
 void G_PlayerRatingStats(gentity_t *ent) {
 	int i;
 	char name[MAX_NETNAME];
 	gclient_t *cl;
 	int sort[MAX_CLIENTS];
 
 	for(i=0; i < level.numConnectedClients; i++)
 		sort[i] = level.sortedClients[i];
 	qsort(sort, i, sizeof(int), G_SortPlayersByPlayerRating);
 
 	CP(va("print \"^f%-3s %-24s %-10s \n\"",
 		"ID", "Player Name", "Rating"));
 	CP("print \"^f"
 		"-----------------------------------------------\n\"");
 	for(i=0; i < level.numConnectedClients; i++) {
 		name[0] = '\0';
 		cl = &level.clients[sort[i]];
 		SanitizeString(cl->pers.netname, name, qtrue);
 		name[24] = '\0';
 		CP(va("print \"%s%-3d %-24s %-10d \n\"",
 			(ent->s.number == sort[i]) ? "^3" : "",
 			sort[i],
 			name,
 			cl->sess.playerrating));
  		}
 }
  
  
 void Force_Suicide( gentity_t *ent) {
 	if(ent->client->sess.sessionTeam == TEAM_SPECTATOR ||
 	  (ent->client->ps.pm_flags & PMF_LIMBO) ||
 	  ent->health <= 0 || level.match_pause != PAUSE_NONE) {
 		return;
 	}
 	ent->flags &= ~FL_GODMODE;
 	ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
  
 	// TTimo - if using /kill while at MG42
 	ent->client->ps.persistant[PERS_HWEAPON_USE] = 0;
 	player_die(ent, ent, ent,
 	(g_gamestate.integer == GS_PLAYING) ? 100000 : 135,
 	MOD_SUICIDE);
 }
 
 // Josh: like shrub, pretty naive
 qboolean G_CensorText(char *text, wordDictionary *dictionary)
 {
 	qboolean foundWord = qfalse, lastWordFound = qfalse;
 	int word = 0;
 	char *textPos = text;
 	while (*textPos) {
 		lastWordFound = qfalse;
 		word = 0;
 		while (word < dictionary->num_words && *textPos) {
 			if(!Q_stricmpn(textPos, dictionary->words[word], strlen(dictionary->words[word]))) {
 				foundWord = qtrue;
 				if (word == dictionary->num_words - 1) {
 					lastWordFound = qtrue;
 				}
 				{
 					int i;
 					for (i = 0; 
 					    i < strlen(
 					      dictionary->words[word]);
 					    i++) {
 						*textPos++ =  '*';
 					}
 				}
 			}
 			word++;
 		}
 		// If it's the last word, it will already be incremented
 		if (!lastWordFound)
 			textPos++;
 	}
 	return foundWord;
 }
 
 // Josh: Won't gib if already in limbo, same as shrub
 void G_CensorPenalize(gentity_t *ent)
 {
 	if (!ent->client) return;
 	if (g_censorPenalty.integer & CNSRPNLTY_TMPMUTE){
 		ent->client->sess.muted = qtrue;
 		ent->client->sess.auto_mute_time = level.time;
     CPx(ent - g_entities, va("print \"^5You've been auto-muted for %d seconds for language.\"",g_censorMuteTime.integer ));
     AP(va("chat \"%s^7 has been auto-muted for %d seconds for language.",  
 		ent->client->pers.netname, g_censorMuteTime.integer ));
 	}
 	if (g_censorPenalty.integer & CNSRPNLTY_KILL) {
 		Force_Suicide(ent);
 		return;
 	}
 	if (g_censorPenalty.integer & CNSRPNLTY_NOGIB){
 		// KILL, don't GIB
 		ent->flags &= ~FL_GODMODE;
 		ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
 		ent->client->ps.persistant[PERS_HWEAPON_USE] = 0; // TTimo - if at MG42
 		player_die(ent, ent, ent , 135, MOD_UNKNOWN);
 	}
 }

 /*
  * G_PlayDead
  */
 void G_PlayDead(gentity_t *ent) 
 {
 
 	if(!ent->client)
 		return;
 	if(!g_playDead.integer)
 		return;
 	if(ent->health < 0)
 		return;
 	if(ent->client->pmext.poisoned)
 		return;
 
 	// tjw: can't playdead with a ticking grenade or a panzer
 	//      that is spinning up.
 	if(ent->client->ps.weaponDelay) 
 		return;
 	
 	// tjw: can't playdead with scoped weapon, or the view doesn't change
 	if(BG_IsScopedWeapon(ent->s.weapon))
 		return;
 
 	// tjw: playdead with heavy weap set causes all kinds of problems
#ifdef __EF__
	if(ent->s.weapon == WP_MORTAR_SET || ent->s.weapon == WP_MOBILE_MG42_SET  || ent->s.weapon == WP_30CAL_SET || ent->s.weapon == WP_PTRS_SET )
#else //!__EF__
 	if(ent->s.weapon == WP_MORTAR_SET || ent->s.weapon == WP_MOBILE_MG42_SET)
#endif //__EF__
 		return;	
 
 	if(ent->client->ps.eFlags & EF_PLAYDEAD) {
 
 		ent->client->ps.stats[STAT_HEALTH] = ent->health;
 		ent->client->ps.pm_type = PM_PLAYDEAD;
 		CP("cp \"SURPRISE!\" 1");
 
 		// forty - ok restore there original weapons they were holding
 		//         before playing dead.
 		ent->s.weapon = ent->client->limboDropWeapon;
 		ent->client->ps.weapon = ent->client->limboDropWeapon;
 	}
 	else {
 		ent->client->ps.pm_type = PM_PLAYDEAD;
 		ent->client->limboDropWeapon = ent->s.weapon;
 
 		// forty - show playdead players holding nothing. 
 		// This also prevents the client side from drawing 
 		// the flamethrower flame while firing and playing dead.
 		ent->s.weapon = WP_NONE; // let them have nothing!
 		ent->client->ps.weapon = WP_NONE;
 	}
 }
 
 void G_PrivateMessage(gentity_t *ent) 
 {
 	int pids[MAX_CLIENTS];
 	char name[MAX_NAME_LENGTH];
 	char netname[MAX_NAME_LENGTH];
 	char cmd[12];
 	char *msg;
 	int pcount;
 	int i;
 	int skipargs = 0;
 	gentity_t *tmpent;
 	qboolean sent = qfalse;
 
 	if(!g_privateMessages.integer && ent) return;
 	Q_SayArgv(0, cmd, sizeof(cmd));
 	if(!Q_stricmp(cmd, "say")) {
 		skipargs = 1;
 		Q_SayArgv(1, cmd, sizeof(cmd));
 	}
 	if(Q_SayArgc() < 3+skipargs) {
 		SP(va("usage: %s [name|slot#] [message]\n", cmd));
 		return;
 	}
 	Q_SayArgv(1+skipargs, name, sizeof(name));
 	msg = Q_SayConcatArgs(2+skipargs);
 	pcount = ClientNumbersFromString(name, pids);
 
 	if(ent)
 		Q_strncpyz(netname, 
 			ent->client->pers.netname,
 			sizeof(name));
 	else
 		Q_strncpyz(netname, "console", sizeof(name));
 
 	for(i=0; i<pcount; i++) {
 		// tjw: allow private message to self for etadmin_mod compat
 		//if(pids[i] == ent-g_entities) continue;
 		tmpent = &g_entities[pids[i]];
 		sent = qtrue;
 		if(COM_BitCheck(
 			tmpent->client->sess.ignoreClients,
 			(ent-g_entities))) {
 
 			SP(va("%s^1 is ignoring you\n",
 				tmpent->client->pers.netname));
  				continue;
  			}
 		CPx(pids[i], va(
 			"chat \"%s^7 -> %s^7: (%d recipients^7): ^3%s^7\"",
 			netname,
 			name,
 			pcount,
 			msg));
 		CPx(pids[i], va("cp \"^3private message from ^7%s^7\"",
 			netname));
 		SP(va("private message sent to %s: ^3%s^7\n",
 			tmpent->client->pers.netname,
 			msg
 			));
 	}
 	if(!sent) {
 		SP("player not found\n");
 	}
 	else if(g_tyranny.integer && g_logOptions.integer & LOGOPTS_PRIVMSG) {
 		G_LogPrintf( "privmsg: %s: %s: %s\n", netname, name, msg );
 	}
 }
 
 void G_PlaySound_Cmd(void) {
 	char cmd[32] = {"playsound"};
 	char sound[MAX_QPATH], name[MAX_NAME_LENGTH];
 	int pids[MAX_CLIENTS];
 	char err[MAX_STRING_CHARS];
 	int index;
 	gentity_t *victim;
 	gentity_t *tent;
 
 	if(trap_Argc() < 2) {
 		G_Printf("usage: playsound [name|slot#] sound\n");
 		return;
 	}
 
 	if(trap_Argc() > 2) {
 		if(!g_tyranny.integer) {
 			G_Printf("playsound: g_tyranny must be enabled to play "
 				"sounds to specific clients\n");
 			return;
 		}
 		trap_Argv(0, cmd, sizeof(cmd));
 		trap_Argv(1, name, sizeof(name));
 		trap_Argv(2, sound, sizeof(sound));
 	}
 	else {
 		trap_Argv(1, sound, sizeof(sound));
 		name[0] = '\0';
 	}
 
 	index = G_SoundIndex(sound);
 
 	if(name[0]) {
 		if(ClientNumbersFromString(name, pids) != 1) {
 			G_MatchOnePlayer(pids, err, sizeof(err));
 			G_Printf("playsound: %s\n", err);
 			return;
 		}
 		victim = &level.gentities[pids[0]];
 		if(!Q_stricmp(cmd, "playsound_env")) {
 			G_AddEvent(victim, EV_GENERAL_SOUND, index);
 		}
 		else {
 			tent = G_TempEntity(victim->r.currentOrigin, 
 			       EV_GLOBAL_CLIENT_SOUND);
 			tent->s.teamNum = pids[0];
 			tent->s.eventParm = index;
 		}
 	}
 	else {
 		G_globalSound(sound);
 	}
 
 }
 
 void G_TeamDamageStats(gentity_t *ent)
 {
 	float teamHitPct;
 	if (!ent->client) return;
 
 	teamHitPct =
 		ent->client->sess.hits ?
 		(ent->client->sess.team_hits / ent->client->sess.hits)*(100):
 		0;
 
 	CPx(ent-g_entities, 
 		va("print \"Team Hits: %.2f Total Hits: %.2f "
 			"Pct: %.2f Limit: %d\n\"",
 		ent->client->sess.team_hits,
 		ent->client->sess.hits,
 		teamHitPct,
 		g_teamDamageRestriction.integer
 		));
 	return;
 }
 // G_SendScore_Add
 // 
 // Add score with clientNum at index i of level.sortedClients[]
 // to the string buf.
 // 
 // returns qtrue if the score was appended to buf, qfalse otherwise.
 qboolean G_SendScore_Add(gentity_t *ent, int i, char *buf, int bufsize) 
 {
 	gclient_t *cl;
 	int ping, playerClass, respawnsLeft;
 	char entry[128];
 	int totalXP = 0;
 	int j;
 
 	entry[0] = '\0';
 
 	cl = &level.clients[level.sortedClients[i]];
  
  				playerClass = 0;
 	// tjw: spectators should be able to see the player's class,
 	//      but apperantly the client won't draw them even if we
 	//      send it.  *clientmod*
 	if(G_smvLocateEntityInMVList(ent, level.sortedClients[i], qfalse) ||
 		cl->sess.sessionTeam == ent->client->sess.sessionTeam ||
 		ent->client->sess.sessionTeam == TEAM_SPECTATOR) { 
 
 		playerClass = cl->sess.playerType;
  			}
  
  			// NERVE - SMF - number of respawns left
  			respawnsLeft = cl->ps.persistant[PERS_RESPAWNS_LEFT];
 	if(g_gametype.integer == GT_WOLF_LMS) {
 		if(g_entities[level.sortedClients[i]].health <= 0) {
  					respawnsLeft = -2;
  				}
  				}
 	else if(respawnsLeft == 0 && 
 		((cl->ps.pm_flags & PMF_LIMBO) || 
 		  (level.intermissiontime && 
 		  g_entities[level.sortedClients[i]].health <= 0))) {
 
 		respawnsLeft = -2;
  			}
#ifdef __BOT__
			if (g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT && !bot_fakePing.integer){
				ping = -2;
			} else
#endif	
  			if ( cl->pers.connected == CON_CONNECTING ) {
  				ping = -1;
 	}
 	else {
 		//unlagged - true ping
 		//ping = cl->ps.ping < 999 
 		//	? cl->ps.ping : 999;
 		ping = cl->pers.realPing < 999 ?
 			cl->pers.realPing : 999;
 		//unlagged - true ping
  			}
  
  			if( g_gametype.integer == GT_WOLF_LMS ) {
 		totalXP = cl->ps.persistant[PERS_SCORE];
 	}
 	else {
 		for(j = 0; j < SK_NUM_SKILLS; j++) {
  					totalXP += cl->sess.skillpoints[j];
  				}
 	}
 
 	Com_sprintf(entry,
 		sizeof(entry),
 		" %i %i %i %i %i %i %i",
 		level.sortedClients[i],
 		totalXP,
 		ping, 
 		(level.time - cl->pers.enterTime) / 60000,
 		g_entities[level.sortedClients[i]].s.powerups,
 		playerClass,
 		respawnsLeft);
  
 	if((strlen(buf) + strlen(entry) + 1) > bufsize) {
 		return qfalse;
  			}
 	Q_strcat(buf, bufsize, entry);
 	return qtrue;
 }
 
  
 /*
 ==================
 G_SendScore
 
 Sends current scoreboard information
 ==================
*/
void G_SendScore( gentity_t *ent ) {
 	int i;
 	int numSorted;
 	int count;
 	// tjw: commands over 1022 will crash the client so they're
 	//      pruned in trap_SendServerCommand()
 	//      1022 -32 for the startbuffer -3 for the clientNum 
 	char		buffer[987];
 	char		startbuffer[32];
 
 	// send the latest information on all clients
 	numSorted = level.numConnectedClients;
 
 	i = 0;
 	count = 0;
 	*buffer = '\0';
 	*startbuffer = '\0';
 	Q_strncpyz(startbuffer, va(
 		"sc0 %i %i",
 		level.teamScores[TEAM_AXIS],
 		level.teamScores[TEAM_ALLIES]),
 		sizeof(startbuffer));
 
 	// tjw: keep adding scores to the sc0 command until we fill 
 	//      up the buffer.  Any scores that are left will be 
 	//      added on to the sc1 command.
 	for(; i < numSorted; i++) {
 		if(g_entities[level.sortedClients[i]].r.svFlags & SVF_POW) {
 			continue;
 		}
 		// tjw: the old version of SendScore() did this.  I removed it
 		//      originally because it seemed like an unneccessary hack.
 		//      perhaps it is necessary for compat with CG_Argv()?
 		if(count == 33) {
  				break;
  			}
 		if(!G_SendScore_Add(ent, i, buffer, sizeof(buffer))) {
  				break;
  			}
 		count++;
  		}
 	trap_SendServerCommand(ent-g_entities, va(
 		"%s %i%s", startbuffer, count, buffer));
  
 	if(i == numSorted)
 		return;
 	
 	count = 0;
 	*buffer = '\0';
 	*startbuffer = '\0';
 	Q_strncpyz(startbuffer, "sc1", sizeof(startbuffer));
 	for(; i < numSorted; i++) {
 		if(g_entities[level.sortedClients[i]].r.svFlags & SVF_POW) {
 			continue;
  		}
 		if(!G_SendScore_Add(ent, i, buffer, sizeof(buffer))) {
 			G_Printf("ERROR: G_SendScore() buffer overflow\n");
 			break;
  	}
 		count++;
 	}
 	if(!count) {
 		return;
 	}
 	trap_SendServerCommand(ent-g_entities, va(
 		"%s %i%s", startbuffer, count, buffer));
 
  }
/*
==================
Cmd_Score_f

Request current scoreboard information
==================
*/
void Cmd_Score_f( gentity_t *ent ) {
	ent->client->wantsscore = qtrue;
//	G_SendScore( ent );
}

/*
==================
CheatsOk
==================
*/
qboolean	CheatsOk( gentity_t *ent ) {
	if ( !g_cheats.integer ) {
		trap_SendServerCommand( ent-g_entities, va("print \"Cheats are not enabled on this server.\n\""));
		return qfalse;
	}
	if ( ent->health <= 0 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"You must be alive to use this command.\n\""));
		return qfalse;
	}
	return qtrue;
}

// josh: tjw moved the declaration to q_shared.h, q_shared.c needs this
// so I moved it to q_shared.c
///*
//==================
//ConcatArgs
//==================
//*/
//char	*ConcatArgs( int start ) {
//	int		i, c, tlen;
//	static char	line[MAX_STRING_CHARS];
//	int		len;
//	char	arg[MAX_STRING_CHARS];
//
//	len = 0;
//	c = trap_Argc();
//	for ( i = start ; i < c ; i++ ) {
//		trap_Argv( i, arg, sizeof( arg ) );
//		tlen = strlen( arg );
//		if ( len + tlen >= MAX_STRING_CHARS - 1 ) {
//			break;
//		}
//		memcpy( line + len, arg, tlen );
//		len += tlen;
//		if ( i != c - 1 ) {
//			line[len] = ' ';
//			len++;
//		}
//	}
//
//	line[len] = 0;
//
//	return line;
//}

/*
==================
DecolorString

Remove color characters
==================
*/
void DecolorString( char *in, char *out)
{
	while(*in) {
		if(*in == 27 || *in == '^') {
			in++;		// skip color code
			if(*in) in++;
			continue;
		}
		*out++ = *in++;
	}
	*out = 0;
}

/*
==================

SanitizeString

Remove case and control characters
==================
*/
void SanitizeString( char *in, char *out, qboolean fToLower )
{
	while(*in) {
		if(*in == 27 || *in == '^') {
			in++;		// skip color code
			if(*in) in++;
			continue;
		}

		if(*in < 32) {
			in++;
			continue;
		}

		*out++ = (fToLower) ? tolower(*in++) : *in++;
	}

	*out = 0;
}

qboolean G_MatchOnePlayer(int *plist, char *err, int len) 
{
	gclient_t *cl;
	int *p;
	char line[MAX_NAME_LENGTH+10];

	err[0] = '\0';
	line[0] = '\0';
	if(plist[0] == -1) {
		Q_strcat(err, len, 
			"no connected player by that name or slot #");
		return qfalse;
	}
	if(plist[1] != -1) {
		Q_strcat(err, len, "more than one player name matches. "
			"be more specific or use the slot #:\n");
		for(p = plist;*p != -1; p++) {
			cl = &level.clients[*p];
			if(cl->pers.connected == CON_CONNECTED) {
				sprintf(line, "%2i - %s^7\n",
					*p,	
					cl->pers.netname);
				if(strlen(err)+strlen(line) > len)
					break;
				Q_strcat(err, len, line);
			}
		}
		return qfalse;
	}
	return qtrue;
}

/*
==================
ClientNumbersFromString

Sets plist to an array of integers that represent client numbers that have 
names that are a partial match for s. List is terminated by a -1.

Returns number of matching clientids.
==================
*/
int ClientNumbersFromString( char *s, int *plist) {
	gclient_t *p;
	int i, found = 0;
	char s2[MAX_STRING_CHARS];
	char n2[MAX_STRING_CHARS];
	char *m;
	qboolean is_slot = qtrue;

	*plist = -1;

	// if a number is provided, it might be a slot # 
	for(i=0; i<(int)strlen(s); i++) { 
		if(s[i] < '0' || s[i] > '9') {
			is_slot = qfalse;
			break;
		}
	}
	if(is_slot) {
		i = atoi(s);
		if(i >= 0 && i < level.maxclients) {
			p = &level.clients[i];
			if(p->pers.connected == CON_CONNECTED ||
				p->pers.connected == CON_CONNECTING) {

				*plist++ = i;
				*plist = -1;
				return 1;
			}
		}
	}

	// now look for name matches
	SanitizeString(s, s2, qtrue);
	if(strlen(s2) < 1) return 0;
	for(i=0; i < level.maxclients; i++) {
		p = &level.clients[i];
		if(p->pers.connected != CON_CONNECTED &&
			p->pers.connected != CON_CONNECTING) {
			
			continue;
		}
		SanitizeString(p->pers.netname, n2, qtrue);
		m = strstr(n2, s2);
		if(m != NULL) {
			*plist++ = i;
			found++;
		}
	}
	*plist = -1;
	return found;
}
	
/*
==================
ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
int ClientNumberFromString( gentity_t *to, char *s ) {
	gclient_t	*cl;
	int			idnum;
	char		s2[MAX_STRING_CHARS];
	char		n2[MAX_STRING_CHARS];
	qboolean	fIsNumber = qtrue;

	// See if its a number or string
	// CHRUKER: b068 - Added the (int) type casting
	for(idnum=0; idnum<(int)strlen(s) && s[idnum] != 0; idnum++) {
		if(s[idnum] < '0' || s[idnum] > '9') {
			fIsNumber = qfalse;
			break;
		}
	}

	// check for a name match
	SanitizeString(s, s2, qtrue);
	for(idnum=0, cl=level.clients; idnum<level.maxclients; idnum++, cl++) {
		if(cl->pers.connected != CON_CONNECTED) continue;

		SanitizeString(cl->pers.netname, n2, qtrue);
		if(!strcmp( n2, s2)) return(idnum);
	}

	// numeric values are just slot numbers
	if(fIsNumber) {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			CPx( to-g_entities, va("print \"Bad client slot: [lof]%i\n\"", idnum));
			return -1;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected != CON_CONNECTED ) {
			CPx( to-g_entities, va("print \"Client[lof] %i [lon]is not active\n\"", idnum));
			return -1;
		}
		return(idnum);
	}

	CPx(to-g_entities, va("print \"User [lof]%s [lon]is not on the server\n\"", s));
	return(-1);
}

/*
=================
Cmd_ListBotGoals_f

=================
*/

void Cmd_ListBotGoals_f( gentity_t* ent ) {
	int i;
	team_t t;

	if( !CheatsOk( ent ) ) {
		return;
	}
	
	for( t = TEAM_AXIS; t <= TEAM_ALLIES; t++ ) {
		gentity_t* list = g_entities, *targ;

		G_Printf( "\n%s bot goals\n=====================\n", (t == TEAM_AXIS ? "Axis" : "Allies"));
		

		for( i = 0; i < level.num_entities; i++, list++ ) {
			if(!list->inuse) {
				continue;
			}

			if(!(list->aiInactive & (1 << t))) {
				G_Printf( "%s (%s)", (list->scriptName ? list->scriptName :( list->targetname ? list->targetname : "NONE" )), list->classname );
				if (list->target_ent) {
					targ = list->target_ent;
					G_Printf( " -> " );
					G_Printf( "%s (%s)", (targ->scriptName ? targ->scriptName :( targ->targetname ? targ->targetname : "NONE" )), targ->classname );
				}
				G_Printf( "\n" );
			}
		}
	}
}

/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void Cmd_Give_f (gentity_t *ent)
{
	char		*name, *amt;
//	gitem_t		*it;
	int			i;
	qboolean	give_all;
//	gentity_t		*it_ent;
//	trace_t		trace;
	int			amount;
	qboolean	hasAmount = qfalse;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	//----(SA)	check for an amount (like "give health 30")
	amt = ConcatArgs(2);
	if( *amt )
		hasAmount = qtrue;
	amount = atoi(amt);
	//----(SA)	end

	name = ConcatArgs( 1 );

	if (Q_stricmp(name, "all") == 0)
		give_all = qtrue;
	else
		give_all = qfalse;

	if( Q_stricmpn( name, "skill", 5 ) == 0 ) {
		if( hasAmount ) {
			if( amount >= 0 && amount < SK_NUM_SKILLS ) {
				G_AddSkillPoints( ent, amount, 20 );
				G_DebugAddSkillPoints( ent, amount, 20, "give skill" ); 
			}
		} else {
			// bumps all skills with 1 level
			for( i = 0; i < SK_NUM_SKILLS; i++ ) {
				G_AddSkillPoints( ent, i, 20 );
				G_DebugAddSkillPoints( ent, i, 20, "give skill" ); 
			}
		}
		return;
	}

	if ( Q_stricmpn( name, "medal", 5) == 0) {
        for( i = 0; i < SK_NUM_SKILLS; i++ ) {
			if( !ent->client->sess.medals[i] )
				ent->client->sess.medals[i] = 1;
		}
		ClientUserinfoChanged( ent-g_entities );
		return;
	}

	if (give_all || Q_stricmpn( name, "health", 6) == 0)
	{
		//----(SA)	modified
		if(amount)
			ent->health += amount;
		else
			ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
		if (!give_all)
			return;
	}

	/*if ( Q_stricmpn( name, "damage", 6) == 0)
	{
		if(amount) {
			name = ConcatArgs( 3 );

			if( *name ) {
				int client = ClientNumberFromString( ent, name ); 
				if( client >= 0 ) {
					G_Damage( &g_entities[client], ent, ent, NULL, NULL, amount, DAMAGE_NO_PROTECTION, MOD_UNKNOWN );
				}
			} else {
				G_Damage( ent, ent, ent, NULL, NULL, amount, DAMAGE_NO_PROTECTION, MOD_UNKNOWN );
			}
		}

        return;
	}*/

	if (give_all || Q_stricmp(name, "weapons") == 0)
	{
		for(i=0;i<WP_NUM_WEAPONS;i++) {
			if ( BG_WeaponInWolfMP(i) )
				COM_BitSet( ent->client->ps.weapons, i );
		}

		if (!give_all)
			return;
	}

	if (give_all || Q_stricmpn(name, "ammo", 4) == 0)
	{
		if(amount) {
			if(ent->client->ps.weapon
				&& ent->client->ps.weapon != WP_SATCHEL && ent->client->ps.weapon != WP_SATCHEL_DET
				)
				Add_Ammo(ent, ent->client->ps.weapon, amount, qtrue);
		} else {
			for ( i = 1 ; i < WP_NUM_WEAPONS ; i++ ) {
				if( COM_BitCheck( ent->client->ps.weapons, i ) && i != WP_SATCHEL && i != WP_SATCHEL_DET)
					Add_Ammo(ent, i, 9999, qtrue);
			}
		}

		if (!give_all)
			return;
	}

	//	"give allammo <n>" allows you to give a specific amount of ammo to /all/ weapons while
	//	allowing "give ammo <n>" to only give to the selected weap.
	if (Q_stricmpn(name, "allammo", 7) == 0 && amount)
	{
		for ( i = 1 ; i < WP_NUM_WEAPONS; i++ )
			Add_Ammo(ent, i, amount, qtrue);

		if (!give_all)
			return;
	}

	//---- (SA) Wolf keys
	if (give_all || Q_stricmp(name, "keys") == 0)
	{
		ent->client->ps.stats[STAT_KEYS] = (1 << KEY_NUM_KEYS) - 2;
		if (!give_all)
			return;
	}
	//---- (SA) end

	// spawn a specific item right on the player
	/*if ( !give_all ) {
		it = BG_FindItem (name);
		if (!it) {
			return;
		}

		it_ent = G_Spawn();
		VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
		it_ent->classname = it->classname;
		G_SpawnItem (it_ent, it);
		FinishSpawningItem(it_ent );
		memset( &trace, 0, sizeof( trace ) );
		it_ent->active = qtrue;
		Touch_Item (it_ent, ent, &trace);
		it_ent->active = qfalse;
		if (it_ent->inuse) {
			G_FreeEntity( it_ent );
		}
	}*/
}


/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f (gentity_t *ent)
{
	char	*msg;
	char	*name;
	qboolean godAll = qfalse;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	name = ConcatArgs( 1 );

	// are we supposed to make all our teammates gods too?
	if (Q_stricmp(name, "all") == 0)
		godAll = qtrue;

	// can only use this cheat in single player
	if (godAll && g_gametype.integer == GT_SINGLE_PLAYER)
	{
		int j;
		qboolean settingFlag = qtrue;
		gentity_t *other;

		// are we turning it on or off?
		if (ent->flags & FL_GODMODE)
			settingFlag = qfalse;

		// loop through all players
		for (j = 0; j < level.maxclients; j++)
		{
			other = &g_entities[j];
			// if they're on the same team
			if (OnSameTeam(other, ent))
			{
				// set or clear the flag
				if (settingFlag)
					other->flags |= FL_GODMODE;
				else
					other->flags &= ~FL_GODMODE;
			}
		}
		if (settingFlag)
			msg = "godmode all ON\n";
		else
			msg = "godmode all OFF\n";
	}
	else
	{
		if(!Q_stricmp( name, "on" ) || atoi( name ) ) {
			ent->flags |= FL_GODMODE;
		} else if ( !Q_stricmp( name, "off" ) || !Q_stricmp( name, "0" )) {
			ent->flags &= ~FL_GODMODE;
		} else {
			ent->flags ^= FL_GODMODE;
		}
		if (!(ent->flags & FL_GODMODE) )
			msg = "godmode OFF\n";
		else
			msg = "godmode ON\n";
	}

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}

/*
==================
Cmd_Nofatigue_f

Sets client to nofatigue

argv(0) nofatigue
==================
*/

void Cmd_Nofatigue_f (gentity_t *ent)
{
	char	*msg;

	char	*name = ConcatArgs( 1 );

	if ( !CheatsOk( ent ) ) {
		return;
	}

	if(!Q_stricmp( name, "on" ) || atoi( name ) ) {
		ent->flags |= FL_NOFATIGUE;
	} else if ( !Q_stricmp( name, "off" ) || !Q_stricmp( name, "0" )) {
		ent->flags &= ~FL_NOFATIGUE;
	} else {
		ent->flags ^= FL_NOFATIGUE;
	}

	if (!(ent->flags & FL_NOFATIGUE) )
		msg = "nofatigue OFF\n";
	else
		msg = "nofatigue ON\n";

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}

/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f( gentity_t *ent ) {
	char	*msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if (!(ent->flags & FL_NOTARGET) )
		msg = "notarget OFF\n";
	else
		msg = "notarget ON\n";

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f( gentity_t *ent ) {
	char	*msg;

	char	*name = ConcatArgs( 1 );

	if ( !CheatsOk( ent ) ) {
		return;
	}

	if(!Q_stricmp( name, "on" ) || atoi( name ) ) {
		ent->client->noclip = qtrue;
	} else if ( !Q_stricmp( name, "off" ) || !Q_stricmp( name, "0" )) {
		ent->client->noclip = qfalse;
	} else {
		ent->client->noclip = !ent->client->noclip;
	}

	if ( ent->client->noclip ) {
		msg = "noclip ON\n";
	} else {
		msg = "noclip OFF\n";
	}

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}
/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f( gentity_t *ent )
{
	gentity_t *attacker;

	if(ent->health <= 0) {
		SP("You must be alive to use /kill\n");
		return;
	}

	if( g_slashKill.integer & SLASHKILL_NOKILL ) {
		SP("/kill is disabled on this server\n");
		return;
	}
	if((g_slashKill.integer & SLASHKILL_NOPOISON) && 
		ent->client->pmext.poisoned) {
		SP("/kill is disabled when you are poisoned\n");
		return;
	}
	if(ent->client->sess.sessionTeam == TEAM_SPECTATOR ||
	  (ent->client->ps.pm_flags & PMF_LIMBO) ||
	  ent->health <= 0 || level.match_pause != PAUSE_NONE) {
		return;
	}
	ent->client->pers.slashKill = qtrue;

#ifdef SAVEGAME_SUPPORT
	if( g_gametype.integer == GT_SINGLE_PLAYER && g_reloading.integer )
		return;
#endif // SAVEGAME_SUPPORT

	ent->flags &= ~FL_GODMODE;
	ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
	// TTimo - if using /kill while at MG42
	ent->client->ps.persistant[PERS_HWEAPON_USE] = 0;

	attacker = &level.gentities[ent->client->lasthurt_client];

	// tjw: put an end to /kill binding stat whores.
	if(g_fear.integer && attacker && attacker->client &&
		(level.time - ent->client->lasthurt_time) < g_fear.integer &&
		attacker->client->sess.sessionTeam !=
			ent->client->sess.sessionTeam &&
		attacker->health > 0) {
		// tjw: 150 added to make sure the player is gibbed since
		//      that was the intent.
		player_die(ent, attacker, attacker,
			(ent->health + 150), MOD_FEAR);
	}
	else {
		player_die(ent, ent, ent,
			(g_gamestate.integer == GS_PLAYING) ? 100000 : 135,
			MOD_SUICIDE);
	}
}


void BotRecordTeamChange( int client );

void G_TeamDataForString( const char* teamstr, int clientNum, team_t* team, spectatorState_t* sState, int* specClient ) {
	*sState = SPECTATOR_NOT;
	if( !Q_stricmp( teamstr, "follow1" ) ) {
		*team =		TEAM_SPECTATOR;
		*sState =	SPECTATOR_FOLLOW;
		if( specClient ) {
			*specClient = -1;
		}
	} else if( !Q_stricmp( teamstr, "follow2" ) ) {
		*team =		TEAM_SPECTATOR;
		*sState =	SPECTATOR_FOLLOW;
		if( specClient ) {
			*specClient = -2;
		}
	} else if( !Q_stricmp( teamstr, "spectator" ) || !Q_stricmp( teamstr, "s" ) ) {
		*team =		TEAM_SPECTATOR;
		*sState =	SPECTATOR_FREE;
	} else if ( !Q_stricmp( teamstr, "red" ) || !Q_stricmp( teamstr, "r" ) || !Q_stricmp( teamstr, "axis" ) ) {
		*team =		TEAM_AXIS;
	} else if ( !Q_stricmp( teamstr, "blue" ) || !Q_stricmp( teamstr, "b" ) || !Q_stricmp( teamstr, "allies" ) ) {
		*team =		TEAM_ALLIES;
	} else {
		*team = PickTeam( clientNum );
		if(!G_teamJoinCheck( *team, &g_entities[clientNum] )) {
			*team = ((TEAM_AXIS | TEAM_ALLIES) & ~*team);
		}
	}
}


void G_DropItems(gentity_t *self) 
{
	gitem_t *item= NULL;

	// drop flag regardless
	if (self->client->ps.powerups[PW_REDFLAG]) {
		item = BG_FindItem("Red Flag");
		if (!item)
			item = BG_FindItem("Objective");

		self->client->ps.powerups[PW_REDFLAG] = 0;
	}
	if (self->client->ps.powerups[PW_BLUEFLAG]) {
		item = BG_FindItem("Blue Flag");
		if (!item)
			item = BG_FindItem("Objective");

		self->client->ps.powerups[PW_BLUEFLAG] = 0;
	}

	if (item) {
		vec3_t launchvel = { 0, 0, 0 };
		vec3_t forward;
		vec3_t origin;
		vec3_t angles;
		gentity_t *flag;

		VectorCopy(self->client->ps.origin, origin);
		// tjw: if the player hasn't died, then assume he's 
		//      throwing objective per g_dropObj
		if(self->health > 0) {
			VectorCopy(self->client->ps.viewangles, angles);
			if(angles[PITCH] > 0)
				angles[PITCH] = 0;
			AngleVectors(angles, forward, NULL, NULL);
			VectorMA(self->client->ps.velocity, 
				96, forward, launchvel);
			VectorMA(origin, 36, forward, origin);
			origin[2] += self->client->ps.viewheight;
		}

		flag = LaunchItem(item, origin, launchvel, self->s.number);

		flag->s.modelindex2 = self->s.otherEntityNum2;// JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
		flag->message = self->message;	// DHM - Nerve :: also restore item name
		// Clear out player's temp copies
		self->s.otherEntityNum2 = 0;
		self->message = NULL;
	}
}


qboolean G_CheckTeamBalance(gentity_t *ent, team_t team) 
{
	int counts[TEAM_NUM_TEAMS];

	if(!g_teamForceBalance_playerrating.integer) {
		if(g_gametype.integer == GT_WOLF_LMS &&
			!g_lms_teamForceBalance.integer) {

			return qtrue;
		}
		else if(!g_teamForceBalance.integer) {
			return qtrue;
		}
	}

	counts[TEAM_ALLIES] = TeamCount( ent-g_entities, TEAM_ALLIES );
	counts[TEAM_AXIS] = TeamCount( ent-g_entities, TEAM_AXIS );

	// g_teamForceBalance_playerrating must override g_teamForceBalance to
	// prevent both teams getting locked, and to show g_balancedteams
	//
	// team balancing by player rating
	if(g_teamForceBalance_playerrating.integer ) {
		// g_teamForceBalance_playerrating is the highest probability
		// a team should have of winning before they're locked
		if (counts[TEAM_ALLIES] + counts[TEAM_AXIS] > 
				g_playerRating_minplayers.integer && 
				100*G_GetWinProbability(team, ent, qfalse) >
					g_teamForceBalance_playerrating.integer) {
			if ( team == TEAM_AXIS ) {
				CP("cp \"The Axis team is too good, join the Allies.\n\"");
				return qfalse; // ignore the request
			} else {
				CP("cp \"The Allies team is too good, join the Axis.\n\"");
				return qfalse; // ignore the request
			}
		}
	} else { // team balancing by numbers
		// We allow a spread of one
		if ( team == TEAM_AXIS && counts[TEAM_AXIS] - counts[TEAM_ALLIES] >= 1 ) {
			CP("cp \"The Axis has too many players.\n\"");
			return qfalse; // ignore the request
		}
		if ( team == TEAM_ALLIES && counts[TEAM_ALLIES] - counts[TEAM_AXIS] >= 1 ) {
			CP("cp \"The Allies have too many players.\n\"");
			return qfalse; // ignore the request
		}
	}
	return qtrue;
}

void G_LeavePlayerTank( gentity_t* ent, qboolean position ); // below...

/*
=================
SetTeam
=================
*/
qboolean SetTeam( gentity_t *ent, char *s, qboolean force, weapon_t w1, weapon_t w2, qboolean setweapons ) {
	team_t				team, oldTeam;
	gclient_t			*client;
	int					clientNum;
	spectatorState_t	specState;
	int					specClient;
	int					respawnsLeft;
	int i;
	gclient_t *cl;

	// if the team changing player is a shrubbot admin with the
	// '5' flag, they can switch teams regardless of balance
	if(G_shrubbot_permission(ent, SBF_FORCETEAMCHANGE)) 
		force = qtrue;

	//
	// see what change is requested
	//
	client = ent->client;

	clientNum = client - level.clients;
	specClient = 0;

	if (client->ps.eFlags & EF_VEHICLE)
	{
		trap_SendServerCommand( clientNum, "cp \"You cannot switch teams until you leave your vehicle!\n\"" );
		return qfalse;
	}
	
	// ydnar: preserve respawn count
	respawnsLeft = client->ps.persistant[ PERS_RESPAWNS_LEFT ];
	
	G_TeamDataForString( s, client - level.clients, &team, &specState, &specClient );

	if( team != TEAM_SPECTATOR ) {
		// Ensure the player can join
		if(!G_teamJoinCheck(team, ent)) {
			// Leave them where they were before the command was issued
			return(qfalse);
		}

		if(g_noTeamSwitching.integer && (team != ent->client->sess.sessionTeam && ent->client->sess.sessionTeam != TEAM_SPECTATOR ) && g_gamestate.integer == GS_PLAYING && !force ) {
			trap_SendServerCommand( clientNum, "cp \"You cannot switch during a match, please wait until the round ends.\n\"" );
			return qfalse;	// ignore the request
		}
		if(!G_CheckTeamBalance(ent, team) && !force)
			return qfalse;
	}

	if ( g_maxGameClients.integer > 0 && level.numNonSpectatorClients >= g_maxGameClients.integer ) {
		team = TEAM_SPECTATOR;
	}

	//
	// decide if we will allow the change
	//
	oldTeam = client->sess.sessionTeam;
	if ( team == oldTeam && team != TEAM_SPECTATOR ) {
		return qfalse;
	}

	// NERVE - SMF - prevent players from switching to regain deployments
	if( g_gametype.integer != GT_WOLF_LMS ) {
		if( ( g_maxlives.integer > 0 || 
			( g_alliedmaxlives.integer > 0 && ent->client->sess.sessionTeam == TEAM_ALLIES ) || 
			( g_axismaxlives.integer > 0 && ent->client->sess.sessionTeam == TEAM_AXIS ) ) 
			
			&& ent->client->ps.persistant[PERS_RESPAWNS_LEFT] == 0 && oldTeam != TEAM_SPECTATOR ) {
			CP("cp \"You can't switch teams because you are out of lives.\n\" 3");
			return qfalse;	// ignore the request
		}
	}

	// DHM - Nerve :: Force players to wait 30 seconds before they can join a new team.
	// OSP - changed to 5 seconds
	// Gordon: disabling if in dev mode: cuz it sucks
	// Gordon: bleh, half of these variables don't mean what they used to, so this doesn't work
/*	if ( !g_cheats.integer ) {
		 if ( team != oldTeam && level.warmupTime == 0 && !client->pers.initialSpawn && ( (level.time - client->pers.connectTime) > 10000 ) && ( (level.time - client->pers.enterTime) < 5000 ) && !force ) {
			CP(va("cp \"^3You must wait %i seconds before joining ^3a new team.\n\" 3", (int)(5 - ((level.time - client->pers.enterTime)/1000))));
			return qfalse;
		}
	}*/
	// dhm
  
	//
	// execute the team change
	//


	// DHM - Nerve
	// OSP
	if(team != TEAM_SPECTATOR) {
		client->pers.initialSpawn = qfalse;
		// no MV in-game
		if(client->pers.mvCount > 0) {
			G_smvRemoveInvalidClients(ent, TEAM_AXIS);
			G_smvRemoveInvalidClients(ent, TEAM_ALLIES);
		}
	}

	// he starts at 'base'
	// RF, in single player, bots always use regular spawn points
	if (!((g_gametype.integer == GT_SINGLE_PLAYER || g_gametype.integer == GT_COOP || g_gametype.integer == GT_NEWCOOP) && (ent->r.svFlags & SVF_BOT))) {
		client->pers.teamState.state = TEAM_BEGIN;
	}
	

	if(oldTeam != TEAM_SPECTATOR && 
		!(ent->client->ps.pm_flags & PMF_LIMBO) ) {
		
			ent->flags &= ~FL_GODMODE;
		// tjw: if they're not dead, don't kill
		if(g_teamChangeKills.integer || ent->health <= 0) {
			ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
			player_die (ent, ent, ent, 100000, MOD_SWITCHTEAM);
		}
		else {
			ent->client->ps.stats[STAT_HEALTH] = ent->health;
			G_DropItems(ent);
		}
	}

	// they go to the end of the line for tournements
	if ( team == TEAM_SPECTATOR ) {
		client->sess.spectatorTime = level.time;
		if(!client->sess.referee) client->pers.invite = 0;
		if(team != oldTeam) G_smvAllRemoveSingleClient(ent - g_entities);
	}

	G_LeaveTank( ent, qfalse );
	G_RemoveClientFromFireteams( clientNum, qtrue, qfalse );
	if( g_landminetimeout.integer ) {
		G_ExplodeMines( ent );
	}
	G_FadeItems(ent, MOD_SATCHEL);

	// remove ourself from teamlists
	{
		int i;
		mapEntityData_t	*mEnt;
		mapEntityData_Team_t *teamList;

		for( i = 0; i < 2; i++ ) {
			teamList = &mapEntityData[i];

			if((mEnt = G_FindMapEntityData(&mapEntityData[0], ent-g_entities)) != NULL) {
				G_FreeMapEntityData( teamList, mEnt );
			}

			mEnt = G_FindMapEntityDataSingleClient( teamList, NULL, ent->s.number, -1 );
			
			while( mEnt ) {
				mapEntityData_t	*mEntFree = mEnt;

				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, ent->s.number, -1 );

				G_FreeMapEntityData( teamList, mEntFree );
			}
		}
	}

	// tjw: if someone was following, make them follow the next 
	//      player on the team.
	for(i=0; i<level.numConnectedClients; i++) {
		cl = &level.clients[level.sortedClients[i]];
		if((cl->ps.pm_flags & PMF_LIMBO) &&
			cl->sess.spectatorState == SPECTATOR_FOLLOW &&
			cl->sess.spectatorClient == (ent - g_entities)) {
			
			//Cmd_FollowCycle_f(&g_entities[level.sortedClients[i]], 1);
			cl->sess.spectatorClient = level.sortedClients[i];
			cl->sess.spectatorState = SPECTATOR_FREE;
		}
	}

	client->sess.spec_team = 0;
	client->sess.sessionTeam = team;
	client->sess.spectatorState = specState;
	client->sess.spectatorClient = specClient;
	client->pers.ready = qfalse;

	// (l)users will spam spec messages... honest!
	if(team != oldTeam) {
		gentity_t* tent = G_PopupMessage( PM_TEAM );
		tent->s.effect2Time = team;
		tent->s.effect3Time = clientNum;
		tent->s.density = 0;

#ifdef __VEHICLES__
		if (ent->client->ps.eFlags & EF_VEHICLE)
		{// UQ1: Remove them from the tank first!
			G_LeavePlayerTank( ent, qfalse );
		}
#endif //__VEHICLES__
	}

	if( setweapons ) {
		G_SetClientWeapons( ent, w1, w2, qfalse );
	}

	// get and distribute relevent paramters
	G_UpdateCharacter( client );			// FIXME : doesn't ClientBegin take care of this already?

//#ifdef __BOT__
//	if (!(ent->r.svFlags & SVF_BOT))
//#endif //__BOT__
	{
		ClientUserinfoChanged( clientNum );
		ClientBegin( clientNum );
	}
	
	// ydnar: restore old respawn count (players cannot jump from team to team to regain lives)
	if(respawnsLeft >= 0 && oldTeam != TEAM_SPECTATOR) {
		client->ps.persistant[ PERS_RESPAWNS_LEFT ] = respawnsLeft;
	}

	G_verifyMatchState(oldTeam);
	BotRecordTeamChange( clientNum );

	/*
	// Reset stats when changing teams
	if(team != oldTeam) {
		G_deleteStats(clientNum);
	}
	*/

	G_UpdateSpawnCounts();

	if( g_gamestate.integer == GS_PLAYING && ( client->sess.sessionTeam == TEAM_AXIS || client->sess.sessionTeam == TEAM_ALLIES ) ) {
		if(g_gametype.integer == GT_WOLF_LMS && level.numTeamClients[0] > 0 && level.numTeamClients[1] > 0) {
			trap_SendServerCommand( clientNum, "cp \"Will spawn next round, please wait.\n\"" );
			limbo(ent, qfalse, qfalse);
			return(qfalse);
		} else {
			int i;
			int x = client->sess.sessionTeam - TEAM_AXIS;

			for( i = 0; i < MAX_COMMANDER_TEAM_SOUNDS; i++ ) {
				if( level.commanderSounds[ x ][ i ].index ) {
					gentity_t* tent = G_TempEntity( client->ps.origin, EV_GLOBAL_CLIENT_SOUND ); 
					tent->s.eventParm = level.commanderSounds[ x ][ i ].index - 1;
					tent->s.teamNum = clientNum;
				}
			}
		}
	}

	ent->client->pers.autofireteamCreateEndTime = 0;
	ent->client->pers.autofireteamJoinEndTime = 0;

	if( client->sess.sessionTeam == TEAM_AXIS || client->sess.sessionTeam == TEAM_ALLIES ) {
		if( g_autoFireteams.integer ) {
			fireteamData_t* ft = G_FindFreePublicFireteam( client->sess.sessionTeam );

			if( ft ) {
				trap_SendServerCommand( ent-g_entities, "aftj -1" );
				ent->client->pers.autofireteamJoinEndTime = level.time + 20500;

//				G_AddClientToFireteam( ent-g_entities, ft->joinOrder[0] );
			} else {
				trap_SendServerCommand( ent-g_entities, "aftc -1" );
				ent->client->pers.autofireteamCreateEndTime = level.time + 20500;
			}
		}
	}
	ent->client->pers.lastTeamChangeTime = level.time;
	return qtrue;
}

/*
=================
StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void StopFollowing( gentity_t *ent ) {
	// ATVI Wolfenstein Misc #474
	// divert behaviour if TEAM_SPECTATOR, moved the code from SpectatorThink to put back into free fly correctly
	// (I am not sure this can be called in non-TEAM_SPECTATOR situation, better be safe)
	if( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		// drop to free floating, somewhere above the current position (that's the client you were following)
		vec3_t pos, angle;
		gclient_t	*client = ent->client;
		VectorCopy(client->ps.origin, pos); 
//		pos[2] += 16; // Gordon: removing for now
		VectorCopy(client->ps.viewangles, angle);
		// Need this as it gets spec mode reset properly
		SetTeam( ent, "s", qtrue, -1, -1, qfalse );
		VectorCopy(pos, client->ps.origin);
		SetClientViewAngle(ent, angle);		
		ent->client->sess.spectatorState = SPECTATOR_FREE;
	} else {
		// legacy code, FIXME: useless?
		// Gordon: no this is for limbo i'd guess
		ent->client->sess.spectatorState = SPECTATOR_FREE;
		ent->client->ps.clientNum = ent - g_entities;
	}
}

int G_NumPlayersWithWeapon( weapon_t weap, team_t team ) {
	int i, j, cnt = 0;

	for( i = 0; i < level.numConnectedClients; i++ ) {
		j = level.sortedClients[i];

		if( level.clients[j].sess.playerType != PC_SOLDIER ) {
			continue;
		}

		if( level.clients[j].sess.sessionTeam != team ) {
			continue;
		}

		if( level.clients[j].sess.latchPlayerWeapon != weap && level.clients[j].sess.playerWeapon != weap ) {
			continue;
		}

		cnt++;
	}

	return cnt;
}

int G_NumPlayersOnTeam( team_t team ) {
	int i, j, cnt = 0;

	for( i = 0; i < level.numConnectedClients; i++ ) {
		j = level.sortedClients[i];

		if( level.clients[j].sess.sessionTeam != team ) {
			continue;
		}

		cnt++;
	}

	return cnt;
}

qboolean G_IsHeavyWeapon( weapon_t weap ) {
	int i;

	for( i = 0; i < NUM_HEAVY_WEAPONS; i++ ) {
		if( bg_heavyWeapons[i] == weap ) {
			return qtrue;
		}
	}

	return qfalse;
}

int G_TeamCount( gentity_t* ent, team_t team, weapon_t weap) {
	int i, j, cnt;

	if( weap == -1 ) { // we aint checking for a weapon, so always include ourselves
		cnt = 1;
	} else { // we ARE checking for a weapon, so ignore ourselves
		cnt = 0;
	}

	for( i = 0; i < level.numConnectedClients; i++ ) {
		j = level.sortedClients[i];
		
		if( j == ent-g_entities ) {
			continue;
		}

		if( level.clients[j].sess.sessionTeam != team ) {
			continue;
		}

		if( weap != -1 ) {
			if( level.clients[j].sess.playerWeapon != weap && level.clients[j].sess.latchPlayerWeapon != weap ) {
				continue;
			}
		}

		cnt++;
	}

	return cnt;
}

qboolean G_IsWeaponDisabled(
	gentity_t* ent,
	weapon_t weapon,
	team_t team,
	qboolean quiet)
{
	int count, wcount;

	// tjw: specs can have any weapon they want
	if(team == TEAM_SPECTATOR) {
		return qfalse;
	}

	count =		G_TeamCount( ent, team, -1 );
	wcount =	G_TeamCount( ent, team, weapon );

	if(G_IsHeavyWeapon(weapon)) {
	if( wcount >= ceil( count * g_heavyWeaponRestriction.integer * 0.01f ) ) {
		return qtrue;
	}
	}

	switch(weapon) {
	case WP_PANZERFAUST:
		if(team_maxPanzers.integer == -1) return qfalse;
		if(wcount >= team_maxPanzers.integer) {
			if(!quiet)
				CP("cp \"^1PANZERFAUST not available^7\" 1");
			return qtrue;
		}
		break;
	case WP_MOBILE_MG42:
		if(team_maxMG42s.integer == -1) return qfalse;
		if(wcount >= team_maxMG42s.integer) {
			if(!quiet) 
				CP("cp \"^1MG42 not available^7\" 1");
			return qtrue;
		}
		break;
	case WP_FLAMETHROWER:
		if(team_maxFlamers.integer == -1) return qfalse;
		if(wcount >= team_maxFlamers.integer) {
			if(!quiet) 
				CP("cp \"^1FLAMETHROWER not available^7\" 1");
			return qtrue;
		}
		break;
	case WP_MORTAR:
		if(team_maxMortars.integer == -1) return qfalse;
		if(wcount >= team_maxMortars.integer) {
			if(!quiet) 
				CP("cp \"^1MORTAR not available^7\" 1");
			return qtrue;
		}
		break;
	// case WP_GPG40: - forty - needs to be WP_KAR98
	case WP_KAR98:
	case WP_CARBINE:
		if(team_maxGrenLaunchers.integer == -1) return qfalse;
		if(wcount >= team_maxGrenLaunchers.integer) {
			if(!quiet)
				CP("cp \"^1GRENADE LAUNCHER not available^7\" 1");
			return qtrue;
		}
		break;
	default:
		return qfalse;
	}
	
	return qfalse;
}

void G_SetClientWeapons( gentity_t* ent, weapon_t w1, weapon_t w2, qboolean updateclient ) {
	qboolean changed = qfalse;

	if( ent->client->sess.latchPlayerWeapon2 != w2 ) {
		ent->client->sess.latchPlayerWeapon2 = w2;
		changed = qtrue;
	}

	if(!G_IsWeaponDisabled(ent,
		w1,
		ent->client->sess.sessionTeam,
		qfalse)) {

		if( ent->client->sess.latchPlayerWeapon != w1 ) {
			ent->client->sess.latchPlayerWeapon = w1;
			changed = qtrue;
		}		
	} else {
		if( ent->client->sess.latchPlayerWeapon != 0 ) {
			ent->client->sess.latchPlayerWeapon = 0;
			changed = qtrue;
		}		
	}

	if( updateclient && changed ) {
		ClientUserinfoChanged( ent-g_entities );
	}
}

int G_ClassCount(gentity_t *ent, int playerType, team_t team)
{
	int i, j, cnt=0;

	if( playerType < PC_SOLDIER || playerType > PC_COVERTOPS )
		return 0;

	for( i = 0; i < level.numConnectedClients; i++ ) {
		j = level.sortedClients[i];

		if( j == ent-g_entities ) {
			continue;
		}

		if( level.clients[j].sess.sessionTeam != team ) {
			continue;
		}

		if( level.clients[j].sess.playerType != playerType && 
				level.clients[j].sess.latchPlayerType != 
						playerType ) {
			continue;
		}
		cnt++;
	}
	return cnt;
}

qboolean G_IsClassFull( gentity_t *ent, int playerType, team_t team)
{
	int maxCount, count, tcount;

	if( playerType < PC_MEDIC || playerType > PC_COVERTOPS )
		return qfalse;

	count = G_ClassCount(ent, playerType, team);
	tcount = G_NumPlayersOnTeam(team);
	switch( playerType ) {
		case PC_MEDIC:
			if( team_maxMedics.integer == -1 ) break;
			maxCount = team_maxMedics.integer;
			if( strstr(team_maxMedics.string,"%") ) {
				maxCount = ceil(team_maxMedics.integer 
						* tcount * 0.01f);
			}
			if( count >= maxCount )
			{
				CP( "cp \"^1Medic^7 is not available! "
					"Choose another class!\n\"" );
				return qtrue;
			}
			break;
		case PC_ENGINEER:
			if( team_maxEngineers.integer == -1 ) break;
			maxCount = team_maxEngineers.integer;
			if( strstr(team_maxEngineers.string,"%") ) {
				maxCount = ceil(team_maxEngineers.integer 
						* tcount * 0.01f);
			}
			if( count >= maxCount )
			{
				CP( "cp \"^1Engineer^7 is not available! "
					"Choose another class!\n\"" );
				return qtrue;
			}
			break;
		case PC_FIELDOPS:
			if( team_maxFieldOps.integer == -1 ) break;
			maxCount = team_maxFieldOps.integer;
			if( strstr(team_maxFieldOps.string,"%") ) {
				maxCount = ceil(team_maxFieldOps.integer 
						* tcount * 0.01f);
			}
			if( count >= maxCount )
			{
				CP( "cp \"^1Field Ops^7 is not available! "
					"Choose another class!\n\"" );
				return qtrue;
			}
			break;
		case PC_COVERTOPS:
			if( team_maxCovertOps.integer == -1 ) break;
			maxCount = team_maxCovertOps.integer;
			if( strstr(team_maxCovertOps.string,"%") ) {
				maxCount = ceil(team_maxCovertOps.integer 
						* tcount * 0.01f);
			}
			if( count >= maxCount )
			{
				CP( "cp \"^1Covert Ops^7 is not available! "
					"Choose another class!\n\"" );
				return qtrue;
			}
			break;
	} 
	return qfalse;
}

/*
=================
Cmd_Team_f
=================
*/
void Cmd_Team_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue ) {
	char		s[MAX_TOKEN_CHARS];
	char		ptype[4];
	char		weap[4], weap2[4];
	weapon_t	w, w2;
	int			playerType;
	team_t		team;
	spectatorState_t	specState;
	int					specClient;
	

	if ( trap_Argc() < 2 ) {
		char *pszTeamName;

		switch ( ent->client->sess.sessionTeam ) {
			case TEAM_ALLIES:
				pszTeamName = "Allies";
				break;
			case TEAM_AXIS:
				pszTeamName = "Axis";
				break;
			case TEAM_SPECTATOR:
				pszTeamName = "Spectator";
				break;
			case TEAM_FREE:
			default:
				pszTeamName = "Free";
				break;
		}

		CP(va("print \"%s team\n\"", pszTeamName));
		return;
	}

	trap_Argv( 1, s,		sizeof( s		));
	trap_Argv( 2, ptype,	sizeof( ptype	));
	trap_Argv( 3, weap,		sizeof( weap	));
	trap_Argv( 4, weap2,	sizeof( weap2	));


	G_TeamDataForString(s,
		ent->s.clientNum,
		&team,
		&specState,
		&specClient);


	if(*ptype) {
		playerType = atoi(ptype);
	}
	else if(ent->client->sess.latchPlayerType >= PC_SOLDIER &&
		ent->client->sess.latchPlayerType <= PC_COVERTOPS) {

		playerType = ent->client->sess.latchPlayerType;
	}
	else {
		playerType = PC_SOLDIER;
	}
	w = (*weap) ? atoi(weap) : ent->client->sess.latchPlayerWeapon;
	w2 = (*weap2) ? atoi(weap2) : ent->client->sess.latchPlayerWeapon2;
	
	if(G_IsClassFull(ent, playerType, team)) {
		CP("print \"team: class is not available\n\"");
		return;
	}
	ent->client->sess.latchPlayerType = playerType;

	if(G_IsWeaponDisabled(ent, w, team, qfalse)) {
		gitem_t *weapon = BG_FindItemForWeapon(w);
		if(weapon)
			CP(va("print \"team: %s is not available\n\"",
				weapon->pickup_name));
		return;
	}

	if(team == ent->client->sess.sessionTeam)
		G_SetClientWeapons(ent, w, w2, qtrue);
	else
		SetTeam(ent, s, qfalse, w, w2, qtrue);
}

void Cmd_ResetSetup_f( gentity_t* ent ) {
	qboolean changed = qfalse;

	if( !ent || !ent->client ) {
		return;
	}

	ent->client->sess.latchPlayerType =		ent->client->sess.playerType;
	
	if( ent->client->sess.latchPlayerWeapon != ent->client->sess.playerWeapon ) {
		ent->client->sess.latchPlayerWeapon = ent->client->sess.playerWeapon;
		changed = qtrue;
	}

	if( ent->client->sess.latchPlayerWeapon2 != ent->client->sess.playerWeapon2 ) {
		ent->client->sess.latchPlayerWeapon2 =	ent->client->sess.playerWeapon2;
		changed = qtrue;
	}

	if( changed ) {
		ClientUserinfoChanged( ent-g_entities );
	}
}

void Cmd_SetClass_f( gentity_t* ent, unsigned int dwCommand, qboolean fValue ) {
}

void Cmd_SetWeapons_f( gentity_t* ent, unsigned int dwCommand, qboolean fValue ) {
}



// START Mad Doc - TDF
/*
=================
Cmd_TeamBot_f
=================
*/
void Cmd_TeamBot_f(gentity_t *foo)
{
	char		ptype[4], weap[4], fireteam[4];
	char entNumStr[4];
	int entNum;
	char *weapon;
	char weaponBuf[MAX_INFO_STRING];
	char userinfo[MAX_INFO_STRING];

	gentity_t *ent;

	trap_Argv( 1, entNumStr, sizeof( entNumStr ) );
	entNum = atoi(entNumStr);

	ent = g_entities + entNum;

	trap_Argv( 3, ptype, sizeof( ptype ) );
	trap_Argv( 4, weap, sizeof( weap ) );
	trap_Argv( 5, fireteam, sizeof( fireteam) );



	ent->client->sess.latchPlayerType = atoi( ptype );
	ent->client->sess.latchPlayerWeapon = atoi( weap );
	ent->client->sess.latchPlayerWeapon2 = 0;
	ent->client->sess.playerType = atoi( ptype );
	ent->client->sess.playerWeapon = atoi( weap );

	// remove any weapon info from the userinfo, so SetWolfSpawnWeapons() doesn't reset the weapon as that
	trap_GetUserinfo(entNum, userinfo, sizeof(userinfo) );
		
	weapon = Info_ValueForKey( userinfo, "pWeapon");
	if (weapon[0])
	{
		Q_strncpyz(weaponBuf, weapon, sizeof(weaponBuf));
		Info_RemoveKey(userinfo, "pWeapon");
		trap_SetUserinfo(entNum, userinfo);
	}

 	SetWolfSpawnWeapons( ent->client ); 
}

// END Mad Doc - TDF
/*
=================
Cmd_Follow_f
=================
*/
void Cmd_Follow_f( gentity_t *ent, unsigned int dwCommand, qboolean fValue ) {
	int		i;
	char	arg[MAX_TOKEN_CHARS];
	int pids[MAX_CLIENTS];

	if ( trap_Argc() != 2 ) {
		if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
			StopFollowing( ent );
		}
		return;
	}

	if ((ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES) &&
			!(ent->client->ps.pm_flags & PMF_LIMBO)) {
		CP("print \"Can't follow while not in limbo if on a team!\n\"");
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	// josh: Let /follow match partial names
	// Use > instead of != since could be a team name
	if(ClientNumbersFromString(arg, pids) > 1) {
		CP("print \"Partial Name Matches more than 1 Player.\n\"");
		return;
	}
	i = pids[0];

	if ( i == -1 ) {
		if(!Q_stricmp(arg, "allies")) i = TEAM_ALLIES;
		else if(!Q_stricmp(arg, "axis")) i = TEAM_AXIS;
		else return;
		if ((ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES) &&
			ent->client->sess.sessionTeam != i) {
			CP("print \"Can't follow a player on an enemy team!\n\"");
			return;
		}

		if(!TeamCount(ent - g_entities, i)) {
			CP(va("print \"The %s team %s empty!  Follow command ignored.\n\"", aTeams[i],
																((ent->client->sess.sessionTeam != i) ? "is" : "would be")));
			return;
		}

		// Allow for simple toggle
		if(ent->client->sess.spec_team != i) {
			if(teamInfo[i].spec_lock && !(ent->client->sess.spec_invite & i)) CP(va("print \"Sorry, the %s team is locked from spectators.\n\"", aTeams[i]));
			else {
				ent->client->sess.spec_team = i;
				CP(va("print \"Spectator follow is now locked on the %s team.\n\"", aTeams[i]));
				Cmd_FollowCycle_f(ent, 1);
			}
		} else {
			ent->client->sess.spec_team = 0;
			CP(va("print \"%s team spectating is now disabled.\n\"", aTeams[i]));
		}

		return;
	}

	// Josh: Can't follow enemy players if not a spectator
	if ((ent->client->sess.sessionTeam == TEAM_AXIS ||
			ent->client->sess.sessionTeam == TEAM_ALLIES) &&
			ent->client->sess.sessionTeam != level.clients[ i ].sess.sessionTeam) {
		CP("print \"Can't follow a player on an enemy team!\n\"");
		return;
	}

	// can't follow self
	if ( &level.clients[ i ] == ent->client ) return;

	// can't follow another spectator
	if ( level.clients[ i ].sess.sessionTeam == TEAM_SPECTATOR ) return;
	if ( level.clients[ i ].ps.pm_flags & PMF_LIMBO ) return;

	// OSP - can't follow a player on a speclocked team, unless allowed
	if(!G_allowFollow(ent, level.clients[i].sess.sessionTeam)) {
		CP(va("print \"Sorry, the %s team is locked from spectators.\n\"", aTeams[level.clients[i].sess.sessionTeam]));
		return;
	}

	// first set them to spectator
	// josh: don't set them to spectator
	//if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
	//	SetTeam( ent, "spectator", qfalse, -1, -1, qfalse );
	//}

	ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
	ent->client->sess.spectatorClient = i;
}

/*
=================
Cmd_FollowCycle_f
=================
*/
void Cmd_FollowCycle_f( gentity_t *ent, int dir ) {
	int		clientnum;
	int		original;

	// first set them to spectator
	if (( ent->client->sess.spectatorState == SPECTATOR_NOT ) && (!( ent->client->ps.pm_flags & PMF_LIMBO)) ) { // JPW NERVE for limbo state
		SetTeam( ent, "spectator", qfalse, -1, -1, qfalse );
	}

	if ( dir != 1 && dir != -1 ) {
		G_Error( "Cmd_FollowCycle_f: bad dir %i", dir );
	}

	clientnum = ent->client->sess.spectatorClient;
	original = clientnum;
	do {
		clientnum += dir;
		if ( clientnum >= level.maxclients ) {
			clientnum = 0;
		}
		if ( clientnum < 0 ) {
			clientnum = level.maxclients - 1;
		}

		// can only follow connected clients
		if ( level.clients[ clientnum ].pers.connected != CON_CONNECTED ) {
			continue;
		}

		// can't follow another spectator
		if ( level.clients[ clientnum ].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}

		// JPW NERVE -- couple extra checks for limbo mode
		if (ent->client->ps.pm_flags & PMF_LIMBO) {
			if (level.clients[clientnum].ps.pm_flags & PMF_LIMBO)
				continue;
			if (level.clients[clientnum].sess.sessionTeam != ent->client->sess.sessionTeam)
				continue;
		}

		if (level.clients[clientnum].ps.pm_flags & PMF_LIMBO)
			continue;

		// OSP
		if(!G_desiredFollow(ent, level.clients[clientnum].sess.sessionTeam))
			continue;

		// this is good, we can use it
		ent->client->sess.spectatorClient = clientnum;
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		return;
	} while ( clientnum != original );

	// tjw: if we're still following the same guy and he's not
	//      on the same team, then follow ourself
	if(ent->client->sess.spectatorClient == original &&
		ent->client->sess.sessionTeam != 
		level.clients[original].sess.sessionTeam) {

		ent->client->ps.viewlocked = 0;
		ent->client->ps.viewlocked_entNum = 0;
		ent->client->sess.spectatorClient =
			(ent->client - level.clients);
		ent->client->sess.spectatorState = SPECTATOR_FREE;
	}

	// leave it where it was
}


/*======================
G_EntitySound
	Mad Doc xkan, 11/06/2002 -

	Plays a sound (wav file or sound script) on this entity

	Note that calling G_AddEvent(..., EV_GENERAL_SOUND, ...) has the danger of
	the event never getting through to the client because the entity might not
	be visible (unless it has the SVF_BROADCAST flag), so if you want to make sure
	the sound is heard, call this function instead.
======================*/
void G_EntitySound( 
	gentity_t *ent,			// entity to play the sound on
	const char *soundId,	// sound file name or sound script ID
	int volume)				// sound volume, only applies to sound file name call
							//   for sound script, volume is currently always 127.
{
	trap_SendServerCommand( -1, va("entitySound %d %s %d %i %i %i normal", ent->s.number, soundId, volume,
		(int)ent->s.pos.trBase[0], (int)ent->s.pos.trBase[1], (int)ent->s.pos.trBase[2] ));
}

/*======================
G_EntitySoundNoCut
	Mad Doc xkan, 1/16/2003 -

	Similar to G_EntitySound, but do not cut this sound off

======================*/
void G_EntitySoundNoCut( 
	gentity_t *ent,			// entity to play the sound on
	const char *soundId,	// sound file name or sound script ID
	int volume)				// sound volume, only applies to sound file name call
							//   for sound script, volume is currently always 127.
{
	trap_SendServerCommand( -1, va("entitySound %d %s %d %i %i %i noCut", ent->s.number, soundId, volume,
		(int)ent->s.pos.trBase[0], (int)ent->s.pos.trBase[1], (int)ent->s.pos.trBase[2] ));
}

char *G_ShortcutSanitize(char *text)
{
	static char n[MAX_SAY_TEXT] = {""};

	if(!*text)
		return n;

	Q_strncpyz(n, text, sizeof(n));

	Q_strncpyz(n, Q_StrReplace(n, "[a]", "(a)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[d]", "(d)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[h]", "(h)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[k]", "(k)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[l]", "(l)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[n]", "(n)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[r]", "(r)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[p]", "(p)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[s]", "(s)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[w]", "(w)"), sizeof(n));
	Q_strncpyz(n, Q_StrReplace(n, "[t]", "(t)"), sizeof(n));
	return n;
}

char *G_Shortcuts(gentity_t *ent, char *text)
{
	static char out[MAX_SAY_TEXT];
	char a[MAX_NAME_LENGTH] = {"*unknown*"};
	char d[MAX_NAME_LENGTH] = {"*unknown*"};
	char h[MAX_NAME_LENGTH] = {"*unknown*"};
	char k[MAX_NAME_LENGTH] = {"*unknown*"};
	char l[32] = {"*unknown*"};
	char n[MAX_NAME_LENGTH] = {"*unknown*"};
	char r[MAX_NAME_LENGTH] = {"*unknown*"};
	char p[MAX_NAME_LENGTH] = {"*unknown*"};
	char s[32] = {"*unknown*"};
	char w[32] = {"*unknown*"};
	char t[32] = {"*unknown*"};
	gclient_t *client = NULL;
	gentity_t *crosshairEnt;
	char *rep;
	gitem_t *weapon;
	int clip;
	int ammo;

	out[0] = '\0';

	if(ent->client->pers.lastammo_client != -1) {
		client = &level.clients[ent->client->pers.lastammo_client];
		if(client) {
			Q_strncpyz(a,
				G_ShortcutSanitize(client->pers.netname),
				sizeof(a));
		}
	}

	if(ent->client->pers.lastkiller_client != -1) {
		client = &level.clients[ent->client->pers.lastkiller_client];
		if(client) {
			Q_strncpyz(d,
				G_ShortcutSanitize(client->pers.netname),
				sizeof(d));
		}
	}

	if(ent->client->pers.lasthealth_client != -1) {
		client = &level.clients[ent->client->pers.lasthealth_client];
		if(client) {
			Q_strncpyz(h,
				G_ShortcutSanitize(client->pers.netname),
				sizeof(h));
		}
	}

	if(ent->client->pers.lastkilled_client != -1) {
		client = &level.clients[ent->client->pers.lastkilled_client];
		if(client) {
			Q_strncpyz(k,
				G_ShortcutSanitize(client->pers.netname),
				sizeof(k));
		}
	}


	Q_strncpyz(l,
		BG_GetLocationString(ent->r.currentOrigin),
		sizeof(l));

	Q_strncpyz(n,
		G_ShortcutSanitize(ent->client->pers.netname),
		sizeof(n));

	if(ent->client->pers.lastrevive_client != -1) {
		client = &level.clients[ent->client->pers.lastrevive_client];
		if(client) {
			Q_strncpyz(r,
				G_ShortcutSanitize(client->pers.netname),
				sizeof(r));
		}
	}

	crosshairEnt = &g_entities[ent->client->ps.identifyClient];
	if(crosshairEnt && crosshairEnt->client && crosshairEnt->inuse) {
		client = crosshairEnt->client;
		if(client) {
			Q_strncpyz(p,
				G_ShortcutSanitize(client->pers.netname),
				sizeof(p));
		}
	}

	Com_sprintf(s, sizeof(s), "%i", ent->health);

	weapon = BG_FindItemForWeapon(ent->client->ps.weapon);
	Q_strncpyz(w, weapon->pickup_name, sizeof(w));

	clip = BG_FindClipForWeapon(ent->client->ps.weapon);
	ammo = BG_FindAmmoForWeapon(ent->client->ps.weapon);
	Com_sprintf(t, sizeof(t), "%i",
		(ent->client->ps.ammoclip[clip] +
		((ent->client->ps.weapon == WP_KNIFE) ? 0 : ent->client->ps.ammo[ammo])));

	rep = Q_StrReplace(text, "[a]", a);
	rep = Q_StrReplace(rep, "[d]", d);
	rep = Q_StrReplace(rep, "[h]", h);
	rep = Q_StrReplace(rep, "[k]", k);
	rep = Q_StrReplace(rep, "[l]", l);
	rep = Q_StrReplace(rep, "[n]", n);
	rep = Q_StrReplace(rep, "[r]", r);
	rep = Q_StrReplace(rep, "[p]", p);
	rep = Q_StrReplace(rep, "[s]", s);
	rep = Q_StrReplace(rep, "[w]", w);
	rep = Q_StrReplace(rep, "[t]", t);

	Q_strncpyz(out, rep, sizeof(out));
	return out;
}

/*
==================
G_Say
==================
*/

void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message, qboolean localize )
{
	char cmd[6];

	if( !other || !other->inuse || !other->client ) {
		return;
	}
	if((mode == SAY_TEAM || mode == SAY_TEAMNL) && !OnSameTeam(ent, other) && 
			!G_shrubbot_permission(other,SBF_SPEC_ALLCHAT) ) {
		return;
	}
	if((mode == SAY_TEAM || mode == SAY_TEAMNL) && 
			G_shrubbot_permission(other,SBF_SPEC_ALLCHAT) &&
			other->client->sess.sessionTeam != TEAM_SPECTATOR && 
			!OnSameTeam(ent, other) ) {
		return;
	}

	// NERVE - SMF - if spectator, no chatting to players in WolfMP
	if ( match_mutespecs.integer > 0 && ent->client->sess.referee == 0 &&	// OSP
		(( ent->client->sess.sessionTeam == TEAM_FREE && other->client->sess.sessionTeam != TEAM_FREE ) ||
		( ent->client->sess.sessionTeam == TEAM_SPECTATOR && other->client->sess.sessionTeam != TEAM_SPECTATOR ))) {
		return;
	} else {
		if ( mode == SAY_BUDDY ) { 	// send only to people who have the sender on their buddy list
			if(ent->s.clientNum != other->s.clientNum) {
				fireteamData_t *ft1, *ft2;
				if(!G_IsOnFireteam( other-g_entities, &ft1 )) {
					return;
				}
				if(!G_IsOnFireteam( ent-g_entities, &ft2 )) {
					return;
				}
				if(ft1 != ft2) {
					return;
				}
			}
		}

		if(COM_BitCheck(other->client->sess.ignoreClients,
			(ent - g_entities))) {

			Q_strncpyz(cmd, "print", sizeof(cmd));
		}
		else if(mode == SAY_TEAM || mode == SAY_BUDDY) {
			Q_strncpyz(cmd, "tchat", sizeof(cmd));
		}
		else {
			Q_strncpyz(cmd, "chat", sizeof(cmd));
		}

		trap_SendServerCommand(other-g_entities,
			va("%s \"%s%c%c%s%s\" %i %i",
			cmd, name, Q_COLOR_ESCAPE, color,
			message,
			(!Q_stricmp(cmd, "print")) ? "\n" : "",
			ent-g_entities, localize));
	}
}

void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText ) {
	int			j;
	gentity_t	*other;
	int			color;
	char		name[64];
	// don't let text be too long for malicious reasons
	char		text[MAX_SAY_TEXT];
	char		censoredText[MAX_SAY_TEXT];
	qboolean	localize = qfalse;
	char		*loc;
	char		*shortcuts;

	Q_strncpyz( text, chatText, sizeof(text) );
	if (g_censor.string[0] && 
			!(G_shrubbot_permission(ent,SBF_NOCENSORFLOOD))) {
		SanitizeString(text, censoredText, qtrue);
		if (G_CensorText(censoredText,&censorDictionary)) {
			// like shrub, if the text is censored, 
			// it becomes colorless
			Q_strncpyz( text, censoredText, sizeof(text) );
			G_CensorPenalize(ent);
		}
	}

	if(g_shortcuts.integer) {
		shortcuts = G_Shortcuts(ent, text);
		Q_strncpyz(text, shortcuts, sizeof(text));
	}

	switch ( mode ) {
	default:
	case SAY_ALL:
		G_LogPrintf( "say: %s: %s\n", ent->client->pers.netname, text );
		Com_sprintf (name, sizeof(name), "%s%c%c: ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_GREEN;
		break;
	case SAY_BUDDY:
		localize = qtrue;
		G_LogPrintf( "saybuddy: %s: %s\n", ent->client->pers.netname, text );
		loc = BG_GetLocationString( ent->r.currentOrigin );
		Com_sprintf (name, sizeof(name), "[lof](%s%c%c) (%s): ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, loc);
		color = COLOR_YELLOW;
		break;
	case SAY_TEAM:
		localize = qtrue;
		G_LogPrintf( "sayteam: %s: %s\n", ent->client->pers.netname, text );
		loc = BG_GetLocationString( ent->r.currentOrigin );
		Com_sprintf (name, sizeof(name), "[lof](%s%c%c) (%s): ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, loc);
		color = COLOR_CYAN;
		break;
	case SAY_TEAMNL:
		G_LogPrintf( "sayteamnl: %s: %s\n", ent->client->pers.netname, text );
		Com_sprintf (name, sizeof(name), "(%s^7): ", ent->client->pers.netname);
		color = COLOR_CYAN;
		break;
	}


	if ( target ) {
		if( !COM_BitCheck( target->client->sess.ignoreClients, ent - g_entities ) ) {
			G_SayTo( ent, target, mode, color, name, text, localize );
		}
		return;
	}

	// echo the text to the console
	if ( g_dedicated.integer ) {
		G_Printf( "%s%s\n", name, text);
	}

	// send it to all the apropriate clients
	for(j=0; j<level.numConnectedClients; j++) {
		other = &g_entities[level.sortedClients[j]];
		// tjw: move this to G_SayTo()
		//if( !COM_BitCheck( other->client->sess.ignoreClients, ent - g_entities ) ) {
		//	G_SayTo( ent, other, mode, color, name, text, localize );
		//}
			G_SayTo( ent, other, mode, color, name, text, localize );
		}

	G_shrubbot_cmd_check(ent);
}


/*
==================
Cmd_Say_f
==================
*/
void Cmd_Say_f( gentity_t *ent, int mode, qboolean arg0 )
{
	char *args;

	if(trap_Argc() < 2 && !arg0) return;
	args = Q_SayConcatArgs(0);
	if(g_privateMessages.integer &&
		(!Q_stricmpn(args, "say /m ", 7) ||
		 !Q_stricmpn(args, "say_team /m ", 12) ||
		 !Q_stricmpn(args, "say_buddy /m ", 13))) {

		G_PrivateMessage(ent);
	}
	else
	G_Say(ent, NULL, mode, ConcatArgs(((arg0) ? 0 : 1)));
}

extern void BotRecordVoiceChat( int client, int destclient, const char *id, int mode, qboolean noResponse );

// NERVE - SMF
void G_VoiceTo( gentity_t *ent, gentity_t *other, int mode, const char *id, qboolean voiceonly ) {
	int color;
	char *cmd;

	if (!other) {
		return;
	}
	if (!other->inuse) {
		return;
	}
	if (!other->client) {
		return;
	}
	if ( mode == SAY_TEAM && !OnSameTeam(ent, other) ) {
		return;
	}

	// OSP - spec vchat rules follow the same as normal chatting rules
	if(match_mutespecs.integer > 0 && ent->client->sess.referee == 0 &&
	  ent->client->sess.sessionTeam == TEAM_SPECTATOR && other->client->sess.sessionTeam != TEAM_SPECTATOR) {
		return;
	}

	// send only to people who have the sender on their buddy list
	if ( mode == SAY_BUDDY ) {
		if(ent->s.clientNum != other->s.clientNum) {
			fireteamData_t *ft1, *ft2;
			if(!G_IsOnFireteam( other-g_entities, &ft1 )) {
				return;
			}
			if(!G_IsOnFireteam( ent-g_entities, &ft2 )) {
				return;
			}
			if(ft1 != ft2) {
				return;
			}
		}
	}

	if( mode == SAY_TEAM ) {
		color = COLOR_CYAN;
		cmd = "vtchat";
	} else if( mode == SAY_BUDDY ) {
		color = COLOR_YELLOW;
		cmd = "vbchat";
	} else {
		color = COLOR_GREEN;
		cmd = "vchat";
	}

	// RF, record this chat so bots can parse them
	// bots respond with voiceonly, so we check for this so they dont keep responding to responses
	BotRecordVoiceChat( ent->s.number, other->s.number, id, mode, voiceonly == 2 );


	if (voiceonly == 2) {
		voiceonly = qfalse;
	}

	if( mode == SAY_TEAM || mode == SAY_BUDDY ) {
		CPx( other-g_entities, va("%s %d %d %d %s %i %i %i", cmd, voiceonly, ent - g_entities, color, id, (int)ent->s.pos.trBase[0], (int)ent->s.pos.trBase[1], (int)ent->s.pos.trBase[2] ));
	} else {
		CPx( other-g_entities, va("%s %d %d %d %s", cmd, voiceonly, ent - g_entities, color, id ));
	}
}

extern void AI_RecordVoiceChat(  gentity_t *other, const char *id );

void G_Voice( gentity_t *ent, gentity_t *target, int mode, const char *id, qboolean voiceonly ) {
	int			j;
	gentity_t *victim;

#ifdef __BOT__
	if (ent->r.svFlags & SVF_BOT)
	{
		if (ent->bot_next_voice_time > level.time)
			return;

		ent->bot_next_voice_time = level.time + 10000;
	}
#endif //__BOT__

	// DHM - Nerve :: Don't allow excessive spamming of voice chats
	ent->voiceChatSquelch -= (level.time - ent->voiceChatPreviousTime);
	ent->voiceChatPreviousTime = level.time;

	if ( ent->voiceChatSquelch < 0 )
		ent->voiceChatSquelch = 0;

#ifdef __BOT__
#ifdef __MEDIC_WAYPOINTING__
	if (!strcmp(id, "Medic"))
	{// Need a medic!
		AI_Needs_Medic(ent->s.clientNum);
	}
	else 
#endif //__MEDIC_WAYPOINTING__
	if (!strcmp(id, "NeedAmmo"))
	{// Need ammo!
		AI_Needs_Ammo(ent->s.clientNum);
	}
	else if (!strcmp(id, "FollowMe"))
	{// Need ammo!
		Scan_AI_For_Followers(ent->s.clientNum);
	}
#endif

	// Only do the spam check for MP
	if ( ent->voiceChatSquelch >= 30000 ) {
#ifdef __BOT__
		if (!(ent->r.svFlags & SVF_BOT))
#endif //__BOT__
			trap_SendServerCommand( ent-g_entities, "cpm \"^1Spam Protection^7: VoiceChat ignored\n\"" );
		return;
	}

// Only do the spam check for MP
 	if ( ent->voiceChatSquelch >= 30000 && 
 			!G_shrubbot_permission(ent, SBF_NOCENSORFLOOD) ) {
 		// CHRUKER: b066 - Was using the cpm command, but this needs
 		//          to be displayed immediately.
#ifdef __BOT__
		if (!(ent->r.svFlags & SVF_BOT))
#endif //__BOT__
 			trap_SendServerCommand( ent-g_entities,
 				"cp \"^1Spam Protection^7: VoiceChat ignored\"" );
  		return;
  	}

	if ( g_voiceChatsAllowed.integer )
		ent->voiceChatSquelch += (34000 / g_voiceChatsAllowed.integer);
	else
		return;
	// dhm


	// OSP - Charge for the lame spam!
	/*if(mode == SAY_ALL && (!Q_stricmp(id, "DynamiteDefused") || !Q_stricmp(id, "DynamitePlanted"))) {
		return;
	}*/

	if ( target ) {
		G_VoiceTo( ent, target, mode, id, voiceonly );
		return;
	}

	// echo the text to the console
	if ( g_dedicated.integer ) {
		G_Printf( "voice: %s %s\n", ent->client->pers.netname, id);
	}

	if( mode == SAY_BUDDY ) {
		char buffer[32];
		int	cls = -1, i, cnt, num;
		qboolean allowclients[MAX_CLIENTS];

		memset( allowclients, 0, sizeof( allowclients ) );

		trap_Argv( 1, buffer, 32 );

		cls = atoi( buffer );

		trap_Argv( 2, buffer, 32 );
		cnt = atoi( buffer );
		if( cnt > MAX_CLIENTS ) {
			cnt = MAX_CLIENTS;
		}

		for( i = 0; i < cnt; i++ ) {
			trap_Argv( 3 + i, buffer, 32 );

			num = atoi( buffer );
			if( num < 0 ) {
				continue;
			}
			if( num >= MAX_CLIENTS ) {
				continue;
			}

			allowclients[ num ] = qtrue;
		}

		for( j = 0; j < level.numConnectedClients; j++ ) {
			victim = &g_entities[level.sortedClients[j]];
			if( level.sortedClients[j] != ent->s.clientNum ) {
				if( cls != -1 && cls != level.clients[level.sortedClients[j]].sess.playerType ) {
					continue;
				}
			}

			if( cnt ) {
				if( !allowclients[ level.sortedClients[j] ] ) {
					continue;
				}
			}

			if(COM_BitCheck(victim->client->sess.ignoreClients,
				(ent - g_entities))) {
			
				continue;
			}

			G_VoiceTo(ent, &g_entities[level.sortedClients[j]], mode, id, voiceonly);
		}
	} else {

		// send it to all the apropriate clients
		for( j = 0; j < level.numConnectedClients; j++ ) {
			victim = &g_entities[level.sortedClients[j]];
			if(COM_BitCheck(victim->client->sess.ignoreClients,
				(ent - g_entities))) {
			
				continue;
			}
			G_VoiceTo(ent, victim, mode, id, voiceonly);
		}
	}

#ifdef __BOT__
#ifdef __MEDIC_WAYPOINTING__
	if (!strcmp(id, "Medic"))
	{// Need a medic!
		AI_Needs_Medic(ent->s.clientNum);
		//set_medic_route(ent);
	}
	else 
#endif //__MEDIC_WAYPOINTING__
	if (!strcmp(id, "NeedAmmo"))
	{// Need ammo!
		AI_Needs_Ammo(ent->s.clientNum);
		//set_fieldops_route(ent);
	}
	else if (!strcmp(id, "FollowMe"))
	{// Need ammo!
		Scan_AI_For_Followers(ent->s.clientNum);
		//set_follow_route(ent);
	}
	else if (!strcmp(id, "NeedBackup")&& !strcmp(id, "CoverMe"))
	{// Need ammo!
		Scan_AI_For_Followers(ent->s.clientNum);
		//set_follow_route(ent);
	}
	else if (!strcmp(id, "NeedEngineer"))
	{// Need Engineer!
		set_engineer_route(ent);
		//G_Printf("Done");
	} 
	else 
	AI_RecordVoiceChat( ent, id);
#endif //__BOT__
}

/*
==================
Cmd_Voice_f
==================
*/
static void Cmd_Voice_f( gentity_t *ent, int mode, qboolean arg0, qboolean voiceonly ) {
	if( mode != SAY_BUDDY ) {
		if(trap_Argc() < 2 && !arg0) {
			return;
		}
		G_Voice(ent, NULL, mode, ConcatArgs(((arg0) ? 0 : 1)), voiceonly);
	} else {
		char buffer[16];
		int index;
	
		trap_Argv( 2, buffer, sizeof( buffer ) );
		index = atoi( buffer );
		if( index < 0 ) {
			index = 0;
		}

		if( trap_Argc() < 3 + index && !arg0 ) {
			return;
		}
		G_Voice(ent, NULL, mode, ConcatArgs(((arg0) ? 2 + index : 3 + index)), voiceonly);
	}
}

// TTimo gcc: defined but not used
#if 0
/*
==================
Cmd_VoiceTell_f
==================
*/
static void Cmd_VoiceTell_f( gentity_t *ent, qboolean voiceonly ) {
	int			targetNum;
	gentity_t	*target;
	char		*id;
	char		arg[MAX_TOKEN_CHARS];

	if ( trap_Argc () < 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	targetNum = atoi( arg );
	if ( targetNum < 0 || targetNum >= level.maxclients ) {
		return;
	}

	target = &g_entities[targetNum];
	if ( !target || !target->inuse || !target->client ) {
		return;
	}

	id = ConcatArgs( 2 );

	G_LogPrintf( "vtell: %s to %s: %s\n", ent->client->pers.netname, target->client->pers.netname, id );
	G_Voice( ent, target, SAY_TELL, id, voiceonly );
	// don't tell to the player self if it was already directed to this player
	// also don't send the chat back to a bot
	if ( ent != target && !(ent->r.svFlags & SVF_BOT)) {
		G_Voice( ent, ent, SAY_TELL, id, voiceonly );
	}
}
#endif

// TTimo gcc: defined but not used
#if 0
/*
==================
Cmd_VoiceTaunt_f
==================
*/
static void Cmd_VoiceTaunt_f( gentity_t *ent ) {
	gentity_t *who;
	int i;

	if (!ent->client) {
		return;
	}

	// insult someone who just killed you
	if (ent->enemy && ent->enemy->client && ent->enemy->client->lastkilled_client == ent->s.number) {
		// i am a dead corpse
		if (!(ent->enemy->r.svFlags & SVF_BOT)) {
//			G_Voice( ent, ent->enemy, SAY_TELL, VOICECHAT_DEATHINSULT, qfalse );
		}
		if (!(ent->r.svFlags & SVF_BOT)) {
//			G_Voice( ent, ent,        SAY_TELL, VOICECHAT_DEATHINSULT, qfalse );
		}
		ent->enemy = NULL;
		return;
	}
	// insult someone you just killed
	if (ent->client->lastkilled_client >= 0 && ent->client->lastkilled_client != ent->s.number) {
		who = g_entities + ent->client->lastkilled_client;
		if (who->client) {
			// who is the person I just killed
			if (who->client->lasthurt_mod == MOD_GAUNTLET) {
				if (!(who->r.svFlags & SVF_BOT)) {
//					G_Voice( ent, who, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse );	// and I killed them with a gauntlet
				}
				if (!(ent->r.svFlags & SVF_BOT)) {
//					G_Voice( ent, ent, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse );
				}
			} else {
				if (!(who->r.svFlags & SVF_BOT)) {
//					G_Voice( ent, who, SAY_TELL, VOICECHAT_KILLINSULT, qfalse );	// and I killed them with something else
				}
				if (!(ent->r.svFlags & SVF_BOT)) {
//					G_Voice( ent, ent, SAY_TELL, VOICECHAT_KILLINSULT, qfalse );
				}
			}
			ent->client->lastkilled_client = -1;
			return;
		}
	}

	if (g_gametype.integer >= GT_TEAM) {
		// praise a team mate who just got a reward
		for(i = 0; i < MAX_CLIENTS; i++) {
			who = g_entities + i;
			if (who->client && who != ent && who->client->sess.sessionTeam == ent->client->sess.sessionTeam) {
				if (who->client->rewardTime > level.time) {
					if (!(who->r.svFlags & SVF_BOT)) {
//						G_Voice( ent, who, SAY_TELL, VOICECHAT_PRAISE, qfalse );
					}
					if (!(ent->r.svFlags & SVF_BOT)) {
//						G_Voice( ent, ent, SAY_TELL, VOICECHAT_PRAISE, qfalse );
					}
					return;
				}
			}
		}
	}

	// just say something
//	G_Voice( ent, NULL, SAY_ALL, VOICECHAT_TAUNT, qfalse );
}
// -NERVE - SMF
#endif

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f( gentity_t *ent ) {
	trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", vtos( ent->s.origin ) ) );
}

/*
==================
Cmd_CallVote_f
==================
*/
qboolean Cmd_CallVote_f( gentity_t *ent, unsigned int dwCommand, qboolean fRefCommand ) {
	int		i;
	char	arg1[MAX_STRING_TOKENS];
	char	arg2[MAX_STRING_TOKENS];
	char voteDesc[VOTE_MAXSTRING];
	qboolean sbfNoVoteLimit;

	// Normal checks, if its not being issued as a referee command
	// CHRUKER: b067 - Was using the cpm command, but these needs to
	//          be displayed immediately.
	if( !fRefCommand ) {
		if( level.voteInfo.voteTime ) {
			CP("cp \"A vote is already in progress.\"");
			return qfalse;
		} else if( level.intermissiontime ) {
			CP("cp \"Cannot callvote during intermission.\"");
			return qfalse;
		} else if( !ent->client->sess.referee ) {
			sbfNoVoteLimit = G_shrubbot_permission(ent,
					SBF_NO_VOTE_LIMIT);
			if( voteFlags.integer == VOTING_DISABLED && !sbfNoVoteLimit) {
				CP("cp \"Voting not enabled on this server.\"");
				return qfalse;
			} else if( vote_limit.integer > 0 && ent->client->pers.voteCount >= vote_limit.integer && 
					!sbfNoVoteLimit ) {
				CP(va("cp \"You have already called the maximum number of votes (%d).\"", vote_limit.integer));
				return qfalse;
			} else if( ent->client->sess.sessionTeam == TEAM_SPECTATOR) {
				CP("cp \"Not allowed to call a vote as a spectator.\"");
				return qfalse;
			}
		}
	}

	// make sure it is a valid command to vote on
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	if(strchr( arg1, ';' ) || strchr( arg2, ';' ) ) {
		char *strCmdBase = (!fRefCommand) ? "vote" : "ref command";

		G_refPrintf(ent, "Invalid %s string.", strCmdBase);
		return(qfalse);
	}


	if(trap_Argc() > 1 && (i = G_voteCmdCheck(ent, arg1, arg2, fRefCommand)) != G_NOTFOUND) {	//  --OSP
		if(i != G_OK)	 {
			if(i == G_NOTFOUND) return(qfalse);	// Command error
			else return(qtrue);
		}
	} else {
		if(!fRefCommand) {
			CP(va("print \"\n^3>>> Unknown vote command: ^7%s %s\n\"", arg1, arg2));
			G_voteHelp(ent, qtrue);
		}
		return(qfalse);
	}

	Com_sprintf(level.voteInfo.voteString, sizeof(level.voteInfo.voteString), "%s %s", arg1, arg2);

	// start the voting, the caller automatically votes yes
	// If a referee, vote automatically passes.	// OSP
	if( fRefCommand ) {
//		level.voteInfo.voteYes = level.voteInfo.numVotingClients + 10;	// JIC :)
		// Don't announce some votes, as in comp mode, it is generally a ref
		// who is policing people who shouldn't be joining and players don't want
		// this sort of spam in the console
		if(level.voteInfo.vote_fn != G_Kick_v && level.voteInfo.vote_fn != G_Mute_v) {
			AP("cp \"^1** Referee Server Setting Change **\n\"");
		}

		// Gordon: just call the stupid thing.... don't bother with the voting faff
		level.voteInfo.vote_fn(NULL, 0, NULL, NULL, qfalse);

		G_globalSound("sound/misc/referee.wav");
	} else {
		level.voteInfo.voteYes = 1;
		AP(va("print \"[lof]%s^7 [lon]called a vote.[lof]  Voting for: %s\n\"", ent->client->pers.netname, level.voteInfo.voteString));
		level.voteInfo.voteCaller = ent->s.number;
		level.voteInfo.voteTeam = ent->client->sess.sessionTeam;
		AP(va("print \"[lof]%s^7 [lon]called a vote.\n\"", ent->client->pers.netname));
		AP(va("cp \"[lof]%s\n^7[lon]called a vote.\n\"", ent->client->pers.netname));
		G_globalSound("sound/misc/vote.wav");
	}

	level.voteInfo.voteTime = level.time;
	level.voteInfo.voteNo = 0;

	// Don't send the vote info if a ref initiates (as it will automatically pass)
	if(!fRefCommand) {
		for(i=0; i<level.numConnectedClients; i++) {
			level.clients[level.sortedClients[i]].ps.eFlags &= ~EF_VOTED;
		}

		ent->client->pers.voteCount++;
		ent->client->ps.eFlags |= EF_VOTED;

		trap_SetConfigstring(CS_VOTE_YES,	 va("%i", level.voteInfo.voteYes));
		trap_SetConfigstring(CS_VOTE_NO,	 va("%i", level.voteInfo.voteNo));
		Q_strncpyz(voteDesc,
			level.voteInfo.voteString,
			sizeof(voteDesc));
		if((g_voting.integer & VOTEF_DISP_CALLER)) {
			Q_strcat(voteDesc, sizeof(voteDesc)," (called by ");
			Q_strcat(voteDesc,
				sizeof(voteDesc),
				ent->client->pers.netname);
			Q_strcat(voteDesc, sizeof(voteDesc),")");
		}
		trap_SetConfigstring(CS_VOTE_STRING, voteDesc);
		trap_SetConfigstring(CS_VOTE_TIME,	 va("%i", level.voteInfo.voteTime));
	}

	return(qtrue);
}

qboolean StringToFilter( const char *s, ipFilter_t *f );

qboolean G_FindFreeComplainIP( gclient_t* cl, ipFilter_t* ip ) {
	int i = 0;

	if( !g_ipcomplaintlimit.integer ) {
		return qtrue;
	}

	for( i = 0; i < MAX_COMPLAINTIPS && i < g_ipcomplaintlimit.integer; i++ ) {
		if( !cl->pers.complaintips[i].compare && !cl->pers.complaintips[i].mask ) {
			cl->pers.complaintips[i].compare = ip->compare;
			cl->pers.complaintips[i].mask = ip->mask;
			return qtrue;
		}
		if( ( cl->pers.complaintips[i].compare & cl->pers.complaintips[i].mask ) == ( ip->compare & ip->mask ) ) {
			return qtrue;
		}
	}
	return qfalse;
}

/*
==================
Cmd_Vote_f
==================
*/
void Cmd_Vote_f( gentity_t *ent ) {
	char		msg[64];
	int			num;

	// DHM - Nerve :: Complaints supercede voting (and share command)
	if ( ent->client->pers.complaintEndTime > level.time && g_gamestate.integer == GS_PLAYING && g_complaintlimit.integer ) {

		gentity_t* other = &g_entities[ ent->client->pers.complaintClient ];
		gclient_t *cl = other->client;
		if ( !cl )
			return;
		if ( cl->pers.connected != CON_CONNECTED )
			return;
		if ( cl->pers.localClient ) {
			trap_SendServerCommand( ent-g_entities, "complaint -3" );
			return;
		}

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			// Increase their complaint counter
			cl->pers.complaints++;

			num = g_complaintlimit.integer - cl->pers.complaints;

			if( !cl->pers.localClient ) {
				
				const char* value;
				char userinfo[MAX_INFO_STRING];
				ipFilter_t ip;

				trap_GetUserinfo( ent-g_entities, userinfo, sizeof( userinfo ) );
				value = Info_ValueForKey( userinfo, "ip" );

				StringToFilter( value, &ip );

				if( num <= 0 || !G_FindFreeComplainIP( cl, &ip ) ) {
					trap_DropClient( cl - level.clients, "kicked after too many complaints.", cl->sess.referee ? 0 : 300 );
					trap_SendServerCommand( ent-g_entities, "complaint -1" );
					return;
				}
			}

#ifdef __BOT__
			if (!(ent->r.svFlags & SVF_BOT))
#endif //__BOT__
				trap_SendServerCommand( ent->client->pers.complaintClient, va("cpm \"^1Warning^7: Complaint filed against you by %s^* You have Lost XP.\n\"", ent->client->pers.netname ) );

			trap_SendServerCommand( ent-g_entities, "complaint -1" );
			
			AddScore( other, WOLF_FRIENDLY_PENALTY );

			G_LoseKillSkillPoints( other, ent->sound2to1, ent->sound1to2, ent->sound2to3 ? qtrue : qfalse );
		} else {
			trap_SendServerCommand( ent->client->pers.complaintClient, "cpm \"No complaint filed against you.\n\"" );
			trap_SendServerCommand( ent-g_entities, "complaint -2" );
		}

		// Reset this ent's complainEndTime so they can't send multiple complaints
		ent->client->pers.complaintEndTime = -1;
		ent->client->pers.complaintClient = -1;

		return;
	}
	// dhm

	if ( ent->client->pers.applicationEndTime > level.time ) {

		gclient_t *cl = g_entities[ ent->client->pers.applicationClient ].client;
		if ( !cl )
			return;
		if ( cl->pers.connected != CON_CONNECTED )
			return;

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			trap_SendServerCommand( ent-g_entities, "application -4" );
			trap_SendServerCommand( ent->client->pers.applicationClient, "application -3" );

			G_AddClientToFireteam( ent->client->pers.applicationClient, ent-g_entities );
		}
		else {
			trap_SendServerCommand( ent-g_entities, "application -4" );
			trap_SendServerCommand( ent->client->pers.applicationClient, "application -2" );
		}

		ent->client->pers.applicationEndTime = 0;
		ent->client->pers.applicationClient = -1;

		return;
	}

	ent->client->pers.applicationEndTime = 0;
	ent->client->pers.applicationClient = -1;

	if ( ent->client->pers.invitationEndTime > level.time ) {

		gclient_t *cl = g_entities[ ent->client->pers.invitationClient ].client;
		if ( !cl )
			return;
		if ( cl->pers.connected != CON_CONNECTED )
			return;

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			trap_SendServerCommand( ent-g_entities, "invitation -4" );
			trap_SendServerCommand( ent->client->pers.invitationClient, "invitation -3" );

			G_AddClientToFireteam( ent-g_entities, ent->client->pers.invitationClient );
		} else {
			trap_SendServerCommand( ent-g_entities, "invitation -4" );
			trap_SendServerCommand( ent->client->pers.invitationClient, "invitation -2" );
		}

		ent->client->pers.invitationEndTime = 0;
		ent->client->pers.invitationClient = -1;

		return;
	}

	ent->client->pers.invitationEndTime = 0;
	ent->client->pers.invitationClient = -1;

	if( ent->client->pers.propositionEndTime > level.time ) {
		gclient_t *cl = g_entities[ ent->client->pers.propositionClient ].client;
		if ( !cl )
			return;
		if ( cl->pers.connected != CON_CONNECTED )
			return;

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			trap_SendServerCommand( ent-g_entities, "proposition -4" );
			trap_SendServerCommand( ent->client->pers.propositionClient2, "proposition -3" );

			G_InviteToFireTeam( ent-g_entities, ent->client->pers.propositionClient );
		} else {
			trap_SendServerCommand( ent-g_entities, "proposition -4" );
			trap_SendServerCommand( ent->client->pers.propositionClient2, "proposition -2" );
		}

		ent->client->pers.propositionEndTime = 0;
		ent->client->pers.propositionClient = -1;
		ent->client->pers.propositionClient2 = -1;

		return;
	}

	if( ent->client->pers.autofireteamEndTime > level.time ) {
		fireteamData_t* ft;

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			trap_SendServerCommand( ent-g_entities, "aft -2" );

			if( G_IsFireteamLeader( ent-g_entities, &ft ) ) {
				ft->priv = qtrue;
			}
		} else {
			trap_SendServerCommand( ent-g_entities, "aft -2" );
		}

		ent->client->pers.autofireteamEndTime = 0;

		return;
	}

	if( ent->client->pers.autofireteamCreateEndTime > level.time ) {
		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			trap_SendServerCommand( ent-g_entities, "aftc -2" );

			G_RegisterFireteam( ent-g_entities );
		} else {
			trap_SendServerCommand( ent-g_entities, "aftc -2" );
		}

		ent->client->pers.autofireteamCreateEndTime = 0;

		return;
	}

	if( ent->client->pers.autofireteamJoinEndTime > level.time ) {
		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			fireteamData_t* ft;

			trap_SendServerCommand( ent-g_entities, "aftj -2" );


			ft = G_FindFreePublicFireteam( ent->client->sess.sessionTeam );
			if( ft ) {
				G_AddClientToFireteam( ent-g_entities, ft->joinOrder[0] );
			}
		} else {
			trap_SendServerCommand( ent-g_entities, "aftj -2" );
		}

		ent->client->pers.autofireteamCreateEndTime = 0;

		return;
	}

	ent->client->pers.propositionEndTime = 0;
	ent->client->pers.propositionClient = -1;
	ent->client->pers.propositionClient2 = -1;

	// dhm
	// Reset this ent's complainEndTime so they can't send multiple complaints
	ent->client->pers.complaintEndTime = -1;
	ent->client->pers.complaintClient = -1;

	if ( !level.voteInfo.voteTime ) {
		trap_SendServerCommand( ent-g_entities, "print \"No vote in progress.\n\"" );
		return;
	}
	if ( ent->client->ps.eFlags & EF_VOTED ) {
		trap_SendServerCommand( ent-g_entities, "print \"Vote already cast.\n\"" );
		return;
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Not allowed to vote as spectator.\n\"" );
		return;
	}

	if( level.voteInfo.vote_fn == G_Kick_v ) {
		int pid = atoi( level.voteInfo.vote_value );
		if( !g_entities[ pid ].client ) {
			return;
		}

		if( g_entities[ pid ].client->sess.sessionTeam != TEAM_SPECTATOR && ent->client->sess.sessionTeam != g_entities[ pid ].client->sess.sessionTeam ) {
			trap_SendServerCommand( ent - g_entities, "print \"Cannot vote to kick player on opposing team.\n\"" );
			return;
		}
	}
	else if(level.voteInfo.vote_fn == G_Surrender_v) {
		if(ent->client->sess.sessionTeam != level.voteInfo.voteTeam) {
			CP("cp \"You cannot vote on the other team's surrender\"");
			return;
		}
	}

	trap_SendServerCommand( ent-g_entities, "print \"Vote cast.\n\"" );

	ent->client->ps.eFlags |= EF_VOTED;

	trap_Argv( 1, msg, sizeof( msg ) );

	if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
		level.voteInfo.voteYes++;
		trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteInfo.voteYes ) );
	} else {
		level.voteInfo.voteNo++;
		trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteInfo.voteNo ) );	
	}

	// a majority will be determined in G_CheckVote, which will also account
	// for players entering or leaving
}


qboolean G_canPickupMelee(gentity_t *ent) {
// JPW NERVE -- no "melee" weapons in net play
		return qfalse;
}
// jpw




/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent ) {
	vec3_t		origin, angles;
	char		buffer[MAX_TOKEN_CHARS];
	int			i;

	if ( !g_cheats.integer ) {
		trap_SendServerCommand( ent-g_entities, va("print \"Cheats are not enabled on this server.\n\""));
		return;
	}
	if ( trap_Argc() != 5 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"usage: setviewpos x y z yaw\n\""));
		return;
	}

	VectorClear( angles );
	for ( i = 0 ; i < 3 ; i++ ) {
		trap_Argv( i + 1, buffer, sizeof( buffer ) );
		origin[i] = atof( buffer );
	}

	trap_Argv( 4, buffer, sizeof( buffer ) );
	angles[YAW] = atof( buffer );

	TeleportPlayer( ent, origin, angles );
}

/*
=================
Cmd_StartCamera_f
=================
*/
void Cmd_StartCamera_f( gentity_t *ent ) {

	if( ent->client->cameraPortal  )
		G_FreeEntity( ent->client->cameraPortal );
	ent->client->cameraPortal = G_Spawn();
	ent->client->cameraPortal->classname = "camera";
	ent->client->cameraPortal->s.eType = ET_CAMERA;
	ent->client->cameraPortal->s.apos.trType = TR_STATIONARY;
	ent->client->cameraPortal->s.apos.trTime = 0;
	ent->client->cameraPortal->s.apos.trDuration = 0;
	VectorClear( ent->client->cameraPortal->s.angles );
	VectorClear( ent->client->cameraPortal->s.apos.trDelta );
	G_SetOrigin( ent->client->cameraPortal, ent->r.currentOrigin );
	VectorCopy( ent->r.currentOrigin, ent->client->cameraPortal->s.origin2 );

	ent->client->cameraPortal->s.frame = 0;

	ent->client->cameraPortal->r.svFlags |= (SVF_PORTAL|SVF_SINGLECLIENT);
	ent->client->cameraPortal->r.singleClient = ent->client->ps.clientNum;
	
	ent->client->ps.eFlags |= EF_VIEWING_CAMERA;
	ent->s.eFlags |= EF_VIEWING_CAMERA;

	VectorCopy( ent->r.currentOrigin, ent->client->cameraOrigin );	// backup our origin

// (SA) trying this in client to avoid 1 frame of player drawing
//	ent->client->ps.eFlags |= EF_NODRAW;
//	ent->s.eFlags |= EF_NODRAW;
}

/*
=================
Cmd_StopCamera_f
=================
*/
void Cmd_StopCamera_f( gentity_t *ent ) {
//	gentity_t *sp;

	if( ent->client->cameraPortal && (ent->client->ps.eFlags & EF_VIEWING_CAMERA) ) {
		// send a script event
//		G_Script_ScriptEvent( ent->client->cameraPortal, "stopcam", "" );

		// go back into noclient mode
		G_FreeEntity( ent->client->cameraPortal );
		ent->client->cameraPortal = NULL;

		ent->s.eFlags &= ~EF_VIEWING_CAMERA;
		ent->client->ps.eFlags &= ~EF_VIEWING_CAMERA;

		//G_SetOrigin( ent, ent->client->cameraOrigin );	// restore our origin
		//VectorCopy( ent->client->cameraOrigin, ent->client->ps.origin );

// (SA) trying this in client to avoid 1 frame of player drawing
//		ent->s.eFlags &= ~EF_NODRAW;
//		ent->client->ps.eFlags &= ~EF_NODRAW;

		// RF, if we are near the spawn point, save the "current" game, for reloading after death
//		sp = NULL;
    // gcc: suggests () around assignment used as truth value
//		while ((sp = G_Find( sp, FOFS(classname), "info_player_deathmatch" ))) {	// info_player_start becomes info_player_deathmatch in it's spawn functon
//			if (Distance( ent->s.pos.trBase, sp->s.origin ) < 256 && trap_InPVS( ent->s.pos.trBase, sp->s.origin )) {
//				G_SaveGame( NULL );
//				break;
//			}
//		}
	}
}

/*
=================
Cmd_SetCameraOrigin_f
=================
*/
void Cmd_SetCameraOrigin_f( gentity_t *ent ) {
	char		buffer[MAX_TOKEN_CHARS];
	int i;
	vec3_t		origin;

	if ( trap_Argc() != 4 ) {
		return;
	}

	for ( i = 0 ; i < 3 ; i++ ) {
		trap_Argv( i + 1, buffer, sizeof( buffer ) );
		origin[i] = atof( buffer );
	}

	if( ent->client->cameraPortal ) {
		//G_SetOrigin( ent->client->cameraPortal, origin );	// set our origin
		VectorCopy( origin, ent->client->cameraPortal->s.origin2 );
		trap_LinkEntity( ent->client->cameraPortal );
	//	G_SetOrigin( ent, origin );	// set our origin
	//	VectorCopy( origin, ent->client->ps.origin );
	}
}

extern gentity_t *BotFindEntityForName( char *name );

/*
==============
Cmd_InterruptCamera_f
==============
*/
void Cmd_InterruptCamera_f( gentity_t *ent ) {
	gentity_t *player;

	if( g_gametype.integer != GT_SINGLE_PLAYER && g_gametype.integer != GT_COOP && g_gametype.integer != GT_NEWCOOP )
		return;

	player = BotFindEntityForName( "player" );

	if( !player )
		return;

	G_Script_ScriptEvent( player, "trigger", "cameraInterrupt" );
}

extern vec3_t playerMins;
extern vec3_t playerMaxs;

qboolean G_TankIsOccupied( gentity_t* ent ) {
	if( !ent->tankLink ) {
		return qfalse;
	}

	return qtrue;
}

qboolean G_TankIsMountable( gentity_t* ent, gentity_t* other ) {
	if( !(ent->spawnflags & 128) ) {
		return qfalse;
	}

	if( level.disableTankEnter ) {
		return qfalse;
	}

	//KW: bugfix: you can't get in a tank while scoped
	if( BG_IsScopedWeapon( other->client->ps.weapon ) ) {
		return qfalse;
	}

	if( G_TankIsOccupied( ent ) ) {
		return qfalse;
	}

	if( ent->health <= 0 ) {
		return qfalse;
	}

	if( other->client->ps.weaponDelay ) {
		return qfalse;
	}

	return qtrue;
}

// Rafael
/*
==================
G_UniformSteal formerly known as Cmd_Activate2_f
==================
*/
qboolean G_UniformSteal(gentity_t *ent, gentity_t *traceEnt) 
{
	// already used corpse
	if(traceEnt->activator)
		return qfalse;
	if(ent->client->sess.playerType != PC_COVERTOPS)
		return qfalse;
	// CHRUKER: b006 - Only steal if we are alive
	if(ent->health <= 0) 
		return qfalse;
	if(ent->client->ps.powerups[PW_OPS_DISGUISED])
		return qfalse;
	if(ent->client->ps.powerups[PW_BLUEFLAG] || 
		ent->client->ps.powerups[PW_REDFLAG])
		return qfalse;
	if(BODY_TEAM(traceEnt) == ent->client->sess.sessionTeam)
		return qfalse;
	if(BODY_TEAM(traceEnt) > 2) return qfalse; //Perro: prevents stealing bug
	if( BODY_VALUE(traceEnt) < 250 ) {
		BODY_VALUE(traceEnt) += 5;
		return qfalse;
	}

						traceEnt->nextthink = traceEnt->timestamp + BODY_TIME(BODY_TEAM(traceEnt));
			
	//BG_AnimScriptEvent( &ent->client->ps, ent->client->pers.character->animModelInfo, ANIM_ET_PICKUPGRENADE, qfalse, qtrue );
	//ent->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
	//ent->client->ps.pm_time = 2100;

						ent->client->ps.powerups[PW_OPS_DISGUISED] = 1;
						ent->client->ps.powerups[PW_OPS_CLASS_1] = BODY_CLASS(traceEnt) & 1;
						ent->client->ps.powerups[PW_OPS_CLASS_2] = BODY_CLASS(traceEnt) & 2;
						ent->client->ps.powerups[PW_OPS_CLASS_3] = BODY_CLASS(traceEnt) & 4;

						BODY_TEAM(traceEnt) += 4;
						traceEnt->activator = ent;

						traceEnt->s.time2 =	1;

						// sound effect
						G_AddEvent( ent, EV_DISGUISE_SOUND, 0 );

						G_AddSkillPoints( ent, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, 5.f );
	G_DebugAddSkillPoints( ent, SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS, 5.f, "stealing uniform" ); 

	Q_strncpyz(ent->client->disguiseNetname,
		g_entities[traceEnt->s.clientNum].client->pers.netname,
		sizeof(ent->client->disguiseNetname));
	ent->client->disguiseRank = g_entities[traceEnt->s.clientNum].client ? 
		g_entities[traceEnt->s.clientNum].client->sess.rank : 0;

						ClientUserinfoChanged( ent->s.clientNum );


	return qtrue;
}

/* 
  G_ClassStealFixWeapons() //tjw
  a helper for handling weapons for G_ClassSteal
  */
void G_ClassStealFixWeapons(gclient_t *client) 
{
	// tools
	COM_BitClear(client->ps.weapons, WP_DYNAMITE);
	COM_BitClear(client->ps.weapons, WP_PLIERS);
	COM_BitClear(client->ps.weapons, WP_LANDMINE);
	COM_BitClear(client->ps.weapons, WP_SMOKE_BOMB);
	COM_BitClear(client->ps.weapons, WP_SATCHEL);
	COM_BitClear(client->ps.weapons, WP_SATCHEL_DET);
	COM_BitClear(client->ps.weapons, WP_SMOKE_MARKER);
	
	COM_BitClear(client->ps.weapons, WP_AMMO);
	COM_BitClear(client->ps.weapons, WP_MEDKIT);
	COM_BitClear(client->ps.weapons, WP_MEDIC_SYRINGE);
	COM_BitClear(client->ps.weapons, WP_MEDIC_ADRENALINE);

	// primary weapons
	COM_BitClear(client->ps.weapons, WP_PANZERFAUST);
	COM_BitClear(client->ps.weapons, WP_FLAMETHROWER);
	COM_BitClear(client->ps.weapons, WP_MOBILE_MG42);
	COM_BitClear(client->ps.weapons, WP_MOBILE_MG42_SET);
#ifdef __EF__
	COM_BitClear(client->ps.weapons, WP_30CAL);
	COM_BitClear(client->ps.weapons, WP_30CAL_SET);
    COM_BitClear(client->ps.weapons, WP_PTRS);
	COM_BitClear(client->ps.weapons, WP_PTRS_SET);
	COM_BitClear(client->ps.weapons, WP_PPSH);
	COM_BitClear(client->ps.weapons, WP_STG44);
	COM_BitClear(client->ps.weapons, WP_PPS);
	COM_BitClear(client->ps.weapons, WP_SVT40);
	COM_BitClear(client->ps.weapons, WP_AIRDROP_MARKER);
#endif //__EF__
	COM_BitClear(client->ps.weapons, WP_MORTAR);
	COM_BitClear(client->ps.weapons, WP_MORTAR_SET);
	COM_BitClear(client->ps.weapons, WP_FG42);
	COM_BitClear(client->ps.weapons, WP_FG42SCOPE);
	COM_BitClear(client->ps.weapons, WP_K43);
	COM_BitClear(client->ps.weapons, WP_GARAND);
	COM_BitClear(client->ps.weapons, WP_K43_SCOPE);
	COM_BitClear(client->ps.weapons, WP_GARAND_SCOPE);
	COM_BitClear(client->ps.weapons, WP_STEN);
	COM_BitClear(client->ps.weapons, WP_MP40);
	COM_BitClear(client->ps.weapons, WP_THOMPSON);
	COM_BitClear(client->ps.weapons, WP_KAR98);
	COM_BitClear(client->ps.weapons, WP_GPG40);
	COM_BitClear(client->ps.weapons, WP_CARBINE);
	COM_BitClear(client->ps.weapons, WP_M7);

	
	// silence pistols are different than regular pistols so swap them
	if(client->sess.playerType == PC_COVERTOPS) {
		client->pmext.silencedSideArm = 1;
		COM_BitSet(client->ps.weapons, WP_SILENCED_LUGER);
		COM_BitSet(client->ps.weapons, WP_SILENCED_COLT);
		if(COM_BitCheck(client->ps.weapons, WP_AKIMBO_LUGER)) {
			COM_BitClear(client->ps.weapons, WP_AKIMBO_LUGER);
			COM_BitSet(client->ps.weapons, WP_AKIMBO_SILENCEDLUGER);
		}
		if(COM_BitCheck(client->ps.weapons, WP_AKIMBO_COLT)) {
			COM_BitClear(client->ps.weapons, WP_AKIMBO_COLT);
			COM_BitSet(client->ps.weapons, WP_AKIMBO_SILENCEDCOLT);
					}
				}
	else {
		client->pmext.silencedSideArm = 0;
		COM_BitClear(client->ps.weapons, WP_SILENCED_LUGER);
		COM_BitClear(client->ps.weapons, WP_SILENCED_COLT);
		if(COM_BitCheck(client->ps.weapons, WP_AKIMBO_SILENCEDLUGER)) {
			COM_BitClear(client->ps.weapons, WP_AKIMBO_SILENCEDLUGER);
			COM_BitSet(client->ps.weapons, WP_AKIMBO_LUGER);
			}
		if(COM_BitCheck(client->ps.weapons, WP_AKIMBO_SILENCEDCOLT)) {
			COM_BitClear(client->ps.weapons, WP_AKIMBO_SILENCEDCOLT);
			COM_BitSet(client->ps.weapons, WP_AKIMBO_COLT);
		}
	}

	// give them all the tool-like weapons for the class
	G_AddClassSpecificTools(client);
}


/*
 G_ClassSteal //tjw
 */

qboolean G_ClassSteal(gentity_t *stealer, gentity_t *deadguy) 
{
	if(!g_classChange.integer) return qfalse;
	if(deadguy->activator) return qfalse; // already used corpse
	if(BODY_TEAM(deadguy) > 2) return qfalse; //Perro: prevents stealing bug

	if( BODY_VALUE(deadguy) < 250 ) {
		if(BODY_VALUE(deadguy) == 0) {
			CPx(stealer->s.number, "cp \"Switching Classes\" 1");
		}
		BODY_VALUE(deadguy) += 5;
		return qtrue;
	}

	deadguy->nextthink = deadguy->timestamp + BODY_TIME(BODY_TEAM(deadguy));
	
	deadguy->activator = stealer;
	deadguy->s.time2 =	1;

	// sound effect
	G_AddEvent(stealer, EV_DISGUISE_SOUND, 0);

	// I guess this disables future use of the corpse 
	BODY_TEAM(deadguy) += 4;

	stealer->client->sess.playerType = BODY_CLASS(deadguy);

	G_ClassStealFixWeapons(stealer->client);

	// team_max[weapon] and g_heavyWeaponRestriction checks
	// perro: only give heavy weapons to soldiers
	if(!G_IsWeaponDisabled(stealer,
			BODY_WEAPON(deadguy),
			stealer->client->sess.sessionTeam,
			qfalse) &&
			stealer->client->sess.playerType == PC_SOLDIER) {

			COM_BitSet(stealer->client->ps.weapons, BODY_WEAPON(deadguy));
			stealer->client->ps.weapon = BODY_WEAPON(deadguy);
	}
	else if((BODY_CLASS(deadguy) == PC_COVERTOPS)
		&& (stealer->client->sess.playerType == PC_COVERTOPS)){
		COM_BitSet(stealer->client->ps.weapons, WP_STEN);
		stealer->client->ps.weapon = WP_STEN;
	}
	else if(BODY_TEAM(deadguy) == TEAM_AXIS) {
		COM_BitSet(stealer->client->ps.weapons, WP_MP40);
		stealer->client->ps.weapon = WP_MP40;
	}
	else {
		COM_BitSet(stealer->client->ps.weapons, WP_THOMPSON);
		stealer->client->ps.weapon = WP_THOMPSON;
	}

	// if they took a gren launching weapon, they can have the
	// grenade launcher too I guess
	// perro: only give nade launchers to engies
	if(COM_BitCheck(stealer->client->ps.weapons, WP_GARAND)
		&& (stealer->client->sess.playerType == PC_ENGINEER)) {
		COM_BitSet(stealer->client->ps.weapons, WP_CARBINE);
	}
	else if(COM_BitCheck(stealer->client->ps.weapons, WP_K43) 
		&& (stealer->client->sess.playerType == PC_ENGINEER)) {
		COM_BitSet(stealer->client->ps.weapons, WP_GPG40);
	}

	// make sure they lose disguise when changing classes
	stealer->client->ps.powerups[PW_OPS_DISGUISED] = 0;

	ClientUserinfoChanged(stealer->s.clientNum);

	return qtrue;
}

qboolean G_PushPlayer(gentity_t *ent, gentity_t *victim) 
{
	vec3_t	dir, push;

	if(!g_shove.integer)
		return qfalse;

	if(ent->health <= 0)
		return qfalse;

	if((level.time - ent->client->pmext.shoveTime) < 500) {
		return qfalse;
	}

	ent->client->pmext.shoveTime = level.time;

	VectorSubtract(victim->r.currentOrigin, ent->r.currentOrigin, dir);

	VectorNormalizeFast(dir);

	VectorScale(dir, (g_shove.value * 5.0f), push);

	if((abs(push[2]) > abs(push[0])) &&
		(abs(push[2]) > abs(push[1]))) {

		// player is being boosted
		if(g_shoveNoZ.integer) {
			return qfalse;
		}
		push[2] = g_shove.value;
		push[0] = push[1] = 0;
	}
	else {
		// give them a little hop
		push[2] = 24;
	}

	VectorAdd(victim->s.pos.trDelta, push, victim->s.pos.trDelta);
	VectorAdd(victim->client->ps.velocity, push,
		victim->client->ps.velocity);

	if(g_shoveSound.string != '\0') {
		G_AddEvent(victim, EV_GENERAL_SOUND,
			G_SoundIndex(g_shoveSound.string));
	}
	return qtrue;
}


/* 
 G_DragCorpse //tjw
 */
qboolean G_DragCorpse(gentity_t *dragger, gentity_t *corpse) 
{
	vec3_t	dir, pull, res;
	float dist;

	if(!g_dragCorpse.integer) return qfalse;
	VectorSubtract(dragger->r.currentOrigin, corpse->r.currentOrigin, dir);
	dir[2] = 0;
	dist = VectorNormalize(dir);

	// don't pull corpses past the dragger's head and don't start dragging
	// until both players look like they're in contact
	if(dist > 85 || dist < 40) return qfalse;

	VectorScale(dir, 110, pull);

	// prevent some zipping around when the corpse doesn't have 
	// much friction
	VectorSubtract(pull, corpse->client->ps.velocity, res);

	// no dragging into the ground
	res[2] = 0;

	VectorAdd(corpse->s.pos.trDelta, res, corpse->s.pos.trDelta );
	VectorAdd(corpse->client->ps.velocity, res, corpse->client->ps.velocity);

	return qtrue; // no checks yet
}

// TAT 1/14/2003 - extracted out the functionality of Cmd_Activate_f from finding the object to use
//		so we can force bots to use items, without worrying that they are looking EXACTLY at the target

#ifdef __VEHICLES__
extern vec3_t	tankMins;
extern vec3_t	tankMaxs;
extern vec3_t	playerMins;
extern vec3_t	playerMaxs;

gentity_t *G_BotLeavePlayerTank( gentity_t* ent, qboolean position );
#endif //__VEHICLES__

qboolean Do_Activate_f(gentity_t *ent, gentity_t *traceEnt) {
	qboolean found = qfalse;
	qboolean	walking = qfalse;
	vec3_t		forward;	//, offset, end;
	//trace_t		tr;
	gclient_t *client;

	if (ent->client)
		client = ent->client;
#ifdef __NPC__
	else if (ent->NPC_client)
		client = ent->NPC_client;
#endif //__NPC__

#ifdef __VEHICLES__
	if ( traceEnt
		&& !(ent->r.svFlags & SVF_BOT)
		&& (traceEnt->client && (traceEnt->r.svFlags & SVF_BOT) && (traceEnt->client->ps.eFlags & EF_VEHICLE)) 
		&& !(ent->client && !(traceEnt->r.svFlags & SVF_BOT) && (ent->client->ps.eFlags & EF_VEHICLE))
		&& OnSameTeam(ent, traceEnt)) 
	{// UQ1: Added.. Kick bots out of tanks and take their place!
		gentity_t *tank = NULL;

		// First, kick the bot out!
		tank = G_BotLeavePlayerTank( traceEnt, qtrue );

		if (!tank)
		{// Failed...
			//G_Printf("*** 2\n");
			return found;
		}

		if (traceEnt->client->ps.eFlags & EF_VEHICLE)
		{// Failed to kick them...
			//G_Printf("*** 3\n");
			return found;
		}

		// Now enter the tank...
		VectorCopy(tank->s.angles, client->ps.viewangles);
		VectorCopy(tank->s.angles, ent->r.currentAngles);
		VectorCopy(tank->s.angles, ent->s.angles);
		client->ps.eFlags |= EF_VEHICLE;
		
#ifdef __BOT__
		if (ent->r.svFlags & SVF_BOT)
		{// UQ1: Make a new goal once in a tank!
			ent->current_node = -1;
			ent->longTermGoal = -1;
		}
#endif //__BOT__

		client->ps.persistant[PERS_TANKUSED] = tank->s.nextWeapon;

		VectorClear( client->ps.velocity );
		TeleportPlayer( ent, tank->s.pos.trBase, tank->s.angles  );
		found = qtrue;
		ent->TankMarker = tank->TankMarker; // mark ourself as tank, for spawning entity
		ent->BackupHealth = client->ps.stats[STAT_HEALTH];
		ent->health = tank->health; 
		ent->maxhealth = tank->maxhealth;
		client->ps.stats[STAT_HEALTH] = (int)((float)((float)ent->health/(float)ent->maxhealth)*(float)100); // send health to clients
		ent->backupWeaponTime = client->ps.weaponTime;
		client->ps.weaponTime = tank->backupWeaponTime;
		tank->active = qtrue;
		ent->s.effect1Time = traceEnt->s.effect1Time; // UQ1: Set vechicle type...
		client->ps.stats[STAT_VEHICLE_CLASS] = tank->s.weapon; // UQ1: Vehicle Class
		RemoveTankTurret(tank);
		G_FreeEntity( tank );
		
		AddTankTurret(ent);

		return found;
	}
#endif //__VEHICLES__

	// Arnout: invisible entities can't be used

	if( traceEnt->entstate == STATE_INVISIBLE || traceEnt->entstate == STATE_UNDERCONSTRUCTION ) {
		//G_Printf("*** 1\n");
		return qfalse;
	}

	if((client->pers.cmd.buttons & BUTTON_WALKING) ||
		(client->ps.pm_flags & PMF_DUCKED)) {

		walking = qtrue;
	}

	if (traceEnt->classname)
	{
		traceEnt->flags &= ~FL_SOFTACTIVATE;	// FL_SOFTACTIVATE will be set if the user is holding 'walk' key

		if (traceEnt->s.eType == ET_ALARMBOX)
		{
			trace_t		trace;

			if ( client->sess.sessionTeam == TEAM_SPECTATOR )
				return qfalse;
			
			memset( &trace, 0, sizeof(trace) );

			if(traceEnt->use)
				G_UseEntity( traceEnt, ent, 0 );
			found = qtrue;
		}
		else if (traceEnt->s.eType == ET_ITEM)
		{
			trace_t		trace;

			if ( client->sess.sessionTeam == TEAM_SPECTATOR )
				return qfalse;
			
			memset( &trace, 0, sizeof(trace) );

			if( traceEnt->touch ) {
				if( client->pers.autoActivate == PICKUP_ACTIVATE )
					client->pers.autoActivate = PICKUP_FORCE;		//----(SA) force pickup
				traceEnt->active = qtrue;
				traceEnt->touch( traceEnt, ent, &trace );
			}

			found = qtrue;
		} else if ( traceEnt->s.eType == ET_MOVER && G_TankIsMountable( traceEnt, ent ) ) {
			G_Script_ScriptEvent( traceEnt, "mg42", "mount" );
			ent->tagParent = traceEnt->nextTrain;
			Q_strncpyz( ent->tagName, "tag_player", MAX_QPATH );
			ent->backupWeaponTime = client->ps.weaponTime;
			client->ps.weaponTime = traceEnt->backupWeaponTime;
			client->ps.weapHeat[WP_DUMMY_MG42] = traceEnt->mg42weapHeat;
			
			ent->tankLink = traceEnt;
			traceEnt->tankLink = ent;

			G_ProcessTagConnect( ent, qtrue );
			found = qtrue;
		} else if( G_EmplacedGunIsMountable( traceEnt, ent ) ) {
			//gclient_t* cl = &level.clients[ ent->s.clientNum ];
			gclient_t* cl = client;
			vec3_t	point;

			AngleVectors (traceEnt->s.apos.trBase, forward, NULL, NULL);
			VectorMA (traceEnt->r.currentOrigin, -36, forward, point);
			point[2] = ent->r.currentOrigin[2];

			// Save initial position
			VectorCopy( point, ent->TargetAngles );

			// Zero out velocity
			VectorCopy( vec3_origin, client->ps.velocity );
			VectorCopy( vec3_origin, ent->s.pos.trDelta );

			traceEnt->active = qtrue;
			ent->active = qtrue;
			traceEnt->r.ownerNum = ent->s.number;
			VectorCopy (traceEnt->s.angles, traceEnt->TargetAngles);
			traceEnt->s.otherEntityNum = ent->s.number;

			cl->pmext.harc = traceEnt->harc;
			cl->pmext.varc = traceEnt->varc;
			VectorCopy( traceEnt->s.angles, cl->pmext.centerangles );
			cl->pmext.centerangles[PITCH] = AngleNormalize180( cl->pmext.centerangles[PITCH] );
			cl->pmext.centerangles[YAW] = AngleNormalize180( cl->pmext.centerangles[YAW] );
			cl->pmext.centerangles[ROLL] = AngleNormalize180( cl->pmext.centerangles[ROLL] );

			ent->backupWeaponTime = client->ps.weaponTime;
			client->ps.weaponTime = traceEnt->backupWeaponTime;
			client->ps.weapHeat[WP_DUMMY_MG42] = traceEnt->mg42weapHeat;

			if(!(g_coverts.integer & COVERTF_NO_MG_EXPOSURE))
				client->ps.powerups[PW_OPS_DISGUISED] = 0;

			G_UseTargets( traceEnt, ent);	//----(SA)	added for Mike so mounting an MG42 can be a trigger event (let me know if there's any issues with this)
			found = qtrue;
		} else if ( ( (Q_stricmp (traceEnt->classname, "func_door") == 0) || (Q_stricmp (traceEnt->classname, "func_door_rotating") == 0) ) ) {
			if( walking ) {
				traceEnt->flags |= FL_SOFTACTIVATE;		// no noise
			}
			G_TryDoor(traceEnt, ent, ent);		// (door,other,activator)
			found = qtrue;
		} else if ( (Q_stricmp (traceEnt->classname, "team_WOLF_checkpoint") == 0) ) {
			if( traceEnt->count != client->sess.sessionTeam ) {
				traceEnt->health++;
			}
			found = qtrue;
		} else if ( (Q_stricmp (traceEnt->classname, "func_button") == 0) && ( traceEnt->s.apos.trType == TR_STATIONARY && traceEnt->s.pos.trType == TR_STATIONARY) && traceEnt->active == qfalse ) {
			Use_BinaryMover (traceEnt, ent, ent);
			traceEnt->active = qtrue;
			found = qtrue;
		} else if ( !Q_stricmp (traceEnt->classname, "func_invisible_user") ) {
			if( walking ) {
				traceEnt->flags |= FL_SOFTACTIVATE;		// no noise
			}
			G_UseEntity( traceEnt, ent, ent );
			found = qtrue;
		} else if ( !Q_stricmp (traceEnt->classname, "props_footlocker") ) {
			G_UseEntity( traceEnt, ent, ent );
			found = qtrue;
#ifdef __VEHICLES__
		} else if ( traceEnt->s.eType == ET_VEHICLE && !(client->ps.eFlags & EF_VEHICLE)) {
			VectorCopy(traceEnt->s.angles, client->ps.viewangles);
			VectorCopy(traceEnt->s.angles, ent->r.currentAngles);
			VectorCopy(traceEnt->s.angles, ent->s.angles);
			client->ps.eFlags |= EF_VEHICLE;

#ifdef __BOT__
			if (ent->r.svFlags & SVF_BOT)
			{// UQ1: Make a new goal once in a tank!
				ent->current_node = -1;
				ent->longTermGoal = -1;
			}
#endif //__BOT__

			client->ps.persistant[PERS_TANKUSED] = traceEnt->s.nextWeapon;
			
			VectorClear( client->ps.velocity );
			TeleportPlayer( ent, traceEnt->s.pos.trBase, traceEnt->s.angles  );
			found = qtrue;
			ent->TankMarker = traceEnt->TankMarker; // mark ourself as tank, for spawning entity
			ent->BackupHealth = client->ps.stats[STAT_HEALTH];
			ent->health = traceEnt->health; 
			ent->maxhealth = traceEnt->maxhealth;
			client->ps.stats[STAT_HEALTH] = (int)((float)((float)ent->health/(float)ent->maxhealth)*(float)100); // send health to clients
			ent->backupWeaponTime = client->ps.weaponTime;
			client->ps.weaponTime = traceEnt->backupWeaponTime;
			ent->s.effect1Time = traceEnt->s.effect1Time; // UQ1: Set vechicle type...
			traceEnt->active = qtrue;
			client->ps.stats[STAT_VEHICLE_CLASS] = traceEnt->s.weapon; // UQ1: Vehicle Class
			RemoveTankTurret( traceEnt );
			G_FreeEntity( traceEnt );

			AddTankTurret(ent);
#endif //__VEHICLES__	
		} 
	}

	return found;
}

gentity_t *G_LeaveTank( gentity_t* ent, qboolean position ) {
#ifdef __VEHICLES__
	gentity_t* tank;
	// found our tank (or whatever)
	vec3_t axis[3];
	vec3_t pos;
	trace_t tr;
	gclient_t *client;

	if (ent->client)
		client = ent->client;
#ifdef __NPC__
	else if (ent->NPC_client)
		client = ent->NPC_client;
#endif //__NPC__

	tank = ent->tankLink;
	if( !tank ) {
		return NULL;
	}

	if( position ) {

		AnglesToAxis( tank->s.angles, axis );

		VectorMA( client->ps.origin, 128, axis[1], pos );
		trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

		if( tr.startsolid ) {
			// try right
			VectorMA( client->ps.origin, -128, axis[1], pos );
			trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

			if( tr.startsolid ) {
				// try back
				VectorMA( client->ps.origin, -224, axis[0], pos );
				trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

				if( tr.startsolid ) {
					// try front
					VectorMA( client->ps.origin, 224, axis[0], pos );
					trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

					if( tr.startsolid ) {
						// give up
						return NULL;
					}
				}
			}
		}

		VectorClear( client->ps.velocity ); // Gordon: dont want them to fly away ;D
		TeleportPlayer( ent, pos, client->ps.viewangles );
	}


	tank->mg42weapHeat = client->ps.weapHeat[WP_DUMMY_MG42];
	tank->backupWeaponTime = client->ps.weaponTime;
	client->ps.weaponTime = ent->backupWeaponTime;

	G_Script_ScriptEvent( tank, "mg42", "unmount" );
	ent->tagParent = NULL;
	*ent->tagName = '\0';
	ent->s.eFlags &= ~EF_MOUNTEDTANK;
	ent->client->ps.eFlags &= ~EF_MOUNTEDTANK;
	tank->s.powerups = -1;

	tank->tankLink = NULL;
	ent->tankLink = NULL;
	ent->client->ps.eFlags &= ~EF_VEHICLE;

	tank->s.effect1Time = ent->s.effect1Time; // UQ1: Set vechicle type...
	tank->s.weapon = client->ps.stats[STAT_VEHICLE_CLASS] ; // UQ1: Vehicle Class
	return tank;
#endif //__VEHICLES__

	return NULL;
}

extern void tank_think(gentity_t *self); 
extern void tank_die(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod); 

#ifdef __VEHICLES__
gentity_t *tankSpawn (gentity_t *self, vec3_t origin, vec3_t angle) {
	gentity_t	*tank;

	tank = G_Spawn();
	switch ( self->client->ps.persistant[PERS_TANKUSED] ){
 	default:
	case VEHICLE_TYPE_CHURCHILL:
		tank->classname = "churchilltank";
		tank->s.effect1Time = VEHICLE_TYPE_CHURCHILL;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_HEAVY_TANK;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/churchhill_full.md3");
		break;
	case VEHICLE_TYPE_PANZER:
		tank->classname = "panzertank";
		tank->s.effect1Time = VEHICLE_TYPE_PANZER;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_HEAVY_TANK;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/jagdpanther_full.md3");
		break;
    case VEHICLE_TYPE_KV1:
		tank->classname = "kv1tank";
		tank->s.effect1Time = VEHICLE_TYPE_KV1;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_HEAVY_TANK;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/kv1_full.md3");
		break;
    case VEHICLE_TYPE_T34:
		tank->classname = "t34tank";
		tank->s.effect1Time = VEHICLE_TYPE_T34;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_MEDIUM_TANK;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/t34_full.md3");
		break;
	case VEHICLE_TYPE_BT7:
		tank->classname = "bt7tank";
		tank->s.effect1Time = VEHICLE_TYPE_BT7;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_LIGHT_TANK;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/bt7_full.md3");
		break;
	case VEHICLE_TYPE_FLAMETANK:
		tank->classname = "flametank";
		tank->s.effect1Time = VEHICLE_TYPE_FLAMETANK;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_FLAMETANK;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/t34_full.md3");
		break;
    case VEHICLE_TYPE_PANZERIV:
		tank->classname = "panzerivtank";
		tank->s.effect1Time = VEHICLE_TYPE_PANZERIV;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_MEDIUM_TANK;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/panzeriv_full.md3");
		break;
	case VEHICLE_TYPE_TIGERTANK:
		tank->classname = "tigertank";
		tank->s.effect1Time = VEHICLE_TYPE_TIGERTANK;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_HEAVY_ASSAULT_TANK;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/tiger_full.md3");
		break;
    case VEHICLE_TYPE_STUG3:
		tank->classname = "stug3tank";
		tank->s.effect1Time = VEHICLE_TYPE_STUG3;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_MEDIUM_TANK;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/stug3_full.md3");
		break;
    case VEHICLE_TYPE_FLAK88:
		tank->classname = "flak88antitank";
		tank->s.effect1Time = VEHICLE_TYPE_FLAK88;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_ARTILLERY;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/artillery/flak88_full.md3");
		break;
    case VEHICLE_TYPE_PAK:
		tank->classname = "pakantitank";
		tank->s.effect1Time = VEHICLE_TYPE_PAK;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_ARTILLERY;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/artillery/pak_full.md3");
		break;
	case VEHICLE_TYPE_AXIS_APC:
		tank->classname = "axis_apc";
		tank->s.effect1Time = VEHICLE_TYPE_AXIS_APC;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_APC;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/m3_full.md3");
		break;
	case VEHICLE_TYPE_ALLIED_APC:
		tank->classname = "allied_apc";
		tank->s.effect1Time = VEHICLE_TYPE_ALLIED_APC;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_APC;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/m3_full.md3");
		break;
	case VEHICLE_TYPE_PANZER2:
		tank->classname = "panzer2tank";
		tank->s.effect1Time = VEHICLE_TYPE_PANZER2;
		tank->s.nextWeapon = self->client->ps.persistant[PERS_TANKUSED];
		tank->s.weapon = VEHICLE_CLASS_LIGHT_TANK;
		tank->s.modelindex =		G_ModelIndex ("models/mapobjects/tanks_sd/panzer2_full.md3");
		break;
	}

	VectorCopy(tankMins, tank->r.mins);
	VectorCopy(tankMaxs, tank->r.maxs);

	tank->nextthink = level.time + 50;	// push it out a little
	tank->think = tank_think;
	tank->s.eType = ET_VEHICLE;

	tank->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	tank->r.contents =	CONTENTS_TRIGGER | CONTENTS_SOLID;

	tank->s.pos.trType = TR_LINEAR_STOP;

	tank->s.apos.trTime =		0;
	tank->s.apos.trDuration =	0;
	tank->s.apos.trType =		TR_LINEAR_STOP;

	VectorCopy (angle, tank->s.angles); 
	VectorCopy (tank->s.angles, tank->s.apos.trBase);
	VectorCopy (tank->s.angles, tank->s.apos.trDelta );

	VectorCopy (self->s.angles, tank->s.angles2);

	G_SetOrigin (tank,  origin);

	tank->active = qfalse;
	tank->health = self->health;//self->client->ps.stats[STAT_HEALTH];
	tank->maxhealth = self->maxhealth;
	//tank->s.dl_intensity = self->client->ps.stats[STAT_HEALTH];
	tank->s.dl_intensity = (int)((float)((float)tank->health/(float)tank->maxhealth)*(float)100); // send health to clients
	tank->takedamage = qtrue;
	tank->TankMarker = self->TankMarker; // mark ourself as tank, for spawning entity
	tank->die = tank_die;


	trap_LinkEntity ( tank );

	return tank;
}

void G_LeavePlayerTank( gentity_t* ent, qboolean position ) {
	gentity_t* tank;
	vec3_t axis[3];
	vec3_t pos;
	trace_t tr;
	vec3_t	angles;
	gclient_t *client;

	if (ent->client)
		client = ent->client;
#ifdef __NPC__
	else if (ent->NPC_client)
		client = ent->NPC_client;
#endif //__NPC__

	RemoveTankTurret( ent );

	VectorCopy(ent->s.apos.trBase, angles);

	angles[0] = angles[2] = 0;

	tank = tankSpawn(ent,ent->s.pos.trBase, angles);

	AddTankTurret( tank );

	tank->backupWeaponTime = client->ps.weaponTime;
	client->ps.weaponTime = ent->backupWeaponTime;

	client->ps.eFlags &= ~EF_VEHICLE;
	ent->s.eFlags &= ~EF_VEHICLE;
	tank->s.weapon = client->ps.stats[STAT_VEHICLE_CLASS];
	tank->s.nextWeapon = client->ps.persistant[PERS_TANKUSED];
	client->ps.persistant[PERS_TANKUSED] = 0;
	ent->TankMarker = 0; // we are no longer spawners tank
	ent->health = client->ps.stats[STAT_HEALTH] = ent->BackupHealth;
	tank->maxhealth = ent->maxhealth;
	tank->s.dl_intensity = (int)((float)((float)tank->health/(float)tank->maxhealth)*(float)100); // send health to clients

	if( position ) {

		AnglesToAxis( tank->s.angles, axis );

		VectorMA( client->ps.origin, 128, axis[2], pos ); // always spawn on tank
		trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

		if( tr.startsolid ) {
		VectorMA( client->ps.origin, 128, axis[0], pos ); // unless it isnt possible
		trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

		if( tr.startsolid ) {
			// try right
			VectorMA( client->ps.origin, -128, axis[1], pos );
			trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

			if( tr.startsolid ) {
				// try back
				VectorMA( client->ps.origin, -224, axis[0], pos );
				trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

				if( tr.startsolid ) {
					// try front
					VectorMA( client->ps.origin, 224, axis[0], pos );
					trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

					if( tr.startsolid ) {
						// give up
						return;
					}
				}
			}
		}
		}
		VectorClear( client->ps.velocity ); // Gordon: dont want them to fly away ;D
		TeleportPlayer( ent, pos, client->ps.viewangles );
	}
}

gentity_t *G_BotLeavePlayerTank( gentity_t* ent, qboolean position ) {
	gentity_t* tank;
	vec3_t axis[3];
	vec3_t pos;
	trace_t tr;
	vec3_t	angles;
	gclient_t *client;

	if (ent->client)
		client = ent->client;
#ifdef __NPC__
	else if (ent->NPC_client)
		client = ent->NPC_client;
#endif //__NPC__

	RemoveTankTurret( ent );

	VectorCopy(ent->s.apos.trBase, angles);

	angles[0] = angles[2] = 0;

	tank = tankSpawn(ent,ent->s.pos.trBase, angles);

	AddTankTurret( tank );

	tank->backupWeaponTime = client->ps.weaponTime;
	client->ps.weaponTime = ent->backupWeaponTime;

	client->ps.eFlags &= ~EF_VEHICLE;
	ent->s.eFlags &= ~EF_VEHICLE;
	tank->s.weapon = client->ps.stats[STAT_VEHICLE_CLASS];
	tank->s.nextWeapon = client->ps.persistant[PERS_TANKUSED];
	client->ps.persistant[PERS_TANKUSED] = 0;
	ent->TankMarker = 0; // we are no longer spawners tank
	ent->health = client->ps.stats[STAT_HEALTH] = ent->BackupHealth;
	tank->maxhealth = ent->maxhealth;
	tank->s.dl_intensity = (int)((float)((float)tank->health/(float)tank->maxhealth)*(float)100); // send health to clients

	if( position ) {

		AnglesToAxis( tank->s.angles, axis );

		VectorMA( client->ps.origin, 128, axis[2], pos ); // always spawn on tank
		trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

		if( tr.startsolid ) {
		VectorMA( client->ps.origin, 128, axis[0], pos ); // unless it isnt possible
		trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

		if( tr.startsolid ) {
			// try right
			VectorMA( client->ps.origin, -128, axis[1], pos );
			trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

			if( tr.startsolid ) {
				// try back
				VectorMA( client->ps.origin, -224, axis[0], pos );
				trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

				if( tr.startsolid ) {
					// try front
					VectorMA( client->ps.origin, 224, axis[0], pos );
					trap_Trace( &tr, pos, playerMins, playerMaxs, pos, -1, CONTENTS_SOLID );

					if( tr.startsolid ) {
						// give up
						return NULL;
					}
				}
			}
		}
		}
		VectorClear( client->ps.velocity ); // Gordon: dont want them to fly away ;D
		TeleportPlayer( ent, pos, client->ps.viewangles );
	}

	return tank;
}
#endif //__VEHICLES__

void Cmd_Activate_f( gentity_t *ent ) {
	trace_t		tr;
	vec3_t		end;
	gentity_t	*traceEnt;
	vec3_t		forward, right, up, offset;
//	int			activatetime = level.time;
	qboolean	found = qfalse;
	qboolean	pass2 = qfalse;
	qboolean	pass3 = qfalse;
	int			i;
	gclient_t *client;

	if (ent->client)
		client = ent->client;
#ifdef __NPC__
	else if (ent->NPC_client)
		client = ent->NPC_client;
#endif //__NPC__

	if( ent->health <= 0 ) {
		return;
	}

#ifdef __EF__
	if( ent->s.weapon == WP_MORTAR_SET || ent->s.weapon == WP_MOBILE_MG42_SET || ent->s.weapon == WP_30CAL_SET  || ent->s.weapon == WP_PTRS_SET ) {
#else //!__EF__
	if( ent->s.weapon == WP_MORTAR_SET || ent->s.weapon == WP_MOBILE_MG42_SET ) {
#endif //__EF__
		return;
	}

	if (ent->active) {
		if (client->ps.persistant[PERS_HWEAPON_USE]) {
			// DHM - Nerve :: Restore original position if current position is bad
			trap_Trace (&tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, ent->s.number, MASK_PLAYERSOLID);
			if ( tr.startsolid ) {
				VectorCopy( ent->TargetAngles, client->ps.origin );
				VectorCopy( ent->TargetAngles, ent->r.currentOrigin );
				ent->r.contents = CONTENTS_CORPSE;		// DHM - this will correct itself in ClientEndFrame
			}

			client->ps.eFlags &= ~EF_MG42_ACTIVE;			// DHM - Nerve :: unset flag
			client->ps.eFlags &= ~EF_AAGUN_ACTIVE;

			client->ps.persistant[PERS_HWEAPON_USE] = 0;
			ent->active = qfalse;
			
			for( i = 0; i < level.num_entities; i++ ) {
				if( g_entities[i].s.eType == ET_MG42_BARREL && g_entities[i].r.ownerNum == ent->s.number ) {
					g_entities[i].mg42weapHeat = client->ps.weapHeat[WP_DUMMY_MG42];
					g_entities[i].backupWeaponTime = client->ps.weaponTime;
					break;
				}
			}
			client->ps.weaponTime = ent->backupWeaponTime;
		} else {
			ent->active = qfalse;
		}
		return;
	} else if( client && (client->ps.eFlags & EF_MOUNTEDTANK) && (ent->s.eFlags & EF_MOUNTEDTANK) && !level.disableTankExit ) {
		G_LeaveTank( ent, qtrue );
		return;
#ifdef __VEHICLES__
	} else if (client && (client->ps.eFlags & EF_VEHICLE) && (ent->s.eFlags & EF_VEHICLE)){
		G_LeavePlayerTank( ent, qtrue );
		return;
#endif //__VEHICLES__
	}
//	@TODO Tanks! Do more for tanks!
	

	AngleVectors( client->ps.viewangles, forward, right, up);

	VectorCopy( client->ps.origin, offset );
	offset[2] += client->ps.viewheight;

	// lean
	if( client->ps.leanf ) {
		VectorMA( offset, client->ps.leanf, right, offset );
	}

	//VectorMA( offset, 256, forward, end );
	VectorMA( offset, 96, forward, end );

	trap_Trace( &tr, offset, NULL, NULL, end, ent->s.number, (CONTENTS_SOLID|CONTENTS_MISSILECLIP|CONTENTS_BODY|CONTENTS_CORPSE)  );

	if ( tr.surfaceFlags & SURF_NOIMPACT || tr.entityNum == ENTITYNUM_WORLD) {
		trap_Trace (&tr, offset, NULL, NULL, end, ent->s.number, (CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE|CONTENTS_MISSILECLIP|CONTENTS_TRIGGER));
		pass2 = qtrue;
	}

tryagain:

	if ( tr.surfaceFlags & SURF_NOIMPACT || tr.entityNum == ENTITYNUM_WORLD) {
		//G_Printf("*** 4\n");
		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	//if (traceEnt)
	//	G_Printf("Hit entity %i. Type %i.\n", tr.entityNum, traceEnt->s.eType);

	found = Do_Activate_f(ent, traceEnt);

	if(!found && !pass2) {
		pass2 = qtrue;
		trap_Trace (&tr, offset, NULL, NULL, end, ent->s.number, (CONTENTS_SOLID|CONTENTS_MISSILECLIP|CONTENTS_BODY|CONTENTS_CORPSE|CONTENTS_TRIGGER|CONTENTS_PLAYERCLIP));
		goto tryagain;
	}

	if(!found && !pass3) {
		pass3 = qtrue;
		trap_Trace (&tr, offset, NULL, NULL, end, ent->s.number, (MASK_SHOT));
		goto tryagain;
	}
}

/*
Cmd_Activate2_f() is only for:
    uniform stealing
	shove 
	dragging
	class switch
 */
void Cmd_Activate2_f( gentity_t *ent ) {
	trace_t		tr;
	vec3_t		end;
	gentity_t	*traceEnt;
	vec3_t		forward, right, up, offset;
	gclient_t *client;

	if (ent->client)
		client = ent->client;
#ifdef __NPC__
	else if (ent->NPC_client)
		client = ent->NPC_client;
#endif //__NPC__


	//if( ent->client->sess.playerType != PC_COVERTOPS ) {
	//	return;
	//}

	AngleVectors (client->ps.viewangles, forward, right, up);
	CalcMuzzlePointForActivate (ent, forward, right, up, offset);
	VectorMA (offset, 96, forward, end);



	// look for a corpse to drag
	trap_Trace(&tr, 
		offset, 
		NULL, 
		NULL, 
		end, 
		ent->s.number, 
		CONTENTS_CORPSE);
	if(tr.entityNum >= 0) {
		traceEnt = &g_entities[tr.entityNum];
		if(traceEnt->client) {
			G_DragCorpse(ent, traceEnt);
			return;
		}
	}

	// look for a guy to push
	trap_Trace(&tr, 
		offset, 
		NULL, 
		NULL, 
		end, 
		ent->s.number, 
		CONTENTS_BODY);
	if(tr.entityNum >= 0) {
		traceEnt = &g_entities[tr.entityNum];
		if(traceEnt->client) {
			if(traceEnt->client->ps.eFlags & EF_PLAYDEAD) 
				G_DragCorpse(ent, traceEnt);
			else
				G_PushPlayer(ent, traceEnt);
		return;
	}
	}

	// look for a gibbed corpse
	trap_Trace(&tr,
		offset,
		NULL,
		NULL,
		end,
		ent->s.number,
		(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE));
	if(tr.entityNum >= 0) {
		traceEnt = &g_entities[tr.entityNum];
		if(traceEnt->s.eType == ET_CORPSE && BODY_TEAM(traceEnt)) {
				if(BODY_TEAM(traceEnt) == client->sess.sessionTeam ) {
					G_ClassSteal(ent, traceEnt);
					return;
				}
				else if(client->sess.playerType == PC_COVERTOPS)  {
					G_UniformSteal(ent, traceEnt);
					return;
				}
		}
	}

	if(g_dropObj.integer &&
		client->pmext.objDrops <= g_dropObj.integer &&
		(client->ps.powerups[PW_REDFLAG] ||
		 client->ps.powerups[PW_BLUEFLAG])) {

		if(client->ps.weapon != WP_KNIFE) {
			CP("cp \"You must switch to knife to drop objective\"");
		}
		else {
			G_DropItems(ent);
			client->pmext.objDrops++;
	}
	}
	
	// forty - canister kicking
	if(g_canisterKick.integer)
		G_CanisterKick(ent);
}

/*
============================
Cmd_ClientMonsterSlickAngle 
============================
*/
/*
void Cmd_ClientMonsterSlickAngle (gentity_t *clent) {

	char s[MAX_STRING_CHARS];
	int	entnum;
	int angle;
	gentity_t *ent;
	vec3_t	dir, kvel;
	vec3_t	forward;

	if (trap_Argc() != 3) {
		G_Printf( "ClientDamage command issued with incorrect number of args\n" );
	}

	trap_Argv( 1, s, sizeof( s ) );
	entnum = atoi(s);
	ent = &g_entities[entnum];

	trap_Argv( 2, s, sizeof( s ) );
	angle = atoi(s);

	// sanity check (also protect from cheaters)
	if (g_gametype.integer != GT_SINGLE_PLAYER && entnum != clent->s.number) {
		trap_DropClient( clent->s.number, "Dropped due to illegal ClientMonsterSlick command\n" );
		return;
	}

	VectorClear (dir);
	dir[YAW] = angle;
	AngleVectors (dir, forward, NULL, NULL);
	
	VectorScale (forward, 32, kvel);
	VectorAdd (ent->client->ps.velocity, kvel, ent->client->ps.velocity);
}
*/

void G_UpdateSpawnCounts( void ) {
	int		i, j;
	char	cs[MAX_STRING_CHARS];
	int		current, count, team;

	for( i = 0; i < level.numspawntargets; i++ ) {
		trap_GetConfigstring( CS_MULTI_SPAWNTARGETS + i, cs, sizeof(cs) );

		current = atoi(Info_ValueForKey( cs, "c" ));
		team = atoi(Info_ValueForKey( cs, "t" )) & ~256;

		count = 0;
		for( j = 0; j < level.numConnectedClients; j++ ) {
			gclient_t* client = &level.clients[ level.sortedClients[ j ] ];

			if( client->sess.sessionTeam != TEAM_AXIS && client->sess.sessionTeam != TEAM_ALLIES ) {
				continue;
			}

			if( client->sess.sessionTeam == team && client->sess.spawnObjectiveIndex == i + 1) {
				count++;
				continue;
			}

			if( client->sess.spawnObjectiveIndex == 0 ) {
				if( client->sess.sessionTeam == TEAM_AXIS ) {
					if( level.axisAutoSpawn == i ) {
						count++;
						continue;
					}
				} else {
					if( level.alliesAutoSpawn == i ) {
						count++;
						continue;
					}
				}
			}
		}

		if(count == current) {
			continue;
		}

		Info_SetValueForKey( cs, "c", va("%i", count));
		trap_SetConfigstring( CS_MULTI_SPAWNTARGETS + i, cs );
	}
}

/*
============
Cmd_SetSpawnPoint_f
============
*/
void SetPlayerSpawn( gentity_t* ent, int spawn, qboolean update ) {
	ent->client->sess.spawnObjectiveIndex = spawn;
	if( ent->client->sess.spawnObjectiveIndex >= MAX_MULTI_SPAWNTARGETS || ent->client->sess.spawnObjectiveIndex < 0 ) {
		ent->client->sess.spawnObjectiveIndex = 0;
	}

	if( update ) {
		G_UpdateSpawnCounts();
	}
}

void Cmd_SetSpawnPoint_f( gentity_t* ent ) {
	char arg[MAX_TOKEN_CHARS];
	int val, i;

	if ( trap_Argc() != 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	val = atoi( arg );

	if( ent->client ) {
		SetPlayerSpawn( ent, val, qtrue);
	}

//	if( ent->client->sess.sessionTeam != TEAM_SPECTATOR && !(ent->client->ps.pm_flags & PMF_LIMBO) ) {
//		return;
//	}

	for( i = 0; i < level.numLimboCams; i++ ) {
		int x = (g_entities[level.limboCams[i].targetEnt].count - CS_MULTI_SPAWNTARGETS) + 1;
		if( level.limboCams[i].spawn && x == val ) {
			VectorCopy( level.limboCams[i].origin, ent->s.origin2 );
			ent->r.svFlags |= SVF_SELF_PORTAL_EXCLUSIVE;
			trap_SendServerCommand( ent-g_entities, va( "portalcampos %i %i %i %i %i %i %i %i", val-1, (int)level.limboCams[i].origin[0], (int)level.limboCams[i].origin[1], (int)level.limboCams[i].origin[2], (int)level.limboCams[i].angles[0], (int)level.limboCams[i].angles[1], (int)level.limboCams[i].angles[2], level.limboCams[i].hasEnt ? level.limboCams[i].targetEnt : -1) );
			break;
		}
	}
}

/*
============
Cmd_SetSniperSpot_f
============
*/
void Cmd_SetSniperSpot_f( gentity_t *clent ) {
	gentity_t *spot;

	vmCvar_t	cvar_mapname;
	char		filename[MAX_QPATH];
	fileHandle_t f;
	char		buf[1024];

	if (!g_cheats.integer) return;
	if (!trap_Cvar_VariableIntegerValue("cl_running")) return;	// only allow locally playing client
	if (clent->s.number != 0) return;	// only allow locally playing client

	// drop a sniper spot here
	spot = G_Spawn();
	spot->classname = "bot_sniper_spot";
	VectorCopy( clent->r.currentOrigin, spot->s.origin );
	VectorCopy( clent->client->ps.viewangles, spot->s.angles );
	spot->aiTeam = clent->client->sess.sessionTeam;

	// output to text file
	trap_Cvar_Register( &cvar_mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );

	Com_sprintf( filename, sizeof(filename), "maps/%s.botents", cvar_mapname.string );
	if (trap_FS_FOpenFile( filename, &f, FS_APPEND ) < 0) {
		G_Error("Cmd_SetSniperSpot_f: cannot open %s for writing", filename );
	}

	Com_sprintf( buf, sizeof(buf), "{\n\"classname\" \"%s\"\n\"origin\" \"%.3f %.3f %.3f\"\n\"angles\" \"%.2f %.2f %.2f\"\n\"aiTeam\" \"%i\"\n}\n\n", spot->classname, spot->s.origin[0], spot->s.origin[1], spot->s.origin[2], spot->s.angles[0], spot->s.angles[1], spot->s.angles[2], spot->aiTeam );
	trap_FS_Write( buf, strlen(buf), f );

	trap_FS_FCloseFile( f );

	G_Printf( "dropped sniper spot\n" );

	return;
}

void G_PrintAccuracyLog( gentity_t *ent );

/*
============
Cmd_SetWayPoint_f
============
*/
/*void Cmd_SetWayPoint_f( gentity_t *ent ) {
	char	arg[MAX_TOKEN_CHARS];
	vec3_t	forward, muzzlePoint, end, loc;
	trace_t	trace;

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Not allowed to set waypoints as spectator.\n\"" );
		return;
	}

	if ( trap_Argc() != 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );

	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );

	VectorCopy( ent->r.currentOrigin, muzzlePoint );
	muzzlePoint[2] += ent->client->ps.viewheight;

	VectorMA( muzzlePoint, 8192, forward, end );
	trap_Trace( &trace, muzzlePoint, NULL, NULL, end, ent->s.number, MASK_SHOT );

	if( trace.surfaceFlags & SURF_NOIMPACT )
		return;

	VectorCopy( trace.endpos, loc );

	G_SetWayPoint( ent, atoi(arg), loc );
}*/

/*
============
Cmd_ClearWayPoint_f
============
*/
/*void Cmd_ClearWayPoint_f( gentity_t *ent ) {

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Not allowed to clear waypoints as spectator.\n\"" );
		return;
	}

	G_RemoveWayPoint( ent->client );
}*/

void Cmd_WeaponStat_f ( gentity_t* ent ) {
	char buffer[16];
	extWeaponStats_t stat;

	if(!ent || !ent->client) {
		return;
	}

	if( trap_Argc() != 2 ) {
		return;
	}
	trap_Argv( 1, buffer, 16 );
	stat = atoi(buffer);

	trap_SendServerCommand( ent-g_entities, va( "rws %i %i", ent->client->sess.aWeaponStats[stat].atts, ent->client->sess.aWeaponStats[stat].hits ) );
}

void Cmd_IntermissionWeaponStats_f ( gentity_t* ent ) {
	char buffer[1024];
	int i, clientNum;

	if( !ent || !ent->client ) {
		return;
	}

	trap_Argv( 1, buffer, sizeof( buffer ) );

	clientNum = atoi( buffer );
	if( clientNum < 0 || clientNum > MAX_CLIENTS ) {
		return;
	}

	Q_strncpyz( buffer, "imws ", sizeof( buffer ) );
	for( i = 0; i < WS_MAX; i++ ) {
		Q_strcat( buffer, sizeof( buffer ), va( "%i %i %i ", level.clients[clientNum].sess.aWeaponStats[i].atts, level.clients[clientNum].sess.aWeaponStats[i].hits, level.clients[clientNum].sess.aWeaponStats[i].kills ) );
	}

	trap_SendServerCommand( ent-g_entities, buffer );
}

void G_MakeReady( gentity_t* ent ) {
	ent->client->ps.eFlags |= EF_READY;
	ent->s.eFlags |= EF_READY;
	// rain - #105 - moved this set here
	ent->client->pers.ready = qtrue;
}

void G_MakeUnready( gentity_t* ent ) {
	ent->client->ps.eFlags &= ~EF_READY;
	ent->s.eFlags &= ~EF_READY;
	// rain - #105 - moved this set here
	ent->client->pers.ready = qfalse;
}

void Cmd_IntermissionReady_f ( gentity_t* ent ) {
	if( !ent || !ent->client ) {
		return;
	}

	G_MakeReady( ent );
}

void Cmd_IntermissionPlayerKillsDeaths_f ( gentity_t* ent ) {
	char buffer[1024];
	int i;

	if( !ent || !ent->client ) {
		return;
	}

	Q_strncpyz( buffer, "impkd ", sizeof( buffer ) );
	for( i = 0; i < MAX_CLIENTS; i++ ) {
		if( g_entities[i].inuse ) {
			Q_strcat( buffer, sizeof( buffer ), va( "%i %i ", level.clients[i].sess.kills, level.clients[i].sess.deaths ) );
		} else {
			Q_strcat( buffer, sizeof( buffer ), "0 0 " );
		}
	}

	trap_SendServerCommand( ent-g_entities, buffer );
}

void G_CalcClientAccuracies( void ) {
	int i, j;
	int shots, hits;

	for( i = 0; i < MAX_CLIENTS; i++ ) {
		shots = 0;
		hits = 0;

		if( g_entities[i].inuse ) {
			for( j = 0; j < WS_MAX; j++ ) {
				shots += level.clients[i].sess.aWeaponStats[j].atts;
				hits += level.clients[i].sess.aWeaponStats[j].hits;
			}

			level.clients[ i ].acc = shots ? (100 * hits) / (float)shots : 0;
		} else {
			level.clients[ i ].acc = 0;
		}
	}	
}


void Cmd_IntermissionWeaponAccuracies_f ( gentity_t* ent ) {
	char buffer[1024];
	int i;

	if( !ent || !ent->client ) {
		return;
	}

	G_CalcClientAccuracies();

	Q_strncpyz( buffer, "imwa ", sizeof( buffer ) );
	for( i = 0; i < MAX_CLIENTS; i++ ) {
		Q_strcat( buffer, sizeof( buffer ), va( "%i ", (int)level.clients[ i ].acc ) );
	}

	trap_SendServerCommand( ent-g_entities, buffer );
}

void Cmd_SelectedObjective_f ( gentity_t* ent ) {
	int i, val;
	char buffer[16];
	vec_t dist, neardist = 0;
	int nearest = -1;


	if(!ent || !ent->client) {
		return;
	}

//	if( ent->client->sess.sessionTeam != TEAM_SPECTATOR && !(ent->client->ps.pm_flags & PMF_LIMBO) ) {
//		return;
//	}

	if( trap_Argc() != 2 ) {
		return;
	}
	trap_Argv( 1, buffer, 16 );
	val = atoi(buffer) + 1;


	for( i = 0; i < level.numLimboCams; i++ ) {
		if( !level.limboCams[i].spawn && level.limboCams[i].info == val ) {			
			if( !level.limboCams[i].hasEnt ) {
				VectorCopy( level.limboCams[i].origin, ent->s.origin2 );
				ent->r.svFlags |= SVF_SELF_PORTAL_EXCLUSIVE;
				trap_SendServerCommand( ent-g_entities, va( "portalcampos %i %i %i %i %i %i %i %i", val-1, (int)level.limboCams[i].origin[0], (int)level.limboCams[i].origin[1], (int)level.limboCams[i].origin[2], (int)level.limboCams[i].angles[0], (int)level.limboCams[i].angles[1], (int)level.limboCams[i].angles[2], level.limboCams[i].hasEnt ? level.limboCams[i].targetEnt : -1) );

				break;
			} else {
				dist = VectorDistanceSquared( level.limboCams[i].origin, g_entities[level.limboCams[i].targetEnt].r.currentOrigin );
				if( nearest == -1 || dist < neardist ) {
					nearest = i;
					neardist = dist;
				}
			}
		}
	}

	if( nearest != -1 ) {
		i = nearest;

		VectorCopy( level.limboCams[i].origin, ent->s.origin2 );
		ent->r.svFlags |= SVF_SELF_PORTAL_EXCLUSIVE;
		trap_SendServerCommand( ent-g_entities, va( "portalcampos %i %i %i %i %i %i %i %i", val-1, (int)level.limboCams[i].origin[0], (int)level.limboCams[i].origin[1], (int)level.limboCams[i].origin[2], (int)level.limboCams[i].angles[0], (int)level.limboCams[i].angles[1], (int)level.limboCams[i].angles[2], level.limboCams[i].hasEnt ? level.limboCams[i].targetEnt : -1) );
	}
}

void Cmd_Ignore_f (gentity_t* ent ) {
	char	cmd[MAX_TOKEN_CHARS];
	int pids[MAX_CLIENTS];
	char err[MAX_STRING_CHARS];
	gentity_t *victim;

	trap_Argv( 1, cmd, sizeof( cmd ) );		

	if(!*cmd) {
		CP("print \"usage: ignore [name|slot#]\n\"");
		return;
	}

	if(ClientNumbersFromString(cmd, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		CP(va("print \"ignore: %s\"", err));
		return;
	}

	if(pids[0] == (ent-g_entities)) {
		CP("print \"ignore: you can't ignore yourself\n\"");
		return;
	}

	victim = &g_entities[pids[0]];

	if(COM_BitCheck(ent->client->sess.ignoreClients, pids[0])) {
		CP(va("print \"ignore: you are already ignoring %s\n\"",
			victim->client->pers.netname));
		return;
	}

	COM_BitSet(ent->client->sess.ignoreClients, pids[0]);
	CP(va("print \"ignore: %s^7 added to your ignore list\n\"",
		victim->client->pers.netname));
	CPx(pids[0], va("chat \"%s^1 is now ignoring you\"",
		ent->client->pers.netname));

}

void Cmd_TicketTape_f ( void ) {
/*	char	cmd[MAX_TOKEN_CHARS];

	trap_Argv( 1, cmd, sizeof( cmd ) );

	trap_SendServerCommand( -1, va( "tt \"LANDMINES SPOTTED <STOP> CHECK COMMAND MAP FOR DETAILS <STOP>\"\n", cmd ));*/
}

void Cmd_UnIgnore_f (gentity_t* ent ) {
	char	cmd[MAX_TOKEN_CHARS];
	int pids[MAX_CLIENTS];
	int i;
	gentity_t *victim;
	int clientNum;
	int matches;

	trap_Argv( 1, cmd, sizeof( cmd ) );		

	if(!*cmd) {
		CP("print \"usage: unignore [name|slot#]\n\"");
		if(!ent->client->sess.ignoreClients[0] &&
			!ent->client->sess.ignoreClients[1]) {

			return;
		}	
		CP("print \"currently ignoring:\n\"");
		for(i=0; i<MAX_CLIENTS; i++) {
			if(COM_BitCheck(ent->client->sess.ignoreClients, i)) {
				victim = &g_entities[i];
				if(!victim || !victim->client)
					continue;
				CP(va("print \" %2d: %s\n\"",
					i,
					victim->client->pers.netname));
			}
		}
		return;
	}

	matches = ClientNumbersFromString(cmd, pids);
	for(i=0; i<matches; i++) {
		if(!COM_BitCheck(ent->client->sess.ignoreClients, pids[i])) {
			matches--;
		}
		else {
			clientNum = pids[i];
		}
	}
	if(matches != 1) {
		CP("print \"unignore: no match found.  "
			"run /unignore with no parameter for a list\n\"");
		return;
	}
	victim = &g_entities[clientNum];

	COM_BitClear(ent->client->sess.ignoreClients, clientNum);
	CP(va("print \"unignore: %s^7 removed from your ignore list\n\"",
		victim->client->pers.netname));
	CPx(clientNum, va("chat \"%s^1 has stopped ignoring you\"",
		ent->client->pers.netname));
}

/*
=================
Cmd_SwapPlacesWithBot_f
=================
*/
void Cmd_SwapPlacesWithBot_f( gentity_t *ent, int botNum ) {
	gentity_t *botent;
	gclient_t cl, *client;
	clientPersistant_t	saved;
	clientSession_t sess;
	int		persistant[MAX_PERSISTANT];
	//
	client = ent->client;
	//
	botent = &g_entities[botNum];
	if (!botent->client) return;
	// if this bot is dead
	if (botent->health <= 0 && (botent->client->ps.pm_flags & PMF_LIMBO)) {
		trap_SendServerCommand( ent-g_entities, "print \"Bot is in limbo mode, cannot swap places.\n\"" );
		return;
	}
	//
	if (client->sess.sessionTeam != botent->client->sess.sessionTeam) {
		trap_SendServerCommand( ent-g_entities, "print \"Bot is on different team, cannot swap places.\n\"" );
		return;
	}
	//
	// copy the client information
	cl = *botent->client;
	//
	G_DPrintf( "Swapping places: %s in for %s\n", ent->client->pers.netname, botent->client->pers.netname);
	// kill the bot
	botent->flags &= ~FL_GODMODE;
	botent->client->ps.stats[STAT_HEALTH] = botent->health = 0;
	player_die (botent, ent, ent, 0, MOD_SWAP_PLACES);// jaquboss,  as damage we goes throught stats :S
	// make sure they go into limbo mode right away, and dont show a corpse
	limbo( botent, qfalse, qfalse );
	// respawn the player
	ent->client->ps.pm_flags &= ~PMF_LIMBO; // JPW NERVE turns off limbo
	// copy the location
	VectorCopy( cl.ps.origin, ent->s.origin );
	VectorCopy( cl.ps.viewangles, ent->s.angles );
	// copy session data, so we spawn in as the same class
	// save items
	saved = client->pers;
	sess = client->sess;
	memcpy( persistant, ent->client->ps.persistant, sizeof(persistant) );
	// give them the right weapons/etc
	*client = cl;
	client->sess = sess;
	client->sess.playerType /*= ent->client->sess.latchPlayerType */= cl.sess.playerType; // jaquboss DO NOT use this for latch player sess, players would like to spawn as they pevious class
	client->sess.playerWeapon /*= ent->client->sess.latchPlayerWeapon */= cl.sess.playerWeapon;
	client->sess.playerWeapon2 /*= ent->client->sess.latchPlayerWeapon2 */= cl.sess.playerWeapon2;
	// spawn them in
	ClientSpawn(ent, qtrue, qfalse, qtrue);
	// restore items
	client->pers = saved;
	memcpy( ent->client->ps.persistant, persistant, sizeof(persistant) );
	client->ps = cl.ps;
	client->ps.clientNum = ent->s.number;
	ent->health = client->ps.stats[STAT_HEALTH];
	SetClientViewAngle( ent, cl.ps.viewangles );
	// make sure they dont respawn immediately after they die
	client->pers.lastReinforceTime = 0;
	ClientUserinfoChanged( ent-g_entities ); // jaquboss, this fixes some bugs found on clients , eg keeping old class look and so..
}

vec3_t mg42_spawn_position;
extern void SP_mg42 (gentity_t *self);

qboolean G_CanSetupMobileMG42Here ( gentity_t *ent )
{// UQ1: Testing to see if we can mount mg42 on objects while standing...
	trace_t				trace;
	vec3_t				angles, forward, muzzlePoint, start, end;
	int					i;

	for (i = MAX_CLIENTS; i < MAX_GENTITIES; i++)
	{// First see if another is too close!
		gentity_t *t = &g_entities[i];

		if (!t)
			continue;

		if (VectorDistance(t->r.currentOrigin, ent->client->ps.origin) < 128)
		{
			if (!Q_stricmp( t->classname, "misc_mg42base" ))
				return qfalse;

			if (!Q_stricmp( t->classname, "misc_mg42" ))
				return qfalse;

			if (t->s.eType == ET_MG42_BARREL)
				return qfalse;
		}
	}

	VectorCopy(ent->client->ps.viewangles, angles);
	angles[PITCH] = 0;

	AngleVectors( angles, forward, NULL, NULL );

	VectorCopy( ent->client->ps.origin, muzzlePoint );
	//muzzlePoint[2] += pm->ps->viewheight;
	//if( ent->client->ps.eFlags & EF_CROUCHING )
	//	muzzlePoint[2] += 10;
	//else
		muzzlePoint[2] += 24;

	VectorMA( muzzlePoint, 24, forward, start );

	VectorCopy(start, end);
	end[2]-=64;

	trap_Trace( &trace, start, NULL, NULL, end, ent->client->ps.clientNum, MASK_SHOT );

	if (trace.allsolid)
		return qfalse;

	if (trace.startsolid)
		return qfalse;

	if (trace.fraction == 1)
		return qfalse;

	if (VectorDistance(trace.endpos, start) < 16)
	{
		VectorCopy(trace.endpos, mg42_spawn_position);
		return qtrue;
	}

	return qfalse;
}

#ifdef __ETE__
extern void UQ_SetGlobalFog( qboolean restore, int duration, vec3_t color, float depthForOpaque );
#endif //__ETE__

/*
=================
ClientCommand
=================
*/
void ClientCommand( int clientNum ) {
	gentity_t *ent;
	char	cmd[MAX_TOKEN_CHARS];

	ent = g_entities + clientNum;
	if ( !ent->client ) {
		return;		// not fully in game yet
	}

	trap_Argv( 0, cmd, sizeof( cmd ) );

	if (Q_stricmp (cmd, "say") == 0) {
		if( !ent->client->sess.muted) {
			Cmd_Say_f (ent, SAY_ALL, qfalse);
		}
		return;
	}

	if (!Q_stricmp (cmd, "m")) {
		G_PrivateMessage(ent);
		return;
	}

	if (!Q_stricmp (cmd, "damage")) {
		G_TeamDamageStats(ent);
		return;
	}

	if (g_killRating.integer & KILL_RATING_VISIBLE && 
			!Q_stricmp (cmd, "killrating")) {
		G_KillRatingStats(ent);
		return;
	}

	if (g_playerRating.integer & PLAYER_RATING_VISIBLE && 
			!Q_stricmp (cmd, "playerrating")) {
		G_PlayerRatingStats(ent);
		return;
	}

	if( Q_stricmp (cmd, "say_team") == 0 ) {
		if( ent->client->sess.sessionTeam == TEAM_SPECTATOR || ent->client->sess.sessionTeam == TEAM_FREE ) {
			trap_SendServerCommand( ent-g_entities, "print \"Can't team chat as spectator\n\"\n" );
			return;
		}

		if( !ent->client->sess.muted ) {
			Cmd_Say_f (ent, SAY_TEAM, qfalse);
		}
		return;
	} else if (Q_stricmp (cmd, "vsay") == 0) {
		if( !ent->client->sess.muted) {
			Cmd_Voice_f (ent, SAY_ALL, qfalse, qfalse);
		}
		return;
	} else if (Q_stricmp (cmd, "vsay_team") == 0) {
		if( ent->client->sess.sessionTeam == TEAM_SPECTATOR || ent->client->sess.sessionTeam == TEAM_FREE ) {
			trap_SendServerCommand( ent-g_entities, "print \"Can't team chat as spectator\n\"\n" );
			return;
		}

		if( !ent->client->sess.muted) {
			Cmd_Voice_f (ent, SAY_TEAM, qfalse, qfalse);
		}
		return;
	} else if (Q_stricmp (cmd, "say_buddy") == 0) {
		if( !ent->client->sess.muted) {
			Cmd_Say_f( ent, SAY_BUDDY, qfalse );
		}
		return;
	} else if (Q_stricmp (cmd, "vsay_buddy") == 0) {
		if( !ent->client->sess.muted) {
			Cmd_Voice_f( ent, SAY_BUDDY, qfalse, qfalse );
		}
		return;
	} else if (Q_stricmp (cmd, "score") == 0) {
		Cmd_Score_f (ent);
		return;
	} else if( Q_stricmp (cmd, "vote") == 0 ) {
		Cmd_Vote_f (ent);
		return;
	} else if (Q_stricmp (cmd, "fireteam") == 0) {
		Cmd_FireTeam_MP_f (ent);
		return;
	} else if (Q_stricmp (cmd, "showstats") == 0) {
		G_PrintAccuracyLog( ent );
		return;
	} else if (Q_stricmp (cmd, "rconAuth") == 0) {
		Cmd_AuthRcon_f( ent );
		return;
	} else if (Q_stricmp (cmd, "ignore") == 0) {
		Cmd_Ignore_f( ent );
		return;
	} else if (Q_stricmp (cmd, "unignore") == 0) {
		Cmd_UnIgnore_f( ent );
		return;
	} else if (Q_stricmp (cmd, "obj") == 0) {
		Cmd_SelectedObjective_f( ent );
		return;
	} else if( !Q_stricmp( cmd, "impkd" ) ) {
		Cmd_IntermissionPlayerKillsDeaths_f( ent );
		return;
	} else if( !Q_stricmp( cmd, "imwa" ) ) {
		Cmd_IntermissionWeaponAccuracies_f( ent );
		return;
	} else if( !Q_stricmp( cmd, "imws" ) ) {		
		Cmd_IntermissionWeaponStats_f( ent );
		return;
	} else if( !Q_stricmp( cmd, "imready" ) ) {		
		Cmd_IntermissionReady_f( ent );
		return;
	} else if (Q_stricmp (cmd, "ws") == 0) {
		Cmd_WeaponStat_f( ent );
		return;
/*#ifdef _DEBUG
#ifdef __SHELLSHOCK__
	} else if (Q_stricmp (cmd, "shellshock") == 0) {
		G_DebugShellShock(ent);
		return;
#endif //__SHELLSHOCK__
#endif //_DEBUG*/
	} else if( !Q_stricmp( cmd, "forcetapout" ) ) {
		if( !ent || !ent->client ) {
			return;
		}

		if( ent->health <= 0 && ( ent->client->sess.sessionTeam == TEAM_AXIS || ent->client->sess.sessionTeam == TEAM_ALLIES ) ) {
			limbo( ent, qtrue, qfalse );
		}

		return;
	}

	// OSP
	// Do these outside as we don't want to advertise it in the help screen
	if(!Q_stricmp(cmd, "wstats")) {
		G_statsPrint(ent, 1);
		return;
	}
	if(!Q_stricmp(cmd, "sgstats")) {	// Player game stats
		G_statsPrint(ent, 2);
		return;
	}
	if(!Q_stricmp(cmd, "stshots")) {	// "Topshots" accuracy rankings
		G_weaponStatsLeaders_cmd(ent, qtrue, qtrue);
		return;
	}
	if( !Q_stricmp( cmd, "rs" ) ) {
		Cmd_ResetSetup_f( ent );
		return;
	}

	if(G_commandCheck(ent, cmd, qtrue)) return;
	// OSP

	if(G_shrubbot_permission(ent, SBF_STEALTH)) 
		if(G_shrubbot_cmd_check(ent)) 
			return;

	// ignore all other commands when at intermission
	if (level.intermissiontime) {
//		Cmd_Say_f (ent, qfalse, qtrue);			// NERVE - SMF - we don't want to spam the clients with this.
		CPx(clientNum, va("print \"^3%s^7 not allowed during intermission.\n\"", cmd));
		return;
	}

	if (Q_stricmp (cmd, "give") == 0) {
		Cmd_Give_f (ent);
	} else if (Q_stricmp (cmd, "listbotgoals") == 0) {
		Cmd_ListBotGoals_f(ent);
	} else if (Q_stricmp (cmd, "god") == 0) {
		Cmd_God_f (ent);
	} else if (Q_stricmp (cmd, "nofatigue") == 0) {
		Cmd_Nofatigue_f (ent);
	} else if (Q_stricmp (cmd, "notarget") == 0) {
		Cmd_Notarget_f (ent);
	} else if (Q_stricmp (cmd, "noclip") == 0) {
		Cmd_Noclip_f (ent);
	} else if (Q_stricmp (cmd, "kill") == 0) {
		Cmd_Kill_f (ent);
#ifdef __ETE__
	} else if (Q_stricmp (cmd, "!fog") == 0 && (ent->client->sess.referee || ent->client->pers.localClient)) {
		qboolean restore = qfalse;
		int duration = 10000;
		vec3_t color;
		float depthForOpaque;
		char token[MAX_TOKEN_CHARS];

		trap_Argv(1, token, sizeof(token));

		if (!token || !token[0])
		{
			G_Printf("Usage: /!fog <r> <g> <b> <depth>\n");
			return;
		}
		color[0] = atof(token);

		trap_Argv(2, token, sizeof(token));
		if (!token || !token[0])
		{
			G_Printf("Usage: /!fog <r> <g> <b> <depth>\n");
			return;
		}
		color[1] = atof(token);

		trap_Argv(3, token, sizeof(token));
		if (!token || !token[0])
		{
			G_Printf("Usage: /!fog <r> <g> <b> <depth>\n");
			return;
		}
		color[2] = atof(token);

		trap_Argv(4, token, sizeof(token));
		if (!token || !token[0])
		{
			G_Printf("Usage: /!fog <r> <g> <b> <depth>\n");
			return;
		}
		depthForOpaque = atof(token);

		VectorCopy(color, fog_color);
		UQ_SetGlobalFog( restore, duration, color, depthForOpaque );

		current_fog_view_distance = depthForOpaque;
		maximum_fog_view_distance = depthForOpaque*1.5;
		minimum_fog_view_distance = depthForOpaque*0.5;

		is_fog_kludge_map = qtrue;
		fog_kludge_checked = qtrue;
#endif //__ETE__
#ifdef __VEHICLES__
	} else if (Q_stricmp (cmd, "addtank") == 0 && (ent->client->sess.referee || ent->client->pers.localClient)) {
		char type[MAX_TOKEN_CHARS];

		trap_Argv(1, type, sizeof(type));

		if (!Q_stricmp(type,"churchhill"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_tank (self);
		}
		else if (!Q_stricmp(type,"panzer"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_panzer (self);
		}
	    else if (!Q_stricmp(type,"panzeriv"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_panzeriv (self);
		}
	    else if (!Q_stricmp(type,"kv1"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_kv1 (self);
		}
	    else if (!Q_stricmp(type,"stug3"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_stug3 (self);
		}
        else if (!Q_stricmp(type,"t34"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_t34 (self);
		}
		else if (!Q_stricmp(type,"bt7"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_bt7 (self);
		}
		else if (!Q_stricmp(type,"flametank"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_flametank (self);
		}
		else if (!Q_stricmp(type,"tiger"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_tigertank (self);
		}
		else if (!Q_stricmp(type,"axis_apc"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_axis_apc (self);
		}
		else if (!Q_stricmp(type,"allied_apc"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_allied_apc (self);
		}
		else if (!Q_stricmp(type,"panzer2"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_panzer2 (self);
		}
		else
		{// UQ1: Converted to send over net...
			trap_SendServerCommand( ent-g_entities, va("print \"^5Use: ^7/addtank ^5<^7tankname^5>\n\"") );
			trap_SendServerCommand( ent-g_entities, va("print \"Current valid tank types are ^3churchhill^5, ^3panzer^5, ^3panzeriv^5, ^3kv1^5, ^3stug3^5, ^3tiger^5, ^3bt7^5, ^3axis_apc^5, ^3allied_apc^5, ^3flametank^5,  ^3panzer2^5 and ^3t34^5.\n\"") );
		}
	} else if (Q_stricmp (cmd, "addantitank") == 0 
		&& (ent->client->sess.referee || ent->client->pers.localClient)) { //Masteries, addantitank flak88, addantitank pak  Fully interactive Artillery
		char type[MAX_TOKEN_CHARS];              //Artillery is special kind of tank without forward/backward movements and without machinegun

		trap_Argv(1, type, sizeof(type));

        if (!Q_stricmp(type,"flak88"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_flak88 (self);
		}
		else if (!Q_stricmp(type,"pak"))
		{
			gentity_t *self = G_Spawn();
			G_SetOrigin(self, ent->r.currentOrigin);
			G_SetAngle(self, ent->r.currentAngles);
			VectorCopy(ent->r.currentOrigin, self->s.origin);
			VectorCopy(ent->r.currentAngles, self->s.angles);
			SP_pak (self);
		}
        else
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^5Use: ^7/addantitank ^5<^7artillery name^5>\n\"") );
			trap_SendServerCommand( ent-g_entities, va("print \"Current valid artillery types are ^3flak88^5, and ^3pak^5.\n\"") );
		}
#endif //__VEHICLES__
	} else if (Q_stricmp (cmd, "viewang") == 0 || Q_stricmp (cmd, "viewangles") == 0) {
		G_Printf("%f %f %f.\n", ent->client->ps.viewangles[0], ent->client->ps.viewangles[1], ent->client->ps.viewangles[2]);
	} else if (Q_stricmp (cmd, "follownext") == 0) {
		Cmd_FollowCycle_f (ent, 1);
	} else if (Q_stricmp (cmd, "followprev") == 0) {
		Cmd_FollowCycle_f (ent, -1);
	} else if (Q_stricmp (cmd, "where") == 0) {
		Cmd_Where_f (ent);
//	} else if (Q_stricmp (cmd, "startCamera") == 0) {
//		Cmd_StartCamera_f( ent );
	} else if (Q_stricmp (cmd, "stopCamera") == 0) {
		Cmd_StopCamera_f( ent );
	} else if (Q_stricmp (cmd, "setCameraOrigin") == 0) {
		Cmd_SetCameraOrigin_f( ent );
	} else if (Q_stricmp (cmd, "cameraInterrupt") == 0) {
		Cmd_InterruptCamera_f( ent );
	} else if (Q_stricmp (cmd, "setviewpos") == 0) {
		Cmd_SetViewpos_f( ent );
	} else if (Q_stricmp (cmd, "setspawnpt") == 0) {
		Cmd_SetSpawnPoint_f( ent );
	} else if (Q_stricmp (cmd, "setsniperspot") == 0) {
		Cmd_SetSniperSpot_f( ent );
//	} else if (Q_stricmp (cmd, "waypoint") == 0) {
//		Cmd_SetWayPoint_f( ent );
//	} else if (Q_stricmp (cmd, "clearwaypoint") == 0) {
//		Cmd_ClearWayPoint_f( ent );
	// OSP
#ifdef __WEAPON_AIM__
#ifndef __FRONTLINE__
	} else if (Q_stricmp (cmd, "aim") == 0) {
		if ( BG_Weapon_Is_Aimable(ent->client->ps.weapon) )
		{
			if (ent->client->ps.eFlags & EF_AIMING)
			{// Turn off aiming...
				ent->client->ps.eFlags &= ~EF_AIMING;
			}
			else
			{// Turn on aiming...
				ent->client->ps.eFlags |= EF_AIMING;
			}
		}
#endif
#endif //__WEAPON_AIM__
	} else if (Q_stricmp (cmd, "order") == 0) {
		char	order[MAX_TOKEN_CHARS];
		char	value1[MAX_TOKEN_CHARS];
		char	value2[MAX_TOKEN_CHARS];
		char	value3[MAX_TOKEN_CHARS];
		int		order_type;

		trap_Argv(1, order, sizeof(order));
		order_type = atoi(order);

		if (order_type == FT_ORDER_COVER)
		{
			vec3_t	order_position;

			trap_Argv(2, value1, sizeof(value1));
			trap_Argv(3, value2, sizeof(value2));
			trap_Argv(4, value3, sizeof(value3));

			order_position[0] = atof(value1);
			order_position[1] = atof(value2);
			order_position[2] = atof(value3);

			AIMOD_MOVEMENT_FireTeam_AI_Issue_Order( ent, FT_ORDER_COVER, NULL, order_position );
			G_Voice( ent, NULL, SAY_TEAM, va("HoldFire"), qfalse );
		}
		else if (order_type == FT_ORDER_RETURN)
		{
			G_Voice( ent, NULL, SAY_TEAM, va("NeedBackup"), qfalse );
			//G_Say( ent, NULL, SAY_TEAM, va("FollowMe") );
			AIMOD_MOVEMENT_FireTeam_AI_Issue_Order( ent, FT_ORDER_RETURN, NULL, NULL );
		}
		else if (order_type == FT_ORDER_TOGGLE_GROUP)
		{
			//G_Voice( ent, NULL, SAY_TEAM, va("NeedBackup"), qfalse );
			AIMOD_MOVEMENT_FireTeam_AI_Issue_Order( ent, FT_ORDER_TOGGLE_GROUP, NULL, NULL );
		}
		else
		{
			int			order_enemy;
			gentity_t	*enemy = NULL;

			trap_Argv(2, value1, sizeof(value1));

			order_enemy = atoi(value1);
			enemy = &g_entities[order_enemy];

			if (enemy && enemy->client)
			{
				if (order_type == FT_ORDER_SUPRESS)
				{
					if (Q_TrueRand(0,1) == 1)
						G_Voice( ent, NULL, SAY_TEAM, va("CoverMe"), qfalse );
					else
						G_Voice( ent, NULL, SAY_TEAM, va("FTCoverMe"), qfalse );
					
					AIMOD_MOVEMENT_FireTeam_AI_Issue_Order( ent, FT_ORDER_SUPRESS, enemy, enemy->r.currentOrigin );
				}
				else
				{
					G_Voice( ent, NULL, SAY_TEAM, va("ClearPath"), qfalse );
					AIMOD_MOVEMENT_FireTeam_AI_Issue_Order( ent, FT_ORDER_ASSAULT, enemy, enemy->r.currentOrigin );
				}
			}
		}
	} else if (Q_stricmp (cmd, "setupmg42") == 0) {
		gentity_t *mg42 = G_Spawn();

		if (G_CanSetupMobileMG42Here(ent))
		{
			vec3_t forward;
			
			mg42->player_mg42 = qtrue;
			mg42_spawn_position[2]=ent->r.currentOrigin[2]+2/*+8*//*-2*/;
			VectorCopy(mg42_spawn_position, mg42->s.origin);
			mg42->s.density = 0;
			SP_mg42 (mg42);
			
			AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
			VectorMA( ent->r.currentOrigin, -32, forward, ent->r.currentOrigin );
			VectorMA( ent->r.currentOrigin, -32, forward, ent->client->ps.origin );
			VectorCopy(ent->client->ps.viewangles, mg42->s.angles);
			mg42->s.angles[2] = 0;
			G_SetAngle(mg42, mg42->s.angles);

			COM_BitClear( ent->client->ps.weapons, WP_MOBILE_MG42 );
			ent->client->ps.ammo[BG_FindClipForWeapon(WP_MOBILE_MG42)] = 0;
			ent->client->ps.ammoclip[BG_FindClipForWeapon(WP_MOBILE_MG42)] = 0;

			COM_BitClear( ent->client->ps.weapons, WP_MOBILE_MG42_SET );
			ent->client->ps.ammo[BG_FindClipForWeapon(WP_MOBILE_MG42_SET)] = 0;
			ent->client->ps.ammoclip[BG_FindClipForWeapon(WP_MOBILE_MG42_SET)] = 0;

			ent->client->ps.weapon = ent->client->sess.playerWeapon2;
		}
	} else if (Q_stricmp (cmd, "setupbrowning") == 0) {
		gentity_t *mg42 = G_Spawn();

		if (G_CanSetupMobileMG42Here(ent))
		{
			vec3_t forward;
			
			mg42->player_mg42 = qtrue;
			mg42_spawn_position[2]=ent->r.currentOrigin[2]+2/*+8*//*-2*/;
			VectorCopy(mg42_spawn_position, mg42->s.origin);
			mg42->s.density = 1;
			SP_mg42 (mg42);
			
			AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
			VectorMA( ent->r.currentOrigin, -32, forward, ent->r.currentOrigin );
			VectorMA( ent->r.currentOrigin, -32, forward, ent->client->ps.origin );
			VectorCopy(ent->client->ps.viewangles, mg42->s.angles);
			mg42->s.angles[2] = 0;
			G_SetAngle(mg42, mg42->s.angles);

			COM_BitClear( ent->client->ps.weapons, WP_30CAL );
			ent->client->ps.ammo[BG_FindClipForWeapon(WP_30CAL)] = 0;
			ent->client->ps.ammoclip[BG_FindClipForWeapon(WP_30CAL)] = 0;

			COM_BitClear( ent->client->ps.weapons, WP_30CAL_SET );
			ent->client->ps.ammo[BG_FindClipForWeapon(WP_30CAL_SET)] = 0;
			ent->client->ps.ammoclip[BG_FindClipForWeapon(WP_30CAL_SET)] = 0;

			ent->client->ps.weapon = ent->client->sess.playerWeapon2;
		}
#ifdef __SCREW_THIS__
// UQ1: Bah! Can't be botherred with this mounted PTRS right now... Too much work...
	} else if (Q_stricmp (cmd, "setupptrs") == 0 && (ent->client->sess.referee || ent->client->pers.localClient)) {
		gentity_t *mg42 = G_Spawn();

		if (G_CanSetupMobileMG42Here(ent))
		{
			vec3_t forward;
			
			mg42->player_mg42 = qtrue;
			mg42_spawn_position[2]=ent->r.currentOrigin[2]+8;
			VectorCopy(mg42_spawn_position, mg42->s.origin);
			mg42->s.density = 2;
			SP_mg42 (mg42);
			
			AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
			VectorMA( ent->r.currentOrigin, -32, forward, ent->r.currentOrigin );
			VectorMA( ent->r.currentOrigin, -32, forward, ent->client->ps.origin );
			VectorCopy(ent->client->ps.viewangles, mg42->s.angles);
			mg42->s.angles[2] = 0;
			G_SetAngle(mg42, mg42->s.angles);

			COM_BitClear( ent->client->ps.weapons, WP_PTRS );
			ent->client->ps.ammo[BG_FindClipForWeapon(WP_PTRS)] = 0;
			ent->client->ps.ammoclip[BG_FindClipForWeapon(WP_PTRS)] = 0;

			COM_BitClear( ent->client->ps.weapons, WP_PTRS_SET );
			ent->client->ps.ammo[BG_FindClipForWeapon(WP_PTRS_SET)] = 0;
			ent->client->ps.ammoclip[BG_FindClipForWeapon(WP_PTRS_SET)] = 0;

			ent->client->ps.weapon = ent->client->sess.playerWeapon2;
		}
#endif //__SCREW_THIS__
	} else if (Q_stricmp (cmd, "me") == 0) {
		// UQ1: IRC style actions...
		char *action_text;

		if (!ent->client)
			return;

		if(trap_Argc() < 2) return;

		action_text = ConcatArgs(1);

		trap_SendServerCommand( -1, va("cpm \"^3*^5 %s ^5%s\n\"", ent->client->pers.netname, action_text) );
#ifdef __BOT__
/*	} else if (Q_stricmp (cmd, "becomeabot") == 0 && (ent->client->sess.referee || ent->client->pers.localClient))	{
		if (ent->r.svFlags & SVF_BOT)
		{// Switch off bot mode...
			ent->r.svFlags &= ~SVF_BOT;
			ent->think = NULL;
			ent->nextthink = 0;
			G_Printf("Bot mode emulation switched OFF!\n");
		}
		else
		{// Switch on bot mode...
			ent->r.svFlags |= SVF_BOT;
			ent->think = SOD_AI_BotThink; // I can think, I'm alive !
			ent->nextthink = level.time + 50;
			G_Printf("Bot mode emulation switched ON!\n");
		}*/
	} else if (Q_stricmp (cmd, "checkbotreach") == 0 && (ent->client->sess.referee || ent->client->pers.localClient))	{
		AIMod_CheckMapPaths ( ent );
#ifdef __OLD_COVER_SPOTS__
	} else if (Q_stricmp (cmd, "gotocp") == 0)	{
		char node[MAX_TOKEN_CHARS];
		int	 nodeNum;

		trap_Argv(1, node, sizeof(node));
		nodeNum = atoi(node);

		/*G_SetOrigin(ent, nodes[nodeNum].origin);
		VectorCopy(nodes[nodeNum].origin, ent->client->ps.origin);*/
		AIMOD_SP_Debug_GotoCP ( ent, nodeNum );
#endif //__OLD_COVER_SPOTS__
	} else if (Q_stricmp (cmd, "gotonode") == 0 && (ent->client->sess.referee || ent->client->pers.localClient))	{
		char node[MAX_TOKEN_CHARS];
		int	 nodeNum;

		trap_Argv(1, node, sizeof(node));
		nodeNum = atoi(node);

		G_SetOrigin(ent, nodes[nodeNum].origin);
		VectorCopy(nodes[nodeNum].origin, ent->client->ps.origin);
	} else if (Q_stricmp (cmd, "gotosniperspot") == 0 && (ent->client->sess.referee || ent->client->pers.localClient))	{
		char node[MAX_TOKEN_CHARS];
		int	 nodeNum = 0;
		int	 i = 0;
		int	 count = 0;

		trap_Argv(1, node, sizeof(node));
		nodeNum = atoi(node);

		for (i = 0; i < MAX_GENTITIES; i++)
		{
			gentity_t *spot = &g_entities[i];

			if (!spot)
				continue;

			if (!Q_stricmp( spot->classname, va( "bot_sniper_spot") ) )
			{
				count++;

				G_Printf("Spot %i is at %f %f %f.\n", count, spot->s.origin[0], spot->s.origin[1], spot->s.origin[2]);

				if (count == nodeNum)
				{
					G_SetOrigin(ent, spot->s.origin);
					VectorCopy(spot->s.origin, ent->client->ps.origin);

					G_Printf("Moving to this spot.\n");
					break;
				}
			}
		}
	} else if (Q_stricmp (cmd, "checknodelinks") == 0 && (ent->client->sess.referee || ent->client->pers.localClient))	{
		AIMod_ShowNodeLinks ( ent );
	} else if (Q_stricmp (cmd, "addbot") == 0 && (ent->client->sess.referee || ent->client->pers.localClient))	{		// Removed obsolete addbot functionality !
		char team[MAX_TOKEN_CHARS];
		char name[MAX_TOKEN_CHARS];
		char skill[MAX_TOKEN_CHARS];

		int	 iskill;

		trap_Argv(1, team, sizeof(team));
		trap_Argv(2, name, sizeof(name));
		trap_Argv(3, skill, sizeof(skill));

		if (!Q_stricmp(skill,""))
			Q_strncpyz(skill, "0", sizeof(skill));

		if (!Q_stricmp(name,""))
		{
			if (team == "red")
				Q_strncpyz(name, va("%s", PickName(TEAM_AXIS)), sizeof(name));
			else
				Q_strncpyz(name, va("%s", PickName(TEAM_ALLIES)), sizeof(name));
		}


		iskill = atoi(skill);

		if (SOD_SPAWN_AddBotShort(team, name, iskill, NULL))
			G_Printf(S_COLOR_YELLOW "Spawned bot %s with skill level %d.\n", name, iskill);
	}
	else if (Q_stricmp (cmd, "removeteamnodes") == 0 && bot_node_edit.integer > 0) 
	{
		AIMOD_NODES_RemoveTeamNodes();
	}
	else if (Q_stricmp (cmd, "addnode") == 0 && bot_node_edit.integer > 0) 
	{
		int teamNum = -1;
		char team[MAX_TOKEN_CHARS];

		trap_Argv(1, team, sizeof(team));

		if (!Q_stricmp(team,"axis"))
			teamNum = TEAM_AXIS;
		else if (!Q_stricmp(team,"allies"))
			teamNum = TEAM_ALLIES;
		else if (!Q_stricmp(team,"vehicle"))
			teamNum = 99; // For ground vehicles only!

		AIMOD_MAPPING_AddNode(ent->s.number, teamNum);
	}
	else if (Q_stricmp (cmd, "relocatenode") == 0 && bot_node_edit.integer > 0) {

		AIMOD_MAPPING_SetRelocateNode();
	}
	else if (Q_stricmp (cmd, "setnodetype") == 0 && bot_node_edit.integer > 0) {
		char strNodeType[MAX_TOKEN_CHARS];
		int node_type;

		trap_Argv(1, strNodeType, sizeof(strNodeType));

		if (!Q_stricmp(strNodeType,""))
			Q_strncpyz(strNodeType, "0", sizeof(strNodeType));

		node_type = atoi(strNodeType);

		AIMOD_MAPPING_SetNodeType(-1, node_type);
	}

	else if (Q_stricmp (cmd, "botlist") == 0 && (ent->client->sess.referee || ent->client->pers.localClient)) {
		AIMOD_SPAWN_BotList();
	}

	else if (Q_stricmp (cmd, "removebot") == 0 && (ent->client->sess.referee || ent->client->pers.localClient)) {
		char botNum[MAX_TOKEN_CHARS];
		int  clientNum;

		trap_Argv(1, botNum, sizeof(botNum));

		if (!Q_stricmp(botNum,"")) {
			G_Printf("usage: removebot <botnum>\n");
		} else {

			clientNum = atoi(botNum);
			AIMOD_SPAWN_RemoveBot(clientNum);
		}
	}
	else if ((Q_stricmp (cmd, "generatecover") == 0 || Q_stricmp (cmd, "generatecoverspots") == 0) && bot_node_edit.integer) {
		AIMOD_Generate_Cover_Spots_Real();
	}
	else if (Q_stricmp (cmd, "savenodes") == 0 && bot_node_edit.integer > 0) {
		AIMOD_NODES_SaveNodes();
	}
	else if (Q_stricmp (cmd, "!loadnodes") == 0 && ent->client->pers.localClient) {
		int i = 0;

		for (i = 0; i < MAX_GENTITIES; i++)
			g_entities[i].pathsize = 0;

		AIMOD_NODES_LoadNodes();
	}
	else if ((Q_stricmp (cmd, "findnewroute") == 0 || Q_stricmp (cmd, "findnewpath") == 0 || Q_stricmp (cmd, "findpath") == 0 )
		&& bot_node_edit.integer)	
	{
		AIMod_CreateNewRoute ( ent );
	}
	else if (Q_stricmp (cmd, "showvisinfo") == 0 && bot_node_edit.integer)	
	{
		int wp1 = 0, wp2 = 0;

		if (trap_Argc() > 1)
		{
			char	arg[MAX_STRING_CHARS];
			char	arg2[MAX_STRING_CHARS];

			trap_Argv( 1, arg, sizeof( arg ) );
			trap_Argv( 2, arg2, sizeof( arg2 ) );

			if (arg[0])
			{
				wp1 = atoi(arg);
			}

			if (arg[1])
			{
				wp2 = atoi(arg2);
			}

			ShowWaypointVisibilityInfo( wp1, wp2 );
		}
	}
	else if (Q_stricmp (cmd, "raisenode") == 0 && bot_node_edit.integer > 0)
	{// Raise a node to force it to be a jump node.
		char nodeNum[MAX_TOKEN_CHARS];
		int  node;

		trap_Argv(1, nodeNum, sizeof(nodeNum));

		node = atoi(nodeNum);

		nodes[node].origin[2]+=32;

		G_Printf("^1*** ^3%s^5: ^5Node ^7%i^5 raised to force jump.\n", GAME_VERSION, node);
	}
	else if (Q_stricmp (cmd, "movenode") == 0 && bot_node_edit.integer > 0)
	{// Move a node to current player position.
		char nodeNum[MAX_TOKEN_CHARS];
		int  node;

		trap_Argv(1, nodeNum, sizeof(nodeNum));

		node = atoi(nodeNum);

		VectorCopy(ent->r.currentOrigin, nodes[node].origin);

		G_Printf("^1*** ^3%s^5: ^5Node ^7%i^5 moved to current player position.\n", GAME_VERSION, node);
	}
	else if (Q_stricmp (cmd, "deletenode") == 0 && bot_node_edit.integer > 0)
	{// Delete the last added node.
		int j = 0;

		VectorClear(nodes[number_of_nodes].origin);
		nodes[number_of_nodes].enodenum = -1;
		nodes[number_of_nodes].type = -1;

		for (j = 0; j < MAX_NODELINKS; j++)
		{
			nodes[number_of_nodes].links[j].targetNode = -1;
			nodes[number_of_nodes].links[j].cost = 99999;
		}

		G_Printf("^1*** ^3%s: ^5Node ^7%i^5 deleted.\n", GAME_VERSION, number_of_nodes);

		number_of_nodes--;
#endif //__BOT__
#ifdef __ETE__
	} else if(!Q_stricmp (cmd, "listsecondaries") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		G_ListSecondaryEntities();
	} else if(!Q_stricmp (cmd, "mgnest") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		ent->action = AIRDROP_TYPE_MG_NEST;
		Create_Constructible_Airdrop(ent);
	} else if(!Q_stricmp (cmd, "sandbags") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		ent->action = AIRDROP_TYPE_SANDBAGS;
		Create_Constructible_Airdrop(ent);
	} else if(!Q_stricmp (cmd, "supremacy_savegameinfo") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		SUPREMACY_SaveGameInfo();
	} else if(!Q_stricmp (cmd, "supremacy_loadgameinfo") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		SUPREMACY_LoadGameInfo();
	} else if(!Q_stricmp (cmd, "supremacy_addhealthcrate") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		SUPREMACY_AddHealthCrate( ent->r.currentOrigin, ent->r.currentAngles );
	} else if(!Q_stricmp (cmd, "supremacy_addammocrate") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		SUPREMACY_AddAmmoCrate( ent->r.currentOrigin, ent->r.currentAngles );
#ifdef __VEHICLES__
	} else if(!Q_stricmp (cmd, "supremacy_addvehicle") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		int  vehicleType;
		char str[MAX_TOKEN_CHARS];

		trap_Argv(1, str, sizeof(str));

		if (!Q_stricmp(str,"heavy")) 
		{
			vehicleType = VEHICLE_CLASS_HEAVY_TANK;
		} 
		else if (!Q_stricmp(str,"medium")) 
		{
			vehicleType = VEHICLE_CLASS_MEDIUM_TANK;
		} 
		else if (!Q_stricmp(str,"flame")) 
		{
			vehicleType = VEHICLE_CLASS_FLAMETANK;
		} 
		else 
		{
			vehicleType = VEHICLE_CLASS_LIGHT_TANK;
		}

		SUPREMACY_AddVehicle( ent->r.currentOrigin, ent->r.currentAngles, vehicleType );
#endif //__VEHICLES__
	} else if(!Q_stricmp (cmd, "supremacy_addflag") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		int  team;
		char str[MAX_TOKEN_CHARS];

		trap_Argv(1, str, sizeof(str));

		if (!Q_stricmp(str,"axis")) 
		{
			team = TEAM_AXIS;
		} 
		else if (!Q_stricmp(str,"allies")) 
		{
			team = TEAM_ALLIES;
		} 
		else 
		{
			team = TEAM_FREE;
		}

		SUPREMACY_AddFlag( ent->r.currentOrigin, team );
	} else if(!Q_stricmp (cmd, "supremacy_changeflagteam") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		int  team = 0, flag_number = 0;
		char str[MAX_TOKEN_CHARS];

		trap_Argv(1, str, sizeof(str));

		if (!str || !str[0])
		{
			G_Printf("Format: /supremacy_changeflagteam <flag #> <team name>\n");
			SUPREMACY_ShowInfo();
			return;
		}

		flag_number = atoi(str);

		trap_Argv(2, str, sizeof(str));

		if (!Q_stricmp(str,"axis")) 
		{
			team = TEAM_AXIS;
		} 
		else if (!Q_stricmp(str,"allies")) 
		{
			team = TEAM_ALLIES;
		} 
		else 
		{
			team = TEAM_FREE;
		}
		
		SUPREMACY_ChangeFlagTeam( flag_number, team );
	} else if(!Q_stricmp (cmd, "supremacy_raiseents") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		SUPREMACY_RaiseAllSupremacyEnts( -1 );
	} else if(!Q_stricmp (cmd, "supremacy_removeents") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		SUPREMACY_RemoveAllSupremacyEnts();
	} else if(!Q_stricmp (cmd, "supremacy_removeammocrate") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		SUPREMACY_RemoveAmmoCrate();
	} else if(!Q_stricmp (cmd, "supremacy_removehealthcrate") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		SUPREMACY_RemoveHealthCrate();
	} else if(!Q_stricmp (cmd, "supremacy_showinfo") && (ent->client->sess.referee || ent->client->pers.localClient)) {
		SUPREMACY_ShowInfo();
#endif //__ETE__
	} else if(G_commandCheck(ent, cmd, qfalse)) {
		return;
	}
	else if (!Q_stricmp (cmd, "playdead")) {
		G_PlayDead(ent);
	}
	else if (!Q_stricmp (cmd, "throwknife")) {
		// forty - checks moved into G_throwKnife in g_weapon.c
		G_throwKnife(ent);
	}
	else if (!Q_stricmp (cmd, "knives")) {
		CPx(clientNum, va("print \"%d knives left\n\"",
			ent->client->ps.ammoclip[BG_FindClipForWeapon(WP_KNIFE)]));
	}
	else {
		trap_SendServerCommand( clientNum, va("print \"unknown cmd[lof] %s\n\"", cmd ) );
	}
}


/*
==================
ConcatArgs
==================
*/
char	*ConcatArgs( int start ) {
	int		i, c, tlen;
	static char	line[MAX_STRING_CHARS];
	int		len;
	char	arg[MAX_STRING_CHARS];

	len = 0;
	c = trap_Argc();
	for ( i = start ; i < c ; i++ ) {
		trap_Argv( i, arg, sizeof( arg ) );
		tlen = strlen( arg );
		if ( len + tlen >= MAX_STRING_CHARS - 1 ) {
			break;
		}
		memcpy( line + len, arg, tlen );
		len += tlen;
		if ( i != c - 1 ) {
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}


char *Q_SayConcatArgs(int start) {
	char *s;
	int c = 0;

	s = ConcatArgs(0);
	while(*s) {
		if(c == start) return s;
		if(*s == ' ') {
			*s++;
			if(*s != ' ') {
				c++;
				continue;
			}
			while(*s && *s == ' ') *s++;  
			c++;
		}
		*s++;
	}
	return s;
}


// replaces all occurances of "\n" with '\n'
char *Q_AddCR(char *s)
{
	char *copy, *place, *start;

	if(!*s) return s;
	start = s;
	while(*s) {
		if(*s == '\\') {
			copy = s;
			place = s;
			*s++;
			if(*s && *s == 'n') {
				*copy = '\n';
				while(*++s) {
					*++copy = *s;
				}
				*++copy = '\0';
				s = place;
				continue;
			}
		}
		*s++;
	}
	return start;
}

// A replacement for trap_Argc() that can correctly handle
//   say "!cmd arg0 arg1"
// as well as
//   say !cmd arg0 arg1
// The client uses the latter for messages typed in the console
// and the former when the message is typed in the chat popup
int Q_SayArgc() 
{
	int c = 1;
	char *s;

	s = ConcatArgs(0);
	if(!*s) return 0;
	while(*s) {
		if(*s == ' ') {
			*s++;
			if(*s != ' ') {
				c++;
				continue;
			}
			while(*s && *s == ' ') *s++;  
			c++;
		}
		*s++;
	}
	return c;
}

// A replacement for trap_Argv() that can correctly handle
//   say "!cmd arg0 arg1"
// as well as
//   say !cmd arg0 arg1
// The client uses the latter for messages typed in the console
// and the former when the message is typed in the chat popup
qboolean Q_SayArgv(int n, char *buffer, int bufferLength)
{
	int bc = 1;
	int c = 0;
	char *s;

	if(bufferLength < 1) return qfalse;
	if(n < 0) return qfalse;
	*buffer = '\0';
	s = ConcatArgs(0);
	while(*s) {
		if(c == n) {
			while(*s && (bc < bufferLength)) {
				if(*s == ' ') {
					*buffer = '\0';
					return qtrue;
				}
				*buffer = *s;
				*buffer++;
				*s++;
				bc++;
			}
			*buffer = '\0';
			return qtrue;
		}
		if(*s == ' ') {
			*s++;
			if(*s != ' ') {
				c++;
				continue;
			}
			while(*s && *s == ' ') *s++;  
			c++;
		}
		*s++;
	}
	return qfalse;
}

