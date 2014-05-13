/* 
 * File:   mlTypeTree.h
 * Author: gschoenh
 *
 * Created on December 5, 2013, 12:44 PM
 */

#ifndef MLTYPETREE_H
#define	MLTYPETREE_H
#include"gstd/src/TypeTree.h"

namespace msii810161816
{   
    namespace ml
    {
        class mlTypeTree {
        public:
            mlTypeTree();
            //mlTypeTree(const mlTypeTree& orig);
            virtual ~mlTypeTree();
            
            static gstd::TypeTree makeInst();
            static gstd::TypeTree inst;
            
        private:

        };
    }
}

#endif	/* MLTYPETREE_H */

