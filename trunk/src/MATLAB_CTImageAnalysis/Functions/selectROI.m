% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: selectROI(v,subsampleStep,threeViews)
% -------------------------------------------------------------------------
% Inputs: v - isosurface vertices, subsampleStep - how to sample so that
% not all points are displayed, otherwise too much data, threeView - if the
% user wants XZ, YZ, XY views or just XZ, YZ.
% Outputs: ROImatrix - (x,y,z) 3d matrix of x,y,z locations of points in
% the region of interest. 
% Sub_Functions Called: subsampleMatrix(), getrect(), find()
% Synopsis:
%   This program will allow the user to select squares of interest my
%   plotting XZ, YZ (and optional XY) views of the region of interest. The
%   selected squares are then used to generate a bounding box which is then
%   used to identify which data points in the data lie within that bounding
%   box. Those points are returned as the region of interest. 
% =========================================================================

% =========================================================================
% BEGIN FUNCTION: selectROI()
% =========================================================================

function ROImatrix = selectROI(v,subsampleStep,threeViews)


% -------------------------------------------------------------------------
% Plotting Views
% -------------------------------------------------------------------------

% Subsample for plot
close all
sampledV = subsampleMatrix(v,subsampleStep);

figure
plot(sampledV(:,1),sampledV(:,3),'.','MarkerSize',4.5);
axis equal
h1 = gca;

figure
plot(sampledV(:,2),sampledV(:,3),'.','MarkerSize',4.5);
axis equal
h2 = gca;

if threeViews
    figure
    plot(sampledV(:,1),sampledV(:,2),'.','MarkerSize',4.5);
    axis equal
    h3 = gca;
end

% -------------------------------------------------------------------------
% USER SELECTED ROI SQUARES
% -------------------------------------------------------------------------

disp('Select Region of Interest for X-Z View  ** START AT UPPER LEFT CORNER **')
xz_rect = getrect(h1);                                                     % Get a square defined by user on the XZ view

disp('Select Region of Interest for Y-Z View  ** START AT UPPER LEFT CORNER **')
yz_rect = getrect(h2);                                                     % Get a square defined by user on the YZ view

if threeViews
    disp('Select Region of Interest for X-Y View  ** START AT UPPER LEFT CORNER **')
    xy_rect = getrect(h3);
    limits = [min(xz_rect(1),xy_rect(1)) ...                               % Find the six bounding box vertices given user selected squares (if three views presented)
              min(xz_rect(1)+xz_rect(3),xy_rect(1)+xy_rect(3)) ...
              min(yz_rect(1),xy_rect(2)) ...
              min(yz_rect(1)+yz_rect(3),xy_rect(2)+xy_rect(4)) ...
              max(xz_rect(2),yz_rect(2)) ...
              max(xz_rect(2)+xz_rect(4),yz_rect(2)+yz_rect(4))];
else
    limits = [xz_rect(1) ...                                               % Find the six bounding box vertices given user selected squares (if two views presented)
              xz_rect(1)+xz_rect(3)  ...
              yz_rect(1) ...
              yz_rect(1)+yz_rect(3) ...
              max(xz_rect(2),yz_rect(2)) ...
              max(xz_rect(2)+xz_rect(4),yz_rect(2)+yz_rect(4))];
end
 
% -------------------------------------------------------------------------
% Generate ROI Matrix
% -------------------------------------------------------------------------
allpts = find(limits(1) < v(:,1) & ...                                      % Find the indicies associated with the limits from the bounding box.
              v(:,1) < limits(2) & ...
              limits(3) < v(:,2) & ...
              v(:,2) < limits(4) & ...
              limits(5) < v(:,3) & ...
              v(:,3) < limits(6));

ROImatrix = v(allpts,:);                                                   % Find the point associated with the indices of the bounding box. 


end

% =========================================================================
% END FUNCTION: selectROI()
% =========================================================================
