function [  ] = plotarc( p0, p1, p2, hfig)
%PLOTARC plot a dashed arc centered on p0 and going between p1 and p2. All
%inputs should be column vectors in R^3. Plots on the figure with handle
%hfig.

% Do vector calculations
 x0 = p0(1); y0 = p0(2); z0 = p0(3);
 x1 = p1(1); y1 = p1(2); z1 = p1(3);
 x2 = p2(1); y2 = p2(2); z2 = p2(3);
 v1 = [x1-x0;y1-y0;z1-z0]; % Vector from center to 1st point
 r = norm(v1); % The radius
 v2 = [x2-x0;y2-y0;z2-z0]; % Vector from center to 2nd point
 v3 = cross(cross(v1,v2),v1); % v3 lies in plane of v1 & v2 and is orthog. to v1
 v3 = r*v3/norm(v3); % Make v3 of length r
 % Let t range through the inner angle between v1 and v2
 t = linspace(0,atan2(norm(cross(v1,v2)),dot(v1,v2))); 
 v = v1*cos(t)+v3*sin(t); % v traces great circle path, relative to center

% Access the figure
figure(hfig);
hold on;
% The plot of the circular arc from p1 to p2
plot3(v(1,:)+x0,v(3,:)+z0,v(2,:)+y0,'k--','LineWidth',0.1) % Plot it in 3D
hold on;

l = 5;
n = [p1 p1+l.*(p0-p1)./norm(p0-p1)];
plot3(n(1,:),n(3,:),n(2,:),'r--','LineWidth',0.5);
plot3([x0 x1],[z0 z1],[y0 y1],'r-','LineWidth',0.1);
plot3([x0 x2],[z0 z2],[y0 y2],'r-','LineWidth',0.1);
 % Release the figure
hold off;
end

