#ifndef __H_ALLIGATOR
#define __H_ALLIGATOR

typedef struct TAG_ALLIGATORINFO
{
	TBVector		attackPoint;			// attack point for long range attacks
	uint32			attackObjectCRC;		// TP: crc of object to use as attack point
	char			contactWithPlayer;		// TRUE when the alligator is in contact with player
	float			jumpT;					// TP: t value specifying position in jump cycle
	TBVector		startPos;
	TBVector		p1,p2;
	TBVector		endPos;
	ACTORINSTANCE	*attackInstance;
	int				alliState;
} ALLIGATORINFO,_ALLIGATORINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceAlligator
   Purpose : Setup the basic ai for an alligator
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceAlligator(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList,uint32 attackPointCRC = NULL);

#endif