//
//  BoundingBox.h
//  RayTracer
//
//  Created by Daniel Shih on 5/30/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__BoundingBox__
#define __RayTracer__BoundingBox__

#include <iostream>
#include "Vector3D.h"
#include "PartitionPlaneEnums.h"
#include "Triangle.h"

/// Describes an axis-aligned bounding box in 3D space
class BoundingBox
{
public:
    BoundingBox(Vector3D min, Vector3D max);
    ~BoundingBox();
    BoundingBox ExpandToContain(BoundingBox targetBoundingBox);
    BoundingBox Constrain(PartitionPlaneType planeType, float partitionValue, PartitionKeepDirection keepDirection);
    
    /// Gets the minimum bounding box required to bound the object
    static BoundingBox GetMinimumBoundingBox(Triangle object);
    
    /// Determines if there is no overlap between the current bounding box and the provided bounding box
    bool IsDisjoint(BoundingBox otherBox);

    /// Determines whether or not the given triangle intersects with any part of the space bounded by this bounding box
    bool Intersects(Triangle triangle);
    
    /// Determines if the given point is within the current bounding box
    bool Contains(Vector3D point);
    
    /// Determines if the current bounding box fully contains the given triangle
    bool Contains(Triangle triangle);
    
    /// Gets the surface area of this bounding box
    double SurfaceArea();

    /// Gets the longest edge
    PartitionPlaneType GetLongestEdge();
    
    /// Gets the length of this bounding box along the specified axis
    double GetLength(PartitionPlaneType axis);
    
    /// Gets the coordinate of the minimum bound of this bounding box in the specified axis
    double GetMinInAxis(PartitionPlaneType axis);
    
    /// The corner defining the maximum of the 3 axes of the bounding box
    Vector3D max;
    /// The corner defining the minimum of the 3 axes of the bounding box
    Vector3D min;
};

#endif /* defined(__RayTracer__BoundingBox__) */
