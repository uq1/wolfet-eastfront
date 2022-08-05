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

#ifdef __BOT__

#include			"../botai/ai_main.h"

#ifdef __BOT_AAS__
extern int			BotGetArea ( int entnum );
int					next_aas_position_time[MAX_CLIENTS];
extern bot_state_t	botstates[MAX_CLIENTS];
extern void			BotRoamGoal ( bot_state_t *bs, vec3_t goal );
extern void			trap_BotUpdateEntityItems ( void );
extern int			AINode_MP_Seek_NBG ( bot_state_t *bs );
#include			"../game/botlib.h"
#include			"../game/be_ai_move.h"
#endif //__BOT_AAS__

#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE	150
#endif //MAX_MESSAGE_SIZE

extern vmCvar_t		bot_skill;
extern vmCvar_t		bot_cpu_use;
extern vmCvar_t		bot_forced_auto_waypoint;

extern vec3_t		botTraceMins;
extern vec3_t		botTraceMaxs;

extern int			next_weapon_change[MAX_CLIENTS];
extern int			no_mount_time[MAX_CLIENTS]; // Don't try to mount emplaced for so many secs...
extern int			next_arty[MAX_CLIENTS];

#ifdef __VEHICLES__
extern void			AIMOD_MOVEMENT_VehicleAttack ( gentity_t *bot, usercmd_t *ucmd );
#endif //__VEHICLES__

extern float		HeightDistance ( vec3_t v1, vec3_t v2 );
extern float		VectorDistanceNoHeight ( vec3_t v1, vec3_t v2 );
extern int			NodeVisible ( vec3_t org1, vec3_t org2, int ignore );
extern int			SP_NodeVisible ( vec3_t org1, vec3_t org2, int ignore ); // below
extern void			G_UcmdMoveForDir ( gentity_t *self, usercmd_t *cmd, vec3_t dir );
extern void			BotSetViewAngles ( gentity_t *bot, float thinktime );
extern float		TravelTime ( gentity_t *bot );
extern /*short*/ int	BotGetNextNode ( gentity_t *bot /*, int *state*/ ); // below...
extern qboolean		AIMOD_MOVEMENT_WaypointTouched ( gentity_t *bot, int touch_node, qboolean onLadder );
extern qboolean		Can_Arty ( gentity_t *ent, vec3_t origin );
extern void			Weapon_Artillery ( gentity_t *ent );
extern void			AIMOD_InformTeamOfArty ( gentity_t *bot );
extern void			AIMOD_InformTeamOfAirstrike ( gentity_t *bot );
extern void			AIMOD_InformTeamOfGrenade ( gentity_t *bot );
extern void			AIMOD_InformTeamOfEnemy ( gentity_t *bot );
extern qboolean		Teammate_SpashDamage_OK ( gentity_t *ent, vec3_t origin );
extern void			AIMOD_SP_MOVEMENT_Move ( gentity_t *bot, usercmd_t *ucmd );
extern void			AIMOD_MOVEMENT_Attack ( gentity_t *bot, usercmd_t *ucmd );
extern void			AIMOD_MOVEMENT_AttackMove ( gentity_t *bot, usercmd_t *ucmd );
extern void			AIMOD_MOVEMENT_CoverSpot_AttackMove ( gentity_t *bot, usercmd_t *ucmd );
extern void			AIMOD_SP_MOVEMENT_CoverSpot_Movement ( gentity_t *bot, usercmd_t *ucmd );
extern void			AIMOD_MOVEMENT_SetupStealth ( gentity_t *bot, usercmd_t *ucmd );
extern qboolean		AIMOD_HaveAmmoForWeapon ( playerState_t *ps, weapon_t wep );
extern void			BOT_ChangeWeapon( gentity_t *bot, int weaponChoice );
extern void			Set_Best_AI_weapon ( gentity_t *bot );
extern int			BotMP_Num_Defenders_Near ( vec3_t pos, gentity_t *bot );
extern void			AIMOD_MOVEMENT_Setup_Strafe ( gentity_t *bot, usercmd_t *ucmd );
extern int			JobType ( gentity_t *bot );

#ifdef __ETE__
vmCvar_t			g_realmedic;
vmCvar_t			g_noSelfHeal;
#endif //__ETE__

#endif //__BOT__
