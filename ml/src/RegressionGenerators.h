/* 
 * File:   RegressionGenerators.h
 * Author: gschoenh
 *
 * Created on December 9, 2013, 10:24 AM
 */

#include "stdafx.h"

#ifndef REGRESSIONGENERATORS_H
#define	REGRESSIONGENERATORS_H

#include "standard.h"
#include "Data/src/Datapoints.h"
#include "Datapoints.h"
#include "Data/src/Client.h"
#include "gstd/src/Map.h"
#include "gstd/src/Primitives.h"
#include "gstd/src/Stat.h"

namespace msii810161816
{   
    namespace ml
    {

		template<typename headerType>
		class LinearLassoGenerator : public data::Server<DpRegression, headerType>, virtual public gstd::Base
        {
        public:
            LinearLassoGenerator()
			{
				noiseStd = -2;
				snr = -2;
				signalVariance = -2;
				d = -1;
				useEmptyModel = false;
			}
            
            //inputs
			data::Client<data::DpDoubles, headerType> inServer;
			//targetModelSettings: either set directly or use empty model or use numNonzeros/d/initTargetModel
            std::map<int, double> targetModel;
            bool useEmptyModel; //set this manually to true when using empty target model
			int numNonzeros;
			int d;
			void initTargetModel() // canonical way to draw model coefficients
			{
				if (d == 0)
					gstd::error("dimension must be set before calling this function");
				std::vector<int> selectedComponents = gstd::Rand::perm(d, numNonzeros);
				int size = selectedComponents.size();
				for (int i = 0; i<size; i++)
					targetModel[selectedComponents[i]] = gstd::Rand::d(3);
			}
			//noiseSettings: used from top to bottom
			double noiseStd;
			double snr;
			void initSignalVariance(int begin, int length) //must be called when using snr BUT SET MODEL FIRST
			{
				signalVariance = 0;
				int cursor = 0;
				while (cursor != length)
				{
					gstd::trial<DpRegression> point = getinnerRaw(cursor + begin);
					if (!point.success)
					{
						if (cursor == 0)
							return;
						else
							break;
					}
					signalVariance += point.result.output*point.result.output;
					cursor++;
				}
				signalVariance = signalVariance / ((double)cursor);
			}
            
			//actions
            //inherited from server

        private:
            gstd::trial<DpRegression> getinnerRaw(int index)
			{
				gstd::trial<data::DpDoubles> point = inServer.get(false)->get(index);
				gstd::trial<DpRegression> result;

				if (!point.success)
				{
					result.success = false;
					return result;
				}

				if (d == -1)
					d = (int)point.result.content.size();
				if (d != (int)point.result.content.size())
					gstd::error("received data point of incorrect size");

				gstd::check(useEmptyModel || targetModel.size() > 0, "model not set");

				result.success = true;
				result.result.input = point.result.content;
				result.result.output = 0;
				typedef std::map<int, double>::iterator Iterator;
				for (Iterator iterator = targetModel.begin(); iterator != targetModel.end(); iterator++)
					result.result.output += result.result.input[iterator->first] * iterator->second;

				return result;
			}
            virtual gstd::trial<DpRegression> getinner(int index)
			{
				double _noiseStd = 0;
				if (noiseStd > -1)
					_noiseStd = noiseStd;
				else if (snr > -1)
				{
					if (signalVariance < -1)
						gstd::error("signal variance must be set for this option");
					_noiseStd = sqrt(signalVariance / snr);
				}
				else
					gstd::error("noise level not set");

				gstd::trial<DpRegression> result = getinnerRaw(index);
				if (result.success)
                                {
                                    double epsilon = gstd::stat::randnorm(4)*_noiseStd;
                                    result.result.output += epsilon;  
                                }
					
				return result;
			}
			virtual headerType getDataHeaderInner()
			{
				return inServer.get(false)->getDataHeader();
			}
            double signalVariance;


