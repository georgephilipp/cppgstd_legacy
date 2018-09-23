#include "stdafx.h"

#pragma once
#include "standard.h"
#include "Vertex.h"

namespace msii810161816
{
	namespace dnn
	{
		class MSII810161816_DNN_API VertexSquareDiff : public Vertex, public virtual gstd::Base
		{
		public:
			//dnn::Vertex
			//std::string id;
			//std::vector<Edge*> incomingEdges;
			//std::vector<Edge*> outgoingEdges;
			//gstd::Pointer<Blob> content;
			//data::Client<data::DpTensor<float>, data::DataHeader> dataClient;
			virtual Vertex* construct();
			virtual void copy(Vertex* target, std::string copyMode, bool isTop = true);
			virtual void run();
			virtual void runGradient(Vertex* valueStoreVertex);
			//Blob* getY();
			//Blob* getX(int index);
			//Blob* getZ(int index);
			//Blob* getdY(Vertex* valueStoreVertex);
			//Blob* getdX(Vertex* valueStoreVertex, int index);
			//Blob* getdZ(Vertex* valueStoreVertex, int index);

			//data::client
			//gstd::Pointer<Server<data::DpTensor<float>, data::DataHeader> >	

			double multiplier;

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
		struct TypeNameGetter<dnn::VertexSquareDiff>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "dnn::VertexSquareDiff";
				return t;
			}
		};
	}
}