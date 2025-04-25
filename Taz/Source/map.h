#ifndef __H_MAP
#define __H_MAP

#include "splashscreen.h"
#include "linkedlist.h"
#include "collisioncache.h"

extern char						ShowFlyBy;	// NH: Stores if flyby needs showing
extern char						NowShowAPostIt;

enum EMapPointFlags
{
	MAPFLAG_POSTER = 1<<1,
	MAPFLAG_POSTERCOMPLETE = 1<<2,
	MAPFLAG_LEVELCOMPLETE = 1<<3,
};

#define	MAP_MAXPOINTS	(800)

extern int	mapCameraCases[][8];
extern int  mapDemoSubs[];
extern TBRenderTarget			*target;

typedef struct TAG_MAPPOINT
{
	TBVector		position;
	short			mapX,mapY;
	uchar			flags;
	uchar			posterNumber;
} MAPPOINT, _MAPPOINT BALIGN16;

typedef class TAG_MAP
{
private:

	SPLASHSCREEN				map;			// TP: Background map
	TBTexture					*pointer;		// TP: You are here pointer
	TBTexture					*poster;		// TP: Poster icon
	TBTexture					*posterCross;	// TP: Poster complete icon
	TBTexture					*levelComplete;	// TP: Icon showing where the exit point is
	MAPPOINT					pointList[MAP_MAXPOINTS];// TP: List of points on level and there associated points on 2d map
	float						offset;			// TP: Current offset of arrow
	TBVector					arrowPos;		// TP: current position of arrow
	MAPPOINT					*lastPoint;		// TP: last point used
	float						arrowAng;		// TP: Current angle of pointer
	float						postAng;		// JW: Current angle of selected Poster
	float						phase;			// TP: Phase value used in the rotation of the flag
	float						postPhase[8];		// JW: Phase value used in rotation of selected Poster
	float						levCompletePhase;// TP: Phase value for level complete icon
	int							selected;		// JW: Number of poster selected by player
	int							numPosters;		// JW: Number of total posters in map
	int							availablePosters;	// JW: Number of available posters in map
	int							numPoints;		// TP: number of points in array

	/*  --------------------------------------------------------------------------------
		Function	: MAP::Init
		Purpose		: Create and setup the map system
		Parameters	: 
		Returns		: TRUE/FALSE
		Info		: 
	*/
	int Init();

	/*  --------------------------------------------------------------------------------
		Function	: MAP::FindClosestPoint
		Purpose		: Find the closest map point on level
		Parameters	: 
		Returns		: MAPPOINT
		Info		: 
	*/
	MAPPOINT *FindClosestPoint(TBVector pos);

public:
	/*  --------------------------------------------------------------------------------
		Function	: MAP::TAG_MAP
		Purpose		: Default constructor
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	TAG_MAP()
	{
		Init();
	}


	/*  --------------------------------------------------------------------------------
		Function	: MAP::LoadMapSplash
		Purpose		: Load the map splash screen
		Parameters	: 
		Returns		: 
		Info		: 
	*/
#if BPLATFORM==GAMECUBE
	// IH: Why is there code in a header file??
	void LoadMapSplash();
#else
	void LoadMapSplash()
	{
		char	mapName[256];

		sprintf(mapName, "maps\\%s.bmp", ::map.mapName);

		// NH: "extras\\" removed as it's defined in splashscreen.cpp so different systems res's can be handled
		LoadSplashScreen(&map, mapName, "splshscr");
	}
#endif
	/*  --------------------------------------------------------------------------------
		Function	: MAP::~TAG_MAP
		Purpose		: Default destructor
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	~TAG_MAP();

	void Update(void);

	/*  --------------------------------------------------------------------------------
		Function	: MAP::Draw
		Purpose		: Draw the map splash screen along with the pointer
		Parameters	: Current position of object to represent on the map
		Returns		: 
		Info		: 
	*/
	void Draw(TBVector	pos);

	/*  --------------------------------------------------------------------------------
		Function	: MAP::AddPoint
		Purpose		: Adds a map marker to the current
		Parameters	: Current position of object to represent on the map
		Returns		: 
		Info		: 
	*/
	void AddPoint(TBVector position, short mapX, short mapY, uchar flag = 0, uchar posterNumber = 0);

	/*  --------------------------------------------------------------------------------
		Function	: MAP::PosterCompleted
		Purpose		: Mark the poster on the map as completed
		Parameters	: poster number
		Returns		: 
		Info		: 
	*/
	void PosterCompleted(int number);

	void GetMapPositionOfCurrentPoster(TBVector2D result);


} MAP, _MAP BALIGN16;

/*  --------------------------------------------------------------------------------
	Function	: DrawPolaroid
	Purpose		: like it sounds
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DrawPolaroid(void);

/*  --------------------------------------------------------------------------------
	Function	: UpdateInGameCameraCase
	Purpose		: Update the map camera case
	Parameters	: 
	Returns		: 
	Info		: 
*/

void UpdateInGameCameraCase();

/*  --------------------------------------------------------------------------------
	Function	: StartIngameCameraCase
	Purpose		: Play the map camera case
	Parameters	: camera number
	Returns		: 
	Info		: 
*/

void StartIngameCameraCase(int cameraNum);



#endif