#ifndef __H_MOUSE
#define __H_MOUSE

typedef struct TAG_MOUSE
{
	TBVector			mousePos;		// Position of Mouse
	TBVector			mouseB1ClickPos;	// Position mouse was when B1 clicked
	bool				mouseB1;		// Mouse Button 1 down flag
	bool				mouseB1Debounce;// Mouse Button 1 click flag
	bool				mouseB2;		// Mouse Button 2 down flag
	bool				mouseB2Debounce;// Mouse Button 2 click flag
	bool				active;	// NH: Update mouse? (Take in clicks and movement)
	bool				draw;	// NH: If updating should we draw it?
} MOUSE, _MOUSE;

extern MOUSE mouse;

/*	--------------------------------------------------------------------------------
	Function 	: UpdateMousePointer
	Purpose 	: Update the mouse pointer
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdateMousePointer(MOUSE *mousePointer);


/*	--------------------------------------------------------------------------------
	Function 	: DrawMousePointer
	Purpose 	: Draw the mouse pointer
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DrawMousePointer(MOUSE *mousePointer);


/*	--------------------------------------------------------------------------------
	Function 	: InitMouse
	Purpose 	: Initialise The Mouse
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitMouse(MOUSE *mousePointer);

#endif