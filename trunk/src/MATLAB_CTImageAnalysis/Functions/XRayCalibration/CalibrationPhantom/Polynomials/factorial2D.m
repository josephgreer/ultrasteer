function F = factorial2D(M)
% function F = factorial2D(M)
% Compute the factorial for each element of the 2D matrix M. Return a
% corresponding matrix of factorials.  We assume that M is a matrix of
% positive integers, and do not check for it.  The implementation does not
% use the matlab factorial() function, but rather computes products of
% layers of a 3D matrix. The number of layers is equal to the maximal
% element in M.  We do not allow the maximal element in M to be over 15.

if min(min(M)) < 0
    error('factorial2D only takes nonnegative arguments');
end

maxM = max(max(M));
if maxM > 15
    error('factorial2D only takes arguments <= 15');
end

D = ones( [size(M), (maxM-1)] );
for k=2:maxM
    greaterEqual_k = (M >= k);
    K = ones(size(M));
    K( greaterEqual_k) = k;
    D(:,:,k) = K;
end

F = prod(D, 3);