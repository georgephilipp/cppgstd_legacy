/* 
 * File:   ServerManager.h
 * Author: gschoenh
 *
 * Created on April 12, 2013, 11:12 AM
 */

#ifndef SERV_MANAGER_H
#define	SERV_MANAGER_H

#include"standard.h"

namespace msii810161816 
{
    namespace data 
    {
        class Serv_Manager : public Server<std::vector<double>>
        {
        public:
            Serv_Manager();
            virtual ~Serv_Manager();
            std::vector<Server*> servers;
            
        private:
            virtual gstd::trial<std::vector<double>> getinner( int index );
        };
    }
}
#endif	/* SERV_MANAGER_H */

