clear; clc; close all;

badRuns = [3];

for run=2:18
    if(sum(badRuns == run) > 0)
        continue;
    end;
    pts = load(sprintf('%02d.mat',run));
    pts = pts.pts;
    
    xs = []; ys = [];
    figure(1);
    scatter(pts{1}(:,1),pts{1}(:,2),'r');
    hold on;
    
    axis1 = pts{1}(2,:)-pts{1}(1,:);  axis1 = axis1/norm(axis1);
    axis2 = pts{1}(4,:)-pts{1}(3,:);  axis2 = axis2/norm(axis2);
    
    ptA = pts{1}(1,:).';
    ptB = pts{1}(2,:).';
    
    basePt = pts{1}(4,:).';
    
    theta = acos(axis1*axis2.');
    
    startPoint = FindIntersectionOfTwoLines(pts{1}(1,:).',pts{1}(2,:).',pts{1}(3,:).',pts{1}(4,:).');
    scatter(startPoint(1),startPoint(2),'b');
    maxLength = norm(pts{1}(2,:).'-startPoint);
    
    npoints = 100;
    modelLengths = linspace(0,maxLength*1.2,npoints);
    
    intersectionPoints = repmat(startPoint,1,npoints)+axis1.'*modelLengths;
    deltas = intersectionPoints-repmat(basePt,1,npoints);
    mags = sqrt(sum(deltas.^2,1));
    mags = repmat(mags,2,1);
    deltas = deltas./mags;
    
    modelThetas = acos(axis2*deltas);
    
    dataLs = [];
    dataThetas = [];
    for i=2:length(pts)
        if(size(pts{i},1) == 0)
            continue;
        end
        
        
        endPt = pts{i}(end,:).';
%         intersectionPoint = FindIntersectionOfTwoLines(basePt,endPt,pts{1}(1,:).',pts{1}(2,:).');
%         endPt = intersectionPoint;
        
        cDelta = endPt-basePt;
        deltaAxis = cDelta/norm(cDelta);
        dataLs = vertcat(dataLs,norm(endPt-startPoint));
        dataThetas = vertcat(dataThetas,acos(axis2*deltaAxis));
        
    end
    
    figure;
    scatter(dataLs, rad2deg(dataThetas));
    hold on;
    plot(modelLengths,rad2deg(modelThetas));
    title(sprintf('Theta = %f, run = %d', rad2deg(theta), run));
    ylim([0 45]);
    
end