/* 
 * File:   LUM_CRBM.h
 * Author: gschoenh
 *
 * Created on April 18, 2013, 9:26 PM
 */

#ifndef LUM_CRBM_H
#define	LUM_CRBM_H

#include"standard.h"
#include"LinearUpdateModel.h"

namespace msii810161816
{   
    namespace ml
    {
        class LUM_CRBM : public LinearUpdateModel
        {
        public:
            LUM_CRBM();
            virtual ~LUM_CRBM();
            
            int numvmaps;
            int numhmaps;
            int x;
            int y;
            int recepsize;
            int numruns;
                                    
            virtual Eigen::MatrixXd getupdate( std::vector<double> datapoints );
            virtual Eigen::VectorXd regress( Eigen::VectorXd input );
            virtual Eigen::VectorXd reconstruct( Eigen::VectorXd input );

        };
    }
}


#endif	/* LUM_CRBM_H */

