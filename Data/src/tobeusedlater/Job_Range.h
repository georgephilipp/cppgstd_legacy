/* 
 * File:   Job_Range.h
 * Author: gschoenh
 *
 * Created on April 11, 2013, 2:02 PM
 */

#ifndef JOB_RANGE_H
#define	JOB_RANGE_H

#include"standard.h"

namespace msii810161816 
{
    namespace data 
    {
        class Job_Range : public Job
        {
        public:
            Job_Range();
            virtual ~Job_Range();
            
            int begin;
            int length;
            int returnpoint;
            
            virtual gstd::trial<int> getindex();
        private:
            int nextserve;
            virtual void wrapext( std::stringstream &str);
            virtual void unwrapext( std::stringstream &str );
        };
    }
}

#endif	/* JOB_RANGE_H */

