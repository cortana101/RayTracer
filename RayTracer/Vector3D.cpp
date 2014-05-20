//
//  Vector3D.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Vector3D.h"

Vector3D::Vector3D(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vector3D::~Vector3D()
{
    // do nothing, we have no pointers in this type.
}

double Vector3D::getX()
{
    return this->x;
}

double Vector3D::getY()
{
    return this->y;
}

double Vector3D::getZ()
{
    return this->z;
}