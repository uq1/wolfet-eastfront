// cg_scoreboard -- draw the scoreboard on top of the game screen
#include "cg_local.h"


#define	SCOREBOARD_WIDTH	(31*BIGCHAR_WIDTH)

vec4_t clrUiBack = { 0.f, 0.f, 0.f, .6f };
vec4_t clrUiBar = { .16f, .2f, .17f, .8f };

/*
=================
WM_DrawObjectives
=================
*/

#define INFO_PLAYER_WIDTH		146
#define INFO_SCORE_WIDTH		56
#define INFO_XP_WIDTH			36
#define INFO_CLASS_WIDTH		43
#define INFO_LATENCY_WIDTH		35
#define INFO_LIVES_WIDTH		20
#define INFO_TEAM_HEIGHT		24
#define INFO_BORDER			2
#define INFO_PADDING			(INFO_BORDER + 2)
#define INFO_LINE_HEIGHT		30
#define INFO_TOTAL_WIDTH		(INFO_PLAYER_WIDTH + INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH)

#define MINIROW_HEIGHT			12
#define NORMALROW_HEIGHT		16
#define FONT_Y_OFFSET			12
#define FONT_Y_OFFSET_SMALL		10

#define INFO_FONTSCALE			0.25f
#define INFO_FONTSCALE_SMALL		0.20f
#define INFO_FONT			cgs.media.tahoma16

int WM_DrawObjectives( int x, int y, int width, float fade ) {
	const char *s, *str;
	int tempy, rows;
	int msec, mins, seconds, tens; // JPW NERVE
	vec4_t tclr =	{ 0.6f,		0.6f,		0.6f,		1.0f };

	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		const char *s, *buf, *shader = NULL, *flagshader = NULL, *nameshader = NULL;

		// Moved to CG_DrawIntermission
/*		static int doScreenshot = 0, doDemostop = 0;

		// OSP - End-of-level autoactions
		if(!cg.demoPlayback) {
			if(!cg.latchVictorySound) {
				if(cg_autoAction.integer & AA_SCREENSHOT) {
					doScreenshot = cg.time + 1000;
				}
				if(cg_autoAction.integer & AA_STATSDUMP) {
					CG_dumpStats_f();
				}
				if((cg_autoAction.integer & AA_DEMORECORD) && (cgs.gametype == GT_WOLF_STOPWATCH && cgs.currentRound != 1)) {
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
*/
		rows = 8;
		y += NORMALROW_HEIGHT * ( rows - 1 );

		s = CG_ConfigString( CS_MULTI_MAPWINNER );
		buf = Info_ValueForKey( s, "winner" );

		if ( atoi( buf ) == -1 )
			str = "ITS A TIE!";
		else if ( atoi( buf ) ) {
			str = "ALLIES";
//			shader = "ui/assets/portraits/allies_win";
			flagshader = "ui/assets/portraits/allies_win_flag.tga";
			nameshader = "ui/assets/portraits/text_allies.tga";

/*			if ( !cg.latchVictorySound ) {
				cg.latchVictorySound = qtrue;
				trap_S_StartLocalSound( trap_S_RegisterSound( "sound/music/allies_win.wav", qtrue ), CHAN_LOCAL_SOUND );	// FIXME: stream
			}*/
		}
		else {
			str = "AXIS";
//			shader = "ui/assets/portraits/axis_win";
			flagshader = "ui/assets/portraits/axis_win_flag.tga";
			nameshader = "ui/assets/portraits/text_axis.tga";

/*			if ( !cg.latchVictorySound ) {
				cg.latchVictorySound = qtrue;
				trap_S_StartLocalSound( trap_S_RegisterSound( "sound/music/axis_win.wav", qtrue ), CHAN_LOCAL_SOUND );	// FIXME: stream
			}*/
		}

		y += NORMALROW_HEIGHT * ( ( rows - 2 ) / 2 );

		if ( flagshader ) {
			CG_DrawPic( 100, 10, 210, 136, trap_R_RegisterShaderNoMip( flagshader ) );
			CG_DrawPic( 325, 10, 210, 136, trap_R_RegisterShaderNoMip( flagshader ) );
		}

		if ( shader )
			CG_DrawPic( 229, 10, 182, 136, trap_R_RegisterShaderNoMip( shader ) );
		if ( nameshader ) {
			CG_DrawPic( 140, 50, 127, 64, trap_R_RegisterShaderNoMip( nameshader ) );
			CG_DrawPic( 365, 50, 127, 64, trap_R_RegisterShaderNoMip( "ui/assets/portraits/text_win.tga" ) );
		}
		return y;
	}
