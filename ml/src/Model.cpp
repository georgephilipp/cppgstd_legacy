/* 
 * File:   Model.cpp
 * Author: gschoenh
 * 
 * Created on November 27, 2013, 9:05 PM
 */

#include "stdafx.h"

#include "Model.h"

namespace msii810161816
{   
    namespace ml
    {
        Model::Model()
        {
            cache = false;
            modelCacheProfile.parameter = false;
            maxDataPoint = -1;
			ytol = 1e-4;
        }
        
        Model::~Model() {}
        
        void Model::cacheModel()
        {
            ModelCache cache;
            if(modelCacheProfile.parameter)
                cache.parameter = parameter;
            modelCache.push_back(cache);            
        }
        
        std::string Model::toString()
        {
            std::stringstream res;
            res << "This is a ml::Model" << std::endl;
            res << "maxDataPoint is: " << maxDataPoint << std::endl;
            res << "cache is: " << cache << std::endl;
			res << "ytol is: " << ytol << std::endl;
            res << "Snipped from target is: " << gstd::Printer::vp(gstd::vector::sub(target, 0, 10, true)) << std::endl;
            res << "Snipped from parameter is: " << gstd::Printer::vp(gstd::vector::sub(parameter, 0, 10, true)) << std::endl;
            res << "modelCache has size: " << modelCache.size() << std::endl;
            return res.str();
        }
    }
}