// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : physics.h
//   Purpose : general physics functions
// Component : Generic Physics
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __BMATHS_PHYSICS_H__
#define __BMATHS_PHYSICS_H__

// ********************************************************************************
// Constants and Macros

// Body types
typedef enum EBBodyType 
{
	EBBODY_CYLINDER,					// Simple cylinder
	EBBODY_CYLINDER_SHELL,				// Hollow cylinder
	EBBODY_RECTANGULAR_CYLINDER,		// Basic rectangular shape
	EBBODY_SPHERE,						// Simple sphere
	EBBODY_SPHERE_SHELL,				// Hollow sphere
};

// Gravitational constant
#define BPHYSICS_G					6.673e-11f
#define BPHYSICS_DENSITY_AIR		1.30f
#define BPHYSICS_DENSITY_HELIUM		0.18f


// ********************************************************************************
// Types, Structures and Classes

// Properties of body mass
typedef struct _TBMassProperties 
{
	float						mass;					// mass
	TBMatrix					momentOfInertia;		// moment of inertia
} TBMassProperties;

// Moment of inertia
//
// [ Ixx, -Ixy, -Ixz]
// [-Ixy,  Iyy, -Iyz]
// [-Ixz, -Iyz,  Izz]
//
// Ixx, Iyy, Izz are the moments of inertia about the 3 coordinate axes
// Ixy, Ixz, Iyz are the products of inertia (zero for a symetrical body)


// Point mass
typedef struct _TBPointMass 
{
	float						mass;						// mass
	TBVector					position;					// position relative to origin
	TBVector					positionRelativeToCG;		// position relative to center of gravity
	TBMatrix					momentOfInertia;			// msually only Ixx, Iyy, Izz - assumed symetric
} TBPointMass;


// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes


/*	--------------------------------------------------------------------------------
	Function : bmSetMomentOfInertia
	Purpose : Set the moment of inertia matrix
	Parameters : Moment of inertia matria to fill in
				 moments of inertia about the 3 coordinate axes:   Ixx, Iyy, Izz
				 products of inertia (zero for a symetrical body): Ixy, Ixz, Iyz
	Returns : 
	Info : 
*/

BINLINE void bmSetMomentOfInertia(TBMatrix moi,
								  float Ixx, float Iyy, float Izz, 
								  float Ixy=0.0f, float Ixz=0.0f, float Iyz=0.0f)
{
	moi[0][0] =  Ixx;	moi[0][1] = -Ixy;	moi[0][2] = -Ixz;
	moi[1][0] = -Ixy;	moi[1][1] =  Iyy;	moi[1][2] = -Iyz;
	moi[2][0] = -Ixz;	moi[2][1] = -Iyz;	moi[2][2] =  Izz;
}


/*	--------------------------------------------------------------------------------
	Function : bmSetMomentOfInertia
	Purpose : calculate moment of inertia for basic geometries
	Parameters : moment of inertia, body type, mass, width, length, height
	Returns : 
	Info : 
*/

void bmSetMomentOfInertia(TBMatrix moi, EBBodyType type, float mass, float w, float l, float h);


/*	--------------------------------------------------------------------------------
	Function : bmCalculateCombinedMass
	Purpose : Calculate center of gravity and total mass
	Parameters : array of point masses, number of points, 
				 center of gravity [out], total mass [out]
	Returns : 
	Info : Also updates the positionRelativeToCG within the pointMasses
*/

void bmCalculateCombinedMass(TBPointMass *pointMasses, int points, 
							 TBVector centerOfGravity, float *totalMass);


/*	--------------------------------------------------------------------------------
	Function : bmCalculateCombinedMomentOfInertia
	Purpose : Calculate moment of inertia for point masses
	Parameters : array of point masses, number of points, 
				 moment of inertia [out]
	Returns : 
	Info : 
*/

void bmCalculateCombinedMomentOfInertia(TBPointMass *pointMasses, int points, 
										TBMatrix momentOfInertia);


