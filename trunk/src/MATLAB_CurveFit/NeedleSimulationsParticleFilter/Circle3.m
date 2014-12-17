%% Represents a circle in 3D through three points
classdef Circle3
    properties
        c % center
        r % radius
        N % plane normal
        bx % basis vector 1 of plane
        by % basis vector 2 of plane
    end
    methods
        % construct a circle going through A = [a1 a2 a3]
        function obj = Circle3(A)
            a1 = A(:,1);  a2 = A(:,2); a3 = A(:,3);
            a = a1-a3;
            b = a2-a3;
            amb = a-b;
            
            asq = a'*a;
            bsq = b'*b;
            ambmag = sqrt(amb'*amb);
            amag = sqrt(asq);
            bmag = sqrt(bsq);
            
            axb = cross(a,b);
            axbsq = axb'*axb;
            axbmag = sqrt(axbsq);
            
            obj.c = cross((asq*b-bsq*a),axb)/(2*axbsq)+a3;
            obj.r = (((amag*bmag)*ambmag)/(axbmag))/2;
            obj.N = axb/axbmag;
            
            obj.bx = a1-obj.c;
            obj.by = cross(obj.bx,obj.N);
        end
        
        function draw(a, f)
            figure(f);
            
            % maps homogenous 2d vectors 2 circle plane
            map = [a.bx a.by a.c];
            
            theta = [0:2*pi/100:2*pi];
            
            pts = map*[cos(theta); sin(theta); ones(size(theta))];
            plot3(pts(1,:), pts(2,:), pts(3,:));
        end
    end
end