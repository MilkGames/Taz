// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : shadow.cpp
//   Purpose : functions to create and draw shadows on the landscape
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "collisioncache.h"
#include "physics.h"
#include "maths.h"
#include "shadow.h"
#include "timer.h"
#include "main.h"
#include "scene.h"
#include "ZooBoss.h"			// PP: Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown
#include "textures.h"			
#include "VideoFX.h"			// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "display.h"			// PP: display stuff
#include "status.h"				// PP: for spin-state check (shadow map turns off for spinning character)
#include "lights.h"				// PP: lights
#include "Spinner.h"			// PP: TEMP
#include "Cheats.h"				// PP: Cheats

// PP: loop-array of all shadow lights in the level
LOOPARRAY<SHADOWLIGHT, MAX_SHADOW_LIGHTS>		shadowLights;


/* --------------------------------------------------------------------------------
   Function : CreateActorShadows
   Purpose : Scans thought an actor list and creates any shadows as defined
   Parameters : actorInstanceList, collisioncache
   Returns : 
   Info : calls	CreateShadow
*/
void CreateActorShadows(ACTORINSTANCELIST *actorInstanceList, COLLISIONCACHE *collisionCache)
{
	ACTORINSTANCE *ptr;
	TBVector	tempVector;
	TBVector	tempVectorAdjusted;		// PP: not too sure what the Y adjustment is for, but I should leave it in

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->actorShadow)
		{
			if (ptr->actorShadow->node)
			{
				baGetNodesWorldPosition(&ptr->actorInstance,ptr->actorShadow->node,tempVector);
			}
			else
			{
				bmVectorCopy(tempVector,ptr->actorInstance.position);
			}

			if(VFX_effectActive(VFX_SHADOWMAP)&&(!(ptr->actorStatus&&(ptr->actorStatus->currentState==STATE_SPIN))))
			{
				CreateTrueShadow(ptr, collisionCache, tempVector);
			}
			else
			{
				// PP: not too sure what the Y adjustment is for, but I should leave it in
				tempVectorAdjusted[X]=tempVector[X];
				tempVectorAdjusted[Y]=tempVector[Y]+METERTOUNIT(0.1f);
				tempVectorAdjusted[Z]=tempVector[Z];
				tempVectorAdjusted[W]=tempVector[W];

#ifndef CONSUMERDEMO
				if(ptr->actorShadow->type == SHADOWTYPE_ZOOBOSS)
				{
					CreateZooBossShadow(ptr->actorShadow, collisionCache, tempVectorAdjusted);
				}
				else// PP: downward
#endif
				{
					CreateShadow(ptr, collisionCache, tempVectorAdjusted);
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : SetShadowNode
   Purpose : Sets the node for the shadow to use
   Parameters : actorInstance, node (if NULL the actors position is used)
   Returns : 
   Info : 
*/
void SetShadowNode(ACTORINSTANCE *actorInstance, TBActorNodeInstance *node, char *nodeName)
{
	if (actorInstance->actorShadow==NULL) return;

	if (node) actorInstance->actorShadow->node = node;
	else
	{
		if (nodeName) actorInstance->actorShadow->node = baFindNode(actorInstance->actorInstance.rootNodeInstance,nodeName);
		else
		{
			actorInstance->actorShadow->node = NULL;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawActorShadows
   Purpose : Scans thought an actor list and draws any shadows listed
   Parameters : actorInstanceList
   Returns : 
   Info : calls	DrawShadow, MUST BE CALLED AFTER ALL LANDSCAPE OBJECTS HAVE BEEN DRAWN!
*/

extern TBActorInstance		ZBshadowZMask;

void DrawActorShadows(ACTORINSTANCELIST *actorInstanceList)
{
	// PP: debug-draw shadow-casting lightsources if appropriate
	if(shadowLightDebug)
	{
		drawShadowLights();
	}

	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	// PP: *** SPECIAL CASE: Z-MASK FOR ZOOBOSS SHADOWS ***
	// PP: (might be temp: poly flags'd be nicer)
	// PP: I wouldn't normally bodge-in a level-check like this, but, well, dammit, y'know!
	// PP: Look if you want to use the same technique on another level, tell me and I'll make the thing a bit more generic, ok?
	// PP: Otherwise, there's no point me wasting time writing nice neat functions that never get used!  OK?!  Sheez.
	// PP: You really are a fussy beggar arent you?!  You make such a fuss ALL the time...
	
#ifndef CONSUMERDEMO
	if(map.sceneNumber == SCENE_ZOOBOSS)
	{
		ApplyDecalZMask(ZOOBOSSGAME::getShadowZMask());
	}
#endif

	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSEQUAL, FALSE);
#ifndef SHADOW_COLLTEST
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, FALSE);
#else
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, FALSE);
#endif
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
//	bdSetRenderState(BDRENDERSTATE_CLIPPING, TRUE, FALSE);

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if((!(ptr->flags&ACTORFLAG_DONTDRAW))&&(ptr->currentAlpha))
		{
			if (ptr->actorShadow)
			{
				// has shadow
				DrawShadow(ptr->actorShadow);
			}
		}
	}
	
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
}

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseShadow
	Purpose 	: intialise a shadow structure
	Parameters 	: ptr to ACTORINSTANCE whose shadow to initialise,
					radius of shadow, max cast range of shadow,
					(opt/default)max radius of object for shadow map rendering,
					(opt/default) type of shadow (SHADOWTYPE_)
	Returns 	: 
	Info 		: // PP: if my changing of the parameters has caused any problems, feel free to slap me
*/
void InitialiseShadow(ACTORINSTANCE *const	actorInstance,
					  float					shadowRadius,
					  float					shadowHeight,
					  const float			shadowMapRadius,
					  const ShadowType		shadowType)
{
	SHADOW		*shadow;								// PP: ptr to the ACTORINSTANCE's shadow
	int			i;										// PP: loop coonturrr

	if(actorInstance == NULL)
	{
		return;											// PP: return if the ACTORINSTANCE is NULL
	}
	else
	{
		shadow=actorInstance->actorShadow;				// PP: otherwise get a ptr to the ACTORINSTANCE's shadow
	}

	shadow->actorInstance = actorInstance;				// PP: keep a ptr to the ACTORINSTANCE casting the shadow
	shadow->numPolys = 0;
	shadow->shadowRadius = shadowRadius;
	shadow->shadowHeight = shadowHeight;
	shadow->flags = SHADOWFLAG_COLLECTPOLYS;

	shadow->type=shadowType;							// PP: set shadow type
	shadow->node = NULL;

	// PP: to start with, don't use shadow map
	shadow->useShadowMap=false;
	shadow->shadowMapRadius=shadowMapRadius;			// PP: store shadow map radius
	shadow->light=NULL;

	// PP: set colour of shadow verts...

	for (i=0;i<MAX_SHADOW_POLYS*3;i++)
	{
		BDVERTEX_SETRGBA(&shadow->vertices[i],0,0,0,(int)80);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateShadow
	Purpose 	: Fill in the shadow structure will all information needed to draw the shadow
	Parameters 	: ptr to ACTORINSTANCE, collisionCache to use, position of object casting shadow
	Returns 	: completed shadow structure
	Info 		: 
*/
void CreateShadow(ACTORINSTANCE* actorInstance, COLLISIONCACHE *collisionCache, TBVector pos)
{
	SHADOW				*shadow=actorInstance->actorShadow;			// PP: get a ptr to the actor instance's shadow
	BOUNDINGBOX			shadowBox;
	int					numVertLists,i;
	int					currVList;
	COLLISIONLISTENTRY	*meshLists[MAX_COLL_MESHES];
	int					currVertIdx;
	COLLPOLY			*tempPoly;
	COLLVERTEX			*tempVertex;
	float				testPoint1[4];
	float				currY;
	COLLISIONLISTENTRY	*ptr;
	TBVector			shadowVerts[4];
	float				shadowHeight = shadow->shadowHeight;
	float				shadowRadius;
	float				xmin,xmax,ymin,ymax,zmin,zmax;
	TBVector			tempVector, v1, norm;


	// TP: do not recalc shadow if the drawonly flag is set
	if (shadow->flags&SHADOWFLAG_DRAWONLY) return;

	shadowRadius=shadow->shadowRadius;
	
	// PP: TEMP - set shadow colour
	shadow->colour=COLOUR(60,60,60,64);// PP: TEMP? whatever I'm too tired to care

	if (shadow->flags&SHADOWFLAG_COLLECTPOLYS)
	{

		shadowBox.xmax = pos[X] + shadowRadius;
		shadowBox.xmin = pos[X] - shadowRadius;
		shadowBox.ymax = pos[Y];
		shadowBox.ymin = pos[Y] - shadowHeight;
		shadowBox.zmax = pos[Z] + shadowRadius;
		shadowBox.zmin = pos[Z] - shadowRadius;

		shadowVerts[0][X] = shadowBox.xmin;
		shadowVerts[0][Z] = shadowBox.zmin;
		shadowVerts[1][X] = shadowBox.xmax;
		shadowVerts[1][Z] = shadowBox.zmin;
		shadowVerts[2][X] = shadowBox.xmax;
		shadowVerts[2][Z] = shadowBox.zmax;
		shadowVerts[3][X] = shadowBox.xmin;
		shadowVerts[3][Z] = shadowBox.zmax;

		// update the collision cache using box test XZ and eliminate Y > pos[Y]
		numVertLists = 0;
		meshLists[numVertLists] = NULL;
		for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;ptr = ptr->next)
		{
			if ((!ptr->actorInstance)||((ptr->actorInstance->instanceList!=&map.invisibleInstances)&&(ptr->actorInstance->instanceList!=&map.triggerInstances)))
			{
				// TP: Don't cast shadow on one's self
				if (ptr->actorInstance==actorInstance)
				{
					continue;
				}
				// test if in or going to enter bounding box
				if ((numVertLists<MAX_COLL_MESHES)&&TestBoxCollision(&shadowBox,&ptr->boundingBox))
				{
					if (MakeEntryResident(collisionCache,ptr)==TRUE)
					{
						// resident so add to collision process
						meshLists[numVertLists] = ptr;
						numVertLists++;
					}
				}
			}
		}

		currY = pos[Y] - shadowHeight;
		currVertIdx=0;

#ifdef USE_SHORTS_FOR_COLLISION
		short		posMin[3],posMax[3];
			
		posMin[X]=(((short)shadowBox.xmin)>>3)-1;
		posMin[Y]=(((short)shadowBox.ymin)>>3)-1;
		posMin[Z]=(((short)shadowBox.zmin)>>3)-1;

		posMax[X]=(((short)shadowBox.xmax)>>3)+1;
		posMax[Y]=(((short)shadowBox.ymax)>>3)+1;
		posMax[Z]=(((short)shadowBox.zmax)>>3)+1;
#endif
		// eliminate polys using box test
		for	(currVList = 0;currVList < numVertLists; currVList++)
		{
			// point to start of list
			tempPoly = meshLists[currVList]->polys;
			for (i=0;i<meshLists[currVList]->numPolys;i++)
			{
				if (currVertIdx >= (MAX_SHADOW_POLYS*3)) break;

				// check poly faces up
				// get normal from first vertex in poly
	#ifndef SHADOW_COLLTEST
 				if (tempPoly->ny <= (0.1f))
				{
					tempPoly++;
					continue;
				}
	#endif
				// if all points on poly are on one side of test box then dismiss
#ifdef USE_SHORTS_FOR_COLLISION
				
				// IH: Use the smaller bounding box data
	#ifndef SHADOW_COLLTEST
				if ((tempPoly->minx > posMax[X])||(tempPoly->minz > posMax[Z])||(tempPoly->miny > posMax[Y]))
				{
					tempPoly++;
					continue;
				}
	#endif
	#ifndef SHADOW_COLLTEST
				if ((tempPoly->maxx < posMin[X])||(tempPoly->maxz < posMin[Z])||(tempPoly->maxy < posMin[Y]))
				{
					tempPoly++;
					continue;
				}
	#endif

#else
			// IH: Use the normal bounding box data
	#ifndef SHADOW_COLLTEST
				if ((tempPoly->minx > shadowBox.xmax)||(tempPoly->minz > shadowBox.zmax)||(tempPoly->miny > shadowBox.ymax))
				{
					tempPoly++;
					continue;
				}
	#endif
	#ifndef SHADOW_COLLTEST
				if ((tempPoly->maxx < shadowBox.xmin)||(tempPoly->maxz < shadowBox.zmin)||(tempPoly->maxy < shadowBox.ymin))
				{
					tempPoly++;
					continue;
				}
	#endif

				// TP: check poly is the right side of position

				// TP: get normal from pos to first vert on poly
				bmVectorSet(tempVector, 0.0f, 0.0f, 0.0f, 1.0f);
				COLLPOLY_GETV1(tempPoly[0], meshLists[currVList]->vertices, v1[X], v1[Y], v1[Z], v1[W]);
				bmVectorSet(norm, tempPoly->nx, tempPoly->ny, tempPoly->nz, 1.0f);

				bmVectorSub(tempVector, pos, v1);
				bmVectorNorm(tempVector, tempVector);

				if (bmVectorDot(tempVector, norm) < 0.0f)
				{
					tempPoly++;
					continue;
				}


#endif	// IH: End USE_SHORTS_FOR_COLLISION
				tempVertex = &meshLists[currVList]->vertices[tempPoly->i1];
				if ((tempVertex->y > currY)) currY = tempVertex->y;
				BDVERTEX_SETXYZW(&shadow->vertices[currVertIdx],tempVertex->x,tempVertex->y/*+5.0f*/,tempVertex->z,1.0f);// PP: REMOUT: using z bias at draw stage now
				currVertIdx++;
				
				tempVertex = &meshLists[currVList]->vertices[tempPoly->i2];
				if ((tempVertex->y > currY)) currY = tempVertex->y;
				BDVERTEX_SETXYZW(&shadow->vertices[currVertIdx],tempVertex->x,tempVertex->y/*+5.0f*/,tempVertex->z,1.0f);// PP: REMOUT: using z bias at draw stage now
				currVertIdx++;

				tempVertex = &meshLists[currVList]->vertices[tempPoly->i3];
				if ((tempVertex->y > currY)) currY = tempVertex->y;
				BDVERTEX_SETXYZW(&shadow->vertices[currVertIdx],tempVertex->x,tempVertex->y/*+5.0f*/,tempVertex->z,1.0f);// PP: REMOUT: using z bias at draw stage now
				currVertIdx++;

				shadow->groundY = currY;
				tempPoly++;
			}
		}
		shadow->numPolys = currVertIdx/3;
	}
	else
	{
		// TP: get the ground y from previous tests
		currY = shadow->groundY;
	}

	if (currY > pos[Y]) currY = pos[Y];

	// TP: Select Radius
	// PP: shadowRadius = shadow->shadowRadius - ((pos[Y] - currY)*(shadowRadius/shadowHeight));
	if (shadowHeight)
	{
		shadowRadius -= ((pos[Y] - currY)*(shadowRadius/shadowHeight));
	}
	else
	{
		shadowRadius = 1;
	}

	if (shadowRadius < 1) shadowRadius = 1;
	// TP: darkness = 1.0f - ((pos[Y] - currY)/shadowHeight);

	// TP: Calculate uv's
	for (i=0;i<shadow->numPolys*3;i++)
	{
		BDVERTEX_SETCOLOUR(&shadow->vertices[i], shadow->colour);		// PP: changed this to work with subtractive blend
		BDVERTEX_GETXYZW(&shadow->vertices[i],testPoint1[X],testPoint1[Y],testPoint1[Z],testPoint1[W]);
	//	BDVERTEX_SETUV(&shadow->vertices[i],0.5f+((pos[X]-testPoint1[X])/shadowRadius),0.5f+((testPoint1[Z]-pos[Z])/shadowRadius));

		BDVERTEX_SETUV(&shadow->vertices[i],0.5f+((pos[X]-testPoint1[X])/shadowRadius),0.5f+((testPoint1[Z]-pos[Z])/shadowRadius));
	}

	// PP: this type of shadow doesn't need a shadow map to be generated
	shadow->useShadowMap=false;
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateTrueShadow
	Purpose 	: Fill in the shadow structure will all information needed to draw a 'True' shadow
	Parameters 	: ptr to ACTORINSTANCE, collisionCache to use, position of object casting shadow
	Returns 	: completed shadow structure
	Info 		: 
*/

#define TEMP_CASTING_RANGE		METERTOUNIT(3)// PP: should be lower

void CreateTrueShadow(ACTORINSTANCE* actorInstance, COLLISIONCACHE *collisionCache, TBVector pos)
{
	SHADOW				*shadow=actorInstance->actorShadow;			// PP: get a ptr to the actor instance's shadow

	BOUNDINGBOX			shadowBox;
	int					numVertLists,i;
	int					currVList;
	COLLISIONLISTENTRY	*meshLists[MAX_COLL_MESHES];
	uint32				currVertIdx;
	COLLPOLY			*tempPoly;
	COLLVERTEX			*tempVertex;
	float				currY;
	COLLISIONLISTENTRY	*ptr;
	TBVector			shadowVerts[4];
	float				shadowHeight = TEMP_CASTING_RANGE;
	float				xmin,xmax,ymin,ymax,zmin,zmax,darkness;
	ShadowLightFlags	flags;	

	// PP: TEMP DEBUG
//	extern float testShadowScale;

	// PP: Shadow map stuff is going to change anyway so sod it.  I'll just leave it looking as nice as possible till I get a chance to work on it - WHICH I WILL DO, OH YES.  There's some very pretty games out there you know...
//	float shadowMapUVAdjust=2.094313f;

	// PP: TEMP: get a ptr to the test lightsource which is going to be casting this shadow
	shadow->light=getShadowLightForCharacter(actorInstance);

	// PP: Where there is no light, there is no shadow.™
	if(shadow->light == NULL) return;

	SHADOWLIGHT			*light=shadow->light;// PP: for conwenience

	// PP: select appropriate set of flags from the light
	flags=light->getShadowFlags(actorInstance);

	// PP: HMMM: set light vector to match shadow
	UpdateCartoonLight(-light->dir, light->col);

//	shadowRadius=shadow->shadowMapRadius*0.5f;// PP: *0.5f: oops!  Shadow map stuff is going to change anyway so sod it.

	// PP: get the effective source of the light ray
	if(light->type == SHADOWLIGHTTYPE_DIRECTIONAL)
	{
		shadow->lightPos=pos-(light->dir*SHADOWLIGHT_INFINITE_DISTANCE);
	}
	else
	{
		shadow->lightPos=light->pos-(light->dir*SHADOWLIGHT_INFINITE_DISTANCE);
	}

	// PP: set shadow colour from shadow light colour
	shadow->colour=light->col*light->shadowDarkness;

	// PP: SET UP A BOUNDING BOX FOR THE SHADOW

	float	realCastRange=light->castRange*shadow->shadowMapRadius;

	shadowBox.xmax = pos[X] + realCastRange;
	shadowBox.xmin = pos[X] - realCastRange;
	shadowBox.ymax = pos[Y]	+ realCastRange;
	shadowBox.ymin = pos[Y] - realCastRange;
	shadowBox.zmax = pos[Z] + realCastRange;
	shadowBox.zmin = pos[Z] - realCastRange;

	shadowVerts[0][X] = shadowBox.xmin;
	shadowVerts[0][Z] = shadowBox.zmin;
	shadowVerts[1][X] = shadowBox.xmax;
	shadowVerts[1][Z] = shadowBox.zmin;
	shadowVerts[2][X] = shadowBox.xmax;
	shadowVerts[2][Z] = shadowBox.zmax;
	shadowVerts[3][X] = shadowBox.xmin;
	shadowVerts[3][Z] = shadowBox.zmax;

	// update the collision cache using box test XZ and eliminate Y > pos[Y]
	numVertLists = 0;
	meshLists[numVertLists] = NULL;

	for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;ptr = ptr->next)
	{
		// TP: Don't cast shadow on one's self
		if (ptr->actorInstance==actorInstance)
		{
			continue;
		}

		if(~ptr->flags & CACHEFLAGS_SUB)
		{
			// PP: IF THE LIGHT IS SET TO CAST SHADOWS ON THE LANDSCAPE ONLY, THEN THAT IS WHAT IT SHALL DO		// PP: what a silly comment		// PP: and that one		// PP: ooh, and that one too	// PP: and this one actually	// PP: oh, they're all rather silly		// PP: teehee!
			if(SHADOWLIGHT::flagSet(flags, SHADOWLIGHTFLAG_LANDSCAPE_ONLY))
			{
				continue;
			}

			// PP: don't cast onto invisible objects
			if((ptr->actorInstance->instanceList!=&map.invisibleInstances)&&(ptr->actorInstance->instanceList!=&map.triggerInstances))
			{
				continue;
			}
		}

		// test if in or going to enter bounding box
		if (TestBoxCollision(&shadowBox,&ptr->boundingBox)&&(numVertLists<MAX_COLL_MESHES))
		{
			if (MakeEntryResident(collisionCache,ptr)==TRUE)
			{
				// resident so add to collision process
				meshLists[numVertLists] = ptr;
				numVertLists++;
			}
		}
	}

	// PP: precalc some bits & bobs for the vector rotations...

	float cosvy=bmCosApprox(-shadow->lightAngle[Y]);
	float sinvy=bmSinApprox(-shadow->lightAngle[Y]);
	float cosvx=bmCosApprox(-shadow->lightAngle[X]);
	float sinvx=bmSinApprox(-shadow->lightAngle[X]);

	register float	x,y,z,t;	// PP: REGISTER: I thought it might be a good idea - is it a good idea?  That sort of thing is all Greek to me

	currY = pos[Y] - shadowHeight;
	currVertIdx=0;

	VEC		polyNormal;
	VEC		vertPos;
	
	float	dot;

#ifdef USE_SHORTS_FOR_COLLISION
	short		posMin[3],posMax[3];
	
	posMin[X]=(((short)shadowBox.xmin)>>3)-1;
	posMin[Y]=(((short)shadowBox.ymin)>>3)-1;
	posMin[Z]=(((short)shadowBox.zmin)>>3)-1;

	posMax[X]=(((short)shadowBox.xmax)>>3)+1;
	posMax[Y]=(((short)shadowBox.ymax)>>3)+1;
	posMax[Z]=(((short)shadowBox.zmax)>>3)+1;
#endif

	// PP: maximum number of verts that can bear the shadow
	uint32	maxShadowVerts=MAX_SHADOW_POLYS*3;

	// eliminate polys using box test
	for	(currVList = 0;currVList < numVertLists; currVList++)
	{
		// point to start of list
		tempPoly = meshLists[currVList]->polys;
		
		for (i=meshLists[currVList]->numPolys-1;i>=0;i--)
		{
			if (currVertIdx >= maxShadowVerts)
			{
				break;
			}

#ifdef USE_SHORTS_FOR_COLLISION
			// if all points on poly are on one side of test box then dismiss
			if ((tempPoly->minx > posMax[X])||(tempPoly->minz > posMax[Z])||(tempPoly->miny > posMax[Y]))
			{
				tempPoly++;
				continue;
			}

			if ((tempPoly->maxx < posMin[X])||(tempPoly->maxz < posMin[Z])||(tempPoly->maxy < posMin[Y]))
			{
				tempPoly++;
				continue;
			}
#else
			// if all points on poly are on one side of test box then dismiss
			if ((tempPoly->minx > shadowBox.xmax)||(tempPoly->minz > shadowBox.zmax)||(tempPoly->miny > shadowBox.ymax))
			{
				tempPoly++;
				continue;
			}

			if ((tempPoly->maxx < shadowBox.xmin)||(tempPoly->maxz < shadowBox.zmin)||(tempPoly->maxy < shadowBox.ymin))
			{
				tempPoly++;
				continue;
			}
#endif

			// PP: REJECT THE POLY IF ITS PLANE IS INTERSECTED BY THE LIGHT RAY...

			// PP: get polygon normal
			COLLPOLY_GETNORMAL(tempPoly[0], polyNormal[X], polyNormal[Y], polyNormal[Z]);

			// PP: get vertex position
			COLLPOLY_GETV1(tempPoly[0], meshLists[currVList]->vertices, vertPos[X], vertPos[Y], vertPos[Z], vertPos[W]);

			if(SHADOWLIGHT::flagSet(flags, SHADOWLIGHTFLAG_BOGUS_TEST))
			{
				// PP: perform the ray-plane intersection test, priced at just 3 dot products excluding p&p.  Order now and you'll recieve this stylish fountain pen set worth £12.99 - ABSOLUTELY FREE!!
				if(intersectRayPlane(shadow->lightPos, pos, vertPos, polyNormal))
				{
					tempPoly++;
					continue;
				}
			}

			// PP: REJECT THE POLY IF IT DOESN'T FACE THE LIGHT...

			if(SHADOWLIGHT::flagSet(flags, SHADOWLIGHTFLAG_BACKFACE_CULL)||SHADOWLIGHT::flagSet(flags, SHADOWLIGHTFLAG_ANGULAR_FALLOFF))
			{
				polyNormal.normalise2();

				dot=dotProduct(polyNormal, light->dir);// PP: light dir is already normalised (in VFX_generateShadowMap) because of the light flags

				if(dot >= 0.0f)
				{
					tempPoly++;
					continue;
				}
				
				// PP: EFFECT ANGULAR FALLOFF...

				if(SHADOWLIGHT::flagSet(flags, SHADOWLIGHTFLAG_ANGULAR_FALLOFF))
				{
					darkness= -dot;
				}
				else
				{
					darkness=1.0f;
				}
			}
			else
			{
				darkness=1.0f;
			}

			// PP: cool, the poly passes all our checks and now gets copied to the shadow structure...

			float	vertDarkness;

			for(int v=0; v<3; v++)
			{
				vertDarkness=darkness;

				// PP: get vertex position
				switch(v)
				{
				case 0:
					COLLPOLY_GETV1(tempPoly[0], meshLists[currVList]->vertices, vertPos[X], vertPos[Y], vertPos[Z], vertPos[W]);
					break;
				case 1:
					COLLPOLY_GETV2(tempPoly[0], meshLists[currVList]->vertices, vertPos[X], vertPos[Y], vertPos[Z], vertPos[W]);
					break;
				case 2:
					COLLPOLY_GETV3(tempPoly[0], meshLists[currVList]->vertices, vertPos[X], vertPos[Y], vertPos[Z], vertPos[W]);
					break;

					NODEFAULT;
				}
				//COLLVERTEX_GETXYZW(tempVertex, vertPos[X], vertPos[Y], vertPos[Z], vertPos[W]);
				BDVERTEX_SETVEC(&shadow->vertices[currVertIdx],vertPos);

				// PP: rotate the offset from the occluder to the vert by the inverse of the light frustum orientation...

				TBVector	distVec;

				bmVectorSub(distVec, vertPos.v, pos);

				x=distVec[X];
				y=distVec[Y];
				z=distVec[Z];

				// PP: rotate on Y
				t=cosvy*x+sinvy*z;
				z=cosvy*z-sinvy*x;
				x=t;

				// PP: rotate on X
				t=cosvx*z+sinvx*y;
				y=cosvx*y-sinvx*z;
				z=t;

				// PP: then just do the same old thang pretty much...

				float	distLightToVert;

				if(light->type == SHADOWLIGHTTYPE_DIRECTIONAL)
				{
					BDVERTEX_SETU(&shadow->vertices[currVertIdx], 0.5f+(x/shadow->shadowMapRadius));
					BDVERTEX_SETV(&shadow->vertices[currVertIdx], 0.5f-(y/shadow->shadowMapRadius));

					// PP: EFFECT DISTANCE FALLOFF
					if(light->range != SHADOWLIGHT_INFINITE_RANGE)
					{
						// PP: use depth axis for distance falloff - not quite so precise in theory
						distLightToVert=shadow->shadowMapDist-z;

						vertDarkness*= 1.0f-(distLightToVert/light->range);
					}
				}
				else
				{
					float	widthFactor;
					
					if(SHADOWLIGHT::flagSet(flags, SHADOWLIGHTFLAG_PARALLEL_POINT))
					{
						// PP: no ray divergence
						widthFactor=1.0f;

						// PP: EFFECT DISTANCE FALLOFF using depth axis - not quite so precise in theory
						distLightToVert=shadow->shadowMapDist-z;
					}
					else
					{
						// PP: EFFECT RAY DIVERGENCE - it's a point light remember?

						distLightToVert=(vertPos-shadow->light->pos).magnitude();

						widthFactor=distLightToVert / shadow->shadowMapDist;
					}

					// PP: EFFECT DISTANCE FALLOFF while we're at it - that costly magnitude can be used here - we might as well get our money's worth
					if(light->range != SHADOWLIGHT_INFINITE_RANGE)
					{
						vertDarkness*= 1.0f-(distLightToVert/light->range);
					}

					BDVERTEX_SETU(&shadow->vertices[currVertIdx], 0.5f+((x/widthFactor)/shadow->shadowMapRadius));
					BDVERTEX_SETV(&shadow->vertices[currVertIdx], 0.5f-((y/widthFactor)/shadow->shadowMapRadius));
				}

				vertDarkness=CONSTRAIN(vertDarkness, 0.0f, 1.0f);

				BDVERTEX_SETCOLOUR(&shadow->vertices[currVertIdx], shadow->colour*vertDarkness);

				currVertIdx++;
			}
			tempPoly++;
		}
	}

	// PP: currY 
	if (currY > pos[Y]) currY = pos[Y];

	shadow->numPolys = currVertIdx/3;

	// PP: this type of shadow needs a shadow map to be generated
	shadow->useShadowMap=true;
}

#ifndef CONSUMERDEMO

/*	--------------------------------------------------------------------------------
	Function 	: CreateZooBossShadow
	Purpose 	: Fill in the shadow structure with all information needed to draw the sunset shadow
	Parameters 	: shadow to fill in, collisionCache to use, position of object casting shadow
	Returns 	: 
	Info 		: // PP: added for the Zoo boss game // PP: this will be improved and neatened
*/
void CreateZooBossShadow(SHADOW *shadow, COLLISIONCACHE *collisionCache, TBVector pos)
{
	BOUNDINGBOX			shadowBox;
	int					numVertLists,i;
	int					currVList;
	COLLISIONLISTENTRY	*meshLists[MAX_COLL_MESHES];
	int					numberOfVerts[MAX_COLL_MESHES];
	int					currVertIdx;
	COLLPOLY			*tempPoly;
	COLLVERTEX			*tempVertex;
	float				testPoint1[4];
	COLLISIONLISTENTRY	*ptr;
	float				shadowLengthBeyondWall = shadow->shadowHeight;
	float				shadowRadius = shadow->shadowRadius;
	float				xmin,xmax,ymin,ymax,zmin,zmax,darkness, maxDarkness;


	shadow->colour=COLOUR(95, 95, 95, 85);// PP: set shadow colour

	shadowBox.xmax = pos[X] + shadowRadius;
	shadowBox.xmin = pos[X] - shadowRadius;
	shadowBox.ymax = pos[Y] + shadowRadius;
	shadowBox.ymin = pos[Y] - shadowRadius;
	shadowBox.zmax = ZB_COURT_TOP + shadowLengthBeyondWall;
	shadowBox.zmin = pos[Z] - shadowRadius;


	// update the collision cache using box test XY and eliminate Z > pos[Z]
	numVertLists = 0;
	meshLists[numVertLists] = NULL;

	for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;ptr = ptr->next)
	{
		if ((!ptr->actorInstance)||((ptr->actorInstance->instanceList!=&map.invisibleInstances)&&(ptr->actorInstance->instanceList!=&map.triggerInstances)))
		{
			// test if in or going to enter bounding box
			if (TestBoxCollision(&shadowBox,&ptr->boundingBox)&&(numVertLists<MAX_COLL_MESHES))
			{
				if (MakeEntryResident(collisionCache,ptr)==TRUE)
				{
					// resident so add to collision process
					meshLists[numVertLists] = ptr;
					numVertLists++;
				}
			}
		}
	}


	currVertIdx=0;
	// eliminate polys using box test
	for	(currVList = 0;currVList < numVertLists; currVList++)
	{
		// point to start of list
		tempPoly = meshLists[currVList]->polys;
		for (i=numberOfVerts[currVList];i>=0;i-=3)
		{
			if (currVertIdx >= (MAX_SHADOW_POLYS*3)) break;// TP: Must fix

			// if all points on poly are on one side of test box then dismiss
#ifndef SHADOW_COLLTEST
			if ((tempPoly->minx > shadowBox.xmax)||(tempPoly->minz > shadowBox.zmax)||(tempPoly->miny > shadowBox.ymax))
			{
				tempPoly++;
				continue;
			}
#endif
#ifndef SHADOW_COLLTEST
			if ((tempPoly->maxx < shadowBox.xmin)||(tempPoly->maxz < shadowBox.zmin)||(tempPoly->maxy < shadowBox.ymin))
			{
				tempPoly++;
				continue;
			}
#endif

			// check poly faces towards the light
			// get normal from first vertex in poly

			// PP: REMOUT TEMP TEST

			/*
#ifndef SHADOW_COLLTEST
			if (tempVertex->nz >= (-0.1f))// PP: ROUGH, TEMP?
			{
				tempVertex+=3;
				continue;
			}
#endif
*/
			tempVertex = &meshLists[currVList]->vertices[tempPoly->i1];
			BDVERTEX_SETXYZW(&shadow->vertices[currVertIdx],tempVertex->x,tempVertex->y/*+5.0f*/,tempVertex->z/*-5.0f*/,1.0f);
			currVertIdx++;

			tempVertex = &meshLists[currVList]->vertices[tempPoly->i2];
			BDVERTEX_SETXYZW(&shadow->vertices[currVertIdx],tempVertex->x,tempVertex->y/*+5.0f*/,tempVertex->z/*-5.0f*/,1.0f);
			currVertIdx++;

			tempVertex = &meshLists[currVList]->vertices[tempPoly->i3];
			BDVERTEX_SETXYZW(&shadow->vertices[currVertIdx],tempVertex->x,tempVertex->y/*+5.0f*/,tempVertex->z/*-5.0f*/,1.0f);
			currVertIdx++;
			tempPoly++;
		}
	}

	if(ZB_COURT_TOP-pos[Z] != 0.0f)
	{
		maxDarkness= (ZB_COURT_TOP-pos[Z])/(float)(ZB_COURT_WIDTH*0.65f);
		maxDarkness=MIN(maxDarkness, 1.0f);
		maxDarkness=1.0f-maxDarkness;
	}

	// TP: Calculate uv's
	for (i=0;i<currVertIdx;i++)
	{
		BDVERTEX_GETXYZW(&shadow->vertices[i],testPoint1[X],testPoint1[Y],testPoint1[Z],testPoint1[W]);

		if(ZB_COURT_TOP-pos[Z] != 0.0f)
		{
			if(testPoint1[Z]>ZB_COURT_TOP)
			{
				darkness=(testPoint1[Z]-ZB_COURT_TOP)/shadowLengthBeyondWall;
				darkness=MIN(darkness, 1.0f);
				darkness=1.0f-darkness;
			}
			else
			{
				darkness=MAX((testPoint1[Z]-pos[Z]), 0.0f)/(ZB_COURT_TOP-pos[Z]);
			}
		}

		darkness*=maxDarkness;

		BDVERTEX_SETCOLOUR(&shadow->vertices[i], shadow->colour*darkness);
		BDVERTEX_SETUV(&shadow->vertices[i],0.5f+((pos[X]-testPoint1[X])/shadowRadius),0.5f+((testPoint1[Y]-pos[Y])/shadowRadius));
	}

	shadow->numPolys = currVertIdx/3;

	// PP: this type of shadow doesn't need a shadow map to be generated
	shadow->useShadowMap=false;
}

#endif


/*	--------------------------------------------------------------------------------
	Function 	: DrawShadow
	Purpose 	: Takes a shadow structure and renders the polys
	Parameters 	: Shadow structure
	Returns 	: 
	Info 		: 
*/
// CMD: now in textures,h #define SHADOW_TEXTURE			"shadowsub.bmp"// PP: was "shad01.tga"		// PP: texture to use for shadows

void DrawShadow(SHADOW *shadow)
{
	if (shadow->numPolys==0) return;

	// PP: if the shadow is of shadow-map type, set the shadow map as the texture for the shadow...

	// PP: use spot shadows for spinning characters - makes sense no?
	if(shadow->useShadowMap)
	{
		// PP: TEMP?: no proper shadows in stereoscopic mode.
		// PP: I suspect there might still be a (very occasional) glitch with them in stereo - no idea what it is.
		// PP: Without having enough time to test it thoroughly (which I DON'T!) I'd prefer not to risk it.
		if(videoMode.numDraws>1)
		{
			return;
		}

		// PP: generate the shadow
		VFX_generateShadowMap(shadow->actorInstance);

		// PP: IMPORTANT!!!!!! MUST SET TEXTURE ADDRESS MODE BEFORE SETTING TEXTURE!!!!!!
		// PP: ...because on Xbox, non-pow2 render targets can't be wrapped (it'd cause a crash)
		bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, FALSE);

		// PP: set our render target as the current texture on stage zero
		bdSetRenderTargetAsTexture(VFX_shadowRenderTarget, 0);

		// PP: Set blend mode specified by the shadow's lightsource
		switch(shadow->light->blend)
		{
		case SHADOWBLEND_SUBTRACTIVE:
			{
				SETBLEND(SUBTRACTIVE);
			}
			break;

		case SHADOWBLEND_BLEND:
			{
				SETBLEND(BLEND);

				// PP: TODO?: on platforms that can do it, set [(destCol*invSrcCol)+(srcCol*srcCol)] blending.
				// PP: This eliminates the need for a 32bit render target for shadows.
			}
			break;

			NODEFAULT;
		}
	}
	else
	{
		bdSetTexture(0,textureShadow);

		// PP: Set subtractive mode
		SETBLEND(SUBTRACTIVE);
	}

#ifndef CONSUMERDEMO
	if(map.sceneNumber == SCENE_ZOOBOSS)
	{
		// PP: temp? (milestone)
		SETFOG(FALSE);
	}
#endif

	// PP: don't write to the z buffer, else your shadow disappears when you get close to someone else's shadow
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);

#if(BPLATFORM == PS2)
	bdSetZBias(-10000.0f);// PP: this will need tweaking - I have no idea what sort of value to use
#else// PP: if (BPLATFORM == PS2) else
	SETZBIAS(-0.3f);
#endif// PP: platform

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSEQUAL, FALSE);

	// PP: TEMP TEST	!!!
	bdSetRenderState(BDRENDERSTATE_BACKFACECULL, BDCULLMODE_OFF, 0);
	bdSetRenderState(BDRENDERSTATE_BACKFACECULL, BDCULLMODE_OFF, 1);

	ASSERT(shadow->numPolys);
	bdDrawPrimitive(BDPRIMTYPE_TRIANGLELIST,shadow->vertices,3*shadow->numPolys,BVERTTYPE_SINGLE);

	// PP: set normal blend mode back in case any subsequent rendering depends on it
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	// PP: ***** TEMP TEST - DRAW THE VERTS OF POLYS IN THE SHADOW


	if(shadowLightDebug)
	{
		TBVector	spritePos;

		SETZBIAS(-0.5f);

		if(shadow->light != NULL)// PP: the following stuff just applies to shadow lights...
		{
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

			// PP: draw shadowmap sphere...

			utilDrawSphere(shadow->actorInstance->actorInstance.position, shadow->shadowMapRadius, PINK);

			// PP: draw casting range as an XZ circle...

			utilDrawCircle(shadow->actorInstance->actorInstance.position, shadow->light->castRange*shadow->shadowMapRadius, BLUE);

			// PP: draw all shadow geometry...

			for(int i=0; i<(3*shadow->numPolys); i++)
			{
				BDVERTEX_GETPOS(&shadow->vertices[i], spritePos);

				bdDrawFlatSprite(
									spritePos,
									20.0f,
									20.0f,
									0.0f,
									QwilsonTexture,
									FALSE,
									FALSE,
									(bmFMod(gameStatus.appTime, 0.2f)>0.1f)?255:90,
									255,
									0,
									255,
									NULL,
									0.0f
								);

				BDVERTEX_SETCOLOUR(&shadow->vertices[i], GREEN);
			}

#if(BPLATFORM != PS2)// PP: TEMP!: crashes PS2 inside drawprim - too many verts?

			bdSetTexture(0,NULL);

			bdDrawPrimitive(BDPRIMTYPE_LINESTRIP, shadow->vertices,3*shadow->numPolys,BVERTTYPE_SINGLE);

#endif// PP: (BPLATFORM != PS2)
		}
	}


	// PP: write to the z buffer again
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);

	// PP: take the z bias off again
	bdSetZBias(0.0f);

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	// PP: Not sure where is the best place to put this.
	// PP: Sometimes the case arises that a wacky texture matrix gets left on.
	// PP: Seems to happen when drawing sprites and shadow maps (but not some other mystery ingredient) in the same frame
