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
% wallIndex is the last wall index the robot tip was in contact with
% xs reperesent the segments of the robot
% otherwise the opposite
function [x, y, xs,wallIndex] = MoveRobotByDl(x, y, dl, walls, thetas, len, wallIndex, xs)
tipTangent = x(5:6)-x(3:4); tipTangent = tipTangent/norm(tipTangent);

thetaIdx = find(abs(thetas(:,1)-len) < 1e-3);
oldTipTangent = tipTangent;
ignoreWall = -1;
if(~isempty(thetaIdx))
    theta = thetas(thetaIdx,2);
    tipTangent = PlaneRotation(theta)*tipTangent;
    
    % patch up the metadata and state
    if(theta > 0)
        % left turn
        
        if(y(5) == 0 || y(5) == 2)
            % if most distal turn is a left turn, repalce it
            x(3:4) = x(5:6);
        elseif(y(5) == 3)
            % if most distal "turn" is a right contact, drop it down
            x(1:2) = y(1:2);
            x(3:4) = x(5:6);
        else
            % otherwise most distal turn is a right turn so slide down
            x(1:2) = x(3:4);
            x(3:4) = x(5:6);
        end
        y(4) = 1;
        y(5) = 0;
    else
        % right turn
        if(y(5) == 1 || y(5) == 3)
            % if most distal turn is a right turn, replace it
            x(3:4) = x(5:6);
        elseif(y(5) == 2)
            % if most distal "turn" is a left contact, drop it down
            x(1:2) = y(1:2);
            x(3:4) = x(5:6);
        else
            % otherwise most distal turn is a left turn so slide down
            x(1:2) = x(3:4);
            x(3:4) = x(5:6);
        end
        y(4) = 0;
        y(5) = 1;
    end
    
    if(wallIndex > 0)
        wallTangent = walls(wallIndex,3:4)-walls(wallIndex,1:2);
        wallTangent = wallTangent/norm(wallTangent);
        
        angleDiffs = angleDiffSigns([repmat(wallTangent,2,1) zeros(2,1)],...
            [oldTipTangent' 0; tipTangent.' 0]);
        
        if(sign(angleDiffs(1)) ~= sign(angleDiffs(2)))
            x(1:2) = x(3:4);
            ignoreWall = wallIndex;
            wallIndex = -1;
        end
    end
    
    xs = vertcat(xs,xs(end,:));
end

while(dl > 0)
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
        
        dl = dl-travel;
        if(dl > 0)
            tipTangent = x(5:6)-x(3:4);
            tipTangent = tipTangent/norm(tipTangent);
        end
    elseif(wallIndex <= -2)
        display('Youre done, son');
        return;
    else
        x(5:6) = tipPoint;
        xs(end,:) = tipPoint.';
        return;
    end
end

end