##  Major Algorithm:

###   Top Level pseudocode:

We have the following data:

1.  **fullAttractorBasin** -
    
    This is a Region that represents the entire attractor basin

2.  **attBasinPartition** -
    
    This is a vector of regions which forms a partition of fullAttractor-
    Basin.  The partition basically splits  fullAttractorBasin  into Sub-
    regions based on how many  time steps they take to get attracted into
    region 0. attBasinPartition[j] <==> j+1 steps to get attracted.

3.  **outOfBoundsRegion** - 
    
    This is the region that consist of points that end up outside the grid
    boundary upon simulation. This is maintained so that these points are
    not tested repeatedly.

The High level algorithm:

    for each point on the outer boundary:
        simulate one timestep, and get NextTimePoint
        if NextTimePoint in attBasinPartition[j] for any existing j:
            add point to attBasinPartition[j+1] (create if it doesnt exist)
            add point to fullAttractorBasin.
        elif NextTimePoint in outOfBoundsRegion or NextTimePoint out of grid:
            add point in outOfBoundsRegion.
    
    for each region in attBasinPartition:
        push region.midwayboundary into regionBoundaries
    
    return fullAttractorBasin.boundary and regionBoundaries.

