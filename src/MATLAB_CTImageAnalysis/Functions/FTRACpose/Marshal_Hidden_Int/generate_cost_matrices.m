function cost = generate_cost_matrices(pts_2d, trs, ex, ey, xo, yo, focal, lambda)

% generate the cost matrices
cost = cell(3,1);
ind = cell(3,1);
[cost{1}, ind{1}] = generate_2img_cost_matrix(pts_2d(1:length(pts_2d)),trs,ex,ey,xo,yo,focal);
[cost{2}, ind{2}] = generate_2img_cost_matrix({pts_2d{2};pts_2d{3};pts_2d{1}},{trs{2};trs{3};trs{1}},[ex(2);ex(3);ex(1)],[ey(2);ey(3);ey(1)],[xo(2);xo(3);xo(1)],[yo(2);yo(3);yo(1)],[focal(2);focal(3);focal(1)]);
[cost{3}, ind{3}] = generate_2img_cost_matrix({pts_2d{3};pts_2d{1};pts_2d{2}},{trs{3};trs{1};trs{2}},[ex(3);ex(1);ex(2)],[ey(3);ey(1);ey(2)],[xo(3);xo(1);xo(2)],[yo(3);yo(1);yo(2)],[focal(3);focal(1);focal(2)]);

% % compute secondary cost matrix using variation
% cost_var = cell(3,1);
% for i = 1:3
%     indices = [];
%     indices(:,:,1) = repmat([1:size(cost{i}, 1)]', 1, size(cost{i}, 2)); 
%     indices(:,:,2) = repmat([1:size(cost{i}, 2)], size(cost{i}, 1), 1); 
%     indices(:,:,3) = ind{i};
% 
%     ind_jk = sub2ind(size(cost{mod(i,3) + 1}), indices(:,:,2), indices(:,:,3));
%     c_jk = cost{mod(i,3) + 1}(ind_jk);
% 
%     ind_ki = sub2ind(size(cost{mod(i + 1,3) + 1}), indices(:,:,3), indices(:,:,1));
%     c_ki = cost{mod(i + 1,3) + 1}(ind_ki);
% 
%     c_avg = (cost{i} + c_jk + c_ki) ./ 3;
%     cost_var{i} = abs(c_avg - cost{i}) + abs(c_avg - c_jk) + abs(c_avg - c_ki);
%     x = find(cost_var{i} < 0.00001);
%     cost_var{i}(x) = 0; 
%     x = find(cost_var{i} >= 0.00001);
%     cost_var{i}(x) = 1;
% end

% lambda = 1;
for i = 1:3
%     cost{i} = cost{i}.*cost_var{i};
    cost{i} = lambda * cost{i};% + (1 - lambda) * cost_var{i};
end