clear; clc; close all;

onAxisMode = 10;
onAxisSigma = 3;
offAxisMode = 1;
offAxisSigma = 100;

syms x y;
S = vpasolve([exp(x-y^2) == onAxisMode, (exp(y^2)-1)*exp(2*x+y^2) == onAxisSigma],[x y]);

xs = [0.1:0.01:15];
hold on;
plot(xs,lognpdf(xs,S.x,S.y));

S = vpasolve([exp(x-y^2) == offAxisMode, (exp(y^2)-1)*exp(2*x+y^2) == offAxisSigma],[x y]);

xs = [0.1:0.01:15];
plot(xs,lognpdf(xs,S.x,S.y),'r');
