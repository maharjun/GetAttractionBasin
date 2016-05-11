#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "Point.hpp"
#include "math.h"

class CoordTransform {
	/* This class contains the static methods that are responsible for trans-
	 * formations between the grid  coordinate system and the coordinate sys-
	 * tem of the state space of the relevant dynamical system.
	 * 
	 * Functions contained:
	 * 
	 *   1. toDynSysPoint(const DoublePoint &gridPoint)
	 *   2. toGridPoint(const DoublePoint &dynSysPoint)
	 *   3. test()
	 * 
	 * It is the responsibility  of the creater of this  class to ensure that
	 * these two functions are actually the inverse of each other.
	 * 
	 * The test()  function is a  function that returns a  boolean value that
	 * represents whether the given CoordTransform is valid or not. It basic-
	 * aly applies the composition  of the above two  functions and checks if
	 * the resultant is identity or not. It is executed on the vector of test
	 * points specified inside the function itself
	 * 
	 */

public:
	static inline DoublePoint toDynSysPoint(const DoublePoint &gridPoint) {
		return DoublePoint();
	}
	static inline DoublePoint toGridPoint(const DoublePoint &dynSysPoint) {
		return DoublePoint();
	}
	static inline bool test() {
		std::vector<DoublePoint> TestGridPointVect = {
			DoublePoint(0,0),
			DoublePoint(1,0),
			DoublePoint(0,1),
			DoublePoint(2,3),
			DoublePoint(1.63,2.42)
		};
		bool isSuccessful = true;
		double eps = 1e-10;
		for(auto &TestGridPoint : TestGridPointVect) {
			auto ReturnPoint = toGridPoint(toDynSysPoint(TestGridPoint));
			if (
				abs(ReturnPoint.x-TestGridPoint.x) >= eps || 
				abs(ReturnPoint.y - TestGridPoint.y) >= eps) {
				isSuccessful = false;
				break;
			}
		}

		return isSuccessful;
	}
};

#endif
