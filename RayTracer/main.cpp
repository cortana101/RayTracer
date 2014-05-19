//
//  main.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include <iostream>
#include "ppma_io.h"
#include "OutputRasterizer.h"

#define XSIZE 500
#define YSIZE 600

using namespace std;

OutputRasterizer* writeSample();

int main(int argc, const char * argv[])
{
    writeSample()->WriteToFile("out.ppm");

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
