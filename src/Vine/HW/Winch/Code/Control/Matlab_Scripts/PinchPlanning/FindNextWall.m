function [wallIndex, intersectX, intersectY] = FindNextWall(walls, ignoreWall, proximalPoint, tipPoint, tipTangent)    

wallIndex = -1;
intersectX = -1;
intersectY = -1;

out = lineSegmentIntersect([proximalPoint(1) proximalPoint(2) tipPoint(1) tipPoint(2)],...
    walls);

if(ignoreWall > 0)
    out.intAdjacencyMatrix(ignoreWall) = 0;
end

numWalls = sum(out.intAdjacencyMatrix);

while(numWalls > 0)
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
        [pts(:,1) walls(pts(:,1),3:4)-pts(:,2:3)]);
    
    
    norms = sqrt(sum(vectors(:,2:3).^2,2));
    
    vectors(norms < 1e-3,:) = [];
    norms(norms < 1e-3)  = [];
    
    % normalize
    vectors(:,2:3) = vectors(:,2:3)./repmat(norms,1,2);
    
    angleDiffs = angleDiffSigns([vectors(:,2:3) zeros(size(vectors,1),1)],...
        repmat([-tipTangent.' 0],size(vectors,1),1));
    
    [maxDiff, maxIdx] = max(angleDiffs);
    [minDiff, minIdx] = min(angleDiffs);
    
    if(sign(maxDiff) == sign(minDiff))
        % this case corresopnds to multiple walls meeting at node and we don't
        % we're at the end of the node
        wallIndex = -1;
        intersectX = -1;
        intersectY = -1;
        
        out.intAdjacencyMatrix(vectors(:,1)) = 0;
    else
        minAngle = max(angleDiffs(angleDiffs < 0));
        maxAngle = min(angleDiffs(angleDiffs > 0));
        
        [~, minIdx] = min(abs(angleDiffs-minAngle));
        [~, maxIdx] = min(abs(angleDiffs-maxAngle));
        
        % check if they're the same wall
        dirA = vectors(maxIdx,2:3); dirB = vectors(minIdx,2:3);
        if(sum(abs(dirA+dirB)) > 1e-3)
            % we're trapped
            wallIndex = -100;
            return;
        else
            if(dot(tipTangent,dirA) > 0)
                wallIndex = vectors(maxIdx,1);
            else
                wallIndex = vectors(minIdx,1);
            end
            intersectX = out.intMatrixX(vectors(maxIdx,1));
            intersectY = out.intMatrixY(vectors(maxIdx,1));
            return;
        end
    end
    numWalls = sum(out.intAdjacencyMatrix);
end

end