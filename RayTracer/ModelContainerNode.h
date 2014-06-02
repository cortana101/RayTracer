//
//  ModelContainerNode.h
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef RayTracer_ModelContainerNode_h
#define RayTracer_ModelContainerNode_h

#include "Vector3D.h"
#include "Triangle.h"
#include "BoundingBox.h"

class ModelContainerNode
{
public:
    ModelContainerNode(BoundingBox boundingBox);
    ~ModelContainerNode();
    
    /// Adds an item to the node
    virtual ModelContainerNode* AddItem(Triangle* object) = 0;
    
    /// Adds an item to the node, but with a nominal position instead of relying on intersects
    virtual ModelContainerNode* AddItem(Triangle* object, Vector3D nominalPosition, bool* outFullyContainedByNode) = 0;
    
    /// Determines if any part of the given triangle object intersects any part of the area defined by the bounding box
    bool Intersects(Triangle object);
    
    virtual bool TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties) = 0;

    BoundingBox boundingBox;
};

#endif
