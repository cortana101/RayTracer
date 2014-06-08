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
    
    virtual ModelContainerNode* AddItem(Triangle* object, BoundingBox boundingBox);
    
    virtual ModelContainerNode* AddItem(Triangle* object, BoundingBox boundingBox, Vector3D nominalPosition, bool* outFullyContainedByNode);
    
    virtual bool TraceRay(Vector3D ray, Vector3D rayOrigin, Vector3D raySearchPosition, BoundingBox boundingBox, ModelObject* ignoredObject, ModelObject** outIntersectedModel, IntersectProperties* outIntersectProperties);
    
    virtual TreeStatistics GetStatistics(int currentDepth);
};

#endif /* defined(__RayTracer__ModelContainerPartition__) */
