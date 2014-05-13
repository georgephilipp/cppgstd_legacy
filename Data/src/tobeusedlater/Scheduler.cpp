/* 
 * File:   Scheduler.cpp
 * Author: gschoenh
 * 
 * Created on April 11, 2013, 11:49 AM
 */

#include "Scheduler.h"

namespace msii810161816 
{
    namespace data 
    {        
        Scheduler::Scheduler()
        {
        }
        
        virtual Scheduler::~Scheduler()
        {
        }
        
        virtual Job *Scheduler::getjob( int rank )
        {
            (void)rank;
            throw("Virtual routine");
        }
        
        virtual void Scheduler::consumejob( Job &job )
        {
            (void)job;
            throw("Virtual routine");
        }
    }
}
    
