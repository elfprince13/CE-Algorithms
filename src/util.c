#include <stdlib.h>
#include "util.h"
#include "debugConfig.h"


void * SafeMalloc(size_t size) {
	void *const mem = malloc(size);
	
	if ( !mem ) {
#ifdef _EZ80
		cleanUp();
		asm("	LD	IY, 13631616");
		asm("JP _ErrMemory");
#else
		dPrintf(("memory overflow: SafeMalloc failed in SafeMalloc."));
		dPrintf(("  Exiting Program.\n"));
		exit(-1);
#endif
	}
	return mem;
}

void cleanUp()
{
#ifdef _EZ80
	// Clear/invalidate some RAM areas
	// and restore the home screen nicely
	_OS( asm("CALL _DelRes");
	asm("CALL _ClrTxtShd");
	asm("CALL _ClrScrn");
	asm("SET  graphDraw,(iy+graphFlags)");
	asm("CALL _HomeUp");
	asm("CALL _DrawStatusBar");
	);
#endif
}

int waitKey(){
	int ret;
#ifdef _EZ80
	_OS( ret = GetKey() );
#else
	ret = getChar();
#endif
	return ret;
}

