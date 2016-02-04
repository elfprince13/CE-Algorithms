#include <stdlib.h>
#include "misc.h"
#include "../debugConfig.h"


/***********************************************************************/
/*  FUNCTION:  SafeMalloc */
/**/
/*    INPUTS:  size is the size to SafeMalloc */
/**/
/*    OUTPUT:  returns pointer to allocated memory if succesful */
/**/
/*    EFFECT:  SafeMallocs new memory.  If SafeMalloc fails, prints error message */
/*             and terminates program. */
/**/
/*    Modifies Input: none */
/**/
/***********************************************************************/

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
