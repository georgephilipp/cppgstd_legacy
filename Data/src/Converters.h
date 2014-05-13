/* 
 * File:   Converter.h
 * Author: gschoenh
 *
 * Created on December 11, 2013, 6:35 PM
 */

#include "stdafx.h"

#pragma once
#ifndef CONVERTER_H
#define	CONVERTER_H
#include"standard.h"
#include"Datapoints.h"
#include"DataHeaders.h"
#include "Server.h"
#include "Client.h"

namespace msii810161816 
{
    namespace data 
    {
        template<typename dataIn, typename dataOut, typename headerIn, typename headerOut>
        class Converter : public Server<dataOut,headerOut>, virtual public gstd::Base
        {
        public:
            //inputs
            Client<dataIn,headerIn> inServer;
            
            gstd::trial<dataOut> getinner(int index)
            {       
                gstd::check(inServer.get(false) != 0,"trying to get from server that is not set");  
                gstd::trial<dataIn> input = inServer.get(false)->get(index);
                gstd::trial<dataOut> res;
                if(input.success == false)
                {
                    res.success = false;
                    return res;
                } 
				res.success = true;
                res.result = DpConverter<dataIn, dataOut>::get(input.result);
                return res;
            }
                
            headerOut getDataHeaderInner()
            {
                gstd::check(inServer.get(false) != NULL,"trying to get from server that is not set");                
                return DhConverter<headerIn, headerOut>::get(inServer.get(false)->getDataHeader());
            }
            
        //Base Package
        public:
            virtual gstd::TypeName getTypeName()
            {
                return gstd::TypeNameGetter<Converter<dataIn, dataOut, headerIn, headerOut> >::get();
            }
            virtual void setInputs() 
            {
                inServer.setInputs();
            }
            virtual bool test() 
            { 
                DpString point;
                point.content = "1,2,3";
                point.delimiter = ',';
                Dataset<DpString,DataHeader> dset;
                dset.set(0, point);
                
                Converter<DpString,DpDoubles,DataHeader,DataHeader> conv;
                conv.inServer.set(&dset, false);
                DpDoubles out = conv.get(0).result;
                if(out.content != std::vector<double>({1,2,3}))
                {
                    conv.reportFailure();
                    return false;
                }
                return true;
            }
            virtual std::string toString()
            {
                std::stringstream res;
                res << "This is a data::Converter" << std::endl;
                res << Server<dataOut,headerOut>::toString();
                res << inServer.toString();
                return res.str();
            }


        private:

        };

		template<typename datatype, typename headertype>
		class IndexMapper : public Server<datatype, headertype>, virtual public gstd::Base
		{
		public:
			//inputs
			Client<datatype, headertype> inServer;
			std::vector<int> indexmap;

			gstd::trial<datatype> getinner(int index)
			{
				gstd::check(inServer.get(false) != 0, "trying to get from server that is not set");
				gstd::trial<datatype> res;
				if ((int)indexmap.size() > index)
					return inServer.get(false)->get(indexmap[index]);
				else
					res.success = false;

				return res;
			}

			virtual headertype getDataHeaderInner()
			{
				return inServer.get(false)->getDataHeader();
			}

		private:

			//Base Package
		public:
			virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<IndexMapper<datatype, headertype> >::get();
			}
			virtual void setInputs()
			{
				Dataset<datatype, headertype>* dset = new Dataset<datatype, headertype>;
				datatype t;
				dset->set(0, t);
				inServer.set(dset, true);
				indexmap.push_back(0);
			}
			virtual bool test()
			{
				IndexMapper<DpString, DataHeader> m;
				m.setInputs();
				DpString point;
				point.content = "1";
				m.inServer.get(false)->set(1, point);
				point.content = "3";
				m.inServer.get(false)->set(3, point);
				m.indexmap.push_back(3);
				m.indexmap.push_back(1);
				if (m.get(1).success != true || m.get(1).result.content != "3"
					|| m.get(2).success != true || m.get(2).result.content != "1"
					|| m.get(3).success != false)
				{
					gstd::Printer::c("IndexMapper failed test. " + m.toString());
					return false;
				}
				return true;
			}
			virtual std::string toString()
			{
				std::stringstream res;
				res << "This is an IndexMapper" << std::endl;
				res << "Size of indexmap is " << indexmap.size();
				if (indexmap.size() > 0)
					res << " and largest element is " << gstd::vector::vmax(indexmap).second << std::endl;
				res << Server<datatype, headertype>::toString();
				res << inServer.toString();
				return res.str();
			}
		};
    }
}

namespace msii810161816
{
    namespace gstd
    {
        template<typename dataIn,typename dataOut, typename headerIn, typename headerOut>
        struct TypeNameGetter<data::Converter<dataIn,dataOut,headerIn, headerOut> >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "data::Converter";
                t.parameters.push_back(TypeNameGetter<dataIn>::get());
                t.parameters.push_back(TypeNameGetter<dataOut>::get());
                t.parameters.push_back(TypeNameGetter<headerIn>::get());
                t.parameters.push_back(TypeNameGetter<headerOut>::get());
                return t;
            }
        };

		template<typename datatype, typename headertype>
		struct TypeNameGetter<data::IndexMapper<datatype, headertype> >
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "data::IndexMapper";
				t.parameters.push_back(TypeNameGetter<datatype>::get());
				t.parameters.push_back(TypeNameGetter<headertype>::get());
				return t;
			}
		};
    }
}
#endif	/* CONVERTER_H */

