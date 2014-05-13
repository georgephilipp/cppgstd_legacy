/* 
 * File:   server.h
 * Author: gschoenh
 *
 * Created on March 6, 2013, 9:26 PM
 */

#include "stdafx.h"

#pragma once
#ifndef SERVER_H
#define	SERVER_H

#include"standard.h"
#include"gstd/src/Vector.h"
#include "Datapoints.h"
#include "DataHeaders.h"
#include<map>

namespace msii810161816 
{
    namespace data 
    {
        template<typename datatype, typename dataHeaderType>
        class DefaultObjectServer;
        
        template<typename datatype, typename dataHeaderType>
        class Server : public virtual gstd::Base 
        {
            friend DefaultObjectServer<DpString,DataHeader>;
        public:
            Server() 
            {
                minfail = -1;
                storedata = false;
                storeDataHeader = false;
                storedDataHeader.success = false;
            }
            virtual ~Server() {}
            
            //options
            bool storedata;
            bool storeDataHeader;
            
            //outputs            
            int minfail;
                
            //actions
            void set(int index, datatype input)
            {
                storeddata[index] = input;
            }   
            gstd::trial<datatype> get( int index )
            {
                if (storeddata.count(index))
                {
                    gstd::trial<datatype> out;
                    out.success = true;
                    out.result = storeddata[index];
                    return out;
                }    
                
                gstd::trial<datatype> out = getinner(index);
                
                if (!out.success && minfail > index)
                    minfail = index;
                
                if (out.success && storedata)
                    storeddata[index] = out.result;
                
                return out;
            }  
            void setminfail()
            {
                int index = 0;
                while(1)
                {
                    if( !get(index).success )
                        break;
                    index++;
                }
                minfail = index;
            }
            dataHeaderType getDataHeader()
            {
                if(storedDataHeader.success)
                    return storedDataHeader.result;
                dataHeaderType res = getDataHeaderInner();
                if(storeDataHeader)
                {
                    storedDataHeader.result = res;
                    storedDataHeader.success = true;
                }
                return res;
            }
            void setDataHeader(dataHeaderType input)
            {
                storedDataHeader.result = input;
                storedDataHeader.success = true;
            }
            int numStoredData()
            {
                return (int)storeddata.size();
            }
            
            //Base package
            //testing is performed from data::DefaultObjectServer
            virtual std::string toString()
            {
                std::stringstream res;
                res << "This server serves type " + gstd::TypeNameGetter<datatype>::get().toString();
                res << " and has data header type " + gstd::TypeNameGetter<dataHeaderType>::get().toString() << std::endl;
                try
                {
                    res << "Data Header is: " << getDataHeader().toString();
                }
                catch(std::exception e){}
                res << "minfail: " << minfail << std::endl;
                res << "storedata : "<< storedata << std::endl;
                res << "storeDataHeader : "<< storeDataHeader << std::endl;
                res << "Trying to find datapoint 0 ...";
                try
                {
                    gstd::trial<datatype> point = get(0);
                    if(!point.success)
                        throw std::exception();
                    res << "succeceded" << std::endl;
                    res << "A sample snippet from data point is: " << DpConverter<datatype,DpString>::get(point.result).content.substr(0, 100) << std::endl;
                }
                catch(std::exception e)
                {
                    res << "failed" << std::endl;
                }
                return res.str();
            }
            
        private:
            std::map<int, datatype> storeddata; 
            gstd::trial<dataHeaderType> storedDataHeader;
        protected:
            virtual gstd::trial<datatype> getinner( int index ) = 0;
            virtual dataHeaderType getDataHeaderInner()
            {
                gstd::error("Data Headers are not implemented for this server");
                dataHeaderType out;
                return out;
            }
        };        
    }
}


namespace msii810161816
{
    namespace gstd
    {
        template<typename datatype, typename headertype>
        struct TypeNameGetter<data::Server<datatype, headertype> >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "data::Server";
                t.parameters.push_back(TypeNameGetter<datatype>::get());
                t.parameters.push_back(TypeNameGetter<headertype>::get());
                return t;
            }
        };
    }
}
#endif	/* SERVER_H */

