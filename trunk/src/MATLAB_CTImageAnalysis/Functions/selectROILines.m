% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: selectROILines(v,radius)
% -------------------------------------------------------------------------
% Inputs: v - isosurface vertices, radius - radius of cylinder to grow
% around the lines.
% Outputs: ROImatrix (x,y,z) points representing the needle in space
% Sub_Functions Called: getline
% Synopsis:
%   This function will select a cylindrical region of interest by having
%   the user define one line in in the XZ view, one in the YZ view and then
%   combining the line to creat a 3D line. Then for each vertex, if the
%   distance between the point and the user defined line is less than the
%   desired radius of cylinder, the point will be added to ROImatrix,
%   otherwise it wont. 
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: selectROILines()
% =========================================================================

function ROImatrix = selectROILines(v,radius)

close all
pause on                                                                   % Can't remember now why we needed to pause, otherwise didn't work. 

figure
plot(v(:,1),v(:,3),'.','MarkerSize',4.5);                                  % Plot XZ
%h1 = gca;

pause(1)                                                                   % Pause to get line

[x z1]= getline;
%disp('Draw line on needle shaft - *** CLICK FIRST POINT THEN SHIFT CLICK FINAL POINT ***')

figure
plot(v(:,2),v(:,3),'.','MarkerSize',4.5);                                  % Plot YZ
pause(1)                                                                   % Pause to get line
h2 = get(gcf,'CurrentAxes');
%disp('Draw line on needle shaft - *** CLICK FIRST POINT THEN SHIFT CLICK FINAL POINT ***')
[y z2] = getline(h2);
pause off

linePts = [x y (z1+z2)/2];                                                 % Find coordinates of point
ROImatrix = [];
% Only take points that are in the cylinder of radius radius surronding the user defined line. 
for i = 1:length(v)
    d = norm( cross(linePts(1,:)-linePts(2,:),v(i,:)-linePts(2,:))) / ...  % Find distance from point to user line
              norm(linePts(1,:)-linePts(2,:));
    if d <= radius                                                         % If d is less than radius, save the point v. 
        ROImatrix = [ROImatrix; v(i,:)];
    end
end


end

% =========================================================================
% END FUNCTION: selectROILines()
% =========================================================================