#include "stdafx.h"

#pragma once
#include "standard.h"
#include "Blob.h"

namespace msii810161816
{
	namespace dnn
	{
		class MSII810161816_DNN_API Blob2DFDBasic : public virtual gstd::Base, public Blob
		{
		public:
			Blob2DFDBasic();
			~Blob2DFDBasic();

			//dnn::Blob
			//std::string id;
			//construction
			virtual Blob* construct();
			virtual void copyData(Blob* target);
			virtual void copy(Blob* target, std::string copyMode, bool isTop = true);
			//core functionality
			virtual bool equals(Blob* val, double margin, bool relative);
			virtual void swapData(Blob* val);
			virtual void allocate(Blob* val);
			//tensor arithmatic
			virtual void transpose();
			virtual void add(Blob* val);
			//void addOrCopy(Blob* val);
			virtual void subtract(Blob* val);
			virtual void convolve(Blob* val, Blob* product);
			virtual void deconvolve(Blob* val, Blob* quotient);
			virtual void elementwiseMultiply(Blob* val);
			virtual void elementwiseDivide(Blob* val);
			virtual void scalarMax(double val);
			virtual void scalarMultiply(double val);
			virtual void scalarAdd(double val);
			virtual void scalarPower(double val);
			virtual void exponential();
			virtual void subtractFromScalar(double val);
			virtual void distributeDatawiseSum();
			virtual void sigmoid();
			virtual double norm(double exponent);
			virtual double diffNorm(Blob* val, double exponent);
			virtual double dot(Blob* val);
			virtual double sum();
			//staging
			virtual int getN();
			virtual void resetN(int N, data::DpTensor<float> val);
			virtual void setn(int index, data::DpTensor<float> val);
			virtual void clear();
			virtual bool isEmpty();
			//initialization
			virtual void setRandGaussian();
			virtual void setContant(double val);

			void reset(int _I, int _J, int _dim1, int _dim2);
			int getI();
			int getJ();
			int getD1();
			int getD2();
			float get(int i, int j, int d1, int d2);
			void set(float val, int i, int j, int d1, int d2);
			std::vector<float> getAll();
			void setAll(std::vector<float> val);

		private:
			int I;
			int J;
			int D1;
			int D2;

			std::vector<float> content;

			//Base Package    
		public:
			virtual gstd::TypeName getTypeName();
			virtual void setInputs();
			virtual bool test();
			virtual std::string toString();
		};
	}
}

namespace msii810161816
{
	namespace gstd
	{
		template<>
		struct TypeNameGetter<dnn::Blob2DFDBasic>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "dnn::Blob2DFDBasic";
				return t;
			}
		};
	}
}