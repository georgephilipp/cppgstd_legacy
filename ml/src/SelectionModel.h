/* 
 * File:   SelectionModel.h
 * Author: gschoenh
 *
 * Created on December 5, 2013, 11:57 AM
 */

#include "stdafx.h"

#ifndef SELECTIONMODEL_H
#define	SELECTIONMODEL_H

#include "standard.h"
#include "Model.h"
#include "LambdaSweepSchedules.h"
#include "Datapoints.h"
#include "LinearRegression.h"
#include "Converters.h"
#include "Data/src/Converters.h"
#include "gstd/src/Map.h"
#include "gstd/src/Rand.h"
#include "gstd/src/Primitives.h"
#include "Data/src/Server.h"
#include "gstd/src/Timer.h"
#include "gstd/src/Writer.h"

namespace msii810161816
{   
    namespace ml
    {
		struct MSII810161816_ML_API SelectionCache
        {
            double lambda;
            std::map<int, bool> selection;
        };

		struct MSII810161816_ML_API SelectionCacheProfile
        {
            SelectionCacheProfile()
            {
                selection = false;
            }
            
            bool selection;
        };
           
		class MSII810161816_ML_API SelectionModel : virtual public Model, virtual public gstd::Base
        {
        public:
            SelectionModel();
            virtual ~SelectionModel();
            
            //inputs
            double lambda;
            
            //options
			int numFolds; 
                        bool crossValidationDocSweep;
            //cache options
            SelectionCacheProfile selectionCacheProfile;
            //target options
            std::map<int, bool> targetSelection;
            //schedules
            gstd::Pointer<LambdaSweepSchedule> lambdaSweepSchedule;
            
            //outputs
            std::map<int, bool> selection;
            std::vector<double> ranks;
            //cache outputs
            std::vector<SelectionCache> selectionCache;
            
			//actions
            void cacheSelectionModel();
            void rank();
			virtual void crossValidate() = 0;
            virtual void sweep() = 0;

            //Base package
		public:
            virtual std::string toString();

			//Standard Fns
		protected:
			void sweepStd();
			void fitDocStd();
			void sweepDocStd();
        };
        
        template<typename datatypename, typename headerTypeName>
        class SelectionModelTyped : virtual public SelectionModel, public ModelClient<datatypename, headerTypeName> 
        {
        public:
            virtual std::string toString() 
            {
                std::stringstream res;
                res << "This is a ml::SelectionModelTyped";
                res << ModelClient<datatypename,headerTypeName>::toString();
                res << SelectionModel::toString();
                return res.str();
            }
        };

