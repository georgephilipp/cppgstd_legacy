/* 
 * File:   Opt_SteepestDescent.cpp
 * Author: gschoenh
 * 
 * Created on April 15, 2013, 7:55 PM
 */

#include "Opt_SteepestDescent.h"

namespace msii810161816 
{
    namespace opt 
    {
        Opt_SteepestDescent::Opt_SteepestDescent() {}
        Opt_SteepestDescent::~Opt_SteepestDescent() {}
        
        result Opt_SteepestDescent::run( std::vector<double> initial )
        {
            int i;
            std::vector<double> out = initial;
            for(i=0;i<maxruns;i++)
            {
                funcval val = objective->objective(out);
                int j;
                int s = out.size();
                for(j=0;j<s;j++)
                    out[j] = out[j] + learnrate * val.gradient[j];
            }
            result res;
            res.outparm = out;
            return res;
        }                
    }
}

