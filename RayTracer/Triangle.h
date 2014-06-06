//
//  Triangle.h
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__Triangle__
#define __RayTracer__Triangle__

#include <iostream>
#include "OutputRasterizer.h"
#include "Vector3D.h"
#include "Colour.h"
#include "ModelObject.h"

/// Represents a triangle in 3d space
class Triangle : public ModelObject
{
public:
    Triangle();
    Triangle(Vector3D p1, Vector3D p2, Vector3D p3, Colour colour = Colour(0.0, 0.0, 0.0), double gloss = 0.0);
    ~Triangle();
    Triangle TranslateBy(Vector3D movement);
    virtual bool ProcessRay(Vector3D ray, Vector3D rayOrigin, IntersectProperties* outIntersectProperties);
    
    // Gets a random nominal position for this triangle
    Vector3D GetNominalPosition();
    
    /// Determines if the current triangle intersects with the other triangle
    ///bool IntersectsWith(Triangle otherTriangle);
    
    /// Determines if the current triangle intersects with the edge defined by the 2 endpoints v1 and v2
    bool IntersectsWithEdge(Vector3D v1, Vector3D v2);
    
    // Gets the surface area of this triangle
    double SurfaceArea();
    
    Vector3D p1;
    Vector3D p2;
    Vector3D p3;
};


#endif /* defined(__RayTracer__Triangle__) */
