/* 
 * File:   standard.h
 * Author: gschoenh
 *
 * Created on March 8, 2013, 4:51 PM
 */

#include "stdafx.h"

#ifndef msii810161816_DNN_STANDARD_H
#define	msii810161816_DNN_STANDARD_H

#ifdef _WIN32
//#pragma warning(disable:4251)
//#pragma warning(disable:4267)
#ifdef MSII810161816_DNN_EXPORTS
#define MSII810161816_DNN_API __declspec(dllexport)
#else
#define MSII810161816_DNN_API __declspec(dllimport)
#endif
#else
#define MSII810161816_DNN_API
#endif

#include<string>
#include<string.h>
#include<vector>
#include"gstd/src/ex.h"
#include"gstd/src/Printer.h"
#include"gstd/src/Base.h"

#endif	/* msii810161816_DNN_STANDARD_H */

