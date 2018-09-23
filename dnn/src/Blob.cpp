#include "stdafx.h"

#include "Blob.h"


namespace msii810161816
{
	namespace dnn
	{
		void Blob::copy(Blob* target, std::string copyMode, bool isTop)
		{
			gstd::check(!isTop, "abstract class can't be top in the copy chain");
			target->id = id;
		}

		void Blob::addOrCopy(Blob* val)
		{
			if (isEmpty())
				val->copyData(this);
			else
				add(val);
		}

		//Base package
		std::string Blob::toString()
		{
			std::stringstream str;
			str << "This is dnn::Blob" << std::endl;
			str << "id is " << id << std::endl;
			return str.str();
		}

	}
}