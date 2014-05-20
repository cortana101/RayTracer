//
//  VectorUtils.h
//  RayTracer
//
//  Created by Daniel Shih on 5/19/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef RayTracer_VectorUtils_h
#define RayTracer_VectorUtils_h

#include "Vector3D.h"

namespace VectorUtils
{
    /// Computes the dot product of the 2 provided vectors
    double DotProduct(Vector3D *v1, Vector3D *v2);
    
    /// Get a vector that goes from point 1 to point 2
    Vector3D* PointToPoint(Vector3D* point1, Vector3D* point2);
    
    /// Gets the magnitude of a vector
    double GetMagnitude(Vector3D* v);
    
    /// Gets the angle between 2 vectors in radians
    double GetAngle(Vector3D *v1, Vector3D *v2);
    
    /// Converts the provided vector to a unit vector
    void ToUnitVector(Vector3D** v);
    
    /// Gets a unit vector in the direction of the reflected ray given
    /// the incident ray and the normal
    Vector3D* GetReflection(Vector3D* incidentRay, Vector3D* normal);
    
    /// Creates a copy of the vector in memory
    Vector3D* Clone(Vector3D* v);
}

#endif
