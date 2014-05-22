//
//  Triangle.h
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__Triangle__
#define __RayTracer__Triangle__

#include <iostream>
#include "OutputRasterizer.h"
#include "Vector3D.h"
#include "Colour.h"

/// Represents a triangle in 3d space
class Triangle
{
public:
    Triangle();
    ~Triangle();
    void TranslateBy(Vector3D movement);
    
    Vector3D p1;
    Vector3D p2;
    Vector3D p3;
    
    /// Determines how glossy an object is, 0 = matt, 1 = full gloss
    float gloss;
 
    /// Determines the natural colour of the object
    Colour colour;
};


#endif /* defined(__RayTracer__Triangle__) */
