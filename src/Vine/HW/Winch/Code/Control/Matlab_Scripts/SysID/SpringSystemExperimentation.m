clear; clc; close all;

addpath('../../../../../../../MATLAB_CurveFit/NeedleSimulationsParticleFilter/');

n_actuators = 3;
leq = [80; 80; 80; 100];
ks = [0; 0; 0; 1; 10];
d = 5;
ll = leq;
psis = actuatorThetas();

system = @(l)(SpringSystemEquations(l, ks, leq, d));
[ls, fval] = fsolve(system, leq(1:n_actuators));
ls = real(ls);
% xs = ls;

systemFull = @(x)(FullSpringSystemEquations(x, ks, leq, d, psis,1));
[xs, fval] = fsolve(systemFull, leq(1:n_actuators));
xs = real(xs);
ls = xs(1:3);

targetPos = [0;0;200];

handles = [];
figure;
handles = drawVine(xs,d,handles);
daspect([1 1 1]);
view([15 45]);
xlim([-80 80]);
ylim([-80 80]);
zlim([0 250]);
xlabel('x');
ylabel('y');
zlabel('z');
grid on;
htitle = title(sprintf('k = [%f %f %f], l = [%f %f %f], fval=[%f %f %f]', ks(1), ks(2), ks(3),...
    ls(1), ls(2), ls(3), fval(1), fval(2),fval(3)));

scatter3(targetPos(1),targetPos(2),targetPos(3));
hold on;

while(true)
    char = getkey(1);
    if(char == 'q')
        ks(1) = ks(1)+0.05;
    elseif(char == 'a')
        ks(1) = ks(1)-0.05;
    elseif(char == 'w')
        ks(2) = ks(2)+0.05;
    elseif(char == 's')
        ks(2) = ks(2)-0.05;
    elseif(char == 'e')
        ks(3) = ks(3)+0.05;
    elseif(char == 'd')
        ks(3) = ks(3)-0.05;
    else
        break;
    end
    system = @(l)(SpringSystemEquations(l, ks, leq, d));
    ls = fsolve(system, ls);
    ls = real(ls);
%     xs = ls;
    
    systemFull = @(x)(FullSpringSystemEquations(x, ks, leq, d, psis,1));
    [xs, fval] = fsolve(systemFull, leq(1:n_actuators));
    xs = real(xs);
    ls = xs(1:3);
    
    assert(norm(xs(1:3)-ls)<1e-4);
    
    handles = drawVine(xs,d,handles);
    set(htitle, 'String', sprintf('k = [%f %f %f], l = [%f %f %f], fval=[%f %f %f]', ks(1), ks(2), ks(3),...
    ls(1), ls(2), ls(3), fval(1), fval(2),fval(3)));
end
