clear; clc; close all;

figure;
x = [0:0.01:20];
plot(x,sigmf(x, [100 100]))

figure;
x = [0:0.01:20];
plot(x, sigmf(x, [-5 5]));