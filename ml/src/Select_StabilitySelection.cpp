/* 
 * File:   StabilitySelection.cpp
 * Author: gschoenh
 * 
 * Created on November 25, 2013, 7:14 PM
 */

#include "stdafx.h"

#include "StabilitySelection.h"
#include "gstd/src/MatlabMgr.h"

namespace msii810161816
{
	namespace ml
	{
		bool stabilitySelectionTestHelperFn(std::string mode)
		{
			//initialize
			StabilitySelection<DpRegression, data::DataHeader> obj;
			gstd::MatlabMgr mgr;
			if (mode == "Lasso")
				obj.setInputs();
			else if (mode == "LassoExtSlep" || mode == "LassoExtSlepEng")
			{
				obj.assignfraction = 0.01;
				obj.numiter = 100;
				SelectionModelTyped<DpRegression, data::DataHeader>* _submodel = 0;
				if (mode == "LassoExtSlep")
					_submodel = new LassoExtSlep;
				else
					_submodel = new LassoExtSlepEng;
				_submodel->setInputs();
				obj.target = _submodel->target;
				obj.targetSelection = _submodel->targetSelection;
				obj.submodel.set(_submodel, true);
				obj.expectedNumberFalseSelection = 100;
				if (mode == "LassoExtSlepEng" && !mgr.instIsOpen())
					mgr.instOpen();
			}
			else
				gstd::error("unknown mode");
			obj.safe = true;
			obj.doc = true;
			double nonzero = 2;
			double large = 4;
			int d = obj.target.size();
			bool success = true;
			int numnonzero = 0;
			int numlarge = 0;
			for (int i = 0; i < d; i++)
			{
				if (gstd::Double::equals(obj.target[i], nonzero))
					numnonzero++;
				else if (gstd::Double::equals(obj.target[i], large))
					numlarge++;
			}

			//fit
			try
			{
				obj.fit();
			}
			catch (std::exception e)
			{
				obj.reportFailure("Safety checks in 'fit' failed");
				return false;
			}
			for (int i = 0; i < d; i++)
			{
				if (gstd::Double::equals(obj.target[i], nonzero) && (obj.ranks[i] < numlarge || obj.ranks[i] >= numlarge + numnonzero))
					success = false;
				else if (gstd::Double::equals(obj.target[i], large) && obj.ranks[i] >= numlarge)
					success = false;
			}
			for (int i = 0; i < d; i++)
			if (obj.targetSelection.count(i) != obj.selection.count(i))
				success = false;
			obj.expectedNumberFalseSelection = -1;
			obj.totalToSelect = numlarge;
			obj.select();
			for (int i = 0; i < d; i++)
			if (obj.selection.count(i) == 1)
			{
				if (!gstd::Double::equals(obj.target[i], large))
					success = false;
			}
			else
			{
				if (gstd::Double::equals(obj.target[i], large))
					success = false;
			}
			if (!success)
			{
				obj.reportFailure("'fit'");
				return false;
			}

			//sweep
			obj.cache = true;
			obj.expectedNumberFalseSelection = -1;
			obj.selectionThreshold = 0.5;
			obj.totalToSelect = -1;
			obj.selectionCacheProfile.selection = true;
			obj.stabilityCacheProfile.selection = true;
			obj.stabilityCacheProfile.stability = true;
			try
			{
				obj.sweep();
			}
			catch (std::exception e)
			{
				obj.reportFailure("Safety checks in 'sweep' failed");
				return false;
			}
			if (obj.selectionCache.size() != 16 || obj.stabilityCache.size() != 16 || obj.stabilityCacheSchedule.size() != 16)
				success = false;
			for (int l = 0; l < 16; l++)
			if (!gstd::Double::equals(obj.selectionCache[l].lambda, 8 - l*0.5) || !gstd::Double::equals(obj.stabilityCache[l].lambda, 8 - l*0.5))
				success = false;
			std::vector<double> sched(0);
			for (int l = 0; l < 16; l++)
			{
				sched.push_back(8 - l*0.5);
				if (!gstd::Linalg::mequals(obj.stabilityCacheSchedule[l].lambdaSchedule, sched))
					success = false;
			}
			int largetestpoint = 10; //for lambda=3, I want all the large elements to be selected and I want the small ones to follow in rank, roughly
			std::vector<double> rs = gstd::vector::rank(obj.stabilityCache[largetestpoint].stability, false);
			for (int i = 0; i < d; i++)
			{
				if (gstd::Double::equals(obj.target[i], large))
				{
					if (obj.selectionCache[largetestpoint].selection.count(i) != 1)
						success = false;
				}
				else if (gstd::Double::equals(obj.target[i], nonzero))
				{
					if (rs[i] >= numlarge + numnonzero || rs[i] < numlarge)
					{
						gstd::Printer::c("jiojiojiojio");
						success = false;
					}

				}
				else
				{
					if (obj.selectionCache[largetestpoint].selection.count(i) == 1)
						success = false;
				}
			}
			rs = gstd::vector::rank(obj.stabilityCacheSchedule[largetestpoint].stability, false);
			for (int i = 0; i < d; i++)
			{
				if (gstd::Double::equals(obj.target[i], large))
				{
					if (obj.stabilityCacheSchedule[largetestpoint].selection.count(i) != 1)
						success = false;
				}
				else if (gstd::Double::equals(obj.target[i], nonzero))
				{
					if (rs[i] >= numlarge + numnonzero + 2 || rs[i] < numlarge - 2)
					{
						gstd::Printer::c("jiojiojiojio");
						success = false;
					}
				}
				else
				{
					if (obj.stabilityCacheSchedule[largetestpoint].selection.count(i) == 1)
						success = false;
				}
			}
			int smalltestpoint = 15; //for lambda=0.5, I want all large and small elements to be selected and the large elements to have stability 1
			int numFalse = 0;
			for (int i = 0; i < d; i++)
			{
				if (gstd::Double::equals(obj.target[i], large))
				{
					if (obj.selectionCache[smalltestpoint].selection.count(i) != 1 || !gstd::Double::equals(obj.stabilityCache[smalltestpoint].stability[i], 1, 1e-12))
						success = false;
				}
				else if (gstd::Double::equals(obj.target[i], nonzero))
				{
					if (obj.selectionCache[smalltestpoint].selection.count(i) != 1)
						success = false;
				}
				else
				{
					if (obj.selectionCache[smalltestpoint].selection.count(i) == 1)
						numFalse++;
				}
			}
			for (int i = 0; i < d; i++)
			{
				if (gstd::Double::equals(obj.target[i], large))
				{
					if (obj.stabilityCacheSchedule[smalltestpoint].selection.count(i) != 1 || !gstd::Double::equals(obj.stabilityCacheSchedule[smalltestpoint].stability[i], 1, 1e-12))
						success = false;
				}
				else if (gstd::Double::equals(obj.target[i], nonzero))
				{
					if (obj.stabilityCacheSchedule[smalltestpoint].selection.count(i) != 1)
						success = false;
				}
				else
				{
					if (obj.stabilityCacheSchedule[smalltestpoint].selection.count(i) == 1)
						numFalse++;
				}
			}
			if (numFalse > 0)
			{
				gstd::Printer::c("linglinglingoing");
				success = false;
			}

			//finally , I want to check that maxStability is larger or equal than stability but not always equal
			bool foundunequal = false;
			for (int l = 0; l < 16; l++)
			for (int i = 0; i < d; i++)
			{
				if (obj.stabilityCacheSchedule[l].stability[i] < obj.stabilityCache[l].stability[i])
					success = false;
				if (obj.stabilityCacheSchedule[l].stability[i] == obj.stabilityCache[l].stability[i])
					foundunequal = true;
			}
			if (!foundunequal)
				success = false;

			if (!success)
			{
				obj.reportFailure("'sweep'");
				return false;
			}

			return true;
		}
	}
}
