#ifndef __H_LANGUAGEMAIN
#define __H_LANGUAGEMAIN

#include "global.h"
#include "main.h"
#include "player.h"
#include "files.h"
#include "control.h"
#include "status.h"
#include "display.h"
#include "shadow.h"
#include "multiplayer.h"
#include "tweety.h"
#include "animation.h"


// PP: the languages supported by Taz.
// PP: this defines the ordering of any arrays in which each element corresponds to one of out supported languages,
// PP: eg. the languageSuffixes array.
enum supportedLanguages
{
	TAZLANG_UK,
	TAZLANG_F,
	TAZLANG_D,
	TAZLANG_E,
	TAZLANG_IT,

	NUM_SUPPORTED_LANGUAGES
};

// PP: Macro to convert a Babel language index into the index of one of our five supported languages.
// PP: uses English if _lang isn't supported
#define BLANG_TO_TAZLANG(_blang) \
		((_blang)==BLANGUAGEID_UK	?TAZLANG_UK: \
		((_blang)==BLANGUAGEID_F	?TAZLANG_F: \
		((_blang)==BLANGUAGEID_D	?TAZLANG_D: \
		((_blang)==BLANGUAGEID_E	?TAZLANG_E: \
		((_blang)==BLANGUAGEID_IT	?TAZLANG_IT: \
									TAZLANG_UK)))))

// PP: suffixes used on localised versions of filenames.
// PP: yes I do know about Gizmo's language options, but that's not always the best approach (see button glyphs).
// PP: and yes, the plural of 'suffix' IS 'suffixes'.
extern const char* languageSuffixes[NUM_SUPPORTED_LANGUAGES];// PP: languagemain.cpp

// PP: Macro to convert a Babel language index into a flag number for the language select screen.
// PP: uses English if _blang isn't supported
#define BLANG_TO_FLAGNUM(_blang) \
		((_blang)==BLANGUAGEID_UK	?0: \
		((_blang)==BLANGUAGEID_E	?1: \
		((_blang)==BLANGUAGEID_F	?2: \
		((_blang)==BLANGUAGEID_D	?3: \
		((_blang)==BLANGUAGEID_IT	?4: \
									0)))))

extern char skipLanguage;

// PP: when this is true, we'll skip the main game intro cutscene.
// PP: it'll be set true when we've been to the language select from the frontend
extern bool skipMainGameIntroCutscene;

int InitialiseLanguageSelect(int startFlag = 0, char skip = skipLanguage);
int PrepLanguageSelect();
int DrawLanguageSelect();
int FreeLanguageSelect();
int UpdateLanguageSelect();
void ChooseLanguagePage();

void SwitchLanguage();

#endif// PP: ndef __H_LANGUAGEMAIN
