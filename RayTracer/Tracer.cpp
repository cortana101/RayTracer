//
//  Tracer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Tracer.h"
#define REFLECTIVELOSS 0.6

Tracer::Tracer()
{
    // Do nothing
}

Tracer::~Tracer()
{
    // Do nothing
}

OutputRasterizer Tracer::Render(Triangle *model, int modelLength, LightSource* lightSources, int lightSourceLength, int viewAngleX, int xSpan, int ySpan)
{
    OutputRasterizer output (xSpan, ySpan);

    for(int i = 0; i < xSpan; i++)
    {
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

Colour Tracer::TraceRay(Triangle *model, int modelLength, LightSource *lightSources, int lightSourceLength, Vector3D ray, Vector3D rayOrigin, int reflections)
{
    Vector3D closestReflectedRay;
    Vector3D closestIntersect;
    Triangle intersectedTriangle;
    bool hasIntersect = ProcessSingleRayInModel(model, modelLength, ray, rayOrigin, &closestIntersect, &closestReflectedRay, &intersectedTriangle);
    
    Colour output = Colour(0, 0, 0);
    
    if(hasIntersect)
    {
        for (int i = 0; i < lightSourceLength; i++)
        {
            Vector3D intersectToLight = closestIntersect.PointToPoint(*lightSources[i].position);
            
            // we dont need these variables actually, we just need to call ProcessSingleRayInModel to determine if the line of sight to the
            // light source is blocked or not
            Vector3D lightIntersectPoint, lightReflection;
            Triangle lightIntersectTriangle;
            Colour lightContribution = Colour(0, 0, 0);
            
            // Scale by this so we will hit the current triangle
            Vector3D adjustedIntersect = closestIntersect;
            adjustedIntersect.Scale(0.999);
            
            // If our line of sight to the light source doesnt hit any other object, we can light the current pixel, otherwise it will be dark and in shadow
            if (!ProcessSingleRayInModel(model, modelLength, intersectToLight, adjustedIntersect, &lightIntersectPoint, &lightReflection, &lightIntersectTriangle))
            {
                double angleToLight = intersectToLight.GetAngle(closestReflectedRay);
                double glossFactor = 1.0 / pow(angleToLight, intersectedTriangle.gloss);
                lightContribution = intersectedTriangle.colour;
                lightContribution.Scale(glossFactor);
                lightContribution.Scale(lightSources[i].intensity);
            }
            
            output.Add(lightContribution);
        }
        
        reflections--;
        
        if (reflections >= 0)
        {
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

bool Tracer::ProcessSingleRay(Triangle triangle, Vector3D ray, Vector3D rayOrigin, Vector3D* outIntersectPoint, Vector3D* outReflection)
{
    // Move the triangle so that its position relative to the origin is the same as its position relative to the ray's
    // starting point
    rayOrigin.Scale(-1.0);
    Triangle newTriangle = triangle.TranslateBy(rayOrigin);
    newTriangle.gloss = triangle.gloss;
    newTriangle.colour = triangle.colour;
    
    ray.ToUnitVector();
    
    // Get the normal of the triangle
    Vector3D oneToTwo = newTriangle.p1.PointToPoint(newTriangle.p2);
    Vector3D oneToThree = newTriangle.p1.PointToPoint(newTriangle.p3);
    
    // Compute the cross product to get the normal
    Vector3D normal = oneToTwo.CrossProduct(oneToThree);
    normal.ToUnitVector();
    
    // if Normal . RayDirection = 0, it is parallel so will never hit
    
    double D = normal.DotProduct(ray);
    
    if (D == 0.0)
    {
        return false;
    }
    
    // Get the distance from the plane to the origin
    double d = -newTriangle.p1.DotProduct(normal);
    double t = -d / D;
    
    // Put in a small nominal value to account for rounding errors and ensure we dont hit the surface we are coming off of
    // The more proper solution here is probably to remove the original triangle from the model
    if (t <= 0.0001)
    {
        return false;
    }
    else
    {
        // Scale the ray by t to get the exact point of intersection
        *outIntersectPoint = ray;
        outIntersectPoint->Scale(t);
        
        // Get angle between all 3 and see if they sum to 360
        Vector3D iToOne = outIntersectPoint->PointToPoint(newTriangle.p1);
        Vector3D iToTwo = outIntersectPoint->PointToPoint(newTriangle.p2);
        Vector3D iToThree = outIntersectPoint->PointToPoint(newTriangle.p3);
        
        double sumOfAngles = iToOne.GetAngle(iToTwo) + iToTwo.GetAngle(iToThree) + iToThree.GetAngle(iToOne);
        
        // Add a little relief angle to handle rounding errors
        if (sumOfAngles >= (2 * M_PI - 0.001))
        {
            *outReflection = outIntersectPoint->GetReflection(normal);
            outReflection->ToUnitVector();
           
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool Tracer::ProcessSingleRayInModel(Triangle *model, int modelLength, Vector3D ray, Vector3D rayOrigin, Vector3D *outIntersectPoint, Vector3D *outReflection, Triangle* outIntersectedTriangle)
{
    Vector3D reflectedRay;
    Vector3D intersect;
    bool hasIntersect = false;
    int modelIterator = 0;
    
    while (modelIterator < modelLength)
    {
        if (this->ProcessSingleRay(model[modelIterator], ray, rayOrigin, &intersect, &reflectedRay))
        {
            if (!hasIntersect || intersect.GetMagnitude() < outIntersectPoint->GetMagnitude())
            {
                hasIntersect = true;
                *outIntersectPoint = intersect;
                *outReflection = reflectedRay;
                *outIntersectedTriangle = model[modelIterator];
            }
        }
        
        modelIterator++;
    }
    
    return hasIntersect;
}