#ifndef __H_FONT
#define __H_FONT

/* --------------------------------------------------------------------------------
   Function : LoadFont
   Purpose : load a font
   Parameters :  font name, pak file name
   Returns : TBFont
   Info : 
*/
TBFont *LoadFont(char *fontName,char *pakName);

/* --------------------------------------------------------------------------------
   Function : FreeFont
   Purpose : free a font
   Parameters : TBFont
   Returns : 
   Info : 
*/
void FreeFont(TBFont *font);

/* --------------------------------------------------------------------------------
   Function : LoadStringTable
   Purpose : load a string table
   Parameters : table name, package name
   Returns : TBStringTable
   Info : 
*/
TBStringTable *LoadStringTable(char *tableName, char *pakName);

/* --------------------------------------------------------------------------------
   Function : FreeStringTable
   Purpose : free a string table
   Parameters : TBStringTable
   Returns : 
   Info : 
*/
void FreeStringTable(TBStringTable *table);

/*  --------------------------------------------------------------------------------
	Function	: ChooseCurrentLanguae
	Purpose		: Fill in gameStatus.currentLanguage with the chosen boot langauge
	Parameters	: 
	Returns		: 
	Info		: 
*/

void ChooseCurrentLanguage();


#endif