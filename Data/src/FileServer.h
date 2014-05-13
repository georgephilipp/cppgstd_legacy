/* 
 * File:   FileServer.h
 * Author: gschoenh
 *
 * Created on February 13, 2013, 5:14 PM
 */

#include "stdafx.h"

#ifndef IFSTREAM_H
#define	IFSTREAM_H

#include"standard.h"
#include"Server.h"
#include"Datapoints.h"
#include"gstd/src/Reader.h"
#include "DataHeaders.h"

namespace msii810161816 
{
    namespace data 
    {
        class MSII810161816_DATA_API FileServer : public Server<DpString,DataHeader>, public gstd::Reader, virtual gstd::Base
        {
        public:
            virtual ~FileServer();
            
            //inputs
            char delimiter;
            //inherited: std::string location;
            
            //actions
            //inherited: void open();
            //inherited: void close();
        
        private:            
            virtual gstd::trial<DpString> getinner( int index );           
            
        //Base Package
        public:
            virtual gstd::TypeName getTypeName();
            virtual void setInputs();
            virtual bool test();
            virtual std::string toString();
        };
    }
}
namespace msii810161816
{
    namespace gstd
    {
        template<>
        struct TypeNameGetter<data::FileServer >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "data::FileServer";
                return t;
            }
        };
    }
}
#endif	/* IFSTREAM_H */

