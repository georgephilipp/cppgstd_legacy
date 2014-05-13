#include "stdafx.h"

#include "DataHeaders.h"
#include "gstd/src/Vector.h"

namespace msii810161816
{
	namespace ml
	{
		InputGroupHeader::~InputGroupHeader() {}

		//Base package
		gstd::TypeName InputGroupHeader::getTypeName()
		{
			return gstd::TypeNameGetter<InputGroupHeader>::get();
		}
		void InputGroupHeader::setInputs() {}
		bool InputGroupHeader::test() { return true; }
		std::string InputGroupHeader::toString()
		{
			std::stringstream res;
			res << "This is a ml::InputGroupHeader" << std::endl;
			res << "Snippet from Input Groups are : " << gstd::Printer::vp(gstd::vector::sub(inputGroups, 0, 100, true)) << std::endl;
			res << data::DataHeader::toString();
			return res.str();
		}

		IOGroupHeader::~IOGroupHeader() {}

		//Base package
		gstd::TypeName IOGroupHeader::getTypeName()
		{
			return gstd::TypeNameGetter<IOGroupHeader>::get();
		}
		void IOGroupHeader::setInputs() {}
		bool IOGroupHeader::test()
		{
			InputGroupHeader input;
			input.log = { "bling", "blang" };
			input.safe = true;
			input.doc = true;
			input.description = "blung";
			input.inputGroups = { 1, 4, 10 };
			IOGroupHeader res = data::DhConverter<InputGroupHeader, IOGroupHeader>::get(input);
			if (res.log != std::vector<std::string>({ "bling", "blang" })
				|| !res.safe
				|| !res.doc
				|| res.description != "blung"
				|| res.inputGroups != std::vector<std::vector<int> >({ { 0 }, { 1, 2, 3 }, { 4, 5, 6, 7, 8, 9 } })
				|| (int)res.outputGroups.size() != 0
				)
			{
				reportFailure("Conversion from InputGroupHeader");
				return false;
			}
			return true;		
		}
		std::string IOGroupHeader::toString()
		{
			std::stringstream res;
			res << "This is a ml::IOGroupHeader" << std::endl;
			res << "Snippet from Input Groups are : " << std::endl;
			for (int i = 0; i < 10; i++)
			{
				if ((int)inputGroups.size() <= i)
					break;
				res << gstd::Printer::vp(gstd::vector::sub(inputGroups[i], 0, 30, true)) << std::endl;
			}
			res << "Snippet from Output Groups are : " << std::endl;
			for (int i = 0; i < 10; i++)
			{
				if ((int)outputGroups.size() <= i)
					break;
				res << gstd::Printer::vp(gstd::vector::sub(outputGroups[i], 0, 30, true)) << std::endl;
			}
			res << data::DataHeader::toString();
			return res.str();
		}
	}
}

namespace msii810161816
{
	namespace data
	{
		ml::IOGroupHeader DhConverter<ml::InputGroupHeader, ml::IOGroupHeader>::get(ml::InputGroupHeader input)
		{
			ml::IOGroupHeader res;
			res.log = input.log;
			res.description = input.description;
			res.safe = input.safe;
			res.doc = input.doc;
			int last = 0;
			for (int i = 0; i < (int)input.inputGroups.size(); i++)
			{
				std::vector<int> newGroup;
				for (int j = last; j < input.inputGroups[i]; j++)
					newGroup.push_back(j);
				res.inputGroups.push_back(newGroup);
				last = input.inputGroups[i];
			}
			return res;
		}
	}
}