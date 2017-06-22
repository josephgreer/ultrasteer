clear; clc; close all;

addpath('../../../../../../../MATLAB_CurveFit/NeedleSimulationsParticleFilter/');
addpath('../SysID');

n_actuators = 3;
leq = 2*[65; 65; 65; 100];
d = 5;
ll = leq;
psis = actuatorThetas();

handles = [];
h = figure;
set(h,'Position',[0 0 900 300]);
for pp =1:2
    subplot(1,2,pp);
    set(gca,'fontsize',22)
    hold on;
    daspect([1 1 1]);
    xlabel('x');
    ylabel('y');
    zlabel('z');
    grid on;
end

kmults = [2.5; 2.5; 2.5]/3;
kmain = 2.5/3;
ktor = 32;

activeActuators = [1 2; 1 3; 2 3];

alphas = 1;%linspace(0.15,1,12);%, 0.5];%linspace(0.5,1,2);

faceColors = ['b','b','b','b','b','b','b','b','b','b','b','b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'];
faceAlphas = 0.25;%linspace(0.75, 0.1, length(alphas));

controlParams.targetPosition = [0;0;1e6+200];
controlParams.targetGoal = [0;0];
controlParams.targetSize = 25;
controlParams.focalLength = 182/5; % expressed in pixels
controlParams.kp = 0.5;
controlParams.q = zeros(3,1);

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
    
