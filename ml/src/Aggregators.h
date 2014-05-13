/* 
 * File:   Aggregators.h
 * Author: gschoenh
 *
 * Created on December 10, 2013, 4:05 PM
 */

#include "stdafx.h"

#ifndef AGGREGATORS_H
#define	AGGREGATORS_H

#include"standard.h"
#include"Data/src/Aggregator.h"
#include"Data/src/DataHeaders.h"
#include"Datapoints.h"
namespace msii810161816 
{
    namespace ml 
    {
		class MSII810161816_ML_API RegressionAggregator : public data::Aggregator<DpRegression, data::DataHeader>, virtual public gstd::Base
        {
        public:
            RegressionAggregator();
            virtual ~RegressionAggregator();
            
            //inputs
            //inherited: data::Client<datatype, headertype> inServer;
            virtual void fill(int begin, int end);
            
            //outputs
            int d;
            int N;
            std::vector<double> input;
            std::vector<double> output;
                        
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
        struct TypeNameGetter<ml::RegressionAggregator>
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "ml::RegressionAggregator";
                return t;
            }
        };
    }
}
#endif	/* AGGREGATORS_H */

