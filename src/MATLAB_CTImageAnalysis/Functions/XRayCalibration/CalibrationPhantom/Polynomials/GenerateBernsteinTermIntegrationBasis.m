function [MF, B] = GenerateBernsteinTermIntegrationBasis(maxPowers)
% function [MF, B] = GenerateBernsteinTermIntegrationBasis(maxPowers)
% Given a row vector maxPowers of size 1*n, generate a matrix B of size
% p*n. The rows of B represent all the possible combinations [i_1 ... i_n]
% so that 0 <= i_k <= maxPowers(k). p is the number of rows, and 
% p = prod(maxPowers + 1). MF is a corresponding p*1 vector of multinomial
% factors. See the function MultinomialFactor for details.
%
% The result matrix B serves to integrate a single term of a Bernstein
% polynomial.

% reverse the order of the powers :-)
backPowers = maxPowers( length(maxPowers):-1:1 );

B = [];
for k=backPowers
    r = size(B, 1);
    if r == 0
        B = [0:k]';
    else
        D = [];
        for j=0:k
            D = [D; [j * ones(r, 1), B]];
        end
        B = D;
    end
end
        
MF = MultinomialFactor(B);