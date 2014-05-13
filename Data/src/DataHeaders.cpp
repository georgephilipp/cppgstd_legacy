/* 
 * File:   DataHeaders.cpp
 * Author: gschoenh
 * 
 * Created on December 11, 2013, 2:51 PM
 */

#include "stdafx.h"

#include "DataHeaders.h"

namespace msii810161816
{
    namespace data
    {  
        DataHeader::~DataHeader() {}
        
        //Base package
        gstd::TypeName DataHeader::getTypeName()
        {
            return gstd::TypeNameGetter<DataHeader>::get();
        }
        void DataHeader::setInputs() {}
        bool DataHeader::test() {return true;}
        std::string DataHeader::toString()
        {
            std::stringstream res;
            res << "This is a data::DataHeader" << std::endl;
            res << "Description: " << description << std::endl;
            res << "Log: " ;
            res << gstd::Printer::vp(log);
            
            return res.str();
        }
        
    }
}
    

