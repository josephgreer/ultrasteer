%% Derivative of (p1, p2, p3)
%      poly = [a11 a12 a13;
%             a21 a22 a23;
%             ...
%             ad1 ad2 ad3];
%  where (a11^(d-1)+...+ad1, a12^(d-1)+...+ad2, a13^(d-1)+...+ad3)
function ds = polyder3(poly)
ds = [polyder(poly(:,1)) polyder(poly(:,2)) polyder(poly(:,3))];
end