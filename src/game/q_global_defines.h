
// ----------------------------------------------------------------------------------------
// Global defines for ETE and FRONTLINE. Enable/disable features here!
// ----------------------------------------------------------------------------------------

//
// ----------------------------------------------------------------------------------------
//

#ifndef __ETE_OPTIONS__
#define __ETE_OPTIONS__

#ifndef _DEBUG
#define _CRT_SECURE_NO_DEPRECATE // UQ1: Stop VC8 compile warnings...
#endif //_DEBUG

//
// ----------------------------------------------------------------------------------------
//

// UQ1: Disabled debug cvars by default (for ET 2.55 support), enable if you need them...
#define __DEBUGGING__

// Generic Stuff...
#ifndef USE_MDXFILE
#define USE_MDXFILE
#endif //USE_MDXFILE

// East Front
#define __EF__

// ET Enhanced Stuff...
#define __ETE__

#ifndef __SHELLSHOCK__
#define __SHELLSHOCK__
#define __SHELLSHOCK_FOG__
#define __SHELLSHOCK_SOUND__
#endif //__SHELLSHOCK__

#define __BLOODY_VIEW__

// UQ1: I want to draw a corona with gun flashes as well...
#define __FLASH_CORONAS__

#define __ENTITY_OVERRIDES__
#define __WEAPON_AIM__
#define __SUPREMACY__
#define __VEHICLES__
#define __PARTICLE_SYSTEM__

#ifdef CGAMEDLL
#ifdef _WIN32
#define __MUSIC_ENGINE__
#define __INETRADIO__
#endif // _WIN32
#endif //CGAMEDLL

// Frontline Stuff...
//#define __FRONTLINE__
//#define NWEAPS

// AI Stuff...
#define __NPC__
#define __NPC_STRAFE__
//#define __NPC_NAMES__ // UQ1: I think this was causing lag, disabling it!
//#define __NPC_NEW_COOP__ // UQ1: Experimental!
#define __BOT__
#define __NODE_SORT__
#define __BOT_STRAFE__
//#define __CORPSE_SCAN__
#define __ADVANCED_BOT_CHECKS__
//#define __MEDIC_WAYPOINTING__
#define __BOTS_USE_GRENADES__ // UQ1: This screws them up... WTF!!! -- Fixed???
#define __BOTS_USE_RIFLE_GRENADES__ // UQ1: This screws them up... WTF!!! -- Fixed???
//#define __BOT_EXPLOSIVE_AVOID__ // UQ1: Uses a little CPU time... OLD SYSTEM -- Don't use...
#define __BOT_EXPLOSIVE_AVOID_CPU_SAVER__ // UQ1: New system can use time savers to reduce load...
//#define __BOT_SHORTEN_ROUTING__ // UQ1: No longer needed...
//#define __BOT_NO_SUPRESSION_AMMO_CHEAT__
//#define __BOT_NO_FIELDOPS_AMMO_CHEAT__

#ifdef _WIN32
//#define __BOT_THREADING__ // UQ1: Disabled this because new ASTAR code uses shared memory (and should be faster/better anyway)
#endif // _WIN32

#define __AAS_BOTS__ // UQ1: Need to clean this out one day and remove the useless code...
#define NO_BOT_SUPPORT // UQ1: Useless original (broken) ET AI disabled...

//#define __DEBUGGING__

//
// ----------------------------------------------------------------------------------------
//

#if defined (_WIN32)

// UQ1: let's add some #pragma's there to disable extra warnings...

// warning C4706: assignment within conditional expression
#pragma warning( disable : 4706 )

// warning C4710: function 'XXXXX' not inlined
#pragma warning( disable : 4710 )

// warning C4221: nonstandard extension used : 'XXXXX' : cannot be initialized using address of automatic variable 'name'
#pragma warning( disable : 4221 )

// warning C4701: local variable 'XXXXX' may be used without having been initialized
#pragma warning( disable : 4701 )

// warning C4211: nonstandard extension used : redefined extern to static
#pragma warning( disable : 4211 )

// warning C4204: nonstandard extension used : non-constant aggregate initializer
#pragma warning( disable : 4204 )

// warning C4130: '==' : logical operation on address of string constant
#pragma warning( disable : 4130 )

// warning C4090: 'function' : different 'const' qualifiers
#pragma warning( disable : 4090 )

//
// Added: Vis Studio 2010 Code Analysis warnings...
//

// warning C6262: Function uses '524376' bytes of stack
#pragma warning( disable : 6262 )

// warning C6011: Dereferencing NULL pointer
#pragma warning( disable : 6011 )

// warning C6385: Invalid data: accessing 'xxx', the readable size is 'xxx' bytes, but 'xxx' bytes might be read
#pragma warning( disable : 6385 )

// warning C6387: 'argument x' might be '0': this does not adhere to the specification for the function 'xxx'
#pragma warning( disable : 6387 )

#endif // _WIN32

//
// ----------------------------------------------------------------------------------------
//

#endif //__ETE_OPTIONS__
