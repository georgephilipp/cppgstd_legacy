#include "stdafx.h"

#include "GroupLassoExtSlep.h"
#include "LassoExtSlep.h"
#include "gstd/src/Rand.h"
#include "DpDoublesGenerators.h"
#include "RegressionGenerators.h"
#include "RegressionNormalizer.h"
#include "gstd/src/Primitives.h"
#include "Data/src/Converters.h"

namespace msii810161816
{
	namespace ml
	{
		GroupLassoExtSlep::GroupLassoExtSlep() {}
                
		GroupLassoExtSlep::~GroupLassoExtSlep() {}
                
		void GroupLassoExtSlep::fitInner(bool withSweep)
		{
                    if(mgr.command == SlepBase::defaultCommandBase)
                        mgr.command = mgr.command + "\"procname='"+procname+"';GroupLasso;\"";
                    
			prepStd(withSweep);

			//set the data
			gstd::trial<ml::DpRegression> point = client.get(false)->get(0);
			gstd::check(point.success, "cannot find data points");
			int dI = point.result.input.size();
			int dO = 1;
			//int dO = point.result.output.size();
			parmDim = dI*dO;
			for (int n = 0; n<maxDataPoint; n++)
			{
				point = client.get(false)->get(n);
				gstd::check(point.success, "cannot find data points");
				gstd::check((int)point.result.input.size() == dI, "point with incorrect data size");
				//gstd::check((int)point.result.output.size() == dO, "point with incorrect data size");
				mgr.inDataFiles[0].push_back(point.result.input);
				mgr.inDataFiles[1].push_back({ point.result.output });
				//mgr.inDataFiles[1].push_back(point.result.output);
			}

			//set the groups
			mgr.inHeaderFiles[0].push_back(gstd::Printer::vp(client.get(false)->getDataHeader().inputGroups, ','));

			mgr.run();

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

		void GroupLassoExtSlep::fit()
		{
			fitInner(false);
		}

		void GroupLassoExtSlep::sweep()
		{
			lambdaSweepSchedule.get(false)->reset();
			fitInner(true);
		}

		//Base Package
		gstd::TypeName GroupLassoExtSlep::getTypeName()
		{
			return gstd::TypeNameGetter<GroupLassoExtSlep>::get();
		}
		std::string GroupLassoExtSlep::toString()
		{
			std::stringstream res;
			res << "This is a ml::GroupLassoExtSlep" << std::endl;
			res << mgr.toString();
			res << SelectionModelTyped<DpRegression, InputGroupHeader>::toString();
			return res.str();
		}
		void GroupLassoExtSlep::setInputs()
		{
			//make choices
			int groupSize = 4;
			int numGroups = 5; //matches GaussianGenerator::defaultCovarianceMatrix()
			maxDataPoint = 10000;
			lambda = 0.005;
			double noiseStd = 1;
			//create a target model (as in GaussianGenerator::defaultCovarianceMatrix()); note that this targetModel isn't REALLY the target model because of later normalization
			int d = groupSize * numGroups;
			target.resize(d);
			targetSelection.clear();
			for (int i = 0; i < groupSize; i++)
			{
				target[groupSize + i] = 20 * gstd::Rand::d(3);
				target[2 * groupSize + i] = gstd::Rand::d(3);
				targetSelection[groupSize + i] = true;
				targetSelection[2 * groupSize + i] = true;
			}			
			//generate the design
			GaussianGenerator<InputGroupHeader>* gen = new GaussianGenerator<InputGroupHeader>;
			gen->setInputsBlockwise(4, 5, 1, 0.5, 0);
			InputGroupHeader header;
			for (int i = 1; i <= numGroups; i++)
				header.inputGroups.push_back(i*groupSize);
			gen->setDataHeader(header);
	        //generate the output
			LinearLassoGenerator<InputGroupHeader>* lasgen = new LinearLassoGenerator<InputGroupHeader>;
			for (int i = 0; i < groupSize; i++)
			{
				lasgen->targetModel[i*numGroups] = target[i*numGroups];
				lasgen->targetModel[i*numGroups+1] = target[i*numGroups+1];
			}
			lasgen->noiseStd = noiseStd;
			lasgen->inServer.set(gen, true);
			lasgen->storedata = true;
		    //normalize
			RegressionNormalizer<InputGroupHeader>* norm = new RegressionNormalizer<InputGroupHeader>;
			norm->normalizeMean = true;
			norm->normalizeStd = true;
			norm->inServer.set(lasgen, true);
			norm->initMeansStds(0, maxDataPoint);
			//set up Group Lasso
			client.set(norm, true);
			//set up the SweepSchedule
			LambdaSweepScheduleGeo* schedule = new LambdaSweepScheduleGeo;
			schedule->start = 1;
			schedule->factor = 0.27;
			schedule->numiter = 5;
			lambdaSweepSchedule.set(schedule, true);			
		}
		bool GroupLassoExtSlep::test()
		{
			GroupLassoExtSlep obj;
			obj.setInputs();
			int groupSize = 4;
			int numGroups = 5;
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
			for (int i = 0; i < d; i++)
			{
				if (obj.targetSelection.count(i) == 1)
				{
					if (obj.selection.count(i) == 0)
						success = false;
				}
				else
				{
					if (obj.selection.count(i) == 1)
						success = false;
				}
			}
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
			//ranks must be groupwise (stronger group first)
			double firstrank = ((double)(groupSize - 1))*0.5;
			double secondrank = (double)((3 * groupSize - 1))*0.5;
			double lastrank = (double)(((numGroups + 2) * groupSize - 1))*0.5;
			for (int i = 0; i < groupSize; i++)
			{
				if (!gstd::Double::equals(obj.ranks[groupSize + i], firstrank))
					success = false;
				if (!gstd::Double::equals(obj.ranks[2 * groupSize + i], secondrank))
					success = false;
				if (!gstd::Double::equals(obj.ranks[i], lastrank))
					success = false;
				for (int j = 3; j < numGroups; j++)
				{
					if (!gstd::Double::equals(obj.ranks[j*groupSize+i], lastrank))
						success = false;
				}
			}
			if (!success)
			{
				obj.reportFailure("'sweep'");
				return false;
			}
            //now, we want to replicate lasso with trivial groups
            LassoExtSlep les;
            les.setInputs();
			les.doc = true;
			les.safe = true;
			try
			{
				les.fit();
			}
			catch (std::exception e)
			{
			    obj.reportFailure("safety checks on LassoExtSlep::fit()");
			    return false;
			}
			GroupLassoExtSlep obj2;
			obj2.maxDataPoint = les.maxDataPoint;
			obj2.lambda = les.lambda;
			gstd::trial<DpRegression> point = les.client.get(false)->get(0);
			gstd::check(point.success, "LassoExtSlep default inputs cannot find data");
			int dim = point.result.input.size();
			InputGroupHeader header;
			for (int i = 1; i <= dim; i++)
				header.inputGroups.push_back(i);
			data::Converter<DpRegression, DpRegression, data::DataHeader, InputGroupHeader> conv;
			conv.setDataHeader(header);
			conv.inServer.set(les.client.get(false), false);
			obj2.client.set(&conv, false);
			obj2.safe = true;
			obj2.doc = true;
			try
			{
				obj2.fit();
			}
			catch (std::exception e)
			{
				obj2.reportFailure("safety checks on second fit()");
				return false;
			}
			if (!gstd::Linalg::mequals(les.parameter, obj2.parameter, 1e-2))
			{
				obj2.reportFailure("second fit()");
				return false;
			}
            return true;
		}

	}
}