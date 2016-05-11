#ifndef POINT_HPP
#define POINT_HPP

#include <stdint.h>
#include <vector>
#include <unordered_set>

template <typename T = uint32_t>
struct GenericPoint {
	T x;
	T y;

	bool operator<(const GenericPoint &P) const {
		return (x < P.x || x == P.x && y < P.y);
	}

	GenericPoint() = default;
	GenericPoint(T x_, T y_) : x(x_), y(y_) {}

	GenericPoint(const GenericPoint& other)
		: x(other.x),
		  y(other.y) {
	}

	template <typename T2>
	GenericPoint(const GenericPoint<T2>& other)
		: x(static_cast<T>(other.x)),
		  y(static_cast<T>(other.y)) {}

	GenericPoint& operator=(const GenericPoint& other) {
		x = other.x;
		y = other.y;
		return *this;
	}

	template <typename T2>
	GenericPoint& operator=(const GenericPoint& other) {
		x = static_cast<T>(other.x);
		y = static_cast<T>(other.y);
		return *this;
	}
};

typedef GenericPoint<uint32_t> Point;
typedef std::unordered_set<Point> PointSet;
typedef std::vector<Point> PointVect;

typedef GenericPoint<float> SinglePoint;
typedef GenericPoint<double> DoublePoint;

#endif