// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Blitz Games Ltd.
//
//      File : diner.cpp
//   Purpose : multiplayer diner subgame
// Component : Taz
//    Author : Chris W
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "textfx.h"

#include "display.h"
#include "maths.h"
#include "actors.h"
#include "status.h"
#include "physics.h"
#include "camera.h"
#include "trail.h"
#include "scene.h"


#include "multiplayer.h"

int dinerHandler(int reason)
{
	switch(reason)
	{
	case MPREASON_INITIALISE:
		break;
	case MPREASON_PREP:
		break;
	case MPREASON_POLL:
		break;
	case MPREASON_DRAW:
		break;
	case MPREASON_KILL:
		break;
	}

	return(0);
}
