/* 
 * File:   StabilitySelection.h
 * Author: gschoenh
 *
 * Created on November 25, 2013, 7:14 PM
 */

#include "stdafx.h"

#pragma once
#ifndef STABILITYSELECTION_H
#define	STABILITYSELECTION_H

#include "standard.h"
#include "Model.h"
#include "Lasso.h"
#include "Data/src/Converters.h"
#include "gstd/src/Linalg.h"
#include "gstd/src/Primitives.h"
#include "gstd/src/Map.h"
#include "gstd/src/Vector.h"
#include "gstd/src/Rand.h"
#include "gstd/src/Timer.h"
#include "LassoExtSlep.h"
#include "LassoExtSlepEng.h"
#include <math.h>
#include <numeric>

namespace msii810161816
{   
    namespace ml
    {
		struct MSII810161816_ML_API StabilityCacheSchedule
        {
            std::vector<double> lambdaSchedule;
            std::vector<double> stability;
            std::map<int, bool> selection;
        };
        
		struct MSII810161816_ML_API StabilityCache
        {
            double lambda;
            std::vector<double> stability;
        };
        
		struct MSII810161816_ML_API StabilityCacheProfile
        {
            StabilityCacheProfile()
            {
                stability = false;
                selection = false;
            }
            
            bool stability;
            bool selection;
        };
        
		bool MSII810161816_ML_API stabilitySelectionTestHelperFn(std::string mode);

		template<typename datatypename, typename headertype>
        class StabilitySelection;
        
        template<typename datatypename, typename headertype>
        class StabilitySelectionGenericBase : public SelectionModelTyped<datatypename,headertype>
        {
        public:
            StabilitySelectionGenericBase() 
            {
                totalToSelect = -1;
                expectedNumberFalseSelection = -1;
                selectionThreshold = -1;
            }
            virtual ~StabilitySelectionGenericBase() {}
            
            //inputs
            double assignfraction; //fraction of total dataset to assign to each run
            int numiter; //number of times to run the submodel
            gstd::Pointer<SelectionModelTyped<datatypename, headertype> > submodel;
            
            //outputs
            //inherited: std::map<int, bool> selection; //map of selected components (contains keys of components that were selected)
            //inherited: std::vector<double> ranks; //applies a ranking to the components in terms of how likely we are to select them
            std::vector<double> stability;
            //cache
            //inherited: std::vector<SelectionCache> selectionCache; //caches selection relevant information
            std::vector<StabilityCache> stabilityCache;
            std::vector<StabilityCacheSchedule> stabilityCacheSchedule;
            
            //options
            //inherited: bool doc; //turning on printing to console of intermediate information
            //inherited: bool crossValidationDocSweep;
            //inherited: bool safe; //turning of safety checks
            //for selection : criterions will be used from top to bottom
            int totalToSelect;
            double selectionThreshold;
            double expectedNumberFalseSelection;
            //cache options
            //inherited: bool cache; //global flag to activate caching of results
            //inherited: SelectionCacheProfile selectionCacheProfile; //set which selection relevant information to cache from model runs
            StabilityCacheProfile stabilityCacheProfile;
            //target options
            //inherited: std::vector<double> target; //used to run performance measures
            //inherited: std::vector<double> targetSelection; //used to run performance mesaures

            //actions
            //inherited void cacheSelectionModel(); //insert selection model outputs into cache
            virtual void initparameter() {}
            
