/* 
 * File:   standard.h
 * Author: gschoenh
 *
 * Created on March 11, 2013, 8:39 PM
 */

#ifndef msii810161816_data_STANDARD_H
#define	msii810161816_data_STANDARD_H

#ifdef _WIN32
#pragma warning(disable:4251)
#pragma warning(disable:4267)
#ifdef MSII810161816_DATA_EXPORTS
#define MSII810161816_DATA_API __declspec(dllexport)
#else
#define MSII810161816_DATA_API __declspec(dllimport)
#endif
#else
#define MSII810161816_DATA_API
#endif

#include<vector>
#include<string>
#include<string.h>
#include"gstd/src/ex.h"
#include"gstd/src/Printer.h"
#include"gstd/src/Base.h"

#endif	/* msii810161816_data_STANDARD_H */

