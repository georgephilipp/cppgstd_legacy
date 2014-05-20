
#include "stdafx.h"

#include "ScreenAndClean.h"
#include "GroupLassoExtSlepEng.h"

namespace msii810161816
{
	namespace ml
	{
		bool screenAndCleanDpRegressionTestHelperFn()
		{
                    int numSucc = 0;
                    int numIter = 3;
                    int needed = 2;
                    for(int iter=0;iter<numIter;iter++)
                    {
                        //initialize
			ScreenAndClean<DpRegression, data::DataHeader> obj;
			obj.setInputs();
			obj.safe = true;
			obj.doc = true;
			try
			{
				obj.fit();
			}
			catch (std::exception e)
			{
				obj.reportFailure("safety checks of 'fit'");
				return false;
			}
			bool success = true;
			int P = obj.target.size();
			double nonzero = 2;
			double large = 4;
			int numnonzero = 0;
			int numlarge = 0;
			for (int i = 0; i < P; i++)
			{
				if (obj.target[i] == nonzero)
					numnonzero++;
				else if (obj.target[i] == large)
					numlarge++;
				else if (obj.target[i] != 0)
					gstd::error("cannot interpret model");
			}
			for (int i = 0; i < P; i++)
			{
				if (obj.target[i] == 0)
				{
					if (obj.ranks[i] < numnonzero + numlarge)
						success = false;
				}
				else if (obj.target[i] == nonzero)
				{
					if (obj.ranks[i] < numlarge || obj.ranks[i] >= numlarge + numnonzero)
						success = false;
				}
				else if (obj.target[i] == large)
				{
					if (obj.ranks[i] >= numlarge)
						success = false;
				}
				else
					gstd::error("cannot interpret model");
			}				
			if (obj.selection != obj.targetSelection)
			{
                            success = false;
			}
                        if(success)
                            numSucc++;
                        gstd::Printer::c("after " + gstd::Printer::p(iter+1) + " iterations, numSucc is " + gstd::Printer::p(numSucc));
                        if(iter + 1 == numIter)
                        {
                            gstd::Printer::c("final numSucc is " + gstd::Printer::p(numSucc));
                            if(numSucc < needed)
                            {                        
                                obj.reportFailure("'fit'");
                                return false;
                            }   
                        }
                    }
                                             
		    return true;
		}

		bool screenAndCleanDpMTRegressionTestHelperFn()
		{
			//res test
			int numSucc = 0;
			int numIter = 3;
			for (int iter = 0; iter < numIter; iter++)
			{
				gstd::Printer::c("Beginning test iteration " + gstd::Printer::p(iter));
				ScreenAndClean<DpMTRegression, IOGroupHeader> obj;
				SIOLassoExtSlepEng sub;
				obj.numiter = 10;
				obj.pvalueThreshold = 1;
				sub.setInputs();
				sub.numFolds = 10;
				obj.targetSelection = sub.targetSelection;
				obj.target = sub.target;
				obj.submodel.set(&sub, false);
				obj.safe = true;
				obj.doc = true;
				try
				{
					obj.fit();
				}
				catch (std::exception e)
				{
					obj.reportFailure("safety checks of 'fit'");
					return false;
				}
				bool success = true;
				int P = obj.target.size();
				double nonzero = 0.5;
				double large = 1;
				int numnonzero = 0;
				int numlarge = 0;
				for (int i = 0; i < P; i++)
				{
					if (obj.target[i] == nonzero)
						numnonzero++;
					else if (obj.target[i] == large)
						numlarge++;
					else if (obj.target[i] != 0)
						gstd::error("cannot interpret model");
				}
				for (int i = 0; i < P; i++)
				{
					if (obj.target[i] == 0)
					{
						if (obj.ranks[i] < numnonzero + numlarge)
							success = false;
					}
					else if (obj.target[i] == nonzero)
					{
						if (obj.ranks[i] < numlarge || obj.ranks[i] >= numlarge + numnonzero)
							success = false;
					}
					else if (obj.target[i] == large)
					{
						if (obj.ranks[i] >= numlarge)
							success = false;
					}
					else
						gstd::error("cannot interpret model");
				}
				if (success && obj.selection == obj.targetSelection)
					numSucc++;
				if (iter + 1 == numIter)
					gstd::Printer::c("final numSucc is " + gstd::Printer::p(numSucc));
				if (iter + 1 == numIter && numSucc + 1 < numIter)
				{
					obj.reportFailure("'fit'");
					return false;
				}
			}

			//reg test
			int numDupl = 4;
			ScreenAndClean<DpRegression, InputGroupHeader> Target;
			GroupLassoExtSlepEng subTarget;
			subTarget.setInputs();
			subTarget.numFolds = 3;
			LambdaSweepScheduleGeo lss;
			lss.start = 1;
			lss.factor = 0.5;
			lss.numiter = 20;
			subTarget.maxDataPoint = 300;
			subTarget.ytol = 1e-12;
			LambdaSweepSchedule* old = subTarget.lambdaSweepSchedule.get(true);
			delete old;
			subTarget.lambdaSweepSchedule.set(&lss, false);
			Target.submodel.set(&subTarget, false);
			Target.numiter = 3;
			Target.pvalueThreshold = 1;
			ScreenAndClean<DpMTRegression, IOGroupHeader> Res;
			SIOLassoExtSlepEng subRes;
			data::Converter<DpRegression, DpRegression, InputGroupHeader, IOGroupHeader> conv;
			conv.inServer.set(subTarget.client.get(false), false);
			DpRegressionTaskDuplicator<IOGroupHeader> dupl;
			dupl.numTask = numDupl;
			dupl.inServer.set(&conv, false);
			subRes.client.set(&dupl, false);
			LambdaSweepScheduleGeo lss2;
			lss2.start = lss.start / 2; //=sqrt(4), this is group size
			lss2.factor = lss.factor;
			lss2.numiter = lss.numiter;
			subRes.lambdaSweepSchedule.set(&lss2, false);
			subRes.numFolds = subTarget.numFolds;
			subRes.maxDataPoint = subTarget.maxDataPoint;
			subRes.ytol = subTarget.ytol;
			subRes.lambdaRatio = 0;
			Res.submodel.set(&subRes, false);
			Res.numiter = Target.numiter;
			Res.pvalueThreshold = numDupl;
		    
			subTarget.client.get(false)->storedata = true;
			for (int i = 0; i < subTarget.maxDataPoint; i++)
				subTarget.client.get(false)->get(i);
			int seed = gstd::Rand::i(10000);
			
			gstd::Rand::seed(seed);
			Target.fit();
			gstd::Rand::seed(seed);
			Res.fit();

			std::vector<double> multiplied = gstd::Linalg::mscale(Target.pvalues, numDupl);
			std::vector<double> targetps;
			for (int i = 0; i < numDupl;i++)
				targetps.insert(targetps.end(), multiplied.begin(), multiplied.end());
			std::map<int, bool> targetsel;
			std::vector<int> targ = gstd::map::keys(Target.selection);
			for (int i = 0; i < numDupl; i++)
			{
				for (int j = 0; j < (int)targ.size(); j++)
				{
					targetsel[targ[j] + i*multiplied.size()] = true;
				}
			}
			if (!gstd::Linalg::mequals(Res.pvalues, targetps, 1e-3) || gstd::map::keys(Res.selection) != gstd::map::keys(targetsel))
			{
				Res.reportFailure("reg test");
				return false;
			}
			
			return true;
		}
	}
}
