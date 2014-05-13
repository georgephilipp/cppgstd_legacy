/* 
 * File:   LassoExtSlep.h
 * Author: heathermceveety
 *
 * Created on January 25, 2014, 10:58 PM
 */


#include "stdafx.h"

#ifndef LASSOEXTSLEP_H
#define	LASSOEXTSLEP_H

#include "standard.h"
#include "gstd/src/TerminalMgr.h"
#include "Lasso.h"
#include "SlepBase.h"

namespace msii810161816
{   
    namespace ml
    {
        class MSII810161816_ML_API LassoExtSlep : public Lasso, public SlepBase
        {
        public:
            LassoExtSlep();
            virtual ~LassoExtSlep();
            
            //inputs
            //everything from Lasso
            //inherited: gstd::TerminalMgr mgr; //the default settings are for my computer         
            
            //options
            //everything from Lasso
            //inherited: procname
            
            //actions
			virtual void fit(); //fit the model
            virtual void sweep(); //run the model for various velues of lambda and obtain a combined selection / ranking
			void crossValidate() { SelectionModelTyped<DpRegression, data::DataHeader>::crossValidate(); }
			//caching
			//inherited: void cacheModel(); //insert model outputs into cache
			//inherited: void cacheSelectionModel(); //insert selection model outputs into cache

			virtual void initparameter() {} //not used
        private:
            void fitInner(bool withSweep);
            
            //Base package
        public:
            virtual void setInputs(); //sets inputs for inputs for quick testing
            virtual bool test(); //test the algorithm
            virtual gstd::TypeName getTypeName(); //get string name of the type of this object
            virtual std::string toString();

        };
    }
}

namespace msii810161816
{
	namespace gstd
	{
		template<>
		struct TypeNameGetter<ml::LassoExtSlep>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::LassoExtSlep";
				return t;
			}
		};
	}
}

#endif	/* LASSOEXTSLEP_H */

