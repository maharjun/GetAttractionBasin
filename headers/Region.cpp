#include <stdint.h>
#include <vector>
#include <algorithm>
#include "Point.hpp"
#include "Region.hpp"

// ##################################################################
// PRIVATE FUNCTIONS
// ##################################################################

void Region::addNonAddedPoints() const {
	if (!isRowColUpdated) {
		std::vector<Point> nonAddedPointsVect(nonAddedPoints.begin(), nonAddedPoints.end());
		std::sort(nonAddedPointsVect.begin(), nonAddedPointsVect.end());
		for (auto &Point : nonAddedPointsVect) {
			gridColumns[Point.x].insert(Point);
			gridRows[Point.y].insert(Point);
		}
		nonAddedPoints.clear();
		isRowColUpdated = true;
	}
}

void Region::updateInnerBoundary() const {
	if (!isInnerBoundaryUpdated) {
		innerBoundary.clear();

		for (auto &rowRegion : gridRows) {
			auto &rowSegBoundaries = rowRegion.getSegmentBoundaries();
			auto YCoord = rowRegion.getCoord();
			for (size_t i = 0; i < rowSegBoundaries.size(); i += 2) {
				innerBoundary.insert(Point(rowSegBoundaries[i], YCoord));
				innerBoundary.insert(Point(rowSegBoundaries[i + 1] - 1, YCoord));
			}
		}

		for (auto &colRegion : gridColumns) {
			auto &colSegBoundaries = colRegion.getSegmentBoundaries();
			auto XCoord = colRegion.getCoord();
			for (size_t i = 0; i < colSegBoundaries.size(); i += 2) {
				innerBoundary.insert(Point(XCoord, colSegBoundaries[i]));
				innerBoundary.insert(Point(XCoord, colSegBoundaries[i + 1] - 1));
			}
		}
		isInnerBoundaryUpdated = true;
	}
}

// ##################################################################
// PUBLIC FUNCTIONS
// ##################################################################

void Region::consolidate() const {
	/* This function forcefully performs all the update steps.  Currently the
	* following:
	*
	* 1.  updateInnerBoundary()
	* 2.  addNonAddedPoints()
	*
	* After these  steps all the data members will be fully represenative of
	* the region they represent
	*/
	updateInnerBoundary();
	addNonAddedPoints();
}

PointSet Region::getOuterBoundary() const
{
	consolidate();
	PointSet outerBoundary;
	// Getting the outer boundary points from all the grid columns
	for (auto &Column : gridColumns) {
		auto &ColumnSegments = Column.getSegmentBoundaries();
		for (uint32_t i = 0; i < ColumnSegments.size(); i+=2) {
			if(ColumnSegments[i] > 0)
				outerBoundary.insert(Point(Column.getCoord(), ColumnSegments[i] - 1));
			if(ColumnSegments[i+1] < gridYLim)
				outerBoundary.insert(Point(Column.getCoord(), ColumnSegments[i+1]));
		}
	}
	// Getting the outer boundary points from all the grid rows
	for(auto &Row: gridRows) {
		auto &RowSegments = Row.getSegmentBoundaries();
		for (uint32_t i = 0; i < RowSegments.size(); i+=2) {
			if (RowSegments[i] > 0)
				outerBoundary.insert(Point(RowSegments[i] - 1, Row.getCoord()));
			if (RowSegments[i + 1] < gridXLim)
				outerBoundary.insert(Point(RowSegments[i+1], Row.getCoord()));
		}
	}
	return outerBoundary;
}

PointVect Region::getMidwayBoundary() const {
}
