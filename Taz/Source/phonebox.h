#ifndef __H_PHONEBOX
#define __H_PHONEBOX

enum EBoxType
{
	BOXTYPE_PHONE,
	BOXTYPE_POLICE,
};

typedef struct TAG_PHONEBOXINFO
{
	ACTORINSTANCE		*actorInstance;		// pointer to draw actor
	ACTORINSTANCE		*collisionInstance;	// pointer to collision actor
	ACTORINSTANCE		*occupier;				// character using phone box
	int					state;					// current state of phone box
	struct TAG_PHONEBOXEFFECTINFO	*effectInfo;
	ACTORINSTANCE		*box;					// TP: box for phone box

	float				time;
	char				type;					// TP: type of box, phone or police
	char				level[32];				// TP: level name to change to if police box
} PHONEBOXINFO, _PHONEBOXINFO BALIGN16;

void CreatePhoneBoxBook();
void UpdatePhoneBoxBook();
// PP: REMOUT: Now handled by the Book List			void DrawPhoneBoxBook();
void KillPhoneBoxBook();

/* --------------------------------------------------------------------------------
   Function : CreateActorInstancePhoneBox
   Purpose : Called to create the phone box
   Parameters : ACTORINSTANCE *actorInstance, ACTORINSTANCE *collisionInstance
   Returns : nothing
   Info : 
*/
int CreateActorInstancePhoneBox(ACTORINSTANCE *actorInstance,ACTORINSTANCE *collisionInstance,int type,char *level=NULL);

/* --------------------------------------------------------------------------------
   Function : ActivatePhoneBoxes
   Purpose : Called when the player has collected 10 taz tops in quick succession
   Parameters : ACTORINSTANCE *actorInstance
   Returns : nothing
   Info : 
*/
void ActivatePhoneBoxes(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: EnterSceneUsingPoliceBox
	Purpose		: If player enters the scene from one of the bonus games
	Parameters	: actorInstance, lastScene
	Returns		: 
	Info		: 
*/
bool EnterSceneUsingPoliceBox(ACTORINSTANCE *actorInstance, int lastScene);

/* --------------------------------------------------------------------------------
   Function : RespondToPhoneBoxCollision
   Purpose : Called when the player touches a phone box
   Parameters : ACTORINSTANCE *actorInstance
   Returns : nothing
   Info : 
*/
int RespondToPhoneBoxCollision(ACTORINSTANCE *collider, ACTORINSTANCE *phoneBox);

/* --------------------------------------------------------------------------------
   Function : UpdatePhoneBoxes
   Purpose : Update all phone boxes on list
   Parameters : ACTORINSTANCELIST *actorInstanceList
   Returns : nothing
   Info : 
*/
void UpdatePhoneBoxes(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : UpdatePhoneBox
   Purpose : Update phone box
   Parameters : ACTORINSTANCE *actorInstance
   Returns : nothing
   Info : 
*/
void UpdatePhoneBox(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: IsPoliceBoxAccessible
	Purpose		: 
	Parameters	: level name that police box is trying to enter
	Returns		: true if ok to enter
	Info		: 
*/

bool IsPoliceBoxAccessible(char *level);

#endif