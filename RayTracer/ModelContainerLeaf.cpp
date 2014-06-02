//
//  ModelContainerLeaf.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainerLeaf.h"

// The computational cost of calculating an intersect
#define COSTOFINTERSECT 50

// The computational cost of calculating traversing 1 more node
#define COSTOFTRAVERSAL 10

// The threshold of evenness when we are searching for a potential split location
#define SPLITTHRESHOLD 0.6

#define WEIGHTEDCHANCETHRESHOLD 0.0001

// The max number of attempts to try to split a node such that it satisfies the split threshold
// if after this many attempts we still cant get to the threshold, just take the split as is
#define MAXSPLITATTEMPTS 4

#define MINOBJECTSBEFORECONSIDERINGSPLIT 3

ModelContainerLeaf::ModelContainerLeaf(BoundingBox boundingBox) : ModelContainerNode(boundingBox)
{
    // Do nothing
}

ModelContainerLeaf::~ModelContainerLeaf()
{
    // Do nothing
}

ModelContainerNode* ModelContainerLeaf::AddItem(Triangle *newObject)
{
    PartitionPlaneType planes[3] = { PartitionPlaneType::X, PartitionPlaneType::Y, PartitionPlaneType::Z };
    PartitionPlaneType longestEdge = this->boundingBox.GetLongestEdge();
    planes[0] = longestEdge;
    
    // Ideally we should split on the longest edge if possible so we dont end up with long thin rectangles, so we try the longest edge first
    if (longestEdge == PartitionPlaneType::Y)
    {
        planes[1] = PartitionPlaneType::X;
    }
    else if (longestEdge == PartitionPlaneType::Z)
    {
        planes[2] = PartitionPlaneType::X;
    }
    
    double candidateSplitPosition;
    
    if(this->objectCount > MINOBJECTSBEFORECONSIDERINGSPLIT)
    {
        double currentCost = this->GetCost(*newObject, this->boundingBox);
        
        for (int i = 0; i < 3; i++)
        {
            if (this->TryGetPotentialSplitPosition(planes[i], *newObject, currentCost, &candidateSplitPosition))
            {
                BoundingBox posChildBoundingBox = this->boundingBox.Constrain(planes[i], candidateSplitPosition, PartitionKeepDirection::Positive);
                BoundingBox negChildBoundingBox = this->boundingBox.Constrain(planes[i], candidateSplitPosition, PartitionKeepDirection::Negative);
                
                ModelContainerLeaf* posChild = new ModelContainerLeaf(posChildBoundingBox);
                ModelContainerLeaf* negChild = new ModelContainerLeaf(negChildBoundingBox);
                
                ModelContainerPartition* newPartitionNode = new ModelContainerPartition(this->boundingBox);
                newPartitionNode->partitionPlane = planes[i];
                newPartitionNode->partitionPosition = candidateSplitPosition;
                newPartitionNode->posChild = posChild;
                newPartitionNode->negChild = negChild;
                
                // Add everything in the current node to the new node, which will decide which child to put the each object into
                for (int i = 0; i < this->objectCount; i++)
                {
                    newPartitionNode->AddItem(this->objects[i]);
                }
                
                newPartitionNode->AddItem(newObject);
                
                // After we add everything to the new node, we can delete the current node, because it is being replaced by the new partition node
                delete this;
                
                return newPartitionNode;
            }
        }
    }
    
    // Add the new object to the current node if we havent already decided to split the node
    this->objects.append(newObject);
    this->objectCount++;
    return this;
}

ModelContainerNode* ModelContainerLeaf::AddItem(Triangle* newObject, Vector3D nominalPosition, bool* outFullyContainedByNode)
{
    // At the leaf node we dont really care about the nominal position anymore, we are now in the right place anyways
    
    *outFullyContainedByNode = this->boundingBox.Contains(*newObject);
    return this->AddItem(newObject);
}

