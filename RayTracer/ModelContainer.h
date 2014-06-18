//
//  ModelContainer.h
//  RayTracer
//
//  Created by Daniel Shih on 5/28/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__ModelContainer__
#define __RayTracer__ModelContainer__

#include <iostream>
#include "ModelObject.h"
#include "ModelContainerNode.h"
#include "ModelContainerLeaf.h"

#define MAXMODELPERNODE 6

struct TraceStatistics
{
    int numberOfRaysProcessed;
    int averageNumOfNodesVisited;
    int averageNumOfTrianglesVisited;
    int maxNumOfNodesVisited;
    int maxNumOfTrianglesVisited;
};

/// Basic k-d tree implementation for partitioning our model space for faster searching by rays
class ModelContainer
{
public:
    ModelContainer();
    ~ModelContainer();
    ModelContainerNode* root;
    void AddItem(Triangle *newObject);
    bool TryGetIntersection(Vector3D ray, Vector3D rayOrigin, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties);
    void PrintTreeStatistics();
    void PrintTraceStatistics();
private:
    // Describes the bounding box for everything in the model, starts as an empty box since nothing is in the model until we add stuff
    BoundingBox globalBoundingBox = BoundingBox(Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0));
    static TraceStatistics traceStatistics;
};

#endif /* defined(__RayTracer__ModelContainer__) */
