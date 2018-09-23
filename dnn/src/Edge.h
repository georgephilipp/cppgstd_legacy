#include "stdafx.h"

#pragma once
#include "standard.h"
#include "Blob.h"
#include "gstd/src/Pointer.h"
#include "DNN.h"

namespace msii810161816
{
	namespace dnn
	{
		class Vertex;
		class DNN;

		class MSII810161816_DNN_API Edge : public virtual gstd::Base
		{
		public:
			std::string id;
			Vertex* parent;
			Vertex* child;
			gstd::Pointer<Blob> weightsHeavy;
			Blob* weightsLight;

			//construction
			Edge* construct();
			void copy(Edge* target, std::string copyMode);

			//core functionality
			Blob* getWeights();

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
		struct TypeNameGetter<dnn::Edge>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "dnn::Edge";
				return t;
			}
		};
	}
}