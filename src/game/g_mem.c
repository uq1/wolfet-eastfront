//
// g_mem.c
//

#ifdef  __ID_MEM__

#include "g_local.h"
#include "bg_malloc.h"

// Ridah, increased this (fixes Dan's crash)

#define POOLSIZE	(4 * 1024 * 1024)

static char		memoryPool[POOLSIZE];
static int		allocPoint;

void *G_Alloc( int size ) {
	char	*p;

	if ( g_debugAlloc.integer ) {
		G_Printf( "G_Alloc of %i bytes (%i left)\n", size, POOLSIZE - allocPoint - ( ( size + 31 ) & ~31 ) );
	}

	if ( allocPoint + size > POOLSIZE ) {
		G_Error( "G_Alloc: failed on allocation of %u bytes\n", size );
		return NULL;
	}

	p = &memoryPool[allocPoint];

	allocPoint += ( size + 31 ) & ~31;

	return p;
}

void G_InitMemory( void ) 
{
	extern vmCvar_t	g_hunkMegs;

	allocPoint = 0;
	
	BG_MemoryInit(g_hunkMegs.integer);
}

void Svcmd_GameMem_f( void ) {
	G_Printf( "Game memory status: %i out of %i bytes allocated\n", allocPoint, POOLSIZE );
}

#else //!__ID_MEM__

//
// g_mem.c
//
#ifdef CGAMEDLL
#include "g_local.h"
#else //!CGAMEDLL
#include "g_local.h"
#endif //CGAMEDLL

void *G_Alloc( int size ) {
	return Q_malloc(size);
}

void G_InitMemory( void ) {
#ifdef CGAMEDLL
	extern vmCvar_t		cg_hunkMegs;
	int					ammount = cg_hunkMegs.integer;

	if (ammount < 192)
		ammount = 192;

	BG_MemoryClose();
	BG_MemoryInit(ammount);
#else //!CGAMEDLL
	extern vmCvar_t		g_hunkMegs;
	int					ammount = g_hunkMegs.integer;

	if (ammount < 80)
		ammount = 80;

	BG_MemoryClose();
	BG_MemoryInit(ammount);
#endif //CGAMEDLL
}

void Svcmd_GameMem_f( void ) {
#ifdef CGAMEDLL
	Com_Printf( "Game memory status: new system\n" );
#else //!CGAMEDLL
	G_Printf( "Game memory status: new system\n" );
#endif //CGAMEDLL
}

#endif // !__ID_MEM__
