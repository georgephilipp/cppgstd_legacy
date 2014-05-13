/* 
 * File:   FileServer.cpp
 * Author: gschoenh
 * 
 * Created on February 13, 2013, 5:14 PM
 */

#include "stdafx.h"

#include "FileServer.h"
#include "Datapoints.h"


namespace msii810161816
{
    namespace data
    {    
        FileServer::~FileServer() {}
        
        gstd::trial<DpString> FileServer::getinner( int index )
        {
            gstd::trial<DpString> res;
            gstd::trial<std::string> out = line(index);
            res.success = out.success;
            if(res.success)
            {
                res.result.content = out.result;
                res.result.delimiter = delimiter;
            }
            
            return res;
        }
        
        //Base Package
        gstd::TypeName FileServer::getTypeName()
        {
            return gstd::TypeNameGetter<FileServer>::get();
        }
        void FileServer::setInputs()
        {
            delimiter = ',';
            gstd::Reader::setInputs();
        }
        bool FileServer::test()
        {
            FileServer s;
            s.setInputs();
            s.get(1);
            gstd::trial<DpString> point = s.get(0);
            if(point.success == false || point.result.delimiter != ',' || point.result.content != "1,2,3")
            {
                gstd::Printer::c("FileServer failed test. " + s.toString());
                return false;
            }
            return true;
        }
        std::string FileServer::toString()
        {
            std::stringstream res;
            res << "This is a data::FileServer. ";
            res << "Delimiter is " << delimiter << std::endl;
            res << gstd::Reader::toString();
            res << Server<DpString,DataHeader>::toString();
            return res.str();
        }
    }
}