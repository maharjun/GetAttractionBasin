#ifndef REGION_HPP
#define REGION_HPP

#include <stdint.h>
#include <vector>

#include "Point.hpp"
#include "RegionExp.hpp"
#include "LinearRegion.hpp"

class ColumnRegion : public LinearRegion {
private:
	uint32_t columnCoord;
	inline void ValidatePoint(const Point &P) const {
		// If debug, validate said point
		if (IS_DEBUG && P.x != this->columnCoord) {
			throw RegionException::COLUMN_COORD_NOT_MATCHED;
		}
	}
public:
	ColumnRegion() : columnCoord(UINT32_MAX) {}
	ColumnRegion(uint32_t regionCoord_, uint32_t regionLenLim_) : LinearRegion(regionLenLim_)
	{
		columnCoord = regionCoord_;
	}
	inline void insert(const Point &P) {
		ValidatePoint(P);
		LinearRegion::insert(P.y);
	}
	inline uint32_t getCoord() const
	{
		return columnCoord;
	}
	inline bool find(const Point& P) const {
		ValidatePoint(P);
		return LinearRegion::find(P.y);
	}
};

struct RowRegion : LinearRegion {
private:
	uint32_t rowCoord;
	inline void ValidatePoint(const Point &P) const {
		// If debug, validate said point
		if (IS_DEBUG && P.y != this->rowCoord) {
			// ReSharper restore CppRedundantBooleanExpressionArgument
			throw RegionException::ROW_COORD_NOT_MATCHED;
		}
	}
public:
	RowRegion() : rowCoord(UINT32_MAX) {}
	RowRegion(uint32_t regionCoord_, uint32_t regionLenLim_) : LinearRegion(regionLenLim_)
	{
		rowCoord = regionCoord_;
	}
	inline void insert(const Point &P) {
		ValidatePoint(P);
		LinearRegion::insert(P.x);
	}
	inline uint32_t getCoord() const
	{
		return rowCoord;
	}
	inline bool find(const Point &P) const {
		ValidatePoint(P);
		return LinearRegion::find(P.x);
	}
};

struct Region{
private:
	uint32_t gridXLim;
	uint32_t gridYLim;
	mutable std::vector<ColumnRegion> gridColumns;
	mutable std::vector<RowRegion> gridRows;
	mutable PointSet nonAddedPoints;
	mutable PointSet innerBoundary;
	
	// The below are status variables used for lazy updating
	mutable bool isRowColUpdated;
	mutable bool isInnerBoundaryUpdated;

	inline void initGridRowColumns() {
		for (uint32_t x = 0; x < gridXLim; ++x) {
			gridColumns[x] = ColumnRegion(x, gridYLim);
		}
		for (uint32_t y = 0; y < gridYLim; ++y) {
			gridRows[y] = RowRegion(y, gridXLim);
		}
	}

	void addNonAddedPoints() const;
	void updateInnerBoundary() const;
	
public:
	
	Region() : 
		gridXLim               (0),
		gridYLim               (0), 
		gridColumns            (),
		gridRows               (), 
		nonAddedPoints         (),
		innerBoundary          (),
		isRowColUpdated        (true),
		isInnerBoundaryUpdated (true) {}

	Region(uint32_t gridXLim_, uint32_t gridYLim_) : Region() {
		gridXLim    = gridXLim_;
		gridYLim    = gridYLim_;
		gridColumns = std::vector<ColumnRegion>(gridXLim_);
		gridRows    = std::vector<RowRegion>(gridYLim_);
		this->initGridRowColumns();
	}

	Region(const Region& other) :
		gridXLim               (other.gridXLim),
		gridYLim               (other.gridYLim),
		gridColumns            (other.gridColumns),
		gridRows               (other.gridRows),
		nonAddedPoints         (other.nonAddedPoints),
		innerBoundary          (other.innerBoundary),
		isRowColUpdated        (other.isRowColUpdated),
		isInnerBoundaryUpdated (other.isInnerBoundaryUpdated) {}

	inline const PointSet & getInnerBoundary() const {
		consolidate();
		return this->innerBoundary;
	}
	void consolidate() const;
	inline void insert(const Point& point2Insert) {
		this->nonAddedPoints.insert(point2Insert);
	}
	inline bool contains(const Point& point2Search) const {
		bool isContained = nonAddedPoints.count(point2Search);
		if (!isContained) {
			isContained = gridColumns[point2Search.x].find(point2Search);
		}
		return isContained;
	}
	PointSet getOuterBoundary() const;
	PointVect getMidwayBoundary() const;

	/*
		 * This describes the algorithm in detail.
		 *
		 * Every Region has 3 boundaries. The boundary is either an inner
		 * boundary,  outer boundary, or edge boundary.  They are defined 
		 * below:
		 * 
		 * Inner Boundary -
		 * 
		 *   This boundary is the set of points in the regionfor which at
		 *   least one of the  horiz/vert (HV) neighbors  does not lie in 
		 *   the region.  This boundary is a property  (i.e. data member)
		 *   of the class and is updated lazily upon get.
		 *   
		 * Outer Boundary -
		 * 
		 *   This is the set of all points outside the region one of whose
		 *   HV Neighbors lies inside the region
		 * 
		 * Edge Boundary -
		 * 
		 *   This is the set of half points  that define the polygon which
		 *   forms the boundary  of the area represented  by the region if
		 *   we draw  each point as a square of side length 1  centered at
		 *   the given point. This is the most time consuming to calculate
		 *   and is ideally done only at the time of plotting.  Also, this
		 *   is the only boundary that is unambiguously 'traceable',  i.e.
		 *   given  the set of points  on the boundary,  we can decide the
		 *   unique ordering of the points which constitute a clockwise or
		 *   anticlockwise trace around the boundary
		 */
};

#endif