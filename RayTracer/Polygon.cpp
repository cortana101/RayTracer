//
//  Polygon.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/29/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "Polygon.h"

Polygon::Polygon(Triangle startingTriangle)
{
    this->vertices[0] = startingTriangle.p1;
    this->vertices[1] = startingTriangle.p2;
    this->vertices[2] = startingTriangle.p3;
    this->vertexCount = 3;
    this->startingColour = startingTriangle.colour;
    this->startingGloss = startingTriangle.gloss;
}

Polygon::Polygon(Vector3D* vertices, int vertexCount, Colour colour, double gloss)
{
    for (int i = 0; i < vertexCount; i++)
    {
        this->vertices[i] = vertices[i];
    }
    
    this->vertexCount = vertexCount;
    this->startingColour = colour;
    this->startingGloss = gloss;
}

Polygon Polygon::Clip(PartitionPlaneType plane, double planePosition, PartitionKeepDirection keepDirection)
{
    // If we have a convex polygon, which is guaranteed with this class, and we clip the polygon along a plane
    // then by definition only a contiguous set of vertices may be clipped, ie its not possible for a disjoint
    // set of vertices to be clipped. This being the case, its sufficient to track the first and last indicies
    // of the vertices that got clipped
    
    Vector3D keepers[MAXVERTEXCOUNT];
    int keeperIterator = 0;
    
    for (int i = 0; i < this->vertexCount; i++)
    {
        double v1PlanePosition, v2PlanePosition;
        Vector3D v1 = this->vertices[i];
        
        int v2Index = (i + 1) >= this->vertexCount ? 0 : i + 1;
        Vector3D v2 = this->vertices[v2Index];
        
        if (plane == PartitionPlaneType::X)
        {
            v1PlanePosition = v1.x;
            v2PlanePosition = v2.x;
        }
        else if (plane == PartitionPlaneType::Y)
        {
            v1PlanePosition = v1.y;
            v2PlanePosition = v2.y;
        }
        else
        {
            v1PlanePosition = v1.z;
            v2PlanePosition = v2.z;
        }
        
        // If the position equals the plane position, we consider that to be in the negative side
        if ((v1PlanePosition > planePosition && planePosition >= v2PlanePosition) ||
            (v1PlanePosition <= planePosition && planePosition < v2PlanePosition))
        {
            Vector3D intersectingVertex = FindIntersectingVertex(plane, planePosition, v1, v2);
            
            if ((v1PlanePosition <= planePosition && keepDirection == PartitionKeepDirection::Negative) ||
                (v1PlanePosition > planePosition && keepDirection == PartitionKeepDirection::Positive))
            {
                keepers[keeperIterator] = v1;
                keeperIterator++;
                keepers[keeperIterator] = intersectingVertex;
                keeperIterator++;
            }
            else
            {
                keepers[keeperIterator] = intersectingVertex;
                keeperIterator++;
            }

        }
        else if ((v1PlanePosition <= planePosition && keepDirection == PartitionKeepDirection::Negative) ||
                 (v1PlanePosition > planePosition && keepDirection == PartitionKeepDirection::Positive))
        {
            // In this case both vertices are keepers
            keepers[keeperIterator] = v1;
            keeperIterator++;
        }
    }
 
    return Polygon(keepers, keeperIterator);
}

Polygon Polygon::Clip(BoundingBox boundingBox)
{
    int clippedVertexCount = 0;
    
    Polygon clippedPolygon = this->Clip(PartitionPlaneType::X, boundingBox.max.x, PartitionKeepDirection::Negative);
    clippedPolygon = clippedPolygon.Clip(PartitionPlaneType::Y, boundingBox.max.y, PartitionKeepDirection::Negative);
    clippedPolygon = clippedPolygon.Clip(PartitionPlaneType::Z, boundingBox.max.z, PartitionKeepDirection::Negative);
    clippedPolygon = clippedPolygon.Clip(PartitionPlaneType::X, boundingBox.min.x, PartitionKeepDirection::Positive);
    clippedPolygon = clippedPolygon.Clip(PartitionPlaneType::Y, boundingBox.min.y, PartitionKeepDirection::Positive);
    clippedPolygon = clippedPolygon.Clip(PartitionPlaneType::Z, boundingBox.min.z, PartitionKeepDirection::Positive);
    return clippedPolygon;
}

double Polygon::SurfaceArea()
{
    int numTriangles = this->vertexCount - 2;
    
    // Its possible after clipping we end up with no vertices left to form triangles, in which case the polygon just gets clipped out of existence
    if (numTriangles < 1)
    {
        return 0.0;
    }
    
    double surfaceArea = 0.0;
   
    for (int i = 0; i < numTriangles; i++)
    {
        // Always start with the vertex at 0
        Triangle triangle (this->vertices[0], this->vertices[1 + i], this->vertices[2 + i]);
        
        surfaceArea += triangle.SurfaceArea();
    }
    
    return surfaceArea;
}

Vector3D Polygon::FindIntersectingVertex(PartitionPlaneType plane, double planePosition, Vector3D vertex1, Vector3D vertex2)
{
    double v1Component, v2Component;
    
    if (plane == PartitionPlaneType::X)
    {
        v1Component = vertex1.x;
        v2Component = vertex2.x;
    }
    else if (plane == PartitionPlaneType::Y)
    {
        v1Component = vertex1.y;
        v2Component = vertex2.y;
    }
    else
    {
        v1Component = vertex1.z;
        v2Component = vertex2.z;
    }
    
    // Gets the scale factor for the vector v1->v2 such that scaled by this scale factor the vector
    // will point directly at the point of intersection with the plane
    double oneToTwoScaleFactor = (planePosition - v1Component) / (v2Component - v1Component);
    
    return vertex1.PointToPoint(vertex2).Scale(oneToTwoScaleFactor).Add(vertex1);
}

