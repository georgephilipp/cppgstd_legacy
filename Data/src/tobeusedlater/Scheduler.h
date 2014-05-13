/* 
 * File:   Scheduler.h
 * Author: gschoenh
 *
 * Created on April 11, 2013, 11:49 AM
 */

#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include"standard.h"

namespace msii810161816 
{
    namespace data 
    {
        class Scheduler {
        public:
            Scheduler();
            virtual ~Scheduler();
            int numpoints;

            virtual Job *getjob( int rank ); 
            virtual void consumejob( Job &job );
        private:

        };
    }
}


#endif	/* SCHEDULER_H */

