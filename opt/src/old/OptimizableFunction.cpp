/* 
 * File:   OptimizableFunction.cpp
 * Author: gschoenh
 * 
 * Created on April 16, 2013, 6:37 PM
 */

#include "OptimizableFunction.h"

namespace msii810161816 
{
    namespace opt 
    {
        OptimizableFunction::OptimizableFunction() {}
        OptimizableFunction::~OptimizableFunction() {}
        
        funcval OptimizableFunction::objective(std::vector<double> arg)
        {
            (void)arg;
            gstd::print<std::string>("Virtual routine");
            throw("");
        }
    }
}

