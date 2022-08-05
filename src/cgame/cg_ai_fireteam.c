/*
 * name:		cg_ai_fireteam.c
 *
 * desc:		UQ1: Dealing with AI and fireteam orders...
 *
*/

#include "cg_local.h"
#include "../game/bg_classes.h"

extern pmove_t cg_pmove;

qboolean
OnSameTeam ( centity_t *ent1, centity_t *ent2 )
{
	if ( ent1->currentState.eType != ET_PLAYER || ent2->currentState.eType != ET_PLAYER )
	{
		return ( qfalse );
	}

	if ( cgs.clientinfo[ent1->currentState.number].team == cgs.clientinfo[ent2->currentState.number].team )
	{
		return ( qtrue );
	}

	return ( qfalse );
}

#ifdef __BOT__
/*
// Commander menu toggle...
typedef enum {
	CM_TOGGLE_DEFAULT,
	CM_TOGGLE_GIVE_ORDERS,
	CM_TOGGLE_FT1,
	CM_TOGGLE_FT2,
	CM_TOGGLE_RETURN
}	fireteam_toggle_t;
*/
void CG_Draw_Commander_Order_Menu ( void )
{
	rectDef_t rect;
	char selected_option[64];

	switch ( cg.current_commander_orders_select )
	{
	case CM_TOGGLE_GIVE_ORDERS:
		strcpy(selected_option, "Give Orders");
		break;
	case CM_TOGGLE_FT1:
		strcpy(selected_option, "Group 1");
		break;
	case CM_TOGGLE_FT2:
		strcpy(selected_option, "Group 2");
		break;
	case CM_TOGGLE_RETURN:
		strcpy(selected_option, "Return To Me");
		break;
	default:
		strcpy(selected_option, "Alt Fire Mode");
		break;
	}

	rect.x = cg.hud.skillpics[0][0]+100;
	rect.y = cg.hud.skillpics[0][1]+20;
	rect.w = 110;
	rect.h = 10;

	CG_FillRect( rect.x, rect.y, rect.w, rect.h, popBG );
	CG_DrawRect_FixedBorder( rect.x, rect.y, rect.w, rect.h, POP_HUD_BORDERSIZE, popBorder );
	CG_Text_Paint_Ext( cg.hud.skillpics[0][0]+104, cg.hud.skillpics[0][1]+28, 0.15f, 0.15f, colorWhite, va("^3Alt Fire Mode^4: ^7%s", selected_option), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
}

void CG_Commander_Menu_Toggle ( void )
{
	cg.current_commander_orders_select++;
	
	// UQ1: TODO: Add groups... Skip these settings for now!
	if (cg.current_commander_orders_select == CM_TOGGLE_FT1)
		cg.current_commander_orders_select = CM_TOGGLE_RETURN;
		
	if (cg.current_commander_orders_select > CM_TOGGLE_RETURN)
		cg.current_commander_orders_select = CM_TOGGLE_DEFAULT;
}

void CG_Enemy_Near_Crosshair( qboolean forced)
{
	int			i;
	trace_t		tr;
	centity_t	*enemy = NULL;
	float		best_dist = 512.0f;
	vec3_t		trace_start_pos, trace_end_pos, selected_position;
	vec3_t		mins, maxs;

	VectorSet(mins, -128, -128, -128);
	VectorSet(maxs, 128, 128, 128);

	VectorCopy( cg.refdef.vieworg, trace_start_pos );
	VectorMA( trace_start_pos, 8192, cg.refdef.viewaxis[0], trace_end_pos );
	
	// Try large box trace first... Try to find an enemy iteelf with the trace...
	CG_Trace( &tr, trace_start_pos, mins, maxs, trace_end_pos, cg.clientNum, /*CONTENTS_SOLID|*/CONTENTS_BODY );

	if (tr.entityNum)
	{
		enemy = &cg_entities[tr.entityNum];

		if (enemy->currentState.eType == ET_PLAYER 
			&& enemy->currentState.number < MAX_CLIENTS
			//&& cgs.clientinfo[enemy->currentState.number].infoValid
			//&& cgs.clientinfo[enemy->currentState.number].health > 0
			&& cgs.clientinfo[enemy->currentState.number].team != TEAM_SPECTATOR
			&& !OnSameTeam(enemy, &cg_entities[cg.clientNum])
			&& !(enemy->currentState.eFlags & EF_DEAD)
			&& !(enemy->currentState.eFlags & EF_NODRAW) )
		{// This enemy will do us! Avoid the loop below!
			cg.current_commander_order_enemy = enemy;
			return;
		}
		else
		{// Reinitialize enemy...
			enemy = NULL;
			cg.current_commander_order_enemy = NULL;
		}
	}

	if (cg.current_commander_order_enemy_vis_time > cg.time && !forced)
	{
		return;
	}

	// Avoid too many traces...
	cg.current_commander_order_enemy_vis_time = cg.time + 50;

	// Try to find the gound position from the trace, then find an enemy close to that position...
	if (VectorDistance(tr.endpos, trace_end_pos) <= 8)
		CG_Trace( &tr, trace_start_pos, NULL, NULL, trace_end_pos, cg.clientNum, CONTENTS_SOLID|CONTENTS_BODY );

	VectorCopy(tr.endpos, selected_position);

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		float distance;
		centity_t *cent = &cg_entities[i];

		if (!cent)
			continue;

		if (cent->currentState.eType != ET_PLAYER)
			continue;

		if (cent->currentState.eFlags & EF_DEAD)
			continue;

		if (cent->currentState.eFlags & EF_NODRAW)
			continue;

		if (cgs.clientinfo[cent->currentState.number].team == TEAM_SPECTATOR)
			continue;

		if (OnSameTeam(cent, &cg_entities[cg.clientNum]))
			continue;

		//if (!cgs.clientinfo[cent->currentState.number].infoValid)
		//	continue;

		//if (cgs.clientinfo[cent->currentState.number].health <= 0)
		//	continue;

		distance = VectorDistance(cent->lerpOrigin, selected_position);

		if ( distance > best_dist)
			continue;
		
		best_dist = distance;
		enemy = cent;
	}

	cg.current_commander_order_enemy = enemy;
}

