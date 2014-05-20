//
//  Tracer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Tracer.h"

using namespace VectorUtils;

Tracer::Tracer()
{
    // Do nothing
}

Tracer::~Tracer()
{
    // Do nothing
}

OutputRasterizer* Tracer::Render(Triangle *model, int modelLength, LightSource* lightSources, int lightSourceLength, int viewAngleX, int xSpan, int ySpan)
{
    Vector3D** projectedRays = projectionUtils::GetProjection(90, xSpan, ySpan);
    
    Vector3D* reflectedRay;
    
    OutputRasterizer* output = new OutputRasterizer(xSpan, ySpan);

    for(int i = 0; i < xSpan; i++)
    {
        for(int j = 0; j < ySpan; j++)
        {
            Vector3D* intersect = this->ProcessSingleRay(*model, projectedRays[i + j * xSpan], &reflectedRay);
            
            if(intersect != nullptr)
            {
                Vector3D* intersectToLight = PointToPoint(intersect, lightSources[0].position);
                double angleToLight = GetAngle(intersectToLight, reflectedRay);
                
                output->SetOutput(i, j, 255 - angleToLight * 150, 0, 0);
            }
            else
            {
                output->SetOutput(i, j, 0, 0, 0);
            }
        }
    }

    return output;
}

Vector3D* Tracer::ProcessSingleRay(Triangle triangle, Vector3D* ray, Vector3D** outReflection)
{
    ToUnitVector(&ray);
    
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
    ToUnitVector(&normal);
    
    // if Normal . RayDirection = 0, it is parallel so will never hit
    
    double D = DotProduct(normal, ray);
    
    if (D == 0.0)
    {
        delete normal;
        return nullptr;
    }
    
    // Get the distance from the plane to the origin
    double d = -DotProduct(triangle.p1, normal);
    double t = -d / D;
    
    if (t < 0.0)
    {
        return nullptr;
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
        
        // Add a little relief angle to handle rounding errors
        if (sumOfAngles >= (2 * M_PI - 0.001))
        {
            *outReflection = GetReflection(intersectionPoint, normal);
            ToUnitVector(outReflection);
            
            delete normal;
            
            return intersectionPoint;
        }
        else
        {
            delete normal;
            return nullptr;
        }
    }
}