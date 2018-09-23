#include "stdafx.h"

#include "VertexSigmoid.h"
#include "Edge.h"

namespace msii810161816
{
	namespace dnn
	{
		Vertex* VertexSigmoid::construct()
		{
			return new VertexSigmoid;
		}

		void VertexSigmoid::copy(Vertex* target, std::string copyMode, bool isTop)
		{
			if (isTop)
				gstd::Base::copy(target);

			Vertex::copy(target, copyMode, false);
		}

		void VertexSigmoid::run()
		{
			gstd::check((int)incomingEdges.size() == 1, "cannot run sigmoid vertex with no incoming edges");
			
			getX(0)->copyData(getY());
			getY()->sigmoid();
		}

		void VertexSigmoid::runGradient(Vertex* valueStoreVertex)
		{
			gstd::check((int)incomingEdges.size() == 1, "cannot run sigmoid vertex with no incoming edges");

			Blob* dX = getdX(valueStoreVertex, 0);
			Blob* Y = getY();
			Y->copyData(dX);
			gstd::Pointer<Blob> OmYstore;
			OmYstore.set(Y->construct(), true);
			Blob* OmY = OmYstore.get(false);
			Y->copyData(OmY);
			OmY->subtractFromScalar(1);
			dX->elementwiseMultiply(OmY);
			dX->elementwiseMultiply(getdY(valueStoreVertex));
		}

		//Base package
		gstd::TypeName VertexSigmoid::getTypeName()
		{
			return gstd::TypeNameGetter<VertexSigmoid>::get();
		}
		void VertexSigmoid::setInputs()
		{

		}
		bool VertexSigmoid::test()
		{
			return true;
		}
		std::string VertexSigmoid::toString()
		{
			std::stringstream res;
			res << "This is dnn::VertexSigmoid " << std::endl;
			res << Vertex::toString();
			return res.str();
		}


	}
}