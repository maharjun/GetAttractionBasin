#ifndef GETATTBASIN_DYNAMICALSYS
#define GETATTBASIN_DYNAMICALSYS

#include <matrix.h>
#include <Grid2D/Headers/LinearRegion.hpp>
#include <Grid2D/Headers/Point.hpp>
#include <Grid2D/Headers/Region.hpp>
#include <Grid2D/Headers/GridTransform.hpp>

/*
 * This file merely contains an incomplete declaration of a type that all dyn-
 * amical system classes must specialize. I.e. any valid DynamicalSystem must
 * be a specialization of this template and must at-least implement the methods
 * mentioned below.
 */

template <typename T>
class DynamicalSystem {
private:
	GridTransform<float> PrivateTransform;
	PointSet PrivateInitialPointSet;
	DiscreteRange XRange, YRange;
	// These constructors must be made public in any specialization
	// They are made private here so that an un-specialized DynamicalSystem
	// can never be instantiated.
	DynamicalSystem() : Transform(PrivateTransform), InitialPointSet(PrivateInitialPointSet) {}
	DynamicalSystem(const mxArray* ParamArray) : DynamicalSystem() {
		this->frommxArray(ParamArray);
	}

public:
	GridTransform<float> &Transform;
	const PointSet &InitialPointSet;
	inline void frommxArray(const mxArray *ParamArray);
	template<typename PointType>
	inline GenericPoint<PointType> simulateTimeStep(const GenericPoint<PointType> &PointNow);
	template<typename PointType>
	inline bool isAttracted(const GenericPoint<PointType> &PointNow);
	inline DiscreteRange getXLims();
	inline DiscreteRange getYLims();
};

#endif