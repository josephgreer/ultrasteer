function F = ComputeGeneralPolynomial(coefficients, termFactors, powerIndex, points)
% function F = ComputeGeneralPolynomial(coefficients, termFactors, powerIndex, points)
%
% A general polynomial is given by a column of coefficients, a column of
% term factors MF, and a matrix representing the power indices of
% the terms.
% \input points a m*d array, where m is the number of points and d is the
% dimension (number of components).
% \input powerIndex a s*d array, where each row represent a term of the
% polynomial as a combination of powers of the corresponding components of
% a point.
% \input termFactors a s*1 column vector that represents additional
% "constant" factors in the terms, such as multinomial factors in a
% Bernstein polynomial.  For a power-basis polynomial this should be a
% column of ones.
% \param coefficients a s*1 column vector with the actual coefficients of
% the terms.
%
% \output a m*1 column vector where F_i = poly(x_i);
%
% poly(x_i) = \sum_{k=1}^s c_k f_k x_{i1}^{p_1} ... p_{id}^{p_d}
%

numTerms = size(coefficients, 1);

% powerProducts will be of size numTerms*m
powerProducts = EvaluatePowerProductsForPoints(points, powerIndex);

% finalCoefficients is of size numTerms*1
finalCoefficients = coefficients .* termFactors;

F = powerProducts' * finalCoefficients;