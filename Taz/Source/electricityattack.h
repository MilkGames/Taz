#ifndef __H_ELECTRICITYATTACK
#define __H_ELECTRICITYATTACK

typedef struct TAG_MTWEETYSTREAKDATA
{
	TBVector			*tempPoints;
	TBVector			*points;
	float				*size;
	float				*screenSize;

	int					*colour;
	TBPrimVertex2D		*buffer;

//items that are required on a 'one per streak' basis
	float				timeConst;
	TBTexture			*texture;
	int32				iPad;
} MTWEETYSTREAKDATA, _MTWEETYSTREAKDATA BALIGN16;

typedef struct TAG_MTWEETYWAVEDATA
{
	float		frequency;
	float		clock;
	float		amplitudeConst;
	float		tilt;

	float		tiltSpeed;
	float		tiltDirection;
	int32		iPad[2];

	MTWEETYSTREAKDATA		streak;
} MTWEETYWAVEDATA, _MTWEETYWAVEDATA BALIGN16;

typedef struct TAG_ELECTRICITYATTACKINFO
{
	TBVector		targetPos;
	TBVector		offset;

	TBActorNodeInstance	*node;
	MTWEETYWAVEDATA		*waveData;
	int32				state;
	int32				padd;

//streak data
	int32				numPoints;
	int32				numStreaks;
	float				streakLength;
	int32				iPad;
} ELECTRICITYATTACKINFO, _ELECTRICITYATTACKINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateElectricityAttack
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

ELECTRICITYATTACKINFO *CreateElectricityAttack(int32 numPointsPerStreak, int32 numStreaks);

/* --------------------------------------------------------------------------------
   Function : CreateElectricityAttack
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateElectricityAttack(struct TAG_ACTORINSTANCE *attacker, ELECTRICITYATTACKINFO *info);

/* --------------------------------------------------------------------------------
   Function : ResetElectricityAttack
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void ResetElectricityAttack(ELECTRICITYATTACKINFO *info);

/* --------------------------------------------------------------------------------
   Function : CreateElectricityAttack
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

ELECTRICITYATTACKINFO *FreeElectricityAttackInfo(ELECTRICITYATTACKINFO *info);

/* --------------------------------------------------------------------------------
   Function : CreateElectricityAttack
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawElectricityAttackInfo(struct TAG_ACTORINSTANCE *attacker, ELECTRICITYATTACKINFO *info);

/* --------------------------------------------------------------------------------
   Function : FillInAttributesForMTweetyGame
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FillInAttributesForMTweetyGame(ELECTRICITYATTACKINFO *info);

/* --------------------------------------------------------------------------------
   Function : UpdateElectricityAttackCollisionWithActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int UpdateElectricityAttackCollisionWithActor(ELECTRICITYATTACKINFO *info, struct TAG_ACTORINSTANCE *collider);

#endif