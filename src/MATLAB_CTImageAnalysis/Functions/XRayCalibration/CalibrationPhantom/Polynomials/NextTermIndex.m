function outIndex = NextTermIndex(inIndex)
% function outIndex = NextTermIndex(inIndex)
% Compute a successort term index according to the rules in class
% cisstNumerical/nmrPolynomialTermPowerIndex . 
%
% \param inIndex is a row vector of non-negative integers representing the
% powers of a term in a multivariable polynomial
%
% \param outIndex is a row vector representing the successor term.  The
% successor is evaluated by setting the power of the last variable from
% lastVarPower to zero, promoting the rightmost remaining power, and adding
% lastVarPower to the newly promoted power.

[m, n] = size(inIndex);
if (m ~= 1)
    error('NextTermIndex -- only one term is allowed at a time');
end

outIndex = inIndex;
lastVarPower = outIndex(n);
outIndex(n) = 0;
wherePower = find(outIndex);
if (isempty(wherePower))
    outIndex(1) = lastVarPower+1;
    return
end

wherePrevPower = wherePower(length(wherePower));
outIndex(wherePrevPower) = outIndex(wherePrevPower) - 1;
outIndex(wherePrevPower+1) = lastVarPower + 1;
