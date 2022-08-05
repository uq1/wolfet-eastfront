#include "../game/q_global_defines.h"

#ifdef __ETE__

#include "cg_local.h"

extern float VectorDistance(vec3_t v1, vec3_t v2);

/*
===============
CG_Ammo_Crate
===============
*/
void CG_Ammo_Crate( centity_t *cent ) {
	refEntity_t		model;
	entityState_t	*cs;
	playerState_t	*ps;
	float			scale;

	cs = &cent->currentState;
	ps = &cg.predictedPlayerState;

	memset(&model, 0, sizeof(model));
	model.reType = RT_MODEL;

	VectorCopy( cent->lerpOrigin, model.lightingOrigin );
	VectorCopy( cent->lerpOrigin, model.origin );
	VectorCopy( cent->lerpOrigin, model.oldorigin);

	AnglesToAxis( cent->currentState.angles, model.axis );

	if (!cent->currentState.modelindex2)
		cent->currentState.modelindex2 = trap_R_RegisterModel( "models/doa/doa_cabinet/ammo_close.md3" );

	if (!cent->currentState.modelindex)
		cent->currentState.modelindex = trap_R_RegisterModel( "models/doa/doa_cabinet/ammo_open.md3" );

	if (cent->currentState.density == 1)
		model.hModel = cgs.gameModels[cent->currentState.modelindex];
	else
		model.hModel = cgs.gameModels[cent->currentState.modelindex2];
	
	scale = cent->currentState.angles2[0];
	
	if (scale <= 0)
		scale = 1.0f;

	VectorScale( model.axis[0], scale, model.axis[0] );
	VectorScale( model.axis[1], scale, model.axis[1] );
	VectorScale( model.axis[2], scale, model.axis[2] );
	model.nonNormalizedAxes = qtrue;

#ifdef __NPC__
	if (cent->currentState.eType != ET_NPC)
	{ //do not do this for NPCs
#endif
		trap_R_AddRefEntityToScene( &model );
#ifdef __NPC__
	}
#endif
}

/*
===============
CG_Health_Crate
===============
*/
void CG_Health_Crate( centity_t *cent ) {
	refEntity_t		model;
	entityState_t	*cs;
	playerState_t	*ps;
	float			scale;

	cs = &cent->currentState;
	ps = &cg.predictedPlayerState;

	memset(&model, 0, sizeof(model));
	model.reType = RT_MODEL;

	VectorCopy( cent->lerpOrigin, model.lightingOrigin );
	VectorCopy( cent->lerpOrigin, model.origin );
	VectorCopy( cent->lerpOrigin, model.oldorigin);

	AnglesToAxis( cent->currentState.angles, model.axis );

	if (!cent->currentState.modelindex2)
		cent->currentState.modelindex2 = trap_R_RegisterModel( "models/doa/doa_cabinet/health_close.md3" );

	if (!cent->currentState.modelindex)
		cent->currentState.modelindex = trap_R_RegisterModel( "models/doa/doa_cabinet/health_open.md3" );

	if (cent->currentState.density == 1)
		model.hModel = cgs.gameModels[cent->currentState.modelindex];
	else
		model.hModel = cgs.gameModels[cent->currentState.modelindex2];

	scale = cent->currentState.angles2[0];
	
	if (scale <= 0)
		scale = 1.0f;

	VectorScale( model.axis[0], scale, model.axis[0] );
	VectorScale( model.axis[1], scale, model.axis[1] );
	VectorScale( model.axis[2], scale, model.axis[2] );
	model.nonNormalizedAxes = qtrue;

#ifdef __NPC__
	if (cent->currentState.eType != ET_NPC)
	{ //do not do this for NPCs
#endif
		trap_R_AddRefEntityToScene( &model );
#ifdef __NPC__
	}
#endif
}

void CG_SandBag ( centity_t *cent ) 
{// UQ1: Uses trap_R_ModelBounds()
	refEntity_t		model;
	entityState_t	*cs;
	playerState_t	*ps;
	vec3_t			mins, maxs;

	cs = &cent->currentState;
	ps = &cg.predictedPlayerState;

	memset(&model, 0, sizeof(model));
	model.reType = RT_MODEL;

	VectorCopy( cent->lerpOrigin, model.lightingOrigin );
	VectorCopy( cent->lerpOrigin, model.origin );
	VectorCopy( cent->lerpOrigin, model.oldorigin);

	AnglesToAxis( cent->currentState.angles, model.axis );

	model.hModel = cgs.gameModels[cent->currentState.modelindex];
	trap_R_ModelBounds( cent->currentState.modelindex, mins, maxs );
	model.radius = RadiusFromBounds( mins, maxs );

#ifdef __NPC__
	if (cent->currentState.eType != ET_NPC)
#endif
		trap_R_AddRefEntityToScene( &model );
}

