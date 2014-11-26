%close all;
%clearvars -except points fname needlePtDists rmsErrors transducerAngles;
clc;
f = strcat(fname, '.gps1');
fgps = fopen(f, 'r');
f = strcat(fname, '.b8');
fim = fopen(f, 'r');

mpp = 83;
gpsheader = ReadHeader(fgps);

% needleTipFname = 'Y:/NeedleScan/gpstests/test4.gps1';
% fnt = fopen(needleTipFname, 'r');
% ntheader = ReadHeader(fnt);
% for i=1:ntheader.nframes
%     gpsnt = RPreadgpsindex(fnt, ntheader, i);
%     fprintf('pos x pos y posz %f %f %f\n', gpsnt.pos(1), gpsnt.pos(2), gpsnt.pos(3));
% end

if(~exist('nskip'))
    nskip = 1;
end


fitPoints = [];
indices = [];

firstGps = [];
for i=1:floor(gpsheader.nframes/nskip)
    gps = RPreadgpsindex(fgps, gpsheader,1+(i-1)*nskip);
    if(i == 1)
        firstGps = gps;
    end
    if(points(i,3) > 0) 
        pt = MapImageCoordToWorldCoord(points(i, [1 2]), gpsheader, mpp, gps);
        fitPoints = [fitPoints; transpose(pt)];
        indices = [indices; i];
    end
end

figure(1);
scatter3(fitPoints(:,1), fitPoints(:,2), fitPoints(:,3));
hold on;
%scatter3(gpsnt.pos(1), gpsnt.pos(2), gpsnt.pos(3), 300, 'k');

%%% Use x as descriptor
descriptor.dir = [1; 0; 0];
descriptor.offset = [0;0;0];
degree = 3;
poly = FitPoly(fitPoints, degree, descriptor);
tvals = TvalsFromDescriptor(fitPoints,descriptor);
range = [min(tvals), max(tvals)];
pPoints = PolyPoints(range, poly, 200);
plot3(pPoints(:,1), pPoints(:, 2), pPoints(:, 3),'r');
%polyRobust = FitPolyRobust(fitPoints, degree, descriptor);
%pPoints = PolyPoints(range, polyRobust, 200);
%plot3(pPoints(:,1), pPoints(:, 2), pPoints(:, 3),'g');

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
fshowAcc = @(hObj, event, ax)ShowAccuracyOfFrame(fitPoints, fgps, fim, poly, descriptor, degree, mpp, indices)
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


set(datacursormode, 'UpdateFcn', @(obj,event_obj)(DataCursorIndex(obj,event_obj,indices, fitPoints)));

oracleModel.model = poly;
oracleModel.range = range;
oracleModel.descriptor = descriptor;
% set(gca, 'CameraPosition', cameraPos);
% set(gca, 'CameraPositionMode', 'manual');
% set(gca, 'CameraTarget', cameraLookVector);
% set(gca, 'CameraUp', cameraUpVector);
