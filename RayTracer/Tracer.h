//
//  Tracer.h
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__Tracer__
#define __RayTracer__Tracer__

#include <iostream>
#include "Vector3D.h"
#include "Triangle.h"
#include <math.h>
#include "OutputRasterizer.h"
#include "ProjectionUtils.h"

class Tracer
{
public:
    Tracer();
    ~Tracer();
    /// Performs the actual render of the output buffer
    OutputRasterizer* Render(Triangle* model, int modelLength, int viewAngleX, int xSpan, int ySpan);
private:
    bool IsHit(Triangle triangle, Vector3D* ray);
    
    double DotProduct(Vector3D* v1, Vector3D* v2);
    
    /// Returns a vector that goes from point1 to point2
    Vector3D* PointToPoint(Vector3D* point1, Vector3D* point2);
    
    /// Gets the magnitude of the given vector
    double GetMagnitude(Vector3D* v);
    
    /// Gets the angle between the 2 vectors in radians
    double GetAngle(Vector3D* v1, Vector3D* v2);
};

#endif /* defined(__RayTracer__Tracer__) */
