#include "stdafx.h"

#pragma once
#include "standard.h"
#include "Data/src/Client.h"
#include "Blob.h"

namespace msii810161816
{
	namespace dnn
	{
		class Edge;

		class MSII810161816_DNN_API Vertex : public virtual gstd::Base
		{
		public:
			std::string id;
			std::vector<Edge*> incomingEdges;
			std::vector<Edge*> outgoingEdges;
			gstd::Pointer<Blob> content;
			data::Client<data::DpTensor<float>, data::DataHeader> dataClient;
			
			virtual Vertex* construct() = 0;
			virtual void copy(Vertex* target, std::string copyMode, bool isTop = true);
			virtual void run() = 0;
			virtual void runGradient(Vertex* valueStoreVertex) = 0;

		protected:
			//helper functions
			Blob* getY();
			Blob* getX(int index);
			Blob* getZ(int index);
			Blob* getdY(Vertex* valueStoreVertex);
			Blob* getdX(Vertex* valueStoreVertex, int index);
			Blob* getdZ(Vertex* valueStoreVertex, int index);
			bool numericalGradientTest(Vertex* valueStoreVertex, double smallFactor = 1e-3, double margin = 1e-2); //tested in VertexLinear

		//data::client
		//gstd::Pointer<Server<data::DpTensor<float>, data::DataHeader> >	

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
		struct TypeNameGetter<dnn::Vertex>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "dnn::Vertex";
				return t;
			}
		};
	}
}