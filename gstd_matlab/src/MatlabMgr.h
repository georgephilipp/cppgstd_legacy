/* 
 * File:   MatlabMgr.h
 * Author: gschoenh
 *
 * Created on February 5, 2014, 7:58 PM
 */




#include "stdafx.h"

#pragma once
#ifndef MATLABMGR_H
#define	MATLABMGR_H

#include"standard.h"
#include"gstd/src/ex.h"
#include"gstd/src/Base.h"
#include "gstd/src/Pointer.h"

namespace msii810161816
{   
    namespace gstd
    {     
        class MatlabMgr;
		struct MatlabEnginePimpl;

		//MatlabSession
		class MSII810161816_GSTD_MATLAB_API MatlabSession : public virtual Base
		{
			friend MatlabMgr;

		public:
			MatlabSession();
			~MatlabSession();

			void open();
			void close();

			bool isOpen();

		private:
			static MatlabSession inst; //default target for MatlabMgr
			bool thisIsOpen;
			MatlabEnginePimpl* engine;
			
			//Base Package
		public:
			virtual TypeName getTypeName();
			virtual void setInputs();
			virtual bool test();
			virtual std::string toString();

		};

		class MSII810161816_GSTD_MATLAB_API MatlabMgr : public virtual Base
		{
		public:
			MatlabMgr();
			virtual ~MatlabMgr();

			gstd::Pointer<MatlabSession> matlabSession;

			//run
			void exec(std::string command);
			void clear();

			//getting
			std::tuple<int, int, std::vector<double> > get(std::string name);
			std::pair<int, int> getSize(std::string name);
			std::vector<double> getRow(std::string name, int rownum);
			std::vector<double> getCol(std::string name, int rownum);

			//creation
			void create(int dim1, int dim2, std::vector<double> input, std::string name);
			void setRow(std::vector<double> input, int rownum, std::string name);
			void setCol(std::vector<double> input, int colnum, std::string name);

			//manage inst
			void instOpen();
			void instClose();
			bool instIsOpen();

		private:
			bool hasOpenedInst;

			//Base Package
		public:
			virtual TypeName getTypeName();
			virtual void setInputs();
			virtual bool test();
			virtual std::string toString();
		};



		/*class MatlabSession;

		class MSII810161816_GSTD_API MatlabSessionPimpl
		{
		public:
			MatlabSessionPimpl();
			~MatlabSessionPimpl();

			MatlabSession* get(bool release);
		private:
			MatlabSession* matlabSession;
			bool ownsPointer;
		};

		class MatlabMgr;

		class MSII810161816_GSTD_API MatlabMgrPimpl
		{
		public:
			MatlabMgrPimpl();
			~MatlabMgrPimpl();

			MatlabMgr* get(bool release);
		private:
			MatlabMgr* matlabMgr;
			bool ownsPointer;
		};*/

        template<>
        struct TypeNameGetter<MatlabSession>
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "gstd::MatlabSession";
                return t;
            }
        };
        
        template<>
        struct TypeNameGetter<MatlabMgr>
        {
            static TypeName get()
            {
                TypeName t;
                t.name = "gstd::MatlabMgr";
                return t;
            }
        };
    }
}

#endif	/* MATLABMGR_H */

