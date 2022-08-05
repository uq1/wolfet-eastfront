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

#ifdef __UNUSED_MALLOC__
#include "../game/g_local.h"
#include "../game/q_shared.h"
#ifndef CGAMEDLL
#ifdef GAMEDLL
#include "../game/botlib.h"
#include "../botai/ai_main.h"
#endif //GAMEDLL
#endif //CGAMEDLL

//#define MAX_POOL_SIZE	1024 * 16384
//#define MAX_POOL_SIZE	1024 * 65536
//#define MAX_POOL_SIZE	1024 * 96000
#define MAX_POOL_SIZE	1024 * 1024 * 27//30//72

//I am using this for all the stuff like NPC client structures on server/client and
//non-humanoid animations as well until/if I can get dynamic memory working properly
//with casted datatypes, which is why it is so large.
static char bg_pool[MAX_POOL_SIZE];
static int	bg_poolSize = 0;
static int	bg_poolTail = MAX_POOL_SIZE;


/* */
void *
BG_Alloc ( int size )
{
	bg_poolSize = ( (bg_poolSize + 0x00000003) & 0xfffffffc );
	if ( bg_poolSize + size > bg_poolTail )
	{
#ifdef _DEBUG

		//		assert(0);
#endif //_DEBUG
		Com_Error( ERR_DROP, "BG_Alloc: buffer exceeded tail (%d > %d)", bg_poolSize + size, bg_poolTail );
		return ( 0 );
	}

	bg_poolSize += size;
	return ( &bg_pool[bg_poolSize - size] );
}


/* */
void *
BG_DebugAlloc ( int size, char *procedure )
{
	bg_poolSize = ( (bg_poolSize + 0x00000003) & 0xfffffffc );
	if ( bg_poolSize + size > bg_poolTail )
	{
#ifdef _DEBUG

		//assert(0);
#endif //_DEBUG
		Com_Error( ERR_DROP, "BG_Alloc: buffer (from %s) exceeded tail (%d > %d)", procedure, bg_poolSize + size,
				   bg_poolTail );
		return ( 0 );
	}

	bg_poolSize += size;
	return ( &bg_pool[bg_poolSize - size] );
}


/* */
void *
BG_AllocUnaligned ( int size )
{
	if ( bg_poolSize + size > bg_poolTail )
	{
		Com_Error( ERR_DROP, "BG_AllocUnaligned: buffer exceeded tail (%d > %d)", bg_poolSize + size, bg_poolTail );
#ifdef _DEBUG
		assert( 0 );
#endif //_DEBUG
		return ( 0 );
	}

	bg_poolSize += size;
	return ( &bg_pool[bg_poolSize - size] );
}


/* */
void *
BG_TempAlloc ( int size )
{
	size = ( (size + 0x00000003) & 0xfffffffc );
	if ( bg_poolTail - size < bg_poolSize )
	{
		Com_Error( ERR_DROP, "BG_TempAlloc: buffer exceeded head (%d > %d)", bg_poolTail - size, bg_poolSize );
#ifdef _DEBUG
		assert( 0 );
#endif //_DEBUG
		return ( 0 );
	}

	bg_poolTail -= size;
	return ( &bg_pool[bg_poolTail] );
}


/* */
void
BG_TempFree ( int size )
{
	size = ( (size + 0x00000003) & 0xfffffffc );
	if ( bg_poolTail + size > MAX_POOL_SIZE )
	{
		Com_Error( ERR_DROP, "BG_TempFree: tail greater than size (%d > %d)", bg_poolTail + size, MAX_POOL_SIZE );
#ifdef _DEBUG
		assert( 0 );
#endif //_DEBUG
	}

	bg_poolTail += size;
}


/* */
char *
BG_StringAlloc ( const char *source )
{
	char	*dest;
	dest = BG_Alloc( strlen( source) + 1 );
	strcpy( dest, source );
	return ( dest );
}


/* */
qboolean
BG_OutOfMemory ( void )
{
	return ( bg_poolSize >= MAX_POOL_SIZE );
}


/* */
qboolean
BG_AvailableMemory ( void )
{
	return ( MAX_POOL_SIZE - bg_poolSize );
}

#ifdef BOT_ZMALLOC
#define MAX_BALLOC	8192
void	*BAllocList[MAX_BALLOC];
#endif
#define MAX_CHAT_BUFFER_SIZE	256
char	gBotChatBuffer[MAX_CLIENTS][MAX_CHAT_BUFFER_SIZE];


/* */
void *
B_TempAlloc ( int size )
{
	return ( BG_TempAlloc( size) );
}


/* */
void
B_TempFree ( int size )
{
	BG_TempFree( size );
}


