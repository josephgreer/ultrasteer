function prm = ComputeIntrinsicParametersCalibration(orderedModelPoints, ...
    orderedImagePoints)
% function prm = ComputeIntrinsicParametersCalibration(orderedModelPoints, orderedImagePoints)
%
% Find the intrinsic projection parameters of a camera given corresponding
% points.
% \param orderedModelPoints an n*3 array of point coordinates with respect
% to the detector coordinate system, in world units (mm)
% \param orderedImagePoints an n*2 array of point coordinates of the
% projections of orderedModelPoints, given world units (mm) with respect to
% the virtual detector center showing in the image.  Note the following:
%    1) The virtual detector center is defined, for example, from a dewarp
%    grid.  The image used for calibration must be dewarped and the
%    position of the virtual center in the image must be known for this
%    calibration to succeed.
%    2) The image axes are assumed to align with the axes of the virtual
%    detector and the calibration phantom.  This means that the positive X
%    direction of the calibration phantom should coincide with the positive
%    X direction of the image pixels (increasing column index), and the
%    positive Y direction of the phantom shold coincide with the positive Y
%    direction of image pixels (increasing row index).
%    3) Some visual marker on the calibration phantom is required to denote
%    the X and Y directions in the phantom coordinate system.  The phantom
%    image must be rotated so that this visual marker corresponds to the
%    correct direction with respect to the virtual detector center.
%    4) This function assumes that the input model points correspond to the
%    input image points.  It is the caller's responsibility to sort either
%    or both to satisfy this precondition.
%    5) It is the caller's responsibility to assure that both sets of input
%    points are passed in the same length units, i.e., millimetres.
%
% \param prm a 3*1 vector of source position with respect to detector
% coordinates.
%
% Method:
%
% When all points are transformed to detector coordinates, then each point
% [x y z] in the world is projected to image coordinates (in mm) [u v] so that
%
% (x - x0) / (z - z0) = (u - x0) / (-z0)
% (y - y0) / (z - z0) = (v - y0) / (-z0)
%
% Develop into
%
% -x*z0 + x0*z0 = u*z - u*z0 + x0*z + x0*z0
% ==>
% -z*x0        + (u-x)*z0 = u*z
%        -z*y0 + (v-y)*z0 = v*z
%
% Now we have a set of linear equations in the unknowns x0, y0, z0.

numImagePoints = size(orderedImagePoints, 1);

% Set the equations
lhsX = [-orderedModelPoints(:,3), zeros(numImagePoints,1), orderedModelPoints(:,1) - orderedImagePoints(:,1)];
rhsX = -(orderedImagePoints(:,1) .* orderedModelPoints(:,3));
lhsY = [zeros(numImagePoints,1), -orderedModelPoints(:,3), orderedModelPoints(:,2) - orderedImagePoints(:,2)];
rhsY = -(orderedImagePoints(:,2) .* orderedModelPoints(:,3));

prm = [lhsX; lhsY] \ [rhsX; rhsY];
