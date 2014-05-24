//
//  ModelObject.h
//  RayTracer
//
//  Created by Daniel Shih on 5/23/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__ModelObject__
#define __RayTracer__ModelObject__

#include <iostream>
#include "Vector3D.h"
#include "Colour.h"

class ModelObject
{
public:
    //ModelObject();
    //~ModelObject();
    virtual bool ProcessRay(Vector3D ray, Vector3D rayOrigin, Vector3D* outIntersectPoint, Vector3D* outNormalizedNormal, Vector3D* outReflection) = 0;
    
    /// The Phong glossiness number, Determines how glossy something is, 0 = matt, > 0 = more glossy
    float gloss;
    
    /// Determines the natural colour of the object
    Colour colour;
};

#endif /* defined(__RayTracer__ModelObject__) */
