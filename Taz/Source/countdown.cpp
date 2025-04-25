// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : countdown.cpp
//   Purpose : provide simple countdown timers for things
// Component : Taz
//    Author : C Wilson, C Vorderman and R Whiteley
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"

#include "countdown.h"

CDTIMER countdownTimer[MAX_COUNTDOWNTIMERS];

void cdSetTimer(int timer, float interval, int numtimes)
{
	countdownTimer[timer].numtimes = numtimes;
	countdownTimer[timer].timeInterval = interval;
	countdownTimer[timer].timeTarget = gameStatus.gameTime + interval;
}

void cdKillTimer(int timer)
{
	countdownTimer[timer].numtimes = -1;
	countdownTimer[timer].timeTarget = -1;
}

//If the selected timer has reached it's target return 0
//else return 1

int cdTestTimer(int timer)
{
	float targetTime = countdownTimer[timer].timeTarget;
	float interval = countdownTimer[timer].timeInterval;

	if(gameStatus.gameTime < targetTime)
	{
		return 1;
	}
	else
	{
		float diff;
		
		countdownTimer[timer].numtimes--;

		if(countdownTimer[timer].numtimes < 0)
			return 0;

		diff = gameStatus.gameTime - targetTime;

		if(diff < countdownTimer[timer].timeInterval)
		{
			 countdownTimer[timer].timeTarget += interval;
		}
		else
		{
			int div = (int)((diff / interval) + 1.0f);
			countdownTimer[timer].timeTarget += (interval * div);
		}

		return 1;
	}
}
