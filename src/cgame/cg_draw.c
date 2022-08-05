// cg_draw.c -- draw all of the graphical elements during
// active (after loading) gameplay

//add some code by Masteries 5/12/2007     line 2770; line 2699

#include "cg_local.h"
#include "../game/q_global_defines.h"

#define STATUSBARHEIGHT 452
char* BindingFromName(const char *cvar);
void Controls_GetConfig( void );
void SetHeadOrigin(clientInfo_t *ci, playerInfo_t *pi);
void CG_DrawOverlays();
int activeFont;

#define TEAM_OVERLAY_TIME 1000

////////////////////////
////////////////////////
////// new hud stuff
///////////////////////
///////////////////////

void CG_Text_SetActiveFont( int font ) {
	activeFont = font;
}

int CG_Text_Width_Ext( const char *text, float scale, int limit, fontInfo_t* font ) {
	int count, len;
	glyphInfo_t *glyph;
	const char *s = text;
	float out, useScale = scale * font->glyphScale;
	
	out = 0;
	if( text ) {
		len = strlen( text );
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[(unsigned char)*s];
				out += glyph->xSkip;
				s++;
				count++;
			}
		}
	}

	return out * useScale;
}

int CG_Text_Width( const char *text, float scale, int limit ) {
	fontInfo_t *font = &cgDC.Assets.fonts[activeFont];

	return CG_Text_Width_Ext( text, scale, limit, font );
}

int CG_Text_Height_Ext( const char *text, float scale, int limit, fontInfo_t* font ) {
 int len, count;
	float max;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text;

	useScale = scale * font->glyphScale;
	max = 0;
	if (text) {
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[(unsigned char)*s];
	      if (max < glyph->height) {
		      max = glyph->height;
			  }
				s++;
				count++;
			}
		}
	}
	return max * useScale;
}

int CG_Text_Height( const char *text, float scale, int limit ) {
	fontInfo_t *font = &cgDC.Assets.fonts[activeFont];

	return CG_Text_Height_Ext( text, scale, limit, font );
}

void CG_Text_PaintChar_Ext(float x, float y, float w, float h, float scalex, float scaley, float s, float t, float s2, float t2, qhandle_t hShader) {
	w *= scalex;
	h *= scaley;
	CG_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s, t, s2, t2, hShader );
}

void CG_Text_PaintChar(float x, float y, float width, float height, float scale, float s, float t, float s2, float t2, qhandle_t hShader) {
	float w, h;
	w = width * scale;
	h = height * scale;
	CG_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s, t, s2, t2, hShader );
}

void CG_Text_Paint_Centred_Ext( float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font ) {
	x -= CG_Text_Width_Ext( text, scalex, limit, font ) * 0.5f;

	CG_Text_Paint_Ext( x, y, scalex, scaley, color, text, adjust, limit, style, font );
}

void CG_Text_Paint_Right_Ext( float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font ) {
	x -= CG_Text_Width_Ext( text, scalex, limit, font );
	CG_Text_Paint_Ext( x, y, scalex, scaley, color, text, adjust, limit, style, font );
}

void CG_Text_Paint_Ext( float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font ) {
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;

	scalex *= font->glyphScale;
	scaley *= font->glyphScale;

	if (text) {
		const char *s = text;
		trap_R_SetColor( color );
		memcpy(&newColor[0], &color[0], sizeof(vec4_t));
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			glyph = &font->glyphs[(unsigned char)*s];
			if ( Q_IsColorString( s ) ) {
				if( *(s+1) == COLOR_NULL ) {
					memcpy( newColor, color, sizeof(newColor) );
				} else {
					memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
					newColor[3] = color[3];
				}
				trap_R_SetColor( newColor );
				s += 2;
				continue;
			} else {
				float yadj = scaley * glyph->top;
				if (style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE) {
					int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
					colorBlack[3] = newColor[3];
					trap_R_SetColor( colorBlack );
					CG_Text_PaintChar_Ext(x + (glyph->pitch * scalex) + ofs, y - yadj + ofs, glyph->imageWidth, glyph->imageHeight, scalex, scaley, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph);
					colorBlack[3] = 1.0;
					trap_R_SetColor( newColor );
				}
				CG_Text_PaintChar_Ext(x + (glyph->pitch * scalex), y - yadj, glyph->imageWidth, glyph->imageHeight, scalex, scaley, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph);
				x += (glyph->xSkip * scalex) + adjust;
				s++;
				count++;
			}
		}
		trap_R_SetColor( NULL );
	}
}

void CG_Text_Paint(float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style) {
	fontInfo_t *font = &cgDC.Assets.fonts[activeFont];

	CG_Text_Paint_Ext( x, y, scale, scale, color, text, adjust, limit, style, font );
}

// NERVE - SMF - added back in
int CG_DrawFieldWidth (int x, int y, int width, int value, int charWidth, int charHeight ) {
	char	num[16], *ptr;
	int		l;
	int		frame;
	int		totalwidth = 0;

	if ( width < 1 ) {
		return 0;
	}

	// draw number string
	if ( width > 5 ) {
		width = 5;
	}

	switch ( width ) {
	case 1:
		value = value > 9 ? 9 : value;
		value = value < 0 ? 0 : value;
		break;
	case 2:
		value = value > 99 ? 99 : value;
		value = value < -9 ? -9 : value;
		break;
	case 3:
		value = value > 999 ? 999 : value;
		value = value < -99 ? -99 : value;
		break;
	case 4:
		value = value > 9999 ? 9999 : value;
		value = value < -999 ? -999 : value;
		break;
	}

	Com_sprintf (num, sizeof(num), "%i", value);
	l = strlen(num);
	if (l > width)
		l = width;

	ptr = num;
	while (*ptr && l)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		totalwidth += charWidth;
		ptr++;
		l--;
	}

	return totalwidth;
}

int CG_DrawField (int x, int y, int width, int value, int charWidth, int charHeight, qboolean dodrawpic, qboolean leftAlign ) {
	char	num[16], *ptr;
	int		l;
	int		frame;
	int		startx;

	if ( width < 1 ) {
		return 0;
	}

	// draw number string
	if ( width > 5 ) {
		width = 5;
	}

	switch ( width ) {
	case 1:
		value = value > 9 ? 9 : value;
		value = value < 0 ? 0 : value;
		break;
	case 2:
		value = value > 99 ? 99 : value;
		value = value < -9 ? -9 : value;
		break;
	case 3:
		value = value > 999 ? 999 : value;
		value = value < -99 ? -99 : value;
		break;
	case 4:
		value = value > 9999 ? 9999 : value;
		value = value < -999 ? -999 : value;
		break;
	}

	Com_sprintf (num, sizeof(num), "%i", value);
	l = strlen(num);
	if (l > width)
		l = width;

	// NERVE - SMF
	if ( !leftAlign ) {
		x -= 2 + charWidth*(l);
	}

	startx = x;

	ptr = num;
	while (*ptr && l)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		if ( dodrawpic )
			CG_DrawPic( x,y, charWidth, charHeight, cgs.media.numberShaders[frame] );
		x += charWidth;
		ptr++;
		l--;
	}

	return startx;
}
// -NERVE - SMF

/*
================
CG_Draw3DModel

================
*/
void CG_Draw3DModel( float x, float y, float w, float h, qhandle_t model, qhandle_t skin, vec3_t origin, vec3_t angles ) {
	refdef_t		refdef;
	refEntity_t		ent;

	CG_AdjustFrom640( &x, &y, &w, &h );

	memset( &refdef, 0, sizeof( refdef ) );

	memset( &ent, 0, sizeof( ent ) );
	AnglesToAxis( angles, ent.axis );
	VectorCopy( origin, ent.origin );
	ent.hModel = model;
	ent.customSkin = skin;
	ent.renderfx = RF_NOSHADOW;		// no stencil shadows

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.fov_x = 30;
	refdef.fov_y = 30;

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.time = cg.time;

	trap_R_ClearScene();
	trap_R_AddRefEntityToScene( &ent );
	trap_R_RenderScene( &refdef );
}

/*
==============
CG_DrawKeyModel
==============
*/
void CG_DrawKeyModel( int keynum, float x, float y, float w, float h, int fadetime) {
	qhandle_t		cm;
	float			len;
	vec3_t			origin, angles;
	vec3_t			mins, maxs;

	VectorClear( angles );

	cm = cg_items[keynum].models[0];

	// offset the origin y and z to center the model
	trap_R_ModelBounds( cm, mins, maxs );

	origin[2] = -0.5 * ( mins[2] + maxs[2] );
	origin[1] = 0.5 * ( mins[1] + maxs[1] );

//	len = 0.5 * ( maxs[2] - mins[2] );		
	len = 0.75 * ( maxs[2] - mins[2] );		
	origin[0] = len / 0.268;	// len / tan( fov/2 )

	angles[YAW] = 30 * sin( cg.time / 2000.0 );;

	CG_Draw3DModel( x, y, w, h, cg_items[keynum].models[0], 0, origin, angles);
}

/*
================
CG_DrawTeamBackground

================
*/
void CG_DrawTeamBackground( int x, int y, int w, int h, float alpha, int team )
{
	vec4_t		hcolor;

	hcolor[3] = alpha;
	if ( team == TEAM_AXIS ) {
		hcolor[0] = 1;
		hcolor[1] = 0;
		hcolor[2] = 0;
	} else if ( team == TEAM_ALLIES ) {
		hcolor[0] = 0;
		hcolor[1] = 0;
		hcolor[2] = 1;
	} else {
		return;
	}
	trap_R_SetColor( hcolor );
	CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );
	trap_R_SetColor( NULL );
}

/*
===========================================================================================

  UPPER RIGHT CORNER

===========================================================================================
*/

#define UPPERRIGHT_X 634
/*
==================
CG_DrawSnapshot
==================
*/
static float CG_DrawSnapshot( float y ) {
	char		*s;
	int			w;

	s = va( "time:%i snap:%i cmd:%i", cg.snap->serverTime, 
		cg.latestSnapshotNum, cgs.serverCommandSequence );
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

	CG_DrawBigString( UPPERRIGHT_X - w, y + 2, s, 1.0F);

	return y + BIGCHAR_HEIGHT + 4;
}

/*
==================
CG_DrawFPS
==================
*/
#define	FPS_FRAMES	4
static float CG_DrawFPS( float y ) {
	char		*s;
	int			w;
	static int	previousTimes[FPS_FRAMES];
	static int	index;
	int		i, total;
	int		fps;
	static	int	previous;
	int		t, frameTime;
	vec4_t		timerBackground =	{ 0.16f,	0.2f,	0.17f,	0.8f	};
	vec4_t		timerBorder     =	{ 0.5f,		0.5f,	0.5f,	0.5f	};
	vec4_t		tclr			=	{ 0.625f,	0.625f,	0.6f,	1.0f	};

	// don't use serverTime, because that will be drifting to
	// correct for internet lag changes, timescales, timedemos, etc
	t = trap_Milliseconds();
	frameTime = t - previous;
	previous = t;

	previousTimes[index % FPS_FRAMES] = frameTime;
	index++;
	if ( index > FPS_FRAMES ) {
		// average multiple frames together to smooth changes out a bit
		total = 0;
		for ( i = 0 ; i < FPS_FRAMES ; i++ ) {
			total += previousTimes[i];
		}
		if ( !total ) {
			total = 1;
		}
		fps = 1000 * FPS_FRAMES / total;

		s = va( "%i FPS", fps );
		w = CG_Text_Width_Ext( s, 0.19f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );

		CG_FillRect( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, timerBackground );
		CG_DrawRect_FixedBorder( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, 1, timerBorder );

		CG_Text_Paint_Ext( UPPERRIGHT_X - w, y + 11, 0.19f, 0.19f, tclr, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
	}

	return y + 12 + 4;
}

float CG_DrawTime( float y )
{
	char displayTime[12];
	int w;
	qtime_t tm;
	vec4_t	timerBackground = { 0.16f, 0.2f, 0.17f, 0.8f };
	vec4_t	timerBorder     = { 0.5f, 0.5f,	0.5f, 0.5f };
	vec4_t	tclr		= { 0.625f, 0.625f, 0.6f, 1.0f };

	trap_RealTime(&tm);
	displayTime[0] = '\0';

	if(cg_drawTime.integer == 1) {
		Q_strcat(displayTime, sizeof(displayTime), 
				va("%d:%02d", tm.tm_hour, tm.tm_min));
		if(cg_drawTimeSeconds.integer) {
			Q_strcat(displayTime, sizeof(displayTime),
				va(":%02d",tm.tm_sec));
		}
	} else {
		Q_strcat(displayTime, sizeof(displayTime), 
			va("%d:%02d",
				((tm.tm_hour == 0 || tm.tm_hour == 12)
					?12:tm.tm_hour%12),
				tm.tm_min));
		if(cg_drawTimeSeconds.integer) {
			Q_strcat(displayTime, sizeof(displayTime),
				va(":%02d",tm.tm_sec));
		}
		Q_strcat(displayTime, sizeof(displayTime),
			(tm.tm_hour < 12)?" am":" pm");
	}
	w = CG_Text_Width_Ext(displayTime, 0.19f, 0, &cgs.media.tahoma16);
	CG_FillRect( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, timerBackground );
	CG_DrawRect_FixedBorder( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, 1, 
			timerBorder );
	CG_Text_Paint_Ext( UPPERRIGHT_X - w, y + 11, 0.19f, 0.19f, tclr, 
			displayTime, 0, 0, 0, &cgs.media.tahoma16);
	return y + 12 + 4;
}

/*
=================
CG_DrawTimer
=================
*/

static float CG_DrawTimer( float y ) {
	char		*s;
	int			w;
	int			mins, seconds, tens;
	int			msec;
	char		*rt;
	vec4_t		color =				{ 0.625f,	0.625f,	0.6f,	1.0f	};
	vec4_t		timerBackground =	{ 0.16f,	0.2f,	0.17f,	0.8f	};
	vec4_t		timerBorder     =	{ 0.5f,		0.5f,	0.5f,	0.5f	};
	// CHRUKER: b018 - Respawn timer shouldn't be shown in spectator mode
	rt = "";
	if(cgs.gametype != GT_WOLF_LMS &&
		cg_drawReinforcementTime.integer > 0 &&
		cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR) {
		
		rt = va("^F%d%s", CG_CalculateReinfTime( qfalse ),
			((cgs.timelimit <= 0.0f) ? "" : " ")) ;


	}

	msec = ( cgs.timelimit * 60.f * 1000.f ) - ( cg.time - cgs.levelStartTime );

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	if(cgs.gamestate != GS_PLAYING) {
		//%	s = va( "%s^*WARMUP", rt );
		s = "^*WARMUP";	// ydnar: don't draw reinforcement time in warmup mode
		color[3] = fabs(sin(cg.time * 0.002));
	} else if ( msec < 0 && cgs.timelimit > 0.0f) {
		s = va( "^N0:00" );
		color[3] = fabs(sin(cg.time * 0.002));
	} else {
		if(cgs.timelimit <= 0.0f) {
			s = va( "%s", rt);
		} else {
			s = va( "%s^*%i:%i%i", rt, mins, tens, seconds);
		}

		color[3] = 1.f;
	}

	w = CG_Text_Width_Ext( s, 0.19f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );

	CG_FillRect( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, timerBackground );
	CG_DrawRect_FixedBorder( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, 1, timerBorder );

	CG_Text_Paint_Ext( UPPERRIGHT_X - w, y + 11, 0.19f, 0.19f, color, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );

	return y + 12 + 4;
}

// START	xkan, 8/29/2002
int CG_BotIsSelected(int clientNum)
{
	int i;

	for (i=0; i<MAX_NUM_BUDDY; i++)
	{
		if (cg.selectedBotClientNumber[i] == 0)
			return 0;
		else if (cg.selectedBotClientNumber[i] == clientNum)
			return 1;
	}
	return 0;
}
// END		xkan, 8/29/2002

/*
=================
CG_DrawTeamOverlay
=================
*/

int maxCharsBeforeOverlay;

#define TEAM_OVERLAY_MAXNAME_WIDTH	16
#define TEAM_OVERLAY_MAXLOCATION_WIDTH	20

#ifdef __BOT__
extern float aw_percent_complete;
extern vmCvar_t cg_autoWaypoint;
extern void AIMod_AutoWaypoint_DrawProgress( void );
extern void AIMod_AutoWaypoint_StandardMethod( void );
extern void AIMod_AutoWaypoint_OutdoorMethod( void );
extern void AIMod_AutoWaypoint_Optimizer ( void ); // below...
extern void AIMod_AutoWaypoint_Cleaner ( qboolean quiet, qboolean null_links_only, qboolean relink_only ); // below...
//#ifdef _WIN32
//extern void AIMod_AutoWaypoint_StandardMethod_ThreadStart ( void );
//extern void AIMod_AutoWaypoint_OutdoorMethod_ThreadStart ( void );
//#endif //_WIN32
extern qboolean AW_Map_Has_Waypoints ( void );
qboolean waypoints_checked = qfalse;

/*#ifdef _WIN32
#include <windows.h>
#include <process.h>
//#include <mmsystem.h>
#include <stdio.h>
#include <conio.h>
DWORD			aw_thread = 0;
qboolean		aw_thread_running = qfalse;

#pragma warning( disable : 4028 )
#pragma warning( disable : 4024 )

void __cdecl AIMod_AutoWaypoint_StandardMethod_Thread ( DWORD index )
{
	aw_thread_running = qtrue;
	AIMod_AutoWaypoint_StandardMethod();
	aw_thread_running = qfalse;
}

void AIMod_AutoWaypoint_StandardMethod_ThreadStart ( void )
{
	aw_thread = _beginthread( AIMod_AutoWaypoint_StandardMethod_Thread, 0, LOWORD( 0) );

	while (aw_thread_running)
	{
		Sleep( 50 );
	}

	aw_thread = 0;

	// _endthread given to terminate
	_endthread();
}

void __cdecl AIMod_AutoWaypoint_OutdoorMethod_Thread ( DWORD index )
{
	aw_thread_running = qtrue;
	AIMod_AutoWaypoint_OutdoorMethod();
	aw_thread_running = qfalse;
}

void AIMod_AutoWaypoint_OutdoorMethod_ThreadStart ( void )
{
	aw_thread = _beginthread( AIMod_AutoWaypoint_OutdoorMethod_Thread, 0, LOWORD( 0) );

	while (aw_thread_running)
	{
		Sleep( 50 );
	}

	aw_thread = 0;

	// _endthread given to terminate
	_endthread();
}
#endif //_WIN32*/
#endif //__BOT__

#ifdef __MUSIC_ENGINE__
#ifdef _WIN32
extern void CG_DrawMusicInformation( void );
#endif //_WIN32
#endif //__MUSIC_ENGINE__


/*
=====================
CG_DrawUpperRight

=====================
*/
static void CG_DrawUpperRight( void ) {
	float	y;

 	y = cg.hud.draws[0];
  
 	// CHRUKER: b008 - Round timer and respawn timer gone when
 	//          cg_drawFireteamOverlay
 	if(cg_drawFireteamOverlay.integer && CG_IsOnFireteam(cg.clientNum) && 
 				cg.hud.fireteam[0] >= 0 ) {
 		rectDef_t rect;
 		rect.x = cg.hud.fireteam[0];
 		rect.y = cg.hud.fireteam[1];
 		rect.w = cg.hud.fireteam[2];
 		rect.h = 100;
		CG_DrawFireTeamOverlay( &rect );
	} else {
//		CG_DrawTeamOverlay( 0 );
	}

	if( !( cg.snap->ps.pm_flags & PMF_LIMBO ) && ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) &&
		( cgs.autoMapExpanded || ( !cgs.autoMapExpanded && ( cg.time - cgs.autoMapExpandTime < 250.f ) ) ) )
		return;

 	if(cg.hud.draws[0] < 0)
 		return;

	if ( cg_drawRoundTimer.integer ) {
		y = CG_DrawTimer( y );
	}

	if ( cg_drawFPS.integer ) {
		y = CG_DrawFPS( y );
	}

	if ( cg_drawTime.integer ) {
		y = CG_DrawTime( y );
	}

	if ( cg_drawSnapshot.integer ) {
		y = CG_DrawSnapshot( y );
	}

#ifdef __MUSIC_ENGINE__
#ifdef _WIN32
	CG_DrawMusicInformation();
#endif //_WIN32
#endif //__MUSIC_ENGINE__

#ifdef __BOT__
	if (!waypoints_checked && cg_autoWaypoint.integer)
	{// Auto-Waypointing of maps hehehe ;)
		int i;

		if (cg.numScores > 0)
		{// See if we are the server host...
			qboolean local = qfalse;

			for(i = 0; i < cg.numScores; i++) 
			{
				if (cg.scores[i].client == cg.clientNum)
				{
					waypoints_checked = qtrue;

					if (cg.scores[i].ping == 0)
					{
						local = qtrue;
						break;
					}

					break;
				}
			}

			if (local && !AW_Map_Has_Waypoints())
			{
/*#ifdef _WIN32
				if (cg_autoWaypoint.integer == 2)
					AIMod_AutoWaypoint_OutdoorMethod_ThreadStart();
				else
					AIMod_AutoWaypoint_StandardMethod_ThreadStart();
#else //!_WIN32*/
				if (cg_autoWaypoint.integer == 2)
					AIMod_AutoWaypoint_OutdoorMethod();
				else
					AIMod_AutoWaypoint_StandardMethod();

				//if (number_of_nodes > 64000)
				//	AIMod_AutoWaypoint_Optimizer();
				//else
					AIMod_AutoWaypoint_Cleaner(qtrue, qtrue, qfalse);
//#endif //_WIN32
			}
		}
	}
#endif //__BOT__

#ifdef __BOT__
	if (aw_percent_complete > 0)
		AIMod_AutoWaypoint_DrawProgress();
#endif //__BOT__

}

/*
===========================================================================================

  LOWER RIGHT CORNER

===========================================================================================
*/

#define CHATLOC_X 160
#define CHATLOC_Y 478
#define CHATLOC_TEXT_X (CHATLOC_X + 0.25f * TINYCHAR_WIDTH)

/*
=================
CG_DrawTeamInfo
=================
*/
static void CG_DrawTeamInfo( void ) {
	int i;
	vec4_t		hcolor;
	int		chatHeight;
	float	alphapercent;
	float	lineHeight = 9.f;
	float scaleVal;
	int		w, h, len;

	int chatWidth = 640 - cg.hud.chattext[0] - 100;
 
	if( cg_teamChatHeight.integer < TEAMCHAT_HEIGHT ) {
		chatHeight = cg_teamChatHeight.integer;
	} else {
		chatHeight = TEAMCHAT_HEIGHT;
	}

 	if( chatHeight <= 0 || cg.hud.chattext[0] < 0 ) {
  		return; // disabled
  	}
  
 	scaleVal = cg.hud.chattext[2]/100.f;

	if( cgs.teamLastChatPos != cgs.teamChatPos ) {
		if( cg.time - cgs.teamChatMsgTimes[cgs.teamLastChatPos % chatHeight] > cg_teamChatTime.integer ) {
			cgs.teamLastChatPos++;
		}

		h = (cgs.teamChatPos - cgs.teamLastChatPos) * lineHeight;

		w = 0;

		for( i = cgs.teamLastChatPos; i < cgs.teamChatPos; i++ ) {
			len = CG_Text_Width_Ext( cgs.teamChatMsgs[i % chatHeight], 0.2f, 0, /*&cgs.media.limboFont2*/&cgs.media.tahoma30 );
			if( len > w ) {
				w = len;
			}
		}
		w *= TINYCHAR_WIDTH;
		w += TINYCHAR_WIDTH * 2;

		for( i = cgs.teamChatPos - 1; i >= cgs.teamLastChatPos; i-- ) {
			alphapercent = 1.0f - (cg.time - cgs.teamChatMsgTimes[i % chatHeight]) / (float)(cg_teamChatTime.integer);
			if( alphapercent > 1.0f ) {
				alphapercent = 1.0f;
			} else if( alphapercent < 0.f ) {
				alphapercent = 0.f;
			}

			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_AXIS ) {
				hcolor[0] = 1;
				hcolor[1] = 0;
				hcolor[2] = 0;
			} else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_ALLIES ) {
				hcolor[0] = 0;
				hcolor[1] = 0;
				hcolor[2] = 1;
			} else {
				hcolor[0] = 0;
				hcolor[1] = 1;
				hcolor[2] = 0;
			}

			hcolor[3] = 0.33f * alphapercent;

			trap_R_SetColor( hcolor );
//			CG_DrawPic( CHATLOC_X, CHATLOC_Y - (cgs.teamChatPos - i)*lineHeight, chatWidth, lineHeight, cgs.media.teamStatusBar );

//			hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
//			hcolor[3] = alphapercent;
//			trap_R_SetColor( hcolor );

//			CG_Text_Paint_Ext( CHATLOC_TEXT_X, CHATLOC_Y - (cgs.teamChatPos - i - 1) * lineHeight - 1, 0.2f, 0.2f, hcolor, cgs.teamChatMsgs[i % chatHeight], 0, 0, 0, /*&cgs.media.limboFont2*/&cgs.media.tahoma30 );
 			CG_DrawPic( cg.hud.chattext[0], 
 				cg.hud.chattext[1] - (cgs.teamChatPos - i)*lineHeight,
 				chatWidth, lineHeight, cgs.media.teamStatusBar );
  
  			hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
  			hcolor[3] = alphapercent;
  			trap_R_SetColor( hcolor );
  
 			CG_Text_Paint_Ext( cg.hud.chattext[0], 
 				cg.hud.chattext[1] - 
 					(cgs.teamChatPos - i - 1) * lineHeight - 1,
 				scaleVal, scaleVal, hcolor, 
 				cgs.teamChatMsgs[i % chatHeight], 
 				0, 0, 0, /*&cgs.media.limboFont2*/&cgs.media.tahoma30 );
		}
	}
}

const char* CG_PickupItemText( int item ) {
	if( bg_itemlist[ item ].giType == IT_HEALTH ) {
		if(bg_itemlist[ item ].world_model[2])	{	// this is a multi-stage item
			// FIXME: print the correct amount for multi-stage
			return va( "a %s", bg_itemlist[ item ].pickup_name );
		} else {
			return va( "%i %s", bg_itemlist[ item ].quantity, bg_itemlist[ item ].pickup_name );
		}
	} else if( bg_itemlist[ item ].giType == IT_TEAM ) {
		return "an Objective";
	} else {
		if( bg_itemlist[ item ].pickup_name[0] == 'a' ||  bg_itemlist[ item ].pickup_name[0] == 'A' ) {
			return va( "an %s", bg_itemlist[ item ].pickup_name );
		} else {
			return va( "a %s", bg_itemlist[ item ].pickup_name );
		}
	}
}

/*
=================
CG_DrawNotify
=================
*/
#define NOTIFYLOC_Y 42 // bottom end
#define NOTIFYLOC_X 0
#define NOTIFYLOC_Y_SP 128

static void CG_DrawNotify( void ) {
	int w, h;
	int i, len;
	vec4_t		hcolor;
	int		chatHeight;
	float	alphapercent;
	char	var[MAX_TOKEN_CHARS];
	float	notifytime = 1.0f;
	int		yLoc;

	return;

	yLoc = NOTIFYLOC_Y;

	trap_Cvar_VariableStringBuffer( "con_notifytime", var, sizeof( var ) );
	notifytime = atof( var ) * 1000;

	if ( notifytime <= 100.f )
		notifytime = 100.0f;

	chatHeight = NOTIFY_HEIGHT;

	if (cgs.notifyLastPos != cgs.notifyPos) {
		if (cg.time - cgs.notifyMsgTimes[cgs.notifyLastPos % chatHeight] > notifytime) {
			cgs.notifyLastPos++;
		}

		h = (cgs.notifyPos - cgs.notifyLastPos) * TINYCHAR_HEIGHT;

		w = 0;

		for (i = cgs.notifyLastPos; i < cgs.notifyPos; i++) {
			len = CG_DrawStrlen(cgs.notifyMsgs[i % chatHeight]);
			if (len > w)
				w = len;
		}
		w *= TINYCHAR_WIDTH;
		w += TINYCHAR_WIDTH * 2;

		if ( maxCharsBeforeOverlay <= 0 )
			maxCharsBeforeOverlay = 80;

		for (i = cgs.notifyPos - 1; i >= cgs.notifyLastPos; i--) {
			alphapercent = 1.0f - ((cg.time - cgs.notifyMsgTimes[i % chatHeight]) / notifytime);
			if (alphapercent > 0.5f)
				alphapercent = 1.0f;
			else 
				alphapercent *= 2;
			
			if (alphapercent < 0.f)
				alphapercent = 0.f;

			hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
			hcolor[3] = alphapercent;
			trap_R_SetColor( hcolor );

			CG_DrawStringExt( NOTIFYLOC_X + TINYCHAR_WIDTH, 
				yLoc - (cgs.notifyPos - i)*TINYCHAR_HEIGHT, 
				cgs.notifyMsgs[i % chatHeight], hcolor, qfalse, qfalse,
				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, maxCharsBeforeOverlay );
		}
	}
}

/*
===============================================================================

LAGOMETER

===============================================================================
*/

#define	LAG_SAMPLES		128


