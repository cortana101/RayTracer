//
//  BoundingBox.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/30/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "BoundingBox.h"

BoundingBox::BoundingBox(Vector3D min, Vector3D max)
{
    this->min = min;
    this->max = max;
    
    if (max.x < min.x ||
        max.y < min.y ||
        max.z < min.z)
    {
        throw "Please ensure bounding box vectors are added in the correct order";
    }
}

BoundingBox::~BoundingBox()
{
    // Do nothing we have no pointers
}

BoundingBox BoundingBox::Constrain(PartitionPlaneType planeType, float partitionValue, PartitionKeepDirection keepDirection)
{
    Vector3D localMin = this->min;
    Vector3D localMax = this->max;
    
    if (planeType == PartitionPlaneType::X && keepDirection == PartitionKeepDirection::Positive)
    {
        localMin.x = partitionValue;
    }
    else if (planeType == PartitionPlaneType::X && keepDirection == PartitionKeepDirection::Negative)
    {
        localMax.x = partitionValue;
    }
    else if (planeType == PartitionPlaneType::Y && keepDirection == PartitionKeepDirection::Positive)
    {
        localMin.y = partitionValue;
    }
    else if (planeType == PartitionPlaneType::Y && keepDirection == PartitionKeepDirection::Negative)
    {
        localMax.y = partitionValue;
    }
    else if (planeType == PartitionPlaneType::Z && keepDirection == PartitionKeepDirection::Positive)
    {
        localMin.z = partitionValue;
    } 
    else
    { 
        localMax.z = partitionValue; 
    } 
    
    return BoundingBox (localMin, localMax);
}

BoundingBox BoundingBox::ExpandToContain(BoundingBox targetBoundingBox)
{
    Vector3D newMax = this->max;
    Vector3D newMin = this->min;
    
    newMax.x = std::max(this->max.x, targetBoundingBox.max.x);
    newMax.y = std::max(this->max.y, targetBoundingBox.max.y);
    newMax.z = std::max(this->max.z, targetBoundingBox.max.z);
    newMin.x = std::min(this->min.x, targetBoundingBox.min.x);
    newMin.y = std::min(this->min.y, targetBoundingBox.min.y);
    newMin.z = std::min(this->min.z, targetBoundingBox.min.z);
    
    return BoundingBox(newMin, newMax);
}

bool BoundingBox::IsDisjoint(BoundingBox otherBox)
{
    if (this->max.x < otherBox.min.x ||
        this->max.y < otherBox.min.y ||
        this->max.z < otherBox.min.z ||
        this->min.x > otherBox.max.x ||
        this->min.y > otherBox.max.y ||
        this->min.z > otherBox.max.z)
    {
        return true;
    }
    
    return false;
}

