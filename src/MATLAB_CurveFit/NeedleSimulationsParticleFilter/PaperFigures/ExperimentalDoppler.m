clear; clc; close all;

load dopplerFigureWorkspace

figHandle = figure;
subplot(1,2,1);
hold on;
[Fpos Xipos] = ksdensity(data(onNeedles,1));%,'bandwidth',1);
plot(Xipos, Fpos,'LineWidth',2)
res = [Xipos; Fpos];

[Fneg Xineg] = ksdensity(data(offNeedles,1));%, 'bandwidth',1);
plot(Xineg, Fneg, 'r','LineWidth',2);
xlabel('d');
ylabel('P');
res = [Xineg; Fneg];
box on;
xlim([min(min(Xineg,Xipos)),max(max(Xineg,Xipos))]);
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
legend('P(d | i)', 'P(d | ni)','Location','northwest');

subplot(1,2,2);
xvals = [-1:1e-2:12]';
vals = mnrval(B,xvals);
plot(xvals,vals(:,1),'b',xvals,vals(:,2),'r','LineWidth',2);
xlabel('d');
ylabel('P');
xlim([-1 12]);
ylim([0 1.5]);
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
box on;
legend('P(i | d)', 'P(ni | d)','Location','northwest');
set(figHandle, 'Position', [100 100 750 240]);
tightfig;
%export_fig -transparent ExperimentalDopplerProb.pdf