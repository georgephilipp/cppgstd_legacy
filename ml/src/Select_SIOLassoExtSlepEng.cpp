#include "stdafx.h"



#include "SIOLassoExtSlepEng.h"
#include "gstd/src/MatlabMgr.h"
#include "gstd/src/Linalg.h"
#include "Data/src/Converters.h"
#include "gstd/src/Primitives.h"
#include "DataHeaders.h"
#include "GroupLassoExtSlepEng.h"
#include "gstd/src/Rand.h"
#include "DpDoublesGenerators.h"
#include "RegressionGenerators.h"
#include "RegressionNormalizer.h"
#include "gstd/src/Dependencies.h"

namespace msii810161816
{
	namespace ml
	{
		SIOLassoExtSlepEng::SIOLassoExtSlepEng() {}

		SIOLassoExtSlepEng::~SIOLassoExtSlepEng() {}

		const std::string SIOLassoExtSlepEng::mid = "ml_SIOLassoExtSlepEng_";

		std::tuple<int, int, std::vector<double> > SIOLassoExtSlepEng::makeInGroupMatrix(IOGroupHeader* header)
		{
			int numInGroups = header->inputGroups.size();
			int maxInGroup = 0;
			for (int i = 0; i < numInGroups; i++)
			{
				int thisGroupSize = (int)header->inputGroups[i].size();
				if (thisGroupSize > maxInGroup)
					maxInGroup = thisGroupSize;
			}
			std::vector<double> inGroupMatrix(maxInGroup*numInGroups, -1.0);
			for (int i = 0; i < numInGroups; i++)
			{
				int thisGroupSize = (int)header->inputGroups[i].size();
				for (int j = 0; j < thisGroupSize; j++)
					inGroupMatrix[i*maxInGroup + j] = header->inputGroups[i][j];
			}
			return std::tuple<int, int, std::vector<double> >(numInGroups, maxInGroup, inGroupMatrix);
		}

		void SIOLassoExtSlepEng::fitInner(bool withSweep)
		{
			gstd::trial<ml::DpMTRegression> point = client.get(false)->get(0);
			gstd::check(point.success, "cannot find data points");
			int d = point.result.input.size();
			int t = point.result.output.size();
			parmDim = d*t;

			prepStd(withSweep);

            //add additional paths
			mgr.exec("addpath(genpath('" + gstd::dependencies::matlab::getObjRoot() + "SIOL'));");
                        
			//set data
			std::vector<double> inMatrix;
			std::vector<double> outMatrix;
			for (int n = 0; n < maxDataPoint; n++)
			{
				gstd::trial<ml::DpMTRegression> point = client.get(false)->get(n);
				gstd::check(point.success, "cannot find data points");
				gstd::check((int)point.result.input.size()*(int)point.result.output.size() == parmDim, "point with incorrect data size");
				inMatrix.insert(inMatrix.end(), point.result.input.begin(), point.result.input.end());
				outMatrix.insert(outMatrix.end(), point.result.output.begin(), point.result.output.end());
			}
			mgr.create(maxDataPoint, d, inMatrix, "X");
			mgr.create(maxDataPoint, t, outMatrix, "Y");

			//set lambda ratio
			mgr.create(1, 1, std::vector<double>({ lambdaRatio }), "lambdaRatio");

			//set the function
			//mgr.exec("opts.fName = 'glLeastR';"); //not needed
			mgr.exec("opts.mFlag = 0;");
			mgr.exec("opts.lFlag = 0;");
			mgr.exec("opts.init = 0;");

			//set groups
			IOGroupHeader header = client.get(false)->getDataHeader();
			//output
			//supported 1
			std::vector<std::vector<int> > supportedGrouping1(1);
			for (int i = 0; i < t; i++)
				supportedGrouping1[0].push_back(i);
			//supported 2
			std::vector<std::vector<int> > supportedGrouping2;
			if (header.outputGroups == supportedGrouping1)
				mgr.exec("UseOutputGroups = 1;");
			else if (header.outputGroups == supportedGrouping2)
				mgr.exec("UseOutputGroups = 0;");
			else
				gstd::error("Output groups specified are of unsupported format");
			//intput
			int numInGroups, maxInGroup;
			std::vector<double> inGroupMatrix;
			std::tie(numInGroups, maxInGroup, inGroupMatrix) = makeInGroupMatrix(&header);
			mgr.create(numInGroups, maxInGroup, inGroupMatrix, "Groups");
			mgr.exec("Groups = Groups + 1;");//0-based to 1-based

			mgr.exec("mySIOL;");

			postStd(withSweep);
		}

