// KernalCompiler.cpp : main project file.

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <msclr/marshal.h>
#include "cl.hpp"
#include <vector>

using namespace System;

enum class VendorTypeEnum
{
	NVidea,
	Intel,
	IntelExperimental,
	AMD
};

cl::Platform GetPlatform(VendorTypeEnum vendor);

int main(array<System::String ^> ^args)
{
	//First: select csv kernal files.
	System::String^ csvFileName = "SampleCSV.csv";
	array<System::String^>^ kernalFiles = System::IO::File::ReadAllLines(csvFileName);

	//Second: Choose your device. If not found then add it.
	cl::Platform currentPlatform = GetPlatform(VendorTypeEnum::AMD);
	cl_context_properties properties[] =
	{ CL_CONTEXT_PLATFORM,(cl_context_properties)((currentPlatform))(),0 };
	
	//Third: Change type based on selection
	cl::Context context = cl::Context(CL_DEVICE_TYPE_GPU, properties); 

	std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

	for each (System::String^ var in kernalFiles)
	{
		Console::WriteLine("Compiling for : " +  var);
		System::String^ fileContent = System::IO::File::ReadAllText(var);
		size_t source_size = fileContent->Length;


		msclr::interop::marshal_context oMarshalContext;
		const char* pFileContetn = oMarshalContext.marshal_as<const char*>(fileContent);

		cl::Program::Sources source(1, std::make_pair(pFileContetn, source_size));
		cl::Program program = cl::Program(context, source);
		program.build(devices);

		std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
		std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
		std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;


		Console::WriteLine(".................................");
	}

    Console::WriteLine(L"Completed...");
	Console::ReadLine();
    return 0;
}

cl::Platform GetPlatform(VendorTypeEnum vendor)
{
	cl::Platform currentPlatform;
	const char* platformNames[4] = { "NVIDIA CUDA","Intel(R) OpenCL", "Experimental OpenCL 2.1 CPU Only Platform","AMD Accelerated Parallel Processing" };

	const char* attributeNames[5] = { "Name","Vendor", "Version","Profile","Extensions" };
	const cl_platform_info attributeTypes[5] = { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR,CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE, CL_PLATFORM_EXTENSIONS };
	const int attributeCount = sizeof(attributeNames) / sizeof(char*);

	//get all platform ids
	std::vector<cl::Platform> cppPlatforms;
	cl::Platform::get(&cppPlatforms);

	for (int i = 0; i < cppPlatforms.size(); i++)
	{
		cl::STRING_CLASS infoString;
		cppPlatforms.at(i).getInfo(CL_PLATFORM_NAME, &infoString);

		if (!strcmp(infoString.c_str(), platformNames[(int)vendor]))
		{
			currentPlatform = cppPlatforms.at(i);
			break;
		}
	}

	std::cout << "Initialized platform having following details : " << std::endl;

	for (int j = 0; j < 5; j++)
	{
		cl::STRING_CLASS infoString;
		currentPlatform.getInfo(attributeTypes[j], &infoString);
		std::cout << attributeNames[j] << " : " << infoString << std::endl;
	}
	std::cout << "------------------------------------" << std::endl;
	return currentPlatform;
}
