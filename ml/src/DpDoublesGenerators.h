/* 
 * File:   DpDoublesGenerators.h
 * Author: gschoenh
 *
 * Created on December 6, 2013, 6:00 PM
 */

#include "stdafx.h"

#ifndef DPDOUBLESGENERATORS_H
#define	DPDOUBLESGENERATORS_H

#include"standard.h"
#include"Data/src/Datapoints.h"
#include"Data/src/Server.h"
#include"Data/src/DataHeaders.h"
#include"gstd/src/Linalg.h"
#include"gstd/src/Stat.h"
#include "gstd/src/Primitives.h"
#include <math.h>

namespace msii810161816
{   
    namespace ml
    {
		template<typename headerType>
		class FactorModelGenerator : public data::Server<data::DpDoubles, headerType>, virtual public gstd::Base
        {
        public:
			//inputs
            int numFactors;
            int d;
			std::vector<double> factors; //numFactors*d matrix
            double noiseStd;

			//input configs
			void setInputsRand()
			{
				gstd::vector::empty(factors);
				for (int i = 0; i < numFactors*d; i++)
					factors.push_back(gstd::stat::randnorm(4));
			}
            
        private:
			gstd::trial<data::DpDoubles> getinner(int index)
			{
				std::vector<double> transitions;
				for (int i = 0; i < numFactors; i++)
					transitions.push_back(gstd::stat::randnorm(4));
				gstd::trial<data::DpDoubles> res;
                res.result.content = gstd::Linalg::mmult(1, numFactors, d, transitions, factors);
				for (int i = 0; i < d; i++)
					res.result.content[i] += gstd::stat::randnorm(4)*noiseStd;
				res.success = true;
                                (void) index;				
				return res;
			}

			//Base package
		public:
			gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<FactorModelGenerator<headerType> >::get();
			}
			void setInputs()
			{
				d = 2000;
				numFactors = 4;
				setInputsRand();
				noiseStd = 2;
			}
			bool test()
			{
				FactorModelGenerator<data::DataHeader> gen;
				gen.setInputs();
				for (int i = 0; i < gen.numFactors; i++)
				{
					std::vector<double> factor = gstd::Linalg::submatrix(gen.numFactors, gen.d, gen.factors, i, 0, 1, gen.d);
					if (gstd::stat::meanPval(0, factor) < 1e-5 || gstd::stat::varPval(1, factor) < 1e-5)
					{
						gen.reportFailure("factors");
						return false;
					}
				}
				std::vector<std::vector<double> > data(gen.d);
				for (int i = 0; i < 10000; i++)
				{
					gstd::trial<data::DpDoubles> point = gen.get(0);
					if (!point.success || (int)point.result.content.size() != gen.d)
					{
						gen.reportFailure("getting data");
						return false;
					}
					for (int j = 0; j < gen.d; j++)
						data[j].push_back(point.result.content[j]);
				}
				for (int i = 0; i < gen.d; i++)
				{
					if (gstd::stat::meanPval(0, data[i]) < 1/((double)gen.d)/20)
					{
						gen.reportFailure("mean of data");
						return false;
					}
					double targetVar = gen.noiseStd*gen.noiseStd;
					for (int j = 0; j < gen.numFactors; j++)
						targetVar += gen.factors[j*gen.d + i]*gen.factors[j*gen.d + i];
					if (gstd::stat::varPval(targetVar, data[i]) < 1/((double)gen.d) / 20)
					{
						gen.reportFailure("var of data");
						return false;
					}
				}
				return true;
			}
			std::string toString()
			{
				std::stringstream res;
				res << "This is a ml::FactorModelGenerator" << std::endl;
				res << "numFactors is : " << numFactors << std::endl;
				res << "d is : " << d << std::endl;
				if (d == 0 || numFactors == 0)
					res << "Factors not set" << std::endl;
				else if (d*numFactors != (int)factors.size())
					res << "Factors have incorrect size" << std::endl;
				else
					res << "Snippet from factor-matrix is : " << gstd::Printer::mp(gstd::Linalg::submatrix(numFactors, d, factors, 0, 0, 2, 100, true));
				return res.str();
			}
        };
        
		template<typename headerType>
		class GaussianGenerator : public data::Server<data::DpDoubles, headerType>, virtual public gstd::Base
        {
        public:
			//inputs
            std::vector<double> covarianceMatrix;
            void initCholesky() //call this when replacing the covarianceMatrix
			{
				int size = covarianceMatrix.size();
				d = (int)sqrt((double)size);
				gstd::check(d*d == size, "covarianceMatrix is not of square size");
				cholesky = gstd::Linalg::cholesky(covarianceMatrix);
			}

			//actions
			//as inherited from server

