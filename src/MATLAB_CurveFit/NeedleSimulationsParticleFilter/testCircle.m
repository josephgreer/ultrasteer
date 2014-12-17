clear; clc; close all;

a1 = [1; 0; 0];
a2 = [cos(pi/5); sin(pi/5); 0];
a3 = [0; 1; 0];

A = [a1 a2 a3];
circ = Circle3([a1 a2 a3]);

figure(1);
scatter3(A(1,:), A(2,:), A(3,:),'r');
hold on;
circ.draw(1);
axis equal;