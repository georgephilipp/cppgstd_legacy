/* 
 * File:   LambdaSweepSchedules.h
 * Author: gschoenh
 *
 * Created on December 10, 2013, 5:47 PM
 */

#include "stdafx.h"

#ifndef LAMBDASWEEPSCHEDULES_H
#define	LAMBDASWEEPSCHEDULES_H

#include "standard.h"

namespace msii810161816
{   
    namespace ml
    {
		class MSII810161816_ML_API LambdaSweepSchedule : public virtual gstd::Base
        {
        public:
            virtual double next() = 0;
            virtual ~LambdaSweepSchedule();
            virtual void reset() = 0;
        };
                
		class MSII810161816_ML_API LambdaSweepScheduleArith : public LambdaSweepSchedule, public virtual gstd::Base
        {
        public:
            double start;
            int numiter;
            double increment;
            
            LambdaSweepScheduleArith();
            virtual double next();
            virtual void reset();
            
            //Base package
            virtual gstd::TypeName getTypeName();
            virtual void setInputs();
            virtual bool test();
            virtual std::string toString();
            
        private:
            int iter;            
        };
        
		class MSII810161816_ML_API LambdaSweepScheduleGeo : public LambdaSweepSchedule, public virtual gstd::Base
        {
        public:
            double start;
            int numiter;
            double factor;
            
            LambdaSweepScheduleGeo();
            virtual double next();
            virtual void reset();
            
            //Base package
            virtual gstd::TypeName getTypeName();
            virtual void setInputs();
            virtual bool test();
            virtual std::string toString();
            
        private:
            int iter; 
            double accumfactor;
        };
    }
}
namespace msii810161816
{
    namespace gstd
    {
        template<>
        struct TypeNameGetter<ml::LambdaSweepScheduleArith>
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "ml::LambdaSweepScheduleArith";
                return t;
            }
        };
        
        template<>
        struct TypeNameGetter<ml::LambdaSweepScheduleGeo>
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "ml::LambdaSweepScheduleGeo";
                return t;
            }
        };
    }
}
#endif	/* LAMBDASWEEPSCHEDULES_H */

