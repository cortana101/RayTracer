//
//  ConsoleUtils.h
//  RayTracer
//
//  Created by Daniel Shih on 7/20/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__ConsoleUtils__
#define __RayTracer__ConsoleUtils__

#include <iostream>
#include <stdio.h>

using namespace std;

struct ProgressParams
{
    int* currentPosition;
    int total;
};

class ConsoleUtils
{
public:
    static void* PrintProgress(void *printProgressParams);
    static void StartProgressBar();
};

#endif /* defined(__RayTracer__ConsoleUtils__) */