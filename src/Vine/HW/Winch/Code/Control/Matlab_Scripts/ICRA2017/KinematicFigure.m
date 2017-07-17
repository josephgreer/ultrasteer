clearvars -except positionFrames; clc; close all;

addpath('../SysID/');

gpsFile = '../SysID/Data/EMTracker/Trial4/scan.gps2';
positionFile = '../SysID/Data/EMTracker/Trial4/positionFrames.mat';
pressureFile = '../SysID/Data/EMTracker/Trial4/Pressure.mat';
kleqFile = '../SysID/Data/EMTracker/Trial4/kleqMainSingleGamma.mat';
verticalOffset = 16.5*25.4;
radius = 3/pi*25.4;


load(pressureFile);
load(positionFile);
params = initParams();

% Pressure(7,:) = [];
% positionFrames(7) = [];
% Pressure(35,:) = [];
% positionFrames(35) = [];
% Pressure(17,:) = [];
% positionFrames(17) = [];

addpath('../../../../../../../MATLAB_CurveFit/LabelData/');
addpath('../../../../../../../MATLAB_CurveFit/NeedleSimulationsParticleFilter');

fgps = fopen(gpsFile);
gpsHeader = ReadHeader(fgps);

[xs, R0, plane] = preProcessPointData(gpsHeader,fgps,positionFrames,verticalOffset,radius);

psis = mod(FindActuatorAngles(xs,plane,R0),2*pi);

kleq = load(kleqFile); 
if(isfield(kleq,'params'))
    params = kleq.params;
end
kleq = kleq.kleq;


Pressure(find(abs(Pressure < 0.042))) = 0;

offset = 6;
if(params.fitMainEqLength)
   leqs = [kleq(offset:offset+3)];
   offset = offset+4;
else
   leqs = [kleq(offset:offset+2);verticalOffset];
   offset = offset+3;
end

if(params.fitGamma == 1)
    gammas = kleq(offset:offset+2);
    offset = offset+3;
elseif(params.fitGamma == 2)
    gammas = kleq(offset)*ones(3,1);
    offset = offset+1;
else
    gammas = ones(3,1);
end

if(params.fitTorGamma)
    torGamma = kleq(offset);
else
    torGamma = 1;
end

estimatedPoses = {};

thetaDists = [];
phiDists = [];
errors = [];
error = 0;
numIndices = 0;
for i=1:length(xs)
    
        [tipPos, RR, theta, l, phi] = PressuresToTip(Pressure(i,:), kleq(1:3),kleq(4),kleq(5), leqs, gammas, torGamma, radius, psis, R0, zeros(3,1));
        [truePhi, trueTheta] = PositionFrameToPhiTheta(xs{i},xs{1},R0,plane);
        
        if(i > 1)
            phiDists = vertcat(phiDists, rad2deg(AngleDist(phi,truePhi)));
            thetaDists = vertcat(thetaDists, rad2deg(AngleDist(theta, trueTheta)));
        end
        
        curr.pos = tipPos;
        curr.q = RotationMatrixToQuat(RR);
        estimatedPoses = vertcat(estimatedPoses,curr);
        
        display(num2str(i));
        realPos = xs{i}.pos;
        if(i > 1)
            currError = norm(norm(xs{i}.pos-curr.pos));
            error = error + currError;
            errors = vertcat(errors,currError);
            numIndices = numIndices+1;
        end
end
display(sprintf('error = %f %f',error,error/numIndices));
% figure(2);
% hist(thetaDists);
% figure(3);
% hist(phiDists);
% figure(4);
% hist(errors/(25.4));
% figure(1);


h = figure(1);
set(h,'Position',[0 0 320*2 250*2]);
set(gca,'FontSize',12*2,'FontName','Times New Roman');
%subplot(1,3,[1 2]);

xs = RotatePoints(xs,R0,1/10);
x0.pos = zeros(3,1);
x0.q = RotationMatrixToQuat(eye(3));
rangePoints = horzcat(xs,{x0});

