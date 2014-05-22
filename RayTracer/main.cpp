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

// Standard 4:3 format
#define XSIZE 800
#define YSIZE 600

using namespace std;

OutputRasterizer* writeSample();

int main(int argc, const char * argv[])
{
    Triangle model[5];
    model[0].p1 = Vector3D(0.0, 2.0, 3.0);
    model[0].p2 = Vector3D(1.5, -1.0, 4.0);
    model[0].p3 = Vector3D(-2.0, -1.0, 2.0);
    model[0].gloss = 0.5;
    model[0].colour = Colour(120, 1, 1);
    model[1].p1 = Vector3D(1.0, 1.0, 3.0);
    model[1].p2 = Vector3D(2.0, -0.5, 2.0);
    model[1].p3 = Vector3D(0.5, -0.5, 4.0);
    model[1].gloss = 0.8;
    model[1].colour = Colour(1, 50, 255);
    model[2].p1 = Vector3D(1.0, 1.0, 3.0);
    model[2].p2 = Vector3D(2.0, -0.5, 2.0);
    model[2].p3 = Vector3D(3.0, 2.0, 3.0);
    model[2].gloss = 0.6;
    model[2].colour = Colour(1, 50, 255);
    model[3].p1 = Vector3D(-100.0, -2.0, 0.0);
    model[3].p2 = Vector3D(100.0, -2.0, 0.0);
    model[3].p3 = Vector3D(-100.0, -2.0, 100.0);
    model[3].gloss = 0.3;
    model[3].colour = Colour(255, 255, 255);
    model[4].p1 = Vector3D(-100.0, -2.0, 100.0);
    model[4].p2 = Vector3D(100.0, -2.0, 0.0);
    model[4].p3 = Vector3D(100.0, -2.0, 100.0);
    model[4].gloss = 0.1;
    model[4].colour = Colour(255, 255, 255);

    
    // Make a light source directly overhead
    LightSource* light = new LightSource();
    light->position = new Vector3D(0.0, 1.0, 0.0);
    
    Tracer tracer;

    tracer.Render(model, 4, light, 1, 90, XSIZE, YSIZE).WriteToFile("out.ppm");
    
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
