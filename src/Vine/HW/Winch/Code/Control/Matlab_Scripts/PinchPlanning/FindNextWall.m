function [wallIndex, intersectX, intersectY] = FindNextWall(walls, ignoreWalls, proximalPoint, tipPoint, tipTangent)    

wallIndex = -1;
intersectX = -1;
intersectY = -1;

forest = true;

out = lineSegmentIntersect([proximalPoint(1) proximalPoint(2) tipPoint(1) tipPoint(2)],...
    walls);

out.intAdjacencyMatrix = out.intAdjacencyMatrix | out.coincAdjacencyMatrix;
cIndices = find(out.coincAdjacencyMatrix);

if(ignoreWalls(1) > 0)
    out.intAdjacencyMatrix(ignoreWalls) = 0;
    cIndices(ismember(cIndices,ignoreWalls)) = [];
end

numWalls = sum(out.intAdjacencyMatrix);

% If we're only coincident to a wall, check if tip is in-between wall end points
if(numWalls > 0 && sum(ismember(find(out.intAdjacencyMatrix),cIndices)) == numWalls)
    values = CheckPointsBetweenLineSegments(repmat(tipPoint.', length(cIndices), 1), walls(cIndices,:));
    if(sum(values) > 0)
        wallIndex = min(find(values));
        intersectX = x(5);
        intersectY = x(6);
    end
    return;
end

while(numWalls > 0)
    pts = [find(out.intAdjacencyMatrix).' out.intMatrixX(out.intAdjacencyMatrix).'...
        out.intMatrixY(out.intAdjacencyMatrix).'];
    
    if(size(pts,1) > 1)
        dists = [pts(:,2)-proximalPoint(1) pts(:,3)-proximalPoint(2)];
        dists = sqrt(sum(dists.^2,2));
        
        [minDist,minIdx] = min(dists);
        
        if(~isempty(cIndices))
            coincidentIdx = ismember(pts(:,1),cIndices);
            dists(coincidentIdx) = minDist;
            pts(coincidentIdx,2:3) = repmat(pts(minIdx,2:3),length(cIndices),1);
        end
        
        wallIndices = abs(dists-minDist) < 1e-6;
        
        pts = pts(wallIndices,:);
    end
    
    vectors = vertcat([pts(:,1) walls(pts(:,1),1:2)-pts(:,2:3)],...
        [pts(:,1) walls(pts(:,1),3:4)-pts(:,2:3)]);
    
    
    norms = sqrt(sum(vectors(:,2:3).^2,2));
    
    badVecs = vectors(norms < 1e-6,:);
    out.intAdjacencyMatrix(badVecs(:,1)) = 0;
    cIndices(ismember(cIndices,badVecs(:,1))) = [];
    vectors(norms < 1e-6,:) = [];
    norms(norms < 1e-6)  = [];
    
    if(size(vectors,1) > 0)
        
        if(size(vectors,1) > 2)
            assert(false);
        end
        
        % normalize
        vectors(:,2:3) = vectors(:,2:3)./repmat(norms,1,2);
        
        if(~forest)
            angleDiffs = angleDiffSigns([vectors(:,2:3) zeros(size(vectors,1),1)],...
                repmat([-tipTangent.' 0],size(vectors,1),1));
            
            [maxDiff, maxIdx] = max(angleDiffs);
            [minDiff, minIdx] = min(angleDiffs);
            
            if(sign(maxDiff) == sign(minDiff) || sign(maxDiff) == 0 || sign(minDiff) == 0)
                % this case corresopnds to multiple walls meeting at node and we don't
                % we're at the end of the node
                wallIndex = -1;
                intersectX = -1;
                intersectY = -1;
                
                out.intAdjacencyMatrix(vectors(:,1)) = 0;
                cIndices(ismember(cIndices,vectors(:,1))) = [];
            else
                minAngle = max(angleDiffs(angleDiffs < 0));
                maxAngle = min(angleDiffs(angleDiffs > 0));
                if(isempty(maxAngle))
                    maxAngle = 0;
                elseif(isempty(minAngle))
                    minAngle = 0;
                end
                
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
        else % forest fix
            uniqueIdxs = unique(vectors(:,1));
            if(length(uniqueIdxs) <= 1)
                wallIndex = uniqueIdxs(1);
                intersectX = out.intMatrixX(wallIndex);
                intersectY = out.intMatrixY(wallIndex);
                return;
            end
            angles = acos(dot([vectors(:,2:3);vectors(1,2:3)],...
                [repmat(-tipTangent.',2,1);vectors(2,2:3)],2));
            
            if(angles(1)+angles(2) <= angles(3))
                % trapped
                wallIndex = -100;
                return;
            elseif(abs(sum(angles)-2*pi) < 1e-4)
                % we're moving onto the next wall
                dirA = vectors(1,2:3); dirB = vectors(2,2:3);
                if(dot(dirA,tipTangent) > 0)
                    wallIndex = vectors(1,1);
                else
                    wallIndex = vectors(2,1);
                end
                
                intersectX = out.intMatrixX(wallIndex);
                intersectY = out.intMatrixY(wallIndex);
            else
                % we're just moving past the obstacle
                wallIndex = -1;
                intersectX = -1;
                intersectY = -1;
                
                out.intAdjacencyMatrix(vectors(:,1)) = 0;
                cIndices(ismember(cIndices,vectors(:,1))) = [];
            end
            
        end
    end
    numWalls = sum(out.intAdjacencyMatrix);
end

end