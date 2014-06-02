//
//  ModelContainerLeaf.h
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__ModelContainerLeaf__
#define __RayTracer__ModelContainerLeaf__

#include <iostream>
#include "ModelContainerNode.h"
#include "ModelContainerPartition.h"
#include "ModelObject.h"
#include "Polygon.h"
#include <QtCore/QList>

class ModelContainerLeaf : public ModelContainerNode
{
public:
    ModelContainerLeaf(BoundingBox boundingBox);
    ~ModelContainerLeaf();
    QList<Triangle*> objects;
    int objectCount = 0;
    virtual ModelContainerNode* AddItem(Triangle* object);
    virtual ModelContainerNode* AddItem(Triangle* object, Vector3D nominalPosition, bool* outFullyContainedByNode);
    virtual bool TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties);
private:
    /// Gets the current computational cost if we add the new object and constrain the bounding box to the provided bounding box
    double GetCost(Triangle newObject, BoundingBox boundingBox);
    double GetClippedSurfaceArea(Triangle object);
    bool TryGetPotentialSplitPosition(PartitionPlaneType candidatePlane, Triangle newObject, double noSplitCost, double* outCandidateSplitPosition);
};

#endif /* defined(__RayTracer__ModelContainerLeaf__) */
