/* 
 * File:   Aggregators.cpp
 * Author: gschoenh
 * 
 * Created on December 10, 2013, 4:05 PM
 */

#include "stdafx.h"

#include "Aggregators.h"
#include "gstd/src/Linalg.h"

namespace msii810161816 
{
    namespace ml 
    {
        RegressionAggregator::RegressionAggregator() 
        {
            d = -1;
            N = -1;
        }
        
        RegressionAggregator::~RegressionAggregator() {}
        
        void RegressionAggregator::fill(int begin, int end)
        {
            gstd::check(inServer.get(false) != 0, "cannot fill. Server is not set");
            gstd::vector::empty(input);
            gstd::vector::empty(output);
            d = -1;
            N = 0;
            gstd::trial<DpRegression> dp;
            int index = begin;
            while(index != end)
            {
                dp = inServer.get(false)->get(index);
                if(!dp.success)
                    break;
                if(d == -1)
                    d = dp.result.input.size();
                if((int)dp.result.input.size() != d)
                    gstd::error("inconsistent size");
                for(int i=0;i<d;i++)
                    input.push_back(dp.result.input[i]);
                output.push_back(dp.result.output);
                index++;
                N++;
            }
        }
        
        //Base package
        gstd::TypeName RegressionAggregator::getTypeName()
        {
            return gstd::TypeNameGetter<RegressionAggregator>::get();
        }
        void RegressionAggregator::setInputs()
        {
            data::Dataset<DpRegression,data::DataHeader>* dset = new data::Dataset<DpRegression,data::DataHeader>;
            DpRegression point;
            point.input = {1,2,3};
            point.output = 1;
            dset->set(0, point);
            point.input = {2,3,4};
            point.output = 2;
            dset->set(1, point);
            inServer.set(dset, true);
            fill(0, -1);
        }
        bool RegressionAggregator::test()
        {
            RegressionAggregator agg;
            agg.setInputs();
            std::vector<double> inTarget = {1,2,3,2,3,4};
            std::vector<double> outTarget = {1,2};
            if(agg.input != inTarget || agg.output != outTarget || agg.d != 3 || agg.N != 2)
            {
                agg.reportFailure();
                return false;
            }
            return true;
        }
        std::string RegressionAggregator::toString()
        {
            std::stringstream res;
            res << "This is a RegressionAggregator" << std::endl;
            res << "d is: " << d << std::endl;
            res << "N is: " << N << std::endl;
            if(input.size() == 0)
                res << "No data set\n";
            else
                res << "Sample data points:\n Input: " << gstd::Printer::mp(gstd::Linalg::submatrix(N, d, input, 0, 0, 3, 100, true)) << "\n Output: " << gstd::Printer::vp(gstd::vector::sub(output,0,3,true)) << "\n";
            res << data::Aggregator<DpRegression, data::DataHeader>::toString();
            return res.str();
        }

    }
}