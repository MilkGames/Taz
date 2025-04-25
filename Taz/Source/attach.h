#ifndef __H_ATTACH
#define __H_ATTACH

#include "actors.h"
#define MAX_ATTACHED_OBJECTS (7)

#define ATTACHFLAG_ANIMATEWITHCHAR	(1<<0)
#define ATTACHFLAG_INHERITSCALE		(1<<1)
#define ATTACHFLAG_NOBLACKOUTLINE	(1<<2)
#define ATTACHFLAG_PRIMTYPEALPHA	(1<<3)

typedef struct TAG_ATTACH
{
	TBActorInstance		*objects[MAX_ATTACHED_OBJECTS];
	TBActorNodeInstance *node[MAX_ATTACHED_OBJECTS];
	TBVector				actualWorldPosition[MAX_ATTACHED_OBJECTS];
	uint32					flags[MAX_ATTACHED_OBJECTS];
	float				alpha[MAX_ATTACHED_OBJECTS];
} ATTACH;

/* --------------------------------------------------------------------------------
   Function : InitAttachedInfo
   Purpose : Initialise the attached info structure
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void InitAttachedInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DestroyAttachedInfo
   Purpose : Remove all attached objects and resets the structure
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void DestroyAttachedInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AttachObjectToCharacter
   Purpose : Create an actor instance and attach to a characters bone with offsets
   Parameters : actor, object to attach, bone to attach to, pos offset, scale, rot, flags
   Returns : handle to attached object, -1 for fail
   Info : 
*/
int AttachObjectToCharacter(ACTORINSTANCE *actorInstance, char *object, char *bone, TBVector pos, TBVector scale, TBQuaternion quat, int flags);

/* --------------------------------------------------------------------------------
   Function : SetAttachedObjectAlpha
   Purpose : sets the alpha on an attached object
   Parameters : 
   Returns : 
   Info : 
*/

int	SetAttachedObjectAlpha(ACTORINSTANCE *actorInstance, int handle, float alpha);

/* --------------------------------------------------------------------------------
   Function : SetAttachedObjectAlpha
   Purpose : sets the alpha on an attached object
   Parameters : 
   Returns : 
   Info : 
*/

int	AddAttachedObjectFlags(ACTORINSTANCE *actorInstance, int handle, uint32	flag);

/* --------------------------------------------------------------------------------
   Function : SetAttachedObjectAlpha
   Purpose : sets the alpha on an attached object
   Parameters : 
   Returns : 
   Info : 
*/

int	RemoveAttachedObjectFlags(ACTORINSTANCE *actorInstance, int handle, uint32	flag);

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromCharacter
   Purpose : Remove an object from the character using the handle returned from attach
   Parameters : actor, handle
   Returns : 
   Info : 
*/
void RemoveObjectFromCharacter(ACTORINSTANCE *actorInstance, int handle);

/* --------------------------------------------------------------------------------
   Function : PrepAttachedObjects
   Purpose : Prepare the attached objects for drawing
   Parameters : actor
   Returns : 
   Info : 
*/
void PrepAttachedObjects(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DrawAttachedObjects
   Purpose : Draw all objects attached to an actor
   Parameters : actor
   Returns : 
   Info : 
*/
void DrawAttachedObjects(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DrawAttachedObjects
   Purpose : Draw all objects attached to an actor
   Parameters : actor
   Returns : 
   Info : 
*/
void DrawAttachedObjects2(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateAttachedObjects
   Purpose : Update all objects attached to an actor
   Parameters : actor
   Returns : 
   Info : 
*/
void UpdateAttachedObjects(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ReturnAttachedObjectInstance
   Purpose : Returns a pointer to the attached objects actorInstance;
   Parameters : ACTORINSTANCE, handle
   Returns : TBActorInstance *
   Info : 
*/
TBActorInstance *ReturnAttachedObjectInstance(ACTORINSTANCE *actorInstance, int handle);

/* --------------------------------------------------------------------------------
   Function		: FindAttachedObjectInstance
   Purpose		: Finds out if a particular TBActorInstnance is attached to a particular ACTORINSTANCE
   Parameters	: ACTORINSTANCE of owner, TBActorInstance of object
   Returns		: true if the object is attached
   Info			: // PP: 
*/
bool FindAttachedObjectInstance(const ACTORINSTANCE* const owner, const TBActorInstance* const object);

/* --------------------------------------------------------------------------------
   Function : GetAttachedObjectsWorldPosition
   Purpose : Returns the actual world position of an attached object
   Parameters : ACTORINSTANCE, handle, pos
   Returns : 
   Info : 
*/
void GetAttachedObjectsWorldPosition(ACTORINSTANCE *actorInstance, int handle, TBVector pos);

/* --------------------------------------------------------------------------------
   Function : AnimateAttachedObjects
   Purpose : Animate any objects attached
   Parameters : 
   Returns : 
   Info : 
*/
void AnimateAttachedObjects(ACTORINSTANCE *actorInstance,TBActorAnimSegment *animation,int speed,float blend,char loop,void *priority);

/* --------------------------------------------------------------------------------
   Function : FlushAttachedObjects
   Purpose : Flush the animation for all attached objects
   Parameters : 
   Returns : 
   Info : 
*/
void FlushAttachedObjects(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : GetAttachedObjectsWorldPosition
   Purpose : Returns the actual world position of an attached object
   Parameters : ACTORINSTANCE, name, pos
   Returns : 
   Info : 
*/
void GetAttachedObjectsWorldPosition(ACTORINSTANCE *actorInstance, char *object, TBVector pos);

/* --------------------------------------------------------------------------------
   Function : GetAttachedObjectsHandle
   Purpose : Returns the handle for the first attached object of the type object
   Parameters : ACTORINSTANCE, name
   Returns : handle number
   Info : 
*/
int	GetAttachedObjectsHandle(ACTORINSTANCE *actorInstance, char *object);

#endif