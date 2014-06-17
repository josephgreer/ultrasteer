% Author: Ann Majewicz
% Date: 10/18/10
% Computer Integrated Surgery 
% Matlab Function Library
% Johns Hopkins University
%--------------------------------------------------------------------------

function [F] = frameCompose(F1,F2)

% FUNCTION [F] = frameCompose(F1,F2)
% Inputs: F1 and F2 (4x4) - homogenous frame
% Outputs: F (4x4) - homogenous frame composed of F1 and F2
% Sub_Functions Called: none
% Synopsis: This function will compose two frames using the associated R
% and p's. 
% -------------------------------------------------------------------------

[R1,p1] = deFrame(F1);
[R2,p2] = deFrame(F2);
F = frame(R1*R2,R1*p2+p1);

end

