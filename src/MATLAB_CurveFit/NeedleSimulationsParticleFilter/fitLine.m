% x = [x1; x2; x3; x4; x5 ...] x_i \in R^3
% line described as line(:,2)+line(:,1)*t \in R^3
function line = fitLine(X)
py = polyfit(X(:,1),X(:,2),1);
pz = polyfit(X(:,1),X(:,3),1);

line = [[1 0]; py; pz];
end