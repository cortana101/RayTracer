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
#include "ModelObject.h"
#include "ModelContainerNode.h"

#define MAXMODELPERNODE 6

/// Basic k-d tree implementation for partitioning our model space for faster searching by rays
class ModelContainer
{
public:
    ModelContainer();
    ~ModelContainer();
    ModelContainerNode* root;
    //bool TryGetIntersection(Vector3D ray, Vector3D rayOrigin, ModelObject* outIntersectedModel, int* outIntersectedModelIndex);
};

#endif /* defined(__RayTracer__ModelContainer__) */
