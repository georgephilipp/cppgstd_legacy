/* 
 * File:   SweepSchedules.cpp
 * Author: gschoenh
 * 
 * Created on December 10, 2013, 5:47 PM
 */

#include "stdafx.h"

#include "LambdaSweepSchedules.h"
#include "gstd/src/Primitives.h"

namespace msii810161816
{   
    namespace ml
    {        
        LambdaSweepSchedule::~LambdaSweepSchedule() {} 
        
        LambdaSweepScheduleArith::LambdaSweepScheduleArith()
        {
            reset();
        }
        
        double LambdaSweepScheduleArith::next()
        {
            if(iter >= numiter)
                return -1;
            double result = start + (double)iter*increment;
            iter++;
            return result;                
        }
        
        void LambdaSweepScheduleArith::reset()
        {
            iter = 0;
        }
        
        //Base Package
        gstd::TypeName LambdaSweepScheduleArith::getTypeName()
        {
            return gstd::TypeNameGetter<LambdaSweepScheduleArith>::get();
        }
        void LambdaSweepScheduleArith::setInputs()
        {
            start = 1;
            increment = -0.1;
            numiter = 10;
        }
        bool LambdaSweepScheduleArithTestHelperFn(LambdaSweepScheduleArith& obj)
        {
            std::vector<double> targets = {1,0.9,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1,-1,-1,-1};
            for(int i=0;i<13;i++)
            {
                double next = obj.next();     
                if(!gstd::Double::equals(targets[i],next))
                    return false;
            }
            return true;
        }        
        bool LambdaSweepScheduleArith::test()
        {
            LambdaSweepScheduleArith obj;
            obj.setInputs();
            if(!LambdaSweepScheduleArithTestHelperFn(obj))
            {
                obj.reportFailure();
                return false;  
            }
            obj.reset();
            if(!LambdaSweepScheduleArithTestHelperFn(obj))
            {
                obj.reportFailure();
                return false;  
            }           
            return true;
        }
        std::string LambdaSweepScheduleArith::toString()
        {
            std::stringstream res;
            res << "This is a ml::LambdaSweepScheduleArith" << std::endl;
            res << "start is: " << start << std::endl;
            res << "increment is: " <<increment << std::endl;
            res << "numiter is: " << numiter << std::endl;
            return res.str(); 
        }
                
        LambdaSweepScheduleGeo::LambdaSweepScheduleGeo()
        {
            reset();
        }
        
        double LambdaSweepScheduleGeo::next()
        {
            if(iter >= numiter)
                return -1;
            double result = start*accumfactor;
            accumfactor *= factor;
            iter++;
            return result;        
        }
        
        void LambdaSweepScheduleGeo::reset()
        {
            iter = 0;
            accumfactor = 1;
        }
        
        //Base Package
        gstd::TypeName LambdaSweepScheduleGeo::getTypeName()
        {
            return gstd::TypeNameGetter<LambdaSweepScheduleArith>::get();
        }
        void LambdaSweepScheduleGeo::setInputs()
        {
            start = 1;
            factor = 0.7;
            numiter = 10;
        }
        bool LambdaSweepScheduleGeoTestHelperFn(LambdaSweepScheduleGeo& obj)
        {
            std::vector<double> targets = {1,0.5,0.25,0.125,-1,-1,-1};
            for(int i=0;i<7;i++)
            {
                double next = obj.next();     
                if(!gstd::Double::equals(targets[i],next))
                    return false;
            }
            return true;
        }        
        bool LambdaSweepScheduleGeo::test()
        {
            LambdaSweepScheduleGeo obj;
            obj.setInputs();
            obj.factor = 0.5;
            obj.numiter = 4;
            if(!LambdaSweepScheduleGeoTestHelperFn(obj))
            {
                obj.reportFailure();
                return false;  
            }
            obj.reset();
            if(!LambdaSweepScheduleGeoTestHelperFn(obj))
            {
                obj.reportFailure();
                return false;  
            }           
            return true;
        }        
        std::string LambdaSweepScheduleGeo::toString()
        {
            std::stringstream res;
            res << "This is a ml::LambdaSweepScheduleGeo" << std::endl;
            res << "start is: " << start << std::endl;
            res << "factor is: " <<factor << std::endl;
            res << "numiter is: " << numiter << std::endl;
            return res.str(); 
        }
        
        
    }
}