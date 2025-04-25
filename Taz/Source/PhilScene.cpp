// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : philscene.cpp
//   Purpose : custom scene information
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "physics.h"
#include "animation.h"
#include "collisioncache.h"
#include "mapfile.h"
#include "status.h"
#include "paths.h"
#include "genericai.h"
#include "camera.h"
#include "control.h"
#include "maths.h"
#include "characters.h"
#include "shadow.h"
#include "scene.h"
#include "trigger.h"
#include "destructibles.h"
#include "water.h"
#include "projectile.h"
#include "springyobjects.h"
#include "mailbox.h"
#include "wantedposter.h"
#include "fallingobjects.h"
#include "main.h"
#include "lights.h"
#include "cutscene.h"
#include "envmap.h"
#include "sfx.h"
#include "textures.h"
#include "swingingobjects.h"
#include "playerstats.h"
#include "tonyscene.h"
#include "claudescene.h"
#include "precipitation.h"
#include "thoughtbubble.h"
#include "collectibles.h"
#include "debris.h"
#include "tiltingobjects.h"
#include "steppingstone.h"
#include "gui.h"
#include "philscene.h"
#include "zookeeper.h"
#include "bears.h"
#include "cameracase.h"
#include "alligator.h"
#include "phonebox.h"
#include "securitybox.h"
#include "files.h"
#include "map.h"
#include "memcard.h"
#include "explode.h"
#include "icon.h"			// PP: Icon effects used by the front end
#include "display.h"		// PP: display stuff
#include "Fireball.h"		// PP: Particle emitters used for explosion effects, eg. by WestBossExp
#include "util.h"			// PP: general utility functions
#include "Spinner.h"		// PP: Spinner control page-item (lets you specify a numerical value)
#include "Table.h"			// PP: Table page-item type - lets you line up items into a grid arrangement
#include "HealthBar.h"		// PP: 'Health Bar' display for player characters, where it is needed
#include "VideoFX.h"		// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "Frontendmenu.h"
#include "ZooBoss.h"
#include "WestBoss.h"
#include "CityBoss.h"
#include "objectviewer.h"
#include "paperbook.h"
#include "PageItemFX.h"	
#include "pause.h"			// PP: pause
#include "Fireball.h"		// PP: Particle emitters used for explosion effects, eg. by WestBossExp

#define PHILSCENE_TEST_TEXT		STR_ICEDOME_INTROCAM2

int g_soundChannel=0;

//#ifndef INFOGRAMES
//#define TEST_BOOK
//#endif// PP: !infogrames	

// PP: choose lottery numbers
// PP: 
//#define LOTTERY

// PP: test sound falloff, etc.
//#define TEST_SOUND

#if((defined LOTTERY)||(defined TEST_SOUND))
#define TEST_BOOK
#endif// PP: def LOTTERY

// PP: tweak map control note positions
// PP: WARNING!: IF YOU LEAVE THIS DEFINED, YOU'll BE SHITFTING THE CONTROL NOTES ABOUT WITHOUT KNOWING IT!!!
//#define CONTROLNOTEPOS

extern BOOK *thwapBook;
extern TEXTBOX	*legendBox;
extern PAPERBOOK	*tutorialBook;					// PP: paper book containing tutorials - each tutorial is a different chapter

TEXTBOX*			flashBox;

// PP: ************************

#ifdef MAYBE_LATER

#define ENUM(_enum) \
struct _enum \
{ \
\
private: \
\
	enum \

#define ENUMEND \
	val; \
\
public: \
\
	inline operator const int32() const \
	{ \
		return int32(this->val); \
	} \
\
	inline operator int32() \
	{ \
		return int32(this->val); \
	} \
};

ENUM(myEnum)
{
	ME_ONE,
	ME_TWO,

	NUM_MYENUM_VALS
}
ENUMEND

void tFunc(const myEnum num)
{
	if(num==1);
}

#endif// PP: def MAYBE_LATER

// PP: ************************




// PP: this is my master test book - it's going to contain an example of absolutely everything you can do with a book,
// PP: so's I can test it all works
static BOOK				testBook;
static BOOK				dofBook;	// PP: book for testing DOF
static float			testFontSize=1.0f;
static bool				testBookDrawDebug=true;		// PP: drawDebug the test book
float					testBookWidthPortion=1.0f;
float					testBookHeightPortion=1.0f;
						
extern FRONTENDMENU		frontEndMenu;
						
// PP: TEST				
bool					frameBlur=false;
						
float					g_testFOV=NORMAL_INGAME_FOV;
						
float					testShadowScale=2.35f;

VEC						soundPos=ZVEC;

/*
char* blendOpNames[]=
{
	"
}
*/
// PP: badly named - the item that we have a grip on and whose justification we can alter using the d-pad
//static PAGEITEM*	selectedItem=NULL;



