// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : physics.cpp
//   Purpose : general physics functions
// Component : Generic Physics
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
Function : bmSetMomentOfInertia
Purpose : calculate moment of inertia for basic geometries
Parameters : moment of inertia, body type, mass, width, length, height
Returns : 
Info : 
*/

void bmSetMomentOfInertia(TBMatrix moi, EBBodyType type, float mass, float w, float l, float h)
{
        bkPrintf("*** WARNING *** bmSetMomentOfInertia was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
Function : bmCalculateCombinedMass
Purpose : Calculate center of gravity and total mass
Parameters : array of point masses, number of points, 
center of gravity [out], total mass [out]
Returns : 
Info : Also updates the positionRelativeToCG within the pointMasses
*/

void bmCalculateCombinedMass(TBPointMass *pointMasses, int points, 
TBVector centerOfGravity, float *totalMass)
{
        bkPrintf("*** WARNING *** bmCalculateCombinedMass was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
Function : bmCalculateCombinedMomentOfInertia
Purpose : Calculate moment of inertia for point masses
Parameters : array of point masses, number of points, 
moment of inertia [out]
Returns : 
Info : 
*/

void bmCalculateCombinedMomentOfInertia(TBPointMass *pointMasses, int points, 
          TBMatrix momentOfInertia)
{
        bkPrintf("*** WARNING *** bmCalculateCombinedMomentOfInertia was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
Function : bmCalculateRelativeVelocity
Purpose : Calculate velocity of a point on a rigid body
Parameters : relative velocity, velocity at center of gravity,
angular velocity, location of point relative to center of gravity
Returns : 
Info : 
*/

void bmCalculateRelativeVelocity(TBVector rv, TBVector v, TBVector w, TBVector r)
{
        bkPrintf("*** WARNING *** bmCalculateRelativeVelocity was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
Function : bmForceOfSpring3D
Purpose : Return the force due to spring
Parameters : spring constant kS, streched length l, rest length r
position of body 1, position of body 2
velocity of body 1, velocity of body 2
Returns : force of spring exerted on body 1 (negate for force on body 2)
Info :  {kS*(L-r)}; 
*/

void bmForceOfSpring3D(TBVector result, float kS, float r, TBVector pos1, TBVector pos2)
{
        bkPrintf("*** WARNING *** bmForceOfSpring3D was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

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
TBVector v1, TBVector v2)
{
        bkPrintf("*** WARNING *** bmForceOfDampedSpring3D was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}