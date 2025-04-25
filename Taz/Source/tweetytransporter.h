#ifndef __H_TWEETYTRANSPORTER
#define __H_TWEETYTRANSPORTER

#define	TWEETYTRANSPORTER_ALPHARATE		(2.0f)


/* --------------------------------------------------------------------------------
   Function : UpdateTransientTransporterEffectForTweety
   Purpose : fades effect up/down
   Parameters : target alpha (will most likely be 256 or 0)
   Returns : TRUE if target alpha reached
   Info : 
*/

short UpdateTransientTransporterEffectForTweety(struct TAG_MECHATWEETYINFO *info, short targetAlpha, short currentAlpha);

#endif