            void fitInner(bool withSweep)
            {
                SelectionModelTyped<datatypename, headertype>* sub = submodel.get(false);

                //back up submodel
                double lamdbaBackup = sub->lambda;
                std::vector<SelectionCache> selectionCacheBackup = sub->selectionCache;
				std::vector<ModelCache> modelCacheBackup = sub->modelCache;
                bool cacheBackup = sub->cache;
                bool selectionCacheProfileBackup = sub->selectionCacheProfile.selection;
                bool modelCacheProfileBackup = sub->modelCacheProfile.parameter;
                int maxDataPointBackup = sub->maxDataPoint;
                bool origserverWasOwned = false;
                if(sub->client.pointerIsOwned())
                {
                    origserverWasOwned = true;
                    sub->client.get(true); 
                }

				//NOTE!!! For a brief period, the origserver may be unowned, which is an unsafe pointer state if we are in a try block. Also, later, the mapper that is assigned to sub is in a lower scope, which has the same problem.
                        
                //get the data stream from the submodel
                data::Server<datatypename,headertype> *origserver = sub->client.get(false);    
                                            
                //initialize
                int P = 0;
                double increment = 1 / (double)numiter;
                int numLambdas = 0; //this is used to store the size of the underlying sweep
                std::vector<double> lambdas(0); //this is used to store the lambdas used in the underlying sweep
                Model::maxDataPoint = sub->maxDataPoint;//used the submodel data set size       
                std::vector<std::vector<double>> stabilities(numLambdas);
                std::vector<std::vector<double>> maxStabilities(numLambdas);
                gstd::vector::empty(stability);

                //iterate
                for(int iter=0;iter<numiter;iter++)
                {              
                    if(gstd::Base::doc)
                    {
                        gstd::Printer::c("starting iteration " + std::to_string(iter));
                        gstd::Timer::inst.t();
                    }
                    
                    //set up the mapper
                    data::IndexMapper<datatypename,headertype> mapper;
                    int N = Model::maxDataPoint;
                                        
                    if(N == -1)
                    {
                        origserver->setminfail();
                        N = origserver->minfail;
                    }                       

                    std::vector<int> map;
                    for(int j=0;j<N;j++)
                        if(gstd::Rand::d(3)<assignfraction)
                            map.push_back(j);
                    mapper.indexmap=map;
                    mapper.inServer.set(origserver, false); 

                    sub->client.set(&mapper, false);
                    sub->maxDataPoint = map.size();
                
                    if(gstd::Base::safe)
                    {
                        sub->client.get(false)->setminfail();
                        int subN = sub->client.get(false)->minfail;
                        double mean = assignfraction*N;
                        double std = sqrt(assignfraction*(1-assignfraction)*N);
                        gstd::check(abs(((double)subN - mean)/std) < 5, "incorrect submodel data set size");
                    }
                                        
                    //set submodel to enable processing                    
                    if(withSweep)
                    {
                        gstd::vector::empty(sub->selectionCache);
						gstd::vector::empty(sub->modelCache);
                        sub->cache = true;
                        sub->selectionCacheProfile.selection = true;
                        sub->modelCacheProfile.parameter = false;
                    }
                    else
                    {
                        sub->cache = false;
                        sub->selectionCacheProfile.selection = false;
                        sub->modelCacheProfile.parameter = false;
                    }
                    
                    //run the submodel
                    if(withSweep)
                        sub->sweep();
                    else
                        sub->fit();
                                                                               
                    //if this is first iteration, do some more initializing                
                    if(iter == 0)
                    {
                        P = sub->parameter.size();
                        if(withSweep)
                        {
                            numLambdas = sub->selectionCache.size();
                            lambdas.resize(numLambdas);
                            stabilities.resize(numLambdas);
                            maxStabilities.resize(numLambdas);
                            for(int l=0;l<numLambdas;l++)
                                lambdas[l] = sub->selectionCache[l].lambda;
                            for(int l=0;l<numLambdas;l++)
                                stabilities[l].resize(P);
                            for(int l=0;l<numLambdas;l++)
                                maxStabilities[l].resize(P);
                        }
                        stability.resize(P);   
                        
                        if(gstd::Base::doc)
                        {
                            gstd::Printer::c("P is  " + std::to_string(P));
                            gstd::Printer::c("Target q is  " + std::to_string(sqrt(0.8*P)));
                        }
                    }
                                        
                    //interpret the results
                    if(withSweep)
                    {
                        if(gstd::Base::safe)
                        {
                            gstd::check((int)sub->selectionCache.size() == numLambdas, "incorrect number of iterations in submodel sweep");
                            for(int l=0;l<numLambdas;l++)
                                gstd::check(sub->selectionCache[l].lambda == lambdas[l], "incorrect lambda in submodel sweep");
                        }
                        
                        for(int p=0;p<P;p++)
                        {
                            bool wasSelected = false;
                            for(int l=0;l<numLambdas;l++)
                            {
                                if(sub->selectionCache[l].selection.count(p) == 1)
                                {
                                    stabilities[l][p] += increment;
                                    wasSelected = true;
                                }
                                if(wasSelected)
                                    maxStabilities[l][p] += increment;
                            }
                        }
                    }
                    else
                    {
                        for(int p=0;p<P;p++)
                            if(sub->selection.count(p) == 1)
                                stability[p] += increment;
                    }   
                                        
                    //give the user an idea of how well he chose lambdas
                    if(gstd::Base::doc)
                    {
                        gstd::Printer::c("completed iteration " + std::to_string(iter));
                        if(withSweep)
                        {
                            std::map<double, double> qvals;
                            std::map<double, double> maxqvals;
                            for(int l=0;l<numLambdas;l++)
                            {
                                double qval = 0;
                                double maxqval = 0;
                                for(int p=0;p<P;p++)
                                {
                                    qval += stabilities[l][p];
                                    maxqval += maxStabilities[l][p];
                                }
                                qvals[lambdas[l]] = qval/((double)(iter+1))/increment;
                                maxqvals[lambdas[l]] = maxqval/((double)(iter+1))/increment;
                            }    
                            gstd::Printer::c("qvals are: " + gstd::Printer::mapp(qvals));
                            gstd::Printer::c("maxqvals are: " + gstd::Printer::mapp(maxqvals));
                        }    
                        else
                        {
                            double qval = 0;
                            for(int p=0;p<P;p++)
                                qval += stability[p];
                            gstd::Printer::c("qval is: " + std::to_string(qval/((double)(iter+1))/increment));
                        }
                    }

					//cleanup
					sub->client.setnull();
                }  

                //process final results
                if(withSweep)
                {
                    std::vector<double> cacheSchedule(0);
                    for(int l=0;l<numLambdas;l++)
                    {
                        SelectionModel::lambda = lambdas[l];
                        stability = stabilities[l];
                        select();
                        SelectionModel::cacheSelectionModel();
                        cacheStability();
                        cacheSchedule.push_back(lambdas[l]);
                        stability = maxStabilities[l];
                        select();
                        cacheStabilitySchedule(cacheSchedule);
                    }                                        
                }
                else
                {
					SelectionModel::lambda = sub->lambda;
                    select();
                    
					if(Model::cache)
                    {
                        SelectionModel::cacheSelectionModel();
                        cacheStability();
                    } 
                }

				SelectionModel::ranks = gstd::vector::rank(stability, false);
                   
                //restore submodel
                sub->lambda = lamdbaBackup;
                sub->selectionCache = selectionCacheBackup;
				sub->modelCache = modelCacheBackup;
                sub->cache = cacheBackup;
                sub->selectionCacheProfile.selection = selectionCacheProfileBackup;
                sub->maxDataPoint = maxDataPointBackup;
                sub->modelCacheProfile.parameter = modelCacheProfileBackup;
                if(origserverWasOwned)
                    sub->client.set(origserver, true);
                else
                    sub->client.set(origserver, false);               
                          
                //print the results
                if(gstd::Base::doc)
                {
                    gstd::Printer::c("Sample of Target model was:");
                    gstd::Printer::vc(gstd::vector::sub(Model::target,0,100,true));
					gstd::Printer::c("Target selection is: ");
					std::vector<int> tsVector = gstd::map::keys(SelectionModel::targetSelection);
					gstd::Printer::vc(tsVector);
					if (withSweep)
					{
						for (int l = 0; l < numLambdas; l++)
						{
							gstd::Printer::c("Results for iteration : " + gstd::Printer::p(l));
							gstd::Printer::c("Lambda is : " + gstd::Printer::p(SelectionModel::selectionCache[l].lambda));
							gstd::Printer::c("Selection is : " + gstd::Printer::vp(gstd::map::keys(SelectionModel::selectionCache[l].selection)));
							gstd::Printer::c("Sample of stability is : " + gstd::Printer::vp(gstd::vector::sub(stabilityCache[l].stability, 0, 100, true)));
							std::vector<double> rks = gstd::vector::rank(stabilityCache[l].stability, false);
							gstd::Printer::c("Ranks of targets are : " + gstd::Printer::vp(gstd::vector::getmany(rks, tsVector)));
							gstd::Printer::c("Lambda Schedule is : " + gstd::Printer::vp(stabilityCacheSchedule[l].lambdaSchedule));
							gstd::Printer::c("Selection is : " + gstd::Printer::vp(gstd::map::keys(stabilityCacheSchedule[l].selection)));
							gstd::Printer::c("Sample of stability is : " + gstd::Printer::vp(gstd::vector::sub(stabilityCacheSchedule[l].stability, 0, 100, true)));
							rks = gstd::vector::rank(stabilityCacheSchedule[l].stability, false);
							gstd::Printer::c("Ranks of targets are : " + gstd::Printer::vp(gstd::vector::getmany(rks, tsVector)));
						}
					}
					else
					{
						gstd::Printer::c("Sample of Stability is:");
						gstd::Printer::vc(gstd::vector::sub(stability, 0, 100, true));
						gstd::Printer::c("Selection is: ");
						gstd::Printer::vc(gstd::map::keys(SelectionModel::selection));
						gstd::Printer::c("Target ranks are: ");
						gstd::Printer::vc(gstd::vector::getmany(SelectionModel::ranks, tsVector));
					}                    
                }  
            }
            
