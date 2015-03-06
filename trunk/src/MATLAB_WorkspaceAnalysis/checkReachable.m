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
        
        % Check if arc leaves the liver
        if (   any( ~inpolyhedron(data.mesh.lv,pts') ) )
            continue;
        end
        
        % Check if arc passes thru obstacles
        inobs = zeros(Ncp,length(data.mesh.obs));
        for k = 1:length(data.mesh.obs)
            inobs(:,k) = inpolyhedron(data.mesh.obs(k),pts');
        end
        if ( any(inobs(:)) )
            continue
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

%% The second iteration
if itr == 2
    
    % Loop through all points
    for i = indUnk
        
        % Loop through all reachable nodes from the previous planning iteration
        for j = intersect( data.closer{i}, indRch )
            
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
            
            % Check if arc leaves the liver
            if (   any( ~inpolyhedron(data.mesh.lv,pts') ) )
                continue;
            end
            
            % Check if arc passes thru obstacles
            inobs = zeros(Ncp,length(data.mesh.obs));
            for k = 1:length(data.mesh.obs)
                inobs(:,k) = inpolyhedron(data.mesh.obs(k),pts');
            end
            if ( any(inobs(:)) )
                continue
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

data.itr = data.itr + 1;
toc