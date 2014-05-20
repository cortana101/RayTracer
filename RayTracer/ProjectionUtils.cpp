//
//  ProjectionUtils.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ProjectionUtils.h"
#include <cmath>

namespace projectionUtils
{
    static double ConvertDegreesToRadians(double degrees);
    
    Vector3D** GetProjection(int viewingAngleX, int x, int y)
    {
        // Set an arbitrary Z value on our vector, it doesnt really matter what this is
        // as long as its some positive number
        double z = 2.0;
        double xSpan = z * tan(ConvertDegreesToRadians((double)viewingAngleX / 2)) * 2;
        double ySpan = xSpan * y / x;
        
        Vector3D topLeftCorner(-xSpan / 2, ySpan / 2, z);
        
        Vector3D** outputBuffer = new Vector3D*[x * y];
        
        for(int i = 0; i < x; i++)
        {
            for(int j = 0; j < y; j++)
            {
                double xComponent = topLeftCorner.getX() + i * (xSpan / x);
                double yComponent = topLeftCorner.getY() - j * (ySpan / y);
                outputBuffer[i + j * x] = new Vector3D(xComponent, yComponent, z);
            }
        }
        
        return outputBuffer;
    }
    
    static double ConvertDegreesToRadians(double degrees)
    {
        return  (degrees / 180) * M_PI;
    }
}