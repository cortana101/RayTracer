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
    long averageNumOfNodesVisited;
    long averageNumOfTrianglesVisited;
    int maxNumOfNodesVisited;
    int maxNumOfTrianglesVisited;
};

struct AddItemProgressParams
{
    int *currentItemIndex;
    int totalItems;
};

struct AddItemThreadParams
{
    pthread_mutex_t *modelRegsiterMutex;
    pthread_mutex_t *modelItemIndexMutex;
    int threadId;
    ModelContainerNode** threadRegister;
    AddItemProgressParams progress;
    vector<Triangle*> *model;
    ModelContainerNode** root;
    BoundingBox* globalBoundingBox;
};

/// Basic k-d tree implementation for partitioning our model space for faster searching by rays
class ModelContainer
{
public:
    ModelContainer();
    ~ModelContainer();
    ModelContainerNode* root;
    bool TryGetIntersection(Vector3D ray, Vector3D rayOrigin, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties);
    void BuildTree(vector<Triangle*> model);
    void PrintTreeStatistics();
    void PrintTraceStatistics();
private:
    // Describes the bounding box for everything in the model, starts as an empty box since nothing is in the model until we add stuff
    static BoundingBox* globalBoundingBox;
    static TraceStatistics traceStatistics;
    static void* AddItemThread(void* addItemThreadParams);
    void SetGlobalBoundingBox(vector<Triangle*> model);
};

#endif /* defined(__RayTracer__ModelContainer__) */
