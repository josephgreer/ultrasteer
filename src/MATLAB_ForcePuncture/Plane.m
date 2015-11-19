%% Represents a circle in 3D through three points
classdef Plane
    properties
        abcd
    end
    methods
        % abcd \in R^{4x1}
        % construct a plane defined abcd^T*[x; 1] = 0
        function obj = Plane(abcd)
            abcd = abcd/norm(abcd(1:3));
            obj.abcd = abcd;
        end
        
        % p is plane
        % pt \in R^{3x1}
        % return signed distance to plane
        function d = pointDistance(p, pt)
            abcd = p.abcd/norm(p.abcd(1:3));
            d = dot(abcd(1:3), pt)+abcd(4);
        end
        
        % p is plane
        % pt \in R^{3x1}
        function ppt = projectPointOntoPlane(p, pt)
            center = [0; 0; -p.abcd(4)/p.abcd(3)];
            n = p.abcd(1:3);
            n = n/norm(n);
            
            dist = dot(pt-center,n);
            ppt = pt-dist*n;
        end
        
        % a is plane
        % corner \in R^{3x1} 
        % extent \in R^{2x1} represents width and height of plane
        % f represents figure number
        function draw(p, corner, extent, f)
            figure(f);
            
            % choose two random directions for drawing
            p1 = [0; 0; 1];
            corner = projectPointOntoPlane(p, corner);
            p1 = projectPointOntoPlane(p, p1);
            
            axis1 = (p1-corner);
            axis1 = axis1/norm(axis1);
            axis2 = cross(axis1, p.abcd(1:3));
            axis2 = axis2/norm(axis2);
            
            p1 = corner;
            p2 = corner+axis1*extent(1);
            p3 = corner+axis2*extent(2);
            p4 = corner+axis1*extent(1)+axis2*extent(2);
            
            fillMatrixA = [p1'; p2'; p3'];
            fillMatrixB = [p2'; p3'; p4'];
            
            dp = fill3([fillMatrixA(:,1) fillMatrixB(:,1)], [fillMatrixA(:,2) fillMatrixB(:,2)],...
                [fillMatrixA(:,3) fillMatrixB(:,3)],'r');
            grid on;
            daspect([1 1 1]);
            alpha(dp, 0.5);
        end
    end
end