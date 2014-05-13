/* 
 * File:   Matrix.cpp
 * Author: gschoenh
 * 
 * Created on February 5, 2014, 9:08 PM
 */

#include "stdafx.h"


#include "Matrix.h"
#include "Printer.h"

namespace msii810161816
{   
    namespace gstd
    {   
        namespace matrix
        {
            bool test()
            {
                gstd::Printer::c("This is the test for gstd::matrix group of functions");
                
                //create
                {
                    std::vector<std::vector<int> > input = { { 1, 2, 3, 4 }, {5, 6, 7, 8}};
                    std::vector<std::vector<int> > input2 = { { 1, 2, 3, 4 }, {5, 6, 7, 8, 9}};
                    std::vector<int> target = {1,2,3,4,5,6,7,8};
                    if(create(input) != target)
                    {
                        gstd::Printer::c("Test failed on function create");
                        return false;
                    }
                    try
                    {
                        gstd::Printer::c("Expecting Error message ...");
                        create(input2);
                        gstd::Printer::c("Test failed on function create");
                        return false;
                    }
                    catch(std::exception e) {}                    
				}

				//to2d
				{
					std::vector<std::vector<int> > target = { { 1, 2, 3, 4 }, { 5, 6, 7, 8 } };
					std::vector<int> input = { 1, 2, 3, 4, 5, 6, 7, 8 };
					if (to2d(2, 4, input) != target)
					{
						gstd::Printer::c("Test failed on function create");
						return false;
					}
					try
					{
						gstd::Printer::c("Expecting Error message ...");
						to2d(3, 3, input);
						gstd::Printer::c("Test failed on function create");
						return false;
					}
					catch (std::exception e) {}
				}

				//isMatrix
				{
					std::vector<std::vector<int> > input1 = {};
					std::vector<std::vector<int> > input2 = { { 1, 2, 3 }, { 1, 2 } };
					std::vector<std::vector<int> > input3 = { { 1, 2, 3 }, { 1, 2, 3 }, { 3, 5, 7 } };
					if (!isMatrix(input1) || isMatrix(input2) || !isMatrix(input3))
					{
						gstd::Printer::c("Test failed on function isMatrix");
						return false;
					}
				}
                
                return true;               
            }
            
            
        }
    }
}
