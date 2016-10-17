% %%
% clear; clc; close all;
% 
% addpath('../SysID');
% 
% 
% W = 42;
% W0 = 2*43/pi;
% R0 = 2.5;
% P = 1.5*6894.76;                      % in pascals
% npoints = 10;
% 
% d0sFromLs = zeros(1,npoints);
% display(sprintf('j=%d',j));
% aspect_ratios = linspace(1e-2, 0.4, npoints);
% 
% for i=1:length(aspect_ratios)
%     L0 = R0/aspect_ratios(i);
%     display(sprintf('i=%d',i));
%     d0sFromLs(1,i) = GetMaxBulgeFromRAndL0(L0, R0, P, 0, 'pleated');
% end
% 
% save varyl0wrtWmax
% 
% %%
% clear; clc; close all
% addpath('../SysID')
% load varyl0wrtWmax 
% 
% maxContractionsFromAspectRatios = zeros(size(aspect_ratios));
% for i=1:length(aspect_ratios)
%     maxContractionsFromAspectRatios(i) = GetE0(1,aspect_ratios(i),P,0,'pleated');
% end
% 
% save varyl0wrtWmax

%%
close all;
load varyl0wrtWmax
h = figure; 

%aspect_ratios(find(aspect_ratios > 0.3)) = [];


%find minimum acceptable aspect ratio for aspect ratio
minAspectRatioIndex = min(find(d0sFromLs(1,:) < W0));
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
subplot(1,3,1);
xs = (R0./currAspectRatios)/W0;
plot(xs, e0s,'k--','LineWidth',2);
xlim([min(xs) max(xs)]);

%%
hold on;
fname = '../SysID/Data/Contraction/ContractionData_2.5.txt';

data = load(fname);

ars = zeros(size(data,1),1);
contractions = zeros(size(data,1),1);
for i=1:size(data,1)
    deflatedLength = data(i,1)/data(i,3);
    inflatedLength = data(i,2)/data(i,3);
    contraction = 1-(inflatedLength/deflatedLength);
    
    ars(i) = R0/deflatedLength;
    contractions(i) = contraction;
end

scatter((R0./ars)/W0,contractions,ones(size(contractions))*75,'b','LineWidth',2);%,'MarkerFaceColor',[0 0 1]);

xlabel('l_0/W_0');
ylabel('Contraction Ratio');
set(gca, 'FontSize', 12, 'FontName', 'Times New Roman');
%title('L_0 vs. Max Cross-Section Diameter');
box on;
ylim([0 0.5]);

% %%
% %%%%%%%%%%%%%%%%%%%%%%%% Tube 1
% clear; clc;
% addpath('../SysID');
% files = rdir('../SysID/Data/Stiffness/1_2.0.txt');
% 
% for i=1:length(files)
%     mode = 'pleated';
%     fname = files(i).name;
%     display(fname)
%     pressure = sscanf(fname,'../SysID/Data/Stiffness/%d_%d.%d.txt');
%     pressure = pressure(2)/10.0+pressure(3)/100.0;      % in psi
%     pressure = 3*pressure*6894.76;                      % in pascals
%     
%     numLinks = 1.5;
%     R0 = 8/2*1e-3;                                    % in m
%     L0 = 40*1e-3;                                     % in m
%     
%     E = 0.1*1e9;                                      %elastic modulus pascals
%     s = 2e-3*0.0254;                                  % thickness in meters
%     A = 2*L0*s;                                       % cross-sectional area in m^2
%     a = pi*pressure*R0^2/(A*E);
%     
%     D0 = numLinks*GetD0(L0,R0,pressure,a,mode);
%     
%     maxContraction = 1-(D0/(numLinks*L0));
%     
%     contractions = linspace(0.04,maxContraction,5);
%     forces = zeros(size(contractions));
%     for i=1:length(contractions)
%         e = contractions(i);
%         forces(i) = numLinks*ContractionToForce(e,L0,R0,pressure,a,mode);
%     end
% end
% save PredictedForceData

%%
clearvars -except h; clc;
load PredictedForceData
    
subplot(1,3,3);
plot(contractions,forces, 'k--', 'LineWidth',2);
hold all;
for i=1:length(files)
    data = load(fname);
    es = zeros(size(data,1),1);
    forces = zeros(size(es));
    forcesReturn = zeros(size(es));
    for i=1:size(data,1)
        addedLength = 10-data(i,1); % (in cm)
        addedLength = addedLength*1e-2;
        len = D0+addedLength;
        es(i) = 1-(len/(numLinks*L0));
        forces(i) = data(i,2)*4.4452016; % in newtons
        forcesReturn(i) = data(i,3)*4.4452016; % in newtons
    end
    scatter(es,forces,ones(size(es))*75,'b','LineWidth',2);
    scatter(es,forcesReturn,ones(size(es))*75,'b','LineWidth',2);
end
xlabel('Contraction Ratio');
ylabel('Tension Force (N)');
set(gca, 'FontSize', 12, 'FontName', 'Times New Roman');
% xlim([min(es) max(es)]);

%%
subplot(1,3,2);
hold on;
fname = '../SysID/Data/Pouch/PouchData_5_8.txt';

data = load(fname);

l0 = 5.75;

xs = data(:,1)/l0;
% plot(xs, 1-(data(:,2)/l0),'b','LineWidth',2);
plot([min(xs); max(xs)], ones(2,1)*0.3636, 'k--', 'LineWidth', 2);
scatter(xs, 1-(data(:,2)/l0),ones(size(data,1),1)*75,'b','LineWidth',2);

xlabel('Pouch Width/l_0');
ylabel('Contraction Ratio');
set(gca, 'FontSize', 12, 'FontName', 'Times New Roman');
box on;
ylim([0 0.5]);
xlim([min(xs)-0.1 max(xs)+0.1]);

set(h, 'Position', [0 0 900 225]);
pause(0.2);
tightfig;
pause(0.2);

export_fig -transparent ActuatorDataFigure.pdf


