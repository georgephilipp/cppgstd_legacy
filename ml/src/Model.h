/* 
 * File:   Model.h
 * Author: gschoenh
 *
 * Created on November 27, 2013, 9:05 PM
 */

#include "stdafx.h"

#pragma once
#ifndef MODEL_H
#define	MODEL_H

#include"standard.h"
#include"Data/src/Client.h"

namespace msii810161816
{   
    namespace ml
    {        
		struct MSII810161816_ML_API ModelCache
        {
            std::vector<double> parameter;
        };

		struct MSII810161816_ML_API ModelCacheProfile
        {
            ModelCacheProfile()
            {
                parameter = false;
            }
            
            bool parameter;
        };
        
		class MSII810161816_ML_API Model : public virtual gstd::Base
        {
        public:
            Model();            
            virtual ~Model();

            //inputs
            int maxDataPoint;
            
            //cache options
            bool cache;
            ModelCacheProfile modelCacheProfile;
            //target options
            std::vector<double> target;
			//termination options
			double ytol;
                   
            //outputs
            std::vector<double> parameter; 
            //cache
            std::vector<ModelCache> modelCache;

            //standard functions
            virtual void fit() = 0;
            virtual void initparameter() = 0;
            void cacheModel();
            //Base Package
            virtual std::string toString();
        private:
        };
        
        template<typename datatypename, typename headerTypeName>
        class ModelClient : virtual public gstd::Base
        {
        public:
            data::Client<datatypename, headerTypeName> client;    
            //Base Package
            virtual std::string toString()
            {
                std::stringstream res;
                res << "This is a ml::ModelClient";
                res << client.toString();
                return res.str();
            }
        };
        
        template<typename datatypename, typename headerTypeName>
        class ModelTyped : virtual public Model, public ModelClient<datatypename,headerTypeName>, virtual public gstd::Base
        {
        public:
            //Base Package
            virtual std::string toString()
            {
                std::stringstream res;
                res << "This is a ml::modelTyped";
                res << ModelClient<datatypename,headerTypeName>::toString();
                res << Model::toString();
                return res.str();
            }
        };
    }
}

#endif	/* MODEL_H */