// JPW NERVE -- mission time & reinforce time
	else {
		tempy = y;
		rows = 1;

		CG_FillRect( x-5, y-2, width+5, 21, clrUiBack );
		CG_FillRect( x-5, y-2, width+5, 21, clrUiBar );
		CG_DrawRect_FixedBorder( x-5, y-2, width+5, 21, 1, colorBlack );

		y += NORMALROW_HEIGHT * ( rows - 1 );
		if( cgs.timelimit > 0.0f ) {
			msec = ( cgs.timelimit * 60.f * 1000.f ) - ( cg.time - cgs.levelStartTime );

			seconds = msec / 1000;
			mins = seconds / 60;
			seconds -= mins * 60;
			tens = seconds / 10;
			seconds -= tens * 10;
		} else {
			msec = mins = tens = seconds = 0;
		}

		if( cgs.gamestate != GS_PLAYING ) {
			s = va("%s %s", CG_TranslateString("MISSION TIME:"),  CG_TranslateString("WARMUP"));
		} else if ( msec < 0 && cgs.timelimit > 0.0f ) {
			if ( cgs.gamestate == GS_WAITING_FOR_PLAYERS )
				s = va( "%s %s", CG_TranslateString( "MISSION TIME:" ),  CG_TranslateString( "GAME STOPPED" ) );
			else
				s = va( "%s %s", CG_TranslateString( "MISSION TIME:" ),  CG_TranslateString( "SUDDEN DEATH" ) );
		} else {
			s = va( "%s   %2.0f:%i%i", CG_TranslateString( "MISSION TIME:" ), (float)mins, tens, seconds ); // float cast to line up with reinforce time
		}

		CG_Text_Paint_Ext( x, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );

		if( cgs.gametype != GT_WOLF_LMS ) {
			if(cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_AXIS || cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_ALLIES) {
				msec = CG_CalculateReinfTime( qfalse ) * 1000;
			}
			else // no team (spectator mode)
				msec = 0;

			if (msec) {
				seconds = msec / 1000;
				mins = seconds / 60;
				seconds -= mins * 60;
				tens = seconds / 10;
				seconds -= tens * 10;

				s = va( "%s %2.0f:%i%i", CG_TranslateString( "REINFORCE TIME:" ), (float)mins, tens, seconds );
				CG_Text_Paint_Ext( 640 - 20 - CG_Text_Width_Ext( s, 0.25f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 ), y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
			}
		}

		// NERVE - SMF
		if ( cgs.gametype == GT_WOLF_STOPWATCH ) {
			int w;
			s = va( "%s %i", CG_TranslateString( "STOPWATCH ROUND" ), cgs.currentRound + 1 );

			w = CG_Text_Width_Ext( s, 0.25f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );

			CG_Text_Paint_Ext( x + 300 - w*0.5f, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
		} else if( cgs.gametype == GT_WOLF_LMS ) {
			int w;
			s = va( "%s %i  %s %i-%i", CG_TranslateString( "ROUND" ), cgs.currentRound + 1, CG_TranslateString( "SCORE" ), cg.teamWonRounds[1], cg.teamWonRounds[0] );
			w = CG_Text_Width_Ext( s, 0.25f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );

			CG_Text_Paint_Ext( x + 300 - w*0.5f, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
		} else if( cgs.gametype == GT_WOLF_CAMPAIGN ) {
#ifdef __ETE__
		} else if( cg_gameType.integer == GT_WOLF_CAMPAIGN || cg_gameType.integer == GT_SUPREMACY_CAMPAIGN ) {
#else //!__ETE__
		} else if( cg_gameType.integer == GT_WOLF_CAMPAIGN ) {
#endif //__ETE__
			int w;
			s = va( "MAP %i of %i", cgs.currentCampaignMap + 1, cgs.campaignData.mapCount );
			w = CG_Text_Width_Ext( s, 0.25f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );

			CG_Text_Paint_Ext( x + 300 - w*0.5f, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
		}
		
		y += NORMALROW_HEIGHT * 2;
	}
// jpw

	return y;
}

static void WM_DrawClientScore( int x, int y, score_t *score, float *color, float fade ) {
	int offset, i, j;
	char truncname[MAX_QPATH];
	float tempx;
	vec4_t hcolor;
	clientInfo_t *ci;

	if ( y + NORMALROW_HEIGHT >= 470 )
		return;

	ci = &cgs.clientinfo[score->client];
	tempx = x;

	if ( score->client == cg.snap->ps.clientNum ) {
		hcolor[3] = fade * 0.3;
		VectorSet( hcolor, .5f, .5f, .2f );			// DARK-RED

		CG_FillRect( x - 5, y, INFO_PLAYER_WIDTH + 5 - INFO_BORDER, NORMALROW_HEIGHT - 1, hcolor);
		tempx += INFO_PLAYER_WIDTH;

		CG_FillRect( tempx, y, INFO_CLASS_WIDTH - INFO_BORDER, NORMALROW_HEIGHT - 1, hcolor );
		tempx += INFO_CLASS_WIDTH;

		if( cg_gameType.integer == GT_WOLF_LMS ) {
			CG_FillRect( tempx, y, INFO_SCORE_WIDTH - INFO_BORDER, NORMALROW_HEIGHT - 1, hcolor );
			tempx += INFO_SCORE_WIDTH;
		} else {
			CG_FillRect( tempx, y, INFO_XP_WIDTH - INFO_BORDER, NORMALROW_HEIGHT - 1, hcolor );
			tempx += INFO_XP_WIDTH;				
		}

		CG_FillRect( tempx, y, INFO_LATENCY_WIDTH - INFO_BORDER, NORMALROW_HEIGHT - 1, hcolor );
		tempx += INFO_LATENCY_WIDTH;

		if( cg_gameType.integer != GT_WOLF_LMS ) {
			CG_FillRect( tempx, y, INFO_LIVES_WIDTH, NORMALROW_HEIGHT - 1, hcolor );
			tempx += INFO_LIVES_WIDTH;
		}
	}

	tempx = x;

	// DHM - Nerve
	VectorSet( hcolor, 1, 1, 1 );
	hcolor[3] = fade;

	offset = 0;

	if ( ci->team != TEAM_SPECTATOR ) {
		if ( ci->powerups & ( (1 << PW_REDFLAG) | (1 << PW_BLUEFLAG) ) ) {
			CG_DrawPic( tempx-4, y, 16, 16, cgs.media.objectiveShader );
			offset += 12;
		}

		// draw the skull icon if out of lives
		if( score->respawnsLeft == -2 || 
				(cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && 
				ci->team == cgs.clientinfo[cg.clientNum].team && 
				cgs.clientinfo[score->client].health == -1 ) ) {
			CG_DrawPic( tempx-4, y, 16, 16, cgs.media.scoreEliminatedShader );
			offset += 12;
		} else if( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && 
				ci->team == cgs.clientinfo[cg.clientNum].team && 
				cgs.clientinfo[score->client].health == 0 ) {
			CG_DrawPic( tempx-4, y, 16, 16, cgs.media.medicIcon );
			offset += 12;
		}
	}

	Q_strncpyz(truncname, va("%s ", ci->name), sizeof(truncname));
	// medals only show if name doesn't take up entire name space
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		for( j = 0; j < ci->medals[i]; j++ )
			Q_strcat( truncname, sizeof(truncname), va( "^%c%c", COLOR_RED + i, skillNames[i][0] ) );
	}
	CG_FitTextToWidth_SingleLine_Ext(truncname, INFO_FONTSCALE, INFO_PLAYER_WIDTH - offset - 5, sizeof(truncname), &INFO_FONT);

	// draw name
	CG_Text_Paint_Ext( tempx + offset, y + FONT_Y_OFFSET, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
		truncname, 0, 0, 0, &INFO_FONT );
	tempx += INFO_PLAYER_WIDTH;

	// draw class/spec status
	tempx += INFO_CLASS_WIDTH;
	if ( ci->team == TEAM_SPECTATOR ) {
		const char *s;

 		// CHRUKER: b031 -  Show connecting people as connecting
		if ( score->ping <= 0 ) {
			s = CG_TranslateString( "^3CONN" );
		} else if ( score->ping == 999 ) {
			s = CG_TranslateString(" ^3LAG" );
		} else {
			s = CG_TranslateString( "^3SPEC" );
		}

		CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
			s, 0, 0, 0, &INFO_FONT );
		//return;
	}
	// OSP - allow MV clients see the class of its merged client's on the scoreboard
	else if ( cg.snap->ps.persistant[PERS_TEAM] == ci->team || CG_mvMergedClientLocate(score->client) ) {
		CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
			CG_TranslateString(BG_ShortClassnameForNumber(score->playerClass)), 0, 0, 0, &INFO_FONT );
	}

	// draw score/XP
	if( cg_gameType.integer == GT_WOLF_LMS ) {
		tempx += INFO_SCORE_WIDTH;
	} else {
		tempx += INFO_XP_WIDTH;
	}
	CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
		va("%d", score->score), 0, 0, 0, &INFO_FONT );

	// draw ping
	tempx += INFO_LATENCY_WIDTH;
	if (  score->ping <= -2 ){ 
		CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite,
			"BOT", 0, 0, 0, &INFO_FONT );
	} else {
		CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
			va("%d", score->ping), 0, 0, 0, &INFO_FONT );
	}

	// draw limited lives num
	if( cg_gameType.integer != GT_WOLF_LMS ) {
		tempx += INFO_LIVES_WIDTH;
		if( score->respawnsLeft >= 0 ) {
			CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
				va("%d", score->respawnsLeft), 0, 0, 0, &INFO_FONT );
		} else {
			CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
				" -", 0, 0, 0, &INFO_FONT );
		}
	}
}

