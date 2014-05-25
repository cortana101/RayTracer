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

struct ProgressParams
{
    int* pixelProgress;
    int totalPixelCount;
};

struct TraceRayParams
{
    ModelObject** model;
    int modelLength;
    LightSource* lightSources;
    int lightSourceLength;
    int reflectionDepth;
    OutputRasterizer* outputBuffer;
    bool* threadRunningFlag;
    ProgressParams progress;
    int xSpan;
    int ySpan;
    int viewAngleX;
    pthread_mutex_t pixelProgressMutex;
};

class Tracer
{
public:
    Tracer();
    ~Tracer();
    /// Performs the actual render of the output buffer
    OutputRasterizer Render(ModelObject** model, int modelLength, LightSource* lightSources, int lightSourceLength, int viewAngleX, int xSpan, int ySpan);
private:
    static void* PrintProgress(void* printProgressParams);
    
    /// Wraps a single TraceRay thread
    static void* TraceRayThread(void* traceRayParams);

    /// Traces a single ray in the model and gets the output colour
    static Colour TraceRay(ModelObject **model, int modelLength, int ignoreModelAtIndex, LightSource *lightSources, int lightSourceLength, Vector3D ray, Vector3D rayOrigin, int reflections);
    
    /// Traces a single ray in the model and gets the output colour
    static Colour TraceRay(ModelObject **model, int modelLength, LightSource *lightSources, int lightSourceLength, Vector3D ray, Vector3D rayOrigin, int reflections);
    
    /// Does the same thing ProcessSingleRay does, but applies to the entire model rather than to a single triangle
    static bool ProcessSingleRayInModel(ModelObject** model, int modelLength, int ignoreModelAtIndex, Vector3D ray, Vector3D rayOrigin, int* outInteresectObjectIndex, IntersectProperties* outIntersectProperties);
    
    /// For a given reflected ray, gets a list of diffuse rays to sample
    static Vector3D* GenerateDiffuseRays(Vector3D primaryRay, Vector3D primaryRayOrigin, int sampleCount, double diffuseness);
};

#endif /* defined(__RayTracer__Tracer__) */
