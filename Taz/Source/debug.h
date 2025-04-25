
// CPW: do an assert macro ...

#ifndef __DEBUG_H
#define __DEBUG_H

void debugAssert(char *sFile, int line, char *sExpr);
void erk(void);

#ifndef _TAZDEBUG
	#define ASSERT
	#define ASSERTM

	#define PFLOAT
	#define PVECTOR

	#define TRACE
#else
	#define ASSERT(A)		if(!(A))	debugAssert(__FILE__, __LINE__, #A)
	#define ASSERTM(A, B)	if(!(A))	debugAssert(__FILE__, __LINE__, B)

	#define PFLOAT(V)		bkPrintf("var "#V" = %f\n", (float)(V))
	#define PVECTOR(V)		bkPrintf("vec "#V" = (%f, %f, %f, %f)\n", (V)[0], (V)[1], (V)[2], (V)[3]);

	#define TRACE			bkPrintf("TRACE: "__FILE__", %d\n", __LINE__)
	#define ERK				erk()
#endif


#define ZEROCLASS			memset(this, 0, sizeof(*this))

void fmemcpy(void *pD, void *pS, size_t size);
void chrisRenderExtraSystem(TBExtraSystem *);

/*  --------------------------------------------------------------------------------
	Function	: SetupProfiler
	Purpose		: Setup the SN profiler
	Parameters	: 
	Returns		: 
	Info		: 
*/
void SetupProfiler();

//void *ZALLOC(size_t size);
#define ZALLOC MALLOC


#endif
