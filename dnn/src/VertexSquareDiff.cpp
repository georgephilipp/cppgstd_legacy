#include "stdafx.h"

#include "VertexSquareDiff.h"
#include "Edge.h"

namespace msii810161816
{
	namespace dnn
	{
		Vertex* VertexSquareDiff::construct()
		{
			return new VertexSquareDiff;
		}

		void VertexSquareDiff::copy(Vertex* target, std::string copyMode, bool isTop)
		{
			if (isTop)
				gstd::Base::copy(target);

			Vertex::copy(target, copyMode, false);
		}

		void VertexSquareDiff::run()
		{
			gstd::check((int)incomingEdges.size() == 2, "VertexSquareL2NormDiff needs two incoming edges");

			Blob* Y = getY();
			Blob* X1 = getX(0);
			Blob* X2 = getX(1);

			X1->copyData(Y);
			Y->subtract(X2);
			Y->scalarPower(2);
			Y->scalarMultiply(multiplier);
		}

		void VertexSquareDiff::runGradient(Vertex* valueStoreVertex)
		{
			gstd::check((int)incomingEdges.size() == 2, "VertexSquareL2NormDiff needs two incoming edges");

			Blob* dX1 = getdX(valueStoreVertex, 0);
			Blob* dX2 = getdX(valueStoreVertex, 1);
			getX(0)->copyData(dX1);
			dX1->subtract(getdX(valueStoreVertex, 1));
			dX1->elementwiseMultiply(getdY(valueStoreVertex));
			dX1->scalarMultiply(2 * multiplier);
			dX1->copyData(dX2);
			dX2->subtractFromScalar(0);
		}

		//Base package
		gstd::TypeName VertexSquareDiff::getTypeName()
		{
			return gstd::TypeNameGetter<VertexSquareDiff>::get();
		}
		void VertexSquareDiff::setInputs()
		{

		}
		bool VertexSquareDiff::test()
		{
			return true;
		}
		std::string VertexSquareDiff::toString()
		{
			std::stringstream res;
			res << "This is dnn::VertexSquareDiff " << std::endl;
			res << "multiplier is " << multiplier << std::endl;
			res << Vertex::toString();
			return res.str();
		}


	}
}