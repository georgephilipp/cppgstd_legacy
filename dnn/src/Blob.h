#include "stdafx.h"

#pragma once
#include "standard.h"
#include "Data/src/Datapoints.h"

namespace msii810161816
{
	namespace dnn
	{
		class MSII810161816_DNN_API Blob : public virtual gstd::Base
		{
		public:
			std::string id;

			//construction
			virtual Blob* construct() = 0;
			virtual void copyData(Blob* target) = 0;
			virtual void copy(Blob* target, std::string copyMode, bool isTop = true); //tested in Blob2DFBasic
			
			//core functionality
			virtual bool equals(Blob* val, double margin, bool relative) = 0;
			virtual void swapData(Blob* val) = 0;
			virtual void allocate(Blob* val) = 0;

			//tensor arithmatic
			virtual void transpose() = 0;
			virtual void add(Blob* val) = 0;
			void addOrCopy(Blob* val); //tested in Blob2DFBasic
			virtual void subtract(Blob* val) = 0;
			virtual void convolve(Blob* val, Blob* product) = 0;
			virtual void deconvolve(Blob* val, Blob* quotient) = 0;
			virtual void elementwiseMultiply(Blob* val) = 0;
			virtual void elementwiseDivide(Blob* val) = 0;
			virtual void scalarMax(double val) = 0;
			virtual void scalarMultiply(double val) = 0;
			virtual void scalarAdd(double val) = 0;
			virtual void scalarPower(double val) = 0;
			virtual void exponential() = 0;
			virtual void subtractFromScalar(double val) = 0;
			virtual void distributeDatawiseSum() = 0;
			virtual void sigmoid() = 0;
			virtual double norm(double exponent) = 0;
			virtual double diffNorm(Blob* val, double exponent) = 0;
			virtual double dot(Blob* val) = 0;
			virtual double sum() = 0;

			//staging
			virtual int getN() = 0;
			virtual void resetN(int N, data::DpTensor<float> val) = 0;
			virtual void setn(int index, data::DpTensor<float> val) = 0;
			virtual void clear() = 0;
			virtual bool isEmpty() = 0;

			//initialization
			virtual void setRandGaussian() = 0;
			virtual void setContant(double val) = 0;

			//Base Package    
		public:
			virtual gstd::TypeName getTypeName() = 0;
			virtual void setInputs() = 0;
			virtual bool test() = 0;
			virtual std::string toString();

		};
	}
}

namespace msii810161816
{
	namespace gstd
	{
		template<>
		struct TypeNameGetter<dnn::Blob>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "dnn::Blob";
				return t;
			}
		};
	}
}