%     systemFull = @(x)(FullSpringSystemEquations(x, [0.05;0;0;kmain;ktor], leq, d, psis,1));
%     [xs, fval] = fsolve(systemFull, leq(1:n_actuators));
%     xs = real(xs);
%     ls = xs(1:3);
%     [theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,d,psis);
%     J0 = JacobianVisual(controlParams,ls,[zeros(3,1);kmain;ktor],1,leq,d,psis);
%     angles0 = zeros(3,1); magnitudes0 = zeros(3,1);
%     for col = 2:4
%         angles0(col-1) = atan2(J0(2,col),J0(1,col));
%         magnitudes0(col-1) = norm(J0(:,col));
%     end
    
    % this is the point we're taking approximate jacobian around
    ks = zeros(3,1);
    k0 = [0.00; 0.00; 0.00];
    systemFull = @(x)(FullSpringSystemEquations(x, [k0;kmain;ktor], leq, d, psis,1));
    [xs, fval] = fsolve(systemFull, leq(1:n_actuators));
    xs = real(xs);
    ls = xs(1:3);
    [theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,d,psis);
    RR = KinematicParametersToTipFrame(phi,theta,l);
    tip = kinematicParametersToTipPoint(l,phi,theta,eye(3),zeros(3,1));
    controlParams.targetPosition = tip+RR*[0;0;1e6];
    im0 = KinematicParametersToImagePoint(phi,theta,l,controlParams);
    
    J0 = zeros(2,4);
    
    alphs = [1.0001; 1; 1; 1];
    kk = diag([0 0.0001 0.0001 0.0001]);
    kk = kk(2:end,:);
    kk = repmat(k0,1,4)+kk;
    for i=1:4
        alpha = alphs(i);
        kcurr = kk(:,i).*kmults;
        ks(1:3) = kcurr;
        kcurr = ks/alpha;
        leqs = leq*alpha;
        
        
        systemFull = @(x)(FullSpringSystemEquations(x, [ks;kmain;ktor], leqs, d, psis,1));
        [xs, fval] = fsolve(systemFull, leq(1:n_actuators));
        xs = real(xs);
        ls = xs(1:3);
        [theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,d,psis);
        im = KinematicParametersToImagePoint(phi,theta,l,controlParams);
        
        J0(:,i) = im-im0;
        if i==1
            J0(:,i) = J0(:,i)/abs(alpha-1);
        end
        if(i > 1)
            J0(:,i) = J0(:,i)/norm((kk(:,i)-k0).*kmults);
        end 
    end
    
    angles0 = zeros(3,1); magnitudes0 = zeros(3,1);
    for col = 2:4
        angles0(col-1) = wrapTo2Pi(atan2(J0(2,col),J0(1,col)));
        magnitudes0(col-1) = norm(J0(:,col));
    end
        
    
    for a = 1:size(activeActuators,1)
        poss = zeros(length(ps1),3);
        pps = zeros(length(ps1),3);
        angErrors = zeros(length(ps1),1);
        magErrors = zeros(length(ps1),1);
        
        jj = 1;
        for i=1:length(ps1)
            ps = zeros(3,1);
            ps(activeActuators(a,1)) = ps1(i);
            ps(activeActuators(a,2)) = ps2(i);
            [pos, ~, ~, ~, ~] = PressuresToTipBasicEquation(ps, kmults, kmain, ktor, alpha*leq, d, eye(3,3), zeros(3,1), n_actuators);
            poss(jj,:) = [pos(1), pos(2), pos(3)];
            pps(jj,:) = ps;
            
            ks = kmults.*ps;    
            systemFull = @(x)(FullSpringSystemEquations(x, [ks;kmain;ktor], leq, d, psis,1));
            [xs, fval] = fsolve(systemFull, leq(1:n_actuators));
            xs = real(xs);
            ls = xs(1:3);
            [theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,d,psis);
            
            RR = KinematicParametersToTipFrame(phi,theta,l);
            tip = kinematicParametersToTipPoint(l,phi,theta,eye(3),zeros(3,1));
            controlParams.targetPosition = tip+RR*[0;0;1e6];
            
            Jcurr = JacobianVisual(controlParams,ls,[ks;kmain;ktor],1,leq,d,psis);
            
            if(norm(ps) < 1e-4)
                % this is the point we're taking approximate jacobian around
                k0 = ps;
                ks(1:3) = k0.*kmults;
                systemFull = @(x)(FullSpringSystemEquations(x, [ks;kmain;ktor], leq, d, psis,1));
                [xs, fval] = fsolve(systemFull, leq(1:n_actuators));
                xs = real(xs);
                ls = xs(1:3);
                [theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,d,psis);
                arcSpace0 = [phi; theta; l; ls(1); ls(2); ls(3)];
                im0 = KinematicParametersToImagePoint(phi,theta,l,controlParams);
                
                Jvis = zeros(2,4);
                
                alphs = [1.0001; 1; 1; 1];
                kk = diag([0 0.0001 0.0001 0.0001]);
                kk = kk(2:end,:);
                kk = repmat(k0,1,4)+kk;
                for bb=1:4
                    alpha = alphs(bb);
                    kcurr = kk(:,bb).*kmults;
                    ks(1:3) = kcurr;
                    kcurr = ks/alpha;
                    leqs = leq*alpha;
                    
                    
                    systemFull = @(x)(FullSpringSystemEquations(x, [ks;kmain;ktor], leqs, d, psis,1));
                    [xs, fval] = fsolve(systemFull, leq(1:n_actuators));
                    xs = real(xs);
                    ls = xs(1:3);
                    [theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,d,psis);
                    im = KinematicParametersToImagePoint(phi,theta,l,controlParams);
                    
                    Jvis(:,bb) = im-im0;
                    if bb==1
                        Jvis(:,bb) = Jvis(:,bb)/abs(alpha-1);
                    end
                    if(bb > 1)
                        Jvis(:,bb) = Jvis(:,bb)/norm((kk(:,bb)-k0).*kmults);
                    end
                end
                Jcurr = Jvis;
            end
            
            cAngErrors = zeros(3,1); cMagErrors = zeros(3,1);
            for col = 2:4
                cAng = wrapTo2Pi(atan2(Jcurr(2,col),Jcurr(1,col)));
                cMag = norm(Jcurr(:,col));
                cMagErrors(col-1) = abs(cMag-magnitudes0(col-1));
                cAngErrors(col-1) = min(abs(cAng-angles0(col-1)),abs(2*pi-cAng-angles0(col-1)));
            end
            
            angErrors(jj) = rad2deg(mean(cAngErrors));
            magErrors(jj) = mean(cMagErrors);
            
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
            
            for pp = 1:2
                subplot(1,2,pp);
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
        end
        
        pointIndex1 = find(px == 1 & py == 1);
        pointIndex2 = find(px == 1 & py == npoints);
        pointIndex3 = find(px == npoints & py == npoints);
        pointIndex4 = find(px == npoints & py == 1);
        boundaryPoints = horzcat(boundaryPoints,poss(pointIndex1,:).',poss(pointIndex2,:).',poss(pointIndex3,:).',poss(pointIndex4,:).');
        
        
        subplot(1,2,1);
        trisurf(triGrid,poss(:,1), poss(:,2), poss(:,3), magErrors, 'EdgeColor', 'none', 'FaceAlpha',faceAlphas(aa));
        colorbar;
        subplot(1,2,2);
        trisurf(triGrid,poss(:,1), poss(:,2), poss(:,3), angErrors, 'EdgeColor', 'none', 'FaceAlpha',faceAlphas(aa));
        colorbar;
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

for pp=1:2
    subplot(1,2,pp)
    xlim([mins(1) maxs(1)]);
    ylim([mins(2) maxs(2)]);
    zlim([mins(3) maxs(3)]);
    view(45,15);
    xlabel('x (cm)');
    ylabel('y (cm)');
    zlabel('z (cm)');
    box on;
end

% na = size(alphas,2);
% nactive = size(activeActuators,1);
% for i=1:4*nactive
%     stackedPoints = zeros(na,3);
%     for j=0:na-1
%         stackedPoints(j+1,:) = boundaryPoints(:,i+j*4*nactive).';
%     end
%     plot3(stackedPoints(:,1), stackedPoints(:,2), stackedPoints(:,3),'k');
% end