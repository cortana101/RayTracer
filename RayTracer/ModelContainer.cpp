//
//  ModelContainer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/28/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainer.h"

ModelContainer::ModelContainer()
{
    // Start with an empty leaf node
    this->root = new ModelContainerLeaf();
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
    
    return this->root->TraceRay(ray, rayOrigin, initialRaySearchPosition, this->globalBoundingBox, ignoredObject, outIntersectedModel, outIntersectProperties);
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