#include "stdafx.h"

#include "File.h"
#include "Printer.h"
#include "Reader.h"
#include "Timer.h"
#include "Writer.h"
#include "Dependencies.h"

namespace msii810161816
{
	namespace gstd
	{
		namespace file
		{
			int fileOpenBufferReps = 1;
			double fileOpenBufferDelay = 0.1;

			std::string joinpaths(std::string path1, std::string path2)
			{
				if (path2.length() > 0)
					gstd::check(path2.at(0) != '/' && path2.at(0) != '\\', "path2 does not seem to be a relative path. Please check!");
				if (path1.at(path1.length() - 1) == '/' || path1.at(path1.length() - 1) == '\\')
				{
					return path1 + path2;
				}
				else
					return path1 + "/" + path2;
			}

			void remove(std::vector<std::string> paths, bool enforceBefore, bool enforceAfter)
			{
				int size = paths.size();
				if (enforceBefore)
				{
					std::vector<bool> existsBefore = gstd::Reader::verifyExistence(paths);
					for (int i = 0; i < size;i++)
						gstd::check(existsBefore[i], "cannot remove file that does not exist : filname is " + paths[i]);
				}
				for (int i = 0; i < size; i++)
			    	::remove(paths[i].c_str());
				if (!enforceAfter)
					return;
				std::vector<bool> remains(size, true);
				for (int j = 0; j < gstd::file::fileOpenBufferReps; j++)
				{
					if (j > 0)
					{
						for (int i = 0; i < size; i++)
						{
							if (remains[i])
								::remove(paths[i].c_str());
						}
					}
					remains = gstd::Reader::verifyExistence(paths); //this call could lead to quadratic blowup in waiting time in theory, but we'll leave it until it becomes an issue
					bool removedall = true;
					for (int i = 0; i < size; i++)
					{
						if (remains[i])
							removedall = false;
					}
					if (!removedall)
						gstd::Timer::sleep(gstd::file::fileOpenBufferDelay);
					else
						return;
				}
				for (int i = 0; i < size; i++)
					gstd::check(!remains[i], "could not remove file " + paths[i]);
			}

			std::string getTestFileName()
			{
				return gstd::dependencies::cpp::getHome() + "libs/gstd/src/FileTestFile.txt";
			}

			bool test()
			{
				gstd::Printer::c("This is a test for group of functions gstd::file");

				//joinpaths
				{
					std::string start1 = "/bla/bla";
					std::string start2 = "bling";
					std::string target = "/bla/bla/bling";
					if (joinpaths(start1, start2) != target)
					{
						gstd::reportFailure("gstd::file::joinpaths, test 1");
						return false;
					}
					start1 = "/bla/bla/";
					target = "/bla/bla/bling";
					if (joinpaths(start1, start2) != target)
					{
						gstd::reportFailure("gstd::file::joinpaths, test 2");
						return false;
					}
					start1 = "/bla/bla\\";
					target = "/bla/bla\\bling";
					if (joinpaths(start1, start2) != target)
					{
						gstd::reportFailure("gstd::file::joinpaths, test 3");
						return false;
					}
					start2 = "/bling";
					try
					{
						gstd::Printer::c("Expecting error message ...");
						joinpaths(start1, start2);
						gstd::reportFailure("gstd::file::joinpaths, test 4");
						return false;
					}
					catch (std::exception e) {}
					start2 = "\\bling";
					try
					{
						gstd::Printer::c("Expecting error message ...");
						joinpaths(start1, start2);
						gstd::reportFailure("gstd::file::joinpaths, test 5");
						return false;
					}
					catch (std::exception e) {}
				}

				//remove
				{
					std::string testFileName = getTestFileName();
					try
					{
						gstd::Printer::c("Expecting error message ...");
						remove({ testFileName }, true, false);
						gstd::reportFailure("gstd::file::remove, test 1");
						return false;
					}
					catch (std::exception e) {}
					try
					{
						remove({ testFileName }, false, false);
					}
					catch (std::exception e)
					{
						gstd::reportFailure("gstd::file::remove, test 2");
						return false;
					}
					gstd::Writer::w(testFileName, "bling", false, std::ios::trunc);
					try
					{
						remove({ testFileName }, true, true);
						gstd::check(!gstd::Reader::exists(testFileName), "");
					}
					catch (std::exception e)
					{
						gstd::reportFailure("gstd::file::remove, test 3");
						return false;
					}
					gstd::Writer::w(testFileName, "bling", false, std::ios::trunc);
					try
					{
						gstd::Timer t;
						remove({ testFileName, testFileName, testFileName }, true, true);
						gstd::Printer::c(file::fileOpenBufferDelay*((double)(file::fileOpenBufferReps - 1)));
						gstd::Printer::c(file::fileOpenBufferDelay*((double)file::fileOpenBufferReps));
						t.t();
						gstd::check(t.t(false) > file::fileOpenBufferDelay*((double)(file::fileOpenBufferReps - 1)) && t.t(false) < file::fileOpenBufferDelay*((double)file::fileOpenBufferReps), "");
						gstd::check(!gstd::Reader::exists(testFileName), "");
					}
					catch (std::exception e)
					{
						gstd::reportFailure("gstd::file::remove, test 4");
						return false;
					}
				}
				return true;
			}


		}
	}
}
