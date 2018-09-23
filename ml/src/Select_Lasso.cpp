/* 
 * File:   Lasso.cpp
 * Author: gschoenh
 * 
 * Created on November 14, 2013, 11:34 AM
 */

#include "stdafx.h"

#include "Lasso.h"
#include "gstd/src/ex.h"
#include "gstd/src/Stat.h"
#include "gstd/src/Rand.h"
#include "gstd/src/Linalg.h"
#include "gstd/src/Primitives.h"
#include "gstd/src/Map.h"
#include "gstd/src/Timer.h"

using namespace msii810161816::gstd::Linalg;

namespace msii810161816
{   
    namespace ml
    {
        Lasso::Lasso() 
		{
			initialparameterscale = 0;
			numiter = 1000;
		}
		
        /*Lasso::Lasso(const Lasso& orig) {
        }*/

        Lasso::~Lasso() {}
 
        void Lasso::initparameter()
        {
            gstd::trial<DpRegression> point = client.get(false)->get(0);
            gstd::check(point.success, "need data to initialize parameter");
            int d = point.result.input.size();
            
            //initialize parameter to small random values
            parameter = std::vector<double>(d);
            for(int i=0;i<d;i++)
                parameter[i] = initialparameterscale*(2*gstd::Rand::d(3)-1);
        }
        
        void Lasso::fit()
        {   
            //prelims
            aggregator.inServer.set(client.get(false), false);
            aggregator.fill(0, maxDataPoint);
            if(aggregator.input.size() == 0)
                gstd::error("Model not linked to data server");
            if(parameter.size() == 0)
                initparameter();            
            int N = aggregator.output.size();
            int d = aggregator.input.size() / N;
            
            gstd::check(N*d == (int)aggregator.input.size(),"incorrect input or output size");    
            gstd::check(d == (int)parameter.size(),"incorrect parameter size" );
                        
            //compute the initial error
            std::vector<double> errorvector = std::vector<double>(N);
            for(int n=0;n<N;n++)
            {
                errorvector[n] = -aggregator.output[n];            
                for(int i=0;i<d;i++)
                    errorvector[n] += aggregator.input[n*d + i]*parameter[i];
            }
            
            if(safe)
            {
                std::vector<double> i1 = mmult(N,d,1,aggregator.input,parameter);
                std::vector<double> i2 = msub(i1,aggregator.output);
                std::vector<double> i2abs = mabs(i2);
                double margin = gstd::vector::vmax(i2abs).second * 1e-13;
                gstd::check(mequals(i2, errorvector, margin, false), "matrix multiplication check failed");          
            }
            
            //compute the component square norms
            std::vector<double> sqnorms = std::vector<double>(d);
            for(int i=0;i<d;i++)
            {
                sqnorms[i] = 0;
                for(int n=0;n<N;n++)
                    sqnorms[i] += aggregator.input[n*d + i]*aggregator.input[n*d + i];
            }
            
            if(safe)
            {
                std::vector<double> i1 = mprod(aggregator.input, aggregator.input);
                std::vector<double> summedcols = msumcols(N, d, i1);
                if(!mequals(sqnorms, summedcols, 1e-12))
                    gstd::check(mequals(sqnorms,summedcols,1e-12), "matrix multiplication check failed");
            }
           
            gstd::Timer t;
            t.name = "ml::Lasso";
            t.channel = "ml::Lasso";
            
			//iterate
            for(int iter = 0;iter != numiter;iter++)
            {          
				//init termination criterion
				double maxupdate = 0;

                for(int i=0;i<d;i++)          
                {
                    //compute the dot product between error and component
                    double dotproduct = 0;
                                        
                    for(int n=0;n<N;n++)
                        dotproduct += errorvector[n]*aggregator.input[n*d + i];
                                        
                    //compute the target multiple of the error in the component direction
                    double upper = (N*lambda - dotproduct) / sqnorms[i];
                    double lower = (-N*lambda - dotproduct) / sqnorms[i];
                    
                    //find the parameter update
                    double update = 0;
                    bool setzero = false;
                    if(-parameter[i] < lower)
                        update = lower;
                    else if(-parameter[i] > upper)
                        update = upper;
                    else
                    {
                        update = -parameter[i];
                        setzero = true;
                    }
                    
                    double absupdate = update;
                    if(update < 0)
                        absupdate = -update;
					if (absupdate > maxupdate)
						maxupdate = absupdate;
                    
                    //perform the update
                    if(setzero)
                    {
                        parameter[i] = 0;
                        selection.erase(i);
                    }
                    else
                    {
                        parameter[i] += update;
                        selection[i] = true;
                    }
                                            
                    for(int n=0;n<N;n++)
                        errorvector[n] += aggregator.input[n*d + i]*update;
                                        
                    //check that the objective increases in the vicinity
                    if(safe)
                    {
                        double margin = parameter[i]*1e-5;
                        if(setzero)
                            margin = 1e-5;
                        double base = 0;
                        double delta = 0;
                        for(int n=0;n<N;n++)
                        {
                            base += errorvector[n]*errorvector[n];
                            double componentdelta = errorvector[n] + aggregator.input[n*d + i]*margin;
                            delta += componentdelta*componentdelta;
                        }
                        double deriv = (delta - base)/margin/2/N;
                        if(setzero)
							gstd::check(deriv < 1.01 * lambda && -deriv < 1.01 * lambda, "gradient check failed");
                        else if(parameter[i] > 0)
                            gstd::check( ( - 0.99 * lambda > deriv ) && - 1.01 * lambda < deriv, "gradient check failed" );
                        else
                            gstd::check( (   0.99 * lambda < deriv ) &&   1.01 * lambda > deriv, "gradient check failed" );                        
                    }
                } 
                if(doc)
                    if( ((iter + 1)*d*N) % 100000000 == 0 )
                    {
                        gstd::Printer::c(iter+1);
                        t.t();
                    }
				//check for termination
				if (maxupdate < ytol)
					break;
            }
                        
            if(cache)
            {
                cacheModel();  
                cacheSelectionModel();
            }
			if (doc)
				fitDocStd();

			//cleanup
			aggregator.inServer.setnull();
        }
        
