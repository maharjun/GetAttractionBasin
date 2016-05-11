#ifndef LINEAR_REGION_HPP
#define LINEAR_REGION_HPP

#include <stdint.h>
#include <vector>

class LinearRegion {
private:
	uint32_t regionLenLim;
	std::vector<uint32_t> segmentBoundaries;

public:
	LinearRegion();
	explicit LinearRegion(uint32_t regionLenLim_);
	LinearRegion(const LinearRegion & LinearRegion_);

	inline uint32_t find(uint32_t val) const;
	inline void insert(uint32_t val);
	inline void resize(uint32_t NewLenLim);
	inline const std::vector<uint32_t> &getSegmentBoundaries() const;
	inline bool contains(uint32_t val) const;
};

#include "LinearRegion.inl"

#endif
