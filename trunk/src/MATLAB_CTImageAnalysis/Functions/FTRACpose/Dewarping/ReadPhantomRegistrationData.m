%$Id: ReadPhantomRegistrationData.m,v 1.1 2006/09/27 15:57:12 gouthami Exp $
function [R, T] = ReadPhantomRegistrationData(filename)
%[R, T] = ReadPhantomRegistrationData(filename) reads the frame computed
%from the optotrak data. This frame transformation is from the calibration
%plate to the distortion correction plate, calculated using optotrak data
% Method: Data is collected along the hexagonal lines on both the plates.
% Fit planes to the data from the upper plate and the lower plate respectively and
% project the data back on to these planes. Then fit lines to the six lines
% on each plate. Compute the center and the axis from the lines and find
% F1 ( registration transformation between the physical coords of the
% calibration plate to the optotrak readings) and F2 (registration
% transformation between the physical coords of the distortion correction
% plate to the optotrak readings) and the resulting frame transformation is
% F2*F1^-1.

fid = fopen(filename, 'rt');
mat = (fscanf(fid, '%f', [4,4]))';
R = mat(1:3, 1:3);
T = mat(1:3, 4)';
fclose(fid);

