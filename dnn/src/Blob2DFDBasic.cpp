#include "stdafx.h"

#include "Blob2DFDBasic.h"
#include "gstd/src/Primitives.h"
#include "gstd/src/Linalg.h"
#include "gstd/src/Pointer.h"
#include "gstd/src/Vector.h"
#include "gstd/src/Stat.h"
#include <algorithm>


namespace msii810161816
{
	namespace dnn
	{
		Blob2DFDBasic::Blob2DFDBasic()
		{
			I = 0;
			J = 0;
			D1 = 0;
			D2 = 0;
		}

		Blob2DFDBasic::~Blob2DFDBasic()
		{

		}

		Blob* Blob2DFDBasic::construct()
		{
			return new Blob2DFDBasic;
		}

		void Blob2DFDBasic::copyData(Blob* target)
		{
			gstd::TypeName valType = target->getTypeName();
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(target);
				dCast->reset(I, J, D1, D2);
				dCast->setAll(content);
			}
			else
				gstd::error("cannot copy Blob2DFDBasic to blob of type " + valType.toString());
		}

		void Blob2DFDBasic::copy(Blob* target, std::string copyMode, bool isTop)
		{
			if (isTop)
			{
				gstd::Base::copy(target);
			}
			Blob::copy(target, copyMode, false);
			
			if (copyMode == "default")
			{
				copyData(target);
			}
			else if (copyMode == "empty")
			{

			}
			else
				gstd::error("unknown copyMode");
		}

		bool Blob2DFDBasic::equals(Blob* val, double margin, bool relative)
		{
			gstd::TypeName valType = val->getTypeName();
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);
		
				gstd::check(dCast != 0, "discovery of blob type failed");

				if (I != dCast->getI() || J != dCast->getJ() || D1 != dCast->getD1() || D2 != dCast->getD2())
					return false;

