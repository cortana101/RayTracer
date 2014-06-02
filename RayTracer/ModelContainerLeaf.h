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
    ModelContainerLeaf();
    ~ModelContainerLeaf();
    QList<Triangle*> objects;
    int objectCount();
    virtual ModelContainerNode* AddItem(Triangle* object, BoundingBox boundingBox);
    virtual ModelContainerNode* AddItem(Triangle* object, BoundingBox boundingBox, Vector3D nominalPosition, bool* outFullyContainedByNode);
    virtual bool TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, BoundingBox boundingBox, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties);
private:
    /// Gets the current computational cost if we add the new object
    double GetCost(Triangle* newObject, BoundingBox boundingBox, QList<Triangle*> *outBoundedObjects);
    double GetClippedSurfaceArea(Triangle object, BoundingBox boundingBox);
    bool TryGetPotentialSplitPosition(PartitionPlaneType candidatePlane, Triangle* newObject, BoundingBox currentBoundingBox, double noSplitCost, double* outCandidateSplitPosition, QList<Triangle*> *posBoundedObjects, QList<Triangle*> *negBoundedObjects);
};

#endif /* defined(__RayTracer__ModelContainerLeaf__) */
