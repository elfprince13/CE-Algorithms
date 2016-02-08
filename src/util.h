#ifndef INC_E_UTIL_
#define INC_E_UTIL_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef _EZ80
#include <ti84pce.h>
#endif

void * SafeMalloc(size_t);
void cleanUp(void);
int waitKey(void);


#endif








