// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Boss.h
//   Purpose : General boss game stuff
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_BOSS
#define __H_BOSS

#include "physics.h"				// PP: physics functions
#include "actors.h"					// PP: actor stuff
#include "camera.h"					// PP: camera stuff
#include "cameracase.h"				// PP: camera case stuff
#include "TextFX.h"					// PP: for the BOOK system
#include "animation.h"				// PP: animation stuff
#include "characters.h"				// PP: character stuff
#include "util.h"					// PP: utility functions
#include "LoopArray.h"				// PP: Looping array class - please use it!
#include "redshield.h"				// PP: Taz's crash helmet that indicates "health" in the boss games
#include "popup.h"					// PP: Popups windows used for things such as toggling options

// PP: ARE WE IN A BOSS GAME?  QUITE HANDY TO KNOW.  THE DEFINITION OF THIS MACRO WILL CHANGE BUT AT LEAST IT'S HERE TO BE USED
#include "scene.h"					// PP: this needs to be included in order to evaluate IN_BOSS_GAME
#define IN_BOSS_GAME				((map.sceneNumber==SCENE_ZOOBOSS)||(map.sceneNumber==SCENE_WESTBOSS)||(map.sceneNumber==SCENE_CITYBOSS)||(map.sceneNumber==SCENE_TAZPREBOSS)||(map.sceneNumber==SCENE_TAZBOSS))

// PP: standard delay before a bossgame 2player play-again popup appears in the outro
#define BOSS_OUTRO_POPUP_DELAY		2.0f

/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														BOSSGAME

												Superclass for boss games.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define BOSS_NUM_PLAYERS		2	// PP: Maximum number of players in a boss game - that's not going to change

// PP: standard boss game states
enum
{
	BGSTATE_NONE,
	BGSTATE_INTRO,			// PP: eg. in the Zoo boss game, the bit where the elephant drops and it says "get ready"
	BGSTATE_PLAY,			// PP: game in progress
	BGSTATE_OUTRO,			// PP: game won / game lost etc.
	BGSTATE_FINISHED,		// PP: NEW: the end of the game won / game lost sequence
	BGSTATE_CUTSCENE,		// TP: game finished now playing exit cutscene

	NUM_BGSTATES
};

// PP: flags for BOSSGAME::createActorInstance
enum
{
	BGACT_NONE			=0,

	BGACT_COLLCACHE		=1,						// PP: actor instance will be kept in the collision cache for other actor instances to collide with
	BGACT_BEHAVIOUR		=1<<1,					// PP: actor instance will have physics
	BGACT_ANIMATION		=1<<2,					// PP: actor instance will have extra animation data (not needed simply to play animations)
	BGACT_CARTOON		=1<<3,					// PP: actor instance will be cartoon-rendered
	BGACT_CHARINFO		=1<<4,					// PP: actor instance will have character info
	BGACT_LINE			=1<<5,					// TP: actor will be line rendered only

	BGACT_DEFAULTS		=BGACT_CARTOON			// PP: default actor instance creation flags
};

// PP: default actor instance list
#define BG_DEFAULT_ACTOR_INSTANCE_LIST			(&characterInstances)//(gameStatus.player1->instanceList)

// PP: erm, alternative actor instance list, for if the other one doesn't work
#define BG_ALTERNATIVE_ACTOR_INSTANCE_LIST		(&map.animatedInstances)

// PP: TEMP?: macro that should be called as the first thing in the update method of a BOSSGAME subclass.
#define BG_START_UPDATE \
	if(!this->initialised) \
	{ \
		if(gameStatus.currentState == GAMESTATE_INGAME) \
		{ \
			/* PP: cutscene has finished - initialise boss game */ \
			this->initialised=true; \
			this->init(); \
		} \
		else \
		{ \
			return; \
		} \
	}