#if (BPLATFORM!=PS2 && BPLATFORM!=GAMECUBE)
	bdDisableTextureMatrix(0);
#endif

	// PP: restore fog
	RESTOREFOG;
}

/*	--------------------------------------------------------------------------------
	Function 	: PolyBoxIntersectXZ
	Purpose 	: Determines if a polygon intersects a box
	Parameters 	: box, p1, p2, p3 points on poly
	Returns 	: TRUE / FALSE
	Info 		: 
*/
BOOL PolyBoxIntersectXZ(TBVector box[4],TBVector p1,TBVector p2,TBVector p3)
{
	if (LineIntersectXZ(box[0],box[1],p1,p2)) return TRUE;
	if (LineIntersectXZ(box[0],box[1],p2,p3)) return TRUE;
	if (LineIntersectXZ(box[0],box[1],p3,p1)) return TRUE;

	if (LineIntersectXZ(box[1],box[2],p1,p2)) return TRUE;
	if (LineIntersectXZ(box[1],box[2],p2,p3)) return TRUE;
	if (LineIntersectXZ(box[1],box[2],p3,p1)) return TRUE;

	if (LineIntersectXZ(box[2],box[3],p1,p2)) return TRUE;
	if (LineIntersectXZ(box[2],box[3],p2,p3)) return TRUE;
	if (LineIntersectXZ(box[2],box[3],p3,p1)) return TRUE;

	if (LineIntersectXZ(box[3],box[0],p1,p2)) return TRUE;
	if (LineIntersectXZ(box[3],box[0],p2,p3)) return TRUE;
	if (LineIntersectXZ(box[3],box[0],p3,p1)) return TRUE;

	if (IsPointInPolyXZ(box[0],p1,p2,p3)) return TRUE;
	if (IsPointInPolyXZ(box[1],p1,p2,p3)) return TRUE;
	if (IsPointInPolyXZ(box[2],p1,p2,p3)) return TRUE;
	if (IsPointInPolyXZ(box[3],p1,p2,p3)) return TRUE;

	if (IsPointInPolyXZ(p1,box[0],box[1],box[2])) return TRUE;
	if (IsPointInPolyXZ(p1,box[0],box[2],box[3])) return TRUE;
	if (IsPointInPolyXZ(p2,box[0],box[1],box[2])) return TRUE;
	if (IsPointInPolyXZ(p2,box[0],box[2],box[3])) return TRUE;
	if (IsPointInPolyXZ(p3,box[0],box[1],box[2])) return TRUE;
	if (IsPointInPolyXZ(p3,box[0],box[2],box[3])) return TRUE;
	return FALSE;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														SHADOW LIGHTS

									Lightsources used to cast true character shadows
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "control.h"			// PP: controller stuff
#include "util.h"				// PP: Utility functions
#include "Slider.h"				// PP: Horizontal slider page-item used for controlling values, eg. music/sound fx volume.
#include "CheckBox.h"			// PP: Check box control page-item - lets you toggle something on & off

// PP: initialise static array of description strings for the shadow flags
const char* const SHADOWLIGHT::shadowFlagStrings[NUM_SHADOWLIGHT_SHADOW_FLAGS]=
{
	"Near-Side Test",
	"Angular Falloff",
	"Backface Cull",
	"Parallel Point",
	"Landscape Only",
	"Below Only",
	"Y Only"
};

bool		shadowLightDebug=false;
BOOK		shadowLightEditBook("SHADOW EDIT BOOK");	// PP: book used to edit shadow-casting lights (this is going to be SO cool! :)

float		shadowLight_gRange		=0;
float		shadowLight_gCastRange	=0;

/*	--------------------------------------------------------------------------------
	Function 	: SHADOWLIGHT::draw
	Purpose 	: debug-draw the shadow light
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#define SHADOWLIGHT_DRAW_SPARKLER_RADIUS			METERTOUNIT(0.6f)
#define SHADOWLIGHT_DRAW_ARROW_LENGTH				METERTOUNIT(1.2f)
#define SHADOWLIGHT_DRAW_TARGETTED_FLASH_TIME		0.2f
#define SHADOWLIGHT_DRAW_SELECTION_MARKER_RADIUS	150.0f

void SHADOWLIGHT::draw(void)
{
	// PP: TODO: draw diamond like in LUST

	if(this->type == SHADOWLIGHTTYPE_POINT)
	{
		if(this->infinite())
		{
			utilDrawSparkler(this->pos, SHADOWLIGHT_DRAW_SPARKLER_RADIUS, this->col);
		}
		else
		{
			utilDrawSphere(this->pos, this->range, this->col);
		}
	}
	else
	{
		utilDrawArrow(this->pos, this->ang, SHADOWLIGHT_DRAW_ARROW_LENGTH, this->col);
	}

	if(this->flagSet(this->editorFlags, SHADOWLIGHTFLAG_SELECTED))
	{
		utilDrawSelectionMarker(this->pos, SHADOWLIGHT_DRAW_SELECTION_MARKER_RADIUS, false);
	}
	else
	{
		if(this->flagSet(this->editorFlags, SHADOWLIGHTFLAG_TARGETTED))
		{
			utilDrawSelectionMarker(this->pos, SHADOWLIGHT_DRAW_SELECTION_MARKER_RADIUS, true);
		}
	}
}


// PP: ~~~~~~~~~~~~~~~~~~


/*	--------------------------------------------------------------------------------
	Function 	: shadowLightEdit_sliderUpdateFunc
	Purpose 	: callback function for the shadow light editor sliders
	Parameters 	: the value represented by the slider (0..1), ptr to slider
	Returns 	: 
	Info 		: called on every update of a slider, whether the value has changed or not
*/
void shadowLightEdit_sliderUpdateFunc(const float sliderValue, SLIDER* const slider)
{
	// PP: TEMP
	SHADOWLIGHT*	light;

	light=gameStatus.player1->actorShadow->light;

	if(light == NULL) return;

	if(*slider == "red")
	{
		light->col.r=(unsigned char)(sliderValue*255.0f);
	}
	else if(*slider == "green")
	{
		light->col.g=(unsigned char)(sliderValue*255.0f);
	}
	else if(*slider == "blue")
	{
		light->col.b=(unsigned char)(sliderValue*255.0f);
	}
	else if(*slider == "darkness")
	{
		light->shadowDarkness=sliderValue;
	}
	else if(*slider == "softness")
	{
		light->softness=sliderValue;
	}
}



/*	--------------------------------------------------------------------------------
	Function 	: initShadowLightEditBook
	Purpose 	: create the book for shadow-light editing
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#define SLED_SLIDER_SCALE				0.5f
#define SLED_FONT_SIZE					0.6f
#define SLED_TEXT_COLOUR				WHITE
#define	SLED_SPINNER_FONT_SIZE			0.75f
#define SLED_SPINNER_SPEED_RANGE		30.0f
#define SLED_SPINNER_SPEED_CASTRANGE	0.2f

static void initShadowLightEditBook(void)
{
	BOOK*		book= &shadowLightEditBook;
	PAGE*		page;
	PAGE*		subPage;
	TEXTBOX*	textBox;
	CHECKBOX*	checkBox;

	page=book->addPage();

	page->insertItem(subPage=new PAGE("PRIMARY CONTROLS"));
	{
#define PRIMARY_CONTROL(_label) \
			subPage->insertItem(textBox=new TEXTBOX()); \
			textBox->setName(_label); \
			textBox->sprintf8(_label); \
			textBox->setFontSize(SLED_FONT_SIZE); \
			textBox->setYPadding(false); \

		PRIMARY_CONTROL("Add Light");
		PRIMARY_CONTROL("Make Point");
		PRIMARY_CONTROL("Make Directional");
		PRIMARY_CONTROL("Select Light");
		PRIMARY_CONTROL("Move Light");
		PRIMARY_CONTROL("Delete Light");

		textBox->setYPadding(true);
		textBox->setYAlign(PIFLAG_YALIGN_TOP);
	}
	// PP: TEST
	subPage->setYPadding(false);
	page->insertItem(subPage=new PAGE("TAZ FLAGS"), false);
	{
		subPage->insertItem(textBox=new TEXTBOX("Player Flags"));
		textBox->setStyle(TS_title);
		textBox->setFontSize(SLED_FONT_SIZE);
		textBox->setSelectable(false);
		textBox->setYPadding(false);

		for(int i=0; i<NUM_SHADOWLIGHT_SHADOW_FLAGS; i++)
		{
			subPage->insertItem(checkBox=new CHECKBOX());
			checkBox->setName(SHADOWLIGHT::shadowFlagStrings[i]);
			checkBox->sprintf8(SHADOWLIGHT::shadowFlagStrings[i]);
			checkBox->setFontSize(SLED_FONT_SIZE);
			checkBox->setYPadding(false);
		}

		checkBox->setYPadding(true);
		checkBox->setYAlign(PIFLAG_YALIGN_TOP);
	}
	// PP: TEST
	subPage->setYPadding(false);
	page->insertItem(subPage=new PAGE("NME FLAGS"), false);
	{
		subPage->insertItem(textBox=new TEXTBOX("Enemy Flags"));
		textBox->setStyle(TS_title);
		textBox->setFontSize(SLED_FONT_SIZE);

		textBox->setYPadding(true);
		textBox->setYAlign(PIFLAG_YALIGN_TOP);
	}
	// PP: TEST
	subPage->setYPadding(false);
	page->insertItem(subPage=new PAGE("COLOUR ETC."), false);
	{
		TEXTBOX*	textBox;
		SLIDER*		slider;
		SPINNER*	spinner;

#define LABELLED_SLIDER(_label, _col, _func) \
			subPage->insertItem(textBox=new TEXTBOX()); \
			textBox->sprintf8(_label); \
			textBox->setFontSize(SLED_FONT_SIZE); \
			textBox->setXAlign(PIFLAG_XALIGN_RIGHT); \
			textBox->setVGrad(false); \
			textBox->setColour(_col); \
			textBox->setSelectable(false); \
			\
			subPage->insertItem(slider=new SLIDER(shadowLightEdit_sliderUpdateFunc, _label), false); \
			slider->setScale(SLED_SLIDER_SCALE); \
			slider->setYPadding(false); \
			slider->setXPadding(false);

		// PP: RED SLIDER
		LABELLED_SLIDER("Red", PINK, shadowLightEdit_sliderUpdateFunc);

		// PP: GREEN SLIDER
		LABELLED_SLIDER("Green", LIGHTGREEN, shadowLightEdit_sliderUpdateFunc);

		// PP: BLUE SLIDER
		LABELLED_SLIDER("Blue", LIGHTBLUE, shadowLightEdit_sliderUpdateFunc);

		// PP: DARKNESS SLIDER
		LABELLED_SLIDER("Darkness", SLED_TEXT_COLOUR, shadowLightEdit_sliderUpdateFunc);

		// PP: SOFTNESS SLIDER
		LABELLED_SLIDER("Softness", SLED_TEXT_COLOUR, shadowLightEdit_sliderUpdateFunc);

#define LABELLED_SPINNER(_label, _valPtr, _speed) \
			subPage->insertItem(textBox=new TEXTBOX()); \
			textBox->sprintf8(_label); \
			textBox->setFontSize(SLED_FONT_SIZE); \
			textBox->setXAlign(PIFLAG_XALIGN_RIGHT); \
			textBox->setVGrad(false); \
			textBox->setColour(SLED_TEXT_COLOUR); \
			textBox->setSelectable(false); \
			\
			subPage->insertItem(spinner=new SPINNER(SPINNERTYPE_UFLOAT, _valPtr, _speed, _label), false); \
			spinner->setFontSize(SLED_SPINNER_FONT_SIZE); \
			spinner->setXPadding(false); \
			spinner->setYPadding(false); \
			spinner->setYAlign(PIFLAG_YALIGN_TOP); \
			//spinner->setAlignScale(1.5f);

		LABELLED_SPINNER("Range:", &shadowLight_gRange, SLED_SPINNER_SPEED_RANGE);

		LABELLED_SPINNER("Cast range:", &shadowLight_gCastRange, SLED_SPINNER_SPEED_CASTRANGE);

		textBox->setYAlign(PIFLAG_YALIGN_TOP);
		textBox->setYPadding(true);
		spinner->setYAlign(PIFLAG_YALIGN_TOP);
		spinner->setYPadding(true);
	}

	// PP: TEST
	subPage->setYPadding(false);

	page->addRow();

	// PP: page is navigable
	page->setNavigation(true);

	// PP: in a columny stylee
	page->setColumnNavigation(true);
}


/*	--------------------------------------------------------------------------------
	Function 	: initShadowLights
	Purpose 	: initialise shadow lights before loading a scene
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void initShadowLights(void)
{
	initShadowLightEditBook();
}


/*	--------------------------------------------------------------------------------
	Function 	: setShadowLightDebug
	Purpose 	: turn shadow light debugging on/off
	Parameters 	: true=on, false=off
	Returns 	: 
	Info 		: 
*/
void setShadowLightDebug(const bool state)
{
	// PP: shadow light debug / editor is only available to us
#ifdef INFOGRAMES

	return;

#endif// PP: def INFOGRAMES

	// PP: TEMP
	SHADOWLIGHT*	light;

	light=gameStatus.player1->actorShadow->light;

	if(light == NULL) return;

	shadowLight_gRange=light->range;
	shadowLight_gCastRange=light->castRange;

	shadowLightDebug=state;

	if(state)
	{
		shadowLightEditBook.open();
	}
	else
	{
		shadowLightEditBook.close();
	}

	// PP: TEMP TEST
	gameStatus.controlSuspend=state;
}


/*	--------------------------------------------------------------------------------
	Function 	: toggleShadowLightDebug
	Purpose 	: toggle shadow light debugging
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void toggleShadowLightDebug(void)
{
	setShadowLightDebug(shadowLightDebug^1);
}


/*	--------------------------------------------------------------------------------
	Function 	: addShadowLight
	Purpose 	: Add a shadow light to the level
	Parameters 	: ref to pos VEC, (opt/point)type(SHADOWLIGHTTYPE_), (opt/0,0,0)ref to ang VEC,
					(opt/grey)ref to COLOUR, (opt/default)shadow darkness 0..1,
					(opt/infinite)range, (opt/default)player flags, (opt/default)enemy flags,
					(opt/default)cast range, (opt/default)softness, (opt/subtractive)blend type (SHADOWBLEND_)
	Returns 	: 
	Info 		: 
*/
SHADOWLIGHT* addShadowLight(const VEC&				pos,
							const EShadowLightType	type,
							const VEC&				ang,		
							const COLOUR&			col,
							const float				shadowDarkness,
							const float				range,
							const int				playerFlags,
							const int				enemyFlags,
							const float				castRange,
							const float				softness,
							const EShadowBlendMode	blend)
{
	SHADOWLIGHT*	light;
	
	light=shadowLights.getNew();

	light->pos=pos;
	light->type=type;
	light->ang=ang;
	light->col=col;
	light->shadowDarkness=shadowDarkness;
	light->range=range;
	light->playerFlags=playerFlags;
	light->enemyFlags=enemyFlags;
	light->editorFlags=SHADOWLIGHT_DEFAULT_EDITOR_FLAGS;
	light->castRange=castRange;
	light->softness=softness;
	light->blend=blend;

	light->dir=VEC(0,0,1);

	if(light->type == SHADOWLIGHTTYPE_DIRECTIONAL)
	{
		light->dir.rotate2(light->ang);

		bkPrintf("adding directional shadow light, ang %f, %f, %f\n", light->ang[X], light->ang[Y], light->ang[Z]);
	}
	else
	{
		bkPrintf("adding point shadow light, pos %f, %f, %f\n", light->pos[X], light->pos[Y], light->pos[Z]);
	}

	return light;
}


/*	--------------------------------------------------------------------------------
	Function 	: drawShadowLights
	Purpose 	: debug-draw all shadow lights in the level
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void drawShadowLights(void)
{
	if(shadowLights.size == 0)
	{
		return;
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	LA_FOREACHDO(shadowLights, draw());
}



/*	--------------------------------------------------------------------------------
	Function 	: pickShadowLight
	Purpose 	: pick a lightsource based on the camera position & orientation
	Parameters 	: 
	Returns 	: 
	Info 		: uses flyCam
*/
SHADOWLIGHT* pickShadowLight(void)
{
	SHADOWLIGHT*	bestLight=NULL;
	float			minDist=FLOATMAX;
	float			dist;

	LA_FOREACH(shadowLights, SHADOWLIGHT, light);
	
		dist=light->pos.distFromLine(flyCam.pos, flyCam.lookAt, true);

		if(dist<minDist)
		{
			minDist=dist;
			bestLight=light;
		}

	LA_ENDFOREACH(shadowLights);

	return bestLight;

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: targetShadowLight
	Purpose 	: exclusively target the specified light
	Parameters 	: ptr to light to target
	Returns 	: 
	Info 		: 
*/
void targetShadowLight(SHADOWLIGHT* const light)
{
	LA_FOREACHDO(shadowLights, target(false));

	light->target();
}


/*	--------------------------------------------------------------------------------
	Function 	: selectShadowLight
	Purpose 	: exclusively select the specified light
	Parameters 	: ptr to light to select
	Returns 	: 
	Info 		: 
*/
void selectShadowLight(SHADOWLIGHT* const light)
{
	LA_FOREACHDO(shadowLights, select(false));

	light->select();
}


/*	--------------------------------------------------------------------------------
	Function 	: updateShadowLights
	Purpose 	: update shadow light stuff
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void updateShadowLights(void)
{
	// PP: TEMP
	SHADOWLIGHT*	light;

	light=gameStatus.player1->actorShadow->light;

	if(light == NULL) return;

	shadowLight_gRange=light->range;
	shadowLight_gCastRange=light->castRange;

	// PP: update the editor book...

	light->range=shadowLight_gRange;
	light->castRange=shadowLight_gCastRange;

	// PP: light targetting...

	SHADOWLIGHT*	targetLight;

	targetLight=pickShadowLight();

	if(targetLight != NULL)
	{
		targetShadowLight(targetLight);
	}

	// PP: X to select a light

	if(controller1.crossDebounceChannel->value)
	{
		selectShadowLight(targetLight);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: getShadowLightForCharacter
	Purpose 	: TEMP
	Parameters 	: TEMP
	Returns 	: TEMP
	Info 		: TEMP
*/
SHADOWLIGHT* getShadowLightForCharacter(const ACTORINSTANCE* const actorInstance)
{
	// PP: TEMP: get a ptr to the closest lightsource, which will cast the shadow

	float			testDist;
	float			closestDist;
	SHADOWLIGHT*	closestLight;

	if(actorInstance->actorShadow->light == NULL)
	{
		closestLight=NULL;
		closestDist=FLOATMAX;
	}
	else
	{
		closestLight=actorInstance->actorShadow->light;
		closestDist=distance(closestLight->pos, actorInstance->actorInstance.position);
	}

	LA_FOREACH(shadowLights, SHADOWLIGHT, testLight);

		testDist=distance(testLight->pos, actorInstance->actorInstance.position);

		if(testDist < closestDist)
		{
			closestLight=testLight;
		}
	
	LA_ENDFOREACH(shadowLights);

	return closestLight;
}


/*	--------------------------------------------------------------------------------
	Function 	: SHADOWLIGHT::setFlag
	Purpose 	: set one of the light's flags either high or low
	Parameters 	: ref to flag group, flag to set, (opt/true)state to which to set flag
	Returns 	: 
	Info 		: 
*/
inline void SHADOWLIGHT::setFlag(ShadowLightFlags& flags, const EShadowLightFlag flag, const bool state)
{
	flags |= SHADOWLIGHT_FLAG(flag);
}


/*	--------------------------------------------------------------------------------
	Function 	: SHADOWLIGHT::select
	Purpose 	: select or deselect the light
	Parameters 	: (opt/true)true to select, false to deselect
	Returns 	: this
	Info 		: 
*/
inline struct TAG_SHADOWLIGHT* SHADOWLIGHT::select(const bool state)
{
	setFlag(this->editorFlags, SHADOWLIGHTFLAG_SELECTED, state);

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: SHADOWLIGHT::target
	Purpose 	: target or untarget the light
	Parameters 	: (opt/true)true to target, false to untarget
	Returns 	: this
	Info 		: you target a light by pointing the camera at it, and select it by pressing X
*/
inline struct TAG_SHADOWLIGHT* SHADOWLIGHT::target(const bool state)
{
	setFlag(this->editorFlags, SHADOWLIGHTFLAG_TARGETTED, state);

	return this;
}