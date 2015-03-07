function [ data ] = checkReachable( data )
%CHECKREACHABLE Check if target points are reachable
% author:   Troy Adebar
% date:     2/27/2015
%
% -------------------------------------------------------------------------
%% Get some initial values
% Determine what iteration we are on
itr = data.itr;

% Extract indices for groups of points
indRch = find( data.V(:) > 0 )';     % points already labeled reachable
index = randperm(numel(indRch));
indRch = indRch(index);
indUnk = find( data.V(:) == 0)'; % possible points not labeled reachable
tic
%% The first iteration
if itr == 1
    
    % Loop through all points within the liver
    for i = indUnk
        
        % Solve for the arc connecting insertion with each point
        a   =   data.entry.p;
        v_i =   data.entry.v;
        b   =   data.pts.coords.XYZ(i,:)';
        Ncp = 25;
        [ rho,~,pts,v_e ] = findCircle( a,b,v_i,Ncp );
        
        % Check if arc is too tight
        if (   rho < data.Rmin  )
            continue;
        end
        
        % Check if arc leaves liver or hits obstacles
        % First check what coarse blocks the arc hits
        arcPts = permute(pts,[1,3,2]);
        arcPts = repmat(arcPts,[1,data.coarsePts.N,1]);
        vecs = arcPts-data.coarsePts.coords;
        
        coarseInd = find( any( all( abs(vecs) < ...
            repmat(data.Del',[1,data.coarsePts.N,data.Narcpts]),1),3));
       
        % Next check each coarse block for collisons
        flag = false;
        for k = coarseInd
            arcPts = permute(pts,[1,3,2]);
            arcPts = repmat(arcPts,[1,data.badPts(k).N,1]);
            vecs = arcPts-data.badPts(k).coords;
            dist = sum(vecs.^2,1);
            if( any(dist(:) < ((data.del/2)^2)) )
                flag = true;
                break;
            end
        end
        if(flag)
            continue;
        end            
       
        % Otherwise record the path end node...
        dist = dot(data.entry.v, data.pts.coords.XYZ(i,:)' - data.entry.p);
        data.paths(itr).node(i).d = dist;
        data.paths(itr).node(i).p = b;
        data.paths(itr).node(i).v = v_e;
        
        % ... and record that the node is reachable
        data.V(i) = itr;
        
    end
    
end
fprintf( 'First iteration runtime is %.2f seconds...\n',toc );
tic
%% The second iteration
if itr == 2
    
    % Loop through all points
    for i = indUnk
        
        % Loop through all reachable nodes from the previous planning iteration
        for j = indRch
                       
            % Check that the current point is farther along the insertion
            dist = dot(data.entry.v, data.pts.coords.XYZ(i,:)' - data.entry.p);
            if dist <= data.paths(itr-1).node(j).d
                continue;
            end
            
            % Solve for the arc connecting a/v_i with b
            a   =   data.paths(itr-1).node(j).p;
            v_i =   data.paths(itr-1).node(j).v;
            b   =   data.pts.coords.XYZ(i,:)';
            Ncp = 25;
            [ rho,~,pts,v_e ] = findCircle( a,b,v_i,Ncp );
            
            % Check if arc is too tight
            if (   rho < data.Rmin  )
                continue;
            end
            
            % Check if arc leaves liver or hits obstacles
            % First check what coarse blocks the arc hits
            arcPts = permute(pts,[1,3,2]);
            arcPts = repmat(arcPts,[1,data.coarsePts.N,1]);
            vecs = arcPts-data.coarsePts.coords;
            
            coarseInd = find( any( all( abs(vecs) < ...
                repmat(data.Del',[1,data.coarsePts.N,data.Narcpts]),1),3));
            
            % Next check each coarse block for collisons
            flag = false;
            for k = coarseInd
                arcPts = permute(pts,[1,3,2]);
                arcPts = repmat(arcPts,[1,data.badPts(k).N,1]);
                vecs = arcPts-data.badPts(k).coords;
                dist = sum(vecs.^2,1);
                if( any(dist(:) < ((data.del/2)^2)) )
                    flag = true;
                    break;
                end
            end
            if(flag)
                continue;
            end
            
            % Otherwise record the path end node...
            dist = dot(data.entry.v, data.pts.coords.XYZ(i,:)' - data.entry.p);
            data.paths(itr).node(i).d = dist;
            data.paths(itr).node(i).p = b;
            data.paths(itr).node(i).v = v_e;
            
            % ... and record that the node is reachable... 
            data.V(i) = itr;
            
            % ... and stop this node
            break;
            
        end
    end
end
fprintf( 'Second iteration runtime is %.2f seconds...\n',toc );
data.itr = data.itr + 1;