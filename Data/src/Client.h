/* 
 * File:   client.h
 * Author: gschoenh
 *
 * Created on March 8, 2013, 10:42 AM
 */

#include "stdafx.h"

#ifndef CLIENT_H
#define	CLIENT_H
#include"Server.h"
#include"Datapoints.h"
#include"Dataset.h"
#include"gstd/src/Pointer.h"

namespace msii810161816 
{
    namespace data 
    {
        template<typename datatype, typename headertype>
        class Client : public gstd::Pointer<Server<datatype,headertype> > //inherits stuff directly from Pointer: ,public virtual gstd::Base
        {
            
        //Base package
        public:
            virtual void setInputs()
            {
                datatype point;
                Dataset<datatype,headertype>* dset = new Dataset<datatype,headertype>;
                dset->set(0,point);
                gstd::Pointer<Server<datatype,headertype> >::set(dset, true);
            }            
            virtual gstd::TypeName getTypeName()
            {
                return gstd::TypeNameGetter<Client<datatype, headertype> >::get();
            }
            virtual std::string toString()
            {
                std::stringstream res;
                res << "This is a data::Client." << std::endl;
                res << gstd::Pointer<Server<datatype,headertype> >::toString();



                if(gstd::Pointer<Server<datatype,headertype> >::isNull())
                    res << "no server set \n";
				else
				{
					Server<datatype, headertype>* point = gstd::Pointer<Server<datatype, headertype> >::get(false);
					res << "served by: " + point->toString();
				}
                    
                return res.str();
            }           
        };
    }
}


namespace msii810161816
{
    namespace gstd
    {
        template<typename datatype, typename headertype>
        struct TypeNameGetter<data::Client<datatype,headertype> >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "data::Client";
                t.parameters.push_back(TypeNameGetter<datatype>::get());
                t.parameters.push_back(TypeNameGetter<headertype>::get());
                return t;
            }
        };
    }
}

#endif	/* CLIENT_H */

