#ifndef __H_FLOORPOLISHERWANTEDPOSTER
#define __H_FLOORPOLISHERWANTEDPOSTER


// PP: the y offset used to seperate the floor polisher wanted poster from the ground
// PP: (also used as the minimum ground clearance for the floor polisher trail)
#define FLOORPOLISHERWANTEDPOSTER_DECAL_Y_OFFSET		3.0f// PP: from floorpolisherwantedposter.cpp

#define	WPDECAL_STRIPS	(16)
#define WPDECAL_VERTS	(34)
#define WPDECAL_NEEDED	(0.95)

typedef struct TAG_WANTEDPOSTERDECALINFO
{
	TBPrimVertex		mesh[WPDECAL_STRIPS][WPDECAL_VERTS];			// 16 strips of 32 verts

	TBVector			position;

	TBMatrix			localToWorld;

	TBMatrix			overlayLocalToWorld;

	float				xDim;
	float				yDim;
	float				rotation;					// rotation from -ve z axis
	TBTexture			*texture;

	int					completed;
	struct TAG_BOOK		*bookPtr;
	struct TAG_PAGE		*pagePtr;
	struct TAG_TEXTBOX	*textboxPtr;
} WANTEDPOSTERDECALINFO, _WANTEDPOSTERDECALINFO BALIGN16;

extern WANTEDPOSTERDECALINFO		*floorPolisherWantedPosterInfo;

/* --------------------------------------------------------------------------------
   Function : CreateFloorPolisherWantedPoster
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitFloorPolisherWantedPoster(WANTEDPOSTERDECALINFO **decalInfo);

/* --------------------------------------------------------------------------------
   Function : CreateFloorPolisherWantedPoster
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

WANTEDPOSTERDECALINFO *CreateFloorPolisherWantedPoster(TBVector position, float xDim, float yDim, float rotation);

/* --------------------------------------------------------------------------------
   Function : CreateFloorPolisherWantedPosterMesh
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateFloorPolisherWantedPosterMesh(WANTEDPOSTERDECALINFO *decalInfo);

/* --------------------------------------------------------------------------------
   Function : UpdateFloorPolisherWantedPoster
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateFloorPolisherWantedPoster(WANTEDPOSTERDECALINFO *decalInfo);

/* --------------------------------------------------------------------------------
   Function : DrawFloorPolisherWantedPoster
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawFloorPolisherWantedPoster(WANTEDPOSTERDECALINFO *decalInfo);

/* --------------------------------------------------------------------------------
   Function : FreeFloorPolisherWantedPosterInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeFloorPolisherWantedPosterInfo(WANTEDPOSTERDECALINFO **decalInfo);

#endif