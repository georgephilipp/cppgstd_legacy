#include "stdafx.h"

#pragma once
#ifndef ML_CONVERTERS_H
#define	ML_CONVERTERS_H
#include"standard.h"
#include"Datapoints.h"
#include"DataHeaders.h"
#include "Data/src/Server.h"
#include "Data/src/Client.h"
#include "Data/src/Dataset.h"

namespace msii810161816
{
	namespace ml
	{
		template<typename headertype>
		class DpRegressionComponentMapper : public data::Server<DpRegression, headertype>, virtual public gstd::Base
		{
		public:
			//inputs
			data::Client<DpRegression, headertype> inServer;
			std::vector<int> componentMap;

			//actions
			//everything from server

		private:
			gstd::trial<DpRegression> getinner(int index)
			{
				gstd::trial<DpRegression> res;
				gstd::trial<DpRegression> input = inServer.get(false)->get(index);
				if (!input.success)
					return input;
				res.result.output = input.result.output;
				int cMapSize = componentMap.size();
				res.result.input.resize(cMapSize);
				for (int i = 0; i<cMapSize; i++)
				{
					if ((int)input.result.input.size() > componentMap[i])
						res.result.input[i] = input.result.input[componentMap[i]];
					else
						gstd::error("componentMap requires nonExistant component");
				}
				res.success = true;
				return res;
			}

			virtual headertype getDataHeaderInner()
			{
				return inServer.get(false)->getDataHeader();
			}

			//Base Package
		public:
			virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<DpRegressionComponentMapper<headertype> >::get();
			}
			virtual void setInputs()
			{
				data::Dataset<DpRegression, headertype>* dset = new data::Dataset<DpRegression, headertype>;
				DpRegression point;
				point.input = { 1, 2, 3, 4, 5 };
				point.output = 6;
				dset->set(0, point);
				point.input = { 6, 7, 8, 9, 0 };
				point.output = { 11 };
				dset->set(1, point);
				inServer.set(dset, true);
				componentMap.push_back(0);
				componentMap.push_back(4);
				componentMap.push_back(2);
			}
			virtual bool test()
			{
				DpRegressionComponentMapper obj;
				obj.setInputs();
				gstd::trial<DpRegression> target1;
				target1.success = true;
				target1.result.output = 6;
				target1.result.input = { 1, 5, 3 };
				gstd::trial<DpRegression> target2;
				target2.success = true;
				target2.result.output = 11;
				target2.result.input = { 6, 0, 8 };
				if (
					!obj.get(0).success
					|| obj.get(0).result.input != target1.result.input
					|| obj.get(0).result.output != target1.result.output
					|| !obj.get(1).success
					|| obj.get(1).result.input != target2.result.input
					|| obj.get(1).result.output != target2.result.output
					|| obj.get(2).success
					)
				{
					obj.reportFailure("");
					return false;
				}
				return true;
			}
			virtual std::string toString()
			{
				std::stringstream res;
				res << "This is an ml::DpRegressionComponentMapper" << std::endl;
				res << "Size of componentMap is " << componentMap.size();
				if (componentMap.size() > 0)
					res << " and largest element is " << gstd::vector::vmax(componentMap).second << std::endl;
				res << data::Server<DpRegression, headertype>::toString();
				res << inServer.toString();
				return res.str();
			}
		};

		template<typename headertype>
		class DpMTRegressionTaskExtractor : public data::Server<DpRegression, headertype>, virtual public gstd::Base
		{
		public:
			//inputs
			data::Client<DpMTRegression, headertype> inServer;
			int taskNum;

			//actions
			//everything from server

		private:
			gstd::trial<DpRegression> getinner(int index)
			{
				gstd::trial<DpRegression> res;
				gstd::trial<DpMTRegression> point = inServer.get(false)->get(index);
				if (!point.success)
				{
					res.success = false;
					return res;
				}
				res.success = true;
				res.result.input = point.result.input;
				gstd::check((int)point.result.output.size() > taskNum, "not enough tasks in input data");
				res.result.output = point.result.output[taskNum];
				return res;
			}

			virtual headertype getDataHeaderInner()
			{
				return inServer.get(false)->getDataHeader();
			}

