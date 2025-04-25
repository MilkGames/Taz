// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : textures.cpp
//   Purpose : function to assist in the management of textures
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "textures.h"
#include "files.h"
#include "CheckBox.h"			// PP: Check box control page-item - lets you toggle something on & off
#include "ZooBoss.h"			// PP: Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown

//global texture ptrs
TBTexture		*textureWhiteBubble;
TBTexture		*textureGreenBubble;
TBTexture		*textureSmokeCloud;
TBTexture		*textureChilliFire;
TBTexture		*textureSleepyZeds;
TBTexture		*texturePowerUpGlow;
TBTexture		*textureSwirl;
TBTexture		*textureSandwich;
TBTexture		*textureShadow;
TBTexture		*textureFootPrintLeft;
TBTexture		*textureFootPrintRight;
TBTexture		*textureRain;
TBTexture		*textureSnow;
TBTexture		*texturePhoneWhite;
TBTexture		*texturePhoneOrange;
TBTexture		*textureLightning;
TBTexture		*textureCrank;
TBTexture		*textureWaterRing;
TBTexture		*textureWaterSplash;
TBTexture		*textureFountain;
TBTexture		*waterWibble;
TBTexture		*textureTwinkle;
TBTexture		*textureSpark;
TBTexture		*textureRing;
TBTexture		*textureHaar;
TBTexture		*textureMechaElec;
TBTexture		*textureBadMechaElec;
TBTexture		*textureSpin;
TBTexture		*textureInvisSpin;
TBTexture		*textureTomato;
TBTexture		*textureCucumber;
TBTexture		*textureCrumb;
TBTexture		*textureGenericGlow;
TBTexture		*textureWhite;					// PP: 16*16*4bit pure white
TBTexture		*textureMagenta;				// PP: 16*16*4bit pure magenta
TBTexture		*textureWaterfallSpray;
TBTexture		*textureSandwichGlow;
TBTexture		*textureLoseCostumeGlow;
/////////////////////////////

#define MAX_MANUAL_GIFS			(32)

TEXSCROLLINFO			scrollingTextures[MAX_SCROLLTEXTURES];
static TBTexture *ppTextureAnimateManually[MAX_MANUAL_GIFS];
static int nTextureAnimateManually;

