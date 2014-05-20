//
//  Tracer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Tracer.h"

Tracer::Tracer()
{
    // Do nothing
}

Tracer::~Tracer()
{
    // Do nothing
}

OutputRasterizer* Tracer::Render(Triangle *model, int modelLength, int viewAngleX, int xSpan, int ySpan)
{
    Vector3D** projectedRays = projectionUtils::GetProjection(90, xSpan, ySpan);
    
    OutputRasterizer* output = new OutputRasterizer(xSpan, ySpan);

    for(int i = 0; i < xSpan; i++)
    {
        for(int j = 0; j < ySpan; j++)
        {
            if(this->IsHit(*model, projectedRays[i + j * xSpan]))
            {
                output->SetOutput(i, j, 255, 0, 0);
            }
            else
            {
                output->SetOutput(i, j, 0, 0, 0);
            }
        }
    }

    return output;
}

bool Tracer::IsHit(Triangle triangle, Vector3D* ray)
{
    // Get the normal of the triangle
    double normalX, normalY, normalZ;
    Vector3D* oneToTwo = PointToPoint(triangle.p1, triangle.p2);
    Vector3D* oneToThree = PointToPoint(triangle.p1, triangle.p3);
    
    // Compute the cross product to get the normal
    normalX = oneToTwo->getY() * oneToThree->getZ() - oneToTwo->getZ() * oneToThree->getY();
    normalY = oneToTwo->getZ() * oneToThree->getX() - oneToTwo->getX() * oneToThree->getZ();
    normalZ = oneToTwo->getX() * oneToThree->getY() - oneToTwo->getY() * oneToThree->getX();
    
    delete oneToTwo;
    delete oneToThree;
    
    Vector3D* normal = new Vector3D(normalX, normalY, normalZ);
    
    // if Normal . RayDirection = 0, it is parallel so will never hit
    
    double D = DotProduct(normal, ray);
    
    if (D == 0.0)
    {
        delete normal;
        return false;
    }
    
    // Get the distance from the plane to the origin
    double d = -DotProduct(triangle.p1, normal);
    double t = -d / D;

    delete normal;
    
    if (t < 0.0)
    {
        return false;
    }
    else
    {
        // Get angle between all 3 and see if they sum to 360
        Vector3D* intersectionPoint = new Vector3D(ray->getX() * t, ray->getY() * t, ray->getZ() * t);
        Vector3D* iToOne = PointToPoint(intersectionPoint, triangle.p1);
        Vector3D* iToTwo = PointToPoint(intersectionPoint, triangle.p2);
        Vector3D* iToThree = PointToPoint(intersectionPoint, triangle.p3);
        
        double sumOfAngles = GetAngle(iToOne, iToTwo) + GetAngle(iToTwo, iToThree) + GetAngle(iToThree, iToOne);
        
        delete iToOne;
        delete iToTwo;
        delete iToThree;
        delete intersectionPoint;
        
        // Add a little relief angle to handle rounding errors
        if (sumOfAngles >= (2 * M_PI - 0.001))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

double Tracer::DotProduct(Vector3D *v1, Vector3D *v2)
{
    return v1->getX() * v2->getX() + v1->getY() * v2->getY() + v1->getZ() * v2->getZ();
}

Vector3D* Tracer::PointToPoint(Vector3D* point1, Vector3D* point2)
{
    return new Vector3D(point2->getX() - point1->getX(), point2->getY() - point1->getY(), point2->getZ() - point1->getZ());
}

double Tracer::GetMagnitude(Vector3D* v)
{
    return sqrt(pow(v->getX(), 2.0) + pow(v->getY(), 2.0) + pow(v->getZ(), 2.0));
}

double Tracer::GetAngle(Vector3D *v1, Vector3D *v2)
{
    return acos(DotProduct(v1, v2) / (GetMagnitude(v1) * GetMagnitude(v2)));
}