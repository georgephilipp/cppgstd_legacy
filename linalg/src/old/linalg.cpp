



#include"linalg.h"
#include"ex.h"
#include<eigen3/Eigen/Dense>

namespace msii810161816
{   
    namespace gstd
    {
        extern std::vector<bool> mute;
        
        namespace linalg
        {
            Eigen::MatrixXd eigenmatrix (std::vector<std::vector<double> > a)
            {
                int d1 = a.size();
                int d2 = a[0].size();
                
                Eigen::MatrixXd amat(d1,d2);
                
                int i,j;
                
                for(i=0;i<d1;i++)
                    for(j=0;j<d2;j++)
                        amat(i,j)=a[i][j];
                
                return amat;
            }
            
            std::vector<std::vector<double> > stdmatrix (Eigen::MatrixXd amat)
            {
                int d1 = amat.rows();
                int d2 = amat.cols();
                
                std::vector<std::vector<double> > a;
                a.resize(d1);
                
                int i,j;
                
                for(i=0;i<d1;i++)
                {
                    a[i].resize(d2);
                    for(j=0;j<d2;j++)
                        a[i][j]=amat(i,j);
                }
                    
                return a;
            }
            
            Eigen::MatrixXd matfromvec(Eigen::VectorXd vector, int numrows, bool colwise /*= true*/)
            {
                Eigen::MatrixXd out;

                int size = vector.rows();
                int numcols = size / numrows;
                if (numcols * numrows != size)
                    gstd::error("Dimensions do not agree");
                    
                out.resize(numrows, numcols);
                int i;
                int j = 0;
                
                if(colwise)
                {
                    for (i=0;i<numcols;i++)
                    {
                        out.block(0,i,numrows,1) = vector.block(j,0,numrows,1);
                        j = j+numrows;
                    }
                }
                else
                {
                    for (i=0;i<numrows;i++)
                    {
                        out.block(i,0,1,numcols) = vector.block(j,0,numcols,1).transpose();
                        j = j+numcols;
                    }
                }
                
                return out;
            }
            
            /*Eigen::MatrixXd random(int numrows, int numcols)
            {
                Eigen::MatrixXd mat(numrows, numcols);
                int i,j, r;
                for(i=0;i<numrows;i++)
                    for(j=0;j<numrows;j++)
                    {
                        
                    }
                        mat(i,j) = rand();
                
                //mat.setRandom(numrows, numcols);
                return mat;
            }*/
            
            Eigen::VectorXd vecfrommat(Eigen::MatrixXd mat, bool colwise /*= true*/)
            {
                int cols, rows;
                cols = mat.cols();
                rows = mat.rows();
                Eigen::VectorXd out(cols*rows);
                int i,j;
                j=0;
                if(colwise)
                {
                    for(i=0;i<cols;i++)
                    {
                        out.segment(j,rows) = mat.block(0,i,rows,1);
                        j = j+rows;
                    }
                }   
                else
                {
                    for(i=0;i<rows;i++)
                    {
                        out.segment(j,cols) = mat.block(i,0,1,cols).transpose();
                        j = j+cols;
                    }
                }
                return out;
            }
            
            void mprint(Eigen::MatrixXd mat, int id /*=-1*/)
            {
                if(id == -1 || id >= (int)mute.size() || !mute[id])
                {
                    int cols = mat.cols();
                    int rows = mat.rows();
                    int i,j = 0;
                    for(i=0;i<rows;i++)
                        for(j=0;j<cols;j++)
                        {
                            std::cout << mat(i,j);
                            if(j<cols-1)
                                std::cout << " ";
                            else
                                std::cout << std::endl << std::flush;
                        }  
                    std::cout << std::endl << std::flush;
                }
            }
            
            Eigen::MatrixXd rescale(Eigen::MatrixXd in, int numrows, int numcols)
            {
                Eigen::MatrixXd out(numrows, numcols);
                int i,j,oldrows,oldcols;
                oldrows = in.rows();
                oldcols = in.cols();
                int rfraction, cfraction, rcursor, ccursor;
                double crat, rrat;
                double val;
                rfraction = 0;
                rcursor = 0;
                for(i=0;i<numrows;i++)
                {
                    rrat = 1 - (double)rfraction / (double)(numrows - 1);               
                    cfraction = 0;
                    ccursor = 0;
                    for(j=0;j<numcols;j++)
                    {
                        crat = 1 - (double)cfraction / (double)(numcols - 1);   
                        val = crat * rrat * in(rcursor, ccursor);
                        if(cfraction > 0)
                            val = val + (1 - crat) * rrat * in(rcursor, ccursor + 1);
                        if(rfraction > 0)
                            val = val + crat * (1 - rrat) * in(rcursor + 1, ccursor);
                        if(cfraction > 0 && rfraction > 0)
                            val = val + (1 - crat) * (1 - rrat) * in(rcursor + 1, ccursor + 1);
                        out(i,j) = val;
                        cfraction = cfraction + oldcols - 1;
                        while(cfraction >= numcols - 1)
                        {
                            cfraction = cfraction - (numcols - 1);
                            ccursor = ccursor + 1;
                        }
                    }
                    rfraction = rfraction + oldrows - 1;
                    while(rfraction > numrows - 1)
                    {
                        rfraction = rfraction - (numrows - 1);
                        rcursor = rcursor + 1;
                    }
                }
                return out;                
            }
        }
    }
}



