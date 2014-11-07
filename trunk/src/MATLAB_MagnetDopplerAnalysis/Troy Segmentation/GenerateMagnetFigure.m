%% Initialize
clear all; close all; clc;
%dbstop in PlotPlane at 11;
%dbstop in Segment_Needle at 63;

% load data for the specific trial
folder = 'V:\NeedleScan\9_26_14_MagnetScan\Scan 7\';%[cats{cat} '/' t_vals{val} '/' num2str(trial) '/']
[mat_x,mat_y,mat_slice] = Segment_Needle(folder);
% load([folder 'manual_segmentation'])
load([folder 'settings']);
matlab_pts = scanconvertpoints([mat_x,mat_y],...
    mat_slice,degPerFr,fpV,probeR3D_mm,...
    micr_x,micr_y,o_x,o_y,m,n);
% fit polynomial curves to the Matlab points
px = polyfit(matlab_pts(:,3),matlab_pts(:,1),3);
py = polyfit(matlab_pts(:,3),matlab_pts(:,2),3);

min_z = min(matlab_pts(:,3));  max_z = max(matlab_pts(:,3));
zs = [min_z:(max_z-min_z)/100:max_z]';
fit_pts = [polyval(px,zs) polyval(py,zs) zs];

figure(2);
plot3(fit_pts(:,1),fit_pts(:,3),fit_pts(:,2), 'b','LineWidth', 2);
npts = size(matlab_pts,1);
%scatter3(matlab_pts([1:1:npts],1),matlab_pts([1:1:npts],3),matlab_pts([1:1:npts],2),'x','g','LineWidth', 2);
xmin = min(matlab_pts(:,1));  xmax = max(matlab_pts(:,1));
ymin = min(matlab_pts(:,3));  ymax = max(matlab_pts(:,3));
zmin = min(matlab_pts(:,2));  zmax = max(matlab_pts(:,2));

view(30,20);
%camroll(180);

axis equal;

% cpos = campos;
% cposmag = sqrt(cpos(1)^2 + cpos(2)^2);
% cpos = [cposmag*cos(6*pi/16) cposmag*sin(6*pi/16) cpos(3)];

%xlim([-60 60]);
%ylim([0 120]);
%zlim([-60 60]);

set(0,'DefaultTextFontname', 'Times New Roman');
set(0,'DefaultAxesFontName', 'Times New Roman');
grid on;

cpos = campos;
ctar = camtarget;
cup = camup;
campos([cpos(1) cpos(2) -cpos(3)]);
camtarget(ctar);
camup(-cup);
grid off;
zlim([20 120]);

hFig = figure(2);
Pos = get(hFig, 'Position');
set(hFig, 'Position', [0 -150 Pos(3)*2 Pos(4)*2]);



% xlabel('x (mm)');
% ylabel('z (mm)');
% zlabel('x (mm)');

set(findall(gcf,'type','text'),'FontSize', 30);
set(gca,'FontSize', 30);
set(gca,'xtick',[]);
set(gca,'xticklabel',[]);
set(gca,'ytick',[]);
set(gca,'yticklabel',[]);
set(gca,'ztick',[]);
set(gca,'zticklabel',[]);
% set(gca, 'xcolor', [1 1 1]);
% set(gca, 'ycolor', [1 1 1]);
% set(gca, 'zcolor', [1 1 1]);
% set(gcf, 'Color', 'None')
export_fig MagnetVol_4.pdf -transparent
% set(findall(gcf,'type','text'),'FontSize', 12);
% set(gca,'FontSize', 12);

% campos(cpos);
% clook = camtarget-campos;
% cup = cross(clook,[1 0 0]);
% cup = cup/norm(cup);
% camup(cup);