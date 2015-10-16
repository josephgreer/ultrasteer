% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
% Troy Adebar
% Stanford University
% October 2, 2015
%
% This script analyzes data saved by Teleoperation2DWidget in the
% RTUltrasteer project, in order to characterize the process noise for
% motion of the needle tip
% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
%% Initialize
clc; clear all;
basedir = 'F:\Dropbox\Troy Adebar Research Folder\1 - Kalman Filter\Data\2015-10-05 NS2 Process Noise\Reg1\';
modelpath = '..\Matlab Model\';
utilpath = '..\Matlab Model\Utility Functions';
addpath(modelpath);
addpath(utilpath);
Wp = [];
rs = [];
lims = [103 100 100 100 100 100;
        146 140 140 136 143 140];

% Loop through runs
for run = 1:6
    % Clear variables
    clear xact xest u x xcor xpred xerr;
    
    % Load and process data
    rstr = num2str(run);
    file = 'xest.m';
    path = fullfile(basedir,rstr,file);
    temp = importdata(path, ' ', 2);
    xest = temp.data;
    xest = reshape(xest,4,4,[]);
    
    file = 'xact.m';
    path = fullfile(basedir,rstr,file);
    temp = importdata(path, ' ', 2);
    xact = temp.data;
    xact = reshape(xact,4,4,[]);
    
    file = 'u.m';
    path = fullfile(basedir,rstr,file);
    temp = importdata(path, ' ', 2);
    u = temp.data;
    u = reshape(u,3,1,[]);
    
    % Correct initial registration error between estimate and in-needle GPS
    Te = xact(:,:,3)^-1*xest(:,:,3);
    for i = 1:length(xact)
        xcor(:,:,i) = xact(:,:,i)*Te;
    end
    
    % Crop extents of data to examine
    i_start = find(squeeze(xcor(3,4,:))>lims(1,run),1);
    i_end = find(squeeze(xcor(3,4,:))>lims(2,run),1);
    x = xcor(:,:,i_start:i_end);
    
    % Fit a circle to the identified points
    [ r,c,circ_pts,~] = findCurvature(squeeze(x(1:3,4,:))',40);
    rs = [rs; r];
    
    % Loop through points and find error between predicted and actual
    for i = i_start:i_end
        xpred(:,:,i) = f(xcor(:,:,i-1),u(:,:,i),zeros(6,1));
        xerr(:,:,i) = xcor(:,:,i)^-1*xpred(:,:,i);
        if(u(3,1,i)>0)
            errvec = [xerr(1:3,4,i); mat2vec(xerr(1:3,1:3,i))];
            if norm(errvec) > 0 
                Wp = cat(3,Wp,state2trans(errvec));
                Wp = cat(3,Wp,state2trans(-errvec));
            end
        end
    end
    if run == 6
        % Plot the first run
        hfig = figure(1); clf; hold on;
        % original data points
        plot3(squeeze(xcor(1,4,:)),squeeze(xcor(2,4,:)),squeeze(xcor(3,4,:)),'k.');
        % best-fit circle
        hold on;
        plot3(circ_pts(:,1),circ_pts(:,2),circ_pts(:,3),'k-');
        grid on; box on; 
        xlabel('x direction (mm)');
        ylabel('y direction (mm)');
        zlabel('z direction (mm)');
        set(gcf,'color','w');
        axis([-40 40 -40 40 80 150]);
        axis equal;
        view(75,10);
        set(findall(gcf,'-property','FontSize'),'FontSize',14,...
    'fontName','Times New Roman')

    end
end

%% Summarize and save results
[mu_we, Qe, W] = meancov(Wp);
display(mu_we);
display(Qe);
save('Results\ProcessNoise','mu_we','Qe','W');

%% Plot errors
hfig2 = figure(2); clf; hold on;
plot3(squeeze(Wp(1,4,:)),squeeze(Wp(2,4,:)),squeeze(Wp(3,4,:)),'k.');
xlabel('x direction (mm)'); 
ylabel('y direction (mm)'); 
zlabel('z direction (mm)');
grid on;
set(gcf,'color','w');
view(50,20);
axis([-3 3 -3 3 -3 3]);
axis equal;
set(findall(gcf,'-property','FontSize'),'FontSize',14,...
    'fontName','Times New Roman')

%% Convert Covariance matrix to degrees
rad2deg = ones(6,6);
rad2deg(4:6,4:6) = (180/pi)^2;
rad2deg(4:6,1:3) = (180/pi);
rad2deg(1:3,4:6) = (180/pi);

Qe = Qe.*rad2deg


