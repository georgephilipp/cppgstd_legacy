#include "stdafx.h"

#include "VertexEmpty.h"
#include "Edge.h"

namespace msii810161816
{
	namespace dnn
	{
		Vertex* VertexEmpty::construct()
		{
			return new VertexEmpty;
		}

		void VertexEmpty::copy(Vertex* target, std::string copyMode, bool isTop)
		{
			if (isTop)
				gstd::Base::copy(target);

			Vertex::copy(target, copyMode, false);
		}

		void VertexEmpty::run()
		{
			gstd::error("cannot run vertexEmpty");
		}

		void VertexEmpty::runGradient(Vertex* valueStoreVertex)
		{
			gstd::error("cannot run vertexEmpty");
		}

		//Base package
		gstd::TypeName VertexEmpty::getTypeName()
		{
			return gstd::TypeNameGetter<VertexEmpty>::get();
		}
		void VertexEmpty::setInputs()
		{

		}
		bool VertexEmpty::test()
		{
			return true;
		}
		std::string VertexEmpty::toString()
		{
			std::stringstream res;
			res << "This is dnn::VertexEmpty " << std::endl;
			res << Vertex::toString();
			return res.str();
		}


	}
}