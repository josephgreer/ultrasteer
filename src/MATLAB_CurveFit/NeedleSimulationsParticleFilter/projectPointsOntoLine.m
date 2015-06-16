% line = line(:,2)+line(:,1)*t \in R^3
% points = [x1 x2 x3 ... xn] x_i \in R^3
function ps = projectPointsOntoLine(line, points)
offset = line(:,2);
pointsShifted =  points-repmat(offset,1,size(points,2));
ps = (line(:,1)'*pointsShifted)/(line(:,1)'*line(:,1));
end