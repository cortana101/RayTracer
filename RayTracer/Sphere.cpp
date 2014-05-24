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

bool Sphere::ProcessRay(Vector3D ray, Vector3D rayOrigin, Vector3D *outIntersectPoint, Vector3D *outNormalizedNormal, Vector3D *outReflection)
{
    rayOrigin.Scale(-1.0);
    Sphere object = this->TranslateBy(rayOrigin);
    Vector3D centerToOrigin = object.center;
    centerToOrigin.Scale(-1.0);
    
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

    *outIntersectPoint = ray;
    
    if (t1 < t2 && t1 > 0.0)
    {
        outIntersectPoint->Scale(t1);
    }
    else if (t2 < t1 && t2 > 0.0)
    {
        outIntersectPoint->Scale(t2);
    }
    else
    {
        return false;
    }

    // At this point the intersect point is not the true intersect, it is the intersect after we reset the ray origin to the planar origin
    *outNormalizedNormal = this->center.PointToPoint(*outIntersectPoint);
    outNormalizedNormal->ToUnitVector();
    
    // Add the ray origin to get the intersect in the normal plane
    *outIntersectPoint = outIntersectPoint->Add(rayOrigin);
    
    *outReflection = outIntersectPoint->GetReflection(*outNormalizedNormal);
    
    return true;
}

