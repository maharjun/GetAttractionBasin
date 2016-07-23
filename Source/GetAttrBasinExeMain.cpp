#include <matrix.h>
#include <mat.h>
#include <mex.h>
#undef printf

#include <fstream>
#include <chrono>
#include <iostream>

#include <MexMemoryInterfacing/Headers/MexMem.hpp>
#include <MexMemoryInterfacing/Headers/GenericMexIO.hpp>
#include <Grid2D/Headers/RegionExc.hpp>

#ifdef _MSC_VER
#  define STRCAT_SAFE(a,b,c) strcat_s((a),(b),(c))
#elif defined __GNUC__
#  define STRCAT_SAFE(a,b,c) strncat((a),(c),(b))
#endif

using namespace std;

int main(int argc, char *args[]){
	// NOTE THAT THERE IS NO DATA VALIDATION AS THIS IS EXPECTED TO HAVE 
	// BEEN DONE IN THE MATLAB SIDE OF THE INTERFACE TO THIS MEX FUNCTION

	// Start Data Input from MAT File
	mxArray * Input = nullptr,
	        * fullBasinBoundary = nullptr,
	        * partitionBoundaries = nullptr;

	char *InputFilePath = nullptr;
	char *OutputFilePath = nullptr;

	// Process Input Arguments
	if (argc == 1) {
		WriteException(ExOps::EXCEPTION_INVALID_INPUT, "The path of the input file is not specified");
	}
	if (argc == 2) {
		InputFilePath = args[1];
		OutputFilePath = (char *)"OutputRegions.mat";
	}
	else {
		InputFilePath = args[1];
		OutputFilePath = args[2];
	}

	MATFile* InputFilePtr = matOpen(InputFilePath, "r");
	MATFile* OutputFilePtr = nullptr;
	Input = matGetVariable(InputFilePtr, "DynSysParams");

	matClose(InputFilePtr);
	if (Input == nullptr){
		WriteOutput("The variable name in the Input MAT File must be DynSysParams");
		return 0;
	}

	mxArrayPtr lhs[2] = { nullptr, nullptr };
	const mxArray *rhs[1] = { Input };

	// Confirm Output File Rewrite
	OutputFilePtr = matOpen(OutputFilePath, "r");
	while (OutputFilePtr){
		char UserConfirmResp;
		std::cout << "The following file already exists - \n" << std::endl
		          << "    " << OutputFilePath << std::endl
		          << "\nSure about rewrite? (y/n): " << std::flush;
		std::cin >> UserConfirmResp;
		if ((UserConfirmResp | 32) == 'y'){
			matClose(OutputFilePtr);
			OutputFilePtr = nullptr;
		}
		else if ((UserConfirmResp | 32) == 'n'){
			matClose(OutputFilePtr);
			OutputFilePtr = nullptr;
			cout << "KTHXBYE" << endl;

			std::cin.ignore(1024, '\n');
			std::cout << "Press enter to terminate simulation...";
			std::cin.get();

			mxDestroyArray(Input);
			return 0;
		}
	}

	// Test Output File writability
	OutputFilePtr = matOpen(OutputFilePath, "r");
	bool OutputFileAlreadyExists;
	if (OutputFilePtr == nullptr) {
		OutputFileAlreadyExists = false;
		OutputFilePtr = matOpen(OutputFilePath, "w");
	}
	else {
		OutputFileAlreadyExists = true;
		matClose(OutputFilePtr);
		OutputFilePtr = matOpen(OutputFilePath, "u");
	}
	if (OutputFilePtr == nullptr) {
		WriteOutput("The following path is not writable:\n    %s", OutputFilePath);
		mxDestroyArray(Input);
		return 0;
	}
	else {
		matClose(OutputFilePtr);
		OutputFilePtr = nullptr;
		if (!OutputFileAlreadyExists) {
			remove(OutputFilePath);
		}
	}

	// Execute Simulation via the Mex Interface
	try{
		mexFunction(2, lhs, 1, rhs);
	}
	catch (RegionException A){
		std::cout << "A Region related Exception Has Occurred" << std::endl;
		mxDestroyArray(Input);
		return 0;
	}

	fullBasinBoundary = lhs[0];
	partitionBoundaries = lhs[1];

	// Write Output into Output MAT File
	OutputFilePtr = matOpen(OutputFilePath, "wz");
	matPutVariable(OutputFilePtr, "FullBasinBoundary", fullBasinBoundary);
	matPutVariable(OutputFilePtr, "PartitionBoundaries", partitionBoundaries);
	matClose(OutputFilePtr);
	OutputFilePtr = nullptr;
	
	mxDestroyArray(Input);
	mxDestroyArray(fullBasinBoundary);
	mxDestroyArray(partitionBoundaries);

	std::cin.ignore(1024, '\n');
	std::cout << "Press enter to end simulation...";
	std::cin.get();
	return 0;
}