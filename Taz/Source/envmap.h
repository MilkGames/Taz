#ifndef	__H_ENVMAP
#define	__H_ENVMAP

//  -----------------------------------------------------
//  File: envmap.h
//  -----------------------------------------------------
//  by David Serafim (03/00)
//  
//  (C) ISL 2000
//  -----------------------------------------------------

typedef struct TAG_ENVMAP
{
	TBVector		camera;

	TBPrimVertex envmapverts[6][4];

	TBTexture	*texturelist[6];
	float		scale;
	float		yOff;
} ENVMAP BALIGN16;

// function declarations

/* --------------------------------------------------------------------------------
   Function : SetupEnvMapSpecial
   Purpose : Create and setup an enviroment map using only a focal texture and secondary texture
   Parameters : ENVMAP, scale, texture names
   Returns : TRUE/FALSE
   Info :
*/
int SetupEnvMapSpecial(ENVMAP *envMap, float scale, char *main, char *second, char *third, float angle = 0.0f, float hOffset = 0.0f);

/* --------------------------------------------------------------------------------
   Function : SetupEnvMap
   Purpose : Create and setup an enviroment map
   Parameters : ENVMAP, scale, texture names
   Returns : TRUE/FALSE
   Info :
*/
int SetupEnvMap(ENVMAP *envMap, float scale, char *front, char *back, char *left, char *right, char *top, char *bottom);

/* --------------------------------------------------------------------------------
   Function : PrepareEnvMap
   Purpose : Prepare the map based on the view vector
   Parameters : ENVMAP, view vector
   Returns : 
   Info :
*/
void PrepareEnvMap(ENVMAP *envMap, TBVector camera);
void RenderEnvMap(ENVMAP *envMap);

#endif