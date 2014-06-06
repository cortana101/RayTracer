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
#include <math.h>

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
    
    /// Determines if the given point is within the current bounding box
    bool Contains(Vector3D point);
    
    /// Determines if the current bounding box fully contains the given triangle
    bool Contains(Triangle triangle);
    
    /// Gets a point on all the surfaces of this bounding box that represents the intersection of the provided ray
    /// and the current bounding box which is closest to the ray origin.
    /// Returns false if there are no intersects
    bool TryGetIntersectionAtSurface(Vector3D ray, Vector3D rayOrigin, Vector3D* outIntersectAtSurface);
    
    /// Gets a point on a specific surface of this bounding box that is intersected by the provided ray
    bool TryGetIntersectionAtSurface(Vector3D ray, Vector3D rayOrigin, PartitionPlaneType plane, PartitionKeepDirection side, Vector3D* outIntersectAtSurface);
    
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