const char* WM_TimeToString( float msec ) {
	int mins, seconds, tens;

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	return va( "%i:%i%i", mins, tens, seconds );
}

static void WM_DrawClientScore_Small( int x, int y, score_t *score, float *color, float fade ) {
/*	int maxchars, offset;
	float tempx;
	vec4_t hcolor;
	clientInfo_t *ci;
	 // CHRUKER: b033 - Added to draw medals
	int i, j;
	char buf[64];

	// CHRUKER: b033 - Was using the wrong char height for this
	//          calculation
	if ( y + MINICHAR_HEIGHT >= 470 )
		return;

	ci = &cgs.clientinfo[score->client];

	if ( score->client == cg.snap->ps.clientNum ) {
		tempx = x;

		hcolor[3] = fade * 0.3;
		VectorSet( hcolor, .5f, .5f, .2f );			// DARK-RED

		CG_FillRect( tempx, y + 1, INFO_PLAYER_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
		tempx += INFO_PLAYER_WIDTH;

		if ( ci->team == TEAM_SPECTATOR ) {
			int width;
			width = INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;

			CG_FillRect( tempx, y + 1, width - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
			tempx += width;
		} else {
			CG_FillRect( tempx, y + 1, INFO_CLASS_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
			tempx += INFO_CLASS_WIDTH;

			if( cg_gameType.integer == GT_WOLF_LMS ) {
				CG_FillRect( tempx, y + 1, INFO_SCORE_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
				tempx += INFO_SCORE_WIDTH;
			} else {
				CG_FillRect( tempx, y + 1, INFO_XP_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
				tempx += INFO_XP_WIDTH;				
			}

			CG_FillRect( tempx, y + 1, INFO_LATENCY_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
			tempx += INFO_LATENCY_WIDTH;

			if( cg_gameType.integer != GT_WOLF_LMS ) {
				CG_FillRect( tempx, y + 1, INFO_LIVES_WIDTH - INFO_BORDER, MINICHAR_HEIGHT - 1, hcolor );
				tempx += INFO_LIVES_WIDTH;
			}
		}
	}

	tempx = x;

	// DHM - Nerve
	VectorSet( hcolor, 1, 1, 1 );
	hcolor[3] = fade;

	// CHRUKER: b033 - Corrected to draw medals
	maxchars = 16;
	offset = 0;

	if ( ci->team != TEAM_SPECTATOR ) {
		if ( ci->powerups & ( (1 << PW_REDFLAG) | (1 << PW_BLUEFLAG) ) ) {
			CG_DrawPic( tempx-2, y-4, 20, 20, trap_R_RegisterShader( "models/multiplayer/treasure/treasure" ) );
			offset += 14;
			tempx += 14;
			maxchars -= 2;
		}

		// draw the skull icon if out of lives
		if ( score->respawnsLeft == -2 || ( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && ci->team == cgs.clientinfo[cg.clientNum].team && cgs.clientinfo[score->client].health == -1 ) ) {
			CG_DrawPic( tempx, y, 12, 12, cgs.media.scoreEliminatedShader );
			offset += 14;
			tempx += 14;
			maxchars -= 2;
		} else if( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && ci->team == cgs.clientinfo[cg.clientNum].team && cgs.clientinfo[score->client].health == 0 ) {
			CG_DrawPic( tempx + 1, y + 1, 10, 10, cgs.media.medicIcon );
			offset += 14;
			tempx += 14;
			maxchars -= 2;
		}
	}

	// draw name
	CG_DrawStringExt( tempx, y, ci->name, hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, maxchars );
	
	// CHRUKER: b033 - Added to draw medals
	maxchars -= CG_DrawStrlen( ci->name );

	buf[0] = '\0';
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		for( j = 0; j < ci->medals[i]; j++ )
			Q_strcat( buf, sizeof(buf), va( "^%c%c", COLOR_RED + i, skillNames[i][0] ) );
	}
	maxchars--;

	if (maxchars > 0) CG_DrawStringExt( tempx + (BG_drawStrlen(ci->name) * MINICHAR_WIDTH + MINICHAR_WIDTH), y, buf, hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, maxchars );
	// b033

	tempx += INFO_PLAYER_WIDTH - offset;
	// dhm - nerve

	if ( ci->team == TEAM_SPECTATOR ) {
		const char *s;
		int w, totalwidth;

		totalwidth = INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH - 8;

	
 		// CHRUKER: b031 -  Show connecting people as connecting
 		if (score->ping == -1) {
 			s = CG_TranslateString( "^3CONNECTING" );
 		}
 		else {
  			s = CG_TranslateString( "^3SPECTATOR" );
 		}
  		w = CG_DrawStrlen( s ) * MINICHAR_WIDTH;
 		// CHRUKER: b034 - Using the mini char height
 		CG_DrawStringExt( tempx + totalwidth - w, y, s, hcolor, qfalse,
 			qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );

		CG_DrawSmallString( tempx + totalwidth - w, y, s, fade );
		return;
	}
	else if ( cg.snap->ps.persistant[PERS_TEAM] == ci->team ) {
		CG_DrawStringExt(	tempx, y, CG_TranslateString( BG_ShortClassnameForNumber( score->playerClass ) ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
//		CG_DrawSmallString( tempx, y, CG_TranslateString( s ), fade );
	}
	tempx += INFO_CLASS_WIDTH;

	CG_DrawStringExt( tempx, y, va( "%3i", score->score ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
	if( cg_gameType.integer == GT_WOLF_LMS ) {
		tempx += INFO_SCORE_WIDTH;
	} else {
		tempx += INFO_XP_WIDTH;
	}

	  if (  score->ping <= -2 ){ 
	CG_DrawStringExt( tempx, y, va( "BOT" ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
      } else {
	CG_DrawStringExt( tempx, y, va( "%4i", score->ping ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );		  
      }

	tempx += INFO_LATENCY_WIDTH;

	if( cg_gameType.integer != GT_WOLF_LMS ) {
		if( score->respawnsLeft >= 0 ) {
			CG_DrawStringExt( tempx, y, va( "%2i", score->respawnsLeft ), hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
		} else {
			CG_DrawStringExt( tempx, y, " -", hcolor, qfalse, qfalse, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );
		}
		tempx += INFO_LIVES_WIDTH;
	}*/
	int offset, i, j;
	char truncname[MAX_QPATH];
	float tempx;
	vec4_t hcolor;
	clientInfo_t *ci;

	if ( y + MINIROW_HEIGHT >= 470 )
		return;

	ci = &cgs.clientinfo[score->client];
	tempx = x;

	if ( score->client == cg.snap->ps.clientNum ) {
		hcolor[3] = fade * 0.3;
		VectorSet( hcolor, .5f, .5f, .2f );			// DARK-RED

		CG_FillRect( x - 5, y, INFO_PLAYER_WIDTH + 5 - INFO_BORDER, MINIROW_HEIGHT - 1, hcolor);
		tempx += INFO_PLAYER_WIDTH;

		CG_FillRect( tempx, y, INFO_CLASS_WIDTH - INFO_BORDER, MINIROW_HEIGHT - 1, hcolor );
		tempx += INFO_CLASS_WIDTH;

		if( cg_gameType.integer == GT_WOLF_LMS ) {
			CG_FillRect( tempx, y, INFO_SCORE_WIDTH - INFO_BORDER, MINIROW_HEIGHT - 1, hcolor );
			tempx += INFO_SCORE_WIDTH;
		} else {
			CG_FillRect( tempx, y, INFO_XP_WIDTH - INFO_BORDER, MINIROW_HEIGHT - 1, hcolor );
			tempx += INFO_XP_WIDTH;				
		}

		CG_FillRect( tempx, y, INFO_LATENCY_WIDTH - INFO_BORDER, MINIROW_HEIGHT - 1, hcolor );
		tempx += INFO_LATENCY_WIDTH;

		if( cg_gameType.integer != GT_WOLF_LMS ) {
			CG_FillRect( tempx, y, INFO_LIVES_WIDTH, MINIROW_HEIGHT - 1, hcolor );
			tempx += INFO_LIVES_WIDTH;
		}
	}

	tempx = x;

	// DHM - Nerve
	VectorSet( hcolor, 1, 1, 1 );
	hcolor[3] = fade;

	offset = 0;

	if ( ci->team != TEAM_SPECTATOR ) {
		if ( ci->powerups & ( (1 << PW_REDFLAG) | (1 << PW_BLUEFLAG) ) ) {
			CG_DrawPic( tempx-4, y, 11, 11, cgs.media.objectiveShader );
			offset += 8;
		}

		// draw the skull icon if out of lives
		if( score->respawnsLeft == -2 || 
				(cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && 
				ci->team == cgs.clientinfo[cg.clientNum].team && 
				cgs.clientinfo[score->client].health == -1 ) ) {
			CG_DrawPic( tempx-4, y, 11, 11, cgs.media.scoreEliminatedShader );
			offset += 8;
		} else if( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR && 
				ci->team == cgs.clientinfo[cg.clientNum].team && 
				cgs.clientinfo[score->client].health == 0 ) {
			CG_DrawPic( tempx-4, y, 11, 11, cgs.media.medicIcon );
			offset += 8;
		}
	}

	Q_strncpyz(truncname, va("%s ", ci->name), sizeof(truncname));
	// medals only show if name doesn't take up entire name space
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		for( j = 0; j < ci->medals[i]; j++ )
			Q_strcat( truncname, sizeof(truncname), va( "^%c%c", COLOR_RED + i, skillNames[i][0] ) );
	}
	CG_FitTextToWidth_SingleLine_Ext(truncname, INFO_FONTSCALE_SMALL, INFO_PLAYER_WIDTH - offset - 5, sizeof(truncname), &INFO_FONT);

	// draw name
	CG_Text_Paint_Ext( tempx + offset, y + FONT_Y_OFFSET_SMALL, INFO_FONTSCALE_SMALL, INFO_FONTSCALE_SMALL, colorWhite, 
		truncname, 0, 0, 0, &INFO_FONT );
	tempx += INFO_PLAYER_WIDTH;

	// draw class/spec status
	tempx += INFO_CLASS_WIDTH;
	if ( ci->team == TEAM_SPECTATOR ) {
		const char *s;

 		// CHRUKER: b031 -  Show connecting people as connecting
		if ( score->ping <= 0 ) {
			s = CG_TranslateString( "^3CONN" );
		} else if ( score->ping == 999 ) {
			s = CG_TranslateString(" ^3LAG" );
		} else {
			s = CG_TranslateString( "^3SPEC" );
		}

		CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET_SMALL, INFO_FONTSCALE_SMALL, INFO_FONTSCALE_SMALL, colorWhite, 
			s, 0, 0, 0, &INFO_FONT );
		//return;
	}
	// OSP - allow MV clients see the class of its merged client's on the scoreboard
	else if ( cg.snap->ps.persistant[PERS_TEAM] == ci->team || CG_mvMergedClientLocate(score->client) ) {
		CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET_SMALL, INFO_FONTSCALE_SMALL, INFO_FONTSCALE_SMALL, colorWhite, 
			CG_TranslateString(BG_ShortClassnameForNumber(score->playerClass)), 0, 0, 0, &INFO_FONT );
	}

	// draw score/XP
	if( cg_gameType.integer == GT_WOLF_LMS ) {
		tempx += INFO_SCORE_WIDTH;
	} else {
		tempx += INFO_XP_WIDTH;
	}
	CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET_SMALL, INFO_FONTSCALE_SMALL, INFO_FONTSCALE_SMALL, colorWhite, 
		va("%d", score->score), 0, 0, 0, &INFO_FONT );

	// draw ping
	tempx += INFO_LATENCY_WIDTH;
	if (  score->ping <= -2 ){ 
		CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET_SMALL, INFO_FONTSCALE_SMALL, INFO_FONTSCALE_SMALL, colorWhite,
			"BOT", 0, 0, 0, &INFO_FONT );
	} else {
		CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET_SMALL, INFO_FONTSCALE_SMALL, INFO_FONTSCALE_SMALL, colorWhite, 
			va("%d", score->ping), 0, 0, 0, &INFO_FONT );
	}

	// draw limited lives num
	if( cg_gameType.integer != GT_WOLF_LMS ) {
		tempx += INFO_LIVES_WIDTH;
		if( score->respawnsLeft >= 0 ) {
			CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET_SMALL, INFO_FONTSCALE_SMALL, INFO_FONTSCALE_SMALL, colorWhite, 
				va("%d", score->respawnsLeft), 0, 0, 0, &INFO_FONT );
		} else {
			CG_Text_Paint_Right_Ext( tempx - INFO_PADDING, y + FONT_Y_OFFSET_SMALL, INFO_FONTSCALE_SMALL, INFO_FONTSCALE_SMALL, colorWhite, 
				" -", 0, 0, 0, &INFO_FONT );
		}
	}
}

