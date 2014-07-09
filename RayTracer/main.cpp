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
#include "ModelContainer.h"
#include <time.h>

#include "BoundingBox.h"
#include "Polygon.h"

// Standard 4:3 format
#define XSIZE 1200
#define YSIZE 900

using namespace std;

OutputRasterizer* writeSample();
vector<Triangle*> concatToModelVector(vector<Triangle*> existingModel, ModelObject** model, int modelLength);

// Features remaining:
//      Diffusion
//      Diffuse light source
//      Refraction
//      Transparency
//      Model partitioning data structure
//      Adaptive Anti-aliasing (only do super sampling near edges)
// Stretch goals:
//      Post-render effects (e.g particles)
//      Volumetric fog

int main(int argc, const char * argv[])
{
    time_t startTime = time(NULL);
    
    // TODO: To really understand how our tree improvements are performing, we need a few more metrics:
        // Avg Number of node intersects per traceray
        // Avg number of triangle intersects per traceray
    // We also may want to cache the surface areas of triangles in a node between insertions to speed it up, we can handle the memory overhead by
    // keeping the precomputed surface areas in a separate array in memory and freeing that as we go.
    // But the main thing is to multithread the construction.
    
    //ModelObject** parsedModel = PlyFileParser::ParseFile("/Users/cortana101/Library/Developer/Xcode/DerivedData/RayTracer-enlhyosbakvceicrngnnfsyssdwm/Build/Products/Debug/bunnyOutput.ply", &modelLength, 15.0, Vector3D(-2.0, -2.0, 4.0));
    vector<Triangle*> parsedModel2 = PlyFileParser::ParseFile("/Users/cortana101/Library/Developer/Xcode/DerivedData/RayTracer-enlhyosbakvceicrngnnfsyssdwm/Build/Products/Debug/bunnyOutput.ply", 10.0, Vector3D(-1.5, -1.0, 3.0));
    //ModelObject** parsedModel3 = PlyFileParser::ParseFile("/Users/cortana101/Library/Developer/Xcode/DerivedData/RayTracer-enlhyosbakvceicrngnnfsyssdwm/Build/Products/Debug/arrayOfTrianglesLarge.ply", &modelLength3);
    
    vector<Triangle*> dragonModel = PlyFileParser::ParseFile("/Users/cortana101/Library/Developer/Xcode/DerivedData/RayTracer-enlhyosbakvceicrngnnfsyssdwm/Build/Products/Debug/dragonOutput.ply", 15.0, Vector3D(0.6, -1.5, 3.0));
    
    time_t finishedParsing = time(NULL);
    
    double parsingSeconds = difftime(finishedParsing, startTime);
    cout << "Finished parsing model, elapsed time: " << parsingSeconds << "\n";
    
    // Add the bunny to the dragon model
    dragonModel.insert(dragonModel.end(), parsedModel2.begin(), parsedModel2.end());

    ModelContainer modelContainer;
    modelContainer.BuildTree(dragonModel);
    
    time_t finishedBuildingModel = time(NULL);
   
    double modelBuildSeconds = difftime(finishedBuildingModel, finishedParsing);
    
    cout << "Finished building model, elapsed time: " << modelBuildSeconds << "\n";
    
    modelContainer.PrintTreeStatistics();
    
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

    tracer.Render(modelContainer, light, 4, 90, XSIZE, YSIZE, false).WriteToFile("out.ppm");
    
    time_t finishedRendering = time(NULL);
    
    cout << "Done\n";
    
    modelContainer.PrintTraceStatistics();
    
    double renderingSeconds = difftime(finishedRendering, finishedBuildingModel);
    cout << "Finished rendering, elapsed time: " << renderingSeconds << "\n";
   
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