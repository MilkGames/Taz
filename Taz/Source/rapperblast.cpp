// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : rumble.cpp
//   Purpose : pad rumbling controls
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "maths.h"
#include "characters.h"
#include "util.h"
#include "textures.h"
#include "rapperblast.h"


/* --------------------------------------------------------------------------------
   Function : UpdateRapperBlast
   Purpose : creates waves of sound to blast enemies, should be called every frame while the attack is taking place
   Parameters : 
   Returns : 
   Info : 
*/

void CreateRapperBlast(ACTORINSTANCE *actorInstance)
{
	RAPPERBLASTINFO		*info;
	int					i, j, k;
	float				angle, angleInc;
	TBMatrix			rotMatrix, transMatrix, resMatrix;
	TBQuaternion		rotation, tempQuat;
	TBVector			tempVec, position;

	if(!(info = actorInstance->characterInfo->rapperBlastInfo)) return;

	info->clock = 0.0f;
	info->maxAngle = RAD(45.0f);
	info->minAngle = RAD(-45.0f);

	angleInc = (info->maxAngle - info->minAngle)/NUM_RAPPERBLASTPOINTS;

	for(i = RAPPERBLAST_AUDIOMODE;i >= 0;--i)
	{
		baGetNodesWorldPosition(&actorInstance->actorInstance, info->speakers[i], position);

		for(j = (NUM_RAPPERBLASTSTREAKS-1);j >= 0;--j)
		{
			angle = info->minAngle;
	
			for(k = (NUM_RAPPERBLASTPOINTS-1);k >= 0;--k)
			{
				// CMD: calculate initial position
				bmVectorSet(info->streaks[i][j].points[k], 0.0f, METERTOUNIT(0.5f), -(RAPPERBLAST_MINRADIUS+((float)j*RAPPERBLAST_RADIUSINC)), 1.0f);
	
				bmVectorSet(tempVec, 0.0f, 1.0f, 0.0f, angle);
				bmRotationToQuat(rotation, tempVec);
				bmQuatMultiply(tempQuat, actorInstance->actorInstance.orientation, rotation);
				bmQuatToMatrix(rotMatrix, tempQuat);
				bmMatTranslate(transMatrix, position[0], position[1], position[2]);
				bmMatMultiply(resMatrix, transMatrix, rotMatrix);

				bmMatMultiplyVector(resMatrix, info->streaks[i][j].points[k]);

				// CMD: size
				info->streaks[i][j].size[k] = METERTOUNIT(0.1f);

				// CMD: colour
				info->streaks[i][j].colour[k][0] = 127;
				info->streaks[i][j].colour[k][1] = 124;
				info->streaks[i][j].colour[k][2] = 40;
				info->streaks[i][j].colour[k][3] = 60;
				info->streaks[i][j].alpha[k] = 60.0f;

				angle += angleInc;
			}
		}
	}
	info->draw = 2;
}

/* --------------------------------------------------------------------------------
   Function : UpdateRapperBlast
   Purpose : updates rapper blast effect
   Parameters : 
   Returns : 
   Info : Willy Weasil is, contrary to general opinion, a fantastic game character
*/

