% $Id: ComputeResidualErrors.m,v 1.1 2005/12/07 22:37:38 gouthami Exp $
function [meanErr, minErr, maxErr, stdev] = ComputeResidualErrors(truePts, calculatedPts)

if (size(truePts) ~= size(calculatedPts))
    error(' truePts and calculatedPts should have same dimension ');
end
    
temp = (truePts - calculatedPts).*(truePts - calculatedPts);
dist = sqrt(temp(:,1) + temp(:,2));

meanErr = mean(dist);
minErr = min(dist);
maxErr = max(dist);
stdev = std(dist);

%$Log: ComputeResidualErrors.m,v $
%Revision 1.1  2005/12/07 22:37:38  gouthami
%Adding the file to CVS for the first time
%