% $Id: GenerateCostMatrix.m,v 1.1 2006/07/15 18:22:27 gouthami Exp $
function C = GenerateCostMatrix(lineEqns, pts)
% function C = GenerateCostMatrix(lineEqns, pts) computes the cost matrix
% used by hungarian to solve the assignment problem. The assignment problem
% here is to find correspondences between the line segments in the image
% and the line segments on the calibration phantom
% @param lineEqns - equation of the line
% @param pts - end points of the lines on the calibration phantom
% @output C - cost matrix

numLines = size(lineEqns, 1);
denom = sqrt(lineEqns(:,1).*lineEqns(:,1) + lineEqns(:,2).*lineEqns(:,2));

for i = 1:numLines
    temp1(:,1) = lineEqns(i,1)* pts(:,1);
    temp1(:,2) = lineEqns(i,2)* pts(:,2);
    
    d1 = abs(temp1(:,1)+temp1(:,2) + ones(numLines,1).*lineEqns(i,3))/denom(i);
    
    temp1(:,1) = lineEqns(i,1)* pts(:,3);
    temp1(:,2) = lineEqns(i,2)* pts(:,4);
    
    d2 = abs(temp1(:,1)+temp1(:,2) + ones(numLines,1).*lineEqns(i,3))/denom(i);
    
    d = (d1+d2)/2;
    C(:, i) = d;
    
end
    
%$Log: GenerateCostMatrix.m,v $
%Revision 1.1  2006/07/15 18:22:27  gouthami
%Adding to CVS
%