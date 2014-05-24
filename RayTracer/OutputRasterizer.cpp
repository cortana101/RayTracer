//
//  OutputRasterizer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "OutputRasterizer.h"
#define EXPOSURESCALEFACTOR 0.03

OutputRasterizer::OutputRasterizer(int xSize, int ySize)
{
    this->xSize = xSize;
    this->ySize = ySize;
    this->buffer = new Colour[xSize * ySize];
}

OutputRasterizer::~OutputRasterizer()
{
    delete buffer;
}

void OutputRasterizer::WriteToFile(string filename)
{
    int* rArray = new int[this->xSize * this->ySize];
    int* gArray = new int[this->xSize * this->ySize];
    int* bArray = new int[this->xSize * this->ySize];
    
    double maxIntensity = 0.0;
    
    for (int i = 0; i < (this->xSize * this->ySize); i++)
    {
        double intensity = buffer[i].Intensity();
        
        if (intensity > maxIntensity)
        {
            maxIntensity = intensity;
        }
    }
    
    for (int i = 0; i < (this->xSize * this->ySize); i++)
    {
        buffer[i].Scale(255 / (maxIntensity * EXPOSURESCALEFACTOR));
        buffer[i].ApplyLimits();
        
        rArray[i] = this->buffer[i].rVal;
        gArray[i] = this->buffer[i].gVal;
        bArray[i] = this->buffer[i].bVal;
    }
    
    ppma_write(filename, this->xSize, this->ySize, rArray, gArray, bArray);
}

// Sets a particular point on the output buffer to the specified value
void OutputRasterizer::SetOutput(int xPos, int yPos, double rVal, double gVal, double bVal)
{
    Colour* bufferPosition;
    bufferPosition = this->buffer + xPos + yPos * xSize;
    
    bufferPosition->rVal = rVal;
    bufferPosition->gVal = gVal;
    bufferPosition->bVal = bVal;
}
