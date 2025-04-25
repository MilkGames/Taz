#define	PATHSPEED	(0.25f)
#define	JUMPSPEED	(0.2f)

#include "languageanimation.h"
#include "frontendanimation.h"

TBVector	*vFlagPos;

int InitialiseLanguageAnimationPaths(void)
{
	ACTORINSTANCE		*tempInstance;

	vFlagPos = new TBVector[NUM_FL_USA_ENTRIES];
	
	TBActorNodeInstance	*node;
	
	LoadActor("objects\\path_usa_all.obe", map.mapName);
	
	tempInstance = CreateActorInstance(&map.animatedInstances, "objects\\path_usa_all.obe", "path");
	
	SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
	SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
	
	bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
	tempInstance->flags |= ACTORFLAG_CUSTOMDRAW;

	ResetAnimPathToStart(tempInstance,"englishtospanish");
	node = baFindNode(tempInstance->actorInstance.rootNodeInstance, "path_usa_all");
	baGetNodesWorldPosition(&tempInstance->actorInstance, node, vFlagPos[FL_USA_ENGLISH]);

	ResetAnimPathToStart(tempInstance,"spanishtofrench");
	node = baFindNode(tempInstance->actorInstance.rootNodeInstance, "path_usa_all");
	baGetNodesWorldPosition(&tempInstance->actorInstance, node, vFlagPos[FL_USA_SPANISH]);

	ResetAnimPathToStart(tempInstance,"frenchtoitalian");
	node = baFindNode(tempInstance->actorInstance.rootNodeInstance, "path_usa_all");
	baGetNodesWorldPosition(&tempInstance->actorInstance, node, vFlagPos[FL_USA_FRENCH]);

	ResetAnimPathToStart(tempInstance,"italiantogerman");
	node = baFindNode(tempInstance->actorInstance.rootNodeInstance, "path_usa_all");
	baGetNodesWorldPosition(&tempInstance->actorInstance, node, vFlagPos[FL_USA_ITALIAN]);

	ResetAnimPathToStart(tempInstance,"germantoenglish");
	node = baFindNode(tempInstance->actorInstance.rootNodeInstance, "path_usa_all");
	baGetNodesWorldPosition(&tempInstance->actorInstance, node, vFlagPos[FL_USA_GERMAN]);

	ResetAnimPathToStart(tempInstance,"englishtogerman");
	ResetAnimPathToStart(tempInstance,"germantoitalian");
	ResetAnimPathToStart(tempInstance,"italiantofrench");
	ResetAnimPathToStart(tempInstance,"frenchtospanish");
	ResetAnimPathToStart(tempInstance,"spanishtoenglish");
	
	LanguageSelectInfo->Path = tempInstance;
	
	return 0;
}