			//Base Package
		public:
			virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<DpMTRegressionTaskExtractor<headertype> >::get();
			}
			virtual void setInputs()
			{
				data::Dataset<DpMTRegression, headertype>* dset = new data::Dataset<DpMTRegression, headertype>;
				DpMTRegression point;
				point.input = { 1, 2, 3, 4, 5 };
				point.output = { 6, 7, 8 };
				dset->set(0, point);
				point.input = { 6, 7, 8, 9, 0 };
				point.output = { 11, 12, 13 };
				dset->set(1, point);
				inServer.set(dset, true);
				taskNum = 2;
			}
			virtual bool test()
			{
				DpMTRegressionTaskExtractor<headertype> obj;
				obj.setInputs();
				gstd::trial<DpRegression> target1;
				target1.success = true;
				target1.result.output = 8;
				target1.result.input = { 1, 2, 3, 4, 5 };
				gstd::trial<DpRegression> target2;
				target2.success = true;
				target2.result.output = 13;
				target2.result.input = { 6, 7, 8, 9, 0 };
				if (
					!obj.get(0).success
					|| obj.get(0).result.input != target1.result.input
					|| obj.get(0).result.output != target1.result.output
					|| !obj.get(1).success
					|| obj.get(1).result.input != target2.result.input
					|| obj.get(1).result.output != target2.result.output
					|| obj.get(2).success
					)
				{
					obj.reportFailure("");
					return false;
				}
				return true;
			}
			virtual std::string toString()
			{
				std::stringstream res;
				res << "This is an ml::DpMTRegressionTaskExtractor" << std::endl;
	            res << "taskNum is " << taskNum << std::endl;
				res << data::Server<DpRegression, headertype>::toString();
				res << inServer.toString();
				return res.str();
			}
		};

		template<typename headertype>
		class DpRegressionTaskDuplicator : public data::Server<DpMTRegression, headertype>, virtual public gstd::Base
		{
		public:
			//inputs
			data::Client<DpRegression, headertype> inServer;
			int numTask;

			//actions
			//everything from server

		private:
			gstd::trial<DpMTRegression> getinner(int index)
			{
				gstd::trial<DpMTRegression> res;
				gstd::trial<DpRegression> point = inServer.get(false)->get(index);
				if (!point.success)
				{
					res.success = false;
					return res;
				}
				res.success = true;
				res.result.input = point.result.input;
				res.result.output = std::vector<double>(numTask, point.result.output);
				return res;
			}

			virtual headertype getDataHeaderInner()
			{
				return inServer.get(false)->getDataHeader();
			}

			//Base Package
		public:
			virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<DpRegressionTaskDuplicator<headertype> >::get();
			}
			virtual void setInputs()
			{
				data::Dataset<DpRegression, headertype>* dset = new data::Dataset<DpRegression, headertype>;
				DpRegression point;
				point.input = { 1, 2, 3, 4, 5 };
				point.output = 6;
				dset->set(0, point);
				point.input = { 6, 7, 8, 9, 0 };
				point.output = 11;
				dset->set(1, point);
				inServer.set(dset, true);
				numTask = 3;
			}
			virtual bool test()
			{
				DpRegressionTaskDuplicator<headertype> obj;
				obj.setInputs();
				gstd::trial<DpMTRegression> target1;
				target1.success = true;
				target1.result.output = { 6, 6, 6 };
				target1.result.input = { 1, 2, 3, 4, 5 };
				gstd::trial<DpMTRegression> target2;
				target2.success = true;
				target2.result.output = { 11, 11, 11 };
				target2.result.input = { 6, 7, 8, 9, 0 };
				if (
					!obj.get(0).success
					|| obj.get(0).result.input != target1.result.input
					|| obj.get(0).result.output != target1.result.output
					|| !obj.get(1).success
					|| obj.get(1).result.input != target2.result.input
					|| obj.get(1).result.output != target2.result.output
					|| obj.get(2).success
					)
				{
					obj.reportFailure("");
					return false;
				}
				return true;
			}
			virtual std::string toString()
			{
				std::stringstream res;
				res << "This is an ml::DpRegressionTaskDuplicator" << std::endl;
				res << "numTask is " << numTask << std::endl;
				res << data::Server<DpMTRegression, headertype>::toString();
				res << inServer.toString();
				return res.str();
			}
		};
	}
}

namespace msii810161816
{
	namespace gstd
	{
		template<typename headertype>
		struct TypeNameGetter<ml::DpRegressionComponentMapper<headertype> >
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::DpRegressionComponentMapper";
				t.parameters.push_back(TypeNameGetter<headertype>::get());
				return t;
			}
		};

		template<typename headertype>
		struct TypeNameGetter<ml::DpMTRegressionTaskExtractor<headertype> >
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::DpMTRegressionTaskExtractor";
				t.parameters.push_back(TypeNameGetter<headertype>::get());
				return t;
			}
		};

		template<typename headertype>
		struct TypeNameGetter<ml::DpRegressionTaskDuplicator<headertype> >
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::DpRegressionTaskDuplicator";
				t.parameters.push_back(TypeNameGetter<headertype>::get());
				return t;
			}
		};
	}
}
#endif