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