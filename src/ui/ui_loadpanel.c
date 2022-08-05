#include "ui_local.h"
#include "ui_shared.h"

qboolean	bg_loadscreeninited = qfalse;
fontInfo_t	bg_loadscreenfont1;
fontInfo_t	bg_loadscreenfont2;

void UI_LoadPanel_RenderHeaderText( panel_button_t* button );
void UI_LoadPanel_RenderLoadingText( panel_button_t* button );
void UI_LoadPanel_RenderPercentageMeter( panel_button_t* button );

// panel_button_text_t FONTNAME = { SCALEX, SCALEY, COLOUR, STYLE, FONT };

panel_button_text_t missiondescriptionTxt = {
	0.20f, 0.20f,
	{ 0.9f, 0.9f, 0.9f, 1.f },
	0, 0,
	&bg_loadscreenfont2,
};

panel_button_text_t missiondescriptionHeaderTxt = {
	0.2f, 0.2f,
	{ 0.9f, 0.9f, 0.9f, 1.f },
	 0,0,
	&bg_loadscreenfont2,
};

panel_button_text_t campaignpheaderTxt = {
	0.2f, 0.2f,
	{ 0.0f, 0.0f, 0.0f, 0.6f },
	0, 0,
	&bg_loadscreenfont2,
};

panel_button_text_t campaignpTxt = {
	0.30f, 0.30f,
	{ 0.0f, 0.0f, 0.0f, 0.6f },
	0, 0,
	&bg_loadscreenfont2,
};

//new

