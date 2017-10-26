% x(1:2) = tip point \in R^2
% x(3:4) = most distal left turn point \in R^2
% x(5:6) = most distal right turn point \in R^2
% x(7) == 0,1 0 means left turn point is more distal than right turn point,
% wall = [p1_x p2_x; p1_y p2_y] \in R^{2 x 2}
% otherwise the opposite
function x = MoveRobotForwardAlongWall(x, p0, wall, dl)
t = wall(:,2)-wall(:,1); t = t/norm(t);

distalPoints = [x(1:2).' x(3:4).'];
proxPoints = [x(3:4).' x(5:6).'];
if(x(7) == 1)
    distalPoints(:,2) = x(5:6).';
    proxPoints = [x(5:6).' x(3:4).'];
end

tipTangent = distalPoints(:,1)-distalPoints(:,2);
tipTangent = tipTangent/norm(tipTangent);

proxTangent = proxPoints(:,1)-proxPoints(:,2);
proxTangent = proxTangent/norm(proxTangent);

if(dot(tipTangent, t) < 0) 
    t = -t;
end

intersection = FindIntersectionOfTwoLines(distalPoints(:,1),distalPoints(:,2),...
    wall(:,1),wall(:,2));


dl = dl-norm(intersection-distalPoints(:,1));
if(dl < 0)
    assert(0);
end
distalPoints(:,1) = intersection;

pdcross = cross(proxTangent, tipTangent);
dtcross = cross(tipTangent, t);

if(sign(pdcross(3)) == sign(dtcross(3)))
    % wall is turning us in the direction of the most distal turn point
else
    % wall is turning us in the direction opposite of the most distal pivot
    % point
    
end

end