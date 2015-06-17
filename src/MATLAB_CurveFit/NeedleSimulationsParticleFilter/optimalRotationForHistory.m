% template = current state projected back
% tempalte{1} = current point
% template{2} = 1 timestep back
% measurements = measurement history
% measurement{1} = latest measurement
% measurement{2} = measurement 1 timestep back etc
function R = optimalRotationForHistory(template,measurements, params)
template = cell2mat(template);
template = [template.pos]';
offset = template(1,:);
template = template-repmat(offset, size(template,1), 1);
measurements = cell2mat(measurements);
measurements = [measurements.pos]';
measurements = measurements-repmat(offset, size(measurements,1), 1);


D = distanceMatrix(measurements,template);

[minD minTemplate] = min(D,[], 2);
goodDs = find(minD < params.p100.distanceThresh);
goodDs = datasample(goodDs, min(params.p100.subsetSize, length(goodDs)), 'Replace', false);
%display(length(goodDs));
minTemplate = minTemplate(goodDs);

X = template(minTemplate,:);
% X = QuatToRotationMatrix(AxisAngleToQuat(2000*rand(3,1)))*X';
% X = X';
Y = measurements(goodDs,:);
R = procrustesRotation(X,Y);


%[~, ~, R] = procrustes(Y,X,'scaling',false, 'reflection', false);
% R.c(1,:)
% R = R.T;
%R = procrustesRotation(Y,X);
%[R, ~] = icp(template', measurements');
end

function drawPointHistories(X,Y,R,offset)
figure(2);

Z = R*X';
Z = Z';
X = X+repmat(offset, size(X,1), 1);
Y = Y+repmat(offset, size(Y,1), 1);
Z = Z+repmat(offset, size(Z,1), 1);
hold on
scatter3(X(:,1), X(:,2), X(:,3), 'filled','MarkerFaceColor',[1 0 0]);
scatter3(Y(:,1), Y(:,2), Y(:,3), 'filled','MarkerFaceColor',[0 1 0]);
scatter3(Z(:,1), Z(:,2), Z(:,3), 'filled','MarkerFaceColor',[0 0 1]);


ylim([-10 150]);
zlim([-10 100]);
xlim([-50 50]);
xlabel('x');
ylabel('y');
zlabel('z');
view(90,0);
axis equal;
hold on;
grid on;

% line = fitLine(X);
% ps = projectPointsOntoLine(line, X);
% pta = line(:,2)+min(ps)*line(:,1);
% ptb = line(:,2)+max(ps)*line(:,1);
% 
% psx = projectPointsOntoLine(line, Y);
% goodPts = find(min(ps) <= psx & psx <= max(ps));

% pts = [pta'; ptb'];
% scatter3(Y(goodPts,1), Y(goodPts,2), Y(goodPts,3), 'filled','MarkerFaceColor',[1 0 0]);
% plot3(pts(:,1), pts(:,2), pts(:,3), 'k', 'LineWidth', 2);
end