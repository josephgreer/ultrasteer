function R = optimalRotationForHistory(X,Y)
X = cell2mat(X);
X = [X.pos]';
offset = X(1,:);
X = X-repmat(offset, size(X,1), 1);
Y = cell2mat(Y);
Y = [Y.pos]';
Y = Y-repmat(offset, size(Y,1), 1);
%[~, ~, R] = procrustes(Y,X,'scaling',false, 'reflection', false);
% R.c(1,:)
% R = R.T;
%R = procrustesRotation(Y,X);
[R, ~] = icp(X', Y');
end

function drawPointHistories(X,Y,R,offset)
figure(2);

Z = R*X';
Z = Z';
X = X+repmat(offset, size(X,1), 1);
Y = Y+repmat(offset, size(Y,1), 1);
Z = Z+repmat(offset, size(Z,1), 1);
scatter3(X(:,1), X(:,2), X(:,3), 'filled','MarkerFaceColor',[1 0 0]);
hold on
scatter3(Y(:,1), Y(:,2), Y(:,3), 'filled','MarkerFaceColor',[0 1 0]);
%scatter3(Z(:,1), Z(:,2), Z(:,3), 'filled','MarkerFaceColor',[0 0 1]);


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

line = fitLine(X);
ps = projectPointsOntoLine(line, X');
pta = line(:,2)+min(ps)*line(:,1);
ptb = line(:,2)+max(ps)*line(:,1);

pts = [pta'; ptb'];
plot3(pts(:,1), pts(:,2), pts(:,3), 'k', 'LineWidth', 2);
end