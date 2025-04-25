#include "languagemain.h"

#define	FL_GO_LEFT	(0)
#define FL_GO_RIGHT	(1)
#define	FL_GO_UP	(2)
#define FL_GO_DOWN	(3)

enum LS_USA_FLAGS
{
	FL_USA_ENGLISH,
	FL_USA_SPANISH,
	FL_USA_FRENCH,
	FL_USA_GERMAN,
	FL_USA_ITALIAN,

	NUM_FL_USA_ENTRIES,
};

typedef struct TAG_LANGUAGESELECTINFO
{	
	ACTORINSTANCE	*Path;
	
	int				currentFlag;
	int				nextFlag;
	int				targetFlag;

	TWEETY			*Tweety;

	char			SelectMade;
}LANGUAGESELECTINFO, _LANGUAGESELECTINFO BALIGN16;

extern LANGUAGESELECTINFO	*LanguageSelectInfo;

extern TBVector				*vFlagPos;

int InitialiseLanguageAnimationPaths(void);
void ChoosePathAnimationToPlay();
void ChooseAnimationForPodium(ACTORINSTANCE *Podium);
void ChooseSylvesterAnimationToPlay(ACTORINSTANCE  *Sylvester);
void ResetAnimPathToStart(ACTORINSTANCE *actorInstance, char *animName);
void ResetAnimPathToEnd(ACTORINSTANCE *actorInstance, char *animName);

void UpdateSylvesterPosition(ACTORINSTANCE *Sylvester);
void UpdatePathAnimation(ACTORINSTANCE *Sylvester);
void SylvesterLookAtTweety(ACTORINSTANCE *Sylvester, TBVector Tweety);

TBActorAnimSegment * ChooseCatchAnimForSylvester(ACTORINSTANCE *actorInstance);

enum LS_USA_FLAGS GetNextFlag(int current, int direction);
