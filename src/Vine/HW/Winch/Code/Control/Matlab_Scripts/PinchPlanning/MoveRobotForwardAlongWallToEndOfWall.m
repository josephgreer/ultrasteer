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
% wall = [p1_x p2_x; p1_y p2_y] \in R^{2 x 2}
% otherwise the opposite
function [x, y, xs, travel, eow] = MoveRobotForwardAlongWallToEndOfWall(x, y, tipTangent, wallIndex, point, dl, walls, xs)
eow = true;
% wall tangent
wall = [walls(wallIndex,1) walls(wallIndex,3); walls(wallIndex,2) walls(wallIndex,4)];
wallTangent = wall(:,2)-wall(:,1); wallTangent = wallTangent/norm(wallTangent);

oldx = x;
oldy = y;

oldTipTangent = tipTangent;
distalLength = norm(x(5:6)-x(3:4));

proxTangent = x(3:4)-x(1:2);
proxLength = norm(proxTangent); proxTangent = proxTangent/proxLength;

if(dot(tipTangent, wallTangent) < 0) 
    wallTangent = -wallTangent;
end

dtcross = cross([tipTangent;0], [wallTangent;0]);
if(norm(dtcross) < 0.0175) 
    tempTangent = x(5:6)-x(1:2);
    tempTangent = tempTangent/norm(tipTangent);
    dtcross = cross([tempTangent; 0], [wallTangent; 0]);
end

newState = false;
opposite = false;

% turning away from an obstacle
if((sign(dtcross(3)) == 1 && y(5) == 3) || (sign(dtcross(3)) == -1 && y(5) == 2))
    y(5) = y(4);
    y(4) = y(3);
end

xs(end,:) = [x(5) x(6)];

if((sign(dtcross(3)) == 1 && (y(5) == 0 || y(5) == 2)) || (sign(dtcross(3)) == -1 && (y(5) == 1 || y(5) == 3)) || proxLength < 1e-3)
    % wall is turning us in the direction of the most distal turn point
    % now we're solving a SSA triangle
    
    % we're turning in the direction of the most distal obstacle, so reset
    % the contact point
    if(proxLength < 1e-3 && ~((y(5) == 2 && sign(dtcross(3)) == 1)...
            || (y(5) == 3 && sign(dtcross(3)) == -1)))
        if(sign(dtcross(3)) == 1)
            y(5) = 0;
            y(4) = 1;
        else
            y(5) = 1;
            y(4) = 0;
        end
    end
    
    % if a left turn glancing contact becomes active
    if(y(5) == 2 || y(5) == 3)
        % shift turns down if need be
        if((y(3) == 0 && y(5) == 2) || (y(3) == 1 && y(5) == 3))
            x(1:2) = x(3:4);
        end
        x(3:4) = y(1:2);
        if(y(5) == 2)
            y(5) = 0;
            y(4) = 1;
        else
            y(5) = 1;
            y(4) = 0;
        end
        newState = true;
    end
    % move it just past the end of the wall
    x(5:6) = point;
else
    opposite = true;
    % wall is turning us in the direction opposite of the most distal pivot
    % point
        % we're being turned left.
        
        % we're turning in the direction of the second most distal obstacle, so reset
        % the contact point
        if(y(4) == 2)
            x(1:2) = y(1:2);
            y(3) = 0;
            y(4) = 1;
            y(5) = 3;
            proxLength = norm(x(3:4)-x(1:2));
        elseif(y(4) == 3)
            x(1:2) = y(1:2);
            y(3) = 1;
            y(4) = 1;
            y(5) = 2;
            proxLength = norm(x(3:4)-x(1:2));
        end
        
        l1 = proxLength;
        baseDelta = point-x(1:2); l3 = norm(baseDelta);
        baseDelta = baseDelta/l3;
        
        cosb = dot(proxTangent, -tipTangent);  cosb = sign(cosb)*min(max(abs(cosb),0),1);
        sinb = sqrt(1-cosb^2);
        
        sinc = l1/l3*sinb;
        sinc = sign(sinc)*min(max(abs(sinc),0),1);
        
        b = acos(cosb); c = asin(sinc);
        
        a = pi-b-c;
        
        if(sign(dtcross(3)) == 1)
            baseDelta = PlaneRotation(a)*baseDelta;
        else
            baseDelta = PlaneRotation(-a)*baseDelta;
        end
        
        x(3:4) = x(1:2)+baseDelta*proxLength;
        x(5:6) = point;
    
    % use law of cosines
end


oldTip = x(5:6);
oldDl = dl;

if(opposite)
    [intersects, intersectX, intersectY,obstacleDl,intersectXs] = CheckForCollisionSlidingAlongWallWithKink(oldx,x,oldy,y,sign(dtcross(3)),asin(sinb),wallIndex,walls,xs);
else
    [intersects, intersectX, intersectY,obstacleDl,intersectXs] = CheckForCollisionsSlidingAlongTheWall(oldx,x,oldy,y,wallIndex,walls,xs);
end

if(intersects)
    dl = dl-obstacleDl;
    xs = intersectXs;
    intersectTipTangent = intersectX(5:6)-intersectX(3:4);
    intersectTipTangent = intersectTipTangent/norm(intersectTipTangent);
    
    [x,y,xs,travel,eow] = MoveRobotForwardAlongWall(intersectX,intersectY,intersectTipTangent,wallIndex,dl,walls,xs);
    travel = travel+obstacleDl;
    return;
end

y(1:2) = point;
y(3) = y(4);
y(4) = y(5);
if(sign(dtcross(3)) == 1)
    y(5) = 3;
else
    y(5) = 2;
end

if(newState)
    tipLen = norm(x(5:6)-x(3:4))+norm(x(3:4)-x(1:2));
else
    tipDelta = x(5:6)-x(3:4);
    tipLen = norm(tipDelta);
end
travel = (tipLen-distalLength);

if(newState)
    xs(end,:) = [x(3) x(4)];
    xs = vertcat(xs, [x(5) x(6)]);
else
    xs(end-1:end,:) = reshape(x(3:6),2,2).';
end

end