/*
 * g_shrubbot.c
 *
 * This code is the original work of Tony J. White
 *
 * The functionality of this code mimics the behaviour of the currently
 * inactive project shrubmod (http://www.etstats.com/shrubet/index.php?ver=2)
 * by Ryan Mannion.   However, shrubmod was a closed-source project and 
 * none of it's code has been copied, only it's functionality.
 *
 * You are free to use this implementation of shrubbot in you're 
 * own mod project if you wish.
 *
 */

#include "g_local.h"
#include "../../etmain/ui/menudef.h"

static const struct g_shrubbot_cmd g_shrubbot_cmds[] = {
	{"readconfig",	G_shrubbot_readconfig,	'G', 0},
	{"time",	G_shrubbot_time,	'C', 0},
	{"setlevel",	G_shrubbot_setlevel,	's', 0},
	{"kick",	G_shrubbot_kick,	'k', 0},
	{"ban",		G_shrubbot_ban,		'b', 0},
	{"unban",	G_shrubbot_unban,	'b', 0},
	{"putteam",	G_shrubbot_putteam,	'p', 0},
	{"pause",	G_shrubbot_pause,	'Z', 0},
	{"unpause",	G_shrubbot_unpause,	'Z', 0},
	{"listplayers",	G_shrubbot_listplayers,	'i', 0},
	{"listteams",	G_shrubbot_listteams,	'I', 0},
	{"mute",	G_shrubbot_mute,	'm', 0},
	{"unmute",	G_shrubbot_mute,	'm', 0},
	{"showbans",	G_shrubbot_showbans,	'B', 0},
	{"help",	G_shrubbot_help,	'h', 0},
	{"admintest",	G_shrubbot_admintest,	'a', 0},
	{"cancelvote",	G_shrubbot_cancelvote,	'c', 0},
	{"passvote",	G_shrubbot_passvote,	'V', 0},
	{"spec999",	G_shrubbot_spec999,	'P', 0},
	{"shuffle",	G_shrubbot_shuffle,	'S', 0},
	{"rename",	G_shrubbot_rename,	'N', SCMDF_TYRANNY},
	{"gib",		G_shrubbot_gib,		'g', SCMDF_TYRANNY},
	{"slap",	G_shrubbot_slap,	'A', SCMDF_TYRANNY},
	{"burn",	G_shrubbot_burn,	'U', SCMDF_TYRANNY},
	{"warn",	G_shrubbot_warn,	'R', 0},
	{"news",	G_shrubbot_news,	'W', 0},
	{"lock",	G_shrubbot_lock,	'K', 0},
	{"unlock",	G_shrubbot_unlock,	'K', 0},
	{"lol",		G_shrubbot_lol,		'x', SCMDF_TYRANNY},
	{"pip",		G_shrubbot_pip,		'z', SCMDF_TYRANNY},
	{"pop",		G_shrubbot_pop,		'z', SCMDF_TYRANNY},
	{"restart",	G_shrubbot_reset,	'r', 0},
	{"reset",	G_shrubbot_reset,	'r', 0},
	{"swap",	G_shrubbot_swap,	'w', 0},
	{"fling",	G_shrubbot_fling,	'l', SCMDF_TYRANNY},
	{"throw",	G_shrubbot_fling,	'l', SCMDF_TYRANNY},
	{"launch",	G_shrubbot_fling,	'l', SCMDF_TYRANNY},
	{"flinga",	G_shrubbot_fling,	'L', SCMDF_TYRANNY},
	{"throwa",	G_shrubbot_fling,	'L', SCMDF_TYRANNY},
	{"launcha",	G_shrubbot_fling,	'L', SCMDF_TYRANNY},
	{"disorient",	G_shrubbot_disorient,	'd', SCMDF_TYRANNY},
	{"orient",	G_shrubbot_orient,	'd', SCMDF_TYRANNY},
	{"resetxp",	G_shrubbot_resetxp,	'X', SCMDF_TYRANNY},
	{"nextmap",	G_shrubbot_nextmap,	'n', 0},
	{"resetmyxp",	G_shrubbot_resetmyxp,	'M', 0},
	{"", NULL, '\0', 0}
};

g_shrubbot_level_t *g_shrubbot_levels[MAX_SHRUBBOT_LEVELS];
g_shrubbot_admin_t *g_shrubbot_admins[MAX_SHRUBBOT_ADMINS];
g_shrubbot_ban_t *g_shrubbot_bans[MAX_SHRUBBOT_BANS];

qboolean G_shrubbot_permission(gentity_t *ent, char flag) 
{
	int i;
	int l = 0;
	char *guid;
	char userinfo[MAX_INFO_STRING];
	char *flags;

	// console always wins
	if(!ent)
		return qtrue;
	
	trap_GetUserinfo(ent-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(guid, g_shrubbot_admins[i]->guid)) {
			flags = g_shrubbot_admins[i]->flags;
			while(*flags) {
				if(*flags == flag)
					return qtrue;
				else if(*flags == '-') {
					while(*flags++) {
						if(*flags == flag) 
							return qfalse;
						else if(*flags == '+')
							break;
					}
				}
				else if(*flags == '*') {
					while(*flags++) {
						if(*flags == flag)
							return qfalse;
					}
					return qtrue;
				}
				*flags++;
			}	
			l = g_shrubbot_admins[i]->level;
		}
	}
	for(i=0; g_shrubbot_levels[i]; i++) {
		if(g_shrubbot_levels[i]->level == l) {
			flags = g_shrubbot_levels[i]->flags;
			while(*flags) {
				if(*flags == flag)
					return qtrue;
				if(*flags == '*') {
					while(*flags++) {
						if(*flags == flag)
							return qfalse;
					}
					return qtrue;
				}
				*flags++;
			}
		}
	}
	return qfalse;
}

qboolean _shrubbot_admin_higher(gentity_t *admin, gentity_t *victim) 
{
	int i;
	int alevel = 0;
	char *guid;
	char userinfo[MAX_INFO_STRING];

	// console always wins
	if(!admin) return qtrue;
	// just in case
	if(!victim) return qtrue;
	
	trap_GetUserinfo(admin-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(guid, g_shrubbot_admins[i]->guid)) {
			alevel = g_shrubbot_admins[i]->level;
		}
	}
	trap_GetUserinfo(victim-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(guid, g_shrubbot_admins[i]->guid)) {
			if(alevel < g_shrubbot_admins[i]->level)
				return qfalse;
		}
	}
	return qtrue;
}

void G_shrubbot_writeconfig_string(char *s, fileHandle_t f) 
{
	char buf[MAX_STRING_CHARS];

	buf[0] = '\0';
	if(s[0]) {
		//Q_strcat(buf, sizeof(buf), s);
		Q_strncpyz(buf, s, sizeof(buf));
		trap_FS_Write(buf, strlen(buf), f);
	}
	trap_FS_Write("\n", 1, f);
}

void G_shrubbot_writeconfig_int(int v, fileHandle_t f) 
{
	char buf[32];

	sprintf(buf, "%d", v);
	if(buf[0]) trap_FS_Write(buf, strlen(buf), f);
	trap_FS_Write("\n", 1, f);
}

void G_shrubbot_writeconfig_float(float v, fileHandle_t f) 
{
	char buf[32];

	sprintf(buf, "%f", v);
	if(buf[0]) trap_FS_Write(buf, strlen(buf), f);
	trap_FS_Write("\n", 1, f);
}

