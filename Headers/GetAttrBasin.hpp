#include "DynamicalSystem.hpp"
#include <Grid2D/Headers/Region.hpp>
#include <vector>

template<typename T>
std::vector<Region> getAttractorBasin(DynamicalSystem<T> &DynSystem) {
	/*
	 * The Algorithm implemented here is as follows:
	 *
	 *     for each point on the outer boundary:
	 *         simulate one timeStep, and get NextTimePoint
	 *         if NextTimePoint in attBasinPartition[j] for any existing j:
	 *             add point to attBasinPartition[j+1] (create if it doesnt exist)
	 *             add point to FullAttractorBasin.
	 *         elseif NextTimePoint in outOfBoundsRegion or NextTimePoint out of grid:
	 *             add point in outOfBoundsRegion.
	 *
	 *     for each region in attBasinPartition:
	 *         push region.midwayBoundary into regionBoundaries
	 *
	 *     return FullAttractorBasin.boundary and regionBoundaries.
	 */

	// Initialize the various relevant regions
	uint32_t XLim = DynSystem.getXLims().endPoint;
	uint32_t YLim = DynSystem.getYLims().endPoint;
	
	Region fullAttractorBasin(XLim, YLim);
	std::vector<Region> attBasinPartition(1, Region(XLim, YLim));
	Region outOfBoundsRegion(XLim, YLim);

	// Get the initial outerBoundary;
	PointSet CurrentOuterBoundary;
	CurrentOuterBoundary = DynSystem.InitialPointSet;
	uint32_t nNewPointsAdded;

	do {
		// This loop runs until, in a particular iteration, no new points were added
		nNewPointsAdded = 0;

		// Filter out the points that are in the inner boundary of
		// outOfBoundsRegion
		auto &outOfBoundsRegionBoundary = outOfBoundsRegion.getInnerBoundary();
		for (auto &boundaryPoint : outOfBoundsRegionBoundary) {
			CurrentOuterBoundary.erase(boundaryPoint);
		}

		// Simulate said boundary points
		for (const auto &outerBoundPoint : CurrentOuterBoundary) {
			auto actualOuterBoundPoint = DynSystem.Transform.toActualCoords(outerBoundPoint);
			auto actualSimulatedPoint = DynSystem.simulateTimeStep(actualOuterBoundPoint);
			auto simulatedPoint = DynSystem.Transform.toGridCoords(actualSimulatedPoint);
			if(DynSystem.isAttracted(actualOuterBoundPoint)) {
				// In case outerBoundPoint is already attracted
				attBasinPartition[0].insert(outerBoundPoint);
				fullAttractorBasin.insert(outerBoundPoint);
				nNewPointsAdded++;
			}
			else if(DynSystem.isAttracted(actualSimulatedPoint)) {
				// This is the case where simulatedPoint was already attracted. Here we simply
				// add it to attBasinPartition[1]
				if(attBasinPartition.size()==1)
					attBasinPartition.push_back(Region(XLim, YLim));
				attBasinPartition[1].insert(outerBoundPoint);
				fullAttractorBasin.insert(outerBoundPoint);
				nNewPointsAdded++;
			}
			else if (simulatedPoint.x > XLim-1 || simulatedPoint.x < 0
			         || simulatedPoint.y > YLim-1 || simulatedPoint.y < 0
			         || outOfBoundsRegion.contains(simulatedPoint)){
				// In case simulatedPoint is out of bounds or lies in
				// outOfBoundsRegion
				outOfBoundsRegion.insert(outerBoundPoint);
			}
			else {
				// In this case, outerBoundPoint is not obviously going out of
				// bounds and it is not attracted. so we check the region that
				// contains simulatedPoint and insert accordingly
				if (attBasinPartition.size() > 0 && attBasinPartition.back().contains(simulatedPoint)) {
					// This is the case where a new partition has to be added for the point.
					attBasinPartition.push_back(Region(XLim, YLim));
					attBasinPartition.back().insert(outerBoundPoint);
					fullAttractorBasin.insert(outerBoundPoint);
					nNewPointsAdded++;
				}
				else if(attBasinPartition.size() > 1 && (attBasinPartition.end()-2)->contains(simulatedPoint)) {
					// This is a common case rolled out of the loop.
					// In this, we check if the simulated point lies in the last but one
					// partition and insert into the last partition if it is the case
					attBasinPartition.back().insert(outerBoundPoint);
					fullAttractorBasin.insert(outerBoundPoint);
					nNewPointsAdded++;
				}
				else if (attBasinPartition.size() > 2) {
					// Here we successively check for all regions upto (excluding)
					// attBasinPartition[0].
					for(uint32_t i = attBasinPartition.size()-2; i --> 0 ;) {
						if(attBasinPartition[i].contains(simulatedPoint)) {
							attBasinPartition[i+1].insert(outerBoundPoint);
							fullAttractorBasin.insert(outerBoundPoint);
							nNewPointsAdded++;
							break;
						}
					}
				}

				// In any case apart from the ones described above, the point
				// the point does not seem to tend towards the attractor OR go
				// out of bounds and so we will leave it as it is.
			}
		}

		// Consolidating partitions
		for(auto &partition : attBasinPartition) {
			partition.consolidate();
		}
		
		// Updating CurrentOuterBoundary
		CurrentOuterBoundary = fullAttractorBasin.getOuterBoundary();
	} while(nNewPointsAdded);

	attBasinPartition.push_back(fullAttractorBasin);
	return attBasinPartition;
}