/*	--------------------------------------------------------------------------------
	Function : gimmeFaceAngle
	Purpose : gimme face angle, maybe show face
	Parameters :
	Returns : 
	Info : // PP: QUICK DISPOSABLE TEST FUNCTION, MESSY I DONT CARE
*/
bool gimmeFaceAngle(COLLISIONLISTENTRY *meshList, TBVector start, TBVector end, TBVector intersect, BOUNDINGBOX *testBox)
{
	bool					hit=false;				// PP: did we hit any of the triangles?
	float					minTime;				// PP: the min time (distance) to a collision
	TBCollisionRay			ray;					// PP: collision ray between start & end points
	TBCollisionTriangle		tri;					// PP: collision triangle
	TBCollisionInfo			info;					// PP: collision info
	int						fc = meshList->numPolys;
	COLLPOLY				*poly;
	VEC						closestAng;
	VEC						closestPos;

	VEC						closestVerts[4]={ZVEC,ZVEC,ZVEC,ZVEC};

	// PP: SET UP COLLISION RAY
	bmVectorCopy(ray.start, start);
	ray.start[W]=1.0f;
	bmVectorSub(ray.unitVector, end, start);// PP: doesn't need to be unit
	ray.unitVector[W]=1.0f;
	ray.length=0.0f;// PP: ignored

	info.flags=(BMCOLLINFO_RETURNPOSITION|BMCOLLINFO_RETURNTIME|BMCOLLINFO_RETURNNORMAL);

	// IH: Are we using the small collision stuff??
#ifdef USE_SHORTS_FOR_COLLISION
	short		posMin[3],posMax[3];

	if (testBox)
	{
		posMin[X]=(((short)testBox->xmin)>>3)-1;
		posMin[Y]=(((short)testBox->ymin)>>3)-1;
		posMin[Z]=(((short)testBox->zmin)>>3)-1;

		posMax[X]=(((short)testBox->xmax)>>3)+1;
		posMax[Y]=(((short)testBox->ymax)>>3)+1;
		posMax[Z]=(((short)testBox->zmax)>>3)+1;
	}

	for (fc = 0; fc < meshList->numPolys;fc++)
	{
		// bounding box collision
		if (testBox)
		{
			poly = &meshList->polys[fc];
			if ((posMax[X] < poly->minx)||(posMax[Z] < poly->minz)||(posMax[Y] < poly->miny))
			{
				continue;
			}

			// TP: check min bbox test passes
			if ((posMin[X] > poly->maxx)||(posMin[Z] > poly->maxz)||(posMin[Y] > poly->maxy))
			{
				continue;
			}
		}

#else

	for (fc = 0; fc < meshList->numPolys;fc++)
	{
		// bounding box collision
		if (testBox)
		{
			poly = &meshList->polys[fc];
			if ((testBox->xmax < poly->minx)||(testBox->zmax < poly->minz)||(testBox->ymax < poly->miny))
			{
				continue;
			}

			// TP: check min bbox test passes
			if ((testBox->xmin > poly->maxx)||(testBox->zmin > poly->maxz)||(testBox->ymin > poly->maxy))
			{
				continue;
			}
		}
#endif
		// PP: SET UP COLLISION TRIANGLE
		COLLPOLY_GETV1(meshList->polys[fc], meshList->vertices, tri.verts[0][X], tri.verts[0][Y], tri.verts[0][Z], tri.verts[0][W]);
		COLLPOLY_GETV2(meshList->polys[fc], meshList->vertices, tri.verts[1][X], tri.verts[1][Y], tri.verts[1][Z], tri.verts[1][W]);
		COLLPOLY_GETV3(meshList->polys[fc], meshList->vertices, tri.verts[2][X], tri.verts[2][Y], tri.verts[2][Z], tri.verts[2][W]);

		// PP: PERFORM COLLISION TEST
		if(bmCollideRayTriangle(&ray, &tri, &info))
		{
			if(!hit)
			{
				// PP: first time we've hit something - this is therefore the closest hit so far
				minTime=info.time;
				hit=true;
			}
			else
			{
				if(info.time < minTime)
				{
					// PP: this is the closest hit so far
					minTime=info.time;

					// PP: this is the closest angle so far

					closestAng=(-VEC(info.normal)).ang();

					// PP: closest pos

					closestPos=info.position;

					for(int i=0;i<3;i++)
					{
						closestVerts[i]=tri.verts[i];
					}
				}
			}

			// PP: fill in intersect point
			bmVectorCopy(intersect, info.position);
		}
	}

	if(hit)
	{
		bkPrintf("gimmeFaceAngle: closestAng is %f, %f, %f\n", closestAng[X], closestAng[Y], closestAng[Z]);

		// PP: aight draw summit at ix pos
		utilDrawSphere(closestPos);

		// PP: get centre pos
		closestVerts[3]=lerp(closestVerts[0], closestVerts[1]);

		bkPrintf("gimmeFaceAngle: centrePos is %f, %f, %f\n", closestVerts[3][X], closestVerts[3][Y], closestVerts[3][Z]);

		for(int r=0; r<4; r++)
		{
			bdDrawFlatSprite(
					closestVerts[r],
					20.0f,
					20.0f,
					0.0f,
					QwilsonTexture,
					FALSE,
					FALSE,
					(bmFMod(gameStatus.appTime, 0.2f)>0.1f)?255:90,
					255,
					0,
					255,
					NULL,
					0.0f
				);
		}
	}

	return hit;
}

/* --------------------------------------------------------------------------------
   Function : myRayTest
   Purpose : // PP: QUICK DISPOSABLE TEST FUNCTION, MESSY I DONT CARE
   Parameters : // PP: QUICK DISPOSABLE TEST FUNCTION, MESSY I DONT CARE
   Returns : // PP: QUICK DISPOSABLE TEST FUNCTION, MESSY I DONT CARE
   Info : // PP: QUICK DISPOSABLE TEST FUNCTION, MESSY I DONT CARE
*/
bool myRayTest(void)
{
	COLLISIONLISTENTRY	*meshLists[MAX_COLL_MESHES];
	int					numberOfMeshes=0;
	int					i;
	TBVector			testPos,tempVector;
	BOUNDINGBOX			actorBox;
	COLLISIONLISTENTRY	*ptr;
	BOOL				value;
	TBVector			intersect;								// PP: point of intersection with the meshes, below test point
	bool				retVal=false;

//	STARTTIMER;

	VEC	start, end;

	start=gameStatus.viewPoint;
	end=start+(VECM(0,0,100).rotate(gameStatus.viewAng));

	actorBox.xmin = start[X] < end[X]?start[X]:end[X];
	actorBox.xmax = start[X] > end[X]?start[X]:end[X];
	actorBox.ymin = start[Y] < end[Y]?start[Y]:end[Y];
	actorBox.ymax = start[Y] > end[Y]?start[Y]:end[Y];
	actorBox.zmin = start[Z] < end[Z]?start[Z]:end[Z];
	actorBox.zmax = start[Z] > end[Z]?start[Z]:end[Z];

	meshLists[numberOfMeshes] = NULL;
	for (ptr = collisionCache.collisionList.head.next;ptr != &collisionCache.collisionList.head;ptr = ptr->next)
	{
		// test if in or going to enter bounding box
		if ((!ptr->actorInstance)||(ptr->actorInstance->instanceList)&&(ptr->actorInstance->instanceList!=&map.triggerInstances)&&(ptr->actorInstance->instanceList!=&map.invisibleInstances))
		{
			if ((numberOfMeshes<MAX_COLL_MESHES)&&TestBoxCollision(&actorBox,&ptr->boundingBox))
			{
				if (MakeEntryResident(&collisionCache,ptr)==TRUE)
				{
					// resident so add to collision process
					meshLists[numberOfMeshes] = ptr;
					numberOfMeshes++;
				}
				else
				{
#if BPLATFORM==GAMECUBE
					static int onceOnly = 1;
					if (onceOnly)
					{
						bkPrintf("*** WARNING *** Could not make entry resident\n");
						onceOnly = 0;
					}
#else
					bkPrintf("*** WARNING *** Could not make entry resident\n");
#endif
				}
			}
		}
	}

	SETPROJECTION(3D);
	bdSetIdentityObjectMatrix();

	for (i=0;i<numberOfMeshes;i++)
	{
		if(gimmeFaceAngle(meshLists[i], start, end, intersect, &actorBox))
		{
			retVal = true;
			break;
		}
	}

//	ENDTIMER("Ray test");
	return retVal;
}


/*	--------------------------------------------------------------------------------
	Function 	: testText
	Purpose 	: yadda
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void testText(void)
{
	float						textRect[4];

	static float				width=400.0f;
	static float				height=400.0f;
	static EBFontFormatting		hFormat=EBFONTFORMATTING_LEFT;
	static EBFontFormatting		vFormat=EBFONTFORMATTING_TOP;
	static float				lineSpace=0.0f;
	static float				scale=0.7f;

	static float				tweakSpeed=70.0f;

	width += controller1.x2*fTime*tweakSpeed;
	height += controller1.y2*fTime*tweakSpeed;

	width=MAX(10, width);
	height=MAX(10, height);

	ushort*						string=mainStringTable->strings[STR_XBOX_DEMO_DEPTSTORE_TROLLEYOBJECTIVE1].string;

	TBFontFormatting			format;
	RECTANGLE					formatRect;

	format.x=-250;
	format.y=250.0f;
	format.width=width;
	format.height=height;
	format.horzFormat=hFormat;
	format.vertFormat=vFormat;
	format.lineSpace=lineSpace;
	format.wrap=true;

	formatRect.left=format.x;
	formatRect.right=format.x+format.width;
	formatRect.top=format.y;
	formatRect.bottom=format.y-format.height;


	// PP: calculate required dimensions of text box
	int rtn=bdFontFormattedRect(
		&textRect[0],
		standardFont,
		string,
		bkStringLength16(string),
		&format,
		NULL,
		NULL);

	TBMatrix	scaleMat;

	bmMatScale(scaleMat, scale, scale, scale);

	SETPROJECTION(2D);
	bdSetObjectMatrix(scaleMat);
	SETZWRITE(FALSE);
	SETZTEST(ALWAYS);



	// PP: draw the required rect

	RECTANGLE	reqRect;

	reqRect.left=textRect[0];
	reqRect.right=textRect[2];
	reqRect.bottom=textRect[1];
	reqRect.top=textRect[3];

	reqRect.draw(COLOUR(255,0,0,100), ALPHAFILL);

	format.y=reqRect.height()*0.5f;
	formatRect.top=format.y;

	// PP: draw the provided rect
	formatRect.draw(COLOUR(0,255,255,128), OUTLINE);

	// PP: now print the text and see if it fits

	bdPrintFont(standardFont, string, bkStringLength16(string),&format,0,0,0,128,/*callback*/NULL,NULL);

	// PP: ~~~~~~~~~~~~~~~~~~~~~

	// PP: now give the text exactly the dimensions it asked for