void ChoosePathAnimationToPlay()
{
	if (LanguageSelectInfo->currentFlag == LanguageSelectInfo->targetFlag)
		return;

	if (LanguageSelectInfo->Tweety->state == TWEETYSTATE_FOLLOW)
		return;
	
	FlushAnimationQueue(LanguageSelectInfo->Path);

	switch (LanguageSelectInfo->currentFlag)
	{
	case FL_USA_ENGLISH:	switch (LanguageSelectInfo->targetFlag)
							{
							case FL_USA_FRENCH:		
							case FL_USA_SPANISH:	LanguageSelectInfo->nextFlag = FL_USA_SPANISH;
													ResetAnimPathToStart(LanguageSelectInfo->Path, "englishtospanish");
													PlayAnimationByName(LanguageSelectInfo->Path, "englishtospanish",  PATHSPEED, 0, 0, 0, NONE);
													break;

							default:				LanguageSelectInfo->nextFlag = FL_USA_GERMAN;
													ResetAnimPathToStart(LanguageSelectInfo->Path, "englishtogerman");
													PlayAnimationByName(LanguageSelectInfo->Path, "englishtogerman",  JUMPSPEED, 0, 0, 0, NONE);
													break;
							}

							break;

	case FL_USA_SPANISH:	switch (LanguageSelectInfo->targetFlag)
							{
							case FL_USA_ENGLISH:								
							case FL_USA_GERMAN:	LanguageSelectInfo->nextFlag = FL_USA_ENGLISH;
													PlayAnimationByName(LanguageSelectInfo->Path, "spanishtoenglish",  PATHSPEED, 0, 0, 0, NONE);
													break;
							
							default:				LanguageSelectInfo->nextFlag = FL_USA_FRENCH;
													PlayAnimationByName(LanguageSelectInfo->Path, "spanishtofrench",  PATHSPEED, 0, 0, 0, NONE);
													break;
							}

							break;

	case FL_USA_FRENCH:		switch (LanguageSelectInfo->targetFlag)
							{
							case FL_USA_ENGLISH:
							case FL_USA_SPANISH:	LanguageSelectInfo->nextFlag = FL_USA_SPANISH;
													PlayAnimationByName(LanguageSelectInfo->Path, "frenchtospanish",  PATHSPEED, 0, 0, 0, NONE);
													break;

							default:				LanguageSelectInfo->nextFlag = FL_USA_ITALIAN;										
													PlayAnimationByName(LanguageSelectInfo->Path, "frenchtoitalian",  PATHSPEED, 0, 0, 0, NONE);
													break;
							}

							break;

	case FL_USA_GERMAN:	switch (LanguageSelectInfo->targetFlag)
							{
							case FL_USA_ENGLISH:	
							case FL_USA_SPANISH:	LanguageSelectInfo->nextFlag = FL_USA_ENGLISH;
													PlayAnimationByName(LanguageSelectInfo->Path, "germantoenglish",  PATHSPEED, 0, 0, 0, NONE);
													break;

							default:				LanguageSelectInfo->nextFlag = FL_USA_ITALIAN;
													PlayAnimationByName(LanguageSelectInfo->Path, "germantoitalian",  JUMPSPEED, 0, 0, 0, NONE);
													break;
							}
							break;

	case FL_USA_ITALIAN:	switch (LanguageSelectInfo->targetFlag)
							{
							case FL_USA_ENGLISH:	
							case FL_USA_GERMAN:	LanguageSelectInfo->nextFlag = FL_USA_GERMAN;
													PlayAnimationByName(LanguageSelectInfo->Path, "italiantogerman",  JUMPSPEED, 0, 0, 0, NONE);
													break;

							default:				LanguageSelectInfo->nextFlag = FL_USA_FRENCH;
													PlayAnimationByName(LanguageSelectInfo->Path, "italiantofrench",  PATHSPEED, 0, 0, 0, NONE);
													break;
							}
							break;
	}

	return;
}

