clear; clc; close all;

load CleanHoleData

r = 25;
npoints = 100;
xs = linspace(0,353,npoints).';

ys = [275];%logspace(0.01, 1, 5);
xs = xs;

h = figure(1);
subplot(2,1,1);
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

set(h, 'Position', [0 0 320, 320]);
box on;
grid on;

xlabel('Horizontal Distance to Hole/Distance To Wall');
ylabel('\theta (^{o})')

title({'Acceptable Orientation Range (\theta) vs','Horizontal Distance from Hole'});

legend('Model','Experimental');

xlim([0 353]/ys);
set(gca,'FontSize',12,'FontName','Times New Roman');


subplot(2,1,2);
hold all;
npoints = 100;
fixedPoint = y/3;
pointAimedAt = linspace(0,fixedPoint,npoints).';
angles = rad2deg(atan2(y*ones(npoints,1), fixedPoint-pointAimedAt));
uncertainties = linspace(0,10,4).';
angle1 = rad2deg(atan2(y,fixedPoint-r));
angle2 = rad2deg(atan2(y,fixedPoint+r));

mna = min(angle1,angle2);
mxa = max(angle1,angle2);
mxa = max(mxa, 90);

plot(pointAimedAt/fixedPoint, ones(npoints,1), 'LineWidth', 3);
strings = {};
for i = 1:length(uncertainties.')
    if(i == 1)
        continue;
    end
    u = uncertainties(i);
    badAngleRange = max(mna-(angles-u),0)+max(angles+u-mxa,0);
    probSuccess = (2*u-badAngleRange)/(2*u);
    plot(pointAimedAt/fixedPoint, probSuccess,'LineWidth',3);
end

xlabel('Aim point (x/x_0)');
ylabel('P_{succ}');
box on;
set(gca,'FontSize',12,'FontName','Times New Roman');

legendCell = cellstr(num2str(uncertainties, '\\Delta\\theta=%-2.2f'))
legend(legendCell);
title({'Aim point vs. probability','of successfully growing through hole'});
ylim([0.5 1.05])
xlim([0 1])
grid on;
% tightfig;

figure;
hold all;
uncertainties = linspace(1e-6,10,npoints);
pointAimedAt = linspace(0,fixedPoint,6).';
angles = rad2deg(atan2(y*ones(size(pointAimedAt,1),1), fixedPoint-pointAimedAt));

for i=1:length(pointAimedAt)
    ang = angles(i);
    badAngleRange = max(mna-(ang-uncertainties),0)+max(ang+uncertainties-mxa,0);
    probSuccess = (2*uncertainties-badAngleRange)./(2*uncertainties);
    plot(uncertainties, probSuccess, 'LineWidth', 3); 
end
legendCell = cellstr(num2str(pointAimedAt/fixedPoint, 'Point Aimed At (x/x_0)=%-2.2f'))
legend(legendCell);
xlabel('\\Delta \\Theta');
ylabel('P_{succ}');
ylim([0 1.05])
title('First idea');
grid on;
box on;




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


