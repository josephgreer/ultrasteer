% x = state \in R^6
% x(1:2) = Second Most Distal Turn Point (left or right) 
% x(3:4) = Most Distal Turn Point (opposite direction of x(1:2))
% x(5:6) = Tip Point
% y = auxillary information
% y(1:2) = wall contact point
% y(3:5) = Contact point descriptions
% ----0----0----0-----Tip Point
%     A    B    C
% y(3) corresponds to A \in {0,1,2, 3} 0 = left turn, 1 = right turn, 2 = wall contact point left, 3 = wall contact point right
% y(4) corresopnds to B
% y(5) corresponds to C
% wall = [p1_x p2_x; p1_y p2_y] \in R^{2 x 2}
% otherwise the opposite
function x = MoveRobotForwardAlongWall(x, y, wall, dl)
% wall tangent
wallTangent = wall(:,2)-wall(:,1); wallTangent = wallTangent/norm(wallTangent);

tipTangent = x(5:6)-x(3:4);
distalLength = norm(tipTangent);
tipTangent = tipTangent/distalLength;


if(dot(tipTangent, wallTangent) < 0) 
    wallTangent = -wallTangent;
end

intersection = FindIntersectionOfTwoLines(x(5:6),x(3:4),...
    wall(:,1),wall(:,2));


dl = dl-norm(intersection-distalPoints(:,1));
if(dl < 0)
    assert(0);
end
x(5:6) = intersection;

dtcross = cross([tipTangent;0], [wallTangent;0]);

if((sign(dtcross(3)) == 1 && (y(5) == 0 || y(5) == 2)) || (sign(dtcross(3)) == -1 && (y(5) == 1 || y(5) == 3)))
    % wall is turning us in the direction of the most distal turn point
    % now we're solving a SSA triangle
    
    % we're turning in the direction of the most distal obstacle, so reset
    % the contact point
    
    % if a left turn glancing contact becomes active
    if(y(5) == 2)
        % if most proximal point is a left turn, shift turns down
        if(y(4) == 1)
            x(1:2) = x(3:4);
        end
        x(3:4) = y(1:2);
        y(5) = 0;
        y(4) = 1;
    elseif(y(5) == 3) % right turn glancing contact becomes active
        % if most proximal point is a right turn, shift turns down
        if(y(4) == 0)
            x(1:2) = x(3:4);
        end
        x(3:4) = y(1:2);
        y(5) = 1;
        y(4) = 0;
    end
    distalLength = norm(x(5:6)-x(3:4));
    
    cospia = dot(tipTangent,wallTangent);
    sina = sqrt(1-cospia^2);
    sinb = (distalLength+dl)*sina/distalLength;
    c = pi-asin(sina)-asin(sinb);
    
    if(sign(dtcross(3)) == -1)
        c = -c;
    end
    
    cosc = cos(c);
    sinc = sin(c);
    
    A = [cosc -sinc; sinc cosc];
    tipTangent = A*tipTangent;
    x(5:6) = x(3:4)+(distalLength+dl)*tipTangent;
    
elseif(sign(dtcross(3)) == 1 && (y(4) == 0 || y(4) == 2))
    % wall is turning us in the direction opposite of the most distal pivot
    % point (left)
    
    % we're turning in the direction of the second most distal obstacle, so reset
    % the contact point
    if(y(4) == 2)
        x(1:2) = y(1:2);
    end
    
    % use law of cosines
    
elseif(sign(dtcross(3)) == -1 && (y(4) == 1 || y(4) == 3))
    % wall is turning us in the direction opposite of the most distal pivot
    % point (left)
    
    % we're turning in the direction of the second most distal obstacle, so reset
    % the contact point
    if(y(4) == 3)
        x(1:2) = y(1:2);
    end
    
    % use law of cosines
end

end