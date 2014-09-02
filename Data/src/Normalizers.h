/*
* File:   RegressionNormalizer.h
* Author: gschoenh
*
* Created on December 6, 2013, 6:35 PM
*/

#include "stdafx.h"

#ifndef NORMALIZER_H
#define	NORMALIZER_H

#include "standard.h"
#include "Datapoints.h"
#include "Client.h"
#include "gstd/src/Primitives.h"
#include "gstd/src/Stat.h"
#include <math.h>

namespace msii810161816
{
	namespace data
	{
		template<typename headerType>
		class DpDoublesNormalizer : public data::Server<DpDoubles, headerType>
		{
		public:
			DpDoublesNormalizer()
			{
				d = -1;
			}
			virtual ~DpDoublesNormalizer() {}

			//inputs
			bool normalizeStd;
			bool normalizeMean;
			data::Client<DpDoubles, headerType> inServer;
			//means and stds:either set directly or all initMeansStds
			std::vector<double> inputStds;
			std::vector<double> inputMeans;
			void initMeansStds(int begin, int length) //call this before using server features
			{
				int cursor = 0;
				while (cursor != length)
				{
					gstd::trial<DpDoubles> point = inServer.get(false)->get(cursor + begin);
					if (d == -1)
					{
						if (!point.success)
							return;
						d = (int)point.result.content.size();
					}
					if (cursor == 0)
					{
						inputStds = std::vector<double>(d);
						inputMeans = std::vector<double>(d);
					}
					if (!point.success)
						break;
					gstd::check((int)point.result.content.size() == d, "received point of incorrect input size");
					for (int i = 0; i<d; i++)
					{
						inputStds[i] += point.result.content[i] * point.result.content[i];
						inputMeans[i] += point.result.content[i];
					}
					cursor++;
				}
				for (int i = 0; i<d; i++)
				{
					inputStds[i] = sqrt(inputStds[i] / ((double)cursor));
					inputMeans[i] /= ((double)cursor);
				}
			}

		private:
			int d;
			virtual gstd::trial<DpDoubles> getinner(int index)
			{
				gstd::trial<DpDoubles> point = inServer.get(false)->get(index);
				if (!point.success)
					return point;
				if (d == -1)
					d = (int)point.result.content.size();
				gstd::check((int)point.result.content.size() == d, "received point of incorrect input size");
				gstd::check((int)inputMeans.size() == d, "inputMeans of incorrect size");
				gstd::check((int)inputStds.size() == d, "inputStds of incorrect size");
				if (normalizeMean)
				{
					for (int i = 0; i<d; i++)
						point.result.content[i] -= inputMeans[i];
					if (normalizeStd)
					{
						for (int i = 0; i<d; i++)
							point.result.content[i] /= sqrt(inputStds[i] * inputStds[i] - inputMeans[i] * inputMeans[i]);
					}
				}
				else if (normalizeStd)
				{
					for (int i = 0; i<d; i++)
						point.result.content[i] /= inputStds[i];
				}

				return point;
			}
			virtual headerType getDataHeaderInner()
			{
				return inServer.get(false)->getDataHeader();
			}

			//Base Package
		public:
			virtual gstd::TypeName getTypeName()
			{
				return gstd::TypeNameGetter<DpDoublesNormalizer<headerType> > ::get();
			}
			virtual void setInputs()
			{
				data::DpDoubles point;
				d = 10;
				for (int i = 0; i<d; i++)
					point.content.push_back((double)i);
				data::Dataset<data::DpDoubles, headerType>* dset = new data::Dataset<data::DpDoubles, headerType>;
				for (int n = 0; n<10000; n++)
				{
					data::DpDoubles newpoint = point;
					for (int i = 0; i<d; i++)
						newpoint.content[i] += 4 * gstd::stat::randnorm(4); //100*(gstd::Rand::d(3)-0.5);
					dset->set(n, newpoint);
				}
				inServer.set(dset, true);
				d = 10;
				normalizeMean = true;
				normalizeStd = true;
				initMeansStds(0, 1000);
			}
			virtual bool test()
			{
				DpDoublesNormalizer<data::DataHeader> norm;
				norm.setInputs();
				int dim = -1;
				std::vector<double> means(0);
				std::vector<double> vars(0);
				for (int n = 0; n<1000; n++)
				{
					DpDoubles point = norm.get(n).result;
					if (dim == -1)
					{
						dim = point.content.size();
						means.resize(dim);
						vars.resize(dim);
					}
					for (int i = 0; i<dim; i++)
					{
						means[i] += point.content[i];
						vars[i] += point.content[i] * point.content[i];
					}
				}
				for (int i = 0; i < dim; i++)
				{
					if (!gstd::Double::equals(means[i], 0, 1e-10, false) || !gstd::Double::equals(vars[i] / 1000, 1, 1e-10))
					{
						norm.reportFailure();
						return false;
					}
				}
				DpDoubles point = norm.get(0).result;
				DpDoubles comparePoint = norm.inServer.get(false)->get(0).result;
				for (int i = 0; i < dim; i++)
				{
					if (!gstd::Double::equals(point.content[i] * sqrt(norm.inputStds[i] * norm.inputStds[i] - norm.inputMeans[i] * norm.inputMeans[i]) + norm.inputMeans[i], comparePoint.content[i], 1e-10))
					{
						norm.reportFailure();
						return false;
					}
				}
				return true;
			}
			virtual std::string toString()
			{
				std::stringstream res;
				res << "This is a ml::RegressionNormalizer " << std::endl;
				res << "normalizeStd is: " << normalizeStd << std::endl;
				res << "normalizeMean is: " << normalizeMean << std::endl;
				res << "d is: " << d << std::endl;
				res << "inputStds is: " << gstd::Printer::vp(inputStds) << std::endl;
				res << "inputMeans is: " << gstd::Printer::vp(inputMeans) << std::endl;
				res << inServer.toString();
				res << data::Server<DpDoubles, headerType>::toString();
				return res.str();
			}

		};
	}
}
namespace msii810161816
{
	namespace gstd
	{
		template<typename headerType>
		struct TypeNameGetter<data::DpDoublesNormalizer<headerType> >
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "ml::DpDoublesNormalizer";
				t.parameters.push_back(TypeNameGetter<headerType>::get());
				return t;
			}
		};

	}
}

#endif	/* REGRESSIONNORMALIZER_H */

