#include "DynamicalSystem.hpp"
#include <MexMemoryInterfacing/Headers/GenericMexIO.hpp>
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
	uint32_t NRounds = 0, NPoints = 0;
	
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
			SinglePoint currentPoint = outerBoundPoint;
			bool isCurrentPointinOutBound = true;
			bool isInAttBasin = false;
			bool isOutOfBounds = false;
			uint32_t attBasinInsertIndex = 0;

			// this represents the number of times a point is simulated before it gets
			// out of a boundary
			uint32_t SimulationLength = 0;
			while (isCurrentPointinOutBound) {
				SimulationLength++;
				// Set the flag below false. Only if it s set to true by the end of the loop
				// will the loop continue.
				isCurrentPointinOutBound = false;
				auto actualCurrentPoint = DynSystem.Transform.toActualCoords(currentPoint);
				auto actualSimulatedPoint = DynSystem.simulateTimeStep(actualCurrentPoint);
				auto simulatedPoint = DynSystem.Transform.toGridCoords(actualSimulatedPoint);
				Point simulatedGridPoint(simulatedPoint.x + 0.5f, simulatedPoint.y + 0.5f);

				if(DynSystem.isAttracted(actualCurrentPoint)) {
					// In case outerBoundPoint is already attracted
					isInAttBasin = true;
					attBasinInsertIndex = SimulationLength-1;
				}
				else if(DynSystem.isAttracted(actualSimulatedPoint)) {
					// This is the case where simulatedPoint was already attracted. Here
					// we simply add it to attBasinPartition[1]
					isInAttBasin = true;
					attBasinInsertIndex = SimulationLength;
				}
				else if (simulatedPoint.x > XLim-1 || simulatedPoint.x < 0
				         || simulatedPoint.y > YLim-1 || simulatedPoint.y < 0
				         || outOfBoundsRegion.contains(simulatedPoint)){
					// In case simulatedPoint is out of bounds or lies in
					// outOfBoundsRegion
					isOutOfBounds = true;
				}
				else if (!CurrentOuterBoundary.count(simulatedGridPoint)) {
					// Here, we check the containership of SimulatedPoint in the existing
					// attBasinPartition's
					for(uint32_t i = attBasinPartition.size(); i --> 0 ;) {
						if(attBasinPartition[i].contains(simulatedPoint)) {
							isInAttBasin = true;
							attBasinInsertIndex = i+SimulationLength;
							break;
						}
					}
					// In any case apart from the ones described above, the point
					// the point does not seem to tend towards the attractor OR go
					// out of bounds and so we will leave it as it is.
				}
				else {
					// In this case, the Point has not been simulated enough. Hence
					// we will set it up for further simulation
					if (currentPoint != simulatedPoint) {
						// This condition is to prevent getting stuck in other equilibria
						// That somehow manage to come exactly on the grid.
						currentPoint = simulatedPoint;
						isCurrentPointinOutBound = true;
					}
				}
			}
			if (isInAttBasin) {
				if (attBasinInsertIndex >= attBasinPartition.size()) {
					attBasinPartition.resize(attBasinInsertIndex+1, Region(XLim, YLim));
				}
				attBasinPartition[attBasinInsertIndex].insert(outerBoundPoint);
				fullAttractorBasin.insert(outerBoundPoint);
				nNewPointsAdded++;
			}
			else if (isOutOfBounds) {
				outOfBoundsRegion.insert(outerBoundPoint);
			}
		}

		// Consolidating partitions
		for(auto &partition : attBasinPartition) {
			partition.consolidate();
		}
		
		// Updating CurrentOuterBoundary
		CurrentOuterBoundary = fullAttractorBasin.getOuterBoundary();

		NRounds++;
		NPoints += nNewPointsAdded;
		if (NRounds % 100 == 0) {
			WriteOutput("NRounds = %d\n", NRounds);
			WriteOutput("NPoints = %d\n", NPoints);
		}

	} while(nNewPointsAdded);

	attBasinPartition.push_back(fullAttractorBasin);
	return attBasinPartition;
}
