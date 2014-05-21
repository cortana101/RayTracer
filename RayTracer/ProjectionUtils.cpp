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
    
    Vector3D GetProjection(int viewingAngleX, int xSpan, int ySpan, int xCoord, int yCoord)
    {
        // Set an arbitrary Z value on our vector, it doesnt really matter what this is
        // as long as its some positive number
        double z = 2.0;
        double xSpanLength = z * tan(ConvertDegreesToRadians((double)viewingAngleX / 2)) * 2;
        double ySpanLength = xSpanLength * ySpan / xSpan;
        
        Vector3D topLeftCorner(-xSpanLength / 2, ySpanLength / 2, z);
        
        return Vector3D(-xSpanLength / 2 + xCoord * (xSpanLength / xSpan), ySpanLength / 2 - yCoord * (ySpanLength / ySpan), z);
    }
    
    static double ConvertDegreesToRadians(double degrees)
    {
        return  (degrees / 180) * M_PI;
    }
}