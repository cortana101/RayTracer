//
//  ProjectionUtils.h
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__ProjectionUtils__
#define __RayTracer__ProjectionUtils__

#include <iostream>
#include "Vector3D.h"

namespace projectionUtils
{
    /// Gets a list of vectors that correspond to the projection from origin
    /// to each pixel when we get the viewport of the provided viewing angle and
    /// lay out the pixels at that angle
    /// \param viewAngleX The viewing angle in degrees
    /// \param x The number of pixels in the X plane
    /// \param y The number of pixels in the Y plane
    /// \returns A list of vectors of length X*Y that contains all the vectors to all of the given pixels
    Vector3D** GetProjection(int viewingAngleX, int x, int y);
};

#endif /* defined(__RayTracer__ProjectionUtils__) */

