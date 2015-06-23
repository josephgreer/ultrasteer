clear; clc; close all;

R = SO3Exp(-[1 0 0]);
a1 = R*[1; 0; 0];
a2 = R*[cos(pi/4); sin(pi/4); 0];

v = [1;2;3];
v = v/norm(v);

A = [a1';a2'];

figure(1);
scatter3(A(:,1), A(:,2), A(:,3));
hold on;

A = A';
len = 0.4;
pt = [A(:,1) A(:,1)+len*v]';
plot3(pt(:,1), pt(:,2), pt(:,3), 'r');

% x = [cx;cy;cz;r]
solution = fsolve(@(x)[norm(a2-x(1:3))-x(4);...
            norm(a1-x(1:3))-x(4);...
            dot(v, a1-x(1:3));
            cross(cross(a1-x(1:3),a2-x(1:3)),cross(v,(a1-x(1:3))))], ones(4,1));
c = solution(1:3);
r = solution(4);
        
        
normal = cross(a1-c,a2-c);
normal = normal/norm(normal);

by = cross((a1-c),normal);
by = by/norm(by);

bx = (a1-c);
bx = bx/norm(bx);

thetas = [0:0.01:2*pi]';
pts = r*(cos(thetas)*bx'+sin(thetas)*by')+repmat(c',size(thetas,1),1);
plot3(pts(:,1),pts(:,2),pts(:,3),'k')

v2 = cross((a2-c),normal);
v2 = v2/norm(2);
pt = [A(:,2) A(:,2)+len*v2]';
plot3(pt(:,1), pt(:,2), pt(:,3), 'r');
axis equal;



        

