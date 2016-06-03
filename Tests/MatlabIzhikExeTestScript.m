%% Create Input File
DynSysParameters.a = single(0.02);
DynSysParameters.b = single(0.25);
DynSysParameters.c = single(-65);
DynSysParameters.d = single(8);

DynSysParameters.GridXSpec = single([-5; 0.05; 35]);
DynSysParameters.GridYSpec = single([-80; 0.05; 30]);

DynSysParameters.onemsbyTstep = uint32(2);

DynSysParams = DynSysParameters;
save ./DynSysInputFile.mat DynSysParams
clear DynSysParams

%% Run Program
% At this point, run the exe to generate the file OutputRegions.mat in the
% current directory.

fprintf('At this point, run the exe to generate the file OutputRegions.mat in the\ncurrent directory.\n\n')
input('press enter when done', 's')

%% Load Regions

load OutputRegions
BasinPartitionBoundaries = PartitionBoundaries;
clear PartitionBoundaries;

%% Plot Regions
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