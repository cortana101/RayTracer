//
//  ModelContainer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/28/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainer.h"

ModelContainer::ModelContainer(ModelObject** parsedModel, int modelLength)
{
    BoundingBox globalBoundingBox = BoundingBox(Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0));
    
    for (int i = 0; i < modelLength; i++)
    {
        Triangle* model = dynamic_cast<Triangle*>(parsedModel[i]);
        globalBoundingBox = globalBoundingBox.ExpandToContain(BoundingBox::GetMinimumBoundingBox(*model));
    }
    
    // Expand the global bounding box slightly to avoid errors when things are right on the bounds
    globalBoundingBox.min = globalBoundingBox.min.Add(Vector3D(-0.1, -0.1, -0.1));
    globalBoundingBox.max = globalBoundingBox.max.Add(Vector3D(0.1, 0.1, 0.1));
    
    // Start with an empty leaf node
    this->root = new ModelContainerLeaf(globalBoundingBox);
    
    for (int i = 0; i < modelLength; i++)
    {
        Triangle* model = dynamic_cast<Triangle*>(parsedModel[i]);
        
        if (model != NULL)
        {
            this->AddItem(model);
        }
    }
}

ModelContainer::~ModelContainer()
{
    // Do nothing
}

void ModelContainer::AddItem(Triangle *newObject)
{
    // Expand the global bounding box if the new object is outside of it
    //this->globalBoundingBox = this->globalBoundingBox.ExpandToContain(BoundingBox::GetMinimumBoundingBox(*newObject));
    
    bool fullyContainedByChild;
    
    this->root = this->root->AddItem(newObject, newObject->GetNominalPosition(), &fullyContainedByChild);
    
    if (!fullyContainedByChild)
    {
        throw "Something is wrong with expansion of our global bounding box, the root should always fully contain any shape we add to the model";
    }
}

bool ModelContainer::TryGetIntersection(Vector3D ray, Vector3D rayOrigin, ModelObject* ignoredObject, ModelObject **outIntersectedModel, IntersectProperties* outIntersectProperties)
{
    Vector3D initialRaySearchPosition;
    bool rayHitsGlobalBoundingBox = false;
    
    if (this->root->boundingBox.Contains(rayOrigin))
    {
        initialRaySearchPosition = rayOrigin;
        rayHitsGlobalBoundingBox = true;
    }
    else
    {
        rayHitsGlobalBoundingBox = this->root->boundingBox.TryGetIntersectionAtSurface(ray, rayOrigin, &initialRaySearchPosition);
    }
    
    return this->root->TraceRay(ray, rayOrigin, initialRaySearchPosition, ignoredObject, outIntersectedModel, outIntersectProperties);
}