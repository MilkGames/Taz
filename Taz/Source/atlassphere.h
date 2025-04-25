// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2001 Interactive Studios Ltd.
//
//      File : Atlasphere.h
//   Purpose : Atlas-Sphere declarations
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_ATLASSPHERE
#define __H_ATLASSPHERE

class ATLASSPHERE
{
public:
	ATLASSPHERE(ACTORINSTANCE *, char *);
	
	~ATLASSPHERE(void);

	void beforeYouCallAnythingElseCallThis(void);
	void setBoundToActor(ACTORINSTANCE *pActorInstance);
	void applySphereRotationToActor(void);
	void calculateRoll(void);
	void calculateSphereCollision(void);
	void calculateMagnets(ACTORINSTANCE **, int);
	void calculateAI(float, float);
	void originSetNormal(ACTORINSTANCE *pActorInstance);
	void originSetAtlas(ACTORINSTANCE *pActorInstance);
	void draw(void);

	TBVector		oldVel;

	static ATLASSPHERE	**ppSphereList;
	static int			instanceCtr;

	ACTORINSTANCE	*pSphereActor;
	float			sphereActorRadius;
	int				hSphere;

	ACTORINSTANCE	*pBoundToActor;
	TBQuaternion	orientation;

	TBVector		homeInOn;
	int				homeInPoint;

	int				collidedLastTime;
	float			ySphereOffset;

	int				backwards;
	TBTexture		*pAnimTexture;
	int				ballColour;
	int				justRanOverRef;
	int				touchedNotTarget;
};

#endif