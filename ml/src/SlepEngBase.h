#include "stdafx.h"

#pragma once
#ifndef SLEPENGBASE_H
#define	SLEPENGBASE_H

#include "standard.h"
#include "gstd_matlab/src/MatlabMgr.h"
#include "SelectionModel.h"

namespace msii810161816
{
	namespace ml
	{

		class MSII810161816_ML_API SlepEngBase : public virtual SelectionModel, public virtual gstd::Base
		{
		public:
			SlepEngBase();
			void prepStd(bool withSweep);
			void postStd(bool withSweep);

		protected:
			gstd::MatlabMgr mgr;
			std::vector<double> lambdas;
			int parmDim;
			static const std::string defaultCommand;

			//Base Package
		public:
			virtual std::string toString();


		};
	}
}
#endif