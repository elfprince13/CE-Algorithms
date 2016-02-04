#include <stdlib.h>
#include "misc.h"
#include "debugConfig.h"


/***********************************************************************/
/*  FUNCTION:  SafeMalloc */
/**/
/*    INPUTS:  size is the size to malloc */
/**/
/*    OUTPUT:  returns pointer to allocated memory if succesful */
/**/
/*    EFFECT:  mallocs new memory.  If malloc fails, prints error message */
/*             and terminates program. */
/**/
/*    Modifies Input: none */
/**/
/***********************************************************************/

#ifdef _EZ80
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <ti84pce.h>
extern void cleanUp(void);
#endif

void * SafeMalloc(size_t size) {
	void *const mem = malloc(size);
	
	if ( !mem ) {
#ifdef _EZ80
		cleanUp();
		_OS( asm("JP _ErrMemory") );
#else
		dPrintf(("memory overflow: SafeMalloc failed in SafeMalloc."));
		dPrintf(("  Exiting Program.\n"));
		exit(-1);
#endif
	}
	return mem;
}
