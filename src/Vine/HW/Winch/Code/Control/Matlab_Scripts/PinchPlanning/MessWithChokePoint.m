function cmap = MessWithChokePoint(map,fraction)
cmap = map;

fraction = 1-fraction;
room = 1-fraction;

pos = unifrnd(0,1);


topPoint = map(26,2);
bottomPoint = map(14,2);

distance = topPoint-bottomPoint;

spareRoom = room*distance;
height = fraction*distance;

topPoint = topPoint-spareRoom*pos;
bottomPoint = topPoint-height;

cmap(26,[2,4]) = topPoint;
cmap(27,2) = topPoint;
cmap(25,4) = topPoint;
cmap(14,[2,4]) = bottomPoint;
cmap(13,4) = bottomPoint;
cmap(15,2) = bottomPoint;

% topPoint = map(25,2);
% bottomPoint = map(13,2);
% 
% distance = topPoint-bottomPoint;
% 
% spareRoom = room*distance;
% height = fraction*distance;
% 
% topPoint = topPoint-spareRoom*pos;
% bottomPoint = topPoint-height;
% 
% cmap(25,[2,4]) = topPoint;
% cmap(26,2) = topPoint;
% cmap(24,4) = topPoint;
% cmap(13,[2,4]) = bottomPoint;
% cmap(12,4) = bottomPoint;
% cmap(14,2) = bottomPoint;
end