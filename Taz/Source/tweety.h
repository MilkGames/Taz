#ifndef __H_TWEETY
#define __H_TWEETY

enum ETweetyState
{
	TWEETYSTATE_HOVER,
	TWEETYSTATE_CIRCLE,
	TWEETYSTATE_FOLLOW,
	TWEETYSTATE_SHORTLAUGH,
	TWEETYSTATE_LAUGH,
	TWEETYSTATE_NULL,
};

typedef struct TAG_TWEETY
{
	ACTORINSTANCE	*actorInstance;		// TP: tweety actorInstance
	ETweetyState	state;				// TP: current state of tweety

	TBVector		destPosition;		// TP: Position tweety is currently heading towards
	TBQuaternion	destQuat;			// TP: Destination quat for tweety

	ACTORINSTANCE	*aoi;				// TP: Current actor of interest for tweety
	TBVector		poi;				// TP: Current point of interest for tweety

	float			timeInState;		// TP: Amount of time tweety has been in current state
	char			rotDir;				// TP: Current direction of rotation for circle mode
	TBVector		vel;				// TP: Current velocity

} TWEETY, _TWEETY BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateTweety
   Purpose : Create, initialise and return a tweety
   Parameters : start position
   Returns : 
   Info : tweety.obe must be loaded before this function is called
*/
void CreateTweety(TBVector start,TBQuaternion quat);

/*  --------------------------------------------------------------------------------
	Function	: ResetTweetyPosition
	Purpose		: Reset tweety's position next to Taz
	Parameters	: position, quat
	Returns		: 
	Info		: 
*/
void ResetTweetyPosition(TBVector position, TBQuaternion quat);

/* --------------------------------------------------------------------------------
   Function : UpdateTweetyLangauge
   Purpose : Update tweeties AI etc.
   Parameters : time of update
   Returns : TWEETY, ftime
   Info : 
*/

TWEETY * UpdateTweetyLanguage(float fTime);

/* --------------------------------------------------------------------------------
   Function : UpdateTweetyNormal
   Purpose : Update tweeties AI etc.
   Parameters : time of update
   Returns : TWEETY, ftime
   Info : 
*/

int UpdateTweetyNormal(float fTime);

/* --------------------------------------------------------------------------------
   Function : DestroyTweety
   Purpose : Shutdown and free TWEETY
   Parameters : 
   Returns : 
   Info : 
*/
void DestroyTweety(void);

void SetTweetyDest(TBVector dest);

void SetTweetyAOI(ACTORINSTANCE *actor);

void TeleportTweety(TBVector);

void TweetyLaugh();

void StartTweetyIntro();

void DoTweetyIntro();

/* --------------------------------------------------------------------------------
   Function : DrawTweety
   Purpose : Draw tweety in front of all objects
   Parameters : 
   Returns : 
   Info : 
*/
void DrawTweety(void);

#endif