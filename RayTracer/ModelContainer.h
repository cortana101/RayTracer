//
//  ModelContainer.h
//  RayTracer
//
//  Created by Daniel Shih on 5/28/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__ModelContainer__
#define __RayTracer__ModelContainer__

#include <iostream>
#include "Triangle.h"
#include "ModelContainerNode.h"
#include "ModelContainerLeaf.h"

/// Basic k-d tree implementation for partitioning our model space for faster searching by rays
class ModelContainer
{
public:
    ModelContainer(ModelObject** model, int modelLength);
    ~ModelContainer();
    ModelContainerNode* root;
    void AddItem(Triangle *newObject);
    bool TryGetIntersection(Vector3D ray, Vector3D rayOrigin, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties);
};

#endif /* defined(__RayTracer__ModelContainer__) */
