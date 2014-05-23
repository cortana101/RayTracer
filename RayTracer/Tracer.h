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

class Tracer
{
public:
    Tracer();
    ~Tracer();
    /// Performs the actual render of the output buffer
    OutputRasterizer Render(Triangle* model, int modelLength, LightSource* lightSources, int lightSourceLength, int viewAngleX, int xSpan, int ySpan);
    
    /// Traces a single ray in the model and gets the output colour
    ///\Param reflections The number of reflections remaining to be traced by this starting ray
    Colour TraceRay(Triangle* model, int modelLength, LightSource* lightSources, int lightSourceLength, Vector3D ray, Vector3D rayOrigin, int reflections);
private:
    /// Determines if there is a hit between the ray and the triangle
    ///\Param RayOrigin indicates the origin of the ray
    ///\Returns true if we have a hit, false otherwise,
    /// If we hit, we will store the intersection point and the reflection vectors in the 2 provided pointers
    bool ProcessSingleRay(Triangle triangle, Vector3D ray, Vector3D rayOrigin, Vector3D* outIntersectPoint, Vector3D* outReflection);
    
    /// Does the same thing ProcessSingleRay does, but applies to the entire model rather than to a single triangle
    bool ProcessSingleRayInModel(Triangle* model, int modelLength, Vector3D ray, Vector3D rayOrigin, Vector3D* outIntersectPoint, Vector3D* outReflection, Triangle* outIntersectedTriangle);
};

#endif /* defined(__RayTracer__Tracer__) */
