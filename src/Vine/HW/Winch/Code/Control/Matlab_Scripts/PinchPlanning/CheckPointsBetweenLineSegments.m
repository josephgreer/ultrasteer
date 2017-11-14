% points = [p1_x p_1y;...
%           ...
%           pn_x pn_y]; \in R^{nx2}
% segments = [e11_x e11_y e12_x e12_y;...
%             ...
%             en1_x en1_y en2_x en2_y]; \in R^{nx4}
% values(i) is true if point p_i is on line segment defined by endpoints
% ei1,ei2
function values = CheckPointsBetweenLineSegments(points,segments)
% check if dist(P,E1)+dist(P,E2) = dist(E1,E2)
PE1 = points-segments(:,1:2);
PE2 = points-segments(:,3:4);
E1E2 = segments(:,1:2)-segments(:,3:4);

PE1 = sqrt(sum(PE1.^2,2));
PE2 = sqrt(sum(PE2.^2,2));
E1E2 = sqrt(sum(E1E2.^2,2));

dists = abs((PE1+PE2-E1E2))./E1E2;
values = dists<1e-4;
end
