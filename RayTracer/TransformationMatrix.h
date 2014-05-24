//
//  TransformationMatrix.h
//  RayTracer
//
//  Created by Daniel Shih on 5/23/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__TransformationMatrix__
#define __RayTracer__TransformationMatrix__

#include "Vector3D.h"
#include <iostream>

class TransformationMatrix
{
public:
    TransformationMatrix();
    TransformationMatrix(double v11, double v12, double v13, double v21, double v22, double v23, double v31, double v32, double v33);
    ~TransformationMatrix();
    
    /// Transforms the given vector by the current transformation matrix
    Vector3D TransformVector(Vector3D v);
private:
    double v11;
    double v12;
    double v13;
    double v21;
    double v22;
    double v23;
    double v31;
    double v32;
    double v33;
};

#endif /* defined(__RayTracer__TransformationMatrix__) */
