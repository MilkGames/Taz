// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : securitybox.cpp
//   Purpose : Security box system
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "collisioncache.h"
#include "mapfile.h"
#include "scene.h"
#include "securitybox.h"
#include "linkedlist.h"
#include "animation.h"
#include "playerstats.h"
#include "collectibles.h"
#include "maths.h"
#include "main.h"
#include "physics.h"
#include "cheats.h"
#include "status.h"
#include "destructibles.h"
#include "trigger.h"
#include "textures.h"
#include "sfx.h"
#include "genericai.h"
#include "characters.h"
#include "rumble.h"
#include "genericai.h"

//#define SECBOX_DEBUG

#define	SECURITYBOXNAME		"secbox.obe"
#define SECURITYBOXACTIVATERANGE	(METERTOUNIT(5.0f))
#define SECURITYBOXMINYRANGE		(METERTOUNIT(2.0f))

int		SamChannel = -1;

float	timeToTing = -1.0f;

LINKEDLIST<SECURITYBOX>		SECURITYBOX::list("secbox");		// TP: List of all security boxes in existance

/*  --------------------------------------------------------------------------------
	Function	: myStateChangeCallback
	Purpose		: standard callback thingy
	Parameters	: 
	Returns		: 
	Info		: 
*/
static void myStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	if((bmRand()%4) == 0)
	{
		PlayAnimationByName(actorInstance, "fly2", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	else
	{
		PlayAnimationByName(actorInstance, "fly1", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	actorInstance->actorAnimation->useMotionBone = FALSE;

	RequestRumbleEffect(gameStatus.player[actorInstance->playerNo].controller, 255, 0.05f, (SPINFX_KOIKTIME-0.1f), SPINFX_KOIKTIME, 0.0f, WAVEFORM_NORMAL, 0.0f, 0.0f, 0);

	/*if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected < 100 && gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_HARD)
	{
		numPickupsRegen += 1;	// JW: Increase Number of pickups to respawn

		ActivateSwirlOverlay(&gameStatus.player[0]);
	}*/
}


/*  --------------------------------------------------------------------------------
	Function	: SECURITYBOX::Reactivate
	Purpose		: re-activate, eg. when the timer has run out
	Parameters	: 
	Returns		: 
	Info		: // PP: made this into a method cos it needs to be cheat-controlled as well
*/
void SECURITYBOX::Reactivate(void)
{
	// PP: don't reactivate if CHEAT_DISABLESECURITYBOXES is active (need this check to make the boxes smashable
	if(CheatActive(CHEAT_DISABLESECURITYBOXES)) return;

	//SetCollisionCallback(actorInstance, NULL);
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "long idle", 1.0f, 1, 0, 0.1f, NONE);

#if(BPLATFORM == XBOX)// PP: DEMO FRIG: Dunno why, but this sound is reeeeeel fookin loud on Xbox
	PlaySample("sboxwakeup.wav",100);
#else// PP: not xbox
	PlaySample("sboxwakeup.wav",255);
#endif// PP: not xbox

	state = SECBOX_IDLE;
}


/*  --------------------------------------------------------------------------------
	Function	: SECURITYBOX::Update
	Purpose		: Perform update on the security box
	Parameters	: 
	Returns		: 
	Info		: 
*/
void SECURITYBOX::Update()
{
	int		i;		// PP: loop counter

	switch(state)
	{
	case SECBOX_INITIALISING:

		// TP: Add to the collision cache
		AddActorToCollisionCache(&collisionCache, actorInstance, 0);

		// TP: By default idle when set up
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "long idle", 1.0f, 1, 0, 0.1f, NONE);

		// TP: Find the trigger instance
		if (triggerCRC)
		{
			triggerBox = FindActorInstanceInMap(NULL, triggerCRC, &map);
			if (!triggerBox)
			{
				bkPrintf("*** WARNING *** Could not find security box [%s] trigger box.\n",actorInstance->instanceName);
			}
		}

		state = SECBOX_IDLE;
#ifdef SECBOX_DEBUG
		bkPrintf("SecBox state = SECBOX_IDLE\n");
#endif

		break;
	case SECBOX_IDLE:

		// PP: don't attack if CHEAT_DISABLESECURITYBOXES is active
		if(CheatActive(CHEAT_DISABLESECURITYBOXES)) return;

		// TP: wait for player to touch collision box
		if (triggerBox)
		{
			if (triggerBox->triggerObjectInfo->active)
			{
				// TP: character is inside the trigger box

				// TP: Find closest player
				for (i=0;i<gameStatus.multiplayer.numPlayers;i++)
				{
					// TP: if the player is invisible then ignore
					if ((gameStatus.player[i].actorInstance->characterInfo->powerUpInfo.invisible)||(!SafeToAttackTaz(gameStatus.player[i].actorInstance)))
					{
						continue;
					}
					if (gameStatus.player[i].actorInstance)
					{
						if (!playerAttacking)
						{
							playerAttacking = &gameStatus.player[i];
						}
						else
						{
							if (bmVectorDistance(gameStatus.player[i].actorInstance->actorInstance.position, actorInstance->actorInstance.position) <
								bmVectorDistance(playerAttacking->actorInstance->actorInstance.position, actorInstance->actorInstance.position))
							{
								playerAttacking = &gameStatus.player[i];
							}
						}
					}
				}

				if (playerAttacking)
				{
					if (characterSounds.sam == -1)
					{
						switch (bmRand()%3)
						{
						case 0:	CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sambox_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam); break;
						case 1:	CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sambox_2.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam); break;
						case 2:	CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sambox_3.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam); break;
						}
					}

					// TP: Red team start your attack run
					PlaySample("secboxspring.wav",255);

					PlayAnimationByName(actorInstance, "hit and recoil", 1.0f, 0, 0, 0.1f, NONE);
					SelectWeapon();
					state = SECBOX_STARTATTACK;
#ifdef SECBOX_DEBUG
				bkPrintf("SecBox state = SECBOX_STARTATTACK\n");
#endif
				}
				break;
			}
		}
		else
		{
			// TP: for now use an area based test
			for (i=0;i<gameStatus.multiplayer.numPlayers;i++)
			{
				if (gameStatus.player[i].actorInstance)
				{
					// TP: if the player is invisible then ignore
					if ((gameStatus.player[i].actorInstance->characterInfo->powerUpInfo.invisible)||(!SafeToAttackTaz(gameStatus.player[i].actorInstance)))
					{
						continue;
					}
					if (gameStatus.player[i].actorInstance->actorInstance.position[Y] > (actorInstance->actorInstance.position[Y] - SECURITYBOXMINYRANGE))
					{
						if (bmVectorDistance(gameStatus.player[i].actorInstance->actorInstance.position, actorInstance->actorInstance.position) <  SECURITYBOXACTIVATERANGE)
						{
							// TP: Red team start your attack run
							PlayAnimationByName(actorInstance, "hit and recoil", 1.0f, 0, 0, 0.1f, NONE);
							PlaySample("secboxspring.wav",255);
							SelectWeapon();
							state = SECBOX_STARTATTACK;
							playerAttacking = &gameStatus.player[i];
		#ifdef SECBOX_DEBUG
							bkPrintf("SecBox state = SECBOX_STARTATTACK\n");
		#endif
							break;
						}
					}
				}
			}
		}

		break;
	case SECBOX_STARTATTACK:

		if (armT < 1.0f)
		{
			armT += fTime*3;
			if (armT > 1.0f)
			{
				armT = 1.0f;
			}
		}
		if (armT)
		{
			CalculateArm();
		}

		if (!CheckIfPlayingAnimation(actorInstance,"hit and recoil"))
		{
			// TP: If attack finished then return to idle
			armT = 0.0f;
			CalculateArm();
			PlayAnimationByName(actorInstance, "long idle", 1.0f, 1, 0, 0.1f, NONE);
			state = SECBOX_IDLE;
#ifdef SECBOX_DEBUG
			bkPrintf("SecBox state = SECBOX_IDLE\n");
#endif
		}

		if (armT == 1.0f)
		{
			// TP: now catapult Taz
			TBVector	tempVector;
			float		tempFloat;

			bmVectorSub(tempVector, playerAttacking->actorInstance->actorInstance.position, actorInstance->actorInstance.position);
			tempFloat = bmVectorLen(tempVector);
			if (tempFloat)
			{
				bmVectorCDiv(tempVector,tempVector,tempFloat);
				tempVector[Y] = 0.5f;
				bmVectorCMul(tempVector,tempVector,METERTOUNIT(13.0f));
				bmVectorCopy(playerAttacking->actorInstance->actorBehaviour->physics.velocity, tempVector);
			}
			else
			{
				// TP: TODO throw taz in the direction the box is facing
			}
	
			PlaySample("whip.wav",255);

			// TP: UseStateChange callback
			// CMD: don't rumble if taz caged
			if((playerAttacking->actorInstance->actorStatus)&&(playerAttacking->actorInstance->actorStatus->currentState != STATE_CAGED))
			{
				if (EnterState(playerAttacking->actorInstance, STATE_PROJECTILESLIDE))
				{
					myStateChangeCallback(playerAttacking->actorInstance);
					RequestRumbleEffect(gameStatus.player[playerAttacking->actorInstance->playerNo].controller, 255, 0.0f, 0.5f, 0.5f, 0.0f, WAVEFORM_NORMAL);
				}
			}

			state = SECBOX_RETRACT;
#ifdef SECBOX_DEBUG
			bkPrintf("SecBox state = SECBOX_RETRACT\n");
#endif
		}

		break;
	case SECBOX_RETRACT:
		if (armT > 0.0f)
		{
			armT -= fTime*2;
			if (armT <= 0.0f)
			{
				armT = 0.0f;
				PlayAnimationByName(actorInstance, "long idle", 1.0f, 1, 0, 0.1f, NONE);
				FreeWeapon();
				playerAttacking = NULL;
				state = SECBOX_IDLE;
#ifdef SECBOX_DEBUG
				bkPrintf("SecBox state = SECBOX_IDLE\n");
#endif
			}
		}
		if (armT)
		{
			CalculateArm();
		}
		break;

	case SECBOX_SHUTDOWN:

		// PP: don't change the timer if it's negative (meaning don't ever reactivate)
		if(this->shutdownTime >= 0.0f)
		{
			shutdownTime -= fTime;
			if (shutdownTime <= 0.0f)
			{
				// TP: reactivate
				this->Reactivate();
			}
		}

		break;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: SECURITYBOX::SelectWeapon
	Purpose		: Choose a weapon and create the instance
	Parameters	: 
	Returns		: 
	Info		: 
