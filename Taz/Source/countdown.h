#define MAX_COUNTDOWNTIMERS (10)

typedef struct CDTIMER
{
	float timeTarget;
	float timeInterval;
	int numtimes;
}CDTIMER;

extern CDTIMER countdownTimer[];


void cdSetTimer(int, float, int);
void cdKillTimer(int);
int cdTestTimer(int);
