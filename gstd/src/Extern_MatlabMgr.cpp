/* 
 * File:   MatlabMgr.cpp
 * Author: gschoenh
 * 
 * Created on February 5, 2014, 7:58 PM
 */

#include"stdafx.h"

#include "MatlabMgr.h"
#include "Primitives.h"
#include "Linalg.h"
#include "Matrix.h"
#include "engine.h"
#include <sstream>
#include <iostream>
#include <cmath>

namespace msii810161816
{   
    namespace gstd
    {   
		struct MatlabEnginePimpl
		{
			Engine* engine;
		};

		const std::string MatlabSession::objRoots[3] = { "/usr0/home/gschoenh/Dropbox/Code_obj/Libraries/Matlab/", "C:/Users/gschoenh/Dropbox/Code_obj/Libraries/Matlab/", "/home/gschoenh/Dropbox/Code_obj/Libraries/Matlab/" };

        MatlabSession::MatlabSession() 
        {
            thisIsOpen = false;      
			engine = new MatlabEnginePimpl;
        }

        MatlabSession::~MatlabSession() 
        {
            if(thisIsOpen)
                close();
			delete engine;
        }
        
        MatlabSession MatlabSession::inst = MatlabSession();
        
        void MatlabSession::open()
        {
            gstd::check(!thisIsOpen, "cannot open matlab engine that is already open");
#ifdef _WIN32
			Engine* eng = engOpen(0);
			gstd::check(eng != 0, "could not open matlab engine");
			engine->engine = eng;
#else            
			gstd::check((engine->engine = engOpen("/opt/matlab/8.1/bin/matlab")) != 0, "could not open matlab engine");
#endif
			thisIsOpen = true;
        }
        
        void MatlabSession::close()
        {
            gstd::check(thisIsOpen, "cannot clsoe matlab engine that is already closed");
			gstd::check(engClose(engine->engine) == 0, "could not close matlab engine");
			thisIsOpen = false;
        }
        
        bool MatlabSession::isOpen()
        {
            return thisIsOpen;
        }
        
        TypeName MatlabSession::getTypeName()
        {
            return gstd::TypeNameGetter<MatlabSession>::get();
        }
        void MatlabSession::setInputs() {}
        bool MatlabSession::test()
        {
            MatlabSession obj;
            try
            {
                obj.open();
            }
            catch(std::exception e)
            {
                obj.reportFailure("cannot open session");
                return false;
            }
            try
            {
                std::cout << "Expecting error message ..." << std::endl;
                obj.open();
                obj.reportFailure("opened session twice");
                return false;
            }
            catch(std::exception e) {}
            try
            {
                obj.close();
            }
            catch(std::exception e)
            {
                obj.reportFailure("cannot close session");
                return false;
            }
            try
            {
                std::cout << "Expecting error message ..." << std::endl;
                obj.close();
                obj.reportFailure("closed session twice");
                return false;
            }
            catch(std::exception e) {} 
            return true;
        }
        std::string MatlabSession::toString()
        {
            std::stringstream str;
            str << "This is a gstd::MatlabSession" <<std::endl;
            str << "thisIsOpen is: " << thisIsOpen << std::endl;
            return str.str();
        }



        //MatlabMgr
        MatlabMgr::MatlabMgr() 
        {
            matlabSession.set(&MatlabSession::inst, false);
            hasOpenedInst = false;
        }
        
        MatlabMgr::~MatlabMgr() 
        {
            if(hasOpenedInst && MatlabSession::inst.isOpen())
                MatlabSession::inst.close();
        }

        //run
        void MatlabMgr::exec(std::string command)
        {
            gstd::check(matlabSession.get(false)->isOpen(), "MatlabMgr cannot operate. Session not open");
			char buffer[128];
			if (doc)
			{
				engOutputBuffer(matlabSession.get(false)->engine->engine, buffer, 128);
			}
            engEvalString(matlabSession.get(false)->engine->engine, command.c_str());  
			if (doc)
			{
				gstd::Printer::c(std::string(buffer));
			}	
        }
        
        void MatlabMgr::clear()
        {
            exec("clear;clc;");
        }        
        
