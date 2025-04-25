#ifndef __H_EFFECTS
#define __H_EFFECTS

#include "linkedlist.h"

enum
{
	EFFECT_CHIMNEYPUFF,
};

typedef struct TAG_EFFECTSLISTENTRY
{
	TBVector				startPos;

	TBVector				velocity;

	uint32					type;
	float					period;
	TAG_EFFECTSLISTENTRY	*next;
	TAG_EFFECTSLISTENTRY	*prev;

	float					clock;
	uint32					pad[3];

} EFFECTSLISTENTRY, _EFFECTSLISTENTRY BALIGN16;

typedef struct TAG_EFFECTSLIST
{
	EFFECTSLISTENTRY	head;

	uint32				numEntries;
	uint32				pad[3];
} EFFECTSLIST, _EFFECTSLIST BALIGN16;

extern	EFFECTSLIST		effectsList;

typedef class TAG_MIRROR
{
private:
	static LINKEDLIST<TAG_MIRROR>		mirrorList;
	ACTORINSTANCE						*actorInstance;
	TBCollisionLine						mirrorPlane;

	/*  --------------------------------------------------------------------------------
		Function	: MIRROR::DrawInstance
		Purpose		: Draws the mirror version of an object
		Parameters	: ActorInstance, 2d line to mirror in, flags
		Returns		: 
		Info		: 
	*/
	void DrawInstance(struct TAG_ACTORINSTANCE *actorInstance, uint32 flags);

public:
	/*  --------------------------------------------------------------------------------
		Function	: FreeAllMirrors
		Purpose		: Free all the mirrors on a level
		Parameters	: 
		Returns		: 
		Info		: should be called before freeing the actorInstance lists!
	*/
	friend void FreeAllMirrors(void);

	/*  --------------------------------------------------------------------------------
		Function	: DrawAllMirrors
		Purpose		: Draw all mirrors affected by actorInstance
		Parameters	: actorInstance
		Returns		: 
		Info		: should be called before freeing the actorInstance lists!
	*/
	friend void DrawAllMirrors(ACTORINSTANCE *actorInstance);

	/*  --------------------------------------------------------------------------------
		Function	: FreeMirror
		Purpose		: Free a mirror
		Parameters	: actorInstance driving the mirror
		Returns		: 
		Info		: 
	*/
	friend void FreeMirror(ACTORINSTANCE *actorInstance);


	/*  --------------------------------------------------------------------------------
		Function	: MIRROR::TAG_MIRROR
		Purpose		: Default constructor
		Parameters	: actorInstance of mirror
		Returns		: 
		Info		: 
	*/
	TAG_MIRROR(struct TAG_ACTORINSTANCE *actorInstance);

	/*  --------------------------------------------------------------------------------
		Function	: MIRROR::~TAG_MIRROR
		Purpose		: Default destructor
		Parameters	: actorInstance of mirror
		Returns		: 
		Info		: 
	*/
	~TAG_MIRROR()
	{
		mirrorList.remove(this);
	}
} MIRROR, _MIRROR BALIGN16;

/* --------------------------------------------------------------------------------
   Function : InitEffectsList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void InitEffectsList(void);

/* --------------------------------------------------------------------------------
   Function : AddEntryToEffectsList
   Purpose : Function to store information about effects
   Parameters : 
   Returns : 
   Info : 
*/

void AddEntryToEffectsList(TBVector startPos, TBVector velocity, float period, uint32 type);

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromEffectsList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RemoveObjectFromEffectsList(EFFECTSLISTENTRY *remPtr);

/* --------------------------------------------------------------------------------
   Function : UpdateEffectsList
   Purpose : updates efects list
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateEffectsList(EFFECTSLIST *list);

/* --------------------------------------------------------------------------------
   Function : FreeAllEffectsFromList
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllEffectsFromList(void);

#endif