//
//  ModelContainerLeaf.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainerLeaf.h"

// The computational cost of calculating an intersect
#define COSTOFINTERSECT 75

// The computational cost of calculating traversing 1 more node
#define COSTOFTRAVERSAL 10

// The threshold of evenness when we are searching for a potential split location
#define SPLITTHRESHOLD 0.8

#define WEIGHTEDCHANCETHRESHOLD 0.0001

// The max number of attempts to try to split a node such that it satisfies the split threshold
// if after this many attempts we still cant get to the threshold, just take the split as is
#define MAXSPLITATTEMPTS 4

#define MINOBJECTSBEFORECONSIDERINGSPLIT 3

ModelContainerLeaf::ModelContainerLeaf()
{
    this->objects = vector<TriangleSplitCosts>();
}

ModelContainerLeaf::~ModelContainerLeaf()
{
    this->objects.clear();
}

double ModelContainerLeaf::currentBoundedSurfaceArea()
{
    double accumulator = 0.0;
    
    for (int i = 0; i < this->objects.size(); i++)
    {
        accumulator += this->objects[i].containedSurfaceArea;
    }
    
    return accumulator;
}

ModelContainerNode* ModelContainerLeaf::AddItem(Triangle *newObject, BoundingBox boundingBox)
{
    PartitionPlaneType planes[3] = { PartitionPlaneType::X, PartitionPlaneType::Y, PartitionPlaneType::Z };
    PartitionPlaneType longestEdge = boundingBox.GetLongestEdge();
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

     TriangleSplitCosts newTriangleCost (newObject, boundingBox);
    
    if(this->objects.size() > MINOBJECTSBEFORECONSIDERINGSPLIT)
    {
        vector<TriangleSplitCosts> posBoundedObjects = vector<TriangleSplitCosts>();
        vector<TriangleSplitCosts> negBoundedObjects = vector<TriangleSplitCosts>();
        
        // We dont actually need to get the bounded objects for this call, just hacking it for now
        double currentCost = this->GetCost(this->currentBoundedSurfaceArea(), (int)this->objects.size() + 1, boundingBox);
        
        for (int i = 0; i < 3; i++)
        {
            if (this->TryGetPotentialSplitPosition(planes[i], newTriangleCost, boundingBox, currentCost, &candidateSplitPosition, &posBoundedObjects, &negBoundedObjects))
            {
                ModelContainerLeaf* posChild = new ModelContainerLeaf();
                ModelContainerLeaf* negChild = new ModelContainerLeaf();
                
                posChild->objects = posBoundedObjects;
                negChild->objects = negBoundedObjects;

                ModelContainerPartition* newPartitionNode = new ModelContainerPartition();
                newPartitionNode->partitionPlane = planes[i];
                newPartitionNode->partitionPosition = candidateSplitPosition;
                newPartitionNode->posChild = posChild;
                newPartitionNode->negChild = negChild;
                
                // After we add everything to the new node, we can delete the current node, because it is being replaced by the new partition node
                delete this;
                
                return newPartitionNode;
            }
        }
    }
    
    // Add the new object to the current node if we havent already decided to split the node
    this->objects.push_back(newTriangleCost);
    return this;
}

ModelContainerNode* ModelContainerLeaf::AddItem(Triangle* newObject, BoundingBox boundingBox, Vector3D nominalPosition, bool* outFullyContainedByNode)
{
    // At the leaf node we dont really care about the nominal position anymore, we are now in the right place anyways
    
    *outFullyContainedByNode = boundingBox.Contains(*newObject);
    return this->AddItem(newObject, boundingBox);
}

double ModelContainerLeaf::GetCost(double totalContainedSurfaceArea, int numberOfContainedObjects, BoundingBox boundingBox)
{
    // Weigh the chances of hitting one of the objects given the fact that we've already hit the bounding box
    double weightedChancesOfHit = totalContainedSurfaceArea / boundingBox.SurfaceArea();

    if (weightedChancesOfHit < WEIGHTEDCHANCETHRESHOLD)
    {
        weightedChancesOfHit = WEIGHTEDCHANCETHRESHOLD;
    }
    
    return COSTOFTRAVERSAL + numberOfContainedObjects * COSTOFINTERSECT / weightedChancesOfHit;
}

