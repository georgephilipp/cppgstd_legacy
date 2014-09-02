/* 
 * File:   RegressionNormalizer.h
 * Author: gschoenh
 *
 * Created on December 6, 2013, 6:35 PM
 */

#include "stdafx.h"

#ifndef REGRESSIONNORMALIZER_H
#define	REGRESSIONNORMALIZER_H

#include "standard.h"
#include "Datapoints.h"
#include"Data/src/Client.h"
#include "gstd/src/Primitives.h"
#include "RegressionGenerators.h"
#include <math.h>

namespace msii810161816
{   
    namespace ml
    {
		template<typename headerType>
		class RegressionNormalizer : public data::Server<DpRegression, headerType>
        {
        public:
            RegressionNormalizer()
			{
				d = -1;
			}
			virtual ~RegressionNormalizer() {}
            
            //inputs
            bool normalizeStd;
            bool normalizeMean;
            data::Client<DpRegression,headerType> inServer;
			//means and stds:either set directly or all initMeansStds
			std::vector<double> inputStds;
			std::vector<double> inputMeans;
			double outputStd;
			double outputMean;
            void initMeansStds(int begin, int length) //call this before using server features
			{
				int cursor = 0;
				while (cursor != length)
				{
					gstd::trial<DpRegression> point = inServer.get(false)->get(cursor + begin);
					if (d == -1)
					{
						if (!point.success)
							return;
						d = (int)point.result.input.size();
					}
					if (cursor == 0)
					{
						inputStds = std::vector<double>(d);
						inputMeans = std::vector<double>(d);
						outputStd = 0;
						outputMean = 0;
					}
					if (!point.success)
						break;
					gstd::check((int)point.result.input.size() == d, "received point of incorrect input size");
					for (int i = 0; i<d; i++)
					{
						inputStds[i] += point.result.input[i] * point.result.input[i];
						inputMeans[i] += point.result.input[i];
					}
					outputStd += point.result.output*point.result.output;
					outputMean += point.result.output;
					cursor++;
				}
				for (int i = 0; i<d; i++)
				{
					inputStds[i] = sqrt(inputStds[i] / ((double)cursor));
					inputMeans[i] /= ((double)cursor);
				}
				outputStd = sqrt(outputStd / ((double)cursor));
				outputMean /= ((double)cursor);
			}

        private:
			int d;
            virtual gstd::trial<DpRegression> getinner(int index)
			{
				gstd::trial<DpRegression> point = inServer.get(false)->get(index);
				if (!point.success)
					return point;
				if (d == -1)
					d = (int)point.result.input.size();
				gstd::check((int)point.result.input.size() == d, "received point of incorrect input size");
				gstd::check((int)inputMeans.size() == d, "inputMeans of incorrect size");
				gstd::check((int)inputStds.size() == d, "inputStds of incorrect size");
				if (normalizeMean)
				{
					for (int i = 0; i<d; i++)
						point.result.input[i] -= inputMeans[i];
					point.result.output -= outputMean;
					if (normalizeStd)
					{
						for (int i = 0; i<d; i++)
							point.result.input[i] /= sqrt(inputStds[i] * inputStds[i] - inputMeans[i] * inputMeans[i]);
						point.result.output /= sqrt(outputStd*outputStd - outputMean*outputMean);
					}
				}
				else if (normalizeStd)
				{
					for (int i = 0; i<d; i++)
						point.result.input[i] /= inputStds[i];
					point.result.output /= outputStd;
				}

				return point;
			}
			virtual headerType getDataHeaderInner()
			{
				return inServer.get(false)->getDataHeader();
			}
            
