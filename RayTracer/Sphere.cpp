//
//  Sphere.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/23/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Sphere.h"
#include <math.h>

Sphere::Sphere()
{
    // Do nothing
}

Sphere::Sphere(Vector3D center, double radius)
{
    this->center = center;
    this->radius = radius;
}

Sphere::~Sphere()
{
    // Do nothing
}

Sphere Sphere::TranslateBy(Vector3D movement)
{
    return Sphere(this->center.Add(movement), this->radius);
}

bool Sphere::ProcessRay(Vector3D ray, Vector3D rayOrigin, IntersectProperties* outIntersectProperties)
{
    rayOrigin = rayOrigin.Scale(-1.0);
    Sphere object = this->TranslateBy(rayOrigin);
    Vector3D centerToOrigin = object.center.Scale(-1.0);
    
    double a = ray.DotProduct(ray);
    double b = 2 * centerToOrigin.DotProduct(ray);
    double c = centerToOrigin.DotProduct(centerToOrigin) - pow(object.radius, 2.0);
    
    double discriminant = pow(b, 2.0) - 4 * a * c;
    
    if (discriminant < 0.0)
    {
        return false;
    }
    
    double t1 = (-b + pow(discriminant, 0.5)) / (2 * a);
    double t2 = (-b - pow(discriminant, 0.5)) / (2 * a);

    outIntersectProperties->intersectPosition = ray;
    
    if (t1 < t2 && t1 > 0.0)
    {
        outIntersectProperties->intersectPosition = outIntersectProperties->intersectPosition.Scale(t1);
    }
    else if (t2 < t1 && t2 > 0.0)
    {
        outIntersectProperties->intersectPosition = outIntersectProperties->intersectPosition.Scale(t2);
    }
    else
    {
        return false;
    }

    // At this point the intersect point is not the true intersect, it is the intersect after we reset the ray origin to the planar origin
    outIntersectProperties->normalizedNormal = this->center.PointToPoint(outIntersectProperties->intersectPosition);
    outIntersectProperties->normalizedNormal = outIntersectProperties->normalizedNormal.ToUnitVector();
    
    // Add the ray origin to get the intersect in the normal plane
    outIntersectProperties->intersectPosition = outIntersectProperties->intersectPosition.Add(rayOrigin);
    
    outIntersectProperties->normalizedReflection = outIntersectProperties->intersectPosition.GetReflection(outIntersectProperties->normalizedNormal);
    outIntersectProperties->normalizedReflection = outIntersectProperties->normalizedReflection.ToUnitVector();
   
    return true;
}

