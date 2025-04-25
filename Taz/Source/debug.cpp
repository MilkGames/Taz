// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : debug.cpp
//   Purpose : debugging code
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "babel.h"

#if BPLATFORM==PS2
	#include "libsn.h"
	#include "consolewindow.h"
#endif

/*  --------------------------------------------------------------------------------
	Function	: PrintDebugVal
	Purpose		: custom printf
	Parameters	: 
	Returns		: 
	Info		: THIS FUNCTION MUST BE BEFORE INCLUDING GLOBAL.H
*/
void PrintDebugVal(char *info, ...)
{
	va_list	argList;

	va_start(argList, info);
	bkPrintf(info, argList);
	va_end(argList);
	va_start(argList, info);
#if BPLATFORM==PS2
	bdConsoleWindowPrintf(info, argList);
#endif
	va_end(argList);
}

#include "global.h"
#include "main.h"

char sMsg[128];

void debugAssert(char *sFile, int line, char *sExpr)
{
	if(!gameStatus.quitNow)
	{
		sprintf(sMsg, "ASSERT FAILED! in %s line %d, with a nasty: %s", sFile, line, sExpr);
		bkAlert(sMsg);
		bkPrintf("\n");

		gameStatus.quitNow = 1;
		exit(1);
	}
}

void erk(void)
{
	if(gameStatus.multiplayer.numPlayers == 1)
	{
		bkPrintf("ERK! numPlayers == 1\n");
		return;
	}

/*
	if(gameStatus.player[1] == NULL)
	{
		bkPrintf("ERK! gameStatus.player[1] == NULL\n");
		return;
	}
*/
	if(gameStatus.player[1].actorInstance == NULL)
	{
		bkPrintf("ERK! gameStatus.player[1].actorInstance == NULL\n");
		return;
	}

	bkPrintf("ERK! ");
	PVECTOR(gameStatus.player[1].actorInstance->actorInstance.orientation);
}

// TP: Removed for now as it prevents me knowing where the malloc actually happened
/*
void *ZALLOC(size_t size)
{
	void *p;

	p = MALLOC(size);
	if(!p)
		return 0;

	try
	{
		memset(p, 0xCD, size);
	}
	catch(...)
	{
		bkPrintf("ERROR! BABEL MALLOC allocated a block of unwriteable memory!\n");
		return 0;
	}

	return p;
}
*/
void fmemcpy(void *pD, void *pS, size_t size)
{
/*	int32 align = ((int32)pD) | ((int32)pS) | size;

	if((align&3) == 0)
	{	// CPW: goody, memory to be copied is on 4 byte boundaries (most is)

		uint c, off;

		off = 0;
		for(c=0; c<size; c+=4)
		{
			((int32 *)pD)[off] = ((int32 *)pS)[off];
			off++;
		}

		memcpy(pD, pS, size);
	}
	else
	{
		memcpy(pD, pS, size);
	}
*/

	memcpy(pD, pS, size);
}


extern TBExtraSystem		bExtraSystemList;

void chrisRenderExtraSystem(TBExtraSystem *eSystem)
{
	return;

	if (!eSystem)
	{
		// no system specified, so render them all
		for(eSystem = bExtraSystemList.next; eSystem != &bExtraSystemList; eSystem = eSystem->next)
		{
			//bkPrintf("Rendering %p ...", eSystem);
			chrisRenderExtraSystem(eSystem);
			//bkPrintf(" done.\n");
		}
		return;
	}

	if ((eSystem->renderFunc) && (eSystem->noofInstances))
	{
		if (eSystem->flags & BEXSYSFLAG_GLOBALEXTRAS)
			bdSetObjectMatrix(bIdentityMatrix);
		else
			bdSetObjectMatrix(eSystem->systemToWorld);
		eSystem->renderFunc(eSystem, eSystem->noofInstances, eSystem->dataPtrs, NULL);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: SetupProfiler
	Purpose		: Setup the SN profiler
	Parameters	: 
	Returns		: 
	Info		: 
*/
#if(BPLATFORM == PS2)// PP: ps2 only
void SetupProfiler()
{
	static u_long128 profdata[2048];
	int		retval;
	snDebugInit();
	sceSifInitRpc(0);
	if(sceSifLoadModule("host0:/usr/local/sce/iop/modules/SNProfil.irx", 0, NULL) < 0)
	{
	    printf("Can't load SNProfil module\n");
		exit(-1);
	}

	if(retval = snProfInit(_20KHZ, profdata, sizeof(profdata)) != 0)
	    printf("Profiler init failed\n");
}
#endif// PP: ps2