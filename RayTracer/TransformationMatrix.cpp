//
//  TransformationMatrix.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/23/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "TransformationMatrix.h"

TransformationMatrix::TransformationMatrix()
{
    // Do nothing
}

TransformationMatrix::~TransformationMatrix()
{
    // Do nothing
}

TransformationMatrix::TransformationMatrix(double v11, double v12, double v13, double v21, double v22, double v23, double v31, double v32, double v33)
{
    this->v11 = v11;
    this->v12 = v12;
    this->v13 = v13;
    this->v21 = v21;
    this->v22 = v22;
    this->v23 = v23;
    this->v31 = v31;
    this->v32 = v32;
    this->v33 = v33;
}

Vector3D TransformationMatrix::TransformVector(Vector3D v)
{
    double xComponent = v.x * v11 + v.y * v12 + v.z * v13;
    double yComponent = v.x * v21 + v.y * v22 + v.z * v23;
    double zComponent = v.x * v31 + v.y * v32 + v.z * v33;
    
    return Vector3D(xComponent, yComponent, zComponent);
}