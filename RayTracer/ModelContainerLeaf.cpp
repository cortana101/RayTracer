//
//  ModelContainerLeaf.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainerLeaf.h"

bool ModelContainerLeaf::AddItem(ModelObject *object)
{
    // If our current list of items is < max number of items, then add it
    // to the current list
    // If we've maxed out our list of items then split this current node and recurse
    
    // After insertion AT THE LEAF NODE, do a bound test to see if the current leaf
    // can completely contain the model, if yes, return success. Otherwise
    // go up the stack and keep inserting the model into neighbours until we've
    // totally contained the model object
    
    
    // Check the one note for details on the implementation
    // The only real missing piece is how to determine the split, we should use SAH
    // For SAH to work we need to be able to clip triangles that are not bounded entirely
    // by the current bounding box, in that case, apply sutherland-hodgman on the triangle,
    // and clip it by all 6 faces of the bounding box, the resulting area is the area we're
    // working with.
    // One key difference with other kd trees is that we wont be storing the clipped triangles
    // in our model. In theory with Sutherland-Hodgman every clip operation can result in 2x triangles
    // which in theory could result in 2^6 triangles as output, we dont want to have to store that
    // it suffices for us to store the original triangle, even as it is more than the bounding volume
    // but when we calculate SAH we can clip the triangle for that purpose and determine the split line.
    
    // Sutherland-Hodges in 3d:
        // 1. Put all vertices of polygon in list (doesnt have to be just triangle, could be any polygon)
        // 2. For a given split plane, determine which vertices are on the cut-side of the plane
        // 3. For each cut vertex, find the intersect via ProcessRay for a ray that originates from that vertex, and goes to its neighbouring vertex
        // 4. Since the polygon is convex, any cut should only result in 2 intersecting vertices
        // 5. those 2 intersecting vertices becomes the new edge vertices of the polygon
        // 6. After all the plane clips have happened, we should still have a convex polygon
        // 7. trivially generate triangles from that set of convex vertices.
    // For the purposes of SAH, calculate the sum of the areas of these triangles
    
    return false;
}