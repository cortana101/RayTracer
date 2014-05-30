//
//  ModelContainerNode.h
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef RayTracer_ModelContainerNode_h
#define RayTracer_ModelContainerNode_h

#include "Vector3D.h"
#include "ModelObject.h"

class ModelContainerNode
{
public:
    /// Adds an item to the node
    virtual bool AddItem(ModelObject* object) = 0;
};

#endif
