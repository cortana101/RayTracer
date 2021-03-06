//
//  Colour.h
//  RayTracer
//
//  Created by Daniel Shih on 5/21/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef RayTracer_Colour_h
#define RayTracer_Colour_h
#define BASECOLOURMAX 255

#include <sstream>

struct Colour
{
    double rVal;
    double gVal;
    double bVal;
    
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
    
    Colour Scale(float scaleFactor)
    {
        return Colour(rVal * scaleFactor, gVal * scaleFactor, bVal * scaleFactor);
    }
    
    Colour Add(Colour c2)
    {
        return Colour(rVal + c2.rVal, gVal + c2.gVal, bVal + c2.bVal);
    }
    
    Colour Multiply(Colour c2)
    {
        if (rVal > BASECOLOURMAX || gVal > BASECOLOURMAX || bVal > BASECOLOURMAX)
        {
            std::stringstream errorString;
            errorString << "Should not be multiplying non-base colours together, base colours cannot have any of its values over " << BASECOLOURMAX;
            throw errorString.str();
        }
        
        return Colour((rVal / BASECOLOURMAX) * c2.rVal, (gVal / BASECOLOURMAX) * c2.gVal, (bVal / BASECOLOURMAX) * c2.bVal);
    }
    
    double Intensity()
    {
        return rVal + gVal + bVal;
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