/*
	format.width=reqRect.width();
	format.height=reqRect.height();

	// PP: calculate required dimensions of text box
	rtn=bdFontFormattedRect(
		&textRect[0],
		standardFont,
		string,
		bkStringLength16(string),
		&format,
		NULL,
		NULL);

	reqRect.left=textRect[0];
	reqRect.right=textRect[2];
	reqRect.bottom=textRect[1];
	reqRect.top=textRect[3];

	reqRect.draw(COLOUR(255,0,0,100), OUTLINE);
*/
	format.width=reqRect.width();
	format.height=reqRect.height();

	// PP: print again, in white
	bdPrintFont(standardFont, string, bkStringLength16(string),&format,128,128,128,128,/*callback*/NULL,NULL);
}


ICON*					dq2=NULL;

/*	--------------------------------------------------------------------------------
	Function 	: testMenuActionFunc
	Purpose 	: yadda
	Parameters 	: ref to a MENUACTION_INFO structure (TextFX.h)
	Returns 	: 
	Info 		: must match the MenuHandlerFunc typedef (TextFX.h)
*/
void testMenuActionFunc(MENUACTION_INFO& info)
{
	switch(info.action)
	{
	case MENUACTION_SELECT:
		{
			bkPrintf("%s selected\n", info.item->name);
		}
		break;

	case MENUACTION_DESELECT:
		{
			bkPrintf("%s de-selected\n", info.item->name);
		}
		break;

	case MENUACTION_PRESS:
		{
			/* PP: REMOUT: DEMO CODE	if(item == "myButton")
			{
				myButtonFunc();
			}*/

			bkPrintf("%s pressed\n", info.item->name);

			if(info.item == dq2)
			{
				bkPrintf("***** dq2 pointer match ******\n");
			}
		}
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: InitialisePhilGame
	Purpose 	: // PP: Initialise stuff for me at the beginning of the game
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

float g_testFloat=69.0f;
int g_testInt=69;
TEXTBOX*	fpsBox;
TEXTBOX*	fontSizeBox;

#ifdef LOTTERY
TEXTBOX*	lotteryBoxes[6];
#endif// PP: def LOTTERY


void InitialisePhilGame(void)
{
//	return;

/*	ActivateCheat(CHEAT_OPEN_BOSS_GAME);
	ActivateCheat(CHEAT_OPEN_BONUS_GAMES);*/

	//ActivateCheat(CHEAT_SUPERSLOWMO);

/*	AwardCheat(CHEAT_COLOURSTEREOSCOPIC);
	AwardCheat(CHEAT_TURBO);
	AwardCheat(CHEAT_SHADOWMAP);*/

// PP: TEMP TEST

/*	TEXTBOX		testTB;
	testTB.sprintf8(" ");*/

	// PP: XBOX DEMO TEST

/*	DEMO_LAUNCH_DATA demoLaunchData;

	XGetLaunchData(&demoLaunchData);*/

	// PP: NEW TEST!!!!
// PP: REMOUT: TEMP TEST	bSoundInfo.support3DSound = FALSE;

	// PP: I thought I'd leave this in for a while to see if we get used to it
//	ActivateCheat(CHEAT_SHADOWMAP);

//	ActivateCheat(CHEAT_TAZCAM);
//	ActivateCheat(CHEAT_COLOURSTEREOSCOPIC);

	{
	// PP: @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// PP: @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// PP: @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	// PP: TEMP TEST
/*	extern BOOK*	frigBook;

	frigBook=new BOOK(true);

	// NH: 0.48 aligns it properly with the pause.
	frigBook->setBookRect(RECTANGLE(-0.48f, 0.45f, -0.45f, 0.48f));

	PAGE*		mainpage, *page;
	TEXTBOX		*spareText;

	mainpage = frigBook->addPage("stats");

	// TP: TODO:	Alternate stats for inaccesable level

	mainpage->insertFreeItem(page = new PAGE("title"), RECTANGLE(-0.4f, 0.4f, 0.35f, 0.5f));

	// NH: Add Title
	page->insertItem(spareText = new TEXTBOX(STR_LEVELNAME_WESTWORLD), false);
	spareText->setStyle(TEXTSTYLE_TITLE)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	mainpage->insertFreeItem(page = new PAGE("boss info"), RECTANGLE(-0.3f, 0.3f, -0.3f, 0.2f));

	page->insertItem(spareText = new TEXTBOX(STR_HUB_BOSSGAMESTAT));
	spareText->setWrap(true);

	frigBook->setScale(0.0f);*/

	// PP: @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// PP: @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// PP: @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	}


//#if 0

#ifdef TEST_BOOK

	PAGE*		page;
	PAGE*		subPage;
	PAGE*		subPage2;
	ICON*		icon;
	TABLE*		table;
	TEXTBOX*	textBox;
	BOOK*		subBook;

	// PP: SET UP THE TEST BOOK
	testBook.setName("TESTBOOK");

	// PP: NEW TEST
	testBook.init();

	//testBook.setBookRect(RECTANGLE(-0.5f, 0.5f, 0.0f, 0.5f));

#ifdef LOTTERY

	page=testBook.addPage();

	page->insertItem(new TEXTBOX)->textbox()
		->setFontSize(1.5f)
		->sprintf8("YOUR LUCKY NUMBERS ARE:")
		->setYAlign(PIFLAG_YALIGN_BOTTOM);

	for(int j=0;j<6;j++)
	{
		page->insertItem(lotteryBoxes[j]=new TEXTBOX, j==0)->textbox()
			->setFontSize(2.0f)
			->setColour(CYAN, BLUE);
	}

#endif// PP: def LOTTERY

	page=testBook.addPage();
	{
		TEXTBOX*	text;
		PAGE*		subPage;

		page->insertItem(subPage=new PAGE);
		
		subPage->insertItem(fontSizeBox = text = new TEXTBOX(PHILSCENE_TEST_TEXT));// PP: REMOUT: OOD	,TEXTFX_DEFAULT_COLOUR1,TEXTFX_DEFAULT_COLOUR2,TBFLAG_DEFAULTS,TRUE));
		
		text->setWrap(true);
		text->setEffect(&PIE_fade);// PP: these subtitles don't seem to suit any fancy effects; just use a bog-standard fade
					
		// PP: I've made the subtitle boxes size themselves according to what's in them - let me know what you think
		// PP: This stops text spilling out, and I also think it's a bit nicer to look at
					
		// PP: align to bottom of container
		subPage->setYAlign(PIFLAG_YALIGN_BOTTOM);
					
		// PP: no height slack please
		subPage->setVSlack(false);
				
		// PP: Add a bit of breathing space round the text
		subPage->setMargin(0.05f);//(0.025f);

		// PP: specify that the page's outline box will be loose on the X but tight on the Y
		subPage->setBoxStyle(false, true);
					
		// PP: set page's background and border
		subPage->setBackgroundColour(COLOUR(0,0,0,75));
		subPage->setBorderColour(COLOUR(0,0,0,255));

		/*(fontSizeBox=page->insertItem(PHILSCENE_TEST_TEXT)->textbox())
			->setWrap(true)
			->setBackgroundColour(COLOUR(0,0,80,70))
			->setBorderColour(COLOUR(255, 255, 0, 255))
			->setBoxStyle(false, true);*/

		// PP: ~~~~~~~~~~~~

		page->insertItem(new SPINNER(SPINNERTYPE_UFLOAT, (void*)(&testFontSize), 0.07f, "FONT"), true);
	}

	page=testBook.addPage();
	{
		// PP: IMPORTANT: DON'T EVER CONFUSE 'select' AND 'selectItem' !!!
		page->insertItem(flashBox=new TEXTBOX(PHILSCENE_TEST_TEXT), true)->textbox()->setWrap(true);

//		page->insertItem(STR_MEMCARD_MEDIAFULL_CHOOSE);
//		page->insertItem(STR_MEMCARD_MEDIAFULL_CONFIRM);
		page->insertItem(STR_MEMCARD_MEDIAFULL_OPTS)->setBackgroundColour(COLOUR(255,0,0,70));
		page->insertItem(new ICON("qwilson.bmp"), false)->setBackgroundColour(COLOUR(255,0,0,70));
		page->insertItem(STR_MEMCARD_MEDIAFULL_CHOOSE_OPTS)->setBackgroundColour(COLOUR(0,255,0,70));
		page->insertItem(new ICON("qwilson.bmp"), false)->setBackgroundColour(COLOUR(0,255,0,70));
		page->insertItem(STR_MEMCARD_MEDIAFULL_CONFIRM_OPTS)->setBackgroundColour(COLOUR(0,0,255,70));
		page->insertItem(new ICON("qwilson.bmp"), false)->setBackgroundColour(COLOUR(0,0,255,70));
	}

	page=testBook.addPage();
	{
		page->insertItem(subBook=new BOOK(true));
		subPage=subBook->addPage();
		{
			subPage->insertItem(new ICON("qwilson.bmp"));
			subPage->setBackgroundColour(COLOUR(0,0,255,70));
			subPage->setBoxStyle(true, false);
		}

		page->insertItem(subBook=new BOOK(true), false);
		subPage=subBook->addPage();
		{
			subPage->insertItem(new ICON("qwilson.bmp"));
			subPage->setBackgroundColour(COLOUR(255,0,0,70));
			subPage->setBoxStyle(false, true);
		}

		page->setBackgroundColour(COLOUR(128,128,128,70));
	}

	page=testBook.addPage();
	{
	/*	page->insertFreeItem(table=new TABLE("TABLE"), RECTANGLE(-0.5f, 0.0f, -0.5f, 0.0f));

		for(int i=0; i<5; i++)
		{
			for(int j=0; j<5; j++)
			{
				table->insertItem(new TEXTBOX("X"), !j);
			}
		}

		table->setBackgroundColour(COLOUR(128,128,128,64));*/

		page->insertFreeItem(subPage=new PAGE(), UORECT.leftHalf());
		{
			subPage->insertItem(icon=new ICON("qwilson.bmp"));
			icon->setXAlign(PIFLAG_XALIGN_LEFT);
			icon->setYAlign(PIFLAG_YALIGN_TOP);
			icon->setXPadding(false);

			subPage->insertItem(fpsBox=new TEXTBOX(), false)
				->setXAlign(PIFLAG_XALIGN_LEFT)
				->setYAlign(PIFLAG_YALIGN_TOP);
		}
	}

	page=testBook.addPage();
	{
		page->insertItem(textBox=new TEXTBOX("MIN"));
		//textBox->setColour(RED);
		textBox->setSelectable(false);

		page->insertItem(new SPINNER(SPINNERTYPE_UFLOAT, (void*)(&SFX_minDist), 30.0f, "MIN"), false);

		page->insertItem(textBox=new TEXTBOX("MAX"));
		textBox->setColour(GREEN);
		textBox->setSelectable(false);

		page->insertItem(new SPINNER(SPINNERTYPE_UFLOAT, (void*)(&SFX_maxDist), 30.0f, "MAX"), false);
	}

	page=testBook.addPage();
	{
		page->insertItem(new ICON("qwilson.bmp"))
			->setXPadding(false)
			->setXAlign(PIFLAG_XALIGN_LEFT)
			->setYAlign(PIFLAG_YALIGN_TOP)
			->setName("QWILSONLEFT");

		textBox=(TEXTBOX*)(page->insertItem(new TEXTBOX()/*STR_DIALOGUE_YES*/, false))
			->setScale(2.5f)
			->setXAlign(PIFLAG_XALIGN_LEFT)
			->setYAlign(PIFLAG_YALIGN_TOP);
		textBox->sprintf8("?");
		textBox->setName("?LEFT");
			
		textBox=(TEXTBOX*)(page->insertItem(new TEXTBOX()/*STR_DIALOGUE_YES*/, false))
			->setScale(2.5f)
			->setXAlign(PIFLAG_XALIGN_RIGHT)
			->setYAlign(PIFLAG_YALIGN_TOP);
		textBox->sprintf8("?");
		textBox->setName("?RIGHT");

		page->insertItem(new ICON("qwilson.bmp"), false)
			->setXPadding(false)
			->setXAlign(PIFLAG_XALIGN_RIGHT)
			->setYAlign(PIFLAG_YALIGN_TOP)
			->setName("QWILSONRIGHT");

		page->insertItem(PHILSCENE_TEST_TEXT, true)->setFontSize(0.75f)->setWrap(true)->setName("LEFTTEXT")->setAlignScale(1.2f);
		page->insertItem(PHILSCENE_TEST_TEXT, false)->setFontSize(0.75f)->setWrap(true)->setName("CENTRETEXT")->setAlignScale(1.2f);
		page->insertItem(PHILSCENE_TEST_TEXT, false)->setFontSize(0.75f)->setWrap(true)->setName("RIGHTTEXT")->setAlignScale(1.2f);
/*
		page->insertItem(subBook=new BOOK(true), true);

		{
			subPage=subBook->addPage("SUBBOOKPAGE");
			subPage->insertItem(new ICON("qwilson.bmp"));
			subPage->insertItem(new ICON("qwilson.bmp"), false);
		}

  		//subBook->setXAlign(PIFLAG_XALIGN_LEFT);

/*/
		page->insertItem(subPage=new PAGE("DUAL-QWILSON SUB-PAGE"));

		{
			subPage->insertItem(new ICON("qwilson.bmp"))->setName("DUALQWILSON1");
			subPage->insertItem(dq2=new ICON("qwilson.bmp"), false)->setName("DUALQWILSON2");
		}

		subPage->setSlack(false);

		// PP: NEW TEST
		//page->getLastRow()->flags |= ROWFLAG_UNIFORM_X_SPACING;
		page->getLastRow()->setName("DUAL-QWILSON SUB-PAGE ROW");

		//*/

		//subPage->setXPadding(false);
		//subPage->setYPadding(false);

		//subPage->setXAlign(PIFLAG_XALIGN_RIGHT);
		//subPage->setYAlign(PIFLAG_YALIGN_TOP);

	//	page->insertItem(new ICON("qwilson.bmp"), false);
	}

	page->setMenuActionFunc(testMenuActionFunc);

	page=testBook.addPage("BLENDPAGE");
	{
		page->insertItem(new TEXTBOX())->setName("BLENDBOX")->setYAlign(PIFLAG_YALIGN_TOP);
	}

	page=testBook.addPage();
	{
		

		// PP: first row...

		page->insertItem(new ICON("qwilson.bmp"));
		page->insertItem(new ICON("qwilson.bmp"), false);
		page->insertItem(new ICON("qwilson.bmp"), false);
		page->insertItem(new ICON("qwilson.bmp"), false);

		// PP: second row...

		page->insertItem(PHILSCENE_TEST_TEXT, true)->setWrap(true);

		// PP: third row...

		page->insertItem(new ICON("qwilson.bmp"), true);
		page->insertItem(new ICON("qwilson.bmp"), false);
		page->insertItem(new ICON("qwilson.bmp"), false);
		page->insertItem(new ICON("qwilson.bmp"), false);
	}

	page=testBook.addPage();
	{
		// PP: free item

		PAGE*	freePage;

		page->insertFreeItem(new ICON("qwilson.bmp"), RECTANGLE(-0.5f, 0.f, 0.f, 0.5f));// PP: RECTANGLE(-0.5f, -0.25f, -0.5f, -0.25f));

		page->insertFreeItem(new TEXTBOX("FREE ITEMS"), RECTANGLE(-0.5f, 0.f, -0.5f, 0.f));// PP: RECTANGLE(0.5f, 0.25f, 0.5f, 0.25f));

		page->insertFreeItem(freePage=new PAGE("freepage"), RECTANGLE(0.f, 0.5f, -0.5f, 0.5f))
			->setBackgroundColour(COLOUR(255, 255, 255, 80));
		{
		/*	freePage->insertItem(new ICON("qwilson.bmp"));
			freePage->insertItem(new TEXTBOX("HELLO"), false);*/
		}
	}

	testBook.setNavigation(true);

#endif// PP: def TEST_BOOK

#ifdef INFOGRAMES
	return;
#endif// PP: def INFOGRAMES

//#endif// PP: 0

	// PP: TEMP
//	testBook.open();


	// PP: Hello Chris ;o)
	//ActivateCheat(CHEAT_QWILSON);
	
	// PP: I thought I'd leave this in for a while to see if we get used to it
	//ActivateCheat(CHEAT_SHADOWMAP);

#if(BPLATFORM == XBOX)// PP: ps2 don't seem to like that lighting state
	// PP: might as well nurture this one through its infancy too
//	ActivateCheat(CHEAT_AIRBRUSH);
#endif// PP: #if(BPLATFORM == XBOX)

	//ActivateCheat(CHEAT_TURBO);
	//ActivateCheat(CHEAT_SUPERSLOWMO);

	// PP: If I can't sleep, why the hell should he?
	ActivateCheat(CHEAT_INSOMNIA);

	ActivateCheat(CHEAT_DISABLESECURITYBOXES);
	ActivateCheat(CHEAT_OPEN_BONUS_GAMES);
	ActivateCheat(CHEAT_OPEN_GALLERY);
	ActivateCheat(CHEAT_OPEN_BOSS_GAME);

	//ActivateCheat(CHEAT_INVINCIBLE);

//	ActivateCheat(CHEAT_OPENLEVELS);

	// PP: *********************** DOF BOOK
/*
	page=dofBook.addPage();

	page->insertItem(textBox=new TEXTBOX())->setName("NEAR");

	textBox->setXAlign(PIFLAG_XALIGN_LEFT)->setYAlign(PIFLAG_YALIGN_BOTTOM)->setScale(0.5f);

	page->insertItem(textBox=new TEXTBOX(), false)->setName("FAR");;

	textBox->setXAlign(PIFLAG_XALIGN_RIGHT)->setYAlign(PIFLAG_YALIGN_BOTTOM)->setScale(0.5f);

	dofBook.open();*/
}


/*	--------------------------------------------------------------------------------
	Function 	: InitialisePhilScene
	Purpose 	: // PP: initialise stuff for me at the start of a level
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void initThwap(void);

void InitialisePhilScene(void)
{
	ActivateCheat(CHEAT_DISABLESECURITYBOXES);
	ActivateCheat(CHEAT_OPEN_GALLERY);
	ActivateCheat(CHEAT_OPEN_BOSS_GAME);
	ActivateCheat(CHEAT_OPEN_BONUS_GAMES);
	//ActivateCheat(CHEAT_TURBO);

	{
		// PP: TEMP - for debug - so's I can bypass the frontend
	/*	bindControllersToDefaultPorts();
		padCheckMode=PADCHECK_NORMAL;*/
	}

	//ActivateCheat(CHEAT_SHOWDEBUGINFO);

	return;

	//ActivateCheat(CHEAT_COLOURSTEREOSCOPIC);

/*

	ActivateCheat(CHEAT_DISABLESECURITYBOXES);
	ActivateCheat(CHEAT_INVINCIBLE);
	ActivateCheat(CHEAT_COSTUME);
	
	ActivateCheat(CHEAT_DIGITALWATCH);*/

/*	initThwap();

	legendBox->sprintf8("hello there");

	thwapBook->gotoPage("legend page", true);*/

	/*SetHearingRange(METERTOUNIT(1000));
	bsSet3DDistanceFactor(UNITTOMETER(1));*/

	//SetHearingRange(METERTOUNIT(1000));
//	SetHearingRange(INFINTE_HEARING_RANGE);
}

 
void PrepPhilScene(int scene)
{
}



/*	--------------------------------------------------------------------------------
	Function 	: TestMatMul
	Purpose 	: yadda
	Parameters 	: yadda
	Returns 	: yadda
	Info 		: yadda
*/
#if 0
void TestMatMul(void)
{
	TBPrimVertex		verts[4];			// PP: prim vertices with which to form the box we're going to draw
	TBMatrix			testMatrix;			// PP: test matrix to set as object matrix in first test, and to multiply the box's corner points in the second test
	TBVector			tempVector;			// PP: temporary vector
	int					i;					// PP: loop counter

	TBMatrix			scaleMat;
// PP: REMOUT: NOT USED		TBMatrix			rotMat;
	TBMatrix			transMat;

	// PP: set up the test object matrix

	bmMatTranslate(transMat, 150, 100, 0);
	
	bmMatScale(scaleMat, 10000.0f, 10000.0f, 1.0f);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 0);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 1);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 1);
	bdSetTexture(0, NULL);

	// PP: set up the test object matrix
	bmMatMultiply(testMatrix, transMat, scaleMat);

	// PP: ***** TEST 1 : DRAW THE BOX UNDER OUR TEST OBJECT MATRIX ******

	// PP: set up the box...

