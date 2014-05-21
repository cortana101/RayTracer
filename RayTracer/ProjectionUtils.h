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
    /// \param xSpan The number of pixels in the X plane
    /// \param ySpan The number of pixels in the Y plane
    /// \param xCoord The x coordinate of the pixel we want to get a ray for
    /// \param yCoord The y coordinate of the pixel we want to get a ray for
    /// \returns A vector corresponding to the ray going from the origin through the provided coordinates
    Vector3D GetProjection(int viewingAngleX, int xSpan, int ySpan, int xCoord, int yCoord);
};

#endif /* defined(__RayTracer__ProjectionUtils__) */

