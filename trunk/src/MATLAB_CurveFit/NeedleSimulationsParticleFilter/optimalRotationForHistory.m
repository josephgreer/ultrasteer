function R = optimalRotationForHistory(X,Y)
X = cell2mat(X);
X = [X.pos]';
X = X-repmat(X(1,:), size(X,1), 1);
Y = cell2mat(Y);
Y = [Y.pos]';
Y = Y-repmat(X(1,:), size(Y,1), 1);
%[~, ~, R] = procrustes(Y,X,'scaling',false, 'reflection', false);
% R.c(1,:)
% R = R.T;
%R = procrustesRotation(Y,X);
[R, ~] = icp(X', Y');
end

function drawPointHistories(X,Y,R)
figure(2);
scatter3(X(:,1), X(:,2), X(:,3), 'filled','MarkerFaceColor',[1 0 0]);
hold on
scatter3(Y(:,1), Y(:,2), Y(:,3), 'filled','MarkerFaceColor',[0 1 0]);

Z = R*X';
Z = Z';
scatter3(Z(:,1), Z(:,2), Z(:,3), 'filled','MarkerFaceColor',[0 0 1]);
end