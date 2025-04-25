// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : WestBossExp.h
//   Purpose : Bomber-Man-style corridor explosions, only for the West Boss game
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_WESTBOSSEXP
#define __H_WESTBOSSEXP


#include "Fireball.h"		// PP: Particle emitters used for explosion effects, eg. by WestBossExp


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													WESTBOSSEXPBRANCH

							The branches (one per direction) that form a west boss explosion
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define WBEB_CRATEEXPLODE_RANGE		METERTOUNIT(1.0f)		// PP: max distance at which an explosion branch will explode a crate

// PP: directions for west boss explosions
enum WBE_Direction
{
	WBEDIRECTION_UP,
	WBEDIRECTION_RIGHT,
	WBEDIRECTION_DOWN,
	WBEDIRECTION_LEFT
};

// PP: convert a WBE direction to a Y angle
inline float toYAng(const WBE_Direction dir)
{
	return dir*HALFPI;
}

// PP: states for west boss explosion branches
enum WBEB_State
{
	WBEBSTATE_NOTUSED,
	WBEBSTATE_PREPARED,
	WBEBSTATE_FINISHED
};

typedef class TAG_WESTBOSSEXPBRANCH
{
public:

	// PP: ~~~~~~~~~~~

	TBVector								pos;
	TBVector								vel;

	int										range;						// PP: range IN GRID SQUARES of the explosion branch
	int										actualRange;				// PP: number of grid squares the explosion branch can reach without hitting something - This is used to highlight danger squares for the player

	WBE_Direction							direction;

	int										startX;
	int										startY;

	int										finalX;
	int										finalY;

	int										extendedFinalX;				// PP: x grid coord of end of corridor along which branch is moving
	int										extendedFinalY;				// PP: y grid coord of end of corridor along which branch is moving

	WBEB_State								state;


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPBRANCH::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);


	TAG_WESTBOSSEXPBRANCH(void)
	{
		this->init();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPBRANCH::init
		Purpose 	: initialisation
		Parameters 	: start pos, start grid X, start grid Y, velocity, range in grid squares, direction, final x & y grid coords, blocking x&y coords
		Returns 	: 
		Info 		: 
	*/
	void init(TBVector posIn, const int startXin, const int startYin, TBVector velIn, const int rangeIn, const WBE_Direction dir, const int finalXin, const int finalYin, const int extendedFinalXin, const int extendedFinalYin);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPBRANCH::start
		Purpose 	: start the explosion branch
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPBRANCH::updateActualRange
		Purpose 	: update the actual range of the branch: find out how far it can go without hitting something
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void updateActualRange(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPBRANCH::update
		Purpose 	: update the explosion branch
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPBRANCH::getYang
		Purpose 	: get the Y angle of the branch's direction
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline float getYang(void) const
	{
		return toYAng(this->direction);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBEXPBRANCH::fireballUpdateCallback
		Purpose 	: custom update callback for FIREBALLs used by WESTBOSSEXPLOSIONs
		Parameters	: ptr to fireball, ptr to branch to which fireball belongs
		Returns		: 
		Info		: must match the FB_UpdateCallback typedef (Fireball.h)
	*/
	static void fireballUpdateCallback(FIREBALL* const fireball, const long32);


}WESTBOSSEXPBRANCH BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													WBE_FIREBALLBRANCHINFO

						32-bit struct used to pass westboss explosion branch details on to their fireballs.
								Safer & more efficient than using a pointer to the original branch.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


typedef struct TAG_WBE_FIREBALLBRANCHINFO
{
	unsigned int		dir					:2;		// PP: direction
	unsigned int		startX				:5;		// PP: initial grid X
	unsigned int		startY				:5;		// PP: initial grid Y
	unsigned int		targetX				:5;		// PP: target grid X
	unsigned int		targetY				:5;		// PP: target grid Y
	unsigned int		extendedTargetX		:5;		// PP: target grid X
	unsigned int		extendedTargetY		:5;		// PP: target grid Y


	/*	--------------------------------------------------------------------------------
		Function 	: WBE_FIREBALLBRANCHINFO::TAG_WBE_FIREBALLBRANCHINFO
		Purpose 	: default constructor, but just you try taking it out!
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	TAG_WBE_FIREBALLBRANCHINFO(void)	{};


	/*	--------------------------------------------------------------------------------
		Function 	: WBE_FIREBALLBRANCHINFO::makeImpossible
		Purpose 	: fill the branch info struct with impossible values
						(so it'll never compare positively with any real one)
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void makeImpossible(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBE_FIREBALLBRANCHINFO::TAG_WBE_FIREBALLBRANCHINFO
		Purpose 	: constructor - fills new info struct with details from branch passed in
		Parameters 	: branch
		Returns 	: 
		Info 		: 
	*/
	TAG_WBE_FIREBALLBRANCHINFO(const WESTBOSSEXPBRANCH* const branch)
	{
		this->dir=branch->direction;
		this->startX=branch->startX;
		this->startY=branch->startY;
		this->targetX=branch->finalX;
		this->targetY=branch->finalY;
		this->extendedTargetX=branch->extendedFinalX;
		this->extendedTargetY=branch->extendedFinalY;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBE_FIREBALLBRANCHINFO::TAG_WBE_FIREBALLBRANCHINFO
		Purpose 	: constructor - memcpy's from a long32
		Parameters 	: long32 to copy
		Returns 	: 
		Info 		: 
	*/
	TAG_WBE_FIREBALLBRANCHINFO(const long32 longIn)
	{
		memcpy(this, &longIn, sizeof(long32));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBE_FIREBALLBRANCHINFO::operator long32()
		Purpose 	: cast the info struct to a long32
		Parameters 	: 
		Returns 	: info struct as a long32
		Info 		: // PP: the WBE_FIREBALLBRANCHINFO is 32 bits;
						that's the whole point of the thing and it would never change size
	*/
	operator long32()
	{
		return *((long32*)this);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBE_FIREBALLBRANCHINFO::operator long32() const
		Purpose 	: cast the info struct to a const long32
		Parameters 	: 
		Returns 	: info struct as a const long32
		Info 		: // PP: the WBE_FIREBALLBRANCHINFO is 32 bits;
						that's the whole point of the thing and it would never change size
	*/
	operator long32() const
	{
		return *((long32*)this);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBE_FIREBALLBRANCHINFO::getDir
		Purpose 	: get the direction member as a WBE_Direction
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline WBE_Direction getDir(void) const
	{
		return (WBE_Direction)this->dir;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBE_FIREBALLBRANCHINFO::reaches
		Purpose 	: find out if the branch reaches a specified grid square
		Parameters 	: grid X, grid Y
		Returns 	: true if the branch reaches the grid square, false otherwise
		Info 		: 
	*/
	bool reaches(const int gridX, const int gridY);


	/*	--------------------------------------------------------------------------------
		Function 	: WBE_FIREBALLBRANCHINFO::getYang
		Purpose 	: get the Y angle of the branch's direction
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline float getYang(void) const
	{
		return toYAng(WBE_Direction(this->dir));
	}


}WBE_FIREBALLBRANCHINFO BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WESTBOSSEXPLOSION

										Bomber-Man explosions for the West Boss game only
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: speed at which branches extend down corridors
#define WBE_BRANCH_SPEED	METERTOUNIT(8.0f)

#define WBE_MAX_RANGE		4						// PP: maximum number of grid squares in any direction that can be covered by the explosion

#define WBE_NUM_BALLS		100

typedef class TAG_WESTBOSSEXPLOSION
{
public:

	int						startX;					// PP: original x grid pos
	int						startY;					// PP: original y grid pos

	TBVector				pos;					// PP: world-space 3D position of explosion origin

	int						range;					// PP: max number of squares to be covered in each direction

	WESTBOSSEXPBRANCH		branches[4];

	unsigned char			numBranches		:2;		// PP: number of branches 0/1/2/3

	bool					finished;				// PP: explosion is completely finished, ready to be taken out of loop array


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPLOSION::update
		Purpose 	: update
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPLOSION::init
		Purpose 	: initialisation
		Parameters 	: x grid pos, y grid pos, range
		Returns 	: this
		Info 		: 
	*/
	TAG_WESTBOSSEXPLOSION* init(const int xIn, const int yIn, const int rangeIn);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPLOSION::TAG_WESTBOSSEXPLOSION
		Purpose 	: constructor: calls init
		Parameters 	: x grid pos, y grid pos, range
		Returns 	: 
		Info 		: 
	*/
	TAG_WESTBOSSEXPLOSION(const int xIn, const int yIn, const int rangeIn)
	{
		this->init(xIn, yIn, rangeIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPLOSION::TAG_WESTBOSSEXPLOSION
		Purpose 	: default constructor: does bugger all
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	TAG_WESTBOSSEXPLOSION(void)
	{
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPLOSION::start
		Purpose 	: start the explosion
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPLOSION::shutdown
		Purpose 	: shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPLOSION::TAG_WESTBOSSEXPLOSION
		Purpose 	: destructor: calls shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	~TAG_WESTBOSSEXPLOSION(void)
	{
		this->shutdown();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSEXPLOSION::draw
		Purpose 	: draw
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);


}WESTBOSSEXPLOSION BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											GENERAL WEST BOSS EXPLOSION FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: WestBossExp_init
	Purpose 	: initialise west boss explosions
	Parameters	: 
	Returns		: 
	Info		: 
*/
void WestBossExp_init(void);


/*	--------------------------------------------------------------------------------
	Function 	: WestBossExp_create
	Purpose 	: create a west boss explosion
	Parameters	: x & y grid pos, range
	Returns		: ptr to newly created west boss explosion
	Info		: 
*/
WESTBOSSEXPLOSION* WestBossExp_create(const int x, const int y, const int range);


/* --------------------------------------------------------------------------------
   Function		: WestBossExp_shutdown
   Purpose		: shutdown the explosion list
   Parameters	: 
   Returns		: 
   Info			: 
*/
void WestBossExp_shutdown(void);


/* --------------------------------------------------------------------------------
   Function		: WestBossExp_update
   Purpose		: update all ball explosions
   Parameters	: 
   Returns		: 
   Info			: 
*/
void WestBossExp_update(void);


/* --------------------------------------------------------------------------------
   Function		: WestBossExp_draw
   Purpose		: draw all west boss explosions
   Parameters	: 
   Returns		: 
   Info			: only need to draw danger tiles - actual fireball gets drawn elsewhere
*/
void WestBossExp_draw(void);


#endif// PP: ndef __H_WESTBOSSEXP