        //Base Package
        public:
            virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<RegressionNormalizer<headerType> > ::get();
			}
            virtual void setInputs()
			{
				LinearLassoGenerator<headerType>* gen = new LinearLassoGenerator<headerType>;
				gen->setInputs();
				gen->storedata = true;
				d = 10;
				normalizeMean = true;
				normalizeStd = true;
				inServer.set(gen, true);
				initMeansStds(0, 1000);
			}
            virtual bool test()
			{
				RegressionNormalizer<data::DataHeader> norm;
				norm.setInputs();
				int dim = -1;
				std::vector<double> means(0);
				std::vector<double> vars(0);
				for (int n = 0; n<1000; n++)
				{
					DpRegression point = norm.get(n).result;
					if (dim == -1)
					{
						dim = point.input.size();
						means.resize(dim + 1);
						vars.resize(dim + 1);
					}
					for (int i = 0; i<dim; i++)
					{
						means[i] += point.input[i];
						vars[i] += point.input[i] * point.input[i];
					}
					means[dim] += point.output;
					vars[dim] += point.output*point.output;
				}
				for (int i = 0; i <= dim; i++)
				{
					if (!gstd::Double::equals(means[i], 0, 1e-10, false) || !gstd::Double::equals(vars[i] / 1000, 1, 1e-10))
					{
						norm.reportFailure();
						return false;
					}
				}
				DpRegression point = norm.get(0).result;
				DpRegression comparePoint = norm.inServer.get(false)->get(0).result;
				for (int i = 0; i < dim; i++)
				{
					if (!gstd::Double::equals(point.input[i] * sqrt(norm.inputStds[i] * norm.inputStds[i] - norm.inputMeans[i] * norm.inputMeans[i]) + norm.inputMeans[i], comparePoint.input[i], 1e-10))
					{
						norm.reportFailure();
						return false;
					}
				}
				if (!gstd::Double::equals(point.output*sqrt(norm.outputStd*norm.outputStd - norm.outputMean*norm.outputMean) + norm.outputMean, comparePoint.output, 1e-10))
				{
					norm.reportFailure();
					return false;
				}
				return true;
			}
            virtual std::string toString()
			{
				std::stringstream res;
				res << "This is a ml::RegressionNormalizer " << std::endl;
				res << "normalizeStd is: " << normalizeStd << std::endl;
				res << "normalizeMean is: " << normalizeMean << std::endl;
				res << "d is: " << d << std::endl;
				res << "inputStds is: " << gstd::Printer::vp(inputStds) << std::endl;
				res << "inputMeans is: " << gstd::Printer::vp(inputMeans) << std::endl;
				res << "outputStd is: " << outputStd << std::endl;
				res << "outputMean is: " << outputMean << std::endl;
				res << inServer.toString();
				res << data::Server<DpRegression, headerType>::toString();
				return res.str();
			}

        };

		template<typename headerType>
		class MTRegressionNormalizer : public data::Server<DpMTRegression, headerType>
		{
		public:
			MTRegressionNormalizer()
			{
				d = -1;
				T = -1;
			}
			virtual ~MTRegressionNormalizer() {}

			//inputs
			bool normalizeStd;
			bool normalizeMean;
			data::Client<DpMTRegression, headerType> inServer;
			//means and stds:either set directly or all initMeansStds
			std::vector<double> inputStds;
			std::vector<double> inputMeans;
			std::vector<double> outputStds;
			std::vector<double> outputMeans;
			void initMeansStds(int begin, int length) //call this before using server features
			{
				int cursor = 0;
				while (cursor != length)
				{
					gstd::trial<DpMTRegression> point = inServer.get(false)->get(cursor + begin);
					if (d == -1)
					{
						if (!point.success)
							return;
						d = (int)point.result.input.size();
					}
					if (T == -1)
					{
						if (!point.success)
							return;
						T = (int)point.result.output.size();
					}
					if (cursor == 0)
					{
						inputStds = std::vector<double>(d);
						inputMeans = std::vector<double>(d);
						outputStds = std::vector<double>(T);
						outputMeans = std::vector<double>(T);
					}
					if (!point.success)
						break;
					gstd::check((int)point.result.input.size() == d, "received point of incorrect input size");
					gstd::check((int)point.result.output.size() == T, "received point of incorrect input size");
					for (int i = 0; i<d; i++)
					{
						inputStds[i] += point.result.input[i] * point.result.input[i];
						inputMeans[i] += point.result.input[i];
					}
					for (int i = 0; i<T; i++)
					{
						outputStds[i] += point.result.output[i] * point.result.output[i];
						outputMeans[i] += point.result.output[i];
					}
					cursor++;
				}
				for (int i = 0; i<d; i++)
				{
					inputStds[i] = sqrt(inputStds[i] / ((double)cursor));
					inputMeans[i] /= ((double)cursor);
				}
				for (int i = 0; i<T; i++)
				{
					outputStds[i] = sqrt(outputStds[i] / ((double)cursor));
					outputMeans[i] /= ((double)cursor);
				}
			}

		private:
			int d;
			int T;
			virtual gstd::trial<DpMTRegression> getinner(int index)
			{
				gstd::trial<DpMTRegression> point = inServer.get(false)->get(index);
				if (!point.success)
					return point;
				if (d == -1)
					d = (int)point.result.input.size();
				if (T == -1)
					T = (int)point.result.output.size();
				gstd::check((int)point.result.input.size() == d, "received point of incorrect input size");
				gstd::check((int)inputMeans.size() == d, "inputMeans of incorrect size");
				gstd::check((int)inputStds.size() == d, "inputStds of incorrect size");
				gstd::check((int)point.result.output.size() == T, "received point of incorrect input size");
				gstd::check((int)outputMeans.size() == T, "inputMeans of incorrect size");
				gstd::check((int)outputStds.size() == T, "inputStds of incorrect size");
				if (normalizeMean)
				{
					for (int i = 0; i<d; i++)
						point.result.input[i] -= inputMeans[i];
					for (int i = 0; i<T; i++)
						point.result.output[i] -= outputMeans[i];
					if (normalizeStd)
					{
						for (int i = 0; i<d; i++)
							point.result.input[i] /= sqrt(inputStds[i] * inputStds[i] - inputMeans[i] * inputMeans[i]);
						for (int i = 0; i<T; i++)
    						point.result.output[i] /= sqrt(outputStds[i]*outputStds[i] - outputMeans[i]*outputMeans[i]);
					}
				}
				else if (normalizeStd)
				{
					for (int i = 0; i<d; i++)
						point.result.input[i] /= inputStds[i];
					for (int i = 0; i<T; i++)
    					point.result.output[i] /= outputStds[i];
				}

				return point;
			}
			virtual headerType getDataHeaderInner()
			{
				return inServer.get(false)->getDataHeader();
			}

