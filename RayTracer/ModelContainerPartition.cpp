//
//  ModelContainerPartition.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainerPartition.h"

bool ModelContainerPartition::AddItem(ModelObject *object)
{
    /// Add an item to the current node recursively, if we added an item successfull we return true
    /// otherwise we return false
    
    // Get the nominal position for the purposes of finding which leaf node to go into first
    // then if the object is bigger than the space bounded by the leaf node, we return up the
    // stack of nodes and recursively add it to the neighbouring nodes as necessary until we have
    // entirely contained the object
    Vector3D nominalPostion = object->GetNominalPosition();
    
    double nominalPositionInPartitionPlane = 0.0;
    
    if (this->partitionPlane == PartitionPlaneType::X)
    {
        nominalPositionInPartitionPlane = nominalPostion.x;
    }
    else if (this->partitionPlane == PartitionPlaneType::Y)
    {
        nominalPositionInPartitionPlane = nominalPostion.y;
    }
    else
    {
        nominalPositionInPartitionPlane = nominalPostion.z;
    }
    
    if (nominalPositionInPartitionPlane > this->partitionPosition)
    {
        return this->posChild->AddItem(object);
    }
    else
    {
        return this->negChild->AddItem(object);
    }
    
    return false;
}