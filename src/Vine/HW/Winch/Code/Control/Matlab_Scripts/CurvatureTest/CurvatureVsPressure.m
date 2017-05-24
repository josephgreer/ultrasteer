clear; clc; close all;

syms rtube a P r b kbar c real

expression = rtube*a*P*(1-c*rtube-b)-kbar*c
pretty(simplify(solve(expression, c)))


P = linspace(0,1,10000);

plot(P,P./(P+1))



    