			//input configs
			void setInputsBlockwise(int groupSize, int numGroups, double var, double inCov, double outCov)
			{
				int dim = groupSize*numGroups;
				covarianceMatrix.resize(dim*dim);
				//set diagonal elements
				for (int i = 0; i < dim; i++)
				{
					covarianceMatrix[i*dim + i] = var;
				}
				//set block elements
				for (int i = 0; i < numGroups; i++)
				{
					for (int j = 0; j < groupSize; j++)
					{
						for (int k = 0; k < groupSize; k++)
						{
							if (j != k)
								covarianceMatrix[(i*groupSize + j)*dim + i*groupSize + k] = inCov;
						}
					}
				}
				//set out-of-block elements
				for (int i = 0; i < numGroups; i++)
				{
					for (int j = 0; j < groupSize; j++)
					{
						for (int k = 0; k < groupSize; k++)
						{
							for (int l = 0; l < numGroups; l++)
							{
								if (i != l)
									covarianceMatrix[(i*groupSize + j)*dim + l*groupSize + k] = outCov;
							}
						}
					}
				}
			}
			void setInputsToeplitz(int dim, double decay)
			{
				covarianceMatrix.resize(dim*dim);
				for (int i = 0; i < dim; i++)
				{
					for (int j = 0; j < dim; j++)
					{
						covarianceMatrix[i*dim + j] = pow(decay, gstd::Double::abs((double)(i - j)));
					}
				}
			}
            
        private:
            gstd::trial<data::DpDoubles> getinner(int index)
			{
				if (cholesky.size() == 0)
					initCholesky();
				gstd::trial<data::DpDoubles> result;
				result.success = true;
				result.result.content.resize(d);
				for (int i = 0; i<d; i++)
					result.result.content[i] = gstd::stat::randnorm(4);
				result.result.content = gstd::Linalg::mmult(1, d, d, result.result.content, cholesky);
				(void)index;
				return result;
			}
            std::vector<double> cholesky;
			int d;
            
        //Base package
        public:
            virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<GaussianGenerator<headerType> >::get();
			}
            virtual void setInputs()
			{
				setInputsBlockwise(3, 2, 1, 0.5, 0);
			}
            virtual bool test()
			{
				GaussianGenerator<data::DataHeader> gen;
				gen.setInputs();
				//check setInputsBlockwise
				std::vector<double> target = { 1, 0.5, 0.5, 0, 0, 0, 0.5, 1, 0.5, 0, 0, 0, 0.5, 0.5, 1, 0, 0, 0, 0, 0, 0, 1, 0.5, 0.5, 0, 0, 0, 0.5, 1, 0.5, 0, 0, 0, 0.5, 0.5, 1 };
				if (gen.covarianceMatrix != target)
				{
					gen.reportFailure();
					return false;
				}
				//check setInputsToeplitz
				GaussianGenerator<data::DataHeader> gen2;
				gen2.setInputsToeplitz(4, 0.5);
				target = { 1, 0.5, 0.25, 0.125, 0.5, 1, 0.5, 0.25, 0.25, 0.5, 1, 0.5, 0.125, 0.25, 0.5, 1 };
				if (!gstd::Linalg::mequals(gen2.covarianceMatrix, target, 1e-14))
				{
					gen2.reportFailure();
					return false;
				}
				//check server behavior
				int N = 20000;
				std::vector<double> datamatrix(0);
				int dim = -1;
				for (int i = 0; i<N; i++)
				{
					data::DpDoubles point = gen.get(i).result;
					datamatrix.insert(datamatrix.end(), point.content.begin(), point.content.end());
					if (dim == -1)
						dim = point.content.size();
				}
				std::vector<double> cov = gstd::stat::covarianceMatrix(N, dim, datamatrix);
				if (!gstd::Linalg::mequals(cov, gen.covarianceMatrix, 0.2, false))
				{
					gen.reportFailure();
					return false;
				}
				return true;
			}
            virtual std::string toString()
			{
				std::stringstream res;
				res << "This is a ml::GaussianGenerator " << std::endl;
				res << "dimension is " << d << std::endl;
				if (covarianceMatrix.size() > 0)
					res << "Snippet from covariance matrix is " << gstd::Printer::mp(gstd::Linalg::submatrix(d, d, covarianceMatrix, 0, 0, 10, 10, true));
				else
					res << "Covariance matrix not set" << std::endl;
				res << data::Server<data::DpDoubles, headerType>::toString();
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
		struct TypeNameGetter<ml::FactorModelGenerator<headerType> >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "ml::FactorModelGenerator";
				t.parameters.push_back(TypeNameGetter<headerType>::get());
                return t;
            }
        };
        
        template<typename headerType>
        struct TypeNameGetter<ml::GaussianGenerator<headerType> >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "ml::GaussianGenerator";
				t.parameters.push_back(TypeNameGetter<headerType>::get());
                return t;
            }
        };     
    }
}
#endif	/* DPDOUBLESGENERATORS_H */

