#ifndef __H_FAMETHROWA
#define __H_FAMETHROWA

typedef struct TAG_FLAMETHROWAINFO
{
	float	length;
	float	period;
	float	clock;
	int32	iPad;
} FLAMETHROWAINFO, _FLAMETHROWAINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateFlameThrowa
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateFlameThrowa(struct TAG_ACTORINSTANCE *actorInstance, float length, float period);

/* --------------------------------------------------------------------------------
   Function : UpdateFlameThrowa
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateFlameThrowa(struct TAG_ACTORINSTANCE *actorInstance);

#endif