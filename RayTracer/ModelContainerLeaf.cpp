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

#define WEIGHTEDCHANCETHRESHOLD 0.0001

#define MINOBJECTSBEFORECONSIDERINGSPLIT 1

ModelContainerLeaf::ModelContainerLeaf()
{
    this->containedObjects = vector<Triangle*>();
}

ModelContainerLeaf::~ModelContainerLeaf()
{
    this->containedObjects.clear();
}

double ModelContainerLeaf::CurrentBoundedSurfaceArea(vector<TriangleSplitCosts> triangleSplitCosts)
{
    double accumulator = 0.0;
    
    for (vector<TriangleSplitCosts>::iterator i = triangleSplitCosts.begin(); i != triangleSplitCosts.end(); ++i)
    {
        accumulator += i->containedSurfaceArea;
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

    // Declare a temporary array just for this add calculation
    vector<TriangleSplitCosts> triangleSplitCosts;

    TriangleSplitCosts newTriangleSplitCost(newObject, boundingBox);
    
    if (newTriangleSplitCost.containedSurfaceArea == 0)
    {
        // Sometimes, if a vertex of a new triangle lies perfectly on the face of the bounding box, and the other vertices as well as the
        // new triangle itself is in all other senses not contained in the triangle, we will end up in a situation where we will clip out
        // everything in the triangle and the contained surface area would be zero, in such a case, we consider the triangle to not actually
        // be contained in this node, we can just return and skip
        
        return this;
    }
    
    triangleSplitCosts.push_back(newTriangleSplitCost);
    
    for (vector<Triangle*>::iterator i = this->containedObjects.begin(); i != this->containedObjects.end(); ++i)
    {
        triangleSplitCosts.push_back(TriangleSplitCosts(*i, boundingBox));
    }
    
    if(this->containedObjects.size() > MINOBJECTSBEFORECONSIDERINGSPLIT)
    {
        vector<Triangle*> posBoundedObjects = vector<Triangle*>();
        vector<Triangle*> negBoundedObjects = vector<Triangle*>();
        
        // We dont actually need to get the bounded objects for this call, just hacking it for now
        double currentCost = this->GetCost(this->CurrentBoundedSurfaceArea(triangleSplitCosts), (int)this->containedObjects.size() + 1, boundingBox);
        
        for (int i = 0; i < 3; i++)
        {
            if (this->TryGetPotentialSplitPosition(planes[i], triangleSplitCosts, boundingBox, currentCost, &candidateSplitPosition, &posBoundedObjects, &negBoundedObjects))
            {
                ModelContainerLeaf* posChild = new ModelContainerLeaf();
                ModelContainerLeaf* negChild = new ModelContainerLeaf();
                
                posChild->containedObjects = posBoundedObjects;
                negChild->containedObjects = negBoundedObjects;

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
    this->containedObjects.push_back(newObject);
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

double ModelContainerLeaf::GetTotalContainedSurfaceArea(vector<TriangleSplitCosts> partiallyContainedObjects, PartitionPlaneType planeType, double planePosition)
{
    // In this method, we dont have to do an intersect test, because it comes in from the SAH scan, we can already assume that everything
    // we are passed in the split structure is contained
    double containedSurfaceArea = 0.0;
    
    for (vector<TriangleSplitCosts>::iterator i = partiallyContainedObjects.begin(); i != partiallyContainedObjects.end(); ++i)
    {
        // Since we are doing an ascending scan, we can always assume that the keep direction is negative
        Polygon childClippedPolygon = i->clippedObject.Clip(planeType, planePosition, PartitionKeepDirection::Negative);
        containedSurfaceArea += childClippedPolygon.SurfaceArea();
    }
    
    return containedSurfaceArea;
}

double ModelContainerLeaf::GetClippedSurfaceArea(Triangle object, BoundingBox boundingBox)
{
    Polygon objectPolygon = Polygon(object);
    return objectPolygon.Clip(boundingBox).SurfaceArea();
}

bool ModelContainerLeaf::TryGetPotentialSplitPosition(PartitionPlaneType candidatePlane, vector<TriangleSplitCosts> triangleSplitCosts, BoundingBox currentBoundingBox, double noSplitCost, double* outCandidateSplitPosition, vector<Triangle*> *posBoundedObjects, vector<Triangle*> *negBoundedObjects)
{
    vector<SplitStructureNode> splitStructure = this->GetSplitStructureInAxis(triangleSplitCosts, candidatePlane);
    
    vector<TriangleSplitCosts> objectsIntersectingPlane;
    double totalSurfaceArea = 0.0;
    
    for (vector<TriangleSplitCosts>::iterator i = triangleSplitCosts.begin(); i != triangleSplitCosts.end(); ++i)
    {
        totalSurfaceArea += i->containedSurfaceArea;
    }
    
    // Initialize both costs to be the same, it can only get better from here.
    double posCost = noSplitCost;
    double negCost = noSplitCost;
    double bestPosCost = posCost;
    double bestNegCost = negCost;
    double bestSplitPositionIndex = 0;
    
    // Keep a separate accumulator of the already calculated surface areas so we dont need to recalculate it all the time
    double whollyContainedSurfaceAreas = 0.0;
    int negBoundedObjectCount = 0;

    for (int i = 0; i < splitStructure.size(); i++)
    {
        double partiallContainedSurfaceAreas = this->GetTotalContainedSurfaceArea(objectsIntersectingPlane, candidatePlane, splitStructure[i].positionInAxis);
        double containedSurfaceArea = partiallContainedSurfaceAreas + whollyContainedSurfaceAreas;
      
        BoundingBox posChildBoundingBox (Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0));
        BoundingBox negChildBoundingBox (Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0));
        
        // Sometimes, due to the way we clip triangles, there may be tiny rounding off errors such that the split position may be very slightly
        // outside of the current bounding box, this results in the bounding box being constrained out of existence, for the purposes of SAH calculations
        // this isn't invalid, though it wont ever be a good split because it is effectively the same as not splitting, so we can safely just ignore it
        // to save some time
        if (currentBoundingBox.TryConstrain(candidatePlane, splitStructure[i].positionInAxis, PartitionKeepDirection::Positive, &posChildBoundingBox) &&
            currentBoundingBox.TryConstrain(candidatePlane, splitStructure[i].positionInAxis, PartitionKeepDirection::Negative, &negChildBoundingBox))
        {
            posCost = this->GetCost(totalSurfaceArea - containedSurfaceArea, (int)triangleSplitCosts.size() - negBoundedObjectCount, posChildBoundingBox);
            negCost = this->GetCost(containedSurfaceArea, negBoundedObjectCount + (int)objectsIntersectingPlane.size(), negChildBoundingBox);
            
            posCost *= posChildBoundingBox.SurfaceArea() / currentBoundingBox.SurfaceArea();
            negCost *= negChildBoundingBox.SurfaceArea() / currentBoundingBox.SurfaceArea();
            
            if ((posCost + negCost) < (bestPosCost + bestNegCost))
            {
                bestPosCost = posCost;
                bestNegCost = negCost;
                bestSplitPositionIndex = i;
            }
        }

        if (splitStructure[i].positionType == SplitStructureNodeType::min)
        {
            // We're entering the bounds of a triangle
            
            objectsIntersectingPlane.push_back(splitStructure[i].referencedObject);
        }
        else
        {
            whollyContainedSurfaceAreas += splitStructure[i].referencedObject.containedSurfaceArea;
            negBoundedObjectCount++;
            
            // We're leaving a triangle, so get it off the list of objects intersecting the split plane
            for (vector<TriangleSplitCosts>::iterator j = objectsIntersectingPlane.begin(); j != objectsIntersectingPlane.end(); ++j)
            {
                if (splitStructure[i].referencedObject.referencedTriangle == j->referencedTriangle)
                {
                    objectsIntersectingPlane.erase(j);
                    break;
                }
            }
        }
    }
    
    if ((bestPosCost + 	bestNegCost) < noSplitCost)
    {
        *outCandidateSplitPosition = splitStructure[bestSplitPositionIndex].positionInAxis;
        negBoundedObjects->clear();
        posBoundedObjects->clear();
        
        for (int i = 0; i < bestSplitPositionIndex; i++)
        {
            if (splitStructure[i].positionType == SplitStructureNodeType::min)
            {
                negBoundedObjects->push_back(splitStructure[i].referencedObject.referencedTriangle);
            }
        }
        
        for (int i = ((int)splitStructure.size() - 1); i > bestSplitPositionIndex; i--)
        {
            if (splitStructure[i].positionType == SplitStructureNodeType::max)
            {
                posBoundedObjects->push_back(splitStructure[i].referencedObject.referencedTriangle);
            }
        }
        
        return true;
    }
    else
    {
        return false;
    }
}

bool ModelContainerLeaf::TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, BoundingBox boundingBox, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties, int *outNumNodesVisited, int *outNumTrianglesVisited)
{
    IntersectProperties localIntersectProperties;
    bool hasIntersect = false;
    
    *outNumTrianglesVisited = 0;
    
    for (int i = 0; i < this->containedObjects.size(); i++)
    {
        if (this->containedObjects[i] != ignoredObject)
        {
            (*outNumTrianglesVisited)++;
            
            if(this->containedObjects[i]->ProcessRay(ray, rayOrigin, &localIntersectProperties))
            {
                if (!hasIntersect || localIntersectProperties.intersectPosition.GetMagnitude() < outIntersectProperties->intersectPosition.GetMagnitude())
                {
                    hasIntersect = true;
                    *outIntersectProperties = localIntersectProperties;
                    *outIntersectedModel = this->containedObjects[i];
                }
            }
        }
    }
    
    *outNumNodesVisited = 1;
    
    return hasIntersect;
}

vector<SplitStructureNode> ModelContainerLeaf::GetSplitStructureInAxis(vector<TriangleSplitCosts> triangleSplitCosts, PartitionPlaneType axis)
{
    vector<SplitStructureNode> splitStructure;
    BoundingBox objectBoundingBox (Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0));
    
    for (vector<TriangleSplitCosts>::iterator i = triangleSplitCosts.begin(); i != triangleSplitCosts.end(); ++i)
    {
        double positionInAxisMin, positionInAxisMax;
        
        if (i->clippedObject.TryGetMinimumBoundingBox(&objectBoundingBox))
        {
            if (axis == PartitionPlaneType::X)
            {
                positionInAxisMin = objectBoundingBox.min.x;
                positionInAxisMax = objectBoundingBox.max.x;
            }
            else if (axis == PartitionPlaneType::Y)
            {
                positionInAxisMin = objectBoundingBox.min.y;
                positionInAxisMax = objectBoundingBox.max.y;
            }
            else
            {
                positionInAxisMin = objectBoundingBox.min.z;
                positionInAxisMax = objectBoundingBox.max.z;
            }
            
            SplitStructureNode minStructureNode = SplitStructureNode { *i, positionInAxisMin, SplitStructureNodeType::min };
            SplitStructureNode maxStructureNode = SplitStructureNode { *i, positionInAxisMax, SplitStructureNodeType::max };
            
            splitStructure.push_back(minStructureNode);
            splitStructure.push_back(maxStructureNode);
        }
    }
    
    std:sort(splitStructure.begin(), splitStructure.end(), SplitPositionNodeComparator);
    
    return splitStructure;
}

bool ModelContainerLeaf::SplitPositionNodeComparator(SplitStructureNode node1, SplitStructureNode node2)
{
    return node1.positionInAxis < node2.positionInAxis;
}

TreeStatistics ModelContainerLeaf::GetStatistics(int currentDepth)
{
    TreeStatistics currentStatistics;
    currentStatistics.averageTrianglesPerLeaf = (int)this->containedObjects.size();
    currentStatistics.averageDepth = currentDepth;
    currentStatistics.maxDepth = currentDepth;
    currentStatistics.maxTrianglesLeaf = (int)this->containedObjects.size();
    currentStatistics.numberOfLeafs = 1;
    currentStatistics.numberOfEmptyLeafs = this->containedObjects.size() == 0 ? 1 : 0;
    
    return currentStatistics;
}
