function [ data ] = checkReachable( data )
%CHECKREACHABLE Check if target points are reachable
% author:   Troy Adebar
% date:     2/27/2015
%
% -------------------------------------------------------------------------
%% Get some initial values
% Determine what iteration we are on
itr = data.itr;
% Determine how many updates per process we will print
n = 20;

%% The first iteration
if itr == 1
    
    % Loop through all possible fine voxels not labelled reachable
    fprintf( '    fine paths...\n');
    fineUnkInd = find( data.fineVol.V(:) == 0)';
    NfineUnkInd = length(fineUnkInd);
    for ii = 1:NfineUnkInd;
        i = fineUnkInd(ii);
        
        % Display an update every 5 percent of the way
        if ismember(ii,round(linspace(1,NfineUnkInd,n)))
            fprintf( '    %.2f%% done at %.2f seconds...\n',...
                ii/NfineUnkInd*100,toc);
        end    
        
        % Check the voxel for reachability
        [ reachable, node, rho ] = ...
            isReachable(    data.entry.p, ...
                            data.fineVol.coords.XYZ(:,i), ...
                            data.entry.v, ...
                            data );
        
        % If it's reachable, save the node and matrix value
        if ( reachable )
            data.finePaths(itr).node(i) = node;
            data.fineVol.V(i) = itr;
            data.fineVol.R(i) = rho;
        end
    end
    
  
    % Loop through all possible coarse voxels not labelled reachable
    fprintf( '    coarse paths...\n');
    crseUnkInd = find( data.coarseVol.V(:) == 0)';
    NcoarseUnkInd = length(crseUnkInd);
    
    for ii = 1:NcoarseUnkInd;
        i = crseUnkInd(ii);
        
        % Display an update every 5 percent of the way
        if ismember(ii,round(linspace(1,NcoarseUnkInd,n)))
            fprintf( '    %.2f%% done at %.2f seconds...\n',...
                ii/NcoarseUnkInd*100,toc);
        end
                
        % Check the voxel for reachability
        [ reachable, node, ~ ] = ...
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
    % Loop through all possible fine voxels not labelled reachable
    fprintf( '    fine paths...\n');
    fineUnkInd = find( data.fineVol.V(:) == 0)';
    NfineUnkInd = length(fineUnkInd);
    for ii = 1:length(fineUnkInd);
        i = fineUnkInd(ii);
        
        % Display an update every 5 percent of the way
        if ismember(ii,round(linspace(1,NfineUnkInd,n)))
            fprintf( '    %.2f%% done at %.2f seconds...\n',...
                ii/NfineUnkInd*100,toc);
        end   
        
        % Randomly loop thru reachable coarse nodes from previous iteration
        indCrseRch = find( data.coarseVol.V(:) > 0 )';     
        index = randperm(numel(indCrseRch));
        for j = indCrseRch(index)
            
            % Check that the current point is farther along the insertion
            insDir = data.coarsePaths(itr-1).node(j).v;
            dist = dot(insDir, data.fineVol.coords.XYZ(:,i) - data.entry.p);
            if dist <= data.coarsePaths(itr-1).node(j).d
                continue;
            end
                                    
            % Check the voxel for reachability
            [ reachable, node, rho ] = ...
                isReachable(    data.coarsePaths(itr-1).node(j).p, ...
                                data.fineVol.coords.XYZ(:,i), ...
                                data.coarsePaths(itr-1).node(j).v, ...
                                data );
                                        
            % If it's reachable, save the node and matrix values...
            if ( reachable )
                data.finePaths(itr).node = node;
                data.fineVol.V(i) = itr;
                data.fineVol.R(i) = rho;
                % ...and stop checking this node
                break;
            end
            
        end
    end
end

% Increase the iteration counter
data.itr = data.itr + 1;

end

function [ reachable, node, rho ] = isReachable( a, b, v_i, data )

% First check if the input and output points are the same
if isequal(a,b)
    node = [];
    reachable = false;
    NarcPts = 0;
    rho = 999;
    return;
end

% Solve for the arc connecting insertion with each point
[ rho,~,pts,v_e ] = findCircle(a,b,v_i,data.darcpts);

% Check if b is inside the insertion sheath
if rho == 999 % indicates the line was seen as straight
    ab = b-a;
    angle = acos(dot(ab,v_i)./norm(ab)./norm(v_i));
    if angle == pi
        node = [];
        reachable = false;
        NarcPts = 0;
        return;
    end
end

% Check if arc is too tight
if (   rho < data.Rmin  )
    node = [];
    reachable = false;
    NarcPts = 0;
    return;
end

%% Check which voxels the arc is within

% % Rearrange the list of arc points
NarcPts = size(pts,2);
stlFlag = false;
    
    % Check if the arc passes thru liver mesh
    if (   any( ~inpolyhedron(data.mesh.lv,pts') ) )
        stlFlag = true;
    end
    
    if(~stlFlag)
        % Check if arc passes thru vessels
        inobs = zeros(NarcPts,length(data.mesh.obs));
        for k = 1:length(data.mesh.obs)
            inobs(:,k) = inpolyhedron(data.mesh.obs(k),pts');
        end
        if ( any(inobs(:)) )
            stlFlag = true;
        end
    end
    
    if stlFlag
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