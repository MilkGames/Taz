// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2002 Blitz Games Ltd.
//
//      File : XboxStrings.h
//   Purpose : Xbox versions of some strings
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_XBOXSTRINGS
#define __H_XBOXSTRINGS


#if(BPLATFORM == XBOX)

// PP: NOT "Régler la fonction vibration"			but "Régler le mode Vibration"
#define	STR_ADJUST_VIBRATION						STR_XBOX_ADJUST_VIBRATION

// PP: NOT "@(start) Appuyer sur la touche START"	lost track of this now						but "@(start) Appuyer sur START pour commencer"
#define	STR_PRESS_START_TO_CONTINUE					STR_OLD_XBOX_PRESS_START_TO_CONTINUE		// PP: STR_OLD_XBOX_PRESS_START_TO_CONTINUE

// PP: NOT "Marche"									but "Oui"
#define	STR_FRONTEND_ON 							STR_XBOX_FRONTEND_ON

// PP: NOT "Arrêt"									but "Non"
#define	STR_FRONTEND_OFF							STR_XBOX_FRONTEND_OFF

/* PP: NOT "Für diese Option wird					but "Bitte einen zweiten Controller für diese Option anschließen"			
			ein zweiter Controller benötigt"*/
#define	STR_DEMO_TWOPLAYER_CONTROLLER_MESSAGE		STR_XBOX_DEMO_TWOPLAYER_CONTROLLER_MESSAGE

/* PP: NOT "A Tasmanian Devil can balance			but "A Tasmanian Devil can balance better when tiptoeing.
			better when tiptoeing. Press @(l2)			Press left trigger to tiptoe on the floating log."
			to tiptoe on the floating log.*/
#define STR_ZOOHUB_TILT_HINT						STR_XBOX_ZOOHUB_TILT_HINT

// CMD: some more xbox specific strings

#define STR_MFX_MARQUEE8							STR_XBOX_MFX_MARQUEE8
#define STR_TP_MP_SAFARIBOSS_INTRO					STR_XBOX_TP_MP_SAFARIBOSS_INTRO
#define STR_TUTORIALBOOK1_1							STR_XBOX_TUTORIALBOOK1_1
#define STR_TUTORIALBOOK1_2							STR_XBOX_TUTORIALBOOK1_2
#define STR_TUTORIALBOOK1_3							STR_XBOX_TUTORIALBOOK1_3
#define STR_TUTORIALBOOK2_2							STR_XBOX_TUTORIALBOOK2_2
#define STR_TUTORIALBOOK2_6							STR_XBOX_TUTORIALBOOK2_6
#define STR_TUTORIALBOOK4_2							STR_XBOX_TUTORIALBOOK4_2
#define STR_TUTORIALBOOK4_4							STR_XBOX_TUTORIALBOOK4_4
#define STR_TUTORIALBOOK4_6							STR_XBOX_TUTORIALBOOK4_6
// PP: REMOUT: doesn't get used		#define STR_BOSSRULES_ZOO_BOSS						STR_XBOX_BOSSRULES_ZOO_BOSS
// PP: REMOUT: doesn't get used		#define STR_BOSSRULES_MP_ZOO_BOSS					STR_XBOX_BOSSRULES_MP_ZOO_BOSS
#define STR_TP_MP_SAFARIBOSS_INTRO_1				STR_XBOX_TP_MP_SAFARIBOSS_INTRO_1

#endif// PP: xbox


#endif// PP: ndef __H_XBOXSTRINGS