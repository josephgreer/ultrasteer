clear; clc; close all;
p = Poly3([3 3 3; 2 2 2; 1 1 1]);
% q = Poly3([1 1 1; 0 0 0]);
% plus = p+q
% times = p.*q
% eval = p.eval(3)
dds = p.deriv()
%yep = p.curvature()


A = magic(3)
p = Poly3.ThroughPoints(A)
p.eval(0)
p.eval(0.5)
p.eval(1)

p1 = [-1 0 0]; p2 = [0 1 0]; p3 = [1 0 0];
p = Poly3.ThroughPoints([p1; p2; p3;]);
epoints = p.eval([0:0.01:1.2]');
plot(epoints(:,1),epoints(:,2));
axis equal;
hold on;
[T N roc] = p.frenet(0.5);
pt = p.eval(0.5);
pt = pt(1:2);
ptT = pt+T(1:2)*0.1;
ptN = pt+N(1:2)*0.1;
plot([pt(1);ptT(1)], [pt(2);ptT(2)], 'r');
plot([pt(1);ptN(1)], [pt(2);ptN(2)], 'g');