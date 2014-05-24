//
//  Triangle.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/22/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Triangle.h"
#include <math.h>

Triangle::Triangle()
{
    // do nothing
}

Triangle::~Triangle()
{
    // do nothing
}

Triangle Triangle::TranslateBy(Vector3D movement)
{
    Triangle output;
    output.p1 = this->p1.Add(movement);
    output.p2 = this->p2.Add(movement);
    output.p3 = this->p3.Add(movement);
    
    return output;
}

bool Triangle::ProcessRay(Vector3D ray, Vector3D rayOrigin, Vector3D *outIntersectPoint, Vector3D *outNormalizedNormal, Vector3D *outReflection)
{
    // Move the triangle so that its position relative to the origin is the same as its position relative to the ray's
    // starting point
    rayOrigin.Scale(-1.0);
    Triangle newTriangle = this->TranslateBy(rayOrigin);
    newTriangle.gloss = this->gloss;
    newTriangle.colour = this->colour;
    
    ray.ToUnitVector();
    
    // Get the normal of the triangle
    Vector3D oneToTwo = newTriangle.p1.PointToPoint(newTriangle.p2);
    Vector3D oneToThree = newTriangle.p1.PointToPoint(newTriangle.p3);
    
    // Compute the cross product to get the normal
    *outNormalizedNormal = oneToTwo.CrossProduct(oneToThree);
    outNormalizedNormal->ToUnitVector();

    // If the normal is in the same general direction as the ray, it means the normal is pointing
    // away from the viewer, in that case we need to reverse the normal because a lot of other subsequent
    // calculations depend on the normal pointing towards the viewer
    if (outNormalizedNormal->DotProduct(ray) > 0.0)
    {
        outNormalizedNormal->Scale(-1.0);
    }
    
    // if Normal . RayDirection = 0, it is parallel so will never hit
    
    double D = outNormalizedNormal->DotProduct(ray);
    
    if (D == 0.0)
    {
        return false;
    }
    
    // Get the distance from the plane to the origin
    double d = -newTriangle.p1.DotProduct(*outNormalizedNormal);
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
            *outReflection = outIntersectPoint->GetReflection(*outNormalizedNormal);
            outReflection->ToUnitVector();
            
            // Shift the intersect back to world space
            rayOrigin.Scale(-1.0);
            *outIntersectPoint = outIntersectPoint->Add(rayOrigin);
            
            return true;
        }
        else
        {
            return false;
        }
    }

}