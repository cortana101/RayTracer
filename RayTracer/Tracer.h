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
#include "TransformationMatrix.h"
#include "ModelObject.h"
#include "IntersectProperties.h"

class Tracer
{
public:
    Tracer();
    ~Tracer();
    /// Performs the actual render of the output buffer
    OutputRasterizer Render(ModelObject** model, int modelLength, LightSource* lightSources, int lightSourceLength, int viewAngleX, int xSpan, int ySpan);
    
    /// Traces a single ray in the model and gets the output colour
    ///\Param reflections The number of reflections remaining to be traced by this starting ray
    Colour TraceRay(ModelObject** model, int modelLength, LightSource* lightSources, int lightSourceLength, Vector3D ray, Vector3D rayOrigin, int reflections);
private:
    /// Does the same thing ProcessSingleRay does, but applies to the entire model rather than to a single triangle
    bool ProcessSingleRayInModel(ModelObject** model, int modelLength, int ignoreModelAtIndex, Vector3D ray, Vector3D rayOrigin, int* outInteresectObjectIndex, IntersectProperties* outIntersectProperties);
    
    /// For a given reflected ray, gets a list of diffuse rays to sample
    Vector3D* GenerateDiffuseRays(Vector3D primaryRay, Vector3D primaryRayOrigin, int sampleCount, double diffuseness);
};

#endif /* defined(__RayTracer__Tracer__) */
