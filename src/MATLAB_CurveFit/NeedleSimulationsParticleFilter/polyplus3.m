%% Evaluate polynomial poly = (p1(s), p2(s), p3(s))
%      poly = [a11 a12 a13;
%             a21 a22 a23;
%             ...
%             ad1 ad2 ad3];
%  where (a11^(d-1)+...+ad1, a12^(d-1)+...+ad2, a13^(d-1)+...+ad3)
%  return value = [p1(s) p2(s) p3s()]
function res = polyplus3(polyA,polyB)
m = size(polyA,1);
n = size(polyB,1);
N = max(size(polyA,1),size(polyB,1));
polyA = [zeros(N-m,3); polyA];  polyB = [zeros(N-n,3); polyB];
res = polyA+polyB;
end