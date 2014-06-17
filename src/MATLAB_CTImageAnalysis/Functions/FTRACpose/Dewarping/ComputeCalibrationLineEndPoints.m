%$Id: ComputeCalibrationLineEndPoints.m,v 1.2 2006/10/02 17:42:02 anton Exp $

function lineEndPoints = ComputeCalibrationLineEndPoints(im, labelIm)
%function linesIm = ComputeLineEndPoints(im, labelIm) computes the end
%points of the lines in the dewarp image. This is used to display in the
%GUI to verify the calibration process.
% @param im - input dewarped image
% @param labelIm - binary image with the pixels along the lines.
% @output lineEndPoints - Nx4 array with the two end points of the line in
% each row

numLines = max(max(labelIm));
imSize = size(im);

for i = 1:numLines
    [x,y] = find(labelIm == i);
    lineEq = least_squares_line([x,y]);
    a = lineEq(1);
    b = lineEq(2);
    c = lineEq(3);
    
    angle = atan(-b/a) * 180/pi;
    if angle > 45
        pt11 = [0,  -c/b];
        pt12 = [imSize(2), (-c-imSize(2)*a)/b];
    else
        pt11 = [-c/a, 0];
        pt12 = [(-c-imSize(2)*b)/a, imSize(2)];
    end    
   lineEndPoints(i,:) = [pt11, pt12];
end

% $Log: ComputeCalibrationLineEndPoints.m,v $
% Revision 1.2  2006/10/02 17:42:02  anton
% ComputeCalibrationLineEndPoints: Take image size into account (was hard
% coded for 480)
%
% Revision 1.1  2006/07/15 21:25:31  gouthami
% Adding to CVS
%
    
    