				if (gstd::Linalg::equals(content, dCast->getAll(), (float)margin, relative))
					return true;
				else
					return false;
			}
			else
				gstd::error("cannot add Blob2DFDBasic with blob of type " + valType.toString());

			return false;
		}

		void Blob2DFDBasic::swapData(Blob* val)
		{
			gstd::TypeName valType = val->getTypeName();
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);
				gstd::check(dCast != 0, "discovery of blob type failed");

				std::vector<float> temp = content;
				int tempI = I;
				int tempJ = J;
				int tempD1 = D1;
				int tempD2 = D2;
				reset(dCast->getI(), dCast->getJ(), dCast->getD1(), dCast->getD2());
				setAll(dCast->getAll());
				dCast->reset(tempI, tempJ, tempD1, tempD2);
				dCast->setAll(temp);
			}
			else
				gstd::error("cannot add Blob2DFDBasic with blob of type " + valType.toString());

			return;
		}

		void Blob2DFDBasic::allocate(Blob* val)
		{
			gstd::TypeName valType = val->getTypeName();
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);
				gstd::check(dCast != 0, "discovery of blob type failed");

				reset(dCast->getI(), dCast->getJ(), dCast->getD1(), dCast->getD2());
			}
			else
				gstd::error("cannot add Blob2DFDBasic with blob of type " + valType.toString());

			return;
		}

		void Blob2DFDBasic::transpose()
		{
			Blob2DFDBasic newContent;
			newContent.reset(J, I, D1, D2);
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							newContent.set(get(i, j, d1, d2), j, i, d1, d2);
						}
					}
				}
			}
			content = newContent.getAll();
			I = newContent.getI();
			J = newContent.getJ();
		}

		void Blob2DFDBasic::add(Blob* val)
		{
			gstd::TypeName valType = val->getTypeName();
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);
				gstd::check(dCast != 0, "discovery of blob type failed");

				gstd::check(I == dCast->getI(), "cannot add blobs with different I dimension");
				gstd::check(J == dCast->getJ(), "cannot add blobs with different J dimension");
				gstd::check(D1 == dCast->getD1(), "cannot add blobs with different D1 dimension");
				gstd::check(D2 == dCast->getD2(), "cannot add blobs with different D2 dimension");

				for (int i = 0; i < I; i++)
				{
					for (int j = 0; j < J; j++)
					{
						for (int d1 = 0; d1 < D1; d1++)
						{
							for (int d2 = 0; d2 < D2; d2++)
							{
								set(get(i, j, d1, d2) + dCast->get(i, j, d1, d2), i, j, d1, d2);
							}
						}
					}
				}
			}
			else
				gstd::error("cannot add Blob2DFDBasic with blob of type " + valType.toString());

			return;
		}

		void Blob2DFDBasic::subtract(Blob* val)
		{
			gstd::TypeName valType = val->getTypeName();
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);

				gstd::check(dCast != 0, "discovery of blob type failed");
				gstd::check(I == dCast->getI(), "cannot add blobs with different I dimension");
				gstd::check(J == dCast->getJ(), "cannot add blobs with different J dimension");
				gstd::check(D1 == dCast->getD1(), "cannot add blobs with different D1 dimension");
				gstd::check(D2 == dCast->getD2(), "cannot add blobs with different D2 dimension");

				for (int i = 0; i < I; i++)
				{
					for (int j = 0; j < J; j++)
					{
						for (int d1 = 0; d1 < D1; d1++)
						{
							for (int d2 = 0; d2 < D2; d2++)
							{
								set(get(i, j, d1, d2) - dCast->get(i, j, d1, d2), i, j, d1, d2);
							}
						}
					}
				}
			}
			else
				gstd::error("cannot subtract Blob2DFDBasic with blob of type " + valType.toString());

			return;
		}

		void Blob2DFDBasic::convolve(Blob* val, Blob* product)
		{
			gstd::TypeName valType = val->getTypeName();
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);

				gstd::check(dCast != 0, "discovery of blob type failed");
				gstd::check(J == dCast->getI(), "cannot multiply blobs with no matching inner dimension");

				int leftDim = I;
				int innerDim = J;
				int rightDim = dCast->getJ();
				int D1filter = dCast->getD1();
				int D2filter = dCast->getD2();
				int D1out = D1 - D1filter + 1;
				int D2out = D2 - D2filter + 1;

				gstd::check(D1out > 0, "cannot convolve a map of 1st dim " + gstd::Printer::p(D1) + " with a filter of size " + gstd::Printer::p(D1filter));
				gstd::check(D2out > 0, "cannot convolve a map of 2nd dim " + gstd::Printer::p(D2) + " with a filter of size " + gstd::Printer::p(D2filter));

				gstd::TypeName productType = product->getTypeName();
				if (productType.toString() == "dnn::Blob2DFDBasic")
				{
					Blob2DFDBasic* prodCast = dynamic_cast<Blob2DFDBasic*>(product);
					
					prodCast->reset(leftDim, rightDim, D1out, D2out);

					for (int left = 0; left < leftDim; left++)
					{
						for (int inner = 0; inner < innerDim; inner++)
						{
							for (int right = 0; right < rightDim; right++)
							{
								for (int d1out = 0; d1out < D1out; d1out++)
								{
									for (int d2out = 0; d2out < D2out; d2out++)
									{
										for (int d1filter = 0; d1filter < D1filter; d1filter++)
										{
											for (int d2filter = 0; d2filter < D2filter; d2filter++)
											{
												prodCast->set(prodCast->get(left, right, d1out, d2out) + get(left, inner, d1out + d1filter, d2out + d2filter)*dCast->get(inner, right, d1filter, d2filter), left, right, d1out, d2out);
											}
										}
									}
								}
							}
						}
					}
				}
				else
					gstd::error("cannot store product of Blob2DFDBasic in blob of type " + productType.toString());
			}
			else
				gstd::error("cannot multiply Blob2DFDBasic with blob of type " + valType.toString());

			return;
		}

		void Blob2DFDBasic::deconvolve(Blob* val, Blob* quotient)
		{
			gstd::TypeName valType = val->getTypeName();
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);

				gstd::check(dCast != 0, "discovery of blob type failed");
				gstd::check(J == dCast->getI(), "cannot multiply blobs with no matching inner dimension");

				int leftDim = I;
				int innerDim = J;
				int rightDim = dCast->getJ();
				int D1filter = dCast->getD1();
				int D2filter = dCast->getD2();
				int D1out = D1 + D1filter - 1;
				int D2out = D2 + D2filter - 1;

				gstd::TypeName productType = quotient->getTypeName();
				if (productType.toString() == "dnn::Blob2DFDBasic")
				{
					Blob2DFDBasic* quoCast = dynamic_cast<Blob2DFDBasic*>(quotient);

					quoCast->reset(leftDim, rightDim, D1out, D2out);

					for (int left = 0; left < leftDim; left++)
					{
						for (int inner = 0; inner < innerDim; inner++)
						{
							for (int right = 0; right < rightDim; right++)
							{
								for (int d1 = 0; d1 < D1; d1++)
								{
									for (int d2 = 0; d2 < D2; d2++)
									{
										for (int d1filter = 0; d1filter < D1filter; d1filter++)
										{
											for (int d2filter = 0; d2filter < D2filter; d2filter++)
											{
												quoCast->set(quoCast->get(left, right, d1 + d1filter, d2 + d2filter) + get(left, inner, d1, d2)*dCast->get(inner, right, d1filter, d2filter), left, right, d1 + d1filter, d2 + d2filter);
											}
										}
									}
								}
							}
						}
					}
				}
				else
					gstd::error("cannot store product of Blob2DFDBasic in blob of type " + productType.toString());
			}
			else
				gstd::error("cannot multiply Blob2DFDBasic with blob of type " + valType.toString());

			return;
		}

		void Blob2DFDBasic::elementwiseMultiply(Blob* val)
		{
			gstd::TypeName valType = val->getTypeName();
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);

				gstd::check(dCast != 0, "discovery of blob type failed");
				gstd::check(I == dCast->getI(), "cannot elementwiseMultiply blobs with different I dimension");
				gstd::check(J == dCast->getJ(), "cannot elementwiseMultiply blobs with different J dimension");
				gstd::check(D1 == dCast->getD1(), "cannot elementwiseMultiply blobs with different D1 dimension");
				gstd::check(D2 == dCast->getD2(), "cannot elementwiseMultiply blobs with different D2 dimension");

				for (int i = 0; i < I; i++)
				{
					for (int j = 0; j < J; j++)
					{
						for (int d1 = 0; d1 < D1; d1++)
						{
							for (int d2 = 0; d2 < D2; d2++)
							{
								set(get(i, j, d1, d2) * dCast->get(i, j, d1, d2), i, j, d1, d2);
							}
						}
					}
				}
			}
			else
				gstd::error("cannot elementwiseMultiply Blob2DFDBasic with blob of type " + valType.toString());

			return;
		}

		void Blob2DFDBasic::elementwiseDivide(Blob* val)
		{
			gstd::TypeName valType = val->getTypeName();
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);

				gstd::check(dCast != 0, "discovery of blob type failed");
				gstd::check(I == dCast->getI(), "cannot elementwiseDivide blobs with different I dimension");
				gstd::check(J == dCast->getJ(), "cannot elementwiseDivide blobs with different J dimension");
				gstd::check(D1 == dCast->getD1(), "cannot elementwiseDivide blobs with different D1 dimension");
				gstd::check(D2 == dCast->getD2(), "cannot elementwiseDivide blobs with different D2 dimension");

				for (int i = 0; i < I; i++)
				{
					for (int j = 0; j < J; j++)
					{
						for (int d1 = 0; d1 < D1; d1++)
						{
							for (int d2 = 0; d2 < D2; d2++)
							{
								set(get(i, j, d1, d2) / dCast->get(i, j, d1, d2), i, j, d1, d2);
							}
						}
					}
				}
			}
			else
				gstd::error("cannot elementwiseDivide Blob2DFDBasic with blob of type " + valType.toString());

			return;
		}

		void Blob2DFDBasic::scalarMax(double val)
		{
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							set(gstd::Double::gmax(get(i, j, d1, d2), (float)val), i, j, d1, d2);
						}
					}
				}
			}

			return;
		}

		void Blob2DFDBasic::scalarMultiply(double val)
		{
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							set(get(i, j, d1, d2) * (float)val, i, j, d1, d2);
						}
					}
				}
			}

			return;
		}

		void Blob2DFDBasic::scalarAdd(double val)
		{
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							set(get(i, j, d1, d2) + (float)val, i, j, d1, d2);
						}
					}
				}
			}

			return;
		}

		void Blob2DFDBasic::scalarPower(double val)
		{
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							set(pow(gstd::Double::abs(get(i, j, d1, d2)),(float)val), i, j, d1, d2);
						}
					}
				}
			}

			return;
		}

		void Blob2DFDBasic::exponential()
		{
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							set(exp(get(i, j, d1, d2)), i, j, d1, d2);
						}
					}
				}
			}

			return;
		}

		void Blob2DFDBasic::subtractFromScalar(double val)
		{
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							set((float)val - get(i, j, d1, d2), i, j, d1, d2);
						}
					}
				}
			}

			return;
		}

		void Blob2DFDBasic::distributeDatawiseSum()
		{
			for (int i = 0; i < I; i++)
			{
				float sum = 0;
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							sum += get(i, j, d1, d2);
						}
					}
				}
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							set(sum, i, j, d1, d2);
						}
					}
				}
			}

			return;
		}

		void Blob2DFDBasic::sigmoid()
		{
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							set(1 / (1 + exp(-get(i, j, d1, d2))), i, j, d1, d2);
						}
					}
				}
			}

			return;
		}

		double Blob2DFDBasic::norm(double exponent)
		{
			return (double)gstd::Linalg::norm(content, (float)exponent);
		}

		double Blob2DFDBasic::diffNorm(Blob* val, double exponent)
		{
			gstd::TypeName valType = val->getTypeName();
			double result = 0;
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);

				gstd::check(dCast != 0, "discovery of blob type failed");
				gstd::check(I == dCast->getI(), "cannot get diffnorm of blobs with different I dimension");
				gstd::check(J == dCast->getJ(), "cannot get diffnorm of blobs with different J dimension");
				gstd::check(D1 == dCast->getD1(), "cannot get diffnorm of blobs with different D1 dimension");
				gstd::check(D2 == dCast->getD2(), "cannot get diffnorm of blobs with different D2 dimension");

				for (int i = 0; i < I; i++)
				{
					for (int j = 0; j < J; j++)
					{
						for (int d1 = 0; d1 < D1; d1++)
						{
							for (int d2 = 0; d2 < D2; d2++)
							{
								result += pow(gstd::Double::abs((double)(get(i,j,d1,d2) - dCast->get(i,j,d1,d2))), exponent);
							}
						}
					}
				}

				result = pow(result, 1 / exponent);
			}
			else
				gstd::error("cannot subtract Blob2DFDBasic with blob of type " + valType.toString());
			
			return result;
		}

		double Blob2DFDBasic::dot(Blob* val)
		{
			gstd::TypeName valType = val->getTypeName();
			double result = 0;
			if (valType.toString() == "dnn::Blob2DFDBasic")
			{
				Blob2DFDBasic* dCast = dynamic_cast<Blob2DFDBasic*>(val);

				gstd::check(dCast != 0, "discovery of blob type failed");
				gstd::check(I == dCast->getI(), "cannot get dot product of blobs with different I dimension");
				gstd::check(J == dCast->getJ(), "cannot get dot product of blobs with different J dimension");
				gstd::check(D1 == dCast->getD1(), "cannot get dot product of blobs with different D1 dimension");
				gstd::check(D2 == dCast->getD2(), "cannot get dot product of blobs with different D2 dimension");

				for (int i = 0; i < I; i++)
				{
					for (int j = 0; j < J; j++)
					{
						for (int d1 = 0; d1 < D1; d1++)
						{
							for (int d2 = 0; d2 < D2; d2++)
							{
								result += get(i, j, d1, d2) * dCast->get(i, j, d1, d2);
							}
						}
					}
				}
			}
			else
				gstd::error("cannot subtract Blob2DFDBasic with blob of type " + valType.toString());

			return result;
		}

		double Blob2DFDBasic::sum()
		{
			double result = 0;
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							result += (double)get(i, j, d1, d2);
						}
					}
				}
			}

			return result;
		}

		int Blob2DFDBasic::getN()
		{
			return I;
		}

		void Blob2DFDBasic::resetN(int N, data::DpTensor<float> val)
		{
			gstd::check(val.dimensions.size() == 3, "cannot populate Blob2DFDBasic with a data point that is not a dimension 3 tensor");

			reset(N, val.dimensions[0], val.dimensions[1], val.dimensions[2]);
		}

		void Blob2DFDBasic::setn(int index, data::DpTensor<float> val)
		{
			gstd::check(val.dimensions.size() == 3, "cannot populate Blob2DFDBasic with a data point that is not a dimension 3 tensor");

			gstd::check(index < I, "cannot populate Blob2DFDBasic at an index beyond its size");
			gstd::check(J == val.dimensions[0], "cannot populate Blob2DFDBasic with a datapoint of mismatching dimension");
			gstd::check(D1 == val.dimensions[1], "cannot populate Blob2DFDBasic with a datapoint of mismatching dimension");
			gstd::check(D2 == val.dimensions[2], "cannot populate Blob2DFDBasic with a datapoint of mismatching dimension");
			gstd::check((int)val.content.size() == J*D1*D2, "invalid data point");

			std::copy(val.content.begin(), val.content.end(), content.begin() + index*J*D1*D2);
		}

		void Blob2DFDBasic::clear()
		{
			content.clear();
			I = 0;
			J = 0;
			D1 = 0;
			D2 = 0;
		}

		bool Blob2DFDBasic::isEmpty()
		{
			if (I == 0 && J == 0 && D1 == 0 && D2 == 0)
				return true;
			else
				return false;
		}

		void Blob2DFDBasic::setRandGaussian()
		{
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							set((float)gstd::stat::randnorm(2), i, j, d1, d2);
						}
					}
				}
			}
		}

		void Blob2DFDBasic::setContant(double val)
		{
			for (int i = 0; i < I; i++)
			{
				for (int j = 0; j < J; j++)
				{
					for (int d1 = 0; d1 < D1; d1++)
					{
						for (int d2 = 0; d2 < D2; d2++)
						{
							set((float)val, i, j, d1, d2);
						}
					}
				}
			}
		}

		void Blob2DFDBasic::reset(int _I, int _J, int _D1, int _D2)
		{
			gstd::check(_I > 0, "cannot set blob to have I dimension " + gstd::Printer::p(_I));
			gstd::check(_J > 0, "cannot set blob to have J dimension " + gstd::Printer::p(_J));
			gstd::check(_D1 > 0, "cannot set blob to have D1 dimension " + gstd::Printer::p(_D1));
			gstd::check(_D2 > 0, "cannot set blob to have D2 dimension " + gstd::Printer::p(_D2));

			content.clear();

			content.resize(_I*_J*_D1*_D2);
			I = _I;
			J = _J;
			D1 = _D1;
			D2 = _D2;
		}

		int Blob2DFDBasic::getI()
		{
			return I;
		}

		int Blob2DFDBasic::getJ()
		{
			return J;
		}

		int Blob2DFDBasic::getD1()
		{
			return D1;
		}

		int Blob2DFDBasic::getD2()
		{
			return D2;
		}

		float Blob2DFDBasic::get(int i, int j, int d1, int d2)
		{
			return content[d2 + D2*(d1 + D1*(j + J*i))];
		}

		void Blob2DFDBasic::set(float val, int i, int j, int d1, int d2)
		{
			content[d2 + D2*(d1 + D1*(j + J*i))] = val;
		}

		std::vector<float> Blob2DFDBasic::getAll()
		{
			return content;
		}

		void Blob2DFDBasic::setAll(std::vector<float> val)
		{
			gstd::check((int)val.size() == I*J*D1*D2, "cannot set Blob value to vector of incorrect size");

			content = val;
		}

		//Base package
		gstd::TypeName Blob2DFDBasic::getTypeName()
		{
			return gstd::TypeNameGetter<Blob2DFDBasic>::get();
		}
		void Blob2DFDBasic::setInputs()
		{
			id = "Blob2DFDBasicTest";
			reset(2, 2, 2, 2);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					for (int d1 = 0; d1 < 2; d1++)
					{
						for (int d2 = 0; d2 < 2; d2++)
						{
							set((float)(i + j + d1 + d2), i, j, d1, d2);
						}
					}
				}
			}
		}
		bool Blob2DFDBasic::test()
		{
			//reset, getI, getJ, getD1, getD2, get, set, getAll, setAll
			{
				Blob2DFDBasic thisBlob;
				thisBlob.reset(1, 2, 3, 4);
				if (thisBlob.getI() != 1 || thisBlob.getJ() != 2 || thisBlob.getD1() != 3 || thisBlob.getD2() != 4)
				{
					thisBlob.reportFailure("Failed test 1");
					return false;
				}
				if (thisBlob.getAll() != std::vector<float>(24, 0))
				{
					thisBlob.reportFailure("Failed test 2");
					return false;
				}
				thisBlob.set(1.2F, 0, 0, 0, 0);
				thisBlob.set(2.2F, 0, 1, 1, 1);
				if (thisBlob.get(0, 0, 0, 0) != 1.2F || thisBlob.get(0, 1, 1, 1) != 2.2F || thisBlob.get(0, 0, 2, 3) != 0.0F || thisBlob.get(0, 1, 2, 0) != 0.0F)
				{
					thisBlob.reportFailure("Failed test 3");
					return false;
				}
				if (thisBlob.getAll() != std::vector<float>({ 1.2F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 2.2F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F }))
				{
					thisBlob.reportFailure("Failed test 4");
					return false;
				}
				std::vector<float> newContent = { 4.0F, 1.24F, 4.25F, 5.14F, 2.53F, 3.12F, 5.23F, 1.12F, 1.52F, 1.42F, 1.53F, 1.31F, 4.0F, 1.24F, 4.25F, 5.14F, 2.53F, 3.12F, 5.23F, 1.12F, 1.52F, 1.42F, 1.53F, 1.31F };
				thisBlob.setAll(newContent);
				if (thisBlob.getAll() != newContent)
				{
					thisBlob.reportFailure("Failed test 5");
					return false;
				}
			}

			//resetN, getN, setn, clear, isEmpty
			{
				Blob2DFDBasic thisBlob;
				if (!thisBlob.isEmpty())
				{
					thisBlob.reportFailure("resetN/getN/setn/isEmpty, 0");
					return false;
				}
				data::DpTensor<float> dp1;
				dp1.dimensions = { 2, 1, 2 };
				dp1.content = { 3.2F, 1.4F, 5.4F, 0.3F };
				data::DpTensor<float> dp2;
				dp2.dimensions = { 2, 1, 2 };
				dp2.content = { 6.6F, 3.7F, 8.1F, 0.3F };
				data::DpTensor<float> dp3;
				dp3.dimensions = { 2, 1, 2 };
				dp3.content = { 1.1F, 2.2F, 3.3F, 0.3F };
				data::DpTensor<float> dpFalse1;
				dpFalse1.dimensions = { 2, 1, 1 };
				dpFalse1.content = { 3.2F, 5.4F };
				data::DpTensor<float> dpFalse2;
				dpFalse2.dimensions = { 2, 1, 2 };
				dpFalse2.content = { 3.2F, 1.4F, 5.4F };
				thisBlob.resetN(4, dp1);
				if (thisBlob.isEmpty())
				{
					thisBlob.reportFailure("resetN/getN/setn/isEmpty, 0.5");
					return false;
				}
				thisBlob.setn(1, dp1);
				thisBlob.setn(2, dp2);
				thisBlob.setn(3, dp3);
				std::vector<float> target = { 0.0F, 0.0F, 0.0F, 0.0F, 3.2F, 1.4F, 5.4F, 0.3F, 6.6F, 3.7F, 8.1F, 0.3F, 1.1F, 2.2F, 3.3F, 0.3F };
				if (thisBlob.getAll() != target || thisBlob.getN() != 4)
				{
					thisBlob.reportFailure("resetN/getN/setn/isEmpty, 1");
					return false;
				}
				gstd::Printer::c("expecting error message ...");
				try
				{
					thisBlob.setn(0, dpFalse1);
					thisBlob.reportFailure("resetN/getN/setn/isEmpty, 2");
					return false;
				}
				catch (std::exception e) {}
				gstd::Printer::c("expecting error message ...");
				try
				{
					thisBlob.setn(0, dpFalse2);
					thisBlob.reportFailure("resetN/getN/setn/isEmpty, 3");
					return false;
				}
				catch (std::exception e) {}
				gstd::Printer::c("expecting error message ...");
				try
				{
					thisBlob.setn(5, dp1);
					thisBlob.reportFailure("resetN/getN/setn/isEmpty, 4");
					return false;
				}
				catch (std::exception e) {}
				thisBlob.clear();
				if (thisBlob.getI() != 0 || thisBlob.getJ() != 0 || thisBlob.getD1() != 0 || thisBlob.getD2() != 0 || thisBlob.getAll() != std::vector<float>(0))
				{
					thisBlob.reportFailure("resetN/getN/setn/isEmpty, 5");
					return false;
				}
				if (!thisBlob.isEmpty())
				{
					thisBlob.reportFailure("resetN/getN/setn/isEmpty, 6");
					return false;
				}
			}

			//equals
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic clone;
				clone.setInputs();
				clone.set(clone.get(0, 1, 1, 1) + 2 * 1e-2F, 0, 1, 1, 1);
				if (!blob.equals(&clone, 1e-2, true) || clone.equals(&blob, 1e-2, false) || !clone.equals(&clone, 0, true))
				{
					clone.reportFailure("equals, 1");
					return false;
				}
				clone.reset(clone.getI()*clone.getJ(), 1, clone.getD1(), clone.getD2());
				clone.setAll(blob.getAll());
				if (blob.equals(&clone, 0, true))
				{
					clone.reportFailure("equals, 2");
					return false;
				}
			}

			//swapData
			{
				Blob2DFDBasic blob1;
				blob1.setInputs();
				Blob2DFDBasic blob1Copy;
				blob1Copy.setInputs();
				Blob2DFDBasic blob2;
				blob2.reset(1, 2, 3, 4);
				std::vector<float> other = { 1.0F, 5.3F, 6.6F, 7.2F, 2.0F, 3.56F, 5.7F, 7.3F, 23.8F, 23.7F,/*10*/ 3.6F, 4.7F, 5.5F, 3.8F, 5.3F, 2.6F, 7.7F, 8.4F, 6.3F, 7.2F,/*20*/ 7.5F, 6.6F, 4.1F, 2.8F };
				blob2.setAll(other);
				Blob2DFDBasic blob2Copy;
				blob2Copy.reset(1, 2, 3, 4);
				blob2Copy.setAll(other);
				blob1.swapData(&blob2);
				if (!blob1.equals(&blob2Copy, 0, true) || !blob2.equals(&blob1Copy, 0, true))
				{
					blob1.reportFailure("swapData, 1");
					return false;
				}
				blob2.swapData(&blob1);
				if (!blob1Copy.equals(&blob1, 0, true) || !blob2Copy.equals(&blob2, 0, true))
				{
					blob1.reportFailure("swapData, 2");
					return false;
				}

			}

			//allocate
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic blob2;
				blob2.allocate(&blob);
				std::vector<float> target(16, 0.0F);
				if (blob.getI() != blob2.getI() || blob.getI() != blob2.getI() || blob.getI() != blob2.getI() || blob.getI() != blob2.getI() || blob2.getAll != target)
				{
					blob2.reportFailure("allocate");
					return false;
				}
			}

			//construct, copyData and copy
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				gstd::Pointer<Blob2DFDBasic> clone;
				clone.set(dynamic_cast<Blob2DFDBasic*>(blob.construct()), true);
				blob.copyData(clone.get(false));
				if (!blob.equals(clone.get(false), 0, true))
				{
					clone.reportFailure("clone, 1");
					return false;
				}
				clone.get(false)->clear();
				blob.copy(clone.get(false), "empty");
				if (!clone.get(false)->isEmpty())
				{
					clone.reportFailure("clone, 2");
					return false;
				}
				blob.copy(clone.get(false), "default");
				if (!blob.equals(clone.get(false), 0, true))
				{
					clone.reportFailure("clone, 3");
					return false;
				}
			}

			//transpose
			{
				Blob2DFDBasic blob;
				blob.reset(2, 3, 2, 3);
				std::vector<float> input = {3.2F, 2.4F, 1.0F, 1.6F, 6.4F, 8.1F, 2.1F, 3.3F, 8.0F, 9.9F, 1.1F, 1.2F, 3.2F, 4.1F, 5.7F, 8.8F, 1.6F, 3.4F, 6.4F, 7.6F, 7.8F, 3.3F, 4.2F, 5.7F, 6.2F, 6.0F, 0.4F, 1.5F, 2.1F, 3.7F, 8.6F, 5.8F, 4.6F, 0.1F, 1.9F, 2.7F};
				blob.setAll(input);
				Blob2DFDBasic targetBlob;
				std::vector<float> target = { 3.2F, 2.4F, 1.0F, 1.6F, 6.4F, 8.1F, 6.4F, 7.6F, 7.8F, 3.3F, 4.2F, 5.7F, 2.1F, 3.3F, 8.0F, 9.9F, 1.1F, 1.2F, 6.2F, 6.0F, 0.4F, 1.5F, 2.1F, 3.7F, 3.2F, 4.1F, 5.7F, 8.8F, 1.6F, 3.4F, 8.6F, 5.8F, 4.6F, 0.1F, 1.9F, 2.7F };
				targetBlob.reset(3, 2, 2, 3);
				targetBlob.setAll(target);
				blob.transpose();
				if (!targetBlob.equals(&blob, 1e-6, true))
				{
					blob.reportFailure("transpose");
					return false;
				}
			}

			//add and subtract
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic clone;
				clone.setInputs();
				clone.set(clone.get(0, 1, 1, 1) + 2 * 1e-2F, 0, 1, 1, 1);
				clone.add(&blob);
				Blob2DFDBasic targetBlob;
				std::vector<float> target = { 0.0F, 2.0F, 2.0F, 4.0F, 2.0F, 4.0F, 4.0F, 6.02F, 2.0F, 4.0F, 4.0F, 6.0F, 4.0F, 6.0F, 6.0F, 8.0F };
				targetBlob.reset(2, 2, 2, 2);
				targetBlob.setAll(target);
				if (!targetBlob.equals(&clone, 1e-6, true))
				{
					clone.reportFailure("add");
					return false;
				}
				clone.subtract(&blob);
				clone.set(3.0F, 0, 1, 1, 1);
				if (!blob.equals(&clone, 1e-6, true))
				{
					clone.reportFailure("subtract");
					return false;
				}
			}

			//addOrCopy
			{
				Blob2DFDBasic blob;
				Blob2DFDBasic blob2;
				blob.setInputs();
				blob2.addOrCopy(&blob);
				blob2.clear();
				blob2.addOrCopy(&blob);
				blob2.addOrCopy(&blob);
				blob.add(&blob);
				if (!blob.equals(&blob2, 1e-6, true))
				{
					blob2.reportFailure("addOrCopy");
					return false;
				}
			}

			//convolve
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic blob2;
				blob2.reset(3, 2, 3, 3);
				std::vector<float> input1 = { 1.0F, 2.0F, 0.5F, 1.5F, 2.0F, 1.0F, 1.0F, 2.5F, 0.0F, /*9*/0.5F, 1.0F, 2.0F, 1.0F, 0.5F, 2.1122F, 2.0F, 0.5F, 1.5F,/*18*/ 2.0F, 1.0F, 1.0F, 1.5F, 0.5F, 0.0F, 1.0F, 1.0F, -1.0F,/*27*/ -3.0F, 3.0F, -2.0F, 1.0F, 1.0F, 1.0F, -2.0F, 3.0F, -1.5F,/*36*/0.0F, 0.5F, 3.0F, -3.0F, 1.0F, 1.0F, -1.0F, -2.0F, 2.5F, /*45*/2.0F, 2.0F, 1.0F, -1.0F, -1.0F, 0.0F, 0.5F, 1.0F, 0.3542F };
				blob2.setAll(input1);
				Blob2DFDBasic targetBlob;
				targetBlob.reset(3, 2, 2, 2);
				std::vector<float> target = { 13.5F, 16.8366F, 15.5F, 13.7244F, 23.0F, 27.9488F, 26.5F, 23.8366F, 11.5F, 5.5F, 11.5F, 3.5F, 18.5F, 11.0F, 18.5F, 7.5F, 0.5F, 8.0F, -3.0F, 6.0626F, 1.0F, 15.5F, -8.5F, 8.9168F };
				targetBlob.setAll(target);
				Blob2DFDBasic blob3;
				blob2.convolve(&blob, &blob3);
				if (!targetBlob.equals(&blob3, 1e-5F, true))
				{
					blob3.reportFailure("convolve");
					return false;
				}
			}

			//deconvolve
			{
				Blob2DFDBasic blob1;
				Blob2DFDBasic blob2;
				Blob2DFDBasic blob3;
				Blob2DFDBasic targetBlob;
				std::vector<float> input1 = {/*map1*/1.5F, -0.5F, 0.5F, -1.0F,/*map2*/ 3.0F, 2.5F, 0.5F, -0.112F, /*map3*/0.5F, 2.5F, -1.5F, -0.5F,/*map4*/ 3.0F, -1.0F, -1.0F, 2.5F, /*map5*/2.43F, 0.5F, 1.5F, 1.0F,/*map6*/ -2.0F, -1.5F, 2.5F, 0.5F };
				std::vector<float> input2 = {/*map1*/ 1.0F, 2.0F,/*map2*/ 1.0F, 0.0F,/*map3*/ 0.5F, 1.5F, /*map4*/2.0F, 1.0F };
				std::vector<float> target = {/*map1*/3.0F, 8.25F, 2.75F, 0.75F, 0.694F, -2.168F, /*map2*/ 7.5F, 7.5F, 2.5F, 1.5F, -0.724F, -0.112F, /*map3*/ 2.0F, 7.5F, 3.5F, -2.0F, -3.75F, 2.75F, /*map4*/ 6.5F, 3.5F, -1.0F, -3.5F, 3.5F, 2.5F, /*map5*/ 1.43F, 1.61F, -1.25F, 2.75F, 8.0F, 2.75F, /*map6*/ -1.57F, -4.5F, -1.5F, 6.5F, 4.5F, 0.5F };
				blob1.reset(3, 2, 2, 2);
				blob1.setAll(input1);
				blob2.reset(2, 2, 1, 2);
				blob2.setAll(input2);
				targetBlob.reset(3, 2, 2, 3);
				targetBlob.setAll(target);
				blob1.deconvolve(&blob2, &blob3);
				if (!targetBlob.equals(&blob3, 1e-5, true))
				{
					blob3.reportFailure("deconvolve");
					return false;
				}
			}

			//elementwiseMultiply
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				blob.elementwiseMultiply(&blob);
				Blob2DFDBasic targetBlob;
				std::vector<float> target = { 0.0F, 1.0F, 1.0F, 4.0F, 1.0F, 4.0F, 4.0F, 9.0F, 1.0F, 4.0F, 4.0F, 9.0F, 4.0F, 9.0F, 9.0F, 16.0F };
				targetBlob.reset(2, 2, 2, 2);
				targetBlob.setAll(target);
				if (!targetBlob.equals(&blob, 1e-6, true))
				{
					blob.reportFailure("elementwiseMultiply");
					return false;
				}
			}

			//elementwiseDivide
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic blob1;
				blob1.reset(2, 2, 2, 2);
				std::vector<float> input1 = { 5.0F, -2.0F, 0.5F, 0.25F, -0.5F, 4.0F, 10.0F, -8.0F, 1.25F, 5.0F, 0.25F, 0.5F, 4.0F, -10.0F, 1.5F, 0.2F};
				blob1.setAll(input1);
				blob.elementwiseDivide(&blob1);
				Blob2DFDBasic targetBlob;
				std::vector<float> target = { 0.0F, -0.5F, 2.0F, 8.0F, -2.0F, 0.5F, 0.2F, -0.375F, 0.8F, 0.4F, 8.0F, 6.0F, 0.5F, -0.3F, 2.0F, 20.0F };
				targetBlob.reset(2, 2, 2, 2);
				targetBlob.setAll(target);
				if (!targetBlob.equals(&blob, 1e-6, true))
				{
					blob.reportFailure("elementwiseDivide");
					return false;
				}
			}

			//scalarMax
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic targetBlob;
				std::vector<float> target = { 1.43F, 1.43F, 1.43F, 2.0F, 1.43F, 2.0F, 2.0F, 3.0F, 1.43F, 2.0F, 2.0F, 3.0F, 2.0F, 3.0F, 3.0F, 4.0F };
				targetBlob.reset(2, 2, 2, 2);
				targetBlob.setAll(target);
				blob.scalarMax(1.43F);
				if (!targetBlob.equals(&blob, 0, true))
				{
					blob.reportFailure("scalarMax");
					return false;
				}
			}

			//scalarMultiply
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic targetBlob;
				std::vector<float> target = { 0.0F, 1.5F, 1.5F, 3.0F, 1.5F, 3.0F, 3.0F, 4.5F, 1.5F, 3.0F, 3.0F, 4.5F, 3.0F, 4.5F, 4.5F, 6.0F };
				targetBlob.reset(2, 2, 2, 2);
				targetBlob.setAll(target);
				blob.scalarMultiply(1.5);
				if (!targetBlob.equals(&blob, 1e-6, true))
				{
					blob.reportFailure("scalarMultiply");
					return false;
				}
			}

			//scalarAdd
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic targetBlob;
				targetBlob.reset(2, 2, 2, 2);
				std::vector<float> target = { 0.431F, 1.431F, 1.431F, 2.431F, 1.431F, 2.431F, 2.431F, 3.431F, 1.431F, 2.431F, 2.431F, 3.431F, 2.431F, 3.431F, 3.431F, 4.431F };
				targetBlob.setAll(target);
				blob.scalarAdd(0.431);
				if (!targetBlob.equals(&blob, 1e-6, true))
				{
					blob.reportFailure("scalarAdd");
					return false;
				}
			}

			//scalarPower
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				blob.scalarAdd(-1.3);
				blob.scalarPower(1.4);
				Blob2DFDBasic targetBlob;
				targetBlob.reset(2, 2, 2, 2);
				std::vector<float> target = { 1.4438453988846F, 0.18534025517022F, 0.18534025517022F, 0.60692811506679F, 0.18534025517022F, 0.60692811506679F, 0.60692811506679F, 2.1019795666252F, 0.18534025517022F, 0.60692811506679F, 0.60692811506679F, 2.1019795666252F, 0.60692811506679F, 2.1019795666252F, 2.1019795666252F, 4.0170687990944F };
				targetBlob.setAll(target);
				if (!targetBlob.equals(&blob, 1e-5, true))
				{
					blob.reportFailure("scalarPower");
					return false;
				}
			}

			//exponential
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				blob.exponential();
				Blob2DFDBasic targetBlob;
				targetBlob.reset(2, 2, 2, 2);
				std::vector<float> target = { 1.0F, 2.71828182845905F, 2.71828182845905F, 7.38905609893065F, 2.71828182845905F, 7.38905609893065F, 7.38905609893065F, 20.0855369231877F, 2.71828182845905F, 7.38905609893065F, 7.38905609893065F, 20.0855369231877F, 7.38905609893065F, 20.0855369231877F, 20.0855369231877F, 54.5981500331442F };
				targetBlob.setAll(target);
				if (!targetBlob.equals(&blob, 1e-5, true))
				{
					blob.reportFailure("exponential");
					return false;
				}
			}

			//subtractFromScalar
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic targetBlob;
				targetBlob.reset(2, 2, 2, 2);
				std::vector<float> target = { -0.431F, -1.431F, -1.431F, -2.431F, -1.431F, -2.431F, -2.431F, -3.431F, -1.431F, -2.431F, -2.431F, -3.431F, -2.431F, -3.431F, -3.431F, -4.431F };
				targetBlob.setAll(target);
				blob.subtractFromScalar(-0.431);
				if (!targetBlob.equals(&blob, 1e-6, true))
				{
					blob.reportFailure("scalarAdd");
					return false;
				}
			}

			//distributeDatawiseSum
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic targetBlob;
				targetBlob.reset(2, 2, 2, 2);
				std::vector<float> target = { 12.0F, 12.0F, 12.0F, 12.0F, 12.0F, 12.0F, 12.0F, 12.0F, 20.0F, 20.0F, 20.0F, 20.0F, 20.0F, 20.0F, 20.0F, 20.0F};
				targetBlob.setAll(target);
				blob.distributeDatawiseSum();
				if (!targetBlob.equals(&blob, 1e-6F, true))
				{
					blob.reportFailure("distributeDatawiseSum");
					return false;
				}
			}

			//sigmoid
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic targetBlob;
				targetBlob.reset(2, 2, 2, 2);
				std::vector<float> target = { 0.5F, 0.731058579F, 0.731058579F, 0.880797077977882F, 0.731058579F, 0.880797077977882F, 0.880797077977882F, 0.952574126822433F, 0.731058579F, 0.880797077977882F, 0.880797077977882F, 0.952574126822433F, 0.880797077977882F, 0.952574126822433F, 0.952574126822433F, 0.982013790037908F };
				targetBlob.setAll(target);
				blob.sigmoid();
				if (!targetBlob.equals(&blob, 1e-5F, true))
				{
					blob.reportFailure("sigmoid");
					return false;
				}
			}

			//norm
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				if (!gstd::Double::equals(blob.norm(3.42), 5.563661927, 1e-5))
				{
					blob.reportFailure("norm");
					return false;
				}
			}

			//diffNorm
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic blob2;
				blob2.reset(2, 2, 2, 2);
				std::vector<float> input2 = { 0.5F, 0.731058579F, 0.731058579F, 0.880797077977882F, 0.731058579F, 0.880797077977882F, 0.880797077977882F, 0.952574126822433F, 0.731058579F, 0.880797077977882F, 0.880797077977882F, 0.952574126822433F, 0.880797077977882F, 0.952574126822433F, 0.952574126822433F, 0.982013790037908F };
				blob2.setAll(input2);
				Blob2DFDBasic blobDiff;
				blobDiff.setInputs();
				blobDiff.subtract(&blob2);
				if (!gstd::Double::equals(blobDiff.norm(1.42), blob2.diffNorm(&blob, 1.42), 1e-5, true) || !gstd::Double::equals(blobDiff.norm(2.45), blob.diffNorm(&blob2, 2.45), 1e-5, true))
				{
					blob.reportFailure("diffNorm");
					return false;
				}
			}

			//dot
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				Blob2DFDBasic blob2;
				blob2.reset(2, 2, 2, 2);
				std::vector<float> input2 = { 2.3F, 5.2F, 6.3F, -1.1F, 9.4F, -2.6F, 6.3F, 2.2F, 1.6F, -1.3F, -5.7F, -0.3F, 1.5F, 1.6F, 4.4F, 0.1F };
				blob2.setAll(input2);
				if (!gstd::Double::equals(blob.dot(&blob2), 40.8, 1e-6, true))
				{
					blob.reportFailure("dot");
					return false;
				}
			}

			//sum
			{
				Blob2DFDBasic blob;
				blob.setInputs();
				blob.set(1.54256F, 0, 0, 0, 0);
				if (!gstd::Double::equals(blob.sum(), 33.54256, 1e-6, true))
				{
					blob.reportFailure("sum");
					return false;
				}
			}

			//setRandGaussian
			{
				Blob2DFDBasic blob;
				blob.reset(10, 10, 10, 10);
				blob.setRandGaussian();
				double variance = blob.dot(&blob) / 10000;
				double mean = blob.sum() / 10000;
				if (mean < -0.1 && mean > 0.1)
				{
					blob.reportFailure("setRandGaussian, 1");
					return false;
				}
				if (variance < 0.9 && variance > 1.1)
				{
					blob.reportFailure("setRandGaussian, 2");
					return false;
				}
			}

			//setConstant
			{
				Blob2DFDBasic blob;
				blob.reset(1, 2, 3, 4);
				blob.setContant(3.4);
				Blob2DFDBasic targetBlob;
				targetBlob.reset(1, 2, 3, 4);
				std::vector<float> target(24, 3.4F);
				targetBlob.setAll(target);
				if (!targetBlob.equals(&blob, 1e-6, true))
				{
					blob.reportFailure("setConstant");
					return false;
				}
			}
			
			return true;
		}
		std::string Blob2DFDBasic::toString()
		{
			std::stringstream res;
			res << "This is dnn::Blob2DFBasic " << std::endl;
			res << "I: " << I << std::endl;
			res << "J: " << J << std::endl;
			res << "D1: " << D1 << std::endl;
			res << "D2: " << D2 << std::endl;
			res << "sneak peek of content: " << gstd::Printer::vp(gstd::vector::sub(content, 0, 50, true)) << std::endl;
			return res.str();
		}
	}
}