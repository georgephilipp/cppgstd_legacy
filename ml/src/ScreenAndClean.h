#include "stdafx.h"

#pragma once
#ifndef SCREENANDCLEAN_H
#define	SCREENANDCLEAN_H

#include "standard.h"
#include "Datapoints.h"
#include "Data/src/DataHeaders.h"
#include "gstd/src/Map.h"
#include "gstd/src/Stat.h"
#include "gstd/src/Timer.h"
#include "LassoExtSlepEng.h"
#include "SelectionModel.h"
#include "Model.h"
#include "gstd/src/Linalg.h"
#include "SIOLassoExtSlepEng.h"

namespace msii810161816
{
	namespace ml
	{
		bool MSII810161816_ML_API screenAndCleanDpRegressionTestHelperFn();

		bool MSII810161816_ML_API screenAndCleanDpMTRegressionTestHelperFn();

		template<typename datatypename, typename headertype>
		class ScreenAndCleanGenericBase : public SelectionModelTyped<datatypename, headertype>
		{
		public:
			ScreenAndCleanGenericBase()
			{
				pvalueThreshold = -1;
			}
			virtual ~ScreenAndCleanGenericBase() {}

			//inputs
			int numiter; //number of iterations over which p value is averaged
			gstd::Pointer<SelectionModelTyped<datatypename, headertype> > submodel;

			//outputs
			//inherited: std::map<int, bool> selection; //map of selected components (contains keys of components that were selected)
			//inherited: std::vector<double> ranks; //applies a ranking to the components in terms of how likely we are to select them
			std::vector<double> pvalues; //contains the pvalue assigned to the components
			
			//options
			//inherited: bool doc; //turning on printing to console of intermediate information
			//inherited: bool safe; //turning of safety checks
			double pvalueThreshold;
			//target options
			//inherited: std::vector<double> target; //used to run performance measures
			//inherited: std::map<int, bool> targetSelection; //used to run performance mesaures

			//actions
			virtual void initparameter() {} //not used

			virtual void fit() //fit the model
			{
				gstd::error("This is only implemented for the specialization of ScreenAndClean (see below)!");
			}
						
			void select()
			{
				SelectionModel::selection.clear();
				gstd::check(pvalueThreshold != -1, "pvalue threshold not set. Cannot select");
				int d = pvalues.size();
				for (int i = 0; i < d; i++)
				{
					if (pvalues[i] < pvalueThreshold)
						SelectionModel::selection[i] = true;
				}
			}

			void sweep()
			{
				gstd::error("Sweep is not possible for this model as there are no hyperparameters");
			}

			//Base Package
			virtual void setInputs()//sets values for inputs for quick testing
			{
				gstd::error("This is only implemented for the specialization of ScreenAndClean (see below)!");
			}
                        
                        virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<ScreenAndCleanGenericBase<datatypename, headertype> >::get();
			}
			
			virtual bool test()//test the algorithm
			{
				return screenAndCleanDpRegressionTestHelperFn() && screenAndCleanDpMTRegressionTestHelperFn();
			}

			virtual std::string toString()
			{
				std::stringstream res;
				res << "This is a ml::ScreenAndClean";
				res << "numiter is: " << numiter << std::endl;
				res << "pvalueThreshold is: " << pvalueThreshold << std::endl;
				res << "Snipped from pvalues is: " << gstd::Printer::vp(gstd::vector::sub(pvalues, 0, 100, true)) << std::endl;
				res << SelectionModelTyped<datatypename, headertype>::toString();
				if (submodel.isNull())
					res << "submodel not set" << std::endl;
				else
					res << "The submodel: " << submodel.get(false)->toString();
				return res.str();
			}
		};

		template<typename datatypename, typename headertype>
		class ScreenAndCleanWithFit : public ScreenAndCleanGenericBase<datatypename, headertype> {};

