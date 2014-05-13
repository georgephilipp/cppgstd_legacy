/* 
 * File:   DefaultObjectServer.h
 * Author: gschoenh
 *
 * Created on December 12, 2013, 8:21 PM
 */

#include "stdafx.h"

#ifndef DEFAULTOBJECTSERVER_H
#define	DEFAULTOBJECTSERVER_H
#include "standard.h"
#include "Server.h"
#include "DataHeaders.h"


namespace msii810161816 
{
    namespace data 
    {
        template<typename datatype, typename dataHeaderType>
        class DefaultObjectServer : public Server<datatype,dataHeaderType>, public virtual gstd::Base 
        {
        public:        
            DefaultObjectServer()
            {
                capacity = -1;
            }
            virtual ~DefaultObjectServer() {}
            
            //options
            int capacity;
                        
        private:
            virtual gstd::trial<datatype> getinner( int index )
            {
                gstd::trial<datatype> res;
                res.success = true;
                if(capacity > -1 && index >= capacity)
                    res.success = false;
                return res;
            }
            virtual dataHeaderType getDataHeaderInner()
            {
                dataHeaderType out;
                return out;
            }
            
        //Base Package
        public:
            virtual gstd::TypeName getTypeName()
            {
                return gstd::TypeNameGetter<DefaultObjectServer<datatype,dataHeaderType> >::get();
            }
            virtual void setInputs() {}
            virtual bool test()
            {
                DefaultObjectServer<DpString,DataHeader> serv;
                
                //set and get
                serv.capacity = 5;
                serv.get(0);
                if(serv.storeddata.size() != 0 || serv.get(3).result.content != "")
                {
                    serv.reportFailure();
                    return false;
                }
                serv.storedata = true;
                serv.get(5);
                if(serv.storeddata.size() != 0 || serv.get(5).success != false)
                {
                    serv.reportFailure();
                    return false;
                }
                serv.get(3);
                if(serv.storeddata.size() != 1 || serv.storeddata.count(3) != 1 || serv.storeddata[3].content != "")
                {
                    serv.reportFailure();
                    return false;
                }
                DpString point;
                point.content = "bling";
                serv.set(0,point);
                if(serv.storeddata.size() != 2 || serv.storeddata.count(0) != 1 || serv.storeddata[0].content != "bling" || serv.get(0).result.content != "bling")
                {
                    serv.reportFailure();
                    return false;
                }
                serv.set(3,point);
                if(serv.storeddata.size() != 2 || serv.storeddata.count(3) != 1 || serv.storeddata[3].content != "bling" || serv.get(3).result.content != "bling")
                {
                    serv.reportFailure();
                    return false;
                }
                                                
                //set and get header
                serv.getDataHeader();
                if(serv.storedDataHeader.success != false || serv.getDataHeader().description != "")
                {
                    serv.reportFailure();
                    return false;
                }
                serv.storeDataHeader = true;
                serv.getDataHeader();
                if(serv.storedDataHeader.success != true || serv.getDataHeader().description != "")
                {
                    serv.reportFailure();
                    return false;
                }
                DataHeader dh;
                dh.description = "bling";
                serv.storedDataHeader.success = false;
                serv.setDataHeader(dh);
                if(serv.storedDataHeader.success != true || serv.getDataHeader().description != "bling")
                {
                    serv.reportFailure();
                    return false;
                }
                
                return true;
            }
            virtual std::string toString()
            {
                return "This is a data::DefaultObjectServer\n" + Server<datatype,dataHeaderType>::toString();
            }

        };
    }
}
namespace msii810161816
{
    namespace gstd
    {
        template<typename datatype, typename dataHeaderType>
        struct TypeNameGetter<data::DefaultObjectServer<datatype,dataHeaderType> >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "data::DefaultObjectServer";
                t.parameters.push_back(TypeNameGetter<datatype>::get());
                t.parameters.push_back(TypeNameGetter<dataHeaderType>::get());
                return t;
            }
        };
    }
}
#endif	/* DEFAULTOBJECTSERVER_H */

