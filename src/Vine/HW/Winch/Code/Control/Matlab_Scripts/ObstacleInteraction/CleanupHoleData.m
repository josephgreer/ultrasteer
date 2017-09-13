clear; clc; close all;

load HoleData

closeHoles = find(dists < 300);
farHoles = find(dists > 300);

cxs = xs(closeHoles);
cdists = dists(closeHoles);
cangles = angles(closeHoles);

fxs = xs(farHoles);
fdists = dists(farHoles);
fangles = angles(farHoles);

[~, fidxs] = sort(fxs);
[~, cidxs] = sort(cxs);

fangles = fangles(fidxs);
cangles = cangles(cidxs);

fxs = fxs(fidxs);
cxs = cxs(cidxs);

fdists = fdists(fidxs);
cdists = cdists(cidxs);

% goodIdxs = [1:2,4,6,7:end];
cangles = cangles([1:2,4,6,7:end]);
cxs = cxs([1:2,4,6,7:end]);
cdists = cdists([1:2,4,6,7:end]);

cspread = [];
fspread = [];
xvals = []
for i=1:2:length(cdists)
    cspread = vertcat(cspread,max(cangles(i),abs(cangles(i)-cangles(i+1))));
    fspread = vertcat(fspread,max(fangles(i),abs(fangles(i)-fangles(i+1))));
    xvals = vertcat(xvals, mean([fxs(i),fxs(i+1),cxs(i),cxs(i+1)]));
end


scatter(cxs, cdists, 'r');
hold on;
scatter(fxs, fdists, 'b');
daspect([1 1 1]);