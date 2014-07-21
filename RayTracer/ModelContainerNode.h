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
#include "Polygon.h"

#define CONCURRENTTHREADS 4

struct TreeStatistics
{
    double averageTrianglesPerLeaf;
    int maxTrianglesLeaf;
    int numberOfLeafs;
    int maxDepth;
    double averageDepth;
    int numberOfEmptyLeafs;
};

class ModelContainerNode
{
public:
    /// Adds an item to the node
    virtual bool TryAddItem(Triangle* object, BoundingBox boundingBox, ModelContainerNode** threadRegister, int threadId, pthread_mutex_t *threadRegisterMutex, ModelContainerNode** outUpdatedNode) = 0;
    
    /// Adds an item to the node, but with a nominal position instead of relying on intersects
    virtual bool TryAddItem(Triangle* object, BoundingBox, Vector3D nominalPosition, ModelContainerNode** threadRegister, int threadId, pthread_mutex_t *threadRegisterMutex, bool* outFullyContainedByNode, ModelContainerNode** outUpdatedNode) = 0;
    
    /// Determines if any part of the given triangle object intersects any part of the area defined by the bounding box
    bool Intersects(BoundingBox boundingBox, Triangle object);
    
    virtual bool TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, BoundingBox boundingBox, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties, int *outNodesVisited, int *outNumTrianglesVisited) = 0;
    
    virtual TreeStatistics GetStatistics(int currentDepth) = 0;
    
    // Tries to obtain a lock on the current node before we start working on it, returns true if we are able to lock the current node, false if the current node is occupied
    static bool TryLockNode(pthread_mutex_t *threadRegisterMutex, ModelContainerNode** threadRegister, int threadId, ModelContainerNode* nodeToLock);
    static void UnlockNode(pthread_mutex_t *threadRegisterMutex, ModelContainerNode** threadRegister, int threadId);
};

#endif
