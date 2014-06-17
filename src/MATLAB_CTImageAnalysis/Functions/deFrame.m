% Author: Ann Majewicz
% Date: 10/13/10
% Computer Integrated Surgery 
% Matlab Function Library
% Johns Hopkins University
%--------------------------------------------------------------------------
function [R,p] = deFrame(F)

% FUNCTION [R,p] = deFrame(F)
% Inputs: F (4x4) - homogenous frame
% Outputs: R (3x3) - rotation matrix and p (3x1) - translation vector.
% Sub_Functions Called: none
% Synopsis: This function will decompose a homogenous frame representation
% into rotation and translation parts. 
% -------------------------------------------------------------------------

R = F(1:3,1:3);
p = F(1:3,4);

end

