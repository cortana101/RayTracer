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

bool BoundingBox::Intersects(Triangle triangle)
{
    BoundingBox triangleBound = BoundingBox::GetMinimumBoundingBox(triangle);
    
    if (this->IsDisjoint(triangleBound))
    {
        return false;
    }
    
    if (this->Contains(triangle.p1) ||
        this->Contains(triangle.p2) ||
        this->Contains(triangle.p3))
    {
        return true;
    }
    else
    {
        // Top 4 vertices of this bounding box
        Vector3D vt1 = this->max;
        Vector3D vt2 = Vector3D(this->max.x, this->max.y, this->min.z);
        Vector3D vt3 = Vector3D(this->min.x, this->max.y, this->min.z);
        Vector3D vt4 = Vector3D(this->min.x, this->max.y, this->max.z);
        
        // The bottom 4 vertices
        Vector3D vb1 = Vector3D(this->max.x, this->min.y, this->max.z);
        Vector3D vb2 = Vector3D(this->max.x, this->min.y, this->min.z);
        Vector3D vb3 = this->min;
        Vector3D vb4 = Vector3D(this->min.x, this->min.y, this->max.z);
       
        // A list of triangles that describe the 6 sides of this bounding box
        Triangle outputTriangles[12];
        outputTriangles[0] = Triangle(vt1, vt2, vt3);
        outputTriangles[1] = Triangle(vt1, vt3, vt4);
        outputTriangles[2] = Triangle(vt1, vb1, vb2);
        outputTriangles[3] = Triangle(vt1, vb2, vt2);
        outputTriangles[4] = Triangle(vt1, vb1, vb4);
        outputTriangles[5] = Triangle(vt1, vb4, vt4);
        outputTriangles[6] = Triangle(vb3, vt3, vt2);
        outputTriangles[7] = Triangle(vb3, vt2, vb2);
        outputTriangles[8] = Triangle(vb3, vb4, vt4);
        outputTriangles[9] = Triangle(vb3, vt4, vt3);
        outputTriangles[10] = Triangle(vb3, vb4, vb1);
        outputTriangles[11] = Triangle(vb3, vb1, vb2);
        
        for (int i = 0; i < 12; i++)
        {
            if (outputTriangles[i].IntersectsWith(triangle))
            {
                return true;
            }
        }
    }

    return false;
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