qboolean CG_BOT_FireTeam_Check_Orders(void)
{
	if (cg.current_commander_crosshair_spin)
		return ( qtrue );
	else 
		return ( qfalse );
}

qboolean CG_BOT_FireTeam_Orders(void)
{// Check on alt attack for fireteam orders...
	if (cg.current_commander_orders_select == CM_TOGGLE_DEFAULT)
	{// Bot orders not selected.. Use alt fire code...
		return ( qfalse );
	}

	if (cg.current_commander_orders_select == CM_TOGGLE_RETURN)
	{
		cg.current_commander_crosshair_spin = qtrue;
		return ( qtrue );
	}

	if (cg.current_commander_orders_select == CM_TOGGLE_FT1 || cg.current_commander_orders_select == CM_TOGGLE_FT2)
	{
		cg.current_commander_crosshair_spin = qtrue;
		return ( qtrue );
	}

	// Update crosshair enemy... (cg.current_commander_order_enemy)
	CG_Enemy_Near_Crosshair(qfalse);

	if (cg.current_commander_order_enemy && cg.current_commander_order_enemy->currentState.eType == ET_PLAYER)
	{// Enemy found near selected spot. Either need to assault (SPRINT HELD DOWN), or supress...
		VectorCopy(cg.current_commander_order_enemy->lerpOrigin, cg.current_commander_order_location);
		cg.current_commander_crosshair_spin = qtrue;
	}
	else
	{// Move to coverspot...
		// UQ1: Fixme, add visible position market...
		trace_t		tr;
		vec3_t		trace_start_pos, trace_end_pos;

		VectorCopy( cg.refdef.vieworg, trace_start_pos );
		VectorMA( trace_start_pos, 8192, cg.refdef.viewaxis[0], trace_end_pos );
		CG_Trace( &tr, trace_start_pos, NULL, NULL, trace_end_pos, cg.clientNum, MASK_SHOT|MASK_SOLID/*CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_PLAYERCLIP|CONTENTS_STRUCTURAL*/ );

		VectorMA( tr.endpos, -32, cg.refdef.viewaxis[0], cg.current_commander_order_location);

		cg.current_commander_order_enemy = NULL;
		cg.current_commander_crosshair_spin = qtrue;
	}

	cg.current_commander_crosshair_spin = qtrue;
	return ( qtrue );
}

void CG_BOT_FireTeam_Orders_Finalize ( void )
{
	if (cg.current_commander_order_time > cg.time)
		return;

	if (cg.current_commander_orders_select == CM_TOGGLE_RETURN)
	{
		trap_SendConsoleCommand( va("order %i\n", FT_ORDER_RETURN) );
		cg.current_commander_order_time = cg.time + 1000; // UQ1: 1 second OK???

		cg.current_commander_crosshair_spin = qfalse;
		return;
	}

	if (cg.current_commander_orders_select == CM_TOGGLE_FT1 || cg.current_commander_orders_select == CM_TOGGLE_FT2)
	{
		trap_SendConsoleCommand( va("order %i\n", FT_ORDER_TOGGLE_GROUP) );
		cg.current_commander_order_time = cg.time + 1000; // UQ1: 1 second OK???

		cg.current_commander_crosshair_spin = qfalse;
		return;
	}

	// Update crosshair enemy... (cg.current_commander_order_enemy)
	CG_Enemy_Near_Crosshair(qtrue);

	if (cg.current_commander_order_enemy && cg.current_commander_order_enemy->currentState.eType == ET_PLAYER)
	{// Enemy found near selected spot. Either need to assault (SPRINT HELD DOWN), or supress...
		if (cg_pmove.cmd.buttons & BUTTON_SPRINT)
		{// Assault enemy...
			trap_SendConsoleCommand( va("order %i %i\n", FT_ORDER_ASSAULT, cg.current_commander_order_enemy->currentState.number) );
			cg.current_commander_order_time = cg.time + 1000; // UQ1: 1 second OK???
		}
		else
		{// Supress...
			trap_SendConsoleCommand( va("order %i %i\n", FT_ORDER_SUPRESS, cg.current_commander_order_enemy->currentState.number) );
			cg.current_commander_order_time = cg.time + 1000; // UQ1: 1 second OK???
		}

		cg.current_commander_crosshair_spin = qfalse;
	}
	else
	{// Move to coverspot...
		trace_t		tr;
		vec3_t		trace_start_pos, trace_end_pos, forward, selected_position;

		VectorCopy(cg.refdef.vieworg, trace_start_pos);
		trace_start_pos[2]+=64;
		AngleVectors( cg.refdefViewAngles, forward, NULL, NULL );
		VectorMA(trace_start_pos, 64000, forward, trace_end_pos);
		CG_Trace( &tr, trace_start_pos, NULL, NULL, trace_end_pos, cg.clientNum, (MASK_SOLID|MASK_WATER) );
		VectorCopy(tr.endpos, selected_position);

		trap_SendConsoleCommand( va("order %i %f %f %f\n", FT_ORDER_COVER, selected_position[0], selected_position[1], selected_position[2]) );
		cg.current_commander_order_time = cg.time + 1000; // UQ1: 1 second OK???

		cg.current_commander_crosshair_spin = qfalse;
	}
}
#endif //__BOT__
