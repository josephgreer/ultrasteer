clear clc; close all;

figure(1);
basePath = 'C:/Joey/Data/ForceData/WithWrap/Orientation6/Trial';
numTrials = 1;
calibrationPath = 'C:/Joey/Data/ForceData/WithWrap/Orientation6/Calibration/calib';
% for each test within folder
for i=1:numTrials
    display(i);
    currentPath = strcat(basePath, num2str(i), '/');
    % holds 4 corners of puncture plane [c1 c2 c3 c4] \in R^{3x4}
    corners = load(strcat(currentPath, 'corners.m'), '-ascii');
    % holds needle tip position for each time step [p1^T; ...; p_m^T] \in
    % R^{mx3}
    needlePos = load(strcat(currentPath, 'needlePos.m'), '-ascii');
    % holds forces and torques for each time step [f1^T t1^T; ...; fm^T tm^T] \in
    % R^{mx6}
    forceTorques = load(strcat(currentPath, 'forceTorques.m'), '-ascii');
    % puncture plane represented by ax+by+cz+d = 0
    puncturePlane = Plane(load(strcat(currentPath, 'abcd.m'), '-ascii'));
    draw(puncturePlane, corners(:,1), [50; 50], 1);
    hold on;
    nframes = size(forceTorques,1);
    [sphereX sphereY sphereZ] = sphere(10);
    needleHandle = surf(sphereX, sphereY, sphereZ);
    hold off;
    
    data = zeros(nframes, 3);
    % for each time step...
    for i=1:nframes
        % set needle position
        set(needleHandle, 'XData', sphereX+needlePos(i,1),...
            'YData', sphereY+needlePos(i,2), 'ZData', sphereZ+needlePos(i,3));
        
        % estimated distance from needle tip to puncture plane
        d = pointDistance(puncturePlane, needlePos(i,:)');
        % force magnitude 
        forceMag = norm(forceTorques(i,1:3));
        % torque magnitude
        torqueMag = norm(forceTorques(i, 4:6));
        data(i,:) = [d forceMag torqueMag];
        % comment this to get rid of animation
        figure(1);
    end
    
    % plot all needle position so we can see trajectory
    hold on;
    scatter3(needlePos(:,1), needlePos(:,2), needlePos(:,3));
    
    figure(2);
    hold on;
    % estimated needle tip-plane distance
    plot([1:nframes], data(:,1),'r', 'LineWidth', 2);
    % force mag
    plot([1:nframes], data(:,2),'b', 'LineWidth', 2);
    % torque mag
    plot([1:nframes], data(:,3),'k', 'LineWidth', 2);
    legend('distance', 'forcemag', 'torquemag');
    pause;
    
    close all;
end