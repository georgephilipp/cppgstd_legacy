/* 
 * File:   Datapoints.h
 * Author: gschoenh
 *
 * Created on November 22, 2013, 11:59 AM
 */

#include "stdafx.h"

#ifndef DATA_DATAPOINTS_H
#define	DATA_DATAPOINTS_H
#include"standard.h"
#include"gstd/src/Parser.h"

namespace msii810161816 
{
    namespace data 
    {       
        struct MSII810161816_DATA_API DpString 
        {
            DpString();
            
            std::string content;
            char delimiter;
        };
        
        struct MSII810161816_DATA_API DpDoubles {
            std::vector<double> content;          
        };  
        
        struct MSII810161816_DATA_API DpInts {
            std::vector<int> content;
        };

		template<typename T> struct DpTensor {
			std::vector<T> content;
			std::vector<int> dimensions;
		};
        
        template<typename from, typename to>
        struct DpConverter
        {
            static to get(from input)
            {
                DpString c = DpConverter<from,DpString>::get(input);
                return DpConverter<DpString,to>::get(c);
            }
        };

        template<typename to>
        struct DpConverter<DpString, to>
        {
            static to get(DpString input)
            {
                (void)input;
                gstd::error("specialize me");
                to t;
                return t;
            }
        };

        template<typename from>
        struct DpConverter<from, DpString>
        {
            static DpString get(from input)
            {
                (void)input;
                gstd::error("specialize me");
                DpString res;
                return res;
            }
        };
        
        template<>
        struct DpConverter<DpString, DpString>
        {
            static DpString get(DpString input)
            {
                return input;
            }
        };

        template<>
        struct DpConverter<DpDoubles,DpString>
        {
            static DpString get(DpDoubles input)
            {
                DpString res;
                res.delimiter = ' ';
                res.content = gstd::Printer::vp(input.content,' ');
                return res;
            }
        };
        
        template<>
        struct DpConverter<DpString,DpDoubles> 
        {
        public:
            static DpDoubles get(DpString input)
            {
                DpDoubles res;
                res.content = gstd::Parser::vector<double>(input.content, input.delimiter);
                return res;
            }
        };
        
        template<>
        struct DpConverter<DpInts,DpString>
        {
            static DpString get(DpInts input)
            {
                DpString res;
                res.delimiter = ' ';
                res.content = gstd::Printer::vp(input.content,' ');
                return res;
            }
        };
        
        template<>
        struct DpConverter<DpString,DpInts> 
        {
        public:
            static DpInts get(DpString input)
            {
                DpInts res;
                res.content = gstd::Parser::vector<int>(input.content, input.delimiter);
                return res;
            }
        };
        
        namespace datapoints
        {
            bool MSII810161816_DATA_API test();
        }
    }
}

namespace msii810161816
{
    namespace gstd
    {
        template<>
        struct TypeNameGetter<data::DpString>
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "data::DpString";
                return t;
            }
        };
        
        template<>
        struct TypeNameGetter<data::DpDoubles>
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "data::DpDoubles";
                return t;
            }
        };
        
        template<>
        struct TypeNameGetter<data::DpInts>
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "data::DpInts";
                return t;
            }
        };
    }
}


#endif	/* DATAPOINTS_H */

