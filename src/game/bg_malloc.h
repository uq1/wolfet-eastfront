#ifndef _QMALLOC_H_
#define _QMALLOC_H_

#include "q_shared.h"

void*	Q_calloc(size_t nmemb, size_t size);
void*	Q_malloc(size_t size);
void	Q_free(void *ptr);
void*	Q_realloc(void *ptr, size_t size);
void	BG_MemoryInit(int hunkMegs);
void	BG_MemoryClose();
qboolean BG_IsMemoryInitialized();

#endif
