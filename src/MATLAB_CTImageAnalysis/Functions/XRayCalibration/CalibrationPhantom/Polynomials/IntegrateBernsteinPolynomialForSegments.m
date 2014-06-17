function I = IntegrateBernsteinPolynomialForSegments(segmentStarts, segmentEnds, startMultinomialFactors, startIntegrationBasis, endMultinomialFactors, endIntegrationBasis, coefficients)

[m, n] = size(segmentStarts);

% evaluate the lengths of the segments and cache it for later
segmentDiffs = segmentEnds - segmentStarts;
diffSquares = segmentDiffs .* segmentDiffs;
squareSums = sum(diffSquares, 2);
segmentLengths = sqrt(squareSums);

% Evaluate a table of power basis to be cached for later
degree = max(max(startIntegrationBasis));
powerBasisStarts = GeneratePowerBasis(segmentStarts, degree);
powerBasisEnds = GeneratePowerBasis(segmentEnds, degree);


I = zeros(m, 1);


startPowerProducts = EvaluatePowerProductsForPoints(segmentStarts, startIntegrationBasis, powerBasisStarts);
endPowerProducts = EvaluatePowerProductsForPoints(segmentEnds, endIntegrationBasis, powerBasisEnds);
    
startPowersWeights = startMultinomialFactors * ones(1, m);
endPowersWeights = endMultinomialFactors * ones(1,m);
    
weighedStartPowers = startPowerProducts .* startPowersWeights;
weighedEndPowers = endPowerProducts .* endPowersWeights;
    
pointwiseStartEndProduct = weighedStartPowers .* weighedEndPowers;
segmentSummation = coefficients' * pointwiseStartEndProduct;


% I = (segmentLengths .* segmentSummation') / (degree + 1);
I = segmentSummation' / (degree + 1);