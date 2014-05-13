/* 
 * File:   LinearUpdateModel.h
 * Author: gschoenh
 *
 * Created on April 10, 2013, 7:46 PM
 */

#ifndef LINEARUPDATEMODEL_H
#define	LINEARUPDATEMODEL_H

#include"standard.h"

namespace msii810161816
{   
    namespace ml
    {
        class LinearUpdateModel {
        public:
            LinearUpdateModel();
            virtual ~LinearUpdateModel();

            Eigen::MatrixXd parameters;
            Eigen::MatrixXd updates;
            
            std::string upmethod;
            std::string regmethod;
            std::string recmethod;
            
            virtual Eigen::MatrixXd getupdate( std::vector<double> datapoints );
            void push();
            virtual Eigen::VectorXd regress( Eigen::VectorXd input );
            virtual Eigen::VectorXd reconstruct( Eigen::VectorXd input );
        private:

        };
    }
}


#endif	/* LINEARUPDATEMODEL_H */

