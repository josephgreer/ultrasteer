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
function [x, y, xs, wallIndex] = MoveRobotUntilNodeEncountered(x, y, xs, tipTangent, walls, nodes, desNode)
dl = 1e4;
node = -1;
ignoreWall = -1;
wallIndex = -1;

while(node ~= desNode)
    if(wallIndex <= 0)
        tipPoint = x(5:6)+tipTangent*dl;
        proxPoint = x(5:6);
        [wallIndex, ix, iy] = FindNextWall(walls,ignoreWall,proxPoint,tipPoint,tipTangent);
        ignoreWall = -1;
    else
        ix = x(5); iy = x(6);
    end
    
    if(wallIndex > 0)
        % if we're at a straight-wall junction -0-, reset glancing wall
        % contact
        if(abs(ix-y(1))+abs(iy-y(2)) < 1e-3 && (y(5) == 2 || y(5) == 3))
            y(5) = y(4); y(4) = y(3);
        end
        dl = dl-(norm([ix;iy]-x(3:4)) - norm(x(5:6)-x(3:4)));
        x(5:6) = [ix; iy];
        
        [x, y, xs, travel, eow] = MoveRobotForwardAlongWall(x, y, tipTangent, wallIndex, dl, walls, xs);
        
        if(eow)
            wallIndex = -1;
        end
        
        node = NodePosToIdx(x(5:6).',nodes);
        
        tipTangent = x(5:6)-x(3:4);
        tipTangent = tipTangent/norm(tipTangent);
    elseif(wallIndex <= -2)
        y(5) = 1e6;
        return;
    else
        assert(0);
    end
end

end