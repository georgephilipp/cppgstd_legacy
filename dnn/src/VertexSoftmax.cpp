#include "stdafx.h"

#include "VertexSoftmax.h"
#include "Edge.h"

namespace msii810161816
{
	namespace dnn
	{
		Vertex* VertexSoftmax::construct()
		{
			return new VertexSoftmax;
		}

		void VertexSoftmax::copy(Vertex* target, std::string copyMode, bool isTop)
		{
			if (isTop)
				gstd::Base::copy(target);

			Vertex::copy(target, copyMode, false);
		}

		void VertexSoftmax::run()
		{
			gstd::check((int)incomingEdges.size() == 1, "vertexSoftmax must have one incoming edge");
			Blob* Y = getY();
			Blob* X = getX(0);
			X->copyData(Y);
			Y->exponential();
			gstd::Pointer<Blob> YsumStore;
			YsumStore.set(Y->construct(), true);
			Blob* Ysum = YsumStore.get(false);
			Y->copyData(Ysum);
			Ysum->distributeDatawiseSum();
			Y->elementwiseDivide(Ysum);
		}

		void VertexSoftmax::runGradient(Vertex* valueStoreVertex)
		{
			gstd::check((int)incomingEdges.size() == 1, "vertexSoftmax must have one incoming edge");
			Blob* dY = getdY(valueStoreVertex);
			Blob* dX = getdX(valueStoreVertex, 0);
			Blob* Y = getY();

			gstd::Pointer<Blob> dYYStore;
			dYYStore.set(Y->construct(), true);
			Blob* dYY = dYYStore.get(false);
			Y->copyData(dYY);
			dYY->elementwiseMultiply(dY);

			gstd::Pointer<Blob> dYYsStore;
			dYYsStore.set(dYY->construct(), true);
			Blob* dYYs = dYYsStore.get(false);
			dYY->copyData(dYYs);
			dYYs->distributeDatawiseSum();

			gstd::Pointer<Blob> dYYsYStore;
			dYYsYStore.set(dYYs->construct(), true);
			Blob* dYYsY = dYYsYStore.get(false);
			dYYs->copyData(dYYsY);
			dYYsY->elementwiseMultiply(Y);

			dYY->copyData(dX);
			dX->subtract(dYYsY);
		}

		//Base package
		gstd::TypeName VertexSoftmax::getTypeName()
		{
			return gstd::TypeNameGetter<VertexSoftmax>::get();
		}
		void VertexSoftmax::setInputs()
		{

		}
		bool VertexSoftmax::test()
		{
			return true;
		}
		std::string VertexSoftmax::toString()
		{
			std::stringstream res;
			res << "This is dnn::VertexSoftmax " << std::endl;
			res << Vertex::toString();
			return res.str();
		}


	}
}