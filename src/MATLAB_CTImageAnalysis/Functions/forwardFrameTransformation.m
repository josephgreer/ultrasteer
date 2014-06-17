% Author: Ann Majewicz
% Date: 10/13/10
% Computer Integrated Surgery 
% Matlab Function Library
% Johns Hopkins University
%--------------------------------------------------------------------------
function [b] = forwardFrameTransformation(F,a)

% FUNCTION [b] = forwardFrameTransformation(F,a)
% Inputs: F (4x4)- homogenous frame, a (3xn) - vector of points.  
% Outputs: b (3xn) - vector of points
% Sub_Functions Called: deFrame ()
% Synopsis: This function will transform the vector a by the frame F and
% store the result in vector b. IE find b such that b = F*a
% -------------------------------------------------------------------------

[R,p] = deFrame(F);
[m,n] = size(a);

b = R*a+repmat(p,1,n);

end