void ChooseSylvesterAnimationToPlay(ACTORINSTANCE *Sylvester)
{
	TBActorAnimSegment	*animSegment;
	
	if (!LanguageSelectInfo->SelectMade)
	{
		if (!CheckIfPlayingAnimation(LanguageSelectInfo->Path, NULL))
		{
			if (LanguageSelectInfo->currentFlag == LanguageSelectInfo->targetFlag || 
				LanguageSelectInfo->Tweety->state == TWEETYSTATE_FOLLOW)
			{
				//We're on the target flag play a catch anim
				if (CheckIfPlayingAnimation(Sylvester, "grabtweety(upper)1") ||
					CheckIfPlayingAnimation(Sylvester, "grabtweety(upper)2") ||
					CheckIfPlayingAnimation(Sylvester, "grabtweety(upper)3") ||
					CheckIfPlayingAnimation(Sylvester, "grabtweety(upper)4") ||
					CheckIfPlayingAnimation(Sylvester, "grabtweety(lower)1") ||
					CheckIfPlayingAnimation(Sylvester, "grabtweety(lower)2") ||
					CheckIfPlayingAnimation(Sylvester, "grabtweety(lower)3") ||
					CheckIfPlayingAnimation(Sylvester, "grabtweety(lower)4"))
				{
					//we're already playing a catch anim
				}
				else
				{
					//we're not : play one

					FlushAnimationQueue(&Sylvester->actorInstance);
					animSegment = ChooseCatchAnimForSylvester(Sylvester);				
					PlayAnimationBySegmentPointer(Sylvester, animSegment, 1.0f, 0, 0, 0.2f, NONE);
				}
			}
			else
			{
				//Tweety's stopped flying
				//We're not on the right flag - run to it

				FlushAnimationQueue(&Sylvester->actorInstance);
			}
		}
		else
		{			
			switch(LanguageSelectInfo->currentFlag)
			{
			case FL_USA_ENGLISH:	switch (LanguageSelectInfo->targetFlag)
									{
									case FL_USA_FRENCH:
									case FL_USA_SPANISH:if (!CheckIfPlayingAnimation(Sylvester, "runrightupper"))
														{
															FlushAnimationQueue(&Sylvester->actorInstance);
															bmQuatCopy(Sylvester->actorInstance.orientation, bIdentityQuaternion);
															PlayAnimationByName(Sylvester, "runrightupper", 1.0f, 1, 0, 0.2f, NONE);
														}
														break;

									default:			if (!CheckIfPlayingAnimation(Sylvester, "jumpdown"))
														{
															FlushAnimationQueue(&Sylvester->actorInstance);
															bmQuatCopy(Sylvester->actorInstance.orientation, bIdentityQuaternion);
															PlayAnimationByName(Sylvester, "jumpdown", 1.0f, 0, 0, 0.0f, NONE);

															PlaySample("sylvester jump down.wav", 255);
														}
														break;
									}
									break;
				
			case FL_USA_SPANISH:	switch (LanguageSelectInfo->targetFlag)
									{
									case FL_USA_ENGLISH:
									case FL_USA_GERMAN:	if (!CheckIfPlayingAnimation(Sylvester, "runleftupper"))
															{
																FlushAnimationQueue(&Sylvester->actorInstance);
																bmQuatCopy(Sylvester->actorInstance.orientation, bIdentityQuaternion);
																PlayAnimationByName(Sylvester, "runleftupper", 1.0f, 1, 0, 0.2f, NONE);
															}
															break;
						
									default:				if (!CheckIfPlayingAnimation(Sylvester, "runrightupper"))
															{
																FlushAnimationQueue(&Sylvester->actorInstance);
																bmQuatCopy(Sylvester->actorInstance.orientation, bIdentityQuaternion);
																PlayAnimationByName(Sylvester, "runrightupper", 1.0f, 1, 0, 0.2f, NONE);
															}
															break;
									}				
									break;
				
			case FL_USA_FRENCH:		switch (LanguageSelectInfo->targetFlag)
									{
									case FL_USA_ENGLISH:									
									case FL_USA_SPANISH:	if (!CheckIfPlayingAnimation(Sylvester, "runleftupper"))
															{
																FlushAnimationQueue(&Sylvester->actorInstance);
																bmQuatCopy(Sylvester->actorInstance.orientation, bIdentityQuaternion);
																PlayAnimationByName(Sylvester, "runleftupper", 1.0f, 1, 0, 0.2f, NONE);
															}
															break;
					
									default:				if (!CheckIfPlayingAnimation(Sylvester, "jumpdown"))
															{
																FlushAnimationQueue(&Sylvester->actorInstance);
																bmQuatCopy(Sylvester->actorInstance.orientation, bIdentityQuaternion);
																PlayAnimationByName(Sylvester, "jumpdown", 1.0f, 1, 0, 0.0f, NONE);

																PlaySample("sylvester jump down.wav", 255);
															}
															break;
									}				
									break;
				
			case FL_USA_GERMAN:	switch (LanguageSelectInfo->targetFlag)
									{
									case FL_USA_ENGLISH:
									case FL_USA_SPANISH:	if (!CheckIfPlayingAnimation(Sylvester, "jumpup"))
															{
																FlushAnimationQueue(&Sylvester->actorInstance);
																bmQuatCopy(Sylvester->actorInstance.orientation, bIdentityQuaternion);
																PlayAnimationByName(Sylvester, "jumpup", 1.0f, 1, 0, 0.0f, NONE);

																PlaySample("sylvester jump up.wav", 255);
															}
															break;

									default:				if (!CheckIfPlayingAnimation(Sylvester, "runrightlower"))
															{
																FlushAnimationQueue(&Sylvester->actorInstance);
																bmQuatCopy(Sylvester->actorInstance.orientation, bIdentityQuaternion);
																PlayAnimationByName(Sylvester, "runrightlower", 1.0f, 0, 0, 0.2f, NONE);
															}
															break;
									}
									break;
			
			case FL_USA_ITALIAN:	switch (LanguageSelectInfo->targetFlag)
									{
									case FL_USA_ENGLISH:
									case FL_USA_GERMAN:	if (!CheckIfPlayingAnimation(Sylvester, "runleftlower"))
															{
																FlushAnimationQueue(&Sylvester->actorInstance);
																bmQuatCopy(Sylvester->actorInstance.orientation, bIdentityQuaternion);
																PlayAnimationByName(Sylvester, "runleftlower", 1.0f, 0, 0, 0.2f, NONE);
															}
															break;

									default:				if (!CheckIfPlayingAnimation(Sylvester, "jumpup"))
															{
																FlushAnimationQueue(&Sylvester->actorInstance);
																bmQuatCopy(Sylvester->actorInstance.orientation, bIdentityQuaternion);
																PlayAnimationByName(Sylvester, "jumpup", 1.0f, 1, 0, 0.0f, NONE);

																PlaySample("sylvester jump up.wav", 255);
															}
															break;
									}
									break;
			}
		}
	}
}

