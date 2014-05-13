/* 
 * File:   Sched_GreedyBatches.h
 * Author: gschoenh
 *
 * Created on April 11, 2013, 11:53 AM
 */

#ifndef SCHED_GREEDYBATCHES_H
#define	SCHED_GREEDYBATCHES_H
#include"standard.h"

namespace msii810161816 
{
    namespace data 
    {
        class Sched_GreedyBatches : public Scheduler
        {
        public:
            Sched_GreedyBatches();
            virtual ~Sched_GreedyBatches();
            int position;
            int batchsize;

            virtual Job *getjob( int rank ); 
            virtual void consumejob( Job &job );
        private:

        };
    }
}

#endif	/* SCHED_GREEDYBATCHES_H */

