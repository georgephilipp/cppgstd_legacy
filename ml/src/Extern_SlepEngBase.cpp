#include "stdafx.h"

#include "SlepEngBase.h"
#include "gstd/src/Vector.h"
#include "gstd/src/MatlabMgr.h"
#include "gstd/src/Linalg.h"

namespace msii810161816
{
	namespace ml
	{
		SlepEngBase::SlepEngBase() 
		{
			if (!mgr.instIsOpen())
				mgr.instOpen();
		}

		const std::string SlepEngBase::defaultCommand =
			"Lambda = Lambda*size(X, 1);"//weird convention that leads to this, but ok ...
			""
			//run
			"B = zeros(size(X, 2), size(Y, 2), numel(Lambda));"
			"for i = 1:size(Y, 2)"
			"	B(:, i, : ) = reshape(pathSolutionLeast(X, Y(:, i), Lambda, opts), size(X, 2), 1, numel(Lambda));"
			"end\n"
			"B = reshape(B,size(B,1)*size(B,2),size(B,3))';";

		void SlepEngBase::prepStd(bool withSweep)
		{
			//set parms
			gstd::vector::empty(lambdas);
			if (withSweep)
			{
				while (1)
				{
					double nextLambda = lambdaSweepSchedule.get(false)->next();
					if (nextLambda < 0)
						break;
					lambdas.push_back(nextLambda);
				}
			}
			else
				lambdas.push_back(lambda);
			int numLambdas = lambdas.size();
			mgr.create(1, numLambdas, lambdas, "Lambda");

			//add paths
			int numObjRoots = sizeof(gstd::MatlabSession::objRoots) / sizeof(gstd::MatlabSession::objRoots[0]);
			for (int i = 0; i < numObjRoots; i++)
			{
				mgr.exec("addpath(genpath('" + gstd::MatlabSession::objRoots[i] + "SLEP'));");
			}

			//set options
			mgr.exec("opts = [];");
			mgr.create(1, 1, { ytol }, "tol");
			mgr.exec("opts.tol = tol;");

		}
		void SlepEngBase::postStd(bool withSweep)
		{
			int numLambdas = lambdas.size();

			//get output
			std::tuple<int, int, std::vector<double> > result = mgr.get("B");

			//check output
			gstd::check(std::get<0>(result) == numLambdas, "Slep produced output of wrong dimension (numLambdas).");
			gstd::check(std::get<1>(result) == parmDim, "Slep produced output of wrong dimension (d)");
			std::vector<double> parmRes = std::get<2>(result);

			//set values
			std::vector<double> scores(parmDim, -numLambdas);
			for (int l = 0; l<numLambdas; l++)
			{
				parameter = gstd::Linalg::submatrix(numLambdas, parmDim, parmRes, l, 0, 1, -1);
				lambda = lambdas[l];
				for (int i = 0; i<parmDim; i++)
				{
					if (parameter[i] != 0)
					{
						selection[i] = true;
						if (scores[i] == -numLambdas)
							scores[i] = -l;
					}
					else
						selection.erase(i);
				}
				if (cache)
				{
					cacheModel();
					cacheSelectionModel();
				}
			}
			if (withSweep)
				ranks = gstd::vector::rank(scores, false);

			//print results
			if (doc)
			{
				if (withSweep)
					sweepDocStd();
				else
					fitDocStd();
			}
		}

		//Base Package
		std::string SlepEngBase::toString()
		{
			std::stringstream res;
			res << "This is a ml::SlepEngBase" << std::endl;
			return res.str();
		}

	}
}