//
//  ModelContainer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/28/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ModelContainer.h"

TraceStatistics ModelContainer::traceStatistics;

ModelContainer::ModelContainer()
{
    // Start with an empty leaf node
    this->root = new ModelContainerLeaf();
    traceStatistics = TraceStatistics { 0, 0, 0, 0, 0 };
}

ModelContainer::~ModelContainer()
{
    // Do nothing
}

void ModelContainer::BuildTree(vector<Triangle*> model)
{
    this->SetGlobalBoundingBox(model);
    
    // Randomise the insertion order to get a more balanced tree
    std::random_shuffle(model.begin(), model.end());
   
    
    // Here we should try to make the construction of the tree multi-threaded, the basics are simple:
    //      We should start up 4 threads, and let them take objects off the model queue together
    //      For each item, the thread will add it as normal, like in the single-threaded model, except that when
    //          it reaches the leaf node, it will check if that leaf node is being updated by another thread
    //      It will do this by the following:
    //          * There will exist an array of node pointers, the length is the number of threads
    //          * each array position will correspond to a thread
    //          * this array will be guarded by a mutex
    //          * the node pointer in that position will point to the node that the thread is currently working on
    //          * NO thread will be allowed to touch a leaf node unless it first acquires a lock on the array and writes the address of the leaf node in its array position
    //          * prior to writing the address it will first check the rest of the array and see if that address is already being used by someone else
    //          * if nobody else is doing stuff with this node, it will write the address to the array and release the lock on the array and begin working on the node
    //          * if it finds someone else is doing stuff with this node, it will abort the add attempt, and try to add the same item again post abort from the root
    //          * after it has finished adding the item, it will again attempt to acquire a lock on the array, and write null in its array position so as to release its hold on that node.
    //          * The add function in modelcontain already returns a pointer to the node, we can pass null through this to signal if the item was added correctly or not
    
    for (vector<Triangle*>::iterator i = model.begin(); i != model.end(); ++i)
    {
        this->AddItemThread(*i);
    }
}

void ModelContainer::SetGlobalBoundingBox(vector<Triangle*> model)
{
    if (this->globalBoundingBox == NULL)
    {
        Triangle* modelObject = model[0];
        
        BoundingBox firstBoundingBox = BoundingBox::GetMinimumBoundingBox(*modelObject);
        
        this->globalBoundingBox = new BoundingBox(firstBoundingBox.min, firstBoundingBox.max);
    }
    
    for (vector<Triangle*>::iterator i = model.begin(); i != model.end(); ++i)
    {
        *this->globalBoundingBox = this->globalBoundingBox->ExpandToContain(BoundingBox::GetMinimumBoundingBox(**i));
    }
}

void ModelContainer::AddItemThread(Triangle *newObject)
{
    bool fullyContainedByChild;
    
    ModelContainerNode* outUpdatedNode;
    
    if(!this->root->TryAddItem(newObject, *this->globalBoundingBox, newObject->GetNominalPosition(), &fullyContainedByChild, &outUpdatedNode))
    {
        throw "At the root level all additions should work, we should not ever be blocked on threading issues here";
    }
    
    this->root = outUpdatedNode;
    
    if (!fullyContainedByChild)
    {
        throw "Something is wrong with expansion of our global bounding box, the root should always fully contain any shape we add to the model";
    }
}

bool ModelContainer::TryGetIntersection(Vector3D ray, Vector3D rayOrigin, ModelObject* ignoredObject, ModelObject **outIntersectedModel, IntersectProperties* outIntersectProperties)
{
    Vector3D initialRaySearchPosition;
    bool rayHitsGlobalBoundingBox = false;
    
    if (this->globalBoundingBox->Contains(rayOrigin))
    {
        initialRaySearchPosition = rayOrigin;
        rayHitsGlobalBoundingBox = true;
    }
    else
    {
        rayHitsGlobalBoundingBox = this->globalBoundingBox->TryGetIntersectionAtSurface(ray, rayOrigin, &initialRaySearchPosition);
    }
    
    int numNodesVisited, numTrianglesVisited;
    
    bool rayIntersectsSomething = this->root->TraceRay(ray, rayOrigin, initialRaySearchPosition, *this->globalBoundingBox, ignoredObject, outIntersectedModel, outIntersectProperties, &numNodesVisited, &numTrianglesVisited);
    
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
