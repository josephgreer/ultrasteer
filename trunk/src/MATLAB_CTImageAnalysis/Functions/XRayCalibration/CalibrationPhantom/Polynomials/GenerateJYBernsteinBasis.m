function [MF, P] = GenerateJYBernsteinBasis(degree)
% function [MF, P] = GenerateJYBernsteinBasis(degree)
% Generate the basis function for Jianhua Yao's bernstein polynomial
% evaluation.  Yao generates a basis for four variables, so the only
% additional information needed is the degree.  It is important to follow
% Yao's procedure to keep the order of basis functions identical to the
% order of coefficients in his application.
% MF is a column vector of multinomial factors.
% P is a N*4 matrix, whose rows form power indices for the terms in the
% Bernstein polynomial.
MF = [];
P = [];

for i=0:degree
    for j = 0:(degree - i)
        for k = 0:(degree - i - j)
            powerIndex = [i j k (degree-i-j-k)];
            P = [P ; powerIndex];
        end
    end
end

MF = MultinomialFactor(P);
