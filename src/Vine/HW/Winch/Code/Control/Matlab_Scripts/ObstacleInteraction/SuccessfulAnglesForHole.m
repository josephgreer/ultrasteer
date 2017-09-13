clear; clc; close all;

load CleanHoleData

r = 25;
npoints = 100;
xs = linspace(0,353,npoints).';

ys = [275];%logspace(0.01, 1, 5);
xs = xs;

h = figure;
hold all;
for y=ys
    angle1 = rad2deg(atan2(y*ones(npoints,1),xs-r));
    angle2 = rad2deg(atan2(y*ones(npoints,1),xs+r));
    
    mna = min(angle1,angle2);
    mxa = max(angle1,angle2);
    
%     mna = min(mna, 90);
    mxa = max(mxa, 90);
    
    plot(xs/y,abs(mxa-mna), 'LineWidth', 5);
    
    if(y == min(ys))
        scatter(xvals/y, cspread, 200, 'o','Linewidth', 5);
    else
        scatter(xvals/y, fspread);
    end
end

set(h, 'Position', [0 0 320, 200]);
box on;
grid on;

xlabel('Horizontal Distance to Hole/Distance To Wall');
ylabel('\theta (^{o})')

title({'Acceptable Orientation Range (\theta) vs','Horizontal Distance from Hole'});

legend('Model','Experimental');

xlim([0 353]/ys);
set(gca,'FontSize',12,'FontName','Times New Roman');

tightfig;



% figure;
% hold on;
% 
% xvals = kron(xvals,[1;1;1;1]);
% ys = repmat(ys,size(xvals,1),1);
% offsets = repmat([0; 20], 10, 1);
% cangles = kron(deg2rad(cangles)+pi/2, [1;1]);
% 
% 
% arrowVals = [xvals ys] + [offsets.*cos(cangles) offsets.*sin(cangles)];
% 
% for i=1:2:size(arrowVals,1)
%     plot(arrowVals(i:i+1,1), arrowVals(i:i+1,2), 'LineWidth',4);
% end
% daspect([1 1 1]);


