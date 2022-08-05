// bg_slidemove.c -- part of bg_pmove functionality

#include "q_shared.h"
#include "bg_public.h"
#include "bg_local.h"

#ifdef CGAMEDLL
#include "../cgame/cg_local.h"
#else
#include "q_shared.h"
#include "bg_public.h"
#endif // CGAMEDLL

#ifdef GAMEDLL
#include "g_local.h"
#endif //GAMEDLL

/*

input: origin, velocity, bounds, groundPlane, trace function

output: origin, velocity, impacts, stairup boolean

*/

#ifdef __VEHICLES__
#ifdef CGAMEDLL
extern void CG_RailTrail2( clientInfo_t *ci, vec3_t start, vec3_t end );
#endif //CGAMEDLL

float GroundNormalAt ( vec3_t org, int ignore_ent )
{// UQ1: improved it again, now checks slope here in a faster way that also stops multiple checks later! :) Also add mover check :)
	trace_t tr;
	vec3_t org1, org2, slopeangles;
	float pitch = 0;

	VectorCopy(org, org1);
	org1[2]+=48;

	VectorCopy(org, org2);
	org2[2]= -65536.0f;

	pm->trace( &tr, org1, NULL, NULL, org2, ignore_ent, MASK_PLAYERSOLID );

	// Added -- Check slope...
	vectoangles( tr.plane.normal, slopeangles );

	pitch = slopeangles[0];
	
	if (pitch > 180)
		pitch -= 360;

	if (pitch < -180)
		pitch += 360;

	pitch += 90.0f;

	return pitch;
}

//#define TANK_HEIGHT 72
#define TANK_HEIGHT 56 //64
//#define TANK_HALF_WIDTH 64
#define TANK_HALF_WIDTH 56//64 //48 //32
#define MEDIUM_TANK_HALF_WIDTH 48 //48 //32
#define LIGHT_TANK_HALF_WIDTH 32 //48 //32
#define APC_HALF_WIDTH 32 //48 //32

#define TANK_FORWARD_SIZE 144 //192 //224 //192
#define HEAVY_TANK_BACK_SIZE 224 //288
#define MEDIUM_TANK_BACK_SIZE 192 //256
#define LIGHT_TANK_BACK_SIZE 144 //224

//#defing __DEBUG_VEHICLE_BOXES__

