% computes the distance between two orientations using Riemaniannian
% metric on SO(3)
function d = SO3Metric(R1,R2)
d = 1/sqrt(2)*norm(SO3Log(R1'*R2),'fro');
end