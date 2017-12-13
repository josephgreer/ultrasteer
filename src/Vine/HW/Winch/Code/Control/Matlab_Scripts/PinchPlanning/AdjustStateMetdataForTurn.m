function [x,y,xs] = AdjustStateMetdataForTurn(theta,x,y,xs)
% patch up the metadata and state
if(theta > 0)
    % left turn
    
    if(y(5) == 0 || (y(5) == 2 && y(4) == 0))
        % if most distal turn is a left turn, repalce it
        x(3:4) = x(5:6);
    elseif(y(5) == 3)
        % if most distal "turn" is a right contact, drop it down
        x(1:2) = y(1:2);
        x(3:4) = x(5:6);
        
        xs(end,:) = [x(1) x(2)];
        xs = vertcat(xs,[x(5) x(6)]);
    else
        % otherwise most distal turn is a right turn so slide down
        x(1:2) = x(3:4);
        x(3:4) = x(5:6);
    end
    y(4) = 1;
    y(5) = 0;
else
    % right turn
    if(y(5) == 1 || (y(5) == 3 && y(4) == 1))
        % if most distal turn is a right turn, replace it
        x(3:4) = x(5:6);
    elseif(y(5) == 2)
        % if most distal "turn" is a left contact, drop it down
        x(1:2) = y(1:2);
        x(3:4) = x(5:6);
        
        xs(end,:) = [x(1) x(2)];
        xs = vertcat(xs,[x(5) x(6)]);
    else
        % otherwise most distal turn is a left turn so slide down
        x(1:2) = x(3:4);
        x(3:4) = x(5:6);
    end
    y(4) = 0;
    y(5) = 1;
end
end