static int WM_DrawInfoLine( int x, int y, float fade ) {
	int w, defender, winner;
	const char *s;
	vec4_t tclr =	{ 0.6f,		0.6f,		0.6f,		1.0f };

	if ( cg.snap->ps.pm_type != PM_INTERMISSION ) {
		return y;
	}

	w = 360;
//	CG_DrawPic( 320 - w/2, y, w, INFO_LINE_HEIGHT, trap_R_RegisterShaderNoMip( "ui/assets/mp_line_strip.tga" ) );

	s = CG_ConfigString( CS_MULTI_INFO );
	defender = atoi( Info_ValueForKey( s, "defender" ) );

	s = CG_ConfigString( CS_MULTI_MAPWINNER );
	winner = atoi( Info_ValueForKey( s, "winner" ) );

	if ( cgs.currentRound ) {
		// first round
		s = va( CG_TranslateString( "CLOCK IS NOW SET TO %s!" ), WM_TimeToString( cgs.nextTimeLimit * 60.f * 1000.f ) );
	}
	else {
		// second round
		if ( !defender ) {
			if ( winner != defender )
				s = "ALLIES SUCCESSFULLY BEAT THE CLOCK!";
			else
				s = "ALLIES COULDN'T BEAT THE CLOCK!";
		}
		else {
			if ( winner != defender )
				s = "AXIS SUCCESSFULLY BEAT THE CLOCK!";
			else
				s = "AXIS COULDN'T BEAT THE CLOCK!";
		}

		s = CG_TranslateString( s );
	}

	CG_FillRect( 320 - w/2, y, w, 20, clrUiBar );
	CG_DrawRect_FixedBorder( 320 - w/2, y, w, 20, 1, colorBlack );

	w = CG_Text_Width_Ext( s, 0.25f, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );

	CG_Text_Paint_Ext( 320 - w*0.5f, y + 15, 0.25f, 0.25f, tclr, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
//	CG_DrawSmallString( 320 - w/2, ( y + INFO_LINE_HEIGHT / 2 ) - SMALLCHAR_HEIGHT / 2, s, fade );
	return y + INFO_LINE_HEIGHT + 6;
}

// CHRUKER: b035 - Added absolute maximum rows
static int WM_TeamScoreboard( int x, int y, team_t team, float fade, int maxrows, int absmaxrows ) {
	vec4_t hcolor;
	float tempx, tempy;
	int height, width;
	int i;
	int count = 0;
	// CHRUKER: b035 - Needed to check if using mini chars
	qboolean use_mini_chars = qfalse;
	vec4_t tclr =	{ 0.6f,		0.6f,		0.6f,		1.0f };

	height = NORMALROW_HEIGHT * maxrows;
	width = INFO_PLAYER_WIDTH + INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;

	CG_FillRect( x-5, y-2, width+5, 21, clrUiBack );
	CG_FillRect( x-5, y-2, width+5, 21, clrUiBar );	
	
	Vector4Set( hcolor, 0, 0, 0, fade );
	CG_DrawRect_FixedBorder( x-5, y-2, width+5, 21, 1, colorBlack );

	// draw header
	if( cg_gameType.integer == GT_WOLF_LMS ) {
		char *s;
		if ( team == TEAM_AXIS ) {
			s = va( "%s [%d] (%d %s)", CG_TranslateString( "AXIS" ), cg.teamScores[0], cg.teamPlayers[team], CG_TranslateString("PLAYERS") );
			s = va( "%s ^3%s", s, cg.teamFirstBlood == TEAM_AXIS ? CG_TranslateString("FIRST BLOOD") : "" );

			CG_Text_Paint_Ext( x, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
		} else if ( team == TEAM_ALLIES ) {
			s = va( "%s [%d] (%d %s)", CG_TranslateString( "ALLIES" ), cg.teamScores[1], cg.teamPlayers[team], CG_TranslateString("PLAYERS") );
			s = va( "%s ^3%s", s, cg.teamFirstBlood == TEAM_ALLIES ? CG_TranslateString("FIRST BLOOD") : "" );

			CG_Text_Paint_Ext( x, y + 13, 0.25f, 0.25f, tclr, s, 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
		}
	} else {
		if ( team == TEAM_AXIS ) {
			CG_Text_Paint_Ext( x, y + 13, 0.25f, 0.25f, tclr, va( "%s [%d] (%d %s)", CG_TranslateString( "AXIS" ), cg.teamScores[0], cg.teamPlayers[team], CG_TranslateString("PLAYERS") ), 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
		} else if ( team == TEAM_ALLIES ) {
			CG_Text_Paint_Ext( x, y + 13, 0.25f, 0.25f, tclr, va( "%s [%d] (%d %s)", CG_TranslateString( "ALLIES" ), cg.teamScores[1], cg.teamPlayers[team], CG_TranslateString("PLAYERS") ), 0, 0, 0, /*&cgs.media.limboFont1*/&cgs.media.tahomabd24 );
		}
	}

	y += NORMALROW_HEIGHT + 3;

	tempx = x;

	CG_FillRect( x-5, y, width+5, 18, clrUiBack );
	trap_R_SetColor( colorBlack );
	CG_DrawBottom_NoScale( x-5, y, width+5, 18, 1 );
	trap_R_SetColor( NULL );

	// draw player info headings
//	CG_DrawSmallString( tempx, y + FONT_Y_OFFSET + 2, CG_TranslateString( "Name" ), fade );
	CG_Text_Paint_Ext( tempx, y + FONT_Y_OFFSET + 2, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
		CG_TranslateString("Name"), 0, 0, 0, &INFO_FONT );
	tempx += INFO_PLAYER_WIDTH;

//	CG_DrawSmallString( tempx, y + FONT_Y_OFFSET + 2, CG_TranslateString( "Class" ), fade );
	tempx += INFO_CLASS_WIDTH;
	CG_Text_Paint_Right_Ext( tempx - INFO_BORDER, y + FONT_Y_OFFSET + 2, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
		CG_TranslateString("Class"), 0, 0, 0, &INFO_FONT );

	if( cgs.gametype == GT_WOLF_LMS ) {
//		CG_DrawSmallString( tempx, y + FONT_Y_OFFSET + 2, CG_TranslateString( "Score" ), fade );
		tempx += INFO_SCORE_WIDTH;
		CG_Text_Paint_Right_Ext( tempx - INFO_BORDER, y + FONT_Y_OFFSET + 2, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
			CG_TranslateString("Score"), 0, 0, 0, &INFO_FONT );
	} else {
//		CG_DrawSmallString( tempx + 1 * SMALLCHAR_WIDTH, y + FONT_Y_OFFSET + 2, CG_TranslateString( "XP" ), fade );
		tempx += INFO_XP_WIDTH;
		CG_Text_Paint_Right_Ext( tempx - INFO_BORDER, y + FONT_Y_OFFSET + 2, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
			CG_TranslateString("XP"), 0, 0, 0, &INFO_FONT );
	}

//	CG_DrawSmallString( tempx, y + FONT_Y_OFFSET + 2, CG_TranslateString( "Ping" ), fade );
	tempx += INFO_LATENCY_WIDTH;
	CG_Text_Paint_Right_Ext( tempx - INFO_BORDER, y + FONT_Y_OFFSET + 2, INFO_FONTSCALE, INFO_FONTSCALE, colorWhite, 
		CG_TranslateString("Ping"), 0, 0, 0, &INFO_FONT );

	if( cgs.gametype != GT_WOLF_LMS ) {
		CG_DrawPicST( tempx + 2, y, INFO_LIVES_WIDTH - 4, 16, 0.f, 0.f, 0.5f, 1.f, team == TEAM_ALLIES ? cgs.media.hudAlliedHelmet : cgs.media.hudAxisHelmet );
		tempx += INFO_LIVES_WIDTH;
	}

	y += NORMALROW_HEIGHT + 2;

	cg.teamPlayers[team] = 0; // JPW NERVE
	for ( i = 0; i < cg.numScores; i++ ) {
		if ( team != cgs.clientinfo[ cg.scores[i].client ].team )
			continue;

		cg.teamPlayers[team]++;
	}

	// CHRUKER: b035 - Adjust maxrows
	if ( cg.teamPlayers[team] > maxrows ) {
		if (maxrows == 9) maxrows = absmaxrows;
		if (maxrows == 25) maxrows = absmaxrows;
		use_mini_chars = qtrue;
	}

	// save off y val
	tempy = y;

	// draw color bands
	for ( i = 0; i < maxrows; i++ ) {
		if ( i % 2 == 0 )
			VectorSet( hcolor, (80.f/255.f), (80.f/255.f), (80.f/255.f) );			// LIGHT BLUE
		else
			VectorSet( hcolor, (0.f/255.f), (0.f/255.f), (0.f/255.f) );			// DARK BLUE
		hcolor[3] = fade * 0.3;

		// CHRUKER: b035 - Missing support for mini char height scoreboard background
		if ( use_mini_chars ) {
			CG_FillRect( x-5, y, width+5, MINIROW_HEIGHT, hcolor );
			trap_R_SetColor( colorBlack );
			CG_DrawBottom_NoScale( x-5, y, width+5, MINIROW_HEIGHT, 1 );
			trap_R_SetColor( NULL );

			y += MINIROW_HEIGHT;
		} else {
			CG_FillRect( x-5, y, width+5, NORMALROW_HEIGHT, hcolor );
			trap_R_SetColor( colorBlack );
			CG_DrawBottom_NoScale( x-5, y, width+5, NORMALROW_HEIGHT, 1 );
			trap_R_SetColor( NULL );

			y += NORMALROW_HEIGHT;
		}
	}

	hcolor[3] = 1;

	y = tempy;

	// draw player info
	VectorSet( hcolor, 1, 1, 1 );
	hcolor[3] = fade;

	count = 0;
	for( i = 0; i < cg.numScores && count < maxrows; i++ ) {
		if( team != cgs.clientinfo[ cg.scores[i].client ].team ) {
			continue;
		}
		// CHRUKER: b035 - Using the flag instead
		if(use_mini_chars) {
			WM_DrawClientScore_Small( x, y, &cg.scores[i], hcolor, fade );
			y += MINIROW_HEIGHT;
		} else {
			WM_DrawClientScore( x, y, &cg.scores[i], hcolor, fade );
			y += NORMALROW_HEIGHT;
		}

		count++;
	}

	// draw spectators
	// CHRUKER: b035 - Missing support for mini char height scoreboard background
	if ( use_mini_chars )
		y += MINIROW_HEIGHT;
	else
		y += NORMALROW_HEIGHT;

	for ( i = 0; i < cg.numScores; i++ ) {
		if ( cgs.clientinfo[ cg.scores[i].client ].team != TEAM_SPECTATOR )
			continue;
		if ( team == TEAM_AXIS && ( i % 2 ) )
			continue;
		if ( team == TEAM_ALLIES && ( ( i + 1 ) % 2 ) )
			continue;
		// CHRUKER: b035 - Missing support for mini char height scoreboard background
		if( use_mini_chars ) {
			WM_DrawClientScore_Small( x, y, &cg.scores[i], hcolor, fade );
			y += MINIROW_HEIGHT;
		} else {
			WM_DrawClientScore( x, y, &cg.scores[i], hcolor, fade );
			y += NORMALROW_HEIGHT;
		}
	}

	return y;
}
// -NERVE - SMF

/*
=================
CG_DrawScoreboard

Draw the normal in-game scoreboard
=================
*/
qboolean CG_DrawScoreboard( void ) {
	int		x = 0, y = 0, x_right;
	float	fade;
	float	*fadeColor;

	x = 20;
	y = 10;

	x_right = 640 - x - (INFO_TOTAL_WIDTH - 5);

	// don't draw anything if the menu or console is up
	if ( cg_paused.integer ) {
		return qfalse;
	}

	// don't draw scoreboard during death while warmup up
	// OSP - also for pesky scoreboards in demos
	if((cg.warmup || (cg.demoPlayback && cg.snap->ps.pm_type != PM_INTERMISSION)) && !cg.showScores) {
		return qfalse;
	}

	// don't draw if in cameramode
	if( cg.cameraMode ) {
		return qtrue;
	}

	if( cg.showScores || cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		fade = 1.0;
		fadeColor = colorWhite;
	} else {
		fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );
		
		if( !fadeColor ) {
			// next time scoreboard comes up, don't print killer
			*cg.killerName = 0;
			return qfalse;
		}
 		fade = fadeColor[3];
	}

	y = WM_DrawObjectives( x, y, 640 - 2*x + 5, fade );

	if ( cgs.gametype == GT_WOLF_STOPWATCH && ( cg.snap->ps.pm_type == PM_INTERMISSION ) ) {
		y = WM_DrawInfoLine( x, 155, fade );
		// CHRUKER: b035 - The maxrows has been split into one for
		//          when to use the mini chars and one for when
		//          to stop writing.
		WM_TeamScoreboard( x, y, TEAM_AXIS, fade, 8, 8 );
		x = x_right;
		WM_TeamScoreboard( x, y, TEAM_ALLIES, fade, 8, 8 );
	} else {
		if(cg.snap->ps.pm_type == PM_INTERMISSION) {
			WM_TeamScoreboard( x, y, TEAM_AXIS, fade, 9, 12 );
			x = x_right;
			WM_TeamScoreboard( x, y, TEAM_ALLIES, fade, 9, 12 );
		} else {
			WM_TeamScoreboard( x, y, TEAM_AXIS, fade, 25, 33 );
			x = x_right;
			WM_TeamScoreboard( x, y, TEAM_ALLIES, fade, 25, 33 );
		}
	}

/*	if(!CG_IsSinglePlayer()) {
		qtime_t ct;

		G_showWindowMessages();
		trap_RealTime(&ct);
		s = va("^3%02d:%02d:%02d - %02d %s %d",
							ct.tm_hour, ct.tm_min, ct.tm_sec,
							ct.tm_mday, aMonths[ct.tm_mon], 1900 + ct.tm_year);
		CG_DrawStringExt(444, 12, s, colorWhite, qfalse, qtrue, 8, 8, 0);
	}
*/
	return qtrue;
}
