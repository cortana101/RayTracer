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

Triangle::Triangle(Vector3D p1, Vector3D p2, Vector3D p3, Colour colour, double gloss)
{
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
    this->colour = colour;
    this->gloss = gloss;
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

bool Triangle::ProcessRay(Vector3D ray, Vector3D rayOrigin, IntersectProperties *outIntersectProperties)
{
    // Move the triangle so that its position relative to the origin is the same as its position relative to the ray's
    // starting point
    rayOrigin = rayOrigin.Scale(-1.0);
    Triangle newTriangle = this->TranslateBy(rayOrigin);
    newTriangle.gloss = this->gloss;
    newTriangle.colour = this->colour;
    
    ray = ray.ToUnitVector();
    
    // Get the normal of the triangle
    Vector3D oneToTwo = newTriangle.p1.PointToPoint(newTriangle.p2);
    Vector3D oneToThree = newTriangle.p1.PointToPoint(newTriangle.p3);
    
    // Compute the cross product to get the normal
    outIntersectProperties->normalizedNormal = oneToTwo.CrossProduct(oneToThree).ToUnitVector();

    // If the normal is in the same general direction as the ray, it means the normal is pointing
    // away from the viewer, in that case we need to reverse the normal because a lot of other subsequent
    // calculations depend on the normal pointing towards the viewer
    if (outIntersectProperties->normalizedNormal.DotProduct(ray) > 0.0)
    {
        outIntersectProperties->normalizedNormal = outIntersectProperties->normalizedNormal.Scale(-1.0);
    }
    
    // if Normal . RayDirection = 0, it is parallel so will never hit
    
    double D = outIntersectProperties->normalizedNormal.DotProduct(ray);
    
    if (D == 0.0)
    {
        return false;
    }
    
    // Get the distance from the plane to the origin
    double d = -newTriangle.p1.DotProduct(outIntersectProperties->normalizedNormal);
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
        outIntersectProperties->intersectPosition = ray.Scale(t);
        
        // Get angle between all 3 and see if they sum to 360, if they do then the intersect is in the triangle, otherwise its not
        Vector3D iToOne = outIntersectProperties->intersectPosition.PointToPoint(newTriangle.p1);
        Vector3D iToTwo = outIntersectProperties->intersectPosition.PointToPoint(newTriangle.p2);
        Vector3D iToThree = outIntersectProperties->intersectPosition.PointToPoint(newTriangle.p3);
        
        double sumOfAngles = iToOne.GetAngle(iToTwo) + iToTwo.GetAngle(iToThree) + iToThree.GetAngle(iToOne);
        
        // Add a little relief angle to handle rounding errors
        if (sumOfAngles >= (2 * M_PI - 0.001))
        {
            outIntersectProperties->normalizedReflection = outIntersectProperties->intersectPosition.GetReflection(outIntersectProperties->normalizedNormal);
            outIntersectProperties->normalizedReflection = outIntersectProperties->normalizedReflection.ToUnitVector();
            
            // Shift the intersect back to world space
            rayOrigin = rayOrigin.Scale(-1.0);
            outIntersectProperties->intersectPosition = outIntersectProperties->intersectPosition.Add(rayOrigin);
            
            return true;
        }
        else
        {
            return false;
        }
    }
}

Vector3D Triangle::GetNominalPosition()
{
    // For now just let the nominal position be one of the existing vertices
    return this->p1;
}

bool Triangle::IntersectsWith(Triangle otherTriangle)
{
    // To determine if 2 triangles intersect each other, we just need to treat each edge of each triangle
    // as a ray starting at its vertex, and see if that ray intersects the other triangle.
    
    return otherTriangle.IntersectsWithEdge(this->p1, this->p2) ||
            otherTriangle.IntersectsWithEdge(this->p2, this->p3) ||
            otherTriangle.IntersectsWithEdge(this->p3, this->p1) ||
            this->IntersectsWithEdge(otherTriangle.p1, otherTriangle.p2) ||
            this->IntersectsWithEdge(otherTriangle.p2, otherTriangle.p3) ||
            this->IntersectsWithEdge(otherTriangle.p3, otherTriangle.p1);
}

bool Triangle::IntersectsWithEdge(Vector3D v1, Vector3D v2)
{
    Vector3D edgeVector = v1.PointToPoint(v2);
    
    IntersectProperties outIntersectProperties;
    
    if (this->ProcessRay(edgeVector, v1, &outIntersectProperties))
    {
        // If process ray determines there is a hit, we still need to see if the hit is within the bounds of the edge
        // Its possible that the hit is at a position longer than the edge in which case there isn't actually a hit
        // To find out, get the Dot product of the edge vector and the vector from the intersect position to v2. If the dot
        // product is positive it means the intersect is within the edge, if its negative it means its beyond the edge
        
        Vector3D intersectToV2 = outIntersectProperties.intersectPosition.PointToPoint(v2);
        
        if (edgeVector.DotProduct(intersectToV2) >= 0.0)
        {
            return true;
        }
    }
    
    return false;
}

double Triangle::SurfaceArea()
{
    Vector3D v1 = this->p1.PointToPoint(this->p2);
    Vector3D v2 = this->p1.PointToPoint(this->p3);
    
    double angle = v1.GetAngle(v2);
    
    // The height of the triangle if v2 is considered the base
    double triangleHeight = v1.GetMagnitude() * sin(angle);
    
    return triangleHeight * v2.GetMagnitude() / 2;
}