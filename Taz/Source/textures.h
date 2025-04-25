#ifndef __H_TEXTURES
#define __H_TEXTURES

#define MAX_SCROLLTEXTURES		(10)

// CMD: scroll texture info
typedef struct TAG_TEXSCROLLINFO
{
	TBTexture		*source;
	TBTexture		*dest;
	float			speed[2];
	float			coord[2];
}TEXSCROLLINFO, _TEXSCROLLINFO;

//global texture ptrs
extern TBTexture		*textureWhiteBubble;
extern TBTexture		*textureGreenBubble;
extern TBTexture		*textureSmokeCloud;
extern TBTexture		*textureChilliFire;
extern TBTexture		*textureSleepyZeds;
extern TBTexture		*texturePowerUpGlow;
extern TBTexture		*textureSwirl;
extern TBTexture		*textureSandwich;
extern TBTexture		*textureShadow;
extern TBTexture		*textureFootPrintLeft;
extern TBTexture		*textureFootPrintRight;
extern TBTexture		*textureRain;
extern TBTexture		*textureSnow;
extern TBTexture		*texturePhoneWhite;
extern TBTexture		*texturePhoneOrange;
extern TBTexture		*textureLightning;
extern TBTexture		*textureCrank;
extern TBTexture		*textureWaterRing;
extern TBTexture		*textureWaterSplash;
extern TBTexture		*textureFountain;
extern TBTexture		*waterWibble;
extern TBTexture		*textureTwinkle;
extern TBTexture		*textureSpark;
extern TBTexture		*textureRing;
extern TBTexture		*textureHaar;
extern TBTexture		*textureMechaElec;
extern TBTexture		*textureBadMechaElec;
extern TBTexture		*textureSpin;
extern TBTexture		*textureInvisSpin;
extern TBTexture		*textureTomato;
extern TBTexture		*textureCucumber;
extern TBTexture		*textureCrumb;
extern TBTexture		*textureGenericGlow;
extern TBTexture		*textureWhite;					// PP: 16*16*4bit pure white
extern TBTexture		*textureMagenta;				// PP: 16*16*4bit pure magenta
extern TBTexture		*textureWaterfallSpray;
extern TBTexture		*textureSandwichGlow;
extern TBTexture		*textureLoseCostumeGlow;
/////////////////////////////

/* --------------------------------------------------------------------------------
   Function : LoadTextures
   Purpose : Load textures from a package
   Parameters : package
   Returns : TRUE, FALSE
   Info : 
*/
int LoadTextures(char *package);

/* --------------------------------------------------------------------------------
   Function : FreeTextures
   Purpose : Free all textures loaded from specified package
   Parameters : package
   Returns : 
   Info : 
*/
void FreeTextures(char *package);

/* --------------------------------------------------------------------------------
   Function : ListAnimatingTextures
   Purpose : Create a list of all animating textures
   Parameters : 
   Returns : 
   Info : 
*/
void ListAnimatingTextures(void);

/* --------------------------------------------------------------------------------
   Function : UpdateAnimatingTextures
   Purpose : Update the animating textures
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateAnimatingTextures(void);

/* --------------------------------------------------------------------------------
   Function : SetTextureFrame
   Purpose : set the frame for a texture
   Parameters : texture index, frame number
   Returns : 
   Info : // PP: 
*/
void SetTextureFrame(const int index, const int frame);

/* --------------------------------------------------------------------------------
   Function : FreeAnimatingTextures
   Purpose : Free the animating textures list
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAnimatingTextures(void);


/* --------------------------------------------------------------------------------
   Function : SpecifyTexturesToAnimateManually
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

TBTexture *SpecifyTexturesToAnimateManually(char *sName);

/* --------------------------------------------------------------------------------
   Function : SearchForTexturesToAnimateManually
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int SearchForTexturesToAnimateManually(TBTexture *pTexture);

/* --------------------------------------------------------------------------------
   Function : SetTextureFrameManually
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetTextureFrameManually(TBTexture *pTexture, int f);


/* --------------------------------------------------------------------------------
   Function : InitialiseCommonlyUsedTextures
   Purpose : Initialises global texture ptrs once, to stopr continual crc searching
   Parameters : package
   Returns : TRUE, FALSE
   Info : 
*/

void InitialiseCommonlyUsedTextures(void);

/* --------------------------------------------------------------------------------
   Function : InitialiseScrollingTexturesArray
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseScrollingTexturesArray(void);

/* --------------------------------------------------------------------------------
   Function : InitialiseScrollingTexturesArray
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateScrollingTextures(void);

/* --------------------------------------------------------------------------------
   Function : InitialiseScrollingTexturesArray
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int SpecifyScrollingTexture(char *source, char* dest, float uSpeed, float vSpeed);

#endif