*/
void SECURITYBOX::SelectWeapon(void)
{
	if (weapon) 
	{
		FreeActorInstance(weapon);
		weapon = NULL;
	}
	weapon = CreateActorInstance(&map.animatedInstances,"boxing.obe","boxing",actorInstance->actorInstance.position,
		NULL,actorInstance->actorInstance.orientation);

	// PP: on some platforms, whack-in-the-boxes are outlined
	if(MORE_CARTOON_RENDERING)
	{
		if (weapon)
		{
			EnableActorLineRenderMode(&weapon->actorInstance);
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: SECURITYBOX::FreeWeapon
	Purpose		: Get rid of the weapon used
	Parameters	: 
	Returns		: 
	Info		: 
*/
void SECURITYBOX::FreeWeapon(void)
{
	if (weapon)
	{
		FreeActorInstance(weapon);
		weapon = NULL;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: SECURITYBOX::CalculateArm
	Purpose		: Calculates points along the arm to use in draw streak
	Parameters	: t - 0-1 percentage of complete path to extend to, so arm extends rather than just appears
	Returns		: 
	Info		: Fills in armPoints ready for drawing
*/
void SECURITYBOX::CalculateArm()
{
	TBVector	start,cp1,cp2,end,offset,tempVector;
	float		tempFloat;
	int			i;

	// TP: Calculate position for control points

	// TP: start
	SETVECTOR(offset, 0.0f, METERTOUNIT(0.5f), 0.0f, 1.0f);
	bmVectorAdd(start, actorInstance->actorInstance.position, offset);

	// TP: end
	bmVectorAdd(end, playerAttacking->actorInstance->actorInstance.position, playerAttacking->actorInstance->actorBehaviour->collSphere.offset);
	// TP: set roughly at Taz's head
	end[Y] += METERTOUNIT(0.3f);

	// TP: 2 choices for the hit, above and to the side
	bmVectorSub(tempVector,end,start);
	tempVector[Y] = 0.0f;
	tempFloat = bmVectorLen(tempVector);
	if (tempFloat > METERTOUNIT(1.0f))
	{
		bmVectorCDiv(tempVector,tempVector,tempFloat);
		bmVectorCMul(tempVector,tempVector,METERTOUNIT(0.4f));
		bmVectorSub(end, end, tempVector);

		bmVectorLerp(cp1, start, end, 0.25f);
		bmVectorLerp(cp2, start, end, 0.75f);

		cp1[Y] += 250;
		cp2[Y] += 125;
	}
	else
	{
		// TP: above the box
		end[Y] -= METERTOUNIT(1.0f);

		bmVectorLerp(cp1, start, end, 0.25f);
		bmVectorLerp(cp2, start, end, 0.75f);
	}

	CalcCurve(start, cp1, cp2, end, SECBOXARMSEGS, armPoints);

	// TP: Set position for weapon
	i = bmFloatToInt(SECBOXARMSEGSF*armT);
	if (i<3) i=3;
	if (i>SECBOXARMSEGS) i=SECBOXARMSEGS;

	if (weapon)
	{
		// TP: set position and orientation of weapon
		bmVectorCopy(weapon->actorInstance.position,armPoints[i]);
		// TP: calc orientation from the last two armPoints
		if (i > 1)
		{
			TBVector tempVector;

			bmVectorSub(tempVector,armPoints[i-2],armPoints[i-1]);
			bmDirectionToQuat(weapon->actorInstance.orientation,tempVector,0.0f);
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: SECURITYBOX::Draw
	Purpose		: Draw any extras used by the box
	Parameters	: 
	Returns		: 
	Info		: 
*/
void SECURITYBOX::Draw()
{
	TBSavedRenderStates	saveBlock;
	float			sizeArray[SECBOXARMSEGS];
	int				colour[SECBOXARMSEGS][4];
	int				i, noofverts;
	TBTexture		*texture;
	TBPrimVertex2D	destPos[(SECBOXARMSEGS * 4) + 4];

	if (armT == 0.0f) return;

	bdSaveRenderStates(&saveBlock);

	for (i=0;i<SECBOXARMSEGS;i++) sizeArray[i] = METERTOUNIT(0.1f);
	for (i=0;i<SECBOXARMSEGS;i++) colour[i][0] = colour[i][1] = colour[i][2] = colour[i][3] = 128;

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	if (CheatActive(CHEAT_QWILSON))
	{
		texture = QwilsonTexture;
	}
	else
	{
		texture = textureCrank;
	}
	bdSetTexture(0, texture);

	i = bmFloatToInt(SECBOXARMSEGSF*armT);
	if (i<3) i=3;
	if (i>SECBOXARMSEGS) i=SECBOXARMSEGS;
	noofverts = bdDrawMultiStreakClipped(i, armPoints[0], sizeArray, sizeArray, &colour[0][0], destPos, 0.0f);

	if(noofverts)
	{
		CalculateTextureCoords(destPos,noofverts);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
		bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, FALSE);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, destPos, noofverts, BVERTTYPE_SINGLE2D);
	}
/*
	if (weapon)
	{
		// TP: set position and orientation of weapon
		bmVectorCopy(weapon->actorInstance.position,armPoints[i]);
		// TP: calc orientation from the last two armPoints
		if (i > 1)
		{
			TBVector tempVector;

			bmVectorSub(tempVector,armPoints[i-2],armPoints[i-1]);
			bmDirectionToQuat(weapon->actorInstance.orientation,tempVector,0.0f);
		}
	}
*/				
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdRestoreRenderStates(&saveBlock);
}

/*  --------------------------------------------------------------------------------
	Function	: SECURITYBOX::CalculateTextureCoords
	Purpose		: Fill in texture coords of arm for use with extendy arm texture
	Parameters	: prims, numPrims
	Returns		: new uv's in prims
	Info		: 
*/
void SECURITYBOX::CalculateTextureCoords(TBPrimVertex2D *prims, int numPrims)
{
	float	t;

	t = 1.0f;

	for (int i=0;i<numPrims;i+=2)
	{
		BDVERTEX_SETUV(&prims[i]	,0.0f,t);
		BDVERTEX_SETUV(&prims[i+1]	,1.0f,t);
		t -= 1.0f;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: SECURITYBOX::Init
	Purpose		: Default constructor
	Parameters	: 
	Returns		: 
	Info		: 
*/
int SECURITYBOX::Init(TBVector position, TBQuaternion orientation, uint32 triggerCRC,CPMNode *rootNode,CPMNode *node)
{

	// TP: Setup security box
	actorInstance = CreateActorInstance(&map.animatedInstances,SECURITYBOXNAME, "secbox");

	// PP: on some platforms, whack-in-the-boxes are outlined
	if(MORE_CARTOON_RENDERING)
	{
		EnableActorLineRenderMode(&actorInstance->actorInstance);
	}

	if (!actorInstance)
	{
		bkPrintf("*** WARNING *** Could not create security box.\n");
		return FALSE;
	}

	if (CreateDestructibleObjectInstance(actorInstance, DESTRUCTIBLETYPE_SPIN, NULL, 
									 0, 0, 0, 0, 0, 0, 
									 0.0f, 0.0f, 0.0f, 0, 100, 0))
	{
		// TP: See if there is any debris information
		if ((rootNode)&&(node))
		{
			actorInstance->destructibleObjectInfo->debris = GetDebrisObjectsFromMap("debrisObject", node);
			if(!actorInstance->destructibleObjectInfo->debris)
				actorInstance->destructibleObjectInfo->debris = GetDebrisObjectsFromMap("debrisObject", rootNode);
		}
		else
		{
			actorInstance->destructibleObjectInfo->debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));

			actorInstance->destructibleObjectInfo->debris->debrisCRC[0] = bkCRC32((uchar*) "textures\\whackbit01.bmp", strlen("textures\\whackbit01.bmp"), 0);
			actorInstance->destructibleObjectInfo->debris->debrisCRC[1] = bkCRC32((uchar*) "textures\\whackbit02.bmp", strlen("textures\\whackbit02.bmp"), 0);
			actorInstance->destructibleObjectInfo->debris->debrisCRC[2] = bkCRC32((uchar*) "textures\\whackbit03.bmp", strlen("textures\\whackbit03.bmp"), 0);
			actorInstance->destructibleObjectInfo->debris->debrisCRC[3] = bkCRC32((uchar*) "textures\\whackbit04.bmp", strlen("textures\\whackbit04.bmp"), 0);
			actorInstance->destructibleObjectInfo->debris->debrisCRC[4] = bkCRC32((uchar*) "textures\\whackbit05.bmp", strlen("textures\\whackbit05.bmp"), 0);
			actorInstance->destructibleObjectInfo->debris->numDebris = 5;
		}
	}

	// TP: clear collision callback set by createdestructible
	//SetCollisionCallback(actorInstance, NULL);
	SetCollisionCallback(actorInstance, RespondToSecurityBoxCollision);

	EnableActorLineRenderMode(&actorInstance->actorInstance);

	if (position)
	{
		SetPosition(position);
	}
	if (orientation)
	{
		SetOrientation(orientation);
	}

	this->triggerCRC = triggerCRC;
	triggerBox = NULL;
	weapon = NULL;
	playerAttacking = NULL;

	// TP: Add to list
	list.append(this);

	// TP: Set initial state
	state = SECBOX_INITIALISING;

	// TP: Set armT value
	armT = 0.0f;

	bkPrintf("Security box created.\n");

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: SECURITYBOX::~TAG_SECURITYBOX
	Purpose		: Default destructor
	Parameters	: 
	Returns		: 
	Info		: 
*/
SECURITYBOX::~TAG_SECURITYBOX()
{
	// TP: Remove from list
	list.remove(this);

	// TP: Free actor instance
	if (actorInstance) 
	{
		if (actorInstance->destructibleObjectInfo->debris)
		{
			SAFE_FREE(actorInstance->destructibleObjectInfo->debris);
		}
		FreeActorInstance(actorInstance);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: RespondToSecurityBoxCollision
	Purpose		: Destroy security box
	Parameters	: 
	Returns		: 
	Info		: 
*/
int RespondToSecurityBoxCollision(ACTORINSTANCE *collider, ACTORINSTANCE *secBox)
{
	SECURITYBOX	*securityBox;
	int			i;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,secBox);
		return FALSE;
	}
	
	// TP: if the collider is not taz then return
	if (collider->playerNo!=0) return FALSE;

	for (i=SECURITYBOX::list.getSize()-1;i>=0;i--)
	{
		securityBox = SECURITYBOX::list.get(i);

		if (securityBox->actorInstance == secBox) break;
	}

	if ((securityBox->GetState() != SECBOX_SHUTDOWN)&&(collider->actorStatus->currentState != STATE_VEHICLE)) return FALSE;

	if (RespondToDestructibleCollision(collider,secBox))
	{
		// TP: RespondToDestructibleCollision will free the actorInstance
		securityBox->actorInstance = NULL;
		if (securityBox) 
		{
			delete securityBox;
		}

		return TRUE;
	}
	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: SECURITYBOX::Shutdown
	Purpose		: Shutdown the security box for given time
	Parameters	: (// PP: opt/-1.0f=forever) time to shutdown for
	Returns		: 
	Info		: 
*/
void SECURITYBOX::Shutdown(float time)
{
	state = SECBOX_SHUTDOWN;
	PlayAnimationByName(actorInstance, "sleep idle", 1.0f, 1, 0, 0.0f, NONE);
	//SetCollisionCallback(actorInstance, RespondToSecurityBoxCollision);
	shutdownTime = time;
}

/*  --------------------------------------------------------------------------------
	Function	: KillSecurityBoxes
	Purpose		: Removes all security boxes from a level
	Parameters	: void
	Returns		: 
	Info		: 
*/
void KillSecurityBoxes(void)
{
	bkPrintf("Destroying all security boxes.\n");
	SECURITYBOX	*securityBox;
	int			i;

	for (i=SECURITYBOX::list.getSize()-1;i>=0;i--)
	{
		securityBox = SECURITYBOX::list.get(i);

		delete securityBox;
	}

	KillSpinPads();
}

/*  --------------------------------------------------------------------------------
	Function	: UpdateSecurityBoxes
	Purpose		: Update all security boxes from a level
	Parameters	: void
	Returns		: 
	Info		: 
*/
void UpdateSecurityBoxes(void)
{
	SECURITYBOX	*securityBox;
	int			i;

	for (i=SECURITYBOX::list.getSize()-1;i>=0;i--)
	{
		securityBox = SECURITYBOX::list.get(i);

		securityBox->Update();
	}

	UpdateSpinPads();
}

/*  --------------------------------------------------------------------------------
	Function	: DrawSecurityBoxes
	Purpose		: Draw any extras on the security boxes
	Parameters	: void
	Returns		: 
	Info		: 
*/
void DrawSecurityBoxes(void)
{
	SECURITYBOX	*securityBox;
	int			i;

	for (i=SECURITYBOX::list.getSize()-1;i>=0;i--)
	{
		securityBox = SECURITYBOX::list.get(i);

		securityBox->Draw();
	}
}

/*  --------------------------------------------------------------------------------
	Function	: ShutdownSecurityBoxes
	Purpose		: Shutdown all security boxes
	Parameters	: // PP: (opt/false) shutdown indefinitely?
	Returns		: 
	Info		: 
*/
void ShutdownSecurityBoxes(const bool forever)
{
	SECURITYBOX	*securityBox;
	int			i;
	float		shutdownTime;		// PP: time for which to tell security boxes to be inactive, or -1.0f meaning forever

	// PP: -1.0f can be passed to SECURITYBOX::shutdown to deactivate indefinitely
	if(forever)
	{
		shutdownTime= -1.0f;
	}
	else
	{
		switch(gameStatus.globalSettings.difficulty)
		{
		case DIFF_EASY:
			shutdownTime=15.0f;
			break;
		case DIFF_MEDIUM:
			shutdownTime=12.5f;
			break;
		case DIFF_HARD:
			shutdownTime=10.0f;
			break;
		}
	}

	for (i=SECURITYBOX::list.getSize()-1;i>=0;i--)
	{
		securityBox = SECURITYBOX::list.get(i);

		securityBox->Shutdown(shutdownTime);
	}

	timeToTing = gameStatus.gameTime + shutdownTime;
}

/*  --------------------------------------------------------------------------------
	Function	: ReactivateSecurityBoxes
	Purpose		: Re-activate all security boxes
	Parameters	: 
	Returns		: 
	Info		: // PP: 
*/
void ReactivateSecurityBoxes(void)
{
	SECURITYBOX	*securityBox;
	int			i;

	for (i=SECURITYBOX::list.getSize()-1;i>=0;i--)
	{
		securityBox = SECURITYBOX::list.get(i);

		securityBox->Reactivate();
	}
}

/* TP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														SPINPAD methods

									Spin pad gadgets that shut down the security boxes

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
	Anims:
		Button returns to normal (this 
		Idle (where it has stopped at t
		Taz spins on button
*/

#define	SPINPADNAME		"spinpad.obe"

LINKEDLIST<SPINPAD>		SPINPAD::list("spinpads");		// TP: List of all spin pads in existance
int						SPINPAD::soundHandle;			// TP: sound handle only one needed
char					SPINPAD::spinPadsActive=0;		// TP: counter of the number of spin pads that have been activated


/*  --------------------------------------------------------------------------------
	Function	: int SPINPAD::Init
	Purpose		: Create and intialise a spin box
	Parameters	: position and orientation
	Returns		: 
	Info		: 
*/
int SPINPAD::Init(TBVector position, TBQuaternion orientation, int flags)
{
	// TP: Setup spin pad
	actorInstance = CreateActorInstance(&map.animatedInstances,SPINPADNAME, "spinpad",position,NULL,orientation,flags);

	if (!actorInstance)
	{
		bkPrintf("*** WARNING *** Could not create spin pad.\n");
		return FALSE;
	}

	EnableActorLineRenderMode(&actorInstance->actorInstance);

	if (position)
	{
		SetPosition(position);
	}
	if (orientation)
	{
		SetOrientation(orientation);
	}

	soundHandle = -1;

	// TP: Add to list
	list.append(this);

	// TP: Set initial state
	state = SPINPAD_INITIALISING;


	bkPrintf("Spin pad created.\n");

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: SPINPAD::~TAG_SPINPAD
	Purpose		: Default destructor, kills the spin pad
	Parameters	: 
	Returns		: 
	Info		: 
*/
SPINPAD::~TAG_SPINPAD()
{
	if ((state!=SPINPAD_IDLE)&&(state!=SPINPAD_INITIALISING))
	{
		spinPadsActive--;
		if ((spinPadsActive==0)&&(soundHandle!=-1))
		{
			StopChannel(&soundHandle);
		}
	}

	// TP: Remove from list
	list.remove(this);

	// TP: Free actor instance
	FreeActorInstance(actorInstance);
}

/*  --------------------------------------------------------------------------------
	Function	: SpinPadCollisionCallback
	Purpose		: Called in responce to collision
	Parameters	: 
	Returns		: 
	Info		: 
*/
int SpinPadCollisionCallback(ACTORINSTANCE *collider, ACTORINSTANCE *spinPadActor)
{
	// TP: Find pad, nasty!
	SPINPAD		*spinPad;
	int			i;

	if ((collider->playerNo!=-1)&&(collider->flags&ACTORFLAG_DEBRISACTOR)) return FALSE;

	for (i=SPINPAD::list.getSize()-1;i>=0;i--)
	{
		spinPad = SPINPAD::list.get(i);

		if (spinPad->actorInstance == spinPadActor)
		{
			// TP: found it!
			if ((collider->actorStatus)&&(collider->actorStatus->currentState==STATE_SPIN))
			{
				spinPad->PlayerSpinOn(&gameStatus.player[collider->playerNo]);
			}
		}
	}

	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: SPINPAD::PlayerSpinOn
	Purpose		: Called each frame player is spinning on pad
	Parameters	: player spinning on pad
	Returns		: 
	Info		: 
*/
void SPINPAD::PlayerSpinOn(struct TAG_PLAYER *player)
{
	if (state == SPINPAD_SPINDOWN)
	{
		spinOnCounter += fTime;
	}

	switch(state)
	{
	case SPINPAD_COUNTDOWN:
		PlayAnimationByName(actorInstance, "button returns to normal (this ", 2.4f, 0, 0, 0.2f, NONE);
		ShutdownSecurityBoxes();
		break;
	case SPINPAD_IDLE:
		PlaySample("spinningbutton.wav",255,actorInstance->actorInstance.position);
		PlayAnimationByName(actorInstance, "taz spins on button", 3.0f, 0, 0, 0.0f, NONE);
		state = SPINPAD_SPINDOWN;
		break;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: void SPINPAD::Update
	Purpose		: Update the spin pad
	Parameters	: 
	Returns		: 
	Info		: 
*/
void SPINPAD::Update()
{
	switch(state)
	{
	case SPINPAD_INITIALISING:

		// TP: Add to the collision cache
		AddActorToCollisionCache(&collisionCache, actorInstance, CACHEFLAGS_LIVEUPDATE);
		SetCollisionCallback(actorInstance, SpinPadCollisionCallback);
		spinOnCounter = 0.0f;
		state = SPINPAD_IDLE;

		break;

	case SPINPAD_IDLE:

		// TP: wait for contact with spinning taz
		break;
	case SPINPAD_SPINDOWN:
		if (!CheckIfPlayingAnimation(actorInstance, "taz spins on button"))
		{
			if (soundHandle==-1)
			{
				soundHandle = PlaySample("buttonticking.wav",255,NULL,0);
			}
			spinPadsActive++;
			// TP: finished spin down anim, shut down the security boxes
			PlayAnimationByName(actorInstance, "button returns to normal (this ", 2.4f, 0, 0, 0.0f, NONE);
			ShutdownSecurityBoxes();
			state = SPINPAD_COUNTDOWN;
		}
		break;
	case SPINPAD_COUNTDOWN:
		if (timeToTing < gameStatus.gameTime && timeToTing != -1.0f)
		{
			state = SPINPAD_IDLE;
			spinPadsActive--;

			if ((spinPadsActive==0)&&(soundHandle!=-1))
			{
				StopChannel(&soundHandle);

				timeToTing = -1.0f;
			}
		}
		break;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: KillSpinPads
	Purpose		: Removes all spin pads from a level
	Parameters	: void
	Returns		: 
	Info		: 
*/
void KillSpinPads(void)
{
	SPINPAD		*spinPad;
	int			i;

	bkPrintf("Destroying all spin pads.\n");

	for (i=SPINPAD::list.getSize()-1;i>=0;i--)
	{
		spinPad = SPINPAD::list.get(i);

		delete spinPad;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: UpdateSpinPads
	Purpose		: Update all security boxes from a level
	Parameters	: void
	Returns		: 
	Info		: 
*/
void UpdateSpinPads(void)
{
	SPINPAD		*spinPad;
	int			i;

	for (i=SPINPAD::list.getSize()-1;i>=0;i--)
	{
		spinPad = SPINPAD::list.get(i);

		spinPad->Update();
	}
}
