clear; clc; close all;

theta0 = 0;
theta1 = pi/4;
theta_t = 0.5*pi/2;
thetas = linspace(theta0, theta0+theta1, 500);
thetas = fliplr(thetas);
ls = [100; 100];

figure;
hold on;

rad = 0;
for i=1:length(thetas)
    currThetas = [thetas(i); theta_t];
    
    handles.robot = [];
    DrawRobot(currThetas, ls, [0; 0], sum(ls), handles);

    if(i == length(thetas))
        xs = thetasLsToXs(currThetas, ls, [0; 0]);
        rad = norm(xs(end,:));
    end
end

thetas = linspace(theta0,theta0+2*pi,100).';
xs = rad*[cos(thetas) sin(thetas)];
plot(xs(:,1),xs(:,2));

rad = rad-ls(1);
%rad = cos(theta_t)*ls(2);

daspect([1 1 1]);

% xlim([-1500 1500]);
% ylim([-1500 1500]);

thetas = linspace(theta0,theta0+theta_t, 500);
x0 = ls(1)*[cos(theta0); sin(theta0)];
for i=1:length(thetas)
    x1 = x0+rad*[cos(thetas(i)); sin(thetas(i))];
    xs = [x0 x1].';
    plot(xs(:,1), xs(:,2),'k');
end


% thetas = linspace(theta0+theta1,theta0+theta1+theta_t, 500);
% x0 = ls(1)*[cos(theta0+theta1); sin(theta0+theta1)];
% for i=1:length(thetas)
%     x1 = x0+rad*[cos(thetas(i)); sin(thetas(i))];
%     xs = [x0 x1].';
%     plot(xs(:,1), xs(:,2),'r');
% end