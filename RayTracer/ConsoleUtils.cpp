//
//  ConsoleUtils.cpp
//  RayTracer
//
//  Created by Daniel Shih on 7/20/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "ConsoleUtils.h"
#include <unistd.h>

#define UPDATEPROGRESSDELAYMS 100
#define CONSOLEPROGRESSLENGTH 60

void* ConsoleUtils::PrintProgress(void *printProgressParams)
{
    ProgressParams* printProgressParameters = (ProgressParams*)printProgressParams;
    
    int stepSize = printProgressParameters->total / CONSOLEPROGRESSLENGTH;
    int lastStepPosition = 0;
    
    while(*printProgressParameters->currentPosition < printProgressParameters->total)
    {
        int progressSteps = (*printProgressParameters->currentPosition - lastStepPosition * stepSize) / stepSize;
        
        for (int i = 0; i < progressSteps; i++)
        {
            cout << "=";
            cout.flush();
        }
        
        lastStepPosition += progressSteps;
        usleep(UPDATEPROGRESSDELAYMS * 1000);
    }
    
    pthread_exit(NULL);
}

void ConsoleUtils::StartProgressBar()
{
    cout << "|";
    
    // Start at 2 to account for the start and end pipe chars
    for(int i = 2; i < CONSOLEPROGRESSLENGTH; i++)
    {
        cout << ".";
    }
    
    cout << "|\n";
    cout.flush();
}

