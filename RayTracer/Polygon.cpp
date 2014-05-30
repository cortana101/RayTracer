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
    this->vertices.append(startingTriangle.p1);
    this->vertices.append(startingTriangle.p2);
    this->vertices.append(startingTriangle.p3);
    this->startingColour = startingTriangle.colour;
    this->startingGloss = startingTriangle.gloss;
}

Triangle* Polygon::Triangulate(int* outNumTriangles)
{
    *outNumTriangles = this->vertices.length() - 2;
    
    Triangle* output = new Triangle[*outNumTriangles];
    
    for (int i = 0; i < *outNumTriangles; i++)
    {
        Triangle outputTriangle;
        
        // Always start with the vertex at 0
        outputTriangle.p1 = this->vertices.at(0);
        outputTriangle.p2 = this->vertices.at(1 + i);
        outputTriangle.p3 = this->vertices.at(2 + i);
        outputTriangle.colour = this->startingColour;
        outputTriangle.gloss = this->startingGloss;
        output[i] = outputTriangle;
    }
    
    return output;
}

void Polygon::Clip(PartitionPlaneType plane, double planePosition, PartitionKeepDirection keepDirection)
{
    // If we have a convex polygon, which is guaranteed with this class, and we clip the polygon along a plane
    // then by definition only a contiguous set of vertices may be clipped, ie its not possible for a disjoint
    // set of vertices to be clipped. This being the case, its sufficient to track the first and last indicies
    // of the vertices that got clipped
    
    QList<Vector3D> keepers;
    
    for (int i = 0; i < this->vertices.length(); i++)
    {
        double v1PlanePosition, v2PlanePosition;
        Vector3D v1 = this->vertices.at(i);
        
        int v2Index = (i + 1) >= this->vertices.length() ? 0 : i + 1;
        Vector3D v2 = this->vertices.at(v2Index);
        
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
                keepers.append(v1);
                keepers.append(intersectingVertex);
            }
            else
            {
                keepers.append(intersectingVertex);
            }

        }
        else if ((v1PlanePosition <= planePosition && keepDirection == PartitionKeepDirection::Negative) ||
                 (v1PlanePosition > planePosition && keepDirection == PartitionKeepDirection::Positive))
        {
            // In this case both vertices are keepers
            keepers.append(v1);
        }
    }
 
    // Overwrite our verticies with the clipped keepers
    this->vertices = keepers;
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

