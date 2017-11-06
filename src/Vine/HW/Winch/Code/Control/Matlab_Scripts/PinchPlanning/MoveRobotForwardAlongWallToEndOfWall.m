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
function [x, y, xs, newState] = MoveRobotForwardAlongWallToEndOfWall(x, y, wall, point, dl, xs)
% wall tangent
wallTangent = wall(:,2)-wall(:,1); wallTangent = wallTangent/norm(wallTangent);

tipTangent = x(5:6)-x(3:4);
distalLength = norm(tipTangent);
tipTangent = tipTangent/distalLength;

proxTangent = x(3:4)-x(1:2);
proxLength = norm(proxTangent); proxTangent = proxTangent/proxLength;

if(dot(tipTangent, wallTangent) < 0) 
    wallTangent = -wallTangent;
end

dtcross = cross([tipTangent;0], [wallTangent;0]);

newState = false;

% turning away from an obstacle
if((sign(dtcross(3)) == 1 && y(5) == 3) || (sign(dtcross(3)) == -1 && y(5) == 2))
    y(5) = y(4);
    y(4) = y(3);
end

if((sign(dtcross(3)) == 1 && (y(5) == 0 || y(5) == 2)) || (sign(dtcross(3)) == -1 && (y(5) == 1 || y(5) == 3)))
    % wall is turning us in the direction of the most distal turn point
    % now we're solving a SSA triangle
    
    % we're turning in the direction of the most distal obstacle, so reset
    % the contact point
    
    % if a left turn glancing contact becomes active
    if(y(5) == 2 || y(5) == 3)
        % if most proximal point is a left turn, shift turns down
        if(y(3) == 0 || y(3) == 1)
            x(1:2) = x(3:4);
        end
        x(3:4) = y(1:2);
        if(y(5) == 2)
            y(5) = 3;
            y(4) = 0;
            y(3) = 1;
        else
            y(5) = 2
            y(4) = 1;
            y(3) = 0;
        end
        newState = true;
    end
    % move it just past the end of the wall
    x(5:6) = point;
else
    % wall is turning us in the direction opposite of the most distal pivot
    % point
    
    if(sign(dtcross(3)) == 1)
        % we're being turned left.
        
        % we're turning in the direction of the second most distal obstacle, so reset
        % the contact point
        if(y(4) == 2)
            x(1:2) = y(1:2);
            y(3) = 0;
            y(4) = 1;
            y(5) = 3;
            proxLength = norm(x(3:4)-x(1:2));
        end
        
        l1 = proxLength;
        baseDelta = point-x(1:2); l3 = norm(baseDelta);
        baseDelta = baseDelta/l3;
        
        cosb = dot(proxTangent, -tipTangent);
        sinb = sqrt(1-cosb^2);
        
        sinc = l1/l3*sinb;
        b = asin(sinb); c = asin(sinc);
        
        a = pi-b-c;
        
        baseDelta = PlaneRotation(a)*baseDelta;
        
        x(3:4) = x(1:2)+baseDelta*proxLength;
        x(5:6) = point;
    else
        
        % we're being turned left.
        
        % we're turning in the direction of the second most distal obstacle, so reset
        % the contact point
        if(y(4) == 3)
            x(1:2) = y(1:2);
            y(3) = 1;
            y(4) = 1;
            y(5) = 2;
            proxLength = norm(x(3:4)-x(1:2));
        end
        
        l1 = proxLength;
        baseDelta = point-x(1:2); l3 = norm(baseDelta);
        baseDelta = baseDelta/l3;
        
        cosb = dot(proxTangent, -tipTangent);
        sinb = sqrt(1-cosb^2);
        
        sinc = l1/l3*sinb;
        b = acos(cosb); c = asin(sinc);
        
        a = pi-b-c;
        
        baseDelta = PlaneRotation(-a)*baseDelta;
        
        x(3:4) = x(1:2)+baseDelta*proxLength;
        x(5:6) = point;
    end
    
    % use law of cosines
end

tipDelta = x(5:6)-x(3:4);
tipLen = norm(tipDelta);
dl = dl-(tipLen-distalLength);
if(dl > 0)
    x(5:6) = x(5:6)+dl*tipDelta/tipLen;
else
    assert(0);
end

if(newState)
    xs(end,:) = [y(1) y(2)];
    xs = vertcat(xs, [x(5) x(6)]);
else
    xs(end-1:end,:) = reshape(x(3:6),2,2).';
end

end