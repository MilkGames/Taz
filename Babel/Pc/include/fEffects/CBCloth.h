// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBCloth.h
//   Purpose : cloth base class
// Component : Generic BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FEFFECTS_CBCLOTH_H__
#define __FEFFECTS_CBCLOTH_H__

// ********************************************************************************
// Constants and macros

#define BCLOTH_PARTICLEFLAG_LOCKED		0x00000001


// ********************************************************************************
// Types, Structures and Classes


// cloth particle type
typedef	struct _CBClothParticle
{
	float			fMass;
	float			fInvMass;
	int				pad;
	int				flag;

	TBVector		vPosition;
	TBVector		vVelocity;
	TBVector		vAcceleration;
	TBVector		vForces;
} CBClothParticle;


// cloth spring type
typedef	struct _CBClothSpring 
{
	CBClothParticle	*p1;			// reference to connected particle #1
	CBClothParticle	*p2;			// reference to connected particle #2
	int				pad[2];

	float			Ks;				// tensile spring constant
	float			Kd;				// damping factor	
	float			L;				// rest length of spring
	float			pad2;
} CBClothSpring;


typedef struct	_CBClothCollision 
{
	CBClothParticle	*p;				// particle colliding
	TBVector		n;				// normal vector of collision
} CBClothCollision;


// base class for cloth
typedef class CBCloth {
	private:
									// Initialise cloth location
		void						InitMasses();

									// Initialise cloth structural springs
		void						InitSprings();

									// Create the cloth render object
		void						CreateClothObject();

									// Calculate forces - wind, gravity and cloth structural springs
		void						CalcForces();

									// Clear collision information
		void						ClearCollisionInfo();

	public:
		float						width;					// Cloth width
		float						height;					// Cloth height
		float						totalMass;				// Cloth mass
		float						pad;

		float						tensionConstant;		// horizontal/vertial tension constraint
		float						shearConstant;			// diagonal shear constraint
		float						dampingConstant;		// spring damping value
		float						dragCoefficient;		// air frictional drag coefficient

		float						collisionTolerance;		// tolerance for collisions
		float						zeroVelocityTolerance;	// zero velocity tolerance
		float						coefficientOfRestitution;// for collision response
		float						coefficientOfFriction;	// for collision response

		int							xSegments;				// number of x segments in cloth
		int							ySegments;				// number of y segments in cloth
		int							noofVerts;				// number of vertices
		int							noofSprings;			// number of springs

		CBClothParticle				*particles;				// Cloth particles
		CBClothSpring				*springs;				// Cloth structural springs
		TBPrimVertex				*vertices;				// Cloth vertex list
		CBClothCollision			*collision;				// Cloth collision info

		int							vertDataSize;			// vertex total data size
		TBTexture					*texture;				// Cloth texture
		// Forces acting on cloth
		TBVector					gravity;				// force of gravity
		TBVector					windVector;				// wind direction

									// constructor
									CBCloth(float iWidth, float iHeight, int iXSegments, int iYSegments, 
											float density, TBTexture *texturePtr, TBMatrix m,
											float tension=500.0f, float shear=600.0f, float damping=4.0f);

									// destructor
									~CBCloth();

									// Initialise cloth location
		void						InitPosition(TBMatrix m);

									// render the cloth in it's current state
		void						Render();

									// update the cloth simulation
		void						Update(float timestep);

									// set wind direction
		void						SetWindVector(float x, float y, float z);
									
									// set individual particle flag: e.g. lock particle
		void						SetParticleFlag(int x, int y, int flag);

									// set all individual particle flags: e.g. unlock all particles
		void						SetAllParticleFlags(int flag);

									// check to see if any collisions occur with a ground plane
		int							CheckForCollisionsWithGround(float height);

									// check to see if any collisions occur with a sphere
		int							CheckForCollisionsWithSphere(TBVector pos, float radius);

									// resolve any collisions
		void						ResolveCollisions();

} CBCloth;


#endif		// __FEFFECTS_CBCLOTH_H__
