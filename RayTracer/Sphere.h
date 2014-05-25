//
//  Sphere.h
//  RayTracer
//
//  Created by Daniel Shih on 5/23/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__Sphere__
#define __RayTracer__Sphere__

#include <iostream>
#include "Vector3D.h"
#include "ModelObject.h"

class Sphere : public ModelObject
{
public:
    Sphere();
    Sphere(Vector3D center, double radius);
    ~Sphere();
    Sphere TranslateBy(Vector3D movement);
    virtual bool ProcessRay(Vector3D ray, Vector3D rayOrigin, IntersectProperties* outIntersectProperties);
    
private:
    Vector3D center;
    double radius;
};

#endif /* defined(__RayTracer__Sphere__) */
