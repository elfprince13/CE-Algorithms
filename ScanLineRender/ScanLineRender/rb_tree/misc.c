#include <stdlib.h>
#include "misc.h"
#include "../debugConfig.h"


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

void * SafeMalloc(size_t size) {
  void * result;

  if ( (result = malloc(size)) ) { /* assignment intentional */
    return(result);
  } else {
    dPrintf(("memory overflow: malloc failed in SafeMalloc."));
    dPrintf(("  Exiting Program.\n"));
    exit(-1);
    return(0);
  }
}
