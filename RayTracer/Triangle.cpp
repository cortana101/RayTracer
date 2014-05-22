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

void Triangle::TranslateBy(Vector3D movement)
{
    this->p1.Add(movement);
    this->p2.Add(movement);
    this->p3.Add(movement);
}