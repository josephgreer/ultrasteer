function drawVector (pt, origin, linspec) 
% draws a vector from the orginin to the point


if (size(linspec,2)== size('real',2))
    if (linspec == 'real')
        plot3 ([pt(1),origin(1)],[pt(2),origin(2)],[pt(3),origin(3)],'-k','LineWidth',2);
    end
else
plot3 ([pt(1),origin(1)],[pt(2),origin(2)],[pt(3),origin(3)],linspec);
end
