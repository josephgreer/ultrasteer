function [e0,L0] = GetMaxContractionFromRAndW(R0,W,P,a)
f = @(L0)(GetMaxBulgeFromRAndL0(L0,R0,P,a,'pleated')-W);
L0 = fsolve(f, 10*R0);
e0 = GetE0(L0,R0,P,a,'pleated');
end