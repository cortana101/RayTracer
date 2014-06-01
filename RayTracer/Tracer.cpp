//
//  Tracer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Tracer.h"
#include <pthread.h>
#include <unistd.h>
#define REFLECTIVELOSS 0.6
#define CONSOLEPROGRESSLENGTH 60
#define LAMBERTIANCONSTANT 8.0
#define PHONGCONSTANT 100
#define NUMCONCURRENTTHREADS 4
#define UPDATEPROGRESSDELAYMS 100

Tracer::Tracer()
{
    // Do nothing
}

Tracer::~Tracer()
{
    // Do nothing
}

OutputRasterizer Tracer::Render(ModelContainer modelContainer, LightSource* lightSources, int lightSourceLength, int viewAngleX, int xSpan, int ySpan)
{
    OutputRasterizer output (xSpan, ySpan);
   
    cout << "|";
    
    // Start at 2 to account for the start and end pipe chars
    for(int i = 2; i < CONSOLEPROGRESSLENGTH; i++)
    {
        cout << ".";
    }
    
    cout << "|\n";
    cout.flush();
    
    // Used as a bag of flags to tell which threads have finished
    bool isThreadRunning[NUMCONCURRENTTHREADS];
    std::fill(isThreadRunning, isThreadRunning + NUMCONCURRENTTHREADS, false);
    
    // Make 1 more thread than the number of concurrent worker threads to hold the print progress thread
    pthread_t threads[NUMCONCURRENTTHREADS + 1];
    
    // PixelProgress stores the global progress across all threads, its basically a pseudo-queue, all the threads
    // will grab a pixel to work on based on the pixel progress counter and it will be incremented every time a thread
    // picks up a pixel to work on
    int pixelProgress = 0;
    int totalPixelCount = xSpan * ySpan;
    
    // Since the threads will be updating pixel progresses, we should lock it to avoid race conditions
    pthread_mutex_t pixelProgressMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    void* status;
    
    ProgressParams progressParam;
    progressParam.pixelProgress = &pixelProgress;
    progressParam.totalPixelCount = totalPixelCount;
    
    for(int i = 0; i < NUMCONCURRENTTHREADS; i++)
    {
        TraceRayParams parameters;
        parameters.modelContainer = modelContainer;
//        parameters.modelLength = modelLength;
        parameters.lightSources = lightSources;
        parameters.lightSourceLength = lightSourceLength;
        parameters.reflectionDepth = 4;
        parameters.outputBuffer = &output;
        parameters.progress = progressParam;
        parameters.xSpan = xSpan;
        parameters.ySpan = ySpan;
        parameters.viewAngleX = viewAngleX;
        parameters.pixelProgressMutex = pixelProgressMutex;
        
        void *(*TraceRayFunction)(void*);
        TraceRayFunction = &Tracer::TraceRayThread;
       
        pthread_create(&threads[i], NULL, TraceRayFunction, (void*)&parameters);
    }
    
    void *(*PrintProgressFunction)(void*);
    PrintProgressFunction = &Tracer::PrintProgress;
    
    pthread_create(&threads[NUMCONCURRENTTHREADS], NULL, PrintProgressFunction, (void*)&progressParam);
    
    pthread_attr_destroy(&attr);
    
    for (int i = 0; i < NUMCONCURRENTTHREADS + 1; i++)
    {
        pthread_join(threads[i], &status);
    }
    
    return output;
}

void* Tracer::PrintProgress(void *printProgressParams)
{
    ProgressParams* printProgressParameters = (ProgressParams*)printProgressParams;
    
    int stepSize = printProgressParameters->totalPixelCount / CONSOLEPROGRESSLENGTH;
    int lastStepPosition = 0;
    
    while(*printProgressParameters->pixelProgress < printProgressParameters->totalPixelCount)
    {
        int progressSteps = (*printProgressParameters->pixelProgress - lastStepPosition * stepSize) / stepSize;
        
        for (int i = 0; i < progressSteps; i++)
        {
            cout << "=";
            cout.flush();
        }
        
        lastStepPosition += progressSteps;
        usleep(UPDATEPROGRESSDELAYMS * 1000);
    }
    
    pthread_exit(NULL);
}

void* Tracer::TraceRayThread(void* traceRayParams)
{
    TraceRayParams* traceRayParameters = (TraceRayParams*)traceRayParams;
    int currentPixel = 0;
    
    while(*traceRayParameters->progress.pixelProgress < traceRayParameters->progress.totalPixelCount)
    {
        pthread_mutex_lock(&traceRayParameters->pixelProgressMutex);
        currentPixel = *traceRayParameters->progress.pixelProgress;
        *traceRayParameters->progress.pixelProgress += 1;
        pthread_mutex_unlock(&traceRayParameters->pixelProgressMutex);
        
        int currentXPos = currentPixel / traceRayParameters->ySpan;
        int currentYPos = currentPixel - currentXPos * traceRayParameters->ySpan;
    
        Vector3D ray = projectionUtils::GetProjection(traceRayParameters->viewAngleX, traceRayParameters->xSpan, traceRayParameters->ySpan, currentXPos, currentYPos);
        Vector3D origin = Vector3D(0.0, 0.0, 0.0);
    
        Colour outputColour = TraceRay(traceRayParameters->modelContainer,
                                       traceRayParameters->lightSources,
                                       traceRayParameters->lightSourceLength,
                                       ray,
                                       origin,
                                       traceRayParameters->reflectionDepth);
    
        traceRayParameters->outputBuffer->SetOutput(currentXPos, currentYPos, outputColour.rVal, outputColour.gVal, outputColour.bVal);
    }
    
    pthread_exit(NULL);
}