#define BOXWIDTH		0.01f

	for(i=0;i<4;i++)
	{
		BDVERTEX_SETXYZW(&verts[i], ((i&1)?1:(-1))*BOXWIDTH, ((i/2)?1:(-1))*BOXWIDTH, 0.0f, 1.0f);
		BDVERTEX_SETRGBA(&verts[i], 0, 255, 0, 255);
	}


	// PP: draw the box under the test object matrix
	bdSetObjectMatrix(testMatrix);
	bdDrawPrimitive(BDPRIMTYPE_LINESTRIP,verts,4,BVERTTYPE_SINGLE);

	// PP: ***** TEST 2 : MULTIPLY THE BOX'S CORNER POINTS BY THE TEST OBJECT MATRIX, THEN DRAW IT UNDER AN IDENTITY OBJECT MATRIX *****

	// PP: multiply each of the box's corner points by the test object matrix...

	for(i=0;i<4;i++)
	{
		BDVERTEX_GETXYZW(&verts[i], tempVector[X], tempVector[Y], tempVector[Z], tempVector[W]);

		bmMatMultiplyVector(testMatrix, tempVector);

		BDVERTEX_SETXYZW(&verts[i], tempVector[X], tempVector[Y], tempVector[Z], 0.0f);
		BDVERTEX_SETRGBA(&verts[i], 255, 0, 0, 255);
	}

	// PP: draw the transformed box under an identity matrix
	bdSetIdentityObjectMatrix();
	bdDrawPrimitive(BDPRIMTYPE_LINESTRIP,verts,4,BVERTTYPE_SINGLE);
}
#endif 0


