/* 
 * File:   LinearUpdateModel.cpp
 * Author: gschoenh
 * 
 * Created on April 10, 2013, 7:46 PM
 */

#include "LinearUpdateModel.h"

namespace msii810161816
{   
    namespace ml
    {
        LinearUpdateModel::LinearUpdateModel() 
        {
        }
        
        void LinearUpdateModel::push()
        {
            parameters += updates;
            updates = Eigen::MatrixXd::Zero(updates.rows(),updates.cols());
        }
    }
}