void _shrubbot_writeconfig() 
{
	fileHandle_t f;
	int len, i;
	time_t t;

	if(!g_shrubbot.string[0]) return;
	time(&t);
	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	len = trap_FS_FOpenFile(g_shrubbot.string, &f, FS_WRITE);
	if(len < 0) {
		G_Printf("_shrubbot_writeconfig: could not open %s\n",
				g_shrubbot.string);
	}
	for(i=0; g_shrubbot_levels[i]; i++) {
		trap_FS_Write("[level]\n", 8, f);
		trap_FS_Write("level   = ", 10, f);
		G_shrubbot_writeconfig_int(g_shrubbot_levels[i]->level, f);
		trap_FS_Write("name    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_levels[i]->name, f);
		trap_FS_Write("flags   = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_levels[i]->flags, f);
		trap_FS_Write("\n", 1, f);
	}
	for(i=0; g_shrubbot_admins[i]; i++) {
		// don't write level 0 users
		if(g_shrubbot_admins[i]->level < 1) continue;

		trap_FS_Write("[admin]\n", 8, f);
		trap_FS_Write("name    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_admins[i]->name, f);
		trap_FS_Write("guid    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_admins[i]->guid, f);
		trap_FS_Write("level   = ", 10, f);
		G_shrubbot_writeconfig_int(g_shrubbot_admins[i]->level, f);
		trap_FS_Write("flags   = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_admins[i]->flags, f);
		trap_FS_Write("\n", 1, f);
	}
	for(i=0; g_shrubbot_bans[i]; i++) {
		// don't write expired bans
		// if expires is 0, then it's a perm ban
		if(g_shrubbot_bans[i]->expires != 0 &&
			(g_shrubbot_bans[i]->expires - t) < 1) continue;

		trap_FS_Write("[ban]\n", 6, f);
		trap_FS_Write("name    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->name, f);
		trap_FS_Write("guid    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->guid, f);
		trap_FS_Write("ip      = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->ip, f);
		trap_FS_Write("reason  = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->reason, f);
		trap_FS_Write("made    = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->made, f);
		trap_FS_Write("expires = ", 10, f);
		G_shrubbot_writeconfig_int(g_shrubbot_bans[i]->expires, f);
		trap_FS_Write("banner  = ", 10, f);
		G_shrubbot_writeconfig_string(g_shrubbot_bans[i]->banner, f);
		trap_FS_Write("\n", 1, f);
	}
	trap_FS_FCloseFile(f);
}

void G_shrubbot_readconfig_string(char **cnf, char *s, int size) 
{
	char *t;

	//COM_MatchToken(cnf, "=");
	t = COM_ParseExt(cnf, qfalse);
	if(!strcmp(t, "=")) {
		t = COM_ParseExt(cnf, qfalse);
	}
	else {
		G_Printf("readconfig: warning missing = before "
			"\"%s\" on line %d\n", 
			t, 
			COM_GetCurrentParseLine());
	}
	s[0] = '\0';
	while(t[0]) {
		if((s[0] == '\0' && strlen(t) <= size) ||
			(strlen(t)+strlen(s) < size)) {

			Q_strcat(s, size, t);
			Q_strcat(s, size, " ");
		}
		t = COM_ParseExt(cnf, qfalse);
	}
	// trim the trailing space
	if(strlen(s) > 0 && s[strlen(s)-1] == ' ') 
		s[strlen(s)-1] = '\0';
}

void G_shrubbot_readconfig_int(char **cnf, int *v) 
{
	char *t;

	//COM_MatchToken(cnf, "=");
	t = COM_ParseExt(cnf, qfalse);
	if(!strcmp(t, "=")) {
		t = COM_ParseExt(cnf, qfalse);
	}
	else {
		G_Printf("readconfig: warning missing = before "
			"\"%s\" on line %d\n", 
			t, 
			COM_GetCurrentParseLine());
	}
	*v = atoi(t);
}

void G_shrubbot_readconfig_float(char **cnf, float *v) 
{
	char *t;

	//COM_MatchToken(cnf, "=");
	t = COM_ParseExt(cnf, qfalse);
	if(!strcmp(t, "=")) {
		t = COM_ParseExt(cnf, qfalse);
	}
	else {
		G_Printf("readconfig: warning missing = before "
			"\"%s\" on line %d\n", 
			t, 
			COM_GetCurrentParseLine());
	}
	*v = atof(t);
}


/*
  if we can't parse any levels from readconfig, set up default
  ones to make new installs easier for admins
*/
void _shrubbot_default_levels() {
	g_shrubbot_level_t *l;
	int i;

	for(i=0; g_shrubbot_levels[i]; i++) {
		free(g_shrubbot_levels[i]);
		g_shrubbot_levels[i] = NULL;
	}
	for(i=0; i<=5; i++) {
		l = malloc(sizeof(g_shrubbot_level_t));
		l->level = i;
		*l->name = '\0';
		*l->flags = '\0';
		g_shrubbot_levels[i] = l;
	}
	Q_strcat(g_shrubbot_levels[0]->flags, 5, "iahCp");
	Q_strcat(g_shrubbot_levels[1]->flags, 5, "iahCp");
	Q_strcat(g_shrubbot_levels[2]->flags, 6, "iahCpP");
	Q_strcat(g_shrubbot_levels[3]->flags, 9, "i1ahCpPkm");
	Q_strcat(g_shrubbot_levels[4]->flags, 11, "i1ahCpPkmBb");
	Q_strcat(g_shrubbot_levels[5]->flags, 12, "i1ahCpPkmBbs");
}

/*
	return a level for a player entity.
*/
int _shrubbot_level(gentity_t *ent) 
{
	int i;
	char *guid;
	qboolean found = qfalse;
	char userinfo[MAX_INFO_STRING];

	if(!ent) {	
		// forty - we are on the console, return something high for now.
		return MAX_SHRUBBOT_LEVELS;
	}

	trap_GetUserinfo(ent-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(g_shrubbot_admins[i]->guid, guid)) {
			found = qtrue;
			break;	
		}
	}

	if(found) {
		return g_shrubbot_admins[i]->level;
	}

	return 0;
}

void _shrubbot_log(gentity_t *admin, char *cmd, int skiparg)
{
	fileHandle_t f;
	int len, i, j;
	char string[MAX_STRING_CHARS];
	int	min, tens, sec;
	char userinfo[MAX_INFO_STRING];
	char *vguid = NULL, *aguid = NULL;
	g_shrubbot_admin_t *a;
	g_shrubbot_level_t *l;
	char flags[MAX_SHRUBBOT_FLAGS*2];
	gentity_t *victim = NULL;
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH];

	if(!g_logAdmin.string[0]) return;


	len = trap_FS_FOpenFile(g_logAdmin.string, &f, FS_APPEND);
	if(len < 0) {
		G_Printf("_shrubbot_log: error could not open %s\n",
			g_logAdmin.string);
		return;
	}

	sec = level.time / 1000;
	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	*flags = '\0';
	if(admin) {
		trap_GetUserinfo(admin->s.clientNum,
			userinfo,
			sizeof(userinfo));
		aguid = Info_ValueForKey(userinfo, "cl_guid");
		for(i=0; g_shrubbot_admins[i]; i++) {
			if(!Q_stricmp(g_shrubbot_admins[i]->guid , aguid)) {
				a = g_shrubbot_admins[i];
				Q_strncpyz(flags, a->flags, sizeof(flags));
				for(j=0; g_shrubbot_levels[j]; j++) {
					if(g_shrubbot_levels[j]->level == a->level) {
						l = g_shrubbot_levels[j];
						Q_strcat(flags, sizeof(flags), l->flags);
						break;
					}
				}
				break;
			}
		}
		trap_GetUserinfo(admin->s.clientNum,
			userinfo,
			sizeof(userinfo));
		aguid = Info_ValueForKey(userinfo, "cl_guid");
	}

	if(Q_SayArgc() > 1+skiparg) {
		Q_SayArgv(1+skiparg, name, sizeof(name));
		if(ClientNumbersFromString(name, pids) == 1) {
			victim = &g_entities[pids[0]];
		}
	}

	if(victim && Q_stricmp(cmd, "attempted")) {
		trap_GetUserinfo(victim->s.clientNum,
			userinfo,
			sizeof(userinfo));
		vguid = Info_ValueForKey(userinfo, "cl_guid");
		Com_sprintf(string, sizeof(string), 
			"%3i:%i%i: %i: %s: %s: %s: %s: %s: %s: \"%s\"\n", 
			min,
			tens,
			sec,
			(admin) ? admin->s.clientNum : -1,
			(admin) ? aguid : "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
			(admin) ? admin->client->pers.netname : "console",
			flags,
			cmd,
			vguid,
			victim->client->pers.netname,
			Q_SayConcatArgs(2+skiparg));
	}
	else {
		Com_sprintf(string, sizeof(string), 
			"%3i:%i%i: %i: %s: %s: %s: %s: \"%s\"\n", 
			min,
			tens,
			sec,
			(admin) ? admin->s.clientNum : -1,
			(admin) ? aguid : "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
			(admin) ? admin->client->pers.netname : "console",
			flags,
			cmd,
			Q_SayConcatArgs(1+skiparg));
	}
	trap_FS_Write(string, strlen(string), f);
	trap_FS_FCloseFile(f);
}

void G_shrubbot_duration(int secs, char *duration, int dursize) 
{

	if(secs > (60*60*24*365*50) || secs < 0) {
		Q_strncpyz(duration, "PERMANENT", dursize);
	}
	else if(secs > (60*60*24*365*2)) {
		Com_sprintf(duration, dursize, "%d years",
			(secs / (60*60*24*365)));
	}
	else if(secs > (60*60*24*365)) {
		Q_strncpyz(duration, "1 year", dursize);
	}
	else if(secs > (60*60*24*30*2)) {
		Com_sprintf(duration, dursize, "%i months",
			(secs / (60*60*24*30)));
	}
	else if(secs > (60*60*24*30)) {
		Q_strncpyz(duration, "1 month", dursize);
	}
	else if(secs > (60*60*24*2)) {
		Com_sprintf(duration, dursize, "%i days",
			(secs / (60*60*24)));
	}
	else if(secs > (60*60*24)) {
		Q_strncpyz(duration, "1 day", dursize);
	}
	else if(secs > (60*60*2)) {
		Com_sprintf(duration, dursize, "%i hours",
			(secs / (60*60)));
	}
	else if(secs > (60*60)) {
		Q_strncpyz(duration, "1 hour", dursize);
	}
	else if(secs > (60*2)) {
		Com_sprintf(duration, dursize, "%i mins",
			(secs / 60));
	}
	else if(secs > 60) {
		Q_strncpyz(duration, "1 minute", dursize);
	}
	else {
		Com_sprintf(duration, dursize, "%i secs", secs);
	}
}

qboolean G_shrubbot_ban_check(char *userinfo) 
{
	char *guid, *ip;
	int i;
	time_t t;

	if(!time(&t)) return qfalse;
	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	if(!*userinfo) return qfalse;
	ip = Info_ValueForKey(userinfo, "ip");
	if(!*ip) return qfalse;
	guid = Info_ValueForKey(userinfo, "cl_guid");
	if(!*guid) return qfalse;
	for(i=0; g_shrubbot_bans[i]; i++) {
		// 0 is for perm ban
		if(g_shrubbot_bans[i]->expires != 0 &&
			(g_shrubbot_bans[i]->expires - t) < 1)
			continue;
		if(strstr(ip, g_shrubbot_bans[i]->ip))
			return qtrue;
		if(!Q_stricmp(g_shrubbot_bans[i]->guid, guid))
			return qtrue;
	}
	return qfalse;
}

qboolean G_shrubbot_cmd_check(gentity_t *ent)
{
	int i;
	char command[MAX_SHRUBBOT_CMD_LEN];
	char *cmd;
	int skip = 0;

	if(g_shrubbot.string[0] == '\0') 
		return qfalse;

	command[0] = '\0';
	Q_SayArgv(0, command, sizeof(command));
	if(!Q_stricmp(command, "say") || 
		(G_shrubbot_permission(ent, SBF_TEAMFTSHRUBCMD) && 
			(!Q_stricmp(command, "say_team") || 
			!Q_stricmp(command, "say_buddy")))) {
				skip = 1;
				Q_SayArgv(1, command, sizeof(command));
	}
	if(!command[0]) 
		return qfalse;

	if(command[0] == '!') {
		cmd = &command[1];
	}
	else if(ent == NULL) {
		cmd = &command[0];
	}
	else {
		return qfalse;
	}

	for (i=0; g_shrubbot_cmds[i].keyword[0]; i++) {
		if(Q_stricmp(cmd, g_shrubbot_cmds[i].keyword)) 
			continue;
		if((g_shrubbot_cmds[i].cmdFlags & SCMDF_TYRANNY) &&
			!g_tyranny.integer) {

			SP(va("%s: g_tyranny is not enabled\n",
				g_shrubbot_cmds[i].keyword));
			_shrubbot_log(ent, "attempted", skip-1);
		}
		else if(G_shrubbot_permission(ent, g_shrubbot_cmds[i].flag)) {
			g_shrubbot_cmds[i].handler(ent, skip);
			_shrubbot_log(ent, cmd, skip);
			return qtrue; 
		}
		else {
			SP(va("%s: permission denied\n",
				g_shrubbot_cmds[i].keyword));
			_shrubbot_log(ent, "attempted", skip-1);
		}
	}
	return qfalse;
}

qboolean G_shrubbot_readconfig(gentity_t *ent, int skiparg) 
{
	g_shrubbot_level_t *l = NULL;
	g_shrubbot_admin_t *a = NULL;
	g_shrubbot_ban_t *b = NULL;
	int lc = 0, ac = 0, bc = 0;
	fileHandle_t f;
//	int i;
	int len;
	char *cnf, *cnf2;
	char *t;
	qboolean level_open, admin_open, ban_open;

	if(!g_shrubbot.string[0]) return qfalse;
	len = trap_FS_FOpenFile(g_shrubbot.string, &f, FS_READ) ; 
	if(len < 0) {
		SP(va("readconfig: could not open shrubbot config file %s\n",
			g_shrubbot.string));
		_shrubbot_default_levels();
		return qfalse;
	}
	cnf = malloc(len+1);
	cnf2 = cnf;
	trap_FS_Read(cnf, len, f);
	*(cnf + len) = '\0';
	trap_FS_FCloseFile(f);

	G_shrubbot_cleanup();	
	
	t = COM_Parse(&cnf);
	level_open = admin_open = ban_open = qfalse;
	while(*t) {
		if(!Q_stricmp(t, "[level]") || 
			!Q_stricmp(t, "[admin]") ||
			!Q_stricmp(t, "[ban]")) {
			
			if(level_open) g_shrubbot_levels[lc++] = l;
			else if(admin_open) g_shrubbot_admins[ac++] = a;
			else if(ban_open) g_shrubbot_bans[bc++] = b;
			level_open = admin_open = ban_open = qfalse;
		}

		if(level_open) {
			if(!Q_stricmp(t, "level")) {
				G_shrubbot_readconfig_int(&cnf, &l->level);
			}
			else if(!Q_stricmp(t, "name")) {
				G_shrubbot_readconfig_string(&cnf, 
					l->name, sizeof(l->name)); 
			}
			else if(!Q_stricmp(t, "flags")) {
				G_shrubbot_readconfig_string(&cnf, 
					l->flags, sizeof(l->flags)); 
			}
			else {
				SP(va("readconfig: [level] parse error near "
					"%s on line %d\n", 
					t, 
					COM_GetCurrentParseLine()));
			}
		}
		else if(admin_open) {
			if(!Q_stricmp(t, "name")) {
				G_shrubbot_readconfig_string(&cnf, 
					a->name, sizeof(a->name)); 
			}
			else if(!Q_stricmp(t, "guid")) {
				G_shrubbot_readconfig_string(&cnf, 
					a->guid, sizeof(a->guid)); 
			}
			else if(!Q_stricmp(t, "level")) {
				G_shrubbot_readconfig_int(&cnf, &a->level); 
			}
			else if(!Q_stricmp(t, "flags")) {
				G_shrubbot_readconfig_string(&cnf, 
					a->flags, sizeof(a->flags)); 
			}
			else {
				SP(va("readconfig: [admin] parse error near "
					"%s on line %d\n", 
					t, 
					COM_GetCurrentParseLine()));
			}

		}
		else if(ban_open) {
			if(!Q_stricmp(t, "name")) {
				G_shrubbot_readconfig_string(&cnf, 
					b->name, sizeof(b->name)); 
			}
			else if(!Q_stricmp(t, "guid")) {
				G_shrubbot_readconfig_string(&cnf, 
					b->guid, sizeof(b->guid)); 
			}
			else if(!Q_stricmp(t, "ip")) {
				G_shrubbot_readconfig_string(&cnf, 
					b->ip, sizeof(b->ip)); 
			}
			else if(!Q_stricmp(t, "reason")) {
				G_shrubbot_readconfig_string(&cnf, 
					b->reason, sizeof(b->reason)); 
			}
			else if(!Q_stricmp(t, "made")) {
				G_shrubbot_readconfig_string(&cnf, 
					b->made, sizeof(b->made)); 
			}
			else if(!Q_stricmp(t, "expires")) {
				G_shrubbot_readconfig_int(&cnf, &b->expires);
			}
			else if(!Q_stricmp(t, "banner")) {
				G_shrubbot_readconfig_string(&cnf, 
					b->banner, sizeof(b->banner)); 
			}
			else {
				SP(va("readconfig: [ban] parse error near "
					"%s on line %d\n", 
					t, 
					COM_GetCurrentParseLine()));
			}
		}

		if(!Q_stricmp(t, "[level]")) {
			if(lc >= MAX_SHRUBBOT_LEVELS) return qfalse;
			l = malloc(sizeof(g_shrubbot_level_t));
			l->level = 0;
			*l->name = '\0';
			*l->flags = '\0';
			level_open = qtrue;
		}
		else if(!Q_stricmp(t, "[admin]")) {
			if(ac >= MAX_SHRUBBOT_ADMINS) return qfalse;
			a = malloc(sizeof(g_shrubbot_admin_t));
			*a->name = '\0';
			*a->guid = '\0';
			a->level = 0;
			*a->flags = '\0';
			admin_open = qtrue;
		}
		else if(!Q_stricmp(t, "[ban]")) {
			if(bc >= MAX_SHRUBBOT_BANS) return qfalse;
			b = malloc(sizeof(g_shrubbot_ban_t));
			*b->name = '\0';
			*b->guid = '\0';
			*b->ip = '\0';
			*b->made = '\0';
			b->expires = 0;
			*b->reason = '\0';
			ban_open = qtrue;
		}
		t = COM_Parse(&cnf);
	}
	if(level_open) g_shrubbot_levels[lc++] = l;
	if(admin_open) g_shrubbot_admins[ac++] = a;
	if(ban_open) g_shrubbot_bans[bc++] = b;
	free(cnf2);
	SP(va("readconfig: loaded %d levels, %d admins, %d bans\n",
		lc, ac, bc));
	if(lc == 0) _shrubbot_default_levels();
	return qtrue;
}

qboolean G_shrubbot_time(gentity_t *ent, int skiparg) 
{
	time_t t;
	struct tm *lt;
	char s[32];

	if(!time(&t)) return qfalse;
	lt = localtime(&t);
	strftime(s, sizeof(s), "%I:%M%p %Z", lt);
	AP(va("chat \"Local Time: %s\"", s));
	return qtrue;
}

qboolean G_shrubbot_setlevel(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char lstr[11]; // 10 is max strlen() for 32-bit int
	char userinfo[MAX_INFO_STRING];
	char *guid;
	char n2[MAX_NAME_LENGTH];
	int l, i;
	gentity_t *vic;
	qboolean updated = qfalse;
	g_shrubbot_admin_t *a;
	qboolean found = qfalse;


	if(Q_SayArgc() != 3+skiparg) {
		SP("setlevel usage: !setlevel [name|slot#] [level]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, lstr, sizeof(lstr));
	l = atoi(lstr);

	// forty - don't allow privilege escalation
	if(l > _shrubbot_level(ent)) {
		SP("setlevel: you may not setlevel higher than your current level\n");
		return qfalse;
	}

	for(i=0; g_shrubbot_levels[i]; i++) {
		if(g_shrubbot_levels[i]->level == l) {
			found = qtrue;
			break;
		}
	}
	if(!found) {
		SP("setlevel: level is not defined\n");
		return qfalse;
	}
	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("setlevel: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("setlevel: sorry, but your intended victim has a higher"
			" admin level than you do.\n");
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	trap_GetUserinfo(pids[0], userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	// tjw: use raw name
	//SanitizeString(vic->client->pers.netname, n2, qtrue);
	Q_strncpyz(n2, vic->client->pers.netname, sizeof(n2));

	for(i=0;g_shrubbot_admins[i];i++) {
		if(!Q_stricmp(g_shrubbot_admins[i]->guid, guid)) {
			g_shrubbot_admins[i]->level = l;
			Q_strncpyz(g_shrubbot_admins[i]->name, n2, 
				sizeof(g_shrubbot_admins[i]->name));
			updated = qtrue;
		}
	}
	if(!updated) {
		if(i == MAX_SHRUBBOT_ADMINS) {
			SP("setlevel: too many admins\n");
			return qfalse;
		}
		a = malloc(sizeof(g_shrubbot_admin_t));
		a->level = l;
		Q_strncpyz(a->name, n2, sizeof(a->name));
		Q_strncpyz(a->guid, guid, sizeof(a->guid));
		*a->flags = '\0';
		g_shrubbot_admins[i] = a;
	}

	AP(va("chat \"^osetlevel: ^7%s^7 is now a level %d user\"",
		vic->client->pers.netname,
		l));
	_shrubbot_writeconfig();
	return qtrue;
}

qboolean G_shrubbot_kick(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], *reason, err[MAX_STRING_CHARS];
	int minargc;

	minargc = 3+skiparg;
	if(G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) 
		minargc = 2+skiparg;

	if(Q_SayArgc() < minargc) {
		SP("kick usage: !kick [name] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);
	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("kick: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("kick: sorry, but your intended victim has a higher admin"
			" level than you do.\n");
		return qfalse;
	}
	//josh: 2.60 won't kick from the engine. This will call 
	//      ClientDisconnect
	// 120 seconds. Make #define?
	trap_DropClient(pids[0],
		va("You have been kicked, Reason: %s",
		(*reason) ? reason : "kicked by admin"),
		120);
	return qtrue;
}

qboolean G_shrubbot_ban(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	int seconds;
	char name[MAX_NAME_LENGTH], secs[7];
	char *reason, err[MAX_STRING_CHARS];
	char userinfo[MAX_INFO_STRING];
	char *guid, *ip;
	char tmp[MAX_NAME_LENGTH];
	int i;
	g_shrubbot_ban_t *b;
	time_t t;
	struct tm *lt;
	gentity_t *vic;
	int minargc;
	char duration[MAX_STRING_CHARS];

	if(!time(&t)) return qfalse;
	if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN) &&
		G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) {
		minargc = 2+skiparg;
	}
	else if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN) ||
		G_shrubbot_permission(ent, SBF_UNACCOUNTABLE)) {

		minargc = 3+skiparg;
	}
	else {
		minargc = 4+skiparg;
	}
	if(Q_SayArgc() < minargc) {
		SP("ban usage: !ban [name] [seconds] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, secs, sizeof(secs));
	seconds = atoi(secs);

	if(seconds <= 0) {
		if(G_shrubbot_permission(ent, SBF_CAN_PERM_BAN)) {
			seconds = 0;
		}
		else {
			SP("ban: seconds must be a positive integer\n");
			return qfalse;
		}
		reason = Q_SayConcatArgs(2+skiparg);
	}
	else {
		reason = Q_SayConcatArgs(3+skiparg);
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("ban: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("ban: sorry, but your intended victim has a higher admin"
			" level than you do.\n");
		return qfalse;
	}

	trap_GetUserinfo(pids[0], userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	ip = Info_ValueForKey(userinfo, "ip");
	vic = &g_entities[pids[0]];
	b = malloc(sizeof(g_shrubbot_ban_t));

	//SanitizeString(vic->client->pers.netname, tmp, qtrue);
	Q_strncpyz(b->name,
		vic->client->pers.netname,
		sizeof(b->name));
	Q_strncpyz(b->guid, guid, sizeof(b->guid));

	// strip port off of ip
	for(i=0; *ip; *ip++) {
		if(i >= sizeof(tmp) || *ip == ':') break;
		tmp[i++] = *ip; 
	}
	tmp[i] = '\0';
	Q_strncpyz(b->ip, tmp, sizeof(b->ip));

	lt = localtime(&t);
	strftime(b->made, sizeof(b->made), "%c", lt);
	if(ent) {
		//SanitizeString(ent->client->pers.netname, tmp, qtrue);
		//Q_strncpyz(b->banner, tmp, sizeof(b->banner));
		Q_strncpyz(b->banner,
			ent->client->pers.netname,
			sizeof(b->banner));
	}
	else Q_strncpyz(b->banner, "console", sizeof(b->banner));
	if(!seconds) 
		b->expires = 0;
	else 
		b->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET + seconds;
	if(!*reason) {
		Q_strncpyz(b->reason,
			"banned by admin",
			sizeof(b->reason));
	}
	else {
		Q_strncpyz(b->reason, reason, sizeof(b->reason));
	}
	for(i=0; g_shrubbot_bans[i]; i++);
	if(i == MAX_SHRUBBOT_BANS) {
		SP("ban: too many bans\n");
		free(b);
		return qfalse;
	}
	g_shrubbot_bans[i] = b;
	SP(va("ban: %s^7 is now banned\n", vic->client->pers.netname));
	_shrubbot_writeconfig();
	// 0 seconds. Let bans handle time
	if(seconds) {
		Com_sprintf(duration,
			sizeof(duration),
			"for %i seconds",
			seconds);
	}
	else {
		Q_strncpyz(duration, "PERMANENTLY", sizeof(duration));
	}
	trap_DropClient(pids[0],
		va("You have been banned %s, Reason: %s",
		duration,
		(*reason) ? reason : "banned by admin"),
		0);
	return qtrue;
}

qboolean G_shrubbot_unban(gentity_t *ent, int skiparg) 
{
	int bnum;
	char bs[4];
	time_t t;
	
	if(!time(&t)) return qfalse;
	if(Q_SayArgc() != 2+skiparg) {
		SP("unban usage: !unban [ban #]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, bs, sizeof(bs));
	bnum = atoi(bs);
	if(bnum < 1) {
		SP("unban: invalid ban #\n");
		return qfalse;
	}
	if(!g_shrubbot_bans[bnum-1]) {
		SP("unban: invalid ban #\n");
		return qfalse;
	}
	g_shrubbot_bans[bnum-1]->expires = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	SP(va("unban: ban #%d removed\n", bnum));
	_shrubbot_writeconfig();
	return qtrue;
}

qboolean G_shrubbot_putteam(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], team[7], err[MAX_STRING_CHARS];
	gentity_t *vic;

	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, team, sizeof(team));
	if(Q_SayArgc() != 3+skiparg) {
		SP("putteam usage: !putteam [name] [r|b]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	Q_SayArgv(2+skiparg, team, sizeof(team));

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("putteam: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("putteam: sorry, but your intended victim has a higher "
			" admin level than you do.\n");
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!SetTeam(vic, team, qtrue, -1, -1, qfalse)) {
		SP("putteam: SetTeam failed\n");
		return qfalse;
	}
	return qtrue;
}

qboolean G_shrubbot_mute(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char command[MAX_SHRUBBOT_CMD_LEN], *cmd;
	gentity_t *vic;

	if(Q_SayArgc() != 2+skiparg) {
		SP("mute usage: !mute [name|slot#]\n");
		return qfalse;
	}
	Q_SayArgv(skiparg, command, sizeof(command));
	cmd = command;
	if(*cmd == '!')
		*cmd++;
	Q_SayArgv(1+skiparg, name, sizeof(name)); 
	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("mute: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("mute: sorry, but your intended victim has a higher admin"
			" level than you do.\n");
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(vic->client->sess.muted == qtrue) {
		if(!Q_stricmp(cmd, "mute")) {
			SP("mute: player is already muted\n");
			return qtrue;
		}
		vic->client->sess.muted = qfalse;
		vic->client->sess.auto_mute_time = -1;
                CPx(pids[0], "print \"^5You've been unmuted\n\"" );
                AP(va("chat \"%s^7 has been unmuted\"",  
			vic->client->pers.netname ));
	}
	else {
		if(!Q_stricmp(cmd, "unmute")) {
			SP("unmute: player is not currently muted\n");
			return qtrue;
		}
		vic->client->sess.muted = qtrue;
		vic->client->sess.auto_mute_time = -1;
                CPx(pids[0], "print \"^5You've been muted\n\"" );
                AP(va("chat \"%s^7 has been muted\"",  
			vic->client->pers.netname ));
	}
	ClientUserinfoChanged(pids[0]);
	return qtrue;
}


qboolean G_shrubbot_pause(gentity_t *ent, int skiparg) 
{
	G_refPause_cmd(ent, qtrue);
	return qtrue;
}

qboolean G_shrubbot_unpause(gentity_t *ent, int skiparg) 
{
	G_refPause_cmd(ent, qfalse);
	return qtrue;
}

qboolean G_shrubbot_listplayers(gentity_t *ent, int skiparg) 
{
	int i,j;
	gclient_t *p;
	char c[3], t[2]; // color and team letter
	char n[MAX_NAME_LENGTH] = {""};
	char n2[MAX_NAME_LENGTH] = {""};
	char n3[MAX_NAME_LENGTH] = {""};
	char lname[MAX_NAME_LENGTH];
	char lname2[MAX_NAME_LENGTH];
	char *guid;
	char guid_stub[9];
	char fireteam[2];
	char muted[2];
	int l;
	fireteamData_t *ft;
	char userinfo[MAX_INFO_STRING];
	int lname_max = 2;
	char lname_fmt[5];

	// detect the longest level name length
	for(i=0; g_shrubbot_levels[i]; i++) {
		//SanitizeString(g_shrubbot_levels[i]->name, n, qtrue);
		DecolorString(g_shrubbot_levels[i]->name, n);
		if(strlen(n) > lname_max)
			lname_max = strlen(n);
		Com_sprintf(n, sizeof(n), "%d", g_shrubbot_levels[i]->level);
		if(strlen(n) > lname_max)
			lname_max = strlen(n);
	}
	Com_sprintf(lname_fmt, 
		sizeof(lname_fmt),
	       "%%%is",
       		lname_max);


	SP(va("listplayers: %d players connected:\n", 
		level.numConnectedClients));
	for(i=0; i < level.maxclients; i++) {
		p = &level.clients[i];
		Q_strncpyz(t, "S", sizeof(t));
		Q_strncpyz(c, S_COLOR_YELLOW, sizeof(c));
		if(p->sess.sessionTeam == TEAM_ALLIES) {
			Q_strncpyz(t, "B", sizeof(t));
			Q_strncpyz(c, S_COLOR_BLUE, sizeof(c));
		}
		else if(p->sess.sessionTeam == TEAM_AXIS) {
			Q_strncpyz(t, "R", sizeof(t));
			Q_strncpyz(c, S_COLOR_RED, sizeof(c));
		}

		if(p->pers.connected == CON_CONNECTING) {
			Q_strncpyz(t, "C", sizeof(t));
			Q_strncpyz(c, S_COLOR_ORANGE, sizeof(c));
		}
		else if(p->pers.connected != CON_CONNECTED) {
			continue;
		}

		trap_GetUserinfo(i, userinfo, sizeof(userinfo));
		guid = Info_ValueForKey(userinfo, "cl_guid");

		guid_stub[0] = '\0';
		for(j=0; j<=8; j++) 
			guid_stub[j] = guid[j+24];

		fireteam[0] = '\0';
		if(G_IsOnFireteam(i, &ft)) {
			Q_strncpyz(fireteam, 
				bg_fireteamNames[ft->ident - 1],
				sizeof(fireteam));
		}

		muted[0] = '\0';
		if(p->sess.muted) {
			Q_strncpyz(muted,"M",sizeof(muted));
		}
	
		l = 0;
		SanitizeString(p->pers.netname, n2, qtrue);
		n[0] = '\0';
		for(j=0; g_shrubbot_admins[j]; j++) {
			if(!Q_stricmp(g_shrubbot_admins[j]->guid, guid)) {
				l = g_shrubbot_admins[j]->level;
				SanitizeString(g_shrubbot_admins[j]->name,
					n3, qtrue);
				if(Q_stricmp(n2, n3)) {
					Q_strncpyz(n,
						g_shrubbot_admins[j]->name,
						sizeof(n));
				}
				break;
			}
		}
		lname[0] = '\0';
		for(j=0; g_shrubbot_levels[j]; j++) {
			if(g_shrubbot_levels[j]->level == l)
				Q_strncpyz(lname,
					g_shrubbot_levels[j]->name,
					sizeof(lname));	
		}
		if(!*lname)  {
			Com_sprintf(lname,
				sizeof(lname),
				"%i",
				l);
		}
		Com_sprintf(lname2, sizeof(lname2), lname_fmt, lname);
		SP(va("%2i %s%s^7 %s^7 (*%s) ^1%1s ^3%1s^7 %s^7 %s%s^7%s\n", 
			i,
			c,
			t,
			lname2,
			guid_stub,
			muted,
			fireteam,
			p->pers.netname,
			(*n) ? "(a.k.a. " : "",
			n,
			(*n) ? ")" : ""
			));
	}
	return qtrue;
}

qboolean G_shrubbot_listteams(gentity_t *ent, int skiparg)
{
	int playerCount[3], pings[3], totalXP[3];
	int i, j;
	gclient_t *p;

	playerCount[0] = pings[0] = totalXP[0] = 0;
	playerCount[1] = pings[1] = totalXP[1] = 0;
	playerCount[2] = pings[2] = totalXP[2] = 0;

	for(i=0; i<level.maxclients; i++) {
		p = &level.clients[i];
		if(p->pers.connected != CON_CONNECTED) {
			continue;
		}
		if(p->sess.sessionTeam == TEAM_ALLIES) {
			playerCount[2]++;
			pings[2] += p->ps.ping;
			for(j = 0; j < SK_NUM_SKILLS; j++) {
				totalXP[2] += p->sess.skillpoints[j];
			}
		} else if(p->sess.sessionTeam == TEAM_AXIS) {
			playerCount[1]++;
			pings[1] += p->ps.ping;
			for(j = 0; j < SK_NUM_SKILLS; j++) {
				totalXP[1] += p->sess.skillpoints[j];
			}
		} else if(p->sess.sessionTeam == TEAM_SPECTATOR) {
			playerCount[0]++;
			pings[0] += p->ps.ping;
			for(j = 0; j < SK_NUM_SKILLS; j++) {
				totalXP[0] += p->sess.skillpoints[j];
			}
		}
	}

	SP(va("Desc       ^4Allies^7(%s^7)      ^1Axis^7(%s^7)       ^3Specs\n",
			teamInfo[TEAM_ALLIES].team_lock ? "^1L" : "^2U",
			teamInfo[TEAM_AXIS].team_lock ? "^1L" : "^2U"));
	SP("^7---------------------------------------------\n");
	SP(va("^2Players     ^7%8d     %8d    %8d\n",
			playerCount[2],
			playerCount[1],
			playerCount[0]));
	SP(va("^2Avg Ping    ^7%8.2f     %8.2f    %8.2f\n",
			playerCount[2]>0?(float)(pings[2])/playerCount[2]:0,
			playerCount[1]>0?(float)(pings[1])/playerCount[1]:0,
			playerCount[0]>0?(float)(pings[0])/playerCount[0]:0));
	SP(va("^2Map XP      ^7%8d     %8d          --\n",
			level.teamScores[TEAM_ALLIES],
			level.teamScores[TEAM_AXIS]));
	SP(va("^2Total XP    ^7%8d     %8d    %8d\n",
			totalXP[2],
			totalXP[1],
			totalXP[0]));
	SP(va("^2Avg Map XP  ^7%8.2f     %8.2f          --\n",
			playerCount[2]>0?
				(float)(level.teamScores[TEAM_ALLIES])/
					playerCount[2]:0,
			playerCount[1]>0?
				(float)(level.teamScores[TEAM_AXIS])/
					playerCount[1]:0));
	SP(va("^2Avg Tot XP  ^7%8.2f     %8.2f    %8.2f\n",
			playerCount[2]>0?(float)(totalXP[2])/playerCount[2]:0,
			playerCount[1]>0?(float)(totalXP[1])/playerCount[1]:0,
			playerCount[0]>0?(float)(totalXP[0])/playerCount[0]:0));
	SP("\n");
	SP("Ratings       ^5Win Prob       ^5Win Points\n");
	SP("^7---------------------------------------------\n");
	G_GetWinProbability(TEAM_ALLIES,ent,qfalse);
	G_GetWinProbability(TEAM_AXIS,ent,qfalse);

	return qfalse;
}

qboolean G_shrubbot_showbans(gentity_t *ent, int skiparg) 
{
	int i, found = 0;
	time_t t;
	char duration[MAX_STRING_CHARS];
	char fmt[MAX_STRING_CHARS];
	int max_name = 1, max_banner = 1;
	int secs;
	int start = 0;
	char skip[11];
	char date[11];
	char *made;
	int j;
	char n1[MAX_NAME_LENGTH] = {""};
	char n2[MAX_NAME_LENGTH] = {""};

	if(!time(&t))
		return qfalse;
	t = t - SHRUBBOT_BAN_EXPIRE_OFFSET;
	
	for(i=0; g_shrubbot_bans[i]; i++) {
		if(g_shrubbot_bans[i]->expires != 0 &&
			(g_shrubbot_bans[i]->expires - t) < 1) {
			
			continue;
		}
		found++;
	}

	if(Q_SayArgc() == 2+skiparg) {
		Q_SayArgv(1+skiparg, skip, sizeof(skip));
		start = atoi(skip);
		// tjw: !showbans 1 means start with ban 0
		if(start > 0) 
			start -= 1;
		else if(start < 0)
			start = found + start;
	}

	// tjw: sanity check
	if(start >= MAX_SHRUBBOT_BANS || start < 0)
		start = 0;

	for(i=start;
		(g_shrubbot_bans[i] && (i-start) < SHRUBBOT_MAX_SHOWBANS);
		i++) {

		DecolorString(g_shrubbot_bans[i]->name, n1);
		DecolorString(g_shrubbot_bans[i]->banner, n2);
		if(strlen(n1) > max_name) {
			max_name = strlen(n1);
		}
		if(strlen(n2) > max_banner) {
			max_banner = strlen(n2);
		}
	}
	Com_sprintf(fmt, sizeof(fmt),
		"^F%%4i^7 %%-%is^7 ^F%%-10s^7 %%-%is^7 ^F%%-9s^7 %%s\n",
		max_name, max_banner);




	if(start > found) {
		SP(va("showbans: there are only %d active bans\n", found));
		return qfalse;
	}

	for(i=start;
		(g_shrubbot_bans[i] && (i-start) < SHRUBBOT_MAX_SHOWBANS);
		i++) {

		if(g_shrubbot_bans[i]->expires != 0 &&
			(g_shrubbot_bans[i]->expires - t) < 1) {
			
			continue;
		}

		// tjw: only print out the the date part of made
		date[0] = '\0';
		made = g_shrubbot_bans[i]->made;
		for(j=0; *made; j++) {
			if((j+1) >= sizeof(date)) 
				break;
			if(*made == ' ')
				break;
			date[j] = *made;
			date[j+1] = '\0';
			*made++;
		}

		secs = (g_shrubbot_bans[i]->expires - t);
		G_shrubbot_duration(secs, duration, sizeof(duration));
		SP(va(fmt, 
			(i+1),
			g_shrubbot_bans[i]->name,
			date,
			g_shrubbot_bans[i]->banner,
			duration,
			g_shrubbot_bans[i]->reason
			));
	}

	SP(va("showbans: showing bans %d - %d of %d\n",
		(start + 1),
		((start + SHRUBBOT_MAX_SHOWBANS) > found) ?
			found : (start + SHRUBBOT_MAX_SHOWBANS),
		found));
	if((start + SHRUBBOT_MAX_SHOWBANS) < found) {
		SP(va("          type !showbans %d to see more\n",
			(start + SHRUBBOT_MAX_SHOWBANS + 1)));
	}
	return qtrue;
}

qboolean G_shrubbot_help(gentity_t *ent, int skiparg) 
{
	int i;
	SP("help: available commands:\n");
	for (i=0; g_shrubbot_cmds[i].keyword[0]; i++) {
		if(G_shrubbot_permission(ent, g_shrubbot_cmds[i].flag))
			SP(va(" !%s\n", g_shrubbot_cmds[i].keyword));
	}
	return qtrue;
}

qboolean G_shrubbot_admintest(gentity_t *ent, int skiparg) 
{
	int i, l = 0;
	char *guid;
	qboolean found = qfalse;
	qboolean lname = qfalse;
	char userinfo[MAX_INFO_STRING];

	if(!ent) {	
		SP("admintest: you are on the console.\n");
		return qtrue;
	}
	trap_GetUserinfo(ent-g_entities, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	for(i=0; g_shrubbot_admins[i]; i++) {
		if(!Q_stricmp(g_shrubbot_admins[i]->guid, guid)) {
			found = qtrue;
			break;	
		}
	}

	if(found) {
		l = g_shrubbot_admins[i]->level;
		for(i=0; g_shrubbot_levels[i]; i++) {
			if(g_shrubbot_levels[i]->level != l) 
				continue;
			if(*g_shrubbot_levels[i]->name) {
				lname = qtrue;
				break;
			}
		}
	}
	AP(va("chat \"admintest: %s^7 is a level %d user %s%s^7%s\"",
		ent->client->pers.netname,
		l,
		(lname) ? "(" : "",
		(lname) ? g_shrubbot_levels[i]->name : "",
		(lname) ? ")" : ""));
	return qtrue;
}

qboolean G_shrubbot_cancelvote(gentity_t *ent, int skiparg) 
{

	level.voteInfo.voteNo = level.numConnectedClients;
	CheckVote();
	SP("cancelvote: turns out everyone voted No\n");
	return qtrue;
}

qboolean G_shrubbot_passvote(gentity_t *ent, int skiparg) 
{
	level.voteInfo.voteYes = level.numConnectedClients;
	level.voteInfo.voteNo = 0;
	CheckVote();
	SP("passvote: turns out everyone voted Yes\n");
	return qtrue;
}

qboolean G_shrubbot_spec999(gentity_t *ent, int skiparg) 
{
	int i;
	gentity_t *vic;

	for(i=0; i < level.maxclients; i++) {
		vic = &g_entities[i];
		if(!vic->client) continue;
		if(vic->client->pers.connected != CON_CONNECTED) continue;
		if(vic->client->ps.ping == 999) {
			SetTeam(vic, "s", qtrue, -1, -1, qfalse);
			AP(va("chat \"spec999: %s^7 moved to spectators\"",
				vic->client->pers.netname));
		}
	}
	return qtrue;
}

qboolean G_shrubbot_shuffle(gentity_t *ent, int skiparg) 
{
	G_shuffleTeams();
	return qtrue;
}

qboolean G_shrubbot_rename(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], *newname, *oldname,err[MAX_STRING_CHARS];
	char userinfo[MAX_INFO_STRING];

	if(Q_SayArgc() < 3+skiparg) {
		SP("rename usage: !rename [name] [newname]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	newname = Q_SayConcatArgs(2+skiparg);
	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("rename: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("rename: sorry, but your intended victim has a higher admin"
			" level than you do.\n");
		return qfalse;
	}
	SP(va("!rename: renaming %s to %s\n",name,newname));
	trap_GetUserinfo( pids[0], userinfo, sizeof( userinfo ) );
	oldname = Info_ValueForKey( userinfo, "name" );
	// Send to chat for shame
	AP(va("chat \"^orename: ^7%s^7 has been renamed to %s\"",
		oldname,
		newname));
	Info_SetValueForKey( userinfo, "name", newname);
	trap_SetUserinfo( pids[0], userinfo );
	ClientUserinfoChanged(pids[0]);
	return qtrue;
}

qboolean G_shrubbot_gib(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;

	if(Q_SayArgc() != 2+skiparg) {
		SP("gib usage: !gib [name|slot#]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name)); 
	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("mute: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("gib: sorry, but your intended victim has a higher admin"
			" level than you do.\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("gib: player must be on a team to be gibbed\n");
		return qfalse;
	}
	
	G_Damage(vic, NULL, NULL, NULL, NULL, 500, 0, MOD_UNKNOWN);
	AP(va("chat \"^ogib: ^7%s ^7was gibbed\"", vic->client->pers.netname));
	return qtrue;
}

qboolean G_shrubbot_slap(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS], dmg;
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char damage[4], *reason;
	gentity_t *vic;

	if(Q_SayArgc() < 2+skiparg) {
		SP("usage: !slap [name|slot#] [damage] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name)); 
	Q_SayArgv(2+skiparg, damage, sizeof(damage));

	dmg = atoi(damage);
	if(dmg <= 0) {
		dmg = 20;
		reason = Q_SayConcatArgs(2+skiparg);
	}
	else {
		reason = Q_SayConcatArgs(3+skiparg);
	}

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("slap: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("slap: sorry, but your intended victim has a higher admin"
			" level than you do.\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("slap: player must be on a team to be slapped\n");
		return qfalse;
	}
	if(vic->health < 1 || vic->client->ps.pm_flags & PMF_LIMBO) {
		SP(va("slap: %s ^7is not alive\n",vic->client->pers.netname));
		return qfalse;
	}
	
	if ( vic->health > dmg ) {
		vic->health -= dmg;
	}
	else {
		vic->health = 1;
	}

	G_AddEvent(vic, EV_GENERAL_SOUND, 
		G_SoundIndex("sound/player/hurt_barbwire.wav"));
	
	AP(va("chat \"^oslap: ^7%s ^7was slapped\"",
		vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^7slapped you%s%s\"", 
		(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
		(*reason) ? " because:\n" : "",
		(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_burn(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;

	if(Q_SayArgc() < 2+skiparg) {
		SP("usage: !burn [name|slot#] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("burn: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("burn: sorry, but your intended victim has a higher admin"
			" level than you do.\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("burn: player must be on a team\n");
		return qfalse;
	}
	
	G_BurnMeGood(vic, vic, NULL);
	AP(va("chat \"^oburn: ^7%s ^7was set ablaze\"",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^7burned you%s%s\"", 
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? " because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_warn(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;

	if(Q_SayArgc() < 3+skiparg) {
		SP("warn usage: !warn [name|slot#] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name)); 
	reason = Q_SayConcatArgs(2+skiparg);

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("warn: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("warn: sorry, but your intended victim has a higher admin"
			" level than you do.\n");
		return qfalse;
	}
	
	G_AddEvent(vic, EV_GENERAL_SOUND,
			G_SoundIndex("sound/misc/referee.wav"));

	AP(va("chat \"^owarn: ^7%s ^7was warned\"",
			vic->client->pers.netname));
	// tjw: can't do color code for the reason because the 
	//      client likes to start new lines and lose the
	//      color with long cp strings
	CPx(pids[0], va("cp \"%s ^3warned ^7you because:\n%s\"", 
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			reason));
	return qtrue;
}

qboolean G_shrubbot_news(gentity_t *ent, int skiparg)
{
	char mapname[MAX_STRING_CHARS];

	if(Q_SayArgc() == 2+skiparg) {
		Q_SayArgv(1+skiparg, mapname, sizeof(mapname));
	} else {
		Q_strncpyz(mapname, level.rawmapname,
				sizeof(mapname));
	}

	G_globalSound(va("sound/vo/%s/news_%s.wav",mapname,mapname));

	return qtrue;
}

qboolean G_shrubbot_lock(gentity_t *ent, int skiparg)
{
	return G_shrubbot_lockteams(ent, skiparg, qtrue);
}

qboolean G_shrubbot_unlock(gentity_t *ent, int skiparg)
{
	return G_shrubbot_lockteams(ent, skiparg, qfalse);
}

qboolean G_shrubbot_lockteams(gentity_t *ent, int skiparg, qboolean toLock)
{
	char team[4];
	char command[MAX_SHRUBBOT_CMD_LEN], *cmd;

	Q_SayArgv(skiparg, command, sizeof(command));
	cmd = command;
	if(*cmd == '!')
		*cmd++;
	
	if(Q_SayArgc() != 2+skiparg) {
		SP(va("%s usage: %s r|b|s|all\n",cmd,cmd));
		return qfalse;
	}
	Q_SayArgv(1+skiparg, team, sizeof(team));

	if ( !Q_stricmp(team, "all") ) {
		teamInfo[TEAM_AXIS].team_lock = 
			(TeamCount(-1, TEAM_AXIS)) ? toLock : qfalse;
		teamInfo[TEAM_ALLIES].team_lock = 
			(TeamCount(-1, TEAM_ALLIES)) ? toLock : qfalse;
		G_updateSpecLock(TEAM_AXIS, 
			(TeamCount(-1, TEAM_AXIS)) ? toLock : qfalse);
		G_updateSpecLock(TEAM_ALLIES, 
			(TeamCount(-1, TEAM_ALLIES)) ? toLock : qfalse);
		if( toLock ) {
			level.server_settings |= CV_SVS_LOCKSPECS;
		} else {
			level.server_settings &= ~CV_SVS_LOCKSPECS;
		}
		AP(va("chat \"^o%s: ^7All teams %sed\"", cmd, cmd));
	} else if ( !Q_stricmp(team, "r") ) {
		teamInfo[TEAM_AXIS].team_lock = 
			(TeamCount(-1, TEAM_AXIS)) ? toLock : qfalse;
		AP(va("chat \"^o%s: ^7Axis team %sed\"", cmd, cmd));
	} else if ( !Q_stricmp(team, "b") ) {
		teamInfo[TEAM_ALLIES].team_lock = 
			(TeamCount(-1, TEAM_ALLIES)) ? toLock : qfalse;
		AP(va("chat \"^o%s: ^7Allied team %sed\"", cmd, cmd));
	} else if ( !Q_stricmp(team, "s") ) {
		G_updateSpecLock(TEAM_AXIS, 
			(TeamCount(-1, TEAM_AXIS)) ? toLock : qfalse);
		G_updateSpecLock(TEAM_ALLIES, 
			(TeamCount(-1, TEAM_ALLIES)) ? toLock : qfalse);
		if( toLock ) {
			level.server_settings |= CV_SVS_LOCKSPECS;
		} else {
			level.server_settings &= ~CV_SVS_LOCKSPECS;
		}
		AP(va("chat \"^o%s: ^7Spectators %sed\"", cmd, cmd));
	} else {
		SP(va("%s usage: !%s r|b|s|all\n",cmd,cmd));
		return qfalse;
	}

	if( toLock ) {
		level.server_settings |= CV_SVS_LOCKTEAMS;
	} else {
		level.server_settings &= ~CV_SVS_LOCKTEAMS;
	}
	trap_SetConfigstring(CS_SERVERTOGGLES, 
			va("%d", level.server_settings));

	return qtrue;
}

// created by: dvl
qboolean G_shrubbot_lol(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS], numNades[4];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int pcount, nades = 0, count = 0;

	if(Q_SayArgc() < 2+skiparg) doAll = qtrue;
	else if(Q_SayArgc() >= 3+skiparg) {
		Q_SayArgv( 2+skiparg, numNades, sizeof( numNades ) );
		nades = atoi( numNades );
		if( nades < 1 ) 
			nades = 1;
		else if( nades > SHRUBBOT_MAX_LOL_NADES ) 
			nades = SHRUBBOT_MAX_LOL_NADES;
	}
	Q_SayArgv( 1+skiparg, name, sizeof( name ) );
	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher(ent, vic) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
			if( nades )
				G_createClusterNade( vic, nades );
			else
				G_createClusterNade( vic, 1 );
			count++;
		}
		AP(va("chat \"^olol: ^7%d players lol\'d\"", count));
		return qtrue;
	} 
	pcount = ClientNumbersFromString(name, pids);
	if(pcount > 1) {
		int it;
		for( it = 0; it < pcount; it++) {
			vic = &g_entities[pids[it]];
			if(!_shrubbot_admin_higher(ent, vic)) {
				SP(va("^olol: ^7sorry, but %s^7 has a higher "
					"admin level than you do.\n", 
					vic->client->pers.netname));
				continue;
			} else if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
					vic->client->sess.sessionTeam == TEAM_ALLIES)) {
				SP(va("^olol: ^7%s^7 must be on a "
						"team to be lol\'d\n", 
						vic->client->pers.netname));
				continue;
			}
			if( nades )
				G_createClusterNade( vic, nades );
			else
				G_createClusterNade( vic, 8 );
			AP(va("chat \"^olol: ^7%s\"", 
					vic->client->pers.netname));
		}
		return qtrue;
	} else if( pcount < 1 ) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^olol: ^7%s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^olol: ^7sorry, but your intended victim has a higher "
			"admin level than you do.\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^olol: ^7player must be on a team to be lol'd\n");
		return qfalse;
	}
	
	if( nades )
		G_createClusterNade( vic, nades );
	else
		G_createClusterNade( vic, 8 );
	AP(va("chat \"^olol: ^7%s\"", vic->client->pers.netname));
	return qtrue;

}
													
// Created by: dvl
qboolean G_shrubbot_pop( gentity_t *ent, int skiparg )
{
	vec3_t dir = { 5, 5, 5 };
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int pcount, count = 0;

	if(Q_SayArgc() != 2+skiparg)
		doAll = qtrue;
	Q_SayArgv( 1+skiparg, name, sizeof( name ) );
	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES) ||
				 (vic->client->ps.eFlags & EF_HEADSHOT))
				continue;
			// tjw: actually take of the players helmet instead of
			//      giving all players an unlimited supply of 
			//      helmets.
			vic->client->ps.eFlags |= EF_HEADSHOT;
			G_AddEvent( vic, EV_LOSE_HAT, DirToByte(dir) );
			count++;
		}
		AP(va("chat \"^opop: ^7%d players popped\"", count));
		return qtrue;
	}
	pcount = ClientNumbersFromString(name, pids);
	if(pcount > 1) {
		int it;
		for( it = 0; it < pcount; it++) {
			vic = &g_entities[pids[it]];
			if(!_shrubbot_admin_higher(ent, vic)) {
				SP( va("^opop: ^7sorry, but %s^7 intended "
					"victim has a higher admin"
					" level than you do.\n", 
					vic->client->pers.netname));
				continue;
			}
			if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
		 		vic->client->sess.sessionTeam == TEAM_ALLIES)) {
				SP(va("^opop: ^7%s^7 must be on a team"
					"to be popped\n", 
					vic->client->pers.netname));
				continue;
			}
			G_AddEvent( vic, EV_LOSE_HAT, DirToByte(dir) );
			AP(va("chat \"^opop: ^7%s ^7lost his hat\"", 
				vic->client->pers.netname));
		}
		return qtrue;
	} else if(pcount < 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^opop: ^7%s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, vic)) {
		SP( "^opop: ^7sorry, but your intended victim has a higher "
			"admin level than you do.\n" );
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
		 vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^opop: ^7player must be on a team to be popped\n");
		return qfalse;
	}
	
	G_AddEvent( vic, EV_LOSE_HAT, DirToByte(dir) );
	AP(va("chat \"^opop: ^7%s ^7lost his hat\"", 
			vic->client->pers.netname));
	return qtrue;
}

// Created by: dvl
qboolean G_shrubbot_pip( gentity_t *ent, int skiparg )
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;
	qboolean doAll = qfalse;
	int pcount, count = 0;

	if(Q_SayArgc() != 2+skiparg)
		doAll = qtrue;
	Q_SayArgv( 1+skiparg, name, sizeof( name ) );
	if( !Q_stricmp( name, "-1" ) || doAll ) {
		int it;
		for( it = 0; it < level.numConnectedClients; it++ ) {
			vic = g_entities + level.sortedClients[it];
			if( !_shrubbot_admin_higher( ent, vic ) ||
				!(vic->client->sess.sessionTeam == TEAM_AXIS ||
				  vic->client->sess.sessionTeam == TEAM_ALLIES))
				continue;
			G_MakePip( vic );
			count++;
		}
		AP(va("chat \"^opip: ^7%d players pipped\"", count));
		return qtrue;
	}
	pcount = ClientNumbersFromString(name, pids);
	if(pcount > 1) {
		int it;
		for( it = 0; it < pcount; it++) {
			vic = &g_entities[pids[it]];
			if(!_shrubbot_admin_higher(ent, vic)) {
				SP( va("^opip: ^7sorry, but %s^7 intended "
					"victim has a higher admin"
					"level than you do.\n", 
					vic->client->pers.netname));
				continue;
			}
			if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
		 		vic->client->sess.sessionTeam == TEAM_ALLIES)) {
				SP(va("^opip: ^7%s^7 must be on a team"
					" to be pipped\n", 
					vic->client->pers.netname));
				continue;
			}
			G_MakePip( vic );
			AP(va("chat \"^opip: ^7%s ^7was pipped\"", 
					vic->client->pers.netname));
		}
		return qtrue;
	} else if(pcount < 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^opip: ^7%s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("^opip: ^7sorry, but your intended victim has a higher"
			" admin level than you do.\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("^opip: ^7player must be on a team to be pipped\n");
		return qfalse;
	}
	
	G_MakePip( vic );
	AP(va("chat \"^opip: ^7%s ^7was pipped\"", vic->client->pers.netname));
	return qtrue;
}

qboolean G_shrubbot_reset(gentity_t *ent, int skiparg)
{
	char command[MAX_SHRUBBOT_CMD_LEN];

	Q_SayArgv(skiparg, command, sizeof(command));
	if(Q_stricmp(command, "reset")) {
		Svcmd_ResetMatch_f(qtrue, qtrue);
	}
	else {
		Svcmd_ResetMatch_f(qfalse, qtrue);
	}
	return qtrue;
}

qboolean G_shrubbot_swap(gentity_t *ent, int skiparg)
{
	Svcmd_SwapTeams_f();
	AP("chat \"^oswap: ^7Teams were swapped\"");
	return qtrue;
}

qboolean G_shrubbot_fling(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char fling[9], pastTense[9];
	char *flingCmd;
	gentity_t *vic;
	int flingType=-1; // 0 = fling, 1 = throw, 2 = launch
	int i, count = 0, pcount;
	qboolean doAll = qfalse;

	Q_SayArgv(0+skiparg, fling, sizeof(fling));
	if(fling[0] == '!') {
		flingCmd = &fling[1];
	} else {
		flingCmd = &fling[0];
	}

	if(!Q_stricmp(flingCmd, "throw")) {
		flingType = 1;
		Q_strncpyz(pastTense, "thrown", sizeof(pastTense));
	} else if (!Q_stricmp(flingCmd, "launch")) {
		flingType = 2;
		Q_strncpyz(pastTense, "launched", sizeof(pastTense));
	} else if (!Q_stricmp(flingCmd, "fling")) {
		flingType = 0;
		Q_strncpyz(pastTense, "flung", sizeof(pastTense));
	} else if (!Q_stricmp(flingCmd, "throwa")) {
		doAll = qtrue;
		flingType = 1;
		Q_strncpyz(pastTense, "thrown", sizeof(pastTense));
	} else if (!Q_stricmp(flingCmd, "launcha")) {
		doAll = qtrue;
		flingType = 2;
		Q_strncpyz(pastTense, "launched", sizeof(pastTense));
	} else if (!Q_stricmp(flingCmd, "flinga")) {
		doAll = qtrue;
		flingType = 0;
		Q_strncpyz(pastTense, "flung", sizeof(pastTense));
	}

	if( !doAll && Q_SayArgc() != 2+skiparg) {
		SP(va("%s usage: %s [name|slot#]\n", fling, fling));
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name)); 

	if( doAll ) {
		for( i = 0; i < level.numConnectedClients; i++ ) {
			vic = g_entities + level.sortedClients[i];
			if( !_shrubbot_admin_higher( ent, vic ) )
				continue;
			count += G_FlingClient( vic, flingType );
		}
		AP(va("chat \"^o%s: ^7%d players %s\"", 
					flingCmd, count, pastTense));
		return qtrue;
	}
	pcount = ClientNumbersFromString(name, pids);
	if(pcount > 1) {
		for( i = 0; i < pcount; i++) {
			vic = &g_entities[pids[i]];
			if( !_shrubbot_admin_higher( ent, vic ) )
				continue;
			count += G_FlingClient( vic, flingType );
		}
		AP(va("chat \"^o%s: ^7%d players %s\"", 
					flingCmd, count, pastTense));
		return qtrue;
	} else if(pcount < 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("^o%s: ^7%s\n", flingCmd, err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP(va("^o%s: ^7sorry, but your intended victim has a higher"
			" admin level than you do.\n",flingCmd));
		return qfalse;
	}
	
	if(G_FlingClient( vic, flingType ))
		AP(va("chat \"^o%s: ^7%s ^7was %s\"", 
				flingCmd,
				vic->client->pers.netname, 
				pastTense));
	return qtrue;
}


qboolean G_shrubbot_disorient(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;

	if(Q_SayArgc() < 2+skiparg) {
		SP("usage: !disorient [name|slot#] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = Q_SayConcatArgs(2+skiparg);

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("disorient: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("disorient: sorry, but your intended victim has a"
			"higher admin level than you do.\n");
		return qfalse;
	}
	if(!(vic->client->sess.sessionTeam == TEAM_AXIS ||
			vic->client->sess.sessionTeam == TEAM_ALLIES)) {
		SP("disorient: player must be on a team\n");
		return qfalse;
	}
	if(vic->client->pmext.disoriented) {
		SP(va("disorient: %s^7 is already disoriented\n",
			vic->client->pers.netname));
		return qfalse;
	}
	vic->client->pmext.disoriented = qtrue;
	AP(va("chat \"^odisorient: ^7%s ^7is disoriented\"",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^7disoriented you%s%s\"", 
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? " because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_orient(gentity_t *ent, int skiparg)
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic;

	if(Q_SayArgc() < 1+skiparg) {
		SP("usage: !disorient [name|slot#]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("orient: %s\n", err));
		return qfalse;
	}
	vic = &g_entities[pids[0]];

	if(!vic->client->pmext.disoriented) {
		SP(va("orient: %s^7 is not currently disoriented\n",
			vic->client->pers.netname));
		return qfalse;
	}
	if(vic->client->pmext.poisoned) {
		SP(va("orient: %s^7 is poisoned at the moment\n",
			vic->client->pers.netname));
		return qfalse;
	}
	vic->client->pmext.disoriented = qfalse;
	AP(va("chat \"^oorient: ^7%s ^7is no longer disoriented\"",
			vic->client->pers.netname));

	CPx(pids[0], va("cp \"%s ^7oriented you\"", 
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE")));
	return qtrue;
}

qboolean G_shrubbot_resetxp(gentity_t *ent, int skiparg) 
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	char *reason;
	gentity_t *vic;


	if(Q_SayArgc() < 2+skiparg) {
		SP("usage: !resetxp [name|slot#] [reason]\n");
		return qfalse;
	}
	Q_SayArgv(1+skiparg, name, sizeof(name));
	reason = ConcatArgs(2+skiparg);

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		SP(va("resetxp: %s\n", err));
		return qfalse;
	}
	if(!_shrubbot_admin_higher(ent, &g_entities[pids[0]])) {
		SP("resetxp: sorry, but your intended victim has a higher "
			" admin level than you do.\n");
		return qfalse;
	}
	vic = &g_entities[pids[0]];
	G_ResetXP(vic);

	AP(va("chat \"^oresetxp: ^7XP has been reset for player %s\"",
			vic->client->pers.netname));

	if(ent && (ent - g_entities) == pids[0])
		return qtrue;

	CPx(pids[0], va("cp \"%s^7 has reset your XP %s%s\"", 
			(ent?ent->client->pers.netname:"^3SERVER CONSOLE"),
			(*reason) ? " because:\n" : "",
			(*reason) ? reason : ""));
	return qtrue;
}

qboolean G_shrubbot_nextmap(gentity_t *ent, int skiparg)
{
	// copied from G_Nextmap_v() in g_vote.c
	g_campaignInfo_t *campaign;
#ifdef __ETE__
	if(g_gametype.integer == GT_WOLF_CAMPAIGN || g_gametype.integer == GT_SUPREMACY_CAMPAIGN) {
#else //!__ETE__
	if(g_gametype.integer == GT_WOLF_CAMPAIGN) {
#endif //__ETE__
		campaign = &g_campaigns[level.currentCampaign];
		if( campaign->current + 1 < campaign->mapCount ) {
			trap_Cvar_Set("g_currentCampaignMap",
					va( "%i", campaign->current + 1));
			trap_SendConsoleCommand(EXEC_APPEND,
				va( "map %s\n",
				campaign->mapnames[campaign->current + 1]));
			AP("cp \"^3*** Loading next map in campaign! ***\n\"");
		} else {
			// Load in the nextcampaign
			trap_SendConsoleCommand(EXEC_APPEND, 
					"vstr nextcampaign\n");
			AP("cp \"^3*** Loading nextcampaign! ***\n\"");
		}
	} else {
		// Load in the nextmap
		trap_SendConsoleCommand(EXEC_APPEND, "vstr nextmap\n");
		AP("cp \"^3*** Loading nextmap! ***\n\"");
	}
	AP("chat \"^onextmap: ^7Next map was loaded\"");
	return qtrue;
}


qboolean G_shrubbot_resetmyxp(gentity_t *ent, int skiparg) 
{
	if(!ent || !ent->client)
		return qfalse;
	G_ResetXP(ent);
	SP("resetmyxp: you have reset your XP\n");
	return qtrue;
}


/*
 * This function facilitates the SP define.  SP() is similar to CP except that
 * it prints the message to the server console if ent is not defined.
 */
void G_shrubbot_print(gentity_t *ent, char *m) 
{

	if(ent) CP(va("print \"%s\"", m));
	else {
		char m2[MAX_STRING_CHARS];
		DecolorString(m, m2);
		G_Printf(m2);
	}
}


void G_shrubbot_cleanup()
{
	int i = 0;

	for(i=0; g_shrubbot_levels[i]; i++) {
		free(g_shrubbot_levels[i]);
		g_shrubbot_levels[i] = NULL;
	}
	for(i=0; g_shrubbot_admins[i]; i++) {
		free(g_shrubbot_admins[i]);
		g_shrubbot_admins[i] = NULL;
	}
	for(i=0; g_shrubbot_bans[i]; i++) {
		free(g_shrubbot_bans[i]);
		g_shrubbot_bans[i] = NULL;
	}
}