/*	--------------------------------------------------------------------------------
	Function 	: DrawPhilScene
	Purpose 	: Draws all actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/

void DrawPhilScene(int scene,CAMERAINFO *cameraInfo)
{
//	return;

	// PP: SOUND TEST

#ifdef TEST_SOUND

	//if(testBook.flags & (BKFLAG_OPEN))
	{
		bdSetIdentityObjectMatrix();

		//soundPos=gameStatus.player1->actorInstance.position;

		utilDrawSphere(soundPos, 100, WHITE);
		utilDrawSphere(soundPos, SFX_minDist, RED);
 		utilDrawSphere(soundPos, SFX_maxDist, GREEN);

		util3DText(soundPos, "%5.2f", soundPos.distance(gameStatus.viewPoint));
	}

#endif// PP: def TEST_SOUND

	// PP: TEMP TEST
	 //myRayTest();

	// PP: ************ FOV TEST

/*	if(gameStatus.player1 != NULL)
	{

		bdSetIdentityObjectMatrix();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

		TBVector spritePos;

		bmVectorCopy(spritePos, gameStatus.player1->actorInstance.position);
		spritePos[Y]+=METERTOUNIT(0.9f);

		float ang=bmFMod(gameStatus.appTime, TWOPI);

		if(bdDrawFlatSprite(spritePos,
							METERTOUNIT(0.5f),
							METERTOUNIT(0.5f),
							ang,
							bkLoadTexture(NULL, "fovtest.bmp",0),
							0,
							0,
							128,
							128,
							128,
							128,
							NULL,
							0.0f))
		{
		}
	}*/



