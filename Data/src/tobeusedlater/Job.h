/* 
 * File:   Job.h
 * Author: gschoenh
 *
 * Created on April 11, 2013, 1:23 PM
 */

#ifndef JOB_H
#define	JOB_H
#include"standard.h"

namespace msii810161816 
{
    namespace data 
    {
        class Job {
        public:
            Job();
            virtual ~Job();
            int rank;
            std::string method;  
            int dataserver;
            int runtime;
            bool success;
            bool doupdate;
            
            std::string wrap();
            void unwrap( std::string str );
            virtual gstd::trial<int> getindex();
            
            Job &init( std::string str );
        protected:
            int numserved;
            virtual void wrapext( std::stringstream &str);
            virtual void unwrapext( std::stringstream &str );
        };
    }
}



#endif	/* JOB_H */

