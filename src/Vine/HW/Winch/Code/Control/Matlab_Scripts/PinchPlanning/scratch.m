clear; clc; close all;

syms x y t real;

P1 = [cos(t); sin(t)];
P0 = [0; 0];

P2 = [x;y];

P01 = P1-P0;
P12 = P2-P1;

solve((dot(P01,P01)*dot(P12,P12)-dot(P01,P12)^2),t)