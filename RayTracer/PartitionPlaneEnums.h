//
//  PartitionPlaneType.h
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef RayTracer_PartitionPlaneType_h
#define RayTracer_PartitionPlaneType_h

/// Defines the planes that the k-d tree nodes are partitioned by
enum PartitionPlaneType
{
    X,
    Y,
    Z
};

/// Defines whether we keep the positive side of a partition plane or the negative side
enum PartitionKeepDirection
{
    Positive,
    Negative
};

#endif
