function [ data ] = checkReachable( data )
%CHECKREACHABLE Check if target points are reachable
% author:   Troy Adebar
% date:     2/27/2015
%
% -------------------------------------------------------------------------
%% Get some initial values
% Determine what iteration we are on
itr = data.itr;


%% The first iteration
if itr == 1
    
    % Loop through all possible fine voxels not labelled reachable
    for i = find( data.fineVol.V(:) == 0)'
        
        % Check the voxel for reachability
        [ reachable, node ] = ...
            isReachable(    data.entry.p, ...
                            data.fineVol.coords.XYZ(:,i), ...
                            data.entry.v, ...
                            data );
        
        % If it's reachable, save the node and matrix value
        if ( reachable )
            data.finePaths(itr).node(i) = node;
            data.fineVol.V(i) = itr;
        end
    end
    
    % Loop through all possible coarse voxels not labelled reachable
    for i = find( data.coarseVol.V(:) == 0)'
        
        % Check the voxel for reachability
        [ reachable, node ] = ...
            isReachable(    data.entry.p, ...
                            data.coarseVol.coords.XYZ(:,i), ...
                            data.entry.v, ...
                            data );
        
        % If it's reachable, save the node and matrix value
        if ( reachable )
            data.coarsePaths(itr).node(i) = node;
            data.coarseVol.V(i) = itr;
        end
    end
end

%% The second iteration
if itr == 2
    
    % Loop through all points
    for i = find( data.fineVol.V(:) == 0)'
        
        % Randomly loop thru reachable coarse nodes from previous iteration
        indCrseRch = find( data.coarseVol.V(:) > 0 )';     
        index = randperm(numel(indCrseRch));
        for j = indCrseRch(index)
            
            % Check that the current point is farther along the insertion
            dist = dot(data.entry.v, data.fineVol.coords.XYZ(:,i) - data.entry.p);
            if dist <= data.coarsePaths(itr-1).node(j).d
                continue;
            end
                        
            % Check the voxel for reachability
            [ reachable, node ] = ...
                isReachable(    data.coarsePaths(itr-1).node(j).p, ...
                                data.fineVol.coords.XYZ(:,i), ...
                                data.coarsePaths(itr-1).node(j).v, ...
                                data );
            
            % If it's reachable, save the node and matrix value...
            if ( reachable )
                data.finePaths(itr).node = node;
                data.fineVol.V(i) = itr;
                % ...and stop checking this node
                break;
            end
            
        end
    end
end

% Increase the iteration counter
data.itr = data.itr + 1;

end

function [ reachable, node ] = isReachable( a, b, v_i, data )
% Solve for the arc connecting insertion with each point
[ rho,~,pts,v_e ] = findCircle(a,b,v_i,data.Narcpts);

% Check if arc is too tight
if (   rho < data.Rmin  )
    node = [];
    reachable = false;
    return;
end

%% Check which voxels the arc is within

% Rearrange the list of arc points
arcPts = permute(pts,[1,3,2]);
arcPts = repmat(arcPts,[1,data.coarseVol.Npts,1]);

% Find the difference vectors between each arc point and the boundaries
vecs = arcPts-data.coarseVol.coords.mXYZ;

% Check the magnitude of the differences is less than voxel size
mDel = repmat(data.coarseVol.del,[1,data.coarseVol.Npts,data.Narcpts]);
coarseInd = find(   any(    all( abs(vecs)<mDel,1)     , 3 )  );

% Next check within these coarse voxels for collisons
arcPts = permute(pts,[1,3,2]);
arcPts = repmat(arcPts,[1,sum([data.boundaries.N(coarseInd)]),1]);
vecs = arcPts-[data.boundaries.coords(coarseInd).mXYZ];
dist = sum(vecs.^2,1);
if( any(dist(:) < ((min(data.fineVol.del)/2)^2)) )
    node = [];
    reachable = false;
    return;
end

% Otherwise record the path end node...
dist = dot(data.entry.v, b - data.entry.p);
node.d = dist;
node.p = b;
node.v = v_e;

% And record that the point is reachable
reachable = true;

end