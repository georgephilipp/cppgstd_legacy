/* 
 * File:   OptimizableFunction.h
 * Author: gschoenh
 *
 * Created on April 16, 2013, 6:37 PM
 */

#ifndef OPTIMIZABLEFUNCTION_H
#define	OPTIMIZABLEFUNCTION_H

#include "standard.h"

namespace msii810161816 
{
    namespace opt 
    {
        struct funcval
        {
            double value;
            std::vector<double> gradient;
            std::vector<std::vector<double>> hessian;
        };
        
        class OptimizableFunction {
        public:
            OptimizableFunction();
            virtual ~OptimizableFunction();
            
            virtual funcval objective(std::vector<double> arg);
        private:

        };
    }
}

#endif	/* OPTIMIZABLEFUNCTION_H */

