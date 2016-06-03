#include <iostream>
#include <mex.h>
#undef printf

#include <MexMemoryInterfacing/Headers/GenericMexIO.hpp>
#include <Grid2D/MexHeaders/MexIO.hpp>
#include "../Headers/GetAttrBasin.hpp"

#include <DynamicalSystemInclude.hpp>

using namespace std;

void mexFunction(int           nlhs,           /* number of expected outputs */
                 mxArray       *plhs[],        /* array of pointers to output arguments */
                 int           nrhs,           /* number of inputs */
                 const mxArray *prhs[]         /* array of pointers to input arguments */) {

	if (nrhs != 1) {
		WriteException(ExOps::ExCodes::EXCEPTION_INVALID_INPUT,
		               "Incorrect number of input arguments. "
			           "Required number of input arguments = 1");
	}
	if (nlhs != 1 && nlhs != 2) {
		WriteException(ExOps::ExCodes::EXCEPTION_INVALID_INPUT,
		               "Incorrect number of output arguments. "
			           "Required number of output arguments = 1 or 2");
	}
	DynamicalSystem<DYNAMICAL_SYSTEM> DynamicalSystem(prhs[0]);
	auto attractorBasinRegions = getAttractorBasin(DynamicalSystem);

	// get boundaries of the attractorBasinRegions
	MexVector<Point, CAllocator> fullBasinBoundary = attractorBasinRegions.back().getMidwayBoundary();
	MexVector<MexVector<Point, CAllocator>> partitionBoundaries(attractorBasinRegions.size()-1);
	for(uint32_t i=0; i<partitionBoundaries.size(); ++i) {
		partitionBoundaries[i] = attractorBasinRegions[i].getMidwayBoundary();
	}

	plhs[0] = assignmxArray(fullBasinBoundary);
	if (nlhs == 2) {
		plhs[1] = assignmxArray(partitionBoundaries);
	}
}