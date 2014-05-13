/* 
 * File:   standard.h
 * Author: gschoenh
 *
 * Created on March 8, 2013, 7:19 PM
 */

#include "stdafx.h"

#ifndef msii810161816_ml_STANDARD_H
#define	msii810161816_ml_STANDARD_H

#ifdef _WIN32
#pragma warning(disable:4251)
#pragma warning(disable:4267)
#ifdef MSII810161816_ML_EXPORTS
#define MSII810161816_ML_API __declspec(dllexport)
#else
#define MSII810161816_ML_API __declspec(dllimport)
#endif
#else
#define MSII810161816_ML_API
#endif

#include<vector>
#include<string>
#include<string.h>
#include"gstd/src/ex.h"
#include"gstd/src/Printer.h"
#include"gstd/src/Base.h"





#endif	/* msii810161816_ml_STANDARD_H */