typedef struct {
	int		frameSamples[LAG_SAMPLES];
	int		frameCount;
	int		snapshotFlags[LAG_SAMPLES];
	int		snapshotSamples[LAG_SAMPLES];
	int		snapshotCount;
} lagometer_t;

lagometer_t		lagometer;

/*
==============
CG_AddLagometerFrameInfo

Adds the current interpolate / extrapolate bar for this frame
==============
*/
void CG_AddLagometerFrameInfo( void ) {
	int			offset;

	offset = cg.time - cg.latestSnapshotTime;
	lagometer.frameSamples[ lagometer.frameCount & ( LAG_SAMPLES - 1) ] = offset;
	lagometer.frameCount++;
}

/*
==============
CG_AddLagometerSnapshotInfo

Each time a snapshot is received, log its ping time and
the number of snapshots that were dropped before it.

Pass NULL for a dropped packet.
==============
*/
void CG_AddLagometerSnapshotInfo( snapshot_t *snap ) {
	// dropped packet
	if ( !snap ) {
		lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = -1;
		lagometer.snapshotCount++;
		return;
	}

	// add this snapshot's info
	lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = snap->ping;
	lagometer.snapshotFlags[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = snap->snapFlags;
	lagometer.snapshotCount++;
}

/*
==============
CG_DrawDisconnect

Should we draw something differnet for long lag vs no packets?
==============
*/
static void CG_DrawDisconnect( void ) {
	float		x, y;
	int			cmdNum;
	usercmd_t	cmd;
	const char		*s;
	int			w;  // bk010215 - FIXME char message[1024];

	// OSP - dont draw if a demo and we're running at a different timescale
	if(cg.demoPlayback && cg_timescale.value != 1.0f) return;
	
	// ydnar: don't draw if the server is respawning
	if( cg.serverRespawning )
		return;

#ifdef __BOT__
	if (cg.snap->ps.eFlags & EF_BOT)
		return; // No lagometer for bots!
#endif //__BOT__

	// draw the phone jack if we are completely past our buffers
	cmdNum = trap_GetCurrentCmdNumber() - CMD_BACKUP + 1;

	trap_GetUserCmd( cmdNum, &cmd );
	if ( cmd.serverTime <= cg.snap->ps.commandTime
#ifdef __BOT__
		|| cmd.flags & 0x02 // do not do this for a bots
#endif 
		|| cmd.serverTime > cg.time ) {	// special check for map_restart // bk 0102165 - FIXME
		return;
	}

	// also add text in center of screen
	s = CG_TranslateString( "Connection Interrupted" ); // bk 010215 - FIXME
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	CG_DrawBigString( 320 - w/2, 100, s, 1.0F);

	// blink the icon
	if ( ( cg.time >> 9 ) & 1 ) {
		return;
	}

	x = cg.hud.lagometer[0];
	y = cg.hud.lagometer[1];

	CG_DrawPic( x, y, 48+1, 48+1, cgs.media.disconnectIcon );
}


#define	MAX_LAGOMETER_PING	900
#define	MAX_LAGOMETER_RANGE	300

/*
==============
CG_DrawLagometer
==============
*/
static void CG_DrawLagometer( void ) {
	int	a, i;
	float	v;
	float	ax, ay, aw, ah, mid, range;
	int	color;
	float	vscale;

	if ( !cg_lagometer.integer || cgs.localServer || cg.hud.lagometer[0] < 0 ) {
		CG_DrawDisconnect();
		return;
	}

	ax = cg.hud.lagometer[0];
	ay = cg.hud.lagometer[1];
	aw = 48;
	ah = 48;

	// adjust +1 on right border because of borderwidth of 1
	CG_FillRect(ax, ay, aw+1, ah+1, popBG);
	CG_DrawRect_FixedBorder(ax, ay, aw+1, ah+1, 1, popBorder);

	CG_AdjustFrom640( &ax, &ay, &aw, &ah );

	color = -1;
	range = ah / 3;
	mid = ay + range;

	vscale = range / MAX_LAGOMETER_RANGE;

	// draw the frame interpoalte / extrapolate graph
	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.frameCount - 1 - a ) & (LAG_SAMPLES - 1);
		v = lagometer.frameSamples[i];
		v *= vscale;
		if ( v > 0 ) {
			if ( color != 1 ) {
				color = 1;
				trap_R_SetColor( colorYellow );
			}
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic ( ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 2 ) {
				color = 2;
				trap_R_SetColor( colorBlue );
			}
			v = -v;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	// draw the snapshot latency / drop graph
	range = ah / 2;
	vscale = range / MAX_LAGOMETER_PING;

	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.snapshotCount - 1 - a ) & (LAG_SAMPLES - 1);
		v = lagometer.snapshotSamples[i];
		if ( v > 0 ) {
			if ( lagometer.snapshotFlags[i] & SNAPFLAG_RATE_DELAYED ) {
				if ( color != 5 ) {
					color = 5;	// YELLOW for rate delay
					trap_R_SetColor( colorYellow );
				}
			} else {
				if ( color != 3 ) {
					color = 3;
					trap_R_SetColor( colorGreen );
				}
			}
			v = v * vscale;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 4 ) {
				color = 4;		// RED for dropped snapshots
				trap_R_SetColor( colorRed );
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	trap_R_SetColor( NULL );

	if ( cg_nopredict.integer 
#ifdef ALLOW_GSYNC
		|| cg_synchronousClients.integer 
#endif // ALLOW_GSYNC
		) {
		CG_DrawBigString( ax, ay, "snc", 1.0 );
	}

	CG_DrawDisconnect();
}


void CG_DrawLivesLeft( void ) {
	if( cg_gameType.integer == GT_WOLF_LMS ) {
		return;
	}

	if( cg.snap->ps.persistant[PERS_RESPAWNS_LEFT] < 0 ) {
		return;
	}

	if ( cg.hud.livesleft[0] < 0 ) {
		return;
	}

	CG_DrawPic( cg.hud.livesleft[0], cg.hud.livesleft[1],
		//48,
		(int)(cg.hud.livesleft[2] * 3.4f),
		//24,
		(int)(cg.hud.livesleft[2] * 1.7f),
		cg.snap->ps.persistant[PERS_TEAM] == TEAM_ALLIES ? 
				cgs.media.hudAlliedHelmet : cgs.media.hudAxisHelmet );

	CG_DrawField(
		//cg.hud.livesleft[0]+40,
		(cg.hud.livesleft[0] + (cg.hud.livesleft[2] * 2.86f)),
		cg.hud.livesleft[1],
		3,
		cg.snap->ps.persistant[PERS_RESPAWNS_LEFT], 
		//14,
		cg.hud.livesleft[2],
		//20,
		(int)(cg.hud.livesleft[2] * 1.43f),
		qtrue, qtrue );
}

/*
===============================================================================

CENTER PRINTING

===============================================================================
*/


/*
==============
CG_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
#define CP_LINEWIDTH 56			// NERVE - SMF

void CG_CenterPrint( const char *str, int y, int charWidth ) {
	char	*s;
	int		i, len;						// NERVE - SMF
	qboolean neednewline = qfalse;		// NERVE - SMF
	int priority = 0;

	// NERVE - SMF - don't draw if this print message is less important
	if ( cg.centerPrintTime && priority < cg.centerPrintPriority )
		return;

	Q_strncpyz( cg.centerPrint, str, sizeof(cg.centerPrint) );
	cg.centerPrintPriority = priority;	// NERVE - SMF

	// NERVE - SMF - turn spaces into newlines, if we've run over the linewidth
	len = strlen( cg.centerPrint );
	for ( i = 0; i < len; i++ ) {

		// NOTE: subtract a few chars here so long words still get displayed properly
		if ( i % ( CP_LINEWIDTH - 20 ) == 0 && i > 0 )
			neednewline = qtrue;
		if ( cg.centerPrint[i] == ' ' && neednewline ) {
			cg.centerPrint[i] = '\n';
			neednewline = qfalse;
		}
	}
	// -NERVE - SMF

	cg.centerPrintTime = cg.time;
	cg.centerPrintY = y;
	cg.centerPrintCharWidth = charWidth;

	// count the number of lines for centering
	cg.centerPrintLines = 1;
	s = cg.centerPrint;
	while( *s ) {
		if (*s == '\n')
			cg.centerPrintLines++;
		s++;
	}
}

// NERVE - SMF
/*
==============
CG_PriorityCenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void CG_PriorityCenterPrint( const char *str, int y, int charWidth, int priority ) {
	char	*s;
	int		i, len;						// NERVE - SMF
	qboolean neednewline = qfalse;		// NERVE - SMF

	// NERVE - SMF - don't draw if this print message is less important
	if( cg.centerPrintTime && priority < cg.centerPrintPriority )
		return;

	Q_strncpyz( cg.centerPrint, str, sizeof(cg.centerPrint) );
	cg.centerPrintPriority = priority;	// NERVE - SMF

	// NERVE - SMF - turn spaces into newlines, if we've run over the linewidth
	len = strlen( cg.centerPrint );
	for ( i = 0; i < len; i++ ) {

		// NOTE: subtract a few chars here so long words still get displayed properly
		if ( i % ( CP_LINEWIDTH - 20 ) == 0 && i > 0 )
			neednewline = qtrue;
		if ( cg.centerPrint[i] == ' ' && neednewline ) {
			cg.centerPrint[i] = '\n';
			neednewline = qfalse;
		}
	}
	// -NERVE - SMF

	cg.centerPrintTime = cg.time + 2000;
	cg.centerPrintY = y;
	cg.centerPrintCharWidth = charWidth;

	// count the number of lines for centering
	cg.centerPrintLines = 1;
	s = cg.centerPrint;
	while( *s ) {
		if (*s == '\n')
			cg.centerPrintLines++;
		s++;
	}
}
// -NERVE - SMF

/*
===================
CG_DrawCenterString
===================
*/
static void CG_DrawCenterString( void ) {
	char	*start;
	int		l;
	int		x, y, w;
	float	*color;

	if ( !cg.centerPrintTime ) {
		return;
	}

	color = CG_FadeColor( cg.centerPrintTime, 1000 * cg_centertime.value );
	if ( !color ) {
		cg.centerPrintTime = 0;
		cg.centerPrintPriority = 0;
		return;
	}

	trap_R_SetColor( color );

	start = cg.centerPrint;

	y = cg.centerPrintY - cg.centerPrintLines * BIGCHAR_HEIGHT / 2;

	while ( 1 ) {
		char linebuffer[1024];

		for ( l = 0; l < CP_LINEWIDTH; l++ ) {			// NERVE - SMF - added CP_LINEWIDTH
			if ( !start[l] || start[l] == '\n' ) {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

		w = cg.centerPrintCharWidth * CG_DrawStrlen( CG_TranslateString(linebuffer) );

		x = ( SCREEN_WIDTH - w ) / 2;

		CG_DrawStringExt( x, y, linebuffer, color, qfalse, qtrue, cg.centerPrintCharWidth, (int)(cg.centerPrintCharWidth * 1.5), 0 );

		y += cg.centerPrintCharWidth * 1.5;

		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	trap_R_SetColor( NULL );
}



/*
================================================================================

CROSSHAIRS

================================================================================
*/

/*
==============
CG_DrawWeapReticle
==============
*/

static void CG_DrawWeapReticle(void) {
	vec4_t		color = {0, 0, 0, 1};
	qboolean	fg, garand, k43;
	centity_t	*ent = &cg_entities[cg.snap->ps.clientNum];

	//if( cg.weaponSelectTime + 1000 > cg.time )
	if(cg.time - cg.weaponSelectTime < cg_weaponCycleDelay.integer * 2.5)
	{
		return;	// force pause so holding it down won't go too fast
	}

	if (!ent)
		return;

	if (ent->currentState.eFlags & EF_DEAD)
		return;

	if (ent->currentState.eFlags & EF_NODRAW)
		return;

	if (ent->currentState.teamNum == TEAM_SPECTATOR)
		return;

	if (ent->currentState.eType == ET_CORPSE)
		return;

#ifdef __NPC__
	if( ent->currentState.eType == ET_NPC_CORPSE )
		return;
#endif //__NPC__

#if defined (__FRONTLINE__) /*|| defined (__EF__)*/
	if ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback ) {
		garand = (qboolean)(cg.snap->ps.weapon == WP_GARAND);
		k43 = (qboolean)(cg.snap->ps.weapon == WP_K43);
		fg = (qboolean)(cg.snap->ps.weapon == WP_FG42);
	} else {
		fg = (qboolean)(cg.weaponSelect == WP_FG42);
		garand = (qboolean)(cg.weaponSelect == WP_GARAND);
		k43 = (qboolean)(cg.weaponSelect == WP_K43);
	}
#else //!defined (__FRONTLINE__) /*|| defined (__EF__)*/
	// DHM - Nerve :: So that we will draw reticle
	if ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback ) {
		garand = (qboolean)(cg.snap->ps.weapon == WP_GARAND_SCOPE);
		k43 = (qboolean)(cg.snap->ps.weapon == WP_K43_SCOPE);
		fg = (qboolean)(cg.snap->ps.weapon == WP_FG42SCOPE);
	} else {
		fg = (qboolean)(cg.weaponSelect == WP_FG42SCOPE);
		garand = (qboolean)(cg.weaponSelect == WP_GARAND_SCOPE);
		k43 = (qboolean)(cg.weaponSelect == WP_K43_SCOPE);
	}
#endif //defined (__FRONTLINE__) /*|| defined (__EF__)*/

#ifdef __OLD__
	if(fg) {
		// sides
		CG_FillRect (0, 0, 80, 480, color);
		CG_FillRect (560, 0, 80, 480, color);

		// center
		if(cgs.media.reticleShaderSimple)
			CG_DrawPic( 80, 0, 480, 480, cgs.media.reticleShaderSimple );

/*		if(cgs.media.reticleShaderSimpleQ) {
			trap_R_DrawStretchPic( x,	0, w, h, 0, 0, 1, 1, cgs.media.reticleShaderSimpleQ );	// tl
			trap_R_DrawStretchPic( x+w, 0, w, h, 1, 0, 0, 1, cgs.media.reticleShaderSimpleQ );	// tr
			trap_R_DrawStretchPic( x,	h, w, h, 0, 1, 1, 0, cgs.media.reticleShaderSimpleQ );	// bl
			trap_R_DrawStretchPic( x+w, h, w, h, 1, 1, 0, 0, cgs.media.reticleShaderSimpleQ );	// br
		}*/

		// hairs
		CG_FillRect (84, 239, 150, 3, color);	// left
		CG_FillRect (234, 240, 173, 1, color);	// horiz center
		CG_FillRect (407, 239, 150, 3, color);	// right


		CG_FillRect (319, 2,   3, 151, color);	// top center top
		CG_FillRect (320, 153, 1, 114, color);	// top center bot

		CG_FillRect (320, 241, 1, 87, color);	// bot center top
		CG_FillRect (319, 327, 3, 151, color);	// bot center bot
	} else if(garand) {
		// sides
		CG_FillRect (0, 0, 80, 480, color);
		CG_FillRect (560, 0, 80, 480, color);

		// center
		if(cgs.media.reticleShaderSimple)
			CG_DrawPic( 80, 0, 480, 480, cgs.media.reticleShaderSimple );

		// hairs
		CG_FillRect (84, 239, 177, 2, color);	// left
		CG_FillRect (320, 242, 1, 58, color);	// center top
		CG_FillRect (319, 300, 2, 178, color);	// center bot
		CG_FillRect (380, 239, 177, 2, color);	// right
	} else if (k43) {
		// sides
		CG_FillRect (0, 0, 80, 480, color);
		CG_FillRect (560, 0, 80, 480, color);

		// center
		if(cgs.media.reticleShaderSimple)
			CG_DrawPic( 80, 0, 480, 480, cgs.media.reticleShaderSimple );

		// hairs
		CG_FillRect (84, 239, 177, 2, color);	// left
		CG_FillRect (320, 242, 1, 58, color);	// center top
		CG_FillRect (319, 300, 2, 178, color);	// center bot
		CG_FillRect (380, 239, 177, 2, color);	// right
	}
#else //!__OLD__
	if(fg) {
		refdef_t	refdef_backup;

		memcpy(&refdef_backup, cg.refdef_current, sizeof(refdef_t));

		CG_DrawActiveFrame( cg.time, 0, cg.demoPlayback, qtrue );

		// center
		if(cgs.media.reticleShaderSimple)
			CG_DrawPic( 80, 12 + ent->snipe_render_view_height_addition, 480, 480, cgs.media.reticleShaderSimple );

		//if(cgs.media.reticleShaderOverlay)
		//	CG_DrawPic( 179, 101, 280, 280, cgs.media.reticleShaderOverlay );

		// hairs
		CG_FillRect (179, 241 + ent->snipe_render_view_height_addition, 280, 1, color);	// horizontal
		CG_FillRect (321, 136 + ent->snipe_render_view_height_addition, 1, 210, color);	// vertical

		memcpy(cg.refdef_current, &refdef_backup, sizeof(refdef_t));
	} else if(garand) {
		refdef_t	refdef_backup;

		memcpy(&refdef_backup, cg.refdef_current, sizeof(refdef_t));

		CG_DrawActiveFrame( cg.time, 0, cg.demoPlayback, qtrue );

		// center
		if(cgs.media.reticleShaderSimple)
			CG_DrawPic( 80, 12 + ent->snipe_render_view_height_addition, 480, 480, cgs.media.reticleShaderSimple );

		//if(cgs.media.reticleShaderOverlay)
		//	CG_DrawPic( 179, 101, 280, 280, cgs.media.reticleShaderOverlay );

		// hairs
		CG_FillRect (179, 241 + ent->snipe_render_view_height_addition, 280, 1, color);	// horizontal
		CG_FillRect (321, 136 + ent->snipe_render_view_height_addition, 1, 210, color);	// vertical

		memcpy(cg.refdef_current, &refdef_backup, sizeof(refdef_t));
	} else if (k43) {
		refdef_t	refdef_backup;

		memcpy(&refdef_backup, cg.refdef_current, sizeof(refdef_t));

		CG_DrawActiveFrame( cg.time, 0, cg.demoPlayback, qtrue );

		// center
		if(cgs.media.reticleShaderSimple)
			CG_DrawPic( 80, 12 + ent->snipe_render_view_height_addition, 480, 480, cgs.media.reticleShaderSimple );

		//if(cgs.media.reticleShaderOverlay)
		//	CG_DrawPic( 179, 101, 280, 280, cgs.media.reticleShaderOverlay );

		// hairs
		CG_FillRect (179, 241 + ent->snipe_render_view_height_addition, 280, 1, color);	// horizontal
		CG_FillRect (321, 136 + ent->snipe_render_view_height_addition, 1, 210, color);	// vertical

		memcpy(cg.refdef_current, &refdef_backup, sizeof(refdef_t));
	}
#endif //__OLD__
}

/*
==============
CG_DrawMortarReticle
==============
*/
static void CG_DrawMortarReticle( void ) {
	vec4_t	color = { 1.f, 1.f, 1.f, .5f };
	vec4_t	color_back = { 0.f, 0.f, 0.f, .25f };
	vec4_t	color_extends = { .77f, .73f, .1f, 1.f };
	vec4_t	color_lastfire = { .77f, .1f, .1f, 1.f };
	//vec4_t	color_firerequest = { .23f, 1.f, .23f, 1.f };
	vec4_t	color_firerequest = { 1.f, 1.f, 1.f, 1.f };
	float	offset, localOffset;
	int		i, min, majorOffset, val, printval, fadeTime;
	char	*s;
	float	angle, angleMin, angleMax;
	qboolean hasRightTarget, hasLeftTarget;

	// Background
	CG_FillRect( 136, 236, 154, 38, color_back );
	CG_FillRect( 290, 160, 60, 208, color_back );
	CG_FillRect( 350, 236, 154, 38, color_back );

	// Horizontal bar

	// bottom
	CG_FillRect( 140, 264, 150, 1, color);	// left
	CG_FillRect( 350, 264, 150, 1, color);	// right

	// 10 units - 5 degrees
	// total of 360 units
	// nothing displayed between 150 and 210 units
	// 360 / 10 = 36 bits, means 36 * 5 = 180 degrees
	// that means left is cg.predictedPlayerState.viewangles[YAW] - .5f * 180
	angle = 360 - AngleNormalize360(cg.predictedPlayerState.viewangles[YAW] - 90.f);

	offset = (5.f / 65536) * ((int)(angle * (65536 / 5.f)) & 65535);
	min = (int)(AngleNormalize360(angle - .5f * 180) / 15.f) * 15;
	majorOffset = (int)(floor((int)floor(AngleNormalize360(angle - .5f * 180)) % 15) / 5.f );

	for( val = i = 0; i < 36; i++ ) {
		localOffset = i * 10.f + (offset * 2.f);

		if( localOffset >= 150 && localOffset <= 210 ) {
			if( i % 3 == majorOffset)
				val++;
			continue;
		}

		if( i % 3 == majorOffset) {
			printval = min - val * 15 + 180;
			
			// rain - old tertiary abuse was nasty and had undefined result
			if (printval < 0)
				printval += 360;
			else if (printval >= 360)
				printval -= 360;

			s = va( "%i", printval );
			//CG_Text_Paint_Ext( 140 + localOffset - .5f * CG_Text_Width_Ext( s, .15f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 ), 244, .15f, .15f, color, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
			//CG_FillRect( 140 + localOffset, 248, 1, 16, color);
			CG_Text_Paint_Ext( 500 - localOffset - .5f * CG_Text_Width_Ext( s, .15f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 ), 244, .15f, .15f, color, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
			CG_FillRect( 500 - localOffset, 248, 1, 16, color);
			val++;
		} else {
			//CG_FillRect( 140 + localOffset, 256, 1, 8, color);
			CG_FillRect( 500 - localOffset, 256, 1, 8, color);			
		}
	}

	// the extremes
	// 30 degrees plus a 15 degree border
	angleMin = AngleNormalize360(360 - (cg.pmext.mountedWeaponAngles[YAW] - 90.f) - (30.f + 15.f));
	angleMax = AngleNormalize360(360 - (cg.pmext.mountedWeaponAngles[YAW] - 90.f) + (30.f + 15.f));

	// right
	localOffset = (AngleNormalize360(angle - angleMin) / 5.f ) * 10.f;
	//CG_FillRect( 320 + localOffset, 252, 2, 18, color_extends);
	CG_FillRect( 320 - localOffset, 252, 2, 18, color_extends);

	// left
	localOffset = (AngleNormalize360(angleMax - angle) / 5.f ) * 10.f;
	//CG_FillRect( 320 - localOffset, 252, 2, 18, color_extends);
	CG_FillRect( 320 + localOffset, 252, 2, 18, color_extends);

	// last fire pos
	fadeTime = 0;
	if( cg.lastFiredWeapon == WP_MORTAR_SET && cg.mortarImpactTime >= -1 ) {
		fadeTime = cg.time - (cg.predictedPlayerEntity.muzzleFlashTime + 5000);

		if( fadeTime < 3000 ) {
			float lastfireAngle;

			if( fadeTime > 0 ) {
				color_lastfire[3] = 1.f - (fadeTime/3000.f);
			}

			lastfireAngle = AngleNormalize360(360 - (cg.mortarFireAngles[YAW] - 90.f));

			localOffset = ( ( AngleSubtract( angle, lastfireAngle ) ) / 5.f ) * 10.f;
			//CG_FillRect( 320 + localOffset, 252, 2, 18, color_lastfire);
			CG_FillRect( 320 - localOffset, 252, 2, 18, color_lastfire);
		}
	}

	// mortar attack requests
	hasRightTarget = hasLeftTarget = qfalse;
	for( i = 0; i < MAX_CLIENTS; i++ ) {
		int requestFadeTime = cg.time - (cg.artilleryRequestTime[i] + 25000);

		if( requestFadeTime < 5000 ) {
			vec3_t dir;
			float yaw;
			float attackRequestAngle;

			VectorSubtract( cg.artilleryRequestPos[i], cg.predictedPlayerEntity.lerpOrigin, dir );

			// ripped this out of vectoangles
			if( dir[1] == 0 && dir[0] == 0 ) {
				yaw = 0;
			} else {
				if( dir[0] ) {
					yaw = ( atan2 ( dir[1], dir[0] ) * 180 / M_PI );
				}
				else if ( dir[1] > 0 ) {
					yaw = 90;
				}
				else {
					yaw = 270;
				}
				if ( yaw < 0 ) {
					yaw += 360;
				}
			}

			if( requestFadeTime > 0 ) {
				color_firerequest[3] = 1.f - (requestFadeTime/5000.f);
			}

			attackRequestAngle = AngleNormalize360(360 - (yaw - 90.f));

			yaw = AngleSubtract( attackRequestAngle, angleMin );

			if( yaw < 0 ) {
				if( !hasLeftTarget ) {
					//CG_FillRect( 136 + 2, 236 + 38 - 6, 4, 4, color_firerequest );

					trap_R_SetColor( color_firerequest );
					CG_DrawPic( 136 + 2, 236 + 38 - 10 + 1, 8, 8, cgs.media.ccMortarTargetArrow );
					trap_R_SetColor( NULL );

					hasLeftTarget = qtrue;
				}
			} else if( yaw > 90 ) {
				if( !hasRightTarget ) {
					//CG_FillRect( 350 + 154 - 6, 236 + 38 - 6, 4, 4, color_firerequest );

					trap_R_SetColor( color_firerequest );
					CG_DrawPic( 350 + 154 - 10, 236 + 38 - 10 + 1, -8, 8, cgs.media.ccMortarTargetArrow );
					trap_R_SetColor( NULL );

					hasRightTarget = qtrue;
				}
			} else {
				localOffset = ( ( AngleSubtract( angle, attackRequestAngle ) ) / 5.f ) * 10.f;
				//CG_FillRect( 320 + localOffset - 3, 264 - 3, 6, 6, color_firerequest );

				trap_R_SetColor( color_firerequest );
 				//CG_DrawPic( 320 + localOffset - 8, 264 - 8, 16, 16, cgs.media.ccMortarTarget );
				CG_DrawPic( 320 - localOffset - 8, 264 - 8, 16, 16, cgs.media.ccMortarTarget );
				trap_R_SetColor( NULL );
			}
		}
	}

 	/*s = va( "%.2f (%i / %i)",AngleNormalize360(angle - .5f * 180), majorOffset, min );
	CG_Text_Paint( 140, 224, .25f, color, s, 0, 0, 0 );
	s = va( "%.2f",AngleNormalize360(angle) );
	CG_Text_Paint( 320 - .5f * CG_Text_Width( s, .25f, 0), 224, .25f, color, s, 0, 0, 0 );
	s = va( "%.2f", AngleNormalize360(angle + .5f * 180) );
	CG_Text_Paint( 500 - CG_Text_Width( s, .25f, 0 ), 224, .25f, color, s, 0, 0, 0 );*/

	// Vertical bar

	// sides
	CG_FillRect( 295, 164, 1, 200, color);	// left
	CG_FillRect( 345, 164, 1, 200, color);	// right

	// 10 units - 2.5 degrees
	// total of 200 units
	// 200 / 10 = 20 bits, means 20 * 2.5 = 50 degrees
	// that means left is cg.predictedPlayerState.viewangles[PITCH] - .5f * 50
	angle = AngleNormalize180(360 - (cg.predictedPlayerState.viewangles[PITCH] - 60));

	offset = (2.5f / 65536) * ((int)(angle * (65536 / 2.5f)) & 65535);
	min = floor((angle + .5f * 50) / 10.f) * 10;
	majorOffset = (int)(floor((int)((angle + .5f * 50) * 10.f) % 100) / 25.f );

	for( val = i = 0; i < 20; i++ ) {
		localOffset = i * 10.f + (offset * 4.f);

		/*if( localOffset >= 150 && localOffset <= 210 ) {
			if( i % 3 == majorOffset)
				val++;
			continue;
		}*/

		if( i % 4 == majorOffset ) {
			printval = min - val * 10;
			
			// rain - old tertiary abuse was nasty and had undefined result
			if (printval <= -180)
				printval += 360;
			else if (printval >= 180)
				printval -= 180;

			s = va( "%i", printval );
			CG_Text_Paint_Ext( 320 - .5f * CG_Text_Width_Ext( s, .15f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 ), 164 + localOffset + .5f * CG_Text_Height_Ext( s, .15f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 ), .15f, .15f, color, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
			CG_FillRect( 295 + 1, 164 + localOffset, 12, 1, color);
			CG_FillRect( 345 - 12, 164 + localOffset, 12, 1, color);
			val++;
		} else {
			CG_FillRect( 295 + 1, 164 + localOffset, 8, 1, color);
			CG_FillRect( 345 - 8, 164 + localOffset, 8, 1, color);
		}
	}

	// the extremes
	// 30 degrees up
	// 20 degrees down
	angleMin = AngleNormalize180(360 - (cg.pmext.mountedWeaponAngles[PITCH] - 60)) - 20.f;
	angleMax = AngleNormalize180(360 - (cg.pmext.mountedWeaponAngles[PITCH] - 60)) + 30.f;

	// top
	localOffset = angleMax - angle;
	if( localOffset < 0 )
		localOffset = 0;
	localOffset = (AngleNormalize360(localOffset) / 2.5f ) * 10.f;
	if( localOffset < 100 ) {
		CG_FillRect( 295 - 2, 264 - localOffset, 6, 2, color_extends);
		CG_FillRect( 345 - 4 + 1, 264 - localOffset, 6, 2, color_extends);
	}

	// bottom
	localOffset = angle - angleMin;
	if( localOffset < 0 )
		localOffset = 0;
	localOffset = (AngleNormalize360(localOffset) / 2.5f ) * 10.f;
	if( localOffset < 100 ) {
		CG_FillRect( 295 - 2, 264 + localOffset, 6, 2, color_extends);
		CG_FillRect( 345 - 4 + 1, 264 + localOffset, 6, 2, color_extends);
	}

	// last fire pos
	if( cg.lastFiredWeapon == WP_MORTAR_SET && cg.mortarImpactTime >= -1 ) {
		if( fadeTime < 3000 ) {
			float lastfireAngle;

			lastfireAngle = AngleNormalize180(360 - (cg.mortarFireAngles[PITCH] - 60));

			if( lastfireAngle > angle ) {
				localOffset = lastfireAngle - angle;
				if( localOffset < 0 )
					localOffset = 0;
				localOffset = (AngleNormalize360(localOffset) / 2.5f ) * 10.f;
				if( localOffset < 100 ) {
					CG_FillRect( 295 - 2, 264 - localOffset, 6, 2, color_lastfire);
					CG_FillRect( 345 - 4 + 1, 264 - localOffset, 6, 2, color_lastfire);
				}
			} else {
				localOffset = angle - lastfireAngle;
				if( localOffset < 0 )
					localOffset = 0;
				localOffset = (AngleNormalize360(localOffset) / 2.5f ) * 10.f;
				if( localOffset < 100 ) {
					CG_FillRect( 295 - 2, 264 + localOffset, 6, 2, color_lastfire);
					CG_FillRect( 345 - 4 + 1, 264 + localOffset, 6, 2, color_lastfire);
				}
			}
		}
	}
 
	/*s = va( "%.2f (%i / %i)", angle + .5f * 50, majorOffset, min );
	CG_Text_Paint( 348, 164, .25f, color, s, 0, 0, 0 );
	s = va( "%.2f",angle );
	CG_Text_Paint( 348, 264, .25f, color, s, 0, 0, 0 );
	s = va( "%.2f", angle - .5f * 50 );
	CG_Text_Paint( 348, 364, .25f, color, s, 0, 0, 0 );*/
}

/*
==============
CG_DrawBinocReticle
==============
*/
static void CG_DrawBinocReticle(void) {
	// an alternative.  This gives nice sharp lines at the expense of a few extra polys
	vec4_t	color;
	color[0] = color[1] = color[2] = 0;
	color[3] = 1;

	if(cgs.media.binocShaderSimple)
		CG_DrawPic( 0, 0, 640, 480, cgs.media.binocShaderSimple );

	CG_FillRect (146, 239, 348, 1, color);

	CG_FillRect (188, 234, 1, 13, color);	// ll
	CG_FillRect (234, 226, 1, 29, color);	// l
	CG_FillRect (274, 234, 1, 13, color);	// lr
	CG_FillRect (320, 213, 1, 55, color);	// center
	CG_FillRect (360, 234, 1, 13, color);	// rl
	CG_FillRect (406, 226, 1, 29, color);	// r
	CG_FillRect (452, 234, 1, 13, color);	// rr
}

void CG_FinishWeaponChange(int lastweap, int newweap); // JPW NERVE

qboolean CG_WorldCoordToScreenCoord( vec3_t worldCoord, int *x, int *y )
{
	float	xF, yF;
	qboolean retVal = CG_WorldCoordToScreenCoordFloat( worldCoord, &xF, &yF );
	*x = (int)xF;
	*y = (int)yF;
	return retVal;
}

// JKII-III style crosshair

qboolean CG_WorldCoordToScreenCoordFloat(vec3_t worldCoord, float *x, float *y)
{
	float	xcenter, ycenter;
	vec3_t	local, transformed;
	vec3_t	vfwd;
	vec3_t	vright;
	vec3_t	vup;
	float xzi;
	float yzi;

//	xcenter = cg.refdef.width / 2;//gives screen coords adjusted for resolution
//	ycenter = cg.refdef.height / 2;//gives screen coords adjusted for resolution
	
	//NOTE: did it this way because most draw functions expect virtual 640x480 coords
	//	and adjust them for current resolution
	xcenter = 640.0f / 2.0f;//gives screen coords in virtual 640x480, to be adjusted when drawn
	ycenter = 480.0f / 2.0f;//gives screen coords in virtual 640x480, to be adjusted when drawn

	AngleVectors (cg.refdefViewAngles, vfwd, vright, vup); 
	VectorSubtract (worldCoord, cg.refdef.vieworg, local);

	transformed[0] = DotProduct(local,vright);
	transformed[1] = DotProduct(local,vup);
	transformed[2] = DotProduct(local,vfwd);		

	// Make sure Z is not negative.
	if(transformed[2] < 0.01f)
	{
		return qfalse;
	}

	xzi = xcenter / transformed[2] * (96.0f/cg.refdef.fov_x);
	yzi = ycenter / transformed[2] * (102.0f/cg.refdef.fov_y);

	*x = xcenter + xzi * transformed[0];
	*y = ycenter - yzi * transformed[1];

	return qtrue;
}

#ifdef __VEHICLES__
/*
=================
CG_DrawCrosshair
=================
*/
float crosshair_angle = 0.0f;
int crosshair_angle_timer = 0;
vec3_t cg_crosshairPos={0,0,0};
static void CG_DrawCrosshair(vec3_t worldPoint) {
	float		w, h;
	qhandle_t	hShader;
	float		f;
	float		x, y ; 
	int			weapnum;		// DHM - Nerve
	float		chX, chY;


	if ( worldPoint )
	{
		VectorCopy( worldPoint, cg_crosshairPos );
	}

#if defined (__FRONTLINE__) || defined (__EF__)
	if (cg.zoomedSniper && !(cg.snap->ps.eFlags & EF_AIMING) && (cg.snap->ps.eFlags & EF_ZOOMING))
	{
		if(cg.mvTotalClients < 1 || cg.snap->ps.stats[STAT_HEALTH] > 0)
		{
			CG_DrawWeapReticle();
		}
		
		return;
	}

	if ( (cg.predictedPlayerState.eFlags & EF_AIMING) && !cg.renderingThirdPerson )
		return;

	if ( cg.snap->ps.pm_flags & PMF_TIME_LOCKPLAYER ) {
		return;
	}
#endif //defined (__FRONTLINE__) || defined (__EF__)

	// using binoculars
	if (!(cg.predictedPlayerState.eFlags & EF_AIMING))
	{
		if(cg.zoomedBinoc  /*&& cg.weaponSelect == WP_BINOCULARS*/) {
			CG_DrawBinocReticle();
			return;
		}
	}

	// DHM - Nerve :: show reticle in limbo and spectator
	if ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback )
		weapnum = cg.snap->ps.weapon;
	else
		weapnum = cg.weaponSelect;


	switch(weapnum) {

		// weapons that get no reticle
		case WP_NONE:	// no weapon, no crosshair
			if (!(cg.predictedPlayerState.eFlags & EF_AIMING))
			{
				if(cg.zoomedBinoc /*&& cg.weaponSelect == WP_BINOCULARS*/)
					CG_DrawBinocReticle();
			}

			if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR )
				return;
			break;

		// special reticle for weapon
		case WP_FG42SCOPE:
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
			if(!BG_PlayerMounted(cg.snap->ps.eFlags)) { //1
				// JPW NERVE -- don't let players run with rifles -- speed 80 == crouch, 128 == walk, 256 == run
				if (VectorLengthSquared(cg.snap->ps.velocity) > SQR(127)) { //2
						if( cg.snap->ps.weapon == WP_FG42SCOPE ) {//3
							CG_FinishWeaponChange( WP_FG42SCOPE, WP_FG42 );
						}//3
						if( cg.snap->ps.weapon == WP_GARAND_SCOPE ) {//4
							CG_FinishWeaponChange( WP_GARAND_SCOPE, WP_GARAND );
						}//4
						if( cg.snap->ps.weapon == WP_K43_SCOPE ) {//5
							CG_FinishWeaponChange( WP_K43_SCOPE, WP_K43 );
						}//5
					}//2
				
				// OSP
				if(cg.mvTotalClients < 1 || cg.snap->ps.stats[STAT_HEALTH] > 0)
				{
					CG_DrawWeapReticle();
				}
				return;
			}// ?!?!
			break;

		default:
			break;
	}

/*#ifdef __FRONTLINE__
	}


	if (cg_crosshair_allowed.integer == 2 ){ // server disallowed any onhud crosshairs
		return;
	}

	if ( cg_3rdPersonAngle.value !=0 &&  cg.renderingThirdPerson ) {
		return;
		}
#endif*/

	if( cg.predictedPlayerState.eFlags & EF_PRONE_MOVING ) {
		return;
	}

	// FIXME: spectators/chasing?
	if( cg.predictedPlayerState.weapon == WP_MORTAR_SET && cg.predictedPlayerState.weaponstate != WEAPON_RAISING ) {
		CG_DrawMortarReticle();
		return;
	}

#if defined (__FRONTLINE__) || defined (__EF__)
/*	if ( ((cg.predictedPlayerState.leanf != 0) || (cg_crosshair_allowed.integer == 0)) && !  cg.renderingThirdPerson  ){ // server can disallow crosshairs, but not for thirdperson...
		if ( !(cg.predictedPlayerState.eFlags & EF_AAGUN_ACTIVE) ) {
		return;
		}
	}*/

	if ( cg.predictedPlayerState.leanf != 0 )
		return; // UQ1: No crosshair while leaning...

	if( cg.predictedPlayerState.pm_flags & PMF_LADDER) 
	{
		return;
	}
#endif //defined (__FRONTLINE__) || defined (__EF__)

	if ( cg_drawCrosshair.integer < 0 || g_realism.integer > 1 )	//----(SA)	moved down so it doesn't keep the scoped weaps from drawing reticles
		return;

	// TAT 1/10/2003 - Don't draw crosshair if have exit hintcursor
	if (cg.snap->ps.serverCursorHint >= HINT_EXIT && cg.snap->ps.serverCursorHint <= HINT_NOEXIT )
		return;
/// alt crosshair
/*#ifdef __FRONTLINE__

	if ( cg_drawAltCrosshair.integer == 1 ) { // if we have enabled alternative crosshair , type one , CoD style
	vec4_t	color ;
	float		f;
	float		x, y ;
	float		w, h;


	if ( cg_crosshairHealth.integer ) {
	CG_ColorForHealth( color ); // coloring for crosshairhealth
	} 
	

	w =  cg_crosshairSize.value/48+0.5; // this is computed from default value
	h =  cg_crosshairSize.value/24+0.5;


	f = (float)( cg.snap->ps.aimSpreadScale/10); // hah , same func as normal crosshair

	if ( worldPoint && VectorLength( worldPoint ) )
	{
		if ( !CG_WorldCoordToScreenCoordFloat( worldPoint, &x, &y ) )
		{//off screen, don't draw it
			return;
		}
		x -= 320;
		y -= 240;
	}
	else
	{
		x = cg_crosshairX.integer;
		y = cg_crosshairY.integer;
	}

	x = x + cg.refdef.x + 0.5 * (640 - w);
	y = y + cg.refdef.y + 0.5 * (480 - h);

	if ( cg_crosshairHealth.integer ) { // simply , we want to use crosshairhealth info
	CG_FillRect (x, y+f+(h*5), w, h, color);// lower
	CG_FillRect (x+f+(h*5), y, h, w, color);// right
	CG_FillRect (x-f-(h*5.5), y, h, w, color);// left
	CG_FillRect (x, y-f-(h*5.5), w, h, color);// upper
	} else {	// or crosshaircolor info
	CG_FillRect (x, y+f+(h*5), w, h, cg.xhairColor);// lower
	CG_FillRect (x+f+(h*5), y, h, w, cg.xhairColor);// right
	CG_FillRect (x-f-(h*5.5), y, h, w, cg.xhairColor);// left
	CG_FillRect (x, y-f-(h*5.5), w, h, cg.xhairColor);// upper
	}
	return;	// other crosshairs will not be drawn if we have altcrosshair enabled
	}

	if ( cg_drawAltCrosshair.integer == 2 ) { // if we have enabled alternative crosshair , type 2 , CS style
	vec4_t	color ;
	float		f;
	float		x, y ;
	float		w, h;


	if ( cg_crosshairHealth.integer ) {
	CG_ColorForHealth( color ); // coloring for crosshairhealth
	} 
	

	w =  cg_crosshairSize.value/48+0.5;
	h =  cg_crosshairSize.value/4.8+0.5;


	f = (float)( cg.snap->ps.aimSpreadScale/10);


	if ( worldPoint && VectorLength( worldPoint ) )
	{
		if ( !CG_WorldCoordToScreenCoordFloat( worldPoint, &x, &y ) )
		{//off screen, don't draw it
			return;
		}
		x -= 320;
		y -= 240;
	}
	else
	{
		x = cg_crosshairX.integer;
		y = cg_crosshairY.integer;
	}

	x = x + cg.refdef.x + 0.5 * (640 - w);
	y = y + cg.refdef.y + 0.5 * (480 - h);


	if ( cg_crosshairHealth.integer ) { // simply , we want to use crosshairhealth info
	CG_FillRect (x, y+f+h, w, h, color);// lower
	CG_FillRect (x+f+h, y, h, w, color);// right
	CG_FillRect (x-f-(h*2), y, h, w, color);// left
	CG_FillRect (x, y-f-(h*2), w, h, color);// upper
	} else {	// or crosshaircolor info
	CG_FillRect (x, y+f+h, w, h, cg.xhairColor);// lower
	CG_FillRect (x+f+h, y, h, w, cg.xhairColor);// right
	CG_FillRect (x-f-(h*2), y, h, w, cg.xhairColor);// left
	CG_FillRect (x, y-f-(h*2), w, h, cg.xhairColor);// upper
	}
	return;	// other crosshairs will not be drawn if we have altcrosshair enabled
	}
/// end of alt crosshair
#endif*/

	// set color based on health
	if ( cg_crosshairHealth.integer ) {
		vec4_t		hcolor;

		CG_ColorForHealth( hcolor );
		trap_R_SetColor( hcolor );
	} else {
		trap_R_SetColor(cg.xhairColor);
	}

	w = h = cg_crosshairSize.value;

	// RF, crosshair size represents aim spread
	f = (float)((cg_crosshairPulse.integer == 0) ? 0 : cg.snap->ps.aimSpreadScale / 255.0);
	w *= ( 1 + f*2.0 );
	h *= ( 1 + f*2.0 );


	if ( worldPoint && VectorLength( worldPoint ) )
	{
		if ( !CG_WorldCoordToScreenCoordFloat( worldPoint, &x, &y ) )
		{//off screen, don't draw it
			return;
		}
		x -= 320;
		y -= 240;
	}
	else
	{
		x = cg_crosshairX.integer;
		y = cg_crosshairY.integer;
	}

	hShader = cgs.media.crosshairShader[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ];
	chX = x + cg.refdef.x + 0.5 * (640 - w);
	chY = y + cg.refdef.y + 0.5 * (480 - h);
	CG_AdjustFrom640( &chX, &chY, &w, &h );

#ifdef __BOT__
	if (!cg.current_commander_crosshair_spin)
	{
		crosshair_angle = 0;
	}

	if (cg.current_commander_crosshair_spin)
	{
		if (cg.current_commander_order_enemy)
		{
			vec4_t colorBrightRED = { 0.9f, 0.2f, 0.2f, 0.9f };	// UQ1: Hopefully a bright red color! :)

			trap_R_SetColor(colorBrightRED);
		}

		trap_R_DrawRotatedPic( chX, chY, w, h, 0, 0, 1, 1, hShader, crosshair_angle*0.01 );
		
		crosshair_angle+=5;

		if (crosshair_angle > 360)
			crosshair_angle = 0 + (360-crosshair_angle);
	}
	else
#endif //__BOT__
		trap_R_DrawStretchPic( chX, chY, w, h, 0, 0, 1, 1, hShader );


	if ( cg.crosshairShaderAlt[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ] ) {
		w = h = cg_crosshairSize.value;


		w = h = cg_crosshairSize.value;
		chX = x + cg.refdef.x + 0.5 * (640 - w);
		chY = y + cg.refdef.y + 0.5 * (480 - h);

		CG_AdjustFrom640( &chX, &chY, &w, &h );

		if(cg_crosshairHealth.integer == 0) {
			trap_R_SetColor(cg.xhairColorAlt);
		}

#ifdef __BOT__
		if (cg.current_commander_crosshair_spin)
		{
			if (cg.current_commander_order_enemy)
			{
				vec4_t colorBrightRED = { 0.9f, 0.2f, 0.2f, 0.9f };	// UQ1: Hopefully a bright red color! :)

				trap_R_SetColor(colorBrightRED);
			}

			trap_R_DrawRotatedPic( chX, chY, w, h, 0, 0, 1, 1, cg.crosshairShaderAlt[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ], crosshair_angle*0.01 );
		}
		else
#endif //__BOT__
		trap_R_DrawStretchPic( chX, chY, w, h, 0, 0, 1, 1, cg.crosshairShaderAlt[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ] );
	}
}
#else 

/*
=================
CG_DrawCrosshair
=================
*/
static void CG_DrawCrosshair(void) {
	float		w, h;
	qhandle_t	hShader;
	float		f;
	float		x, y;
	int			weapnum;		// DHM - Nerve
//#ifdef __WEAPON_AIM__
//	playerState_t *ps = &cg.snap->ps;
//#endif //__WEAPON_AIM__

	if ( cg.renderingThirdPerson 
#ifdef __VEHICLES__
		&& !(cg.snap->ps.eFlags & EF_VEHICLE)
#endif //__VEHICLES__
		) 
	{ // @TODO JKIII crosshair
		return;
	}

	// UQ1: Until we have real sights on the models, lets keep this...
//#ifdef __WEAPON_AIM__
//	if (ps && (ps->eFlags & EF_AIMING))
//	{// No crosshair when aiming a weapon...
//		return;
//	}
//#endif //__WEAPON_AIM__

	// using binoculars
	if(cg.zoomedBinoc) {
		CG_DrawBinocReticle();
		return;
	}

	// DHM - Nerve :: show reticle in limbo and spectator
	if ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback )
		weapnum = cg.snap->ps.weapon;
	else
		weapnum = cg.weaponSelect;


	switch(weapnum) {

		// weapons that get no reticle
		case WP_NONE:	// no weapon, no crosshair
			if(cg.zoomedBinoc)
				CG_DrawBinocReticle();

			if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR )
				return;
			break;

		// special reticle for weapon
		case WP_FG42SCOPE:
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
			if(!BG_PlayerMounted(cg.snap->ps.eFlags)) {
				// JPW NERVE -- don't let players run with rifles -- speed 80 == crouch, 128 == walk, 256 == run
					if (VectorLengthSquared(cg.snap->ps.velocity) > SQR(127)) {
						if( cg.snap->ps.weapon == WP_FG42SCOPE ) {
							CG_FinishWeaponChange( WP_FG42SCOPE, WP_FG42 );
						}
						if( cg.snap->ps.weapon == WP_GARAND_SCOPE ) {
							CG_FinishWeaponChange( WP_GARAND_SCOPE, WP_GARAND );
						}
						if( cg.snap->ps.weapon == WP_K43_SCOPE ) {
							CG_FinishWeaponChange( WP_K43_SCOPE, WP_K43 );
						}
					}
				
				// OSP
				//if(cg.mvTotalClients < 1 || cg.snap->ps.stats[STAT_HEALTH] > 0)
#ifdef __WEAPON_AIM__
				if(!(cg.snap->ps.eFlags & EF_AIMING) && (cg.mvTotalClients < 1 || cg.snap->ps.stats[STAT_HEALTH] > 0))
#else //!__WEAPON_AIM__
				if(cg.mvTotalClients < 1 || cg.snap->ps.stats[STAT_HEALTH] > 0)
#endif //__WEAPON_AIM__
					CG_DrawWeapReticle();

				return;
			}
			break;
		default:
			break;
	}

	if( cg.predictedPlayerState.eFlags & EF_PRONE_MOVING ) {
		return;
	}

	// FIXME: spectators/chasing?
	if( cg.predictedPlayerState.weapon == WP_MORTAR_SET && cg.predictedPlayerState.weaponstate != WEAPON_RAISING ) {
		CG_DrawMortarReticle();
		return;
	}

	if ( cg_drawCrosshair.integer < 0 )	//----(SA)	moved down so it doesn't keep the scoped weaps from drawing reticles
		return;

	// no crosshair while leaning
	if( cg.snap->ps.leanf ) {
		return;
	}

	// TAT 1/10/2003 - Don't draw crosshair if have exit hintcursor
	if (cg.snap->ps.serverCursorHint >= HINT_EXIT && cg.snap->ps.serverCursorHint <= HINT_NOEXIT )
		return;

	// set color based on health
	if ( cg_crosshairHealth.integer ) {
		vec4_t		hcolor;

		CG_ColorForHealth( hcolor );
		trap_R_SetColor( hcolor );
	} else {
		trap_R_SetColor(cg.xhairColor);
	}

	w = h = cg_crosshairSize.value;

	// RF, crosshair size represents aim spread
	f = (float)((cg_crosshairPulse.integer == 0) ? 0 : cg.snap->ps.aimSpreadScale / 255.0);
	w *= ( 1 + f*2.0 );
	h *= ( 1 + f*2.0 );
	
	x = cg_crosshairX.integer;
	y = cg_crosshairY.integer;
	CG_AdjustFrom640( &x, &y, &w, &h );

	hShader = cgs.media.crosshairShader[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ];

	trap_R_DrawStretchPic( x + 0.5 * (cg.refdef_current->width - w), y + 0.5 * (cg.refdef_current->height - h), w, h, 0, 0, 1, 1, hShader );

	if ( cg.crosshairShaderAlt[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ] ) {
		w = h = cg_crosshairSize.value;
		x = cg_crosshairX.integer;
		y = cg_crosshairY.integer;
		CG_AdjustFrom640( &x, &y, &w, &h );

		if(cg_crosshairHealth.integer == 0) {
			trap_R_SetColor(cg.xhairColorAlt);
		}

		trap_R_DrawStretchPic( x + 0.5 * (cg.refdef_current->width - w), y + 0.5 * (cg.refdef_current->height - h), w, h, 0, 0, 1, 1, cg.crosshairShaderAlt[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ] );
	}
}
#endif

