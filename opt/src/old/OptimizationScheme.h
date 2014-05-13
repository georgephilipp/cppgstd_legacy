/* 
 * File:   OptimizationScheme.h
 * Author: gschoenh
 *
 * Created on April 15, 2013, 7:36 PM
 */

#ifndef OPTIMIZATIONSCHEME_H
#define	OPTIMIZATIONSCHEME_H

#include"standard.h"
#include"OptimizableFunction.h"

namespace msii810161816 
{
    namespace opt 
    {      
        struct result
        {
            std::vector<double> outparm;
        };

        class OptimizationScheme {
        public:
            OptimizationScheme();
            virtual ~OptimizationScheme();

            OptimizableFunction* objective;
            virtual result run( std::vector<double> initial );
        private:

        };
    }
}

#endif	/* OPTIMIZATIONSCHEME_H */