            virtual void fit() //fit the model
            {     
                fitInner(false);
            }
            
            void cacheStability()
            {
                StabilityCache cache;
                cache.lambda = SelectionModel::lambda;
                if(stabilityCacheProfile.stability)
                    cache.stability = stability;
                stabilityCache.push_back(cache);                
            }
            
            void cacheStabilitySchedule(std::vector<double> lambdaSchedule)
            {
                StabilityCacheSchedule cache;
                cache.lambdaSchedule = lambdaSchedule;
                if(stabilityCacheProfile.selection)
                    cache.selection = SelectionModel::selection;
                if(stabilityCacheProfile.stability)
                    cache.stability = stability;
                stabilityCacheSchedule.push_back(cache);
            }
            
            void select()
            {
                SelectionModel::selection.clear();
                if(totalToSelect > 0)
                {
                    std::vector<int> ranking = gstd::vector::sort<double>(stability, true).first;
                    gstd::check(totalToSelect <= (int)stability.size(), "cannot select more variables than exist");   
                    for(int k=0;k<totalToSelect;k++)
                        SelectionModel::selection[ranking[k]] = true;
                }
                else if(selectionThreshold > 0)
                {
                    double P = (double)stability.size();
                    for(int p=0;p<P;p++)
                        if(stability[p] > selectionThreshold)
                            SelectionModel::selection[p] = true;
                }
                else if(expectedNumberFalseSelection > 0)
                {    
                    double P = (double)stability.size();
                    double q = gstd::Linalg::msumall(stability)[0];
                    double thresh = (q*q/(P*expectedNumberFalseSelection)+1)/2;         
                    for(int p=0;p<P;p++)
                        if(stability[p] > thresh)
                            SelectionModel::selection[p] = true;
                }
            }
            
