#ifndef __H_SUPERBURP
#define __H_SUPERBURP

#define SUPERBURP_RANGE		(METERTOUNIT(23.0f))

#define SUPERBURP_EASY		(30.0f)
#define SUPERBURP_MEDIUM	(25.0f)
#define SUPERBURP_HARD		(20.0f)


#define SMASHFLAG_CHILLI	(1<<0)
#define SMASHFLAG_RANGEDATTACK	(1<<1)


/*  --------------------------------------------------------------------------------
	Function	: SuperBurp
	Purpose		: Called when the player chooses to do the super burp move
	Parameters	: PLAYER
	Returns		: 
	Info		: if the player does not have enough pop this will not have any function
*/
void SuperBurp(struct TAG_PLAYER *player);


typedef struct TAG_SUPERBURP
{
private:
	float					activeTime;			// TP: currentTime superburp has been active for
	struct TAG_PLAYER		*player;			// TP: player causing burp shinanigans

public:
	/*  --------------------------------------------------------------------------------
		Function	: SUPERBURP::SmashStuff
		Purpose		: Call this to destroy a number of destructible items on the list given in range of position
		Parameters	: actorInstanceList, destructor, range, number, optional center point for test
		Returns		: 
		Info		: 
	*/
	friend void SmashStuff(const ACTORINSTANCELIST* const actorInstanceList, ACTORINSTANCE *destructor, const float range, int number, TBVector center=NULL, int flags=0);


	/*  --------------------------------------------------------------------------------
		Function	: SUPERBURP::TAG_SUPERBURP
		Purpose		: Called when player chooses to do the superburp move
		Parameters	: none
		Returns		: 
		Info		: 
	*/
	TAG_SUPERBURP(struct TAG_PLAYER *player);

	/*  --------------------------------------------------------------------------------
		Function	: SUPERBURP::TAG_SUPERBURP
		Purpose		: destructor called when burp affect has finished
		Parameters	: none
		Returns		: 
		Info		: 
	*/
	~TAG_SUPERBURP(void);

	/*  --------------------------------------------------------------------------------
		Function	: SUPERBURP::Update
		Purpose		: called to do a general update of the superburp
		Parameters	: none
		Returns		: TRUE, FALSE ( FALSE if burp effect has finished)
		Info		: 
	*/
	int	Update(void);

	/*  --------------------------------------------------------------------------------
		Function	: SUPERBURP::Draw
		Purpose		: called to do any visual effects needed
		Parameters	: none
		Returns		: 
		Info		: 
	*/
	void Draw(void);

} SUPERBURP;

/*  --------------------------------------------------------------------------------
	Function	: FinishBurp
	Purpose		: End the burpcan power up
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void FinishBurp(struct TAG_ACTORINSTANCE *actorInstance);

#endif