function [startMultinomialFactors, startIntegrationBasis, endMultinomialFactors, endIntegrationBasis, coefficientDuplicator] = GenerateBernsteinPolynomialIntegrationBasis(termPowerIndexes)
% function [startMultinomialFactors, startIntegrationBasis, endMultinomialFactor, endIntegrationBasis, coefficientDuplicator] = GenerateBernsteinPolynomialIntegrationBasis(termPowerIndexes)
% The input to the function is a table of power indexes which represent all
% the terms of a Bernstein polynomial.
%
% The function creates a table of power indexes that are used to integrate
% each of the terms of the original polynomial. Since the line integral
% involves the coordinates of two points, we generate power indexes and
% multinomial factor for the start point of the segment and for the end
% point of the segment. For each single term, the power indexes of the
% start and of the end are in an opposite order.
% The last output is a binary matrix to duplicate the coefficients of the
% terms. if the coefficients are a t*1 column vector C, then
%  coefficientDuplicator * C  duplicates each of the coefficients in C the
%  number of times as the number of integration elements for that term.

numTerms = size(termPowerIndexes, 1);

startMultinomialFactors = [];
startIntegrationBasis = [];
endMultinomialFactors = [];
endIntegrationBasis = [];
coefficientDuplicator = [];

for term=1:numTerms
    termPowerIndex = termPowerIndexes(term,:);
    [startTermMultinomialFactor, startTermIntegrationBasis] = GenerateBernsteinTermIntegrationBasis(termPowerIndex);
    basisSize = size(startTermIntegrationBasis, 1);
    endTermMultinomialFactor = startTermMultinomialFactor( basisSize:-1:1, : );
    endTermIntegrationBasis = startTermIntegrationBasis( basisSize:-1:1, : );
    
    startMultinomialFactors = [startMultinomialFactors; startTermMultinomialFactor];
    startIntegrationBasis = [startIntegrationBasis; startTermIntegrationBasis];
    endMultinomialFactors = [endMultinomialFactors; endTermMultinomialFactor];
    endIntegrationBasis = [endIntegrationBasis; endTermIntegrationBasis];
    duplicatorRows = size(coefficientDuplicator, 1);
    coefficientDuplicator(duplicatorRows+1:duplicatorRows+basisSize, term) = ones(basisSize,1);
end