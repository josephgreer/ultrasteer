% LoadWireGridGeometry
%
% This script defines variables describing the dewarp grid geometry.  It
% returns three variables to be used in the dewarp scripts.
%
modelVertRadii = [-100:10:100];
modelHorzRadii = [-100:10:100];
modelBoundingBox = [...
    min(modelVertRadii), max(modelVertRadii);...
    min(modelHorzRadii), max(modelHorzRadii)];
