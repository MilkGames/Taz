#ifndef __H_FOOTALIGN
#define __H_FOOTALIGN

/* --------------------------------------------------------------------------------
   Function : AlignNodeWithPolys
   Purpose : Align a node with the supplied polys
   Parameters : actorInstance containing nodes, nodeName to affect, TBVector poly verts, TBVector list of poly normals, num of normals
   Returns : TRUE if node was aligned, FALSE if node could not be aligned
   Info : 
*/
bool AlignNodeWithPolys(TBActorInstance *actorInstance, char *nodeName, TBVector normals, float offset);

void AlignFeet(ACTORINSTANCE *actorInstance);


#endif