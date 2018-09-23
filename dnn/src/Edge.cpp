#include "stdafx.h"

#include "Edge.h"
#include "Vertex.h"
#include "Blob2DFDBasic.h"
#include "VertexEmpty.h"



namespace msii810161816
{
	namespace dnn
	{
		Edge* Edge::construct()
		{
			return new Edge;
		}

		void Edge::copy(Edge* target, std::string copyMode)
		{
			gstd::Base::copy(target);

			gstd::check(!weightsHeavy.isNull() && !target->weightsHeavy.isNull(), "cannot copy edge if no weight container is present");

			target->id = id;
			target->parent = parent;
			target->child = child;
			target->weightsLight = weightsLight;
			weightsHeavy.get(false)->copy(target->weightsHeavy.get(false), copyMode);
				
		}

		Blob* Edge::getWeights()
		{
			if (!weightsHeavy.isNull() && !weightsHeavy.get(false)->isEmpty())
				return weightsHeavy.get(false);

			gstd::check(weightsLight != 0 && !weightsLight->isEmpty(), "weights not found");

			return weightsLight;
		}

		//Base package
		gstd::TypeName Edge::getTypeName()
		{
			return gstd::TypeNameGetter<Edge>::get();
		}
		void Edge::setInputs()
		{
			id = "EdgeTest";
			gstd::check(!weightsHeavy.pointerIsOwned(), "cannot set inputs on an edge where the pointer is already owned");
			weightsHeavy.set(new Blob2DFDBasic, true);
			weightsHeavy.get(false)->setInputs();
		}
		bool Edge::test()
		{
			//copy, construct
			{
				VertexEmpty parent;
				VertexEmpty child;
				Blob2DFDBasic weightsLight;
				Edge edge1;
				edge1.setInputs();
				edge1.parent = &parent;
				edge1.child = &child;
				edge1.weightsLight = &weightsLight;
				gstd::Pointer<Edge> edge2;
				edge2.set(edge1.construct(), true);
				edge2.get(false)->weightsHeavy.set(edge1.weightsHeavy.get(false)->construct(), true);
				edge1.copy(edge2.get(false), "default");
				if (edge2.get(false)->id != edge1.id || edge2.get(false)->parent != edge1.parent || edge2.get(false)->child != edge1.child || edge2.get(false)->weightsLight != edge1.weightsLight)
				{
					edge2.get(false)->reportFailure("copy/construct, 1");
					return false;
				}
				if (!edge2.get(false)->weightsHeavy.get(false)->equals(edge1.weightsHeavy.get(false), 0, true))
				{
					edge2.get(false)->reportFailure("copy/construct, 2");
					return false;
				}
				Edge edge3;
				gstd::Printer::c("expecting error message ...");
				try
				{
					edge1.copy(&edge3, "default");
					edge3.reportFailure("copy/construct, 3");
					return false;
				}
				catch (std::exception e) {}
				gstd::Printer::c("expecting error message ...");
				try
				{
					edge3.copy(&edge1, "default");
					edge1.reportFailure("copy/construct, 4");
					return false;
				}
				catch (std::exception e) {}
			}

			//getWeights
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic blobLight;
				blobLight.setInputs();
				blobLight.id = "EdgeTestLight";
				Blob2DFDBasic blobHeavy;
				blobHeavy.setInputs();
				blobHeavy.id = "EdgeTestHeavy";
				Blob2DFDBasic blobEmpty;
				Edge edge;
				edge.setInputs();
				//needs nonempty weights
				edge.weightsHeavy.set(&blobEmpty, false);
				edge.weightsLight = &blobEmpty;
				gstd::Printer::c("expecting error message ...");
				try
				{
					edge.getWeights();
					edge.reportFailure("getWeights, 1");
					return false;
				}
				catch (std::exception) {}
				//finds light
				edge.weightsHeavy.set(0, false);
				edge.weightsLight = &blobLight;
				try
				{
					if (edge.getWeights()->id != "EdgeTestLight")
						gstd::error("");
				}
				catch (std::exception)
				{
					edge.reportFailure("getWeights, 2");
					return false;
				}
				//finds heavy
				edge.weightsHeavy.set(&blobHeavy, false);
				edge.weightsLight = 0;
				try
				{
					if (edge.getWeights()->id != "EdgeTestHeavy")
						gstd::error("");
				}
				catch (std::exception)
				{
					edge.reportFailure("getWeights, 3");
					return false;
				}
				//prefer heavy
				edge.weightsLight = &blobLight;
				if (edge.getWeights()->id != "EdgeTestHeavy")
				{
					edge.reportFailure("getWeights, 4");
					return false;
				}
			}

			return true;
		}
		std::string Edge::toString()
		{
			std::stringstream res;
			res << "This is dnn::Edge " << std::endl;
			res << "id is " << std::endl;
			if (weightsHeavy.isNull())
				res << "weightsHeavy not set" << std::endl;
			else
				res << weightsHeavy.get(false)->toString();
			if (parent == 0)
				res << "parent not set" << std::endl;
			else
				res << "parentId is" << parent->id << std::endl;
			if (child == 0)
				res << "child not set" << std::endl;
			else
				res << "childId is" << child->id << std::endl;
			if (weightsLight == 0)
				res << "weightsLight not set" << std::endl;
			else
				res << "weightsLightId is" << weightsLight->id << std::endl;

			return res.str();
		}


	}
}