//
//  main.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "Colour.h"
#include "ppma_io.h"
#include "OutputRasterizer.h"
#include "Vector3D.h"
#include "ProjectionUtils.h"
#include "Triangle.h"
#include "Tracer.h"
#include "LightSource.h"
#include "Sphere.h"
#include "PlyFileParser.h"

#include "Polygon.h"

// Standard 4:3 format
#define XSIZE 1200
#define YSIZE 900

using namespace std;

OutputRasterizer* writeSample();

// Features remaining:
//      Diffusion
//      Diffuse light source
//      Refraction
//      Transparency
//      Read ply files
//      Model partitioning data structure
//      Anti-aliasing
// Stretch goals:
//      Post-render effects (e.g particles)
//      Volumetric fog

int main(int argc, const char * argv[])
{
    int modelLength;
    ModelObject** parsedModel = PlyFileParser::ParseFile("arrayOfTriangles.ply", &modelLength);
    /*  Quick and dirty polygon clipping test
     
    Triangle* testModel = new Triangle();
    testModel->p1 = Vector3D(0.0, 0.0, 2.0);
    testModel->p2 = Vector3D(1.0, 1.0, 3.0);
    testModel->p3 = Vector3D(2.0, 0.0, 4.0);
    testModel->colour = Colour(200, 200, 200);
    testModel->gloss = 200.0;
    
    Polygon testPolygon = Polygon(*testModel);
    testPolygon.Clip(PartitionPlaneType::X, 1.5, PartitionKeepDirection::Negative);
    testPolygon.Clip(PartitionPlaneType::Y, 0.9, PartitionKeepDirection::Negative);
    testPolygon.Clip(PartitionPlaneType::X, 0.3, PartitionKeepDirection::Positive);
    testPolygon.Clip(PartitionPlaneType::Z, 3.0, PartitionKeepDirection::Negative);
    
    int numTriangles;
    Triangle* triangles = testPolygon.Triangulate(&numTriangles);
    
    ModelObject* staticModel[numTriangles];
    for (int i = 0; i < numTriangles; i++)
    {
        staticModel[i] = &triangles[i];
    }*/
    
    // Make a light source directly overhead
    LightSource light[4];
    
    light[0].position = new Vector3D(2.0, 1.0, 0.0);
    light[0].intensity = 1.0;
    light[1].position = new Vector3D(6.5, 3.0, -1.0);
    light[1].intensity = 0.6;
    light[2].position = new Vector3D(-10.0, 10.0, 9.94);
    light[2].intensity = 0.4;
    light[3].position = new Vector3D(1.0, 1.0, 3.0);
    light[3].intensity = 1.0;
    
    Tracer tracer;
    
    cout << "Rendering...\n";

    tracer.Render(parsedModel, modelLength, light, 4, 90, XSIZE, YSIZE).WriteToFile("out.ppm");
    
    cout << "Done\n";
   
    return 0;
}


OutputRasterizer* writeSample()
{
    OutputRasterizer* output = new OutputRasterizer(XSIZE, YSIZE);
    
    for(int i = 0; i < XSIZE; i++)
    {
        for(int j = 0; j < YSIZE; j++)
        {
            if (i < (XSIZE / 2))
            {
                if (j < (YSIZE / 2))
                {
                    // Top left
                    output->SetOutput(i, j, 0, 0, 255);
                }
                else
                {
                    // Bottom left
                    output->SetOutput(i, j, 255, 0, 255);
                }
            }
            else
            {
                if (j < (YSIZE / 2))
                {
                    // Top right
                    output->SetOutput(i, j, 255, 0, 0);
                }
                else
                {
                    // Bottom right
                    output->SetOutput(i, j, 0, 255, 0);
                }
            }
        }
    }
    
    return output;
}
