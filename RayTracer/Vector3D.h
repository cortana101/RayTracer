//
//  Vector3D.h
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__Vector3D__
#define __RayTracer__Vector3D__

#include <iostream>

class Vector3D
{
public:
    Vector3D();
    Vector3D(double x, double y, double z);
    ~Vector3D();
    /// Computes the dot product of the 2 provided vectors
    double DotProduct(Vector3D v2);
    
    /// Get a vector that goes from point 1 to point 2
    Vector3D PointToPoint(Vector3D point2);
    
    /// Gets the magnitude of a vector
    double GetMagnitude();
    
    /// Gets the angle between 2 vectors in radians
    double GetAngle(Vector3D v2);
    
    /// Converts the provided vector to a unit vector
    void ToUnitVector();
    
    /// Gets a unit vector in the direction of the reflected ray given
    /// the incident ray and the normal
    Vector3D GetReflection(Vector3D normal);
    
    /// Creates a copy of the vector in memory
    Vector3D* Clone();
    
    /// Scales the current vector by the provided factor
    void Scale(double scaleFactor);
    
    /// Gets the cross product between the current vector and the provided vector
    Vector3D CrossProduct(Vector3D v2);
private:
    double x;
    double y;
    double z;
};

#endif /* defined(__RayTracer__Vector3D__) */