		template<typename headertype>
		class ScreenAndCleanWithFit<DpRegression, headertype> : public ScreenAndCleanGenericBase<DpRegression, headertype>
		{
		public:
			void fit()
			{
				SelectionModelTyped<DpRegression, headertype>* sub = ScreenAndCleanGenericBase<DpRegression, headertype>::submodel.get(false);

				//get the data stream from the model
				bool origserverWasOwned = false;
				if (sub->client.pointerIsOwned())
					origserverWasOwned = true;
				gstd::Pointer<data::Server<DpRegression, headertype> > origserver;
				origserver.set(sub->client.get(origserverWasOwned), origserverWasOwned);
				sub->client.setnull();

				//backup
				bool cacheBackup = sub->cache;
				sub->cache = false;

				//initialize
				int mdp = sub->maxDataPoint;
				int numScreen = mdp / 2;
				int numClean = mdp - numScreen;
				std::vector<std::vector<double> > allps;
				gstd::trial<DpRegression> point = origserver.get(false)->get(0);
				gstd::check(point.success, "cannot find data");
				int P = point.result.input.size();
				LinearRegression linReg;
				DpRegressionComponentMapper<headertype> cMapper;
				std::vector<int> targets = gstd::map::keys(ScreenAndCleanGenericBase<DpRegression, headertype>::targetSelection);
				data::Converter<DpRegression, DpRegression, headertype, data::DataHeader> conv;
				conv.inServer.set(&cMapper, false);
				linReg.client.set(&conv, false);
				linReg.maxDataPoint = numClean;

				//iterate
				for (int iter = 0; iter < ScreenAndCleanGenericBase<DpRegression, headertype>::numiter; iter++)
				{
					if (ScreenAndCleanGenericBase<DpRegression, headertype>::doc)
					{
						gstd::Printer::c("starting iteration " + std::to_string(iter));
						gstd::Timer::inst.t();
					}

					std::vector<int> rperm = gstd::Rand::perm(mdp, mdp);

					//set up the mappers
					data::IndexMapper<DpRegression, headertype> mapper1;
					for (int i = 0; i < numScreen; i++)
						mapper1.indexmap.push_back(rperm[i]);
					mapper1.inServer.set(origserver.get(false), false);
					sub->client.set(&mapper1, false);
					sub->maxDataPoint = numScreen;
					data::IndexMapper<DpRegression, headertype> mapper2;
					for (int i = numScreen; i < mdp; i++)
						mapper2.indexmap.push_back(rperm[i]);
					mapper2.inServer.set(origserver.get(false), false);
					cMapper.inServer.set(&mapper2, false);

					if (ScreenAndCleanGenericBase<DpRegression, headertype>::safe)
					{
						std::map<int, bool> m1 = gstd::map::create(mapper1.indexmap);
						std::map<int, bool> m2 = gstd::map::create(mapper2.indexmap);
						int m1size = m1.size();
						int m2size = m2.size();
						gstd::check(m1size + m2size == mdp, "problem with the mapper setup");
						gstd::check((int)mapper1.indexmap.size() == m1size, "problem with the mapper setup");
						gstd::check((int)mapper2.indexmap.size() == m2size, "problem with the mapper setup");
						for (int i = 0; i < m1size; i++)
							gstd::check(m2.count(mapper1.indexmap[i]) == 0, "problem with the mapper setup");
						for (int i = 0; i < m2size; i++)
							gstd::check(m1.count(mapper2.indexmap[i]) == 0, "problem with the mapper setup");
					}

					sub->crossValidate();
					sub->fit();

					//if model is too big, we fall back on sweep
					if ((int)sub->selection.size() > numClean)
					{
						bool cacheBackup = sub->cache;
						sub->cache = false;
						sub->sweep();
						sub->selection.clear();
						std::vector<std::vector<int> > rinds = gstd::vector::rankIndeces(gstd::Linalg::mneg(sub->ranks));
						for (int i = 0; i<(int)rinds.size(); i++)
						{
							if ((int)sub->selection.size() + (int)rinds[i].size() <= numClean)
							{
								for (int j = 0; j<(int)rinds[i].size(); j++)
									sub->selection[rinds[i][j]] = true;
							}
						}
						sub->cache = cacheBackup;
						gstd::Printer::vc(gstd::map::keys(sub->selection));
					}

					gstd::check((int)sub->selection.size() <= numClean, "at this point, the model size should not exceed the number of data points");

					if (ScreenAndCleanGenericBase<DpRegression, headertype>::doc)
					{
						gstd::Printer::c("Final fit model size is " + gstd::Printer::p(sub->selection.size()));
					}

					//set up the componentMapper
					cMapper.componentMap = gstd::map::keys(sub->selection);

					linReg.fitWithOptions();

					//interpret the results
					std::vector<double> iterps(P, -1);
					for (int i = 0; i < P; i++)
					{
						if (sub->selection.count(i) == 0)
							iterps[i] = (double)P;
					}
					for (int i = 0; i < (int)cMapper.componentMap.size(); i++)
					{
						int component = cMapper.componentMap[i];
						gstd::check(iterps[component] == -1, "this iterps value should not have been set");
						iterps[component] = linReg.pvalues[i] * (int)sub->selection.size();
					}
					allps.push_back(iterps);

					//cleanup
					sub->client.setnull();

					if (ScreenAndCleanGenericBase<DpRegression, headertype>::doc)
					{
						gstd::Printer::c("finished iteration " + std::to_string(iter));
						std::vector<double> goodps = gstd::vector::getmany(iterps, targets);
						gstd::Printer::c("p-values of targets this round were : " + gstd::Printer::vp(goodps));
						gstd::Printer::c("general snippet from p-value this round is : " + gstd::Printer::vp(gstd::vector::sub(iterps, 0, 100, true)));
					}
				}

				//interpret the final results
				gstd::vector::empty(ScreenAndCleanGenericBase<DpRegression, headertype>::pvalues);
				ScreenAndCleanGenericBase<DpRegression, headertype>::pvalues.resize(P);
				for (int i = 0; i < P; i++)
				{
					std::vector<double> resps;
					for (int iter = 0; iter < ScreenAndCleanGenericBase<DpRegression, headertype>::numiter; iter++)
						resps.push_back(allps[iter][i]);
					ScreenAndCleanGenericBase<DpRegression, headertype>::pvalues[i] = gstd::stat::combineTestPs(resps);
				}

				if (ScreenAndCleanGenericBase<DpRegression, headertype>::pvalueThreshold != -1)
					ScreenAndCleanGenericBase<DpRegression, headertype>::select();

				ScreenAndCleanGenericBase<DpRegression, headertype>::ranks = gstd::vector::rank(ScreenAndCleanGenericBase<DpRegression, headertype>::pvalues, true);

				//restore
				sub->client.set(origserver.get(origserverWasOwned), origserverWasOwned);
				sub->cache = cacheBackup;
				sub->maxDataPoint = mdp;

				//print the results
				if (gstd::Base::doc)
				{
					gstd::Printer::c("targetSelection is : " + gstd::Printer::vp(targets));
					std::vector<double> goodps = gstd::vector::getmany(ScreenAndCleanGenericBase<DpRegression, headertype>::pvalues, targets);
					gstd::Printer::c("p-values of targets were : " + gstd::Printer::vp(goodps));
					gstd::Printer::c("general snippet from p-value is : " + gstd::Printer::vp(gstd::vector::sub(ScreenAndCleanGenericBase<DpRegression, headertype>::pvalues, 0, 100, true)));
					gstd::Printer::c("pvalueThreshold is : " + std::to_string(ScreenAndCleanGenericBase<DpRegression, headertype>::pvalueThreshold));
					gstd::Printer::c("selection is : " + gstd::Printer::vp(gstd::map::keys(ScreenAndCleanGenericBase<DpRegression, headertype>::selection)));
				}
			}
		};