void ChooseAnimationForPodium(ACTORINSTANCE *Podium)
{
	FlushAnimationQueue(Podium);

	switch (LanguageSelectInfo->currentFlag)
	{
	case FL_USA_ENGLISH:	PlayAnimationByName(Podium, "english",  1.0f, 0, 0, 0.0f, NONE);	break;
	case FL_USA_FRENCH:		PlayAnimationByName(Podium, "french",  1.0f, 0, 0, 0.0f, NONE);		break;
	case FL_USA_SPANISH:	PlayAnimationByName(Podium, "spanish",  1.0f, 0, 0, 0.0f, NONE);		break;
	case FL_USA_ITALIAN:	PlayAnimationByName(Podium, "italian",  1.0f, 0, 0, 0.0f, NONE);	break;
	case FL_USA_GERMAN:		PlayAnimationByName(Podium, "german",  1.0f, 0, 0, 0.0f, NONE);	break;
	}
}

void UpdatePathAnimation(ACTORINSTANCE *Sylvester)
{
	if (!LanguageSelectInfo->Tweety)
		return;

	if ((LanguageSelectInfo->currentFlag == LanguageSelectInfo->nextFlag) &&
		(LanguageSelectInfo->Tweety->state == TWEETYSTATE_FOLLOW))
		return;

	if (CheckIfPlayingAnimation(LanguageSelectInfo->Path, NULL))
		return;

	LanguageSelectInfo->currentFlag = LanguageSelectInfo->nextFlag;

	ChoosePathAnimationToPlay();
}

void UpdateSylvesterPosition(ACTORINSTANCE *Sylvester)
{
	if (!Sylvester)	return;

	TBActorNodeInstance	*node;

	int	result = CheckIfPlayingAnimation(LanguageSelectInfo->Path, NULL);

	if (result)
	{
		node = baFindNode(LanguageSelectInfo->Path->actorInstance.rootNodeInstance,"path_usa_all");
		
		baGetNodesWorldPosition(&LanguageSelectInfo->Path->actorInstance, node, Sylvester->actorInstance.position);
	}
	//else
		//bmVectorCopy(Sylvester->actorInstance.position, vFlagPos[LanguageSelectInfo->currentFlag]);

	SetShadowNode(Sylvester, NULL, "root");
}

void ResetAnimPathToStart(ACTORINSTANCE *actorInstance, char *animName)
{
	TBActorAnimSegment	*segment;

	//reset path
	FlushAnimationQueue(actorInstance);
	segment = baFindAnimSegment(&actorInstance->actorInstance,animName);
	baSetNodeFrame(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,segment->startFrame,TRUE);
	baPrepActorInstance(&actorInstance->actorInstance,TRUE);
}