        //Base Package
        public:
            virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<LinearLassoGenerator<headerType> >::get();
			}
            virtual void setInputs()
			{
				data::DpDoubles point;
				d = 10;
				for (int i = 0; i<d; i++)
					point.content.push_back((double)i);
				data::Dataset<data::DpDoubles, headerType>* dset = new data::Dataset<data::DpDoubles, headerType>;
				for (int n = 0; n<10000; n++)
				{
					data::DpDoubles newpoint = point;
					for (int i = 0; i<d; i++)
						newpoint.content[i] += 4 * gstd::stat::randnorm(4); //100*(gstd::Rand::d(3)-0.5);
					dset->set(n, newpoint);
				}
				inServer.set(dset, true);
				numNonzeros = 3;
				initTargetModel();
				initSignalVariance(0, 10000);
				snr = 1;
			}
            virtual bool test()
			{
				LinearLassoGenerator<headerType> gen;
				gen.setInputs();
				if (gen.targetModel.size() != 3)
				{
					gen.reportFailure("1");
					return false;
				}
				typedef std::map<int, double>::iterator Iterator;
				double total = 0;
				double totalVar = 0;
				for (Iterator iter = gen.targetModel.begin(); iter != gen.targetModel.end(); iter++)
				{
					total += ((double)iter->first)*iter->second;
					totalVar += iter->second*iter->second;
				}
				double targetVar = 2 * (total*total + 16 * totalVar) - total*total;
				std::vector<double> outvals;
				for (int i = 0; i<10000; i++)
					outvals.push_back(gen.get(i).result.output);
				std::pair<double, double> meanvar = gstd::stat::meanAndVar(outvals);
				if (!gstd::Double::equals(meanvar.first, total, 0.05) || !gstd::Double::equals(meanvar.second, targetVar, 0.05))
				{
					gen.reportFailure("2");
					return false;
				}
				return true;
			}
            virtual std::string toString()
			{
				std::stringstream res;
				res << "This is a LinearLassoGenerator " << std::endl;
				res << "numNonzeros is " << numNonzeros << std::endl;
				res << "noiseStd is " << noiseStd << std::endl;
				res << "snr is " << snr << std::endl;
				res << "d is " << d << std::endl;
				res << "Snippet from targetModel is " << gstd::Printer::mapp(gstd::map::submap(targetModel, 10, true)) << std::endl;
				res << "signalVariance is " << signalVariance << std::endl;
				res << inServer.toString();
				res << data::Server<DpRegression, headerType>::toString();
				return res.str();
			}
        };

		template<typename headerType>
		class LinearMTLassoGenerator : public data::Server<DpMTRegression, headerType>, virtual public gstd::Base
		{
		public:
			LinearMTLassoGenerator()
			{
				noiseStd = -2;
				snr = -2;
				signalVariance = -2;
				d = -1;
				useEmptyModel = false;
			}

			//inputs
			data::Client<data::DpDoubles, headerType> inServer;
			//targetModelSettings: either set directly or use empty model or use numNonzeros/d/initTargetModel
			std::vector<std::map<int, double> > targetModel;
			bool useEmptyModel; //set this to true to use the empty model
			//noiseSettings: used from top to bottom
			double noiseStd;
			double snr;
			void initSignalVariance(int begin, int length) //must be called when using snr BUT AFTER SETTING MODEL
			{
				signalVariance = 0;
				int cursor = 0;
				while (cursor != length)
				{
					gstd::trial<DpMTRegression> point = getinnerRaw(cursor + begin);
					if (!point.success)
					{
						if (cursor == 0)
							return;
						else
							break;
					}
					for (int i = 0; i < (int)targetModel.size();i++)
    					signalVariance += point.result.output[i]*point.result.output[i];
					cursor++;
				}
				signalVariance = signalVariance / ((double)(cursor*(int)targetModel.size()));
			}
			

			//actions
			//inherited from server

		private:
			int d;
			gstd::trial<DpMTRegression> getinnerRaw(int index)
			{
				gstd::trial<data::DpDoubles> point = inServer.get(false)->get(index);
				gstd::trial<DpMTRegression> result;

				if (!point.success)
				{
					result.success = false;
					return result;
				}

				if (d == -1)
					d = (int)point.result.content.size();
				if (d != (int)point.result.content.size())
					gstd::error("received data point of incorrect size");

				gstd::check(useEmptyModel || targetModel.size() > 0, "model not set");

				int T = targetModel.size();
				result.success = true;
				result.result.input = point.result.content;
				result.result.output = std::vector<double>(T, 0);
				typedef std::map<int, double>::iterator Iterator;
				for (int i = 0; i < T; i++)
				{
					for (Iterator iterator = targetModel[i].begin(); iterator != targetModel[i].end(); iterator++)
						result.result.output[i] += result.result.input[iterator->first] * iterator->second;
				}			
			    return result;
			}
			virtual gstd::trial<DpMTRegression> getinner(int index)
			{
				double _noiseStd = 0;
				if (noiseStd > -1)
					_noiseStd = noiseStd;
				else if (snr > -1)
				{
					if (signalVariance < -1)
						gstd::error("signal variance must be set for this option");
					_noiseStd = sqrt(signalVariance / snr);
				}
				else
					gstd::error("noise level not set");

				gstd::trial<DpMTRegression> result = getinnerRaw(index);
				if (result.success)
				{
					for (int i = 0; i < (int)result.result.output.size(); i++)
					{
						double epsilon = gstd::stat::randnorm(4)*_noiseStd;
						result.result.output[i] += epsilon;
					}					
				}

				return result;
			}
			virtual headerType getDataHeaderInner()
			{
				return inServer.get(false)->getDataHeader();
			}
			double signalVariance;


