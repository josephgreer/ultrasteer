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
function [x, y, xs, wallIndex] = MoveRobotUntilNodeEncountered(x, y, xs, wallIndex, walls, theta, nodes, desNode)
dl = 5e2;
node = -1;

tipTangent = x(5:6)-x(3:4);
if(norm(tipTangent) > 0)
    tipTangent = tipTangent/norm(tipTangent);
else
    tipTangent = [1;0];
end

oldTipTangent = tipTangent;
ignoreWall = -1;
if(abs(theta) > 0)
    tipTangent = PlaneRotation(theta)*tipTangent;
    
    proxTangent = x(3:4)-x(1:2);
    proxTangent = proxTangent/norm(proxTangent);
    
    [x,y,xs] = AdjustStateMetdataForTurn(theta,x,y,xs);
    
    if(wallIndex > 0)
        wallTangent = walls(wallIndex,3:4)-walls(wallIndex,1:2);
        wallTangent = wallTangent/norm(wallTangent);
        
        angleDiffs = angleDiffSigns([repmat(wallTangent,2,1) zeros(2,1)],...
            [oldTipTangent' 0; tipTangent.' 0]);
        
        if(abs(angleDiffs(1)) < 1e-3)
            angleDiffs = angleDiffSigns([repmat(wallTangent,2,1) zeros(2,1)],...
                [proxTangent' 0; tipTangent.' 0]);
        end
        
        if(sign(angleDiffs(1)) ~= sign(angleDiffs(2)) && min(abs(angleDiffs)) > 1e-3)
            x(1:2) = x(3:4);
            ignoreWall = wallIndex;
            wallIndex = -1;
        end
    end
    
    xs = vertcat(xs,xs(end,:));
end

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
        x(5:6) = tipPoint;
        xs(end,:) = tipPoint.';
    end
end

end