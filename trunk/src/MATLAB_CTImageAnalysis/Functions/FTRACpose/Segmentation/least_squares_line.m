% $Id: least_squares_line.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

%Function to fit a least squares line to a set of input points in a 2D. A
%direction of the line is first sought using PCA analysis. Depending on the
%orientation of the line, either y = mx + c is used...or x=my+c is used.
%The pi/4 mark is used as a decider. for each of these cases, the
%parameters are computed using the typical least square method.
function [line] = least_squares_line(points)

%PCA analysis to get approximate direction.
pts(:,1) = points(:,1) - mean(points(:,1));
pts(:,2) = points(:,2) - mean(points(:,2));
PCA = [ sum(pts(:,1).*pts(:,1)) sum(pts(:,1).*pts(:,2)) ; sum(pts(:,1).*pts(:,2)) sum(pts(:,2).*pts(:,2))];
[V D] = eig(PCA);
[tmp, ind] = max(abs(diag(D)));
V = V(:,ind);

%Depending on the case, compute the line parameters.
if(abs(V(1)) >= abs(V(2)) )
    A = [points(:,1) ones(size(points,1),1)];
    x = inv(A'*A)*(A'*points(:,2));
    line(1) = x(1);
    line(2) = -1;
    line(3) = x(2);
    if( isnan(line(1)) )
        display 'What the?'
    end
    if(line(3)>0)
        line = -line;
    end
    line = line/norm(line(1:2));
else
    A = [points(:,2) ones(size(points,1),1)];
    x = inv(A'*A)*(A'*points(:,1));
    line(1) = -1;
    line(2) = x(1);
    line(3) = x(2);
    if( isnan(line(1)) )
        display 'What the?'
    end
    if(line(3)>0)
        line = -line;
    end
    line = line/norm(line(1:2));
end

%If the sum of errors(diatance from line) for all points is greater than
%20, then there may be a problem in the least square fitting. Check for
%possible errors.
A = line(1);
B = line(2);
C = line(3);

E = A*points(:,1) + B*points(:,2) + C;
if(sum(abs(E)>20)~=0)
    points
    line
    E
    t = inv(points'*points)*(points'*(-1*ones(10,1)))
    [t' 1]/norm(t)
    return
end
    
    
% $Log: least_squares_line.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%