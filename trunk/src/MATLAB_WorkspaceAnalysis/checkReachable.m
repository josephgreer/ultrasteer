function [ data ] = checkReachable( data )
%CHECKREACHABLE Check if target points are reachable
% author:   Troy Adebar
% date:     2/27/2015
%
% -------------------------------------------------------------------------
itr = data.itr;

%% The first iteration
if itr == 1
    
    % Initialize reachable vector
    data.reachable = zeros(data.pts.Npts,0);
    
    % Loop through all points
    for i = 1:data.pts.Npts
        
        % Solve for the arc connecting insertion with each point
        a   =   data.entry.p;
        v_i =   data.entry.v;
        b   =   data.pts.coords(i,:)';
        Ncp = 25;
        [ rho,~,pts,v_e ] = findCircle( a,b,v_i,Ncp );
        
        % Check obstacles
        if( any(strcmp('obs',fieldnames(data.mesh))) )
            % there are obstacle meshes
            inobs = zeros(Ncp,length(data.mesh.obs));
            % loop thru the meshes
            for k = 1:length(data.mesh.obs)
                inobs(:,k) = inpolyhedron(data.mesh.obs(k),pts');
            end
            hitsObs = any(inobs(:));
        else
            % there are no obstacle meshes
            hitsObs = false;
        end
        
        % Check if arc is too tight or leaves liver
        if (   rho < data.Rmin || ... % too tight
                any( ~inpolyhedron(data.mesh.lv,pts') ) || ... % leaves
                hitsObs )
        else
            % Record the path end node
            dist = dot(data.entry.v, data.pts.coords(i,:)' - data.entry.p);
            data.paths(itr).node(i).d = dist;
            data.paths(itr).node(i).p = b;
            data.paths(itr).node(i).v = v_e;
            
            % Record that the node is reachable
            data.reachable(i,1) = itr;
        end
    end
end


%% The second iteration
if itr == 2
    
    % Loop through all points
    for i = 1:data.pts.Npts
        
        % Check if the current point has already been identified as reachable
        if data.reachable(i,data.itr-1)
            break
        end
        
        % Loop through all reachable nodes from the previous planning iteration
        for j = find(data.reachable~=0)
            
            % Check that the current point is farther along the insertion
            dist = dot(data.entry.v, data.pts.coords(i,:)' - data.entry.p);
            if dist > data.paths(it).node(j).d
                
                % Solve for the arc connecting a/v_i with b
                a   =   data.paths(it).node(j).p;
                v_i =   data.paths(it).node(j).v;
                b   =   data.pts.coords(i,:)';
                Ncp = 25;
                [ rho,~,pts,v_e ] = findCircle( a,b,v_i,Ncp );
                
                % Check obstacles
                if( any(strcmp('obs',fieldnames(data.mesh))) )
                    % there are obstacle meshes
                    inobs = zeros(Ncp,length(data.mesh.obs));
                    % loop thru the meshes
                    for k = 1:length(data.mesh.obs)
                        inobs(:,k) = inpolyhedron(data.mesh.obs(k),pts');
                    end
                    hitsObs = any(inobs(:));
                else
                    % there are no obstacle meshes
                    hitsObs = false;
                end
                
                % Check if arc is too tight or leaves liver
                if (   rho < data.Rmin || ... % too tight
                        any( ~inpolyhedron(data.mesh.lv,pts') ) || ... % leaves
                        hitsObs )
                else
                    % Record the path end node
                    dist = dot(data.entry.v, data.pts.coords(i,:)' - data.entry.p);
                    data.paths(itr).node(i).d = dist;
                    data.paths(itr).node(i).p = b;
                    data.paths(itr).node(i).v = v_e;
                    
                    % Record that the node is reachable
                    data.reachable(i,1) = itr;
                end
            end
        end
    end
end

data.itr = data.itr + 1;
