#include "stdafx.h"

#pragma once
#ifndef GROUPLASSOEXTSLEP_H
#define	GROUPLASSOEXTSLEP_H

#include"standard.h"
#include"SelectionModel.h"
#include"Datapoints.h"
#include"DataHeaders.h"
#include"SlepBase.h"
#include"Lasso.h"

namespace msii810161816
{
	namespace ml
	{
		class MSII810161816_ML_API GroupLassoExtSlep : public SelectionModelTyped<DpRegression, InputGroupHeader>, public SlepBase, public virtual gstd::Base
		{
		public:
			GroupLassoExtSlep();
			virtual ~GroupLassoExtSlep();

			// inputs : specify those before running anything
			//inherited: int maxDataPoint;
			//inherited: double lambda; //regularization parameter
			//inherited: data::Client<data::DpRegression,data::HeaderType> client; //interface to data server
			//inherited: gstd::TerminalMgr mgr; //the default settings are for my computer 

			//outputs
			//inherited: std::vector<double> parameter; //parameter. modified during fitting 
			//inherited: std::map<int, bool> selection; //map of selected components (contains keys of components that were selected)
			//inherited: std::vector<double> ranks;
			//cache
			//inherited: std::vector<ModelCache> modelCache; //caches model relevant information
			//inherited: std::vector<SelectionCache> selectionCache; //caches selection relevant information

			//options
			//inherited: int numFolds; // for xVal
			//inherited: bool doc; //turning on printing to console of intermediate information
			//inherited: bool crossValidationDocSweep;
                        //inherited: bool safe; //turning of safety checks
			//cache options
			//inherited: bool cache; //global flag to activate caching of results
			//inherited: ModelCacheProfile modelCacheProfile; //select which model relevant information to cache
			//inherited: SelectionCacheProfile selectionCacheProfile; //set which selection relevant information to cache from model runs
			//target options
			//inherited: std::vector<double> target; //used to run performance measures
			//inherited: std::vector<double> targetSelection; //used to run performance mesaures
			//schedules
			//inherited: gstd::Pointer<LambdaSweepSchedule> lambdaSweepSchedule;
//systems options
                        //inherited: std::string procname; //give the process an id
                        
                        
			//actions
			virtual void fit(); //fit the model
			virtual void sweep(); //fun the model for various velues of lambda and obtain a combined selection / ranking
			void crossValidate() { SelectionModelTyped<DpRegression, InputGroupHeader>::crossValidate(); }
			//caching
			//inherited: void cacheModel(); //insert model outputs into cache
			//inherited: void cacheSelectionModel(); //insert selection model outputs into cache

			virtual void initparameter() {} //not used
		private:
			void fitInner(bool withSweep);

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
		struct TypeNameGetter<ml::GroupLassoExtSlep>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::GroupLassoExtSlep";
				return t;
			}
		};
	}
}
#endif