#include "stdafx.h"

#include "SlepBase.h"
#include "gstd/src/Vector.h"

namespace msii810161816
{
	namespace ml
	{      
#ifdef _WIN32
			const std::string SlepBase::defaultCommandBase = "matlab -nojvm -nodisplay -nosplash -r ";
#else
			const std::string SlepBase::defaultCommandBase = "/opt/matlab/8.1/bin/matlab -nojvm -nodisplay -nosplash -r ";
#endif             
            
		SlepBase::SlepBase()
		{
			mgr.externalPath = "../../../../Code_obj/Libraries/Matlab/SLEP_wrappers/";
			mgr.internalPath = "../../../../Code_obj/Libraries/Matlab/SLEP_wrappers/";
#ifdef _WIN32
			mgr.externalPath = "../" + mgr.externalPath;
			mgr.internalPath = "../" + mgr.internalPath;
#endif
#ifdef _WIN32
			mgr.command = SlepBase::defaultCommandBase;
#else
			mgr.command = SlepBase::defaultCommandBase;
#endif
			mgr.delimiter = ',';
                        
                        procname = "";
		}
		void SlepBase::prepStd(bool withSweep)
		{
			//initialize
			gstd::vector::empty(mgr.inHeaderFileNames);
			gstd::vector::empty(mgr.inDataFileNames);
			gstd::vector::empty(mgr.inHeaderFiles);
			gstd::vector::empty(mgr.inDataFiles);
			gstd::vector::empty(mgr.outHeaderFileNames);
			gstd::vector::empty(mgr.outDataFileNames);
                        mgr.outFileNameInternal = procname + gstd::TerminalMgr::defaultOutFileName;
			mgr.outFileNameExternal = procname + gstd::TerminalMgr::defaultOutFileName;
                        mgr.completeFileNameInternal = procname + gstd::TerminalMgr::defaultCompleteFileName;
                        
						gstd::Printer::c("termination criterions are not supperted at this point");

			//set the data files
			mgr.inDataFileNames.push_back(procname + "input_X.csv");
			mgr.inDataFileNames.push_back(procname + "input_Y.csv");
			mgr.inDataFiles.push_back(std::vector<std::vector<double> >());
			mgr.inDataFiles.push_back(std::vector<std::vector<double> >());

			//set the parameter files
			mgr.inHeaderFileNames.push_back(procname + "input_parms.csv");

			//set the output files
			mgr.outDataFileNames.push_back(procname + "output_B.csv");
			mgr.outHeaderFileNames.push_back(procname + "output_err.txt");
			mgr.outHeaderFileNames.push_back(procname + "output_log.txt");

			//set lambdas
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
			mgr.inHeaderFiles.push_back({ gstd::Printer::vp(lambdas, ',') });
		}
		void SlepBase::postStd(bool withSweep)
		{
			//intepret the output
			int numLambdas = lambdas.size();
			gstd::check(mgr.outHeaderFiles[0].size() == 0, "Slep failed. Message is: " + gstd::Printer::vp(mgr.outHeaderFiles[0], ' '));
			gstd::check((int)mgr.outDataFiles[0].size() == numLambdas, "Slep produced output of wrong dimension (numLambdas).");
			for (int l = 0; l<numLambdas; l++)
				gstd::check((int)mgr.outDataFiles[0][l].size() == parmDim, "Slep produced output of wrong dimension (d)");

			//set values
			std::vector<double> scores(parmDim, -numLambdas);
			for (int l = 0; l<numLambdas; l++)
			{
				parameter = mgr.outDataFiles[0][l];
				lambda = lambdas[l];
				for (int i = 0; i<parmDim; i++)
				if (parameter[i] != 0)
				{
					selection[i] = true;
					if (scores[i] == -numLambdas)
						scores[i] = -l;
				}
				else
					selection.erase(i);
				if (cache)
				{
					cacheModel();
					cacheSelectionModel();
				}
			}
			if (withSweep)
				ranks = gstd::vector::rank(scores, false);
		}

		//Base Package
		std::string SlepBase::toString()
		{
			std::stringstream res;
			res << "This is a ml::SlepBase" << std::endl;
			res << mgr.toString();
			return res.str();
		}

	}
}