/*	--------------------------------------------------------------------------------
	Function : bmCalculateRelativeVelocity
	Purpose : Calculate velocity of a point on a rigid body
	Parameters : relative velocity, velocity at center of gravity,
				 angular velocity, location of point relative to center of gravity
	Returns : 
	Info : 
*/

void bmCalculateRelativeVelocity(TBVector rv, TBVector v, TBVector w, TBVector r);


/*	--------------------------------------------------------------------------------
	Function : bmGetSpeed
	Purpose : Return the speed from a velocity vector
	Parameters : velocity
	Returns : speed
	Info : 
*/

BINLINE float bmGetSpeed(TBVector velocity)	{ return bmVectorLen(velocity); }


/*	--------------------------------------------------------------------------------
	Function : bmForceOfGravity
	Purpose : Return the force due to gravity
	Parameters : mass of body 1, mass of body 2, distance between body centres
	Returns : force of gravity
	Info : For the earth mass1 = 5.98e24, radius = 6.38e6, F=ma so a = 9.8
*/

BINLINE float bmForceOfGravity(float mass1, float mass2, float r)	
{ return  (BPHYSICS_G*mass1*mass2)/(r*r); }


/*	--------------------------------------------------------------------------------
	Function : bmForceOfDrag
	Purpose : Return the force due to drag
	Parameters : density of fluid, speed, area facing fluid, drag coefficient
	Returns : force of drag
	Info : The fluid can be air !
		   c = 0.29 to 0.4 for sports cars
		   c = 0.43 to 0.5 for pickup trucks
		   c = 0.6  to 0.9 for tractor-trailers
		   c = 0.4  to 0.5 for economy cars
		   density of helium is 0.18 kg/ m3	(BPHYSICS_DENSITY_HELIUM) 
		   density of air is 1.30 kg/ m3	(BPHYSICS_DENSITY_AIR)
		   (average density values since they depend on factors such as temperature)
*/

BINLINE float bmForceOfDrag(float density, float speed, float area, float c)	
{ return  (0.5f*density*speed*speed*area*c); }


/*	--------------------------------------------------------------------------------
	Function : bmForceOfSpring1D
	Purpose : Return the force due to spring
	Parameters : spring constant kS, rest length r stretched length l
	Returns : force of spring
	Info : 
*/

BINLINE float bmForceOfSpring1D(float kS, float r, float l)
{ return  (kS*(l-r)); }


/*	--------------------------------------------------------------------------------
	Function : bmForceOfDampedSpring1D
	Purpose : Return the force due to damped spring
	Parameters : spring constant kS, rest length r, stretched length l,
				 damping kD, velocity of body 1, velocity of body 2
	Returns : force of spring
	Info : 
*/

BINLINE float bmForceOfDampedSpring1D(float kS, float r, float l, 
									float kD, float v1, float v2)
{ return  (kS*(l-r) + kD*(v2-v1)); }


/*	--------------------------------------------------------------------------------
	Function : bmForceOfSpring3D
	Purpose : Return the force due to spring
	Parameters : spring constant kS, streched length l, rest length r
				 position of body 1, position of body 2
				 velocity of body 1, velocity of body 2
	Returns : force of spring exerted on body 1 (negate for force on body 2)
	Info :  {kS*(L-r)}; 
*/

void bmForceOfSpring3D(TBVector result, float kS, float r, TBVector pos1, TBVector pos2);

/*	--------------------------------------------------------------------------------
	Function : bmForceOfDampedSpring3D
	Purpose : Return the force due to spring (with damping)
	Parameters : spring constant kS, rest length r, damping kD, 
				 position of body 1, position of body 2
				 velocity of body 1, velocity of body 2
	Returns : force of spring exerted on body 1 (negate for force on body 2)
	Info :  {kS*(L-r) + kD*[(v2-v1).L]/l}L/l; 
*/

void bmForceOfDampedSpring3D(TBVector result,
						   float kS, float r, float kD, 
						   TBVector pos1, TBVector pos2,
						   TBVector v1, TBVector v2);

#endif //__BMATHS_PHYSICS_H__
