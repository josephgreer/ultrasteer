clear; clc; close all;


baseDir = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/AnalysisOfGrowingRobotActuation/Data/PegForest/8/*.png';

files = rdir(baseDir);

dist1 = 276.323607111321;
dist2 = 401;
wall1 = [1148.25; 65.7500000000001];
wall2 = [1143.75; 259.25];
holeWidth = 60;
holeCenter = [1155.75; 437.75];
% P2 = [x(3); y(3)];

dists = [];
xs = [];
angles = [];
for i=1:length(files)
    
    im = imread(files(i).name);
    imshow(im);
    h = size(im,1);
    hold on;
    [x y] = ginput;
    
    dists = vertcat(dists,PointLineDistance(wall1, wall2, [x(1) y(1)]));
    xx = ProjectPointOntoLine(wall1, wall2, [x(1); y(1)]);
    plot([wall1(1) wall2(1)], h-[wall1(1) wall2(2)]);
    scatter(xx(1), h-xx(2), 100, 'LineWidth', 10);
    xx = norm(xx-holeCenter);
    xs = vertcat(xs, xx);
    
    yep = [x y];
    delta = yep(2,:)-yep(1,:); delta = delta.';
    angles = vertcat(angles,rad2deg(atan2(delta(2),delta(1))));
    
end
