//
//  Polygon.h
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__Polygon__
#define __RayTracer__Polygon__

#include <iostream>
#include <QtCore/QList>
#include "Vector3D.h"
#include "Triangle.h"
#include "PartitionPlaneEnums.h"

/// Contains a polygon of 3 or more vertices that lie on the same plane
/// Mainly used for clipping operations and getting the resulting triangles
class Polygon
{
public:
    Polygon(Triangle startingTriangle);
    Triangle* Triangulate(int* outNumTriangles);
    
    /// Clips the current polygon along the provided plane
    void Clip(PartitionPlaneType plane, double planePosition, PartitionKeepDirection keepDirection);
    
private:
    Colour startingColour;
    double startingGloss;
    /// List of vertices for this polygon
    QList<Vector3D> vertices;
    
    /// Takes in 2 vertices, one a keeper the other clipped, and finds the point where the edge between them intersects
    /// the clipping plane
    Vector3D FindIntersectingVertex(PartitionPlaneType plane, double planePosition, Vector3D vertex1, Vector3D vertex2);
};

#endif /* defined(__RayTracer__Polygon__) */