/* */
void *
B_Alloc ( int size )
{
#ifdef BOT_ZMALLOC
	void	*ptr = NULL;
	int		i = 0;
#ifdef BOTMEMTRACK
	int		free = 0;
	int		used = 0;
	while ( i < MAX_BALLOC )
	{
		if ( !BAllocList[i] )
		{
			free++;
		}
		else
		{
			used++;
		}

		i++;
	}

	G_Printf( "Allocations used: %i\nFree allocation slots: %i\n", used, free );
	i = 0;
#endif
	ptr = trap_BotGetMemoryGame( size );
	while ( i < MAX_BALLOC )
	{
		if ( !BAllocList[i] )
		{
			BAllocList[i] = ptr;
			break;
		}

		i++;
	}

	if ( i == MAX_BALLOC )
	{

		//If this happens we'll have to rely on this chunk being freed manually with B_Free, which it hopefully will be
#ifdef DEBUG
		G_Printf( "WARNING: MAXIMUM B_ALLOC ALLOCATIONS EXCEEDED\n" );
#endif
	}

	return ( ptr );
#else
	return ( BG_Alloc( size) );
#endif
}


/* */
void
B_Free ( void *ptr )
{
#ifdef BOT_ZMALLOC
	int i = 0;
#ifdef BOTMEMTRACK
	int free = 0;
	int used = 0;
	while ( i < MAX_BALLOC )
	{
		if ( !BAllocList[i] )
		{
			free++;
		}
		else
		{
			used++;
		}

		i++;
	}

	G_Printf( "Allocations used: %i\nFree allocation slots: %i\n", used, free );
	i = 0;
#endif
	while ( i < MAX_BALLOC )
	{
		if ( BAllocList[i] == ptr )
		{
			BAllocList[i] = NULL;
			break;
		}

		i++;
	}

	if ( i == MAX_BALLOC )
	{

		//Likely because the limit was exceeded and we're now freeing the chunk manually as we hoped would happen
#ifdef DEBUG
		G_Printf( "WARNING: Freeing allocation which is not in the allocation structure\n" );
#endif
	}

	trap_BotFreeMemoryGame( ptr );
#endif
}


/* */
void
B_InitAlloc ( void )
{
#ifdef BOT_ZMALLOC
	memset( BAllocList, 0, sizeof(BAllocList) );
#endif

	//	memset(gWPArray, 0, sizeof(gWPArray));
}


/* */
void
BG_Free ( void *ptr )
{
	B_Free( ptr );
}


/* */
void
B_CleanupAlloc ( void )
{
#ifdef BOT_ZMALLOC
	int i = 0;
	while ( i < MAX_BALLOC )
	{
		if ( BAllocList[i] )
		{
			trap_BotFreeMemoryGame( BAllocList[i] );
			BAllocList[i] = NULL;
		}

		i++;
	}
#endif
}


/* */
int
GetValueGroup ( char *buf, char *group, char *outbuf )
{
	char	*place, *placesecond;
	int		iplace;
	int		failure;
	int		i;
	int		startpoint, startletter;
	int		subg = 0;
	i = 0;
	iplace = 0;
	place = strstr( buf, group );
	if ( !place )
	{
		return ( 0 );
	}

	startpoint = place - buf + strlen( group ) + 1;
	startletter = ( place - buf ) - 1;
	failure = 0;
	while ( buf[startpoint + 1] != '{' || buf[startletter] != '\n' )
	{
		placesecond = strstr( place + 1, group );
		if ( placesecond )
		{
			startpoint += ( placesecond - place );
			startletter += ( placesecond - place );
			place = placesecond;
		}
		else
		{
			failure = 1;
			break;
		}
	}

	if ( failure )
	{
		return ( 0 );
	}

	//we have found the proper group name if we made it here, so find the opening brace and read into the outbuf
	//until hitting the end brace
	while ( buf[startpoint] != '{' )
	{
		startpoint++;
	}

	startpoint++;
	while ( buf[startpoint] != '}' || subg )
	{
		if ( buf[startpoint] == '{' )
		{
			subg++;
		}
		else if ( buf[startpoint] == '}' )
		{
			subg--;
		}

		outbuf[i] = buf[startpoint];
		i++;
		startpoint++;
	}

	outbuf[i] = '\0';
	return ( 1 );
}


