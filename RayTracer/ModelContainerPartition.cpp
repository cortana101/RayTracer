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

bool ModelContainerPartition::TryAddItem(Triangle *object, BoundingBox boundingBox, ModelContainerNode** threadRegister, int threadId, pthread_mutex_t *threadRegisterMutex, ModelContainerNode** outUpdatedNode)
{
    BoundingBox posChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Positive);
    BoundingBox negChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Negative);
    
    // Recursively add the triangle to the child nodes. Note that if the triangle intersects
    // both childs, we add a reference to it in BOTH childs
    
    if (this->Intersects(posChildBoundingBox, *object))
    {
        this->posChild = this->AddItemWithWait(object, &this->posChild, posChildBoundingBox, threadRegister, threadId, threadRegisterMutex);
    }
    
    if (this->Intersects(negChildBoundingBox, *object))
    {
        this->negChild = this->AddItemWithWait(object, &this->negChild, negChildBoundingBox, threadRegister, threadId, threadRegisterMutex);
    }
    
    ModelContainerNode::UnlockNode(threadRegisterMutex, threadRegister, threadId);
    
    *outUpdatedNode = this;
    return true;
}

bool ModelContainerPartition::TryAddItem(Triangle* object, BoundingBox boundingBox, Vector3D nominalPosition, ModelContainerNode** threadRegister, int threadId, pthread_mutex_t *threadRegisterMutex, bool* outFullyContainedByNode, ModelContainerNode** outUpdatedNode)
{
    // Ideally we would use a nominal position to avoid having to calculate intersects all the way down the tree
    // calculating intersects is relatively expensive, calculating intersect using a nominal position is much cheaper
    
    BoundingBox posChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Positive);
    BoundingBox negChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Negative);
    
    bool fullyContainedByChild = false;
   
    if (posChildBoundingBox.Contains(nominalPosition))
    {
        this->posChild = this->AddItemWithWait(object, &this->posChild, nominalPosition, posChildBoundingBox, threadRegister, threadId, threadRegisterMutex, &fullyContainedByChild);
        
        if (!fullyContainedByChild && this->Intersects(negChildBoundingBox, *object))
        {
            this->negChild = this->AddItemWithWait(object, &this->negChild, negChildBoundingBox, threadRegister, threadId, threadRegisterMutex);
        }
    }
    else
    {
        this->negChild = this->AddItemWithWait(object, &this->negChild, nominalPosition, negChildBoundingBox, threadRegister, threadId, threadRegisterMutex, &fullyContainedByChild);
        
        if (!fullyContainedByChild && this->Intersects(posChildBoundingBox, *object))
        {
            this->posChild = this->AddItemWithWait(object, &this->posChild, posChildBoundingBox, threadRegister, threadId, threadRegisterMutex);
        }
    }
    
    ModelContainerNode::UnlockNode(threadRegisterMutex, threadRegister, threadId);
    
    if (!fullyContainedByChild)
    {
        *outFullyContainedByNode = boundingBox.Contains(*object);
    }
    else
    {
        *outFullyContainedByNode = true;
    }
    
    *outUpdatedNode = this;
    return true;
}

