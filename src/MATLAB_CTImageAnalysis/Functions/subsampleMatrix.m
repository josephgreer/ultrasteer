% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: subsampleMatrix(matrix,sampleStep)
% -------------------------------------------------------------------------
% Inputs: matrix (3xn), sampleStep - iteration step i.e. how much to skip
% Outputs: sampledV - the subsampled matrix.
% the region of interest. 
% Sub_Functions Called: none
% Synopsis:
%   This program create a new matrix in which rows are deleted from the
%   original corresponding to the size of sampleStep. For example if it's
%   1, then no rows are deleted, if 2, every other row is deleted. 
% =========================================================================

% =========================================================================
% BEGIN FUNCTION: selectROI()
% =========================================================================

function sampledV = subsampleMatrix(matrix,sampleStep)

    j = 1;
    for i = 1:sampleStep: length(matrix)
        sampledV(j,:) = matrix(i,:);
        j = j+1;
    end


end

% =========================================================================
% END FUNCTION: subsampleMatrix()
% =========================================================================