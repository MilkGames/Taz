// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : eating.cpp
//   Purpose : Functions for showing Taz eating objects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "eating.h"
#include "maths.h"

/*  --------------------------------------------------------------------------------
	Function	: EATING::TAG_EATING
	Purpose		: Default constructor
	Parameters	: 
	Returns		: 
	Info		: 
*/
EATING::TAG_EATING()
{
}

/*  --------------------------------------------------------------------------------
	Function	: EATING::~TAG_EATING
	Purpose		: Default constructor
	Parameters	: 
	Returns		: 
	Info		: 
*/
EATING::~TAG_EATING()
{
	EATITEM		*item;
	int			i;

	for (i=list.getSize()-1;i>=0;i--)
	{
		item = list.get(i);

		baFreeActorInstance(item->item);

		SAFE_FREE(item);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: EATING::AddItem
	Purpose		: Add an item to the eat list
	Parameters	: TBActorInstance pointer to the object to be eaten
	Returns		: 
	Info		: 
*/
void EATING::AddItem(TBActorInstance *actorInstance)
{
	EATITEM		*item;

	if (!actorInstance) return;

	item = (EATITEM*)ZALLOC(sizeof(EATITEM));

	if (!item) return;

	item->item = actorInstance;
	bmVectorCopy(origPos,actorInstance->position);
	bmQuatCopy(origQuat,actorInstance->orientation);

	list.append(item);
}

/*  --------------------------------------------------------------------------------
	Function	: EATING::Update
	Purpose		: Update any objects on the eat list
	Parameters	: ACTORINSTANCE of character eating ie. Taz
	Returns		: 
	Info		: 
*/
void EATING::Update(ACTORINSTANCE *actorInstance)
{
	EATITEM					*item;
	int						i;
	TBActorNodeInstance		*node;
	TBQuaternion			tempQuat;
	TBVector				tempVector;

	if (list.getSize()==0) return;

	node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"snoutstem");

	if (node)
	{
		// TP: if there is a node then eat
		baGetNodesWorldPosition(&actorInstance->actorInstance,node,destPos);
		destPos[Y] -= METERTOUNIT(0.15f);
		bmVectorCopy(tempVector,destPos);
		tempVector[Y] -= METERTOUNIT(0.25f);
	}

	for (i=list.getSize()-1;i>=0;i--)
	{
		item = list.get(i);

		if (!node)
		{
			baFreeActorInstance(item->item);
			SAFE_FREE(item->item);
			list.remove(item);
			SAFE_FREE(item);
			continue;
		}

		bmVectorSub(tempVector, item->item->position, tempVector);
		float len;
		len = bmVectorLen(tempVector);
		if (len)
		{
			SETVECTOR(tempVector,0.0f,1.0f,0.0f,1.0f);
			bmDirectionToQuat(tempQuat, tempVector,0.0f);
		}
		else
		{
			SETVECTOR(tempQuat, 0.0f,0.0f,0.0f,1.0f);
		}

		if (item->item->scale[X] < 1.0f)
		{
			bmVectorLerp(item->item->position, origPos, destPos, 1.0f-item->item->scale[X]);
			bmQuatSlerp(item->item->orientation, origQuat, tempQuat, 1.0f-item->item->scale[X]);
		}
		bmVectorCDiv(item->item->scale, item->item->scale, 1.1f);

		// TP: update
		baPrepActorInstance(item->item, TRUE);

		if (item->item->scale[X] < 0.1f)
		{
			// TP: delete
			baFreeActorInstance(item->item);
			SAFE_FREE(item->item);
			list.remove(item);
			SAFE_FREE(item);
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: EATING::Draw
	Purpose		: Draw any objects currently on the eat list
	Parameters	: 
	Returns		: 
	Info		: 
*/
void EATING::Draw()
{
	EATITEM					*item;
	int						i;

	for (i=list.getSize()-1;i>=0;i--)
	{
		item = list.get(i);

		// TP: draw
		baDrawActorInstance(item->item);
	}
}


