#ifndef __H_EVENTS
#define __H_EVENTS

enum
{
	TRIGGERFLAG_CONTAINSWANTED = 1<<0,
	TRIGGERFLAG_CONTAINSSOUND = 1<<1,
};

typedef struct TAG_TRIGGERINFO
{
	int		numMessages,maxMessages;
	char	flags;
	char	**events;
	char	**messages;
} TRIGGERINFO;

extern int32	showHubStats;
extern int32	levelNameStrings[];
extern bool		openLevels;	// TP: level roaming feature is disabled by default
extern TBEventClient	*soundEventClient;


/*  --------------------------------------------------------------------------------
	Function	: AllowedToEnterLevel
	Purpose		: Returns true/false if the player is allowed to enter the specified level
	Parameters	: level
	Returns		: true/false
	Info		: 
*/
bool AllowedToEnterLevel(int level);

/*	--------------------------------------------------------------------------------
	Function 	: CreateEventManagers
	Purpose 	: Setup event managers
	Parameters 	: 
	Returns 	: TRUE/FALSE
	Info 		:
*/
int CreateEventManagers(void);

/*	--------------------------------------------------------------------------------
	Function 	: DeleteEventManagers
	Purpose 	: Delete event managers
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DeleteEventManagers(void);

/*	--------------------------------------------------------------------------------
	Function 	: CreateTriggerInfo
	Purpose 	: Malloc trigger info for maxMessages
	Parameters 	: maximum number of messages
	Returns 	: triggerInfo/NULL
	Info 		:
*/
struct TAG_TRIGGERINFO *CreateTriggerInfo(int maxMessages);

struct TAG_TRIGGERINFO *CopyTriggerInfo(struct TAG_TRIGGERINFO *trigger);

/*	--------------------------------------------------------------------------------
	Function 	: CreateTrigger
	Purpose 	: Create trigger message to trigger info
	Parameters 	: event, message
	Returns 	: 
	Info 		:
*/
int CreateTrigger(struct TAG_TRIGGERINFO *triggerInfo,char *event,char *message,bool appendUnderscore=true);

/*	--------------------------------------------------------------------------------
	Function 	: AddTrigger
	Purpose 	: Add trigger message to trigger info
	Parameters 	: event, message
	Returns 	: 
	Info 		:
*/
int AddTrigger(struct TAG_TRIGGERINFO *triggerInfo,char *event,char *message);

/*	--------------------------------------------------------------------------------
	Function 	: DeleteTriggerInfo
	Purpose 	: Delete all trigger info
	Parameters 	: triggerInfo
	Returns 	: 
	Info 		:
*/
void DeleteTriggerInfo(struct TAG_TRIGGERINFO *triggerInfo);

/*	--------------------------------------------------------------------------------
	Function 	: SendTrigger
	Purpose 	: Send trigger
	Parameters 	: event, message, optional sound position
	Returns 	: true if a sound message was found, else false
	Info 		:
*/
bool SendTrigger(struct TAG_TRIGGERINFO *triggerInfo, TBVector position = NULL);

/*	--------------------------------------------------------------------------------
	Function 	: RespondToSceneChangeMessages
	Purpose 	: Respond to sound messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToFootstepMessages(void);

/*	--------------------------------------------------------------------------------
	Function 	: CreateEventHandlers
	Purpose 	: Setup event handlers
	Parameters 	: 
	Returns 	: TRUE/FALSE
	Info 		:
*/
int CreateEventHandlers(void);

/*	--------------------------------------------------------------------------------
	Function 	: DeleteEventHandlers
	Purpose 	: Delete event handlers
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DeleteEventHandlers(void);

/*	--------------------------------------------------------------------------------
	Function 	: RespondToSceneChangeMessages
	Purpose 	: Respond to sound messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToSceneChangeMessages(void);

/*	--------------------------------------------------------------------------------
	Function 	: RespondToTriggerMessages
	Purpose 	: Respond to sound messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RespondToTriggerMessages(void);

/*	--------------------------------------------------------------------------------
	Function 	: ProcessTriggerForSpecialTypes
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void ProcessTriggerForSpecialTypes(char *messageType, char *message);

/*	--------------------------------------------------------------------------------
	Function 	: RespondToCameraMessages
	Purpose 	: Respond to camera messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToCameraMessages(void);

/*	--------------------------------------------------------------------------------
	Function 	: RespondToPlatformMessages
	Purpose 	: Respond to platform messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToPlatformMessages(void);

/*	--------------------------------------------------------------------------------
	Function 	: RespondToAnimationMessages
	Purpose 	: Respond to animation messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToAnimationMessages(void);

/*	--------------------------------------------------------------------------------
	Function 	: RespondToSpecialMessages
	Purpose 	: Respond to special messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToSpecialMessages(void);

/*	--------------------------------------------------------------------------------
	Function 	: RespondToSpecialMessages
	Purpose 	: Respond to special messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToMinecartMessages(void);

/*  --------------------------------------------------------------------------------
	Function	: EnterHubUpdate
	Purpose		: custom pause for when exiting an ingame level
	Parameters	: context
	Returns		: 
	Info		: 
*/
// PP: REMOUT: Now handled by the Book List			void EnterHubUpdate(const int32 context);

/*  --------------------------------------------------------------------------------
	Function	: FindLevel
	Purpose		: Find a level number using a string
	Parameters	: 
	Returns		: 
	Info		: 
*/
int	FindLevel(char *levelName);

/*  --------------------------------------------------------------------------------
	Function	: DecodeMessage
	Purpose		: Returns the message and babel header info if available
	Parameters	: 
	Returns		: 
	Info		: 
*/

char *DecodeMessage(char *string, TBActorInstance **actorInstance, TBActorNodeInstance **actorInstanceNode);

/*	--------------------------------------------------------------------------------
	Function 	: RespondToSpecialMessages
	Purpose 	: Respond to special messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RespondToRumbleMessages(void);

/*	--------------------------------------------------------------------------------
	Function 	: RespondToPromptMessages
	Purpose 	: Respond to special messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RespondToPromptMessages(void);

#endif