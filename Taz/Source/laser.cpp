// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : laser.cpp
//   Purpose : uses streaks to produces a laser effect
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "scene.h"
#include "sparks.h"
#include "main.h"
#include "sfx.h"
#include "laser.h"
#include "linkedlist.h"
#include "maths.h"


/* --------------------------------------------------------------------------------
   Function : CreateLaser
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateLaser(char state, char *objectName, char *name, char *texture, float size, CPMNode *node, CPMNode *rootNode)
{
	ACTORINSTANCE	*actor;

	actor = CreateActorInstance(&map.invisibleInstances, objectName, name);
	if(!CreateElectricityInfoStructure(actor)) return FALSE;

	actor->special = SPECIALTYPE_LASER;
	actor->electricityInfo->state = state;
	if(!(actor->laserInfo->pathNodes = GetElectricityPathNodesFromMap("pathNode", node)))
		actor->laserInfo->pathNodes = GetElectricityPathNodesFromMap("pathNode", rootNode);
	if(actor->laserInfo->pathNodes)
	{
		actor->laserInfo->texture = bkLoadTexture(NULL,"laserred.tga",0);
		actor->laserInfo->size = size;
		return TRUE;
	}
	
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateLaser
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawLaser(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr;
	ACTORINSTANCE	*node;
	int				counter;
	TBVector		from, to;
	float			size;
	int				white[4] = {127, 127, 127, 255};

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_LASER)
		{
			bdSetIdentityObjectMatrix();
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

			size = ptr->laserInfo->size;

			for(counter = 0; counter < (ptr->laserInfo->pathNodes->numObjects-1); counter++)
			{
				//find node positions
				node = FindActorInstanceInInstanceList(0, ptr->laserInfo->pathNodes->objectNames[counter], &map.invisibleInstances);
				if(node) bmVectorCopy(from, node->actorInstance.position);
				from[3] = 1.0f;
				node = FindActorInstanceInInstanceList(0, ptr->laserInfo->pathNodes->objectNames[counter+1], &map.invisibleInstances);
				if(node) bmVectorCopy(to, node->actorInstance.position);
				to[3] = 1.0f;

				//set alpha
				white[3] = 50 + bmRand()%77;
				bdDrawStreakClipped(from, size, size, white, to, size, size, white, ptr->laserInfo->texture, NULL, 0.0f);
//				PlaySample("obj_mus_01a.wav", 50, from, 0);
			}
			//complete the loop
			node = FindActorInstanceInInstanceList(0, ptr->laserInfo->pathNodes->objectNames[ptr->laserInfo->pathNodes->numObjects-1], &map.invisibleInstances);
			if(node) bmVectorCopy(from, node->actorInstance.position);
			from[3] = 1.0f;
			node = FindActorInstanceInInstanceList(0, ptr->laserInfo->pathNodes->objectNames[0], &map.invisibleInstances);
			if(node) bmVectorCopy(to, node->actorInstance.position);
			to[3] = 1.0f;

			//set alpha
			white[3] = 50 + bmRand()%77;

			bdDrawStreakClipped(from, size, size, white, to, size, size, white, ptr->laserInfo->texture, NULL, 0.0f);

			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		}
	}
}


LINKEDLIST<LASERBEAM>	laserBeams("laser");

/* --------------------------------------------------------------------------------
   Function : CreateLaserBeam
   Purpose : Create a laser beam from start to end
   Parameters : start pos, end pos, life in seconds
   Returns : LASERBEAM*
   Info : 
*/
LASERBEAM *CreateLaserBeam(TBVector start,TBVector end,float life)
{
	LASERBEAM		*laserBeam;

	laserBeam = (LASERBEAM*)ZALLOC(sizeof(LASERBEAM));
	if (!laserBeam) return NULL;

	laserBeams.append(laserBeam);

	bmVectorCopy(laserBeam->start,start);
	laserBeam->start[W] = 1.0f;
	bmVectorCopy(laserBeam->end,end);
	laserBeam->end[W] = 1.0f;
	laserBeam->life = life;

	return laserBeam;
}

/* --------------------------------------------------------------------------------
   Function : UpdateLaserBeams
   Purpose : Updates and deletes laser beams as necessary
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateLaserBeams(void)
{
	LASERBEAM	*laserBeam;
	int			i;

	for (i=laserBeams.getSize()-1;i>=0;i--)
	{
		laserBeam = laserBeams.get(i);
		if (laserBeam->life)
		{
			laserBeam->life -= fTime;
			if (laserBeam->life < 0.0f) 
			{
				laserBeam->life = 0.0f;
				laserBeams.remove(laserBeam);
				FREE(laserBeam);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawLaserBeams
   Purpose : Draws laser beams
   Parameters : 
   Returns : 
   Info : 
*/
void DrawLaserBeams(void)
{
	LASERBEAM	*laserBeam;
	int			white[4] = {127, 127, 127, 255};
	int			whitetrans[4] = {127, 127, 127, 40};
	TBTexture	*texture;
	int			i;

	if (laserBeams.getSize()==0) return;

	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	texture = bkLoadTexture(NULL,"laserred.tga",0);

	for (i=laserBeams.getSize()-1;i>=0;i--)
	{
		laserBeam = laserBeams.get(i);

		bdDrawStreakClipped(laserBeam->start, METERTOUNIT(0.02f), METERTOUNIT(0.02f), white, laserBeam->end, METERTOUNIT(0.02f), METERTOUNIT(0.02f), white, texture, NULL, 0.0f);
		bdDrawStreakClipped(laserBeam->start, METERTOUNIT(0.05f), METERTOUNIT(0.05f), whitetrans, laserBeam->end, METERTOUNIT(0.05f), METERTOUNIT(0.05f), whitetrans, texture, NULL, 0.0f);
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : DeleteLaserBeams
   Purpose : Delete laser beams
   Parameters : 
   Returns : 
   Info : 
*/
void DeleteLaserBeams(void)
{
	laserBeams.kill();
}