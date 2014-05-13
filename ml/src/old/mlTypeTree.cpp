/* 
 * File:   mlTypeTree.cpp
 * Author: gschoenh
 * 
 * Created on December 5, 2013, 12:44 PM
 */

#include "mlTypeTree.h"

namespace msii810161816
{   
    namespace ml
    {
        mlTypeTree::mlTypeTree() {
        }

        //mlTypeTree::mlTypeTree(const mlTypeTree& orig) {}

        mlTypeTree::~mlTypeTree() {
        }
        
        
        gstd::TypeTree mlTypeTree::makeInst()
        {
            gstd::TypeTree tree;
            
            std::map<std::string,std::vector<std::string> > res;
            res["ml::Model"] = {"gstd::ErasureBaseInterface"};
            res["ml::Lasso"] = {"ml::Model"};
            
            tree.typeMap = res;  
            tree.childTypeTrees.push_back(&gstd::gstdTypeTree::inst);
            
            return tree;
        }
       
        gstd::TypeTree mlTypeTree::inst = mlTypeTree::makeInst();
    }
}

