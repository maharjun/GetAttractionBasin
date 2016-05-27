addpath build/install;

%%
DynSysParameters.a = single(0.02);
DynSysParameters.b = single(0.25);
DynSysParameters.c = single(-65);
DynSysParameters.d = single(8);

DynSysParameters.GridXSpec = single([-5; 0.05; 35]);
DynSysParameters.GridYSpec = single([-80; 0.05; 30]);

DynSysParameters.onemsbyTstep = uint32(2);
DynSysParameters.InitialPointSet.ClassName = 'PointVector';
DynSysParameters.InitialPointSet.X = single(DynSysParameters.GridXSpec(1):DynSysParameters.GridXSpec(2):DynSysParameters.GridXSpec(3));
DynSysParameters.InitialPointSet.Y = single(30*ones(size(DynSysParameters.InitialPointSet.X)));

[FullBasinBoundary, BasinPartitionBoundaries] = GetAttBasin_IzhikevichSpiking(DynSysParameters);

%% Plot
PartitionFig = figure();
colormap(PartitionFig, copper(length(BasinPartitionBoundaries)));
GridXSpec = DynSysParameters.GridXSpec; % [0 1 1];
GridYSpec = DynSysParameters.GridYSpec; % [0 1 1];
fill(double(FullBasinBoundary.X)*GridXSpec(2) + GridXSpec(1), double(FullBasinBoundary.Y)*GridYSpec(2) + GridYSpec(1), 'b');
hold on;
for i = 1:length(BasinPartitionBoundaries)
	fill(double(BasinPartitionBoundaries{i}.X)*GridXSpec(2) + GridXSpec(1), double(BasinPartitionBoundaries{i}.Y)*GridYSpec(2) + GridYSpec(1), i, 'LineStyle', 'none');
end
xlim([GridXSpec(1),GridXSpec(3)])
ylim([GridYSpec(1),GridYSpec(3)])

hold off;