typedef struct TAG_BOSSGAME
{

private:

	POPUP*				playAgainPopup;			// PP: popup used in 2player to offer the chance to play again

protected:

	bool				initialised;			// PP: has the structure been initialised
	BOOK				messageBook;			// PP: book for messages such as "GET READY..." or "GOAL!"
	BOOK				scoreBook;				// PP: book for overlays such as score readouts
	BOOK				instructionBook;		// PP: book for instructions such as "press X to start"
	bool				twoPlayer;				// PP: are there 2 players?
	int					state;					// PP: state of the game (BGSTATE_...)
	float				timeInState;			// PP: length of time for which the boss game has been in its current state
	float				cameraTimer;			// PP: timer used for camera shots - handy no?


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::initMessageBook
		Purpose 	: initialise the book for messages such as "GET READY..." or "GOAL!"
		Parameters 	: 
		Returns 	: 
		Info 		: left here because BOSSGAME::init calls it
	*/
	virtual void initMessageBook(void)		{this->messageBook.init();};


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::initScoreBook
		Purpose 	: initialise the book for overlays such as score readouts
		Parameters 	: 
		Returns 	: 
		Info 		: left here because BOSSGAME::init calls it
	*/
	virtual void initScoreBook(void)		{this->scoreBook.init();};


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::initInstructionBook
		Purpose 	: initialise the book for instructions such as "press X to start"
		Parameters 	: 
		Returns 	: 
		Info 		: left here because BOSSGAME::init calls it
	*/
	virtual void initInstructionBook(void)	{this->instructionBook.init();};


public:

	// PP: public: TEMP? MILESTONE # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::setState
		Purpose 	: switch the game into a new state
		Parameters 	: the state into which to switch
		Returns 	: 
		Info 		: 
	*/
	virtual void setState(const int newState)
	{
		if(newState != this->state)
		{
			this->state=newState;
			this->timeInState=0.0f;
		}
	}

	int					winner;					// PP: index of player who's won - can also be used to record index of last goal-scorer or whatever
	int					loser;					// PP: index of player who's lost - can also be used to record index of last goal-loser or whatever

	// PP: # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::win
		Purpose 	: win the boss game
		Parameters 	: (opt/0)player to win
		Returns 	: 
		Info 		: 
	*/
	virtual void win(const int weiner=0);


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::lose
		Purpose 	: lose the boss game
		Parameters 	: (opt/0)player to lose
		Returns 	: 
		Info 		: 
	*/
	virtual void lose(const int luzer=0);


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::initPreCutscene
		Purpose 	: initialisation for the Boss game before its intro cutscene
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void initPreCutscene(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::init
		Purpose 	: initialise the Boss game 
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::update
		Purpose 	: update the Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::draw
		Purpose 	: draw the Boss game solid stuff
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void draw(void)		{};


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::draw2
		Purpose 	: draw the Boss game alpha stuff
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void draw2(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::shutdown
		Purpose 	: shutdown the boss game
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::setupActorInstance
		Purpose 	: set-up an existing actor instance for use in the boss game
		Parameters 	: ptr to ACTORINSTANCE, flags
		Returns 	: ptr to the actor instance
		Info 		: 
	*/
	static ACTORINSTANCE* setupActorInstance(ACTORINSTANCE* const instance, const int flags);


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::createActorInstance
		Purpose 	: create an actor instance for use in the boss game
		Parameters 	: name of model, position for instance, (opt/0)y ang,
						(opt/BGACT_DEFAULTS)flags,
						(opt/default)instance list to which to add instance
		Returns 	: ptr to the new actor instance, or NULL if failed
		Info 		: actor instance is added to Taz's instance list, so's it won't fog or owt.
	*/
	static ACTORINSTANCE* createActorInstance(char* const model,
												TBVector pos,
												const float yAng=0.0f,
												const int flags=BGACT_DEFAULTS,
												ACTORINSTANCELIST* const instanceList=BG_DEFAULT_ACTOR_INSTANCE_LIST);

	// TP: TEMP!!!!
	virtual	bool overrideInput()
	{
		return false;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOSSGAME::handle2PlayerOutroState
		Purpose 	: handle 2player outro state
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void handle2PlayerOutroState(void);



}BOSSGAME, _TIDDLYBINGBONGWHERESMYTROUSERS BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														AIUSOUND

										Entries in an ACTORINSTANCEUSER's sound list.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


enum AIUSoundFlag
{
	AIUSOUND_POSITION	=1<<0,			// PP: play the sample from the AIU's current position
	AIUSOUND_TRACK		=1<<1,			// PP: play the sample from the AIU's tracked position
	AIUSOUND_LOOP		=1<<2,			// PP: loop the sample
	AIUSOUND_EXCLUSIVE	=1<<3,			// PP: stop playing all other samples from the AIU as this sample starts
	AIUSOUND_QUEUE		=1<<4,			// PP: play this sample only when any other samples are finished
};

// PP: maximum length of a sample name
#define AIUSOUND_NAME_LENGTH				64

// PP: type for sample-finished callbacks for AIUSOUNDs
typedef void(*AIUSoundFinishCallback)(const struct AIUSOUND* const sound);

// PP: type for entries in an AIU's sound list
struct AIUSOUND
{
	char									name[AIUSOUND_NAME_LENGTH];		// PP: name of sample
	TBSample*								sample;							// PP: pointer to sample
	int										channel;						// PP: channel being used for sample
	AIUSoundFinishCallback					finishFunc;						// PP: function to get called when this sound finishes, or NULL
	bool									started;						// PP: sample has started - if it has not finished then it is currently playing
	bool									finished;						// PP: sample has finished and should be removed from the list
	int										flags;							// PP: combination of play flags (AIUSOUND_...)
	unsigned char							volume;							// PP: volume at which to play the sample (0..255)
	struct TAG_ACTORINSTANCEUSER*			user;							// PP: pointer to AIU making the sound


	/*	--------------------------------------------------------------------------------
		Function 	: AIUSOUND::start
		Purpose 	: start this sound
		Parameters 	: 
		Returns 	: true if the sound could be played, else false
		Info 		: 
	*/
	bool start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: AIUSOUND::stop
		Purpose 	: stop this sound
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void stop(void);
};


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													ACTORINSTANCEUSER

									Base type for anything that uses an ACTORINSTANCE.
												Useful accessor methods, etc.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: maximum number of sounds that an AIU can be playing/queueing at once
#define AIU_MAX_SOUNDS						5

typedef struct TAG_ACTORINSTANCEUSER
{
private:

	LOOPARRAY<AIUSOUND, AIU_MAX_SOUNDS>		sounds;				// PP: list of sounds currently being played by the AIU
	bool									isPlayingSound;		// PP: is the AIU playing one or more sounds at the moment?
	bool									muted;				// PP: is the AIU muted?

public:

	ACTORINSTANCE*							actorInstance;		// PP: actor instance
																
	static TBVector							defaultPosition;	
																
	TBActorAnimSegment*						lastAnim;			// PP: anim being played last update
	TBActorAnimSegment*						currentAnim;		// PP: anim being played last update
	bool									animChanged;		// PP: current anim is not last anim


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::init
		Purpose 	: initialisation
		Parameters 	: model name, position, (opt/0.0f)angle, (opt/default) creation flags, (opt/default)instance list to which to add instance
		Returns 	: 
		Info 		: 
	*/
	void init(char* const model, TBVector pos=defaultPosition, const float yAng=0.0f, const int flags=BGACT_DEFAULTS, ACTORINSTANCELIST* instanceList=BG_DEFAULT_ACTOR_INSTANCE_LIST);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::init
		Purpose 	: initialisation
		Parameters 	: model name, creation flags, (opt/default)instance list to which to add instance
		Returns 	: 
		Info 		: 
	*/
	void init(char* const model, const int flags, ACTORINSTANCELIST* instanceList=BG_DEFAULT_ACTOR_INSTANCE_LIST);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::init
		Purpose 	: initialisation
		Parameters 	: ptr to ACTORINSTANCE, (opt/default)flags
		Returns 	: 
		Info 		: use this to set up an AIU from an instance already in the map
	*/
	void init(ACTORINSTANCE* const actorInstanceIn, const int flags=BGACT_DEFAULTS);
	

	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);

	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::TAG_ACTORINSTANCEUSER
		Purpose 	: default constructor: calls init
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	TAG_ACTORINSTANCEUSER(void)
	{
		this->init();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::TAG_ACTORINSTANCEUSER
		Purpose 	: constructor: calls init
		Parameters 	: position, (opt/0.0f)angle, (opt/false)keep in collision cache, (opt/false) create behaivour, (opt/false) create animation, (opt/true)cartoon rendering, (opt/default)instance list to which to add instance
		Returns 	: 
		Info 		: 
	*/
	TAG_ACTORINSTANCEUSER(char* const model, TBVector pos, const float yAng=0.0f, const int flags=BGACT_DEFAULTS, ACTORINSTANCELIST* instanceList=BG_DEFAULT_ACTOR_INSTANCE_LIST)
	{
		this->init(model, pos, yAng, flags, instanceList);
	}


	/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::udpateSounds
	Purpose 	: udpate the ACTORINSTANCEUSER's sounds
	Parameters 	: 
	Returns 	: 
	Info 		: 
	*/
	void updateSounds(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::udpate
		Purpose 	: udpate
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::shutdown
		Purpose 	: shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::~TAG_ACTORINSTANCEUSER
		Purpose 	: destructor: calls shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	~TAG_ACTORINSTANCEUSER(void)
	{
		this->shutdown();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::playAnim
		Purpose 	: play an animation
		Parameters 	: ptr to anim, (opt/false) whether or not to loop,
						(opt/false) whether or not to queue, (opt/1.0f)speed
		Returns 	: 
		Info 		: 
	*/
	void playAnim(TBActorAnimSegment* const anim, const bool loop=false, const float speed=1.0f);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::playAnim
		Purpose 	: play an animation
		Parameters 	: name of anim, (opt/false) whether or not to loop, (opt/1.0f)speed
		Returns 	: 
		Info 		: 
	*/
	void playAnim(char* const animName, const bool loop=false, const float speed=1.0f);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::playAnim
		Purpose 	: play an animation
		Parameters 	: ptr to anim, (opt/false) whether or not to loop, (opt/1.0f)speed
		Returns 	: 
		Info 		: 
	*/
	void queueAnim(TBActorAnimSegment* const anim, const bool loop=false, const float speed=1.0f);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::queueAnim
		Purpose 	: queue an animation
		Parameters 	: name of anim, (opt/false) whether or not to loop, (opt/1.0f)speed
		Returns 	: 
		Info 		: 
	*/
	void queueAnim(char* const animName, const bool loop=false, const float speed=1.0f);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::playingAnim
		Purpose 	: check if the ACTORINSTANCEUSER is playing an animation
		Parameters 	: (opt/NULL=any anim)name of animation for which to check
		Returns 	: whether or not the ACTORINSTANCEUSER is playing an animation
		Info 		: 
	*/
	inline bool playingAnim(char* const animName=NULL)
	{
		if(animName == NULL)
		{
			return (baIsActorAnimPlaying(&this->actorInstance->actorInstance, this->actorInstance->actorInstance.rootNodeInstance,TRUE)==TRUE);
		}
		else
		{
			return (CheckIfPlayingAnimation(this->actorInstance, animName)==0);
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::vel
		Purpose 	: get a ref to the velocity vector of the actor instance
		Parameters 	: 
		Returns 	: a ref to the velocity vector of the actor instance
		Info 		: 
	*/
	inline TBVector& vel(void) const
	{
		return this->actorInstance->actorBehaviour->physics.velocity;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::pos
		Purpose 	: get a ref to the position vector of the actor instance
		Parameters 	: 
		Returns 	: a ref to the position vector of the actor instance
		Info 		: 
	*/
	inline TBVector& pos(void) const
	{
		return this->actorInstance->actorInstance.position;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::ang
		Purpose 	: get a ref to the orientation quaternion of the actor instance
		Parameters 	: 
		Returns 	: a ref to the orientation quaternion of the actor instance
		Info 		: 
	*/
	inline TBQuaternion& ang(void) const
	{
		return this->actorInstance->actorInstance.orientation;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::scale
		Purpose 	: get a ref to the scale vector of the actor instance
		Parameters 	: 
		Returns 	: a ref to the scale vector of the actor instance
		Info 		: 
	*/
	inline TBVector& scale(void) const
	{
		return this->actorInstance->actorInstance.scale;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::setScale
		Purpose 	: set the uniform scale of the actor instance
		Parameters 	: new uniform scale for the actor instance
		Returns 	: 
		Info 		: 
	*/
	inline void setScale(const float scaleIn)
	{
		SETVECTOR(this->scale(), scaleIn, scaleIn, scaleIn, 1.0f);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::setYang
		Purpose 	: set the Y angle of the actor instance
		Parameters 	: new Y angle for the actor instance
		Returns 	: 
		Info 		: 
	*/
	inline void setYang(const float yAng)
	{
		bmQuatYRotation(this->ang(), Aabs(yAng));

		// PP: TEST
		//utilNormaliseQuaternion(this->ang());

		if(this->actorInstance->actorBehaviour != NULL)
		{
			bmQuatCopy(actorInstance->actorBehaviour->rotAim, this->ang());
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::getYang
		Purpose 	: get the Y angle of the actor instance
		Parameters 	: 
		Returns 	: the Y angle of the actor instance
		Info 		: 
	*/
	inline float getYang(void) const
	{
		TBVector		rotVec;
		TBQuaternion	rotQuat;

		bmQuatCopy(rotQuat, this->ang());

		utilNormaliseQuaternion(rotQuat);

		bmQuatToRotation(rotVec, rotQuat);

		return  Aabs(rotVec[W]*rotVec[Y]);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::dist
		Purpose 	: get the scalar distance between this and a point
		Parameters 	: point
		Returns 	: the scalar distance between this and the point
		Info 		: this is just plain lazy
	*/
	inline float dist(TBVector point) const
	{
		return bmVectorDistance(this->pos(), point);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::dist
		Purpose 	: get the scalar distance between this and another ACTORINSTANCEUSER
		Parameters 	: other ACTORINSTANCEUSER
		Returns 	: the scalar distance between this and another ACTORINSTANCEUSER
		Info 		: this is really lazy...in a good way
	*/
	inline float dist(const TAG_ACTORINSTANCEUSER& that) const
	{
		return this->dist(that.pos());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::XZdist
		Purpose 	: get the scalar XZ distance between this and another ACTORINSTANCEUSER
		Parameters 	: other ACTORINSTANCEUSER
		Returns 	: the scalar XZ distance between this and another ACTORINSTANCEUSER
		Info 		: 
	*/
	inline float XZdist(const TAG_ACTORINSTANCEUSER& that) const
	{
		TBVector2D	thisPos;
		TBVector2D	thatPos;

		thisPos[X]=this->pos()[X];
		thisPos[Y]=this->pos()[Z];

		thatPos[X]=that.pos()[X];
		thatPos[Y]=that.pos()[Z];

		return bmVector2DDistance(thatPos, thisPos);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::XZdist
		Purpose 	: get the scalar XZ distance between this and a 3D point
		Parameters 	: point vector
		Returns 	: the scalar XZ distance between this and a 3D point
		Info 		: 
	*/
	inline float XZdist(const TBVector point) const
	{
		TBVector2D	thisPos;
		TBVector2D	thatPos;

		thisPos[X]=this->pos()[X];
		thisPos[Y]=this->pos()[Z];

		thatPos[X]=point[X];
		thatPos[Y]=point[Z];

		return bmVector2DDistance(thatPos, thisPos);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::getYang
		Purpose 	: get the Y angle from this to a 3D point
		Parameters 	: 3D point to which to find Y angle
		Returns 	: the Y angle from this to the 3D point
		Info 		: 
	*/
	inline float getYang(TBVector point) const
	{
		TBVector2D	distVec;

		distVec[X]=point[X]-this->pos()[X];
		distVec[Y]=point[Z]-this->pos()[Z];

		return bmATan2(distVec[X], distVec[Y]);		
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::getYang
		Purpose 	: get the Y angle from this to another ACTORINSTNACEUSER
		Parameters 	: another ACTORINSTNACEUSER
		Returns 	: the Y angle from this to another ACTORINSTNACEUSER
		Info 		: 
	*/
	inline float getYang(const TAG_ACTORINSTANCEUSER& that) const
	{
		return this->getYang(that.pos());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::YrotateToFace
		Purpose 	: intantly rotate the ACTORINSTANCEUSER on the Y axis so it faces a point
		Parameters 	: point to face
		Returns 	: 
		Info 		: does nothing if the this and the point are XZ-copositional
	*/
	inline void YrotateToFace(TBVector point)
	{
		TBVector2D	distVec;

		distVec[X]=point[X]-this->pos()[X];
		distVec[Y]=point[Z]-this->pos()[Z];

		float dist;

		dist=bmVector2DLen(distVec);

		if(dist > 0.0f)
		{
			this->setYang(bmATan2(distVec[Y], distVec[X])+HALFPI);
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::YrotateToFace
		Purpose 	: intantly rotate the ACTORINSTANCEUSER on the Y axis so it faces another ACTORINSTANCEUSER
		Parameters 	: ref to other ACTORINSTANCEUSER to face
		Returns 	: 
		Info 		: does nothing if the two ACTORINSTANCEUSERs are XZ-copositional
	*/
	inline void YrotateToFace(const TAG_ACTORINSTANCEUSER& that)
	{
		this->YrotateToFace(that.pos());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::getYAngFromLOS
		Purpose 	: get the angle from a character's line-of-sight to another character
		Parameters 	: ref character to whom to get the a angle from this character's line-of-sight
		Returns 	: the angle from a character's line-of-sight to another character
		Info 		: 
	*/
	inline float getYAngFromLOS(const TAG_ACTORINSTANCEUSER& that) const
	{
		return GetYAngleFromLineOfSight(&(this->actorInstance->actorInstance), &(that.actorInstance->actorInstance));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::getYAngBetweenLOS
		Purpose 	: get the angle between the lines-of-sight of two characters
		Parameters 	: ref to character between whose line-of-sight and this character's line-of-sight to get the angle
		Returns 	: the angle between the lines-of-sight of two characters
		Info 		: 
	*/
	inline float getYAngBetweenLOS(const TAG_ACTORINSTANCEUSER& that) const
	{
		return GetYAngleBetweenLinesOfSight(&(this->actorInstance->actorInstance), &(that.actorInstance->actorInstance));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::getNodePos
		Purpose 	: get the world position of a node within the ACTORINSTANCEUSER
		Parameters 	: ptr to bone to locate
		Returns 	: ref to the world position of the node
		Info 		: 
	*/
	inline void getNodePos(TBActorNodeInstance* const node, TBVector nodePos) const
	{
		baGetNodesWorldPosition(&this->actorInstance->actorInstance, node, nodePos);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::getNodePos
		Purpose 	: get the world position of a node within the ACTORINSTANCEUSER
		Parameters 	: name of bone to locate, node position vector to fill in
		Returns 	: ref to the world position of the node
		Info 		: 
	*/
	inline void getNodePos(char* const nodeName, TBVector nodePos)
	{
		this->getNodePos(baFindNode(this->actorInstance->actorInstance.rootNodeInstance, nodeName), nodePos);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::getSound
		Purpose 	: get a ptr to a sound if it exists in the AIU's sound list
		Parameters 	: 
		Returns 	: AIUSOUND*, NULL if not found
		Info 		: 
	*/
	AIUSOUND* getSound(const char* const name);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::playingSound
		Purpose 	: determine if the ACTORINSTANCEUSER is playing a sound
		Parameters 	: 
		Returns 	: true if a sound is being played by the ACTORINSTANCEUSER
		Info 		: 
	*/
	inline bool playingSound(void)
	{
		return this->isPlayingSound;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::sampleFinishCallback
		Purpose 	: callback triggered whan an AIU sound finishes
		Parameters 	: void ptr to AIUSOUND finishing
		Returns 	: 
		Info 		: 
	*/
	static void sampleFinishCallback(void* context);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::playSound
		Purpose 	: play a sound
		Parameters 	: sound name, (opt/default)flags, (opt/255)volume(0..255), (opt/NULL) custom sample-finished callback (See AIUSoundFinishCallback typedef)
		Returns 	: 
		Info 		: Might add a frequency param when I find a need for it
	*/

	// PP: default flags for ACTORINSTANCEUSER::playSound and ACTORINSTANCEUSER::queueSound
#define AIU_PLAYSOUND_DEFAULT_FLAGS			(AIUSOUND_TRACK|AIUSOUND_EXCLUSIVE)

	void playSound(char* const name, const int flags=AIU_PLAYSOUND_DEFAULT_FLAGS, const unsigned char volume=255, const AIUSoundFinishCallback finishFunc=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::queueSound
		Purpose 	: queue a sound
		Parameters 	: sound name, (opt/default)flags, (opt/255)volume(0..255), (opt/NULL) custom sample-finished callback (See AIUSoundFinishCallback typedef)
		Returns 	: 
		Info 		: Might add a frequency param when I find a need for it
	*/
	inline void queueSound(char* const name, const int flags=AIU_PLAYSOUND_DEFAULT_FLAGS, const unsigned char volume=255, const AIUSoundFinishCallback finishFunc=NULL)
	{
		// PP: heheheh - lazy git I am
		this->playSound(name, flags|AIUSOUND_QUEUE, volume, finishFunc);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::stopSound
		Purpose 	: stop playing a sound from this ACTORINSTANCEUSER
		Parameters 	: (opt/NULL=all)name of sound to stop
		Returns 	: 
		Info 		: 
	*/
	void stopSound(const char* const name=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::mute
		Purpose 	: prevent the AIU from making any sound until it is un-muted
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void mute(void)
	{
		this->stopSound();
		this->muted=true;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::unmute
		Purpose 	: allow the AIU to make sounds again
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void unmute(void)
	{
		this->muted=false;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ACTORINSTANCEUSER::flushSound
		Purpose 	: clear the sound queue of this ACTORINSTANCEUSER
		Parameters 	: 
		Returns 	: 
		Info 		: leaves be any samples that have already started
	*/
	void flushSound(void);


}ACTORINSTANCEUSER, _ACTORINSTANCEUSER BALIGN16;

#endif // PP: ndef __H_BOSS