/* 
 * File:   Aggregator.h
 * Author: heathermceveety
 *
 * Created on November 23, 2013, 12:29 AM
 */

#include "stdafx.h"

#ifndef AGGREGATOR_H
#define	AGGREGATOR_H
#include "standard.h"
#include "Client.h"
namespace msii810161816 
{
    namespace data 
    {
        template<typename datatype, typename headertype>
        class Aggregator : virtual public gstd::Base
        {
        public:
            virtual ~Aggregator() {}
            
            //inputs
            data::Client<datatype, headertype> inServer;
            virtual void fill(int begin, int end) = 0;
            
        //Base Package : nothing to do   
        public:
            virtual std::string toString()
            {
                std::stringstream res;
                res << "This is a data::Aggregator.\n";
                if(inServer.isNull())
                    res << "Server not set\n";
                else
                    res << inServer.toString();
                return res.str();
            }
        };
    }
}

#endif	/* AGGREGATOR_H */

