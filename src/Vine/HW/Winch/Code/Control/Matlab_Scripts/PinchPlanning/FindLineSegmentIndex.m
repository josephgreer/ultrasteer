% wall end point 1 = [p11; p12; ... p1n] \in R^{nx2}
% wall end point 2 = [p21; p22; ... p2n] \in R^{nx2}
% point \in R^2
function idx = FindLineSegmentIndex(P1,P2,P)
n = size(P1,1);
Ps = repmat(P,n,1);
dotDivisor = dot(Ps-P1,P2-P1,2) ./ dot(P2-P1,P2-P1,2);
PP = P1 +  repmat(dotDivisor, 1, 2) .* (P2-P1);
deltas = Ps-PP;
dists = sqrt(sum(deltas.^2,2));

values = CheckPointsBetweenLineSegments(repmat(P,size(P1,1),1),[P1 P2]);
dists(~values) = 1e6;

idx = find(dists < 1e-3);
end