/* */
int
GetPairedValue ( char *buf, char *key, char *outbuf )
{
	char	*place, *placesecond;
	int		startpoint, startletter;
	int		i, found;
	if ( !buf || !key || !outbuf )
	{
		return ( 0 );
	}

	i = 0;
	while ( buf[i] && buf[i] != '\0' )
	{
		if ( buf[i] == '/' )
		{
			if ( buf[i + 1] && buf[i + 1] != '\0' && buf[i + 1] == '/' )
			{
				while ( buf[i] != '\n' )
				{
					buf[i] = '/';
					i++;
				}
			}
		}

		i++;
	}

	place = strstr( buf, key );
	if ( !place )
	{
		return ( 0 );
	}

	//tab == 9
	startpoint = place - buf + strlen( key );
	startletter = ( place - buf ) - 1;
	found = 0;
	while ( !found )
	{
		if
		(
			startletter == 0 ||
			!buf[startletter] ||
			buf[startletter] == '\0' ||
			buf[startletter] == 9 ||
			buf[startletter] == ' ' ||
			buf[startletter] == '\n'
		)
		{
			if ( buf[startpoint] == '\0' || buf[startpoint] == 9 || buf[startpoint] == ' ' || buf[startpoint] == '\n' )
			{
				found = 1;
				break;
			}
		}

		placesecond = strstr( place + 1, key );
		if ( placesecond )
		{
			startpoint += placesecond - place;
			startletter += placesecond - place;
			place = placesecond;
		}
		else
		{
			place = NULL;
			break;
		}
	}

	if ( !found || !place || !buf[startpoint] || buf[startpoint] == '\0' )
	{
		return ( 0 );
	}

	while ( buf[startpoint] == ' ' || buf[startpoint] == 9 || buf[startpoint] == '\n' )
	{
		startpoint++;
	}

	i = 0;
	while ( buf[startpoint] && buf[startpoint] != '\0' && buf[startpoint] != '\n' )
	{
		outbuf[i] = buf[startpoint];
		i++;
		startpoint++;
	}

	outbuf[i] = '\0';
	return ( 1 );
}

#else //!__UNUSED_MALLOC__

#include "../game/g_local.h"
#include "../game/q_shared.h"

extern void Q_free(void *ptr);
extern void* Q_malloc(size_t size);

void *B_Alloc ( int size )
{
	return Q_malloc(size);
}

void *BG_Alloc ( int size )
{
	return Q_malloc(size);
}

void B_Free ( void *ptr )
{
	Q_free(ptr);
}

void BG_Free ( void *ptr )
{
	Q_free(ptr);
}

//
// UQ1: The following is memcpy that uses SSE/SSE2 instructions for speed...
//

#ifdef _WIN32

#pragma warning(push)
#pragma warning(disable:4018 4102)

int sse_memcpy(int* piDst, int* piSrc, unsigned long SizeInBytes)
{
//	unsigned long dwNumElements = SizeInBytes / sizeof(int);
	// not really using it, just for debuging. it keeps number of looping. 
	// it also means number of packed data.
	//unsigned long dwNumPacks = dwNumElements / (128/(sizeof(int)*8));

	_asm
	{
	// remember for cleanup
	pusha;
begin:
	// init counter to SizeInBytes
	mov  ecx,SizeInBytes;
	// get destination pointer
	mov  edi,piDst;
	// get source pointer
	mov  esi,piSrc;
begina:
	// check if counter is 0, yes end loop.
	cmp  ecx,0;
	jz  end;
body:
	// calculate offset
	mov  ebx,SizeInBytes;
	sub  ebx,ecx;
	// copy source's content to 128 bits registers
	movdqa xmm1,[esi+ebx];
	// copy 128 bits registers to destination
	movdqa [edi+ebx],xmm1;

bodya:
	// we've done "1 packed == 4 * sizeof(int)" already.
	sub  ecx,16;
	jmp  begina;
end:
	// cleanup
	popa;
 }

 return 0;
}

void sse_memset(void *dst, int n32, unsigned long i)
{
	__asm {
		movq mm0, n32
		punpckldq mm0, mm0
		mov edi, dst

loopwrite:

		movntq 0[edi], mm0
		movntq 8[edi], mm0
		//movntq 16[edi], mm0
		//movntq 24[edi], mm0
		//movntq 32[edi], mm0
		//movntq 40[edi], mm0
		//movntq 48[edi], mm0
		//movntq 56[edi], mm0

		add edi, 16
		sub i, 2
		jg loopwrite

		emms
	}
}
#else //!_WIN32
int sse_memcpy(int* piDst, int* piSrc, unsigned long SizeInBytes)
{
	memcpy(piDst, piSrc, SizeInBytes*8);
	return 0;
}

void sse_memset(void *dst, int n32, unsigned long i)
{
	memset (dst, n32, i*8);
}

#endif //_WIN32

#endif //__UNUSED_MALLOC__