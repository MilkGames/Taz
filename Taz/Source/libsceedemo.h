// SCEE Demo Disk SDK Version 1.0 Pre-release 1

#ifndef _LIBSCEEDEMO_H
#define _LIBSCEEDEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#define SCEE_DEMO_LANGUAGE_ENGLISH  0
#define SCEE_DEMO_LANGUAGE_FRENCH   1
#define SCEE_DEMO_LANGUAGE_GERMAN   2
#define SCEE_DEMO_LANGUAGE_SPANISH  3
#define SCEE_DEMO_LANGUAGE_ITALIAN  4

#define SCEE_DEMO_ASPECT_4_3     0
#define SCEE_DEMO_ASPECT_16_9    1

#define SCEE_DEMO_PLAYMODE_PLAYABLE  0
#define SCEE_DEMO_PLAYMODE_ATTRACT   1

void sceeDemoStart(int            argc,
		   char           **argv,
		   unsigned short *language,
		   unsigned short *aspect,
		   unsigned short *playmode,
		   unsigned short *to_inactive,
		   unsigned short *to_gameplay
		   );

void sceeDemoEnd(void);

#ifdef __cplusplus
}
#endif

#endif _LIBSCEEDEMO_H
