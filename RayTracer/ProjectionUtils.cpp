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
    
    AARayBundle GetProjectionWithAA(int viewingAngleX, int xSpan, int ySpan, int xCoord, int yCoord)
    {
        Vector3D center = GetProjection(viewingAngleX, xSpan, ySpan, xCoord, yCoord);
        
        double z = 2.0;
        double xSpanLength = z * tan(ConvertDegreesToRadians((double)viewingAngleX / 2)) * 2;
        
        // xSpanLength / xspan is the width per pixel, divide by 3 so we spread the rays out evenly between pixels
        double perPixelSpan = (xSpanLength / xSpan) / 3;
        
        Vector3D topLeft = center.Add(Vector3D(-perPixelSpan, perPixelSpan, 0.0));
        Vector3D topRight = center.Add(Vector3D(perPixelSpan, perPixelSpan, 0.0));
        Vector3D bottomLeft = center.Add(Vector3D(-perPixelSpan, -perPixelSpan, 0.0));
        Vector3D bottomRight = center.Add(Vector3D(perPixelSpan, -perPixelSpan, 0.0));
        
        AARayBundle rayBundle { center, topLeft, topRight, bottomLeft, bottomRight };
        return rayBundle;
    }
    
    static double ConvertDegreesToRadians(double degrees)
    {
        return  (degrees / 180) * M_PI;
    }
}