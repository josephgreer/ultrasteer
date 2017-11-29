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
function [x, y, xs, travel,eow] = MoveRobotForwardAlongWall(x, y, tipTangent, wallIndex, dl, walls, xs)
oldx = x;
oldy = y;

% wall tangent
wall = [walls(wallIndex,1) walls(wallIndex,3); walls(wallIndex,2) walls(wallIndex,4)];
wallTangent = wall(:,2)-wall(:,1); wallTangent = wallTangent/norm(wallTangent);

oldTipTangent = tipTangent;

proxTangent = x(3:4)-x(1:2);
proxLength = norm(proxTangent); proxTangent = proxTangent/proxLength;


if(dot(tipTangent, wallTangent) < 0) 
    wallTangent = -wallTangent;
end

newState = false; 

dtcross = cross([tipTangent;0], [wallTangent;0]);

if((sign(dtcross(3)) == 1 && y(5) == 3) || (sign(dtcross(3)) == -1 && y(5) == 2))
    % moving away from a proximal intersection so forget about it
    y(5) = y(4);
    y(4) = y(3);
end

opposite = false;

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
            y(5) =  1;
            y(4) = 0;
        end
        newState = true;
    end
    distalLength = norm(x(5:6)-x(3:4));
    
    cospia = dot(tipTangent,wallTangent);
    sina = sqrt(1-cospia^2);
    sinb = (distalLength)*sina/(dl+distalLength);
    c = pi-(pi-asin(sina))-asin(sinb);
   
    if(sign(dtcross(3)) == -1)
        c = -c;
    end
    
    tipTangent = PlaneRotation(c)*tipTangent;
    x(5:6) = x(3:4)+(distalLength+dl)*tipTangent;
    
else
    opposite = true;
    % wall is turning us in the direction opposite of the most distal pivot
    % point
    
    % we're turning in the direction of the second most distal obstacle, so reset
    % the contact point
    if(y(4) == 2)
        x(1:2) = y(1:2);
        y(4) = 0;
        proxLength = norm(x(3:4)-x(1:2));
    elseif(y(4) == 3)
        x(1:2) = y(1:2);
        y(4) = 1;
        proxLength = norm(x(3:4)-x(1:2));
    end
    
    l1 = proxLength;
    l2 = norm(x(5:6)-x(3:4))+dl;
    cosb = dot(proxTangent, -tipTangent);
    sinb = sqrt(1-cosb^2);
    
    l3 = sqrt(l1^2+l2^2-2*l1*l2*cosb);
    sina = l2/l3*sinb;
    a = asin(sina);
    
    l5dir = x(5:6)-x(1:2); l5 = norm(l5dir); l5dir = l5dir/l5;
    
    cospif = dot(l5dir,wallTangent); sinf = sqrt(1-cospif^2);
    sine = l5/l3*sinf;
    
    f = pi-acos(cospif); e = asin(sine);
    g = pi-f-e;
    
    if(sign(dtcross(3)) == 1)
        proxTangent = PlaneRotation(g+a)*l5dir;
        tipTangent = PlaneRotation(-asin(sinb))*proxTangent;
    else
        proxTangent = PlaneRotation(-(g+a))*l5dir;
        tipTangent = PlaneRotation(asin(sinb))*proxTangent;
    end
    x(3:4) = x(1:2)+l1*proxTangent;
    
    x(5:6) = x(3:4)+l2*tipTangent;
    
    % use law of cosines
end

[eow, point] = CheckEndOfWall(wall,x);
if(eow)
    [x, y, xs,travel,eow] = MoveRobotForwardAlongWallToEndOfWall(oldx,oldy,oldTipTangent,wallIndex,point,dl,walls,xs);
    return;
end

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

travel = dl;

if(newState)
    xs(end,:) = [x(3) x(4)];
    xs = vertcat(xs, [x(5) x(6)]);
else
    xs(end-1:end,:) = reshape(x(3:6),2,2).';
end

end