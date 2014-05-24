//
//  Tracer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Tracer.h"
#define REFLECTIVELOSS 0.6
#define CONSOLEPROGRESSLENGTH 30
#define LAMBERTIANCONSTANT 8.0
#define PHONGCONSTANT 0.1

Tracer::Tracer()
{
    // Do nothing
}

Tracer::~Tracer()
{
    // Do nothing
}

OutputRasterizer Tracer::Render(ModelObject **model, int modelLength, LightSource* lightSources, int lightSourceLength, int viewAngleX, int xSpan, int ySpan)
{
    OutputRasterizer output (xSpan, ySpan);
    
    int consoleProgressCounter = 0;

    for(int i = 0; i < xSpan; i++)
    {
        if (consoleProgressCounter > (xSpan / CONSOLEPROGRESSLENGTH))
        {
            cout << "=";
            consoleProgressCounter = 0;
        }
        
        consoleProgressCounter++;
        
        for(int j = 0; j < ySpan; j++)
        {
            Vector3D ray = projectionUtils::GetProjection(viewAngleX, xSpan, ySpan, i, j);
            Vector3D origin = Vector3D(0.0, 0.0, 0.0);
 
            // Since the viewport is grounded at the origin, we start tracing with the rays grounded in the origin
            Colour colour = this->TraceRay(model, modelLength, lightSources, lightSourceLength, ray, origin, 3);
            output.SetOutput(i, j, colour.rVal, colour.gVal, colour.bVal);
        }
    }

    return output;
}

Colour Tracer::TraceRay(ModelObject **model, int modelLength, LightSource *lightSources, int lightSourceLength, Vector3D ray, Vector3D rayOrigin, int reflections)
{
    Vector3D closestReflectedRay;
    Vector3D closestIntersect;
    Vector3D normal;
    Triangle intersectedTriangle;
    bool hasIntersect = ProcessSingleRayInModel(model, modelLength, ray, rayOrigin, &closestIntersect, &normal, &closestReflectedRay, &intersectedTriangle);
    
    Colour output = Colour(0, 0, 0);
    
    if(hasIntersect)
    {
        for (int i = 0; i < lightSourceLength; i++)
        {
            Vector3D intersectToLight = closestIntersect.PointToPoint(*lightSources[i].position);
            
            // we dont need these variables actually, we just need to call ProcessSingleRayInModel to determine if the line of sight to the
            // light source is blocked or not
            Vector3D lightIntersectPoint, lightReflection, lightNormal;
            Triangle lightIntersectTriangle;
            Colour lightContribution = Colour(0, 0, 0);
            
            // Scale by this so we will hit the current triangle
            Vector3D adjustedIntersect = closestIntersect;
            adjustedIntersect.Scale(0.999);
            
            // If our line of sight to the light source doesnt hit any other object, we can light the current pixel, otherwise it will be dark and in shadow
            if (!ProcessSingleRayInModel(model, modelLength, intersectToLight, adjustedIntersect, &lightIntersectPoint, &lightNormal, &lightReflection, &lightIntersectTriangle))
            {
                // We should use lambertian and phong shading models here
                
                intersectToLight.ToUnitVector();
                closestReflectedRay.ToUnitVector();
                double lambertianContributionFactor = intersectToLight.DotProduct(normal) * LAMBERTIANCONSTANT;
                double phongContributionFactor = pow(closestReflectedRay.DotProduct(intersectToLight), intersectedTriangle.gloss) * PHONGCONSTANT * intersectedTriangle.gloss;
                
                Colour lambertianColour = intersectedTriangle.colour;
                lambertianColour.Scale(lambertianContributionFactor * lightSources[i].intensity);
                
                Colour phongColour = intersectedTriangle.colour;
                phongColour.Scale(phongContributionFactor * lightSources[i].intensity);
                
                lightContribution = lambertianColour;
                lightContribution.Add(phongColour);
            }
            
            output.Add(lightContribution);
        }
        
        reflections--;
        
        if (reflections >= 0)
        {
            Vector3D* diffuseRays = GenerateDiffuseRays(closestReflectedRay, closestIntersect, 1, 0.9);
            Colour reflectionColour = TraceRay(model, modelLength, lightSources, lightSourceLength, closestReflectedRay, closestIntersect, reflections);
            reflectionColour.Scale(REFLECTIVELOSS);
            output.Add(reflectionColour);
        }
    }
    else
    {
        output = Colour(0, 0, 0);
    }
    
    return output;
}

bool Tracer::ProcessSingleRayInModel(ModelObject **model, int modelLength, Vector3D ray, Vector3D rayOrigin, Vector3D *outIntersectPoint, Vector3D* outNormalizedNormal, Vector3D *outReflection, ModelObject* outIntersectedObject)
{
    Vector3D reflectedRay;
    Vector3D intersect;
    bool hasIntersect = false;
    int modelIterator = 0;
    
    while (modelIterator < modelLength)
    {
        if (model[modelIterator]->ProcessRay(ray, rayOrigin, &intersect, outNormalizedNormal, &reflectedRay))
        {
            if (!hasIntersect || intersect.GetMagnitude() < outIntersectPoint->GetMagnitude())
            {
                hasIntersect = true;
                *outIntersectPoint = intersect;
                *outReflection = reflectedRay;
                *outIntersectedObject = *model[modelIterator];
            }
        }
        
        modelIterator++;
    }
    
    return hasIntersect;
}

Vector3D* Tracer::GenerateDiffuseRays(Vector3D primaryRay, Vector3D primaryRayOrigin, int sampleCount, double diffuseness)
{
    // Need to generate the transformation matrix first
    // Transform the entire model around the reflected vector, such that the reflected vector is at origin and points in 0,0,1
    // Generate a set of diffuse ray samples given this
    
    primaryRay.ToUnitVector();
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