        void Lasso::sweep()
        {           
			sweepStd();
        }
        
        //Base package
        void Lasso::setInputs()
        {
            //make choices
            int d = 20;
            int N = 10000; 
            double nonzerofraction = 0.5;
            double largefraction = 0.25;
            initialparameterscale = 0.1;
            lambda = 1;
            numiter = 1000;
            double nonzero = 2;
            double largenonzero = 4;
            double noisestd = 1;           
            //create a target model
            target.resize(d);
            targetSelection.clear();
            for(int i=0;i<d;i++)
            {
                double selector = gstd::Rand::d(3);
                if(selector+nonzerofraction > 1)
                {
                    targetSelection[i] = true;
                    if(selector+largefraction > 1)
                        target[i] = largenonzero;
                    else
                        target[i] = nonzero;
                } 
                else
                    target[i] = 0;
            }
                
            //generate data
            std::vector<double> input = std::vector<double>(d*N);
            for(int n=0;n<N;n++)
                for(int i=0;i<d;i++)
                    input[n*d + i] = gstd::Rand::d(3);
                        
            std::vector<double> output = std::vector<double>(N);
            for(int n=0;n<N;n++)
            {
                output[n] = gstd::stat::randnorm(4)*noisestd;
                for(int i=0;i<d;i++)
                    output[n] += input[n*d + i]*target[i];
            }      
            //set up the pipeline
            data::Dataset<DpRegression,data::DataHeader> *dset = new data::Dataset<DpRegression,data::DataHeader>();
            for(int n=0;n<N;n++)
            {
                DpRegression point;
                for(int i=0;i<d;i++)
                    point.input.push_back(input[n*d + i]);
                point.output = output[n];   
                dset->set(n, point);
            }
            client.set(dset, true);                   
            maxDataPoint = N;
            //set up the SweepSchedule
            LambdaSweepScheduleArith* schedule = new LambdaSweepScheduleArith;
            schedule->start = 8;
            schedule->increment = -0.5;
            schedule->numiter = 16;
            lambdaSweepSchedule.set(schedule, true);
        }
		bool Lasso::testInner(Lasso& obj)
		{
			obj.safe = true;
			obj.doc = true;
			try
			{
				obj.fit();
			}
			catch (std::exception e)
			{
				obj.reportFailure("Safety checks in 'fit' failed");
				return false;
			}
			int d = obj.parameter.size();
			bool success = true;
			for (int i = 0; i<d; i++)
			if (obj.targetSelection.count(i) == 1)
			{
				if (!gstd::Double::equals(obj.parameter[i], obj.target[i], obj.lambda, false))
					success = false;
			}
			else
			if (obj.selection.count(i) == 1)
				success = false;
			if (!success)
			{
				obj.reportFailure("'fit'");
				return false;
			}
			try
			{
				obj.sweep();
			}
			catch (std::exception e)
			{
				obj.reportFailure("Safety checks in 'sweep' failed");
				return false;
			}
			double nonzero = 2;
			double large = 4;
			int numnonzero = 0;
			int numlarge = 0;
			for (int i = 0; i<d; i++)
			{
				if (gstd::Double::equals(obj.target[i], nonzero))
					numnonzero++;
				else if (gstd::Double::equals(obj.target[i], large))
					numlarge++;
			}
			for (int i = 0; i<d; i++)
			{
				if (gstd::Double::equals(obj.target[i], nonzero) && (obj.ranks[i] < (double)numlarge || obj.ranks[i] >= (double)(numlarge + numnonzero)))
					success = false;
				else if (gstd::Double::equals(obj.target[i], large) && obj.ranks[i] >= (double)numlarge)
					success = false;
			}
			if (!success)
			{
				obj.reportFailure("'sweep'");
				return false;
			}
			return true;
		}
        bool Lasso::test()
        {     
            Lasso obj;
			obj.setInputs();
			return testInner(obj);
        }       
        gstd::TypeName Lasso::getTypeName()
        {
            return gstd::TypeNameGetter<Lasso>::get();
        }      
        std::string Lasso::toString()
        {
            std::stringstream res;
            res << "This is a ml::Lasso" <<std::endl;
            res << "initialparameterscale is: " << initialparameterscale << std::endl;
            res << "numiter is: " << numiter << std::endl;
            res << aggregator.toString();
            res << SelectionModelTyped<DpRegression,data::DataHeader>::toString();
            return res.str();
        }       
    }
}

