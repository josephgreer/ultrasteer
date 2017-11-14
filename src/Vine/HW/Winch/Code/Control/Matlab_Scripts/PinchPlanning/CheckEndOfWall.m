function [eow,point] = CheckEndOfWall(wall,x)
eow = false;
point = [0;0];
% check if we're past the end of the wall
mn = min(wall,[],2); mx = max(wall,[],2);
if(~(sum(mn-1e-3 <= x(5:6)) == 2 && sum(x(5:6) <= mx+1e-3) == 2))
    eow = true;
    delta1 = x(5:6)-wall(:,1); delta2 = x(5:6)-wall(:,2);
    delta1 = dot(delta1,delta1); delta2 = dot(delta2,delta2);
    if(delta1 < delta2)
        point = wall(:,1);
    else
        point = wall(:,2);
    end
    return;
end
end