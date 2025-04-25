#ifndef __H_CHEESEGUN
#define __H_CHEESEGUN

#include "linkedlist.h"

#define	MAX_CHEESE			(20)

#define	CHEESE_RADIUSRATE	(80.0f)
#define CHEESE_ANGULARVEL	(RAD(30.0f))
#define CHEESE_SCALERATE	(62.5f)

//#define QUESTIONMARK_FADERATE	(128.0f)

/*enum
{
	QUESTIONMARKSTATE_FADEUP,
	QUESTIONMARKSTATE_STEADY,
	QUESTIONMARKSTATE_FADEDOWN,
};*/

typedef struct TAG_CHEESEEXTRAINFO
{
	TBVector	position;

	TBTexture	*texture;
	float		scale;
	float		angle;
	float		life;

	float		radius;
	float		height;
	float		alpha;
	float		maxLife;
} CHEESEEXTRAINFO, _CHEESEEXTRAINFO BALIGN16;

typedef class TAG_STINKYCHEESE
{
private:
	static	LINKEDLIST<TAG_STINKYCHEESE>	list;		// TP: list of all the cheese on level

	float									life;		// TP: time in seconds left for cheese
	float									maxLife;	// TP: total time cheese should last for
	float									lastSmoke;	// TP: time since last smoke created
	ACTORINSTANCE							*cheese;	// TP: the cheese itself

	/*  --------------------------------------------------------------------------------
		Function	: STINKYCHEESE::Init
		Purpose		: Create the cheese
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Init(TBVector position,TBQuaternion orientation,TBVector scale,float life);

	/*  --------------------------------------------------------------------------------
		Function	: STINKYCHEESE::Update
		Purpose		: Update the cheese
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Update();

	/*  --------------------------------------------------------------------------------
		Function	: STINKYCHEESE::Draw
		Purpose		: Draws the cheese
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Draw();

	/*  --------------------------------------------------------------------------------
		Function	: STINKYCHEESE::StunEnemies
		Purpose		: find all enemies within range and stun them
		Parameters	: range of stun
		Returns		: 
		Info		: 
	*/
	void StunEnemies(float range);

public:

	/*  --------------------------------------------------------------------------------
		Function	: STINKYCHEESE::TAG_STINKYCHEESE
		Purpose		: Create a piece of stinky cheese
		Parameters	: position, orientation, scale, life
		Returns		: 
		Info		: 
	*/
	TAG_STINKYCHEESE(TBVector position,TBQuaternion orientation,TBVector scale,float life)
	{
		Init(position, orientation, scale, life);
	}


	/*  --------------------------------------------------------------------------------
		Function	: ~STINKYCHEESE::TAG_STINKYCHEESE
		Purpose		: Destroy the cheese
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	~TAG_STINKYCHEESE()
	{
		if (cheese) FreeActorInstance(cheese);
	}


	/*  --------------------------------------------------------------------------------
		Function	: NumActive
		Purpose		: Returns the number of active cheeses
		Parameters	: 
		Returns		: number of active cheeses
		Info		: 
	*/

	static int NumActive(void)
	{
		return list.getSize();
	}


	/*  --------------------------------------------------------------------------------
		Function	: KillStinkyCheese
		Purpose		: Removes all cheese from a level
		Parameters	: void
		Returns		: 
		Info		: 
	*/
	friend void KillStinkyCheese(void);


	/*  --------------------------------------------------------------------------------
		Function	: UpdateStinkyCheese
		Purpose		: Removes all cheese from a level
		Parameters	: void
		Returns		: 
		Info		: 
	*/
	friend void UpdateStinkyCheese(void);


	/*  --------------------------------------------------------------------------------
		Function	: DrawStinkyCheese
		Purpose		: Removes all cheese from a level
		Parameters	: void
		Returns		: 
		Info		: 
	*/
	friend void DrawStinkyCheese(void);


	/*  --------------------------------------------------------------------------------
		Function	: FireCheese
		Purpose		: Launch a piece of cheese
		Parameters	: ACTORINSTANCE throwing the cheese
		Returns		: 
		Info		: 
	*/
	friend int FireCheese(struct TAG_ACTORINSTANCE *actorInstance);

} STINKYCHEESE;


/* --------------------------------------------------------------------------------
   Function : InitCheeseExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitCheeseExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : CheeseExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void CheeseExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : CheeseExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int CheeseExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : CheeseExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void CheeseExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : CheeseExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void CheeseExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : CheeseExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void CheeseExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif
