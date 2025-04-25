// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : thwap.h
//   Purpose : handles thwap procedures
//			   Don't know what a thwap is?
//			   Ask yer project manager.
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum THWAPSTAGES
{
	Off,
	Slomo,
	PosterZoom,
	CashZoom,
	Steady,
	Out,
};

void initThwap();
void startThwap(EStringIndex string, int amount);
THWAPSTAGES getThwapStage();
void DoThwap();
void updateThwap();
void drawThwap();
void killThwap();