void CG_Constructible_Sandbags( centity_t *cent ) {
	refEntity_t		model;
	entityState_t	*cs;
	playerState_t	*ps;
	vec3_t			mins, maxs;

	cs = &cent->currentState;
	ps = &cg.predictedPlayerState;

	memset(&model, 0, sizeof(model));
	model.reType = RT_MODEL;

	VectorCopy( cent->lerpOrigin, model.lightingOrigin );
	VectorCopy( cent->lerpOrigin, model.origin );
	VectorCopy( cent->lerpOrigin, model.oldorigin);

	AnglesToAxis( cent->currentState.angles, model.axis );

	model.hModel = cgs.gameModels[cent->currentState.modelindex];

	if (cent->currentState.dl_intensity != SB_CONSTRUCTED)
	{
		if (cent->currentState.teamNum == TEAM_ALLIES)
			model.customSkin = trap_R_RegisterSkin("models/mapobjects/cmarker/allied_crates.skin");
		else
			model.customSkin = trap_R_RegisterSkin("models/mapobjects/cmarker/axis_crates.skin");

		VectorScale( model.axis[0], 0.6, model.axis[0] );
		VectorScale( model.axis[1], 0.6, model.axis[1] );
		VectorScale( model.axis[2], 0.6, model.axis[2] );
		model.nonNormalizedAxes = qtrue;
	}

	trap_R_ModelBounds( cent->currentState.modelindex, mins, maxs );
	model.radius = RadiusFromBounds( mins, maxs );

#ifdef __NPC__
	if (cent->currentState.eType != ET_NPC)
#endif
		trap_R_AddRefEntityToScene( &model );

	if (cent->currentState.modelindex2)
	{
		refEntity_t		model2;
		vec3_t			mins2, maxs2;

		cs = &cent->currentState;
		ps = &cg.predictedPlayerState;

		memset(&model2, 0, sizeof(model2));
		model2.reType = RT_MODEL;

		VectorCopy( cent->lerpOrigin, model2.lightingOrigin );
		VectorCopy( cent->lerpOrigin, model2.origin );
		VectorCopy( cent->lerpOrigin, model2.oldorigin);

		AnglesToAxis( cent->currentState.angles, model2.axis );

		model2.hModel = cgs.gameModels[cent->currentState.modelindex2];

		if( cent->currentState.dl_intensity == SB_CONSTRUCTING ) 
		{
			model2.customShader = cgs.media.genericConstructionShader;
		}

		trap_R_ModelBounds( cent->currentState.modelindex2, mins2, maxs2 );
		model2.radius = RadiusFromBounds( mins2, maxs2 );

#ifdef __NPC__
		if (cent->currentState.eType != ET_NPC)
#endif
			trap_R_AddRefEntityToScene( &model2 );
	}
}

void CG_MG_Nest ( centity_t *cent ) 
{// UQ1: Uses trap_R_ModelBounds()
	refEntity_t		model;
	entityState_t	*cs;
	playerState_t	*ps;
	vec3_t			mins, maxs;

	cs = &cent->currentState;
	ps = &cg.predictedPlayerState;

	memset(&model, 0, sizeof(model));
	model.reType = RT_MODEL;

	VectorCopy( cent->lerpOrigin, model.lightingOrigin );
	VectorCopy( cent->lerpOrigin, model.origin );
	VectorCopy( cent->lerpOrigin, model.oldorigin);

	AnglesToAxis( cent->currentState.angles, model.axis );

	model.hModel = cgs.gameModels[cent->currentState.modelindex];
	trap_R_ModelBounds( cent->currentState.modelindex, mins, maxs );
	model.radius = RadiusFromBounds( mins, maxs );

#ifdef __NPC__
	if (cent->currentState.eType != ET_NPC)
#endif
		trap_R_AddRefEntityToScene( &model );
}

