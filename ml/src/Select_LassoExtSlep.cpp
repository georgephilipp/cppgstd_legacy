/* 
 * File:   LassoExtSlep.cpp
 * Author: heathermceveety
 * 
 * Created on January 25, 2014, 10:58 PM
 */

#include "stdafx.h"

#include "LassoExtSlep.h"
#include "Datapoints.h"
#include "gstd/src/Map.h"
#include "gstd/src/Linalg.h"

namespace msii810161816
{   
    namespace ml
    {
        LassoExtSlep::LassoExtSlep() {}
			
        LassoExtSlep::~LassoExtSlep() {}
        
        void LassoExtSlep::fitInner(bool withSweep)
        {
            if(mgr.command == SlepBase::defaultCommandBase)
                 mgr.command = mgr.command + "\"procname='"+procname+"';Lasso;\"";
            
			prepStd(withSweep);

            //set the data
			gstd::trial<ml::DpRegression> point = client.get(false)->get(0);
			gstd::check(point.success, "cannot find data points");
			parmDim = point.result.input.size();
            for(int n=0;n<maxDataPoint;n++)
            {
                point = client.get(false)->get(n);
				gstd::check(point.success, "cannot find data points");
				gstd::check((int)point.result.input.size() == parmDim, "point with incorrect data size");
				mgr.inDataFiles[0].push_back(point.result.input);
				mgr.inDataFiles[1].push_back({ point.result.output });
            }
            
            std::vector<std::string> cmdres = mgr.run().result;           
                        
            if(doc) 
            {
                gstd::Printer::vc(cmdres, '\n');
            }
            
			postStd(withSweep);

            //print results
			if (doc)
			{
				if (withSweep)
					sweepDocStd();
				else
					fitDocStd();
			}
        }
        
        void LassoExtSlep::fit()
        {
            fitInner(false);
        }
        
        void LassoExtSlep::sweep()
        {
			lambdaSweepSchedule.get(false)->reset();
            fitInner(true);
        }

		//Base Package
		gstd::TypeName LassoExtSlep::getTypeName()
		{
			return gstd::TypeNameGetter<LassoExtSlep>::get();
		}
		std::string LassoExtSlep::toString()
		{
			std::stringstream res;
			res << "This is a ml::LassoExtSlep" << std::endl;
			res << mgr.toString();
			res << ml::Lasso::toString();
			return res.str();
		}
		void LassoExtSlep::setInputs()
		{
			Lasso::setInputs();
		}
		bool LassoExtSlep::test()
		{
			LassoExtSlep obj;
			obj.setInputs();
			if (!testInner(obj))
				return false;
			LassoExtSlep obj2;
			Lasso las;
			las.setInputs();
			obj2.maxDataPoint = las.maxDataPoint;
			obj2.lambda = las.lambda;
			obj2.client.set(las.client.get(false), false);
			obj2.safe = true;
			try
			{
				las.fit();
				obj2.fit();
			}
			catch (std::exception e)
			{
				obj2.reportFailure("safety checks on second fit()");
				return false;
			}
			if (!gstd::Linalg::mequals(las.parameter, obj2.parameter, 1e-1))
			{
				obj2.reportFailure("second fit()");
				return false;
			}
			return true;
		}
        
    }
}