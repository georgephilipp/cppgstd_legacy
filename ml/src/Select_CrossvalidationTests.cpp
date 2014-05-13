#include "stdafx.h"

#include "CrossvalidationTests.h"
#include "GroupLassoExtSlepEng.h"
#include "SIOLassoExtSlepEng.h"

namespace msii810161816
{
	namespace ml
	{
            namespace crossValidation
            {
				bool dpRegressionTest()
                {
                    gstd::Printer::c("This is a test for the crossvalidation feature on abstract class ml::SelectionModel, datatype = DpRegression");

                    LassoExtSlepEng obj;
                    obj.setInputs();
                    obj.doc = true;
                    obj.safe = true;
                    obj.numFolds = 11;
                    LambdaSweepScheduleGeo lss;
                    lss.start = 8;
                    lss.factor = 0.5;
                    lss.numiter = 30;
                    obj.maxDataPoint = 4000;
                    LambdaSweepSchedule* old = obj.lambdaSweepSchedule.get(true);
                    delete old;
                    obj.lambdaSweepSchedule.set(&lss, false);
                    try
                    {
                            obj.crossValidate();
                    }
                    catch (std::exception e)
                    {
                            obj.reportFailure("safety checks on crossValidate()");
                            return false;
                    }
                    obj.fit();

                    if (obj.selection != obj.targetSelection)
                    {
                            obj.reportFailure("crossValidate()");
                            return false;
                    }
                    obj.lambda = 1e-4;
                    obj.fit();
                    if (obj.selection == obj.targetSelection)
                    {
                            obj.reportFailure("crossValidate()");
                            return false;
                    }
                    return true;
                }    

				bool dpMTRegressionTest()
				{
					gstd::Printer::c("This is a test for the crossvalidation feature on abstract class ml::SelectionModel, datatype = DpMTRegression");

					//result tes
					SIOLassoExtSlepEng sio2;
					sio2.setInputs();
					sio2.doc = true;
					sio2.safe = true;
					sio2.numFolds = 10;
					try
					{
						sio2.crossValidate();
					}
					catch (std::exception e)
					{
						sio2.reportFailure("safety checks on DpMTRegression::crossValidate()");
						return false;
					}
					sio2.fit();
					if (gstd::map::keys(sio2.selection) != gstd::map::keys(sio2.targetSelection))
					{
						sio2.reportFailure("res test");
						return false;
					}
					sio2.lambda = 1e-4;
					sio2.fit();
					if (sio2.selection == sio2.targetSelection)
					{
						sio2.reportFailure("res test 2");
						return false;
					}

					//regtest
					GroupLassoExtSlepEng obj;
					obj.setInputs();
					obj.doc = true;
					obj.safe = true;
					obj.numFolds = 3;
					LambdaSweepScheduleGeo lss;
					lss.start = 1;
					lss.factor = 0.95;
					lss.numiter = 200;				
					LambdaSweepScheduleGeo lss2;
					lss2.start = 0.5;
					lss2.factor = 0.95;
					lss2.numiter = 200;
					obj.maxDataPoint = 500;
					LambdaSweepSchedule* old = obj.lambdaSweepSchedule.get(true);
					delete old;
					obj.lambdaSweepSchedule.set(&lss, false);
					SIOLassoExtSlepEng sio;
					sio.safe = true;
					sio.doc = true;
					sio.target = obj.target;
					sio.targetSelection = obj.targetSelection;
					sio.lambdaRatio = 0;
					sio.maxDataPoint = obj.maxDataPoint;
					sio.lambdaSweepSchedule.set(&lss2, false);
					sio.numFolds = obj.numFolds;
					data::Server<DpRegression, InputGroupHeader>* orig = obj.client.get(false);
					data::Converter<DpRegression, DpRegression, InputGroupHeader, IOGroupHeader> conv;
					conv.inServer.set(orig, false);
					DpRegressionTaskDuplicator<IOGroupHeader> dupl;
					dupl.numTask = 4;
					dupl.inServer.set(&conv, false);
					sio.client.set(&dupl, false);

					obj.client.get(false)->storedata = true;
					for (int i = 0; i < obj.maxDataPoint;i++)
						obj.client.get(false)->get(i);
					int seed = gstd::Rand::i(10000);
					gstd::Rand::seed(seed);
					try
					{
						sio.crossValidate();
					}
					catch (std::exception e)
					{
						sio.reportFailure("safety checks on DpMTRegression::crossValidate()");
						return false;
					}
					gstd::Rand::seed(seed);
					try
					{
						obj.crossValidate();
					}
					catch (std::exception e)
					{
						obj.reportFailure("safety checks on DpRegression::crossValidate()");
						return false;
					}
					if (!gstd::Double::equals(2*sio.lambda, obj.lambda, 1e-14))
					{
						sio.reportFailure("reg test");
						return false;
					}
					return true;
				}
            }
        }
}