		template<typename headertype>
		class ScreenAndCleanWithFit<DpMTRegression, headertype> : public ScreenAndCleanGenericBase<DpMTRegression, headertype>
		{
		public:
			void fit()
			{
				SelectionModelTyped<DpMTRegression, headertype>* sub = ScreenAndCleanGenericBase<DpMTRegression, headertype>::submodel.get(false);

				//get the data stream from the model
				bool origserverWasOwned = false;
				if (sub->client.pointerIsOwned())
					origserverWasOwned = true;
				gstd::Pointer<data::Server<DpMTRegression, headertype> > origserver;
				origserver.set(sub->client.get(origserverWasOwned), origserverWasOwned);
				sub->client.setnull();

				//backup
				bool cacheBackup = sub->cache;
				sub->cache = false;

				//initialize
				int mdp = sub->maxDataPoint;
				int numScreen = mdp / 2;
				int numClean = mdp - numScreen;
				std::vector<std::vector<double> > allps;
				gstd::trial<DpMTRegression> point = origserver.get(false)->get(0);
				gstd::check(point.success, "cannot find data");
				int inDim = point.result.input.size();
				int outDim = point.result.output.size();
				int P = inDim*outDim;
				LinearRegression linReg;
				DpRegressionComponentMapper<headertype> cMapper;
				std::vector<int> targets = gstd::map::keys(ScreenAndCleanGenericBase<DpMTRegression, headertype>::targetSelection);
				data::Converter<DpRegression, DpRegression, headertype, data::DataHeader> conv;
				conv.inServer.set(&cMapper, false);
				linReg.client.set(&conv, false);
				linReg.maxDataPoint = numClean;
				DpMTRegressionTaskExtractor<headertype> extr;
				cMapper.inServer.set(&extr, false);

				//iterate
				for (int iter = 0; iter < ScreenAndCleanGenericBase<DpMTRegression, headertype>::numiter; iter++)
				{
					if (ScreenAndCleanGenericBase<DpMTRegression, headertype>::doc)
					{
						gstd::Printer::c("starting iteration " + std::to_string(iter));
						gstd::Timer::inst.t();
					}

					std::vector<int> rperm = gstd::Rand::perm(mdp, mdp);

					//set up the mappers
					data::IndexMapper<DpMTRegression, headertype> mapper1;
					for (int i = 0; i < numScreen; i++)
						mapper1.indexmap.push_back(rperm[i]);
					mapper1.inServer.set(origserver.get(false), false);
					sub->client.set(&mapper1, false);
					sub->maxDataPoint = numScreen;
					data::IndexMapper<DpMTRegression, headertype> mapper2;
					for (int i = numScreen; i < mdp; i++)
						mapper2.indexmap.push_back(rperm[i]);
					mapper2.inServer.set(origserver.get(false), false);
					extr.inServer.set(&mapper2, false);

					if (ScreenAndCleanGenericBase<DpMTRegression, headertype>::safe)
					{
						std::map<int, bool> m1 = gstd::map::create(mapper1.indexmap);
						std::map<int, bool> m2 = gstd::map::create(mapper2.indexmap);
						int m1size = m1.size();
						int m2size = m2.size();
						gstd::check(m1size + m2size == mdp, "problem with the mapper setup");
						gstd::check((int)mapper1.indexmap.size() == m1size, "problem with the mapper setup");
						gstd::check((int)mapper2.indexmap.size() == m2size, "problem with the mapper setup");
						for (int i = 0; i < m1size; i++)
							gstd::check(m2.count(mapper1.indexmap[i]) == 0, "problem with the mapper setup");
						for (int i = 0; i < m2size; i++)
							gstd::check(m1.count(mapper2.indexmap[i]) == 0, "problem with the mapper setup");
					}

					sub->crossValidate();
					sub->fit();

					//split up the selections by task
					std::vector<std::map<int, bool> > taskWiseSelections(outDim);
					int totalSelected = 0;
					for (int i = 0; i < inDim; i++)
					{
						for (int t = 0; t < outDim; t++)
						{
							if (sub->selection.count(t*inDim + i) == 1)
							{
								taskWiseSelections[t][i] = true;
								totalSelected++;
							}
						}
					}
					gstd::check(totalSelected == (int)sub->selection.size(), "taskwise selection creation failed");

					//if model is too big, we fall back on sweep
					for (int t = 0; t < outDim; t++)
					{
						if ((int)taskWiseSelections[t].size() > numClean)
						{
							bool cacheBackup = sub->cache;
							sub->cache = false;
							sub->sweep();
							taskWiseSelections[t].clear();
							std::vector<double> taskRanks = gstd::vector::sub(sub->ranks, t*inDim, inDim);
							std::vector<std::vector<int> > rinds = gstd::vector::rankIndeces(gstd::Linalg::mneg(taskRanks));
							for (int i = 0; i < (int)rinds.size(); i++)
							{
								if ((int)taskWiseSelections[t].size() + (int)rinds[i].size() <= numClean)
								{
									for (int j = 0; j < (int)rinds[i].size(); j++)
										taskWiseSelections[t][rinds[i][j]] = true;
								}
							}
							sub->cache = cacheBackup;
							gstd::Printer::c("Replaced task-wise selection for task " + gstd::Printer::p(t) + " with " + gstd::Printer::vp(gstd::map::keys(taskWiseSelections[t])));
						}
					}

					for (int t = 0; t < outDim; t++)
					{
						gstd::check((int)taskWiseSelections[t].size() <= numClean, "at this point, the model size should not exceed the number of data points");
					}

					if (ScreenAndCleanGenericBase<DpMTRegression, headertype>::doc)
					{
						std::vector<int> fitSizes;
						for (int t = 0; t < outDim; t++)
							fitSizes.push_back(taskWiseSelections[t].size());
						gstd::Printer::c("Final fit model size by task are " + gstd::Printer::vp(fitSizes));
					}

					std::vector<double> iterps(P, -1);
					for (int t = 0; t < outDim; t++)
					{
						//set up the servers
						extr.taskNum = t;
						cMapper.componentMap = gstd::map::keys(taskWiseSelections[t]);

						//run
						linReg.fitWithOptions();

						//interpret the results
						for (int i = 0; i < inDim; i++)
						{
							if (taskWiseSelections[t].count(i) == 0)
							{
								gstd::check(iterps[i + t*inDim] == -1, "this iterps value should not have been set");
								iterps[i + t*inDim] = (double)P;
							}
						}
						for (int i = 0; i < (int)cMapper.componentMap.size(); i++)
						{
							int component = cMapper.componentMap[i];
							gstd::check(iterps[component + t*inDim] == -1, "this iterps value should not have been set");
							iterps[component + t*inDim] = linReg.pvalues[i] * (int)taskWiseSelections[t].size() * outDim; //adjustment
						}
					}
					allps.push_back(iterps);

					//cleanup
					sub->client.setnull();

					if (ScreenAndCleanGenericBase<DpMTRegression, headertype>::doc)
					{
						gstd::Printer::c("finished iteration " + std::to_string(iter));
						std::vector<double> goodps = gstd::vector::getmany(iterps, targets);
						gstd::Printer::c("p-values of targets this round were : " + gstd::Printer::vp(goodps));
						gstd::Printer::c("general snippet from p-value this round is : " + gstd::Printer::vp(gstd::vector::sub(iterps, 0, 100, true)));
					}
				}

				//interpret the final results
				gstd::vector::empty(ScreenAndCleanGenericBase<DpMTRegression, headertype>::pvalues);
				ScreenAndCleanGenericBase<DpMTRegression, headertype>::pvalues.resize(P);
				for (int i = 0; i < P; i++)
				{
					std::vector<double> resps;
					for (int iter = 0; iter < ScreenAndCleanGenericBase<DpMTRegression, headertype>::numiter; iter++)
						resps.push_back(allps[iter][i]);
					ScreenAndCleanGenericBase<DpMTRegression, headertype>::pvalues[i] = gstd::stat::combineTestPs(resps);
				}

				if (ScreenAndCleanGenericBase<DpMTRegression, headertype>::pvalueThreshold != -1)
					ScreenAndCleanGenericBase<DpMTRegression, headertype>::select();

				ScreenAndCleanGenericBase<DpMTRegression, headertype>::ranks = gstd::vector::rank(ScreenAndCleanGenericBase<DpMTRegression, headertype>::pvalues, true);

				//restore
				sub->client.set(origserver.get(origserverWasOwned), origserverWasOwned);
				sub->cache = cacheBackup;
				sub->maxDataPoint = mdp;

				//print the results
				if (gstd::Base::doc)
				{
					gstd::Printer::c("targetSelection is : " + gstd::Printer::vp(targets));
					std::vector<double> goodps = gstd::vector::getmany(ScreenAndCleanGenericBase<DpMTRegression, headertype>::pvalues, targets);
					gstd::Printer::c("p-values of targets were : " + gstd::Printer::vp(goodps));
					gstd::Printer::c("general snippet from p-value is : " + gstd::Printer::vp(gstd::vector::sub(ScreenAndCleanGenericBase<DpMTRegression, headertype>::pvalues, 0, 100, true)));
					gstd::Printer::c("pvalueThreshold is : " + std::to_string(ScreenAndCleanGenericBase<DpMTRegression, headertype>::pvalueThreshold));
					gstd::Printer::c("selection is : " + gstd::Printer::vp(gstd::map::keys(ScreenAndCleanGenericBase<DpMTRegression, headertype>::selection)));
				}
			}
		};