double ModelContainerLeaf::GetTotalContainedSurfaceArea(TriangleSplitCosts newObjectCost, PartitionPlaneType planeType, double planePosition, PartitionKeepDirection keepDirection, vector<TriangleSplitCosts> *outChildBoundedObjects)
{
    // The cost of a leaf node is basically the cost of the number of triangles in the leaf
    // weighed by the probability of hitting any of the triangles
    double totalSurfaceAreaOfClippedObjects = 0.0;
    int numberOfContainedObjects = 0;
    bool objectIntersectsPlane = false;
    
    outChildBoundedObjects->clear();
    
    for (int i = 0; i < this->objects.size(); i++)
    {
        if (this->objects[i].clippedObject.IsOnSideOfPlane(planeType, planePosition, keepDirection, &objectIntersectsPlane))
        {
            if (objectIntersectsPlane)
            {
                Polygon childClippedPolygon = this->objects[i].clippedObject.Clip(planeType, planePosition, keepDirection);
                TriangleSplitCosts childSplitCosts (childClippedPolygon, this->objects[i].referencedTriangle);
                outChildBoundedObjects->push_back(childSplitCosts);
                totalSurfaceAreaOfClippedObjects += childSplitCosts.containedSurfaceArea;
            }
            else
            {
                totalSurfaceAreaOfClippedObjects += this->objects[i].containedSurfaceArea;
                outChildBoundedObjects->push_back(this->objects[i]);
            }
            
            numberOfContainedObjects++;
        }
    }
    
    if (newObjectCost.clippedObject.IsOnSideOfPlane(planeType, planePosition, keepDirection, &objectIntersectsPlane))
    {
        if (objectIntersectsPlane)
        {
            Polygon childClippedPolygon = newObjectCost.clippedObject.Clip(planeType, planePosition, keepDirection);
            TriangleSplitCosts childSplitCosts (childClippedPolygon, newObjectCost.referencedTriangle);
            outChildBoundedObjects->push_back(childSplitCosts);
            totalSurfaceAreaOfClippedObjects += childSplitCosts.containedSurfaceArea;
        }
        else
        {
            totalSurfaceAreaOfClippedObjects += newObjectCost.containedSurfaceArea;
            outChildBoundedObjects->push_back(newObjectCost);
        }
        
        numberOfContainedObjects++;
    }
    
    return totalSurfaceAreaOfClippedObjects;
}

double ModelContainerLeaf::GetClippedSurfaceArea(Triangle object, BoundingBox boundingBox)
{
    Polygon objectPolygon = Polygon(object);
    return objectPolygon.Clip(boundingBox).SurfaceArea();
}

bool ModelContainerLeaf::TryGetPotentialSplitPosition(PartitionPlaneType candidatePlane, TriangleSplitCosts newObjectCost, BoundingBox currentBoundingBox, double noSplitCost, double* outCandidateSplitPosition, vector<TriangleSplitCosts> *posBoundedObjects, vector<TriangleSplitCosts> *negBoundedObjects)
{
    // Binary search the best potential split position until we reach a certain threshold
    double searchLength = currentBoundingBox.GetLength(candidatePlane) / 2;
    double candidateSplitPosition = currentBoundingBox.GetMinInAxis(candidatePlane) + searchLength;
    
    BoundingBox posBound = currentBoundingBox.Constrain(candidatePlane, candidateSplitPosition, PartitionKeepDirection::Positive);
    BoundingBox negBound = currentBoundingBox.Constrain(candidatePlane, candidateSplitPosition, PartitionKeepDirection::Negative);
    
    double posBoundedSurfaceArea = this->GetTotalContainedSurfaceArea(newObjectCost, candidatePlane, candidateSplitPosition, PartitionKeepDirection::Positive, posBoundedObjects);
    double negBoundedSurfaceArea = this->GetTotalContainedSurfaceArea(newObjectCost, candidatePlane, candidateSplitPosition, PartitionKeepDirection::Negative, negBoundedObjects);
    
    double posCost = this->GetCost(posBoundedSurfaceArea, (int)posBoundedObjects->size(), posBound);
    double negCost = this->GetCost(negBoundedSurfaceArea, (int)negBoundedObjects->size(), negBound);
    
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
        
        posBound = currentBoundingBox.Constrain(candidatePlane, candidateSplitPosition, PartitionKeepDirection::Positive);
        negBound = currentBoundingBox.Constrain(candidatePlane, candidateSplitPosition, PartitionKeepDirection::Negative);

        posBoundedSurfaceArea = this->GetTotalContainedSurfaceArea(newObjectCost, candidatePlane, candidateSplitPosition, PartitionKeepDirection::Positive, posBoundedObjects);
        negBoundedSurfaceArea = this->GetTotalContainedSurfaceArea(newObjectCost, candidatePlane, candidateSplitPosition, PartitionKeepDirection::Negative, negBoundedObjects);
        
        posCost = this->GetCost(posBoundedSurfaceArea, (int)posBoundedObjects->size(), posBound);
        negCost = this->GetCost(negBoundedSurfaceArea, (int)negBoundedObjects->size(), negBound);
        
        splitAttempts++;
    }

    double currentBoundingboxSurfaceArea = currentBoundingBox.SurfaceArea();
   
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

bool ModelContainerLeaf::TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, BoundingBox boundingBox, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties)
{
    IntersectProperties localIntersectProperties;
    bool hasIntersect = false;
    
    for (int i = 0; i < this->objects.size(); i++)
    {
        if (this->objects[i].referencedTriangle != ignoredObject && this->objects[i].referencedTriangle->ProcessRay(ray, rayOrigin, &localIntersectProperties))
        {
            if (!hasIntersect || localIntersectProperties.intersectPosition.GetMagnitude() < outIntersectProperties->intersectPosition.GetMagnitude())
            {
                hasIntersect = true;
                *outIntersectProperties = localIntersectProperties;
                *outIntersectedModel = this->objects[i].referencedTriangle;
            }
        }
    }
    
    return hasIntersect;
}