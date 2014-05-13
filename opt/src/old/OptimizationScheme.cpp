/* 
 * File:   OptimizationScheme.cpp
 * Author: gschoenh
 * 
 * Created on April 15, 2013, 7:36 PM
 */

#include "OptimizationScheme.h"


namespace msii810161816 
{
    namespace opt 
    {
        OptimizationScheme::OptimizationScheme() {}
        OptimizationScheme::~OptimizationScheme() {}
        
        result OptimizationScheme::run( std::vector<double> initial )
        {
            (void)initial;
            gstd::print<std::string>("Virtual routine");
            throw("");
        }
    }
}




