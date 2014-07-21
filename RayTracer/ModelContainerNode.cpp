//
//  ModelContainerNode.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/30/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainerNode.h"
#include "ModelContainerLeaf.h"


bool ModelContainerNode::Intersects(BoundingBox boundingBox, Triangle object)
{
    Polygon objectPolygon (object);
    return objectPolygon.Intersects(boundingBox);
}

bool ModelContainerNode::TryLockNode(pthread_mutex_t *threadRegisterMutex, ModelContainerNode** threadRegister, int threadId, ModelContainerNode* nodeToLock)
{
    bool nodeOccupied = false;

    pthread_mutex_lock(threadRegisterMutex);
    
        for(int i = 0; i < CONCURRENTTHREADS; i++)
        {
            if (threadRegister[i] == nodeToLock)
            {
                nodeOccupied = true;
            }
        }

        if (!nodeOccupied)
        {
            // We only need to lock leaf nodes, since the contents of a partition node wont ever change.
            ModelContainerLeaf* leafNode = dynamic_cast<ModelContainerLeaf*>(nodeToLock);
            
            if (leafNode != NULL)
            {
                threadRegister[threadId] = nodeToLock;
            }
        }
    
    pthread_mutex_unlock(threadRegisterMutex);
    
    return !nodeOccupied;
}

void ModelContainerNode::UnlockNode(pthread_mutex_t *threadRegisterMutex, ModelContainerNode** threadRegister, int threadId)
{
    pthread_mutex_lock(threadRegisterMutex);
    threadRegister[threadId] = NULL;
    pthread_mutex_unlock(threadRegisterMutex);
}