#include "stdafx.h"

#pragma once

#include"standard.h"
#include"Base.h"

namespace msii810161816
{
	namespace gstd
	{
		namespace file
		{
			extern int MSII810161816_GSTD_API fileOpenBufferReps;
			extern double MSII810161816_GSTD_API fileOpenBufferDelay;
			std::string MSII810161816_GSTD_API joinpaths(std::string path1, std::string path2);
			void MSII810161816_GSTD_API remove(std::vector<std::string> paths, bool enforceBefore, bool enforceAfter);

			bool MSII810161816_GSTD_API test();

		}
	}
}