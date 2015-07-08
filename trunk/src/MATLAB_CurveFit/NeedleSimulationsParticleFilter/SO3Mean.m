% computes the Karcher mean of a set of elements in SO(3)
% uses A GLOBALLY CONVERGENT NUMERICAL ALGORITHM FOR COMPUTING THE
% CENTRE OF MASS ON COMPACT LIE GROU
% q(1,:) = [qx qy qz qw]
% ...
% q(n,:) = [qx qy qz qw]
function X = SO3Mean(q, w, eps)
R = {};
k = size(q,1);
for i=1:k
    R{i} = QuatToRotationMatrix(q(i,:)');
end
error = eps+1;

X = R{1};
Xinv = inv(X);
jj = 0;
while error > eps && jj<100
    A = zeros(3,3);
    for i=1:k
        A = A+w(i)*SO3Log(Xinv*R{i});
    end
    X = X*SO3Exp(A);
    Xinv = inv(X);
    error = norm(A);
    jj = jj+1;
end
end