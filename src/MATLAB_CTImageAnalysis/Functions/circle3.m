function circle3(P,N,R)
% circle3(P,N,R)
% draw circle in R^3 where
%    P: center
%    N: normal vector for plane of circle
%    R: radius
% P,N,R are ROW vectors
%
%Example: circle with center [1,2,3], radius 5, plane with normal [2,-1,3]:
%  P=[1,2,3]; N=[2,-1,3];
%  circle3(P,N,5); hold on; arrow3(P,N); hold off; nice3d

tv = linspace(0,2*pi,100)';

if R > 500
    tv = linspace(-pi/2,pi/2,100)';
end

[s,j] = min(abs(N));
u = zeros(1,3); u(j) = 1;  % u is nonzero vector not parallel to N
v = cross(N,u);            % v is perpendicular on N
w = cross(N,v);            % w is perpendicular on N,v

rv = R*(cos(tv)*v/norm(v) + sin(tv)*w/norm(w));

plot3(P(1)+rv(:,1),P(2)+rv(:,2),P(3)+rv(:,3))
