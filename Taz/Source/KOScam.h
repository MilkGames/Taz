// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
// PP: Adapted from Fred's Party-Game Rowing camera, which was a re-write of 
// PP:	my own Rowing camera anyway, so I feel less guilty somehow!
// PP:	It is the Great Circle of Life.
//
//      File : KOScam.h
//   Purpose : "Keep-On-Screen" camera - keeps its target actor instances on-screen at all times
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_KOSCAM
#define __H_KOSCAM

#include "LinkedList.h"					// PP: linked list class


// PP: type for targets of KOS-cameras
typedef struct TAG_KOSCAMTARGET
{
	TBActorInstance*		actorInstance;		// PP: actor instance to be kept on screen
	float					yOffset;			// PP: y offset from actor instance's origin to centre point to be kept on screen
	float					radius;				// PP: radius of visibility sphere around actor instance


	/* --------------------------------------------------------------------------------
	   Function		: KOSCAMTARGET::TAG_KOSCAMTARGET
	   Purpose		: constructor: create a new KOS cam target instance
	   Parameters	: TBActorInstance ptr, (opt/default)height offset, (opt/default)radius
	   Returns		: new KOS cam target instance
	   Info			: 
	*/
	TAG_KOSCAMTARGET(TBActorInstance* const actorInstanceIn, const float yOffsetIn, const float radiusIn)
	{
		this->actorInstance=actorInstanceIn;
		this->yOffset=yOffsetIn;
		this->radius=radiusIn;
	}


}KOSCAMTARGET, _KOSCAMTARGET BALIGN16;



#define KOSCAM_DEFAULT_ANGLE			0.0f
#define KOSCAM_DEFAULT_DIST				2000.0f
#define KOSCAM_DEFAULT_HEIGHT			2000.0f

#define KOSCAM_DEFAULT_Y_OFFSET			METERTOUNIT(1.5f)
#define KOSCAM_DEFAULT_RADIUS			METERTOUNIT(1.5f)

typedef struct TAG_KOSCAMDATA
{
	LINKEDLIST<KOSCAMTARGET>	targetList;			// PP: list of actor instances to keep on-screen
	TBVector					angle;				// PP: angle of camera

	
	/* --------------------------------------------------------------------------------
	   Function : KOSCAMDATA::TAG_KOSCAMDATA
	   Purpose : default constructor; does bugger all
	   Parameters : 
	   Returns : new KOSCAMDATA instance
	   Info : 
	*/	
	TAG_KOSCAMDATA(void);


	/* --------------------------------------------------------------------------------
	   Function : KOSCAMDATA::TAG_KOSCAMDATA
	   Purpose : constructor
	   Parameters : 
	   Returns : new KOSCAMDATA instance
	   Info : 
	*/
	TAG_KOSCAMDATA(const TBVector angleIn)
	{
		bmVectorCopy(this->angle, angleIn);
		this->targetList.init();
	}


	/* --------------------------------------------------------------------------------
	   Function : KOSCAMDATA::~TAG_KOSCAMDATA
	   Purpose : destructor - shuts down a KOSCAMDATA instance
	   Parameters : 
	   Returns : 
	   Info : 
	*/
	~TAG_KOSCAMDATA(void)
	{
		this->targetList.kill();
	}


	/* --------------------------------------------------------------------------------
	   Function		: KOSCAMDATA::addTarget
	   Purpose		: add a target to the list
	   Parameters	: TBActorInstance ptr, (opt/default)height offset, (opt/default)radius
	   Returns		: 
	   Info			: 
	*/
	inline void addTarget(TBActorInstance* const actorInstance, const float yOffset=KOSCAM_DEFAULT_Y_OFFSET, const float radius=KOSCAM_DEFAULT_RADIUS)
	{
		this->targetList.append(new KOSCAMTARGET(actorInstance, yOffset, radius));
	}


	/* --------------------------------------------------------------------------------
	   Function : KOSCAMDATA::numTargets
	   Purpose : get the number of targets in the target list
	   Parameters : 
	   Returns : the number of targets in the target list
	   Info : 
	*/
	inline int numTargets(void) const
	{
		return this->targetList.getSize();
	}


	/* --------------------------------------------------------------------------------
	   Function : KOSCAMDATA::getTarget
	   Purpose : get the target at the specified index
	   Parameters : index of target to get
	   Returns : the target at the specified index
	   Info : 
	*/
	inline KOSCAMTARGET* getTarget(const int index)
	{
		return this->targetList.get(index);
	}


}KOSCAMDATA , _KOSCAMDATA BALIGN16;


/* --------------------------------------------------------------------------------
   Function : KOScam_update
   Purpose : update the camera
   Parameters : ptr to camera case
   Returns : 
   Info : 
*/
void KOScam_update(CAMERACASE* const camCase);



#endif // PP: ndef __H_KOSCAM