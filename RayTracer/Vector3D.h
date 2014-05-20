//
//  Vector3D.h
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__Vector3D__
#define __RayTracer__Vector3D__

#include <iostream>

class Vector3D
{
public:
    Vector3D(double x, double y, double z);
    ~Vector3D();
    double getX();
    double getY();
    double getZ();
private:
    double x;
    double y;
    double z;
};

#endif /* defined(__RayTracer__Vector3D__) */

