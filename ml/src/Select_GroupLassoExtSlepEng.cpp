#include "stdafx.h"

#include "GroupLassoExtSlepEng.h"
#include "gstd_matlab/src/MatlabMgr.h"
#include "gstd/src/Linalg.h"
#include "Data/src/Converters.h"
#include "gstd/src/Primitives.h"
#include "DataHeaders.h"
#include "LassoExtSlepEng.h"
#include "gstd/src/Rand.h"
#include "DpDoublesGenerators.h"
#include "RegressionGenerators.h"
#include "RegressionNormalizer.h"

namespace msii810161816
{
	namespace ml
	{
		GroupLassoExtSlepEng::GroupLassoExtSlepEng() {}

		GroupLassoExtSlepEng::~GroupLassoExtSlepEng() {}

		const std::string GroupLassoExtSlepEng::mid = "ml_GroupLassoExtSlepEng_";

		void GroupLassoExtSlepEng::fitInner(bool withSweep)
		{
			gstd::trial<ml::DpRegression> point = client.get(false)->get(0);
			gstd::check(point.success, "cannot find data points");
			parmDim = point.result.input.size();

			prepStd(withSweep);

			//set data
			std::vector<double> inMatrix;
			std::vector<double> outMatrix;
			for (int n = 0; n < maxDataPoint; n++)
			{
				gstd::trial<ml::DpRegression> point = client.get(false)->get(n);
				gstd::check(point.success, "cannot find data points");
				gstd::check((int)point.result.input.size() == parmDim, "point with incorrect data size");
				inMatrix.insert(inMatrix.end(), point.result.input.begin(), point.result.input.end());
				outMatrix.push_back(point.result.output);
			}
			mgr.create(maxDataPoint, parmDim, inMatrix, "X");
			mgr.create(maxDataPoint, 1, outMatrix, "Y");

			//set the function
			mgr.exec("opts.fName = 'glLeastR';");
			mgr.exec("opts.mFlag = 1;");
			mgr.exec("opts.lFlag = 1;");

			//set starting point
			mgr.exec("opts.init = 0;");

			//set groups
			InputGroupHeader header = client.get(false)->getDataHeader();
			std::vector<double> groups(1, 0);
			int numGroups = header.inputGroups.size();
			for (int i = 0; i < numGroups; i++)
				groups.push_back((double)header.inputGroups[i]);
			mgr.create(1, numGroups + 1, groups, "ind");
			mgr.exec("opts.ind = ind;");

			mgr.exec(defaultCommand);

			postStd(withSweep);
		}

		void GroupLassoExtSlepEng::fit()
		{
			fitInner(false);
		}

		void GroupLassoExtSlepEng::sweep()
		{
			lambdaSweepSchedule.get(false)->reset();
			fitInner(true);
		}

		//Base Package
		gstd::TypeName GroupLassoExtSlepEng::getTypeName()
		{
			return gstd::TypeNameGetter<GroupLassoExtSlepEng>::get();
		}
		std::string GroupLassoExtSlepEng::toString()
		{
			std::stringstream res;
			res << "This is a ml::GroupLassoExtSlepEng" << std::endl;
			res << SelectionModelTyped<DpRegression, InputGroupHeader>::toString();
			return res.str();
		}
		void GroupLassoExtSlepEng::setInputs()
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
				lasgen->targetModel[i*numGroups + 1] = target[i*numGroups + 1];
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
		bool GroupLassoExtSlepEng::test()
		{
			GroupLassoExtSlepEng obj;
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
					if (!gstd::Double::equals(obj.ranks[j*groupSize + i], lastrank))
						success = false;
				}
			}
			if (!success)
			{
				obj.reportFailure("'sweep'");
				return false;
			}
			//now, we want to replicate lasso with trivial groups
			LassoExtSlepEng les;
			les.setInputs();
			les.doc = true;
			les.safe = true;
			les.ytol = 1e-12;
			try
			{
				les.fit();
			}
			catch (std::exception e)
			{
				obj.reportFailure("safety checks on LassoExtSlep::fit()");
				return false;
			}
			GroupLassoExtSlepEng obj2;
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
			obj2.ytol = 1e-12;
			try
			{
				obj2.fit();
			}
			catch (std::exception e)
			{
				obj2.reportFailure("safety checks on second fit()");
				return false;
			}
			if (!gstd::Linalg::mequals(les.parameter, obj2.parameter, 1e-5))
			{
				obj2.reportFailure("second fit()");
				return false;
			}
			return true;
		}

	}
}