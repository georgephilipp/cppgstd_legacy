/* 
 * File:   linalg.h
 * Author: gschoenh
 *
 * Created on April 18, 2013, 9:43 PM
 */

#ifndef msii810161816_gstd_LINALG_H
#define	msii810161816_gstd_LINALG_H

#include "standard.h"
#include<eigen3/Eigen/Dense>

namespace msii810161816
{   
    namespace gstd
    {
        namespace linalg
        {
            struct funcval
            {
                double value;
                Eigen::VectorXd gradient;
                Eigen::MatrixXd hessian;
            };
            
            Eigen::MatrixXd eigenmatrix (std::vector<std::vector<double> > a);
            std::vector<std::vector<double> > stdmatrix (Eigen::MatrixXd amat);
            Eigen::MatrixXd matfromvec(Eigen::VectorXd vector, int numrows, bool colwise = true);
            Eigen::VectorXd vecfrommat(Eigen::MatrixXd vector, bool colwise = true);
            //Eigen::MatrixXd random(int numrows, int numcols);
            void mprint(Eigen::MatrixXd mat, int id = -1);
            Eigen::MatrixXd rescale(Eigen::MatrixXd in, int numrows, int numcols);
        }
    }
}

#endif	/* msii810161816_gstd_LINALG_H */

