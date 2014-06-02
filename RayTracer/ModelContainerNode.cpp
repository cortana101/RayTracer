//
//  ModelContainerNode.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/30/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainerNode.h"

ModelContainerNode::ModelContainerNode(BoundingBox boundingBox) : boundingBox(boundingBox)
{
}

ModelContainerNode::~ModelContainerNode()
{
    // Do nothing
}

bool ModelContainerNode::Intersects(Triangle object)
{
    return this->boundingBox.Intersects(object);
}