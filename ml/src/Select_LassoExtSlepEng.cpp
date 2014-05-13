#include "stdafx.h"

#include "LassoExtSlepEng.h"
#include "gstd/src/Linalg.h"
#include "gstd/src/MatlabMgr.h"

namespace msii810161816
{
	namespace ml
	{
		LassoExtSlepEng::LassoExtSlepEng() {}

		LassoExtSlepEng::~LassoExtSlepEng() {}

		const std::string LassoExtSlepEng::mid = "ml_LassoExtSlepEng_";

		void LassoExtSlepEng::fitInner(bool withSweep)
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
			mgr.exec("opts.fName = 'LeastR';");
			mgr.exec("opts.mFlag = 0;");
			mgr.exec("opts.lFlag = 0;");

			//set starting point
			mgr.exec("opts.init = 0;");

			mgr.exec(defaultCommand);

			postStd(withSweep);
		}

		void LassoExtSlepEng::fit()
		{
			fitInner(false);
		}

		void LassoExtSlepEng::sweep()
		{
			lambdaSweepSchedule.get(false)->reset();
			fitInner(true);
		}

		//Base Package
		gstd::TypeName LassoExtSlepEng::getTypeName()
		{
			return gstd::TypeNameGetter<LassoExtSlepEng>::get();
		}
		std::string LassoExtSlepEng::toString()
		{
			std::stringstream res;
			res << "This is a ml::LassoExtSlepEng" << std::endl;
			res << ml::Lasso::toString();
			return res.str();
		}
		void LassoExtSlepEng::setInputs()
		{
			Lasso::setInputs();
		}
		bool LassoExtSlepEng::test()
		{
			LassoExtSlepEng obj;
			obj.setInputs();
			if (!testInner(obj))
                            return false;
			LassoExtSlepEng obj2;
			Lasso las;
			las.setInputs();
			las.ytol = 1e-10;
			obj2.maxDataPoint = las.maxDataPoint;
			obj2.lambda = las.lambda;
			obj2.client.set(las.client.get(false), false);
			obj2.safe = true;
			obj2.ytol = 1e-10;
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
			if (!gstd::Linalg::mequals(las.parameter, obj2.parameter, 1e-5))
			{
				obj2.reportFailure("second fit()");
				return false;
			}			
			return true;
		}

	}
}