Colour Tracer::TraceRay(ModelContainer modelContainer, ModelObject* ignoredModel, LightSource *lightSources, int lightSourceLength, Vector3D ray, Vector3D rayOrigin, int reflections)
{
    IntersectProperties intersectProperties;
    ModelObject* outIntersectedObject;
    //int outIntersectedModelIndex;
    
    // pass NULL to the ignored model in order to include everything
    bool hasIntersect = modelContainer.TryGetIntersection(ray, rayOrigin, ignoredModel, &outIntersectedObject, &intersectProperties);
    
    Colour output = Colour(0, 0, 0);
    
    if(hasIntersect)
    {
        for (int i = 0; i < lightSourceLength; i++)
        {
            Vector3D intersectToLight = intersectProperties.intersectPosition.PointToPoint(*lightSources[i].position).ToUnitVector();
            
            // we dont need these variables actually, we just need to call ProcessSingleRayInModel to determine if the line of sight to the
            // light source is blocked or not
            IntersectProperties lightIntersectProperties;
            ModelObject* lightIntersectedObject;
            //int lightIntersectedModelIndex;
            
            // If our line of sight to the light source doesnt hit any other object, we can light the current pixel, otherwise it will be dark and in shadow
            // If the dot product of the normal and the path to the light is negative, it means the path to the light is behind the reflecting object, thus
            // light cannot possibly reach and we dont have to consider this particular light for this intersection
            if (!modelContainer.TryGetIntersection(intersectToLight, intersectProperties.intersectPosition, outIntersectedObject, &lightIntersectedObject, &lightIntersectProperties))
            {
                // We should use lambertian and phong shading models here
                
                double reflectRayDotLight = intersectProperties.normalizedReflection.DotProduct(intersectToLight);
                double lambertianContributionFactor = intersectToLight.DotProduct(intersectProperties.normalizedNormal) * LAMBERTIANCONSTANT;
                double phongContributionFactor = 0.0;
                
                if (reflectRayDotLight > 0.0)
                {
                    phongContributionFactor = pow(intersectProperties.normalizedReflection.DotProduct(intersectToLight), outIntersectedObject->gloss) * PHONGCONSTANT;
                }
                
                if (lambertianContributionFactor < 0.0)
                {
                    lambertianContributionFactor = 0.0;
                }
                
                Colour lambertianColour = outIntersectedObject->colour.Scale(lambertianContributionFactor * lightSources[i].intensity);
                
                Colour phongColour = outIntersectedObject->colour.Scale(phongContributionFactor * lightSources[i].intensity);
                
                output = output.Add(lambertianColour.Add(phongColour));
            }
        }
        
        reflections--;
        
        if (reflections >= 0)
        {
            Vector3D* diffuseRays = GenerateDiffuseRays(intersectProperties.normalizedReflection, intersectProperties.intersectPosition, 1, 0.9);
            Colour reflectionColour = TraceRay(modelContainer, outIntersectedObject, lightSources, lightSourceLength, intersectProperties.normalizedReflection, intersectProperties.intersectPosition, reflections);
            reflectionColour = reflectionColour.Scale(REFLECTIVELOSS);
            
            // Add after multiple to take into account the surface's reflectivity for each colour
            output = output.Add(outIntersectedObject->colour.Multiply(reflectionColour));
        }
    }
    else
    {
        output = Colour(0, 0, 0);
    }
    
    return output;
}

Colour Tracer::TraceRay(ModelContainer modelContainer, LightSource *lightSources, int lightSourceLength, Vector3D ray, Vector3D rayOrigin, int reflections)
{
    // Pass NULL to the ignored model, ie by default we dont ignore anything
    return TraceRay(modelContainer, NULL, lightSources, lightSourceLength, ray, rayOrigin, reflections);
}

Vector3D* Tracer::GenerateDiffuseRays(Vector3D primaryRay, Vector3D primaryRayOrigin, int sampleCount, double diffuseness)
{
    // Need to generate the transformation matrix first
    // Transform the entire model around the reflected vector, such that the reflected vector is at origin and points in 0,0,1
    // Generate a set of diffuse ray samples given this
    
    primaryRay = primaryRay.ToUnitVector();
    Vector3D unitX = Vector3D(1.0, 0.0, 0.0);
    Vector3D unitY = Vector3D(0.0, 1.0, 0.0);
    Vector3D unitZ = Vector3D(0.0, 0.0, 1.0);
    double v11 = primaryRay.DotProduct(unitX);
    double v12 = primaryRay.DotProduct(unitY);
    double v13 = primaryRay.DotProduct(unitZ);
    double v21 = primaryRay.DotProduct(unitX);
    double v22 = primaryRay.DotProduct(unitY);
    double v23 = primaryRay.DotProduct(unitZ);
    double v31 = primaryRay.DotProduct(unitX);
    double v32 = primaryRay.DotProduct(unitY);
    double v33 = primaryRay.DotProduct(unitZ);
    
    TransformationMatrix transform = TransformationMatrix(v11, v12, v13, v21, v22, v23, v31, v32, v33);
    Vector3D primaryInTermsOfPrimary = transform.TransformVector(primaryRay);
    
    return NULL;
}