#ifdef __ETE__

int			stealth_blink_time = 0;
qboolean	stealth_blink_on = qfalse;

static void CG_DrawStealthIcon( void ) 
{
	float x, y, w, h;

	if (stealth_blink_time <= cg.time)
	{
		if (stealth_blink_on)
			stealth_blink_on = qfalse;
		else
			stealth_blink_on = qtrue;

		if (cg_entities[cg.clientNum].currentState.eFlags & EF_CLOAKED && !( cg_entities[cg.clientNum].currentState.eFlags & EF_PRONE ) && !(cg_entities[cg.clientNum].currentState.eFlags & EF_CROUCHING))
			stealth_blink_time = cg.time + 1000;
		else
			stealth_blink_time = cg.time + 500;
	}

	if (!stealth_blink_on)
		return;

	w = h = 48.f;

	/*
	cg.hud.head[0] = 8;
	cg.hud.head[1] = 388;
	cg.hud.head[2] = 34.875;
	cg.hud.head[3] = 45;
	*/

	x = 64;
	y = 720;

	// FIXME precache
	trap_R_DrawStretchPic( x, y, w, h, 0, 0, 1, 1, cgs.media.stealthShader );
}
#endif //__ETE__

static void CG_DrawNoShootIcon( void ) {
	float x, y, w, h;
	float *color;

	if( cg.predictedPlayerState.eFlags & EF_PRONE && cg.snap->ps.weapon == WP_PANZERFAUST ) {
		trap_R_SetColor( colorRed );
	} else if ( cg.crosshairClientNoShoot 
				// xkan, 1/6/2003 - don't shoot friend or civilian
				|| cg.snap->ps.serverCursorHint == HINT_PLYR_NEUTRAL
				|| cg.snap->ps.serverCursorHint == HINT_PLYR_FRIEND) {
		color = CG_FadeColor( cg.crosshairClientTime, 1000 );

		if ( !color ) {
			trap_R_SetColor( NULL );
			return;
		} else {
			trap_R_SetColor( color );
		}
	} else {
		return;
	}

	w = h = 48.f;

	x = cg_crosshairX.integer + 1;
	y = cg_crosshairY.integer + 1;
	CG_AdjustFrom640( &x, &y, &w, &h );

	// FIXME precache
	trap_R_DrawStretchPic( x + 0.5 * (cg.refdef_current->width - w), y + 0.5 * (cg.refdef_current->height - h), w, h, 0, 0, 1, 1, cgs.media.friendShader );
}

/*
=================
CG_ScanForCrosshairEntity
=================

Returns the distance to the entity

*/
static float CG_ScanForCrosshairEntity( float * zChange, qboolean * hitClient ) {
	trace_t		trace;
//	gentity_t	*traceEnt;
	vec3_t		start, end;
	float		dist;
	centity_t*	cent;
#ifdef __VEHICLES__
	vec3_t		forward;
	vec3_t		angles;
#endif

	// We haven't hit a client yet
	*hitClient = qfalse;

#ifdef __VEHICLES__
	// correct way
	VectorCopy( cg.predictedPlayerState.origin, start );
	start[2] += cg.predictedPlayerState.viewheight;

	VectorCopy( cg.predictedPlayerState.viewangles, angles );

	if ( cg.predictedPlayerState.eFlags & EF_VEHICLE )
		angles[1] = cg.predictedPlayerState.stats[STAT_DEAD_YAW];

	AngleVectors (angles, forward, NULL, NULL);

	VectorMA( start, 8192, forward, end );	//----(SA)	changed from 8192
#else 
	VectorCopy( cg.refdef.vieworg, start );
	VectorMA( start, 8192, cg.refdef.viewaxis[0], end );	//----(SA)	changed from 8192
#endif 

	cg.crosshairClientNoShoot = qfalse;

/*#ifdef __VEHICLES__
	if (cg_entities[cg.snap->ps.clientNum].currentState.eFlags & EF_VEHICLE)
	{
		CG_DrawCrosshair(end);	 // Jedi Knight style
		CG_Trace( &trace, start, NULL, NULL, end, cg.snap->ps.clientNum, CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_ITEM );
	}
	else
#endif //__VEHICLES__*/
	{
		CG_Trace( &trace, start, NULL, NULL, end, cg.snap->ps.clientNum, CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_ITEM );
		CG_DrawCrosshair(trace.endpos);	 // Jedi Knight style
	}

	// How far from start to end of trace?
	dist = VectorDistance( start, trace.endpos );

	// How far up or down are we looking?
	*zChange = trace.endpos[2] - start[2];

#ifdef __NPC__
	if ( cg_entities[trace.entityNum].currentState.eType == ET_NPC && cg_entities[trace.entityNum].currentState.modelindex2 == CLASS_HUMANOID ) 
	{
	}
	else
#endif //__NPC__
	if ( trace.entityNum >= MAX_CLIENTS ) {
		if( cg_entities[trace.entityNum].currentState.eFlags & EF_TAGCONNECT ) {
			trace.entityNum = cg_entities[trace.entityNum].tagParent;
		}

		// is a tank with a healthbar
		// this might have some side-effects, but none right now as the script_mover is the only one that sets effect1Time
		if( ( cg_entities[trace.entityNum].currentState.eType == ET_MOVER && cg_entities[trace.entityNum].currentState.effect1Time ) ||
			cg_entities[trace.entityNum].currentState.eType == ET_CONSTRUCTIBLE_MARKER 
#ifdef __VEHICLES__
			|| cg_entities[trace.entityNum].currentState.eType == ET_VEHICLE
#endif //__VEHICLES__
			) {
			// update the fade timer
			cg.crosshairClientNum = trace.entityNum;
			cg.crosshairClientTime = cg.time;
			cg.identifyClientRequest = cg.crosshairClientNum;
		}


		// Default: We're not looking at a client
		cg.crosshairNotLookingAtClient = qtrue;
		
		return dist;
	}

//	traceEnt = &g_entities[trace.entityNum];

	// Reset the draw time for the SP crosshair
	cg.crosshairSPClientTime = cg.time;

	// Default: We're not looking at a client
	cg.crosshairNotLookingAtClient = qfalse;

	// We hit a client
	*hitClient = qtrue;

	// update the fade timer
	cg.crosshairClientNum = trace.entityNum;
	cg.crosshairClientTime = cg.time;
	if ( cg.crosshairClientNum != cg.snap->ps.identifyClient && cg.crosshairClientNum != ENTITYNUM_WORLD ) {
		cg.identifyClientRequest = cg.crosshairClientNum;
	}

	cent = &cg_entities[cg.crosshairClientNum];

	if( cent && cent->currentState.powerups & (1 << PW_OPS_DISGUISED) ) {
		if(cgs.clientinfo[cg.crosshairClientNum].team == cgs.clientinfo[cg.clientNum].team) {
			cg.crosshairClientNoShoot = qtrue;
		}
	}

	return dist;
}