		template<typename datatypename, typename headertype>
		class ScreenAndClean : public ScreenAndCleanWithFit<datatypename, headertype> {};    	

		template<>
		class ScreenAndClean<DpRegression, data::DataHeader> : public ScreenAndCleanWithFit<DpRegression, data::DataHeader>
		{
		public:                    
			virtual void setInputs()//sets values for inputs for quick testing
			{
				numiter = 10;
				pvalueThreshold = 1;
				SelectionModelTyped<DpRegression, data::DataHeader>* _submodel = new LassoExtSlepEng;
				_submodel->setInputs();
				_submodel->numFolds = 10;
				_submodel->ytol = 1e-12;
				LambdaSweepScheduleGeo* lss = new LambdaSweepScheduleGeo;
				lss->start = 8;
				lss->factor = 0.5;
				lss->numiter = 40;
				_submodel->maxDataPoint = 300;
				LambdaSweepSchedule* old = _submodel->lambdaSweepSchedule.get(true);
				delete old;
				_submodel->lambdaSweepSchedule.set(lss, true);
				targetSelection = _submodel->targetSelection;
				target = _submodel->target;
				submodel.set(_submodel, true);
			}
		};

		template<>
		class ScreenAndClean<DpMTRegression, IOGroupHeader> : public ScreenAndCleanWithFit<DpMTRegression, IOGroupHeader>
		{
		public:
			virtual void setInputs()//sets values for inputs for quick testing
			{
				numiter = 10;
				SelectionModelTyped<DpMTRegression, IOGroupHeader>* _submodel = new SIOLassoExtSlepEng;
				_submodel->setInputs();
				_submodel->numFolds = 10;
				int T = _submodel->client.get(false)->get(0).result.output.size();
				pvalueThreshold = (double)T;
				targetSelection = _submodel->targetSelection;
				target = _submodel->target;
				submodel.set(_submodel, true);
			}
		};
	}
}

namespace msii810161816
{
	namespace gstd
	{
		template<typename datatype, typename headertype>
		struct TypeNameGetter<ml::ScreenAndCleanGenericBase<datatype, headertype> >
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::ScreenAndClean";
				t.parameters.push_back(TypeNameGetter<datatype>::get());
				t.parameters.push_back(TypeNameGetter<headertype>::get());
				return t;
			}
		};
	}
}

#endif	/* STABILITYSELECTION_H */
