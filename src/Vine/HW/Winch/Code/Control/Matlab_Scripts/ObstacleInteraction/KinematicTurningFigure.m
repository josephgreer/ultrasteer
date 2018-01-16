clear; clc; close all;

addpath('../PinchPlanning/');

path = 'points1.txt';
imFile = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/ObstaclePlanning/Data/Set2/00001.jpg';

flip = true;

im = imread(imFile);

wall = [896 57 30 75];
basePoint = [763 422];
link1 = [763 422 601 277];
link2 = [601 277 430 73];
% link2 = [601 277 466 73];

delta1 = link1(3:4)-link1(1:2);
delta1 = delta1/norm(delta1);
delta2 = link2(3:4)-link2(1:2);
delta2 = delta2/norm(delta2);

linkAngle = angleDiffSigns([delta1 0], [delta2 0])

h = figure;
set(h,'Position',[0 0 333 250]);
% imshow(im);
hold on;

points = load(path,'-ascii');
firstBad = min(find(points(:,1) == -1));
points = points-repmat(basePoint,size(points,1),2);

link1 = link1-[basePoint basePoint];
link2 = link2-[basePoint basePoint];

wall = wall-[basePoint basePoint];

points = points(1:firstBad,:);

delta1s = points(:,3:4);
delta2s = points(:,1:2)-points(:,3:4);
measuredLens = sqrt(sum(delta1s.^2,2)) + sqrt(sum(delta2s.^2,2));

ls = [];
len = 0;
dl = 10;
x = [link1.'; link2(3:4).'];
y = [0 0 0 1 0];
map = wall;
thetas = zeros(0,2);
len = 0;
wallIndex = -1;
xs = [x(1:2:end) x(2:2:end)];
handles.robot = [];

nps = 38;
modelLens = zeros(nps,1);
modelP1s = zeros(nps,2);
modelP2s = zeros(nps,2);
for i=1:nps
    [x, y, xs,wallIndex] = MoveRobotByDl(x, y, dl, map, thetas, len, wallIndex, xs);
    len = RobotLength(xs)
    modelLens(i) = len;
    modelP1s(i,:) = x(3:4).';
    modelP2s(i,:) = x(5:6).';
%     handles = DrawRobotXs(xs,-1,handles);
%     pause(0.1);
end 


drawPoints = points;
drawModelP1s = modelP1s;
drawModelP2s = modelP2s;

scalar = 1;
if(flip)
    scalar = 100/400;
    wallDelta = wall(3:4)-wall(1:2);
    wallDelta = wallDelta/norm(wallDelta);
    wallAngle = atan2(wallDelta(2),wallDelta(1));
    theta = pi-wallAngle;
    
    matrix = scalar*[cos(theta) -sin(theta); sin(theta) cos(theta)];
    
    drawPoints1 = drawPoints(:,1:2).';
    drawPoints2 = drawPoints(:,3:4).';
    
    drawPoints1 = (matrix*drawPoints1).';
    drawPoints2 = (matrix*drawPoints2).';
    drawPoints = [drawPoints1 drawPoints2];
    
    drawModelP1s = (matrix*drawModelP1s.').';
    drawModelP2s = (matrix*drawModelP2s.').';
    
    drawPoints(:,[2 4]) = -drawPoints(:,[2 4]);
    drawModelP1s(:,2) = -drawModelP1s(:,2);
    drawModelP2s(:,2) = -drawModelP2s(:,2);
    
    yep = 0;
end

ax = gca;

plot(drawModelP1s(:,1),drawModelP1s(:,2),'LineWidth',4);
ax.ColorOrderIndex = ax.ColorOrderIndex-1;
plot(drawModelP2s(:,1),drawModelP2s(:,2),'LineWidth',4);

scatter(drawPoints(1:4:end,3),drawPoints(1:4:end,4),75,'LineWidth',2);
ax.ColorOrderIndex = ax.ColorOrderIndex-1;
scatter(drawPoints(:,1),drawPoints(:,2),75,'LineWidth',2);
xlim(scalar*[-800 0]);
ylim(scalar*[0 450]);
daspect([1 1 1]);
xlabel('x (cm)');
ylabel('y (cm)');
legend('Model','Data');
set(gca,'FontSize',12,'FontName','Times New Roman');
box on;
grid on;

tightfig;

% figure;
% hold on;
% firstPoint = modelP1s(1,:);
% modelP1s(:,1) = modelP1s(:,1)-firstPoint(1);
% modelP1s(:,2) = modelP1s(:,2)-firstPoint(2);
% modelThetaPs = rad2deg(atan2(modelP1s(:,2),modelP1s(:,1)));
% 
% points(:,3) = points(:,3) - firstPoint(1);
% points(:,4) = points(:,4) - firstPoint(2);
% measuredThetaPs = rad2deg(atan2(points(:,4),points(:,3)));
% 
% plot(modelLens-modelLens(1), modelP1s(:,1));
% plot(modelLens-modelLens(1), modelP1s(:,2));
% scatter(measuredLens-modelLens(1),points(:,3));
% scatter(measuredLens-modelLens(1),points(:,4));
% 
% 
% maxX = max(measuredLens-modelLens(1));
% ylim([-40 80]);
% xlim([0 350]);
% xlim([0 300]);
% 
% 
% figure;
% hold on;
% plot(modelLens-modelLens(1), modelP2s(:,1));
% plot(modelLens-modelLens(1), modelP2s(:,2));
% scatter(measuredLens-modelLens(1),points(:,1));
% scatter(measuredLens-modelLens(1),points(:,2));
% 
% figure; 
% hold on;
% plot(modelLens-modelLens(1), modelThetaPs);
% scatter(measuredLens-measuredLens(1),measuredThetaPs)