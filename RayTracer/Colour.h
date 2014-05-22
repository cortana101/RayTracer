//
//  Colour.h
//  RayTracer
//
//  Created by Daniel Shih on 5/21/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef RayTracer_Colour_h
#define RayTracer_Colour_h

struct Colour
{
    int rVal;
    int gVal;
    int bVal;
    
    Colour()
    {
        rVal = 0;
        gVal = 0;
        bVal = 0;
    }
    
    Colour(int r, int g, int b)
    {
        rVal = r;
        gVal = g;
        bVal = b;
    }
    
    void Scale(float scaleFactor)
    {
        rVal *= scaleFactor;
        gVal *= scaleFactor;
        bVal *= scaleFactor;
        
        ApplyLimits();
    }
    
    void Add(Colour c2)
    {
        rVal += c2.rVal;
        gVal += c2.gVal;
        bVal += c2.bVal;
        
        ApplyLimits();
    }
    
    void ApplyLimits()
    {
        if (rVal < 0)
        {
            rVal = 0;
        }
        
        if (rVal > 255)
        {
            rVal = 255;
        }
        
        if (gVal < 0)
        {
            gVal = 0;
        }
        
        if (gVal > 255)
        {
            gVal = 255;
        }
        
        if (bVal < 0)
        {
            bVal = 0;
        }
        
        if (bVal > 255)
        {
            bVal = 255;
        }
    }
};

#endif