void UpdateRapperBlast(ACTORINSTANCE *actorInstance)
{
	RAPPERBLASTINFO		*info;
	TBActorInstance		*attachedInstance;
	int					i, j, k, finished;
	float				angle, angleInc, delta, delConst, dist;
	TBMatrix			rotMatrix, transMatrix, resMatrix;
	TBQuaternion		rotation, tempQuat;
	TBVector			tempVec, position;

	if(!(info = actorInstance->characterInfo->rapperBlastInfo)) return;
	if(info->draw == FALSE) return;

	switch(info->draw)
	{
	case 2:
		angleInc = (info->maxAngle - info->minAngle)/NUM_RAPPERBLASTPOINTS;

		info->clock += fTime;

		for(i = RAPPERBLAST_AUDIOMODE;i >= 0;--i)
		{
			attachedInstance = ReturnAttachedObjectInstance(actorInstance, actorInstance->characterInfo->costumeHandle);
			if(attachedInstance)
			{
				switch(i)
				{
				case 0:
					info->speakers[i] = baFindNode(attachedInstance->rootNodeInstance, "speakercase01");
					break;
				case 1:
					info->speakers[i] = baFindNode(attachedInstance->rootNodeInstance, "speakercase02");
					break;
				case 2:
					info->speakers[i] = baFindNode(attachedInstance->rootNodeInstance, "speakercase03");
					break;
				case 3:
					info->speakers[i] = baFindNode(attachedInstance->rootNodeInstance, "speakercase04");
					break;
				}
				if(info->speakers[i])
				{
					baGetNodesWorldPosition(&actorInstance->actorInstance, info->speakers[i], position);

					for(j = (NUM_RAPPERBLASTSTREAKS-1);j >= 0;--j)
					{
						angle = info->minAngle;
				
						for(k = (NUM_RAPPERBLASTPOINTS-1);k >= 0;--k)
						{
							// CMD: calculate perturbation
							delConst = angle*RAD(90.0f)/info->maxAngle;
							delta = /*amplitude*/ RAPPERBLAST_PERTAMP * 
									/*attenuation to amplitude from zero to max to zero*/ bmCos(delConst) * 
									/*sin wave series, varies with time*/ (bmCos(3.0f*delConst + 12.0f*info->clock) + bmCos(6.0f*delConst + 14.0f*info->clock) + bmCos(9.0f*delConst + 16.0f*info->clock));

							// CMD: calculate position including perturbation
			//				bmVectorSet(info->streaks[i][j].points[k], 0.0f, METERTOUNIT(0.5f), -(delta+RAPPERBLAST_MINRADIUS+((float)j*RAPPERBLAST_RADIUSINC)), 1.0f);
							if((dist = delta+(METERTOUNIT((actorInstance->characterInfo->timer - ((float)j*2.0f))))) < 0.0f) dist = 0.0f;
							bmVectorSet(info->streaks[i][j].points[k], 0.0f, METERTOUNIT(0.5f), -dist, 1.0f);
				
							bmVectorSet(tempVec, 0.0f, 1.0f, 0.0f, angle);
							bmRotationToQuat(rotation, tempVec);
							bmQuatMultiply(tempQuat, actorInstance->actorInstance.orientation, rotation);
							bmQuatToMatrix(rotMatrix, tempQuat);
							bmMatTranslate(transMatrix, position[0], position[1], position[2]);
							bmMatMultiply(resMatrix, transMatrix, rotMatrix);

							bmMatMultiplyVector(resMatrix, info->streaks[i][j].points[k]);

							// CMD: calculate alpha based on distance travelled
							if((info->streaks[i][j].alpha[k] = 60.0f - (60.0f*dist/METERTOUNIT(10.0f))) < 0.0f) info->streaks[i][j].alpha[k] = 0.0f;
							info->streaks[i][j].colour[k][3] = (int)info->streaks[i][j].alpha[k];

							angle += angleInc;
						}
					}
				}
				else
				{
					info->draw = 1;
				}
			}
			else
			{
				info->draw = 1;
			}
		}
		break;
	case 1:
		finished = TRUE;
		for(i = RAPPERBLAST_AUDIOMODE;i >= 0;--i)
		{
			for(j = (NUM_RAPPERBLASTSTREAKS-1);j >= 0;--j)
			{
				for(k = (NUM_RAPPERBLASTPOINTS-1);k >= 0;--k)
				{
					// CMD: reduce alpha
					if((info->streaks[i][j].alpha[k] -= 120.0f*fTime) < 0.0f)
					{
						info->streaks[i][j].alpha[k] = 0.0f;
						info->streaks[i][j].colour[k][3] = 0;
					}
					else
					{
						info->streaks[i][j].colour[k][3] = (int)info->streaks[i][j].alpha[k];
						finished = FALSE;
					}
				}
			}
		}
		if(finished) info->draw = FALSE;
		break;
	}
}
	
/* --------------------------------------------------------------------------------
   Function : DrawRapperBlast
   Purpose : draws rapper blast effect
   Parameters : 
   Returns : 
   Info : Willy Weasil is, contrary to general opinion, a fantastic game character
*/

void DrawRapperBlast(ACTORINSTANCE *actorInstance)
{
	RAPPERBLASTINFO		*info;
	int32				noofVerts;
	int					i, j, k;
	float				screenSize[NUM_RAPPERBLASTPOINTS];


	if(!(info = actorInstance->characterInfo->rapperBlastInfo)) return;
	if(!info->draw) return;

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	for(i = RAPPERBLAST_AUDIOMODE;i >= 0;--i)
	{
		for(j = (NUM_RAPPERBLASTSTREAKS-1);j >= 0;--j)
		{
	
			for(k = (NUM_RAPPERBLASTPOINTS-1);k >= 0;--k)
			{
				info->streaks[i][j].points[k][3] = 1.0f;
			}

			bdSetIdentityObjectMatrix();
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			noofVerts = bdDrawMultiStreakClipped(NUM_RAPPERBLASTPOINTS, &info->streaks[i][j].points[0][0], info->streaks[i][j].size, screenSize,
												&info->streaks[i][j].colour[0][0], global2DVertexBuffer, 0.0f);

			if(noofVerts)
			{
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
				bdSetTexture(0, textureMechaElec);
				bdSetIdentityObjectMatrix();
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, noofVerts, BVERTTYPE_SINGLE2D);
			}
		}
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);

}