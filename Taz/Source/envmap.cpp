// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : envmap.cpp
//   Purpose : provide basic display setup and managment functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "envmap.h"
#include "clouds.h"
#include "maths.h"
#include "videofx.h"
#include "display.h"


const float box[6][12] = {
	{ 1,-1,-1, -1,-1,-1, 1,1,-1, -1,1,-1, },	// front
	{ -1,-1,1, 1,-1,1, -1,1,1, 1,1,1, },		// back
	{ 1,-1,1, 1,-1,-1, 1,1,1, 1,1,-1, },		// left
	{ -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, },	// right
	{ 1,-1,1, -1,-1,1, 1,-1,-1, -1,-1,-1, },	// top
	{ 1,1,-1, -1,1,-1, 1,1,1, -1,1,1, },		// bottom
};

float drawBox[6][12];


/* --------------------------------------------------------------------------------
   Function : SetupEnvMapSpecial
   Purpose : Create and setup an enviroment map using only a focal texture and secondary texture
   Parameters : ENVMAP, scale, texture names
   Returns : TRUE/FALSE
   Info :
*/
int SetupEnvMapSpecial(ENVMAP *envMap, float scale, char *main, char *second, char *third, float angle, float hOffset)
{
	int			r,v,i,j;
	char		three = FALSE;
	TBVector	tempVector;

	envMap->scale = scale;

	envMap->texturelist[0] = bkLoadTexture(NULL,main,0);	// front
	if (third)
	{
		if (third[0]!=0)
		{
			envMap->texturelist[1] = bkLoadTexture(NULL,third,0);	// back
			three = TRUE;
		}
		else
		{
			envMap->texturelist[1] = bkLoadTexture(NULL,second,0);	// back
		}
	}
	else
	{
		envMap->texturelist[1] = bkLoadTexture(NULL,second,0);	// back
	}

	envMap->texturelist[2] = bkLoadTexture(NULL,second,0);	// left
	envMap->texturelist[3] = bkLoadTexture(NULL,second,0);	// right
	envMap->texturelist[4] = bkLoadTexture(NULL,second,0);	// top
	envMap->texturelist[5] = bkLoadTexture(NULL,second,0);	// bottom

	for (r=0;r<6;r++)
	{
		if (!envMap->texturelist[r])
		{
			// texture missing
			return FALSE;
		}
	}
	
	// 0,1 top 2,3 bottom
	for (r=0;r<6;r++)
	{
		if ((r==3)&&(three))
		{
			BDVERTEX_SETUV(&envMap->envmapverts[r][0], 1 , -1.25f);
			BDVERTEX_SETUV(&envMap->envmapverts[r][1], 0 , -1.25f);
			BDVERTEX_SETUV(&envMap->envmapverts[r][2], 1 ,  2.75f);
			BDVERTEX_SETUV(&envMap->envmapverts[r][3], 0 ,  2.75f);
		}
		else
		{
			BDVERTEX_SETUV(&envMap->envmapverts[r][0], 0 , -1.25f);
			BDVERTEX_SETUV(&envMap->envmapverts[r][1], 1 , -1.25f);
			BDVERTEX_SETUV(&envMap->envmapverts[r][2], 0 ,  2.75f);
			BDVERTEX_SETUV(&envMap->envmapverts[r][3], 1 ,  2.75f);
		}

		for (v=0;v<4;v++)
		{
			BDVERTEX_SETRGBA(&envMap->envmapverts[r][v], 127, 127, 127, 127);
			BDVERTEX_SETNORMAL(&envMap->envmapverts[r][v], 0.0f, 0.0f, -1.0f);
		}
	}
	BDVERTEX_SETUV(&envMap->envmapverts[4][0], 0, 0);
	BDVERTEX_SETUV(&envMap->envmapverts[4][1], 0, 0);
	BDVERTEX_SETUV(&envMap->envmapverts[4][2], 1, 0);
	BDVERTEX_SETUV(&envMap->envmapverts[4][3], 1, 0);

	for (i=0;i<6;i++)
	{
		for (j=0;j<4;j++)
		{
			tempVector[X] = box[i][j*3];
			tempVector[Y] = box[i][(j*3)+1];
			tempVector[Z] = box[i][(j*3)+2];

			// TP: rotate if necessary
			bmVectorRotateY(tempVector,tempVector,angle);

			drawBox[i][j*3] = tempVector[X];
			drawBox[i][(j*3)+1] = tempVector[Y];
			drawBox[i][(j*3)+2] = tempVector[Z];
		}
	}

	envMap->yOff = hOffset;

	for (v=0;v<4;v++)	BDVERTEX_SETUV(&envMap->envmapverts[5][v], 0, 1);

	envMap->camera[X] = 0.0f;
	envMap->camera[Y] = 0.0f;
	envMap->camera[Z] = 0.0f;
	envMap->camera[W] = 1.0f;
	
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SetupEnvMap
   Purpose : Create and setup an enviroment map
   Parameters : ENVMAP, scale, texture names
   Returns : TRUE/FALSE
   Info :
*/
int SetupEnvMap(ENVMAP *envMap, float scale, char *front, char *back, char *left, char *right, char *top, char *bottom)
{
	int r,v;

	envMap->scale = scale;

	envMap->texturelist[0] = bkLoadTexture(NULL,front,0);
	envMap->texturelist[1] = bkLoadTexture(NULL,back,0);
	envMap->texturelist[2] = bkLoadTexture(NULL,left,0);
	envMap->texturelist[3] = bkLoadTexture(NULL,right,0);
	envMap->texturelist[4] = bkLoadTexture(NULL,top,0);
	envMap->texturelist[5] = bkLoadTexture(NULL,bottom,0);

	for (r=0;r<6;r++)
	{
		if (!envMap->texturelist[r])
		{
			// texture missing
			return FALSE;
		}
	}

	for (r=0;r<6;r++)
	{
		for (v=0;v<4;v++)
		{
			if (r==4)
			{
				BDVERTEX_SETUV(&envMap->envmapverts[r][3-v], (float)(v & 1) , (float)((v&2)>>1));
			}
			else
			{
				BDVERTEX_SETUV(&envMap->envmapverts[r][v], (float)(v & 1) , (float)((v&2)>>1));
			}
			BDVERTEX_SETRGBA(&envMap->envmapverts[r][v], 127, 127, 127, 127);
			BDVERTEX_SETNORMAL(&envMap->envmapverts[r][v], 0.0f, 0.0f, -1.0f);
			BDVERTEX_SETXYZW(&envMap->envmapverts[r][v], box[r][v*3], -box[r][v*3+1], box[r][v*3+2], 1.0f);
		}
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : PrepareEnvMap
   Purpose : Prepare the map based on the view vector
   Parameters : ENVMAP, view vector
   Returns : 
   Info :
*/
void PrepareEnvMap(ENVMAP *envMap, TBVector camera)
{
	// TP: update lateral movement
	envMap->camera[X] = camera[X];
	envMap->camera[Z] = camera[Z];
}

/* --------------------------------------------------------------------------------
   Function : RenderEnvMap
   Purpose : Render the environment map
   Parameters : ENVMAP
   Returns : 
   Info :
*/
void RenderEnvMap(ENVMAP *envMap)
{
	int r,v;

#if BPLATFORM == PC
	
	// NH: If farplane nearer than on PS2 (300) then have a look at scraping the envbox so it doesn't show through the walls
	if (videoMode.farPlane < METERTOUNIT(300))
	{
		// NH: If in indoor based levels return as we don't want to show the envmap through walls that are clipped
		switch(map.sceneNumber)
		{
		case SCENE_DEPTSTR:
		case SCENE_MUSEUM:
		case SCENE_GOLDMINE:
		case SCENE_CITYBOSS:	// NH: Needs fog enabled
		case SCENE_WESTBOSS:	// NH: Needs fog enabled?
		case SCENE_TAZPREBOSS:	// NH: Fog Not Needed
		case SCENE_TAZBOSS:		// NH: Needs fog enabled?
			return;
			break;
		}
	}

	bdSetViewClipPlanes(videoMode.nearPlane, METERTOUNIT(600));
	bdSetupView();
#endif

	// texture list order: front, back, left, right, top, bottom
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);
	
	if (gameStatus.currentState==GAMESTATE_CAMERACASE)
	{
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 0);
	}
	else
	{
		if (VFX_effectActive(VFX_FOCUS))
		{
			bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 0);// PP: was false, needs to write to zbuffer for DOF to work
		}
		else
		{
			bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 0);
		}
	}
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, 0);
	bdSetRenderState(BDRENDERSTATE_LIGHTING,FALSE,0);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);// PP: 
	bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);

	// PP: Not sure where is the best place to put this.
	// PP: Sometimes the case arises that a wacky texture matrix gets left on.
	// PP: Seems to happen when drawing sprites and shadow maps (but not some other mystery ingredient) in the same frame
#if (BPLATFORM!=PS2 && BPLATFORM!=GAMECUBE)
	bdDisableTextureMatrix(0);
#endif

	bdSetIdentityObjectMatrix();
	for (r=0;r<6;r++)
	{
		for (v=0;v<4;v++)
		{
			BDVERTEX_SETXYZW(&envMap->envmapverts[r][v], envMap->camera[0]+envMap->scale*drawBox[r][v*3],
												envMap->camera[1]+envMap->scale*-drawBox[r][v*3+1]+envMap->yOff,
												envMap->camera[2]+envMap->scale*drawBox[r][v*3+2], 1.0f);
		}
		bdSetTexture(0,envMap->texturelist[r]);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, envMap->envmapverts[r], 4, BVERTTYPE_SINGLE);
	}

	DrawCloudBankBackground();

	if (map.fogState)
	{
		bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, TRUE, FALSE);
	}
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 0);
	bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);

#if BPLATFORM == PC
	bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
	bdSetupView();
#endif
}


