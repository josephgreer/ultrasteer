%%
clear; clc; close all;

addpath('../SysID');


W0 = 1;
R0s = linspace(W0/20, W0/5, 4);
P = 1.5*6894.76;                      % in pascals
npoints = 10;

d0sFromLs = zeros(length(R0s),npoints);
for j=1:length(R0s)
    display(sprintf('j=%d',j));
    R0 = R0s(j);
    aspect_ratios = linspace(1e-2, 0.4, npoints);
    
    for i=1:length(aspect_ratios)
        L0 = R0/aspect_ratios(i);
        display(sprintf('i=%d',i));
        d0sFromLs(j,i) = GetMaxBulgeFromRAndL0(L0, R0, P, 0, 'pleated');
    end
end

save varyl0andR0wrtWmax

%%
clear; clc; close all
addpath('../SysID')
load varyl0andR0wrtWmax 

maxContractionsFromAspectRatios = zeros(size(aspect_ratios));
for i=1:length(aspect_ratios)
    maxContractionsFromAspectRatios(i) = GetE0(1,aspect_ratios(i),P,0,'pleated');
end

save varyl0andR0wrtWmax

%%
clear; clc; close all;
load varyl0andR0wrtWmax

h = figure;
subplot(2,2,1);
hold on;


legendEntries = {};
for j=1:length(R0s)
    R0 = R0s(j);
    legendEntries{j} = sprintf('R0/W_{max} = %f', R0);
    plot(aspect_ratios, d0sFromLs(j,:));
end
legend(legendEntries);
xlabel('Aspect Ratio R/L_0');
ylabel('D/W_{max}');
xlim([min(aspect_ratios) max(aspect_ratios)]);
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
ylim([0 2])
title('blank title');
box on;

%%
load varyl0andR0wrtWmax

subplot(2,2,2);
hold on;

legendEntries = {};
for j=1:length(R0s)
    R0 = R0s(j);
    legendEntries{j} = sprintf('R0/W_{max} = %f', R0);
    
    %find minimum acceptable aspect ratio for aspect ratio
    minAspectRatioIndex = min(find(d0sFromLs(j,:) < 1));
    minAspectRatio = aspect_ratios(minAspectRatioIndex);
    maxContraction = maxContractionsFromAspectRatios(minAspectRatioIndex);
    
    currAspectRatios = aspect_ratios(minAspectRatioIndex:end);
    currAspectRatios = [linspace(0.01,minAspectRatio, 30) currAspectRatios];
    e0s = zeros(size(currAspectRatios));
    
    for i=1:length(currAspectRatios)
        ar = currAspectRatios(i);
        if(ar < minAspectRatio)
            e0s(i) = ar/minAspectRatio*maxContraction;
        else
            kk = find(aspect_ratios == ar);
            e0s(i) = maxContractionsFromAspectRatios(kk);
            
        end
    end
    plot(currAspectRatios, e0s);
end
legend(legendEntries);
box on;
xlabel('Aspect Ratio R/L_0');
ylabel('e0');
xlim([min(0) max(aspect_ratios)]);
ylim([0 0.6]);
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
title('blank title');

subplot(2,2,3);
plot([1:5], [1:5]);
box on;
xlabel('x');
ylabel('y');
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
title('blank title');
subplot(2,2,4);
plot([1:5], [1:5]);
box on;
xlabel('x');
ylabel('y');
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
title('blank title');

set(h,'Position',[0 0 450 300]);
pause(0.5);

tightfig;
pause(0.5);

export_fig -transparent ActuatorPlots2.pdf