/*	SETPROJECTION(3D);
	bdSetIdentityObjectMatrix();
	SETZWRITE(FALSE);
	SETZTEST(ALWAYS);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);

	utilDrawLine(tazCam.pos, tazCam.lookAt);*/

	// PP: TEMP TEST!!!!!
//	testText();

#ifdef INFOGRAMES
//	return;
#endif// PP: def INFOGRAMES

	// PP: SPLITSCREEN ASPECT RATIO TEST
/*
#define RECTSIZE	80.0f

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,FALSE,FALSE);

	RECTANGLE(-RECTSIZE, RECTSIZE, -RECTSIZE, RECTSIZE).draw(BLUE, OUTLINE|ALPHAFILL);
*/



/*	bdSetIdentityObjectMatrix();
	utilDrawSphere(soundPos, METERTOUNIT(1), CYAN);
	utilDrawArrow(soundPos, VEC(cartoonLightVec).ang());*/



/*	bdSetIdentityObjectMatrix();
	utilDrawSphere(soundPos, METERTOUNIT(1), GREEN);*/

	// PP: TEMP TEST
//	TestMatMul();

	return;
		
	// PP: ************ BOOK TEST

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetIdentityObjectMatrix();

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);// PP: WAS FALSE. MAYBE SHOULD BE, FOR FAKING VOLUME SHADOWS
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	// PP: REMOUT: RENDER STATE DOESN@T SEEM TO EXIST NOW HMM		bdSetRenderState(BDRENDERSTATE_CLIPPING,FALSE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	// PP: draw the test book
// PP: REMOUT: Now Handled by the Book List			testBook.draw();

/*	if(testBookDrawDebug)
	{
	//	if(testBook.flags&(BKFLAG_OPEN))
		{
			// PP: debug-draw the test book
			testBook.drawDebug();
		}
	}*/


	// PP: draw the dof book
//	dofBook.draw();


	// PP: ******* BLEND TEST *************
/*
#if((BPLATFORM == XBOX)||(BPLATFORM == PC))

	dispContext->d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);//add
	dispContext->d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);//add

#endif// PP: platform
*/
/*
	TBVector	spritePos;

	bmVectorZero(spritePos);

#define PULSE_PERIOD	4.0f

	int		alpha=((bmSin((fmod(gameStatus.appTime, PULSE_PERIOD)/PULSE_PERIOD)*TWOPI)/2.0f)+0.5f)*255;

	TBTexture*	tex=bkLoadTexture(NULL, "testcols.bmp", 0);

	int		max=10;

#if((BPLATFORM == XBOX)||(BPLATFORM == PC))

		D3DDevice_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); 
		D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);


#endif// PP: platform

	for(int i=0;i<max;i++)
	{
		utilDraw2DSprite(tex, spritePos, 350, 0,TWOPI, COLOUR(87,87,87, 100));
	}

//	bkPrintf("%d\n", alpha);

	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);// PP: WAS FALSE. MAYBE SHOULD BE, FOR FAKING VOLUME SHADOWS
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);*/

}


bool qsfunc(uchar* const i, uchar* const j)
{
	return ( (*((int32*)i)) > (*((int32*)j)) );
}


/*	--------------------------------------------------------------------------------
	Function 	: testSliderupdateFunc
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void testSliderupdateFunc(const float sliderValue, struct TAG_SLIDER* const slider)
{

}


void testVECs(void)
{
	TBVector	tbv;
	VEC			vec;

	//tbv zero
	bmVectorZero(tbv);

	//vec zero
	vec=ZVEC;

	//tbv set
	SETVECTOR(tbv, 0,1,2,1);

	//vec set
	vec=VEC(0,1,2);

	//tbv cmul
	bmVectorCMul(tbv, tbv, 2.0f);

	//vec cmul
	vec*=2.0f;
}


void testDashboardLaunch(void)
{

}


#ifdef LOTTERY
void chooseLotteryNumbers(void)
{
	static uchar	lotteryNumbers[6]={0,0,0,0,0,0};
	static bool		lotteryNumbersChosen[6]={0,0,0,0,0,0};
	static uchar	numLotteryNumbersChosen=0;
	int				i;

	for(i=0; i<6; i++)
	{
		if(!lotteryNumbersChosen[i])
		{
			lotteryNumbers[i]=bmFloatToInt(bmFloor((bmRandf()*49.0f)+1.0f));
		}

		lotteryBoxes[i]->sprintf8("%d", lotteryNumbers[i]);
	}

	if(controller1.squareDebounceChannel->value)
	{
		if(numLotteryNumbersChosen < 6)
		{
			lotteryNumbersChosen[numLotteryNumbersChosen++]=true;
		}
	}
}
#endif// PP: def LOTTERY


/*	--------------------------------------------------------------------------------
	Function 	: UpdatePhilScene
	Purpose 	: Update scene animations and status etc.
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/

uchar	vol=255;

extern PAPERBOOK	*RuleBook;

void UpdatePhilScene(int scene) 
{
	// PP: TEMP TEST!!!
	//MemCard_update()

	{
		static int port=1;
	//	biFFSetParm(port, BIFEEDBACK_XBOXBOTHMOTORS, BXBOX_MOTORSPEED*controller1.r1, BXBOX_MOTORSPEED*controller1.r1);
	}

	if(controller1.l1DebounceChannel->value)
	{
		/*extern int32	videoReturnCodes[];
		videoReturnCodes[6]=bdVideoPlayStream("d:\\infoident_tazhi.wmv",0);

		// PP: TEMP TEST!!!!
		extern void initMissingControllerBooks(void);
		initMissingControllerBooks();*/

		//badDisk();

		toggleFlyCam();
	}

	// PP: TEMP BODGE!!! I FORGOT TO DEFINE INFOGRAMES
	if((controller2.selectDebounceChannel->value != 0)
		||(controller1.selectDebounceChannel->value != 0))
	{
					biZeroMap(controller1.inputMap);
			UpdateGuiItems();
			bgEnterGui(GuiCallback);
	}

	//return;

	// PP: TEMP TEST
