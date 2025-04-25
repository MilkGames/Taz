#ifndef __H_TRAIL
#define __H_TRAIL

#define TRAIL_MAXBUFFERSIZE		(10)
#define TRAIL_MAXSMALLINDICES	(16)
#define TRAIL_STEPSIZE			(0.5f)

#define TRAIL_RESET_ALL			(0)
#define TRAIL_RESET_CAMIGNORE	(1)


void trailSetUp(ACTORINSTANCE *);
void trailReset(ACTORINSTANCE *, int action);
void trailFree(ACTORINSTANCE *pActorInstance);
void trailAddPosition(ACTORINSTANCE *pActorInstance);
void trailDraw(ACTORINSTANCE *pActorInstance);
int trailGetPosFromFramesAgo(ACTORINSTANCE *pActorInstance, TBVector pPos, int framesAgo);
int trailFindNearestPoint(ACTORINSTANCE *pActorInstance, TBVector pPoint, TBVector pDest);
int trailFindBestPoint(ACTORINSTANCE *pActorInstance, TBVector pPoint, TBVector pDest);
int trailGetIndexedPoint(ACTORINSTANCE *pActorInstance, int index, TBVector pPoint);
void trailAdvanceToNext(ACTORINSTANCE *pActorInstance, int *pIndex);
float trailHowFarBehind(ACTORINSTANCE *pActorInstance, TBVector);

#endif
