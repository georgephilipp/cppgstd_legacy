/* 
 * File:   Datapoints.h
 * Author: gschoenh
 *
 * Created on December 10, 2013, 3:59 PM
 */

#include "stdafx.h"

#ifndef ML_DATAPOINTS_H
#define	ML_DATAPOINTS_H

#include"Data/src/Datapoints.h"
#include"standard.h"
#include"gstd/src/Parser.h"

namespace msii810161816 
{
    namespace ml 
    {
		struct MSII810161816_ML_API DpRegression
        {
            DpRegression();
            
            std::vector<double> input;
            double output;   
        };

		struct MSII810161816_ML_API DpMTRegression
		{
			std::vector<double> input;
			std::vector<double> output;
		};
        
        namespace datapoints
        {
			bool MSII810161816_ML_API test();
        }
    }
}

namespace msii810161816 
{
    namespace data 
    {  
        template<>
        struct DpConverter<ml::DpRegression,DpString>
        {
            static DpString get(ml::DpRegression input)
            {
                DpString res;
                res.delimiter = ' ';
                input.input.push_back(input.output);
                res.content = gstd::Printer::vp(input.input,' ');
                return res;
            }
        };
        
        template<>
        struct DpConverter<DpString,ml::DpRegression>
        {
            static ml::DpRegression get(DpString input)
            {
                ml::DpRegression res;
                std::vector<double> content = gstd::Parser::vector<double>(input.content, input.delimiter);
                int size = content.size();
                for(int i=0;i<size-1;i++)
                    res.input.push_back(content[i]);
                res.output = content[size-1];
                return res;
            }
        };

		template<>
		struct DpConverter<ml::DpMTRegression, DpString>
		{
			static DpString get(ml::DpMTRegression input)
			{
				DpString res;
				res.delimiter = ' ';
				std::vector<double> resvector;
				resvector.push_back((double)input.input.size());
				resvector.push_back((double)input.output.size());
				resvector.insert(resvector.end(), input.input.begin(), input.input.end());
				resvector.insert(resvector.end(), input.output.begin(), input.output.end());
				res.content = gstd::Printer::vp(resvector, ' ');
				return res;
			}
		};

		template<>
		struct DpConverter<DpString, ml::DpMTRegression>
		{
			static ml::DpMTRegression get(DpString input)
			{
				ml::DpMTRegression res;
				std::vector<double> content = gstd::Parser::vector<double>(input.content, input.delimiter);
				gstd::check(content.size() >= 2, "data point has invalid size");
				int inSize = (int)content[0];
				int outSize = (int)content[1];
				gstd::check((int)content.size() == inSize + outSize + 2, "data point has invalid size");
				for (int i = 2; i<2+inSize; i++)
					res.input.push_back(content[i]);
				for (int i = 2+inSize; i<2 + inSize + outSize; i++)
					res.output.push_back(content[i]);
				return res;
			}
		};
    }
}

namespace msii810161816
{
    namespace gstd
    {
        template<>
        struct TypeNameGetter<ml::DpRegression>
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "ml::DpRegression";
                return t;
            }
        };

		template<>
		struct TypeNameGetter<ml::DpMTRegression>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::DpMTRegression";
				return t;
			}
		};
    }
}
#endif	/* ML_DATAPOINTS_H */