		void SIOLassoExtSlepEng::fit()
		{
			fitInner(false);
		}

		void SIOLassoExtSlepEng::sweep()
		{
			lambdaSweepSchedule.get(false)->reset();
			fitInner(true);
		}

		//Base Package
		gstd::TypeName SIOLassoExtSlepEng::getTypeName()
		{
			return gstd::TypeNameGetter<SIOLassoExtSlepEng>::get();
		}
		std::string SIOLassoExtSlepEng::toString()
		{
			std::stringstream res;
			res << "This is a ml::SIOLassoExtSlepEng" << std::endl;
			res << SelectionModelTyped<DpMTRegression, IOGroupHeader>::toString();
			return res.str();
		}
		void SIOLassoExtSlepEng::setInputs()
		{
			lambda = 0.05;
			lambdaRatio = 1;
			maxDataPoint = 1000;
			//generate the design
			GaussianGenerator<ml::IOGroupHeader>* gen = new GaussianGenerator<ml::IOGroupHeader>;
			gen->setInputsBlockwise(1, 7, 1, 0, 0);
			gen->storedata = true;
			//generate the output
			LinearMTLassoGenerator<ml::IOGroupHeader>* las = new LinearMTLassoGenerator<ml::IOGroupHeader>;
			las->noiseStd = 1;
			las->inServer.set(gen, true);
			las->targetModel.resize(3);
			las->targetModel[0][3] = 1;
			las->targetModel[2][3] = 0.5;
			las->targetModel[0][4] = 0.5;
			targetSelection = gstd::map::create(std::vector<int>({ 3, 4, 17 }));
			target = std::vector<double>(21, 0);
			target[3] = 1;
			target[4] = 0.5;
			target[17] = 0.5;
			las->storedata = true;
			//normalize
			MTRegressionNormalizer<ml::IOGroupHeader>* norm = new MTRegressionNormalizer<ml::IOGroupHeader>;
			norm->inServer.set(las, true);
			norm->normalizeMean = true;
			norm->normalizeStd = true;
			norm->initMeansStds(0, maxDataPoint);
			norm->storedata = true;
			//set client
			client.set(norm, true);
			//sweep schedule
			LambdaSweepScheduleGeo* sched = new LambdaSweepScheduleGeo;
			sched->start = 1;
			sched->factor = 0.98;
			sched->numiter = 300;
			lambdaSweepSchedule.set(sched, true);
			//groups
			IOGroupHeader head;
			head.inputGroups.push_back({ 0, 1, 2 });
			head.inputGroups.push_back({ 2, 3, 4 });
			head.inputGroups.push_back({ 4, 5, 6 });
			head.outputGroups.push_back({ 0, 1, 2 });
			norm->setDataHeader(head);
		}
		bool SIOLassoExtSlepEng::test()
		{
			SIOLassoExtSlepEng obj;
			obj.setInputs();
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
			if (gstd::map::keys(obj.selection) != gstd::map::keys(obj.targetSelection))
			{
				obj.reportFailure("'fit'");
				return false;
			}
			int numsucc = 0;
			for (int i = 0; i < 10; i++)
			{
				SIOLassoExtSlepEng obj3;
				obj3.setInputs();
				obj3.safe = true;
				try
				{
				    obj3.sweep();
				}
				catch (std::exception e)
				{
					obj.reportFailure("Safety checks in 'sweep' failed");
					return false;
				}
				if (!gstd::Double::equals(obj3.ranks[3],0.5)
					|| !gstd::Double::equals(obj3.ranks[17], 0.5)
					|| !gstd::Double::equals(obj3.ranks[4], 2)
					)
				{
					continue;
				}
				numsucc++;
			}
			if (numsucc <= 8)
			{
				obj.reportFailure("'sweep'");
				return false;
			}

			
			
			SIOLassoExtSlepEng obj2;

			//test the inGroupMatric function
			IOGroupHeader h;
			h.inputGroups = { { 1, 2, 4 }, { 501, 502 }, { 5, 6, 7, 8 } };
			std::vector<double> target = { 1, 2, 4, -1, 501, 502, -1, -1, 5, 6, 7, 8 };
			int targetrow = 3;
			int targetcol = 4;
			int outrow, outcol;
			std::vector<double> outMat;
			std::tie(outrow, outcol, outMat) = makeInGroupMatrix(&h);
			if (!gstd::Linalg::mequals(outMat, target) || targetrow != outrow || targetcol != outcol)
			{
				obj2.reportFailure("inGroupMatrix");
				return false;
			}

			//now, we want to replicate group lasso with trivial groups
			GroupLassoExtSlepEng gles;
			gles.setInputs();
			gles.doc = true;
			gles.safe = true;
			gles.ytol = 1e-12;
			obj2.maxDataPoint = gles.maxDataPoint;
			obj2.lambda = gles.lambda / 2.0; //the 2.0 is the sqrt of the group size
			data::Converter<DpRegression, DpRegression, InputGroupHeader, IOGroupHeader> conv;
			conv.inServer.set(gles.client.get(false), false);
			DpRegressionTaskDuplicator<IOGroupHeader> dupl;
			dupl.numTask = 4;
			dupl.inServer.set(&conv, false);
			obj2.client.set(&dupl, false);
			obj2.safe = true;
			obj2.doc = true;
			obj2.ytol = 1e-12;
			obj2.lambdaRatio = 0;
			try
			{
				gles.fit();
			}
			catch (std::exception e)
			{
				obj2.reportFailure("safety checks on LassoExtSlep::fit()");
				return false;
			}
			try
			{
				obj2.fit();
			}
			catch (std::exception e)
			{
				obj2.reportFailure("safety checks on second fit()");
				return false;
			}
			for (int i = 0; i < dupl.numTask; i++)
			{
				if (!gstd::Linalg::mequals(gles.parameter, gstd::vector::sub(obj2.parameter,i*gles.parameter.size(), gles.parameter.size()), 1e-4))
				{
					obj2.reportFailure("second fit()");
					return false;
				}
			}

			//now, the same with sweep
			LambdaSweepScheduleGeo sched;
			sched.start = 1;
			sched.numiter = 3;
			sched.factor = 0.1;
			LambdaSweepScheduleGeo sched2;
			sched2.start = 0.5;
			sched2.numiter = 3;
			sched2.factor = 0.1;
			gstd::check(obj2.lambdaSweepSchedule.isNull(), "this should be null");
			obj2.lambdaSweepSchedule.set(&sched2, false);
			gstd::Pointer<LambdaSweepSchedule> schedpoint2;
			bool glesOwns = gles.lambdaSweepSchedule.pointerIsOwned();
			schedpoint2.set(gles.lambdaSweepSchedule.get(glesOwns), glesOwns);
			gles.lambdaSweepSchedule.set(&sched, false);
			obj2.cache = true;
			obj2.modelCacheProfile.parameter = true;
			obj2.selectionCacheProfile.selection = true;
			gles.cache = true;
			gles.modelCacheProfile.parameter = true;
			gles.selectionCacheProfile.selection = true;
			try
			{
				obj2.sweep();
				gles.sweep();
			}
			catch (std::exception e)
			{
				obj2.reportFailure("safety checks during sweep");
				return false;
			}
			if ((int)obj2.modelCache.size() != sched.numiter || (int)obj2.selectionCache.size() != sched.numiter || (int)gles.modelCache.size() != sched.numiter || (int)gles.selectionCache.size() != sched.numiter)
			{
				obj2.reportFailure("cache not set correctly");
				return false;
			}
			int inDim = (int)gles.parameter.size();
			for (int l = 0; l < sched.numiter; l++)
			{
				bool success = true;
				if (!gstd::Double::equals(gles.selectionCache[l].lambda, gles.selectionCache[l].lambda))
					success = false;
				std::vector<double> targetParm;
				for (int t = 0; t < dupl.numTask; t++)
					targetParm.insert(targetParm.end(), gles.modelCache[l].parameter.begin(), gles.modelCache[l].parameter.end());
				if (!gstd::Linalg::mequals(obj2.modelCache[l].parameter, targetParm, 1e-3))
					success = false;
				if ((int)obj2.selectionCache[l].selection.size() != dupl.numTask*(int)gles.selectionCache[l].selection.size())
					success = false;
				std::vector<int> glesSelected = gstd::map::keys(gles.selectionCache[l].selection);
				for (int t = 0; t < dupl.numTask; t++)
				{
					for (int i = 0; i < (int)glesSelected.size(); i++)
					{
						if (obj2.selectionCache[l].selection.count(glesSelected[i] + t*inDim) != 1)
							success = false;
					}
				}
				if (!success)
				{
					obj2.reportFailure("sweep");
					return false;
				}
			}

			return true;
		}

	}
}