/* 
 * File:   Dataset.h
 * Author: gschoenh
 *
 * Created on November 22, 2013, 8:40 PM
 */

#include "stdafx.h"

#ifndef DATASET_H
#define	DATASET_H
#include"standard.h"
#include"Server.h"
#include"DataHeaders.h"

namespace msii810161816 
{
    namespace data 
    {
        template<typename datatype,typename headertype>
        class Dataset : public Server<datatype, headertype>, public virtual gstd::Base
        {
        public:
            Dataset() : Server<datatype, headertype>() {}            
            //Dataset(const Dataset& orig);
            ~Dataset() {}
             
        private:
            virtual gstd::trial<datatype> getinner( int index )
            {
                (void)index;
                gstd::trial<datatype> res;
                res.success = false;                
                return res;
            }
            
            //Base package
        public:
            virtual gstd::TypeName getTypeName()
            {
                return gstd::TypeNameGetter<Dataset<datatype, headertype> >::get();
            }
            virtual void setInputs()
            {
                datatype t;
                Server<datatype, headertype>::set(0, t);
                Server<datatype, headertype>::set(1, t);
                Server<datatype, headertype>::set(2, t);
            }
            virtual bool test()
            {
                Dataset<DpString,DataHeader> dset;
                dset.setInputs();
                if(!dset.get(0).success || dset.get(4).success)
                {
                    gstd::Printer::c("Dataset failed test. " + dset.toString());
                    return false;
                }
                return true;        
            }
            virtual std::string toString()
            {
                std::stringstream res;
                res << "This is a data::Dataset.\n" << Server<datatype,headertype>::toString();
                return res.str();
            }
        };
    }
}

namespace msii810161816
{
    namespace gstd
    {
        template<typename datatype,typename headertype>
        struct TypeNameGetter<data::Dataset<datatype,headertype> >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "data::Dataset";
                t.parameters.push_back(TypeNameGetter<datatype>::get());
                t.parameters.push_back(TypeNameGetter<headertype>::get());
                return t;
            }
        };
    }
}
#endif	/* DATASET_H */

