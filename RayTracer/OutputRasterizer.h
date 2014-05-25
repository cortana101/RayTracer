//
//  OutputRasterizer.h
//  RayTracer
//
//  Created by Daniel Shih on 5/18/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__OutputRasterizer__
#define __RayTracer__OutputRasterizer__

#include <iostream>
#include "ppma_io.h"
#include "Colour.h"

using namespace std;

class OutputRasterizer
{
public:
    OutputRasterizer(int xSize, int ySize);
    ~OutputRasterizer();
    void SetOutput(int xPos, int yPos, double rVal, double gVal, double bVal);
    void WriteToFile(string filename);
private:
    int xSize;
    int ySize;
    Colour* buffer;
    double maxIntensity;
};

#endif /* defined(__RayTracer__OutputRasterizer__) */