/*	if(controller1.selectDebounceChannel->value != 0)
	{
		EnterPauseMode(PAUSE_MAP);
	}
*/


	// PP: TEMP TEST!!!!
	/*if(controller1.l1DebounceChannel->value)
	{
		int h=0;
		int j=1/h;

		bkPrintf("got here\n");
	}*/

#ifdef CONTROLNOTEPOS

	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PP: tweak map control note positions
	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	extern float controlNotePos[][2];

#define ADJUSTSPEED		140.0f

	controlNotePos[map.sceneNumber][X]+=controller1.x2*ADJUSTSPEED*fTime;
	controlNotePos[map.sceneNumber][Y]-=controller1.y2*ADJUSTSPEED*fTime;

	if(controller1.l1DebounceChannel->value)
	{
		bkPrintf("\n\nfloat controlNotePos[][2] = \n");

		bkPrintf("{\n");
		for(int i=0; i<19; i++)
		{
			bkPrintf("\t{%5.3ff,\t\t%5.3ff},\n", controlNotePos[i][X]/videoMode.xScreen, controlNotePos[i][Y]/videoMode.yScreen);
		}
		bkPrintf("};\n\n");
	}
		
#endif// PP: def CONTROLNOTEPOS

	// PP: allow lottery numbers to be picked
#ifdef LOTTERY

	chooseLotteryNumbers();

#endif// PP: def LOTTERY

	/*if(controller1.l1DebounceChannel->value != 0)
	{
		ActivateCheat(CHEAT_TIMEFREEZE);

		// PP: bsSetGlobalAudioStreamVolume(vol);
	}*/

#ifdef TEST_BOOK

	// PP: open book
	if(controller1.triangleDebounceChannel->value)
	{
		testBook.toggleOpenClose();
	}

/*	if(controller1.squareDebounceChannel->value)
	{
		flashBox->startEffect(PIE_flash.copy()->setItem(flashBox)->setMode(PIEMODE_ON));
	}*/

/*	if(controller1.squareDebounceChannel->value)
	{
		testBook.realign();
		//	RuleBook->realign();
	}*/

#endif// PP: defTEST_BOOK


#ifdef TEST_BOOK

	if(controller2.l1DebounceChannel->value)
	{
		testBook.nextPage()->selectItem();
	}
	else if(controller2.l2DebounceChannel->value)
	{
		testBook.prevPage()->selectItem();
	}

	if((testBook.flags & BKFLAG_OPEN) && testBook.getSelectedItem())
	{
		testFontSize=MAX(0.1f, testFontSize);
		bkPrintf("setting font size to %5.2f\n", testFontSize);
		fontSizeBox->setFontSize(testFontSize);
		testBook.realign();

		if(controller2.r3DebounceChannel->value)
		{
			// PP: RESET

			if(controller2.r1Channel->value)
			{
				testBook.getSelectedItem()->setDrawScale(1.0f);
				//testBook.getSelectedItem()->textbox()->setFontSize(1.0f);
			}
			
			if(controller2.r2Channel->value)
			{
				testBook.getSelectedItem()->setAlignScale(1.0f);
				testBook.realign();
			}

			if(!(controller2.r1Channel->value || controller2.r2Channel->value))
			{
				testBook.getSelectedItem()->setXAlign(PIFLAG_XALIGN_CENTRE);
				testBook.getSelectedItem()->setYAlign(PIFLAG_YALIGN_CENTRE);
				testBook.realign();
			}
		}
		else
		{
#define SCALE_ADJUST_STEP			(-1.0f*(1/(float)BIMAX)*fTime)

			if(testBook.getSelectedItem() != NULL)
			{
				if(controller2.r1Channel->value)
				{
					testBook.getSelectedItem()->setDrawScale(testBook.getSelectedItem()->getDrawScale()+controller2.y2Channel->value*SCALE_ADJUST_STEP);
					//testBook.getSelectedItem()->textbox()->setFontSize(testBook.getSelectedItem()->textbox()->getFontSize()+controller2.y2Channel->value*SCALE_ADJUST_STEP);
				}

				if(controller2.r2Channel->value)
				{
					testBook.getSelectedItem()->setAlignScale(testBook.getSelectedItem()->getAlignScale()+controller2.y2Channel->value*SCALE_ADJUST_STEP);
					testBook.realign();
				}

				if(!(controller2.r1Channel->value || controller2.r2Channel->value))
				{
					if(controller2.x2DebounceChannel->value < 0)
					{
						testBook.getSelectedItem()->setXAlign(PIFLAG_XALIGN_LEFT);
						testBook.realign();
					}
					else if(controller2.x2DebounceChannel->value > 0)
					{
						testBook.getSelectedItem()->setXAlign(PIFLAG_XALIGN_RIGHT);
						testBook.realign();
					}

					if(controller2.y2DebounceChannel->value < 0)
					{
						testBook.getSelectedItem()->setYAlign(PIFLAG_YALIGN_BOTTOM);
						testBook.realign();
					}
					else if(controller2.y2DebounceChannel->value > 0)
					{
						testBook.getSelectedItem()->setYAlign(PIFLAG_YALIGN_TOP);
						testBook.realign();
					}
				}
			}
		}
	}

	// PP: Nyeh very temp - force the test book to respond to navigation
//	testBook.getCurrentPage()->respondToNavigation();

	// PP: update test book
// PP: REMOUT: Now Handled by the Book List			testBook.update();

	// PP: TEMP BODGE: TODO: add limit params to spinner constructor
	if(SFX_minDist == 0.0f)
	{
		SFX_minDist=0.01f;
	}

	// PP: SOUND TEST

	if(controller1.l1DebounceChannel->value)
	{
		bkPrintf("positional\n");

		(soundPos=gameStatus.player1->actorInstance.position)[Y]+=METERTOUNIT(0.5f);
		StopChannel(&g_soundChannel);
		g_soundChannel=PlaySample("phoneboxloop.wav", 255, soundPos.v);
	}

	if(controller1.r1DebounceChannel->value)
	{
		bkPrintf("tracking\n");
		StopChannel(&g_soundChannel);
		(soundPos=gameStatus.player1->actorInstance.position)[Y]+=METERTOUNIT(0.5f);
		g_soundChannel=PlayTrackingSample("phoneboxloop.wav", 255, soundPos.v);
	}


#endif// PP: def TEST_BOOK

	return;	

	// PP: TEMP TESTTEMP TESTTEMP TESTTEMP TEST

/*	if (controller1.selectDebounceChannel->value)
	{
		biZeroMap(controller1.inputMap);
		UpdateGuiItems();
		bgEnterGui(GuiCallback);
	}*/

	/*
	if(controller1.triangleDebounceChannel->value)
	{
		//gameStatus.player[0].stats->endOfLevelStatsBook->realign();
		tutorialBook->realign();
	}
*/
	// PP: SCREENSHOTS ON BLACK & WHITE, PAD 1/2

/* PP: 	if((controller1.l1DebounceChannel->value)||(controller1.r1DebounceChannel->value)
		||(controller2.l1DebounceChannel->value)||(controller2.r1DebounceChannel->value))
	{
		char	mystring[256];

		bdScreenshot(bkGenerateUniqueFilename(mystring,"Screenshot",".bmp"));	
	}*/

	// PP: SCREENSHOTS ON SELECT/START, 2

/*	if((controller2.startDebounceChannel->value)||(controller2.selectDebounceChannel->value))
	{
		char	mystring[256];

		bdScreenshot(bkGenerateUniqueFilename(mystring,"Screenshot",".bmp"));
	}*/

	// PP: TEMP TEST
// PP: what's happened to me light directions???!!!!
/*	baSetDirectionalLight(&cartoonLight, normalCartoonLightVec);

	EnableActorCartoonRenderMode(&gameStatus.player1->actorInstance);

	//InitCartoonLight("textures\\ramps.bmp");
	UpdateCartoonLight(normalCartoonLightVec, WHITE);*/

	// PP: white & black buttons to end demo