/* --------------------------------------------------------------------------------
   Function : LoadTextures
   Purpose : Load textures from a package
   Parameters : package
   Returns : TRUE, FALSE
   Info : 
*/
int LoadTextures(char *package)
{
	TBPackageIndex	*pakIndex;

	bkPrintf("LoadTextures(%s)\n",package);

	if (pakIndex = FindPackage(package))
	{
		bkLoadPackageTextures(pakIndex);
	}
	else
	{
		if ((pakIndex = bkOpenPackage(package)))
		{
			// Load all textures
			bkLoadPackageTextures(pakIndex);
			bkClosePackage(pakIndex);
			return TRUE;
		}
		else
		{
			bkPrintf("*** ERROR *** Package %s not loaded\n",package);
			return FALSE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : FreeTextures
   Purpose : Free all textures loaded from specified package
   Parameters : package
   Returns : 
   Info : 
*/
void FreeTextures(char *package)
{

	if (FindPackage(package)) return;

	bkPrintf("FreeTextures(%s)\n",package);
/*	if ((pakIndex = bkOpenPackage(package)))
	{
		bkDeletePackageTextures(bkGetPackageID(pakIndex));
		bkClosePackage(pakIndex);
	}*/
}

static	TBTexture	**animatedTextures;
static	int			numAnimatedTextures;

/* --------------------------------------------------------------------------------
   Function : ListAnimatingTextures
   Purpose : Create a list of all animating textures
   Parameters : 
   Returns : 
   Info : 
*/
void ListAnimatingTextures(void)
{
	TBTexture	*myTexture;
	int			i = 0;
	int			noFrames;

	// TP: find number of animating textures

    myTexture = NULL;

    while((myTexture = (TBTexture *)bkWalkResourceList(EBRESTYPE_TEXTURE, (TBResourceInfo *)myTexture)) != NULL)
    {
		bdGetTextureInfo(myTexture,NULL,NULL,NULL,&noFrames,NULL);
		if (noFrames>1)
		{
			i++;
		}
    }
	
	// TP: malloc memory for pointers
	if (i==0) 
	{
		animatedTextures = NULL;
		numAnimatedTextures = 0;
		return;
	}

	animatedTextures = (TBTexture**)ZALLOC(i*sizeof(TBTexture*));
	ASSERT(animatedTextures);

	// TP: get pointer to all animated textures
    myTexture = NULL;
	i = 0;
    while((myTexture = (TBTexture *)bkWalkResourceList(EBRESTYPE_TEXTURE, (TBResourceInfo *)myTexture)) != NULL)
    {
		bdGetTextureInfo(myTexture,NULL,NULL,NULL,&noFrames,NULL);

		if ((noFrames > 1) && !SearchForTexturesToAnimateManually(myTexture))
		{
			animatedTextures[i++] = myTexture;
		}
    }

	numAnimatedTextures = i;
}

static	float	animUpdate  = 0;

/* --------------------------------------------------------------------------------
   Function : UpdateAnimatingTextures
   Purpose : Update the animating textures
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateAnimatingTextures(void)
{
	int		i;
	int		t;
#if 1
	if (!animatedTextures) return;

	animUpdate += fTime;
	t = (int)(animUpdate*1000.0f);

	for (i=0;i<numAnimatedTextures;i++)
	{
		bdSetTextureFrameByTime(animatedTextures[i],t);
	}

#else
	int		curFrame,noFrames;

	if (!animatedTextures) return;

	animUpdate += fTime;
	while (animUpdate > 0.1f)
	{
		// TP: update all texture by one frame
		for (i=0;i<numAnimatedTextures;i++)
		{
			bdGetTextureInfo(animatedTextures[i],NULL,NULL,NULL,&noFrames,&curFrame);
			curFrame++;
			if (curFrame == noFrames) curFrame = 0;
			bdSetTextureFrame(animatedTextures[i],curFrame);
		}
		animUpdate -= 0.1f;
	}
#endif
}


/* --------------------------------------------------------------------------------
   Function : SetTextureFrame
   Purpose : set the frame for a texture
   Parameters : texture index, frame number
   Returns : 
   Info : // PP: 
*/
void SetTextureFrame(const int index, const int frame)
{
	bdSetTextureFrame(animatedTextures[index],frame);
}


/* --------------------------------------------------------------------------------
   Function : FreeAnimatingTextures
   Purpose : Free the animating textures list`
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAnimatingTextures(void)
{
	if (animatedTextures) FREE(animatedTextures);
}


/* --------------------------------------------------------------------------------
   Function : SpecifyTexturesToAnimateManually
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

TBTexture *SpecifyTexturesToAnimateManually(char *sName)
{
	TBTexture *pTexture = bkLoadTexture(NULL, sName, BDEFAULTGROUP);

	if(pTexture)
	{
		if(nTextureAnimateManually < MAX_MANUAL_GIFS)
		{
			ppTextureAnimateManually[nTextureAnimateManually++] = pTexture;
			bdSetTextureFrame(pTexture, 0);
		}
	}

	return pTexture;
}

/* --------------------------------------------------------------------------------
   Function : SearchForTexturesToAnimateManually
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int SearchForTexturesToAnimateManually(TBTexture *pTexture)
{
	if(nTextureAnimateManually == 0)
		return 0;

	for(int t=0; t<nTextureAnimateManually; t++)
	{
		if(pTexture == ppTextureAnimateManually[t])
			return 1;
	}
	return 0;
}

/* --------------------------------------------------------------------------------
   Function : SetTextureFrameManually
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetTextureFrameManually(TBTexture *pTexture, int f)
{
	bdSetTextureFrame(pTexture, f);
}


/* --------------------------------------------------------------------------------
   Function : InitialiseCommonlyUsedTextures
   Purpose : Initialises global texture ptrs once, to stop continual crc searching
   Parameters : package
   Returns : TRUE, FALSE
   Info : 
*/

void InitialiseCommonlyUsedTextures(void)
{
	// PP: I hope you don't mind me macroing this up - I just didn't like the look of all that repeated code

	// PP: Macro to load a commonly-used texture
#define LOAD_COMMON_TEXTURE(_ptr, _filename) \
		if(!((_ptr) = bkLoadTexture(NULL,(_filename),0))) \
		{ \
			bkPrintf("*** WARNING *** Could not load common texture \"%s\" (setting '%s' pointer to NULL).\n", _filename, #_ptr); \
			(_ptr)=NULL; \
		}

	LOAD_COMMON_TEXTURE(textureWhiteBubble,		"whitebubble.tga");
	LOAD_COMMON_TEXTURE(textureGreenBubble,		"greenbubble.tga");
	LOAD_COMMON_TEXTURE(textureSmokeCloud,		"cloudgrey.bmp");
	// PP: REMOUT: OOD:	LOAD_COMMON_TEXTURE(textureChilliFire,		"chilliflame.tga");
	LOAD_COMMON_TEXTURE(textureChilliFire,		"chillialt.tga");
	LOAD_COMMON_TEXTURE(textureSleepyZeds,		"zed.tga");
	LOAD_COMMON_TEXTURE(texturePowerUpGlow,		"powerupglow.tga");
	LOAD_COMMON_TEXTURE(textureSwirl,			"swirl.tga");
	LOAD_COMMON_TEXTURE(textureSandwich,		"sandwich.bmp");
	LOAD_COMMON_TEXTURE(textureShadow,			"shadowsub.bmp");
	LOAD_COMMON_TEXTURE(textureFootPrintLeft,	"footprintl.tga");
	LOAD_COMMON_TEXTURE(textureFootPrintRight,	"footprintr.tga");
	LOAD_COMMON_TEXTURE(textureRain,			"raindrop.tga");
	LOAD_COMMON_TEXTURE(textureSnow,			"snow2.tga");
	LOAD_COMMON_TEXTURE(texturePhoneWhite,		"phoneboxwhite.tga");
	LOAD_COMMON_TEXTURE(texturePhoneOrange,		"phoneboxorange.tga");
	LOAD_COMMON_TEXTURE(textureLightning,		"lightning.tga");
	LOAD_COMMON_TEXTURE(textureCrank,			"crank.bmp");
	LOAD_COMMON_TEXTURE(textureWaterSplash,		"watersplash.tga");
	LOAD_COMMON_TEXTURE(textureFountain,		"fountain.bmp");
	LOAD_COMMON_TEXTURE(waterWibble,			"wash.tga");
	LOAD_COMMON_TEXTURE(textureTwinkle,			"twinkle.tga");
	LOAD_COMMON_TEXTURE(textureSpark,			"spark.tga");
	LOAD_COMMON_TEXTURE(textureRing,			"fireworkring.tga");
	LOAD_COMMON_TEXTURE(textureHaar,			"haar.tga");
	LOAD_COMMON_TEXTURE(textureMechaElec,		"mechaelectricity.tga");
	LOAD_COMMON_TEXTURE(textureBadMechaElec,	"mechaelectricitybad.tga");
	LOAD_COMMON_TEXTURE(textureSpin,			"spin.tga");
	LOAD_COMMON_TEXTURE(textureInvisSpin,		"invisspin.tga");
	LOAD_COMMON_TEXTURE(textureTomato,			"tomato.bmp");
	LOAD_COMMON_TEXTURE(textureCucumber,		"cucumber.bmp");
	LOAD_COMMON_TEXTURE(textureCrumb,			"crumb.bmp");
	LOAD_COMMON_TEXTURE(textureGenericGlow,		"generichalo.tga");
	LOAD_COMMON_TEXTURE(textureWaterfallSpray,	"wfallspray.tga");
	LOAD_COMMON_TEXTURE(textureSandwichGlow,	"sandwichhalo.tga");
	LOAD_COMMON_TEXTURE(textureWaterRing,		"waterring.bmp");
	LOAD_COMMON_TEXTURE(textureLoseCostumeGlow,	"losecostumeglow.tga");

	// PP: Pure-colour textures...
	LOAD_COMMON_TEXTURE(textureWhite,			"white.bmp");
	LOAD_COMMON_TEXTURE(textureMagenta,			"blank.bmp");

	// PP: "Mind if I join you chaps?"
	CHECKBOX::initTextures();

	// CMD: lets see wilson, perhaps we should initialise the array and index used for these gifs
	for(int i = 0;i < MAX_MANUAL_GIFS;i++)
	{
		ppTextureAnimateManually[i] = NULL;
	}
	nTextureAnimateManually = 0;

	SpecifyTexturesToAnimateManually("extras\\ballcolour2.gif");
	SpecifyTexturesToAnimateManually("textures\\trafficlights.gif");
	SpecifyTexturesToAnimateManually("crashhelmet_numbers01.gif");			// PP: REMOUT: "extras\\..." (now using 'crashhelmet' package)
	SpecifyTexturesToAnimateManually(ZBBALL_CUTSCENE_EYE_TEXTURE);			// PP: the blinking eyes of the elephant in the Zoo Boss intro
	SpecifyTexturesToAnimateManually(ZBBALL_GAME_EYE_TEXTURE);				// PP: the blinking eyes of the elephant in the Zoo Boss game

	// CMD: scrolling textures
	InitialiseScrollingTexturesArray();
	switch(map.sceneNumber)
	{
	case SCENE_FRONTEND:
	case SCENE_ZOOHUB:
	case SCENE_AQUA:
	case SCENE_SAFARI:
	case SCENE_TAZHUB:
	case SCENE_SAFARIRACE:
		SpecifyScrollingTexture("scrollingtextures\\newwaterfall.bmp", "scrollingtextures\\newwaterfallsmall.bmp", 0.0f, -0.5f);
		break;
	case SCENE_DEPTSTRVEHICLERACE:
	case SCENE_DEPTSTR:
		SpecifyScrollingTexture("scrollingtextures\\newwaterfall.bmp", "scrollingtextures\\newwaterfallsmall.bmp", 0.0f, -0.5f);
		SpecifyScrollingTexture("scrollingtextures\\deptesc.bmp", "scrollingtextures\\deptescsmall.bmp", 0.0f, 2.0f);
		SpecifyScrollingTexture("scrollingtextures\\deptescdown.bmp", "scrollingtextures\\deptescsdownmall.bmp", 0.0f, -2.0f);
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : InitialiseScrollingTexturesArray
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseScrollingTexturesArray(void)
{
	int		i;

	for(i = (MAX_SCROLLTEXTURES-1);i >= 0;--i)
	{
		scrollingTextures[i].dest = NULL;
		scrollingTextures[i].source = NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : InitialiseScrollingTexturesArray
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateScrollingTextures(void)
{
	int	i;

	for(i = (MAX_SCROLLTEXTURES-1);i >= 0;--i)
	{
		if((scrollingTextures[i].dest)&&(scrollingTextures[i].source))
		{
			scrollingTextures[i].coord[0] += scrollingTextures[i].speed[0]*fTime;
			if (scrollingTextures[i].coord[0] > 1.0f) scrollingTextures[i].coord[0] -= bmFloor(scrollingTextures[i].coord[0]);
			if (scrollingTextures[i].coord[0] < 0.0f) scrollingTextures[i].coord[0] += bmFAbs(bmFloor(scrollingTextures[i].coord[0]));


			/*if((scrollingTextures[i].coord[0] += scrollingTextures[i].speed[0]*fTime) > 1.0f) scrollingTextures[i].coord[0] -= 1.0f;
			if(scrollingTextures[i].coord[0] < 0.0f) scrollingTextures[i].coord[0] += 1.0f;*/

			scrollingTextures[i].coord[1] += scrollingTextures[i].speed[1]*fTime;
			if (scrollingTextures[i].coord[1] > 1.0f) scrollingTextures[i].coord[1] -= bmFloor(scrollingTextures[i].coord[1]);
			if (scrollingTextures[i].coord[1] < 0.0f) scrollingTextures[i].coord[1] += bmFAbs(bmFloor(scrollingTextures[i].coord[1]));


			/*if((scrollingTextures[i].coord[1] += scrollingTextures[i].speed[1]*fTime) > 1.0f) scrollingTextures[i].coord[1] -= 1.0f;
			if(scrollingTextures[i].coord[1] < 0.0f) scrollingTextures[i].coord[1] += 1.0f;*/

// NH: Was giving /0 errors when stepping through on PC. Became a bitch to debug
#ifndef NHIGGS
			bdScrollTexture(scrollingTextures[i].source, scrollingTextures[i].dest, scrollingTextures[i].coord[0], scrollingTextures[i].coord[1]);
#endif
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : SpecifyScrollingTexture
   Purpose : 
   Parameters : 
   Returns : TRUE if texture specification succeeds
   Info : 
*/

int SpecifyScrollingTexture(char *source, char *dest, float uSpeed, float vSpeed)
{
	int  i;

	for(i = 0;i < MAX_SCROLLTEXTURES;i++)
	{
		if((scrollingTextures[i].source)||(scrollingTextures[i].dest)) continue;
		
		scrollingTextures[i].source = bkLoadTexture(NULL, source, 0);
		scrollingTextures[i].dest = bkLoadTexture(NULL, dest, 0);
		scrollingTextures[i].speed[0] = uSpeed;
		scrollingTextures[i].speed[1] = vSpeed;
		scrollingTextures[i].coord[0] = 0.0f;
		scrollingTextures[i].coord[1] = 0.0f;
		return TRUE;
	}
	return FALSE;
}