void CG_Constructible_MG_Nest( centity_t *cent ) {
	refEntity_t		model;
	entityState_t	*cs;
	playerState_t	*ps;
	vec3_t			mins, maxs;

	cs = &cent->currentState;
	ps = &cg.predictedPlayerState;

	memset(&model, 0, sizeof(model));
	model.reType = RT_MODEL;

	VectorCopy( cent->lerpOrigin, model.lightingOrigin );
	VectorCopy( cent->lerpOrigin, model.origin );
	VectorCopy( cent->lerpOrigin, model.oldorigin);

	AnglesToAxis( cent->currentState.angles, model.axis );

	model.hModel = cgs.gameModels[cent->currentState.modelindex];

	if (cent->currentState.dl_intensity != SB_CONSTRUCTED)
	{
		if (cent->currentState.teamNum == TEAM_ALLIES)
			model.customSkin = trap_R_RegisterSkin("models/mapobjects/cmarker/allied_crates.skin");
		else
			model.customSkin = trap_R_RegisterSkin("models/mapobjects/cmarker/axis_crates.skin");

		VectorScale( model.axis[0], 0.6, model.axis[0] );
		VectorScale( model.axis[1], 0.6, model.axis[1] );
		VectorScale( model.axis[2], 0.6, model.axis[2] );
		model.nonNormalizedAxes = qtrue;
	}

	trap_R_ModelBounds( cent->currentState.modelindex, mins, maxs );
	model.radius = RadiusFromBounds( mins, maxs );

#ifdef __NPC__
	if (cent->currentState.eType != ET_NPC)
#endif
		trap_R_AddRefEntityToScene( &model );

	if (cent->currentState.modelindex2)
	{
		refEntity_t		model2;
		vec3_t			mins2, maxs2;

		cs = &cent->currentState;
		ps = &cg.predictedPlayerState;

		memset(&model2, 0, sizeof(model2));
		model2.reType = RT_MODEL;

		VectorCopy( cent->lerpOrigin, model2.lightingOrigin );
		VectorCopy( cent->lerpOrigin, model2.origin );
		VectorCopy( cent->lerpOrigin, model2.oldorigin);

		AnglesToAxis( cent->currentState.angles, model2.axis );

		model2.hModel = cgs.gameModels[cent->currentState.modelindex2];

		if( cent->currentState.dl_intensity == SB_CONSTRUCTING ) 
		{
			model2.customShader = cgs.media.genericConstructionShader;
		}

		trap_R_ModelBounds( cent->currentState.modelindex2, mins2, maxs2 );
		model2.radius = RadiusFromBounds( mins2, maxs2 );

#ifdef __NPC__
		if (cent->currentState.eType != ET_NPC)
#endif
			trap_R_AddRefEntityToScene( &model2 );
	}
}

#ifdef __UQ_FOG_SHADER__TEST__
qboolean	uq_fog_registerred = qfalse;
qhandle_t	uq_fog_shaders[256];
float		uq_fog_angle = 0.0f;
int			uq_fog_currentshader = 0;
int			uq_fog_currentshader_counter = 0;
int			uq_fog_num_shaders = 0;

void CG_UQ_DrawFogShaders( void )
{
	int i = 0;

	if (!uq_fog_registerred)
	{
		uq_fog_registerred = qtrue;

		for (i = 115; i < 164; i++)
		{
			uq_fog_shaders[uq_fog_num_shaders] = trap_R_RegisterShader( va("gfx/misc/uq_fog_00%i", i) );
			uq_fog_num_shaders++;
		}
	}

	/*uq_fog_angle+=0.0001;
	
	if (uq_fog_angle >= 1)
		uq_fog_angle = 0;

	uq_fog_currentshader++;

	if (uq_fog_currentshader >= uq_fog_num_shaders)
		uq_fog_currentshader = 0;

	CG_DrawRotatedPic( -640, -480, 1280, 960, uq_fog_shaders[uq_fog_currentshader], uq_fog_angle );*/

	uq_fog_currentshader_counter++;

	if (uq_fog_currentshader_counter > 4)
	{
		uq_fog_currentshader++;
		uq_fog_currentshader_counter = 0;
	}

	if (uq_fog_currentshader >= uq_fog_num_shaders)
		uq_fog_currentshader = 0;

	CG_DrawPic( 0, 0, 640, 480, uq_fog_shaders[uq_fog_currentshader] );
}
#endif //__UQ_FOG_SHADER__TEST__
#endif //__ETE__
