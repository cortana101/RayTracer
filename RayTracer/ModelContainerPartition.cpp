//
//  ModelContainerPartition.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainerPartition.h"

ModelContainerPartition::ModelContainerPartition(BoundingBox boundingBox) : ModelContainerNode(boundingBox)
{
    // Do nothing
}

ModelContainerPartition::~ModelContainerPartition()
{
    // Do nothing
}

ModelContainerNode* ModelContainerPartition::AddItem(Triangle *object)
{
    BoundingBox posChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Positive);
    BoundingBox negChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Negative);
    
    // Recursively add the triangle to the child nodes. Note that if the triangle intersects
    // both childs, we add a reference to it in BOTH childs
    if (posChildBoundingBox.Intersects(*object))
    {
        this->posChild = this->posChild->AddItem(object);
    }
    
    if (negChildBoundingBox.Intersects(*object))
    {
        this->negChild = this->negChild->AddItem(object);
    }
    
    return this;
}

ModelContainerNode* ModelContainerPartition::AddItem(Triangle* object, Vector3D nominalPosition, bool* outFullyContainedByNode)
{
    // Ideally we would use a nominal position to avoid having to calculate intersects all the way down the tree
    // calculating intersects is relatively expensive, calculating intersect using a nominal position is much cheaper
    bool fullyContainedByChild = false;
    
    if (this->posChild->boundingBox.Contains(nominalPosition))
    {
        this->posChild = this->posChild->AddItem(object, nominalPosition, &fullyContainedByChild);
        
        if (!fullyContainedByChild && this->negChild->boundingBox.Intersects(*object))
        {
            this->negChild = this->negChild->AddItem(object);
        }
    }
    else
    {
        this->negChild = this->negChild->AddItem(object, nominalPosition, &fullyContainedByChild);
        
        if (!fullyContainedByChild && this->posChild->boundingBox.Intersects(*object))
        {
            this->posChild = this->posChild->AddItem(object);
        }
    }
    
    *outFullyContainedByNode = fullyContainedByChild || this->boundingBox.Contains(*object);
    
    return this;
}

bool ModelContainerPartition::TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties)
{
    double raySearchPositionInPartitionPlane = 0.0;
    
    if (this->partitionPlane == PartitionPlaneType::X)
    {
        raySearchPositionInPartitionPlane = raySearchPosition.x;
    }
    else if (this->partitionPlane == PartitionPlaneType::Y)
    {
        raySearchPositionInPartitionPlane = raySearchPosition.y;
    }
    else
    {
        raySearchPositionInPartitionPlane = raySearchPosition.z;
    }
    
    bool foundHitInChild = false;
    
    Vector3D newRaySearchPosition;
    
    if (raySearchPositionInPartitionPlane > this->partitionPosition)
    {
        foundHitInChild = this->posChild->TraceRay(ray, rayOrigin, raySearchPosition, ignoredObject, outIntersectedModel, outIntersectProperties);
        
        // See if the ray intersects the positive side of negChild's bounding box
        if (!foundHitInChild && this->negChild->boundingBox.TryGetIntersectionAtSurface(ray, rayOrigin, this->partitionPlane, PartitionKeepDirection::Positive, &newRaySearchPosition))
        {
            foundHitInChild = this->negChild->TraceRay(ray, rayOrigin, newRaySearchPosition, ignoredObject, outIntersectedModel, outIntersectProperties);
        }
    }
    else
    {
        foundHitInChild = this->negChild->TraceRay(ray, rayOrigin, raySearchPosition, ignoredObject, outIntersectedModel, outIntersectProperties);
        
        // See if the ray intersects the negative side of posChild's bounding box
        if (!foundHitInChild && this->posChild->boundingBox.TryGetIntersectionAtSurface(ray, rayOrigin, this->partitionPlane, PartitionKeepDirection::Negative, &newRaySearchPosition))
        {
            foundHitInChild = this->posChild->TraceRay(ray, rayOrigin, newRaySearchPosition, ignoredObject, outIntersectedModel, outIntersectProperties);
        }
    }
    
    return foundHitInChild;
}