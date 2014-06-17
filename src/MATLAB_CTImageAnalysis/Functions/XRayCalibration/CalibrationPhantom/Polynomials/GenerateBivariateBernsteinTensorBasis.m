function [MF, P] = GenerateBivariateBernsteinTensorBasis(degree)
% function [MF, P] = GenerateBivariateBernsteinTensorBasis(degree)
%
% This function generates a table of powers and multinomial factors for a
% bivariate Bernstein polynomial in tensor-product representation.  The
% tensor product representation is:
%
% F(x,y) = \sum_{i=0}^p \sum_{j=0}^p \alpha_{i, j} {p \choose i} x^i (1-x)^{p-i}} {p \choose j} y^j (1-y)^{p-j}
%
% {\alpha_{i,j}} are free coefficients; the combinatorial elements {p \choose i} and {p \choose j}
% are multinomial factors;  the powers of x and (1-x), and of y and (1-y)
% sum up to the degree p; the summation is over all combinations of i and j
% between 0 and p .  Therefore, the polynomial will have (p+1)^2 terms.
%
% \output MF a column vector of the multinomial factors {p \choose i}*{p \choose j}
% Note that this is the product of the factor from the tensor components,
% stored as one factor here.
% \output P a ( (p+1)^2 ) by 4 matrix, where the rows are the power indexes
% of the terms in the tensor form.

[matx, maty] = meshgrid(0:degree);

basisSize = (degree+1) * (degree+1);

vecx = reshape(matx, [basisSize, 1]);
vecy = reshape(maty, [basisSize, 1]);

basisx = [vecx, degree-vecx];
basisy = [vecy, degree-vecy];

mfx = MultinomialFactor(basisx);
mfy = MultinomialFactor(basisy);

MF = mfx .* mfy;
P = [basisx, basisy];
