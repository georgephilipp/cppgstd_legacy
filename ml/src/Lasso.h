/* 
 * File:   Lasso.h
 * Author: gschoenh
 *
 * Created on November 14, 2013, 11:34 AM
 */

#include "stdafx.h"

#ifndef LASSO_H
#define	LASSO_H

#include"SelectionModel.h"
#include"standard.h"
#include"Datapoints.h"
#include"Aggregators.h"
#include"Data/src/Dataset.h"
#include"Data/src/DataHeaders.h"

namespace msii810161816
{   
    namespace ml
    {
		class MSII810161816_ML_API Lasso : public SelectionModelTyped<DpRegression, data::DataHeader>, public virtual gstd::Base
        {
        public:
            Lasso();
            /*Lasso(const Lasso& orig);*/
            virtual ~Lasso();
            
            // inputs : specify those before running anything
            //inherited: int maxDataPoint;
            //inherited: double lambda; //regularization parameter
            //inherited: data::Client<data::DpRegression,data::HeaderType> client; //interface to data server
            
            //outputs
            //inherited: std::vector<double> parameter; //parameter. modified during fitting 
            //inherited: std::map<int, bool> selection; //map of selected components (contains keys of components that were selected)
            //inherited: std::vector<double> ranks;
            //cache
            //inherited: std::vector<ModelCache> modelCache; //caches model relevant information
            //inherited: std::vector<SelectionCache> selectionCache; //caches selection relevant information
            
            //options
			double initialparameterscale; //size of randomly initialized parameters
			int numiter;  //number of iterations for which to run lasso
			//inherited: int numFolds; // for xVal
			//inherited: double ytol; //maximal size of parameter update afterwhich to terminate Lasso
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
            
            //actions
            virtual void initparameter(); //initialize parameters for a cold / clean start. Must be called when the model is trained for first time
            virtual void fit(); //fit the model
            virtual void sweep(); //fun the model for various velues of lambda and obtain a combined selection / ranking
			
            //caching
            //inherited: void cacheModel(); //insert model outputs into cache
            //inherited: void cacheSelectionModel(); //insert selection model outputs into cache
            
			//Base package
            virtual void setInputs(); //sets inputs for inputs for quick testing
            virtual bool test(); //test the algorithm
            virtual gstd::TypeName getTypeName(); //get string name of the type of this object
            virtual std::string toString();
		protected:
			bool testInner(Lasso& obj);
           
        private:
            RegressionAggregator aggregator; //loads datapoints for fast access. 
            
        };
    }
}

namespace msii810161816
{
    namespace gstd
    {
        template<>
        struct TypeNameGetter<ml::Lasso>
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "ml::Lasso";
                return t;
            }
        };
    }
}

#endif	/* LASSO_H */

