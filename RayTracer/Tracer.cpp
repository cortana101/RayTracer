//
//  Tracer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Tracer.h"

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
            Vector3D ray = projectionUtils::GetProjection(90, xSpan, ySpan, i, j);
 
            Vector3D reflectedRay;
            Vector3D closestReflectedRay;
            Vector3D intersect;
            Vector3D closestIntersect;
            Triangle intersectedTriangle;
            bool hasIntersect = false;
            int modelIterator = 0;
            
            while (modelIterator < modelLength)
            {
                if (this->ProcessSingleRay(model[modelIterator], ray, &intersect, &reflectedRay))
                {
                    if (hasIntersect)
                    {
                        if (intersect.GetMagnitude() < closestIntersect.GetMagnitude())
                        {
                            closestIntersect = intersect;
                            closestReflectedRay = reflectedRay;
                            intersectedTriangle = model[modelIterator];
                        }
                    }
                    else
                    {
                        hasIntersect = true;
                        closestIntersect = intersect;
                        closestReflectedRay = reflectedRay;
                        intersectedTriangle = model[modelIterator];
                    }
                }
                
                modelIterator++;
            }
           
            if(hasIntersect)
            {
                Vector3D intersectToLight = closestIntersect.PointToPoint(*lightSources[0].position);
                double angleToLight = intersectToLight.GetAngle(closestReflectedRay);
                double glossFactor = 1.0 / pow(angleToLight, intersectedTriangle.gloss);
                Colour colour = intersectedTriangle.colour;
                colour.Scale(glossFactor);
                
                output.SetOutput(i, j, colour.rVal, colour.gVal, colour.bVal);
            }
            else
            {
                output.SetOutput(i, j, 0, 0, 0);
            }
        }
    }

    return output;
}

bool Tracer::ProcessSingleRay(Triangle triangle, Vector3D ray, Vector3D* intersectPoint, Vector3D* outReflection)
{
    ray.ToUnitVector();
    
    // Get the normal of the triangle
    Vector3D oneToTwo = triangle.p1.PointToPoint(triangle.p2);
    Vector3D oneToThree = triangle.p1.PointToPoint(triangle.p3);
    
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
    double d = -triangle.p1.DotProduct(normal);
    double t = -d / D;
    
    if (t < 0.0)
    {
        return false;
    }
    else
    {
        // Scale the ray by t to get the exact point of intersection
        *intersectPoint = ray;
        intersectPoint->Scale(t);
        
        // Get angle between all 3 and see if they sum to 360
        Vector3D iToOne = intersectPoint->PointToPoint(triangle.p1);
        Vector3D iToTwo = intersectPoint->PointToPoint(triangle.p2);
        Vector3D iToThree = intersectPoint->PointToPoint(triangle.p3);
        
        double sumOfAngles = iToOne.GetAngle(iToTwo) + iToTwo.GetAngle(iToThree) + iToThree.GetAngle(iToOne);
        
        // Add a little relief angle to handle rounding errors
        if (sumOfAngles >= (2 * M_PI - 0.001))
        {
            *outReflection = intersectPoint->GetReflection(normal);
            outReflection->ToUnitVector();
           
            return true;
        }
        else
        {
            return false;
        }
    }
}