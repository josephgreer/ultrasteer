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
% happen
% otherwise the opposite
function [x, y] = MoveRobotByDlOrUntilEndOfWall(x, y, walls, ignoreWalls)
tipTangent = x(5:6)-x(3:4); tipTangent = tipTangent/norm(tipTangent);

dl = 1e4;
tipPoint = x(5:6)+tipTangent*dl;
proxPoint = x(5:6);
[wallIndex, ix, iy] = FindNextWall(walls,ignoreWalls,proxPoint,tipPoint,tipTangent);

if(wallIndex > 0)
    % if we're at a straight-wall junction -0-, reset glancing wall
    % contact
    if(abs(ix-y(1))+abs(iy-y(2)) < 1e-3 && (y(5) == 2 || y(5) == 3))
        y(5) = y(4); y(4) = y(3);
    end
    dl = dl-(norm([ix;iy]-x(3:4)) - norm(x(5:6)-x(3:4)));
    x(5:6) = [ix; iy];
    
    xs = [x(1:2).'; x(5:6).'];
    [x, y, ~, ~, eow] = MoveRobotForwardAlongWall(x, y, tipTangent, wallIndex, dl, walls, xs);
    if(~eow)
        assert(0);
    end
elseif(wallIndex < -1)
    x(5:6) = [1e10; 1e10];
    return;
else
    x(5:6) = tipPoint;
    return;
end

end