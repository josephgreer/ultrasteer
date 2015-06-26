%% Evaluate polynomial poly = (p1(s), p2(s), p3(s))
%      poly = [a11 a12 a13;
%             a21 a22 a23;
%             ...
%             ad1 ad2 ad3];
%  where (a11^(d-1)+...+ad1, a12^(d-1)+...+ad2, a13^(d-1)+...+ad3)
%  return value = [p1(s) p2(s) p3s()]
function val = polyval3(poly,s)
val = [polyval(poly(:,1), s) polyval(poly(:,2),s) polyval(poly(:,3),s)];
end