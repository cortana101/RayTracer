//
//  ModelContainer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/28/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include <pthread.h>
#include "ModelContainer.h"

TraceStatistics ModelContainer::traceStatistics;
BoundingBox* ModelContainer::globalBoundingBox;

ModelContainer::ModelContainer()
{
    // Start with an empty leaf node
    this->root = new ModelContainerLeaf();
    traceStatistics = TraceStatistics { 0, 0, 0, 0, 0 };
    globalBoundingBox = NULL;
}

ModelContainer::~ModelContainer()
{
    // Do nothing
}

void ModelContainer::BuildTree(vector<Triangle*> model)
{
    this->SetGlobalBoundingBox(model);
    
    ConsoleUtils::StartProgressBar();
    
    // Randomise the insertion order to get a more balanced tree
    std::random_shuffle(model.begin(), model.end());
   
    // Stores the index of the next node to be added
    int modelProgress = 0;
    ModelContainerNode* threadRegister[CONCURRENTTHREADS + 1] = { };
    
    pthread_mutex_t modelRegisterMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t modelItemIndexMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t threads[CONCURRENTTHREADS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    void* status;
    
    ProgressParams progressParams;
    progressParams.currentPosition = &modelProgress;
    progressParams.total = (int)model.size();
    
    for (int i = 0; i < CONCURRENTTHREADS; i++)
    {
        AddItemThreadParams *parameters = new AddItemThreadParams();
        parameters->modelRegsiterMutex = &modelRegisterMutex;
        parameters->modelItemIndexMutex = &modelItemIndexMutex;
        parameters->threadId = i;
        parameters->threadRegister = threadRegister;
        parameters->progress = progressParams;
        parameters->model = &model;
        parameters->globalBoundingBox = globalBoundingBox;
        parameters->root = &this->root;
        
        void *(*AddItemThread)(void*);
        AddItemThread = &ModelContainer::AddItemThread;
        
        pthread_create(&threads[i], NULL, AddItemThread, (void*)parameters);
    }
    
    void *(*PrintProgressFunction)(void*);
    PrintProgressFunction = &ConsoleUtils::PrintProgress;
    
    pthread_create(&threads[CONCURRENTTHREADS], NULL, PrintProgressFunction, (void*)&progressParams);
    
    pthread_attr_destroy(&attr);
    
    for (int i = 0; i < CONCURRENTTHREADS + 1; i++)
    {
        pthread_join(threads[i], &status);
    }
}

void ModelContainer::SetGlobalBoundingBox(vector<Triangle*> model)
{
    if (globalBoundingBox == NULL)
    {
        Triangle* modelObject = model[0];
        
        BoundingBox firstBoundingBox = BoundingBox::GetMinimumBoundingBox(*modelObject);
        
        globalBoundingBox = new BoundingBox(firstBoundingBox.min, firstBoundingBox.max);
    }
    
    for (vector<Triangle*>::iterator i = model.begin(); i != model.end(); ++i)
    {
        *globalBoundingBox = globalBoundingBox->ExpandToContain(BoundingBox::GetMinimumBoundingBox(**i));
    }
}

void* ModelContainer::AddItemThread(void* addItemThreadParams)
{
    AddItemThreadParams* addItemParameters = (AddItemThreadParams*)addItemThreadParams;
    
    while(*addItemParameters->progress.currentPosition < addItemParameters->progress.total)
    {
        pthread_mutex_lock(addItemParameters->modelItemIndexMutex);
        int currentModelIndex = *addItemParameters->progress.currentPosition;
        *addItemParameters->progress.currentPosition += 1;
        pthread_mutex_unlock(addItemParameters->modelItemIndexMutex);
        
        Triangle* objectToAdd = (*addItemParameters->model)[currentModelIndex];

        bool fullyContainedByChild;
        
        ModelContainerNode* outUpdatedNode;

        while(!ModelContainerNode::TryLockNode(addItemParameters->modelRegsiterMutex, addItemParameters->threadRegister, addItemParameters->threadId, *addItemParameters->root))
        {
            // Loop until we add it, we may want to add a tiny sleep here if it makes a difference
        }
        
        (*addItemParameters->root)->TryAddItem(objectToAdd, *addItemParameters->globalBoundingBox, objectToAdd->GetNominalPosition(), addItemParameters->threadRegister, addItemParameters->threadId, addItemParameters->modelRegsiterMutex, &fullyContainedByChild, &outUpdatedNode);

        *addItemParameters->root = outUpdatedNode;
        
        ModelContainerNode::UnlockNode(addItemParameters->modelRegsiterMutex, addItemParameters->threadRegister, addItemParameters->threadId);
        
        if (!fullyContainedByChild)
        {
            throw "Something is wrong with expansion of our global bounding box, the root should always fully contain any shape we add to the model";
        }
    }
    
    pthread_exit(NULL);
}

bool ModelContainer::TryGetIntersection(Vector3D ray, Vector3D rayOrigin, ModelObject* ignoredObject, ModelObject **outIntersectedModel, IntersectProperties* outIntersectProperties)
{
    Vector3D initialRaySearchPosition;
    bool rayHitsGlobalBoundingBox = false;
    
    if (globalBoundingBox->Contains(rayOrigin))
    {
        initialRaySearchPosition = rayOrigin;
        rayHitsGlobalBoundingBox = true;
    }
    else
    {
        rayHitsGlobalBoundingBox = globalBoundingBox->TryGetIntersectionAtSurface(ray, rayOrigin, &initialRaySearchPosition);
    }
    
    int numNodesVisited, numTrianglesVisited;
    
    bool rayIntersectsSomething = this->root->TraceRay(ray, rayOrigin, initialRaySearchPosition, *globalBoundingBox, ignoredObject, outIntersectedModel, outIntersectProperties, &numNodesVisited, &numTrianglesVisited);
    
    traceStatistics.numberOfRaysProcessed += 1;
    traceStatistics.maxNumOfNodesVisited = std::max(traceStatistics.maxNumOfNodesVisited, numNodesVisited);
    traceStatistics.maxNumOfTrianglesVisited = std::max(traceStatistics.maxNumOfTrianglesVisited, numTrianglesVisited);
    
    // For the averages, use these as accumulators first, we will do the final averaging when we print out the results
    traceStatistics.averageNumOfNodesVisited += numNodesVisited;
    traceStatistics.averageNumOfTrianglesVisited += numTrianglesVisited;
    
    return rayIntersectsSomething;
}

void ModelContainer::PrintTreeStatistics()
{
    TreeStatistics statistics = this->root->GetStatistics(1);
    statistics.averageDepth /= statistics.numberOfLeafs;
    statistics.averageTrianglesPerLeaf /= statistics.numberOfLeafs;
    
    cout << "ModelContainerStatistics: \n";
    cout << "MaxDepth: " << statistics.maxDepth << "\n";
    cout << "AverageDepth: " << statistics.averageDepth << "\n";
    cout << "MaxTrianglesPerNode: " << statistics.maxTrianglesLeaf << "\n";
    cout << "AverageTrianglesPerNode: " << statistics.averageTrianglesPerLeaf << "\n";
    cout << "NumberOfLeafs: " << statistics.numberOfLeafs << "\n";
    cout << "NumberOfEmptyLeafs: " << statistics.numberOfEmptyLeafs << "\n";
    cout.flush();
}

void ModelContainer::PrintTraceStatistics()
{
    traceStatistics.averageNumOfNodesVisited /= traceStatistics.numberOfRaysProcessed;
    traceStatistics.averageNumOfTrianglesVisited /= traceStatistics.numberOfRaysProcessed;
    
    cout << "ModelTraceStatistics: \n";
    cout << "Number of rays processed: " << traceStatistics.numberOfRaysProcessed << "\n";
    cout << "Max nodes visited: " << traceStatistics.maxNumOfNodesVisited << "\n";
    cout << "Average nodes visited: " << traceStatistics.averageNumOfNodesVisited << "\n";
    cout << "Max triangles visited: " << traceStatistics.maxNumOfTrianglesVisited << "\n";
    cout << "Average triangles visited: " << traceStatistics.averageNumOfTrianglesVisited << "\n";
    cout.flush();
}