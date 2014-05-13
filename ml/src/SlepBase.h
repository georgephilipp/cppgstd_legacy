#include "stdafx.h"

#pragma once
#ifndef SLEPBASE_H
#define	SLEPBBASE_H

#include "standard.h"
#include "gstd/src/TerminalMgr.h"
#include "SelectionModel.h"

namespace msii810161816
{
	namespace ml
	{

		class MSII810161816_ML_API SlepBase : public virtual SelectionModel, public virtual gstd::Base
		{
		public:
			gstd::TerminalMgr mgr;
                        
                        //options
                        std::string procname; //id of the process

			SlepBase();
			void prepStd(bool withSweep);
			void postStd(bool withSweep);
                        
                        static const std::string defaultCommandBase;
                       
		protected:
			std::vector<double> lambdas;
			int parmDim;

			//Base Package
		public:
			virtual std::string toString();


		};
	}
}
#endif