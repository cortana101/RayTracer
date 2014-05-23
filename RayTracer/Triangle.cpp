//
//  Triangle.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/22/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Triangle.h"

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