bool BoundingBox::Contains(Vector3D point)
{
    if (point.x > this->max.x ||
        point.x < this->min.x ||
        point.y > this->max.y ||
        point.y < this->min.y ||
        point.z > this->max.z ||
        point.z < this->min.z)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool BoundingBox::Contains(Triangle triangle)
{
    return this->Contains(triangle.p1) &&
            this->Contains(triangle.p2) &&
            this->Contains(triangle.p3);
}

BoundingBox BoundingBox::GetMinimumBoundingBox(Triangle object)
{
    double maxX = std::max(object.p1.x, std::max(object.p2.x, object.p3.x));
    double maxY = std::max(object.p1.y, std::max(object.p2.y, object.p3.y));
    double maxZ = std::max(object.p1.z, std::max(object.p2.z, object.p3.z));
    double minX = std::min(object.p1.x, std::min(object.p2.x, object.p3.x));
    double minY = std::min(object.p1.y, std::min(object.p2.y, object.p3.y));
    double minZ = std::min(object.p1.z, std::min(object.p2.z, object.p3.z));
    
    Vector3D min (minX, minY, minZ);
    Vector3D max (maxX, maxY, maxZ);
    
    return BoundingBox(min, max);
}

double BoundingBox::SurfaceArea()
{
    Vector3D diagonal = this->min.PointToPoint(this->max);
    
    return 2 * (diagonal.x * diagonal.y + diagonal.y * diagonal.z + diagonal.x * diagonal.z);
}

bool BoundingBox::TryGetIntersectionAtSurface(Vector3D ray, Vector3D rayOrigin, Vector3D* outIntersectAtSurface)
{
    struct IntersectScaleFactor
    {
        PartitionPlaneType partitionPlane;
        PartitionKeepDirection side;
    };
   
    IntersectScaleFactor scaleFactors[6];
    
    scaleFactors[0] = IntersectScaleFactor { PartitionPlaneType::X, PartitionKeepDirection::Positive };
    scaleFactors[1] = IntersectScaleFactor { PartitionPlaneType::X, PartitionKeepDirection::Negative };
    scaleFactors[2] = IntersectScaleFactor { PartitionPlaneType::Y, PartitionKeepDirection::Positive };
    scaleFactors[3] = IntersectScaleFactor { PartitionPlaneType::Y, PartitionKeepDirection::Negative };
    scaleFactors[4] = IntersectScaleFactor { PartitionPlaneType::Z, PartitionKeepDirection::Positive };
    scaleFactors[5] = IntersectScaleFactor { PartitionPlaneType::Z, PartitionKeepDirection::Negative };

    Vector3D closestIntersect;
    Vector3D originToClosestIntersect;
    bool haveFoundAnIntersect = false;
    
    for (int i = 0; i < 6; i++)
    {
        Vector3D outIntersectPosition;
        Vector3D originToIntersect = rayOrigin.PointToPoint(outIntersectPosition);
        
        if (this->TryGetIntersectionAtSurface(ray, rayOrigin, scaleFactors[i].partitionPlane, scaleFactors[i].side, &outIntersectPosition))
        {
            if (!haveFoundAnIntersect || originToClosestIntersect.GetMagnitude() > originToIntersect.GetMagnitude())
            {
                haveFoundAnIntersect = true;
                closestIntersect = outIntersectPosition;
                originToClosestIntersect = rayOrigin.PointToPoint(closestIntersect);
            }
        }
    }

    if (haveFoundAnIntersect)
    {
        *outIntersectAtSurface = closestIntersect;
        return true;
    }
    else
    {
        return false;
    }
}

bool BoundingBox::TryGetIntersectionAtSurface(Vector3D ray, Vector3D rayOrigin, PartitionPlaneType plane, PartitionKeepDirection side, Vector3D* outIntersectAtSurface)
{
    Vector3D originToRelevantVertex;
    Vector3D localIntersectAtSurface;
    
    if (side == PartitionKeepDirection::Positive)
    {
        originToRelevantVertex = rayOrigin.PointToPoint(this->max);
    }
    else
    {
        originToRelevantVertex = rayOrigin.PointToPoint(this->min);
    }
    
    double scaleFactor;
    Vector3D planeIntersect;
    bool hasIntersect = false;
    
    if (plane == PartitionPlaneType::X)
    {
        scaleFactor = originToRelevantVertex.x / ray.x;
    }
    else if (plane == PartitionPlaneType::Y)
    {
        scaleFactor = originToRelevantVertex.y / ray.y;
    }
    else
    {
        scaleFactor = originToRelevantVertex.z / ray.z;
    }
    
    // Slight redundancy in plane detection logic here, to speed up the return of the scale factor is negative
    if (!isnan(scaleFactor) && scaleFactor > 0.0)
    {
        planeIntersect = rayOrigin.Add(ray.Scale(scaleFactor));
        localIntersectAtSurface = planeIntersect;

        if (plane == PartitionPlaneType::X)
        {
            hasIntersect = planeIntersect.y >= this->min.y &&
                            planeIntersect.y <= this->max.y &&
                            planeIntersect.z >= this->min.z &&
                            planeIntersect.z <= this->max.z;
        }
        else if (plane == PartitionPlaneType::Y)
        {
            hasIntersect = planeIntersect.x >= this->min.x &&
                            planeIntersect.x <= this->max.x &&
                            planeIntersect.z >= this->min.z &&
                            planeIntersect.z <= this->max.z;
        }
        else
        {
            hasIntersect = planeIntersect.x >= this->min.x &&
                            planeIntersect.x <= this->max.x &&
                            planeIntersect.y >= this->min.y &&
                            planeIntersect.y <= this->max.y;
        }
        
        if (hasIntersect)
        {
            *outIntersectAtSurface = localIntersectAtSurface;
        }
    }
    
    return hasIntersect;
}

PartitionPlaneType BoundingBox::GetLongestEdge()
{
    Vector3D diagonal = this->min.PointToPoint(this->max);
    
    PartitionPlaneType longestEdge = PartitionPlaneType::X;
    
    if (diagonal.y > diagonal.x)
    {
        longestEdge = PartitionPlaneType::Y;
    }
    
    if (diagonal.z > diagonal.y && diagonal.z > diagonal.x)
    {
        longestEdge = PartitionPlaneType::Z;
    }
    
    return longestEdge;
}

double BoundingBox::GetLength(PartitionPlaneType axis)
{
    Vector3D diagonal = this->min.PointToPoint(this->max);
    
    if (axis == PartitionPlaneType::X)
    {
        return diagonal.x;
    }
    else if (axis == PartitionPlaneType::Y)
    {
        return diagonal.y;
    }
    else
    {
        return diagonal.z;
    }
}

double BoundingBox::GetMinInAxis(PartitionPlaneType axis)
{
    if (axis == PartitionPlaneType::X)
    {
        return this->min.x;
    }
    else if (axis == PartitionPlaneType::Y)
    {
        return this->min.y;
    }
    else
    {
        return this->min.z;
    }
}