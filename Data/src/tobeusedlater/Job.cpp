/* 
 * File:   Job.cpp
 * Author: gschoenh
 * 
 * Created on April 11, 2013, 1:23 PM
 */

#include "Job.h"

namespace msii810161816 
{
    namespace data 
    {
        Job::Job()
        {    
            numserved = 0;
        }
        
        Job::~Job()
        {    
        }

        std::string Job::wrap()
        {
            std::stringstream wrap;
            gstd::append<int>(wrap, rank);
            gstd::append<int>(wrap, method);
            gstd::append<int>(wrap, dataserver);
            gstd::append<int>(wrap, runtime);
            gstd::append<int>(wrap, success);
            gstd::append<int>(wrap, doupdate);
            gstd::append<int>(wrap, numserved); 
            
            wrapext(wrap);
                        
            return wrap.str();
        }
        
        void Job::unwrap( std::string str )
        {
            std::stringstream wrap;
            wrap << str;
            
            rank = gstd::numparse<int>(wrap);
            getline(wrap, method, ',');
            dataserver = gstd::numparse<int>(wrap);
            runtime = gstd::numparse<int>(wrap);
            success = gstd::numparse<int>(wrap);
            doupdate = gstd::numparse<int>(wrap);
            numserved = gstd::numparse<int>(wrap);
            
            unwrapext(wrap);
        }
        
        int Job::getindex()
        {
            throw("Virtual Routine");
        }
        
        Job *Job::init( std::string str )
        {
            Job *point = 0;
            
            std::stringstream stream;
            stream << str;
            std::string type;
            getline(str, type, ',');
            str.erase(0,str.length()+1);
            
            if (type == "Range")
                point = new Job_Range();
            else
            {
                gstd::print<string>("Unknown Job type");
                throw("");
            }
            
            point->unwrap(str);
            return point;            
        }
        
        void Job::wrapext( std::stringstream &str)
        {
            (void)str;
            throw("Virtual routine");
        }
                
        void Job::unwrapext( std::stringstream &str )
        {
            (void)str;
            throw("Virtual Routine");
        }
    }
}
