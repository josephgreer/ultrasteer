clear; clc; close all;

xs = linspace(-2,2,1000);
xs = 10.^xs;
ys = 1./(xs+10);

plot(xs,ys);

set(gca,'xscale','log');
set(gca,'yscale','log');