#include "stdafx.h"

#include "VertexLinear.h"
#include "Edge.h"

namespace msii810161816
{
	namespace dnn
	{
		Vertex* VertexLinear::construct()
		{
			return new VertexLinear;
		}

		void VertexLinear::copy(Vertex* target, std::string copyMode, bool isTop)
		{
			if (isTop)
				gstd::Base::copy(target);

			Vertex::copy(target, copyMode, false);
		}

		void VertexLinear::run()
		{
			int numProducts = incomingEdges.size();
			gstd::check(numProducts > 0, "cannot run linear vertex with no incoming edges");
			std::vector<gstd::Pointer<Blob> > XZstore(numProducts - 1);

			getX(0)->convolve(getZ(0), getY());

			/*parallelizable*/
			for (int i = 1; i < numProducts; i++)
			{
				XZstore[i - 1].set(getY()->construct(), true);
				Blob* XZ = XZstore[i - 1].get(false);
				getX(i)->convolve(getZ(i), XZ);
			}

			/*somewhat parallelizable*/
			for (int i = 1; i < numProducts; i++)
				getY()->add(XZstore[i - 1].get(false));
		}

		void VertexLinear::runGradient(Vertex* valueStoreVertex)
		{
			int numProducts = incomingEdges.size();

			/*parallelizable*/
			for (int i = 0; i < numProducts; i++)
			{
				/*parallelizable*/
				for (int j = 0; j < 2; j++)
				{
					if (j == 0)
					{
						Blob* X = getX(i);
						Blob* dZ = getdZ(valueStoreVertex, i);
						Blob* dY = getdY(valueStoreVertex);
						//get the transpose of X
						gstd::Pointer<Blob> Xtstore;
						Xtstore.set(X->construct(), true);
						Blob* Xt = Xtstore.get(false);
						X->copyData(Xt);
						Xt->transpose();
						//convolve X with dY
						Xt->convolve(dY, dZ);
					}
					else
					{
						Blob* dX = getdX(valueStoreVertex, i);
						Blob* Z = getZ(i);
						Blob* dY = getdY(valueStoreVertex);
						//get the transpose of Z
						gstd::Pointer<Blob> Ztstore;
						Ztstore.set(Z->construct(), true);
						Blob* Zt = Ztstore.get(false);
						Z->copyData(Zt);
						Zt->transpose();
						//deconvolve dY with Zt
						dY->deconvolve(Zt, dX);
					}
				}
			}
		}

		//Base package
		gstd::TypeName VertexLinear::getTypeName()
		{
			return gstd::TypeNameGetter<VertexLinear>::get();
		}
		void VertexLinear::setInputs()
		{

		}
		bool VertexLinear::test()
		{
			return true;
		}
		std::string VertexLinear::toString()
		{
			std::stringstream res;
			res << "This is dnn::VertexLinear " << std::endl;
			res << Vertex::toString();
			return res.str();
		}


	}
}