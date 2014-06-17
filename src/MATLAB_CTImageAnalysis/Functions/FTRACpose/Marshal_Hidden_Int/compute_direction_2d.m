function direction = compute_direction_2d(data)
% Compute direction using PCA

% adjust data
x_mean = mean(data(:,1));
y_mean = mean(data(:,2));
data_adjusted = [data(:,1) - x_mean data(:,2) - y_mean];

% calculate covariance matrix, eigenvalues, and eigenvectors
cov_mat = cov(data_adjusted);
[eig_vec eig_val] = eig(cov_mat);

% compute resulting direction
[v, ind] = max([eig_val(1,1) eig_val(2,2)]);
direction = eig_vec(:,ind);