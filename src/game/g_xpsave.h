#ifndef _G_XPSAVE_H
#define _G_XPSAVE_H

#define MAX_XPSAVES 32768
#define MAX_MAPSTATS 1024

typedef struct {
	char guid[33];
	char name[MAX_NAME_LENGTH];
	int time;
	float skill[SK_NUM_SKILLS];
	int lives;
	int killrating;
	int playerrating;
	int muted;
	float hits;
	float team_hits;
} g_xpsave_t;

typedef struct {
	char name[MAX_QPATH];
	int allied_wins;
	int axis_wins;
	float skill_weight;
	float map_weight;
} g_mapstat_t;

void G_xpsave_writeconfig();
void G_xpsave_readconfig();
qboolean G_xpsave_add(gentity_t *ent);
qboolean G_xpsave_load(gentity_t *ent);
g_mapstat_t *G_xpsave_mapstat(char *mapname);
void G_xpsave_resetxp();
void G_xpsave_cleanup();
#endif /* ifndef _G_XPSAVE_H */