			//Base Package
		public:
			virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<MTRegressionNormalizer<headerType> >::get();
			}
			virtual void setInputs()
			{
				LinearMTLassoGenerator<headerType>* gen = new LinearMTLassoGenerator<headerType>;
				gen->setInputs();
				gen->storedata = true;
				d = 10;
				T = 2;
				normalizeMean = true;
				normalizeStd = true;
				inServer.set(gen, true);
				initMeansStds(0, 10000);
			}
			virtual bool test()
			{
				MTRegressionNormalizer<data::DataHeader> norm;
				norm.setInputs();
				int inDim = -1;
				int outDim = -1;
				std::vector<double> inMeans;
				std::vector<double> inVars;
				std::vector<double> outMeans;
				std::vector<double> outVars;
				for (int n = 0; n<10000; n++)
				{
					DpMTRegression point = norm.get(n).result;
					if (inDim == -1)
					{
						inDim = point.input.size();
						inMeans = std::vector<double>(inDim, 0);
						inVars = std::vector<double>(inDim, 0);
					}
					if (outDim == -1)
					{
						outDim = point.output.size();
						outMeans = std::vector<double>(outDim, 0);
						outVars = std::vector<double>(outDim, 0);
					}
					for (int i = 0; i<inDim; i++)
					{
						inMeans[i] += point.input[i];
						inVars[i] += point.input[i] * point.input[i];
					}
					for (int i = 0; i<outDim; i++)
					{
						outMeans[i] += point.output[i];
						outVars[i] += point.output[i] * point.output[i];
					}
				}
				for (int i = 0; i < inDim; i++)
				{
					if (!gstd::Double::equals(inMeans[i], 0, 1e-9, false) || !gstd::Double::equals(inVars[i] / 10000, 1, 1e-9))
					{
						norm.reportFailure();
						return false;
					}
				}
				for (int i = 0; i < outDim; i++)
				{
					if (!gstd::Double::equals(outMeans[i], 0, 1e-9, false) || !gstd::Double::equals(outVars[i] / 10000, 1, 1e-9))
					{
						norm.reportFailure();
						return false;
					}
				}
				DpMTRegression point = norm.get(0).result;
				DpMTRegression comparePoint = norm.inServer.get(false)->get(0).result;
				for (int i = 0; i < inDim; i++)
				{
					if (!gstd::Double::equals(point.input[i] * sqrt(norm.inputStds[i] * norm.inputStds[i] - norm.inputMeans[i] * norm.inputMeans[i]) + norm.inputMeans[i], comparePoint.input[i], 1e-10))
					{
						norm.reportFailure();
						return false;
					}
				}
				for (int i = 0; i < outDim; i++)
				{
					if (!gstd::Double::equals(point.output[i] * sqrt(norm.outputStds[i] * norm.outputStds[i] - norm.outputMeans[i] * norm.outputMeans[i]) + norm.outputMeans[i], comparePoint.output[i], 1e-10))
					{
						norm.reportFailure();
						return false;
					}
				}
				return true;
			}
			virtual std::string toString()
			{
				std::stringstream res;
				res << "This is a ml::MTRegressionNormalizer " << std::endl;
				res << "normalizeStd is: " << normalizeStd << std::endl;
				res << "normalizeMean is: " << normalizeMean << std::endl;
				res << "d is: " << d << std::endl;
				res << "inputStds is: " << gstd::Printer::vp(inputStds) << std::endl;
				res << "inputMeans is: " << gstd::Printer::vp(inputMeans) << std::endl;
				res << "inputStds is: " << gstd::Printer::vp(outputStds) << std::endl;
				res << "inputMeans is: " << gstd::Printer::vp(outputMeans) << std::endl;
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
		struct TypeNameGetter<ml::RegressionNormalizer<headerType> >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "ml::RegressionNormalizer";
				t.parameters.push_back(TypeNameGetter<headerType>::get());
                return t;
            }
        };

		template<typename headerType>
		struct TypeNameGetter<ml::MTRegressionNormalizer<headerType> >
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::MTRegressionNormalizer";
				t.parameters.push_back(TypeNameGetter<headerType>::get());
				return t;
			}
		};
    }
}

#endif	/* REGRESSIONNORMALIZER_H */

