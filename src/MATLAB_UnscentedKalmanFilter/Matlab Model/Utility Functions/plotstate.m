function [  ] = plotstate( T, hfig, vlength )
%PLOTSTATE plot a colored coordinate frame to indicate position and
%orientation based on the [x y z r p y] state. Plot on the figure with
%supplied handle hfig

% Extract position
p = T(1:3,4);

% Define axis colors
col = 'rgb';

% Access the figure
figure(hfig);
hold on;

% Loop through x,y,z axes
for i = 1:3
    v = [p p+vlength*T(1:3,i)];
    plot3(v(1,:),v(3,:),v(2,:),col(i),'LineWidth',2);
end
plot3(p(1),p(3),p(2),'ko','MarkerSize',5);

% Release the figure
hold off;

end

