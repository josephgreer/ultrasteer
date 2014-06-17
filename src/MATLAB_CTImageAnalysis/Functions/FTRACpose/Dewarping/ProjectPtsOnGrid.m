%$Id: ProjectPtsOnGrid.m,v 1.1 2006/07/15 18:28:25 gouthami Exp $
function [projectedPts, transformedPts] = ProjectPtsOnGrid(pts, rot, focallength)
% function [projectedPts, transformedPts] = ProjectPtsOnGrid(pts, rot,
% focallength) projects the input points on to the grid 
% @param pts - input pts
% @param rot - orientation of the grid to the image plane
% @param focallength - approximate focal length
% @output projectedPts - grid pts after projecting on to the image plane
% @output transformedPts - transformed projected pts to align with those on
% the image plane

endPts1 = pts(:,1:2);
endPts1 = (rot * endPts1')';
endPts2 = pts(:,3:4);
endPts2 = (rot * endPts2')';
transformedPts = [endPts1, endPts2];
ptsZ = focallength - 125;
m = focallength/ ptsZ;
projectedPts = transformedPts *m;

% $Log: ProjectPtsOnGrid.m,v $
% Revision 1.1  2006/07/15 18:28:25  gouthami
% Adding to CVS
%
