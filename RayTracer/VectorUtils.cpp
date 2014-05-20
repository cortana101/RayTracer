//
//  VectorUtils.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/19/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "VectorUtils.h"
#include <math.h>

namespace VectorUtils
{
    double DotProduct(Vector3D *v1, Vector3D *v2)
    {
        return v1->getX() * v2->getX() + v1->getY() * v2->getY() + v1->getZ() * v2->getZ();
    }
    
    Vector3D* PointToPoint(Vector3D* point1, Vector3D* point2)
    {
        return new Vector3D(point2->getX() - point1->getX(), point2->getY() - point1->getY(), point2->getZ() - point1->getZ());
    }
    
    double GetMagnitude(Vector3D* v)
    {
        return sqrt(pow(v->getX(), 2.0) + pow(v->getY(), 2.0) + pow(v->getZ(), 2.0));
    }
    
    double GetAngle(Vector3D *v1, Vector3D *v2)
    {
        return acos(DotProduct(v1, v2) / (GetMagnitude(v1) * GetMagnitude(v2)));
    }
    
    void ToUnitVector(Vector3D** v)
    {
        double magnitude = GetMagnitude(*v);
        double xComponent = (*v)->getX() / magnitude;
        double yComponent = (*v)->getY() / magnitude;
        double zComponent = (*v)->getZ() / magnitude;
        
        delete *v;
        
        *v = new Vector3D(xComponent, yComponent, zComponent);
    }
    
    Vector3D* GetReflection(Vector3D* incidentRay, Vector3D* normal)
    {
        double xComponent = incidentRay->getX() - 2 * incidentRay->getX() * pow(normal->getX(), 2.0);
        double yComponent = incidentRay->getY() - 2 * incidentRay->getY() * pow(normal->getY(), 2.0);
        double zComponent = incidentRay->getZ() - 2 * incidentRay->getZ() * pow(normal->getZ(), 2.0);
        
        return new Vector3D(xComponent, yComponent, zComponent);
    }
}