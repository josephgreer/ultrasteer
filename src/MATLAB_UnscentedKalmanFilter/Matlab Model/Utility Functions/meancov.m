function [mu,P,E] = meancov(X)
% Mean and covariance of a number of transform samples
%Inputs:
%       X:  matrix of transform samples (4 x 4 x k)
%Output:
%       mu: mean             (4 x 4)
%       P:  covariance       (6 x 6)
%       E:  displacement vec (6 x 1)

% Initialize
mu = eye(4,4);
k = size(X,3);

% Find position mean
mu(1:3,4) = mean(X(1:3,4,:),3);

% Find position deviations
for i = 1:k
    Ep(:,1,i) = X(1:3,4,i) - mu(1:3,4);
end

% Find orientation mean
Rm_ = X(1:3,1:3,1);
flag = true;
% Iteratively find equidistant orientation
while(flag)
    for i = 1:k
        Er(:,1,i) = mat2vec(X(1:3,1:3,i)*inv(Rm_));
    end
    Rm_old = Rm_;
    Rm_ = vec2mat((1/k).*sum(Er,3))*Rm_;
    % Break on convergence
    flag = norm(mat2vec(Rm_*inv(Rm_old))) > 1e-5;
end
mu(1:3,1:3) = Rm_;
for i = 1:k
    Er(:,1,i) = mat2vec(X(1:3,1:3,i)*inv(Rm_));
end

% Find combined covariance
P = zeros(6,6);
for i = 1:k
    E(:,:,i) = cat(1,Ep(:,1,i),Er(:,1,i));
    P = P + (1/k)*(E(:,:,i)*E(:,:,i)');
end
