#ifndef __H_TRANSPORTER
#define __H_TRANSPORTER


typedef struct TAG_TRANSPORTERINFO
{
	uint32			transportTo;
	float			xOffset;
	float			yOffset;
	float			zOffset;

	struct TAG_TRIGGERINFO		*onContact;
	struct TAG_TRIGGERINFO		*onReachingDestination;
	struct TAG_TRIGGERINFO		*onComplete;
	float			delay;
	bool			active;

	uint32			fadeUp;
	uint32			fadeDown;
} TRANSPORTERINFO, _TRANSPORTERINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateTransporter
   Purpose : Creates and initialises actorInstances that act as transporters
   Parameters : transportTo is CRC of object to transport to, name is for debug purposes only
   Returns : 
   Info : 
*/

int CreateTransporter(ACTORINSTANCE *transporter, uint32 transportTo, char *name, float xOffset, float yOffset, float zOffset, float delay, bool active = true, bool collision = true, uint32 fadeUp = FALSE, uint32 fadeDown = FALSE);

/* --------------------------------------------------------------------------------
   Function : CreateTransporterInfo
   Purpose : Creates and initialises transporter info part of object
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateTransporterInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RespondToTransporterCollision
   Purpose : calculates response when an actor collides
   Parameters : ACTORINSTANCE *spring, ACTORINSTANCE *collider
   Returns : 
   Info : 
*/

int RespondToTransporterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *transporter);

#endif
