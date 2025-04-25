// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : resettutorial.cpp
//   Purpose : info to reset tutorial after completion
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"

#include "tutorial.h"
#include "destructibles.h"
#include "securitybox.h"

struct	ObjectInfo
{
	TBVector		Pos;
	TBVector		Scale;
	TBQuaternion	Quat;
	short			Flags;
	char			Name[16];
};

struct	TUTORIALONEINFO
{
	ObjectInfo Gate;

	TBActorAnimSegment *Idle;
	TBActorAnimSegment *First;
	TBActorAnimSegment *Second;
	TBActorAnimSegment *Destroy;

	ObjectInfo	Trees[9];
	ObjectInfo	Crate[2];
	ObjectInfo	Crates[2];

}	tutOneInfo;

struct	TUTORIALTHRINFO
{
	TBVector		SBPos;
	TBQuaternion	SBQuat;

	TBVector		WPPos;
	TBQuaternion	WPQuat;
	short			WPFlags;
}	tutThrInfo;

void GetTutorialAreaInfo()
{
	GetTutOneInfo();
	GetTutThreeInfo();
}

void GetTutOneInfo()
{
	ACTORINSTANCE	*Actor;

	Actor = FindActorInstanceInMap("maingatenull", 0, &map);

	bmVectorCopy(tutOneInfo.Gate.Pos, Actor->actorInstance.position);
	bmQuatCopy(tutOneInfo.Gate.Quat, Actor->actorInstance.orientation);
	tutOneInfo.Gate.Flags= Actor->flags;

	tutOneInfo.Idle		= baFindAnimSegment(&Actor->actorInstance, "idle");
	tutOneInfo.First	= baFindAnimSegment(&Actor->actorInstance, "1st_hit");
	tutOneInfo.Second	= baFindAnimSegment(&Actor->actorInstance, "2nd_hit");
	tutOneInfo.Destroy	= baFindAnimSegment(&Actor->actorInstance, "3rd_hit");

	Actor = FindActorInstanceInMap("bank_treeb_i15", 0, &map);

	bmVectorCopy(tutOneInfo.Trees[0].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Trees[0].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Trees[0].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Trees[0].Flags= Actor->flags;
	strcpy(tutOneInfo.Trees[0].Name, "bank_treeb_i15");

	Actor = FindActorInstanceInMap("bank_treeb_i12", 0, &map);

	bmVectorCopy(tutOneInfo.Trees[1].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Trees[1].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Trees[1].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Trees[1].Flags= Actor->flags;
	strcpy(tutOneInfo.Trees[1].Name, "bank_treeb_i12");

	Actor = FindActorInstanceInMap("bank_treeb_i10", 0, &map);

	bmVectorCopy(tutOneInfo.Trees[2].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Trees[2].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Trees[2].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Trees[2].Flags= Actor->flags;
	strcpy(tutOneInfo.Trees[2].Name, "bank_treeb_i10");

	Actor = FindActorInstanceInMap("bank_treeb_i21", 0, &map);

	bmVectorCopy(tutOneInfo.Trees[3].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Trees[3].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Trees[3].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Trees[3].Flags= Actor->flags;
	strcpy(tutOneInfo.Trees[3].Name, "bank_treeb_i21");

	Actor = FindActorInstanceInMap("bank_treeb_i20", 0, &map);

	bmVectorCopy(tutOneInfo.Trees[4].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Trees[4].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Trees[4].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Trees[4].Flags= Actor->flags;
	strcpy(tutOneInfo.Trees[4].Name, "bank_treeb_i20");

	Actor = FindActorInstanceInMap("bank_treeb_i11", 0, &map);

	bmVectorCopy(tutOneInfo.Trees[5].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Trees[5].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Trees[5].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Trees[5].Flags= Actor->flags;
	strcpy(tutOneInfo.Trees[5].Name, "bank_treeb_i11");

	Actor = FindActorInstanceInMap("bank_treeb_i13", 0, &map);

	bmVectorCopy(tutOneInfo.Trees[6].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Trees[6].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Trees[6].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Trees[6].Flags= Actor->flags;
	strcpy(tutOneInfo.Trees[6].Name, "bank_treeb_i13");

	Actor = FindActorInstanceInMap("bank_treeb_i16", 0, &map);

	bmVectorCopy(tutOneInfo.Trees[7].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Trees[7].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Trees[7].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Trees[7].Flags= Actor->flags;
	strcpy(tutOneInfo.Trees[7].Name, "bank_treeb_i16");

	Actor = FindActorInstanceInMap("bank_treeb_i14", 0, &map);

	bmVectorCopy(tutOneInfo.Trees[8].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Trees[8].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Trees[8].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Trees[8].Flags= Actor->flags;
	strcpy(tutOneInfo.Trees[8].Name, "bank_treeb_i14");

	Actor = FindActorInstanceInMap("crate banana2", 0, &map);

	bmVectorCopy(tutOneInfo.Crate[0].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Crate[0].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Crate[0].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Crate[0].Flags= Actor->flags;
	strcpy(tutOneInfo.Crate[0].Name, "crate banana2");

	Actor = FindActorInstanceInMap("crate banana04", 0, &map);

	bmVectorCopy(tutOneInfo.Crate[1].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Crate[1].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Crate[1].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Crate[1].Flags= Actor->flags;
	strcpy(tutOneInfo.Crate[1].Name, "crate banana04");

	Actor = FindActorInstanceInMap("crate banana05", 0, &map);

	bmVectorCopy(tutOneInfo.Crates[0].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Crates[0].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Crates[0].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Crates[0].Flags= Actor->flags;
	strcpy(tutOneInfo.Crates[0].Name, "crate banana05");

	Actor = FindActorInstanceInMap("crate banana06", 0, &map);

	bmVectorCopy(tutOneInfo.Crates[1].Pos, Actor->actorInstance.position);
	bmVectorCopy(tutOneInfo.Crates[1].Scale, Actor->actorInstance.scale);
	bmQuatCopy(tutOneInfo.Crates[1].Quat, Actor->actorInstance.orientation);
	tutOneInfo.Crates[1].Flags= Actor->flags;
	strcpy(tutOneInfo.Crates[1].Name, "crate banana06");
}

void GetTutThreeInfo()
{
	ACTORINSTANCE	*Actor;

	Actor = FindActorInstanceInMap("secbox", 0, &map);

	bmVectorCopy(tutThrInfo.SBPos, Actor->actorInstance.position);
	bmVectorCopy(tutThrInfo.SBQuat, Actor->actorInstance.orientation);
	
	Actor = FindActorInstanceInMap("tazwantedposter", 0, &map);

	bmVectorCopy(tutThrInfo.WPPos, Actor->actorInstance.position);
	bmVectorCopy(tutThrInfo.WPQuat, Actor->actorInstance.orientation);

	tutThrInfo.WPFlags = Actor->flags;
}

void ResetTutorialOne()
{
	ACTORINSTANCE		*Actor, *tempInstance;

	int					i;

	TBVector			scale;

	MAPOBJECTATTRIBS	instanceAttribs;

	DESTRUCTIBLEOBJECTINFO	*destInfo;

	SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);

	Actor = FindActorInstanceInMap("maingatenull", 0, &map);

	if (!Actor)
	{
		tempInstance = CreateActorInstance(&map.animatedInstances, "objects\\bank - maingatenull.obe", "maingatenull",
										   tutOneInfo.Gate.Pos, scale, tutOneInfo.Gate.Quat, tutOneInfo.Gate.Flags);

		AddActorToCollisionCache(map.collisionCache, tempInstance, 1);

		CreateDestructibleObjectInstance(tempInstance, 5, NULL, 3, 
										 tutOneInfo.Idle->crc, tutOneInfo.First->crc, tutOneInfo.Second->crc, tutOneInfo.Destroy->crc,
										 0,0,0,0,0,0,0,0,1500);
	}

	for (i = 0; i < 9; i++)
	{
		Actor = FindActorInstanceInMap(tutOneInfo.Trees[i].Name, 0, &map);
		
		if (!Actor)
		{
			tempInstance = CreateActorInstance(&map.animatedInstances, "objects\\bank_treeb_i.obe", tutOneInfo.Trees[i].Name,
				tutOneInfo.Trees[i].Pos, tutOneInfo.Trees[i].Scale, tutOneInfo.Trees[i].Quat, tutOneInfo.Trees[i].Flags);
			
			AddActorToCollisionCache(map.collisionCache, tempInstance, 1);
			
			CreateDestructibleObjectInstance(tempInstance, 1, NULL, 1, 
				0, 0, 0, 0, 0,0,0,0,1,0,0,0,2500);
			
			tempInstance->destructibleObjectInfo->debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));
			
			tempInstance->destructibleObjectInfo->debris->debrisCRC[0] = bkCRC32((uchar*) "textures\\leafgreen.bmp", strlen("textures\\leafgreen.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[1] = bkCRC32((uchar*) "textures\\leaftree.bmp", strlen("textures\\leaftree.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[2] = bkCRC32((uchar*) "textures\\leafred.bmp", strlen("textures\\leafred.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[3] = bkCRC32((uchar*) "textures\\leafgreen.bmp", strlen("textures\\leafgreen.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[4] = bkCRC32((uchar*) "textures\\leaftree.bmp", strlen("textures\\leaftree.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[5] = bkCRC32((uchar*) "textures\\leafred.bmp", strlen("textures\\leafred.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[6] = bkCRC32((uchar*) "textures\\leafgreen.bmp", strlen("textures\\leafgreen.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->numDebris = 7;
		}
	}

	for (i = 0; i < 2; i++)
	{
		Actor = FindActorInstanceInMap(tutOneInfo.Crate[i].Name, 0, &map);
		
		if (!Actor)
		{
			tempInstance = CreateActorInstance(&map.animatedInstances, "objects\\bank - crate banana03.obe", tutOneInfo.Crate[i].Name,
				tutOneInfo.Crate[i].Pos, tutOneInfo.Crate[i].Scale, tutOneInfo.Crate[i].Quat, tutOneInfo.Crate[i].Flags);
			
			AddActorToCollisionCache(map.collisionCache, tempInstance, 1);
			
			CreateDestructibleObjectInstance(tempInstance, 1, NULL, 1, 
				0, 0, 0, 0, 0,0,0,0,1,0,0,0,2500);
			
			tempInstance->destructibleObjectInfo->debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));
			
			tempInstance->destructibleObjectInfo->debris->debrisCRC[0] = bkCRC32((uchar*) "textures\\splinter01.bmp", strlen("textures\\splinter01.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[1] = bkCRC32((uchar*) "textures\\splinter02.bmp", strlen("textures\\splinter02.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[2] = bkCRC32((uchar*) "textures\\splinter03.bmp", strlen("textures\\splinter03.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[3] = bkCRC32((uchar*) "textures\\banana.bmp", strlen("textures\\banana.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[4] = bkCRC32((uchar*) "textures\\bananapeel.bmp", strlen("textures\\bananapeel.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->numDebris = 5;
		}
	}

	for (i = 0; i < 2; i++)
	{
		Actor = FindActorInstanceInMap(tutOneInfo.Crates[i].Name, 0, &map);
		
		if (!Actor)
		{
			tempInstance = CreateActorInstance(&map.animatedInstances, "objects\\bank - crate banana05.obe", tutOneInfo.Crates[i].Name,
				tutOneInfo.Crates[i].Pos, tutOneInfo.Crates[i].Scale, tutOneInfo.Crates[i].Quat, tutOneInfo.Crates[i].Flags);
			
			AddActorToCollisionCache(map.collisionCache, tempInstance, 1);
			
			CreateDestructibleObjectInstance(tempInstance, 1, NULL, 1, 
				0, 0, 0, 0, 0,0,0,0,1,0,0,0,2500);
			
			tempInstance->destructibleObjectInfo->debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));
			
			tempInstance->destructibleObjectInfo->debris->debrisCRC[0] = bkCRC32((uchar*) "textures\\splinter01.bmp", strlen("textures\\splinter01.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[1] = bkCRC32((uchar*) "textures\\splinter02.bmp", strlen("textures\\splinter02.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[2] = bkCRC32((uchar*) "textures\\splinter03.bmp", strlen("textures\\splinter03.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[3] = bkCRC32((uchar*) "textures\\banana.bmp", strlen("textures\\banana.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->debrisCRC[4] = bkCRC32((uchar*) "textures\\bananapeel.bmp", strlen("textures\\bananapeel.bmp"), 0);
			tempInstance->destructibleObjectInfo->debris->numDebris = 5;
		}
	}
}

void ResetTutorialThree()
{
	ACTORINSTANCE		*Actor, *tempInstance;

	TBVector			scale;

	MAPOBJECTATTRIBS	instanceAttribs;

	DESTRUCTIBLEOBJECTINFO	*destInfo;

	SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);

	Actor = FindActorInstanceInMap("secbox", 0, &map);

	if (!Actor)
	{
		new SECURITYBOX(tutThrInfo.SBPos,tutThrInfo.SBQuat,NULL,NULL,NULL);
	}

	Actor = FindActorInstanceInMap("tazwantedposter", 0, &map);

	if (!Actor)
	{
		tempInstance = CreateActorInstance(&map.animatedInstances, "objects\\bank - tazwantedposter.obe", "tazwantedposter",
										   tutThrInfo.WPPos, scale, tutThrInfo.WPQuat, tutThrInfo.WPFlags);

		AddActorToCollisionCache(map.collisionCache, tempInstance, 1);

		CreateDestructibleObjectInstance(tempInstance, 1, NULL, 1, 
										 0, 0, 0, 0, 0,0,0,0,1,0,0,0,2500);

		tempInstance->destructibleObjectInfo->debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));

		tempInstance->destructibleObjectInfo->debris->debrisCRC[0] = bkCRC32((uchar*) "objects\\bank - debris pole.obe", strlen("objects\\bank - debris pole.obe"), 0);
		tempInstance->destructibleObjectInfo->debris->debrisCRC[1] = bkCRC32((uchar*) "objects\\bank - debris poletop.obe", strlen("objects\\bank - debris poletop.obe"), 0);
		tempInstance->destructibleObjectInfo->debris->debrisCRC[2] = bkCRC32((uchar*) "objects\\bank - debris fragment1.obe", strlen("objects\\bank - debris fragment1.obe"), 0);
		tempInstance->destructibleObjectInfo->debris->debrisCRC[3] = bkCRC32((uchar*) "objects\\bank - debris fragment2.obe", strlen("objects\\bank - debris fragment2.obe"), 0);
		tempInstance->destructibleObjectInfo->debris->debrisCRC[4] = bkCRC32((uchar*) "objects\\bank - debris fragment3.obe", strlen("objects\\bank - debris fragment3.obe"), 0);
		tempInstance->destructibleObjectInfo->debris->numDebris = 5;
	}
}