#define CH_KNIFE_DIST		48	// from g_weapon.c
#define CH_LADDER_DIST		100
#define CH_WATER_DIST		100
#define CH_BREAKABLE_DIST	64
#define CH_DOOR_DIST		96

#define CH_DIST				100 //128		// use the largest value from above

/*
==============
CG_CheckForCursorHints
	concept in progress...
==============
*/
void CG_CheckForCursorHints( void ) {
	trace_t		trace;
	vec3_t		start, end;
	centity_t	*tracent;
	vec3_t		pforward, eforward;
	float		dist;

	if ( cg.renderingThirdPerson 
#ifdef __VEHICLES__
		&& !(cg.snap->ps.eFlags & EF_VEHICLE) 
#endif //__VEHICLES__
		) 
	{
		return;
	}

	if(cg.snap->ps.serverCursorHint) {	// server is dictating a cursor hint, use it.
		cg.cursorHintTime = cg.time;
		cg.cursorHintFade = 500;	// fade out time
		cg.cursorHintIcon = cg.snap->ps.serverCursorHint;
		cg.cursorHintValue = cg.snap->ps.serverCursorHintVal;
		return;
	}

	// From here on it's client-side cursor hints.  So if the server isn't sending that info (as an option)
	// then it falls into here and you can get basic cursorhint info if you want, but not the detailed info
	// the server sends.

	// the trace
	VectorCopy( cg.refdef_current->vieworg, start );
	VectorMA( start, CH_DIST, cg.refdef_current->viewaxis[0], end );

//	CG_Trace( &trace, start, vec3_origin, vec3_origin, end, cg.snap->ps.clientNum, MASK_ALL &~CONTENTS_MONSTERCLIP);
	CG_Trace( &trace, start, vec3_origin, vec3_origin, end, cg.snap->ps.clientNum, MASK_PLAYERSOLID);

	if(trace.fraction == 1)
		return;

	dist = trace.fraction * CH_DIST;

	tracent = &cg_entities[ trace.entityNum ];

	// Arnout: invisible entities don't show hints
	if( trace.entityNum >= MAX_CLIENTS &&
		( tracent->currentState.powerups == STATE_INVISIBLE ||
		tracent->currentState.powerups == STATE_UNDERCONSTRUCTION ) ) {
		return;
	}

	//
	// world
	//
	if(trace.entityNum == ENTITYNUM_WORLD) {
		if( (trace.surfaceFlags & SURF_LADDER) && !(cg.snap->ps.pm_flags & PMF_LADDER) ) {
			if(dist <= CH_LADDER_DIST) {
				cg.cursorHintIcon = HINT_LADDER;
				cg.cursorHintTime = cg.time;
				cg.cursorHintFade = 500;
				cg.cursorHintValue = 0;
			}
		}


	} else if(trace.entityNum < MAX_CLIENTS ) { // people

		// knife
		if(trace.entityNum < MAX_CLIENTS && (cg.snap->ps.weapon == WP_KNIFE ) ) {
			if(dist <= CH_KNIFE_DIST) {

				AngleVectors (cg.snap->ps.viewangles,	pforward, NULL, NULL);
				AngleVectors (tracent->lerpAngles,		eforward, NULL, NULL);

				if( DotProduct( eforward, pforward ) > 0.6f )	{	// from behind(-ish)
					cg.cursorHintIcon = HINT_KNIFE;
					cg.cursorHintTime = cg.time;
					cg.cursorHintFade = 100;
					cg.cursorHintValue = 0;
				}
			}
		}
	}
}



/*
=====================
CG_DrawCrosshairNames
=====================
*/
extern void CG_Crosshair_HealthBar(float x, float y, float w, float h, float *startColor, float *endColor, const float *bgColor, float frac, int flags);

