function [wallIndex, intersectX, intersectY] = FindNextWall(walls, proximalPoint, tipPoint, tipTangent)    

wallIndex = -1;
intersectX = -1;
intersectY = -1;

out = lineSegmentIntersect([proximalPoint(1) proximalPoint(2) tipPoint(1) tipPoint(2)],...
    walls);

numWalls = sum(out.intAdjacencyMatrix);

if(numWalls == 0)
    return;

pts = [find(out.intAdjacencyMatrix).' out.intMatrixX(out.intAdjacencyMatrix).'...
    out.intMatrixY(out.intAdjacencyMatrix).'];

if(size(pts,1) > 1)
    dists = [pts(:,2)-proximalPoint(1) pts(:,3)-proximalPoint(2)];
    dists = sum(dists.^2,2);
    
    minDist = min(dists);
    wallIndices = abs(dists-minDist) < 1e-3;
    pts = pts(wallIndices,:);
end

vectors = vertcat([pts(:,1) walls(pts(:,1),1:2)-pts(:,2:3)],...
    [pts(:,1) walls(pts(:,1),2:3)-pts(:,2:3)]);


norms = sqrt(sum(vectors(:,2:3).^2,2));

vectors(norms < 1e-3,:) = [];

% normalize
vectors(:,2:3) = vectors(:,2:3)./repmat(norms,1,2);

angleDiffs = angleDiffSigns([vectors(:,2:3) zeros(size(vectors,1),1)],...
    repmat([-tipTangent.' 0],1,size(vectors,1)));

[maxDiff, maxIdx] = max(angleDiffs);
[minDiff, minIdx] = min(angleDiffs);

if(sign(maxDiff) == sign(minDiff))
    % this case corresopnds to multiple walls meeting at node and we don't
    % we're at the end of the node
    wallIndex = -1;
    intersectX = -1;
    interesectY = -1;
    return;
else
    % check if they're the same wall
    dirA = vectors(maxIdx,2:3); dirB = vectors(minIdx,2:3);
    if(sum(abs(dirA+dirB)) > 1e-3)
        % we're trapped
        assert(0);
        return;
    else
        if(dot(tipTangent,dirA) > 0)
            wallIndex = vectors(maxIdx,1);
        else
            wallIndex = vectors(minIdx,1);
        end
        intersectX = pts(maxIdx,2);
        intersectY = pts(maxIdx,3);
        return;
    end
end

end