void ResetAnimPathToEnd(ACTORINSTANCE *actorInstance, char *animName)
{
	TBActorAnimSegment	*segment;

	//reset path
	FlushAnimationQueue(actorInstance);
	segment = baFindAnimSegment(&actorInstance->actorInstance,animName);
	baSetNodeFrame(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,segment->endFrame,TRUE);
	baPrepActorInstance(&actorInstance->actorInstance,TRUE);
}

enum LS_USA_FLAGS GetNextFlag(int current, int direction)
{
	switch (current)
	{ 
	case FL_USA_ENGLISH:	switch (direction)
						{
						case FL_GO_RIGHT:	return FL_USA_SPANISH;
						case FL_GO_DOWN:	return FL_USA_GERMAN;
						default:			return FL_USA_ENGLISH;
						}
						break;

	case FL_USA_SPANISH:	switch (direction)
						{
						case FL_GO_DOWN:	return FL_USA_GERMAN;
						case FL_GO_LEFT:	return FL_USA_ENGLISH;
						case FL_GO_RIGHT:	return FL_USA_FRENCH;
						default:			return FL_USA_SPANISH;
						}
						break;

	case FL_USA_FRENCH:		switch (direction)
						{
						case FL_GO_DOWN:	return FL_USA_ITALIAN;
						case FL_GO_LEFT:	return FL_USA_SPANISH;
						default:			return FL_USA_FRENCH;
						}
						break;

	case FL_USA_GERMAN:	switch (direction)
							{
							case FL_GO_UP:		return FL_USA_ENGLISH;
							case FL_GO_RIGHT:	return FL_USA_ITALIAN;
							default:			return FL_USA_GERMAN;
							}
							break;

	case FL_USA_ITALIAN:	switch (direction)
							{
							case FL_GO_LEFT:	return FL_USA_GERMAN;
							case FL_GO_UP:		return FL_USA_FRENCH;
							default:			return FL_USA_ITALIAN;
							}
							break;
	}
}

TBActorAnimSegment * ChooseCatchAnimForSylvester(ACTORINSTANCE *actorInstance)
{
	if (LanguageSelectInfo->currentFlag == FL_USA_ENGLISH ||
		LanguageSelectInfo->currentFlag == FL_USA_FRENCH ||
		LanguageSelectInfo->currentFlag == FL_USA_SPANISH)
	{
		switch(bmRand() % 4)
		{
		case 0:	return baFindAnimSegment(&actorInstance->actorInstance, "grabtweety(upper)1");
		case 1:	return baFindAnimSegment(&actorInstance->actorInstance, "grabtweety(upper)2");
		case 2:	return baFindAnimSegment(&actorInstance->actorInstance, "grabtweety(upper)3");
		case 3:	return baFindAnimSegment(&actorInstance->actorInstance, "grabtweety(upper)4");
		NODEFAULT;// PP: there should be no unhandled case here
		}
	}
	else
	{
		switch(bmRand() % 4)
		{
		case 0:	return baFindAnimSegment(&actorInstance->actorInstance, "grabtweety(lower)1");
		case 1:	return baFindAnimSegment(&actorInstance->actorInstance, "grabtweety(lower)2");
		case 2:	return baFindAnimSegment(&actorInstance->actorInstance, "grabtweety(lower)3");
		case 3:	return baFindAnimSegment(&actorInstance->actorInstance, "grabtweety(lower)4");
		NODEFAULT;// PP: there should be no unhandled case here
		}
	}
}

void SylvesterLookAtTweety(ACTORINSTANCE *Sylvester, TBVector Tweety)
{
	TBVector		vec;
	TBQuaternion	Quat;

	bmVectorSub(vec, Tweety, Sylvester->actorInstance.position);
	
	vec[Y] = 0;
	vec[W] = 1.0f;
	
	if ((vec[X] == 0.0f)&&(vec[Z] == 0.0f)) return;
	
	bmDirectionToQuat(Quat, vec, 0.0f);

	bmQuatCopy(Sylvester->actorInstance.orientation, Quat);
}