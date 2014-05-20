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

using namespace std;

struct Point
{
    int rVal;
    int gVal;
    int bVal;
};

class OutputRasterizer
{
public:
    OutputRasterizer(int xSize, int ySize);
    ~OutputRasterizer();
    void SetOutput(int xPos, int yPos, int rVal, int gVal, int bVal);
    void WriteToFile(string filename);
private:
    int xSize;
    int ySize;
    Point* buffer;
};

#endif /* defined(__RayTracer__OutputRasterizer__) */

