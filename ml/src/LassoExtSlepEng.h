#include "stdafx.h"

#pragma once
#ifndef LASSOEXTSLEPENG_H
#define	LASSOEXTSLEPENG_H

#include "standard.h"
#include "Lasso.h"
#include "SlepEngBase.h"

namespace msii810161816
{
	namespace ml
	{
		class MSII810161816_ML_API LassoExtSlepEng : public Lasso, public SlepEngBase
		{
		public:
			LassoExtSlepEng();
			virtual ~LassoExtSlepEng();

			//inputs
			//everything from Lasso

			//options
			//everything from Lasso

			virtual void fit(); //fit the model
			virtual void sweep(); //run the model for various velues of lambda and obtain a combined selection / ranking
			void crossValidate() { SelectionModelTyped<DpRegression, data::DataHeader>::crossValidate(); }

			virtual void initparameter() {} //not used
		private:
			void fitInner(bool withSweep);
			static const std::string mid; //use this if matlab variables clash

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
		struct TypeNameGetter<ml::LassoExtSlepEng>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::LassoExtSlepEng";
				return t;
			}
		};
	}
}

#endif	/* LASSOEXTSLEP_H */