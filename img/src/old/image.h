/* 
 * File:   image.h
 * Author: gschoenh
 *
 * Created on April 25, 2013, 7:15 PM
 */

#ifndef msii810161816_gstd_IMAGE_H
#define	msii810161816_gstd_IMAGE_H

#include"standard.h"
#include<eigen3/Eigen/Dense>
#include<opencv2/highgui/highgui.hpp>

namespace msii810161816
{   
    namespace gstd
    {  
        namespace image
        {
            std::vector<Eigen::MatrixXd> cvtoeig(cv::Mat in);
            cv::Mat eigtocv(std::vector<Eigen::MatrixXd> in, int type = CV_8UC3);
        }
    }
}



#endif	/* msii810161816_gstd_IMAGE_H */

