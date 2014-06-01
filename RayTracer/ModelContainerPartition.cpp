//
//  ModelContainerPartition.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainerPartition.h"

ModelContainerPartition::ModelContainerPartition()
{
    // Do nothing
}

ModelContainerPartition::~ModelContainerPartition()
{
    // Do nothing
}

ModelContainerNode* ModelContainerPartition::AddItem(Triangle *object, BoundingBox boundingBox)
{
    BoundingBox posChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Positive);
    BoundingBox negChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Negative);
    
    // Recursively add the triangle to the child nodes. Note that if the triangle intersects
    // both childs, we add a reference to it in BOTH childs
    if (posChildBoundingBox.Intersects(*object))
    {
        this->posChild = this->posChild->AddItem(object, posChildBoundingBox);
    }
    
    if (negChildBoundingBox.Intersects(*object))
    {
        this->negChild = this->negChild->AddItem(object, negChildBoundingBox);
    }
    
    return this;
}

ModelContainerNode* ModelContainerPartition::AddItem(Triangle* object, BoundingBox boundingBox, Vector3D nominalPosition, bool* outFullyContainedByNode)
{
    // Ideally we would use a nominal position to avoid having to calculate intersects all the way down the tree
    // calculating intersects is relatively expensive, calculating intersect using a nominal position is much cheaper
    
    BoundingBox posChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Positive);
    BoundingBox negChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Negative);
    
    bool fullyContainedByChild = false;
    
    if (posChildBoundingBox.Contains(nominalPosition))
    {
        this->posChild = this->posChild->AddItem(object, posChildBoundingBox, nominalPosition, &fullyContainedByChild);
        
        if (!fullyContainedByChild && negChildBoundingBox.Intersects(*object))
        {
            this->negChild = this->negChild->AddItem(object, negChildBoundingBox);
        }
    }
    else
    {
        this->negChild = this->negChild->AddItem(object, negChildBoundingBox, nominalPosition, &fullyContainedByChild);
        
        if (!fullyContainedByChild && posChildBoundingBox.Intersects(*object))
        {
            this->posChild = this->posChild->AddItem(object, posChildBoundingBox);
        }
    }
    
    if (!fullyContainedByChild)
    {
        *outFullyContainedByNode = boundingBox.Contains(*object);
    }
    
    return this;
}