#ifdef CONSUMERDEMO_RECORD_DEMO
	if(gameStatus.demoMode == -1)
	{
		if((controller1.l1DebounceChannel->value)
			||(controller1.r1DebounceChannel->value)
			||(controller2.l1DebounceChannel->value)
			||(controller2.r1DebounceChannel->value)
			)
		{
			FadeAndChangeScene(&camera[0].fade, &gameStatus, FADETYPE_NORMAL, map.sceneNumber, 1.0f);
			//bgSetMenuItem(recordDemo,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_TICKED);
		}
	}
#endif// PP: def CONSUMERDEMO_RECORD_DEMO


#ifdef INFOGRAMES
	return;
#endif// PP: def INFOGRAMES

	if(controller1.startDebounceChannel->value != 0)
	{
		if(flyCamActive)
		{
			// PP: Aye, if you is in fly cam and you presses start, you get da fing wot I like to call "SLED"
			toggleShadowLightDebug();
		}
	}

/*
		if(controller1.l1DebounceChannel->value)
	{
//		bgSetMenuItem(ingameView,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
//		bgSetMenuItem(objectView,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		setFOV(OBJECT_VIEWER_FOV);
		bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
		SetGameState(&gameStatus,GAMESTATE_OBJECTVIEWER,GAMESUBSTATE_NORMAL);
		InitObjectViewer();
	}
	else if(controller1.r1DebounceChannel->value)
	{
//		bgSetMenuItem(objectView,NULL,NULL,NULL,0,BGUIMENUITEMFLAG_DISABLED);
//		bgSetMenuItem(ingameView,NULL,NULL,NULL,BGUIMENUITEMFLAG_DISABLED,BGUIMENUITEMFLAG_DISABLED);
		SetGameState( &gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL );
		setFOV(NORMAL_INGAME_FOV);
		bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
		FreeObjectViewer();
	}
*/

	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*	if(controller1.triangleDebounceChannel->value)
	{
	// PP: 	testBook.toggleOpenClose();

		thwapBook->toggleOpenClose();
	}

	thwapBook->update();*/




	// PP: ********** SOUND TEST **********

/*	TBSample*	testSample;

	if(bmRand()&1)
	{
		testSample=bkLoadSample(NULL, "spring.wav", NULL);
	}
	else
	{
		testSample=bkLoadSample(NULL, "spring2.wav", NULL);
	}

	if(controller1.l2DebounceChannel->value != 0)
	{
		bsPlayPositionalSample(	testSample,
								gameStatus.player1->actorInstance.position,
								255,
								0,
								0,
								25,
								250000);// PP: TT: keep this within about 10x of min (100, 500)
	}

	if(controller1.r2DebounceChannel->value != 0)
	{
		bsPlaySample(testSample,255,0,0,0);
	}

	if(controller1.l1DebounceChannel->value != 0)
	{
		bsPlayTrackingSample(	testSample,
								gameStatus.player1->actorInstance.position,
								255,
								0,
								0,
								NULL,
								0,
								25,
								250000);
	}
*/

/*


	return;

	{
		if(controller1.squareDebounceChannel->value != 0)
		{
			CityBoss_game.win();
		}

		if(controller1.circleDebounceChannel->value != 0)
		{
			CityBoss_game.lose();
		}

		return;
	}

	// PP: VEC speed test
	testVECs();

	// PP: FPS TEST
	fpsBox->sprintf8("bFPS: %5.2f", bFPS);
*/

//	bmVectorCopy(cartoonLightVec, gameStatus.player1->actorInstance.position-soundPos);

//	baSetDirectionalLight(&cartoonLight, cartoonLightVec);

//	return;
	
	// PP: QUICKSORT TEST (utilQuickSort works fine)

	if(controller2.squareDebounceChannel->value != 0)
	{
		testBookDrawDebug ^= 1;
	}

/*


	int32		testInts[10];
	int32		i;

	bkPrintf("raw: ");
	for(i=0;i<10;i++)
	{
		testInts[i]=(int32)UtilGenerateRandomNumber(0,100);
		bkPrintf("%d, ", testInts[i]);
	}
	bkPrintf("\n");

	//utilQuickSort(testInts, 0,9);
	//utilQuickSort((uchar*)testInts, sizeof(int32), (uchar*)(&swap), 0,9, qsfunc);
	utilQuickSort(testInts, 0,9);
	
	bkPrintf("sorted: ");
	for(int i=0;i<10;i++)
	{
		bkPrintf("%d, ", testInts[i]);
	}
	bkPrintf("\n");
	bkPrintf("\n");


*/



	// PP: POSTER EXPLOSION TEST

/*	static float	expHeight=METERTOUNIT(4);

	if (controller2.squareDebounceChannel->value)
	{
		TBVector	pos;

		bmVectorCopy(pos, gameStatus.player1->actorInstance.position);

		pos[Z]+=METERTOUNIT(3.5f);

		//Fireball_normalExplosion(pos, METERTOUNIT(2));

		Fireball_flatExplosion(pos, METERTOUNIT(6), expHeight);
	}*/
/*
	// PP: cannon EXPLOSION TEST
	if(controller2.squareDebounceChannel->value)
	{
		Fireball_cannonExplosion(gameStatus.player1);
	}
*/
	// PP: ##################
	// PP: blend mode test...
	// PP: ##################

	// PP: fly cam test...

/*	if(controller2.squareDebounceChannel->value)
	{
		toggleFlyCam();
	}*/



/*
	if(controller2.crossDebounceChannel->value)
	{
		frameBlur^=1;
		bkPrintf("frameBlur %s\n", frameBlur?"ON":"off");
	}*/

	// PP: *************
	// PP: TEST DOF
	// PP: *************
/*
#define DOFSTEP		(fTime*METERTOUNIT(10))

	g_testDOFNear-=(controller2.y1*DOFSTEP);
	g_testDOFFar-=(controller2.y2*DOFSTEP);

	((TEXTBOX*)(dofBook.getItem("NEAR")))->sprintf8("near %d", (int)(UNITTOMETER(g_testDOFNear)));
	((TEXTBOX*)(dofBook.getItem("FAR")))->sprintf8("far %d", (int)(UNITTOMETER(g_testDOFFar)));

	dofBook.update();
*/
	// PP: **************
	// PP: TEST FOV
	// PP: **************
/*
#define FOVSTEP		(fTime*RAD(10))

	if(controller2.y1Channel->value != 0.0f)
	{
		g_testFOV-=(FOVSTEP*controller2.y1);

		setFOV(g_testFOV);

	//	bkPrintf("FOV: %5.2f\n", DEG(g_testFOV));
	}
*/


	// PP: **************
	// PP: TEST BOOK
	// PP: **************

#define STEP	0.08f

	/*testBookWidthPortion+=controller1.y1*fTime*STEP;
	testBookHeightPortion+=controller1.y2*fTime*STEP;

	//testBook.setBookRect(RECTANGLE(-testBookWidthPortion*0.5f, testBookWidthPortion*0.5f, -testBookHeightPortion*0.5f, testBookHeightPortion*0.5f));
	bkPrintf("width %5.2f, height %5.2f\n", testBookWidthPortion, testBookHeightPortion);*/
/*
	testShadowScale+=controller2.y2*fTime*STEP;

	bkPrintf("shadow scale %f\n", testShadowScale);
*/
}

/*	--------------------------------------------------------------------------------
	Function 	: FreePhilScene
	Purpose 	: Free all scene data, instances, actors and textures
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void FreePhilScene(int scene)
{
}