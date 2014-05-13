#include "stdafx.h"

#pragma once
#ifndef CROSSVALIDATIONTESTS_H
#define	CROSSVALIDATIONTESTS_H

#include"standard.h"
#include"SelectionModel.h"
#include"LassoExtSlepEng.h"

namespace msii810161816
{
	namespace ml
	{
            namespace crossValidation
            {
                bool MSII810161816_ML_API dpRegressionTest();
				bool MSII810161816_ML_API dpMTRegressionTest();
                
            }
        }
}
 
#endif	/* CROSSVALIDATIONTESTS_H */

