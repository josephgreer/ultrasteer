clear; clc; close all;

syms L r D theta real;


rs = linspace(40,1e4,100);
L = 100;
D = 5;
r2s = rs+D/2;
r1s = rs-D/2;

thetas = L./rs;

plot(rs, thetas.*(r2s-r1s))


figure;
hold on;
tubeHandles = [];


daspect([1 1 1]);