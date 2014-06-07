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
#include <vector>

struct TriangleSplitCosts
{
    Polygon clippedObject;
    Triangle* referencedTriangle;
    double containedSurfaceArea;
    
    TriangleSplitCosts() { };
    TriangleSplitCosts(Triangle* object, BoundingBox boundingBox) : clippedObject(Polygon(*object))
    {
        referencedTriangle = object;
        containedSurfaceArea = clippedObject.Clip(boundingBox).SurfaceArea();
    }
    TriangleSplitCosts(Polygon preClippedObject, Triangle* referencedObject) : clippedObject(preClippedObject)
    {
        referencedTriangle = referencedObject;
        containedSurfaceArea = preClippedObject.SurfaceArea();
    }
};

class ModelContainerLeaf : public ModelContainerNode
{
public:
    ModelContainerLeaf();
    ~ModelContainerLeaf();
    vector<TriangleSplitCosts> objects;
    double currentBoundedSurfaceArea();
    virtual ModelContainerNode* AddItem(Triangle* object, BoundingBox boundingBox);
    virtual ModelContainerNode* AddItem(Triangle* object, BoundingBox boundingBox, Vector3D nominalPosition, bool* outFullyContainedByNode);
    virtual bool TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, BoundingBox boundingBox, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties);
private:
    /// Gets the current computational cost if we add the new object
    double GetCost(double totalContainedSurfaceArea, int numberOfContainedObjects, BoundingBox boundingBox);
    double GetTotalContainedSurfaceArea(TriangleSplitCosts newObjectCost, PartitionPlaneType planeType, double planePosition, PartitionKeepDirection keepDirection, vector<TriangleSplitCosts> *outChildBoundedObjects);
    double GetClippedSurfaceArea(Triangle object, BoundingBox boundingBox);
    bool TryGetPotentialSplitPosition(PartitionPlaneType candidatePlane, TriangleSplitCosts newObjectCost, BoundingBox currentBoundingBox, double noSplitCost, double* outCandidateSplitPosition, vector<TriangleSplitCosts> *posBoundedObjects, vector<TriangleSplitCosts> *negBoundedObjects);
};

#endif /* defined(__RayTracer__ModelContainerLeaf__) */
