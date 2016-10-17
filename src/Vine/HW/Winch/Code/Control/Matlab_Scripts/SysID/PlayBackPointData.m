clearvars -except positionFrames; clc; close all;

gpsFile = 'Data/EMTracker/Trial4/scan.gps2';
positionFile = 'Data/EMTracker/Trial4/positionFrames.mat';
pressureFile = 'Data/EMTracker/Trial4/Pressure.mat';
kleqFile = 'Data/EMTracker/Trial4/kleqMainSingleGamma.mat';
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
range = FindPointArrayRange(xs);

global i;
global run;

run = true;

h = figure(1);
%draw(plane, xs{1}.pos+R0*[250;0;0], [250; 250], 1);

hold on; 

pp1 = projectPointOntoPlane(plane, xs{2}.pos);
pp2 = projectPointOntoPlane(plane, xs{3}.pos);
pp3 = projectPointOntoPlane(plane, xs{4}.pos);

%scatter3([pp1(1) pp2(1) pp3(1)], [pp1(2) pp2(2) pp3(2)], [pp1(3) pp2(3) pp3(3)]);

set(h, 'KeyPressFcn', 'keyHit');
hTitle = title(sprintf('Frame 1 of %d', gpsHeader.nframes));
xlim(range(1,:));
ylim(range(2,:));
zlim(range(3,:));
daspect([1 1 1]);
grid on;
view(3);
handles = [];

psis = mod(FindActuatorAngles(xs,plane,R0),2*pi);

kleq = load(kleqFile); 
if(isfield(kleq,'params'))
    params = kleq.params;
end
kleq = kleq.kleq;
kleq0 = kleq;
% k0 = [1; 1; 1; 0.5; 150];
% leq0 = [verticalOffset*0.7*ones(3,1)];
% gamma0 = [1;1;1];
% kleq0 = vertcat(k0,leq0);
% if(params.fitMainEqLength)
%     kleq0 = vertcat(kleq0, verticalOffset);
% end
% if(params.fitGamma == 1)
%     kleq0 = vertcat(kleq0,gamma0);
% elseif(params.fitGamma == 2)
%     kleq0 = vertcat(kleq0,1);
% end
% 
% if(params.fitTorGamma)
%     kleq0 = vertcat(kleq0, 1);
% end


Pressure(find(abs(Pressure < 0.042))) = 0;

% % 5 ks,3 leqs (1 optional main tube length), (optional 3 gammas)
% lb = [0.01; 0.01; 0.01; 0.01; 0.01; verticalOffset*0.5*ones(3,1)];
% ub = [3;3;3;Inf;Inf;verticalOffset*ones(3,1)];
% if(params.fitMainEqLength)
%     lb = vertcat(lb,verticalOffset*0.8);
%     ub = vertcat(ub,verticalOffset*1.2);
% end
% if(params.fitGamma == 1)
%     lb = vertcat(lb,0.8*ones(3,1));
%     ub = vertcat(ub,2.5*ones(3,1));
% elseif(params.fitGamma == 2)
%     lb = vertcat(lb,0.8);
%     ub = vertcat(ub,2.5);
% end
% if(params.fitTorGamma)
%     lb = vertcat(lb,1);
%     ub = vertcat(ub,5);
% end
% evalFun = @(x)(EvaluateModelParameters({xs{2:end}}, Pressure(2:end,:), psis, zeros(3,1), R0, radius, verticalOffset, params, x));
% [kleq,fval] = fmincon(evalFun, kleq0, [], [], [], [], lb,ub);

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
figure(2);
hist(thetaDists);
figure(3);
hist(phiDists);
figure(4);
hist(errors/(25.4));
figure(1);

trueHandles = [];
trueHandles = drawFrames(1, xs, 100, [], trueHandles);
posCoords = cell2mat(xs);
posCoords = [posCoords.pos].';
%scatter3(posCoords(:,1),posCoords(:,2),posCoords(:,3));

estimatedHandles = [];
% estimatedHandles = drawFrames(1, estimatedPoses, 100, [], estimatedHandles);

posHandles = [];

state = 'play';
i = 1;
while(run)
    i = max(min(i,2*length(xs)),1);
    tipPos = xs{1}.pos;
    if(mod(i,2) == 0)
        ii = i/2;
        [tipPos, ~, theta, l, phi] = PressuresToTip(Pressure(ii,:), kleq(1:3),kleq(4),kleq(5), leqs, gammas, torGamma, radius, psis, R0, zeros(3,1));
        [truePhi, trueTheta] = PositionFrameToPhiTheta(xs{ii},xs{1},R0,plane);
        phiDist = rad2deg(AngleDist(truePhi,phi));
        thetaDist = rad2deg(AngleDist(theta,trueTheta));
        display(sprintf('ii = %d thetaDist = %f, phiDist = %f', ii, thetaDist, phiDist));
    else
        ii = 1;
    end
    jj = ceil(i/2);
    
    handles = drawFrames(1, {xs{ii}}, 100, [], handles);
    
%     posHandles = drawScatterPos(tipPos.', posHandles);
    estimatedHandles = drawFrames(1, {estimatedPoses{ii}}, 50, [], estimatedHandles);
       
    hTitle.String = sprintf('Frame %d [%f %f %f]', positionFrames(jj),...
        Pressure(jj,1), Pressure(jj,2), Pressure(jj,3));
    
    pause(1/30);
end

