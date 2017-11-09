% x = state \in R^6
% x(1:2) = Second Most Distal Turn Point (left or right) 
% x(3:4) = Most Distal Turn Point (opposite direction of x(1:2))
% x(5:6) = Tip Point
% y = auxillary information
% y(1:2) = obstacle contact point
% y(3:5) = Contact point descriptions
% ----0----0----0-----Tip Point
%     A    B    C
% y(3) corresponds to A \in {0,1,2, 3} 0 = left turn, 1 = right turn, 2 = wall contact point left, 3 = wall contact point right
% y(4) corresopnds to B
% y(5) corresponds to C
% walls = [w11_x w11_y w12_x w12_y;
%           ...
%          wn1_x wn1_y wn2_x wn2_y]
% walls \in R^{nx4}
% thetas = [l_1 theta_1
%           ...
%           l_m theta_m]
% thetas \in R^{mx2} (first column represents lengths at which the thetas
% happen
% len is the length of the robot
% lastWallIndex is the last wall index the robot tip was in contact with
% xs reperesent the segments of the robot
% otherwise the opposite
function [x, y, xs, newState] = MoveRobotByDl(x, y, dl, walls, thetas, len, lastWallIndex, xs)
tipTangent = x(5:6)-x(3:4); distalLength = norm(tipTangent);
tipTangent = tipTangent/distalLength;


thetaIdx = find(thetas(:,1) == len);
if(~isempty(thetaIdx))
    theta = thetas(thetaIdx,2);
    tipTangent = PlaneRotation(theta)*tipTangent;
end

oldX = x;
oldY = y;

% check if extending intersects wall
proximalPoint = x(3:4);
if(y(5) == 2 || y(5) == 3)
    proximalPoint = y(1:2);
end

tipPoint = proximalPoint+tipTangent*dl;

out = lineSegmentIntersect([proximalPoint(1) proximalPoint(2) tipPoint(1) tipPoint(2)],...
    walls);

if(lastWallIndex > 0)
    out.intAdjacencyMatrix(lastWallIndex) = 0;
end

intersectingWalls = find(out.intAdjacencyMatrix > 0);

intersectionPoints = [out.intPointsX(intersectingWalls) out.intPointsY(intersectingWalls)];



end