double ModelContainerLeaf::GetCost(Triangle newObject, BoundingBox boundingBox)
{
    // The cost of a leaf node is basically the cost of the number of triangles in the leaf
    // weighed by the probability of hitting any of the triangles
    double totalSurfaceAreaOfClippedObjects = 0.0;
    int numberOfContainedObjects = 0;
    
    for (int i = 0; i < this->objectCount; i++)
    {
        if (boundingBox.Intersects(*this->objects[i]))
        {
            totalSurfaceAreaOfClippedObjects += this->GetClippedSurfaceArea(*this->objects[i]);
            numberOfContainedObjects++;
        }
    }
    
    if (boundingBox.Intersects(newObject))
    {
        totalSurfaceAreaOfClippedObjects += this->GetClippedSurfaceArea(newObject);
        numberOfContainedObjects++;
    }
    
    // Weigh the chances of hitting one of the objects given the fact that we've already hit the bounding box
    double weightedChancesOfHit = totalSurfaceAreaOfClippedObjects / boundingBox.SurfaceArea();

    if (weightedChancesOfHit < WEIGHTEDCHANCETHRESHOLD)
    {
        weightedChancesOfHit = WEIGHTEDCHANCETHRESHOLD;
    }
    
    return COSTOFTRAVERSAL + numberOfContainedObjects * COSTOFINTERSECT / weightedChancesOfHit;
}

double ModelContainerLeaf::GetClippedSurfaceArea(Triangle object)
{
    Polygon objectPolygon = Polygon(object);
    return objectPolygon.Clip(this->boundingBox).SurfaceArea();
}

bool ModelContainerLeaf::TryGetPotentialSplitPosition(PartitionPlaneType candidatePlane, Triangle newObject, double noSplitCost, double* outCandidateSplitPosition)
{
    // Binary search the best potential split position until we reach a certain threshold
    double searchLength = this->boundingBox.GetLength(candidatePlane) / 2;
    double candidateSplitPosition = this->boundingBox.GetMinInAxis(candidatePlane) + searchLength;
    
    BoundingBox posBound = this->boundingBox.Constrain(candidatePlane, candidateSplitPosition, PartitionKeepDirection::Positive);
    BoundingBox negBound = this->boundingBox.Constrain(candidatePlane, candidateSplitPosition, PartitionKeepDirection::Negative);
    
    double posCost = this->GetCost(newObject, posBound);
    double negCost = this->GetCost(newObject, negBound);
    
    int splitAttempts = 0;
    
    while ((posCost > negCost ? negCost / posCost : posCost / negCost) < SPLITTHRESHOLD && splitAttempts < MAXSPLITATTEMPTS)
    {
        // Do a binary search
        searchLength /= 2;
        
        if (posCost > negCost)
        {
            candidateSplitPosition += searchLength;
        }
        else
        {
            candidateSplitPosition -= searchLength;
        }
        
        posBound = this->boundingBox.Constrain(candidatePlane, candidateSplitPosition, PartitionKeepDirection::Positive);
        negBound = this->boundingBox.Constrain(candidatePlane, candidateSplitPosition, PartitionKeepDirection::Negative);
        
        posCost = this->GetCost(newObject, posBound);
        negCost = this->GetCost(newObject, negBound);
        
        splitAttempts++;
    }

    double currentBoundingboxSurfaceArea = this->boundingBox.SurfaceArea();
   
    // This is the total cost of doing a split
    double splitCost = COSTOFTRAVERSAL + posBound.SurfaceArea() * posCost / currentBoundingboxSurfaceArea + negBound.SurfaceArea() * negCost / currentBoundingboxSurfaceArea;

    // If its cheaper not to split, then we shouldnt split
    if (splitCost < noSplitCost)
    {
        *outCandidateSplitPosition = candidateSplitPosition;
        return true;
    }
    else
    {
        return false;
    }
}

bool ModelContainerLeaf::TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties)
{
    IntersectProperties localIntersectProperties;
    bool hasIntersect = false;
    
    for (int i = 0; i < this->objectCount; i++)
    {
        if (this->objects[i] != ignoredObject && this->objects[i]->ProcessRay(ray, rayOrigin, &localIntersectProperties))
        {
            if (!hasIntersect || localIntersectProperties.intersectPosition.GetMagnitude() < outIntersectProperties->intersectPosition.GetMagnitude())
            {
                hasIntersect = true;
                *outIntersectProperties = localIntersectProperties;
                *outIntersectedModel = this->objects[i];
            }
        }
    }
    
    return hasIntersect;
}