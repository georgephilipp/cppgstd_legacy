#include "stdafx.h"

#pragma once
#ifndef ML_DATAHEADERS_H
#define	ML_DATAHEADERS_H

#include"standard.h"
#include"Data/src/DataHeaders.h"

namespace msii810161816
{
	namespace ml
	{
		class MSII810161816_ML_API InputGroupHeader : public data::DataHeader, public virtual  gstd::Base
		{
		public:
			virtual ~InputGroupHeader();

			std::vector<int> inputGroups;

			//Base package
		public:
			virtual gstd::TypeName getTypeName();
			virtual void setInputs();
			virtual bool test();
			virtual std::string toString();
		};

		class MSII810161816_ML_API IOGroupHeader : public data::DataHeader, public virtual  gstd::Base
		{
		public:
			virtual ~IOGroupHeader();

			std::vector<std::vector<int> > inputGroups;
			std::vector<std::vector<int> > outputGroups;

			//Base package
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
	namespace data
	{
		template<>
		struct DhConverter<ml::InputGroupHeader, ml::IOGroupHeader>
		{
			static ml::IOGroupHeader get(ml::InputGroupHeader input);
		};
	}
}


namespace msii810161816
{
	namespace gstd
	{                   
		template<>
		struct TypeNameGetter<ml::InputGroupHeader>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::InputGroupHeader";
				return t;
			}
		};

		template<>
		struct TypeNameGetter<ml::IOGroupHeader>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::IOGroupHeader";
				return t;
			}
		};
	}
}




#endif