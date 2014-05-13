/* 
 * File:   Sched_GreedyBatches.cpp
 * Author: gschoenh
 * 
 * Created on April 11, 2013, 11:53 AM
 */

#include "Sched_GreedyBatches.h"
#include "gstd/sequences.h"

namespace msii810161816 
{
    namespace data 
    {
        Sched_GreedyBatches::Sched_GreedyBatches() : Scheduler()
        {
            position = 0;
        }
        
        virtual Sched_GreedyBatches::~Sched_GreedyBatches()
        {
        }

        virtual Job *Sched_GreedyBatches::getjob(int rank)
        {
            Job_Range *range = new Job_Range();
            range->rank = rank;
            
            range->begin = position;
            range->length = batchsize;
            range->returnpoint = numpoints;
            
            position = (position + batchsize) % numpoints;
            
            Job *job = range;            
            return(job);
        }
        
        virtual void Sched_GreedyBatches::consumereport( Job &job )
        {
            (void)job;
        }
    }
}
