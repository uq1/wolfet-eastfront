#ifndef _G_SHRUBBOT_H
#define _G_SHRUBBOT_H

#define MAX_SHRUBBOT_LEVELS 32
// tjw: some nutters !setlevel every player in sight
#define MAX_SHRUBBOT_ADMINS 32768
#define MAX_SHRUBBOT_BANS 1024
#define MAX_SHRUBBOT_FLAGS 64

#define MAX_SHRUBBOT_CMD_LEN 17

/*
 * 1 - cannot be vote kicked, vote muted, dropped from inactivity, dropped 
 * due to high team damage ratio, or complained against
 * 2 - cannot be censored or flood protected
 * 3 - can run commands "silently" through the console (/!commandname 
 * parameters)
 * 4 - can see Axis' and Allies' team chat as a spectator
 * 5 - can switch teams any time, regardless of balance
 * 6 - does not need to specify a reason for a kick/ban
 * 7 - can call a vote at any time (regardless of a vote being disabled or 
 * voting limitations)
 * 8 - does not need to specify a duration for a ban
 * 9 - can run commands from team and fireteam chat (as opposed to global chat)
 */
#define SBF_IMMUNITY '1'
#define SBF_NOCENSORFLOOD '2'
#define SBF_STEALTH '3'
#define SBF_SPEC_ALLCHAT '4'
#define SBF_FORCETEAMCHANGE '5'
#define SBF_UNACCOUNTABLE '6'
#define SBF_NO_VOTE_LIMIT '7'
#define SBF_CAN_PERM_BAN '8'
#define SBF_TEAMFTSHRUBCMD '9'

// tjw: used by g_shrubbot_cmd.cmdFlags
#define SCMDF_TYRANNY 1

// expires field seems to be offset by (30 years - 54 hours)...  go figure
#define SHRUBBOT_BAN_EXPIRE_OFFSET 946490400
#define SHRUBBOT_MAX_LOL_NADES 16
#define SHRUBBOT_MAX_SHOWBANS 30

struct g_shrubbot_cmd {
	const char *keyword;
	qboolean (* const handler)(gentity_t *ent, int skiparg);
	char flag;
	int cmdFlags; // see SCMDF_ defines
};

typedef struct g_shrubbot_level {
	int level;
	char name[MAX_NAME_LENGTH];
	char flags[MAX_SHRUBBOT_FLAGS];
} g_shrubbot_level_t;

typedef struct g_shrubbot_admin {
	char guid[33];
	char name[MAX_NAME_LENGTH];
	int level;
	char flags[MAX_SHRUBBOT_FLAGS];
} g_shrubbot_admin_t;

typedef struct g_shrubbot_ban {
	char name[MAX_NAME_LENGTH];
	char guid[33];
	char ip[18];
	char reason[MAX_STRING_CHARS];
	char made[50]; // big enough for strftime() %c
	int expires;
	char banner[MAX_NAME_LENGTH];
} g_shrubbot_ban_t;

qboolean G_shrubbot_ban_check(char *userinfo);
qboolean G_shrubbot_cmd_check(gentity_t *ent);
qboolean G_shrubbot_readconfig(gentity_t *ent, int skiparg);
qboolean G_shrubbot_time(gentity_t *ent, int skiparg);
qboolean G_shrubbot_setlevel(gentity_t *ent, int skiparg);
qboolean G_shrubbot_kick(gentity_t *ent, int skiparg);
qboolean G_shrubbot_ban(gentity_t *ent, int skiparg);
qboolean G_shrubbot_unban(gentity_t *ent, int skiparg);
qboolean G_shrubbot_putteam(gentity_t *ent, int skiparg);
qboolean G_shrubbot_pause(gentity_t *ent, int skiparg);
qboolean G_shrubbot_unpause(gentity_t *ent, int skiparg);
qboolean G_shrubbot_listplayers(gentity_t *ent, int skiparg);
qboolean G_shrubbot_mute(gentity_t *ent, int skiparg);
qboolean G_shrubbot_showbans(gentity_t *ent, int skiparg);
qboolean G_shrubbot_help(gentity_t *ent, int skiparg);
qboolean G_shrubbot_admintest(gentity_t *ent, int skiparg);
qboolean G_shrubbot_cancelvote(gentity_t *ent, int skiparg);
qboolean G_shrubbot_passvote(gentity_t *ent, int skiparg);
qboolean G_shrubbot_spec999(gentity_t *ent, int skiparg);
qboolean G_shrubbot_shuffle(gentity_t *ent, int skiparg);
qboolean G_shrubbot_rename(gentity_t *ent, int skiparg);
qboolean G_shrubbot_gib(gentity_t *ent, int skiparg);
qboolean G_shrubbot_slap(gentity_t *ent, int skiparg);
qboolean G_shrubbot_burn(gentity_t *ent, int skiparg);
qboolean G_shrubbot_warn(gentity_t *ent, int skiparg);
qboolean G_shrubbot_news(gentity_t *ent, int skiparg);
qboolean G_shrubbot_lock(gentity_t *ent, int skiparg);
qboolean G_shrubbot_unlock(gentity_t *ent, int skiparg);
qboolean G_shrubbot_lockteams(gentity_t *ent, int skiparg, qboolean toLock);
qboolean G_shrubbot_lol(gentity_t *ent, int skiparg);
qboolean G_shrubbot_pip(gentity_t *ent, int skiparg);
qboolean G_shrubbot_pop(gentity_t *ent, int skiparg);
qboolean G_shrubbot_reset(gentity_t *ent, int skiparg);
qboolean G_shrubbot_fling(gentity_t *ent, int skiparg);
qboolean G_shrubbot_listteams(gentity_t *ent, int skiparg);
qboolean G_shrubbot_disorient(gentity_t *ent, int skiparg);
qboolean G_shrubbot_orient(gentity_t *ent, int skiparg);
qboolean G_shrubbot_resetxp(gentity_t *ent, int skiparg);
qboolean G_shrubbot_swap(gentity_t *ent, int skiparg);
qboolean G_shrubbot_nextmap(gentity_t *ent, int skiparg);
qboolean G_shrubbot_resetmyxp(gentity_t *ent, int skiparg);

qboolean G_shrubbot_permission(gentity_t *ent, char flag);
void G_shrubbot_print(gentity_t *ent, char *m);

void G_shrubbot_readconfig_string(char **cnf, char *s, int size);
void G_shrubbot_readconfig_int(char **cnf, int *v);
void G_shrubbot_readconfig_float(char **cnf, float *v);
void G_shrubbot_writeconfig_string(char *s, fileHandle_t f);
void G_shrubbot_writeconfig_int(int v, fileHandle_t f);
void G_shrubbot_writeconfig_float(float v, fileHandle_t f);
void G_shrubbot_duration(int secs, char *duration, int dursize);
void G_shrubbot_cleanup();
#endif /* ifndef _G_SHRUBBOT_H */
