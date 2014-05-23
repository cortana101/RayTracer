//
//  LightSource.h
//  RayTracer
//
//  Created by Daniel Shih on 5/19/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef RayTracer_LightSource_h
#define RayTracer_LightSource_h

#include "Vector3D.h"

struct LightSource
{
    Vector3D* position;
    double intensity;
};


#endif
