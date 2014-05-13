/* 
 * File:   Datapoints.cpp
 * Author: gschoenh
 * 
 * Created on December 10, 2013, 3:59 PM
 */

#include "stdafx.h"

#include "Datapoints.h"

namespace msii810161816 
{
    namespace ml 
    {   
        DpRegression::DpRegression()
        {
            output = 0;
        }        
        
        namespace datapoints
        {
            bool test()
            {
				gstd::Printer::c("This is the test for functions related to datapoints in namespace ml");

				//DpRegression
				{
					DpRegression point;
					point.input = std::vector<double>({ 1, 2, 3, 4 });
					point.output = 5;
					DpRegression recon = data::DpConverter<data::DpString, DpRegression>::get(data::DpConverter<DpRegression, data::DpString>::get(point));
					if (recon.input != point.input || recon.output != point.output)
					{
						gstd::Printer::c("Failed conversion of DpRegression");
						return false;
					}
				}

				//DpMTRegression
				{
					DpMTRegression point;
					point.input = std::vector<double>({ 1, 2, 3, 4 });
					point.output = { 5, 6, 7 };
					data::DpString base = data::DpConverter<DpMTRegression, data::DpString>::get(point);
					DpMTRegression recon = data::DpConverter<data::DpString, DpMTRegression>::get(base);
					if (recon.input != point.input || recon.output != point.output)
					{
						gstd::Printer::c("Failed conversion of DpMTRegression");
						return false;
					}
				}

                return true;
            }
        }
    }
}
