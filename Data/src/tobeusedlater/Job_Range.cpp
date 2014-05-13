/* 
 * File:   Job_Range.cpp
 * Author: gschoenh
 * 
 * Created on April 11, 2013, 2:02 PM
 */

#include "Job_Range.h"

namespace msii810161816 
{
    namespace data 
    {
        Job_Range::Job_Range() : Job()
        {
            nextserve = -1;
        }
        
        virtual Job_Range::~Job_Range()
        {
        }
        
        virtual int Job_Range::getindex()
        {
            gstd::trial<int> out;
            if(nextserve == -1)
                nextserve = begin;
            if(numserved == length)
            {
                out.success = false;
                return out;
            }
            else
            {
                out.success = true;
                out.result = nextserve;
                numserved++;
                nextserve++;
                if(nextserve == returnpoint)
                    nextserve = 0;
            }
            return out;
        }
        
        virtual void Job_Range::wrapext( std::stringstream &str )
        {
            gstd::append<int>(str, begin);
            gstd::append<int>(str, length);
            gstd::append<int>(str, returnpoint);
            gstd::append<int>(str, nextserve);
        }
        
        virtual void Job_Range::unwrapext( std::stringstream &str )
        {
            begin = gstd::numparse<int>(str);
            length = gstd::numparse<int>(str);
            returnpoint = gstd::numparse<int>(str);
            nextserve = gstd::numparse<int>(str);
        }
    }
}