% workspacePoints = GenerateRandomPointsInWorkspace(500,kleq(1:3),kleq(4),kleq(5)/10, leqs, gammas, torGamma, radius, psis, R0, zeros(3,1));
% workspacePoints = RotatePoints(workspacePoints,R0,1/10);
% workspacePoints = cell2mat(workspacePoints);
% workspacePoints = [workspacePoints.pos].';
% 
range = FindPointArrayRange(rangePoints,5);
hold on;
% 
% % K = delaunay(workspacePoints(:,1), workspacePoints(:,2), workspacePoints(:,3));
% %tetramesh(K,workspacePoints,'FaceColor',[0.7,0.7,0.7],'FaceAlpha',0.1,'EdgeColor','none');%,workspacePoints(:,1), workspacePoints(:,2), workspacePoints(:,3));
% K = convhull(workspacePoints(:,1), workspacePoints(:,2), workspacePoints(:,3));
% trimesh(K,workspacePoints(:,1),workspacePoints(:,2),workspacePoints(:,3),'FaceColor',[0.7,0.7,0.7],'FaceAlpha',0.1,'EdgeColor','none');
% tetramesh(K,workspacePoints,'FaceColor',[0.7,0.7,0.7],'FaceAlpha',0.1,'EdgeColor','none');%,workspacePoints(:,1), workspacePoints(:,2), workspacePoints(:,3));

workspacePoints = GenerateRandomPointsInWorkspace(100,kleq(1:3),kleq(4),kleq(5), leqs, gammas, torGamma, radius, psis, R0, zeros(3,1));
workspacePoints = RotatePoints(workspacePoints,R0,1/10);
workspacePoints = cell2mat(workspacePoints);
workspacePoints = [workspacePoints.pos].';

% K = delaunay(workspacePoints(:,1), workspacePoints(:,2), workspacePoints(:,3));
% tetramesh(K,workspacePoints,'FaceColor',[0.7,0.7,0.7],'FaceAlpha',1,'EdgeColor','none');%,workspacePoints(:,1), workspacePoints(:,2), workspacePoints(:,3));
K = convhull(workspacePoints(:,1), workspacePoints(:,2), workspacePoints(:,3));
trimesh(K,workspacePoints(:,1),workspacePoints(:,2),workspacePoints(:,3),'FaceColor',[0.7,0.7,0.7],'FaceAlpha',1,'EdgeColor','none');

drawFrames(1, {x0}, 7.5, [], []);

trueHandles = [];
%trueHandles = drawFrames(1, xs, 7.5, [], trueHandles);
truePosCoords = cell2mat(xs);
truePosCoords = [truePosCoords.pos].';
truePosCoords = truePosCoords*1.05;
truePosCoords(1,:) = [];
scatter3(truePosCoords(:,1),truePosCoords(:,2),truePosCoords(:,3),100,errors/10,'filled','MarkerEdgeColor',[0 0 0]);%'MarkerFaceColor',[0 .75 .75]);
view(3);
xlim(range(1,:));
ylim(range(2,:));
zlim([0 range(3,2)]);
xlabel('x (cm)');
ylabel('y (cm)');
zlabel('z (cm)');
view([-19 37]);
daspect([1 1 1]);
grid on;
box on;

% subplot(1,2,2);
% grid on;
% box on;
% estimatedPoses = RotatePoints(estimatedPoses,R0);
% estimatedPosCoords = cell2mat(estimatedPoses);
% estimatedPosCoords = [estimatedPosCoords.pos].';
% estimatedPosCoords(1,:) = [];
% scatter3(estimatedPosCoords(:,1),estimatedPosCoords(:,2),estimatedPosCoords(:,3));
% % estimatedHandles = [];
% % estimatedHandles = drawFrames(1, estimatedPoses, 75, [], estimatedHandles);
% view(3);
% xlim(range(1,:));
% ylim(range(2,:));
% zlim(range(3,:));
% view([-50 14]);
% daspect([1 1 1]);

caxis([0 max(errors/10)]);
hh = colorbar;
ylabel(hh,'Error (cm)');
set(hh,'YTick',[0 16]);
pause(0.1);
% tightfig;
% print(gcf, '-dbmp256', 'KinematicDataFigureScatterOnly.png','-r300');
%%
% subplot(1,3,3);
% hold on;
% hHist = histogram(errors/(10));
% set(hHist,'FaceColor',[0.5 0.5 0.5]);
% xlabel('Error (cm)');
% ylabel('Number of Samples');
% set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
% minMax = [min(hHist.BinEdges) max(hHist.BinEdges)];
% % plot(minMax, ones(1,2)*error/(10*numIndices),'r--','LineWidth',2);
% xlim(minMax);
% box on;
% 
% set(h,'Position',[0 0 1*450 250]);
% pause(0.1);
% tightfig;
%print(gcf, '-dpdf', 'KinematicDataFigure.pdf');

% export_fig -transparent KinematicDataFigure.png



