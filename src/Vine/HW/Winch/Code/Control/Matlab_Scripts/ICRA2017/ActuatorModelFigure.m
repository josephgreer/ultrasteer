clear; clc; close all;

addpath('../SysID');

R0 = 8/2*1e-3;                                    % in m
L0 = 40*1e-3;                                     % in m
W0 = L0;
P = 1.5*6894.76;                      % in pascals
npoints = 10;

% e0 = GetE0(L0,R0,pressure,0,'pleated');
e0 = 0.4548;
%%
es = linspace(0.01,e0,npoints);
fs = zeros(size(es));
phirs = zeros(size(es));
ms = zeros(size(es));


phir = 1; m = 1;
for i=1:length(es)
    [phir, m] = GetPhiRandM(es(i),L0,R0,P,0,[phir;m]);
    phirs(i) = phir;
    ms(i) = m;
    fs(i) = ContractionToForce(es(i), L0, R0, P, 0, 'pleated', [phir;m]);
end

fs = fs/(P*L0^2);

plot(es, phirs);
figure;
plot(es, ms);
figure;
plot(es, fs);

R0s = linspace(R0/8,4*R0, npoints);

%%
npoints = 30;
es = linspace(0.01, e0, npoints);
forces = zeros(size(es));

for i=1:npoints
    forces(i) = ContractionToForce(es(i),L0,R0,P,0,'pleated');
end
forces = forces/(P*L0^2);
h = figure;
plot(es,forces,'k','LineWidth',2);
xlabel('Actuator Contraction (\epsilon)');
ylabel('F/(P L_0^2)');
xlim([min(es) max(es)]);
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
set(h,'Position',[0 0 450 300]);
pause(0.5);

tightfig;
pause(0.5);

export_fig -transparent ActuatorForceFig.pdf

%%
e0sFromRs = zeros(size(R0s));
l0sFromRs = zeros(size(R0s));
for i=1:length(R0s)
    display(sprintf('Rindex = %d', i));
    [currE0,currL0] = GetMaxContractionFromRAndW(R0s(i),W0,P,0);
    e0sFromRs(i) = currE0;
    l0sFromRs(i) = currL0;
end

plot(R0s, e0sFromRs);
figure;
plot(R0s, l0sFromRs);

%%
L0s = linspace(20e-3, 80e-3, npoints);
d0sFromLs = zeros(size(L0s));

for i=1:length(L0s)
    d0sFromLs(i) = GetMaxBulgeFromRAndL0(L0s(i), R0, P, 0, 'pleated');
end

plot(L0s, d0sFromLs);

%%
npoints = 20;
slendernesses = linspace(1e-2,0.5,npoints);
e0sFromSlendernesses = zeros(size(slendernesses));
for i=1:npoints
    e0sFromSlendernesses(i) = GetE0(1,slendernesses(i),P,0,'pleated')
end

plot(slendernesses, e0sFromSlendernesses);


%% 
clear; clc; close all;
load ActuatorParameters
load d0sFromLs
load e0sFromSlenderness

W0 = W0*1e-3;
R0s = R0s*1e-3;
R0 = 8/2*1e-3;

h = figure;
subplot(2,2,1);
slims = R0./L0s;
d0sFromLs = d0sFromLs/R0;
plot(slims, d0sFromLs,'k','LineWidth',2);
xlabel('R/l_0');
ylabel('D/W');
xlim([min(slims) max(slims)]);
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
%title('L_0 vs. Max Cross-Section Diameter');
box on;

subplot(2,2,2);
R0s = R0s/W0;
l0sFromRs = l0sFromRs*1e-3;
l0sFromRs = l0sFromRs/W0;
plot(R0s,R0s./l0sFromRs,'k','LineWidth',2);
xlabel('R/W');
ylabel('Minimum Slenderness (R/l_0)');
title('Actuator Radius vs. Minimum Slenderness');
xlim([min(R0s) max(R0s)]);
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
box on;

subplot(2,2,3);
plot(slendernesses,e0sFromSlendernesses,'k','LineWidth',2);
xlabel('R/l_0');
ylabel('\epsilon_{max}');
xlim([min(R0s) max(R0s)]);
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
title('Slenderness vs. Max Contraction');
box on;

subplot(2,2,4);
plot(R0s,e0sFromRs,'k','LineWidth',2);
xlabel('R/W');
ylabel('\epsilon_{max}');
xlim([min(R0s) max(R0s)]);
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
title('R_0 vs. Max Contraction');
box on;

set(h,'Position',[0 0 450 300]);
pause(0.5);

tightfig;
pause(0.5);

export_fig -transparent ActuatorFig.pdf

