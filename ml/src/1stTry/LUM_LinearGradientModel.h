/* 
 * File:   LUM_LinearGradientModel.h
 * Author: gschoenh
 *
 * Created on April 10, 2013, 7:57 PM
 */

#ifndef LUM_LINEARGRADIENTMODEL_H
#define	LUM_LINEARGRADIENTMODEL_H

#include"standard.h"
#include"LinearUpdateModel.h"


struct LGM_gradient 
{
    std::vector<double> values;
    std::vector<std::vector<double>> gradients;
};

class LUM_LinearGradientModel : public LinearUpdateModel
{
public:
    LUM_LinearGradientModel();
    virtual ~LUM_LinearGradientModel();
    
    bool usegradient;
    LGM_gradient gradient(bool getgradient = true);
    
private:
    void getupdatefromgradient();
};

#endif	/* LUM_LINEARGRADIENTMODEL_H */

