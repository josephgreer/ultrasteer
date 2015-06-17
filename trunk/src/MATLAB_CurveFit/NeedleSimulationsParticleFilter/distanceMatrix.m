% X = [x1; x2; x3; ...] x_i \in R^3
% Y = [y1; y2; y3; ...] y_i \in R^3

% D = D_ij = |(x_i-y_j)|^2
function D = distanceMatrix(X,Y)
D = zeros(size(X,1), size(Y,1));

dp = zeros(1,3);
for i=1:size(D,1)
    for j=1:size(D,2)
        dp = X(i,:)-Y(j,:);
        D(i,j) = dp*dp';
    end
end
end