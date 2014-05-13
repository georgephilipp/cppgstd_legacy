/* 
 * File:   DataHeaders.h
 * Author: gschoenh
 *
 * Created on December 11, 2013, 2:51 PM
 */

#include "stdafx.h"

#pragma once
#ifndef DATA_DATAHEADERS_H
#define	DATA_DATAHEADERS_H
#include "standard.h"

namespace msii810161816
{
    namespace data
    {    
        class MSII810161816_DATA_API DataHeader : public virtual  gstd::Base
        {
        public:
            virtual ~DataHeader();

            std::string description;
            std::vector<std::string> log;    
            
        //Base package
        public:
            virtual gstd::TypeName getTypeName();
            virtual void setInputs();
            virtual bool test();
            virtual std::string toString();   
        };
        
        template<typename from, typename to>
        struct DhConverter
        {
            static to get(from input)
            {
                DataHeader c = DhConverter<from,DataHeader>::get(input);
                return DhConverter<DataHeader,to>::get(c);
            }
        };

        template<typename to>
        struct DhConverter<DataHeader, to>
        {
            static to get(DataHeader input)
            {
                (void)input;
                gstd::error("specialize me");
                to t;
                return t;
            }
        };

        template<typename from>
        struct DhConverter<from, DataHeader>
        {
            static DataHeader get(from input)
            {
                (void)input;
                gstd::error("specialize me");
                DataHeader res;
                return res;
            }
        };

		template<>
		struct DhConverter<DataHeader, DataHeader>
		{
			static DataHeader get(DataHeader input)
			{
				return input;
			}
		};
    }
}


namespace msii810161816
{
    namespace gstd
    {
        template<>
        struct TypeNameGetter<data::DataHeader >
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "data::DataHeader";
                return t;
            }
        };
    }
}
#endif	/* DATAHEADERS_H */