qboolean	PM_VehicleTraces ( float time_left, trace_t tr )
{
	qboolean movefail = qfalse;
	vec3_t mins, maxs, sides_mins, sides_maxs;
	float normal = GroundNormalAt( pm->ps->origin, pm->ps->clientNum );
	float TRACE_HEIGHT_MULTIPLIER = 0.85;
	float THIS_TANK_HALF_WIDTH = TANK_HALF_WIDTH;

#ifndef CGAMEDLL
	if (g_entities[pm->ps->clientNum].turret_ent)
	{
		//G_Printf("Removing turret...\n");
		trap_UnlinkEntity( g_entities[pm->ps->clientNum].turret_ent );
	}
#endif //CGAMEDLL

	switch (pm->ps->stats[STAT_VEHICLE_CLASS])
	{
	case VEHICLE_CLASS_APC:
		// UQ1: Used for center and front/back traces...
		VectorSet(mins, 0-(APC_HALF_WIDTH*0.5), 0-(APC_HALF_WIDTH*0.5), -(TANK_HEIGHT*0.5));
		VectorSet(maxs, (APC_HALF_WIDTH*0.5), (APC_HALF_WIDTH*0.5), (TANK_HEIGHT*0.5));

		// UQ1: Used for right and left side traces...
		VectorSet(sides_mins, 0-(APC_HALF_WIDTH*0.25), 0-(APC_HALF_WIDTH*0.25), -(TANK_HEIGHT*0.5));
		VectorSet(sides_maxs, (APC_HALF_WIDTH*0.25), (APC_HALF_WIDTH*0.25), (TANK_HEIGHT*0.5));

		THIS_TANK_HALF_WIDTH = APC_HALF_WIDTH;
		break;
	case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
	case VEHICLE_CLASS_HEAVY_TANK:
		// UQ1: Used for center and front/back traces...
		VectorSet(mins, 0-(TANK_HALF_WIDTH*0.5), 0-(TANK_HALF_WIDTH*0.5), -(TANK_HEIGHT*0.5));
		VectorSet(maxs, (TANK_HALF_WIDTH*0.5), (TANK_HALF_WIDTH*0.5), (TANK_HEIGHT*0.5));

		// UQ1: Used for right and left side traces...
		VectorSet(sides_mins, 0-(TANK_HALF_WIDTH*0.25), 0-(TANK_HALF_WIDTH*0.25), -(TANK_HEIGHT*0.5));
		VectorSet(sides_maxs, (TANK_HALF_WIDTH*0.25), (TANK_HALF_WIDTH*0.25), (TANK_HEIGHT*0.5));

		THIS_TANK_HALF_WIDTH = TANK_HALF_WIDTH;
		break;
	case VEHICLE_CLASS_MEDIUM_TANK:
	case VEHICLE_CLASS_FLAMETANK:
		// UQ1: Used for center and front/back traces...
		VectorSet(mins, 0-(MEDIUM_TANK_HALF_WIDTH*0.5), 0-(MEDIUM_TANK_HALF_WIDTH*0.5), -(TANK_HEIGHT*0.5));
		VectorSet(maxs, (MEDIUM_TANK_HALF_WIDTH*0.5), (MEDIUM_TANK_HALF_WIDTH*0.5), (TANK_HEIGHT*0.5));

		// UQ1: Used for right and left side traces...
		VectorSet(sides_mins, 0-(MEDIUM_TANK_HALF_WIDTH*0.25), 0-(MEDIUM_TANK_HALF_WIDTH*0.25), -(TANK_HEIGHT*0.5));
		VectorSet(sides_maxs, (MEDIUM_TANK_HALF_WIDTH*0.25), (MEDIUM_TANK_HALF_WIDTH*0.25), (TANK_HEIGHT*0.5));

		THIS_TANK_HALF_WIDTH = MEDIUM_TANK_HALF_WIDTH;
		break;
	default:
		// UQ1: Used for center and front/back traces...
		VectorSet(mins, 0-(LIGHT_TANK_HALF_WIDTH*0.5), 0-(LIGHT_TANK_HALF_WIDTH*0.5), -(TANK_HEIGHT*0.5));
		VectorSet(maxs, (LIGHT_TANK_HALF_WIDTH*0.5), (LIGHT_TANK_HALF_WIDTH*0.5), (TANK_HEIGHT*0.5));

		// UQ1: Used for right and left side traces...
		VectorSet(sides_mins, 0-(LIGHT_TANK_HALF_WIDTH*0.25), 0-(LIGHT_TANK_HALF_WIDTH*0.25), -(TANK_HEIGHT*0.5));
		VectorSet(sides_maxs, (LIGHT_TANK_HALF_WIDTH*0.25), (LIGHT_TANK_HALF_WIDTH*0.25), (TANK_HEIGHT*0.5));

		THIS_TANK_HALF_WIDTH = LIGHT_TANK_HALF_WIDTH;
		break;
	}

	

	if (pm->cmd.forwardmove == 0)
	{
#ifndef CGAMEDLL
		if (g_entities[pm->ps->clientNum].turret_ent)
		{
			trap_LinkEntity(g_entities[pm->ps->clientNum].turret_ent);
		}
#endif //CGAMEDLL
		return qfalse;
	}

	if (pm->ps->eFlags & EF_VEHICLE)
	{// Check the center!
		//trace_t tr;
		vec3_t  end2, origin2, forward, right, up, use_origin, angles;
#ifdef CGAMEDLL
#ifdef __DEBUG_VEHICLE_BOXES__
		vec3_t  end3;
#endif //__DEBUG_VEHICLE_BOXES__
#endif //CGAMEDLL

		VectorCopy(pm->ps->origin, use_origin);
		use_origin[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

		VectorCopy(pm->ps->origin, origin2);
		origin2[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

		// Start: Calculate forward and right...
#ifdef CGAMEDLL
		VectorCopy(cg_entities[pm->ps->clientNum].lerpAngles, angles);
#else //!CGAMEDLL
		//VectorCopy(g_entities[pm->ps->clientNum].s.angles, angles);
		VectorCopy(pm->ps->viewangles, angles);
#endif //CGAMEDLL
		//VectorCopy(pm->ps->viewangles, angles);
		angles[PITCH]=normal;//0;
		angles[ROLL]=0;
		AngleVectors (angles, forward, NULL, NULL);
		AngleVectors (angles, NULL, right, NULL);
		AngleVectors (angles, NULL, NULL, up);
		forward[2] = 0;
		right[2] = 0;
		up[2] = 0;
		// End: Calculate forward and right...
			
		if (pm->cmd.forwardmove > 0)
		{
			pm->pml_tank_forwardmove = qtrue;
			VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
		}
		else if (pm->cmd.forwardmove < 0)
		{
			pm->pml_tank_forwardmove = qfalse;

			switch (pm->ps->stats[STAT_VEHICLE_CLASS])
			{
			case VEHICLE_CLASS_HEAVY_TANK:
			case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
				VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
				break;
			case VEHICLE_CLASS_MEDIUM_TANK:
			case VEHICLE_CLASS_FLAMETANK:
				VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
				break;
			default:
				VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
				break;
			}
			
		}
		else if (pm->pml_tank_forwardmove)
		{
			VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
		}
		else
		{
			switch (pm->ps->stats[STAT_VEHICLE_CLASS])
			{
			case VEHICLE_CLASS_HEAVY_TANK:
				VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
				break;
			case VEHICLE_CLASS_MEDIUM_TANK:
			case VEHICLE_CLASS_FLAMETANK:
				VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
				break;
			default:
				VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
				break;
			}
			//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
		}

		// calculate position we are trying to move to
		VectorMA( use_origin, time_left, pm->ps->velocity, end2 );

		//PM_TraceAll( &tr, origin2, end2 );
		pm->trace(&tr, origin2, mins, maxs/*NULL, NULL*/, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
#ifdef __DEBUG_VEHICLE_BOXES__
		CG_RailTrail2( NULL, origin2, end2 );
#endif //__DEBUG_VEHICLE_BOXES__
#endif //CGAMEDLL

		if (tr.fraction < 1)
		{
			// return qfalse;
			movefail = qtrue;
		}
		//else
		{// Check the right hand side!
			VectorCopy(pm->ps->origin, use_origin);
			use_origin[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

			VectorCopy(pm->ps->origin, origin2);
			origin2[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

			if (pm->cmd.forwardmove > 0)
			{
				VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
			}
			else if (pm->cmd.forwardmove < 0)
			{
				switch (pm->ps->stats[STAT_VEHICLE_CLASS])
				{
				case VEHICLE_CLASS_HEAVY_TANK:
				case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
					VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
					break;
				case VEHICLE_CLASS_MEDIUM_TANK:
				case VEHICLE_CLASS_FLAMETANK:
					VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
					break;
				default:
					VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
					break;
				}
				//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
			}
			else if (pm->pml_tank_forwardmove)
			{
				VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
			}
			else
			{
				switch (pm->ps->stats[STAT_VEHICLE_CLASS])
				{
				case VEHICLE_CLASS_HEAVY_TANK:
				case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
					VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
					break;
				case VEHICLE_CLASS_MEDIUM_TANK:
				case VEHICLE_CLASS_FLAMETANK:
					VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
					break;
				default:
					VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
					break;
				}
				//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
			}

			// calculate position we are trying to move to
			VectorMA( use_origin, time_left, pm->ps->velocity, end2 );

			//VectorMA( use_origin, 64, right, use_origin );
			VectorMA( origin2, THIS_TANK_HALF_WIDTH, right, origin2 );
			VectorMA( end2, THIS_TANK_HALF_WIDTH, right, end2 );

			//PM_TraceAll( &tr, origin2, end2 );
			pm->trace(&tr, origin2, sides_mins, sides_maxs/*NULL, NULL*/, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
#ifdef __DEBUG_VEHICLE_BOXES__
			CG_RailTrail2( NULL, origin2, end2 );
			VectorCopy(end2, end3);
#endif //__DEBUG_VEHICLE_BOXES__
#endif //CGAMEDLL

			if (tr.fraction < 1)
			{
#ifndef CGAMEDLL
				if (g_entities[pm->ps->clientNum].turret_ent)
				{
					trap_LinkEntity(g_entities[pm->ps->clientNum].turret_ent);
				}
#endif //CGAMEDLL
				return qfalse;
			}
			//else
			{// Check the left hand side!
				VectorCopy(pm->ps->origin, use_origin);
				use_origin[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

				VectorCopy(pm->ps->origin, origin2);
				origin2[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

				if (pm->cmd.forwardmove > 0)
				{
					VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
				}
				else if (pm->cmd.forwardmove < 0)
				{
					switch (pm->ps->stats[STAT_VEHICLE_CLASS])
					{
					case VEHICLE_CLASS_HEAVY_TANK:
					case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
						VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
						break;
					case VEHICLE_CLASS_MEDIUM_TANK:
					case VEHICLE_CLASS_FLAMETANK:
						VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
						break;
					default:
						VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
						break;
					}
					//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
				}
				else if (pm->pml_tank_forwardmove)
				{
					VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
				}
				else
				{
					switch (pm->ps->stats[STAT_VEHICLE_CLASS])
					{
					case VEHICLE_CLASS_HEAVY_TANK:
					case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
						VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
						break;
					case VEHICLE_CLASS_MEDIUM_TANK:
					case VEHICLE_CLASS_FLAMETANK:
						VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
						break;
					default:
						VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
						break;
					}
					//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
				}

				// calculate position we are trying to move to
				VectorMA( use_origin, time_left, pm->ps->velocity, end2 );

				//VectorMA( use_origin, -64, right, use_origin );
				VectorMA( origin2, -THIS_TANK_HALF_WIDTH, right, origin2 );
				VectorMA( end2, -THIS_TANK_HALF_WIDTH, right, end2 );

				//PM_TraceAll( &tr, origin2, end2 );
				pm->trace(&tr, origin2, sides_mins, sides_maxs/*NULL, NULL*/, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
#ifdef __DEBUG_VEHICLE_BOXES__
				CG_RailTrail2( NULL, origin2, end2 );
				CG_RailTrail2( NULL, end2, end3 );
#endif //__DEBUG_VEHICLE_BOXES__
#endif //CGAMEDLL

				if (tr.fraction < 1)
				{
					movefail = qtrue; 
				}
			}
		}
	}

	if ((pm->ps->eFlags & EF_VEHICLE) && movefail)
	{// Check the center!
		//trace_t tr;
		vec3_t  end2, origin2, forward, right, up, use_origin, angles;
#ifdef CGAMEDLL
#ifdef __DEBUG_VEHICLE_BOXES__
		vec3_t  end3;
#endif //__DEBUG_VEHICLE_BOXES__
#endif //CGAMEDLL

		movefail = qfalse;

		VectorCopy(pm->ps->origin, use_origin);
		use_origin[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

		VectorCopy(pm->ps->origin, origin2);
		origin2[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

		// Start: Calculate forward and right...
#ifdef CGAMEDLL
		VectorCopy(cg_entities[pm->ps->clientNum].lerpAngles, angles);
#else //!CGAMEDLL
		//VectorCopy(g_entities[pm->ps->clientNum].s.angles, angles);
		VectorCopy(pm->ps->viewangles, angles);
#endif //CGAMEDLL
		//VectorCopy(pm->ps->viewangles, angles);
		angles[PITCH]=normal;//0;
		angles[ROLL]=0;
		AngleVectors (angles, forward, NULL, NULL);
		AngleVectors (angles, NULL, right, NULL);
		AngleVectors (angles, NULL, NULL, up);
		forward[2] = 0;
		right[2] = 0;
		up[2] = 0;
		// End: Calculate forward and right...
			
		if (pm->cmd.forwardmove > 0)
		{
			pm->pml_tank_forwardmove = qtrue;
			VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
		}
		else if (pm->cmd.forwardmove < 0)
		{
			pm->pml_tank_forwardmove = qfalse;
			switch (pm->ps->stats[STAT_VEHICLE_CLASS])
			{
			case VEHICLE_CLASS_HEAVY_TANK:
			case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
				VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
				break;
			case VEHICLE_CLASS_MEDIUM_TANK:
			case VEHICLE_CLASS_FLAMETANK:
				VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
				break;
			default:
				VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
				break;
			}
			//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
		}
		else if (pm->pml_tank_forwardmove)
		{
			VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
		}
		else
		{
			switch (pm->ps->stats[STAT_VEHICLE_CLASS])
			{
			case VEHICLE_CLASS_HEAVY_TANK:
			case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
				VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
				break;
			case VEHICLE_CLASS_MEDIUM_TANK:
			case VEHICLE_CLASS_FLAMETANK:
				VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
				break;
			default:
				VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
				break;
			}
			//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
		}

		// calculate position we are trying to move to
		VectorMA( use_origin, time_left, pm->ps->velocity, end2 );

		//PM_TraceAll( &tr, origin2, end2 );
		pm->trace(&tr, origin2, mins, maxs/*NULL, NULL*/, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
#ifdef __DEBUG_VEHICLE_BOXES__
		CG_RailTrail2( NULL, origin2, end2 );
#endif //__DEBUG_VEHICLE_BOXES__
#endif //CGAMEDLL

		if (tr.fraction < 1)
		{
			// return qfalse;
			movefail = qtrue;
		}
		else
		{// Check the right hand side!
			VectorCopy(pm->ps->origin, use_origin);
			use_origin[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

			VectorCopy(pm->ps->origin, origin2);
			origin2[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

			if (pm->cmd.forwardmove > 0)
			{
				VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
			}
			else if (pm->cmd.forwardmove < 0)
			{
				switch (pm->ps->stats[STAT_VEHICLE_CLASS])
				{
				case VEHICLE_CLASS_HEAVY_TANK:
				case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
					VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
					break;
				case VEHICLE_CLASS_MEDIUM_TANK:
				case VEHICLE_CLASS_FLAMETANK:
					VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
					break;
				default:
					VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
					break;
				}
				//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
			}
			else if (pm->pml_tank_forwardmove)
			{
				VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
			}
			else
			{
				switch (pm->ps->stats[STAT_VEHICLE_CLASS])
				{
				case VEHICLE_CLASS_HEAVY_TANK:
				case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
					VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
					break;
				case VEHICLE_CLASS_MEDIUM_TANK:
				case VEHICLE_CLASS_FLAMETANK:
					VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
					break;
				default:
					VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
					break;
				}
				//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
			}

			// calculate position we are trying to move to
			VectorMA( use_origin, time_left, pm->ps->velocity, end2 );

			//VectorMA( use_origin, 64, right, use_origin );
			VectorMA( origin2, THIS_TANK_HALF_WIDTH, right, origin2 );
			VectorMA( end2, THIS_TANK_HALF_WIDTH, right, end2 );

			//PM_TraceAll( &tr, origin2, end2 );
			pm->trace(&tr, origin2, sides_mins, sides_maxs/*NULL, NULL*/, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
#ifdef __DEBUG_VEHICLE_BOXES__
			CG_RailTrail2( NULL, origin2, end2 );
			VectorCopy(end2, end3);
#endif //__DEBUG_VEHICLE_BOXES__
#endif //CGAMEDLL

			if (tr.fraction < 1)
			{
#ifndef CGAMEDLL
				if (g_entities[pm->ps->clientNum].turret_ent)
				{
					trap_LinkEntity(g_entities[pm->ps->clientNum].turret_ent);
				}
#endif //CGAMEDLL
				return qfalse;
			}
			else
			{// Check the left hand side!
				VectorCopy(pm->ps->origin, use_origin);
				use_origin[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

				VectorCopy(pm->ps->origin, origin2);
				origin2[2]+=(TANK_HEIGHT * TRACE_HEIGHT_MULTIPLIER);

				if (pm->cmd.forwardmove > 0)
				{
					VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
				}
				else if (pm->cmd.forwardmove < 0)
				{
					switch (pm->ps->stats[STAT_VEHICLE_CLASS])
					{
					case VEHICLE_CLASS_HEAVY_TANK:
					case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
						VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
						break;
					case VEHICLE_CLASS_MEDIUM_TANK:
					case VEHICLE_CLASS_FLAMETANK:
						VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
						break;
					default:
						VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
						break;
					}
					//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
				}
				else if (pm->pml_tank_forwardmove)
				{
					VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
				}
				else
				{
					switch (pm->ps->stats[STAT_VEHICLE_CLASS])
					{
					case VEHICLE_CLASS_HEAVY_TANK:
					case VEHICLE_CLASS_HEAVY_ASSAULT_TANK:
						VectorMA( use_origin, 0-HEAVY_TANK_BACK_SIZE, forward, use_origin );
						break;
					case VEHICLE_CLASS_MEDIUM_TANK:
					case VEHICLE_CLASS_FLAMETANK:
						VectorMA( use_origin, 0-MEDIUM_TANK_BACK_SIZE, forward, use_origin );
						break;
					default:
						VectorMA( use_origin, 0-LIGHT_TANK_BACK_SIZE, forward, use_origin );
						break;
					}
					//VectorMA( use_origin, TANK_FORWARD_SIZE, forward, use_origin );
				}

				// calculate position we are trying to move to
				VectorMA( use_origin, time_left, pm->ps->velocity, end2 );

				//VectorMA( use_origin, -64, right, use_origin );
				VectorMA( origin2, -THIS_TANK_HALF_WIDTH, right, origin2 );
				VectorMA( end2, -THIS_TANK_HALF_WIDTH, right, end2 );

				//PM_TraceAll( &tr, origin2, end2 );
				pm->trace(&tr, origin2, sides_mins, sides_maxs/*NULL, NULL*/, end2, pm->ps->clientNum, pm->tracemask);
#ifdef CGAMEDLL
#ifdef __DEBUG_VEHICLE_BOXES__
				CG_RailTrail2( NULL, origin2, end2 );
				CG_RailTrail2( NULL, end2, end3 );
#endif //__DEBUG_VEHICLE_BOXES__
#endif //CGAMEDLL

				if (tr.fraction < 1)
				{
					//return qfalse;
					movefail = qtrue;
				}
			}
		}
	}

	if (movefail)
	{
		pm->pml_tank_movetime = 1;
#ifndef CGAMEDLL
		if (g_entities[pm->ps->clientNum].turret_ent)
		{
			trap_LinkEntity(g_entities[pm->ps->clientNum].turret_ent);
		}
#endif //CGAMEDLL
		return qfalse;
	}

#ifndef CGAMEDLL
	if (g_entities[pm->ps->clientNum].turret_ent)
	{
		trap_LinkEntity(g_entities[pm->ps->clientNum].turret_ent);
	}
#endif //CGAMEDLL

	pm->pml_tank_movetime = 0;
	return qtrue;
}
#endif //__VEHICLES__

/*
==================
PM_SlideMove

Returns qtrue if the velocity was clipped in some way
==================
*/
#define	MAX_CLIP_PLANES	5
qboolean	PM_SlideMove( qboolean gravity ) {
	int			bumpcount, numbumps, extrabumps;
	vec3_t		dir;
	float		d;
	int			numplanes;
	vec3_t		planes[MAX_CLIP_PLANES];
	vec3_t		primal_velocity;
	vec3_t		clipVelocity;
	int			i, j, k;
	trace_t	trace;
	vec3_t		end;
	float		time_left;
	float		into;
	vec3_t		endVelocity;
	vec3_t		endClipVelocity;
	
	numbumps = 4;
	extrabumps = 0;

	VectorCopy (pm->ps->velocity, primal_velocity);

	if ( gravity ) {
		VectorCopy( pm->ps->velocity, endVelocity );
		endVelocity[2] -= pm->ps->gravity * pml.frametime;
		pm->ps->velocity[2] = ( pm->ps->velocity[2] + endVelocity[2] ) * 0.5;
		primal_velocity[2] = endVelocity[2];
		if ( pml.groundPlane ) {
			// slide along the ground plane
			PM_ClipVelocity (pm->ps->velocity, pml.groundTrace.plane.normal, 
				pm->ps->velocity, OVERCLIP );
		}
	} else {
		VectorClear( endVelocity );
	}

	time_left = pml.frametime;

	// never turn against the ground plane
	if ( pml.groundPlane ) {
		numplanes = 1;
		VectorCopy( pml.groundTrace.plane.normal, planes[0] );
	} else {
		numplanes = 0;
	}

	// never turn against original velocity
	VectorNormalize2( pm->ps->velocity, planes[numplanes] );
	numplanes++;

	for ( bumpcount=0 ; bumpcount < numbumps ; bumpcount++ ) {
		// calculate position we are trying to move to
		VectorMA( pm->ps->origin, time_left, pm->ps->velocity, end );

		// see if we can make it there
		PM_TraceAll( &trace, pm->ps->origin, end );
		if ( pm->debugLevel > 1 ) {
			Com_Printf("RESULT: c_pmove: %i endpos: (%f %f %f): fraction: %f allsolid: %d startsolid: %d\n",
				c_pmove,
				trace.endpos[0],
				trace.endpos[1],
				trace.endpos[2],
				trace.fraction, trace.allsolid, trace.startsolid
			);
			Com_Printf("WANTED: end: (%f %f %f)\n",
				end[0],
				end[1],
				end[2]
			);
		}

#ifdef __VEHICLES__
		if (pm->ps->eFlags & EF_VEHICLE)
		{// Check the tank move direction from center, left and right of current origin!
			if (!PM_VehicleTraces(time_left, trace))
			{
				//trace.fraction = 0;
				//VectorCopy (pm->ps->origin, trace.endpos);
				return qtrue;
			}
		}
#endif //__VEHICLES__

		if (trace.allsolid) {
			// entity is completely trapped in another solid
			pm->ps->velocity[2] = 0;	// don't build up falling damage, but allow sideways acceleration
			return qtrue;
		}

		if (trace.fraction > 0) {
			// actually covered some distance
			VectorCopy (trace.endpos, pm->ps->origin);
		}

		if (trace.fraction == 1) {
			 break;		// moved the entire distance
		}

		// save entity for contact
		PM_AddTouchEnt( trace.entityNum );

		time_left -= time_left * trace.fraction;

		if (numplanes >= MAX_CLIP_PLANES) {
			// this shouldn't really happen
			VectorClear( pm->ps->velocity );
			return qtrue;
		}

		//
		// if this is the same plane we hit before, nudge velocity
		// out along it, which fixes some epsilon issues with
		// non-axial planes
		//
		for ( i = 0 ; i < numplanes ; i++ ) {
			if ( DotProduct( trace.plane.normal, planes[i] ) > 0.99 ) {
				if ( extrabumps <= 0 ) {
					VectorAdd( trace.plane.normal, pm->ps->velocity, pm->ps->velocity );
					extrabumps++;
					numbumps++;

					if ( pm->debugLevel )
						Com_Printf( "%i:planevelocitynudge\n", c_pmove );
				} else {
					// zinx - if it happens again, nudge the origin instead,
					// and trace it, to make sure we don't end up in a solid

					VectorAdd( pm->ps->origin, trace.plane.normal, end );
					PM_TraceAll( &trace, pm->ps->origin, end );
					VectorCopy( trace.endpos, pm->ps->origin );

					if ( pm->debugLevel )
						Com_Printf( "%i:planeoriginnudge\n", c_pmove );
				}
				break;
			}
		}
		if ( i < numplanes ) {
			continue;
		}
		VectorCopy (trace.plane.normal, planes[numplanes]);
		numplanes++;

		//
		// modify velocity so it parallels all of the clip planes
		//

		// find a plane that it enters
		for ( i = 0 ; i < numplanes ; i++ ) {
			into = DotProduct( pm->ps->velocity, planes[i] );
			if ( into >= 0.1 ) {
				continue;		// move doesn't interact with the plane
			}

			// see how hard we are hitting things
			if ( -into > pml.impactSpeed ) {
				pml.impactSpeed = -into;
			}

			// slide along the plane
			PM_ClipVelocity (pm->ps->velocity, planes[i], clipVelocity, OVERCLIP );

			// slide along the plane
			PM_ClipVelocity (endVelocity, planes[i], endClipVelocity, OVERCLIP );

			// see if there is a second plane that the new move enters
			for ( j = 0 ; j < numplanes ; j++ ) {
				if ( j == i ) {
					continue;
				}
				if ( DotProduct( clipVelocity, planes[j] ) >= 0.1 ) {
					continue;		// move doesn't interact with the plane
				}

				// try clipping the move to the plane
				PM_ClipVelocity( clipVelocity, planes[j], clipVelocity, OVERCLIP );
				PM_ClipVelocity( endClipVelocity, planes[j], endClipVelocity, OVERCLIP );

				// see if it goes back into the first clip plane
				if ( DotProduct( clipVelocity, planes[i] ) >= 0 ) {
					continue;
				}

				// slide the original velocity along the crease
				CrossProduct (planes[i], planes[j], dir);
				VectorNormalize( dir );
				d = DotProduct( dir, pm->ps->velocity );
				VectorScale( dir, d, clipVelocity );

				CrossProduct (planes[i], planes[j], dir);
				VectorNormalize( dir );
				d = DotProduct( dir, endVelocity );
				VectorScale( dir, d, endClipVelocity );

				// see if there is a third plane the the new move enters
				for ( k = 0 ; k < numplanes ; k++ ) {
					if ( k == i || k == j ) {
						continue;
					}
					if ( DotProduct( clipVelocity, planes[k] ) >= 0.1 ) {
						continue;		// move doesn't interact with the plane
					}

					// stop dead at a tripple plane interaction
					VectorClear( pm->ps->velocity );
					return qtrue;
				}
			}

			// if we have fixed all interactions, try another move
			VectorCopy( clipVelocity, pm->ps->velocity );
			VectorCopy( endClipVelocity, endVelocity );
			break;
		}
	}

	if ( gravity ) {
		VectorCopy( endVelocity, pm->ps->velocity );
	}

	// don't change velocity if in a timer (FIXME: is this correct?)
	if ( pm->ps->pm_time ) {
		VectorCopy( primal_velocity, pm->ps->velocity );
	}

#ifdef __VEHICLES__
#ifndef CGAMEDLL
	if (pm->ps->eFlags & EF_VEHICLE)
		UpdateTankTurret( &g_entities[pm->ps->clientNum] );
#endif //CGAMEDLL
#endif //__VEHICLES__

	return ( bumpcount != 0 );
}

void PM_DebugTraceFlags ( int contents, int surfaceFlags )
{
	UQ_Printf("Current surface flags (%i):\n", surfaceFlags);

	if (surfaceFlags & SURF_NODAMAGE)
		UQ_Printf("SURF_NODAMAGE ");

	if (surfaceFlags & SURF_SLICK)
		UQ_Printf("SURF_SLICK ");

	if (surfaceFlags & SURF_SKY)
		UQ_Printf("SURF_SKY ");

	if (surfaceFlags & SURF_LADDER)
		UQ_Printf("SURF_LADDER ");

	if (surfaceFlags & SURF_NOIMPACT)
		UQ_Printf("SURF_NOIMPACT ");

	if (surfaceFlags & SURF_NOMARKS)
		UQ_Printf("SURF_NOMARKS ");

	if (surfaceFlags & SURF_SPLASH)
		UQ_Printf("SURF_SPLASH ");

	if (surfaceFlags & SURF_NODRAW)
		UQ_Printf("SURF_NODRAW ");

	if (surfaceFlags & SURF_HINT)
		UQ_Printf("SURF_HINT ");

	if (surfaceFlags & SURF_SKIP)
		UQ_Printf("SURF_SKIP ");

	if (surfaceFlags & SURF_NOLIGHTMAP)
		UQ_Printf("SURF_NOLIGHTMAP ");

	if (surfaceFlags & SURF_METAL)
		UQ_Printf("SURF_METAL ");

	if (surfaceFlags & SURF_NOSTEPS)
		UQ_Printf("SURF_NOSTEPS ");

	if (surfaceFlags & SURF_NONSOLID)
		UQ_Printf("SURF_NONSOLID ");

	if (surfaceFlags & SURF_LIGHTFILTER)
		UQ_Printf("SURF_LIGHTFILTER ");

	if (surfaceFlags & SURF_ALPHASHADOW)
		UQ_Printf("SURF_ALPHASHADOW ");

	if (surfaceFlags & SURF_NODLIGHT)
		UQ_Printf("SURF_NODLIGHT ");

	if (surfaceFlags & SURF_WOOD)
		UQ_Printf("SURF_WOOD ");

	if (surfaceFlags & SURF_GRASS)
		UQ_Printf("SURF_GRASS ");

	if (surfaceFlags & SURF_GRAVEL)
		UQ_Printf("SURF_GRAVEL ");

	if (surfaceFlags & SURF_GLASS)
		UQ_Printf("SURF_GLASS ");

	if (surfaceFlags & SURF_SNOW)
		UQ_Printf("SURF_SNOW ");

	if (surfaceFlags & SURF_ROOF)
		UQ_Printf("SURF_ROOF ");

	if (surfaceFlags & SURF_RUBBLE)
		UQ_Printf("SURF_RUBBLE ");

	if (surfaceFlags & SURF_CARPET)
		UQ_Printf("SURF_CARPET ");

	if (surfaceFlags & SURF_MONSTERSLICK)
		UQ_Printf("SURF_MONSTERSLICK ");

	if (surfaceFlags & SURF_MONSLICK_W)
		UQ_Printf("SURF_MONSLICK_W ");

	if (surfaceFlags & SURF_MONSLICK_N)
		UQ_Printf("SURF_MONSLICK_N ");

	if (surfaceFlags & SURF_MONSLICK_E)
		UQ_Printf("SURF_MONSLICK_E ");

	if (surfaceFlags & SURF_MONSLICK_S)
		UQ_Printf("SURF_MONSLICK_S ");

	if (surfaceFlags & SURF_LANDMINE)
		UQ_Printf("SURF_LANDMINE ");

	UQ_Printf("\n");

	//
	// Contents...
	//

	UQ_Printf("Current contents flags (%i):\n", surfaceFlags);

	if (contents & CONTENTS_SOLID)
		UQ_Printf("CONTENTS_SOLID ");

	if (contents & CONTENTS_LIGHTGRID)
		UQ_Printf("CONTENTS_LIGHTGRID ");

	if (contents & CONTENTS_LAVA)
		UQ_Printf("CONTENTS_LAVA ");

	if (contents & CONTENTS_SLIME)
		UQ_Printf("CONTENTS_SLIME ");

	if (contents & CONTENTS_WATER)
		UQ_Printf("CONTENTS_WATER ");

	if (contents & CONTENTS_FOG)
		UQ_Printf("CONTENTS_FOG ");

	if (contents & CONTENTS_MISSILECLIP)
		UQ_Printf("CONTENTS_MISSILECLIP ");

	if (contents & CONTENTS_ITEM)
		UQ_Printf("CONTENTS_ITEM ");

	if (contents & CONTENTS_MOVER)
		UQ_Printf("CONTENTS_MOVER ");

	if (contents & CONTENTS_AREAPORTAL)
		UQ_Printf("CONTENTS_AREAPORTAL ");

	if (contents & CONTENTS_PLAYERCLIP)
		UQ_Printf("CONTENTS_PLAYERCLIP ");

	if (contents & CONTENTS_MONSTERCLIP)
		UQ_Printf("CONTENTS_MONSTERCLIP ");

	if (contents & CONTENTS_TELEPORTER)
		UQ_Printf("CONTENTS_TELEPORTER ");

	if (contents & CONTENTS_JUMPPAD)
		UQ_Printf("CONTENTS_JUMPPAD ");

	if (contents & CONTENTS_CLUSTERPORTAL)
		UQ_Printf("CONTENTS_CLUSTERPORTAL ");

	if (contents & CONTENTS_DONOTENTER)
		UQ_Printf("CONTENTS_DONOTENTER ");

	if (contents & CONTENTS_DONOTENTER_LARGE)
		UQ_Printf("CONTENTS_DONOTENTER_LARGE ");

	if (contents & CONTENTS_ORIGIN)
		UQ_Printf("CONTENTS_ORIGIN ");

	if (contents & CONTENTS_BODY)
		UQ_Printf("CONTENTS_BODY ");

	if (contents & CONTENTS_CORPSE)
		UQ_Printf("CONTENTS_CORPSE ");

	if (contents & CONTENTS_DETAIL)
		UQ_Printf("CONTENTS_DETAIL ");

	if (contents & CONTENTS_STRUCTURAL)
		UQ_Printf("CONTENTS_STRUCTURAL ");

	if (contents & CONTENTS_TRANSLUCENT)
		UQ_Printf("CONTENTS_TRANSLUCENT ");

	if (contents & CONTENTS_TRIGGER)
		UQ_Printf("CONTENTS_TRIGGER ");

	if (contents & CONTENTS_NODROP)
		UQ_Printf("CONTENTS_NODROP ");

	UQ_Printf("\n");
}

/*
==================
PM_StepSlideMove

==================
*/
void PM_StepSlideMove( qboolean gravity ) {
	vec3_t		start_o, start_v;
	vec3_t		down_o, down_v;
	trace_t		trace;
//	float		down_dist, up_dist;
//	vec3_t		delta, delta2;
	vec3_t		up, down;

	VectorCopy (pm->ps->origin, start_o);
	VectorCopy (pm->ps->velocity, start_v);

	if ( pm->debugLevel ) {
		qboolean wassolid, slidesucceed;
	
		PM_TraceAll( &trace, pm->ps->origin, pm->ps->origin );
		wassolid = trace.allsolid;
	
		slidesucceed = (PM_SlideMove( gravity ) == 0);
	
		PM_TraceAll( &trace, pm->ps->origin, pm->ps->origin );
		if (trace.allsolid && !wassolid)
			Com_Printf("%i:PM_SlideMove solidified! (%f %f %f) -> (%f %f %f)\n", c_pmove,
				start_o[0],
				start_o[1],
				start_o[2],
				pm->ps->origin[0],
				pm->ps->origin[1],
				pm->ps->origin[2]
			);
	
		if (slidesucceed)
			return;
	} else {
		if ( PM_SlideMove( gravity ) == 0 ) {
			return;		// we got exactly where we wanted to go first try
		}
#ifdef __VEHICLES__
		if ((pm->ps->eFlags & EF_VEHICLE) && pm->pml_tank_movetime == 1)
			return;
#endif //__VEHICLES__
	}

	if ( pm->debugLevel ) {
		Com_Printf("%i:stepping\n", c_pmove);
	}

	VectorCopy(start_o, down);
	down[2] -= STEPSIZE;

	PM_TraceAll( &trace, start_o, down );
	VectorSet(up, 0, 0, 1);
	// never step up when you still have up velocity
	if ( pm->ps->velocity[2] > 0 && (trace.fraction == 1.0 || DotProduct(trace.plane.normal, up) < 0.7)) {
		return;
	}

	VectorCopy (pm->ps->origin, down_o);
	VectorCopy (pm->ps->velocity, down_v);

	VectorCopy (start_o, up);
	up[2] += STEPSIZE;

	// test the player position if they were a stepheight higher
	PM_TraceAll( &trace, up, up );
	if ( trace.allsolid ) {
		if ( pm->debugLevel ) {
			Com_Printf("%i:bend can't step\n", c_pmove);

			if (trace.entityNum >= 0)
			{
				Com_Printf("Trace hit entity %i.\n", trace.entityNum);
#ifdef GAMEDLL
				Com_Printf("Mins: %f %f %f. Maxs: %f %f %f.\n", g_entities[trace.entityNum].r.mins[0], g_entities[trace.entityNum].r.mins[1], g_entities[trace.entityNum].r.mins[2], g_entities[trace.entityNum].r.maxs[0], g_entities[trace.entityNum].r.maxs[1], g_entities[trace.entityNum].r.maxs[2]);
#endif //GAMEDLL
			}

			PM_DebugTraceFlags(trace.contents, trace.surfaceFlags);
		}
/*
		// UQ1: BEGIN - Hack for eltz_beta1 bad mover...
#ifdef GAMEDLL
		if (trace.entityNum == 276 
			&& g_entities[trace.entityNum].r.mins[0] == -5 && g_entities[trace.entityNum].r.mins[1] == -57
			&& g_entities[trace.entityNum].r.maxs[0] == 5 && g_entities[trace.entityNum].r.maxs[1] == 57 && g_entities[trace.entityNum].r.maxs[2] == 125)
		{
			Com_Printf("*** ELTZ_BETA1 - Bad mover hack #1 activated!\n");
			g_entities[trace.entityNum].r.contents = 0;
			trap_UnlinkEntity(&g_entities[trace.entityNum]);
		}

		if (trace.entityNum == 279 
			&& g_entities[trace.entityNum].r.mins[0] == -109 && g_entities[trace.entityNum].r.mins[1] == -7 && g_entities[trace.entityNum].r.mins[2] == -9
			&& g_entities[trace.entityNum].r.maxs[0] == 109 && g_entities[trace.entityNum].r.maxs[1] == 7 && g_entities[trace.entityNum].r.maxs[2] == 197)
		{
			Com_Printf("*** ELTZ_BETA1 - Bad mover hack #2 activated!\n");
			g_entities[trace.entityNum].r.contents = 0;
			trap_UnlinkEntity(&g_entities[trace.entityNum]);
		}
#endif GAMEDLL
		// UQ1: END - Hack for eltz_beta1 bad mover...
		*/
		return;		// can't step up
	}

	// try slidemove from this position
	VectorCopy (up, pm->ps->origin);
	VectorCopy (start_v, pm->ps->velocity);

	PM_SlideMove( gravity );

	// push down the final amount
	VectorCopy (pm->ps->origin, down);
	down[2] -= STEPSIZE;

	// check legs separately
	if ( pm->ps->eFlags & EF_PRONE ) {
		memset(&trace, 0, sizeof(trace));
		PM_TraceLegs( &trace, NULL, pm->ps->origin, down, NULL, pm->ps->viewangles, pm->trace, pm->ps->clientNum, pm->tracemask );
		if (trace.fraction < 1.0f) {
			// legs don't step, just fuzz.
			VectorCopy( down_o, pm->ps->origin );
			VectorCopy( down_v, pm->ps->velocity );
			if ( pm->debugLevel ) {
				Com_Printf("%i:legs unsteppable\n", c_pmove);
			}
			return;
		}
		memset(&trace, 0, sizeof(trace));
		PM_TraceHead( &trace, pm->ps->origin, down, NULL,
			pm->ps->viewangles, pm->trace, pm->ps->clientNum,
			pm->tracemask);
		if(trace.fraction < 1.0f) {
			VectorCopy( down_o, pm->ps->origin );
			VectorCopy( down_v, pm->ps->velocity );
			if ( pm->debugLevel ) {
				Com_Printf("%i:head unsteppable\n", c_pmove);
			}
			return;
		}
	}

	memset(&trace, 0, sizeof(trace));
	pm->trace( &trace, pm->ps->origin, pm->mins, pm->maxs, down, pm->ps->clientNum, pm->tracemask );
	if ( !trace.allsolid ) {
		VectorCopy (trace.endpos, pm->ps->origin);
	}
	if ( trace.fraction < 1.0 ) {
		PM_ClipVelocity( pm->ps->velocity, trace.plane.normal, pm->ps->velocity, OVERCLIP );
	}


	{
		// use the step move
		float	delta;

		delta = pm->ps->origin[2] - start_o[2];
		if ( delta > 2 ) {
			if ( delta < 7 ) {
				PM_AddEvent( EV_STEP_4 );
			} else if ( delta < 11 ) {
				PM_AddEvent( EV_STEP_8 );
			} else if ( delta < 15 ) {
				PM_AddEvent( EV_STEP_12 );
			} else {
				PM_AddEvent( EV_STEP_16 );
			}
		}
		if ( pm->debugLevel ) {
			Com_Printf("%i:stepped\n", c_pmove);
		}
	}
}

