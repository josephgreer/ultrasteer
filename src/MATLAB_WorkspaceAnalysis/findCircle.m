function [ rho,c,pts,v_e ] = findCircle( a,b,v_i,d )
%FINDCIRCLE Summary of this function goes here
% [ rho,c,pts ] = findCircle( a,b,v )
% Inputs:   a:          point (3x1) on the circle
%           b:          point (3x1) on the circle
%           v_i:        tangent vector (3x1) at a
%           d:          linear spacing between points on arc [a,b]
% Outputs:  rho:        radius of the circle
%           c:          center point (3x1) of the circle
%           pts:        output pts (3xN) on arc [a,b]
%           v_e:        tangent vector (3x1) at b
% 
% author:   Troy Adebar
% date:     2/24/2015
% -------------------------------------------------------------------------
%% First check to see if point b happens to lie exactly along v_i from a
ab = b-a;
angle = acos(dot(ab,v_i)./norm(ab)./norm(v_i));

if (angle == pi) || (angle == 0)
    u = v_i./norm(v_i);
    smax = norm(ab);
    s = 0:d:smax;
    Ns = length(s);
    pts = repmat(a,1,Ns) + repmat(u,1,Ns).*repmat(s,3,1);
    if Ns < 3
        clear pts;
        for i = 1:3
            pts(i,:) = linspace(a(i),b(i),3);
        end
    end
    if Ns > 100
        clear pts;
        for i = 1:3
            pts(i,:) = linspace(a(i),b(i),100);
        end
    end
    v_e = u;
    c = [inf inf inf]';
    rho = 777;
    return;
end
    
    %% Define unit vectors
% define "y-axis" unit vector in the direction of v
u_2 = v_i./norm(v_i);
% define "x-axis" unit vector using projection of b-a
v_b = b-a;
v_bp = v_b-dot(v_b,u_2).*u_2;
u_1 = v_bp./norm(v_bp);
% define "z-axis" unit vector
u_3 = cross(u_1,u_2);


%% Project vector onto unit vectors
x = dot(u_1,v_b);
y = dot(u_2,v_b);


%% Solve for radius and center
rho = ( x^2 + y^2 )/2/abs(x);
c = a + rho.*u_1;


%% Find arc angles at a and b
theta_a = pi;                           % angle at a; by construction
% R = inv([u_1 u_2 u_3]);               % define basis transform
h = ([u_1 u_2 u_3])\(b-c);              % convert vector to u_1,u_2 basis
theta_b = atan2(h(2)/rho,h(1)/rho);     % find angle using ratio


%% Find points between a and b with linear spacing d

delTheta = abs(asin(d/2/rho)*2);
if theta_a > theta_b
    theta = theta_b:delTheta:theta_a;
else
    theta = theta_b:-delTheta:theta_a;
end

Npts = numel(theta);

if(Npts < 3)
    theta = linspace(theta_b,theta_a,3);
    Npts = 3;
end

if(Npts > 100)
    theta = linspace(theta_b,theta_a,100);
    Npts = 100;
end

% for th = theta
%     pts = [pts c + rho*u_1*cos(th) + rho*u_2*sin(th)];
% end

mTheta = repmat(theta,3,1);
mU_1 = repmat(u_1,1,Npts);
mU_2 = repmat(u_2,1,Npts);

if ~isequal(size(mU_1), size(mTheta))
    display('poop');
end

pts = repmat(c,size(theta)) + rho.*mU_1.*cos(mTheta) + rho.*mU_2.*sin(mTheta);

%% Find tangent vector at end point
r = [ u_3' theta_b - theta_a ];
R = vrrotvec2mat(r);
v_e = R*v_i;

end

