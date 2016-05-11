#ifndef REGIONCOLUMN_INL
#define REGIONCOLUMN_INL

#include <limits.h>
#include <stdint.h>
#include "RegionExp.hpp"

// ##################################################################
// CONSTRUCTORS
// ##################################################################

inline LinearRegion::LinearRegion() : regionLenLim(UINT32_MAX), segmentBoundaries() {}

inline LinearRegion::LinearRegion(uint32_t regionLenLim_) : regionLenLim(regionLenLim_), segmentBoundaries() {}

inline LinearRegion::LinearRegion(const LinearRegion & LinearRegion_) :
	regionLenLim(LinearRegion_.regionLenLim),
	segmentBoundaries(LinearRegion_.segmentBoundaries) {}


// ##################################################################
// MEMBER FUNCTIONS
// ##################################################################

uint32_t LinearRegion::find(uint32_t val) const {
	/*
	 * This function returns the index i such that, a point with the given 'y'
	 * value and 'x' value equal to the column position lies either within or
	 * before the contiguous interval of points given by segmentBoundaries[i] to
	 * segmentBoundaries[i+1]
	 */

	// If debug, validate said point
	if (IS_DEBUG && val >= this->regionLenLim) {
		throw RegionException::INDEX_OUT_OF_LIMS;
	}

	// Search for this point in the given column
	bool isInsideInterval = false;
	bool isBeforeInterval = false;
	uint32_t IntervalIndex = 0;

	for(int i=0; i<segmentBoundaries.size(); i+=2) {
		if (segmentBoundaries[i] <= val && segmentBoundaries[i+1] > val) {
			isInsideInterval = true;
			IntervalIndex = i/2;
		}
		else if(segmentBoundaries[i] > val) {
			isBeforeInterval = true;
			IntervalIndex = i/2;
			break;
		}
	}

	if(!isInsideInterval && !isBeforeInterval) {
		// This means that the given y coordinate lies beyond any of the
		// currently contiguous regions. This means that we return the
		// length of segmentBoundaries/2
		IntervalIndex = segmentBoundaries.size()/2;
	}
	return IntervalIndex;
}

void LinearRegion::insert(uint32_t val) {
	/* 
	 * This function inserts the value val into the linear region.
	 * 
	 * It first checks for the contiguous section that either contains it
	 * or is the first section to be ahead of it. Then, if it is not con-
	 * tained in any existing section, it is added according to the foll-
	 * owing rules:
	 * 
	 * 1.  If it is adjacent to a single section  (i.e. either to the one 
	 *     before or after it,  the section that it is adjacent to is ex-
	 *     tended.
	 * 
	 * 2.  If it is the 'missing link' between two sections, the sections
	 *     are fused.
	 * 
	 * 3.  If it is not adjacent  to any section,  a new section containg
	 *     only this point is created.
	 */
	uint32_t IntervalIndex = this->find(val);
	if (IntervalIndex == segmentBoundaries.size()/2
	    || segmentBoundaries[2*IntervalIndex] > val) {
		// The above condition means that the point needs to be inserted
		// and is not already present

		bool isJustBeforeNextSeg = false;
		bool isJustAfterPrevSeg = false;

		if (!segmentBoundaries.empty()) {
			if (IntervalIndex > 0)
				isJustAfterPrevSeg = (segmentBoundaries[2*IntervalIndex-1] == val);
			if (IntervalIndex < segmentBoundaries.size()/2)
				isJustBeforeNextSeg = (segmentBoundaries[2*IntervalIndex] == val + 1);
		}

		if (isJustAfterPrevSeg && !isJustBeforeNextSeg) {
			segmentBoundaries[2*IntervalIndex-1]++;
		}
		else if (isJustBeforeNextSeg && !isJustAfterPrevSeg) {
			segmentBoundaries[2*IntervalIndex]--;
		}
		else if (isJustBeforeNextSeg && isJustAfterPrevSeg) {
			// in this case we need to fuse two regions
			segmentBoundaries.erase(
				segmentBoundaries.begin() + 2*IntervalIndex-1,
				segmentBoundaries.begin() + 2*IntervalIndex+1
			);
		}
		else{
			// In this case we need to add a new region.
			segmentBoundaries.insert(segmentBoundaries.begin() + 2*IntervalIndex, {val, val + 1});
		}
	}
}

void LinearRegion::resize(uint32_t NewLenLim)
{
	if (NewLenLim < this->regionLenLim) {
		// Will have to strip the points off
		// Calculate position of new last point in current region
		auto IntervalIndex = find(NewLenLim-1);
		uint32_t TerminationSize;

		// Find if the current point exists in interval
		if (segmentBoundaries[2 * IntervalIndex] <= NewLenLim - 1) {
			// in this case, the segment [SB[2I], SB[2I+1]) is partially
			// cutoff in the middle by NewLenLim and it now extends from
			// [SB[2I], NewLenLim-1] == [SB[2I], NewLenLim)
			segmentBoundaries[2 * IntervalIndex + 1] = NewLenLim;
			TerminationSize = 2 * IntervalIndex + 2;
		}
		else {
			// In this case, the last point does not belong to any segment
			// Thus, the IntervalIndex returned points to an interval beyond
			// the new allowed range. hence this interval and all subsequent
			// intervals are removed
			TerminationSize = 2 * IntervalIndex;
		}
		segmentBoundaries.resize(TerminationSize);
	}
	this->regionLenLim = NewLenLim;
}

inline const std::vector<uint32_t> & LinearRegion::getSegmentBoundaries() const
{
	return segmentBoundaries;
}

inline bool LinearRegion::contains(uint32_t val) const {
	auto IntervalIndex = this->find(val);
	return (segmentBoundaries[2 * IntervalIndex] <= val);
}
#endif