bool ModelContainerPartition::TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, BoundingBox boundingBox, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties, int *outNodesVisited, int *outNumTrianglesVisited)
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
    
    BoundingBox posChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Positive);
    BoundingBox negChildBoundingBox = boundingBox.Constrain(this->partitionPlane, this->partitionPosition, PartitionKeepDirection::Negative);
    
    Vector3D newRaySearchPosition;
    int numNodesVisitedInPosChild = 0;
    int numNodesVisitedInNegChild = 0;
    int numTrianglesVisitedInPosChild = 0;
    int numTrianglesVisitedInNegChild = 0;
    
    if (raySearchPositionInPartitionPlane > this->partitionPosition)
    {
        foundHitInChild = this->posChild->TraceRay(ray, rayOrigin, raySearchPosition, posChildBoundingBox, ignoredObject, outIntersectedModel, outIntersectProperties, &numNodesVisitedInPosChild, &numTrianglesVisitedInPosChild);
        
        // See if the ray intersects the positive side of negChild's bounding box
        if (!foundHitInChild && negChildBoundingBox.TryGetIntersectionAtSurface(ray, rayOrigin, this->partitionPlane, PartitionKeepDirection::Positive, &newRaySearchPosition))
        {
            foundHitInChild = this->negChild->TraceRay(ray, rayOrigin, newRaySearchPosition, negChildBoundingBox, ignoredObject, outIntersectedModel, outIntersectProperties, &numNodesVisitedInNegChild, &numTrianglesVisitedInNegChild);
        }
    }
    else
    {
        foundHitInChild = this->negChild->TraceRay(ray, rayOrigin, raySearchPosition, negChildBoundingBox, ignoredObject, outIntersectedModel, outIntersectProperties, &numNodesVisitedInNegChild, &numTrianglesVisitedInNegChild);
        
        // See if the ray intersects the negative side of posChild's bounding box
        if (!foundHitInChild && posChildBoundingBox.TryGetIntersectionAtSurface(ray, rayOrigin, this->partitionPlane, PartitionKeepDirection::Negative, &newRaySearchPosition))
        {
            foundHitInChild = this->posChild->TraceRay(ray, rayOrigin, newRaySearchPosition, posChildBoundingBox, ignoredObject, outIntersectedModel, outIntersectProperties, &numNodesVisitedInPosChild, &numTrianglesVisitedInPosChild);
        }
    }
    
    *outNodesVisited = numNodesVisitedInPosChild + numNodesVisitedInNegChild + 1; // Plus one for the current node
    *outNumTrianglesVisited = numTrianglesVisitedInPosChild + numTrianglesVisitedInNegChild;
    
    return foundHitInChild;
}

TreeStatistics ModelContainerPartition::GetStatistics(int currentDepth)
{
    TreeStatistics posChildStatistics = this->posChild->GetStatistics(currentDepth + 1);
    TreeStatistics negChildStatistics = this->negChild->GetStatistics(currentDepth + 1);
    
    TreeStatistics currentStatistics;
    currentStatistics.maxDepth = std::max(posChildStatistics.maxDepth, negChildStatistics.maxDepth);
    currentStatistics.numberOfLeafs = posChildStatistics.numberOfLeafs + negChildStatistics.numberOfLeafs;
    currentStatistics.numberOfEmptyLeafs = posChildStatistics.numberOfEmptyLeafs + negChildStatistics.numberOfEmptyLeafs;
    currentStatistics.maxTrianglesLeaf = std::max(posChildStatistics.maxTrianglesLeaf, negChildStatistics.maxTrianglesLeaf);
    
    // We arent actually using this as the average at this point, at this point we're just using it as an accumulator and count
    // the total number of triangles in the tree, we will do the average as the final step once we've traversed the whole tree
    // and have the total number of triangles as well as the total number of leaves
    currentStatistics.averageTrianglesPerLeaf = posChildStatistics.averageTrianglesPerLeaf + negChildStatistics.averageTrianglesPerLeaf;
    
    // Doing the same thing with the average depth
    currentStatistics.averageDepth = posChildStatistics.averageDepth + negChildStatistics.averageDepth;
    
    return currentStatistics;
}

ModelContainerNode* ModelContainerPartition::AddItemWithWait(Triangle* object, ModelContainerNode** targetNode, BoundingBox boundingBox, ModelContainerNode** threadRegister, int threadId, pthread_mutex_t *threadRegisterMutex)
{
    ModelContainerNode* outUpdatedNode;
    
    while(!ModelContainerNode::TryLockNode(threadRegisterMutex, threadRegister, threadId, *targetNode))
    {
        // May want to add a little sleep here, but basially just keep trying until we're able to add it
    }
    
    (*targetNode)->TryAddItem(object, boundingBox, threadRegister, threadId, threadRegisterMutex, &outUpdatedNode);
    
    return outUpdatedNode;
}

ModelContainerNode* ModelContainerPartition::AddItemWithWait(Triangle* object, ModelContainerNode** targetNode, Vector3D nominalPosition, BoundingBox boundingBox, ModelContainerNode** threadRegister, int threadId, pthread_mutex_t *threadRegisterMutex, bool* outFullyContainedByNode)
{
    ModelContainerNode* outUpdatedNode;
    
    while(!ModelContainerNode::TryLockNode(threadRegisterMutex, threadRegister, threadId, *targetNode))
    {
        // May want to add a little sleep here, but basially just keep trying until we're able to add it
    }
    
    (*targetNode)->TryAddItem(object, boundingBox, nominalPosition, threadRegister, threadId, threadRegisterMutex, outFullyContainedByNode, &outUpdatedNode);

    return outUpdatedNode;
};