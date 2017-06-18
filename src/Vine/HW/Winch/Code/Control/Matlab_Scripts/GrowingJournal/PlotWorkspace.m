clear; clc; close all;

addpath('../../../../../../../MATLAB_CurveFit/NeedleSimulationsParticleFilter/');
addpath('../SysID');

n_actuators = 3;
leq = [65; 65; 65; 100]*2;
d = 5;
ll = leq;
psis = actuatorThetas();

handles = [];
figure;
set(gca,'fontsize',22)
hold on;
daspect([1 1 1]);
xlabel('x');
ylabel('y');
zlabel('z');
grid on;

kmults = [2.5; 2.5; 2.5]/3;
kmain = 2.5/3;
ktor = 32;

activeActuators = [1 2; 1 3; 2 3];



alphas = linspace(0.15,1,12);%, 0.5];%linspace(0.5,1,2);

faceColors = ['b','b','b','b','b','b','b','b','b','b','b','b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'];
faceAlphas = linspace(0.75, 0.1, length(alphas));

nps = ones(1, length(alphas))*15;%[15, 15, 15];

boundaryPoints = [];

posses = {};

mins = [1e6 1e6 1e6];
maxs = -[1e6 1e6 1e6];

for aa = 1:length(alphas)
    alpha = alphas(aa);
    display(alpha);
    
    posses{aa} = [];
    
    npoints = nps(aa);
    
    pts = linspace(0,1,npoints).^2;
    pts(2) = pts(2)+0.01;
    pts = pts([1 3:end]);
    npoints = length(pts);
    
    [ps1, ps2] = meshgrid(pts,pts);
    ps1 = ps1(:);
    ps2 = ps2(:);
    for a = 1:size(activeActuators,1)
        poss = zeros(length(ps1),3);
        pps = zeros(length(ps1),3);
        jj = 1;
        for i=1:length(ps1)
            ps = zeros(3,1);
            ps(activeActuators(a,1)) = ps1(i);
            ps(activeActuators(a,2)) = ps2(i);
            [pos, ~, ~, ~, ~] = PressuresToTipBasicEquation(ps, kmults, kmain, ktor, alpha*leq, d, eye(3,3), zeros(3,1), n_actuators);
            poss(jj,:) = [pos(1), pos(2), pos(3)];
            pps(jj,:) = ps;
            jj = jj+1;
        end
        
        view(3);
        grid on;
        daspect([1 1 1]);
        hold on;
        
        
        pvals = linspace(1,npoints,npoints);
        [px,py] = meshgrid(pvals,pvals);
        px = px(:); py = py(:);
        
        triGrid = [];
        for i=1:length(px)
            ii = px(i);
            jj = py(i);
            
            if aa == length(alphas)
                if ii + 1 <= npoints
                    pointIndex = find(px==ii+1 & py == jj);
                    stackedPoints = [poss(i,:); poss(pointIndex,:)];
                    plot3(stackedPoints(:,1), stackedPoints(:,2), stackedPoints(:,3),'k');
                end
                if ii - 1 >= 0
                    pointIndex = find(px==ii-1 & py == jj);
                    stackedPoints = [poss(i,:); poss(pointIndex,:)];
                    plot3(stackedPoints(:,1), stackedPoints(:,2), stackedPoints(:,3),'k');
                end
                if jj + 1 <= npoints
                    pointIndex = find(px==ii & py == jj+1);
                    stackedPoints = [poss(i,:); poss(pointIndex,:)];
                    plot3(stackedPoints(:,1), stackedPoints(:,2), stackedPoints(:,3),'k');
                end
                if jj - 1 >= 0
                    pointIndex = find(px==ii & py == jj-1);
                    stackedPoints = [poss(i,:); poss(pointIndex,:)];
                    plot3(stackedPoints(:,1), stackedPoints(:,2), stackedPoints(:,3),'k');
                end
            end
            if ii+1 <= npoints && jj+1 <= npoints
                pointIndex1 = find(px == ii+1 & py == jj+1);
                pointIndex2 = find(px == ii+1 & py == jj);
                pointIndex3 = find(px == ii & py == jj+1);
                triGrid = vertcat(triGrid, [i pointIndex1 pointIndex2], [i pointIndex2 pointIndex3]);
            end
            
            if ii-1 >= 1 && jj+1 <= npoints
                pointIndex1 = find(px == ii-1 & py == jj+1);
                pointIndex2 = find(px == ii-1 & py == jj);
                pointIndex3 = find(px == ii & py == jj+1);
                triGrid = vertcat(triGrid, [i pointIndex1 pointIndex2], [i pointIndex2 pointIndex3]);
            end
            
            if ii-1 >= 1 && jj-1 >= 1
                pointIndex1 = find(px == ii-1 & py == jj-1);
                pointIndex2 = find(px == ii-1 & py == jj);
                pointIndex3 = find(px == ii & py == jj-1);
                triGrid = vertcat(triGrid, [i pointIndex1 pointIndex2], [i pointIndex2 pointIndex3]);
            end
            
            if ii+1 <= npoints && jj-1 >= 1
                pointIndex1 = find(px == ii+1 & py == jj-1);
                pointIndex2 = find(px == ii+1 & py == jj);
                pointIndex3 = find(px == ii & py == jj-1);
                triGrid = vertcat(triGrid, [i pointIndex1 pointIndex2], [i pointIndex2 pointIndex3]);
            end
        end
        
        pointIndex1 = find(px == 1 & py == 1);
        pointIndex2 = find(px == 1 & py == npoints);
        pointIndex3 = find(px == npoints & py == npoints);
        pointIndex4 = find(px == npoints & py == 1);
        boundaryPoints = horzcat(boundaryPoints,poss(pointIndex1,:).',poss(pointIndex2,:).',poss(pointIndex3,:).',poss(pointIndex4,:).');
        
        
        trisurf(triGrid,poss(:,1), poss(:,2), poss(:,3), 'EdgeColor', 'none', 'FaceAlpha',faceAlphas(aa), 'FaceColor',faceColors(aa));
%         scatter3(poss(:,1), poss(:,2), poss(:,3), 'b');
    
        posses{aa} = vertcat(posses{aa},poss);
        mins = min(mins, min(poss,[],1));
        maxs = max(maxs, max(poss,[],1));
    end
    
%     posses{aa}(find(posses{aa}(:,2) < 0),:) = [];
end

% for aa = 1:length(alphas)-1
%     indices = [1:5:size(posses{aa},1)];
%     for jj = indices
%         stacked = [posses{aa}(jj,:); posses{aa+1}(jj,:)];
%         plot3(stacked(:,1),stacked(:,2), stacked(:,3), 'k');
%     end
% end

xlim([mins(1) maxs(1)]);
ylim([mins(2) maxs(2)]);
zlim([mins(3) maxs(3)]);
view(45,15);
xlabel('x (cm)');
ylabel('y (cm)');
zlabel('z (cm)');
box on;

% na = size(alphas,2);
% nactive = size(activeActuators,1);
% for i=1:4*nactive
%     stackedPoints = zeros(na,3);
%     for j=0:na-1
%         stackedPoints(j+1,:) = boundaryPoints(:,i+j*4*nactive).';
%     end
%     plot3(stackedPoints(:,1), stackedPoints(:,2), stackedPoints(:,3),'k');
% end