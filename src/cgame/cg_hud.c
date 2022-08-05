#include "cg_local.h"

qboolean CG_LoadHud(char *filename)
{
	int handle;
	char hudFilename[MAX_TOKEN_CHARS];
	pc_token_t token;

	memset(&cg.hud, 0, sizeof(cg.hud));

	if(!Q_stricmp(filename, "blank")) {
		CG_LoadBlankHud();
		return qtrue;
	}
	Q_strncpyz(hudFilename, va("hud/%s.hud", filename), MAX_TOKEN_CHARS);

	handle = trap_PC_LoadSource( hudFilename );
	if (!handle) {
		CG_LoadDefaultHud();
		return qfalse;
	}

	CG_LoadDefaultHud();

	while ( 1 ) {
		// hud elements
		if ( !trap_PC_ReadToken( handle, &token ) ) {
			break;
		}

		if ( !Q_stricmp( token.string, "elements") ) {
			// we need a bracket
			if ( !trap_PC_ReadToken( handle, &token ) ) {
				return qfalse;
			}

			if ( !Q_stricmp( token.string, "{" ) ) {
				while ( 1 ) {
					if ( !trap_PC_ReadToken( handle, &token ) ) {
						break;
					}

					// we've reached the end
					if ( !Q_stricmp( token.string, "}" ) ) {
						break;
					}

					//
					// compass
					//
					if ( !Q_stricmp ( token.string, "compass" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.compass[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.compass[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.compass[2] ) )
							continue;
					//
					// health bar
					//
					} else if ( !Q_stricmp ( token.string, "healthbar" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.healthbar[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.healthbar[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.healthbar[2] ) )
							continue;
					//
					// stamina bar
					//
					} else if ( !Q_stricmp( token.string, "staminabar" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.staminabar[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.staminabar[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.staminabar[2] ) )
							continue;

					//
					// weapon charge bar
					//
					} else if ( !Q_stricmp( token.string, "chargebar" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.chargebar[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.chargebar[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.chargebar[2] ) )
							continue;

					//
					// weapon overheat bar
					//
					} else if ( !Q_stricmp( token.string, "overheat" ) ) {
						// x y width height

						if ( !PC_Int_Parse( handle, &cg.hud.overheat[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.overheat[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.overheat[2] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.overheat[3] ) )
							continue;

					//
					// weapon card
					//
					} else if ( !Q_stricmp( token.string, "weaponcard" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.weaponcard[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.weaponcard[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.weaponcard[2] ) )
							continue;

					//
					// fireteam
					//
					} else if ( !Q_stricmp( token.string, "fireteam" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.fireteam[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.fireteam[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.fireteam[2] ) )
							continue;

					//
					// health text
					//
					} else if (!Q_stricmp( token.string, "healthtext" ) ) {
						// x y scale

						if ( !PC_Int_Parse( handle, &cg.hud.hp[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.hp[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.hp[2] ) )
							continue;

					//
					// xp text
					//
					} else if ( !Q_stricmp( token.string, "xptext" ) ) {
						// x y scale

						if ( !PC_Int_Parse( handle, &cg.hud.xp[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.xp[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.xp[2] ) )
							continue;

					//
					// upper right
					//
					} else if ( !Q_stricmp( token.string, "upperright" ) ) {
						// y
		
						if ( !PC_Int_Parse( handle, &cg.hud.draws[0] ) )
							continue;

					//
					// ammo count
					//
					} else if ( !Q_stricmp( token.string, "ammocount" ) ) {
						// x y scale

						if ( !PC_Int_Parse( handle, &cg.hud.ammo[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.ammo[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.ammo[2] ) )
							continue;

					//
					// skill pic #1
					//
					} else if ( !Q_stricmp( token.string, "skillpic1" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.skillpics[0][0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.skillpics[0][1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.skillpics[0][2] ) )
							continue;
					//
					// skill pic #2
					//
					} else if ( !Q_stricmp( token.string, "skillpic2" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.skillpics[1][0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.skillpics[1][1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.skillpics[1][2] ) )
							continue;
					//
					// skill pic #3
					//
					} else if ( !Q_stricmp( token.string, "skillpic3" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.skillpics[2][0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.skillpics[2][1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.skillpics[2][2] ) )
							continue;
					//
					// skill text #1
					//
					} else if ( !Q_stricmp( token.string, "skilltext1" ) ) {
						// x y scale

						if (!PC_Int_Parse( handle, &cg.hud.skilltexts[0][0] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skilltexts[0][1] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skilltexts[0][2] ))
							continue;

					//
					// skill text #2
					//
					} else if ( !Q_stricmp( token.string, "skilltext2" ) ) {
						// x y scale

						if (!PC_Int_Parse( handle, &cg.hud.skilltexts[1][0] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skilltexts[1][1] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skilltexts[1][2] ))
							continue;
					//
					// skill text #3
					//
					} else if ( !Q_stricmp( token.string, "skilltext3" ) ) {
						// x y scale

						if (!PC_Int_Parse( handle, &cg.hud.skilltexts[2][0] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skilltexts[2][1] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skilltexts[2][2] ))
							continue;
					//
					// head
					//
					} else if ( !Q_stricmp( token.string, "head" ) ) {
						// x y width height

						if ( !PC_Int_Parse( handle, &cg.hud.head[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.head[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.head[2] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.head[3] ) )
							continue;

					//
					// skillbox1
					//
					} else if ( !Q_stricmp( token.string, "skillbox1" ) ) {
						// x y size

						if (!PC_Int_Parse( handle, &cg.hud.skillboxes[0][0] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skillboxes[0][1] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skillboxes[0][2] ))
							continue;

					//
					// skillbox2
					//
					} else if ( !Q_stricmp( token.string, "skillbox2" ) ) {
						// x y size

						if (!PC_Int_Parse( handle, &cg.hud.skillboxes[1][0] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skillboxes[1][1] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skillboxes[1][2] ))
							continue;

					//
					// skillbox3
					//
					} else if ( !Q_stricmp( token.string, "skillbox3" ) ) {
						// x y size

						if (!PC_Int_Parse( handle, &cg.hud.skillboxes[2][0] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skillboxes[2][1] ))
							continue;
						if (!PC_Int_Parse( handle, &cg.hud.skillboxes[2][2] ))
							continue;
					//
					// lagometer
					//
					} else if ( !Q_stricmp( token.string, "lagometer" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.lagometer[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.lagometer[1] ) )
							continue;
					//
					// flag/covert op disguise
					//
					} else if ( !Q_stricmp( token.string, "flagcov" ) ) {
						// x y size

						if ( !PC_Int_Parse( handle, &cg.hud.flagcov[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.flagcov[1] ) )
							continue;

					//
					// cpm text, popup messages
					//
					} else if ( !Q_stricmp( token.string, "cpmtext" ) ) {
						// x y scale

						if ( !PC_Int_Parse( handle, &cg.hud.cpmtext[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.cpmtext[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.cpmtext[2] ) )
							continue;

					//
					// chat text area
					//
					} else if ( !Q_stricmp( token.string, "chattext" ) ) {
						// x y scale

						if ( !PC_Int_Parse( handle, &cg.hud.chattext[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.chattext[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.chattext[2] ) )
							continue;

					//
					// vote and fireteam text
					//
					} else if ( !Q_stricmp( token.string, "votefttext" ) ) {
						// x y

						if ( !PC_Int_Parse( handle, &cg.hud.votefttext[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.votefttext[1] ) )
							continue;

					//
					// lives left on a limited lives server
					//
					} else if ( !Q_stricmp( token.string, "livesleft" ) ) {
						// x y

						if ( !PC_Int_Parse( handle, &cg.hud.livesleft[0] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.livesleft[1] ) )
							continue;
						if ( !PC_Int_Parse( handle, &cg.hud.livesleft[2] ) )
							continue;
					}
				}
			}
		}   
	}
	return qtrue;
}

#ifndef __ETE__

// Create the etmain HUD - Normal RTCW UI
qboolean CG_LoadDefaultHud()
{
	cg.hud.ammo[0] = 618;
	cg.hud.ammo[1] = 470;
	cg.hud.ammo[2] = 25;
	cg.hud.chargebar[0] = 622;
	cg.hud.chargebar[1] = 388;
	cg.hud.chargebar[2] = 12;
	cg.hud.chargebar_vertical = 1;
	cg.hud.chattext[0] = 160;
	cg.hud.chattext[1] = 478;
	cg.hud.chattext[2] = 20;
	cg.hud.compass[0] = 504;
	cg.hud.compass[1] = 4;
	cg.hud.compass[2] = 132;
	cg.hud.cpmtext[0] = 4;
	cg.hud.cpmtext[1] = 360;
	cg.hud.cpmtext[2] = 20;
//	cg.hud.draws[0] = 140;
	cg.hud.draws[0] = 180;
	cg.hud.fireteam[0] = 10;
	cg.hud.fireteam[1] = 10;
	cg.hud.fireteam[2] = 100;
	cg.hud.flagcov[0] = 600;
	cg.hud.flagcov[1] = 340;
	cg.hud.head[0] = 44;
	cg.hud.head[1] = 388;
	cg.hud.head[2] = 62;
	cg.hud.head[3] = 80;
	cg.hud.healthbar[0] = 20;
	cg.hud.healthbar[1] = 388;
	cg.hud.healthbar[2] = 12;
	cg.hud.healthbar_vertical = 1;
	cg.hud.hp[0] = 84;
	cg.hud.hp[1] = 476;
	cg.hud.hp[2] = 25;
	cg.hud.lagometer[0] = 592;
	cg.hud.lagometer[1] = 280;
	cg.hud.livesleft[0] = 4;
	cg.hud.livesleft[1] = 360;
	cg.hud.livesleft[2] = 14;
	cg.hud.overheat[0] = 558;
	cg.hud.overheat[1] = 424;
	cg.hud.overheat[2] = 60;
	cg.hud.overheat[3] = 32;
	cg.hud.skillboxes[0][0] = 112;
	cg.hud.skillboxes[0][1] = 386;
	cg.hud.skillboxes[0][2] = 14;
	cg.hud.skillboxes[1][0] = 128;
	cg.hud.skillboxes[1][1] = 386;
	cg.hud.skillboxes[1][2] = 14;
	cg.hud.skillboxes[2][0] = 144;
	cg.hud.skillboxes[2][1] = 386;
	cg.hud.skillboxes[2][2] = 14;
	cg.hud.skillpics[0][0] = 112;
	cg.hud.skillpics[0][1] = 446;
	cg.hud.skillpics[0][2] = 16;
	cg.hud.skillpics[1][0] = 128;
	cg.hud.skillpics[1][1] = 446;
	cg.hud.skillpics[1][2] = 16;
	cg.hud.skillpics[2][0] = 144;
	cg.hud.skillpics[2][1] = 446;
	cg.hud.skillpics[2][2] = 16;
	cg.hud.skilltexts[0][0] = -1;
	cg.hud.skilltexts[0][1] = -1;
	cg.hud.skilltexts[0][2] = -1;
	cg.hud.skilltexts[1][0] = -1;
	cg.hud.skilltexts[1][1] = -1;
	cg.hud.skilltexts[1][2] = -1;
	cg.hud.skilltexts[2][0] = -1;
	cg.hud.skilltexts[2][1] = -1;
	cg.hud.skilltexts[2][2] = -1;
	cg.hud.staminabar[0] = 4;
	cg.hud.staminabar[1] = 388;
	cg.hud.staminabar[2] = 12;
	cg.hud.staminabar_vertical = 1;
	cg.hud.votefttext[0] = 8;
	cg.hud.votefttext[1] = 200;
	cg.hud.weaponcard[0] = 558;
	cg.hud.weaponcard[1] = 424;
	cg.hud.weaponcard[2] = 60;
	cg.hud.xp[0] = 140;
	cg.hud.xp[1] = 476;
	cg.hud.xp[2] = 25;
	return qtrue;
}

#else //__ETE__

// Create the etmain HUD - New ETE UI.
qboolean CG_LoadDefaultHud()
{
	cg.hud.ammo[0] = 626;
	cg.hud.ammo[1] = 428;
	cg.hud.ammo[2] = 16;
	cg.hud.weaponcard[0] = 558;
	cg.hud.weaponcard[1] = 414;
	cg.hud.weaponcard[2] = 16;

	cg.hud.healthbar[0] = 566.5;
	cg.hud.healthbar[1] = 440;
	cg.hud.healthbar[2] = 12;
	cg.hud.healthbar_vertical = 0;
	cg.hud.chargebar[0] = 566.5;
	cg.hud.chargebar[1] = 452;
	cg.hud.chargebar[2] = 12;
	cg.hud.chargebar_vertical = 0;
	cg.hud.staminabar[0] = 566.5;
	cg.hud.staminabar[1] = 464;
	cg.hud.staminabar[2] = 12;
	cg.hud.staminabar_vertical = 0;

	cg.hud.chattext[0] = 220;//160;
	cg.hud.chattext[1] = 478;
	cg.hud.chattext[2] = 20;
	cg.hud.compass[0] = 504;
	cg.hud.compass[1] = 4;
	cg.hud.compass[2] = 132;
	cg.hud.cpmtext[0] = 4;
	cg.hud.cpmtext[1] = 360;
	cg.hud.cpmtext[2] = 20;
	cg.hud.draws[0] = 180;
	cg.hud.fireteam[0] = 10;
	cg.hud.fireteam[1] = 10;
	cg.hud.fireteam[2] = 100;
	cg.hud.flagcov[0] = 56;
	cg.hud.flagcov[1] = 392;
	cg.hud.head[0] = 8;
	cg.hud.head[1] = 388;
	cg.hud.head[2] = 34.875;
	cg.hud.head[3] = 45;
	
	cg.hud.hp[0] = 72;
	cg.hud.hp[1] = 472;
	cg.hud.hp[2] = 16;

	cg.hud.lagometer[0] = 587;
	cg.hud.lagometer[1] = 280;
	cg.hud.livesleft[0] = 4;
	cg.hud.livesleft[1] = 360;
	cg.hud.livesleft[2] = 14;
	cg.hud.overheat[0] = 550;
	cg.hud.overheat[1] = 414;
	cg.hud.overheat[2] = 82;
	cg.hud.overheat[3] = 22;
	cg.hud.skillboxes[0][0] = 112;
	cg.hud.skillboxes[0][1] = 386;
	cg.hud.skillboxes[0][2] = 14;
	cg.hud.skillboxes[1][0] = 128;
	cg.hud.skillboxes[1][1] = 386;
	cg.hud.skillboxes[1][2] = 14;
	cg.hud.skillboxes[2][0] = 144;
	cg.hud.skillboxes[2][1] = 386;
	cg.hud.skillboxes[2][2] = 14;
	cg.hud.skillpics[0][0] = 8;
	cg.hud.skillpics[0][1] = 446;
	cg.hud.skillpics[0][2] = 16;
	cg.hud.skillpics[1][0] = 40;
	cg.hud.skillpics[1][1] = 446;
	cg.hud.skillpics[1][2] = 16;
	cg.hud.skillpics[2][0] = 72;
	cg.hud.skillpics[2][1] = 446;
	cg.hud.skillpics[2][2] = 16;

	cg.hud.skilltexts[0][0] = -1;
	cg.hud.skilltexts[0][1] = -1;
	cg.hud.skilltexts[0][2] = -1;
	cg.hud.skilltexts[1][0] = -1;
	cg.hud.skilltexts[1][1] = -1;
	cg.hud.skilltexts[1][2] = -1;
	cg.hud.skilltexts[2][0] = -1;
	cg.hud.skilltexts[2][1] = -1;
	cg.hud.skilltexts[2][2] = -1;
	cg.hud.votefttext[0] = 8;
	cg.hud.votefttext[1] = 200;
	cg.hud.xp[0] = 24;
	cg.hud.xp[1] = 472;
	cg.hud.xp[2] = 16;
	return qtrue;
}

#endif //__ETE__

qboolean CG_LoadBlankHud()
{
	cg.hud.ammo[0] = -1;
	cg.hud.chargebar[0] = -1;
	cg.hud.chattext[0] = -1;
	cg.hud.compass[0] = -1;
	cg.hud.cpmtext[0] = -1;
	cg.hud.draws[0] = -1;
	cg.hud.fireteam[0] = -1;
	cg.hud.flagcov[0] = -1;
	cg.hud.head[0] = -1;
	cg.hud.healthbar[0] = -1;
	cg.hud.hp[0] = -1;
	cg.hud.lagometer[0] = -1;
	cg.hud.livesleft[0] = -1;
	cg.hud.overheat[0] = -1;
	cg.hud.skillboxes[0][0] = -1;
	cg.hud.skillboxes[1][0] = -1;
	cg.hud.skillboxes[2][0] = -1;
	cg.hud.skillpics[0][0] = -1;
	cg.hud.skillpics[1][0] = -1;
	cg.hud.skillpics[2][0] = -1;
	cg.hud.skilltexts[0][0] = -1;
	cg.hud.skilltexts[1][0] = -1;
	cg.hud.skilltexts[2][0] = -1;
	cg.hud.staminabar[0] = -1;
	cg.hud.votefttext[0] = -1;
	cg.hud.weaponcard[0] = -1;
	cg.hud.xp[0] = -1;
	return qtrue;
}

void CG_EditHud()
{
	char hudElement[MAX_TOKEN_CHARS];
	char hudValue[5];

	if ( trap_Argc() < 3 ) {
		CG_Printf("^1usage: /edithud [elementName] [...]\n");
		return;
	}

	trap_Argv(1, hudElement, sizeof(hudElement));

	if(!Q_stricmp(hudElement, "upperright")) {
		if(trap_Argc() >= 3) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.draws[0] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "healthbar")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.healthbar[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.healthbar[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.healthbar[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "staminabar")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.staminabar[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.staminabar[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.staminabar[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "chargebar")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.chargebar[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.chargebar[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.chargebar[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "overheat")) {
		if(trap_Argc() >= 6) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.overheat[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.overheat[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.overheat[2] = atoi(hudValue);
			trap_Argv(5,hudValue,sizeof(hudValue));
			cg.hud.overheat[3] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "weaponcard")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.weaponcard[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.weaponcard[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.weaponcard[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "ammocount")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.ammo[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.ammo[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.ammo[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "head")) {
		if(trap_Argc() >= 6) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.head[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.head[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.head[2] = atoi(hudValue);
			trap_Argv(5,hudValue,sizeof(hudValue));
			cg.hud.head[3] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "fireteam")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.fireteam[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.fireteam[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.fireteam[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "xptext")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.xp[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.xp[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.xp[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "healthtext")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.hp[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.hp[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.hp[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "compass")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.compass[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.compass[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.compass[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "skillbox1")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.skillboxes[0][0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.skillboxes[0][1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.skillboxes[0][2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "skillbox2")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.skillboxes[1][0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.skillboxes[1][1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.skillboxes[1][2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "skillbox3")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.skillboxes[2][0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.skillboxes[2][1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.skillboxes[2][2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "skillpic1")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.skillpics[0][0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.skillpics[0][1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.skillpics[0][2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "skillpic2")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.skillpics[1][0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.skillpics[1][1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.skillpics[1][2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "skillpic3")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.skillpics[2][0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.skillpics[2][1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.skillpics[2][2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "skilltext1")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.skilltexts[0][0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.skilltexts[0][1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.skilltexts[0][2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "skilltext2")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.skilltexts[1][0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.skilltexts[1][1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.skilltexts[1][2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "skilltext3")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.skilltexts[2][0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.skilltexts[2][1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.skilltexts[2][2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "lagometer")) {
		if(trap_Argc() >= 4) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.lagometer[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.lagometer[1] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "flagcov")) {
		if(trap_Argc() >= 4) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.flagcov[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.flagcov[1] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "cpmtext")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.cpmtext[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.cpmtext[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.cpmtext[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "chattext")) {
		if(trap_Argc() >= 5) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.chattext[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.chattext[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.chattext[2] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "votefttext")) {
		if(trap_Argc() >= 4) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.votefttext[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.votefttext[1] = atoi(hudValue);
		}
	} else if (!Q_stricmp(hudElement, "livesleft")) {
		if(trap_Argc() >= 4) {
			trap_Argv(2,hudValue,sizeof(hudValue));
			cg.hud.livesleft[0] = atoi(hudValue);
			trap_Argv(3,hudValue,sizeof(hudValue));
			cg.hud.livesleft[1] = atoi(hudValue);
			trap_Argv(4,hudValue,sizeof(hudValue));
			cg.hud.livesleft[2] = atoi(hudValue);
		}
	}

	return;
}

void CG_DumpHud()
{
	CG_Printf("elements {\n");
	CG_Printf("  \"ammocount\" %d %d %d\n",
		cg.hud.ammo[0],cg.hud.ammo[1],cg.hud.ammo[2]);
	CG_Printf("  \"chargebar\" %d %d %d\n",
		cg.hud.chargebar[0],cg.hud.chargebar[1],cg.hud.chargebar[2]);
	CG_Printf("  \"chattext\" %d %d %d\n",
		cg.hud.chattext[0],cg.hud.chattext[1],cg.hud.chattext[2]);
	CG_Printf("  \"compass\" %d %d %d\n",
		cg.hud.compass[0],cg.hud.compass[1],cg.hud.compass[2]);
	CG_Printf("  \"cpmtext\" %d %d %d\n",
		cg.hud.cpmtext[0],cg.hud.cpmtext[1],cg.hud.cpmtext[2]);
	CG_Printf("  \"upperright\" %d\n",
		cg.hud.draws[0]);
	CG_Printf("  \"fireteam\" %d %d %d\n",
		cg.hud.fireteam[0],cg.hud.fireteam[1],cg.hud.fireteam[2]);
	CG_Printf("  \"flagcov\" %d %d\n",
		cg.hud.flagcov[0],cg.hud.flagcov[1]);
	CG_Printf("  \"head\" %d %d %d %d\n",
		cg.hud.head[0],cg.hud.head[1],cg.hud.head[2],cg.hud.head[3]);
	CG_Printf("  \"healthbar\" %d %d %d\n",
		cg.hud.healthbar[0],cg.hud.healthbar[1],cg.hud.healthbar[2]);
	CG_Printf("  \"healthtext\" %d %d %d\n",
		cg.hud.hp[0],cg.hud.hp[1],cg.hud.hp[2]);
	CG_Printf("  \"lagometer\" %d %d\n",
		cg.hud.lagometer[0],cg.hud.lagometer[1]);
	CG_Printf("  \"livesleft\" %d %d %d\n",
		cg.hud.livesleft[0],cg.hud.livesleft[1],cg.hud.livesleft[2]);
	CG_Printf("  \"overheat\" %d %d %d %d\n",
		cg.hud.overheat[0],cg.hud.overheat[1],
		cg.hud.overheat[2],cg.hud.overheat[3]);
	CG_Printf("  \"skillbox1\" %d %d %d\n",
		cg.hud.skillboxes[0][0],
		cg.hud.skillboxes[0][1],
		cg.hud.skillboxes[0][2]);
	CG_Printf("  \"skillbox2\" %d %d %d\n",
		cg.hud.skillboxes[1][0],
		cg.hud.skillboxes[1][1],
		cg.hud.skillboxes[1][2]);
	CG_Printf("  \"skillbox3\" %d %d %d\n",
		cg.hud.skillboxes[2][0],
		cg.hud.skillboxes[2][1],
		cg.hud.skillboxes[2][2]);
	CG_Printf("  \"skillpic1\" %d %d %d\n",
		cg.hud.skillpics[0][0],
		cg.hud.skillpics[0][1],
		cg.hud.skillpics[0][2]);
	CG_Printf("  \"skillpic2\" %d %d %d\n",
		cg.hud.skillpics[1][0],
		cg.hud.skillpics[1][1],
		cg.hud.skillpics[1][2]);
	CG_Printf("  \"skillpic3\" %d %d %d\n",
		cg.hud.skillpics[2][0],
		cg.hud.skillpics[2][1],
		cg.hud.skillpics[2][2]);
	CG_Printf("  \"skilltext1\" %d %d %d\n",
		cg.hud.skilltexts[0][0],
		cg.hud.skilltexts[0][1],
		cg.hud.skilltexts[0][2]);
	CG_Printf("  \"skilltext2\" %d %d %d\n",
		cg.hud.skilltexts[1][0],
		cg.hud.skilltexts[1][1],
		cg.hud.skilltexts[1][2]);
	CG_Printf("  \"skilltext3\" %d %d %d\n",
		cg.hud.skilltexts[2][0],
		cg.hud.skilltexts[2][1],
		cg.hud.skilltexts[2][2]);
	CG_Printf("  \"staminabar\" %d %d %d\n",
		cg.hud.staminabar[0],cg.hud.staminabar[1],cg.hud.staminabar[2]);
	CG_Printf("  \"votefttext\" %d %d\n",
		cg.hud.votefttext[0],cg.hud.votefttext[1]);
	CG_Printf("  \"weaponcard\" %d %d %d\n",
		cg.hud.weaponcard[0],cg.hud.weaponcard[1],cg.hud.weaponcard[2]);
	CG_Printf("  \"xptext\" %d %d %d\n",
		cg.hud.xp[0],cg.hud.xp[1],cg.hud.xp[2]);
	CG_Printf("}\n");
	return;
}

