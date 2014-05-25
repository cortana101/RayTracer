//
//  IntersectProperties.h
//  RayTracer
//
//  Created by Daniel Shih on 5/24/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__IntersectProperties__
#define __RayTracer__IntersectProperties__

#include <iostream>
#include "Vector3D.h"

class IntersectProperties
{
public:
    Vector3D intersectPosition;
    Vector3D normalizedNormal;
    Vector3D normalizedReflection;
};

#endif /* defined(__RayTracer__IntersectProperties__) */
