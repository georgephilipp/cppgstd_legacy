#include"image.h"
#include"ex.h"
#include<opencv2/highgui/highgui.hpp>

namespace msii810161816
{   
    namespace gstd
    {  
        namespace image
        {
            std::vector<Eigen::MatrixXd> cvtoeig(cv::Mat in)
            {              
                int nummaps = in.channels();
                std::vector<Eigen::MatrixXd> out(nummaps);
                for(int k=0;k<nummaps;k++)
                    out[k].resize(in.rows,in.cols);
                
                uint8_t* pixelPtr = (uint8_t*)in.data;
                for(int i = 0; i < in.rows; i++)
                    for(int j = 0; j < in.cols; j++)
                        for(int k = 0; k<nummaps; k++)
                            out[k](i,j) = pixelPtr[i*in.cols*nummaps + j*nummaps + k];
                
                return out;
            }     
            
            cv::Mat eigtocv(std::vector<Eigen::MatrixXd> in, int type /*=CV_8UC3*/)
            {       
                int nummaps = in.size();
                int rows = in[0].rows();
                int cols = in[0].cols();
                cv::Mat out(rows, cols, type);
                                
                uint8_t* pixelPtr = (uint8_t*)out.data;
                for(int i = 0; i < rows; i++)
                    for(int j = 0; j < cols; j++)
                        for(int k = 0; k<nummaps; k++)
                            pixelPtr[i*cols*nummaps + j*nummaps + k] = in[k](i,j);
                
                return out;
            }
        }
    }
}



