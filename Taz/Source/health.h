#ifndef __H_HEALTH
#define __H_HEALTH

#define	TAZHEALTH_MAX			(6.0f)
#define	TAZHEALTH_THETARATE		(RAD(240.0f))
#define TAZHEALTH_PHIRATE		(RAD(-60.0f))
#define TAZHEALTH_AXISANGLE		(RAD(7.5f))
#define TAZHEALTH_DISPLAYTIME	(4.0f)
#define TAZHEALTH_SCALE			(0.5f)

#define HEALTHFLAG_NOPREP		(1<<0)

#define NOHEALTH

enum
{
	HEALTHSTATE_NONE,
	HEALTHSTATE_HALF,
	HEALTHSTATE_FULL,
};

typedef struct TAG_HEALTHINFO
{
	TBActorInstance		drumstickFull;
	TBActorInstance		drumstickHalf;
	TBActorInstance		drumstickEmpty;

	int					drumsticks[(int)TAZHEALTH_MAX];				//current states of the drumsticks
	float				scale;
	int					iPad;

	float				theta;
	float				phi;
	float				angVelTheta;
	float				angVelPhi;
} HEALTHINFO, _HEIALTHINFO BALIGN16;

typedef struct TAG_HEALTH
{
	float	maxHealth;					// maximum health of character
	float	currentHealth;				// current health of character
	float	oldHealth;					// health from last frame
	float	timeHealthBubbleActive;		// current time in seconds health thought bubble has been active
	
	int		isHealthBubbleActive;		// is there a health thought bubble active
	int		currentBurp;				// TP: number of burp cans collected
	int		flags;						// CMD: flags
	int		iPad;

	struct TAG_HEALTHINFO	info;
} HEALTH, _HEALH BALIGN16;

/* --------------------------------------------------------------------------------
   Function : InitCharactersHealth
   Purpose : initialise the characters health structure
   Parameters : charinfo, max health
   Returns : 
   Info : 
*/
void InitCharactersHealth(struct TAG_CHARACTERINFO *charInfo, float health);

/* --------------------------------------------------------------------------------
   Function : InitHealthInfo
   Purpose : initialise the characters health structure
   Parameters : charinfo, max health
   Returns : 
   Info : 
*/

void InitHealthInfo(struct TAG_HEALTH *health);

/* --------------------------------------------------------------------------------
   Function : ResetCharactersHealth
   Purpose : 
   Parameters : charinfo
   Returns : 
   Info : 
*/
void ResetCharactersHealth(struct TAG_CHARACTERINFO *charInfo);

/* --------------------------------------------------------------------------------
   Function : ReduceCharactersHealth
   Purpose : Reduce the characters health by a given amount
   Parameters : charinfo, amount
   Returns : 
   Info : 
*/
void ReduceCharactersHealth(struct TAG_ACTORINSTANCE *actorInstance, float amount);

/* --------------------------------------------------------------------------------
   Function : IncreaseCharactersHealth
   Purpose : Increase the characters health
   Parameters : charinfo, amount
   Returns : 
   Info : used when collecting food
*/
void IncreaseCharactersHealth(struct TAG_CHARACTERINFO *charInfo, float amount);

/* --------------------------------------------------------------------------------
   Function : IncreaseCharactersBurp
   Purpose : Increase the characters burp
   Parameters : charinfo, amount
   Returns : 
   Info : used when collecting burp cans
*/
void IncreaseCharactersBurp(struct TAG_CHARACTERINFO *charInfo, int amount);

/* --------------------------------------------------------------------------------
   Function : ProcessCharacterDeath
   Purpose : Respond to a character running out of health
   Parameters : ACTORINSTANCE
   Returns : 
   Info : Internal
*/
void ProcessCharacterDeath(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : PrepCharactersHealth
   Purpose : Update the thought bubble with current health if needed and looks for characters death
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void PrepCharactersHealth(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateHealthThoughtBubble
   Purpose : Sets up a health thought bubble
   Parameters : CHARACTERINFO
   Returns : 
   Info : 
*/
int CreateHealthThoughtBubble(struct TAG_CHARACTERINFO *charInfo);

/* --------------------------------------------------------------------------------
   Function : DrawCharactersHealth
   Purpose : Draws any items needed in relation to health and thought bubble
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DrawCharactersHealth(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ResetCharacterPropertiesAfterDeath
   Purpose : 
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/

void ResetCharacterPropertiesAfterDeath(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateCharactersHealthDisplay
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCharactersHealthDisplay(struct TAG_HEALTH *health);

/* --------------------------------------------------------------------------------
   Function : UpdateCharactersHealthDisplayGeometry
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCharactersHealthDisplayGeometry(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: KillAllPowerUps
	Purpose		: Like it sounds
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void KillAllPowerUps(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: respawns all power ups
	Purpose		: spins through collectibles extra instance list
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void RespawnAllPowerUps(void);

/*  --------------------------------------------------------------------------------
	Function	: ResetCharacterAfterDeath
	Purpose		: Reset the characters position, orientation, camera and health
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void ResetCharacterAfterDeath(struct TAG_ACTORINSTANCE *actorInstance);


#endif