        //getting
        std::tuple<int,int,std::vector<double> > MatlabMgr::get(std::string name)
        {          
            gstd::check(matlabSession.get(false)->isOpen(), "MatlabMgr cannot operate. Session not open");
			mxArray* R = engGetVariable(matlabSession.get(false)->engine->engine, name.c_str());
            int dim1 = mxGetM(R);
            int dim2 = mxGetN(R);            
            std::vector<double> res(dim1*dim2, 0);
            memcpy((void *)&res[0], (void*)mxGetPr(R), dim1*dim2*mxGetElementSize(R));
            mxDestroyArray(R);
            return std::tuple<int, int, std::vector<double> >(dim1, dim2, gstd::Linalg::transpose(dim2, dim1, res));
        }
        
        std::pair<int, int> MatlabMgr::getSize(std::string name)
        {
            exec("msii810161816 = size(" + name + ");");
            std::tuple<int,int,std::vector<double> > res = get("msii810161816");
            gstd::check(std::get<0>(res) == 1, "size vector is of incorrect size");
            gstd::check(std::get<1>(res) == 2, "size vector is of incorrect size");
            std::pair<int, int> out;
            out.first = (int)std::round(std::get<2>(res)[0]);
            out.second = (int)std::round(std::get<2>(res)[1]);
            return out;
        }               
        
        std::vector<double> MatlabMgr::getRow(std::string name, int rownum)
        {
            if(safe)
            {
                std::pair<int, int> matSize = getSize(name);
                gstd::check(matSize.first > rownum, "cannot retrieve row. Matrix too small");
            }
            
            exec("msii810161816 = "+name+"(" + std::to_string(rownum+1) + ",:);");
            std::tuple<int,int,std::vector<double> > res = get("msii810161816");
            gstd::check(std::get<0>(res) == 1, "retrieved row that has more than 1 row");
            return std::get<2>(res);            
        }
        
        std::vector<double> MatlabMgr::getCol(std::string name, int colnum)
        {
            if(safe)
            {
                std::pair<int, int> matSize = getSize(name);
                gstd::check(matSize.second > colnum, "cannot retrieve column. Matrix too small");
            }
            
            exec("msii810161816 = "+name+"(:," + std::to_string(colnum+1) + ");");
            std::tuple<int,int,std::vector<double> > res = get("msii810161816");
            gstd::check(std::get<1>(res) == 1, "retrieved column that has more than 1 column");
            return std::get<2>(res);            
        }

		//setting
        void MatlabMgr::create(int dim1, int dim2, std::vector<double> input, std::string name)
        {
            gstd::check(matlabSession.get(false)->isOpen(), "MatlabMgr cannot operate. Session not open");
            int size = input.size();
            gstd::check(size == dim1*dim2, "input has wrong dimension");
            mxArray* T = mxCreateDoubleMatrix(dim1, dim2, mxREAL);
			std::vector<double> inputTrans = gstd::Linalg::transpose(dim1, dim2, input); //matlab stores matrices with trasposed indices
			memcpy((void*)mxGetPr(T), (void *)&inputTrans[0], size*mxGetElementSize(T));
			engPutVariable(matlabSession.get(false)->engine->engine, name.c_str(), T);
            mxDestroyArray(T);   
            
			if (safe)
			{
				std::vector<double> retrieved = std::get<2>(get(name.c_str()));
				gstd::check(gstd::Linalg::mequals(input, retrieved), "variable transfer failed");
			} 
        }
        
        void MatlabMgr::setRow(std::vector<double> input, int rownum, std::string name)
        {
            int size = input.size();
            
            if(safe)
            {
                std::pair<int, int> matSize = getSize(name);
                gstd::check(matSize.second == size, "input row has incorrect size");
            }
            
            create(1, size, input, "msii810161816");
            exec(name + "(" + std::to_string(rownum+1) + ",:) = msii810161816;" );
            exec("clear msii810161816;");
            
            if(safe)
            {
                std::vector<double> retrieved = getRow(name, rownum);
                gstd::check(gstd::Linalg::mequals(retrieved, input), "setting of row failed");             
            }
        }
        
