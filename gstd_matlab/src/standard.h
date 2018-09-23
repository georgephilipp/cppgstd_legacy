/* 
 * File:   standard.h
 * Author: gschoenh
 *
 * Created on March 8, 2013, 4:51 PM
 */

#include "stdafx.h"

#ifndef msii810161816_gstd_matlab_STANDARD_H
#define	msii810161816_gstd_matlab_STANDARD_H

#ifdef _WIN32
#pragma warning(disable:4251)
#pragma warning(disable:4267)
#ifdef MSII810161816_GSTD_MATLAB_EXPORTS
#define MSII810161816_GSTD_MATLAB_API __declspec(dllexport)
#else
#define MSII810161816_GSTD_MATLAB_API __declspec(dllimport)
#endif
#else
#define MSII810161816_GSTD_MATLAB_API
#endif

#include<string>
#include<string.h>
#include<vector>

#endif	/* msii810161816_gstd_STANDARD_H */