		template<typename headerTypeName>
		class SelectionModelTyped<DpRegression, headerTypeName> : virtual public SelectionModel, public ModelClient<DpRegression, headerTypeName>
		{
		public:
			virtual void crossValidate()
			{
				//back up model
				std::vector<SelectionCache> selectionCacheBackup = selectionCache;
				std::vector<ModelCache> modelCacheBackup = modelCache;
				bool cacheBackup = cache;
				bool selectionCacheProfileBackup = selectionCacheProfile.selection;
				bool modelCacheProfileBackup = modelCacheProfile.parameter;
				int maxDataPointBackup = maxDataPoint;
				bool origserverWasOwned = false;
				if (ModelClient<DpRegression, headerTypeName>::client.pointerIsOwned())
					origserverWasOwned = true;

				//get the data stream from the model
				gstd::Pointer<data::Server<DpRegression, headerTypeName> > origserver;
				origserver.set(ModelClient<DpRegression, headerTypeName>::client.get(origserverWasOwned), origserverWasOwned);
				ModelClient<DpRegression, headerTypeName>::client.setnull();

				//initialize
				gstd::check(numFolds >= 0, "numFolds must be set");
				int numFoldsUsed = numFolds;
				if (numFolds == 0)
					numFoldsUsed = maxDataPoint;
				int numLambdas = 0; //this is used to store the size of the underlying sweep
				std::vector<double> lambdas; //this is used to store the lambdas used in the underlying sweep
				std::vector<double> errors;
				std::vector<std::vector<int> > modelSizes;
				std::vector<int> folds = { 0 };
				for (int i = 0; i < numFoldsUsed; i++)
					folds.push_back(gstd::Int::create(((double)maxDataPoint) / ((double)numFoldsUsed)*((double)(i + 1)), "round"));
				gstd::check(folds[numFoldsUsed] == maxDataPoint, "folds not created correctly");
				std::vector<int> rperm = gstd::Rand::perm(maxDataPoint, maxDataPoint);
				LinearRegression linReg;
				data::IndexMapper<DpRegression, headerTypeName> mapper;
				DpRegressionComponentMapper<headerTypeName> cMapper;
				data::Converter<DpRegression, DpRegression, headerTypeName, data::DataHeader> conv;
				mapper.inServer.set(origserver.get(false), false);
				cMapper.inServer.set(origserver.get(false), false);
				ModelClient<DpRegression, headerTypeName>::client.set(&mapper, false);
				conv.inServer.set(&mapper, false);
				linReg.client.set(&conv, false);
				int maxModelUsed = gstd::Int::create(((double)maxDataPoint) / ((double)numFoldsUsed) / 2 * ((double)(numFoldsUsed - 1)), "down");
				int maxLambdaUsed = -1;

				//iterate
				for (int iter = 0; iter<numFoldsUsed; iter++)
				{
					if (gstd::Base::doc)
					{
						gstd::Printer::c("starting iteration " + std::to_string(iter));
                                                gstd::Timer::inst.t();
					}

					//set up the mapper
					gstd::vector::empty(mapper.indexmap);
					for (int fold = 0; fold < numFoldsUsed; fold++)
					{
						if (iter != fold)
						{
							for (int i = folds[fold]; i < folds[fold + 1]; i++)
								mapper.indexmap.push_back(rperm[i]);
						}
					}
					maxDataPoint = mapper.indexmap.size();
					linReg.maxDataPoint = mapper.indexmap.size();
					gstd::check(!ModelClient<DpRegression, headerTypeName>::client.get(false)->get(folds[numFoldsUsed] - folds[iter + 1] + folds[iter]).success, "mapper not set correctly");
					gstd::check(ModelClient<DpRegression, headerTypeName>::client.get(false)->get(folds[numFoldsUsed] - folds[iter + 1] + folds[iter] - 1).success, "mapper not set correctly");

					//set caches to enable processing                    
					gstd::vector::empty(selectionCache);
					gstd::vector::empty(modelCache);
					cache = true;
					selectionCacheProfile.selection = true;
					modelCacheProfile.parameter = false;

					if(gstd::Base::doc)
                                        {
                                            gstd::Printer::c("sweep begins");
                                            gstd::Timer::inst.t();
                                        }
                                        
                                        bool docBack = gstd::Base::doc;
                                        gstd::Base::doc = SelectionModel::crossValidationDocSweep;
                                            
					//run the sweep
					sweep();

                                        gstd::Base::doc = docBack;
                                        
                                        if(gstd::Base::doc)
                                        {
                                            gstd::Printer::c("sweep ended");
                                            gstd::Timer::inst.t();
                                        }                                       

					//if this is first iteration, do some more initializing                
					if (iter == 0)
					{
						numLambdas = selectionCache.size();
						maxLambdaUsed = numLambdas;
						lambdas.resize(numLambdas);
						for (int l = 0; l<numLambdas; l++)
							lambdas[l] = selectionCache[l].lambda;
					}

					//interpret the results
					gstd::check((int)selectionCache.size() == numLambdas, "incorrect number of iterations in model sweep");
					for (int l = 0; l<numLambdas; l++)
						gstd::check(selectionCache[l].lambda == lambdas[l], "incorrect lambda in model sweep");

					//set limit to the lambdas so we don't fit undercomplete l2 models
					for (int l = 0; l < numLambdas; l++)
					{
						if ((int)selectionCache[l].selection.size() > maxModelUsed)
						{
							if (l < maxLambdaUsed)
								maxLambdaUsed = l;
							break;
						}
					}
					errors.resize(maxLambdaUsed);
					modelSizes.resize(maxLambdaUsed);

					std::map<int, bool> mapperMap;
					int testcount = 0;
					if (safe)
					{
						mapperMap = gstd::map::create(mapper.indexmap);
					}

					linReg.push();
					std::vector<int> rowInds;
					for (int i = 0; i < maxDataPoint; i++)
						rowInds.push_back(i);

					if(gstd::Base::doc)
                                        {
                                            gstd::Printer::c("starting l2 fitting");
                                            gstd::Timer::inst.t();
                                        }
                                        
					for (int l = 0; l < maxLambdaUsed; l++)
						modelSizes[l].push_back(selectionCache[l].selection.size());
					double runningError = 0;
					for (int l = 0; l < maxLambdaUsed; l++)
					{
						if (l > 0 && selectionCache[l].selection == selectionCache[l - 1].selection)
						{
							errors[l] += runningError;
							continue;
						}
						std::vector<int> currentSelection = gstd::map::keys(selectionCache[l].selection);
						cMapper.componentMap = currentSelection;

						linReg.fitSubset(rowInds, currentSelection);
						double error = 0;
						for (int i = folds[iter]; i < folds[iter + 1]; i++)
						{
							if (safe)
							{
								gstd::check(mapperMap.count(rperm[i]) == 0, "data point used for both train and test");
								testcount++;
							}
							error += linReg.test(cMapper.get(rperm[i]).result).second;
						}
						if (safe)
						{
							gstd::check(testcount + (int)mapperMap.size() == maxDataPointBackup, "train + test is not whole data set");
							testcount = 0;
						}
						errors[l] += error;
						runningError = error;
					}

					//give the user an idea of how well we chose lambdas
					if (gstd::Base::doc)
					{
						gstd::Printer::c("completed iteration " + std::to_string(iter));
						gstd::Printer::c("number of Lambda iterations : " + std::to_string(maxLambdaUsed));
						gstd::Printer::c("least lambda used is : " + std::to_string(lambdas[maxLambdaUsed - 1]));
						int optimum = gstd::vector::vmin(errors).first;
						gstd::Printer::c("current optimum lambda is : " + std::to_string(lambdas[optimum]));
						std::map<double, double> errorMap;
						for (int l = 0; l < maxLambdaUsed; l++)
							errorMap[lambdas[l]] = errors[l];
						gstd::Printer::c("test errors are: " + gstd::Printer::mapp(errorMap));
					}
				}

				//process final results
				int optimum = gstd::vector::vmin(errors).first;
				lambda = lambdas[optimum];

				//restore model
				selectionCache = selectionCacheBackup;
				modelCache = modelCacheBackup;
				cache = cacheBackup;
				selectionCacheProfile.selection = selectionCacheProfileBackup;
				maxDataPoint = maxDataPointBackup;
				modelCacheProfile.parameter = modelCacheProfileBackup;
				ModelClient<DpRegression, headerTypeName>::client.set(origserver.get(origserverWasOwned), origserverWasOwned);

				if (gstd::Base::doc)
				{
					gstd::Printer::c("finished cross-validation");
                                        gstd::Printer::c("lambda chosen was : " + gstd::Printer::p(lambda));
                                        gstd::Printer::c("its index was : " + gstd::Printer::p(optimum));
                                        gstd::Printer::c("numLambdas was : " + gstd::Printer::p(numLambdas));
                                        gstd::Printer::c("maxLambdaUsed was : " + gstd::Printer::p(maxLambdaUsed));
                      			gstd::Printer::c("error at used lambda was : " + gstd::Printer::p(errors[optimum]));
                                        gstd::Printer::c("error at smallest useable lambda was : " + gstd::Printer::p(errors[maxLambdaUsed - 1]));
					gstd::Printer::c("submodel sizes were : " + gstd::Printer::vp(modelSizes[optimum]));
				}
                                
                                gstd::check(numLambdas != maxLambdaUsed || !gstd::Double::equals(errors[optimum], errors[errors.size() - 1]), "lambda schedule not comprehensive enough");
			}


