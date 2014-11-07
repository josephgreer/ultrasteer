close all;
clearvars -except fitPoints;
clc;
fname = 'Y:/NeedleScan/Jan16_Scan1/Scan 6/scan';
f = strcat(fname, '.gps1');
fgps = fopen(f, 'r');
f = strcat(fname, '.b8');
fim = fopen(f, 'r');

mpp = 83;
gpsheader = ReadHeader(fgps);


points = [];
indices = [];

firstGps = [];
for i=1:size(fitPoints,1)
    indices = [indices; fitPoints(i,5)];
    gps = RPreadgpsindex(fgps, gpsheader, fitPoints(i,5));
    if(i == 1)
        firstGps = gps;
    end
    pt = MapWorldCoordToImageCoord(transpose(fitPoints(i,[1:3])), gpsheader, mpp, gps);
    points = [points; pt];
end

figure(1);
scatter3(fitPoints(:,1), fitPoints(:,2), fitPoints(:,3));
hold on;

%%% Use x as descriptor
descriptor.dir = [1;0;0];
descriptor.offset = [0;0;0];

degree = 3;
poly = FitPoly(fitPoints, degree, descriptor);
range = [min(fitPoints(:,1)), max(fitPoints(:,1))];
pPoints = PolyPoints(range, poly, 200);
plot3(pPoints(:,1), pPoints(:, 2), pPoints(:, 3),'r');
polyRobust = FitPolyRobust(fitPoints, degree, descriptor);
pPoints = PolyPoints(range, polyRobust, 200);
plot3(pPoints(:,1), pPoints(:, 2), pPoints(:, 3),'g');

xlabel('x');
ylabel('y');
zlabel('z');
axis equal;

len = 0;
for i=1:size(pPoints)-1
    len = len+norm(pPoints(i,:)-pPoints(i+1,:));
end

ul = MapImageCoordToWorldCoord([gpsheader.ul(1) gpsheader.ul(2)], gpsheader, mpp, firstGps);
bl = MapImageCoordToWorldCoord([gpsheader.ul(1) gpsheader.br(2)], gpsheader, mpp, firstGps);
ur = MapImageCoordToWorldCoord([gpsheader.br(1) gpsheader.ul(2)], gpsheader, mpp, firstGps);


fabove = @(hObj,event,ax)SetCam(1, ul, bl, ur, pPoints(100,:));
fside = @(hObj,event,ax)SetCam(0, ul, bl, ur, pPoints(100,:));
fshowAcc = @(hObj, event, ax)ShowAccuracyOfFrame(fitPoints, fgps, fim, poly, descriptor, degree, mpp, indices);
fWorstFits = @(hObj, event, ax)WorstFits(fgps, fim, fitPoints, points, indices, poly, descriptor, degree, mpp);
fDrawFrame = @(hObj, event, ax)DrawFrame(fgps, fim, mpp, 1);
fDrawFrame = @(hObj, event, ax)DrawFrame(fgps, fim, mpp, 1);
fPlotScores = @(hObj, event, ax)PlotScores(fgps, fitPoints, indices, poly, descriptor, degree, mpp);
fPlotTravel = @(hObj, event, ax)PlotFrameTravelAmount(gpsheader, mpp, fgps, indices);
fTestAccuracy = @(hObj, event, ax)(TestAccuracyOfGuess(fitPoints, fgps, fim, poly, descriptor, degree, mpp, indices));
fMoveAlongTangent = @(hObj, event, ax)(MovePointAlongCurveTangent(fitPoints, fgps, fim, poly, descriptor, degree, mpp, indices));


%%%%% Camera parameters
uicontrol('Style', 'pushbutton', 'String', 'LookFromAbove', 'Position', [20 20 100 30], 'Callback', fabove);
uicontrol('Style', 'pushbutton', 'String', 'LookFromSide', 'Position', [20 60 100 30], 'Callback', fside);
uicontrol('Style', 'pushbutton', 'String', 'View Accuracy Of Label', 'Position', [20 100 150 30], 'Callback', fshowAcc);
uicontrol('Style', 'pushbutton', 'String', 'Worst Fits', 'Position', [380 20 100 30], 'Callback', fWorstFits);
uicontrol('Style', 'pushbutton', 'String', 'Draw Frame', 'Position', [20 140 100 30], 'Callback', fDrawFrame);
uicontrol('Style', 'pushbutton', 'String', 'Plot Scores', 'Position', [380 60 100 30], 'Callback', fPlotScores);
uicontrol('Style', 'pushbutton', 'String', 'Plot Frame Travel', 'Position', [380 100 100 30], 'Callback', fPlotTravel);
uicontrol('Style', 'pushbutton', 'String', 'Test Guess', 'Position', [380 140 100 30], 'Callback', fTestAccuracy);
uicontrol('Style', 'pushbutton', 'String', 'Move Guess Along Tangent', 'Position', [380 180 100 30], 'Callback', fMoveAlongTangent);


set(datacursormode, 'UpdateFcn', @(obj,event_obj)(DataCursorIndex(obj,event_obj,indices,fitPoints)));
% set(gca, 'CameraPosition', cameraPos);
% set(gca, 'CameraPositionMode', 'manual');
% set(gca, 'CameraTarget', cameraLookVector);
% set(gca, 'CameraUp', cameraUpVector);
