/* 
 * File:   Opt_SteepestDescent.h
 * Author: gschoenh
 *
 * Created on April 15, 2013, 7:55 PM
 */

#ifndef OPT_STEEPESTDESCENT_H
#define	OPT_STEEPESTDESCENT_H

#include"OptimizationScheme.h"

namespace msii810161816 
{
    namespace opt 
    {
        class Opt_SteepestDescent : public OptimizationScheme
        {
        public:
            Opt_SteepestDescent();
            virtual ~Opt_SteepestDescent();
            
            int maxruns;
            int learnrate;            
            
            virtual result run( std::vector<double> initial );
        private:

        };
    }
}
#endif	/* OPT_STEEPESTDESCENT_H */
/*
Eigen::VectorXd sgd(
        std::string target,
        Eigen::VectorXd parms,
        Eigen::MatrixXd data,
        hyperparms hyper,
        double (schedule)(int), 
        int batchsize,
        int numiter
)
{
    int numdata = data.rows();
    int numdim = data.cols();
    int numbatches = numdata / batchsize;
    int lastbatchsize = numdata - batchsize * (numbatches - 1);

    int i,j,currentbatchsize;
    objective obj;
    Eigen::MatrixXd batch;
    int cursor;
    for (i=0;i<numiter;i++)
    {
        cursor = 0;
        for (j=0;j<numbatches;j++)
        {
            if (j == numbatches - 1 )
                currentbatchsize = lastbatchsize;
            else
                currentbatchsize = batchsize;
            batch.resize(currentbatchsize, numdim);
            batch = data.block(cursor, 0, currentbatchsize, numdim);
            cursor = cursor + currentbatchsize;
            obj = manager( target, parms, batch, hyper );
            parms = parms - schedule(i)*obj.gradient;
        }
    }

    return parms;       
}

double schedule( int iter )
{
    double factor = iter;
    factor = factor / 3 + 1;
    factor = 0.1/factor;  
    return factor;
}*/