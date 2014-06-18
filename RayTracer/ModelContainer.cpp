//
//  ModelContainer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/28/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainer.h"

TraceStatistics ModelContainer::traceStatistics;

ModelContainer::ModelContainer()
{
    // Start with an empty leaf node
    this->root = new ModelContainerLeaf();
    traceStatistics = TraceStatistics { 0, 0, 0, 0, 0 };
}

ModelContainer::~ModelContainer()
{
    // Do nothing
}

void ModelContainer::AddItem(Triangle *newObject)
{
    // Expand the global bounding box if the new object is outside of it
    this->globalBoundingBox = this->globalBoundingBox.ExpandToContain(BoundingBox::GetMinimumBoundingBox(*newObject));
    
    bool fullyContainedByChild;
    
    this->root = this->root->AddItem(newObject, this->globalBoundingBox, newObject->GetNominalPosition(), &fullyContainedByChild);
    
    if (!fullyContainedByChild)
    {
        throw "Something is wrong with expansion of our global bounding box, the root should always fully contain any shape we add to the model";
    }
}

bool ModelContainer::TryGetIntersection(Vector3D ray, Vector3D rayOrigin, ModelObject* ignoredObject, ModelObject **outIntersectedModel, IntersectProperties* outIntersectProperties)
{
    Vector3D initialRaySearchPosition;
    bool rayHitsGlobalBoundingBox = false;
    
    if (this->globalBoundingBox.Contains(rayOrigin))
    {
        initialRaySearchPosition = rayOrigin;
        rayHitsGlobalBoundingBox = true;
    }
    else
    {
        rayHitsGlobalBoundingBox = this->globalBoundingBox.TryGetIntersectionAtSurface(ray, rayOrigin, &initialRaySearchPosition);
    }
    
    int numNodesVisited, numTrianglesVisited;
    
    bool rayIntersectsSomething = this->root->TraceRay(ray, rayOrigin, initialRaySearchPosition, this->globalBoundingBox, ignoredObject, outIntersectedModel, outIntersectProperties, &numNodesVisited, &numTrianglesVisited);
    
    traceStatistics.numberOfRaysProcessed += 1;
    traceStatistics.maxNumOfNodesVisited = std::max(traceStatistics.maxNumOfNodesVisited, numNodesVisited);
    traceStatistics.maxNumOfTrianglesVisited = std::max(traceStatistics.maxNumOfTrianglesVisited, numTrianglesVisited);
    
    // For the averages, use these as accumulators first, we will do the final averaging when we print out the results
    traceStatistics.averageNumOfNodesVisited += numNodesVisited;
    traceStatistics.averageNumOfTrianglesVisited += numTrianglesVisited;
    
    return rayIntersectsSomething;
}

void ModelContainer::PrintTreeStatistics()
{
    TreeStatistics statistics = this->root->GetStatistics(1);
    statistics.averageDepth /= statistics.numberOfLeafs;
    statistics.averageTrianglesPerLeaf /= statistics.numberOfLeafs;
    
    cout << "ModelContainerStatistics: \n";
    cout << "MaxDepth: " << statistics.maxDepth << "\n";
    cout << "AverageDepth: " << statistics.averageDepth << "\n";
    cout << "MaxTrianglesPerNode: " << statistics.maxTrianglesLeaf << "\n";
    cout << "AverageTrianglesPerNode: " << statistics.averageTrianglesPerLeaf << "\n";
    cout << "NumberOfLeafs: " << statistics.numberOfLeafs << "\n";
    cout << "NumberOfEmptyLeafs: " << statistics.numberOfEmptyLeafs << "\n";
    cout.flush();
}

void ModelContainer::PrintTraceStatistics()
{
    traceStatistics.averageNumOfNodesVisited /= traceStatistics.numberOfRaysProcessed;
    traceStatistics.averageNumOfTrianglesVisited /= traceStatistics.numberOfRaysProcessed;
    
    cout << "ModelTraceStatistics: \n";
    cout << "Number of rays processed: " << traceStatistics.numberOfRaysProcessed << "\n";
    cout << "Max nodes visited: " << traceStatistics.maxNumOfNodesVisited << "\n";
    cout << "Average nodes visited: " << traceStatistics.averageNumOfNodesVisited << "\n";
    cout << "Max triangles visited: " << traceStatistics.maxNumOfTrianglesVisited << "\n";
    cout << "Average triangles visited: " << traceStatistics.averageNumOfTrianglesVisited << "\n";
    cout.flush();
}