panel_button_t loadScreenMap = {
	"gfx/loading/camp_worldmap",
	NULL,
	{ 385, 24, 224, 86 },	// shouldn't this be square?? // Gordon: no, the map is actually WIDER that tall, which makes it even worse...
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/	
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t loadScreenMain = {
	"gfx/loading/camp_main",
	NULL,
	{ 0, 220, 440, 260 },	// shouldn't this be square??
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/	
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t loadScreenFull = {
	"gfx/loading/camp_full",
	NULL,
	{ 0, 0, 640, 480 },	// shouldn't this be square??
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/	
	BG_PanelButtonsRender_Img,
	NULL,
};
panel_button_t loadScreenBack = {
	"gfx/loading/camp_side",
	NULL,
	{ 440, 0, 200, 480 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/	
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t loadingPanelText = {
	NULL,
	NULL,
	{ 392, 42, 228, 192 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&missiondescriptionTxt,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	UI_LoadPanel_RenderLoadingText,
	NULL,
};


panel_button_t campaignPanelText = {
	NULL,
	NULL, //"CONNECTING...",
	{ 385, 145, 235, 220 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&campaignpTxt,			/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	UI_LoadPanel_RenderHeaderText,
	NULL,
};




panel_button_t* loadpanelButtons[] = {
//	&loadScreenMap, &loadScreenBack, &loadScreenMain,
	&loadScreenFull, //&loadScreenMap, 
	&loadingPanelText, /*&loadingPanelHeaderText,*/
	/*&campaignheaderPanelText,*/ /*&campaignPanelText,*/

	NULL,
};

/*
================
CG_DrawConnectScreen
================
*/
static qboolean connect_ownerdraw;
void UI_DrawLoadPanel( qboolean forcerefresh, qboolean ownerdraw, qboolean uihack ) {
	static qboolean inside = qfalse;

	if( inside ) {
		if( !uihack && trap_Cvar_VariableValue( "ui_connecting" ) ) {
			trap_Cvar_Set( "ui_connecting", "0" );
		}
		return;
	}

	connect_ownerdraw = ownerdraw;

	inside = qtrue;

	if( !bg_loadscreeninited ) {
		//trap_R_RegisterFont( "ariblk", 27, &bg_loadscreenfont1 );
		//trap_R_RegisterFont( "courbd", 30, &bg_loadscreenfont2 );

		//trap_R_RegisterFont( "tahoma", 16, &bg_loadscreenfont1 );
		//trap_R_RegisterFont( "tahomabd", 24, &bg_loadscreenfont2 );

		trap_R_RegisterFont( "tahoma", 36, &bg_loadscreenfont1 );
		trap_R_RegisterFont( "tahomabd", 36, &bg_loadscreenfont2 );
		
		//trap_R_RegisterFont( "tahoma", 30, &bg_loadscreenfont1 );
//		trap_R_RegisterFont( "tahoma", 16, &bg_loadscreenfont1 );
//		trap_R_RegisterFont( "tahomabd", 24, &bg_loadscreenfont2 );

		BG_PanelButtonsSetup( loadpanelButtons );

		bg_loadscreeninited = qtrue;
	}

	BG_PanelButtonsRender( loadpanelButtons );

	if( forcerefresh ) {
		//trap_UpdateScreen();
	}

	if( !uihack && trap_Cvar_VariableValue( "ui_connecting" ) ) {
		trap_Cvar_Set( "ui_connecting", "0" );
	}

	inside = qfalse;
}

#define STARTANGLE 40
void UI_LoadPanel_RenderPercentageMeter( panel_button_t* button ) {
	float hunkfrac;
	float w, h;
	vec2_t org;
	polyVert_t verts[4];

	org[0] =	button->rect.x;
	org[1] =	button->rect.y;
	w =			button->rect.w;
	h =			button->rect.h;

	hunkfrac = 0.f;
	AdjustFrom640( &org[0], &org[1], &w, &h );
	SetupRotatedThing( verts, org, w, h, DEG2RAD((180-STARTANGLE) - ((180-(2*STARTANGLE)) * hunkfrac)) );

	trap_R_Add2dPolys( verts, 4, button->hShaderNormal );
}
/*
void UI_LoadPanel_RenderCampaignNameText( panel_button_t* button ) {
	uiClientState_t	cstate;
	char *s;

	trap_GetClientState( &cstate );

	s = Q_strupr( cstate.servername );

	Text_Paint_Ext( button->rect.x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, s, 0, 14, 0, button->font->font );
}*/

void MiniAngleToAxis( vec_t angle, vec2_t axes[2] ) {
	axes[0][0] = (vec_t)sin( -angle );
	axes[0][1] = -(vec_t)cos( -angle );

	axes[1][0] = -axes[0][1];
	axes[1][1] = axes[0][0];
}

void SetupRotatedThing( polyVert_t* verts, vec2_t org, float w, float h, vec_t angle ) {
	vec2_t axes[2];

	MiniAngleToAxis( angle, axes );

	verts[0].xyz[0] = org[0] - (w * 0.5f) * axes[0][0];
	verts[0].xyz[1] = org[1] - (w * 0.5f) * axes[0][1];
	verts[0].xyz[2] = 0;
	verts[0].st[0] = 0;
	verts[0].st[1] = 1;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;

	verts[1].xyz[0] = verts[0].xyz[0] + w * axes[0][0];
	verts[1].xyz[1] = verts[0].xyz[1] + w * axes[0][1];
	verts[1].xyz[2] = 0;
	verts[1].st[0] = 1;
	verts[1].st[1] = 1;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;

	verts[2].xyz[0] = verts[1].xyz[0] + h * axes[1][0];
	verts[2].xyz[1] = verts[1].xyz[1] + h * axes[1][1];
	verts[2].xyz[2] = 0;
	verts[2].st[0] = 1;
	verts[2].st[1] = 0;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;

	verts[3].xyz[0] = verts[2].xyz[0] - w * axes[0][0];
	verts[3].xyz[1] = verts[2].xyz[1] - w * axes[0][1];
	verts[3].xyz[2] = 0;
	verts[3].st[0] = 0;
	verts[3].st[1] = 0;
	verts[3].modulate[0] = 255;
	verts[3].modulate[1] = 255;
	verts[3].modulate[2] = 255;
	verts[3].modulate[3] = 255;
}

void UI_LoadPanel_RenderHeaderText( panel_button_t* button ) {
	uiClientState_t	cstate;
	char			downloadName[MAX_INFO_VALUE];

	trap_GetClientState( &cstate );

	trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName, sizeof(downloadName) );

	if( ( cstate.connState == CA_DISCONNECTED || cstate.connState == CA_CONNECTED ) && *downloadName ) {
		button->text = "DOWNLOADING...";
	} else {
		button->text = "CONNECTING...";
	}

	BG_PanelButtonsRender_Text( button );
}

//
// UQ1: START: Percent bar added for downloads...
//

/*
================
CG_AdjustFrom640

Adjusted for resolution and screen aspect ratio
================
*/
void CG_AdjustFrom640( float *x, float *y, float *w, float *h ) {
	// scale for screen sizes
	
	*x *= uiInfo.uiDC.glconfig.vidWidth / 640.0;
	*y *= uiInfo.uiDC.glconfig.vidHeight / 480.0;
	*w *= uiInfo.uiDC.glconfig.vidWidth / 640.0;
	*h *= uiInfo.uiDC.glconfig.vidHeight / 480.0;
}

/*
================
CG_FillRect

Coordinates are 640*480 virtual values
=================
*/
void CG_FillRect( float x, float y, float width, float height, const float *color ) {
	trap_R_SetColor( color );

	CG_AdjustFrom640( &x, &y, &width, &height );
	trap_R_DrawStretchPic( x, y, width, height, 0, 0, 0, 1, uiInfo.uiDC.whiteShader );

	trap_R_SetColor( NULL );
}

// TODO: these flags will be shared, but it was easier to work on stuff if I wasn't changing header files a lot
#define BAR_LEFT		0x0001
#define BAR_CENTER		0x0002
#define BAR_VERT		0x0004
#define BAR_NOHUDALPHA	0x0008
#define BAR_BG			0x0010
// different spacing modes for use w/ BAR_BG
#define BAR_BGSPACING_X0Y5	0x0020
#define BAR_BGSPACING_X0Y0	0x0040

#define BAR_LERP_COLOR	0x0100

#define BAR_BORDERSIZE 2

void CG_FilledBar(float x, float y, float w, float h, float *startColor, float *endColor, const float *bgColor, float frac, int flags) {
	vec4_t	backgroundcolor = {1, 1, 1, 0.25f}, colorAtPos;	// colorAtPos is the lerped color if necessary
	int indent = BAR_BORDERSIZE;

	if( frac > 1 ) {
		frac = 1.f;
	}
	if( frac < 0 ) {
		frac = 0;
	}

	if((flags&BAR_BG) && bgColor) {	// BAR_BG set, and color specified, use specified bg color
		Vector4Copy(bgColor, backgroundcolor);
	}

	if(flags&BAR_LERP_COLOR) {
		Vector4Average(startColor, endColor, frac, colorAtPos);
	}

	// background
	if((flags&BAR_BG)) {
		// draw background at full size and shrink the remaining box to fit inside with a border.  (alternate border may be specified by a BAR_BGSPACING_xx)
		CG_FillRect (	x,
						y,
						w,
						h,
						backgroundcolor );

		if(flags&BAR_BGSPACING_X0Y0) {			// fill the whole box (no border)

		} else if(flags&BAR_BGSPACING_X0Y5) {	// spacing created for weapon heat
			indent*=3;
			y+=indent;
			h-=(2*indent);

		} else {								// default spacing of 2 units on each side
			x+=indent;
			y+=indent;
			w-=(2*indent);
			h-=(2*indent);
		}
	}


	// adjust for horiz/vertical and draw the fractional box
	if(flags&BAR_VERT) {
		if(flags&BAR_LEFT) {	// TODO: remember to swap colors on the ends here
			y+=(h*(1-frac));
		} else if (flags&BAR_CENTER) {
			y+=(h*(1-frac)/2);
		}

		if(flags&BAR_LERP_COLOR) {
			CG_FillRect ( x, y, w, h * frac, colorAtPos );
		} else {
//			CG_FillRectGradient ( x, y, w, h * frac, startColor, endColor, 0 );
			CG_FillRect ( x, y, w, h * frac, startColor );
		}

	} else {

		if(flags&BAR_LEFT) {	// TODO: remember to swap colors on the ends here
			x+=(w*(1-frac));
		} else if (flags&BAR_CENTER) {
			x+=(w*(1-frac)/2);
		}

		if(flags&BAR_LERP_COLOR) {
			CG_FillRect ( x, y, w*frac, h, colorAtPos );
		} else {
//			CG_FillRectGradient ( x, y, w * frac, h, startColor, endColor, 0 );
			CG_FillRect ( x, y, w*frac, h, startColor );
		}
	}

}

/*
================
CG_DrawSides

Coords are virtual 640x480
================
*/
void CG_DrawSides( float x, float y, float w, float h, float size ) {
	CG_AdjustFrom640( &x, &y, &w, &h );
	size *= (uiInfo.uiDC.glconfig.vidWidth / 640.0);
	trap_R_DrawStretchPic( x, y, size, h, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
	trap_R_DrawStretchPic( x + w - size, y, size, h, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}

void CG_DrawTopBottom( float x, float y, float w, float h, float size ) {
	CG_AdjustFrom640( &x, &y, &w, &h );
	size *= (uiInfo.uiDC.glconfig.vidHeight / 480.0);
	trap_R_DrawStretchPic( x, y, w, size, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
	trap_R_DrawStretchPic( x, y + h - size, w, size, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}

void CG_DrawSides_NoScale( float x, float y, float w, float h, float size ) {
	CG_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, size, h, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
	trap_R_DrawStretchPic( x + w - size, y, size, h, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}

void CG_DrawTopBottom_NoScale( float x, float y, float w, float h, float size ) {
	CG_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, size, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
	trap_R_DrawStretchPic( x, y + h - size, w, size, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}

// CHRUKER: b076 - Scoreboard background had black lines drawn twice
void CG_DrawBottom_NoScale( float x, float y, float w, float h, float size ) {
	CG_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y + h - size, w, size, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}

/*
================
UI_DrawRect

Coordinates are 640*480 virtual values
=================
*/
void CG_DrawRect( float x, float y, float width, float height, float size, const float *color ) {
	trap_R_SetColor( color );

	CG_DrawTopBottom(x, y, width, height, size);
	CG_DrawSides(x, y, width, height, size);

	trap_R_SetColor( NULL );
}

void CG_DrawRect_FixedBorder( float x, float y, float width, float height, int border, const float *color ) {
	trap_R_SetColor( color );

	CG_DrawTopBottom_NoScale( x, y, width, height, border );
	CG_DrawSides_NoScale( x, y, width, height, border );

	trap_R_SetColor( NULL );
}

fontInfo_t		tahoma16;
/*qboolean		tahoma16registerred = qfalse;

void UI_RegisterTahoma16Font ( void )
{
	if (!tahoma16registerred)
	{
		trap_R_RegisterFont( "tahoma", 16, &tahoma16 );
		tahoma16registerred = qtrue;
	}
}*/

#define POP_HUD_BORDERSIZE 1

float aw_percent_complete = 0.0f;
int aw_seconds_left = 0;
char task_string1[255];
char task_string2[255];
char task_string3[255];
char last_node_added_string[255];

void UQ1_Download_DrawProgressBar ( void )
{
	int				flags = 64|128;
	float			frac;
	rectDef_t		rect;
	int				total_seconds_left = 0;
	int				seconds_left = 0;
	int				minutes_left = 0;
	qboolean		estimating = qfalse;

	//UI_RegisterTahoma16Font();

	if (aw_percent_complete > 100.0f)
	{// UQ: It can happen... Somehow... LOL!
		aw_percent_complete = 100.0f;
	}

	if (aw_seconds_left == 0)
	{
		estimating = qtrue;
	}
	else
	{
		total_seconds_left = aw_seconds_left;
		minutes_left = total_seconds_left/60;
		seconds_left = total_seconds_left-(minutes_left*60);
	}

	// Draw the bar!
	frac = (float)((aw_percent_complete)*0.01);

	rect.w = 305;//300;//500;
	rect.h = 30;
	
	rect.x = 39;//69;
	rect.y = 169;//369;

	// draw the background, then filled bar, then border
	CG_FillRect( rect.x, rect.y, rect.w, rect.h, popBG );
	CG_FilledBar( rect.x, rect.y, rect.w, rect.h, popRed, NULL, NULL, frac, flags );
	CG_DrawRect_FixedBorder( rect.x, rect.y, rect.w, rect.h, POP_HUD_BORDERSIZE, popBorder );

	//Text_Paint_Ext((rect.x), (rect.y + (rect.h*0.5) - 100), 0.22, 0.22, colorWhite, va("^3DOWNLOADING^5 - Please wait..."), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &tahoma16 );
	//Text_Paint_Ext((rect.x), (rect.y + (rect.h*0.5) - 60), 0.22, 0.22, colorWhite, task_string1, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &tahoma16 );
	//Text_Paint_Ext((rect.x), (rect.y + (rect.h*0.5) - 40), 0.22, 0.22, colorWhite, task_string2, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &tahoma16 );
	//Text_Paint_Ext((rect.x), (rect.y + (rect.h*0.5) - 20), 0.22, 0.22, colorWhite, task_string3, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &tahoma16 );
	Text_Paint_Ext((rect.x), (rect.y + (rect.h*0.5) - 40), 0.22, 0.22, colorWhite, va("^3DOWNLOADING^5 - Please wait..."), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &tahoma16 );
	Text_Paint_Ext((rect.x), (rect.y + (rect.h*0.5) - 20), 0.16, 0.16/*0.22, 0.22*/, colorWhite, task_string3, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &tahoma16 );
	Text_Paint_Ext((rect.x + (rect.w*0.5) - 35), (rect.y + (rect.h*0.5) + 8), 0.40, 0.40, colorWhite, va("^7%.2f%%", aw_percent_complete), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &tahoma16 );
	
	if (!estimating /*&& aw_percent_complete > 1.0f*/)
		Text_Paint_Ext((rect.x + 50), (rect.y + (rect.h*0.5) + 30), 0.16, 0.16, colorWhite, va("^3%i ^5minutes ^3%i ^5seconds remaining (estimated)", minutes_left, seconds_left), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &tahoma16 );
	else
		Text_Paint_Ext((rect.x + 50), (rect.y + (rect.h*0.5) + 30), 0.16, 0.16, colorWhite, va("^5    ... estimating time remaining ..."), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &tahoma16 );

	Text_Paint_Ext((rect.x + 70), (rect.y + (rect.h*0.5) + 50), 0.22, 0.22, colorWhite, last_node_added_string, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &tahoma16 );

	trap_R_SetColor( NULL );
}

//
// UQ1: END: Percent bar added for downloads...
//


#define ESTIMATES 80
const char *UI_DownloadInfo( const char *downloadName )
{
	static char dlText[]	= "^3Downloading^5:";
	static char etaText[]	= "^3Estimated time left^5:";
	static char xferText[]	= "^3Transfer rate^5:";
	//static char hunkmegs_info[] = "^7com_hunkmegs^5 must be higher then ^7127^5.\n\n^5After the download, type\n^3/set com_hunkmegs 128^5\n^5at your console (^7~ key^5)\n^5and restart ET...";
	static char hunkmegs_info[] = "";
	static int	tleEstimates[ESTIMATES] = { 60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
											60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
											60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
											60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60 };
	static int	tleIndex = 0;

	char dlSizeBuf[64], totalSizeBuf[64], xferRateBuf[64], dlTimeBuf[64];
	int downloadSize, downloadCount, downloadTime;
	int xferRate;
	const char *s, *ds;

	downloadSize = trap_Cvar_VariableValue( "cl_downloadSize" );
	downloadCount = trap_Cvar_VariableValue( "cl_downloadCount" );
	downloadTime = trap_Cvar_VariableValue( "cl_downloadTime" );

	if( downloadSize > 0 ) {
		ds = va( "%s (%d%%)", downloadName, (int)( (float)downloadCount * 100.0f / (float)downloadSize ) );
	} else {
		ds = downloadName;
	}
/*
char task_string1[255];
char task_string2[255];
char task_string3[255];
char last_node_added_string[255];
*/
	strcpy(task_string3, va("%s %s", dlText, downloadName));

	UI_ReadableSize( dlSizeBuf,		sizeof dlSizeBuf,		downloadCount );
	UI_ReadableSize( totalSizeBuf,	sizeof totalSizeBuf,	downloadSize );

	if( downloadCount < 4096 || !downloadTime ) {
		s = va( "%s\n %s\n%s\n\n%s\n estimating...\n\n%s\n\n%s copied\n\n%s", dlText, ds, totalSizeBuf,
												etaText,
												xferText,
												dlSizeBuf, hunkmegs_info );

		UQ1_Download_DrawProgressBar();
		return s;
	} else {
		if( ( uiInfo.uiDC.realTime - downloadTime ) / 1000 ) {
			xferRate = downloadCount / ( ( uiInfo.uiDC.realTime - downloadTime ) / 1000 );
		} else {
			xferRate = 0;
		}
		UI_ReadableSize( xferRateBuf, sizeof xferRateBuf, xferRate );

		// Extrapolate estimated completion time
		if( downloadSize && xferRate ) {
			int n = downloadSize / xferRate; // estimated time for entire d/l in secs
			int timeleft = 0, i;

			// We do it in K (/1024) because we'd overflow around 4MB
			tleEstimates[ tleIndex ] = (n - (((downloadCount/1024) * n) / (downloadSize/1024)));
			tleIndex++;
			if( tleIndex >= ESTIMATES )
				tleIndex = 0;

			for( i = 0; i<ESTIMATES; i++ )
				timeleft += tleEstimates[ i ];

			timeleft /= ESTIMATES;

			UI_PrintTime( dlTimeBuf, sizeof dlTimeBuf, timeleft );

			aw_seconds_left = timeleft;
			aw_percent_complete = (float)downloadCount * 100.0f / (float)downloadSize;//(downloadCount / downloadSize) * 100;
			strcpy(last_node_added_string, va("%s %s/sec", xferText, xferRateBuf));
		} else {
			dlTimeBuf[0] = '\0';
			aw_seconds_left = 0;
			aw_percent_complete = 0;
		}

		if( xferRate ) {
			s = va( "%s\n %s\n%s\n\n%s\n %s\n\n%s\n %s/sec\n\n%s copied\n\n%s", dlText, ds, totalSizeBuf,
													etaText, dlTimeBuf,
													xferText, xferRateBuf,
													dlSizeBuf, hunkmegs_info );
		} else {
			if( downloadSize ) {
				s = va( "%s\n %s\n%s\n\n%s\n estimating...\n\n%s\n\n%s copied\n\n%s", dlText, ds, totalSizeBuf,
																	   etaText,
																	   xferText,
																	   dlSizeBuf, hunkmegs_info );
			} else {
				s = va( "%s\n %s\n\n%s\n estimating...\n\n%s\n\n%s copied\n\n%s", dlText, ds,
																 etaText,
																 xferText,
																 dlSizeBuf, hunkmegs_info );
			}
		}

		UQ1_Download_DrawProgressBar();

		return s;
	}

	UQ1_Download_DrawProgressBar();
    return "";
}

void UI_LoadPanel_RenderLoadingText( panel_button_t* button )
{
	uiClientState_t	cstate;
	char			downloadName[MAX_INFO_VALUE];
	char			buff[2560];
	static connstate_t	lastConnState;
	static char			lastLoadingText[MAX_INFO_VALUE];
	char			*p, *s = "";
	float			y;

	tahoma16 = *button->font->font;

	// UQ1: Added. Force com_hunkmegs to 256!
	/*if ((int)trap_Cvar_VariableValue( "com_hunkmegs" ) < 256)
	{
		trap_Cvar_Set( "com_hunkmegs", "256" );
		//trap_Cvar_Set( "fs_game", uiInfo.modList[uiInfo.modIndex].modName);
		//trap_Cmd_ExecuteText( EXEC_NOW, "vid_restart;" );
		trap_Cmd_ExecuteText( EXEC_NOW, "reconnect\n" );
		return;
	}*/

	// UQ1: Added. Force com_hunkmegs to 256!
	/*if ((int)trap_Cvar_VariableValue( "com_hunkmegs" ) < 256 && (int)trap_Cvar_VariableValue( "com_hunkmegs" ) != 0)
	{
		//Com_Printf( "Restarting to set com_hunkmegs higher...\n" );
		//trap_Cvar_Set( "com_hunkmegs", "256" );
		//trap_Cmd_ExecuteText( EXEC_NOW, "reconnect\n" );
		trap_Cmd_ExecuteText( EXEC_NOW, "exec hunkmegs.cfg\n" );
		//return;
	}*/

	/*
	if ((int)trap_Cvar_VariableValue( "com_hunkmegs" ) < 128)
	{
		trap_Error( va("com_hunkmegs must be >= 128.\nType /set com_hunkmegs 128 at your console (~ key)") );
		return;
	}*/

	trap_GetClientState( &cstate );

	Com_sprintf( buff, sizeof(buff), "^3Connecting to:\n ^7%s^*\n\n^5%s", cstate.servername, Info_ValueForKey( cstate.updateInfoString, "motd" ) );

	//Com_sprintf( buff, sizeof(buff), "%s^*", cstate.servername, Info_ValueForKey( cstate.updateInfoString, "motd" ) );

	trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName, sizeof(downloadName) );

	if ( lastConnState > cstate.connState ) {
		lastLoadingText[0] = '\0';
	}
	lastConnState = cstate.connState;

	if( !connect_ownerdraw ) {
		if( !trap_Cvar_VariableValue( "ui_connecting" ) ) {
			switch( cstate.connState ) {
			case CA_CONNECTING:
				s = va( trap_TranslateString( "^3Awaiting connection...^7%i" ), cstate.connectPacketCount );
				break; 
			case CA_CHALLENGING:
				s = va( trap_TranslateString( "^3Awaiting challenge...^7%i" ), cstate.connectPacketCount );
				break;
			case CA_DISCONNECTED:
			case CA_CONNECTED:
				if( *downloadName || cstate.connState == CA_DISCONNECTED ) {
					s = (char *)UI_DownloadInfo( downloadName );
				} else {
					s = trap_TranslateString( "^3Awaiting gamestate..." );
				}
				break;
			case CA_LOADING:
			case CA_PRIMED:
			default:
				break;
			}
		} else if ( trap_Cvar_VariableValue( "ui_dl_running" ) ) {
			// only toggle during a disconnected download
			s = (char *)UI_DownloadInfo( downloadName );
		}

		Q_strcat( buff, sizeof(buff), va( "\n\n%s^*", s ) );

		if( cstate.connState < CA_CONNECTED && *cstate.messageString ) {
			Q_strcat( buff, sizeof(buff), va( "\n\n%s^*", cstate.messageString ) );
		}
	}

	BG_FitTextToWidth_Ext( buff, button->font->scalex, button->rect.w, sizeof(buff), button->font->font );

	//UI_DrawRect( button->rect.x, button->rect.y, button->rect.w, button->rect.h, colorRed );

	y = button->rect.y;

	s = p = buff;

	while( *p ) {
		if( *p == '\n' ) {
			*p++ = '\0';
			Text_Paint_Ext( button->rect.x, y, button->font->scalex, button->font->scaley, button->font->colour, s, 0, 0, 0, button->font->font );
			y += 8;
			s = p;
		} else {
			p++; 
		}
	}
}
