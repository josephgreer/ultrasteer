function [MF, P] = GenerateCisstBernsteinBasis(numVariables, degree)
% function [MF, P] = GenerateCisstBernsteinBasis(numVariables, degree)
% Generate the basis functions for Bernstein polynomials as given in the
% CISST libraty numerical code.  See NextTermIndex for more details.
% MF is a column vector of multinomial factors.
% P is a N*numVariables matrix, whose rows form power indices for the terms
% in the Bernstein polynomial.  Note that the sum of elements in each row
% is equal to degree, and therefore the row represents the powers of
% barycentric point components, not of a Euclidean point.
MF = [];
P = [];

powerIndex = zeros(1, numVariables);
powerIndex(1) = degree;
P = powerIndex;
while powerIndex(numVariables) < degree
    powerIndex = NextTermIndex(powerIndex);
    P = [P; powerIndex];
end

MF = MultinomialFactor(P);
