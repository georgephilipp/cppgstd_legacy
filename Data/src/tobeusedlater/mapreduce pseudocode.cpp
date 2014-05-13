int main(int argc, char** argv) 
{
    (void)argc;
    (void)argv;
    
    //initialize servers
    data::server s1; //eg ifstream
    data::server s2(s1); //also a client
    data::ServerManager s3(s1,s2);
    
    //initialize scheduler
    data::scheduler sched;
    
    //initialize the model
    ml::model m1; 
    
    if (rank == 0)
    {
        for(i=1:1000)
        {
            job = sched.getjob(rank = 1);
            wrap = job.wrap;
            send(wrap);
            receive(report);
            sched.consume(report);
            receive(update);
            ml.update();
        }
    }
    else
    {
        receive(job);
        vector<vector> data;
        while(job.getindex())
            data &= s3.getpoint(index);
        ml.method = job.method;
        ml.update(data);
        if(job.report)
            send(update);
    }
    return 0;
}