static void CG_DrawCrosshairNames( void ) {
	float		*color;
	char		*name;
	float		w;
	const char	*s, *playerClass;
	int			playerHealth = 0;
	vec4_t		c;
	float		barFrac;
	qboolean	drawStuff = qfalse;
	const char *playerRank;
	qboolean	isTank = qfalse;
	int			maxHealth = 1;
	int			i;
	int			myTeam = TEAM_SPECTATOR;
	int			crosshairTeam = TEAM_SPECTATOR;

	// Distance to the entity under the crosshair
	float		dist;
	float		zChange;

	qboolean hitClient = qfalse;

	if ( cg_drawCrosshair.integer < 0 ) {
		return;
	}

	// scan the known entities to see if the crosshair is sighted on one
	dist = CG_ScanForCrosshairEntity(&zChange, &hitClient );

	if ( cg.renderingThirdPerson 
#ifdef __VEHICLES__
		&& !(cg.snap->ps.eFlags & EF_VEHICLE) 
#endif //__VEHICLES__
		) {
		return;
	}

	// draw the name of the player being looked at
	color = CG_FadeColor( cg.crosshairClientTime, 1000 );

	if ( !color ) {
		trap_R_SetColor( NULL );
		return;
	}

	//UQ1: Darken color of the health bar so it can be seen in white fog...
	color[0]*=0.5;
	color[1]*=0.5;
	color[2]*=0.5;

	// UQ1: Copy teams to a new variable to make this simpler with NPCs (and neater!)...
	myTeam = cgs.clientinfo[cg.snap->ps.clientNum].team;
#ifdef __NPC__
	if ( cg_entities[cg.crosshairClientNum].currentState.eType == ET_NPC)
		crosshairTeam = cg_entities[cg.crosshairClientNum].currentState.teamNum;
	else
#endif //__NPC__
	crosshairTeam = cgs.clientinfo[ cg.crosshairClientNum ].team;

	// NERVE - SMF
#ifdef __NPC__
	if ( cg_entities[cg.crosshairClientNum].currentState.eType == ET_NPC 
		&& cg_entities[cg.crosshairClientNum].currentState.modelindex2 == CLASS_HUMANOID ) 
	{
	}
	else
#endif //__NPC__
	if ( cg.crosshairClientNum > MAX_CLIENTS ) {
		if ( !cg_drawCrosshairNames.integer ) {
			return;
		}

		if( cgs.clientinfo[cg.snap->ps.clientNum].team != TEAM_SPECTATOR ) {
			if( cg_entities[cg.crosshairClientNum].currentState.eType == ET_MOVER && cg_entities[cg.crosshairClientNum].currentState.effect1Time ) {
				isTank = qtrue;

				playerHealth = cg_entities[cg.crosshairClientNum].currentState.dl_intensity;
				maxHealth = 255;

				s = Info_ValueForKey( CG_ConfigString( CS_SCRIPT_MOVER_NAMES ), va( "%i", cg.crosshairClientNum ) );
				if( !*s ) {
					return;
				}

				w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( 320 - w / 2, 170, s, color );
			} else if( cg_entities[cg.crosshairClientNum].currentState.eType == ET_CONSTRUCTIBLE_MARKER ) {
				s = Info_ValueForKey( CG_ConfigString( CS_CONSTRUCTION_NAMES ), va( "%i", cg.crosshairClientNum ) );
				if( *s ) {
					w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
					CG_DrawSmallStringColor( 320 - w / 2, 170, s, color );
				}
				return;
#ifdef __VEHICLES__
			} else if( cg_entities[cg.crosshairClientNum].currentState.eType == ET_VEHICLE ) {
				isTank = qtrue;

				playerHealth = cg_entities[cg.crosshairClientNum].currentState.dl_intensity;
				maxHealth = 100;

				//CG_Printf( "%i \n" , playerHealth );
//some code added by Masteries
				if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_CHURCHILL) {
				s = "Churchill Tank";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_PANZER) {
				s = "Jagdpanther";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_TIGERTANK) {
				s = "PanzerVI - Tiger";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_KV1) {
				s = "KV-1";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_FLAMETANK) {
				s = "Flame Tank";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_T34) {
				s = "T-34";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_BT7) {
				s = "BT-7";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_PANZERIV) {
				s = "Panzer IV";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_FLAK88) {
				s = "Flak 88 Cannon";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_PAK) {
				s = "Antitank Cannon";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_STUG3) {
				s = "Stug-3";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_AXIS_APC) {
				s = "ADD ME :) (Axis APC)";
				} else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_ALLIED_APC) {
				s = "M3 (Russian APC)";
				}else if ( cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_PANZER2) {
				s = "Panzer II";
				}
				else {
				s = "";
				}
				if( !*s ) {
					return;
				}

				w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( 320 - w / 2, 170, va("  ^3%s^5", s), color );
#endif //__VEHICLES__
			} 
		}

		if( !isTank ) {
			return;
		}
	} else if( crosshairTeam != myTeam ) {
		if( (cg_entities[cg.crosshairClientNum].currentState.powerups & (1 << PW_OPS_DISGUISED)) && myTeam != TEAM_SPECTATOR) {
			if( myTeam != TEAM_SPECTATOR &&
				cgs.clientinfo[cg.snap->ps.clientNum].skill[SK_SIGNALS] >= 4 && cgs.clientinfo[cg.snap->ps.clientNum].cls == PC_FIELDOPS ) {
				s = CG_TranslateString( "Disguised Enemy!" );
				w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( 320 - w / 2, 170, s, color );
				return;
			} else if( dist > 512 ||
				(cgs.coverts & COVERTF_TRUE_DISGUISE)) {

				if ( !cg_drawCrosshairNames.integer ) {
					return;
				}

				drawStuff = qtrue;

				// determine player class
				playerClass = BG_ClassLetterForNumber( (cg_entities[ cg.crosshairClientNum ].currentState.powerups >> PW_OPS_CLASS_1) & 6 );

				name = cgs.clientinfo[ cg.crosshairClientNum ].disguiseName;

				playerRank = cgs.clientinfo[ cg.crosshairClientNum ].team != TEAM_AXIS ? rankNames_Axis[cgs.clientinfo[cg.crosshairClientNum].disguiseRank] : rankNames_Allies[cgs.clientinfo[cg.crosshairClientNum].disguiseRank];

#ifdef __VEHICLES__  //add code by Masteries 5/12/2007
				if (  cg_entities[cg.crosshairClientNum].currentState.eFlags & EF_VEHICLE)
				{
	 				if(cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_PANZER) 
					{
 						s = va( "^3Jagdpanther^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					} 
					else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_CHURCHILL) 
					{
 						s = va( "^3Churchill^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
					else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_TIGERTANK) 
					{
 						s = va( "^3PanzerVI - Tiger^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
					else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_KV1) 
					{
 						s = va( "^3KV-1^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
					else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_FLAMETANK) 
					{
 						s = va( "^3Flame Tank^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
                    else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_T34) 
					{
 						s = va( "^3T-34^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
					else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_BT7) 
					{
 						s = va( "^3BT-7^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
					else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_PANZERIV) 
					{
 						s = va( "^3PanzerIV^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
					else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_STUG3) 
					{
 						s = va( "^3Stug-3^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
					else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_AXIS_APC) 
					{
 						s = va( "^3ADD ME :) (Axis APC)^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
					else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_ALLIED_APC) 
					{
						s = va( "^3M3 (Russian APC)^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
					else if (cg_entities[cg.crosshairClientNum].currentState.effect1Time == VEHICLE_TYPE_PANZER2) 
					{
						s = va( "^3Panzer II^5: [^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 					}
 				} else {
  					s = va( "^5[^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
 				}
#else //!__VEHICLES__
				s = va( "^5[^3%s^5]^5 %s %s", CG_TranslateString( playerClass ), playerRank, name );
#endif //__VEHICLES__
				w = CG_DrawStrlen( CG_TranslateString(s) ) * SMALLCHAR_WIDTH;

				// draw the name and class
				CG_DrawSmallStringColor( 320 - w / 2, 170, s, color );

				// set the health
				// rain - #480 - make sure it's the health for the right entity;
				// if it's not, use the clientinfo health (which is updated
				// by tinfo)
				if( cg.crosshairClientNum == cg.snap->ps.identifyClient ) {
					playerHealth = cg.snap->ps.identifyClientHealth;
				} else {
					playerHealth = cgs.clientinfo[ cg.crosshairClientNum ].health;
				}

				maxHealth = 100;
			} else {
				// rain - #480 - don't show the name after you look away, should this be
				// a disguised covert
				cg.crosshairClientTime = 0;
				return;
			}
		} else {
			return;
		}
	}

	if ( !cg_drawCrosshairNames.integer ) {
		return;
	}

	// Mad Doc - TDF
	// changed this from early-exiting if true, to only executing most stuff if false. We want to
	// show debug info regardless

	// we only want to see players on our team
	if ( !isTank && myTeam != TEAM_SPECTATOR && crosshairTeam == myTeam ) 
	{
		drawStuff = qtrue;

		// determine player class
#ifdef __NPC__
		if ( cg_entities[cg.crosshairClientNum].currentState.eType == ET_NPC && cg_entities[cg.crosshairClientNum].currentState.modelindex2 == CLASS_HUMANOID ) 
		{
			playerClass = BG_ClassLetterForNumber(PC_SOLDIER);
			cgs.clientinfo[ cg.crosshairClientNum ].health = cg_entities[cg.crosshairClientNum].currentState.dl_intensity;
		}
		else
#endif //__NPC__
			playerClass = BG_ClassLetterForNumber( cg_entities[ cg.crosshairClientNum ].currentState.teamNum );

#ifdef __NPC__
#ifndef __NPC_NAMES__
		if ( cg_entities[cg.crosshairClientNum].currentState.eType == ET_NPC)
		{
			if (cg_entities[cg.crosshairClientNum].currentState.teamNum == TEAM_AXIS)
				name = va("^8German soldier.");
			else if (cg_entities[cg.crosshairClientNum].currentState.teamNum == TEAM_ALLIES)
				name = va("^1Russian soldier.");
			else
				name = va("^5Civilian");
		}
		else
#endif //__NPC_NAMES__
#endif //__NPC__
		name = cgs.clientinfo[ cg.crosshairClientNum ].name;

#ifdef __NPC__
#ifndef __NPC_NAMES__
		if ( cg_entities[cg.crosshairClientNum].currentState.eType == ET_NPC)
		{// NPCs only show the name...
			s = va( "^7%s", name );
			w = CG_DrawStrlen( CG_TranslateString(s) ) * 13/*SMALLCHAR_WIDTH*/;
		}
		else
		{
			playerRank = cgs.clientinfo[cg.crosshairClientNum].team == TEAM_AXIS ? rankNames_Axis[cgs.clientinfo[cg.crosshairClientNum].rank] : rankNames_Allies[cgs.clientinfo[cg.crosshairClientNum].rank];
			s = va( "^5[^3%s^5]^5 %s ^7%s", CG_TranslateString( playerClass ), playerRank, name );
			w = CG_DrawStrlen( CG_TranslateString(s) ) * 13/*SMALLCHAR_WIDTH*/;
		}
#else //__NPC_NAMES__
		playerRank = cgs.clientinfo[cg.crosshairClientNum].team == TEAM_AXIS ? rankNames_Axis[cgs.clientinfo[cg.crosshairClientNum].rank] : rankNames_Allies[cgs.clientinfo[cg.crosshairClientNum].rank];
		s = va( "^5[^3%s^5]^5 %s ^7%s", CG_TranslateString( playerClass ), playerRank, name );
		w = CG_DrawStrlen( CG_TranslateString(s) ) * 13/*SMALLCHAR_WIDTH*/;
#endif //__NPC_NAMES__
#else //!__NPC__
		playerRank = cgs.clientinfo[cg.crosshairClientNum].team == TEAM_AXIS ? rankNames_Axis[cgs.clientinfo[cg.crosshairClientNum].rank] : rankNames_Allies[cgs.clientinfo[cg.crosshairClientNum].rank];
		s = va( "^5[^3%s^5]^5 %s ^7%s", CG_TranslateString( playerClass ), playerRank, name );
		w = CG_DrawStrlen( CG_TranslateString(s) ) * 13/*SMALLCHAR_WIDTH*/;
#endif //__NPC__

		// draw the name and class
		CG_DrawSmallStringColor( 320 - w / 2, 170, s, color );

		// set the health
		if( cg.crosshairClientNum == cg.snap->ps.identifyClient ) {
			playerHealth = cg.snap->ps.identifyClientHealth;
		} else {
			playerHealth = cgs.clientinfo[ cg.crosshairClientNum ].health;
		}

		maxHealth = 100;

#ifdef __NPC__
		if ( cg_entities[cg.crosshairClientNum].currentState.eType == ET_NPC && cg_entities[cg.crosshairClientNum].currentState.modelindex2 == CLASS_HUMANOID ) 
			maxHealth = 120;
		else
#endif //__NPC__
		for( i = 0; i < MAX_CLIENTS; i++ ) {
			if( !cgs.clientinfo[i].infoValid ) {
				continue;
			}

			if( cgs.clientinfo[i].team != cgs.clientinfo[cg.snap->ps.clientNum].team ) {
				continue;
			}

			if( cgs.clientinfo[i].cls != PC_MEDIC ) {
				continue;
			}

			maxHealth += 10;

			if( maxHealth >= 125 ) {
				maxHealth = 125;
				break;
			}
		}

		if( cgs.clientinfo[ cg.crosshairClientNum ].skill[SK_BATTLE_SENSE] >= 3 ) {
			maxHealth += 15;
		}

		if( cgs.clientinfo[ cg.crosshairClientNum ].cls == PC_MEDIC ) {
			maxHealth *= 1.12f;
		}
	}

	// draw the health bar
#ifdef __NPC__
	if ( (!g_realism.integer || isTank) 
		&& cg_entities[cg.crosshairClientNum].currentState.eType == ET_NPC 
		&& cg_entities[cg.crosshairClientNum].currentState.modelindex2 == CLASS_HUMANOID ) 
	{
		if ( myTeam != TEAM_SPECTATOR && crosshairTeam == myTeam )
		{
			vec4_t bgcolor;

			barFrac = (float)playerHealth / maxHealth;

			if ( barFrac > 1.0 )
				barFrac = 1.0;
			else if ( barFrac < 0 )
				barFrac = 0;

			c[0] = 1.0f;
			c[1] = c[2] = barFrac;
			c[3] = (0.25 + barFrac * 0.5) * color[3];

			Vector4Set( bgcolor, 1.f, 1.f, 1.f, .25f * color[3] );

			CG_Crosshair_HealthBar( 320 - 110/*w*/ / 2, 190, 110, 6/*10*/, popRed/*c*/, popBlue/*NULL*/, NULL/*bgcolor*/, barFrac, 16 | 0x0100 );
		}
	}
	else
#endif //__NPC__
	if (!g_realism.integer || isTank)
	{// UQ1: Realism mode has no health bars!
		vec4_t bgcolor;

		barFrac = (float)playerHealth / maxHealth;

		if ( barFrac > 1.0 )
			barFrac = 1.0;
		else if ( barFrac < 0 )
			barFrac = 0;

		c[0] = 1.0f;
		c[1] = c[2] = barFrac;
		c[3] = (0.25 + barFrac * 0.5) * color[3];

		Vector4Set( bgcolor, 1.f, 1.f, 1.f, .25f * color[3] );

		//CG_FilledBar( 320 - 110/*w*/ / 2, 190, 110, 10, c, NULL, bgcolor, barFrac, 16 );
		CG_Crosshair_HealthBar( 320 - 110/*w*/ / 2, 190, 110, 6/*10*/, popRed/*c*/, popBlue/*NULL*/, NULL/*bgcolor*/, barFrac, 16 | 0x0100 );
	}

	// -NERVE - SMF
	if( isTank )
		return;

	if (drawStuff)
		trap_R_SetColor( NULL );
}



//==============================================================================

/*
=================
CG_DrawSpectator
=================
*/
static void CG_DrawSpectator(void) {
	//CG_DrawBigString( 320 - 9 * 8, 440, CG_TranslateString( "SPECTATOR" ), 1.f );
	//CG_DrawBigString( 320 - 9 * 3, 440, CG_TranslateString( "SPECTATOR" ), 1.f );
	CG_Text_Paint_Centred_Ext(320, 25, 0.30f, 0.30f,
		colorWhite, CG_TranslateString("SPECTATOR"), 
		0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahomabd24);
}

/*
=================
CG_DrawVote
=================
*/
  static void CG_DrawVote(void) {
  	char	*s;
 	char str1[32], str2[32], str3[32];
  	float color[4] = { 1, 1, 0, 1 };
 	int x,y;
  	int		sec;
  
 	x = cg.hud.votefttext[0];
 	y = cg.hud.votefttext[1];
 
  	if( cgs.complaintEndTime > cg.time && !cg.demoPlayback && cg_complaintPopUp.integer > 0 && cgs.complaintClient >= 0 ) {
  		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
  		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );
  
  		s = va( CG_TranslateString( "File complaint against %s for team-killing?" ), cgs.clientinfo[cgs.complaintClient].name);
 		CG_DrawStringExt( x, y, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
  
  		s = va( CG_TranslateString( "Press '%s' for YES, or '%s' for No" ), str1, str2 );
 		CG_DrawStringExt( x, y+14, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
  		return;
  	}

	if( cgs.applicationEndTime > cg.time && cgs.applicationClient >= 0 ) {
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = va( CG_TranslateString( "^5Accept ^7%s^5's application to join your fireteam?" ), cgs.clientinfo[cgs.applicationClient].name);
		CG_DrawStringExt( x, y, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );

		s = va( CG_TranslateString( "^5Press '^3%s^5' for ^7YES^5, or '^3%s^5' for ^7No^5" ), str1, str2 );
		CG_DrawStringExt( x, y+14, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
		return;
	}

	if( cgs.propositionEndTime > cg.time && cgs.propositionClient >= 0) {
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = va( CG_TranslateString( "^5Accept ^7%s^5's proposition to invite ^7%s^5 to join your fireteam?" ), cgs.clientinfo[cgs.propositionClient2].name, cgs.clientinfo[cgs.propositionClient].name);
		CG_DrawStringExt( x, y, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );

		s = va( CG_TranslateString( "^5Press '^3%s^5' for ^7YES^5, or '^3%s^5' for ^7No^5" ), str1, str2 );
		CG_DrawStringExt( x, y+14, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
		return;
	}

	if ( cgs.invitationEndTime > cg.time && cgs.invitationClient >= 0 ) {
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = va( CG_TranslateString( "^5Accept ^7%s^5's invitation to join their fireteam?" ), cgs.clientinfo[cgs.invitationClient].name);
		CG_DrawStringExt( x, y, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );

		s = va( CG_TranslateString( "^5Press '^3%s^5' for ^7YES^5, or '^3%s^5' for ^7No^5" ), str1, str2 );
		CG_DrawStringExt( x, y+14, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
		return;
	}

	if ( cgs.autoFireteamEndTime > cg.time && cgs.autoFireteamNum == -1 ) {
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = "Make Fireteam private?";
		CG_DrawStringExt( x, y, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );

		s = va( CG_TranslateString( "^5Press '^3%s^5' for ^7YES^5, or '^3%s^5' for ^7No^5" ), str1, str2 );
		CG_DrawStringExt( x, y+14, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
		return;
	}

	if ( cgs.autoFireteamCreateEndTime > cg.time && cgs.autoFireteamCreateNum == -1 ) {
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = "Create a Fireteam?";
		CG_DrawStringExt( x, y, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );

		s = va( CG_TranslateString( "^5Press '^3%s^5' for ^7YES^5, or '^3%s^5' for ^7No^5" ), str1, str2 );
		CG_DrawStringExt( x, y+14, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
		return;
	}
	
	if ( cgs.autoFireteamJoinEndTime > cg.time && cgs.autoFireteamJoinNum == -1 ) {
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = "Join a Fireteam?";
		CG_DrawStringExt( x, y, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );

		s = va( CG_TranslateString( "^5Press '^3%s^5' for ^7YES^5, or '^3%s^5' for ^7No^5" ), str1, str2 );
		CG_DrawStringExt( x, y+14, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
		return;
	}
	

	if( cgs.voteTime ) {
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );
		Q_strncpyz( str3, BindingFromName( "hidevote" ), 32 );
		if(!Q_stricmp(str3, "(?" "?" "?)")) {
			Q_strncpyz(str3, "\\hidevote", 32);
		}

		// play a talk beep whenever it is modified
		if( cgs.voteModified ) {
			cgs.voteModified = qfalse;
		}

		// tjw: see if this vote has been hidden
		if(cgs.voteTime != cgs.hidevote)
			cgs.hidevote = 0;

		sec = ( VOTE_TIME - ( cg.time - cgs.voteTime ) ) / 1000;

		if( sec < 0 || cgs.hidevote ) {
			sec = 0;
		}

		// tjw: there's no point in drawing inactive polls.  this
		//      should prevent the "stuck poll" bug too.
		if(!sec)
			return;

		if( !Q_stricmpn( cgs.voteString, "kick", 4 ) ) {
			if( strlen( cgs.voteString ) > 5 ) {
				int nameindex;
				char buffer[ 128 ];
				Q_strncpyz( buffer, cgs.voteString + 5, sizeof( buffer ) );
				Q_CleanStr( buffer );

				for( nameindex = 0; nameindex < MAX_CLIENTS; nameindex++ ) {
					if( !cgs.clientinfo[ nameindex ].infoValid ) {
						continue;
					}

					if( !Q_stricmp( cgs.clientinfo[ nameindex ].cleanname, buffer ) ) {
						if( cgs.clientinfo[ nameindex ].team != TEAM_SPECTATOR && cgs.clientinfo[ nameindex ].team != cgs.clientinfo[ cg.clientNum ].team ) {
							return;
						}
					}
				}
			}
		}

		if ( !(cg.snap->ps.eFlags & EF_VOTED) ) {
			s = va( CG_TranslateString( "^4VOTE^5(^7%i^5): ^5%s" ), sec, cgs.voteString);
			CG_DrawStringExt( x, y, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );

			if( cgs.clientinfo[cg.clientNum].team != TEAM_AXIS && cgs.clientinfo[cg.clientNum].team != TEAM_ALLIES ) {
				s = CG_TranslateString( "^1Cannot vote as Spectator" );
			} else {
				//s = va( CG_TranslateString( "^7YES^5(^3%s^5):^7%i^5, ^7NO^5(^3%s^5):^7%i^5" ), str1, cgs.voteYes, str2, cgs.voteNo );
 				s = va( CG_TranslateString(
 					"^7YES^5(^3%s^5):^7%i^5, ^7NO^5(^3%s^5):^7%i^5 ^7HIDE^5(^3%s^5)" ),
 					str1, cgs.voteYes,
 					str2, cgs.voteNo,
 					str3);
			}
			CG_DrawStringExt( x, y+14, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 60 );
			return;
		} else {
			s = va( CG_TranslateString( "^3YOU VOTED ON^5: ^7%s^5" ), cgs.voteString);
			CG_DrawStringExt( x, y, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );

			s = va( CG_TranslateString( "^7Y^5:^3%i^5, ^7N^5:^3%i^5" ), cgs.voteYes, cgs.voteNo );
			CG_DrawStringExt( x, y+14, s, color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 20 );
			return;
		}
	}

	if( cgs.complaintEndTime > cg.time && !cg.demoPlayback && cg_complaintPopUp.integer > 0 && cgs.complaintClient < 0 ) {
		if( cgs.complaintClient == -1 ) {
			s = "^3Your complaint has been filed";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}
		if( cgs.complaintClient == -2 ) {
			s = "^3Complaint dismissed";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}
		if( cgs.complaintClient == -3 ) {
			s = "^3Server Host cannot be complained against";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}
		if( cgs.complaintClient == -4 ) {
			s = "^3You were team-killed by the Server Host";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}
		if( cgs.complaintClient == -5 ) {
			s = "^3You were team-killed by a bot";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}
	}

	if( cgs.applicationEndTime > cg.time && cgs.applicationClient < 0 ) {
		if( cgs.applicationClient == -1 ) {
			s = "^3Your application has been submitted";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		if( cgs.applicationClient == -2 ) {
			s = "^3Your application failed";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		if( cgs.applicationClient == -3 ) {
			s = "^3Your application has been approved";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		if( cgs.applicationClient == -4 ) {
			s = "^3Your application reply has been sent";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}
	}

	if( cgs.propositionEndTime > cg.time && cgs.propositionClient < 0) {
		if( cgs.propositionClient == -1 ) {
			s = "^3Your proposition has been submitted";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		if( cgs.propositionClient == -2 ) {
			s = "^3Your proposition was rejected";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		if( cgs.propositionClient == -3 ) {
			s = "^3Your proposition was accepted";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		if( cgs.propositionClient == -4 ) {
			s = "^3Your proposition reply has been sent";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}
	}

	if( cgs.invitationEndTime > cg.time && cgs.invitationClient < 0 ) {
		if( cgs.invitationClient == -1 ) {
			s = "^3Your invitation has been submitted";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		if( cgs.invitationClient == -2 ) {
			s = "^3Your invitation was rejected";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		if( cgs.invitationClient == -3 ) {
			s = "^3Your invitation was accepted";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		if( cgs.invitationClient == -4 ) {
			s = "^3Your invitation reply has been sent";
			CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		if( cgs.invitationClient < 0 ) {
			return;
		}
	}

	if( (cgs.autoFireteamEndTime > cg.time && cgs.autoFireteamNum == -2) || (cgs.autoFireteamCreateEndTime > cg.time && cgs.autoFireteamCreateNum == -2) || (cgs.autoFireteamJoinEndTime > cg.time && cgs.autoFireteamJoinNum == -2)) {
		s = "Response Sent";
		CG_DrawStringExt( x, y, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
		return;
	}
}

/*
=================
CG_DrawIntermission
=================
*/
static void CG_DrawIntermission( void )
{
	// End-of-level autoactions
	if(!cg.demoPlayback) {
		static int doScreenshot = 0, doDemostop = 0;

		if(!cg.latchAutoActions) {
			cg.latchAutoActions = qtrue;

			if(cg_autoAction.integer & AA_SCREENSHOT) {
				doScreenshot = cg.time + 1000;
			}

			if(cg_autoAction.integer & AA_STATSDUMP) {
				CG_dumpStats_f();
			}

			if((cg_autoAction.integer & AA_DEMORECORD) &&
			  ((cgs.gametype == GT_WOLF_STOPWATCH && cgs.currentRound == 0) ||
			    cgs.gametype != GT_WOLF_STOPWATCH))
			{
				doDemostop = cg.time + 5000;	// stats should show up within 5 seconds
			}
		}

		if(doScreenshot > 0 && doScreenshot < cg.time) {
			CG_autoScreenShot_f();
			doScreenshot = 0;
		}

		if(doDemostop > 0 && doDemostop < cg.time) {
			trap_SendConsoleCommand("stoprecord\n");
			doDemostop = 0;
		}
	}

	// Intermission view
	CG_Debriefing_Draw();

/*	cg.scoreFadeTime = cg.time;
	CG_DrawScoreboard();
*/
}

/*
=================
CG_ActivateLimboMenu

NERVE - SMF
=================
*/
static void CG_ActivateLimboMenu(void) {
/*	static qboolean latch = qfalse;
	qboolean test;

	// should we open the limbo menu (make allowances for MV clients)
	test = ((cg.snap->ps.pm_flags & PMF_LIMBO) ||
			( (cg.mvTotalClients < 1 && (
				(cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR) ||
				(cg.warmup))
			  )
			&& cg.snap->ps.pm_type != PM_INTERMISSION ) );


	// auto open/close limbo mode
	if(cg_popupLimboMenu.integer && !cg.demoPlayback) {
		if(test && !latch) {			
			CG_LimboMenu_f();
			latch = qtrue;
		} else if(!test && latch && cg.showGameView) {
			CG_EventHandling(CGAME_EVENT_NONE, qfalse);
			latch = qfalse;
		}
	}*/
}

/*
=================
CG_DrawSpectatorMessage
=================
*/
static void CG_DrawSpectatorMessage( void ) {
	const char *str, *str2;
	float x, y;
	static int lastconfigGet = 0;

	if ( !cg_descriptiveText.integer )
		return;

	if ( !( cg.snap->ps.pm_flags & PMF_LIMBO || cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) )
		return;

	if(cg.time - lastconfigGet > 1000) {
		Controls_GetConfig();

		lastconfigGet = cg.time;
	}

	x = (cg.snap->ps.pm_flags & PMF_LIMBO) ? 170 : 80;
	y = 408;

	y -= 2 * TINYCHAR_HEIGHT;

	str2 = BindingFromName( "openlimbomenu" );
	if ( !Q_stricmp( str2, "(openlimbomenu)" ) ) {
		str2 = "ESCAPE";
	}
	str = va( CG_TranslateString( "^5Press ^3%s^5 to open Limbo Menu" ), str2 );
	CG_DrawStringExt( 8, 184, str, colorWhite, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );

	str2 = BindingFromName( "+attack" );
	str = va( CG_TranslateString( "^5Press ^3%s^5 to follow next player" ), str2 );
	CG_DrawStringExt( 8, 202, str, colorWhite, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );

#ifdef MV_SUPPORT
	str2 = BindingFromName( "mvactivate" );
	str = va( CG_TranslateString( "^1- ^5Press ^3%s^5 to ^7%s^5 multiview mode" ), str2, ((cg.mvTotalClients > 0) ? "disable" : "activate") );
	CG_DrawStringExt( x, y, str, colorWhite, qtrue, qtrue, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
	y += TINYCHAR_HEIGHT;
#endif
}


float CG_CalculateReinfTime_Float( qboolean menu ) {
	team_t team; 
	int dwDeployTime; 

	if( menu ) {
		if(cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR) {
			team = cgs.ccSelectedTeam == 0 ? TEAM_AXIS : TEAM_ALLIES;
		} else {
			team = cgs.clientinfo[cg.clientNum].team;
		}
	} else {
		team = cgs.clientinfo[cg.snap->ps.clientNum].team;
	}

	dwDeployTime = (team == TEAM_AXIS) ? cg_redlimbotime.integer : cg_bluelimbotime.integer;
	return (1 + (dwDeployTime - ((cgs.aReinfOffset[team] + cg.time - cgs.levelStartTime) % dwDeployTime)) * 0.001f);
}

int CG_CalculateReinfTime( qboolean menu ) {
	return((int)CG_CalculateReinfTime_Float( menu ));
}


/*
=================
CG_DrawLimboMessage
=================
*/

#define INFOTEXT_STARTX	8

static void CG_DrawLimboMessage( void ) {
	float color[4] = { 1, 1, 1, 1 };
	const char *str; 
	playerState_t *ps;
	int y = 148;


	ps = &cg.snap->ps;

	if ( ps->stats[STAT_HEALTH] > 0 ) {
		return;
	}

	if(ps->eFlags & EF_PLAYDEAD) {
		return;
	}

	if( cg.snap->ps.pm_flags & PMF_LIMBO || cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR ) {
		return;
	}

	if( cg_descriptiveText.integer ) {
		str = CG_TranslateString( "^5You are wounded and waiting for a medic." );
		CG_DrawSmallStringColor( INFOTEXT_STARTX, y, str, color );
		y += 18;

		if( cgs.gametype == GT_WOLF_LMS ) {
			trap_R_SetColor( NULL );
			return;
		}

		str = CG_TranslateString( "^5Press ^3JUMP^5 to go into reinforcement queue." );
		CG_DrawSmallStringColor( INFOTEXT_STARTX, y, str, color );
		y += 18;
	} else if( cgs.gametype == GT_WOLF_LMS ) {
		trap_R_SetColor( NULL );
		return;
	}

	// JPW NERVE
	str = (ps->persistant[PERS_RESPAWNS_LEFT] == 0) ? CG_TranslateString("^7No more reinforcements this round.") : va(CG_TranslateString("^5Reinforcements deploy in ^3%d^5 seconds."), CG_CalculateReinfTime( qfalse ));

	CG_DrawSmallStringColor( INFOTEXT_STARTX, y, str, color );
	y += 18;
	// jpw

	trap_R_SetColor( NULL );
}
// -NERVE - SMF

/*
=================
CG_DrawFollow
=================
*/
static qboolean CG_DrawFollow(void)
{
	char deploytime[128];

	// MV following info for mainview
	if(CG_ViewingDraw()) {
		return(qtrue);
	}

	if(!(cg.snap->ps.pm_flags & PMF_FOLLOW)) {
		return(qfalse);
	}

	// if in limbo, show different follow message 
	if(cg.snap->ps.pm_flags & PMF_LIMBO) {
		if(cgs.gametype != GT_WOLF_LMS) {
			if( cg.snap->ps.persistant[PERS_RESPAWNS_LEFT] == 0 ) {
				if( cg.snap->ps.persistant[PERS_RESPAWNS_PENALTY] >= 0 ) {
					int deployTime = (cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_AXIS) ? cg_redlimbotime.integer : cg_bluelimbotime.integer;

					deployTime *= 0.001f;

					sprintf(deploytime, CG_TranslateString("^3Bonus Life^5! Deploying in ^7%d^5 seconds"), CG_CalculateReinfTime(qfalse) + cg.snap->ps.persistant[PERS_RESPAWNS_PENALTY] * deployTime );
				} else {
					sprintf(deploytime, CG_TranslateString("^3No more deployments this round"));
				}
			} else {
				sprintf(deploytime, CG_TranslateString("^5Deploying in ^3%d^5 seconds"), CG_CalculateReinfTime(qfalse));
			}

			CG_DrawStringExt(INFOTEXT_STARTX, 148, deploytime, colorWhite, qtrue, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 80);
		}

		// Don't display if you're following yourself
		if(cg.snap->ps.clientNum != cg.clientNum) {
			sprintf(deploytime, "^5(Following ^5[^3%s^5] ^3%s ^7%s^5)",
					BG_ClassnameForNumber(cgs.clientinfo[cg.snap->ps.clientNum].cls),
					cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_ALLIES ? rankNames_Allies[cgs.clientinfo[cg.snap->ps.clientNum].rank] : rankNames_Axis[cgs.clientinfo[cg.snap->ps.clientNum].rank],
					cgs.clientinfo[cg.snap->ps.clientNum].name);

			CG_DrawStringExtSmall( INFOTEXT_STARTX, 166, deploytime, colorWhite, qtrue, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 80);
		}
	} else {
		sprintf(deploytime, "^5Following ^5[^3%s^5] ^3%s ^7%s^5",
				BG_ClassnameForNumber(cgs.clientinfo[cg.snap->ps.clientNum].cls),
				cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_ALLIES ? rankNames_Allies[cgs.clientinfo[cg.snap->ps.clientNum].rank] : rankNames_Axis[cgs.clientinfo[cg.snap->ps.clientNum].rank],
				cgs.clientinfo[cg.snap->ps.clientNum].name);

		CG_DrawStringExtSmall( INFOTEXT_STARTX, 166, deploytime, colorWhite, qtrue, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 80);
//		CG_DrawStringExtSmall( INFOTEXT_STARTX, 166, "^5Following", colorWhite, qtrue, qtrue, BIGCHAR_WIDTH/2, BIGCHAR_HEIGHT, 0 );

//		CG_DrawStringExtSmall( 84, 166, va("^5[^3%s^5] ^7%s ^3%s",
//					BG_ClassnameForNumber(cgs.clientinfo[cg.snap->ps.clientNum].cls),
//					cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_ALLIES ? rankNames_Allies[cgs.clientinfo[cg.snap->ps.clientNum].rank] : rankNames_Axis[cgs.clientinfo[cg.snap->ps.clientNum].rank],
//					cgs.clientinfo[cg.snap->ps.clientNum].name), colorWhite, qtrue, qtrue, BIGCHAR_WIDTH/2, BIGCHAR_HEIGHT, 0);
	}

	return(qtrue);
}


/*
=================
CG_DrawWarmup
=================
*/
static void CG_DrawWarmup( void ) {
	//int			w;
	int			sec;
	int			cw;
	const char	*s, *s1, *s2;

	sec = cg.warmup;
	if(!sec) {
		if((cgs.gamestate == GS_WARMUP && !cg.warmup) || cgs.gamestate == GS_WAITING_FOR_PLAYERS) {
			cw = 10;

			s1 = va( CG_TranslateString( "^7WARMUP^5: Waiting on ^3%i^5 %s" ), cgs.minclients, cgs.minclients == 1 ? "player" : "players" );
			//w = CG_DrawStrlen( s1 );
			//CG_DrawStringExt(320 - w * 12/2, 188, s1, colorWhite, qfalse, qtrue, 12, 18, 0);
			CG_Text_Paint_Centred_Ext(320, 188, 0.25f, 0.25f,
				colorWhite, s1, 
				0, 0, 0, &cgs.media.tahomabd24);

			if(!cg.demoPlayback && cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR &&
			  (!(cg.snap->ps.pm_flags & PMF_FOLLOW) || (cg.snap->ps.pm_flags & PMF_LIMBO))) {
				char str1[32];
				Q_strncpyz( str1, BindingFromName( "ready" ), 32 );
				if( !Q_stricmp( str1, "(?" "?" "?)" ) ) {
					s2 = CG_TranslateString( "Type ^3\\ready^* in the console to start" );
				} else {
					s2 = va( "^5Press ^3%s^*^5 to start", str1 );
					s2 = CG_TranslateString( s2 );
				}				
				//w = CG_DrawStrlen( s2 );
				//CG_DrawStringExt(320 - w * cw/2, 208, s2, colorWhite, qfalse, qtrue, cw, (int)(cw * 1.5), 0);
				CG_Text_Paint_Centred_Ext(320, 208, 0.25f, 0.25f,
					colorWhite, s2, 
					0, 0, 0, &cgs.media.tahomabd24);
			}
	
/*	if ( !sec ) {
		if ( cgs.gamestate == GS_WAITING_FOR_PLAYERS ) {
			cw = 10;

			s = CG_TranslateString( "Game Stopped - Waiting for more players" );

			w = CG_DrawStrlen( s );
			CG_DrawStringExt( 320 - w * 6, 120, s, colorWhite, qfalse, qtrue, 12, 18, 0 );

			if( cg_gameType.integer != GT_WOLF_LMS ) {
			s1 = va( CG_TranslateString( "Waiting for %i players" ), cgs.minclients );
			s2 = CG_TranslateString( "or call a vote to start the match" );

			w = CG_DrawStrlen( s1 );
			CG_DrawStringExt( 320 - w * cw/2, 160, s1, colorWhite, 
				qfalse, qtrue, cw, (int)(cw * 1.5), 0 );

			w = CG_DrawStrlen( s2 );
			CG_DrawStringExt( 320 - w * cw/2, 180, s2, colorWhite, 
				qfalse, qtrue, cw, (int)(cw * 1.5), 0 );
			}
*/
			return;
		}

		return;
	}

	sec = ( sec - cg.time ) / 1000;
	if ( sec < 0 ) {
		sec = 0;
	}

	s = va( "%s %i", CG_TranslateString( "^5(^3WARMUP^5) Match begins in:^7" ), sec + 1 );

	//w = CG_DrawStrlen( s );
	//CG_DrawStringExt( 320 - w * 6, 120, s, colorYellow, qfalse, qtrue, 12, 18, 0 );
	CG_Text_Paint_Centred_Ext(320, 120, 0.25f, 0.25f,
		colorWhite, s,
		0, 0, 0, &cgs.media.tahomabd24);

	// NERVE - SMF - stopwatch stuff
	s1 = "";
	s2 = "";

	if ( cgs.gametype == GT_WOLF_STOPWATCH ) {
		const char	*cs;
		int		defender;

		s = va( "%s %i", CG_TranslateString( "^5Stopwatch Round^7" ), cgs.currentRound + 1 );

		cs = CG_ConfigString( CS_MULTI_INFO );
		defender = atoi( Info_ValueForKey( cs, "defender" ) );

		if ( !defender ) {
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_AXIS ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Axis team";
					s2 = "Keep the Allies from beating the clock!";
				}
				else {
					s1 = "You are on the Axis team";
				}
			}
			else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_ALLIES ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Allied team";
					s2 = "Try to beat the clock!";
				}
				else {
					s1 = "You are on the Allied team";
				}
			}
		}
		else {
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_AXIS ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Axis team";
					s2 = "Try to beat the clock!";
				}
				else {
					s1 = "You are on the Axis team";
				}
			}
			else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_ALLIES ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Allied team";
					s2 = "Keep the Axis from beating the clock!";
				}
				else {
					s1 = "You are on the Allied team";
				}
			}
		}

		if ( strlen( s1 ) )
			s1 = CG_TranslateString( s1 );

		if ( strlen( s2 ) )
			s2 = CG_TranslateString( s2 );

		cw = 10;

		//w = CG_DrawStrlen( s );
		//CG_DrawStringExt( 320 - w * cw/2, 140, s, colorWhite, 
		//	qfalse, qtrue, cw, (int)(cw * 1.5), 0 );
		CG_Text_Paint_Centred_Ext(320, 140, 0.25f, 0.25f,
			colorWhite, s,
			0, 0, 0, &cgs.media.tahomabd24);

		//w = CG_DrawStrlen( s1 );
		//CG_DrawStringExt( 320 - w * cw/2, 160, s1, colorWhite, 
		//	qfalse, qtrue, cw, (int)(cw * 1.5), 0 );
		CG_Text_Paint_Centred_Ext(320, 160, 0.25f, 0.25f,
			colorWhite, s1,
			0, 0, 0, &cgs.media.tahomabd24);

		//w = CG_DrawStrlen( s2 );
		//CG_DrawStringExt( 320 - w * cw/2, 180, s2, colorWhite, 
		//	qfalse, qtrue, cw, (int)(cw * 1.5), 0 );
		CG_Text_Paint_Centred_Ext(320, 160, 0.25f, 0.25f,
			colorWhite, s2,
			0, 0, 0, &cgs.media.tahomabd24);
	}
}

//==================================================================================

/*
=================
CG_DrawFlashFade
=================
*/
static void CG_DrawFlashFade( void ) {
	static int lastTime;
	int elapsed, time;
	vec4_t col;
	qboolean fBlackout = (!CG_IsSinglePlayer() && int_ui_blackout.integer > 0);

	if (( cg.snap->ps.weapon != WP_BINOCULARS 
		/*&& cg.snap->ps.weapon != WP_K43 
		&& cg.snap->ps.weapon != WP_FG42
		&& cg.snap->ps.weapon != WP_GARAND
		&& cg.snap->ps.weapon != WP_K43_SCOPE 
		&& cg.snap->ps.weapon != WP_FG42SCOPE 
		&& cg.snap->ps.weapon != WP_GARAND_SCOPE*/ ) || BG_PlayerMounted( cg.snap->ps.eFlags ) )
		return;

	if (cgs.fadeStartTime + cgs.fadeDuration < cg.time) {
		cgs.fadeAlphaCurrent = cgs.fadeAlpha;
	} else if (cgs.fadeAlphaCurrent != cgs.fadeAlpha) {
		elapsed = (time = trap_Milliseconds()) - lastTime;	// we need to use trap_Milliseconds() here since the cg.time gets modified upon reloading
		lastTime = time;
		if (elapsed < 500 && elapsed > 0) {
			if (cgs.fadeAlphaCurrent > cgs.fadeAlpha) {
				cgs.fadeAlphaCurrent -= ((float)elapsed/(float)cgs.fadeDuration);
				if (cgs.fadeAlphaCurrent < cgs.fadeAlpha)
					cgs.fadeAlphaCurrent = cgs.fadeAlpha;
			} else {
				cgs.fadeAlphaCurrent += ((float)elapsed/(float)cgs.fadeDuration);
				if (cgs.fadeAlphaCurrent > cgs.fadeAlpha)
					cgs.fadeAlphaCurrent = cgs.fadeAlpha;
			}
		}
	}

	// OSP - ugh, have to inform the ui that we need to remain blacked out (or not)
	if(int_ui_blackout.integer == 0) {
		if(cg.mvTotalClients < 1 && cg.snap->ps.powerups[PW_BLACKOUT] > 0) {
			trap_Cvar_Set("ui_blackout", va("%d", cg.snap->ps.powerups[PW_BLACKOUT]));
		}
	} else if(cg.snap->ps.powerups[PW_BLACKOUT] == 0 || cg.mvTotalClients > 0) {
		trap_Cvar_Set("ui_blackout", "0");
	}

	// now draw the fade
	if(cgs.fadeAlphaCurrent > 0.0 || fBlackout) {
		VectorClear( col );
		col[3] = (fBlackout) ? 1.0f : cgs.fadeAlphaCurrent;
//		CG_FillRect( -10, -10, 650, 490, col );
		CG_FillRect( 0, 0, 640, 480, col );	// why do a bunch of these extend outside 640x480?

		//bani - #127 - bail out if we're a speclocked spectator with cg_draw2d = 0
		if( cgs.clientinfo[ cg.clientNum ].team == TEAM_SPECTATOR && !cg_draw2D.integer ) {
			return;
		}

		// OSP - Show who is speclocked
		if(fBlackout) {
			int i, nOffset = 90;
			char *str, *format = "The %s team is speclocked!";
			char *teams[TEAM_NUM_TEAMS] = { "??", "AXIS", "ALLIES", "???" };
			float color[4] = { 1, 1, 0, 1 };

			for(i=TEAM_AXIS; i<=TEAM_ALLIES; i++) {
				if(cg.snap->ps.powerups[PW_BLACKOUT] & i) {
					str = va(format, teams[i]);
					CG_DrawStringExt(INFOTEXT_STARTX, nOffset, str, color, qtrue, qfalse, 10, 10, 0);
					nOffset += 12;
				}
			}
		}
	}
}



/*
==============
CG_DrawFlashZoomTransition
	hide the snap transition from regular view to/from zoomed

  FIXME: TODO: use cg_fade?
==============
*/
static void CG_DrawFlashZoomTransition(void) {
	vec4_t	color;
	float	frac;
	int		fadeTime;

	if (!cg.snap)
		return;

#ifdef __EF__//__WEAPON_AIM_NEW__
	if ( cg.snap->ps.eFlags & EF_AIMING ) // dont use this for ironsights , aiming
		return;

	if (( cg.snap->ps.weapon != WP_BINOCULARS 
		/*&& cg.snap->ps.weapon != WP_K43 
		&& cg.snap->ps.weapon != WP_FG42
		&& cg.snap->ps.weapon != WP_GARAND
		&& cg.snap->ps.weapon != WP_K43_SCOPE 
		&& cg.snap->ps.weapon != WP_FG42SCOPE 
		&& cg.snap->ps.weapon != WP_GARAND_SCOPE*/ ) || BG_PlayerMounted( cg.snap->ps.eFlags ) )
		return;

#endif //__EF__//__WEAPON_AIM_NEW__

	if( BG_PlayerMounted( cg.snap->ps.eFlags ) ) {
		// don't draw when on mg_42
		// keep the timer fresh so when you remove yourself from the mg42, it'll fade
		cg.zoomTime = cg.time;
		return;
	}

	if( cg.renderingThirdPerson ) {
		return;
	}

	fadeTime = 400;

	frac = cg.time - cg.zoomTime;

	if(frac < fadeTime) {
		frac = frac/(float)fadeTime;
		Vector4Set( color, 0, 0, 0, 1.0f - frac );
		CG_FillRect( -10, -10, 650, 490, color );
	}
}



/*
=================
CG_DrawFlashDamage
=================
*/
static void CG_DrawFlashDamage( void ) {
	vec4_t		col;
	float		redFlash;

	if (!cg.snap)
		return;

#ifdef __VEHICLES__
	if (cg.snap->ps.eFlags & EF_VEHICLE)
		return;
#endif //__VEHICLES__

	if (cg.v_dmg_time > cg.time) {
		redFlash = fabs(cg.v_dmg_pitch * ((cg.v_dmg_time - cg.time) / DAMAGE_TIME));

		// blend the entire screen red
		if (redFlash > 5)
			redFlash = 5;

		VectorSet( col, 0.2, 0, 0 );
		col[3] =  0.7 * (redFlash/5.0) * ((cg_bloodFlash.value > 1.0) ? 1.0 :
												(cg_bloodFlash.value < 0.0) ? 0.0 :
																			  cg_bloodFlash.value);

		CG_FillRect( -10, -10, 650, 490, col );
	}
}


/*
=================
CG_DrawFlashFire
=================
*/
static void CG_DrawFlashFire( void ) {
	vec4_t		col={1,1,1,1};
	float		alpha, max, f;

	if (!cg.snap)
		return;

	if ( cg.renderingThirdPerson ) {
		return;
	}

	if (!cg.snap->ps.onFireStart) {
		cg.v_noFireTime = cg.time;
		return;
	}

#ifdef __VEHICLES__
	if (cg.snap->ps.eFlags & EF_VEHICLE)
		return;
#endif //__VEHICLES__

	alpha = (float)((FIRE_FLASH_TIME-1000) - (cg.time - cg.snap->ps.onFireStart))/(FIRE_FLASH_TIME-1000);
	if (alpha > 0) {
		if (alpha >= 1.0) {
			alpha = 1.0;
		}

		// fade in?
		f = (float)(cg.time - cg.v_noFireTime)/FIRE_FLASH_FADEIN_TIME;
		if (f >= 0.0 && f < 1.0)
			alpha = f;

		max = 0.5 + 0.5*sin((float)((cg.time/10)%1000)/1000.0);
		if (alpha > max)
			alpha = max;
		col[0] = alpha;
		col[1] = alpha;
		col[2] = alpha;
		col[3] = alpha;
		trap_R_SetColor( col );
		CG_DrawPic( -10, -10, 650, 490, cgs.media.viewFlashFire[(cg.time/50)%16] );
		trap_R_SetColor( NULL );

		trap_S_AddLoopingSound( cg.snap->ps.origin, vec3_origin, cgs.media.flameSound, (int)(255.0*alpha), 0 );
		trap_S_AddLoopingSound( cg.snap->ps.origin, vec3_origin, cgs.media.flameCrackSound, (int)(255.0*alpha), 0 );
	} else {
		cg.v_noFireTime = cg.time;
	}
}



/*
==============
CG_DrawFlashBlendBehindHUD
	screen flash stuff drawn first (on top of world, behind HUD)
==============
*/
static void CG_DrawFlashBlendBehindHUD(void) {
	CG_DrawFlashZoomTransition();
	CG_DrawFlashFade();
}


/*
=================
CG_DrawFlashBlend
	screen flash stuff drawn last (on top of everything)
=================
*/
static void CG_DrawFlashBlend( void ) {
	// Gordon: no flash blends if in limbo or spectator, and in the limbo menu
	if( (cg.snap->ps.pm_flags & PMF_LIMBO || cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR) && cg.showGameView ) {
		return;
	}

	CG_DrawFlashFire();
	CG_DrawFlashDamage();
}

// NERVE - SMF
/*
=================
CG_DrawObjectiveInfo
=================
*/
#define OID_LEFT	10
#define OID_TOP		360
#define OID_FONTSCALE	0.2f
#define OID_FONT	cgs.media.tahomabd24

void CG_ObjectivePrint( const char *str, int charWidth ) {
	char	*s;
	int		i, len;						// NERVE - SMF
	qboolean neednewline = qfalse;		// NERVE - SMF

	if( cg.centerPrintTime ) {
		return;
	}

	s = CG_TranslateString( str );

	Q_strncpyz( cg.oidPrint, s, sizeof(cg.oidPrint) );

	// NERVE - SMF - turn spaces into newlines, if we've run over the linewidth
	len = strlen( cg.oidPrint );
	for ( i = 0; i < len; i++ ) {

		// NOTE: subtract a few chars here so long words still get displayed properly
		if ( i % ( CP_LINEWIDTH - 20 ) == 0 && i > 0 )
			neednewline = qtrue;
		if ( cg.oidPrint[i] == ' ' && neednewline ) {
			cg.oidPrint[i] = '\n';
			neednewline = qfalse;
		}
	}
	// -NERVE - SMF

	cg.oidPrintTime = cg.time;
	cg.oidPrintY = OID_TOP;

	// count the number of lines for oiding
	cg.oidPrintLines = 1;
	s = cg.oidPrint;
	while( *s ) {
		if (*s == '\n')
			cg.oidPrintLines++;
		s++;
	}
}

static void CG_DrawObjectiveInfo( void ) {
	char *start;
	int l;
	int y, w, h;
	int maxWidth = 0;
	float *color;
	vec4_t backColor;
	char lastColor = COLOR_WHITE;

	backColor[0] = 0.2f;
	backColor[1] = 0.2f;
	backColor[2] = 0.2f;
	backColor[2] = 1.f;

	if ( !cg.oidPrintTime ) {
		return;
	}

	color = CG_FadeColor( cg.oidPrintTime, 250 );
	if ( !color ) {
		cg.oidPrintTime = 0;
		return;
	}

	trap_R_SetColor( color );
	start = cg.oidPrint;
	y = cg.oidPrintY;

	// first just find the bounding rect
	while ( 1 ) {
		char linebuffer[1024];
		for ( l = 0; l < CP_LINEWIDTH; l++ ) {
			if ( !start[l] || start[l] == '\n' ) {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

		w = CG_Text_Width_Ext( linebuffer, OID_FONTSCALE, 0, &OID_FONT);
		if ( w > maxWidth ) {
			maxWidth = w;
		}

		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	h = (cg.oidPrintLines * 12) + 5;

	VectorCopy( color, backColor );
	backColor[3] = 0.5 * color[3];
	trap_R_SetColor( backColor );

	CG_DrawPic( 320 - (maxWidth/2) - 5, y, maxWidth + 10, h, cgs.media.teamStatusBar );

	VectorSet( backColor, 0, 0, 0 );
	CG_DrawRect( 320 - (maxWidth/2) - 5, y, maxWidth + 10, h, 1, backColor );

	trap_R_SetColor( color );

	// do the actual drawing
	start = cg.oidPrint;

	while ( 1 ) {
		char linebuffer[1024];
		char nextColor = lastColor;

		for ( l = 0; l < CP_LINEWIDTH; l++ ) {
			if ( !start[l] || start[l] == '\n' ) {
				break;
			}
			linebuffer[l] = start[l];
			if ( Q_IsColorString( &start[l]) ) {
				lastColor = start[l+1];
			}
		}
		linebuffer[l] = 0;

		y += 12;
		CG_Text_Paint_Centred_Ext( 320, y,
			OID_FONTSCALE, OID_FONTSCALE, color,
			va("%c%c%s", Q_COLOR_ESCAPE, nextColor, linebuffer),
			0, 0, 0, &OID_FONT );

		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	trap_R_SetColor( NULL );
}

//==================================================================================

void CG_DrawTimedMenus() {
	if (cg.voiceTime) {
		int t = cg.time - cg.voiceTime;
		if ( t > 2500 ) {
			Menus_CloseByName("voiceMenu");
			trap_Cvar_Set("cl_conXOffset", "0");
			cg.voiceTime = 0;
		}
	}
}

/*
=================
CG_Fade
=================
*/
void CG_Fade( int r, int g, int b, int a, int time, int duration ) {

	// incorporate this into the current fade scheme

	cgs.fadeAlpha = (float)a / 255.0f;
	cgs.fadeStartTime = time;
	cgs.fadeDuration = duration;

	if (cgs.fadeStartTime + cgs.fadeDuration <= cg.time) {
		cgs.fadeAlphaCurrent = cgs.fadeAlpha;
	}
	return;


	if ( time <= 0 ) {	// do instantly
		cg.fadeRate = 1;
		cg.fadeTime = cg.time - 1;	// set cg.fadeTime behind cg.time so it will start out 'done'
	} else {
		cg.fadeRate = 1.0f / time;
		cg.fadeTime = cg.time + time;
	}

	cg.fadeColor2[ 0 ] = ( float )r / 255.0f;
	cg.fadeColor2[ 1 ] = ( float )g / 255.0f;
	cg.fadeColor2[ 2 ] = ( float )b / 255.0f;
	cg.fadeColor2[ 3 ] = ( float )a / 255.0f;
}

/*
=================
CG_ScreenFade
=================
*/
static void CG_ScreenFade( void ) {
	int		msec;
	int		i;
	float	t, invt;
	vec4_t	color;

	if ( !cg.fadeRate ) {
		return;
	}

	msec = cg.fadeTime - cg.time;
	if ( msec <= 0 ) {
		cg.fadeColor1[ 0 ] = cg.fadeColor2[ 0 ];
		cg.fadeColor1[ 1 ] = cg.fadeColor2[ 1 ];
		cg.fadeColor1[ 2 ] = cg.fadeColor2[ 2 ];
		cg.fadeColor1[ 3 ] = cg.fadeColor2[ 3 ];

		if ( !cg.fadeColor1[ 3 ] ) {
			cg.fadeRate = 0;
			return;
		}

		CG_FillRect( 0, 0, 640, 480, cg.fadeColor1 );

	} else {
		t = ( float )msec * cg.fadeRate;
		invt = 1.0f - t;

		for( i = 0; i < 4; i++ ) {
			color[ i ] = cg.fadeColor1[ i ] * t + cg.fadeColor2[ i ] * invt;
		}

		if ( color[ 3 ] ) {
			CG_FillRect( 0, 0, 640, 480, color );
		}
	}
}

#if 0 // rain - unused
// JPW NERVE
void CG_Draw2D2(void) {
	qhandle_t weapon;

	trap_R_SetColor( NULL );

	CG_DrawPic( 0,480, 640, -70, cgs.media.hud1Shader );

	if(!BG_PlayerMounted(cg.snap->ps.eFlags) ) {
		switch (cg.snap->ps.weapon) {
		case WP_COLT:
		case WP_LUGER:
			weapon = cgs.media.hud2Shader;
			break;
		case WP_KNIFE:
			weapon = cgs.media.hud5Shader;
			break;
		default:
			weapon = cgs.media.hud3Shader;
		}
		CG_DrawPic( 220,410, 200,-200,weapon);
	}
}
#endif

/*
=================
CG_DrawCompassIcon

NERVE - SMF
=================
*/
void CG_DrawCompassIcon( float x, float y, float w, float h, vec3_t origin, vec3_t dest, qhandle_t shader ) {
	float angle, pi2 = M_PI * 2;
	vec3_t v1, angles;
	float len;

	VectorCopy( dest, v1 );
	VectorSubtract( origin, v1, v1 );
	len = VectorLength( v1 );
	VectorNormalize( v1 );
	vectoangles( v1, angles );

	if ( v1[0] == 0 && v1[1] == 0 && v1[2] == 0 )
		return;

//	if( cg_drawCompass.integer == 2 )
//		angles[YAW] = AngleSubtract( 90, angles[YAW] );
//	else
		angles[YAW] = AngleSubtract( cg.predictedPlayerState.viewangles[YAW], angles[YAW] );

	angle = ( ( angles[YAW] + 180.f ) / 360.f - ( 0.50 / 2.f ) ) * pi2;


//	if (!CG_IsSinglePlayer()) {
		w /= 2;
		h /= 2;

		x += w;
		y += h;


//		if (CG_IsSinglePlayer())
/*		if (0)
		{
			w = 80; // hardcoded, because it has to fit the art
		}
		else*/
		{
			w = sqrt( ( w * w ) + ( h * h ) ) / 3.f * 2.f * 0.9f;
		}

		x = x + ( cos( angle ) * w );
		y = y + ( sin( angle ) * w );

		len = 1 - min( 1.f, len / 2000.f );


		CG_DrawPic( x - (14 * len + 4)/2, y - (14 * len + 4)/2, 14 * len + 8, 14 * len + 8, shader );
#ifdef SQUARE_COMPASS
	} else {
		int iconWidth, iconHeight;
		// START Mad Doc - TDF
		// talk about fitting a square peg into a round hole...
		// we're now putting the compass icons around the square automap instead of the round compass

		while (angle < 0)
			angle += pi2;

		while (angle >= pi2)
			angle -= pi2;


		x = x + w/2;
		y = y + h/2;
		w /= 2;// = sqrt( ( w * w ) + ( h * h ) ) / 3.f * 2.f * 0.9f;

		if ( (angle >= 0) && (angle < M_PI/4.0))
		{
			x += w;
			y += w * tan(angle);

		}
		else if ( (angle >= M_PI/4.0) && (angle < 3.0 * M_PI / 4.0) )
		{
			x += w / tan(angle);
			y += w;
		}
		else if ( (angle >= 3.0 * M_PI / 4.0) && (angle < 5.0 * M_PI / 4.0) )
		{
			x -= w;
			y -= w * tan(angle);
		}
		else if ( (angle >= 5.0 * M_PI / 4.0) && (angle < 7.0 * M_PI / 4.0) )
		{
			x -= w / tan(angle);
			y -= w;
		}
		else
		{
			x += w;
			y += w * tan(angle);

		}

		len = 1 - min( 1.f, len / 2000.f );
		iconWidth = 14 * len + 4; // where did this calc. come from?
		iconHeight = 14 * len + 4; 

		// adjust so that icon is always outside of the map
		if ( (angle > 5.0*M_PI/4.0) && (angle < 2*M_PI) )
		{

			y -= iconHeight;
		} 

		if ((angle >= 3.0*M_PI/4.0) && (angle <= 5.0*M_PI/4.0) )
		{
			x -= iconWidth;
		}
	
		
		CG_DrawPic( x, y, iconWidth, iconHeight, shader );


		// END Mad Doc - TDF

	}
#endif
}

/*
=================
CG_DrawNewCompass
=================
*/
static void CG_DrawNewCompass( void ) {
	float basex, basey;
	float basew, baseh;
	snapshot_t	*snap;
	float angle;
	int i;
	static float lastangle = 0;
	static float anglespeed = 0;
	float diff;

	if ( cg.nextSnap && !cg.nextFrameTeleport && !cg.thisFrameTeleport ) {
		snap = cg.nextSnap;
	} else {
		snap = cg.snap;
	}

	if ( snap->ps.pm_flags & PMF_LIMBO || snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR || cg.mvTotalClients > 0 )
		return;

	// Arnout: bit larger
/*	basex = 520 - 16;
	basey = 20 - 16;
	basew = 100 + 32;
	baseh = 100 + 32;*/
	basex = cg.hud.compass[0];
	basey = cg.hud.compass[1];
	basew = cg.hud.compass[2];
	baseh = cg.hud.compass[2];

	CG_DrawAutoMap();

	if( cgs.autoMapExpanded ) {
		if( cg.time - cgs.autoMapExpandTime < 100.f ) {
			basey -= ( ( cg.time - cgs.autoMapExpandTime ) / 100.f ) * 128.f;
		} else {
			//basey -= 128.f;
			return;
		}
	} else {
		if( cg.time - cgs.autoMapExpandTime <= 150.f ) {
			//basey -= 128.f;
			return;
		} else if( ( cg.time - cgs.autoMapExpandTime > 150.f ) && ( cg.time - cgs.autoMapExpandTime < 250.f ) ) {

			basey = ( basey - 128.f ) + ( ( cg.time - cgs.autoMapExpandTime - 150.f ) / 100.f ) * 128.f;
		} else {
			rectDef_t compassHintRect =	{ 640 - 22, 128, 20, 20 };

			CG_DrawKeyHint( &compassHintRect, "+mapexpand" );
		}
	}

	angle = ( cg.predictedPlayerState.viewangles[YAW] + 180.f ) / 360.f - ( 0.125f );	
	diff = AngleSubtract( angle * 360, lastangle * 360 ) / 360.f;
	anglespeed /= 1.08f;
	anglespeed += diff * 0.01f;
	if( Q_fabs(anglespeed) < 0.00001f ) {
		anglespeed = 0;
	}
	lastangle += anglespeed;

//	if( !(cgs.ccFilter & CC_FILTER_REQUESTS) ) {
		// draw voice chats
		for ( i = 0; i < MAX_CLIENTS; i++ ) {
			centity_t *cent = &cg_entities[i];

			if ( cg.predictedPlayerState.clientNum == i || !cgs.clientinfo[i].infoValid || cg.predictedPlayerState.persistant[PERS_TEAM] != cgs.clientinfo[i].team )
				continue;

			// also draw revive icons if cent is dead and player is a medic
			if ( cent->voiceChatSpriteTime < cg.time ) {
				continue;
			}

			if ( cgs.clientinfo[i].health <= 0 ) {
				// reset
				cent->voiceChatSpriteTime = cg.time;
				continue;
			}

			CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->lerpOrigin, cent->voiceChatSprite );
		}
//	}

	/*if( !(cgs.ccFilter & CC_FILTER_DESTRUCTIONS) ) {
		// draw explosives if an engineer
		if ( cg.predictedPlayerState.stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
			for ( i = 0; i < snap->numEntities; i++ ) {
				centity_t *cent = &cg_entities[ snap->entities[ i ].number ];

				if ( cent->currentState.eType != ET_EXPLOSIVE_INDICATOR ) {
					continue;
				}

				if ( cent->currentState.teamNum == 1 && cg.predictedPlayerState.persistant[PERS_TEAM] == TEAM_AXIS )
					continue;
				else if ( cent->currentState.teamNum == 2 && cg.predictedPlayerState.persistant[PERS_TEAM] == TEAM_ALLIES )
					continue;

				CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->lerpOrigin, cgs.media.compassDestroyShader );
			}
		}
	}*/

//	if( !(cgs.ccFilter & CC_FILTER_REQUESTS) ) {
		// draw revive medic icons
		if ( cg.predictedPlayerState.stats[ STAT_PLAYER_CLASS ] == PC_MEDIC ) {
			for ( i = 0; i < snap->numEntities; i++ ) {
				entityState_t *ent = &snap->entities[i];

				if ( ent->eType != ET_PLAYER )
					continue;

				if ( ( ent->eFlags & EF_DEAD ) && ent->number == ent->clientNum ) {
					if ( !cgs.clientinfo[ent->clientNum].infoValid || cg.predictedPlayerState.persistant[PERS_TEAM] != cgs.clientinfo[ent->clientNum].team )
						continue;

					CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, ent->pos.trBase, cgs.media.medicReviveShader );
				}
			}
		}
//	}

/*	if( !(cgs.ccFilter & CC_FILTER_DESTRUCTIONS) ) {
		// draw constructibles if an engineer
		if ( cg.predictedPlayerState.stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
			for ( i = 0; i < snap->numEntities; i++ ) {
				centity_t *cent = &cg_entities[ snap->entities[ i ].number ];

				if ( cent->currentState.eType != ET_CONSTRUCTIBLE_INDICATOR ) {
					continue;
				}

				if ( cent->currentState.teamNum != cg.predictedPlayerState.persistant[PERS_TEAM] && cent->currentState.teamNum != 3 )
					continue;

				CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->lerpOrigin, cgs.media.compassConstructShader );
			}
		}
	}*/

/*	if( !(cgs.ccFilter & CC_FILTER_WAYPOINTS) ) {
		// draw waypoint icons
		for ( i = 0; i < snap->numEntities; i++ ) {
			centity_t *cent = &cg_entities[ snap->entities[ i ].number ];

			if( cent->currentState.eType != ET_WAYPOINT ) {
				continue;
			}

			// see if the waypoint owner is someone that you accept waypoints from
			if( !CG_IsOnSameFireteam( cg.clientNum, cent->currentState.clientNum )) {  // TODO: change to fireteam
					continue;
			}

			switch( cent->currentState.frame ) {
			case WAYP_ATTACK: CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->currentState.pos.trBase, cgs.media.waypointCompassAttackShader ); break;
			case WAYP_DEFEND: CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->currentState.pos.trBase, cgs.media.waypointCompassDefendShader ); break;
			case WAYP_REGROUP: CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->currentState.pos.trBase, cgs.media.waypointCompassRegroupShader ); break;
			}
		}
	}*/

//	if( !(cgs.ccFilter & CC_FILTER_BUDDIES) ) {
		for ( i = 0; i < snap->numEntities; i++ ) {
			entityState_t *ent = &snap->entities[i];

			if ( ent->eType != ET_PLAYER ) {
				continue;
			}

			if ( ent->eFlags & EF_DEAD ) {
				continue;
			}

			if ( !cgs.clientinfo[ent->clientNum].infoValid || cg.predictedPlayerState.persistant[PERS_TEAM] != cgs.clientinfo[ent->clientNum].team ) {
				continue;
			}
			
			if(!CG_IsOnSameFireteam( cg.clientNum, ent->clientNum )) {
				continue;
			}

			CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, ent->pos.trBase, cgs.media.buddyShader );
		}
//	}
}

static int CG_PlayerAmmoValue( int *ammo, int *clips, int *akimboammo ) {
	centity_t		*cent;
	playerState_t	*ps;
	int				weap;
	qboolean		skipammo = qfalse;

	*ammo = *clips = *akimboammo = -1;

	if( cg.snap->ps.clientNum == cg.clientNum )
		cent = &cg.predictedPlayerEntity;
	else
		cent = &cg_entities[cg.snap->ps.clientNum];
	ps = &cg.snap->ps;

	weap = cent->currentState.weapon;

	if ( !weap )
		return weap;

	if ( weap == WP_KNIFE && cgs.throwableKnives <= 0 ) {
		return weap;
	}

	switch(weap) {		// some weapons don't draw ammo count text
		case WP_AMMO:
		case WP_MEDKIT:
		//KW: draw the throwing knife ammo
		//case WP_KNIFE:	
		case WP_PLIERS:
		case WP_SMOKE_MARKER:
		case WP_DYNAMITE:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
		case WP_SMOKE_BOMB:
		case WP_BINOCULARS:
			return weap;

		case WP_LANDMINE:
		case WP_MEDIC_SYRINGE:
		case WP_MEDIC_ADRENALINE:
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_FLAMETHROWER:
		case WP_MORTAR:
		case WP_MORTAR_SET:
		case WP_PANZERFAUST:
#ifdef __EF__
		case WP_AIRDROP_MARKER:
#endif //__EF__
			skipammo = qtrue;
			break;

		default:
			break;
	}

	if( cg.snap->ps.eFlags & EF_MG42_ACTIVE || cg.snap->ps.eFlags & EF_MOUNTEDTANK 
#ifdef __VEHICLES__
		|| cg.snap->ps.eFlags & EF_VEHICLE 
#endif //__VEHICLES__
		)
	{ // @TODO
		return WP_MOBILE_MG42;
	}

	// total ammo in clips
	*clips = cg.snap->ps.ammo[BG_FindAmmoForWeapon(weap)];

	// current clip
	*ammo = ps->ammoclip[BG_FindClipForWeapon(weap)];

	if( BG_IsAkimboWeapon( weap ) ) {
		*akimboammo = ps->ammoclip[BG_FindClipForWeapon(BG_AkimboSidearm(weap))];
	} else {
		*akimboammo = -1;
	}

	if( weap == WP_LANDMINE ) {
		if( !cgs.gameManager ) {
			*ammo = 0;
		} else {
			if( cgs.clientinfo[ps->clientNum].team == TEAM_AXIS ) {
				*ammo = cgs.gameManager->currentState.otherEntityNum;
			} else {
				*ammo = cgs.gameManager->currentState.otherEntityNum2;
			}
		}
	} else if( weap == WP_MORTAR || weap == WP_MORTAR_SET || weap == WP_PANZERFAUST ) {
		*ammo += *clips;
	}
	
	if( skipammo ) {
		*clips = -1;
	}

	return weap;
}

#define HEAD_TURNTIME 10000
#define HEAD_TURNANGLE 20
#define HEAD_PITCHANGLE 2.5
static void CG_DrawPlayerStatusHead( void ) {
	hudHeadAnimNumber_t anim;
	rectDef_t headRect;// =		{ 44, 480 - 92, 62, 80 };
//	rectDef_t headHintRect =	{ 40, 480 - 22, 20, 20 };
	bg_character_t* character = CG_CharacterForPlayerstate( &cg.snap->ps );
	bg_character_t* headcharacter = BG_GetCharacter( cgs.clientinfo[ cg.snap->ps.clientNum ].team, cgs.clientinfo[ cg.snap->ps.clientNum ].cls );

	qhandle_t painshader = 0;

	if(cg.hud.head[0] < 0)
		return;

	anim = cg.idleAnim;
	headRect.x = cg.hud.head[0];
	headRect.y = cg.hud.head[1];
	headRect.w = cg.hud.head[2];
	headRect.h = cg.hud.head[3];

	if( cg.weaponFireTime > 500 ) {
		anim = HD_ATTACK;
	} else if( cg.time - cg.lastFiredWeaponTime < 500 ) {
		anim = HD_ATTACK_END;
	} else if( cg.time - cg.painTime < (character->hudheadanimations[ HD_PAIN ].numFrames * character->hudheadanimations[ HD_PAIN ].frameLerp) ) {
		anim = HD_PAIN;
	} else if( cg.time > cg.nextIdleTime ) { 
		cg.nextIdleTime = cg.time + 7000 + rand() % 1000;
		if( cg.snap->ps.stats[ STAT_HEALTH ] < 40 ) {
			cg.idleAnim = (rand() % (HD_DAMAGED_IDLE3 - HD_DAMAGED_IDLE2 + 1)) + HD_DAMAGED_IDLE2;
		} else {
			cg.idleAnim = (rand() % (HD_IDLE8 - HD_IDLE2 + 1)) + HD_IDLE2;
		}

		cg.lastIdleTimeEnd = cg.time + character->hudheadanimations[ cg.idleAnim ].numFrames * character->hudheadanimations[ cg.idleAnim ].frameLerp;
	}

	if( cg.snap->ps.stats[ STAT_HEALTH ] < 5 ) {
		painshader = cgs.media.hudDamagedStates[3];
	} else if( cg.snap->ps.stats[ STAT_HEALTH ] < 20 ) {
		painshader = cgs.media.hudDamagedStates[2];
	} else if( cg.snap->ps.stats[ STAT_HEALTH ] < 40 ) {
		painshader = cgs.media.hudDamagedStates[1];
	} else if( cg.snap->ps.stats[ STAT_HEALTH ] < 60 ) {
		painshader = cgs.media.hudDamagedStates[0];
	}

	if( cg.time > cg.lastIdleTimeEnd ) {
		if( cg.snap->ps.stats[ STAT_HEALTH ] < 40 ) {
			cg.idleAnim = HD_DAMAGED_IDLE1;
		} else {
			cg.idleAnim = HD_IDLE1;
		}
	}
	

	CG_DrawPlayerHead( &headRect, character, headcharacter, 180, 0, cg.snap->ps.eFlags & EF_HEADSHOT ? qfalse : qtrue, anim, painshader, cgs.clientinfo[ cg.snap->ps.clientNum ].rank, qfalse );

//	CG_DrawKeyHint( &headHintRect, "openlimbomenu" );
}

extern void CG_HealthBar(float x, float y, float w, float h, float *startColor, float *endColor, const float *bgColor, float frac, int flags);

static void CG_DrawPlayerHealthBar( rectDef_t *rect ) {
	vec4_t bgcolour =	{	1.f,	1.f,	1.f,	0.3f	};
	vec4_t colour;
		
	int flags = 1|4|16|64|0x0100;
	float frac;

	if (g_realism.integer && !(cg.snap->ps.eFlags & EF_VEHICLE))
		return; // UQ1: No player health bar visible in realism mode... (but keep vehicle health bar)

	CG_ColorForHealth( colour );
	colour[3] = 0.5f;

#ifdef __VEHICLES__
	if (cg.snap->ps.eFlags & EF_VEHICLE)
	{
		frac = cg.snap->ps.stats[STAT_HEALTH]*0.01; // For tanks, this is a percent anyway...
	} 
	else 
#endif //__VEHICLES__
	if ( cgs.clientinfo[ cg.snap->ps.clientNum ].cls == PC_MEDIC ) 
	{
		frac = cg.snap->ps.stats[STAT_HEALTH] / ( (float) cg.snap->ps.stats[STAT_MAX_HEALTH] * 1.12f );
	} 
	else 
	{
		frac = cg.snap->ps.stats[STAT_HEALTH] / (float) cg.snap->ps.stats[STAT_MAX_HEALTH];
	}


	if (!cg.hud.healthbar_vertical)
	{
		flags = 64|128|0x0100;

		rect->x = cg.hud.healthbar[0];
		rect->y = cg.hud.healthbar[1];
		rect->w = 64;//122;
  		rect->h = 4;//10;

		CG_FillRect( rect->x, rect->y, rect->w, rect->h, popBG );
		//CG_FilledBar( rect->x, rect->y, rect->w, rect->h, colour, NULL, NULL, frac, flags );
		CG_HealthBar( rect->x, rect->y, rect->w, rect->h, popRed, popBlue, NULL, frac, flags );
		CG_DrawRect_FixedBorder( rect->x, rect->y, rect->w, rect->h, POP_HUD_BORDERSIZE, popBorder );
	

		//CG_FilledBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, colour, NULL, bgcolour, frac, flags );

		trap_R_SetColor( NULL );
		//CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hudSprintBar );
		//CG_DrawPic( rect->x, rect->y + rect->h + 4, rect->w, rect->w, cgs.media.hudHealthIcon );
		CG_DrawPic( rect->x-16, rect->y-4, 12, 12, cgs.media.hudHealthIcon );
	}
	else
	{
		rect->x = cg.hud.healthbar[0];
		rect->y = cg.hud.healthbar[1];
		rect->w = cg.hud.healthbar[2];
  		rect->h = 72;

		//CG_FilledBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, colour, NULL, bgcolour, frac, flags );
		CG_HealthBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, popRed, popBlue, bgcolour, frac, flags );

		trap_R_SetColor( NULL );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hudSprintBar );
		CG_DrawPic( rect->x, rect->y + rect->h + 4, rect->w, rect->w, cgs.media.hudHealthIcon );
	}
}

static void CG_DrawStaminaBar( rectDef_t *rect ) {
	vec4_t bgcolour =	{	1.f,	1.f,	1.f,	0.3f	};
	vec4_t colour =		{	0.1f,	1.0f,	0.1f,	0.5f	};
	vec4_t colourlow =	{	1.0f,	0.1f,	0.1f,	0.5f	};
	vec_t* color = colour;
	int flags = 1|4|16|64;
	float frac = cg.pmext.sprintTime / (float)SPRINTTIME;

	if( cg.snap->ps.powerups[PW_ADRENALINE] ) {
		if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
			Vector4Average( colour, colorWhite, sin(cg.time*.005f), colour);
		} else {
			float msec = cg.snap->ps.powerups[PW_ADRENALINE] - cg.time;

			if( msec < 0 ) {
				msec = 0;
			} else {
				Vector4Average( colour, colorWhite, .5f + sin(.2f * sqrt(msec) * 2 * M_PI) * .5f, colour);
			}
		}
	} else {
		if( frac < 0.25 ) {
			color = colourlow;
		}
	}

	if (!cg.hud.staminabar_vertical)
	{
		flags = 64|128;

		rect->x = cg.hud.staminabar[0];
		rect->y = cg.hud.staminabar[1];
		rect->w = 64;//122;
  		rect->h = 4;//10;

		CG_FillRect( rect->x, rect->y, rect->w, rect->h, popBG );
		CG_FilledBar( rect->x, rect->y, rect->w, rect->h, color, NULL, NULL, frac, flags );
		CG_DrawRect_FixedBorder( rect->x, rect->y, rect->w, rect->h, POP_HUD_BORDERSIZE, popBorder );
	

	//	CG_FilledBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, color, NULL, bgcolour, frac, flags );

		trap_R_SetColor( NULL );
		//CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hudSprintBar );
		//CG_DrawPic( rect->x, rect->y + rect->h + 4, rect->w, rect->w, cgs.media.hudSprintIcon);
		CG_DrawPic( rect->x-16, rect->y-4, 12, 12, cgs.media.hudSprintIcon );
	}
	else
	{
		rect->x = cg.hud.staminabar[0];
		rect->y = cg.hud.staminabar[1];
		rect->w = cg.hud.staminabar[2];
  		rect->h = 72;

		CG_FilledBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, color, NULL, bgcolour, frac, flags );

		trap_R_SetColor( NULL );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hudSprintBar );
		CG_DrawPic( rect->x, rect->y + rect->h + 4, rect->w, rect->w, cgs.media.hudSprintIcon);
	}
}

static void CG_DrawWeapRecharge( rectDef_t *rect ) {
	float		barFrac, chargeTime;
	int			weap, flags;
	qboolean	fade = qfalse;

	vec4_t	bgcolor = { 1.0f, 1.0f, 1.0f, 0.25f };
	vec4_t	color;

	flags = 1|4|16;

	weap = cg.snap->ps.weapon;

//	if( !(cg.snap->ps.eFlags & EF_ZOOMING) ) {
//		if ( weap != WP_PANZERFAUST && weap != WP_DYNAMITE && weap != WP_MEDKIT && weap != WP_SMOKE_GRENADE && weap != WP_PLIERS && weap != WP_AMMO ) {
//			fade = qtrue;
//		}
//	}

	// Draw power bar
#ifdef __VEHICLES__
	if (cg.snap->ps.eFlags & EF_VEHICLE) {

		chargeTime = 4500.0f;
		barFrac = 1.0f-((float)(cg.snap->ps.grenadeTimeLeft/*weaponTime*/)/chargeTime);

	} else { // normal
#endif //__VEHICLES__
	if( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
		chargeTime = cg.engineerChargeTime[cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_MEDIC ) {
		chargeTime = cg.medicChargeTime[cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_FIELDOPS ) {
		chargeTime = cg.ltChargeTime[cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_COVERTOPS ) {
		chargeTime = cg.covertopsChargeTime[cg.snap->ps.persistant[PERS_TEAM]-1];
	} else {
		chargeTime = cg.soldierChargeTime[cg.snap->ps.persistant[PERS_TEAM]-1];
	}

	barFrac = (float)(cg.time - cg.snap->ps.classWeaponTime) / chargeTime;
#ifdef __VEHICLES__
	} // tank
#endif //__VEHICLES__

	if( barFrac > 1.0 ) {
		barFrac = 1.0;
	}

	color[0] = 1.0f;
	color[1] = color[2] = barFrac;
	color[3] = 0.25 + barFrac*0.5;

	if ( fade ) {
		bgcolor[3] *= 0.4f;
		color[3] *= 0.4;
	}
	

	if (!cg.hud.chargebar_vertical)
	{
		flags = 64|128;

		rect->x = cg.hud.chargebar[0];
		rect->y = cg.hud.chargebar[1];
		rect->w = 64;//122;
  		rect->h = 4;//10;

		CG_FillRect( rect->x, rect->y, rect->w, rect->h, popBG );
		CG_FilledBar( rect->x, rect->y, rect->w, rect->h, color, NULL, NULL, barFrac, flags );
		CG_DrawRect_FixedBorder( rect->x, rect->y, rect->w, rect->h, POP_HUD_BORDERSIZE, popBorder );
	

	//	CG_FilledBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, color, NULL, bgcolor, barFrac, flags );

		trap_R_SetColor( NULL );
		//CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hudSprintBar );
		//CG_DrawPic( rect->x + (rect->w * 0.25f) - 1, rect->y + rect->h + 4, (rect->w * 0.5f) + 2, rect->w + 2, cgs.media.hudPowerIcon );
		CG_DrawPic( rect->x-14, rect->y-4, 7, 12, cgs.media.hudPowerIcon );
	}
	else
	{
		rect->x = cg.hud.chargebar[0];
		rect->y = cg.hud.chargebar[1];
		rect->w = cg.hud.chargebar[2];
  		rect->h = 72;

		CG_FilledBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, color, NULL, bgcolor, barFrac, flags );

		trap_R_SetColor( NULL );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hudSprintBar );
		CG_DrawPic( rect->x + (rect->w * 0.25f) - 1, rect->y + rect->h + 4, (rect->w * 0.5f) + 2, rect->w + 2, cgs.media.hudPowerIcon );
	}
}

#ifdef __VEHICLES__
extern void CG_DrawTankMGHeat(rectDef_t *rect, int align);

static void CG_DrawPlayerStatus_Vehicle( void ) {
	int				value, value2, value3;
	char			buffer[32];
	int				weap;
//	float			ammocountScale;
	playerState_t	*ps;
	rectDef_t		rect;
//	vec4_t			colorFaded = { 1.f, 1.f, 1.f, 0.3f };

	ps = &cg.snap->ps;
	
#ifdef __VEHICLES__
	if (!(cg.snap->ps.eFlags & EF_VEHICLE)) 
	{
#endif //__VEHICLES__
		// Draw weapon icon and overheat bar
		rect.x = 640 - 82;
		rect.y = 480 - 56;
		rect.w = 60;
		rect.h = 32;
		CG_DrawWeapHeat( &rect, HUD_HORIZONTAL );

		if( cg.mvTotalClients < 1 && cg_drawWeaponIconFlash.integer == 0 ) {
			CG_DrawPlayerWeaponIcon(&rect, qtrue, ITEM_ALIGN_RIGHT, &colorWhite);
		} else {
			int ws = (cg.mvTotalClients > 0) ? cgs.clientinfo[cg.snap->ps.clientNum].weaponState : BG_simpleWeaponState(cg.snap->ps.weaponstate);
			CG_DrawPlayerWeaponIcon(&rect, (ws != WSTATE_IDLE), ITEM_ALIGN_RIGHT, ((ws == WSTATE_SWITCH) ? &colorWhite : (ws == WSTATE_FIRE) ? &colorRed : &colorYellow));
		}

		// Draw ammo	
		weap = CG_PlayerAmmoValue( &value, &value2, &value3 );
		if( value3 >= 0 ) {
			Com_sprintf( buffer, sizeof(buffer), "%i|%i/%i", value3, value, value2 );
			CG_Text_Paint_Ext( 640 - 22 - CG_Text_Width_Ext( buffer, .25f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahoma16 ), 480 - 1 * ( 16 + 2 ) + 12 - 4, .25f, .25f, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
//			CG_DrawPic( 640 - 2 * ( 12 + 2 ) - 16 - 4, 480 - 1 * ( 16 + 2 ) - 4, 16, 16, cgs.media.SPPlayerInfoAmmoIcon );
		} else if( value2 >= 0 ) {
			Com_sprintf( buffer, sizeof(buffer), "%i/%i", value, value2 );
			CG_Text_Paint_Ext( 640 - 22 - CG_Text_Width_Ext( buffer, .25f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahoma16 ), 480 - 1 * ( 16 + 2 ) + 12 - 4, .25f, .25f, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
//			CG_DrawPic( 640 - 2 * ( 12 + 2 ) - 16 - 4, 480 - 1 * ( 16 + 2 ) - 4, 16, 16, cgs.media.SPPlayerInfoAmmoIcon );
		} else if( value >= 0 ) {
			Com_sprintf( buffer, sizeof(buffer), "%i", value );
			CG_Text_Paint_Ext( 640 - 22 - CG_Text_Width_Ext( buffer, .25f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahoma16 ), 480 - 1 * ( 16 + 2 ) + 12 - 4, .25f, .25f, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
//			CG_DrawPic( 640 - 2 * ( 12 + 2 ) - 16 - 4, 480 - 1 * ( 16 + 2 ) - 4, 16, 16, cgs.media.SPPlayerInfoAmmoIcon );
		}
#ifdef __VEHICLES__
	}
#endif //__VEHICLES__

#ifdef __VEHICLES__
	if (cg.snap->ps.eFlags & EF_VEHICLE) 
	{
		// Draw weapon icon and overheat bar
		//rect.x = 640 - 82;
		//rect.y = 480 - 56;
		//rect.w = 12;//60;
		//rect.h = 72;//32;

		//rect.x = 640 - 98;
		//rect.y = 480 - 72;
		rect.x = cg.hud.chargebar[0];
		rect.y = cg.hud.chargebar[1]-24;
		rect.w = 64;//122;
  		rect.h = 4;//10;

		CG_DrawTankMGHeat( &rect, HUD_HORIZONTAL );

		//rect.x = 640 - 106;
		//rect.y = 480 - 78;
		rect.x = cg.hud.chargebar[0]-18;
		rect.y = cg.hud.chargebar[1]-30;
		rect.w = 16;
  		rect.h = 12;

		if( cg.mvTotalClients < 1 && cg_drawWeaponIconFlash.integer == 0 ) {
			CG_DrawPlayerWeaponIcon(&rect, qtrue, ITEM_ALIGN_RIGHT, &colorWhite);
		} else {
			int ws = (cg.mvTotalClients > 0) ? cgs.clientinfo[cg.snap->ps.clientNum].weaponState : BG_simpleWeaponState(cg.snap->ps.weaponstate);
			CG_DrawPlayerWeaponIcon(&rect, (ws != WSTATE_IDLE), ITEM_ALIGN_RIGHT, ((ws == WSTATE_SWITCH) ? &colorWhite : (ws == WSTATE_FIRE) ? &colorRed : &colorYellow));
		}

		// Draw ammo	
		/*weap = CG_PlayerAmmoValue( &value, &value2, &value3 );
		if( value3 >= 0 ) {
			Com_sprintf( buffer, sizeof(buffer), "%i|%i/%i", value3, value, value2 );
			CG_Text_Paint_Ext( 640 - 22 - CG_Text_Width_Ext( buffer, .25f, 0, &cgs.media.tahoma16 ), 480 - 1 * ( 16 + 2 ) + 12 - 4, .25f, .25f, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahomabd24 );
		} else if( value2 >= 0 ) {
			Com_sprintf( buffer, sizeof(buffer), "%i/%i", value, value2 );
			CG_Text_Paint_Ext( 640 - 22 - CG_Text_Width_Ext( buffer, .25f, 0, &cgs.media.tahoma16 ), 480 - 1 * ( 16 + 2 ) + 12 - 4, .25f, .25f, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahomabd24 );
		} else if( value >= 0 ) {
			Com_sprintf( buffer, sizeof(buffer), "%i", value );
			CG_Text_Paint_Ext( 640 - 22 - CG_Text_Width_Ext( buffer, .25f, 0, &cgs.media.tahoma16 ), 480 - 1 * ( 16 + 2 ) + 12 - 4, .25f, .25f, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahomabd24 );
		}*/
	}
#endif //__VEHICLES__

// ==
	rect.x = 24;
	rect.y = 480 - 92;
	rect.w = 12;
	rect.h = 72;
	CG_DrawPlayerHealthBar( &rect );
// ==

// ==
	rect.x = 4;
	rect.y = 480 - 92;
	rect.w = 12;
	rect.h = 72;
#ifdef __VEHICLES__
	if (!(cg.snap->ps.eFlags & EF_VEHICLE)) 
	{
		CG_DrawStaminaBar( &rect );
	} 
	else 
	{
		CG_DrawWeapRecharge( &rect );
	}
#else //!__VEHICLES__
	CG_DrawWeapRecharge( &rect );
#endif //__VEHICLES__
// ==

// ==
#ifdef __VEHICLES__
	if (!(cg.snap->ps.eFlags & EF_VEHICLE)) 
	{
		rect.x = 640 - 16;
		rect.y = 480 - 92;
		rect.w = 12;
		rect.h = 72;
		CG_DrawWeapRecharge( &rect );
	}
#else //!__VEHICLES_
	rect.x = 640 - 16;
	rect.y = 480 - 92;
	rect.w = 12;
	rect.h = 72;
	CG_DrawWeapRecharge( &rect );
#endif //__VEHICLES__
// ==
}
#endif //__VEHICLES__

static void CG_DrawPlayerStatus( void ) {
	int				value, value2, value3;
	char			buffer[32];
	int				weap;
	float			ammocountScale;
	playerState_t	*ps;
	rectDef_t		rect;
//	vec4_t			colorFaded = { 1.f, 1.f, 1.f, 0.3f };

	ps = &cg.snap->ps;
	
#ifdef __VEHICLES__
	if (cg.snap->ps.eFlags & EF_VEHICLE) 
	{
		if (!cg.hud.healthbar_vertical)
		{// Using new horizontal HUD.. Draw a border around it...
			CG_FillRect( 546, 410, 90, 68, popBG );
			CG_DrawRect_FixedBorder( 546, 410, 90, 68, POP_HUD_BORDERSIZE, popBorder );
		}

		CG_DrawPlayerStatus_Vehicle();
		return;
	}
#endif //__VEHICLES__

	if (!cg.hud.healthbar_vertical)
	{// Using new horizontal HUD.. Draw a border around it...
		CG_FillRect( 546, 410, 90, 68, popBG );
		CG_DrawRect_FixedBorder( 546, 410, 90, 68, POP_HUD_BORDERSIZE, popBorder );
	}

  	// Draw weapon icon and overheat bar
	if(cg.hud.overheat[0] >= 0) {
		rect.x = cg.hud.overheat[0];
		rect.y = cg.hud.overheat[1];
		rect.w = cg.hud.overheat[2];
		rect.h = cg.hud.overheat[3];
		CG_DrawWeapHeat( &rect, HUD_HORIZONTAL );
	}

	// reset for weapon itself
	if(cg.hud.weaponcard[0] >= 0) {
#ifdef __ETE__
		// ETE has this smaller
		rect.x = cg.hud.weaponcard[0];
		rect.y = cg.hud.weaponcard[1];
		rect.w = 32;//cg.hud.weaponcard[2];
		rect.h = 24;
#else //!__ETE__
		rect.x = cg.hud.weaponcard[0];
		rect.y = cg.hud.weaponcard[1];
		rect.w = cg.hud.weaponcard[2];
		rect.h = 32;
#endif //__ETE__
  	if( cg.mvTotalClients < 1 && cg_drawWeaponIconFlash.integer == 0 ) {
  		CG_DrawPlayerWeaponIcon(&rect, qtrue, ITEM_ALIGN_RIGHT, &colorWhite);
  	} else {
  		int ws = (cg.mvTotalClients > 0) ? cgs.clientinfo[cg.snap->ps.clientNum].weaponState : BG_simpleWeaponState(cg.snap->ps.weaponstate);
  		CG_DrawPlayerWeaponIcon(&rect, (ws != WSTATE_IDLE), ITEM_ALIGN_RIGHT, ((ws == WSTATE_SWITCH) ? &colorWhite : (ws == WSTATE_FIRE) ? &colorRed : &colorYellow));
  	}
	}
  
  	// Draw ammo
	if(cg.hud.ammo[0] >= 0) {
  	weap = CG_PlayerAmmoValue( &value, &value2, &value3 );
		ammocountScale = (float)(cg.hud.ammo[2])/100;
  	if( value3 >= 0 ) {
  		Com_sprintf( buffer, sizeof(buffer), "%i|%i/%i", value3, value, value2 );
			CG_Text_Paint_Ext( cg.hud.ammo[0] - CG_Text_Width_Ext( buffer, ammocountScale, 0, &cgs.media.tahoma16 ), cg.hud.ammo[1], ammocountScale, ammocountScale, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
  //		CG_DrawPic( 640 - 2 * ( 12 2 ) - 16 - 4, 480 - 1 * ( 16 2 ) - 4, 16, 16, cgs.media.SPPlayerInfoAmmoIcon );
  	} else if( value2 >= 0 ) {
  		Com_sprintf( buffer, sizeof(buffer), "%i/%i", value, value2 );
			CG_Text_Paint_Ext( cg.hud.ammo[0] - CG_Text_Width_Ext( buffer, ammocountScale, 0, &cgs.media.tahoma16 ), cg.hud.ammo[1], ammocountScale, ammocountScale, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
  //		CG_DrawPic( 640 - 2 * ( 12 2 ) - 16 - 4, 480 - 1 * ( 16 2 ) - 4, 16, 16, cgs.media.SPPlayerInfoAmmoIcon );
  	} else if( value >= 0 ) {
  		Com_sprintf( buffer, sizeof(buffer), "%i", value );
			CG_Text_Paint_Ext( cg.hud.ammo[0] - CG_Text_Width_Ext( buffer, ammocountScale, 0, &cgs.media.tahoma16 ), cg.hud.ammo[1], ammocountScale, ammocountScale, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
  //		CG_DrawPic( 640 - 2 * ( 12 2 ) - 16 - 4, 480 - 1 * ( 16 2 ) - 4, 16, 16, cgs.media.SPPlayerInfoAmmoIcon );
  	}
	}

  // ==
	if(cg.hud.healthbar[0] >= 0) {
		CG_DrawPlayerHealthBar( &rect );
	}
  // ==
  
  // ==
	if(cg.hud.staminabar[0] >= 0) {
	 	CG_DrawStaminaBar( &rect );
	}
  // ==
  
  // ==
	if(cg.hud.chargebar[0] >= 0) {
	  	CG_DrawWeapRecharge( &rect );
	}
}

#ifndef __ETE__
static void CG_DrawSkillBar( float x, float y, float w, float h, int skill ) {
	int i;
	float blockheight = ( h - 4 ) / (float)(NUM_SKILL_LEVELS - 1);
	float draw_y;
	vec4_t colour;
	float x1, y1, w1, h1;

	draw_y = y + h - blockheight;
	for( i = 0; i < NUM_SKILL_LEVELS - 1; i++ ) {
		if( i >= skill ) {
			Vector4Set( colour, 1.f, 1.f, 1.f, .15f );
		} else {
			Vector4Set( colour, 0.f, 0.f, 0.f, .4f );
		}

		CG_FillRect( x, draw_y, w, blockheight, colour );

		if( i < skill ) {
			x1 = x;
			y1 = draw_y;
			w1 = w;
			h1 = blockheight;
			CG_AdjustFrom640( &x1, &y1, &w1, &h1 );

			trap_R_DrawStretchPic( x1, y1, w1, h1, 0, 0, 1.f, 0.5f, cgs.media.limboStar_roll );
		}

		CG_DrawRect_FixedBorder( x, draw_y, w, blockheight, 1, colorBlack );
//		CG_DrawPic( x, draw_y, w, blockheight, cgs.media.hudBorderVert2 );
		draw_y -= ( blockheight + 1 );
	}
/*	CG_Text_Paint_Ext( 
		x, 
		300, 
		0.25f, 0.25f, 
		clr, 
//		va("%d",cgs.clientinfo[cg.snap->ps.clientNum].skill[skill]), 
		va("%d",skill),
		0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont1 );*/
}
#endif //!__ETE__

#define SKILL_ICON_SIZE		14

#define SKILLS_X 112
#define SKILLS_Y 20

#define SKILL_BAR_OFFSET	(2*SKILL_BAR_X_INDENT)
#define SKILL_BAR_X_INDENT	0
#define SKILL_BAR_Y_INDENT	6

#define SKILL_BAR_WIDTH		( SKILL_ICON_SIZE - SKILL_BAR_OFFSET )
#define SKILL_BAR_X			( SKILL_BAR_OFFSET + SKILL_BAR_X_INDENT + SKILLS_X )
#define SKILL_BAR_X_SCALE	( SKILL_ICON_SIZE + 2 )
#define SKILL_ICON_X		( SKILL_BAR_OFFSET + SKILLS_X )
#define SKILL_ICON_X_SCALE	( SKILL_ICON_SIZE + 2 )
#define SKILL_BAR_Y			( SKILL_BAR_Y_INDENT - SKILL_BAR_OFFSET - SKILLS_Y )
#define SKILL_BAR_Y_SCALE	( SKILL_ICON_SIZE + 2 )
#define SKILL_ICON_Y		(- ( SKILL_ICON_SIZE + 2 ) - SKILL_BAR_OFFSET - SKILLS_Y )

skillType_t CG_ClassSkillForPosition( clientInfo_t* ci, int pos ) {
	switch( pos ) {
		case 0:
			return BG_ClassSkillForClass(ci->cls);
		case 1:
			return SK_BATTLE_SENSE;
		case 2:
			return SK_LIGHT_WEAPONS;
	}

	return SK_BATTLE_SENSE;
}

static void CG_DrawPlayerStats( void ) {
	playerState_t		*ps;
	clientInfo_t		*ci;
	skillType_t			skill;
	int					i;
	const char*			str;
	float				w;
	vec_t*				clr;
	float				scaleVal;


#ifdef __VEHICLES__
 	if( cg.snap->ps.eFlags & EF_VEHICLE ) 
	{

 	} 
	else 
#endif //__VEHICLES__
	{
		if(cg.hud.hp[0] >= 0) 
		{
  			str = va( "%i", cg.snap->ps.stats[STAT_HEALTH] );
 			scaleVal = cg.hud.hp[2]/100.f;
 			w = CG_Text_Width_Ext( str, scaleVal, 0, &cgs.media.tahoma16 );
 			CG_Text_Paint_Ext( cg.hud.hp[0] - w, cg.hud.hp[1], scaleVal, scaleVal, colorWhite, str, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
 			CG_Text_Paint_Ext( cg.hud.hp[0] + 2, cg.hud.hp[1], scaleVal, scaleVal, colorWhite, "HP", 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
		}
 	}

	if( cgs.gametype == GT_WOLF_LMS ) {
		return;
	}

	ps = &cg.snap->ps;
	ci = &cgs.clientinfo[ ps->clientNum ];

#ifndef __ETE__
	for( i = 0; i < 3; i++ ) {
		skill = CG_ClassSkillForPosition( ci, i );
		if(cg.hud.skillboxes[i][0] >= 0) {
			CG_DrawSkillBar( cg.hud.skillboxes[i][0], cg.hud.skillboxes[i][1], cg.hud.skillboxes[i][2], 4 * cg.hud.skillboxes[i][2], ci->skill[skill] );
		}

		if(cg.hud.skillpics[i][0] >= 0) {
			CG_DrawPic( cg.hud.skillpics[i][0], cg.hud.skillpics[i][1], cg.hud.skillpics[i][2], cg.hud.skillpics[i][2], cgs.media.skillPics[skill] );
		}

		if(cg.hud.skilltexts[i][0] >= 0) {
			scaleVal = cg.hud.skilltexts[i][2]/100.f;
			CG_Text_Paint_Ext(cg.hud.skilltexts[i][0],cg.hud.skilltexts[i][1],
				scaleVal, scaleVal, colorWhite, 
				va("%d",ci->skill[skill]),
				0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
		}
	}
#else //__ETE__
	// In ETE, we want to display this numerically...

	for( i = 0; i < 3; i++ ) 
	{
		skill = CG_ClassSkillForPosition( ci, i );

		if(cg.hud.skillboxes[i][0] >= 0) 
		{
			rectDef_t rect;
			int flags = 64|128|256;
			float frac;
			int curLevel = 0, nextLevel = 1;

			rect.x = cg.hud.skillpics[i][0];
			rect.y = cg.hud.skillpics[i][1]-6;
			rect.w = 28;//64;//122;
			rect.h = 4;//10;

			if ( ci->skill[skill] > 0 ) {
				curLevel = ci->skill[skill];
				nextLevel = curLevel + 1;
			}
			frac = 0.0f;
			if ( ci->skill[skill] == (NUM_SKILL_LEVELS - 1) ) {
				frac = 1.0f;
			} else if ( (skillLevels[skill][nextLevel] - skillLevels[skill][curLevel]) != 0 ) {
				frac = (float)((ci->skillpoints[skill] - skillLevels[skill][curLevel]) / 
						(float)(skillLevels[skill][nextLevel] - skillLevels[skill][curLevel]));
			}
			CG_FillRect( rect.x, rect.y, rect.w, rect.h, popBG );
			CG_FilledBar( rect.x, rect.y, rect.w, rect.h, popBlue, popLime, NULL, frac, flags );
			CG_DrawRect_FixedBorder( rect.x, rect.y, rect.w, rect.h, POP_HUD_BORDERSIZE, popBorder );

			trap_R_SetColor( NULL );

			if(cg.hud.skillpics[i][0] >= 0) 
			{
				CG_DrawPic( cg.hud.skillpics[i][0], cg.hud.skillpics[i][1], cg.hud.skillpics[i][2], cg.hud.skillpics[i][2], cgs.media.skillPics[skill] );
			}

			clr = colorWhite;
			CG_Text_Paint_Ext( cg.hud.skillpics[i][0]+20, cg.hud.skillpics[i][1]+14, 0.25f, 0.25f, clr, va("%d",ci->skill[skill]), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
		}
	}
#endif //__ETE__

	if( cg.time - cg.xpChangeTime < 1000 ) {
		clr = colorYellow;
	} else {
		clr = colorWhite;
	}


 	if(cg.hud.xp[0] >= 0) {
		str = va( "%i", cg.snap->ps.stats[STAT_XP] );
 		scaleVal = cg.hud.xp[2]/100.f;
 		w = CG_Text_Width_Ext( str, scaleVal, 0, &cgs.media.tahoma16 );
 		CG_Text_Paint_Ext( cg.hud.xp[0] - w, cg.hud.xp[1], scaleVal, scaleVal, clr, str, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
 		CG_Text_Paint_Ext( cg.hud.xp[0] + 2, cg.hud.xp[1], scaleVal, scaleVal, clr, "XP", 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
 	}
  
  	// draw treasure icon if we have the flag
  	// rain - #274 - use the playerstate instead of the clientinfo
 	if(cg.hud.flagcov[0] >= 0) 
	{
	  	if( ps->powerups[PW_REDFLAG] || ps->powerups[PW_BLUEFLAG] ) 
		{
  			trap_R_SetColor( NULL );
 			CG_DrawPic( cg.hud.flagcov[0], cg.hud.flagcov[1], 36, 36, cgs.media.objectiveShader );
  		} else if ( ps->powerups[PW_OPS_DISGUISED] ) { // Disguised?
 			CG_DrawPic( cg.hud.flagcov[0], cg.hud.flagcov[1], 36, 36, ps->persistant[PERS_TEAM] == TEAM_AXIS ? cgs.media.alliedUniformShader : cgs.media.axisUniformShader );
 		}
	}

	// Draw BOT Fireteam control commander menu...
	CG_Draw_Commander_Order_Menu();
}

static char statsDebugStrings[6][512];
static int statsDebugTime[6];
static int statsDebugTextWidth[6];
static int statsDebugPos;

void CG_InitStatsDebug( void )
{
	memset( &statsDebugStrings, 0, sizeof(statsDebugStrings) );
	memset( &statsDebugTime, 0, sizeof(statsDebugTime) );
	statsDebugPos = -1;
}

void CG_StatsDebugAddText( const char *text )
{
	if( cg_debugSkills.integer ) {
		statsDebugPos++;

		if( statsDebugPos >= 6 )
			statsDebugPos = 0;

		Q_strncpyz( statsDebugStrings[statsDebugPos], text, 512 );
		statsDebugTime[statsDebugPos] = cg.time;
		statsDebugTextWidth[statsDebugPos] = CG_Text_Width_Ext( text, .15f, 0, /*&cgs.media.limboFont2*/&cgs.media.tahoma30 );

		CG_Printf( "%s\n", text );
	}
}

static void CG_DrawStatsDebug( void )
{
	int textWidth = 0;
	float x, y, w, h;
	int i;

	if( !cg_debugSkills.integer )
		return;

	for( i = 0; i < 6; i++ ) {
		if( statsDebugTime[i] + 9000 > cg.time ) {
			if( statsDebugTextWidth[i] > textWidth )
				textWidth = statsDebugTextWidth[i];
		}
	}

	w = textWidth + 6;
	h = 9;
	x = 640 - w;
	y = (480 - 5 * ( 12 + 2 ) + 6 - 4) - 6 - h;	// don't ask

	i = statsDebugPos;

	do {
		vec4_t colour;

		if( statsDebugTime[i] + 9000 <= cg.time ) {
			break;
		}

        colour[0] = colour[1] = colour[2] = .5f;
		if( cg.time - statsDebugTime[i] > 5000 )
			colour[3] = .5f - .5f * ( ( cg.time - statsDebugTime[i] - 5000 ) / 4000.f );
		else
			colour[3] = .5f ;
		CG_FillRect( x, y, w, h, colour );

		colour[0] = colour[1] = colour[2] = 1.f;
		if( cg.time - statsDebugTime[i] > 5000 )
			colour[3] = 1.f - ( ( cg.time - statsDebugTime[i] - 5000 ) / 4000.f );
		else
			colour[3] = 1.f ;
		CG_Text_Paint_Ext( 640.f - 3 - statsDebugTextWidth[i], y + h - 2, .15f, .15f, colour, statsDebugStrings[i], 0, 0, ITEM_TEXTSTYLE_NORMAL, /*&cgs.media.limboFont2*/&cgs.media.tahoma30 );

		y -= h;

		i--;
		if( i < 0 )
			i = 6 - 1;
	} while( i != statsDebugPos );
}

#ifdef __SHELLSHOCK__
void CG_DrawShellShockView()
{
	vec4_t	hcolor;

	if ((cg.snap->ps.pm_flags & PMF_FOLLOW) == 0 && (cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR))
		return;

	if (cg.snap->ps.eFlags & EF_VEHICLE)
		return;

	if (HasExtFlag(cg.snap->ps.clientNum, EX_SHELLSHOCK))
	{
		float	duration	= SecondaryEntityState(cg.snap->ps.clientNum)->effect1Time;
		float	elapsed		= SecondaryEntityState(cg.snap->ps.clientNum)->effect2Time;
		float	alpha	= 0.8f * (1.0f - (elapsed / duration)); // 0.05
		float	x		= (640 - 1024) / 2;
		float	y		= (480 - 1024) / 2;
		float	w		= 1024;
		float	h		= 1024;

		//CG_Printf("%i (ent %i) - Shellshocked! duration is %i. elapsed is %i.\n", cg.time, SecondaryEntityState(cg.snap->ps.clientNum)->number, duration, elapsed);

		hcolor[0]	= 0.0f;
		hcolor[1]	= 0.0f;
		hcolor[2]	= 1.0f;
		hcolor[3]	= alpha;

		trap_R_SetColor( hcolor );
		CG_DrawPicST(x, y, w, h, 0, 0, 1, 1, cgs.media.shellShockViewShader);
		trap_R_SetColor( NULL );
	}
}
#endif //__SHELLSHOCK__

#ifdef __BLOODY_VIEW__
void CG_DrawBloodyView()
{
	vec4_t	hcolor;
	float	val;

	if ((cg.snap->ps.pm_flags & PMF_FOLLOW) == 0 && (cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR))
		return;

	if (cg.snap->ps.stats[STAT_HEALTH] < 100 && cg.snap->ps.stats[STAT_HEALTH] > 0)
	{
		val			= cg.snap->ps.stats[STAT_HEALTH];
		hcolor[0]	= 0.5f;
		hcolor[1]	= 0.0f;
		hcolor[2]	= 0.0f;
		hcolor[3]	= 1.0f - (val / 100.0f);

		if (hcolor[3] > 0.7f)
			hcolor[3] = 0.7f;

		trap_R_SetColor( hcolor );
		CG_DrawPic(0, 0, 640, 480, cgs.media.troubledViewShader );
		trap_R_SetColor( NULL );
//		CG_Printf("health: %i alpha:%f ", cg.predictedPlayerState.stats[STAT_HEALTH], hcolor[3]);
	}
}
#endif //__BLOODY_VIEW__

//bani
void CG_DrawDemoRecording( void ) {
	char status[1024];
	char demostatus[128];
	char wavestatus[128];

	if( !cl_demorecording.integer && !cl_waverecording.integer ) {
		return;
	}

	if( !cg_recording_statusline.integer ) {
		return;
	}

	if( cl_demorecording.integer ) {
		Com_sprintf( demostatus, sizeof( demostatus ), " demo %s: %ik ", cl_demofilename.string, cl_demooffset.integer / 1024 );
	} else {
		strncpy( demostatus, "", sizeof( demostatus ) );
	}

	if( cl_waverecording.integer ) {
		Com_sprintf( wavestatus, sizeof( demostatus ), " audio %s: %ik ", cl_wavefilename.string, cl_waveoffset.integer / 1024 );
	} else {
		strncpy( wavestatus, "", sizeof( wavestatus ) );
	}

	Com_sprintf( status, sizeof( status ), "RECORDING%s%s", demostatus, wavestatus );

	CG_Text_Paint_Ext( 5, cg_recording_statusline.integer, 0.2f, 0.2f, colorWhite, status, 0, 0, 0, /*&cgs.media.limboFont2*/&cgs.media.tahoma30 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __ETE__
/*void CG_DrawFlagCaptureBar(void)
{
	const int numticks = 50, tickwidth = 1*1.95, tickheight = 3*1.95;
	const int tickpadx = 2*1.95, tickpady = 2*1.95;
	const int capwidth = 2;
	const int barwidth = (numticks*tickwidth+tickpadx*2+capwidth*2)*1.95;
	const int barleft = ((1145-barwidth)/2);
	const int barheight = tickheight + tickpady*2;
	const int bartop = 173-barheight;
	const int capleft = barleft+tickpadx;
	const int tickleft = capleft+capwidth, ticktop = bartop+tickpady;
	float percentage = 0.0f;

	//CG_Printf("Capturing flag!!! (% is %i)\n", cg.captureFlagPercent);

	if (cg.captureFlagPercent <= 0)
		return;

	//CG_Printf("Capturing flag!!! (% is %i)\n", cg.captureFlagPercent);

	percentage = cg.captureFlagPercent;

	trap_R_SetColor( colorWhite );

	// Draw background
	CG_DrawPic(barleft, bartop, barwidth, barheight, trap_R_RegisterShader( "gfx/hud/bar_background" ));

	// Draw left cap (backwards)
	CG_DrawPic(tickleft, ticktop, -capwidth, tickheight, trap_R_RegisterShader( "gfx/hud/bar_tick_cap" ));

	// Draw bar
	CG_DrawPic(tickleft, ticktop, tickwidth*percentage, tickheight, trap_R_RegisterShader( "gfx/hud/bar_tick" ));

	// Draw right cap
	CG_DrawPic(tickleft+tickwidth*percentage, ticktop, capwidth, tickheight, trap_R_RegisterShader( "gfx/hud/bar_tick_cap" ));
}*/

void CG_DrawFlagCaptureBar(void)
{
	int flags = 64|128;
	float frac;
	rectDef_t rect;
	qboolean enemyFlag = qfalse;
	qboolean neutralFlag = qfalse;

	if (cg.captureFlagPercent <= 0)
		return;

	if (cg_entities[cg.captureEntityNum].currentState.modelindex == TEAM_FREE)
	{
		neutralFlag = qtrue;
		frac = cg.captureFlagPercent*0.01;
	}
	else if (cg_entities[cg.captureEntityNum].currentState.modelindex != cgs.clientinfo[cg.snap->ps.clientNum].team)
	{
		//frac = cg.captureFlagPercent*0.01;
		frac = (100-cg.captureFlagPercent)*0.01;
		enemyFlag = qtrue;
	}
	else
	{
		frac = (100-cg.captureFlagPercent)*0.01;		
	}

	rect.w = 122;
	rect.h = 10;
	rect.x = ((1123-rect.w)/2);
	rect.y = 173-rect.h;

	// draw the background, then filled bar, then border
	CG_FillRect( rect.x, rect.y, rect.w, rect.h, popBG );
	CG_FilledBar( rect.x, rect.y, rect.w, rect.h, popBlue, NULL, NULL, frac, flags );
	CG_DrawRect_FixedBorder( rect.x, rect.y, rect.w, rect.h, POP_HUD_BORDERSIZE, popBorder );
	
	// draw the bar icon -- UQ1: Put a flag icon here maybe???
	if (neutralFlag)
	{// Capturing a neutral flag...
		CG_DrawPic( rect.x - 40, rect.y-3, POP_HUD_ICONWIDTH*2, POP_HUD_ICONHEIGHT, cgs.media.hudFlagNeutralIcon );
	}
	else if (enemyFlag)
	{// Capturing enemy flag...
		if (cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_ALLIES)
			CG_DrawPic( rect.x - 40, rect.y-3, POP_HUD_ICONWIDTH*2, POP_HUD_ICONHEIGHT, cgs.media.hudFlagAxisIcon );
		else
			CG_DrawPic( rect.x - 40, rect.y-3, POP_HUD_ICONWIDTH*2, POP_HUD_ICONHEIGHT, cgs.media.hudFlagAlliesIcon );
	}
	else
	{// Consolidating our own flag...
		if (cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_ALLIES)
			CG_DrawPic( rect.x - 40, rect.y-3, POP_HUD_ICONWIDTH*2, POP_HUD_ICONHEIGHT, cgs.media.hudFlagAlliesIcon );
		else
			CG_DrawPic( rect.x - 40, rect.y-3, POP_HUD_ICONWIDTH*2, POP_HUD_ICONHEIGHT, cgs.media.hudFlagAxisIcon );
	}

//	else
//	{// UQ1: For spectators later???
//		CG_DrawPic( rect.x - 20, rect.y-3, POP_HUD_ICONWIDTH, POP_HUD_ICONHEIGHT, cgs.media.hudFlagAxisIcon );
//		CG_DrawPic( rect.x - 20, rect.y-3, POP_HUD_ICONWIDTH, POP_HUD_ICONHEIGHT, cgs.media.hudFlagAlliesIcon );
//		CG_DrawPic( rect.x - 20, rect.y-3, POP_HUD_ICONWIDTH, POP_HUD_ICONHEIGHT, cgs.media.hudFlagNeutralIcon );
//	}
}

static int color_selector = 0;
int next_color_update = 0;
qboolean color_forwards = qtrue;
int last_axis_spawn_time = 0;
int last_allies_spawn_time = 0;
int	last_axis_ticket_count = 0;
int	last_allies_ticket_count = 0;
qboolean last_flash = qfalse;
int	next_flash_time = 0;

// Supression percentage bars..

int			next_vischeck[MAX_CLIENTS];
qboolean	currently_visible[MAX_CLIENTS];

extern int Q_TrueRand ( int low, int high );

qboolean CG_CheckClientVisibility ( centity_t *cent )
{
	trace_t		trace;
	vec3_t		start, end, forward, right, up;
	centity_t	*traceEnt = NULL;

	if (next_vischeck[cent->currentState.clientNum] > cg.time)
	{
		return currently_visible[cent->currentState.clientNum];
	}

	next_vischeck[cent->currentState.clientNum] = cg.time + 500 + Q_TrueRand(500, 1000);

	VectorCopy(cg.refdef.vieworg, start);
	start[2]+=48;

	VectorCopy(cent->lerpOrigin, end);
	end[2]+=48;

	CG_Trace( &trace, start, NULL, NULL, end, cg.clientNum, MASK_SHOT );

	traceEnt = &cg_entities[trace.entityNum];

	if (traceEnt == cent)
	{
		currently_visible[cent->currentState.clientNum] = qtrue;
		return qtrue;
	}

	if (VectorDistance(trace.endpos, cent->lerpOrigin) < 128)
	{
		currently_visible[cent->currentState.clientNum] = qtrue;
		return qtrue;
	}

	// Try from right of them...
	AngleVectors( cent->lerpAngles, forward, right, up );
	VectorMA( end, 64, right, end );

	trap_CM_BoxTrace ( &trace, start, end, NULL, NULL, 0, MASK_SHOT );

	if (VectorDistance(trace.endpos, cent->lerpOrigin) < 128)
	{
		currently_visible[cent->currentState.clientNum] = qtrue;
		return qtrue;
	}

	// Try from left of them...
	VectorMA( end, -128, right, end );

	trap_CM_BoxTrace ( &trace, start, end, NULL, NULL, 0, MASK_SHOT );

	if (VectorDistance(trace.endpos, cent->lerpOrigin) < 128)
	{
		currently_visible[cent->currentState.clientNum] = qtrue;
		return qtrue;
	}

	//if( trap_R_inPVS( cg.refdef_current->vieworg, cent->currentState.origin/*cent->lerpOrigin*/ ) )
	//	return qtrue;

	currently_visible[cent->currentState.clientNum] = qfalse;
	return qfalse;
}

void CG_SupressionBarAdjustFrom640( float *x, float *y, float *w, float *h ) {
	// scale for screen sizes
	//*x *= cgs.screenXScale;
	//*y *= cgs.screenYScale;
	*w *= cgs.screenXScale;
	*h *= cgs.screenYScale;
}

void CG_DrawSupresionBars( void )
{// Float a supression bar above their head!
	refEntity_t		ent;
	int				height = 50;
	int				flags = 64|128;
	float			frac;
	rectDef_t		rect;
	int				i;

	memset( &ent, 0, sizeof( ent ) );

	for (i = 0; i < MAX_CLIENTS; i++)
	{// Cycle through them...
		centity_t *cent = &cg_entities[i];
		clientInfo_t	*ci = &cgs.clientinfo[i];

		if (!cent)
			continue;
		
		if (cent->currentState.density <= 0)
			continue;

		//if ( ci->health <= 0 )
		//	continue;

		if (cent->currentState.eFlags & EF_DEAD)
			continue;

		if (cent->currentState.eFlags & EF_NODRAW)
			continue;

		if (cent->currentState.teamNum == TEAM_SPECTATOR)
			continue;

		if (cent->currentState.eType == ET_CORPSE)
			continue;

#ifdef __NPC__
		if( cent->currentState.eType == ET_NPC_CORPSE )
			continue;
#endif //__NPC__

		if (!ci)
			continue;

		// it is possible to see corpses from disconnected players that may
		// not have valid clientinfo
		if (!ci->infoValid)
			return;

		//if (ci->health <= 0)
		//	continue;
		
		VectorCopy( cent->lerpOrigin, ent.origin );
		ent.origin[2] += height;			// DHM - Nerve :: was '48'

		// Account for ducking
		if ( cent->currentState.clientNum == cg.snap->ps.clientNum ) {
			if( cg.snap->ps.pm_flags & PMF_DUCKED )
				ent.origin[2] -= 18;
		} else {
			if( (qboolean)cent->currentState.animMovetype )
				ent.origin[2] -= 18;
		}
	
		// Draw the bar!
		frac = (float)((cent->currentState.density)*0.01);

		rect.w = 28;
		rect.h = 5;

		if (!CG_WorldCoordToScreenCoordFloat(ent.origin, &rect.x, &rect.y))
		{
			continue;
		}

		if (!CG_CheckClientVisibility(cent))
		{
			continue;
		}

		rect.x -= 12;

		// draw the background, then filled bar, then border
		CG_FillRect( rect.x, rect.y, rect.w, rect.h, popBG );
		CG_FilledBar( rect.x, rect.y, rect.w, rect.h, popRed, NULL, NULL, frac, flags );
		CG_DrawRect_FixedBorder( rect.x, rect.y, rect.w, rect.h, POP_HUD_BORDERSIZE, popBorder );
	}
}
#endif //__ETE__

/*
=================
CG_Draw2D
=================
*/
static void CG_Draw2D( void ) {
#ifdef __ETE__
	qboolean flashaxis = qfalse, flashallies = qfalse;
#endif //__ETE__

//	CG_ScreenFade();

	//CG_Printf("CGAME: dl_intensity is %i.\n", cg_entities[cg.clientNum].currentState.dl_intensity);

	// Arnout: no 2d when in esc menu
	// FIXME: do allow for quickchat (bleh)
	// Gordon: Removing for now
/*	if( trap_Key_GetCatcher() & KEYCATCH_UI ) {
		return;
	}*/

	if( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		CG_DrawIntermission();
		return;
	} else {
		if( cgs.dbShowing ) {
			CG_Debriefing_Shutdown();
		}
	}

	if( cg.editingSpeakers ) {
		CG_SpeakerEditorDraw();
		return;
	}

	//bani - #127 - no longer cheat protected, we draw crosshair/reticle in non demoplayback
	if ( cg_draw2D.integer == 0 ) {
		if( cg.demoPlayback ) {
			return;
		}

#ifdef __SHELLSHOCK__
		CG_DrawShellShockView();
#endif //__SHELLSHOCK__

#ifdef __BLOODY_VIEW__
		CG_DrawBloodyView();
#endif //__BLOODY_VIEW__

#ifndef __VEHICLES__
		CG_DrawCrosshair();
#endif
		CG_DrawFlashFade();
		return;
	}

	if( !cg.cameraMode ) {
		CG_DrawFlashBlendBehindHUD();

#ifdef __SHELLSHOCK__
		CG_DrawShellShockView();
#endif //__SHELLSHOCK__

#ifdef __BLOODY_VIEW__
		CG_DrawBloodyView();
#endif //__BLOODY_VIEW__

		if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
			CG_DrawSpectator();
#ifndef __VEHICLES__
			CG_DrawCrosshair();
#endif
			CG_DrawCrosshairNames();

			// NERVE - SMF - we need to do this for spectators as well
			CG_DrawTeamInfo();
		} else {
			// don't draw any status if dead
			if ( cg.snap->ps.stats[STAT_HEALTH] > 0 || (cg.snap->ps.pm_flags & PMF_FOLLOW) ) {

#ifndef __VEHICLES__
				CG_DrawCrosshair();
#endif

				CG_DrawCrosshairNames();

				CG_DrawNoShootIcon();

//				CG_DrawPickupItem();

				if (cg_entities[cg.clientNum].currentState.eFlags & EF_CLOAKED)
					CG_DrawStealthIcon();
			}

			CG_DrawTeamInfo();

			if ( cg_drawStatus.integer ) {
				Menu_PaintAll();
				CG_DrawTimedMenus();
			}
		}

		CG_DrawVote();

		CG_DrawLagometer();
	}

	// don't draw center string if scoreboard is up
	if ( !CG_DrawScoreboard() ) {
		if( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
			rectDef_t rect;

#ifdef __ETE__
			// Using new horizontal HUD.. Draw a border around it...
			// draw the box/bg first, before anything inside it, otherwise we have nasty gray overlay
			CG_FillRect( cg.hud.head[0]-4, cg.hud.head[1]-4, 100, 95, popBG );
			CG_DrawRect_FixedBorder( cg.hud.head[0]-4, cg.hud.head[1]-4, 100, 95, POP_HUD_BORDERSIZE, popBorder );
#endif //__ETE__
			if( cg.snap->ps.stats[STAT_HEALTH] > 0 
#ifdef __VEHICLES__
				&& !(cg.snap->ps.eFlags & EF_VEHICLE)
#endif //__VEHICLES__
				) {
				CG_DrawPlayerStatusHead();
				CG_DrawPlayerStatus();
				CG_DrawPlayerStats();
			} else if ( cg.snap->ps.stats[STAT_HEALTH] > 0) {
				CG_DrawPlayerStatus();
				CG_DrawPlayerStats();
			}

			CG_DrawLivesLeft();

			// Cursor hint
			rect.w = rect.h = 48;
			rect.x = .5f * SCREEN_WIDTH - .5f * rect.w;
			rect.y = 260;
			CG_DrawCursorhint( &rect );

			// Stability bar
			rect.x = 50;
			rect.y = 208;
			rect.w = 10;
			rect.h = 64;

			if (!g_realism.integer) // UQ1: Hide this for realism...
				CG_DrawWeapStability( &rect );

			// Stats Debugging
			CG_DrawStatsDebug();
		}

		if (!cg_paused.integer) {
			CG_DrawUpperRight();
		}

		CG_DrawCenterString();
		CG_DrawPMItems();
		CG_DrawPMItemsBig();

		CG_DrawFollow();

		if (cgs.gametype >= GT_WOLF /* UQ1: not coop & sp */)
			CG_DrawWarmup();

		CG_DrawNotify();

		if ( cg_drawCompass.integer && cg.hud.compass[0] >= 0 ) {
			CG_DrawNewCompass();
		}

		CG_DrawObjectiveInfo();

		CG_DrawSpectatorMessage();

		CG_DrawLimboMessage();
	} else {
		if(cgs.eventHandling != CGAME_EVENT_NONE) {
//			qboolean old = cg.showGameView;

//			cg.showGameView = qfalse;
			// draw cursor
			trap_R_SetColor( NULL );
			CG_DrawPic( cgDC.cursorx-14, cgDC.cursory-14, 32, 32, cgs.media.cursorIcon);
//			cg.showGameView = old;
		}
	}
/*
	{
		vec3_t color;
		VectorSet(color, 250, 100, 1);
		CG_Text_Paint_Ext(518, 178, 0.15, 0.15, color, va("^5[^1Axis^5: ^1%i^5] [^4Allies^5: ^1%i^5]", cgs.redtickets, cgs.bluetickets), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
		//CG_Text_Paint_Ext(100, 120, 0.25, 0.25, color, va("(0.25) ^5[^1Axis^5: ^1%i^5] [^4Allies^5: ^1%i^5]", cgs.redtickets, cgs.bluetickets), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
		//CG_Text_Paint_Ext(100, 140, 0.5, 0.5, color, va("(0.5) ^5[^1Axis^5: ^1%i^5] [^4Allies^5: ^1%i^5]", cgs.redtickets, cgs.bluetickets), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
	}
*/

#ifdef __ETE__
	if (cg.captureFlagPercent > 0 && cg.captureFlagPercent < 100 && cg.capturingFlag)
	{// For OM gametype flag captures...
		float x = 528;//523;
		float y = 148;
		vec3_t color;

		if (cg.captureFlagPercent > 100)
			cg.captureFlagPercent = 100;

		CG_DrawFlagCaptureBar();

		VectorSet(color, 250, 100, 1);

		if (next_color_update < cg.time)
		{
			// Cycle writing color...
			if (color_forwards)
			{
				if (color_selector >= 250)
				{
					color_forwards = qfalse;
					color_selector--;
				}
				else
				{
					color_selector++;
				}
			}
			else
			{
				if (color_selector <= 50)
				{
					color_forwards = qtrue;
					color_selector++;
				}
				else
				{
					color_selector--;
				}
			}

			next_color_update = cg.time + 10;
		}

		color[0] = color_selector;
		color[1] = color_selector;
		color[2] = 1;

		CG_Text_Paint_Ext(x, y, 0.24f, 0.24f, color, va("Capturing..."), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
	}
	else if (cg.captureFlagPercent >= 100 && cg.capturingFlag)
	{// For OM gametype flag captures...
		float x = 538;//533;
		float y = 148;
		vec3_t color;

		if (cg.captureFlagPercent > 100)
			cg.captureFlagPercent = 100;

		CG_DrawFlagCaptureBar();

		VectorSet(color, 250, 250, 0);

		CG_Text_Paint_Ext(x, y, 0.24f, 0.24f, color, va("^3Captured!"), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
	}

	// Firstly we'll want to make the ticket number flash for a team that has just spawned another player...
	if (last_axis_ticket_count != cgs.redtickets)
	{
		if (last_axis_ticket_count > cgs.redtickets)
		{
			last_axis_spawn_time = cg.time;
		}
		last_axis_ticket_count = cgs.redtickets;
	}

	if (last_allies_ticket_count != cgs.bluetickets)
	{
		if (last_allies_ticket_count > cgs.bluetickets)
		{
			last_allies_spawn_time = cg.time;
		}
		last_allies_ticket_count = cgs.bluetickets;
	}

	if (cg.time - last_axis_spawn_time < 5000)
		flashaxis = qtrue;

	if (cg.time - last_allies_spawn_time < 5000)
		flashallies = qtrue;

	if (cgs.gametype == GT_SUPREMACY || cgs.gametype == GT_SUPREMACY_CAMPAIGN)
	{// Draw the ticket numbers...
		float x = 518;//513;
		float y = 158;
		vec3_t color;

		VectorSet(color, 250, 250, 0);

		if (flashaxis && flashallies)
		{// Flash the axis and allied ticket numbers...
			if (next_flash_time < cg.time)
			{
				if (!last_flash)
					last_flash = qtrue;
				else
					last_flash = qfalse;

				next_flash_time = cg.time + 500;
			}

			if (last_flash)
			{
				char *text = va("^5[^1Axis^5: ^1%i^5] [^4Allies^5: ^1%i^5]", cgs.redtickets, cgs.bluetickets);
				CG_Text_Paint_Ext(x, y, 0.15f, 0.15f, color, text, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
			}
			else
			{
				char *text = va("^5[^1Axis^5: ^7%i^5] [^4Allies^5: ^7%i^5]", cgs.redtickets, cgs.bluetickets);
				CG_Text_Paint_Ext(x, y, 0.15f, 0.15f, color, text, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
			}
		}
		else if (flashaxis)
		{// Flash the axis ticket number...
			if (next_flash_time < cg.time)
			{
				if (!last_flash)
					last_flash = qtrue;
				else
					last_flash = qfalse;

				next_flash_time = cg.time + 500;
			}

			if (last_flash)
			{
				char *text = va("^5[^1Axis^5: ^1%i^5] [^4Allies^5: ^7%i^5]", cgs.redtickets, cgs.bluetickets);
				CG_Text_Paint_Ext(x, y, 0.15f, 0.15f, color, text, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
			}
			else
			{
				char *text = va("^5[^1Axis^5: ^7%i^5] [^4Allies^5: ^7%i^5]", cgs.redtickets, cgs.bluetickets);
				CG_Text_Paint_Ext(x, y, 0.15f, 0.15f, color, text, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
			}
		}
		else if (flashallies)
		{// Flash the allied ticket number...
			if (next_flash_time < cg.time)
			{
				if (!last_flash)
					last_flash = qtrue;
				else
					last_flash = qfalse;

				next_flash_time = cg.time + 500;
			}

			if (last_flash)
			{
				char *text = va("^5[^1Axis^5: ^7%i^5] [^4Allies^5: ^1%i^5]", cgs.redtickets, cgs.bluetickets);
				CG_Text_Paint_Ext(x, y, 0.15f, 0.15f, color, text, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
			}
			else
			{
				char *text = va("^5[^1Axis^5: ^7%i^5] [^4Allies^5: ^7%i^5]", cgs.redtickets, cgs.bluetickets);
				CG_Text_Paint_Ext(x, y, 0.15f, 0.15f, color, text, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
			}
		}
		else
		{// Nothing special happening...
			char *text = va("^5[^1Axis^5: ^7%i^5] [^4Allies^5: ^7%i^5]", cgs.redtickets, cgs.bluetickets);
			CG_Text_Paint_Ext(x, y, 0.15f, 0.15f, color, text, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.tahoma16 );
		}
	}

	// Bot coverspot distance percentage bars..
	CG_DrawSupresionBars();

#endif //__ETE__

	if( cg.showFireteamMenu ) {
		CG_Fireteams_Draw();
	}

	// Info overlays
	CG_DrawOverlays();

	// OSP - window updates
	CG_windowDraw();

	// Ridah, draw flash blends now
	CG_DrawFlashBlend();

	CG_DrawDemoRecording();
}

// NERVE - SMF
void CG_StartShakeCamera( float p ) {
	cg.cameraShakeScale = p;

	cg.cameraShakeLength = 1000 * (p*p);
	cg.cameraShakeTime = cg.time + cg.cameraShakeLength;
	cg.cameraShakePhase = crandom()*M_PI; // start chain in random dir
}

void CG_ShakeCamera() {
	float x, val;

	if ( cg.time > cg.cameraShakeTime ) {
		cg.cameraShakeScale = 0; // JPW NERVE all pending explosions resolved, so reset shakescale
		return;
	}
	
	// JPW NERVE starts at 1, approaches 0 over time
	x = (cg.cameraShakeTime - cg.time) / cg.cameraShakeLength;

	// ydnar: move the camera
	#if 0
		// up/down
		val = sin(M_PI * 8 * x + cg.cameraShakePhase) * x * 18.0f * cg.cameraShakeScale;
		cg.refdefViewAngles[0] += val; 

		// left/right
		val = sin(M_PI * 15 * x + cg.cameraShakePhase) * x * 16.0f * cg.cameraShakeScale;
		cg.refdefViewAngles[1] += val;
	#else
		// move
		val = sin( M_PI * 7 * x + cg.cameraShakePhase ) * x * 4.0f * cg.cameraShakeScale;
		cg.refdef.vieworg[ 2 ] += val;
		val = sin( M_PI * 13 * x + cg.cameraShakePhase ) * x * 4.0f * cg.cameraShakeScale;
		cg.refdef.vieworg[ 1 ] += val;
		val = cos( M_PI * 17 * x + cg.cameraShakePhase ) * x * 4.0f * cg.cameraShakeScale;
		cg.refdef.vieworg[ 0 ] += val;
	#endif

	AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
}
// -NERVE - SMF

void CG_DrawMiscGamemodels( void ) {
	int i, j;
	refEntity_t ent;
	int drawn = 0;

	memset( &ent, 0, sizeof( ent ) );

	ent.reType = RT_MODEL;
	ent.nonNormalizedAxes =	qtrue;
	
	// ydnar: static gamemodels don't project shadows
	ent.renderfx = RF_NOSHADOW;
	
	for( i = 0; i < cg.numMiscGameModels; i++ ) {
		if( cgs.miscGameModels[i].radius ) {
			if( CG_CullPointAndRadius( cgs.miscGameModels[i].org, cgs.miscGameModels[i].radius ) ) {
 				continue;
			}
		}

		if( !trap_R_inPVS( cg.refdef_current->vieworg, cgs.miscGameModels[i].org ) ) {
			continue;
		}

		VectorCopy( cgs.miscGameModels[i].org, ent.origin );
		VectorCopy( cgs.miscGameModels[i].org, ent.oldorigin );
		VectorCopy( cgs.miscGameModels[i].org, ent.lightingOrigin );

/*		{
			vec3_t v;
			vec3_t vu = { 0.f, 0.f, 1.f };
			vec3_t vl = { 0.f, 1.f, 0.f };
			vec3_t vf = { 1.f, 0.f, 0.f };

			VectorCopy( cgs.miscGameModels[i].org, v );
			VectorMA( v, cgs.miscGameModels[i].radius, vu, v );
			CG_RailTrail2( NULL, cgs.miscGameModels[i].org, v );

			VectorCopy( cgs.miscGameModels[i].org, v );
			VectorMA( v, cgs.miscGameModels[i].radius, vf, v );
			CG_RailTrail2( NULL, cgs.miscGameModels[i].org, v );

			VectorCopy( cgs.miscGameModels[i].org, v );
			VectorMA( v, cgs.miscGameModels[i].radius, vl, v );
			CG_RailTrail2( NULL, cgs.miscGameModels[i].org, v );

			VectorCopy( cgs.miscGameModels[i].org, v );
			VectorMA( v, -cgs.miscGameModels[i].radius, vu, v );
			CG_RailTrail2( NULL, cgs.miscGameModels[i].org, v );

			VectorCopy( cgs.miscGameModels[i].org, v );
			VectorMA( v, -cgs.miscGameModels[i].radius, vf, v );
			CG_RailTrail2( NULL, cgs.miscGameModels[i].org, v );

			VectorCopy( cgs.miscGameModels[i].org, v );
			VectorMA( v, -cgs.miscGameModels[i].radius, vl, v );
			CG_RailTrail2( NULL, cgs.miscGameModels[i].org, v );
		}*/

		for( j = 0; j < 3; j++ ) {
			VectorCopy( cgs.miscGameModels[i].axes[j], ent.axis[j] );
		}
		ent.hModel = cgs.miscGameModels[i].model;
		
		trap_R_AddRefEntityToScene( &ent );

		drawn++;
	}
}

/*
=====================
CG_DrawActive

Perform all drawing needed to completely fill the screen
=====================
*/

extern void CG_UQ_DrawFogShaders( void );

void CG_DrawActive( stereoFrame_t stereoView, qboolean sniping ) {
	float		separation;
	vec3_t		baseOrg;

	// optionally draw the info screen instead
	if ( !cg.snap ) {
		if (!sniping)
			CG_DrawInformation( qfalse );
		return;
	}

	// optionally draw the tournement scoreboard instead
	/*if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR &&
		( cg.snap->ps.pm_flags & PMF_SCOREBOARD ) ) {
		CG_DrawTourneyScoreboard();
		return;
	}*/

	switch ( stereoView ) {
	case STEREO_CENTER:
		separation = 0;
		break;
	case STEREO_LEFT:
		separation = -cg_stereoSeparation.value / 2;
		break;
	case STEREO_RIGHT:
		separation = cg_stereoSeparation.value / 2;
		break;
	default:
		separation = 0;
		CG_Error( "CG_DrawActive: Undefined stereoView" );
	}


	// clear around the rendered view if sized down
	if (!sniping)
		CG_TileClear();

	// offset vieworg appropriately if we're doing stereo separation
	VectorCopy( cg.refdef_current->vieworg, baseOrg );
	if ( separation != 0 ) {
		VectorMA( cg.refdef_current->vieworg, -separation, cg.refdef_current->viewaxis[1], cg.refdef_current->vieworg );
	}

	cg.refdef_current->glfog.registered = 0;	// make sure it doesn't use fog from another scene

#ifdef __SHELLSHOCK__
#ifdef __SHELLSHOCK_FOG__
	CG_SetupFogParms(&cg.refdef_current->glfog);
#endif //__SHELLSHOCK_FOG__
#endif //__SHELLSHOCK__

	if (!sniping)
		CG_ActivateLimboMenu();

//	if( cgs.ccCurrentCamObjective == -1 ) {
//		if( cg.showGameView ) {
//			CG_FillRect( 0, 0, 640, 480, colorBlack );
//			CG_LimboPanel_Draw();
//			return;
//		}
//	}

	if ( cg.showGameView ) {
 		float x, y, w, h;
 		x = LIMBO_3D_X;
 		y = LIMBO_3D_Y;
 		w = LIMBO_3D_W;
 		h = LIMBO_3D_H;

 		CG_AdjustFrom640( &x, &y, &w, &h );

 		cg.refdef_current->x = x;
 		cg.refdef_current->y = y;
 		cg.refdef_current->width = w;
 		cg.refdef_current->height = h;

 		CG_Letterbox( (LIMBO_3D_W/640.f)*100, (LIMBO_3D_H/480.f)*100, qfalse );
	}

	CG_ShakeCamera();		// NERVE - SMF

	// Gordon
	CG_PB_RenderPolyBuffers();

	// Gordon
	CG_DrawMiscGamemodels();

	if( !(cg.limboEndCinematicTime > cg.time && cg.showGameView) ) {
		trap_R_RenderScene( cg.refdef_current );
	}

	// restore original viewpoint if running stereo
	if ( separation != 0 ) {
		VectorCopy( baseOrg, cg.refdef_current->vieworg );
	}

#ifdef __UQ_FOG_SHADER__TEST__
	if (using_uq_fog)
		CG_UQ_DrawFogShaders();
#endif //__UQ_FOG_SHADER__TEST__

	if (sniping)
		return;

	if( !cg.showGameView ) {
		// draw status bar and other floating elements
		CG_Draw2D();
	} else {
		CG_LimboPanel_Draw();
	}
}
