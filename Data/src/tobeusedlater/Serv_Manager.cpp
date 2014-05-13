/* 
 * File:   ServerManager.cpp
 * Author: gschoenh
 * 
 * Created on April 12, 2013, 11:12 AM
 */

#include "Serv_Manager.h"

namespace msii810161816 
{
    namespace data 
    {
        Serv_Manager::Serv_Manager() 
        {
        }
        
        virtual Serv_Manager::~Serv_Manager() 
        {
            int numservers = servers->size();
            int i;
            for (i=0;i<numservers;i++)
                delete servers[i];
        }    
        
        virtual gstd::trial<std::vector<double>> Serv_Manager::getinner( int index )
        {
            (void)index;
            throw("Virtual Method");
        }
    }
}


