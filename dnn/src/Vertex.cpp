#include "stdafx.h"

#include "Vertex.h"
#include "Edge.h"
#include "gstd/src/Primitives.h"



namespace msii810161816
{
	namespace dnn
	{
		void Vertex::copy(Vertex* target, std::string copyMode, bool isTop)
		{
			gstd::check(!isTop, "abstract class cannot be top in the copy chain");
			
			gstd::check(!content.isNull() && !target->content.isNull(), "cannot copy Vertex if no weight container is present");
			gstd::check(!target->dataClient.pointerIsOwned(), "cannot copy into Vertex that has an owned data client");

			target->id = id;
			target->outgoingEdges = outgoingEdges;
			target->incomingEdges = incomingEdges;
			content.get(false)->copy(target->content.get(false), copyMode);
			target->dataClient.set(dataClient.get(false, false), false);
		}

		Blob* Vertex::getY()
		{
			return content.get(false);
		}

		Blob* Vertex::getZ(int index)
		{
			return incomingEdges[index]->getWeights();
		}

		Blob* Vertex::getX(int index)
		{
			return incomingEdges[index]->parent->content.get(false);
		}

		Blob* Vertex::getdY(Vertex* valueStoreVertex)
		{
			return valueStoreVertex->content.get(false);
		}

		Blob* Vertex::getdZ(Vertex* valueStoreVertex, int index)
		{
			return valueStoreVertex->incomingEdges[index]->weightsHeavy.get(false);
		}

		Blob* Vertex::getdX(Vertex* valueStoreVertex, int index)
		{
			return valueStoreVertex->incomingEdges[index]->parent->content.get(false);
		}

		bool Vertex::numericalGradientTest(Vertex* valueStoreVertex, double smallFactor = 1e-3, double margin = 1e-2)
		{
			Blob* Y = getY();
			Blob* dY = getdY(valueStoreVertex);

			run();

			dY->allocate(Y);
			dY->setContant(1);

			runGradient(valueStoreVertex);

			int numIncomingEdges = incomingEdges.size();
			std::vector<Blob*> Zs;
			std::vector<Blob*> Xs;
			std::vector<Blob*> dZs;
			std::vector<Blob*> dXs;
			for (int i = 0; i < numIncomingEdges; i++)
			{
				Blob* temp = getdZ(valueStoreVertex, i);
				if (!temp->isEmpty())
				{
					dZs.push_back(temp);
					Zs.push_back(getZ(i));
				}
				temp = getdX(valueStoreVertex, i);
				if (!temp->isEmpty())
				{
					dXs.push_back(temp);
					Xs.push_back(getX(i));
				}
			}
			int numXs = Xs.size();
			int numZs = Zs.size();
			std::vector<gstd::Pointer<Blob> > ZsmallsStore(numZs);
			std::vector<Blob*> Zsmalls(numZs);
			std::vector<gstd::Pointer<Blob> > XsmallsStore(numXs);
			std::vector<Blob*> Xsmalls(numXs);
			gstd::Pointer<Blob> YsmallStore;
			Blob* Ysmall;
			for (int i = 0; i < numZs; i++)
			{
				ZsmallsStore[i].set(Zs[i]->construct(), true);
				Zsmalls[i] = ZsmallsStore[i].get(false);
			}
			for (int i = 0; i < numXs; i++)
			{
				XsmallsStore[i].set(Xs[i]->construct(), true);
				Xsmalls[i] = XsmallsStore[i].get(false);
			}
			YsmallStore.set(Y->construct(), true);
			Ysmall = YsmallStore.get(false);

			std::function<bool()> inner = [&]()
			{
				double exact = 0;
				for (int i = 0; i < numZs; i++)
				{
					Zsmalls[i]->scalarMultiply(smallFactor);
					exact += Zsmalls[i]->dot(dZs[i]);
					Zsmalls[i]->add(Zs[i]);
					Zs[i]->swapData(Zsmalls[i]);
				}
				for (int i = 0; i < numXs; i++)
				{
					Xsmalls[i]->scalarMultiply(smallFactor);
					exact += Xsmalls[i]->dot(dXs[i]);
					Xsmalls[i]->add(Xs[i]);
					Xs[i]->swapData(Xsmalls[i]);
				}
				Y->swapData(Ysmall);

				run();

				for (int i = 0; i < numZs; i++)
					Zs[i]->swapData(Zsmalls[i]);
				for (int i = 0; i < numXs; i++)
					Xs[i]->swapData(Xsmalls[i]);
				Y->swapData(Ysmall);

				Ysmall->subtract(Y);
				double numerical = Ysmall->dot(dY);

				if (!gstd::Double::equals(exact, numerical, margin, true))
					return false;
				else
					return true;
			};

			//direction of gradient
			for (int i = 0; i < numZs; i++)
				dZs[i]->copyData(Zsmalls[i]);
			for (int i = 0; i < numXs; i++)
				dXs[i]->copyData(Xsmalls[i]);

			if (!inner())
				return false;

			//random direction
			for (int i = 0; i < numZs; i++)
			{
				Zsmalls[i]->allocate(Zs[i]);
				Zsmalls[i]->setRandGaussian();
				Zsmalls[i]->scalarMultiply(Zs[i]->norm(2) / Zsmalls[i]->norm(2));
			}
			for (int i = 0; i < numXs; i++)
			{
				Xsmalls[i]->allocate(Xs[i]);
				Xsmalls[i]->setRandGaussian();
				Xsmalls[i]->scalarMultiply(Xs[i]->norm(2) / Xsmalls[i]->norm(2));
			}

			if (!inner())
				return false;
		}

		//Base package
		std::string Vertex::toString()
		{
			std::stringstream res;
			res << "This is dnn::Vertex " << std::endl;
			return res.str();
		}


	}
}