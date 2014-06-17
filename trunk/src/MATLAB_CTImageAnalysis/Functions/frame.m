% Author: Ann Majewicz
% Date: 10/13/10
% Computer Integrated Surgery 
% Matlab Function Library
% Johns Hopkins University
%--------------------------------------------------------------------------
function [F] = frame(R,p)

% FUNCTION [F] = frame(R,p)
% Inputs: R (3x3) - rotation matrix, p (3x1) - translation vector.
% Outputs: F (4x4) - homogeneous frame representation 
% Sub_Functions Called: none
% Synopsis: This function will generate the homogenous frame representation
% given a rotation matrix and a translation vector. 
% -------------------------------------------------------------------------

F = [R p; 0 0 0 1];


end