			//Base Package
		public:
			//tested in Select_LassoExtSlepEng
			virtual std::string toString()
			{
				std::stringstream res;
				res << "This is a ml::SelectionModelTyped";
				res << ModelClient<DpRegression, headerTypeName>::toString();
				res << SelectionModel::toString();
				return res.str();
			}
		};

		template<typename headerTypeName>
		class SelectionModelTyped<DpMTRegression, headerTypeName> : virtual public SelectionModel, public ModelClient<DpMTRegression, headerTypeName>
		{
		public:
			virtual void crossValidate()
			{
				//back up model
				std::vector<SelectionCache> selectionCacheBackup = selectionCache;
				std::vector<ModelCache> modelCacheBackup = modelCache;
				bool cacheBackup = cache;
				bool selectionCacheProfileBackup = selectionCacheProfile.selection;
				bool modelCacheProfileBackup = modelCacheProfile.parameter;
				int maxDataPointBackup = maxDataPoint;
				bool origserverWasOwned = false;
				if (ModelClient<DpMTRegression, headerTypeName>::client.pointerIsOwned())
					origserverWasOwned = true;

				//set dimensions
				gstd::trial<DpMTRegression> initpoint = ModelClient<DpMTRegression, headerTypeName>::client.get(false)->get(0);
				gstd::check(initpoint.success, "data not found");
				int outDim = initpoint.result.output.size();
				int inDim = initpoint.result.input.size();

				//get the data stream from the model
				gstd::Pointer<data::Server<DpMTRegression, headerTypeName> > origserver;
				origserver.set(ModelClient<DpMTRegression, headerTypeName>::client.get(origserverWasOwned), origserverWasOwned);
				ModelClient<DpMTRegression, headerTypeName>::client.setnull();

				//initialize
				gstd::check(numFolds >= 0, "numFolds must be set");
				int numFoldsUsed = numFolds;
				if (numFolds == 0)
					numFoldsUsed = maxDataPoint;
				int numLambdas = 0; //this is used to store the size of the underlying sweep
				std::vector<double> lambdas; //this is used to store the lambdas used in the underlying sweep
				std::vector<double> errors;
				std::vector<std::vector<int> > modelSizes;
				std::vector<int> folds = { 0 };
				for (int i = 0; i < numFoldsUsed; i++)
					folds.push_back(gstd::Int::create(((double)maxDataPoint) / ((double)numFoldsUsed)*((double)(i + 1)), "round"));
				gstd::check(folds[numFoldsUsed] == maxDataPoint, "folds not created correctly");
				std::vector<int> rperm = gstd::Rand::perm(maxDataPoint, maxDataPoint);
				LinearRegression linReg;
				data::IndexMapper<DpMTRegression, headerTypeName> mapper;
				DpRegressionComponentMapper<headerTypeName> cMapper;
				data::Converter<DpRegression, DpRegression, headerTypeName, data::DataHeader> conv;
				DpMTRegressionTaskExtractor<headerTypeName> extr1;
				DpMTRegressionTaskExtractor<headerTypeName> extr2;
				mapper.inServer.set(origserver.get(false), false);
				cMapper.inServer.set(&extr2, false);
				ModelClient<DpMTRegression, headerTypeName>::client.set(&mapper, false);
				conv.inServer.set(&extr1, false);
				linReg.client.set(&conv, false);
				extr1.inServer.set(&mapper, false);
				extr2.inServer.set(origserver.get(false), false);
				int maxModelUsed = gstd::Int::create(((double)maxDataPoint) / ((double)numFoldsUsed) / 2 * ((double)(numFoldsUsed - 1)), "down");
				int maxLambdaUsed = -1;

				//iterate
				for (int iter = 0; iter<numFoldsUsed; iter++)
				{
					if (gstd::Base::doc)
					{
						gstd::Printer::c("starting iteration " + std::to_string(iter));
                                                gstd::Timer::inst.t();
					}
                                        
					//set up the mapper
					gstd::vector::empty(mapper.indexmap);
					for (int fold = 0; fold < numFoldsUsed; fold++)
					{
						if (iter != fold)
						{
							for (int i = folds[fold]; i < folds[fold + 1]; i++)
								mapper.indexmap.push_back(rperm[i]);
						}
					}
					maxDataPoint = mapper.indexmap.size();
					linReg.maxDataPoint = mapper.indexmap.size();
					gstd::check(!ModelClient<DpMTRegression, headerTypeName>::client.get(false)->get(folds[numFoldsUsed] - folds[iter + 1] + folds[iter]).success, "mapper not set correctly");
					gstd::check(ModelClient<DpMTRegression, headerTypeName>::client.get(false)->get(folds[numFoldsUsed] - folds[iter + 1] + folds[iter] - 1).success, "mapper not set correctly");

					//set caches to enable processing                    
					gstd::vector::empty(selectionCache);
					gstd::vector::empty(modelCache);
					cache = true;
					selectionCacheProfile.selection = true;
					modelCacheProfile.parameter = false;

                                        if(gstd::Base::doc)
                                        {
                                            gstd::Printer::c("sweep begins");
                                            gstd::Timer::inst.t();
                                        }
                                        
					bool docBack = gstd::Base::doc;
                                        gstd::Base::doc = SelectionModel::crossValidationDocSweep;
                                            
					//run the sweep
					sweep();

                                        gstd::Base::doc = docBack;

                                        if(gstd::Base::doc)
                                        {
                                            gstd::Printer::c("sweep ended");
                                            gstd::Timer::inst.t();
                                        }
                                        
					//if this is first iteration, do some more initializing                
					if (iter == 0)
					{
						numLambdas = selectionCache.size();
						maxLambdaUsed = numLambdas;
						lambdas.resize(numLambdas);
						for (int l = 0; l<numLambdas; l++)
							lambdas[l] = selectionCache[l].lambda;
					}

					//interpret the results
					gstd::check((int)selectionCache.size() == numLambdas, "incorrect number of iterations in model sweep");
					for (int l = 0; l<numLambdas; l++)
						gstd::check(selectionCache[l].lambda == lambdas[l], "incorrect lambda in model sweep");

					//split up the selections by task
					std::vector<std::vector<std::map<int, bool> > > taskWiseSelections;
					for (int l = 0; l < maxLambdaUsed; l++)
					{
						int totalSelected = 0;
						std::vector<std::map<int, bool> > lambdaTaskWiseSelection(outDim);
						for (int i = 0; i < inDim; i++)
						{
							for (int t = 0; t < outDim; t++)
							{
								if (selectionCache[l].selection.count(t*inDim + i) == 1)
								{
									lambdaTaskWiseSelection[t][i] = true;
									totalSelected++;
								}
							}
						}
						taskWiseSelections.push_back(lambdaTaskWiseSelection);
						gstd::check(totalSelected == (int)selectionCache[l].selection.size(), "creating taskwise selection has failed");
					}					

					//set limit to the lambdas so we don't fit undercomplete l2 models
					for (int l = 0; l < maxLambdaUsed; l++)
					{
						for (int t = 0; t < outDim; t++)
						{
							if ((int)taskWiseSelections[l][t].size() > maxModelUsed)
							{
								if (l < maxLambdaUsed)
									maxLambdaUsed = l;
								break;
							}
						}
					}
					errors.resize(maxLambdaUsed);
					modelSizes.resize(maxLambdaUsed);

					std::map<int, bool> mapperMap;
					int testcount = 0;
					if (safe)
					{
						mapperMap = gstd::map::create(mapper.indexmap);
					}

					std::vector<int> rowInds;
					for (int i = 0; i < maxDataPoint; i++)
						rowInds.push_back(i);

					if(gstd::Base::doc)
                                        {
                                            gstd::Printer::c("starting l2 fitting");
                                            gstd::Timer::inst.t();
                                        }
                                        
					for (int l = 0; l < maxLambdaUsed; l++)
						modelSizes[l].push_back(selectionCache[l].selection.size());
					for (int t = 0; t < outDim; t++)
					{
						extr1.taskNum = t;
						extr2.taskNum = t;
						gstd::check(extr1.get(0).result.output == mapper.get(0).result.output[t], "extraction failed");
						gstd::check(extr2.get(0).result.output == origserver.get(false)->get(0).result.output[t], "extraction failed");
						linReg.push();
						double runningError = 0;
						for (int l = 0; l < maxLambdaUsed; l++)
						{
							if (l > 0 && taskWiseSelections[l][t] == taskWiseSelections[l - 1][t])
							{
								errors[l] += runningError;
								continue;
							}
							std::vector<int> currentSelection = gstd::map::keys(taskWiseSelections[l][t]);
							cMapper.componentMap = currentSelection;

							linReg.fitSubset(rowInds, currentSelection);
							double error = 0;
							for (int i = folds[iter]; i < folds[iter + 1]; i++)
							{
								if (safe)
								{
									gstd::check(mapperMap.count(rperm[i]) == 0, "data point used for both train and test");
									testcount++;
								}
								error += linReg.test(cMapper.get(rperm[i]).result).second;
							}
							if (safe)
							{
								gstd::check(testcount + (int)mapperMap.size() == maxDataPointBackup, "train + test is not whole data set");
								testcount = 0;
							}
							errors[l] += error;
							runningError = error;
						}
					}
					

					//give the user an idea of how well we chose lambdas
					if (gstd::Base::doc)
					{
						gstd::Printer::c("completed iteration " + std::to_string(iter));
						gstd::Printer::c("number of Lambda iterations : " + std::to_string(maxLambdaUsed));
						gstd::Printer::c("least lambda used is : " + std::to_string(lambdas[maxLambdaUsed - 1]));
						int optimum = gstd::vector::vmin(errors).first;
						gstd::Printer::c("current optimum lambda is : " + std::to_string(lambdas[optimum]));
						std::map<double, double> errorMap;
						for (int l = 0; l < maxLambdaUsed; l++)
							errorMap[lambdas[l]] = errors[l];
						gstd::Printer::c("test errors are: " + gstd::Printer::mapp(errorMap));
					}
				}

				//process final results
				int optimum = gstd::vector::vmin(errors).first;				
				lambda = lambdas[optimum];

				//restore model
				selectionCache = selectionCacheBackup;
				modelCache = modelCacheBackup;
				cache = cacheBackup;
				selectionCacheProfile.selection = selectionCacheProfileBackup;
				maxDataPoint = maxDataPointBackup;
				modelCacheProfile.parameter = modelCacheProfileBackup;
				ModelClient<DpMTRegression, headerTypeName>::client.set(origserver.get(origserverWasOwned), origserverWasOwned);

				if (gstd::Base::doc)
				{
					gstd::Printer::c("finished cross-validation");
                                        gstd::Printer::c("lambda chosen was : " + gstd::Printer::p(lambda));
                                        gstd::Printer::c("its index was : " + gstd::Printer::p(optimum));
                                        gstd::Printer::c("numLambdas was : " + gstd::Printer::p(numLambdas));
                                        gstd::Printer::c("maxLambdaUsed was : " + gstd::Printer::p(maxLambdaUsed));
                      			gstd::Printer::c("error at used lambda was : " + gstd::Printer::p(errors[optimum]));
                                        gstd::Printer::c("error at smallest useable lambda was : " + gstd::Printer::p(errors[maxLambdaUsed - 1]));
					gstd::Printer::c("submodel sizes were : " + gstd::Printer::vp(modelSizes[optimum]));
				}
                                
                                gstd::check(numLambdas != maxLambdaUsed || !gstd::Double::equals(errors[optimum], errors[errors.size() - 1]), "lambda schedule not comprehensive enough");
			}


			//Base Package
		public:
			//tested in Select_LassoExtSlepEng
			virtual std::string toString()
			{
				std::stringstream res;
				res << "This is a ml::SelectionModelTyped";
				res << ModelClient<DpMTRegression, headerTypeName>::toString();
				res << SelectionModel::toString();
				return res.str();
			}
		};
    }
}

#endif	/* SELECTIONMODEL_H */

