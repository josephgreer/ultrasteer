% Author: Ann Majewicz
% Date: 4/12/2011
%--------------------------------------------------------------------------
function [Finv] = frameInverse(F)

% FUNCTION [a] = inverseFrameTransformation(F,b)
% Inputs: F (4x4)- homogenous frame, b (3xn) - vector of points.  
% Outputs: a (3xn) - vector of points
% Sub_Functions Called: deFrame ()
% Synopsis: Find a such that b = F*a
% -------------------------------------------------------------------------

[R,p] = deFrame(F);
Finv = frame(inv(R),-1*inv(R)*p);
end