			//Base Package
		public:
			virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<LinearMTLassoGenerator<headerType> >::get();
			}
			virtual void setInputs()
			{
				data::DpDoubles point;
				d = 10;
				for (int i = 0; i<d; i++)
					point.content.push_back((double)i);
				data::Dataset<data::DpDoubles, headerType>* dset = new data::Dataset<data::DpDoubles, headerType>;
				for (int n = 0; n<10000; n++)
				{
					data::DpDoubles newpoint = point;
					for (int i = 0; i<d; i++)
						newpoint.content[i] += 4 * gstd::stat::randnorm(4); //100*(gstd::Rand::d(3)-0.5);
					dset->set(n, newpoint);
				}
				inServer.set(dset, true);
				std::map<int, double> firstTask;
				firstTask[1] = 1;
				firstTask[3] = 0.5;
				std::map<int, double> secondTask;
				secondTask[2] = 0.25;
				secondTask[4] = 0.3;
				targetModel.push_back(firstTask);
				targetModel.push_back(secondTask);
				initSignalVariance(0, 10000);
				snr = 1;
			}
			virtual bool test()
			{
				LinearMTLassoGenerator<headerType> gen;
				gen.setInputs();
				typedef std::map<int, double>::iterator Iterator;
				int T = gen.targetModel.size();
				double signalAcrossTasks = 0;
				double meanAcrossTasks = 0;
				std::vector<double> allOutVals;
				for (int t = 0; t < T; t++)
				{
					double total = 0;
					double totalVar = 0;
					for (Iterator iter = gen.targetModel[t].begin(); iter != gen.targetModel[t].end(); iter++)
					{
						total += ((double)iter->first)*iter->second;
						totalVar += iter->second*iter->second;
					}
					signalAcrossTasks += total*total + 16 * totalVar;
					meanAcrossTasks += total;
					std::vector<double> outvals;
					for (int i = 0; i < 10000; i++)
					{
						double newVal = gen.get(i).result.output[t];
						outvals.push_back(newVal);
						allOutVals.push_back(newVal);
					}
					std::pair<double, double> meanvar = gstd::stat::meanAndVar(outvals);
					if (!gstd::Double::equals(meanvar.first, total, 0.05))
					{
						gen.reportFailure("2");
						return false;
					}
				}
				std::pair<double, double> meanvar2 = gstd::stat::meanAndVar(allOutVals);
				signalAcrossTasks /= ((double)T);
				meanAcrossTasks /= ((double)T);
				double targetVar = 2.0 * signalAcrossTasks - meanAcrossTasks*meanAcrossTasks;
				if (!gstd::Double::equals(meanvar2.second, targetVar, 0.05))
				{
					gen.reportFailure("2");
					return false;
				}
				return true;
			}
			virtual std::string toString()
			{
				std::stringstream res;
				res << "This is a LinearMTLassoGenerator " << std::endl;
				res << "noiseStd is " << noiseStd << std::endl;
				res << "snr is " << snr << std::endl;
				res << "d is " << d << std::endl;
				res << "signalVariance is " << signalVariance << std::endl;
				res << inServer.toString();
				res << data::Server<DpMTRegression, headerType>::toString();
				return res.str();
			}
		};
    
    }
}
namespace msii810161816
{
    namespace gstd
    {
        template<typename headerType>
		struct TypeNameGetter<ml::LinearLassoGenerator<headerType> >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "ml::LinearLassoGenerator";
				t.parameters.push_back(TypeNameGetter<headerType>::get());
                return t;
            }
        };

		template<typename headerType>
		struct TypeNameGetter<ml::LinearMTLassoGenerator<headerType> >
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::LinearMTLassoGenerator";
				t.parameters.push_back(TypeNameGetter<headerType>::get());
				return t;
			}
		};
    }
}
#endif	/* REGRESSIONGENERATORS_H */

