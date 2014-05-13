/* 
 * File:   Datapoints.cpp
 * Author: gschoenh
 * 
 * Created on November 22, 2013, 11:59 AM
 */

#include "stdafx.h"

#include "Datapoints.h"

namespace msii810161816 
{
    namespace data 
    {  
        DpString::DpString()
        {
            delimiter = ',';
        }
        
        namespace datapoints
        {
            bool test()
            {
                gstd::Printer::c("This is the test for the data::Datapoints class of functions");
                
                //DpConverter
                {
                    DpDoubles input;
                    input.content.push_back(1);
                    input.content.push_back(2);
                    DpInts output = DpConverter<DpDoubles,DpInts>::get(input);
                    DpDoubles inverse = DpConverter<DpInts,DpDoubles>::get(output);
                    if(input.content != inverse.content)
                    {
                        gstd::Printer::c("Test failed on function DpConverter");
                        return false;
                    }
                    
                    return true;
                }
            }
            
        }        
    }
}