            void sweep()
            {
                gstd::check(Model::cache && (stabilityCacheProfile.selection || stabilityCacheProfile.stability), "must have cache enabled for sweep");
                fitInner(true);
            }
            
            virtual void setInputs()//sets values for inputs for quick testing
            {
                gstd::error("This is only implemented for the specialization of StabilitySelection (see below)!");
            }
            
            virtual bool test()//test the algorithm
            {
				gstd::Printer::c("NOTE!!!! This test has only about a 80% chance of succeeding, so please rerun with new random seed if fails.");
				return stabilitySelectionTestHelperFn("Lasso") /*&& stabilitySelectionTestHelperFn("LassoExtSlep") && stabilitySelectionTestHelperFn("LassoExtSlepEng")*/;
            }
            
            virtual gstd::TypeName getTypeName()
            {
                return gstd::TypeNameGetter<StabilitySelection<datatypename,headertype> >::get();
            }

            virtual std::string toString()
            {
                std::stringstream res;
                res << "This is a ml::StabilitySelection";
                res << "Assignfraction is: " << assignfraction << std::endl;
                res << "numiter is: " << numiter << std::endl;
                res << "totalToSelect is: " << totalToSelect << std::endl;
                res << "expectedNumberFalseSelection is: " << expectedNumberFalseSelection << std::endl;
                res << "size of stabilityCache is: " << stabilityCache.size() << std::endl;
                res << "Snipped from stability is: " << gstd::Printer::vp(gstd::vector::sub(stability, 0, 10, true)) << std::endl;               
                res << SelectionModelTyped<datatypename, headertype>::toString();               
                if(submodel.isNull())
                    res << "submodel not set" << std::endl;
                else
                    res << "The submodel: " << submodel.get(false)->toString();
                return res.str();
            }  
        };
        
        template<typename datatypename, typename headertype>
        class StabilitySelection : public StabilitySelectionGenericBase<datatypename,headertype> {};
                
        template<>
        class StabilitySelection<DpRegression,data::DataHeader> : public StabilitySelectionGenericBase<DpRegression,data::DataHeader> 
        {
        public:
            virtual void setInputs()//sets values for inputs for quick testing
            {
                assignfraction = 0.01;
                numiter = 100;    
				SelectionModelTyped<DpRegression,data::DataHeader>* _submodel = new Lasso;
                _submodel->setInputs();
                target = _submodel->target;
                targetSelection = _submodel->targetSelection;
                submodel.set(_submodel, true);     
                expectedNumberFalseSelection = 100;
            }        
        };   
    }
}

namespace msii810161816
{
    namespace gstd
    {
        template<typename datatype, typename headertype>
        struct TypeNameGetter<ml::StabilitySelection<datatype, headertype> >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "ml::StabilitySelection";
                t.parameters.push_back(TypeNameGetter<datatype>::get());
                t.parameters.push_back(TypeNameGetter<headertype>::get());
                return t;
            }
        };
    }
}

#endif	/* STABILITYSELECTION_H */

