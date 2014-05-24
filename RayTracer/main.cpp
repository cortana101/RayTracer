//
//  main.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include <iostream>
#include "Colour.h"
#include "ppma_io.h"
#include "OutputRasterizer.h"
#include "Vector3D.h"
#include "ProjectionUtils.h"
#include "Triangle.h"
#include "Tracer.h"
#include "LightSource.h"
#include "Sphere.h"

// Standard 4:3 format
#define XSIZE 800
#define YSIZE 600

using namespace std;

OutputRasterizer* writeSample();

// Features remaining:
//      Diffusion
//      Diffuse light source
//      Refraction
//      Transparency
//      Exposure control
//      Read ply files
//      Model partitioning data structure
//      Multi threading
//      Anti-aliasing
// Stretch goals:
//      Post-render effects (e.g particles)
//      Volumetric fog

int main(int argc, const char * argv[])
{
    ModelObject* model[6];
    Triangle model0, model1, model2, model3, model4;
    Sphere sphere0;
    model0.p1 = Vector3D(-0.5, 2.0, 4.5);
    model0.p2 = Vector3D(1.5, -1.9, 5.7);
    model0.p3 = Vector3D(-2.5, -1.9, 3.2);
    model0.gloss = 600.0;
    model0.colour = Colour(120, 10, 10);
    model1.p1 = Vector3D(0.5, 1.0, 3.0);
    model1.p2 = Vector3D(1.0, -0.5, 2.0);
    model1.p3 = Vector3D(0.5, -0.5, 4.0);
    model1.gloss = 600.0;
    model1.colour = Colour(1, 50, 255);
    model2.p1 = Vector3D(0.5, 1.0, 3.0);
    model2.p2 = Vector3D(1.0, -0.5, 2.0);
    model2.p3 = Vector3D(3.0, 1.5, 3.0);
    model2.gloss = 100.0;
    model2.colour = Colour(1, 50, 255);
    model3.p1 = Vector3D(-3.0, -2.0, 4.0);
    model3.p2 = Vector3D(1.0, -2.0, 4.0);
    model3.p3 = Vector3D(-3.0, -2.0, 6.0);
    model3.gloss = 50.0;
    model3.colour = Colour(60, 60, 60);
    model4.p1 = Vector3D(-2.4, -2.0, 3.0);
    model4.p2 = Vector3D(-1.4, -2.0, 3.75);
    model4.p3 = Vector3D(-1.0, -2.0, 2.5);
    model4.gloss = 50.0;
    model4.colour = Colour(60, 60, 60);
    sphere0 = Sphere(Vector3D(0.0, 0.0, 2.0), 0.25);
    sphere0.gloss = 500.0;
    sphere0.colour = Colour(10, 255, 10);
    
    model[0] = &model0;
    model[1] = &model1;
    model[2] = &model2;
    model[3] = &model3;
    model[4] = &model4;
    model[5] = &sphere0;
    
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

    tracer.Render(model, 6, light, 4, 90, XSIZE, YSIZE).WriteToFile("out.ppm");
    
    // insert code here...
    cout << "Wrote to file\n";
    
    // General flow:
        // Need a class for handling projection of the viewport and generating the rays from the camera
        // Need a thing to read models from a file
        // Need to implement tracing 1 ray, and recursively implement for all rays from the projection
    
    
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
