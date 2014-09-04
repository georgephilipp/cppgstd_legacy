/* 
 * File:   Reader.cpp
 * Author: gschoenh
 * 
 * Created on November 22, 2013, 6:58 PM
 */

#include "stdafx.h"

#include "Reader.h"
#include "ex.h"
#include "Printer.h"
#include "Parser.h"
#include "Timer.h"
#include "File.h"
#include "Dependencies.h"

namespace msii810161816
{   
    namespace gstd
    {       
        Reader::Reader()
        {
			position = -1;
        }

		Reader::~Reader()
		{
			if (position >= 0)
				close();
		}

		bool Reader::openInner()
		{
			file.open(location);
			if (file.fail())
			{
				file.clear();
				return false;
			}
			return true;
		}
        
        void Reader::open()
        {
			gstd::check(!fileIsOpen(), "cannot open file that is already open");
			for (int i = 0; i < gstd::file::fileOpenBufferReps; i++)
			{
				if (openInner())
				{
					position = 0;
					return;
				}
				else
					gstd::Timer::sleep(gstd::file::fileOpenBufferDelay);
			}
			gstd::error("Reader failed to open file " + location);            
        }
        
        void Reader::close()
        {
			gstd::check(fileIsOpen(), "cannot close file that is not open");
            file.close();
			if (file.fail())
			{
				file.clear();
				gstd::error("Reader failed to close file " + location);
			}
            position = -1;
        }

		bool Reader::fileIsOpen()
		{
			return (position >= 0);
		}

		bool Reader::fileExists()
		{
			file.open(location);
			if (file.fail())
			{
				file.clear();
				return false;
			}
			else
			{
				file.close();
				return true;
			}
		}
        
        gstd::trial<std::string> Reader::line( int index /*=-1*/ )
        {
            std::string line;
            
            if(position == -1)
                open();
            
            if(index < -1)
                gstd::error("invalid file line index");
            else if(index == -1)
                index = position;    
            
            if(index < position)
            {
                close();
                open();
            }
            
            while(index > position)
            {
                std::getline(file, line);
                position++;
            }
            
            gstd::trial<std::string> res;
			std::getline(file, line);        
			if (file.eof() && line == "")
			{
				file.clear();
				res.success = false;
			} 
            else
            {
                position++;              
                res.success = true;
                res.result = line;
            }
            
            return res;
        }     
        
		bool Reader::exists(std::string location)
		{
			Reader reader;
			reader.location = location;
			return reader.fileExists();
		}

		std::vector<bool> Reader::verifyExistence(std::vector<std::string> locations)
		{
			int size = locations.size();
			std::vector<Reader> readers(size);
			for (int i = 0; i < size; i++)
			{
				readers[i].location = locations[i];
			}
			std::vector<double> probabilities(size, 0);
			double increment = 1 / (double)file::fileOpenBufferReps;
			std::vector<bool> res(size, false);
			for (int j = 0; j < file::fileOpenBufferReps; j++)
			{
				for (int i = 0; i < size; i++)
				{
					if (readers[i].fileExists())
						probabilities[i] += increment;
				}
				if (j + 1 != file::fileOpenBufferReps)
					gstd::Timer::sleep(file::fileOpenBufferDelay);					
			}
			for (int i = 0; i < size; i++)
			{
				if (probabilities[i] < 0.5)
					res[i] = false;
				else
					res[i] = true;
			}
			return res;
		}
                
                int Reader::numRows(std::string location)
                {
                    Reader r;
                    r.location = location;
                    int res = 0;
                    while(1)
                    {
                        gstd::trial<std::string> l = r.line();
                        if(!l.success)
                            break;
                        res++;
                    }
                    return res;
                }

		std::vector<std::string> Reader::ls(std::string location)
		{
			Reader reader;
			reader.location = location;
			reader.open();
			std::vector<std::string> res;
			while (1)
			{
				gstd::trial<std::string> next = reader.line();
				if (!next.success)
					break;
				else
					res.push_back(next.result);
			}
			return res;
		}

        //Base Package
        TypeName Reader::getTypeName()
        {
            return gstd::TypeNameGetter<Reader>::get();
        }
		std::string Reader::getTestFileName()
		{
			return dependencies::cpp::getHome() + "libs/gstd/src/ReaderTestFile.txt";
		}

        void Reader::setInputs()
        {
			location = getTestFileName();
			delimiter = ',';
			open();
        }
        bool Reader::test()
        {
            Reader r;
            r.setInputs();
			std::string testFileName = r.location;
			if (!r.fileIsOpen())
			{
				r.reportFailure("Failed test 1");
				return false;
			}
			try
			{
				gstd::Printer::c("Expecting error message ...");
				r.open();
				r.reportFailure("Failed test 2");
				return false;
			}
			catch (std::exception e) {}
			r.close();
			if (r.fileIsOpen())
			{
				r.reportFailure("Failed test 3");
				return false;
			}
			try
			{
				gstd::Printer::c("Expecting error message ...");
				r.close();
				r.reportFailure("Failed test 4");
				return false;
			}
			catch (std::exception e) {}
			if (!r.fileExists())
			{
				r.reportFailure("Failed Test 5");
				return false;
			}
            r.line(1);
			if (r.position != 2)
			{
				r.reportFailure("Failed Test 6");
				return false;
			}
            if(r.line(0).result != "1,2,3" || r.position != 1)
            {
				r.reportFailure("Failed Test 7");
                return false;
            }
            r.close();
            r.open();
            r.close();
            if(r.line(1).result != "blabla,bla")
            {
				r.reportFailure("Failed Test 8");
                return false;
            }
			r.location = "This is not a valid location";
			if (r.fileExists())
			{
				r.reportFailure("Failed Test 9");
				return false;
			}
			if (!Reader::exists(testFileName))
			{
				r.reportFailure("Failed Test 10");
				return false;
			}
			if (Reader::exists("This is not a valid location"))
			{
				r.reportFailure("Failed Test 11");
				return false;
			}
			Timer t;
			std::vector<bool> res = Reader::verifyExistence({ testFileName, "This is not a valid location" });
			if (res != std::vector<bool>({ true, false }) || t.t(false) < file::fileOpenBufferDelay*((double)(file::fileOpenBufferReps - 1)) || t.t(false) > file::fileOpenBufferDelay*((double)file::fileOpenBufferReps))
			{
				r.reportFailure("Failed Test 12");
				return false;
			}
			std::vector<std::string> target({ "1,2,3", "blabla,bla" });
			std::vector<std::string> out = Reader::ls(testFileName);
			if (out != target)
			{
				r.reportFailure("Failed Test 13");
				return false;
			}
			std::vector<std::vector<std::string> > target2({ { "1", "2", "3" }, { "blabla", "bla" } });
			std::vector<std::vector<std::string> > out2 = Reader::rs<std::string>(testFileName, ',');
			if (target2 != out2)
			{
				r.reportFailure("Failed Test 14");
				return false;
			}
                        if(numRows(testFileName) != 2)
                        {
                                r.reportFailure("Failed Test 15");
				return false;
                        }
            return true;
        }
        std::string Reader::toString()
        {
            std::stringstream res;
            res << "This is gstd::Reader " << std::endl;
            res << "position: " << position << std::endl;
            res << "location: " << location << std::endl;
            return res.str();
        }
        
    }
}

