clear; clc; close all;

R = SO3Exp(-[1 0 0]);
a1 = R*[1; 0; 0];
a2 = R*[cos(pi/4); sin(pi/4); 0];
a3 = R*[0; 1; 0];

A = [a1 a2 a3];
circ = Circle3([a1 a2 a3]);

figure(1);
scatter3(A(1,:), A(2,:), A(3,:));
hold on;
circ.draw(1);
axis equal;

idx = 1;
binormal = circ.c-A(:,idx);
binormal = binormal/norm(binormal);
tangent = cross(circ.N, binormal);
tangent = tangent/norm(tangent);

len = 0.4;
pt = [A(:,idx) A(:,idx)+len*tangent]';
plot3(pt(:,1), pt(:,2), pt(:,3), 'r');
pt = [A(:,idx) A(:,idx)+len*binormal]';
plot3(pt(:,1), pt(:,2), pt(:,3), 'g');
pt = [A(:,idx) A(:,idx)+len*circ.N]';
plot3(pt(:,1), pt(:,2), pt(:,3), 'b');