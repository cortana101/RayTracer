//
//  Vector3D.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Vector3D.h"
#include <math.h>

Vector3D::Vector3D()
{
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
}

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

double Vector3D::DotProduct(Vector3D v2)
{
    return this->x * v2.x + this->y * v2.y + this->z * v2.z;
}

Vector3D Vector3D::PointToPoint(Vector3D point2)
{
    return Vector3D(point2.x - this->x, point2.y - this->y, point2.z - this->z);
}

double Vector3D::GetMagnitude()
{
    return sqrt(pow(this->x, 2.0) + pow(this->y, 2.0) + pow(this->z, 2.0));
}

double Vector3D::GetAngle(Vector3D v2)
{
    double angle = acos(DotProduct(v2) / (GetMagnitude() * v2.GetMagnitude()));
    return isnan(angle) ? 0.0 : angle;
}

Vector3D Vector3D::ToUnitVector()
{
    double magnitude = GetMagnitude();
    return this->Scale(1 / magnitude);
}

Vector3D Vector3D::Scale(double scaleFactor)
{
    return Vector3D(this->x * scaleFactor, this->y * scaleFactor, this->z * scaleFactor);
}

Vector3D Vector3D::Add(Vector3D v2)
{
    return Vector3D(this->x + v2.x, this->y + v2.y, this->z + v2.z);
}

Vector3D Vector3D::GetReflection(Vector3D normal)
{
    double thisDotProductNormal = this->DotProduct(normal);
    
    double xComponent = this->x - 2 * thisDotProductNormal * normal.x;
    double yComponent = this->y - 2 * thisDotProductNormal * normal.y;
    double zComponent = this->z - 2 * thisDotProductNormal * normal.z;
    
    return Vector3D(xComponent, yComponent, zComponent);
}

Vector3D* Vector3D::Clone()
{
    return new Vector3D(this->x, this->y, this->z);
}

Vector3D Vector3D::CrossProduct(Vector3D v2)
{
    double normalX = this->y * v2.z - this->z * v2.y;
    double normalY = this->z * v2.x - this->x * v2.z;
    double normalZ = this->x * v2.y - this->y * v2.x;
    
    return Vector3D(normalX, normalY, normalZ);
}
