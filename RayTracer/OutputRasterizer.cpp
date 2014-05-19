//
//  OutputRasterizer.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "OutputRasterizer.h"

OutputRasterizer::OutputRasterizer(int xSize, int ySize)
{
    this->xSize = xSize;
    this->ySize = ySize;
    this->buffer = new Point[xSize * ySize];
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
    
    for (int i = 0; i < (this->xSize * this->ySize); i++)
    {
        rArray[i] = this->buffer[i].rVal;
        gArray[i] = this->buffer[i].gVal;
        bArray[i] = this->buffer[i].bVal;
    }
    
    ppma_write(filename, this->xSize, this->ySize, rArray, gArray, bArray);
}

// Sets a particular point on the output buffer to the specified value
void OutputRasterizer::SetOutput(int xPos, int yPos, int rVal, int gVal, int bVal)
{
    Point* bufferPosition;
    bufferPosition = this->buffer + xPos + yPos * xSize;
    
    bufferPosition->rVal = rVal;
    bufferPosition->gVal = gVal;
    bufferPosition->bVal = bVal;
}
