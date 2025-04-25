#ifndef __H_CARTOONSCALING
#define __H_CARTOONSCALING


enum 
{
	CARTOONSCALE_STAGEONE,
	CARTOONSCALE_STAGETWO,
	CARTOONSCALE_STAGETHREE,
	CARTOONSCALEDOWN_STAGEONE,
	CARTOONSCALEDOWN_STAGETWO,
	CARTOONSCALEDOWN_STAGETHREE,
};

enum 
{
	SCALETYPE_UP,
	SCALETYPE_DOWN,
};

typedef struct TAG_CARTOONSCALEINFO
{
	uint32			stage;
	float			time;
	int				type;
	float			maxScale;
} CARTOONSCALEINFO, _CARTTONSCALEINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : updateCartoonScaling
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void UpdateCartoonScaling(ACTORINSTANCE *actor, CARTOONSCALEINFO *cartoonScaleInfo);

/* --------------------------------------------------------------------------------
   Function : CreateCartoonScalingInfo
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

CARTOONSCALEINFO *CreateCartoonScalingInfo(float maxScale=2.0f);

#endif