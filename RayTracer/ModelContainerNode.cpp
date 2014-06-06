//
//  ModelContainerNode.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/30/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainerNode.h"


bool ModelContainerNode::Intersects(BoundingBox boundingBox, Triangle object)
{
    Polygon objectPolygon (object);
    return objectPolygon.Intersects(boundingBox);
}