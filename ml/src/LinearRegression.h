
#include "stdafx.h"

#pragma once
#ifndef LINEARREGRESSION_H
#define	LINEARREGRESSION_H

#include"standard.h"
#include"Model.h"
#include "Datapoints.h"
#include "gstd/src/MatlabMgr.h"
#include <math.h>

namespace msii810161816
{
	namespace ml
	{
		class MSII810161816_ML_API LinearRegression : public ModelTyped<DpRegression, data::DataHeader>, public virtual gstd::Base
		{
		public:
			LinearRegression();
			~LinearRegression();

			//inputs
			// inputs : specify those before running anything
			//inherited: int maxDataPoint;
			//inherited: data::Client<DpRegression,data::DataHeader> client; //interface to data server

			//options
			//inherited:
			//bool doc;
			//bool safe;
			//target options
			//inherited: std::vector<double> target;

			//outputs
			//inherited: std::vector<double> parameter;
			std::vector<double> pvalues;

			//actions
			virtual void fit();
			virtual void initparameter();
			void fitWithOptions();
			double apply(data::DpDoubles input);
			std::pair<double, double> test(DpRegression input);
			void push();
			void fitSubset(std::vector<int> rowSubSet, std::vector<int> colSubSet);

		private:
			gstd::MatlabMgr mgr;
			int pushedRows;
			int pushedCols;

			//Base package
		public:
			virtual void setInputs(); //sets inputs for inputs for quick testing
			virtual bool test(); //test the algorithm
			virtual gstd::TypeName getTypeName(); //get string name of the type of this object
			virtual std::string toString();

		};
	}
}

namespace msii810161816
{
	namespace gstd
	{
		template<>
		struct TypeNameGetter<ml::LinearRegression>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::LinearRegression";
				return t;
			}
		};
	}
}

#endif	/* MODEL_H */

