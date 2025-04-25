#ifndef __H_EATING
#define __H_EATING

#include "linkedlist.h"

typedef struct TAG_EATITEM
{
	TBActorInstance		*item;			// TP: Object that taz is trying to eat
} EATITEM, _EATITEM BALIGN16;

typedef class TAG_EATING
{
	LINKEDLIST<TAG_EATITEM>		list;		// TP: list of objects currently being eaten
	TBVector					destPos;	// TP: current position of bone for eaten object to head to
	TBVector					origPos;	// TP: position of object before being eaten
	TBQuaternion				origQuat;	// TP: quat of object before being eaten

public:

	/*  --------------------------------------------------------------------------------
		Function	: EATING::TAG_EATING
		Purpose		: Default constructor
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	TAG_EATING();

	/*  --------------------------------------------------------------------------------
		Function	: EATING::~TAG_EATING
		Purpose		: Default constructor
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	~TAG_EATING();

	/*  --------------------------------------------------------------------------------
		Function	: EATING::AddItem
		Purpose		: Add an item to the eat list
		Parameters	: TBActorInstance pointer to the object to be eaten
		Returns		: 
		Info		: 
	*/
	void AddItem(TBActorInstance *actorInstance);

	/*  --------------------------------------------------------------------------------
		Function	: EATING::Update
		Purpose		: Update any objects on the eat list
		Parameters	: ACTORINSTANCE of character eating ie. Taz
		Returns		: 
		Info		: 
	*/
	void Update(ACTORINSTANCE *actorInstance);

	/*  --------------------------------------------------------------------------------
		Function	: EATING::Draw
		Purpose		: Draw any objects currently on the eat list
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Draw();

} EATING, _EATING BALIGN16;

#endif