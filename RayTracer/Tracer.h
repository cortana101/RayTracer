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
#include "LightSource.h"
#include "VectorUtils.h"

class Tracer
{
public:
    Tracer();
    ~Tracer();
    /// Performs the actual render of the output buffer
    OutputRasterizer* Render(Triangle* model, int modelLength, LightSource* lightSources, int lightSourceLength, int viewAngleX, int xSpan, int ySpan);
private:
    /// Determines if there is a hit between the ray and the triangle
    ///\Returns the exact hit point if it exists, otherwise returns null
    Vector3D* ProcessSingleRay(Triangle triangle, Vector3D* ray, Vector3D** outReflection);
};

#endif /* defined(__RayTracer__Tracer__) */
