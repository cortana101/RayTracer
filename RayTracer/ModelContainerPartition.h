//
//  ModelContainerPartition.h
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__ModelContainerPartition__
#define __RayTracer__ModelContainerPartition__

#include <iostream>
#include "ModelContainerNode.h"
#include "PartitionPlaneEnums.h"
#include "Polygon.h"

class ModelContainerPartition : public ModelContainerNode
{
public:
    ModelContainerPartition();
    ~ModelContainerPartition();
    
    /// The type of partition
    PartitionPlaneType partitionPlane;
    
    /// The position of the partitioning plane
    double partitionPosition;
    
    /// The child on the positive side of the partition plane
    ModelContainerNode* posChild;
    
    /// The child on the negative side of the partition plane
    ModelContainerNode* negChild;
    
    virtual bool TryAddItem(Triangle* object, BoundingBox boundingBox, ModelContainerNode** outUpdatedNode);
    
    virtual bool TryAddItem(Triangle* object, BoundingBox boundingBox, Vector3D nominalPosition, bool* outFullyContainedByNode, ModelContainerNode** outUpdatedNode);
    
    virtual bool TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, BoundingBox boundingBox, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties, int *outNodesVisited, int *outNumTrianglesVisited);
    
    virtual TreeStatistics GetStatistics(int currentDepth);
private:
    // These are helper functions that basically perform an AddItem, not a TryAddItem, these will guarantee to add the item to the tree, difference
    // being that because of other threads potentially locking on the node we want to update, we may need to try multiple times before we finally make
    // the update while we wait for other threads to finish working with a node
    ModelContainerNode* AddItemWithWait(Triangle* object, ModelContainerNode* targetNode, BoundingBox boundingBox);
    ModelContainerNode* AddItemWithWait(Triangle* object, ModelContainerNode* targetNode, Vector3D nominalPosition, BoundingBox boundingBox, bool* outFullyContainedByNode);
};

#endif /* defined(__RayTracer__ModelContainerPartition__) */