        void MatlabMgr::setCol(std::vector<double> input, int colnum, std::string name)
        {
            int size = input.size();
            
            if(safe)
            {
                std::pair<int, int> matSize = getSize(name);
                gstd::check(matSize.first == size, "input row has incorrect size");
            }
            
            create(size, 1, input, "msii810161816");
            exec(name + "(:," + std::to_string(colnum+1) + ") = msii810161816;" );
            exec("clear msii810161816;");
            
            if(safe)
            {
                std::vector<double> retrieved = getCol(name, colnum);
                gstd::check(gstd::Linalg::mequals(retrieved, input), "setting of column failed");             
            }            
        }
 
        //inst management
        void MatlabMgr::instOpen()
        {
            MatlabSession::inst.open();
            hasOpenedInst = true;
        }
        
        void MatlabMgr::instClose()
        {
            MatlabSession::inst.close();
            hasOpenedInst = false;
        }
        
        bool MatlabMgr::instIsOpen()
        {
            return MatlabSession::inst.isOpen();
        }
        
        //Base Package
        TypeName MatlabMgr::getTypeName()
        {
            return gstd::TypeNameGetter<MatlabMgr>::get();
        }
        void MatlabMgr::setInputs() {}
        bool MatlabMgr::test()
        {
            MatlabMgr* mgr = new MatlabMgr;
            mgr->safe = true;
            //inst management
            if(mgr->instIsOpen())
            {
                mgr->reportFailure("please do not execute this test while inst is open");
                return false;                
            }
            if(mgr->hasOpenedInst)
            {
                mgr->reportFailure("hasOpenedInst not set correctly");
                return false;                
            }
            try
            {
                mgr->instOpen();
            }
            catch(std::exception e)
            {
                mgr->reportFailure("cannot open inst");
                return false;
            }
            if(!mgr->hasOpenedInst)
            {
                mgr->reportFailure("hasOpenedInstnot set correctly");
                return false;                
            }
            if(!mgr->instIsOpen())
            {
                mgr->reportFailure("has not opened inst correctly");
                return false;                
            }
            try
            {
                mgr->instClose();
            }
            catch(std::exception e)
            {
                mgr->reportFailure("cannot close inst");
                return false;
            }
            if(mgr->hasOpenedInst)
            {
                mgr->reportFailure("hasOpenedInst not set correctly");
                return false;                
            }
            if(mgr->instIsOpen())
            {
                mgr->reportFailure("has not closed inst correctly");
                return false;                
            }
            mgr->instOpen();
            gstd::check(MatlabSession::inst.isOpen(),"this feature was already tested! it should not fail");
            delete mgr;
            if(MatlabSession::inst.isOpen())
            {
                gstd::Printer::c("deleting MatlabMgr has not closed inst!!!! this is seious!!!");
                return false;       
            }
            //getting and setting
			MatlabMgr obj;
            obj.safe = true;
			try
			{
				obj.instOpen();
				obj.create(2, 4, matrix::create(std::vector<std::vector<double> >({ { 1, 2, 3, 4 }, { 5, 6, 7, 8 } })), "var1");
				obj.setRow({ 9, 0, 1, 2 }, 4, "var1");
				obj.setCol({ 22, 26, 20, 20, 20 }, 1, "var1");
			}
			catch (std::exception e)
			{
				obj.reportFailure("safety checks for get and set failed");
				return false;
			}
            obj.exec("var1 = -var1;");
            std::tuple<int, int, std::vector<double> > res = get("var1");
            std::pair<int, int> resSize = getSize("var1");
            if(std::get<0>(res) != 5 || std::get<1>(res) != 4 || resSize.first != 5 || resSize.second != 4)
            {
                obj.reportFailure("sizes post assignment do not match");
                return false;  
            }
            if(!gstd::Linalg::mequals(std::get<2>(res),matrix::create(std::vector<std::vector<double> >({{-1,-22,-3,-4},{-5,-26,-7,-8},{0,-20,0,0},{0,-20,0,0},{-9,-20,-1,-2}}))))
            {
                obj.reportFailure("result is incorrect");
                return false;  
            }
            return true;
        }
        std::string MatlabMgr::toString()
        {
            std::stringstream res;
            res << "This is a MatlabMgr" << std::endl;
            if(matlabSession.isNull())
                res << "Session not set" <<std::endl;
            else
                res << "Session is: " << std::endl << matlabSession.get(false)->toString();
            return res.str();
        }
        
    }
}
